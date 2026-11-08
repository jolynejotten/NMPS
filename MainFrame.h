#pragma once
#include <wx/wx.h>
#include <wx/simplebook.h>
#include "ObsClient.h"

enum class Scene {
    Initial,
    NSC
};

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);
    wxSimplebook* pages;
    void ChangeScene(Scene scene);
    void BackToInitalScene();

private:
    void OnButtonClicked_NSC(wxCommandEvent& evt);
    ObsClient obsClient_;
};