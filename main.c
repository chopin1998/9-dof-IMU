#include "common.h"

#include "uart.h"
#include "clock.h"
#include "uart_process.h"
#include "imu_sensor.h"




extern volatile unsigned char rtc_flag;
volatile unsigned char gyro_updated = 0;


ISR (PORTD_INT0_vect)
{
    static unsigned char count = 0;
    
    PORTD.INTFLAGS = 0x00;

    PORTE.OUTTGL = PIN6_bm;    
}

ISR (PORTF_INT0_vect)
{
    PORTF.INTFLAGS = 0x00;

    PORTE.OUTTGL = PIN5_bm;

    gyro_updated++;
}

void init_io(void)
{
    PORTE.DIRSET = 0xff;
    PORTE.OUT = 0xff;
    PORTF.DIRSET = PIN3_bm;
    PORTF.DIRCLR = PIN2_bm;

    
    PORTD.DIRCLR = PIN0_bm;
    PORTD.INT0MASK = PIN0_bm;
    PORTD.PIN0CTRL |= PORT_ISC_FALLING_gc | PORT_OPC_PULLUP_gc;
    PORTD.INTCTRL = PORT_INT0LVL_LO_gc;

    PORTF.DIRCLR = PIN4_bm;
    PORTF.INT0MASK = PIN4_bm;
    PORTF.PIN4CTRL |= PORT_ISC_RISING_gc | PORT_OPC_PULLDOWN_gc;
    PORTF.INTCTRL = PORT_INT0LVL_LO_gc;

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
            static unsigned char count = 0;
            IMU_GYRO_RESULT_t rev;
            
            gyro_updated = 0;

            imu_gyro_read(&rev);

            count++;
            printf("%d|%d|%d|%d\n", count, rev.x, rev.y, rev.z);
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
