// Setting a #define is like a find-and-replace
#define BUZZER_PIN 6
#define BAUD_RATE 9600
#define N_INTERVALS 8

// Global variables.
unsigned long last_timestamp;
unsigned long current_delay = 0;
unsigned long intervals[N_INTERVALS] = {1000,500,100,600,200,500,1500,800};

// Remember, setup runs once when the romi is powered up
void setup() {
  

  //Start a serial connection
  Serial.begin(BAUD_RATE);
  // Wait for stable connection, report reset.
  delay(1000);
  Serial.println("***RESET***");
  last_timestamp = millis();


  // Set pin 6 (buzzer) to output.
  pinMode( BUZZER_PIN, OUTPUT);
  
  // Initialise your global variables.
  
}

int toggleBuzzer(){
  static bool toggle = false;
  toggle = !toggle;
  if(toggle){
    //analogWrite(BUZZER_PIN,2);
    digitalWrite(BUZZER_PIN,1); 
    return 0;
     }
  else{
    //analogWrite(BUZZER_PIN,0);
    digitalWrite(BUZZER_PIN,0);
    return 1;
  }
}

// Remember, loop repeats again and again.
void loop() {
static int array_index = 0;
int next;
  current_delay = intervals[array_index];
  unsigned long level = 200000;
  unsigned long current_time= micros();
  unsigned long elapsed_time = current_time - last_timestamp;
  if(elapsed_time > current_delay){
      next = toggleBuzzer();
      //last_timestamp=micros();
  }
  if(next && elapsed_time > level){
    array_index = (array_index+1)%N_INTERVALS;
    last_timestamp=micros();
  }
  if(elapsed_time > level){
    //array_index = (array_index +1) % N_INTERVALS;
  }
  Serial.print(array_index);
  Serial.print("->");
  Serial.println(intervals[array_index]);
  
  // Implement a millis() or micros() task block
  // to toggle the state of the buzzer.
}
