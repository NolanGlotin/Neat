#ifndef NEAT_H
#define NEAT_H

#include "network.h"

network_t *neat(int input_number, int output_number, float (*fitness_function)(network_t *), float *reached_fitness, char *log_path);

#endif