/* linux_audio.c - Sound on Linux: a small software mixer on PulseAudio (PipeWire speaks it too), plus MY SONGS
 * decoded with ffmpeg. libpulse-simple is opened at run time, so without it the game just stays silent.
 * The mixer runs on its own thread; the voices and the song list are shared with the main thread under `mix`. */
#include "linux.h"
#include <dlfcn.h>
#include <sys/stat.h>
#include <unistd.h>

#define ABUF 384
#define NVOICE 24

/* the few libpulse-simple declarations we need (pulse/simple.h) */
typedef struct { int format; unsigned rate; unsigned char channels; } PaSpec;
typedef struct { unsigned maxlength, tlength, prebuf, minreq, fragsize; } PaAttr;
enum { PA_SAMPLE_S16LE = 3, PA_STREAM_PLAYBACK = 1 };
static void *(*paNew)(const char *, const char *, int, const char *, const char *, const PaSpec *, const void *, const PaAttr *, int *);
static int (*paWrite)(void *, const void *, size_t, int *);
static void (*paFree)(void *);
static void *pa;

typedef struct { int s, pos; float vol; } Voice;
static Voice voices[NVOICE];
static int musPos;
static float musVol, musTarget; /* the looping soundtrack, faded toward musTarget */
static volatile int wantTrack = -1, running;
static pthread_t mixer;
static pthread_mutex_t mix = PTHREAD_MUTEX_INITIALIZER;

void plat_sound(int id, float vol)
{
    if (!pa) return;
    pthread_mutex_lock(&mix);
    int best = 0, bestpos = -1;
    for (int i = 0; i < NVOICE; i++) {
        if (voices[i].s < 0) { best = i; break; }
        if (voices[i].pos > bestpos) { bestpos = voices[i].pos; best = i; }
    }
    voices[best].s = id;
    voices[best].pos = 0;
    voices[best].vol = vol;
    pthread_mutex_unlock(&mix);
}

void plat_music(float vol) { musTarget = vol; }

void audioFrame(void) { wantTrack = game_music_track(); }

/* MY SONGS: the player's own music files
   The picked files are copied into my_songs/ in the save folder; my_songs/songs.txt lists them ("map<TAB>name<TAB>file").
   ffmpeg (when the computer has it) decodes them to 22050 Hz mono on a background thread the first time one plays. */
#define MY_MAX 8
static MySong *mySongs[MY_MAX];
static int myN;
char songDir[PATH_MAX + 16], songIndex[PATH_MAX + 32];

static void songFree(MySong *s) { free(s->pcm); free(s); }

/* a path inside '...' for the shell */
static void quoted(char *out, size_t cap, const char *path)
{
    size_t n = 0;
    out[n++] = '\'';
    for (const char *p = path; *p && n + 5 < cap; p++) {
        if (*p == '\'') { memcpy(out + n, "'\\''", 4); n += 4; }
        else out[n++] = *p;
    }
    out[n++] = '\'';
    out[n] = 0;
}

