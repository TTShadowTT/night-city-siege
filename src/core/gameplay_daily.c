/* gameplay_daily.c - Daily quests. */
#include "core.h"

enum { DP_NONE, DP_ETYPE, DP_WEAPON, DP_DISTRICT }; /* what a template parameter ranges over */
typedef struct { int ev, best, kind[3], g0, gstep, tiers, r0, rstep; const char *fmt; } DailyT;
static const DailyT DAILYT[] = {
    { DE_KILL, 0, { 0, 0, 0 }, 40, 20, 10, 60, 15, "SLAY %d ENEMIES" },
    { DE_KILL, 0, { DP_ETYPE, 0, 0 }, 12, 4, 10, 60, 12, "TAKE DOWN %d %s" },
    { DE_KILL, 0, { DP_WEAPON, 0, 0 }, 15, 5, 10, 70, 12, "GET %d KILLS WITH %s" },
    { DE_KILL, 0, { DP_ETYPE, DP_WEAPON, 0 }, 5, 2, 8, 100, 15, "TAKE DOWN %d %s WITH %s" },
    { DE_KILL, 0, { DP_DISTRICT, 0, 0 }, 30, 15, 10, 70, 12, "SLAY %d ENEMIES IN %s" },
    { DE_KILL, 0, { DP_ETYPE, DP_DISTRICT, 0 }, 8, 3, 8, 90, 14, "TAKE DOWN %d %s IN %s" },
    { DE_KILL, 0, { DP_WEAPON, DP_DISTRICT, 0 }, 10, 4, 8, 100, 14, "GET %d KILLS WITH %s IN %s" },
    { DE_KILL, 0, { DP_ETYPE, DP_WEAPON, DP_DISTRICT }, 4, 2, 6, 140, 20, "TAKE DOWN %d %s WITH %s IN %s" },
    { DE_KILL_AIR, 0, { 0, 0, 0 }, 10, 5, 10, 80, 15, "GET %d KILLS IN MID-AIR" },
    { DE_KILL_AIR, 0, { DP_WEAPON, 0, 0 }, 5, 3, 8, 110, 15, "GET %d MID-AIR KILLS WITH %s" },
    { DE_KILL_ROPE, 0, { 0, 0, 0 }, 5, 3, 10, 90, 15, "GET %d KILLS WHILE HANGING ON A ROPE" },
    { DE_KILL_ELITE, 0, { 0, 0, 0 }, 5, 3, 10, 90, 15, "TAKE DOWN %d ELITES" },
    { DE_WAVE, 0, { 0, 0, 0 }, 3, 2, 10, 60, 20, "CLEAR %d WAVES" },
    { DE_WAVE, 0, { DP_DISTRICT, 0, 0 }, 2, 2, 10, 70, 20, "CLEAR %d WAVES IN %s" },
    { DE_FLAWLESS, 0, { 0, 0, 0 }, 1, 1, 8, 100, 30, "CLEAR %d WAVES WITHOUT GETTING HIT" },
    { DE_ENDLESS, 1, { 0, 0, 0 }, 5, 1, 12, 80, 20, "REACH WAVE %d IN ENDLESS" },
    { DE_ENDLESS, 1, { DP_DISTRICT, 0, 0 }, 4, 1, 10, 90, 20, "REACH WAVE %d IN ENDLESS IN %s" },
    { DE_SCORE, 1, { 0, 0, 0 }, 2000, 1500, 12, 60, 20, "SCORE %d IN ONE RUN" },
    { DE_COMBO, 1, { 0, 0, 0 }, 10, 5, 10, 60, 18, "REACH A %d KILL COMBO" },
    { DE_MULTI, 0, { 0, 0, 0 }, 5, 3, 10, 70, 15, "GET %d MULTI-KILLS" },
    { DE_MEDKIT, 0, { 0, 0, 0 }, 3, 2, 8, 60, 15, "PICK UP %d MEDKITS" },
    { DE_PERK, 0, { 0, 0, 0 }, 3, 2, 8, 60, 15, "TAKE %d UPGRADES" },
    { DE_ROPE, 0, { 0, 0, 0 }, 5, 5, 8, 50, 10, "CLIMB %d ROPES" },
    { DE_DASH, 0, { 0, 0, 0 }, 20, 15, 8, 50, 10, "DASH %d TIMES" },
    { DE_RJUMP, 0, { 0, 0, 0 }, 3, 2, 8, 80, 15, "ROCKET JUMP %d TIMES" },
    { DE_BOSS, 0, { 0, 0, 0 }, 1, 1, 4, 120, 60, "DOWN %d GUNSHIPS" },
    { DE_COINS, 0, { 0, 0, 0 }, 100, 75, 10, 60, 12, "EARN %d COINS" },
    { DE_DISTRICT, 0, { 0, 0, 0 }, 1, 1, 3, 150, 80, "CLEAR %d DISTRICTS IN THE CAMPAIGN" },
};
#define DAILYT_COUNT (int)(sizeof DAILYT / sizeof DAILYT[0])
static const char *const ENEMY_PLURAL[6] = { "THUGS", "DRONES", "GUNNERS", "BOMBERS", "BRUTES", "RIOT SHIELDS" };
static const int ETYPE_PCT[6] = { 100, 100, 70, 50, 35, 40 }; /* rarer enemies need fewer kills */
static const char *const WEAPON_PHRASE[7] = { "THE PISTOL", "THE MINIGUN", "THE SHOTGUN", "THE RAILGUN", "GRENADES", "ROCKETS", "THE GRAVITY GUN" };
Daily dqCache[3]; /* today's picks dq[] (pool indices) decoded; progress in dqProg[], finished bits in dqDone */

