#include <math.h>

#include "graphics.h"
#include "raylib.h"
#include "settings.h"

// Paramètres de la fenêtre
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600

// Paramètres graphiques
#define CART_WIDTH 0.1f
#define CART_HEIGHT 0.05f
#define WEIGHT_RADIUS 0.02f

// Zoom de l'affichage
#define ZOOM 600.0f

// Couleurs des objets
#define BACKGROUND_COLOR (Color){ 50, 50, 50, 255 }
#define CART_COLOR       (Color){ 150, 150, 150, 255 }
#define WEIGHT_COLOR     (Color){ 150, 150, 150, 255 }
#define ROD_COLOR        (Color){ 100, 100, 100, 255 }
#define RAIL_COLOR       (Color){ 80, 80, 80, 255 }

// Initialisation de la fenêtre
void init_window(void) {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Inverted pendulum");
    SetTargetFPS(60);
}

// Fermeture de la fenêtre
void close_window(void) {
    CloseWindow();
}

// Vérification de demande de fermeture de la fenêtre
bool window_should_close(void) {
    return WindowShouldClose();
}

// Accès au temps écoulé depuis la dernière frame
float get_frame_time(void) {
    return GetFrameTime();
}

// Affichage d'un pendule
void draw(inverted_pendulum_t pendulum) {
    SetWindowTitle(TextFormat("Inverted Pendulum - %dfps", GetFPS()));
    
    BeginDrawing();

    ClearBackground(BACKGROUND_COLOR);

    int center_x = (int)(WINDOW_WIDTH/2.0f + pendulum.x*ZOOM);
    int center_y = (int)(WINDOW_HEIGHT/2.0f);

    int end_x = center_x + (int)(sin(pendulum.theta)*ROD_LENGTH*ZOOM);
    int end_y = center_y + (int)(cos(pendulum.theta)*ROD_LENGTH*ZOOM);

    DrawLineEx((Vector2){ (WINDOW_WIDTH - RAIL_LENGTH*ZOOM)/2.0f, WINDOW_HEIGHT/2.0f }, (Vector2){ (WINDOW_WIDTH + RAIL_LENGTH*ZOOM)/2.0f, WINDOW_HEIGHT/2.0f }, 0.01*ZOOM, RAIL_COLOR);
    DrawRectangle(center_x - CART_WIDTH/2.0f*ZOOM, center_y - CART_HEIGHT/2.0f*ZOOM, CART_WIDTH*ZOOM, CART_HEIGHT*ZOOM, CART_COLOR);
    DrawLineEx((Vector2){ center_x, center_y }, (Vector2){ end_x, end_y }, 0.01f*ZOOM, ROD_COLOR);
    DrawCircle(end_x, end_y, WEIGHT_RADIUS*ZOOM, WEIGHT_COLOR);

    EndDrawing();
}