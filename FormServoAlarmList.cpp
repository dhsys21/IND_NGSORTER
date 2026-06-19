//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TServoAlarmListForm *ServoAlarmListForm;
//---------------------------------------------------------------------------
__fastcall TServoAlarmListForm::TServoAlarmListForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TServoAlarmListForm::FormCreate(TObject *Sender)
{
	m_ListView[0] = ListView1;
	m_ListView[1] = ListView2;
	m_ListView[2] = ListView3;
	m_ListView[3] = ListView4;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TServoAlarmListForm::LanguageChange(int index)
{
	TMemo *mm[4];
	if(index == 0)
	{
		mm[0] = Memo_Ko1;
		mm[1] = Memo_Ko2;
		mm[2] = Memo_Ko3;
		mm[3] = Memo_Ko4;
	}
	else if(index == 1)
	{
		mm[0] = Memo_En1;
		mm[1] = Memo_En2;
		mm[2] = Memo_En3;
		mm[3] = Memo_En4;
	}
	else if(index == 2)
	{
		mm[0] = Memo_Hu1;
		mm[1] = Memo_Hu2;
		mm[2] = Memo_Hu3;
		mm[3] = Memo_Hu4;
	}

	for(int i = 0; i < 4; i++)
	{
		m_ListView[i]->Clear();
		m_Length[i] = mm[i]->Lines->Count / 2;
	}

	AnsiString temp = "";
	int maxLength;
	for(int i = 0; i < 4; i++)
	{
		maxLength = 0;
		for( int j = 0; j < m_Length[i]; j++)
		{
			ITEM = m_ListView[i]->Items->Add();
			ITEM->SubItems->Add(mm[i]->Lines->Strings[j]);
			temp = mm[i]->Lines->Strings[j + m_Length[i]];
			ITEM->SubItems->Add(temp);
			if(maxLength < temp.Length()) maxLength = temp.Length();
		}
		m_ListView[i]->Column[2]->Width = maxLength * 8;
	}
}
//---------------------------------------------------------------------------
