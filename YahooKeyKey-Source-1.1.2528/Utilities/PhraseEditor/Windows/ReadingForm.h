#pragma once

namespace PhraseEditor {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

/// <summary>
/// Summary for ReadingForm
///
/// WARNING: If you change the name of this class, you will need to change the
///          'Resource File Name' property for the managed resource compiler
///          tool associated with all .resx files this class depends on.
///          Otherwise, the designers will not be able to interact properly with
///          localized resources associated with this form.
/// </summary>
public
ref class ReadingForm : public System::Windows::Forms::Form {
 public:
  ReadingForm(void) {
    InitializeComponent();
    ControlCount = 0;
  }

 protected:
  /// <summary>
  /// Clean up any resources being used.
  /// </summary>
  ~ReadingForm() {
    if (components) {
      delete components;
    }
  }

 private:
  System::Windows::Forms::Button ^ u_okButton;

 private:
  System::Windows::Forms::Button ^ u_cancelButton;

 private:
  System::Windows::Forms::Panel ^ u_readingPanel;

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
            ReadingForm::typeid));
    this->u_okButton = (gcnew System::Windows::Forms::Button());
    this->u_cancelButton = (gcnew System::Windows::Forms::Button());
    this->u_readingPanel = (gcnew System::Windows::Forms::Panel());
    this->SuspendLayout();
    //
    // u_okButton
    //
    this->u_okButton->AccessibleDescription = nullptr;
    this->u_okButton->AccessibleName = nullptr;
    resources->ApplyResources(this->u_okButton, L"u_okButton");
    this->u_okButton->BackgroundImage = nullptr;
    this->u_okButton->Font = nullptr;
    this->u_okButton->Name = L"u_okButton";
    this->u_okButton->UseVisualStyleBackColor = true;
    this->u_okButton->Click +=
        gcnew System::EventHandler(this, &ReadingForm::OKAction);
    //
    // u_cancelButton
    //
    this->u_cancelButton->AccessibleDescription = nullptr;
    this->u_cancelButton->AccessibleName = nullptr;
    resources->ApplyResources(this->u_cancelButton, L"u_cancelButton");
    this->u_cancelButton->BackgroundImage = nullptr;
    this->u_cancelButton->DialogResult =
        System::Windows::Forms::DialogResult::Cancel;
    this->u_cancelButton->Font = nullptr;
    this->u_cancelButton->Name = L"u_cancelButton";
    this->u_cancelButton->UseVisualStyleBackColor = true;
    this->u_cancelButton->Click +=
        gcnew System::EventHandler(this, &ReadingForm::CancelAction);
    //
    // u_readingPanel
    //
    this->u_readingPanel->AccessibleDescription = nullptr;
    this->u_readingPanel->AccessibleName = nullptr;
    resources->ApplyResources(this->u_readingPanel, L"u_readingPanel");
    this->u_readingPanel->BackgroundImage = nullptr;
    this->u_readingPanel->Font = nullptr;
    this->u_readingPanel->Name = L"u_readingPanel";
    //
    // ReadingForm
    //
    this->AcceptButton = this->u_okButton;
    this->AccessibleDescription = nullptr;
    this->AccessibleName = nullptr;
    resources->ApplyResources(this, L"$this");
    this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
    this->BackgroundImage = nullptr;
    this->CancelButton = this->u_cancelButton;
    this->Controls->Add(this->u_readingPanel);
    this->Controls->Add(this->u_cancelButton);
    this->Controls->Add(this->u_okButton);
    this->Font = nullptr;
    this->FormBorderStyle =
        System::Windows::Forms::FormBorderStyle::FixedDialog;
    this->Icon = nullptr;
    this->MaximizeBox = false;
    this->MinimizeBox = false;
    this->Name = L"ReadingForm";
    this->ShowIcon = false;
    this->ShowInTaskbar = false;
    this->ResumeLayout(false);
  }
#pragma endregion
 public:
  int ControlCount;
  System::Void AddControl(String ^ string, List<String ^> ^ readings,
                          String ^ currentReading) {
    Label ^ aLabel = gcnew Label();
    aLabel->Text = string;
    aLabel->ClientSize = Drawing::Size(40, 20);
    aLabel->Location = Drawing::Point(0, ControlCount * 25 + 5);

    ComboBox ^ comboBox = gcnew ComboBox();
    comboBox->ClientSize = Drawing::Size(120, 20);
    comboBox->Location = Drawing::Point(40, ControlCount * 25);
    comboBox->Items->AddRange(readings->ToArray());
    comboBox->Text = currentReading;
    comboBox->DropDownStyle = ComboBoxStyle::DropDownList;

    this->u_readingPanel->Controls->Add(aLabel);
    this->u_readingPanel->Controls->Add(comboBox);
    this->ControlCount++;
  }
  System::Void SetRow(DataGridViewRow ^ row) { m_row = row; }

 private:
  DataGridViewRow ^ m_row;

  System::Void CancelAction(System::Object ^ sender, System::EventArgs ^ e) {
    this->Close();
  }
  System::Void OKAction(System::Object ^ sender, System::EventArgs ^ e) {
    String ^ reading = L"";
    int count = u_readingPanel->Controls->Count;
    int i = 0;
    for each (Object ^ object in this->u_readingPanel->Controls) {
      ComboBox ^ comboBox = dynamic_cast<ComboBox ^>(object);
      if (comboBox != nullptr) {
        String ^ text = comboBox->Text;
        reading += text;
        if (i < ControlCount - 1) {
          reading += ",";
        }
        i++;
      }
    }
    if (m_row) m_row->Cells[1]->Value = reading;
    this->Close();
  }
};
}
