#include <avr/io.h>
#include "i2cmaster.h"
#include "imu.h"


#define ga (LSM9DS0_G)
#define xma (LSM9DS0_XM)


#define g_rb(x) (imu_rb(ga,x))
#define g_rbs(x,y,z) (imu_rbs(ga,x,y,z))
#define g_wb(x,y) (imu_wb(ga,x,y))
#define xm_rb(x) (imu_rb(xma,x))
#define xm_rbs(x,y,z) (imu_rbs(xma,x,y,z))
#define xm_wb(x,y) (imu_wb(xma,x,y))
#define multiple(x) (x | 0x80)

int16_t readTemp()
{
  uint8_t temp[2]; // We'll read two bytes from the temperature sensor into temp  
  xm_rbs(OUT_TEMP_L_XM, temp, 2); // Read 2 bytes, beginning at OUT_TEMP_L_M
  return  (((int16_t) temp[1] << 12) | temp[0] << 4 ) >> 4; // Temperature is a 12-bit signed integer
}


uint16_t init_imu(enum gyro_scale gScl, enum accel_scale aScl,enum  mag_scale mScl,
    enum gyro_odr gODR,enum  accel_odr aODR,enum  mag_odr mODR)
{

  i2c_init();
  // Store the given scales in class variables. These scale variables
  // are used throughout to calculate the actual g's, DPS,and Gs's.
  gScale = gScl;
  aScale = aScl;
  mScale = mScl;

  // Once we have the scale values, we can calculate the resolution
  // of each sensor. That's what these functions are for. One for each sensor
  calcgRes(); // Calculate DPS / ADC tick, stored in gRes variable
  calcmRes(); // Calculate Gs / ADC tick, stored in mRes variable
  calcaRes(); // Calculate g / ADC tick, stored in aRes variable
  uint8_t gTest = g_rb(WHO_AM_I_G);    // Read the gyro WHO_AM_I
  uint8_t xmTest = xm_rb(WHO_AM_I_XM); // Read the accel/mag WHO_AM_I

  if (gTest != 0xD4 || xmTest != 0x49)
  {
    while(1);
  }

    // Gyro initialization stuff:
    initGyro(); // This will "turn on" the gyro. Setting up interrupts, etc.
    setGyroODR(gODR); // Set the gyro output data rate and bandwidth.
    setGyroScale(gScale); // Set the gyro range

    // Accelerometer initialization stuff:
    initAccel(); // "Turn on" all axes of the accel. Set up interrupts, etc.
    setAccelODR(aODR); // Set the accel data rate.
    setAccelScale(aScale); // Set the accel range.

    // Magnetometer initialization stuff:
    initMag(); // "Turn on" all axes of the mag. Set up interrupts, etc.
    setMagODR(mODR); // Set the magnetometer output data rate.
    setMagScale(mScale); // Set the magnetometer's range.

    // Once everything is initialized, return the WHO_AM_I registers we read:
    return 0;// (xmTest << 8) | gTest;
}

