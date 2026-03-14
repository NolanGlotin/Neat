#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include <neat/neat.h>
#include <neat/network.h>
#include <neat/config.h>

#include "physics.h"
#include "graphics.h"
#include "settings.h"

// Injection de l'état du pendule vers 'inputs'
void inject_inputs(inverted_pendulum_t pendulum, float *inputs) {
    inputs[0] = pendulum.x;
    inputs[1] = sinf(pendulum.theta);
    inputs[2] = -cosf(pendulum.theta);
    inputs[3] = pendulum.dtheta;
    inputs[4] = 1.0f; // Biais
}

// Fonction d'évalution d'un génome
float fitness_function(network_t *network) {
    // Initialisation de la simulation
    float dt = 1.0f/LEARNING_SIMULATION_FRAMERATE;
    float inputs[INPUT_NB];
    float outputs[OUTPUT_NB];
    inverted_pendulum_t pendulum = {0};
    reset_pendulum(&pendulum);
    float score = 1.0f;
    float duration = 0.0f;
    
    while (is_pendulum_between_bounds(&pendulum) && duration < LEARNING_SIMULATION_DURATION) {
        // Activation du réseau de neurones
        inject_inputs(pendulum, inputs);
        feed_forward(network, inputs, outputs);

        // Contrôle du chariot
        if (CONTROL_CART_SPEED)
            update_pendulum_speed(dt, outputs[0]*CART_SPEED, &pendulum);
        else
            update_pendulum_acceleration(dt, outputs[0]*CART_ACCELERATION, &pendulum);

        // Attribution de la récompense
        if (-cosf(pendulum.theta) > 0.9)
            score += 1.0f/(1.0f + fabsf(pendulum.dtheta));
        duration += dt;
    }
    return score;
}

// Entraînement d'un nouveau modèle
void train_model(char *model_output) {
    srand(time(NULL));
    network_t *model = neat(
        INPUT_NB,
        OUTPUT_NB,
        fitness_function,
        NULL,
        "log.csv"
    );
    save_network(model, model_output);
    free_network(model);
}

// Test d'un modèle existant
void test_model(char *model_name) {
    // Chargement du modèle
    network_t *model = load_network(model_name);
    float inputs[INPUT_NB];
    float outputs[OUTPUT_NB];

    // Initialisation de la simulation
    inverted_pendulum_t pendulum = {0};
    reset_pendulum(&pendulum);
    float duration = 0.0f;
    init_window();

    while(!window_should_close()) {
        float dt = get_frame_time();
        duration += dt;

        // Activation du réseau de neurones
        inject_inputs(pendulum, inputs);
        feed_forward(model, inputs, outputs);

        // Contrôle du chariot
        if (CONTROL_CART_SPEED)
            update_pendulum_speed(dt, outputs[0]*CART_SPEED, &pendulum);
        else
            update_pendulum_acceleration(dt, outputs[0]*CART_ACCELERATION, &pendulum);

        // Représentation graphique
        draw(pendulum);
    }
    close_window();
    free_network(model);
}

// Fonction principale
int main(void) {
    train_model("output.txt");
    test_model("output.txt");
    return EXIT_SUCCESS;
}