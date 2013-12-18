#ifndef _NGINE_H_
#define _NGINE_H_

#define DIRECTINPUT_VERSION  0x0800
#define INITGUID
//#define KEYDOWN(name, key) (name[key]&0x80)
//#define MOUSE_LB 0
//#define MOUSE_RB 1
//#define MOUSE_MB 2

#include <windows.h>
#include <dinput.h>
#include <d3dx9.h>

class GAMEWINDOW : public WNDCLASSEX {

public:
	HWND hwnd;
	bool isFullScreen;
	int  width;
	int  height;
	int  top;
	int  left;

	GAMEWINDOW(bool fullScreen, int width, int height);
};



class NGINE {

public:

	NGINE(HINSTANCE hinst);
	~NGINE();

	HRESULT createWindow();
	VOID		destroyWindow();

	HRESULT	initDirect3D();
	VOID		destroyDirect3D();

	HRESULT	initInput();
	VOID		getInput();
	VOID		destroyInput();

private:
	GAMEWINDOW						*window;
	HINSTANCE							hinstance;

	LPDIRECT3D9						Direct3D;
	LPDIRECT3DDEVICE9			Device;

	LPDIRECTINPUT8				Input;
	LPDIRECTINPUTDEVICE8	Mouse;
	LPDIRECTINPUTDEVICE8	Keyboard;
	BYTE									KeyboardKeys[256];
	DIMOUSESTATE					MouseState;

	FLOAT							Correlation;

	static LRESULT CALLBACK mainWinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

#endif