#include <stdlib.h>
#include <stdio.h>

#include "../include/genome.h"
#include "../include/utils.h"
#include "../include/config.h"

// Affichage d'un génome
void display_genome(genome_t *genome) {
    printf("[ %d conections : ", genome->connection_number);
    for (int c = 0; c < genome->connection_number; c++) {
        printf("(%d, %d, inno %d, w %f", genome->connections[c].in, genome->connections[c].out, genome->connections[c].innovation_number, genome->connections[c].weight);
        if (genome->connections[c].enabled == false) printf(" [disabled] )");
        else printf(" )");
    }
    printf(" | %d nodes : ", genome->node_number);
    for (int n = 0; n < genome->node_number; n++)
        printf("(id %d, depth %d) ", n, genome->nodes[n].depth);
    printf("]\n");
}

// Création d'un nouveau génome
genome_t create_genome(void) {
    genome_t genome          = (genome_t){0};
    genome.nodes             = (node_t *)malloc(sizeof(node_t)*MAX_NODES);
    genome.connections       = (connection_t *)malloc(sizeof(connection_t)*MAX_CONNECTIONS);
    genome.node_number       = 0;
    genome.connection_number = 0;
    genome.input_number      = 0;
    genome.output_number     = 0;
    genome.fitness           = 0.0f;

    return genome;
}

// Initialisation d'un génome avec des neurones d'entrée et de sortie
genome_t init_genome(innovation_data_t *innovation, int input_number, int output_number) {
    ASSERT(input_number + output_number < MAX_NODES);
    ASSERT(input_number*output_number < MAX_CONNECTIONS);

    // Création du génome
    genome_t genome      = create_genome();
    genome.node_number   = input_number + output_number;
    genome.input_number  = input_number;
    genome.output_number = output_number;

    // Création des noeuds initiaux
    for (int id = 0; id < input_number + output_number; id++)
        genome.nodes[id] = (node_t){ .depth = id };

    // Création des connexions initiales
    int c = 0;
    for (int in = 0; in < input_number; in++)
        for (int out = 0; out < output_number; out++) {
            genome_add_connection(&genome, innovation, in, input_number + out, randfloat(neat_config.network_weight_range, neat_config.network_weight_range));
            c++;
        }

    return genome;
}

// Destruction d'un génome
void destroy_genome(genome_t *genome) {
    if (genome != NULL) {
        free(genome->nodes);
        free(genome->connections);
    }
}

// Copie du génome 'src' vers le génome 'dest'
void copy_genome(genome_t *src, genome_t *dest) {
    dest->node_number       = src->node_number;
    dest->connection_number = src->connection_number;
    dest->input_number      = src->input_number;
    dest->output_number     = src->output_number;
    dest->fitness           = src->fitness;
    for (int n = 0; n < src->node_number; n++)
        dest->nodes[n] = src->nodes[n];
    for (int c = 0; c < src->connection_number; c++)
        dest->connections[c] = src->connections[c];
}

// Comparaison de la fitness de deux génomes (pour la fonction de tri)
int compare_fitness(const void *genome_1, const void *genome_2) {
    float delta = (**(const genome_t **)genome_1).fitness - (**(const genome_t **)genome_2).fitness;
    if (delta > 0.0f)
        return -1;
    if (delta < 0.0f)
        return 1;
    return 0;
}

// Ajout d'une connexion depuis le noeud 'in' vers le noeud 'out'
void genome_add_connection(genome_t *genome, innovation_data_t *innovation, int in, int out, float weight) {
    ASSERT(genome->connection_number < MAX_CONNECTIONS);

    // Accès au numéro d'innovation associé
    int inno = get_connection_inno(innovation, in, out);

    // S'il n'existe pas, on l'ajoute à la table
    if (inno == INNO_NONEXISTANT) {
        inno = get_next_inno(innovation);
        add_inno(innovation, in, out, inno);
    }

    // Recherche de l'indice d'insertion en fonction du numéro d'innovation
    int index = genome->connection_number;
    while (index > 0 && genome->connections[index - 1].innovation_number > inno) {
        genome->connections[index] = genome->connections[index - 1];
        index--;
    }

    // Création de la connexion
    genome->connections[index] = (connection_t){
        .in = in,
        .out = out,
        .weight = weight,
        .enabled = true,
        .innovation_number = inno
    };
    genome->connection_number++;
}

// Supprimer une connexion
void genome_delete_connection(genome_t *genome, int connection) {
    ASSERT(connection >= 0 && connection < genome->connection_number);
    genome->connections[connection].enabled = false;
}

// Ajout d'un noeud au milieu d'une connexion
void genome_add_node(genome_t *genome, innovation_data_t *innovation, int connection) {
    ASSERT(genome->node_number < MAX_NODES);
    ASSERT(genome->connection_number < MAX_CONNECTIONS - 1);
    ASSERT(connection >= 0 && connection < genome->connection_number);

    int id = genome->node_number;
    connection_t c = genome->connections[connection];
    int out_depth = genome->nodes[c.out].depth;

    // Création du nouveau noeud
    genome->nodes[genome->node_number] = (node_t){ .depth = out_depth };

    // Création des deux nouvelle connexions
    genome_add_connection(genome, innovation, c.in, id, c.weight);
    genome_add_connection(genome, innovation, id, c.out, 1.0f);

    // Suppression de l'ancienne connexion
    genome_delete_connection(genome, connection);

    // Mise à jour de la profondeur des noeuds
    for (int n = 0; n < genome->node_number; n++)
        if (genome->nodes[n].depth >= out_depth)
            genome->nodes[n].depth++;

    genome->node_number++;
}

