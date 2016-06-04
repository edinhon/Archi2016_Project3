#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "D_page_table.h"

D_page_table::D_page_table(){

	num_of_entries = 64;//disk size / page size
	page_size = 16;
	for(int i = 0 ; i < num_of_entries ; i++){
		valid[i] = false;
	}
}

bool D_page_table::checkValid(unsigned int virtual_page_number){
	if(valid[virtual_page_number]) return true;
	else return false;
}

unsigned int D_page_table::readFromPageTable(unsigned int virtual_page_number){
	return physical_page_number[virtual_page_number];
}

void D_page_table::updatePageTable(unsigned int virtual_page_number, unsigned int ppn){
	for(int i = 0 ; i < num_of_entries ; i++){
		if(physical_page_number[i] == ppn){
			valid[i] = false;
		}
	}
	valid[virtual_page_number] = true;
	physical_page_number[virtual_page_number] = ppn;
}
