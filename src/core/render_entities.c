/* render_entities.c - Drawing enemies, the pet, the player and everything else in the world. */
#include "core.h"

void renderEnemy(const Enemy *e)
{
    if (e->state == -2) { /* rope unrolling from above */
        float tip = (1 - e->timer3 / 0.45f) * (e->targetY + e->hh);
        for (int y = 0; y < (int)tip; y++) pset((int)e->x + ox, y + oy, ((y >> 1) & 1) ? C(160, 118, 72) : C(104, 74, 44));
        return;
    }
    if (e->type != EN_BOSS && e->type != EN_DRONE && (e->state == -1)) {
        for (int y = (int)e->anchorY; y < (int)(e->y - e->hh + 2); y++)
            pset((int)e->x + ox, y + oy, ((y >> 1) & 1) ? C(160, 118, 72) : C(104, 74, 44));
    }
    int flip;
    const Spr *s = enemySprite(e, &flip);
    int dm = DM_NORMAL;
    unsigned tint = 0;
    if (e->flash > 0 && state == ST_PLAY) { dm = DM_SOLID; tint = C_WHITE; }
    else if (e->type == EN_BOMBER && e->state == 1 && ((int)(gt * 16) & 1)) { dm = DM_FILL; tint = C(255, 150, 40); }
    else if (e->type == EN_BRUTE && e->state == 1 && ((int)(gt * 16) & 1)) { dm = DM_TINT; tint = C_WHITE; }
    else if (e->type == EN_NINJA && e->state >= 1 && ((int)(gt * 16) & 1)) { dm = DM_TINT; tint = C(60, 230, 255); }
    else if (e->elite) { dm = DM_TINT; tint = C(255, 210, 60); }
    float feet = e->y + e->hh;
    if (e->type == EN_BOSS && e->mega) { drawGiant(e); return; }
    if (e->type == EN_BOSS) {
        int x = (int)e->x + ox, y = (int)e->y - 10 + oy;
        int blade = (int)(gt * 30) % 3;
        int half = blade == 0 ? 34 : blade == 1 ? 22 : 8;
        fillRect(x - half, y + 1, half * 2, 1, C(30, 30, 40));
        fillRect(x - half + 2, y, half * 2 - 4, 1, C(80, 80, 96));
        drawSprite(s, x - s->w / 2, y + 2, flip, dm, tint);
        int tx = flip ? x + s->w / 2 - 1 : x - s->w / 2;
        if ((int)(gt * 20) & 1) { pset(tx, y + 6, C(200, 200, 210)); pset(tx, y + 10, C(200, 200, 210)); }
        int lx = flip ? x - s->w / 2 + 2 : x + s->w / 2 - 3;
        if (fmodf(gt, 0.8f) < 0.2f) pset(lx, y + 14, C(255, 60, 60));
        if (e->state == 0 && e->timer < 0.5f && ((int)(gt * 16) & 1)) { /* aim warning before each burst */
            float gx = e->x + e->facing * 14, gy = e->y + 8, a = atan2f(py - gy, px - gx);
            for (int k = 6; k < 140; k += 4) pset((int)(gx + cosf(a) * k) + ox, (int)(gy + sinf(a) * k) + oy, C(255, 50, 50));
        }
        return;
    }
    if (e->onGround && e->type != EN_DRONE)
        for (int i = -e->hw - 1; i <= e->hw + 1; i++) pdark((int)e->x + i + ox, (int)feet + oy, 160);
    float jx = (e->type == EN_BRUTE && e->state == 1) ? frange(-1, 1) : 0;
    int own = eBody && (e->type == EN_THUG || e->type == EN_GUNNER || e->type == EN_BRUTE || e->type == EN_DRONE);
    const unsigned *keep = skinOv;
    if (own) skinOv = eBody->pal;
    drawFeet(s, e->x + jx, feet, flip, dm, tint);
    skinOv = keep;
    if (e->type == EN_DRONE && e->state == 1 && ((int)(gt * 16) & 1)) pset((int)e->x + ox, (int)e->y + 1 + oy, C(255, 255, 120));
    if (e->type == EN_GUNNER) {
        float a = atan2f(py - (e->y - 3), px - e->x);
        drawArmGun(e->x + e->facing, e->y - 3, a, 9, 2, e->state ? C(255, 80, 80) : C(70, 74, 90), 0, own ? eBody->pal[3] : pal('G'));
        if (e->state && ((int)(gt * 20) & 1)) /* laser sight */
            for (int k = 14; k < 80; k += 3) pset((int)(e->x + cosf(a) * k) + ox, (int)(e->y - 3 + sinf(a) * k) + oy, C(255, 40, 40));
    }
    if (e->type == EN_BRUTE && e->state == 1)
        for (int k = 10; k < 90; k += 4) pset((int)(e->x + e->ax * k) + ox, (int)feet - 4 + oy, C(255, 60, 40));
    if (e->type == EN_SHIELD) { /* the riot shield: raised in front, or lowered and tilted while recovering */
        int lowered = e->state == 3, sx = (int)e->x + e->facing * (lowered ? 5 : 6) + ox, top = (int)feet - (lowered ? 9 : 18) + oy;
        int h = lowered ? 8 : 16;
        unsigned rim = e->timer2 > 0 ? C_WHITE : e->state == 1 && ((int)(gt * 16) & 1) ? C(255, 120, 80) : C(70, 82, 112);
        for (int j = 0; j < h; j++)
            for (int i = 0; i < 3; i++) {
                int x = sx + (e->facing > 0 ? i : -i);
                if (i == 2 || j == 0 || j == h - 1) pset(x, top + j, rim);
                else pmix(x, top + j, e->timer2 > 0 ? C_WHITE : C(150, 190, 230), 0.65f);
            }
        if (!lowered) { pset(sx + (e->facing > 0 ? 1 : -1), top + 4, C(220, 240, 255)); pset(sx + (e->facing > 0 ? 1 : -1), top + 5, C(220, 240, 255)); }
    }
    if (e->elite) { int x = (int)e->x + ox, y = (int)feet - s->h - 4 + oy; pset(x, y, C(255, 220, 60)); pset(x - 1, y + 1, C(255, 220, 60)); pset(x + 1, y + 1, C(255, 220, 60)); }
}

