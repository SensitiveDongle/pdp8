#ifndef _PDP8_INIT_
#define _PDP8_INIT_	1

#ifndef _PDP8_H_
#include "pdp8.h"
#endif

#include <math.h>
#include "decode.c"

void pdp8_init(int, STRING *);
void pdp8_mem_init();
void mem_write(WORD, WORD);

int read_object_file(FILE *);
int get_EP(FILE *);
int read_line(FILE *);
int read_word_from_file(int *, FILE *);
int skip_line(int *, FILE *);
int skip_whitespace(int *, FILE *);
int xval(int);

void scan_args(int, STRING *);
void parse_flags(STRING);

void quit(STRING, int);

/***************************************************************************************/
/*										       */
/*		MEMORY INITIALIZATION						       */
/*										       */
/***************************************************************************************/

void pdp8_init(int argc, STRING * argv){
	scan_args(argc, argv);
	HALTED = FALSE;
	pdp8_mem_init();
}

void pdp8_mem_init(){


	pdp8_PC = read_object_file(pdp8_obj_file);
	if(pdp8_PC == -1 || pdp8_PC > pdp8_MAXVAL)
		quit("no entry point found / invalid entry point specified", -1);

	pdp8_ACC = pdp8_MD = pdp8_MA = 0;
	initialize_instruction_table();
}

void mem_write(WORD loc, WORD val){
	if(loc > pdp8_MAXVAL){
		fprintf(stderr, "bad write...\n");
		return;
	}

	pdp8_MEM[loc] = val;
}

/***************************************************************************************/
/*										       */
/*		READING THE OBJECT FILE						       */
/*										       */
/***************************************************************************************/

int read_object_file(FILE * input){

	int ep = get_EP(input);

	rewind(input);

	int c;
	while( (c = read_line(input)) != -1 ){
	}
	fclose(pdp8_obj_file);

	return ep;
}

int get_EP(FILE * input){

	int c = getc(input);
	int ep = -1;
	bool found = FALSE;
	while(c != EOF && !found){
		if(c == 'E' || c == 'e'){
			c = getc(input);
			if(c == 'P' || c == 'p'){
				skip_whitespace(&c, input);
				ep = read_word_from_file(&c, input);
				if((unsigned) ep <= pdp8_MAXVAL)
					found = TRUE;
				else
					ep = -1;
			}
		}
		skip_line(&c, input);
		c = getc(input);
	}
	return ep;
}
	

int read_line(FILE * input){

	// cursor
	int c, loc, val;
	val = pdp8_NULL;
	loc = 0;

	skip_whitespace(&c, input);
	loc = read_word_from_file(&c, input);

	skip_whitespace(&c, input);
	val = read_word_from_file(&c, input);

	if(!(loc == -1 || val == -1))
		mem_write(loc, val);

	return skip_line(&c, input);
}

int read_word_from_file(int * c, FILE * input){

	int val = 0;
	int i = 3;
	while(i-- > 0){
		if(isxdigit((*c))){
			val += (xval(*c) * (int)pow(16, i));
		}
		else return -1;
		*c = getc(input);
	}
	return val;
}

int xval(int c){
	switch(c){
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'A':
		case 'a':
			return 10;
		case 'B':
		case 'b':
			return 11;
		case 'c':
		case 'C':
			return 12;
		case 'D':
		case 'd':
			return 13;
		case 'E':
		case 'e':
			return 14;
		case 'F':
		case 'f':
			return 15;
		default:
			return -1;
	}
}

int skip_whitespace(int * c, FILE * input){
	*c = getc(input);
	while( isblank(*c)  || *c == ':')
		*c = getc(input);
}

int skip_line(int * c, FILE * input){
	while( *c != EOF && *c != '\n' ){
		*c = getc(input);
	}

	if( *c == EOF )
		return -1;
	else
		return 1;
}

/***************************************************************************************/
/*										       */
/*		COMMAND LINE ARGUMENT PARSING					       */
/*										       */
/***************************************************************************************/

void scan_args(int argc, STRING * argv){
	if(argc < 2)
		quit("no object file provided", -1);

	bool scan_complete = FALSE;

	while(argc-- > 1 && !scan_complete){
		if( *(++argv)[0] == '-' )
			parse_flags(*argv);
		else{
			pdp8_obj_file = fopen(*(argv), "r");
			scan_complete = TRUE;
		}
	}

	if(pdp8_obj_file == NULL){
		fprintf(stderr, "pdp8: could not open '%s'; quitting...\n", *argv);
		exit(1);
	}
}

void parse_flags(STRING s){
	while(*(++s) != '\0')
		switch(*s){
			case 'v':
				VERBOSE = TRUE;
				break;
			default:
				fprintf(stderr, "pdp8: invalid option -- '%c'\n", *s);
				quit("(usage) pdp8 [-d] <obj_file>", 1);
		}
}

void quit(STRING message, int status){
	fprintf(stderr, "pdp8: %s; quitting...\n", message);
	exit(status);
}

#endif
