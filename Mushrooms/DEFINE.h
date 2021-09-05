#ifndef DEFINE_H
#define DEFINE_H

#define PIN_WS2812 12 //адресная светодиодная лента
#define COL 13 		    //куллер на всасывание

#define IKR 22 //ИК датчик объекта справо
#define IKL 23 //ИК датчик объекта слево
#define IKB 24 //ИК датчик объекта сзади

#define PIN_BUTTON 28 //кнопка
#define LED_BUTTON 29 //светодиод кнопки

#define BURNER_IN1 32 //один пин для зажега горелки 
#define BURNER_IN2 33 //второй пин для зажега горелки

#define TrigR 40 //trig правого сонара
#define EchoR 41 //echo правого сонара
#define TrigL 42 //trig левого сонара
#define EchoL 43 //echo левого сонара

#define MAX_DISTANCE 400 //макс. дистанция работы сонаров

#define BMX055_DISABLE_BMA //доп. настройка гироскопа
#define BMX055_DISABLE_BMM //доп. настройка гироскопа

#define SERVO_MAST 0     //серво мачты
#define ANGLE_MAST_1 999 //поднятое положение матчы 
#define ANGLE_MAST_2 999 //опущенное положение матчы

#define SERVO_FLAP 1     //серво заслонки кислорода
#define ANGLE_FLAP_1 999 //заслонка открыта 
#define ANGLE_FLAP_2 999 //заслонка закрыта

#define MQ_135 4      //MQ-135 в камере 
#define CJMCU_FRONT 5 //CJMCU-1100 в камере 
#define CJMCU_TANK 6  //CJMCU-1100 в баке 
#define PH 7          //PH датчик в баке

// ms
#define LOOP_UPDATE_RATE 10      // время между обновлениями
#define MOVE_UPDATE_RATE 1000    // время между рандомными перемещениями
#define INTERRUPT_MOVE_TIME 2000 //время выделенное на убегания от линии после срабатывания прерывания
#define TIME_STOPPING 60000      //робот стоит и не двигается после получения сообщения

#endif // DEFINE_H
