//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tmes *mes;

DWORD WINAPI TxThread(LPVOID parm_data);
//---------------------------------------------------------------------------
__fastcall Tmes::Tmes(TComponent* Owner)
	: TDataModule(Owner)
{
	savePath = ExtractFilePath(Application->ExeName);
	bConnect = false;
	mesMsg = "";
	pcName = "H1DIF01A";
	systemByte = 1000;
}
//---------------------------------------------------------------------------
void __fastcall Tmes::DataModuleCreate(TObject *Sender)
{
	this->ServerSocket->Active = true;
}
//---------------------------------------------------------------------------
// ims 접속시 스레드 생성
void __fastcall Tmes::ServerSocketClientConnect(TObject *Sender,
	  TCustomWinSocket *Socket)
{

	tx_data = new Tx_Thread_Data;
	tx_data->q = &Tx_queue;
	tx_data->tx = Socket;
	ThreadFlag = true;

	tx_thread = CreateThread(NULL, 0, TxThread, (LPVOID)tx_data, CREATE_SUSPENDED, NULL);
	SetThreadPriority(tx_thread, THREAD_PRIORITY_BELOW_NORMAL); // 쓰레드 우선순위
	ResumeThread(tx_thread);

	CloseHandle(tx_thread);
	bConnect = true;
}
//---------------------------------------------------------------------------
void __fastcall Tmes::ServerSocketClientError(TObject *Sender,
	  TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
	ErrorCode = 0;
	Socket->Close();
}
//---------------------------------------------------------------------------
// ims 접속 해제시 스레드 소멸
void __fastcall Tmes::ServerSocketClientDisconnect(TObject *Sender,
	  TCustomWinSocket *Socket)
{
	if(ThreadFlag){
		ThreadFlag = false;

		Sleep(1000);

		delete tx_data;

	}
	bConnect = false;

}
//---------------------------------------------------------------------------
void __fastcall Tmes::DataModuleDestroy(TObject *Sender)
{
	ServerSocket->Close();
}
//---------------------------------------------------------------------------
void __fastcall Tmes::WritemesLog(AnsiString flow, AnsiString msg)
{
    AnsiString str, dir;
	int file_handle;

	dir = (AnsiString)SOCK_LOG + Now().FormatString("yymmdd") + "\\";
	if(!DirectoryExists(dir))MkDir((AnsiString)dir);

	str = dir + "MES_" + Now().FormatString("yymmdd-hh") + ".log";

	if(FileExists(str))
		file_handle = FileOpen(str, fmOpenWrite);
	else{
		file_handle = FileCreate(str);
	}

	FileSeek(file_handle, 0, 2);

	str = Now().FormatString("yyyy-mm-dd hh:nn:ss ") + flow + "\t" + msg +  "\r\n";
	FileWrite(file_handle, str.c_str(), str.Length());
	FileClose(file_handle);
}


// 송신 스레드
DWORD WINAPI TxThread(LPVOID parm_data)
{
	Tx_Thread_Data *parm = (Tx_Thread_Data*)parm_data;
	AnsiString tx;
	int a = 0;
	while(mes->ThreadFlag){
		Sleep(500);
		if(parm->q->empty() == false){	// 데이터가 있으면 전송
			tx = parm->q->front().data();
			parm->tx->SendText(tx);
			parm->q->pop();
			mes->WritemesLog("PC - MES", tx);
		}
	}
	delete parm;
	return 1;
}
 //---------------------------------------------------------------------------
