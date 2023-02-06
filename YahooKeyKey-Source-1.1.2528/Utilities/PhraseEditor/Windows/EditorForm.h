#pragma once
#include "AboutForm.h"
#include "PhraseDBService.h"
#include "ReadingForm.h"

namespace PhraseEditor {

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Globalization;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::Windows::Forms;

/// <summary>
/// Summary for EditorForm
///
/// WARNING: If you change the name of this class, you will need to change the
///          'Resource File Name' property for the managed resource compiler
///          tool associated with all .resx files this class depends on.
///          Otherwise, the designers will not be able to interact properly with
///          localized resources associated with this form.
/// </summary>
public
ref class EditorForm : public System::Windows::Forms::Form {
 public:
  EditorForm(void) {
    InitializeComponent();
    LoadTable();

    u_cutToolStripMenuItem->Enabled = false;
    u_copyToolStripMenuItem->Enabled = false;
    u_pasteToolStripMenuItem->Enabled = false;
  }

 protected:
  /// <summary>
  /// Clean up any resources being used.
  /// </summary>
  ~EditorForm() {
    if (components) {
      delete components;
    }
  }

 private:
  System::Windows::Forms::MenuStrip ^ u_menu;

 private:
  System::Windows::Forms::ToolStripSeparator ^ toolStripSeparator1;

 private:
  System::Windows::Forms::ToolStripSeparator ^ toolStripSeparator2;

 private:
  System::Windows::Forms::ToolStripSeparator ^ toolStripSeparator3;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_fileToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_addNewPhraseToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_importToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_exportToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_exitToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_editToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_cutToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_copyToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_pasteToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_deleteToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_editPhraseToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_editReadingToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_helpToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_launchHelpToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStripMenuItem ^ u_aboutToolStripMenuItem;

 private:
  System::Windows::Forms::ToolStrip ^ u_toolStrip;

 private:
  System::Windows::Forms::ToolStripButton ^ u_addToolStripButton;

 private:
  System::Windows::Forms::ToolStripButton ^ u_removeToolStripButton;

 private:
  System::Windows::Forms::ToolStripButton ^ u_editPhraseToolStripButton;

 private:
  System::Windows::Forms::ToolStripButton ^ u_editReadingtoolStripButton;

 private:
  System::Windows::Forms::StatusStrip ^ u_statusStrip;

 private:
  System::Windows::Forms::DataGridView ^ u_dataGridView;

 private:
  System::Windows::Forms::ToolStripStatusLabel ^ u_toolStripStatusLabel;

 private:
  System::Windows::Forms::DataGridViewTextBoxColumn ^
      u_phraseDataGridViewTextBoxColumn;

 private:
  System::Windows::Forms::DataGridViewTextBoxColumn ^
      u_readingDataGridViewTextBoxColumn;

 private:
  System::Windows::Forms::OpenFileDialog ^ u_importDialog;

 private:
  System::Windows::Forms::SaveFileDialog ^ u_exportDialog;

 private:
  System::Data::DataColumn ^ readingDataColumn;

 private:
  System::Data::DataColumn ^ phraseDataColumn;

 private:
  System::Data::DataTable ^ phraseTable;

 private:
  System::Data::DataSet ^ m_dataSet;

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
            EditorForm::typeid));
    this->u_menu = (gcnew System::Windows::Forms::MenuStrip());
    this->u_fileToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_addNewPhraseToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->toolStripSeparator3 =
        (gcnew System::Windows::Forms::ToolStripSeparator());
    this->u_importToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_exportToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->toolStripSeparator2 =
        (gcnew System::Windows::Forms::ToolStripSeparator());
    this->u_exitToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_editToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_cutToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_copyToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_pasteToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_deleteToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->toolStripSeparator1 =
        (gcnew System::Windows::Forms::ToolStripSeparator());
    this->u_editPhraseToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_editReadingToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_helpToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_launchHelpToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_aboutToolStripMenuItem =
        (gcnew System::Windows::Forms::ToolStripMenuItem());
    this->u_toolStrip = (gcnew System::Windows::Forms::ToolStrip());
    this->u_addToolStripButton =
        (gcnew System::Windows::Forms::ToolStripButton());
    this->u_removeToolStripButton =
        (gcnew System::Windows::Forms::ToolStripButton());
    this->u_editPhraseToolStripButton =
        (gcnew System::Windows::Forms::ToolStripButton());
    this->u_editReadingtoolStripButton =
        (gcnew System::Windows::Forms::ToolStripButton());
    this->u_dataGridView = (gcnew System::Windows::Forms::DataGridView());
    this->u_phraseDataGridViewTextBoxColumn =
        (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    this->u_readingDataGridViewTextBoxColumn =
        (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
    this->m_dataSet = (gcnew System::Data::DataSet());
    this->phraseTable = (gcnew System::Data::DataTable());
    this->phraseDataColumn = (gcnew System::Data::DataColumn());
    this->readingDataColumn = (gcnew System::Data::DataColumn());
    this->u_statusStrip = (gcnew System::Windows::Forms::StatusStrip());
    this->u_toolStripStatusLabel =
        (gcnew System::Windows::Forms::ToolStripStatusLabel());
    this->u_importDialog = (gcnew System::Windows::Forms::OpenFileDialog());
    this->u_exportDialog = (gcnew System::Windows::Forms::SaveFileDialog());
    this->u_menu->SuspendLayout();
    this->u_toolStrip->SuspendLayout();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->u_dataGridView))
        ->BeginInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->m_dataSet))
        ->BeginInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->phraseTable))
        ->BeginInit();
    this->u_statusStrip->SuspendLayout();
    this->SuspendLayout();
    //
    // u_menu
    //
    this->u_menu->Items->AddRange(
        gcnew cli::array<System::Windows::Forms::ToolStripItem ^>(3){
            this->u_fileToolStripMenuItem, this->u_editToolStripMenuItem,
            this->u_helpToolStripMenuItem});
    resources->ApplyResources(this->u_menu, L"u_menu");
    this->u_menu->Name = L"u_menu";
    //
    // u_fileToolStripMenuItem
    //
    this->u_fileToolStripMenuItem->DropDownItems->AddRange(
        gcnew cli::array<System::Windows::Forms::ToolStripItem ^>(6){
            this->u_addNewPhraseToolStripMenuItem, this->toolStripSeparator3,
            this->u_importToolStripMenuItem, this->u_exportToolStripMenuItem,
            this->toolStripSeparator2, this->u_exitToolStripMenuItem});
    this->u_fileToolStripMenuItem->Name = L"u_fileToolStripMenuItem";
    resources->ApplyResources(this->u_fileToolStripMenuItem,
                              L"u_fileToolStripMenuItem");
    //
    // u_addNewPhraseToolStripMenuItem
    //
    resources->ApplyResources(this->u_addNewPhraseToolStripMenuItem,
                              L"u_addNewPhraseToolStripMenuItem");
    this->u_addNewPhraseToolStripMenuItem->Name =
        L"u_addNewPhraseToolStripMenuItem";
    this->u_addNewPhraseToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::AddPhraseAction);
    //
    // toolStripSeparator3
    //
    this->toolStripSeparator3->Name = L"toolStripSeparator3";
    resources->ApplyResources(this->toolStripSeparator3,
                              L"toolStripSeparator3");
    //
    // u_importToolStripMenuItem
    //
    this->u_importToolStripMenuItem->Name = L"u_importToolStripMenuItem";
    resources->ApplyResources(this->u_importToolStripMenuItem,
                              L"u_importToolStripMenuItem");
    this->u_importToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::ImportAction);
    //
    // u_exportToolStripMenuItem
    //
    this->u_exportToolStripMenuItem->Name = L"u_exportToolStripMenuItem";
    resources->ApplyResources(this->u_exportToolStripMenuItem,
                              L"u_exportToolStripMenuItem");
    this->u_exportToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::ExportAction);
    //
    // toolStripSeparator2
    //
    this->toolStripSeparator2->Name = L"toolStripSeparator2";
    resources->ApplyResources(this->toolStripSeparator2,
                              L"toolStripSeparator2");
    //
    // u_exitToolStripMenuItem
    //
    this->u_exitToolStripMenuItem->Name = L"u_exitToolStripMenuItem";
    resources->ApplyResources(this->u_exitToolStripMenuItem,
                              L"u_exitToolStripMenuItem");
    this->u_exitToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::ExitApplication);
    //
    // u_editToolStripMenuItem
    //
    this->u_editToolStripMenuItem->DropDownItems->AddRange(
        gcnew cli::array<System::Windows::Forms::ToolStripItem ^>(7){
            this->u_cutToolStripMenuItem, this->u_copyToolStripMenuItem,
            this->u_pasteToolStripMenuItem, this->u_deleteToolStripMenuItem,
            this->toolStripSeparator1, this->u_editPhraseToolStripMenuItem,
            this->u_editReadingToolStripMenuItem});
    this->u_editToolStripMenuItem->Name = L"u_editToolStripMenuItem";
    resources->ApplyResources(this->u_editToolStripMenuItem,
                              L"u_editToolStripMenuItem");
    //
    // u_cutToolStripMenuItem
    //
    this->u_cutToolStripMenuItem->Name = L"u_cutToolStripMenuItem";
    resources->ApplyResources(this->u_cutToolStripMenuItem,
                              L"u_cutToolStripMenuItem");
    this->u_cutToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::CutAction);
    //
    // u_copyToolStripMenuItem
    //
    this->u_copyToolStripMenuItem->Name = L"u_copyToolStripMenuItem";
    resources->ApplyResources(this->u_copyToolStripMenuItem,
                              L"u_copyToolStripMenuItem");
    this->u_copyToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::CopyAction);
    //
    // u_pasteToolStripMenuItem
    //
    this->u_pasteToolStripMenuItem->Name = L"u_pasteToolStripMenuItem";
    resources->ApplyResources(this->u_pasteToolStripMenuItem,
                              L"u_pasteToolStripMenuItem");
    this->u_pasteToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::PasteAction);
    //
    // u_deleteToolStripMenuItem
    //
    this->u_deleteToolStripMenuItem->Name = L"u_deleteToolStripMenuItem";
    resources->ApplyResources(this->u_deleteToolStripMenuItem,
                              L"u_deleteToolStripMenuItem");
    this->u_deleteToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::DeleteAction);
    //
    // toolStripSeparator1
    //
    this->toolStripSeparator1->Name = L"toolStripSeparator1";
    resources->ApplyResources(this->toolStripSeparator1,
                              L"toolStripSeparator1");
    //
    // u_editPhraseToolStripMenuItem
    //
    resources->ApplyResources(this->u_editPhraseToolStripMenuItem,
                              L"u_editPhraseToolStripMenuItem");
    this->u_editPhraseToolStripMenuItem->Name =
        L"u_editPhraseToolStripMenuItem";
    this->u_editPhraseToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::EditPhraseAction);
    //
    // u_editReadingToolStripMenuItem
    //
    resources->ApplyResources(this->u_editReadingToolStripMenuItem,
                              L"u_editReadingToolStripMenuItem");
    this->u_editReadingToolStripMenuItem->Name =
        L"u_editReadingToolStripMenuItem";
    this->u_editReadingToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::EditReadingAction);
    //
    // u_helpToolStripMenuItem
    //
    this->u_helpToolStripMenuItem->DropDownItems->AddRange(
        gcnew cli::array<System::Windows::Forms::ToolStripItem ^>(2){
            this->u_launchHelpToolStripMenuItem,
            this->u_aboutToolStripMenuItem});
    this->u_helpToolStripMenuItem->Name = L"u_helpToolStripMenuItem";
    resources->ApplyResources(this->u_helpToolStripMenuItem,
                              L"u_helpToolStripMenuItem");
    //
    // u_launchHelpToolStripMenuItem
    //
    this->u_launchHelpToolStripMenuItem->Name =
        L"u_launchHelpToolStripMenuItem";
    resources->ApplyResources(this->u_launchHelpToolStripMenuItem,
                              L"u_launchHelpToolStripMenuItem");
    this->u_launchHelpToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::LaunchOnlineHelp);
    //
    // u_aboutToolStripMenuItem
    //
    this->u_aboutToolStripMenuItem->Name = L"u_aboutToolStripMenuItem";
    resources->ApplyResources(this->u_aboutToolStripMenuItem,
                              L"u_aboutToolStripMenuItem");
    this->u_aboutToolStripMenuItem->Click +=
        gcnew System::EventHandler(this, &EditorForm::ShowAboutWindow);
    //
    // u_toolStrip
    //
    this->u_toolStrip->Items->AddRange(
        gcnew cli::array<System::Windows::Forms::ToolStripItem ^>(4){
            this->u_addToolStripButton, this->u_removeToolStripButton,
            this->u_editPhraseToolStripButton,
            this->u_editReadingtoolStripButton});
    resources->ApplyResources(this->u_toolStrip, L"u_toolStrip");
    this->u_toolStrip->Name = L"u_toolStrip";
    //
    // u_addToolStripButton
    //
    resources->ApplyResources(this->u_addToolStripButton,
                              L"u_addToolStripButton");
    this->u_addToolStripButton->Name = L"u_addToolStripButton";
    this->u_addToolStripButton->Click +=
        gcnew System::EventHandler(this, &EditorForm::AddPhraseAction);
    //
    // u_removeToolStripButton
    //
    resources->ApplyResources(this->u_removeToolStripButton,
                              L"u_removeToolStripButton");
    this->u_removeToolStripButton->Name = L"u_removeToolStripButton";
    this->u_removeToolStripButton->Click +=
        gcnew System::EventHandler(this, &EditorForm::RemovePhraseAction);
    //
    // u_editPhraseToolStripButton
    //
    resources->ApplyResources(this->u_editPhraseToolStripButton,
                              L"u_editPhraseToolStripButton");
    this->u_editPhraseToolStripButton->Name = L"u_editPhraseToolStripButton";
    this->u_editPhraseToolStripButton->Click +=
        gcnew System::EventHandler(this, &EditorForm::EditPhraseAction);
    //
    // u_editReadingtoolStripButton
    //
    resources->ApplyResources(this->u_editReadingtoolStripButton,
                              L"u_editReadingtoolStripButton");
    this->u_editReadingtoolStripButton->Name = L"u_editReadingtoolStripButton";
    this->u_editReadingtoolStripButton->Click +=
        gcnew System::EventHandler(this, &EditorForm::EditReadingAction);
    //
    // u_dataGridView
    //
    this->u_dataGridView->AllowUserToResizeRows = false;
    this->u_dataGridView->AutoGenerateColumns = false;
    this->u_dataGridView->BackgroundColor =
        System::Drawing::SystemColors::Control;
    this->u_dataGridView->BorderStyle =
        System::Windows::Forms::BorderStyle::None;
    this->u_dataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::
        DataGridViewColumnHeadersHeightSizeMode::AutoSize;
    this->u_dataGridView->Columns->AddRange(
        gcnew cli::array<System::Windows::Forms::DataGridViewColumn ^>(2){
            this->u_phraseDataGridViewTextBoxColumn,
            this->u_readingDataGridViewTextBoxColumn});
    this->u_dataGridView->DataMember = L"phrase";
    this->u_dataGridView->DataSource = this->m_dataSet;
    resources->ApplyResources(this->u_dataGridView, L"u_dataGridView");
    this->u_dataGridView->Name = L"u_dataGridView";
    this->u_dataGridView->RowHeadersWidthSizeMode = System::Windows::Forms::
        DataGridViewRowHeadersWidthSizeMode::DisableResizing;
    this->u_dataGridView->SelectionMode =
        System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
    this->u_dataGridView->CellValueChanged +=
        gcnew System::Windows::Forms::DataGridViewCellEventHandler(
            this, &EditorForm::u_dataGridView_CellValueChanged);
    this->u_dataGridView->UserDeletingRow +=
        gcnew System::Windows::Forms::DataGridViewRowCancelEventHandler(
            this, &EditorForm::u_dataGridView_UserDeletingRow);
    this->u_dataGridView->CellBeginEdit +=
        gcnew System::Windows::Forms::DataGridViewCellCancelEventHandler(
            this, &EditorForm::u_dataGridView_CellBeginEdit);
    this->u_dataGridView->CellDoubleClick +=
        gcnew System::Windows::Forms::DataGridViewCellEventHandler(
            this, &EditorForm::u_dataGridView_CellDoubleClick);
    this->u_dataGridView->CellEndEdit +=
        gcnew System::Windows::Forms::DataGridViewCellEventHandler(
            this, &EditorForm::u_dataGridView_CellEndEdit);
    this->u_dataGridView->SelectionChanged += gcnew System::EventHandler(
        this, &EditorForm::u_dataGridView_SelectionChanged);
    //
    // u_phraseDataGridViewTextBoxColumn
    //
    this->u_phraseDataGridViewTextBoxColumn->AutoSizeMode =
        System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
    this->u_phraseDataGridViewTextBoxColumn->DataPropertyName = L"Phrase";
    this->u_phraseDataGridViewTextBoxColumn->FillWeight = 40;
    resources->ApplyResources(this->u_phraseDataGridViewTextBoxColumn,
                              L"u_phraseDataGridViewTextBoxColumn");
    this->u_phraseDataGridViewTextBoxColumn->MaxInputLength = 7;
    this->u_phraseDataGridViewTextBoxColumn->Name =
        L"u_phraseDataGridViewTextBoxColumn";
    this->u_phraseDataGridViewTextBoxColumn->SortMode =
        System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
    //
    // u_readingDataGridViewTextBoxColumn
    //
    this->u_readingDataGridViewTextBoxColumn->AutoSizeMode =
        System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
    this->u_readingDataGridViewTextBoxColumn->DataPropertyName = L"Reading";
    resources->ApplyResources(this->u_readingDataGridViewTextBoxColumn,
                              L"u_readingDataGridViewTextBoxColumn");
    this->u_readingDataGridViewTextBoxColumn->Name =
        L"u_readingDataGridViewTextBoxColumn";
    this->u_readingDataGridViewTextBoxColumn->ReadOnly = true;
    this->u_readingDataGridViewTextBoxColumn->SortMode =
        System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
    //
    // m_dataSet
    //
    this->m_dataSet->DataSetName = L"PhraseDataSet";
    this->m_dataSet->Tables->AddRange(
        gcnew cli::array<System::Data::DataTable ^>(1){this->phraseTable});
    //
    // phraseTable
    //
    this->phraseTable->Columns->AddRange(
        gcnew cli::array<System::Data::DataColumn ^>(2){
            this->phraseDataColumn, this->readingDataColumn});
    this->phraseTable->TableName = L"phrase";
    //
    // phraseDataColumn
    //
    this->phraseDataColumn->ColumnName = L"Phrase";
    //
    // readingDataColumn
    //
    this->readingDataColumn->ColumnName = L"Reading";
    this->readingDataColumn->DefaultValue = L"";
    //
    // u_statusStrip
    //
    this->u_statusStrip->Items->AddRange(
        gcnew cli::array<System::Windows::Forms::ToolStripItem ^>(1){
            this->u_toolStripStatusLabel});
    resources->ApplyResources(this->u_statusStrip, L"u_statusStrip");
    this->u_statusStrip->Name = L"u_statusStrip";
    //
    // u_toolStripStatusLabel
    //
    this->u_toolStripStatusLabel->Name = L"u_toolStripStatusLabel";
    resources->ApplyResources(this->u_toolStripStatusLabel,
                              L"u_toolStripStatusLabel");
    //
    // u_importDialog
    //
    this->u_importDialog->DefaultExt = L"txt";
    this->u_importDialog->FileName = L"database.txt";
    resources->ApplyResources(this->u_importDialog, L"u_importDialog");
    //
    // u_exportDialog
    //
    this->u_exportDialog->DefaultExt = L"txt";
    this->u_exportDialog->FileName = L"database.txt";
    resources->ApplyResources(this->u_exportDialog, L"u_exportDialog");
    //
    // EditorForm
    //
    resources->ApplyResources(this, L"$this");
    this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
    this->Controls->Add(this->u_statusStrip);
    this->Controls->Add(this->u_dataGridView);
    this->Controls->Add(this->u_toolStrip);
    this->Controls->Add(this->u_menu);
    this->MainMenuStrip = this->u_menu;
    this->Name = L"EditorForm";
    this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(
        this, &EditorForm::EditorForm_FormClosing);
    this->u_menu->ResumeLayout(false);
    this->u_menu->PerformLayout();
    this->u_toolStrip->ResumeLayout(false);
    this->u_toolStrip->PerformLayout();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->u_dataGridView))
        ->EndInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->m_dataSet))
        ->EndInit();
    (cli::safe_cast<System::ComponentModel::ISupportInitialize ^>(
         this->phraseTable))
        ->EndInit();
    this->u_statusStrip->ResumeLayout(false);
    this->u_statusStrip->PerformLayout();
    this->ResumeLayout(false);
    this->PerformLayout();
  }
