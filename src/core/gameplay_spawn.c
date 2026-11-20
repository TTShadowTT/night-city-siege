/* gameplay_spawn.c - Spawning particles, pickups, enemies, bosses and waves. */
#include "core.h"

void addPart(float x, float y, float vx, float vy, float life, unsigned c, int sz, float grav, int bounce)
{
    for (int i = 0; i < MAXP; i++)
        if (parts[i].life <= 0) {
            Part *p = &parts[i];
            p->x = x; p->y = y; p->vx = vx; p->vy = vy; p->life = p->max = life;
            p->c = c; p->sz = sz; p->grav = grav; p->bounce = bounce; p->blood = 0;
            return;
        }
}

/* blood drops fly, fall and leave a stain on whatever ledge or street they land on */
static void bleedAs(float x, float y, int n, float dirx, float spd, int enemy)
{
    static const unsigned confetti[6] = { C(255, 80, 160), C(80, 200, 255), C(255, 230, 60), C(120, 255, 120), C(200, 120, 255), C(255, 140, 40) };
    const EnemySkin *E = &ESKINS[enemy ? eskin : 0];
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < MAXP; i++)
            if (parts[i].life <= 0) {
                Part *p = &parts[i];
                float a = frand() * 2 * PI, s = spd * frange(0.2f, 1.0f);
                p->x = x; p->y = y;
                p->vx = cosf(a) * s * 0.6f + dirx * spd * frange(0.2f, 0.8f);
                p->vy = sinf(a) * s * 0.6f - spd * frange(0.2f, 0.7f);
                p->life = p->max = frange(0.8f, 1.6f);
                p->c = E->confetti ? confetti[irand(6)] : frand() < 0.5f ? E->bloodA : E->bloodB;
                p->sz = frand() < 0.35f ? 2 : 1; p->grav = 650; p->bounce = 1; p->blood = 1;
                break;
            }
    }
}

void bleed(float x, float y, int n, float dirx, float spd) { bleedAs(x, y, n, dirx, spd, 0); }      /* the player */
void gore(float x, float y, int n, float dirx, float spd) { bleedAs(x, y, n, dirx, spd, 1); }       /* an enemy */

void burst(float x, float y, int n, float spd, unsigned c, float life)
{
    for (int i = 0; i < n; i++) {
        float a = frand() * 2 * PI, s = spd * frange(0.3f, 1.0f);
        addPart(x, y, cosf(a) * s, sinf(a) * s - spd * 0.4f, life * frange(0.6f, 1.2f), c, frand() < 0.3f ? 2 : 1, 500, 1);
    }
}

void floater(float x, float y, const char *s, unsigned c)
{
    for (int i = 0; i < MAXF; i++)
        if (fl[i].life <= 0) {
            fl[i].x = clampf(x, 40, FW - 40); fl[i].y = fmaxf(y, 34);
            fl[i].life = 1.0f; fl[i].c = c;
            sfmt(fl[i].s, sizeof fl[i].s, "%s", s);
            return;
        }
}

void addBoom(float x, float y, float r)
{
    for (int i = 0; i < MAXBOOM; i++)
        if (booms[i].life <= 0) { booms[i].x = x; booms[i].y = y; booms[i].r = r; booms[i].life = booms[i].max = 0.35f; return; }
}

void addRopeFx(float x, float y0, float y1)
{
    for (int i = 0; i < MAXRFX; i++)
        if (ropeFx[i].life <= 0) { ropeFx[i].x = x; ropeFx[i].y0 = y0; ropeFx[i].y1 = y1; ropeFx[i].life = 0.4f; return; }
}

void spawnEShot(float x, float y, float vx, float vy, int kind)
{
    for (int i = 0; i < MAXES; i++)
        if (eshots[i].life <= 0) {
            eshots[i].x = x; eshots[i].y = y; eshots[i].vx = vx; eshots[i].vy = vy;
            eshots[i].life = kind == 2 ? 4.5f : 5; eshots[i].kind = kind;
            return;
        }
}

/* crates parachute in from above; enemy drops pop out where the enemy died */
void dropPickup(float x, float y, int type, int chute)
{
    for (int i = 0; i < MAXPU; i++)
        if (pu[i].life <= 0) {
            Pickup *p = &pu[i];
            p->x = clampf(x, 12, FW - 12); p->y = chute ? -12 : y;
            p->vy = chute ? 40 : -120; p->life = chute ? 20 : 14;
            p->type = type; p->landed = 0; p->chute = chute; p->plat = -1;
            return;
        }
}

