/* ui_screens_extras.c - Menu screens: achievements and the RESET EVERYTHING warnings. */
#include "core.h"

void renderAchievements(void)
{
    char s[64];
    int pages = (ACH_COUNT + ACH_PAGE - 1) / ACH_PAGE;
    const unsigned gold = C(255, 205, 60), green = C(120, 255, 140);
    text(FW / 2, 4, "ACHIEVEMENTS", C_WHITE, 3, AL_CENTER);
    sfmt(s, sizeof s, "%d OF %d DONE - EACH PAYS ITS COINS ONCE", achCount(), ACH_COUNT);
    text(FW / 2, 24, s, ui.dim, 1, AL_CENTER);
    for (int i = 0; i < nBtns; i++) drawButton(&btns[i], i == focusBtn);
    coinsCorner();
    for (int n = achPage * ACH_PAGE; n < ACH_COUNT && n < (achPage + 1) * ACH_PAGE; n++) {
        int a = achAt(n);
        const Achievement *A = &ACHS[a];
        int y = 34 + (n - achPage * ACH_PAGE) * 20, done = achDone(a), hidden = A->secret && !done;
        frame(30, y, FW - 60, 18, done ? C(20, 44, 30) : C(18, 16, 30), done ? C(80, 200, 110) : scalec(D->neonB, 110));
        frame(35, y + 5, 8, 8, C(10, 10, 16), done ? green : C(120, 120, 140));
        if (done) { line(36, y + 8, 38, y + 10, green); line(38, y + 10, 41, y + 6, green); }
        text(50, y + 3, hidden ? "???" : A->name, done ? C(160, 230, 170) : a == ACH_ALL ? gold : C_WHITE, 1, AL_LEFT);
        text(50, y + 11, hidden ? xs(a == ACH_S1 ? 7 : 8) : A->what, done ? C(110, 170, 120) : ui.dim, 1, AL_LEFT);
        if (done) text(FW - 36, y + 7, "DONE!", green, 1, AL_RIGHT);
        else { sfmt(s, sizeof s, "%d", A->coins); text(FW - 36, y + 7, s, C(255, 214, 80), 1, AL_RIGHT); drawCoin(FW - 42 - textWidth(s, 1), y + 9); }
    }
    for (int p = 0; p < pages; p++) fillRect(FW / 2 + 172 + p * 7, 243, 4, 4, p == achPage ? ui.hi : C(70, 66, 84));
    sfmt(s, sizeof s, "PAGE %d/%d", achPage + 1, pages);
    text(FW / 2 + 172, 251, s, ui.dim, 1, AL_LEFT);
    text(FW / 2 - 205, 243, "Q / E", C(110, 110, 130), 1, AL_LEFT);
}


