#ifndef _NGINE_H_
#define _NGINE_H_

#define DIRECTINPUT_VERSION  0x0800
#define INITGUID
//#define KEYDOWN(name, key) (name[key]&0x80)
//#define MOUSE_LB 0
//#define MOUSE_RB 1
//#define MOUSE_MB 2

#define MAX_OBJECTS 1000

#include <windows.h>
#include <dinput.h>
#include <d3dx9.h>

class OBJECT3D {

public:
	BOOL						drawEnabled;
	static int			count;
	static OBJECT3D	**objects;
	
	OBJECT3D(LPDIRECT3DDEVICE9 dev, char* address, bool visible);
	virtual ~OBJECT3D();
	virtual void draw();
	virtual void Physic() {};
	virtual void Control() {};
	void release();
	void refresh();
	void changeMesh(char* address);
	void setPosition(float x, float y, float z);
	D3DXVECTOR3 getPosition();
	D3DXVECTOR3 getAngles();

private:
	LPDIRECT3DDEVICE9		device;			// pointer to device
	D3DXVECTOR3					position;		// position
	D3DXVECTOR3					angle;			// angles
	LPD3DXBUFFER				meshBuffer;
	LPDIRECT3DTEXTURE9	*meshTexture;
	D3DMATERIAL9				*meshMaterials;
	LPD3DXMESH					mesh;
	DWORD								partCount;
	char								path[256];
	
	void load(char* address);
};



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

	HRESULT		createWindow();
	VOID			destroyWindow();

	HRESULT		initDirect3D();
	VOID			destroyDirect3D();

	HRESULT		initInput();
	VOID			destroyInput();

	VOID			engine();
	OBJECT3D*	createObject(char* pathToObject, bool visible);

private:
	GAMEWINDOW						*window;
	HINSTANCE							hinstance;

	LPDIRECT3D9						Direct3D;
	LPDIRECT3DDEVICE9			Device;

	LPDIRECTINPUT8				Input;
	LPDIRECTINPUTDEVICE8	Mouse,
												Keyboard;

	BYTE									KeyboardKeys[256];
	DIMOUSESTATE					MouseState;

	D3DXMATRIX						World, 
												Camera, 
												Projection;

	FLOAT									Correlation;
	BOOL									isDeviceLosted;
	
	VOID		getInput();
	VOID		controlInput();
	VOID    render();

	static LRESULT CALLBACK mainWinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

#endif