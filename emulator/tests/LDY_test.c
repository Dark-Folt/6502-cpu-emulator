#include <criterion/criterion.h>
#include <string.h>
#include "../include/cpu_6502.h"

mem_6502 memory_3;
cpu_6502 cpu_3;

Test(CPU, load_y_register_immediate_address)
{
    cpu_reset(&cpu_3, &memory_3);
    memory_3.data[0xFFFC] = INS_LDY_IM;
    memory_3.data[0xFFFD] = 0x02; 
    memory_3.data[0xFFFE] = 0xB3; 

    uint32_t  cycles = 2;
    cpu_execute_inst(&cycles, &memory_3, &cpu_3);
    cr_expect(cpu_3.y == 0xB3);
    cr_assert_eq(cycles, 0);
}

Test(CPU, load_y_register_zero_page)
{
    cpu_reset(&cpu_3, &memory_3);
    memory_3.data[0xFFFC] = INS_LDY_ZP;
    memory_3.data[0xFFFD] = 0x02; 
    memory_3.data[0x0002] = 0xB3; 

    uint32_t  cycles = 3;
    cpu_execute_inst(&cycles, &memory_3, &cpu_3);
    cr_expect(cpu_3.y == 0xB3);
    cr_assert_eq(cycles, 0);
}

Test(CPU, load_y_register_zero_page_x)
{
    cpu_reset(&cpu_3, &memory_3);
    memory_3.data[0xFFFC] = INS_LDY_ZPX;
    memory_3.data[0xFFFD] = 0x02; 
    memory_3.data[0x0002] = 0xB3; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory_3, &cpu_3);
    cr_expect(cpu_3.y == 0xB3);
    cr_assert_eq(cycles, 0);
}

Test(CPU, load_y_register_abs)
{
    cpu_reset(&cpu_3, &memory_3);
    memory_3.data[0xFFFC] = INS_LDY_ABS;
    memory_3.data[0xFFFD] = 0x02; 
    memory_3.data[0x0002] = 0xB3; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory_3, &cpu_3);
    cr_expect(cpu_3.y == 0xB3);
    cr_assert_eq(cycles, 0);
}

Test(CPU, load_y_register_abs_x)
{
    cpu_reset(&cpu_3, &memory_3);
    cpu_3.x = 0x36;
    memory_3.data[0xFFFC] = INS_LDY_ABSX;
    memory_3.data[0xFFFD] = 0x02; 
    memory_3.data[0xFFFE] = 0xB3; 
    memory_3.data[0xB338] = 0x87; 

    uint32_t  cycles = 4;
    cpu_execute_inst(&cycles, &memory_3, &cpu_3);
    cr_expect(cpu_3.y == 0x87);
    cr_assert_eq(cycles, 0);
}

Test(CPU, load_y_register_abs_x_page_cross)
{
    cpu_reset(&cpu_3, &memory_3);
    cpu_3.x = 0xFF;
    memory_3.data[0xFFFC] = INS_LDY_ABSX;
    memory_3.data[0xFFFD] = 0x80; 
    memory_3.data[0xFFFE] = 0x90; 
    memory_3.data[0x917F] = 0x87; 

    uint32_t  cycles = 5;
    cpu_execute_inst(&cycles, &memory_3, &cpu_3);
    cr_expect(cpu_3.y == 0x87);
    cr_assert_eq(cycles, 0);
}


