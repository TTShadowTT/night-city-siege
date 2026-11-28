/* ui_screens_shop.h - Menu screens: main menu, map select, menu background, UI style, skins and pet editor. */
#ifndef NCS_UI_SCREENS_SHOP_H
#define NCS_UI_SCREENS_SHOP_H

void renderMainMenu(void);
void renderMapSelect(void);
void renderMenuBg(void);
/* UI STYLE screen: each card is drawn in its own style - a sample button and the style's colours */
void renderUiStyle(void);
void renderSkins(void);
/* EDIT under the pet in the main menu: collar on / off and its colour, with the pet walking in place to show it */
void renderPetEdit(void);

#endif
