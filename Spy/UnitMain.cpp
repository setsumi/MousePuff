//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;

HWND prevWnd = NULL;

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
	if (wnd && wnd != Handle && wnd != prevWnd) {
		prevWnd = wnd;
		memoInfo->Lines->Clear();
		memoInfo->Lines->Add(L">>>>>>>>>>( Window Title & Class )<<<<<<<<<<<");
		memoInfo->Lines->Add(GetWindowTitlePlus(wnd).Trim());
		memoInfo->Lines->Add(GetWindowClassPlus(wnd).Trim());
	}
	TimerPoll->Enabled = true;
}
//---------------------------------------------------------------------------





