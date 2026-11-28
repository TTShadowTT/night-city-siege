/* ui_screens_options.c - Menu screens: options, menu title editor and the soundtrack list. */
#include "core.h"

static void marquee(int x, int y, int w, const char *s, unsigned c, int alignRight, int scale);

void renderOptions(void)
{
    char s[16];
    if (optFromPause) for (int i = 0; i < FW * FH; i++) fb[i] = scalec(fb[i], 90);
    text(FW / 2, scrY(8), state == ST_UPGRADES ? "UPGRADES" : "OPTIONS", C_WHITE, 3, AL_CENTER);
    for (int i = 0; i < nBtns; i++) {
        const Btn *b = &btns[i];
        int focus = i == focusBtn, a = b->action;
        if (a == A_BACK) { drawButton(b, focus); continue; }
        panel(b->x, b->y, b->w, b->h, focus);
        text(b->x + 10, b->y + 5, b->label, focus ? ui.hi : ui.txt, 2, AL_LEFT);
        if (isSlider(a)) {
            int lim = a == A_OPT_PLIMN; /* MAX UPGRADES runs 1 .. PERK_TOTAL instead of 0 .. 100% */
            int v = a == A_OPT_MVOL ? musicVolPct : a == A_OPT_SVOL ? sfxVolPct : perkLimit, by = b->y + 8;
            int fill = lim ? SLIDER_W * (v - 1) / (PERK_TOTAL - 1) : SLIDER_W * v / 100, kx = SLIDER_X0 + fill;
            int dim = a == A_OPT_MVOL && !musicOn;
            fillRect(SLIDER_X0 - 1, by - 1, SLIDER_W + 2, 6, C_OUT);
            fillRect(SLIDER_X0, by, SLIDER_W, 4, C(50, 48, 64));
            fillRect(SLIDER_X0, by, fill, 4, dim ? C(110, 110, 120) : lim ? C(255, 214, 80) : C(120, 220, 255));
            if (lim) pset(SLIDER_X0 + SLIDER_W * (PERK_DEFAULT - 1) / (PERK_TOTAL - 1), by + 5, C(255, 214, 80)); /* the normal amount */
            else for (int t = 0; t <= 4; t++) pset(SLIDER_X0 + SLIDER_W * t / 4, by + 5, C(110, 110, 130));
            fillRect(kx - 2, by - 3, 5, 10, C_OUT);
            fillRect(kx - 1, by - 2, 3, 8, focus || sliderDrag == a ? ui.hi : C_WHITE);
            sfmt(s, sizeof s, "%d", v);
            if (!lim) { s[strlen(s) + 1] = 0; s[strlen(s)] = '%'; }
            text(b->x + b->w - 10, b->y + 7, s, C_WHITE, 1, AL_RIGHT);
        } else if (a == A_OPT_UPG) { /* opens its own screen; sums up what's set there */
            int r = b->x + b->w - 10;
            if (!perksOn) sfmt(s, sizeof s, "OFF");
            else if (!perkLimitOn) sfmt(s, sizeof s, "INFINITE");
            else sfmt(s, sizeof s, "MAX %d", perkLimit);
            text(r - 8, b->y + 7, s, perksOn ? C(255, 214, 80) : C(230, 90, 90), 1, AL_RIGHT);
            text(r, b->y + 7, ">", focus ? ui.hi : ui.dim, 1, AL_RIGHT);
        } else if (a == A_OPT_MENUBG) { /* opens its own screen; shows the current pick */
            const char *nm = menuBgName();
            int r = b->x + b->w - 10;
            text(r - 8, b->y + 7, nm, menuBg >= 0 ? DISTRICTS[menuBg].neonA : C(160, 220, 255), 1, AL_RIGHT);
            text(r, b->y + 7, ">", focus ? ui.hi : ui.dim, 1, AL_RIGHT);
        } else if (a == A_OPT_RESET) { /* click twice: the first click arms it for 3 seconds */
            int armed = gt - resetArmT < 3;
            text(b->x + b->w - 10, b->y + 7, armed ? "CLICK AGAIN!" : "CLICK TWICE", armed && fmodf(gt, 0.5f) < 0.3f ? C(255, 90, 90) : armed ? C(255, 160, 160) : ui.dim, 1, AL_RIGHT);
        } else if (a == A_OPT_HARD && !hardUnlocked && !devMode) { /* locked until the campaign is beaten */
            int lx = b->x + b->w - 16, ly = b->y + 10; /* a small lock, so it fits the row */
            fillRect(lx - 2, ly - 4, 1, 3, C(190, 190, 200)); fillRect(lx + 2, ly - 4, 1, 3, C(190, 190, 200)); fillRect(lx - 1, ly - 5, 3, 1, C(190, 190, 200));
            frame(lx - 3, ly - 1, 7, 5, C(210, 170, 60), C_OUT);
            pset(lx, ly + 1, C_OUT);
            text(b->x + b->w - 24, b->y + 7, "BEAT THE CAMPAIGN", ui.dim, 1, AL_RIGHT);
        } else if (a == A_OPT_WIPE) { /* the dangerous one: red all over, so nobody mixes it up with RESET SETTINGS */
            frame(b->x, b->y, b->w, b->h, focus ? C(110, 20, 28) : C(70, 12, 18), focus ? C(255, 120, 120) : C(230, 50, 60));
            text(b->x + 10, b->y + 5, b->label, focus ? C_WHITE : C(255, 150, 150), 2, AL_LEFT);
            text(b->x + b->w - 10, b->y + 7, "DELETES ALL PROGRESS >", C(255, 90, 90), 1, AL_RIGHT);
        } else if (a == A_OPT_TRACK) { /* what's picked; a long name slides along so it can all be read */
            int r = b->x + b->w - 10;
            marquee(b->x + 112, b->y + 7, r - 8 - (b->x + 112), musicMode == 0 ? "EACH MAP ITS OWN SONG" : musicModeName(), C(160, 220, 255), 1, 1);
            text(r, b->y + 7, ">", focus ? ui.hi : ui.dim, 1, AL_RIGHT);
        } else if (a == A_OPT_SONGS) { /* how many, and it opens its own screen */
            int r = b->x + b->w - 10;
            char t[24];
            sfmt(t, sizeof t, myCount == 1 ? "1 SONG" : "%d SONGS", myCount);
            text(r - 8, b->y + 7, t, C(160, 220, 255), 1, AL_RIGHT);
            text(r, b->y + 7, ">", focus ? ui.hi : ui.dim, 1, AL_RIGHT);
        } else if (a == A_OPT_TITLE) { /* the price until it's bought, then the title's style */
            int r = b->x + b->w - 10;
            if (ownsTitle()) text(r - 8, b->y + 7, TSTY[titleStyle].name, C(160, 220, 255), 1, AL_RIGHT);
            else {
                sfmt(s, sizeof s, "%d", TITLE_COST);
                text(r - 8, b->y + 7, s, C(255, 214, 80), 1, AL_RIGHT);
                drawCoin(r - 14 - textWidth(s, 1), b->y + 9);
            }
            text(r, b->y + 7, ">", focus ? ui.hi : ui.dim, 1, AL_RIGHT);
        } else if (a == A_OPT_UI) { /* the current style's name in its own highlight colour */
            int r = b->x + b->w - 10;
            text(r - 8, b->y + 7, ui.name, ui.hi, 1, AL_RIGHT);
            text(r, b->y + 7, ">", focus ? ui.hi : ui.dim, 1, AL_RIGHT);
        } else {
            int on = a == A_OPT_MUSIC ? musicOn : a == A_OPT_SFX ? sfxOn : a == A_OPT_PERKS ? perksOn : a == A_OPT_PLIM ? perkLimitOn : a == A_OPT_DEV ? devMode : a == A_OPT_FULL ? fullOn : a == A_OPT_DMGNUM ? dmgNumsOn : a == A_OPT_HARD ? hardOn : shakeOn;
            int px_ = b->x + b->w - 50;
            frame(px_, b->y + 4, 40, 13, on ? C(30, 90, 50) : C(70, 30, 36), on ? C(120, 255, 140) : C(230, 90, 90));
            fillRect(on ? px_ + 26 : px_ + 3, b->y + 7, 11, 7, on ? C(120, 255, 140) : C(230, 90, 90));
            text(on ? px_ + 13 : px_ + 27, b->y + 8, on ? "ON" : "OFF", C_WHITE, 1, AL_CENTER);
        }
    }
    const char *hint = "";
    if (focusBtn < nBtns)
        switch (btns[focusBtn].action) {
        case A_OPT_MUSIC: hint = "THE SYNTHWAVE SOUNDTRACK (SHORTCUT: M)"; break;
        case A_OPT_MVOL: hint = "HOW LOUD THE MUSIC PLAYS"; break;
        case A_OPT_SFX: hint = "GUNS, EXPLOSIONS, HITS AND MENU SOUNDS - OFF = SILENT"; break;
        case A_OPT_SVOL: hint = "HOW LOUD THE SOUND EFFECTS ARE"; break;
        case A_OPT_PERKS: hint = "PICK AN UPGRADE CARD AFTER EVERY WAVE - OFF = CLASSIC MODE"; break;
        case A_OPT_UPG: hint = "CLICK TO TURN UPGRADE CARDS ON OR OFF AND SET THE LIMIT"; break;
        case A_OPT_PLIM: hint = "OFF = INFINITE UPGRADES - CARDS AFTER THE 10TH GET WEAKER"; break;
        case A_OPT_PLIMN: hint = "UPGRADES PER RUN - NORMAL IS 10, MORE THAN 10 ARE WEAKER"; break;
        case A_OPT_SHAKE: hint = "SCREEN SHAKE ON HITS AND EXPLOSIONS"; break;
        case A_OPT_DMGNUM: hint = "SHOW HOW HARD EACH HIT LANDS, OVER THE ENEMY"; break;
        case A_OPT_HARD: hint = !hardUnlocked && !devMode ? "CLEAR THE LAST DISTRICT OF THE CAMPAIGN TO UNLOCK IT"
                             : optFromPause ? "CHANGE IT FROM THE MAIN MENU OPTIONS" : "TOUGHER ENEMIES THAT HIT HARDER - BUT EVERY COIN COUNTS DOUBLE"; break;
        case A_OPT_FULL: hint = "FILL THE WHOLE SCREEN (SHORTCUT: F OR F11)"; break;
        case A_OPT_MENUBG: hint = optFromPause ? "CHANGE IT FROM THE MAIN MENU OPTIONS" : "CLICK TO PICK THE MAP BEHIND THE MAIN MENU - OR BUY ONE"; break;
        case A_OPT_UI: hint = optFromPause ? "CLICK OR LEFT / RIGHT FOR THE NEXT STYLE - ALL FREE" : "CLICK TO PICK HOW MENUS AND BUTTONS LOOK - ALL FREE"; break;
        case A_OPT_RESET: hint = "ONLY THE SETTINGS GO BACK TO DEFAULT - COINS, UNLOCKS AND YOUR SONGS ARE KEPT"; break;
        case A_OPT_WIPE: hint = optFromPause ? "ONLY FROM THE MAIN MENU OPTIONS" : "A FRESH START: DELETES ALL COINS, UNLOCKS AND PROGRESS - IT ASKS TWICE FIRST"; break;
        case A_OPT_SONGS: hint = optFromPause ? "CHANGE IT FROM THE MAIN MENU OPTIONS" : "ADD YOUR OWN MUSIC FILES AND PICK WHICH MAP THEY PLAY ON"; break;
        case A_OPT_TRACK: hint = "CLICK TO PICK A SONG FOR EVERY MAP OR ONE PER MAP - AND ADD YOUR OWN"; break;
        case A_OPT_TITLE: hint = optFromPause ? "CHANGE IT FROM THE MAIN MENU OPTIONS" : ownsTitle() ? "CLICK TO TYPE YOUR OWN MENU TITLE AND PICK ITS STYLE"
                                 : "BUY ONCE: TYPE YOUR OWN MENU TITLE AND PICK ITS STYLE"; break;
        }
    if (state == ST_UPGRADES) {
        text(FW / 2, 29, "CLICK, OR USE ARROWS - LEFT / RIGHT CHANGES A SETTING", ui.dim, 1, AL_CENTER);
        text(FW / 2, 242, hint, C(255, 214, 80), 1, AL_CENTER);
    }
    else text(FW / 2, scrY(29), *hint ? hint : "CLICK, OR USE ARROWS - LEFT / RIGHT CHANGES A SETTING", *hint ? C(255, 214, 80) : ui.dim, 1, AL_CENTER);
}

