#include <formula/syntax.h>

#include <wx/dynlib.h>
#include <wx/stc/stc.h>
#include <wx/wx.h>

enum class MarginIndex
{
    LINE_NUMBER = 0,
};
inline int operator+(MarginIndex value)
{
    return static_cast<int>(value);
}

class ScintillaApp : public wxApp
{
public:
    virtual bool OnInit();
};

class ScintillaFrame : public wxFrame
{
public:
    ScintillaFrame(const wxString& title);

private:
    void load_language_theme();
    void on_exit(wxCommandEvent &event);
    void show_hide_line_numbers();
    void on_view_line_numbers(wxCommandEvent &event);

    wxMenuItem *m_view_lines{};
    wxStyledTextCtrl *m_stc{};
    int m_line_margin_width{};
    bool m_show_lines{};
};

wxIMPLEMENT_APP(ScintillaApp);

bool ScintillaApp::OnInit()
{
    ScintillaFrame *frame = new ScintillaFrame("Scintilla Editing Example");
    frame->Show(true);
    return true;
}

ScintillaFrame::ScintillaFrame(const wxString &title) :
    wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    wxMenuBar *menu_bar = new wxMenuBar;
    wxMenu *file = new wxMenu;
    file->Append(wxID_EXIT, "&Quit\tAlt-F4", "Quit");
    menu_bar->Append(file, "&File");
    wxMenu *view = new wxMenu;
    m_view_lines = view->Append(wxID_ANY, "&Line Numbers", "Line Numbers", wxITEM_CHECK);
    menu_bar->Append(view, "&View");
    Bind(wxEVT_MENU, &ScintillaFrame::on_view_line_numbers, this, m_view_lines->GetId());
    wxFrameBase::SetMenuBar(menu_bar);
    Bind(wxEVT_MENU, &ScintillaFrame::on_exit, this, wxID_EXIT);

    m_stc = new wxStyledTextCtrl(this, wxID_ANY);
    m_stc->LoadLexerLibrary(wxT("./formula-lexer") + wxDynamicLibrary::GetDllExt(wxDL_LIBRARY));
    m_stc->SetLexerLanguage(wxT("id-formula"));
    load_language_theme();
    m_stc->Colourise(0, -1);

    show_hide_line_numbers();
}

void ScintillaFrame::load_language_theme()
{
    wxFont typewriter;
    typewriter.SetFamily(wxFONTFAMILY_TELETYPE);
    typewriter.SetPointSize(12);
    m_stc->StyleSetFont(+formula::Syntax::NONE, typewriter);
    m_stc->StyleSetFont(+formula::Syntax::COMMENT, typewriter);
    m_stc->StyleSetFont(+formula::Syntax::KEYWORD, typewriter);
    m_stc->StyleSetFont(+formula::Syntax::WHITESPACE, typewriter);
    m_stc->StyleSetFont(+formula::Syntax::FUNCTION, typewriter);
    m_stc->StyleSetFont(+formula::Syntax::IDENTIFIER, typewriter);
    wxColour comment;
    wxASSERT(wxFromString("forest green", &comment));
    m_stc->StyleSetForeground(+formula::Syntax::COMMENT, comment);
    wxColor keyword;
    wxASSERT(wxFromString("blue", &keyword));
    m_stc->StyleSetForeground(+formula::Syntax::KEYWORD, keyword);
    wxColor function;
    wxASSERT(wxFromString("red", &function));
    m_stc->StyleSetForeground(+formula::Syntax::FUNCTION, function);
    wxColor identifier;
    wxASSERT(wxFromString("purple", &identifier));
    m_stc->StyleSetForeground(+formula::Syntax::IDENTIFIER, identifier);
    m_stc->SetMarginType(+MarginIndex::LINE_NUMBER, wxSTC_MARGIN_NUMBER);
    m_stc->SetMarginWidth(+MarginIndex::LINE_NUMBER, 0);
    m_line_margin_width = m_stc->TextWidth(wxSTC_STYLE_LINENUMBER, "_99999");
}

void ScintillaFrame::on_exit(wxCommandEvent & /*event*/)
{
    Close(true);
}

void ScintillaFrame::show_hide_line_numbers()
{
    m_stc->SetMarginWidth(+MarginIndex::LINE_NUMBER, m_show_lines ? m_line_margin_width : 0);
    m_view_lines->Check(m_show_lines);
}

void ScintillaFrame::on_view_line_numbers(wxCommandEvent &event)
{
    m_show_lines = !m_show_lines;
    show_hide_line_numbers();
}
