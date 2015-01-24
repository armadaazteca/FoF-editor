#include "Config.h"

const wxString & Config::APP_NAME = "NewEditor";
const wxString & Config::VERSION_STRING = wxString::Format("%i.%i.%i", Config::MAJOR_VERSION,
                                          Config::MINOR_VERSION, Config::BUILD_VERSION);
const wxString & Config::ERROR_TITLE = "Error";
const wxString & Config::COMMON_READ_ERROR = "The %s file cannot be read";
const wxString & Config::COMMON_WRITE_ERROR = "The %s file cannot be written";
