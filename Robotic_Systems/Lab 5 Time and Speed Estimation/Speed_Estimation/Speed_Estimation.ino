// These #defines act like a find-and-replace
// in your code, and make your code more readable.
// Note that there is no #define for E0_B:.
// it's a non-standard pin, check out setupEncoder0().
#define E1_A_PIN  7
#define E1_B_PIN  23
#define E0_A_PIN  26
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

// Volatile Global variables used by Encoder ISR.
volatile long count_right; // used by encoder to count the rotation
volatile bool old_right_A;  // used by encoder to remember prior state of A
volatile bool old_right_B;  // used by encoder to remember prior state of B
volatile unsigned long timer3_count;

volatile long count_left; // used by encoder to count the rotation
volatile bool old_left_A;  // used by encoder to remember prior state of A
volatile bool old_left_B;  // used by encoder to remember prior state of B
float last_timestamp, last_change;
long measured_speed;
long last_count;
bool g_move_or_rotate = true;

volatile long last_left = 0;
volatile long last_timer3 = 0;
volatile long timer3_speed = 0;

volatile unsigned long e0_interval;
volatile unsigned long e0_last_time;

// Global volatile to hold the determined
// speed of wheel fixed to encoder0
volatile float e0_speed;



enum movement_profile {
  accelerate,
  stay,
  decelerate,
  dead
};

int index = 0;

// put your setup code here, to run once:
void setup() {

  // These two function set up the pin
  // change interrupts for the encoders.
  // If you want to know more, find them
  // at the end of this file.
  last_timestamp = micros();
  last_count = 0;
  measured_speed = 0;
  last_change = millis();

  // Setup our timer3
  setupTimer3(1);
  timer3_count = 0;

  pinMode( L_PWM_PIN, OUTPUT );
  pinMode( L_DIR_PIN, OUTPUT );
  pinMode( R_PWM_PIN, OUTPUT );
  pinMode( R_DIR_PIN, OUTPUT );

  // Set initial direction for l and r
  // Which of these is foward, or backward?
  digitalWrite( L_DIR_PIN, LOW);
  digitalWrite( R_DIR_PIN, LOW);

  setupEncoder0();
  setupEncoder1();


  // Initialise the Serial communication
  // so that we can inspect the values of
  // our encoder using the Monitor.
  Serial.begin( 9600 );
}

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

void velocityProfile(float elapsed_time, float duration) {
  static movement_profile state = accelerate;
  static int move_speed = 10;
  float current_time = millis();
  float change_split = current_time - last_change;
  if (state == accelerate) {
    if (change_split > 500) {
      move_speed += 10;
      last_change = millis();
      if (move_speed > 230) {
        move_speed = 230;
      }
      moveMotor(LEFT, move_speed);
      moveMotor(RIGHT, move_speed);
    }

  }
  if (state == decelerate) {
    if (change_split > 500) {
      move_speed -= 10;
      last_change = millis();
    }
    if (move_speed < 10) {
      move_speed = 10;
    }
    moveMotor(LEFT, move_speed);
    moveMotor(RIGHT, move_speed);
  }
  if (state == dead) {
    stopMotor();
  }
  if (elapsed_time > duration) {
    switch (state) {
      case accelerate: state = stay;         break;
      case       stay: state = decelerate; break;
      case decelerate: state = dead;       break;
      case       dead:                     break;
    }
    last_timestamp = millis();
    index = (index + 1) % 3;
  }
  Serial.println(move_speed);
}

