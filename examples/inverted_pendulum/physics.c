#include <math.h>
#include <stdio.h>

#include "physics.h"
#include "settings.h"

#define PI 3.14159265358979323846f

// Bornage d'un nombre dans un intervalle
float clampf(float x, float min, float max) {
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

// Réinitialisation du pendule
void reset_pendulum(inverted_pendulum_t *pendulum) {
    pendulum->theta   = 0.0f;
    pendulum->dtheta  = 0.0f;
    pendulum->x       = 0.0f;
    pendulum->dx      = 0.0f;
    pendulum->ddx     = 0.0f;
}

// Mise à jour du pendule avec la méthode d'Euler
void update_pendulum(float dt, inverted_pendulum_t *pendulum) {
    // Limitation de la position sur le rail
    if (pendulum->x < -RAIL_LENGTH/2.0f) {
        pendulum->ddx = 0.0f;
        pendulum->dx = 0.0f;
        pendulum->x = -RAIL_LENGTH/2.0f;
    }
    else if (pendulum->x > RAIL_LENGTH/2.0f) {
        pendulum->ddx = 0.0f;
        pendulum->dx = 0.0f;
        pendulum->x = RAIL_LENGTH/2.0f;
    }

    // Mise à jour de la vitesse et de l'accélération du pendule   
    pendulum->dtheta += (-G/ROD_LENGTH*sin(pendulum->theta) - COEFFICIENT_OF_FRICTION*(pendulum->dtheta)/MASS - pendulum->ddx/ROD_LENGTH*cos(pendulum->theta))*dt;
    pendulum->theta = fmodf(pendulum->theta + pendulum->dtheta*dt, 2.0f*PI);
}

// Vérification de la position du chariot
bool is_pendulum_between_bounds(inverted_pendulum_t *pendulum) {
    return pendulum->x > -RAIL_LENGTH/2.0f && pendulum->x < RAIL_LENGTH/2.0f;
}

// Modification de la vitesse du chariot
void update_pendulum_speed(float dt, float speed, inverted_pendulum_t *pendulum) {
    if (dt > 0) {
        pendulum->ddx = (speed - pendulum->dx)/dt;
        pendulum->dx = speed;
        pendulum->x += pendulum->dx*dt;
        update_pendulum(dt, pendulum);
    }
}

// Modification de l'accélération du chariot
void update_pendulum_acceleration(float dt, float acceleration, inverted_pendulum_t *pendulum) {
    if (dt > 0) {
        pendulum->ddx = acceleration;
        pendulum->dx = clampf(pendulum->dx + pendulum->ddx*dt, -CART_SPEED, CART_SPEED);
        pendulum->x += pendulum->dx*dt;
        update_pendulum(dt, pendulum);
    }
}