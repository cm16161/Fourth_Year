#include "encoders.h"
#include "pid.h"
#include "kinematics.h"

#define LOOP_DELAY 100

//Pin definitions for motor
#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN  9
#define R_DIR_PIN 15

float Kp_pose = 0; //Proportional gain 
float Kd_pose = 0; //Derivative gain 
float Ki_pose = 0; //Integral gain 
PID left_PID(Kp_left, Ki_left, Kd_left ); // controller for left wheel 

Kinematics kinematics;  // New kinematics class instance.


void setupMotorPins()
{
    // Set our motor driver pins as outputs.
  pinMode( L_PWM_PIN, OUTPUT );
  pinMode( L_DIR_PIN, OUTPUT );
  pinMode( R_PWM_PIN, OUTPUT );
  pinMode( R_DIR_PIN, OUTPUT );

  // Set initial direction for l and r
  // Which of these is foward, or backward?
  digitalWrite( L_DIR_PIN, LOW  );
  digitalWrite( R_DIR_PIN, LOW );
}

// Remember, setup runs just once.
void setup() {

  
  // These two function set up the pin
  // change interrupts for the encoders.
  // If you want to know more, find them
  // at the end of this file.  
  setupEncoder0();
  setupEncoder1();

  // Initialise your other globals and 
  // devices.


  // Initialise the Serial communication
  // so that we can inspect the values of
  // our encoder using the Monitor.
  Serial.begin( 9600 );
  delay(1000);
  Serial.println("***RESET***");
}

// Remember, loop runs again and again
void loop()  {

  
  
}




