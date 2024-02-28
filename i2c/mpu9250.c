#include <stdint.h>
#include "mpu9250_i2c.h"
//#include "ADXL345_Accelerometer.h"
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48
#define MPU_9250_ADDRESS 0x68
void init_MPU9250(){
	initI2C0();
}

uint8_t getAccelerometer_ID() {
	return (readI2C0(0x53,0x00));
}

void SetPowerMode(unsigned char powerMode) {
	uint8_t i2c_data = readI2C0(0x53,0x2d);
	if (powerMode==1){
		i2c_data = i2c_data | (powerMode<<3);
	} else if (powerMode==0){
		i2c_data &= ~(1<<3);
	}
	i2c_data = i2c_data | (powerMode<<3);
	writeI2C0(0x53,0x2d,i2c_data);
}

int getAccelerationData(){

	return 0;
}

uint16_t getAcceleration_rawX(){
	uint8_t accelData1, accelData2;
    uint16_t rawX;
	accelData1=readI2C0(MPU_9250_ADDRESS,ACCEL_XOUT_H);
	accelData2=readI2C0(MPU_9250_ADDRESS,ACCEL_XOUT_L);
	rawX = (accelData2<<8)|accelData1;
	return (rawX);
}

uint16_t getAcceleration_rawY(){
	uint8_t accelData1, accelData2;
    uint16_t rawY;
	accelData1=readI2C0(MPU_9250_ADDRESS,ACCEL_YOUT_H);
	accelData2=readI2C0(MPU_9250_ADDRESS,ACCEL_XOUT_L);
	rawY = (accelData2<<8)|accelData1;
	return (rawY);
}

uint16_t getAcceleration_rawZ(){
	uint8_t accelData1, accelData2;
    uint16_t rawZ;
	accelData1=readI2C0(MPU_9250_ADDRESS,ACCEL_ZOUT_H);
	accelData2=readI2C0(MPU_9250_ADDRESS,ACCEL_ZOUT_L);
	rawZ = (accelData2<<8)|accelData1;
	return (rawZ);
}

uint16_t getGyroscope_rawX(){
	uint8_t gyroData1, gyroData2;
    uint16_t rawX;
	gyroData1=readI2C0(MPU_9250_ADDRESS,GYRO_XOUT_H);
	gyroData2=readI2C0(MPU_9250_ADDRESS,GYRO_XOUT_L);
	rawX = (gyroData2<<8)|gyroData1;
	return (rawX);
}
uint16_t getGyroscope_rawY(){
	uint8_t gyroData1, gyroData2;
    uint16_t rawY;
	gyroData1=readI2C0(MPU_9250_ADDRESS,GYRO_YOUT_H);
	gyroData2=readI2C0(MPU_9250_ADDRESS,GYRO_YOUT_L);
	rawY = (gyroData2<<8)|gyroData1;
	return (rawY);
}
uint16_t getGyroscope_rawZ(){
	uint8_t gyroData1, gyroData2;
    uint16_t rawZ;
	gyroData1=readI2C0(MPU_9250_ADDRESS,GYRO_ZOUT_H);
	gyroData2=readI2C0(MPU_9250_ADDRESS,GYRO_ZOUT_L);
	rawZ = (gyroData2<<8)|gyroData1;
	return (rawZ);
}

uint16_t getTemperature_raw(){
	uint8_t gyroData1, gyroData2;
    uint16_t temp;
	gyroData1=readI2C0(MPU_9250_ADDRESS,TEMP_OUT_H);
	gyroData2=readI2C0(MPU_9250_ADDRESS,TEMP_OUT_L);
	temp = (gyroData2<<8)|gyroData1;
	return (temp);
}
signed int getAcceleration_X(){
	signed int short raw = (signed int short) getAcceleration_rawX();
	signed int acceleration = (signed int)(((signed int)raw) * 3.9);
	return acceleration;
}

signed int getAcceleration_Y(){
	signed int short raw = (signed int short) getAcceleration_rawY();
	signed int acceleration = (signed int)(((signed int)raw) * 3.9);
	return acceleration;
}

signed int getAcceleration_Z(){
	signed int short raw = (signed int short) getAcceleration_rawZ();
	signed int acceleration = (signed int)(((signed int)raw) * 3.9);
	return acceleration;
}

signed int getGyroscope_X(){
	signed int short raw = (signed int short) getGyroscope_rawX();
	signed int acceleration = (signed int)(((signed int)raw) * 3.9);
	return acceleration;
}

signed int getGyroscope_Y(){
	signed int short raw = (signed int short) getGyroscope_rawY();
	signed int acceleration = (signed int)(((signed int)raw) * 3.9);
	return acceleration;
}

signed int getGyroscope_Z(){
	signed int short raw = (signed int short) getGyroscope_rawZ();
	signed int acceleration = (signed int)(((signed int)raw) * 3.9);
	return acceleration;
}
signed int getTemperature(){
	signed int short raw = (signed int short) getTemperature_raw();
	signed int acceleration = (signed int)(((signed int)raw) * 3.9);
	return acceleration;
}