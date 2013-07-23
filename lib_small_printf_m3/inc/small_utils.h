/*
===============================================================================
 File small_printf.h: Header file for small printf by Georges Menie and Christian Ettinger
 Project DebugTips_1343: Demonstration of debugging techniques for LPCXpresso
 Copyright (C) 2010 NXP Semiconductors
===============================================================================
*/

#ifndef SMALLUTILS_H_INCLUDED
#define SMALLUTILS_H_INCLUDED

int small_strlen(const char *str);
int small_strcmp(const char *str1, const char *str2);
int small_stricmp(const char *str, const char *str2);
void small_strim(char *str);

unsigned long gethex(const char *s);

double small_fmodf(double f, double div);

#endif
