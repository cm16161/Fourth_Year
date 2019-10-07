// Pin definitions, to make the
// code easier to read.
#define L_PWM_PIN 10
#define L_DIR_PIN 16
#define R_PWM_PIN  9
#define R_DIR_PIN 15

// Variables to remember our
// motor speeds for Left and Right.
// Byte stores 0 to 255
float l_speed;
float r_speed;

// Setup, only runs once when powered on.
void setup() {

  // Set our motor driver pins as outputs.
  pinMode( L_PWM_PIN, OUTPUT );
  pinMode( L_DIR_PIN, OUTPUT );
  pinMode( R_PWM_PIN, OUTPUT );
  pinMode( R_DIR_PIN, OUTPUT );

  // Set initial direction for l and r
  // Which of these is foward, or backward?
  digitalWrite( L_DIR_PIN, LOW);
  digitalWrite( R_DIR_PIN, LOW);
  
  // Set initial l_speed and r_speed values.
  l_speed = 0;
  r_speed = 0;

  // Start up the Serial for debugging.
  Serial.begin(9600);
  delay(1000);
  // Print reset so we can catch any reset error.
  Serial.println(" ***Reset*** ");

}


// put your main code here, to run repeatedly:
void loop() {
    
  // Adjust speeds. e.g., increment by 4 on every loop()
  l_speed = l_speed + 1;
  r_speed = r_speed + 1;

  // Send speeds to pins, to motor drivers.
  analogWrite( L_PWM_PIN, l_speed );
  analogWrite( R_PWM_PIN, r_speed );
  Serial.println(l_speed);

  // Brief pause
  delay(1000);

}
