#ifndef FUNC_H
#define FUNC_H

#include <Arduino.h>
#include "INCLUDE.h"
#include "DEFINE.h"

extern iarduino_I2C_Expander gpio;
extern Adafruit_NeoPixel backlight;
extern char interruptFlag;
extern uint64_t interruptTimer;

void interruptRF();
void interruptLF();
void interruptRB();
void interruptLB();

void onBurner();
void offBurner();

void changeColor(unsigned char, unsigned char, unsigned char);

#endif // FUNC_H
