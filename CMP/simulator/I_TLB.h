#ifndef LAB3_I_TLB_H
#define LAB3_I_TLB_H

#include<iostream>
#include<cstdio>
#include<cstdlib>

class I_TLB{

public:

    I_TLB();

	bool checkInTLB(unsigned int virtual_page_number);

	unsigned int readFromTLB(unsigned int virtual_page_number);

	unsigned int findUsableTLBIndex();

	void updateUsedPC(int counter, unsigned int virtual_page_number);

	void updateTLB(unsigned int index, int counter, unsigned int virtual_page_number, unsigned int ppn);

	void updateTLBWithPageFault(unsigned int index, int counter, unsigned int virtual_page_number, unsigned int ppn);

	int num_of_entries;// #page table entry/4

	class I_TLB_entry{
		public:
			bool valid;
			unsigned int tag;//virtual page number
			unsigned int physical_page_number;
			unsigned usedPCCycle;
	};
	I_TLB_entry I_TLB_table[100000];

};

#endif LAB3_I_TLB_H
