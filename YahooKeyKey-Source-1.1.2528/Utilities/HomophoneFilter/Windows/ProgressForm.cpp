#include "ProgressForm.h"

#include "StdAfx.h"

using namespace System::Windows::Forms;
using namespace HomophoneFilter;

System::Void ProgressForm::SetProgress(int Max, int Current) {
  if (this->InvokeRequired) {
    cli::array<Object ^> ^ oa = gcnew cli::array<Object ^>(2);
    oa[0] = Max;
    oa[1] = Current;

    this->Invoke(gcnew SetProgressCallBack(this, &ProgressForm::SetProgress),
                 oa);
    return;
  }
  this->u_progressBar->Maximum = Max;
  this->u_progressBar->Value = Current;
}

System::Void ProgressForm::SetMessage(String ^ message) {
  if (this->InvokeRequired) {
    this->Invoke(gcnew SetMessageCallBack(this, &ProgressForm::SetMessage),
                 message);
    return;
  }
  this->u_label->Text = message;
}

System::Void ProgressForm::ProgressForm_FormClosing(
    System::Object ^ sender, System::Windows::Forms::FormClosingEventArgs ^ e) {
  if (e->CloseReason == System::Windows::Forms::CloseReason::UserClosing) {
    e->Cancel = true;
    this->Hide();
  }
}