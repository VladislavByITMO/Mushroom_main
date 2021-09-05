#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "INCLUDE.h"
#include "DEFINE.h"

class MOTOR {
  private:
    char pin1, pin2;

  public:
    MOTOR();
    MOTOR(char, char);
    ~MOTOR();

    void setPin1(char);
    void setPin2(char);

    void mF(int); //вперед
    void mB(int); //назад
    void stop(); //стоп

    void m(int); //движение с pwm [-255..255]
};

#endif // MOTOR_H
