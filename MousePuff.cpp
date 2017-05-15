//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

#include <Registry.hpp>

//---------------------------------------------------------------------------
USEFORM("Main.cpp", Form1);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TForm1), &Form1);

		// read config
		TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
		Application->ShowMainForm = ini->ReadInteger(L"MAIN", L"StartToTray", 0) == 0;
		delete ini;
		// read command line (overrides config)
		for (int i=1; i<=ParamCount(); i++) {
			if (LowerCase(ParamStr(i)) == "-tray") {
				Application->ShowMainForm = false;
				break;
			}
		}

		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
