/*
 * selftest.cpp  —  Omock_2023184027.h 자동 대전(self-play) 테스트
 *
 *   내 AI가 흑/백을 모두 두며 한 판을 끝까지 진행한다.
 *   - 컴파일 문법 검증
 *   - 5목 완성/무한루프/잘못된 좌표 여부 확인
 *   - 보드를 단계별로 출력
 *
 *   컴파일: cl /EHsc /nologo selftest.cpp
 */

#include <cstdio>
#include "Omock_2023184027.h"

#define N 19

static int g[N][N];   /* 마스터 보드(판정용) — AI 내부 _bd와 별개 */

/* (y,x)에 color를 둔 뒤, 그 점이 정확히 5개(또는 그 이상)를 만들면 1 */
static int win_at(int y, int x, int color) {
    int dr[4] = {0,1,1,1}, dc[4] = {1,0,1,-1};
    for (int d = 0; d < 4; d++) {
        int cnt = 1;
        int r = y+dr[d], c = x+dc[d];
        while (r>=0&&r<N&&c>=0&&c<N && g[r][c]==color){cnt++; r+=dr[d]; c+=dc[d];}
        r = y-dr[d]; c = x-dc[d];
        while (r>=0&&r<N&&c>=0&&c<N && g[r][c]==color){cnt++; r-=dr[d]; c-=dc[d];}
        if (cnt >= 5) return 1;
    }
    return 0;
}

static void show(void) {
    printf("    ");
    for (int j = 0; j < N; j++) printf("%2d", j%10);
    printf("\n");
    for (int i = 0; i < N; i++) {
        printf("%2d  ", i);
        for (int j = 0; j < N; j++) {
            if      (g[i][j]==1) printf(" X");   /* 흑 */
            else if (g[i][j]==2) printf(" O");   /* 백 */
            else                 printf(" .");
        }
        printf("\n");
    }
}

int main(void) {
    int x, y;
    int turn = 1;          /* 1=흑, 2=백 */
    int moves = 0;

    printf("=== Omock_2023184027 self-play 테스트 ===\n\n");

    for (moves = 0; moves < N*N; moves++) {
        if (turn == 1) BlackAttack_2023184027(&x, &y);
        else           WhiteAttack_2023184027(&x, &y);

        /* 좌표 범위 검사 */
        if (x < 0 || x >= N || y < 0 || y >= N) {
            printf("[오류] %s가 범위 밖 좌표 반환: (x=%d, y=%d)\n",
                   turn==1?"흑":"백", x, y);
            return 1;
        }
        /* 중복 착수 검사 */
        if (g[y][x] != 0) {
            printf("[오류] %s가 이미 둔 자리에 착수: (x=%d, y=%d)\n",
                   turn==1?"흑":"백", x, y);
            show();
            return 1;
        }

        g[y][x] = turn;
        printf("%3d수: %s -> (x=%2d, y=%2d)\n", moves+1, turn==1?"흑(X)":"백(O)", x, y);

        if (win_at(y, x, turn)) {
            printf("\n>>> %s 5목 완성! 승리 <<<\n\n", turn==1?"흑(X)":"백(O)");
            show();
            return 0;
        }
        turn = (turn==1) ? 2 : 1;
    }

    printf("\n무승부(보드 가득 참)\n");
    show();
    return 0;
}
