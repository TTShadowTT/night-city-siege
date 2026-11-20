/* ui_style.h - UI STYLE presets: the colours and shapes of every menu look. */
#ifndef NCS_UI_STYLE_H
#define NCS_UI_STYLE_H

#define UI_HAND_N 15 /* entries in UI_HAND */
#define UI_SHAPED_N 15 /* entries in UI_SHAPED */
#define UI_COUNT (UI_HAND_N + D_COUNT + UI_SHAPED_N)

extern const UiStyle UI_HAND[];
extern const UiStyle UI_SHAPED[];

/* picker order (hand-made, shaped, then the map looks) <-> style number */
int uiAt(int slot);
int uiSlot(int i);
int uiAfterNewMaps(int i, int oldMaps);
UiStyle uiStyleOf(int i);
void setUi(int i);

#endif
