#ifndef GENOME_H
#define GENOME_H

#include <stdbool.h>
#include "innovation.h"

#define MAX_NODES 30
#define MAX_CONNECTIONS 100

// Type noeud
typedef struct {
    int depth;  // Profondeur du noeud
} node_t;

// Type connexion
typedef struct {
    int in;                 // Noeud d'entrée
    int out;                // Noeud de sortie
    float weight;           // Poids
    bool enabled;           // Drapeau d'activation
    int innovation_number;  // Numéro d'innovation
} connection_t;

// Type génome
typedef struct {
    int node_number;            // Nombre de noeuds
    node_t *nodes;              // Ensemble de noeuds
    int connection_number;      // Nombre de connexions
    connection_t *connections;  // Ensemble de connexions
    int input_number;           // Nombre de neurones d'entrée
    int output_number;          // Nombre de neurones de sortie
    float fitness;              // Fitness
} genome_t;

genome_t create_genome(void);
genome_t init_genome(innovation_data_t *innovation, int input_number, int output_number);
void destroy_genome(genome_t *genome);
void copy_genome(genome_t *src, genome_t *dest);
int compare_fitness(const void *genome_1, const void *genome_2);
bool connection_exists(genome_t *genome, int in, int out);

void genome_add_connection(genome_t *genome, innovation_data_t *innovation, int in, int out, float weight);
void genome_delete_connection(genome_t *genome, int connection);
void genome_add_node(genome_t *genome, innovation_data_t *innovation, int connection);
void genome_delete_node(genome_t *genome, int node_id);
void genome_toogle_enabled(genome_t *genome, int connection);
void crossover(genome_t *parent_1, genome_t *parent_2, genome_t *child);

void display_genome(genome_t *genome);

#endif