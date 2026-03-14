#ifndef SPECIATION_H
#define SPECIATION_H

#include "genome.h"

// Type espèce
typedef struct {
    genome_t **genomes;           // Ensemble de génomes de l'espèce
    int size;                     // Taille de l'espèce
    float total_fitness;          // Fitness totale de l'espèce
    genome_t *representative;     // Représentant de l'espèce
    int offspring_number;         // Nombre de progénitures attribuées
    int elite_number;             // Nombre d'élites attribuées
    float last_best_fitness;      // Meilleure fitness de la génération précédente
    int stagnation;               // Nombre de générations pendant lesquelles la fitness n'a pas augmenté
} species_t;

// Type spéciation
typedef struct {
    species_t *species;       // Ensemble d'espèces
    int species_number;       // Nombre d'espèces
    float average_fitness;    // Fitness moyenne de la population
    int population_size;      // Taille de la population
} speciation_t;

speciation_t *init_speciation(int pop_size);
void free_speciation(speciation_t *speciation);

void compute_species(genome_t *population, speciation_t *speciation);
void select_survivors(speciation_t *speciation);
void share_fitness(speciation_t *speciation);
void compute_offspring_number(speciation_t *speciation);
void get_random_parents(species_t *species, genome_t **parent_1, genome_t **parent_2);

#endif