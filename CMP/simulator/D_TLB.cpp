#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "D_TLB.h"

D_TLB::D_TLB(){
	num_of_entries = 100000;
	for(int i = 0 ; i < num_of_entries ; i++){
		D_TLB_table[i].valid = false;
		D_TLB_table[i].usedPCCycle = 0;
	}
}

bool D_TLB::checkInTLB(unsigned int virtual_page_number){
	for(int i = 0 ; i < num_of_entries ; i++){
		if(D_TLB_table[i].valid && (D_TLB_table[i].tag == virtual_page_number)){
			return true;
		}
	}
	return false;
}

unsigned int D_TLB::readFromTLB(unsigned int virtual_page_number){
	for(int i = 0 ; i < num_of_entries ; i++){
		if(D_TLB_table[i].valid && D_TLB_table[i].tag == virtual_page_number){
			return D_TLB_table[i].physical_page_number;
		}
	}
}

unsigned int D_TLB::findUsableTLBIndex(){
	unsigned int minCycle = 10000000;
	unsigned int indexWait = 0;
	for(int i = 0 ; i < num_of_entries ; i++){
		if(D_TLB_table[i].valid == false){
			return (unsigned int)i;
		}
		else{
			if(D_TLB_table[i].usedPCCycle < minCycle){
				minCycle = D_TLB_table[i].usedPCCycle;
				indexWait = i;
			}
		}
	}
	return indexWait;
}

void D_TLB::updateUsedPC(int counter, unsigned int virtual_page_number){
	for(int i = 0 ; i < num_of_entries ; i++){
		if(D_TLB_table[i].valid == true && D_TLB_table[i].tag == virtual_page_number){
			D_TLB_table[i].usedPCCycle = counter;
		}
	}
}

void D_TLB::updateTLB(unsigned int index, int counter, unsigned int virtual_page_number, unsigned int ppn){
	D_TLB_table[index].valid = true;
	D_TLB_table[index].tag = virtual_page_number;
	D_TLB_table[index].physical_page_number = ppn;
	D_TLB_table[index].usedPCCycle = counter;
}

void D_TLB::updateTLBWithPageFault(unsigned int index, int counter, unsigned int virtual_page_number, unsigned int ppn){
	for(int i = 0 ; i < num_of_entries ; i++){
		if(D_TLB_table[i].physical_page_number == ppn){
			D_TLB_table[i].valid = false;
		}
	}
	index = findUsableTLBIndex();
	D_TLB_table[index].valid = true;
	D_TLB_table[index].tag = virtual_page_number;
	D_TLB_table[index].physical_page_number = ppn;
	D_TLB_table[index].usedPCCycle = counter;
}
