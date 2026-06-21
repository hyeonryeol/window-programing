/*
 * battle2.cpp  —  my AI (2023184027) vs friend (2023180015)
 *   compile: cl /EHsc /O2 /nologo battle2.cpp
 */
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "Omock_2023184027.h"   /* my AI + ec_* referee + EC_* */
#include "2023180015.h"          /* friend AI (namespace omok_2023180015) */

static int M[EC_SZ][EC_SZ];      /* master board: 1=black 2=white */

static void reset_all() {
    memset(M, 0, sizeof(M));
    memset(g_board_2023184027, 0, sizeof(g_board_2023184027));
    g_init_2023184027 = 1;
    memset(omok_2023180015_engine::g_board_2023184027, 0, sizeof(omok_2023180015_engine::g_board_2023184027));
    omok_2023180015_engine::g_init_2023184027 = 1;
}

static int ref_check(int r, int c, int color) {
    M[r][c] = EC_EMPTY;
    ECFeat f = ec_features(M, r, c, color);
    M[r][c] = color;
    if (f.win) return 1;
    if (f.overline) return 2;
    if (color == EC_BLACK && (f.fours >= 2 || f.threes >= 2)) return 2;
    return 0;
}

/* myColor = which color MY ai plays. returns +1 my win, -1 friend win, 0 draw */
static int play_one(int myColor, int* why) {
    reset_all();
    *why = 0;
    int turn = EC_BLACK;
    for (int ply = 0; ply < EC_SZ * EC_SZ; ply++) {
        int empty = 0;
        for (int i = 0; i < EC_SZ && !empty; i++) for (int j = 0; j < EC_SZ; j++) if (M[i][j] == EC_EMPTY) { empty = 1; break; }
        if (!empty) return 0;

        int x = -1, y = -1;
        int mine = (turn == myColor);
        if (turn == EC_BLACK) {
            if (mine) BlackAttack_2023184027(&x, &y); else BlackAttack_2023180015(&x, &y);
        } else {
            if (mine) WhiteAttack_2023184027(&x, &y); else WhiteAttack_2023180015(&x, &y);
        }
        if (x < 0 || x >= EC_SZ || y < 0 || y >= EC_SZ || M[y][x] != EC_EMPTY) {
            *why = 2; return mine ? -1 : +1;
        }
        M[y][x] = turn;
        int rr = ref_check(y, x, turn);
        if (turn == EC_BLACK) {
            if (mine) WhiteDefence_2023180015(x, y); else WhiteDefence_2023184027(x, y);
        } else {
            if (mine) BlackDefence_2023180015(x, y); else BlackDefence_2023184027(x, y);
        }
        if (rr == 1) { *why = 0; return mine ? +1 : -1; }
        if (rr == 2) { *why = 1; return mine ? -1 : +1; }
        turn = (turn == EC_BLACK) ? EC_WHITE : EC_BLACK;
    }
    return 0;
}

int main(int argc, char** argv) {
    int games = (argc > 1) ? atoi(argv[1]) : 10;
    printf("=== BATTLE: MINE(2023184027) vs FRIEND(2023180015), %d games each color ===\n", games);

    int mw = 0, fw = 0, dr = 0, ill = 0, foul = 0;
    for (int g = 0; g < games; g++) {
        int why; int r = play_one(EC_BLACK, &why);
        if (r > 0) mw++; else if (r < 0) fw++; else dr++;
        if (why == 2) ill++; if (why == 1) foul++;
        printf("  [MINE=BLACK] game %2d: %s\n", g+1, r>0?"MINE win":(r<0?"FRIEND win":"draw")); fflush(stdout);
    }
    printf("[MINE = BLACK] mine %d  friend %d  draw %d  (illegal %d, foul %d)\n", mw, fw, dr, ill, foul);

    int mw2 = 0, fw2 = 0, dr2 = 0, ill2 = 0, foul2 = 0;
    for (int g = 0; g < games; g++) {
        int why; int r = play_one(EC_WHITE, &why);
        if (r > 0) mw2++; else if (r < 0) fw2++; else dr2++;
        if (why == 2) ill2++; if (why == 1) foul2++;
        printf("  [MINE=WHITE] game %2d: %s\n", g+1, r>0?"MINE win":(r<0?"FRIEND win":"draw")); fflush(stdout);
    }
    printf("[MINE = WHITE] mine %d  friend %d  draw %d  (illegal %d, foul %d)\n", mw2, fw2, dr2, ill2, foul2);

    printf("\nTOTAL: mine %d, friend %d, draw %d  (of %d games)\n", mw+mw2, fw+fw2, dr+dr2, games*2);
    return 0;
}
