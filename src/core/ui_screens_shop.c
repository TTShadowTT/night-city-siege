/* ui_screens_shop.c - Menu screens: main menu, map select, menu background, UI style, skins and pet editor. */
#include "core.h"

void renderMainMenu(void)
{
    char s[64];
    int top = menuBtnTop();
    drawMenuTitle(scrY(10));
    /* your current skin, standing guard */
    skinOv = skinPal(skin);
    drawSkinScaled(skinFrame(skin, 0), 390, scrY(112) + 60, 3);
    skinOv = 0;
    text(390, scrY(178), SKINS[skin].name, C(200, 200, 215), 1, AL_CENTER);
    if (pet) { /* and your pet beside you, shuffling about (its EDIT button sits underneath) */
        float t = fmodf(gt, 4.0f);
        int hop = t > 3.4f ? (int)(sinf((t - 3.4f) / 0.6f * PI) * 8) : 0;
        if (petFlies(pet)) drawPet(pet, 436, scrY(150) + (int)(sinf(gt * 3) * 3), 1, petFlap(gt * 12), 2); /* flyers hover */
        else drawPet(pet, 432, scrY(172) - hop, 1, hop ? 1 : ((int)(gt * 2) & 1) * 2, 2);
        if (petName(pet) != PETS[pet].name) { /* the name you gave it, above the pet - clear of its bob or hop, whatever its size */
            int h = PET_SHAPES[PETS[pet].shape].h * 2;
            text(petFlies(pet) ? 436 : 432, petFlies(pet) ? scrY(150) - h - 11 : scrY(172) - h - 16, petName(pet), C(200, 200, 215), 1, AL_CENTER);
        }
    }
    enemyLook(eskin, 1);
    {
        const Spr *t = enemyBodySpr(EB_THUG, (int)(gt * 7) & 1, ((int)(gt * 7) & 1) ? &sThug1 : &sThug0);
        const unsigned *keep = skinOv;
        if (eBody) skinOv = eBody->pal;
        drawSpriteScaled(t, 72 - t->w * 3 / 2, scrY(115) + 57 - t->h * 3, 3); /* standing on the same spot whatever its size */
        skinOv = keep;
    }
    enemyLook(0, 0);
    { /* daily quest badge beside the QUESTS button */
        int n = (dqDone & 1) + (dqDone >> 1 & 1) + (dqDone >> 2 & 1);
        sfmt(s, sizeof s, "DAILY %d/3", n);
        text(FW / 2 + 96, scrY(top + 3 * menuStep() + 7), s, n == 3 ? C(120, 255, 140) : ((int)(gt * 3) & 1) ? ui.hi : C(255, 180, 60), 1, AL_LEFT);
    }
    sfmt(s, sizeof s, "%d/%d", achCount(), ACH_COUNT); /* achievements done, under their button */
    text(56, scrY(200), s, achCount() == ACH_COUNT ? C(120, 255, 140) : C(255, 214, 80), 1, AL_CENTER);
    sfmt(s, sizeof s, "BEST SCORE %d", pstat[P_BESTSCORE]); /* in the bottom corners, so the buttons get the middle */
    text(6, scrY(FH - 9), s, C(170, 170, 190), 1, AL_LEFT);
    sfmt(s, sizeof s, "BEST ENDLESS WAVE %d", pstat[P_ENDLESS]);
    text(FW - 6, scrY(FH - 9), s, C(170, 170, 190), 1, AL_RIGHT);
    coinsCorner();
}

