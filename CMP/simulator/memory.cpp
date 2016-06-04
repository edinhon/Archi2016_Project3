#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "memory.h"
#include "D_page_table.h"
#include "D_TLB.h"
#include <math.h>

memory::memory(){
	for(int i = 0 ; i < 256 ; i++){
        D_disk[i] = 0;
    }

	D_memory_size = 32;
	D_page_size = 16;
	page_entry_number = D_memory_size / D_page_size;
	for(int i = 0 ; i < page_entry_number ; i++){
		D_memory[i].valid = false;
		D_memory[i].usedPCCycle = 0;
		for(int j = 0 ; j < (D_page_size) ; j++){
			D_memory[i].memory[j] = 0;
		}
	}

	D_cache_size = 16;
	D_block_size = 4;
	n_way = 1;
	block_entry_number = D_cache_size / D_block_size;
	index_number = block_entry_number / n_way;
	for(int i = 0 ; i < index_number ; i++){
		for(int j = 0 ; j < n_way ; j++){
			D_cache_set[i].D_cache_block[j].valid = false;
			D_cache_set[i].D_cache_block[j].MRU = 0;
		}
	}

	D_cache_hit = 0;
	D_cache_miss = 0;
	D_TLB_hit = 0;
	D_TLB_miss = 0;
	D_page_table_hit = 0;
	D_page_table_miss = 0;
}

void memory::D_setArgu(char* memory_size, char* page_size, char* cache_size, char* block_size, char* n){
	D_memory_size = atoi(memory_size);
	D_page_size = atoi(page_size);
	D_cache_size = atoi(cache_size);
	D_block_size = atoi(block_size);
	n_way = atoi(n);
}

void memory::readMemory(int *$sp){
    FILE *fPtr;
	unsigned char input[4];

	fPtr = fopen("dimage.bin", "rb");

	if (!fPtr) {
        printf("wrong data\n");
        return ;
    }

    fread(&input[0], sizeof(unsigned char), 1, fPtr);
    fread(&input[1], sizeof(unsigned char), 1, fPtr);
    fread(&input[2], sizeof(unsigned char), 1, fPtr);
    fread(&input[3], sizeof(unsigned char), 1, fPtr);
    *$sp = ( input[0] << 24 ) + ( input[1] << 16 ) + ( input[2] <<  8 ) + input[3];
	//*$sp /= 4;

    fread(&input[0], sizeof(unsigned char), 1, fPtr);
    fread(&input[1], sizeof(unsigned char), 1, fPtr);
    fread(&input[2], sizeof(unsigned char), 1, fPtr);
    fread(&input[3], sizeof(unsigned char), 1, fPtr);
    times = ( input[0] << 24 ) + ( input[1] << 16 ) + ( input[2] <<  8 ) + input[3];
	times *= 4;

    for(int i = 0 ; i < times ; i++){
        fread(&input[0], sizeof(char), 1, fPtr);
        /*fread(&input[1], sizeof(unsigned char), 1, fPtr);
        fread(&input[2], sizeof(unsigned char), 1, fPtr);
        fread(&input[3], sizeof(unsigned char), 1, fPtr);*/
        D_disk[i] = ( input[0]);
        //printf("%X\n", D_memory[i]);
    }

    fclose(fPtr);
}

