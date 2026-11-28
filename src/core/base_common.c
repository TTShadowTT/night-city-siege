/* base_common.c - Shared constants, random numbers, colour maths and a tiny string formatter. */
#include "core.h"

unsigned rng = 2463534242u;

const int bayer[4][4] = { { 0, 8, 2, 10 }, { 12, 4, 14, 6 }, { 3, 11, 1, 9 }, { 15, 7, 13, 5 } };
void sfmt(char *out, int cap, const char *f, ...)
{
    va_list ap;
    int n = 0;
    va_start(ap, f);
    for (; *f && n < cap - 1; f++) {
        if (f[0] == '%' && f[1] == 'd') {
            int v = va_arg(ap, int), k = 0;
            unsigned u = v < 0 ? 0u - (unsigned)v : (unsigned)v;
            char tmp[12];
            if (v < 0) out[n++] = '-';
            do { tmp[k++] = (char)('0' + u % 10); u /= 10; } while (u);
            while (k && n < cap - 1) out[n++] = tmp[--k];
            f++;
        } else if (f[0] == '%' && f[1] == 's') {
            const char *s = va_arg(ap, const char *);
            while (*s && n < cap - 1) out[n++] = *s++;
            f++;
        } else {
            out[n++] = *f;
        }
    }
    out[n] = 0;
    va_end(ap);
}

unsigned *fb, *farL, *midL, *foreL;
short *sndPool, *musPool;

unsigned game_workspace_size(void)
{
    return (unsigned)((FW * FH + FARW * FH + MIDW * FH + FW * FH + 11 * FW * FH) * 4 + (11 + 6) * (FW / 2) * (FH / 2) /* picture blocks */ + SND_POOL * 2 + MUS_TRACKS * MUS_MAXLEN * 2 + 64);
}
