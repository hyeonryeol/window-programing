/*
 * arena.cpp  —  엔진 자동 대국 테스트 (심판 규칙 일치)
 *
 *   규칙: 정확히5=승, 6목이상=패(양쪽), 흑은 3-3/4-4도 패.
 *   매 게임 랜덤 오프닝으로 다양화하여 통계를 낸다.
 *
 *   컴파일: cl /EHsc /O2 /nologo arena.cpp
 */
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "Omock_2023184027.h"   /* 엔진 단일 헤더 (self-contained) */

typedef int Board[EC_SZ][EC_SZ];

static unsigned long rng = 88172645ULL;
static unsigned long xr(){ rng ^= rng<<13; rng ^= rng>>7; rng ^= rng<<17; return rng; }

/* 아무 빈칸이나 (폴백용) */
static void any_empty(Board b, int* or_, int* oc){
    for(int r=0;r<EC_SZ;r++)for(int c=0;c<EC_SZ;c++) if(b[r][c]==EC_EMPTY){*or_=r;*oc=c;return;}
    *or_=9;*oc=9;
}

/* ── 심판 판정: 방금 (r,c)에 color가 둔 결과 ──
   반환: 0=계속, 1=color 승, 2=color 패(반칙/장목) */
static int referee(Board b, int r, int c, int color) {
    /* ec_features는 빈칸 가정으로 두었다 지운다. 이미 놓인 돌이므로 잠시 비웠다 복원 */
    b[r][c] = EC_EMPTY;
    ECFeat f = ec_features(b, r, c, color);
    b[r][c] = color;
    if (f.win) return 1;
    if (f.overline) return 2;
    if (color == EC_BLACK) {
        if (f.fours >= 2) return 2;
        if (f.threes >= 2) return 2;
    }
    return 0;
}

/* ── 상대 봇들 ── */
/* 랜덤: 이웃한 빈칸 중 무작위 */
static void bot_random(Board b, int me, int* or_, int* oc) {
    int cand[400][2], n=0;
    for (int r=0;r<=EC_PLAYMAX;r++)for(int c=0;c<=EC_PLAYMAX;c++){
        if(b[r][c]!=EC_EMPTY)continue;
        int near=0;
        for(int dr=-1;dr<=1&&!near;dr++)for(int dc=-1;dc<=1&&!near;dc++){
            int rr=r+dr,cc=c+dc; if(ec_on(rr,cc)&&b[rr][cc]!=EC_EMPTY)near=1;}
        if(near&&n<400){cand[n][0]=r;cand[n][1]=c;n++;}
    }
    if(n==0){any_empty(b,or_,oc);return;}
    int i=xr()%n; *or_=cand[i][0]; *oc=cand[i][1];
}

/* 그리디: 1-ply, max(공격,수비). 흑 금수는 회피. */
static void bot_greedy(Board b, int me, int* or_, int* oc) {
    int opp = (me==EC_BLACK)?EC_WHITE:EC_BLACK;
    int mv[400][2]; int n=ec_gen(b,mv,400);
    if(n==0){any_empty(b,or_,oc);return;}
    /* 즉시 승 */
    for(int i=0;i<n;i++){ECFeat f=ec_features(b,mv[i][0],mv[i][1],me); if(f.win){*or_=mv[i][0];*oc=mv[i][1];return;}}
    /* 상대 즉시 승 차단 */
    for(int i=0;i<n;i++){ECFeat f=ec_features(b,mv[i][0],mv[i][1],opp); if(f.win){*or_=mv[i][0];*oc=mv[i][1];return;}}
    int bv=-1,br=mv[0][0],bc=mv[0][1];
    for(int i=0;i<n;i++){
        int r=mv[i][0],c=mv[i][1];
        if(me==EC_BLACK){ECFeat f=ec_features(b,r,c,me); if(ec_black_forbidden(f))continue;}
        int a=ec_move_score(b,r,c,me);
        int d=ec_move_score(b,r,c,opp);
        int v=(a>d?a:d);
        if(v>bv){bv=v;br=r;bc=c;}
    }
    *or_=br;*oc=bc;
}

/* 내 엔진 */
static int g_budget = 120;
static double g_maxms = 0, g_summs = 0; static long g_moves = 0;
static void bot_engine(Board b, int me, int* or_, int* oc) {
    clock_t t0 = clock();
    ec_best(b, me, or_, oc, g_budget);
    double ms = (double)(clock()-t0)*1000.0/CLOCKS_PER_SEC;
    if(ms>g_maxms) g_maxms=ms; g_summs+=ms; g_moves++;
}

