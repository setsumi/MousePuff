// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OwnedWnd.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormOwnedWnd *FormOwnedWnd;

// ---------------------------------------------------------------------------
__fastcall TFormOwnedWnd::TFormOwnedWnd(TComponent* Owner) : TForm(Owner)
{
}

// ---------------------------------------------------------------------------
void __fastcall TFormOwnedWnd::FormDestroy(TObject *Sender)
{
	FormOwnedWnd = NULL;
}
// ---------------------------------------------------------------------------
