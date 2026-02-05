#include "PlayerBullet.h"

PlayerBullet::PlayerBullet(const Vector2& position)
	: super("@", position, Color::Blue),
	yPosition(static_cast<float>(position.y))
{
}

PlayerBullet::~PlayerBullet()
{
}

void PlayerBullet::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 위로 이동 처리. 초단위로 이동.
	yPosition -= moveSpeed * deltaTime;

	// 좌표 검사.
	if (yPosition < 0.0f)
	{
		// 삭제 처리.
		Destroy();
		return;
	}

	// 액터의 위치로 변환.
	Vector2 newPosition = GetPosition();
	newPosition.y = static_cast<int>(yPosition);
	
	// 위치 갱신.
	SetPosition(newPosition);
}
