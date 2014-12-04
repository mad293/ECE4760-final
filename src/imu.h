#ifndef _IMU_H_
#define _IMU_H_

#include <stdio.h>
/**
 * @brief Register to address mapping for the Gyroscope
 */
enum GYRO_CONTROL {
  WHO_AM_I_G=0x0F,
  CTRL_REG1_G=0x20,
  CTRL_REG2_G=0x21,
  CTRL_REG3_G=0x22,
  CTRL_REG4_G=0x23,
  CTRL_REG5_G=0x24,
  REFERENCE_G=0x25,
  STATUS_REG_G=0x27,
  OUT_X_L_G=0x28,
  OUT_X_H_G=0x29,
  OUT_Y_L_G=0x2A,
  OUT_Y_H_G=0x2B,
  OUT_Z_L_G=0x2C,
  OUT_Z_H_G=0x2D,
  FIFO_CTRL_REG_G=0x2E,
  FIFO_SRC_REG_G=0x2F,
  INT1_CFG_G=0x30,
  INT1_SRC_G=0x31,
  INT1_THS_XH_G=0x32,
  INT1_THS_XL_G=0x33,
  INT1_THS_YH_G=0x34,
  INT1_THS_YL_G=0x35,
  INT1_THS_ZH_G=0x36,
  INT1_THS_ZL_G=0x37,
  INT1_DURATION_G=0x38,
};


/**
 * @brief Register to address mapping for the accelometer / magnemeter
 */
enum XM_CONTROL {
  OUT_TEMP_L_XM=0x05,
  OUT_TEMP_H_XM=0x06,
  STATUS_REG_M=0x07,
  OUT_X_L_M=0x08,
  OUT_X_H_M=0x09,
  OUT_Y_L_M=0x0A,
  OUT_Y_H_M=0x0B,
  OUT_Z_L_M=0x0C,
  OUT_Z_H_M=0x0D,
  WHO_AM_I_XM=0x0F,
  INT_CTRL_REG_M=0x12,
  INT_SRC_REG_M=0x13,
  INT_THS_L_M=0x14,
  INT_THS_H_M=0x15,
  OFFSET_X_L_M=0x16,
  OFFSET_X_H_M=0x17,
  OFFSET_Y_L_M=0x18,
  OFFSET_Y_H_M=0x19,
  OFFSET_Z_L_M=0x1A,
  OFFSET_Z_H_M=0x1B,
  REFERENCE_X=0x1C,
  REFERENCE_Y=0x1D,
  REFERENCE_Z=0x1E,
  CTRL_REG0_XM=0x1F,
  CTRL_REG1_XM=0x20,
  CTRL_REG2_XM=0x21,
  CTRL_REG3_XM=0x22,
  CTRL_REG4_XM=0x23,
  CTRL_REG5_XM=0x24,
  CTRL_REG6_XM=0x25,
  CTRL_REG7_XM=0x26,
  STATUS_REG_A=0x27,
  OUT_X_L_A=0x28,
  OUT_X_H_A=0x29,
  OUT_Y_L_A=0x2A,
  OUT_Y_H_A=0x2B,
  OUT_Z_L_A=0x2C,
  OUT_Z_H_A=0x2D,
  FIFO_CTRL_REG=0x2E,
  FIFO_SRC_REG=0x2F,
  INT_GEN_1_REG=0x30,
  INT_GEN_1_SRC=0x31,
  INT_GEN_1_THS=0x32,
  INT_GEN_1_DURATION=0x33,
  INT_GEN_2_REG=0x34,
  INT_GEN_2_SRC=0x35,
  INT_GEN_2_THS=0x36,
  INT_GEN_2_DURATION=0x37,
  CLICK_CFG=0x38,
  CLICK_SRC=0x39,
  CLICK_THS=0x3A,
  TIME_LIMIT=0x3B,
  TIME_LATENCY=0x3C,
  TIME_WINDOW=0x3D,
  ACT_THS=0x3E,
  ACT_DUR=0x3F,
};

