#include <iostream>
#include <cstdio>
#include "instruction.h"
#include "memory.h"
#include "regfile.h"
#include "I_page_table.h"
#include "I_TLB.h"
#include "D_page_table.h"
#include "D_TLB.h"

using namespace std;

unsigned int PC = 0;
void printHitAndMiss(FILE *rprt, instruction *inst, memory *memo);

int main(int argc, char *argv[])
{
    int i = 0;
	FILE *snap, *dump, *rprt;
	snap = fopen("snapshot.rpt", "w");
	dump = fopen("error_dump.rpt", "w");
	rprt = fopen("report.rpt", "w");
    instruction *inst = new instruction();
    memory *memo = new memory();
	regfile reg;
	I_page_table *ipt = new I_page_table();
	I_TLB *itlb = new I_TLB();
	D_page_table *dpt = new D_page_table();
	D_TLB *dtlb = new D_TLB();
	
	if(argc == 11){
		inst->I_setArgu(argv[1], argv[3], argv[5], argv[6], argv[7]);
		ipt->page_size = atoi(argv[3]);
		memo->D_setArgu(argv[2], argv[4], argv[8], argv[9], argv[10]);
		dpt->page_size = atoi(argv[4]);
	}
	ipt->num_of_entries = 1024/ipt->page_size;
	itlb->num_of_entries = ipt->num_of_entries/4;
	dpt->num_of_entries = 1024/dpt->page_size;
	dtlb->num_of_entries = dpt->num_of_entries/4;
	
	inst->readInstructionInput(&PC);
    memo->readMemory(&(reg.Register[29]));
	fprintf(snap, "cycle %d\n", i);
	reg.printRegister(snap);
	fprintf(snap, "PC: 0x%0.8X\n\n\n", PC*4);
    i++;


	while(inst->op != 0x3F && !reg.error[2] && !reg.error[3]){
		
		inst->decode(PC, ipt, itlb, i);
		inst->implement(&PC, &reg, memo, dpt, dtlb, i);
		if(reg.error != 0){
			if(reg.error[0]){
				reg.error[0] = false;
				fprintf(dump,  "In cycle %d: Write $0 Error\n", i);
			}if(reg.error[1]){
				reg.error[1] = false;
				fprintf(dump, "In cycle %d: Number Overflow\n", i);
			}if(reg.error[2]){
				fprintf(dump, "In cycle %d: Address Overflow\n", i);
			}if(reg.error[3]){
				fprintf(dump, "In cycle %d: Misalignment Error\n", i);
			}
		}
	
		/*if(i >= 0){
			printf("cycle = %d\n", i);
			for(int j = 0 ; j < 4 ; j++){
				printf("block:%d ", j);
				printf("valid = %d, tag = %d, MRU = %d\n", inst->I_cache_set[0].I_cache_block[j].valid, inst->I_cache_set[0].I_cache_block[j].tag, inst->I_cache_set[0].I_cache_block[j].MRU);
			}
		}*/

		if(inst->op != 0x3F && !reg.error[2] && !reg.error[3]){
			fprintf(snap, "cycle %d\n", i);
			reg.printRegister(snap);
			fprintf(snap, "PC: 0x%0.8X\n\n\n", PC*4);
			/*fprintf(snap, "op code: 0x%0.2X\n", inst.op);
			if(inst.op == 0x00) fprintf(snap, "funct: 0x%0.2X\n\n\n", inst.funct);*/
			i++;
		}
	}
	
	printHitAndMiss(rprt, inst, memo);

	fclose(snap);
	fclose(dump);
	fclose(rprt);
	return 0;
}

void printHitAndMiss(FILE *rprt, instruction *inst, memory *memo){
	fprintf(rprt, "ICache :\n");
	fprintf(rprt, "# hits: %u\n", inst->I_cache_hit );
	fprintf(rprt, "# misses: %u\n\n", inst->I_cache_miss );
	fprintf(rprt, "DCache :\n");
	fprintf(rprt, "# hits: %u\n", memo->D_cache_hit );
	fprintf(rprt, "# misses: %u\n\n", memo->D_cache_miss );

	fprintf(rprt, "ITLB :\n");
	fprintf(rprt, "# hits: %u\n", inst->I_TLB_hit );
	fprintf(rprt, "# misses: %u\n\n", inst->I_TLB_miss );
	fprintf(rprt, "DTLB :\n");
	fprintf(rprt, "# hits: %u\n", memo->D_TLB_hit );
	fprintf(rprt, "# misses: %u\n\n", memo->D_TLB_miss );

	fprintf(rprt, "IPageTable :\n");
	fprintf(rprt, "# hits: %u\n", inst->I_page_table_hit );
	fprintf(rprt, "# misses: %u\n\n", inst->I_page_table_miss );
	fprintf(rprt, "DPageTable :\n");
	fprintf(rprt, "# hits: %u\n", memo->D_page_table_hit );
	fprintf(rprt, "# misses: %u\n\n", memo->D_page_table_miss );
}
