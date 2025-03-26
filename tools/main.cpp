#include <wx/wx.h>
#include <wx/stc/stc.h>

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

private:
    void OnExit(wxCommandEvent& event);

    wxStyledTextCtrl* m_stc;
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame("Scintilla Editing Example");
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "&Quit\tAlt-F4", "Quit");
    menuBar->Append(fileMenu, "&File");
    wxFrameBase::SetMenuBar(menuBar);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

    m_stc = new wxStyledTextCtrl(this, wxID_ANY);
    m_stc->SetText("Hello, wxStyledTextCtrl!");
}

void MyFrame::OnExit(wxCommandEvent &event)
{
    Close(true);
}
