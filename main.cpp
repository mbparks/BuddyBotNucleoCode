//-------------------------------------------------------------
//File:         /CatLaserProject/main.cpp
//Target HW:    ST Nucleo F401RE
//Description:  This project allows a user to control a laser
//cat toy via Bluetooth from a companion Android app.
//
//Released under Creative Commons License
//Pinout:
//PA_2:  Computer Serial TX  / D1
//PA_3:  Computer Serial RX  / D0
//
//PA_8:  Laser               / D7
//
//PA_9:  Bluetooth Serial TX   / D8
//PA_10: Bluetooth Serial RX   / D2
//
//PB_3:  Left Kill Switch   / D3
//PB_4:  Right Kill Switch  / D5   
//
//PB_6 /D10:  hMotor Coil 1A + to 293-2   |   1Y+   O B/W    293-3
//PA_7 /D11:  hMotor Coil 2A + to 293-15  |   2Y+   R Y/W    293-14
//PB_10 /D6: hMotor Coil 1B -  to 293-7   |   1Y-   B O/W    293-6
//PA_5 /D13:  hMotor Coil 2B - to 293-10  |   2Y-   Y R/W    293-11
//
//PA_6:  Ultrasonic Sensor   / D12
//
//
//Bluetooth Commands Codes:
// l:  Move laser LEFT
// r:  Move laser RIGHT
// a:  Turn laser ON
// z:  Turn laser OFF
// b:  Enter AUTO mode
// y:  Enter MANUAL mode
//-------------------------------------------------------------

//Include needed libraries
#include "mbed.h"
#include "Ping.h"
#include "Timer.h"
#include "Motor.h"
#include "InterruptIn.h"
#include <stdlib.h> 

#define laserOn 1   //Command to turn laser on
#define laserOff 0  //Command to turn laser off

//Initialize hardware
Serial pc(SERIAL_TX, SERIAL_RX);  //Print debug info to computer
Serial bt(PA_9,PA_10);            //Bluetooth RN42 serial

DigitalOut LaserPin(PA_8);        //Laser control pin

Motor hMotor(PB_6, PA_7, PB_10, PA_5);   //Stepper Motor

Timer timer;

Ping catSensor(PA_6);    // Ping Ultrasonic sensor

InterruptIn leftKillSwitch(PB_3);    //Interrupt when motor drives too far left
InterruptIn rightKillSwitch(PC_7);   //Interrupt when motor drive too far right

//Declare variables
bool autoModeOnFlag = false;    //Track if system is in automatic robot mode  
bool laserOnFlag = false;       //Track if laser is turned on

unsigned int m_z=74626;         //Used for random number generator
unsigned int m_w=23865;         //Used for random number generator

int triggerDistanceCM = 100;    //Distance pet needs to be from BuddyBot before triggering auto move, in centimeters
int autoMoveInterval = 5;       //Time between movements of the laser when in auto mode, in milliseconds

float pause = 0.02;             //Wait time for certain actions, in milliseconds


//Declare supporting functions / Interrupt Service Routines

//---------------------------------------------------------
//Function: setLaser
//Description:  Ths function toggles the laser attached to 
//based on variable newLaserStatus.
//0 turn laser OFF, 1 to turn laser ON
//---------------------------------------------------------      
void setLaser(int newLaserStatus) {
    if(newLaserStatus == laserOn) {
        LaserPin = laserOn;
        //pc.printf("Cmd Rx: Laser ON\n");
        laserOnFlag = true;
    }
    else if(newLaserStatus == laserOff) {
        LaserPin = laserOff;
        //pc.printf("Cmd Rx: Laser OFF\n");
        laserOnFlag = false;
    }
}


//---------------------------------------------------------
//Function: handleLeftInterrupt   (ISR)
//Description:  Moves motor slightly to the right after
//the left kill switch is triggered (motor move to max. left
//position).
//---------------------------------------------------------   
void handleLeftInterrupt() {
    //pc.printf("Handling left interrupt\n");
    hMotor.moveLeft();
    wait(pause);
    hMotor.moveLeft();
    wait(pause);
}


