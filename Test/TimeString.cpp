#include <stdafx.h>

#define TEST_STRING

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
#include "..\StringSTL.h"

#include <ConvertUTF.h>

using namespace Stamina;



class CharType {
public:
	typedef char This;
	typedef wchar_t Other;
};
class WCharType {
public:
	typedef wchar_t This;
	typedef char Other;
};

template <class CHARTYPE>
class StaminaString {
public:
	typedef CHARTYPE Char;
	typedef Stamina::String This;
	typedef Stamina::String Other;
	typedef Stamina::StringSTL<typename Char::This> TypedThis;
	typedef Stamina::StringSTL<typename Char::Other> TypedOther;
};

template <class CHARTYPE>
class STLString {
public:
	typedef CHARTYPE Char;
	typedef std::basic_string<typename Char::This> This;
	typedef std::basic_string<typename Char::Other> Other;
	typedef std::basic_string<typename Char::This> TypedThis;
	typedef std::basic_string<typename Char::Other> TypedOther;
};


template <class STRINGTYPE>
class TimeString : public CPPUNIT_NS::TestFixture
{

protected:

	typedef typename STRINGTYPE::Char::This CHAR;
	typedef typename STRINGTYPE::Char::Other OTHER;

	// neutralne ciπgi do przechowywania danych
	typedef std::basic_string<CHAR> tString;
	typedef std::basic_string<OTHER> tOther;


	typedef typename STRINGTYPE::This tTimeString;
	typedef typename STRINGTYPE::Other tTimeOther;

	typedef typename STRINGTYPE::TypedThis tTypedString;
	typedef typename STRINGTYPE::TypedOther tTypedOther;

	const static int timingLoops = 100000;
	const static int txtLength = 320;

	CPPUNIT_TEST_SUITE( TimeString );
  
	CPPUNIT_TEST( timeCopy );
	CPPUNIT_TEST( timeCopyConvert );
	CPPUNIT_TEST( timeConvert );
	CPPUNIT_TEST( timeAdd );
	CPPUNIT_TEST( timeErase );

	CPPUNIT_TEST_SUITE_END();



protected:



	template <typename CH, int length>
	std::basic_string<CH> generateString() {
		static std::basic_string<CH> s;
		if (s.empty()) {
			std::basic_string<CH> fill = keepChar<std::basic_string<CH> > (L"≥ÛÒπúÊ£•”å—∆Øè1234567890ABCDefgh");
			s.resize(length);
			for (int i = 0; i < length; i++) {
				s[i] = fill[i % fill.length()];
			}
		}
		return s;
	}

	tString testString(const std::wstring& s) {
		if (s.empty()) {
			return generateString<CHAR, txtLength>();
		}
		return keepChar<std::basic_string<CHAR> >(s);
	}

	tOther otherString(const std::wstring& s) {
		if (s.empty()) {
			return generateString<OTHER, txtLength>();
		}
		return keepChar<std::basic_string<OTHER> >(s);
	}

	bool isWide() {
		return sizeof(CHAR) == sizeof(wchar_t);
	}


public:


	void setUp() {
		setlocale(LC_ALL, "polish");
		std::locale::global(std::locale("polish"));

	}
	void tearDown() {
	}

protected:

	void timeCopy() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString s;
		for (int i = 0; i < timingLoops; i++) {
			s = a;
		}
	}

	virtual void timeCopyConvert() {
		tTypedOther a = generateString<OTHER, txtLength>();
		tTimeString s;
		for (int i = 0; i < timingLoops; i++) {
			s = keepChar<tString> ( a.c_str() );
		}
	}

	virtual void timeConvert() {
		tTypedOther a = generateString<OTHER, txtLength>();
		tString s;
		for (int i = 0; i < timingLoops; i++) {
			s = keepChar<tString> ( a.c_str() );
		}
	}

	virtual void timeAdd() {
		tTimeString a;
		tTimeString b = generateString<CHAR, txtLength / 10>();
		for (int i = 0; i < timingLoops; i++) {
			a += b;
		}
	}

	virtual void timeErase() {
		tTimeString a = generateString<CHAR, timingLoops>();
		for (int i = 0; i < timingLoops / 4; i++) {
			a.erase(0, 4);
		}
	}

};

template <class CHARTYPE>
class TimeStringStamina : public TimeString< StaminaString<CHARTYPE> > {
	CPPUNIT_TEST_SUB_SUITE( TimeStringStamina, TimeString< StaminaString<CHARTYPE> > );
	CPPUNIT_TEST_SUITE_END();


	void timeCopyConvert() {
		tTypedOther a = generateString<OTHER, txtLength>();
		tTimeString s;
		for (int i = 0; i < timingLoops; i++) {
			s = a.str<CHAR>();
		}
	}

	virtual void timeConvert() {
		tTypedOther a = generateString<OTHER, txtLength>();
		tString s;
		for (int i = 0; i < timingLoops; i++) {
			s = a.str<CHAR>();
		}
	}


};

template <class CHARTYPE>
class TimeStringSTL : public TimeString< STLString<CHARTYPE> > {
	CPPUNIT_TEST_SUB_SUITE( TimeStringSTL, TimeString< STLString<CHARTYPE> > );
	CPPUNIT_TEST_SUITE_END();
};


CPPUNIT_TEST_SUITE_REGISTRATION( TimeStringSTL<CharType> );
CPPUNIT_TEST_SUITE_REGISTRATION( TimeStringStamina<CharType> );
//CPPUNIT_TEST_SUITE_REGISTRATION( TimeString<WCharType> );

