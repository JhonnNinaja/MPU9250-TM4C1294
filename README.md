Try "mpu_Experimental/test_mpu9250.c"


This repository contains C code to interface with the MPU9250 sensor using a TMC4XL1294 development board. The code reads accelerometer and gyroscope data from the MPU9250 sensor via I2C communication and calculates the angle based on the gyroscope data.

### Dependencies
- TivaWare™ Peripheral Driver Library from Texas Instruments.
- UART Standard I/O Library (`utils/uartstdio.h`) for UART communication.

### Hardware Setup
- Connect the MPU9250 sensor to the TMC4XL1294 development board using the I2C interface.
- Ensure proper power supply and ground connections.

### Code Overview
- The code initializes UART communication for debugging purposes.
- It configures and initializes the I2C0 peripheral for communication with the MPU9250 sensor.
- The `MPU_9250_INIT()` function initializes the MPU9250 sensor by writing configuration values to specific registers.
- Sensor data is read in raw format using the `read_raw_data()` function.
- The `Timer0IntHandler()` function is called periodically by Timer0, where sensor data is acquired, converted to meaningful values, and angle calculation is performed.
- The angle calculated represents the orientation change based on gyroscope readings.
- The main loop continuously prints the calculated angle via UART.


### How Angle is Calculated
- Gyroscope readings are used to calculate the change in angle over time.
- The gyroscope data is converted to degrees per second.
- Integration of the gyroscope readings over time provides the change in angle.
- The calculated angle is updated periodically based on the gyroscope readings.

### Usage
- Flash the compiled code onto the TMC4XL1294 development board.
- Connect to the board via a serial terminal (e.g., PuTTY) to view the printed angle values.
- Ensure proper power supply and connections to the MPU9250 sensor.

### How Data is Read from the Sensor

The MPU9250 sensor's data is accessed via the Inter-Integrated Circuit (I2C) communication protocol. Here's how the code accesses the sensor's memory:

1. **Initialization**: 
   - The `initI2C0()` function initializes the I2C0 peripheral on the TMC4XL1294 development board.
   - It configures the I2C pins and sets the clock frequency.
   - This enables the microcontroller to communicate with the MPU9250 sensor via the I2C protocol.

2. **Read Operation**:
   - The `readI2C0()` function is used to read data from a specific register address of the MPU9250 sensor.
   - It takes two parameters: the device address (MPU_9250_ADDRESS) and the register address from which data is to be read.
   - This function sets the device address and register address on the I2C bus and initiates a read operation.
   - After the read operation is completed, the function returns the byte of data read from the specified register.

3. **Write Operation**:
   - The `writeI2C0()` function is used to write data to a specific register address of the MPU9250 sensor.
   - It takes three parameters: the device address, the register address to which data is to be written, and the data to be written.
   - This function sets the device address and register address on the I2C bus, writes the data to the specified register, and initiates a write operation.

4. **Register Addressing**:
   - The MPU9250 sensor's internal registers are accessed using specific register addresses.
   - For example, `ACCEL_XOUT_H`, `ACCEL_XOUT_L`, `GYRO_XOUT_H`, `GYRO_XOUT_L`, etc., are defined as register addresses.
   - These addresses are used to read accelerometer and gyroscope data from their respective axes.

By using these functions and register addresses, the code can effectively communicate with the MPU9250 sensor and read data from its internal registers, facilitating sensor data acquisition and processing.

### License
This code is provided under the [MIT License](LICENSE).