void *songDecode(void *arg)
{
    MySong *s = (MySong *)arg;
    char a[PATH_MAX + 64], q[2 * PATH_MAX + 64], cmd[2 * PATH_MAX + 160];
    short *buf = NULL;
    int len = 0, cap = 0, ok = 0;
    snprintf(a, sizeof a, "%s/%s", songDir, s->file);
    quoted(q, sizeof q, a);
    snprintf(cmd, sizeof cmd, "ffmpeg -nostdin -v quiet -i %s -vn -f s16le -ac 1 -ar %d - 2>/dev/null", q, SND_RATE);
    FILE *p = popen(cmd, "r");
    if (p) {
        for (;;) {
            if (len + 16384 > cap) {
                short *nb = realloc(buf, (size_t)(cap + 1048576) * sizeof(short));
                if (!nb) break;
                buf = nb; cap += 1048576;
            }
            size_t got = fread(buf + len, sizeof(short), 16384, p);
            if (!got) break;
            len += (int)got;
        }
        pclose(p);
        ok = len > SND_RATE / 2;
    }
    pthread_mutex_lock(&mix);
    if (ok) { s->pcm = buf; s->len = len; } else free(buf);
    s->state = ok ? 2 : 3;
    int dead = s->dead;
    pthread_mutex_unlock(&mix);
    if (dead) songFree(s); /* removed while we were busy */
    return NULL;
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
static void songName(const char *path, char *out)
{
    const char *b = strrchr(path, '/'), *dot;
    int n = 0, sp = 1;
    b = b ? b + 1 : path;
    dot = strrchr(b, '.');
    for (const char *p = b; *p && p != dot && n < MY_NAME - 1; p++) {
        char c = *p >= 'a' && *p <= 'z' ? (char)(*p - 32) : *p;
        int keep = (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c && strchr("!?.,:'()+-/", c));
        if (keep) { out[n++] = c; sp = 0; }
        else if (!sp) { out[n++] = ' '; sp = 1; }
    }
    while (n && out[n - 1] == ' ') n--;
    out[n] = 0;
    if (!n) strcpy(out, "MY SONG");
}

static int copyFile(const char *src, const char *dst)
{
    char b[65536];
    size_t n;
    int ok = 1;
    FILE *in = fopen(src, "rb"), *out = in ? fopen(dst, "wb") : NULL;
    if (!out) { if (in) fclose(in); return 0; }
    while ((n = fread(b, 1, sizeof b, in)) > 0) if (fwrite(b, 1, n, out) != n) { ok = 0; break; }
    fclose(in);
    if (fclose(out)) ok = 0;
    if (!ok) remove(dst);
    return ok;
}

static void songAddFile(const char *src)
{
    if (myN >= MY_MAX) return;
    MySong *s = calloc(1, sizeof *s);
    if (!s) return;
    const char *dot = strrchr(src, '.'), *sl = strrchr(src, '/');
    char ext[8] = "", a[PATH_MAX + 64];
    if (dot && (!sl || dot > sl)) for (int i = 0; dot[i] && i < 6; i++) ext[i] = (char)(dot[i] >= 'A' && dot[i] <= 'Z' ? dot[i] + 32 : dot[i]);
    for (int k = 1; k < 1000; k++) { /* a free file name: song1.flac, song2.mp3, ... */
        snprintf(s->file, sizeof s->file, "song%d%s", k, ext);
        snprintf(a, sizeof a, "%s/%s", songDir, s->file);
        if (access(a, F_OK) != 0) break;
    }
    if (!copyFile(src, a)) { free(s); return; }
    songName(src, s->name);
    s->map = -1;
    pthread_mutex_lock(&mix);
    mySongs[myN++] = s;
    pthread_mutex_unlock(&mix);
}

static int have(const char *tool)
{
    char cmd[96];
    snprintf(cmd, sizeof cmd, "command -v %s >/dev/null 2>&1", tool);
    return system(cmd) == 0;
}

/* the file picker: zenity (GNOME and most desktops) or kdialog (KDE); songs need ffmpeg to play */
int plat_song_add(void)
{
    static int tools = -1;
    if (tools < 0) tools = have("ffmpeg") ? (have("zenity") ? 1 : have("kdialog") ? 2 : 0) : 0;
    if (!tools) return 0;
    FILE *p = popen(tools == 1 ? "zenity --file-selection --multiple --separator='\n' --title='Add your own songs' "
                                 "--file-filter='Music | *.mp3 *.flac *.wav *.m4a *.aac *.wma *.ogg *.opus' --file-filter='All files | *' 2>/dev/null"
                               : "kdialog --title 'Add your own songs' --multiple --separate-output --getopenfilename ~ "
                                 "'*.mp3 *.flac *.wav *.m4a *.aac *.wma *.ogg *.opus|Music' 2>/dev/null", "r");
    if (!p) return 0;
    char line[PATH_MAX + 2];
    int added = 0;
    mkdir(songDir, 0755);
    while (fgets(line, sizeof line, p)) {
        line[strcspn(line, "\r\n")] = 0;
        if (*line) { songAddFile(line); added = 1; }
    }
    pclose(p);
    if (added) { songsSave(); songsPush(); }
    return 1;
}

void plat_song_remove(int i)
{
    if (i < 0 || i >= myN) return;
    char a[PATH_MAX + 64];
    pthread_mutex_lock(&mix);
    MySong *s = mySongs[i];
    memmove(&mySongs[i], &mySongs[i + 1], (size_t)(myN - i - 1) * sizeof mySongs[0]);
    myN--;
    int busy = s->state == 1;
    if (busy) s->dead = 1; /* the decoder frees it when it's done */
    pthread_mutex_unlock(&mix);
    snprintf(a, sizeof a, "%s/%s", songDir, s->file);
    remove(a);
    if (!busy) songFree(s);
    songsSave();
    songsPush();
}

void plat_song_map(int i, int map)
{
    if (i < 0 || i >= myN) return;
    mySongs[i]->map = map;
    songsSave();
}

/* the song for track t (10 and up), decoded on first use; NULL while it isn't ready (called with `mix` held) */
static const short *songPcm(int t, int *len)
{
    int i = t - 10;
    if (i < 0 || i >= myN) return NULL;
    MySong *s = mySongs[i];
    if (s->state == 0) {
        pthread_t th;
        s->state = 1;
        if (pthread_create(&th, NULL, songDecode, s) == 0) pthread_detach(th); else s->state = 3;
    }
    if (s->state != 2) return NULL;
    *len = s->len;
    return s->pcm;
}

static void *mixLoop(void *arg)
{
    (void)arg;
    static short out[ABUF];
    int track = -1;
    while (running) {
        int acc[ABUF] = { 0 };
        pthread_mutex_lock(&mix);
        if (wantTrack != track) { track = wantTrack; musPos = 0; musVol = 0; } /* a new song: start it from the top, faded in */
        int mlen = 0;
        float lvl = 0.5f;
        const short *mus = NULL;
        if (track >= 10) { mus = songPcm(track, &mlen); lvl = 0.25f; } /* MY SONGS are mastered much louder */
        else if (track >= 0) { mus = game_music_data(); mlen = game_music_len(); }
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
        pthread_mutex_unlock(&mix);
        for (int j = 0; j < ABUF; j++)
            out[j] = (short)(acc[j] > 32767 ? 32767 : acc[j] < -32768 ? -32768 : acc[j]);
        int err;
        if (paWrite(pa, out, sizeof out, &err) < 0) usleep(ABUF * 1000000 / SND_RATE); /* the sound server went away: keep time */
    }
    return NULL;
}

void audioInit(void)
{
    for (int i = 0; i < NVOICE; i++) voices[i].s = -1;
    void *lib = dlopen("libpulse-simple.so.0", RTLD_NOW);
    if (!lib) return;
    paNew = (void *(*)(const char *, const char *, int, const char *, const char *, const PaSpec *, const void *, const PaAttr *, int *))dlsym(lib, "pa_simple_new");
    paWrite = (int (*)(void *, const void *, size_t, int *))dlsym(lib, "pa_simple_write");
    paFree = (void (*)(void *))dlsym(lib, "pa_simple_free");
    if (!paNew || !paWrite || !paFree) return;
    PaSpec ss = { PA_SAMPLE_S16LE, SND_RATE, 1 };
    PaAttr at = { (unsigned)-1, ABUF * 2 * 4, (unsigned)-1, ABUF * 2, (unsigned)-1 }; /* about 70 ms queued: quick to react */
    int err;
    pa = paNew(NULL, "Night City Siege", PA_STREAM_PLAYBACK, NULL, "game", &ss, NULL, &at, &err);
    if (!pa) return;
    running = 1;
    if (pthread_create(&mixer, NULL, mixLoop, NULL) != 0) { running = 0; paFree(pa); pa = NULL; }
}

void audioQuit(void)
{
    if (!pa) return;
    running = 0;
    pthread_join(mixer, NULL);
    paFree(pa);
    pa = NULL;
}
