/* linux_storage.c - Where the Linux game keeps your progress, and the local date for the daily quests. */
#include "linux.h"
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

char dataDir[PATH_MAX], savePath[PATH_MAX + 16];

void storageInit(void)
{
    const char *xdg = getenv("XDG_DATA_HOME"), *home = getenv("HOME");
    if (xdg && *xdg) snprintf(dataDir, sizeof dataDir, "%s/NeonEdge", xdg);
    else {
        snprintf(dataDir, sizeof dataDir, "%s/.local/share", home ? home : ".");
        mkdir(dataDir, 0755);
        strncat(dataDir, "/NeonEdge", sizeof dataDir - strlen(dataDir) - 1);
    }
    mkdir(dataDir, 0755);
    snprintf(savePath, sizeof savePath, "%s/slop_save.txt", dataDir);
}

int plat_load_data(char *buf, int cap)
{
    int n = 0;
    FILE *f = fopen(savePath, "rb");
    if (f) { n = (int)fread(buf, 1, (size_t)cap, f); fclose(f); }
    return n;
}

/* written to a temporary file first, so a crash halfway never leaves a broken save */
void plat_save_data(const char *text)
{
    char tmp[PATH_MAX + 32];
    snprintf(tmp, sizeof tmp, "%s.new", savePath);
    FILE *f = fopen(tmp, "wb");
    if (!f) return;
    fputs(text, f);
    if (fclose(f) == 0) rename(tmp, savePath);
}

int plat_day(void)
{
    time_t t = time(NULL);
    struct tm lt;
    localtime_r(&t, &lt);
    return (int)((t + lt.tm_gmtoff) / 86400);
}

int plat_daysec(void)
{
    time_t t = time(NULL);
    struct tm lt;
    localtime_r(&t, &lt);
    return lt.tm_hour * 3600 + lt.tm_min * 60 + lt.tm_sec;
}

/* a link in the game (the credits): open it in the default browser, without waiting for it */
void plat_open_url(const char *url)
{
    pid_t p = fork();
    if (p == 0) {
        if (fork() == 0) { execlp("xdg-open", "xdg-open", url, (char *)NULL); _exit(1); }
        _exit(0);
    }
    if (p > 0) waitpid(p, NULL, 0);
}
