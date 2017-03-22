/*
 * =====================================================================================
 *
 *       Filename:  alpha.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/16/2017 10:00:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "capstone.h"
#include "evoasm_lib.h"
#include "evoasm-pop.h"

void run() {
	evoasm_pop_t *pop = evoasm_pop_alloc();
	//evoasm_param_t param;
    evoasm_pop_params_t param; 
	
	evoasm_pop_params_get_n_params
	evoasm_pop_params_get_param


	evoasm_pop_init(pop,EVOASM_ARCH_X64,&param);

    evoasm_pop_seed(pop);
	evoasm_program_t *program = evoasm_program_alloc();
    evoasm_pop_load_best_program(pop,program);
	evoasm_pop_get_best_loss
	evoasm_pop_eval

	//evoasm_pop_get_gen_counter
	//
	evoasm_pop_next_gen
	




	//evoasm_pop_alloc
}

void pop_params_run() {
	evoasm_pop_params_t  *pop_params = 
	evoasm_pop_params_alloc();

	//evoasm_pop_params_get_deme_size();
	//evoasm_pop_params_set_deme_size();
	//evoasm_pop_params_get_n_params()
	//evoasm_pop_params_set_get_param
	//evoasm_pop_params_get_inst
	//evoasm_pop_params_set_inst
    //evoasm_pop_params_set_program_input 
	//evoasm_pop_params_set_program_output
}

void capstone_run() {
		csh h;
		cs_open( CS_ARCH_X86, CS_MODE_LITTLE_ENDIAN, &h);
		char buf[]= "\xe9\x79\xff\xff\xff";
		cs_insn *insn;
        size_t cnt = cs_disasm(h, buf, sizeof(buf), 0, 0, &insn);
		printf("cs_disasm size %ld\n", cnt);
		for (int j = 0; j < cnt; j++) {
		    printf("0x%" PRIx64 ":\t%s\t%s\n", 
				   insn[j].address, insn[j].mnemonic, insn[j].op_str);
		}
}

void instruction_names() {
	//evoasm_enum_type(x64_inst_id);
    evoasm_arch_info_t *arch_info = evoasm_get_arch_info(EVOASM_ARCH_X64);
    //uint64_t features = evoasm_arch_info_get_features(archinfo);
    uint64_t flags,operand_types,reg_types;	
    uint64_t features;
	evoasm_x64_inst_id_t insts[64];
    evoasm_x64_insts(flags,features,operand_types,reg_types,insts);

}

int main(int argc, char *argv[]) {
  printf("alpha first\n");
  FILE *lf = fopen("alpha.log","w+");
  evoasm_init(argc,argv,lf);
  capstone_run();
  lf = NULL;
  return 0;
}
