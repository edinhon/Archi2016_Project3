#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "regfile.h"

regfile::regfile(){
    for(int i = 0 ; i < 32 ; i++){
        Register[i] = 0;
    }
    Register[29] = 0x400;
	for(int i = 0 ; i < 4 ; i++){
		error[i] = false;
	}
}

void regfile::printRegister(FILE *snap){
	for(int i = 0 ; i < 32 ; i++){
		fprintf(snap, "$%0.2d: 0x%0.8X\n", i, Register[i]);
	}
}

//R-TYPE
void regfile::add (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00){
		error[0] = true;
		*PC += 1;
	}
	//error "Number Overflow"
	if(Register[rs] > 0 && Register[rt] > 0 && (Register[rs] + Register[rt]) <= 0){
		error[1] = true;
		//*PC += 1;
	}
	else if(Register[rs] < 0 && Register[rt] < 0 && (Register[rs] + Register[rt]) >= 0){
		error[1] = true;
		//*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rd] = Register[rs] + Register[rt];
		*PC += 1;
	}
}
void regfile::addu(unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rd] = Register[rs] + (unsigned int)Register[rt];
		*PC += 1;
	}
}
void regfile::sub (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00){
		error[0] = true;
		*PC += 1;
	}
	//error "Number Overflow"
	if(Register[rs] > 0 && (Register[rt]*-1) > 0 && (Register[rs] + (Register[rt]*-1)) <= 0){
		error[1] = true;
		//*PC += 1;
	}
	else if(Register[rs] < 0 && (Register[rt]*-1) < 0 && (Register[rs] + (Register[rt]*-1)) >= 0){
		error[1] = true;
		//*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rd] = Register[rs] - Register[rt];
		*PC += 1;
	}
}
void regfile::andf(unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rd] = Register[rs] & Register[rt];
		*PC += 1;
	}
}
void regfile::orf (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rd] = Register[rs] | Register[rt];
		*PC += 1;
	}
}
void regfile::xorf(unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rd] = Register[rs] ^ Register[rt];
		*PC += 1;
	}
}
void regfile::nor (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rd] = ~(Register[rs] | Register[rt]);
		*PC += 1;
	}
}
void regfile::nand(unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rd] = ~(Register[rs] & Register[rt]);
		*PC += 1;
	}
}
void regfile::slt (unsigned int rs, unsigned int rt, unsigned int rd, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rd] = (Register[rt] > Register[rs]) ? 1 : 0;
		*PC += 1;
	}
}
void regfile::sll (unsigned int rt, unsigned int rd, unsigned int shamt, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00 && (rt != 0x00 || shamt != 0x00)){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rd] = Register[rt] << shamt;
		*PC += 1;
	}
}
void regfile::srl (unsigned int rt, unsigned int rd, unsigned int shamt, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		if((Register[rt] >> 31) == 0xFFFFFFFF) {
			//Register[rd] = ( (((0xFFFFFFFF << (32 - shamt)))*-1)-1) & (Register[rt] >> shamt);
			if(shamt != 0) Register[rd] = ~(0xFFFFFFFF << (32 - shamt)) & (Register[rt] >> shamt);
			else if(shamt == 0) Register[rd] = ~(0xFFFFFFFF << 32) & (Register[rt] >> shamt);
		}
		else{
			Register[rd] = Register[rt] >> shamt;
		}
		*PC += 1;
	}
}
void regfile::sra (unsigned int rt, unsigned int rd, unsigned int shamt, unsigned int *PC){
	//error "Write to register $0"
	if(rd == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rd] = Register[rt] >> shamt;
		*PC += 1;
	}
}
void regfile::jr  (unsigned int rs, unsigned int *PC){
	*PC = Register[rs] >> 2;
}