/* MENU TITLE screen: the title itself on top (live), the two lines to type into, then a card per style */
void renderTitleEdit(void)
{
    drawMenuTitle(10);
    for (int i = 0; i < nBtns; i++) {
        const Btn *b = &btns[i];
        int a = b->action, focus = i == focusBtn, cx = b->x + b->w / 2;
        if (a == A_TTL_L1 || a == A_TTL_L2) { /* a text field */
            int l = a == A_TTL_L1 ? 1 : 2, on = titleEdit == l;
            const char *t = l == 1 ? title1 : title2;
            char s[40];
            panel(b->x, b->y, b->w, b->h, focus || on);
            text(b->x + 8, b->y + 7, b->label, focus || on ? ui.hi : ui.dim, 1, AL_LEFT);
            sfmt(s, sizeof s, "%s%s", t, on && fmodf(gt, 0.8f) < 0.5f ? "_" : "");
            text(b->x + 48, b->y + 4, *t || on ? s : "(EMPTY)", on ? C_WHITE : *t ? ui.txt : ui.dim, 2, AL_LEFT);
            if (on) text(b->x + b->w - 8, b->y + 7, "ENTER = DONE", C(120, 255, 140), 1, AL_RIGHT);
        } else if (a >= A_TS0 && a < A_TS_END) { /* a style card: a sample in that style, its name, a green outline when it's the one */
            int k = a - A_TS0;
            panel(b->x, b->y, b->w, b->h, focus);
            int ps = TSTY[k].fx & FX_TALL || textWidthFx("TITLE", 3, TSTY[k].fx) > b->w - 10 ? 2 : 3;
            styledText(cx, b->y + 7 + (15 - titleH(ps, k)) / 2 + (ps == 2 ? 2 : 0), "TITLE", ps, k, 0);
            text(cx, b->y + 31, b->label, focus ? ui.hi : C_WHITE, 1, AL_CENTER);
            if (k == titleStyle) {
                unsigned g = C(120, 255, 140);
                fillRect(b->x + 2, b->y + 2, b->w - 4, 1, g); fillRect(b->x + 2, b->y + b->h - 3, b->w - 4, 1, g);
                fillRect(b->x + 2, b->y + 2, 1, b->h - 4, g); fillRect(b->x + b->w - 3, b->y + 2, 1, b->h - 4, g);
            }
        } else drawButton(b, focus);
    }
    text(FW / 2, 224, titleEdit ? "TYPE YOUR TITLE - BACKSPACE DELETES - ENTER WHEN DONE"
                                : "CLICK A LINE TO TYPE IT - CLICK A STYLE TO USE IT", C(255, 214, 80), 1, AL_CENTER);
    {
        char s[16];
        sfmt(s, sizeof s, "PAGE %d/%d", tsPage + 1, (TS_COUNT + TS_PAGE - 1) / TS_PAGE);
        text(FW - 14, 224, s, ui.dim, 1, AL_RIGHT);
        text(14, 224, "Q / E", C(110, 110, 130), 1, AL_LEFT);
    }
    coinsCorner();
}

