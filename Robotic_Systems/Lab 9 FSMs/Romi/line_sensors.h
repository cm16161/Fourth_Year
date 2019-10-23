#ifndef _Line_follow_h
#define _Line_follow_h

//Number of readings to take for calibration
const int NUM_CALIBRATIONS = 500;

/*
    Class to represent a single line sensor
*/
class LineSensor
{
  public:

    // Required function.
    LineSensor(int pin);   //Constructor

    // Suggested functions.
    void calibrate();       //Calibrate
    int read_raw();         //Return the uncalibrated value from the sensor
    int read_calibrated();  //Return the calibrated value from the sensor

    // You may wish to add other functions!
    // ...
    float calibrated_value;

  private:

    int pin;
    /*
       Add any variables needed for calibration here
    */

};


// Class Constructor:
// Sets pin passed in as argument to input
LineSensor::LineSensor(int Line_pin)
{
  pin = Line_pin;
  pinMode(pin, INPUT);
}

// Returns unmodified reading.
int LineSensor::read_raw()
{
  return analogRead(pin);
}

// Write this function to measure any
// systematic error in your sensor and
// set some bias values.
void LineSensor::calibrate()
{
  for (int i = 0; i < NUM_CALIBRATIONS;i++){
  calibrated_value += read_raw() / NUM_CALIBRATIONS;
  }
  Serial.println(calibrated_value);
  digitalWrite(6, HIGH);
  digitalWrite(6, LOW);

  /*
     Write code to calibrate your sensor here
  */
}


// Use the above bias values to return a
// compensated ("corrected") sensor reading.
int LineSensor::read_calibrated()
{
  int raw = analogRead(pin);
  int ret = raw - (int) calibrated_value;
  return ret;
  /*
     Write code to return a calibrated reading here
  */
}


#endif
