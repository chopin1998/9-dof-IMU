#include "common.h"
#include "util.h"


void hexed_to_plain(const unsigned char *hexed, unsigned char *plain)
{
    unsigned int len = strlen(hexed);

    unsigned int i, j;
    for (i=0,j=0; i<len; i+=2,j++)
    {
        if (hexed[i] >= '0' && hexed[i] <= '9')
        {
            plain[j] = (hexed[i] & 0x0f) << 4;
        }
        else if (hexed[i] >= 'a' && hexed[i] <= 'f')
        {
            plain[j] = (hexed[i] - 0x57) << 4;
        }
        else if (hexed[i] >= 'A' && hexed[i] <= 'F')
        {
            plain[j] = (hexed[i] - 0x37) << 4;
        }
        else
        {
            plain[j] = 0x00;
            continue;
        }

        if (hexed[i+1] >= '0' && hexed[i+1] <= '9')
        {
            plain[j] |= (hexed[i+1] & 0x0f);
        }
        else if (hexed[i+1] >= 'a' && hexed[i+1] <= 'f')
        {
            plain[j] |= (hexed[i+1] - 0x57);
        }
        else if (hexed[i+1] >= 'A' && hexed[i+1] <= 'F')
        {
            plain[j] |= (hexed[i+1] - 0x37);
        }
        else
        {
            plain[j] = 0x00;
            continue;
        }
    }

    plain[j] = 0x00;
}

void plain_to_hexed(const unsigned char *plain, const unsigned int len,
                    unsigned char *hexed)
{
    unsigned int i;
    unsigned char tmp;
    
    for (i=0; i<len; i++)
    {
        tmp = plain[i] >> 4;
        if (tmp < 10)
        {
            hexed[i*2] = tmp + 0x30;
        }
        else
        {                       /* a - f */
            hexed[i*2] = tmp + 0x37;
        }

        tmp = plain[i] & 0x0f;
        if (tmp < 10)
        {
            hexed[i*2+1] = tmp + 0x30;
        }
        else
        {                       /* a - f */
            hexed[i*2+1] = tmp + 0x37;
        }
    }
    hexed[i*2] = 0x00;
}

void p_list_clear(P_LIST_t *head)
{
    P_LIST_t *curr, *tmp;

    curr = head;
    while (curr)
    {
        tmp = curr->next;
        free(curr);
        curr = tmp;
    }
}
