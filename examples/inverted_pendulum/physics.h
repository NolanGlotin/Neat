#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdbool.h>

// Type pendule
typedef struct inverted_pendulum_t {
    float theta;     // Angle du pendule
    float dtheta;    // Vitesse angulaire du pendule
    float x;         // Position du chariot
    float dx;        // Vitesse du chariot
    float ddx;       // Accélération du chariot
} inverted_pendulum_t;

void reset_pendulum(inverted_pendulum_t *pendulum);

void update_pendulum_speed(float dt, float speed, inverted_pendulum_t *pendulum);
void update_pendulum_acceleration(float dt, float acceleration, inverted_pendulum_t *pendulum);
bool is_pendulum_between_bounds(inverted_pendulum_t *pendulum);

#endif