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

$Id$

*/
#include "stdafx.h"
#include "BrowserCtrl.h"
#include "../WinHelper.h"

#include <mshtmhst.h>
#include <mshtmdid.h>

#include "../Exception.h"

#include <comutil.h>
#include <exdispid.h>


namespace Stamina {
namespace UI {


	BrowserCtrl::BrowserCtrl(const Rect& rc, HWND parent, BrowserCtrlSink* sink) {

		if (!sink) {
			sink = new BrowserCtrlSink();
		}

		OleInitialize(0);

		_ref = 0;
		_parent = parent;
		_inPlaceActive = false;
		_rect = rc;
		_sink = sink;
		_wndProcServer = 0;

		if (!SUCCEEDED(CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC, IID_IWebBrowser2, (LPVOID*)&_browser))) {
			throw Stamina::ExceptionString("CoCreateInstance failed!");
		}

		IOleObject*   oleObject = NULL;
		MSG msg;

		if (SUCCEEDED(_browser->QueryInterface(IID_IOleObject, (void**)&oleObject))) {
    		oleObject->SetClientSite(this);
    		oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, &msg, this, 0, parent, rc.ref());
    		oleObject->Release();
   		} else {
			throw Stamina::ExceptionString("IID_IOleObject failed!");
   		}

		IOleInPlaceObject *oleInPlace;

		if (SUCCEEDED(_browser->QueryInterface(IID_IOleInPlaceObject, (void**)&oleInPlace))) {
    		oleInPlace->GetWindow(&this->_hwndShell);
    		oleInPlace->Release();
		} else {
			throw Stamina::ExceptionString("IID_IOleInPlaceObject failed!");
		}

		if (_sink) {
			_sink->_browser = this;

   			IConnectionPointContainer* pCPContainer;
   			if (SUCCEEDED(_browser->QueryInterface(IID_IConnectionPointContainer,
                                      			(void**)&pCPContainer))) {
      			if (SUCCEEDED(pCPContainer->FindConnectionPoint(DIID_DWebBrowserEvents2,
													&_connectionPoint))) {
	         		if (FAILED(_connectionPoint->Advise((IUnknown *)_sink, &_cookie)))     {
						throw Stamina::ExceptionString("DIID_DWebBrowserEvents2 sink failed!");
	         		}
	      		}
      			pCPContainer->Release();
   			}
		}
		
