#include "MOTOR.h"

MOTOR::MOTOR() {
  pin1 = -1;
  pin2 = -1;
}

MOTOR::MOTOR(char a, char b) {
  setPin1(a);
  setPin2(b);
}

MOTOR::~MOTOR() {

}

void MOTOR::setPin1(char a) {
  pin1 = a;
  pinMode(a, INPUT);
}

void MOTOR::setPin2(char a) {
  pin2 = a;
  pinMode(a, INPUT);
}

void MOTOR::mF(int par) {
  if (pin1 == -1 || pin2 == -1) return;
  par = constrain(par, -255, 255);
  analogWrite(pin1, par);
  digitalWrite(pin2, LOW);
}

void MOTOR::mB(int par) {
  if (pin1 == -1 || pin2 == -1) return;
  par = constrain(par, -255, 255);
  analogWrite(pin2, par);
  digitalWrite(pin1, LOW);
}

void MOTOR::stop() {
  if (pin1 == -1 || pin2 == -1) return;
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
}

void MOTOR::m(int par) {
  par = constrain(par, -255, 255);
  if (par > 0) mF(par);
  else if (par < 0) mB(abs(par));
  else stop();
}
