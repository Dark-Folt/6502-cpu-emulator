#include "cpu_6502.h"

int main()
{
    mem_6502 mem;
    cpu_6502 cpu;
    cpu_reset(&cpu, &mem);
    mem.data[0xFFFC] = INS_LDA_ABS;
    mem.data[0xFFFD] = 0xDA;
    mem.data[0xFFFE] = 0xFF;
    mem.data[0xFFDA] = 0x65;

    uint32_t  n = 4;
    cpu_execute_inst(&n, &mem, &cpu);

    // HexDump(mem.data, MEM_SIZE);
    hexDump("addr", mem.data, MEM_SIZE);
    // for(size_t i=0; i < MEM_SIZE; i++) {
    //     printf("%ld\t%x\n", i, mem.data[i]);
    // }
    printf("\n");
    cpu_dump(&cpu);

    return 0;
}