#include <formula/syntax.h>

#include <wx/dynlib.h>
#include <wx/stc/stc.h>
#include <wx/wx.h>

enum class MarginIndex
{
    LINE_NUMBER = 0,
    FOLDING = 1,
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
    ScintillaFrame(const wxString &title);

private:
    void set_style_font_color(formula::Syntax style, const wxFont &font, const char *color_name);
    void load_language_theme();
    void on_exit(wxCommandEvent &event);
    void show_hide_line_numbers();
    void show_hide_folding();
    void on_view_line_numbers(wxCommandEvent &event);
    void on_view_folding(wxCommandEvent &event);

    wxMenuItem *m_view_lines{};
    wxMenuItem *m_view_folding{};
    wxStyledTextCtrl *m_stc{};
    int m_line_margin_width{};
    int m_folding_margin_width{20};
    bool m_show_lines{};
    bool m_show_folding{true};
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
    Bind(wxEVT_MENU, &ScintillaFrame::on_view_line_numbers, this, m_view_lines->GetId());
    m_view_folding = view->Append(wxID_ANY, "&Folding", "Folding", wxITEM_CHECK);
    Bind(wxEVT_MENU, &ScintillaFrame::on_view_folding, this, m_view_folding->GetId());
    menu_bar->Append(view, "&View");
    wxFrameBase::SetMenuBar(menu_bar);
    Bind(wxEVT_MENU, &ScintillaFrame::on_exit, this, wxID_EXIT);

    m_stc = new wxStyledTextCtrl(this, wxID_ANY);
    m_stc->LoadLexerLibrary(wxT("./formula-lexer") + wxDynamicLibrary::GetDllExt(wxDL_LIBRARY));
    m_stc->SetLexerLanguage(wxT("id-formula"));
    load_language_theme();
    m_stc->SetMarginType(+MarginIndex::LINE_NUMBER, wxSTC_MARGIN_NUMBER);
    m_line_margin_width = m_stc->TextWidth(wxSTC_STYLE_LINENUMBER, "_99999");
    m_stc->SetMarginType(+MarginIndex::FOLDING, wxSTC_MARGIN_SYMBOL);
    m_stc->Colourise(0, -1);

    show_hide_line_numbers();
    show_hide_folding();
}

void ScintillaFrame::set_style_font_color(formula::Syntax style, const wxFont &font, const char *color_name)
{
    m_stc->StyleSetFont(+style, font);
    wxColour color;
    wxASSERT(wxFromString(color_name, &color));
    m_stc->StyleSetForeground(+style, color);
}

void ScintillaFrame::load_language_theme()
{
    wxFont typewriter;
    typewriter.SetFamily(wxFONTFAMILY_TELETYPE);
    typewriter.SetPointSize(12);
    set_style_font_color(formula::Syntax::NONE, typewriter, "black");
    set_style_font_color(formula::Syntax::COMMENT, typewriter, "forest green");
    set_style_font_color(formula::Syntax::KEYWORD, typewriter, "blue");
    set_style_font_color(formula::Syntax::WHITESPACE, typewriter, "black");
    set_style_font_color(formula::Syntax::FUNCTION, typewriter, "red");
    set_style_font_color(formula::Syntax::IDENTIFIER, typewriter, "purple");
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

void ScintillaFrame::show_hide_folding()
{
    m_stc->SetProperty("fold", m_show_folding ? "1" : "0");
    m_stc->SetMarginWidth(+MarginIndex::FOLDING, m_show_folding ? m_folding_margin_width : 0);
    m_view_folding->Check(m_show_folding);
}

void ScintillaFrame::on_view_line_numbers(wxCommandEvent &/*event*/)
{
    m_show_lines = !m_show_lines;
    show_hide_line_numbers();
}

void ScintillaFrame::on_view_folding(wxCommandEvent &/*event*/)
{
    m_show_folding = !m_show_folding;
    show_hide_folding();
}