void __fastcall Tmes::ServerSocketClientRead(TObject *Sender,
	  TCustomWinSocket *Socket)
{
	AnsiString queue_msg;
	mesMsg = mesMsg + Socket->ReceiveText();
	int pos = mesMsg.Pos("");

	while(pos > 0){
    	queue_msg = mesMsg.SubString(1, pos);
		Rx_queue.push(queue_msg.c_str());
		mesMsg.Delete(1, pos);
		pos = mesMsg.Pos("");
	}
}
//---------------------------------------------------------------------------
void __fastcall Tmes::SendMsg(TX_DATA *tx, bool reply)
{
	AnsiString header;
	AnsiString data;

	if(reply)tx->SYSTEM_BYTES = mes_rx.SYSTEM_BYTES;
	else{
		systemByte += 1;
		if(systemByte > 9000)systemByte = 1000;
		tx->SYSTEM_BYTES = systemByte;
	}
	tx->EQUIP_ID = mes_rx.EQUIP_ID;
	tx->FROM = pcName;
	tx->TO = "MES";
	tx->RECIPE_ID = mes_rx.RECIPE_ID;
	tx->USER_ID = mes_rx.USER_ID;
	tx->OPER = mes_rx.OPER;


	data = "<DEFAULT><SYSTEM_BYTES>"+
			tx->SYSTEM_BYTES + "</SYSTEM_BYTES><EQUIP_ID>" +
			tx->EQUIP_ID + "</EQUIP_ID><LOT_ID>" +
			tx->LOT_ID + "</LOT_ID><FROM>" +
			tx->FROM + "</FROM><TO>" +
			tx->TO + "</TO><RECIPE_ID>" +
			tx->RECIPE_ID + "</RECIPE_ID><USER_ID>" +
			tx->USER_ID + "</USER_ID><OPER>" +
			tx->OPER + "</OPER><PROCESS>" +
			tx->PROCESS + "</PROCESS><DATE>" +
			Now().FormatString("yyyy-mm-dd hh:nn:ss") + "</DATE></DEFAULT>" +
			tx->DATA;

	header = "<MESSAGE><HEADER><MSG_ID>" + tx->MSG_ID + "</MSG_ID><MSG_LEN>" + IntToStr(data.Length()) + "</MSG_LEN></HEADER>";
	data = header + data + "</MESSAGE>";

	Tx_queue.push(data.c_str());
}
//---------------------------------------------------------------------------
void __fastcall Tmes::RecvMsg(AnsiString msg)
{
	AnsiString *tempMsg, str, parmeter;
	MainForm->mesTimer->Enabled = false;

	if(msg.IsEmpty())return;

	if(RecvMsg_GetMesMessage(msg))
	{
		str = getData(mes_rx.DATA, "ERROR_MSG");
		if(getData(mes_rx.DATA, "RETURN_VALUE") != "0"
			|| ((getData(mes_rx.DATA, "RETURN_VALUE") == "0")
			&& (str.Pos("WIP-0058")))){

			if(mes_rx.MSG_ID == "LINKTEST_EVENT") RecvMsg_LINK_TEST_EVENT();	// 통신 테스트
			else if(mes_rx.MSG_ID == "TRAY_REPLY") RecvMsg_TRAY_REPLY();		// 트레이 정보
			else if(mes_rx.MSG_ID == "DATE_EVENT") RecvMsg_DATE_EVENT();		// 시간 설정
			else if(mes_rx.MSG_ID == "TRANSFER_IN_REPLY") RecvMsg_TRANSFER_IN_REPLY();		// 트레이 작업 시작
			else if(mes_rx.MSG_ID == "SEND_EVENT") RecvMsg_SEND_EVENT();		// 트레이 배출 요청

			else	RecvMsg_etc();
		}else{

			// 대상 트레이의 TRANSFER_IN_REPLY 에러는 무시한다.
			if(mes_rx.MSG_ID == "TRANSFER_IN_REPLY" && mes_rx.LOT_ID == MainForm->pTrayid_target->Caption)RecvMsg_TRANSFER_IN_REPLY();
            else ErrorForm_mes->ShowError(mes_rx.LOT_ID, "[C_Maint] " + mes_rx.MSG_ID, str);
//			else if(str.Pos("WIP-0058") == 0)ErrorForm_mes->ShowError(mes_rx.LOT_ID, mes_rx.MSG_ID, str);
//			else RecvMsg_TRANSFER_IN_REPLY();
		}

	}

}
//---------------------------------------------------------------------------
AnsiString __fastcall Tmes::getData(AnsiString msg, AnsiString strFind)
{
	int first, end;

	first = msg.Pos("<" + strFind + ">") + strFind.Length() + 2;
	end = msg.Pos("</" + strFind + ">");

	return msg.SubString(first, end - first).Trim();
}
//---------------------------------------------------------------------------
bool __fastcall Tmes::RecvMsg_GetMesMessage(AnsiString msg)
{
	memset(&mes_rx, 0, sizeof(mes_rx));

	if(msg.Pos("<HEADER>"))
	{
		mes_rx.MSG_ID = getData(msg, "MSG_ID");
		mes_rx.MSG_LEN = getData(msg, "MSG_LEN");
		mes_rx.SYSTEM_BYTES = getData(msg, "SYSTEM_BYTES");
		mes_rx.EQUIP_ID = getData(msg, "EQUIP_ID");
		mes_rx.LOT_ID = getData(msg, "LOT_ID");
		mes_rx.FROM = getData(msg, "FROM");
		mes_rx.TO = getData(msg, "TO");
		mes_rx.RECIPE_ID = getData(msg, "RECIPE_ID");
		mes_rx.USER_ID = getData(msg, "USER_ID");
		mes_rx.OPER = getData(msg, "OPER");
		mes_rx.PROCESS = getData(msg, "PROCESS");
		mes_rx.DATE = getData(msg, "DATE");
		mes_rx.TRAY_GUBUN = getData(msg, "TRAY_GUBUN");
	}
	if(msg.Pos("<DATA>"))
	{
		mes_rx.DATA = getData(msg, "DATA");
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall Tmes::RecvMsg_TRAY_REPLY()
{
//	MainForm->tray = &MainForm->tray_source;
	memset(MainForm->tray, 0, sizeof(MainForm->tray));

	if(mes->mes_rx.LOT_ID == MainForm->pTrayid_source->Caption)MainForm->tray = &MainForm->tray_source;
	else if(mes->mes_rx.LOT_ID == MainForm->pTrayid_target->Caption)MainForm->tray = &MainForm->tray_target;

	MainForm->tray->RETURN_VALUE = getData(mes_rx.DATA, "RETURN_VALUE");	         			//정상;1, 오류;0
	MainForm->tray->ERROR_MSG = getData(mes_rx.DATA, "ERROR_MSG");	         				//Error Message
	MainForm->tray->SLOT_COUNT = getData(mes_rx.DATA, "SLOT_COUNT").ToInt();                     	//SLOT COUNT ( 기본24 EA )
	MainForm->tray->SLOT_COUNT = mes_rx.TRAY_GUBUN.ToInt();	// SLOT_COUNT 대신 TRAY_GUBUN 으로 사용한다.
	MainForm->tray->TRAY_GUBUN = mes_rx.TRAY_GUBUN;

	AnsiString msg;
	msg = mes_rx.DATA;
	int index = 0;
	int pos = msg.Pos("</SLOT_DATA>");

	while(pos > 0){
		MainForm->tray->SLOT_POSITION[index] = getData(msg, "SLOT_POSITION");          	//현재 TRAY 안의 CELL 위치 ( 1, 2, 3, 4 … ) , 공백 없는 것으로 판단.
		MainForm->tray->TARGET_SLOT_POSITION[index] = getData(msg, "TARGET_SLOT_POSITION");      //대상 TRAY 안의 CELL 위치 ( 1. 3. 8, 9 … )
		MainForm->tray->SLOT_ID[index] = getData(msg, "SLOT_ID");                   //SLOT 의 CELL ID
		MainForm->tray->LOSS_CD[index] = getData(msg, "LOSS_CD");                   //SLOT 불량코드
		MainForm->tray->LOSS_DESC[index] = getData(msg, "LOSS_DESC");                 //SLOT 의 불량 명
		MainForm->tray->PICK[index] = getData(msg, "PICK");                      //배출요청  = getData(msg, "RETURN_VALUE"); Y (불량 선별기, 랭크 선별 장비 이거보고 처리 )
		MainForm->tray->RANK[index] = getData(msg, "RANK");                      //CELL 랭크 정보 ( A, B, C, D … ) , 없을 수 있음
		MainForm->tray->SAMPLE_CODE[index] = getData(msg, "SAMPLE_CODE");            	//sample 취출 구분 코드(sample선별기와 코드가 일치하면 취출함)
		index++;
		msg.Delete(1, pos);
		pos = msg.Pos("</SLOT_DATA>");
	}
	MainForm->tray->PASS = getData(mes_rx.DATA, "PASS");                               //Y   = getData(mes_rx.DATA, "RETURN_VALUE"); 현재 위치 장비 정상 진행하지 않고 다음 물류 이동 ( 예  = getData(mes_rx.DATA, "RETURN_VALUE"); IR/OCV 검사하지 않고 다음공정 진행 )
	MainForm->tray->CONTACT_FLAG = getData(mes_rx.DATA, "CONTACT_FLAG");	         			//Contact 진행 여부, Y 일 경우 진행, N 일 경우 진행하지 않음 (충방전기, 출력 선별 설비 전용)
	MainForm->tray->KIND = getData(mes_rx.DATA, "KIND");                               //기종 ( 2층 격벽체결’ 해체 사용 )

	//* New Kind T1A, F3A, (Gamma, HKMC) Gamma 48, HKMC 24 2023 06 26,  F4A - 2023 10 24,
	//* JG1(F5A) - 2024 09 04
	//* F6A - 2025 01 06
	//if(MainForm->tray->SLOT_COUNT =! 12){ //* 2023 09 26 12개는 대상 트레이
		if(MainForm->tray->SLOT_COUNT == 96 && MainForm->tray->KIND != "PPE")
			MainForm->tray->CH_GUBUN = 96;
		else if(MainForm->tray->SLOT_COUNT == 96 && MainForm->tray->KIND == "PPE")
			MainForm->tray->CH_GUBUN = 962;
		//* Gamma
		else if(MainForm->tray->SLOT_COUNT == 48 && (MainForm->tray->KIND == "L4A" || MainForm->tray->KIND == "L3A"
		|| MainForm->tray->KIND == "F2A" || MainForm->tray->KIND == "F3A" || MainForm->tray->KIND == "F4A"
		|| MainForm->tray->KIND == "F5A" || MainForm->tray->KIND == "F6A"))
			MainForm->tray->CH_GUBUN = 482;
		//* New Kind (HKMC - T1A) 2023 06 26
		//* 2025 12 T3A(XV1) 모델 추가
		else if(MainForm->tray->SLOT_COUNT == 24 && (MainForm->tray->KIND == "T1A" || MainForm->tray->KIND == "T3A"))
			MainForm->tray->CH_GUBUN = 242;
		else if(MainForm->tray->SLOT_COUNT == 48)
			MainForm->tray->CH_GUBUN = 48;
	//}

	MainForm->tray->STOPPERTYPE = getData(mes_rx.DATA, "STOPPERTYPE");                   		// StopperType = getData(mes_rx.DATA, "RETURN_VALUE"); RIVET,BOLT
	MainForm->tray->BATCH = getData(mes_rx.DATA, "BATCH");                              // 화성 배치(Rank선별기에서 사용)
	MainForm->tray->SHIPBATCH = getData(mes_rx.DATA, "SHIPBATCH");

	MainForm->tray->EMPTY_FLAG = getData(mes_rx.DATA, "EMPTY_FLAG");
	MainForm->tray->TYPE = getData(mes_rx.DATA, "TYPE");
	MainForm->tray->WORK_TYPE = getData(mes_rx.DATA, "WORK_TYPE");
	MainForm->tray->WORK_CODE = getData(mes_rx.DATA, "WORK_CODE");
	MainForm->tray->NUTEJOIN_FLAG = getData(mes_rx.DATA, "NUTEJOIN_FLAG");
	MainForm->DisplayTrayInfo();
}
//---------------------------------------------------------------------------
void __fastcall Tmes::RecvMsg_TRANSFER_IN_REPLY()
{
	MainForm->DisplayTranserIn(mes->mes_rx.LOT_ID);
}
//---------------------------------------------------------------------------
void __fastcall Tmes::RecvMsg_LINK_TEST_EVENT()
{
	TX_DATA tx;
	memset(&tx, 0, sizeof(tx));
	tx.MSG_ID = "LINKTEST_REPLY";
	tx.LOT_ID = mes_rx.LOT_ID;
	tx.DATA = "<DATA><RETURN_VALUE>1</RETURN_VALUE><ERROR_MSG></ERROR_MSG></DATA>";

	SendMsg(&tx, true);
}
//---------------------------------------------------------------------------
void __fastcall Tmes::RecvMsg_DATE_EVENT()
{

	TDateTime date;
	date = StrToDateTime(getData(mes_rx.DATA, "DATE"));


	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);

	LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &luid);
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);

	SYSTEMTIME st;
	st.wYear= date.FormatString("yyyy").ToInt();
	st.wMonth= date.FormatString("mm").ToInt();
	st.wDay= date.FormatString("dd").ToInt();
	st.wHour= date.FormatString("hh").ToInt();
	st.wMinute= date.FormatString("nn").ToInt();
	st.wSecond= date.FormatString("ss").ToInt();
	st.wMilliseconds= 0;
	SetLocalTime(&st);

	TX_DATA tx;
	memset(&tx, 0, sizeof(tx));
	tx.MSG_ID = "DATE_REPLY";
	tx.DATA = "<DATA><RETURN_VALUE>1</RETURN_VALUE><ERROR_MSG></ERROR_MSG></DATA>";
	SendMsg(&tx, true);

}
//---------------------------------------------------------------------------
void __fastcall Tmes::RecvMsg_SEND_EVENT()
{
	TX_DATA tx;
	memset(&tx, 0, sizeof(tx));
	tx.MSG_ID = "SEND_REPLY";
	tx.DATA = "<DATA><RETURN_VALUE>1</RETURN_VALUE><ERROR_MSG></ERROR_MSG><WORK_NO></WORK_NO></DATA>";
	SendMsg(&tx, true);

	MainForm->CmdTrayOut(0);
}
//---------------------------------------------------------------------------
void __fastcall Tmes::RecvMsg_etc()
{
	getData(mes_rx.DATA, "RETURN_VALUE");	         			//정상;1, 오류;0
}
//---------------------------------------------------------------------------
void __fastcall Tmes::rxTimerTimer(TObject *Sender)
{
	AnsiString RxStr;
	if(Rx_queue.empty() == false){	// 데이터가 있으면 처리
		RxStr = Rx_queue.front().data();
		Rx_queue.pop();
		RecvMsg(RxStr);      // 헤더, 데이터 길이 체크
		WritemesLog("MES - PC",RxStr);
	}

}
//---------------------------------------------------------------------------

