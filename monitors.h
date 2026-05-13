#ifndef MONITORS_H
#define MONITORS_H

#define MAX_MONITORS 16

typedef struct {
    char name[64];
    int x, y;
    int w, h;
    int connected;
    int primary;
} Monitor;

int monitors_get(Monitor *mons, int max);
void monitors_apply(Monitor *mons, int count);
int  monitors_save(Monitor *mons, int count);  /* returnerar 0 vid lyckad sparning */

#endif
