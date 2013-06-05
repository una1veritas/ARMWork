/*****************************************************************************
 *   $Id:: LPC_ROM_DIV.h 7228 2011-04-27 22:47:17Z nxp28548              $		 *
 *   Project: Division API Example											 *
 *																			 *
 *   Description:															 *
 *			LPC1xxx ROM Division API			 							 *
 *																			 *
 *****************************************************************************/
#ifndef LPC1xxx_DIVROM_H
#define LPC1xxx_DIVROM_H

typedef struct { int quot; int rem; } sidiv_return; 
typedef struct { unsigned quot; unsigned rem; } uidiv_return; 

typedef struct{
	/* Signed integer division */
	int			(*sidiv)	(int numerator, int denominator);
	/* Unsigned integer division */
	unsigned	(*uidiv) 	(unsigned numerator, unsigned denominator);
	/* Signed integer division with remainder */
	sidiv_return	(*sidivmod)	(int numerator, int denominator);
	/* Unsigned integer division with remainder */
	uidiv_return	(*uidivmod) (unsigned numerator, unsigned denominator);
} LPC_ROM_DIV_STRUCT;



#endif//LPC1xxx_DIVROM_H