static int kindSize(int k) { return k == DP_ETYPE ? 6 : k == DP_WEAPON ? W_COUNT : k == DP_DISTRICT ? D_COUNT : 1; }
static int templateSize(const DailyT *T) { return kindSize(T->kind[0]) * kindSize(T->kind[1]) * kindSize(T->kind[2]) * T->tiers; }
int dailyPool(void) { int n = 0; for (int i = 0; i < DAILYT_COUNT; i++) n += templateSize(&DAILYT[i]); return n; }

static void decodeDaily(int idx, Daily *q)
{
    const char *s[3] = { "", "", "" };
    int t = 0;
    while (t < DAILYT_COUNT - 1 && idx >= templateSize(&DAILYT[t])) idx -= templateSize(&DAILYT[t++]);
    const DailyT *T = &DAILYT[t];
    int tier = idx % T->tiers, pct = 100;
    idx /= T->tiers;
    q->t = t;
    for (int k = 0, a = 0; k < 3; k++) {
        q->v[k] = idx % kindSize(T->kind[k]);
        idx /= kindSize(T->kind[k]);
        if (T->kind[k] == DP_ETYPE) { s[a++] = ENEMY_PLURAL[q->v[k]]; pct = ETYPE_PCT[q->v[k]]; }
        else if (T->kind[k] == DP_WEAPON) s[a++] = WEAPON_PHRASE[q->v[k]];
        else if (T->kind[k] == DP_DISTRICT) s[a++] = DISTRICTS[q->v[k]].name;
    }
    q->goal = (T->g0 + tier * T->gstep) * pct / 100;
    if (q->goal < 1) q->goal = 1;
    q->reward = T->r0 + tier * T->rstep;
    /* fill the template: %d is the goal, each %s the next parameter name */
    int n = 0, a = 0, cap = (int)sizeof q->text - 1;
    for (const char *f = T->fmt; *f && n < cap; f++) {
        char num[12];
        const char *ins = 0;
        if (f[0] == '%' && f[1] == 'd') { sfmt(num, sizeof num, "%d", q->goal); ins = num; f++; }
        else if (f[0] == '%' && f[1] == 's') { ins = a < 3 ? s[a++] : ""; f++; }
        if (ins) while (*ins && n < cap) q->text[n++] = *ins++;
        else q->text[n++] = *f;
    }
    q->text[n] = 0;
}

/* a new local day: deal three fresh quests (different kinds, only in districts you have) */
void refreshDaily(void)
{
    int day = plat_day();
    if (day == dqDay) { /* same day as the save: just decode the saved picks */
        if (!dqCacheOk) for (int i = 0; i < 3; i++) decodeDaily(((dq[i] % dailyPool()) + dailyPool()) % dailyPool(), &dqCache[i]);
        dqCacheOk = 1;
        return;
    }
    dqDay = day;
    dqDone = 0;
    dqCacheOk = 1;
    for (int slot = 0; slot < 3; slot++) {
        dqProg[slot] = 0;
        for (int tries = 0; tries < 200; tries++) {
            int idx = (int)(hash3(day, slot, tries) % (unsigned)dailyPool()), ok = 1;
            Daily q;
            decodeDaily(idx, &q);
            for (int k = 0; k < 3; k++)
                if (DAILYT[q.t].kind[k] == DP_DISTRICT && !(mapsMask & (1 << q.v[k]))) ok = 0;
            for (int o = 0; o < slot; o++) if (dqCache[o].t == q.t || DAILYT[dqCache[o].t].ev == DAILYT[q.t].ev) ok = 0;
            dq[slot] = idx;
            dqCache[slot] = q;
            if (ok) break;
        }
    }
    saveGame();
}

void dailyEvent(int ev, int etype, int weapon, int district, int n)
{
    for (int i = 0; i < 3; i++) {
        const Daily *q = &dqCache[i];
        const DailyT *T = &DAILYT[q->t];
        if (dqDone & (1 << i) || T->ev != ev) continue;
        int match = 1;
        for (int k = 0; k < 3; k++) {
            int want = q->v[k];
            if ((T->kind[k] == DP_ETYPE && etype != want) || (T->kind[k] == DP_WEAPON && weapon != want) ||
                (T->kind[k] == DP_DISTRICT && district != want)) match = 0;
        }
        if (!match) continue;
        dqProg[i] = T->best ? (n > dqProg[i] ? n : dqProg[i]) : dqProg[i] + n;
        if (dqProg[i] >= q->goal) {
            char s[64];
            dqProg[i] = q->goal;
            dqDone |= 1 << i;
            coins += q->reward;
            sfmt(s, sizeof s, "DAILY QUEST DONE! +%d COINS", q->reward);
            if (dqDone == 7) { coins += DAILY_BONUS; sfmt(s, sizeof s, "ALL 3 DAILIES DONE! +%d COINS", q->reward + DAILY_BONUS); }
            toast(s);
            sfx(S_PERK, 0.6f);
            addProgress(P_DAILIES, 1);
            saveGame();
        }
    }
}
void dailyAdd(int ev, int n) { dailyEvent(ev, -1, -1, curMap, n); }

void addCoins(int n)
{
    float f = n * (hardOn ? 2 : 1) * (1 + 0.5f * perkPow[PK_LUCK]) + coinFrac; /* HARD MODE: double coins */ /* lucky charm: fractions carry over to the next coin */
    int k = (int)f;
    coinFrac = f - k;
    coins += k; runCoins += k;
    if (k > 0) { addProgress(P_COINS, k); dailyAdd(DE_COINS, k); }
}
