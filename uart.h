#ifndef _UART_H
#define _UART_H 1

#include "common.h"


#define UART_BT_RXC_vect    USARTD0_RXC_vect
#define UART_BT   USARTD0


#define UART_RAW_RXBUF_MAX 512
typedef struct
{
    unsigned char queue[UART_RAW_RXBUF_MAX];
    unsigned short rx_in, rx_out;
    unsigned short rx_count;
} RAW_QUEUE_t;
volatile RAW_QUEUE_t Q_USB, Q_MDB, Q_VM, Q_VC, Q_BT;


void uart_init(USART_t *dev);
void uart_queue_init(RAW_QUEUE_t *q);

void uart_sendc(USART_t *dev, unsigned char data);
void uart_sends(USART_t *dev, const char *data);
signed char uart_dequeue(RAW_QUEUE_t *q);
void uart_join(RAW_QUEUE_t *q, unsigned char data);


#endif
