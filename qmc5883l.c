#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <string.h>

#include "qmc5883l.h"

static const char *i2c_device = "/dev/i2c-2";

int8_t i2c_reg_write(QMC5883 qmc, uint8_t reg_addr, uint8_t data, uint16_t len)
{
    if (write(qmc.fd, &data, len + 1) < (uint16_t)len) 
        return -1;

    return 0;
}


int8_t i2c_reg_read(QMC5883 qmc, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    write(qmc.fd, &reg_addr, 1);
    return read(qmc.fd, data, len);
}

int8_t qmc_init(QMC5883 *qmc)
{
    uint8_t *buf;
    buf = malloc(2);

    if ((qmc->fd = open(qmc->i2c_bus, O_RDWR)) < 0) {
        fprintf(stderr, "Failed to open i2c bus\n");
        return -1;
    }

    if (ioctl(qmc->fd, I2C_SLAVE, qmc->address) < 0) {
        fprintf(stderr, "Failed to talk to I2C slave\n");
        return -2;
    }

    i2c_reg_read(*qmc, REG_CHIP_ID, buf, 1);
    int8_t ret = 0;

    ret |= buf[0] != 0xff;
    free(buf);

    ret |= i2c_reg_write(*qmc, REG_CONTROL_2, SOFT_RST, 1);
    ret |= i2c_reg_write(*qmc, REG_CONTROL_2, INT_ENB, 1);
    ret |= i2c_reg_write(*qmc, REG_RST_PERIOD, 0x01, 1);
    ret |= i2c_reg_write(*qmc, REG_CONTROL_1,MODE_CONT | qmc->output_rate | qmc->output_range | qmc->oversampling_rate,1);

    return ret;
}

int8_t read_word_2c(QMC5883 qmc, uint8_t reg_addr, uint16_t *data) {
    uint16_t word;
    uint8_t *buf;
    int8_t ret = 0;

    buf = malloc(3);
    ret = i2c_reg_read(qmc, reg_addr, buf, 2);

    word = (uint16_t)buf[1] << 8 + buf[0];
    *data = word >= 0x8000 ? word - 0x10000 : word;

    return ret;
}

int8_t qmc_read_data(QMC5883 *qmc)
{
    uint8_t *buf;
    buf = malloc(2);

    for(int i=0; i < 20; i++){
        i2c_reg_read(*qmc, REG_STATUS_1, buf, 1);
        if(*buf & STAT_OVL) {
            // Some values have reached an overflow
            fprintf(stderr, "Magnetic sensor overflow\n");
            free(buf);
            return -4;
        } else if(*buf & STAT_DOR) {
            read_word_2c(*qmc, REG_XOUT_LSB, &qmc->x);
            read_word_2c(*qmc, REG_YOUT_LSB, &qmc->y);
            read_word_2c(*qmc, REG_ZOUT_LSB, &qmc->z);
            break;
        } else if(*buf & STAT_DRDY) {
            read_word_2c(*qmc, REG_XOUT_LSB, &qmc->x);
            read_word_2c(*qmc, REG_YOUT_LSB, &qmc->y);
            read_word_2c(*qmc, REG_ZOUT_LSB, &qmc->z);
            read_word_2c(*qmc, REG_TOUT_LSB, &qmc->t);
            break;
        } else {
            nanosleep((const struct timespec[]){{0, 100000L}}, NULL);
        }
    }

    free(buf);
    return 0;
}

