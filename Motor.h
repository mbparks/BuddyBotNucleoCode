#ifndef MBED_MOTOR_H
#define MBED_MOTOR_H

#include "mbed.h"

class Motor {
  public:
    Motor(PinName hMotor1A, PinName hMotor2A, PinName hMotor1B, PinName hMotor2B);
    void moveLeft();
    void moveRight();
    
  protected:
    DigitalOut _hMotor1A;  //Motor 1 + terminal
    DigitalOut _hMotor2A;  //Motor 2 + terminal
    DigitalOut _hMotor1B;  //Motor 1 - terminal
    DigitalOut _hMotor2B;  //Motor 2 - terminal
    
    int _motorOn;
    int _motorOff;

    float _motorOnDuration;

    int _numMotorPulseCycleStandard;
    int _numMotorPulseCycleMax;
    
    void hMoveCW(int numPulseCycles);
    void hMoveCCW(int numPulseCycles);
};
  
#endif
  