#include "common.h"
#include "twi_master_driver.h"
#include "imu_sensor.h"


TWI_Master_t imu_twi;


ISR (IMU_TWI_vect)
{
    twi_master_irq_handler(&imu_twi);
}

unsigned char imu_read_reg(unsigned char part_addr, unsigned char reg)
{
    unsigned char cmd_buf[] = { reg };
    
    twi_master_writeread(&imu_twi, part_addr, cmd_buf, 1, 1);
    while (imu_twi.status != TWIM_STATUS_READY)
    {
    }

    return imu_twi.read_data[0];
}

unsigned char imu_write_reg(unsigned char part_addr, unsigned char reg, unsigned char val)
{
    unsigned char cmd_buf[] = { reg, val };

    twi_master_write(&imu_twi, part_addr, cmd_buf, 2);
    while (imu_twi.status != TWIM_STATUS_READY)
    {
    }

    return 0;
}



void imu_init(void)
{
    twi_master_init(&imu_twi, &IMU_TWI,
                    TWI_MASTER_INTLVL_LO_gc, TWI_BAUD(F_CPU, 400000));


    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_CTRL2, 0x37); /* HPF autoreset, cut-off @ 0.1hz when ODR=200 */
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_CTRL3, 0x00); /*  */
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_CTRL4, 0x80); /* block data update, FS = 250dps */
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_REF, 0x00);
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_INT1_THS_XH, 0x00);
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_INT1_THS_XL, 0x00);
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_INT1_THS_YH, 0x00);
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_INT1_THS_YL, 0x00);
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_INT1_THS_ZH, 0x00);
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_INT1_THS_ZL, 0x00);
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_INT1_DURATION, 0x00);
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_INT1_CFG, 0x00);
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_CTRL5, 0x13); /* HPF and LPF2 enable */
    
    imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_CTRL1, 0x60); /* power off, LPF1->54hz, LPF2->50hz, ODR->200hz */
}


unsigned char imu_gyro_read(IMU_GYRO_RESULT_t *rev)
{
    /*
    unsigned char cmd_buf[] = { IMU_GYRO_X_L };
    twi_master_writeread(&imu_twi, IMU_GYRO_ADDR, cmd_buf, 1, 6);
    memcpy((void *)rev, (const void *)imu_twi.read_data, 6);
    */

    unsigned char tmp;
    unsigned char *p = (unsigned char *)(rev);
    
    for (unsigned char i=0; i<6; i++)
    {
        tmp = imu_read_reg(IMU_GYRO_ADDR, IMU_GYRO_X_L + i);
        *p = tmp;
        p++;

        // printf("tmp: 0x%02x, ", tmp);
    }
    // printf("\n");

    return 0;
}

void imu_gyro_power(unsigned char sel)
{
    unsigned char oldval = imu_read_reg(IMU_GYRO_ADDR, IMU_GYRO_CTRL1);
    
    switch(sel)
    {
    case IMU_GYRO_POWER_ON:
        imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_CTRL1, oldval | 0x0f);
        
        break;
    case IMU_GYRO_POWER_SLEEP:
        imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_CTRL1, oldval & 0xf8);
        
        break;
    case IMU_GYRO_POWER_OFF:
        imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_CTRL1, oldval & 0xf0);
        
        break;
    default:
        break;
    }
}

void imu_gyro_dump(unsigned char on)
{
    if (on)
    {
        imu_gyro_power(IMU_GYRO_POWER_ON);
        _delay_ms(100);
        
        imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_CTRL3, 0x08);
    }
    else
    {
        imu_write_reg(IMU_GYRO_ADDR, IMU_GYRO_CTRL3, 0x00);

        imu_gyro_power(IMU_GYRO_POWER_OFF);
    }
}

