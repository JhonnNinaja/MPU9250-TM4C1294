
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
#include "inc/hw_types.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
uint32_t g_ui32SysClock;
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
#define SMPLRT_DIV 0x19
#define PWR_MAGNT_1 0x6B
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define INT_ENABLE 0x38

int16_t Accel_X, Accel_Y, Accel_Z;
int16_t Gyro_X, Gyro_Y, Gyro_Z;
float Ax, Ay, Az, Gx, Gy, Gz;

void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}

void initI2C0(void)
{
	//This function is for eewiki and is to be updated to handle any port

	//enable I2C module
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

	//reset I2C module
	SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

	//enable GPIO peripheral that contains I2C
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	// Configure the pin muxing for I2C0 functions on port B2 and B3.
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);

	// Select the I2C function for these pins.
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

	// Enable and initialize the I2C0 master module.  Use the system clock for
	// the I2C0 module.  The last parameter sets the I2C data transfer rate.
	// If false the data rate is set to 100kbps and if true the data rate will
	// be set to 400kbps.
	I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

	//clear I2C FIFOs
	HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
}

uint8_t readI2C0(uint16_t device_address, uint16_t device_register)
{
	//specify that we want to communicate to device address with an intended write to bus
	I2CMasterSlaveAddrSet(I2C0_BASE, device_address, false);

	//the register to be read
	I2CMasterDataPut(I2C0_BASE, device_register);

	//send control byte and register address byte to slave device
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	//wait for MCU to complete send transaction
	while(I2CMasterBusy(I2C0_BASE));

	//read from the specified slave device
	I2CMasterSlaveAddrSet(I2C0_BASE, device_address, true);

	//send control byte and read from the register from the MCU
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

	//wait while checking for MCU to complete the transaction
	while(I2CMasterBusy(I2C0_BASE));

	//Get the data from the MCU register and return to caller
	return( I2CMasterDataGet(I2C0_BASE));
}

void writeI2C0(uint16_t device_address, uint16_t device_register, uint8_t device_data)
{
	//specify that we want to communicate to device address with an intended write to bus
	I2CMasterSlaveAddrSet(I2C0_BASE, device_address, false);

	//register to be read
	I2CMasterDataPut(I2C0_BASE, device_register);

	//send control byte and register address byte to slave device
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	//wait for MCU to finish transaction
	while(I2CMasterBusy(I2C0_BASE));

	I2CMasterSlaveAddrSet(I2C0_BASE, device_address, true);

	//specify data to be written to the above mentioned device_register
	I2CMasterDataPut(I2C0_BASE, device_data);

	//wait while checking for MCU to complete the transaction
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

	//wait for MCU & device to complete transaction
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
void ftoa(float f,char *buf)
{
    int pos=0,ix,dp,num;
    if (f<0)
    {
        buf[pos++]='-';
        f = -f;
    }
    dp=0;
    while (f>=10.0)
    {
        f=f/10.0;
        dp++;
    }
    for (ix=1;ix<8;ix++)
    {
            num = (int)f;
            f=f-num;
            if (num>9)
                buf[pos++]='#';
            else
                buf[pos++]='0'+num;
            if (dp==0) buf[pos++]='.';
            f=f*10.0;
            dp--;
    }
}
int main()
{
	

    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_240), 120000000);
                                             
    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    ConfigureUART();
    initI2C0();

    
    MAP_IntMasterEnable();
    //IntEnable(INT_I2C0);


    
    MPU_9250_INIT();

    
    
	
	
	char buffer[20];

	
	

	
    
    while (1)
    {
        Accel_X = read_raw_data(ACCEL_XOUT_H);
		Accel_Y = read_raw_data(ACCEL_YOUT_H);
		Accel_Z = read_raw_data(ACCEL_ZOUT_H);
		Gyro_X = read_raw_data(GYRO_XOUT_H);
		Gyro_Y = read_raw_data(GYRO_YOUT_H);
		Gyro_Z = read_raw_data(GYRO_ZOUT_H);
		Ax = Accel_X/16384.0;
		Ay = Accel_Y/16384.0;
		Az = Accel_Z/16384.0;
		Gx = Gyro_X/131.0;
		Gy = Gyro_Y/131.0;
		Gz = Gyro_Z/131.0;

		
		
		ftoa(Gy,buffer);
        UARTprintf("%s\n", buffer);

		SysCtlDelay(1000000);
		//
    }
    
}

