#include "i8080.h"
#include <stdlib.h>
#include <time.h>

enum bdos_syscalls {
    P_TERMCPM = 0x00,
    C_WRITE = 0x01,
    C_WRITECHAR = 0x02,
    C_WRITESTR = 0x09
};

typedef struct {
    size_t ins;
    size_t cycles;
    double time;
    double mips;
    double mhz;
} test_results_t;

uint8_t *memptr;
bool finished = false;
i8080_cpu_t cpu;

void load_file(const char *path) {
    FILE *f = fopen(path, "rb");

    if (f == NULL) {
        perror("Failed to open file");
        return;
    }

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
        case P_TERMCPM: {
            finished = true;
            break;
        }
        case C_WRITE:
            switch (cpu.c) {
                case C_WRITECHAR: {
                    printf("%c", cpu.e);
                    break;
                }
                case C_WRITESTR: {
                    uint16_t addr = cpu.de;
                    do {
                        printf("%c", rb(NULL, addr++));
                    } while (rb(NULL, addr) != '$');
                    break;
                }
            };
    }
}

void log_results(const char* test_name, test_results_t *results) {
    printf("\nTEST %s RESULTS", test_name);
    printf("\n\tInstructions Executed: %ld", results->ins);
    printf("\n\tCycles Executed: %ld", results->cycles);
    printf("\n\tTime Elapsed: %f", results->time);
    printf("\n\tSpeed (MIPS): %f", results->mips);
    printf("\n\tSpeed (MHz): %f", results->mhz);
}

void run_test(const char *path) {
    test_results_t results = {0};
    printf("\nBEGINNING TEST %s\n", path);
    finished = false;

    memptr = malloc(0x10000 * sizeof(uint8_t));
    if (memptr == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    load_file(path);

    memptr[RESTART_0+0] = OUT_D8;
    memptr[RESTART_0+1] = 0x00;
    memptr[RESTART_0+5] = OUT_D8;
    memptr[RESTART_0+6] = 0x01;
    memptr[RESTART_0+7] = RET;
    i8080_init(&cpu, &cpu, rb, wb, inb, outb);
    cpu.pc = 0x100;

    clock_t begin = clock();
    while (!cpu.hlt && !finished) {
        i8080_step(&cpu);
        results.ins += 1;
    }
    clock_t end = clock();

    results.time = (double)(end - begin) / CLOCKS_PER_SEC;
    results.mips = (results.ins / results.time) / 1000000;
    results.cycles = cpu.cycles;
    results.mhz = (results.cycles / results.time) / 1000000;

    free(memptr);
    log_results(path, &results);
}

int main() {
    run_test("roms/8080PRE.COM");
    run_test("roms/TST8080.COM");
    run_test("roms/CPUTEST.COM");
    run_test("roms/8080EXM.COM");
    return 0;
}