void initGyro()
{
  /* CTRL_REG1_G sets output data rate, bandwidth, power-down and enables
     Bits[7:0]: DR1 DR0 BW1 BW0 PD Zen Xen Yen
     DR[1:0] - Output data rate selection
     00=95Hz, 01=190Hz, 10=380Hz, 11=760Hz
     BW[1:0] - Bandwidth selection (sets cutoff frequency)
     Value depends on ODR. See datasheet table 21.
     PD - Power down enable (0=power down mode, 1=normal or sleep mode)
     Zen, Xen, Yen - Axis enable (o=disabled, 1=enabled) */
  g_wb(CTRL_REG1_G, 0x0F); // Normal mode, enable all axes

  /* CTRL_REG2_G sets up the HPF
     Bits[7:0]: 0 0 HPM1 HPM0 HPCF3 HPCF2 HPCF1 HPCF0
     HPM[1:0] - High pass filter mode selection
     00=normal (reset reading HP_RESET_FILTER, 01=ref signal for filtering,
     10=normal, 11=autoreset on interrupt
     HPCF[3:0] - High pass filter cutoff frequency
     Value depends on data rate. See datasheet table 26.
   */
  g_wb(CTRL_REG2_G, 0x00); // Normal mode, high cutoff frequency

  /* CTRL_REG3_G sets up interrupt and DRDY_G pins
     Bits[7:0]: I1_IINT1 I1_BOOT H_LACTIVE PP_OD I2_DRDY I2_WTM I2_ORUN I2_EMPTY
     I1_INT1 - Interrupt enable on INT_G pin (0=disable, 1=enable)
     I1_BOOT - Boot status available on INT_G (0=disable, 1=enable)
     H_LACTIVE - Interrupt active configuration on INT_G (0:high, 1:low)
     PP_OD - Push-pull/open-drain (0=push-pull, 1=open-drain)
     I2_DRDY - Data ready on DRDY_G (0=disable, 1=enable)
     I2_WTM - FIFO watermark interrupt on DRDY_G (0=disable 1=enable)
     I2_ORUN - FIFO overrun interrupt on DRDY_G (0=disable 1=enable)
     I2_EMPTY - FIFO empty interrupt on DRDY_G (0=disable 1=enable) */
  // Int1 enabled (pp, active low), data read on DRDY_G:
  g_wb(CTRL_REG3_G, 0x88);

  /* CTRL_REG4_G sets the scale, update mode
     Bits[7:0] - BDU BLE FS1 FS0 - ST1 ST0 SIM
     BDU - Block data update (0=continuous, 1=output not updated until read
     BLE - Big/little endian (0=data LSB @ lower address, 1=LSB @ higher add)
     FS[1:0] - Full-scale selection
     00=245dps, 01=500dps, 10=2000dps, 11=2000dps
     ST[1:0] - Self-test enable
     00=disabled, 01=st 0 (x+, y-, z-), 10=undefined, 11=st 1 (x-, y+, z+)
     SIM - SPI serial interface mode select
     0=4 wire, 1=3 wire */
  g_wb(CTRL_REG4_G, 0x00); // Set scale to 245 dps

  /* CTRL_REG5_G sets up the FIFO, HPF, and INT1
     Bits[7:0] - BOOT FIFO_EN - HPen INT1_Sel1 INT1_Sel0 Out_Sel1 Out_Sel0
     BOOT - Reboot memory content (0=normal, 1=reboot)
     FIFO_EN - FIFO enable (0=disable, 1=enable)
     HPen - HPF enable (0=disable, 1=enable)
     INT1_Sel[1:0] - Int 1 selection configuration
     Out_Sel[1:0] - Out selection configuration */
  g_wb(CTRL_REG5_G, 0x00);


}
void assert(char v) {
  if (!v) {
    while(1);
  } 

}
void initAccel()
{
  xm_wb(CTRL_REG0_XM, 0x00);
  xm_wb(CTRL_REG1_XM,0x57); // 100Hz data rate, x/y/z all enabled
  xm_wb(CTRL_REG2_XM, 0x00); // Set scale to 2g
  xm_wb(CTRL_REG3_XM, 0x04);

  assert(xm_rb(CTRL_REG0_XM)== 0x00);
  assert(xm_rb(CTRL_REG1_XM)==0x57); // 100Hz data rate)== x/y/z all enabled
  assert(xm_rb(CTRL_REG2_XM)== 0x00); // Set scale to 2g
  assert(xm_rb(CTRL_REG3_XM)== 0x04);
}

