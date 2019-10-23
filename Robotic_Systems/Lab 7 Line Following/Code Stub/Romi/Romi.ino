#include "encoders.h"
#include "pid.h"
#include "line_sensors.h"

//Pin definitions for motor
#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN  9
#define R_DIR_PIN 15
#define BUZZER_PIN 6

#define LEFT 0
#define RIGHT 1
#define FORWARD 0
#define BACKWARD 1
#define CAT(a, b) a ## b
#define DIR_PIN(pin) CAT(pin ,_DIR_PIN)
#define PWM_PIN(pin) CAT(pin ,_PWM_PIN)
#define ONE_REVOLUTION 1435
#define CIRCUMFERENCE 22
#define SPEED 30


float Kp_left = 0; //Proportional gain for position controller
float Kd_left = 0; //Derivative gain for position controller
float Ki_left = 0; //Integral gain for position controller
PID left_PID(Kp_left, Ki_left, Kd_left); //Position controller for left wheel position

#define kp_left 0.075
#define ki_left 0
#define kd_left 00
#define DELAY_DURATION 1
unsigned long last_time;

PID line_pid( kp_left, ki_left, kd_left );

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

//////////////////////////////////////////////////////////////////////////////////////////////

enum state {
  pre_line,
  on_line,
  found_end_of_line,
  go_home
};

state g_state;

//////////////////////////////////////////////////////////////////////////////////////////////

void setupMotorPins()
{
  // Set our motor driver pins as outputs.
  pinMode( L_PWM_PIN, OUTPUT );
  pinMode( L_DIR_PIN, OUTPUT );
  pinMode( R_PWM_PIN, OUTPUT );
  pinMode( R_DIR_PIN, OUTPUT );
  pinMode(6, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_BUILTIN_TX, OUTPUT);

  // Set initial direction for l and r
  // Which of these is foward, or backward?
  digitalWrite( L_DIR_PIN, LOW  );
  digitalWrite( R_DIR_PIN, LOW );
}

//////////////////////////////////////////////////////////////////////////////////////////////

void moveMotor(int pin, float scalar_speed) {
  bool motor_direction;
  if (scalar_speed < 0) {
    motor_direction = true;
    scalar_speed *= -1;
  }
  else motor_direction = false;
  if (scalar_speed >= 230) {
    scalar_speed = 230;
  }
//  else if (scalar_speed <= 10) {
//    scalar_speed = 10;
//  }
//  else if(scalar_speed == 0){
//    scalar_speed = 0;
//  }
  int dir, pwm;
  if (pin == LEFT) {
    dir = DIR_PIN(L);
    pwm = PWM_PIN(L);
  }
  else if (pin == RIGHT) {
    dir = DIR_PIN(R);
    pwm = PWM_PIN(R);
  }
  digitalWrite(dir, motor_direction);
  analogWrite(pwm, scalar_speed);

}

