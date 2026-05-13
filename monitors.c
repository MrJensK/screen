#include "monitors.h"
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int monitors_get(Monitor *mons, int max) {
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Kan inte öppna display\n");
        return 0;
    }

    Window root = DefaultRootWindow(dpy);
    XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);
    if (!res) {
        XCloseDisplay(dpy);
        return 0;
    }

    RROutput primary_out = XRRGetOutputPrimary(dpy, root);

    int count = 0;
    for (int i = 0; i < res->noutput && count < max; i++) {
        XRROutputInfo *out = XRRGetOutputInfo(dpy, res, res->outputs[i]);
        if (!out) continue;

        if (out->connection == RR_Connected && out->crtc) {
            Monitor *m = &mons[count];
            strncpy(m->name, out->name, sizeof(m->name) - 1);
            m->name[sizeof(m->name) - 1] = '\0';
            m->connected = 1;
            m->primary   = (res->outputs[i] == primary_out) ? 1 : 0;
            m->x = m->y = m->w = m->h = 0;

            XRRCrtcInfo *crtc = XRRGetCrtcInfo(dpy, res, out->crtc);
            if (crtc) {
                m->x = (int)crtc->x;
                m->y = (int)crtc->y;
                m->w = (int)crtc->width;
                m->h = (int)crtc->height;
                XRRFreeCrtcInfo(crtc);
            }
            count++;
        }
        XRRFreeOutputInfo(out);
    }

    XRRFreeScreenResources(res);
    XCloseDisplay(dpy);
    return count;
}

static void sanitize_name(char *name) {
    for (char *p = name; *p; p++) {
        if (!(*p >= 'A' && *p <= 'Z') && !(*p >= 'a' && *p <= 'z') &&
            !(*p >= '0' && *p <= '9') && *p != '-' && *p != '_')
            *p = '_';
    }
}

static void build_xrandr_args(Monitor *mons, int count, char *buf, size_t bufsz) {
    int pos = snprintf(buf, bufsz, "xrandr");
    for (int i = 0; i < count && pos < (int)bufsz - 80; i++) {
        sanitize_name(mons[i].name);
        pos += snprintf(buf + pos, bufsz - (size_t)pos,
                        " --output %s --pos %dx%d --auto%s",
                        mons[i].name, mons[i].x, mons[i].y,
                        mons[i].primary ? " --primary" : "");
    }
}

void monitors_apply(Monitor *mons, int count) {
    char cmd[4096];
    build_xrandr_args(mons, count, cmd, sizeof(cmd));
    system(cmd);
}

int monitors_save(Monitor *mons, int count) {
    const char *home = getenv("HOME");
    if (!home) return -1;

    char dir[512];
    snprintf(dir, sizeof(dir), "%s/.screenlayout", home);
    mkdir(dir, 0755);

    char path[560];
    snprintf(path, sizeof(path), "%s/xrantui.sh", dir);
    FILE *f = fopen(path, "w");
    if (!f) return -1;

    fprintf(f, "#!/bin/sh\n");
    for (int i = 0; i < count; i++) {
        sanitize_name(mons[i].name);
        fprintf(f, "xrandr --output %s --pos %dx%d --auto%s\n",
                mons[i].name, mons[i].x, mons[i].y,
                mons[i].primary ? " --primary" : "");
    }
    fclose(f);
    chmod(path, 0755);

    /* lägg till anrop i ~/.xprofile om det inte redan finns */
    char xprofile[512];
    snprintf(xprofile, sizeof(xprofile), "%s/.xprofile", home);

    int found = 0;
    f = fopen(xprofile, "r");
    if (f) {
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            if (strstr(line, "xrantui.sh")) { found = 1; break; }
        }
        fclose(f);
    }
    if (!found) {
        f = fopen(xprofile, "a");
        if (f) {
            fprintf(f, "\n[ -f \"$HOME/.screenlayout/xrantui.sh\" ]"
                       " && sh \"$HOME/.screenlayout/xrantui.sh\"\n");
            fclose(f);
        }
    }

    return 0;
}
