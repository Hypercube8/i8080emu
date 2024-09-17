#include <stdint.h>

typedef enum {
    NOP       = 0x00;
    LXI_B_D16 = 0x01;
    STAX_B    = 0x02;
    INX_B     = 0x03;
} instruction_t;

typedef void (*i8080_reader_t)(uint16_t);
typedef void (*i8080_writer_t)(uint16_t, uint8_t);
typedef void (*i8080_input_t)(uint8_t);
typedef void (*i8080_output_t)(uint8_t, uint8_t);

typedef struct {

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