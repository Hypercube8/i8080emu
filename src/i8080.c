#include "i8080.h"

#define SZP_MASK 0b11000100
#define BIT(V, B) ((V) >> (B) & 1) 

#define LOBYTE(WORD) ((WORD) & 0xFF)
#define HIBYTE(WORD) ((WORD) >> 8)
#define LONYBLE(BYTE) ((BYTE) & 0x0F)
#define HINYBLE(BYTE) ((BYTE) & 0xF0)
#define CONCAT(LO, HI) ((LO) | ((HI) << 8))

#define WITH_CARRY cpu->f.cy
#define WITHOUT_CARRY 0

#define WITH_BORROW cpu->f.cy
#define WITHOUT_BORROW 0

#define NZ !cpu->f.z
#define Z cpu->f.z
#define NC !cpu->f.cy
#define C cpu->f.cy
#define PO !cpu->f.p
#define PE cpu->f.p
#define P !cpu->f.s
#define M cpu->f.s

void i8080_init(i8080_cpu_t *cpu,
                void *uptr,
                i8080_reader_t rb, 
                i8080_writer_t wb, 
                i8080_input_t inb, 
                i8080_output_t outb) {
    cpu->psw = 0x0002;
    cpu->bc  = 0x0000;
    cpu->de  = 0x0000;
    cpu->hl  = 0x0000;

    cpu->pc = 0x0000;
    cpu->sp = 0x0000;

    cpu->pending = false;
    cpu->inte = true;
    cpu->delay = 0;
    cpu->vec = 0;

    cpu->hlt = false;

    cpu->cycles = 0;

    cpu->userptr = uptr;

    cpu->read_byte  = rb;
    cpu->write_byte = wb;
    cpu->in_byte    = inb;
    cpu->out_byte   = outb;
}

static inline uint8_t read_byte(i8080_cpu_t *cpu, uint16_t addr) {
    return cpu->read_byte(cpu->userptr, addr);
}

static inline void write_byte(i8080_cpu_t *cpu, uint16_t addr, uint8_t val) {
    cpu->write_byte(cpu->userptr, addr, val);
}

static inline uint8_t in_byte(i8080_cpu_t *cpu, uint8_t port) {
    return cpu->in_byte(cpu->userptr, port);
}

static inline void out_byte(i8080_cpu_t *cpu, uint8_t port, uint8_t val) {
    cpu->out_byte(cpu->userptr, port, val);
}

static inline uint8_t get_m(i8080_cpu_t *cpu) {
    return read_byte(cpu, cpu->hl);
}

static inline void set_m(i8080_cpu_t *cpu, uint8_t val) {
    write_byte(cpu, cpu->hl, val);
}

#ifdef DEBUG
void flags_as_str(i8080_cpu_t *cpu, char *flags) {
    char f[8];
    f[0] =  cpu->f.s ? 'S' : 's'; 
    f[1] =  cpu->f.z ? 'Z' : 'z';
    f[2] = '0';
    f[3] = cpu->f.ac ? 'A' : 'a';
    f[4] = '0';
    f[5] = cpu->f.p ? 'P' : 'p';
    f[6] = '1';
    f[7] = cpu->f.cy ? 'C' : 'c';
    strcpy(flags, f);
}

void i8080_dump_registers(i8080_cpu_t* cpu) {
    printf("\nREGISTERS AND FLAGS\n");
    printf("\tPC: %.4x \n", cpu->pc);
    printf("\tSP: %.4x \n", cpu->sp);
    printf("\tA: %.2x", cpu->a);
    printf("\tB: %.2x", cpu->b);
    printf(" C: %.2x \n", cpu->c);
    printf("\tD: %.2x", cpu->d);
    printf(" E: %.2x \n", cpu->e);
    printf("\tH: %.2x", cpu->h);
    printf(" L: %.2x \n", cpu->l);

    char flags[8];
    flags_as_str(cpu, flags);
    printf("\tflags: %s", flags);
}

void i8080_dump_memory(i8080_cpu_t *cpu, uint8_t page) {
    printf("\nMEMORY - PAGE %.2x", page);
    uint16_t base = page * 0xFF;
    for (int i=0; i<16; i++) {
        printf("\n \t %.4x: ", base + i * 16);
        for (int j=0; j<16; j++) {
            printf("%.2x ", read_byte(cpu, base + i * 16 + j));
        }
    }
}

