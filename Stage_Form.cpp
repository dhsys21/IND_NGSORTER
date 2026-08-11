#include "FormBase.h"


void __fastcall TMainForm::sensorColor(TPanel *pnl, bool bon)
{
	if(bon){
		pnl->Color = clLime;
	}else{
		pnl->Color = clSilver;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::setLamp()
{
	switch(nowLampMode){
		case LampAuto:
			if(gripper->pauseStatus)
			{
				robostar->gripper.TOWER_LAMP_RED = 1;
				robostar->gripper.TOWER_LAMP_YELLOW = 0;
				robostar->gripper.TOWER_LAMP_GREEN = 0;
			}
			else if(gripper->seq == seqIdle && robostar->seq == seqIdle)
			{
				if(LampCount >= 20)
				{
					robostar->gripper.TOWER_LAMP_RED = 0;
					robostar->gripper.TOWER_LAMP_YELLOW = 1;
					robostar->gripper.TOWER_LAMP_GREEN = 0;
				}
				else
				{
					LampCount++;
					robostar->gripper.TOWER_LAMP_RED = 0;
					robostar->gripper.TOWER_LAMP_YELLOW = 0;
					robostar->gripper.TOWER_LAMP_GREEN = 1;
				}
			}
			else
			{
				robostar->gripper.TOWER_LAMP_RED = 0;
				robostar->gripper.TOWER_LAMP_YELLOW = 0;
				robostar->gripper.TOWER_LAMP_GREEN = 1;
			}
			break;
		case LampManual:
			robostar->gripper.TOWER_LAMP_RED = 1;
			robostar->gripper.TOWER_LAMP_YELLOW = 0;
			robostar->gripper.TOWER_LAMP_GREEN = 0;
			break;
		case LampEmergency:
			robostar->gripper.TOWER_LAMP_RED = 1;
			robostar->gripper.TOWER_LAMP_YELLOW = 0;
			robostar->gripper.TOWER_LAMP_GREEN = 0;
			break;
		case LampAlarm:
			robostar->gripper.TOWER_LAMP_RED = 1;
			robostar->gripper.TOWER_LAMP_YELLOW = 0;
			robostar->gripper.TOWER_LAMP_GREEN = 0;
			break;
		case LampStop:
			robostar->gripper.TOWER_LAMP_RED = 1;
			robostar->gripper.TOWER_LAMP_YELLOW = 0;
			robostar->gripper.TOWER_LAMP_GREEN = 0;
			break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TMainForm::DisplaySourceCell(int toolNum, int ch)
{
	TColor clr;
	switch(toolNum){
		case -1:clr = clYellow;break;
		case 0:clr = CLR1->Font->Color;break;
		case 1:clr = CLR2->Font->Color;break;
	}
	psort_ing[ch]->Color = clr;
	psort_bad[ch]->Color = clr;
	psort_rank[ch]->Color = clr;
	if(toolNum == -1){
		psort_ing[ch]->Caption = "취출";
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DisplayTargetCell(int toolNum, int ch)
{
	if(toolNum >= 0){
		color_target[ch / 24][23 - (ch % 24)] = clYellow;
		targetGrid->Cells[ch / 24][23 - (ch % 24)] = "Gripper #" + IntToStr(toolNum+1) + " 예약";
	}else{
		color_target[ch / 24][23 - (ch % 24)] = clInactiveCaption;
		targetGrid->Cells[ch / 24][23 - (ch % 24)] = tray_target.LOSS_CD[ch] + "-" + getCodeName(tray_target.LOSS_CD[ch].Trim());
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DisplayTargetCellInfo(int toolNum, int ch)
{
	if(toolNum >= 0){
		pTarget_bad[ch]->Caption = "Gripper #" + IntToStr(toolNum+1) + " 예약";
		pTarget_bad[ch]->Color = clYellow;
	}else{
		pTarget_bad[ch]->Caption = tray_target.LOSS_CD[ch] + "-" + getCodeName(tray_target.LOSS_CD[ch].Trim());
		pTarget_bad[ch]->Color = clInactiveCaption;
	}
}
//---------------------------------------------------------------------------
// 메인화면 검사 진행 표시
void __fastcall TMainForm::DisplayStatus(int status)
{

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::setMapping()
{
	for(int i = 0; i < 96; ++i){
		mapSort[0][i] = i + 1;	// 1~96
	}
}
//---------------------------------------------------------------------------
// 메인화면 패널 만들기
void __fastcall TMainForm::MakePanel()
{
	int nx, ny, nw, nh;

	nw = 146; // 196 -> 146
	nh = 16;
    nx = 2 + (3 * (30 + nw + 3));
	ny = pBase->Height - nh - 3;//490;

	for(int index = 0; index < 96;){
		psort_ch[index] = new TPanel(this);
		SetOption(psort_ch[index], nx, ny, 30, nh, pnlSourceTrayId->Color, index);
		psort_ch[index]->Font->Color = pnlSourceTrayId->Font->Color;
		psort_ch[index]->Font->Size = 13;
		psort_ch[index]->Caption = index+1;

		psort_ing[index] = new TPanel(this);
		SetOption(psort_ing[index], nx + 30 + 1, ny, 50, nh, clWhite, index);

		psort_bad[index] = new TPanel(this);
		SetOption(psort_bad[index], nx + 80 + 2, ny, 95, nh, clWhite, index);

//		psort_rank[index] = new TPanel(this);
//		SetOption(psort_rank[index], nx + 175 + 3, ny, 50, nh, clWhite, index);
		ny = ny - nh - 1;
		index += 1;

		if(index % 12 == 0) ny -= 3;
		if(index % 24 == 0){
			nx = nx - 30 - nw - 3;
			ny = pBase->Height - nh - 3;
		}
	}
//	pBase->Color = clWhite;
	pt_ch[0] = pt1;
	pt_ch[1] = pt2;
	pt_ch[2] = pt3;
	pt_ch[3] = pt4;
	pt_ch[4] = pt5;
	pt_ch[5] = pt6;
	pt_ch[6] = pt7;
	pt_ch[7] = pt8;
	pt_ch[8] = pt9;
	pt_ch[9] = pt10;
	pt_ch[10] = pt11;
	pt_ch[11] = pt12;
	pt_ch[12] = pt13;
	pt_ch[13] = pt14;
	pt_ch[14] = pt15;
	pt_ch[15] = pt16;
	pt_ch[16] = pt17;
	pt_ch[17] = pt18;
	pt_ch[18] = pt19;
	pt_ch[19] = pt20;
	pt_ch[20] = pt21;
	pt_ch[21] = pt22;
	pt_ch[22] = pt23;
	pt_ch[23] = pt24;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetOption(TPanel *pnl, int nx, int ny, int nw, int nh, TColor clr, int tagValue)
{
	pnl->Parent = pBase;
	pnl->Left =  nx;
	pnl->Top = ny;
	pnl->Width = nw;
	pnl->Height = nh;
	pnl->Alignment = taCenter;
    pnl->ParentBackground = false;
	if(nw != 30){
		pnl->Font->Size = 8;
		pnl->Font->Color = clBlack;
	//	pnl->Font->Style = Font->Style << fsBold;
	}
	pnl->Color = clr;
	pnl->Tag = tagValue;

	pnl->BevelInner = bvNone;
	pnl->BevelKind = bkNone;
	pnl->BevelOuter = bvNone;
    pnl->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ChangeTrayMap(int channel)
{
	int nx, ny, nw, nh;
	nx = 2;
	ny = 490;
	nw = 196;


	if(channel == 96){
		nh = 20;
		for(int index=0; index<96;){
			psort_ing[index]->Caption = mapSort[0][index];
			if(index < 48){
				SetOption(psort_ch[index], nx, ny, 30, nh, pnlSourceTrayId->Color, index);
				SetOption(psort_ing[index], nx + 30 + 1, ny, 50, nh, clWhite, index);
				SetOption(psort_bad[index], nx + 80 + 2, ny, 95, nh, clWhite, index);
				SetOption(psort_rank[index], nx + 175 + 3, ny, 50, nh, clWhite, index);
				ny = ny - nh - 1;
				index += 1;

				if(index % 12 == 0) ny -= 5;
				if(index % 24 == 0){
					nx = nx + 30 + nw + 3;
					ny = 490;
				}
			}else{
				psort_ch[index]->Visible = true;
				psort_ing[index]->Visible = true;
				psort_bad[index]->Visible = true;
				psort_rank[index]->Visible = true;
				index += 1;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MakePanel_TargetTray()
{
    int nx, ny, nw, nh;

    nw = 145;
	nh = 16;
	nx = 2 + (3 * (30 + nw + 4));
	ny = pTargetBase->Height - nh - 3;

	for(int index = 0; index < 96;){
		pTarget_ch[index] = new TPanel(this);
		SetOption_TargetTray(pTarget_ch[index], nx, ny, 30, nh, pnlSourceTrayId->Color, index);
		pTarget_ch[index]->Font->Color = pnlSourceTrayId->Font->Color;
		pTarget_ch[index]->Font->Size = 13;
		pTarget_ch[index]->Caption = index+1;

		pTarget_bad[index] = new TPanel(this);
		SetOption_TargetTray(pTarget_bad[index], nx + 30 + 2, ny, nw, nh, clWhite, index);

		ny = ny - nh - 1;
		index += 1;
        if(index % 12 == 0) ny -= 3;
		if(index % 24 == 0){
			nx = nx - 30 - nw - 4;
			ny = pTargetBase->Height - nh - 3;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetOption_TargetTray(TPanel *pnl, int nx, int ny, int nw, int nh, TColor clr, int tagValue)
{
	pnl->Parent = pTargetBase;
	pnl->Left =  nx;
	pnl->Top = ny;
	pnl->Width = nw;
	pnl->Height = nh;
	pnl->Alignment = taCenter;
    pnl->ParentBackground = false;
	if(nw != 30){
		pnl->Font->Size = 8;
		pnl->Font->Color = clBlack;
	//	pnl->Font->Style = Font->Style << fsBold;
	}
	pnl->Color = clr;
	pnl->Tag = tagValue;

	pnl->BevelInner = bvNone;
	pnl->BevelKind = bkNone;
	pnl->BevelOuter = bvNone;
    pnl->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ChangeTrayMap_TargetTray(int channel)
{
//	int nx, ny, nw, nh;
//
//	nx = 2;
//	ny = 284;
//	nh = 56;
//
//	if(channel == 482)
//	{
//		nw = 421;
//        for(int index=0; index < 24;){
//			if(index < 12)
//			{
//				SetOption_TargetTray(pTarget_ch[index], nx, ny, 30, nh, psample->Color, index);
//				SetOption_TargetTray(pTarget_bad[index], nx + 30 + 1, ny, 420, nh, clWhite, index);
//
//				ny = ny - nh - 1;
//				index += 1;
//
//				if(index % 6 == 0){
//					nx = nx + 30 + nw + 3;
//					ny = 284;
//				}
//			}else{
//				pTarget_ch[index]->Visible = false;
//				pTarget_bad[index]->Visible = false;
//				index += 1;
//			}
//		}
//	}
//	else
//	{
//		nw = 194;
//		for(int index = 0; index < 24;){
//            if(index < 12)
//			{
//				SetOption_TargetTray(pTarget_ch[index], nx, ny, 30, nh, psample->Color, index);
//				SetOption_TargetTray(pTarget_bad[index], nx + 30 + 1, ny, 193, nh, clWhite, index);
//
//				ny = ny - nh - 1;
//				index += 1;
//
//				if(index % 6 == 0){
//					nx = nx + 30 + nw + 3;
//					ny = 284;
//				}
//			}else{
//				pTarget_ch[index]->Visible = true;
//				pTarget_bad[index]->Visible = true;
//				index += 1;
//			}
//		}
//    }
}
//---------------------------------------------------------------------------
int __fastcall TMainForm::FindList(AnsiString strType)
{
	for(int i= 0; i<badList->Items->Count; ++i){
		if(badList->Items->Item[i]->SubItems->Strings[0] == strType){
			return i;
		}
	}
	return -1;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AddList(AnsiString strType)
{
	int index  = FindList(strType);
	if(index < 0){
		badList->Items->BeginUpdate();
		ITEM = badList->Items->Add();
		ITEM->Caption = badList->Items->Count;
		ITEM->SubItems->Add(strType);
		ITEM->SubItems->Add(1);
		badList->Items->EndUpdate();
	}else{
		badList->Items->Item[index]->SubItems->Strings[1] = badList->Items->Item[index]->SubItems->Strings[1].ToInt() + 1;
		badList->ItemIndex = index;
	}

	pbad_sum->Caption = pbad_sum->Caption.ToInt() + 1; 
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::setTrayInfo(int index)
{
	if(index == 0)
	{
		m_saveTrayInfo[index].LOT_ID = pTrayid_source->Caption;
		m_saveTrayInfo[index].SLOT_COUNT = tray_source.SLOT_COUNT;
		for(int i = 0; i < tray_source.SLOT_COUNT; i++)
		{
			m_saveTrayInfo[index].SLOT_POSITION[i] = tray_source.SLOT_POSITION[i];
			m_saveTrayInfo[index].SLOT_ID[i] = tray_source.SLOT_ID[i];
			m_saveTrayInfo[index].PICK[i] = tray_source.PICK[i];
		}
	}
	else if(index == 1)
	{
		m_saveTrayInfo[index].LOT_ID = pTrayid_target->Caption;
		m_saveTrayInfo[index].SLOT_COUNT = tray_target.SLOT_COUNT;
		for(int i = 0; i < tray_target.SLOT_COUNT; i++)
		{
			m_saveTrayInfo[index].SLOT_POSITION[i] = tray_target.SLOT_POSITION[i];
			m_saveTrayInfo[index].SLOT_ID[i] = tray_target.SLOT_ID[i];
			m_saveTrayInfo[index].PICK[i] = tray_target.PICK[i];
		}
	}

	saveTrayInfo(index);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::saveTrayInfo(int index)
{
	AnsiString file;
	file = (AnsiString)BIN + "TrayInfo.ini";

	ini = new TIniFile(file);

	ini->WriteString(index, "LOT_ID", m_saveTrayInfo[index].LOT_ID);
	ini->WriteInteger(index, "SLOT_COUNT", m_saveTrayInfo[index].SLOT_COUNT);

	for(int i = 0; i < m_saveTrayInfo[index].SLOT_COUNT; i++)
	{
		ini->WriteString(index, "SLOT_POSITION" + IntToStr(i), m_saveTrayInfo[index].SLOT_POSITION[i]);
		ini->WriteString(index, "SLOT_ID" + IntToStr(i), m_saveTrayInfo[index].SLOT_ID[i]);
		ini->WriteString(index, "PICK" + IntToStr(i), m_saveTrayInfo[index].PICK[i]);
	}

	delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::loadTrayInfo(int index)
{
	AnsiString file;
	file = (AnsiString)BIN + "TrayInfo.ini";

	ini = new TIniFile(file);

	m_saveTrayInfo[index].LOT_ID = ini->ReadString(index, "LOT_ID", "");
	m_saveTrayInfo[index].SLOT_COUNT = ini->ReadInteger(index, "SLOT_COUNT", 96);

	for(int i = 0; i < m_saveTrayInfo[index].SLOT_COUNT; i++)
	{
		m_saveTrayInfo[index].SLOT_POSITION[i] = ini->ReadString(index, "SLOT_POSITION" + IntToStr(i), "");
		m_saveTrayInfo[index].SLOT_ID[i] = ini->ReadString(index, "SLOT_ID" + IntToStr(i), "");
		m_saveTrayInfo[index].PICK[i] = ini->ReadString(index, "PICK" + IntToStr(i), "");
	}

	delete ini;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::checkTrayInfo(int index)
{
	if(index == 0)
	{
		if(m_saveTrayInfo[0].LOT_ID == pTrayid_source->Caption)
		{
			for(int i = 0; i < tray_source.SLOT_COUNT; i++)
			{
				if(m_saveTrayInfo[0].SLOT_POSITION[i] != tray_source.SLOT_POSITION[i]
				   || m_saveTrayInfo[0].SLOT_ID[i] != tray_source.SLOT_ID[i]
				   || m_saveTrayInfo[0].PICK[i] != tray_source.PICK[i])
				{
					return false;
				}
			}
		}
	}
	else if(index == 1)
	{
		if(m_saveTrayInfo[1].LOT_ID == pTrayid_target->Caption)
		{
			for(int i = 0; i < tray_target.SLOT_COUNT; i++)
			{
				if(m_saveTrayInfo[1].SLOT_POSITION[i] != tray_target.SLOT_POSITION[i]
				   || m_saveTrayInfo[1].SLOT_ID[i] != tray_target.SLOT_ID[i]
				   || m_saveTrayInfo[1].PICK[i] != tray_target.PICK[i])
				{
					return false;
				}
			}
		}
	}
    return true;
}
//---------------------------------------------------------------------------
