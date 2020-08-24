// DaphniaWinApi.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DaphniaWinApi.h"
#include "ParallelPhysics/ObserverClient.h"
#include <string>
#include <playsoundapi.h>
#include "NervousSystem/NervousSystem.h"

#pragma comment (lib, "msimg32.lib")
#pragma comment(lib,"Winmm.lib")

constexpr int EYE_PIXEL_SIZE = 32;
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

	NervousSystem::Init();
	PPh::ObserverClient::Init();
	PPh::ObserverClient::Instance()->StartSimulation();

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
	HBRUSH hb = (HBRUSH)GetStockObject(BLACK_BRUSH);
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

	constexpr int TIMER2_STATISTICS_IDT = 2;
	constexpr int TIMER2_DURATION_MS = 1000;
	static bool s_statisticsUpdate = false;

	static char *s_bmBuffer;
	static HDC s_hdcMem;
	static HBITMAP s_hbmBall;
    switch (message)
    {
	case WM_CREATE:
	{
		SetTimer(hWnd, TIMER1_IDT, TIMER1_DURATION_MS, (TIMERPROC)NULL);
		SetTimer(hWnd, TIMER2_STATISTICS_IDT, TIMER2_DURATION_MS, (TIMERPROC)NULL);

		s_hdcMem = CreateCompatibleDC(GetDC(hWnd));
		BITMAPINFO bmInfo = { sizeof(BITMAPINFOHEADER),
				1, 1, 1, 24, BI_RGB, 3, 0, 0, 0, 0 };
		s_hbmBall = CreateDIBSection(s_hdcMem, &bmInfo, DIB_RGB_COLORS, (void**)&s_bmBuffer, 0, 0);
		if (s_hbmBall == NULL) {
			DWORD lastError = GetLastError();
			return lastError;
		}
		HGDIOBJ selectedObjectOld = SelectObject(s_hdcMem, s_hbmBall);

		if (!PPh::ObserverClient::Instance()->IsSimulationRunning())
		{
			uint32_t serverVersion = PPh::ObserverClient::Instance()->GetServerProtocolVersion();
			if (serverVersion && serverVersion != PPh::CommonParams::PROTOCOL_VERSION)
			{
				std::string str = "Server protocol: " + std::to_string(serverVersion) + "\n" + "Client protocol: " +
					std::to_string(PPh::CommonParams::PROTOCOL_VERSION);
				MessageBox(hWnd, str.c_str(), "Wrong protocol", MB_OK);
			}
			else
			{
				MessageBox(hWnd, "Unknown server error", "Server error", MB_OK);
			}
			
		}
	}
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
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		bool bSet = true;
		if (WM_KEYUP == message)
		{
			bSet = false;
		}
		switch (wParam)
		{
		case VK_LEFT:
			PPh::ObserverClient::Instance()->SetIsLeft(bSet);
			break;
		case VK_RIGHT:
			PPh::ObserverClient::Instance()->SetIsRight(bSet);
			break;
		case VK_UP:
			PPh::ObserverClient::Instance()->SetIsUp(bSet);
			break;
		case VK_DOWN:
			PPh::ObserverClient::Instance()->SetIsDown(bSet);
			break;
		case VK_SPACE:
			PPh::ObserverClient::Instance()->SetIsForward(bSet);
			break;
		case VK_OEM_2:
			PPh::ObserverClient::Instance()->SetIsBackward(bSet);
			break;
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
			case TIMER2_STATISTICS_IDT:
			{
				s_statisticsUpdate = true;
			}
			break;
		}
	}
	break;
    case WM_PAINT:
    {
		PAINTSTRUCT ps;

		HDC hdc = BeginPaint(hWnd, &ps);

		static PPh::SP_EyeColorArray s_spEyeColorArrayOut;
		PPh::SP_EyeColorArray spEyeColorArrayOut = PPh::ObserverClient::Instance()->GrabTexture();
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
					uint8_t alpha = eyeColorArray[yy][xx].m_colorA; // first yy second xx see Observer::m_eyeColorArray comments
					PPh::EtherColor &color = eyeColorArray[yy][xx];
					s_bmBuffer[0] = color.m_colorB * alpha / 255;
					s_bmBuffer[1] = color.m_colorG * alpha / 255;
					s_bmBuffer[2] = color.m_colorR * alpha / 255;

					StretchBlt(hdc, xx*EYE_PIXEL_SIZE, yy*EYE_PIXEL_SIZE, EYE_PIXEL_SIZE, EYE_PIXEL_SIZE, s_hdcMem, 0, 0, 1, 1, SRCCOPY);
				}
			}
		}

		PPh::VectorInt32Math outPosition;
		uint16_t outMovingProgress;
		int16_t outLatitude, outLongitude;
		bool outEatenCrumb;
		PPh::ObserverClient::Instance()->GetStateExtParams(outPosition, outMovingProgress, outLatitude, outLongitude, outEatenCrumb);
		PPh::ObserverClient::Instance()->GrabEatenCrumbPos();
		if (outEatenCrumb)
		{
			PlaySound((char*)IDR_WAVE1, NULL, SND_RESOURCE | SND_ASYNC);
		}
		if (s_statisticsUpdate)
		{
			s_statisticsUpdate = false;

			RECT rc;
			GetClientRect(hWnd, &rc);
			rc.left = EYE_PIXEL_SIZE * PPh::GetObserverEyeSize() + EYE_PIXEL_SIZE;
			rc.top += (rc.bottom - rc.top) / 5;
			SelectObject(hdc, GetStockObject(BLACK_BRUSH));
			Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom); // cleanup screen
			SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(255, 255, 255));
			std::string strOut = "STATISTICS:";
			{
				uint32_t outQuantumOfTimePerSecond;
				uint32_t outUniverseThreadsNum;
				uint32_t outTickTimeMusAverageUniverseThreadsMin;
				uint32_t outTickTimeMusAverageUniverseThreadsMax;
				uint32_t outTickTimeMusAverageObserverThread;
				uint64_t outClientServerPerformanceRatio;
				uint64_t outServerClientPerformanceRatio;
				PPh::ObserverClient::Instance()->GetStatisticsParams(outQuantumOfTimePerSecond, outUniverseThreadsNum,
					outTickTimeMusAverageUniverseThreadsMin, outTickTimeMusAverageUniverseThreadsMax,
					outTickTimeMusAverageObserverThread, outClientServerPerformanceRatio, outServerClientPerformanceRatio);
				strOut += std::string("\nFPS (quantum of time per second): ") + std::to_string(outQuantumOfTimePerSecond);
				strOut += "\nUniverse threads count: " + std::to_string(outUniverseThreadsNum);
				if (outUniverseThreadsNum)
				{
					strOut += "\nTick time(ms). Observer thread: " + std::to_string(outTickTimeMusAverageObserverThread / 1000.0f);
					strOut += "\nTick time(ms). Fastest universe thread: " + std::to_string(outTickTimeMusAverageUniverseThreadsMin / 1000.0f);
					strOut += "\nTick time(ms). Slowest universe thread: " + std::to_string(outTickTimeMusAverageUniverseThreadsMax / 1000.0f);
				}
				strOut += "\nClient-Server performance ratio: " + std::to_string(outClientServerPerformanceRatio / 1000.0f);
				strOut += "\nServer-Client performance ratio: " + std::to_string(outServerClientPerformanceRatio / 1000.0f);
				strOut += std::string("\nPosition: (") + std::to_string(outPosition.m_posX) + ", " +std::to_string(outPosition.m_posY) + ", " +
					std::to_string(outPosition.m_posZ) + ")";
				strOut += std::string("\nLattitude: ") + std::to_string(outLatitude);
				strOut += std::string("\nLongitude: ") + std::to_string(outLongitude);
			}
			DrawText(hdc, strOut.c_str(), (int)strOut.length(), &rc, DT_LEFT);
		}

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		DeleteObject(s_hbmBall);
		DeleteDC(s_hdcMem);
		KillTimer(hWnd, TIMER1_IDT);
		PPh::ObserverClient::Instance()->StopSimulation();
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
