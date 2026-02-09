#include <iostream>
#include <Windows.h>
#include "Engine/Engine.h"
#include "Level/BattleLevel.h"
#include "Unit/Unit.h"

using namespace Wanted;

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// 콘솔을 UTF-8로 설정
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	
	// 가상 터미널 시퀀스 활성화 (UTF-8 박스 문자 지원)
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);

	// 엔진 초기화
	Engine engine;

	// 전투 레벨 생성
	BattleLevel* battleLevel = new BattleLevel();

	// 맵 로딩 (Assets 폴더에 BattleMap.txt 필요)
	if (!battleLevel->LoadMap("Assets/BattleMap.txt"))
	{
		std::cout << "Failed to load map: Assets/BattleMap.txt" << std::endl;
		std::cout << "Using default map instead." << std::endl;
	}

	// 플레이어 유닛 추가 (최대 10개, 숫자 키 0~9)
	// 로드 (주인공)
	Unit* lord = new Unit(UnitClass::Lord);
	battleLevel->AddUnit(lord, Vector2(1, 1), Faction::Player, 0);

	// 기병
	Unit* cavalier = new Unit(UnitClass::Cavalier);
	battleLevel->AddUnit(cavalier, Vector2(3, 1), Faction::Player, 1);

	// 궁수
	Unit* archer = new Unit(UnitClass::Archer);
	battleLevel->AddUnit(archer, Vector2(5, 1), Faction::Player, 2);

	// 적 유닛 추가
	Unit* enemySoldier1 = new Unit(UnitClass::Soldier);
	battleLevel->AddUnit(enemySoldier1, Vector2(10, 10), Faction::Enemy);

	Unit* enemySoldier2 = new Unit(UnitClass::Soldier);
	battleLevel->AddUnit(enemySoldier2, Vector2(11, 10), Faction::Enemy);

	Unit* enemyArcher = new Unit(UnitClass::Archer);
	battleLevel->AddUnit(enemyArcher, Vector2(12, 10), Faction::Enemy);

	// 레벨 설정 및 실행
	engine.SetNewLevel(battleLevel);
	engine.Run();

	return 0;
}