/*
 * vstest.cpp  —  내 AI(흑) vs 내장 그리디봇(백) 실전 형식 테스트
 *
 *   심판 프로그램과 동일한 호출 흐름을 재현:
 *     흑 차례: BlackAttack_me(&x,&y)  →  봇의 Defence는 생략(봇은 master board를 봄)
 *     백 차례: 봇이 수를 고름  →  BlackDefence_me(x,y)로 내 AI에 통보
 *
 *   내장 봇(백): "상대 4 막기 > 내 4 잇기 > 상대 3 막기 > 내 잇기 > 랜덤" 수준의
 *                간단한 그리디. 내 AI가 이걸 이기면 합격.
 *
 *   컴파일: cl /EHsc /nologo /utf-8 vstest.cpp
 */

#include <cstdio>
#include <cstdlib>
#include "Omock_2023184027.h"

#define N 19
#define EMP 0
#define BLK 1
#define WHT 2

static int g[N][N];   /* 마스터 보드 */

static int inb(int r,int c){ return r>=0&&r<N&&c>=0&&c<N; }

static int win_at(int y,int x,int col){
    int dr[4]={0,1,1,1}, dc[4]={1,0,1,-1};
    for(int d=0;d<4;d++){
        int cnt=1, r=y+dr[d], c=x+dc[d];
        while(inb(r,c)&&g[r][c]==col){cnt++; r+=dr[d]; c+=dc[d];}
        r=y-dr[d]; c=x-dc[d];
        while(inb(r,c)&&g[r][c]==col){cnt++; r-=dr[d]; c-=dc[d];}
        if(cnt>=5) return 1;
    }
    return 0;
}

/* 봇(백) 평가: (r,c)에 col을 둘 때 만들어지는 최장 연속 라인 길이 + 열림 가중 */
static int bot_score(int r,int c,int col){
    g[r][c]=col;
    int best=0;
    int dr[4]={0,1,1,1}, dc[4]={1,0,1,-1};
    for(int d=0;d<4;d++){
        int cnt=1, open=0;
        int rr=r+dr[d], cc=c+dc[d];
        while(inb(rr,cc)&&g[rr][cc]==col){cnt++; rr+=dr[d]; cc+=dc[d];}
        if(inb(rr,cc)&&g[rr][cc]==EMP) open++;
        rr=r-dr[d]; cc=c-dc[d];
        while(inb(rr,cc)&&g[rr][cc]==col){cnt++; rr-=dr[d]; cc-=dc[d];}
        if(inb(rr,cc)&&g[rr][cc]==EMP) open++;
        int s=cnt*10+open+ (cnt>=5?100000:0);
        if(s>best) best=s;
    }
    g[r][c]=EMP;
    return best;
}

/* 봇(백) 한 수: 공격(백) 점수와 수비(흑 막기) 점수 중 큰 쪽 */
static void bot_move(int *ox,int *oy){
    int bv=-1,br=-1,bc=-1;
    for(int r=0;r<N;r++) for(int c=0;c<N;c++){
        if(g[r][c]!=EMP) continue;
        int near=0;
        for(int dr=-1;dr<=1&&!near;dr++)for(int dc=-1;dc<=1&&!near;dc++){
            int nr=r+dr,nc=c+dc; if(inb(nr,nc)&&g[nr][nc]!=EMP) near=1; }
        if(!near) continue;
        int atk=bot_score(r,c,WHT);   /* 내가(백) 공격 */
        int def=bot_score(r,c,BLK);   /* 상대(흑) 막기 */
        int v=(atk>def?atk:def);
        if(v>bv){bv=v;br=r;bc=c;}
    }
    if(br<0){ /* 첫 수 등 */ br=9; bc=9; if(g[9][9]!=EMP){ for(int r=0;r<N;r++)for(int c=0;c<N;c++) if(g[r][c]==EMP){br=r;bc=c;goto done;} } }
done:
    *oy=br; *ox=bc;
}

int main(void){
    int x,y;
    int turn=BLK;
    printf("=== 내 AI(흑 X) vs 내장 그리디봇(백 O) ===\n\n");

    for(int m=0;m<N*N;m++){
        if(turn==BLK){
            BlackAttack_2023184027(&x,&y);     /* 내 AI 착수 */
            if(!inb(x,y)||g[y][x]!=EMP){ printf("[오류] 내 AI 잘못된 수 (x=%d,y=%d)\n",x,y); return 1; }
            g[y][x]=BLK;
            printf("%3d수 흑(나): (x=%2d,y=%2d)\n",m+1,x,y);
            if(win_at(y,x,BLK)){ printf("\n>>> 내 AI(흑) 승리! <<<\n"); return 0; }
            turn=WHT;
        } else {
            bot_move(&x,&y);                   /* 봇 착수 */
            if(!inb(x,y)||g[y][x]!=EMP){ printf("[봇오류] 무승부 처리\n"); break; }
            g[y][x]=WHT;
            BlackDefence_2023184027(x,y);       /* 내 AI에 상대 수 통보 */
            printf("%3d수 백(봇): (x=%2d,y=%2d)\n",m+1,x,y);
            if(win_at(y,x,WHT)){ printf("\n>>> 봇(백) 승리... 내 AI 패배 <<<\n"); return 0; }
            turn=BLK;
        }
    }
    printf("\n무승부\n");
    return 0;
}
