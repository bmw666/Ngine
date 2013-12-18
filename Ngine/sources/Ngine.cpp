#include "../headers/Ngine.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dinput8.lib")

//== NGINE =======================================================================================

NGINE::NGINE(HINSTANCE hinst) {
	hinstance = hinst;
	window    = NULL;

	Direct3D  = NULL;
	Device    = NULL;

	Input			= NULL;
	Mouse			= NULL;
	Keyboard	= NULL;
//BYTE									Key[256];
//DIMOUSESTATE					MouseState;
}

NGINE::~NGINE() {
	destroyInput();
	destroyDirect3D();
	destroyWindow();
}

LRESULT CALLBACK NGINE::mainWinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}

// create game window
HRESULT NGINE::createWindow() {
	window = new GAMEWINDOW(false, 800, 600);

	window->cbSize				= sizeof(WNDCLASSEX);										// size of class
	window->style					= CS_OWNDC | CS_DBLCLKS;								// window styles
	window->lpfnWndProc		= &NGINE::mainWinProc;									// set handler function
	window->cbClsExtra		= 0;																		// reserved fields
	window->cbWndExtra		= 0;
	window->hInstance			= hinstance;														// instance of application
	window->hIcon					= LoadIcon(NULL, IDI_APPLICATION);			// icon
	window->hIconSm				= LoadIcon(NULL, IDI_APPLICATION);			// icon on panel
	window->hCursor				= LoadCursor(NULL, IDC_ARROW);					// cursor
	window->hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);	// background color
	window->lpszMenuName	= NULL;																	// standart window menu
	window->lpszClassName	= "WINDOWSCLASS";												// class name

	if (!RegisterClassEx( window ))		// register class
		return S_FALSE;

	if (!(window->hwnd = CreateWindowEx(NULL,
		"WINDOWSCLASS", "NGINE : by B&A Games Corporation",
			//  without border
			//WS_POPUP | 
			WS_OVERLAPPED | 
			WS_VISIBLE,
			window->left, window->top,
			window->width, window->height, 
			NULL, NULL, hinstance, NULL))) 
		return S_FALSE;
		
	ShowWindow(window->hwnd, SW_SHOWDEFAULT);
	UpdateWindow(window->hwnd);

	return S_OK;
}

VOID NGINE::destroyWindow() {
	if (window != NULL)
		delete window;
	window = NULL;
}

// init direcx3d device
HRESULT NGINE::initDirect3D() {

	if ((Direct3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return E_FAIL;

	D3DDISPLAYMODE display;

	if (FAILED(Direct3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &display)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS Direct3DParametr;
	
	ZeroMemory(&Direct3DParametr, sizeof(Direct3DParametr));
	Direct3DParametr.Windowed = !window->isFullScreen;
	Direct3DParametr.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Direct3DParametr.BackBufferFormat = display.Format;
	
	// fullscreen smoothing
	//if (RELEASE)
	{
		if(SUCCEEDED(Direct3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
			Direct3DParametr.BackBufferFormat, FALSE, D3DMULTISAMPLE_2_SAMPLES, NULL)))
			Direct3DParametr.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
//		if(SUCCEEDED(pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
//			Direct3DParametr.BackBufferFormat, FALSE, D3DMULTISAMPLE_4_SAMPLES, NULL)))
//			Direct3DParametr.MultiSampleType=D3DMULTISAMPLE_4_SAMPLES;
//		Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS,DWORD(TRUE) );
//		Device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE,DWORD(TRUE) );
	}

	Direct3DParametr.EnableAutoDepthStencil = TRUE;						// set z-buffer
	Direct3DParametr.AutoDepthStencilFormat = D3DFMT_D24S8;

	if (window->isFullScreen) {
		Direct3DParametr.BackBufferWidth = window->width;
		Direct3DParametr.BackBufferHeight = window->height;
		Direct3DParametr.BackBufferCount = 3;
		Direct3DParametr.FullScreen_RefreshRateInHz = display.RefreshRate;
	}

	Correlation = window->width/float(window->height);

	// try to set hardware processing
	if (FAILED(Direct3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL, window->hwnd,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&Direct3DParametr, &Device))) {
		
		// if failset software processing
		// new MESSAGE("Ваша карта не поппдерживает апаратное ускорение :(", 5);
		if (FAILED(Direct3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL, window->hwnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&Direct3DParametr, &Device)))
			// if fail close application
			return E_FAIL;
	}

	// turn off light
	//Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	Device->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
	Device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	// туман
	DWORD Color = 0x808080; // Цвет - белый 
	DWORD Mode = D3DFOG_LINEAR; // Вид
	float Start = 50.f;
	float End = 200.f;
	float Density = 0.05f;
	Device->SetRenderState(D3DRS_FOGCOLOR, Color);
	Device->SetRenderState(D3DRS_FOGVERTEXMODE, Mode);
	Device->SetRenderState(D3DRS_FOGTABLEMODE, Mode);

  // Ставим началное и конечное расстояние для тумана
  Device->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
  Device->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));

	// для прозрачных текстур
	Device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
  Device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
  Device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );    
  Device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
  Device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
  Device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	//LightMaterial();
	return S_OK;	
}

