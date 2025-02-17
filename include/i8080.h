#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

typedef enum {
    NOP       = 0x00,
    LXI_B_D16 = 0x01,
    STAX_B    = 0x02,
    INX_B     = 0x03,
    INR_B     = 0x04,
    DCR_B     = 0x05,
    MVI_B_D8  = 0x06,
    RLC       = 0x07,
    DAD_B     = 0x09,
    LDAX_B    = 0x0A,
    DCX_B     = 0x0B,
    INR_C     = 0x0C,
    DCR_C     = 0x0D,
    MVI_C_D8  = 0x0E,
    RRC       = 0x0F,
    LXI_D_D16 = 0x11,
    STAX_D    = 0x12,
    INX_D     = 0x13,
    INR_D     = 0x14,
    DCR_D     = 0x15,
    MVI_D_D8  = 0x16,
    RAL       = 0x17,
    DAD_D     = 0x19,
    LDAX_D    = 0x1A,
    DCX_D     = 0x1B,
    INR_E     = 0x1C,
    DCR_E     = 0x1D,
    MVI_E_D8  = 0x1E,
    RAR       = 0x1F,
    LXI_H_D16 = 0x21,
    SHLD_A16  = 0x22,
    INX_H     = 0x23,
    INR_H     = 0x24,
    DCR_H     = 0x25,
    MVI_H_D8  = 0x26,
    DAA       = 0x27,
    DAD_H     = 0x29,
    LHLD_A16  = 0x2A,
    DCX_H     = 0x2B,
    INR_L     = 0x2C,
    DCR_L     = 0x2D,
    MVI_L_D8  = 0x2E,
    CMA       = 0x2F,
    LXI_SP_D16= 0x31,
    STA_A16   = 0x32,
    INX_SP    = 0x33,
    INR_M     = 0x34,
    DCR_M     = 0x35,
    MVI_M_D8  = 0x36,
    STC       = 0x37,
    DAD_SP    = 0x39,
    LDA_A16   = 0x3A,
    DCX_SP    = 0x3B,
    INR_A     = 0x3C,
    DCR_A     = 0x3D,
    MVI_A_D8  = 0x3E,
    CMC       = 0x3F,
    MOV_B_B   = 0x40,
    MOV_B_C   = 0x41,
    MOV_B_D   = 0x42,
    MOV_B_E   = 0x43,
    MOV_B_H   = 0x44,
    MOV_B_L   = 0x45,
    MOV_B_M   = 0x46,
    MOV_B_A   = 0x47,
    MOV_C_B   = 0x48,
    MOV_C_C   = 0x49,
    MOV_C_D   = 0x4A,
    MOV_C_E   = 0x4B,
    MOV_C_H   = 0x4C,
    MOV_C_L   = 0x4D,
    MOV_C_M   = 0x4E,
    MOV_C_A   = 0x4F,
    MOV_D_B   = 0x50,
    MOV_D_C   = 0x51,
    MOV_D_D   = 0x52,
    MOV_D_E   = 0x53,
    MOV_D_H   = 0x54,
    MOV_D_L   = 0x55,
    MOV_D_M   = 0x56,
    MOV_D_A   = 0x57,
    MOV_E_B   = 0x58,
    MOV_E_C   = 0x59,
    MOV_E_D   = 0x5A,
    MOV_E_E   = 0x5B,
    MOV_E_H   = 0x5C,
    MOV_E_L   = 0x5D,
    MOV_E_M   = 0x5E,
    MOV_E_A   = 0x5F,
    MOV_H_B   = 0x60,
    MOV_H_C   = 0x61,
    MOV_H_D   = 0x62,
    MOV_H_E   = 0x63,
    MOV_H_H   = 0x64,
    MOV_H_L   = 0x65,
    MOV_H_M   = 0x66,
    MOV_H_A   = 0x67,
    MOV_L_B   = 0x68,
    MOV_L_C   = 0x69,
    MOV_L_D   = 0x6A,
    MOV_L_E   = 0x6B,
    MOV_L_H   = 0x6C,
    MOV_L_L   = 0x6D,
    MOV_L_M   = 0x6E,
    MOV_L_A   = 0x6F,
    MOV_M_B   = 0x70,
    MOV_M_C   = 0x71,
    MOV_M_D   = 0x72,
    MOV_M_E   = 0x73,
    MOV_M_H   = 0x74,
    MOV_M_L   = 0x75,
    HLT       = 0x76,
    MOV_M_A   = 0x77,
    MOV_A_B   = 0x78,
    MOV_A_C   = 0x79,
    MOV_A_D   = 0x7A,
    MOV_A_E   = 0x7B,
    MOV_A_H   = 0x7C,
    MOV_A_L   = 0x7D,
    MOV_A_M   = 0x7E,
    MOV_A_A   = 0x7F,
    ADD_B     = 0x80,
    ADD_C     = 0x81,
    ADD_D     = 0x82,
    ADD_E     = 0x83,
    ADD_H     = 0x84,
    ADD_L     = 0x85,
    ADD_M     = 0x86,
    ADD_A     = 0x87,
    ADC_B     = 0x88,
    ADC_C     = 0x89,
    ADC_D     = 0x8A,
    ADC_E     = 0x8B,
    ADC_H     = 0x8C,
    ADC_L     = 0x8D,
    ADC_M     = 0x8E,
    ADC_A     = 0x8F,
    SUB_B     = 0x90,
    SUB_C     = 0x91,
    SUB_D     = 0x92,
    SUB_E     = 0x93,
    SUB_H     = 0x94,
    SUB_L     = 0x95,
    SUB_M     = 0x96,
    SUB_A     = 0x97,
    SBB_B     = 0x98,
    SBB_C     = 0x99,
    SBB_D     = 0x9A,
    SBB_E     = 0x9B,
    SBB_H     = 0x9C,
    SBB_L     = 0x9D,
    SBB_M     = 0x9E,
    SBB_A     = 0x9F,
    ANA_B     = 0xA0,
    ANA_C     = 0xA1,
    ANA_D     = 0xA2,
    ANA_E     = 0xA3,
    ANA_H     = 0xA4,
    ANA_L     = 0xA5,
    ANA_M     = 0xA6,
    ANA_A     = 0xA7,
    XRA_B     = 0xA8,
    XRA_C     = 0xA9,
    XRA_D     = 0xAA,
    XRA_E     = 0xAB,
    XRA_H     = 0xAC,
    XRA_L     = 0xAD,
    XRA_M     = 0xAE,
    XRA_A     = 0xAF,
    ORA_B     = 0xB0,
    ORA_C     = 0xB1,
    ORA_D     = 0xB2,
    ORA_E     = 0xB3,
    ORA_H     = 0xB4,
    ORA_L     = 0xB5,
    ORA_M     = 0xB6,
    ORA_A     = 0xB7,
    CMP_B     = 0xB8,
    CMP_C     = 0xB9,
    CMP_D     = 0xBA,
    CMP_E     = 0xBB,
    CMP_H     = 0xBC,
    CMP_L     = 0xBD,
    CMP_M     = 0xBE,
    CMP_A     = 0xBF,
    RNZ       = 0xC0,
    POP_B     = 0xC1,
    JNZ_A16   = 0xC2,
    JMP_A16   = 0xC3,
    CNZ_A16   = 0xC4,
    PUSH_B    = 0xC5,
    ADI_D8    = 0xC6,
    RST_0     = 0xC7,
    RZ        = 0xC8,
    RET       = 0xC9,
    JZ_A16    = 0xCA,
    CZ_A16    = 0xCC,
    CALL_A16  = 0xCD,
    ACI_D8    = 0xCE,
    RST_1     = 0xCF,
    RNC       = 0xD0,
    POP_D     = 0xD1,
    JNC_A16   = 0xD2,
    OUT_D8    = 0xD3,
    CNC_A16   = 0xD4,
    PUSH_D    = 0xD5,
    SUI_D8    = 0xD6,
    RST_2     = 0xD7,
    RC        = 0xD8,
    JC_A16    = 0xDA,
    IN_D8     = 0xDB,
    CC_A16    = 0xDC,
    SBI_D8    = 0xDE,
    RST_3     = 0xDF,
    RPO       = 0xE0,
    POP_H     = 0xE1,
    JPO_A16   = 0xE2,
    XTHL      = 0xE3,
    CPO_A16   = 0xE4,
    PUSH_H    = 0xE5,
    ANI_D8    = 0xE6,
    RST_4     = 0xE7,
    RPE       = 0xE8,
    PCHL      = 0xE9,
    JPE_A16   = 0xEA,
    XCHG      = 0xEB,
    CPE_A16   = 0xEC,
    XRI_D8    = 0xEE,
    RST_5     = 0xEF,
    RP        = 0xF0,
    POP_PSW   = 0xF1,
    JP_A16    = 0xF2,
    DI        = 0xF3,
    CP_A16    = 0xF4,
    PUSH_PSW  = 0xF5,
    ORI_D8    = 0xF6,
    RST_6     = 0xF7,
    RM        = 0xF8,
    SPHL      = 0xF9,
    JM_A16    = 0xFA,
    EI        = 0xFB,
    CM_A16    = 0xFC,
    CPI_D8    = 0xFE,
    RST_7     = 0xFF
} i8080_instruction_t;

