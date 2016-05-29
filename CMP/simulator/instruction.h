#ifndef LAB1_INSTRUCTION_H
#define LAB1_INSTRUCTION_H

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include "regfile.h"
#include "I_page_table.h"
#include "I_TLB.h"

class instruction{

public:

    instruction();

	void readInstructionInput(unsigned int *PC);

	void decode(unsigned int i, I_page_table *ipt, I_TLB *itlb, int counter);

	void implement(unsigned int *PC, regfile *reg, char Memory[]);

	unsigned int findUsablePhysicalPageNumber();

	void moveFromDiskToMemory(int counter, unsigned int physical_page_number, unsigned int PC);

	void updateMemoryUsedPC(int counter, unsigned int physical_page_number);

	bool checkInCache(unsigned int cache_index, unsigned int physical_address_tag);

	unsigned int readFromCache(unsigned int cache_index, unsigned int physical_address_tag, unsigned int block_offset);

	void updateCacheUsedPC(unsigned int cache_index, unsigned int physical_address_tag);

	int findUsableCacheBlockIndex(unsigned int cache_index);

	void moveFromMemoryToCache(int counter, unsigned int cache_index, unsigned int physical_address_tag, unsigned int physical_page_number,
		unsigned int page_offset);

	void deleteCacheInOriginMemory(unsigned int physical_page_number);

	unsigned int I_disk[256] = {0};

	//I memory
	unsigned int I_memory_size;
	unsigned int I_page_size;
	unsigned int page_entry_number;
	class I_memory_entry{
		public:
			unsigned int memory[1000];
			bool valid;
			unsigned int usedPCCycle;
	};
	I_memory_entry I_memory[1000];

	//I cache
	unsigned int I_cache_size;
	unsigned int I_block_size;
	int n_way;
	unsigned int block_entry_number;
	int index_number;
	class I_cache_block_entry{
		public:
			unsigned int content[100];
			bool valid;
			unsigned int tag;
			int MRU;
	};
	class I_cache_set_entry{
		public:
			I_cache_block_entry I_cache_block[100];
	};
	I_cache_set_entry I_cache_set[100];

	int I_cache_hit;
	int I_cache_miss;
	int I_TLB_hit;
	int I_TLB_miss;
	int I_page_table_hit;
	int I_page_table_miss;

	int times = 0;
	unsigned int op, rs, rt, rd, shamt, funct, address;
	int immediate;
};

#endif //LAB1_INSTRUCTION_H
