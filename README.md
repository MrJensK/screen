# xrantui

Interactive terminal UI for arranging monitors, built with ncurses and Xrandr.

![C11](https://img.shields.io/badge/C-11-blue) ![ncurses](https://img.shields.io/badge/TUI-ncurses-green)

## Features

- Visual scaled-down canvas showing all active monitors
- Move monitors with arrow keys — snaps to edges automatically
- Cycle through each monitor's supported resolutions
- Set primary monitor
- Apply layout instantly via xrandr
- Save layout for persistence across reboots (XDG autostart, `.xprofile`, `.xinitrc`)
- Undo to revert to positions at startup

## Requirements

### Runtime

- X11 with Xrandr
- `xrandr` in `$PATH`

### Build dependencies

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
sudo make install PREFIX=/usr     # → /usr/bin/xrantui
make install PREFIX=~/.local      # → ~/.local/bin/xrantui (no sudo)
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

If `DISPLAY` is not set:

```bash
DISPLAY=:0 xrantui
```

## Keybindings

| Key | Action |
|---|---|
| `Tab` | Select next monitor |
| `← ↑ → ↓` | Move selected monitor (40 px/step, snaps to edges) |
| `+` / `-` | Cycle selected monitor's resolution (larger / smaller) |
| `p` | Set selected monitor as primary |
| `u` | Undo — revert all monitors to positions at startup |
| `Enter` | Apply layout with xrandr |
| `s` | Save layout |
| `q` | Quit |

## Persistent layout

Press `s` to save the current layout. xrantui writes to all relevant startup
files automatically, covering most X setups:

| File | Used by |
|---|---|
| `~/.screenlayout/xrantui.sh` | The xrandr script itself |
| `~/.config/autostart/xrantui.desktop` | XDG autostart — GNOME, KDE, XFCE, LXDE |
| `~/.xprofile` | Display manager without DE — e.g. LightDM + sxwm |
| `~/.xinitrc` | `startx` + bare WM — e.g. sxwm. Inserted before `exec`. |

`~/.xprofile` and `~/.xinitrc` are only modified if they already exist.

## Project structure

```
├── main.c       — event loop and keybindings
├── monitors.c   — read/write monitor state via Xrandr
├── monitors.h
├── draw.c       — render monitors with ncurses
└── draw.h
```
