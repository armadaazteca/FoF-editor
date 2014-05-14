class DatSprOpenDialog : public wxDialog
{
public:
	const static unsigned int WIDTH = 400;
	const static unsigned int HEIGHT = 150;
	DatSprOpenDialog(wxWindow * parent);
private:
	enum MenuItemIds
	{
		ID_BROWSE_DAT_BUTTON = 1,
		ID_BROWSE_SPR_BUTTON = 2,
		ID_OPEN = 3
	};
	void OnClickBrowseButton(wxCommandEvent & event);
	void OnClickOpenButton(wxCommandEvent & event);
	wxDECLARE_EVENT_TABLE();

	wxTextCtrl * datPath, * sprPath;
};
