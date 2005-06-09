/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 */

#pragma once
#include <windows.h>
#include <Richedit.h>
#include <stdstring.h>

namespace Stamina {

	class RichEditFormat {
	public:
		struct SetStyle {
			SetStyle(CHARFORMAT2 & cf , PARAFORMAT2 & pf): cf(cf), pf(pf) {}
			CHARFORMAT2& cf;
			PARAFORMAT2& pf;

			inline void setBold(bool bold) {
				cf.dwMask |= CFM_BOLD;
				cf.dwEffects |= CFE_BOLD;
			}
			inline void setItalic(bool italic) {
				cf.dwMask |= CFM_ITALIC;
				cf.dwEffects |= CFE_ITALIC;
			}
			inline void setUnderline(bool underline) {
				cf.dwMask |= CFM_UNDERLINE;
				cf.dwEffects |= CFE_UNDERLINE;
			}
			inline void setColor(int color) {
				cf.dwMask |= CFM_COLOR;
				cf.crTextColor = color;
				cf.dwEffects &= ~CFE_AUTOCOLOR;
			}
			inline void setBackColor(int color) {
				cf.dwMask |= CFM_BACKCOLOR;
				cf.crBackColor = color;
				cf.dwEffects &= ~CFE_AUTOBACKCOLOR;
			}
			inline void setSize(int size) {
				cf.dwMask |= CFM_SIZE;
				cf.yHeight = size * 20;
			}
			inline void offsetSize(int size) {
				this->setSize(this->getSize() + size);
			}
			inline void setFont(const CStdString& face) {
				cf.dwMask |= CFM_FACE;
				strncpy(cf.szFaceName , face, sizeof(cf.szFaceName)-1);
			}

			inline int getSize() {
				return cf.yHeight / 20;
			}

		};

		typedef void (*fSetStyleCB)(const CStdString & token , const CStdString & styleClass , SetStyle & ss);

#ifdef _STACK_

		RichEditFormat(HWND hwnd);

		void insertHTML(const CStdString& body , fSetStyleCB styleCB = 0);
		void clear();
		void insertText(const CStdString& body);
		void insertNewLine(int paragraph); /* 1-BR 2-P */

		void operate(int oper);
		void push(bool apply=true);
		void pop(bool apply=true);
		void apply(); // ustawia format

		void setBold(bool bold, bool apply=true);
		void setItalic(bool italic, bool apply=true);
		void setUnderline(bool underline, bool apply=true);
		void setColor(int color, bool apply=true);
		void setSize(int size, bool apply=true);
		void setFont(const CStdString& face, bool apply=true);

		template <class TY> 
		class FormatStack : public std::stack<TY> {
		public:
			void operate(int oper , TY & v) {
				if (oper > 0) {
					this->push(v);
				} else if (oper < 0) {
					if (this->size() < 2) return; // za ma³o...
					this->pop();
				}
			}
			void reset() {
				while (this->size() > 1)
					this->pop();
			}
		};


		struct State {
			FormatStack <CHARFORMAT2> cf;
			FormatStack <PARAFORMAT2> pf;
			FormatStack <DWORD> maskcf;
			FormatStack <DWORD> maskpf;
		} _state;
		CHARFORMAT2 _cf;
		PARAFORMAT2 _pf;


	private:
		HWND _hwnd;

#endif
	};



	class RichEditHtml {
	public:
		static HWND create(int style , int x , int y , int w , int h , HWND parent , HMENU id);
		static void insertHTML(HWND hwnd ,  const CStdString& body , RichEditFormat::fSetStyleCB styleCB = 0);
		static void setHTML(HWND hwnd , const CStdString & body , RichEditFormat::fSetStyleCB styleCB = 0);
		static void init(HWND hwnd);
		static void init(HWND hwnd, bool getLinks, COLORREF color);
		static void clear(HWND hwnd);
		static bool isReadOnly(HWND hwnd);
		static int CALLBACK parentWindowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam);
		static int CALLBACK windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam);
	};


};