void renderMapSelect(void)
{
    char s[64];
    text(FW / 2, 10, mode == MODE_ENDLESS ? "ENDLESS" : "CAMPAIGN", C_WHITE, 3, AL_CENTER);
    text(FW / 2, 32, mode == MODE_ENDLESS ? "THE WAVES NEVER STOP - HOW FAR CAN YOU GET?" : "5 WAVES PER DISTRICT, THEN ON TO THE NEXT ONE",
         C(180, 200, 220), 1, AL_CENTER);
    text(FW / 2, 44, "CHOOSE A DISTRICT", ui.hi, 1, AL_CENTER);
    for (int i = 0; i < nBtns; i++) {
        const Btn *b = &btns[i];
        if (b->action < A_MAP0 || b->action >= A_MAP0 + D_COUNT) { drawButton(b, i == focusBtn); continue; }
        int d = b->action - A_MAP0, locked = !ownsMap(d), cx = b->x + b->w / 2, focus = i == focusBtn;
        panel(b->x, b->y, b->w, b->h, focus);
        drawMapPreview(d, b->x + 4, b->y + 4, b->w - 8, 60, locked);
        text(cx, b->y + 70, DISTRICTS[d].name, focus ? ui.hi : C_WHITE, 1, AL_CENTER);
        sfmt(s, sizeof s, "DISTRICT %d", d + 1);
        text(cx, b->y + 79, s, ui.dim, 1, AL_CENTER);
        if (locked) {
            drawLock(cx, b->y + 34);
            sfmt(s, sizeof s, "%d", MAP_COST[d]);
            text(cx + 4, b->y + 94, s, C(255, 214, 80), 1, AL_CENTER);
            drawCoin(cx - 4 - textWidth(s, 1) / 2, b->y + 96);
            text(cx, b->y + 106, "OR REACH IT IN", ui.dim, 1, AL_CENTER);
            text(cx, b->y + 113, "THE CAMPAIGN", ui.dim, 1, AL_CENTER);
            if (focus) text(cx, b->y + 123, coins >= MAP_COST[d] ? "CLICK TO BUY" : "LOCKED", coins >= MAP_COST[d] ? C(120, 255, 140) : C(230, 90, 90), 1, AL_CENTER);
        } else {
            text(cx, b->y + 94, "UNLOCKED", C(120, 255, 140), 1, AL_CENTER);
            if (focus && fmodf(gt, 0.8f) < 0.55f) text(cx, b->y + 116, "CLICK TO PLAY", ui.hi, 1, AL_CENTER);
        }
    }
    {
        int pages = (D_COUNT + MAP_PAGE - 1) / MAP_PAGE;
        for (int p = 0; p < pages; p++) fillRect(FW / 2 + 172 + p * 7, 218, 4, 4, p == mapPage ? ui.hi : C(70, 66, 84));
        sfmt(s, sizeof s, "PAGE %d/%d", mapPage + 1, pages);
        text(FW / 2 + 172, 226, s, ui.dim, 1, AL_LEFT);
        text(FW / 2 - 205, 218, "Q / E", C(110, 110, 130), 1, AL_LEFT);
    }
    if (focusBtn < nBtns && btns[focusBtn].action >= A_MAP0 && btns[focusBtn].action < A_MAP0 + D_COUNT)
        text(FW / 2, 201, DISTRICTS[btns[focusBtn].action - A_MAP0].blurb, C(200, 200, 215), 1, AL_CENTER);
    coinsCorner();
}

