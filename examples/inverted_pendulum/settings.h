#ifndef SETTINGS_H
#define SETTINGS_H

// Paramètres physiques
#define G                        9.81f
#define COEFFICIENT_OF_FRICTION  0.2f
#define RAIL_LENGTH              1.5f
#define ROD_LENGTH               0.3f
#define MASS                     0.5f
#define CART_SPEED               3.0f
#define CART_ACCELERATION        16.0f

// Paramètres de la simulation
#define INPUT_NB                       5
#define OUTPUT_NB                      1
#define LEARNING_SIMULATION_DURATION   60.0f
#define LEARNING_SIMULATION_FRAMERATE  60.0f
#define CONTROL_CART_SPEED             0

#endif