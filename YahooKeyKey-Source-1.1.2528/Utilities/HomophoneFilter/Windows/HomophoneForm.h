#pragma once
#define SUPPRESS_TAKAO_PREFERENCE
#include "PhraseDBService.h"
#include "ProgressForm.h"

namespace HomophoneFilter {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::IO;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Threading;
using namespace PhraseEditor;

/// <summary>
/// The main window form of the HomophoneFilter utility.
/// </summary>
public
ref class HomophoneForm : public System::Windows::Forms::Form {
 public:
  HomophoneForm(void) {
    InitializeComponent();
    this->currentText = String::Empty;
    this->isHandling = false;
    this->progressForm = gcnew ProgressForm();
    this->progressForm->Owner = this;
  }

 protected:
  /// <summary>
  /// Clean up any resources being used.
  /// </summary>
  ~HomophoneForm() {
    if (components) {
      delete components;
    }
  }

 private:
  System::Windows::Forms::DataGridView ^ u_dataGridView;

 private:
  System::Windows::Forms::BindingSource ^ homophoneBindingSource;

 private:
  System::Windows::Forms::BindingSource ^ m_bindingSource;

 private:
  System::Data::DataSet ^ m_dataSet;

 private:
  System::Data::DataTable ^ homophone;

 private:
  System::Data::DataColumn ^ dataColumn1;

 private:
  System::Data::DataColumn ^ dataColumn2;

 private:
  System::Windows::Forms::SplitContainer ^ u_splitContainer;

 private:
  System::Windows::Forms::Button ^ u_findButton;

 private:
  System::Windows::Forms::RichTextBox ^ u_textBox;

 private:
  System::Windows::Forms::StatusStrip ^ u_statusStrip;

 private:
  System::Windows::Forms::MenuStrip ^ u_menuStrip;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_fileToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_exitToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_editToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_undoToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_redoToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_cutToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_copyToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_pasteToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripSeparator ^ u_toolStripSeparator1;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_saveResultToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripSeparator ^ toolStripSeparator1;

 private:
  System::Windows::Forms::DataGridViewTextBoxColumn ^
      readingDataGridViewTextBoxColumn;

 private:
  System::Windows::Forms::DataGridViewTextBoxColumn ^
      phrasesDataGridViewTextBoxColumn;

 private:
  System::Windows::Forms::SaveFileDialog ^ u_saveFileDialog;

 private:
  System::ComponentModel::IContainer ^ components;

 private:
  /// <summary>
  /// Required designer variable.
  /// </summary>

#pragma region Windows Form Designer generated code
  /// <summary>
  /// Required method for Designer support - do not modify
  /// the contents of this method with the code editor.
  /// </summary>
  void InitializeComponent(void) {
    this->components = (gcnew System::ComponentModel::Container());
    this->u_dataGridView = (gcnew System::Windows::Forms::DataGridView());
    this->readingDataGridViewTextBoxColumn =
        (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    this->phrasesDataGridViewTextBoxColumn =
        (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    this->homophoneBindingSource =
        (gcnew System::Windows::Forms::BindingSource(this->components));
    this->m_bindingSource =
        (gcnew System::Windows::Forms::BindingSource(this->components));
    this->m_dataSet = (gcnew System::Data::DataSet());
    this->homophone = (gcnew System::Data::DataTable());
    this->dataColumn1 = (gcnew System::Data::DataColumn());
    this->dataColumn2 = (gcnew System::Data::DataColumn());
    this->u_splitContainer = (gcnew System::Windows::Forms::SplitContainer());
    this->u_textBox = (gcnew System::Windows::Forms::RichTextBox());
    this->u_findButton = (gcnew System::Windows::Forms::Button());
    this->u_statusStrip = (gcnew System::Windows::Forms::StatusStrip());
    this->u_menuStrip = (gcnew System::Windows::Forms::MenuStrip());
    this->u_fileToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_saveResultToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->toolStripSeparator1 =
        (gcnew System::Windows::Forms::ToolStripSeparator());
    this->u_exitToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_editToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_undoToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_redoToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_toolStripSeparator1 =
        (gcnew System::Windows::Forms::ToolStripSeparator());
    this->u_cutToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_copyToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_pasteToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_saveFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->u_dataGridView))
        ->BeginInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->homophoneBindingSource))
        ->BeginInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->m_bindingSource))
        ->BeginInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->m_dataSet))
        ->BeginInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->homophone))
        ->BeginInit();
    this->u_splitContainer->Panel1->SuspendLayout();
    this->u_splitContainer->Panel2->SuspendLayout();
    this->u_splitContainer->SuspendLayout();
    this->u_menuStrip->SuspendLayout();
    this->SuspendLayout();
    //
    // u_dataGridView
    //
    this->u_dataGridView->AutoGenerateColumns = false;
    this->u_dataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::
        DataGridViewColumnHeadersHeightSizeMode::AutoSize;
    this->u_dataGridView->Columns->AddRange(
        gcnew cli::array<System::Windows::Forms::DataGridViewColumn ^>(2){
            this->readingDataGridViewTextBoxColumn,
            this->phrasesDataGridViewTextBoxColumn});
    this->u_dataGridView->DataSource = this->homophoneBindingSource;
    this->u_dataGridView->Dock = System::Windows::Forms::DockStyle::Fill;
    this->u_dataGridView->Location = System::Drawing::Point(0, 0);
    this->u_dataGridView->Name = L"u_dataGridView";
    this->u_dataGridView->Size = System::Drawing::Size(561, 408);
    this->u_dataGridView->TabIndex = 1;
    //
    // readingDataGridViewTextBoxColumn
    //
    this->readingDataGridViewTextBoxColumn->DataPropertyName = L"reading";
    this->readingDataGridViewTextBoxColumn->HeaderText = L"reading";
    this->readingDataGridViewTextBoxColumn->Name =
        L"readingDataGridViewTextBoxColumn";
    this->readingDataGridViewTextBoxColumn->ReadOnly = true;
    this->readingDataGridViewTextBoxColumn->SortMode =
        System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
    this->readingDataGridViewTextBoxColumn->Width = 200;
    //
    // phrasesDataGridViewTextBoxColumn
    //
    this->phrasesDataGridViewTextBoxColumn->AutoSizeMode =
        System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
    this->phrasesDataGridViewTextBoxColumn->DataPropertyName = L"phrases";
    this->phrasesDataGridViewTextBoxColumn->HeaderText = L"phrases";
    this->phrasesDataGridViewTextBoxColumn->Name =
        L"phrasesDataGridViewTextBoxColumn";
    this->phrasesDataGridViewTextBoxColumn->ReadOnly = true;
    this->phrasesDataGridViewTextBoxColumn->SortMode =
        System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
    //
    // homophoneBindingSource
    //
    this->homophoneBindingSource->DataMember = L"Homophone";
    this->homophoneBindingSource->DataSource = this->m_bindingSource;
    //
    // m_bindingSource
    //
    this->m_bindingSource->DataSource = this->m_dataSet;
    this->m_bindingSource->Position = 0;
    //
    // m_dataSet
    //
    this->m_dataSet->DataSetName = L"NewDataSet";
    this->m_dataSet->Tables->AddRange(
        gcnew cli::array<System::Data::DataTable ^>(1){this->homophone});
    //
    // homophone
    //
    this->homophone->Columns->AddRange(
        gcnew cli::array<System::Data::DataColumn ^>(2){this->dataColumn1,
                                                        this->dataColumn2});
    this->homophone->TableName = L"Homophone";
    //
    // dataColumn1
    //
    this->dataColumn1->ColumnName = L"reading";
    //
    // dataColumn2
    //
    this->dataColumn2->Caption = L"phrases";
    this->dataColumn2->ColumnName = L"phrases";
    //
    // u_splitContainer
    //
    this->u_splitContainer->Dock = System::Windows::Forms::DockStyle::Fill;
    this->u_splitContainer->Location = System::Drawing::Point(0, 24);
    this->u_splitContainer->Name = L"u_splitContainer";
    //
    // u_splitContainer.Panel1
    //
    this->u_splitContainer->Panel1->Controls->Add(this->u_textBox);
    this->u_splitContainer->Panel1->Controls->Add(this->u_findButton);
    //
    // u_splitContainer.Panel2
    //
    this->u_splitContainer->Panel2->Controls->Add(this->u_dataGridView);
    this->u_splitContainer->Size = System::Drawing::Size(844, 408);
    this->u_splitContainer->SplitterDistance = 279;
    this->u_splitContainer->TabIndex = 2;
    this->u_splitContainer->TabStop = false;
    //
    // u_textBox
    //
    this->u_textBox->Dock = System::Windows::Forms::DockStyle::Fill;
    this->u_textBox->Font = (gcnew System::Drawing::Font(
        L"MingLiU", 11.25F, System::Drawing::FontStyle::Regular,
        System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(136)));
    this->u_textBox->Location = System::Drawing::Point(0, 0);
    this->u_textBox->Name = L"u_textBox";
    this->u_textBox->Size = System::Drawing::Size(279, 385);
    this->u_textBox->TabIndex = 2;
    this->u_textBox->Text = L"";
    //
    // u_findButton
    //
    this->u_findButton->Dock = System::Windows::Forms::DockStyle::Bottom;
    this->u_findButton->Location = System::Drawing::Point(0, 385);
    this->u_findButton->Name = L"u_findButton";
    this->u_findButton->Size = System::Drawing::Size(279, 23);
    this->u_findButton->TabIndex = 1;
    this->u_findButton->Text = L"Find";
    this->u_findButton->UseVisualStyleBackColor = true;
    this->u_findButton->Click +=
        gcnew System::EventHandler(this, &HomophoneForm::FindAction);
    //
    // u_statusStrip
    //
    this->u_statusStrip->Location = System::Drawing::Point(0, 432);
    this->u_statusStrip->Name = L"u_statusStrip";
    this->u_statusStrip->Size = System::Drawing::Size(844, 22);
    this->u_statusStrip->TabIndex = 3;
    this->u_statusStrip->Text = L"u_statusStrip";
    //
    // u_menuStrip
    //
    this->u_menuStrip->Items->AddRange(
        gcnew cli::array<System::Windows::Forms::ToolStripItem ^>(2){
            this->u_fileToolStripMenuItem, this->u_editToolStripMenuItem});
    this->u_menuStrip->Location = System::Drawing::Point(0, 0);
    this->u_menuStrip->Name = L"u_menuStrip";
    this->u_menuStrip->Size = System::Drawing::Size(844, 24);
    this->u_menuStrip->TabIndex = 2;
    this->u_menuStrip->Text = L"u_menuStrip";
    //
    // u_fileToolStripMenuItem
    //
    this->u_fileToolStripMenuItem->DropDownItems->AddRange(
        gcnew cli::array<System::Windows::Forms::ToolStripItem ^>(3){
            this->u_saveResultToolStripMenuItem, this->toolStripSeparator1,
            this->u_exitToolStripMenuItem});
    this->u_fileToolStripMenuItem->Name = L"u_fileToolStripMenuItem";
    this->u_fileToolStripMenuItem->Size = System::Drawing::Size(35, 20);
    this->u_fileToolStripMenuItem->Text = L"&File";
    //
    // u_saveResultToolStripMenuItem
    //
    this->u_saveResultToolStripMenuItem->Name =
        L"u_saveResultToolStripMenuItem";
    this->u_saveResultToolStripMenuItem->Size = System::Drawing::Size(152, 22);
    this->u_saveResultToolStripMenuItem->Text = L"&Save Result..";
    this->u_saveResultToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &HomophoneForm::SaveAction);
    //
    // toolStripSeparator1
    //
    this->toolStripSeparator1->Name = L"toolStripSeparator1";
    this->toolStripSeparator1->Size = System::Drawing::Size(149, 6);
    //
    // u_exitToolStripMenuItem
    //
    this->u_exitToolStripMenuItem->Name = L"u_exitToolStripMenuItem";
    this->u_exitToolStripMenuItem->Size = System::Drawing::Size(152, 22);
    this->u_exitToolStripMenuItem->Text = L"E&xit";
    this->u_exitToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &HomophoneForm::ExitAction);
    //
    // u_editToolStripMenuItem
    //
    this->u_editToolStripMenuItem->DropDownItems->AddRange(
        gcnew cli::array<System::Windows::Forms::ToolStripItem ^>(6){
            this->u_undoToolStripMenuItem, this->u_redoToolStripMenuItem,
            this->u_toolStripSeparator1, this->u_cutToolStripMenuItem,
            this->u_copyToolStripMenuItem, this->u_pasteToolStripMenuItem});
    this->u_editToolStripMenuItem->Name = L"u_editToolStripMenuItem";
    this->u_editToolStripMenuItem->Size = System::Drawing::Size(37, 20);
    this->u_editToolStripMenuItem->Text = L"&Edit";
    //
    // u_undoToolStripMenuItem
    //
    this->u_undoToolStripMenuItem->Name = L"u_undoToolStripMenuItem";
    this->u_undoToolStripMenuItem->ShortcutKeys =
        static_cast<System::Windows::Forms::Keys>(
            (System::Windows::Forms::Keys::Control |
             System::Windows::Forms::Keys::Z));
    this->u_undoToolStripMenuItem->Size = System::Drawing::Size(139, 22);
    this->u_undoToolStripMenuItem->Text = L"&Undo";
    this->u_undoToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &HomophoneForm::UndoAction);
    //
    // u_redoToolStripMenuItem
    //
    this->u_redoToolStripMenuItem->Name = L"u_redoToolStripMenuItem";
    this->u_redoToolStripMenuItem->ShortcutKeys =
        static_cast<System::Windows::Forms::Keys>(
            (System::Windows::Forms::Keys::Control |
             System::Windows::Forms::Keys::Y));
    this->u_redoToolStripMenuItem->Size = System::Drawing::Size(139, 22);
    this->u_redoToolStripMenuItem->Text = L"&Redo";
    this->u_redoToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &HomophoneForm::RedoAction);
    //
    // u_toolStripSeparator1
    //
    this->u_toolStripSeparator1->Name = L"u_toolStripSeparator1";
    this->u_toolStripSeparator1->Size = System::Drawing::Size(136, 6);
    //
    // u_cutToolStripMenuItem
    //
    this->u_cutToolStripMenuItem->Name = L"u_cutToolStripMenuItem";
    this->u_cutToolStripMenuItem->ShortcutKeys =
        static_cast<System::Windows::Forms::Keys>(
            (System::Windows::Forms::Keys::Control |
             System::Windows::Forms::Keys::X));
    this->u_cutToolStripMenuItem->Size = System::Drawing::Size(139, 22);
    this->u_cutToolStripMenuItem->Text = L"Cu&t";
    this->u_cutToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &HomophoneForm::CutAction);
    //
    // u_copyToolStripMenuItem
    //
    this->u_copyToolStripMenuItem->Name = L"u_copyToolStripMenuItem";
    this->u_copyToolStripMenuItem->ShortcutKeys =
        static_cast<System::Windows::Forms::Keys>(
            (System::Windows::Forms::Keys::Control |
             System::Windows::Forms::Keys::C));
    this->u_copyToolStripMenuItem->Size = System::Drawing::Size(139, 22);
    this->u_copyToolStripMenuItem->Text = L"&Copy";
    this->u_copyToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &HomophoneForm::CopyAction);
    //
    // u_pasteToolStripMenuItem
    //
    this->u_pasteToolStripMenuItem->Name = L"u_pasteToolStripMenuItem";
    this->u_pasteToolStripMenuItem->ShortcutKeys =
        static_cast<System::Windows::Forms::Keys>(
            (System::Windows::Forms::Keys::Control |
             System::Windows::Forms::Keys::V));
    this->u_pasteToolStripMenuItem->Size = System::Drawing::Size(139, 22);
    this->u_pasteToolStripMenuItem->Text = L"&Paste";
    this->u_pasteToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &HomophoneForm::PasteAction);
    //
    // u_saveFileDialog
    //
    this->u_saveFileDialog->FileName = L"data.tsv";
    this->u_saveFileDialog->Title = L"Save as .tsv file...";
    //
    // HomophoneForm
    //
    this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
    this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
    this->ClientSize = System::Drawing::Size(844, 454);
    this->Controls->Add(this->u_splitContainer);
    this->Controls->Add(this->u_menuStrip);
    this->Controls->Add(this->u_statusStrip);
    this->MainMenuStrip = this->u_menuStrip;
    this->Name = L"HomophoneForm";
    this->Text = L"Homophone Filter";
    this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(
        this, &HomophoneForm::HomophoneForm_FormClosed);
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->u_dataGridView))
        ->EndInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->homophoneBindingSource))
        ->EndInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->m_bindingSource))
        ->EndInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->m_dataSet))
        ->EndInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->homophone))
        ->EndInit();
    this->u_splitContainer->Panel1->ResumeLayout(false);
    this->u_splitContainer->Panel2->ResumeLayout(false);
    this->u_splitContainer->ResumeLayout(false);
    this->u_menuStrip->ResumeLayout(false);
    this->u_menuStrip->PerformLayout();
    this->ResumeLayout(false);
    this->PerformLayout();
  }
