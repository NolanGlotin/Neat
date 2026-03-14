#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "../include/neat.h"
#include "../include/speciation.h"
#include "../include/genome.h"
#include "../include/innovation.h"
#include "../include/mutation.h"
#include "../include/config.h"
#include "../include/utils.h"

// Intercepteur d'exceptions
jmp_buf jump_buffer;


// Algorithme neat
network_t *neat(int input_number, int output_number, float (*fitness_function)(network_t *), float *reached_fitness, char *log_path) {
    genome_t *next_population = NULL;
    genome_t *population = NULL;
    innovation_data_t *innovation = NULL;
    speciation_t *speciation = NULL;
    network_t *network = NULL;

    // Fichier journal
    FILE *logfile = fopen(log_path, "w");

    // Initialisation de l'intercepteur d'exceptions
    if (setjmp(jump_buffer) == 1) {
        printf("Aborting\n");
        fclose(logfile);
        free_innovation(innovation);
        free_speciation(speciation);
        free_network(network);
        if (population != NULL) {
            for (int id = 0; id < neat_config.population_size; id++)
                destroy_genome(&population[id]);
            free(population);
        }
        if (next_population != NULL) {
            for (int id = 0; id < neat_config.population_size; id++)
                destroy_genome(&next_population[id]);
            free(next_population);
        }
        if (reached_fitness != NULL)
            *reached_fitness = -1.0f;
        return NULL;
    }

    // Initialisation des structures de données
    population      = (genome_t *)malloc(neat_config.population_size*sizeof(genome_t));
    next_population = (genome_t *)malloc(neat_config.population_size*sizeof(genome_t));
    innovation      = init_innovation(MAX_NODES);
    speciation      = init_speciation(neat_config.population_size);
    network         = create_network(neat_config.network_activation_function);

    // Initialisation de la population
    for (int id = 0; id < neat_config.population_size; id++) {
        population[id] = init_genome(innovation, input_number, output_number);
        next_population[id] = create_genome();
    }
    
    // Boucle principalee
    for (int generation = 0; generation < neat_config.generation_number; generation++) {
        float best = 0.0f;
        float average_fitness = 0.0f;

        // Calcul de la fitness de chaque génome
        for (int id = 0; id < neat_config.population_size; id++) {
            build_network(&population[id], network);
            population[id].fitness = fitness_function(network);
            average_fitness += population[id].fitness;
            if (population[id].fitness >= best)
                best = population[id].fitness;
        }
        average_fitness /= neat_config.population_size;

        // Arrêt de la boucle si la fitness voulue est atteinte
        if (best >= neat_config.fitness_threshold) {
            fprintf(logfile, "%d, %f, %f, %d\n", generation, best, average_fitness, speciation->species_number);
            break;
        }
    
        // Spéciation
        compute_species(population, speciation);
        select_survivors(speciation);
        share_fitness(speciation);
        compute_offspring_number(speciation);

        // Création de la prochaine génération
        int current = 0;
        for (int s = 0; s < speciation->species_number; s++) {
            // Copie des meilleurs génomes (élites)
            for (int id = 0; id < speciation->species[s].elite_number; id++) {
                copy_genome(speciation->species[s].genomes[id], &next_population[current]);
                current++;
            }

            // Création des descendants avec les croisements
            for (int i = 0; i < speciation->species[s].offspring_number; i++) {
                // Sélection des deux parents
                genome_t *parent_1 = NULL; genome_t *parent_2 = NULL;
                if (speciation->species[s].size >= 2)
                    get_random_parents(&speciation->species[s], &parent_1, &parent_2);
                else {
                    parent_1 = speciation->species[s].genomes[0];
                    parent_2 = speciation->species[s].genomes[0];
                }
                // Croisement
                crossover(parent_1, parent_2, &next_population[current]);
                current++;
            }
        }
        ASSERT(current == neat_config.population_size);

        // Mutation de la nouvelle population
        for (int id = 0; id < neat_config.population_size; id++) {
            // Mutation d'un poids
            if (proba_rng(neat_config.mutation_perturb_weight_proba))
                mutation_mutate_weight(&next_population[id]);
            
            // Ajout d'une connexion
            if (proba_rng(neat_config.mutation_add_connection_proba))
                mutation_add_connection(&next_population[id], innovation);

            // Ajout d'un neurone
            if (proba_rng(neat_config.mutation_add_neuron_proba)) 
                mutation_add_node(&next_population[id], innovation);
            
            // Activation / désactivation d'une connexion
            if (proba_rng(neat_config.mutation_toggle_enabled_proba))
                mutation_toogle_enabled(&next_population[id]);
        }

        // Mise à jour de la population
        genome_t *tmp = population;
        population = next_population;
        next_population = tmp;

        // Enregistrement des informations de la génération
        fprintf(logfile, "%d, %f, %f, %d\n", generation, best, average_fitness, speciation->species_number);
    }

    // Recherche du meilleur génome de la population finale
    float max_fitness = 0.0f;
    genome_t *champion = NULL;
    for (int id = 0; id < neat_config.population_size; id++) {
        build_network(&population[id], network);
        float fitness = fitness_function(network);
        if (champion == NULL || fitness > max_fitness) {
            max_fitness = fitness;
            champion = &population[id];
        }
    }
    build_network(champion, network);

    // Libération de la mémoire
    free_innovation(innovation);
    free_speciation(speciation);
    for (int id = 0; id < neat_config.population_size; id++) {
        destroy_genome(&population[id]);
        destroy_genome(&next_population[id]);
    }
    free(population);
    free(next_population);
    fclose(logfile);

    // Mise à jour de la fitness finale
    if (reached_fitness != NULL)
        *reached_fitness = fitness_function(network);

    return network;
}