void i8080_dump_stack(i8080_cpu_t *cpu) {
    printf("\nSTACK\n");
    printf("\t> %.2x \n", read_byte(cpu, cpu->sp));
    for (int i = 1; i<10; i++) {
        printf("\t  %.2x \n", read_byte(cpu, cpu->sp+i));
    }
}
#endif

static inline uint8_t fetch_byte(i8080_cpu_t* cpu) {
    return read_byte(cpu, cpu->pc++);
}

static inline uint16_t fetch_word(i8080_cpu_t* cpu) {
    uint8_t lo = read_byte(cpu, cpu->pc++);
    uint8_t hi = read_byte(cpu, cpu->pc++);
    return CONCAT(lo, hi);
}

static inline uint16_t read_word(i8080_cpu_t* cpu, uint16_t addr) {
    uint8_t lo = read_byte(cpu, addr);
    uint8_t hi = read_byte(cpu, addr+1);
    return CONCAT(lo, hi);
}

static inline void write_word(i8080_cpu_t* cpu, uint16_t addr, uint16_t val) {
    write_byte(cpu, addr, LOBYTE(val));
    write_byte(cpu, addr+1, HIBYTE(val));
}

static const uint8_t SZP_TABLE[] = {
    68, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4, 
    0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0, 
    0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0, 
    4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4, 
    0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0, 
    4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4, 
    4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4, 
    0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0, 
    128, 132, 132, 128, 132, 128, 128, 132, 132, 128, 128, 132, 128, 132, 132, 128, 
    132, 128, 128, 132, 128, 132, 132, 128, 128, 132, 132, 128, 132, 128, 128, 132, 
    132, 128, 128, 132, 128, 132, 132, 128, 128, 132, 132, 128, 132, 128, 128, 132, 
    128, 132, 132, 128, 132, 128, 128, 132, 132, 128, 128, 132, 128, 132, 132, 128, 
    132, 128, 128, 132, 128, 132, 132, 128, 128, 132, 132, 128, 132, 128, 128, 132, 
    128, 132, 132, 128, 132, 128, 128, 132, 132, 128, 128, 132, 128, 132, 132, 128, 
    128, 132, 132, 128, 132, 128, 128, 132, 132, 128, 128, 132, 128, 132, 132, 128, 
    132, 128, 128, 132, 128, 132, 132, 128, 128, 132, 132, 128, 132, 128, 128, 132, 
};

static const uint8_t CYCLES[] = {
    4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,  
    4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,  
    4, 10, 16, 5,  5,  5,  7,  4,  4, 10, 16, 5,  5,  5,  7, 4,  
    4, 10, 13, 5,  10, 10, 10, 4,  4, 10, 13, 5,  5,  5,  7, 4,  
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  
    7, 7,  7,  7,  7,  7,  7,  7,  5, 5,  5,  5,  5,  5,  7, 5,  
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  
    5, 10, 10, 10, 11, 11, 7,  11, 5, 10, 10, 10, 11, 17, 7, 11, 
    5, 10, 10, 10, 11, 11, 7,  11, 5, 10, 10, 10, 11, 17, 7, 11, 
    5, 10, 10, 18, 11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11, 
    5, 10, 10, 4,  11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11  
};

static inline void set_szp(i8080_cpu_t *cpu, uint8_t val) {
    cpu->psw ^= cpu->psw & SZP_MASK; 
    cpu->psw |= SZP_TABLE[val];
}

static inline bool aux_carry(uint8_t res, uint8_t a, uint8_t val) {
    return BIT(res ^ a ^ val, 4);
}

void i8080_interrupt(i8080_cpu_t *cpu, instruction_t instr) {
    cpu->pending = true;
    cpu->vec = instr;
}

static void xchg(i8080_cpu_t *cpu) {
    uint16_t tmp = cpu->de;
    cpu->de = cpu->hl;
    cpu->hl = tmp;
}

static void xthl(i8080_cpu_t *cpu) {
    uint16_t tmp = cpu->hl;
    cpu->hl = read_word(cpu, cpu->sp);
    write_word(cpu, cpu->sp, tmp);
}

static void rlc(i8080_cpu_t *cpu) {
    bool msb = BIT(cpu->a, 7);
    cpu->a <<= 1;
    cpu->a |= msb;
    cpu->f.cy = msb;
}

static void rrc(i8080_cpu_t *cpu) {
    bool lsb = BIT(cpu->a, 0);
    cpu->a >>= 1;
    cpu->a |= lsb << 7;
    cpu->f.cy = lsb;
}

