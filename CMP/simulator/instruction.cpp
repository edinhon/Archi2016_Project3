#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "instruction.h"
#include "regfile.h"
#include "I_page_table.h"
#include "I_TLB.h"
#include <math.h>


instruction::instruction(){
    for(int i = 0 ; i < 256 ; i++){
        I_disk[i] = 0;
    }

	I_memory_size = 64;
	I_page_size = 8;
	page_entry_number = I_memory_size / I_page_size;
	for(int i = 0 ; i < page_entry_number ; i++){
		I_memory[i].valid = false;
		I_memory[i].usedPCCycle = 0;
		for(int j = 0 ; j < (I_page_size/4) ; j++){
			I_memory[i].memory[j] = 0;
		}
	}

	I_cache_size = 16;
	I_block_size = 4;
	n_way = 4;
	block_entry_number = I_cache_size / I_block_size;
	index_number = block_entry_number / n_way;
	for(int i = 0 ; i < index_number ; i++){
		for(int j = 0 ; j < n_way ; j++){
			I_cache_set[i].I_cache_block[j].valid = false;
			I_cache_set[i].I_cache_block[j].MRU = 0;
		}
	}

	I_cache_hit = 0;
	I_cache_miss = 0;
	I_TLB_hit = 0;
	I_TLB_miss = 0;
	I_page_table_hit = 0;
	I_page_table_miss = 0;
}

void instruction::readInstructionInput(unsigned int *PC){

	FILE *fPtr;
	unsigned char input[4];

	fPtr = fopen("iimage.bin", "rb");

	if (!fPtr) {
        printf("wrong data\n");
        return ;
    }

    fread(&input[0], sizeof(unsigned char), 1, fPtr);
    fread(&input[1], sizeof(unsigned char), 1, fPtr);
    fread(&input[2], sizeof(unsigned char), 1, fPtr);
    fread(&input[3], sizeof(unsigned char), 1, fPtr);
    *PC = ( input[0] << 24 ) + ( input[1] << 16 ) + ( input[2] <<  8 ) + input[3];
	*PC /= 4;
	//printf("%d\n", *PC);

    fread(&input[0], sizeof(unsigned char), 1, fPtr);
    fread(&input[1], sizeof(unsigned char), 1, fPtr);
    fread(&input[2], sizeof(unsigned char), 1, fPtr);
    fread(&input[3], sizeof(unsigned char), 1, fPtr);
    times = ( input[0] << 24 ) + ( input[1] << 16 ) + ( input[2] <<  8 ) + input[3];

    for(int i = 0 ; i < times ; i++){
        fread(&input[0], sizeof(unsigned char), 1, fPtr);
        fread(&input[1], sizeof(unsigned char), 1, fPtr);
        fread(&input[2], sizeof(unsigned char), 1, fPtr);
        fread(&input[3], sizeof(unsigned char), 1, fPtr);
        I_disk[*PC + i] |= ( input[0] << 24 ) + ( input[1] << 16 ) + ( input[2] <<  8 ) + input[3];
        //printf("%0.8X\n", I_disk[*PC + i]);
    }

    fclose(fPtr);
}

