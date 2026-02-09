#include "BattleLevel.h"
#include "Render/Renderer.h"
#include "Core/Input.h"
#include <fstream>
#include <sstream>

namespace FEClone
{
	RTTI_DEFINITIONS(BattleLevel)

	// 생성자
	BattleLevel::BattleLevel()
		: Level()
		, grid(nullptr)
		, selectedUnit(nullptr)
		, isPlayerTurn(true)
		, turnCount(1)
	{
		// 기본 맵 크기로 그리드 생성 (15x15)
		grid = new Grid(15, 15);
	}

	// 소멸자
	BattleLevel::~BattleLevel()
	{
		delete grid;

		// 유닛들은 Level에서 자동으로 삭제됨 (Actor 목록에 포함)
	}

	// BeginPlay: 레벨 시작
	void BattleLevel::BeginPlay()
	{
		Level::BeginPlay();
	}

	// Tick: 매 프레임 업데이트
	void BattleLevel::Tick(float deltaTime)
	{
		Level::Tick(deltaTime);

		// 입력 처리
		HandleInput();
	}

	// Draw: 렌더링
	void BattleLevel::Draw()
	{
		// 그리드 렌더링
		DrawGrid();

		// 이동 가능 범위 하이라이트
		if (selectedUnit != nullptr)
		{
			DrawMovementRange();
		}

		// 액터(유닛) 렌더링
		Level::Draw();

	// 스탯 UI 패널
	DrawStatsPanel();

	// 키보드 툴팁
	DrawKeyboardTooltip();

	// 턴 정보 표시 (클래스 멤버 버퍼 사용)
	const char* phaseStr = isPlayerTurn ? "PLAYER" : "ENEMY";
	sprintf_s(uiBuffers[11], sizeof(uiBuffers[11]), "Turn:%d Phase:%s", turnCount, phaseStr);
	Renderer::Get().Submit(uiBuffers[11], Vector2(0, 0), Color::White, 10);
	}

	// 맵 로딩 (간단한 텍스트 파일 형식)
	bool BattleLevel::LoadMap(const char* mapFilePath)
	{
		std::ifstream file(mapFilePath);
		if (!file.is_open())
		{
			return false;
		}

		// 맵 크기 읽기
		int width, height;
		file >> width >> height;

		// 기존 그리드 삭제 후 새로 생성
		delete grid;
		grid = new Grid(width, height);

		// 지형 데이터 읽기 (한 줄씩)
		std::string line;
		std::getline(file, line); // 첫 줄 개행 건너뛰기

		for (int y = 0; y < height; ++y)
		{
			std::getline(file, line);
			for (int x = 0; x < width && x < static_cast<int>(line.size()); ++x)
			{
				char ch = line[x];
				TerrainType type = TerrainType::Plain;

				// 문자에 따라 지형 타입 결정
				switch (ch)
				{
				case '.': type = TerrainType::Plain; break;
				case 'T': type = TerrainType::Forest; break;
				case '^': type = TerrainType::Mountain; break;
				case '#': type = TerrainType::Castle; break;
				case 'H': type = TerrainType::Village; break;
				case '~': type = TerrainType::Water; break;
				case 'X': type = TerrainType::Wall; break;
				default: type = TerrainType::Plain; break;
				}

				grid->SetTile(x, y, type);
			}
		}

		file.close();
		return true;
	}

	// 유닛 추가
	void BattleLevel::AddUnit(Unit* unit, const Vector2& gridPosition, Faction faction, int unitIndex)
	{
		unit->SetGridPosition(gridPosition);
		unit->SetFaction(faction);
		unit->SetUnitIndex(unitIndex);

		// 진영에 따라 유닛 목록에 추가
		if (faction == Faction::Player)
		{
			playerUnits.push_back(unit);
		}
		else if (faction == Faction::Enemy)
		{
			enemyUnits.push_back(unit);
		}

		// 그리드에 유닛 존재 표시
		Tile* tile = grid->GetTile(gridPosition);
		if (tile != nullptr)
		{
			tile->SetHasUnit(true);
		}

		// 레벨에 액터 추가
		AddNewActor(unit);
	}

