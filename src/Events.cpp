#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include "Events.h"

wxDEFINE_EVENT(DAT_SPR_LOADED, wxCommandEvent);
wxDEFINE_EVENT(DAT_SPR_SAVED, wxCommandEvent);
wxDEFINE_EVENT(RME_RES_GENERATED, wxCommandEvent);
wxDEFINE_EVENT(ADV_ATTRS_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(SPRITES_CLEANED_UP, wxCommandEvent);
wxDEFINE_EVENT(PREFERENCES_SAVED, wxCommandEvent);
wxDEFINE_EVENT(AUTOBACKUP_PROCESSED, wxCommandEvent);
