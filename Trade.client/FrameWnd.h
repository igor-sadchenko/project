#if !defined(AFX_SKSTU_1604538404_H__68F2E_6E38EE04_84F55D53BA7DE01_D2CE__HEAD__)
#define AFX_SKSTU_1604538404_H__68F2E_6E38EE04_84F55D53BA7DE01_D2CE__HEAD__

#include "wx/wx.h"
#include "wx/wxprec.h"
#include "wx/mdi.h"
#include "wx/artprov.h"
#include "wx/toolbar.h"
#include "wx/clipbrd.h"

#include "ChildWndStd.h"

namespace client {

    class FrameApp : public wxApp
    {
    public:
        virtual bool OnInit() wxOVERRIDE;
    };

    class MenuEventLogger
    {
    public:
        MenuEventLogger(const char* label, wxFrame* frame)
            : m_label(label),
            m_frame(frame)
        {
        }
    protected:
        void LogMenuOpenClose(wxMenuEvent& event, const char* action)
        {
            event.Skip();
            wxString what;
            wxMenu* const menu = event.GetMenu();
            if (menu)
                what.Printf("Menu \"%s\"", menu->GetTitle());
            else
                what = "Unknown menu";
            wxLogMessage(m_frame, "%s %s in %s", what, action, m_label);
        }

        void LogMenuHighlight(wxMenuEvent& event)
        {
            event.Skip();
            wxLogMessage(m_frame, "Item %d selected in %s",event.GetMenuId(), m_label);
        }
        const wxString m_label;
        wxFrame* const m_frame;
        wxDECLARE_NO_COPY_CLASS(MenuEventLogger);
    };

    class FrameCanvas : public wxScrolledWindow, private MenuEventLogger
    {
    public:
        FrameCanvas(wxFrame* parent, const wxPoint& pos, const wxSize& size);
        virtual void OnDraw(wxDC& dc) wxOVERRIDE;
        bool IsDirty() const { return m_dirty; }
        void SetText(const wxString& text) { m_text = text; Refresh(); }
    private:
        void OnMenuOpen(wxMenuEvent& event) { LogMenuOpenClose(event, "opened"); }
        void OnMenuHighlight(wxMenuEvent& event) { LogMenuHighlight(event); }
        void OnMenuClose(wxMenuEvent& event) { LogMenuOpenClose(event, "closed"); }
        void OnUpdateUIDisable(wxUpdateUIEvent& event) { event.Enable(false); }
        void OnMenu(wxContextMenuEvent& event);
        void OnEvent(wxMouseEvent& event);
        wxString m_text;
        bool m_dirty;
        wxDECLARE_EVENT_TABLE();
    };

    class FrameWnd : public wxMDIParentFrame, private MenuEventLogger
    {
    public:
        FrameWnd();
        virtual ~FrameWnd();
        static wxMenuBar* CreateMainMenubar();
    private:
        void InitToolBar(wxToolBar* toolBar);
        void OnSize(wxSizeEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnNewWindow(wxCommandEvent& event);
        void OnFullScreen(wxCommandEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnCloseAll(wxCommandEvent& event);
        void OnMenuOpen(wxMenuEvent& event) { LogMenuOpenClose(event, "opened"); }
        void OnMenuHighlight(wxMenuEvent& event) { LogMenuHighlight(event); }
        void OnMenuClose(wxMenuEvent& event) { LogMenuOpenClose(event, "closed"); }
        void OnClose(wxCloseEvent& event);
        wxTextCtrl* m_textWindow;
        wxDECLARE_EVENT_TABLE();
    };

    class ChildWndStd : public wxMDIChildFrame, private MenuEventLogger
    {
    public:
        ChildWndStd(wxMDIParentFrame* parent);
        virtual ~ChildWndStd();
        static unsigned GetChildrenCount() { return ms_numChildren; }
    private:
        void OnActivate(wxActivateEvent& event);
        void OnRefresh(wxCommandEvent& event);
        void OnUpdateRefresh(wxUpdateUIEvent& event);
        void OnChangeTitle(wxCommandEvent& event);
        void OnChangePosition(wxCommandEvent& event);
        void OnChangeSize(wxCommandEvent& event);
        void OnClose(wxCommandEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnMove(wxMoveEvent& event);
        void OnMenuOpen(wxMenuEvent& event) { LogMenuOpenClose(event, "opened"); }
        void OnMenuHighlight(wxMenuEvent& event) { LogMenuHighlight(event); }
        void OnMenuClose(wxMenuEvent& event) { LogMenuOpenClose(event, "closed"); }
        void OnUpdateUIDisable(wxUpdateUIEvent& event) { event.Enable(false); }
        void OnCloseWindow(wxCloseEvent& event);

#if wxUSE_CLIPBOARD
        void OnPaste(wxCommandEvent& event);
        void OnUpdatePaste(wxUpdateUIEvent& event);
#endif // wxUSE_CLIPBOARD

        static unsigned ms_numChildren;
        FrameCanvas* m_canvas;

        class EventHandler : public wxEvtHandler
        {
        public:
            EventHandler(unsigned numChild) : m_numChild(numChild) { }
        private:
            void OnRefresh(wxCommandEvent& event)
            {
                wxLogMessage("Child #%u refreshed.", m_numChild);
                event.Skip();
            }
            const unsigned m_numChild;
            wxDECLARE_EVENT_TABLE();
            wxDECLARE_NO_COPY_CLASS(EventHandler);
        };
        wxDECLARE_EVENT_TABLE();
    };

    // menu items ids
    enum
    {
        MDI_FULLSCREEN = 100,
        MDI_REFRESH,
        MDI_DISABLED_FROM_CANVAS,
        MDI_DISABLED_FROM_CHILD,
        MDI_CHANGE_TITLE,
        MDI_CHANGE_POSITION,
        MDI_CHANGE_SIZE
    };
}///namespace client



/// /*新生联创（上海）**/
/// /*2020年11月5日 10:9:53|628**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_1604538404_H__68F2E_6E38EE04_84F55D53BA7DE01_D2CE__TAIL__