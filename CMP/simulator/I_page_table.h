#ifndef LAB3_I_PAGE_TABLE_H
#define LAB3_I_PAGE_TABLE_H

#include<iostream>
#include<cstdio>
#include<cstdlib>

class I_page_table{

public:

    I_page_table();
	
	bool checkValid(unsigned int virtual_page_number);
	
	unsigned int readFromPageTable(unsigned int virtual_page_number);
	
	void updatePageTable(unsigned int virtual_page_number, unsigned int ppn);

	int num_of_entries = 128;//disk size / page size
	int page_size = 8;
	
	bool valid[num_of_entries];
	unsigned int physical_page_number[num_of_entries];
};

#endif LAB3_I_PAGE_TABLE_H