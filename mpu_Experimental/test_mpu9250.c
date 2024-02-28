
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

#include "driverlib/fpu.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
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
char buffer[20];
float angulo;
void
ConfigureUART(void)
{

    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    MAP_GPIOPinConfigure(GPIO_PA0_U0RX);
    MAP_GPIOPinConfigure(GPIO_PA1_U0TX);
    MAP_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 115200, g_ui32SysClock);
}

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
	writeI2C0(MPU_9250_ADDRESS, GYRO_CONFIG, 0x00); //antes 24
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
void ConfigureTimer0A(void)
{
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);


    // se configura los timers en 32 bits 
    MAP_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    MAP_TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32SysClock/10);


    MAP_IntEnable(INT_TIMER0A);

    MAP_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    MAP_TimerEnable(TIMER0_BASE, TIMER_A);

}
void
Timer0IntHandler(void)
{
    
    MAP_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	
    MAP_IntMasterDisable();
    Accel_X = read_raw_data(ACCEL_XOUT_H);
	Accel_Y = read_raw_data(ACCEL_YOUT_H);
	Accel_Z = read_raw_data(ACCEL_ZOUT_H);
	Gyro_X = read_raw_data(GYRO_XOUT_H);
	Gyro_Y = read_raw_data(GYRO_YOUT_H);
	Gyro_Z = read_raw_data(GYRO_ZOUT_H);

	
	
    MAP_IntMasterEnable();
	Ax = Accel_X/16384.0;
	Ay = Accel_Y/16384.0;
	Az = Accel_Z/16384.0;
	Gx = Gyro_X/131.0;
	Gy = Gyro_Y/131.0;
	Gz = Gyro_Z/131.0;
	angulo = angulo + Gz*0.1 - 0.046;
	
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

    MPU_9250_INIT();
    MAP_IntMasterEnable();
	ConfigureTimer0A();
	

    while (1)
    {
        
		
        //UARTprintf("%s\n", buffer);
		int ang = (int)angulo;
		UARTprintf("%d\n",ang);
		SysCtlDelay(1000000);
    }
    
}

