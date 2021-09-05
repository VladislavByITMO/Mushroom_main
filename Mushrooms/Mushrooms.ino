#include "INCLUDE.h"
#include "DEFINE.h"

#include "TRUCK.h"
#include "MOTOR.h"
#include "FUNC.h"

static Adafruit_MLX90614 mlx = Adafruit_MLX90614();
static Adafruit_INA219 ina219(0x40);
static Adafruit_NeoPixel backlight(1, PIN_WS2812, NEO_GRB + NEO_KHZ800);

static iarduino_I2C_Expander gpio(0x09);
static iarduino_Position_BMX055 gyro(BMX);
static NewPing sonarR(TrigR, EchoR, 400);
static NewPing sonarL(TrigL, EchoL, 400);

static TRUCK truck(5, 4, 6, 7, 9, 8, 10, 11);
static MOTOR pump(44, 45);

static GButton BUTTON(PIN_BUTTON);

enum class States {
  goodK,
  passiveGoodK,
  lowK,
  manyK,
  stopping
};

static char interruptFlag = 0;       //переменная для обработки прерывания
static States state = States::goodK; //режим работы

int vel, wel;   //линейная и угловая скорость
int rvel, rwel; //целевые случайные линейная и угловая скорости

//набор таймеров
uint64_t loopTimer = 0, moveUpdateTimer = 0; //таймеры Арсения
uint64_t stoppingTimer = 0, ledUpdateTimer = 0, interruptTimer = 0; //таймеры Руслана
uint64_t moveRoundTimer = 0, moveTimer = 0; //таймеры Влада

//переменные Влада
boolean flag_Timer = false, diod_Flag = true;

int flag_manyK = 0, flag_lowK = 0, SR = 0, SL = 0;

//перменные Руслана
float curBatteryV = -1., lastBatteryV = -1.; //текущее и последнее значение на батарее
bool FLAG_LED_BUTTON = 0;                    //моргание светодиодом
byte data[4]; //сообщение радиомодуля

void goodK();
void passiveGoodK();
void lowK();
void manyK();

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A15));

  attachInterrupt(2, interruptLB, RISING); //настройка прерывания
  attachInterrupt(3, interruptRB, RISING);
  attachInterrupt(18, interruptLF, RISING);
  attachInterrupt(19, interruptRF, RISING);

  pinMode(COL, OUTPUT); //настройка пинов
  pinMode(IKR, INPUT);
  pinMode(IKL, INPUT);
  pinMode(IKB, INPUT);
  pinMode(LED_BUTTON, OUTPUT);
  pinMode(BURNER_IN1, OUTPUT);
  pinMode(BURNER_IN2, OUTPUT);

  gyro.begin();      //гироскоп
  mlx.begin();       //инфакрасный термометр
  ina219.begin();    //датчик тока\напряжения
  backlight.begin(); //Ws2812

  gpio.begin(); //настройка i2c расширителя
  gpio.changeAddress(0x09);
  gpio.pinMode(SERVO_MAST, OUTPUT, SERVO);
  gpio.pinMode(SERVO_FLAP, OUTPUT, SERVO);
  gpio.pinMode(MQ_135, INPUT, ANALOG);
  gpio.pinMode(CJMCU_FRONT, INPUT, ANALOG);
  gpio.pinMode(CJMCU_TANK, INPUT, ANALOG);
  gpio.pinMode(PH, INPUT, ANALOG);

  Mirf.spi = &MirfHardwareSpi; //настройка радиомодуля
  Mirf.init();
  Mirf.setRADDR((byte*)"mushR");
  Mirf.payload = 4;
  Mirf.config();

  truck.stop();
  pump.stop();

  while (!BUTTON.isSingle()) BUTTON.tick(); //ожидание нажатия на кнопку для старта
}

