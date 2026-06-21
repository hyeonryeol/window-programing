// Omock battle - my AI (Black) vs example AI (White)
// To swap opponent: change the included header and the White* function names.
#include <iostream>
#include "오목 심판/Judgment.h"
#include "2023184027.h"          // my AI
#include "2023184010.h"  // opponent (example)
using namespace std;

int main(void)
{
    // 모든 소스가 UTF-8(BOM) 이라 문자열 리터럴이 UTF-8로 컴파일된다.
    // 콘솔 출력/입력 코드페이지를 65001(UTF-8)로 맞춰 한글/바둑판 문자가 깨지지 않게 한다.
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    Judgment judgment;

    // ── self-play: 내 AI끼리 두기 (끝까지 가는 진짜 게임 관전용) ──
    judgment.SetYourFunc(
        BlackAttack_2023184010, BlackDefence_2023184010,   // 흑 = 내 AI
        WhiteAttack_2023184027, WhiteDefence_2023184027    // 백 = 친구
    );
    judgment.GamePlay();
}


