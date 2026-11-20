/* gameplay_player.c - The player, gravity-gun holes and every kind of shot. */
#include "core.h"

static void botInput(float *ix)
{
    float best = 1e9f, tx = FW / 2.f, ty = py;
    for (int i = 0; i < MAXE; i++)
        if (en[i].alive && en[i].state >= -1 && en[i].y > 0) {
            float d = lenf(en[i].x - px, en[i].y - py);
            if (d < best) { best = d; tx = en[i].x; ty = en[i].y; }
        }
    mx = (int)tx; my = (int)ty; mdown = best < 1e8f;
    float want = tx > px ? tx - 110 : tx + 110;
    for (int i = 0; i < MAXPU; i++)
        if (pu[i].life > 0 && pu[i].landed && best > 70) { want = pu[i].x; break; }
    *ix = want > px + 8 ? 1.f : want < px - 8 ? -1.f : 0.f;
    if (onGround && (frand() < 0.015f || (best < 50 && frand() < 0.08f))) jumpPress = 1;
    if (ty > py + 40 && onGround && onPlat >= 0 && frand() < 0.03f) downPress = 1;
    if (best < 26 && frand() < 0.1f) dashReq = 1;
    /* react like a player would: hop or dash when something is about to hit */
    for (int i = 0; i < MAXES; i++) {
        EShot *b = &eshots[i];
        if (b->life <= 0) continue;
        float rx = px - b->x, ry = py - b->y;
        if (rx * rx + ry * ry < 45 * 45 && rx * b->vx + ry * b->vy > 0 && frand() < 0.25f) {
            if (onGround) jumpPress = 1; else dashReq = 1;
            break;
        }
    }
    if (bossIdx >= 0 && en[bossIdx].alive && en[bossIdx].state == 0 && en[bossIdx].timer < 0.25f && onGround && frand() < 0.3f) jumpPress = 1;
    botWeaponT -= DT;
    if (botWeaponT <= 0) {
        int w = irand(W_COUNT);
        selectWeapon(w);
        botWeaponT = 4;
    }
    jumpHeld = 1;
}