void instruction::decode(unsigned int i, I_page_table *ipt, I_TLB *itlb, int counter){
	unsigned int inst;
	unsigned int page_offset;
	unsigned int physical_page_number;
	unsigned int physical_address;
	unsigned int physical_address_tag;
	unsigned int cache_index;
	unsigned int block_offset;

	unsigned int virtual_page_number = (i / I_page_size);
	//TLB Hit
	if(itlb->checkInTLB(virtual_page_number)){
		physical_page_number = itlb->readFromTLB(virtual_page_number);

		page_offset = (i % I_page_size);
		physical_address = (physical_page_number * I_page_size) + page_offset;
		physical_address_tag = (physical_address / I_block_size) / index_number;
		int tag_bit = (32-(log2(index_number) + log2(I_block_size)));
		cache_index = (physical_address << tag_bit);
		cache_index = cache_index >> (tag_bit + (int)log2(I_block_size));
		block_offset = physical_address << (int)(32-log2(I_block_size));
		block_offset = block_offset >> (int)(32-log2(I_block_size));

		//find in Cache
		if(checkInCache(cache_index, physical_address_tag)){
			inst = readFromCache(cache_index, physical_address_tag, block_offset);
			updateCacheUsedPC(cache_index, physical_address_tag);
			I_cache_hit++;
		}
		//find in Memory
		else{
			moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);
			//updateMemoryUsedPC(counter, physical_page_number);
			inst = readFromCache(cache_index, physical_address_tag, block_offset);
			I_cache_miss++;
		}
		itlb->updateUsedPC(counter, virtual_page_number);
		I_TLB_hit++;
	}
	//TLB Miss
	else{
		//Page Table Hit
		if(ipt->checkValid(virtual_page_number)){
			physical_page_number = ipt->readFromPageTable(virtual_page_number);

			page_offset = (i % I_page_size);
			physical_address = (physical_page_number * I_page_size) + page_offset;
			physical_address_tag = (physical_address / I_block_size) / index_number;
			int tag_bit = (32-(log2(index_number) + log2(I_block_size)));
			cache_index = (physical_address << tag_bit);
			cache_index = cache_index >> (tag_bit + (int)log2(I_block_size));
			block_offset = physical_address << (int)(32-log2(I_block_size));
			block_offset = block_offset >> (int)(32-log2(I_block_size));

			//find in Cache
			if(checkInCache(cache_index, physical_address_tag)){
				inst = readFromCache(cache_index, physical_address_tag, block_offset);
				updateCacheUsedPC(cache_index, physical_address_tag);
				I_cache_hit++;
			}
			//find in Memory
			else{
				moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);
				//updateMemoryUsedPC(counter, physical_page_number);
				inst = readFromCache(cache_index, physical_address_tag, block_offset);
				I_cache_miss++;
			}
			unsigned int TLBindex = itlb->findUsableTLBIndex();
			itlb->updateTLB(TLBindex, counter, virtual_page_number, physical_page_number);
			I_page_table_hit++;
			I_TLB_miss++;
		}
		//Page Table Miss
		else{
			inst = I_disk[i];
			physical_page_number = findUsablePhysicalPageNumber();

			page_offset = (i % I_page_size);
			physical_address = (physical_page_number * I_page_size) + page_offset;
			physical_address_tag = (physical_address / I_block_size) / index_number;
			int tag_bit = (32-(log2(index_number) + log2(I_block_size)));
			cache_index = (physical_address << tag_bit);
			cache_index = cache_index >> (tag_bit + (int)log2(I_block_size));
			block_offset = physical_address << (int)(32-log2(I_block_size));
			block_offset = block_offset >> (int)(32-log2(I_block_size));

			if(I_memory[physical_page_number].valid) deleteCacheInOriginMemory(physical_page_number);

			moveFromDiskToMemory(counter, physical_page_number, i);

			ipt->updatePageTable(virtual_page_number, physical_page_number);

			unsigned int TLBindex = itlb->findUsableTLBIndex();
			if(I_memory[physical_page_number].valid) itlb->updateTLBWithPageFault(TLBindex, counter, virtual_page_number, physical_page_number);
			else itlb->updateTLB(TLBindex, counter, virtual_page_number, physical_page_number);

			moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);
			updateMemoryUsedPC(counter, physical_page_number);

			I_page_table_miss++;
			I_TLB_miss++;
		}
	}


    op = (inst >> 26);
    //printf("%X\n", op);
    //R-TYPE
    if(op == 0x00){
        rs = (inst << 6);
        rs = (rs >> 27);
        rt = (inst << 11);
        rt = (rt >> 27);
        rd = (inst << 16);
        rd = (rd >> 27);
        shamt = (inst << 21);
        shamt = (shamt >> 27);
        funct = (inst << 26);
        funct = (funct >> 26);
        //printf("%X %X %X %X %X\n", rs, rt, rd, shamt, funct);
    }
    //J-TYPE
    else if(op == 0x02){
        address = (inst << 6);
        address = (address >> 6);
    }
    else if(op == 0x03){
        address = (inst << 6);
        address = (address >> 6);
    }
    //halt
    else if(op == 0x3F){
        address = (inst << 6);
        address = (address >> 6);
    }
    //I-TYPE
    else{
        rs = (inst << 6);
        rs = (rs >> 27);
        rt = (inst << 11);
        rt = (rt >> 27);
        immediate = (inst << 16);
        immediate = (immediate >> 16);
    }

}

