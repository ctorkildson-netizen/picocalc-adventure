#include "pdcpicocalc.h"
#include <pico/stdlib.h>

/* Adapted from picocalc-empire's picocalc/pdcutil.c. That version also
   defines getuid()/getpwuid()/chmod() stubs -- Empire's own source
   calls getpwuid() to find a home directory for its save file, which
   the Pico SDK's minimal libc has no real implementation of. Adventure
   never calls any of those (its save/resume just fopen()s whatever
   filename the player typed -- see saveresume.c), so there's nothing
   here for this port to stand in for. */

void PDC_beep(void)
{
    audio_play_sound_blocking(PITCH_A5, PITCH_A5, NOTE_EIGHTH);
}

const char *PDC_sysname(void)
{
    return "PicoCalc";
}

void PDC_napms(int ms)
{
    tight_loop_contents();
    sleep_ms(ms);
}
