#include "encoders.h"
#include "pid.h"
#include "kinematics.h"

#define LOOP_DELAY 100

//Pin definitions for motor
#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN  9
#define R_DIR_PIN 15

#define DELAY_DURATION 1000
#define LEFT 0
#define RIGHT 1
#define SPEED 30
#define NINETY_L 0
#define NINETY_R 1
#define ONE_EIGHTY 2
#define NINETY_DEGREES 1000
#define FORWARD 0
#define BACKWARD 1
#define NINETY_ROTATE_SPEED 29

#define TEN_CM 653
#define ONE_REVOLUTION 1435
#define CIRCUMFERENCE 22

#define CAT(a, b) a ## b
#define DIR_PIN(pin) CAT(pin ,_DIR_PIN)
#define PWM_PIN(pin) CAT(pin ,_PWM_PIN)

volatile unsigned long timer3_count;
float last_timestamp, last_change;
long measured_speed;
long last_count;
volatile long last_left = 0;
volatile long last_timer3 = 0;
volatile long timer3_speed = 0;

volatile unsigned long e0_interval;
volatile unsigned long e0_last_time;

// Global volatile to hold the determined
// speed of wheel fixed to encoder0
volatile float e0_speed;


float Kp_pose = 0; //Proportional gain
float Kd_pose = 0; //Derivative gain
float Ki_pose = 0; //Integral gain

float Kp_left = 0;
float Ki_left = 0;
float Kd_left = 0;
PID left_PID(Kp_left, Ki_left, Kd_left ); // controller for left wheel

Kinematics kinematics(count_left, count_right);  // New kinematics class instance.


bool g_switch;

void moveMotor(int pin, float scalar_speed) {
  bool motor_direction;
  if (scalar_speed < 0) {
    motor_direction = true;
    scalar_speed *= -1;
  }
  else motor_direction = false;
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
      g_switch = !g_switch;
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
      g_switch = !g_switch;
    }
  }
}

void rotateDegrees(float degree) {
  int dir = 0;
  if (degree < 0) {
    degree *= -1;
    dir = 1;
  }
  float encoder_count = (degree / 360) * 2 * ONE_REVOLUTION;
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

  g_switch = false;
  // These two function set up the pin
  // change interrupts for the encoders.
  // If you want to know more, find them
  // at the end of this file.
  setupEncoder0();
  setupEncoder1();
  setupTimer3(1);
  last_timestamp = micros();
  last_count = 0;
  measured_speed = 0;
  last_change = millis();
  timer3_count = 0;

  // Initialise your other globals and
  // devices.


  // Initialise the Serial communication
  // so that we can inspect the values of
  // our encoder using the Monitor.
  Serial.begin( 9600 );
  delay(1000);
  Serial.println("***RESET***");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

double getHomeAngle(Kinematics kinematics) {
  double dx, dy;
  dx = -kinematics.m_x;
  dy = -kinematics.m_y;
  return atan2(dy, dx);
}

double getHomeDistance(Kinematics kinematics){
  double dx = -kinematics.m_x;
  double dy = -kinematics.m_y;
  return sqrt((dx*dx)+(dy*dy));
}

void goHome(Kinematics kinematics){
  double home_theta = getHomeAngle(kinematics);
  double home_distance = getHomeDistance(kinematics);
  float home_theta_degrees = home_theta * PI / 180;
  rotateDegrees(home_theta_degrees);
  moveDistance((float)home_distance);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Remember, loop runs again and again
void loop()  {
  static bool reverse;
  kinematics.update(count_left, count_right);
  float elapsed_time, current_time;

  //  if (!g_switch) {
  //    moveDistance(100);
  //  }
  //  else{
  //    moveDistance(-100);
  //  }
  current_time = millis();
  elapsed_time = current_time - last_timestamp;
  rotateDegrees(90);
  Serial.println(kinematics.m_theta * 180 / 3.14);


}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setupTimer3( int hertz ) {

  // disable global interrupts
  cli();

  // Reset timer3 to a blank condition.
  // TCCR = Timer/Counter Control Register
  TCCR3A = 0;     // set entire TCCR3A register to 0
  TCCR3B = 0;     // set entire TCCR3B register to 0

  // First, turn on CTC mode.  Timer3 will count up
  // and create an interrupt on a match to a value.
  // See table 14.4 in manual, it is mode 4.
  TCCR3B = TCCR3B | (1 << WGM32);

  // Set prescaler value.
  TCCR3B = TCCR3B | (1 << CS32);

  // Set Compare Match counter value
  OCR3A =  62500 / hertz;


  // enable timer compare interrupt:
  TIMSK3 = TIMSK3 | (1 << OCIE3A);

  // enable global interrupts:
  sei();
}



// The ISR routine.
// The name TIMER3_COMPA_vect is a special flag to the
// compiler.  It automatically associates with Timer3 in
// CTC mode.
ISR( TIMER3_COMPA_vect ) {
  timer3_count++;
  float time_diff = timer3_count - last_timer3;
  float delta_c = count_left - last_left;
  timer3_speed = delta_c / time_diff;
  last_left = count_left;
  last_timer3 = timer3_count;

}
