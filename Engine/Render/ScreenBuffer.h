#pragma once

#include "Math/Vector2.h"
#include <Windows.h>

namespace FE
{
	/*
	* �ۼ��� : �弼��
	* ��¥   : 2026.02.03
	* ����   : ���� ���۸��� ����� Console Output �ڵ��� �����ϴ� Ŭ����.
	*/
	class ScreenBuffer
	{
	public:
		ScreenBuffer(const Vector2& screenSize);
		~ScreenBuffer();

		// �ܼ� ���۸� ����� �Լ�.
		void Clear();

		// �ֿܼ� 2���� ���� �迭�� �׸� �� ����ϴ� �Լ�.
		void Draw(CHAR_INFO* charInfo);

		// ���� ��ȯ Getter.
		inline HANDLE GetBuffer() const { return buffer; }

	private:
		// �ܼ� ��� �ڵ�.
		HANDLE buffer = nullptr;

		// ȭ�� ũ��.
		Vector2 screenSize;
	};
}