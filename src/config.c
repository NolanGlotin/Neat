#include "../include/config.h"

config_t neat_config = (config_t) {
    .generation_number = DEFAULT_GENERATION_NUMBER,
    .fitness_threshold = DEFAULT_FITNESS_THRESHOLD,
    .population_size = DEFAULT_POPULATION_SIZE,
    .survival_rate = DEFAULT_SURVIVAL_RATE,
    .elitism = DEFAULT_ELITISM,

    .network_weight_range = DEFAULT_NETWORK_WEIGHT_RANGE,
    .network_activation_function = DEFAULT_NETWORK_ACTIVATION_FUNCTION,

    .mutation_replace_weight_proba = DEFAULT_MUTATION_REPLACE_WEIGHT_PROBA,
    .mutation_perturb_weight_proba = DEFAULT_MUTATION_PERTURB_WEIGHT_PROBA,
    .mutation_add_connection_proba = DEFAULT_MUTATION_ADD_CONNECTION_PROBA,
    .mutation_delete_connection_proba = DEFAULT_MUTATION_DELETE_CONNECTION_PROBA,
    .mutation_add_neuron_proba = DEFAULT_MUTATION_ADD_NEURON_PROBA,
    .mutation_delete_neuron_proba = DEFAULT_MUTATION_DELETE_NEURON_PROBA,
    .mutation_toggle_enabled_proba = DEFAULT_MUTATION_TOGGLE_ENABLED_PROBA,
    .mutation_perturb_weight_factor = DEFAULT_MUTATION_PERTURB_WEIGHT_FACTOR,
    .mutation_new_connection_tries = DEFAULT_MUTATION_NEW_CONNECTION_TRIES,
    .mutation_new_node_tries = DEFAULT_MUTATION_NEW_NODE_TRIES,

    .speciation_c1 = DEFAULT_SPECIATION_C1,
    .speciation_c2 = DEFAULT_SPECIATION_C2,
    .speciation_c3 = DEFAULT_SPECIATION_C3,
    .speciation_compatibility_treshold = DEFAULT_SPECIATION_COMPATIBILITY_THRESHOLD,
    .speciation_stagnation_threshold = DEFAULT_SPECIATION_STAGNATION_THRESHOLD
};