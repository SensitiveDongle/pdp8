#include <stdio.h>
#include <stdlib.h>
#include "pdp8.h"

int main(int argc, STRING * argv){

	/* initialize the global environment */
	pdp8_init(argc, argv);

	/* fetch, decode, execute loop until halt */
	while(!HALTED){

		/* fetch, decode, update pc */
		pdp8_INSTRUCTION* i = pdp8_decode(pdp8_MEM[pdp8_PC]);
		update_PC(1);

		/* if we have an operate instruction the name will have to be
			built */
		bool fetched_name = FALSE;
		if(!strcmp(i->name, "<operate>")){
			fetched_name = TRUE;
			i->name = get_OPR_name(i->addr);
		}

		/* execute the instruction */
		execute(i);
		if(VERBOSE)
			show_trace(i);

		/* if we allocated space for the name, clean up and reset */
		if(fetched_name){
			free(i->name);
			i->name = "<operate>";
		}

	}
	exit(0);
}
