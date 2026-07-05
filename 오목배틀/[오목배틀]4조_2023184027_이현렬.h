/*
 * ================================================================
 *    AI    Omock_2023184027.h
 *
 *    engine_core.h   ( ,   ).
 *        .
 *
 *    ( ):
 *     BlackAttack(&x,&y)  mMap[y+10][x+10]  .
 *     x = (col), y = (row).
 *      board[row][col] 
 *       Attack:  ec_best -> (row,col),  *x=col, *y=row
 *       Defence:  (x,y)=(col,row)   -> board[y][x]
 *
 *  ():  5=, 6 ()=,  3-3/4-4 .
 *      ,    .
 * ================================================================
 */
#ifndef OMOCK_2023184027_H
#define OMOCK_2023184027_H

/*
 * engine_core.h     AI   ( ,  )
 *
 *  : board[row][col],  0= 1= 2=
 *  ( ):
 *    -  5 = ,  6 () = ()
 *    -  3-3, 4-4  
 *   19x19(0~18).   EndGame 0~17  
 *    0~17  .
 *
 *       (Omock_*.h) .
 */



#include <ctime>
#include <cstring>

#define EC_SZ      19      /*    */
#define EC_PLAYMAX 17      /*    (0~17) */
#define EC_EMPTY   0
#define EC_BLACK   1
#define EC_WHITE   2

/*      */
struct ECFeat {
    int win;        /*  5  */
    int overline;   /* 6 () */
    int fours;      /* 4(/ ,  5)   */
    int openfours;  /*   4   */
    int threes;     /*  3   */
};

/*  4  (dr,dc)  */
static const int EC_DR[4] = { 0, 1, 1, 1 };
static const int EC_DC[4] = { 1, 0, 1,-1 };

static inline int ec_on(int r, int c) {
    return r >= 0 && r < EC_SZ && c >= 0 && c < EC_SZ;
}

/*
 * (r,c) color  (b)   d   .
 * 11 ( index5 =  )  .
 *   M=, E=, B=(/)
 * :  (1=five, 2=overline, 4=four, 8=openfour, 16=openthree)
 */
static int ec_dir(const int b[EC_SZ][EC_SZ], int r, int c, int color, int d) {
    char s[11];
    int dr = EC_DR[d], dc = EC_DC[d];
    for (int k = -5; k <= 5; k++) {
        int rr = r + dr * k, cc = c + dc * k;
        if (k == 0) { s[k + 5] = 'M'; continue; }
        if (!ec_on(rr, cc))            s[k + 5] = 'B';
        else if (b[rr][cc] == color)   s[k + 5] = 'M';
        else if (b[rr][cc] == EC_EMPTY)s[k + 5] = 'E';
        else                           s[k + 5] = 'B';
    }
    int flag = 0;

    /*  ( ) */
    int run = 1;
    for (int i = 4; i >= 0 && s[i] == 'M'; i--) run++;
    for (int i = 6; i <= 10 && s[i] == 'M'; i++) run++;
    if (run == 5) flag |= 1;
    if (run >= 6) flag |= 2;

    /* four:  , 5 ( ) M==4,E==1 */
    for (int st = 1; st <= 5; st++) {
        int m = 0, e = 0, bad = 0;
        for (int i = st; i < st + 5; i++) {
            if (s[i] == 'M') m++;
            else if (s[i] == 'E') e++;
            else { bad = 1; break; }
        }
        if (!bad && m == 4 && e == 1) { flag |= 4; break; }
    }
    /* open four:  , "EMMMME" */
    for (int st = 0; st <= 5; st++) {
        if (st + 6 > 11) break;
        if (st <= 5 && st + 6 > 5 &&
            s[st]=='E'&&s[st+1]=='M'&&s[st+2]=='M'&&s[st+3]=='M'&&s[st+4]=='M'&&s[st+5]=='E')
            { flag |= 8; flag |= 4; break; }
    }
    /* open three:   "EMMME" / "EMMEME" / "EMEMME" ( 4 ) */
    if (!(flag & 4)) {
        const char* pats[3] = { "EMMME", "EMMEME", "EMEMME" };
        int plen[3] = { 5, 6, 6 };
        for (int p = 0; p < 3; p++) {
            int L = plen[p];
            for (int st = 0; st + L <= 11; st++) {
                if (st > 5 || st + L <= 5) continue; /*   */
                int ok = 1;
                for (int i = 0; i < L; i++) if (s[st + i] != pats[p][i]) { ok = 0; break; }
                if (ok) { flag |= 16; break; }
            }
            if (flag & 16) break;
        }
    }
    return flag;
}

