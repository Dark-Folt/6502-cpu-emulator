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
 * Programm Counter
*/
Test(CPU, PC)
{
    uint32_t  n = 2;
    memory.data[0xFFFC] = INS_LDA_IM;
    memory.data[0xFFFD] = 0x87;
    cpu_execute_inst(&n, &memory, &cpu);
    // printf("pc: %x\n",cpu.pc);
    cr_expect(cpu.pc == 0xFFFD + 1);
    cr_expect(cpu.a == 0x87);
}


/**
 * Load Accumulator Tests
*/
Test(CPU, LDA_IM)
{
    memory.data[0xFFFC] = INS_LDA_IM;
    memory.data[0xFFFD] = 0x84; 

    uint32_t  cycles = 2;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(cpu.a == 0x84);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_expect(cycles == 0);
}

Test(CPU, LDA_ZP)
{
    memory.data[0xFFFC] = INS_LDA_ZP;
    memory.data[0xFFFD] = 0x05; 
    memory.data[0x0005] = 0x65;

    uint32_t  cycles = 3;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(cpu.a == 0x65);
    cr_expect(cycles == 0);
    cr_expect(cpu.pc == 0xFFFC + 2);
}

Test(CPU, LDA_ZPX)
{
    cpu.x = 0x04;
    memory.data[0xFFFC] = INS_LDA_ZPX;
    memory.data[0xFFFD] = 0x05; 
    memory.data[0x0009] = 0x37;

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(cpu.a == 0x37);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_assert_eq(cycles, 0);
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
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_assert_eq(cyles, 0);
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
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_assert_eq(cycles, 1);
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

    cr_expect(cpu.a == 0x65);
    cr_expect(cpu.pc == 0xFFFC + 3);
    cr_assert_eq(cycles, 0);
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

    cr_expect(cpu.a == 0x18);
    cr_expect(cpu.pc == 0xFFFC + 3);
    cr_assert_eq(cycles, 0);
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
    cr_expect(cpu.pc == 0xFFFC + 3);
    cr_assert_eq(cycles, 0);
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
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_assert_eq(cycles, 0);
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
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_assert_eq(cycles, 3);
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
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_assert_eq(cycles, 0);
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
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_assert_eq(cycles, 0);
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
    cr_expect(cpu.pc == 0xFFFC + 2);
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
    cr_expect(cpu.pc == 0xFFFC + 2);
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
    cr_expect(cpu.pc == 0xFFFC + 2);
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
    cr_expect(cpu.pc == 0xFFFC + 3);
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
    cr_expect(cpu.pc == 0xFFFC + 3);
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
    cr_expect(cpu.pc == 0xFFFC + 3);
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

    uint32_t  cycles = 2;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_expect(cpu.y == 0x02);
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
    cr_expect(cpu.pc == 0xFFFC + 2);
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
    cr_expect(cpu.pc == 0xFFFC + 2);
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
    cr_expect(cpu.pc == 0xFFFC + 3);
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
    cr_expect(cpu.pc == 0xFFFC + 3);
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
    cr_expect(cpu.pc == 0xFFFC + 3);
    cr_expect(cpu.y == 0x87);
    cr_assert_eq(cycles, 0);
}


/**
 * STore A into memory Tests
*/

Test(CPU, STA_ZP)
{
    cpu.a = 0x12;
    memory.data[0xFFFC] = INS_STA_ZP;
    memory.data[0xFFFD] = 0x84; 

    uint32_t  cycles = 3;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x0084] == 0x12);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_expect(cycles == 0, "le nombre de cycle doit être égale à 0");
}

Test(CPU, STA_ZPX)
{
    cpu.a = 0x12;
    cpu.x = 0x26;
    memory.data[0xFFFC] = INS_STA_ZPX;
    memory.data[0xFFFD] = 0x84; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x00AA] == 0x12);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_expect(cycles == 0);
}

Test(CPU, STA_ZPX_wrong_cycles)
{
    cpu.a = 0x12;
    cpu.x = 0x26;
    memory.data[0xFFFC] = INS_STA_ZPX;
    memory.data[0xFFFD] = 0x84; 

    uint32_t  cycles = 5;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x00AA] == 0x12);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_expect(cycles == 1);
}

