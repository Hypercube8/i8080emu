#include "i8080.h"
#include <stdlib.h>

uint8_t *memptr;
bool finished = false;
i8080_cpu_t cpu;

void load_file(const char *path) {
    FILE *f = fopen(path, "rb");

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    rewind(f);

    fread(&memptr[0x100], sizeof(uint8_t), length, f);
    fclose(f);
}

uint8_t rb(uint16_t addr) {
    return memptr[addr];
}

void wb(uint16_t addr, uint8_t val) {
    memptr[addr] = val;
}

uint8_t inb(uint8_t port) {
    return 0;
}

void outb(uint8_t port, uint8_t val) {
    switch (port) {
        case 0: {
            finished = true;
            break;
        }
        case 1:
            switch (cpu.c) {
                case 2: {
                    printf("%c", cpu.e);
                    break;
                }
                case 9: {
                    uint16_t addr = cpu.de;
                    do {
                        printf("%c", rb(addr++));
                    } while (rb(addr) != '$');
                    break;
                }
            };
    }
}

int main() {
    size_t ins;
    memptr = malloc(0x10000 * sizeof(uint8_t));
    load_file("roms/8080EXM.COM");
    memptr[0x0] = OUT_D8;
    memptr[0x1] = 0x00;
    memptr[0x5] = OUT_D8;
    memptr[0x6] = 0x01;
    memptr[0x7] = RET;
    
    i8080_init(&cpu, rb, wb, inb, outb);
    cpu.pc = 0x100;
    while (!cpu.hlt && !finished) {
        // #if __linux__
        //     system("clear");
        // #elif _WIN32
        //     system("cls");
        // #endif
        i8080_step(&cpu);
        // i8080_dump_registers(&cpu);
        //i8080_dump_memory(&cpu, 0x00);
        //i8080_dump_stack(&cpu);
        //fgetc(stdin);
        ins++;
    }
    printf("\nInstructions: %ld Cycles: %ld \n", ins, cpu.cycles);
    free(memptr);
    return 0;
}