/* a label that fits in w pixels: too long, and it slides right to left (with a pause at each end) so all of it can be read */
static void marquee(int x, int y, int w, const char *s, unsigned c, int alignRight, int scale)
{
    int tw = textWidth(s, scale);
    if (tw <= w) { text(alignRight ? x + w : x, y, s, c, scale, alignRight ? AL_RIGHT : AL_LEFT); return; }
    float v = 22.f * scale, span = (float)(tw - w), slide = span / v, cycle = 1.2f + slide + 1.2f + slide / 3; /* wait, slide, wait, slide back fast */
    float t = fmodf(gt, cycle), off;
    if (t < 1.2f) off = 0;
    else if (t < 1.2f + slide) off = (t - 1.2f) * v;
    else if (t < 2.4f + slide) off = span;
    else off = span * (1 - (t - 2.4f - slide) / (slide / 3));
    clipL = x; clipR = x + w;
    text(x - (int)off, y, s, c, scale, AL_LEFT);
    clipL = 0; clipR = 1 << 30;
}

/* a little equaliser, for the song that's playing */
static void eqBars(int x, int y)
{
    for (int e = 0; e < 3; e++) {
        int hgt = 2 + (int)(6 * (0.5f + 0.5f * sinf(gt * (7 + e * 3) + e)));
        fillRect(x + e * 3, y + 10 - hgt, 2, hgt, C(120, 255, 140));
    }
}

