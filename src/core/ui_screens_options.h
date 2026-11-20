/* ui_screens_options.h - Menu screens: options, menu title editor and the soundtrack list. */
#ifndef NCS_UI_SCREENS_OPTIONS_H
#define NCS_UI_SCREENS_OPTIONS_H

void renderOptions(void);
/* MENU TITLE screen: the title itself on top (live), the two lines to type into, then a card per style */
void renderTitleEdit(void);
/* SOUNDTRACK screen: one long list that scrolls - + ADD SONG, the choices and the game's songs, then MY SONGS */
void renderSongs(void);

#endif
