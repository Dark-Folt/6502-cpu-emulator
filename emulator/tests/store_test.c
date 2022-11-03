#include <criterion/criterion.h>
#include <string.h>
#include "../include/cpu_6502.h"

mem_6502 memory_2;
cpu_6502 cpu_2;

void setup_memory_2(void)
{
    mem_initialise(&memory_2);
}

void setup_cpu_2(void)
{
    cpu_reset(&cpu_2, &memory_2);
}

TestSuite(LOAD_MEMORY, .init=setup_memory_2);

Test(LOAD_MEMORY, init_memory) {

    byte exp_data[MEM_SIZE] = {0};

    cr_expect_arr_eq(memory_2.data, exp_data, sizeof(byte), "Chaque valeur de la memoire doit être égale à 0 apres initialisation");
} 

/**CPU test*/
TestSuite(CPU_2, .init=setup_cpu_2);

Test(CPU_2, init_cpu)
{
    cr_assert(
        cpu_2.a == cpu_2.x == cpu_2.y == 0
    , "Le registre à 0");
}

Test(CPU_2, STA_ZP)
{
    cpu_2.a = 0x12;
    memory_2.data[0xFFFC] = INS_STA_ZP;
    memory_2.data[0xFFFD] = 0x84; 

    uint32_t  cycles = 3;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);

    cr_expect(memory_2.data[0x0084] == 0x12);
    cr_expect(cycles == 0, "le nombre de cycle doit être égale à 0");
}

Test(CPU_2, STA_ZPX)
{
    cpu_2.a = 0x12;
    cpu_2.x = 0x26;
    memory_2.data[0xFFFC] = INS_STA_ZPX;
    memory_2.data[0xFFFD] = 0x84; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);

    cr_expect(memory_2.data[0x00AA] == 0x12);
    cr_expect(cycles == 0);
}

Test(CPU_2, STA_ZPX_wrong_cycles)
{
    cpu_2.a = 0x12;
    cpu_2.x = 0x26;
    memory_2.data[0xFFFC] = INS_STA_ZPX;
    memory_2.data[0xFFFD] = 0x84; 

    uint32_t  cycles = 5;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);

    cr_expect(memory_2.data[0x00AA] == 0x12);
    cr_expect(cycles == 1);
}

Test(CPU_2, STA_ABS)
{
    cpu_2.a = 0x12;
    memory_2.data[0xFFFC] = INS_STA_ABS;
    memory_2.data[0xFFFD] = 0x92; 
    memory_2.data[0xFFFE] = 0x93; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);

    cr_expect(memory_2.data[0x9392] == 0x12);
    cr_expect(cycles == 0);
}

Test(CPU_2, STA_ABSX)
{
    cpu_2.a = 0x12;
    cpu_2.x = 0x44;
    memory_2.data[0xFFFC] = INS_STA_ABSX;
    memory_2.data[0xFFFD] = 0x92; 
    memory_2.data[0xFFFE] = 0x93; 

    uint32_t  cycles = 5;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);

    cr_expect(memory_2.data[0x93D6] == 0x12);
    cr_expect(cycles == 0);
}

Test(CPU_2, STA_ABSY)
{
    cpu_2.a = 0x12;
    cpu_2.y = 0x44;
    memory_2.data[0xFFFC] = INS_STA_ABSY;
    memory_2.data[0xFFFD] = 0x92; 
    memory_2.data[0xFFFE] = 0x93; 

    uint32_t  cycles = 5;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);

    cr_expect(memory_2.data[0x93D6] == 0x12);
    cr_expect(cycles == 0);
}

Test(CPU_2, STA_INDX)
{
    cpu_2.a = 0x99;
    cpu_2.x = 0x44;
    memory_2.data[0xFFFC] = INS_STA_INDX;
    memory_2.data[0xFFFD] = 0x92; 
    memory_2.data[0x00D6] = 0x24; 
    memory_2.data[0x00D7] = 0x77; 

    uint32_t  cycles = 6;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);

    cr_expect(memory_2.data[0x7724] == 0x99);
    cr_expect(cycles == 0);
}

Test(CPU_2, STA_INDY)
{
    cpu_2.a = 0x99;
    cpu_2.y = 0x44;
    memory_2.data[0xFFFC] = INS_STA_INDY;
    memory_2.data[0xFFFD] = 0x92; 
    memory_2.data[0x00D6] = 0x24; 
    memory_2.data[0x00D7] = 0x77; 

    uint32_t  cycles = 6;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);

    cr_expect(memory_2.data[0x7724] == 0x99);
    cr_expect(cycles == 0);
}
