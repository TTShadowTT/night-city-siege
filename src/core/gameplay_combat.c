/* gameplay_combat.c - Combat: hurting the player, damaging and killing enemies, explosions and weapons. */
#include "core.h"

static void gameOver(void)
{
    state = ST_OVER;
    overT = 0;
    deaths++;
    bleed(px, py, 45, 0, 170);
    shake = 8;
    if (score > pstat[P_BESTSCORE]) newHi = 1;
    setBest(P_BESTSCORE, score);
    saveGame();
    sfx(S_HURT, 1.0f);
    sfx(S_BOOM, 0.6f);
}

void hurtPlayer(float fx, float fy, int dmg)
{
    char s[16];
    if (inv > 0 || dashT > 0 || state != ST_PLAY) return;
    (void)fy;
    if (hardOn) dmg = dmg * 3 / 2;
    if (perk[PK_ARMOR]) { dmg = (int)(dmg * (1 - fminf(0.15f * perkPow[PK_ARMOR], 0.7f)) + 0.5f); if (dmg < 1) dmg = 1; } /* never more than 70% off */
    if (!god) hp -= dmg;
    if (hp < 0) hp = 0;
    inv = 0.75f;
    waveHurt = 1;
    hurtFlash = 0.25f;
    shake = 4 + dmg * 0.12f;
    if (ropeIdx < 0) { pvx = (px >= fx ? 1 : -1) * 150; pvy = -160; }
    bleed(px, py - 2, 6 + dmg / 3, px >= fx ? 1.f : -1.f, 110);
    sfmt(s, sizeof s, "-%d", dmg);
    floater(px, py - 18, s, C(255, 80, 80));
    sfx(S_HURT, 0.8f);
    if (!selfHit) { combo = 0; comboT = 0; } /* blowing yourself up costs health, not your combo */
    if (dmg >= 20) hitStop = fmaxf(hitStop, 0.05f);
    if (hp <= 0) gameOver();
}

static unsigned enemyColor(int t)
{
    switch (t) {
    case EN_THUG: return C(186, 64, 72);
    case EN_DRONE: return C(150, 156, 176);
    case EN_GUNNER: return C(90, 116, 98);
    case EN_BOMBER: return C(255, 150, 40);
    case EN_BRUTE: return C(120, 96, 170);
    case EN_SHIELD: return C(72, 86, 124);
    case EN_NINJA: return C(60, 230, 255);
    case EN_JET: return C(110, 116, 136);
    case EN_MECH: return C(130, 130, 148);
    default: return C(255, 170, 50);
    }
}

static int explosiveKill; /* set while the player's grenade/rocket blast is dealing damage */
int killW = -1;    /* the weapon dealing the damage right now (for daily quests), -1 = none */
int gravityKill;   /* set while a black hole is crushing something */

/* people bleed; drones and bomb-bots spark */
int isFlesh(int t) { return t == EN_THUG || t == EN_GUNNER || t == EN_BRUTE || t == EN_SHIELD || t == EN_NINJA || t == EN_JET; }

void announce(const char *s, unsigned c)
{
    sfmt(callout, sizeof callout, "%s", s);
    calloutT = 1.3f;
    calloutCol = c;
}

