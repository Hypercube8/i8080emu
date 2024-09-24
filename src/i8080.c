#include "i8080.h"

#define CONCAT(LO, HI) ((LO) | ((HI) << 8))
#define LSB(WORD) ((WORD) & 0xFF)
#define MSB(WORD) ((WORD) >> 8)

#define DATA8 fetch_byte(cpu);
#define DATA16 fetch_word(cpu);
#define ADDR16 fetch_word(cpu);

static inline uint8_t get_m(i8080_cpu_t* cpu) {
    return cpu->read_byte(cpu->hl);
}

static inline void set_m(i8080_cpu_t* cpu, uint8_t val) {
    cpu->write_byte(cpu->hl, val);
}

static inline uint8_t fetch_byte(i8080_cpu_t* cpu) {
    return cpu->read_byte(cpu->pc++)
}

static inline uint16_t fetch_word(i8080_cpu_t* cpu) {
    uint8_t lo = cpu->read_byte(cpu->pc++);
    uint8_t hi = cpu->read_byte(cpu->pc++);
    return CONCAT(lo, hi);
}

static inline uint16_t read_word(i8080_cpu_t* cpu, uint16_t addr) {
    uint8_t lo = cpu->read_byte(addr);
    uint8_t hi = cpu->read_byte(addr+1);
    return CONCAT(lo, hi);
}

static inline void write_word(i8080_cpu_t* cpu, uint16_t addr, uint8_t val) {
    cpu->write_byte(addr, LSB(val));
    cpu->write_byte(addr+1, MSB(val));
}

static void decode(i8080_cpu_t *cpu, instruction_t instr) {
    switch (instr) {
        // MOV r1, r2
        case MOV_A_A: cpu->a = cpu->a; break;
        case MOV_A_B: cpu->a = cpu->b; break;
        case MOV_A_C: cpu->a = cpu->c; break;
        case MOV_A_D: cpu->a = cpu->d; break;
        case MOV_A_E: cpu->a = cpu->e; break;
        case MOV_A_H: cpu->a = cpu->h; break;
        case MOV_A_L: cpu->a = cpu->l; break;
        case MOV_B_A: cpu->b = cpu->a; break;
        case MOV_B_B: cpu->b = cpu->b; break;
        case MOV_B_C: cpu->b = cpu->c; break;
        case MOV_B_D: cpu->b = cpu->d; break;
        case MOV_B_E: cpu->b = cpu->e; break;
        case MOV_B_H: cpu->b = cpu->h; break;
        case MOV_B_L: cpu->b = cpu->l; break;
        case MOV_C_A: cpu->c = cpu->a; break;
        case MOV_C_B: cpu->c = cpu->b; break;
        case MOV_C_C: cpu->c = cpu->c; break;
        case MOV_C_D: cpu->c = cpu->d; break;
        case MOV_C_E: cpu->c = cpu->e; break;
        case MOV_C_H: cpu->c = cpu->h; break;
        case MOV_C_L: cpu->c = cpu->l; break;
        case MOV_D_A: cpu->d = cpu->a; break;
        case MOV_D_B: cpu->d = cpu->b; break;
        case MOV_D_C: cpu->d = cpu->c; break;
        case MOV_D_D: cpu->d = cpu->d; break;
        case MOV_D_E: cpu->d = cpu->e; break;
        case MOV_D_H: cpu->d = cpu->h; break;
        case MOV_D_L: cpu->d = cpu->l; break;
        case MOV_E_A: cpu->e = cpu->a; break;
        case MOV_E_B: cpu->e = cpu->b; break;
        case MOV_E_C: cpu->e = cpu->c; break;
        case MOV_E_D: cpu->e = cpu->d; break;
        case MOV_E_E: cpu->e = cpu->e; break;
        case MOV_E_H: cpu->e = cpu->h; break;
        case MOV_E_L: cpu->e = cpu->l; break;   
        case MOV_H_A: cpu->h = cpu->a; break;
        case MOV_H_B: cpu->h = cpu->b; break;
        case MOV_H_C: cpu->h = cpu->c; break;
        case MOV_H_D: cpu->h = cpu->d; break;
        case MOV_H_E: cpu->h = cpu->e; break;
        case MOV_H_H: cpu->h = cpu->h; break;
        case MOV_H_L: cpu->h = cpu->l; break;  
        case MOV_L_A: cpu->l = cpu->a; break;
        case MOV_L_B: cpu->l = cpu->b; break;
        case MOV_L_C: cpu->l = cpu->c; break;
        case MOV_L_D: cpu->l = cpu->d; break;
        case MOV_L_E: cpu->l = cpu->e; break;
        case MOV_L_H: cpu->l = cpu->h; break;
        case MOV_L_L: cpu->l = cpu->l; break;     
        // MOV r, M
        case MOV_A_M: cpu->a = get_m(cpu); break;
        case MOV_B_M: cpu->b = get_m(cpu); break;
        case MOV_C_M: cpu->c = get_m(cpu); break;
        case MOV_D_M: cpu->d = get_m(cpu); break;
        case MOV_E_M: cpu->e = get_m(cpu); break;
        case MOV_H_M: cpu->h = get_m(cpu); break;
        case MOV_L_M: cpu->l = get_m(cpu); break;
        // MOV M, r
        case MOV_M_A: set_m(cpu, cpu->a); break;
        case MOV_M_B: set_m(cpu, cpu->b); break;
        case MOV_M_C: set_m(cpu, cpu->c); break;
        case MOV_M_D: set_m(cpu, cpu->d); break;
        case MOV_M_E: set_m(cpu, cpu->e); break;
        case MOV_M_H: set_m(cpu, cpu->h); break;
        case MOV_M_L: set_m(cpu, cpu->l); break;
        // MVI r, data
        case MVI_A_D8: cpu->a = DATA8; break;
        case MVI_B_D8: cpu->b = DATA8; break;
        case MVI_C_D8: cpu->c = DATA8; break;
        case MVI_D_D8: cpu->d = DATA8; break;
        case MVI_E_D8: cpu->e = DATA8; break;
        case MVI_H_D8: cpu->h = DATA8; break;
        case MVI_L_D8: cpu->l = DATA8; break;
        // MVI M, data
        case MVI_M_D8: set_m(cpu, DATA8); break;
        // LXI rp, data 16
        case LXI_B_D16:  cpu->bc = IMM16;
        case LXI_D_D16:  cpu->de = IMM16;
        case LXI_H_D16:  cpu->hl = IMM16;
        case LXI_SP_D16: cpu->sp = IMM16;
        // LDA addr
        case LDA_A16: cpu->a = cpu->read_byte(ADDR16); break;
        // STA addr
        case STA_A16: cpu->write_byte(ADDR16, cpu->a); break; 
        // LHLD addr
        case LHLD_A16: cpu->hl = read_word(cpu, ADDR16); break;
        // SHLD addr
        case SHLD_A16: write_word(cpu, ADDR16, cpu->hl); break;
        // LDAX rp
        case LDAX_B: cpu->a = cpu->read_byte(cpu->bc); break;
        case LDAX_D: cpu->a = cpu->read_byte(cpu->de); break;
        // STAX rp
        case STAX_B: cpu->write_byte(cpu->bc, cpu->a); break;
        case STAX_D: cpu->write_byte(cpu->de, cpu->a); break;
        // XCHG
        case XCHG: {
            uint8_t tmp = cpu->de;
            cpu->de = cpu->hl;
            cpu->hl = tmp;
        }
    }
}

#undef CONCAT
#undef LSB
#undef MSB

#undef IMM8
#undef IMM16
#undef ADDR16