Test(CPU, STA_ABS)
{
    cpu.a = 0x12;
    cpu.pc = 0xFF00;
    memory.data[0xFF00] = INS_STA_ABS;
    memory.data[0xFF01] = 0x92; 
    memory.data[0xFF02] = 0x93; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x9392] == 0x12);
    cr_expect(cpu.pc == 0xFF00 + 3);
    cr_expect(cycles == 0);
}

Test(CPU, STA_ABSX)
{
    cpu.a = 0x12;
    cpu.x = 0x44;
    memory.data[0xFFFC] = INS_STA_ABSX;
    memory.data[0xFFFD] = 0x92; 
    memory.data[0xFFFE] = 0x93; 

    uint32_t  cycles = 5;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x93D6] == 0x12);
    cr_expect(cpu.pc == 0xFFFC + 3);
    cr_expect(cycles == 0);
}

Test(CPU, STA_ABSY)
{
    cpu.a = 0x12;
    cpu.y = 0x44;
    memory.data[0xFFFC] = INS_STA_ABSY;
    memory.data[0xFFFD] = 0x92; 
    memory.data[0xFFFE] = 0x93; 

    uint32_t  cycles = 5;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x93D6] == 0x12);
    cr_expect(cpu.pc == 0xFFFC + 3);
    cr_expect(cycles == 0);
}

Test(CPU, STA_INDX)
{
    cpu.a = 0x99;
    cpu.x = 0x44;
    memory.data[0xFFFC] = INS_STA_INDX;
    memory.data[0xFFFD] = 0x92; 
    memory.data[0x00D6] = 0x24; 
    memory.data[0x00D7] = 0x77; 

    uint32_t  cycles = 6;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x7724] == 0x99);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_expect(cycles == 0);
}

Test(CPU, STA_INDY)
{
    cpu.a = 0x99;
    cpu.y = 0x44;
    memory.data[0xFFFC] = INS_STA_INDY;
    memory.data[0xFFFD] = 0x92; 
    memory.data[0x0092] = 0x00; 
    memory.data[0x0093] = 0x80; 
    memory.data[0x8000 + 0x44] = 0x99; 

    uint32_t  cycles = 6;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x8000 + 0x44] == 0x99);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_expect(cycles == 0);
}

/**
 * Store X register into memory
*/
Test(CPU, STX_ZP)
{
    cpu.x = 0x12;
    memory.data[0xFFFC] = INS_STX_ZP;
    memory.data[0xFFFD] = 0x84; 

    uint32_t  cycles = 3;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x0084] == 0x12);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_expect(cycles == 0, "le nombre de cycle doit être égale à 0");
}

Test(CPU, STX_ZPY)
{
    cpu.x = 0x12;
    cpu.y = 0x08;

    memory.data[0xFFFC] = INS_STX_ZPY;
    memory.data[0xFFFD] = 0x84; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x84 + 0x08] == 0x12);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_expect(cycles == 0, "le nombre de cycle doit être égale à 0");
}

Test(CPU, STX_ABS)
{
    cpu.x = 0x08;

    memory.data[0xFFFC] = INS_STX_ABS;
    memory.data[0xFFFD] = 0x97; 
    memory.data[0xFFFE] = 0x61; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x6197] == 0x08);
    cr_expect(cpu.pc == 0xFFFC + 3);
    cr_expect(cycles == 0, "le nombre de cycle doit être égale à 0");
}

/**
 * Store X register into memory
*/
Test(CPU, STY_ZP)
{
    cpu.y = 0x08;

    memory.data[0xFFFC] = INS_STY_ZP;
    memory.data[0xFFFD] = 0x97; 

    uint32_t  cycles = 3;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x97] == 0x08);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_expect(cycles == 0, "le nombre de cycle doit être égale à 0");
}

Test(CPU, STY_ZPX)
{
    cpu.y = 0x08;
    cpu.x = 0x38;

    memory.data[0xFFFC] = INS_STY_ZPX;
    memory.data[0xFFFD] = 0x97; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x97 + 0x38] == 0x08);
    cr_expect(cpu.pc == 0xFFFC + 2);
    cr_expect(cycles == 0, "le nombre de cycle doit être égale à 0");
}

Test(CPU, STY_ABS)
{
    cpu.y = 0x08;

    memory.data[0xFFFC] = INS_STY_ABS;
    memory.data[0xFFFD] = 0x97; 
    memory.data[0xFFFE] = 0x97; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    cr_expect(memory.data[0x9797] == 0x08);
    cr_expect(cpu.pc == 0xFFFC + 3);
    cr_expect(cycles == 0, "le nombre de cycle doit être égale à 0");
}

