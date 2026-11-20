/* ui_actions.c - What every menu button does: buying, equipping, settings and the soundtrack. */
#include "core.h"

/* UI STYLE: every preset is free - picking one switches the whole interface over at once */
void pickUi(int i)
{
    char s[64];
    uiStyle = clampi(i, 0, UI_COUNT - 1);
    sfmt(s, sizeof s, "UI STYLE: %s", uiStyleOf(uiStyle).name);
    toast(s);
    sfx(S_PICKUP, 0.5f);
    saveGame();
}

/* MENU BACKGROUND screen: card i is LAST PLAYED (0), RANDOM (1), the picture (2) or map i - 3; locked maps can be bought right here */
static void pickMenuBg(int i)
{
    char s[64];
    int m = i - BG_FIXED, buy = m >= 0 ? m : i == 2 ? D_ORBIT : -1; /* the picture comes with the DAVID AND LUCY map */
    if (i == 2 && !picOk) { toast("NO PICTURE IN THIS VERSION"); sfx(S_HURT, 0.3f); return; }
    if (buy >= 0 && !ownsMap(buy)) {
        if (coins < MAP_COST[buy]) {
            sfmt(s, sizeof s, "NEED %d COINS (YOU HAVE %d)", MAP_COST[buy], coins);
            toast(s);
            sfx(S_HURT, 0.3f);
            return;
        }
        coins -= MAP_COST[buy];
        mapsMask |= 1 << buy;
        achCheckOwned();
        sfmt(s, sizeof s, "UNLOCKED %s!", DISTRICTS[buy].name);
        toast(s);
        sfx(S_PICKUP, 0.7f);
    } else {
        sfmt(s, sizeof s, "MENU BACKGROUND: %s", m >= 0 ? DISTRICTS[m].name : BG_FIXED_NAME[i]);
        toast(s);
        sfx(S_PICKUP, 0.5f);
    }
    menuBg = m >= 0 ? m : i == 0 ? BG_LAST : i == 1 ? BG_RANDOM : BG_PICTURE;
    loadDistrict(menuMap()); menuMusMap = curMap;
    saveGame();
}

/* RESET SETTINGS: every setting back to how a new game starts - but coins, everything bought or unlocked
   (skins, maps, pets, the MENU TITLE purchase), what you wear and MY SONGS all stay */
float resetArmT = -10; /* when RESET SETTINGS was clicked once; a second click within 3 s does it */
static void defaultSettings(void)
{
    musicOn = perksOn = shakeOn = sfxOn = 1;
    musicVolPct = sfxVolPct = 100;
    perkLimitOn = 1; perkLimit = PERK_DEFAULT;
    uiStyle = 0; cursorSkin = 0;
    musicMode = 0; musFor = -1;
    sandyOff = 0;
    dmgNumsOn = 1; hardOn = 0;
    titleStyle = 0; sfmt(title1, sizeof title1, "NIGHT CITY"); sfmt(title2, sizeof title2, "SIEGE"); /* the title stays bought */
    menuBg = BG_LAST;
    if (fullOn) fullReq = 1; /* back to a window */
    if (devMode) setDevMode(0);
    else if (!optFromPause) { loadDistrict(menuMap()); menuMusMap = curMap; }
}
static void resetSettings(void)
{
    defaultSettings();
    saveGame();
    toast("SETTINGS RESET - UNLOCKS AND SONGS KEPT");
    sfx(S_PICKUP, 0.6f);
}

/* RESET EVERYTHING: like a brand new game - no coins, nothing unlocked, no quests, achievements or records,
   every setting back to default. Only MY SONGS stay (they're the player's own files, not progress). */