	// 숫자 키로 유닛 선택
	void BattleLevel::SelectUnitByIndex(int index)
	{
		// 플레이어 턴이 아니면 무시
		if (!isPlayerTurn)
		{
			return;
		}

		// 해당 인덱스의 유닛 찾기
		for (Unit* unit : playerUnits)
		{
			if (unit->GetUnitIndex() == index)
			{
				// 이미 행동한 유닛이면 무시
				if (unit->HasActedThisTurn())
				{
					return;
				}

				// 이전 선택 해제
				if (selectedUnit != nullptr)
				{
					selectedUnit->SetState(UnitState::Idle);
				}

				// 새 유닛 선택
				selectedUnit = unit;
				selectedUnit->SetState(UnitState::Selected);

				// 이동 가능 범위 계산
				reachableTiles.clear();
				movementCalculator.CalculateReachableTiles(
					selectedUnit->GetGridPosition(),
					selectedUnit->GetStats().movement,
					grid,
					&reachableTiles
				);

				return;
			}
		}
	}

	// 마우스 클릭 처리
	void BattleLevel::OnMouseClick(const Vector2& mousePos)
	{
		// 화면 좌표를 그리드 좌표로 변환 (UI 공간 1칸 고려)
		Vector2 gridPos = mousePos - Vector2(1, 1);

		// 유효한 그리드 위치인지 확인
		if (!grid->IsValidPosition(gridPos))
		{
			return;
		}

		// 유닛이 선택되어 있으면 이동 시도
		if (selectedUnit != nullptr)
		{
			// 클릭한 위치가 이동 가능한 범위인지 확인
			bool isReachable = false;
			for (const Vector2& tile : reachableTiles)
			{
				if (tile == gridPos)
				{
					isReachable = true;
					break;
				}
			}

			if (isReachable)
			{
				// A*로 경로 찾기
				std::deque<Vector2> path;
				std::vector<std::vector<bool>> navMap = grid->GenerateNavigationMap();
				navigationSystem.FindPath(
					selectedUnit->GetGridPosition(),
					gridPos,
					navMap,
					&path
				);

				if (!path.empty())
				{
					// 이전 위치 타일 업데이트
					Tile* oldTile = grid->GetTile(selectedUnit->GetGridPosition());
					if (oldTile != nullptr)
					{
						oldTile->SetHasUnit(false);
					}

					// 유닛 이동 시작 (경로 설정하면 상태가 Moving으로 변경됨)
					selectedUnit->SetPath(path);

					// 새 위치 타일 업데이트 (목적지)
					Tile* newTile = grid->GetTile(gridPos);
					if (newTile != nullptr)
					{
						newTile->SetHasUnit(true);
					}

					// 주의: EndTurn()은 이동이 완료된 후 UpdateMovement()에서 호출됨

					// 선택 해제
					selectedUnit = nullptr;
					reachableTiles.clear();
				}
			}
		}
	}

	// 그리드 렌더링
	void BattleLevel::DrawGrid()
	{
		if (grid == nullptr)
		{
			return;
		}

		for (int y = 0; y < grid->GetHeight(); ++y)
		{
			for (int x = 0; x < grid->GetWidth(); ++x)
			{
				Tile* tile = grid->GetTile(x, y);
				if (tile != nullptr)
				{
					// UI 공간 1칸 확보 (0,0은 턴 정보용)
					Renderer::Get().Submit(tile->GetDisplayString(), Vector2(x + 1, y + 1), tile->GetDisplayColor(), 0);
				}
			}
		}
	}

	// 이동 가능 범위 하이라이트
	void BattleLevel::DrawMovementRange()
	{
		for (const Vector2& tile : reachableTiles)
		{
			// UI 공간 고려
			Vector2 renderPos = tile + Vector2(1, 1);

			// Lord 유닛은 항상 녹색 (선택 시 제외)
			if (selectedUnit->GetUnitClass() == UnitClass::Lord)
			{
				Renderer::Get().Submit("·", renderPos, Color::Yellow, 3);  // Middle dot (UTF-8)
			}
			else // 이외 Player 유닛은 하이라이트 하늘색
			{
				Renderer::Get().Submit("·", renderPos, Color::Cyan, 3);  // Middle dot (UTF-8)
			}
		}
	}

