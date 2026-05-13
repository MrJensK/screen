# xrantui

TUI för att placera ut skärmar med notcurses och Xrandr i C.

## Syfte

Interaktivt verktyg som kör i terminalen (inom X). Användaren väljer skärm, flyttar den med piltangenter och applicerar konfigurationen via xrandr.

## Stack

- **Språk:** C (C11)
- **TUI:** notcurses
- **Skärminformation:** libxrandr / libx11
- **Build:** gcc + pkg-config

## Bygg

```bash
gcc main.c -o xrantui \
    $(pkg-config --libs --cflags notcurses) \
    -lX11 -lXrandr
```

## Projektstruktur

```
xrantui/
├── CLAUDE.md
├── main.c          # entry point, event loop
├── monitors.c      # hämta/sätta skärminfo via Xrandr
├── monitors.h
├── draw.c          # rita skärmar med notcurses
└── draw.h
```

## Datastruktur

```c
typedef struct {
    char name[64];      // output-namn, t.ex. "HDMI-1"
    int x, y;           // position i pixlar
    int w, h;           // upplösning i pixlar
    int connected;      // RR_Connected
} Monitor;
```

## Keybindings

| Tangent | Funktion |
|---|---|
| Tab | Växla vald skärm |
| Piltangenter | Flytta skärm (ett steg = scale px) |
| Enter | Applicera med xrandr |
| q | Avsluta |

## Skalfaktor

Variabeln `scale` styr hur många pixlar en terminalcell representerar.
Standard: `40`. Justera om skärmarna inte får plats eller blir för små.

## Applicering

Applicerar via `system()` med ett genererat xrandr-kommando:

```bash
xrandr --output HDMI-1 --pos 320x1080 --auto
```

## Beroenden (Debian)

```bash
sudo apt install libnotcurses-dev libnotcurses-core-dev \
                 libx11-dev libxrandr-dev
```

## Känt att bygga vidare på

- Snapping — magnetism när skärmar är nära varandra
- Batch-apply — ändra alla skärmar innan xrandr anropas
- Spara ut `monitors.sh` istället för att köra direkt
- Visa upplösning och Hz inuti skärmrektangeln
- Stöd för att sätta primärskärm (`--primary`)