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

uint8_t rb(void *userptr, uint16_t addr) {
    return memptr[addr];
}

void wb(void *userptr, uint16_t addr, uint8_t val) {
    memptr[addr] = val;
}

uint8_t inb(void *userptr, uint8_t port) {
    return 0;
}

void outb(void *userptr, uint8_t port, uint8_t val) {
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
                        printf("%c", rb(NULL, addr++));
                    } while (rb(NULL, addr) != '$');
                    break;
                }
            };
    }
}

int main() {
    size_t ins;
    memptr = malloc(0x10000 * sizeof(uint8_t));
    load_file("roms/8080EXM.COM");
    memptr[RESTART_0+0] = OUT_D8;
    memptr[RESTART_0+1] = 0x00;
    memptr[RESTART_0+5] = OUT_D8;
    memptr[RESTART_0+6] = 0x01;
    memptr[RESTART_0+7] = RET;
    
    i8080_init(&cpu, &cpu, rb, wb, inb, outb);
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