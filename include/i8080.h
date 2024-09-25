#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

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
    RAR       = 0x1F
} instruction_t;

typedef uint8_t (*i8080_reader_t)(uint16_t);
typedef void (*i8080_writer_t)(uint16_t, uint8_t);
typedef uint8_t (*i8080_input_t)(uint8_t);
typedef void (*i8080_output_t)(uint8_t, uint8_t);

typedef struct {
    bool s    : 1;
    bool z    : 1;
    bool pad1 : 1;
    bool ac   : 1;
    bool pad2 : 1;
    bool p    : 1;
    bool pad3 : 1;
    bool cy   : 1;
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

    i8080_reader_t read_byte;
    i8080_writer_t write_byte;
    i8080_input_t in_byte;
    i8080_output_t out_byte;
} i8080_cpu_t;