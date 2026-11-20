/* gameplay_update.c - One game step: pickups, waves, upgrade cards, music choice and the pet. */
#include "core.h"

static void updatePickups(float dt)
{
    for (int i = 0; i < MAXPU; i++) {
        Pickup *p = &pu[i];
        if (p->life <= 0) continue;
        p->life -= dt;
        if (!p->landed) {
            float vx = 0, vy = p->chute ? 40 : p->vy;
            if (!p->chute) p->vy = fminf(p->vy + GRAV * gravK * dt, 300);
            if (bodyStep(&p->x, &p->y, &vx, &vy, 5, 5, &p->plat, p->vy < 0 ? 1.f : 0.f, 0, dt)) { p->landed = 1; p->chute = 0; }
        }
        if (fabsf(p->x - px) < PHW + 6 && fabsf(p->y - py) < PHH + 6) { givePickup(p); p->life = 0; }
    }
}

static void updateWaves(float dt)
{
    if (interT > 0) {
        interT -= dt;
        if (interT <= 0) {
            if (areaClear) {
                areaClear = 0;
                area++;
                waveIdx = 0;
                memset(eshots, 0, sizeof eshots); memset(shots, 0, sizeof shots); memset(pu, 0, sizeof pu);
                loadDistrict(area % D_COUNT);
                resetPlayer();
                hp = hp + 30 > maxHp ? maxHp : hp + 30;
                if (!(mapsMask & (1 << curMap))) { /* reaching a district in the campaign unlocks it */
                    char s[64];
                    mapsMask |= 1 << curMap;
                    sfmt(s, sizeof s, "DISTRICT UNLOCKED: %s", D->name);
                    toast(s);
                }
                saveGame();
            }
            startWave();
        }
        return;
    }
    int alive = countAlive();
    supplyT -= dt;
    if (supplyT <= 0) { dropPickup(frange(30, FW - 30), 0, 0, 1); supplyT = 25; }
    if (toSpawn > 0) {
        spawnCd -= dt;
        if (spawnCd <= 0 && alive < 8 + level()) {
            spawnFromQueue();
            toSpawn--;
            spawnCd = fmaxf(0.35f, 1.1f - level() * 0.04f) * frange(0.6f, 1.2f) * (waveKind == WK_HORDE ? 0.6f : 1);
        }
        stallT = 0;
    } else if (alive > 0) {
        /* safety net: if nothing has died for a while, the last few come straight for you */
        if (totalKills != stallKills) { stallKills = totalKills; stallT = 0; }
        stallT += dt;
        if (stallT > 15)
            for (int i = 0; i < MAXE; i++) if (en[i].alive && en[i].type != EN_BOSS && en[i].state >= 0) en[i].hunt = 1;
    } else {
        char s[28];
        int bonus = 50 * level();
        stallT = 0;
        score += bonus;
        addCoins(3 + level() / 2);
        sfmt(s, sizeof s, "BONUS +%d", bonus);
        floater(FW / 2.f, FH / 2.f + 10, s, C(140, 255, 140));
        sfx(S_PICKUP, 0.6f);
        if (mode == MODE_ENDLESS) setBest(P_ENDLESS, waveIdx);
        if (mode == MODE_CAMPAIGN && waveIdx >= 5) addProgress(P_DISTRICTS, 1);
        perkPending = perksOn && perksTaken < perkCap();
        addProgress(P_WAVES, 1);
        dailyAdd(DE_WAVE, 1);
        if (!waveHurt) dailyAdd(DE_FLAWLESS, 1);
        if (!waveHurt) achGive(ACH_UNTOUCHABLE);
        if (hp <= 10) achGive(ACH_CLOSE_CALL);
        if (mode == MODE_ENDLESS && waveIdx >= 40) achGive(ACH_ENDLESS40);
        if (mode == MODE_ENDLESS) dailyAdd(DE_ENDLESS, waveIdx);
        if (mode == MODE_CAMPAIGN && waveIdx >= 5) dailyAdd(DE_DISTRICT, 1);
        saveGame();
        if (mode == MODE_CAMPAIGN && waveIdx >= 5) {
            areaClear = 1;
            if (hardOn) achGive(ACH_HARD);
            if (curMap == D_COUNT - 1 && !hardUnlocked) { /* the last district of the campaign: HARD MODE opens up */
                hardUnlocked = 1;
                toast("HARD MODE UNLOCKED - TURN IT ON IN OPTIONS");
                saveGame();
            }
            interT = 4.0f;
            sfmt(banner, sizeof banner, "DISTRICT CLEAR");
            sfmt(banner2, sizeof banner2, "NEXT: %s", DISTRICTS[(area + 1) % D_COUNT].name);
        } else {
            interT = 3.0f;
            sfmt(banner, sizeof banner, "WAVE %d CLEAR", waveIdx);
            sfmt(banner2, sizeof banner2, "SUPPLY DROP INCOMING");
            dropPickup(frange(60, FW - 60), 0, 0, 1);
        }
        bannerT = 2.4f;
    }
}