char memory::getData(unsigned int dataAddress, D_page_table *dpt, D_TLB *dtlb, int counter){
	char data;
	unsigned int page_offset;
	unsigned int physical_page_number;
	unsigned int physical_address;
	unsigned int physical_address_tag;
	unsigned int cache_index;
	unsigned int block_offset;

	unsigned int virtual_page_number = (dataAddress / D_page_size);

	//TLB Hit
	if(dtlb->checkInTLB(virtual_page_number)){
		physical_page_number = dtlb->readFromTLB(virtual_page_number);
		
		page_offset = (dataAddress % D_page_size);
		physical_address = (physical_page_number * D_page_size) + page_offset;
		//printf("PA = %d\n", physical_address);
		physical_address_tag = (physical_address / D_block_size) / index_number;
		int tag_bit = (32-(log2(index_number) + log2(D_block_size)));
		cache_index = (physical_address << tag_bit);
		cache_index = cache_index >> (tag_bit + (int)log2(D_block_size));
		block_offset = physical_address << (int)(32-log2(D_block_size));
		block_offset = block_offset >> (int)(32-log2(D_block_size));

		//find in Cache
		if(checkInCache(cache_index, physical_address_tag)){
			data = readFromCache(cache_index, physical_address_tag, block_offset);
			//data = D_disk[dataAddress];
			updateCacheUsedPC(cache_index, physical_address_tag);
			//D_cache_hit++;
			ch = true;
		}
		//find in Memory
		else{
			moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);
			//updateMemoryUsedPC(counter, physical_page_number);
			data = readFromCache(cache_index, physical_address_tag, block_offset);
			//data = D_disk[dataAddress];
			//D_cache_miss++;
			ch = false;
		}
		dtlb->updateUsedPC(counter, virtual_page_number);
		//D_TLB_hit++;
		th = true;
		return data;
	}
	//TLB Miss
	else{
		//Page Table Hit
		if(dpt->checkValid(virtual_page_number)){
			physical_page_number = dpt->readFromPageTable(virtual_page_number);

			page_offset = (dataAddress % D_page_size);
			physical_address = (physical_page_number * D_page_size) + page_offset;
			physical_address_tag = (physical_address / D_block_size) / index_number;
			int tag_bit = (32-(log2(index_number) + log2(D_block_size)));
			cache_index = (physical_address << tag_bit);
			cache_index = cache_index >> (tag_bit + (int)log2(D_block_size));
			block_offset = physical_address << (int)(32-log2(D_block_size));
			block_offset = block_offset >> (int)(32-log2(D_block_size));

			//find in Cache
			if(checkInCache(cache_index, physical_address_tag)){
				data = readFromCache(cache_index, physical_address_tag, block_offset);
				//data = D_disk[dataAddress];
				updateCacheUsedPC(cache_index, physical_address_tag);
				//D_cache_hit++;
				ch = true;
			}
			//find in Memory
			else{
				moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);
				//updateMemoryUsedPC(counter, physical_page_number);
				data = readFromCache(cache_index, physical_address_tag, block_offset);
				//data = D_disk[dataAddress];
				//D_cache_miss++;
				ch = false;
			}
			unsigned int TLBindex = dtlb->findUsableTLBIndex();
			dtlb->updateTLB(TLBindex, counter, virtual_page_number, physical_page_number);
			//D_page_table_hit++;
			ph = true;
			//D_TLB_miss++;
			th = false;
			return data;
		}
		//Page Table Miss
		else{
			data = D_disk[dataAddress];
			physical_page_number = findUsablePhysicalPageNumber();

			page_offset = (dataAddress % D_page_size);
			physical_address = (physical_page_number * D_page_size) + page_offset;
			physical_address_tag = (physical_address / D_block_size) / index_number;
			int tag_bit = (32-(log2(index_number) + log2(D_block_size)));
			cache_index = (physical_address << tag_bit);
			cache_index = cache_index >> (tag_bit + (int)log2(D_block_size));
			block_offset = physical_address << (int)(32-log2(D_block_size));
			block_offset = block_offset >> (int)(32-log2(D_block_size));
			
			if(D_memory[physical_page_number].valid) deleteCacheInOriginMemory(physical_page_number);

			moveFromDiskToMemory(counter, physical_page_number, dataAddress, page_offset);

			dpt->updatePageTable(virtual_page_number, physical_page_number);

			unsigned int TLBindex = dtlb->findUsableTLBIndex();
			if(D_memory[physical_page_number].valid) dtlb->updateTLBWithPageFault(TLBindex, counter, virtual_page_number, physical_page_number);
			else dtlb->updateTLB(TLBindex, counter, virtual_page_number, physical_page_number);

			moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);
			updateMemoryUsedPC(counter, physical_page_number);

			//D_cache_miss++;
			ch = false;
			//D_page_table_miss++;
			ph = false;
			//D_TLB_miss++;
			th = false;
			return data;
		}
	}
}

unsigned int memory::findUsablePhysicalPageNumber(){
	unsigned int minCycle = 10000000;
	unsigned int PPNWait = 0;
	for(int i = 0 ; i < page_entry_number ; i++){
		if(D_memory[i].valid == false){
			return (unsigned int)i;
		}
		else{
			if(D_memory[i].usedPCCycle < minCycle){
				minCycle = D_memory[i].usedPCCycle;
				PPNWait = i;
			}
		}
	}
	return PPNWait;
}

void memory::moveFromDiskToMemory(int counter, unsigned int physical_page_number, unsigned int PC, unsigned int page_offset){
	int len = D_page_size;
	D_memory[physical_page_number].valid = true;
	D_memory[physical_page_number].usedPCCycle = counter;
	for(int i = 0 ; i < len ; i++){
		D_memory[physical_page_number].memory[i] = D_disk[PC + i - page_offset];
	}
}

void memory::updateMemoryUsedPC(int counter, unsigned int physical_page_number){
	D_memory[physical_page_number].usedPCCycle = counter;
}

bool memory::checkInCache(unsigned int cache_index, unsigned int physical_address_tag){
	for(int i = 0 ; i < n_way ; i++){
		if(D_cache_set[cache_index].D_cache_block[i].valid && D_cache_set[cache_index].D_cache_block[i].tag == physical_address_tag){
			return true;
		}
	}
	return false;
}

