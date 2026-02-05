#include "Player.h"
#include "Core/Input.h"
#include "Engine/Engine.h"
#include "Actor/Box.h"
#include "Level/Level.h"
#include "Game/Game.h"

#include "Interface/ICanPlayerMove.h"

#include <iostream>
#include <Windows.h>

using namespace Wanted;

Player::Player(const Vector2& position)
	: super("P", position, Color::Red)
{
	// 그리기 우선순위 높게 설정.
	sortingOrder = 10;
}

void Player::BeginPlay()
{
	// 상위 함수 호출.
	// C++는 부모함수 가리키는 포인터가 없음.
	Actor::BeginPlay();

	//std::cout << "TestActor::BeginPlay().\n";
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// ESC키 처리
	if (Wanted::Input::Get().GetKeyDown(VK_ESCAPE))
	{
		// 메뉴 활성화
		Game::Get().ToggleMenu();
		return;
	}

	// Q키 종료.
	if (Wanted::Input::Get().GetKeyDown('Q'))
	{
		// Todo: 게임 엔진 종료 요청.
		Wanted::Engine::Get().QuitEngine();
	}

	// 스페이스로 박스 생성.
	// vk->virtual key.
	if (Input::Get().GetKeyDown(VK_SPACE))
	{
		// 박스 생성.
		if (owner)
		{
			owner->AddNewActor(new Box(GetPosition()));
		}
	}

	// 인터페이스 확인
	static ICanPlayerMove* canPlayerMoveInterface = nullptr;

	// 오너십 확인 (null 확인)
	if (!canPlayerMoveInterface && GetOwner())
	{
		// 인터페이스로 형변환
		canPlayerMoveInterface = dynamic_cast<ICanPlayerMove*>(GetOwner());
	}

	// 이동.
	if (Input::Get().GetKeyDown(VK_RIGHT) && GetPosition().x < 20)
	{
		// 이동 가능 여부 판단
		Vector2 newPosition(GetPosition().x + 1, GetPosition().y);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition))
		{
			SetPosition(newPosition);
		}

		//Vector2 newPosition = GetPosition();
		//newPosition.x += 1;
		//SetPosition(newPosition);
	}

	if (Input::Get().GetKeyDown(VK_LEFT) && GetPosition().x > 0)
	{
		// 이동 가능 여부 판단
		Vector2 newPosition(GetPosition().x - 1, GetPosition().y);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition))
		{
			SetPosition(newPosition);
		}

		//Vector2 newPosition = GetPosition();
		//newPosition.x -= 1;
		//SetPosition(newPosition);
	}

	if (Input::Get().GetKeyDown(VK_DOWN) && GetPosition().y < 15)
	{
		// 이동 가능 여부 판단
		Vector2 newPosition(GetPosition().x, GetPosition().y + 1);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition))
		{
			SetPosition(newPosition);
		}

		//Vector2 newPosition = GetPosition();
		//newPosition.y += 1;
		//SetPosition(newPosition);
	}

	if (Input::Get().GetKeyDown(VK_UP) && GetPosition().y > 0)
	{
		// 이동 가능 여부 판단
		Vector2 newPosition(GetPosition().x, GetPosition().y - 1);
		if (canPlayerMoveInterface->CanMove(GetPosition(), newPosition))
		{
			SetPosition(newPosition);
		}

		//Vector2 newPosition = GetPosition();
		//newPosition.y -= 1;
		//SetPosition(newPosition);
	}
}

void Player::Draw()
{
	Actor::Draw();
}