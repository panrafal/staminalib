#include <stdafx.h>
#include <math.h>
#include <cppunit/extensions/HelperMacros.h>
#include <boost/pool/pool.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <Stamina/VersionControl.h>
#include <Stamina/CriticalSection.h>
#include <Stamina/Helpers.h>
#include <Stamina/Lock.h>
#include <Stamina/Mutex.h>
#include "..\String.h"

#include <ConvertUTF.h>

using namespace Stamina;

__declspec(noinline) int testCall(int a) {
	return max(a, 0);
}


class TimeStringBuffer : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TimeStringBuffer );
  
	//CPPUNIT_TEST( timeFullStaticInit );
	//CPPUNIT_TEST( timeStaticInit );
	//CPPUNIT_TEST( timeHalfStaticInit );
	//CPPUNIT_TEST( timeDynamicInit );
	//CPPUNIT_TEST( timeDynamicInit2 );
	//CPPUNIT_TEST( timeShortPoolInit );
	//CPPUNIT_TEST( timePoolInit );
	//CPPUNIT_TEST( timeSingletonPoolInit );
	//CPPUNIT_TEST( timeSingletonPoolInit2 );
	//CPPUNIT_TEST( timeSingletonPoolInit3 );
	//CPPUNIT_TEST( timeSingletonPoolInit4 );
	//CPPUNIT_TEST( timeSingletonPoolInit5 );
	//CPPUNIT_TEST( timeMyPoolInit );

	//CPPUNIT_TEST( timeWideToCharConv );
	//CPPUNIT_TEST( timeCharToWideConv );
	//CPPUNIT_TEST( timeCharToUTF8Conv );
	//CPPUNIT_TEST( timeUTF8ToCharConv );
	//CPPUNIT_TEST( timeCharToUTF8Conv2 );
	//CPPUNIT_TEST( timeUTF8ToCharConv2 );


	CPPUNIT_TEST_SUITE_END();

protected:

public:

	TimeStringBuffer() {
		std::wstring fillW (L"≥ÛÒπúÊ£•”å—∆Øè1234567890ABCDefgh");
		for (int i = 0; i < txtLength; i++) {
			txtW[i] = fillW[i % fillW.length()];
		}
		txtW[txtLength] = 0;

		std::string fillA ("≥ÛÒπúÊ£•”å—∆Øè1234567890ABCDefgh");
		for (int i = 0; i < txtLength; i++) {
			txtA[i] = fillA[i % fillA.length()];
		}
		txtA[txtLength] = 0;

		WideCharToMultiByte(CP_UTF8, 0, txtW, txtLength, txtU, txtLength * 2, 0, 0);
		txtULength = strlen(txtU);
	}

	void setUp() {

	}
	void tearDown() {
	}

