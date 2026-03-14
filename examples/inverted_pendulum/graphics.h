#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include "physics.h"

void init_window(void);
void close_window(void);

bool window_should_close(void);
float get_frame_time(void);

void draw(inverted_pendulum_t pendulum);

#endif