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

/**
 * Load Accumulator Tests
*/
Test(CPU, LDA_IM)
{
    memory.data[0xFFFC] = INS_LDA_IM;
    memory.data[0xFFFD] = 0x84; 

    uint32_t  n = 2;
    cpu_execute_inst(&n, &memory, &cpu);

    cr_expect(cpu.a == 0x84, "0x84 doit être la valeur dans le registre");
    cr_expect(n == 0, "le nombre de cycle doit être égale à 0");
}

Test(CPU, LDA_ZP)
{
    memory.data[0xFFFC] = INS_LDA_ZP;
    memory.data[0xFFFD] = 0x05; 
    memory.data[0x0005] = 0x65;

    uint32_t  n = 3;
    cpu_execute_inst(&n, &memory, &cpu);

    cr_expect(cpu.a == 0x65, "0x65 doit être la valeur dans le registre a");
    cr_expect(n == 0, "le nombre de cycle doit être égale à 0");
}

Test(CPU, LDA_ZPX)
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

Test(CPU, LDA_ZPX_when_it_wraps)
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

Test(CPU, LDA_ZPX_with_wrong_cycles)
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

Test(CPU, LDA_ABS)
{
    // cpu_reset(&cpu, &memory);
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

Test(CPU, LDA_ABSX)
{
    // cpu_reset(&cpu, &memory);
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

Test(CPU, LDA_ABSY)
{
    // cpu_reset(&cpu, &memory);
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

Test(CPU, LDA_INDX)
{
    // cpu_reset(&cpu, &memory);
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

Test(CPU, LDA_INDX_with_wrong_cycles)
{
    // cpu_reset(&cpu, &memory);
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

Test(CPU, LDA_INDY)
{
    // cpu_reset(&cpu, &memory);
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

Test(CPU, LDA_INDY_if_page_cross)
{
    // cpu_reset(&cpu, &memory);
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

/**
 * Load X Register Tests
*/

Test(CPU, LDX_IM)
{
    // cpu_reset(&cpu, &memory);
    memory.data[0xFFFC] = INS_LDX_IM;
    memory.data[0xFFFD] = 0x02; 

    uint32_t  cycles = 2;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.x == 0x02);
    cr_assert_eq(cycles, 0);
}

Test(CPU, LDX_ZP)
{
    // cpu_reset(&cpu, &memory);
    memory.data[0xFFFC] = INS_LDX_ZP;
    memory.data[0xFFFD] = 0x02; 
    memory.data[0x0002] = 0x44; 

    uint32_t  cycles = 3;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.x == 0x44);
    cr_assert_eq(cycles, 0);
}

Test(CPU, LDX_ZPY)
{
    // cpu_reset(&cpu, &memory);
    cpu.y = 0x04;
    memory.data[0xFFFC] = INS_LDX_ZPY;
    memory.data[0xFFFD] = 0x08;
    memory.data[0x000C] = 0x88;

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.x == 0x88);
    cr_assert_eq(cycles, 0);
}

Test(CPU, LDX_ABS)
{
    // cpu_reset(&cpu, &memory);
    memory.data[0xFFFC] = INS_LDX_ABS;
    memory.data[0xFFFD] = 0x08;
    memory.data[0xFFFE] = 0x80;
    memory.data[0x8008] = 0xBE;

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.x == 0xBE);
    cr_assert_eq(cycles, 0);
}

Test(CPU, LDX_ABSY)
{
    // cpu_reset(&cpu, &memory);
    cpu.y = 0x24;
    memory.data[0xFFFC] = INS_LDX_ABSY;
    memory.data[0xFFFD] = 0x08;
    memory.data[0xFFFE] = 0x80;
    memory.data[0x802C] = 0xA1;

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.x == 0xA1);
    cr_assert_eq(cycles, 0);
}

Test(CPU, LDX_ABSY_page_crossed)
{
    // cpu_reset(&cpu, &memory);
    cpu.y = 0xFF;
    memory.data[0xFFFC] = INS_LDX_ABSY;
    memory.data[0xFFFD] = 0x08;
    memory.data[0xFFFE] = 0x80;
    memory.data[0x8107] = 0xA4;

    uint32_t  cycles = 5;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.x == 0xA4);
    cr_assert_eq(cycles, 0);
}


/**
 * Load Y Register Tests
*/


Test(CPU, LDY_IM)
{
    // cpu_reset(&cpu, &memory);
    memory.data[0xFFFC] = INS_LDY_IM;
    memory.data[0xFFFD] = 0x02; 
    memory.data[0xFFFE] = 0xB3; 

    uint32_t  cycles = 2;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.y == 0xB3);
    cr_assert_eq(cycles, 0);
}

Test(CPU, LDY_ZP)
{
    // cpu_reset(&cpu, &memory);
    memory.data[0xFFFC] = INS_LDY_ZP;
    memory.data[0xFFFD] = 0x02; 
    memory.data[0x0002] = 0xB3; 

    uint32_t  cycles = 3;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.y == 0xB3);
    cr_assert_eq(cycles, 0);
}

Test(CPU, LDY_ZPX)
{
    // cpu_reset(&cpu, &memory);
    memory.data[0xFFFC] = INS_LDY_ZPX;
    memory.data[0xFFFD] = 0x02; 
    memory.data[0x0002] = 0xB3; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.y == 0xB3);
    cr_assert_eq(cycles, 0);
}

Test(CPU, LDY_ABS)
{
    // cpu_reset(&cpu, &memory);
    memory.data[0xFFFC] = INS_LDY_ABS;
    memory.data[0xFFFD] = 0x02; 
    memory.data[0x0002] = 0xB3; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.y == 0xB3);
    cr_assert_eq(cycles, 0);
}

Test(CPU, LDY_ABSX)
{
    // cpu_reset(&cpu, &memory);
    cpu.x = 0x36;
    memory.data[0xFFFC] = INS_LDY_ABSX;
    memory.data[0xFFFD] = 0x02; 
    memory.data[0xFFFE] = 0xB3; 
    memory.data[0xB338] = 0x87; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.y == 0x87);
    cr_assert_eq(cycles, 0);
}

Test(CPU, LDY_ABX_page_cross)
{
    // cpu_reset(&cpu, &memory);
    cpu.x = 0xFF;
    memory.data[0xFFFC] = INS_LDY_ABSX;
    memory.data[0xFFFD] = 0x80; 
    memory.data[0xFFFE] = 0x90; 
    memory.data[0x917F] = 0x87; 

    uint32_t  cycles = 5;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.y == 0x87);
    cr_assert_eq(cycles, 0);
}



























