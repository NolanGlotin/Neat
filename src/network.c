#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#include "../include/network.h"
#include "../include/config.h"

// Création d'un réseau de neurones
network_t *create_network(float (*activation)(float)) {
    network_t *network = (network_t *)malloc(sizeof(network_t));
    network->neuron_number = 0;
    network->input_number  = 0;
    network->output_number = 0;
    network->neurons = (neuron_t *)malloc(sizeof(neuron_t)*MAX_NODES);
    network->activation_function = activation;

    for (int id = 0; id < MAX_NODES; id++) {
        network->neurons[id].activation = 0.0f;
        network->neurons[id].connection_number = 0;
        network->neurons[id].connections = (edge_t *)malloc(sizeof(edge_t)*MAX_CONNECTIONS);
    }
    return network;
}

// Libération d'un réseau de neurones
void free_network(network_t *network) {
    if (network == NULL)
        return;
    for (int id = 0; id < MAX_NODES; id++)
        free(network->neurons[id].connections);
    free(network->neurons);
    free(network);
}

// Création d'un réseau de neurones à partir d'un génome
void build_network(genome_t *genome, network_t *network) {
    assert(network != NULL);

    // Initialisation du réseau
    network->neuron_number = genome->node_number;
    network->input_number  = genome->input_number;
    network->output_number = genome->output_number;
    for (int n = 0; n < genome->node_number; n++) {
        network->neurons[n].activation = 0.0f;
        network->neurons[n].connection_number = 0;
    }

    // Construction de chaque connexion du génome
    for (int c = 0; c < genome->connection_number; c++)
        if (genome->connections[c].enabled) {
            connection_t connection = genome->connections[c];
            int in = genome->nodes[connection.in].depth;
            int out = genome->nodes[connection.out].depth;
            network->neurons[out].connections[network->neurons[out].connection_number] = (edge_t){ .in = in, .weight = connection.weight };
            network->neurons[out].connection_number++;
        }
}

// Propagation avant
void feed_forward(network_t *network, float *inputs, float *outputs) {
    assert(network != NULL);

    // Injection des entrées
    for (int in = 0; in < network->input_number; in++)
        network->neurons[in].activation = inputs[in];

    // Initialisation de toutes les activations à 0
    for (int id = network->input_number; id < network->neuron_number; id++)
        network->neurons[id].activation = 0.0f;

    // Propagation de l'information
    for (int id = network->input_number; id < network->neuron_number; id++) {
        // Somme pondérée des activations entrantes
        float activation = 0.0f;
        for (int in = 0; in < network->neurons[id].connection_number; in++) {
            int in_id = network->neurons[id].connections[in].in;
            activation += network->neurons[id].connections[in].weight*network->neurons[in_id].activation;
        }
        // Composition par la fonction d'activation
        network->neurons[id].activation = network->activation_function(activation);
    }

    // Sauvegarde des sorties
    for (int out = 0; out < network->output_number; out++)
        outputs[out] = network->neurons[network->neuron_number - network->output_number + out].activation;
}

// Sauvegarde d'un réseau de neurones dans un fichier
void save_network(network_t *network, char *file_name) {
    assert(network != NULL);

    // Chargement du fichier
    FILE *file = fopen(file_name, "w");
    assert(file != NULL);

    // Ecriture du nombre de neurones
    fprintf(file, "total %d | inputs %d | outputs %d\n", network->neuron_number, network->input_number, network->output_number);
    for (int id = network->input_number; id < network->neuron_number; id++) {
        // Ecriture du nombre de connexions
        fprintf(file, "%d", network->neurons[id].connection_number);
        for (int connection = 0; connection < network->neurons[id].connection_number; connection++)
            // Ecriture de l'id du neurone entrant et du poids de la connexion
            fprintf(file, " | %d : %f", network->neurons[id].connections[connection].in, network->neurons[id].connections[connection].weight);
        fprintf(file, "\n");
    }

    // Fermeture du fichier
    fclose(file);
}

// Chargement d'un réseau de neurones depuis un fichier
network_t *load_network(char *file_name) {
    // Chargement du fichier
    FILE *file = fopen(file_name, "r");
    assert(file != NULL);

    // Chargement du nombre de neurones
    int neuron_number, input_number, output_number;
    fscanf(file, "total %d | inputs %d | outputs %d\n", &neuron_number, &input_number, &output_number);

    // Création d'un réseau vide
    network_t *network = create_network(neat_config.network_activation_function);
    network->neuron_number = neuron_number;
    network->input_number = input_number;
    network->output_number = output_number;
    assert(neuron_number < MAX_NODES);

    // Construction du réseau
    for (int id = input_number; id < neuron_number; id++) {
        int connection_number;
        fscanf(file, "%d", &connection_number);
        network->neurons[id].connection_number = connection_number;
        for (int c = 0; c < connection_number; c++) {
            int in;
            float weight;
            fscanf(file, " | %d : %f", &in, &weight);
            network->neurons[id].connections[c] = (edge_t){ .in = in, .weight = weight };
        }
    }

    // Fermeture du fichier
    fclose(file);
    
    return network;
}