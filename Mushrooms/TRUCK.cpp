#include "TRUCK.h"

TRUCK::TRUCK() {
  motorLF.setPin1(-1);
  motorLF.setPin2(-1);
  motorRF.setPin1(-1);
  motorRF.setPin2(-1);
  motorLB.setPin1(-1);
  motorLB.setPin2(-1);
  motorRB.setPin1(-1);
  motorRB.setPin2(-1);
}

TRUCK::TRUCK(char a, char b, char c, char d, char e, char f, char g, char h) {
  motorLF.setPin1(a);
  motorLF.setPin2(b);
  motorRF.setPin1(c);
  motorRF.setPin2(d);
  motorLB.setPin1(e);
  motorLB.setPin2(f);
  motorRB.setPin1(g);
  motorRB.setPin2(h);
}

TRUCK::~TRUCK() {

}

void TRUCK::mF(int a) {
  motorRF.mF(a);
  motorRB.mF(a);
  motorLF.mF(a);
  motorLB.mF(a);
}

void TRUCK::mB(int a) {
  motorRF.mB(a);
  motorRB.mB(a);
  motorLF.mB(a);
  motorLB.mB(a);
}

void TRUCK::rL(int a) {
  motorRF.mF(a);
  motorRB.mF(a);
  motorLF.mB(a);
  motorLB.mB(a);
}

void TRUCK::rR(int a) {
  motorRF.mB(a);
  motorRB.mB(a);
  motorLF.mF(a);
  motorLB.mF(a);
}

void TRUCK::m(int L, int R) {
  motorRF.m(R);
  motorRB.m(R);
  motorLF.m(L);
  motorLB.m(L);
}

void TRUCK::stop() {
  motorRF.stop();
  motorRB.stop();
  motorLF.stop();
  motorLB.stop();
}

void TRUCK::move(int vx, int vy, int w) {
  double r = 1;

  a = constrain(r * (vx - vy - w), -255, 255);
  b = constrain(r * (vx + vy + w), -255, 255);
  c = constrain(r * (vx + vy - w), -255, 255);
  d = constrain(r * (vx - vy + w), -255, 255);

  motorLF.m(a);
  motorRF.m(b);
  motorLB.m(c);
  motorRB.m(d);
}
