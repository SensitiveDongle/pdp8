/*
 *  Decoding of pdp8 instructions.
 *
 *  pdp8 instructions are encoded in this way:
 *  (TODO)
 *
 *  Decoding happens in this way:
 *  (1) An instruction is fetched as a 12-bit WORD from pdp8_MEM[pdp8_PC]
 *  (2) Opcode is extracted from instruction and used to index into the
 *  instruction table.
 *  (3) Structure members are evaluated (memory resolution, name string
 *  construction)
 *  (4) Instruction is passed up for execution.
 *
 */


// abbreviations for pdp8 opcodes, numeric values correspond to position of that
// instruction in instruction_table
#define OP_AND	0
#define OP_TAD	1
#define OP_ISZ	2
#define	OP_DCA	3
#define OP_JMS	4
#define OP_JMP	5
#define OP_IOT	6
#define OP_OPR	7

// internal representation of an instruction
typedef struct instruction {
	STRING name;                          // for logging
	byte time;                            // how many cycles the instruction uses
	EFFECTIVE_ADDRESS addr;               //
	void (* function)(EFFECTIVE_ADDRESS); // execution code
} pdp8_INSTRUCTION;

// hardcoded instruction table
pdp8_INSTRUCTION instruction_table[8];

// needed for function pointer assignment 
#ifndef _PDP_EXECUTE_
#include "execute.c"
#endif

// initialize instruction_table members
void initialize_instruction_table(void) {
pdp8_INSTRUCTION pdp8_AND = { .name = "AND", .time = 2, .addr = pdp8_NULL, &pdp8_AND_function};
instruction_table[OP_AND] = pdp8_AND;

pdp8_INSTRUCTION pdp8_TAD = { .name = "TAD", .time = 2, .addr = pdp8_NULL, &pdp8_TAD_function};
instruction_table[OP_TAD] = pdp8_TAD;

pdp8_INSTRUCTION pdp8_ISZ = { .name = "ISZ", .time = 2, .addr = pdp8_NULL, &pdp8_ISZ_function};
instruction_table[OP_ISZ] = pdp8_ISZ;

pdp8_INSTRUCTION pdp8_DCA = { .name = "DCA", .time = 2, .addr = pdp8_NULL, &pdp8_DCA_function};
instruction_table[OP_DCA] = pdp8_DCA;

pdp8_INSTRUCTION pdp8_JMS = { .name = "JMS", .time = 2, .addr = pdp8_NULL, &pdp8_JMS_function};
instruction_table[OP_JMS] = pdp8_JMS;

pdp8_INSTRUCTION pdp8_JMP = { .name = "JMP", .time = 1, .addr = pdp8_NULL, &pdp8_JMP_function};
instruction_table[OP_JMP] = pdp8_JMP;

pdp8_INSTRUCTION pdp8_IOT = { .name = "IOT", .time = 1, .addr = pdp8_NULL, &pdp8_IOT_function};
instruction_table[OP_IOT] = pdp8_IOT;

pdp8_INSTRUCTION pdp8_OPR = { .name = "<operate>", .time = 1, .addr = pdp8_NULL, &pdp8_OPR_function};
instruction_table[OP_OPR] = pdp8_OPR;
}

// decoding functions
pdp8_INSTRUCTION * pdp8_decode(WORD);
EFFECTIVE_ADDRESS get_effective_address(WORD);
byte get_opcode(WORD);
