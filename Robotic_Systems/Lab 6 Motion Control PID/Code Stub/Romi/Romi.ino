#include "encoders.h"
#include "pid.h"

#define kp_left 0.00
#define ki_left 0.00
#define kd_left 0.00

PID left_pid( kp_left, ki_left, kd_left );

// Remember, setup only runs once.
void setup() 
{

  // Initialise your other globals variables
  // and devices.
  
  // Initialise the Serial communication
  Serial.begin( 9600 );
  delay(1000);
  Serial.println("***RESET***");
}



// Remmeber, loop is called again and again.
void loop() 
{

  // build your main code here. 
  // Call your pid.update() at a regular time interval.
}