void renderMenuBg(void)
{
    char s[64];
    int pages = (BG_CHOICES + MAP_PAGE - 1) / MAP_PAGE;
    text(FW / 2, 10, "MENU BACKGROUND", C_WHITE, 3, AL_CENTER);
    text(FW / 2, 32, "PICK THE MAP BEHIND THE MAIN MENU - LOCKED MAPS CAN BE BOUGHT HERE", C(180, 200, 220), 1, AL_CENTER);
    for (int i = 0; i < nBtns; i++) {
        const Btn *b = &btns[i];
        if (b->action < A_BG0 || b->action >= A_BG_END) { drawButton(b, i == focusBtn); continue; }
        int k = b->action - A_BG0, m = k - BG_FIXED, cx = b->x + b->w / 2, focus = i == focusBtn;
        int buy = m >= 0 ? m : k == 2 ? D_ORBIT : -1; /* the picture is locked until you own the DAVID AND LUCY map */
        int sel = k == bgCard() && (k != 2 || ownsMap(D_ORBIT)), locked = buy >= 0 && !ownsMap(buy);
        panel(b->x, b->y, b->w, b->h, focus);
        if (m >= 0) drawMapPreview(m, b->x + 4, b->y + 4, b->w - 8, 60, locked);
        else if (k == 0) drawMapPreview(startMap, b->x + 4, b->y + 4, b->w - 8, 60, 0);
        else if (k == 2) { /* the picture, shrunk to fit the card */
            for (int y = 0; y < 60; y++)
                for (int x = 0; x < b->w - 8; x++)
                    pset(b->x + 4 + x, b->y + 4 + y, picOk ? pic[(y * FH / 60) * FW + x * FW / (b->w - 8)] : C(20, 18, 30));
        } else { /* RANDOM: a question mark over a strip of every map's sky */
            for (int x = 0; x < b->w - 8; x++) {
                const District *dd = &DISTRICTS[x * D_COUNT / (b->w - 8)];
                for (int y = 0; y < 60; y++) pset(b->x + 4 + x, b->y + 4 + y, mixc(dd->skyTop, dd->skyBot, y / 60.f));
            }
            text(cx, b->y + 18, "?", C_WHITE, 5, AL_CENTER);
        }
        if (sel) { /* green outline around the chosen one */
            unsigned g = C(120, 255, 140);
            fillRect(b->x + 3, b->y + 3, b->w - 6, 1, g); fillRect(b->x + 3, b->y + 64, b->w - 6, 1, g);
            fillRect(b->x + 3, b->y + 3, 1, 62, g); fillRect(b->x + b->w - 4, b->y + 3, 1, 62, g);
        }
        text(cx, b->y + 70, b->label, focus ? ui.hi : C_WHITE, 1, AL_CENTER);
        if (k == 0) text(cx, b->y + 79, "YOUR LAST MAP", ui.dim, 1, AL_CENTER);
        else if (k == 1) text(cx, b->y + 79, "NEW EACH TIME", ui.dim, 1, AL_CENTER);
        else if (k == 2) text(cx, b->y + 79, "DAVID AND LUCY", ui.dim, 1, AL_CENTER);
        if (sel) text(cx, b->y + 94, "SELECTED", C(120, 255, 140), 1, AL_CENTER);
        else if (locked) {
            drawLock(cx, b->y + 34);
            sfmt(s, sizeof s, "%d", MAP_COST[buy]);
            text(cx + 4, b->y + 94, s, C(255, 214, 80), 1, AL_CENTER);
            drawCoin(cx - 4 - textWidth(s, 1) / 2, b->y + 96);
            if (k == 2) text(cx, b->y + 104, "WITH THE MAP", ui.dim, 1, AL_CENTER);
            if (focus) text(cx, b->y + 114, coins >= MAP_COST[buy] ? "CLICK TO BUY" : "NOT ENOUGH", coins >= MAP_COST[buy] ? C(120, 255, 140) : C(230, 90, 90), 1, AL_CENTER);
        } else {
            text(cx, b->y + 94, m >= 0 ? "OWNED" : "", C(160, 220, 255), 1, AL_CENTER);
            if (focus) text(cx, b->y + 112, "CLICK TO USE", ui.hi, 1, AL_CENTER);
        }
    }
    for (int p = 0; p < pages; p++) fillRect(FW / 2 + 172 + p * 7, 218, 4, 4, p == bgPage ? ui.hi : C(70, 66, 84));
    sfmt(s, sizeof s, "PAGE %d/%d", bgPage + 1, pages);
    text(FW / 2 + 172, 226, s, ui.dim, 1, AL_LEFT);
    text(FW / 2 - 205, 218, "Q / E", C(110, 110, 130), 1, AL_LEFT);
    coinsCorner();
}

/* UI STYLE screen: each card is drawn in its own style - a sample button and the style's colours */
void renderUiStyle(void)
{
    char s[32];
    int pages = (UI_COUNT + UI_PAGE - 1) / UI_PAGE;
    text(FW / 2, 8, "UI STYLE", C_WHITE, 3, AL_CENTER);
    text(FW / 2, 30, "HOW MENUS AND BUTTONS LOOK - EVERY STYLE IS FREE", ui.dim, 1, AL_CENTER);
    for (int i = 0; i < nBtns; i++) {
        const Btn *b = &btns[i];
        if (b->action < A_UI0 || b->action >= A_UI0 + UI_COUNT) { drawButton(b, i == focusBtn); continue; }
        int k = b->action - A_UI0, focus = i == focusBtn;
        setUi(k);
        panel(b->x, b->y, b->w, b->h, focus);
        text(b->x + b->w / 2, b->y + 5, b->label, focus ? ui.hi : ui.titleA, 1, AL_CENTER);
        Btn sample = { b->x + 8, b->y + 15, 54, 12, -1, "PLAY" };
        panel(sample.x, sample.y, sample.w, sample.h, 1);
        text(sample.x + sample.w / 2, sample.y + 4, sample.label, ui.hi, 1, AL_CENTER);
        const unsigned sw[6] = { ui.titleA, ui.titleB, ui.hi, ui.edge, ui.txt, ui.fillHi };
        for (int c = 0; c < 6; c++) fillRect(b->x + 70 + c % 3 * 9, b->y + 15 + c / 3 * 7, 7, 5, sw[c]);
        if (k == uiStyle) text(b->x + b->w / 2, b->y + 32, "SELECTED", C(120, 255, 140), 1, AL_CENTER);
        else if (focus) text(b->x + b->w / 2, b->y + 32, "CLICK TO USE", ui.hi, 1, AL_CENTER);
        setUi(uiStyle);
    }
    for (int p = 0; p < pages; p++) fillRect(FW / 2 + 172 + p * 7, 218, 4, 4, p == uiPage ? ui.hi : C(70, 66, 84));
    sfmt(s, sizeof s, "PAGE %d/%d", uiPage + 1, pages);
    text(FW / 2 + 172, 226, s, ui.dim, 1, AL_LEFT);
    text(FW / 2 - 205, 218, "Q / E", C(110, 110, 130), 1, AL_LEFT);
}

