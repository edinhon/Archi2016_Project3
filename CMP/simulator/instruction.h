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
	
	void moveFromDiskToMemory(int counter, unsigned int physical_page_number);
	
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
	unsigned int I_memory_size = 64;
	unsigned int I_page_size = 8;
	unsigned int page_entry_number = I_memory_size / I_page_size;
	class I_memory_entry{
		public:
			unsigned int memory[(I_page_size/4)];
			bool valid;
			unsigned int usedPCCycle;
	};
	I_memory_entry I_memory[page_entry_number];
	
	//I cache
	unsigned int I_cache_size = 16;
	unsigned int I_block_size = 4;
	int n_way = 4;
	unsigned int block_entry_number = I_cache_size / I_block_size;
	int index_number = block_entry_number / n_way;
	class I_cache_block_entry{
		public:
			unsigned int content[I_block_size/4];
			bool valid;
			unsigned int tag;
			int MRU;
	};
	class I_cache_set_entry{
		public:
			I_cache_block_entry I_cache_block[n_way];
	};
	I_cache_set_entry I_cache_set[index_number];
	
	int I_cache_hit = 0;
	int I_cache_miss = 0;
	int I_TLB_hit = 0;
	int I_TLB_miss = 0;
	int I_page_table_hit = 0;
	int I_page_table_miss = 0;
	
	int times = 0;
	unsigned int op, rs, rt, rd, shamt, funct, address;
	int immediate;
};

#endif LAB1_INSTRUCTION_H
