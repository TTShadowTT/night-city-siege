/* linux_storage.h - Where the Linux game keeps your progress, and the local date for the daily quests. */
#ifndef NCS_LINUX_STORAGE_H
#define NCS_LINUX_STORAGE_H

/* ~/.local/share/NeonEdge: the save and MY SONGS (kept when the game is uninstalled or updated) */
extern char dataDir[PATH_MAX], savePath[PATH_MAX + 16];

void storageInit(void);

#endif
