#ifndef NETWORK_H
#define NETWORK_H

#include "genome.h"

// Type arête
typedef struct {
    int in;         // Noeud d'entrée
    float weight;   // Poids de la connexion
} edge_t;

// Type neurone
typedef struct {
    edge_t *connections;     // Ensemble de connexions entrantes
    int connection_number;   // Nombre de connexions entrantes
    float activation;        // Activation
} neuron_t;

// Type réseau de neurones
typedef struct {
    neuron_t *neurons;                    // Ensemble de neurones
    int neuron_number;                    // Nombre de neurones
    int input_number;                     // Nombre de neurones d'entrée
    int output_number;                    // Nombre de neurones de sortie
    float (*activation_function)(float);  // Fonction d'activation
} network_t;

network_t *create_network(float (*activation)(float));
void free_network(network_t *network);

void build_network(genome_t *genome, network_t *network);
void feed_forward(network_t *network, float *inputs, float *outputs);

void save_network(network_t *network, char *file_name);
network_t *load_network(char *file_name);

#endif