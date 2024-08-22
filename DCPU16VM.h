#ifndef __DCPU16VM_H__
#define __DCPU16VM_H__

#include <stdio.h>

#define DCPU16_RAM_SIZE		0x10000 //words of ram
#define DCPUWORD_MAX		65535	//16bit max
//execution return values
#define EXEC_OK				1
#define EXEC_ABORT			2

//16 bit unsigned word
typedef unsigned short DCPUWORD;

//Virtual Machine state
struct DCPU16_VMachine
{
	DCPUWORD PC;
	DCPUWORD SP;
	DCPUWORD OF;
	DCPUWORD Regs[8];
	DCPUWORD RAM[DCPU16_RAM_SIZE];
};

//Opcodes
enum DCPU16_OpCodes
{
	OP_NON_BASIC = 0x0,
	OP_SET,	OP_ADD,	OP_SUB,	OP_MUL,	OP_DIV,
	OP_MOD,	OP_SHL,	OP_SHR,	OP_AND,	OP_BOR,
	OP_XOR,	OP_IFE,	OP_IFN,	OP_IFG,	OP_IFB
};

//DCPU16 non basic opcodes
enum DCPU16_NonBasicOpCodes
{
	OP_JSR = 0x01,
	OP_BRK = 0x3f //custom break instruction
};

//DCPU16 opcodes a and b
enum DCPU16_Values
{
	//registers
	REG_A = 0x0, REG_B, REG_C, REG_X, REG_Y, REG_Z, REG_I, REG_J,
	//[registers]
	REG_A_ADDR = 0x08, REG_B_ADDR, REG_C_ADDR, REG_X_ADDR, REG_Y_ADDR, REG_Z_ADDR, REG_I_ADDR, REG_J_ADDR,
	//[next word + register]
	REG_A_PLUS = 0x10, REG_B_PLUS, REG_C_PLUS, REG_X_PLUS, REG_Y_PLUS, REG_Z_PLUS, REG_I_PLUS, REG_J_PLUS,
	//stack actions
	POP = 0x18, 
	PEEK, PUSH,
	STACK_POINTER = 0x1b,
	PROGRAM_COUNTER,
	OVERFLOW,
	//[next word]
	NEXT_WORD_ADDR,
	//next word (literal)
	NEXT_WORD_LIT,
	//literals
	LITERAL = 0x20 //0x20-0x3f = 0x1f range
};


//emulator
void DCPU16VMMemorySetup(struct DCPU16_VMachine* vm, const DCPUWORD* p, int wordCount);
int DCPU16VMExecute(struct DCPU16_VMachine* vm);
//compiler
int DCPU16Compile(const char* src, DCPUWORD* dest, const char* cmdline);


#endif //__DCPU16VM_H__