void renderSkins(void)
{
    char s[64];
    text(FW / 2, 6, "SKINS", C_WHITE, 3, AL_CENTER);
    for (int i = 0; i < nBtns; i++) {
        const Btn *b = &btns[i];
        int focus = i == focusBtn, cx = b->x + b->w / 2;
        if (b->action == A_TAB_PLAYER || b->action == A_TAB_ENEMY || b->action == A_TAB_CURSOR || b->action == A_TAB_PETS) {
            drawTab(b, (b->action == A_TAB_ENEMY ? 1 : b->action == A_TAB_CURSOR ? 2 : b->action == A_TAB_PETS ? 3 : 0) == skinTab, focus);
            continue;
        }
        if (b->action >= A_PET0 && b->action < A_PET_END) {
            int k = b->action - A_PET0, owned = ownsPet(k);
            panel(b->x, b->y, b->w, b->h, focus);
            if (k == 0) { /* NO PET: an empty spot */
                for (int i = -8; i <= 8; i++) { pset(cx + i, b->y + 28 + i, C(230, 90, 90)); pset(cx + i, b->y + 28 - i, C(230, 90, 90)); }
            } else {
                int walk = petFlies(k) ? petFlap(gt * 12) : focus ? 1 + ((int)(gt * 6) & 1) : 0;
                drawPet(k, cx, b->y + 48, 0, walk, PET_SHAPES[PETS[k].shape].h > 8 ? 3 : 4 - (PET_SHAPES[PETS[k].shape].w > 10));
            }
            if (!owned) {
                for (int j = 0; j < 48; j++)
                    for (int x = 0; x < b->w - 4; x++) pdark(b->x + 2 + x, b->y + 3 + j, 120);
                drawLock(cx, b->y + 30);
            }
            text(cx, b->y + 54, petName(k), focus ? ui.hi : C_WHITE, 1, AL_CENTER);
            if (k == pet) text(cx, b->y + 64, "EQUIPPED", C(120, 255, 140), 1, AL_CENTER);
            else if (owned) { text(cx, b->y + 64, k ? "OWNED" : "FREE", C(160, 220, 255), 1, AL_CENTER); if (focus) text(cx, b->y + 78, "CLICK TO USE", ui.hi, 1, AL_CENTER); }
            else {
                sfmt(s, sizeof s, "%d", PETS[k].cost);
                text(cx + 4, b->y + 64, s, C(255, 214, 80), 1, AL_CENTER);
                drawCoin(cx - 4 - textWidth(s, 1) / 2, b->y + 66);
                if (focus) text(cx, b->y + 78, coins >= PETS[k].cost ? "CLICK TO ADOPT" : "NOT ENOUGH", coins >= PETS[k].cost ? C(120, 255, 140) : C(230, 90, 90), 1, AL_CENTER);
            }
            continue;
        }
        if (b->action >= A_CUR0 && b->action < A_CUR_END) {
            int k = b->action - A_CUR0;
            panel(b->x, b->y, b->w, b->h, focus);
            fillRect(b->x + 8, b->y + 6, b->w - 16, 44, C(14, 12, 22)); /* a dark window so every cursor reads */
            if (k == 8 || k == CUR_SWORD) drawCursor(k, cx - 9, b->y + 17, 2); /* the arrow's and sword's hot spot is the tip, not the middle */
            else drawCursor(k, cx - 1, b->y + 27, k == 3 ? 2 : k >= 26 || k == 12 ? 4 : 3); /* the sniper scope is too wide for x3, the tiny ones get x4 */
            text(cx, b->y + 54, CURSORS[k], focus ? ui.hi : C_WHITE, 1, AL_CENTER);
            if (k == cursorSkin) text(cx, b->y + 64, "EQUIPPED", C(120, 255, 140), 1, AL_CENTER);
            else text(cx, b->y + 64, focus ? "CLICK TO USE" : "FREE", focus ? ui.hi : C(160, 220, 255), 1, AL_CENTER);
            continue;
        }
        if (b->action >= A_SKIN0 && b->action < A_SKIN0 + SKIN_COUNT) {
            int k = b->action - A_SKIN0, owned = ownsSkin(k);
            panel(b->x, b->y, b->w, b->h, focus);
            skinOv = skinPal(k);
            drawSkinScaled(skinFrame(k, focus && ((int)(gt * 6) & 1) ? 1 : 0), cx, b->y + 50, 2);
            skinOv = 0;
            if (!owned) {
                for (int j = 0; j < 48; j++)
                    for (int x = 0; x < b->w - 4; x++) pdark(b->x + 2 + x, b->y + 3 + j, 120);
                drawLock(cx, b->y + 30);
            }
            text(cx, b->y + 54, SKINS[k].name, focus ? ui.hi : C_WHITE, 1, AL_CENTER);
            if (k == skin) text(cx, b->y + 64, "EQUIPPED", C(120, 255, 140), 1, AL_CENTER);
            else if (owned) { text(cx, b->y + 64, "OWNED", C(160, 220, 255), 1, AL_CENTER); if (focus) text(cx, b->y + 78, "CLICK TO WEAR", ui.hi, 1, AL_CENTER); }
            else if (SKINS[k].cost > 0) {
                sfmt(s, sizeof s, "%d", SKINS[k].cost);
                text(cx + 4, b->y + 64, s, C(255, 214, 80), 1, AL_CENTER);
                drawCoin(cx - 4 - textWidth(s, 1) / 2, b->y + 66);
                if (focus) text(cx, b->y + 78, coins >= SKINS[k].cost ? "CLICK TO BUY" : "NOT ENOUGH", coins >= SKINS[k].cost ? C(120, 255, 140) : C(230, 90, 90), 1, AL_CENTER);
            } else {
                text(cx, b->y + 64, "QUEST:", C(255, 180, 90), 1, AL_CENTER);
                wrapText(cx, b->y + 72, QUESTS[SKINS[k].quest].title, 16, C(200, 200, 215));
            }
            continue;
        }
        if (b->action >= A_ESKIN0 && b->action < A_ESKIN0 + ESKIN_COUNT) {
            int k = b->action - A_ESKIN0, owned = ownsESkin(k), wob = focus && ((int)(gt * 6) & 1);
            const EnemySkin *E = &ESKINS[k];
            panel(b->x, b->y, b->w, b->h, focus);
            /* line-up: thug and gunner up front, a brute behind, a drone overhead (each standing on its feet, whatever its size) */
            enemyLook(k, 1);
            {
                const unsigned *keep = skinOv;
                if (eBody) skinOv = eBody->pal;
                const Spr *d = enemyBodySpr(EB_DRONE, 0, &sDrone0), *br = enemyBodySpr(EB_BRUTE, 0, &sBrute0);
                const Spr *th = enemyBodySpr(EB_THUG, wob, wob ? &sThug1 : &sThug0), *gu = enemyBodySpr(EB_GUNNER, wob, wob ? &sGunner1 : &sGunner0);
                drawSpriteScaled(d, cx - d->w, b->y + 6, 2);
                drawSpriteScaled(br, cx - br->w, b->y + 76 - br->h * 2, 2);
                drawSpriteScaled(th, cx - 22 - th->w, b->y + 78 - th->h * 2, 2);
                drawSpriteScaled(gu, cx + 22 - gu->w, b->y + 78 - gu->h * 2, 2);
                skinOv = keep;
            }
            enemyLook(0, 0);
            for (int j = 0; j < 9; j++) { /* a splash of what they bleed */
                static const unsigned conf[4] = { C(255, 80, 160), C(80, 200, 255), C(255, 230, 60), C(120, 255, 120) };
                int x = cx - 30 + (int)(hash3(j, k, 4) % 60), y = b->y + 80 + (int)(hash3(j, k, 5) % 3);
                unsigned c = E->confetti ? conf[j % 4] : j & 1 ? E->bloodA : E->bloodB;
                fillRect(x, y, 2 + (int)(hash3(j, k, 6) % 3), 1 + (j % 2), c);
            }
            if (!owned) {
                for (int j = 0; j < 82; j++)
                    for (int x = 0; x < b->w - 4; x++) pdark(b->x + 2 + x, b->y + 3 + j, 120);
                drawLock(cx, b->y + 44);
            }
            text(cx, b->y + 96, E->name, focus ? ui.hi : C_WHITE, 1, AL_CENTER);
            if (k == eskin) text(cx, b->y + 110, "EQUIPPED", C(120, 255, 140), 1, AL_CENTER);
            else if (owned) { text(cx, b->y + 110, "OWNED", C(160, 220, 255), 1, AL_CENTER); if (focus) text(cx, b->y + 126, "CLICK TO USE", ui.hi, 1, AL_CENTER); }
            else {
                sfmt(s, sizeof s, "%d", E->cost);
                text(cx + 4, b->y + 110, s, C(255, 214, 80), 1, AL_CENTER);
                drawCoin(cx - 4 - textWidth(s, 1) / 2, b->y + 112);
                if (focus) text(cx, b->y + 126, coins >= E->cost ? "CLICK TO BUY" : "NOT ENOUGH", coins >= E->cost ? C(120, 255, 140) : C(230, 90, 90), 1, AL_CENTER);
            }
            wrapText(cx, b->y + 136, E->blurb, 20, ui.dim);
            continue;
        }
        if (b->action == A_SANDY) continue; /* drawn last, on top of David's card */
        drawButton(b, focus);
    }
    for (int i = 0; i < nBtns; i++) { /* DAVID's Sandevistan switch: a little ON / OFF toggle at the bottom of his card */
        const Btn *b = &btns[i];
        int focus = i == focusBtn, on = !sandyOff;
        if (b->action != A_SANDY) continue;
        frame(b->x, b->y, b->w, b->h, on ? C(24, 60, 36) : C(60, 24, 30), focus ? ui.hi : on ? C(120, 255, 140) : C(230, 90, 90));
        text(b->x + 4, b->y + 4, "TRAIL", focus ? ui.hi : C_WHITE, 1, AL_LEFT);
        text(b->x + b->w - 4, b->y + 4, on ? "ON" : "OFF", on ? C(120, 255, 140) : C(230, 90, 90), 1, AL_RIGHT);
        if (focus) text(FW / 2, 228, "SANDEVISTAN: DASHING LEAVES A TRAIL OF AFTERIMAGES BEHIND DAVID", ui.hi, 1, AL_CENTER);
    }
    if (skinPages() > 1) { /* which page you're on, as dots and a number */
        for (int p = 0; p < skinPages(); p++) fillRect(FW / 2 + 172 + p * 7, 243, 4, 4, p == skinPage ? ui.hi : C(70, 66, 84));
        sfmt(s, sizeof s, "PAGE %d/%d", skinPage + 1, skinPages());
        text(FW / 2 + 172, 251, s, ui.dim, 1, AL_LEFT);
        text(FW / 2 - 205, 243, "Q / E", C(110, 110, 130), 1, AL_LEFT);
    }
    if (skinTab == 1) text(FW / 2, 216, "CHANGES HOW EVERY ENEMY LOOKS - AND WHAT THEY BLEED", ui.dim, 1, AL_CENTER);
    if (skinTab == 2) text(FW / 2, 216, "YOUR MOUSE CURSOR IN MENUS AND WHILE AIMING - ALL FREE", ui.dim, 1, AL_CENTER);
    if (skinTab == 3) text(FW / 2, 228, "YOUR PET RUNS WITH YOU - EDIT ITS COLLAR UNDER IT IN THE MAIN MENU", ui.dim, 1, AL_CENTER);
    coinsCorner();
}

