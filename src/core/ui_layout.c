/* ui_layout.c - Menu buttons: where each screen puts them, and what the mouse is over. */
#include "core.h"

const char *const BG_FIXED_NAME[BG_FIXED] = { "LAST PLAYED", "RANDOM", "THE PICTURE" };
int bgCard(void) { return menuBg >= 0 ? menuBg + BG_FIXED : menuBg == BG_PICTURE ? 2 : -menuBg - 1; } /* LAST PLAYED -1 -> 0, RANDOM -2 -> 1 */
int bgPage;
int uiPage;
int mapPage;
int questPage; /* quests screen: the dailies are pinned on top, the full list pages below */
int skinPage;
int skinPages(void) { return skinTab == 1 ? (ESKIN_COUNT + ESKIN_PAGE - 1) / ESKIN_PAGE : ((skinTab == 2 ? CURSOR_COUNT : skinTab == 3 ? PET_COUNT : skinsShown()) + SKIN_PAGE - 1) / SKIN_PAGE; }
Btn btns[MAXBTN];
int nBtns;

/* the main menu and OPTIONS scroll (mouse wheel, or follow the keyboard focus) when they don't fit on the screen */
int uiScroll, uiScrollMax, uiScrollState = -1, uiFollow;
int wipeStep = 1;
float wipeT;
int scrY(int y) { return y - uiScroll; }
#define OPT_STEP 23  /* option rows: 18 high with a 5 pixel gap */
#define OPT_ROWS 15
int menuBtnTop(void) { int t = titleBottom(10) + 6; return t < 66 ? 66 : t; }
int menuStep(void) { return clampi((FH - 6 - 19 - menuBtnTop()) / 6, 22, 25); }
#define SND_TOP 68  /* SOUNDTRACK list: the first row, and the step between rows */
#define SND_STEP 22
static int songMatch(const char *name) /* the search, ignoring upper / lower case */
{
    int n = (int)strlen(songSearch);
    if (!n) return 1;
    for (const char *p = name; *p; p++) {
        int i = 0;
        while (i < n && p[i] && (p[i] >= 'a' && p[i] <= 'z' ? p[i] - 32 : p[i]) == songSearch[i]) i++;
        if (i == n) return 1;
    }
    return 0;
}

static void addBtn(int x, int y, int w, int h, int action, const char *label)
{
    if (nBtns < MAXBTN) { Btn *b = &btns[nBtns++]; b->x = x; b->y = y; b->w = w; b->h = h; b->action = action; b->label = label; }
}

