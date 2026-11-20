/* ui_screens_misc.c - Menu screens: quests, keys, quit, upgrade cards and toasts. */
#include "core.h"

/* a thin bar at the right edge when the screen scrolls */
void drawScrollBar(void)
{
    int track = FH - 16, th = track * FH / (FH + uiScrollMax), ty = 8 + (track - th) * uiScroll / uiScrollMax;
    fillRect(FW - 5, 8, 2, track, C(50, 46, 64));
    fillRect(FW - 6, ty, 4, th, ui.hi);
    if (uiScroll < uiScrollMax && fmodf(gt, 1.0f) < 0.7f) text(FW - 12, FH - 10, "V", ui.hi, 1, AL_CENTER);
}

void renderQuests(void)
{
    char s[80];
    int pages = (QUEST_COUNT + QUEST_PAGE - 1) / QUEST_PAGE;
    const unsigned gold = C(255, 205, 60);
    text(FW / 2, 4, "QUESTS", C_WHITE, 3, AL_CENTER);
    for (int i = 0; i < nBtns; i++) drawButton(&btns[i], i == focusBtn);
    coinsCorner();
    /* today's three, pinned on top in gold */
    {
        int left = 86400 - plat_daysec();
        neonText(FW / 2, 25, "DAILY QUESTS", gold, 2, 1);
        sfmt(s, sizeof s, "FROM %d", dailyPool());
        text(32, 29, s, C(150, 140, 100), 1, AL_LEFT);
        sfmt(s, sizeof s, "NEW IN %dH %dM", left / 3600, left % 3600 / 60);
        text(FW - 32, 29, s, C(150, 140, 100), 1, AL_RIGHT);
        for (int i = 0; i < 3; i++) {
            const Daily *q = &dqCache[i];
            int y = 39 + i * 22, done = dqDone & (1 << i), have = dqProg[i] < q->goal ? dqProg[i] : q->goal;
            frame(30, y, FW - 60, 20, done ? C(20, 44, 30) : C(40, 32, 14), done ? C(80, 200, 110) : gold);
            frame(35, y + 6, 8, 8, C(10, 10, 16), done ? C(120, 255, 140) : gold);
            if (done) { line(36, y + 9, 38, y + 11, C(120, 255, 140)); line(38, y + 11, 41, y + 7, C(120, 255, 140)); }
            text(50, y + 4, q->text, done ? C(160, 230, 170) : C(255, 236, 170), 1, AL_LEFT);
            hudBar(50, y + 13, 280, 3, have / (float)q->goal, done ? C(120, 255, 140) : gold);
            sfmt(s, sizeof s, "%d/%d", have, q->goal);
            text(336, y + 12, s, C(220, 210, 180), 1, AL_LEFT);
            if (done) text(FW - 36, y + 8, "DONE!", C(120, 255, 140), 1, AL_RIGHT);
            else { sfmt(s, sizeof s, "+%d", q->reward); text(FW - 36, y + 8, s, C(255, 214, 80), 1, AL_RIGHT); drawCoin(FW - 42 - textWidth(s, 1), y + 10); }
        }
        if (dqDone == 7) text(FW / 2, 106, "ALL 3 DONE - BONUS COLLECTED! COME BACK TOMORROW", C(120, 255, 140), 1, AL_CENTER);
        else { sfmt(s, sizeof s, "FINISH ALL 3 FOR A +%d COIN BONUS", DAILY_BONUS); text(FW / 2, 106, s, gold, 1, AL_CENTER); }
    }
    text(FW / 2, 117, "ALL QUESTS", C_WHITE, 1, AL_CENTER);
    for (int p = 0; p < pages; p++) fillRect(FW / 2 + 172 + p * 7, 243, 4, 4, p == questPage ? ui.hi : C(70, 66, 84));
    sfmt(s, sizeof s, "PAGE %d/%d", questPage + 1, pages);
    text(FW / 2 + 172, 251, s, ui.dim, 1, AL_LEFT);
    for (int q = questPage * QUEST_PAGE; q < QUEST_COUNT && q < (questPage + 1) * QUEST_PAGE; q++) {
        const Quest *Q = &QUESTS[q];
        int y = 126 + (q - questPage * QUEST_PAGE) * 13, done = questMask & (1 << q);
        int have = pstat[Q->stat] < Q->goal ? pstat[Q->stat] : Q->goal;
        frame(30, y, FW - 60, 13, done ? C(20, 44, 30) : C(18, 16, 30), done ? C(80, 200, 110) : scalec(D->neonB, 110));
        frame(35, y + 3, 7, 7, C(10, 10, 16), done ? C(120, 255, 140) : C(120, 120, 140));
        if (done) { line(36, y + 6, 38, y + 8, C(120, 255, 140)); line(38, y + 8, 41, y + 4, C(120, 255, 140)); }
        text(50, y + 4, Q->title, done ? C(160, 230, 170) : C_WHITE, 1, AL_LEFT);
        fillRect(250, y + 5, 80, 4, C(40, 38, 54));
        fillRect(250, y + 5, 80 * have / Q->goal, 4, done ? C(120, 255, 140) : C(255, 200, 70));
        sfmt(s, sizeof s, "%d/%d", have, Q->goal);
        text(336, y + 4, s, C(200, 200, 215), 1, AL_LEFT);
        if (Q->skin >= 0) { sfmt(s, sizeof s, "SKIN: %s", SKINS[Q->skin].name); text(FW - 36, y + 4, s, C(255, 180, 90), 1, AL_RIGHT); }
        else { sfmt(s, sizeof s, "%d", Q->coins); text(FW - 36, y + 4, s, C(255, 214, 80), 1, AL_RIGHT); drawCoin(FW - 42 - textWidth(s, 1), y + 6); }
    }
}