static void renderPet(void)
{
    if (!pet || state == ST_OVER) return; /* it stays with you until you go down */
    int f = petFlies(pet) ? petFlap(petWalk) : !petGround ? 1 : fabsf(petVx) > 6 ? 1 + ((int)petWalk & 1) : 0;
    if (petGround) for (int i = -4; i <= 4; i++) pdark((int)petX + i + ox, (int)petY + 4 + oy, 170);
    drawPet(pet, (int)petX + ox, (int)petY + 4 + oy, petFace < 0, f, 1);
}

/* the Sandevistan trail: see-through copies of David in shifting neon colours, oldest faintest */
static void renderGhosts(void)
{
    static const unsigned cols[6] = { C(80, 255, 140), C(60, 230, 255), C(150, 110, 255), C(255, 80, 200), C(255, 160, 60), C(255, 240, 80) };
    for (int k = 0; k < MAXGHOST; k++) {
        const Ghost *g = &ghosts[k];
        if (g->life <= 0) continue;
        const Spr *s = skinFrame(skin, g->f);
        float a = g->life / GHOST_LIFE * 0.75f;
        unsigned c = skin == SKIN_X ? (g->n & 1 ? C(255, 30, 60) : C(120, 10, 30)) : cols[g->n % 6];
        int x0 = (int)floorf(g->x) - s->w / 2 + ox, y0 = (int)floorf(g->y) - s->h + oy;
        for (int j = 0; j < s->h; j++) {
            const char *row = s->rows[j];
            int len = (int)strlen(row);
            for (int i = 0; i < s->w && i < len; i++)
                if (row[i] != '.') pmix(g->face < 0 ? x0 + s->w - 1 - i : x0 + i, y0 + j, c, a);
        }
    }
}

