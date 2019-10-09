// Pin definitions, to make the
// code easier to read.
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


#define CAT(a, b) a ## b
#define DIR_PIN(pin) CAT(pin ,_DIR_PIN)
#define PWM_PIN(pin) CAT(pin ,_PWM_PIN)

// Variables to remember our
// motor speeds for Left and Right.
// Byte stores 0 to 255
float l_speed;
float r_speed;
float last_timestamp;
bool g_move_or_rotate = true;
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
  last_timestamp = millis();

}

void moveMotor(int pin, float scalar_speed){
  bool motor_direction;
  if(scalar_speed < 0) {
    motor_direction = true;
    scalar_speed *= -1;
  }
  else motor_direction = false;
  int dir, pwm;
  if(pin == LEFT){
    dir = DIR_PIN(L);
    pwm = PWM_PIN(L);
  }
  else if(pin == RIGHT){
    dir = DIR_PIN(R);
    pwm = PWM_PIN(R);
  }
  digitalWrite(dir, motor_direction);
  analogWrite(pwm, scalar_speed);
  
}

void stopMotor(){
   moveMotor(LEFT,0);
   moveMotor(RIGHT,0);
}

void moveInLine(float elapsed_time, float delay_duration, bool travel_direction){
  static bool travel =true;
  if(elapsed_time > delay_duration){
  if(travel && (travel_direction == FORWARD)){
      moveMotor(LEFT, SPEED);
      moveMotor(RIGHT,SPEED);
      travel = !travel;
    }
    else if(travel && (travel_direction == BACKWARD)){
      moveMotor(LEFT, -SPEED);
      moveMotor(RIGHT,-SPEED);
      travel = !travel;
    }
    else{
      stopMotor();
      travel = !travel;
      g_move_or_rotate = false;
      
    }
    last_timestamp = millis();  
  }
    
}

void moveBackAndForth(float elapsed_time, float delay_duration){
  static bool travel =false;
  static bool travel_direction = true;
  if(elapsed_time > delay_duration){
  if(travel && travel_direction){
      moveMotor(LEFT, SPEED);
      moveMotor(RIGHT,SPEED);
      travel_direction = !travel_direction;
    }
    else if(travel && !travel_direction){
      moveMotor(LEFT, -SPEED);
      moveMotor(RIGHT,-SPEED);
      travel_direction = !travel_direction;
    }
    else{
      stopMotor();
      
    }
    travel= !travel;
    last_timestamp = millis();  
  }
}

void rotateRomi(float elapsed_time, float delay_duration, int r_dir, float r_speed){
  static bool travel = true;
  if(elapsed_time > delay_duration){
    if(travel && (r_dir ==0)){
      moveMotor(LEFT, -r_speed);
      moveMotor(RIGHT,r_speed);
      travel=false;
    }
    else if(travel && (r_dir == 1)){
      moveMotor(LEFT, r_speed);
      moveMotor(RIGHT,-r_speed);
      travel=false;
    }
    else if(travel && (r_dir==2)){
      moveMotor(LEFT,-2*r_speed);
      moveMotor(RIGHT,2*r_speed);
      travel=false;
    }
    else{
      stopMotor();
      travel= true;
      g_move_or_rotate = true;
    }
    last_timestamp = millis();  
  }
}

void square(float elapsed_time){
  static bool move_or_rotate = true;
  if(move_or_rotate){
    moveInLine(elapsed_time, DELAY_DURATION,FORWARD);
    if(!g_move_or_rotate){
      move_or_rotate = !move_or_rotate;    
    }
  }
  
  else{
    rotateRomi(elapsed_time, NINETY_DEGREES,NINETY_R, NINETY_ROTATE_SPEED);
    if(g_move_or_rotate){
    move_or_rotate = !move_or_rotate;  
    } 
  }
}

// put your main code here, to run repeatedly:
void loop() {
  
  // Send speeds to pins, to motor drivers.
  float current_time, elapsed_time;
  current_time = millis();
  elapsed_time = current_time - last_timestamp;
  square(elapsed_time);
  //moveBackAndForth(elapsed_time, DELAY_DURATION);
  
  //rotateRomi(elapsed_time, NINETY_DEGREES, NINETY_R);
}
