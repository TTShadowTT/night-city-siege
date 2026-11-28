/* ui_style.c - UI STYLE presets: the colours and shapes of every menu look. */
#include "core.h"

/* UI STYLE presets, all free: hand-made looks first, then one matching each menu background
   (the picture, then every map). 0 in edge / titleA / titleB = take it from the current map. deco is the
   panel shape: 0 plain, 1 cyberpunk (accent bar + clipped corner), 2 double line, 3 corner brackets. */
const UiStyle UI_HAND[] = {
    { "CLASSIC", C(18, 16, 30), C(40, 32, 62), 0, C(255, 255, 255), C(225, 225, 240), C(255, 230, 120), C(150, 150, 170), 0, 0, 0 },
    { "CYBERPUNK", C(8, 10, 14), C(46, 42, 6), C(0, 200, 215), C(252, 238, 10), C(240, 240, 230), C(252, 238, 10), C(0, 190, 200), C(252, 238, 10), C(0, 240, 255), 1 },
    { "ARCADE", C(0, 0, 40), C(0, 0, 110), C(80, 80, 255), C(255, 255, 0), C(255, 255, 255), C(255, 255, 0), C(255, 120, 200), C(255, 60, 60), C(255, 255, 0), 2 },
    { "TERMINAL", C(0, 12, 0), C(0, 40, 6), C(0, 140, 40), C(80, 255, 120), C(80, 230, 100), C(190, 255, 190), C(40, 150, 60), C(60, 255, 90), C(40, 180, 70), 3 },
    { "VAPORWAVE", C(40, 10, 50), C(90, 20, 100), C(255, 110, 200), C(100, 240, 255), C(255, 220, 250), C(100, 240, 255), C(200, 140, 220), C(255, 120, 210), C(110, 230, 255), 0 },
    { "BLOOD MOON", C(24, 4, 6), C(70, 8, 12), C(170, 20, 30), C(255, 90, 80), C(240, 210, 200), C(255, 120, 90), C(170, 110, 110), C(255, 50, 50), C(255, 150, 60), 3 },
    { "ICE", C(10, 24, 36), C(30, 60, 90), C(120, 200, 240), C(255, 255, 255), C(220, 245, 255), C(150, 240, 255), C(120, 160, 190), C(160, 230, 255), C(230, 250, 255), 2 },
    { "GOLD", C(24, 18, 6), C(60, 44, 10), C(190, 140, 40), C(255, 230, 140), C(245, 230, 200), C(255, 214, 80), C(170, 140, 90), C(255, 200, 60), C(255, 240, 180), 2 },
    { "MONOCHROME", C(12, 12, 12), C(60, 60, 60), C(120, 120, 120), C(255, 255, 255), C(220, 220, 220), C(255, 255, 255), C(130, 130, 130), C(240, 240, 240), C(160, 160, 160), 0 },
    { "SAKURA", C(36, 16, 26), C(80, 34, 56), C(240, 150, 190), C(255, 230, 240), C(255, 235, 245), C(255, 170, 210), C(200, 150, 175), C(255, 160, 200), C(255, 230, 240), 3 },
    { "TOXIC", C(10, 20, 4), C(30, 56, 6), C(150, 230, 30), C(230, 255, 60), C(220, 255, 190), C(230, 255, 60), C(140, 180, 90), C(170, 255, 40), C(255, 140, 30), 1 },
    { "SUNSET", C(36, 14, 20), C(90, 36, 30), C(255, 120, 60), C(255, 220, 120), C(255, 235, 215), C(255, 200, 90), C(220, 140, 120), C(255, 110, 70), C(255, 200, 100), 0 },
    { "HOLOGRAM", C(4, 20, 30), C(10, 50, 70), C(0, 200, 220), C(200, 255, 255), C(170, 240, 250), C(230, 255, 255), C(80, 170, 190), C(60, 230, 255), C(180, 120, 255), 3 },
    { "MILITECH", C(14, 16, 12), C(40, 44, 30), C(130, 140, 100), C(255, 150, 40), C(225, 225, 205), C(255, 150, 40), C(150, 155, 130), C(255, 150, 40), C(200, 205, 170), 1 },
    { "DAVID AND LUCY", C(16, 10, 30), C(52, 20, 70), C(200, 70, 255), C(120, 255, 90), C(225, 235, 255), C(140, 255, 100), C(150, 190, 230), C(220, 90, 255), C(110, 240, 255), 1 },
};
_Static_assert(sizeof UI_HAND / sizeof UI_HAND[0] == UI_HAND_N, "UI_HAND_N must match the entries in UI_HAND");

/* shaped styles, added later: their numbers come after the map styles so saved picks keep their meaning,
   but the picker shows them right after the hand-made ones. deco (see shapedPanel): 4 pill, 5 neon tube,
   6 sci-fi cut corners, 7 waves + bubbles, 8 flames, 9 drips, 10 spider webs, 11 snow + candy cane,
   12 glitch, 13 comic, 14 vines, 15 cyberpsychosis (cracks, doubled ghosts, a green bolt) */
