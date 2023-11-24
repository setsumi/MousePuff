// ---------------------------------------------------------------------
#include <vcl.h>
#include <tchar.h>
#pragma hdrstop

#include "shared.h"

HWND hLeasedWnd = NULL;

// ---------------------------------------------------------------------------
HWND SetWindowOwner(HWND hwnd, HWND hwndOwner)
{
	if ((GetWindowLong(hwnd, GWL_STYLE) & WS_CHILD) || (GetWindowLong(hwndOwner,
		GWL_STYLE) & WS_CHILD))
		return NULL;
	return (HWND)SetWindowLongPtr(hwnd, GWLP_HWNDPARENT, (LONG_PTR)hwndOwner);
}

// ---------------------------------------------------------------------
LRESULT CALLBACK LeasedWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WMM_SHOWPOINTER:
		if (SetWindowOwner(hLeasedWnd, (HWND)wParam))
			while (ShowCursor(TRUE) <= -1);
		return 0;
	case WMM_HIDEPOINTER:
		if (SetWindowOwner(hLeasedWnd, (HWND)wParam))
			while (ShowCursor(FALSE) >= 0);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg =
	{0};
	BOOL bRet;

	try
	{
		// register window class
		WNDCLASSEX wc =
		{sizeof(WNDCLASSEX), CS_DBLCLKS, LeasedWindowProc, 0L, 0L, hInstance, NULL, NULL,
			NULL, NULL, LEASED_WNDCLASS, NULL};
		if (!RegisterClassEx(&wc))
		{
			throw Exception(L"RegisterClassEx() failed");
		}

		// create window
		hLeasedWnd = CreateWindow(LEASED_WNDCLASS, LEASED_WNDTITLE, WS_OVERLAPPEDWINDOW,
			0, 0, 320, 200, NULL, NULL, hInstance, NULL);
		if (!hLeasedWnd)
		{
			throw Exception(L"CreateWindow() failed");
		}

		// show window
		// ShowWindow(hLeasedWnd, SW_SHOW);
		// UpdateWindow(hLeasedWnd);

		// start message loop
		while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
		{
			if (bRet == -1)
			{
				throw Exception(L"GetMessage() failed");
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	catch (Exception &e)
	{
		MessageBox(NULL, e.Message.c_str(), LEASED_WNDTITLE, MB_OK | MB_ICONERROR);
	}

	// return the exit code to the system
	return msg.wParam;
}
// ---------------------------------------------------------------------