void updatePlayer(float dt)
{
    float ix = 0;
    if (bot) botInput(&ix);
    else ix = (float)(held[K_RIGHT] - held[K_LEFT]);
    aim = atan2f(my - (py - 3), mx - px);
    facing = cosf(aim) >= 0 ? 1 : -1;

    /* grab a rope */
    if (ropeIdx < 0 && dashT <= 0 && (held[K_UP] || held[K_DOWN] || (upPress && !onGround))) {
        for (int i = 0; i < D->nRope; i++) {
            const Rope *r = &D->rope[i];
            if (fabsf(px - r->x) < 6 && py > r->y0 - 2 && py - PHH < r->y1 - 4) {
                if (held[K_DOWN] && onGround && py + PHH >= r->y1 - 2) continue; /* standing at its foot */
                if (held[K_UP] && !held[K_DOWN] && py - PHH <= r->y0 + 1) continue; /* already at the top */
                ropeIdx = i; px = r->x; pvx = pvy = 0; jumps = 1 + perk[PK_JUMP]; airDash = 1; upPress = 0;
                addProgress(P_ROPES, 1);
                dailyAdd(DE_ROPE, 1);
                sfx(S_ROPE, 0.3f);
                break;
            }
        }
    }

    if (ropeIdx >= 0) {
        const Rope *r = &D->rope[ropeIdx];
        float climb = (float)(held[K_DOWN] - held[K_UP]) * 90;
        py += climb * dt;
        if (climb) runAnim += dt * 8;
        if (py - PHH < r->y0 - 1) {
            if (platAt(r->x, r->y0) >= 0) { py = r->y0 - PHH - 0.5f; ropeIdx = -1; pvy = 0; onGround = 1; }
            else py = r->y0 + PHH - 1;
        }
        if (ropeIdx >= 0 && py + PHH >= r->y1) { py = r->y1 - PHH; ropeIdx = -1; pvy = 0; onGround = 1; }
        if (ropeIdx >= 0 && jumpPress) {
            ropeIdx = -1; pvy = -260; pvx = ix * 130; dropT = 0.1f;
            sfx(S_JUMP, 0.4f);
        }
        jumpPress = upPress = downPress = 0;
        if (ropeIdx >= 0) { dashReq = 0; goto timers; }
    }

    if (onGround) { coyote = 0.09f; jumps = 1 + perk[PK_JUMP]; airDash = 1; }
    else if (coyote > 0) coyote -= dt;
    if (jumpPress || upPress) {
        if (coyote > 0) { pvy = -305; coyote = 0; onGround = 0; sfx(S_JUMP, 0.4f); }
        else if (jumps > 0 && jumpPress) {
            pvy = -270; jumps--;
            for (int k = 0; k < 6; k++) addPart(px, py + PHH, frange(-40, 40), frange(10, 40), 0.3f, C(200, 200, 220), 1, 0, 0);
            sfx(S_JUMP, 0.4f);
        }
    }
    if (!jumpHeld && !held[K_UP] && pvy < -120) pvy += GRAV * gravK * dt * 1.3f; /* short hop when the key is released */
    if (downPress && onGround && onPlat >= 0) { dropT = 0.22f; onGround = 0; }
    jumpPress = upPress = downPress = 0;

    if (dashReq && dashCd <= 0 && (onGround || airDash)) {
        dashT = 0.15f; dashCd = dashMax();
        addProgress(P_DASHES, 1);
        dailyAdd(DE_DASH, 1);
        dashDir = ix ? (ix > 0 ? 1 : -1) : facing;
        if (!onGround) airDash = 0;
        sfx(S_DASH, 0.6f);
    }
    dashReq = 0;
    for (int g = 0; g < MAXGHOST; g++) if (ghosts[g].life > 0) ghosts[g].life -= dt;

    if (dashT > 0) {
        dashT -= dt;
        pvx = dashDir * 360; pvy = 0;
        addPart(px, py, 0, 0, 0.18f, C(150, 200, 255), 2, 0, 0);
        if ((isDavid(skin) && !sandyOff) || skin == SKIN_X) addGhost(px, py + PHH, facing, onGround ? 1 : 3);
        onGround = bodyStep(&px, &py, &pvx, &pvy, PHW, PHH, &onPlat, 1, 0, dt);
        if (dashT <= 0) pvx *= 0.3f;
    } else {
        float target_ = ix * 115 * (1 + 0.12f * fminf(perkPow[PK_SPEED], 5)), accel = onGround ? 16.f : 7.f;
        pvx += (target_ - pvx) * fminf(1, dt * accel);
        float fallV = pvy;
        int was = onGround;
        onGround = bodyStep(&px, &py, &pvx, &pvy, PHW, PHH, &onPlat, dropT, 1, dt);
        if (onGround && !was && fallV > 260)
            for (int k = 0; k < 6; k++) addPart(px + frange(-4, 4), py + PHH, frange(-50, 50), frange(-30, -5), 0.3f, C(170, 170, 190), 1, 200, 0);
    }
    if (py - PHH < 2) { py = PHH + 2; if (pvy < 0) pvy = 0; }
    if (onGround && fabsf(pvx) > 15) runAnim += dt * fabsf(pvx) / 12; else if (onGround) runAnim = 0;

timers:
    if (dropT > 0) dropT -= dt;
    if (dashCd > 0) dashCd -= dt;
    if (inv > 0) inv -= dt;
    if (fireCd > 0) fireCd -= dt;
    if (hurtFlash > 0) hurtFlash -= dt;
    if (reloadT > 0) {
        reloadT -= dt;
        if (reloadT <= 0) { reloadT = 0; mag[curW] = magSize(curW); sfx(S_RELOAD, 0.4f); }
    }
    if (muzzleT > 0) muzzleT -= dt;
    if (mdown && fireCd <= 0) fireWeapon();
}

static void openHole(float x, float y)
{
    for (int i = 0; i < MAXHOLE; i++)
        if (holes[i].life <= 0) { holes[i].x = x; holes[i].y = y; holes[i].life = HOLE_TIME; holes[i].kills = 0; break; }
    shake = fmaxf(shake, 3);
    sfx(S_WARP, 0.8f);
}

static int holeNow; /* the hole doing the crushing right now */
void holeKill(void) { if (++holes[holeNow].kills >= 5) achGive(ACH_HORIZON); }

