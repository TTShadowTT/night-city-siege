/* gameplay_enemies.c - Enemy AI, including the giant bosses. */
#include "core.h"

static int findRopeUp(const Enemy *e)
{
    float feet = e->y + e->hh;
    int best = -1;
    float bestD = 110;
    for (int i = 0; i < D->nRope; i++) {
        const Rope *r = &D->rope[i];
        if (r->y0 > feet - 20) continue;            /* must lead somewhere higher */
        if (fabsf(r->y1 - feet) > 16 && !(feet > r->y0 && feet < r->y1)) continue; /* reachable from here */
        float d = fabsf(r->x - e->x);
        if (d < bestD) { bestD = d; best = i; }
    }
    return best;
}

static int platAbove(const Enemy *e)
{
    float feet = e->y + e->hh;
    for (int i = 0; i < D->nPlat; i++) {
        const Plat *p = &D->plat[i];
        if (p->y < feet - 12 && p->y > feet - 78 && e->x > p->x + 3 && e->x < p->x + p->w - 3) return 1;
    }
    return 0;
}

/* walk, drop, jump and climb toward the player */
static void groundChase(Enemy *e, float speed, float dt)
{
    float dx = px - e->x, dy = (py + PHH) - (e->y + e->hh);
    int dir = dx > 6 ? 1 : dx < -6 ? -1 : 0;
    if (e->rope >= 0) {
        const Rope *r = &D->rope[e->rope];
        e->x = r->x; e->vx = 0; e->vy = 0;
        int up = dy < -6 ? 1 : 0;
        e->y += (up ? -70.f : 70.f) * dt;
        if (e->y - e->hh < r->y0 - 2) {
            if (platAt(r->x, r->y0) >= 0) e->y = r->y0 - e->hh - 0.5f;
            e->rope = -1;
        } else if (e->y + e->hh >= r->y1) {
            e->y = r->y1 - e->hh;
            e->rope = -1;
        } else if (!up && dy < 10) {
            e->rope = -1; e->vy = -160; e->vx = (dir ? dir : 1) * speed;
        }
        return;
    }
    if (dy < -22) {
        int ri = findRopeUp(e);
        if (ri >= 0) {
            float rx = D->rope[ri].x;
            dir = rx > e->x + 2 ? 1 : rx < e->x - 2 ? -1 : 0;
            if (!dir && e->onGround) { e->rope = ri; e->x = rx; e->vx = e->vy = 0; return; }
        } else if (e->onGround && e->jumpCd <= 0 && platAbove(e)) {
            e->vy = -330; e->jumpCd = 1.0f;
        }
    } else if (dy > 22) {
        if (e->onGround && e->plat >= 0 && fabsf(dx) < 50) e->dropT = 0.25f;
        if (!dir) dir = e->facing;
    }
    if (e->onGround && e->jumpCd <= 0 && fabsf(e->vx) < 5 && dir && frand() < 0.02f) { e->vy = -300; e->jumpCd = 1.0f; }
    e->vx += (dir * speed - e->vx) * fminf(1, dt * 8);
    if (dir) e->facing = dir;
}

/* the big bosses: a giant armoured fighter that stomps after you, leaps and slams the ground,
   fires its arm cannon, has a signature attack and keeps calling in squads; angrier below half and a quarter health */
/* the big boss's arm cannon: shoulder, aim (kept in front of it) and muzzle */
float megaAim(const Enemy *e, float *sx, float *sy)
{
    *sx = e->x + e->facing * 13; *sy = e->y - 18;
    float a = atan2f(py - *sy, px - *sx);
    if (cosf(a) * e->facing < 0.2f) a = e->facing > 0 ? (sinf(a) > 0 ? 1.37f : -1.37f) : (sinf(a) > 0 ? PI - 1.37f : -PI + 1.37f);
    return a;
}
void megaGun(const Enemy *e, float *gx, float *gy)
{
    float sx, sy, a = megaAim(e, &sx, &sy);
    *gx = sx + cosf(a) * 26; *gy = sy + sinf(a) * 26;
}