//I-TYPE
void regfile::addi (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	//error "Number Overflow"
	if(Register[rs] > 0 && immediate > 0 && (Register[rs] + immediate) <= 0){
		error[1] = true;
		//*PC += 1;
	}
	else if(Register[rs] < 0 && immediate < 0 && (Register[rs] + immediate) >= 0){
		error[1] = true;
		//*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = Register[rs] + immediate;
		*PC += 1;
	}
}
void regfile::addiu(unsigned int rs, unsigned int rt, int immediate, unsigned int *PC){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = Register[rs] + ((unsigned int)immediate);
		*PC += 1;
	}
}
void regfile::lw   (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC, char Memory[]){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	//error "Address Overflow"
	if((Register[rs] + immediate) > 1020 || (Register[rs] + immediate) < 0){
		error[2] = true;
	}
	//error "Misalignment Error"
	if((Register[rs] + immediate)%4 != 0){
		error[3] = true;
	}
	//error "Number Overflow"
		if(Register[rs] > 0 && immediate > 0 && (Register[rs] + immediate) <= 0){
			error[1] = true;
			//*PC += 1;
		}
		else if(Register[rs] < 0 && immediate < 0 && (Register[rs] + immediate) >= 0){
			error[1] = true;
			//*PC += 1;
		}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = (( Memory[Register[rs] + immediate] << 24 ) & 0xFF000000) | (( Memory[Register[rs] + immediate + 1] << 16 ) & 0x00FF0000) |
						(( Memory[Register[rs] + immediate + 2] << 8 ) & 0x0000FF00) | (( Memory[Register[rs] + immediate + 3] ) & 0x000000FF);
		*PC += 1;
	}
}
void regfile::lh   (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC, char Memory[]){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	//error "Address Overflow"
	if((Register[rs] + immediate) > 1022 || (Register[rs] + immediate) < 0){
		error[2] = true;
	}
	//error "Misalignment Error"
	if((Register[rs] + immediate)%2 != 0){
		error[3] = true;
	}
	//error "Number Overflow"
		if(Register[rs] > 0 && immediate > 0 && (Register[rs] + immediate) <= 0){
			error[1] = true;
			//*PC += 1;
		}
		else if(Register[rs] < 0 && immediate < 0 && (Register[rs] + immediate) >= 0){
			error[1] = true;
			//*PC += 1;
		}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = (((Memory[Register[rs] + immediate] << 24 ) >> 16) & 0xFFFFFF00 ) | ((( Memory[Register[rs] + immediate + 1] << 24) >> 24) & 0x000000FF);
		//Register[rt] = Register[rt] & 0x0000FFFF;
		*PC += 1;
	}
}
void regfile::lhu  (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC, char Memory[]){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	//error "Address Overflow"
	if((Register[rs] + immediate) > 1022 || (Register[rs] + immediate) < 0){
		error[2] = true;
	}
	//error "Misalignment Error"
	if((Register[rs] + immediate)%2 != 0){
		error[3] = true;
	}
	//error "Number Overflow"
		if(Register[rs] > 0 && immediate > 0 && (Register[rs] + immediate) <= 0){
			error[1] = true;
			//*PC += 1;
		}
		else if(Register[rs] < 0 && immediate < 0 && (Register[rs] + immediate) >= 0){
			error[1] = true;
			//*PC += 1;
		}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = (((Memory[Register[rs] + immediate] << 24 ) >> 16) & 0x0000FF00) | ((( Memory[Register[rs] + immediate + 1] << 24) >> 24) & 0x000000FF);
		//Register[rt] = Register[rt] & 0x0000FFFF;
		Register[rt] = (unsigned int)Register[rt];
		*PC += 1;
	}
}
void regfile::lb   (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC, char Memory[]){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	//error "Address Overflow"
	if((Register[rs] + immediate) > 1023 || (Register[rs] + immediate) < 0){
		error[2] = true;
	}
	//error "Number Overflow"
		if(Register[rs] > 0 && immediate > 0 && (Register[rs] + immediate) <= 0){
			error[1] = true;
			//*PC += 1;
		}
		else if(Register[rs] < 0 && immediate < 0 && (Register[rs] + immediate) >= 0){
			error[1] = true;
			//*PC += 1;
		}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = ((( Memory[Register[rs] + immediate] << 24) >> 24));
		//Register[rt] = Register[rt] & 0x000000FF;
		*PC += 1;
	}
}
void regfile::lbu  (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC, char Memory[]){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	//error "Address Overflow"
	if((Register[rs] + immediate) > 1023 || (Register[rs] + immediate) < 0){
		error[2] = true;
	}
	//error "Number Overflow"
		if(Register[rs] > 0 && immediate > 0 && (Register[rs] + immediate) <= 0){
			error[1] = true;
			//*PC += 1;
		}
		else if(Register[rs] < 0 && immediate < 0 && (Register[rs] + immediate) >= 0){
			error[1] = true;
			//*PC += 1;
		}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = ((( Memory[Register[rs] + immediate] << 24) >> 24) & 0x000000FF);
		//Register[rt] = Register[rt] & 0x000000FF;
		Register[rt] = (unsigned int)Register[rt];
		*PC += 1;
	}
}
void regfile::sw   (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC, char Memory[]){
	//error "Address Overflow"
	if((Register[rs] + immediate) > 1020 || (Register[rs] + immediate) < 0){
		error[2] = true;
	}
	//error "Misalignment Error"
	if((Register[rs] + immediate)%4 != 0){
		error[3] = true;
	}
	//error "Number Overflow"
		if(Register[rs] > 0 && immediate > 0 && (Register[rs] + immediate) <= 0){
			error[1] = true;
			//*PC += 1;
		}
		else if(Register[rs] < 0 && immediate < 0 && (Register[rs] + immediate) >= 0){
			error[1] = true;
			//*PC += 1;
		}
	if(!error[0] && !error[2] && !error[3]){
		Memory[Register[rs] + immediate] = ( Register[rt] >> 24 ) & 0x000000FF;
		Memory[Register[rs] + immediate + 1] = ( Register[rt] << 8 ) >> 24 & 0x000000FF;
		Memory[Register[rs] + immediate + 2] = ( Register[rt] << 16 ) >> 24 & 0x000000FF;
		Memory[Register[rs] + immediate + 3] = ( Register[rt] << 24 ) >> 24 & 0x000000FF;
		*PC += 1;
	}
}
void regfile::sh   (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC, char Memory[]){
	//error "Address Overflow"
	if((Register[rs] + immediate) > 1022 || (Register[rs] + immediate) < 0){
		error[2] = true;
	}
	//error "Misalignment Error"
	if((Register[rs] + immediate)%2 != 0){
		error[3] = true;
	}
	//error "Number Overflow"
		if(Register[rs] > 0 && immediate > 0 && (Register[rs] + immediate) <= 0){
			error[1] = true;
			//*PC += 1;
		}
		else if(Register[rs] < 0 && immediate < 0 && (Register[rs] + immediate) >= 0){
			error[1] = true;
			//*PC += 1;
		}
	if(!error[0] && !error[2] && !error[3]){
		Memory[Register[rs] + immediate] = ( (Register[rt]&0x0000FFFF) << 16 ) >> 24;
		Memory[Register[rs] + immediate] = Memory[Register[rs] + immediate] & 0x000000FF;
		Memory[Register[rs] + immediate + 1] = ( (Register[rt]&0x0000FFFF) << 24 ) >> 24;
		Memory[Register[rs] + immediate + 1] = Memory[Register[rs] + immediate + 1] & 0x000000FF;
		*PC += 1;
	}
}
void regfile::sb   (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC, char Memory[]){
	//error "Address Overflow"
	if((Register[rs] + immediate) > 1023 || (Register[rs] + immediate) < 0){
		error[2] = true;
	}
	//error "Number Overflow"
		if(Register[rs] > 0 && immediate > 0 && (Register[rs] + immediate) <= 0){
			error[1] = true;
			//*PC += 1;
		}
		else if(Register[rs] < 0 && immediate < 0 && (Register[rs] + immediate) >= 0){
			error[1] = true;
			//*PC += 1;
		}
	if(!error[0] && !error[2] && !error[3]){
		Memory[Register[rs] + immediate] = ( (Register[rt]&0x000000FF) << 24 ) >> 24 & 0x000000FF;
		Memory[Register[rs] + immediate] = Memory[Register[rs] + immediate] & 0x000000FF;
		*PC += 1;
	}
}
void regfile::lui  (unsigned int rt, int immediate, unsigned int *PC){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = (immediate << 16);
		*PC += 1;
	}
}
void regfile::andi (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = Register[rs] & ((unsigned int)immediate & 0x0000FFFF);
		*PC += 1;
	}
}
void regfile::ori  (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = Register[rs] | ((unsigned int)immediate & 0x0000FFFF);
		*PC += 1;
	}
}
void regfile::nori (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = Register[rs] | ((unsigned int)immediate & 0x0000FFFF);
		Register[rt] = (Register[rt]*-1) - 1;
		*PC += 1;
	}
}
void regfile::slti (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC){
	//error "Write to register $0"
	if(rt == 0x00){
		error[0] = true;
		*PC += 1;
	}
	if(!error[0] && !error[2] && !error[3]){
		Register[rt] = (Register[rs] < immediate) ? 1 : 0;
		*PC += 1;
	}
}
void regfile::beq  (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC){
	if(Register[rs] == Register[rt]) *PC += (1 + immediate);
	else *PC += 1;
}
void regfile::bne  (unsigned int rs, unsigned int rt, int immediate, unsigned int *PC){
	if(Register[rs] != Register[rt]) *PC += (1 + immediate);
	else *PC += 1;
}
void regfile::bgtz (unsigned int rs, int immediate, unsigned int *PC){
	if(Register[rs] > 0) *PC += (1 + immediate);
	else *PC += 1;
}


//J-TYPE
void regfile::jal  (unsigned int address, unsigned int *PC){
	Register[31] = (*PC + 1);
	Register[31] = Register[31] << 2;
	*PC = address;
}
