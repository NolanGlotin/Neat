#ifndef MUTATION_H
#define MUTATION_H

#include "genome.h"
#include "innovation.h"

void mutation_mutate_weight(genome_t *genome);
void mutation_add_connection(genome_t *genome, innovation_data_t *innovation);
void mutation_remove_connection(genome_t *genome);
void mutation_add_node(genome_t *genome, innovation_data_t *innovation);
void mutation_remove_node(genome_t *genome);
void mutation_toogle_enabled(genome_t *genome);

#endif