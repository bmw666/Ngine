﻿#include "main.h"
#include "Ngine/headers/Ngine.h"

int WINAPI WinMain(HINSTANCE	hinstance,
									 HINSTANCE	hprevinstance,
									 LPSTR			lpcmdline,
									 int				ncmdshow)
{
	MSG msg;
	
	NGINE *ngine = new NGINE(hinstance);
	ngine->createWindow();
	ngine->initDirect3D();
	ngine->initInput();

	OBJECT3D *cyl = ngine->createObject("objects/cylinder.x", true);
	cyl->setPosition(-10, -10, -10);

	//if (SUCCEEDED( InitDirect3D(window, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) ) )) {
	//	InitDrawText();
	//	LoadArea();
	ZeroMemory(&msg, sizeof(msg));
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) break;
			/*switch(msg.message)
			{
				case WM_ACTIVATEAPP : ; ResumeGame(); break;
			}*/
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			ngine->engine();
//			//if (GameMode) {
//				if (timeGetTime() - startTime > 33 || limitFPS)	{
//					startTime = timeGetTime();					
//					//Engine();
//					Render();
//					frameTime = timeGetTime() - startTime;
//					/*if (FPS%10 == 0)
//						sprintf_s(&RENDER_TIME[8], 10, "%d", frameTime);*/
//					FPS++;
//				}
//				if (FPSTime+1000 < timeGetTime()) {
//					//sprintf_s(&STR_FPS[5], 10, "%d", FPS);
//					FPSTime = timeGetTime();
//					FPS=0;
//				}
			}
		}
	
	//DeleteDrawText();
	//DeleteInput();
	
	delete ngine;

	return 0;
}