enum i8080_restart_vector {
    RESTART_0 = 0x0000,
    RESTART_1 = 0x0008,
    RESTART_2 = 0x0010,
    RESTART_3 = 0x0018,
    RESTART_4 = 0x0020,
    RESTART_5 = 0x0028,
    RESTART_6 = 0x0030,
    RESTART_7 = 0x0038
};

typedef enum {
    INT_ENABLED,
    INT_ENABLED_NEXT,
    INT_DISABLED
} i8080_interrupt_state_t;

typedef struct {
    bool pending;
    uint8_t vec;
    i8080_interrupt_state_t status;
} i8080_interrupt_t;

typedef uint8_t (*i8080_reader_t)(void *, uint16_t);
typedef void (*i8080_writer_t)(void *, uint16_t, uint8_t);
typedef uint8_t (*i8080_input_t)(void *, uint8_t);
typedef void (*i8080_output_t)(void *, uint8_t, uint8_t);

typedef struct {
    bool cy   : 1;
    bool pad1 : 1;
    bool p    : 1;
    bool pad2 : 1;
    bool ac   : 1;
    bool pad3 : 1;
    bool z    : 1;
    bool s    : 1;
} i8080_flags_t;

typedef struct {
    union {
        struct {
            i8080_flags_t f;
            uint8_t a;
            uint8_t c;
            uint8_t b;
            uint8_t e;
            uint8_t d;
            uint8_t l;
            uint8_t h;
        };
        struct {
            uint16_t psw;
            uint16_t bc;
            uint16_t de;
            uint16_t hl;
        };
    };

    uint16_t pc;
    uint16_t sp;

    i8080_interrupt_t inte;
    bool hlt;

    size_t cycles;

    void *userptr;
    i8080_reader_t read_byte;
    i8080_writer_t write_byte;
    i8080_input_t in_byte;
    i8080_output_t out_byte;
} i8080_cpu_t;

void i8080_init(i8080_cpu_t *cpu,
                void *userptr,
                i8080_reader_t rb, 
                i8080_writer_t wb,
                i8080_input_t inb,
                i8080_output_t outb);
void i8080_step(i8080_cpu_t *cpu);
#ifdef DEBUG
void i8080_dump_registers(i8080_cpu_t *cpu);
void i8080_dump_memory(i8080_cpu_t *cpu, uint8_t page);
void i8080_dump_stack(i8080_cpu_t *cpu);
#endif 
void i8080_interrupt(i8080_cpu_t *cpu, i8080_instruction_t instr);