#include <stdio.h>
#include <stdint.h> 


typedef uint8_t     byte;
typedef uint16_t    word;
typedef uint32_t    qword;



#define INS_LDA_IM  0xA9

#define MEM_SIZE  1024 * 64
typedef struct
{
    byte data[MEM_SIZE];

} mem_6502;

typedef struct
{
    // programm counter
    word pc;
    // stack pointer
    word sp;
    // 8 bits registers
    byte a; byte x; byte y;
    // flags
    byte p : 1;
    byte c : 1;
    byte n : 1;
    byte v : 1;
    byte z : 1;
    byte d : 1;
    byte i : 1;

    // void (*reset)(struct cpu_6502*)
} cpu_6502 ;

void
mem_initialise(mem_6502 *mem)
{
    size_t i;
    for(i=0; i < MEM_SIZE; i++) {
        mem->data[i] = 0;
    }
}

void
cpu_reset(cpu_6502 * cpu, mem_6502 *mem)
{
    cpu->pc = 0xFFFC;
    cpu->sp = 0x0100;
    // registre à 0
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    // flags
    cpu->p = 0;
    cpu->c = 0;
    cpu->n = 0;
    cpu->v = 0;
    cpu->z = 0;
    cpu->d = 0;
    cpu->i = 0;

    // initialisation de la memoire
    mem_initialise(mem);
}

byte
cpu_fectch_byte(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu)
{
    byte data = memory->data[cpu->pc];
    cpu->pc++;
    cycles--;

    return data;
}


void
cpu_execute_inst(uint32_t cycles, mem_6502 *memory, cpu_6502 *cpu)
{
    while (cycles > 0)
    {
        byte inst = cpu_fectch_byte(&cycles, memory, cpu);
        switch (inst)
        {
        case INS_LDA_IM:
        {
            byte value = cpu_fectch_byte(&cycles, memory, cpu);
            cpu->a = value;
            cpu->z = (cpu->a == 0);
            cpu->n = (cpu->a & 0b1000000) > 0;
        } break;
        
        default:
            printf("Instruction not handled %d\n", inst);
            break;
        }
        break;
    }
    
}

void
mem_dump(mem_6502 *mem)
{
    size_t i;
    for(i=0; i < MEM_SIZE; i++) {
        printf("%02X", mem->data[i]);
    }
    printf("\n");
}


int main()
{
    mem_6502 mem;
    cpu_6502 cpu;
    cpu_reset(&cpu, &mem);
    mem.data[0xFFFC] = INS_LDA_IM;
    mem.data[0xFFFD] = 0x42;
    cpu_execute_inst(2, &mem, &cpu);
    // int a = 1;
    // size_t o = cpu_fectch_byte(&a, &mem, &cpu);

    // mem_dump(&mem);
    // printf("%c\n", mem.data[0]);

    return 0;
}