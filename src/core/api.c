/* api.c - The entry points the platform calls: start-up, ticks and input. */
#include "core.h"

void game_init(unsigned seed, void *workspace)
{
    char *p = (char *)workspace;
    fb = (unsigned *)p; p += FW * FH * 4;
    farL = (unsigned *)p; p += FARW * FH * 4;
    midL = (unsigned *)p; p += MIDW * FH * 4;
    foreL = (unsigned *)p; p += FW * FH * 4;
    for (int k = 0; k < PIC_N; k++) { pics[k] = (unsigned *)p; p += FW * FH * 4; }
    pic = pics[PIC_ORBIT];
    picIdx = (unsigned char (*)[PBW * PBH])p; p += PIC_N * PBW * PBH;
    smp = (short (*)[3])p; p += PBW * PBH * 6;
    sndPool = (short *)p; p += SND_POOL * 2;
    musPool = (short *)p;
    rng = seed ? seed : 1;
    genSounds();

    extraInit();
    loadGame();
    refreshDaily();
    achCheckOwned(); /* things bought before achievements existed count too */
    startMap = 0;
    loadDistrict(menuMap()); menuMusMap = curMap;
    target(fb, FW, FH);
    state = ST_MENU;
}

void game_tick(void)
{
    update(DT);
    /* the autopilot restarts on its own so long test runs keep going */
    if (bot && state == ST_OVER && overT > 2) newGame();
}

void game_mouse_move(int x, int y)
{
    mx = x; my = y;
    padAim = 0; /* the mouse aims again */
    if (state != ST_PLAY) {
        if (sliderDrag) { setSlider(sliderDrag, sliderAtMouse(sliderDrag)); return; }
        layoutUI();
        int b = btnAt(x, y);
        if (b >= 0) focusBtn = b;
    }
}

void game_mouse_button(int button, int down)
{
    if (state != ST_PLAY) {
        mdown = 0;
        if (button == 0 && !down && sliderDrag) { sliderDrag = 0; saveGame(); }
        if (state == ST_EXTRA && down && button == 0 && extraTvAt(mx, my)) {
            extraTv = (extraTv + 1) % (PIC_N - PIC_X + 1); extraTvT = gt;
            sfx(S_SWITCH, 0.5f);
            return;
        }
        if (state == ST_EXTRA && down && button == 0 && extraLinkAt(mx, my)) {
            extraOpenLink();
            sfx(S_PICKUP, 0.4f);
            return;
        }
        if (button == 0 && down) {
            layoutUI();
            int b = btnAt(mx, my);
            if (b >= 0 && isSlider(btns[b].action)) {
                focusBtn = b; /* grab the slider: it follows the mouse until the button is released */
                sliderDrag = btns[b].action;
                setSlider(sliderDrag, sliderAtMouse(sliderDrag));
            } else if (b >= 0) { focusBtn = b; activate(btns[b].action); }
        }
        return;
    }
    if (button == 0) mdown = down;
    else if (down) dashReq = 1;
}

void game_wheel(int dir)
{
    if (state == ST_PLAY) cycleWeapon(dir > 0 ? 1 : -1);
    else if (state == ST_SKINS || state == ST_QUESTS || state == ST_ACH || state == ST_MAPSEL || state == ST_MENUBG || state == ST_UISTYLE || (state == ST_TITLE && !titleEdit)) { flipSkinPage(dir > 0 ? 1 : -1); sfx(S_SWITCH, 0.4f); }
    else if (state == ST_MENU || state == ST_OPTIONS || state == ST_SONGS) { /* scroll when the screen doesn't fit */
        layoutUI();
        uiScroll = clampi(uiScroll + (dir > 0 ? 14 : -14), 0, uiScrollMax);
        layoutUI();
        int b = btnAt(mx, my);
        if (b >= 0) focusBtn = b;
    }
}

static void toggleMusic(void)
{
    musicOn = !musicOn;
    if (state != ST_OPTIONS) toast(musicOn ? "MUSIC ON" : "MUSIC OFF");
    saveGame();
}

