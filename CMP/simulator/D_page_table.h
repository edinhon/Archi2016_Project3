#ifndef LAB3_D_PAGE_TABLE_H
#define LAB3_D_PAGE_TABLE_H

#include<iostream>
#include<cstdio>
#include<cstdlib>

class D_page_table{

public:

    D_page_table();

	bool checkValid(unsigned int virtual_page_number);

	unsigned int readFromPageTable(unsigned int virtual_page_number);

	void updatePageTable(unsigned int virtual_page_number, unsigned int ppn);

	int num_of_entries;//disk size / page size
	int page_size;

	bool valid[100000];
	unsigned int physical_page_number[100000];
};

#endif //LAB3_D_PAGE_TABLE_H
