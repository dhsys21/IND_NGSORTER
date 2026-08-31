//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "ModMes_Gateway.h"
#include "ModMes_OPCUA.h"
#include "FormAlarm_fms.h"
//---------------------------------------------------------------------------
USEFORM("Modmes.cpp", mes); /* TDataModule: File Type */
USEFORM("ModGripper.cpp", gripper); /* TDataModule: File Type */
USEFORM("ModMes_OPCUA.cpp", MesOpc); /* TDataModule: File Type */
USEFORM("ModMes_Gateway.cpp", Mod_Fms); /* TDataModule: File Type */
USEFORM("ModPLC_Bin.cpp", PlcBin); /* TDataModule: File Type */
USEFORM("SmokeDetector_comm.cpp", SmokeDetector); /* TDataModule: File Type */
USEFORM("Mod_SRX100W.cpp", Mod_Bcr); /* TDataModule: File Type */
USEFORM("ModRobostar.cpp", robostar); /* TDataModule: File Type */
USEFORM("FormTeaching.cpp", teachForm);
USEFORM("FormBase.cpp", BaseForm);
USEFORM("FormAlarm_TrayInfo.cpp", trayinfoForm);
USEFORM("FormDoor.cpp", doorForm);
USEFORM("FormConfig.cpp", ConfigForm);
USEFORM("FormAlarm_LoadFactor.cpp", loadfactor_AlarmForm);
USEFORM("Barcode_comm.cpp", Barcode); /* TDataModule: File Type */
USEFORM("FormAlarm.cpp", AlarmForm);
USEFORM("FormError.cpp", ErrorForm);
USEFORM("FormLoadFactor.cpp", loadfactorForm);
USEFORM("FormInterface.cpp", InterfaceForm);
USEFORM("FormServoAlarmList.cpp", ServoAlarmListForm);
USEFORM("FormMain.cpp", MainForm);
//* DRY RUN : Inspection-only modal form and dedicated timer/state machine.
USEFORM("FormDryRun.cpp", DryRunForm);
USEFORM("FormError_mes.cpp", ErrorForm_mes);
USEFORM("FormError_eject.cpp", ErrorForm_eject);
USEFORM("FormError_bcr.cpp", ErrorForm_bcr);
USEFORM("FormError_limit.cpp", ErrorForm_limit);
USEFORM("FormError_insert.cpp", ErrorForm_insert);
USEFORM("FormAlarm_fms.cpp", AlarmForm_fms);
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
		Application->CreateForm(__classid(TMod_Fms), &Mod_Fms);
		Application->CreateForm(__classid(TMesOpc), &MesOpc);
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
		Application->CreateForm(__classid(TConfigForm), &ConfigForm);
		Application->CreateForm(__classid(TdoorForm), &doorForm);
		Application->CreateForm(__classid(TErrorForm_bcr), &ErrorForm_bcr);
		Application->CreateForm(__classid(TServoAlarmListForm), &ServoAlarmListForm);
		Application->CreateForm(__classid(Tloadfactor_AlarmForm), &loadfactor_AlarmForm);
		Application->CreateForm(__classid(TloadfactorForm), &loadfactorForm);
		Application->CreateForm(__classid(TtrayinfoForm), &trayinfoForm);
		Application->CreateForm(__classid(TPlcBin), &PlcBin);
		//* DRY RUN : Created after robot/gripper/PLC modules are available.
		Application->CreateForm(__classid(TDryRunForm), &DryRunForm);
		Application->CreateForm(__classid(TSmokeDetector), &SmokeDetector);
		Application->CreateForm(__classid(TInterfaceForm), &InterfaceForm);
		Application->CreateForm(__classid(TAlarmForm_fms), &AlarmForm_fms);
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
