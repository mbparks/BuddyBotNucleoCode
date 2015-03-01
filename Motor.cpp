#include "Motor.h"
#include "mbed.h"

//-----------------------------------------
//Public Function:  Constructor
//Inputs: 4 integers corresponding to the 
//4 pins that are driving the L293D
//   Motor 1 + terminal
//   Motor 2 + terminal
//   Motor 1 - terminal
//   Motor 2 - terminal
//
//Returns: None
//-----------------------------------------
Motor::Motor(PinName hMotor1A, PinName hMotor2A, PinName hMotor1B, PinName hMotor2B)
    : _hMotor1A(hMotor1A)
    , _hMotor2A(hMotor2A)
    , _hMotor1B(hMotor1B)
    , _hMotor2B(hMotor2B)
    { 
        _motorOn = 1;
        _motorOff = 0;

        _motorOnDuration = 0.02;
        
        _numMotorPulseCycleStandard = 5;
        _numMotorPulseCycleMax = 10;
}
    
    
//-----------------------------------------
//Public Function:  moveLeft
//Desctiption: Calls hMoveCW to turn motor
//to the left
//Inputs: none
//Returns: none
//-----------------------------------------
void Motor::moveLeft() {
    hMoveCW(_numMotorPulseCycleStandard);
}


//-----------------------------------------
//Public Function:  moveRight
//Desctiption: Calls hMoveCCW to turn motor
//to the left
//Inputs: none
//Returns: none
//-----------------------------------------
void Motor::moveRight() {
    hMoveCCW(_numMotorPulseCycleStandard);
}


//-----------------------------------------
//Private Function:  hMoveCW
//Desctiption: Pulse motor in sequence to
//cause clockwise motion, repeats as many
//times as input numPulseCycles
//Inputs: int numPulseCycles
//Returns: none
//-----------------------------------------
void Motor::hMoveCW(int numPulseCycles) {
    for (int x=0; x<numPulseCycles; x++) {
    _hMotor1A=_motorOn;
    _hMotor2A=_motorOn;
    _hMotor1B=_motorOff;
    _hMotor2B=_motorOff;
    wait(_motorOnDuration);
        
    _hMotor1A=_motorOff;
    _hMotor2A=_motorOn;
    _hMotor1B=_motorOn;
    _hMotor2B=_motorOff;
    wait(_motorOnDuration);
        
    _hMotor1A=_motorOff;
    _hMotor2A=_motorOff;
    _hMotor1B=_motorOn;
    _hMotor2B=_motorOn;
    wait(_motorOnDuration);
        
    _hMotor1A=_motorOn;
    _hMotor2A=_motorOff;
    _hMotor1B=_motorOff;
    _hMotor2B=_motorOn;
    wait(_motorOnDuration); }
}


//-----------------------------------------
//Private Function:  hMoveCCW
//Desctiption: Pulse motor in sequence to
//cause counter-clockwise motion, repeats as many
//times as input numPulseCycles
//Inputs: int numPulseCycles
//Returns: none
//-----------------------------------------
void Motor::hMoveCCW(int numPulseCycles) {
    for (int x=0; x<numPulseCycles; x++) {
    _hMotor1A=_motorOn;
    _hMotor2A=_motorOff;
    _hMotor1B=_motorOff;
    _hMotor2B=_motorOn;
    wait(_motorOnDuration);
        
    _hMotor1A=_motorOff;
    _hMotor2A=_motorOff;
    _hMotor1B=_motorOn;
    _hMotor2B=_motorOn;
    wait(_motorOnDuration);
        
    _hMotor1A=_motorOff;
    _hMotor2A=_motorOn;
    _hMotor1B=_motorOn;
    _hMotor2B=_motorOff;
    wait(_motorOnDuration);    
    
    _hMotor1A=_motorOn;
    _hMotor2A=_motorOn;
    _hMotor1B=_motorOff;
    _hMotor2B=_motorOff;
    wait(_motorOnDuration); }
}