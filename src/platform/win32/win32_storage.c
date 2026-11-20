/* win32_storage.c - Where the Windows exe keeps your progress, and the local date for the daily quests. */
#include "win32.h"

char savePath[MAX_PATH];

int plat_load_data(char *buf, int cap)
{
    int n = 0;
    FILE *f = fopen(savePath, "rb");
    if (f) { n = (int)fread(buf, 1, (size_t)cap, f); fclose(f); }
    return n;
}

void plat_save_data(const char *text)
{
    FILE *f = fopen(savePath, "wb");
    if (f) { fputs(text, f); fclose(f); }
}

int plat_day(void)
{
    SYSTEMTIME st;
    FILETIME ft;
    ULARGE_INTEGER u;
    GetLocalTime(&st);
    st.wHour = st.wMinute = st.wSecond = st.wMilliseconds = 0;
    SystemTimeToFileTime(&st, &ft);
    u.LowPart = ft.dwLowDateTime; u.HighPart = ft.dwHighDateTime;
    return (int)(u.QuadPart / 864000000000ULL) - 134774; /* days since 1601 -> days since 1970 */
}

int plat_daysec(void)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    return st.wHour * 3600 + st.wMinute * 60 + st.wSecond;
}

/* a link in the game (the credits): open it in the default browser */
void plat_open_url(const char *url) { ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL); }