void loop() {
  if (millis() - loopTimer > LOOP_UPDATE_RATE) {
    loopTimer = millis();

    vel = 0;
    wel = 0;

    if (!Mirf.isSending() && Mirf.dataReady()) { //проверка радио, сообщение: 4 байта - буква, число, число, число
      int number = 0;
      byte *temp = data + 1;
      data[0] = data[1] = data[2] = data[3] = 0;
      Mirf.getData(data);

      while (*temp) {
        number += *temp - '0';
        number *= 10;
        temp++;
      }
      number /= 10;

      switch (data[0]) { //реагирование на сообщения
        case 'S':
        case 's': //остановка всех роботов
          vel = 0;
          wel = 0;
          state = States::stopping;
          stoppingTimer = millis();
          break;
      }
    }

    curBatteryV = ina219.getBusVoltage_V();
    if (curBatteryV  > 6.6 && curBatteryV < 7) { //батарея садиться
      if (millis() - ledUpdateTimer  > MOVE_UPDATE_RATE) {
        ledUpdateTimer = millis();
        digitalWrite(LED_BUTTON, FLAG_LED_BUTTON = !FLAG_LED_BUTTON); //моргание светодиодом на кнопке
      }
    } else if (curBatteryV - 6.6 < 0.0001) { //батарея села
      if (lastBatteryV - curBatteryV < 0.0001) {
        lastBatteryV == curBatteryV;
        onBurner(); //включаем гарелку
        truck.stop(); //останавливаемся
        //еще что-то выключаем если надо
        digitalWrite(LED_BUTTON, LOW);
        while (!BUTTON.isSingle()) BUTTON.tick(); //ждем пока к нам подойдут и нажмут на кнопку
        offBurner(); //выключаем гарелку
        while (1); //остаемся здесь пока не выключат тумблером робота
      }
    } else { //аккум в порядке
      if (lastBatteryV - curBatteryV < 0.0001) {
        lastBatteryV == curBatteryV;
        offBurner();
      }
    }

    if (interruptFlag) { //прерывание
      if (millis() - interruptTimer < INTERRUPT_MOVE_TIME)
        interruptFlag = 0;
      switch (interruptFlag) { //потом исправить на осознанное "убегание" от линии !!!!!
        case 1: //засек правый передний датчик
          vel = -255;
          wel = 127;
          break;
        case 2: //засек левый передний датчик
          vel = -255;
          wel = -127;
          break;
        case 3: //засек правый задний датчик
          vel = 255;
          wel = 127;
          break;
        case 4: //засек правый задний датчик
          vel = 255;
          wel = -127;
          break;
      }
    } else {
      switch (state) {
        case States::goodK:
          changeColor(255, 127, 39); //оранжевый
          goodK();
          break;
        case States::passiveGoodK:
          changeColor(255, 242, 0); //желтый
          passiveGoodK();
          break;
        case States::lowK:
          changeColor(255, 0, 0); //красный
          switch (flag_lowK) {
            case 0:        //Поиск вращением робота( ну или стенки, как повезет ) лучше поставить лазерный датчик, чтобы стенку не видеть при таком большом расстоянии
              if (SR < 50 || SL < 50) {
                flag_lowK = 2;
              } else {
                vel = 30;
                wel = -100;  //вращаемся пока не найдем что либо ближе чем 50см
              }
              break;
            case 1:      //пока не доделала логика
              //              if (flag_Timer) {
              //                moveRoundTimer = millis();
              //                flag_Timer = false;
              //              }
              //              if (millis() - moveRoundTimer > 1000) { //движение вокруг себя на 180
              //                flag_Timer = true;
              //                flag_lowK = 2;
              //              } else {
              //                vel = 0;
              //                wel = 150;
              //              }
              break;
            case 2:

              if (flag_Timer) {
                moveTimer = millis();
                flag_Timer = true;
              }
              if (millis() - moveTimer > 3000) { //движение назад от опасности 3 секунды
                flag_Timer = !flag_Timer;
                flag_lowK = 0;
              } else {
                vel = -150;
                wel = 0;
              }

              break;


              break;
          }

          break;
        case States::manyK:
          changeColor(0, 0, 255); //синий
          switch (flag_manyK) {
            case 0: //Поиск вращением робота( ну или стенки, как повезет ) лучше поставить лазерный датчик, чтобы стенку не видеть при таком большом расстоянии
              if (SR < 50 || SL < 50) {
                flag_manyK = 1;
              } else if (SR < 20 || SL < 20) {
                data[0] = 's';  //команда по радио об остановке (в теории)
                data[1] = data[2] = data[3] = 0;
                Mirf.send(data);
                
                if (flag_Timer) {
                  moveRoundTimer = millis();
                  flag_Timer = false;
                }
                if (millis() - moveRoundTimer > 10000) { //Пауза 10 секунд типа стыковка, тут можно продумать как то switch для стыковки
                  flag_Timer = true;
                  break;
                } else {
                  if (diod_Flag) {   // индикация RGB диодом того же цвета что происходит стыковка
                    changeColor(0, 0, 150);
                    diod_Flag = !diod_Flag;
                  } else {
                    changeColor(0, 0, 255);
                    diod_Flag = !diod_Flag;
                  }
                  vel = 0;
                  wel = 0;
                }

              } else {
                vel = 0;
                wel = -100; //вращаемся пока не найдем что либо ближе чем 50см
              }
              break;
            case 1: //разворот на 180
              if (flag_Timer) {
                moveRoundTimer = millis();
                flag_Timer = false;
              }
              if (millis() - moveRoundTimer > 5000) { //движение вокруг себя на 180
                flag_Timer = true;
                flag_manyK = 2;
              } else {
                vel = 0;
                wel = 150;
              }
              break;
            case 2:

              if (flag_Timer) {
                moveTimer = millis();
                flag_Timer = true;
              }
              if (millis() - moveTimer > 3000) { //движение вперед 3 секунды
                flag_Timer = !flag_Timer;
                flag_manyK = 0;
              } else {
                vel = 0;
                wel = 150;
              }
              break;
              break;
          }
          break;
        case States::stopping:
          changeColor(0, 0, 0);
          if (millis() - stoppingTimer < TIME_STOPPING) { //стоим на месте TIME_STOPPING (нас кто-то нашел и послал команду через радио)
            stoppingTimer = millis();
            state = States::goodK; //исправить на переключение по генератору режимов по данным с датчиков !!!!!!!!!!!!!!!!
          }
          break;
      }
    }

    truck.move(vel, 0, wel);
  }
}

void goodK() {
  static double convergeRate = 0.7;

  if (millis() - moveUpdateTimer > MOVE_UPDATE_RATE) {
    moveUpdateTimer = millis();

    rvel = 100 + (rand() % 155);
    rwel = (rand() % 510) - 255;
  }

  vel += (rvel - vel) * convergeRate;
  wel += (rwel - wel) * convergeRate;
}

void passiveGoodK() {
  static double scaleK = 0.6;
  goodK();
  vel *= scaleK;
  wel *= scaleK;
}

void lowK() {

}

void manyK() {

}