/* deal three upgrade cards (fewer once most upgrades are maxed out) */
static void openPerks(void)
{
    int avail[PK_COUNT], n = 0;
    perkPending = 0;
    for (int i = 0; i < PK_COUNT; i++) if (perk[i] < PERKS[i].maxStack || (!perkLimitOn && (PERK_ENDLESS >> i & 1))) avail[n++] = i;
    for (int c = 0; c < 3; c++) {
        perkChoice[c] = -1;
        if (!n) continue;
        int k = irand(n);
        perkChoice[c] = avail[k];
        avail[k] = avail[--n];
    }
    if (perkChoice[0] < 0) return; /* everything maxed */
    state = ST_PERK;
    focusBtn = 1;
    mdown = 0;
    perkOpenT = gt;
    sfx(S_PERK, 0.6f);
}

void takePerk(int c)
{
    char s[40];
    int id = perkChoice[c];
    if (id < 0 || (!bot && gt - perkOpenT < 0.4f)) return;
    perkPow[id] += perkWeight(perksTaken);
    perk[id]++;
    perksTaken++;
    addProgress(P_PERKS, 1);
    if (perksTaken >= 10) achGive(ACH_OVERCLOCK);
    dailyAdd(DE_PERK, 1);
    if (id == PK_HP) { int add = (int)(25 * perkWeight(perksTaken - 1) + 0.5f); maxHp += add; hp = hp + add > maxHp ? maxHp : hp + add; }
    if (id == PK_MAG) for (int w = 0; w < W_COUNT; w++) mag[w] = magSize(w);
    sfmt(s, sizeof s, "%s", PERKS[id].name);
    floater(px, py - 24, s, PERKS[id].col);
    if (perksTaken == perkCap()) floater(px, py - 34, "ALL UPGRADE SLOTS FULL", C(255, 214, 80));
    sfx(S_PICKUP, 0.7f);
    state = ST_PLAY;
    saveGame();
}

static void updateMusic(void)
{
    float v = 0;
    if (musicOn && !bot)
        switch (state) {
        case ST_PLAY: v = 0.55f; break;
        case ST_PERK: v = 0.45f; break;
        case ST_PAUSE: v = 0.22f; break;
        case ST_OVER: v = 0.3f; break;
        case ST_QUIT: v = 0; break;
        case ST_OPTIONS: case ST_UPGRADES: v = optFromPause ? 0.22f : 0.5f; break;
        default: v = 0.5f; break;
        }
    v *= musicVolPct / 100.f;
    if (v != musicSent) { musicSent = v; plat_music(v); }
    { /* which song: the district you're in (or the one behind the menu), a random one, or the one you picked */
        int inGame = state == ST_PLAY || state == ST_PAUSE || state == ST_PERK || state == ST_OVER || ((state == ST_OPTIONS || state == ST_UPGRADES) && optFromPause);
        int m = clampi(inGame ? curMap : menuMusMap, 0, D_COUNT - 1);
        if (m != musFor) {
            musFor = m;
            int all = MUS_TRACKS + myCount;
            if (musicMode == 0) { /* the district's own song - or one of MY SONGS tied to it */
                musTrack = MAP_TRACK[m];
                for (int i = 0; i < myCount; i++) if (myMap[i] == m) musTrack = MUS_TRACKS + i;
            } else if (musicMode == 1) { int t = irand(all - 1); musTrack = t >= musTrack ? t + 1 : t; } /* never the same one twice */
            else musTrack = clampi(musicMode - 2, 0, all - 1);
        }
    }
}

