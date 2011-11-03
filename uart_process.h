#ifndef _UART_PROCESS_H
#define _UART_PROCESS_H 1


#define STX    '['               // 0x02
#define ETX    ']'               // 0x03


enum {IN_PACKET, OUT_PACKET};


#define LINE_BUF_MAX 128
typedef struct
{
    unsigned char packet_status;
    
    unsigned char buf[LINE_BUF_MAX];
    unsigned char index;
} LINE_BUF_t;
LINE_BUF_t LB_BT;


void uart_process_init_linebuf(LINE_BUF_t *lb_t);
signed char uart_process_tick(RAW_QUEUE_t *q, LINE_BUF_t *lb, void (*process)(void),
                              unsigned char _STX, unsigned char _ETX);


void uart_process_lb_bt(void);



#endif
