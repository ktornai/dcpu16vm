#include <stdio.h>
#include <stdlib.h>
#include "dcpu16vm.h"


const DCPUWORD prog[] = 
{
  0x8001, 0x7801, 0x0010, 0x7C09, 0x01FF, 0x7901, 0x8000, 0x0010,
  0x85E2, 0x0010, 0x7DED, 0x0010, 0x0400, 0x85C1, 0x3F00, 0x0000, 0xC5C1
};


void DumpVMRegisters(struct DCPU16_VMachine* vm)
{
	printf("A:0x%.4x (%i)\nB:0x%.4x (%i)\nC:0x%.4x (%i)\nX:0x%.4x (%i)\nY:0x%.4x (%i)\nZ:0x%.4x (%i)\nI:0x%.4x (%i)\nJ:0x%.4x (%i)\n\n", 
			vm->Regs[REG_A], vm->Regs[REG_A], vm->Regs[REG_B], vm->Regs[REG_B], vm->Regs[REG_C], vm->Regs[REG_C], 
			vm->Regs[REG_X], vm->Regs[REG_X], vm->Regs[REG_Y], vm->Regs[REG_Y], vm->Regs[REG_Z], vm->Regs[REG_Z], 
			vm->Regs[REG_I], vm->Regs[REG_I], vm->Regs[REG_J], vm->Regs[REG_J]);
	printf("PC:0x%.4x (%i)\nSP:0x%.4x (%i)\nOF:0x%.4x (%i)\n\n", vm->PC, vm->PC, vm->SP, vm->SP, vm->OF, vm->OF);
}

void DumpVMMemory(struct DCPU16_VMachine* vm)
{
	int i;
	FILE* f = fopen("d:\\memdump.txt", "wt");

	for(i=0; i<(DCPU16_RAM_SIZE); i+=8)
	{
		fprintf(f, "0x%.4x: 0x%.4x 0x%.4x 0x%.4x 0x%.4x 0x%.4x 0x%.4x 0x%.4x 0x%.4x\n", i, vm->RAM[i], vm->RAM[i+1], vm->RAM[i+2], vm->RAM[i+3], 
																	vm->RAM[i+4], vm->RAM[i+5], vm->RAM[i+6],vm->RAM[i+7]);    
	}

	fclose(f);
}


void VM_Main()
{
	struct DCPU16_VMachine vm = {0};

	vm.RAM[0x2000] = 0xabcd;

	DCPU16VMMemorySetup(&vm, prog, sizeof(prog) / sizeof(prog[0]));
	DCPU16VMExecute(&vm);

	DumpVMRegisters(&vm);
	DumpVMMemory(&vm);
}

int main()
{
	printf("Hello DCPU16!\n\n");

	VM_Main();

	printf("\n");
	system("PAUSE");
}