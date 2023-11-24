// ---------------------------------------------------------------------------
#include <vcl.h>
#include <Registry.hpp>
#pragma hdrstop

#include "Main.h"
#include "Hider/shared.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMousePuff1 *FormMousePuff1;

HINSTANCE hInstance = NULL;
HHOOK hMouseHook = NULL, hKbdHook = NULL;
HCURSOR hCurBlank = NULL;
HWND hWindow = NULL;
bool FastTermination = false;

// ---------------------------------------------------------------------------
String GetWindowClassPlus(HWND hwnd)
{
	wchar_t tbuf[2048];
	tbuf[0] = L'\0';
	GetClassName(hwnd, tbuf, 2047);
	return tbuf;
}

String GetWindowTitlePlus(HWND hwnd)
{
	wchar_t tbuf[2048];
	tbuf[0] = L'\0';
	GetWindowText(hwnd, tbuf, 2047);
	return tbuf;
}

String GetWindowExePlus(HWND hwnd)
{
	DWORD pid = 0;
	wchar_t tbuf[2048] =
	{0};
	DWORD size = 2048;
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE ph = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	QueryFullProcessImageName(ph, 0, tbuf, &size);
	CloseHandle(ph);
	return tbuf;
}

void RunExe(String cmdline)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if (CreateProcess(NULL, cmdline.w_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL,
		NULL, &si, &pi))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		throw Exception(L"RunExe(\"" + cmdline + L"\") failed");
	}
}

// ---------------------------------------------------------------------------
String TFormMousePuff1::ProgramVer()
{
	String ret;
	DWORD hnd = 0, vis;
	if (0 != (vis = GetFileVersionInfoSize(GetModuleName(NULL).w_str(), &hnd)))
	{
		BYTE *data = new BYTE[vis];
		if (GetFileVersionInfo(GetModuleName(NULL).w_str(), hnd, vis, data))
		{
			UINT ilen = 0;

			struct LANGANDCODEPAGE
			{
				WORD wLanguage;
				WORD wCodePage;
			}*lpTranslate;

			// Read first language and code page
			if (VerQueryValue(data, L"\\VarFileInfo\\Translation",
				(LPVOID*)&lpTranslate, &ilen))
			{
				String str;
				str.printf(L"\\StringFileInfo\\%04x%04x\\FileVersion",
					lpTranslate->wLanguage, lpTranslate->wCodePage);
				// Retrieve file version for language and code page
				wchar_t *pver = NULL;
				if (VerQueryValue(data, str.w_str(), (LPVOID*)&pver, &ilen))
				{
					ret = pver;
				}
			}
		}
		delete[]data;
	}
	return ret;
}

// ---------------------------------------------------------------------------
void TFormMousePuff1::TimerReset()
{
	timerPuff->Enabled = false;
	timerPuff->Interval = udTimeout->Position * 1000;
	timerPuff->Enabled = true;
}

// ---------------------------------------------------------------------------
void TFormMousePuff1::Save()
{
	memoTitle->Lines->SaveToFile(L"title.ini");
	memoClass->Lines->SaveToFile(L"class.ini");
	memoExe->Lines->SaveToFile(L"executable.ini");

	TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
	ini->WriteInteger(L"MAIN", L"Global", radioBtnGlobal->Checked ? 1 : 0);
	ini->WriteInteger(L"MAIN", L"Timeout", udTimeout->Position);
	ini->WriteInteger(L"MAIN", L"StartToTray", chkStartToTray->Checked ? 1 : 0);
	ini->WriteInteger(L"MAIN", L"WinPageIndex", pageControl1->TabIndex);
	ini->WriteInteger(L"MAIN", L"HideTrayIcon", chkHideTrayIcon->Checked ? 1 : 0);
	delete ini;
}

// ---------------------------------------------------------------------------
void TFormMousePuff1::Load()
{
	if (FileExists(L"title.ini"))
		memoTitle->Lines->LoadFromFile(L"title.ini");
	if (FileExists(L"class.ini"))
		memoClass->Lines->LoadFromFile(L"class.ini");
	if (FileExists(L"executable.ini"))
		memoExe->Lines->LoadFromFile(L"executable.ini");

	TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
	if (ini->ReadInteger(L"MAIN", L"Global", 1) == 1)
		radioBtnGlobal->Checked = true;
	else
		radioBtnProgram->Checked = true;
	udTimeout->Position = ini->ReadInteger(L"MAIN", L"Timeout", 3);
	TimerReset();
	chkStartToTray->Checked = ini->ReadInteger(L"MAIN", L"StartToTray", 0) == 1;
	pageControl1->TabIndex = ini->ReadInteger(L"MAIN", L"WinPageIndex", 0);
	chkHideTrayIcon->Checked = ini->ReadInteger(L"MAIN", L"HideTrayIcon", 0) == 1;
	delete ini;
}

