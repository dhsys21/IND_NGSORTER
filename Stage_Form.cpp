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
	if(toolNum == -1){
		psort_ing[ch]->Caption = "취출";
	}
}
//---------------------------------------------------------------------------
//* 불량트레이 관리
int __fastcall TMainForm::GetTargetReservationTool(int ch) const
{
	for(int i = 0; i < gripCnt; ++i){
		if(StrToIntDef(gripper->tool[i].target_ch, 0) == ch + 1)
			return i;
	}
	return -1;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DisplayTargetCell(int toolNum, int ch)
{
	//* 불량트레이 관리
	// Local target state uses PICK=R until the physical insert completes.
	if(tray_target.PICK[ch] == "R" && toolNum < 0){
		toolNum = GetTargetReservationTool(ch);
		if(toolNum < 0) toolNum = 0; // This machine has one gripper.
	}
	if(toolNum >= 0){
		color_target[ch / 24][23 - (ch % 24)] = clYellow;
		AnsiString reservation = "Gripper #" + IntToStr(toolNum+1) + " reservation";
		if(!tray_target.LOSS_CD[ch].Trim().IsEmpty())
			reservation = tray_target.LOSS_CD[ch].Trim() + " / " + reservation;
		targetGrid->Cells[ch / 24][23 - (ch % 24)] = reservation;
	}else{
		color_target[ch / 24][23 - (ch % 24)] = clInactiveCaption;
		targetGrid->Cells[ch / 24][23 - (ch % 24)] = tray_target.LOSS_CD[ch];
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DisplayTargetCellInfo(int toolNum, int ch)
{
	//* 불량트레이 관리
	// Do not replace a locally persisted reservation before insert completion.
	if(tray_target.PICK[ch] == "R" && toolNum < 0){
		toolNum = GetTargetReservationTool(ch);
		if(toolNum < 0) toolNum = 0;
	}
	if(toolNum >= 0){
		AnsiString reservation = "Gripper #" + IntToStr(toolNum+1) + " reservation";
		if(!tray_target.LOSS_CD[ch].Trim().IsEmpty())
			reservation = tray_target.LOSS_CD[ch].Trim() + " / " + reservation;
		pTarget_bad[ch]->Caption = reservation;
		pTarget_bad[ch]->Color = clYellow;
	}else{
		pTarget_bad[ch]->Caption = tray_target.LOSS_CD[ch];
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

	nw = 130; // Compact four-column map for side-by-side Source/Target trays.
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
		SetOption(psort_bad[index], nx + 80 + 2, ny, 79, nh, clWhite, index);

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
				SetOption(psort_bad[index], nx + 80 + 2, ny, 79, nh, clWhite, index);
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
				index += 1;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MakePanel_TargetTray()
{
    int nx, ny, nw, nh;

    nw = 128; // Compact four-column map for side-by-side Source/Target trays.
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
//* 불량트레이 관리
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
			m_saveTrayInfo[index].CELL_LOT_ID[i] = tray_source.CELL_LOT_ID[i];
			m_saveTrayInfo[index].CELL_EXIST[i] = tray_source.CELL_EXIST[i];
			m_saveTrayInfo[index].WORK_FLAG[i] = tray_source.WORK_FLAG[i];
			m_saveTrayInfo[index].PICK[i] = tray_source.PICK[i];
			m_saveTrayInfo[index].LOSS_CD[i] = tray_source.LOSS_CD[i];
			m_saveTrayInfo[index].RANK[i] = tray_source.RANK[i];
		}
	}
	else if(index == 1)
	{
		//* 불량트레이 관리
		m_saveTrayInfo[index].LOT_ID = pTrayid_target->Caption.Trim();
		targetTrayInfoActiveId = m_saveTrayInfo[index].LOT_ID;
		m_saveTrayInfo[index].SLOT_COUNT = tray_target.SLOT_COUNT;
		for(int i = 0; i < tray_target.SLOT_COUNT && i < 96; i++)
		{
			m_saveTrayInfo[index].SLOT_POSITION[i] = tray_target.SLOT_POSITION[i];
			m_saveTrayInfo[index].SLOT_ID[i] = tray_target.SLOT_ID[i];
			m_saveTrayInfo[index].CELL_LOT_ID[i] = tray_target.CELL_LOT_ID[i];
			m_saveTrayInfo[index].CELL_EXIST[i] = tray_target.CELL_EXIST[i];
			m_saveTrayInfo[index].WORK_FLAG[i] = tray_target.WORK_FLAG[i];
			m_saveTrayInfo[index].PICK[i] = tray_target.PICK[i];
			m_saveTrayInfo[index].LOSS_CD[i] = tray_target.LOSS_CD[i];
			m_saveTrayInfo[index].RANK[i] = tray_target.RANK[i];
		}
	}

	saveTrayInfo(index);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::saveTrayInfo(int index)
{
	if(index < 0 || index > 1 || m_saveTrayInfo[index].LOT_ID.IsEmpty()) return;
	AnsiString file = index == 1 ?
		GetTargetTrayInfoFile(m_saveTrayInfo[index].LOT_ID) :
		GetSourceTrayInfoFile(m_saveTrayInfo[index].LOT_ID);

	ini = new TIniFile(file);
	// Local tray schema follows TrackInCellInformation. Pick is the only
	// equipment-only field and is required to restore reservation/work state.
	ini->EraseSection("0"); // Remove the legacy Source layout on the next save.
	ini->EraseSection("1");
	ini->EraseSection("TRAY");
	ini->WriteString("TRAY", "TrayId", m_saveTrayInfo[index].LOT_ID);
	ini->WriteInteger("TRAY", "CellCount", m_saveTrayInfo[index].SLOT_COUNT);
	ini->WriteString("TRAY", "LastUpdated", FormatDateTime("yyyy-mm-dd hh:nn:ss", Now()));
	ini->WriteString("TRAY", "State", "ACTIVE");

	for(int i = 0; i < 96; ++i)
	{
		AnsiString section = "CELL_" + IntToStr(i + 1);
		ini->EraseSection(section); // Remove legacy SLOT_*/LOSS_CD/RANK keys.
		if(i >= m_saveTrayInfo[index].SLOT_COUNT) continue;
		ini->WriteString(section, "CellId", m_saveTrayInfo[index].SLOT_ID[i]);
		ini->WriteInteger(section, "CellNo",
			m_saveTrayInfo[index].SLOT_POSITION[i].ToIntDef(i + 1));
		ini->WriteString(section, "LotId", m_saveTrayInfo[index].CELL_LOT_ID[i]);
		ini->WriteBool(section, "CellExist", m_saveTrayInfo[index].CELL_EXIST[i]);
		ini->WriteString(section, "NGCode", m_saveTrayInfo[index].LOSS_CD[i]);
		ini->WriteString(section, "Grade", m_saveTrayInfo[index].RANK[i]);
		ini->WriteBool(section, "WorkFlag", m_saveTrayInfo[index].WORK_FLAG[i]);
		ini->WriteString(section, "Pick", m_saveTrayInfo[index].PICK[i]);
	}
	delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::loadTrayInfo(int index)
{
	if(index < 0 || index > 1) return;
	AnsiString trayId = index == 1 ? targetTrayInfoActiveId : pTrayid_source->Caption.Trim();
	if(index == 1 && trayId.IsEmpty()) trayId = pTrayid_target->Caption.Trim();
	AnsiString file = index == 1 ? GetTargetTrayInfoFile(trayId) : GetSourceTrayInfoFile(trayId);

	ini = new TIniFile(file);
	// Read the unified TrackIn-style schema first, then fall back to legacy files.
	AnsiString legacySection = IntToStr(index);
	m_saveTrayInfo[index].LOT_ID = ini->ReadString("TRAY", "TrayId",
		ini->ReadString("TRAY", "TRAY_ID", ini->ReadString(legacySection, "LOT_ID", "")));
	m_saveTrayInfo[index].SLOT_COUNT = ini->ReadInteger("TRAY", "CellCount",
		ini->ReadInteger("TRAY", "SLOT_COUNT", ini->ReadInteger(legacySection, "SLOT_COUNT", 96)));
	if(m_saveTrayInfo[index].SLOT_COUNT < 1 || m_saveTrayInfo[index].SLOT_COUNT > 96)
		m_saveTrayInfo[index].SLOT_COUNT = 96;

	for(int i = 0; i < m_saveTrayInfo[index].SLOT_COUNT; ++i)
	{
		AnsiString section = "CELL_" + IntToStr(i + 1);
		AnsiString oldCellNo = index == 1 ?
			ini->ReadString(section, "SLOT_POSITION", IntToStr(i + 1)) :
			ini->ReadString(legacySection, "SLOT_POSITION" + IntToStr(i), IntToStr(i + 1));
		AnsiString oldCellId = index == 1 ?
			ini->ReadString(section, "SLOT_ID", "") :
			ini->ReadString(legacySection, "SLOT_ID" + IntToStr(i), "");
		AnsiString oldLotId = index == 1 ?
			ini->ReadString(section, "LOT_ID", "") :
			ini->ReadString(legacySection, "CELL_LOT_ID" + IntToStr(i), "");
		AnsiString oldPick = index == 1 ?
			ini->ReadString(section, "PICK", "N") :
			ini->ReadString(legacySection, "PICK" + IntToStr(i), "N");
		AnsiString oldNgCode = index == 1 ?
			ini->ReadString(section, "LOSS_CD", "") :
			ini->ReadString(legacySection, "LOSS_CD" + IntToStr(i), "");
		AnsiString oldGrade = index == 1 ?
			ini->ReadString(section, "RANK", "") :
			ini->ReadString(legacySection, "RANK" + IntToStr(i), "");
		bool oldCellExist = index == 1 ?
			ini->ReadBool(section, "CELL_EXIST", oldPick == "Y" || !oldCellId.IsEmpty()) :
			ini->ReadBool(legacySection, "CELL_EXIST" + IntToStr(i), !oldCellId.IsEmpty());

		m_saveTrayInfo[index].SLOT_POSITION[i] = IntToStr(
			ini->ReadInteger(section, "CellNo", oldCellNo.ToIntDef(i + 1)));
		m_saveTrayInfo[index].SLOT_ID[i] = ini->ReadString(section, "CellId", oldCellId);
		m_saveTrayInfo[index].CELL_LOT_ID[i] = ini->ReadString(section, "LotId", oldLotId);
		m_saveTrayInfo[index].CELL_EXIST[i] = ini->ReadBool(section, "CellExist", oldCellExist);
		m_saveTrayInfo[index].LOSS_CD[i] = ini->ReadString(section, "NGCode", oldNgCode);
		m_saveTrayInfo[index].RANK[i] = ini->ReadString(section, "Grade", oldGrade);
		m_saveTrayInfo[index].WORK_FLAG[i] = ini->ReadBool(section, "WorkFlag", oldCellExist);
		m_saveTrayInfo[index].PICK[i] = ini->ReadString(section, "Pick", oldPick);
	}
	delete ini;
}
//---------------------------------------------------------------------------
static AnsiString MakeSafeTrayFileId(AnsiString trayId)
{
	AnsiString safeId = "";
	trayId = trayId.Trim();
	for(int i = 1; i <= trayId.Length(); ++i){
		unsigned char ch = (unsigned char)trayId[i];
		if((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') ||
			(ch >= 'a' && ch <= 'z') || ch == '-' || ch == '_')
			safeId += (char)ch;
		else
			safeId += '_';
	}
	if(safeId.IsEmpty()) safeId = "UNKNOWN";
	return safeId;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainForm::GetSourceTrayInfoFile(AnsiString trayId) const
{
	return (AnsiString)TRAY_PATH + "SourceTray_" + MakeSafeTrayFileId(trayId) + ".ini";
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainForm::GetTargetTrayInfoFile(AnsiString trayId) const
{
	return (AnsiString)TRAY_PATH + "TargetTray_" + MakeSafeTrayFileId(trayId) + ".ini";
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::PrepareActiveTrayInfoFile(bool sourceTray, AnsiString trayId)
{
	trayId = trayId.Trim();
	if(trayId.IsEmpty()) return;
	if(!DirectoryExists((AnsiString)TRAY_PATH) && !ForceDirectories((AnsiString)TRAY_PATH)){
		memoMainLineAdd("[LOCAL TRAY] ERROR - cannot create folder: " + AnsiString(TRAY_PATH));
		return;
	}

	AnsiString keepFile = sourceTray ? GetSourceTrayInfoFile(trayId) : GetTargetTrayInfoFile(trayId);
	AnsiString legacyFile;
	bool preserveLegacy = false;
	if(sourceTray){
		legacyFile = (AnsiString)BIN + "TrayInfo.ini";
		if(FileExists(legacyFile)){
			TIniFile *legacyIni = new TIniFile(legacyFile);
			AnsiString legacyTrayId = legacyIni->ReadString("0", "LOT_ID", "").Trim();
			delete legacyIni;
			if(SameText(legacyTrayId, trayId) && !FileExists(keepFile)){
				if(RenameFile(legacyFile, keepFile))
					memoMainLineAdd("[LOCAL SOURCE] Migrated: " + keepFile);
				else
					preserveLegacy = true;
			}
			if(FileExists(legacyFile) && !preserveLegacy) DeleteFile(legacyFile);
		}
	}else{
		legacyFile = (AnsiString)BIN + "NG_TrayInfo_" + MakeSafeTrayFileId(trayId) + ".ini";
		if(FileExists(legacyFile) && !FileExists(keepFile)){
			if(RenameFile(legacyFile, keepFile))
				memoMainLineAdd("[LOCAL TARGET] Migrated: " + keepFile);
			else
				preserveLegacy = true;
		}
		if(FileExists(legacyFile) && !preserveLegacy) DeleteFile(legacyFile);
	}

	AnsiString prefix = sourceTray ? "SourceTray_" : "TargetTray_";
	TSearchRec searchRec;
	int findResult = FindFirst((AnsiString)TRAY_PATH + prefix + "*.ini", faAnyFile, searchRec);
	if(findResult == 0){
		while(findResult == 0){
			if((searchRec.Attr & faDirectory) == 0){
				AnsiString foundFile = (AnsiString)TRAY_PATH + searchRec.Name;
				if(AnsiCompareText(foundFile, keepFile) != 0){
					DeleteFile(foundFile);
					memoMainLineAdd("[LOCAL TRAY] Removed previous " + prefix + " file: " + searchRec.Name);
				}
			}
			findResult = FindNext(searchRec);
		}
		FindClose(searchRec);
	}

	// Remove remaining legacy Target files independently from Source files.
	if(!sourceTray){
		findResult = FindFirst((AnsiString)BIN + "NG_TrayInfo_*.ini", faAnyFile, searchRec);
		if(findResult == 0){
			while(findResult == 0){
				if((searchRec.Attr & faDirectory) == 0){
					AnsiString foundFile = (AnsiString)BIN + searchRec.Name;
					if(!preserveLegacy || AnsiCompareText(foundFile, legacyFile) != 0)
						DeleteFile(foundFile);
				}
				findResult = FindNext(searchRec);
			}
			FindClose(searchRec);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ResetTargetTraySaveInfo(AnsiString trayId)
{
	//* 불량트레이 관리
	m_saveTrayInfo[1].LOT_ID = trayId.Trim();
	m_saveTrayInfo[1].SLOT_COUNT = 96;
	for(int i = 0; i < 96; ++i){
		m_saveTrayInfo[1].SLOT_POSITION[i] = IntToStr(i + 1);
		m_saveTrayInfo[1].SLOT_ID[i] = "";
		m_saveTrayInfo[1].CELL_LOT_ID[i] = "";
		m_saveTrayInfo[1].CELL_EXIST[i] = false;
		m_saveTrayInfo[1].WORK_FLAG[i] = false;
		m_saveTrayInfo[1].PICK[i] = "N";
		m_saveTrayInfo[1].LOSS_CD[i] = "";
		m_saveTrayInfo[1].RANK[i] = "";
	}
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::TargetTrayInfoHasData(int &occupiedCount, int &reservedCount) const
{
	//* 불량트레이 관리
	occupiedCount = 0;
	reservedCount = 0;
	for(int i = 0; i < m_saveTrayInfo[1].SLOT_COUNT && i < 96; ++i){
		if(m_saveTrayInfo[1].PICK[i] == "R")
			++reservedCount;
		else if(m_saveTrayInfo[1].CELL_EXIST[i])
			++occupiedCount;
	}
	return occupiedCount > 0 || reservedCount > 0;
}
//---------------------------------------------------------------------------
int __fastcall TMainForm::RestoreTargetTrayInfo(AnsiString trayId, bool confirmExisting)
{
	//* 불량트레이 관리
	trayId = trayId.Trim();
	if(trayId.IsEmpty()) return 0;
	targetTrayInfoActiveId = trayId;
	AnsiString file = GetTargetTrayInfoFile(trayId);
	if(!FileExists(file)){
		ResetTargetTraySaveInfo(trayId);
		// 바코드를 읽은 즉시 빈 불량트레이 파일을 생성한다.
		saveTrayInfo(1);
		memoMainLineAdd("[LOCAL TARGET] Created: " + trayId);
		return 0;
	}

	loadTrayInfo(1);
	if(m_saveTrayInfo[1].LOT_ID != trayId ||
		m_saveTrayInfo[1].SLOT_COUNT < 1 || m_saveTrayInfo[1].SLOT_COUNT > 96){
		ResetTargetTraySaveInfo(trayId);
		saveTrayInfo(1);
		memoMainLineAdd("[LOCAL TARGET] Recreated invalid file: " + trayId);
		return 0;
	}

	int occupiedCount = 0;
	int reservedCount = 0;
	if(confirmExisting && TargetTrayInfoHasData(occupiedCount, reservedCount)){
		TIniFile *targetIni = new TIniFile(file);
		AnsiString lastUpdated = targetIni->ReadString("TRAY", "LAST_UPDATED", "-");
		delete targetIni;
		UnicodeString message = L"기존 불량트레이 정보가 있습니다.\r\n\r\n";
		message += L"바코드: ";
		message += UnicodeString(trayId);
		message += L"\r\n삽입 완료: ";
		message += IntToStr(occupiedCount);
		message += L"\r\n그리퍼 예약: ";
		message += IntToStr(reservedCount);
		message += L"\r\n마지막 저장: ";
		message += UnicodeString(lastUpdated);
		message += L"\r\n\r\n예: 기존 정보 사용\r\n아니요: 빈 트레이로 초기화\r\n취소: 작업 중단";
		int reply = MessageBox(Handle, message.c_str(), L"불량트레이 정보 확인",
			MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1);
		if(reply == IDCANCEL) return -1;
		if(reply == IDNO){
			DeleteFile(file);
			ResetTargetTraySaveInfo(trayId);
			saveTrayInfo(1);
			memoMainLineAdd("[LOCAL TARGET] Initialized by operator: " + trayId);
			return 0;
		}
	}

	tray_target.SLOT_COUNT = m_saveTrayInfo[1].SLOT_COUNT;
	tray_target.TRAY_GUBUN = IntToStr(tray_target.SLOT_COUNT);
	bool recoveredTrackInValues = false;
	for(int i = 0; i < tray_target.SLOT_COUNT; ++i){
		tray_target.SLOT_POSITION[i] = m_saveTrayInfo[1].SLOT_POSITION[i];
		tray_target.SLOT_ID[i] = m_saveTrayInfo[1].SLOT_ID[i];
		tray_target.CELL_LOT_ID[i] = m_saveTrayInfo[1].CELL_LOT_ID[i];
		tray_target.CELL_EXIST[i] = m_saveTrayInfo[1].CELL_EXIST[i];
		tray_target.WORK_FLAG[i] = m_saveTrayInfo[1].WORK_FLAG[i];
		tray_target.PICK[i] = m_saveTrayInfo[1].PICK[i];
		tray_target.LOSS_CD[i] = m_saveTrayInfo[1].LOSS_CD[i];
		tray_target.RANK[i] = m_saveTrayInfo[1].RANK[i];
		if(tray_target.SLOT_POSITION[i].IsEmpty())
			tray_target.SLOT_POSITION[i] = IntToStr(i + 1);
		if(tray_target.PICK[i].IsEmpty())
			tray_target.PICK[i] = "N";

		// Older local files have no per-cell LOT_ID. Recover all three original
		// TrackIn values only when the same CellId is still present in Source Tray.
		if(!tray_target.SLOT_ID[i].IsEmpty() && tray_target.CELL_LOT_ID[i].IsEmpty()){
			for(int src = 0; src < tray_source.SLOT_COUNT && src < 96; ++src){
				if(tray_source.SLOT_ID[src] == tray_target.SLOT_ID[i]){
					tray_target.CELL_LOT_ID[i] = tray_source.CELL_LOT_ID[src];
					tray_target.LOSS_CD[i] = tray_source.LOSS_CD[src];
					tray_target.RANK[i] = tray_source.RANK[src];
					tray_target.WORK_FLAG[i] = tray_source.WORK_FLAG[src];
					m_saveTrayInfo[1].CELL_LOT_ID[i] = tray_target.CELL_LOT_ID[i];
					m_saveTrayInfo[1].WORK_FLAG[i] = tray_target.WORK_FLAG[i];
					m_saveTrayInfo[1].LOSS_CD[i] = tray_target.LOSS_CD[i];
					m_saveTrayInfo[1].RANK[i] = tray_target.RANK[i];
					recoveredTrackInValues = true;
					break;
				}
			}
		}
	}
	if(recoveredTrackInValues){
		saveTrayInfo(1);
		memoMainLineAdd("[LOCAL TARGET] Recovered LotId/Grade/NGCode from TrackIn by CellId.");
	}
	return 1;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClearTargetTrayInfo()
{
	//* 불량트레이 관리
	AnsiString trayId = targetTrayInfoActiveId;
	if(trayId.IsEmpty()) trayId = pTrayid_target->Caption.Trim();
	if(!trayId.IsEmpty()){
		AnsiString file = GetTargetTrayInfoFile(trayId);
		if(FileExists(file)) DeleteFile(file);
		memoMainLineAdd("[LOCAL TARGET] Deleted after confirmed Target Tray Out: " + trayId);
	}
	ResetTargetTraySaveInfo("");
	targetTrayInfoActiveId = "";
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::checkTrayInfo(int index)
{
	if(index == 0)
	{
		if(m_saveTrayInfo[0].LOT_ID == AnsiString(pTrayid_source->Caption))
		{
			for(int i = 0; i < tray_source.SLOT_COUNT; i++)
			{
				if(m_saveTrayInfo[0].SLOT_POSITION[i] != tray_source.SLOT_POSITION[i]
				   || m_saveTrayInfo[0].SLOT_ID[i] != tray_source.SLOT_ID[i]
				   || m_saveTrayInfo[0].CELL_EXIST[i] != tray_source.CELL_EXIST[i]
				   || m_saveTrayInfo[0].PICK[i] != tray_source.PICK[i])
				{
					return false;
				}
			}
		}
	}
	else if(index == 1)
	{
		if(m_saveTrayInfo[1].LOT_ID == AnsiString(pTrayid_target->Caption))
		{
			for(int i = 0; i < tray_target.SLOT_COUNT; i++)
			{
				if(m_saveTrayInfo[1].SLOT_POSITION[i] != tray_target.SLOT_POSITION[i]
				   || m_saveTrayInfo[1].SLOT_ID[i] != tray_target.SLOT_ID[i]
				   || m_saveTrayInfo[1].CELL_EXIST[i] != tray_target.CELL_EXIST[i]
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
