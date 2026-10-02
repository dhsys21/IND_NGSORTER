#ifndef FormAccessGuideH
#define FormAccessGuideH
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Forms.hpp>

class TAccessGuideForm : public TForm
{
__published:
    TPanel *pnlPermissions;
    TLabel *lblTitle;
    TStringGrid *gridPermissions;
    TLabel *lblNotes;
    TButton *btnClose;
private:
public:
    __fastcall TAccessGuideForm(TComponent *Owner);
    void Prepare();
};
extern PACKAGE TAccessGuideForm *AccessGuideForm;
#endif
