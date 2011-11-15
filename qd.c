#include "common.h"
#include "qd.h"



void qd_init(void)
{
    /*
      config port.pins as input
      config pins to level sensing
      select first pin for event
      enable qd and df for event
      select event source fro tc
      select perid register of tc to (n*4-1)
      enable tc
     */
    
    QD_PORT.DIRCLR = QD_PINA | QD_PINB;

    PORTCFG.MPCMASK = QD_PINA | QD_PINB;
    QD_PORT.PIN0CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_LEVEL_gc;

    EVSYS.CH0MUX = QD_EV_MUX;

    EVSYS.CH0CTRL = EVSYS_QDEN_bm | EVSYS_DIGFILT_2SAMPLES_gc;

    QD_TCC.CTRLD = TC_EVACT_QDEC_gc | QD_TC_EV;

    QD_TCC.PER = 0xffff;

    QD_TCC.CTRLA = TC_CLKSEL_DIV1_gc;
}

