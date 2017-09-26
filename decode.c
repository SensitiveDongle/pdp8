#ifndef _PDP8_DECODE_
#define _PDP8_DECODE_	1

#ifndef _PDP8_H_
#include "pdp8.h"
#endif

#include "decode.h"

pdp8_INSTRUCTION * pdp8_decode(WORD word){
	pdp8_INSTRUCTION * instruction = instruction_table+get_opcode(word);
	instruction->addr = get_effective_address(word);
	return instruction;
}

EFFECTIVE_ADDRESS get_effective_address(WORD word){

	// if it's an operate  or i/o instruction we can just return
	if(get_opcode(word) == OP_OPR || get_opcode(word) == OP_IOT){
		return word;
	}

	short addr = 0;
	if( word & 0x80 ) {
		addr = (word & 0x7F) | (pdp8_PC & 0xF80);
	}
	else{
		addr = word & 0x7F;
	}

	// direct addressing
	if( word & 0x100 ){
		return pdp8_MEM[addr];
	}
	else
		return addr;
}

byte get_opcode(WORD word){
	return (word & 0xFFF) >> 9;
}

#endif