void instruction::implement(unsigned int *PC, regfile *reg, char Memory[]){
	//printf("%X\n", op);
	//R-TYPE
    if(op == 0x00){
		//add
        if(funct == 0x20){
			reg->add(rs, rt, rd, PC);
		}
		//addu
		else if(funct == 0x21){
            reg->addu(rs, rt, rd, PC);
		}
		//sub
		else if(funct == 0x22){
            reg->sub(rs, rt, rd, PC);
		}
		//and
		else if(funct == 0x24){
            reg->andf(rs, rt, rd, PC);
		}
		//or
		else if(funct == 0x25){
            reg->orf(rs, rt, rd, PC);
		}
		//xor
		else if(funct == 0x26){
            reg->xorf(rs, rt, rd, PC);
		}
		//nor
		else if(funct == 0x27){
            reg->nor(rs, rt, rd, PC);
		}
		//nand
		else if(funct == 0x28){
            reg->nand(rs, rt, rd, PC);
		}
		//slt
		else if(funct == 0x2A){
            reg->slt(rs, rt, rd, PC);
		}
		//sll
		else if(funct == 0x00){
            reg->sll(rt, rd, shamt, PC);
		}
		//srl
		else if(funct == 0x02){
            reg->srl(rt, rd, shamt, PC);
		}
		//sra
		else if(funct == 0x03){
            reg->sra(rt, rd, shamt, PC);
		}
		//jr
		else if(funct == 0x08){
            reg->jr(rs, PC);
		}
    }
    //J-TYPE
    else if(op == 0x02){
        *PC = address;
    }
    else if(op == 0x03){
        reg->jal(address, PC);
    }
    //halt
    else if(op == 0x3F){
        return ;
    }
    //I-TYPE
    else{
		//addi
        if(op == 0x08){
			reg->addi(rs, rt, immediate, PC);
		}
		//addiu
		else if(op == 0x09){
			reg->addiu(rs, rt, immediate, PC);
		}
		//lw
		else if(op == 0x23){
            reg->lw(rs, rt, immediate, PC, Memory);
		}
		//lh
		else if(op == 0x21){
            reg->lh(rs, rt, immediate, PC, Memory);
		}
		//lhu
		else if(op == 0x25){
            reg->lhu(rs, rt, immediate, PC, Memory);
		}
		//lb
		else if(op == 0x20){
            reg->lb(rs, rt, immediate, PC, Memory);
		}
		//lbu
		else if(op == 0x24){
            reg->lbu(rs, rt, immediate, PC, Memory);
		}
		//sw
		else if(op == 0x2B){
            reg->sw(rs, rt, immediate, PC, Memory);
		}
		//sh
		else if(op == 0x29){
            reg->sh(rs, rt, immediate, PC, Memory);
		}
		//sb
		else if(op == 0x28){
            reg->sb(rs, rt, immediate, PC, Memory);
		}
		//lui
		else if(op == 0x0F){
            reg->lui(rt, immediate, PC);
		}
		//andi
		else if(op == 0x0C){
            reg->andi(rs, rt, immediate, PC);
		}
		//ori
		else if(op == 0x0D){
            reg->ori(rs, rt, immediate, PC);
		}
		//nori
		else if(op == 0x0E){
            reg->nori(rs, rt, immediate, PC);
		}
		//slti
		else if(op == 0x0A){
            reg->slti(rs, rt, immediate, PC);
		}
		//beq
		else if(op == 0x04){
            reg->beq(rs, rt, immediate, PC);
		}
		//bne
		else if(op == 0x05){
            reg->bne(rs, rt, immediate, PC);
		}
		//bgtz
		else if(op == 0x07){
            reg->bgtz(rs, immediate, PC);
		}
    }
}

unsigned int instruction::findUsablePhysicalPageNumber(){
	unsigned int minCycle = 10000000;
	unsigned int PPNWait = 0;
	for(int i = 0 ; i < page_entry_number ; i++){
		if(I_memory[i].valid == false){
			return (unsigned int)i;
		}
		else{
			if(I_memory[i].usedPCCycle < minCycle){
				minCycle = I_memory[i].usedPCCycle;
				PPNWait = i;
			}
		}
	}
	return PPNWait;
}

void instruction::moveFromDiskToMemory(int counter, unsigned int physical_page_number, unsigned int PC){
	int len = I_page_size/4;
	I_memory[physical_page_number].valid = true;
	I_memory[physical_page_number].usedPCCycle = counter;
	for(int i = 0 ; i < len ; i++){
		I_memory[physical_page_number].memory[i] = I_disk[PC + i];
	}
}

