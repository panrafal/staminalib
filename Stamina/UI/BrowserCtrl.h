/*

The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License from
/LICENSE.HTML in this package or at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is "Stamina.lib" library code, released Feb 1, 2006.

The Initial Developer of the Original Code is "STAMINA" - Rafa- Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa- Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id: CriticalSection.cpp 160 2006-01-29 13:54:08Z hao $

*/

#pragma once

#include <objbase.h>
#include <exdisp.h>

#include <initguid.h>
#include <shlguid.h>
#include <shlobj.h>
#include <mshtml.h>
#include <oleauto.h>
#include <mshtmhst.h>

#include "Control.h"

#include <Stamina/String.h>


namespace Stamina {
namespace UI {

	class BrowserCtrlSink:public  DWebBrowserEvents2 {
	private:
		unsigned _ref;
	public:
		BrowserCtrlSink();
		virtual ~BrowserCtrlSink();
		// IDispatch
		STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppv);
		STDMETHODIMP_(ULONG) AddRef(void);
		STDMETHODIMP_(ULONG) Release(void);

		STDMETHOD(GetTypeInfoCount)(UINT*);
		STDMETHOD(GetTypeInfo)(UINT, LCID, LPTYPEINFO*);
		STDMETHOD(GetIDsOfNames)(REFIID,LPOLESTR*,UINT,LCID,DISPID*);
		STDMETHOD(Invoke)(DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
		// DWebBrowserEvents2
		STDMETHODIMP_(void)StatusTextChange(BSTR) {}
		virtual void __stdcall ProgressChange(long, long) {}
		STDMETHODIMP_(void)CommandStateChange(long, VARIANT_BOOL) {}
		STDMETHODIMP_(void)DownloadBegin() {}
		STDMETHODIMP_(void)DownloadComplete() {}
		STDMETHODIMP_(void)TitleChange(BSTR Text) {}
		STDMETHODIMP_(void)PropertyChange(BSTR Text) {}
		virtual void __stdcall BeforeNavigate2(IDispatch*,VARIANT*,VARIANT*,VARIANT*,VARIANT*,VARIANT*,VARIANT_BOOL*) {}
		STDMETHODIMP_(void)NewWindow2(IDispatch**, VARIANT_BOOL*) {}
		STDMETHODIMP_(void)NavigateComplete(IDispatch*, VARIANT*) {}
		virtual void __stdcall DocumentComplete(IDispatch*, VARIANT*) {}
		STDMETHODIMP_(void)OnQuit() {}
		STDMETHODIMP_(void)OnVisible(VARIANT_BOOL) {}
		STDMETHODIMP_(void)OnToolBar(VARIANT_BOOL) {}
		STDMETHODIMP_(void)OnMenuBar(VARIANT_BOOL) {}
		STDMETHODIMP_(void)OnStatusBar(VARIANT_BOOL) {}
		STDMETHODIMP_(void)OnFullScreen(VARIANT_BOOL) {}
		STDMETHODIMP_(void)OnTheaterMode(VARIANT_BOOL) {}
		STDMETHODIMP_(void)WindowSetResizable(VARIANT_BOOL) {}
		STDMETHODIMP_(void)WindowSetLeft(long) {}
		STDMETHODIMP_(void)WindowSetTop(long) {}
		STDMETHODIMP_(void)WindowSetWidth(long) {}
		STDMETHODIMP_(void)WindowSetHeight(long) {}
		STDMETHODIMP_(void)WindowClosing(VARIANT_BOOL, VARIANT_BOOL*) {}
		STDMETHODIMP_(void)ClientToHostWindow(long*,long*) {}
		STDMETHODIMP_(void)SetSecureLockIcon(long) {}
		STDMETHODIMP_(void)FileDownload(VARIANT_BOOL*) {}

	protected:

		friend class BrowserCtrl;

		class BrowserCtrl* _browser;
	};
	

	class BrowserCtrl: public IDispatch, public IOleClientSite, public IOleInPlaceSite, public IDocHostUIHandler {
		
	public:
		friend class BrowserCtrlSink;

		BrowserCtrl(const Rect& rc, HWND parent, BrowserCtrlSink* sink = 0);
		virtual ~BrowserCtrl();

		static BrowserCtrl* replaceWindow(HWND replace, BrowserCtrlSink* sink = 0);

		void navigate(const StringRef& url, const StringRef& post = "", StringRef headers = "");
		IHTMLDocument2 *getDocument2();
		IHTMLDocument3 *getDocument3();

	protected:

		// IUnknown
		STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppv);
		STDMETHODIMP_(ULONG) AddRef(void);
		STDMETHODIMP_(ULONG) Release(void);

		// IDispatch
		STDMETHOD(GetTypeInfoCount)(UINT*);
		STDMETHOD(GetTypeInfo)(UINT, LCID, LPTYPEINFO*);
		STDMETHOD(GetIDsOfNames)(REFIID,LPOLESTR*,UINT,LCID,DISPID*);
		STDMETHOD(Invoke)(DISPID,REFIID,LCID,WORD,DISPPARAMS*,VARIANT*,EXCEPINFO*,UINT*);
		// IOleWindow
		STDMETHOD(GetWindow)(HWND *phwnd);
		STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);
		// IOleInPlace
		STDMETHOD(CanInPlaceActivate)(void);
		STDMETHOD(OnInPlaceActivate)(void);
		STDMETHOD(OnUIActivate)(void);
		STDMETHOD(GetWindowContext)(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,
									LPRECT lprcPosRect, LPRECT lprcClipRect,
								LPOLEINPLACEFRAMEINFO lpFrameInfo);
		STDMETHOD(Scroll)(SIZE scrollExtant);

		STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
		STDMETHOD(OnInPlaceDeactivate)( void);
		STDMETHOD(DiscardUndoState)( void);
		STDMETHOD(DeactivateAndUndo)( void);
		STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);
		// IOleClientSite
		STDMETHOD(SaveObject)(void);
		STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
		STDMETHOD(GetContainer)(IOleContainer **ppContainer);
		STDMETHOD(ShowObject)(void);
		STDMETHOD(OnShowWindow)(BOOL fShow);
		STDMETHOD(RequestNewObjectLayout)(void);

		// IDocHostUIHandler
		STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
		STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
		STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget,
    					IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc);
		STDMETHOD(HideUI)(void);
		STDMETHOD(UpdateUI)(void);
		STDMETHOD(EnableModeless)(BOOL fEnable);
		STDMETHOD(OnDocWindowActivate)(BOOL fEnable);
		STDMETHOD(OnFrameWindowActivate)(BOOL fEnable);
		STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow);
		STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
		STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw);
		STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
		STDMETHOD(GetExternal)(IDispatch **ppDispatch);
		STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);
		STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet);


	protected:

		static LRESULT CALLBACK serverWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		void translateAccelerator(int message, WPARAM wParam, LPARAM lParam);

		IWebBrowser2* _browser;
		BrowserCtrlSink* _sink;
		HWND _parent;
		HWND _hwndShell;
		HWND _hwndView;
		HWND _hwndServer;
		unsigned _ref;
		bool _inPlaceActive;
		Rect _rect;
		DWORD _cookie;
		IConnectionPoint* _connectionPoint;

		WNDPROC _wndProcServer;

	};

};};