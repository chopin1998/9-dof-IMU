#include "common.h"

#include "uart.h"
#include "clock.h"
#include "uart_process.h"
#include "imu_sensor.h"
#include "qd.h"
#include "tilt.h"

#define DEG_RAD_OP (3.14159265359 / 180.0)
#define RAD_REG_OP (180.0 / 3.14159265359)

float gyro_x, gyro_y, gyro_z;
float accel_x, accel_y, accel_z;

float roll, pitch, yaw;
float roll_rate, pitch_rate, yaw_rate;
// float roll_est, pitch_est, yaw_est;

// Tilt state structures.
tilt roll_tilt_state, pitch_tilt_state, yaw_tilt_state;


extern volatile unsigned char rtc_flag;
extern volatile unsigned char accel_updated, gyro_updated;


void imu_filter(IMU_GYRO_RESULT_t* g_rev, IMU_ACCEL_RESULT_t* a_rev)
{
    float roll_est, pitch_est, yaw_est;
    
    roll = atan2(a_rev->y, a_rev->z);
    pitch = atan2(a_rev->x, a_rev->z);
    yaw = atan2(a_rev->x, a_rev->y);

    
    roll_rate = g_rev->x * DEG_RAD_OP / 1000;
    pitch_rate = g_rev->y * DEG_RAD_OP / 1000;
    yaw_rate = g_rev->z * DEG_RAD_OP / 1000;

    
    tilt_state_update(&roll_tilt_state, roll_rate);
    tilt_kalman_update(&roll_tilt_state, roll);
    roll_est = tilt_get_angle(&roll_tilt_state);

    tilt_state_update(&pitch_tilt_state, pitch_rate);
    tilt_kalman_update(&pitch_tilt_state, pitch);
    pitch_est = tilt_get_angle(&pitch_tilt_state);

    // tilt_state_update(&yaw_tilt_state, yaw_rate);
    // tilt_kalman_update(&yaw_tilt_state, yaw);
    // yaw_est = tilt_get_angle(&yaw_tilt_state);

    printf("tilt:%f,%f,%f\n", roll_est, pitch_est, yaw_est);
}


void init_io(void)
{
    // PORTE.DIRSET = 0xff;        /* LED port */
    // PORTE.OUT = 0xff;
    PORTA.DIRSET = PIN6_bm | PIN7_bm; /* LED pins */
    
    PORTD.DIRSET = PIN3_bm;     /* serial tx pin */
    PORTD.DIRCLR = PIN2_bm;     /* serial rx pin */
    
    PORTD.DIRCLR = PIN0_bm | PIN1_bm;


    PORTF.DIRCLR = PIN4_bm;     /* gyro int */
    PORTF.INT0MASK |= PIN4_bm;
    PORTF.PIN4CTRL |= PORT_ISC_RISING_gc | PORT_OPC_PULLDOWN_gc;
    PORTF.INTCTRL |= PORT_INT0LVL_LO_gc;

    PORTF.DIRCLR = PIN5_bm;      /* accel int */
    PORTF.INT1MASK |= PIN5_bm;
    PORTF.PIN5CTRL |= PORT_ISC_RISING_gc | PORT_OPC_PULLDOWN_gc;
    PORTF.INTCTRL |= PORT_INT1LVL_LO_gc;

    
    // PORTC.DIRSET = PIN7_bm;
    // PORTCFG.CLKEVOUT |= PORTCFG_CLKOUT_PC7_gc;
}


void _uart_sendc(unsigned char c)
{
    uart_sendc(&UART_BT, c);
}

void init_uart(void)
{
    uart_init(&UART_BT);
    uart_queue_init(&Q_BT);

    uart_process_init_linebuf(&LB_BT);
    LB_BT.packet_status = OUT_PACKET;

    
    fdevopen(_uart_sendc, NULL);
}

void timer_init(void)
{
    TCC0.CTRLB = TC0_CCAEN_bm | TC_WGMODE_SS_gc;
    TCC0.CTRLC = TC0_CMPA_bm;
    TCC0.PER = 0xffff;
    TCC0.CCA = 0x7fff;
    TCC0.CTRLA = TC_CLKSEL_DIV8_gc; /* clock sel */
}



int main(void)
{
    unsigned char light_count = 0;

    IMU_GYRO_RESULT_t g_rev;
    IMU_ACCEL_RESULT_t a_rev;

    
    clock_pll_init();
    clock_rtc_init();

    init_io();
    init_uart();
    timer_init();

    /*
    PR.PRGEN = PR_AES_bm | PR_EBI_bm | PR_EVSYS_bm | PR_DMA_bm;
    PR.PRPA = PR_DAC_bm | PR_ADC_bm | PR_AC_bm;
    PR.PRPB = PR_DAC_bm | PR_ADC_bm | PR_AC_bm;
    
    PR.PRPC = PR_TWI_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;
    PR.PRPD = PR_TWI_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;
    PR.PRPE = PR_TWI_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;
    PR.PRPF = PR_SPI_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;
    */

    PMIC.CTRL |= PMIC_LOLVLEX_bm | PMIC_RREN_bm;
    sei();

    qd_init();
    
    imu_init();
    // void tilt_init(tilt *self, float dt, float R_angle, float Q_gyro, float Q_angle)
    tilt_init(&roll_tilt_state, 0.25, 0.225, 0.003, 0.001);
    tilt_init(&pitch_tilt_state, 0.25, 0.225, 0.003, 0.001);
    tilt_init(&yaw_tilt_state, 0.25, 0.225, 0.003, 0.001);    
    
    _delay_ms(10);
    
    do {} while (gyro_updated && accel_updated);
    
    imu_gyro_read(&g_rev);
    imu_accel_read(&a_rev);
    gyro_updated = 0;
    accel_updated = 0;
    
    // roll_est = atan2(a_rev.y, a_rev.z);
    // pitch_est = atan2(a_rev.x, a_rev.z);
    
    for (;;)
    {
        if (rtc_flag)
        {
            rtc_flag = 0;

            /*
            light_count++;
            if ((light_count & 0x0f) == 0x0a)
            {
                PORTE.OUTCLR = PIN7_bm;
            }
            else
            {
                PORTE.OUTSET = PIN7_bm;
            }
            */
        }

        if (gyro_updated)
        {
            gyro_updated = 0;
            imu_gyro_read(&g_rev);
            // printf("gyro:%d|%d|%d\n", g_rev.x, g_rev.y, g_rev.z);

            if (accel_updated)
                imu_filter(&g_rev, &a_rev);
        }

        if (accel_updated)
        {
            accel_updated = 0;
            
            imu_accel_read(&a_rev);
            // imu_read_reg(IMU_ACCEL_ADDR, IMU_ACCEL_HPF_RST);

            // printf("accel:%d|%d|%d|%d\n", a_rev.x>>4, a_rev.y>>4, a_rev.z>>4, QD_READ());
        }

        
        if (1)
        {
            uart_process_tick(&Q_BT, &LB_BT, uart_process_lb_bt, STX, ETX);
        }
        
        if (1)
        {
            // SLEEP.CTRL = SLEEP_SEN_bm | SLEEP_SMODE_PSAVE_gc;
            SLEEP.CTRL = SLEEP_SEN_bm | SLEEP_SMODE_IDLE_gc;
            __asm__ __volatile__ ("sleep");
        }
    }
}
