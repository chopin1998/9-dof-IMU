#include "common.h"
#include "uart.h"
#include "uart_process.h"
#include "util.h"
#include "imu_sensor.h"
#include "qd.h"


void uart_process_init_linebuf(LINE_BUF_t *lb)
{
    memset(lb->buf, 0x00, LINE_BUF_MAX);
    lb->index = 0;
}


signed char uart_process_tick(RAW_QUEUE_t *q, LINE_BUF_t *lb, void (*process)(void),
                              unsigned char _STX, unsigned char _ETX)
{
    signed char recv;
    
    cli();
    recv = uart_dequeue(q);
    sei();

    if (recv == -1)
    {
        return -1;
    }
    else
    {
        /* as following */
    }

    if (lb->packet_status == OUT_PACKET)
    {
        if (recv == _STX)
        {
            lb->packet_status = IN_PACKET;
            
            uart_process_init_linebuf(lb);
        }
        else
        {
            /* ignor */
        }
    }
    else
    {                           /* packet_status == IN_PACK */
        if (recv == _ETX)
        {                       /* end of a packet */
            lb->packet_status = OUT_PACKET;
            lb->buf[lb->index] = 0x00; /* padding a terminal char */

            if (process != NULL)
            {
                process();
            }

            uart_process_init_linebuf(lb);
        }
        else
        {                       /* normally */
            lb->buf[lb->index] = recv;            
            lb->index++;

            if (lb->index >= LINE_BUF_MAX)
            {                   /* a huge packet ? */
                lb->packet_status = OUT_PACKET;

                uart_process_init_linebuf(lb);
            }
        }
    }
}

void uart_process_lb_bt(void)
{
    // printf("%s", LB_BT.buf);

    P_LIST_t *para_head, *curr;
    char *p = NULL;
    unsigned char p_count = 0;

    unsigned char reg, val;
    IMU_GYRO_RESULT_t gyro_rev;
    IMU_ACCL_RESULT_t accl_rev;
    unsigned char rev;

    para_head = (P_LIST_t *)malloc(sizeof(P_LIST_t));

    p = strtok(&LB_BT.buf[0], ",");
    curr = para_head;

    while (p)
    {
        p_count++;

        curr->para = p;
        curr->next = (P_LIST_t *)malloc(sizeof(P_LIST_t));
        curr = curr->next;

        p = strtok(NULL, ",");
    }
    curr->next = NULL;

    if ( !strcmp(para_head->para, "hi") )
    {
        printf("hello\n");
    }
    else if ( !strcmp(para_head->para, "gyro") )
    {
        if ( !strcmp(para_head->next->para, "readreg") )
        {
            if (p_count == 3)
            {
                hexed_to_plain(para_head->next->next->para, &reg);
                printf("reg(0x%02x): 0x%02x\n", reg, imu_read_reg(IMU_GYRO_ADDR, reg));
            }
        }
        else if ( !strcmp(para_head->next->para, "writereg") )
        {
            if (p_count == 4)
            {
                hexed_to_plain(para_head->next->next->para, &reg);
                hexed_to_plain(para_head->next->next->next->para, &val);

                imu_write_reg(IMU_GYRO_ADDR, reg, val);
                printf("write reg(0x%02x) to 0x%02x\n", reg, val);
            }
        }
        else if ( !strcmp(para_head->next->para, "read") )
        {
            imu_gyro_read(&gyro_rev);
            
            printf("x-> %d, y-> %d, z->%d\n", gyro_rev.x, gyro_rev.y, gyro_rev.z);
        }
        else if ( !strcmp(para_head->next->para, "temp") )
        {
            printf("temp: %d\n", imu_read_reg(IMU_GYRO_ADDR, IMU_GYRO_TEMP));
        }
        else if ( !strcmp(para_head->next->para, "power") )
        {
            if (p_count == 3)
            {
                if ( !strcmp(para_head->next->next->para, "on") )
                {
                    printf("power on\n");
                    rev = IMU_GYRO_POWER_ON;
                }
                else if ( !strcmp(para_head->next->next->para, "sleep") )
                {
                    printf("power sleep\n");
                    rev = IMU_GYRO_POWER_SLEEP;
                }
                else
                {
                    printf("power off\n");
                    rev = IMU_GYRO_POWER_OFF;
                }
                
                imu_gyro_power(rev);
            }
        }
        else if ( !strcmp(para_head->next->para, "dump") )
        {
            if (p_count == 2)
            {
                printf("start dump..\n");
                imu_gyro_dump(1);
            }
            else
            {
                printf("stop dump\n");
                imu_gyro_dump(0);
            }
        }
        else
        {
        }
    }
    else if ( !strcmp(para_head->para, "accl") )
    {
        if ( !strcmp(para_head->next->para, "readreg") )
        {
            if (p_count == 3)
            {
                hexed_to_plain(para_head->next->next->para, &reg);
                printf("reg(0x%02x): 0x%02x\n", reg, imu_read_reg(IMU_ACCL_ADDR, reg));
            }
        }
        else if ( !strcmp(para_head->next->para, "writereg") )
        {
            if (p_count == 4)
            {
                hexed_to_plain(para_head->next->next->para, &reg);
                hexed_to_plain(para_head->next->next->next->para, &val);

                imu_write_reg(IMU_ACCL_ADDR, reg, val);
                printf("write reg(0x%02x) to 0x%02x\n", reg, val);
            }
        }
        else if ( !strcmp(para_head->next->para, "power") )
        {
            if (p_count == 3)
            {
                if ( !strcmp(para_head->next->next->para, "on") )
                {
                    printf("power on\n");
                    rev = IMU_ACCL_POWER_ON;
                }
                else
                {
                    printf("power off\n");
                    rev = IMU_ACCL_POWER_OFF;
                }

                imu_accl_power(rev);
            }
        }
        else if ( !strcmp(para_head->next->para, "read") )
        {
            imu_accl_read(&accl_rev);
            
            printf("x-> %d, y-> %d, z->%d\n", accl_rev.x>>4, accl_rev.y>>4, accl_rev.z>>4);
        }
        else if ( !strcmp(para_head->next->para, "dump") )
        {
            if (p_count == 2)
            {
                printf("start dump..\n");
                imu_accl_dump(1);
            }
            else
            {
                printf("stop dump\n");
                imu_accl_dump(0);
            }
        }
    }
    else if ( !strcmp(para_head->para, "magn") )
    {
        if ( !strcmp(para_head->next->para, "readreg") )
        {
            if (p_count == 3)
            {
                hexed_to_plain(para_head->next->next->para, &reg);
                printf("reg(0x%02x): 0x%02x\n", reg, imu_read_reg(IMU_MAGN_ADDR, reg));
            }
        }
        else if ( !strcmp(para_head->next->para, "writereg") )
        {
            if (p_count == 4)
            {
                hexed_to_plain(para_head->next->next->para, &reg);
                hexed_to_plain(para_head->next->next->next->para, &val);

                imu_write_reg(IMU_MAGN_ADDR, reg, val);
                printf("write reg(0x%02x) to 0x%02x\n", reg, val);
            }
        }
    }
    else if ( !strcmp(para_head->para, "qd") )
    {
        if ( !strcmp(para_head->next->para, "read") )
        {
            printf("qd: %d\n", QD_READ());
        }
        else if ( !strcmp(para_head->next->para, "clear") )
        {
            QD_CLEAR();
            printf("qd cleaned\n");
        }
    }
    else
    {
        printf("UNKNOW FUNC\n");
    }

    p_list_clear(para_head);
}