static void killEnemy(Enemy *e)
{
    static const int base[EN_TYPES] = { 10, 15, 25, 20, 60, 35, 1500, 40, 45, 120 };
    float ex = e->x, ey = e->y;
    int type = e->type, elite = e->elite, mega = e->mega;
    char s[28];
    e->alive = 0;

    totalKills++;
    runKills++;
    addCoins(mega ? 0 : type == EN_BOSS ? 40 : type == EN_MECH ? 5 : type == EN_BRUTE || type == EN_SHIELD || type >= EN_NINJA ? 3 : elite ? 2 : 1);
    addProgress(P_KILLS, 1);
    if (explosiveKill) addProgress(P_EXPLOSIVE, 1);
    if (gravityKill) addProgress(P_GRAVITY, 1);
    if (type == EN_BOSS) addProgress(P_BOSSES, 1);
    if (type == EN_SHIELD) addProgress(P_SHIELDS, 1);
    if (gravityKill) holeKill();
    achGive(ACH_FIRST_BLOOD);
    dailyEvent(DE_KILL, type, killW, curMap, 1);
    if (!onGround && ropeIdx < 0) dailyEvent(DE_KILL_AIR, type, killW, curMap, 1);
    if (ropeIdx >= 0) dailyAdd(DE_KILL_ROPE, 1);
    if (elite) dailyAdd(DE_KILL_ELITE, 1);
    if (type == EN_BOSS) dailyAdd(DE_BOSS, 1);
    combo++;
    comboT = 2.0f;
    if (combo > bestCombo) bestCombo = combo;
    setBest(P_COMBO, combo);
    if (combo >= 100) achGive(ACH_COMBO100);
    dailyAdd(DE_COMBO, combo);
    if (perk[PK_VAMP] && hp > 0 && hp < maxHp) { int heal = (int)(2 * perkPow[PK_VAMP] + 0.5f); if (heal < 1) heal = 1; hp = hp + heal > maxHp ? maxHp : hp + heal; }
    /* multi-kills: kills landing within a moment of each other */
    streakN = streakT > 0 ? streakN + 1 : 1;
    streakT = 0.55f;
    if (streakN == 2) { addProgress(P_DOUBLES, 1); dailyAdd(DE_MULTI, 1); }
    if (combo == 15) announce("RAMPAGE!", C(255, 140, 40));
    else if (combo == 30) announce("UNSTOPPABLE!", C(255, 80, 200));
    else if (combo == 50) announce("GODLIKE!", C(120, 255, 255));
    else if (streakN == 2) announce("DOUBLE KILL", C(255, 230, 120));
    else if (streakN == 3) announce("TRIPLE KILL", C(255, 190, 80));
    else if (streakN == 4) announce("QUAD KILL", C(255, 120, 60));
    else if (streakN == 6) announce("MASSACRE!", C(255, 60, 60));
    if (type == EN_BOSS) hitStop = 0.3f;
    else if (type == EN_BRUTE || elite) hitStop = fmaxf(hitStop, 0.045f);
    int mult = 1 + combo / 6;
    if (mult > 5) mult = 5;
    int pts = base[type] * (elite ? 2 : 1) * (mega ? 5 : 1);
    score += pts * mult;
    if (mult > 1) sfmt(s, sizeof s, "%d X%d", pts, mult); else sfmt(s, sizeof s, "%d", pts);
    floater(ex, ey - 16, s, mult > 1 ? C(255, 220, 90) : C(230, 230, 230));

    unsigned bc = enemyColor(type);
    burst(ex, ey, type == EN_BOSS ? 80 : 10, type == EN_BOSS ? 220 : 120, bc, 0.8f);
    if (isFlesh(type)) gore(ex, ey, type == EN_BRUTE ? 34 : 22, 0, 150);
    else burst(ex, ey, 10, 140, C(255, 220, 120), 0.4f);
    shake = fmaxf(shake, type == EN_BOSS ? 14 : 2);
    sfx(S_KILL, 0.5f);

    if (type == EN_BOMBER || type == EN_DRONE) {
        explode(ex, ey, type == EN_BOMBER ? 30 : 16, type == EN_BOMBER ? 8 : 3, 1);
    } else if (type == EN_BOSS && mega) { /* a big boss pays out a fortune, uncut by the lucky charm */
        int reward = (300 + 200 * (runWave / 20)) * (hardOn ? 2 : 1);
        bossKills++;
        achGive(ACH_GIANT);
        if (mega - 1 == 0) achGive(ACH_SMASHER); /* MEGA_NAME[0] */
        coins += reward; runCoins += reward; addProgress(P_COINS, reward); dailyAdd(DE_COINS, reward);
        for (int k = 0; k < 16; k++) explode(ex + frange(-50, 50), ey + frange(-14, 14), 26, 0, 1);
        for (int k = 0; k < 4; k++) dropPickup(ex - 45 + k * 30, ey, 0, 0);
        sfmt(s, sizeof s, "+%d COINS", reward);
        floater(ex, ey + 4, "BIG BOSS DOWN!", MEGA_COL[mega - 1]);
        floater(ex, ey + 18, s, C(255, 214, 80));
        announce("BIG BOSS DOWN!", MEGA_COL[mega - 1]);
        hitStop = 0.5f; shake = 20;
        sfx(S_BOSS, 1.0f);
    } else if (type == EN_BOSS) {
        bossKills++;
        for (int k = 0; k < 7; k++) explode(ex + frange(-26, 26), ey + frange(-8, 8), 22, 0, 1);
        dropPickup(ex - 20, ey, 0, 0);
        dropPickup(ex + 20, ey, 0, 0);
        floater(ex, ey + 20, "GUNSHIP DOWN!", C(255, 210, 60));
        if (!waveHurt) achGive(ACH_GUNSHIP_ACE);
        sfx(S_BOSS, 1.0f);
    } else if (frand() < (elite ? 0.18f : hp <= 40 ? 0.10f : 0.06f) * (1 + 0.6f * fminf(perkPow[PK_LUCK], 4))) {
        dropPickup(ex, ey, 0, 0); /* medkit */
    }
}

