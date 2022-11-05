#include <criterion/criterion.h>
#include <string.h>
#include "../include/cpu_6502.h"

mem_6502 memory_3;
cpu_6502 cpu_3;

void setup_memory_3(void)
{
    mem_initialise(&memory_3);
}

void setup_cpu_3(void)
{
    cpu_reset(&cpu_3, &memory_3);
}

TestSuite(LOAD_MEMORY_3, .init=setup_memory_3);

Test(LOAD_MEMORY_3, init_memory) {

    byte exp_data[MEM_SIZE] = {0};

    cr_expect_arr_eq(memory_3.data, exp_data, sizeof(byte), "Chaque valeur de la memoire doit être égale à 0 apres initialisation");
} 

/**CPU test*/
TestSuite(CPU_3, .init=setup_cpu_3);

Test(CPU_3, init_cpu)
{
    cr_assert(
        cpu_3.a == cpu_3.x == cpu_3.y == 0
    , "Le registre à 0");
}

Test(CPU_3, STACK_PUSH_BYTE)
{
    uint32_t cycles = 5;
    cpu_push_byte_on_stack(&cycles, 0x80, &memory_3, &cpu_3);
    cpu_push_byte_on_stack(&cycles, 0x70, &memory_3, &cpu_3);
    cr_expect(memory_3.data[cpu_3.sp + 1] == 0x80);
    cr_expect(memory_3.data[cpu_3.sp] == 0x70);
}

// Test(CPU_3, STACK_PUSH_WORD)
// {
//     uint32_t cycles = 5;
//     cpu_push_word_on_stack(&cycles, 0x8024, &memory_3, &cpu_3);
//     cr_expect(memory_3.data[cpu_3.sp + 1] == 0x80);
//     cr_expect(memory_3.data[cpu_3.sp] == 0x24);
// }

// Test(CPU_3, JSR)
// {
//     cpu_3.pc = 0xC0E0;
//     memory_3.data[0xC0E0] = INS_JSR;
//     memory_3.data[0xC0E1] = 0x00;
//     memory_3.data[0xC0E2] = 0x80;
//     memory_3.data[0xC0E3] = INS_LDX_IM;
//     memory_3.data[0xC0E4] = 0x70;

//     uint32_t cycles = 6 + 2;
//     cpu_execute_inst(&cycles, &memory_3, &cpu_3);
//     cr_expect(cpu_3.x == 0x80);
//     printf("pc: %x\n",cpu_3.pc);
//     printf("cycles: %d\n",cycles);
//     printf("x: %x\n",cpu_3.x);
//     cr_assert_eq(cycles, 0);
// }