void moveDistance(float distance) {
  distance *= 0.1;
  int encoder_count = (distance / CIRCUMFERENCE) * ONE_REVOLUTION;
  if (count_left <= encoder_count) {
    moveMotor(LEFT, SPEED);
  }
  if (count_right <= encoder_count) {
    moveMotor(RIGHT, SPEED);
  }

  if (count_left >= encoder_count && count_right >= encoder_count) {
    moveMotor(LEFT, 0);
    moveMotor(RIGHT, 0);
    g_move_or_rotate = false;
    count_left = 0;
    count_right = 0;
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
    if (count_right <= -encoder_count && count_left >= encoder_count) {
      g_move_or_rotate = true;
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
    if (count_left <= -encoder_count && count_right >= encoder_count) {
      g_move_or_rotate = true;
      count_left = 0;
      count_right = 0;
    }
  }

}

void square() {
  static bool move_or_rotate = true;
  static int count = 0;
  if (count >= 4) {
    stopMotor();
    return;
  }
  if (move_or_rotate) {
    moveDistance(100);
    if (!g_move_or_rotate) {
      move_or_rotate = !move_or_rotate;
      count++;
    }
  }

  else {
    rotateDegrees(90);
    if (g_move_or_rotate) {
      count_left = 0;
      count_right = 0;
      move_or_rotate = !move_or_rotate;
    }
  }
}

// put your main code here, to run repeatedly:
void loop() {

  // Output the count values for our encoders
  // with a comma seperation, which allows for
  // two lines to be drawn on the Plotter.
  //
  // NOTE: count_left and count_right values are now
  //       automatically updated by the ISR when
  //       the encoder pins change.
  //
  float durations[3] = {5000, 5000, 5000};
  float elapsed_time, current_time;
  static bool finished = false;
  current_time = micros();
  elapsed_time = current_time - last_timestamp;
  last_timestamp = current_time;
  long change_in_count = count_left - last_count;
  measured_speed = (change_in_count * 1000000) / (long)elapsed_time;
  moveMotor(LEFT, 2 * SPEED);
  //  Serial.print(current_time);
  //  Serial.print(", ");
  //  Serial.print(timer3_count);
  //  Serial.print("\n");
  moveMotor(RIGHT, 2 * SPEED);
  //  Serial.println(last_left);
  //  Serial.println(count_left);

  //  Serial.print(last_count);
  //  Serial.print(" -> ");
  //  Serial.print(count_left);
  //long timer3_speed = (count_left - last_left) / (timer3_count - last_timer3);
 
  Serial.print(e0_speed);
  Serial.print(" -> ");
  Serial.println(timer3_speed);
  //  Serial.print(" = ");
  //  Serial.println(change_in_count);
  //    Serial.print( e0_speed );
  //   Serial.print("\n");

  last_count = count_left;
  delay(1000);
}

// This ISR handles just Encoder 1
// ISR to read the Encoder1 Channel A and B pins
// and then look up based on  transition what kind of
// rotation must have occured.
ISR( INT6_vect ) {
  // First, Read in the new state of the encoder pins.
  // Standard pins, so standard read functions.
  boolean new_right_B = digitalRead( E1_B_PIN );
  boolean new_right_A = digitalRead( E1_A_PIN );

  // Some clever electronics combines the
  // signals and this XOR restores the
  // true value.
  new_right_A ^= new_right_B;

  // Create a bitwise representationright of our states
  // We do this by shifting the boolean value up by
  // the appropriate number of bits, as per our table
  // header:
  //
  // State :  (bit3)  (bit2)  (bit1)  (bit0)
  // State :  New A,  New B,  Old A,  Old B.
  byte state = 0;
  state = state | ( new_right_A  << 3 );
  state = state | ( new_right_B  << 2 );
  state = state | ( old_right_A  << 1 );
  state = state | ( old_right_B  << 0 );


  // This is an inefficient way of determining
  // the direction.  However it illustrates well
  // against the lecture slides.
  switch ( state ) {
    case 0:                 break; // No movement.
    case 1:  count_right--; break;  // anti-clockwise
    case 2:  count_right++; break;  // clockwise
    case 3:                 break;  // Invalid
    case 4:  count_right++; break;  // clockwise
    case 5:                 break;  // No movement.
    case 6:                 break;  // Invalid
    case 7:  count_right--; break;  // anti-clockwise
    case 8:  count_right--; break;  // anti-clockwise
    case 9:                 break;  // Invalid
    case 10:                break;  // No movement.
    case 11: count_right++; break;  // clockwise
    case 12:                break;  // Invalid
    case 13: count_right++; break;  // clockwise
    case 14: count_right--; break;  // anti-clockwise
    case 15:                break;  // No movement.
  }

  // Save current state as old state for next call.
  old_right_A = new_right_A;
  old_right_B = new_right_B;
  unsigned long current_time, elapsed_time;
  unsigned long change_in_count = count_right - last_count;
  current_time = micros();
  e0_interval = current_time - e0_last_time;
  e0_speed = (change_in_count * 1000000) / e0_interval;
  e0_last_time = micros();
  last_count = count_right;

}


// This ISR handles just Encoder 0
// ISR to read the Encoder0 Channel A and B pins
// and then look up based on  transition what kind of
// rotation must have occured.
ISR( PCINT0_vect ) {

  // First, Read in the new state of the encoder pins.

  // Mask for a specific pin from the port.
  // Non-standard pin, so we access the register
  // directly.
  // Reading just PINE would give us a number
  // composed of all 8 bits.  We want only bit 2.
  // B00000100 masks out all but bit 2
  boolean new_left_B = PINE & (1 << PINE2);
  //boolean newE0_B = PINE & B00000100;  // Does same as above.

  // Standard read fro the other pin.
  boolean new_left_A = digitalRead( E0_A_PIN ); // 26 the same as A8

  // Some clever electronics combines the
  // signals and this XOR restores the
  // true value.
  new_left_A ^= new_left_B;



  // Create a bitwise representation of our states
  // We do this by shifting the boolean value up by
  // the appropriate number of bits, as per our table
  // header:
  //
  // State :  (bit3)  (bit2)  (bit1)  (bit0)
  // State :  New A,  New B,  Old A,  Old B.
  byte state = 0;
  state = state | ( new_left_A  << 3 );
  state = state | ( new_left_B  << 2 );
  state = state | ( old_left_A  << 1 );
  state = state | ( old_left_B  << 0 );

  // This is an inefficient way of determining
  // the direction.  However it illustrates well
  // against the lecture slides.
  switch ( state ) {
    case 0:  break; // No movement.
    case 1:  count_left--; break;  // clockwise
    case 2:  count_left++; break;  // anti-clockwise
    case 3:                break;  // Invalid
    case 4:  count_left++; break;  // anti-clockwise
    case 5:                break;  // No movement.
    case 6:                break;  // Invalid
    case 7:  count_left--; break;  // clockwise
    case 8:  count_left--; break;  // clockwise
    case 9:                break;  // Invalid
    case 10:               break;  // No movement.
    case 11: count_left++; break;  // anti-clockwise
    case 12:               break;  // Invalid
    case 13: count_left++; break;  // anti-clockwise
    case 14: count_left--; break;  // clockwise?
    case 15:               break;  // No movement.
  }

  // Save current state as old state for next call.
  old_left_A = new_left_A;
  old_left_B = new_left_B;
}





/*
   This setup routine enables interrupts for
   encoder1.  The interrupt is automatically
   triggered when one of the encoder pin changes.
   This is really convenient!  It means we don't
   have to check the encoder manually.
*/
void setupEncoder1() {

  // Initialise our count value to 0.
  count_right = 0;

  // Initialise the prior A & B signals
  // to zero, we don't know what they were.
  old_right_A = 0;
  old_right_B = 0;

  // Setup pins for encoder 1
  pinMode( E1_A_PIN, INPUT );
  pinMode( E1_B_PIN, INPUT );

  // Now to set up PE6 as an external interupt (INT6), which means it can
  // have its own dedicated ISR vector INT6_vector

  // Page 90, 11.1.3 External Interrupt Mask Register – EIMSK
  // Disable external interrupts for INT6 first
  // Set INT6 bit low, preserve other bits
  EIMSK = EIMSK & ~(1 << INT6);
  //EIMSK = EIMSK & B1011111; // Same as above.

  // Page 89, 11.1.2 External Interrupt Control Register B – EICRB
  // Used to set up INT6 interrupt
  EICRB |= ( 1 << ISC60 );  // using header file names, push 1 to bit ISC60
  //EICRB |= B00010000; // does same as above

  // Page 90, 11.1.4 External Interrupt Flag Register – EIFR
  // Setting a 1 in bit 6 (INTF6) clears the interrupt flag.
  EIFR |= ( 1 << INTF6 );
  //EIFR |= B01000000;  // same as above

  // Now that we have set INT6 interrupt up, we can enable
  // the interrupt to happen
  // Page 90, 11.1.3 External Interrupt Mask Register – EIMSK
  // Disable external interrupts for INT6 first
  // Set INT6 bit high, preserve other bits
  EIMSK |= ( 1 << INT6 );
  //EIMSK |= B01000000; // Same as above

}

void setupEncoder0() {

  // Initialise our count value to 0.
  count_left = 0;

  // Initialise the prior A & B signals
  // to zero, we don't know what they were.
  old_left_A = 0;
  old_left_B = 0;

  // Setting up E0_PIN_B:
  // The Romi board uses the pin PE2 (port E, pin 2) which is
  // very unconventional.  It doesn't have a standard
  // arduino alias (like d6, or a5, for example).
  // We set it up here with direct register access
  // Writing a 0 to a DDR sets as input
  // DDRE = Data Direction Register (Port)E
  // We want pin PE2, which means bit 2 (counting from 0)
  // PE Register bits [ 7  6  5  4  3  2  1  0 ]
  // Binary mask      [ 1  1  1  1  1  0  1  1 ]
  //
  // By performing an & here, the 0 sets low, all 1's preserve
  // any previous state.
  DDRE = DDRE & ~(1 << DDE6);
  //DDRE = DDRE & B11111011; // Same as above.

  // We need to enable the pull up resistor for the pin
  // To do this, once a pin is set to input (as above)
  // You write a 1 to the bit in the output register
  PORTE = PORTE | (1 << PORTE2 );
  //PORTE = PORTE | 0B00000100;

  // Encoder0 uses conventional pin 26
  pinMode( E0_A_PIN, INPUT );
  digitalWrite( E0_A_PIN, HIGH ); // Encoder 0 xor

  // Enable pin-change interrupt on A8 (PB4) for encoder0, and disable other
  // pin-change interrupts.
  // Note, this register will normally create an interrupt a change to any pins
  // on the port, but we use PCMSK0 to set it only for PCINT4 which is A8 (PB4)
  // When we set these registers, the compiler will now look for a routine called
  // ISR( PCINT0_vect ) when it detects a change on the pin.  PCINT0 seems like a
  // mismatch to PCINT4, however there is only the one vector servicing a change
  // to all PCINT0->7 pins.
  // See Manual 11.1.5 Pin Change Interrupt Control Register - PCICR

  // Page 91, 11.1.5, Pin Change Interrupt Control Register
  // Disable interrupt first
  PCICR = PCICR & ~( 1 << PCIE0 );
  // PCICR &= B11111110;  // Same as above

  // 11.1.7 Pin Change Mask Register 0 – PCMSK0
  PCMSK0 |= (1 << PCINT4);

  // Page 91, 11.1.6 Pin Change Interrupt Flag Register – PCIFR
  PCIFR |= (1 << PCIF0);  // Clear its interrupt flag by writing a 1.

  // Enable
  PCICR |= (1 << PCIE0);
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
  float delta_c = count_left - last_left;
   timer3_speed = delta_c/time_diff;
  last_left = count_left;
  last_timer3 = timer3_count;

}