static void renderPlayer(void)
{
    if (state == ST_OVER) return;
    if (inv > 0 && dashT <= 0 && fmodf(gt, 0.14f) < 0.07f) return;
    int f = 0;
    if (ropeIdx >= 0) f = (((int)runAnim) & 1) ? 2 : 0;
    else if (!onGround) f = 3;
    else if (runAnim > 0) f = (((int)runAnim) & 1) ? 1 : 2;
    const Spr *s = skinFrame(skin, f);
    const Weapon *w = &WEAP[curW];
    float sx, sy;
    shoulder(&sx, &sy);
    if (onGround) for (int i = -5; i <= 5; i++) pdark((int)px + i + ox, (int)(py + PHH) + oy, 150);
    unsigned sleeve = SKINS[skin].pal[4];
    if (ropeIdx >= 0) { /* free hand holds the rope */
        int rx = (int)D->rope[ropeIdx].x + ox;
        thickLine((int)px + ox, (int)py - 4 + oy, rx, (int)py - 10 + oy, sleeve, 2);
    }
    skinOv = skinPal(skin);
    drawFeet(s, px, py + PHH, facing < 0, dashT > 0 ? DM_TINT : DM_NORMAL, C(180, 220, 255));
    skinOv = 0;
    if (skin == SKIN_X) { /* red glitch lines flicker across it */
        unsigned h = hash3((int)(gt * 24), 7, 11);
        if (h % 3 == 0) fillRect((int)px - 7 + (int)(h >> 4) % 5 + ox, (int)(py - PHH) + (int)(h >> 8) % (int)(2 * PHH) + oy, 3 + (int)(h >> 12) % 6, 1, C(255, 30, 50));
        for (int i = 0; i < 4; i++) { /* and red sparks drift up off it */
            float t = fmodf(gt * 0.9f + i * 0.25f, 1.0f);
            int sx = (int)px - 6 + (int)(hash3(i, (int)(gt * 0.9f + i * 0.25f), 3) % 13);
            pset(sx + ox, (int)(py + PHH - 4 - t * 26) + oy, t < 0.7f ? C(255, 40, 60) : C(140, 20, 40));
        }
    }
    if (reloadT > 0) { /* gun tipped down while reloading, progress over the head */
        drawArmGun(sx, sy, facing > 0 ? 1.1f : PI - 1.1f, w->gunLen, w->thick, w->col, 0, sleeve);
        int bx = (int)px - 8 + ox, by = (int)(py - PHH) - 6 + oy;
        fillRect(bx - 1, by - 1, 18, 4, C_OUT);
        fillRect(bx, by, (int)(16 * (1 - reloadT / reloadMax)), 2, C(255, 220, 90));
    } else {
        drawArmGun(sx, sy, aim, w->gunLen, w->thick, w->col, muzzleT > 0, sleeve);
    }
}

