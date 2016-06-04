#ifndef LAB1_MEMORY_H
#define LAB1_MEMORY_H

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include "D_page_table.h"
#include "D_TLB.h"

class memory{

public:

    memory();
	
	void D_setArgu(char* memory_size, char* page_size, char* cache_size, char* block_size, char* n);

    void readMemory(int *$sp);

	unsigned int findUsablePhysicalPageNumber();

	void moveFromDiskToMemory(int counter, unsigned int physical_page_number, unsigned int PC, unsigned int page_offset);

	void updateMemoryUsedPC(int counter, unsigned int physical_page_number);

	bool checkInCache(unsigned int cache_index, unsigned int physical_address_tag);

	char readFromCache(unsigned int cache_index, unsigned int physical_address_tag, unsigned int block_offset);

	void updateCacheUsedPC(unsigned int cache_index, unsigned int physical_address_tag);

	int findUsableCacheBlockIndex(unsigned int cache_index);

	void moveFromMemoryToCache(int counter, unsigned int cache_index, unsigned int physical_address_tag, unsigned int physical_page_number,
		unsigned int page_offset);

	void deleteCacheInOriginMemory(unsigned int physical_page_number);
	
	char getData(unsigned int PC, D_page_table *dpt, D_TLB *dtlb, int counter);
	
	void writeBack(unsigned int dataAddress, char value, D_page_table *dpt, D_TLB *dtlb, int counter);
	
	void writeIntoCache(unsigned int cache_index, unsigned int physical_address_tag, unsigned int block_offset, char value);
	
	void writeIntoMemory(unsigned int physical_page_number, unsigned int page_offset, int counter, char value);

	char D_disk[1024];

	//D memory
	unsigned int D_memory_size;
	unsigned int D_page_size;
	unsigned int page_entry_number;
	class D_memory_entry{
		public:
			char memory[1000];
			bool valid;
			unsigned int usedPCCycle;
	};
	D_memory_entry D_memory[1000];

	//D cache
	unsigned int D_cache_size;
	unsigned int D_block_size;
	int n_way;
	unsigned int block_entry_number;
	int index_number;
	class D_cache_block_entry{
		public:
			char content[100];
			bool valid;
			unsigned int tag;
			int MRU;
	};
	class D_cache_set_entry{
		public:
			D_cache_block_entry D_cache_block[100];
	};
	D_cache_set_entry D_cache_set[100];

	int D_cache_hit;
	int D_cache_miss;
	int D_TLB_hit;
	int D_TLB_miss;
	int D_page_table_hit;
	int D_page_table_miss;
	
	bool ch, cm, th, tm, ph, pm;
    
    int times = 0;
};


#endif // LAB1_MEMORY_H
