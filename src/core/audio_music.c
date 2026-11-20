/* audio_music.c - The soundtracks (8-bar synth loops, one per kind of district) and MY SONGS. */
#include "core.h"

static float saw(float ph) { return 2 * (ph - floorf(ph)) - 1; }
static float tri(float ph) { float f = ph - floorf(ph); return f < 0.5f ? 4 * f - 1 : 3 - 4 * f; }

/* a track: tempo (samples per 16th), drum / arp / lead / bass flavours, how loud the pad is,
   then MIDI notes: 4 bass roots and 4 chords (one per bar, round and round) and the melody in quarter notes
   (0 = rest, -1 = hold the note before) */
enum { DR_FOUR, DR_HALF, DR_BREAK, DR_NONE };
enum { AR_PULSE16, AR_TRI8, AR_NONE };
enum { LD_SYNTH, LD_SQUARE, LD_BELL };
enum { BS_EIGHTHS, BS_LONG, BS_PUMP };
const Track TRACKS[MUS_TRACKS] = {
    { "NIGHT DRIVE", 2950, DR_FOUR, AR_PULSE16, LD_SYNTH, BS_EIGHTHS, 1, { 45, 41, 48, 43 }, /* the original: Am F C G */
      { { 57, 60, 64 }, { 53, 57, 60 }, { 60, 64, 67 }, { 55, 59, 62 } },
      { { 0 }, { 0 }, { 0 }, { 0 }, { 76, -1, 72, -1 }, { 69, -1, 72, -1 }, { 67, -1, 76, -1 }, { 74, -1, 71, -1 } } },
    { "NEON RAIN", 2756, DR_FOUR, AR_PULSE16, LD_SQUARE, BS_PUMP, 0.8f, { 38, 34, 41, 36 },
      { { 62, 65, 69 }, { 58, 62, 65 }, { 60, 65, 69 }, { 60, 64, 67 } },
      { { 0 }, { 0 }, { 74, -1, 77, 76 }, { 72, -1, 69, -1 }, { 74, -1, 81, -1 }, { 79, 77, -1, 74 }, { 77, -1, 76, 72 }, { 74, -1, -1, 0 } } },
    { "HARBOR FOG", 3595, DR_HALF, AR_TRI8, LD_BELL, BS_LONG, 1.4f, { 40, 36, 43, 38 },
      { { 55, 59, 64 }, { 55, 60, 64 }, { 55, 59, 62 }, { 54, 57, 62 } },
      { { 0 }, { 0 }, { 71, -1, -1, 67 }, { 69, -1, -1, -1 }, { 71, -1, 74, -1 }, { 72, -1, 71, 67 }, { 66, -1, -1, 64 }, { 67, -1, -1, -1 } } },
    { "SKYLINE RUSH", 2432, DR_BREAK, AR_PULSE16, LD_SYNTH, BS_PUMP, 0.7f, { 40, 36, 38, 35 },
      { { 59, 64, 67 }, { 60, 64, 67 }, { 57, 62, 66 }, { 59, 63, 66 } },
      { { 76, 74, 76, 79 }, { 76, -1, 72, -1 }, { 74, 72, 74, 78 }, { 75, -1, 71, -1 }, { 79, -1, 78, 76 }, { 72, -1, 76, 79 }, { 81, -1, 79, 78 }, { 75, -1, -1, 0 } } },
    { "GREEN HOLLOW", 3180, DR_HALF, AR_TRI8, LD_BELL, BS_EIGHTHS, 1.1f, { 41, 36, 38, 34 },
      { { 57, 60, 65 }, { 55, 60, 64 }, { 57, 62, 65 }, { 58, 62, 65 } },
      { { 0 }, { 72, -1, 74, 76 }, { 77, -1, 76, 72 }, { 74, -1, -1, 0 }, { 69, 72, 77, -1 }, { 76, -1, 72, -1 }, { 74, 76, 77, 79 }, { 77, -1, -1, 0 } } },
    { "THUNDERHEAD", 2625, DR_FOUR, AR_PULSE16, LD_SQUARE, BS_PUMP, 0.9f, { 36, 32, 39, 34 },
      { { 60, 63, 67 }, { 56, 60, 63 }, { 58, 63, 67 }, { 58, 62, 65 } },
      { { 0 }, { 0 }, { 72, -1, 75, -1 }, { 74, 72, 70, -1 }, { 67, -1, 72, 75 }, { 79, -1, 77, 75 }, { 74, -1, 70, 74 }, { 72, -1, -1, 0 } } },
    { "SUMMIT", 3307, DR_HALF, AR_TRI8, LD_SYNTH, BS_LONG, 1.2f, { 38, 45, 47, 43 },
      { { 62, 66, 69 }, { 61, 64, 69 }, { 62, 66, 71 }, { 62, 67, 71 } },
      { { 0 }, { 0 }, { 78, -1, -1, 76 }, { 73, -1, 74, -1 }, { 78, -1, 81, -1 }, { 76, -1, -1, 74 }, { 71, -1, 74, 78 }, { 79, -1, -1, 0 } } },
    { "CORPO PLAZA", 2803, DR_FOUR, AR_PULSE16, LD_SQUARE, BS_EIGHTHS, 0.9f, { 42, 38, 45, 40 },
      { { 61, 66, 69 }, { 62, 66, 69 }, { 61, 64, 69 }, { 59, 64, 68 } },
      { { 0 }, { 0 }, { 73, -1, -1, -1 }, { 74, -1, 73, 71 }, { 69, -1, -1, -1 }, { 71, -1, 68, -1 }, { 73, 76, 78, -1 }, { 76, -1, 73, -1 } } },
    { "LUNAR DUST", 3938, DR_NONE, AR_TRI8, LD_BELL, BS_LONG, 1.6f, { 45, 41, 48, 40 },
      { { 57, 60, 64 }, { 53, 57, 64 }, { 55, 60, 64 }, { 56, 59, 64 } },
      { { 0 }, { 76, -1, -1, -1 }, { 72, -1, -1, -1 }, { 71, -1, 68, -1 }, { 69, -1, -1, -1 }, { 72, -1, 76, -1 }, { 79, -1, -1, 76 }, { 76, -1, -1, -1 } } },
    { "LUCY'S DREAM", 3445, DR_HALF, AR_PULSE16, LD_SYNTH, BS_EIGHTHS, 1.3f, { 43, 38, 40, 36 }, /* dreamy and bittersweet, for the two of them */
      { { 59, 62, 67 }, { 57, 62, 66 }, { 59, 64, 67 }, { 60, 64, 67 } },
      { { 0 }, { 0 }, { 74, -1, 71, 74 }, { 76, -1, 74, -1 }, { 79, -1, 78, 76 }, { 74, -1, 71, -1 }, { 72, 74, 76, -1 }, { 74, -1, -1, 0 } } },
};
/* the track each district plays when the soundtrack is set to BY MAP */
const signed char MAP_TRACK[] = { 0, 1, 2, 3, 4, 4, 5, 6, 1, 7, 7, 8, 9, 9, 3 };
int musReady[MUS_TRACKS], musTrack;