void renderKeys(void)
{
    typedef struct { const char *k[3]; const char *what; } KeyRow;
    static const KeyRow PAD[] = { /* shown once a controller has been used */
        { { "L-STICK", "D-PAD", 0 }, "MOVE, CLIMB ROPES AND WALK THE MENUS" },
        { { "A", 0, 0 }, "JUMP (TWICE TO DOUBLE JUMP). PICKS IN MENUS" },
        { { "DOWN", 0, 0 }, "DROP DOWN THROUGH A LEDGE" },
        { { "B", "LT", 0 }, "DASH. B GOES BACK IN MENUS" },
        { { "R-STICK", 0, 0 }, "AIM" },
        { { "RT", 0, 0 }, "SHOOT - HOLD FOR AUTOMATIC FIRE" },
        { { "X", 0, 0 }, "RELOAD (PLAY AGAIN AFTER YOU DIE)" },
        { { "LB", "RB", "Y" }, "PREVIOUS / NEXT WEAPON. LB / RB FLIP MENU PAGES" },
        { { "START", 0, 0 }, "PAUSE" },
        { { "BACK", 0, 0 }, "MAIN MENU FROM THE PAUSE SCREEN" },
        { { "PS", 0, 0 }, "PLAYSTATION: A = CROSS, B = CIRCLE, X = SQUARE, Y = TRIANGLE" },
    };
    static const KeyRow KEYS[] = {
        { { "A", "D", 0 }, "MOVE LEFT / RIGHT" },
        { { "W", "SPACE", 0 }, "JUMP - PRESS AGAIN IN THE AIR TO DOUBLE JUMP" },
        { { "S", 0, 0 }, "DROP DOWN THROUGH A LEDGE" },
        { { "W", "S", 0 }, "CLIMB UP / DOWN A ROPE" },
        { { "SHIFT", "RMB", 0 }, "DASH - YOU CAN'T BE HIT WHILE DASHING" },
        { { "MOUSE", 0, 0 }, "AIM" },
        { { "LMB", 0, 0 }, "SHOOT - HOLD FOR AUTOMATIC FIRE" },
        { { "R", 0, 0 }, "RELOAD" },
        { { "1-7", 0, 0 }, "PICK A WEAPON (7 = GRAVITY GUN)" },
        { { "Q", "E", "WHEEL" }, "PREVIOUS / NEXT WEAPON" },
        { { "P", "ESC", 0 }, "PAUSE" },
        { { "M", 0, 0 }, "MUSIC ON / OFF" },
        { { "F", "F11", 0 }, "FULLSCREEN ON / OFF" },
        { { "1-3", 0, 0 }, "PICK AN UPGRADE CARD AFTER EACH WAVE" },
    };
    const KeyRow *rows = padSeen ? PAD : KEYS;
    unsigned n = padSeen ? sizeof PAD / sizeof PAD[0] : sizeof KEYS / sizeof KEYS[0];
    text(FW / 2, 8, padSeen ? "CONTROLLER" : "KEYS", C_WHITE, 3, AL_CENTER);
    text(FW / 2, 28, "TIP: FIRE A ROCKET AT YOUR FEET TO ROCKET JUMP. SHOOT RIOT SHIELDS FROM BEHIND OR ABOVE", C(255, 200, 90), 1, AL_CENTER);
    for (unsigned r = 0; r < n; r++) {
        int y = 37 + (int)r * 13, x = 186;
        for (int k = 2; k >= 0; k--) {
            if (!rows[r].k[k]) continue;
            int w = textWidth(rows[r].k[k], 1) + 8;
            x -= w;
            fillRect(x + 1, y + 2, w, 12, C(4, 4, 8));
            frame(x, y, w, 12, C(44, 42, 60), ui.dim);
            fillRect(x + 1, y + 10, w - 2, 1, C(30, 28, 40));
            drawText(x + 4, y + 3, rows[r].k[k], C_WHITE, 1);
            x -= 4;
        }
        text(196, y + 3, rows[r].what, C(210, 210, 225), 1, AL_LEFT);
    }
}

