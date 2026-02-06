#pragma once

#include "Common/Common.h"

namespace FE
{
	// ���� ����.
	class Input;
	class Renderer;
	// Tip: ���� ���� ���� �� �ý������� �͵��� ��������� �̱������� ���������� �����ϵ��� ��.

	// Main game engine class.
	class FE_API Engine
	{
		// ���� ���� ����ü.
		struct EngineSetting
		{
			// ������ �ӵ�.
			float framerate = 0.0f;

			// ȭ�� �ʺ�.
			int width = 0;

			// ȭ�� ����.
			int height = 0;
		};

	public:
		Engine();
		// Tip: �Ҹ��ڿ� virtual �ٿ���� �ڼ� ��ü������ ������� �ʰ� �� �����
		virtual ~Engine();

		// ���� ����(���� ����).
		void Run();

		// ���� ���� �Լ�.
		void QuitEngine();

		// �� ������ �߰�(����)�ϴ� �Լ�.
		void SetNewLevel(class Level* newLevel);

		// ���� ���� �Լ�.
		static Engine& Get();

		// ȭ�� �ʺ� ��ȯ �Լ�.
		inline int GetWidth() const { return setting.width; }

		// ȭ�� ���� ��ȯ �Լ�.
		inline int GetHeight() const { return setting.height; }

	protected:
		// ���� �Լ�.
		void Shutdown();

		// ���� ���� �ε� �Լ�.
		void LoadSetting();

		// ���� �÷��� ���� �Լ�.
		// Unity�� ���: Start/Awake.
		void BeginPlay();

		// ������Ʈ �Լ� (deltaTime: ������ ���� �ð�)
		void Tick(float deltaTime);

		// �׸��� �Լ�. (Draw/Render).
		void Draw();

	protected:
		// ���� ���� �÷��� (���Ǻ� ���ѷ���)
		bool isQuit = false;

		// ���� ���� ��.
		EngineSetting setting;

		// �Է� ������.
		Input* input = nullptr;

		// ������ ��ü.
		Renderer* renderer = nullptr;

		// ���� ����.
		class Level* mainLevel = nullptr;

		// ��ȯ�� ������ �ӽ� �����ϴ� ����.
		class Level* nextLevel = nullptr;

		// ���� ����.
		static Engine* instance;
	};
}