void renderWorld(void)
{
    for (int i = 0; i < MAXRFX; i++) {
        RopeFx *r = &ropeFx[i];
        if (r->life <= 0) continue;
        int y1 = (int)(r->y0 + (r->y1 - r->y0) * (r->life / 0.4f));
        for (int y = (int)r->y0; y < y1; y++) pset((int)r->x + ox, y + oy, ((y >> 1) & 1) ? C(160, 118, 72) : C(104, 74, 44));
    }
    for (int i = 0; i < MAXPU; i++) {
        Pickup *p = &pu[i];
        if (p->life <= 0 || (p->life < 3 && ((int)(gt * 8) & 1))) continue;
        int x = (int)p->x + ox, y = (int)p->y + oy;
        if (p->chute) { /* parachute canopy and lines */
            for (int k = -9; k <= 9; k++) { int cy = y - 22 + (k * k) / 14; pset(x + k, cy, k & 2 ? C(230, 230, 240) : C(220, 70, 70)); pset(x + k, cy - 1, C(200, 60, 60)); }
            line(x - 9, y - 16, x - 4, y - 5, C(200, 200, 210));
            line(x + 9, y - 16, x + 4, y - 5, C(200, 200, 210));
        }
        drawSprite(&sMedkit, x - 5, y - 5 + (p->landed ? (int)(sinf(gt * 4 + i) * 1.2f) : 0), 0, DM_NORMAL, 0);
    }
    for (int h = 0; h < MAXHOLE; h++) { /* black hole: darkened lens and a spinning accretion ring behind everything */
        Hole *H = &holes[h];
        if (H->life <= 0) continue;
        int cx = (int)H->x + ox, cy = (int)H->y + oy;
        float grow = fminf(1, (HOLE_TIME - H->life) * 5) * fminf(1, H->life * 4);
        int R = (int)(34 * grow * blastMul());
        for (int j = -R; j <= R; j++)
            for (int i = -R; i <= R; i++) {
                int d2 = i * i + j * j;
                if (d2 <= R * R) pdark(cx + i, cy + j, 90 + 166 * d2 / (R * R + 1));
            }
        for (int k = 0; k < 24; k++) {
            float a = k * PI / 12 + gt * 6, rr = (14 + 3 * sinf(gt * 9 + k)) * grow;
            pset(cx + (int)(cosf(a) * rr), cy + (int)(sinf(a) * rr * 0.55f), k & 1 ? C(200, 140, 255) : C(120, 60, 220));
        }
    }
    for (int i = 0; i < MAXE; i++) if (en[i].alive && en[i].type == EN_BOSS) renderEnemy(&en[i]);
    enemyLook(eskin, 1);
    for (int i = 0; i < MAXE; i++) if (en[i].alive && en[i].type != EN_BOSS) renderEnemy(&en[i]);
    enemyLook(0, 0);
    renderPet();
    renderGhosts();
    renderPlayer();
    for (int h = 0; h < MAXHOLE; h++) { /* the core sits in front, so things vanish into it */
        Hole *H = &holes[h];
        if (H->life <= 0) continue;
        float grow = fminf(1, (HOLE_TIME - H->life) * 5) * fminf(1, H->life * 4);
        int cx = (int)H->x + ox, cy = (int)H->y + oy, r = (int)((6 + sinf(gt * 14)) * grow);
        fillCircle(cx, cy, r + 2, C(150, 90, 255));
        fillCircle(cx, cy, r + 1, C(60, 20, 110));
        fillCircle(cx, cy, r, C(0, 0, 0));
    }

    for (int i = 0; i < MAXS; i++) {
        Shot *s = &shots[i];
        if (s->life <= 0) continue;
        int x = (int)s->x + ox, y = (int)s->y + oy;
        if (s->kind == 1) {
            int tx = (int)(s->x - s->vx * 0.02f) + ox, ty = (int)(s->y - s->vy * 0.02f) + oy;
            thickLine(tx, ty, x, y, C(150, 150, 90), 2);
            pset(tx, ty, C(255, 160, 40));
        } else if (s->kind == 2) {
            fillCircle(x, y, 2, C_OUT);
            fillCircle(x, y, 1, C(120, 180, 100));
            if ((int)(gt * 10) & 1) pset(x, y - 2, C(255, 80, 60));
        } else if (s->kind == 3) { /* gravity orb */
            fillCircle(x, y, 4, C(120, 60, 220));
            fillCircle(x, y, 2, C(20, 0, 40));
            pset(x - 1, y - 1, C(230, 210, 255));
        } else {
            int tx = (int)(s->x - s->vx * 0.012f) + ox, ty = (int)(s->y - s->vy * 0.012f) + oy;
            line(tx, ty, x, y, C(255, 200, 80));
            pset(x, y, C(255, 255, 220));
        }
    }
    for (int i = 0; i < MAXES; i++) {
        EShot *b = &eshots[i];
        if (b->life <= 0) continue;
        int x = (int)b->x + ox, y = (int)b->y + oy;
        if (b->kind == 1) {
            fillCircle(x, y, 2, C_OUT); pset(x, y, C(90, 90, 100));
            if ((int)(gt * 12) & 1) pset(x, y - 2, C(255, 60, 60));
        } else if (b->kind == 2) {
            int tx = (int)(b->x - b->vx * 0.05f) + ox, ty = (int)(b->y - b->vy * 0.05f) + oy;
            thickLine(tx, ty, x, y, C(200, 200, 210), 2);
            pset(x, y, C(255, 60, 60));
        } else {
            int tx = (int)(b->x - b->vx * 0.02f) + ox, ty = (int)(b->y - b->vy * 0.02f) + oy;
            line(tx, ty, x, y, C(255, 70, 90));
            pset(x, y, C(255, 200, 210));
        }
    }
    for (int i = 0; i < MAXBEAM; i++) {
        Beam *b = &beams[i];
        if (b->life <= 0) continue;
        int x0 = (int)b->x0 + ox, y0 = (int)b->y0 + oy, x1 = (int)b->x1 + ox, y1 = (int)b->y1 + oy;
        line(x0, y0 - 1, x1, y1 - 1, C(40, 160, 220));
        line(x0, y0 + 1, x1, y1 + 1, C(40, 160, 220));
        line(x0, y0, x1, y1, b->life > 0.06f ? C_WHITE : C(140, 250, 255));
    }
    for (int i = 0; i < MAXBOOM; i++) {
        Boom *b = &booms[i];
        if (b->life <= 0) continue;
        float t = 1 - b->life / b->max;
        int x = (int)b->x + ox, y = (int)b->y + oy, r = (int)(b->r * (0.4f + 0.6f * t));
        fillCircle(x, y, r, t < 0.3f ? C_WHITE : t < 0.6f ? C(255, 200, 60) : C(230, 90, 30));
        if (t > 0.3f) fillCircle(x, y, (int)(r * (t - 0.2f)), C(90, 60, 50));
    }
    for (int i = 0; i < MAXP; i++) {
        Part *p = &parts[i];
        if (p->life <= 0) continue;
        int x = (int)p->x + ox, y = (int)p->y + oy;
        if (p->sz > 1 && p->life > p->max * 0.4f) fillRect(x, y, 2, 2, p->c); else pset(x, y, p->c);
    }
    for (int i = 0; i < MAXF; i++)
        if (fl[i].life > 0) { /* a dark outline keeps them readable over explosions */
            int x = (int)fl[i].x + ox, y = (int)fl[i].y + oy;
            for (int d = 0; d < 4; d++) text(x + (d == 0) - (d == 1), y + (d == 2) - (d == 3), fl[i].s, C_OUT, 1, AL_CENTER);
            text(x, y, fl[i].s, fl[i].c, 1, AL_CENTER);
        }
}
