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

/**
 *************** JUMPS *******************

 * > JSR : Jump To New Location Saving Return Address
 *          The address before the next instruction (PC  - 1) is pushed onto the stack.
 *          first the upper byte followed by the lower byte. As the stack grows backwards,
 *          The return address is therefore stored as a little-endian number in memory.
 *          PC is set to the targctet address.
 * 
 * > When you call a subroutine using JSR, two things happen:
 *      - The address of the last byte of the JSR (That is the next instruction - 1) is pushed
 *        onto the stack
 *      - The programm counter jumps to the subroutine indicated.
 *          
 * > When the program encouters an RTS instruction, this happens:
 *      - An address is popped off the stack
 *      - The programm counter jumps to this address + 1
 * 
 * Exemple:
 *       - source : https://www.nesdev.org/wiki/RTS_Trick
 *      $C0E0: 20 00 80     JSR $8000
 *      $C0E3: A2 00        LDX #$00
 *      $C0E5: A0           LDY #$00
 * 
 *      $8000: A9 0F        LDA #$0F
 *      $8002: 8D 15 40     STA $4015
 *      $8005: 60               
 *      
*/
#define INS_JSR         0x20 // JUMP TO SUBROUTINE

#define INS_JMP_ABS     0x4C // JUMP ABSOLUTE
#define INS_JMP_IND     0x6C 

// RTS
#define INS_RTS_IMP     0x60

/**
 * Compare instructions
*/
#define CMP_IM          0xC9
#define CMP_ZP          0xC5
#define CMP_ZPX         0xD5
#define CMP_ABS         0xCD
#define CMP_ABSX        0xDD

// ADD
#define INS_ADC_ZP      0x65
#define INS_ADC_IM      0x69

#define MEM_SIZE 65536 // 64KB memory size

typedef uint8_t     byte;
typedef uint16_t    word;

/**
 * 
 * CPU FLAGS 
 */
#define SET_FLAG(cpu, flag)   ((cpu)->p |= (flag))
#define CLEAR_FLAG(cpu, flag) ((cpu)->p &= ~(flag))

// Macro to test if a flag is set
#define IS_FLAG_SET(cpu, flag) (((cpu)->p & (flag)) != 0)

// Flag definitions
// Bit masks for the 6502 status register (P)
#define FLAG_CARRY    0x01  // Carry flag (C) (bit 0)
#define FLAG_ZERO     0x02  // Zero flag (Z) (bit 1)
#define FLAG_INTERRUPT 0x04 // Interrupt disable flag (I) (bit 2)
#define FLAG_DECIMAL  0x08  // Decimal mode flag (D) (bit 3)
#define FLAG_BREAK    0x10  // Break command flag (B) (bit 4, virtual)
#define FLAG_UNUSED   0x20  // Unused bit 
#define FLAG_OVERFLOW 0x40  // Overflow flag (V) (bit 6)
#define FLAG_NEGATIVE 0x80  // Negative flag (N) (bit 7)


/**
 * mem_6502
 * Define memory 
 * Source : https://en.wikibooks.org/wiki/6502_Assembly
 * zero page : $0000 - $00FF
 *          The first of memory, which is faster to access than other pages;
 *          Instructions can specify addresses within the zero page with a sigle byte
 *          as opposed to two, so instructions that use the zero page instead of any other
 *          page require one less CPU cycles to execute 
 * 
 * stack : $0100 - $01FF 
 *          Last-in first-out data structure. Grows backwards from $01FF to $0100.
 *          Used by some transfer, stack and subroutine instructions.
 * 
 * General-purpose : $0200 - $FFFF 
 *          Memory that can be used for whatever purpose needed.
 *          Devices that use the 6502 processor may choose to reserve sub-regions for
 *          other purposes, such as memory-mapped I/0.
*/
typedef struct
{
    word begin_stack;
    word end_stack;
    byte data[MEM_SIZE];
} mem_6502;


typedef struct
{
    word pc;  // Program Counter
    word sp;  // Stack Pointer
    byte a;   // Accumulator
    byte x;   // Index Register X
    byte y;   // Index Register Y

    byte p;   // Status register (flags: N, V, -, B, D, I, Z, C)
} cpu_6502;


void
cpu_dump(const cpu_6502 *cpu);

/**
 * @param mem_6502
*/
void
mem_initialise(mem_6502 *mem);

/**
 * Reset the CPU and set all flags to 0
 * @param cpu_6502
 * @param mem_6502
*/
void
cpu_reset(cpu_6502 * cpu, mem_6502 *mem);

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
cpu_read_byte_from_zp_address(uint32_t *cycles, byte src, mem_6502 *memory, cpu_6502 *cpu);

byte
cpu_read_byte_from_word_address(uint32_t *cycles, word src, mem_6502 *memory, cpu_6502 *cpu);

word
cpu_read_word_from_address(uint32_t *cycles, word src, mem_6502 *memory, cpu_6502 *cpu);

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

void
cpu_push_byte_on_stack(uint32_t *cycles, byte data, mem_6502 *memory, cpu_6502 *cpu);

void
cpu_push_word_on_stack(uint32_t *cycles, word data, mem_6502 *memory, cpu_6502 *cpu);

byte
cpu_pop_byte_off_stack(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu);

word
cpu_pop_word_off_stack(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu);

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