static void ral(i8080_cpu_t *cpu) {
    bool msb = BIT(cpu->a, 7);
    cpu->a <<= 1;
    cpu->a |= cpu->f.cy;
    cpu->f.cy = msb;
}

static void rar(i8080_cpu_t *cpu) {
    bool lsb = BIT(cpu->a, 0);
    cpu->a >>= 1;
    cpu->a |= cpu->f.cy << 7;
    cpu->f.cy = lsb;
}

static void add(i8080_cpu_t *cpu, uint8_t val, bool c) {
    uint16_t res = cpu->a + val + c;
    set_szp(cpu, res);
    cpu->f.cy = BIT(res, 8);
    cpu->f.ac = aux_carry(res, cpu->a, val);
    cpu->a = res;
}

static void sub(i8080_cpu_t* cpu, uint8_t val, bool b) {
    uint16_t res = cpu->a - val - b;
    set_szp(cpu, res);
    cpu->f.cy = BIT(res, 8);
    cpu->f.ac = !aux_carry(res, cpu->a, val);
    cpu->a = res; 
}

static uint8_t inc(i8080_cpu_t* cpu, uint8_t val) {
    uint16_t res = val + 1;
    set_szp(cpu, res);
    cpu->f.ac = (res & 0x0F) == 0;
    return res; 
}

static uint8_t dec(i8080_cpu_t* cpu, uint8_t val) {
    uint16_t res = val - 1;
    set_szp(cpu, res);
    cpu->f.ac = (res & 0x0F) != 0x0F;
    return res; 
}

static void dad(i8080_cpu_t* cpu, uint16_t val) {
    uint32_t res = cpu->hl + val;
    cpu->f.cy = BIT(res, 16);
    cpu->hl = res;
}

static void and(i8080_cpu_t* cpu, uint8_t val) {
    uint8_t res = cpu->a & val;
    set_szp(cpu, res);
    cpu->f.cy = 0;
    cpu->f.ac = BIT(cpu->a | val, 3);
    cpu->a = res;
}

static void xor(i8080_cpu_t *cpu, uint8_t val) {
    uint8_t res = cpu->a ^ val;
    set_szp(cpu, res);
    cpu->f.cy = 0;
    cpu->f.ac = 0;
    cpu->a = res;
}

static void or(i8080_cpu_t *cpu, uint8_t val) {
    uint8_t res = cpu->a | val;
    set_szp(cpu, res);
    cpu->f.cy = 0;
    cpu->f.ac = 0;
    cpu->a = res;
}

static void cmp(i8080_cpu_t *cpu, uint8_t val) {
    uint16_t res = cpu->a - val;
    set_szp(cpu, res);
    cpu->f.cy = BIT(res, 8);
    cpu->f.ac = !aux_carry(res, cpu->a, val);
}

static void daa(i8080_cpu_t *cpu) {
    if (LONYBLE(cpu->a) > 0x09 || cpu->f.ac) {
        uint16_t res = cpu->a + 0x06;
        cpu->f.cy |= BIT(res, 8);
        cpu->f.ac = aux_carry(res, cpu->a, 0x06);
        cpu->a = res;
    }

    if (HINYBLE(cpu->a) > 0x90 || cpu->f.cy) {
        uint16_t res = cpu->a + 0x60;
        cpu->f.cy |= BIT(res, 8);
        cpu->a = res;
    }
    set_szp(cpu, cpu->a);
}

static inline void push(i8080_cpu_t *cpu, uint16_t val) {
    write_word(cpu, cpu->sp-2, val);
    cpu->sp -= 2;
}

static inline uint16_t pop(i8080_cpu_t *cpu) {
    uint16_t val = read_word(cpu, cpu->sp);
    cpu->sp += 2;
    return val;
}

static void jmp_cond(i8080_cpu_t *cpu, bool condition) {
    uint16_t addr = fetch_word(cpu);
    if (condition) {
        cpu->pc = addr;
    }
}

static void call(i8080_cpu_t *cpu, uint16_t addr) {
    push(cpu, cpu->pc);
    cpu->pc = addr;
}

static void call_cond(i8080_cpu_t *cpu, bool condition) {
    uint16_t addr = fetch_word(cpu);
    if (condition) {
        call(cpu, addr);
        cpu->cycles += 6;
    }
}

static void ret(i8080_cpu_t *cpu) {
    cpu->pc = pop(cpu);
}

