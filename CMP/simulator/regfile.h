#ifndef LAB1_REGFILE_H
#define LAB1_REGFILE_H

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include "memory.h"
#include "D_page_table.h"
#include "D_TLB.h"

class regfile{

public:

    regfile();
	void printRegister(FILE *snap);

	//R-TYPE
    void add (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void addu(unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void sub (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void andf(unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void orf (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void xorf(unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void nor (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void nand(unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void slt (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void sll (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void srl (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void sra (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC);
    void jr  (unsigned int rs, unsigned int *PC);
	//I-TYPE
    void addi (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC);
    void addiu(unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC);
    void lw   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, memory *memo, D_page_table *dpt, D_TLB *dtlb, int counter);
    void lh   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, memory *memo, D_page_table *dpt, D_TLB *dtlb, int counter);
    void lhu  (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, memory *memo, D_page_table *dpt, D_TLB *dtlb, int counter);
    void lb   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, memory *memo, D_page_table *dpt, D_TLB *dtlb, int counter);
    void lbu  (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, memory *memo, D_page_table *dpt, D_TLB *dtlb, int counter);
    void sw   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, memory *memo, D_page_table *dpt, D_TLB *dtlb, int counter);
    void sh   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, memory *memo, D_page_table *dpt, D_TLB *dtlb, int counter);
    void sb   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, memory *memo, D_page_table *dpt, D_TLB *dtlb, int counter);
    void lui  (unsigned int rt,  int immediate, unsigned int *PC);
    void andi (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC);
    void ori  (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC);
    void nori (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC);
    void slti (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC);
    void beq  (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC);
    void bne  (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC);
    void bgtz (unsigned int rs,  int immediate, unsigned int *PC);
	//J-TYPE
	void jal  (unsigned int address, unsigned int *PC);

    int Register[32] = {0};
	bool error[4];

};

#endif // LAB1_REGFILE_H
