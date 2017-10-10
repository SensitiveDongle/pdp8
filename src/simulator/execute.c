#ifndef _PDP8_EXECUTE_
#define _PDP8_EXECUTE_	1

#define trace	"Time %lld: PC=0x%03X instruction = 0x%03X (%s), rA = 0x%03X, rL = %d\n"

#ifndef _PDP8_H_
#include "pdp8.h"
#endif

#ifndef _PDP8_DECODE_
#include "decode.c"
#endif

#include "execute.h"

void execute(pdp8_INSTRUCTION * i){
	i->function(i->addr);
}

void pdp8_AND_function(EFFECTIVE_ADDRESS address){
	short LB = pdp8_ACC & 0x1000;
	pdp8_ACC = pdp8_MEM[address] & pdp8_ACC;
	pdp8_ACC |= LB;

	instruction_table[OP_AND].name = (pdp8_MEM[old_PC] & 0x100) ? "AND I" : "AND";
	if(pdp8_MEM[old_PC] & 0x100)
		time++;
}

void pdp8_TAD_function(EFFECTIVE_ADDRESS address){
	short LB = pdp8_ACC & 0x1000;

	pdp8_ACC &= 0xFFF;
	bool link = !(LB == 0);

	int i = pdp8_MEM[address];
	while(i-- > 0){
		pdp8_ACC++;
		pdp8_ACC %= 0xFFF+1;
		if(pdp8_ACC == 0)
			link = !link;
	}

	pdp8_ACC |= link ? 0x1000 : 0x0;

	instruction_table[OP_TAD].name = (pdp8_MEM[old_PC] & 0x100) ? "TAD I" : "TAD";
	if(pdp8_MEM[old_PC] & 0x100)
		time++;
}

void pdp8_ISZ_function(EFFECTIVE_ADDRESS address){
	instruction_table[OP_ISZ].name = (pdp8_MEM[old_PC] & 0x100) ? "ISZ I" : "ISZ";
	if(pdp8_MEM[old_PC] & 0x100)
		time++;

	pdp8_MEM[address]++;
	pdp8_MEM[address] &= 0xFFF;
	if(pdp8_MEM[address] == 0){
		pdp8_PC++;
		pdp8_PC %= (pdp8_MAXVAL+1);
	}
}

void pdp8_DCA_function(EFFECTIVE_ADDRESS address){
	pdp8_MEM[address] = pdp8_ACC & 0xFFF;
	pdp8_ACC &= 0x1000;

	instruction_table[OP_DCA].name = (pdp8_MEM[old_PC] & 0x100) ? "DCA I" : "DCA";
	if(pdp8_MEM[old_PC] & 0x100)
		time++;
}

void pdp8_JMS_function(EFFECTIVE_ADDRESS address){
	old_PC = pdp8_PC-1;
	pdp8_MEM[address] = pdp8_PC;
	set_PC(address + 1);

	// quick and dirty hack to get the indirect reference jmp name right
	instruction_table[OP_JMS].name = (pdp8_MEM[old_PC] & 0x100) ? "JMS I" : "JMS";
	if(pdp8_MEM[old_PC] & 0x100)
		time++;
}

void pdp8_JMP_function(EFFECTIVE_ADDRESS address){
	old_PC = pdp8_PC-1;
	set_PC(address);

	// quick and dirty hack to get the indirect reference jmp name right
	instruction_table[OP_JMP].name = (pdp8_MEM[old_PC] & 0x100) ? "JMP I" : "JMP";
	if(pdp8_MEM[old_PC] & 0x100)
		time++;
}

// (the effective address here is really the word in the PC -- see the get_effective_address
// function for clarification)
void pdp8_IOT_function(EFFECTIVE_ADDRESS address){
	unsigned char device = (address & 0x1F8) >> 3;
	if(device == 3){
		instruction_table[OP_IOT].name = "IOT 3";
		short LB = pdp8_ACC & 0x1000;
		pdp8_ACC = 0xFFF & getchar();
		pdp8_ACC |= LB;
	}
	else if(device == 4){
		instruction_table[OP_IOT].name = "IOT 4";
		char out = pdp8_ACC & 0xFF;
		putchar(out);
	}
	else{
		pdp8_OPR_HLT();
	}
}

// samesies on the address thing as IOT
void pdp8_OPR_function(EFFECTIVE_ADDRESS address){

	// group 1 instructions
	if(!(address & 0x100)){
		if(address & 0x80)
			pdp8_OPR_CLA();
		if(address & 0x40)
			pdp8_OPR_CLL();
		if(address & 0x20)
			pdp8_OPR_CMA();
		if(address & 0x10)
			pdp8_OPR_CML();
		if(address & 0x1)
			pdp8_OPR_IAC();
		if(address & 0x8)
			pdp8_OPR_RAR(address);
		else if(address & 0x4)
			pdp8_OPR_RAL(address);
	}
	// group 2 instructions
	else{
		bool SKIP = FALSE;
		if(address & 0x40)
			pdp8_OPR_SMA(&SKIP);
		if(address & 0x20)
			pdp8_OPR_SZA(&SKIP);
		if(address & 0x10)
			pdp8_OPR_SNL(&SKIP);
		if(address & 0x8)
			pdp8_OPR_RSS(&SKIP);
		if(address & 0x80)
			pdp8_OPR_CLA();
		if(address & 0x2)
			pdp8_OPR_HLT();

		if(SKIP){
			pdp8_PC++;
			pdp8_PC %= (pdp8_MAXVAL+1);
		}
	}
}