/**
 * JUMPS test
*/

Test(CPU, STACK_PUSH_BYTE)
{
    cpu_reset(&cpu, &memory);
    uint32_t cycles = 5;
    cpu_push_byte_on_stack(&cycles, 0x80, &memory, &cpu);
    cpu_push_byte_on_stack(&cycles, 0x70, &memory, &cpu);
    cr_expect(memory.data[cpu.sp + 1] == 0x80);
    cr_expect(memory.data[cpu.sp] == 0x70);
}

Test(CPU, STACK_PUSH_WORD)
{
    cpu_reset(&cpu, &memory);
    uint32_t cycles = 5;
    cpu_push_word_on_stack(&cycles, 0x8024, &memory, &cpu);
    cr_expect(memory.data[cpu.sp + 1] == 0x80);
    cr_expect(memory.data[cpu.sp] == 0x24);
}

Test(CPU, POP_BYTE_OFF_THE_STACK)
{
    cpu_reset(&cpu, &memory);
    uint32_t c = 6;
    cpu_push_word_on_stack(&c, 0xC0E2, &memory, &cpu);

    byte data = cpu_pop_byte_off_stack(&c, &memory, &cpu);
    byte d = cpu_pop_byte_off_stack(&c, &memory, &cpu);
    cr_expect(data == 0xE2);
    cr_expect(d == 0xC0);
    cr_expect(c == 0);
}

Test(CPU, POP_WORD_OFF_THE_STACK)
{
    cpu_reset(&cpu, &memory);
    uint32_t c = 6;
    cpu_push_word_on_stack(&c, 0xC0E2, &memory, &cpu);

    word data = cpu_pop_word_off_stack(&c, &memory, &cpu);
    cr_expect(data == 0xC0E2);
    cr_expect(c == 0);
}
Test(CPU, RTS)
{
    cpu_reset(&cpu, &memory);
    uint32_t c = 6 + 2;
    cpu_push_word_on_stack(&c, 0xC0E2, &memory, &cpu);
    memory.data[0xFFFC] = INS_RTS_IMP;
    cpu_execute_inst(&c, &memory, &cpu);
    cr_expect(cpu.pc == 0xC0E2 + 1);
    cr_expect(c == 0);
}

Test(CPU, JSR)
{
    cpu_reset(&cpu, &memory);
    cpu.pc = 0xC0E0;
    memory.data[0xC0E0] = INS_JSR; // 6 cycles
    memory.data[0xC0E1] = 0x00;
    memory.data[0xC0E2] = 0x80;
    memory.data[0xC0E3] = INS_LDX_IM; // 2 cycles
    memory.data[0xC0E4] = 0x70;
    memory.data[0xC0E5] = INS_LDY_IM; // 2 cycles
    memory.data[0xC0E6] = 0xA7;

    memory.data[0x8000] = INS_LDA_IM; // 2 cycles
    memory.data[0x8001] = 0xBE;
    memory.data[0x8002] = INS_STA_ABS; // 4 cycles
    memory.data[0x8003] = 0x15;
    memory.data[0x8004] = 0x40;
    memory.data[0x8005] = INS_RTS_IMP; // 6 cycles

    uint32_t cycles = 6 + 2 + 4 + 6 + 2 + 2;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.x == 0x70);
    cr_expect(cpu.y == 0xA7);
    cr_expect(cpu.a == 0xBE);
    cr_expect(cpu.pc == 0xC0E7);
    cr_expect(memory.data[0x4015] == 0xBE);
    cr_assert_eq(cycles, 0);
}

// Test(CPU, CMP_IM_ACC_INF_MEMORY_V1)
// {
//     int cycles = 2;

//     // Initialize CPU and memory
//     cpu_reset(&cpu, &memory);

//     // Load program with CMP instruction
//     memory_write_byte(&memory, 0x8000, 0xC9); // CMP #$05
//     memory_write_byte(&memory, 0x8001, 0x05);

//     // Set accumulator to a value greater than the compared value
//     cpu.a = 0x07;
//     cpu.pc = 0x8000;

//     // Execute CMP instruction
//     cpu_step(&cpu, &memory, &cycles);