/* (r,c) color      (b      ) */
static ECFeat ec_features(int b[EC_SZ][EC_SZ], int r, int c, int color) {
    ECFeat f; f.win=f.overline=f.fours=f.openfours=f.threes=0;
    b[r][c] = color;
    for (int d = 0; d < 4; d++) {
        int fl = ec_dir(b, r, c, color, d);
        if (fl & 1)  f.win = 1;
        if (fl & 2)  f.overline = 1;
        if (fl & 4)  f.fours++;
        if (fl & 8)  f.openfours++;
        if (fl & 16) f.threes++;
    }
    b[r][c] = EC_EMPTY;
    return f;
}

/*    (3-3, 4-4, ). win  (5  ) */
static int ec_black_forbidden(const ECFeat& f) {
    if (f.win) return 0;
    if (f.overline) return 1;
    if (f.fours >= 2) return 1;
    if (f.threes >= 2) return 1;
    return 0;
}

/*
 *   " "  (color ). /  .
 * me_is_black: color   .
 */
static int ec_move_score(int b[EC_SZ][EC_SZ], int r, int c, int color) {
    ECFeat f = ec_features(b, r, c, color);
    int black = (color == EC_BLACK);
    if (f.win) return 10000000;
    if (black && ec_black_forbidden(f)) return -1; /*    */
    /*    */
    if (f.openfours >= 1) return 1000000;
    if (!black && f.fours >= 2) return 900000;             /*  4-4 */
    if (f.fours >= 1 && f.threes >= 1) return 800000;      /* 4-3 ( ) */
    if (!black && f.threes >= 2) return 700000;            /*  3-3 */
    if (f.fours >= 1) return 50000;                         /*  4 */
    if (f.threes >= 1) return 8000;                         /*  3 */
    /* :    */
    return 100 + f.fours*40 + f.threes*20;
}

/*   : -5  (leaf )  */
/* run-based line score for one color: exact run length + open ends */
static long ec_color_score(const int b[EC_SZ][EC_SZ], int color) {
    long s = 0;
    for (int d = 0; d < 4; d++) {
        int dr = EC_DR[d], dc = EC_DC[d];
        for (int r = 0; r < EC_SZ; r++) for (int c = 0; c < EC_SZ; c++) {
            if (b[r][c] != color) continue;
            int pr = r - dr, pc = c - dc;
            if (ec_on(pr, pc) && b[pr][pc] == color) continue; /* not run start */
            int L = 1, er = r + dr, ec = c + dc;
            while (ec_on(er, ec) && b[er][ec] == color) { L++; er += dr; ec += dc; }
            int o1 = (ec_on(pr, pc) && b[pr][pc] == EC_EMPTY) ? 1 : 0;
            int o2 = (ec_on(er, ec) && b[er][ec] == EC_EMPTY) ? 1 : 0;
            int open = o1 + o2;
            long v;
            if (L >= 5)       v = 1000000;
            else if (L == 4)  v = (open == 2) ? 100000 : (open == 1 ? 12000 : 0);
            else if (L == 3)  v = (open == 2) ? 6000   : (open == 1 ? 600 : 0);
            else if (L == 2)  v = (open == 2) ? 250    : (open == 1 ? 30 : 0);
            else              v = (open == 2) ? 6 : 0;
            s += v;
        }
    }
    return s;
}

static int ec_eval(const int b[EC_SZ][EC_SZ], int me) {
    int opp = (me == EC_BLACK) ? EC_WHITE : EC_BLACK;
    long s = ec_color_score(b, me) - ec_color_score(b, opp) * 11 / 10;
    if (s > 1000000) s = 1000000;
    if (s < -1000000) s = -1000000;
    return (int)s;
}

/*    :   2  (0~17)  */
static int ec_gen(const int b[EC_SZ][EC_SZ], int mv[][2], int cap) {
    int n = 0;
    for (int r = 0; r <= EC_PLAYMAX; r++) for (int c = 0; c <= EC_PLAYMAX; c++) {
        if (b[r][c] != EC_EMPTY) continue;
        int adj = 0;
        for (int dr = -2; dr <= 2 && !adj; dr++)
            for (int dc = -2; dc <= 2 && !adj; dc++) {
                int rr=r+dr, cc=c+dc;
                if (ec_on(rr,cc) && b[rr][cc] != EC_EMPTY) adj = 1;
            }
        if (adj && n < cap) { mv[n][0]=r; mv[n][1]=c; n++; }
    }
    return n;
}

/*   */
static clock_t ec_deadline;
static int     ec_timeup;
static int ec_time_over() {
    if (clock() >= ec_deadline) ec_timeup = 1;
    return ec_timeup;
}

