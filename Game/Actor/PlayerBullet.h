#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

class PlayerBullet : public Actor
{
	RTTI_DECLARATIONS(PlayerBullet, Actor)

public:
	PlayerBullet(const Vector2& position);
	~PlayerBullet();

	virtual void Tick(float deltaTime) override;

private:

	// 이동 속도.
	// 초당 몇칸 이동하는지를 나타냄.
	float moveSpeed = 30.0f;

	// 위치 갱신을 할 때 소수점처리를 위한 변수.
	float yPosition = 0.0f;
};

