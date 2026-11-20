/* gameplay_save.h - Saving and loading progress, toasts and quest checks. */
#ifndef NCS_GAMEPLAY_SAVE_H
#define NCS_GAMEPLAY_SAVE_H

extern int dqDay, dq[3], dqProg[3], dqDone, dqCacheOk; /* daily quests (see below) */

void toast(const char *s);
void saveGame(void);
void loadGame(void);
void addProgress(int stat, int n);
void setBest(int stat, int v);

#endif
