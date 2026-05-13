# xrantui

Interactive terminal UI for arranging monitors, built with ncurses and Xrandr.

![C11](https://img.shields.io/badge/C-11-blue) ![ncurses](https://img.shields.io/badge/TUI-ncurses-green)

## Features

- Visual scaled-down canvas of your monitor layout
- Move monitors with arrow keys and apply instantly via xrandr
- Snap-to-edge when dragging monitors near each other
- Set primary monitor
- Save layout to `~/.screenlayout/xrantui.sh` with automatic `~/.xprofile` hook for persistence across reboots
- Undo to revert unsaved changes

## Requirements

### Runtime

- X11 with Xrandr
- `xrandr` CLI tool in `$PATH`

### Build

```bash
sudo apt install gcc make pkg-config libncurses-dev libxrandr-dev libx11-dev
```

## Installation

```bash
git clone <repo>
cd xrantui
make
sudo make install        # installs to /usr/local/bin
```

Custom prefix:

```bash
sudo make install PREFIX=/usr          # → /usr/bin/xrantui
make install PREFIX=~/.local           # → ~/.local/bin/xrantui (no sudo needed)
```

Uninstall:

```bash
sudo make uninstall
```

## Usage

Run inside a terminal within an X session:

```bash
xrantui
```

If your `DISPLAY` variable is not set:

```bash
DISPLAY=:0 xrantui
```

## Keybindings

| Key | Action |
|---|---|
| `Tab` | Select next monitor |
| `← ↑ → ↓` | Move selected monitor (40 px/step, snaps to edges) |
| `p` | Set selected monitor as primary |
| `u` | Undo — revert all monitors to positions at startup |
| `Enter` | Apply layout with xrandr |
| `s` | Save layout to `~/.screenlayout/xrantui.sh` |
| `q` | Quit |

## Persistent layout

Press `s` to save the current layout. This writes a shell script to
`~/.screenlayout/xrantui.sh` and adds a one-time entry to `~/.xprofile` so the
layout is restored automatically on every X login.

## Project structure

```
├── main.c       — event loop and keybindings
├── monitors.c   — read/write monitor state via Xrandr
├── monitors.h
├── draw.c       — render monitors with ncurses
└── draw.h
```
