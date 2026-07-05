/* one game: my AI (2023184027) vs OPP. argv[1]=B/W (my color). prints MINE/FRIEND/DRAW */
#define NOMINMAX
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "../2023184027.h"   /* my AI + ec_* referee + EC_* */
#include "opp.h"              /* opponent (copied) */

#define CAT(a,b) a##b
#define XC(a,b) CAT(a,b)
#define oBA XC(BlackAttack_,OPP)
#define oBD XC(BlackDefence_,OPP)
#define oWA XC(WhiteAttack_,OPP)
#define oWD XC(WhiteDefence_,OPP)

static int M[EC_SZ][EC_SZ];

static int ref_check(int r, int c, int color) {
    M[r][c] = EC_EMPTY;
    ECFeat f = ec_features(M, r, c, color);
    M[r][c] = color;
    if (f.win) return 1;
    if (f.overline) return 2;
    if (color == EC_BLACK && (f.fours >= 2 || f.threes >= 2)) return 2;
    return 0;
}

int main(int argc, char** argv) {
    int myColor = (argc > 1 && argv[1][0] == 'B') ? EC_BLACK : EC_WHITE;
    memset(M, 0, sizeof(M));
    int turn = EC_BLACK, why = 0, result = 0;
    for (int ply = 0; ply < EC_SZ * EC_SZ; ply++) {
        int e = 0;
        for (int i = 0; i < EC_SZ && !e; i++) for (int j = 0; j < EC_SZ; j++) if (M[i][j] == EC_EMPTY) { e = 1; break; }
        if (!e) { result = 0; break; }
        int x = -1, y = -1, mine = (turn == myColor);
        if (turn == EC_BLACK) { if (mine) BlackAttack_2023184027(&x, &y); else oBA(&x, &y); }
        else                  { if (mine) WhiteAttack_2023184027(&x, &y); else oWA(&x, &y); }
        if (x < 0 || x >= EC_SZ || y < 0 || y >= EC_SZ || M[y][x] != EC_EMPTY) { result = mine ? -1 : 1; why = 2; break; }
        M[y][x] = turn;
        int rr = ref_check(y, x, turn);
        if (turn == EC_BLACK) { if (mine) oWD(x, y); else WhiteDefence_2023184027(x, y); }
        else                  { if (mine) oBD(x, y); else BlackDefence_2023184027(x, y); }
        if (rr == 1) { result = mine ? 1 : -1; break; }
        if (rr == 2) { result = mine ? -1 : 1; why = 1; break; }
        turn = (turn == EC_BLACK) ? EC_WHITE : EC_BLACK;
    }
    printf("%s%s\n", result > 0 ? "MINE" : (result < 0 ? "FRIEND" : "DRAW"), why == 1 ? "(foul)" : (why == 2 ? "(illegal)" : ""));
    return 0;
}
