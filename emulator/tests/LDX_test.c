#include <criterion/criterion.h>
#include <string.h>
#include "../include/cpu_6502.h"

mem_6502 memory_2;
cpu_6502 cpu_2;

Test(CPU, load_x_register_immediate_address)
{
    cpu_reset(&cpu_2, &memory_2);
    memory_2.data[0xFFFC] = INS_LDX_IM;
    memory_2.data[0xFFFD] = 0x02; 

    uint32_t  cycles = 2;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);
    cr_expect(cpu_2.x == 0x02);
    cr_assert_eq(cycles, 0);
}

Test(CPU, load_x_register_zero_page)
{
    cpu_reset(&cpu_2, &memory_2);
    memory_2.data[0xFFFC] = INS_LDX_ZP;
    memory_2.data[0xFFFD] = 0x02; 
    memory_2.data[0x0002] = 0x44; 

    uint32_t  cycles = 3;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);
    cr_expect(cpu_2.x == 0x44);
    cr_assert_eq(cycles, 0);
}

Test(CPU, load_x_register_zp_y)
{
    cpu_reset(&cpu_2, &memory_2);
    cpu_2.y = 0x04;
    memory_2.data[0xFFFC] = INS_LDX_ZPY;
    memory_2.data[0xFFFD] = 0x08;
    memory_2.data[0x000C] = 0x88;

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);
    cr_expect(cpu_2.x == 0x88);
    cr_assert_eq(cycles, 0);
}

Test(CPU, load_x_regiser_abs)
{
    cpu_reset(&cpu_2, &memory_2);
    memory_2.data[0xFFFC] = INS_LDX_ABS;
    memory_2.data[0xFFFD] = 0x08;
    memory_2.data[0xFFFE] = 0x80;
    memory_2.data[0x8008] = 0xBE;

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);
    cr_expect(cpu_2.x == 0xBE);
    cr_assert_eq(cycles, 0);
}

Test(CPU, load_x_register_abs_y)
{
    cpu_reset(&cpu_2, &memory_2);
    cpu_2.y = 0x24;
    memory_2.data[0xFFFC] = INS_LDX_ABSY;
    memory_2.data[0xFFFD] = 0x08;
    memory_2.data[0xFFFE] = 0x80;
    memory_2.data[0x802C] = 0xA1;

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);
    cr_expect(cpu_2.x == 0xA1);
    cr_assert_eq(cycles, 0);
}

Test(CPU, load_x_register_abs_y_page_crossed)
{
    cpu_reset(&cpu_2, &memory_2);
    cpu_2.y = 0xFF;
    memory_2.data[0xFFFC] = INS_LDX_ABSY;
    memory_2.data[0xFFFD] = 0x08;
    memory_2.data[0xFFFE] = 0x80;
    memory_2.data[0x8107] = 0xA4;

    uint32_t  cycles = 5;
    cpu_execute_inst(&cycles, &memory_2, &cpu_2);
    cr_expect(cpu_2.x == 0xA4);
    cr_assert_eq(cycles, 0);
}
