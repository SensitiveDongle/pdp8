#include <string.h>

void pdp8_AND_function(EFFECTIVE_ADDRESS);
void pdp8_TAD_function(EFFECTIVE_ADDRESS);
void pdp8_ISZ_function(EFFECTIVE_ADDRESS);
void pdp8_DCA_function(EFFECTIVE_ADDRESS);
void pdp8_JMS_function(EFFECTIVE_ADDRESS);
void pdp8_JMP_function(EFFECTIVE_ADDRESS);
void pdp8_IOT_function(EFFECTIVE_ADDRESS);
void pdp8_OPR_function(EFFECTIVE_ADDRESS);

	void pdp8_OPR_CLA();
	void pdp8_OPR_CLL();
	void pdp8_OPR_CMA();
	void pdp8_OPR_CML();
	void pdp8_OPR_RAR(WORD);
	void pdp8_OPR_RAL(WORD);
	void pdp8_OPR_IAC();

	void pdp8_OPR_SMA(bool *);
	void pdp8_OPR_SZA(bool *);
	void pdp8_OPR_SNL(bool *);
	void pdp8_OPR_RSS(bool *);
	void pdp8_OPR_OSR();
	void pdp8_OPR_HLT();

STRING get_OPR_name(WORD);
EFFECTIVE_ADDRESS get_EA(WORD);
void set_PC(unsigned short);
void update_PC(unsigned short);
void clean_ACC(void);