void initMag()
{
  /* CTRL_REG5_XM enables temp sensor, sets mag resolution and data rate
     Bits (7-0): TEMP_EN M_RES1 M_RES0 M_ODR2 M_ODR1 M_ODR0 LIR2 LIR1
     TEMP_EN - Enable temperature sensor (0=disabled, 1=enabled)
     M_RES[1:0] - Magnetometer resolution select (0=low, 3=high)
     M_ODR[2:0] - Magnetometer data rate select
     000=3.125Hz, 001=6.25Hz, 010=12.5Hz, 011=25Hz, 100=50Hz, 101=100Hz
     LIR2 - Latch interrupt request on INT2_SRC (cleared by reading INT2_SRC)
     0=interrupt request not latched, 1=interrupt request latched
     LIR1 - Latch interrupt request on INT1_SRC (cleared by readging INT1_SRC)
     0=irq not latched, 1=irq latched                   */
  xm_wb(CTRL_REG5_XM, 0x94); // Mag data rate - 100 Hz, enable temperature sensor

  /* CTRL_REG6_XM sets the magnetometer full-scale
     Bits (7-0): 0 MFS1 MFS0 0 0 0 0 0
     MFS[1:0] - Magnetic full-scale selection
00:+/-2Gauss, 01:+/-4Gs, 10:+/-8Gs, 11:+/-12Gs               */
  xm_wb(CTRL_REG6_XM, 0x00); // Mag scale to +/- 2GS

  /* CTRL_REG7_XM sets magnetic sensor mode, low power mode, and filters
     AHPM1 AHPM0 AFDS 0 0 MLP MD1 MD0
     AHPM[1:0] - HPF mode selection
     00=normal (resets reference registers), 01=reference signal for filtering,
     10=normal, 11=autoreset on interrupt event
     AFDS - Filtered acceleration data selection
     0=internal filter bypassed, 1=data from internal filter sent to FIFO
     MLP - Magnetic data low-power mode
     0=data rate is set by M_ODR bits in CTRL_REG5
     1=data rate is set to 3.125Hz
     MD[1:0] - Magnetic sensor mode selection (default 10)
     00=continuous-conversion, 01=single-conversion, 10 and 11=power-down */
  xm_wb(CTRL_REG7_XM, 0x00); // Continuous conversion mode

  /* CTRL_REG4_XM is used to set interrupt generators on INT2_XM
     Bits (7-0): P2_TAP P2_INT1 P2_INT2 P2_INTM P2_DRDYA P2_DRDYM P2_Overrun P2_WTM
   */
  xm_wb(CTRL_REG4_XM, 0x04); // Magnetometer data ready on INT2_XM (0x08)

  /* INT_CTRL_REG_M to set push-pull/open drain, and active-low/high
     Bits[7:0] - XMIEN YMIEN ZMIEN PP_OD IEA IEL 4D MIEN
     XMIEN, YMIEN, ZMIEN - Enable interrupt recognition on axis for mag data
     PP_OD - Push-pull/open-drain interrupt configuration (0=push-pull, 1=od)
     IEA - Interrupt polarity for accel and magneto
     0=active-low, 1=active-high
     IEL - Latch interrupt request for accel and magneto
     0=irq not latched, 1=irq latched
     4D - 4D enable. 4D detection is enabled when 6D bit in INT_GEN1_REG is set
     MIEN - Enable interrupt generation for magnetic data
     0=disable, 1=enable) */
  xm_wb(INT_CTRL_REG_M, 0x09); // Enable interrupts for mag, active-low, push-pull
}

void readAccel(int16_t * ax, int16_t * ay, int16_t * az)
{
  uint8_t temp[6]; // We'll read six bytes from the accelerometer into temp
  xm_rbs(OUT_X_L_A, temp, 6); // Read 6 bytes, beginning at OUT_X_L_A
  *ax = (temp[1] << 8) | temp[0]; // Store x-axis values into ax
  *ay = (temp[3] << 8) | temp[2]; // Store y-axis values into ay
  *az = (temp[5] << 8) | temp[4]; // Store z-axis values into az
}

void readMag(int16_t * mx, int16_t * my, int16_t * mz)
{
  uint8_t temp[6]; // We'll read six bytes from the mag into temp
  xm_rbs(OUT_X_L_M, temp, 6); // Read 6 bytes, beginning at OUT_X_L_M
  *mx = (temp[1] << 8) | temp[0]; // Store x-axis values into mx
  *my = (temp[3] << 8) | temp[2]; // Store y-axis values into my
  *mz = (temp[5] << 8) | temp[4]; // Store z-axis values into mz
}

