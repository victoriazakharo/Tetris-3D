#pragma once
#include <Windows.h>
#include "Game.h"
#include "SimpleTimer.h"

class Window
{
public:
	Window(HINSTANCE instance);
	void Update();
	bool Show(bool fullscreenflag);
	bool ExitRequested();
	bool SwitchModeIfRequested();
	bool IsActive() const;
	void RunGame();
	void ReleaseResources();
	~Window();
private:
	const LPCWSTR TITLE = L"Fog Tetris";
	const LPCWSTR CLASSNAME = L"Tetris Window";

	HGLRC      hRC;
	HDC        hDC;
	HWND       hWnd;
	HINSTANCE  hInstance;

	bool  keys[256] = {};
	bool  active = true;
	bool  fullscreen = true;	
	SimpleTimer timer;
	Game *game;

	LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK GlobalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	
	
	bool SetFullScreenSettings(int width, int height);
	bool SetWindowContexts();
	bool CreateGameWindow(int width, int height);	
	void Draw();
	void UpdateGame();
	static void ShowErrorMessage(LPCWSTR text, LPCWSTR title);
};