		_browser->put_RegisterAsDropTarget(VARIANT_FALSE);

	}


	BrowserCtrl::~BrowserCtrl() {
		IOleObject*   oleObject = NULL;
		if (SUCCEEDED(_browser->QueryInterface(IID_IOleObject, (void**)&oleObject))) {
			oleObject->SetClientSite(NULL);
			oleObject->Release();
		} else {
			throw Stamina::ExceptionString("IID_IOleObject failed!");
		}
		
		if (_connectionPoint != NULL) {
			_connectionPoint->Unadvise(_cookie);
			_connectionPoint->Release();
		}
		if (_sink != NULL) {
			delete _sink;
		}

		_browser->Release();
		DestroyWindow(_hwndShell);
	}



	BrowserCtrl* BrowserCtrl::replaceWindow(HWND replace, BrowserCtrlSink* sink) {
		HWND parent = GetParent(replace);
		Rect rc;
		Stamina::getChildRect(replace, rc);
		DestroyWindow(replace);
		return new BrowserCtrl(rc, parent, sink);
	}


	// -------------------------------------

	void BrowserCtrl::navigate(const StringRef& url, const StringRef& post, StringRef headers) {
		_bstr_t bUrl = url.w_str();
		_bstr_t bPost = post.w_str();
		VARIANT vPost;
		SAFEARRAY* arr;
		if (!post.empty()) {
			SAFEARRAYBOUND bound;
			bound.lLbound = 0;
			bound.cElements = post.length();
			arr = SafeArrayCreate(VT_UI1, 1, &bound);
			for (LONG i=0; i < post.length(); ++i) {
				char c = post.c_str()[i];
                SafeArrayPutElement(arr, &i, &c);
			}
			vPost.parray = arr;
			vPost.vt = VT_ARRAY;

			headers += "Content-Type: application/x-www-form-urlencoded\r\n";

		}
		_bstr_t bHeaders = headers.w_str();
		_variant_t vHeaders = bHeaders;
		_browser->Navigate(bUrl, 0, 0, post.empty() ? 0 : &vPost, headers.empty() ? 0 : &vHeaders);
		if (!post.empty()) {
			SafeArrayDestroy(arr);
		}
	}


	IHTMLDocument2 *BrowserCtrl::getDocument2() {
		IHTMLDocument2 *document = NULL;
		IDispatch *dispatch = NULL;
		if (SUCCEEDED(_browser->get_Document(&dispatch)) && (dispatch != NULL)) {
			dispatch->QueryInterface(IID_IHTMLDocument2, (void **)&document);
			dispatch->Release();
		}
		return document;
	}

	IHTMLDocument3 *BrowserCtrl::getDocument3() {
		IHTMLDocument3 *document = NULL;
		IDispatch *dispatch = NULL;
		if (SUCCEEDED(_browser->get_Document(&dispatch)) && (dispatch != NULL)) {
			dispatch->QueryInterface(IID_IHTMLDocument3, (void **)&document);
			dispatch->Release();
		}
		return document;
	}


	// -------------------------------------

	// IUnknown
	STDMETHODIMP BrowserCtrl::QueryInterface(REFIID riid, PVOID *ppv) {
		*ppv=NULL;
		if (IID_IUnknown==riid)
			*ppv=this;
		if (IID_IOleClientSite==riid)
			*ppv=(IOleClientSite*)this;
		if (IID_IOleWindow==riid || IID_IOleInPlaceSite==riid)
			*ppv=(IOleInPlaceSite*)this;
		if (IID_IDocHostUIHandler==riid)
			*ppv=(IDocHostUIHandler*)this;
		if (NULL!=*ppv) {
			((LPUNKNOWN)*ppv)->AddRef();
			return NOERROR;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) BrowserCtrl::AddRef(void) {
		++_ref;
		return _ref;
	}

	STDMETHODIMP_(ULONG) BrowserCtrl::Release(void) {
		--_ref;
		return _ref;
	}

	// IDispatch
	STDMETHODIMP BrowserCtrl::GetTypeInfoCount(UINT *ptr) { return E_NOTIMPL; }
	STDMETHODIMP BrowserCtrl::GetTypeInfo(UINT iTInfo, LCID lcid, LPTYPEINFO* ppTInfo) { return S_OK; }
	STDMETHODIMP BrowserCtrl::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) { return S_OK; }

	STDMETHODIMP BrowserCtrl::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid , WORD wFlags,
								DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO*pExcepInfo, UINT*puArgErr) {
 		switch (dispIdMember) {
			case  DISPID_AMBIENT_DLCONTROL:
				break;
		}
		return DISP_E_MEMBERNOTFOUND;
	}

	// IOleWindow
	STDMETHODIMP BrowserCtrl::GetWindow(HWND *phwnd) {
		*phwnd = this->_parent;
		return S_OK;
	}

	STDMETHODIMP BrowserCtrl::ContextSensitiveHelp(BOOL fEnterMode) {return E_NOTIMPL;}

	// IOleInPlace
	STDMETHODIMP BrowserCtrl::CanInPlaceActivate(void) {
		return S_OK;
	}

	STDMETHODIMP BrowserCtrl::OnInPlaceActivate(void) {
		_inPlaceActive = true;
		return S_OK;
	}

	STDMETHODIMP BrowserCtrl::OnUIActivate(void) {
		return E_NOTIMPL;
	}

	STDMETHODIMP BrowserCtrl::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect,	LPOLEINPLACEFRAMEINFO lpFrameInfo) {
		
		*lprcPosRect = _rect.operator RECT();
		*lprcClipRect = _rect.operator RECT();
		return S_OK;
	}

	STDMETHODIMP BrowserCtrl::Scroll(SIZE scrollExtant) {return E_NOTIMPL;}
	STDMETHODIMP BrowserCtrl::OnUIDeactivate(BOOL fUndoable) {return E_NOTIMPL;}

	STDMETHODIMP BrowserCtrl::OnInPlaceDeactivate( void) {
		_inPlaceActive = FALSE;
		return S_OK;
	}

	STDMETHODIMP BrowserCtrl::DiscardUndoState( void) {return E_NOTIMPL;}

	STDMETHODIMP BrowserCtrl::DeactivateAndUndo( void) {return E_NOTIMPL;}

	STDMETHODIMP BrowserCtrl::OnPosRectChange(LPCRECT lprcPosRect) {return E_NOTIMPL;}

	// IOleClientSite
	STDMETHODIMP BrowserCtrl::SaveObject(void) {return E_NOTIMPL;}
	STDMETHODIMP BrowserCtrl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk) {	return E_NOTIMPL;}
	STDMETHODIMP BrowserCtrl::GetContainer(IOleContainer **ppContainer) {return E_NOTIMPL;}
	STDMETHODIMP BrowserCtrl::ShowObject(void) {return E_NOTIMPL;}
	STDMETHODIMP BrowserCtrl::OnShowWindow(BOOL fShow) {return E_NOTIMPL;}
	STDMETHODIMP BrowserCtrl::RequestNewObjectLayout(void) {return E_NOTIMPL;}


	// IDocHostUIHandler
	STDMETHODIMP BrowserCtrl::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdTarget, IDispatch *pdispReserved) {
		if (dwID == 0x2) return E_NOTIMPL;
		return S_OK;
	}

	STDMETHODIMP BrowserCtrl::GetHostInfo(DOCHOSTUIINFO *pInfo) {
 		pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE
 | DOCHOSTUIFLAG_THEME;// | DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE;
 		return S_OK;
	}

	STDMETHODIMP BrowserCtrl::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc) {
 		return S_OK;
	}

	STDMETHODIMP BrowserCtrl::HideUI(void) {return S_OK;}
	STDMETHODIMP BrowserCtrl::UpdateUI(void) {return S_OK;}
	STDMETHODIMP BrowserCtrl::EnableModeless(BOOL fEnable) { return E_NOTIMPL; }
	STDMETHODIMP BrowserCtrl::OnDocWindowActivate(BOOL fEnable) { return E_NOTIMPL; }
	STDMETHODIMP BrowserCtrl::OnFrameWindowActivate(BOOL fEnable) { return E_NOTIMPL; }
	STDMETHODIMP BrowserCtrl::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow) {return E_NOTIMPL;}
	STDMETHODIMP BrowserCtrl::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID) { return S_FALSE;}
	STDMETHODIMP BrowserCtrl::GetOptionKeyPath(LPOLESTR *pchKey, DWORD dw) { return E_NOTIMPL; }
	STDMETHODIMP BrowserCtrl::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget) {
		*ppDropTarget = NULL;
		return S_OK;
	//	return E_NOTIMPL;
	}

	STDMETHODIMP BrowserCtrl::GetExternal(IDispatch **ppDispatch) {
		*ppDispatch = NULL;
		return S_FALSE;
	}
	STDMETHODIMP BrowserCtrl::TranslateUrl(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut) { return E_NOTIMPL; }
	STDMETHODIMP BrowserCtrl::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet) { return E_NOTIMPL; }



	void BrowserCtrl::translateAccelerator(int message, WPARAM wParam, LPARAM lParam) {
		IOleInPlaceActiveObject* pIOIPAO;
		if (SUCCEEDED(_browser->QueryInterface(IID_IOleInPlaceActiveObject, (void**)&pIOIPAO))) {
			MSG msg;
			msg.message = message;
			msg.wParam = wParam;
			msg.lParam = lParam;
			pIOIPAO->TranslateAccelerator(&msg);
			pIOIPAO->Release();
		}
	
	}


	LRESULT CALLBACK BrowserCtrl::serverWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
		BrowserCtrl* me = (BrowserCtrl*) GetProp(hwnd, "BrowserCtrl*");
		S_ASSERT(me != 0);
		switch (message) {
			case WM_KEYDOWN: {
				bool ctrl = (GetKeyState(VK_CONTROL)&0x80)!=0;
				me->translateAccelerator(message, wParam, lParam);
				return 0;}
		}
		return CallWindowProc(me->_wndProcServer, hwnd, message, wParam, lParam);
	}




	// --------------------------  SINK ---------------------------------


	BrowserCtrlSink::BrowserCtrlSink() {
		_ref = 0;
	}

	BrowserCtrlSink::~BrowserCtrlSink() {}

	STDMETHODIMP BrowserCtrlSink::QueryInterface(REFIID riid, PVOID *ppv) {
		*ppv=NULL;
		if (IID_IUnknown==riid) {
			*ppv=(IUnknown *)this;
		}
		if (IID_IDispatch==riid) {
			*ppv=(IDispatch *)this;
		}
		if (DIID_DWebBrowserEvents2==riid) {
			*ppv=(DWebBrowserEvents2*)this;
		}
		if (NULL!=*ppv) {
			((LPUNKNOWN)*ppv)->AddRef();
			return NOERROR;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) BrowserCtrlSink::AddRef(void) {
		return ++_ref;
	}

	STDMETHODIMP_(ULONG) BrowserCtrlSink::Release(void) {
		return --_ref;
	}

	STDMETHODIMP BrowserCtrlSink::GetTypeInfoCount(UINT *ptr) { return E_NOTIMPL; }
	STDMETHODIMP BrowserCtrlSink::GetTypeInfo(UINT iTInfo, LCID lcid, LPTYPEINFO* ppTInfo) { return S_OK; }
	STDMETHODIMP BrowserCtrlSink::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) { return S_OK; }

	STDMETHODIMP BrowserCtrlSink::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid , WORD wFlags,
								DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO*pExcepInfo, UINT*puArgErr) {
		if (!pDispParams) return E_INVALIDARG;
 		switch (dispIdMember) {
			case DISPID_BEFORENAVIGATE2:

				this->BeforeNavigate2(pDispParams->rgvarg[6].pdispVal,
								pDispParams->rgvarg[5].pvarVal,
								pDispParams->rgvarg[4].pvarVal,
            					pDispParams->rgvarg[3].pvarVal,
            					pDispParams->rgvarg[2].pvarVal,
            					pDispParams->rgvarg[1].pvarVal,
            					pDispParams->rgvarg[0].pboolVal);
	    		return S_OK;
			case DISPID_NAVIGATECOMPLETE2: {
				if (_browser->_wndProcServer == 0) {
					_browser->_hwndView = GetTopWindow(this->_browser->_hwndShell); //view
					_browser->_hwndServer = GetTopWindow(_browser->_hwndView); // server
					SetProp(_browser->_hwndServer, "BrowserCtrl*", _browser);
					_browser->_wndProcServer = (WNDPROC) SetWindowLong(_browser->_hwndServer, GWL_WNDPROC, (LONG)BrowserCtrl::serverWndProc);
				}
				return S_OK;}
			case DISPID_PROGRESSCHANGE:
				ProgressChange(pDispParams->rgvarg[1].lVal, pDispParams->rgvarg[0].lVal);
				return S_OK;
			case DISPID_DOCUMENTCOMPLETE:
				DocumentComplete(0, 0);
				return S_OK;
				
		}
		return DISP_E_MEMBERNOTFOUND;
	}
	// DWebBrowserEvents2




}}