// ---------------------------------------------------------------------------

#ifndef MainH
#define MainH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

// ---------------------------------------------------------------------------
class TFormMousePuff1 : public TForm
{
__published: // IDE-managed Components
	TTimer *timerPuff;
	TRadioButton *radioBtnGlobal;
	TRadioButton *radioBtnProgram;
	TTrayIcon *trayIcon;
	TUpDown *udTimeout;
	TEdit *editTimeout;
	TLabel *Label3;
	TButton *btnSpy;
	TButton *btnHide;
	TButton *btnHelp;
	TButton *btnExit;
	TCheckBox *chkStartToTray;
	TCheckBox *chkEnabled;
	TPageControl *pageControl1;
	TTabSheet *tabsheetTitle;
	TTabSheet *tabsheetClass;
	TMemo *memoTitle;
	TMemo *memoClass;
	TTabSheet *tabsheetExe;
	TMemo *memoExe;
	TTimer *timerKb;
	TLabel *Label1;
	TCheckBox *chkHideTrayIcon;
	TTimer *timerResetHook;

	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall timerPuffTimer(TObject *Sender);
	void __fastcall trayIconClick(TObject *Sender);
	void __fastcall btnSpyClick(TObject *Sender);
	void __fastcall radioBtnGlobalClick(TObject *Sender);
	void __fastcall btnHideClick(TObject *Sender);
	void __fastcall btnHelpClick(TObject *Sender);
	void __fastcall btnExitClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall chkEnabledClick(TObject *Sender);
	void __fastcall timerKbTimer(TObject *Sender);
	void __fastcall editTimeoutChange(TObject *Sender);
	void __fastcall chkHideTrayIconClick(TObject *Sender);
	void __fastcall timerResetHookTimer(TObject *Sender);

protected:
	void __fastcall WndProc(TMessage& Message);

private: // User declarations
	void Save();
	void Load();
	UnicodeString ProgramVer();

public: // User declarations
	__fastcall TFormMousePuff1(TComponent* Owner);

	bool TargetProgram();
	void TimerReset();
	void HookReset();
};

// ---------------------------------------------------------------------------
extern PACKAGE TFormMousePuff1 *FormMousePuff1;
// ---------------------------------------------------------------------------
#endif
