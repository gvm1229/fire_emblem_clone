#pragma once

#include "Common/RTTI.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

namespace FE
{
	// ���� ����.
	class Level;

	class FE_API Actor : public RTTI
	{
		// RTTI �ڵ� �߰�.
		RTTI_DECLARATIONS(Actor, RTTI)

	public:
		Actor(
			const char* image = "",
			const Vector2& position = Vector2::Zero, // Tip: static �����صθ� �⺻�� ���� �� ������
			Color color = Color::White
		);
		virtual ~Actor();

		// ���� �÷��� �̺�Ʈ.
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// ���� ��û �Լ�.
		void Destroy();

		// ������ �� �� ȣ��� �̺�Ʈ �Լ�.
		virtual void OnDestroy();

		// ���� ���� �Լ�.
		void QuitGame();

		// �浹 ���� Ȯ�� �Լ�.
		bool TestIntersect(const Actor* const other);

		// ������ �̹��� �� ���� �Լ�.
		void ChangeImage(const char* newImage);

		// ��ġ ���� �� �б� �Լ�.
		void SetPosition(const Vector2& newPosition);
		inline Vector2 GetPosition() const { return position; }

		// ���ʽ� �߰�/�б� �Լ�.
		inline void SetOwner(Level* newOwner) { owner = newOwner; }
		inline Level* GetOwner() const { return owner; }

		// Getter.
		inline bool HasBeganPlay() const
		{
			return hasBeganPlay;
		}

		inline bool IsActive() const
		{
			return isActive && !destroyRequested;
		}

		inline bool DestroyRequested() const
		{
			return destroyRequested;
		}

		inline int GetSortingOrder() const { return sortingOrder; }

		inline int GetWidth() const { return width; }

	protected:
		// �̹� BeginPlay �̺�Ʈ�� �޾Ҵ��� ����.
		bool hasBeganPlay = false;

		// Ȱ��ȭ ���� ����.
		bool isActive = true;

		// ���� �����ӿ� ���� ��û �޾Ҵ��� ����.
		bool destroyRequested = false;

		// �׸� ����(�̹���).
		char* image = nullptr;

		// ���ڿ� ����.
		int width = 0;

		// ����.
		Color color = Color::White;

		// ���ʽ�(Ownership).
		Level* owner = nullptr;

		// �׸��� �켱 ���� (���� ũ�� �켱������ ����).
		int sortingOrder = 0;

		// ��ġ.
		Vector2 position;
	};
}
