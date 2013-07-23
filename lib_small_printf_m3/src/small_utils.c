/*
===============================================================================
 File small_utils.c: Small utils for LPC111x and LPC134x
 Project DebugTips_1343: Demonstration of debugging techniques for LPCXpresso
 Copyright (C) 2010 NXP Semiconductors
===============================================================================
*/

#include "small_utils.h"

int small_strlen(const char *str)
{
    const char *p = str;

    while(*p)
        p++;

    return p-str;
}

int small_strcmp(const char *str1, const char *str2)
{
    while(*str1 && *str2 && (*str1)==(*str2))
        str1++,str2++;
    return *str2 - *str1;
}

#define CMPIC(a, b) (((a)>='A' && (a)<='Z' ? (a)-'A' + 'a' : (a)) == ((b)>='A' && (b)<='Z' ? (b)-'A' + 'a' : (b)))
int small_stricmp(const char *str1, const char *str2)
{
    while(*str1 && *str2 && CMPIC(*str1,*str2))
        str1++,str2++;
    return *str2 - *str1;
}

void small_strim(char *str)
{
    char *p = &str[small_strlen(str)];

    while(p-- >= str)
    {
        if(*p == '\r' || *p == '\n' || *p == '\t' || *p == ' ')
            *p = 0;
    }
}

unsigned long gethex(const char *s)
{
    unsigned long value = 0;
    unsigned char n;

    while(1)
    {

        // if letter from a-f convert to uppercase
        if(*s >= 'a' && *s <= 'f')
            n = *s - 'a' + 10;
        else if(*s >= 'A' && *s <= 'F')
            n = *s - 'A' + 10;
        else if(*s >= '0' && *s <= '9')
            n = *s - '0';
        else
            return value;

        value = (value<<4) | n;
        s++;
    }
    return value;
}

double small_fmodf(double f, double div)
{
    if(f<0)
        f = -f;
    if(div<0)
        div = -div;
    while(f>div)
        f -= div;
    return f;
}
