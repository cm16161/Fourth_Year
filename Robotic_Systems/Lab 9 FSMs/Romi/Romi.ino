//////////////////////////// Assesment Code \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\    

#include "encoders.h"
#include "pid.h"
#include "line_sensors.h"
#include "kinematics.h"

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
#define CIRCUMFERENCE 220
#define SPEED 30


float Kp_left = 0; //Proportional gain for position controller
float Kd_left = 0; //Derivative gain for position controller
float Ki_left = 0; //Integral gain for position controller
PID left_PID(Kp_left, Ki_left, Kd_left); //Position controller for left wheel position

#define kp_left 0.0275
#define ki_left 0.000000001
#define kd_left 10

#define kp_line 0.075
#define ki_line 0
#define kd_line 00

#define kp_head 1
#define ki_head 0.000
#define kd_head 0

#define kp_left_vel 0.05
#define ki_left_vel 0.0000000005
#define kd_left_vel 0

#define DELAY_DURATION 1
unsigned long last_time;

PID line_pid( kp_line, ki_line, kd_line );
PID left_pid( kp_left, ki_left, kd_left );
PID right_pid(kp_left, ki_left, kd_left );
PID head_pid(kp_head, ki_head, kd_head);

PID left_vel_pid( kp_left, ki_left, kd_left );
PID right_vel_pid( kp_left, ki_left, kd_left );


// You may need to change these depending on how you wire
// in your line sensor.
#define LINE_LEFT_PIN A4 //Pin for the left line sensor
#define LINE_CENTRE_PIN A3 //Pin for the centre line sensor
#define LINE_RIGHT_PIN A2 //Pin for the right line sensor

LineSensor line_left(LINE_LEFT_PIN); //Create a line sensor object for the left sensor
LineSensor line_centre(LINE_CENTRE_PIN); //Create a line sensor object for the centre sensor
LineSensor line_right(LINE_RIGHT_PIN); //Create a line sensor object for the right sensor

float pL, pC, pR, LineCentre;
Kinematics kinematics(count_left, count_right);  // New kinematics class instance.
bool g_move_rotate;


