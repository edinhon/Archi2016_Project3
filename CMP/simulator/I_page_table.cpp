#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "I_page_table.h"

I_page_table::I_page_table(){
	for(int i = 0 ; i < num_of_entries ; i++){
		valid[i] = false;
	}
}

bool I_page_table::checkValid(unsigned int virtual_page_number){
	if(valid[virtual_page_number]) return true;
	else false;
}

unsigned int I_page_table::readFromPageTable(unsigned int virtual_page_number){
	return physical_page_number[virtual_page_number];
}

void I_page_table::updatePageTable(unsigned int virtual_page_number, unsigned int ppn){
	valid[virtual_page_number] = true;
	physical_page_number[virtual_page_number] = ppn;
}