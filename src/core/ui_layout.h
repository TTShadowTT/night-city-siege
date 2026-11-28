/* ui_layout.h - Menu buttons: where each screen puts them, and what the mouse is over. */
#ifndef NCS_UI_LAYOUT_H
#define NCS_UI_LAYOUT_H

enum { A_CAMPAIGN, A_ENDLESS, A_SKINS, A_QUESTS, A_KEYS, A_QUIT, A_BACK, A_RESUME, A_TOMENU, A_RETRY, A_MAP0,
       A_SKIN0 = A_MAP0 + D_COUNT, A_PERK0 = A_SKIN0 + SKIN_COUNT, A_ESKIN0 = A_PERK0 + 3, A_OPTIONS = A_ESKIN0 + ESKIN_COUNT,
       A_TAB_PLAYER, A_TAB_ENEMY, A_OPT_MUSIC, A_OPT_MVOL, A_OPT_SVOL, A_OPT_PERKS, A_OPT_SHAKE, A_PAGE_PREV, A_PAGE_NEXT,
       A_QTAB_DAILY, A_QTAB_ALL, A_OPT_MENUBG, A_OPT_DEV, A_OPT_UI, A_OPT_FULL, A_UI0, A_UI_END = A_UI0 + 64, /* UI STYLE cards */
       A_BG0, A_BG_END = A_BG0 + D_COUNT + 3, /* menu background cards: LAST PLAYED, RANDOM, the picture, then every map */
       A_OPT_UPG, A_OPT_PLIM, A_OPT_PLIMN, A_TAB_CURSOR, A_CUR0, A_CUR_END = A_CUR0 + CURSOR_COUNT,
       A_TAB_PETS, A_PET0, A_PET_END = A_PET0 + PET_COUNT, A_PET_EDIT, A_PET_COLLAR, A_PCOL0, A_PCOL_END = A_PCOL0 + COLLAR_COUNT, A_PCOL_NONE, A_PET_NAME,
       A_OPT_SONGS, A_SONG_ADD, A_SONG0, A_SONGDEL0 = A_SONG0 + MY_MAX, A_SONGDEL_END = A_SONGDEL0 + MY_MAX, A_TRK0, A_TRK_END = A_TRK0 + 2 + MUS_TRACKS + MY_MAX, A_SEARCH, A_SEARCH_CLR, /* SOUNDTRACK screen */
       A_SANDY, /* DAVID: Sandevistan trail on / off */
       A_OPT_RESET, /* OPTIONS: every setting back to default (click twice) */
       A_OPT_TITLE, A_OPT_TRACK, A_TTL_L1, A_TTL_L2, A_TTL_RESET, A_TS0, A_TS_END = A_TS0 + 64, /* MENU TITLE screen (room for 64 styles) */
       A_ACH, /* main menu: the achievements screen */
       A_OPT_WIPE, A_WIPE_NEXT, A_WIPE_DO, A_OPT_DMGNUM, A_OPT_HARD, A_OPT_SFX }; /* OPTIONS: RESET EVERYTHING, and its two warning screens */
#define BG_FIXED 3 /* cards before the first map */
#define BG_CHOICES (D_COUNT + BG_FIXED)
#define UI_PAGE 12 /* UI STYLE cards per page: 4 x 3 */
#define QUEST_PAGE 8
#define MAP_PAGE 4
/* the skins screen shows a page of cards per tab */
#define SKIN_PAGE 12
#define ESKIN_PAGE 5
#define MAXBTN 48
typedef struct { int x, y, w, h, action; const char *label; } Btn;
int menuStep(void); /* main menu buttons: 19 high, spread out as far as the screen allows (no scrolling) */

extern const char *const BG_FIXED_NAME[BG_FIXED];
extern int bgPage;
extern int uiPage;
extern int mapPage;
extern int questPage; /* quests screen: the dailies are pinned on top, the full list pages below */
extern int skinPage;
extern Btn btns[MAXBTN];
extern int nBtns;
extern int uiScroll, uiScrollMax, uiScrollState, uiFollow;
extern int wipeStep; /* RESET EVERYTHING: 1 = the first warning, 2 = the last one */
extern float wipeT; /* when the last warning opened: DELETE EVERYTHING waits WIPE_WAIT seconds */
#define WIPE_WAIT 5

int bgCard(void);
int skinPages(void);
int scrY(int y);
int menuBtnTop(void);
/* the clickable things on the current screen; used both to draw and to hit-test */
void layoutUI(void);
int btnAt(int x, int y);
int menuMap(void);
void toMenu(void);
const char *menuBgName(void);

#endif
