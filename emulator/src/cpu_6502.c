#include "../include/cpu_6502.h"

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
    mem->begin_stack    = 0x01FF;
    mem->end_stack      = 0x0100;
    // init data by 0
    memset(mem->data, 0, MEM_SIZE);
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
    // initialisation de la memoire
    mem_initialise(mem);


    cpu->pc = 0xFFFC;
    cpu->sp = mem->begin_stack + 1;
    // registre à 0
    cpu->a = 0;
    cpu->x = 0;
    cpu->y = 0;
    // flags
    cpu->b = 0;
    cpu->c = 0;
    cpu->n = 0;
    cpu->v = 0;
    cpu->z = 0;
    cpu->d = 0;
    cpu->i = 0;
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

void
hexDump(char *desc, void *addr, int len) 
{
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf("  %s\n", buff);

            // Output the offset.
            printf("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf(" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
            buff[i % 16] = '.';
        } else {
            buff[i % 16] = pc[i];
        }

        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf("  %s\n", buff);
}


/**
 * Recupere le byte de poids faible dans la mémoire
 * @param uint32_t cycles : nombre de cycle pour le fetch
 * @param mem_6502
 * @param cpu_6502
 * @return byte
*/
byte
cpu_fetch_lsb(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu)
{
    byte data = memory->data[cpu->pc];
    cpu->pc += 1;
    (*cycles) -= 1;

    return data;
}

/**
 * Recupere le byte de poifs fort dans la mémoire
 * @param uint32_t cycles : nombre de cycle pour le fetch
 * @param mem_6502
 * @param cpu_6502
 * @return byte
*/
byte
cpu_fetch_msb(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu)
{
    byte data = memory->data[cpu->pc + 1];
    cpu->pc += 2;
    (*cycles) -= 1;

    return data;
}

word
cpu_fetch_word(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu)
{
    byte lb = cpu_fetch_lsb(cycles, memory, cpu);
    byte hb = cpu_fetch_lsb(cycles, memory, cpu);

    return (lb | (hb << 8));
}

byte
cpu_read_byte_from_zp_adress(uint32_t *cycles, byte addr, mem_6502 *memory, cpu_6502 *cpu)
{
    byte data = memory->data[addr];
    (*cycles) -= 1;
    return data;
}

byte
cpu_read_byte_from_word_adress(uint32_t *cycles, word addr, mem_6502 *memory, cpu_6502 *cpu)
{
    byte data = memory->data[addr];
    (*cycles) -= 1;
  
    return data;
}

/**
 * Permet de lire un mot
 * vue que c'est little endian le low byte avant le hight
 * lb: low byte
 * hb: hight byte
*/
word
cpu_read_word_from_adress(uint32_t *cycles, word addr, mem_6502 *memory, cpu_6502 *cpu)
{
    byte lb = memory->data[addr];
    (*cycles) -= 1;
    byte hb = memory->data[addr + 1];
    (*cycles) -= 1;

    return (lb | (hb << 8));
}


/**
 * Write 2 bytes
*/
void
cpu_write_word_at(uint32_t *cycles, word data, uint32_t dst, mem_6502 *memory, cpu_6502 *cpu)
{
    memory->data[dst] = (data & 0xFF);
    (*cycles) -= 1;
    memory->data[dst + 1] = (data >> 8);
    (*cycles) -= 1;
    cpu->sp += 1;
}

void
cpu_write_byte_at_zp_addr(uint32_t *cycles,
byte zp_addr,
byte value,
mem_6502 *memory,
cpu_6502 *cpu)
{
    memory->data[zp_addr] = value;
    (*cycles) -= 1;
}

/**
 * Permet d'écrire un octet dans la memoire avec une @ de 16bits. 
 * @param cycles : le nombre de cycles nécessaires pour l'instruction
 * @param addr   : l'@ memoire dans la quelle on va ecrire l'octet
 * @param value  : L'octet à ecrire dans la mémoire
 * @param memory : Dans quel memoire on va ecrire ?
 * @param cpu    : Le cpu en question
 * 
*/
void
cpu_write_byte_at_word_addr(uint32_t *cycles,
word addr,
byte value,
mem_6502 *memory,
cpu_6502 *cpu)
{
    memory->data[addr] = value;
    (*cycles) -= 1;
}


