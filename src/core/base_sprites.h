/* base_sprites.h - Pixel sprites drawn as text rows, and the palette letters they use. */
#ifndef NCS_BASE_SPRITES_H
#define NCS_BASE_SPRITES_H

typedef struct { int w, h; const char *const *rows; } Spr;

extern const Spr sPlayerIdle, sPlayerRun1, sPlayerRun2, sPlayerJump, sRobot0, sRobot1, sRobot2, sRobot3, sNinja0,
    sNinja1, sNinja2, sNinja3, sAstro0, sAstro1, sAstro2, sAstro3, sNeko0, sNeko1, sNeko2, sNeko3, sBones0, sBones1,
    sBones2, sBones3, sDavid0, sDavid1, sDavid2, sDavid3, sLucy0, sLucy1, sLucy2, sLucy3, sRebecca0, sRebecca1,
    sRebecca2, sRebecca3, sMaine0, sMaine1, sMaine2, sMaine3, sKiwi0, sKiwi1, sKiwi2, sKiwi3, sKnight0, sKnight1,
    sKnight2, sKnight3, sDino0, sDino1, sDino2, sDino3, sFalco0, sFalco1, sFalco2, sFalco3, sGloria0, sGloria1, sGloria2, sGloria3, sDorio0, sDorio1, sDorio2, sDorio3,
    sPilar0, sPilar1, sPilar2, sPilar3, sSkinX_0, sSkinX_1, sSkinX_2, sSkinX_3, sSkinY_0, sSkinY_1, sSkinY_2, sSkinY_3,
    sSkinZ_0, sSkinZ_1, sSkinZ_2, sSkinZ_3, sThug0, sThug1, sGunner0, sGunner1, sDrone0, sDrone1, sBomber0,
    sBomber1, sBrute0, sBrute1, sShield0, sShield1, sGunship, sCyNinja0, sCyNinja1, sJet0, sJet1, sMech0, sMech1,
    sHeart, sMedkit, sIcPistol, sIcSmg, sIcShotgun, sIcLaser, sIcGrenade, sIcRocket, sIcGravity;

extern const Spr sEAraThug0, sEAraThug1, sEAraGun0, sEAraGun1, sEAraBrute0, sEAraBrute1, sEAraDrone0, sEAraDrone1, sEMaxThug0,
    sEMaxThug1, sEMaxGun0, sEMaxGun1, sEMaxBrute0, sEMaxBrute1, sEMaxDrone0, sEMaxDrone1, sETygThug0, sETygThug1,
    sETygGun0, sETygGun1, sETygBrute0, sETygBrute1, sETygDrone0, sETygDrone1, sEPsyThug0, sEPsyThug1, sEPsyGun0,
    sEPsyGun1, sEPsyBrute0, sEPsyBrute1, sEPsyDrone0, sEPsyDrone1, sEScvThug0, sEScvThug1, sEScvGun0, sEScvGun1,
    sEScvBrute0, sEScvBrute1, sEScvDrone0, sEScvDrone1, sESmaThug0, sESmaThug1, sESmaGun0, sESmaGun1, sESmaBrute0,
    sESmaBrute1, sESmaDrone0, sESmaDrone1;
extern const Spr sEGangThug0, sEGangThug1, sEGangGun0, sEGangGun1, sEGangBrute0, sEGangBrute1, sEGangDrone0, sEGangDrone1,
    sEZomThug0, sEZomThug1, sEZomGun0, sEZomGun1, sEZomBrute0, sEZomBrute1, sEZomDrone0, sEZomDrone1, sEBotThug0,
    sEBotThug1, sEBotGun0, sEBotGun1, sEBotBrute0, sEBotBrute1, sEBotDrone0, sEBotDrone1, sEClownThug0,
    sEClownThug1, sEClownGun0, sEClownGun1, sEClownBrute0, sEClownBrute1, sEClownDrone0, sEClownDrone1,
    sEAlienThug0, sEAlienThug1, sEAlienGun0, sEAlienGun1, sEAlienBrute0, sEAlienBrute1, sEAlienDrone0,
    sEAlienDrone1;

extern unsigned bossArmor, bossArmorLight;
extern const unsigned *skinOv; /* while drawing the player: hair, visor, scarf, jacket, jacket light, pants, pants light, boots */
extern unsigned eTab[128]; /* enemy skin: letter -> colour while an enemy is being drawn */
extern int eTabOn;

unsigned pal(char ch);

#endif
