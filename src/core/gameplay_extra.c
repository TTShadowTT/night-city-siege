/* gameplay_extra.c - Extra menu input and the text that goes with it. */
#include "core.h"

static const unsigned char XD[568] = {
    229, 136, 160, 189, 80, 111, 20, 62, 195, 172, 154, 163, 64, 114, 120, 14, 87, 199, 250, 135, 165, 40, 112, 12, 
    19, 51, 218, 253, 150, 180, 55, 10, 98, 23, 129, 202, 243, 157, 213, 80, 99, 13, 42, 205, 244, 129, 145, 182, 
    55, 102, 20, 35, 206, 229, 135, 167, 173, 79, 25, 15, 60, 197, 173, 133, 171, 64, 1, 109, 24, 48, 218, 253, 
    131, 236, 76, 104, 23, 37, 47, 186, 227, 156, 180, 46, 120, 13, 38, 208, 218, 232, 249, 181, 92, 116, 8, 74, 
    200, 234, 225, 138, 179, 93, 21, 31, 46, 197, 231, 230, 174, 69, 83, 111, 87, 54, 212, 230, 130, 166, 65, 2, 
    107, 20, 60, 182, 224, 147, 168, 68, 98, 23, 97, 56, 210, 246, 145, 210, 91, 100, 12, 70, 199, 229, 235, 159, 
    187, 91, 97, 11, 57, 168, 247, 141, 144, 177, 25, 119, 115, 35, 200, 233, 149, 161, 85, 62, 22, 120, 38, 221, 
    226, 137, 201, 73, 111, 4, 125, 61, 214, 249, 148, 189, 43, 96, 4, 38, 95, 221, 153, 144, 178, 93, 98, 31, 
    52, 164, 226, 241, 159, 189, 21, 115, 111, 63, 204, 229, 145, 165, 169, 58, 26, 116, 38, 198, 228, 232, 168, 67, 
    91, 121, 121, 55, 223, 252, 237, 190, 79, 109, 18, 97, 123, 220, 240, 132, 170, 64, 102, 22, 38, 210, 189, 232, 
    152, 187, 92, 46, 5, 33, 194, 234, 235, 252, 186, 95, 103, 9, 77, 217, 238, 129, 166, 187, 27, 117, 20, 54, 
    202, 140, 139, 191, 3, 116, 105, 9, 63, 213, 245, 129, 188, 92, 113, 66, 119, 56, 216, 248, 154, 181, 65, 10, 
    17, 45, 213, 183, 187, 159, 188, 70, 103, 25, 43, 168, 224, 143, 144, 213, 67, 96, 2, 38, 202, 236, 138, 181, 
    171, 72, 57, 31, 55, 213, 236, 153, 203, 36, 101, 123, 8, 49, 210, 252, 239, 173, 71, 98, 99, 52, 56, 201, 
    227, 157, 191, 73, 17, 104, 49, 214, 204, 244, 152, 178, 92, 103, 25, 62, 135, 231, 142, 154, 190, 56, 98, 0, 
    38, 216, 253, 131, 173, 32, 73, 117, 16, 49, 197, 230, 142, 186, 37, 117, 118, 8, 49, 182, 240, 156, 172, 95, 
    99, 1, 97, 55, 213, 235, 133, 187, 93, 105, 13, 70, 193, 249, 157, 153, 174, 86, 116, 28, 59, 221, 235, 137, 
    255, 206, 41, 1, 100, 80, 204, 228, 131, 196, 68, 90, 124, 29, 39, 199, 225, 130, 172, 84, 112, 64, 4, 53, 
    194, 142, 241, 203, 111, 8, 14, 43, 51, 208, 234, 246, 222, 48, 8, 46, 71, 198, 238, 237, 136, 189, 70, 18, 
    98, 76, 172, 194, 227, 183, 188, 76, 114, 7, 113, 171, 207, 232, 170, 68, 63, 25, 61, 86, 210, 243, 133, 163, 
    66, 114, 4, 19, 62, 214, 225, 129, 207, 33, 9, 4, 38, 211, 201, 154, 132, 183, 94, 124, 14, 72, 160, 198, 
    191, 136, 177, 87, 125, 27, 62, 170, 225, 139, 179, 222, 75, 116, 20, 43, 198, 226, 142, 202, 35, 99, 117, 21, 
    56, 217, 144, 206, 165, 71, 5, 17, 22, 59, 215, 247, 159, 240, 70, 99, 9, 35, 129, 207, 238, 157, 176, 92, 
    47, 78, 107, 133, 160, 192, 220, 250, 22, 118, 16, 105, 136, 171, 207, 236 };