/* EDIT under the pet in the main menu: collar on / off and its colour, with the pet walking in place to show it */
void renderPetEdit(void)
{
    text(FW / 2, 8, "EDIT PET", C_WHITE, 3, AL_CENTER);
    panel(FW / 2 - 70, 34, 140, 76, 0);
    for (int i = 0; i < 132; i += 4) fillRect(FW / 2 - 66 + i - (int)(gt * 20) % 4, 98, 2, 1, ui.dim); /* the floor rolls by */
    if (petFlies(pet)) drawPet(pet, FW / 2, 88 + (int)(sinf(gt * 3) * 3), 0, petFlap(gt * 12), 5);
    else drawPet(pet, FW / 2, 97, 0, 1 + ((int)(gt * 6) & 1), 5);
    for (int i = 0; i < nBtns; i++) {
        const Btn *b = &btns[i];
        int focus = i == focusBtn, a = b->action;
        if (a == A_PET_NAME) { /* the name: click to type a new one */
            int on = petNameEdit, custom = petName(pet) != PETS[pet].name;
            panel(b->x, b->y, b->w, b->h, focus || on);
            text(b->x + b->w / 2, b->y + 5, !on ? petName(pet) : petNameFresh ? "TYPE A NAME" : petNames[pet], on ? C_WHITE : custom ? ui.hi : ui.txt, 1, AL_CENTER);
            if (on && !petNameFresh && fmodf(gt, 1.0f) < 0.6f) fillRect(b->x + b->w / 2 + textWidth(petNames[pet], 1) / 2 + 2, b->y + 4, 1, 7, C_WHITE); /* the caret */
        } else if (a == A_PET_COLLAR) {
            panel(b->x, b->y, b->w, b->h, focus);
            text(b->x + 10, b->y + 5, b->label, focus ? ui.hi : ui.txt, 2, AL_LEFT);
            int px_ = b->x + b->w - 50;
            frame(px_, b->y + 4, 40, 13, collarOn ? C(30, 90, 50) : C(70, 30, 36), collarOn ? C(120, 255, 140) : C(230, 90, 90));
            fillRect(collarOn ? px_ + 26 : px_ + 3, b->y + 7, 11, 7, collarOn ? C(120, 255, 140) : C(230, 90, 90));
            text(collarOn ? px_ + 13 : px_ + 27, b->y + 8, collarOn ? "ON" : "OFF", C_WHITE, 1, AL_CENTER);
        } else if (a >= A_PCOL0 && a < A_PCOL_END) {
            int k = a - A_PCOL0, sel = collarOn && k == collarCol;
            frame(b->x - 1, b->y - 1, b->w + 2, b->h + 2, C_OUT, sel ? C_WHITE : focus ? ui.hi : C(70, 66, 84));
            fillRect(b->x + 2, b->y + 2, b->w - 4, b->h - 4, COLLAR_COL[k]);
            if (sel) { fillRect(b->x + b->w / 2 - 1, b->y + b->h + 3, 3, 2, C_WHITE); }
        } else if (a == A_PCOL_NONE) { /* a crossed-out swatch */
            frame(b->x - 1, b->y - 1, b->w + 2, b->h + 2, C_OUT, !collarOn ? C_WHITE : focus ? ui.hi : C(70, 66, 84));
            fillRect(b->x + 2, b->y + 2, b->w - 4, b->h - 4, C(30, 28, 40));
            for (int t = 3; t < b->w - 3; t++) { pset(b->x + t, b->y + t, C(230, 90, 90)); pset(b->x + b->w - 1 - t, b->y + t, C(230, 90, 90)); }
            if (!collarOn) fillRect(b->x + b->w / 2 - 1, b->y + b->h + 3, 3, 2, C_WHITE);
        } else drawButton(b, focus);
    }
    text(FW / 2, 160, collarOn ? "COLLAR COLOUR" : "NO COLLAR - PICK A COLOUR TO PUT ONE ON", ui.dim, 1, AL_CENTER);
    text(FW / 2, 204, petNameEdit ? "TYPE A NAME - BACKSPACE DELETES - ENTER WHEN DONE (EMPTY = ITS OWN NAME)" : "CLICK THE NAME TO GIVE YOUR PET A NAME", ui.dim, 1, AL_CENTER);
}
