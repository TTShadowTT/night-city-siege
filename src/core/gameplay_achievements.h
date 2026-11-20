/* gameplay_achievements.h - Achievements: one-off goals that each pay coins once, listed on their own screen. */
#ifndef NCS_GAMEPLAY_ACHIEVEMENTS_H
#define NCS_GAMEPLAY_ACHIEVEMENTS_H

/* append only: the save stores them as one bit mask ("#C mask"; the older "#A" lists were reset) */
enum { ACH_FIRST_BLOOD, ACH_UNTOUCHABLE, ACH_GUNSHIP_ACE, ACH_GIANT, ACH_SMASHER, ACH_ROCKET, ACH_CLOSE_CALL, ACH_HORIZON,
       ACH_OVERCLOCK, ACH_COMBO100, ACH_ENDLESS40, ACH_ALL_MAPS, ACH_SKINS10, ACH_PETS10, ACH_HARD, /* ACH_HARD took the slot of a removed one (see loadGame) */
       ACH_PAD, ACH_S1, ACH_ALL, ACH_S2, ACH_COUNT };
#define ACH_PAGE 10
/* secret: shown as ??? until it's done */
typedef struct { const char *name, *what; int coins, secret; } Achievement;

extern const Achievement ACHS[ACH_COUNT];
extern int achMask, achPage;

int achDone(int id);
int achCount(void);
/* unlocks it once: coins, a toast and a save (never during autopilot test runs) */
void achGive(int id);
/* the ones about what you own: checked after buying, and when the game starts */
void achCheckOwned(void);
/* list order: LEGEND OF NIGHT CITY stays last, though newer ones come after it in the save */
int achAt(int slot);

#endif
