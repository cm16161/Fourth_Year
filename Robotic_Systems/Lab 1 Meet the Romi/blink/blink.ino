#define BAUD_RATE 9600

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(6,OUTPUT);

  Serial.begin(BAUD_RATE);
  // Wait for stable connection, report reset.
  delay(1000);
  Serial.println("***RESET***");
  
}

void flashLEDs(int delayms){
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(delayms);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(delayms);                       // wait for a second
}

void beepBoop(int volume, int delayms){
  analogWrite(6,volume);
  delay(delayms);
  analogWrite(6,0);
}

int count = 0;
bool toggle = false; 
// the loop function runs over and over again forever
void loop() {
  
  if(toggle){
          flashLEDs(500);
        }
  if (Serial.available()) //This line checks whether there is anything to read
  {
      char inChar = Serial.read(); //This reads one byte
      
      if (inChar =='s')
      {
          Serial.println("Got s");
      }
      if(inChar == '1'){
        flashLEDs(500);
      }
      if(inChar == 'r'){
        toggle = !toggle;
      }
  }

//  for(count;count<5;count++){
//    flashLEDs(500);
//    Serial.println("Hello, from the Romi");
//    beepBoop(100,20);
//  }
}
