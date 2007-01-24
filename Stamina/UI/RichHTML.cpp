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

The Initial Developer of the Original Code is "STAMINA" - Rafa³ Lindemann.
Portions created by STAMINA are 
Copyright (C) 2003-2006 "STAMINA" - Rafa³ Lindemann. All Rights Reserved.

Contributor(s): 

--

$Id$

*/


/*
TEST:

bleeeee<b>bold<b><font color="#FF0000">gnie¿<u>d¿ony</u></font></b>i <i>jesz</i>cze</b>koniec<br/>
SPANY:<br/>
piiiipii<span class="mark">MARKO-<span class="mark">WAN</span>IE</span><br/>
<p align="center>Œrodek</p>W nowej linii<div align="right">po prawo</div>

*/

#include "stdafx.h"
#include <stack>
#include <shellapi.h>
#include "RichHTML.h"
#include "../SimXML.h"
#include "../WinHelper.h"
#include "../Helpers.h"
using namespace std;

namespace Stamina { namespace UI {

	HWND RichEditHtml::create(int style , int x , int y , int w , int h , HWND parent , HMENU id) {
		HWND hwnd = CreateWindowEx(0 , RICHEDIT_CLASS  , "" , style | WS_CHILD|WS_VISIBLE|ES_MULTILINE | ES_AUTOVSCROLL | 
			ES_READONLY | WS_VSCROLL/* | WS_TABSTOP*/
			,x,y,w,h,parent,id , 0 , 0);
		init(hwnd);
		return hwnd;
	}
	void RichEditHtml::init(HWND hwnd) {
		init(hwnd, true, GetSysColor(COLOR_3DFACE));
	}
	void RichEditHtml::init(HWND hwnd, bool getLinks, COLORREF color) {
		if (GetProp(hwnd, "RichEditHtml::SubProc")) return;
		SendMessage(hwnd , EM_SETLANGOPTIONS , 0 , 0);
		SendMessage(hwnd , EM_AUTOURLDETECT , getLinks ? 1 : 0 , 0);
		SendMessage(hwnd , EM_SETEVENTMASK  , 0 , ENM_SELCHANGE | (getLinks ? ENM_LINK : 0));
		SendMessage(hwnd , EM_SETBKGNDCOLOR , 0 , color);
		SetProp(hwnd, "RichEditHtml::SubProc", (HANDLE)SetWindowLong(hwnd, GWL_WNDPROC, (LONG)RichEditHtml::windowProc));
		if (GetWindowLong(hwnd, GWL_STYLE) & ES_READONLY) {
			HideCaret(hwnd);
		}
	}
	bool RichEditHtml::isReadOnly(HWND hwnd) {
		return (GetWindowLong(hwnd, GWL_STYLE) & ES_READONLY) != 0;
	}


	void RichEditHtml::setHTML(HWND hwnd , const StringRef & body , RichEditFormat::fSetStyleCB styleCB) {
		SendMessage(hwnd , WM_SETREDRAW , 0,0);
		SetWindowText(hwnd , "");
		SendMessage(hwnd , EM_SETSEL , 0 , 0);
		insertHTML(hwnd , body , styleCB);
		SendMessage(hwnd , EM_SETSEL , 0 , 0);
		if (GetWindowLong(hwnd, GWL_STYLE) & ES_READONLY) {
			HideCaret(hwnd);
		}
		POINT pt;
		pt.x = pt.y = 0;
		SendMessage(hwnd , EM_SETSCROLLPOS , 0 , (int)&pt);
		SendMessage(hwnd , EM_HIDESELECTION , 1 , 0);
		SendMessage(hwnd , WM_SETREDRAW , 1,0);
		repaintWindow(hwnd);
	}


	DWORD CALLBACK StreamInNewLine(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb) {
		int & lines = *(int*)dwCookie;
		if (lines == 0 || cb < 2) {
			*pcb = 0;
		} else {
			/*		char * buff; 
			if (lines == 2) {
			buff = "{\\rtf\\par}";
			} else {
			buff = "{\\rtf\\line}";
			}
			*pcb = strlen(buff);
			memcpy(pbBuff , buff , *pcb);
			*/
			if (lines == 2) {
				pbBuff[0] = '\r';
			}
			pbBuff[lines - 1] = '\n';
			*pcb = lines;
			lines = 0;
		}
		return 0;
	}

#define COLOR_SWAP(c) ((c & 0x00FF0000)>>16) | (c&0xFF00) | ((c & 0x00FF)<<16)

