CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g \
          $(shell pkg-config --cflags ncurses)
LDLIBS  = $(shell pkg-config --libs ncurses) -lX11 -lXrandr

OBJS = main.o monitors.o draw.o

xrantui: $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o:     main.c monitors.h draw.h
monitors.o: monitors.c monitors.h
draw.o:     draw.c draw.h monitors.h

PREFIX ?= /usr/local

install: xrantui
	install -Dm755 xrantui $(DESTDIR)$(PREFIX)/bin/xrantui

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/xrantui

clean:
	rm -f $(OBJS) xrantui

.PHONY: clean install uninstall