void readGyro(int16_t * gx, int16_t * gy, int16_t * gz)
{
  uint8_t temp[6]; // We'll read six bytes from the gyro into temp
  g_rbs(OUT_X_L_G, temp, 6); // Read 6 bytes, beginning at OUT_X_L_G
  *gx = (temp[1] << 8) | temp[0]; // Store x-axis values into gx
  *gy = (temp[3] << 8) | temp[2]; // Store y-axis values into gy
  *gz = (temp[5] << 8) | temp[4]; // Store z-axis values into gz
}


void read_gyro(float * x, float * y, float * z) {
  int16_t xd, yd, zd;
  readGyro(&xd,&yd,&zd);
  *x = calcGyro(xd);
  *y = calcGyro(yd);
  *z = calcGyro(zd);
}

void read_accel(float * x, float * y, float * z) {
  int16_t xd, yd, zd;
  readAccel(&xd,&yd,&zd);
  *x = calcAccel(xd);
  *y = calcAccel(yd);
  *z = calcAccel(zd);
}

void read_mag(float * x, float * y, float * z) {
  int16_t xd, yd, zd;
  readMag(&xd,&yd,&zd);
  *x = calcMag(xd);
  *y = calcMag(yd);
  *z = calcMag(zd);
}



float calcGyro(int16_t gyro)
{
  // Return the gyro raw reading times our pre-calculated DPS / (ADC tick):
  return gRes * gyro;
}

float calcAccel(int16_t accel)
{
  // Return the accel raw reading times our pre-calculated g's / (ADC tick):
  return aRes * accel;
}

float calcMag(int16_t mag)
{
  // Return the mag raw reading times our pre-calculated Gs / (ADC tick):
  return mRes * mag;
}

void setGyroScale(enum gyro_scale gScl)
{
  // We need to preserve the other bytes in CTRL_REG4_G. So, first read it:
  uint8_t temp = g_rb(CTRL_REG4_G);
  // Then mask out the gyro scale bits:
  temp &= 0xFF^(0x3 << 4);
  // Then shift in our new scale bits:
  temp |= gScl << 4;
  // And write the new register value back into CTRL_REG4_G:
  g_wb(CTRL_REG4_G, temp);

  // We've updated the sensor, but we also need to update our class variables
  // First update gScale:
  gScale = gScl;
  // Then calculate a new gRes, which relies on gScale being set correctly:
  calcgRes();
}

void setAccelScale(enum accel_scale aScl)
{
  //printf("new scale = %d\n",aScl);
  // We need to preserve the other bytes in CTRL_REG2_XM. So, first read it:
  uint8_t temp = xm_rb(CTRL_REG2_XM);
  // Then mask out the accel scale bits:
  temp &= 0xFF^(0x3 << 3);
  // Then shift in our new scale bits:
  temp |= aScl << 3;
  // And write the new register value back into CTRL_REG2_XM:
  xm_wb(CTRL_REG2_XM, temp);

  // We've updated the sensor, but we also need to update our class variables
  // First update aScale:
  aScale = aScl;
  // Then calculate a new aRes, which relies on aScale being set correctly:
  calcaRes();
}

void setMagScale(enum mag_scale mScl)
{
  // We need to preserve the other bytes in CTRL_REG6_XM. So, first read it:
  uint8_t temp = xm_rb(CTRL_REG6_XM);
  // Then mask out the mag scale bits:
  temp &= 0xFF^(0x3 << 5);
  // Then shift in our new scale bits:
  temp |= mScl << 5;
  // And write the new register value back into CTRL_REG6_XM:
  xm_wb(CTRL_REG6_XM, temp);

  // We've updated the sensor, but we also need to update our class variables
  // First update mScale:
  mScale = mScl;
  // Then calculate a new mRes, which relies on mScale being set correctly:
  calcmRes();
}

