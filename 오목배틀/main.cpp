// Omock battle - my AI (Black) vs example AI (White)
// To swap opponent: change the included header and the White* function names.
#include <iostream>
#include "오목 심판/Judgment.h"
#include "Omock_2023184027.h"          // my AI
#include "심판코드사용예/2020182001.h"  // opponent (example)
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
        BlackAttack_2023184027,   // Black = 내 AI
        BlackDefence_2023184027,
        WhiteAttack_2023184027,   // White = 내 AI
        WhiteDefence_2023184027
    );

    // ── 실제 상대와 둘 때는 위를 주석 처리하고 아래처럼 (상대 헤더 #include 추가) ──
    // judgment.SetYourFunc(
    //     BlackAttack_2023184027, BlackDefence_2023184027,   // 흑 = 내 AI
    //     WhiteAttack_상대학번,   WhiteDefence_상대학번        // 백 = 상대
    // );

    judgment.GamePlay();
    return 0;
}
