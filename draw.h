#ifndef DRAW_H
#define DRAW_H

#include "monitors.h"

void draw_init_colors(void);
void draw_monitors(Monitor *mons, int count, int selected, int scale,
                   const char *msg);  /* msg visas i statusraden; NULL = normal hjälptext */

#endif
