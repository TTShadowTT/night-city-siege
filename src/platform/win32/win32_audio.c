/* win32_audio.c - Sound on Windows: a small software mixer on waveOut, plus MY SONGS decoded with Media Foundation. */
#include "win32.h"

#define ABUF 384
#define NABUF 4
#define NVOICE 24

typedef struct { int s, pos; float vol; } Voice;
static Voice voices[NVOICE];
static int musPos;
static float musVol, musTarget; /* the looping soundtrack, faded toward musTarget */
HWAVEOUT hwo;
static WAVEHDR whdr[NABUF];
static short abuf[NABUF][ABUF];
int audioOk;

void audioInit(void)
{
    WAVEFORMATEX wf = { 0 };
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 1;
    wf.nSamplesPerSec = SND_RATE;
    wf.wBitsPerSample = 16;
    wf.nBlockAlign = 2;
    wf.nAvgBytesPerSec = SND_RATE * 2;
    for (int i = 0; i < NVOICE; i++) voices[i].s = -1;
    if (waveOutOpen(&hwo, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) return;
    for (int i = 0; i < NABUF; i++) {
        whdr[i].lpData = (LPSTR)abuf[i];
        whdr[i].dwBufferLength = ABUF * sizeof(short);
        waveOutPrepareHeader(hwo, &whdr[i], sizeof(WAVEHDR));
    }
    audioOk = 1;
}

void plat_sound(int id, float vol)
{
    if (!audioOk) return;
    int best = 0, bestpos = -1;
    for (int i = 0; i < NVOICE; i++) {
        if (voices[i].s < 0) { best = i; break; }
        if (voices[i].pos > bestpos) { bestpos = voices[i].pos; best = i; }
    }
    voices[best].s = id;
    voices[best].pos = 0;
    voices[best].vol = vol;
}

void plat_music(float vol) { musTarget = vol; }

/* MY SONGS: the player's own music files
   The picked files are copied into my_songs\ next to the save; my_songs\songs.txt lists them ("map<TAB>name<TAB>file").
   Windows' Media Foundation decodes them (MP3, FLAC, WAV, M4A, WMA...) to 22050 Hz mono on a background thread
   the first time one plays. */
#define MY_MAX 8
static MySong *mySongs[MY_MAX];
static int myN;
char songDir[MAX_PATH], songIndex[MAX_PATH + 16];

static void songFree(MySong *s) { free(s->pcm); free(s); }

DWORD WINAPI songDecode(LPVOID arg)
{
    MySong *s = (MySong *)arg;
    char a[MAX_PATH + 40];
    WCHAR path[MAX_PATH];

    IMFSourceReader *rd = NULL;
    IMFMediaType *mt = NULL;
    short *buf = NULL;
    int len = 0, cap = 0, ok = 0, com = SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED));
    UINT32 ch = 0, rate = 0;
    snprintf(a, sizeof a, "%s\\%s", songDir, s->file);
    MultiByteToWideChar(CP_ACP, 0, a, -1, path, MAX_PATH);
    if (SUCCEEDED(MFStartup(MF_VERSION, MFSTARTUP_LITE))) {
        if (SUCCEEDED(MFCreateSourceReaderFromURL(path, NULL, &rd))) {
            IMFMediaType *cur = NULL;
            IMFSourceReader_SetStreamSelection(rd, (DWORD)MF_SOURCE_READER_ALL_STREAMS, FALSE);
            IMFSourceReader_SetStreamSelection(rd, (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
            /* ask for plain 16-bit PCM in whatever channels and rate the file has; we mix down and resample ourselves */
            MFCreateMediaType(&mt);
            IMFMediaType_SetGUID(mt, &MF_MT_MAJOR_TYPE, &MFMediaType_Audio);
            IMFMediaType_SetGUID(mt, &MF_MT_SUBTYPE, &MFAudioFormat_PCM);
            IMFMediaType_SetUINT32(mt, &MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
            if (SUCCEEDED(IMFSourceReader_SetCurrentMediaType(rd, (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, mt)) &&
                SUCCEEDED(IMFSourceReader_GetCurrentMediaType(rd, (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &cur))) {
                IMFMediaType_GetUINT32(cur, &MF_MT_AUDIO_NUM_CHANNELS, &ch);
                IMFMediaType_GetUINT32(cur, &MF_MT_AUDIO_SAMPLES_PER_SECOND, &rate);
                IMFMediaType_Release(cur);
            }
            if (ch >= 1 && ch <= 8 && rate >= 8000) {
                for (;;) {
                    DWORD flags = 0;
                    IMFSample *smp = NULL;
                    if (FAILED(IMFSourceReader_ReadSample(rd, (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, NULL, &flags, NULL, &smp))) break;
                    if (smp) {
                        IMFMediaBuffer *mb = NULL;
                        if (SUCCEEDED(IMFSample_ConvertToContiguousBuffer(smp, &mb))) {
                            BYTE *d;
                            DWORD n;
                            if (SUCCEEDED(IMFMediaBuffer_Lock(mb, &d, NULL, &n))) {
                                const short *in = (const short *)d;
                                int k = (int)(n / 2 / ch); /* frames */
                                if (len + k > cap) {
                                    short *nb = realloc(buf, (size_t)(len + k) * 2 * sizeof(short));
                                    if (nb) { buf = nb; cap = (len + k) * 2; } else k = 0;
                                }
                                for (int f = 0; f < k; f++) { /* mix the channels down to mono */
                                    int sum = 0;
                                    for (UINT32 c = 0; c < ch; c++) sum += in[f * ch + c];
                                    buf[len + f] = (short)(sum / (int)ch);
                                }
                                len += k;
                                IMFMediaBuffer_Unlock(mb);
                            }
                            IMFMediaBuffer_Release(mb);
                        }
                        IMFSample_Release(smp);
                    }
                    if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;
                }
                if (len > 0 && rate != SND_RATE) { /* resample to the game's rate, straight-line between samples */
                    int out = (int)((long long)len * SND_RATE / rate);
                    short *rs = malloc((size_t)out * sizeof(short));
                    if (rs) {
                        for (int i = 0; i < out; i++) {
                            double p = (double)i * rate / SND_RATE;
                            int i0 = (int)p, i1 = i0 + 1 < len ? i0 + 1 : i0;
                            double t = p - i0;
                            rs[i] = (short)(buf[i0] * (1 - t) + buf[i1] * t);
                        }
                        free(buf); buf = rs; len = out;
                    } else len = 0;
                }
                ok = len > SND_RATE / 2;
            }
        }
        if (mt) IMFMediaType_Release(mt);
        if (rd) IMFSourceReader_Release(rd);
        MFShutdown();
    }
    if (com) CoUninitialize();
    if (ok) { s->pcm = buf; s->len = len; } else free(buf);
    InterlockedExchange(&s->state, ok ? 2 : 3);
    if (s->dead) songFree(s); /* removed while we were busy */
    return 0;
}

/* tell the game the names and districts */
void songsPush(void)
{
    char *nm = game_song_names();
    signed char *mp = game_song_maps();
    memset(nm, 0, MY_MAX * MY_NAME);
    for (int i = 0; i < MY_MAX; i++) mp[i] = -1;
    for (int i = 0; i < myN; i++) { snprintf(nm + i * MY_NAME, MY_NAME, "%s", mySongs[i]->name); mp[i] = (signed char)mySongs[i]->map; }
    game_songs_set(myN);
}

static void songsSave(void)
{
    FILE *f = fopen(songIndex, "w");
    if (!f) return;
    for (int i = 0; i < myN; i++) fprintf(f, "%d\t%s\t%s\n", mySongs[i]->map, mySongs[i]->name, mySongs[i]->file);
    fclose(f);
}

void songsLoad(void)
{
    char line[256];
    FILE *f = fopen(songIndex, "r");
    if (!f) return;
    while (myN < MY_MAX && fgets(line, sizeof line, f)) {
        char *t1 = strchr(line, '\t'), *t2 = t1 ? strchr(t1 + 1, '\t') : NULL;
        if (!t2) continue;
        *t1 = *t2 = 0;
        t2[1 + strcspn(t2 + 1, "\r\n")] = 0;
        MySong *s = calloc(1, sizeof *s);
        if (!s) break;
        s->map = atoi(line);
        snprintf(s->name, sizeof s->name, "%s", t1 + 1);
        snprintf(s->file, sizeof s->file, "%s", t2 + 1);
        mySongs[myN++] = s;
    }
    fclose(f);
}

/* a file name the pixel font can show: no folder or extension, capitals, only letters it has */
static void songName(const WCHAR *path, char *out)
{
    const WCHAR *b = path, *dot = NULL;
    int n = 0, sp = 1;
    for (const WCHAR *p = path; *p; p++) if (*p == L'\\' || *p == L'/') b = p + 1;
    for (const WCHAR *p = b; *p; p++) if (*p == L'.') dot = p;
    for (const WCHAR *p = b; *p && p != dot && n < MY_NAME - 1; p++) {
        WCHAR c = *p >= L'a' && *p <= L'z' ? (WCHAR)(*p - 32) : *p;
        int keep = (c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9') || wcschr(L"!?.,:'()+-/", c);
        if (keep) { out[n++] = (char)c; sp = 0; }
        else if (!sp) { out[n++] = ' '; sp = 1; }
    }
    while (n && out[n - 1] == ' ') n--;
    out[n] = 0;
    if (!n) strcpy(out, "MY SONG");
}

static void songAddFile(const WCHAR *src)
{
    if (myN >= MY_MAX) return;
    MySong *s = calloc(1, sizeof *s);
    if (!s) return;
    const WCHAR *dot = wcsrchr(src, L'.');
    char ext[8] = "";
    for (int i = 0; dot && dot[i] && i < 6; i++) ext[i] = (char)(dot[i] >= L'A' && dot[i] <= L'Z' ? dot[i] + 32 : dot[i]);
    for (int k = 1; k < 1000; k++) { /* a free file name: song1.flac, song2.mp3, ... */
        char a[MAX_PATH + 40];
        snprintf(s->file, sizeof s->file, "song%d%s", k, ext);
        snprintf(a, sizeof a, "%s\\%s", songDir, s->file);
        if (GetFileAttributesA(a) == INVALID_FILE_ATTRIBUTES) break;
    }
    char a[MAX_PATH + 40];
    WCHAR dst[MAX_PATH];
    snprintf(a, sizeof a, "%s\\%s", songDir, s->file);
    MultiByteToWideChar(CP_ACP, 0, a, -1, dst, MAX_PATH);
    if (!CopyFileW(src, dst, FALSE)) { free(s); return; }
    songName(src, s->name);
    s->map = -1;
    mySongs[myN++] = s;
}

int plat_song_add(void)
{
    static WCHAR files[32768];
    OPENFILENAMEW of = { 0 };
    files[0] = 0;
    of.lStructSize = sizeof of;
    of.hwndOwner = hwnd;
    of.lpstrFilter = L"Music (MP3, FLAC, WAV, M4A, WMA)\0*.mp3;*.flac;*.wav;*.m4a;*.aac;*.wma;*.ogg\0All files\0*.*\0";
    of.lpstrFile = files;
    of.nMaxFile = sizeof files / sizeof files[0];
    of.lpstrTitle = L"Add your own songs";
    of.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameW(&of)) {
        const WCHAR *first = files, *next = files + wcslen(files) + 1;
        if (!*next) songAddFile(first); /* one file: the full path */
        else for (; *next; next += wcslen(next) + 1) { /* several: the folder, then each name */
            WCHAR full[MAX_PATH];
            _snwprintf(full, MAX_PATH, L"%ls\\%ls", first, next);
            full[MAX_PATH - 1] = 0;
            songAddFile(full);
        }
        songsSave();
        songsPush();
    }
    return 1;
}

void plat_song_remove(int i)
{
    if (i < 0 || i >= myN) return;
    MySong *s = mySongs[i];
    char a[MAX_PATH + 40];
    snprintf(a, sizeof a, "%s\\%s", songDir, s->file);
    DeleteFileA(a);
    memmove(&mySongs[i], &mySongs[i + 1], (size_t)(myN - i - 1) * sizeof mySongs[0]);
    myN--;
    if (s->state == 1) InterlockedExchange(&s->dead, 1); /* the decoder frees it when it's done */
    else songFree(s);
    songsSave();
    songsPush();
}

void plat_song_map(int i, int map)
{
    if (i < 0 || i >= myN) return;
    mySongs[i]->map = map;
    songsSave();
}

/* the song for track t (10 and up), decoded on first use; NULL while it isn't ready */
static const short *songPcm(int t, int *len)
{
    int i = t - 10;
    if (i < 0 || i >= myN) return NULL;
    MySong *s = mySongs[i];
    if (s->state == 0) {
        HANDLE th;
        s->state = 1;
        th = CreateThread(NULL, 0, songDecode, s, 0, NULL);
        if (th) CloseHandle(th); else s->state = 3;
    }
    if (s->state != 2) return NULL;
    *len = s->len;
    return s->pcm;
}

void audioPump(void)
{
    if (!audioOk) return;
    for (int b = 0; b < NABUF; b++) {
        if (*(volatile DWORD *)&whdr[b].dwFlags & WHDR_INQUEUE) continue;
        int acc[ABUF] = { 0 };
        static int track = -1;
        if (game_music_track() != track) { track = game_music_track(); musPos = 0; musVol = 0; } /* a new song: start it from the top, faded in */
        int mlen = 0;
        float lvl = 0.5f;
        const short *mus;
        if (track >= 10) { mus = songPcm(track, &mlen); lvl = 0.25f; } /* MY SONGS are mastered much louder */
        else { mus = game_music_data(); mlen = game_music_len(); }
        for (int j = 0; j < ABUF && mus && mlen > 0; j++) {
            musVol += (musTarget - musVol) * 0.0005f;
            if (musPos >= mlen) musPos = 0;
            if (musVol > 0.001f) acc[j] += (int)(mus[musPos] * musVol * lvl);
            musPos++;
        }
        for (int v = 0; v < NVOICE; v++) {
            Voice *vc = &voices[v];
            if (vc->s < 0) continue;
            const short *data = game_sound_data(vc->s);
            int len = game_sound_len(vc->s), vol = (int)(vc->vol * 256 * 0.5f);
            for (int j = 0; j < ABUF; j++) {
                acc[j] += (data[vc->pos] * vol) >> 8;
                if (++vc->pos >= len) { vc->s = -1; break; }
            }
        }
        for (int j = 0; j < ABUF; j++)
            abuf[b][j] = (short)(acc[j] > 32767 ? 32767 : acc[j] < -32768 ? -32768 : acc[j]);
        waveOutWrite(hwo, &whdr[b], sizeof(WAVEHDR));
    }
}
