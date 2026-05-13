#include "draw.h"
#include <ncurses.h>
#include <string.h>
#include <stdio.h>

#define COLOR_NORMAL  1
#define COLOR_SEL     2
#define COLOR_STATUS  3
#define COLOR_PRIMARY 4   /* primärskärm, ej vald */
#define COLOR_PRISEL  5   /* primärskärm, vald */

void draw_init_colors(void) {
    start_color();
    use_default_colors();
    init_pair(COLOR_NORMAL,  COLOR_WHITE,  COLOR_BLUE);
    init_pair(COLOR_SEL,     COLOR_YELLOW, COLOR_GREEN);
    init_pair(COLOR_STATUS,  COLOR_WHITE,  COLOR_BLACK);
    init_pair(COLOR_PRIMARY, COLOR_CYAN,   COLOR_BLUE);
    init_pair(COLOR_PRISEL,  COLOR_CYAN,   COLOR_GREEN);
}

static int pair_for(int selected, int primary) {
    if (selected && primary) return COLOR_PRISEL;
    if (selected)            return COLOR_SEL;
    if (primary)             return COLOR_PRIMARY;
    return COLOR_NORMAL;
}

static void draw_box(int y, int x, int h, int w, int sel, int primary) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    if (y >= rows || x >= cols || h < 2 || w < 2) return;
    if (y + h > rows) h = rows - y;
    if (x + w > cols) w = cols - x;
    if (h < 2 || w < 2) return;

    int pair = pair_for(sel, primary);
    attron(COLOR_PAIR(pair));

    mvaddch(y, x, ACS_ULCORNER);
    for (int c = 1; c < w - 1; c++) addch(ACS_HLINE);
    mvaddch(y, x + w - 1, ACS_URCORNER);

    for (int r = 1; r < h - 1; r++) {
        mvaddch(y + r, x,         ACS_VLINE);
        for (int c = 1; c < w - 1; c++) mvaddch(y + r, x + c, ' ');
        mvaddch(y + r, x + w - 1, ACS_VLINE);
    }

    mvaddch(y + h - 1, x,         ACS_LLCORNER);
    for (int c = 1; c < w - 1; c++) mvaddch(y + h - 1, x + c, ACS_HLINE);
    mvaddch(y + h - 1, x + w - 1, ACS_LRCORNER);

    /* primärmarkör i övre vänstra hörnet av interiören */
    if (primary && w > 2)
        mvaddstr(y + 1, x + 1, "P");

    attroff(COLOR_PAIR(pair));
}

static void draw_centered(int y, int x_start, int width, const char *text, int pair) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    (void)cols;
    if (y < 0 || y >= rows) return;

    int len = (int)strlen(text);
    if (len > width) len = width;
    int lx = x_start + (width - len) / 2;

    char buf[66];
    strncpy(buf, text, (size_t)len);
    buf[len] = '\0';

    attron(COLOR_PAIR(pair));
    mvaddstr(y, lx, buf);
    attroff(COLOR_PAIR(pair));
}

void draw_monitors(Monitor *mons, int count, int selected, int scale,
                   const char *msg) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    clear();

    for (int i = 0; i < count; i++) {
        Monitor *m = &mons[i];

        int tx = m->x / scale;
        int ty = m->y / (scale * 2);
        int tw = (m->w > 0) ? m->w / scale       : 10;
        int th = (m->h > 0) ? m->h / (scale * 2) :  4;
        if (tw < 10) tw = 10;
        if (th <  4) th =  4;

        draw_box(ty, tx, th, tw, i == selected, m->primary);

        int inner_w = tw - 2;
        int pair = pair_for(i == selected, m->primary);

        /* namn — centrerat på övre halvan av interiören */
        int name_row = ty + 1 + (th - 2) / 3;
        draw_centered(name_row, tx + 1, inner_w, m->name, pair);

        /* upplösning — en rad under namnet om plats finns */
        if (th >= 4 && m->w > 0 && m->h > 0) {
            char res[32];
            snprintf(res, sizeof(res), "%dx%d", m->w, m->h);
            draw_centered(name_row + 1, tx + 1, inner_w, res, pair);
        }
    }

    /* statusrad */
    attron(COLOR_PAIR(COLOR_STATUS));
    mvhline(rows - 1, 0, ' ', cols);
    if (msg) {
        mvprintw(rows - 1, 0, " %s", msg);
    } else {
        mvprintw(rows - 1, 0,
                 " Tab:välj  pil:flytta  p:primär  u:ångra"
                 "  Enter:applicera  s:spara  q:avsluta"
                 "  | %s @ %d,%d%s",
                 mons[selected].name,
                 mons[selected].x, mons[selected].y,
                 mons[selected].primary ? " [P]" : "");
    }
    attroff(COLOR_PAIR(COLOR_STATUS));
}
