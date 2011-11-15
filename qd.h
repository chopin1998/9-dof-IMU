#ifndef _QD_H
#define _QD_H

#define QD_PORT  PORTD
#define QD_PINA  PIN0_bm
#define QD_PINB  PIN1_bm
#define QD_EV_MUX EVSYS_CHMUX_PORTD_PIN0_gc
#define QD_TCC   TCC0
#define QD_TC_EV TC_EVSEL_CH0_gc

#define QD_READ() QD_TCC.CNT
#define QD_CLEAR() do { QD_TCC.CNT = 0; } while (0)


#endif