//---------------------------------------------------------
//Function: handleRightInterrupt     (ISR)
//Description:  Moves motor slightly to the left after
//the right kill switch is triggered (motor move to max. right
//position).
//--------------------------------------------------------- 
void handleRightInterrupt() {
    //pc.printf("Handling right interrupt\n");
    hMotor.moveRight();
    wait(pause);
    hMotor.moveRight();
    wait(pause);
}


//---------------------------------------------------------
//Function: rnd
//Description:  Computes a random integer
//--------------------------------------------------------- 
unsigned int rnd() {
    m_z = 36969 * (m_z & 65535) + (m_z >>16);
    m_w = 18000 * (m_w & 65535) + (m_w >>16);
    return ((m_z <<16) + m_w);
}


//---------------------------------------------------------
//Function: moveLaserRandom
//Description:  Moves motor in a random fashion when
//user sets BuddyBot into automatics robot mode.
//Only moves if it senses pet within distance as controlled
//by the variable triggerDistanceCM.
//--------------------------------------------------------- 
void moveLaserRandom() {
    int range;
    catSensor.Send();
    wait_ms(autoMoveInterval);
    range = catSensor.Read_cm();
    
    if (range < triggerDistanceCM) {
        int random = rnd()%2;
        //pc.printf("Random Number: %d \n", random);
        if (random == 1) {
            hMotor.moveLeft();
            wait(3);
        }
        else {
            hMotor.moveRight();
            wait(3);
        }
    }
}


//---------------------------------------------------------
//Function: autoMode_Main
//Description:  Main routine that runs while the BuddyBot
//is in automatic robot mode.
//Listens for user commands over Bluetooth and runs the
//moveLaserRandom() function to make random movements
//of the laser.
//--------------------------------------------------------- 
void autoMode_Main() {
    //pc.printf("Currently in AUTO MODE\n");
    while(1) {
        
        moveLaserRandom();  //Move the laser
        
        //Has the user sent a new command?
        if(bt.readable()) {
            char c = bt.getc();  //If so, read the char
        
            //If received char 'y', return to manual control mode
            if(c == 'y') {
                //pc.printf("Cmd Rx: Manual Mode\n");
                autoModeOnFlag = false;
                break;
            }
        
            //If received char 'a',  turn laser ON
            if(c == 'a') {
               setLaser(laserOn);
            }
    
            //If received char 'z',  turn laser OFF
            if(c == 'z') {
                setLaser(laserOff);
            }
        }
    }
}


//---------------------------------------------
//MAIN LOOP:
//---------------------------------------------
int main() {
    
    //Intialize the interrupts
    leftKillSwitch.rise(&handleLeftInterrupt);
    rightKillSwitch.rise(&handleRightInterrupt);
    
    //Put interrupt pins into PullDown mode
    leftKillSwitch.mode(PullDown);
    rightKillSwitch.mode(PullDown);
    
    //Set Bluetooth communication baud rate to 115200
    bt.baud(115200);

    //Main loop, continue to listen from user commands over Bluetooth
    while(1) {
        char c = bt.getc();  //If command received, read it into char
    
        //If we are in manual mode, only then accept commands from app
        //to turn the motor
        if(autoModeOnFlag == false) {
        
            //If received char 'l', move laser LEFT
            if(c == 'l') {
                hMotor.moveLeft();
                //pc.printf("Cmd Rx: Left\n");
            }
    
            //If received char 'r', move Laser RIGHT
            if(c == 'r') {
                hMotor.moveRight();
                //pc.printf("Cmd Rx: Right\n");
            }
        }
    
        //If received char 'a', turn laser ON
        if(c == 'a') {
            //pc.printf("Cmd Rx: Turn laser ON\n");
            setLaser(laserOn);
        }
    
        //If received char 'z', turn laser OFF
        if(c == 'z') {
            //pc.printf("Cmd Rx: Turn laser OFF\n");
            setLaser(laserOff);
        }
    
        //If received char 'b', enter Auto Mode
        if(c == 'b') {
            //pc.printf("Cmd Rx: Auto Mode\n");
            autoModeOnFlag = true;
            autoMode_Main();
        }
    
        //If received char 'y', enter Manual Mode  
        if(c == 'y') {
            //pc.printf("Cmd Rx: Manual Mode\n");
            autoModeOnFlag = false;
        }   
    }
} 