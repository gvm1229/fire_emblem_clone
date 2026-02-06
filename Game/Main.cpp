#include <crtdbg.h>

#include "Engine/Engine.h"
#include "Level/MapLevel.h"

using namespace FE;

int main()
{
	// 메모리 누수 체크 설정
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// 안내 메시지 출력 (Engine 생성 전)
	printf("==============================================\n");
	printf("   Fire Emblem Clone - Tutorial\n");
	printf("==============================================\n");
	printf("\n");
	printf("Controls:\n");
	printf("  - Mouse: Click to select unit and move\n");
	printf("  - A: Attack\n");
	printf("  - W: Wait\n");
	printf("  - SPACE: End turn (player phase)\n");
	printf("  - ESC: Cancel\n");
	printf("\n");
	printf("Press the Enter key to start...\n");
	getchar(); // 사용자 입력 대기

	// 엔진 생성 (싱글톤 인스턴스 초기화)
	Engine* engine = new Engine();

	// 튜토리얼 맵 로드 (상대 경로: Game.exe가 Bin\x64\Debug\Game에 있으므로 ../Assets)
	MapLevel* tutorialLevel = new MapLevel("..\\Assets\\Tutorial.txt");

	// 레벨 설정 및 게임 시작
	Engine::Get().SetNewLevel(tutorialLevel);
	Engine::Get().Run();

	// 엔진 정리
	delete engine;

	return 0;
}