char memory::readFromCache(unsigned int cache_index, unsigned int physical_address_tag, unsigned int block_offset){
	for(int i = 0 ; i < n_way ; i++){
		if(D_cache_set[cache_index].D_cache_block[i].tag == physical_address_tag){
			return D_cache_set[cache_index].D_cache_block[i].content[block_offset];
		}
	}
}

void memory::updateCacheUsedPC(unsigned int cache_index, unsigned int physical_address_tag){
	for(int i = 0 ; i < n_way ; i++){
		if(D_cache_set[cache_index].D_cache_block[i].valid && D_cache_set[cache_index].D_cache_block[i].tag == physical_address_tag){
			if(D_cache_set[cache_index].D_cache_block[i].MRU == 0) {
				D_cache_set[cache_index].D_cache_block[i].MRU = 1;
			}
		}
	}
	//check is all one&valid or not
	bool allOne = true;
	for(int i = 0 ; i < n_way ; i++){
		if(!D_cache_set[cache_index].D_cache_block[i].valid || D_cache_set[cache_index].D_cache_block[i].MRU == 0){
			allOne = false;
			break;
		}
	}
	//change all block MRU to 0 except this used
	if(allOne){
		for(int i = 0 ; i < n_way ; i++){
			D_cache_set[cache_index].D_cache_block[i].MRU = 0;
			if(D_cache_set[cache_index].D_cache_block[i].tag == physical_address_tag){
				D_cache_set[cache_index].D_cache_block[i].MRU = 1;
			}
		}
	}
}

int memory::findUsableCacheBlockIndex(unsigned int cache_index){
	int thisIsZero = 0;
	bool unFindZero = true;
	for(int i = 0 ; i < n_way ; i++){
		if(D_cache_set[cache_index].D_cache_block[i].valid == false){
			return i;
		}
		else if(D_cache_set[cache_index].D_cache_block[i].MRU == 0 && unFindZero){
			thisIsZero = i;
			unFindZero = false;
		}
	}
	return thisIsZero;
}

void memory::moveFromMemoryToCache(int counter, unsigned int cache_index, unsigned int physical_address_tag, unsigned int physical_page_number,
	unsigned int page_offset){//include the function of updateCacheUsedPC()

		int usableIndex = findUsableCacheBlockIndex(cache_index);
		int len = D_block_size;

		D_cache_set[cache_index].D_cache_block[usableIndex].valid = true;
		D_cache_set[cache_index].D_cache_block[usableIndex].tag = physical_address_tag;
		for(int i = 0 ; i < len ; i++){
			D_cache_set[cache_index].D_cache_block[usableIndex].content[i] = D_memory[physical_page_number].memory[page_offset + i];
		}
		D_cache_set[cache_index].D_cache_block[usableIndex].MRU = 1;

		//check is all one&valid or not
		bool allOne = true;
		for(int i = 0 ; i < n_way ; i++){
			if(!D_cache_set[cache_index].D_cache_block[i].valid || D_cache_set[cache_index].D_cache_block[i].MRU == 0){
				allOne = false;
				break;
			}
		}
		//change all block MRU to 0 except this used
		if(allOne){
			for(int i = 0 ; i < n_way ; i++){
				D_cache_set[cache_index].D_cache_block[i].MRU = 0;
				if(i == usableIndex){
					D_cache_set[cache_index].D_cache_block[i].MRU = 1;
				}
			}
		}
	}

void memory::deleteCacheInOriginMemory(unsigned int physical_page_number){
	unsigned int thisPPN;
	for(int i = 0 ; i < index_number ; i++){
		for(int j = 0 ; j < n_way ; j++){
			int ppnNeed = i * D_block_size;
			ppnNeed = ppnNeed >> (int)(log2(D_page_size));
			ppnNeed = ppnNeed << (int)(log2(D_page_size));
			thisPPN = ((D_cache_set[i].D_cache_block[j].tag * index_number * D_block_size) + ppnNeed) >> (int)(log2(D_page_size));
			if(thisPPN == physical_page_number){
				D_cache_set[i].D_cache_block[j].valid = false;
				D_cache_set[i].D_cache_block[j].MRU = 0;
			}
		}
	}
}