//     // Check flags
//     assert(IS_FLAG_SET(&cpu, FLAG_CARRY));    // Carry should be set (7 >= 5)
//     assert(!IS_FLAG_SET(&cpu, FLAG_ZERO));    // Zero should not be set (7 != 5)
//     assert(!IS_FLAG_SET(&cpu, FLAG_NEGATIVE));// Negative should not be set (result >= 0)

//     // Change accumulator to a value equal to the compared value
//     cpu.a = 0x05;
//     cpu.pc = 0x8000;

//     // Execute CMP instruction
//     cpu_step(&cpu, &memory, &cycles);

//     // Check flags
//     assert(IS_FLAG_SET(&cpu, FLAG_CARRY));    // Carry should be set (5 >= 5)
//     assert(IS_FLAG_SET(&cpu, FLAG_ZERO));     // Zero should be set (5 == 5)
//     assert(!IS_FLAG_SET(&cpu, FLAG_NEGATIVE));// Negative should not be set (result >= 0)

//     // Change accumulator to a value less than the compared value
//     cpu.a = 0x03;
//     cpu.pc = 0x8000;

//     // Execute CMP instruction
//     cpu_step(&cpu, &memory, &cycles);

//     // Check flags
//     assert(!IS_FLAG_SET(&cpu, FLAG_CARRY));   // Carry should not be set (3 < 5)
//     assert(!IS_FLAG_SET(&cpu, FLAG_ZERO));    // Zero should not be set (3 != 5)
//     assert(IS_FLAG_SET(&cpu, FLAG_NEGATIVE)); // Negative should be set (result < 0)

//     printf("CMP_IM test passed!\n");

// }

// Test(CPU, CMP_IM_ACC_INF_MEMORY)
// {
//     cpu_reset(&cpu, &memory);
//     cpu.pc = 0xC0E0;
//     cpu.a = 13;
//     memory.data[0xC0E0] = CMP_IM;
//     memory.data[0xC0E1] = 15;

//     // copy of the cpu
//     cpu_6502 cpu_copy;
//     memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

//     uint32_t cycles = 2;
//     cpu_execute_inst(&cycles, &memory, &cpu);
//     cr_expect(cpu.n == 1, "Le flag n doit être activé");
//     // make sure othe flag are not affected
//     cr_expect(cpu.z == cpu_copy.z, "Le flag z ne doit pas être modifié");
//     cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
//     cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
//     cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
//     cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

//     cr_assert_eq(cycles, 0);
// }

Test(CPU, CMP_IM_ACC_SUP_MEMORY)
{
    cpu_reset(&cpu, &memory);
    cpu.pc = 0xC0E0;
    cpu.a = 25;
    memory.data[0xC0E0] = CMP_IM;
    memory.data[0xC0E1] = 15;

    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 2;
    cpu_execute_inst(&cycles, &memory, &cpu);
    // cr_expect(cpu.c == 1, "Le flag c doit être activé");
    // cr_expect(cpu.z == 0, "Le flag z doit être activé");
    cr_expect(cpu.pc == 0xC0E0 + 2, "PC n'est pas incrémenté");
    // make sure othe flag are not affected
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0);
}

Test(CPU, CMP_IM_ACC_EQ_MEMORY)
{
    cpu_reset(&cpu, &memory);
    cpu.pc = 0xC0E0;
    cpu.a = 25;
    memory.data[0xC0E0] = CMP_IM;
    memory.data[0xC0E1] = 25;

    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 2;
    cpu_execute_inst(&cycles, &memory, &cpu);
    // cr_expect(cpu.c == 1, "Le flag c doit être activé");
    // cr_expect(cpu.z == 1, "Le flag z doit être activé");
    // make sure othe flag are not affected
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0);
}

Test(CPU, CMP_ZERO_PAGE_ACC_INF_MEMORY)
{
    cpu_reset(&cpu, &memory);
    cpu.pc = 0xC0E0;
    cpu.a = 13;
    // ZP
    memory.data[0x00FA] = 15;

    memory.data[0xC0E0] = CMP_ZP;
    memory.data[0xC0E1] = 0xFA;
    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 3;
    cpu_execute_inst(&cycles, &memory, &cpu);
    // cr_expect(cpu.n == 1, "Le flag n doit être activé");
    // make sure othe flag are not affected
    // cr_expect(cpu.z == cpu_copy.z, "Le flag z ne doit pas être modifié");
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0, "cycles: %d\n", cycles);
}