#define EC_WIN  9000000
#define EC_CAND 16

/* negamax + alpha-beta. me . : me   */
static int ec_search(int b[EC_SZ][EC_SZ], int me, int depth, int alpha, int beta) {
    int opp = (me == EC_BLACK) ? EC_WHITE : EC_BLACK;

    /*    */
    int mv[400][2];
    int n = ec_gen(b, mv, 400);
    if (n == 0) return 0;

    /* 1)   5? */
    for (int i = 0; i < n; i++) {
        ECFeat f = ec_features(b, mv[i][0], mv[i][1], me);
        if (f.win) return EC_WIN - (10 - depth);
    }
    if (depth <= 0) return ec_eval(b, me);
    if (ec_time_over()) return ec_eval(b, me);

    /*   + ( EC_CAND) */
    int sc[400];
    for (int i = 0; i < n; i++) {
        int a = ec_move_score(b, mv[i][0], mv[i][1], me);
        int dfn = ec_move_score(b, mv[i][0], mv[i][1], opp);
        sc[i] = (a > dfn ? a : dfn) + (a + dfn) / 8;
    }
    /*    */
    int lim = n < EC_CAND ? n : EC_CAND;
    for (int i = 0; i < lim; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++) if (sc[j] > sc[best]) best = j;
        int t0=mv[best][0],t1=mv[best][1],ts=sc[best];
        mv[best][0]=mv[i][0]; mv[best][1]=mv[i][1]; sc[best]=sc[i];
        mv[i][0]=t0; mv[i][1]=t1; sc[i]=ts;
    }

    int bestv = -2000000000;
    for (int i = 0; i < lim; i++) {
        int r = mv[i][0], c = mv[i][1];
        /*    */
        if (me == EC_BLACK) {
            ECFeat f = ec_features(b, r, c, me);
            if (ec_black_forbidden(f)) continue;
        }
        b[r][c] = me;
        int v = -ec_search(b, opp, depth - 1, -beta, -alpha);
        b[r][c] = EC_EMPTY;
        if (ec_timeup) return bestv > -2000000000 ? bestv : ec_eval(b, me);
        if (v > bestv) bestv = v;
        if (v > alpha) alpha = v;
        if (alpha >= beta) break;
    }
    if (bestv == -2000000000) return ec_eval(b, me); /*   ( ) */
    return bestv;
}

/*
 *  . b  ().  (*bx=col? :  / )
 *    (out_r, out_c).
 *  time_ms:   ( 380ms )
 */
