 
#ifndef QMC5883L_H_
#define QMC5883L_H_

#define DFLT_BUS 1
#define DFLT_ADDRESS 0x0d

#define REG_XOUT_LSB 0x00   // Output Data Registers for magnetic sensor.
#define REG_XOUT_MSB 0x01
#define REG_YOUT_LSB 0x02
#define REG_YOUT_MSB 0x03
#define REG_ZOUT_LSB 0x04
#define REG_ZOUT_MSB 0x05
#define REG_STATUS_1 0x06   // Status Register.
#define REG_TOUT_LSB 0x07   // Output Data Registers for temperature.
#define REG_TOUT_MSB 0x08
#define REG_CONTROL_1 0x09  // Control Register #1.
#define REG_CONTROL_2 0x0a  // Control Register #2.
#define REG_RST_PERIOD 0x0b  // SET/RESET Period Register.
#define REG_CHIP_ID 0x0d   // Chip ID register.

#define STAT_DRDY 0x01 // Data Ready.
#define STAT_OVL 0x02  // Overflow flag.
#define STAT_DOR 0x04  // Data skipped for reading.

#define INT_ENB 0x01 // Interrupt Pin Enabling.
#define POL_PNT 0x40  // Pointer Roll-over.
#define SOFT_RST 0x80  // Soft Reset.

#define MODE_STBY 0x00 // Standby mode.
#define MODE_CONT 0x01 // Continuous read mode.
#define ODR_10HZ 0x00  // Output Data Rate Hz.
#define ODR_50HZ 0x04
#define ODR_100HZ 0x08
#define ODR_200HZ 0x0c
#define RNG_2G 0x00   // Range 2 Gauss: for magnetic-clean environments.
#define RNG_8G 0x10 // Range 8 Gauss: for strong magnetic fields.
#define OSR_512 0x00  // Over Sample Rate 512: less noise; more power.
#define OSR_256 0x40
#define OSR_128 0x80
#define OSR_64 0xc0

typedef struct QMC5883 {
    char* i2c_bus; 
    uint8_t address;
    uint8_t output_rate;
    uint8_t output_range;
    uint8_t oversampling_rate;
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint16_t t;
    int8_t fd;
} QMC5883;
