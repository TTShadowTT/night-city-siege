/* gameplay_daily.h - Daily quests. */
#ifndef NCS_GAMEPLAY_DAILY_H
#define NCS_GAMEPLAY_DAILY_H

/* daily quests
 * Every quest in the pool is a template ("TAKE DOWN %d %s WITH %s IN %s") times its parameters (enemy type,
 * weapon, district) times a goal tier. A pool index decodes to one concrete quest; each local calendar day
 * picks three indices from the date, so everyone gets the same three on the same day. */
enum { DE_KILL, DE_KILL_AIR, DE_KILL_ROPE, DE_KILL_ELITE, DE_WAVE, DE_FLAWLESS, DE_ENDLESS, DE_SCORE, DE_COMBO, DE_MULTI,
       DE_MEDKIT, DE_PERK, DE_ROPE, DE_DASH, DE_RJUMP, DE_BOSS, DE_COINS, DE_DISTRICT };
#define DAILY_BONUS 250 /* for finishing all three */

typedef struct { int t, v[3], goal, reward; char text[72]; } Daily;

extern Daily dqCache[3]; /* today's picks dq[] (pool indices) decoded; progress in dqProg[], finished bits in dqDone */

int dailyPool(void);
/* a new local day: deal three fresh quests (different kinds, only in districts you have) */
void refreshDaily(void);
void dailyEvent(int ev, int etype, int weapon, int district, int n);
void dailyAdd(int ev, int n);
void addCoins(int n);

#endif