// ---------------------------------------------------------------------------
bool TFormMousePuff1::TargetProgram()
{
	bool rv = false;
	HWND hw = ::GetForegroundWindow();
	String query;
	if (hw)
	{
		// compare title
		if (memoTitle->Lines->Count)
		{
			query = GetWindowTitlePlus(hw);
			for (int i = 0; i < memoTitle->Lines->Count; i++)
			{
				if (query.Pos(memoTitle->Lines->Strings[i]))
				{
					rv = true;
					goto target;
				}
			}
		}
		// compare class
		if (memoClass->Lines->Count)
		{
			query = GetWindowClassPlus(hw);
			for (int i = 0; i < memoClass->Lines->Count; i++)
			{
				if (query == memoClass->Lines->Strings[i])
				{
					rv = true;
					goto target;
				}
			}
		}
		// compare exe
		if (memoExe->Lines->Count)
		{
			query = GetWindowExePlus(hw);
			for (int i = 0; i < memoExe->Lines->Count; i++)
			{
				if (query.Pos(memoExe->Lines->Strings[i]))
				{
					rv = true;
					goto target;
				}
			}
		}
	}
target:
	return rv;
}

// ---------------------------------------------------------------------------
void MyProcShowCursor(bool show)
{
	HWND hHider = FindWindow(LEASED_WNDCLASS, LEASED_WNDTITLE);
	if (hHider)
	{
		HWND hTarget = GetForegroundWindow();
		PostMessage(hHider, show ? WMM_SHOWPOINTER : WMM_HIDEPOINTER, (WPARAM)hTarget, 0);
	}
}

const DWORD cursorID[] =
{OCR_APPSTARTING, OCR_NORMAL, OCR_CROSS, OCR_HAND, OCR_IBEAM, OCR_NO, OCR_SIZEALL,
	OCR_SIZENESW, OCR_SIZENS, OCR_SIZENWSE, OCR_SIZEWE, OCR_UP, OCR_WAIT,
	32651 /* OCR_HELP */ };

void MyShowCursor(bool show, bool force = false)
{
	static bool visible = true;

	if (show != visible || force)
	{
		visible = show;

		// debug sound
		if (FormMousePuff1->chkDebug->Checked)
		{
			MessageBeep(show ? 0 : MB_ICONASTERISK);
		}

		if (show)
		{
			SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
			MyProcShowCursor(show);
		}
		else
		{
			for (size_t i = 0; i < (sizeof(cursorID) / sizeof(*cursorID)); i++)
			{
				SetSystemCursor(CopyCursor(hCurBlank), cursorID[i]);
			}
			MyProcShowCursor(show);
		}
	}
}

// ---------------------------------------------------------------------------
LRESULT CALLBACK LLHookMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) // allowed to process message
	{
		FormMousePuff1->TimerReset();
		MyShowCursor(true);
	}
	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
LRESULT CALLBACK LLHookKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) // allowed to process message now
	{
		if (!FormMousePuff1->radioBtnGlobal->Checked) // ignore keyboard in global mode
		{
			if (!FormMousePuff1->timerKb->Enabled) // suppress fast keyboard processind
					FormMousePuff1->timerKb->Enabled = true;
		}
	}
	return CallNextHookEx(hKbdHook, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
void MyUnhook()
{
	// release hooks
	if (hMouseHook)
	{
		UnhookWindowsHookEx(hMouseHook);
		hMouseHook = NULL;
	}
	if (hKbdHook)
	{
		UnhookWindowsHookEx(hKbdHook);
		hKbdHook = NULL;
	}
}

// ---------------------------------------------------------------------------
void MyHook()
{
	MyUnhook();
	// set up hooks
	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)LLHookMouseProc,
		GetModuleHandle(NULL), NULL);
	if (!hMouseHook)
		throw Exception(L"SetWindowsHookEx(WH_MOUSE_LL) failed");
	hKbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LLHookKeyboardProc,
		GetModuleHandle(NULL), NULL);
	if (!hKbdHook)
		throw Exception(L"SetWindowsHookEx(WH_KEYBOARD_LL) failed");
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::timerKbTimer(TObject *Sender)
{
	timerKb->Enabled = false;
	TimerReset();
	MyShowCursor(!TargetProgram());
}

