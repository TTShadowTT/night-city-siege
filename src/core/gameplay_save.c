/* gameplay_save.c - Saving and loading progress, toasts and quest checks. */
#include "core.h"

void toast(const char *s)
{
    sfmt(toastTxt, sizeof toastTxt, "%s", s);
    toastT = 3.0f;
}

int dqDay, dq[3], dqProg[3], dqDone, dqCacheOk; /* daily quests (see below) */

void saveGame(void)
{
    char buf[1280];
    int n;
    if (bot) return; /* automated test runs never touch the player's save */
    sfmt(buf, sizeof buf, "SLOP9 %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", coins, mapsMask, skinsMask, skin, questMask,
         musicOn | (perksOn ? 0 : 2) | (shakeOn ? 0 : 4) | (devMode ? 8 : 0) | uiStyle << 4 | 2048 | (perkLimitOn ? 0 : 4096) | perkLimit << 13 | cursorSkin << 19 | (sandyOff ? 1 << 25 : 0) | (sfxOn ? 0 : 1 << 26), musicVolPct, sfxVolPct, eskin, eskinsMask,
         dqDay, dq[0], dq[1], dq[2], dqProg[0], dqProg[1], dqProg[2], dqDone, menuBg, pet, petsMask, collarOn | collarCol << 1, petsMask2);
    for (int i = 0; i < P_COUNT; i++) {
        n = (int)strlen(buf);
        sfmt(buf + n, (int)sizeof buf - n, " %d", pstat[i]);
    }
    /* then the MENU TITLE as "#T owned style line1 line2": spaces become _, an empty line is ~ */
    n = (int)strlen(buf);
    sfmt(buf + n, (int)sizeof buf - n, " #T %d %d ", titleOwned, titleStyle);
    for (int l = 0; l < 2; l++) {
        const char *t = l ? title2 : title1;
        n = (int)strlen(buf);
        if (!*t) buf[n++] = '~';
        for (; *t && n < (int)sizeof buf - 3; t++) buf[n++] = *t == ' ' ? '_' : *t;
        if (!l) buf[n++] = ' ';
        buf[n] = 0;
    }
    n = (int)strlen(buf);
    sfmt(buf + n, (int)sizeof buf - n, " #M %d #D %d #C %d #X %d #K %d", musicMode, D_COUNT, achMask, hardUnlocked | hardOn << 1 | !dmgNumsOn << 2, SKIN_COUNT); /* the SOUNDTRACK setting, how many maps there are, the achievements, how many skins */
    n = (int)strlen(buf); /* then "#P id name ...": the names you gave your pets, spaces as _ */
    sfmt(buf + n, (int)sizeof buf - n, " #P");
    for (int k = 0; k < PET_COUNT; k++) {
        if (!petNames[k][0]) continue;
        n = (int)strlen(buf);
        sfmt(buf + n, (int)sizeof buf - n, " %d ", k);
        n = (int)strlen(buf);
        for (const char *t = petNames[k]; *t && n < (int)sizeof buf - 2; t++) buf[n++] = *t == ' ' ? '_' : *t;
        buf[n] = 0;
    }
    plat_save_data(buf);
}
void loadGame(void)
{
    char buf[1280];
    int len = plat_load_data(buf, sizeof buf - 1), vals[23 + P_COUNT], nv = 0, ver, fixed, sawX = 0;
    if (len < 6) return;
    buf[len] = 0;
    if (strncmp(buf, "SLOP", 4) != 0 || buf[4] < '1' || buf[4] > '9') return;
    ver = buf[4] - '0';
    fixed = ver == 1 ? 5 : ver == 2 ? 6 : ver == 3 ? 10 : ver <= 5 ? 18 : ver <= 7 ? 19 : ver == 8 ? 22 : 23; /* SLOP5 = SLOP4 with 8 districts in the daily pool */
    for (const char *p = buf + 5; *p && nv < 23 + P_COUNT;) {
        while (*p == ' ') p++;
        if (!*p || *p == '#') break; /* # starts the text part (the MENU TITLE) */
        int v = 0, neg = *p == '-';
        if (neg) p++;
        while (*p >= '0' && *p <= '9') v = v * 10 + (*p++ - '0');
        vals[nv++] = neg ? -v : v;
        while (*p && *p != ' ') p++;
    }
    if (nv < 5) return;
    coins = vals[0]; mapsMask = vals[1] | 1; skinsMask = vals[2] | 1; skin = clampi(vals[3], 0, SKIN_COUNT - 1); questMask = vals[4];
    if (ver >= 2 && nv > 5) { musicOn = vals[5] & 1; perksOn = !(vals[5] & 2); shakeOn = !(vals[5] & 4); devMode = (vals[5] & 8) != 0; uiStyle = (vals[5] >> 4) & 63; picUndone = (vals[5] & 2048) != 0;
        /* added later to the same bit field: older saves have zeros there, which mean the defaults */
        perkLimitOn = !(vals[5] & 4096);
        perkLimit = (vals[5] >> 13) & 63 ? clampi((vals[5] >> 13) & 63, 1, PERK_TOTAL) : PERK_DEFAULT;
        cursorSkin = clampi((vals[5] >> 19) & 63, 0, CURSOR_COUNT - 1);
        sandyOff = (vals[5] >> 25) & 1;
        sfxOn = !((vals[5] >> 26) & 1);
    }
    if (!ownsSkin(skin)) skin = 0;
    if (ver >= 3 && nv > 9) {
        musicVolPct = clampi(vals[6], 0, 100); sfxVolPct = clampi(vals[7], 0, 100);
        eskinsMask = vals[9] | 1; eskin = clampi(vals[8], 0, ESKIN_COUNT - 1);
        if (!ownsESkin(eskin)) eskin = 0;
    }
    if (ver >= 4 && nv > 17) {
        dqDay = vals[10];
        for (int i = 0; i < 3; i++) { dq[i] = vals[11 + i]; dqProg[i] = vals[14 + i]; }
        dqDone = vals[17] & 7;
        if (ver <= 6) dqDay = -1; /* the daily pool grew with the new maps: deal again */
        dqCacheOk = 0;
    }
    if (ver >= 6 && nv > 18) menuBg = clampi(vals[18], BG_PICTURE, D_COUNT - 1);
    if (ver >= 8 && nv > 21) {
        petsMask = vals[20] | 1; if (ver >= 9 && nv > 22) petsMask2 = vals[22];
        pet = clampi(vals[19], 0, PET_COUNT - 1);
        if ((petsMask2 >> 10) & 1) { petsMask2 &= ~(1 << 10); coins += 800; } /* pet 41 (NIBBLES) was removed: its coins come back */
        if (!ownsPet(pet)) pet = 0;
        collarOn = vals[21] & 1; collarCol = clampi(vals[21] >> 1, 0, COLLAR_COUNT - 1);
    }
    if (!picUndone && menuBg == BG_PICTURE) menuBg = BG_LAST;
    if (menuBg == BG_PICTURE && !ownsMap(D_ORBIT)) menuBg = BG_LAST; /* the picture used to be free by mistake */
    for (int i = 0; i < P_COUNT && fixed + i < nv; i++) pstat[i] = vals[fixed + i];
    { /* "#D maps": how many maps the save was made with - the map-coloured UI styles sit before the shaped ones, so new maps move those */
        int oldMaps = 13;
        for (const char *p = buf; *p; p++)
            if (p[0] == '#' && p[1] == 'D' && p[2] == ' ') { oldMaps = 0; for (p += 3; *p >= '0' && *p <= '9'; p++) oldMaps = oldMaps * 10 + (*p - '0'); break; }
        uiStyle = uiAfterNewMaps(uiStyle, oldMaps);
    }
    for (const char *p = buf; *p; p++) /* "#C mask": the achievements (an older "#A" list is ignored: those were reset) */
        if (p[0] == '#' && p[1] == 'C' && p[2] == ' ') {
            int v = 0;
            for (p += 3; *p >= '0' && *p <= '9'; p++) v = v * 10 + (*p - '0');
            achMask = v & ((1 << ACH_COUNT) - 1);
            break;
        }
    for (const char *p = buf; *p; p++) /* "#X flags": 1 HARD MODE unlocked, 2 HARD MODE on, 4 DAMAGE NUMBERS off */
        if (p[0] == '#' && p[1] == 'X' && p[2] == ' ') {
            int v = 0;
            for (p += 3; *p >= '0' && *p <= '9'; p++) v = v * 10 + (*p - '0');
            hardUnlocked = v & 1; hardOn = (v >> 1) & 1 && hardUnlocked; dmgNumsOn = !((v >> 2) & 1);
            sawX = 1;
            break;
        }
    if (!sawX) achMask &= ~(1 << ACH_HARD); /* in saves from before HARD MODE that bit meant the removed NIBBLES! */
    { /* "#K skins": how many skins the save was made with - SKIN_X is always the last one, so new skins move it */
        int oldSkins = 19;
        for (const char *p = buf; *p; p++)
            if (p[0] == '#' && p[1] == 'K' && p[2] == ' ') { oldSkins = 0; for (p += 3; *p >= '0' && *p <= '9'; p++) oldSkins = oldSkins * 10 + (*p - '0'); break; }
        if (oldSkins == 26) { /* 26 had six more skins (19-24) that were taken out again: their coins come back */
            static const int refund[6] = { 2000, 1600, 1500, 1700, 1300, 2200 };
            int raw = vals[3], last = (skinsMask >> 25) & 1;
            for (int i = 0; i < 6; i++)
                if ((skinsMask >> (19 + i)) & 1) coins += refund[i];
            skinsMask &= (1 << 19) - 1;
            if (last) skinsMask |= 1 << SKIN_X;
            skin = raw == 25 && last ? SKIN_X : raw >= 19 ? 0 : raw;
        } else if (oldSkins > 0 && oldSkins < SKIN_COUNT) {
            int was = oldSkins - 1;
            if ((skinsMask >> was) & 1) { skinsMask &= ~(1 << was); skinsMask |= 1 << SKIN_X; }
            if (skin == was) skin = SKIN_X;
        }
    }
    for (const char *p = buf; *p; p++) /* the pet names: "#P id name id name ..." up to the next # */
        if (p[0] == '#' && p[1] == 'P' && (p[2] == ' ' || !p[2])) {
            for (p += 2; *p && *p != '#';) {
                while (*p == ' ') p++;
                if (*p < '0' || *p > '9') break;
                int k = 0, n = 0;
                while (*p >= '0' && *p <= '9') k = k * 10 + (*p++ - '0');
                while (*p == ' ') p++;
                char *t = k < PET_COUNT ? petNames[k] : 0;
                for (; *p && *p != ' ' && *p != '#'; p++)
                    if (t && n < PETNAME_MAX) t[n++] = *p == '_' ? ' ' : *p;
                if (t) t[n] = 0;
            }
            break;
        }
    for (const char *p = buf; *p; p++) /* the SOUNDTRACK: "#M mode" */
        if (p[0] == '#' && p[1] == 'M' && p[2] == ' ') {
            int m = 0;
            for (p += 3; *p >= '0' && *p <= '9'; p++) m = m * 10 + (*p - '0');
            musicMode = clampi(m, 0, MUS_TRACKS + 1 + MY_MAX);
            break;
        }
    for (const char *p = buf; *p; p++) /* the MENU TITLE: "#T owned style line1 line2" */
        if (p[0] == '#' && p[1] == 'T' && p[2] == ' ') {
            p += 3;
            titleOwned = *p == '1';
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            titleStyle = 0;
            while (*p >= '0' && *p <= '9') titleStyle = titleStyle * 10 + (*p++ - '0');
            titleStyle = clampi(titleStyle, 0, TS_COUNT - 1);
            for (int l = 0; l < 2; l++) {
                char *t = l ? title2 : title1;
                int n = 0, max = l ? TITLE2_MAX : TITLE1_MAX;
                while (*p == ' ') p++;
                for (; *p && *p != ' '; p++)
                    if (*p != '~' && n < max) t[n++] = *p == '_' ? ' ' : *p;
                t[n] = 0;
            }
            break;
        }
    for (int q = 0; q < QUEST_COUNT; q++) /* quests finished before their reward became a skin still grant it */
        if ((questMask & (1 << q)) && QUESTS[q].skin >= 0) skinsMask |= 1 << QUESTS[q].skin;
}

static void checkQuests(void)
{
    for (int q = 0; q < QUEST_COUNT; q++) {
        if (questMask & (1 << q) || pstat[QUESTS[q].stat] < QUESTS[q].goal) continue;
        char s[64];
        questMask |= 1 << q;
        coins += QUESTS[q].coins;
        if (QUESTS[q].skin >= 0) skinsMask |= 1 << QUESTS[q].skin;
        if (QUESTS[q].skin >= 0) sfmt(s, sizeof s, "QUEST DONE! SKIN UNLOCKED: %s", SKINS[QUESTS[q].skin].name);
        else sfmt(s, sizeof s, "QUEST DONE! +%d COINS", QUESTS[q].coins);
        toast(s);
        sfx(S_WAVE, 0.5f);
        saveGame();
    }
}

void addProgress(int stat, int n) { pstat[stat] += n; checkQuests(); }
void setBest(int stat, int v) { if (v > pstat[stat]) { pstat[stat] = v; checkQuests(); } }
