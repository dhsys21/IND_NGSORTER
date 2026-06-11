//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("ModGripper.cpp", gripper); /* TDataModule: File Type */
USEFORM("FormServoAlarmList.cpp", ServoAlarmListForm);
USEFORM("FormMain.cpp", MainForm);
USEFORM("FormTeaching.cpp", teachForm);
USEFORM("Modplc.cpp", plc); /* TDataModule: File Type */
USEFORM("Modmes.cpp", mes); /* TDataModule: File Type */
USEFORM("ModRobostar.cpp", robostar); /* TDataModule: File Type */
USEFORM("FormAlarm_TrayInfo.cpp", trayinfoForm);
USEFORM("FormAlarm_LoadFactor.cpp", loadfactor_AlarmForm);
USEFORM("FormConfig.cpp", ConfigForm);
USEFORM("FormBase.cpp", BaseForm);
USEFORM("Barcode_comm.cpp", Barcode); /* TDataModule: File Type */
USEFORM("FormAlarm.cpp", AlarmForm);
USEFORM("FormError_limit.cpp", ErrorForm_limit);
USEFORM("FormError_insert.cpp", ErrorForm_insert);
USEFORM("FormLoadFactor.cpp", loadfactorForm);
USEFORM("FormError_mes.cpp", ErrorForm_mes);
USEFORM("FormError.cpp", ErrorForm);
USEFORM("FormDoor.cpp", doorForm);
USEFORM("FormError_eject.cpp", ErrorForm_eject);
USEFORM("FormError_bcr.cpp", ErrorForm_bcr);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	HANDLE Mutex;
	try
	{
		const char ProgMutex[] = "NGSORTER";

		if((Mutex = OpenMutexA(MUTEX_ALL_ACCESS, false, ProgMutex)) == NULL)
			Mutex = CreateMutexA(NULL, true, ProgMutex);
		else
		{
			ShowMessage("The program is already running.");
			return 0;
		}

		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TBaseForm), &BaseForm);
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(Tmes), &mes);
		Application->CreateForm(__classid(Trobostar), &robostar);
		Application->CreateForm(__classid(Tgripper), &gripper);
		Application->CreateForm(__classid(TErrorForm_eject), &ErrorForm_eject);
		Application->CreateForm(__classid(TteachForm), &teachForm);
		Application->CreateForm(__classid(TErrorForm_limit), &ErrorForm_limit);
		Application->CreateForm(__classid(TErrorForm_mes), &ErrorForm_mes);
		Application->CreateForm(__classid(TAlarmForm), &AlarmForm);
		Application->CreateForm(__classid(TErrorForm_insert), &ErrorForm_insert);
		Application->CreateForm(__classid(Tplc), &plc);
		Application->CreateForm(__classid(TConfigForm), &ConfigForm);
		Application->CreateForm(__classid(TdoorForm), &doorForm);
		Application->CreateForm(__classid(TErrorForm_bcr), &ErrorForm_bcr);
		Application->CreateForm(__classid(TServoAlarmListForm), &ServoAlarmListForm);
		Application->CreateForm(__classid(Tloadfactor_AlarmForm), &loadfactor_AlarmForm);
		Application->CreateForm(__classid(TloadfactorForm), &loadfactorForm);
		Application->CreateForm(__classid(TtrayinfoForm), &trayinfoForm);
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
	ReleaseMutex(Mutex);
	return 0;
}
//---------------------------------------------------------------------------
