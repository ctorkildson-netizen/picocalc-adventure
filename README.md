# Colossal Cave Adventure on PicoCalc

A port of **Colossal Cave Adventure** -- Eric S. Raymond's
[Open Adventure](https://gitlab.com/esr/open-adventure), the 430-point
"Adventure 2.5" descended directly from Crowther & Woods -- to the
[PicoCalc](https://www.clockworkpi.com/product-page/picocalc) handheld,
running on the RP2350. Built on the same PDCurses-on-PicoCalc backend as
[picocalc-empire](https://github.com/ctorkildson-netizen/picocalc-empire)
and Blair Leduc's [picocalc-rogue](https://github.com/BlairLeduc/picocalc-rogue).

> **Alpha release.** It boots, plays and saves on real hardware, but has had
> one tester and a short play session. Reports on the ClockworkPi forum are
> very welcome.

## Installing

You need a PicoCalc with a **Pico 2 / Pico 2 W (RP2350)** module, the
current keyboard BIOS (v1.7 at the time of writing) and pelrun's
[uf2loader](https://github.com/pelrun/uf2loader) boot menu.

1. Download [`bin/picocalc-adventure.uf2`](https://github.com/ctorkildson-netizen/picocalc-adventure/raw/main/bin/picocalc-adventure.uf2).
2. Copy it into the `pico2-apps` folder on the PicoCalc's SD card.
3. Power on while holding **Up** (or F1/F5) and pick `picocalc-adventure`.

The last app you launched stays in flash and starts on every power-on, so
hold Up whenever you want the menu back.

## Playing

Type commands of one or two words (`east`, `get lamp`, `xyzzy`). `save` and
`resume` write and read an ordinary file on the SD card, named whatever you
type at the prompt.

## What's different on the PicoCalc

- The game's messages were written for ~70-column terminals; on the PicoCalc's
  64-column screen they are re-flowed paragraph by paragraph at word
  boundaries. The handful of deliberately laid-out messages (lines that start
  with spaces) are kept exactly as written.
- No command-line options and no readline history; input is line-at-a-time
  through curses.

## Building

    git clone --recursive https://github.com/ctorkildson-netizen/picocalc-adventure
    cd picocalc-adventure
    cmake -S . -B build          # needs the Pico SDK (2.3.0) and an ARM toolchain
    cmake --build build

`pc/` holds a desktop build (PDCurses SDL2 backend) used to test game logic
off-device.

## Credits

- **Colossal Cave Adventure**: Will Crowther and Don Woods (1977, 2005).
- **Open Adventure**: Eric S. Raymond (2017), BSD 2-clause -- see `COPYING`.
- **PDCurses**: William McBrine and contributors (public domain).
- **PicoCalc driver kit and PDCurses backend pattern**: Blair Leduc
  ([picocalc-text-starter](https://github.com/BlairLeduc/picocalc-text-starter), MIT).
- **PicoCalc port**: Chris Torkildson, 2026, with Claude's help.
