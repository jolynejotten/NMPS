#include "MainFrame.h"

#include <wx/wx.h>
#include <wx/simplebook.h>

namespace
{
    enum
    {
        ID_BUTTON_NSC = wxID_HIGHEST + 1,
        ID_CHECKBOX_NSC_TO_OBS,
        ID_BUTTON_RETURN_HOME,
        ID_HOTKEY_NUM0,
        ID_HOTKEY_NUM1
    };
}

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title)
{
    SetBackgroundColour(wxColour(32, 38, 48));

    pages = new wxSimplebook(this, wxID_ANY);

    auto* scene_Initial = new wxPanel(pages);
    auto* buttonNSC = new wxButton(
        scene_Initial,
        ID_BUTTON_NSC,
        "NSC",
        wxPoint(100, 100),
        wxSize(100, 100));

    pages->AddPage(scene_Initial, "Initial", true);
    buttonNSC->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked_NSC, this);

    pages->AddPage(new wxPanel(pages), "NSC", false);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(pages, 1, wxEXPAND);
    SetSizer(sizer);

    CreateStatusBar();

    obsClient_.Connect();

    // Register global hotkeys: no modifiers, so these work while
    // this window is not focused.
    const bool num0Registered =
        RegisterHotKey(ID_HOTKEY_NUM0, wxMOD_NONE, WXK_NUMPAD0);
    const bool num1Registered =
        RegisterHotKey(ID_HOTKEY_NUM1, wxMOD_NONE, WXK_NUMPAD1);

    if (!num0Registered)
        wxLogWarning("Could not register global Numpad 0 hotkey.");

    if (!num1Registered)
        wxLogWarning("Could not register global Numpad 1 hotkey.");

    Bind(wxEVT_HOTKEY, [this](wxKeyEvent& event)
        {
            switch (event.GetId())
            {
            case ID_HOTKEY_NUM0:
                obsClient_.SetCurrentProgramScene("Gaming");
                break;

            case ID_HOTKEY_NUM1:
                obsClient_.SetCurrentProgramScene("Just Chatting");
                break;
            }
        });

    // Release the global hotkeys when this window closes.
    Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& event)
        {
            UnregisterHotKey(ID_HOTKEY_NUM0);
            UnregisterHotKey(ID_HOTKEY_NUM1);
            event.Skip();
        });
}

void MainFrame::OnButtonClicked_NSC(wxCommandEvent& evt)
{
    ChangeScene(Scene::NSC);
}

void MainFrame::ChangeScene(Scene scene)
{
    const char* sceneName = "Unknown";

    switch (scene)
    {
    case Scene::Initial:
        sceneName = "Initial";
        pages->SetSelection(0);
        break;

    case Scene::NSC:
        sceneName = "NSC";
        pages->SetSelection(1);
        break;
    }

    wxLogStatus("Switched scene to: %s", sceneName);
}

void MainFrame::BackToInitalScene()
{
    ChangeScene(Scene::Initial);
}