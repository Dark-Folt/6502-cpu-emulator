#include <criterion/criterion.h>
#include <string.h>
#include "../include/cpu_6502.h"

mem_6502 memory;
cpu_6502 cpu;

void setup_memory(void)
{
    mem_initialise(&memory);
}

void setup_cpu(void)
{
    cpu_reset(&cpu, &memory);
}

TestSuite(Memory, .init=setup_memory);

Test(Memory, init_memory) {

    byte exp_data[MEM_SIZE] = {0};

    cr_expect_arr_eq(memory.data, exp_data, sizeof(byte), "Chaque valeur de la memoire doit être égale à 0 apres initialisation");
} 

/**CPU test*/
TestSuite(CPU, .init=setup_cpu);

Test(CPU, init_cpu)
{
    cr_assert(
        cpu.a == cpu.x == cpu.y == 0
    , "Le registre à 0");
}

Test(CPU,  zero_page)
{
    cr_assert(cpu.pc == 0xFFFC);
}