void pdp8_OPR_CLA(){
	short LB = pdp8_ACC & 0x1000;
	pdp8_ACC &= LB;
}
void pdp8_OPR_CLL(){
	pdp8_ACC &= 0xFFF;
}

void pdp8_OPR_CMA(){
	short LB = pdp8_ACC & 0x1000;
	pdp8_ACC &= 0xFFF;
	pdp8_ACC = ~pdp8_ACC;
	pdp8_ACC &= 0xFFF;
	pdp8_ACC |= LB;
}

void pdp8_OPR_CML(){
	pdp8_ACC &= 0x1FFF;
	pdp8_ACC ^= 0x1000;
}

void pdp8_OPR_RAR(WORD w){
	short LB = pdp8_ACC & 0x1000;
	pdp8_ACC &= 0xFFF;

	if(w & 0x2){
		pdp8_ACC = (pdp8_ACC >> 2) | (pdp8_ACC << 10);
		pdp8_ACC &= 0xFFF;
		pdp8_ACC |= LB;
	}
	else{
		pdp8_ACC = (pdp8_ACC >> 1) | (pdp8_ACC << 11);
		pdp8_ACC &= 0xFFF;
		pdp8_ACC |= LB;
	}

}	
void pdp8_OPR_RAL(WORD w){
	short LB = pdp8_ACC & 0x1000;
	pdp8_ACC &= 0xFFF;

	if(w & 0x2){
		pdp8_ACC = (pdp8_ACC << 2) | (pdp8_ACC >> 10);
		pdp8_ACC &= 0xFFF;
		pdp8_ACC |= LB;
	}
	else{
		pdp8_ACC = (pdp8_ACC << 1) | (pdp8_ACC >> 11);
		pdp8_ACC &= 0xFFF;
		pdp8_ACC |= LB;
	}

}

void pdp8_OPR_IAC(){
	pdp8_ACC++;
}

void pdp8_OPR_SMA(bool * flag){
	if(pdp8_ACC & 0x800)
		*flag = TRUE;
}
void pdp8_OPR_SZA(bool * flag){
	if(!(pdp8_ACC & 0xFFF))
		*flag = TRUE;
}

void pdp8_OPR_SNL(bool * flag){
	if(pdp8_ACC & 0x1000)
		*flag = TRUE;
}

void pdp8_OPR_RSS(bool * flag){
	*flag = !(*flag);
}

void pdp8_OPR_OSR(){
	update_PC(1);
}

void pdp8_OPR_HLT(){
	HALTED = TRUE;
}

void update_PC(unsigned short val){
	old_PC = pdp8_PC;
	while(val-- > 0){
		pdp8_PC++;
		pdp8_PC %= (pdp8_MAXVAL+1);
	}
}

void set_PC(unsigned short val){
	if(val <= pdp8_MAXVAL+1)
		pdp8_PC = val;
}


STRING get_OPR_name(WORD word){

	STRING opstring = (STRING) malloc(sizeof(char)*32);
	*opstring = '\0';
	
	bool firstadd = TRUE;
	// group 1
	if(!(word & 0x100)){
		if(word & 0x80)
			strcat(opstring, firstadd ? "CLA" : " CLA"), firstadd = FALSE;
		if(word & 0x40)
			strcat(opstring, firstadd ? "CLL" : " CLL"), firstadd = FALSE;
		if(word & 0x20)
			strcat(opstring, firstadd ? "CMA" : " CMA"), firstadd = FALSE;
		if(word & 0x10)
			strcat(opstring, firstadd ? "CML" : " CML"), firstadd = FALSE;
		if(word & 0x1)
			strcat(opstring, firstadd ? "IAC" : " IAC"), firstadd = FALSE;
		if(word & 0x8)
			strcat(opstring, firstadd ?  ((word & 0x2) ? "RTR" : "RAR") : ((word & 0x2) ? " RTR" : " RAR")), firstadd = FALSE;
		if(word & 0x4)
			strcat(opstring, firstadd ? ((word & 0x2) ? "RTL" : "RAL") : ((word & 0x2) ? " RTL" : " RAL")), firstadd = FALSE;
	}
	// group 2
	else{
		if(word & 0x40)
			strcat(opstring, firstadd ? "SMA" : " SMA"), firstadd = FALSE;
		if(word & 0x20)
			strcat(opstring, firstadd ? "SZA" : " SZA"), firstadd = FALSE;	
		if(word & 0x10)
			strcat(opstring, firstadd ? "SNL" : " SNL"), firstadd = FALSE;
		if(word & 0x8)
			strcat(opstring, firstadd ? "RSS" : " RSS"), firstadd = FALSE;
		if(word & 0x80)
			strcat(opstring, firstadd ? "CLA" : " CLA"), firstadd = FALSE;
		if(word & 0x4)
			strcat(opstring, firstadd ? "OSR" : " OSR"), firstadd = FALSE;
		if(word & 0x2)
			strcat(opstring, firstadd ? "HLT" : " HLT"), firstadd = FALSE;
	}

	return opstring;
}

void show_trace(pdp8_INSTRUCTION * i){
	time += i->time;
	fprintf(stderr, trace, time, old_PC, pdp8_MEM[old_PC], i->name, (pdp8_ACC & 0xFFF), (pdp8_ACC >> 12) & 0x1); 
}

#endif
