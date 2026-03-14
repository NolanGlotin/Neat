#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/utils.h"

// Entier aléatoire entre 'min' (inclus) et 'max' (non inclus)
int randint(int min, int max) {
    ASSERT(max > min);
    return rand()%(max - min) + min;
}

// Flottant aléatoire entre 'min' et 'max'
float randfloat(float min, float max) {
    ASSERT(max >= min);
    return (float)rand()/(float)RAND_MAX*(max - min) + min;
}

// Tirage de 'true' avec la probabilité 'proba'
bool proba_rng(float proba) {
    ASSERT(proba >= 0.0f && proba <= 1.0f);
    return randfloat(0.0f, 1.0f) < proba;
}

// Fonctions d'activation usuelles
float sigmoid(float x) {
    return 1.0f/(1.0f + expf(-x));
}

float hyperbolic_tangent(float x) {
    return tanhf(x);
}

float relu(float x) {
    return x < 0.0f ? 0.0f : x;
}


// Fonctions maximum / minimum
int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}