static const unsigned short XO[24] = { 0, 10, 35, 63, 74, 113, 131, 155, 199, 229, 244, 261, 269, 280, 293, 315, 353, 387, 430, 467, 505, 532, 542, 547 };
#define XN 553

static int xb(int i) { return XD[i] ^ ((0xA7 + i * 29) & 255); }
const char *xs(int id)
{
    static char buf[4][64];
    static int n;
    char *s = buf[n++ & 3];
    int i = 0;
    for (int p = XO[id]; i < 63 && xb(p); p++) s[i++] = (char)xb(p);
    s[i] = 0;
    return s;
}

char extraSkinName[16], extraSkinName2[16], extraSkinName3[16], extraAchText[4][48];
void extraInit(void)
{
    sfmt(extraSkinName, sizeof extraSkinName, "%s", xs(0));
    sfmt(extraSkinName2, sizeof extraSkinName2, "%s", xs(22));
    sfmt(extraSkinName3, sizeof extraSkinName3, "%s", xs(23));
    for (int i = 0; i < 4; i++) sfmt(extraAchText[i], sizeof extraAchText[i], "%s", xs(3 + i));
}

static int step;
static float holdT;
static void done(void)
{
    char s[64];
    step = 0;
    if (!((skinsMask >> SKIN_X) & 1)) {
        skinsMask |= 1 << SKIN_X;
        sfmt(s, sizeof s, xs(1), SKINS[SKIN_X].name);
        toast(s);
    } else toast(xs(2));
    sfx(S_WARP, 0.9f);
    achGive(ACH_S1);
    saveGame();
}
static void miss(int key) { step = key == xb(XN) ? (step == 2 ? 2 : 1) : 0; }

int extraKey(int key)
{
    if (state != ST_MENU) { step = 0; return 0; }
    if (step < 8) {
        if (key == xb(XN + step)) step++;
        else miss(key);
        return 0;
    }
    if (step == 8 && key == K_ESC) { step = 9; return 1; }
    if (step == 9 && (key == K_JUMP || key == K_ENTER)) { done(); return 1; }
    if (step == 9 && key == xb(XN + 4)) return 0;
    miss(key);
    return 0;
}

void extraChar(int ch)
{
    if (state != ST_MENU) return;
    if (step == 8 && ch == xb(XN + 8)) step = 9;
    else if (step == 9 && ch == xb(XN + 9)) done();
    else if (ch != 'W' && ch != 'A' && ch != 'S' && ch != 'D') step = 0;
}

void extraTick(float dt)
{
    if (state != xb(XN + 10) || !held[xb(XN + 11)]) { holdT = 0; return; }
    if ((holdT += dt) <= xb(XN + 12)) return;
    holdT = 0; extraTv = 0;
    state = ST_EXTRA; focusBtn = 0;
    sfx(S_WARP, 0.8f);
    achGive(ACH_S2);
}

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

/* two of them come with achievements */
static int extraFor(int id)
{
    for (int i = 0; i < 2; i++)
        if (id == xb(XN + 13 + i)) return SKIN_X - 2 + i;
    return -1;
}
void extraAch(int id)
{
    char s[64];
    int k = extraFor(id);
    if (k < 0 || ((skinsMask >> k) & 1)) return;
    skinsMask |= 1 << k;
    sfmt(s, sizeof s, xs(1), SKINS[k].name);
    toast(s);
    sfx(S_WARP, 0.9f);
}
void extraOwned(void)
{
    for (int i = 0; i < 2; i++)
        if (achDone(xb(XN + 13 + i))) skinsMask |= 1 << (SKIN_X - 2 + i);
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
