#ifndef TRUCK_H
#define TRUCK_H

#include <Arduino.h>
#include "INCLUDE.h"
#include "DEFINE.h"

#include "MOTOR.h"

class TRUCK {
  private:
    MOTOR motorRF;
    MOTOR motorLF;
    MOTOR motorRB;
    MOTOR motorLB;

    int a, b, c, d;

  public:
    TRUCK();
    TRUCK(char, char, char, char, char, char, char, char);
    ~TRUCK();

    void mF(int); //движение вперед
    void mB(int); //движение назад

    void m(int, int); //движение по танковой схеме с заданным PWM на левую и правую сторону

    void rR(int); //поворот направао
    void rL(int); //поворот налево

    void stop(); //остановиться

    void move(int, int, int); //тест гироскопа ???
};

#endif // MOTOR_H
