#include <stdio.h>

#include "../include/mutation.h"
#include "../include/utils.h"
#include "../include/config.h"

// Mutation : modification d'un poids de connexion
void mutation_mutate_weight(genome_t *genome) {
    ASSERT(genome->connection_number > 0);

    // Choix d'une connexion au hasard
    int connection = randint(0, genome->connection_number);
    
    if (proba_rng(neat_config.mutation_replace_weight_proba)) {
        // Remplacement du poids actuel par un nouveau poids aléatoire
        genome->connections[connection].weight = randfloat(-neat_config.network_weight_range, neat_config.network_weight_range);
    }
    else {
        // Modification du poids actuel en fonction du facteur de mutation
        genome->connections[connection].weight += randfloat(-neat_config.mutation_perturb_weight_factor, neat_config.mutation_perturb_weight_factor);
    }
}

// Mutation : ajout d'une connexion
void mutation_add_connection(genome_t *genome, innovation_data_t *innovation) {
    ASSERT(genome->connection_number < MAX_CONNECTIONS);
    
    // Choix de deux noeuds aléatoires jusqu'à ce qu'ils ne soient pas déjà connectés
    int try = 0;
    int in, out;
    do {
        try++;
        in = randint(0, genome->node_number);
        out = randint(0, genome->node_number);
        // Inversion de 'in' et 'out' si l'ordre topologique n'est pas respecté
        if (genome->nodes[in].depth > genome->nodes[out].depth) {
            int tmp = out;
            out = in;
            in = tmp;
        }
    } while (try < neat_config.mutation_new_connection_tries && (in == out || out < genome->input_number || connection_exists(genome, in, out)));

    // Si deux noeuds ont été trouvés, ajout de la connexion
    if (try < neat_config.mutation_new_connection_tries) {
        float weight = randfloat(-neat_config.network_weight_range, neat_config.network_weight_range);
        genome_add_connection(genome, innovation, in, out, weight);
    }
}

// Mutation : suppression d'une connexion
void mutation_remove_connection(genome_t *genome) {
    int connection = randint(0, genome->connection_number);
    genome_delete_connection(genome, connection);
}

// Mutation : ajout d'un noeud
void mutation_add_node(genome_t *genome, innovation_data_t *innovation) {
    ASSERT(genome->node_number < MAX_NODES);
    ASSERT(genome->connection_number < MAX_CONNECTIONS - 1);

    // Choix d'une connexion aléatoire activée
    int connection;
    int try = 0;
    do {
        try++;
        connection = randint(0, genome->connection_number);
    }
    while (try < neat_config.mutation_new_node_tries && !genome->connections[connection].enabled);

    // Insertion du nouveau noeud sur cette connexion
    if (genome->connections[connection].enabled)
        genome_add_node(genome, innovation, connection);

}

// Mutation : suppression d'un noeud
void mutation_remove_node(genome_t *genome) {
    int node = randint(0, genome->node_number);
    genome_delete_node(genome, node);
}

// Mutation : activation / désactivation d'un noeud
void mutation_toogle_enabled(genome_t *genome) {
    int connection = randint(0, genome->connection_number);
    genome_toogle_enabled(genome, connection);
}