static void ec_best(int b[EC_SZ][EC_SZ], int me, int* out_r, int* out_c, int time_ms) {
    int opp = (me == EC_BLACK) ? EC_WHITE : EC_BLACK;

    /*   /     */
    int any = 0;
    for (int r = 0; r < EC_SZ && !any; r++) for (int c = 0; c < EC_SZ; c++) if (b[r][c]) { any=1; break; }
    if (!any) { *out_r = 9; *out_c = 9; return; }

    int mv[400][2];
    int n = ec_gen(b, mv, 400);
    if (n == 0) { /* fallback:   ( 0~17 ) */
        for (int r=0;r<=EC_PLAYMAX;r++)for(int c=0;c<=EC_PLAYMAX;c++) if(b[r][c]==EC_EMPTY){*out_r=r;*out_c=c;return;}
        for (int r=0;r<EC_SZ;r++)for(int c=0;c<EC_SZ;c++) if(b[r][c]==EC_EMPTY){*out_r=r;*out_c=c;return;}
        *out_r=9;*out_c=9; return;
    }
    int black = (me == EC_BLACK);

    /* 1)   (5    ) */
    for (int i = 0; i < n; i++) {
        ECFeat f = ec_features(b, mv[i][0], mv[i][1], me);
        if (f.win) { *out_r=mv[i][0]; *out_c=mv[i][1]; return; }
    }
    /* 2)           */
    {
        int blkR=-1, blkC=-1, anyR=-1, anyC=-1;
        for (int i = 0; i < n; i++) {
            ECFeat fo = ec_features(b, mv[i][0], mv[i][1], opp);
            if (!fo.win) continue;
            anyR=mv[i][0]; anyC=mv[i][1];
            if (black) {
                ECFeat fm = ec_features(b, mv[i][0], mv[i][1], me);
                if (ec_black_forbidden(fm)) continue;   /*   () */
            }
            blkR=mv[i][0]; blkC=mv[i][1]; break;
        }
        if (blkR>=0) { *out_r=blkR; *out_c=blkC; return; }
        if (anyR>=0) { /*       ,     */
            *out_r=anyR; *out_c=anyC; /*      */
        }
    }

    /* 3)  alpha-beta */
    ec_timeup = 0;
    ec_deadline = clock() + (clock_t)((double)time_ms / 1000.0 * CLOCKS_PER_SEC);

    int br = mv[0][0], bc = mv[0][1];
    /*     */
    int sc[400];
    for (int i = 0; i < n; i++) {
        int a = ec_move_score(b, mv[i][0], mv[i][1], me);
        int dfn = ec_move_score(b, mv[i][0], mv[i][1], opp);
        sc[i] = (a > dfn ? a : dfn) + (a + dfn) / 8;
    }
    int lim = n < EC_CAND ? n : EC_CAND;
    for (int i = 0; i < lim; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++) if (sc[j] > sc[best]) best = j;
        int t0=mv[best][0],t1=mv[best][1],ts=sc[best];
        mv[best][0]=mv[i][0]; mv[best][1]=mv[i][1]; sc[best]=sc[i];
        mv[i][0]=t0; mv[i][1]=t1; sc[i]=ts;
    }

    for (int depth = 2; depth <= 10; depth += 2) {
        int alpha = -2000000000, beta = 2000000000;
        int curBestR = br, curBestC = bc, curBestV = -2000000000;
        int done = 1;
        for (int i = 0; i < lim; i++) {
            int r = mv[i][0], c = mv[i][1];
            if (me == EC_BLACK) {
                ECFeat f = ec_features(b, r, c, me);
                if (ec_black_forbidden(f)) continue;
            }
            b[r][c] = me;
            int v = -ec_search(b, opp, depth - 1, -beta, -alpha);
            b[r][c] = EC_EMPTY;
            if (ec_timeup) { done = 0; break; }
            if (v > curBestV) { curBestV = v; curBestR = r; curBestC = c; }
            if (v > alpha) alpha = v;
        }
        if (done && curBestV > -2000000000) { br = curBestR; bc = curBestC; }
        if (ec_timeup) break;
        if (curBestV >= EC_WIN - 100) break; /*    */
    }

    /* :  (3-3/4-4/)   .
          ,     . */
    if (black) {
        ECFeat fb = ec_features(b, br, bc, me);
        if (!fb.win && ec_black_forbidden(fb)) {
            int bestS = -2000000000, sr = -1, sc = -1;
            for (int i = 0; i < n; i++) {
                ECFeat f = ec_features(b, mv[i][0], mv[i][1], me);
                if (!f.win && ec_black_forbidden(f)) continue;
                int s = ec_move_score(b, mv[i][0], mv[i][1], me);
                int d = ec_move_score(b, mv[i][0], mv[i][1], opp);
                int v = (s > d ? s : d);
                if (v > bestS) { bestS = v; sr = mv[i][0]; sc = mv[i][1]; }
            }
            if (sr >= 0) { br = sr; bc = sc; }
        }
    }
    *out_r = br; *out_c = bc;
}




/*  500ms   400ms  */
#define OMOCK_BUDGET_MS 400

static int  g_board_2023184027[EC_SZ][EC_SZ];
static int  g_init_2023184027 = 0;

static void g_ensure_2023184027(void) {
    if (!g_init_2023184027) {
        memset(g_board_2023184027, 0, sizeof(g_board_2023184027));
        g_init_2023184027 = 1;
    }
}

static int g_valid_2023184027(int x, int y) {
    return x >= 0 && x < EC_SZ && y >= 0 && y < EC_SZ;
}

void BlackAttack_2023184027(int* x, int* y) {
    g_ensure_2023184027();
    int r, c;
    ec_best(g_board_2023184027, EC_BLACK, &r, &c, OMOCK_BUDGET_MS);
    *x = c; *y = r;                         /* x=, y= */
    g_board_2023184027[r][c] = EC_BLACK;
}

void BlackDefence_2023184027(int x, int y) {
    g_ensure_2023184027();
    if (g_valid_2023184027(x, y))
        g_board_2023184027[y][x] = EC_WHITE; /* ()   */
}

void WhiteAttack_2023184027(int* x, int* y) {
    g_ensure_2023184027();
    int r, c;
    ec_best(g_board_2023184027, EC_WHITE, &r, &c, OMOCK_BUDGET_MS);
    *x = c; *y = r;
    g_board_2023184027[r][c] = EC_WHITE;
}

void WhiteDefence_2023184027(int x, int y) {
    g_ensure_2023184027();
    if (g_valid_2023184027(x, y))
        g_board_2023184027[y][x] = EC_BLACK; /* ()   */
}

#endif /* OMOCK_2023184027_H */