	void RichEditHtml::insertHTML(HWND hwnd ,  const StringRef& body , RichEditFormat::fSetStyleCB styleCB) {
		RichEditFormat f(hwnd);
		f.insertHTML(body, styleCB);
	}
	void RichEditHtml::clear(HWND hwnd) {
		SetWindowText(hwnd, "");
	}

	void RichEditFormat::insertHTML(const StringRef& insertBody , fSetStyleCB styleCB) {
		SXML xml;
		RegEx preg;
		String body;// (insertBody);
		body = preg.replace("#\\r|\\n|\\&\\#1[30];#" , "" , insertBody.a_str());
		//	body = preg.replace("#<br/?>#is" , "" , body);
		/* HACK */
		body = preg.replace("#(?<!\\>)<br/?>(?!\\<)#is" , "\n" , body.a_str());
		body = preg.replace("#<br>#is" , "<br/>" , body.a_str());
		xml.loadSource(body.a_str());
		SXML::NodeWalkInfo ni;
		size_t last = 0 , lastApply = -1 , current = 0;

		String prefix = "";
		String suffix = "";
		while (xml.nodeWalk(ni)) {
			xml.pos.start = xml.pos.end;
			xml.pos.start_end = xml.pos.end_end;
			if (last < ni.start) {
				SETTEXTEX ste;
				ste.codepage = CP_ACP;
				ste.flags = ST_SELECTION | ST_KEEPUNDO;
				SendMessage(_hwnd , EM_SETTEXTEX , (int)&ste , (LPARAM)(prefix + String(decodeEntities(body.substr(last , ni.start - last))) + suffix).c_str());
			}
			current += ni.start - last;
			last = ni.end;
			String token = ni.path.substr(ni.path.find_last_of('/')+1);
			prefix = "";
			suffix = "";
			token.makeLower();
			int newParagraph = 0;
			int oper = (ni.state == SXML::NodeWalkInfo::opened)? 1 : -1;
			if (ni.state == SXML::NodeWalkInfo::closed)
				oper = 0;
			if (oper < 0) {
				/* generalnie... nic nie trzeba robiæ... */
			} else if (oper == 0) {
				if (token == "br")
					newParagraph = 1;
				_cf.dwMask = _pf.dwMask = 0;
			} else if (token == "b") {
				_cf.dwMask |= CFM_BOLD;
				_cf.dwEffects |= CFE_BOLD;
			} else if (token == "i") {
				_cf.dwMask |= CFM_ITALIC;
				_cf.dwEffects |= CFM_ITALIC;
			} else if (token == "u") {
				_cf.dwMask |= CFM_UNDERLINE;
				_cf.dwEffects |= CFM_UNDERLINE;
			} else if (token == "sub") {
				_cf.dwMask |= CFM_SUBSCRIPT;
				_cf.dwEffects |= CFE_SUBSCRIPT;
			} else if (token == "sup") {
				_cf.dwMask |= CFM_SUPERSCRIPT;
				_cf.dwEffects |= CFE_SUPERSCRIPT;
			} else if (token == "sup") {
			} else if (token == "font") {
				_cf.dwMask |= CFM_COLOR | CFM_FACE | CFM_SIZE;
				String color = xml.getAttrib("color");
				String face = xml.getAttrib("face");
				String size = xml.getAttrib("size");
				/* W aktualnym CF ju¿ siedzi ostatnio ustawiony kolor, czy font, nie trzeba wiêc nic kombinowaæ :) */
				if (!color.empty())
					_cf.crTextColor = COLOR_SWAP(chtoint(color));
				if (!face.empty())
					strncpy_s(_cf.szFaceName, sizeof(_cf.szFaceName)-1, face.c_str(), min(sizeof(_cf.szFaceName)-1, face.length()));
				/* parametr Size to ogólnie liczby dodatnie.. * 20 daj¹ dobr¹ wysokoœæ... */
				if (!size.empty())
					_cf.yHeight = chtoint(size) * 20;
			} else if (token == "div" || token == "p") {
				String align = xml.getAttrib("align");
				if (!align.empty()) {
					_pf.dwMask |= PFM_ALIGNMENT;
					int alignment = 0;
					if (align == "center") alignment = PFA_CENTER;
					else if (align == "right") alignment = PFA_RIGHT;
					else if (align == "justify") alignment = PFA_JUSTIFY;
					else alignment = PFA_LEFT;
					_pf.wAlignment = alignment;
				}
			}
			if (oper > 0 && styleCB) {
				// Nak³adamy styl...
				String styleClass = xml.getAttrib("class");
				if (!styleClass.empty()) {
					SetStyle ss (_cf , _pf);
					styleCB(token , styleClass , ss);
				}
			}

			/* Nak³adamy zmiany */
			if (oper < 0) {
				if (token == "p" || token == "div")
					newParagraph = 2;
			}
			this->operate(oper);

			if (newParagraph && ni.start/* && ni.end < preg.getSubjectRef().size()-2*/) {
				this->insertNewLine(newParagraph);
			}
			/* Nie przywracamy poprzedniego paragrafu ostatniej linijce... 
			¯eby przywróciæ t¹ funkcjonalnoœæ, trzeba tekst zakoñczyæ dodatkowo <br/> */
			this->apply();
			if (_cf.dwMask || _pf.dwMask)
				lastApply = current;

			//		}
		}
		if (ni.end != -1) {
			this->insertText(decodeEntities(body.substr(ni.end)));
		}
	}



