#include "stdafx.h"
#include "CefHelper.h"

namespace local {

#if 0

    namespace {

        // When using the Views framework this object provides the delegate
        // implementation for the CefWindow that hosts the Views-based browser.
        class SimpleWindowDelegate : public CefWindowDelegate {
        public:
            explicit SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view)
                : browser_view_(browser_view) {}

            void OnWindowCreated(CefRefPtr<CefWindow> window) OVERRIDE {
                // Add the browser view and show the window.
                window->AddChildView(browser_view_);
                window->Show();

                // Give keyboard focus to the browser view.
                browser_view_->RequestFocus();
            }

            void OnWindowDestroyed(CefRefPtr<CefWindow> window) OVERRIDE {
                browser_view_ = nullptr;
            }

            bool CanClose(CefRefPtr<CefWindow> window) OVERRIDE {
                // Allow the window to close if the browser says it's OK.
                CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
                if (browser)
                    return browser->GetHost()->TryCloseBrowser();
                return true;
            }

            CefSize GetPreferredSize(CefRefPtr<CefView> view) OVERRIDE {
                return CefSize(800, 600);
            }

        private:
            CefRefPtr<CefBrowserView> browser_view_;

            IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
            DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
        };

        class SimpleBrowserViewDelegate : public CefBrowserViewDelegate {
        public:
            SimpleBrowserViewDelegate() {}

            bool OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
                CefRefPtr<CefBrowserView> popup_browser_view,
                bool is_devtools) OVERRIDE {
                // Create a new top-level Window for the popup. It will show itself after
                // creation.
                CefWindow::CreateTopLevelWindow(
                    new SimpleWindowDelegate(popup_browser_view));

                // We created the Window.
                return true;
            }

        private:
            IMPLEMENT_REFCOUNTING(SimpleBrowserViewDelegate);
            DISALLOW_COPY_AND_ASSIGN(SimpleBrowserViewDelegate);
        };

    }  // namespace

    SimpleApp::SimpleApp() {}

    void SimpleApp::OnContextInitialized() {
        CEF_REQUIRE_UI_THREAD();

        CefRefPtr<CefCommandLine> command_line =
            CefCommandLine::GetGlobalCommandLine();

        const bool enable_chrome_runtime =
            command_line->HasSwitch("enable-chrome-runtime");

#if defined(OS_WIN) || defined(OS_LINUX)
        // Create the browser using the Views framework if "--use-views" is specified
        // via the command-line. Otherwise, create the browser using the native
        // platform framework. The Views framework is currently only supported on
        // Windows and Linux.
        const bool use_views = command_line->HasSwitch("use-views");
#else
        const bool use_views = false;
#endif

        // SimpleHandler implements browser-level callbacks.
        CefRefPtr<SimpleHandler> handler(new SimpleHandler(use_views));

        // Specify CEF browser settings here.
        CefBrowserSettings browser_settings;

        std::string url;

        // Check if a "--url=" value was provided via the command-line. If so, use
        // that instead of the default URL.
        url = command_line->GetSwitchValue("url");
        if (url.empty())
            url = "http://cn.bing.com";

        if (use_views && !enable_chrome_runtime) {
            // Create the BrowserView.
            CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(
                handler, url, browser_settings, nullptr, nullptr,
                new SimpleBrowserViewDelegate());

            // Create the Window. It will show itself after creation.
            CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(browser_view));
        }
        else {
            // Information used when creating the native window.
            CefWindowInfo window_info;

#if defined(OS_WIN)
            // On Windows we need to specify certain flags that will be passed to
            // CreateWindowEx().
            window_info.SetAsPopup(NULL, "cefsimple");
#endif

            // Create the first browser window.
            CefBrowserHost::CreateBrowser(window_info, handler, url, browser_settings,
                nullptr, nullptr);
        }
    }




    namespace {

        SimpleHandler* g_instance = nullptr;

        // Returns a data: URI with the specified contents.
        std::string GetDataURI(const std::string& data, const std::string& mime_type) {
            return "data:" + mime_type + ";base64," +
                CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
                .ToString();
        }

    }  // namespace

    SimpleHandler::SimpleHandler(bool use_views)
        : use_views_(use_views), is_closing_(false) {
        DCHECK(!g_instance);
        g_instance = this;
    }

    SimpleHandler::~SimpleHandler() {
        g_instance = nullptr;
    }

    // static
    SimpleHandler* SimpleHandler::GetInstance() {
        return g_instance;
    }

    void SimpleHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
        const CefString& title) {
        CEF_REQUIRE_UI_THREAD();

        if (use_views_) {
            // Set the title of the window using the Views framework.
            CefRefPtr<CefBrowserView> browser_view =
                CefBrowserView::GetForBrowser(browser);
            if (browser_view) {
                CefRefPtr<CefWindow> window = browser_view->GetWindow();
                if (window)
                    window->SetTitle(title);
            }
        }
        else {
            // Set the title of the window using platform APIs.
            PlatformTitleChange(browser, title);
        }
    }

    void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
        CEF_REQUIRE_UI_THREAD();

        // Add to the list of existing browsers.
        browser_list_.push_back(browser);
    }

    bool SimpleHandler::DoClose(CefRefPtr<CefBrowser> browser) {
        CEF_REQUIRE_UI_THREAD();

        // Closing the main window requires special handling. See the DoClose()
        // documentation in the CEF header for a detailed destription of this
        // process.
        if (browser_list_.size() == 1) {
            // Set a flag to indicate that the window close should be allowed.
            is_closing_ = true;
        }

        // Allow the close. For windowed browsers this will result in the OS close
        // event being sent.
        return false;
    }

    void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
        CEF_REQUIRE_UI_THREAD();

        // Remove from the list of existing browsers.
        BrowserList::iterator bit = browser_list_.begin();
        for (; bit != browser_list_.end(); ++bit) {
            if ((*bit)->IsSame(browser)) {
                browser_list_.erase(bit);
                break;
            }
        }

        if (browser_list_.empty()) {
            // All browser windows have closed. Quit the application message loop.
            CefQuitMessageLoop();
        }
    }

    void SimpleHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl) {
        CEF_REQUIRE_UI_THREAD();

        // Don't display an error for downloaded files.
        if (errorCode == ERR_ABORTED)
            return;

        // Display a load error message using a data: URI.
        std::stringstream ss;
        ss << "<html><body bgcolor=\"white\">"
            "<h2>Failed to load URL "
            << std::string(failedUrl) << " with error " << std::string(errorText)
            << " (" << errorCode << ").</h2></body></html>";

        frame->LoadURL(GetDataURI(ss.str(), "text/html"));
    }

    void SimpleHandler::CloseAllBrowsers(bool force_close) {
        if (!CefCurrentlyOn(TID_UI)) {
            // Execute on the UI thread.
            CefPostTask(TID_UI, base::Bind(&SimpleHandler::CloseAllBrowsers, this,
                force_close));
            return;
        }

        if (browser_list_.empty())
            return;

        BrowserList::const_iterator it = browser_list_.begin();
        for (; it != browser_list_.end(); ++it)
            (*it)->GetHost()->CloseBrowser(force_close);
    }

    void SimpleHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
        const CefString& title) {
        CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
        if (hwnd)
            SetWindowText(hwnd, std::wstring(title).c_str());
    }









#endif
}///namespace local