// Suppression d'un noeud
void genome_delete_node(genome_t *genome, int node_id) {
    ASSERT(node_id >= 0 && node_id < genome->node_number);

    // Suppression de toutes les connexions adjascentes au noeud
    for (int c = 0; c < genome->connection_number; c++)
        if (genome->connections[c].in == node_id || genome->connections[c].out == node_id)
            genome->connections[c].enabled = false;
}

// Activation / désactivation d'une connexion
void genome_toogle_enabled(genome_t *genome, int connection) {
    ASSERT(connection >= 0 && connection < genome->connection_number);

    genome->connections[connection].enabled = !genome->connections[connection].enabled;
}


// Fonction auxiliaire pour le DFS
void dfs(genome_t *genome, int **adjacency, int *node_connection_number, bool *visited, int node, int *current_depth) {
    if (!visited[node]) {
        visited[node] = true;
        for (int c = 0; c < node_connection_number[node]; c++)
            dfs(genome, adjacency, node_connection_number, visited, adjacency[node][c], current_depth);
        genome->nodes[node].depth = *current_depth;
        (*current_depth)--;
    }
}

// Mise à jour de la profondeur des noeuds
void udpate_node_depth(genome_t *genome) {
    // Construction de la liste d'adjacence
    int **adjacency = (int **)malloc(sizeof(int *)*genome->node_number);
    int *node_connection_number = (int *)malloc(sizeof(int)*genome->node_number);
    for (int n = 0; n < genome->node_number; n++) {
        adjacency[n] = (int *)malloc(sizeof(int)*genome->node_number);
        node_connection_number[n] = 0;
    }
    for (int c = 0; c < genome->connection_number; c++) {
        adjacency[genome->connections[c].in][node_connection_number[genome->connections[c].in]] = genome->connections[c].out;
        node_connection_number[genome->connections[c].in]++;
    }

    // Tri topologique avec un DFS depuis chaque neurone d'entrée
    bool *visited = (bool *)malloc(sizeof(bool)*genome->node_number);
    for (int id = 0; id < genome->node_number; id++)
        visited[id] = false;
    int current_depth = genome->node_number - 1;
    for (int in = 0; in < genome->input_number; in++)
        dfs(genome, adjacency, node_connection_number, visited, in, &current_depth);
    
    // Libération des structures
    for (int n = 0; n < genome->node_number; n++)
        free(adjacency[n]);
    free(adjacency);
    free(node_connection_number);
    free(visited);
}

// Test de l'existence d'une connexion entre deux noeuds
bool connection_exists(genome_t *genome, int in, int out) {
    for (int c = 0; c < genome->connection_number; c++)
        if (genome->connections[c].in == in && genome->connections[c].out == out)
            return true;
    return false;
}

// Croisement entre deux génomes
void crossover(genome_t *parent_1, genome_t *parent_2, genome_t *child) {
    ASSERT(parent_1->input_number == parent_2->input_number && parent_1->output_number == parent_2->output_number);

    // Attribution du parent avec la meilleure fitness au parent 1 par convention
    if (parent_1->fitness < parent_2->fitness) {
        genome_t *tmp = parent_1;
        parent_1 = parent_2;
        parent_2 = tmp;
    }

    // Initialisation de l'enfant
    child->node_number = parent_1->node_number;
    child->input_number = parent_1->input_number;
    child->output_number = parent_1->output_number;

    // Transmission des connexions grâce aux numéros d'innovations
    int i1 = 0; int i2 = 0; int i3 = 0;
    while (i1 < parent_1->connection_number || i2 < parent_2->connection_number) {
        // Gène en excès manquant au parent 1 (donc pas de transmission)
        if (i1 >= parent_1->connection_number) {
            i2++;
        }
        // Gène en excès manquant au parent 2 (donc transmission)
        else if (i2 >= parent_2->connection_number) {
            child->connections[i3] = parent_1->connections[i1];
            i1++; i3++;
        }
        // Gène disjoint manquant au parent 1 (donc pas de transmission)
        else if (parent_1->connections[i1].innovation_number > parent_2->connections[i2].innovation_number) {
            i2++;
        }
        // Gène disjoint manquant au parent 2 (donc transmission)
        else if (parent_1->connections[i1].innovation_number < parent_2->connections[i2].innovation_number) {
            child->connections[i3] = parent_1->connections[i1];
            i1++; i3++;
        }
        // Gène coïncidant
        else if (parent_1->connections[i1].innovation_number == parent_2->connections[i2].innovation_number) {
            if (proba_rng(0.5f))
                // Héritage depuis le parent 1 avec une proba de 1/2
                child->connections[i3] = parent_1->connections[i1];
            else
                // Héritage depuis le parent 2 avec une proba de 1/2
                child->connections[i3] = parent_2->connections[i2];
            i1++; i2++; i3++;
        }
    }
    child->connection_number = i3;

    // Mise à jour de la profondeur des noeuds de l'enfant
    udpate_node_depth(child);
}