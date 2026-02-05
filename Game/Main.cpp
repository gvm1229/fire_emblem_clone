#include <crtdbg.h>

#include "Engine/Engine.h"
#include "Level/MapLevel.h"

using namespace FE;

int main()
{
	// 메모리 누수 체크 설정
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// 엔진 초기화
	Engine::Get().Initialize();

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
	printf("Starting Tutorial Map...\n");
	printf("\n");

	// 튜토리얼 맵 로드
	MapLevel* tutorialLevel = new MapLevel("Assets/Tutorial.txt");

	// 레벨 설정 및 게임 시작
	Engine::Get().SetNewLevel(tutorialLevel);
	Engine::Get().Run();

	return 0;
}
