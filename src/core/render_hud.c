/* render_hud.c - The in-game HUD: health, ammo, weapons, score and boss bars. */
#include "core.h"

void drawCoin(int x, int y)
{
    fillCircle(x, y, 3, C(150, 100, 20));
    fillCircle(x, y, 2, C(255, 210, 60));
    pset(x - 1, y - 1, C(255, 250, 200));
}

void hudBar(int x, int y, int w, int h, float frac, unsigned c)
{
    fillRect(x - 1, y - 1, w + 2, h + 2, C_OUT);
    fillRect(x, y, w, h, C(50, 48, 60));
    fillRect(x, y, (int)(w * clampf(frac, 0, 1)), h, c);
}

void renderHud(void)
{
    char s[48];
    /* health bar with the number of hit points left on it */
    float hf = hp / (float)maxHp;
    unsigned hc = hf > 0.6f ? C(80, 220, 100) : hf > 0.3f ? C(240, 200, 60) : C(230, 50, 50);
    if (!strncmp(ui.name, "COLOURBLIND", 12)) hc = hf > 0.6f ? C(86, 180, 233) : hf > 0.3f ? C(240, 228, 66) : C(230, 159, 0);
    if (hf <= 0.3f && ((int)(gt * 6) & 1)) hc = C(255, 120, 120);
    drawSprite(&sHeart, 4, 5, 0, DM_NORMAL, 0);
    hudBar(14, 5, 92, 7, hf, hc);
    fillRect(14, 5, (int)(92 * hf), 1, mixc(hc, C_WHITE, 0.4f));
    sfmt(s, sizeof s, "%d", hp);
    text(60, 6, s, C_WHITE, 1, AL_CENTER);
    hudBar(14, 15, 50, 2, dashCd <= 0 ? 1 : 1 - dashCd / dashMax(), dashCd <= 0 ? C(120, 200, 255) : C(70, 100, 130));
    text(67, 14, "DASH", dashCd <= 0 ? C(120, 200, 255) : C(80, 90, 110), 1, AL_LEFT);
    if (perksTaken) { /* one pip per upgrade taken */
        for (int i = 0, n = 0; i < PK_COUNT; i++)
            for (int k = 0; k < perk[i]; k++, n++) {
                if (n >= 46) continue; /* two rows; the rest is a number */
                int x = 14 + (n % 23) * 4, y = 21 + (n / 23) * 4; fillRect(x - 1, y - 1, 4, 4, C_OUT); fillRect(x, y, 2, 2, PERKS[i].col);
            }
        if (perksTaken > 46) { sfmt(s, sizeof s, "+%d", perksTaken - 46); text(107, 25, s, C(255, 214, 80), 1, AL_LEFT); }
    }

    if (mode == MODE_ENDLESS) sfmt(s, sizeof s, "%s - ENDLESS WAVE %d", D->name, waveIdx);
    else sfmt(s, sizeof s, "%s - WAVE %d/5", D->name, waveIdx);
    text(FW / 2, 4, s, C(150, 255, 200), 1, AL_CENTER);
    sfmt(s, sizeof s, "%d LEFT", toSpawn + countAlive());
    text(FW / 2, 11, s, C(170, 170, 185), 1, AL_CENTER);
    if (hardOn) text(FW / 2 + textWidth(s, 1) / 2 + 6, 11, "HARD", ((int)(gt * 2) & 1) ? C(255, 60, 60) : C(200, 30, 40), 1, AL_LEFT);

    sfmt(s, sizeof s, "%d", score);
    text(FW - 4, 4, s, C_WHITE, 1, AL_RIGHT);
    sfmt(s, sizeof s, "BEST %d", pstat[P_BESTSCORE] > score ? pstat[P_BESTSCORE] : score);
    text(FW - 4, 11, s, C(150, 150, 165), 1, AL_RIGHT);
    sfmt(s, sizeof s, "%d", coins);
    text(FW - 4, 18, s, C(255, 214, 80), 1, AL_RIGHT);
    drawCoin(FW - 8 - textWidth(s, 1), 20);
    if (combo >= 6) {
        int mult = 1 + combo / 6;
        if (mult > 5) mult = 5;
        sfmt(s, sizeof s, "COMBO X%d", mult);
        text(FW - 4, 26, s, C(255, 220, 90), 1, AL_RIGHT);
        hudBar(FW - 37, 33, 33, 1, comboT / 2.0f, C(255, 220, 90));
    }

    /* weapon bar: keys 1-6 */
    for (int w = 0; w < W_COUNT; w++) {
        int x = 5 + w * 36, y = FH - 17;
        int empty = mag[w] <= 0, reloading = w == curW && reloadT > 0;
        unsigned border = w == curW ? C(255, 230, 120) : C(70, 66, 84);
        fillRect(x, y, 34, 15, C_OUT);
        fillRect(x + 1, y + 1, 32, 13, w == curW ? C(64, 58, 44) : C(30, 28, 40));
        for (int i = 0; i < 34; i++) { pset(x + i, y, border); pset(x + i, y + 14, border); }
        for (int j = 0; j < 15; j++) { pset(x, y + j, border); pset(x + 33, y + j, border); }
        drawSprite(WEAP[w].icon, x + 13, y + 2, 0, empty ? DM_SOLID : DM_NORMAL, C(60, 56, 70));
        sfmt(s, sizeof s, "%d", w + 1);
        drawText(x + 3, y + 3, s, w == curW ? C(255, 230, 120) : C(120, 116, 130), 1);
        sfmt(s, sizeof s, "%d", mag[w]);
        drawText(x + 32 - textWidth(s, 1), y + 8, s, empty ? C(200, 80, 80) : C(200, 200, 210), 1);
        if (reloading) fillRect(x + 1, y + 13, (int)(32 * (1 - reloadT / reloadMax)), 1, C(255, 220, 90));
    }
    if (reloadT > 0) {
        sfmt(s, sizeof s, "%s - RELOADING", WEAP[curW].name);
        text(5 + W_COUNT * 36 + 4, FH - 13, s, ((int)(gt * 6) & 1) ? C(255, 220, 90) : WEAP[curW].col, 1, AL_LEFT);
    } else {
        sfmt(s, sizeof s, "%s %d/%d", WEAP[curW].name, mag[curW], magSize(curW));
        text(5 + W_COUNT * 36 + 4, FH - 13, s, WEAP[curW].col, 1, AL_LEFT);
    }

    if (bossIdx >= 0 && en[bossIdx].alive && en[bossIdx].type == EN_BOSS) {
        /* the boss bar: a framed health bar with a trailing chunk that shows the last hits, ticks every 10% */
        static float shownHp = 1;
        const Enemy *b = &en[bossIdx];
        float f = clampf(b->hp / b->maxhp, 0, 1);
        int big = b->mega != 0, bw = big ? 300 : 220, bh = big ? 7 : 5, bx = FW / 2 - bw / 2, by = 22;
        unsigned col = big ? MEGA_COL[b->mega - 1] : C(230, 70, 60);
        char s[40];
        shownHp = shownHp < f || shownHp - f > 0.5f ? f : shownHp - (shownHp - f) * 0.04f;
        fillRect(bx - 3, by - 3, bw + 6, bh + 6, C_OUT);
        fillRect(bx - 2, by - 2, bw + 4, 1, col); fillRect(bx - 2, by + bh + 1, bw + 4, 1, scalec(col, 120));
        fillRect(bx - 2, by - 2, 1, bh + 4, col); fillRect(bx + bw + 1, by - 2, 1, bh + 4, col);
        fillRect(bx, by, bw, bh, C(30, 16, 22));
        fillRect(bx, by, (int)(bw * shownHp), bh, C(255, 240, 200));
        fillRect(bx, by, (int)(bw * f), bh, f < 0.25f && ((int)(gt * 8) & 1) ? C_WHITE : col);
        fillRect(bx, by, (int)(bw * f), 1, mixc(col, C_WHITE, 0.5f));
        for (int t = 1; t < 10; t++) fillRect(bx + bw * t / 10, by, 1, bh, C_OUT);
        if (big) sfmt(s, sizeof s, "BIG BOSS - %s", MEGA_NAME[b->mega - 1]);
        else sfmt(s, sizeof s, "GUNSHIP - %s", D->bossName);
        text(FW / 2, by + bh + 4, s, col, 1, AL_CENTER);
        sfmt(s, sizeof s, "%d%s", (int)(f * 100 + 0.99f), "%");
        text(bx + bw + 5, by + (bh - 5) / 2, s, C_WHITE, 1, AL_LEFT);
    }

    if (bossCardT > 0 && state != ST_PERK) { /* the boss intro card: slams in from the side, holds, then whips out */
        float el = BOSS_CARD_T - bossCardT, in = el < 0.22f ? 1 - el / 0.22f : 0, out = bossCardT < 0.3f ? 1 - bossCardT / 0.3f : 0;
        int dx = (int)(in * in * FW) - (int)(out * out * FW), top = 84, h = 66, nsc = textWidth(bossCardName, 4) <= FW - 40 ? 4 : 3;
        unsigned c = bossCardCol;
        if (el < 0.12f) for (int i = 0; i < FW * FH; i++) fb[i] = mixc(fb[i], C_WHITE, 0.25f); /* the slam flash */
        for (int y = top; y < top + h; y++)
            for (int x = 0; x < FW; x++) {
                int sx = x - dx + (y - top) / 3; /* slanted edges */
                if (sx < 0 || sx >= FW) continue;
                fb[y * FW + x] = mixc(fb[y * FW + x], C(8, 4, 12), 0.82f);
            }
        for (int x = 0; x < FW; x++) {
            int a = x + dx;
            if (a < 0 || a >= FW) continue;
            fillRect(a, top, 1, 2, c); fillRect(a, top + h - 2, 1, 2, c);
            if (((x + (int)(gt * 60)) / 8) % 2) pset(a, top + 4, scalec(c, 120));
        }
        text(FW / 2 + dx, top + 8, ((int)(gt * 8) & 1) ? "! WARNING !" : "", C(255, 60, 60), 1, AL_CENTER);
        text(FW / 2 + dx + 2, top + 18 + 2, bossCardName, C_OUT, nsc, AL_CENTER);
        text(FW / 2 + dx, top + 18, bossCardName, el < 0.4f && ((int)(gt * 20) & 1) ? C_WHITE : c, nsc, AL_CENTER);
        text(FW / 2 + dx, top + 46, bossCardSub, C_WHITE, 1, AL_CENTER);
        text(FW / 2 + dx, top + 56, bossCardLine, scalec(c, 200), 1, AL_CENTER);
    }
    if (bannerT > 0 && state != ST_PERK && bossCardT <= 0) {
        int y = 70 - (bannerT > 2.1f ? (int)((bannerT - 2.1f) * 60) : 0);
        text(FW / 2, y, banner, C_WHITE, 3, AL_CENTER);
        text(FW / 2, y + 20, banner2, C(255, 210, 80), 1, AL_CENTER);
    }
    if (calloutT > 0 && bannerT <= 0) { /* multi-kill / rampage callout, pops in big then settles */
        int sc = calloutT > 1.15f ? 3 : 2, y = 46;
        unsigned c = calloutT < 0.35f ? mixc(calloutCol, C(40, 40, 50), 1 - calloutT / 0.35f) : calloutCol;
        text(FW / 2 + 1, y + 1, callout, C_OUT, sc, AL_CENTER);
        text(FW / 2, y, callout, ((int)(gt * 12) & 1) && calloutT > 0.9f ? C_WHITE : c, sc, AL_CENTER);
    }
}

/* cursor skins: drawn around a hot spot, in "cursor pixels" that are curS screen pixels big (1 in game, bigger on the cards) */
const char *const CURSORS[CURSOR_COUNT] = { "CLASSIC", "DOT", "RING", "SNIPER", "X", "BRACKETS", "NEON", "RAINBOW",
                                                   "ARROW", "SKULL", "HEART", "STAR", "PIXEL", "TARGET", "DIAMOND", "LOCK-ON",
                                                   "RADAR", "GLITCH", "TRI", "SWORD", "FLAME", "GHOST", "SLIME", "BOLT",
                                                   "MOON", "CROWN", "MINI CROSS", "SPECK", "LIME DOT", "MINI X", "MINI RING",
                                                   "GAP CROSS", "CHEVRON", "BLINK" };
int curX, curY, curS = 1;
void curPx(int x, int y, unsigned c) { if (curS == 1) pset(curX + x, curY + y, c); else fillRect(curX + x * curS, curY + y * curS, curS, curS, c); }
