#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "../include/speciation.h"
#include "../include/utils.h"
#include "../include/config.h"

#define MAX_SPECIES 500

// Initialisation de la spéciation
speciation_t *init_speciation(int pop_size) {
    speciation_t *speciation    = (speciation_t *)malloc(sizeof(speciation_t));
    speciation->species         = (species_t *)malloc(sizeof(species_t)*MAX_SPECIES);
    speciation->species_number  = 0;
    speciation->population_size = pop_size;
    speciation->average_fitness = 0.0f;

    for (int s = 0; s < MAX_SPECIES; s++) {
        speciation->species[s].size              = 0;
        speciation->species[s].offspring_number  = 0;
        speciation->species[s].elite_number      = 0;
        speciation->species[s].total_fitness     = 0.0f;
        speciation->species[s].representative    = NULL;
        speciation->species[s].genomes           = (genome_t **)malloc(pop_size*sizeof(genome_t *));
        speciation->species[s].last_best_fitness = 0.0f;
        speciation->species[s].stagnation        = 0;
        for (int id = 0; id < pop_size; id++)
            speciation->species[s].genomes[id] = NULL;
    }

    return speciation;
}

// Libération de la spéciation
void free_speciation(speciation_t *speciation) {
    if (speciation != NULL) {
        for (int s = 0; s < MAX_SPECIES; s++) {
            free(speciation->species[s].genomes);
        }
        free(speciation->species);
        free(speciation);
    }
}

// Calcul de la distance entre deux génomes
float compute_compatibility(genome_t *genome_1, genome_t *genome_2) {
    int i1 = 0; int i2 = 0;
    int matching = 0; int disjoint = 0; int excess = 0;
    float average_weight_diff = 0.0f;

    // Calcul du nombre de gènes disjoints, en excès et de la moyenne des différences de poids
    while (i1 < genome_1->connection_number || i2 < genome_2->connection_number) {
        // Gène en excès
        if (i1 >= genome_1->connection_number) {
            excess++;
            i2++;
        }
        else if (i2 >= genome_2->connection_number) {
            excess++;
            i1++;
        }
        // Gène disjoint
        else if (genome_1->connections[i1].innovation_number > genome_2->connections[i2].innovation_number) {
            disjoint++;
            i2++;
        }
        else if (genome_1->connections[i1].innovation_number < genome_2->connections[i2].innovation_number) {
            disjoint++;
            i1++;
        }
        // Gène coïncidant
        else if (genome_1->connections[i1].innovation_number == genome_2->connections[i2].innovation_number) {
            matching++;
            average_weight_diff += fabsf(genome_1->connections[i1].weight - genome_2->connections[i2].weight);
            i1++;
            i2++;
        }
    }
    if (matching > 0)
        average_weight_diff /= (float)matching;

    // Calcul de la distance de compatibilité
    int N = max(genome_1->connection_number, genome_2->connection_number);
    if (N < 20) N = 1;   // Pour des petits génomes, on fixe N à 1

    float delta =
        neat_config.speciation_c1*(float)excess/(float)N +
        neat_config.speciation_c2*(float)disjoint/(float)N +
        neat_config.speciation_c3*average_weight_diff;
    return delta;
}

// Formation des espèces
void compute_species(genome_t *population, speciation_t *speciation) {
    // Réinitialisation de chaque espèce
    speciation->species_number = 0;
    for (int s = 0; s < MAX_SPECIES; s++)
        speciation->species[s].size = 0;

    // Attribution d'une espèce à chaque génome
    for (int id = 0; id < speciation->population_size; id++) {
        float delta;
        int s = -1;
        do {
            s++;
            if (s >= speciation->species_number) {
                // Ce génome ne correspond à aucune espèce existante, donc on en crée une nouvelle
                ASSERT(s < MAX_SPECIES);
                speciation->species[s].representative = &population[id];
                speciation->species_number++;
                break;
            }
            // Calcul de la compatibilité avec le représentant de l'espèce n° s
            delta = compute_compatibility(&population[id], speciation->species[s].representative);
        } while (delta > neat_config.speciation_compatibility_treshold);
        
        // Ajout du génome à l'espèce désignée
        speciation->species[s].genomes[speciation->species[s].size] = &population[id];
        speciation->species[s].size++;
    }
}

