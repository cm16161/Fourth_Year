// Global variable to remember the
// on/off state of the LED.  
volatile boolean DEBUG_LED_STATE = false;
volatile unsigned long time_count = 0;

// Remember: Setup only runs once when the arduino is powered up.
void setup() {
  
  // Enable pin 13 LED for debugging
  pinMode(13,OUTPUT );
  digitalWrite( 13, DEBUG_LED_STATE );
  
  // Start Serial monitor and print "reset"
  // so we know if the board is reseting
  // unexpectedly.
  Serial.begin(9600);
  delay(1500);
  Serial.println("***RESET***");

  // Setup our timer3
  setupTimer3(1);
}


// Remember: loop is called again and again.
void loop() {

  // Nothing to do here.
  delay(100);
}


// Routine to setupt timer3 to run 
// Routine to setup timer3 with a specified
// frequency - not finished, please complete.
 setupTimer3( int hertz ) {
  
  // disable global interrupts
  cli();          

  // Reset timer3 to a blank condition.
  // TCCR = Timer/Counter Control Register
  TCCR3A = 0;     // set entire TCCR3A register to 0
  TCvoidCR3B = 0;     // set entire TCCR3B register to 0
  
  // First, turn on CTC mode.  Timer3 will count up
  // and create an interrupt on a match to a value.
  // See table 14.4 in manual, it is mode 4.
  TCCR3B = TCCR3B | (1 << WGM32);

  // Set prescaler value.
  TCCR3B = TCCR3B | (1 << CS32);

  // Set Compare Match counter value
  OCR3A =  62500/hertz;


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

  // Invert LED state.
  DEBUG_LED_STATE = !DEBUG_LED_STATE;
  time_count++;
  // Enable/disable LED
  digitalWrite(13, DEBUG_LED_STATE);

}
