#include "i8080.h"

#define CONCAT(LO, HI) ((LO) | ((HI) << 8))
#define LSB(WORD) ((WORD) & 0xFF)
#define MSB(WORD) ((WORD) >> 8)

#define DATA8 fetch_byte(cpu)
#define DATA16 fetch_word(cpu)
#define ADDR16 fetch_word(cpu)

void i8080_init(i8080_cpu_t* cpu, 
                i8080_reader_t* rb, 
                i8080_output_t* wb, 
                i8080_input_t* inb, 
                i8080_output_t* outb) {
    cpu->psw = 0x0004;
    cpu->bc  = 0x0000;
    cpu->de  = 0x0000;
    cpu->hl  = 0x0000;

    cpu->read_byte  = rb;
    cpu->write_byte = wb;
    cpu->in_byte    = inb;
    cpu->out_byte   = outb;
}

void i8080_debug(i8080_cpu_t* cpu) {
    static int callnum = 1;
    printf("\n Debug Call #%d \n", callnum);
    printf("\n Registers: \n");
    printf("\t A: %.2x");
    printf("B: %.2x", cpu->b);
    printf("C: %.2x", cpu->c);
    printf("D: %.2x", cpu->d);
    printf("E: %.2x", cpu->e);
    printf("H: %.2x", cpu->h);
    printf("L: %.2x \n", cpu->l);
    printf("\t M: %.2x", get_m(cpu));
    printf("\n Register Pairs: \n");
    printf("\t PC: %.4x \n", cpu->pc);
    printf("\tBC: %.4x", cpu->bc);
    printf("DE: %.4x", cpu->de);
    printf("HL: %.4x", cpu->hl);
    printf("SP: %.4x \n", cpu->sp);
    printf("\t PSW: %.4x", cpu->psw);
    printf("\n Flags: \n");
    printf("\t F: 0b%d", itoa(cpu->f, 2));
    printf("\t Z: %d", cpu->f.z);
    printf("S: %d", cpu->f.s);
    printf("P: %d", cpu->f.p);
    printf("CY: %d", cpu->f.cy);
    printf("AC: %d", cpu->f.ac);
    callnum++;
}

void i8080_dump(i8080_cpu_t *cpu, uint8_t page) {
    printf("Memory Dump of Page #%d", page);
    uint16_t base = page * 0xFF;
    for (int i=0; i<16; i++) {
        printf("\n \t %.4x:", base + i * 16);
        for (int j=0; j<16; j++) {
            printf("%.2x", base + i * 16 + j);
        }
    }
}

static inline uint8_t get_m(i8080_cpu_t* cpu) {
    return cpu->read_byte(cpu->hl);
}

static inline void set_m(i8080_cpu_t* cpu, uint8_t val) {
    cpu->write_byte(cpu->hl, val);
}

static inline uint8_t fetch_byte(i8080_cpu_t* cpu) {
    return cpu->read_byte(cpu->pc++);
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
        case LXI_B_D16:  cpu->bc = DATA16;
        case LXI_D_D16:  cpu->de = DATA16;
        case LXI_H_D16:  cpu->hl = DATA16;
        case LXI_SP_D16: cpu->sp = DATA16;
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
            break;
        }

        #define WITH_CARRY cpu->f.cy
        #define WITHOUT_CARRY 0

        #define WITH_BORROW cpu->f.cy
        #define WITHOUT_BORROW 0

        // ADD r 
        case ADD_A: add(cpu, cpu->a, WITHOUT_CARRY); break;
        case ADD_B: add(cpu, cpu->b, WITHOUT_CARRY); break;
        case ADD_C: add(cpu, cpu->c, WITHOUT_CARRY); break;
        case ADD_D: add(cpu, cpu->d, WITHOUT_CARRY); break;
        case ADD_E: add(cpu, cpu->e, WITHOUT_CARRY); break;
        case ADD_H: add(cpu, cpu->h, WITHOUT_CARRY); break;
        case ADD_L: add(cpu, cpu->l, WITHOUT_CARRY); break;
        // ADD M
        case ADD_M: add(cpu, get_m(cpu), WITHOUT_CARRY); break;
        // ADI data
        case ADI_D8: add(cpu, DATA8, WITHOUT_CARRY); break;
        // ADC r
        case ADC_A: add(cpu, cpu->a, WITH_CARRY); break;
        case ADC_B: add(cpu, cpu->b, WITH_CARRY); break;
        case ADC_C: add(cpu, cpu->c, WITH_CARRY); break;
        case ADC_D: add(cpu, cpu->d, WITH_CARRY); break;
        case ADC_E: add(cpu, cpu->e, WITH_CARRY); break;
        case ADC_H: add(cpu, cpu->h, WITH_CARRY); break;
        case ADC_L: add(cpu, cpu->l, WITH_CARRY); break;
        // ADC M
        case ADC_M: add(cpu, get_m(cpu), WITH_CARRY); break;
        // ACI data
        case ADC_D8: add(cpu, DATA8, WITH_CARRY); break;
    }
}

#define SET(F, COND) cpu->f.F = COND;
#define BIT(V, B) ((V) >> (B) & 1) 

static void add(i8080_cpu_t* cpu, uint8_t val, bool c) {
    uint16_t res = cpu->a + val + c;
    SET(z, res == 0);
    SET(s, BIT(res, 7));
    SET(p, PARITY[res]);
    SET(cy, BIT(res, 8));
    SET(ac, BIT(cpu->a & val, 3));
    cpu->a = res & 0xFF;
}

static void sub(i8080_cpu_t* cpu, uint8_t val, bool b) {
    uint16_t res = cpu->a - val - b;
    SET(z, res == 0);
    SET(s, BIT(res, 7));
    SET(p, PARITY[res]);
    SET(cy, BIT(res, 8));
    SET(ac, BIT(cpu->a & val, 3));
    cpu->a = res & 0xFF;
}

#undef SET
#undef BIT

#undef CONCAT
#undef LSB
#undef MSB

#undef IMM8
#undef IMM16
#undef ADDR16