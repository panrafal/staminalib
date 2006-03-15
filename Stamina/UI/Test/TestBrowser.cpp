#include <stdafx.h>
#include <cppunit/extensions/HelperMacros.h>
#include <Stamina\UI\WindowFrame.h>
#include <Stamina\WinHelper.h>
#include "..\BrowserCtrl.h"

using namespace Stamina;
using namespace Stamina::UI;

using std::string;
using std::cout;
using std::endl;

namespace Stamina { namespace UI {

	class WindowTestCtrl: public WindowFrame {
	public:

		WindowTestCtrl (const char* title, Rect rc): WindowFrame(title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, rc, 0, 0) {
		}

		virtual int onPaint() {
			PAINTSTRUCT ps;
			BeginPaint(this->_hwnd, &ps);
			EndPaint(this->_hwnd, &ps);
			return 1;
		}

	};


	class BrowserCtrlSink_Test: public BrowserCtrlSink {
	public:
		void __stdcall ProgressChange(long a, long b) {
			std::cout << "Sink::ProgressChange a=" << a << " b=" << b << endl;
		}
		void __stdcall DocumentComplete(IDispatch*, VARIANT*) {
			std::cout << "Sink::DocumentComplete" << endl;
		}

		void __stdcall BeforeNavigate2(IDispatch*,VARIANT*,VARIANT*,VARIANT*,VARIANT*,VARIANT*,VARIANT_BOOL*) {
			std::cout << "Sink::BeforeNavigate" << endl;
		}


	};



class TestBrowser : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestBrowser );
  
	CPPUNIT_TEST( testStartup );
	CPPUNIT_TEST( testNavigate );
	CPPUNIT_TEST( testDocument );
	CPPUNIT_TEST( testShutdown );

    CPPUNIT_TEST_SUITE_END();

protected:

	static SharedPtr<Window> wnd;
	static BrowserCtrl* browser;

public:

	TestBrowser() {
	}
	void setUp() {
	}
	void tearDown() {
	}

protected:

	void testStartup() {
		wnd = new WindowTestCtrl("TestBrowser", Rect(100, 100, 550, 450));
		Stamina::processMessages(0);
		browser = new BrowserCtrl(Rect(10, 10, 500, 400), wnd->getHwnd(), new BrowserCtrlSink_Test());
		//MessageBox(0, "Czekam", "", MB_OK);
	}

	void testNavigate() {
		//browser->navigate("http://test.konnekt.info/test.php", "test1=blah&test2=ciap", "User-Agent: Konnekt blablabla\r\n");
		browser->navigate("http://test.konnekt.info/test.php", "test1=blah&test2=ciap");
		//browser->navigate("https://81.15.209.200:8445/login.html", "user=48222132379&password=imlgn6&redirectOnLoginError=http://www.konnekt.info/test.php");

		/*
		while (1) {
			SleepEx(1, 1);
			Stamina::processMessages(0);
		}
		*/
		MessageBox(0, "Czekam2", "", MB_OK);
	}

	void testDocument() {


		cout << endl << "LocationUrl:" << browser->getLocationURL() << endl;
		cout << endl << "Content:" << browser->getInnerHTML() << endl;


	}

	void testShutdown() {
		Stamina::processMessages(0);
		//delete browser;
		wnd->destroyWindow();
		wnd.reset();
		Stamina::processMessages(0);
	}


};


BrowserCtrl* TestBrowser::browser;
SharedPtr<Window> TestBrowser::wnd;


} }

CPPUNIT_TEST_SUITE_REGISTRATION( TestBrowser );
//CPPUNIT_TEST_SUITE_REGISTRATION( TestFileBinPass );
//CPPUNIT_TEST_SUITE_REGISTRATION( TestFileBinCryptAll );