	// 스탯 UI 패널 (오른쪽)
	void BattleLevel::DrawStatsPanel()
	{
		// 맵에서 5칸 떨어진 위치에 패널 배치
		int panelX = (grid != nullptr) ? grid->GetWidth() + 6 : 22;
		int panelY = 0;

		// 패널 타이틀
		Renderer::Get().Submit("==================", Vector2(panelX, panelY), Color::White, 10);
		Renderer::Get().Submit("   UNIT INFO", Vector2(panelX, panelY + 1), Color::White, 10);
		Renderer::Get().Submit("==================", Vector2(panelX, panelY + 2), Color::White, 10);
		panelY += 4;

		if (selectedUnit != nullptr)
		{
			const UnitStats& stats = selectedUnit->GetStats();
			const UnitClass unitClass = selectedUnit->GetUnitClass();
			
			// 유닛 인덱스 표시
			if (unitClass == UnitClass::Lord)
			{
				sprintf_s(uiBuffers[0], sizeof(uiBuffers[0]), "Unit: #%d (Player Unit)", selectedUnit->GetUnitIndex() + 1);
			}
			else
			{
				sprintf_s(uiBuffers[0], sizeof(uiBuffers[0]), "Unit: #%d", selectedUnit->GetUnitIndex() + 1);
			}
			Renderer::Get().Submit(uiBuffers[0], Vector2(panelX, panelY++), Color::White, 10);

			panelY++;

			// HP 표시
			sprintf_s(uiBuffers[1], sizeof(uiBuffers[1]), "HP: %d/%d", stats.currentHP, stats.maxHP);
			Renderer::Get().Submit(uiBuffers[1], Vector2(panelX, panelY++), Color::White, 10);

			// 클래스 표시 (전체 이름)
			sprintf_s(uiBuffers[2], sizeof(uiBuffers[2]), "Class: %s", GetFullUnitClassString(unitClass));
			Renderer::Get().Submit(uiBuffers[2], Vector2(panelX, panelY++), Color::White, 10);

			panelY++;

			// 스탯 표시
			sprintf_s(uiBuffers[3], sizeof(uiBuffers[3]), "STR: %d", stats.strength);
			Renderer::Get().Submit(uiBuffers[3], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[4], sizeof(uiBuffers[4]), "MAG: %d", stats.magic);
			Renderer::Get().Submit(uiBuffers[4], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[5], sizeof(uiBuffers[5]), "SKL: %d", stats.skill);
			Renderer::Get().Submit(uiBuffers[5], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[6], sizeof(uiBuffers[6]), "SPD: %d", stats.speed);
			Renderer::Get().Submit(uiBuffers[6], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[7], sizeof(uiBuffers[7]), "LCK: %d", stats.luck);
			Renderer::Get().Submit(uiBuffers[7], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[8], sizeof(uiBuffers[8]), "DEF: %d", stats.defense);
			Renderer::Get().Submit(uiBuffers[8], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[9], sizeof(uiBuffers[9]), "RES: %d", stats.resistance);
			Renderer::Get().Submit(uiBuffers[9], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[10], sizeof(uiBuffers[10]), "MOV: %d", stats.movement);
			Renderer::Get().Submit(uiBuffers[10], Vector2(panelX, panelY++), Color::White, 10);
		}
		else
		{
			Renderer::Get().Submit("No unit selected", Vector2(panelX, panelY), Color::White, 10);
		}

		// 구분선 (맵과 입력 모니터 사이)
		int separatorY = (grid != nullptr) ? grid->GetHeight() + 2 : 17;
		Renderer::Get().Submit("--------------------", Vector2(0, separatorY), Color::White, 10);
	}