static void wipeEverything(void)
{
    coins = 0; mapsMask = 1; skinsMask = 1; skin = 0; questMask = 0;
    memset(pstat, 0, sizeof pstat);
    eskin = 0; eskinsMask = 1;
    pet = 0; petsMask = 1; petsMask2 = 0; collarOn = 1; collarCol = 0;
    memset(petNames, 0, sizeof petNames);
    dqDay = -1; dqDone = 0; dqCacheOk = 0;
    for (int i = 0; i < 3; i++) dq[i] = dqProg[i] = 0;
    achMask = 0;
    hardUnlocked = 0;
    titleOwned = 0;
    startMap = 0;
    totalKills = bossKills = deaths = 0;
    defaultSettings();
    refreshDaily();
    toMenu(); /* saves */
    loadDistrict(menuMap()); menuMusMap = curMap;
    toast("EVERYTHING RESET - A FRESH START");
    sfx(S_BOOM, 0.6f);
}

void setDevMode(int on)
{
    devMode = on;
    if (!ownsSkin(skin)) skin = 0;   /* turning it off puts back what you really own */
    if (!ownsESkin(eskin)) eskin = 0;
    if (!ownsPet(pet)) pet = 0;
    if (menuBg == BG_PICTURE && !ownsMap(D_ORBIT)) menuBg = BG_LAST;
    if (menuBg >= 0 && !ownsMap(menuBg)) menuBg = BG_LAST;
    int inRun = optFromPause || state == ST_PLAY || state == ST_PAUSE || state == ST_PERK || state == ST_OVER; /* the dev shortcut works mid-run too */
    if (!inRun) { loadDistrict(menuMap()); menuMusMap = curMap; }
    toast(on ? "DEVELOPER MODE ON - EVERYTHING UNLOCKED" : "DEVELOPER MODE OFF");
    saveGame();
}

static void tryMap(int i)
{
    char s[64];
    if (ownsMap(i)) { startMap = i; newGame(); return; }
    if (coins >= MAP_COST[i]) {
        coins -= MAP_COST[i];
        mapsMask |= 1 << i;
        sfmt(s, sizeof s, "UNLOCKED %s!", DISTRICTS[i].name);
        toast(s);
        sfx(S_PICKUP, 0.7f);
        saveGame();
    } else {
        sfmt(s, sizeof s, "NEED %d COINS - OR REACH IT IN CAMPAIGN", MAP_COST[i]);
        toast(s);
        sfx(S_HURT, 0.3f);
    }
}

static void trySkin(int i)
{
    char s[64];
    if (ownsSkin(i)) { skin = i; saveGame(); sfx(S_PICKUP, 0.5f); return; }
    if (SKINS[i].cost > 0 && coins >= SKINS[i].cost) {
        coins -= SKINS[i].cost;
        skinsMask |= 1 << i;
        achCheckOwned();
        skin = i;
        sfmt(s, sizeof s, "UNLOCKED %s!", SKINS[i].name);
        toast(s);
        sfx(S_PICKUP, 0.7f);
        saveGame();
    } else if (SKINS[i].cost > 0) {
        sfmt(s, sizeof s, "NEED %d COINS (YOU HAVE %d)", SKINS[i].cost, coins);
        toast(s);
        sfx(S_HURT, 0.3f);
    } else {
        sfmt(s, sizeof s, "QUEST: %s", QUESTS[SKINS[i].quest].title);
        toast(s);
        sfx(S_HURT, 0.3f);
    }
}

void flipSkinPage(int dir) /* also flips the quest list pages */
{
    if (state == ST_ACH) {
        int n = (ACH_COUNT + ACH_PAGE - 1) / ACH_PAGE;
        achPage = (achPage + dir + n) % n;
        return;
    }
    if (state == ST_QUESTS) {
        int n = (QUEST_COUNT + QUEST_PAGE - 1) / QUEST_PAGE;
        questPage = (questPage + dir + n) % n;
        return;
    }
    if (state == ST_MAPSEL) {
        int n = (D_COUNT + MAP_PAGE - 1) / MAP_PAGE;
        mapPage = (mapPage + dir + n) % n;
        return;
    }
    if (state == ST_MENUBG) {
        int n = (BG_CHOICES + MAP_PAGE - 1) / MAP_PAGE;
        bgPage = (bgPage + dir + n) % n;
        return;
    }
    if (state == ST_TITLE) {
        int n = (TS_COUNT + TS_PAGE - 1) / TS_PAGE;
        tsPage = (tsPage + dir + n) % n;
        return;
    }
    if (state == ST_UISTYLE) {
        int n = (UI_COUNT + UI_PAGE - 1) / UI_PAGE;
        uiPage = (uiPage + dir + n) % n;
        return;
    }
    skinPage = (skinPage + dir + skinPages()) % skinPages();
}

