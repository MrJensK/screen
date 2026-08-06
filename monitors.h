#ifndef MONITORS_H
#define MONITORS_H

#define MAX_MONITORS 16
#define MAX_MODES 32

typedef struct {
    char name[64];
    int x, y;
    int w, h;
    int connected;
    int primary;

    /* tillgängliga upplösningar, sorterade störst->minst; mode_idx pekar
       på den som motsvarar aktuellt w/h (-1 om okänd) */
    int modes_w[MAX_MODES];
    int modes_h[MAX_MODES];
    int nmodes;
    int mode_idx;
} Monitor;

int monitors_get(Monitor *mons, int max);
void monitors_cycle_mode(Monitor *m, int dir);  /* dir: +1/-1, sätter w/h från modes-listan */
void monitors_apply(Monitor *mons, int count);
int  monitors_save(Monitor *mons, int count);  /* returnerar 0 vid lyckad sparning */

#endif