static void updateMega(Enemy *e, float dx, float dy, float dt)
{
    int v = MEGA_STYLE[e->mega - 1];
    float rage = e->hp < e->maxhp * 0.25f ? 0.55f : e->hp < e->maxhp * 0.5f ? 0.75f : 1;
    int grounded = e->y >= GROUND_Y - e->hh - 0.5f;
    (void)dy;
    if (!grounded) e->vy += 900 * dt;
    else {
        if (!e->onGround) { /* landing: the ground shakes and a shockwave runs along it */
            shake = fmaxf(shake, 12);
            explode(e->x, GROUND_Y - 4, 44, 0, 0);
            for (int s = -1; s <= 1; s += 2)
                for (int k = 0; k < 3; k++) spawnEShot(e->x + s * 20, GROUND_Y - 5, s * (120 + k * 45.f), 0, 0);
        }
        e->vy = 0;
        float want = fabsf(dx) > 80 ? (dx > 0 ? 1.f : -1.f) : 0;
        e->vx += (want * e->speed / rage - e->vx) * fminf(1, dt * 4);
    }
    e->onGround = grounded;
    e->facing = dx > 0 ? 1 : -1;
    e->ax -= dt; /* leap at you */
    if (grounded && e->ax <= 0) {
        e->vy = -440; e->vx = clampf(dx * 1.2f, -260, 260);
        e->ax = frange(4.5f, 6.5f) * rage;
        sfx(S_DASH, 0.8f);
    }
    float gx, gy;
    megaGun(e, &gx, &gy);
    e->timer -= dt; /* aimed spread from the arm cannon */
    if (e->timer <= 0) {
        float a = atan2f(py - gy, px - gx);
        for (int j = -2; j <= 2; j++) spawnEShot(gx, gy, cosf(a + j * 0.13f) * 220, sinf(a + j * 0.13f) * 220, 0);
        e->timer = 1.7f * rage;
        sfx(S_ESHOT, 0.8f);
    }
    e->timer2 -= dt; /* the signature attack */
    if (v == 1 && e->anchorY > 0) { /* a spinning spiral of bullets */
        e->anchorY -= dt;
        e->ay += dt * 2.6f;
        e->jumpCd -= dt;
        if (e->jumpCd <= 0) {
            for (int k = 0; k < 3; k++) { float a = e->ay + k * 2 * PI / 3; spawnEShot(e->x, e->y - 20, cosf(a) * 120, sinf(a) * 120, 0); }
            e->jumpCd = 0.13f;
        }
    } else if (e->timer2 <= 0) {
        if (v == 0) { /* a ring of bullets from the chest */
            for (int k = 0; k < 16; k++) { float a = k * PI / 8 + gt; spawnEShot(e->x, e->y - 20, cosf(a) * 130, sinf(a) * 130, 0); }
            sfx(S_BOOM, 0.5f);
        } else if (v == 1) {
            e->anchorY = 2.6f; e->jumpCd = 0;
            sfx(S_WARP, 0.7f);
        } else { /* homing missiles from the shoulder pods */
            for (int j = 0; j < 4; j++) spawnEShot(e->x - 18 + j * 12, e->y - 30, (j - 1.5f) * 50, -90, 2);
            sfx(S_ROCKET, 0.8f);
        }
        e->timer2 = (v == 1 ? 4.8f : 4.0f) * rage;
    }
    e->timer3 -= dt; /* calls in a squad: ropes drop from the top of the screen */
    if (e->timer3 <= 0) {
        for (int j = 0; j < (rage < 1 ? 4 : 3); j++) {
            int t = pickType();
            if (t == EN_DRONE || t == EN_JET) spawnEnemy(t, frange(30, FW - 30), 0);
            else spawnEnemy(t, clampf(e->x + (j - 1.5f) * 60.f + frange(-10, 10), 16, FW - 16), 0);
        }
        announce("REINFORCEMENTS!", MEGA_COL[e->mega - 1]);
        e->timer3 = 9.0f * rage;
    }
}