/* the pet trots after you: it walks to a spot just behind you, jumps up to the platform you're on, drops down
   after you, and pops back to your side if it gets left far behind. It never gets hurt. */
static void updatePet(float dt)
{
    static float hopT = 2;
    if (!pet) return;
    float feet = py + PHH, tx = px - facing * 16, dx = tx - petX;
    if (fabsf(px - petX) > 160 || fabsf(feet - (petY + 4)) > 150) { /* left behind: poof, it's back */
        for (int k = 0; k < 8; k++) addPart(tx, feet - 4, frange(-60, 60), frange(-90, -20), 0.4f, C(255, 255, 255), 1, 200, 0);
        petX = tx; petY = onGround ? feet - 4 : petY; petVx = petVy = 0;
    }
    if (petGoo(pet) && frand() < dt * 7) /* acid drips */
        addPart(petX + frange(-6, 6), petY + 2, frange(-8, 8), frange(-20, 0), 0.8f, frand() < 0.5f ? C(60, 255, 30) : C(20, 170, 20), 1, 400, 0);
    if (petGoo(pet) && frand() < dt * 2) /* toxic bubbles float up */
        addPart(petX + frange(-4, 4), petY - 6, frange(-6, 6), frange(-30, -15), 0.7f, C(200, 255, 110), 1, 0, 0);
    if (petFlies(pet)) { /* flyers hover by your shoulder, bobbing, and swoop after you */
        float ty = py - 14 + sinf(gt * 3) * 3, k = fminf(1, dt * 5);
        petVx = (tx - petX) * 5;
        petX += (tx - petX) * k; petY += (ty - petY) * k;
        petFace = fabsf(petVx) > 10 ? (petVx > 0 ? 1 : -1) : px > petX ? 1 : -1;
        petGround = 0; petVy = 0;
        petWalk += dt * 12; /* wing beats */
        return;
    }
    float goal = fabsf(dx) < 4 ? 0 : fmaxf(-190, fminf(190, dx * 6));
    petVx += (goal - petVx) * fminf(1, dt * 10);
    if (fabsf(petVx) > 6) petFace = petVx > 0 ? 1 : -1;
    else petFace = px > petX ? 1 : -1;
    if (petGround && onGround && feet < petY + 4 - 12 && fabsf(px - petX) < 70) /* you're up on a platform: jump after you */
        petVy = -sqrtf(2 * GRAV * gravK * (petY + 4 - feet + 12));
    else if (petGround && fabsf(petVx) < 6 && (hopT -= dt) <= 0) { petVy = -110; hopT = 2.5f + frand() * 3; } /* idle: a happy little hop */
    int drop = onGround && feet > petY + 4 + 12; /* you went down: follow through the platform */
    petGround = bodyStep(&petX, &petY, &petVx, &petVy, 3, 4, &petPlat, drop ? 1.f : 0.f, 1, dt);
    petWalk += fabsf(petVx) * dt * 0.15f;
}