/* DAMAGE NUMBERS: the hits of a moment add up into one number over the enemy (shown x10, so they read as whole numbers) */
void dmgFlush(Enemy *e)
{
    char s[12];
    if (e->dmgAcc <= 0) return;
    sfmt(s, sizeof s, "%d", (int)(e->dmgAcc * 10 + 0.5f));
    floater(e->x + frange(-5, 5), e->y - e->hh - 6, s, e->mega ? MEGA_COL[e->mega - 1] : C(255, 170, 90));
    e->dmgAcc = 0; e->dmgT = 0;
}

void damageEnemy(Enemy *e, float dmg, float kx, float ky)
{
    if (!e->alive || e->state == -2) return;
    e->hp -= dmg;
    e->flash = 0.08f;
    float kb = e->type == EN_BOSS ? 0 : e->type == EN_BRUTE ? (e->state == 2 ? 0 : 25) : 80;
    e->vx += kx * kb;
    if (e->type == EN_DRONE) e->vy += ky * kb;
    if (dmgNumsOn) { e->dmgAcc += dmg; if (e->dmgT <= 0) e->dmgT = 0.2f; }
    if (e->hp <= 0) { dmgFlush(e); killEnemy(e); }
}

int inBox(const Enemy *e, float x, float y, float m)
{
    return x > e->x - e->hw - m && x < e->x + e->hw + m && y > e->y - e->hh - m && y < e->y + e->hh + m;
}

void explode(float x, float y, float radius, float dmg, int byPlayer)
{
    addBoom(x, y, radius);
    for (int i = 0; i < 18; i++) {
        float a = frand() * 2 * PI, s = frange(30, 160);
        addPart(x, y, cosf(a) * s, sinf(a) * s - 40, frange(0.3f, 0.8f), frand() < 0.5f ? C(255, 200, 60) : C(90, 84, 90), 2, frand() < 0.5f ? 400 : -30, 1);
    }
    shake = fmaxf(shake, 5);
    sfx(S_BOOM, 0.8f);
    int prevCtx = explosiveKill;
    explosiveKill = byPlayer;
    if (dmg > 0)
        for (int i = 0; i < MAXE; i++) {
            Enemy *e = &en[i];
            if (!e->alive || e->state == -2) continue;
            float dx = e->x - x, dy = e->y - y, d = lenf(dx, dy);
            if (d < radius + e->hw) damageEnemy(e, dmg * (1 - 0.5f * d / (radius + e->hw)), dx / (d + 0.1f), dy / (d + 0.1f));
        }
    explosiveKill = prevCtx;
    if (!byPlayer && lenf(px - x, py - y) < radius * 0.85f) hurtPlayer(x, y, dmg > 0 ? (int)(dmg * 3) : 20);
}

static void laserBeam(float x, float y, float a)
{
    float dx = cosf(a), dy = sinf(a), ex = x, ey = y;
    for (int i = 0; i < 400; i++) {
        float nx = ex + dx * 2, ny = ey + dy * 2;
        if (nx < 0 || ny < 0 || nx >= FW || ny >= GROUND_Y) break;
        ex = nx; ey = ny;
    }
    float len = lenf(ex - x, ey - y);
    for (int i = 0; i < MAXE; i++) {
        Enemy *e = &en[i];
        if (!e->alive || e->state == -2) continue;
        float t = clampf((e->x - x) * dx + (e->y - y) * dy, 0, len);
        if (inBox(e, x + dx * t, y + dy * t, 2)) {
            for (int k = 0; k < 6; k++) addPart(e->x, e->y, frange(-80, 80), frange(-80, 40), 0.3f, C(140, 250, 255), 1, 200, 0);
            killW = W_LASER;
            damageEnemy(e, WEAP[W_LASER].dmg * dmgMul(), dx, dy); /* goes straight through riot shields */
            killW = -1;
        }
    }
    for (int i = 0; i < MAXBEAM; i++)
        if (beams[i].life <= 0) { beams[i].x0 = x; beams[i].y0 = y; beams[i].x1 = ex; beams[i].y1 = ey; beams[i].life = 0.12f; break; }
}

