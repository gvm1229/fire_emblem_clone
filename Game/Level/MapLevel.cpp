#include "MapLevel.h"
#include "Unit/UnitClass.h"
#include "Item/Weapon.h"
#include "Render/Renderer.h"
#include "Engine/Engine.h"
#include "System/TurnManager.h"
#include "System/PlayerPhaseController.h"
#include "System/EnemyPhaseController.h"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <string>

namespace FE
{
	RTTI_DEFINITIONS(MapLevel)

	MapLevel::MapLevel(const char* mapFilePath)
		: Level()
		, grid(nullptr)
		, mapFilePath(mapFilePath)
		, turnManager(nullptr)
		, playerPhaseController(nullptr)
		, enemyPhaseController(nullptr)
		, isGameOver(false)
		, isVictory(false)
		, nextMapName(nullptr)
	{
		grid = new Grid();
		LoadMap(mapFilePath);

		// 턴 관리자 및 컨트롤러 생성
		turnManager = new TurnManager(this);
		playerPhaseController = new PlayerPhaseController(this, turnManager);
		enemyPhaseController = new EnemyPhaseController(this, turnManager);
	}

	MapLevel::~MapLevel()
	{
		if (grid != nullptr)
		{
			delete grid;
			grid = nullptr;
		}

		if (turnManager != nullptr)
		{
			delete turnManager;
			turnManager = nullptr;
		}

		if (playerPhaseController != nullptr)
		{
			delete playerPhaseController;
			playerPhaseController = nullptr;
		}

		if (enemyPhaseController != nullptr)
		{
			delete enemyPhaseController;
			enemyPhaseController = nullptr;
		}
	}

	void MapLevel::BeginPlay()
	{
		Level::BeginPlay();

		// 플레이어 페이즈로 시작
		if (turnManager != nullptr)
		{
			turnManager->StartPlayerPhase();
		}
	}

	void MapLevel::Tick(float deltaTime)
	{
		Level::Tick(deltaTime);

		// 게임 오버가 아니면 턴 관리 업데이트
		if (!isGameOver)
		{
			// 턴 관리자 업데이트
			if (turnManager != nullptr)
			{
				turnManager->Update(deltaTime);
			}

			// 페이즈 컨트롤러 업데이트
			if (playerPhaseController != nullptr)
			{
				playerPhaseController->Update(deltaTime);
			}

			if (enemyPhaseController != nullptr)
			{
				enemyPhaseController->Update(deltaTime);
			}

			// 승리/패배 조건 체크
			if (CheckVictoryCondition())
			{
				isGameOver = true;
				isVictory = true;
				printf("\n=== VICTORY! ===\n");
				printf("Press Q to quit.\n");
			}
			else if (CheckDefeatCondition())
			{
				isGameOver = true;
				isVictory = false;
				printf("\n=== DEFEAT... ===\n");
				printf("Press Q to quit.\n");
			}
		}
	}

	void MapLevel::Draw()
	{
		// 그리드 그리기
		if (grid != nullptr)
		{
			for (int y = 0; y < grid->GetHeight(); ++y)
			{
				for (int x = 0; x < grid->GetWidth(); ++x)
				{
				Tile* tile = grid->GetTile(x, y);
				if (tile != nullptr)
				{
					char tileChar = tile->GetDisplayCharacter();
					// 그리드는 (1, 1)부터 시작 (0, 0)은 UI용
					Renderer::Get().Draw(Vector2(x + 1, y + 1), tileChar, Color::White);
				}
				}
			}
		}

		// 액터(유닛) 그리기
		Level::Draw();
	}

