#pragma once

#include "Common/RTTI.h"

// STL(Standard Template Library - C++���� �⺻ ����).
// ũ�Ⱑ �˾Ƽ� ����Ǵ� �迭.
#include <vector>

namespace FE
{
	// ���� ����.
	class Actor;

	// ��� �ӹ�: ������ �ִ� ��� ����(��ü) ����.
	class FE_API Level : public RTTI
	{
		RTTI_DECLARATIONS(Level, RTTI)

	public:
		Level();
		virtual ~Level();

		// ���� �÷��� �̺�Ʈ.
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// ���� �߰� �Լ�.
		void AddNewActor(Actor* newActor);

		// ���� �߰�/���� ó�� �Լ�.
		void ProcessAddAndDestroyActors();

	protected:
		// ���� �迭.
		std::vector<Actor*> actors;

		// ���� �߿� �߰� ��û�� ������ �迭.
		std::vector<Actor*> addRequestedActors;
	};
}