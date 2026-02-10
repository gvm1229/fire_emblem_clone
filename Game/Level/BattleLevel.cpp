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

		// 적 턴: AI 처리
		if (!isPlayerTurn)
		{
			// 이동 중인 적이 있으면 대기
			bool anyEnemyMoving = false;
			for (Unit* unit : enemyUnits)
			{
				if (unit->IsMoving())
				{
					anyEnemyMoving = true;
					break;
				}
			}

			if (!anyEnemyMoving)
			{
				// 행동하지 않은 첫 번째 적 찾기
				Unit* nextEnemy = nullptr;
				for (Unit* unit : enemyUnits)
				{
					if (!unit->HasActedThisTurn())
					{
						nextEnemy = unit;
						break;
					}
				}

				if (nextEnemy != nullptr)
				{
					enemyAI.RunAI(nextEnemy, playerUnits, grid, movementCalculator, navigationSystem,
						[this](const char* msg) { AddLog(msg); });
				}
				else
				{
					// 모든 적이 행동 완료 → 플레이어 턴으로 전환
					AddLog("ENEMY TURN ENDED");
					isPlayerTurn = true;
					turnCount++;
					for (Unit* unit : playerUnits)
					{
						unit->ResetTurn();
					}
				}
			}
		}

		// 입력 처리
		HandleInput();

		// 플레이어 턴: 모든 플레이어 유닛이 행동 완료하면 자동으로 턴 종료
		if (isPlayerTurn)
		{
			bool allPlayerUnitsDone = true;
			for (Unit* unit : playerUnits)
			{
				if (!unit->HasActedThisTurn())
				{
					allPlayerUnitsDone = false;
					break;
				}
			}
			if (allPlayerUnitsDone && !playerUnits.empty())
			{
				isPlayerTurn = false;
				for (Unit* unit : enemyUnits)
				{
					unit->ResetTurn();
				}
				if (selectedUnit != nullptr)
				{
					selectedUnit->SetState(UnitState::Idle);
					selectedUnit = nullptr;
					reachableTiles.clear();
				}
				AddLog("PLAYER TURN ENDED");
			}
		}
	}

	// Draw: 렌더링
	void BattleLevel::Draw()
	{
		// 적 유닛 색상: 적 턴일 때만 Done을 보라색으로, 플레이어 턴에는 항상 빨간색
		for (Unit* unit : enemyUnits)
		{
			unit->SetDisplayAsEnemyTurn(!isPlayerTurn);
		}

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

	// 이벤트 로그 패널
	DrawLogPanel();

	// 키보드 툴팁
	DrawKeyboardTooltip();

	// 턴 정보 표시 (클래스 멤버 버퍼 사용)
	const char* phaseStr = isPlayerTurn ? "PLAYER" : "ENEMY";
	sprintf_s(uiBuffers[0], sizeof(uiBuffers[0]), "Turn:%d Phase:%s", turnCount, phaseStr);
	Renderer::Get().Submit(uiBuffers[0], Vector2(0, 0), Color::White, 10);
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
		// 화면 좌표를 그리드 좌표로 변환 (UI 공간 1칸 + 2x2 그리드 고려)
		int gridX = (mousePos.x - 1) / 2;
		int gridY = (mousePos.y - 1) / 2;
		Vector2 gridPos(gridX, gridY);

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

					// 로그: Unit #N moved X tiles to Terrain
					{
						int unitNum = selectedUnit->GetUnitIndex() >= 0 ? selectedUnit->GetUnitIndex() + 1 : 0;
						const char* terrainName = GetTerrainTypeName(newTile ? newTile->GetTerrainType() : TerrainType::Plain);
						char logBuf[80];
						sprintf_s(logBuf, sizeof(logBuf), "Unit #%d moved %zu tiles to %s.", unitNum, path.size(), terrainName);
						AddLog(logBuf);
					}

					// 선택 해제
					selectedUnit = nullptr;
					reachableTiles.clear();
				}
			}
		}
	}

	// 그리드 렌더링 (멀티라인 ASCII - 2x2)
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
					// 각 타일을 2x2로 렌더링
					const char* topLeft;
					const char* topRight;
					const char* bottomLeft;
					const char* bottomRight;
					tile->GetDisplayStrings(topLeft, topRight, bottomLeft, bottomRight);

					// UI 공간 1칸 확보 + 2x2 그리드
					int baseX = x * 2 + 1;
					int baseY = y * 2 + 1;

					// Render Priority: 7 (Plain terrain)
					if (tile->GetTerrainType() == TerrainType::Plain)
					{
						Renderer::Get().Submit(topLeft, Vector2(baseX, baseY), tile->GetDisplayColor(), 7);
						Renderer::Get().Submit(topRight, Vector2(baseX + 1, baseY), tile->GetDisplayColor(), 7);
						Renderer::Get().Submit(bottomLeft, Vector2(baseX, baseY + 1), tile->GetDisplayColor(), 7);
						Renderer::Get().Submit(bottomRight, Vector2(baseX + 1, baseY + 1), tile->GetDisplayColor(), 7);
					}
					else // Render Priority: 9 (Terrain)
					{
						Renderer::Get().Submit(topLeft, Vector2(baseX, baseY), tile->GetDisplayColor(), 9);
						Renderer::Get().Submit(topRight, Vector2(baseX + 1, baseY), tile->GetDisplayColor(), 9);
						Renderer::Get().Submit(bottomLeft, Vector2(baseX, baseY + 1), tile->GetDisplayColor(), 9);
						Renderer::Get().Submit(bottomRight, Vector2(baseX + 1, baseY + 1), tile->GetDisplayColor(), 9);
					}
				}
			}
		}
	}

	// 이동 가능 범위 하이라이트 (2x2)
	void BattleLevel::DrawMovementRange()
	{
		for (const Vector2& tile : reachableTiles)
		{
			// 2x2 좌표로 변환
			int baseX = tile.x * 2 + 1;
			int baseY = tile.y * 2 + 1;

			// Render Priority: 8 (Tile highlights - below units and non-plain terrains)
			Color highlightColor = Color::Cyan;

			// 2x2로 하이라이트 렌더링
			Renderer::Get().Submit("·", Vector2(baseX, baseY), highlightColor, 8);
			Renderer::Get().Submit("·", Vector2(baseX + 1, baseY), highlightColor, 8);
			Renderer::Get().Submit("·", Vector2(baseX, baseY + 1), highlightColor, 8);
			Renderer::Get().Submit("·", Vector2(baseX + 1, baseY + 1), highlightColor, 8);
		}
	}

	// 스탯 UI 패널 (오른쪽)
	void BattleLevel::DrawStatsPanel()
	{
		// 맵에서 5칸 떨어진 위치에 패널 배치 (2x2 그리드 고려)
		int panelX = (grid != nullptr) ? grid->GetWidth() * 2 + 6 : 37;
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
				sprintf_s(uiBuffers[1], sizeof(uiBuffers[1]), "Unit: #%d (Player)", selectedUnit->GetUnitIndex() + 1);
			}
			else
			{
				sprintf_s(uiBuffers[1], sizeof(uiBuffers[1]), "Unit: #%d", selectedUnit->GetUnitIndex() + 1);
			}
			Renderer::Get().Submit(uiBuffers[1], Vector2(panelX, panelY++), Color::White, 10);

			panelY++;

			// HP 표시
			sprintf_s(uiBuffers[2], sizeof(uiBuffers[2]), "HP: %d/%d", stats.currentHP, stats.maxHP);
			Renderer::Get().Submit(uiBuffers[2], Vector2(panelX, panelY++), Color::White, 10);

			// 클래스 표시 (전체 이름)
			sprintf_s(uiBuffers[3], sizeof(uiBuffers[3]), "Class: %s", GetFullUnitClassString(unitClass));
			Renderer::Get().Submit(uiBuffers[3], Vector2(panelX, panelY++), Color::White, 10);

			panelY++;

			// 스탯 표시
			sprintf_s(uiBuffers[4], sizeof(uiBuffers[4]), "STR: %d", stats.strength);
			Renderer::Get().Submit(uiBuffers[4], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[5], sizeof(uiBuffers[5]), "MAG: %d", stats.magic);
			Renderer::Get().Submit(uiBuffers[5], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[6], sizeof(uiBuffers[6]), "SKL: %d", stats.skill);
			Renderer::Get().Submit(uiBuffers[6], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[7], sizeof(uiBuffers[7]), "SPD: %d", stats.speed);
			Renderer::Get().Submit(uiBuffers[7], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[8], sizeof(uiBuffers[8]), "LCK: %d", stats.luck);
			Renderer::Get().Submit(uiBuffers[8], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[9], sizeof(uiBuffers[9]), "DEF: %d", stats.defense);
			Renderer::Get().Submit(uiBuffers[9], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[10], sizeof(uiBuffers[10]), "RES: %d", stats.resistance);
			Renderer::Get().Submit(uiBuffers[10], Vector2(panelX, panelY++), Color::White, 10);

			sprintf_s(uiBuffers[11], sizeof(uiBuffers[11]), "MOV: %d", stats.movement);
			Renderer::Get().Submit(uiBuffers[11], Vector2(panelX, panelY++), Color::White, 10);

			// 구분선
			panelY++;
			Renderer::Get().Submit("------------------", Vector2(panelX, panelY++), Color::White, 10);
			
			// 지형 정보 표시
			Tile* currentTile = grid->GetTile(selectedUnit->GetGridPosition());
			if (currentTile != nullptr)
			{
				Renderer::Get().Submit("  TERRAIN INFO", Vector2(panelX, panelY++), Color::Yellow, 10);
				Renderer::Get().Submit("------------------", Vector2(panelX, panelY++), Color::White, 10);
				panelY++;

				// 지형 타입
				sprintf_s(uiBuffers[12], sizeof(uiBuffers[12]), "Terrain: %s", GetTerrainTypeName(currentTile->GetTerrainType()));
				Renderer::Get().Submit(uiBuffers[12], Vector2(panelX, panelY++), Color::White, 10);

				panelY++;

				// 지형 보너스
				Renderer::Get().Submit("Bonuses:", Vector2(panelX, panelY++), Color::Cyan, 10);
				
				sprintf_s(uiBuffers[13], sizeof(uiBuffers[13]), "  Avoid: +%d", currentTile->GetAvoidBonus());
				Renderer::Get().Submit(uiBuffers[13], Vector2(panelX, panelY++), Color::White, 10);
				
				sprintf_s(uiBuffers[14], sizeof(uiBuffers[14]), "  Defense: +%d", currentTile->GetDefenseBonus());
				Renderer::Get().Submit(uiBuffers[14], Vector2(panelX, panelY++), Color::White, 10);
				
				// 회복 지형이면 표시
				if (currentTile->IsHealingTile())
				{
					sprintf_s(uiBuffers[15], sizeof(uiBuffers[15]), "  Healing: Yes");
					Renderer::Get().Submit(uiBuffers[15], Vector2(panelX, panelY++), Color::Green, 10);
				}
				else
				{
					sprintf_s(uiBuffers[15], sizeof(uiBuffers[15]), "  Healing: No");
					Renderer::Get().Submit(uiBuffers[15], Vector2(panelX, panelY++), Color::White, 10);
				}
			}
		}
		else
		{
			Renderer::Get().Submit("No unit selected", Vector2(panelX, panelY), Color::White, 10);
		}

		// 구분선 (맵과 입력 모니터 사이) - 2x2 그리드 고려
		int separatorY = (grid != nullptr) ? grid->GetHeight() * 2 + 2 : 32;
		Renderer::Get().Submit("--------------------", Vector2(0, separatorY), Color::White, 10);
	}

	// 이벤트 로그 패널 (유닛 정보 오른쪽). 로그 메시지는 줄 단위로 감싸서 전체 표시.
	// Submit()은 포인터만 저장하므로, 래핑된 줄은 wrappedLogLines에 보관해 Draw() 완료까지 수명 유지.
	void BattleLevel::DrawLogPanel()
	{
		int panelX = (grid != nullptr) ? grid->GetWidth() * 2 + 6 : 37;
		int logX = panelX + 20;  // UNIT INFO 패널 오른쪽에 인접
		int logY = 0;

		Renderer::Get().Submit("======================", Vector2(logX, logY), Color::White, 10);
		Renderer::Get().Submit("   EVENT LOG", Vector2(logX, logY + 1), Color::Yellow, 10);
		Renderer::Get().Submit("======================", Vector2(logX, logY + 2), Color::White, 10);
		logY += 4;

		wrappedLogLines.clear();
		const int wrapWidth = 48;  // 한 줄 최대 문자 수 (창 너비에 맞춤)
		for (const std::string& msg : eventLog)
		{
			if (msg.empty())
				continue;
			size_t pos = 0;
			while (pos < msg.length())
			{
				size_t remain = msg.length() - pos;
				size_t len = remain > static_cast<size_t>(wrapWidth)
					? static_cast<size_t>(wrapWidth) : remain;
				// 줄 끝 근처에서 공백이 있으면 그곳에서 줄바꿈
				if (len == static_cast<size_t>(wrapWidth) && pos + len < msg.length())
				{
					size_t lastSpace = msg.rfind(' ', pos + len);
					if (lastSpace != std::string::npos && lastSpace >= pos)
						len = lastSpace - pos + 1;
				}
				wrappedLogLines.push_back(msg.substr(pos, len));
				pos += len;
				while (pos < msg.length() && msg[pos] == ' ')
					pos++;
			}
		}
		for (size_t i = 0; i < wrappedLogLines.size(); ++i)
			Renderer::Get().Submit(wrappedLogLines[i].c_str(), Vector2(logX, logY + static_cast<int>(i)), Color::White, 10);
	}

	// 이벤트 로그 추가 (최근 20개 유지)
	void BattleLevel::AddLog(const char* message)
	{
		if (message == nullptr || message[0] == '\0')
			return;
		eventLog.push_back(message);
		while (eventLog.size() > static_cast<size_t>(kMaxEventLogEntries))
			eventLog.pop_front();
	}

	// 키보드 툴팁 (하단)
	void BattleLevel::DrawKeyboardTooltip()
	{
		// 맵 아래 충분히 떨어진 위치 (구분선 + 3칸) - 2x2 그리드 고려
		int separatorY = (grid != nullptr) ? grid->GetHeight() * 2 + 2 : 32;
		int tooltipY = separatorY + 3;  // 구분선 아래 3칸
		int col1X = 0;  // 첫 번째 열
		int col2X = 28; // 두 번째 열

		// 첫 번째 열: CONTROLS
		Renderer::Get().Submit("=== CONTROLS ===", Vector2(col1X, tooltipY), Color::Yellow, 10);
		Renderer::Get().Submit("[1-9,0] Select Unit", Vector2(col1X, tooltipY + 1), Color::White, 10);
		Renderer::Get().Submit("[Click] Move Unit", Vector2(col1X, tooltipY + 2), Color::White, 10);
		Renderer::Get().Submit("[ESC] Deselect", Vector2(col1X, tooltipY + 3), Color::White, 10);
		Renderer::Get().Submit("[SPACE] End Turn", Vector2(col1X, tooltipY + 4), Color::White, 10);

		// 두 번째 열: UNIT COLORS
		Renderer::Get().Submit("=== COLORS ===", Vector2(col2X, tooltipY), Color::Yellow, 10);
		Renderer::Get().Submit("[Yellow] Lord", Vector2(col2X, tooltipY + 1), Color::Yellow, 10);
		Renderer::Get().Submit("[Blue] Player", Vector2(col2X, tooltipY + 2), Color::Blue, 10);
		Renderer::Get().Submit("[Red] Enemy", Vector2(col2X, tooltipY + 3), Color::Red, 10);
		Renderer::Get().Submit("[Cyan] Selected", Vector2(col2X, tooltipY + 4), Color::Cyan, 10);
		Renderer::Get().Submit("[Purple] Done", Vector2(col2X, tooltipY + 5), Color::Purple, 10);
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

		// SPACE 키: 턴 종료
		if (Input::Get().GetKeyDown(VK_SPACE))
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
				// 플레이어가 Space로 턴 종료 → 적 턴 시작
				AddLog("PLAYER TURN FORCE ENDED");
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
