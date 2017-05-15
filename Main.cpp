//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <Registry.hpp>

#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

HINSTANCE hInstance = NULL;
HHOOK hMouseHook, hKbdHook;
HCURSOR hCurBlank = NULL;
bool CurIsVisible = true; // assuming current state of cursor (set in MyShowCursor())
int Emergency = 0; // force show cursor after some amount of mouse movement (reset in MyShowCursor())
int EmergencyMax = 300;
bool KeybTriggered = false; // flag when keyboard activated re-hide
bool MouseTriggered = false; // flag when mouse button activated hide

UnicodeString GetWindowClassPlus(HWND hwnd)
{
	wchar_t *tbuf = new wchar_t[2048]; tbuf[0] = L'\0';
	GetClassName(hwnd, tbuf, 2047);
	UnicodeString ret(tbuf);
	delete []tbuf;
	return ret;
}
UnicodeString GetWindowTitlePlus(HWND hwnd)
{
	wchar_t *tbuf = new wchar_t[2048]; tbuf[0] = L'\0';
	GetWindowText(hwnd, tbuf, 2047);
	UnicodeString ret(tbuf);
	delete []tbuf;
	return ret;
}
void RunExe(UnicodeString cmdline) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(NULL, cmdline.t_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
		throw Exception(L"RunExe(\"" + cmdline + L"\") failed");
}
//---------------------------------------------------------------------------
UnicodeString TForm1::ProgramVer()
{
	UnicodeString ret;
	DWORD hnd=0, vis=0;
	if(0 != (vis = GetFileVersionInfoSize(GetModuleName(NULL).w_str(), &hnd))) {
		BYTE *data = new BYTE[vis];
		if(GetFileVersionInfo(GetModuleName(NULL).w_str(), hnd, vis, data)) {
			UINT ilen = 0;
			struct LANGANDCODEPAGE {
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate;
			// Read first language and code page
			if(VerQueryValue(data, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &ilen)) {
				UnicodeString str;
				str.printf(L"\\StringFileInfo\\%04x%04x\\FileVersion", lpTranslate->wLanguage, lpTranslate->wCodePage);
				// Retrieve file version for language and code page
				wchar_t *pver = NULL;
				if(VerQueryValue(data, str.w_str(), (LPVOID*)&pver, &ilen)) {
          ret = pver;
        }
			}
		}
		delete []data;
	}
	return ret;
}
//---------------------------------------------------------------------------
void TForm1::TimerReset() {
	timerPuff->Enabled = false;
	timerPuff->Interval = udTimeout->Position * 1000;
	timerPuff->Enabled = true;
}
//---------------------------------------------------------------------------
void TForm1::Save() {
	memoTitle->Lines->SaveToFile(L"title.ini");
	memoClass->Lines->SaveToFile(L"class.ini");

	TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
	ini->WriteInteger(L"MAIN", L"Global", radioBtnGlobal->Checked?1:0);
	ini->WriteInteger(L"MAIN", L"Timeout", udTimeout->Position);
	ini->WriteInteger(L"MAIN", L"StartToTray", chkStartToTray->Checked?1:0);
	ini->WriteInteger(L"MAIN", L"EmergencyMax", EmergencyMax);
	delete ini;
}
//---------------------------------------------------------------------------
void TForm1::Load() {
	memoTitle->Lines->LoadFromFile(L"title.ini");
	memoClass->Lines->LoadFromFile(L"class.ini");

	TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
	if (ini->ReadInteger(L"MAIN", L"Global", 1) == 1)
		radioBtnGlobal->Checked = true;
	else
		radioBtnProgram->Checked = true;
	udTimeout->Position = ini->ReadInteger(L"MAIN", L"Timeout", 3);
	TimerReset();
	chkStartToTray->Checked = ini->ReadInteger(L"MAIN", L"StartToTray", 0) == 1;
	EmergencyMax = ini->ReadInteger(L"MAIN", L"EmergencyMax", 300);
	delete ini;
}
//---------------------------------------------------------------------------
bool TForm1::TargetProgram() {
	bool rv = false;
	HWND hw = ::GetForegroundWindow();
	if (hw) {
		// compare title
		UnicodeString wtitle(GetWindowTitlePlus(hw));
		for (int i=0; i < memoTitle->Lines->Count; i++) {
			if (wtitle.Pos(memoTitle->Lines->Strings[i])) {
				rv = true;
				break;
			}
		}
		// compare class
		if (!rv) {
			UnicodeString wclass(GetWindowClassPlus(hw));
			for (int i=0; i < memoClass->Lines->Count; i++) {
				if (wclass == memoClass->Lines->Strings[i]) {
					rv = true;
					break;
				}
			}
		}
	}
	return rv;
}
//---------------------------------------------------------------------------
DWORD cursorID[] = {OCR_APPSTARTING, OCR_NORMAL, OCR_CROSS, OCR_HAND, OCR_IBEAM,
										OCR_NO, OCR_SIZEALL, OCR_SIZENESW, OCR_SIZENS, OCR_SIZENWSE,
										OCR_SIZEWE, OCR_UP, OCR_WAIT, 32651/*OCR_HELP*/};
void MyShowCursor(bool show)
{
	// debug
	if (Form1->chkDebug->Checked) {
		MessageBeep(show? 0:MB_ICONASTERISK);
	}

	CurIsVisible = show;
	Emergency = 0;
	if (show) {
		SystemParametersInfo(SPI_SETCURSORS, 0, NULL, 0);
	} else {
		for (size_t i=0; i < (sizeof(cursorID)/sizeof(*cursorID)); i++) {
			SetSystemCursor(CopyCursor(hCurBlank), cursorID[i]);
		}
	}
}
//---------------------------------------------------------------------------
LRESULT CALLBACK LLHookMouseProc(int nCode,	WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) { // allowed to process message
		if (wParam == WM_MOUSEMOVE) {  // show cursor on mouse move
			Form1->timerPuff->Enabled = false;
			if (!CurIsVisible || KeybTriggered || MouseTriggered) {
      	KeybTriggered = MouseTriggered = false;
				MyShowCursor(true);
			} else if (++Emergency > EmergencyMax) {
				MyShowCursor(true);
			}
			Form1->TimerReset();
		} // start hide sequence on mouse button (for switching apps by only clicking without moving)
		else if (wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP) {
			MouseTriggered = true;
			CurIsVisible = true;
			Form1->TimerReset();
		}
	}
	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}
