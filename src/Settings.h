#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <map>

using namespace std;

class Settings
{
public:
	static Settings & getInstance();
	void set(const wxString & key, const wxString & value);
	const wxString & get(const wxString & key);
	void save();

private:
	static const char * FILENAME;
	static const char * DELIMITER;
	bool loaded = false;
	map <wxString, wxString> settings;
};

#endif // _SETTINGS_H_
