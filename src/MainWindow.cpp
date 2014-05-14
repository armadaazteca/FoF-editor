#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "MainWindow.h"
#include "DatSprOpenDialog.h"

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_MENU(wxID_OPEN,  MainWindow::OnOpenDatSprDialog)
	EVT_MENU(wxID_EXIT,  MainWindow::OnExit)
	EVT_MENU(wxID_ABOUT, MainWindow::OnAbout)
wxEND_EVENT_TABLE()

MainWindow::MainWindow(const wxString & title, const wxPoint & pos, const wxSize & size) : wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxMenu * menuFile = new wxMenu();
	menuFile->Append(wxID_OPEN);
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu * menuHelp = new wxMenu();
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar * menuBar = new wxMenuBar();
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	CreateStatusBar();
}

void MainWindow::OnOpenDatSprDialog(wxCommandEvent & event)
{
	DatSprOpenDialog * dialog = new DatSprOpenDialog(this);
	dialog->ShowModal();
}

void MainWindow::OnExit(wxCommandEvent & event)
{
	Close(true);
}

void MainWindow::OnAbout(wxCommandEvent & event)
{
	wxMessageBox("Here will be some about text", "About", wxOK | wxICON_INFORMATION);
}
