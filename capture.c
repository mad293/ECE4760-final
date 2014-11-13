#include "imu.h"





void avg_mag(float *x, float *y, float *z, char samples) {
  float tx,ty,tz;
  *x =0; *y=0; *z=0;
  float fsamp = (float)samples;
  for (char i = 0; i < samples; i++) {
    read_mag(&tx,&ty,&tz);
    *x = *x + (tx/fsamp);
    *y = *y + (ty/fsamp);
    *z = *z + (tz/fsamp);
  }
}
