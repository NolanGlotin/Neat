#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <setjmp.h>
#include <stdio.h>

// Interception des exceptions levées par ASSERT
extern jmp_buf jump_buffer;
#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("Assert failed in %s file %s line %d\n", __func__, __FILE__, __LINE__); \
        longjmp(jump_buffer, 1); \
    } \
} while(0)

// Fonctions aléatoires
int randint(int min, int max);
float randfloat(float min, float max);
bool proba_rng(float proba);

// Fonctions d'activation
float sigmoid(float x);
float hyperbolic_tangent(float x);
float relu(float x);

// Max / Min
int max(int a, int b);
int min(int a, int b);

#endif