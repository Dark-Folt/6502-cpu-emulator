#include "../include/cpu_6502.h"

int main()
{
    mem_6502 mem;
    cpu_6502 cpu;
    cpu_reset(&cpu, &mem);
    cpu.y = 0x04;
    mem.data[0xFFFC] = INS_LDA_ABSY;
    mem.data[0xFFFD] = 0x02; 
    mem.data[0x0002] = 0x00;  
    mem.data[0x0003] = 0x80;
    mem.data[0x8004] = 0x37;

    uint32_t  n = 4;
    cpu_execute_inst(&n, &mem, &cpu);

    hexDump("addr", mem.data, MEM_SIZE);
    printf("\n");
    cpu_dump(&cpu);

    return 0;
}