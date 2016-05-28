#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "I_TLB.h"

I_TLB::I_TLB(){
	for(int i = 0 ; i < num_of_entries ; i++){
		I_TLB_table[i].valid = false;
		I_TLB_table[i].usedPCCycle = 0;
	}
}

bool I_TLB::checkInTLB(unsigned int virtual_page_number){
	for(int i = 0 ; i < num_of_entries ; i++){
		if(I_TLB_table[i].valid && (I_TLB_table[i].tag == virtual_page_number)){
			return true;
		}
	}
	return false;
}
	
unsigned int I_TLB::readFromTLB(unsigned int virtual_page_number){
	for(int i = 0 ; i < num_of_entries ; i++){
		if(I_TLB_table[i].tag == virtual_page_number){
			return I_TLB_table[i].physical_page_number;
		}
	}
}

unsigned int I_TLB::findUsableTLBIndex(){
	unsigned int minCycle = 10000000;
	unsigned int indexWait = 0;
	for(int i = 0 ; i < num_of_entries ; i++){
		if(I_TLB_table[i].valid == false){
			return (unsigned int)i;
		}
		else{
			if(I_TLB_table[i].usedPCCycle < minCycle){
				minCycle = I_TLB_table[i].usedPCCycle;
				indexWait = i;
			}
		}
	}
	return indexWait;
}

void I_TLB::updateUsedPC(int counter, unsigned int virtual_page_number){
	for(int i = 0 ; i < num_of_entries ; i++){
		if(I_TLB_table[i].tag == virtual_page_number){
			I_TLB_table[i].usedPCCycle = counter;
		}
	}
}
	
void I_TLB::updateTLB(unsigned int index, int counter, unsigned int virtual_page_number, unsigned int ppn){
	I_TLB_table[index].valid = true;
	I_TLB_table[index].tag = virtual_page_number;
	I_TLB_table[index].physical_page_number = ppn;
	I_TLB_table[index].usedPCCycle = counter;
}