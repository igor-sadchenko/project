#if !defined(AFX_SK_H__7DA931DA_6361_4564_8387_943E341134E1__HEAD__)
#define AFX_SK_H__7DA931DA_6361_4564_8387_943E341134E1__HEAD__

namespace local {
	// Implement application-level callbacks for the browser process.
	class SimpleApp : public CefApp, public CefBrowserProcessHandler {
	public:
		SimpleApp();

		// CefApp methods:
		virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
			OVERRIDE {
			return this;
		}

		// CefBrowserProcessHandler methods:
		virtual void OnContextInitialized() OVERRIDE;

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

		// Include the default reference counting implementation.
		IMPLEMENT_REFCOUNTING(SimpleHandler);
	};

}///namespace local

//!@ /*新生联创®（上海）*/
//!@ /*Sun Nov 8 15:42:11 UTC+0800 2020*/
//!@ /*___www.skstu.com___*/
#endif /*AFX_SK_H__7DA931DA_6361_4564_8387_943E341134E1__HEAD__*/