Enemy *spawnEnemy(int type, float x, float anchorY)
{
    for (int i = 0; i < MAXE; i++) {
        if (en[i].alive) continue;
        Enemy *e = &en[i];
        int L = level();
        float hpScale = 1 + 0.06f * (L - 1), spScale = 1 + fminf((float)L, 25) * 0.01f;
        memset(e, 0, sizeof *e);
        e->alive = 1; e->type = type; e->x = x; e->phase = frand() * 10; e->plat = -1; e->rope = -1; e->facing = 1;
        switch (type) {
        case EN_THUG: e->hp = 4; e->speed = 55; e->hw = 4; e->hh = 9; break;
        case EN_DRONE: e->hp = 3; e->speed = 70; e->hw = 6; e->hh = 4; e->timer = frange(2, 3.5f); break;
        case EN_GUNNER: e->hp = 6; e->speed = 45; e->hw = 4; e->hh = 9; e->timer = frange(1.5f, 2.5f); break;
        case EN_BOMBER: e->hp = 3; e->speed = 85; e->hw = 4; e->hh = 6; break;
        case EN_BRUTE: e->hp = 24; e->speed = 34; e->hw = 7; e->hh = 12; e->timer = 1.5f; break;
        case EN_SHIELD: e->hp = 10; e->speed = 36; e->hw = 5; e->hh = 9; e->timer = 2.0f; break;
        case EN_NINJA: e->hp = 9; e->speed = 88; e->hw = 4; e->hh = 9; e->timer = frange(1.5f, 3); break;
        case EN_JET: e->hp = 7; e->speed = 75; e->hw = 5; e->hh = 8; e->timer = frange(1.5f, 2.5f); e->ax = frand() < 0.5f ? -1.f : 1.f; break;
        case EN_MECH: e->hp = 45; e->speed = 26; e->hw = 9; e->hh = 9; e->timer = 2.5f; e->timer2 = 1; break;
        case EN_BOSS:
            e->hp = 220 + 90 * area; e->speed = 70; e->hw = 26; e->hh = 8;
            e->timer = 2.0f; e->timer2 = 5; e->timer3 = 8; e->ax = 3;
            hpScale = 1; spScale = 1;
            break;
        }
        e->hp *= hpScale;
        e->speed *= spScale;
        if (waveKind == WK_ELITE && type != EN_BOSS && frand() < 0.45f) { e->elite = 1; e->hp *= 1.8f; e->speed *= 1.12f; }
        if (hardOn) e->hp *= HARD_HP;
        e->maxhp = e->hp;
        if (type == EN_DRONE || type == EN_JET) {
            e->y = -10;
        } else if (type == EN_BOSS) {
            e->x = FW / 2.f; e->y = -30; e->state = -1; e->targetY = 46;
        } else { /* rappel: a rope drops first, then the enemy slides down it */
            e->anchorY = anchorY;
            e->y = anchorY - e->hh - 4;
            e->targetY = surfaceBelow(x, anchorY + 4, 0) - e->hh;
            e->state = anchorY > 0 ? -1 : -2;
            e->timer3 = 0.45f;
            sfx(S_ROPE, 0.25f);
        }
        return e;
    }
    return 0;
}

int countAlive(void)
{
    int n = 0;
    for (int i = 0; i < MAXE; i++) n += en[i].alive;
    return n;
}

int pickType(void)
{
    static const int unlock[EN_GRUNTS] = { 1, 2, 3, 4, 6, 7 };
    int w[EN_GRUNTS], sum = 0, L = level();
    for (int t = 0; t < EN_GRUNTS; t++) {
        w[t] = L >= unlock[t] ? D->weights[t] : 0;
        if (t == EN_BRUTE && L >= unlock[t] && w[t] == 0) w[t] = 6;
        if (waveKind == WK_SWARM && t == D->signature) w[t] = (w[t] + 10) * (t == EN_BRUTE ? 2 : 4);
        sum += w[t];
    }
    /* every big boss beaten lets one more new kind in; the wave right after it is full of them */
    int unlocked = (runWave - 1) / 20, w2[EN_NEW];
    for (int n = 0; n < EN_NEW; n++) {
        w2[n] = unlocked > n ? 14 + 4 * (unlocked - EN_NEW > 0 ? unlocked - EN_NEW : 0) : 0;
        if (unlocked == n + 1 && (runWave - 1) % 20 == 0) w2[n] = sum / 3 + 10;
        sum += w2[n];
    }
    if (sum <= 0) return EN_THUG;
    int r = irand(sum);
    for (int t = 0; t < EN_GRUNTS; t++) { if (r < w[t]) return t; r -= w[t]; }
    for (int n = 0; n < EN_NEW; n++) { if (r < w2[n]) return EN_NINJA + n; r -= w2[n]; }
    return EN_THUG;
}

