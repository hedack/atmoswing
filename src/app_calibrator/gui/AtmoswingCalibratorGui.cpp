///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct  8 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "AtmoswingCalibratorGui.h"

///////////////////////////////////////////////////////////////////////////

asFrameCalibrationVirtual::asFrameCalibrationVirtual( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 600,600 ), wxDefaultSize );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_PanelMain = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxVERTICAL );
	
	m_NotebookBase = new wxNotebook( m_PanelMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_LEFT );
	m_PanelControls = new wxPanel( m_NotebookBase, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_StaticTextMethod = new wxStaticText( m_PanelControls, wxID_ANY, _("Select the calibration method"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextMethod->Wrap( -1 );
	bSizer5->Add( m_StaticTextMethod, 0, wxALL, 5 );
	
	wxString m_ChoiceMethodChoices[] = { _("Single assessment"), _("(Exhaustive exploration)"), _("Classic calibration"), _("Classic+ calibration"), _("Variables exploration Classic+"), _("Evaluate all scores"), _("Only predictand values") };
	int m_ChoiceMethodNChoices = sizeof( m_ChoiceMethodChoices ) / sizeof( wxString );
	m_ChoiceMethod = new wxChoice( m_PanelControls, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ChoiceMethodNChoices, m_ChoiceMethodChoices, 0 );
	m_ChoiceMethod->SetSelection( 0 );
	bSizer5->Add( m_ChoiceMethod, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_StaticTextFileParameters = new wxStaticText( m_PanelControls, wxID_ANY, _("Select the parameters file for the calibration"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextFileParameters->Wrap( -1 );
	bSizer5->Add( m_StaticTextFileParameters, 0, wxALL, 5 );
	
	m_FilePickerParameters = new wxFilePickerCtrl( m_PanelControls, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*.xml"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	bSizer5->Add( m_FilePickerParameters, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_StaticTextFilePredictand = new wxStaticText( m_PanelControls, wxID_ANY, _("Select the predictand DB file"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextFilePredictand->Wrap( -1 );
	bSizer5->Add( m_StaticTextFilePredictand, 0, wxALL, 5 );
	
	m_FilePickerPredictand = new wxFilePickerCtrl( m_PanelControls, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*.nc"), wxDefaultPosition, wxSize( -1,-1 ), wxFLP_DEFAULT_STYLE );
	bSizer5->Add( m_FilePickerPredictand, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_StaticTextPredictorDir = new wxStaticText( m_PanelControls, wxID_ANY, _("Select the predictors directory"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextPredictorDir->Wrap( -1 );
	bSizer5->Add( m_StaticTextPredictorDir, 0, wxALL, 5 );
	
	m_DirPickerPredictor = new wxDirPickerCtrl( m_PanelControls, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE );
	bSizer5->Add( m_DirPickerPredictor, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_StaticTextCalibrationResultsDir = new wxStaticText( m_PanelControls, wxID_ANY, _("Directory to save calibration outputs"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextCalibrationResultsDir->Wrap( -1 );
	bSizer5->Add( m_StaticTextCalibrationResultsDir, 0, wxALL, 5 );
	
	m_DirPickerCalibrationResults = new wxDirPickerCtrl( m_PanelControls, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DEFAULT_STYLE );
	bSizer5->Add( m_DirPickerCalibrationResults, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_CheckBoxParallelEvaluations = new wxCheckBox( m_PanelControls, wxID_ANY, _("Parallel evaluations when possible (competes with multithreading in the processor)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_CheckBoxParallelEvaluations, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer131;
	bSizer131 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer( new wxStaticBox( m_PanelControls, wxID_ANY, _("Intermediate results saving options") ), wxVERTICAL );
	
	wxBoxSizer* bSizer141;
	bSizer141 = new wxBoxSizer( wxHORIZONTAL );
	
	m_StaticTextSaveAnalogDates = new wxStaticText( m_PanelControls, wxID_ANY, _("Analog dates steps:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextSaveAnalogDates->Wrap( -1 );
	bSizer141->Add( m_StaticTextSaveAnalogDates, 0, wxALL, 5 );
	
	m_CheckBoxSaveAnalogDatesStep1 = new wxCheckBox( m_PanelControls, wxID_ANY, _("1"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer141->Add( m_CheckBoxSaveAnalogDatesStep1, 0, wxALL, 5 );
	
	m_CheckBoxSaveAnalogDatesStep2 = new wxCheckBox( m_PanelControls, wxID_ANY, _("2"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer141->Add( m_CheckBoxSaveAnalogDatesStep2, 0, wxALL, 5 );
	
	m_CheckBoxSaveAnalogDatesStep3 = new wxCheckBox( m_PanelControls, wxID_ANY, _("3"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer141->Add( m_CheckBoxSaveAnalogDatesStep3, 0, wxALL, 5 );
	
	m_CheckBoxSaveAnalogDatesStep4 = new wxCheckBox( m_PanelControls, wxID_ANY, _("4"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer141->Add( m_CheckBoxSaveAnalogDatesStep4, 0, wxALL, 5 );
	
	
	sbSizer8->Add( bSizer141, 0, wxEXPAND, 5 );
	
	m_CheckBoxSaveAnalogDatesAllSteps = new wxCheckBox( m_PanelControls, wxID_ANY, _("All analog dates steps (overwrite previous)"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer8->Add( m_CheckBoxSaveAnalogDatesAllSteps, 0, wxALL, 5 );
	
	m_CheckBoxSaveAnalogValues = new wxCheckBox( m_PanelControls, wxID_ANY, _("Analog values"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer8->Add( m_CheckBoxSaveAnalogValues, 0, wxALL, 5 );
	
	m_CheckBoxSaveForecastScores = new wxCheckBox( m_PanelControls, wxID_ANY, _("Forecast scores"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer8->Add( m_CheckBoxSaveForecastScores, 0, wxALL, 5 );
	
	m_CheckBoxSaveFinalForecastScore = new wxCheckBox( m_PanelControls, wxID_ANY, _("Final forecast score"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer8->Add( m_CheckBoxSaveFinalForecastScore, 0, wxALL, 5 );
	
	m_staticText60 = new wxStaticText( m_PanelControls, wxID_ANY, _("Options are always desactivated at initialization !"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText60->Wrap( -1 );
	sbSizer8->Add( m_staticText60, 0, wxALL, 5 );
	
	
	bSizer131->Add( sbSizer8, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer81;
	sbSizer81 = new wxStaticBoxSizer( new wxStaticBox( m_PanelControls, wxID_ANY, _("Intermediate results loading options") ), wxVERTICAL );
	
	wxBoxSizer* bSizer1411;
	bSizer1411 = new wxBoxSizer( wxHORIZONTAL );
	
	m_StaticTextLoadAnalogDates = new wxStaticText( m_PanelControls, wxID_ANY, _("Analog dates steps:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextLoadAnalogDates->Wrap( -1 );
	bSizer1411->Add( m_StaticTextLoadAnalogDates, 0, wxALL, 5 );
	
	m_CheckBoxLoadAnalogDatesStep1 = new wxCheckBox( m_PanelControls, wxID_ANY, _("1"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1411->Add( m_CheckBoxLoadAnalogDatesStep1, 0, wxALL, 5 );
	
	m_CheckBoxLoadAnalogDatesStep2 = new wxCheckBox( m_PanelControls, wxID_ANY, _("2"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1411->Add( m_CheckBoxLoadAnalogDatesStep2, 0, wxALL, 5 );
	
	m_CheckBoxLoadAnalogDatesStep3 = new wxCheckBox( m_PanelControls, wxID_ANY, _("3"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1411->Add( m_CheckBoxLoadAnalogDatesStep3, 0, wxALL, 5 );
	
	m_CheckBoxLoadAnalogDatesStep4 = new wxCheckBox( m_PanelControls, wxID_ANY, _("4"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1411->Add( m_CheckBoxLoadAnalogDatesStep4, 0, wxALL, 5 );
	
	
	sbSizer81->Add( bSizer1411, 0, wxEXPAND, 5 );
	
	m_CheckBoxLoadAnalogDatesAllSteps = new wxCheckBox( m_PanelControls, wxID_ANY, _("All analog dates steps (overwrite previous)"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer81->Add( m_CheckBoxLoadAnalogDatesAllSteps, 0, wxALL, 5 );
	
	m_CheckBoxLoadAnalogValues = new wxCheckBox( m_PanelControls, wxID_ANY, _("Analog values"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer81->Add( m_CheckBoxLoadAnalogValues, 0, wxALL, 5 );
	
	m_CheckBoxLoadForecastScores = new wxCheckBox( m_PanelControls, wxID_ANY, _("Forecast scores"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer81->Add( m_CheckBoxLoadForecastScores, 0, wxALL, 5 );
	
	m_staticText61 = new wxStaticText( m_PanelControls, wxID_ANY, _("Options are always desactivated at initialization !"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText61->Wrap( -1 );
	sbSizer81->Add( m_staticText61, 0, wxALL, 5 );
	
	
	bSizer131->Add( sbSizer81, 1, wxEXPAND|wxALL, 5 );
	
	
	bSizer5->Add( bSizer131, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );
	
	m_StaticTextStateLabel = new wxStaticText( m_PanelControls, wxID_ANY, _("Calibration state: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextStateLabel->Wrap( -1 );
	m_StaticTextStateLabel->Hide();
	
	bSizer23->Add( m_StaticTextStateLabel, 0, wxALL, 5 );
	
	m_StaticTextState = new wxStaticText( m_PanelControls, wxID_ANY, _("Not running."), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextState->Wrap( 350 );
	m_StaticTextState->Hide();
	
	bSizer23->Add( m_StaticTextState, 1, wxALL, 5 );
	
	
	bSizer5->Add( bSizer23, 0, wxEXPAND, 5 );
	
	
	m_PanelControls->SetSizer( bSizer5 );
	m_PanelControls->Layout();
	bSizer5->Fit( m_PanelControls );
	m_NotebookBase->AddPage( m_PanelControls, _("Controls"), true );
	m_PanelOptions = new wxPanel( m_NotebookBase, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxVERTICAL );
	
	m_NotebookOptions = new wxNotebook( m_PanelOptions, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_MULTILINE );
	m_PanelSingle = new wxPanel( m_NotebookOptions, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( m_PanelSingle, wxID_ANY, _("Classic+ calibration") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 5, 2, 0, 0 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_StaticTextClassicPlusStepsLonPertinenceMap = new wxStaticText( m_PanelSingle, wxID_ANY, _("Multiple of the steps in lon for pertinence map"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextClassicPlusStepsLonPertinenceMap->Wrap( -1 );
	fgSizer21->Add( m_StaticTextClassicPlusStepsLonPertinenceMap, 0, wxALL, 5 );
	
	m_TextCtrlClassicPlusStepsLonPertinenceMap = new wxTextCtrl( m_PanelSingle, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_TextCtrlClassicPlusStepsLonPertinenceMap->SetMaxLength( 0 ); 
	fgSizer21->Add( m_TextCtrlClassicPlusStepsLonPertinenceMap, 0, wxRIGHT|wxLEFT, 5 );
	
	m_StaticTextClassicPlusStepsLatPertinenceMap = new wxStaticText( m_PanelSingle, wxID_ANY, _("Multiple of the steps in lat for pertinence map"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextClassicPlusStepsLatPertinenceMap->Wrap( -1 );
	fgSizer21->Add( m_StaticTextClassicPlusStepsLatPertinenceMap, 0, wxALL, 5 );
	
	m_TextCtrlClassicPlusStepsLatPertinenceMap = new wxTextCtrl( m_PanelSingle, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_TextCtrlClassicPlusStepsLatPertinenceMap->SetMaxLength( 0 ); 
	fgSizer21->Add( m_TextCtrlClassicPlusStepsLatPertinenceMap, 0, wxRIGHT|wxLEFT, 5 );
	
	m_StaticTextClassicPlusResizingIterations = new wxStaticText( m_PanelSingle, wxID_ANY, _("Iterations in final resizing attempts"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextClassicPlusResizingIterations->Wrap( -1 );
	fgSizer21->Add( m_StaticTextClassicPlusResizingIterations, 0, wxALL, 5 );
	
	m_TextCtrlClassicPlusResizingIterations = new wxTextCtrl( m_PanelSingle, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_TextCtrlClassicPlusResizingIterations->SetMaxLength( 0 ); 
	fgSizer21->Add( m_TextCtrlClassicPlusResizingIterations, 0, wxRIGHT|wxLEFT, 5 );
	
	m_CheckBoxProceedSequentially = new wxCheckBox( m_PanelSingle, wxID_ANY, _("Proceed sequentially (standard)"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer21->Add( m_CheckBoxProceedSequentially, 0, wxALL, 5 );
	
	m_StaticTextSpacer = new wxStaticText( m_PanelSingle, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextSpacer->Wrap( -1 );
	fgSizer21->Add( m_StaticTextSpacer, 0, wxALL, 5 );
	
	m_CheckBoxClassicPlusResize = new wxCheckBox( m_PanelSingle, wxID_ANY, _("Resize the spatial windows separately"), wxDefaultPosition, wxDefaultSize, 0 );
	m_CheckBoxClassicPlusResize->Enable( false );
	
	fgSizer21->Add( m_CheckBoxClassicPlusResize, 0, wxALL, 5 );
	
	
	sbSizer10->Add( fgSizer21, 1, wxEXPAND, 5 );
	
	
	fgSizer3->Add( sbSizer10, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( m_PanelSingle, wxID_ANY, _("No option for") ), wxVERTICAL );
	
	m_staticText66 = new wxStaticText( m_PanelSingle, wxID_ANY, _("Single assessment"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText66->Wrap( -1 );
	sbSizer9->Add( m_staticText66, 0, wxALL, 5 );
	
	m_staticText67 = new wxStaticText( m_PanelSingle, wxID_ANY, _("Classic calibration"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText67->Wrap( -1 );
	sbSizer9->Add( m_staticText67, 0, wxALL, 5 );
	
	m_staticText68 = new wxStaticText( m_PanelSingle, wxID_ANY, _("Exhaustive exploration"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText68->Wrap( -1 );
	sbSizer9->Add( m_staticText68, 0, wxALL, 5 );
	
	
	fgSizer3->Add( sbSizer9, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer12;
	sbSizer12 = new wxStaticBoxSizer( new wxStaticBox( m_PanelSingle, wxID_ANY, _("Monte-Carlo") ), wxVERTICAL );
	
	m_StaticTextMonteCarloRandomNb = new wxStaticText( m_PanelSingle, wxID_ANY, _("Number of random param. sets"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextMonteCarloRandomNb->Wrap( -1 );
	sbSizer12->Add( m_StaticTextMonteCarloRandomNb, 0, wxALL, 5 );
	
	m_TextCtrlMonteCarloRandomNb = new wxTextCtrl( m_PanelSingle, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_TextCtrlMonteCarloRandomNb->SetMaxLength( 0 ); 
	sbSizer12->Add( m_TextCtrlMonteCarloRandomNb, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	
	fgSizer3->Add( sbSizer12, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer91;
	sbSizer91 = new wxStaticBoxSizer( new wxStaticBox( m_PanelSingle, wxID_ANY, _("Variables exploration") ), wxVERTICAL );
	
	m_StaticTextVarExploStepToExplore = new wxStaticText( m_PanelSingle, wxID_ANY, _("Step to explore (0-based)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_StaticTextVarExploStepToExplore->Wrap( -1 );
	sbSizer91->Add( m_StaticTextVarExploStepToExplore, 0, wxALL, 5 );
	
	m_TextCtrlVarExploStepToExplore = new wxTextCtrl( m_PanelSingle, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_TextCtrlVarExploStepToExplore->SetMaxLength( 0 ); 
	sbSizer91->Add( m_TextCtrlVarExploStepToExplore, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	
	fgSizer3->Add( sbSizer91, 1, wxEXPAND|wxALL, 5 );
	
	
	m_PanelSingle->SetSizer( fgSizer3 );
	m_PanelSingle->Layout();
	fgSizer3->Fit( m_PanelSingle );
	m_NotebookOptions->AddPage( m_PanelSingle, _("Calibration"), true );
	
	bSizer28->Add( m_NotebookOptions, 1, wxEXPAND | wxALL, 5 );
	
	
	m_PanelOptions->SetSizer( bSizer28 );
	m_PanelOptions->Layout();
	bSizer28->Fit( m_PanelOptions );
	m_NotebookBase->AddPage( m_PanelOptions, _("Options"), false );
	
	bSizer29->Add( m_NotebookBase, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	m_ButtonSaveDefault = new wxButton( m_PanelMain, wxID_ANY, _("Save as default"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_ButtonSaveDefault, 0, wxALIGN_RIGHT, 5 );
	
	
	bSizer29->Add( bSizer15, 0, wxALIGN_RIGHT|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	
	m_PanelMain->SetSizer( bSizer29 );
	m_PanelMain->Layout();
	bSizer29->Fit( m_PanelMain );
	bSizer4->Add( m_PanelMain, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer4 );
	this->Layout();
	bSizer4->Fit( this );
	m_ToolBar = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY );
	m_ToolBar->SetToolBitmapSize( wxSize( 32,32 ) );
	m_ToolBar->Realize(); 
	
	m_statusBar1 = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	m_MenuBar = new wxMenuBar( 0 );
	m_MenuOptions = new wxMenu();
	wxMenuItem* m_MenuItemPreferences;
	m_MenuItemPreferences = new wxMenuItem( m_MenuOptions, wxID_ANY, wxString( _("Preferences") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuOptions->Append( m_MenuItemPreferences );
	
	m_MenuBar->Append( m_MenuOptions, _("Options") ); 
	
	m_MenuLog = new wxMenu();
	wxMenuItem* m_MenuItemShowLog;
	m_MenuItemShowLog = new wxMenuItem( m_MenuLog, wxID_ANY, wxString( _("Show Log Window") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuLog->Append( m_MenuItemShowLog );
	
	m_MenuLogLevel = new wxMenu();
	wxMenuItem* m_MenuItemLogLevel1;
	m_MenuItemLogLevel1 = new wxMenuItem( m_MenuLogLevel, wxID_ANY, wxString( _("Only errors") ) , wxEmptyString, wxITEM_CHECK );
	m_MenuLogLevel->Append( m_MenuItemLogLevel1 );
	
	wxMenuItem* m_MenuItemLogLevel2;
	m_MenuItemLogLevel2 = new wxMenuItem( m_MenuLogLevel, wxID_ANY, wxString( _("Errors and warnings") ) , wxEmptyString, wxITEM_CHECK );
	m_MenuLogLevel->Append( m_MenuItemLogLevel2 );
	
	wxMenuItem* m_MenuItemLogLevel3;
	m_MenuItemLogLevel3 = new wxMenuItem( m_MenuLogLevel, wxID_ANY, wxString( _("Verbose") ) , wxEmptyString, wxITEM_CHECK );
	m_MenuLogLevel->Append( m_MenuItemLogLevel3 );
	
	m_MenuLog->Append( -1, _("Log level"), m_MenuLogLevel );
	
	m_MenuBar->Append( m_MenuLog, _("Log") ); 
	
	m_MenuHelp = new wxMenu();
	wxMenuItem* m_MenuItemAbout;
	m_MenuItemAbout = new wxMenuItem( m_MenuHelp, wxID_ANY, wxString( _("About") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuHelp->Append( m_MenuItemAbout );
	
	m_MenuBar->Append( m_MenuHelp, _("Help") ); 
	
	m_MenuControls = new wxMenu();
	wxMenuItem* m_MenuItemLaunch;
	m_MenuItemLaunch = new wxMenuItem( m_MenuControls, wxID_ANY, wxString( _("Launch") ) , wxEmptyString, wxITEM_NORMAL );
	m_MenuControls->Append( m_MenuItemLaunch );
	
	m_MenuBar->Append( m_MenuControls, _("Controls") ); 
	
	this->SetMenuBar( m_MenuBar );
	
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_ButtonSaveDefault->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( asFrameCalibrationVirtual::OnSaveDefault ), NULL, this );
	this->Connect( m_MenuItemPreferences->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OpenFramePreferences ) );
	this->Connect( m_MenuItemShowLog->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OnShowLog ) );
	this->Connect( m_MenuItemLogLevel1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OnLogLevel1 ) );
	this->Connect( m_MenuItemLogLevel2->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OnLogLevel2 ) );
	this->Connect( m_MenuItemLogLevel3->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OnLogLevel3 ) );
	this->Connect( m_MenuItemAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OpenFrameAbout ) );
	this->Connect( m_MenuItemLaunch->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::Launch ) );
}

asFrameCalibrationVirtual::~asFrameCalibrationVirtual()
{
	// Disconnect Events
	m_ButtonSaveDefault->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( asFrameCalibrationVirtual::OnSaveDefault ), NULL, this );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OpenFramePreferences ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OnShowLog ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OnLogLevel1 ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OnLogLevel2 ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OnLogLevel3 ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::OpenFrameAbout ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( asFrameCalibrationVirtual::Launch ) );
	
}
