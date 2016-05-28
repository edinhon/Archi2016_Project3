#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "instruction.h"
#include "regfile.h"
#include <tgmath.h>


instruction::instruction(){
    for(int i = 0 ; i < 256 ; i++){
        I_disk[i] = 0;
    }
	for(int i = 0 ; i < page_entry_number ; i++){
		I_memory[i].valid = false;
		I_memory[i].usedPCCycle = 0;
		for(int j = 0 ; j < (I_page_size/4) ; j++){
			I_memory[i].memory[j] = 0;
		}
	}
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
		page_offset = ((i << (32-log2(I_page_size))) >> (32-log2(I_page_size)));
		physical_address = (physical_page_number * I_page_size) + page_offset;
		physical_address_tag = (physical_address / I_block_size) / index_number;
		int tag_bit = (32-(log2(index_number) + log2(I_block_size)));
		cache_index = (physical_address << tag_bit) >> (tag_bit + log2(I_block_size));
		block_offset = (physical_address << (32-log2(I_block_size))) >> (32-log2(I_block_size));
		
		//find in Cache
		if(checkInCache(cache_index, physical_address_tag)){
			inst = readFromCache(cache_index, physical_address_tag, block_offset);
			updateCacheUsedPC(counter);//MRU wait to do
		}
		//find in Memory
		else{
			moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);//MRU wait to do
			updateMemoryUsedPC(counter, physical_page_number);
			inst = readFromCache(cache_index, physical_address_tag, block_offset);
		}
		itlb->updateUsedPC(counter, virtual_page_number);
	}
	//TLB Miss
	else{
		//Page Table Hit
		if(ipt->checkValid(virtual_page_number)){
			physical_page_number = ipt->readFromPageTable(virtual_page_number);
			page_offset = ((i << (32-log2(I_page_size))) >> (32-log2(I_page_size)));
			physical_address = (physical_page_number * I_page_size) + page_offset;
			physical_address_tag = (physical_address / I_block_size) / index_number;
			int tag_bit = (32-(log2(index_number) + log2(I_block_size)));
			cache_index = (physical_address << tag_bit) >> (tag_bit + log2(I_block_size));
			block_offset = (physical_address << (32-log2(I_block_size))) >> (32-log2(I_block_size));
			
			//find in Cache
			if(checkInCache(cache_index, physical_address_tag)){
				inst = readFromCache(cache_index, physical_address_tag, block_offset);
				updateCacheUsedPC(counter);//MRU wait to do
			}
			//find in Memory
			else{
				moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);//MRU wait to do
				updateMemoryUsedPC(counter, physical_page_number);
				inst = readFromCache(cache_index, physical_address_tag, block_offset);
			}
			unsigned int TLBindex = itlb->findUsableTLBIndex;
			itlb->updateTLB(TLBindex, counter, virtual_page_number, physical_page_number);
		}
		//Page Table Miss
		else{
			inst = I_disk[i];
			physical_page_number = findUsablePhysicalPageNumber();
			page_offset = ((i << (32-log2(I_page_size))) >> (32-log2(I_page_size)));
			physical_address = (physical_page_number * I_page_size) + page_offset;
			physical_address_tag = (physical_address / I_block_size) / index_number;
			int tag_bit = (32-(log2(index_number) + log2(I_block_size)));
			cache_index = (physical_address << tag_bit) >> (tag_bit + log2(I_block_size));
			block_offset = (physical_address << (32-log2(I_block_size))) >> (32-log2(I_block_size));
			
			moveFromDiskToMemory(counter, physical_page_number);
			
			ipt->updatePageTable(virtual_page_number, physical_page_number);
			
			unsigned int TLBindex = itlb->findUsableTLBIndex;
			itlb->updateTLB(TLBindex, counter, virtual_page_number, physical_page_number);
			
			moveFromMemoryToCache(counter, cache_index, physical_address_tag, physical_page_number, page_offset);//MRU wait to do
			updateMemoryUsedPC(counter, physical_page_number);
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

void instruction::moveFromDiskToMemory(int counter, unsigned int physical_page_number){
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

void instruction::updateCacheUsedPC(int counter){//MRU wait to do

}

void instruction::moveFromMemoryToCache(int counter, unsigned int cache_index, unsigned int physical_address_tag, unsigned int physical_page_number, 
	unsigned int page_offset){//MRU wait to do
		int usableIndex;//find usableIndex by MRU
		int len = I_block_size/4;
		
		I_cache_set[cache_index].I_cache_block[usableIndex].valid = true;
		I_cache_set[cache_index].I_cache_block[usableIndex].tag = physical_address_tag;
		for(int i = 0 ; i < len ; i++){
			I_cache_set[cache_index].I_cache_block[usableIndex].content[i] = I_memory[physical_page_number].memory[page_offset + i];
		}
		//update MRU
	}
