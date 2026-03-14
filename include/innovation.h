#ifndef INNOVATION_H
#define INNOVATION_H

// Convention pour les connexions ne possédant pas de numéro d'innovation
#define INNO_NONEXISTANT -1

// Type innovation
typedef struct {
    int current_number;  // Numéro d'innovation actuel
    int **database;      // Table d'innovation
    int size;            // Taille de la table d'innovation
} innovation_data_t;

innovation_data_t *init_innovation(int size);
void reset_innovation(innovation_data_t *innovation);
void free_innovation(innovation_data_t *innovation);

int get_next_inno(innovation_data_t *innovation);
int get_connection_inno(innovation_data_t *innovation, int in, int out);
void add_inno(innovation_data_t *innovation, int in, int out, int inno);

#endif