

void get_pitch_roll(float * pitch, float * roll);
float getheading(float mag_x, float mag_y, float mag_z, float pitch_radians, float roll_radians);

void comp(float accData[3], float gyrData[3], float *pitch, float *roll);
