#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "inc/hw_ints.h"

#include "inc/hw_i2c.h"
#include "driverlib/i2c.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"






void initI2C0(void)
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

	SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	// Configure the pin muxing for I2C0 functions on port B2 and B3.
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);

	// Select the I2C function for these pins.
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
	I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

	//clear I2C FIFOs
	HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
}

uint8_t readI2C0(uint16_t device_address, uint16_t device_register)
{
	I2CMasterSlaveAddrSet(I2C0_BASE, device_address, false);
	I2CMasterDataPut(I2C0_BASE, device_register);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
	while(I2CMasterBusy(I2C0_BASE));
	I2CMasterSlaveAddrSet(I2C0_BASE, device_address, true);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
	while(I2CMasterBusy(I2C0_BASE));
	return( I2CMasterDataGet(I2C0_BASE));
}

void writeI2C0(uint16_t device_address, uint16_t device_register, uint8_t device_data)
{
	I2CMasterSlaveAddrSet(I2C0_BASE, device_address, false);
	I2CMasterDataPut(I2C0_BASE, device_register);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C0_BASE));

	I2CMasterSlaveAddrSet(I2C0_BASE, device_address, true);
	I2CMasterDataPut(I2C0_BASE, device_data);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

	while(I2CMasterBusy(I2C0_BASE));
}
void MPU_9250_INIT()
{
	writeI2C0(MPU_9250_ADDRESS,0x6B, 0x00);
	writeI2C0(MPU_9250_ADDRESS, SMPLRT_DIV, 7);
	writeI2C0(MPU_9250_ADDRESS, PWR_MAGNT_1, 1);
	writeI2C0(MPU_9250_ADDRESS, CONFIG, 0);
	writeI2C0(MPU_9250_ADDRESS, GYRO_CONFIG, 24);
	writeI2C0(MPU_9250_ADDRESS, INT_ENABLE, 1);
}

int16_t read_raw_data(uint16_t addr)
{
	uint16_t high, low, value;
	high = readI2C0(MPU_9250_ADDRESS, addr);
	low = readI2C0(MPU_9250_ADDRESS, addr+1);
	value = ((high<<8)|low);

	if(value > 32768)
	{
		value = value - 65536;

	}
	return value;
}