static float midiF(int m) { return 440.f * powf(2.f, (m - 69) / 12.f); }

static void genTrack(int t)
{
    static const unsigned short KICK[4] = { 0x1111, 0x0401, 0x0441, 0 }, SNARE[4] = { 0x1010, 0x0100, 0x5010, 0 };
    static const int bassOct[8] = { 1, 1, 2, 1, 1, 2, 1, 2 };
    static const int arpSeq[8] = { 0, 1, 2, 1, 0, 2, 1, 2 };
    const Track *T = &TRACKS[t];
    short *out = musPool + t * MUS_MAXLEN;
    int STEP = T->step, len = STEP * 128, kickAt = -1000000, snAt = -1000000;
    float rootF[4], chF[4][3], melF[8][4];
    float pph[6] = { 0 }, plp = 0, bph = 0, blp = 0, aph = 0, lph = 0, nlp = 0, kph = 0;
    for (int c = 0; c < 4; c++) {
        rootF[c] = midiF(T->root[c]);
        for (int k = 0; k < 3; k++) chF[c][k] = midiF(T->chord[c][k]);
    }
    for (int b = 0; b < 8; b++)
        for (int q = 0; q < 4; q++) melF[b][q] = T->mel[b][q] > 0 ? midiF(T->mel[b][q]) : 0;
    for (int i = 0; i < len; i++) {
        int step = i / STEP, bar = step / 16, inBar = step % 16, c = bar % 4, s16 = i % STEP;
        float ts = s16 / (float)SND_RATE, stepLen = STEP / (float)SND_RATE, barLen = 16 * stepLen;
        float nz = frand() * 2 - 1, v = 0, rel = fminf(1, (stepLen - ts) / 0.004f); /* tiny release so notes never click */
        nlp += (nz - nlp) * 0.3f;
        if (s16 == 0 && (KICK[T->drum] >> inBar & 1)) { kickAt = i; kph = 0; }
        if (s16 == 0 && (SNARE[T->drum] >> inBar & 1)) snAt = i;
        /* kick and snare */
        float tk = (i - kickAt) / (float)SND_RATE, kenv = expf(-tk * 9);
        kph += (45 + 90 * expf(-tk * 30)) / SND_RATE;
        v += sinf(kph * 2 * PI) * expf(-tk * 14) * 0.55f;
        {
            float tsn = (i - snAt) / (float)SND_RATE, ghost = T->drum == DR_BREAK && inBar >= 14 ? 0.4f : 1;
            v += ((nz - nlp) * 0.5f + sinf(tsn * 2 * PI * 185) * 0.2f) * expf(-tsn * 16) * 0.5f * ghost;
        }
        /* hats: closed 16ths, louder off the beat; only soft 8ths when there are no drums */
        if (T->drum == DR_NONE) { if (inBar % 2 == 0) v += (nz - nlp) * expf(-ts * 60) * 0.025f; }
        else v += (nz - nlp) * expf(-ts * (inBar % 4 == 2 ? 28 : 70)) * (inBar % 4 == 2 ? 0.14f : 0.06f);
        /* bass, ducked by the kick */
        {
            float f = rootF[c], env, w;
            if (T->bass == BS_LONG) { /* one long note a bar */
                float tb = inBar * stepLen + ts;
                env = fminf(1, tb / 0.05f) * fminf(1, (barLen - tb) / 0.02f) * 0.8f;
                w = 0.05f;
            } else if (T->bass == BS_PUMP) { /* driving 16ths, octave jump on the off-beat */
                f *= inBar % 4 == 2 ? 2 : 1;
                env = expf(-ts * 8) * fminf(1, ts / 0.003f) * rel;
                w = 0.08f + 0.3f * expf(-ts * 20);
            } else { /* 8ths */
                float t8 = (step % 2) * stepLen + ts;
                f *= bassOct[step / 2 % 8];
                env = expf(-t8 * 3) * fminf(1, t8 / 0.003f) * ((step % 2) ? rel : 1);
                w = 0.08f + 0.25f * expf(-t8 * 12);
            }
            bph += f / SND_RATE;
            blp += (saw(bph) - blp) * w;
            v += blp * env * 0.42f * (1 - 0.6f * kenv);
        }
        /* pad: detuned saws through a soft low-pass, swelling each bar */
        {
            float tb = inBar * stepLen + ts, pad = 0;
            for (int k = 0; k < 3; k++) {
                pph[k * 2] += chF[c][k] * 1.003f / SND_RATE;
                pph[k * 2 + 1] += chF[c][k] * 0.997f / SND_RATE;
                pad += saw(pph[k * 2]) + saw(pph[k * 2 + 1]);
            }
            plp += (pad - plp) * 0.035f;
            float env = fminf(1, tb / 0.08f) * fminf(1, (barLen - tb) / 0.03f) * (0.75f + 0.25f * sinf(tb * 2 * PI / barLen));
            v += plp * env * 0.075f * T->pad * (1 - 0.45f * kenv);
        }
        /* arpeggio over the chord */
        if (T->arp == AR_PULSE16) {
            float f = chF[c][arpSeq[inBar % 8]] * (inBar >= 8 && bar % 2 ? 4 : 2);
            aph += f / SND_RATE;
            float pw = (aph - floorf(aph)) < 0.3f ? 1.f : -1.f;
            v += pw * expf(-ts * 18) * fminf(1, ts / 0.002f) * rel * (bar < 1 ? 0.03f : 0.075f);
        } else if (T->arp == AR_TRI8) {
            float t8 = (inBar % 2) * stepLen + ts, f = chF[c][arpSeq[inBar / 2 % 4]] * 2;
            aph += f / SND_RATE;
            v += tri(aph) * expf(-t8 * 5) * fminf(1, t8 / 0.004f) * (inBar % 2 ? rel : 1) * 0.09f;
        }
        /* lead melody in quarter notes; -1 holds the note before */
        {
            int q = inBar / 4, qs = q, ql = 1;
            while (qs > 0 && T->mel[bar][qs] < 0) qs--;
            while (qs + ql < 4 && T->mel[bar][qs + ql] < 0) ql++;
            float f = T->mel[bar][qs] > 0 ? melF[bar][qs] : 0;
            if (f > 0) {
                float th = ((q - qs) * 4 + inBar % 4) * stepLen + ts, hl = ql * 4 * stepLen;
                float vib = 1 + 0.006f * sinf(th * 2 * PI * 5.5f) * fminf(1, th / 0.3f);
                lph += f * vib / SND_RATE;
                float env = fminf(1, th / 0.02f) * fminf(1, (hl - th) / 0.02f), w;
                if (T->lead == LD_BELL) w = (sinf(lph * 2 * PI) + 0.35f * sinf(lph * 4 * PI) * expf(-th * 6)) * (0.35f + 0.65f * expf(-th * 3));
                else if (T->lead == LD_SQUARE) w = ((lph - floorf(lph)) < 0.5f ? 0.55f : -0.55f) * (0.6f + 0.4f * expf(-th * 3));
                else w = (tri(lph) * 0.7f + saw(lph) * 0.15f) * (0.6f + 0.4f * expf(-th * 3));
                v += w * env * 0.12f;
            }
        }
        v = v / (1 + fabsf(v)) * 1.25f; /* soft clip */
        out[i] = (short)(clampf(v, -1, 1) * 32767 * 0.9f);
    }
    /* fade the first and last few ms together so the loop seam is silent */
    for (int i = 0; i < 64; i++) {
        out[i] = (short)(out[i] * (i / 64.f));
        out[len - 1 - i] = (short)(out[len - 1 - i] * (i / 64.f));
    }
    musReady[t] = 1;
}
int myCount;
char myNames[MY_MAX][MY_NAME];
signed char myMap[MY_MAX];
int musicMode, musFor = -1; /* SOUNDTRACK: 0 = BY MAP, 1 = RANDOM, 2 + n = always track n (MY SONGS after the built-in ones) */
char *game_song_names(void) { return &myNames[0][0]; }
signed char *game_song_maps(void) { return myMap; }
void game_songs_set(int n) /* the platform filled in the names and maps of its n songs */
{
    myCount = n < 0 ? 0 : n > MY_MAX ? MY_MAX : n;
    for (int i = 0; i < MY_MAX; i++) myNames[i][MY_NAME - 1] = 0;
    if (musicMode >= 2 + MUS_TRACKS + myCount) musicMode = 0; /* that song is gone */
    musFor = -1; /* pick again */
}
const char *trackName(int t) { return t < MUS_TRACKS ? TRACKS[t].name : t - MUS_TRACKS < myCount ? myNames[t - MUS_TRACKS] : "?"; }

/* the platform plays whatever track these point at, and starts over (with a crossfade) when game_music_track changes;
   tracks from MUS_TRACKS on are MY SONGS, which the platform plays from its own copy */
int game_music_track(void) { return musTrack; }
const short *game_music_data(void)
{
    int t = musTrack < MUS_TRACKS ? musTrack : 0;
    if (!musReady[t]) genTrack(t);
    return musPool + t * MUS_MAXLEN;
}
int game_music_len(void) { return TRACKS[musTrack < MUS_TRACKS ? musTrack : 0].step * 128; }

/* every sound effect goes through here so the SOUND EFFECTS slider applies */
int sfxVolPct = 100, musicVolPct = 100;
void sfx(int id, float vol)
{
    if (sfxOn && sfxVolPct > 0) plat_sound(id, vol * sfxVolPct / 100.f);
}

int game_sound_count(void) { return S_COUNT; }
const short *game_sound_data(int id) { return sndPool + sndOff[id]; }
int game_sound_len(int id) { return sndLen[id]; }