void stopMotor() {
  moveMotor(LEFT, 0);
  moveMotor(RIGHT, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void moveDistance(float distance) {
  int dir = 0;
  distance *= 0.1;
  if (distance < 0) {
    dir = 1;
  }
  int encoder_count = (distance / CIRCUMFERENCE) * ONE_REVOLUTION;
  if (dir == 1) {
    if (count_left >= encoder_count) {
      moveMotor(LEFT, -SPEED);
    }
    if (count_right >= encoder_count) {
      moveMotor(RIGHT, -SPEED);
    }
    if (count_left <= encoder_count && count_right <= encoder_count) {
      moveMotor(LEFT, 0);
      moveMotor(RIGHT, 0);
    }
  }
  else {
    if (count_left <= encoder_count) {
      moveMotor(LEFT, SPEED);
    }
    if (count_right <= encoder_count) {
      moveMotor(RIGHT, SPEED);
    }

    if (count_left >= encoder_count && count_right >= encoder_count) {
      moveMotor(LEFT, 0);
      moveMotor(RIGHT, 0);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////

//void rotateDegrees(float degree) {
//  int dir = 0;
//  if (degree < 0) {
//    degree *= -1;
//    dir = 1;
//  }
//  float encoder_count = (degree / 360) * 2 * ONE_REVOLUTION;
//  if (dir == 0) {
//    if (count_left <= encoder_count) {
//      moveMotor(LEFT, SPEED);
//    }
//    else {
//      moveMotor(LEFT, 0);
//    }
//    if (count_right >= -encoder_count) {
//      moveMotor(RIGHT, -SPEED);
//    }
//    else {
//      moveMotor(RIGHT, 0);
//    }
//  }
//  else {
//    if (count_left >= -encoder_count) {
//      moveMotor(LEFT, -SPEED);
//    }
//    else {
//      moveMotor(LEFT, 0);
//    }
//    if (count_right <= encoder_count) {
//      moveMotor(RIGHT, SPEED);
//    }
//    else {
//      moveMotor(RIGHT, 0);
//    }
//  }
//
//}

//////////////////////////////////////////////////////////////////////////////////////////////

// Remember, setup only runs once.
void setup()
{
  last_time = millis();
  // These two function set up the pin
  // change interrupts for the encoders.
  setupEncoder0();
  setupEncoder1();
  g_state = pre_line;


  // Initialise your other globals variables
  // and devices.
  // Initialise the Serial communication
  Serial.begin( 9600 );
  delay(1000);
  Serial.println("***RESET***");
  line_left.calibrate();
  line_centre.calibrate();
  line_right.calibrate();
  delay(1000);
  moveMotor(RIGHT, 20);
  moveMotor(LEFT, 20);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void bangBang() {
  int left_val = line_left.read_calibrated();
  int cent_val = line_centre.read_calibrated();
  int right_val = line_right.read_calibrated();
//  Serial.print(left_val);
//  Serial.print(", ");
//  Serial.print(cent_val);
//  Serial.print(", ");
//  Serial.println(right_val);
  if (left_val >= 200) {
//    Serial.println(left_val);
    //moveMotor(RIGHT, 15);
    moveMotor(LEFT, -15);
  }
  else if (cent_val >= 200) {
//    Serial.println(cent_val);
    moveMotor(LEFT, 12);
    moveMotor(RIGHT, 12);
  }
  else if (right_val >= 200) {
//    Serial.println(right_val);
    //moveMotor(LEFT, 15);
    moveMotor(RIGHT, -15);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////

bool getFirstLine() {
  int left_val = line_left.read_calibrated();
  int cent_val = line_centre.read_calibrated();
  int right_val = line_right.read_calibrated();
  if (left_val >= 500 || cent_val >= 500 || right_val >= 500) {
    return true;

  }
  else return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////

bool checkForLine() {
  int left_val = line_left.read_calibrated();
  int cent_val = line_centre.read_calibrated();
  int right_val = line_right.read_calibrated();
  static long confidence_value = 10000;
  const int confidence_threshold = 10000;
//  Serial.print(left_val);
//  Serial.print(", ");
//  Serial.print(cent_val);
//  Serial.print(", ");
//  Serial.println(right_val);
    if (left_val >= 500) {
      confidence_value += 1;
    }
    else if (cent_val >= 500) {
      confidence_value += 1;
    }
    else if (right_val >= 500) {
      confidence_value += 1;
    }
//  if (left_val >= 500 || cent_val >= 500 || right_val >= 500) {
//    return true;
//
//  }
  if(left_val < 500 && cent_val <500 && right_val <500)
  {
        confidence_value -= 100000000000000000000000000000;
  }

    if (confidence_value >= confidence_threshold) {
      return true;
    }
    else {
      return false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////

void rotateDegrees(float degree) {
  int dir = 0;
  if (degree < 0) {
    degree *= -1;
    dir = 1;
  }
  float encoder_count = (degree / 360) * 2 * ONE_REVOLUTION;
//  Serial.println(encoder_count);
//  Serial.println(0.25 * 2 * ONE_REVOLUTION);
  if (dir == 0) {
    if (count_left <= encoder_count) {
      moveMotor(LEFT, SPEED);
    }
    else {
      moveMotor(LEFT, 0);
    }
    if (count_right >= -encoder_count) {
      moveMotor(RIGHT, -SPEED);
    }
    else {
      moveMotor(RIGHT, 0);
    }
  }
  else {
    if (count_left >= -encoder_count) {
      moveMotor(LEFT, -SPEED);
    }
    else {
      moveMotor(LEFT, 0);
    }
    if (count_right <= encoder_count) {
      moveMotor(RIGHT, SPEED);
    }
    else {
      moveMotor(RIGHT, 0);
    }
  }

}

//////////////////////////////////////////////////////////////////////////////////////////////

int toggleBuzzer() {
  static bool toggle = false;
  toggle = !toggle;
  if (toggle) {
    digitalWrite(BUZZER_PIN, 1);
    return 0;
  }
  else {
    digitalWrite(BUZZER_PIN, 0);
    return 1;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////


// Remmeber, loop is called again and again.
void loop()
{
  delay(5);
  static bool onLine;
  static bool firstFound = false;
  static bool block = false;
  unsigned long elapsed_time, current_time;
  current_time = millis();
  elapsed_time = current_time - last_time;
  switch (g_state) {
    case pre_line: firstFound = getFirstLine(); if (firstFound) {
        g_state = on_line;
      }
      break;
    case on_line: onLine = checkForLine();
      if (onLine) {
        if (elapsed_time > DELAY_DURATION) {
          int left_val = line_left.read_calibrated();
          int cent_val = line_centre.read_calibrated();
          int right_val = line_right.read_calibrated();
          float total_calibrate = left_val + cent_val + right_val;
          pL = left_val / total_calibrate;
          pC = cent_val / total_calibrate;
          pR = right_val / total_calibrate;
          LineCentre = (1000 * pL + 2000 * pC + 3000 * pR);
          float output_line = line_pid.update(2000, LineCentre);
          if (output_line > 5) {
            moveMotor(LEFT, -30);
            moveMotor(RIGHT, 30);
          }
          else if (output_line < -3.5) {
            moveMotor(RIGHT, -30);
            moveMotor(LEFT, 30);
          }
          else {
            moveMotor(LEFT, 20);
            moveMotor(RIGHT, 20);
          }
          last_time = millis();
        }
      }
      else {
        stopMotor();
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(LED_BUILTIN_TX, LOW);
        g_state = found_end_of_line;
      } break;
    case found_end_of_line: 
      stopMotor();
      static int buz_count = 0;
      if (elapsed_time > 500) {
        if (buz_count < 10) {
          toggleBuzzer();
          buz_count++;
        }
        else {
          g_state = go_home;
        }
        last_time = millis();
      }
      stopMotor();
      break;
  }

  // build your main code here.

}