/* the clickable things on the current screen; used both to draw and to hit-test */
void layoutUI(void)
{
    static const struct { int a; const char *label; } mainB[7] = {
        { A_CAMPAIGN, "CAMPAIGN" }, { A_ENDLESS, "ENDLESS" }, { A_SKINS, "SKINS" }, { A_QUESTS, "QUESTS" },
        { A_OPTIONS, "OPTIONS" }, { A_KEYS, "KEYS" }, { A_QUIT, "QUIT GAME" } };
    static const struct { int a; const char *label; } optB[OPT_ROWS] = {
        { A_OPT_MUSIC, "MUSIC" }, { A_OPT_MVOL, "MUSIC VOLUME" }, { A_OPT_SFX, "SOUND EFFECTS" }, { A_OPT_SVOL, "EFFECTS VOLUME" }, { A_OPT_TRACK, "SOUNDTRACK" },
        { A_OPT_UPG, "UPGRADES" }, { A_OPT_SHAKE, "SCREEN SHAKE" }, { A_OPT_DMGNUM, "DAMAGE NUMBERS" }, { A_OPT_HARD, "HARD MODE" }, { A_OPT_MENUBG, "MENU BACKGROUND" }, { A_OPT_TITLE, "MENU TITLE" },
        { A_OPT_UI, "UI STYLE" }, { A_OPT_FULL, "FULLSCREEN" }, { A_OPT_RESET, "RESET SETTINGS" },
        { A_OPT_WIPE, "RESET EVERYTHING" } };
    nBtns = 0;
    if (state != uiScrollState) { uiScroll = 0; uiScrollState = state; }
    uiScrollMax = state == ST_MENU ? menuBtnTop() + 6 * menuStep() + 19 + 6 - FH
                : state == ST_OPTIONS ? 40 + OPT_ROWS * OPT_STEP + 4 + 17 + 4 - FH
                : 0; /* SOUNDTRACK works its own out below (the search changes how long it is) */
    if (uiScrollMax < 0) uiScrollMax = 0;
    if (state != ST_SONGS) uiScroll = clampi(uiScroll, 0, uiScrollMax); /* SOUNDTRACK clamps once it knows its length */
    switch (state) {
    case ST_MENU: {
        int top = menuBtnTop();
        for (int i = 0; i < 7; i++) addBtn(FW / 2 - 75, scrY(top + i * menuStep()), 150, 19, mainB[i].a, mainB[i].label);
        if (pet) addBtn(412, scrY(184), 40, 13, A_PET_EDIT, "EDIT"); /* under the pet, beside your skin */
        addBtn(14, scrY(184), 84, 13, A_ACH, "ACHIEVEMENTS"); /* under the enemy on the left */
        break; }
    case ST_OPTIONS: /* add new settings here: one row each */
        for (int i = 0; i < OPT_ROWS; i++) addBtn(FW / 2 - 140, scrY(40 + i * OPT_STEP), 280, 18, optB[i].a, optB[i].label);
        addBtn(FW / 2 - 50, scrY(40 + OPT_ROWS * OPT_STEP + 4), 100, 17, A_BACK, "BACK");
        break;
    case ST_SONGS: /* SOUNDTRACK: one long scrolling list - SEARCH and + ADD SONG, the choices and the game's songs, then MY SONGS
                      (map tag, X); a search hides the songs that don't match. First pass: how long it is; second: the buttons. */
        for (int pass = 0; pass < 2; pass++) {
            int y = SND_TOP, any = 0;
            if (pass) {
                addBtn(FW / 2 - 150, scrY(30), songSearch[0] ? 174 : 196, 18, A_SEARCH, "SEARCH");
                if (songSearch[0]) addBtn(FW / 2 + 28, scrY(30), 18, 18, A_SEARCH_CLR, "X");
                addBtn(FW / 2 + 52, scrY(30), 98, 18, A_SONG_ADD, "+ ADD SONG");
                sndLblGame = scrY(y - 10);
            }
            for (int k = 0; k < 2 + MUS_TRACKS; k++) {
                const char *nm = k == 0 ? "EACH MAP ITS OWN SONG" : k == 1 ? "RANDOM" : TRACKS[k - 2].name;
                if (k < 2 ? songSearch[0] != 0 : !songMatch(nm)) continue; /* while searching, only songs */
                if (pass) addBtn(FW / 2 - 150, scrY(y), 300, 18, A_TRK0 + k, nm);
                y += SND_STEP; any++;
            }
            if (!any) y += 16; /* room for "no match" */
            if (pass) sndGameAny = any;
            y += 18;
            if (pass) sndLblMine = scrY(y - 10);
            any = 0;
            for (int i = 0; i < myCount; i++) {
                if (!songMatch(myNames[i])) continue;
                if (pass) {
                    addBtn(FW / 2 - 150, scrY(y), 220, 18, A_TRK0 + 2 + MUS_TRACKS + i, myNames[i]);
                    addBtn(FW / 2 + 74, scrY(y), 56, 18, A_SONG0 + i, "MAP");
                    addBtn(FW / 2 + 134, scrY(y), 16, 18, A_SONGDEL0 + i, "X");
                }
                y += SND_STEP; any++;
            }
            if (!any) y += 34; /* room for "no songs yet" / "no match" */
            if (pass) sndMineAny = any;
            y += 6;
            if (pass) addBtn(FW / 2 - 50, scrY(y), 100, 17, A_BACK, "BACK");
            else { uiScrollMax = y + 17 + 18 - FH; if (uiScrollMax < 0) uiScrollMax = 0; uiScroll = clampi(uiScroll, 0, uiScrollMax); }
        }
        break;
    case ST_TITLE: {
        int top = titleBottom(10) + 8 < 74 ? 74 : titleBottom(10) + 8;
        addBtn(FW / 2 - 150, top, 300, 18, A_TTL_L1, "LINE 1");
        addBtn(FW / 2 - 150, top + 22, 300, 18, A_TTL_L2, "LINE 2");
        for (int n = 0, i = tsPage * TS_PAGE; n < TS_PAGE && i < TS_COUNT; n++, i++) addBtn(12 + n % 5 * 92, top + 48 + n / 5 * 46, 86, 40, A_TS0 + i, TSTY[i].name);
        addBtn(14, 240, 80, 17, A_PAGE_PREV, "< PREV");
        addBtn(FW / 2 - 105, 240, 100, 17, A_TTL_RESET, "RESET");
        addBtn(FW / 2 + 5, 240, 100, 17, A_BACK, "BACK");
        addBtn(FW - 94, 240, 80, 17, A_PAGE_NEXT, "NEXT >");
        break; }
    case ST_UPGRADES: /* each row only shows up when the one above it is ON */
        addBtn(FW / 2 - 140, 60, 280, 18, A_OPT_PERKS, "UPGRADE CARDS");
        if (perksOn) addBtn(FW / 2 - 140, 82, 280, 18, A_OPT_PLIM, "UPGRADE LIMIT");
        if (perksOn && perkLimitOn) addBtn(FW / 2 - 140, 104, 280, 18, A_OPT_PLIMN, "MAX UPGRADES");
        addBtn(FW / 2 - 50, 218, 100, 17, A_BACK, "BACK");
        break;
    case ST_PETEDIT: /* NO COLLAR first, then the colours: picking a colour puts the collar back on */
        addBtn(FW / 2 - 70, 112, 140, 14, A_PET_NAME, "NAME");
        addBtn(FW / 2 - 100, 130, 200, 18, A_PET_COLLAR, "COLLAR");
        addBtn(FW / 2 - ((COLLAR_COUNT + 1) * 22 - 4) / 2, 172, 18, 18, A_PCOL_NONE, "NO COLLAR");
        for (int i = 0; i < COLLAR_COUNT; i++) addBtn(FW / 2 - ((COLLAR_COUNT + 1) * 22 - 4) / 2 + (i + 1) * 22, 172, 18, 18, A_PCOL0 + i, "");
        addBtn(FW / 2 - 50, 218, 100, 17, A_BACK, "BACK");
        break;
    case ST_UISTYLE:
        for (int n = 0, i = uiPage * UI_PAGE; n < UI_PAGE && i < UI_COUNT; n++, i++)
            addBtn(14 + n % 4 * 116, 50 + n / 4 * 52, 104, 42, A_UI0 + uiAt(i), uiStyleOf(uiAt(i)).name);
        addBtn(FW / 2 - 160, 214, 90, 17, A_PAGE_PREV, "< PREV");
        addBtn(FW / 2 - 50, 214, 100, 17, A_BACK, "BACK");
        addBtn(FW / 2 + 70, 214, 90, 17, A_PAGE_NEXT, "NEXT >");
        break;
    case ST_MENUBG:
        for (int n = 0, i = bgPage * MAP_PAGE; n < MAP_PAGE && i < BG_CHOICES; n++, i++)
            addBtn(14 + n * 116, 62, 104, 132, A_BG0 + i, i < BG_FIXED ? BG_FIXED_NAME[i] : DISTRICTS[i - BG_FIXED].name);
        addBtn(FW / 2 - 160, 214, 90, 17, A_PAGE_PREV, "< PREV");
        addBtn(FW / 2 - 50, 214, 100, 17, A_BACK, "BACK");
        addBtn(FW / 2 + 70, 214, 90, 17, A_PAGE_NEXT, "NEXT >");
        break;
    case ST_MAPSEL:
        for (int n = 0, i = mapPage * MAP_PAGE; n < MAP_PAGE && i < D_COUNT; n++, i++)
            addBtn(14 + n * 116, 62, 104, 132, A_MAP0 + i, DISTRICTS[i].name);
        addBtn(FW / 2 - 160, 214, 90, 17, A_PAGE_PREV, "< PREV");
        addBtn(FW / 2 - 50, 214, 100, 17, A_BACK, "BACK");
        addBtn(FW / 2 + 70, 214, 90, 17, A_PAGE_NEXT, "NEXT >");
        break;
    case ST_SKINS:
        addBtn(FW / 2 - 206, 26, 100, 15, A_TAB_PLAYER, "PLAYER SKINS");
        addBtn(FW / 2 - 102, 26, 100, 15, A_TAB_ENEMY, "ENEMY SKINS");
        addBtn(FW / 2 + 2, 26, 100, 15, A_TAB_CURSOR, "CURSORS");
        addBtn(FW / 2 + 106, 26, 100, 15, A_TAB_PETS, "PETS");
        if (skinTab == 0) {
            /* DAVID, when he's the one you wear, gets a SANDEVISTAN switch on his card (added first so it takes the click) */
            if (isDavid(skin) && skin / SKIN_PAGE == skinPage) {
                int n = skin % SKIN_PAGE;
                addBtn(12 + (n % 6) * 77 + 4, 47 + (n / 6) * 92 + 72, 62, 13, A_SANDY, "TRAIL");
            }
            for (int n = 0, i = skinPage * SKIN_PAGE; n < SKIN_PAGE && i < skinsShown(); n++, i++)
                addBtn(12 + (n % 6) * 77, 47 + (n / 6) * 92, 70, 86, A_SKIN0 + skinShownAt(i), SKINS[skinShownAt(i)].name);
        }
        else if (skinTab == 3)
            for (int n = 0, i = skinPage * SKIN_PAGE; n < SKIN_PAGE && i < PET_COUNT; n++, i++)
                addBtn(12 + (n % 6) * 77, 47 + (n / 6) * 92, 70, 86, A_PET0 + i, PETS[i].name);
        else if (skinTab == 2)
            for (int n = 0, i = skinPage * SKIN_PAGE; n < SKIN_PAGE && i < CURSOR_COUNT; n++, i++)
                addBtn(12 + (n % 6) * 77, 47 + (n / 6) * 82, 70, 76, A_CUR0 + i, CURSORS[i]);
        else
            for (int n = 0, i = skinPage * ESKIN_PAGE; n < ESKIN_PAGE && i < ESKIN_COUNT; n++, i++)
                addBtn(12 + n * 92, 52, 86, 152, A_ESKIN0 + i, ESKINS[i].name);
        if (skinPages() > 1) addBtn(FW / 2 - 160, 237, 90, 17, A_PAGE_PREV, "< PREV");
        addBtn(FW / 2 - 50, 237, 100, 17, A_BACK, "BACK");
        if (skinPages() > 1) addBtn(FW / 2 + 70, 237, 90, 17, A_PAGE_NEXT, "NEXT >");
        break;
    case ST_QUESTS:
        addBtn(FW / 2 - 160, 237, 90, 17, A_PAGE_PREV, "< PREV");
        addBtn(FW / 2 - 50, 237, 100, 17, A_BACK, "BACK");
        addBtn(FW / 2 + 70, 237, 90, 17, A_PAGE_NEXT, "NEXT >");
        break;
    case ST_WIPE: /* CANCEL comes first, so ENTER right away keeps everything */
        addBtn(FW / 2 - 220, 218, 210, 22, A_BACK, wipeStep == 1 ? "NO, KEEP IT ALL" : "NO, CANCEL");
        addBtn(FW / 2 + 10, 218, 210, 22, wipeStep == 1 ? A_WIPE_NEXT : A_WIPE_DO, wipeStep == 1 ? "I UNDERSTAND" : "DELETE EVERYTHING");
        break;
    case ST_ACH:
        addBtn(FW / 2 - 160, 237, 90, 17, A_PAGE_PREV, "< PREV");
        addBtn(FW / 2 - 50, 237, 100, 17, A_BACK, "BACK");
        addBtn(FW / 2 + 70, 237, 90, 17, A_PAGE_NEXT, "NEXT >");
        break;
    case ST_KEYS:
        addBtn(FW / 2 - 50, 236, 100, 17, A_BACK, "BACK");
        break;
    case ST_EXTRA:
        addBtn(FW / 2 - 50, 236, 100, 17, A_BACK, "BACK");
        break;
    case ST_QUIT:
        addBtn(FW / 2 - 70, 160, 140, 19, A_TOMENU, "BACK TO MENU");
        break;
    case ST_PAUSE:
        addBtn(FW / 2 - 70, 108, 140, 19, A_RESUME, "RESUME");
        addBtn(FW / 2 - 70, 130, 140, 19, A_OPTIONS, "OPTIONS");
        addBtn(FW / 2 - 70, 152, 140, 19, A_TOMENU, "MAIN MENU");
        break;
    case ST_OVER:
        if (overT > 0.8f) {
            addBtn(FW / 2 - 70, 196, 140, 19, A_RETRY, "PLAY AGAIN");
            addBtn(FW / 2 - 70, 220, 140, 19, A_TOMENU, "MAIN MENU");
        }
        break;
    case ST_PERK:
        for (int c = 0; c < 3; c++)
            if (perkChoice[c] >= 0) addBtn(31 + c * 144, 78, 130, 118, A_PERK0 + c, PERKS[perkChoice[c]].name);
        break;
    }
    if (focusBtn >= nBtns) focusBtn = nBtns - 1;
    if (focusBtn < 0) focusBtn = 0;
    if (uiFollow && uiScrollMax > 0 && nBtns) { /* the keyboard moved the focus: scroll it into view */
        const Btn *b = &btns[focusBtn];
        int bottom = state == ST_SONGS ? FH - 18 : FH - 4; /* SOUNDTRACK keeps a hint bar along the bottom */
        int d = b->y < 4 ? b->y - 4 : b->y + b->h > bottom ? b->y + b->h - bottom : 0;
        uiFollow = 0;
        if (d) { uiScroll = clampi(uiScroll + d, 0, uiScrollMax); layoutUI(); }
    }
    uiFollow = 0;
}

int btnAt(int x, int y)
{
    for (int i = 0; i < nBtns; i++)
        if (x >= btns[i].x && x < btns[i].x + btns[i].w && y >= btns[i].y && y < btns[i].y + btns[i].h) return i;
    return -1;
}

int menuMap(void)
{
    if (menuBg >= 0 && ownsMap(menuBg)) return menuBg;
    if (menuBg == BG_RANDOM) { /* a random map you own */
        int n = 0, pick[D_COUNT];
        for (int i = 0; i < D_COUNT; i++) if (ownsMap(i)) pick[n++] = i;
        return pick[irand(n)];
    }
    return startMap;
}

void toMenu(void)
{
    if (state == ST_PAUSE) setBest(P_BESTSCORE, score);
    saveGame();
    state = ST_MENU;
    focusBtn = 0;
    loadDistrict(menuMap()); menuMusMap = curMap;
}

const char *menuBgName(void) { return menuBg >= 0 ? DISTRICTS[menuBg].name : BG_FIXED_NAME[bgCard()]; }
