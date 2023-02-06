#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace HomophoneFilter {

/// <summary>
/// Summary for ProgressForm
/// </summary>
public
ref class ProgressForm : public System::Windows::Forms::Form {
 public:
  ProgressForm(void) { InitializeComponent(); }

 protected:
  /// <summary>
  /// Clean up any resources being used.
  /// </summary>
  ~ProgressForm() {
    if (components) {
      delete components;
    }
  }

 private:
  System::Windows::Forms::Label ^ u_label;

 private:
  System::Windows::Forms::ProgressBar ^ u_progressBar;

 protected:
 private:
  /// <summary>
  /// Required designer variable.
  /// </summary>
  System::ComponentModel::Container ^ components;

#pragma region Windows Form Designer generated code
  /// <summary>
  /// Required method for Designer support - do not modify
  /// the contents of this method with the code editor.
  /// </summary>
  void InitializeComponent(void) {
    this->u_label = (gcnew System::Windows::Forms::Label());
    this->u_progressBar = (gcnew System::Windows::Forms::ProgressBar());
    this->SuspendLayout();
    //
    // u_label
    //
    this->u_label->Location = System::Drawing::Point(12, 30);
    this->u_label->Name = L"u_label";
    this->u_label->Size = System::Drawing::Size(272, 23);
    this->u_label->TabIndex = 0;
    this->u_label->Text = L"Progressing, please wait...";
    this->u_label->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
    //
    // u_progressBar
    //
    this->u_progressBar->Location = System::Drawing::Point(12, 65);
    this->u_progressBar->Name = L"u_progressBar";
    this->u_progressBar->Size = System::Drawing::Size(272, 23);
    this->u_progressBar->TabIndex = 2;
    //
    // ProgressForm
    //
    this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
    this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
    this->ClientSize = System::Drawing::Size(296, 106);
    this->ControlBox = false;
    this->Controls->Add(this->u_progressBar);
    this->Controls->Add(this->u_label);
    this->FormBorderStyle =
        System::Windows::Forms::FormBorderStyle::FixedSingle;
    this->MaximizeBox = false;
    this->MinimizeBox = false;
    this->Name = L"ProgressForm";
    this->ShowIcon = false;
    this->ShowInTaskbar = false;
    this->StartPosition =
        System::Windows::Forms::FormStartPosition::CenterParent;
    this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(
        this, &ProgressForm::ProgressForm_FormClosing);
    this->ResumeLayout(false);
  }
#pragma endregion
 public:
  delegate System::Void SetProgressCallBack(int Max, int Current);
  System::Void SetProgress(int Max, int Current);
  delegate System::Void SetMessageCallBack(String ^ message);
  System::Void SetMessage(String ^ message);

 private:
  System::Void ProgressForm_FormClosing(
      System::Object ^ sender,
      System::Windows::Forms::FormClosingEventArgs ^ e);
};
}  // namespace HomophoneFilter