static void ret_cond(i8080_cpu_t *cpu, bool condition) {
    if (condition) {
        ret(cpu);
        cpu->cycles += 6;
    }
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
        case MVI_A_D8: cpu->a = fetch_byte(cpu); break;
        case MVI_B_D8: cpu->b = fetch_byte(cpu); break;
        case MVI_C_D8: cpu->c = fetch_byte(cpu); break;
        case MVI_D_D8: cpu->d = fetch_byte(cpu); break;
        case MVI_E_D8: cpu->e = fetch_byte(cpu); break;
        case MVI_H_D8: cpu->h = fetch_byte(cpu); break;
        case MVI_L_D8: cpu->l = fetch_byte(cpu); break;
        // MVI M, data
        case MVI_M_D8: set_m(cpu, fetch_byte(cpu)); break;
        // LXI rp, data 16
        case LXI_B_D16:  cpu->bc = fetch_word(cpu); break;
        case LXI_D_D16:  cpu->de = fetch_word(cpu); break;
        case LXI_H_D16:  cpu->hl = fetch_word(cpu); break;
        case LXI_SP_D16: cpu->sp = fetch_word(cpu); break;
        // LDA addr
        case LDA_A16: cpu->a = read_byte(cpu, fetch_word(cpu)); break;
        // STA addr
        case STA_A16: write_byte(cpu, fetch_word(cpu), cpu->a); break; 
        // LHLD addr
        case LHLD_A16: cpu->hl = read_word(cpu, fetch_word(cpu)); break;
        // SHLD addr
        case SHLD_A16: write_word(cpu, fetch_word(cpu), cpu->hl); break;
        // LDAX rp
        case LDAX_B: cpu->a = read_byte(cpu, cpu->bc); break;
        case LDAX_D: cpu->a = read_byte(cpu, cpu->de); break;
        // STAX rp
        case STAX_B: write_byte(cpu, cpu->bc, cpu->a); break;
        case STAX_D: write_byte(cpu, cpu->de, cpu->a); break;
        // XCHG
        case XCHG: xchg(cpu); break;
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
        case ADI_D8: add(cpu, fetch_byte(cpu), WITHOUT_CARRY); break;
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
        case ACI_D8: add(cpu, fetch_byte(cpu), WITH_CARRY); break;
        // SUB r
        case SUB_A: sub(cpu, cpu->a, WITHOUT_BORROW); break;
        case SUB_B: sub(cpu, cpu->b, WITHOUT_BORROW); break;
        case SUB_C: sub(cpu, cpu->c, WITHOUT_BORROW); break;
        case SUB_D: sub(cpu, cpu->d, WITHOUT_BORROW); break;
        case SUB_E: sub(cpu, cpu->e, WITHOUT_BORROW); break;
        case SUB_H: sub(cpu, cpu->h, WITHOUT_BORROW); break;
        case SUB_L: sub(cpu, cpu->l, WITHOUT_BORROW); break;
        // SUB M
        case SUB_M: sub(cpu, get_m(cpu), WITHOUT_BORROW); break;
        // SUI data
        case SUI_D8: sub(cpu, fetch_byte(cpu), WITHOUT_BORROW); break;
        // SBB r
        case SBB_A: sub(cpu, cpu->a, WITH_BORROW); break;
        case SBB_B: sub(cpu, cpu->b, WITH_BORROW); break;
        case SBB_C: sub(cpu, cpu->c, WITH_BORROW); break;
        case SBB_D: sub(cpu, cpu->d, WITH_BORROW); break;
        case SBB_E: sub(cpu, cpu->e, WITH_BORROW); break;
        case SBB_H: sub(cpu, cpu->h, WITH_BORROW); break;
        case SBB_L: sub(cpu, cpu->l, WITH_BORROW); break;
        // SBB M
        case SBB_M: sub(cpu, get_m(cpu), WITH_BORROW); break;
        // SBI data
        case SBI_D8: sub(cpu, fetch_byte(cpu), WITH_BORROW); break;
        // INR r
        case INR_A: cpu->a = inc(cpu, cpu->a); break;
        case INR_B: cpu->b = inc(cpu, cpu->b); break;
        case INR_C: cpu->c = inc(cpu, cpu->c); break;
        case INR_D: cpu->d = inc(cpu, cpu->d); break;
        case INR_E: cpu->e = inc(cpu, cpu->e); break;
        case INR_H: cpu->h = inc(cpu, cpu->h); break;
        case INR_L: cpu->l = inc(cpu, cpu->l); break;
        // INR M
        case INR_M: set_m(cpu, inc(cpu, get_m(cpu))); break;
        // DCR r
        case DCR_A: cpu->a = dec(cpu, cpu->a); break;
        case DCR_B: cpu->b = dec(cpu, cpu->b); break;
        case DCR_C: cpu->c = dec(cpu, cpu->c); break;
        case DCR_D: cpu->d = dec(cpu, cpu->d); break;
        case DCR_E: cpu->e = dec(cpu, cpu->e); break;
        case DCR_H: cpu->h = dec(cpu, cpu->h); break;
        case DCR_L: cpu->l = dec(cpu, cpu->l); break;
        // DCR M
        case DCR_M: set_m(cpu, dec(cpu, get_m(cpu))); break;
        // INX rp
        case INX_B:  cpu->bc++; break;
        case INX_D:  cpu->de++; break;
        case INX_H:  cpu->hl++; break;
        case INX_SP: cpu->sp++; break;
        // DCX rp
        case DCX_B:  cpu->bc--; break;
        case DCX_D:  cpu->de--; break;
        case DCX_H:  cpu->hl--; break;
        case DCX_SP: cpu->sp--; break;
        // DAD
        case DAD_B:  dad(cpu, cpu->bc); break;
        case DAD_D:  dad(cpu, cpu->de); break;
        case DAD_H:  dad(cpu, cpu->hl); break;
        case DAD_SP: dad(cpu, cpu->sp); break;
        // DAA
        case DAA: daa(cpu); break; 
        // ANA r
        case ANA_A: and(cpu, cpu->a); break;
        case ANA_B: and(cpu, cpu->b); break;
        case ANA_C: and(cpu, cpu->c); break;
        case ANA_D: and(cpu, cpu->d); break;
        case ANA_E: and(cpu, cpu->e); break;
        case ANA_H: and(cpu, cpu->h); break;
        case ANA_L: and(cpu, cpu->l); break;
        // ANA M
        case ANA_M: and(cpu, get_m(cpu)); break;
        // ANI data
        case ANI_D8: and(cpu, fetch_byte(cpu)); break;
        // XRA r
        case XRA_A: xor(cpu, cpu->a); break;
        case XRA_B: xor(cpu, cpu->b); break;
        case XRA_C: xor(cpu, cpu->c); break;
        case XRA_D: xor(cpu, cpu->d); break;
        case XRA_E: xor(cpu, cpu->e); break;
        case XRA_H: xor(cpu, cpu->h); break;
        case XRA_L: xor(cpu, cpu->l); break;
        // XRA M
        case XRA_M: xor(cpu, get_m(cpu)); break;
        // XRI data
        case XRI_D8: xor(cpu, fetch_byte(cpu)); break;
        // ORA r
        case ORA_A: or(cpu, cpu->a); break;
        case ORA_B: or(cpu, cpu->b); break;
        case ORA_C: or(cpu, cpu->c); break;
        case ORA_D: or(cpu, cpu->d); break;
        case ORA_E: or(cpu, cpu->e); break;
        case ORA_H: or(cpu, cpu->h); break;
        case ORA_L: or(cpu, cpu->l); break;
        // ORA M
        case ORA_M: or(cpu, get_m(cpu)); break;
        // ORI data
        case ORI_D8: or(cpu, fetch_byte(cpu)); break;
        // CMP r
        case CMP_A: cmp(cpu, cpu->a); break;
        case CMP_B: cmp(cpu, cpu->b); break;
        case CMP_C: cmp(cpu, cpu->c); break;
        case CMP_D: cmp(cpu, cpu->d); break;
        case CMP_E: cmp(cpu, cpu->e); break;
        case CMP_H: cmp(cpu, cpu->h); break;
        case CMP_L: cmp(cpu, cpu->l); break;
        // CMP M
        case CMP_M: cmp(cpu, get_m(cpu)); break;
        // CMP data
        case CPI_D8: cmp(cpu, fetch_byte(cpu)); break;
        // RLC
        case RLC: rlc(cpu); break; 
        // RRC
        case RRC: rrc(cpu); break;
        // RAL
        case RAL: ral(cpu); break;
        // RAR
        case RAR: rar(cpu); break;
        // CMA
        case CMA: cpu->a = ~cpu->a; break;
        // CMC
        case CMC: cpu->f.cy = !cpu->f.cy; break;
        // STC
        case STC: cpu->f.cy = 1; break;
        // JMP addr
        case JMP_A16: cpu->pc = fetch_word(cpu); break;
        // Jcondition addr
        case JNZ_A16: jmp_cond(cpu, NZ); break; 
        case JZ_A16:  jmp_cond(cpu, Z ); break;
        case JNC_A16: jmp_cond(cpu, NC); break;
        case JC_A16:  jmp_cond(cpu, C ); break;
        case JPO_A16: jmp_cond(cpu, PO); break;
        case JPE_A16: jmp_cond(cpu, PE); break;
        case JP_A16:  jmp_cond(cpu, P ); break;
        case JM_A16:  jmp_cond(cpu, M ); break;
        // CALL A16
        case CALL_A16: call(cpu, fetch_word(cpu)); break;
        // Ccondition addr
        case CNZ_A16: call_cond(cpu, NZ); break;
        case CZ_A16:  call_cond(cpu, Z ); break;
        case CNC_A16: call_cond(cpu, NC); break;
        case CC_A16:  call_cond(cpu, C ); break;
        case CPO_A16: call_cond(cpu, PO); break;
        case CPE_A16: call_cond(cpu, PE); break;
        case CP_A16:  call_cond(cpu, P ); break;
        case CM_A16:  call_cond(cpu, M ); break;
        // RET
        case RET: ret(cpu); break;
        // Rcondition 
        case RNZ: ret_cond(cpu, NZ); break;
        case RZ:  ret_cond(cpu, Z ); break;
        case RNC: ret_cond(cpu, NC); break;
        case RC:  ret_cond(cpu, C ); break;
        case RPO: ret_cond(cpu, PO); break;
        case RPE: ret_cond(cpu, PE); break;
        case RP:  ret_cond(cpu, P ); break;
        case RM:  ret_cond(cpu, M ); break;
        // RST n
        case RST_0: call(cpu, RESTART_0); break;
        case RST_1: call(cpu, RESTART_1); break;
        case RST_2: call(cpu, RESTART_2); break;
        case RST_3: call(cpu, RESTART_3); break;
        case RST_4: call(cpu, RESTART_4); break;
        case RST_5: call(cpu, RESTART_5); break;
        case RST_6: call(cpu, RESTART_6); break;
        case RST_7: call(cpu, RESTART_7); break;
        // PCHL
        case PCHL: cpu->pc = cpu->hl; break;
        // PUSH rp
        case PUSH_B: push(cpu, cpu->bc); break;
        case PUSH_D: push(cpu, cpu->de); break;
        case PUSH_H: push(cpu, cpu->hl); break;
        // PUSH PSW
        case PUSH_PSW: cpu->f.pad1 = 1; cpu->f.pad2=0; cpu->f.pad3=0; push(cpu, cpu->psw); break;
        // POP rp
        case POP_B: cpu->bc = pop(cpu); break;
        case POP_D: cpu->de = pop(cpu); break;
        case POP_H: cpu->hl = pop(cpu); break;
        // POP PSW 
        case POP_PSW: cpu->psw = pop(cpu); cpu->f.pad1 = 1; cpu->f.pad2=0; cpu->f.pad3=0; break;
        // XTHL
        case XTHL: xthl(cpu); break;
        // SPHL
        case SPHL: cpu->sp = cpu->hl; break;
        // IN
        case IN_D8: cpu->a = in_byte(cpu, fetch_byte(cpu)); break;
        // OUT
        case OUT_D8: out_byte(cpu, fetch_byte(cpu), cpu->a); break;
        // EI
        case EI: cpu->delay = 1; cpu->inte = true; break;
        // DI
        case DI: cpu->inte = false; break;
        // HLT
        case HLT: cpu->hlt = true; break;
        // NOP
        case NOP: break; 
    }
}

void i8080_step(i8080_cpu_t *cpu) {
    if (cpu->inte && cpu->pending && cpu->delay == 0) {
        cpu->pending = false;
        cpu->hlt = false;
        decode(cpu, cpu->vec);
    } else if (!cpu->hlt) {
        instruction_t opcode = fetch_byte(cpu);
        cpu->cycles += CYCLES[opcode];
        decode(cpu, opcode);
    }
}

#undef BIT
#undef SZP_MASK

#undef LOBYTE
#undef HIBYTE
#undef LONYBLE
#undef HINYBLE
#undef CONCAT

#undef WITH_CARRY
#undef WITHOUT_CARRY
#undef WITH_BORROW
#undef WITHOUT_BORROW

#undef NZ
#undef Z 
#undef NC 
#undef C 
#undef PO 
#undef PE 
#undef P 
#undef M 