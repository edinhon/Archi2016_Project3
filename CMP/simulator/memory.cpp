#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "memory.h"

memory::memory(){
    for(int i = 0 ; i < 1024 ; i++){
        D_memory[i] = 0x00;
    }
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
        D_memory[i] = ( input[0]);
        //printf("%X\n", D_memory[i]);
    }

    fclose(fPtr);
}