/* RESET EVERYTHING: two red warning screens before your progress is wiped */
void renderWipe(void)
{
    static const char *const gone[] = { "ALL YOUR COINS", "EVERY SKIN, ENEMY SKIN, PET AND MAP YOU BOUGHT OR UNLOCKED",
                                        "ALL QUESTS, DAILY QUESTS AND ACHIEVEMENTS", "YOUR BEST SCORES AND RECORDS",
                                        "YOUR MENU TITLE AND EVERY SETTING" };
    char s[48];
    int last = wipeStep == 2, left = WIPE_WAIT - (int)(gt - wipeT), flash = fmodf(gt, 0.6f) < 0.35f;
    for (int i = 0; i < FW * FH; i++) fb[i] = mixc(fb[i], C(60, 0, 8), 0.75f);
    for (int x = -FH; x < FW; x += 24) /* hazard stripes along the top and bottom */
        for (int t = 0; t < 10; t++) { line(x + t, 0, x + t + 10, 10, C(200, 30, 40)); line(x + t, FH - 11, x + t + 10, FH - 1, C(200, 30, 40)); }
    neonText(FW / 2, 18, last ? "LAST WARNING" : "WARNING", last && flash ? C_WHITE : C(255, 60, 70), 4, 1);
    if (!last) {
        text(FW / 2, 58, "RESET EVERYTHING STARTS THE GAME OVER FROM ZERO. THIS DELETES:", C(255, 210, 210), 1, AL_CENTER);
        for (int i = 0; i < 5; i++) {
            fillRect(FW / 2 - 150, 76 + i * 16, 4, 4, C(255, 70, 80));
            text(FW / 2 - 140, 75 + i * 16, gone[i], C_WHITE, 1, AL_LEFT);
        }
        text(FW / 2, 166, "ONLY YOUR OWN SONGS (MY SONGS) STAY", C(160, 220, 255), 1, AL_CENTER);
        text(FW / 2, 182, "IT CAN'T BE UNDONE", flash ? C(255, 90, 90) : C(255, 160, 160), 2, AL_CENTER);
        text(FW / 2, 200, "JUST WANT DEFAULT SETTINGS? USE RESET SETTINGS INSTEAD - IT KEEPS EVERYTHING ELSE", C(255, 200, 120), 1, AL_CENTER);
    } else {
        text(FW / 2, 66, "ARE YOU REALLY SURE?", C_WHITE, 2, AL_CENTER);
        text(FW / 2, 92, "ALL YOUR COINS, UNLOCKS, QUESTS, ACHIEVEMENTS AND RECORDS", C(255, 210, 210), 1, AL_CENTER);
        text(FW / 2, 104, "WILL BE GONE FOR GOOD. THERE IS NO WAY TO GET THEM BACK.", C(255, 210, 210), 1, AL_CENTER);
        sfmt(s, sizeof s, "%d COINS", coins);
        text(FW / 2, 128, s, C(255, 214, 80), 2, AL_CENTER);
        text(FW / 2, 148, "WILL BECOME 0", C(255, 120, 120), 1, AL_CENTER);
        text(FW / 2, 182, left > 0 ? "READ IT ONCE MORE - THE DELETE BUTTON UNLOCKS IN A MOMENT" : "LAST CHANCE: CANCEL KEEPS EVERYTHING",
             C(255, 200, 120), 1, AL_CENTER);
    }
    for (int i = 0; i < nBtns; i++) {
        const Btn *b = &btns[i];
        int focus = i == focusBtn;
        if (b->action == A_BACK) { drawButton(b, focus); continue; }
        int locked = b->action == A_WIPE_DO && left > 0;
        frame(b->x, b->y, b->w, b->h, locked ? C(40, 20, 24) : focus ? C(140, 20, 30) : C(90, 14, 22), locked ? C(110, 60, 66) : focus ? C_WHITE : C(255, 70, 80));
        if (locked) sfmt(s, sizeof s, "WAIT %d...", left);
        else sfmt(s, sizeof s, "%s", b->label);
        int sc = textWidth(s, 2) <= b->w - 8 ? 2 : 1; /* big like the other button, when it fits */
        text(b->x + b->w / 2, b->y + (b->h - 5 * sc) / 2 + 1, s, locked ? C(150, 110, 116) : C_WHITE, sc, AL_CENTER);
    }
}

/* the monitor (82 x 60): what is on it gets the look of an old tube - every other line darker, the whole
   screen flickering a little, a bright band rolling down and now and then a line pushed sideways */
static unsigned tubeLine(unsigned c, int y, int roll)
{
    int k = (y & 1 ? 190 : 255) * (230 + (int)(hash3((int)(gt * 16), 4, 4) % 26)) / 255 + (y >= roll && y < roll + 8 ? 22 : 0);
    return C(clampi((int)(c & 255) * k / 255, 0, 255), clampi((int)((c >> 8) & 255) * k / 255, 0, 255), clampi((int)((c >> 16) & 255) * k / 255, 0, 255));
}
#define TV_X 67
#define TV_Y 81
#define TV_W 82
#define TV_H 60
/* extra picture k (each one is already cut to the screen shape); builds without the private pictures show a message instead */
static void tvPicture(int k, int roll)
{
    const unsigned *src = picsOk[PIC_X + k] ? pics[PIC_X + k] : 0;
    int tear = hash3((int)(gt * 5), 9, 2) % 6 == 0 ? (int)(hash3((int)(gt * 5), 9, 3) % TV_H) : -99;
    for (int y = 0; y < TV_H; y++) {
        int shift = y >= tear && y < tear + 2 ? 2 : 0;
        int sy0 = y * FH / TV_H, sy1 = (y + 1) * FH / TV_H;
        if (sy1 <= sy0) sy1 = sy0 + 1;
        for (int x = 0; x < TV_W; x++) {
            unsigned c = C(14, 14, 22);
            if (src) { /* the average of the picture pixels under this screen pixel */
                int xs = clampi(x + shift, 0, TV_W - 1), sx0 = xs * FW / TV_W, sx1 = (xs + 1) * FW / TV_W, r = 0, g = 0, b = 0, n = 0;
                for (int j = sy0; j < sy1; j++)
                    for (int i = sx0; i < sx1; i++) { unsigned p = src[j * FW + i]; r += p & 255; g += (p >> 8) & 255; b += (p >> 16) & 255; n++; }
                c = C(r / n, g / n, b / n);
            }
            pset(TV_X + x, TV_Y + y, tubeLine(c, y, roll));
        }
    }
    if (!src) text(TV_X + TV_W / 2, TV_Y + TV_H / 2 - 2, xs(21), C(200, 200, 210), 1, AL_CENTER);
}
static void tvCode(int roll)
{
    for (int y = 0; y < TV_H; y++)
        for (int x = 0; x < TV_W; x++) pset(TV_X + x, TV_Y + y, C(8, 16, 10));
    for (int r = 0; r < 9; r++) {
        unsigned h = hash3(r + (int)(gt * 3), 3, 5);
        int ind = (int)(h % 3) * 6, len = 12 + (int)(h >> 4) % 40;
        fillRect(TV_X + 5 + ind, TV_Y + 4 + r * 6, len, 2, (h >> 9) % 4 == 0 ? C(255, 214, 80) : (h >> 9) % 4 == 1 ? C(0, 230, 255) : C(80, 230, 110));
    }
    if (fmodf(gt, 1.0f) < 0.5f) fillRect(TV_X + 5, TV_Y + 4 + 9 * 6, 4, 3, C(80, 230, 110)); /* the cursor */
    for (int y = 0; y < TV_H; y++)
        for (int x = 0; x < TV_W; x++) pset(TV_X + x, TV_Y + y, tubeLine(fb[(TV_Y + y) * FW + TV_X + x], y, roll));
}