static void tryESkin(int i)
{
    char s[64];
    if (ownsESkin(i)) { eskin = i; saveGame(); sfx(S_PICKUP, 0.5f); return; }
    if (coins >= ESKINS[i].cost) {
        coins -= ESKINS[i].cost;
        eskinsMask |= 1 << i;
        eskin = i;
        sfmt(s, sizeof s, "UNLOCKED %s!", ESKINS[i].name);
        toast(s);
        sfx(S_PICKUP, 0.7f);
        saveGame();
    } else {
        sfmt(s, sizeof s, "NEED %d COINS (YOU HAVE %d)", ESKINS[i].cost, coins);
        toast(s);
        sfx(S_HURT, 0.3f);
    }
}

static void tryPet(int i)
{
    char s[64];
    if (ownsPet(i)) { pet = i; saveGame(); sfx(S_PICKUP, 0.5f); return; }
    if (coins >= PETS[i].cost) {
        coins -= PETS[i].cost;
        givePet(i);
        achCheckOwned();
        pet = i;
        sfmt(s, sizeof s, "ADOPTED %s!", PETS[i].name);
        toast(s);
        sfx(S_PICKUP, 0.7f);
        saveGame();
    } else {
        sfmt(s, sizeof s, "NEED %d COINS (YOU HAVE %d)", PETS[i].cost, coins);
        toast(s);
        sfx(S_HURT, 0.3f);
    }
}
void setSlider(int a, int pct)
{
    if (a == A_OPT_PLIMN) { perkLimit = clampi(pct, 1, PERK_TOTAL); return; } /* MAX UPGRADES counts cards, not percent */
    int *v = a == A_OPT_MVOL ? &musicVolPct : &sfxVolPct;
    pct = clampi(pct, 0, 100);
    if (pct == *v) return;
    *v = pct;
    if (a == A_OPT_SVOL) sfx(S_PICKUP, 0.5f); /* let you hear the new level */
}

int isSlider(int a) { return a == A_OPT_MVOL || a == A_OPT_SVOL || a == A_OPT_PLIMN; }

/* the slider value under the mouse: volumes snap to 5%, MAX UPGRADES to whole cards */
int sliderAtMouse(int a)
{
    if (a == A_OPT_PLIMN) return 1 + ((mx - SLIDER_X0) * (PERK_TOTAL - 1) + SLIDER_W / 2) / SLIDER_W;
    return ((mx - SLIDER_X0) * 100 / SLIDER_W + 2) / 5 * 5;
}

/* SOUNDTRACK: BY MAP, RANDOM, then every song by name, round and round */
const char *musicModeName(void) { return musicMode == 0 ? "BY MAP" : musicMode == 1 ? "RANDOM" : trackName(clampi(musicMode - 2, 0, MUS_TRACKS + myCount - 1)); }
void setMusicMode(int m)
{
    char s[64];
    musicMode = (m + MUS_TRACKS + myCount + 2) % (MUS_TRACKS + myCount + 2);
    musFor = -1; /* pick again right away */
    sfmt(s, sizeof s, "SOUNDTRACK: %s", musicModeName());
    toast(s);
    saveGame();
}