	// 맵 파일 로딩
	void MapLevel::LoadMap(const char* filePath)
	{
		printf("[MapLevel::LoadMap] Loading map: %s\n", filePath);

		std::ifstream file(filePath);
		if (!file.is_open())
		{
			printf("[MapLevel::LoadMap] Failed to open file: %s\n", filePath);
			return;
		}

		std::string line;
		int width = 0;
		int height = 0;
		bool readingTiles = false;
		bool readingUnits = false;
		int currentRow = 0;

		while (std::getline(file, line))
		{
			// 주석 또는 빈 줄 스킵
			if (line.empty() || line[0] == '#')
			{
				continue;
			}

			// WIDTH 파싱
			if (line.find("WIDTH") == 0)
			{
				std::istringstream iss(line);
				std::string keyword;
				iss >> keyword >> width;
				continue;
			}

			// HEIGHT 파싱
			if (line.find("HEIGHT") == 0)
			{
				std::istringstream iss(line);
				std::string keyword;
				iss >> keyword >> height;

				// Grid 초기화
				if (width > 0 && height > 0)
				{
					grid->Initialize(width, height);
					printf("[MapLevel::LoadMap] Grid initialized: %dx%d\n", width, height);
				}
				continue;
			}

			// TILES 섹션 시작
			if (line.find("TILES") == 0)
			{
				readingTiles = true;
				readingUnits = false;
				currentRow = 0;
				continue;
			}

			// UNITS 섹션 시작
			if (line.find("UNITS") == 0)
			{
				readingTiles = false;
				readingUnits = true;
				continue;
			}

			// VICTORY/DEFEAT 섹션 (현재는 스킵)
			if (line.find("VICTORY") == 0 || line.find("DEFEAT") == 0)
			{
				readingTiles = false;
				readingUnits = false;
				continue;
			}

			// 타일 데이터 읽기
			if (readingTiles && currentRow < height)
			{
				for (int x = 0; x < width && x < static_cast<int>(line.length()); ++x)
				{
					char c = line[x];
					TerrainType type = TerrainType::Plain;

					switch (c)
					{
					case '.':
						type = TerrainType::Plain;
						break;
					case 'T':
						type = TerrainType::Forest;
						break;
					case '^':
						type = TerrainType::Mountain;
						break;
					case '#':
						type = TerrainType::Castle;
						break;
					case 'V':
						type = TerrainType::Village;
						break;
					case '~':
						type = TerrainType::Water;
						break;
					}

					grid->SetTile(x, currentRow, type);
				}
				currentRow++;
				continue;
			}

			// 유닛 데이터 읽기
			if (readingUnits)
			{
				std::istringstream iss(line);
				std::string factionStr, classStr, weaponStr;
				int gridX, gridY;

				iss >> factionStr >> gridX >> gridY >> classStr >> weaponStr;

				// Faction 파싱
				UnitFaction faction = UnitFaction::Player;
				if (factionStr == "PLAYER")
					faction = UnitFaction::Player;
				else if (factionStr == "ENEMY")
					faction = UnitFaction::Enemy;
				else if (factionStr == "ALLY")
					faction = UnitFaction::Ally;

				// Class 파싱
				UnitClassType classType = UnitClassType::Fighter;
				if (classStr == "Lord")
					classType = UnitClassType::Lord;
				else if (classStr == "Cavalier")
					classType = UnitClassType::Cavalier;
				else if (classStr == "Knight")
					classType = UnitClassType::Knight;
				else if (classStr == "Archer")
					classType = UnitClassType::Archer;
				else if (classStr == "Mage")
					classType = UnitClassType::Mage;
				else if (classStr == "Fighter")
					classType = UnitClassType::Fighter;
				else if (classStr == "Mercenary")
					classType = UnitClassType::Mercenary;

				// Weapon 파싱
				WeaponType weaponType = WeaponType::Sword;
				if (weaponStr == "Sword")
					weaponType = WeaponType::Sword;
				else if (weaponStr == "Lance")
					weaponType = WeaponType::Lance;
				else if (weaponStr == "Axe")
					weaponType = WeaponType::Axe;
				else if (weaponStr == "Bow")
					weaponType = WeaponType::Bow;
				else if (weaponStr == "Magic")
					weaponType = WeaponType::Magic;

				// 유닛 생성
				SpawnUnit(faction, gridX, gridY, classType, weaponType);
			}
		}

		file.close();
		printf("[MapLevel::LoadMap] Map loaded successfully.\n");
	}

	// 유닛 생성
	Unit* MapLevel::SpawnUnit(UnitFaction faction, int gridX, int gridY, UnitClassType classType, WeaponType weaponType)
	{
		// 유닛 이름 생성
		char unitName[32];
		const char* factionName = (faction == UnitFaction::Player) ? "Player" : (faction == UnitFaction::Enemy) ? "Enemy" : "Ally";
		sprintf_s(unitName, "%s_%s", factionName, GetUnitClassInfo(classType).className);

		Unit* unit = new Unit(classType, faction, gridX, gridY, unitName);

		// 무기 장비
		Weapon* weapon = nullptr;
		switch (weaponType)
		{
		case WeaponType::Sword:
			weapon = CreateIronSword();
			break;
		case WeaponType::Lance:
			weapon = CreateIronLance();
			break;
		case WeaponType::Axe:
			weapon = CreateIronAxe();
			break;
		case WeaponType::Bow:
			weapon = CreateIronBow();
			break;
		case WeaponType::Magic:
			weapon = CreateFire();
			break;
		}
		unit->SetEquippedWeapon(weapon);

		AddNewActor(unit);

		printf("[MapLevel::SpawnUnit] Spawned %s at (%d, %d)\n", unitName, gridX, gridY);

		return unit;
	}

	// 승리 조건: 모든 적 제거
	bool MapLevel::CheckVictoryCondition()
	{
		std::vector<Unit*> enemies = GetEnemyUnits();
		return enemies.empty();
	}

	// 패배 조건: 모든 플레이어 유닛 사망
	bool MapLevel::CheckDefeatCondition()
	{
		std::vector<Unit*> players = GetPlayerUnits();
		return players.empty();
	}

	// 특정 그리드 위치의 유닛 찾기
	Unit* MapLevel::GetUnitAtPosition(int gridX, int gridY) const
	{
		for (Actor* actor : actors)
		{
			Unit* unit = dynamic_cast<Unit*>(actor);
			if (unit != nullptr && unit->GetGridX() == gridX && unit->GetGridY() == gridY)
			{
				return unit;
			}
		}
		return nullptr;
	}

	// 모든 유닛 가져오기
	std::vector<Unit*> MapLevel::GetAllUnits() const
	{
		std::vector<Unit*> units;
		for (Actor* actor : actors)
		{
			Unit* unit = dynamic_cast<Unit*>(actor);
			if (unit != nullptr && unit->IsAlive())
			{
				units.push_back(unit);
			}
		}
		return units;
	}

	// 플레이어 유닛만 가져오기
	std::vector<Unit*> MapLevel::GetPlayerUnits() const
	{
		std::vector<Unit*> units;
		for (Actor* actor : actors)
		{
			Unit* unit = dynamic_cast<Unit*>(actor);
			if (unit != nullptr && unit->IsAlive() && unit->GetFaction() == UnitFaction::Player)
			{
				units.push_back(unit);
			}
		}
		return units;
	}

	// 적 유닛만 가져오기
	std::vector<Unit*> MapLevel::GetEnemyUnits() const
	{
		std::vector<Unit*> units;
		for (Actor* actor : actors)
		{
			Unit* unit = dynamic_cast<Unit*>(actor);
			if (unit != nullptr && unit->IsAlive() && unit->GetFaction() == UnitFaction::Enemy)
			{
				units.push_back(unit);
			}
		}
		return units;
	}
}