/* the extra screen: who made the game, and your own numbers */
void renderExtra(void)
{
    char s[64];
    const int cx = EXTRA_CX;
    for (int i = 0; i < FW * FH; i++) fb[i] = mixc(fb[i], C(6, 4, 16), 0.85f);
    neonText(FW / 2, 6, xs(9), C(0, 230, 255), 3, 1);
    /* a desk with a flickering monitor (a click changes what it shows), your agent at it, your pet asleep under it */
    int hover = extraTvAt(mx, my), roll = (int)fmodf(gt * 24, (float)(TV_H + 20)) - 10;
    frame(TV_X - 1, TV_Y - 1, TV_W + 2, TV_H + 2, C(8, 16, 10), hover ? C(255, 90, 200) : C(110, 116, 136));
    if (gt - extraTvT < 0.15f) /* switching over: a moment of grey lines */
        for (int y = 0; y < TV_H; y++) fillRect(TV_X, TV_Y + y, TV_W, 1, scalec(C(200, 200, 210), 40 + (int)(hash3(y, (int)(gt * 60), 7) % 180)));
    else if (extraTv) tvPicture(extraTv - 1, roll);
    else tvCode(roll);
    fillRect(104, 142, 8, 8, C(70, 74, 88));
    fillRect(56, 150, 104, 5, C(110, 80, 56)); fillRect(56, 155, 104, 2, C(70, 50, 36));
    fillRect(62, 157, 4, 26, C(70, 50, 36)); fillRect(150, 157, 4, 26, C(70, 50, 36));
    fillRect(70, 146, 26, 4, C(40, 40, 52)); /* keyboard */
    skinOv = skinPal(skin);
    drawSkinScaled(skinFrame(skin, 0), 44, 183 + ((int)(gt * 5) & 1), 2);
    skinOv = 0;
    fillRect(0, 183, 176, 1, C(70, 66, 84));
    if (pet) drawPet(pet, 120, 183, 1, 0, 2);
    if (pet && fmodf(gt, 2.0f) < 1.4f) text(136, 160 - (int)(fmodf(gt, 2.0f) * 6), "Z", C(160, 200, 255), 1, AL_LEFT);
    /* who made it */
    text(cx, 34, xs(10), C(255, 214, 80), 2, AL_CENTER);
    text(cx, 56, xs(11), ui.dim, 1, AL_CENTER);
    neonText(cx, 66, xs(12), C(255, 60, 200), 3, 1);
    text(cx, 90, xs(13), C(0, 230, 255), 2, AL_CENTER);
    int link = extraLinkAt(mx, my); /* a link: brighter under the mouse */
    text(cx, 110, xs(14), link ? C(200, 255, 210) : C(120, 255, 140), 1, AL_CENTER);
    fillRect(cx - textWidth(xs(14), 1) / 2, 117, textWidth(xs(14), 1), 1, link ? C(200, 255, 210) : C(60, 150, 80));
    text(cx, 130, xs(15), C_WHITE, 1, AL_CENTER);
    text(cx, 140, xs(16), C_WHITE, 1, AL_CENTER);
    text(cx, 150, xs(17), ui.dim, 1, AL_CENTER);
    sfmt(s, sizeof s, xs(18), pstat[P_KILLS], pstat[P_BOSSES], pstat[P_WAVES]);
    text(cx, 168, s, C(255, 200, 120), 1, AL_CENTER);
    sfmt(s, sizeof s, xs(19), achCount(), ACH_COUNT, pstat[P_BESTSCORE]);
    text(cx, 178, s, C(255, 200, 120), 1, AL_CENTER);
    text(FW / 2, 206, xs(20), ((int)(gt * 3) & 1) ? C(255, 90, 200) : C(0, 230, 255), 2, AL_CENTER);
    for (int i = 0; i < nBtns; i++) drawButton(&btns[i], i == focusBtn);
}
