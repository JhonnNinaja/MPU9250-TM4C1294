

extern void initI2C0(void);

extern uint8_t readI2C0(uint16_t device_address, uint16_t device_register);
extern void writeI2C0(uint16_t device_address, uint16_t device_register, uint8_t device_data);
extern void MPU_9250_INIT();
extern int16_t read_raw_data(uint16_t addr);