void shoulder(float *sx, float *sy) { *sx = px + facing * 1; *sy = py - 3; }

static void muzzlePos(float *mxp, float *myp)
{
    float sx, sy;
    shoulder(&sx, &sy);
    float reach = 5 + WEAP[curW].gunLen;
    *mxp = sx + cosf(aim) * reach; *myp = sy + sinf(aim) * reach;
}

void startReload(void)
{
    if (reloadT > 0 || mag[curW] >= magSize(curW)) return;
    reloadT = reloadMax = WEAP[curW].reload * (perk[PK_RELOAD] ? (perk[PK_RELOAD] == 1 ? 0.7f : perk[PK_RELOAD] == 2 ? 0.5f : 0.38f) : 1);
    sfx(S_RELOAD, 0.5f);
}

void fireWeapon(void)
{
    const Weapon *w = &WEAP[curW];
    float mxp, myp;
    if (reloadT > 0) return;
    if (mag[curW] <= 0) { startReload(); return; }
    muzzlePos(&mxp, &myp);
    if (curW == W_LASER) {
        laserBeam(mxp, myp, aim);
    } else {
        for (int p = 0; p < w->pellets; p++) {
            float a = aim + (w->pellets > 1 ? (p / (float)(w->pellets - 1) - 0.5f) * w->spread : 0) +
                      frange(-w->spread, w->spread) * (w->pellets > 1 ? 0.25f : 1);
            for (int i = 0; i < MAXS; i++)
                if (shots[i].life <= 0) {
                    float sp = w->speed * (w->pellets > 1 ? frange(0.85f, 1.1f) : 1);
                    Shot *s = &shots[i];
                    s->x = mxp; s->y = myp; s->vx = cosf(a) * sp; s->vy = sinf(a) * sp;
                    s->life = w->life * (w->pellets > 1 ? frange(0.8f, 1.15f) : 1);
                    s->dmg = w->dmg * dmgMul(); s->kind = curW == W_ROCKET ? 1 : curW == W_GRENADE ? 2 : curW == W_GRAVITY ? 3 : 0;
                    s->pierce = perk[PK_PIERCE]; s->lastHit = -1; s->weapon = curW;
                    break;
                }
        }
    }
    if (curW == W_PISTOL || curW == W_SMG || curW == W_SHOTGUN) /* spent shell casing */
        addPart(px, py - 3, -facing * frange(30, 70), frange(-140, -80), 1.2f, C(230, 190, 80), 1, 600, 1);
    fireCd = w->cd / (1 + 0.15f * perkPow[PK_RATE]);
    muzzleT = 0.05f;
    if (--mag[curW] <= 0) startReload(); /* empty: reload automatically */
    float kick = curW == W_SHOTGUN ? 70 : curW == W_ROCKET ? 60 : curW == W_LASER ? 40 : 0;
    if (ropeIdx < 0) pvx -= cosf(aim) * kick;
    if (curW >= W_SHOTGUN) shake = fmaxf(shake, 2.5f);
    sfx(w->snd, curW == W_SMG ? 0.3f : 0.45f);
}

void selectWeapon(int w)
{
    if (w < 0 || w >= W_COUNT || w == curW) return;
    curW = w;
    reloadT = 0; /* switching cancels a reload; each gun keeps its own magazine */
    fireCd = fmaxf(fireCd, 0.15f);
    sfx(S_SWITCH, 0.5f);
    if (mag[w] <= 0) startReload();
}

void cycleWeapon(int dir)
{
    selectWeapon((curW + dir + W_COUNT) % W_COUNT);
}

void givePickup(Pickup *p)
{
    char s[28];
    sfx(S_PICKUP, 0.7f);
    burst(p->x, p->y, 10, 80, C(140, 255, 160), 0.4f);
    addProgress(P_MEDKITS, 1);
    dailyAdd(DE_MEDKIT, 1);
    if (hp < maxHp) {
        int add = maxHp - hp < MEDKIT_HEAL ? maxHp - hp : MEDKIT_HEAL;
        hp += add;
        sfmt(s, sizeof s, "+%d HP", add);
        floater(p->x, p->y - 14, s, C(120, 255, 140));
    } else {
        score += 100;
        floater(p->x, p->y - 14, "+100", C(120, 255, 140));
    }
}