void renderQuit(void)
{
    neonText(FW / 2, 90, "THANKS FOR PLAYING!", ui.titleA, 3, 1);
    text(FW / 2, 124, "YOUR PROGRESS IS SAVED. YOU CAN CLOSE THIS WINDOW NOW.", C(210, 210, 225), 1, AL_CENTER);
}

void renderPerks(void)
{
    static const Spr *const emblem[PK_COUNT] = { &sHeart, &sIcPistol, &sIcSmg, &sIcSmg, &sIcSmg, &sHeart, &sPlayerJump,
                                                 &sPlayerRun1, &sMedkit, &sIcRocket, &sIcLaser, &sShield0, &sPlayerRun2 };
    char s[40];
    for (int i = 0; i < FW * FH; i++) fb[i] = scalec(fb[i], 90);
    neonText(FW / 2, 20, "CHOOSE AN UPGRADE", C(255, 214, 80), 3, 1);
    text(FW / 2, 50, "IT LASTS FOR THE REST OF THIS RUN", C(200, 200, 215), 1, AL_CENTER);
    text(FW / 2, 60, "CLICK A CARD OR PRESS 1, 2 OR 3", ui.dim, 1, AL_CENTER);
    for (int i = 0; i < nBtns; i++) {
        const Btn *b = &btns[i];
        int c = b->action - A_PERK0, id = perkChoice[c], focus = i == focusBtn, cx = b->x + b->w / 2;
        const Perk *P = &PERKS[id];
        int lift = focus ? 3 : 0, y = b->y - lift;
        fillRect(b->x + 3, b->y + 3, b->w, b->h, C(4, 4, 8));
        frame(b->x, y, b->w, b->h, focus ? C(40, 32, 62) : C(20, 18, 32), focus ? C_WHITE : scalec(P->col, 150));
        fillRect(b->x + 1, y + 1, b->w - 2, 3, P->col);
        frame(b->x + 5, y + 8, 11, 11, C(10, 10, 16), ui.dim);
        sfmt(s, sizeof s, "%d", c + 1);
        drawText(b->x + 9, y + 11, s, C_WHITE, 1);
        /* emblem on a glowing disc */
        const Spr *e = emblem[id];
        int sc = e->h > 12 ? 2 : 3;
        fillCircle(cx, y + 38, 24, scalec(P->col, 50));
        fillCircle(cx, y + 38, 20, scalec(P->col, 80));
        if (id == PK_HP || id == PK_VAMP || id == PK_LUCK || id == PK_ARMOR) {
            drawSpriteScaled(e, cx - e->w * sc / 2, y + 38 - e->h * sc / 2, sc);
        } else if (e->h > 12) { /* your own skin doing the move */
            e = skinFrame(skin, id == PK_JUMP ? 3 : id == PK_DASH ? 1 : 2);
            skinOv = skinPal(skin);
            drawSpriteScaled(e, cx - e->w * sc / 2, y + 38 - e->h * sc / 2, sc);
            skinOv = 0;
        } else {
            drawSpriteScaled(e, cx - e->w * sc / 2, y + 38 - e->h * sc / 2, sc);
        }
        if (id == PK_ARMOR) { fillRect(cx + 9, y + 20, 4, 34, C(150, 190, 230)); fillRect(cx + 12, y + 20, 1, 34, C(30, 34, 48)); }
        text(cx, y + 68, P->name, focus ? ui.hi : P->col, 1, AL_CENTER);
        text(cx, y + 81, P->line1, ui.txt, 1, AL_CENTER);
        text(cx, y + 90, P->line2, ui.txt, 1, AL_CENTER);
        if (perk[id] >= P->maxStack) { /* past its max level (infinite upgrades) */
            sfmt(s, sizeof s, "LEVEL %d", perk[id] + 1);
            text(cx, y + 101, s, P->col, 1, AL_CENTER);
        } else
            for (int k = 0; k < P->maxStack; k++) { /* level pips: owned, the one you'd get, the rest */
                int px_ = cx - P->maxStack * 4 + k * 8 + 1;
                unsigned pc = k < perk[id] ? P->col : k == perk[id] ? (((int)(gt * 6) & 1) ? C_WHITE : P->col) : C(50, 48, 64);
                fillRect(px_ - 1, y + 102, 7, 5, C_OUT);
                fillRect(px_, y + 103, 5, 3, pc);
            }
        if (perkWeight(perksTaken) < 1) { /* past the normal 10: this card is weaker */
            sfmt(s, sizeof s, "STRENGTH %d", (int)(perkWeight(perksTaken) * 100 + 0.5f));
            s[strlen(s) + 1] = 0; s[strlen(s)] = '%';
            text(cx, y + 110, s, C(255, 150, 90), 1, AL_CENTER);
        }
        if (focus && fmodf(gt, 0.8f) < 0.55f) text(cx, y + b->h + 6, "CLICK TO TAKE", ui.hi, 1, AL_CENTER);
    }
}

/* the upgrades this run has collected, two columns */
void perkList(int y)
{
    char s[40];
    int n = 0;
    for (int i = 0; i < PK_COUNT; i++) {
        if (!perk[i]) continue;
        if (perk[i] > 1) sfmt(s, sizeof s, "%s X%d", PERKS[i].name, perk[i]); else sfmt(s, sizeof s, "%s", PERKS[i].name);
        int x = n % 2 ? FW / 2 + 10 : FW / 2 - 10;
        fillRect(n % 2 ? x : x - 4, y + (n / 2) * 9 + 1, 3, 3, PERKS[i].col);
        text(n % 2 ? x + 6 : x - 8, y + (n / 2) * 9, s, C(210, 210, 225), 1, n % 2 ? AL_LEFT : AL_RIGHT);
        n++;
    }
}

void renderToast(void)
{
    if (toastT <= 0) return;
    int w = textWidth(toastTxt, 1) + 16, x = FW / 2 - w / 2, y = state == ST_PLAY ? 40 : 250;
    frame(x, y, w, 13, C(20, 18, 30), C(255, 214, 80));
    text(FW / 2, y + 4, toastTxt, ui.hi, 1, AL_CENTER);
}
