#include "process.h"
#include "imu.h"
#include <math.h>

float getheading(float mag_x, float mag_y, float mag_z, float pitch_radians, float roll_radians) {
  return atan2( (-mag_y*cos(roll_radians) + mag_z*sin(roll_radians) ) , (mag_x*cos(pitch_radians) + mag_y*sin(pitch_radians)*sin(roll_radians)+ mag_z*sin(pitch_radians)*cos(roll_radians)));
}

void get_pitch_roll(float *pitch, float *roll) {

  float x,y,z;
  read_accel(&x,&y,&z);
  *pitch = atan2(x, sqrt(y * y) + (z * z));
  *roll = atan2(y, sqrt(x * x) + (z * z));
  *pitch *= 180.0 / M_PI;
  *roll *= 180.0 / M_PI;

}
//http://www.pieter-jan.com/node/11

void comp(float accData[3], float gyrData[3], float *pitch, float *roll)
{
    float dt = .01;
    float pitchAcc, rollAcc;

    // Integrate the gyroscope data -> int(angularSpeed) = angle
    *pitch += ((float)gyrData[0]) * dt; // Angle around the X-axis
    *roll -= ((float)gyrData[1]) * dt;    // Angle around the Y-axis
    // Compensate for drift with accelerometer data if !bullshit
    int forceMagnitudeApprox = fabs(accData[0]) + fabs(accData[1]) + fabs(accData[2]);
    if (forceMagnitudeApprox > .5 && forceMagnitudeApprox < 2.0)
    {
  // Turning around the X axis results in a vector on the Y-axis
        pitchAcc = atan2f(accData[1], accData[2]) * 180 / M_PI;
        *pitch = *pitch * 0.98 + pitchAcc * 0.02;

  // Turning around the Y axis results in a vector on the X-axis
        rollAcc = atan2f((float)accData[0], (float)accData[2]) * 180 / M_PI;
        *roll = *roll * 0.98 + rollAcc * 0.02;
    }
}
