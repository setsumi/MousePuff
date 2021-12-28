//---------------------------------------------------------------------------

#include <vcl.h>
#include <psapi.h>
#include <winternl.h>
#pragma hdrstop

#include "UnitMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;

HWND prevWnd = NULL;

BOOL sm_EnableTokenPrivilege(LPCTSTR pszPrivilege)
{
	HANDLE hToken        = 0;
	TOKEN_PRIVILEGES tkp = {0};

	// Get a token for this process.

	if (!OpenProcessToken(GetCurrentProcess(),
						  TOKEN_ADJUST_PRIVILEGES |
						  TOKEN_QUERY, &hToken))
	{
		return FALSE;
	}

	// Get the LUID for the privilege.

	if(LookupPrivilegeValue(NULL, pszPrivilege,
							&tkp.Privileges[0].Luid))
	{
		tkp.PrivilegeCount = 1;  // one privilege to set

		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		// Set the privilege for this process.

		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
							  (PTOKEN_PRIVILEGES)NULL, 0);

		if (GetLastError() != ERROR_SUCCESS)
		   return FALSE;

		return TRUE;
	}

	return FALSE;
}

void CopyToClipboard(UnicodeString text)
{
	// copy to clipboard
	HGLOBAL hGlobal = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, (text.Length()+1) * sizeof(wchar_t));
	if(hGlobal) {
		void *data = GlobalLock(hGlobal);
		if(data) {
			memcpy(data, text.w_str(), (text.Length()+1) * sizeof(wchar_t));
			GlobalUnlock(hGlobal);
			if(OpenClipboard(NULL)) {
				EmptyClipboard();
				SetClipboardData(CF_UNICODETEXT, hGlobal);
				CloseClipboard();
			}else
				data = 0;
		}
		if(!data) {
			GlobalFree(hGlobal);
		}
	}
}
//---------------------------------------------------------------------------
UnicodeString GetWindowClassPlus(HWND hwnd)
{
	wchar_t *tbuf = new wchar_t[2048]; tbuf[0] = L'\0';
	GetClassName(hwnd, tbuf, 2047);
	UnicodeString ret(tbuf);
	delete []tbuf;
	return ret;
}
//---------------------------------------------------------------------------
UnicodeString GetWindowTextPlus(HWND hwnd)
{
	UnicodeString ret;
	LRESULT textlen = SendMessage(hwnd, WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0);
	if (textlen) {
		wchar_t *tbuf = new wchar_t[textlen + 1];
		SendMessage(hwnd, WM_GETTEXT, (WPARAM)(textlen + 1), (LPARAM)tbuf);
		ret = tbuf;
		delete []tbuf;
	}
	return ret;
}
//---------------------------------------------------------------------------
UnicodeString GetWindowTitlePlus(HWND hwnd)
{
	wchar_t *tbuf = new wchar_t[2048]; tbuf[0] = L'\0';
	GetWindowText(hwnd, tbuf, 2047);
	UnicodeString ret(tbuf);
	delete []tbuf;
	return ret;
}
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
	: TForm(Owner)
{
	sm_EnableTokenPrivilege(SE_DEBUG_NAME);
}
//---------------------------------------------------------------------------
__fastcall TFormMain::~TFormMain()
{
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerPollTimer(TObject *Sender)
{
	TimerPoll->Enabled = false;
	HWND wnd = ::GetForegroundWindow();
	if (wnd == Handle) { // redetect on activate self and then go back
		prevWnd = NULL;
	}
	if (wnd && wnd != Handle && wnd != prevWnd) {
		prevWnd = wnd;
		memoInfo->Lines->Clear();
		memoInfo->Lines->Add(L">>>>>>>>>> Window Title | Class | Image file | Dimensions <<<<<<<<");

		memoInfo->Lines->Add(GetWindowTitlePlus(wnd).Trim());

		memoInfo->Lines->Add(GetWindowClassPlus(wnd).Trim());

		DWORD pid;
		GetWindowThreadProcessId(wnd, &pid);
		HANDLE ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		wchar_t name[MAX_PATH];
		DWORD size = MAX_PATH;
//		GetProcessImageFileName(ph, name, MAX_PATH);
//		memoInfo->Lines->Add(name);
		QueryFullProcessImageName(ph, 0, name, &size);
		CloseHandle(ph);
		memoInfo->Lines->Add(name);

		RECT rect = {0};
		if (S_OK == DwmGetWindowAttribute(wnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT))) {
			UnicodeString str;
			str.sprintf(L"%d, %d (%d, %d)", rect.left, rect.top,
				rect.right - rect.left, rect.bottom - rect.top);
			memoInfo->Lines->Add(str);
		}
	}
	TimerPoll->Enabled = true;
}
//---------------------------------------------------------------------------





