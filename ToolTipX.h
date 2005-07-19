#ifndef __STAMINA_TOOLTIPX__
#define __STAMINA_TOOLTIPX__

/*
 *  Stamina.LIB
 *  
 *  Please READ /License.txt FIRST! 
 * 
 *  Copyright (C)2003,2004,2005 Rafa³ Lindemann, Stamina
 *
 *  $Id$
 */


#include <boost/shared_ptr.hpp>
#include <deque>
#include <map>

#include "RichHtml.h"
#include "Rect.h"
#include "Region.h"
#include "Image.h"
#include "ObjectImpl.h"
#include "iToolTipX.h"

namespace Stamina { namespace ToolTipX {

	class TipImpl:public SharedObject<iTip> {
	};
	class TargetImpl:public SharedObject<iTarget> {
	};

	class Target:public TargetImpl {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::Target, iTarget);

		Target() {}
		Target(oTip tip):_tip(tip) {}
		virtual void setTip(ToolTip * ttx, const oTip& tip);
		oTip getTip(ToolTip * ttx) {return _tip;}
		// zawsze w obrêbie, ¿eby automat nam go nie wywali³
		bool mouseAtTarget(ToolTip * ttx,HWND window, const Point& screen, const Point& local) {return true;}
	protected:
		oTip _tip;
	};
	class TargetFixed:public Target {
	public:
		TargetFixed(const oTip& tip):Target(tip) {}
		enPositioning getPositioning() {return positionFixed;}
	};


