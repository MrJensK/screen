#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "monitors.h"
#include "draw.h"

#define SCALE      40
#define SNAP_DIST  SCALE   /* pixlar — en knapptryckning */

static void snap(Monitor *mons, int count, int idx) {
    Monitor *m = &mons[idx];
    for (int i = 0; i < count; i++) {
        if (i == idx) continue;
        Monitor *o = &mons[i];
        int mr = m->x + m->w,  or_ = o->x + o->w;
        int mb = m->y + m->h,  ob  = o->y + o->h;

        /* horisontell snap: vänster mot höger och tvärtom */
        if (abs(m->x - or_) < SNAP_DIST) m->x = or_;
        else if (abs(mr - o->x) < SNAP_DIST) m->x = o->x - m->w;

        /* vertikal snap: topp mot botten och tvärtom */
        if (abs(m->y - ob) < SNAP_DIST) m->y = ob;
        else if (abs(mb - o->y) < SNAP_DIST) m->y = o->y - m->h;
    }
    if (m->x < 0) m->x = 0;
    if (m->y < 0) m->y = 0;
}

int main(void) {
    Monitor mons[MAX_MONITORS];
    int nmons = monitors_get(mons, MAX_MONITORS);
    if (nmons == 0) {
        fprintf(stderr, "xrantui: inga aktiva skärmar hittades\n");
        return 1;
    }

    /* spara ursprungspositioner för ångra */
    Monitor orig[MAX_MONITORS];
    memcpy(orig, mons, sizeof(Monitor) * (size_t)nmons);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    draw_init_colors();

    int selected = 0;
    const char *msg = NULL;

    for (;;) {
        draw_monitors(mons, nmons, selected, SCALE, msg);
        refresh();
        msg = NULL;

        int ch = getch();
        switch (ch) {
        case 'q':
        case 'Q':
            goto done;

        case '\t':
            selected = (selected + 1) % nmons;
            break;

        case KEY_UP:
            mons[selected].y -= SCALE;
            if (mons[selected].y < 0) mons[selected].y = 0;
            snap(mons, nmons, selected);
            break;
        case KEY_DOWN:
            mons[selected].y += SCALE;
            snap(mons, nmons, selected);
            break;
        case KEY_LEFT:
            mons[selected].x -= SCALE;
            if (mons[selected].x < 0) mons[selected].x = 0;
            snap(mons, nmons, selected);
            break;
        case KEY_RIGHT:
            mons[selected].x += SCALE;
            snap(mons, nmons, selected);
            break;

        case 'p':
        case 'P':
            /* sätt vald skärm som primär, rensa övriga */
            for (int i = 0; i < nmons; i++)
                mons[i].primary = (i == selected) ? 1 : 0;
            msg = "Primärskärm satt — tryck Enter för att applicera";
            break;

        case 'u':
        case 'U':
            memcpy(mons, orig, sizeof(Monitor) * (size_t)nmons);
            msg = "Återställt till ursprungspositioner";
            break;

        case KEY_ENTER:
        case '\n':
        case '\r':
            monitors_apply(mons, nmons);
            msg = "Applicerat";
            break;

        case 's':
        case 'S':
            if (monitors_save(mons, nmons) == 0)
                msg = "Sparat till ~/.screenlayout/xrantui.sh och ~/.xprofile";
            else
                msg = "Fel: kunde inte spara";
            break;
        }
    }

done:
    endwin();
    return 0;
}