void update(float dt)
{
    static float dayCheckT;
    gt += dt;
    if (toastT > 0) toastT -= dt;
    updateMusic();
    if (curMap == D_STORM && state != ST_PAUSE) { /* thunder rolls in with each lightning strike */
        static int lastWin = -1;
        int win;
        float age = strikeAge(&win, 0);
        if (age >= 0 && win != lastWin) { lastWin = win; sfx(S_BOOM, 0.35f); }
    }
    if ((dayCheckT -= dt) <= 0) { dayCheckT = 5; refreshDaily(); } /* past midnight: new daily quests */
    extraTick(dt);
    if (state == ST_PERK && bot) takePerk(irand(perkChoice[2] >= 0 ? 3 : perkChoice[1] >= 0 ? 2 : 1));
    if (state == ST_PAUSE || state == ST_PERK || ((state == ST_OPTIONS || state == ST_UPGRADES) && optFromPause)) return;
    if (state == ST_PLAY && hitStop > 0) { hitStop -= dt; return; } /* a few frozen frames sell a big hit */
    if (calloutT > 0) calloutT -= dt;
    if (streakT > 0) streakT -= dt;
    if (state == ST_PLAY && perkPending && bannerT < 1.3f && interT > 0) openPerks();
    for (int i = 0; i < MAXP; i++) {
        Part *p = &parts[i];
        if (p->life <= 0) continue;
        float oy_ = p->y;
        p->vy += p->grav * dt;
        p->x += p->vx * dt; p->y += p->vy * dt;
        p->vx *= 1 - dt * 1.5f;
        if (p->bounce && p->vy > 0) {
            float surf = surfaceBelow(p->x, oy_ - 1, 0);
            if (oy_ <= surf && p->y >= surf) {
                if (p->blood) { /* splat: stain the ledge / street it landed on, for the rest of this district */
                    int bx = (int)p->x, by = (int)surf;
                    for (int k = 0; k < p->sz + 1; k++)
                        for (int j = 0; j < 2; j++) {
                            int sx = bx + k - p->sz / 2, sy = by + j;
                            if ((unsigned)sx < FW && (unsigned)sy < FH && (foreL[sy * FW + sx] >> 24))
                                foreL[sy * FW + sx] = mixc(foreL[sy * FW + sx], p->c, j ? 0.45f : 0.75f);
                        }
                    p->life = 0;
                    continue;
                }
                p->y = surf - 1; p->vy *= -0.35f; p->vx *= 0.6f;
            }
        }
        p->life -= dt;
    }
    for (int i = 0; i < MAXF; i++) if (fl[i].life > 0) { fl[i].life -= dt; fl[i].y -= 20 * dt; }
    for (int i = 0; i < MAXBOOM; i++) if (booms[i].life > 0) booms[i].life -= dt;
    for (int i = 0; i < MAXBEAM; i++) if (beams[i].life > 0) beams[i].life -= dt;
    for (int i = 0; i < MAXRFX; i++) if (ropeFx[i].life > 0) ropeFx[i].life -= dt;
    if (bannerT > 0) bannerT -= dt;
    if (bossCardT > 0) bossCardT -= dt;
    for (int i = 0; i < MAXE; i++) if (en[i].alive && en[i].dmgT > 0 && (en[i].dmgT -= dt) <= 0) dmgFlush(&en[i]);
    if (state == ST_PLAY) {
        updatePlayer(dt);
        updatePet(dt);
        updateShots(dt);
        updateEnemies(dt);
        if (state == ST_PLAY) {
            updateHoles(dt);
            updatePickups(dt);
            updateWaves(dt);
            dailyAdd(DE_SCORE, score);
            if (comboT > 0) { comboT -= dt; if (comboT <= 0) combo = 0; }
        }
    } else if (state == ST_OVER) overT += dt;
    shake -= shake * fminf(1, dt * 10);
    ox = (int)roundf((frand() * 2 - 1) * shake * 0.5f);
    oy = (int)roundf((frand() * 2 - 1) * shake * 0.5f);
    if (!shakeOn) ox = oy = 0;
}