void memory::writeBack(unsigned int dataAddress, char value, D_page_table *dpt, D_TLB *dtlb, int counter){
	unsigned int page_offset;
	unsigned int physical_page_number;
	unsigned int physical_address;
	unsigned int physical_address_tag;
	unsigned int cache_index;
	unsigned int block_offset;

	unsigned int virtual_page_number = (dataAddress / D_page_size);

	//TLB Hit
	if(dtlb->checkInTLB(virtual_page_number)){
		physical_page_number = dtlb->readFromTLB(virtual_page_number);

		page_offset = (dataAddress % D_page_size);
		physical_address = (physical_page_number * D_page_size) + page_offset;
		//printf("PA = %d\n", physical_address);
		physical_address_tag = (physical_address / D_block_size) / index_number;
		int tag_bit = (32-(log2(index_number) + log2(D_block_size)));
		cache_index = (physical_address << tag_bit);
		cache_index = cache_index >> (tag_bit + (int)log2(D_block_size));
		block_offset = physical_address << (int)(32-log2(D_block_size));
		block_offset = block_offset >> (int)(32-log2(D_block_size));

		//Write back Cache if in Cache
		if(checkInCache(cache_index, physical_address_tag)){
			writeIntoCache(cache_index, physical_address_tag, block_offset, value);
			updateCacheUsedPC(cache_index, physical_address_tag);
			writeIntoMemory(physical_page_number, page_offset, counter, value);
			//D_cache_hit++;
			ch = true;
		}
		else {
			writeIntoMemory(physical_page_number, page_offset, counter, value);
			//updateMemoryUsedPC(counter, physical_page_number);
			moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);
			//D_cache_miss++;
			ch = false;
		}

		dtlb->updateUsedPC(counter, virtual_page_number);
		D_disk[dataAddress] = value;
		//D_TLB_hit++;
		th = true;
	}
	//TLB Miss
	else{
		//Page Table Hit
		if(dpt->checkValid(virtual_page_number)){
			physical_page_number = dpt->readFromPageTable(virtual_page_number);

			page_offset = (dataAddress % D_page_size);
			physical_address = (physical_page_number * D_page_size) + page_offset;
			physical_address_tag = (physical_address / D_block_size) / index_number;
			int tag_bit = (32-(log2(index_number) + log2(D_block_size)));
			cache_index = (physical_address << tag_bit);
			cache_index = cache_index >> (tag_bit + (int)log2(D_block_size));
			block_offset = physical_address << (int)(32-log2(D_block_size));
			block_offset = block_offset >> (int)(32-log2(D_block_size));

			//Write back Cache if in Cache
			if(checkInCache(cache_index, physical_address_tag)){
				writeIntoCache(cache_index, physical_address_tag, block_offset, value);
				updateCacheUsedPC(cache_index, physical_address_tag);
				writeIntoMemory(physical_page_number, page_offset, counter, value);
				updateMemoryUsedPC(counter, physical_page_number);
				//D_cache_hit++;
				ch = true;
			}
			else {
				writeIntoMemory(physical_page_number, page_offset, counter, value);
				updateMemoryUsedPC(counter, physical_page_number);
				moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);
				//D_cache_miss++;
				ch = false;
			}

			unsigned int TLBindex = dtlb->findUsableTLBIndex();
			dtlb->updateTLB(TLBindex, counter, virtual_page_number, physical_page_number);
			D_disk[dataAddress] = value;
			//D_page_table_hit++;
			ph = true;
			//D_TLB_miss++;
			th = false;
		}
		//Page Table Miss
		else{
			D_disk[dataAddress] = value;
			physical_page_number = findUsablePhysicalPageNumber();

			page_offset = (dataAddress % D_page_size);
			physical_address = (physical_page_number * D_page_size) + page_offset;
			physical_address_tag = (physical_address / D_block_size) / index_number;
			int tag_bit = (32-(log2(index_number) + log2(D_block_size)));
			cache_index = (physical_address << tag_bit);
			cache_index = cache_index >> (tag_bit + (int)log2(D_block_size));
			block_offset = physical_address << (int)(32-log2(D_block_size));
			block_offset = block_offset >> (int)(32-log2(D_block_size));

			if(D_memory[physical_page_number].valid) deleteCacheInOriginMemory(physical_page_number);

			moveFromDiskToMemory(counter, physical_page_number, dataAddress, page_offset);

			dpt->updatePageTable(virtual_page_number, physical_page_number);

			unsigned int TLBindex = dtlb->findUsableTLBIndex();
			if(D_memory[physical_page_number].valid) dtlb->updateTLBWithPageFault(TLBindex, counter, virtual_page_number, physical_page_number);
			else dtlb->updateTLB(TLBindex, counter, virtual_page_number, physical_page_number);

			moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);
			updateMemoryUsedPC(counter, physical_page_number);

			//D_cache_miss++;
			ch = false;
			//D_page_table_miss++;
			ph = false;
			//D_TLB_miss++;
			th = false;
		}
	}
}

void memory::writeIntoCache(unsigned int cache_index, unsigned int physical_address_tag, unsigned int block_offset, char value){
	for(int i = 0 ; i < n_way ; i++){
		if(D_cache_set[cache_index].D_cache_block[i].tag == physical_address_tag){
			D_cache_set[cache_index].D_cache_block[i].content[block_offset] = value;
		}
	}
}

void memory::writeIntoMemory(unsigned int physical_page_number, unsigned int page_offset, int counter, char value){
	D_memory[physical_page_number].memory[page_offset] = value;
}
