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


/** Test instructions **/
Test(CPU, load_immediate_address)
{
    memory.data[0xFFFC] = INS_LDA_IM;
    memory.data[0xFFFD] = 0x84; 

    uint32_t  n = 2;
    cpu_execute_inst(&n, &memory, &cpu);

    cr_expect(cpu.a == 0x84, "0x84 doit être la valeur dans le registre");
    cr_expect(n == 0, "le nombre de cycle doit être égale à 0");
}

Test(CPU, load_zero_page)
{
    memory.data[0xFFFC] = INS_LDA_ZP;
    memory.data[0xFFFD] = 0x05; 
    memory.data[0x0005] = 0x65;

    uint32_t  n = 3;
    cpu_execute_inst(&n, &memory, &cpu);

    cr_expect(cpu.a == 0x65, "0x65 doit être la valeur dans le registre a");
    cr_expect(n == 0, "le nombre de cycle doit être égale à 0");
}


Test(CPU, load_zero_page_x)
{
    cpu.x = 0x04;
    memory.data[0xFFFC] = INS_LDA_ZPX;
    memory.data[0xFFFD] = 0x05; 
    memory.data[0x0009] = 0x37;

    uint32_t cyles = 4;
    cpu_execute_inst(&cyles, &memory, &cpu);

    cr_expect(cpu.a == 0x37, "0x65 doit être la valeur dans le registre a");
    cr_assert_eq(cyles, 0);
    cr_expect_eq(cpu.z, 0);
    cr_expect_eq(cpu.n, 0);
}

Test(CPU, load_zero_page_x_with_wrong_cycles)
{
    cpu.x = 0x04;
    memory.data[0xFFFC] = INS_LDA_ZPX;
    memory.data[0xFFFD] = 0x05; 
    memory.data[0x0009] = 0x37;

    uint32_t cyles = 6;
    cpu_execute_inst(&cyles, &memory, &cpu);

    cr_expect(cpu.a == 0x37, "0x65 doit être la valeur dans le registre a");
    cr_assert_eq(cyles, 2);
    cr_expect_eq(cpu.z, 0);
    cr_expect_eq(cpu.n, 0);
}