	class TargetWindow: public Target{
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::TargetWindow, Target);
		TargetWindow(const oTip& tip, HWND hwnd): _hwnd(hwnd), Target(tip) {}
		bool mouseAtTarget(ToolTip * ttx, HWND window, const Point& screen, const Point& local);
		bool operator==(const iTarget & b) const {return iTarget::operator==(b) && static_cast<const TargetWindow&>(b)._hwnd == _hwnd;}
	protected:
		HWND _hwnd;
	};
	class TargetId: public Target {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::TargetId, Target);

		TargetId(const oTip& tip, int id):_id(id), Target(tip) {}
		bool mouseAtTarget(ToolTip * ttx,HWND window, const Point& screen, const Point& local);
		bool operator==(const iTarget & b) const {return iTarget::operator==(b) && static_cast<const TargetId&>(b)._id == _id;}
	protected:
		int _id;
	};
	class TargetRect: public Target {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::TargetRect, Target);

		TargetRect(const oTip& tip, const Rect& rc):_rect(rc), Target(tip) {}
		bool mouseAtTarget(ToolTip * ttx,HWND window, const Point& screen, const Point& local);
		bool operator==(const iTarget & b) const;
	protected:
		Rect _rect;
	};

	class iTipCallBack: public TipImpl {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::iTipCallBack, iTip);

		void adjustSize(HDC hdc, Rect& rc);
		void paint(HDC hdc, const Rect& rc);
		void attach(HWND hwnd);
		void detach(HWND hwnd);
		int parentMessageProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam, bool& returned);
		bool checkCache();
		virtual void fillCache()=0;
		virtual void resetCache() {_cachedTip = oTip();}
	protected:
		oTip _cachedTip;
	};



	class TipText: public TipImpl {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::TipText, iTip);

		TipText(const CStdString& txt, HFONT font=0, int width = 0);
		void adjustSize(HDC hdc, Rect& rc);
		void paint(HDC hdc, const Rect& rc);
		void setText(const CStdString& text) {_text = text;}
		void setWidth(int width) {this->_width = width;}
		CStdString getText() {return _text;}
	protected:
		CStdString _text;
		HFONT _font;
		int _width;
	};
	class TipTextRich: public TipImpl {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::TipTextRich, iTip);

		TipTextRich(const CStdString& txt, HFONT font=0, RichEditFormat::fSetStyleCB styleCB=0, int width = 0);
		~TipTextRich();
		void adjustSize(HDC hdc, Rect& rc);
		void paint(HDC hdc, const Rect& rc);
		void setText(const CStdString& text);
		void setWidth(int width) {this->_width = width;}
		void attach(HWND hwnd);
		/** Tip odpinany jest od okna */
		void detach(HWND hwnd);
		int parentMessageProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam, bool& returned);
		CStdString getText();
		static TipTextRich* fromHWND(HWND window);
	protected:
		HFONT _font;
		CStdString _text;
		RichEditFormat::fSetStyleCB _styleCB;
		Rect _rect;
		HWND _rich;
		int _width;
		bool _positioned;
	};

	class TipOnlyImage : public TipImpl {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::TipOnlyImage, iTip);

		TipOnlyImage(const oImage& image);
		~TipOnlyImage();
		void adjustSize(HDC hdc, Rect& rc);
		void paint(HDC hdc, const Rect& rc);
		void setImage(const oImage& image);
		void resetImage();
		oImage getImage() {return _image;}

	protected:
		oImage _image;
	};
	class TipOnlyTitle : public TipOnlyImage {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::TipOnlyTitle, TipOnlyImage);

		TipOnlyTitle(const CStdString & title, const oImage& icon=oImage(), HFONT font=0);
		~TipOnlyTitle();
		void adjustSize(HDC hdc, Rect& rc);
		void paint(HDC hdc, const Rect& rc);
		void setFont(HFONT font);
		void setTitle(const CStdString & title) {this->_title = title;}
		CStdString getTitle() {return _title;}
	private:
		CStdString _title;
		HFONT _font;
	};

	class Tip : public TipImpl {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::Tip, iTip);

		enum enAlign {
			alignAuto = 0,
			alignLeft = 1,
			alignRight = 2,
			alignWrap = 4
		};
		class Element {
		public:
			Element(oTip tip, Rect rc, enAlign align = alignAuto):tip(tip),rc(rc),align(align) {}
			iTip * operator->() {return tip.get();}
			oTip tip;
			Rect rc;
			enAlign align;
		};
		static oTip titleAndText(const oImage& image, const CStdString & title, const CStdString & text, bool textRich=false);
		static oTip titleTextAndImage(const oImage& icon, const CStdString & title, const CStdString & text, bool textRich, const oImage& image);
		static oTip textAndImage(const CStdString & text, bool textRich, const oImage& image);
		void add(oTip tip, enAlign align = alignAuto);
		void add(iTip * tip, enAlign align = alignAuto) {
			this->add(oTip(tip), align);
		}
		void adjustSize(HDC hdc, Rect& rc);
		void paint(HDC hdc, const Rect& rc);
		void attach(HWND hwnd);
		void detach(HWND hwnd);
		bool empty() {return this->_elements.empty();}
		int parentMessageProc(HWND hwnd, int message, WPARAM wParam, LPARAM lParam, bool & returned);

	private:
		typedef std::deque<Element> tElements;
		static const int wrapWidth = 400;
		tElements _elements;
	};


	class ToolTip: LockableObject<iLockableObject> {
	public:
		STAMINA_OBJECT_CLASS(ToolTipX::ToolTip, iLockableObject);

		ToolTip(HWND parent, HINSTANCE hInst);
		ToolTip();
		void create(HWND parent, HINSTANCE hInst);
		~ToolTip();
		static ToolTip * fromHWND(HWND hwnd);
		/** 
		@param window oknie z kontrolkami
		*/
		void mouseMovement(HWND window, const Point& screen, int scrollX=-1, int scrollY=-1);
		void mouseMovement(HWND window, const Point& screen, const Point& local);
		void mouseMovement(HWND window, int scrollX=-1, int scrollY=-1);
		/** Myszka "rusza" siê w kontrolce
		*/
		//	void mouseMovementInTarget(HWND window, int x, int y);
		void hide(bool deactivate = false); // ukrywa tip'a
		void hideAuto(); // ukrywa tip'a
		void show(bool wait = false); // pokazuje tip'a
		bool visible();
		void refresh();
		void refreshRegion();
		void addTarget(HWND hwnd, oTarget &);
		void removeTarget(HWND hwnd, const oTarget &);
		void clearTargets(HWND hwnd);
		void setTip(HWND hwnd, oTarget &, bool treatAsAuto);
		void setPos(const Point& screen, bool offset, enPositioning position, enPlacement place = pNone);
		Point getPlacementPos(const Point& screen, int offset, enPlacement place);
		//	void setFloating(bool floating) {this->_lockedPos=!floating;}
		/** Ustawia tip'a i zeruje aktualny target.
		@warning przekazany obiekt zostanie USUNIÊTY! */
		void setTip(iTip *, bool treatAsAuto);
		void reset();
		void attach();
		oTarget getCurrentTarget() {return _currentTarget;}
		bool isCurrentTarget(iTarget * target) {return (_currentTarget.get() && target) && *_currentTarget.get() == *target;}
		HWND getTipWindow();
		HWND getCurrentWindow() {return _currentWindow;}
		bool isAutomated() {return this->_automated;}
		bool isAttached() {return this->_attached;}

		unsigned int enterWait; // czas oczekiwania na wyœwietlenie
		unsigned int enterSiblingTimeout;
		unsigned int hideTimeout;
		unsigned int moveDelay;

	private:
		typedef std::multimap<HWND, oTarget> tTargets;


		tTargets::iterator findTarget(HWND hwnd, const iTarget & target); 
		tTargets::iterator findTarget(HWND hwnd, const oTarget & target) {return findTarget(hwnd, *target.get());}
		static void registerClass(HINSTANCE hInst);
		static int CALLBACK windowProc(HWND hwnd , int message , WPARAM wParam, LPARAM lParam);
		void adjustClientRect(Rect& rc);
		void adjustTipRect(Rect& rc);
		static VOID CALLBACK timerShowHideProc(ToolTip * tip , DWORD low , DWORD high);

		oTarget _currentTarget;
		HWND _currentWindow;
		HWND _hwnd;
		tTargets _targets;
		bool _active;
		HANDLE _timer;
		DWORD _lastShown;
		DWORD _lastPlacementSwitch;
		DWORD _lastPositioned;
		//DWORD _lastReading;
		Region _region;
		enPositioning _defaultPosition;
		bool _automated;
		bool _attached;
		static const int roundness = 4;
		static const int margin = 2;
		static const int marginLeft = 10;
		static const int cursorOffset = 10;
		Size _size;
		enPlacement _placement;
		Point _lastScreen;
		int _dontReset;
	};

	typedef std::multimap<HWND, ToolTip*> TooltipsReg; 
	const TooltipsReg& getRegisteredMap();

	void registerTooltip(HWND hwnd, ToolTip* tt);
	inline void registerGlobalTooltip(ToolTip* tt) {
		registerTooltip(0, tt);
	}
	void unregisterTooltip(ToolTip* tt);
	/**Unregisters all tooltip objects for specified window*/
	void unregisterTooltip(HWND hwnd);
	void unregisterTooltip(HWND hwnd, ToolTip* tt);
	

	void mouseMovement(HWND hwnd, Point scroll = Point(-1, -1));


} }
#endif