void activate(int a)
{
    sfx(S_SWITCH, 0.4f);
    switch (a) {
    case A_CAMPAIGN:
    case A_ENDLESS:
        mode = a == A_ENDLESS ? MODE_ENDLESS : MODE_CAMPAIGN;
        state = ST_MAPSEL;
        mapPage = startMap / MAP_PAGE; focusBtn = startMap % MAP_PAGE;
        break;
    case A_SKINS: state = ST_SKINS; skinTab = 0; skinPage = skinShownPos(skin) / SKIN_PAGE; focusBtn = 4 + isDavid(skin) + skinShownPos(skin) % SKIN_PAGE; break; /* (+1: David's TRAIL switch comes first) */
    case A_TAB_PLAYER: skinTab = 0; skinPage = skinShownPos(skin) / SKIN_PAGE; break;
    case A_TAB_ENEMY: skinTab = 1; skinPage = eskin / ESKIN_PAGE; break;
    case A_TAB_CURSOR: skinTab = 2; skinPage = cursorSkin / SKIN_PAGE; break;
    case A_TAB_PETS: skinTab = 3; skinPage = pet / SKIN_PAGE; break;
    case A_PET_EDIT: state = ST_PETEDIT; focusBtn = 0; petNameEdit = 0; break;
    case A_PET_NAME: /* click to type a name, click again (or ENTER) when done */
        petNameEdit = !petNameEdit;
        petNameFresh = petNameEdit;
        if (!petNameEdit) saveGame();
        break;
    case A_PET_COLLAR: collarOn = !collarOn; saveGame(); break;
    case A_OPT_UPG: state = ST_UPGRADES; focusBtn = 0; break;
    case A_OPT_PLIM: perkLimitOn = !perkLimitOn; saveGame(); break;
    case A_OPT_PLIMN: break; /* slider, like the volumes */
    case A_PAGE_PREV: case A_PAGE_NEXT: flipSkinPage(a == A_PAGE_NEXT ? 1 : -1); break;
    case A_OPTIONS: optFromPause = state == ST_PAUSE; state = ST_OPTIONS; focusBtn = 0; break;
    case A_OPT_MUSIC: musicOn = !musicOn; saveGame(); break;
    case A_OPT_SFX: sfxOn = !sfxOn; saveGame(); sfx(S_PICKUP, 0.5f); break;
    case A_OPT_PERKS: perksOn = !perksOn; saveGame(); break;
    case A_OPT_SHAKE: shakeOn = !shakeOn; saveGame(); break;
    case A_OPT_DMGNUM: dmgNumsOn = !dmgNumsOn; saveGame(); break;
    case A_OPT_HARD:
        if (!hardUnlocked && !devMode) { toast("LOCKED - CLEAR THE LAST CAMPAIGN DISTRICT FIRST"); sfx(S_HURT, 0.3f); break; }
        if (optFromPause) { toast("ONLY FROM THE MAIN MENU OPTIONS"); break; }
        hardOn = !hardOn;
        toast(hardOn ? "HARD MODE ON - TOUGHER ENEMIES, DOUBLE COINS" : "HARD MODE OFF");
        sfx(hardOn ? S_BOSS : S_PICKUP, 0.5f);
        saveGame();
        break;
    case A_OPT_FULL: fullReq = 1; break; /* the platform switches it right after this click or key */
    case A_OPT_MVOL: case A_OPT_SVOL: break; /* sliders: dragged with the mouse or nudged with LEFT / RIGHT */
    case A_OPT_MENUBG:
        if (optFromPause) { toast("CHANGE IT FROM THE MAIN MENU OPTIONS"); break; }
        state = ST_MENUBG;
        bgPage = bgCard() / MAP_PAGE; focusBtn = bgCard() % MAP_PAGE;
        break;
    case A_OPT_DEV: setDevMode(!devMode); break;
    case A_OPT_TRACK: /* the SOUNDTRACK screen, on the current pick (buttons: + ADD SONG, the 12 choices, then 3 per song of yours) */
        state = ST_SONGS; songDel = -1; uiFollow = 1; /* scroll down to it */
        songSearch[0] = 0; searchEdit = 0;
        focusBtn = musicMode < 2 + MUS_TRACKS ? 2 + musicMode : 4 + MUS_TRACKS + 3 * (musicMode - 2 - MUS_TRACKS); /* after SEARCH and + ADD SONG */
        break;
    case A_OPT_SONGS:
        if (optFromPause) { toast("CHANGE IT FROM THE MAIN MENU OPTIONS"); break; }
        state = ST_SONGS; focusBtn = 0; songDel = -1;
        break;
    case A_OPT_RESET:
        if (gt - resetArmT < 3) { resetArmT = -10; resetSettings(); }
        else { resetArmT = gt; toast("CLICK AGAIN TO RESET ALL SETTINGS"); }
        break;
    case A_SANDY: sandyOff = !sandyOff; saveGame(); toast(sandyOff ? "SANDEVISTAN TRAIL OFF" : "SANDEVISTAN TRAIL ON"); break;
    case A_SEARCH: searchEdit = !searchEdit; songDel = -1; break; /* click to type, click again (or ENTER) when done */
    case A_SEARCH_CLR: songSearch[0] = 0; searchEdit = 0; uiScroll = 0; break;
    case A_SONG_ADD: /* the platform opens a file picker; the list fills in once the files are read */
        songDel = -1;
        if (myCount >= MY_MAX) toast("8 SONGS IS THE MOST - REMOVE ONE FIRST");
        else toast(plat_song_add() ? "PICK ONE OR MORE MUSIC FILES" : "CAN'T ADD SONGS IN THIS VERSION");
        break;
    case A_OPT_TITLE: /* buy it once, then it opens the MENU TITLE screen */
        if (optFromPause) { toast("CHANGE IT FROM THE MAIN MENU OPTIONS"); break; }
        if (!ownsTitle()) {
            char s[64];
            if (coins < TITLE_COST) { sfmt(s, sizeof s, "NEED %d COINS (YOU HAVE %d)", TITLE_COST, coins); toast(s); sfx(S_HURT, 0.3f); break; }
            coins -= TITLE_COST;
            titleOwned = 1;
            toast("UNLOCKED: YOUR OWN MENU TITLE!");
            sfx(S_PICKUP, 0.7f);
            saveGame();
        }
        state = ST_TITLE; focusBtn = 0; titleEdit = 0; tsPage = titleStyle / TS_PAGE;
        break;
    case A_TTL_L1: case A_TTL_L2: {
        int l = a == A_TTL_L1 ? 1 : 2;
        titleEdit = titleEdit == l ? 0 : l; /* click the line again to finish */
        titleFresh = titleEdit != 0;
        if (!titleEdit) saveGame();
        break; }
    case A_TTL_RESET:
        sfmt(title1, sizeof title1, "NIGHT CITY"); sfmt(title2, sizeof title2, "SIEGE");
        titleStyle = 0; titleEdit = 0; saveGame(); toast("MENU TITLE RESET");
        break;
    case A_OPT_UI: /* the picker screen from the main menu; from the pause screen it just steps to the next style */
        if (optFromPause) { pickUi(uiAt((uiSlot(uiStyle) + 1) % UI_COUNT)); break; }
        state = ST_UISTYLE;
        uiPage = uiSlot(uiStyle) / UI_PAGE; focusBtn = uiSlot(uiStyle) % UI_PAGE;
        break;
    case A_QUESTS: state = ST_QUESTS; questPage = 0; focusBtn = 0; refreshDaily(); break;
    case A_KEYS: state = ST_KEYS; focusBtn = 0; break;
    case A_OPT_WIPE:
        if (optFromPause) { toast("ONLY FROM THE MAIN MENU OPTIONS"); break; }
        state = ST_WIPE; wipeStep = 1; focusBtn = 0; sfx(S_HURT, 0.4f);
        break;
    case A_WIPE_NEXT: wipeStep = 2; wipeT = gt; focusBtn = 0; sfx(S_HURT, 0.5f); break;
    case A_WIPE_DO:
        if (gt - wipeT < WIPE_WAIT) { toast("WAIT FOR THE COUNTDOWN - OR CANCEL"); sfx(S_HURT, 0.3f); break; }
        wipeEverything();
        break;
    case A_ACH: state = ST_ACH; achPage = 0; focusBtn = 1; achCheckOwned(); break;
    case A_QUIT: saveGame(); state = ST_QUIT; focusBtn = 0; wantsQuit = 1; break;
    case A_BACK:
        if (state == ST_MENUBG) { state = ST_OPTIONS; focusBtn = 9; loadDistrict(menuMap()); menuMusMap = curMap; }
        else if (state == ST_TITLE) { state = ST_OPTIONS; focusBtn = 10; titleEdit = 0; saveGame(); }
        else if (state == ST_UISTYLE) { state = ST_OPTIONS; focusBtn = 11; }
        else if (state == ST_SONGS) { state = ST_OPTIONS; focusBtn = 4; songDel = -1; searchEdit = 0; songSearch[0] = 0; }
        else if (state == ST_UPGRADES) { state = ST_OPTIONS; focusBtn = 5; saveGame(); }
        else if (state == ST_WIPE) { state = ST_OPTIONS; focusBtn = 14; uiFollow = 1; toast("NOTHING WAS DELETED"); }
        else if (state == ST_EXTRA) { state = ST_KEYS; focusBtn = 0; }
        else if (state == ST_OPTIONS && optFromPause) { state = ST_PAUSE; focusBtn = 1; saveGame(); }
        else { int fromOpt = state == ST_OPTIONS, fromPet = state == ST_PETEDIT, fromAch = state == ST_ACH; toMenu(); if (fromOpt) focusBtn = 4; if (fromPet) { focusBtn = 7; petNameEdit = 0; saveGame(); } if (fromAch) focusBtn = pet ? 8 : 7; }
        break;
    case A_TOMENU: toMenu(); break;
    case A_RESUME: state = ST_PLAY; break;
    case A_RETRY: newGame(); break;
    default:
        if (a >= A_MAP0 && a < A_MAP0 + D_COUNT) tryMap(a - A_MAP0);
        else if (a >= A_SKIN0 && a < A_SKIN0 + SKIN_COUNT) trySkin(a - A_SKIN0);
        else if (a >= A_ESKIN0 && a < A_ESKIN0 + ESKIN_COUNT) tryESkin(a - A_ESKIN0);
        else if (a >= A_PERK0 && a < A_PERK0 + 3) takePerk(a - A_PERK0);
        else if (a >= A_BG0 && a < A_BG_END) pickMenuBg(a - A_BG0);
        else if (a >= A_UI0 && a < A_UI0 + UI_COUNT) pickUi(a - A_UI0);
        else if (a >= A_CUR0 && a < A_CUR_END) { cursorSkin = a - A_CUR0; saveGame(); sfx(S_PICKUP, 0.5f); }
        else if (a >= A_PET0 && a < A_PET_END) tryPet(a - A_PET0);
        else if (a >= A_PCOL0 && a < A_PCOL_END) { collarCol = a - A_PCOL0; collarOn = 1; saveGame(); sfx(S_PICKUP, 0.5f); }
        else if (a == A_PCOL_NONE) { collarOn = 0; saveGame(); }
        else if (a >= A_TRK0 && a < A_TRK_END) { setMusicMode(a - A_TRK0); songDel = -1; sfx(S_PICKUP, 0.4f); }
        else if (a >= A_SONG0 && a < A_SONG0 + MY_MAX && a - A_SONG0 < myCount) { /* tie the song to the next district (or none) */
            int i = a - A_SONG0, m = myMap[i] + 1;
            myMap[i] = (signed char)(m >= D_COUNT ? -1 : m);
            plat_song_map(i, myMap[i]);
            musFor = -1; songDel = -1;
        } else if (a >= A_SONGDEL0 && a < A_SONGDEL_END && a - A_SONGDEL0 < myCount) { /* X: click twice to remove */
            int i = a - A_SONGDEL0;
            if (songDel == i) { plat_song_remove(i); songDel = -1; toast("SONG REMOVED"); }
            else { songDel = i; toast("CLICK THE X AGAIN TO REMOVE IT"); }
        }
        else if (a >= A_TS0 && a < A_TS_END) { titleStyle = a - A_TS0; titleEdit = 0; saveGame(); sfx(S_PICKUP, 0.5f); }
        break;
    }
}
