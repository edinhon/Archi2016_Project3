#ifndef LAB3_D_TLB_H
#define LAB3_D_TLB_H

#include<iostream>
#include<cstdio>
#include<cstdlib>

class D_TLB{

public:

    D_TLB();

	bool checkInTLB(unsigned int virtual_page_number);

	unsigned int readFromTLB(unsigned int virtual_page_number);

	unsigned int findUsableTLBIndex();

	void updateUsedPC(int counter, unsigned int virtual_page_number);

	void updateTLB(unsigned int index, int counter, unsigned int virtual_page_number, unsigned int ppn);

	void updateTLBWithPageFault(unsigned int index, int counter, unsigned int virtual_page_number, unsigned int ppn);

	int num_of_entries;// #page table entry/4

	class D_TLB_entry{
		public:
			bool valid;
			unsigned int tag;//virtual page number
			unsigned int physical_page_number;
			unsigned usedPCCycle;
	};
	D_TLB_entry D_TLB_table[100000];

};

#endif //LAB3_D_TLB_H