void setGyroODR(enum gyro_odr gRate)
{
  // We need to preserve the other bytes in CTRL_REG1_G. So, first read it:
  uint8_t temp = g_rb(CTRL_REG1_G);
  // Then mask out the gyro ODR bits:
  temp &= 0xFF^(0xF << 4);
  // Then shift in our new ODR bits:
  temp |= (gRate << 4);
  // And write the new register value back into CTRL_REG1_G:
  g_wb(CTRL_REG1_G, temp);
}
void setAccelODR(enum accel_odr aRate)
{
  // We need to preserve the other bytes in CTRL_REG1_XM. So, first read it:
  uint8_t temp = xm_rb(CTRL_REG1_XM);
  // Then mask out the accel ODR bits:
  temp &= 0xFF^(0xF << 4);
  // Then shift in our new ODR bits:
  temp |= (aRate << 4);
  // And write the new register value back into CTRL_REG1_XM:
  xm_wb(CTRL_REG1_XM, temp);
}
void setAccelABW(enum accel_abw abwRate)
{
  // We need to preserve the other bytes in CTRL_REG2_XM. So, first read it:
  uint8_t temp = xm_rb(CTRL_REG2_XM);
  // Then mask out the accel ABW bits:
  temp &= 0xFF^(0x3 << 7);
  // Then shift in our new ODR bits:
  temp |= (abwRate << 7);
  // And write the new register value back into CTRL_REG2_XM:
  xm_wb(CTRL_REG2_XM, temp);
}

void setMagODR(enum mag_odr mRate)
{
  // We need to preserve the other bytes in CTRL_REG5_XM. So, first read it:
  uint8_t temp = xm_rb(CTRL_REG5_XM);
  // Then mask out the mag ODR bits:
  temp &= 0xFF^(0x7 << 2);
  // Then shift in our new ODR bits:
  temp |= (mRate << 2);
  // And write the new register value back into CTRL_REG5_XM:
  xm_wb(CTRL_REG5_XM, temp);
}

void calcgRes()
{
  // Possible gyro scales (and their register bit settings) are:
  // 245 DPS (00), 500 DPS (01), 2000 DPS (10). Here's a bit of an algorithm
  // to calculate DPS/(ADC tick) based on that 2-bit value:
  switch (gScale)
  {
    case G_SCALE_245DPS:
      gRes = 245.0 / 32768.0;
      break;
    case G_SCALE_500DPS:
      gRes = 500.0 / 32768.0;
      break;
    case G_SCALE_2000DPS:
      gRes = 2000.0 / 32768.0;
      break;
  }
}

void calcaRes()
{
  // Possible accelerometer scales (and their register bit settings) are:
  // 2 g (000), 4g (001), 6g (010) 8g (011), 16g (100). Here's a bit of an
  // algorithm to calculate g/(ADC tick) based on that 3-bit value:
  aRes = aScale == A_SCALE_16G ? 16.0 / 32768.0 :
    (((float) aScale + 1.0) * 2.0) / 32768.0;
}

void calcmRes()
{
  // Possible magnetometer scales (and their register bit settings) are:
  // 2 Gs (00), 4 Gs (01), 8 Gs (10) 12 Gs (11). Here's a bit of an algorithm
  // to calculate Gs/(ADC tick) based on that 2-bit value:
  mRes = mScale == M_SCALE_2GS ? 2.0 / 32768.0 :
    (float) (mScale << 2) / 32768.0;
}


void imu_wb(uint8_t address, uint8_t subAddress, uint8_t data)
{
  if (address == (0x1D<<1)) {
  }
  i2c_start((address)+I2C_WRITE);
  i2c_write(subAddress);
  i2c_write(data);
  i2c_stop();
}

uint8_t imu_rb(uint8_t address, uint8_t subAddress)
{
  uint8_t data; // `data` will store the register data
  i2c_start((address)+I2C_WRITE);
  i2c_write(subAddress);                  // Put slave register address in Tx buffer
  i2c_rep_start((address)+I2C_READ);
  data = i2c_readNak();                      // Fill Rx buffer with result
  i2c_stop();
  return data;                             // Return data read from slave register
}

void imu_rbs(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count)
{

  if (address == (0x1D<<1)&& subAddress == OUT_X_L_A) {
  }
  i2c_start((address)+I2C_WRITE);

  i2c_write(multiple(subAddress));

  i2c_rep_start((address)+I2C_READ); // Restart the signal
  uint8_t i = 0;

  for (i = 0; i < count; i++) {
    if(i == (count-1)) {
      dest[i] = i2c_readNak();
    } else {
      dest[i] = i2c_readAck();
    }
  }
  i2c_stop();

}
