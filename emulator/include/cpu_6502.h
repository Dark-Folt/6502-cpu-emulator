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
// LDY
#define INS_LDY_IM      0xA0
#define INS_LDY_ZP      0xA4
#define INS_LDY_ZPX     0xB4
#define INS_LDY_ABS     0xAC
#define INS_LDY_ABSX    0xBC
// STA 
#define INS_STA_ZP      0x85
#define INS_STA_ZPX     0x95
#define INS_STA_ABS     0x8D
#define INS_STA_ABSX    0x9D
#define INS_STA_ABSY    0x99
#define INS_STA_INDX    0x81
#define INS_STA_INDY    0x91
// STX
#define INS_STX_ZP      0x86
#define INS_STX_ZPY     0x96
#define INS_STX_ABS     0x8E
// STX
#define INS_STY_ZP      0x84
#define INS_STY_ZPX     0x94
#define INS_STY_ABS     0x8C

#define MEM_SIZE  1024 * 64

typedef uint8_t     byte;
typedef uint16_t    word;


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
 * Recupere un byte de poids faible dans la mémoire (LSB)
 * @param uint32_t cycles : nombre de cycle pour le fetch
 * @param mem_6502
 * @param cpu_6502
 * @return byte
*/
byte
cpu_fetch_lsb(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu);

/**
 * Recupere un byte de poids fort dans la mémoire (MSB)
 * @param uint32_t cycles : nombre de cycle pour le fetch
 * @param mem_6502
 * @param cpu_6502
 * @return byte
*/
byte
cpu_fetch_msb(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu);

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

void
cpu_write_byte_at_zp_addr(uint32_t *cycles, byte zp_addr, byte value, mem_6502 *memory, cpu_6502 *cpu);

void
cpu_write_byte_at_word_addr(uint32_t *cycles, word zp_addr, byte value, mem_6502 *memory, cpu_6502 *cpu);

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