volatile unsigned long timer3_count;
volatile long last_left = 0;
volatile long last_timer3 = 0;
volatile long timer3_speed_left = 0;
volatile long last_right = 0;
volatile long timer3_speed_right = 0;
unsigned long target_count = 1435;


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

  if (scalar_speed >= 70) {
    scalar_speed = 70;
  }

  else if (scalar_speed == 0) {
    scalar_speed = 0;
  }
  else if (scalar_speed <= 15) {
    scalar_speed = 15;
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


// Remember, setup only runs once.
void setup()
{
  last_time = millis();
  // These two function set up the pin
  // change interrupts for the encoders.
  setupEncoder0();
  setupEncoder1();
  setupTimer3(1);
  g_state = pre_line;
  g_move_rotate = false;


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
  moveMotor(RIGHT, 30);
  moveMotor(LEFT, 30);
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
  const int confidence_threshold = 0;
  //  Serial.print(left_val);
  //  Serial.print(", ");
  //  Serial.print(cent_val);
  //  Serial.print(", ");
  //  Serial.println(right_val);
  if (left_val >= 500) {
    confidence_value += 5;
  }
  else if (cent_val >= 500) {
    confidence_value += 10;
  }
  else if (right_val >= 500) {
    confidence_value += 5;
  }
  //  if (left_val >= 500 || cent_val >= 500 || right_val >= 500) {
  //    return true;
  //
  //  }
  if (left_val < 500 && cent_val < 500 && right_val < 500)
  {
    //    confidence_value = 0; // Kills movement, used for debugging and testing
    confidence_value -= 2000;
  }

  if (confidence_value >= confidence_threshold) {
    return true;
  }
  else {
    return false;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////

void moveDistance(float distance) {
  if (g_move_rotate) {
    static bool init = true;
    if (init) {
      count_left = 0;
      count_right = 0;
      init = !init;
    }
    int dir = 0;
    //distance *= 0.1;
    if (distance < 0) {
      dir = 1;
    }
    float encoder_count = (distance / CIRCUMFERENCE) * ONE_REVOLUTION;
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
        if (g_state == go_home) {
          moveMotor(LEFT, (SPEED + 1));
        }
        else {
          moveMotor(LEFT, SPEED);
        }

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

}

//////////////////////////////////////////////////////////////////////////////////////////////

void rotateDegrees(float degree) {
  if (!g_move_rotate) {
    static bool init = true;
    static int rotate_l, rotate_r;
    if (init) {
      rotate_l = count_left;
      rotate_r = count_right;
      init = !init;
    }
    int dir = 0;
    if (degree < 0) {
      degree *= -1;
      dir = 1;
    }
    float encoder_count = (degree / 360) * 2 * ONE_REVOLUTION;
    //  Serial.println(encoder_count);
    //  Serial.println(0.25 * 2 * ONE_REVOLUTION);
    if (dir == 0) {
      if (count_left <= encoder_count + rotate_l) {
        moveMotor(LEFT, SPEED);
      }
      else {
        moveMotor(LEFT, 0);
      }
      if (count_right >= -encoder_count - rotate_r) {
        moveMotor(RIGHT, -SPEED);
      }
      else {
        moveMotor(RIGHT, 0);
      }
      if (count_left >= encoder_count + rotate_l && count_right <= -encoder_count - rotate_r) {
        g_move_rotate = true;
      }
    }
    else {
      if (count_left >= -encoder_count - rotate_l) {
        moveMotor(LEFT, -SPEED);
      }
      else {
        moveMotor(LEFT, 0);
      }
      if (count_right <= encoder_count + rotate_r) {
        moveMotor(RIGHT, SPEED);
      }
      else {
        moveMotor(RIGHT, 0);
      }
      if (count_left <= -encoder_count - rotate_l && count_right >= encoder_count + rotate_r) {
        g_move_rotate = true;
      }
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

double getHomeAngle(Kinematics kinematics) {
  double dx, dy;
  dx = kinematics.m_x;
  dy = kinematics.m_y;
  return atan2(dy, dx);
}

double getHomeDistance(Kinematics kinematics) {
  double dx = kinematics.m_x;
  double dy = kinematics.m_y;
  double hype = (dx * dx) + (dy * dy);
  return sqrt(hype);
}

void goHome(Kinematics kinematics) {
  static double home_theta;// = getHomeAngle(kinematics);
  static bool init = true;
  if (!g_move_rotate) {
    kinematics.update(count_left, count_right);
  }
  if (init) {
    home_theta = getHomeAngle(kinematics);
    init = false;
  }
  static double home_distance;// = getHomeDistance(kinematics);
  float home_theta_degrees = home_theta * 180 / PI;
  static float speed_l;
  static float speed_r;

  //Serial.println(home_distance);
  //  Serial.println(home_distance * ONE_REVOLUTION / CIRCUMFERENCE);

  if (!g_move_rotate) {
    float head_output = head_pid.update(home_theta_degrees + 180, kinematics.m_theta);
    if (head_output < -0.1) {
      moveMotor(LEFT, head_output);
      moveMotor(RIGHT, -head_output);
    }
    else if (head_output > 0.1) {
      moveMotor(RIGHT, -head_output);
      moveMotor(LEFT, head_output);
    }
    else {
      stopMotor();
      g_move_rotate = !g_move_rotate;
      home_distance = getHomeDistance(kinematics);
      count_left = count_right = 0;
      kinematics.m_last_left = kinematics.m_last_right = 0;
      delay(1000);
    }
  }
  else {
    float output_l = left_pid.update(home_distance * ONE_REVOLUTION / CIRCUMFERENCE, count_left);
    float output_r = right_pid.update(home_distance * ONE_REVOLUTION / CIRCUMFERENCE, count_right);

    if (output_l < -5) {
      moveMotor(LEFT, 0);
    }
    else {
      //      float output_vel_l = left_vel_pid.update(1400, timer3_speed_left);
      //      speed_l += output_vel_l;
      moveMotor(LEFT, output_l);
      //      moveMotor(LEFT, 65);
    }

    if (output_r < -5) {
      moveMotor(RIGHT, 0);
    }
    else {
      //      float output_vel_r = right_vel_pid.update(1400, timer3_speed_right);
      //      speed_r += output_vel_r;
      moveMotor(RIGHT, output_r);
      //      moveMotor(RIGHT, 65);
    }
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////

// Remember, loop is called again and again.
void loop()
{
  delay(5);
  static bool onLine;
  static bool firstFound = false;
  static bool block = false;
  unsigned long elapsed_time, current_time;
  current_time = millis();
  elapsed_time = current_time - last_time;
  //  kinematics.update(count_left, count_right);
  //float head_output = head_pid.update(0,kinematics.m_theta);
  //  Serial.println(kinematics.m_theta);
  switch (g_state) {
    case pre_line:
      kinematics.update(count_left, count_right);
      firstFound = getFirstLine();
      if (firstFound) {
        g_state = on_line;
        kinematics.update(count_left, count_right);
      }
      break;
    case on_line:
      kinematics.update(count_left, count_right);
      onLine = checkForLine();
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
          if (output_line > 7.5) {
            moveMotor(LEFT, -30);
            moveMotor(RIGHT, 30);
          }
          else if (output_line < -7.5) {
            moveMotor(RIGHT, -30);
            moveMotor(LEFT, 30);
          }
          else {
            moveMotor(LEFT, 25);
            moveMotor(RIGHT, 25);
          }
          last_time = millis();
        }
      }
      else {
        stopMotor();
        //        digitalWrite(LED_BUILTIN, LOW);
        //        digitalWrite(LED_BUILTIN_TX, LOW);
        g_state = found_end_of_line;
        kinematics.update(count_left, count_right);
      } break;
    case found_end_of_line:
      kinematics.update(count_left, count_right);
      stopMotor();
      static int buz_count = 0;
      if (elapsed_time > 500) {
        if (buz_count < 2) {
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
    case go_home:
      goHome(kinematics);
      break;
  }



  // build your main code here.

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