// TODO:  Est-ce necessaire ???
void
cpu_push_byte_on_stack(uint32_t *cycles, byte data, mem_6502 *memory, cpu_6502 *cpu)
{
    cpu->sp -= 1;
    memory->data[cpu->sp] = data;
    (*cycles) -= 1;
}

void
cpu_push_word_on_stack(uint32_t *cycles, word data, mem_6502 *memory, cpu_6502 *cpu)
{
    byte lb = (data &  0xFF);
    byte hb = (data >> 0x08);
    /**
     * On push dans ce sens car la stack evolue des @hot vers les bases
     * Et que nous sommes en little endian
    */
    cpu_push_byte_on_stack(cycles, hb, memory, cpu);
    cpu_push_byte_on_stack(cycles, lb, memory, cpu);
}

byte
cpu_pop_byte_off_stack(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu)
{
    byte data = memory->data[cpu->sp];
    memory->data[cpu->sp] = 0;
    cpu->sp += 1;
    (*cycles) -= 2;
    return data;
}

word
cpu_pop_word_off_stack(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu)
{
    byte lb = cpu_pop_byte_off_stack(cycles, memory, cpu);
    byte hb = cpu_pop_byte_off_stack(cycles, memory, cpu);
    return (lb | (hb << 8));
}

/**
 * Permet l'execution d'un instruction qui sera
 * lu dans la mémoire
 * @param cycles
 * @param memory
 * @param cpu
*/
uint32_t
cpu_execute_inst(uint32_t *cycles, mem_6502 *memory, cpu_6502 *cpu)
{
    while (*cycles > 0)
    {
        // lecture de l'OPcode pour savoir l'instruction à executer
        byte inst = cpu_fetch_lsb(cycles, memory, cpu); 
        switch (inst)
        {
        case INS_LDA_IM:
        {
            byte value = cpu_fetch_lsb(cycles, memory, cpu);
            cpu->a = value;
            set_LDA_status(cpu);
        } break;
        case INS_LDA_ZP:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            cpu->a = cpu_read_byte_from_zp_adress(cycles, zp_addr, memory, cpu);
            set_LDA_status(cpu);
            if (*cycles) return (*cycles);
        } break;
        case INS_LDA_ZPX:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            if (zp_addr + cpu->x > 0xFF) {
                (*cycles) -= 1;
            }
            zp_addr += cpu->x;
            cpu->a = cpu_read_byte_from_zp_adress(cycles, zp_addr, memory, cpu);
            (*cycles) -= 1;
            set_LDA_status(cpu);
            if (*cycles) return (*cycles);
        } break;
        case INS_LDA_ABS:
        {
            word abs_addr = cpu_fetch_word(cycles, memory, cpu);
            cpu->a = cpu_read_byte_from_word_adress(cycles, abs_addr, memory, cpu);
            if (*cycles) return (*cycles);
        } break;
        case INS_LDA_ABSX:
        {
            word absx_addr = cpu_fetch_word(cycles, memory, cpu);
            (absx_addr += cpu->x);
            cpu->a = cpu_read_byte_from_word_adress(cycles, absx_addr, memory, cpu);
            set_LDA_status(cpu);
            if (*cycles) return (*cycles);
        } break;
        case INS_LDA_ABSY:
        {
            word absy_addr = cpu_fetch_word(cycles, memory, cpu);
            word absy_addr_y = absy_addr + cpu->y;
            cpu->a = cpu_read_byte_from_word_adress(cycles, absy_addr_y, memory, cpu);
            if (absy_addr_y - absy_addr >= 0xFF)
            {
                (*cycles) -= 1;
            }
            set_LDA_status(cpu);
            if (*cycles) return (*cycles);
        } break;
        case INS_LDA_INDX:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            zp_addr += cpu->x;
            word e_addr = cpu_read_word_from_adress(cycles, zp_addr, memory, cpu);
            cpu->a = cpu_read_byte_from_word_adress(cycles, e_addr, memory, cpu);
            (*cycles) -= 1;
            if (*cycles) return (*cycles);
        }break;
        case INS_LDA_INDY:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            word e_addr = cpu_read_word_from_adress(cycles, zp_addr, memory, cpu);
            word e_addr_y = e_addr + cpu->y;
            cpu->a = cpu_read_byte_from_word_adress(cycles, e_addr_y, memory, cpu);
            if (e_addr_y - e_addr >= 0xFF) // test if cross page
            {
                (*cycles) -= 1;
            }
            if (*cycles) return (*cycles);
        } break;
        case INS_JSR:
        {
            word tr_addr = cpu_fetch_word(cycles, memory, cpu);
            cpu_push_word_on_stack(cycles, cpu->pc - 1, memory, cpu);
            cpu->pc = tr_addr;
            (*cycles) -= 1;
        } break;
        case INS_RTS_IMP:
        {
            word ret_addr = cpu_pop_word_off_stack(cycles, memory, cpu);
            cpu->pc = ret_addr + 1;
            (*cycles) -= 1;
        }break;
        case INS_LDX_IM:
        {
            byte value = cpu_fetch_lsb(cycles, memory, cpu);
            cpu->x = value;
        }break;
        case INS_LDX_ZP:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            cpu->x = cpu_read_byte_from_zp_adress(cycles, zp_addr, memory, cpu);
            
            if (*cycles) return (*cycles);
        }break;
        case INS_LDX_ZPY:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            zp_addr += cpu->y;
            cpu->x = cpu_read_byte_from_zp_adress(cycles, zp_addr, memory, cpu);
            (*cycles) -= 1;
            if (*cycles) return (*cycles);
        }break;
        case INS_LDX_ABS:
        {
            word addr = cpu_fetch_word(cycles, memory, cpu);
            cpu->x = cpu_read_byte_from_word_adress(cycles, addr, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_LDX_ABSY:
        {
            word addr = cpu_fetch_word(cycles, memory, cpu);
            word e_addr = addr + cpu->y;
            if (e_addr - addr >= 0xFF)
            {
                (*cycles) -= 1;
            }
            cpu->x = cpu_read_byte_from_word_adress(cycles, e_addr, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_LDY_IM:
        {
            byte value = cpu_fetch_lsb(cycles, memory, cpu);
            cpu->y = value;
            if (*cycles) return (*cycles);
        }break;
        case INS_LDY_ZP:
        {
            byte addr = cpu_fetch_lsb(cycles, memory, cpu);
            cpu->y = cpu_read_byte_from_zp_adress(cycles, addr, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_LDY_ZPX:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            zp_addr += cpu->x;
            cpu->y = cpu_read_byte_from_zp_adress(cycles, zp_addr, memory, cpu);
            (*cycles) -= 1;
            if (*cycles) return (*cycles);
        }break;
        case INS_LDY_ABS:
        {
            word abs_addr = cpu_fetch_word(cycles, memory, cpu);
            cpu->y = cpu_read_byte_from_word_adress(cycles, abs_addr, memory, cpu); 
            if (*cycles) return (*cycles);
        }break;
        case INS_LDY_ABSX:
        {
            word abs_addr = cpu_fetch_word(cycles, memory, cpu);
            word e_addr = abs_addr + cpu->x;
            if (e_addr - abs_addr >= 0xFF)
            {
                (*cycles) -= 1;
            }
            cpu->y = cpu_read_byte_from_word_adress(cycles, e_addr, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_STA_ZP:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            cpu_write_byte_at_zp_addr(cycles, zp_addr, cpu->a, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_STA_ZPX:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            zp_addr += cpu->x;
            (*cycles) -= 1;
            cpu_write_byte_at_zp_addr(cycles, zp_addr, cpu->a, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_STA_ABS:
        {
            word addr = cpu_fetch_word(cycles, memory, cpu);
            cpu_write_byte_at_word_addr(cycles, addr, cpu->a, memory, cpu);
        }break;
        case INS_STA_ABSX:
        {
            word addr = cpu_fetch_word(cycles, memory, cpu);
            addr += cpu->x;
            cpu_write_byte_at_word_addr(cycles, addr, cpu->a, memory, cpu);
            (*cycles) -= 1;
            if (*cycles) return (*cycles);
        }break;
        case INS_STA_ABSY:
        {
            word addr = cpu_fetch_word(cycles, memory, cpu);
            addr += cpu->y;
            (*cycles) -= 1;
            cpu_write_byte_at_word_addr(cycles, addr, cpu->a, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_STA_INDX:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            zp_addr += cpu->x;
            (*cycles) -= 1;
            word e_addr = cpu_read_word_from_adress(cycles, zp_addr, memory, cpu);
            cpu_write_byte_at_word_addr(cycles, e_addr, cpu->a, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_STA_INDY:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            word e_addr = cpu_read_word_from_adress(cycles, zp_addr, memory, cpu);
            word e_addr_y = e_addr + cpu->y;
            cpu->a = cpu_read_byte_from_word_adress(cycles, e_addr_y, memory, cpu);
            cpu_write_byte_at_word_addr(cycles, e_addr_y, cpu->a, memory, cpu);
            if (e_addr_y - e_addr >= 0xFF) // test if cross page
            {
                (*cycles) -= 1;
            }
            if (*cycles) return (*cycles);
        }break;
        case INS_STX_ZP:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            cpu_write_byte_at_zp_addr(cycles, zp_addr, cpu->x, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_STX_ZPY:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            zp_addr += cpu->y;
            (*cycles) -= 1;
            cpu_write_byte_at_zp_addr(cycles, zp_addr, cpu->x, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_STX_ABS:
        {
            word addr = cpu_fetch_word(cycles, memory, cpu);
            cpu_write_byte_at_word_addr(cycles, addr, cpu->x, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_STY_ZP:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            cpu_write_byte_at_zp_addr(cycles, zp_addr, cpu->y, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_STY_ZPX:
        {
            byte zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            zp_addr += cpu->x;
            (*cycles) -= 1;
            cpu_write_byte_at_zp_addr(cycles, zp_addr, cpu->y, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case INS_STY_ABS:
        {
            word addr = cpu_fetch_word(cycles, memory, cpu);
            cpu_write_byte_at_word_addr(cycles, addr, cpu->y, memory, cpu);
            if (*cycles) return (*cycles);
        }break;
        case CMP_IM:
        {
            word value = cpu_fetch_lsb(cycles, memory, cpu);
            /**
             * Compare acc value with addr value
            */
            uint16_t result = cpu->a - value;
            if (result >= 0) 
                cpu->c = 1;
            if (result == 0)
                cpu->z = 1;
            // test if bit 7 is set
            // when the result is negative
            uint16_t mask = 1 << 7;
            if ((result & mask) != 0)
                cpu->n = 1;

           if (*cycles) return (*cycles);
        }break;
        case CMP_ZP:
        {
            word addr = cpu_fetch_lsb(cycles, memory, cpu);
            word value = cpu_read_byte_from_zp_adress(cycles, addr, memory, cpu);

            uint16_t result = cpu->a - value;
            if (result >= 0) 
                cpu->c = 1;
            if (result == 0)
                cpu->z = 1;
            // test if bit 7 is set
            // when the result is negative
            uint16_t mask = 1 << 7;
            if ((result & mask) != 0)
                cpu->n = 1;
            if (*cycles) return (*cycles);
        }break;
        case CMP_ZPX:
        {
            word zp_addr = cpu_fetch_lsb(cycles, memory, cpu);
            zp_addr += cpu->x;
            (*cycles) -= 1;
            word value = cpu_read_byte_from_zp_adress(cycles, zp_addr, memory, cpu);

            uint16_t result = cpu->a - value;
            if (result >= 0) 
                cpu->c = 1;
            if (result == 0)
                cpu->z = 1;
            // test if bit 7 is set
            // when the result is negative
            uint16_t mask = 1 << 7;
            if ((result & mask) != 0)
                cpu->n = 1;

            if (*cycles) return (*cycles);
        }break;
        default:
            printf("Instruction not handled (0x%X)\n", inst);
            exit(EXIT_FAILURE);
        }
    }
    return (*cycles);
}
