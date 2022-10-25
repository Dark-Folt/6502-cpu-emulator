#include "cpu_6502.h"

int main()
{
    mem_6502 mem;
    cpu_6502 cpu;
    cpu_reset(&cpu, &mem);
    cpu.x = 0x02;
    mem.data[0xFFFC] = INS_LDA_ABSX;
    mem.data[0xFFFD] = 0xDA;
    mem.data[0xFFFE] = 0xFF;
    mem.data[0xFFDC] = 0x63;

    uint32_t  n = 4;
    cpu_execute_inst(&n, &mem, &cpu);

    hexDump("addr", mem.data, MEM_SIZE);
    printf("\n");
    cpu_dump(&cpu);

    return 0;
}