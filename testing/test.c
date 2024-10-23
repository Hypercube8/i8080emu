#include "i8080.h"

uint8_t *memptr;

void load_file(const char *path) {
    FILE *f = fopen(path, "rb");

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    rewind(f);

    fread(memptr, sizeof(uint8_t), length, f);
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
    return;
}

int main() {
    memptr = malloc(0x10000 * sizeof(uint8_t));
    load_file("roms/test.rom");
    i8080_cpu_t cpu;
    i8080_init(&cpu, rb, wb, inb, outb);
    i8080_dump(&cpu, 0x00);
    while (!cpu.hlt) {
        i8080_step(&cpu);
    }
    i8080_debug(&cpu);
    free(memptr);
    return 0;
}