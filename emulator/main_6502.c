#include <stdio.h>
#include <stdint.h> 
#include <stdlib.h>



// Instruction set
#define INS_LDA_IM      0xA9
#define INS_LDA_ZP      0xA5
#define INS_LDA_ZPX     0xB5

#define MEM_SIZE  1024 * 64

typedef uint8_t     byte;
typedef uint16_t    word;
typedef uint32_t    qword;

/**
 * mem_6502
 * Dénie la mémoire
*/
typedef struct
{
    byte data[MEM_SIZE];

} mem_6502;

/**
 * cpu_6502
 * Définie un le cpu 6502
 * avec les registres suivants: p, n, c, v, z, i
 * program counter   : pc
 * stack pointer    : sp
 * registre géneraux : a, x, y 
*/
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

} cpu_6502 ;

void
cpu_dump(const cpu_6502 *cpu)
{
    printf("/****** CPU DUMP******\n");
    printf("pc: %04x\n",cpu->pc);
    printf("sp: %04x\n",cpu->sp);
    printf("ra: %04x\n",cpu->a);
    printf("rx: %04x\n",cpu->x);
    printf("ry: %04x\n",cpu->y);
}

/**
 * @param mem_6502
 * Permet d'initialiser la mémoire
*/
void
mem_initialise(mem_6502 *mem)
{
    size_t i;
    for(i=0; i < MEM_SIZE; i++) {
        mem->data[i] = 0;
    }
}

/**
 * @param cpu_6502
 * @param mem_6502
 * Permet reset le cpu
 * Reset le cpu, c'est mettre tout les registres
 * à leurs états de départ
*/
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

/**
 * LDA_set_status
 * permet de mettre des status apres instructions
 * pour certaines instructions les status sont pareil
 * donc on factorise
*/
void
set_LDA_status(cpu_6502 *cpu)
{
    cpu->z = (cpu->a == 0);
    cpu->n = (cpu->a & 0b1000000) > 0;
}

/**
 * Recupere un byte dans la mémoire
 * @param uint32_t cycles : nombre de cycle pour le fetch
 * @param mem_6502
 * @param cpu_6502
 * @return byte
*/
byte
cpu_fectch_byte(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu)
{
    byte data = memory->data[cpu->pc];
    cpu->pc++;
    cycles--;

    return data;
}

byte
cpu_read_byte_from_adress(uint32_t *cycles, byte src,mem_6502 *memory, cpu_6502 *cpu)
{
    byte data = memory->data[src];
    cycles--;
    return data;
}

/**
 * Permet l'execution d'un instruction qui sera
 * lu dans la mémoire
 * @param cycles
 * @param memory
 * @param cpu
*/
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
            set_LDA_status(cpu);
        } break;
        case INS_LDA_ZP:
        {
            byte zp_addr = cpu_fectch_byte(&cycles, memory, cpu);
            cpu->a = cpu_read_byte_from_adress(&cycles, zp_addr, memory, cpu);
            set_LDA_status(cpu);
        } break;
        case INS_LDA_ZPX:
        {
            byte zp_addr = cpu_fectch_byte(&cycles, memory, cpu);
            zp_addr += cpu->x;
            cycles --;

            cpu->a = cpu_read_byte_from_adress(&cycles, zp_addr, memory, cpu);
            set_LDA_status(cpu);
        } break;
        default:
            printf("Instruction not handled %d\n", inst);
            break;
        }
        break;
    }
    
}

/**
 * Permet d'afficher le contenue da la mémoire en hexa
 * Parcour toute la mémoire et affiche octet par octet
 * @param mem_6502
*/
void HexDump(const void* data, size_t size) 
{
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        printf("%02X ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i+1) % 8 == 0 || i+1 == size) {
            printf(" ");
            if ((i+1) % 16 == 0) {
                printf("|  %s \n", ascii);
            } else if (i+1 == size) {
                ascii[(i+1) % 16] = '\0';
                if ((i+1) % 16 <= 8) {
                    printf(" ");
                }
                for (j = (i+1) % 16; j < 16; ++j) {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}



int main()
{
    mem_6502 mem;
    cpu_6502 cpu;
    cpu_reset(&cpu, &mem);
    mem.data[0xFFFC] = INS_LDA_ZP;
    mem.data[0xFFFD] = 0x42;
    mem.data[0x0042] = 0x84;

    // mem_dump(&mem);
    // HexDump(mem.data, MEM_SIZE);
    // cpu_dump(&cpu);
    // cpu_execute_inst(3, &mem, &cpu);
    // HexDump(mem.data, MEM_SIZE);
    // cpu_dump(&cpu);
    // int a = 1;ap_helptext
    // size_t o = cpu_fectch_byte(&a, &mem, &cpu);

    // mem_dump(&mem);
    // printf("%c\n", mem.data[0]);

    return 0;
}