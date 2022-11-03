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

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(cpu.a == 0x37, "0x65 doit être la valeur dans le registre a");
    cr_assert_eq(cycles, 0);
    cr_expect_eq(cpu.z, 0);
    cr_expect_eq(cpu.n, 0);
}

Test(CPU, load_zero_page_x_when_it_wraps)
{
    cpu.x = 0xFF;
    memory.data[0xFFFC] = INS_LDA_ZPX;
    memory.data[0xFFFD] = 0x80; 
    memory.data[0x007F] = 0x37;

    uint32_t cyles = 5;
    cpu_execute_inst(&cyles, &memory, &cpu);
    cr_expect(cpu.a == 0x37, "0x37 doit être la valeur dans le registre a");
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

    uint32_t cycles = 5;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.a == 0x37, "0x65 doit être la valeur dans le registre a");
    cr_assert_eq(cycles, 1);
    cr_expect_eq(cpu.z, 0);
    cr_expect_eq(cpu.n, 0);
}

Test(CPU, load_accumulator_absolute)
{
    cpu_reset(&cpu, &memory);
    memory.data[0xFFFC] = INS_LDA_ABS;
    memory.data[0xFFFD] = 0x23; 
    memory.data[0xFFFE] = 0x56; 
    memory.data[0x5623] = 0x65; 

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(cpu.a == 0x65, "0x65 doit être la valeur dans le registre a");
    cr_assert_eq(cycles, 0);
    cr_expect_eq(cpu.z, 0);
    cr_expect_eq(cpu.n, 0);
}


Test(CPU, load_accumulator_absolute_x)
{
    cpu_reset(&cpu, &memory);
    cpu.x = 0x06;
    memory.data[0xFFFC] = INS_LDA_ABSX;
    memory.data[0xFFFD] = 0x23; 
    memory.data[0xFFFE] = 0x56; 
    memory.data[0x5629] = 0x18; 

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(cpu.a == 0x18, "0x18 doit être la valeur dans le registre a");
    cr_assert_eq(cycles, 0);
    cr_expect_eq(cpu.z, 0);
    cr_expect_eq(cpu.n, 0);
}

Test(CPU, load_accumulator_absolute_y)
{
    cpu_reset(&cpu, &memory);
    cpu.y = 0x24;
    memory.data[0xFFFC] = INS_LDA_ABSY;
    memory.data[0xFFFD] = 0x23; 
    memory.data[0xFFFE] = 0x56; 
    memory.data[0x5647] = 0x87; 

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(cpu.a == 0x87);
    cr_assert_eq(cycles, 0);
    cr_expect_eq(cpu.z, 0);
    cr_expect_eq(cpu.n, 0);
}

Test(CPU, load_accumulator_index_x)
{
    cpu_reset(&cpu, &memory);
    cpu.x = 0x24;
    memory.data[0xFFFC] = INS_LDA_INDX;
    memory.data[0xFFFD] = 0x23; 
    memory.data[0x0047] = 0x65; 
    memory.data[0x0048] = 0x65; 
    memory.data[0x6565] = 0x04; 

    uint32_t cycles = 6;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(cpu.a == 0x04);
    cr_assert_eq(cycles, 0);
    cr_expect_eq(cpu.z, 0);
    cr_expect_eq(cpu.n, 0);
}

Test(CPU, load_accumulator_index_x_with_wrong_cycles)
{
    cpu_reset(&cpu, &memory);
    cpu.x = 0x24;
    memory.data[0xFFFC] = INS_LDA_INDX;
    memory.data[0xFFFD] = 0x23; 
    memory.data[0x0047] = 0x65; 
    memory.data[0x0048] = 0x65; 
    memory.data[0x6565] = 0x04; 

    uint32_t cycles = 9;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.a == 0x04);
    cr_assert_eq(cycles, 3);
    cr_expect_eq(cpu.z, 0);
    cr_expect_eq(cpu.n, 0);
}


Test(CPU, load_accumulator_index_y)
{
    cpu_reset(&cpu, &memory);
    cpu.y = 0x04;
    memory.data[0xFFFC] = INS_LDA_INDY;
    memory.data[0xFFFD] = 0x02; 
    memory.data[0x0002] = 0x00; 
    memory.data[0x0003] = 0x80; 
    memory.data[0x8004] = 0x37; 

    uint32_t cycles = 5;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(cpu.a == 0x37);
    cr_assert_eq(cycles, 0);
    cr_expect_eq(cpu.z, 0);
    cr_expect_eq(cpu.n, 0);
}

Test(CPU, load_accumulator_index_y_if_page_cross)
{
    cpu_reset(&cpu, &memory);
    cpu.y = 0xFF;
    memory.data[0xFFFC] = INS_LDA_INDY;
    memory.data[0xFFFD] = 0x02; 
    memory.data[0x0002] = 0x00; 
    memory.data[0x0003] = 0x80; 
    memory.data[0x80FF] = 0x37; 

    uint32_t cycles = 6;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(cpu.a == 0x37);
    cr_assert_eq(cycles, 0);
    cr_expect_eq(cpu.z, 0);
    cr_expect_eq(cpu.n, 0);
}




















