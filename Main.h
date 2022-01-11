//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TTimer *timerPuff;
	TRadioButton *radioBtnGlobal;
	TRadioButton *radioBtnProgram;
	TTrayIcon *trayIcon;
	TUpDown *udTimeout;
	TEdit *editTimeout;
	TLabel *Label3;
	TButton *btnSpy;
	TCheckBox *chkDebug;
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
private:	// User declarations
	void Save();
	void Load();
	UnicodeString ProgramVer();

public:		// User declarations
	__fastcall TForm1(TComponent* Owner);

	bool TargetProgram();
	void TimerReset();
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