/* SOUNDTRACK screen: one long list that scrolls - + ADD SONG, the choices and the game's songs, then MY SONGS */
void renderSongs(void)
{
    const char *hint = "CLICK A SONG TO PLAY IT ON EVERY MAP - OR LET EACH MAP PLAY ITS OWN";
    unsigned green = C(120, 255, 140);
    text(FW / 2, scrY(6), "SOUNDTRACK", C_WHITE, 3, AL_CENTER);
    text(FW / 2 - 150, sndLblGame, "THE GAME'S SONGS", ui.dim, 1, AL_LEFT);
    text(FW / 2 - 150, sndLblMine, "MY SONGS", ui.dim, 1, AL_LEFT);
    if (!sndGameAny) text(FW / 2, sndLblGame + 14, "NO MATCH", ui.dim, 1, AL_CENTER);
    if (!sndMineAny && !myCount) {
        text(FW / 2, sndLblMine + 14, "NO SONGS YET - CLICK + ADD SONG AT THE TOP", ui.txt, 1, AL_CENTER);
        text(FW / 2, sndLblMine + 26, "MP3, FLAC, WAV, M4A OR OGG - THEY STAY ON THIS COMPUTER", ui.dim, 1, AL_CENTER);
        text(FW / 2, sndLblMine + 38, "TIP: MP3, FLAC OR WAV - 2 TO 5 MINUTES WORKS BEST - UP TO 8 SONGS", C(255, 214, 80), 1, AL_CENTER);
    } else if (!sndMineAny) text(FW / 2, sndLblMine + 14, "NO MATCH", ui.dim, 1, AL_CENTER);
    for (int i = 0; i < nBtns; i++) {
        const Btn *b = &btns[i];
        int a = b->action, focus = i == focusBtn;
        if (a == A_SEARCH) { /* the search box: a magnifying glass, then what you typed */
            int on = searchEdit, cx = b->x + 10, cy = b->y + 8;
            frame(b->x, b->y, b->w, b->h, on ? C(20, 16, 34) : focus ? ui.fillHi : ui.fill, on || focus ? ui.hi : ui.edge);
            for (int t = 0; t < 360; t += 20) pset(cx + (int)roundf(cosf(t * PI / 180) * 4), cy + (int)roundf(sinf(t * PI / 180) * 4), on || focus ? ui.hi : ui.txt);
            thickLine(cx + 3, cy + 3, cx + 6, cy + 6, on || focus ? ui.hi : ui.txt, 2);
            if (songSearch[0] || on) {
                char s[SEARCH_MAX + 2];
                sfmt(s, sizeof s, "%s%s", songSearch, on && fmodf(gt, 0.8f) < 0.5f ? "_" : "");
                marquee(b->x + 22, b->y + 4, b->w - 26, s, C_WHITE, 0, 2);
            } else text(b->x + 22, b->y + 7, "SEARCH SONGS...", ui.dim, 1, AL_LEFT);
            if (focus || on) hint = on ? "TYPE TO FIND A SONG - BACKSPACE DELETES - ENTER WHEN DONE" : "CLICK AND TYPE TO FIND A SONG";
            continue;
        }
        if (a == A_SEARCH_CLR) { /* clears the search */
            frame(b->x, b->y, b->w, b->h, focus ? ui.fillHi : ui.fill, focus ? C(255, 80, 90) : ui.edge);
            text(b->x + b->w / 2, b->y + 7, "X", focus ? C(255, 120, 120) : ui.txt, 1, AL_CENTER);
            if (focus) hint = "CLEAR THE SEARCH";
            continue;
        }
        if (b->y + b->h < 0 || b->y > FH) continue; /* scrolled away */
        if (a >= A_TRK0 && a < A_TRK_END) { /* a choice: green when it's the one picked, an equaliser on the song playing */
            int k = a - A_TRK0, sel = k == musicMode, t = k - 2, playing = t >= 0 && t == musTrack && musicOn;
            panel(b->x, b->y, b->w, b->h, focus);
            if (sel) { /* green frame and a tick */
                frame(b->x + 1, b->y + 1, b->w - 2, b->h - 2, C(24, 60, 36), green);
                thickLine(b->x + 6, b->y + 8, b->x + 8, b->y + 11, green, 2); thickLine(b->x + 8, b->y + 11, b->x + 13, b->y + 5, green, 2);
            }
            marquee(b->x + 18, b->y + 4, b->w - 36, b->label, sel ? green : focus ? ui.hi : ui.txt, 0, 2);
            if (playing) eqBars(b->x + b->w - 14, b->y + 3);
            if (focus && k == 0) hint = "EVERY DISTRICT PLAYS ITS OWN SONG (AND YOUR SONGS ON THE MAPS YOU PICK FOR THEM)";
            else if (focus && k == 1) hint = "A DIFFERENT RANDOM SONG EACH TIME THE MAP CHANGES";
            else if (focus) hint = "CLICK TO PLAY THIS SONG ON EVERY MAP";
        } else if (a >= A_SONG0 && a < A_SONG0 + MY_MAX) { /* the map a song of yours plays on */
            int m = myMap[a - A_SONG0];
            frame(b->x, b->y, b->w, b->h, focus ? ui.fillHi : ui.fill, focus ? ui.edgeHi : ui.edge);
            marquee(b->x + 4, b->y + 7, b->w - 8, m < 0 ? "NO MAP" : DISTRICTS[m].name, m < 0 ? ui.dim : DISTRICTS[m].neonA, 0, 1);
            if (focus) hint = "CLICK: PLAY IT ON THE NEXT MAP (WITH EACH MAP ITS OWN SONG) - OR ON NONE";
        } else if (a >= A_SONGDEL0 && a < A_SONGDEL_END) {
            int armed = songDel == a - A_SONGDEL0;
            frame(b->x, b->y, b->w, b->h, armed ? C(120, 20, 30) : focus ? ui.fillHi : ui.fill, armed || focus ? C(255, 80, 90) : ui.edge);
            text(b->x + b->w / 2, b->y + 7, "X", armed || focus ? C(255, 120, 120) : C(200, 90, 100), 1, AL_CENTER);
            if (focus) hint = armed ? "CLICK AGAIN TO REMOVE THIS SONG" : "REMOVE THIS SONG (CLICK TWICE)";
        } else {
            drawButton(b, focus);
            if (focus && a == A_SONG_ADD) hint = "TIP: MP3, FLAC OR WAV - 2 TO 5 MINUTES WORKS BEST - UP TO 8 SONGS";
        }
    }
    /* the hint sits on a bar that stays put while the list scrolls under it */
    for (int y = FH - 14; y < FH; y++)
        for (int x = 0; x < FW; x++) fb[y * FW + x] = scalec(fb[y * FW + x], 60);
    fillRect(0, FH - 14, FW, 1, ui.edge);
    text(FW / 2, FH - 10, hint, C(255, 214, 80), 1, AL_CENTER);
}