void spawnFromQueue(void)
{
    int type = pickType();
    float x = 0;
    for (int tries = 0; tries < 20; tries++) {
        x = frange(16, FW - 16);
        if (fabsf(x - px) > 60) break;
    }
    if (type == EN_DRONE) {
        for (int k = 0; k < 2; k++) spawnEnemy(EN_DRONE, clampf(x + frange(-20, 20), 10, FW - 10), 0);
    } else {
        spawnEnemy(type, x, 0);
    }
}

static const char *enemyName(int t)
{
    static const char *n[] = { "THUG", "DRONE", "GUNNER", "BOMBER", "BRUTE", "RIOT SHIELD" };
    return n[t];
}
const char *const MEGA_NAME[MEGA_COUNT] = { "ADAM SMASHER", "BLOOD OGRE", "IRON SHOGUN", "TOXIC TITAN", "FROST REAPER", "NEON DEMON" };
const unsigned MEGA_COL[MEGA_COUNT] = { C(255, 40, 40), C(255, 210, 60), C(190, 90, 255), C(120, 255, 90), C(80, 230, 255), C(60, 255, 230) };
const int MEGA_STYLE[MEGA_COUNT] = { 2, 0, 1, 2, 1, 0 };
static int lastMega = -1;
static const char *const MEGA_LINE[MEGA_COUNT] = { "ARASAKA'S CHROME-PLATED NIGHTMARE", "HUNGRY. ANGRY. HUGE.", "THE BLADE OF THE OLD WAYS",
                                                   "A WALKING TOXIC SPILL", "WINTER COMES FOR YOU", "BORN IN THE NEON" };
static void bossCard(const char *name, const char *sub, const char *line, unsigned col)
{
    sfmt(bossCardName, sizeof bossCardName, "%s", name);
    sfmt(bossCardSub, sizeof bossCardSub, "%s", sub);
    sfmt(bossCardLine, sizeof bossCardLine, "%s", line);
    bossCardCol = col;
    bossCardT = BOSS_CARD_T;
}

/* a big boss with skin k at strength tier (1 = wave 20), dropping in on the side away from you */
Enemy *spawnMega(int k, int tier)
{
    Enemy *e = spawnEnemy(EN_BOSS, FW / 2.f, 0);
    if (!e) return 0;
    lastMega = k;
    e->mega = 1 + k;
    e->hp = e->maxhp = (1400.f + 800.f * (tier - 1)) * (hardOn ? HARD_HP : 1);
    e->hw = 16; e->hh = 32; e->speed = 55;
    e->x = px < FW / 2.f ? FW * 0.75f : FW * 0.25f; e->y = -80; e->targetY = GROUND_Y - e->hh;
    e->timer = 2.5f; e->timer2 = 4; e->timer3 = 7; e->ax = 5;
    bossCard(MEGA_NAME[k], hardOn ? "BIG BOSS - HARD MODE" : "BIG BOSS", MEGA_LINE[k], MEGA_COL[k]);
    return e;
}
static const char *const NEW_ENEMY_NAME[EN_NEW] = { "CYBER NINJA", "JETPACK TROOPER", "SPIDER MECH" };

