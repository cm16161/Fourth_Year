#include "encoders.h"
#include "pid.h"
#include "line_sensors.h"

//Pin definitions for motor
#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN  9
#define R_DIR_PIN 15

float Kp_left = 0; //Proportional gain for position controller
float Kd_left = 0; //Derivative gain for position controller
float Ki_left = 0; //Integral gain for position controller
PID left_PID(Kp_left, Ki_left, Kd_left); //Position controller for left wheel position


// You may need to change these depending on how you wire
// in your line sensor.
#define LINE_LEFT_PIN A4 //Pin for the left line sensor
#define LINE_CENTRE_PIN A3 //Pin for the centre line sensor
#define LINE_RIGHT_PIN A2 //Pin for the right line sensor

LineSensor line_left(LINE_LEFT_PIN); //Create a line sensor object for the left sensor
LineSensor line_centre(LINE_CENTRE_PIN); //Create a line sensor object for the centre sensor
LineSensor line_right(LINE_RIGHT_PIN); //Create a line sensor object for the right sensor

float pL, pC, pR, LineCentre;

#define BAUD_RATE = 115200;

void setupMotorPins()
{
  // Set our motor driver pins as outputs.
  pinMode( L_PWM_PIN, OUTPUT );
  pinMode( L_DIR_PIN, OUTPUT );
  pinMode( R_PWM_PIN, OUTPUT );
  pinMode( R_DIR_PIN, OUTPUT );
  pinMode(6, OUTPUT);

  // Set initial direction for l and r
  // Which of these is foward, or backward?
  digitalWrite( L_DIR_PIN, LOW  );
  digitalWrite( R_DIR_PIN, LOW );
}

// Remember, setup only runs once.
void setup()
{

  // These two function set up the pin
  // change interrupts for the encoders.
  setupEncoder0();
  setupEncoder1();

  // Initialise your other globals variables
  // and devices.
  // Initialise the Serial communication
  Serial.begin( 9600 );
  delay(1000);
  Serial.println("***RESET***");
  line_left.calibrate();
  line_centre.calibrate();
  line_right.calibrate();
  float total_calibrate = line_left.calibrated_value + line_centre.calibrated_value + line_right.calibrated_value;
  pL = line_left.calibrated_value / total_calibrate;
  pC = line_centre.calibrated_value / total_calibrate;
  pR = line_right.calibrated_value / total_calibrate;
  LineCentre = (1000 * pL + 2000 * pC + 3000 * pR) - 2000;
  delay(1000);
}



// Remmeber, loop is called again and again.
void loop()
{
  // build your main code here.
  int left_val = line_left.read_calibrated();
  int cent_val = line_centre.read_calibrated();
  int right_val = line_right.read_calibrated();
  if (left_val >= 100) {
    Serial.println(left_val);
  }
  if (cent_val >= 100) {
    Serial.println(cent_val);
  }
  if (right_val >= 100) {
    Serial.println(right_val);
  }
  delay(250);
}