static void uiKey(int key)
{
    layoutUI();
    if (key == K_MUSIC) { toggleMusic(); return; }
    if (state == ST_PERK) {
        if (key >= K_WEAPON1 && key <= K_WEAPON3) takePerk(key - K_WEAPON1);
        else if (key == K_ENTER || key == K_JUMP) { if (nBtns) takePerk(btns[focusBtn].action - A_PERK0); }
        else if (key == K_LEFT || key == K_UP) { if (nBtns) focusBtn = (focusBtn + nBtns - 1) % nBtns; }
        else if (key == K_RIGHT || key == K_DOWN) { if (nBtns) focusBtn = (focusBtn + 1) % nBtns; }
        return;
    }
    if ((state == ST_OPTIONS || state == ST_UPGRADES) && nBtns && (key == K_LEFT || key == K_RIGHT)) { /* LEFT / RIGHT change the focused setting */
        int a = btns[focusBtn].action, d = key == K_RIGHT ? 10 : -10;
        if (a == A_OPT_PLIMN) { setSlider(a, perkLimit + d / 10); saveGame(); }
        else if (a == A_OPT_MVOL) { setSlider(a, musicVolPct + d); saveGame(); }
        else if (a == A_OPT_SVOL) { setSlider(a, sfxVolPct + d); saveGame(); }
        else if (a == A_OPT_TRACK) setMusicMode(musicMode + (key == K_RIGHT ? 1 : -1));
        else if (a == A_OPT_UI) pickUi(uiAt((uiSlot(uiStyle) + (key == K_RIGHT ? 1 : UI_COUNT - 1)) % UI_COUNT));
        else if (a != A_BACK && a != A_OPT_MENUBG && a != A_OPT_UPG && a != A_OPT_TITLE && a != A_OPT_RESET) activate(a); /* the ones with their own screen open with ENTER or a click */
        return;
    }
    if ((state == ST_OPTIONS || state == ST_MENUBG || state == ST_UISTYLE || state == ST_UPGRADES || state == ST_TITLE || state == ST_SONGS || state == ST_WIPE || state == ST_EXTRA) && key == K_ESC) { activate(A_BACK); return; }
    if ((state == ST_SKINS || state == ST_QUESTS || state == ST_ACH || state == ST_MAPSEL || state == ST_MENUBG || state == ST_UISTYLE || state == ST_TITLE) && (key == K_PREV_WEAPON || key == K_NEXT_WEAPON)) { activate(key == K_NEXT_WEAPON ? A_PAGE_NEXT : A_PAGE_PREV); return; }
    switch (key) {
    case K_UP: case K_LEFT: if (nBtns) focusBtn = (focusBtn + nBtns - 1) % nBtns; uiFollow = 1; break;
    case K_DOWN: case K_RIGHT: if (nBtns) focusBtn = (focusBtn + 1) % nBtns; uiFollow = 1; break;
    case K_ENTER: case K_JUMP: if (nBtns) activate(btns[focusBtn].action); break;
    case K_ESC: if (state == ST_PAUSE) state = ST_PLAY; else if (state != ST_MENU) toMenu(); break;
    case K_PAUSE: if (state == ST_PAUSE) state = ST_PLAY; break;
    case K_MENU: if (state == ST_PAUSE) toMenu(); break;
    case K_RETRY: if (state == ST_OVER && overT > 0.6f) newGame(); break;
    }
}

/* typing a MENU TITLE line, the SOUNDTRACK search or a pet's name: letters arrive here, the keys below are ignored except ENTER and ESC, which finish */
static int titleCharOk(int ch) { return ch == ' ' || (ch != '_' && ch != '~' && ch != '#' && glyph((char)ch)); }
void game_char(int ch)
{
    char *t = titleEdit == 2 ? title2 : title1;
    int n = (int)strlen(t), max = titleEdit == 2 ? TITLE2_MAX : TITLE1_MAX;
    if (ch >= 'a' && ch <= 'z') ch -= 32;
    if (!game_typing()) { extraChar(ch); return; } /* the platforms send every letter */
    if (state == ST_SONGS && searchEdit) { /* the search: the list follows as you type */
        n = (int)strlen(songSearch);
        if (ch == 8) { if (n) songSearch[n - 1] = 0; }
        else if (ch >= 32 && ch <= 126 && n < SEARCH_MAX) { songSearch[n] = (char)ch; songSearch[n + 1] = 0; }
        uiScroll = 0;
        return;
    }
    if (state == ST_PETEDIT && petNameEdit) { /* your pet's name: the same letters as a MENU TITLE */
        char *p = petNames[pet];
        n = (int)strlen(p);
        if (ch == 8) { if (n) p[n - 1] = 0; petNameFresh = 0; return; }
        if (ch < 32 || ch > 126 || !titleCharOk(ch)) return;
        if (petNameFresh) { petNameFresh = 0; p[0] = 0; n = 0; } /* starting to type replaces the old name */
        if (n < PETNAME_MAX) { p[n] = (char)ch; p[n + 1] = 0; }
        return;
    }
    if (state != ST_TITLE || !titleEdit) return;
    if (ch == 8) { if (n) t[n - 1] = 0; titleFresh = 0; return; }
    if (ch < 32 || ch > 126 || !titleCharOk(ch)) return;
    if (titleFresh) { titleFresh = 0; t[0] = 0; n = 0; } /* starting to type replaces the old line */
    if (n >= max) return;
    t[n] = (char)ch; t[n + 1] = 0;
}
int game_typing(void) { return (state == ST_TITLE && titleEdit) || (state == ST_SONGS && searchEdit) || (state == ST_PETEDIT && petNameEdit); }

