#define RED 0
#define YELLOW 1
#define GREEN 2

unsigned long last_timestamp;   // We will store a timestamp in this.
unsigned long last_timestamp_yellow;
unsigned long last_timestamp_green;
// Remember: Setup only runs once when the arduino is powered up.
void setup() {

  pinMode(LED_BUILTIN, OUTPUT);        // Setup up A0 as input to read.
  pinMode(LED_BUILTIN_RX, OUTPUT);
  pinMode(LED_BUILTIN_TX, OUTPUT);
  last_timestamp = millis();    // We set an intial timestamp value.
last_timestamp_yellow = millis();
last_timestamp_green = millis();

}

void flashLED(int on, int off1, int off2){
  digitalWrite(on, HIGH);
  digitalWrite(off1, LOW);
  digitalWrite(off2,LOW);
}

void chooseLED(int colour){
  switch(colour){
    case RED: flashLED(LED_BUILTIN_RX,LED_BUILTIN,LED_BUILTIN_TX); break;
    case YELLOW: flashLED(LED_BUILTIN,LED_BUILTIN_RX,LED_BUILTIN_TX);break;
    case GREEN:flashLED(LED_BUILTIN_TX,LED_BUILTIN,LED_BUILTIN_RX);break;
  }
}

// Remember: loop is called again and again.
void loop() {

    // Get how much time has passed right now.
    unsigned long time_now = millis();     

    // Work out how many milliseconds have gone passed by subtracting
    // our two timestamps.  time_now will always be bigger than the
    // time_of_read (except when millis() overflows after 50 days).
    unsigned long elapsed_time = time_now % 1500;



    // See if 10000 milliseconds have ellapsed
    // If not, this block is skipped.
    if( elapsed_time > 1000 ) { 
      chooseLED(RED);

        // Since 1000ms elapsed, we overwrite our last_timestamp with the current time
        // so that another 10000ms is needed to pass.
        
    }
    else if(elapsed_time>500){
      chooseLED(YELLOW);

    }
    else if (elapsed_time>0){
       chooseLED(GREEN);
    }
    

    // Code outside the above if{} will run on every loop!
    // Therefore code here is no longer stopped waiting for delay()

}
