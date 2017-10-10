#ifndef _PDP8_H_
#define _PDP8_H_	1

typedef enum {FALSE, TRUE} bool;
typedef char * STRING;
typedef unsigned char byte;
typedef unsigned short WORD;
typedef unsigned short EFFECTIVE_ADDRESS;

/* global environment for PDP-8 simulator */

#define pdp8_WORD	12
#define	pdp8_MEMSIZE	4096
#define pdp8_MAXVAL	0xFFF
#define pdp8_NULL	0xFFFF

/* 
 * PDP-8 Architecture
 * program counter, accumulator, memory data register, memory address register
 * 4K of 12-bit words
 *
 */
WORD pdp8_PC, pdp8_ACC, pdp8_MD, pdp8_MA;
WORD pdp8_MEM[pdp8_MEMSIZE];
WORD old_PC;


/*
 * System state for log output.
 *
 */
long long int time = 0;
FILE * pdp8_obj_file;
bool VERBOSE = FALSE;
bool HALTED = FALSE;


/*************************************************************************************/

#include "init.c"
#include "decode.c"
#include "execute.c"

#endif