	// 키보드 툴팁 (하단)
	void BattleLevel::DrawKeyboardTooltip()
	{
		// 맵 아래 충분히 떨어진 위치 (구분선 + 3칸)
		int separatorY = (grid != nullptr) ? grid->GetHeight() + 2 : 17;
		int tooltipY = separatorY + 3;  // 구분선 아래 3칸
		int col1X = 0;  // 첫 번째 열
		int col2X = 28; // 두 번째 열

		// 첫 번째 열: CONTROLS
		Renderer::Get().Submit("=== CONTROLS ===", Vector2(col1X, tooltipY), Color::Yellow, 10);
		Renderer::Get().Submit("[1-9,0] Select Unit", Vector2(col1X, tooltipY + 1), Color::White, 10);
		Renderer::Get().Submit("[Click] Move Unit", Vector2(col1X, tooltipY + 2), Color::White, 10);
		Renderer::Get().Submit("[ESC] Deselect", Vector2(col1X, tooltipY + 3), Color::White, 10);
		Renderer::Get().Submit("[ENTER] End Turn", Vector2(col1X, tooltipY + 4), Color::White, 10);

		// 두 번째 열: UNIT COLORS
		Renderer::Get().Submit("=== COLORS ===", Vector2(col2X, tooltipY), Color::Yellow, 10);
		Renderer::Get().Submit("[Yellow] Lord", Vector2(col2X, tooltipY + 1), Color::Yellow, 10);
		Renderer::Get().Submit("[Blue] Player", Vector2(col2X, tooltipY + 2), Color::Blue, 10);
		Renderer::Get().Submit("[Red] Enemy", Vector2(col2X, tooltipY + 3), Color::Red, 10);
		Renderer::Get().Submit("[Cyan] Selected", Vector2(col2X, tooltipY + 4), Color::Cyan, 10);
	}

	// 입력 처리
	void BattleLevel::HandleInput()
	{
		// 숫자 키 (1~9, 0)로 유닛 선택
		// 1번 키 = 첫 번째 유닛 (인덱스 0)
		// 2번 키 = 두 번째 유닛 (인덱스 1)
		// ...
		// 0번 키 = 열 번째 유닛 (인덱스 9)
		for (int i = 1; i <= 9; ++i)
		{
			// VK_1 ~ VK_9 (0x31 ~ 0x39)
			int vkCode = 0x30 + i;
			if (Input::Get().GetKeyDown(vkCode))
			{
				SelectUnitByIndex(i - 1);  // 1번 키 -> 인덱스 0
				return;
			}
		}

		// 0번 키는 인덱스 9 (10번째 유닛)
		if (Input::Get().GetKeyDown(0x30))
		{
			SelectUnitByIndex(9);
			return;
		}

		// ESC 키: 유닛 선택 해제
		if (Input::Get().GetKeyDown(VK_ESCAPE))
		{
			if (selectedUnit != nullptr)
			{
				selectedUnit->SetState(UnitState::Idle);
				selectedUnit = nullptr;
				reachableTiles.clear();
			}
			return;
		}

		// ENTER 키: 턴 종료
		if (Input::Get().GetKeyDown(VK_RETURN))
		{
			// 플레이어 턴이면 적 턴으로, 적 턴이면 플레이어 턴으로
			isPlayerTurn = !isPlayerTurn;
			
			if (isPlayerTurn)
			{
				// 플레이어 턴 시작: 모든 플레이어 유닛 턴 초기화
				turnCount++;
				for (Unit* unit : playerUnits)
				{
					unit->ResetTurn();
				}
			}
			else
			{
				// 적 턴 시작: 모든 적 유닛 턴 초기화
				for (Unit* unit : enemyUnits)
				{
					unit->ResetTurn();
				}
			}

			// 선택 해제
			if (selectedUnit != nullptr)
			{
				selectedUnit->SetState(UnitState::Idle);
				selectedUnit = nullptr;
				reachableTiles.clear();
			}
			return;
		}

		// 마우스 왼쪽 클릭으로 유닛 이동
		if (Input::Get().GetMouseButtonDown(0))  // 0 = 왼쪽 버튼
		{
			Vector2 mousePos = Input::Get().MousePosition();
			OnMouseClick(mousePos);
			return;
		}
	}
}