//---------------------------------------------------------------------------
LRESULT CALLBACK LLHookKeyboardProc(int nCode,	WPARAM wParam, LPARAM lParam)
{
	if (!(nCode < 0)) { // allowed to process message now
		if (nCode == HC_ACTION) {
			// start re-hide sequence on keyboard (in case some operation make cursor reappear)
			KeybTriggered = true;
			CurIsVisible = true;
			Form1->TimerReset();
		}
	}
	return CallNextHookEx(hKbdHook, nCode, wParam, lParam);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
	hInstance = (HINSTANCE)GetWindowLong(Handle, GWL_HINSTANCE);
	// set up hooks
	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)LLHookMouseProc, GetModuleHandle(NULL), NULL);
	if (!hMouseHook) throw Exception(L"INIT: SetWindowsHookEx(WH_MOUSE_LL) failed");
	hKbdHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LLHookKeyboardProc, GetModuleHandle(NULL), NULL);
	if (!hKbdHook) throw Exception(L"INIT: SetWindowsHookEx(WH_KEYBOARD_LL) failed");

	// load config
	Load();
	// load blank cursor fromk resource
	hCurBlank = LoadCursor(hInstance, L"Cursor_Blank");
	if (hCurBlank == NULL) throw Exception(L"INIT: LoadCursor() failed");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormDestroy(TObject *Sender)
{
	// release hooks
	UnhookWindowsHookEx(hMouseHook);
	UnhookWindowsHookEx(hKbdHook);

	Save();
	MyShowCursor(true);
	DestroyCursor(hCurBlank);
}
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::timerPuffTimer(TObject *Sender)
{
	timerPuff->Enabled = false;
	if (radioBtnGlobal->Checked || TargetProgram()) {
		if (CurIsVisible) {
			MyShowCursor(false);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::trayIconClick(TObject *Sender)
{
	if (Visible) {
		Hide();
  } else {
		Show();
		SetForegroundWindow(Handle);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnSpyClick(TObject *Sender)
{
	RunExe(L"Spy.exe");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::radioBtnGlobalClick(TObject *Sender)
{
	TimerReset();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnHideClick(TObject *Sender)
{
	Hide();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnHelpClick(TObject *Sender)
{
	ShowMessage(Caption + L"\nversion " + ProgramVer() +
			L"\n\nCommand line parameters:\n-tray      start minimized to tray");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnExitClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

