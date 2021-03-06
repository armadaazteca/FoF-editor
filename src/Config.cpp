#include "Config.h"

const wxString & Config::APP_NAME = wxT("FoF editor");
const wxString & Config::VERSION_STRING = wxString::Format(wxT("%i.%i.%i"), Config::MAJOR_VERSION,
                                          Config::MINOR_VERSION, Config::BUILD_VERSION);
const wxString & Config::ERROR_TITLE = wxT("Error");
const wxString & Config::COMMON_READ_ERROR = wxT("The %s file cannot be read");
const wxString & Config::COMMON_WRITE_ERROR = wxT("The %s file cannot be written");
const wxString & Config::INVALID_SIGNATURE_ERROR = wxT("The %s file has invalid signature (must be %#010x)");
