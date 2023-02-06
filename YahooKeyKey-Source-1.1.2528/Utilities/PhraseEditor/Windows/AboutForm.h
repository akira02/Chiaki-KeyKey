#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace PhraseEditor {

/// <summary>
/// Summary for AboutForm
///
/// WARNING: If you change the name of this class, you will need to change the
///          'Resource File Name' property for the managed resource compiler
///          tool associated with all .resx files this class depends on.
///          Otherwise, the designers will not be able to interact properly with
///          localized resources associated with this form.
/// </summary>
public
ref class AboutForm : public System::Windows::Forms::Form {
 public:
  AboutForm(void) { InitializeComponent(); }

 protected:
  /// <summary>
  /// Clean up any resources being used.
  /// </summary>
  ~AboutForm() {
    if (components) {
      delete components;
    }
  }

 private:
  System::Windows::Forms::Button ^ u_okButton;

 private:
  System::Windows::Forms::Label ^ u_aboutLabel;

 private:
  System::Windows::Forms::Label ^ u_copyrightLabel;

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
    System::ComponentModel::ComponentResourceManager ^ resources =
        (gcnew System::ComponentModel::ComponentResourceManager(
            AboutForm::typeid));
    this->u_okButton = (gcnew System::Windows::Forms::Button());
    this->u_aboutLabel = (gcnew System::Windows::Forms::Label());
    this->u_copyrightLabel = (gcnew System::Windows::Forms::Label());
    this->SuspendLayout();
    //
    // u_okButton
    //
    resources->ApplyResources(this->u_okButton, L"u_okButton");
    this->u_okButton->Name = L"u_okButton";
    this->u_okButton->UseVisualStyleBackColor = true;
    this->u_okButton->Click +=
        gcnew System::EventHandler(this, &AboutForm::OKAction);
    //
    // u_aboutLabel
    //
    resources->ApplyResources(this->u_aboutLabel, L"u_aboutLabel");
    this->u_aboutLabel->Name = L"u_aboutLabel";
    //
    // u_copyrightLabel
    //
    resources->ApplyResources(this->u_copyrightLabel, L"u_copyrightLabel");
    this->u_copyrightLabel->Name = L"u_copyrightLabel";
    //
    // AboutForm
    //
    resources->ApplyResources(this, L"$this");
    this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
    this->Controls->Add(this->u_copyrightLabel);
    this->Controls->Add(this->u_aboutLabel);
    this->Controls->Add(this->u_okButton);
    this->FormBorderStyle =
        System::Windows::Forms::FormBorderStyle::FixedDialog;
    this->MaximizeBox = false;
    this->MinimizeBox = false;
    this->Name = L"AboutForm";
    this->ShowIcon = false;
    this->ShowInTaskbar = false;
    this->ResumeLayout(false);
    this->PerformLayout();
  }
#pragma endregion
 private:
  System::Void OKAction(System::Object ^ sender, System::EventArgs ^ e) {
    this->Close();
  }
};
}  // namespace PhraseEditor
