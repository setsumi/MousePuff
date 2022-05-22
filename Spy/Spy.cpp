// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("UnitMain.cpp", FormMain);

// ---------------------------------------------------------------------------
int PASCAL wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "Active Window Info";
		Application->CreateForm(__classid(TFormMain), &FormMain);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
// ---------------------------------------------------------------------------
