#include "NSC.h"
#include "Manager.h"
#include "MainFrame.h"
#include <wx/log.h>

NSC::NSC(wxWindow* parent)
	: wxPanel(parent)
{
	auto* checkboxNSCToObs = new wxButton(this, ID_CHECKBOX_NSC_TO_OBS, "OBS Triggers", wxPoint(500, 100), wxSize(100, 100));
	checkboxNSCToObs->Bind(
		wxEVT_BUTTON,
		&NSC::OnCheckboxClicked_NscToObs,
		this);

	auto* returnHomeButton = new wxButton(this, ID_BUTTON_RETURN_HOME, "Returned Home", wxPoint(100, 100), wxSize(100, 100));
	returnHomeButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
		if (auto* frame =
			dynamic_cast<MainFrame*>(wxGetTopLevelParent(this))) {
			frame->BackToInitalScene();
		}
		});
}

void NSC:: OnCheckboxClicked_NscToObs(wxCommandEvent& evt) {
	nsc_Obs_TriggersAreActive = !nsc_Obs_TriggersAreActive;
	wxLogStatus(
		wxGetTopLevelParent(this),
		"OBS triggers %s",
		nsc_Obs_TriggersAreActive ? "enabled" : "disabled");
}