	int CALLBACK RichEditHtml::parentWindowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam) {
		if (message == WM_NOTIFY) {
			ENLINK* enl;
			enl = (ENLINK*)lParam;
			if (enl->nmhdr.code == EN_LINK) {
				if (enl->msg == WM_LBUTTONUP) {
					TEXTRANGE tr;
					tr.chrg = enl->chrg;
					if ((tr.chrg.cpMax - tr.chrg.cpMin) > 500) return 0;
					tr.lpstrText = new char [tr.chrg.cpMax - tr.chrg.cpMin + 1];
					tr.lpstrText[0] = 0;
					if (SendMessage(enl->nmhdr.hwndFrom , EM_GETTEXTRANGE  , 0 , (LPARAM)&tr) && tr.lpstrText[0])
						ShellExecute(0 , "open" , tr.lpstrText , "" , "" , SW_SHOW);
					delete [] tr.lpstrText;
				}
			} else if (enl->nmhdr.code == EN_SELCHANGE) {
				if (isReadOnly(enl->nmhdr.hwndFrom))
					HideCaret(enl->nmhdr.hwndFrom);
			}
		}
		return 0;
	}
	int CALLBACK RichEditHtml::windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam) {
		switch (message) {
		case WM_SYSKEYDOWN: case WM_KEYDOWN:
			if (!isReadOnly(hwnd)) break;
			if (wParam != VK_UP 
				&& wParam != VK_DOWN 
				&& wParam != VK_LEFT
				&& wParam != VK_RIGHT
				&& wParam != VK_LEFT
				&& wParam != 'C'
				) {
					return 1;
				}
				break;
		}
		return CallWindowProc((WNDPROC)GetProp(hwnd, "RichEditHtml::SubProc"), hwnd, message, wParam, lParam);

	}

	RichEditFormat::RichEditFormat(HWND hwnd) {
		this->_hwnd = hwnd; 
		memset(&_cf , 0 , sizeof(_cf));
		memset(&_pf , 0 , sizeof(_pf));
		_cf.cbSize = sizeof(_cf);
		_pf.cbSize = sizeof(_pf);

		_cf.dwMask = -1; 
		SendMessage(hwnd , EM_GETCHARFORMAT , SCF_SELECTION , (LPARAM)&_cf);
		_pf.dwMask = -1;
		SendMessage(hwnd , EM_GETPARAFORMAT , 0 , (LPARAM)&_pf);

		_state.cf.push(_cf);
		_state.pf.push(_pf);
		_state.maskcf.push(0);
		_state.maskpf.push(0);

		_cf.dwMask = 0;
		_pf.dwMask = 0;
	}
	void RichEditFormat::insertText(const StringRef & body) {
		SETTEXTEX ste;
		ste.codepage = CP_ACP;
		ste.flags = ST_SELECTION | ST_KEEPUNDO;
		SendMessage(_hwnd , EM_SETTEXTEX , (int)&ste , (LPARAM)(body.c_str()));
	}
	void RichEditFormat::insertNewLine(int paragraph) {
		EDITSTREAM es;
		es.pfnCallback = StreamInNewLine;
		int lines = paragraph;
		es.dwCookie = (DWORD) &lines;
		SendMessage(_hwnd , EM_STREAMIN , SF_TEXT | SFF_SELECTION , (LPARAM)&es);
	}

	void RichEditFormat::push(bool apply) {
		operate(1);
		if (apply) this->apply();
	}
	void RichEditFormat::pop(bool apply) {
		operate(-1);
		if (apply) this->apply();
	}
	void RichEditFormat::operate(int oper) {
		if (oper) {
			if (oper < 0) {
				/* Przy "schodzeniu" nak³adamy poprzedni (po operate) format z aktualn¹ mask¹... */
				_cf.dwMask = _state.maskcf.top();
				_pf.dwMask = _state.maskpf.top();
			}
			if (_cf.dwMask)
				_state.cf.operate(oper , _cf);
			if (_pf.dwMask)
				_state.pf.operate(oper , _pf);
			_state.maskcf.operate(oper , _cf.dwMask);
			_state.maskpf.operate(oper , _pf.dwMask);
			if (oper < 0) {
				DWORD cfmask = _cf.dwMask;
				DWORD pfmask = _pf.dwMask;
				_cf = _state.cf.top();
				_pf = _state.pf.top();
				_cf.dwMask = cfmask;
				_pf.dwMask = pfmask;
			}
		} else
			_cf.dwMask = _pf.dwMask = 0;
	}

	void RichEditFormat::apply() {
		if (_pf.dwMask/* && ni.end < preg.getSubjectRef().size()-2*/) {
			SendMessage(_hwnd , EM_SETPARAFORMAT , 0 , (LPARAM)&_pf);
			_pf.dwMask = 0;
		}
		if (_cf.dwMask) {
			SendMessage(_hwnd , EM_SETCHARFORMAT , SCF_SELECTION , (LPARAM)&_cf);
			_cf.dwMask = 0;
		}
	}
	void RichEditFormat::clear() {
		RichEditHtml::clear(this->_hwnd);

		_state.cf.reset();
		_state.pf.reset();
		_state.maskcf.reset();
		_state.maskpf.reset();
		this->apply();
	}

	void RichEditFormat::setBold(bool bold, bool apply) {
		_cf.dwMask |= CFM_BOLD;
		_cf.dwEffects |= CFE_BOLD;
		if (apply) this->push();
	}
	void RichEditFormat::setItalic(bool italic, bool apply) {
		_cf.dwMask |= CFM_ITALIC;
		_cf.dwEffects |= CFE_ITALIC;
		if (apply) this->push();
	}
	void RichEditFormat::setUnderline(bool underline, bool apply) {
		_cf.dwMask |= CFM_UNDERLINE;
		_cf.dwEffects |= CFE_UNDERLINE;
		if (apply) this->push();
	}
	void RichEditFormat::setColor(int color, bool apply) {
		_cf.dwMask |= CFM_COLOR;
		_cf.crTextColor = color;
		if (apply) this->push();
	}
	void RichEditFormat::setSize(int size, bool apply) {
		_cf.dwMask |= CFM_SIZE;
		_cf.yHeight = size * 20;
		if (apply) this->push();
	}
	void RichEditFormat::setFont(const StringRef& face, bool apply) {
		_cf.dwMask |= CFM_FACE;
		strncpy_s(_cf.szFaceName, sizeof(_cf.szFaceName)-1, face.c_str(), min(sizeof(_cf.szFaceName)-1, face.length()));
		if (apply) this->push();
	}
	void RichEditFormat::setAlignment(int align, bool apply) {
		_pf.dwMask |= PFM_ALIGNMENT;
		_pf.wAlignment = PFA_CENTER;
		if (apply) this->push();
	}


} };