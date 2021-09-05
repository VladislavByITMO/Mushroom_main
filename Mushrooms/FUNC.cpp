#include "FUNC.h"

void interruptRF() {
  interruptFlag = 1;
  interruptTimer = millis();
}

void interruptLF() {
  interruptFlag = 2;
  interruptTimer = millis();
}

void interruptRB() {
  interruptFlag = 3;
  interruptTimer = millis();
}

void interruptLB() {
  interruptFlag = 4;
  interruptTimer = millis();
}

void onBurner() {
  gpio.servoWrite(SERVO_FLAP, ANGLE_FLAP_1); //открыть заслонку
  delay(400);

  digitalWrite(BURNER_IN1, HIGH);
  delay(100);
  for (int i = 0; i < 3; i++) {
    digitalWrite(BURNER_IN2, HIGH);
    delay(500);
    digitalWrite(BURNER_IN2, LOW);
    delay(500);
  }
  digitalWrite(BURNER_IN1, LOW);
  digitalWrite(BURNER_IN2, LOW);
  delay(200);
}

void offBurner() {
  digitalWrite(BURNER_IN1, LOW);
  digitalWrite(BURNER_IN2, LOW);
  gpio.servoWrite(SERVO_FLAP, ANGLE_FLAP_2); //закрыть заслонку
}

void changeColor(unsigned char r, unsigned char g, unsigned char b) {
  backlight.clear();
  backlight.setPixelColor(0, backlight.Color(r, g, b));
  backlight.show();
}