const UiStyle UI_SHAPED[] = {
    { "HALLOWEEN", C(22, 10, 30), C(60, 24, 70), C(255, 120, 20), C(255, 200, 60), C(255, 225, 190), C(255, 150, 30), C(170, 130, 170), C(255, 130, 20), C(170, 80, 255), 10 },
    { "CHRISTMAS", C(10, 40, 22), C(20, 80, 40), C(220, 30, 40), C(255, 255, 255), C(245, 250, 245), C(255, 215, 80), C(160, 200, 170), C(235, 40, 50), C(60, 200, 90), 11 },
    { "SLIME", C(10, 24, 8), C(26, 56, 14), C(110, 230, 40), C(190, 255, 90), C(215, 255, 190), C(200, 255, 80), C(120, 170, 90), C(120, 255, 50), C(220, 255, 120), 9 },
    { "VAMPIRE", C(20, 4, 8), C(54, 8, 16), C(180, 10, 24), C(255, 40, 50), C(240, 215, 215), C(255, 70, 70), C(160, 100, 110), C(220, 20, 40), C(255, 230, 230), 9 },
    { "BUBBLEGUM", C(60, 30, 72), C(130, 60, 140), C(255, 150, 210), C(140, 240, 255), C(255, 240, 250), C(150, 245, 255), C(220, 170, 220), C(255, 140, 210), C(140, 230, 255), 4 },
    { "NEON TUBE", C(8, 6, 18), C(26, 12, 46), C(255, 40, 180), C(60, 255, 255), C(255, 220, 245), C(90, 255, 255), C(170, 120, 200), C(255, 50, 190), C(60, 240, 255), 5 },
    { "SCI-FI", C(8, 18, 30), C(16, 40, 64), C(70, 150, 255), C(255, 255, 255), C(200, 225, 255), C(120, 220, 255), C(100, 140, 190), C(120, 200, 255), C(255, 255, 255), 6 },
    { "OCEAN", C(4, 26, 48), C(8, 60, 96), C(40, 170, 230), C(170, 250, 255), C(210, 240, 255), C(120, 255, 230), C(90, 160, 200), C(60, 200, 255), C(120, 255, 220), 7 },
    { "INFERNO", C(26, 8, 4), C(70, 20, 6), C(255, 90, 20), C(255, 220, 60), C(255, 230, 200), C(255, 210, 60), C(200, 120, 80), C(255, 110, 20), C(255, 230, 80), 8 },
    { "GLITCH", C(10, 10, 14), C(30, 30, 40), C(230, 230, 240), C(255, 255, 255), C(230, 230, 240), C(255, 255, 255), C(140, 140, 160), C(255, 40, 90), C(0, 240, 255), 12 },
    { "COMIC", C(30, 60, 140), C(200, 30, 40), C(10, 10, 14), C(10, 10, 14), C(255, 255, 255), C(255, 240, 60), C(200, 200, 220), C(255, 230, 40), C(255, 60, 60), 13 },
    { "JUNGLE", C(14, 30, 12), C(34, 64, 24), C(120, 90, 40), C(200, 160, 70), C(225, 240, 200), C(170, 255, 90), C(140, 170, 110), C(110, 220, 60), C(240, 200, 80), 14 },
    { "ONYX", C(6, 6, 6), C(64, 64, 64), C(190, 190, 190), C(255, 255, 255), C(230, 230, 230), C(255, 255, 255), C(140, 140, 140), C(255, 255, 255), C(120, 120, 120), 4 },
    { "CYBERPSYCHOSIS", C(14, 8, 34), C(48, 14, 74), C(255, 50, 210), C(130, 255, 60), C(235, 220, 255), C(130, 255, 60), C(170, 140, 210), C(255, 50, 210), C(0, 225, 255), 15 },
    /* colour-blind safe: only blue, sky blue, orange and yellow (Okabe-Ito), never red against green; the health bar follows it too */
    { "COLOURBLIND", C(8, 14, 34), C(0, 60, 120), C(0, 114, 178), C(240, 228, 66), C(255, 255, 255), C(240, 228, 66), C(150, 190, 220), C(230, 159, 0), C(86, 180, 233), 4 },
};
_Static_assert(sizeof UI_SHAPED / sizeof UI_SHAPED[0] == UI_SHAPED_N, "UI_SHAPED_N must match the entries in UI_SHAPED");

/* picker order (hand-made, shaped, then the map looks) <-> style number */
int uiAt(int slot)
{
    if (slot < UI_HAND_N) return slot;
    if (slot < UI_HAND_N + UI_SHAPED_N) return UI_HAND_N + D_COUNT + slot - UI_HAND_N;
    return slot - UI_SHAPED_N;
}
int uiSlot(int i)
{
    if (i < UI_HAND_N) return i;
    if (i < UI_HAND_N + D_COUNT) return i + UI_SHAPED_N;
    return i - D_COUNT;
}

int uiAfterNewMaps(int i, int oldMaps) /* a UI style number saved when there were oldMaps maps */
{
    return i >= UI_HAND_N + oldMaps ? i + D_COUNT - oldMaps : i;
}

UiStyle uiStyleOf(int i)
{
    if (i < UI_HAND_N) return UI_HAND[i];
    if (i >= UI_HAND_N + D_COUNT) return UI_SHAPED[i - UI_HAND_N - D_COUNT];
    const District *d = &DISTRICTS[i - UI_HAND_N]; /* a map's look, built from its sky and neon colours */
    UiStyle s = { d->name, mixc(d->skyTop, C(0, 0, 0), 0.45f), mixc(d->skyTop, d->neonB, 0.35f), scalec(d->neonB, 190),
                  mixc(d->neonA, C_WHITE, 0.5f), mixc(d->neonA, C_WHITE, 0.8f), mixc(d->neonA, C_WHITE, 0.3f),
                  mixc(d->neonB, C(150, 150, 170), 0.5f), d->neonA, d->neonB, i % 4 };
    return s;
}

void setUi(int i)
{
    ui = uiStyleOf(clampi(i, 0, UI_COUNT - 1));
    if (!ui.edge) ui.edge = scalec(D->neonB, 150);
    if (!ui.titleA) ui.titleA = D->neonA;
    if (!ui.titleB) ui.titleB = D->neonB;
}
