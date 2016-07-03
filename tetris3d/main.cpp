#include "Window.h"

int WINAPI wWinMain(HINSTANCE  hInstance, HINSTANCE hPrevInstance,  PWSTR lpCmdLine, int nCmdShow)
{
	bool fullscreen = true;
	Window window(hInstance);
	if (!window.Show(fullscreen)) {
		return -1;
	}
	window.RunGame();
	BOOL done = false;
	MSG  msg;
	while (!done) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				done = true;                               
			}
			else {
				TranslateMessage(&msg);                    
				DispatchMessage(&msg);                      
			}
		}
		else if (window.IsActive()) {
			if (window.ExitRequested()) {			
				done = true;
			}
			else {
				window.Update();
				if (!window.SwitchModeIfRequested()) {
					return -1;
				}
			}
		}
	}  
	window.ReleaseResources();
	return msg.wParam;                                 
}
