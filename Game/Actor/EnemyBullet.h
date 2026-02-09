#pragma once

#include "Actor/Actor.h"

using namespace Wanted;

class EnemyBullet : public Actor
{
	RTTI_DECLARATIONS(EnemyBullet, Actor)

public:
	EnemyBullet(
		const Vector2& position,
		float moveSpeed = 15.0f
	);

private:
	virtual void Tick(float deltaTime) override;

private:
	// 이동 처리를 위한 변수.
	float moveSpeed = 0.0f; // 이후 랜덤으로 처리 예정

	// y 이동 위치 처리를 위한 float 변수.
	float yPosition = 0.0f;
};
