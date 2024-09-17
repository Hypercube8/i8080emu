#include "i8080.h"




static void decode(i8080_cpu_t *cpu, instruction_t instr) {
    switch (instr) {
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
    }
}