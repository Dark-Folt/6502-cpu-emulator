#include "../include/cpu_6502.h"

#include <stdio.h>
#include <stdint.h>

#define PROGRAM_1_START 0x0000

#define PROGRAM_1_SIZE 6

mem_6502 mem;
cpu_6502 cpu;

// Load the programs into memory
void load_programs()
{
    uint8_t program_1[] = {
      0xA9, 0x03,   // LDA #$03       ; Load 3 into accumulator (A = 3)
      0x85, 0x10,   // STA $10        ; Store 3 into address $10
      0xA9, 0x05,   // LDA #$05       ; Load 5 into accumulator (A = 5)
      0x65, 0x10,   // ADC $10        ; Add the value at address $10 (which is 3) to A
      0x85, 0x20    // STA $20        ; Store the result (A = 8) into address $20
    };

    // Load program 1 into memory
    for (int i = 0; i < sizeof(program_1); i++) {
        mem.data[PROGRAM_1_START + i] = program_1[i];
    }
}

// Function to print memory contents
void print_memory(word start, word end)
{
    for (word i = start; i < end; i++) {
        printf("0x%04X: 0x%02X\n", i, mem.data[i]);
    }
}

// Function to execute a program
void run_program(cpu_6502 *cpu, mem_6502 *mem, word start_address, int steps)
{
    cpu->pc = start_address; // Set the program counter to the start address
    printf("\nExecuting program starting at 0x%04X...\n", start_address);
    uint32_t a = steps;

    for (int i = 0; i < steps; i++) {
      cpu_execute_inst(&a, mem, cpu);
    }
}

int main()
{
    // Initialize the CPU and memory
    cpu_reset(&cpu, &mem);

    // Load the programs into memory
    load_programs();

    // Execute each program
    printf("=== Program 1 ===\n");
    run_program(&cpu, &mem, PROGRAM_1_START, 13);
    printf("Res: 0x%02X\n", mem.data[0x0020]);
    cpu_dump(&cpu);

    return 0;
}