#pragma endregion
 public:
  System::Void MSG(String ^ theMessage) {
    System::Diagnostics::Debug::WriteLine(theMessage, L"Debug");
  }
  System::Void HandleFiltering() {
    this->m_dataSet->Tables[0]->Rows->Clear();

    array<Char> ^ chars = {'\n'};
    array<String ^> ^ currentTextArray = this->currentText->Split(chars);

    Dictionary<String ^, List<String ^> ^> ^ dictionary =
        gcnew Dictionary<String ^, List<String ^> ^>();

    if (this->progressForm != nullptr)
      this->progressForm->SetMessage(L"Marking readings...");

    int i = 0;
    for (i = 0; i < currentTextArray->Length; i++) {
      if (this->progressForm != nullptr) {
        this->progressForm->SetProgress(currentTextArray->Length, i);
        String ^ progress = String::Format(L"Marking readings... {0}/{1}", i,
                                           currentTextArray->Length);
        this->progressForm->SetMessage(progress);
      }

      String ^ currentLine = currentTextArray[i]->Trim();
      array<Char> ^ space = {' '};
      array<String ^> ^ currentArray = currentLine->Split(space);
      String ^ string;
      if (currentArray->Length > 1) {
        string = this->ValidatedString(currentArray[0]);
      } else {
        string = this->ValidatedString(currentLine);
      }

      if (string->Length > 1) {
        String ^ reading = L"";
        int j = 0;
        for (j = 0; j < string->Length; j++) {
          wchar_t aChar = string[j];
          String ^ s = gcnew String(aChar, 1);
          List<String ^> ^ readings = PhraseDBService::ReadingsForCharacter(s);
          reading += readings[0];
          if (i < string->Length - 1) reading += ", ";
        }

        List<String ^> ^ list;
        dictionary->TryGetValue(reading, list);
        if (list == nullptr) {
          list = gcnew List<String ^>();
          list->Add(string);
          dictionary->Add(reading, list);
        } else {
          if (!list->Contains(string)) list->Add(string);
        }
      }
    }

    if (this->progressForm != nullptr)
      this->progressForm->SetMessage(L"Filtering...");

    i = 0;
    for each (KeyValuePair<String ^, List<String ^> ^> kvp in dictionary) {
      String ^ reading = kvp.Key;
      List<String ^> ^ phrases = kvp.Value;

      if (this->progressForm != nullptr) {
        this->progressForm->SetProgress(dictionary->Count, i);
        String ^ progress =
            String::Format(L"Filtering... {0}/{1}", i, dictionary->Count);
        this->progressForm->SetMessage(progress);
      }

      if (phrases->Count > 1) {
        String ^ phrase = L"";
        int j = 0;
        for (j = 0; j < phrases->Count; j++) {
          phrase += phrases[j];
          if (j < phrases->Count - 1) phrase += ", ";
        }

        DataRow ^ row = this->m_dataSet->Tables[0]->NewRow();
        row[L"reading"] = reading;
        row[L"phrases"] = phrase;
        this->m_dataSet->Tables[0]->Rows->Add(row);
      }
      i++;
    }
    MSG(L"Done");
    this->PerformRefresh();
    this->isHandling = false;
  }

 private:
  bool isHandling;
  String ^ currentText;
  Thread ^ currentFilterThread;
  ProgressForm ^ progressForm;

  System::Void PerformRefresh() {
    if (this->InvokeRequired) {
      this->Invoke(gcnew MethodInvoker(this, &HomophoneForm::PerformRefresh));
      return;
    }
    this->progressForm->Hide();
    this->Refresh();
  }

  System::Void ExitAction(System::Object ^ sender, System::EventArgs ^ e) {
    this->Close();
  }

  System::Void HomophoneForm_FormClosed(
      System::Object ^ sender,
      System::Windows::Forms::FormClosedEventArgs ^ e) {
    if (currentFilterThread != nullptr) currentFilterThread->Abort();
    ;
    Application::Exit();
  }

  System::String ^ ValidatedString(String ^ originalString) {
    if (!originalString) return L"";

    originalString = originalString->Trim();
    int i = 0;
    int j = 0;
    wchar_t set[128];
    for (i = 0; i < originalString->Length; i++) {
      wchar_t aChar = originalString[i];
      if (aChar >= 0x2E80 && aChar < 0xFF00) {
        set[j] = aChar;
        j++;
      }
    }
    set[j++] = '\0';
    String ^ string = gcnew String(set);
    return string;
  } System::Void FindAction(System::Object ^ sender, System::EventArgs ^ e) {
    if (isHandling) return;

    this->isHandling = true;
    this->currentText = this->u_textBox->Text;

    if (!currentText->Length) {
      this->isHandling = false;
      return;
    }

    ThreadStart ^ threadStart =
        gcnew ThreadStart(this, &HomophoneForm::HandleFiltering);
    Thread ^ thread = gcnew Thread(threadStart);
    currentFilterThread = thread;
    thread->Start();

    this->progressForm->ShowDialog();
  }
  System::Void UndoAction(System::Object ^ sender, System::EventArgs ^ e) {
    Control ^ control = this->u_splitContainer->ActiveControl;
    RichTextBox ^ textBox = dynamic_cast<RichTextBox ^>(control);
    if (textBox != nullptr) {
      textBox->Undo();
      return;
    }
  }
  System::Void RedoAction(System::Object ^ sender, System::EventArgs ^ e) {
    Control ^ control = this->u_splitContainer->ActiveControl;
    RichTextBox ^ textBox = dynamic_cast<RichTextBox ^>(control);
    if (textBox != nullptr) {
      textBox->Redo();
      return;
    }
  }
  System::Void CutAction(System::Object ^ sender, System::EventArgs ^ e) {
    Control ^ control = this->u_splitContainer->ActiveControl;
    RichTextBox ^ textBox = dynamic_cast<RichTextBox ^>(control);
    if (textBox != nullptr) {
      textBox->Cut();
      return;
    }
    MessageBox::Show(control->ToString());
  }
  System::Void CopyAction(System::Object ^ sender, System::EventArgs ^ e) {
    Control ^ control = this->u_splitContainer->ActiveControl;
    RichTextBox ^ textBox = dynamic_cast<RichTextBox ^>(control);
    if (textBox != nullptr) {
      textBox->Copy();
      return;
    }
  }
  System::Void PasteAction(System::Object ^ sender, System::EventArgs ^ e) {
    Control ^ control = this->u_splitContainer->ActiveControl;
    RichTextBox ^ textBox = dynamic_cast<RichTextBox ^>(control);
    if (textBox != nullptr) {
      textBox->Paste();
      return;
    }
  }
  System::Void SaveAction(System::Object ^ sender, System::EventArgs ^ e) {
    if (u_saveFileDialog->ShowDialog(this) ==
        System::Windows::Forms::DialogResult::OK) {
      String ^ fileName = u_saveFileDialog->FileName;
      if (fileName == nullptr || !fileName->Length) return;

      StreamWriter ^ fileWriter = nullptr;

      try {
        if (File::Exists(fileName))
          fileWriter = File::AppendText(fileName);
        else
          fileWriter = File::CreateText(fileName);

        for each (DataRow ^ row in this->m_dataSet->Tables[0]->Rows) {
          String ^ reading = (String ^) row[L"reading"];
          String ^ phrases = (String ^) row[L"phrases"];
          phrases = phrases->Replace(L",", L"\t");
          String ^ line = reading + L"\t" + phrases + L"\n";
          fileWriter->Write(line);
        }
      } finally {
        if (fileWriter != nullptr) fileWriter->Close();
      }
    }
  }
};
}
