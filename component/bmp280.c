// Second-modified on 2025 for DBF X1 Firmware

// This file is modified based on BMP280/BMP280.c
// by leon-anavi@github
// URL: https://github.com/leon-anavi/rpi-examples/blob/master/BMP280/c/BMP280.c

#include "bmp280.h"
#include "sys.h"

/* BMP280 default address */
#define BMP280_I2CADDR 0x76
#define BMP280_CHIPID 0xD0

/* BMP280 Registers */
#define BMP280_DIG_T1 0x88 /* R   Unsigned Calibration data (16 bits) */
#define BMP280_DIG_T2 0x8A /* R   Signed Calibration data (16 bits) */
#define BMP280_DIG_T3 0x8C /* R   Signed Calibration data (16 bits) */
#define BMP280_DIG_P1 0x8E /* R   Unsigned Calibration data (16 bits) */
#define BMP280_DIG_P2 0x90 /* R   Signed Calibration data (16 bits) */
#define BMP280_DIG_P3 0x92 /* R   Signed Calibration data (16 bits) */
#define BMP280_DIG_P4 0x94 /* R   Signed Calibration data (16 bits) */
#define BMP280_DIG_P5 0x96 /* R   Signed Calibration data (16 bits) */
#define BMP280_DIG_P6 0x98 /* R   Signed Calibration data (16 bits) */
#define BMP280_DIG_P7 0x9A /* R   Signed Calibration data (16 bits) */
#define BMP280_DIG_P8 0x9C /* R   Signed Calibration data (16 bits) */
#define BMP280_DIG_P9 0x9E /* R   Signed Calibration data (16 bits) */

#define BMP280_CONTROL 0xF4 /*sets the data acquisition options of the device*/
#define BMP280_RESET 0xE0
#define BMP280_CONFIG 0xF5 /*sets the rate, filter and interface options of the device*/
#define BMP280_PRESSUREDATA 0xF7
#define BMP280_TEMPDATA 0xFA

void bmp280_cal(bmp280_t dev)
{
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_T1, dev->cal.cal_t1);
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_T2, dev->cal.cal_t2);
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_T3, dev->cal.cal_t3);
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_P1, dev->cal.cal_p1);
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_P2, dev->cal.cal_p2);
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_P3, dev->cal.cal_p3);
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_P4, dev->cal.cal_p4);
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_P5, dev->cal.cal_p5);
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_P6, dev->cal.cal_p6);
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_P7, dev->cal.cal_p7);
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_P8, dev->cal.cal_p8);
    i2c_read_mem_16(&dev->i2c_dev, BMP280_DIG_P9, dev->cal.cal_p9);
}

int32_t compensate_temp(int raw_temp, struct bmp280_cal *cal)
{
    // Magic stuff by leon..
    int t1 = (((raw_temp >> 3) - (cal->cal_t1 << 1)) * (cal->cal_t2)) >> 11;
    int t2 = (((((raw_temp >> 4) - (cal->cal_t1)) *
                ((raw_temp >> 4) - (cal->cal_t1))) >>
               12) *
              (cal->cal_t3)) >>
             14;
    return t1 + t2;
}

int bmp280_init(bmp280_t dev)
{
    if (dev == NULL)
        return -1;

    if (i2c_check(&dev->i2c_dev) != HAL_OK)
        return -1;

    bmp280_cal(dev);

    uint8_t ctrl_bits = 0b11110011;
    i2c_write_mem_8(&dev->i2c_dev, BMP280_CONTROL, ctrl_bits);

    return 0;
}

int bmp280_update(bmp280_t dev)
{
    // Temperature
    int raw_temp;
    i2c_read_mem_20(&dev->i2c_dev, BMP280_TEMPDATA, raw_temp);
    int comp_temp = compensate_temp(raw_temp, &dev->cal);
    dev->data.temperature = ((comp_temp * 5 + 128) >> 8) / 100.0f;

    // Pressure
    int raw_press;
    i2c_read_mem_20(&dev->i2c_dev, BMP280_PRESSUREDATA, raw_press);

    // Magic thing on pressure ...
    int64_t p1 = comp_temp / 2 - 64000;
    int64_t p2 = p1 * p1 * (int64_t)dev->cal.cal_p6 / 32768;
    int64_t buf = (p1 * (int64_t)dev->cal.cal_p5 * 2);
    p2 += buf << 17;
    p2 += (int64_t)dev->cal.cal_p4 << 35;
    p1 = ((p1 * p1 * dev->cal.cal_p3) >> 8) + ((p1 * dev->cal.cal_p2) << 12);
    p1 = (((int64_t)1 << 47) + p1) * ((int64_t)dev->cal.cal_p1) >> 33;

    // Avoid exception caused by division by zero
    if (0 == p1)
    {
        // printf("bmp div by 0 err!\n");
        dev->data.pressure = 0.0f;
    }
    else
    {
        int64_t p = 1048576 - raw_press;
        p = (((p << 31) - p2) * 3125) / p1;
        p1 = ((int64_t)dev->cal.cal_p9 * (p >> 13) * (p >> 13)) >> 25;
        p2 = ((int64_t)dev->cal.cal_p8 * p) >> 19;
        p = ((p + p1 + p2) >> 8) + (((int64_t)dev->cal.cal_p7) << 4);
        dev->data.pressure = p / 256.0f;
    }
    // printf("BMP update! %f\n", dev->data.pressure);

    // For testing when the BMP is burnt
    // dev->data.pressure = 1.23456 * 1e5;
    // dev->data.temperature = 27.89;

    return 0;
}