// Sélections des génomes aptes à la reproduction
void select_survivors(speciation_t *speciation) {
    for (int s = 0; s < speciation->species_number; s++) {
        qsort((void *)speciation->species[s].genomes, speciation->species[s].size, sizeof(genome_t *), compare_fitness);
        speciation->species[s].size = (int)ceilf((float)speciation->species[s].size*neat_config.survival_rate);
    }
}

// Partage de la fitness dans chaque espèce
void share_fitness(speciation_t *speciation) {
    speciation->average_fitness = 0.0f;
    for (int s = 0; s < speciation->species_number; s++) {
        float best_fitness = 0.0f;
        speciation->species[s].total_fitness = 0.0f;
        for (int id = 0; id < speciation->species[s].size; id++) {
            if (speciation->species[s].genomes[id]->fitness > best_fitness)
                best_fitness = speciation->species[s].genomes[id]->fitness;
            // Division de la fitness par la taille de l'espèce
            speciation->species[s].genomes[id]->fitness /= (float)speciation->species[s].size;
            speciation->species[s].total_fitness += speciation->species[s].genomes[id]->fitness;
        }
        // Stagnation
        if (best_fitness <= speciation->species[s].last_best_fitness)
            speciation->species[s].stagnation += 1;
        speciation->species[s].last_best_fitness = best_fitness;

        if (speciation->species[s].stagnation < neat_config.speciation_stagnation_threshold)
            speciation->average_fitness += speciation->species[s].total_fitness;
    }
    speciation->average_fitness /= speciation->population_size;
}

// Calcul du nombre de descendants attribuées à chaque espèce
void compute_offspring_number(speciation_t *speciation) {
    int total = 0;
    for (int s = 0; s < speciation->species_number; s++) {
        if (speciation->species[s].stagnation < neat_config.speciation_stagnation_threshold) {
            // Calcul de la taille de l'espèce dans la prochaine génération
            int next_size = (
                speciation->species[s].total_fitness/
                speciation->average_fitness
            );

            // Calcul du nombres d'élites et de descendants
            int elites = (int)(neat_config.elitism*(float)next_size);
            if (elites > speciation->species[s].size)
                elites = speciation->species[s].size;
            int offsprings = next_size - elites;

            speciation->species[s].elite_number = elites;
            speciation->species[s].offspring_number = offsprings;
            total += next_size;
        }
        else {
            // Cette espèce a atteint le seuil de stagnation, elle s'éteint
            speciation->species[s].elite_number = 0;
            speciation->species[s].offspring_number = 0;
            speciation->species[s].last_best_fitness = 0.0f;
            speciation->species[s].stagnation = 0;
        }
    }

    // Ajustement éventuel du nombre de descendants pour garder une taille de population constante
    if (total < speciation->population_size)
        speciation->species[randint(0, speciation->species_number)].offspring_number += speciation->population_size - total;
}

// Choix aléatoire de deux parents pondéré par leur fitness
void get_random_parents(species_t *species, genome_t **parent_1, genome_t **parent_2) {
    ASSERT(species->size >= 2);

    int id1 = 0; int id2 = 0;

    // Sélection du premier parent
    float random = randfloat(0.0f, 1.0f);
    for (int id = 0; id < species->size; id++) {
        // Calcul de la probabilité de choisir ce génome
        float proba = species->genomes[id]->fitness/species->total_fitness;
        if (random < proba) {
            id1 = id;
            break;
        }
        random -= proba;
    }

    // Sélection du deuxième parent
    float adjusted_total = species->total_fitness - species->genomes[id1]->fitness;
    random = randfloat(0.0f, 1.0f);
    for (int id = 0; id < species->size; id++) {
        // Vérification qu'il ne s'agit pas du même génome
        if (id != id1) {
            // Calcul de la probabilité de choisir ce génome
            float proba = species->genomes[id]->fitness/adjusted_total;
            if (random < proba) {
                id2 = id;
                break;
            }
            random -= proba;
        }
    }

    // Renvoi des parents choisis
    *parent_1 = species->genomes[id1];
    *parent_2 = species->genomes[id2];
}