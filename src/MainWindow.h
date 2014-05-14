class MainWindow : public wxFrame
{
public:
	MainWindow(const wxString & title, const wxPoint & pos, const wxSize & size);

private:
	void OnOpenDatSprDialog(wxCommandEvent & event);
	void OnExit(wxCommandEvent & event);
	void OnAbout(wxCommandEvent & event);
	wxDECLARE_EVENT_TABLE();
};
