#include "Config.h"

const wxString & Config::APP_NAME = "NewEditor";
const wxString & Config::VERSION_STRING = wxString::Format("%i.%i.%i", Config::MAJOR_VERSION,
                                        Config::MINOR_VERSION, Config::BUILD_VERSION);
