/* 
   Assembler for PDP-8.  Memory and object file creation. 
*/

#include "asm8.h"
#include "objmem.h"


/* ***************************************************************** */
/*                                                                   */
/*                                                                   */
/* ***************************************************************** */

/* we want to assemble instructions.  We could assemble and output them
   all at once.  But we have a problem of forward references.  So we
   keep the (partially) assembled instructions in an array of them,
   essentially simulated memory.  That allows us to come back and 
   fix them up when the forward reference is resolved.

   We need to know which memory locations are from assembled
   instructions, and which are just empty; so each memory location
   has a bit (defined/not defined).
*/

INST     memory[4096];
Boolean defined[4096];
Address entry_point = 0;


void Clear_Object_Code(void)
{
    int i;
    for (i = 0; i < 4096; i++)
        {
            defined[i] = FALSE;
        }
}

void Define_Object_Code(Address addr, INST inst, Boolean redefine)
{
    if (debug)
        fprintf(stderr, "object code: 0x%03X = 0x%03X\n", addr, inst);
    if (defined[addr] && !redefine)
        {
            fprintf(stderr, "redefined memory location: 0x%03X: was 0x%03X; new value 0x%03X\n",
                    addr, memory[addr], inst);
            number_of_errors += 1;
        }
                
    defined[addr] = TRUE;
    memory[addr] = inst;
}

INST Fetch_Object_Code(Address addr)
{
    INST inst;

    if (defined[addr])
        inst = memory[addr];
    else
        inst = 0;

    if (debug)
        fprintf(stderr, "read object code: 0x%03X = 0x%03X\n", addr, inst);
    return(inst);
}


void Output_Object_Code(void)
{

	if(!print_ascii){
	/* print our binary file header */
	fwrite(HEADER, sizeof(char), sizeof(HEADER)-1, output);

	/* print the entry point */
	write_addr(split_addr(entry_point));

	Address i;
	for( i = 0; i < 4096; i++)
		if(defined[i])
			write_block(get_block(&i));
	}
	else{
    	/* original method to output code in ASCII format */
		fprintf(output, "EP: %03X\n", entry_point);
    		int i;
   		for (i = 0; i < 4096; i++)
       		{
            		if (defined[i])
               		fprintf(output, "%03X: %03X\n", i, memory[i]);
        	}
	}

}

/* get a block starting from memory address addr */
BLOCK get_block(Address * addr){

	BLOCK b = { .size = 0, .starting_point = *addr};

	int i;
	for(i = 0; i < 126 && (*addr+i) < 4096 && defined[*addr+i]; i++)

	b.size = (i+1)*2 + 3;
	*addr += (i-1);

	return b;
}

void write_block(BLOCK b){

	/* write the size of this block */
	fwrite(&b.size, sizeof(char), 1, output);

	/* write the starting address of the block */
	write_addr(split_addr(b.starting_point));

	/* write values */
	int i;
	for(i = 0; i < (b.size-3)/2; i++)
		write_addr(split_addr(memory[b.starting_point+i]));
}

/* split a 12bit address into two bytes */
/* reads only the lower 12 bits of addr */
/* the value is returned as an Address, which is a 2byte short */
Address split_addr(Address addr){

	/* upper */
	Address new_addr = (addr & 0xFC0) << 2;

	/* lower */
	new_addr |= (addr & 0x3F);

	/* we need to swap the two halves since fwrite goes right-left */
	char u = new_addr >> 8;
	new_addr = (new_addr << 8) | u;

	return new_addr;
}

void printBits(Address * a){
	int i = sizeof(*a)*8 - 1;
	while(i >= 0){
		*a & (1 << i) ? putchar('1') : putchar('0');
		i--;
	}
	putchar('\n');
}

/* write the address using binary fwrite */
void write_addr(Address  addr){
	fwrite(&addr, sizeof(Address), 1, output);
}
	
