/* 
   Assembler for PDP-8.  Memory and object file creation header file
*/

#define HEADER	"OBJ8"

/* variables */
Address entry_point;


/* block struct used for writing chunks to file */
typedef struct _block_ {

	/* number of bytes to write (including this byte for size and 2 more bytes
		for our starting point */
	unsigned char size;

	/* address in memory to start reading <size-3> bytes from */
	Address starting_point;
} BLOCK;


/* prototypes */
void Clear_Object_Code(void);
void Define_Object_Code(Address addr, INST inst, Boolean redefine);
INST Fetch_Object_Code(Address addr);
void Output_Object_Code(void);

Address split_addr(Address);
void write_addr(Address);

BLOCK get_block(Address *);
void write_block(BLOCK);



void printBits(Address *);
