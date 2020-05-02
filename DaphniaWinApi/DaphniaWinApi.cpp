// DaphniaWinApi.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DaphniaWinApi.h"
#include "ParallelPhysics/ParallelPhysics.h"

#pragma comment (lib, "msimg32.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DAPHNIAWINAPI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	PPh::Observer::Init();
	PPh::Observer::Instance()->StartSimulation();

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DAPHNIAWINAPI));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DAPHNIAWINAPI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	HBRUSH hb = ::CreateSolidBrush(RGB(0, 0, 0));
	wcex.hbrBackground = hb;
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DAPHNIAWINAPI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HBITMAP g_hbmBall = NULL;
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	constexpr int TIMER1_IDT = 1;
	constexpr int TIMER1_DURATION_MS = 30;
    switch (message)
    {
	case WM_CREATE:
		SetTimer(hWnd, TIMER1_IDT, TIMER1_DURATION_MS, (TIMERPROC)NULL);
		g_hbmBall = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BLUE));
		if (g_hbmBall == NULL)
			MessageBox(hWnd, L"Could not load IDB_BALL!", L"Error", MB_OK | MB_ICONEXCLAMATION);
		break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
	case WM_TIMER:
	{
		switch (wParam)
		{
			case TIMER1_IDT:
			{
				InvalidateRect(hWnd, NULL, false);
			}
			break;
		}
	}
	break;
	/*case WM_ERASEBKGND:
	{
		RECT rcWin;
		RECT rcWnd;
		HWND parWnd = GetParent(hWnd); // Get the parent window.
		HDC parDc = GetDC(parWnd); // Get its DC.

		GetWindowRect(hWnd, &rcWnd);

		POINT rectTL;
		rectTL.x = rcWnd.left;
		rectTL.y = rcWnd.top;
		ScreenToClient(hWnd, &rectTL);
		ScreenToClient(parWnd, &rectTL); // Convert to the parent's co-ordinates
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		GetClipBox(hdc, &rcWin);
		// Copy from parent DC.
		BitBlt(hdc, rcWin.left, rcWin.top, rcWin.right - rcWin.left,
			rcWin.bottom - rcWin.top, parDc, rcWnd.left, rcWnd.top, SRCCOPY);

		ReleaseDC(parWnd, parDc);
		EndPaint(hWnd, &ps);
	}
	break;*/
	//case WM_ERASEBKGND:
	//	return 1;
    case WM_PAINT:
    {
		BITMAP bm;
		PAINTSTRUCT ps;

		HDC hdc = BeginPaint(hWnd, &ps);

		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, g_hbmBall);

		static PPh::SP_EyeColorArray s_spEyeColorArrayOut;
		PPh::SP_EyeColorArray spEyeColorArrayOut = PPh::Observer::Instance()->GrabTexture();
		if (spEyeColorArrayOut)
		{
			s_spEyeColorArrayOut = spEyeColorArrayOut;
		}
		if (s_spEyeColorArrayOut)
		{
			PPh::EyeColorArray &eyeColorArray = *s_spEyeColorArrayOut;
			for (uint32_t yy = 0; yy < eyeColorArray.size(); ++yy)
			{
				for (uint32_t xx = 0; xx < eyeColorArray[yy].size(); ++xx)
				{
					uint8_t alpha = eyeColorArray[xx][yy].m_colorA;
					if (alpha > 0)
					{
						SelectObject(hdcMem, g_hbmBall);
						GetObject(g_hbmBall, sizeof(bm), &bm);

						::SetStretchBltMode(hdcMem, COLORONCOLOR);
						BLENDFUNCTION bf;
						bf.BlendOp = AC_SRC_OVER;
						bf.BlendFlags = 0;
						bf.SourceConstantAlpha = alpha;
						bf.AlphaFormat = 0;
						::AlphaBlend(hdc, xx*16, 240-yy*16, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, bf);
					}
				}
			}
		}

		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		KillTimer(hWnd, TIMER1_IDT);
		PPh::Observer::Instance()->StopSimulation();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