void startWave(void)
{
    waveIdx++;
    runWave++;
    if (mode == MODE_ENDLESS) area = (waveIdx - 1) / 5; /* gunships get tougher every cycle */
    int L = level(), k = (waveIdx - 1) % 5 + 1;
    waveKind = k == 5 ? WK_BOSS : k == 4 ? WK_ELITE : k == 3 ? WK_SWARM : k == 2 ? WK_HORDE : WK_NORMAL;
    toSpawn = 6 + L * 2;
    spawnCd = 1.2f;
    supplyT = 22;
    sfmt(banner, sizeof banner, "WAVE %d", waveIdx);
    waveHurt = 0;
    switch (waveKind) {
    case WK_HORDE: toSpawn = toSpawn * 3 / 2; sfmt(banner2, sizeof banner2, "THE HORDE"); break;
    case WK_SWARM: sfmt(banner2, sizeof banner2, "%s SWARM", enemyName(D->signature)); break;
    case WK_ELITE: toSpawn = toSpawn * 4 / 5; sfmt(banner2, sizeof banner2, "ELITES"); break;
    case WK_BOSS:
        if (runWave % 20 == 0) { /* every 20th wave: a big boss instead of the gunship */
            int k = irand(MEGA_COUNT);
            if (k == lastMega) k = (k + 1 + irand(MEGA_COUNT - 1)) % MEGA_COUNT;
            Enemy *e = spawnMega(k, runWave / 20);
            dropPickup(frange(60, FW / 2.f), 0, 0, 1);
            dropPickup(frange(FW / 2.f, FW - 60), 0, 0, 1);
            toSpawn = 2 + L / 6;
            sfmt(banner, sizeof banner, "BIG BOSS");
            sfmt(banner2, sizeof banner2, "%s - WAVE %d", MEGA_NAME[e ? e->mega - 1 : 0], runWave);
            sfx(S_BOSS, 1.0f);
            break;
        }
        spawnEnemy(EN_BOSS, FW / 2.f, 0);
        dropPickup(frange(60, FW - 60), 0, 0, 1);
        toSpawn = 3 + L / 3;
        sfmt(banner2, sizeof banner2, "GUNSHIP: %s", D->bossName);
        bossCard(D->bossName, hardOn ? "GUNSHIP - HARD MODE" : "GUNSHIP", D->name, C(255, 90, 60));
        sfx(S_BOSS, 1.0f);
        break;
    default: sfmt(banner2, sizeof banner2, "%s", D->name); break;
    }
    if (runWave > 20 && (runWave - 1) % 20 == 0) { /* first wave after a big boss: say who just joined */
        int n = (runWave - 1) / 20 - 1;
        if (n < EN_NEW) sfmt(banner2, sizeof banner2, "NEW ENEMY: %s", NEW_ENEMY_NAME[n]);
        else sfmt(banner2, sizeof banner2, "MORE OF THEM NOW");
    }
    bannerT = 2.4f;
    sfx(S_WAVE, 0.6f);
}

void resetPlayer(void)
{
    px = FW / 2.f; py = GROUND_Y - PHH; pvx = pvy = 0;
    ropeIdx = -1; onGround = 1; onPlat = -1; dashT = 0; dropT = 0;
    petX = px - 16; petY = GROUND_Y - 4; petVx = petVy = 0; petGround = 1; petPlat = -1;
}

void newGame(void)
{
    memset(shots, 0, sizeof shots); memset(eshots, 0, sizeof eshots);
    memset(parts, 0, sizeof parts); memset(pu, 0, sizeof pu);
    memset(fl, 0, sizeof fl); memset(en, 0, sizeof en);
    memset(booms, 0, sizeof booms); memset(beams, 0, sizeof beams); memset(ropeFx, 0, sizeof ropeFx);
    memset(holes, 0, sizeof holes);
    memset(perk, 0, sizeof perk); memset(perkPow, 0, sizeof perkPow);
    perksTaken = perkPending = 0;
    maxHp = MAX_HP;
    for (int w = 0; w < W_COUNT; w++) mag[w] = magSize(w);
    curW = W_PISTOL;
    reloadT = 0; hurtFlash = 0;
    bossIdx = -1;
    resetPlayer();
    hp = maxHp; inv = 1; dashCd = fireCd = 0;
    runKills = bestCombo = streakN = 0; streakT = calloutT = hitStop = coinFrac = 0;
    score = 0; area = mode == MODE_ENDLESS ? 0 : startMap; waveIdx = runWave = 0; combo = 0; comboT = 0; shake = 0; interT = 0; areaClear = 0; newHi = 0;
    stallT = 0; runCoins = 0; bossCardT = 0;
    loadDistrict(startMap % D_COUNT);
    state = ST_PLAY;
    startWave();
}