Test(CPU, CMP_ZP_ACC_SUP_MEMORY)
{
    cpu_reset(&cpu, &memory);
    cpu.pc = 0xC0E0;
    cpu.a = 25;
    // ZP
    memory.data[0x00FB] = 15;

    memory.data[0xC0E0] = CMP_ZP;
    memory.data[0xC0E1] = 0xFB;


    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 3;
    cpu_execute_inst(&cycles, &memory, &cpu);
    // cr_expect(cpu.c == 1, "Le flag c doit être activé");
    // cr_expect(cpu.z == 0, "Le flag z doit être activé");
    cr_expect(cpu.pc == 0xC0E0 + 2, "PC n'est pas incrémenté");
    // make sure othe flag are not affected
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0);
}

Test(CPU, CMP_ZP_ACC_EQ_MEMORY)
{
    cpu_reset(&cpu, &memory);
    cpu.pc = 0xC0E0;
    cpu.a = 25;
    // ZP
    memory.data[0x00FA] = 25;

    memory.data[0xC0E0] = CMP_ZP;
    memory.data[0xC0E1] = 0xFA;

    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 3;
    cpu_execute_inst(&cycles, &memory, &cpu);
    // cr_expect(cpu.c == 1, "Le flag c doit être activé");
    // cr_expect(cpu.z == 1, "Le flag z doit être activé");
    // make sure othe flag are not affected
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0);
}

Test(CPU, CMP_ZERO_PAGE_X_ACC_INF_MEMORY)
{
    cpu_reset(&cpu, &memory);
    cpu.x = 0x0F;
    cpu.a = 13;

    memory.data[0x80 + 0x0F] = 15;

    cpu.pc = 0xC0E0;
    memory.data[0xC0E0] = CMP_ZPX;
    memory.data[0xC0E1] = 0x80;
    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    // cr_expect(cpu.n == 1, "Le flag n doit être activé");
    // make sure othe flag are not affected
    // cr_expect(cpu.z == cpu_copy.z, "Le flag z ne doit pas être modifié");
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0, "cycles: %d\n", cycles);
}

Test(CPU, CMP_ZERO_PAGE_X_ACC_SUP_MEMORY)
{
    cpu_reset(&cpu, &memory);
    cpu.x = 0x0F;
    cpu.a = 18;

    memory.data[0x80 + 0x0F] = 15;

    cpu.pc = 0xC0E0;
    memory.data[0xC0E0] = CMP_ZPX;
    memory.data[0xC0E1] = 0x80;
    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    // cr_expect(cpu.c == 1, "Le flag c doit être activé");
    // cr_expect(cpu.z == 0, "Le flag z doit être activé");
    cr_expect(cpu.pc == 0xC0E0 + 2, "PC n'est pas incrémenté");
    // make sure othe flag are not affected
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0, "cycles: %d\n", cycles);
}


Test(CPU, CMP_ABS_INF_MEMORY)
{
    cpu_reset(&cpu, &memory);
    cpu.a = 15;
    cpu.pc = 0xC0E0;

    memory.data[0xC0E0] = CMP_ABS;
    memory.data[0xC0E1] = 0x80;
    memory.data[0xC0E2] = 0x12;

    memory.data[0x1280] = 25;
    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    // cr_expect(cpu.n == 1, "Le flag n doit être activé");
    // make sure othe flag are not affected
    // cr_expect(cpu.z == cpu_copy.z, "Le flag z ne doit pas être modifié");
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0, "cycles: %d\n", cycles);
}

Test(CPU, CMP_ABS_SUP_MEMORY)
{
    cpu_reset(&cpu, &memory);
    cpu.a = 55;
    cpu.pc = 0xC0E0;

    memory.data[0xC0E0] = CMP_ABS;
    memory.data[0xC0E1] = 0x80;
    memory.data[0xC0E2] = 0x12;

    memory.data[0x1280] = 15;
    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    // cr_expect(cpu.c == 1, "Le flag c doit être activé");
    // cr_expect(cpu.z == 0, "Le flag z doit être activé");
    cr_expect(cpu.pc == 0xC0E0 + 3, "PC n'est pas incrémenté");
    // make sure othe flag are not affected
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0, "cycles: %d\n", cycles);
}