#pragma endregion
 public:
  static System::Void LaunchOnlineHelp() {
    try {
      String ^ helpURL =
          L"http://tw.widget.yahoo.com/keykey/help/phraseeditor.html";
      Process ^ proccess = gcnew Process();
      proccess->StartInfo->FileName = helpURL;
      proccess->Start();
    } catch (Exception ^ ex) {
    }
  }

  static System::String ^
      ValidatedString(String ^ originalString) {
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
      }

      private : bool m_init;

  System::Void LoadTable() {
    this->m_init = true;
    this->m_dataSet->Tables[0]->Rows->Clear();
    int rows = PhraseDBService::NumberOfRow();
    for (int i = 0; i < rows; i++) {
      KeyValuePair<String ^, String ^> kp = PhraseDBService::DataAtRow(i);
      DataRow ^ row = this->m_dataSet->Tables[0]->NewRow();
      row[L"phrase"] = kp.Key;
      row[L"reading"] = kp.Value;
      this->m_dataSet->Tables[0]->Rows->Add(row);
    }
    this->UpdateStatusMessage();
    this->m_init = false;
  }

  System::Void UpdateStatusMessage() {
    int count = PhraseDBService::NumberOfRow();
    String ^ message = String::Format(L"{0} user phrase(s).", count);
    String ^ locale = CultureInfo::CurrentUICulture->Name;
    if (locale->Equals(L"zh-TW"))
      message = String::Format(
          L"{0} \u7b46\u4f7f\u7528\u8005\u81ea\u5b9a\u8a5e", count);
    else if (locale->Equals(L"zh-CN"))
      message = String::Format(
          L"{0} \u7b14\u4f7f\u7528\u8005\u81ea\u5b9a\u8bcd", count);
    this->u_toolStripStatusLabel->Text = message;
  }

  System::Void OpenReadingWindow(int rowIndex) {
    if (rowIndex >= m_dataSet->Tables[0]->Rows->Count || rowIndex < 0) return;

    DataGridViewRow ^ row = this->u_dataGridView->Rows[rowIndex];
    String ^ phrase = (String ^) row->Cells[0]->Value;
    String ^ currentReading = (String ^) row->Cells[1]->Value;
    array<Char> ^ chars = {','};
    array<String ^> ^ currentReadingArray = currentReading->Split(chars);

    ReadingForm ^ readingForm = gcnew ReadingForm();
    readingForm->SetRow(row);

    for (int i = 0; i < phrase->Length; i++) {
      wchar_t aChar = phrase[i];
      String ^ string = gcnew String(aChar, 1);
      List<String ^> ^ readings = PhraseDBService::ReadingsForCharacter(string);
      readingForm->AddControl(string, readings, currentReadingArray[i]);
    }

    readingForm->ShowDialog();
  }

  // menu action

  System::Void CutAction(System::Object ^ sender, System::EventArgs ^ e) {
    Control ^ control = this->ActiveControl;
    TextBox ^ textBox = dynamic_cast<TextBox ^>(control);
    if (textBox != nullptr) {
      textBox->Cut();
      return;
    }
  }
  System::Void CopyAction(System::Object ^ sender, System::EventArgs ^ e) {
    Control ^ control = this->ActiveControl;
    TextBox ^ textBox = dynamic_cast<TextBox ^>(control);
    if (textBox != nullptr) {
      textBox->Copy();
      return;
    }

    DataGridViewSelectedRowCollection ^ rows =
        this->u_dataGridView->SelectedRows;
    String ^ text = "";
    for each (DataGridViewRow ^ row in rows) {
      DataGridViewCellCollection ^ cells = row->Cells;
      String ^ line = L"";
      for each (DataGridViewCell ^ cell in cells) {
        String ^ text = cell->Value->ToString();
        line += text + " ";
      }
      line = line->TrimEnd() + "\r\n";
      text += line;
    }
    Clipboard::SetText(text);
  }
  System::Void PasteAction(System::Object ^ sender, System::EventArgs ^ e) {
    Control ^ control = this->ActiveControl;
    TextBox ^ textBox = dynamic_cast<TextBox ^>(control);
    if (textBox != nullptr) {
      textBox->Paste();
      return;
    }
  }
  System::Void DeleteAction(System::Object ^ sender, System::EventArgs ^ e) {
    Control ^ control = this->ActiveControl;
    TextBox ^ textBox = dynamic_cast<TextBox ^>(control);
    if (textBox != nullptr) {
      if (textBox->SelectedText->Length) textBox->SelectedText = L"";
      return;
    }
    DataGridView ^ dataGridView = dynamic_cast<DataGridView ^>(control);
    if (dataGridView != nullptr) {
      this->RemovePhraseAction(this, nullptr);
      return;
    }
  }
  System::Void ImportAction(System::Object ^ sender, System::EventArgs ^ e) {
    String ^ currentLocale = CultureInfo::CurrentUICulture->Name;
    if (this->u_importDialog->ShowDialog(this) ==
        System::Windows::Forms::DialogResult::OK) {
      String ^ filename = this->u_importDialog->FileName;
      bool importResult =
          PreferenceConnector::SharedInstance->importPhraseDB(filename);
      if (importResult) {
        this->LoadTable();
        if (currentLocale->Equals(L"zh-TW"))
          MessageBox::Show(
              L"\u8a5e\u5f59\u5df2\u7d93\u6210\u529f\u532f\u5165\u3002",
              L"\u5b8c\u6210", MessageBoxButtons::OK,
              MessageBoxIcon::Information);
        else if (currentLocale->Equals("zh-CN"))
          MessageBox::Show(
              L"\u8bcd\u6c47\u5df2\u7ecf\u6210\u529f\u6c47\u5165\u3002",
              L"\u5b8c\u6210", MessageBoxButtons::OK,
              MessageBoxIcon::Information);
        else
          MessageBox::Show(L"Your phrases are successfully imported", L"Done",
                           MessageBoxButtons::OK, MessageBoxIcon::Information);
      } else {
        if (currentLocale->Equals(L"zh-TW"))
          MessageBox::Show(L"\u8a5e\u5f59\u532f\u5165\u5931\u6557\u3002",
                           L"\u932f\u8aa4", MessageBoxButtons::OK,
                           MessageBoxIcon::Error);
        else if (currentLocale->Equals(L"zh-CN"))
          MessageBox::Show(L"\u8bcd\u6c47\u6c47\u5165\u5931\u8d25\u3002",
                           L"\u9519\u8bef", MessageBoxButtons::OK,
                           MessageBoxIcon::Error);
        else
          MessageBox::Show(L"Your phrases could not be imported.", L"Error!",
                           MessageBoxButtons::OK, MessageBoxIcon::Error);
      }
    }
  }
  System::Void ExportAction(System::Object ^ sender, System::EventArgs ^ e) {
    String ^ currentLocale = CultureInfo::CurrentUICulture->Name;
    if (this->u_exportDialog->ShowDialog(this) ==
        System::Windows::Forms::DialogResult::OK) {
      String ^ filename = this->u_exportDialog->FileName;
      bool exportResult =
          PreferenceConnector::SharedInstance->exportPhraseDB(filename);
      if (exportResult) {
        if (currentLocale->Equals("zh-TW"))
          MessageBox::Show(
              L"\u8a5e\u5f59\u5df2\u7d93\u6210\u529f\u532f\u51fa\u3002",
              L"\u5b8c\u6210", MessageBoxButtons::OK,
              MessageBoxIcon::Information);
        else if (currentLocale->Equals(L"zh-CN"))
          MessageBox::Show(
              L"\u8bcd\u6c47\u5df2\u7ecf\u6210\u529f\u6c47\u51fa\u3002",
              L"\u5b8c\u6210", MessageBoxButtons::OK,
              MessageBoxIcon::Information);
        else
          MessageBox::Show(L"Your phrases are successfully exported", L"Done",
                           MessageBoxButtons::OK, MessageBoxIcon::Information);
      } else {
        if (currentLocale->Equals("zh-TW"))
          MessageBox::Show(L"\u8a5e\u5f59\u532f\u51fa\u5931\u6557\u3002",
                           L"\u932f\u8aa4", MessageBoxButtons::OK,
                           MessageBoxIcon::Error);
        else if (currentLocale->Equals(L"zh-CN"))
          MessageBox::Show(L"\u8bcd\u6c47\u6c47\u51fa\u5931\u8d25\u3002",
                           L"\u9519\u8bef", MessageBoxButtons::OK,
                           MessageBoxIcon::Error);
        else
          MessageBox::Show(L"Your phrases could not be exported.", L"Error!",
                           MessageBoxButtons::OK, MessageBoxIcon::Error);
      }
    }
  }
  System::Void ExitApplication(System::Object ^ sender, System::EventArgs ^ e) {
    this->Close();
  }

  System::Void LaunchOnlineHelp(System::Object ^ sender,
                                System::EventArgs ^ e) {
    ThreadStart ^ threadStart =
        gcnew ThreadStart(&EditorForm::LaunchOnlineHelp);
    Thread ^ thread = gcnew Thread(threadStart);
    thread->Start();
    // this->LaunchOnlineHelp();
  }

  System::Void ShowAboutWindow(System::Object ^ sender, System::EventArgs ^ e) {
    PhraseEditor::AboutForm ^ aboutForm = gcnew AboutForm();
    aboutForm->ShowDialog();
  }

  // Toolbar actions.

  System::Void AddPhraseAction(System::Object ^ sender, System::EventArgs ^ e) {
    int lastRowInedx = this->u_dataGridView->RowCount - 1;
    this->u_dataGridView->CurrentCell =
        u_dataGridView->Rows[lastRowInedx]->Cells[0];
    this->u_dataGridView->BeginEdit(true);
    this->u_removeToolStripButton->Enabled = false;
    this->u_editPhraseToolStripButton->Enabled = false;
    this->u_editReadingtoolStripButton->Enabled = false;
  }
  System::Void RemovePhraseAction(System::Object ^ sender,
                                  System::EventArgs ^ e) {
    DataGridViewSelectedRowCollection ^ rows =
        this->u_dataGridView->SelectedRows;
    int count = rows->Count;
    for (int i = 0; i < count; i++) {
      int index = count - i - 1;
      DataGridViewRow ^ row = rows[index];
      int rowIndex = row->Index;
      this->u_dataGridView->Rows->RemoveAt(rowIndex);
      PhraseDBService::DeleteRow(rowIndex);
    }
    this->UpdateStatusMessage();
  }
  System::Void EditPhraseAction(System::Object ^ sender,
                                System::EventArgs ^ e) {
    DataGridViewRow ^ row = this->u_dataGridView->CurrentRow;
    if (!row) return;

    int currentRowIndex = row->Index;
    if (currentRowIndex < this->m_dataSet->Tables[0]->Rows->Count) {
      this->u_dataGridView->CurrentCell =
          this->u_dataGridView->Rows[currentRowIndex]->Cells[0];
      this->u_dataGridView->BeginEdit(true);
    }
  }
  System::Void EditReadingAction(System::Object ^ sender,
                                 System::EventArgs ^ e) {
    DataGridViewRow ^ row = this->u_dataGridView->CurrentRow;
    if (!row) return;

    int currentRowIndex = row->Index;
    if (currentRowIndex < this->m_dataSet->Tables[0]->Rows->Count) {
      this->u_dataGridView->EndEdit();
      this->OpenReadingWindow(currentRowIndex);
    }
  }

  /// DataGridView Events
  System::Void u_dataGridView_CellValueChanged(
      System::Object ^ sender,
      System::Windows::Forms::DataGridViewCellEventArgs ^ e) {
    if (this->m_init) return;

    String ^ columnName = this->u_dataGridView->Columns[e->ColumnIndex]->Name;

    if (columnName->Equals("u_phraseDataGridViewTextBoxColumn")) {
      int rowIndex = e->RowIndex;
      DataGridViewRow ^ row = u_dataGridView->Rows[rowIndex];
      String ^ originalString = (String ^) row->Cells[0]->Value;
      String ^ phrase = EditorForm::ValidatedString(originalString);
      phrase = EditorForm::ValidatedString(phrase);

      if (!phrase->Length) {
        String ^ message =
            L"\"" + originalString + L"\" is not a valid phrase!";
        String ^ locale = CultureInfo::CurrentUICulture->Name;
        if (locale->Equals(L"zh-TW"))
          message = L"\u300c" + originalString +
                    L"\u300d\u4e0d\u662f\u6b63\u78ba\u7684\u8a5e\u5f59\uff01";
        else if (locale->Equals(L"zh-CN"))
          message = L"\u300c" + originalString +
                    L"\u300d\u4e0d\u662f\u6b63\u786e\u7684\u8bcd\u6c47\uff01";

        MessageBox::Show(this, message, L"", MessageBoxButtons::OK,
                         MessageBoxIcon::Asterisk);

        if (rowIndex >= PhraseDBService::NumberOfRow()) {
          this->u_dataGridView->Rows->RemoveAt(rowIndex);
        } else {
          KeyValuePair<String ^, String ^> kp =
              PhraseDBService::DataAtRow(rowIndex);
          DataGridViewRow ^ row = this->u_dataGridView->Rows[rowIndex];
          row->Cells[0]->Value = kp.Key;
          row->Cells[1]->Value = kp.Value;
        }
      } else {
        if (rowIndex >= PhraseDBService::NumberOfRow())
          PhraseDBService::AddNewRow(phrase);

        PhraseDBService::SetPhraseAtRow(phrase, rowIndex);
        List<String ^> ^ readings =
            PhraseDBService::ReadingsForCharacter(phrase);
        String ^ reading = readings[0];
        DataGridViewRow ^ row = this->u_dataGridView->Rows[rowIndex];
        row->Cells[0]->Value = phrase;
        row->Cells[1]->Value = reading;
      }
    } else if (columnName->Equals("u_readingDataGridViewTextBoxColumn")) {
      int rowIndex = e->RowIndex;
      DataGridViewRow ^ row = this->u_dataGridView->Rows[rowIndex];
      String ^ reading = (String ^) row->Cells[1]->Value;
      PhraseDBService::SetNewReadingForPhraseAtRow(reading, rowIndex);
    }
    this->UpdateStatusMessage();
  }
  System::Void u_dataGridView_CellDoubleClick(
      System::Object ^ sender,
      System::Windows::Forms::DataGridViewCellEventArgs ^ e) {
    int columnIndex = e->ColumnIndex;
    if (columnIndex < 0) return;

    String ^ columnName = this->u_dataGridView->Columns[e->ColumnIndex]->Name;

    if (columnName->Equals("u_readingDataGridViewTextBoxColumn")) {
      int rowIndex = e->RowIndex;
      this->OpenReadingWindow(rowIndex);
    } else if (columnName->Equals("u_readingDataGridViewTextBoxColumn")) {
      int rowIndex = e->RowIndex;
      this->u_dataGridView->CurrentCell =
          this->u_dataGridView->Rows[rowIndex]->Cells[0];
      this->u_dataGridView->BeginEdit(true);
    }
    this->UpdateStatusMessage();
  }
  System::Void u_dataGridView_UserDeletingRow(
      System::Object ^ sender,
      System::Windows::Forms::DataGridViewRowCancelEventArgs ^ e) {
    DataGridViewRow ^ row = e->Row;
    if (!row) return;
    PhraseDBService::DeleteRow(row->Index);
    this->UpdateStatusMessage();
  }
  System::Void u_dataGridView_SelectionChanged(System::Object ^ sender,
                                               System::EventArgs ^ e) {
    if (this->m_init) return;

    DataGridViewRow ^ row = this->u_dataGridView->CurrentRow;
    int currentRowIndex = -1;
    if (row) currentRowIndex = row->Index;

    if (currentRowIndex < this->m_dataSet->Tables[0]->Rows->Count) {
      this->u_removeToolStripButton->Enabled = true;
      this->u_editPhraseToolStripButton->Enabled = true;
      this->u_editReadingtoolStripButton->Enabled = true;
      this->u_editPhraseToolStripMenuItem->Enabled = true;
      this->u_editReadingToolStripMenuItem->Enabled = true;
      this->u_deleteToolStripMenuItem->Enabled = true;
      this->u_copyToolStripMenuItem->Enabled = true;
    } else {
      this->u_removeToolStripButton->Enabled = false;
      this->u_editPhraseToolStripButton->Enabled = false;
      this->u_editReadingtoolStripButton->Enabled = false;
      this->u_editPhraseToolStripMenuItem->Enabled = false;
      this->u_editReadingToolStripMenuItem->Enabled = false;
      this->u_deleteToolStripMenuItem->Enabled = false;
      this->u_copyToolStripMenuItem->Enabled = false;
    }
  }
  System::Void u_dataGridView_CellBeginEdit(
      System::Object ^ sender,
      System::Windows::Forms::DataGridViewCellCancelEventArgs ^ e) {
    if (this->m_init) return;

    DataGridViewRow ^ row = this->u_dataGridView->CurrentRow;
    int currentRowIndex = -1;
    if (row) currentRowIndex = row->Index;

    if (currentRowIndex >= this->m_dataSet->Tables[0]->Rows->Count) {
      this->u_editPhraseToolStripButton->Enabled = false;
      this->u_editReadingtoolStripButton->Enabled = false;
      this->u_editPhraseToolStripMenuItem->Enabled = false;
      this->u_editReadingToolStripMenuItem->Enabled = false;
    }

    this->u_cutToolStripMenuItem->Enabled = true;
    this->u_copyToolStripMenuItem->Enabled = true;
    this->u_pasteToolStripMenuItem->Enabled = true;
    this->u_deleteToolStripMenuItem->Enabled = true;
  }
  System::Void u_dataGridView_CellEndEdit(
      System::Object ^ sender,
      System::Windows::Forms::DataGridViewCellEventArgs ^ e) {
    if (this->m_init) return;

    this->u_cutToolStripMenuItem->Enabled = false;
    // this->u_copyToolStripMenuItem->Enabled = false;
    this->u_pasteToolStripMenuItem->Enabled = false;
  }

  // EditorForm events

  System::Void EditorForm_FormClosing(
      System::Object ^ sender,
      System::Windows::Forms::FormClosingEventArgs ^ e) {
    PhraseDBService::Save();
  }
};
}
