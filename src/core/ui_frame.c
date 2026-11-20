/* ui_frame.c - Draws a whole frame, and takes the platform's pictures. */
#include "core.h"

void game_render(void)
{
    char s[64];
    setUi(uiStyle);
    if (state == ST_PLAY || state == ST_PAUSE || state == ST_OVER || state == ST_PERK || ((state == ST_OPTIONS || state == ST_UPGRADES) && optFromPause)) {
        animateLayers();
        composite(0);
        target(fb, FW, FH);
        ambienceBack();
        renderWorld();
        ambienceFront();
        ox = oy = 0;
        /* red screen edge when hit, and a slow pulse when health is low */
        float edge = hurtFlash > 0 ? hurtFlash * 2.4f : 0;
        if (state == ST_PLAY && hp <= 30) edge = fmaxf(edge, 0.25f + 0.2f * sinf(gt * 5));
        if (edge > 0)
            for (int y = 0; y < FH; y++)
                for (int x = 0; x < FW; x++) {
                    int d = x < y ? x : y;
                    d = d < FW - 1 - x ? d : FW - 1 - x;
                    d = d < FH - 1 - y ? d : FH - 1 - y;
                    if (d < 24) pmix(x, y, C(200, 0, 20), clampf(edge, 0, 0.6f) * (1 - d / 24.f));
                }
        renderHud();
        if (state == ST_PAUSE) {
            for (int i = 0; i < FW * FH; i++) fb[i] = scalec(fb[i], 110);
            text(FW / 2, 64, "PAUSED", C_WHITE, 5, AL_CENTER);
            if (perksTaken) { text(FW / 2, 180, "YOUR UPGRADES", C(255, 214, 80), 1, AL_CENTER); perkList(191); }
            text(FW / 2, 97, musicOn ? "M - MUSIC: ON" : "M - MUSIC: OFF", ui.dim, 1, AL_CENTER);
        } else if (state == ST_OVER) {
            int k = overT > 0.5f ? 110 : 256 - (int)(overT * 290);
            for (int i = 0; i < FW * FH; i++) fb[i] = scalec(fb[i], k);
            text(FW / 2, 56, "YOU DIED", C(240, 70, 70), 5, AL_CENTER);
            sfmt(s, sizeof s, "SCORE %d", score);
            text(FW / 2, 96, s, C_WHITE, 2, AL_CENTER);
            if (mode == MODE_ENDLESS) sfmt(s, sizeof s, "%s - ENDLESS WAVE %d", D->name, waveIdx);
            else sfmt(s, sizeof s, "%s - WAVE %d/5", D->name, waveIdx);
            text(FW / 2, 114, s, C(190, 190, 200), 1, AL_CENTER);
            sfmt(s, sizeof s, "+%d COINS", runCoins);
            text(FW / 2 + 5, 128, s, C(255, 214, 80), 1, AL_CENTER);
            drawCoin(FW / 2 - textWidth(s, 1) / 2 - 4, 130);
            sfmt(s, sizeof s, "KILLS %d     BEST COMBO %d     UPGRADES %d", runKills, bestCombo, perksTaken);
            text(FW / 2, 142, s, C(170, 200, 230), 1, AL_CENTER);
            if (newHi && fmodf(gt, 0.6f) < 0.4f) text(FW / 2, 160, "NEW BEST!", C(255, 220, 90), 2, AL_CENTER);
        }
    } else {
        ox = oy = 0;
        if (state == ST_MAPSEL) { /* preview the district under the cursor */
            layoutUI();
            int a = focusBtn < nBtns ? btns[focusBtn].action - A_MAP0 : -1;
            if (a >= 0 && a < D_COUNT && curMap != a) loadDistrict(a);
        }
        int showPic = state != ST_MAPSEL && menuBg == BG_PICTURE && ownsMap(D_ORBIT);
        if (state == ST_MENUBG) { /* show the card under the cursor behind the screen */
            layoutUI();
            int a = focusBtn < nBtns ? btns[focusBtn].action - A_BG0 : -1;
            int m = a == 0 ? startMap : a >= BG_FIXED && a < BG_CHOICES ? a - BG_FIXED : -1;
            if (m >= 0 && curMap != m) loadDistrict(m);
            if (a == 2) showPic = 1;
            else if (m >= 0) showPic = 0;
        }
        if (showPic && picOk) { /* the picture instead of a district, a little less dimmed */
            int k = state == ST_MENU ? 190 : 120;
            for (int i = 0; i < FW * FH; i++) fb[i] = scalec(pic[i], k);
            target(fb, FW, FH);
        } else backdrop(state == ST_MENU ? 150 : 95);
    }
    layoutUI();
    switch (state) {
    case ST_MENU: renderMainMenu(); break;
    case ST_MAPSEL: renderMapSelect(); break;
    case ST_SKINS: renderSkins(); break;
    case ST_QUESTS: renderQuests(); break;
    case ST_KEYS: renderKeys(); break;
    case ST_QUIT: renderQuit(); break;
    case ST_PERK: renderPerks(); break;
    case ST_OPTIONS: case ST_UPGRADES: renderOptions(); break;
    case ST_PETEDIT: renderPetEdit(); break;
    case ST_MENUBG: renderMenuBg(); break;
    case ST_UISTYLE: renderUiStyle(); break;
    case ST_TITLE: renderTitleEdit(); break;
    case ST_SONGS: renderSongs(); break;
    case ST_ACH: renderAchievements(); break;
    case ST_WIPE: renderWipe(); break;
    case ST_EXTRA: renderExtra(); break;
    default: break;
    }
    if (devMode && state != ST_PLAY) text(6, 6, "DEV MODE", C(255, 90, 90), 1, AL_LEFT);
    if (state != ST_MAPSEL && state != ST_MENUBG && state != ST_UISTYLE && state != ST_SKINS && state != ST_PLAY && state != ST_PERK && state != ST_OPTIONS && state != ST_UPGRADES && state != ST_PETEDIT && state != ST_QUESTS && state != ST_TITLE && state != ST_SONGS && state != ST_ACH && state != ST_WIPE && state != ST_EXTRA)
        for (int i = 0; i < nBtns; i++) drawButton(&btns[i], i == focusBtn);
    if ((state == ST_MENU || state == ST_OPTIONS || state == ST_SONGS) && uiScrollMax > 0) drawScrollBar();
    renderToast();
    if (!bot) crosshair();
}

const unsigned *game_framebuffer(void) { return fb; }
unsigned *game_picture(int k) { return pics[k >= 0 && k < PIC_N ? k : 0]; }
void game_picture_ready(int k)
{
    if (k < 0 || k >= PIC_N) return;
    picsOk[k] = 1;
    picBuilt[k] = 0;
    if (k == PIC_ORBIT) picOk = 1;
    if (mapPic(curMap) == k) loadDistrict(curMap); /* redraw that map from its picture */
}
