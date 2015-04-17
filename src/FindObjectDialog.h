#ifndef _FIND_OBJECT_DIALOG_
#define _FIND_OBJECT_DIALOG_

class FindObjectDialog : public wxDialog
{
public:
	FindObjectDialog(wxWindow * parent);
	inline const wxString & getObjectID() { return objectID; }
	inline virtual ~FindObjectDialog() {}

private:
	void OnClickFindButton(wxCommandEvent & event);
	wxDECLARE_EVENT_TABLE();

	wxTextCtrl * objectIDInput = nullptr;
	wxString objectID;
};

#endif // _FIND_OBJECT_DIALOG_