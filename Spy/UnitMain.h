// ---------------------------------------------------------------------------

#ifndef UnitMainH
#define UnitMainH
// ---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>

// ---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published: // IDE-managed Components
	TTimer *TimerPoll;
	TMemo *memoInfo;
	TStatusBar *StatusBar1;

	void __fastcall TimerPollTimer(TObject *Sender);

public: // User declarations

	// User declarations
	__fastcall TFormMain(TComponent* Owner);
	__fastcall ~TFormMain();
};

// ---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
// ---------------------------------------------------------------------------
#endif