void game_key(int key, int down)
{
    int ui = state != ST_PLAY;
    if (game_typing()) {
        if (down && (key == K_ENTER || key == K_ESC)) { titleEdit = 0; searchEdit = 0; petNameEdit = 0; saveGame(); sfx(S_PICKUP, 0.4f); }
        return;
    }
    if (key == K_X) { if (down) setDevMode(!devMode); return; }
    if (down && extraKey(key)) return;
    if (key <= K_DOWN) {
        if (down && !held[key]) {
            if (ui) uiKey(key);
            else if (key == K_UP) upPress = 1;
            else if (key == K_DOWN) downPress = 1;
        }
        held[key] = down;
        return;
    }
    if (key == K_JUMP) {
        if (down && !jumpHeld) { if (ui) uiKey(key); else jumpPress = 1; }
        jumpHeld = down;
        return;
    }
    if (!down) return;
    if (ui) { uiKey(key); return; }
    switch (key) {
    case K_DASH: dashReq = 1; break;
    case K_PAUSE:
    case K_ESC: state = ST_PAUSE; focusBtn = 0; mdown = 0; break;
    case K_RETRY: startReload(); break; /* R reloads */
    case K_MUSIC: toggleMusic(); break;
    case K_PREV_WEAPON: cycleWeapon(-1); break;
    case K_NEXT_WEAPON: cycleWeapon(1); break;
    default:
        if (key >= K_WEAPON1 && key <= K_WEAPON7) selectWeapon(key - K_WEAPON1);
        break;
    }
}

void game_focus_lost(void)
{
    mdown = 0;
    held[0] = held[1] = held[2] = held[3] = 0;
    jumpHeld = 0;
    if (state == ST_PLAY) { state = ST_PAUSE; focusBtn = 0; }
}

int game_wants_quit(void)
{
    int q = wantsQuit;
    wantsQuit = 0;
    return q;
}

int game_wants_fullscreen(void)
{
    int q = fullReq;
    fullReq = 0;
    return q;
}

void game_set_fullscreen(int on) { fullOn = on; }

/* test hook: drop big boss skin k into the running game */
void game_test_mega(int k)
{
    if (state == ST_PLAY) spawnMega(clampi(k, 0, MEGA_COUNT - 1), 1);
}

void game_set_bot(int b, int g)
{
    bot = b;
    god = g;
    if (bot && state != ST_PLAY) { mode = MODE_CAMPAIGN; newGame(); }
}

int game_stat(int which)
{
    int n = 0;
    switch (which) {
    case STAT_STATE: return state;
    case STAT_WAVE: return waveIdx;
    case STAT_SCORE: return score;
    case STAT_HP: return hp;
    case STAT_ENEMIES: return countAlive();
    case STAT_KILLS: return totalKills;
    case STAT_BOSSES: return bossKills;
    case STAT_DEATHS: return deaths;
    case STAT_PX: return (int)px;
    case STAT_PY: return (int)py;
    case STAT_SHOTS: for (int i = 0; i < MAXS; i++) n += shots[i].life > 0; return n;
    case STAT_ESHOTS: for (int i = 0; i < MAXES; i++) n += eshots[i].life > 0; return n;
    case STAT_AREA: return area;
    case STAT_WEAPON: return curW;
    case STAT_MAP: return curMap;
    case STAT_ON_ROPE: return ropeIdx;
    case STAT_ON_GROUND: return onGround;
    case STAT_MAG: return mag[curW];
    case STAT_RELOADING: return reloadT > 0;
    case STAT_COINS: return coins;
    case STAT_MODE: return mode;
    case STAT_FOCUS: return focusBtn;
    case STAT_MAPS: return mapsMask;
    case STAT_SKINS_OWNED: return skinsMask;
    case STAT_SKIN: return skin;
    case STAT_QUESTS_DONE: return questMask;
    case STAT_PERKS: return perksTaken;
    case STAT_MAX_HP: return maxHp;
    case STAT_MUSIC: return musicOn;
    case STAT_DAILY_DONE: return dqDone;
    case STAT_DAILY_POOL: return dailyPool();
    }
    return 0;
}
