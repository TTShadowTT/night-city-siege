/* gameplay_extra.c - The Extra screens: which skin cards show, the developer room's TV and link.
   The hidden parts (codes, texts) are in secrets.c, which stays private; secrets_stub.c stands in without it. */
#include "core.h"

_Static_assert(SKIN_X == SKIN_COUNT - 1, "SKIN_X must stay the last skin");
/* the last SKIN_HID skins are listed only once they're owned (the real mask: developer mode doesn't show them) */
#define SKIN_HID 3
int skinShownAt(int n)
{
    for (int k = SKIN_COUNT - SKIN_HID; k < SKIN_COUNT && n >= SKIN_COUNT - SKIN_HID; k++)
        if ((skinsMask >> k) & 1) { if (n == SKIN_COUNT - SKIN_HID) return k; n--; }
    return n < SKIN_COUNT - SKIN_HID ? n : -1;
}
int skinsShown(void)
{
    int n = SKIN_COUNT - SKIN_HID;
    while (skinShownAt(n) >= 0) n++;
    return n;
}
int skinShownPos(int k)
{
    for (int n = 0; n < skinsShown(); n++)
        if (skinShownAt(n) == k) return n;
    return 0;
}

int extraTv;
float extraTvT;
int extraTvAt(int x, int y) { return x >= 66 && x < 150 && y >= 80 && y < 142; }

/* the GitHub line in the credits is a link: clicking it opens the page in the browser */
int extraLinkAt(int x, int y)
{
    int w = textWidth(xs(14), 1);
    return y >= 107 && y < 120 && x >= EXTRA_CX - w / 2 - 3 && x < EXTRA_CX + w / 2 + 3;
}
void extraOpenLink(void)
{
    char url[64];
    sfmt(url, sizeof url, "https://%s", xs(14));
    for (char *c = url; *c; c++) if (*c >= 'A' && *c <= 'Z') *c += 32;
    plat_open_url(url);
}
