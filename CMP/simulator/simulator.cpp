#include <iostream>
#include <cstdio>
#include "instruction.h"
#include "memory.h"
#include "regfile.h"

using namespace std;

unsigned int PC = 0;

int main()
{
    int i = 0;
	FILE *snap, *dump;
	snap = fopen("snapshot.rpt", "w");
	dump = fopen("error_dump.rpt", "w");
    instruction inst;
    memory memo;
	regfile reg;
	I_page_table ipt;
	I_TLB itlb;

	inst.readInstructionInput(&PC);
    memo.readMemory(&(reg.Register[29]));
	fprintf(snap, "cycle %d\n", i);
	reg.printRegister(snap);
	fprintf(snap, "PC: 0x%0.8X\n\n\n", PC*4);
    i++;



	while(inst.op != 0x3F && !reg.error[2] && !reg.error[3]){
		inst.decode(PC, &ipt, &itlb);
		inst.implement(&PC, &reg, memo.D_memory);
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
		if(inst.op != 0x3F && !reg.error[2] && !reg.error[3]){
			fprintf(snap, "cycle %d\n", i);
			reg.printRegister(snap);
			fprintf(snap, "PC: 0x%0.8X\n\n\n", PC*4);
			/*fprintf(snap, "op code: 0x%0.2X\n", inst.op);
			if(inst.op == 0x00) fprintf(snap, "funct: 0x%0.2X\n\n\n", inst.funct);*/
			i++;
		}
	}

	return 0;
}