/* 엔진 라이트(작은 예산) — 강한 스파링 상대 */
static int g_budget_lite = 8;
static void bot_engine_lite(Board b, int me, int* or_, int* oc) {
    ec_best(b, me, or_, oc, g_budget_lite);
}

typedef void (*BotFn)(Board,int,int*,int*);

/* 한 판 진행. black/white = 봇함수. randopen=랜덤 첫수 개수.
   반환: 1=흑승,2=백승,0=무. *foulLoser 설정(반칙으로 진 쪽) */
static int play(BotFn black, BotFn white, int* result_kind) {
    Board b; memset(b,0,sizeof(b));
    int turn = EC_BLACK;
    /* 랜덤 오프닝: 흑 첫수를 중앙 부근 랜덤 */
    {
        int rr = 7 + (xr()%5), cc = 7 + (xr()%5);
        b[rr][cc]=EC_BLACK; turn=EC_WHITE;
    }
    for (int ply=0; ply<EC_SZ*EC_SZ; ply++) {
        /* 보드 완전 포화 → 무승부 (실제 심판도 둘 곳 없음) */
        int hasEmpty=0;
        for(int rr=0;rr<EC_SZ&&!hasEmpty;rr++)for(int cc=0;cc<EC_SZ;cc++) if(b[rr][cc]==EC_EMPTY){hasEmpty=1;break;}
        if(!hasEmpty){ *result_kind=0; return 0; }
        int r,c;
        if(turn==EC_BLACK) black(b,EC_BLACK,&r,&c); else white(b,EC_WHITE,&r,&c);
        /* 유효성 (실제 심판과 동일하게 0~18 허용) */
        if(r<0||r>=EC_SZ||c<0||c>=EC_SZ||b[r][c]!=EC_EMPTY){
            *result_kind = (turn==EC_BLACK)?-1:-2; /* 불법수 */
            return (turn==EC_BLACK)?2:1; /* 상대 승 */
        }
        b[r][c]=turn;
        int rf = referee(b,r,c,turn);
        if(rf==1){ *result_kind=turn; return turn; }
        if(rf==2){ *result_kind=(turn==EC_BLACK)?10:20; /*반칙패*/ return (turn==EC_BLACK)?2:1; }
        turn = (turn==EC_BLACK)?EC_WHITE:EC_BLACK;
    }
    *result_kind=0; return 0; /* 무승부 */
}

static void run(const char* name, BotFn black, BotFn white, int games) {
    int bw=0,ww=0,dr=0, illegal=0, foul=0;
    for(int g=0;g<games;g++){
        int kind; int res=play(black,white,&kind);
        if(res==1)bw++; else if(res==2)ww++; else dr++;
        if(kind==-1||kind==-2)illegal++;
        if(kind==10||kind==20)foul++;
    }
    printf("%-28s  흑승 %3d  백승 %3d  무 %3d   (불법 %d, 반칙 %d)\n",
           name, bw, ww, dr, illegal, foul);
}

int main(int argc, char** argv){
    int games = (argc>1)?atoi(argv[1]):60;
    if(argc>2) g_budget = atoi(argv[2]);
    printf("=== Arena (게임수=%d, 엔진예산=%dms) ===\n", games, g_budget);
    printf("[엔진이 흑]\n");
    run("엔진(흑) vs 랜덤(백)", bot_engine, bot_random, games);
    run("엔진(흑) vs 그리디(백)", bot_engine, bot_greedy, games);
    printf("[엔진이 백]\n");
    run("랜덤(흑) vs 엔진(백)", bot_random, bot_engine, games);
    run("그리디(흑) vs 엔진(백)", bot_greedy, bot_engine, games);
    printf("[강한 상대: 엔진 vs 엔진라이트(%dms)]\n", g_budget_lite);
    run("엔진(흑) vs 라이트(백)", bot_engine, bot_engine_lite, games);
    run("라이트(흑) vs 엔진(백)", bot_engine_lite, bot_engine, games);
    printf("[참고: 그리디 vs 그리디]\n");
    run("그리디(흑) vs 그리디(백)", bot_greedy, bot_greedy, games);
    printf("\n엔진 1수 시간: 최대 %.1fms, 평균 %.1fms (%ld수)\n",
           g_maxms, g_moves? g_summs/g_moves : 0.0, g_moves);
    return 0;
}
