#include <fstream>
#include "Settings.h"

const char * Settings::FILENAME = "settings.ini";
const char * Settings::DELIMITER = "=";

Settings & Settings::getInstance()
{
	static Settings instance;
	if (!instance.loaded)
	{
		wxString fullPath = wxGetCwd() + "/" + FILENAME;
		ifstream file(fullPath.mb_str());
		if (file.is_open())
		{
			string line, key, value;
			int pos = 0;
			while (getline(file, line))
			{
				pos = line.find(DELIMITER);
				key = line.substr(0, pos);
				value = line.substr(pos + 1);
				instance.settings[key] = value;
			}
			file.close();
		}
		instance.loaded = true;
	}
	return instance;
}

void Settings::set(const wxString & key, const wxString & value)
{
	settings[key] = value;
}

const wxString & Settings::get(const wxString & key)
{
	return settings[key];
}

void Settings::save()
{
	ofstream file((wxGetCwd() + "/" + FILENAME).mb_str(), ios::out | ios::trunc);
	if (file.is_open())
	{
		for (auto const & kv : settings)
		{
			file << kv.first << "=" << kv.second << endl;
		}
		file.close();
	}
}
