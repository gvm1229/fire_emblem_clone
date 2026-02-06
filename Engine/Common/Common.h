#pragma once
#pragma warning(disable: 4251)	// �̺�å(��� ����).
#pragma warning(disable: 4172)	// ���輺 ������ ���·� ���.

//#define ENGINE_BUILD_DLL 1

#define DLLEXPORT   __declspec(dllexport)
#define DLLIMPORT   __declspec(dllimport)

// ENGINE_BUILD_DLL�� ����Ǿ� �ִ����� ���� �б�.
#if ENGINE_BUILD_DLL
#define FE_API DLLEXPORT
#else
#define FE_API DLLIMPORT
#endif