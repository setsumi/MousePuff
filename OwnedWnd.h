// ---------------------------------------------------------------------------

#ifndef OwnedWndH
#define OwnedWndH
// ---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

// ---------------------------------------------------------------------------
class TFormOwnedWnd : public TForm
{
__published: // IDE-managed Components
	void __fastcall FormDestroy(TObject *Sender);

private: // User declarations
public: // User declarations
	__fastcall TFormOwnedWnd(TComponent* Owner);
};

// ---------------------------------------------------------------------------
extern PACKAGE TFormOwnedWnd *FormOwnedWnd;
// ---------------------------------------------------------------------------
#endif