Test(CPU, CMP_ABS_EQ_MEMORY)
{
    cpu_reset(&cpu, &memory);
    cpu.a = 15;
    cpu.pc = 0xC0E0;

    memory.data[0xC0E0] = CMP_ABS;
    memory.data[0xC0E1] = 0x80;
    memory.data[0xC0E2] = 0x12;

    memory.data[0x1280] = 15;
    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    // cr_expect(cpu.c == 1, "Le flag c doit être activé");
    // cr_expect(cpu.z == 1, "Le flag z doit être activé");
    cr_expect(cpu.pc == 0xC0E0 + 3, "PC n'est pas incrémenté");
    // make sure othe flag are not affected
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0, "cycles: %d\n", cycles);
}

Test(CPU, CMP_ABS_X_ACC_SUP_MEMORY_VALUE)
{
    cpu_reset(&cpu, &memory);
    cpu.x = 15;
    cpu.a = 36;
    cpu.pc = 0xC0E0;

    memory.data[0xC0E0] = CMP_ABSX;
    memory.data[0xC0E1] = 0xDF;
    memory.data[0xC0E2] = 0x00;

    memory.data[0x00DF + cpu.x] = 23;

    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);
    // cr_expect(cpu.c == 1, "Le flag c doit être activé");
    // cr_expect(cpu.z == 0, "Le flag z doit être activé");
    // cr_expect(cpu.n == 0, "Le flag n doit être activé");
    // make sure othe flag are not affected
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0, "cycles: %d\n", cycles);
}

Test(CPU, CMP_ABS_X_ACC_INF_MEMORY_VALUE)
{
    cpu_reset(&cpu, &memory);
    cpu.x = 15;
    cpu.a = 6;
    cpu.pc = 0xC0E0;

    memory.data[0xC0E0] = CMP_ABSX;
    memory.data[0xC0E1] = 0xDF;
    memory.data[0xC0E2] = 0x00;

    memory.data[0x00DF + cpu.x] = 23;

    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    // cr_expect(cpu.n == 1, "Le flag n doit être activé");
    // make sure othe flag are not affected
    // cr_expect(cpu.z == cpu_copy.z, "Le flag z ne doit pas être modifié");
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0, "cycles: %d\n", cycles);
}

Test(CPU, CMP_ABS_X_ACC_EQ_MEMORY_VALUE)
{
    cpu_reset(&cpu, &memory);
    cpu.x = 15;
    cpu.a = 23;
    cpu.pc = 0xC0E0;

    memory.data[0xC0E0] = CMP_ABSX;
    memory.data[0xC0E1] = 0xDF;
    memory.data[0xC0E2] = 0x00;

    memory.data[0x00DF + cpu.x] = 23;

    // copy of the cpu
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 4;
    cpu_execute_inst(&cycles, &memory, &cpu);

    // cr_expect(cpu.n == 0, "Le flag n doit être désactivé");
    // cr_expect(cpu.z == 1, "Le flag n doit être activé");
    // cr_expect(cpu.c == 1, "Le flag n doit être activé");
    // make sure othe flag are not affected
    // cr_expect(cpu.i == cpu_copy.i, "Le flag i ne doit pas être modifié");
    // cr_expect(cpu.d == cpu_copy.d, "Le flag d ne doit pas être modifié");
    // cr_expect(cpu.b == cpu_copy.b, "Le flag b ne doit pas être modifié");
    // cr_expect(cpu.v == cpu_copy.v, "Le flag v ne doit pas être modifié");

    cr_assert_eq(cycles, 0, "cycles: %d\n", cycles);
}

Test(CPU, JMP_INDIRECT)
{
    cpu_reset(&cpu, &memory);

    cpu.pc = 0xC000;

    memory.data[0xC000] = INS_JMP_IND; // Opcode pour JMP (Indirect)
    memory.data[0xC001] = 0x00; // LSB de l'adresse du vecteur
    memory.data[0xC002] = 0x80; // MSB de l'adresse du vecteur

    // Adresse indirecte : 0x8000 contient l'adresse cible
    memory.data[0x8000] = 0x34; // LSB de l'adresse cible
    memory.data[0x8001] = 0x12; // MSB de l'adresse cible

    // Sauvegarder l'état initial du CPU
    cpu_6502 cpu_copy;
    memcpy(&cpu_copy, &cpu, sizeof(cpu_6502));

    uint32_t cycles = 5; // 5 cycles pour JMP (Indirect)
    cpu_execute_inst(&cycles, &memory, &cpu);

    // Vérifier que le PC a sauté à l'adresse cible
    cr_assert_eq(cpu.pc, 0x1234, "PC incorrect après JMP (Indirect), attendu: 0x1234, obtenu: 0x%04X", cpu.pc);

    // Vérifier que les cycles ont été utilisés
    cr_assert_eq(cycles, 0, "Cycles incorrects après JMP (Indirect), restant: %d", cycles);

    // Vérifier que les autres registres n'ont pas changé
    cr_expect_eq(cpu.a, cpu_copy.a, "Le registre A ne doit pas être modifié");
    cr_expect_eq(cpu.x, cpu_copy.x, "Le registre X ne doit pas être modifié");
    cr_expect_eq(cpu.y, cpu_copy.y, "Le registre Y ne doit pas être modifié");
}


