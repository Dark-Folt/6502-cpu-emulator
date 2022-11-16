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
    // $C0E0: 20 00 80     JSR $8000
    // $C0E3: A2 00        LDX #$00
    // $C0E5: A0 00        LDY #$00

    // $8000: A9 0F        LDA #$0F
    // $8002: 8D 15 40     STA $4015
    // $8005: 60           RTS

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

Test(CPU, TAX)
{
    cpu_reset(&cpu, &memory);
    uint32_t cycles = 2;
    cpu.x = 0;
    cpu.a = 0x76;
    memory.data[0xFFFC] = INS_TAX_IMP;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.x == 0x76);
    cr_assert_eq(cycles, 0);
}


Test(CPU, TAY)
{
    cpu_reset(&cpu, &memory);
    uint32_t cycles = 2;
    cpu.y = 0;
    cpu.a = 0x76;
    memory.data[0xFFFC] = INS_TAY_IMP;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.y == 0x76);
    cr_assert_eq(cycles, 0);
}


Test(CPU, TSX)
{
    cpu_reset(&cpu, &memory);
    uint32_t cycles = 2 + 1;
    cpu_push_byte_on_stack(&cycles, 0x12, &memory, &cpu);
    cpu.x = 0x0;
    memory.data[0xFFFC] = INS_TSX_IMP;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.x == 0x12);
    cr_assert_eq(cycles, 0);
}

Test(CPU, TXA)
{
    cpu_reset(&cpu, &memory);
    uint32_t cycles = 2;
    cpu.a = 0x0;
    cpu.x = 0x88;
    memory.data[0xFFFC] = INS_TXA_IMP;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.a == 0x88);
    cr_assert_eq(cycles, 0);
}

/**
 * Substract with carry
*/
Test(CPU, SBC_IM_C_FLAG_N_FLAG_NOT_SET)
{
    cpu_reset(&cpu, &memory);
    cpu.a = 0xa;
    cpu.pc = 0xFFFC;
    uint32_t cycles = 2;
    memory.data[0xFFFC] = INS_SBC_IM;
    memory.data[0xFFFD] = 0xa;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.a == 0x0);
    cr_expect(cpu.z == 0x1);
    cr_assert_eq(cycles, 0);
}

Test(CPU, SBC_ZP_C_FLAG_N_FLAG_IS_SET)
{
    cpu_reset(&cpu, &memory);
    cpu.a = 0x23;
    cpu.pc = 0xFFFC;
    uint32_t cycles = 3;
    memory.data[0xFFFC] = INS_SBC_ZP;
    memory.data[0xFFFD] = 0x60;
    memory.data[0x0060] = 0x13;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.a == 0x10);
    cr_assert_eq(cycles, 0);
}

Test(CPU, SBC_ZP_X)
{
    cpu_reset(&cpu, &memory);
    cpu.a = 0x23;
    cpu.x = 0x6D;
    cpu.pc = 0xFFFC;
    uint32_t cycles = 4;
    memory.data[0xFFFC] = INS_SBC_ZPX;
    memory.data[0xFFFD] = 0x60;
    memory.data[0x60 + 0x6D] = 0x13;
    cpu_execute_inst(&cycles, &memory, &cpu);
    cr_expect(cpu.a == 0x10);
    cr_assert_eq(cycles, 0);
}
