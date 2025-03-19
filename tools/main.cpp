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
    wxStyledTextCtrl* m_stc;
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame("wxStyledTextCtrl Example");
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    m_stc = new wxStyledTextCtrl(this, wxID_ANY);
    m_stc->SetText("Hello, wxStyledTextCtrl!");
}
