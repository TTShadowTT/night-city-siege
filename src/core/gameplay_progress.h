/* gameplay_progress.h - Progression: coins, quests, skins, enemy skins and the Sandevistan trail. */
#ifndef NCS_GAMEPLAY_PROGRESS_H
#define NCS_GAMEPLAY_PROGRESS_H

enum { P_KILLS, P_BOSSES, P_DISTRICTS, P_EXPLOSIVE, P_MEDKITS, P_ROPES, P_BESTSCORE, P_ENDLESS, P_GRAVITY, P_PERKS, P_SHIELDS,
       P_WAVES, P_COMBO, P_DOUBLES, P_DASHES, P_RJUMPS, P_DAILIES, P_COINS, P_COUNT }; /* append only: the save stores them in order */
#define SKIN_COUNT 25
#define QUEST_COUNT 27

/* cost < 0: unlocked by a quest (quest -2: unlocked another way). shape: idle, run, run, jump frames (0 = the standard agent) */
typedef struct { const char *name; unsigned pal[8]; int cost, quest; const Spr *shape[4]; } Skin;

/* a faction's own enemies: sprites for the thug, gunner, brute and drone (two frames each, slot EB_x + frame),
   drawn with their own colours for the letters h v r j J p P b */
enum { EB_THUG = 0, EB_GUNNER = 2, EB_BRUTE = 4, EB_DRONE = 6 };
typedef struct { const Spr *s[8]; unsigned pal[8]; } EnemyBody;

/* enemy skins recolour every enemy sprite letter listed in ESKIN_KEYS, and change what they bleed; body (or 0) gives
   the thug, gunner, brute and drone bodies of their own */
#define ESKIN_COUNT 11
typedef struct { const char *name; int cost; unsigned col[21], bloodA, bloodB; int confetti; const char *blurb; const EnemyBody *body; } EnemySkin;

/* DAVID's Sandevistan: while he dashes he leaves a trail of coloured afterimages that fade out
   (switched on and off on his card on the SKINS screen; saved as settings bit 1 << 25, set = off) */
#define MAXGHOST 24
#define GHOST_LIFE 1.0f
typedef struct { float x, y, life; int face, f, n; } Ghost;

typedef struct { const char *title; int stat, goal, coins, skin; } Quest;

extern const Skin SKINS[SKIN_COUNT];
extern const EnemySkin ESKINS[ESKIN_COUNT];
extern int eskin, eskinsMask;
extern const EnemyBody *eBody; /* the body of the enemy skin enemyLook switched on, or 0 */
extern Ghost ghosts[MAXGHOST];
extern int ghostN, sandyOff;
extern const Quest QUESTS[QUEST_COUNT];
extern const int MAP_COST[D_COUNT];
extern int coins, mapsMask, skinsMask, skin, questMask, pstat[P_COUNT];

void enemyLook(int k, int on);
/* the sprite for body slot `slot` + frame f of the current enemy look, or `dflt` when it has no body of its own */
const Spr *enemyBodySpr(int slot, int f, const Spr *dflt);
/* frame f (0 idle, 1 run, 2 run, 3 jump) of skin k */
const Spr *skinFrame(int k, int f);
const unsigned *skinPal(int k);
int isDavid(int k);
void addGhost(float x, float feet, int face, int f);
/* what you can use right now: what you own, or everything in developer mode (the real masks stay untouched) */
int ownsMap(int i);
int ownsTitle(void);
int ownsSkin(int i);
int ownsESkin(int i);

#endif
