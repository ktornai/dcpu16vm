#include <stdlib.h>
#include <string.h>
#include "dcpu16vm.h"

#define GetValueA(inst)		((DCPUWORD)((inst >> 4) & 0x3f))
#define GetValueB(inst)		((DCPUWORD)((inst >> 10) & 0x3f))
#define GetOpCode(inst)		((DCPUWORD)(inst & 0xf))

static DCPUWORD DecodeValue(DCPUWORD value, DCPUWORD** decValue, struct DCPU16_VMachine* vm);
static void SkipNextInstruction(struct DCPU16_VMachine* vm);
static DCPUWORD IsMultiByte(DCPUWORD value);


DCPUWORD DecodeValue(DCPUWORD value, DCPUWORD** decValue, struct DCPU16_VMachine* vm)
{
	DCPUWORD* val = 0;

	if(value <= REG_J)
	{
		val = vm->Regs + value;
	}
	else if(value <= REG_J_ADDR)
	{
		DCPUWORD reg = value - REG_A_ADDR;
		val = &vm->RAM[vm->Regs[reg]];
	}
	else if(value <= REG_J_PLUS)
	{
		DCPUWORD reg = value - REG_A_PLUS;
		DCPUWORD nextWord = vm->RAM[(vm->PC++)];
		val = &vm->RAM[nextWord + vm->Regs[reg]];
	}
	else if(value == POP)
	{
		val = &vm->RAM[vm->SP++];
	}
	else if(value == PEEK)
	{
		val = &vm->RAM[vm->SP];
	}
	else if(value == PUSH)
	{
		val = &vm->RAM[vm->SP--];
	}
	else if(value == STACK_POINTER)
	{
		val = &vm->SP;
	}
	else if(value == PROGRAM_COUNTER)
	{
		val = &vm->PC;
	}
	else if(value == OVERFLOW)
	{
		val = &vm->OF;
	}
	else if(value == NEXT_WORD_ADDR)
	{
		val = &vm->RAM[vm->RAM[vm->PC++]];
	}
	else if(value == NEXT_WORD_LIT)
	{
		val = &vm->RAM[vm->PC++];
	}
	else if(value >= LITERAL)
	{
		val = (DCPUWORD*)(value - LITERAL);
	}

	*decValue = val;
	return value;
}

static DCPUWORD IsMultiByte(DCPUWORD value)
{
	if(value >= REG_A_PLUS && value <= REG_J_PLUS ||
		value == NEXT_WORD_ADDR ||
		value == NEXT_WORD_LIT)
		return 1;
	
	return 0;
}

static void SkipNextInstruction(struct DCPU16_VMachine* vm)
{
	DCPUWORD inst = vm->RAM[vm->PC++];
	
	vm->PC += IsMultiByte(GetValueB(inst));
	vm->PC += ((inst & 0xf) > 0x0000) ? IsMultiByte(GetValueA(inst)) : 0;
}

int DCPU16VMExecute(struct DCPU16_VMachine* vm)
{
	DCPUWORD* mem = vm->RAM;

	while(1)
	{
		DCPUWORD inst = mem[vm->PC++];
		DCPUWORD ocBasic = GetOpCode(inst); //try to get opcode as basic opcode
		DCPUWORD* workingA = NULL, *workingB = NULL;
		DCPUWORD valueA = GetValueA(inst);
		int a, b;

		if(DecodeValue(valueA, &workingA, vm) >= LITERAL)
			a = (int)workingA;
		else
			a = *workingA;

		if(DecodeValue(GetValueB(inst), &workingB, vm) >= LITERAL)
			b = (int)workingB;
		else
			b = *workingB;
			
		if(ocBasic > 0)
		{
			vm->OF = 0x0000;

			switch(ocBasic)
			{
			case OP_SET:
				*workingA = b;
				break;
			case OP_ADD:
				*workingA = a += b;
				vm->OF = (a > 0xffff) ? 0x0001 : 0x0000;
				break;
			case OP_SUB:
				*workingA = a -= b;
				vm->OF = (a < 0) ? 0xffff : 0x0000;
				break;
			case OP_MUL:
				*workingA *= b;
				vm->OF = ((a*b) >> 16) & 0xffff;
				break;
			case OP_DIV:
				*workingA = ((b != 0) ? a / b : 0);
				break;
			case OP_MOD:
				*workingA = ((b != 0) ? a % b : 0);
				break;
			case OP_SHL:
				*workingA <<= b;
				vm->OF = ((a<<b) >> 16) & 0xffff;
				break;
			case OP_SHR:
				*workingA >>= b;
				vm->OF = ((a<<16) >> b) & 0xffff;
				break;
			case OP_AND:
				*workingA &= b;
				break;
			case OP_BOR:
				*workingA |= b;
				break;
			case OP_XOR:
				*workingA ^= b;
				break;
			case OP_IFE:
				if(a != b)
					SkipNextInstruction(vm);
				break;
			case OP_IFN:
				if(a == b)
					SkipNextInstruction(vm);
				break;
			case OP_IFG:
				if(a <= b)
					SkipNextInstruction(vm);
				break;
			case OP_IFB:
				if((a & b) == 0)
					SkipNextInstruction(vm);
				break;
			default:
				return EXEC_ABORT;
			}
		}
		else
		{
			switch(valueA)
			{
			case OP_JSR:
				mem[vm->SP] = vm->PC;
				vm->PC = b;
				break;
			case OP_BRK:
			default:
				return EXEC_ABORT;
			}
		}
	}

	return EXEC_OK;
}

void DCPU16VMMemorySetup(struct DCPU16_VMachine* vm, const DCPUWORD* p, int wordCount)
{
	vm->PC = 0x0000; //program counter starts at 0
	vm->SP = 0xffff; //stack pointer starts at 0ffff
	vm->OF = 0x0000; //overflow flag is 0 by default

	memcpy(vm->RAM, p, wordCount * sizeof(DCPUWORD));
}