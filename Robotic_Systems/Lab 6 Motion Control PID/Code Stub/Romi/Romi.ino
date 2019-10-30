#include "encoders.h"
#include "pid.h"

#define E1_A_PIN  7
#define E1_B_PIN  23
#define E0_A_PIN  26
#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN  9
#define R_DIR_PIN 15
#define LEFT 0
#define RIGHT 1
#define FORWARD 0
#define BACKWARD 1
#define CAT(a, b) a ## b
#define DIR_PIN(pin) CAT(pin ,_DIR_PIN)
#define PWM_PIN(pin) CAT(pin ,_PWM_PIN)


#define kp_left 0.005
#define ki_left 0.0000000005
#define kd_left 15
#define DELAY_DURATION 2000

PID left_pid( kp_left, ki_left, kd_left );
PID right_pid(kp_left, ki_left, kd_left );
unsigned long last_time;
volatile unsigned long timer3_count;
volatile long last_left = 0;
volatile long last_timer3 = 0;
volatile long timer3_speed_left = 0;
volatile long last_right = 0;
volatile long timer3_speed_right = 0;

// Remember, setup only runs once.
void setup()
{

  // Initialise your other globals variables
  // and devices.
  last_time = millis();
  // Initialise the Serial communication
  setupEncoder0();
  setupEncoder1();

  pinMode( L_PWM_PIN, OUTPUT );
  pinMode( L_DIR_PIN, OUTPUT );
  pinMode( R_PWM_PIN, OUTPUT );
  pinMode( R_DIR_PIN, OUTPUT );

  // Set initial direction for l and r
  // Which of these is foward, or backward?
  digitalWrite( L_DIR_PIN, LOW);
  digitalWrite( R_DIR_PIN, LOW);

  Serial.begin( 9600 );
  delay(1000);
  Serial.println("***RESET***");
}

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
  else if (scalar_speed <= 10) {
    scalar_speed = 10;
  }
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

// Remmeber, loop is called again and again.
void loop()
{
  unsigned long elapsed_time, current_time;
  current_time = millis();
  elapsed_time = current_time - last_time;
  if (elapsed_time >= DELAY_DURATION) {
      float output_l = left_pid.update(5000, count_left);
      moveMotor(LEFT, output_l);
      float output_r = right_pid.update(5000, count_right);
      moveMotor(RIGHT, output_r);
    Serial.print("Left wheel output is: ");
    Serial.println(output_l);
    Serial.println(count_left);
    last_time = millis();
  }

  // build your main code here.
  // Call your pid.update() at a regular time interval.
  //    output_signal <----PID-- demand, measurement
  //  float output_l = left_pid.update(5000, count_left);
  //  moveMotor(LEFT, output_l);
  //  float output_r = right.pid.update(5000,count_right);
  //  moveMotor(RIGHT,output_r);

}

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
  float delta_cl = count_left - last_left;
  timer3_speed_left = delta_cl / time_diff;
  float delta_cr = count_right - last_right;
  timer3_speed_right = delta_cr / time_diff;
  last_right = count_right;
  last_left = count_left;
  last_timer3 = timer3_count;

}
