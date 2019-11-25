#include <Wire.h>
#include <LSM6.h>

LSM6 imu;

float weight = 0.6;
float old_output = 0.;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  if (!imu.init())
  {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  imu.enableDefault();
  imu.writeReg(LSM6::CTRL1_XL, 0b01011000); // 208 Hz, +/4 g
}

void loop()
{
  imu.read();

  float output = (weight * imu.a.x) + ((1-weight) * old_output);
  old_output = output;

  Serial.print("A: ");
  Serial.println(output);

  delay(100);
}