Test(CPU, JMP_ABS)
{
    cpu_reset(&cpu, &memory); // Reset the CPU and memory
    cpu.pc = 0xC000; // Initial Program Counter set to 0xC000

    // Set up the JMP instruction (0x4C), with a target address of 0x1234
    memory.data[0xC000] = INS_JMP_ABS;  // Opcode for JMP ABSOLUTE
    memory.data[0xC001] = 0x34;         // Low byte of the target address (0x1234)
    memory.data[0xC002] = 0x12;         // High byte of the target address (0x1234)

    // Execute the instruction
    uint32_t cycles = 3; // JMP ABS takes 3 cycles
    cpu_execute_inst(&cycles, &memory, &cpu);

    // Test if the Program Counter (PC) has been updated to the correct address
    cr_expect_eq(cpu.pc, 0x1234, "The PC should jump to the address 0x1234");

    // Check the number of cycles after execution
    cr_assert_eq(cycles, 0, "Expected 0 cycles remaining after execution.");
}

Test(CPU, ADC_ZP)
{
    cpu_reset(&cpu, &memory);  // Reset the CPU and memory
    cpu.pc = 0xC000;  // Set the initial Program Counter to 0xC000

    // Set up the ADC instruction with the zero-page address 0x65 (opcode 0x65)
    memory.data[0xC000] = 0x65;  // Opcode for ADC in Zero Page mode
    memory.data[0xC001] = 0x65;  // Zero-page address 0x65 (target for the addition)

    // Initialize the A register with an arbitrary value, for example, 0x10
    cpu.a = 0x10;  // Initial value of the A register

    // Set the value in memory at address 0x65 to be added to A
    memory.data[0x65] = 0x05;  // The value at address 0x65 is 0x05

    // Execute the ADC instruction
    uint32_t cycles = 3;  // ADC in Zero Page mode takes 3 cycles
    cpu_execute_inst(&cycles, &memory, &cpu);

    // Check if the A register is updated correctly (A = 0x10 + 0x05)
    cr_expect_eq(cpu.a, 0x15, "The A register should be 0x15 after the addition.");

    // Check the number of remaining cycles after execution
    cr_assert_eq(cycles, 0, "The number of remaining cycles should be 0.");
}

Test(CPU, ADC_IMMEDIATE)
{
    cpu_reset(&cpu, &memory); // Reset the CPU and memory to a known state
    cpu.pc = 0xC000;          // Set the initial Program Counter to 0xC000

    // Setup the ADC Immediate instruction (opcode 0x69)
    memory.data[0xC000] = 0x69;  // ADC Immediate opcode
    memory.data[0xC001] = 0x05;  // Operand value: 5

    cpu.a = 0x03;  // Initialize the accumulator with the value 3
    cpu.p = 0x00;  // Clear the status flags (no carry, no overflow)

    uint32_t cycles = 2; // ADC Immediate takes 2 cycles
    cpu_execute_inst(&cycles, &memory, &cpu);

    // Verify that the accumulator holds the correct result
    cr_expect_eq(cpu.a, 0x08, "ADC Immediate: Expected accumulator to be 0x08, but got 0x%02X", cpu.a);

    // Verify that the carry flag is not set
    cr_expect_eq(cpu.p & 0x01, 0x00, "ADC Immediate: Carry flag should not be set.");

    // Verify that the overflow flag is not set
    cr_expect_eq(cpu.p & 0x40, 0x00, "ADC Immediate: Overflow flag should not be set.");

    // Verify that the number of cycles is exhausted
    cr_assert_eq(cycles, 0, "Expected 0 cycles remaining after execution.");
}
