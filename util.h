#ifndef _UTIL_H
#define _UTIL_H


typedef struct _p_list
{
    char *para;
    struct _p_list *next;
} P_LIST_t;
void p_list_clear(P_LIST_t *head);



void hexed_to_plain(const unsigned char *hexed, unsigned char *plain);
void plain_to_hexed(const unsigned char *plain, const unsigned int len,
                    unsigned char *hexed);



#endif
