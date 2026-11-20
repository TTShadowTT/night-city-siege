/* render_pets.c - Drawing the pets. */
#include "core.h"

/* one pet pixel's colour, or 0 for see-through */
static unsigned petPix(const Pet *P, char ch)
{
    switch (ch) {
    case 'b': return P->body;
    case 'd': return P->dark;
    case 'l': return P->light;
    case 'e': return P->eye;
    case 'w': return P->shine;
    case 'n': return P->nose;
    case 'c': return collarOn ? COLLAR_COL[collarCol] : P->body;
    case 'C': return collarOn ? COLLAR_COL[collarCol] : P->light;
    default: return 0;
    }
}

/* pet k centred on x with its feet on feetY, in sc x sc blocks, outlined */
void drawPet(int k, int x, int feetY, int flip, int frame, int sc)
{
    if (k <= 0 || k >= PET_COUNT) return;
    const Pet *P = &PETS[k];
    const PetShape *S = &PET_SHAPES[P->shape];
    int x0 = x - S->w * sc / 2, y0 = feetY - S->h * sc;
    const char *const *rows = S->rows[clampi(frame, 0, 2)];
#define PET_AT(i, j) ((i) >= 0 && (j) >= 0 && (j) < S->h && (i) < (int)strlen(rows[j]) && petPix(P, rows[j][i]))
    for (int pass = 0; pass < 2; pass++) /* outline first, then the pet on top */
        for (int j = -1; j <= S->h; j++)
            for (int i = -1; i <= S->w; i++) {
                unsigned c;
                if (pass == 0) {
                    if (PET_AT(i, j) || !(PET_AT(i - 1, j) || PET_AT(i + 1, j) || PET_AT(i, j - 1) || PET_AT(i, j + 1))) continue;
                    c = C_OUT;
                } else {
                    if (!PET_AT(i, j)) continue;
                    c = petPix(P, rows[j][i]);
                }
                int sx = flip ? S->w - 1 - i : i;
                if (sc == 1) pset(x0 + sx, y0 + j, c); else fillRect(x0 + sx * sc, y0 + j * sc, sc, sc, c);
            }
#undef PET_AT
    if (petGoo(k)) { /* glossy goo streaks run down it, fat drops ooze off its sides into a puddle */
#define GOO(i, j, c) (sc == 1 ? pset(x0 + (i), y0 + (j), c) : fillRect(x0 + (i) * sc, y0 + (j) * sc, sc, sc, c))
        unsigned lt = P->light, dk = P->dark;
        for (int i = -3; i < S->w + 3; i++) /* puddle */
            if (i < 0 || i >= S->w) GOO(flip ? S->w - 1 - i : i, S->h - 1, i == -3 || i == S->w + 2 ? dk : i == -2 ? lt : P->body);
        static const int col[3] = { 2, 5, 7 };
        for (int d = 0; d < 3; d++) { /* dark goo channels creep down its face, a bright bead at the tip */
            float ph = gt * 0.7f + d * 0.37f;
            ph -= floorf(ph);
            int len = 1 + (int)(ph * (S->h - 2));
            for (int j = 1; j <= len && j < S->h - 1; j++)
                if (rows[j][col[d]] == 'b') GOO(flip ? S->w - 1 - col[d] : col[d], j, j == len ? lt : dk);
        }
        for (int s = 0; s < 2; s++) { /* fat drops slide down its sides into the puddle */
            float ph = gt * 0.9f + s * 0.5f;
            ph -= floorf(ph);
            int j = 1 + (int)(ph * (S->h - 2)), e = s ? S->w - 1 : 0;
            while (e >= 0 && e < S->w && rows[j][e] == '.') e += s ? -1 : 1;
            int i = e + (s ? 1 : -1);
            if (flip) i = S->w - 1 - i;
            GOO(i, j, lt);
            if (j > 1) GOO(i, j - 1, P->body);
        }
        if (k == PET_TOXIC) { /* toxic drool from its mouth */
            float ph = gt * 1.3f;
            ph -= floorf(ph);
            for (int j = 5; j < 5 + (int)(ph * 3) && j < S->h; j++) GOO(flip ? S->w - 1 - 3 : 3, j, lt);
        }
#undef GOO
    }
}