/* black holes pull enemies (and their bullets) in, crush whatever reaches the core, then collapse */
void updateHoles(float dt)
{
    for (int h = 0; h < MAXHOLE; h++) {
        Hole *H = &holes[h];
        if (H->life <= 0) continue;
        H->life -= dt;
        holeNow = h;
        gravityKill = 1; killW = W_GRAVITY;
        float pullR = HOLE_PULL * blastMul(), dm = dmgMul();
        for (int i = 0; i < MAXE; i++) {
            Enemy *e = &en[i];
            if (!e->alive || e->state < 0) continue;
            float dx = H->x - e->x, dy = H->y - e->y, d = lenf(dx, dy) + 0.001f;
            if (e->type == EN_BOSS) { if (d < 40) damageEnemy(e, 6 * dm * dt, 0, 0); continue; }
            if (d > pullR) continue;
            float pull = (200 * (1 - d / pullR) + 45) * 4 * dt;
            e->rope = -1;
            e->vx += dx / d * pull;
            e->vy += dy / d * pull;
            e->vx *= 1 - dt * 2; e->vy *= 1 - dt * 2; /* spiral in rather than overshoot */
            if (d < 20) {
                e->x += dx * dt * 4; e->y += dy * dt * 4;
                damageEnemy(e, 4.5f * dm * dt, 0, 0);
            }
        }
        gravityKill = 0; killW = -1;
        for (int i = 0; i < MAXES; i++) { /* bend enemy shots into the hole */
            EShot *b = &eshots[i];
            if (b->life <= 0) continue;
            float dx = H->x - b->x, dy = H->y - b->y, d = lenf(dx, dy) + 0.001f;
            if (d < 8) { b->life = 0; continue; }
            if (d < 90) { b->vx += dx / d * 500 * dt; b->vy += dy / d * 500 * dt; }
        }
        for (int k = 0; k < 3; k++) { /* swirling dust being sucked in */
            float a = frand() * 2 * PI, r = frange(30, 80);
            addPart(H->x + cosf(a) * r, H->y + sinf(a) * r, -sinf(a) * 90 - cosf(a) * 110, cosf(a) * 90 - sinf(a) * 110,
                    0.45f, frand() < 0.6f ? C(170, 110, 255) : C(235, 225, 255), 1, 0, 0);
        }
        if (H->life <= 0) { /* collapse */
            gravityKill = 1; killW = W_GRAVITY;
            for (int i = 0; i < MAXE; i++) {
                Enemy *e = &en[i];
                if (!e->alive || e->state < 0) continue;
                float dx = e->x - H->x, dy = e->y - H->y, d = lenf(dx, dy);
                if (d < 36 * blastMul()) damageEnemy(e, 5 * dm, dx / (d + 0.1f), dy / (d + 0.1f));
            }
            gravityKill = 0; killW = -1;
            for (int k = 0; k < 30; k++) {
                float a = frand() * 2 * PI, sp = frange(60, 200);
                addPart(H->x, H->y, cosf(a) * sp, sinf(a) * sp, frange(0.3f, 0.7f), frand() < 0.5f ? C(200, 150, 255) : C(255, 255, 255), 2, 0, 0);
            }
            shake = fmaxf(shake, 6);
            sfx(S_BOOM, 0.6f);
        }
    }
}