// ---------------------------------------------------------------------------
static BOOL CALLBACK enumWindowCallback(HWND hWnd, LPARAM lparam)
{
	// Activate another instance if already running
	if (hWnd != hWindow && SameStr(GetWindowClassPlus(hWnd), L"TFormMousePuff1"))
	{
		if (!IsWindowVisible(hWnd))
			ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd);
		FastTermination = true;
		exit(0); // Terminate myself
	}
	return TRUE;
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::FormCreate(TObject *Sender)
{
	hInstance = (HINSTANCE)GetWindowLong(Handle, GWL_HINSTANCE);
	hWindow = Handle;

	// Check if another instance is already running
	EnumWindows(enumWindowCallback, NULL);

#ifdef _DEBUG
	chkDebug->Visible = true;
#endif

	// set up hooks
	MyHook();
	// load config
	Load();
	// read command line (overrides config)
	bool noicon = false;
	for (int i = 1; i <= ParamCount(); i++)
	{
		if (LowerCase(ParamStr(i)) == "-noicon")
		{
			noicon = true;
			break;
		}
	}
	if (!noicon) // if no parameter then decide by config
	{
		trayIcon->Visible = !chkHideTrayIcon->Checked;
	}

	// load blank cursor from resource
	hCurBlank = LoadCursor(hInstance, L"Cursor_Blank");
	if (hCurBlank == NULL)
		throw Exception(L"INIT: LoadCursor() failed");

	// run custom app based pointers hider
	// as NON-elevated to avoid breaking global hook based applications
	RunExe(L"explorer.exe Hider.exe");
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::FormDestroy(TObject *Sender)
{
	MyUnhook();
	Save();
	MyShowCursor(true, true);
	DestroyCursor(hCurBlank);

	if (!FastTermination)
	{
		HWND hHider = FindWindow(LEASED_WNDCLASS, LEASED_WNDTITLE);
		if (hHider)
			PostMessage(hHider, WM_CLOSE, 0, 0);
	}
}

// ---------------------------------------------------------------------------
__fastcall TFormMousePuff1::TFormMousePuff1(TComponent* Owner) : TForm(Owner)
{
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::timerPuffTimer(TObject *Sender)
{
	timerPuff->Enabled = false;
	if (radioBtnGlobal->Checked || TargetProgram())
	{
		MyShowCursor(false);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::trayIconClick(TObject *Sender)
{
	if (this->Visible)
	{
		this->Hide();
	}
	else
	{
		this->Show();
		::SetForegroundWindow(Handle);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::btnSpyClick(TObject *Sender)
{
	RunExe(L"Spy.exe");
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::radioBtnGlobalClick(TObject *Sender)
{
	TimerReset();
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::btnHideClick(TObject *Sender)
{
	Hide();
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::btnHelpClick(TObject *Sender)
{
	MessageBox(Handle, (Caption + L"\nversion " + ProgramVer() +
		L"\n\nCommand line parameters:\n-tray\tstart minimized to tray" +
		L"\n-noicon\tdo not display tray icon" +
		L"\n\nNote 1: If both parameters are used, run program again to show its window." +
		L"\nNote 2: Command line parameters override UI options." +
		L"\nNote 3: Window Title and Executable Path can be partial string.").w_str(),
		L"Information", MB_OK | MB_ICONINFORMATION);
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::btnExitClick(TObject *Sender)
{
	Close();
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::FormKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift)
{
	if (Key == VK_ESCAPE)
	{
		btnHideClick(this);
	}
	else if (Key == 'E' && Shift.Contains(ssCtrl))
	{
		chkEnabled->Checked = !chkEnabled->Checked;
	}
	else if (Key == VK_F1)
	{
		btnHelpClick(NULL);
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::chkEnabledClick(TObject *Sender)
{
	timerPuff->Enabled = false;
	if (!chkEnabled->Checked)
	{
		MyUnhook();
		MyShowCursor(true, true);
	}
	else
	{
		MyHook();
		TimerReset();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFormMousePuff1::WndProc(TMessage& Message)
{
	if (Message.Msg == WM_SHOWWINDOW && Message.WParam == TRUE)
	{
		Show(); // repair VCL on raw ShowWindow() call
	}
	TForm::WndProc(Message);
}
// ---------------------------------------------------------------------------