void instruction::updateMemoryUsedPC(int counter, unsigned int physical_page_number){
	I_memory[physical_page_number].usedPCCycle = counter;
}

bool instruction::checkInCache(unsigned int cache_index, unsigned int physical_address_tag){
	for(int i = 0 ; i < n_way ; i++){
		if(I_cache_set[cache_index].I_cache_block[i].valid && I_cache_set[cache_index].I_cache_block[i].tag == physical_address_tag){
			return true;
		}
	}
	return false;
}

unsigned int instruction::readFromCache(unsigned int cache_index, unsigned int physical_address_tag, unsigned int block_offset){
	for(int i = 0 ; i < n_way ; i++){
		if(I_cache_set[cache_index].I_cache_block[i].tag == physical_address_tag){
			return I_cache_set[cache_index].I_cache_block[i].content[block_offset/4];
		}
	}
}

void instruction::updateCacheUsedPC(unsigned int cache_index, unsigned int physical_address_tag){
	for(int i = 0 ; i < n_way ; i++){
		if(I_cache_set[cache_index].I_cache_block[i].valid && I_cache_set[cache_index].I_cache_block[i].tag == physical_address_tag){
			if(I_cache_set[cache_index].I_cache_block[i].MRU == 0) {
				I_cache_set[cache_index].I_cache_block[i].MRU = 1;
			}
		}
	}
	//check is all one&valid or not
	bool allOne = true;
	for(int i = 0 ; i < n_way ; i++){
		if(!I_cache_set[cache_index].I_cache_block[i].valid || I_cache_set[cache_index].I_cache_block[i].MRU == 0){
			allOne = false;
			break;
		}
	}
	//change all block MRU to 0 except this used
	if(allOne){
		for(int i = 0 ; i < n_way ; i++){
			I_cache_set[cache_index].I_cache_block[i].MRU = 0;
			if(I_cache_set[cache_index].I_cache_block[i].tag == physical_address_tag){
				I_cache_set[cache_index].I_cache_block[i].MRU = 1;
			}
		}
	}
}

int instruction::findUsableCacheBlockIndex(unsigned int cache_index){
	int thisIsZero = 0;
	bool unFindZero = true;
	for(int i = 0 ; i < n_way ; i++){
		if(I_cache_set[cache_index].I_cache_block[i].valid == false){
			return i;
		}
		else if(I_cache_set[cache_index].I_cache_block[i].MRU == 0 && unFindZero){
			thisIsZero = i;
			unFindZero = false;
		}
	}
	return thisIsZero;
}

void instruction::moveFromMemoryToCache(int counter, unsigned int cache_index, unsigned int physical_address_tag, unsigned int physical_page_number,
	unsigned int page_offset){//include the function of updateCacheUsedPC()

		int usableIndex = findUsableCacheBlockIndex(cache_index);
		int len = I_block_size/4;

		I_cache_set[cache_index].I_cache_block[usableIndex].valid = true;
		I_cache_set[cache_index].I_cache_block[usableIndex].tag = physical_address_tag;
		for(int i = 0 ; i < len ; i++){
			I_cache_set[cache_index].I_cache_block[usableIndex].content[i] = I_memory[physical_page_number].memory[page_offset/4 + i];
		}
		I_cache_set[cache_index].I_cache_block[usableIndex].MRU = 1;

		//check is all one&valid or not
		bool allOne = true;
		for(int i = 0 ; i < n_way ; i++){
			if(!I_cache_set[cache_index].I_cache_block[i].valid || I_cache_set[cache_index].I_cache_block[i].MRU == 0){
				allOne = false;
				break;
			}
		}
		//change all block MRU to 0 except this used
		if(allOne){
			for(int i = 0 ; i < n_way ; i++){
				I_cache_set[cache_index].I_cache_block[i].MRU = 0;
				if(i == usableIndex){
					I_cache_set[cache_index].I_cache_block[i].MRU = 1;
				}
			}
		}
	}

void instruction::deleteCacheInOriginMemory(unsigned int physical_page_number){
	unsigned int thisPPN;
	for(int i = 0 ; i < index_number ; i++){
		for(int j = 0 ; j < n_way ; j++){
			thisPPN = (I_cache_set[i].I_cache_block[j].tag * index_number * I_block_size) + (i * I_block_size);
			if(thisPPN == physical_page_number){
				I_cache_set[i].I_cache_block[j].valid = false;
				I_cache_set[i].I_cache_block[j].MRU = 0;
			}
		}
	}
}

