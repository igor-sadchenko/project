#if !defined(AFX_SK_H__A2B6A2BE_4394_42CF_85F7_E86ADDDF4740__HEAD__)
#define AFX_SK_H__A2B6A2BE_4394_42CF_85F7_E86ADDDF4740__HEAD__

namespace local {
#if ENABLE_CEF

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
	}



	// Implement application-level callbacks for the browser process.
	class SimpleApp : public CefApp, public CefBrowserProcessHandler {
	public:
		SimpleApp();
		SimpleApp(const HWND&);
		// CefApp methods:
		virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
			OVERRIDE {
			return this;
		}

		// CefBrowserProcessHandler methods:
		virtual void OnContextInitialized() OVERRIDE;
	private:
		HWND m_hParent = nullptr;
	private:
		// Include the default reference counting implementation.
		IMPLEMENT_REFCOUNTING(SimpleApp);
	};

	class SimpleHandler : public CefClient,
		public CefDisplayHandler,
		public CefLifeSpanHandler,
		public CefLoadHandler {
	public:
		explicit SimpleHandler(bool use_views);
		~SimpleHandler();

		HWND GetHwnd() const { return m_hWnd; }
		// Provide access to the single global instance of this object.
		static SimpleHandler* GetInstance();

		// CefClient methods:
		virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
			return this;
		}
		virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
			return this;
		}
		virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }

		// CefDisplayHandler methods:
		virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
			const CefString& title) OVERRIDE;

		// CefLifeSpanHandler methods:
		virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
		virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

		// CefLoadHandler methods:
		virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			ErrorCode errorCode,
			const CefString& errorText,
			const CefString& failedUrl) OVERRIDE;

		// Request that all existing browser windows close.
		void CloseAllBrowsers(bool force_close);

		bool IsClosing() const { return is_closing_; }

	private:
		// Platform-specific implementation.
		void PlatformTitleChange(CefRefPtr<CefBrowser> browser,
			const CefString& title);

		// True if the application is using the Views framework.
		const bool use_views_;

		// List of existing browser windows. Only accessed on the CEF UI thread.
		typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
		BrowserList browser_list_;

		bool is_closing_;
		HWND m_hWnd = nullptr;
		// Include the default reference counting implementation.
		IMPLEMENT_REFCOUNTING(SimpleHandler);
	};
#endif
}///namespace local

//!@ /*新生联创®（上海）*/
//!@ /*Mon Nov 9 14:37:15 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__A2B6A2BE_4394_42CF_85F7_E86ADDDF4740__HEAD__*/