void updateEnemies(float dt)
{
    int L = level();
    bossIdx = -1;
    for (int i = 0; i < MAXE; i++) {
        Enemy *e = &en[i];
        if (!e->alive) continue;
        if (e->type == EN_BOSS) bossIdx = i;
        if (e->flash > 0) e->flash -= dt;
        e->anim += dt;

        /* arrival */
        if (e->state == -2) { /* rope still unrolling from the top of the screen */
            e->timer3 -= dt;
            if (e->timer3 <= 0) e->state = -1;
            continue;
        }
        if (e->state == -1) {
            if (e->type == EN_BOSS) {
                e->y += (e->mega ? 260 : 40) * dt;
                if (e->y >= e->targetY) {
                    e->y = e->targetY; e->state = 0;
                    if (e->mega) { e->onGround = 1; shake = fmaxf(shake, 16); explode(e->x, GROUND_Y - 4, 50, 0, 0); sfx(S_BOSS, 1.0f); }
                }
                continue;
            }
            e->y += 120 * dt;
            if (e->y >= e->targetY) {
                e->y = e->targetY; e->state = 0; e->onGround = 1;
                addRopeFx(e->x, e->anchorY, e->targetY - e->hh);
            }
            continue;
        }

        float dx = px - e->x, dy = py - e->y, d = lenf(dx, dy) + 0.001f, nx = dx / d, ny = dy / d;
        int flying = e->type == EN_DRONE || e->type == EN_BOSS || e->type == EN_JET || e->hunt;
        if (e->jumpCd > 0) e->jumpCd -= dt;
        if (e->dropT > 0) e->dropT -= dt;

        switch (e->type) {
        case EN_THUG:
            groundChase(e, e->speed * (0.9f + 0.1f * sinf(gt * 3 + e->phase)), dt);
            break;
        case EN_DRONE: {
            float wob = sinf(gt * 4 + e->phase);
            e->timer -= dt;
            if (e->state == 0) {
                float tx = nx * e->speed - ny * wob * 30, ty = ny * e->speed + nx * wob * 30 - 20;
                e->vx += (tx - e->vx) * fminf(1, dt * 3); e->vy += (ty - e->vy) * fminf(1, dt * 3);
                if (e->timer <= 0 && d < 160) { e->state = 1; e->timer = 0.5f; e->ax = nx; e->ay = ny; sfx(S_DASH, 0.3f); }
            } else {
                e->vx += (e->ax * e->speed * 2.6f - e->vx) * fminf(1, dt * 10);
                e->vy += (e->ay * e->speed * 2.6f - e->vy) * fminf(1, dt * 10);
                if (e->timer <= 0) { e->state = 0; e->timer = frange(2, 3.5f); }
            }
        } break;
        case EN_GUNNER: {
            int level_ = fabsf(dy) < 30;
            if (fabsf(dx) > 140 || !level_) groundChase(e, e->speed, dt);
            else { e->vx *= 0.85f; e->facing = dx > 0 ? 1 : -1; }
            e->timer -= dt;
            e->state = e->timer < 0.45f;
            if (e->timer <= 0) {
                if (d < 260) {
                    float s = fminf(250, 150 + L * 3.0f);
                    float a = atan2f(py - (e->y - 3), px - e->x);
                    spawnEShot(e->x + cosf(a) * 9, e->y - 3 + sinf(a) * 9, cosf(a) * s, sinf(a) * s, 0);
                    if (L >= 10 || e->elite) {
                        spawnEShot(e->x, e->y - 3, cosf(a - 0.18f) * s, sinf(a - 0.18f) * s, 0);
                        spawnEShot(e->x, e->y - 3, cosf(a + 0.18f) * s, sinf(a + 0.18f) * s, 0);
                    }
                    sfx(S_ESHOT, 0.4f);
                }
                e->timer = frange(1.6f, 2.6f);
            }
        } break;
        case EN_BOMBER:
            if (e->state == 0) {
                groundChase(e, e->speed, dt);
                if (fabsf(dx) < 14 && fabsf(dy) < 18) { e->state = 1; e->timer = 0.5f; }
            } else {
                e->vx *= 0.8f;
                e->timer -= dt;
                if (e->timer <= 0) { e->alive = 0; explode(e->x, e->y, 30, 8, 0); continue; }
            }
            break;
        case EN_BRUTE:
            if (e->state == 0) {
                groundChase(e, e->speed, dt);
                e->timer -= dt;
                if (e->timer <= 0 && fabsf(dy) < 18 && fabsf(dx) < 170 && e->onGround) { e->state = 1; e->timer = 0.6f; e->ax = dx > 0 ? 1.f : -1.f; e->facing = (int)e->ax; }
            } else if (e->state == 1) {
                e->vx *= 0.8f;
                e->timer -= dt;
                if (e->timer <= 0) { e->state = 2; e->timer = 0.75f; }
            } else if (e->state == 2) {
                e->vx = e->ax * 220;
                e->timer -= dt;
                if ((int)(gt * 30) % 2) addPart(e->x - e->ax * 6, e->y + e->hh, -e->ax * 30, -20, 0.3f, C(150, 140, 130), 1, 0, 0);
                if (e->timer <= 0 || (e->x <= e->hw + 1 || e->x >= FW - e->hw - 1)) { e->state = 3; e->timer = 0.9f; shake = fmaxf(shake, 2); }
            } else {
                e->vx *= 0.85f;
                e->timer -= dt;
                if (e->timer <= 0) { e->state = 0; e->timer = 1.4f; }
            }
            break;
        case EN_SHIELD: /* advances behind a riot shield; lunges with it, then has to recover with it lowered */
            if (e->timer2 > 0) e->timer2 -= dt; /* shield flash after a block */
            if (e->state == 0) {
                groundChase(e, e->speed, dt);
                if (fabsf(dy) < 30) e->facing = dx > 0 ? 1 : -1; /* keep the shield toward you */
                e->timer -= dt;
                if (e->timer <= 0 && fabsf(dy) < 18 && fabsf(dx) < 70 && e->onGround) { e->state = 1; e->timer = 0.35f; e->ax = dx > 0 ? 1.f : -1.f; e->facing = (int)e->ax; }
            } else if (e->state == 1) {
                e->vx *= 0.7f;
                e->timer -= dt;
                if (e->timer <= 0) { e->state = 2; e->timer = 0.3f; sfx(S_DASH, 0.4f); }
            } else if (e->state == 2) {
                e->vx = e->ax * 210;
                e->timer -= dt;
                if (e->timer <= 0) { e->state = 3; e->timer = 0.8f; }
            } else {
                e->vx *= 0.8f;
                e->timer -= dt;
                if (e->timer <= 0) { e->state = 0; e->timer = frange(1.6f, 2.6f); }
            }
            break;
        case EN_NINJA: /* runs at you, then blinks to your side and slashes */
            e->timer -= dt;
            if (e->state == 0) {
                groundChase(e, e->speed, dt);
                if (e->timer <= 0 && d < 230) {
                    float side = px > FW / 2.f ? -1.f : 1.f, tx = clampf(px + side * 30, 10, FW - 10);
                    int plat;
                    burst(e->x, e->y, 12, 90, C(60, 230, 255), 0.35f);
                    e->x = tx; e->y = surfaceBelow(tx, py - 6, &plat) - e->hh; e->plat = plat; e->vx = e->vy = 0;
                    burst(e->x, e->y, 12, 90, C(60, 230, 255), 0.35f);
                    e->state = 1; e->timer = 0.35f; e->facing = px > e->x ? 1 : -1; e->ax = (float)e->facing;
                    sfx(S_WARP, 0.4f);
                }
            } else if (e->state == 1) { /* wind-up: a moment to react */
                e->vx *= 0.7f;
                if (e->timer <= 0) { e->state = 2; e->timer = 0.22f; sfx(S_DASH, 0.4f); }
            } else if (e->state == 2) {
                e->vx = e->ax * 290;
                if (e->timer <= 0) { e->state = 0; e->timer = frange(2.5f, 4); }
            }
            break;
        case EN_JET: { /* hovers off to one side above you and fires short bursts */
            float tx = clampf(px + e->ax * 110, 20, FW - 20), ty = clampf(py - 70 + sinf(gt * 2 + e->phase) * 12, 20, GROUND_Y - 40);
            e->vx += ((tx - e->x) * 1.5f - e->vx) * fminf(1, dt * 2);
            e->vy += ((ty - e->y) * 1.5f - e->vy) * fminf(1, dt * 2);
            e->vx = clampf(e->vx, -e->speed, e->speed); e->vy = clampf(e->vy, -e->speed, e->speed);
            e->facing = dx > 0 ? 1 : -1;
            e->timer -= dt;
            if (e->timer <= 0) {
                if (e->state < 3 && d < 280) {
                    float a = atan2f(dy, dx) + frange(-0.06f, 0.06f), s = fminf(240, 170 + L * 2.0f);
                    spawnEShot(e->x, e->y, cosf(a) * s, sinf(a) * s, 0);
                    sfx(S_ESHOT, 0.3f);
                    e->state++; e->timer = 0.14f;
                } else { e->state = 0; e->timer = frange(1.8f, 2.6f); if (frand() < 0.3f) e->ax = -e->ax; }
            }
            if ((int)(gt * 30) & 1) addPart(e->x - e->facing * 4, e->y + 8, 0, 60, 0.2f, C(255, 160, 40), 1, 0, 0);
        } break;
        case EN_MECH: /* slow walker: homing missiles from its pods, a stomp when you get close */
            groundChase(e, e->speed, dt);
            e->timer -= dt;
            if (e->timer <= 0) {
                spawnEShot(e->x - 8, e->y - 4, -40, -90, 2);
                spawnEShot(e->x + 8, e->y - 4, 40, -90, 2);
                sfx(S_ROCKET, 0.5f);
                e->timer = frange(3.5f, 4.5f);
            }
            e->timer2 -= dt;
            if (e->timer2 <= 0 && d < 34 && e->onGround) {
                explode(e->x, e->y + e->hh - 2, 28, 0, 0);
                e->timer2 = 2.2f;
            }
            break;
        case EN_BOSS: {
            if (e->mega) { updateMega(e, dx, dy, dt); break; }
            int enraged = e->hp < e->maxhp * 0.5f;
            /* hover off to one side of the player rather than straight overhead */
            float tx = clampf(px + (sinf(gt * 0.45f + e->phase) > 0 ? 95.f : -95.f), 50, FW - 50);
            e->vx += ((tx - e->x) * 1.2f - e->vx) * fminf(1, dt * 1.5f);
            e->vx = clampf(e->vx, -e->speed, e->speed);
            e->vy = (46 + sinf(gt * 1.3f) * 6 - e->y) * 2;
            e->facing = dx > 0 ? 1 : -1;
            e->timer -= dt; /* aimed burst */
            if (e->timer <= 0) {
                float gx = e->x + e->facing * 14, gy = e->y + 8, a = atan2f(py - gy, px - gx);
                for (int j = -1; j <= 1; j++) spawnEShot(gx, gy, cosf(a + j * 0.12f) * 210, sinf(a + j * 0.12f) * 210, 0);
                e->timer = enraged ? 1.2f : 1.8f;
                sfx(S_ESHOT, 0.7f);
            }
            e->timer2 -= dt; /* carpet bombs */
            if (e->timer2 <= 0) {
                for (int j = 0; j < 4; j++) spawnEShot(e->x - 18 + j * 12, e->y + 8, e->vx * 0.5f, 20, 1);
                e->timer2 = enraged ? 4.0f : 5.5f;
            }
            e->timer3 -= dt; /* reinforcements rappel from the hull */
            if (e->timer3 <= 0) {
                int sig = D->signature == EN_DRONE ? EN_THUG : D->signature;
                for (int j = 0; j < (enraged ? 2 : 1); j++) spawnEnemy(j ? EN_THUG : sig, e->x + (j ? 12.f : -12.f), e->y + 10);
                e->timer3 = enraged ? 6.0f : 8.0f;
            }
            if (enraged) { /* homing missiles */
                e->ax -= dt;
                if (e->ax <= 0) {
                    spawnEShot(e->x - 20, e->y + 4, -60, 40, 2);
                    spawnEShot(e->x + 20, e->y + 4, 60, 40, 2);
                    e->ax = 3.2f;
                    sfx(S_ROCKET, 0.6f);
                }
            }
        } break;
        }

        if (e->hunt && e->type != EN_DRONE) { /* end-of-wave straggler: fly straight in */
            e->vx += (nx * fmaxf(e->speed, 60) - e->vx) * fminf(1, dt * 4);
            e->vy += (ny * fmaxf(e->speed, 60) - e->vy) * fminf(1, dt * 4);
        }
        if (flying) {
            e->x = clampf(e->x + e->vx * dt, e->hw, FW - e->hw);
            e->y = clampf(e->y + e->vy * dt, e->hh, GROUND_Y - e->hh);
        } else if (e->rope < 0) {
            e->onGround = bodyStep(&e->x, &e->y, &e->vx, &e->vy, e->hw, e->hh, &e->plat, e->dropT, 1, dt);
        }

        if (e->type != EN_BOMBER && (e->type != EN_BOSS || e->mega) &&
            fabsf(px - e->x) < e->hw + PHW - 1 && fabsf(py - e->y) < e->hh + PHH - 1) {
            static const int contactDmg[EN_TYPES] = { 12, 10, 10, 0, 20, 10, 0, 14, 8, 22 };
            hurtPlayer(e->x, e->y, e->mega ? 25 : contactDmg[e->type] + (e->type == EN_SHIELD && e->state == 2 ? 8 : 0));
        }
    }

    /* keep crowds from stacking into one pixel */
    for (int i = 0; i < MAXE; i++) {
        Enemy *a = &en[i];
        if (!a->alive || a->state < 0 || a->type == EN_BOSS) continue;
        for (int j = i + 1; j < MAXE; j++) {
            Enemy *b = &en[j];
            if (!b->alive || b->state < 0 || b->type == EN_BOSS) continue;
            float dx = b->x - a->x, rr = a->hw + b->hw - 1;
            if (fabsf(dx) >= rr || fabsf(b->y - a->y) > a->hh + b->hh - 2) continue;
            float push = (rr - fabsf(dx)) * 0.5f * (dx >= 0 ? 1 : -1);
            a->x = clampf(a->x - push, a->hw, FW - a->hw);
            b->x = clampf(b->x + push, b->hw, FW - b->hw);
        }
    }
}