VOID NGINE::destroyDirect3D() {
	if (Device != NULL)
		Device->Release();
	Device = NULL;

	if (Direct3D != NULL)
		Direct3D->Release();
	Direct3D = NULL;
}

// init directx input
HRESULT NGINE::initInput() {

	DirectInput8Create(hinstance , DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (void**) &Input, NULL);
	
	if FAILED( Input->CreateDevice(GUID_SysKeyboard, &Keyboard, NULL) ) {
		destroyInput();
		MessageBox(window->hwnd, "Не удалось установить устройство клавиатуры", "Клавиатура", MB_OK);
		return S_FALSE;
	}

	if FAILED( Keyboard->SetCooperativeLevel(window->hwnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) ) {
		destroyInput();
		MessageBox(window->hwnd, "Не удалось установить уровень взаемодействия с клавиатурой", "Клавиатура", MB_OK);
		return S_FALSE;
	}

	if FAILED( Keyboard->SetDataFormat(&c_dfDIKeyboard) ) {
		destroyInput();
		MessageBox(window->hwnd, "Не удалось установить формат данных клавиатуры", "Клавиатура", MB_OK);
		return S_FALSE;
	} 

	if FAILED( Keyboard->Acquire() ) {
		destroyInput();
		MessageBox(window->hwnd, "Не удалось захватить клавиатуру", 
			"Клавиатура", MB_OK);
		return S_FALSE;
	}

	if FAILED( Input->CreateDevice(GUID_SysMouse, &Mouse, NULL) ) {
		destroyInput();
		MessageBox(window->hwnd, "Не удалось установить устройство мыши", "Мышь", MB_OK);
		return S_FALSE;
	}

	if FAILED( Mouse->SetCooperativeLevel(window->hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE) ) {
		destroyInput();
		MessageBox(window->hwnd, "Не удалось установить уровень взаемодействия с мышей", "Мышь", MB_OK);
		return S_FALSE;
	} 

	if FAILED( Mouse->SetDataFormat(&c_dfDIMouse) ) {
		destroyInput();
		MessageBox(window->hwnd, "Не удалось установить формат данных мыши", "Мышь", MB_OK);
		return S_FALSE;
	}

	if FAILED( Mouse->Acquire() ) {
		destroyInput();
		MessageBox(window->hwnd, "Не удалось захватить мышь", "Мышь", MB_OK);
		return S_FALSE;
	}

	return S_OK;
}

VOID NGINE::getInput() {
	
	static bool LostFocus=false;
	
	// if window not in focus, don't take input
	if (window->hwnd != GetFocus()) {LostFocus=true; return;}
	if (LostFocus) {destroyInput(); initInput(); LostFocus=false;}

	if FAILED( Keyboard->GetDeviceState(256, (LPVOID)&KeyboardKeys) ) {
		destroyInput();
		MessageBox(window->hwnd, "Не удалось считать данные с клавиатуры", "Клавиатура", MB_OK);
		return;
	}

	if FAILED( Mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&MouseState) ) {
		destroyInput();
		MessageBox(window->hwnd, "Не удалось считать данные с мыши", "Мышь", MB_OK);
		return;
	}
}

VOID NGINE::destroyInput() {
	if (Input) {
		if(Keyboard) {
			if (Keyboard != NULL)
				Keyboard->Unacquire();
			if (Keyboard != NULL)
				Keyboard->Release();
			Keyboard = NULL;
		}

		if(Mouse) {
			if (Mouse != NULL)
				Mouse->Unacquire();
			if (Mouse != NULL)
				Mouse->Release();
			Mouse = NULL;
		}

		if (Input != NULL)
			Input->Release();
		Input = NULL;
	}
}

//== GAMEWINDOW ==================================================================================

GAMEWINDOW::GAMEWINDOW(bool fullScreen, int screenWidth=0, int screenHeight=0) {
	isFullScreen	= fullScreen;
	if (isFullScreen) {
		width  = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
		left   = 0;
		top    = 0;
	}
	else {
		width		= screenWidth;
		height	= screenHeight;
		left    = (GetSystemMetrics(SM_CXSCREEN) - width)/2;
		top     = (GetSystemMetrics(SM_CYSCREEN) - height)/2;
	}
}