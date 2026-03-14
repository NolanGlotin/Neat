#ifndef CONFIG_H
#define CONFIG_H

#include "utils.h"

#define DEFAULT_GENERATION_NUMBER  100
#define DEFAULT_FITNESS_THRESHOLD  2000.0f
#define DEFAULT_POPULATION_SIZE    1000
#define DEFAULT_SURVIVAL_RATE      0.6f
#define DEFAULT_ELITISM            0.25f

#define DEFAULT_NETWORK_WEIGHT_RANGE         1.0f
#define DEFAULT_NETWORK_ACTIVATION_FUNCTION  hyperbolic_tangent

#define DEFAULT_MUTATION_REPLACE_WEIGHT_PROBA     0.1f
#define DEFAULT_MUTATION_PERTURB_WEIGHT_PROBA     0.8f
#define DEFAULT_MUTATION_ADD_CONNECTION_PROBA     0.3f
#define DEFAULT_MUTATION_DELETE_CONNECTION_PROBA  0.01f
#define DEFAULT_MUTATION_ADD_NEURON_PROBA         0.03f
#define DEFAULT_MUTATION_DELETE_NEURON_PROBA      0.01f
#define DEFAULT_MUTATION_TOGGLE_ENABLED_PROBA     0.01f
#define DEFAULT_MUTATION_PERTURB_WEIGHT_FACTOR    0.1f
#define DEFAULT_MUTATION_NEW_CONNECTION_TRIES     20
#define DEFAULT_MUTATION_NEW_NODE_TRIES           20

#define DEFAULT_SPECIATION_C1                       1.0f
#define DEFAULT_SPECIATION_C2                       1.0f
#define DEFAULT_SPECIATION_C3                       0.3f
#define DEFAULT_SPECIATION_COMPATIBILITY_THRESHOLD  3.0f
#define DEFAULT_SPECIATION_STAGNATION_THRESHOLD     15


typedef struct {
    int generation_number;      // Nombre de générations
    float fitness_threshold;    // Seuil de fitness pour arrêter l'algorithme
    int population_size;        // Taille de la population
    float survival_rate;        // Proportion de la population qui peut se reproduire
    float elitism;              // Proportion de la population qui est garantie de survivre à la prochaine génération

    float network_weight_range;                     // Plage de poids pour les connexions
    float (*network_activation_function)(float);    // Fonction d'activation utilisée par les réseaux de neurones

    float mutation_replace_weight_proba;     // Probabilité de remplacer un poids de connexion par une nouvelle valeur aléatoire
    float mutation_perturb_weight_proba;     // Probabilité de modifier un poids de connexion en fonction du facteur de mutation
    float mutation_add_connection_proba;     // Probabilité d'ajouter une connexion
    float mutation_delete_connection_proba;  // Probabilité de supprimer une connexion
    float mutation_add_neuron_proba;         // Probabilité d'ajouter un noeud
    float mutation_delete_neuron_proba;      // Probabilité de supprimer un noeud
    float mutation_toggle_enabled_proba;     // Probabilité de basculer l'état d'une connexion (activée/désactivée)
    float mutation_perturb_weight_factor;    // Facteur de mutation pour la modification d'un poids de connexion
    int mutation_new_connection_tries;       // Nombre de tentatives pour trouver deux noeuds non connectés lors de l'ajout d'une connexion
    int mutation_new_node_tries;             // Nombre de tentatives pour trouver une connexion activée lors de l'ajout d'un noeud

    float speciation_c1;                        // Coefficient pour les gènes en excès
    float speciation_c2;                        // Coefficient pour les gènes disjoints
    float speciation_c3;                        // Coefficient pour la différence de poids moyenne des gènes appariés
    float speciation_compatibility_treshold;    // Seuil de compatibilité pour la formation des espèces
    int speciation_stagnation_threshold;        // Seuil de stagnation pour l'extinction d'une espèce
} config_t;

// Configuration globale
extern config_t neat_config;

#endif