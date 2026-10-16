# Native (SDL2) desktop build of Adventure -- for running/testing on
# this PC directly, without a PicoCalc.
#
# Unlike picocalc-empire, there's no separate pc/main.c here. Empire
# needed one because its top-level main.c is PicoCalc-hardware-specific
# and empire_main() has no real main() of its own to hand the
# SDL2main rename trick to. Adventure's own main.c (one directory up)
# already contains a complete, ordinary main(argc, argv) -- the same
# file this build compiles for curses I/O also becomes the SDL2
# desktop entry point once ADVENT_SDL is defined below, which is what
# makes it #include <SDL.h> (see the top of main.c for why that
# matters). One shared main.c, no duplicate wrapper to keep in sync.
#
# This is a separate CMake project from a future top-level (PicoCalc
# hardware) one on purpose, exactly as in picocalc-empire: a bare-metal
# ARM cross-compile toolchain and a native desktop toolchain can't
# share one configure. Build and run it with:
#
#   cmake -S pc -B build-pc
#   cmake --build build-pc
#   build-pc\picocalc-adventure-pc.exe
#
# It compiles the exact same curses-enabled Adventure sources (one
# directory up) that already pass the plain-build regression suite
# unchanged, plus PDCurses' own sdl2 backend (modules/PDCurses/sdl2)
# in place of a hardware backend. Both are used straight from the
# PDCurses submodule, unpatched -- no copy+patch step like Empire's,
# because nothing here needs patching (Empire's pdcurses.patch and
# empire.patch exist for problems specific to Empire's 1988 source and
# to a hardware getch() quirk; Adventure/this build hits neither).

cmake_minimum_required(VERSION 3.13)
project(picocalc-adventure-pc C)

set(CMAKE_C_STANDARD 99)

set(REPO_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

find_package(SDL2 REQUIRED)

# --- Adventure's own sources, straight from the parent directory ---
# dungeon.c/dungeon.h are generated (by ../make_dungeon.py, from
# ../adventure.yaml) rather than hand-written, but they're committed
# here as plain, ordinary source/header files -- so this build, unlike
# picocalc-empire's copy+patch custom commands, needs no generation
# step and no Python on your machine at build time. If adventure.yaml
# is ever edited, regenerate them the same way the original project
# does (./make_dungeon.py from the repo root) and commit the result.
set(ADVENTURE_SOURCES
    ${REPO_ROOT}/main.c
    ${REPO_ROOT}/init.c
    ${REPO_ROOT}/actions.c
    ${REPO_ROOT}/score.c
    ${REPO_ROOT}/misc.c
    ${REPO_ROOT}/saveresume.c
    ${REPO_ROOT}/dungeon.c
)

# --- PDCurses' core (generic) implementation, straight from the submodule ---
file(GLOB PDCURSES_SOURCES ${REPO_ROOT}/modules/PDCurses/pdcurses/*.c)

# --- PDCurses' own sdl2 backend, straight from the submodule ---
# sdltest.c is PDCurses' own standalone demo program (it defines its
# own main()) -- excluded, since main.c (via ADVENT_CURSES/ADVENT_SDL)
# supplies ours.
file(GLOB SDL2_BACKEND_SOURCES ${REPO_ROOT}/modules/PDCurses/sdl2/*.c)
list(REMOVE_ITEM SDL2_BACKEND_SOURCES ${REPO_ROOT}/modules/PDCurses/sdl2/sdltest.c)

add_executable(picocalc-adventure-pc
    ${ADVENTURE_SOURCES}
    ${PDCURSES_SOURCES}
    ${SDL2_BACKEND_SOURCES}
)

# ADVENT_CURSES switches Adventure's own I/O between curses calls and
# the original readline/printf calls (both are still built from the
# very same source files the plain, non-curses regression suite
# verifies unchanged). ADVENT_SDL additionally pulls in <SDL.h> inside
# main.c for the SDL2main linking trick -- see the comment there. The
# plain ncurses sandbox build defines only ADVENT_CURSES; this is the
# one build that defines both.
target_compile_definitions(picocalc-adventure-pc PRIVATE
    ADVENT_CURSES
    ADVENT_SDL
    VERSION=\"1.7\"
)

target_include_directories(picocalc-adventure-pc PRIVATE
    ${REPO_ROOT}
    ${REPO_ROOT}/modules/PDCurses
    ${REPO_ROOT}/modules/PDCurses/sdl2
)

# SDL2main must be listed before SDL2 -- it's a static lib that itself
# calls into SDL2 (SDL_strlen, SDL_SetMainReady, etc.), and the GNU
# linker only looks forward through the libraries it's given, never
# back, to resolve a symbol. Reversed, it links "successfully" build
# after build until the exact moment it doesn't -- the reported
# undefined references (SDL_main, SDL_SetMainReady...) are the tell
# that this is a link-order problem, not a missing library.
target_link_libraries(picocalc-adventure-pc PRIVATE SDL2::SDL2main SDL2::SDL2)