void updateShots(float dt)
{
    for (int i = 0; i < MAXS; i++) {
        Shot *s = &shots[i];
        if (s->life <= 0) continue;
        float ox_ = s->x, oy_ = s->y;
        if (s->kind == 2) s->vy += 520 * dt;
        s->x += s->vx * dt; s->y += s->vy * dt; s->life -= dt;
        if (s->kind == 1 && (int)(gt * 60) % 2) addPart(s->x, s->y, frange(-10, 10), frange(-10, 10), 0.4f, C(150, 150, 150), 1, -30, 0);
        if (s->x < -10 || s->x > FW + 10 || s->y < -40) { s->life = 0; continue; }
        int boom = 0;
        if (s->kind == 3) { /* gravity orb: wobbling purple trail */
            addPart(s->x, s->y, frange(-25, 25), frange(-25, 25), 0.35f, frand() < 0.5f ? C(180, 120, 255) : C(90, 40, 160), 1, 0, 0);
            if (s->life <= 0) boom = 1;
        }
        if (s->kind == 2) { /* grenades bounce off platforms and the street */
            int pi;
            float surf = surfaceBelow(s->x, oy_ - 1, &pi);
            if (s->vy > 0 && oy_ <= surf && s->y >= surf) { s->y = surf - 1; s->vy *= -0.45f; s->vx *= 0.7f; }
            if (s->y >= GROUND_Y) { /* fired into the street at point blank: it still bounces back up */
                s->y = GROUND_Y - 1;
                if (s->vy > 0) s->vy *= -0.45f;
                s->vx *= 0.7f;
            }
            if (s->life <= 0) boom = 1;
        } else if (s->y >= GROUND_Y) {
            boom = s->kind == 1 || s->kind == 3;
            if (!boom) for (int k = 0; k < 3; k++) addPart(s->x, GROUND_Y - 1, frange(-50, 50), frange(-90, -20), 0.3f, C(255, 230, 150), 1, 400, 0);
            s->life = 0;
        }
        if (s->life > 0 || boom) {
            for (int j = 0; j < MAXE && !boom; j++) {
                Enemy *e = &en[j];
                if (!e->alive || e->state == -2 || j == s->lastHit || !inBox(e, s->x, s->y, 1)) continue;
                if (s->kind == 0) {
                    float sl = lenf(s->vx, s->vy) + 0.001f;
                    /* riot shields stop bullets from the front - unless it's lowered after a lunge, or you shoot down over it */
                    if (e->type == EN_SHIELD && e->state != 3 && s->vx * e->facing < 0 && s->vy < fabsf(s->vx) * 1.2f) {
                        for (int k = 0; k < 5; k++) addPart(s->x, s->y, e->facing * frange(30, 110), frange(-110, -10), 0.25f, k & 1 ? C(255, 255, 255) : C(150, 220, 255), 1, 400, 0);
                        e->timer2 = 0.08f;
                        sfx(S_CLANG, 0.3f);
                        s->life = 0;
                        break;
                    }
                    if (isFlesh(e->type)) gore(s->x, s->y, 5, s->vx / sl, 90);
                    else for (int k = 0; k < 4; k++) addPart(s->x, s->y, -s->vx / sl * 40 + frange(-60, 60), frange(-90, 0), 0.25f, C(255, 220, 120), 1, 400, 0);
                    sfx(S_HIT, 0.25f);
                    killW = s->weapon;
                    damageEnemy(e, s->dmg, s->vx / sl, s->vy / sl);
                    killW = -1;
                    if (s->pierce > 0) { s->pierce--; s->lastHit = j; s->dmg *= 0.8f; } /* piercing rounds keep going */
                    else s->life = 0;
                } else boom = 1;
                break;
            }
        }
        if (boom) {
            s->life = 0;
            if (s->kind == 3) {
                openHole(ox_, clampf(oy_, 12, GROUND_Y - 8));
            } else {
                float R = (s->kind == 1 ? 30 : 34) * blastMul();
                killW = s->weapon;
                explode(ox_, oy_, R, s->dmg, 1);
                killW = -1;
                /* your own blasts hurt you too and throw you around: grenades badly, rockets only a little,
                   so a rocket at your feet works as a rocket jump */
                float dx = px - ox_, dy = py - oy_, d = lenf(dx, dy), reach = R * 0.92f;
                if (d < reach) {
                    float k = 1 - d / reach;
                    selfHit = 1;
                    hurtPlayer(ox_, oy_, s->kind == 2 ? (int)(14 + 26 * k) : (int)(4 + 8 * k));
                    selfHit = 0;
                    if (ropeIdx < 0 && state == ST_PLAY) {
                        float nx = d > 0.5f ? dx / d : 0, ny = d > 0.5f ? dy / d : -1, push = (s->kind == 1 ? 300 : 200) * (0.45f + 0.55f * k);
                        pvx += nx * push;
                        pvy = fminf(pvy, 0) * 0.3f + (ny - 0.35f) * push;
                        onGround = 0; coyote = 0;
                        if (s->kind == 1 && pvy < -150) { achGive(ACH_ROCKET); addProgress(P_RJUMPS, 1); dailyAdd(DE_RJUMP, 1); }
                    }
                }
            }
        }
    }

    for (int i = 0; i < MAXES; i++) {
        EShot *b = &eshots[i];
        if (b->life <= 0) continue;
        float oy_ = b->y;
        if (b->kind == 1) b->vy += 400 * dt;
        if (b->kind == 2) { /* homing missile */
            float a = atan2f(b->vy, b->vx), want = atan2f(py - b->y, px - b->x), da = want - a;
            while (da > PI) da -= 2 * PI;
            while (da < -PI) da += 2 * PI;
            a += clampf(da, -2.2f * dt, 2.2f * dt);
            b->vx = cosf(a) * 130; b->vy = sinf(a) * 130;
            if ((int)(gt * 60) % 2) addPart(b->x, b->y, 0, 0, 0.35f, C(140, 140, 150), 1, -20, 0);
        }
        b->x += b->vx * dt; b->y += b->vy * dt; b->life -= dt;
        if (b->x < -10 || b->x > FW + 10 || b->y > FH + 10) { b->life = 0; continue; }
        int hitPlayer = fabsf(b->x - px) < PHW + 1.5f && fabsf(b->y - py) < PHH + 1.5f && inv <= 0 && dashT <= 0;
        if (b->kind == 1) {
            float surf = surfaceBelow(b->x, oy_ - 1, 0);
            if ((b->vy > 0 && oy_ <= surf && b->y >= surf) || hitPlayer) { b->life = 0; explode(b->x, b->y - 2, 24, 0, 0); }
        } else if (b->kind == 2) {
            if (hitPlayer || b->life <= 0 || b->y >= GROUND_Y) { b->life = 0; explode(b->x, b->y, 22, 0, 0); }
        } else {
            if (b->y >= GROUND_Y) { b->life = 0; continue; }
            if (hitPlayer) { hurtPlayer(b->x, b->y, bossIdx >= 0 ? 12 : 10 + (level() > 32 ? 8 : level() / 4)); b->life = 0; }
        }
    }
}
