#pragma once
#include <wx/wx.h>


class NSC : public wxPanel {
public:
	explicit NSC(wxWindow* parent);
private:
	void OnCheckboxClicked_NscToObs(wxCommandEvent& evt);
	bool nsc_Obs_TriggersAreActive = false;
};


