#pragma once
#include <Windows.h>

class WinAPI {
private:
	WinAPI(const WinAPI& winapi) = delete;
	WinAPI& operator=(const WinAPI& winapi) = delete;

	static WinAPI* winApi;

	WinAPI(const wchar_t* windowTitle = L"DirectXGame");
	~WinAPI();

	WNDCLASSEX w{}; // ウィンドウクラスの設定
	HWND hwnd;

	//ウィンドウプロシージャ
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	const static int window_width = 1280;
	const static int window_height = 720;

	static void create(const wchar_t* windowTitle = L"DirectXGame");
	static void destroy();
	static WinAPI* getInstance();

	HWND getHwnd();
	WNDCLASSEX getW();

	void setWindowText(const LPCSTR window_title);

	//ゲームループ内で毎回呼びだす。trueが返ってきたらすぐに終了させるべき
	//@return 異常の有無をbool型で返す(true == 異常 , false == 正常)
	bool processMessage();
};

