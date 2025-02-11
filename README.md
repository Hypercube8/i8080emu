# Intel 8080 Emulator

## Overview
    
This project implements an **I8080A** emulator in C. It handles all 256 *opcodes*, including *illegal* ones. It also counts *cycles* and has an *interrupt* system. 

## Usage

### Setup

Include `<i8080.h>` and create a new instance of `i8080_cpu_t`. Then call `i8080_init` in order to initalize the processor.

```
#include <i8080.h>

i8080_cpu_t cpu;
i8080_init(&cpu, &cpu, rb, wb, inb, outb);
```


### User Defined Functions

To allow for the most flexibility, the behaviour of *memory* and *I/O* are left up to the user. Define **read byte**, **write byte**, **input byte**, and **output byte** functions. Then give these functions to the emulator by passing them as pointers to `i8080_init`.

```
uint8_t rb(void *userptr, uint16_t addr) {
    // implement read byte
}

void wb(void *userptr, uint16_t addr, uint8_t val) {
    // implement write byte
}

uint8_t inb(void *userptr, uint8_t port) {
    // implement in byte
}

void outb(void *userptr, uint8_t port, uint8_t val) {
    // implement out byte
}

// pass the function pointers
i8080_init(&cpu, /*userdata*/, rb, wb, inb, outb);
```

The second arguement of `i8080_init` can be used to input *user defined data* that can be acessed by *user functions*.

### Running

Call the `i8080_step` function to run the emulator. You can check if the processor has *halted* by acessing `cpu.hlt`.

```
while (!cpu.hlt) {
    i8080_step(&cpu);
}
```

### Interrupts

The **I8080A** implements an interrupt system. To request an interrupt, call the `i8080_interrupt` function with the *opcode* of the *instruction* you would like to be called (usually this is a **RST** *instruction*).

```
#include <i8080.h>

i8080_interrupt(&cpu, RST_0);
```

## Cycles

*Cycles* are useful for timing purposes. This emulator is *cycle* accurate. The amount of total *cycles* that have been executed are stored in `cpu.cycles`.

### Debugging

You may want to see the values of the *registers* and *memory* for debugging purposes. This emulator provides the following functions:

- `i8080_dump_registers`
    - Prints the values of registers and register pairs
- `i8080_dump_stack`
    - Prints the first ten values on the stack
- `i8080_dump_memory`
    - Prints a page of memory (256 bytes)
    - The second arguement decides the page of memory to be printed

## Accuracy

This emulator should be accurate enough for most applications. It is capable of executing all **8080** instructions, including *illegal* *opcodes*. It implements *cycle* counting for more precise timings. 

### Tests

Here are the *tests* use to verify the behavior of the emulator:

- [x] Preliminary Z80 Tests by Frank D. Cringle (modified for Intel 8080 by Ian Bartholomew)
- [x] 8080/8085 CPU Diagnostic by Microcosm Associates 
- [x] Supersoft Associates CPU Test
- [x] 8080/8085 Exerciser "Modified"

### Results

This is the listing of the *results* from running `test.c`, which goes through each *test* suite.

```
BEGINNING TEST roms/8080PRE.COM
8080 Preliminary tests complete
TEST roms/8080PRE.COM RESULTS
        Instructions Executed: 1061
        Cycles Executed: 7817
        Time Elapsed: 0.000012
        Speed (MIPS): 88.416667
        Speed (MHz): 651.416667
BEGINNING TEST roms/TST8080.COM
MICROCOSM ASSOCIATES 8080/8085 CPU DIAGNOSTIC
 VERSION 1.0  (C) 1980

 CPU IS OPERATIONAL
TEST roms/TST8080.COM RESULTS
        Instructions Executed: 651
        Cycles Executed: 4924
        Time Elapsed: 0.000042
        Speed (MIPS): 15.500000
        Speed (MHz): 117.238095
BEGINNING TEST roms/CPUTEST.COM

DIAGNOSTICS II V1.2 - CPU TEST
COPYRIGHT (C) 1981 - SUPERSOFT ASSOCIATES

ABCDEFGHIJKLMNOPQRSTUVWXYZ
CPU IS 8080/8085
BEGIN TIMING TEST
END TIMING TEST
CPU TESTS OK

TEST roms/CPUTEST.COM RESULTS
        Instructions Executed: 33971311
        Cycles Executed: 255653383
        Time Elapsed: 0.284545
        Speed (MIPS): 119.388185
        Speed (MHz): 898.463804
BEGINNING TEST roms/8080EXM.COM
8080 instruction exerciser
dad <b,d,h,sp>................  PASS! crc is:14474ba6
aluop nn......................  PASS! crc is:9e922f9e
aluop <b,c,d,e,h,l,m,a>.......  PASS! crc is:cf762c86
<daa,cma,stc,cmc>.............  PASS! crc is:bb3f030c
<inr,dcr> a...................  PASS! crc is:adb6460e
<inr,dcr> b...................  PASS! crc is:83ed1345
<inx,dcx> b...................  PASS! crc is:f79287cd
<inr,dcr> c...................  PASS! crc is:e5f6721b
<inr,dcr> d...................  PASS! crc is:15b5579a
<inx,dcx> d...................  PASS! crc is:7f4e2501
<inr,dcr> e...................  PASS! crc is:cf2ab396
<inr,dcr> h...................  PASS! crc is:12b2952c
<inx,dcx> h...................  PASS! crc is:9f2b23c0
<inr,dcr> l...................  PASS! crc is:ff57d356
<inr,dcr> m...................  PASS! crc is:92e963bd
<inx,dcx> sp..................  PASS! crc is:d5702fab
lhld nnnn.....................  PASS! crc is:a9c3d5cb
shld nnnn.....................  PASS! crc is:e8864f26
lxi <b,d,h,sp>,nnnn...........  PASS! crc is:fcf46e12
ldax <b,d>....................  PASS! crc is:2b821d5f
mvi <b,c,d,e,h,l,m,a>,nn......  PASS! crc is:eaa72044
mov <bcdehla>,<bcdehla>.......  PASS! crc is:10b58cee
sta nnnn / lda nnnn...........  PASS! crc is:ed57af72
<rlc,rrc,ral,rar>.............  PASS! crc is:e0d89235
stax <b,d>....................  PASS! crc is:2b0471e9
Tests complete
TEST roms/8080EXM.COM RESULTS
        Instructions Executed: 2919050698
        Cycles Executed: 23803381171
        Time Elapsed: 23.672660
        Speed (MIPS): 123.308944
        Speed (MHz): 1005.522031
```

## Resources

Here is a list of *resources* used to make this emulator:

### Datasheets

- [The 8080 Central Processor Unit](https://deramp.com/downloads/intel/8080%20Data%20Sheet.pdf)

- [8080 Assembly Language Programming Manual](https://altairclone.com/downloads/manuals/8080%20Programmers%20Manual.pdf)

### Tables

- [Tobias V. Langhoff's Intel 8080/8085 opcode table](https://tobiasvl.github.io/optable/intel-8080/)

- [Intel 8080 instruction set](https://pastraiser.com/cpu/i8080/i8080_opcodes.html)

### Emulators

- [8080 by superzazu](https://github.com/superzazu/8080/blob/master/README.md)
- [m8080 by Pedro Minicz](https://github.com/pedrominicz/m8080/blob/master/m8080.h)
- [Intel 8080 Emulator by nav97](https://github.com/nav97/Intel-8080-Emulator/blob/master/i8080.c) 

### Other

- [Intel 8080](https://en.wikipedia.org/wiki/Intel_8080)