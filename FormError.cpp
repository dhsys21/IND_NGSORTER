//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TErrorForm *ErrorForm;
//---------------------------------------------------------------------------
static AnsiString WrapCommonErrorDetail(const AnsiString &text)
{
	// Keep the common alarm detail within two visible lines. Respect text that
	// already contains an explicit line break; otherwise break near 52 chars.
	if(text.Length() <= 52 || text.Pos("\r\n") > 0 || text.Pos("\n") > 0)
		return text;

	int split = 52;
	while(split > 26 && text[split] != ' ')
		--split;
	if(split <= 26){
		split = 53;
		while(split <= text.Length() && text[split] != ' ')
			++split;
	}
	if(split > text.Length())
		return text;

	return text.SubString(1, split - 1).Trim() + "\r\n" +
		text.SubString(split + 1, text.Length() - split).Trim();
}
//---------------------------------------------------------------------------
__fastcall TErrorForm::TErrorForm(TComponent* Owner)
	: TForm(Owner)
{
	this->Parent = BaseForm;
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm::ShowError(AnsiString MainStr, AnsiString SubStr, AnsiString SubStr2)
{
	if(MesOpc != NULL) MesOpc->SetLocalAlarm(NGSorterErrors::Sequence,true);
	// Common automatic-sequence errors must stop motion immediately. Closing
	// this popup does not release Pause; the existing FormMain Restart button
	// restores the saved gripper/robot steps after the operator corrects the cause.
	if(gripper != NULL)
		gripper->req_Pause(true);
	if(robostar != NULL)
		robostar->req_Pause(true);

	MainErr->Caption = MainStr;
	pTrayId->Caption = SubStr2;
	pTrayId->Visible = !SubStr2.IsEmpty();
	SubErr->Top = pTrayId->Visible ? 174 : 125;
	SubErr->Caption = WrapCommonErrorDetail(SubStr);
	FormStyle = fsStayOnTop;
	if(MainForm != NULL)
		MainForm->BuzzerOn(true);

	// MainForm is another child of BaseForm. Show first, then always raise this
	// form so a previous Visible=true state cannot leave the error behind MainForm.
	this->Visible = true;
	this->BringToFront();
	if(this->CanFocus())
		this->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall ShowCommonError(AnsiString MainStr, AnsiString SubStr1, AnsiString SubStr2)
{
	// Most calls come from UI timers. Keep a defensive lazy-create path so a
	// startup ordering change cannot turn ErrorForm->ShowError() into a NULL call.
	if(ErrorForm == NULL && Application != NULL && BaseForm != NULL)
		Application->CreateForm(__classid(TErrorForm), &ErrorForm);

	AnsiString Detail = SubStr1;
	if(!SubStr2.IsEmpty()){
		if(!Detail.IsEmpty())
			Detail += " / ";
		Detail += SubStr2;
	}
	if(MainForm != NULL){
		MainForm->WriteErrorLog(MainStr, Detail);
		AnsiString DisplayMessage = "[ERROR] " + MainStr;
		if(!Detail.IsEmpty())
			DisplayMessage += " : " + Detail;
		MainForm->memoMainLineAdd(DisplayMessage);
	}

	if(ErrorForm != NULL){
		ErrorForm->ShowError(MainStr, SubStr1, SubStr2);
		return;
	}

	// Last-resort message when form creation is unavailable.
	ShowMessage(MainStr + "\n" + SubStr1 + "\n" + SubStr2);
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm::manualBtnClick(TObject *Sender)
{
	// Explicit acknowledgement of a generic sequence dialog; hardware and FMS
	// alarms have separate owners and are not cleared by this button.
	if(MesOpc != NULL) MesOpc->SetLocalAlarm(NGSorterErrors::Sequence,false);
	if(MainForm != NULL)
		MainForm->BuzzerOn(false);
    this->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm::buzzerStopBtnClick(TObject *Sender)
{
	// Silence only the buzzer. Keep the alarm visible and all Pause states intact.
	if(MainForm != NULL)
		MainForm->BuzzerOn(false);
}
//---------------------------------------------------------------------------

