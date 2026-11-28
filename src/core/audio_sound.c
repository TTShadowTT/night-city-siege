/* audio_sound.c - Sound effects, synthesised into sample arrays at start-up. */
#include "core.h"

static const float sdur[S_COUNT] = { 0.1f, 0.06f, 0.25f, 0.35f, 0.18f, 0.3f, 0.6f, 0.05f, 0.18f, 0.3f, 0.2f,
                                     0.7f, 0.15f, 0.1f, 0.9f, 0.05f, 0.1f, 0.12f, 0.22f, 0.5f, 0.14f, 0.45f };
int sndOff[S_COUNT], sndLen[S_COUNT];

static float sq(float ph) { return sinf(ph) > 0 ? 1.f : -1.f; }

void genSounds(void)
{
    static const float notes[4] = { 523, 659, 784, 1046 };
    int off = 0;
    for (int id = 0; id < S_COUNT; id++) {
        int n = (int)(sdur[id] * SND_RATE);
        short *d = sndPool + off;
        float ph = 0, lp = 0, lp2 = 0;
        for (int i = 0; i < n; i++) {
            float t = i / (float)SND_RATE, p = t / sdur[id], v = 0, f, nz = frand() * 2 - 1;
            lp += (nz - lp) * 0.12f;
            lp2 += (nz - lp2) * 0.5f;
            switch (id) {
            case S_PISTOL:
                f = 900 - 700 * p; ph += 2 * PI * f / SND_RATE;
                v = (sq(ph) * 0.35f + nz * 0.25f * (1 - p)) * (1 - p) * (1 - p);
                break;
            case S_SMG:
                f = 700 - 400 * p; ph += 2 * PI * f / SND_RATE;
                v = (sq(ph) * 0.2f + lp2 * 0.3f) * (1 - p);
                break;
            case S_SHOTGUN:
                f = 120 - 70 * p; ph += 2 * PI * f / SND_RATE;
                v = (lp * 1.2f + sinf(ph) * 0.5f) * powf(1 - p, 2.2f);
                break;
            case S_LASER:
                f = 2200 - 1900 * p; ph += 2 * PI * f / SND_RATE;
                v = (sinf(ph) * 0.4f + sq(ph * 0.5f) * 0.15f) * (1 - p);
                break;
            case S_LAUNCH:
                f = 160 - 80 * p; ph += 2 * PI * f / SND_RATE;
                v = (sinf(ph) * 0.6f + lp * 0.4f) * (1 - p) * (1 - p);
                break;
            case S_ROCKET:
                f = 180 + 300 * p; ph += 2 * PI * f / SND_RATE;
                v = (lp2 * 0.5f + sq(ph) * 0.12f) * sinf(PI * p);
                break;
            case S_BOOM:
                f = 70 - 40 * p; ph += 2 * PI * f / SND_RATE;
                v = (lp * 1.4f + sinf(ph) * 0.6f) * powf(1 - p, 1.6f);
                break;
            case S_HIT:
                f = 320 - 160 * p; ph += 2 * PI * f / SND_RATE;
                v = sq(ph) * 0.25f * (1 - p);
                break;
            case S_KILL:
                f = 420 * (1 - p) + 60; ph += 2 * PI * f / SND_RATE;
                v = (sq(ph) * 0.3f + lp * 0.5f) * (1 - p);
                break;
            case S_HURT:
                f = 520 - 430 * p; ph += 2 * PI * f / SND_RATE;
                v = (fmodf(ph / (2 * PI), 1) * 2 - 1) * 0.5f * (1 - p) + lp * 0.3f * (1 - p);
                break;
            case S_PICKUP:
                f = p < 0.33f ? 660 : p < 0.66f ? 880 : 1320; ph += 2 * PI * f / SND_RATE;
                v = sq(ph) * 0.2f * (1 - p * 0.6f);
                break;
            case S_WAVE: {
                int k = (int)(p * 4);
                if (k > 3) k = 3;
                float q = p * 4 - k;
                f = notes[k]; ph += 2 * PI * f / SND_RATE;
                v = (sq(ph) * 0.15f + sinf(ph) * 0.2f) * (1 - q * 0.7f) * (k == 3 ? 1 - q : 1);
            } break;
            case S_DASH:
                v = (lp2 - lp) * 0.7f * sinf(PI * p);
                break;
            case S_ESHOT:
                f = 500 - 200 * p; ph += 2 * PI * f / SND_RATE;
                v = sq(ph) * 0.18f * (1 - p);
                break;
            case S_BOSS:
                f = 65 + 8 * sinf(t * 20); ph += 2 * PI * f / SND_RATE;
                v = ((fmodf(ph / (2 * PI), 1) * 2 - 1) * 0.5f + lp * 0.5f) * sinf(PI * p);
                break;
            case S_SWITCH:
                f = 1500; ph += 2 * PI * f / SND_RATE;
                v = sq(ph) * 0.15f * (1 - p);
                break;
            case S_JUMP:
                f = 300 + 500 * p; ph += 2 * PI * f / SND_RATE;
                v = sq(ph) * 0.15f * (1 - p);
                break;
            case S_ROPE:
                v = lp2 * 0.3f * (1 - p) * (0.5f + 0.5f * sq(t * 300));
                break;
            case S_WARP: /* deep wobbling whoosh of a black hole opening */
                f = 260 - 200 * p + 40 * sinf(t * 45); ph += 2 * PI * f / SND_RATE;
                v = (sinf(ph) * 0.5f + sq(ph * 0.5f) * 0.12f + lp * 0.25f) * sinf(PI * p);
                break;
            case S_RELOAD: /* click ... clack */
                f = p < 0.5f ? 1700 : 1100; ph += 2 * PI * f / SND_RATE;
                v = (p < 0.12f || (p > 0.62f && p < 0.78f)) ? sq(ph) * 0.25f + lp2 * 0.35f : 0;
                break;
            case S_CLANG: /* bullet pinging off a riot shield */
                f = 1900 + 300 * sinf(t * 900); ph += 2 * PI * f / SND_RATE;
                v = (sinf(ph) * 0.35f + sinf(ph * 2.76f) * 0.2f + lp2 * 0.2f * (1 - p)) * powf(1 - p, 2);
                break;
            case S_PERK: { /* rising sparkle */
                int k = (int)(p * 6);
                f = notes[k % 4] * (k >= 4 ? 2 : 1); ph += 2 * PI * f / SND_RATE;
                v = (sinf(ph) * 0.3f + sq(ph) * 0.1f) * (1 - p * 0.8f);
            } break;
            }
            v = floorf(clampf(v, -1, 1) * 24) / 24; /* crunchy 8-bit quantisation */
            d[i] = (short)(v * 32767 * 0.8f);
        }
        sndOff[id] = off;
        sndLen[id] = n;
        off += n;
    }
}