// gyro_scale defines the possible full-scale ranges of the gyroscope:
enum gyro_scale
{
  G_SCALE_245DPS,   // 00:  245 degrees per second
  G_SCALE_500DPS,   // 01:  500 dps
  G_SCALE_2000DPS,  // 10:  2000 dps
};
// accel_scale defines all possible FSR's of the accelerometer:
enum accel_scale
{
  A_SCALE_2G, // 000:  2g
  A_SCALE_4G, // 001:  4g
  A_SCALE_6G, // 010:  6g
  A_SCALE_8G, // 011:  8g
  A_SCALE_16G // 100:  16g
};
// mag_scale defines all possible FSR's of the magnetometer:
enum mag_scale
{
  M_SCALE_2GS,  // 00:  2Gs
  M_SCALE_4GS,  // 01:  4Gs
  M_SCALE_8GS,  // 10:  8Gs
  M_SCALE_12GS, // 11:  12Gs
};
// gyro_odr defines all possible data rate/bandwidth combos of the gyro:
enum gyro_odr
{             // ODR (Hz) --- Cutoff
  G_ODR_95_BW_125  = 0x0, //   95         12.5
  G_ODR_95_BW_25   = 0x1, //   95          25
  // 0x2 and 0x3 define the same data rate and bandwidth
  G_ODR_190_BW_125 = 0x4, //   190        12.5
  G_ODR_190_BW_25  = 0x5, //   190         25
  G_ODR_190_BW_50  = 0x6, //   190         50
  G_ODR_190_BW_70  = 0x7, //   190         70
  G_ODR_380_BW_20  = 0x8, //   380         20
  G_ODR_380_BW_25  = 0x9, //   380         25
  G_ODR_380_BW_50  = 0xA, //   380         50
  G_ODR_380_BW_100 = 0xB, //   380         100
  G_ODR_760_BW_30  = 0xC, //   760         30
  G_ODR_760_BW_35  = 0xD, //   760         35
  G_ODR_760_BW_50  = 0xE, //   760         50
  G_ODR_760_BW_100 = 0xF, //   760         100
};
// accel_oder defines all possible output data rates of the accelerometer:
enum accel_odr
{
  A_POWER_DOWN,   // Power-down mode (0x0)
  A_ODR_3125,   // 3.125 Hz (0x1)
  A_ODR_625,    // 6.25 Hz (0x2)
  A_ODR_125,    // 12.5 Hz (0x3)
  A_ODR_25,   // 25 Hz (0x4)
  A_ODR_50,   // 50 Hz (0x5)
  A_ODR_100,    // 100 Hz (0x6)
  A_ODR_200,    // 200 Hz (0x7)
  A_ODR_400,    // 400 Hz (0x8)
  A_ODR_800,    // 800 Hz (9)
  A_ODR_1600    // 1600 Hz (0xA)
};

// accel_abw defines all possible anti-aliasing filter rates of the accelerometer:
enum accel_abw
{
  A_ABW_773,    // 773 Hz (0x0)
  A_ABW_194,    // 194 Hz (0x1)
  A_ABW_362,    // 362 Hz (0x2)
  A_ABW_50,   //  50 Hz (0x3)
};


// mag_oder defines all possible output data rates of the magnetometer:
enum mag_odr
{
  M_ODR_3125, // 3.125 Hz (0x00)
  M_ODR_625,  // 6.25 Hz (0x01)
  M_ODR_125,  // 12.5 Hz (0x02)
  M_ODR_25, // 25 Hz (0x03)
  M_ODR_50, // 50 (0x04)
  M_ODR_100,  // 100 Hz (0x05)
};


enum gyro_scale gScale;
enum accel_scale aScale;
enum mag_scale mScale;

/**
 * @brief sets the aRes variable to the correct resolution of the accelometer readings
 */
void calcaRes();
/**
 * @brief sets the mRes variable to the correct resolution of the magenmeter readings
 */
void calcmRes();
/**
 * @brief sets the gRes variable to the correct resolution of the gyroscope readings
 */
void calcgRes();
/**
 * @brief initialize the gyroscope, must be done before reading from the gyroscope
 */
