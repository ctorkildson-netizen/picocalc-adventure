# Top-level (real PicoCalc hardware) build of Colossal Cave Adventure.
# Mirrors picocalc-empire's top-level CMakeLists.txt closely -- same
# Pico SDK boilerplate, same picocalc-text-starter driver library, same
# copy+patch approach for PDCurses -- with two differences worth
# knowing about going in:
#
#   * PICO_BOARD is now "pico2" (RP2350). It was "pico" (RP2040) through
#     Sept 2026; Oct 2026 retargeted to the RP2350W along with Empire, and
#     picked up Empire's fixed picocalc/ display layer (font, keyboard
#     polling, colour, cursor -- see the comments in picocalc/pdc*.c).
#
#   * There's no copy+patch step for Adventure's own sources the way
#     picocalc-empire has one for empire/. Empire vendors an untouched
#     upstream copy under modules/empire and patches it at build time;
#     Adventure's ADVENT_CURSES/ADVENT_PICOCALC support was written
#     directly into main.c/misc.c/advent.h/etc. as ordinary #ifdef
#     blocks (see those files), so they're just plain sources here,
#     structured that way so the byte-for-byte-verified plain (non-
#     curses) regression suite keeps passing on the exact same files.
#     PDCurses itself still gets copy+patched, below, same as Empire.
#
# Build with:
#
#   cmake -S . -B build
#   cmake --build build
#
# ...then copy the resulting build/picocalc-adventure.uf2 onto the
# PicoCalc while it's in BOOTSEL mode, same as any other Pico SDK UF2.

cmake_minimum_required(VERSION 3.13)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type") # RP2350 -- was "pico" (RP2040) until Oct 2026

include(pico_sdk_import.cmake)

project(picocalc-adventure C CXX ASM)

# The Pico SDK's own headers use static_assert(), which needs C11 (they
# expect the <assert.h> macro C11 provides, not C99). Adventure's own
# code is happy under C11 too, so there's no need for a per-file
# override the way the pdcurses/empire C89 quirks would need.
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_sdk_init()

set(STARTER_KIT_SOURCES
    modules/picocalc-text-starter/drivers/audio.c
    modules/picocalc-text-starter/drivers/clib.c
    modules/picocalc-text-starter/drivers/fat32.c
    modules/picocalc-text-starter/drivers/font-5x10.c
    modules/picocalc-text-starter/drivers/font-8x10.c
    modules/picocalc-text-starter/drivers/lcd.c
    modules/picocalc-text-starter/drivers/sdcard.c
    modules/picocalc-text-starter/drivers/southbridge.c
)

file(GLOB PICOCALC_SOURCES CONFIGURE_DEPENDS
    picocalc/*.c
)

# --- Copy+patch PDCurses' generic core, same approach as picocalc-empire ---
# (Only the core (pdcurses/) needs patching -- see patches/pdcurses.patch's
# own comment for what it fixes and why. The picocalc/ backend above is
# ours already, and needs no patching of its own.)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_LIST_DIR}/pdcurses/.patched
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_CURRENT_LIST_DIR}/pdcurses
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_LIST_DIR}/modules/PDCurses/pdcurses
            ${CMAKE_CURRENT_LIST_DIR}/pdcurses
    COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/scripts/apply_pdcurses_patches.sh
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_LIST_DIR}/pdcurses/.patched
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/patches/pdcurses.patch
            ${CMAKE_CURRENT_LIST_DIR}/scripts/apply_pdcurses_patches.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    COMMENT "Copying and patching pdcurses sources"
    VERBATIM
)

add_custom_target(patch_pdcurses ALL
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/pdcurses/.patched
)

file(GLOB PDCURSES_SOURCES CONFIGURE_DEPENDS
    pdcurses/*.c
)

# Ninja won't otherwise notice that a patch change should force a
# rebuild -- see picocalc-empire's CMakeLists.txt for the full
# explanation of why this OBJECT_DEPENDS is needed.
set_source_files_properties(${PDCURSES_SOURCES} PROPERTIES OBJECT_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/pdcurses/.patched)

# --- Adventure's own sources -- plain files, no copy/patch step (see header) ---
set(ADVENTURE_SOURCES
    main.c
    init.c
    actions.c
    score.c
    misc.c
    saveresume.c
    dungeon.c
)

add_executable(picocalc-adventure
    ${PDCURSES_SOURCES}
    ${STARTER_KIT_SOURCES}
    ${PICOCALC_SOURCES}
    ${ADVENTURE_SOURCES}
)

add_dependencies(picocalc-adventure patch_pdcurses)

target_compile_definitions(picocalc-adventure PRIVATE
    ADVENT_CURSES
    ADVENT_PICOCALC
    VERSION=\"1.7\"
)

pico_set_program_name(picocalc-adventure "picocalc-adventure")
pico_set_program_url(picocalc-adventure "https://github.com/ctorkildson-netizen/picocalc-adventure")

pico_generate_pio_header(picocalc-adventure ${CMAKE_CURRENT_LIST_DIR}/modules/picocalc-text-starter/drivers/audio.pio)

pico_enable_stdio_uart(picocalc-adventure 0)
pico_enable_stdio_usb(picocalc-adventure 0)

target_link_libraries(picocalc-adventure
    pico_stdlib
    pico_printf
    pico_rand
    pico_bootrom
    hardware_gpio
    hardware_i2c
    hardware_spi
    hardware_pio
    hardware_clocks)

target_include_directories(picocalc-adventure PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/pdcurses
    ${CMAKE_CURRENT_LIST_DIR}/modules/PDCurses
    ${CMAKE_CURRENT_LIST_DIR}/modules/picocalc-text-starter
    ${CMAKE_CURRENT_LIST_DIR}/picocalc
)

pico_add_extra_outputs(picocalc-adventure)
