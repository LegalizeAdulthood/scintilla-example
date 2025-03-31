#include <wx/dynlib.h>
#include <wx/stc/stc.h>
#include <wx/wx.h>

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
    void load_language_theme();
    void on_exit(wxCommandEvent& event);

    wxStyledTextCtrl *m_stc{};
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame("Scintilla Editing Example");
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString &title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "&Quit\tAlt-F4", "Quit");
    menuBar->Append(fileMenu, "&File");
    wxFrameBase::SetMenuBar(menuBar);
    Bind(wxEVT_MENU, &MyFrame::on_exit, this, wxID_EXIT);

    m_stc = new wxStyledTextCtrl(this, wxID_ANY);
    m_stc->LoadLexerLibrary(wxT("./formula-lexer") + wxDynamicLibrary::GetDllExt(wxDL_LIBRARY));
    m_stc->SetLexerLanguage(wxT("id-formula"));
    load_language_theme();
    m_stc->Colourise(0, -1);
}

void MyFrame::load_language_theme()
{
    wxFont typewriter;
    typewriter.SetFamily(wxFONTFAMILY_TELETYPE);
    typewriter.SetPointSize(12);
    typewriter.SetFaceName("consolas");
    m_stc->StyleSetFont(0, typewriter);
    m_stc->StyleSetFont(1, typewriter);
    wxColour purple;
    wxFromString("purple", &purple);
    m_stc->StyleSetForeground(1, purple);
}

void MyFrame::on_exit(wxCommandEvent &/*event*/)
{
    Close(true);
}
