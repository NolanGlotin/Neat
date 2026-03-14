#include <stdlib.h>

#include "../include/utils.h"
#include "../include/innovation.h"

// Initialisation de la table d'innovation
innovation_data_t *init_innovation(int size) {
    innovation_data_t *innovation = (innovation_data_t *)malloc(sizeof(innovation_data_t));
    innovation->current_number = 0;
    innovation->size = size;
    innovation->database = (int **)malloc(sizeof(int *)*size);
    for (int i = 0; i < innovation->size; i++)
        innovation->database[i] = (int *)malloc(sizeof(int)*innovation->size);
    
    reset_innovation(innovation);
    return innovation;
}

// Suppression de tous les numéros d'innovations de la table
void reset_innovation(innovation_data_t *innovation) {
    for (int i = 0; i < innovation->size; i++)
        for (int j = 0; j < innovation->size; j++)
            innovation->database[i][j] = INNO_NONEXISTANT;
}

// Libération de la table d'innovation
void free_innovation(innovation_data_t *innovation) {
    if (innovation != NULL) {
        for (int i = 0; i < innovation->size; i++)
            free(innovation->database[i]);
        free(innovation->database);
        free(innovation);
    }
}

// Accès au prochain numéro d'innovation
int get_next_inno(innovation_data_t *innovation) {
    innovation->current_number++;
    return innovation->current_number - 1;
}

// Accès au numéro d'innovation associé à une connexion
int get_connection_inno(innovation_data_t *innovation, int in, int out) {
    ASSERT(in >= 0 && in < innovation->size && out >= 0 && out < innovation->size);
    return innovation->database[in][out];
}

// Ajout d'un numéro d'innovation pour une connexion
void add_inno(innovation_data_t *innovation, int in, int out, int inno) {
    ASSERT(in >= 0 && in < innovation->size && out >= 0 && out < innovation->size);
    innovation->database[in][out] = inno;
}