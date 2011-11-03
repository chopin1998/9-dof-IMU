#ifndef _IMU_SENSOR_H
#define _IMU_SENSOR_H 1


#define IMU_TWI         TWIF
#define IMU_TWI_vect    TWIF_TWIM_vect

#define IMU_GYRO_ADDR   0x69
#define IMU_ACCL_ADDR   0x18
#define IMU_MAGN_ADDR   0x1e


/* GYRO REGISTER MAP */
#define IMU_GYRO_WHOAMI    0x0f

#define IMU_GYRO_CTRL1     0x20
#define IMU_GYRO_CTRL2     0x21
#define IMU_GYRO_CTRL3     0x22
#define IMU_GYRO_CTRL4     0x23
#define IMU_GYRO_CTRL5     0x24
#define IMU_GYRO_REF       0x25
#define IMU_GYRO_TEMP      0x26
#define IMU_GYRO_STATUS    0x27
#define IMU_GYRO_X_L       0x28
#define IMU_GYRO_X_H       0x29
#define IMU_GYRO_Y_L       0x2a
#define IMU_GYRO_Y_H       0x2b
#define IMU_GYRO_Z_L       0x2c
#define IMU_GYRO_Z_H       0x2d
#define IMU_GYRO_FF_CTRL   0x2e
#define IMU_GYRO_FF_SRC    0x2f
#define IMU_GYRO_INT1_CFG  0x30
#define IMU_GYRO_INT1_SRC  0x31
#define IMU_GYRO_INT1_THS_XH 0x32
#define IMU_GYRO_INT1_THS_XL 0x33
#define IMU_GYRO_INT1_THS_YH 0x34
#define IMU_GYRO_INT1_THS_YL 0x35
#define IMU_GYRO_INT1_THS_ZH 0x36
#define IMU_GYRO_INT1_THS_ZL 0x37
#define IMU_GYRO_INT1_DURATION 0x38


/* ACCL REGISTER MAP */
#define IMU_ACCL_CTRL1    0x20
#define IMU_ACCL_CTRL2    0x21
#define IMU_ACCL_CTRL3    0x22
#define IMU_ACCL_CTRL4    0x23
#define IMU_ACCL_CTRL5    0x24
#define IMU_ACCL_HPF      0x25
#define IMU_ACCL_REF      0x26
#define IMU_ACCL_STATUS   0x27
#define IMU_ACCL_OUT_X_L  0x28
#define IMU_ACCL_OUT_X_H  0x29
#define IMU_ACCL_OUT_Y_L  0x2a
#define IMU_ACCL_OUT_Y_H  0x2b
#define IMU_ACCL_OUT_Z_L  0x2c
#define IMU_ACCL_OUT_Z_H  0x2d
#define IMU_ACCL_INT1_CFG 0x30
#define IMU_ACCL_INT1_SRC 0x31
#define IMU_ACCL_INT1_THS 0x32
#define IMU_ACCL_INT1_DURATION 0x33
#define IMU_ACCL_INT2_CFG 0x34
#define IMU_ACCL_INT2_SRC 0x35
#define IMU_ACCL_INT2_THS 0x36
#define IMU_ACCL_INT2_DURATION 0x37



/* MAGN REGISTER MAP */
#define IMU_MAGN_CRA      0x00
#define IMU_MAGN_CRB      0x01
#define IMU_MAGN_MR       0x02
#define IMU_MAGN_OUT_X_H  0x03
#define IMU_MAGN_OUT_X_L  0x04
#define IMU_MAGN_OUT_Y_H  0x05
#define IMU_MAGN_OUT_Y_L  0x06
#define IMU_MAGN_OUT_Z_H  0x07
#define IMU_MAGN_OUT_Z_L  0x08
#define IMU_MAGN_SR       0x09
#define IMU_MAGN_IRA      0x0a
#define IMU_MAGN_IRB      0x0b
#define IMU_MAGN_IRC      0x0c



enum { IMU_GYRO_POWER_ON,
       IMU_GYRO_POWER_SLEEP,
       IMU_GYRO_POWER_OFF,
};


typedef struct IMU_GYRO_RESULT
{
    /*
    unsigned char x_l;
    unsigned char x_h;
    unsigned char y_l;
    unsigned char y_h;
    unsigned char z_l;
    unsigned char z_h;
    */
    signed short x;
    signed short y;
    signed short z;
} IMU_GYRO_RESULT_t;



void imu_init(void);

unsigned char imu_read_reg(unsigned char addr, unsigned char reg);
unsigned char imu_write_reg(unsigned char addr, unsigned char reg, unsigned char val);

void imu_gyro_power(unsigned char sel);
void imu_gyro_dump(unsigned char on);


#endif
