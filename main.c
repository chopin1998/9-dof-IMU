#include "common.h"

#include "uart.h"
#include "clock.h"
#include "uart_process.h"
#include "imu_sensor.h"


extern volatile unsigned char rtc_flag;
volatile unsigned char gyro_updated=0, accl_updated=0, magn_update=0;


ISR (PORTF_INT0_vect)
{
    PORTE.OUTCLR = PIN6_bm;

    gyro_updated++;
}

ISR (PORTF_INT1_vect)
{
    PORTE.OUTCLR = PIN5_bm;
    
    accl_updated++;
}

void init_io(void)
{
    PORTE.DIRSET = 0xff;        /* LED port */
    PORTE.OUT = 0xff;
    
    PORTF.DIRSET = PIN3_bm;     /* serial tx pin */
    PORTF.DIRCLR = PIN2_bm;     /* serial rx pin */


    PORTF.DIRCLR = PIN4_bm;     /* gyro int */
    PORTF.INT0MASK |= PIN4_bm;
    PORTF.PIN4CTRL |= PORT_ISC_RISING_gc | PORT_OPC_PULLDOWN_gc;
    PORTF.INTCTRL |= PORT_INT0LVL_LO_gc;

    PORTF.DIRCLR = PIN5_bm;      /* accl int */
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
    IMU_ACCL_RESULT_t a_rev;

    
    clock_pll_init();
    clock_rtc_init();

    init_io();
    init_uart();
    timer_init();

    PR.PRGEN = PR_AES_bm | PR_EBI_bm | PR_EVSYS_bm | PR_DMA_bm;
    PR.PRPA = PR_DAC_bm | PR_ADC_bm | PR_AC_bm;
    PR.PRPB = PR_DAC_bm | PR_ADC_bm | PR_AC_bm;

    PR.PRPC = PR_TWI_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;
    PR.PRPD = PR_TWI_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;
    PR.PRPE = PR_TWI_bm | PR_SPI_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;
    PR.PRPF = PR_SPI_bm | PR_HIRES_bm | PR_TC1_bm | PR_TC0_bm;
        

    PMIC.CTRL |= PMIC_LOLVLEX_bm | PMIC_RREN_bm;
    sei();
    
    imu_init();
    _delay_ms(200);
    
    for (;;)
    {
        if (rtc_flag)
        {
            rtc_flag = 0;

            light_count++;
            if ((light_count & 0x0f) == 0x0a)
            {
                PORTE.OUTCLR = PIN7_bm;
            }
            else
            {
                PORTE.OUTSET = PIN7_bm;
            }
        }

        if (gyro_updated)
        {
            gyro_updated = 0;

            imu_gyro_read(&g_rev);

            printf("gyro:%d|%d|%d\n", g_rev.x, g_rev.y, g_rev.z);
        }

        if (accl_updated)
        {
            accl_updated = 0;

            imu_accl_read(&a_rev);
            // imu_read_reg(IMU_ACCL_ADDR, IMU_ACCL_HPF_RST);

            printf("accl:%d|%d|%d\n", a_rev.x>>4, a_rev.y>>4, a_rev.z>>4);
        }

        if (1)
        {
            PORTE.OUTSET = PIN6_bm | PIN5_bm;
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
