#ifndef __cpu_6502__H
#define __cpu_6502__H

#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * CPU Instruction set
 * /
 **/ 
// LDA
#define INS_LDA_IM      0xA9
#define INS_LDA_ZP      0xA5
#define INS_LDA_ZPX     0xB5
#define INS_LDA_ABS     0xAD
#define INS_LDA_ABSX    0xBD
#define INS_LDA_ABSY    0xB9
#define INS_LDA_INDX    0xA1
#define INS_LDA_INDY    0xB1
#define INS_JSR         0x20  
// LDX
#define INS_LDX_IM      0xA2
#define INS_LDX_ZP      0xA6
#define INS_LDX_ZPY     0xB6
#define INS_LDX_ABS     0xAE
#define INS_LDX_ABSY    0xBE


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
    byte b : 1;
    byte c : 1;
    byte n : 1;
    byte v : 1;
    byte z : 1;
    byte d : 1;
    byte i : 1;

} cpu_6502 ;

/**
 * Affiche l'etat actuel du cpu
 * Affiche le contenue des registres
*/
void
cpu_dump(const cpu_6502 *cpu);

/**
 * @param mem_6502
 * Permet d'initialiser la mémoire
*/
void
mem_initialise(mem_6502 *mem);

/**
 * @param cpu_6502
 * @param mem_6502
 * Permet reset le cpu
 * Reset le cpu, c'est mettre tout les registres
 * à leurs états de départ
*/
void
cpu_reset(cpu_6502 * cpu, mem_6502 *mem);

/**
 * LDA_set_status
 * permet de mettre des status apres instructions
 * pour certaines instructions les status sont pareil
 * donc on factorise
*/
void
set_LDA_status(cpu_6502 *cpu);

/**
 * Permet d'afficher le contenue da la mémoire en hexa
 * Parcour toute la mémoire et affiche octet par octet
 * @param mem_6502
*/
void
HexDump(const void* data, size_t size);

void
hexDump(char *desc, void *addr, int len);

/**
 * Recupere un byte dans la mémoire
 * @param uint32_t cycles : nombre de cycle pour le fetch
 * @param mem_6502
 * @param cpu_6502
 * @return byte
*/
byte
cpu_fetch_byte(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu);

byte
cpu_read_byte_from_zp_adress(uint32_t *cycles, byte src, mem_6502 *memory, cpu_6502 *cpu);

byte
cpu_read_byte_from_word_adress(uint32_t *cycles, word src, mem_6502 *memory, cpu_6502 *cpu);

word
cpu_read_word_from_adress(uint32_t *cycles, word src, mem_6502 *memory, cpu_6502 *cpu);

word
cpu_fetch_word(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu);

/**
 * Write 2 bytes
*/
void
cpu_write_word_at(uint32_t *cycles, word data, uint32_t dst, mem_6502 *memory, cpu_6502 *cpu);

/**
 * Permet l'execution d'un instruction qui sera
 * lu dans la mémoire
 * @param cycles
 * @param memory
 * @param cpu
*/
uint32_t
cpu_execute_inst(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu);

#endif