void initGyro();

/**
 * @brief set the data capture rate of the gyroscope see the enum gyro_odr to see what values this can take.
 *
 * @param gRate
 */
void setGyroODR(enum gyro_odr gRate);
/**
 * @brief set the scale of the gyroscope see enum gyro_Scale to see what values this can take.
 *
 * @param gScl
 */
void setGyroScale(enum gyro_scale gScl);

/**
 * @brief initilize the accelometer, this must be called before using any accelometer fucntions
 */
void initAccel(void);
/**
 * @brief see setGyroODR function
 *
 * @param aRate
 */
void setAccelODR(enum accel_odr aRate);
/**
 * @brief see the setGyroScle function
 *
 * @param aScl
 */
void setAccelScale(enum accel_scale aScl);

/**
 * @brief
 */
void initMag(void);
/**
 * @brief see the setGyroODR function
 *
 * @param mRate
 */
void setMagODR(enum mag_odr mRate);
/**
 * @brief see the setGyroScale function
 *
 * @param mScl
 */
void setMagScale(enum mag_scale mScl);

/**
 * @brief takes a raw gyro reading and returns the actual value
 *
 * @param gyro raw reading form gyroscope
 *
 * @return The adjusted value
 */
float calcGyro(int16_t gyro);
/**
 * @brief
 *
 * @param accel
 *
 * @return The adjusted value
 */
float calcAccel(int16_t accel);
/**
 * @brief
 *
 * @param mag
 *
 * @return The adjusted value
 *
 */
float calcMag(int16_t mag);


/**
 * @brief read multiple bytes from the imu
 *
 * @param address  address of the twi device to read from
 * @param subAddress register to read from
 * @param dest destination buffer must be of size >= count
 * @param count Number of bytes to read
 */
void imu_rbs(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count);
/**
 * @brief Read one byte from the device
 *
 * @param address i2c device address
 * @param subAddress Address of desired register
 *
 * @return The byte read from the device
 */
uint8_t imu_rb(uint8_t address, uint8_t subAddress);
/**
 * @brief Writes a byte to the register at the address specified by subAddress
 *
 * @param address i2c/twi address of the accelometer / gyroscope
 * @param subAddress The address of the register that will be written to
 * @param data The singular data byte that is to be written to the register specified
 */
void imu_wb(uint8_t address, uint8_t subAddress, uint8_t data);

float aRes;
float gRes;
float mRes;

enum i2c_address {
  LSM9DS0_XM=(0x1D<<1),
  LSM9DS0_G=(0x6B<<1)
};


/**
 * @brief  Initilizes the IMU and enables all 9 axes and set the respective scales and data rates
 *
 * @param gScl Gyro Scale
 * @param a_scl Accelometer Scale
 * @param m_scl Magnemeter Scale
 * @param g_odr Gyro data rate
 * @param a_odr accelometer data rate
 * @param m_odr magnemeter data rate
 *
 */
uint16_t
init_imu(enum gyro_scale gScl, enum accel_scale a_scl,enum  mag_scale m_scl,
    enum gyro_odr g_odr, enum accel_odr a_odr,enum mag_odr m_odr);

/**
 * @brief Reads the gyroscope and calculates the real value from the raw reading.
 *
 * @param x x-axis
 * @param y y-axis
 * @param z z-axis
 */
void read_gyro(float * x, float * y, float * z);
/**
 * @brief Reads the accelometer and calculates the real value from the raw reading.
 *
 * @param x x-axis
 * @param y y-axis
 * @param z z-axis
 */
void read_accel(float * x, float * y, float * z);
/**
 * @brief Reads the magnemeter and calculates the real value from the raw reading.
 *
 * @param x x-axis
 * @param y y-axis
 * @param z z-axis
 */
void read_mag(float * x, float * y, float * z);


/**
 * @brief Returns the temperature in Celcius. Will be 25 degrees, which is the optimal temparture for the gyroscope. Is used internally for self-calibration.

 *
 * @return The temperature in celcius,*/
int16_t readTemp();
#endif
