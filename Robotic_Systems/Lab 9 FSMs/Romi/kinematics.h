#ifndef _Kinematics
#define _Kinematics_h
#define PI 3.141592653589793

//You may want to use some/all of these variables
const float WHEEL_DIAMETER    = 70;
const float WHEEL_RADIUS      = 35;
const float WHEEL_SEPERATION  = 142.5;
const float GEAR_RATIO        = 12;
const float WHEEL_CIRCUMFERENCE = 220;
//const float COUNTS_PER_SHAFT_REVOLUTION = ??;
const float COUNTS_PER_WHEEL_REVOLUTION =  1440;
//const float COUNTS_PER_MM               = ??;

class Kinematics
{
  public:

    Kinematics(int count_left, int count_right);   // Constructor, required.
    int m_last_left;
    int m_last_right;
    float m_distance_travelled;
    float m_theta;
    double m_x, m_y;
    // Write your method functions:
    // ...
    void update(int count_left, int count_right);

  private:

    //Private variables and methods go here

};


// Required constructor.  Initialise variables.
Kinematics::Kinematics(int count_left, int count_right) {
  m_last_left = count_left;
  m_last_right = count_right;
  m_distance_travelled = 0;
  m_theta = 0;
}

void Kinematics::update(int count_left, int count_right) {
  float delta_l = count_left - m_last_left;
  float delta_r = count_right - m_last_right;
  m_last_left = count_left;
  m_last_right = count_right;
  float d_l = delta_l * WHEEL_CIRCUMFERENCE / COUNTS_PER_WHEEL_REVOLUTION;
  float d_r = delta_r * WHEEL_CIRCUMFERENCE / COUNTS_PER_WHEEL_REVOLUTION;
  float delta_d = d_l - d_r;
  m_theta += (delta_d / WHEEL_SEPERATION) * 180 / PI;
  //m_theta = (float)((int)m_theta% 360);
  float mean_distance = (delta_l + delta_r) / 2;
  float mean_distance_mm = mean_distance * WHEEL_CIRCUMFERENCE / COUNTS_PER_WHEEL_REVOLUTION;
  m_x += mean_distance_mm * cos(m_theta * PI / 180);
  m_y += mean_distance_mm * sin(m_theta * PI / 180);
//  float mm_distance = (mean_distance * WHEEL_CIRCUMFERENCE) / COUNTS_PER_WHEEL_REVOLUTION;
//  m_distance_travelled += mm_distance;
  //  Serial.print(m_distance_travelled);
  //  Serial.print(", ");
  //  Serial.println(m_theta);



  return;
}

#endif
