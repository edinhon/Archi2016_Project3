#ifndef LAB1_REGFILE_H
#define LAB1_REGFILE_H

#include<iostream>
#include<cstdio>
#include<cstdlib>

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
    void lw   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, char Memory[]);
    void lh   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, char Memory[]);
    void lhu  (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, char Memory[]);
    void lb   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, char Memory[]);
    void lbu  (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, char Memory[]);
    void sw   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, char Memory[]);
    void sh   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, char Memory[]);
    void sb   (unsigned int rs, unsigned int rt,  int immediate, unsigned int *PC, char Memory[]);
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