protected:

	const static int initBytes = 32;
	const static int txtLength = 320;
	int txtULength;
	const static int timingLoops = 2000000;
	const static bool freeData = true;

	wchar_t txtW [txtLength + 1];
	char txtA [txtLength + 1];
	char txtU [txtLength * 2];

	void timeStaticInitAlloc() {
		class Test {
		public:
			char check [initBytes];
		};
		Test a;
		a.check [1] = 'a';
	}

	void timeStaticInit() {

		for (int i = 0; i < timingLoops; ++i) {
			timeStaticInitAlloc();
		}
	}
	void timeFullStaticInit() {
		class Test {
		public:
			char check [initBytes];
		};
		for (int i = 0; i < timingLoops; ++i) {
			Test a;
			a.check [1] = 'a';
		}
	}


	void timeHalfStaticInit() {
		class Test {
		public:
			char check [initBytes];
		};

		for (int i = 0; i < timingLoops; ++i) {
			{
				Test* a = new Test();
				a->check [1] = 'a';
				if (freeData) delete a;
			}
		}
	}


	void timeDynamicInit() {
		for (int i = 0; i < timingLoops; ++i) {
			char * a = (char*) malloc(initBytes);
			a[1] = 'a';
			if (freeData) free(a);
		}
	}

	void timeDynamicInit2() {
		for (int i = 0; i < timingLoops; ++i) {
			char * a = new char [initBytes];
			a[1] = 'a';
			if (freeData) delete a;
		}
	}

	void timeShortPoolInit() {
		boost::pool<> p (sizeof(char));
		for (int i = 0; i < timingLoops; ++i) {
			char * a = (char*)p.ordered_malloc(initBytes);
			a[1] = 'a';
			if (freeData) p.free(a, initBytes);
		}
	}

	void timePoolInit() {
		boost::pool<> p (initBytes);
		for (int i = 0; i < timingLoops; ++i) {
			char * a = (char*)p.malloc();
			a[1] = 'a';
			if (freeData) p.free(a);
		}
	}

	struct MyPoolTag { };

	void timeSingletonPoolInit() {
		typedef boost::singleton_pool<MyPoolTag, initBytes> my_pool;

		for (int i = 0; i < timingLoops; ++i) {
			char * a = (char*)my_pool::malloc();
			a[1] = 'a';
			if (freeData) my_pool::free(a);
		}
	}

	struct MyPoolTag2 { };

	void timeSingletonPoolInit2() {
		typedef boost::singleton_pool<MyPoolTag2, initBytes, boost::default_user_allocator_new_delete, CriticalSection> my_pool;

		for (int i = 0; i < timingLoops; ++i) {
			char * a = (char*)my_pool::malloc();
			a[1] = 'a';
			if (freeData) my_pool::free(a);
		}
	}

	struct MyPoolTag3 { };

	void timeSingletonPoolInit3() {
		typedef boost::singleton_pool<MyPoolTag3, initBytes, boost::default_user_allocator_new_delete, Lock_blank> my_pool;

		for (int i = 0; i < timingLoops; ++i) {
			char * a = (char*)my_pool::malloc();
			a[1] = 'a';
			if (freeData) my_pool::free(a);
		}
	}

	struct MyPoolTag4 { };

	void timeSingletonPoolInit4() {
		typedef boost::singleton_pool<MyPoolTag4, initBytes, boost::default_user_allocator_new_delete, CriticalSection_> my_pool;

		for (int i = 0; i < timingLoops; ++i) {
			char * a = (char*)my_pool::malloc();
			a[1] = 'a';
			if (freeData) my_pool::free(a);
		}
	}


	struct MyPoolTag5 { };

	void timeSingletonPoolInit5() {
		typedef boost::singleton_pool<MyPoolTag5, initBytes, boost::default_user_allocator_new_delete, FastMutex> my_pool;

		for (int i = 0; i < timingLoops; ++i) {
			char * a = (char*)my_pool::malloc();
			a[1] = 'a';
			if (freeData) my_pool::free(a);
		}
	}

	void timeMyPoolInit() {
		const int tableSize = 100;
		char table [tableSize];
		char buffer [tableSize * initBytes];
		FastMutex mutex;
		for (int i = 0; i < timingLoops; ++i) {

			FastLocker<FastMutex> lock(mutex);
			int j = 0;
			while (1) { // symulujemy szukanie w tablicy
				bool ok = table[j % tableSize] == 0;
                if (j >= i) break;
				j++;
			}
			table[j % tableSize] = 1;
			char* a = buffer + ((j % tableSize) * initBytes);
			a[1] = 'a';
			table[j % tableSize] = 0;
		}
	}

	void timeWideToCharConv() {
		char buff [txtLength + 1];
		for (int i = 0; i < timingLoops; ++i) {
			WideCharToMultiByte(CP_ACP, 0, txtW, txtLength, buff, txtLength + 1, 0, 0);
		}
	}

	void timeCharToWideConv() {
		wchar_t buff [txtLength + 1];
		for (int i = 0; i < timingLoops; ++i) {
			MultiByteToWideChar(CP_ACP, 0, txtA, txtLength, buff, txtLength + 1);
		}
	}

	void timeCharToUTF8Conv() {
		wchar_t buff16 [txtLength * 2];
		char buff8 [txtLength * 2];
		for (int i = 0; i < timingLoops; ++i) {
			MultiByteToWideChar(CP_ACP, 0, txtA, txtLength, buff16, txtLength * 2);
			WideCharToMultiByte(CP_UTF8, 0, buff16, txtLength, buff8, txtLength * 2, 0, 0);
		}
	}

	void timeUTF8ToCharConv() {
		wchar_t buff16 [txtLength * 2];
		char buff8 [txtLength * 2];
		for (int i = 0; i < timingLoops; ++i) {
			MultiByteToWideChar(CP_UTF8, 0, txtU, txtULength, buff16, txtLength * 2);
			WideCharToMultiByte(CP_ACP, 0, buff16, txtLength, buff8, txtLength * 2, 0, 0);
		}
	}

	void timeCharToUTF8Conv2() {
		wchar_t buff16 [txtLength * 2];
		char buff8 [txtLength * 2];
		for (int i = 0; i < timingLoops; ++i) {
			MultiByteToWideChar(CP_ACP, 0, txtA, txtLength, buff16, txtLength * 2);
			const UTF16 * _buff16 = buff16;
			UTF8 * _buff8 = (UTF8*)buff8;
			ConvertUTF16toUTF8(&_buff16, _buff16 + txtLength, &_buff8, (_buff8 + (txtLength * 2)), strictConversion);
		}
	}

	void timeUTF8ToCharConv2() {
		wchar_t buff16 [txtLength * 2];
		char buff8 [txtLength * 2];
		for (int i = 0; i < timingLoops; ++i) {
			UTF16 * _buff16 = buff16;
			const UTF8 * _buff8 = (UTF8*)txtU;
			ConvertUTF8toUTF16(&_buff8, _buff8 + txtULength, &_buff16, _buff16 + (txtLength * 2), strictConversion);
			WideCharToMultiByte(CP_ACP, 0, buff16, txtLength, buff8, txtLength * 2, 0, 0);
		}
	}



};

CPPUNIT_TEST_SUITE_REGISTRATION( TimeStringBuffer );

