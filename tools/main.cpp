#include <formula/syntax.h>

#include <wx/dynlib.h>
#include <wx/stc/stc.h>
#include <wx/wx.h>

enum class MarginIndex
{
    LINE_NUMBER = 0,
    FOLDING = 2,
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
    void init_lexer();
    void init_coloring();
    void init_line_numbers();
    void init_folding();
    void show_hide_line_numbers();
    void show_hide_folding();
    void on_view_line_numbers(wxCommandEvent &event);
    void on_view_folding(wxCommandEvent &event);
    void on_margin_click(wxStyledTextEvent &event);
    void on_exit(wxCommandEvent &event);

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
    init_lexer();
    init_coloring();
    init_line_numbers();
    init_folding();
}

void ScintillaFrame::set_style_font_color(formula::Syntax style, const wxFont &font, const char *color_name)
{
    m_stc->StyleSetFont(+style, font);
    wxColour color;
    wxASSERT(wxFromString(color_name, &color));
    m_stc->StyleSetForeground(+style, color);
}

void ScintillaFrame::init_lexer()
{
    m_stc->LoadLexerLibrary(wxT("./formula-lexer") + wxDynamicLibrary::GetDllExt(wxDL_LIBRARY));
    m_stc->SetLexerLanguage(wxT("id-formula"));
}

void ScintillaFrame::init_coloring()
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
    m_stc->Colourise(0, -1);
}

void ScintillaFrame::init_line_numbers()
{
    m_stc->SetMarginType(+MarginIndex::LINE_NUMBER, wxSTC_MARGIN_NUMBER);
    m_line_margin_width = m_stc->TextWidth(wxSTC_STYLE_LINENUMBER, "_99999");
    show_hide_line_numbers();
}

void ScintillaFrame::init_folding()
{
    m_stc->SetMarginType(+MarginIndex::FOLDING, wxSTC_MARGIN_SYMBOL);
    for (int i = wxSTC_MARKNUM_FOLDEREND; i <= wxSTC_MARKNUM_FOLDEROPEN; ++i)
    {
        m_stc->MarkerDefine(i, wxSTC_MARK_EMPTY);
    }
    m_stc->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN);
    m_stc->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_ARROW);
    m_stc->SetMarginMask(+MarginIndex::FOLDING, wxSTC_MASK_FOLDERS);
    m_stc->SetMarginSensitive(+MarginIndex::FOLDING, true);
    Bind(wxEVT_STC_MARGINCLICK, &ScintillaFrame::on_margin_click, this, m_stc->GetId());
    show_hide_folding();
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

void ScintillaFrame::on_margin_click(wxStyledTextEvent &event)
{
    m_stc->ToggleFold(m_stc->LineFromPosition(event.GetPosition()));
}

void ScintillaFrame::on_exit(wxCommandEvent & /*event*/)
{
    Close(true);
}
