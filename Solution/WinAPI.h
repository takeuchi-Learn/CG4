#pragma once
#include <Windows.h>

class WinAPI {
private:
	WinAPI(const WinAPI& winapi) = delete;
	WinAPI& operator=(const WinAPI& winapi) = delete;

	WinAPI();
	~WinAPI();

	WNDCLASSEX w{}; // �E�B���h�E�N���X�̐ݒ�
	HWND hwnd;

	//�E�B���h�E�v���V�[�W��
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	const static int window_width = 1280;
	const static int window_height = 720;

	static WinAPI* getInstance();

	HWND getHwnd();
	WNDCLASSEX getW();

	void setWindowText(const LPCSTR window_title);

	//�Q�[�����[�v���Ŗ���Ăт����Btrue���Ԃ��Ă����炷���ɏI��������ׂ�
	//@return �ُ�̗L����bool�^�ŕԂ�(true == �ُ� , false == ����)
	bool processMessage();
};

