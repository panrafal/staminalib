#include <stdafx.h>

#define TEST_STRING

#include <math.h>
#include <stdstring.h>
#include <locale>
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

#ifdef _DEBUG
	const static int timingLoops = 10000;
	const static int txtLength = 320;
#else
	//const static int timingLoops = 100000;
	const static int timingLoops = 10000;
	const static int txtLength = 1320;
#endif

	CPPUNIT_TEST_SUITE( TimeString );
  
/*	CPPUNIT_TEST( timeCopy );
	CPPUNIT_TEST( timeCopyConvert );
	CPPUNIT_TEST( timeConvert );
	CPPUNIT_TEST( timeAdd );
	CPPUNIT_TEST( timeErase );
	CPPUNIT_TEST( timeTruncate );
	CPPUNIT_TEST( timeInsert );
	CPPUNIT_TEST( timeReplace );*/

	CPPUNIT_TEST( timeChangeCase );
	CPPUNIT_TEST( timeEqual );
	CPPUNIT_TEST( timeCompare );
	CPPUNIT_TEST( timeCompare_noCase );
	CPPUNIT_TEST( timeFindChar );
	CPPUNIT_TEST( timeFind );
	CPPUNIT_TEST( timeFind_noCase );

//	CPPUNIT_TEST(  );
	
/*	CPPUNIT_TEST( timePassByReference );
	CPPUNIT_TEST( timePassOtherByReference );
	CPPUNIT_TEST( timePassPointerByReference );
	CPPUNIT_TEST( timePassAndModify );
	CPPUNIT_TEST( timeReturn );
	CPPUNIT_TEST( timeConditionalReturn );
	CPPUNIT_TEST( timeComplex );*/
	
//	CPPUNIT_TEST(  );
//	CPPUNIT_TEST(  );
//	CPPUNIT_TEST(  );

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
		for (int i = 0; i < timingLoops * 5; i++) {
			tTimeString s;
			s = a;
		}
	}

	virtual void timeCopyConvert() {
		tTypedOther a = generateString<OTHER, txtLength>();
		for (int i = 0; i < timingLoops; i++) {
			tTimeString s;
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

	virtual void timeTruncate() {
		tTimeString a = generateString<CHAR, timingLoops>();
		for (int i = 0; i < timingLoops / 4; i++) {
			a.erase(a.length() - 4, 4);
		}
	}

	virtual void timeInsert() {
		tTimeString a;
		tTimeString b = generateString<CHAR, txtLength / 10>();
		tTimeString c;
		for (int i = 0; i < timingLoops; i++) {
			a.insert( a.length() / 2, b );
			if (i % txtLength == 0) {
				a = c;
			}
		}
	}

	virtual void timeFind() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString b = generateString<CHAR, txtLength / 4>();
		static int c;
		for (int i = 0; i < timingLoops; i++) {
			c = a.find( b, 2 );
		}

	}

	virtual void timeFindChar() = 0;

	virtual void timeFind_noCase() = 0;

	virtual void timeEqual() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString b = generateString<CHAR, txtLength>();
		static bool c;
		for (int i = 0; i < timingLoops; i++) {
			c = (a == b);
		}

	}

	virtual void timeCompare() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString b = generateString<CHAR, txtLength>();
		static bool c;
		for (int i = 0; i < timingLoops; i++) {
			c = (a < b);
		}

	}

	virtual void timeCompare_noCase() = 0;

	virtual void timeChangeCase() = 0;

	virtual void timeReplace() = 0;

	virtual void timePassByReference() = 0;

	virtual void timePassOtherByReference() = 0;

	virtual void timePassPointerByReference() = 0;

	virtual void timePassAndModify() = 0;

	virtual void timeReturn() = 0;

	virtual void timeConditionalReturn() = 0;

	virtual void timeComplex() = 0;

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
		tTimeString s;
		for (int i = 0; i < timingLoops; i++) {
			s = a;
		}
	}


	virtual void timeFindChar() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString b = testString(L"456");
		static int c;
		for (int i = 0; i < timingLoops; i++) {
			c = a.findLastChars( b );
		}

	}

	virtual void timeFind_noCase() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString b = generateString<CHAR, txtLength / 4>();
		static int c;
		for (int i = 0; i < timingLoops / 4; i++) {
			c = a.find(b, 2, true);
		}
	}

	virtual void timeCompare_noCase() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString b = generateString<CHAR, txtLength>();
		static int c;
		for (int i = 0; i < timingLoops / 4; i++) {
			c = a.compare(b, true);
		}

	}

	virtual void timeChangeCase() {
		tTimeString a = generateString<CHAR, txtLength>();
		static const CHAR* b;
		for (int i = 0; i < timingLoops / 4; i++) {
			a.toLower();
            b = a.str<CHAR>();
		}
	}


	virtual void timeReplace() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString b = generateString<CHAR, txtLength / 2>();

		for (int i = 0; i < timingLoops; i++) {
			a.replace(txtLength / 4, b, txtLength / 2);
		}

	}

	void timePassByReference_f(const StringRef& a) {
		//static tTimeString b;
		//b = a;
		static const CHAR* b;
		b = a.str<CHAR>();
		//static const CHAR* b;
		//b = a.str<CHAR>();
	}

	virtual void timePassByReference() {
		tTimeString a = generateString<CHAR, txtLength>();
		StringRef b = a;
		for (int i = 0; i < timingLoops * 20; i++) {
			timePassByReference_f(b);
		}
	}

	virtual void timePassOtherByReference() {
		tTimeString a = generateString<CHAR, txtLength>();
		for (int i = 0; i < timingLoops * 20; i++) {
			timePassByReference_f(a);
		}
	}

	virtual void timePassPointerByReference() {
		tString a = generateString<CHAR, txtLength>();
		const CHAR* ch = a.c_str();
		for (int i = 0; i < timingLoops * 4; i++) {
			timePassByReference_f(ch);
		}
	}


	void timePassAndModify_f(StringRef a) {
		static const CHAR* b;
		a += "ha!";
		b = a.str<CHAR>();
	}

	virtual void timePassAndModify() {
		tTimeString a = generateString<CHAR, txtLength>();
		for (int i = 0; i < timingLoops * 4; i++) {
			timePassAndModify_f(a);
		}
	}

	String timeReturn_f() {
		static tString a = generateString<CHAR, txtLength>();
		return PassStringRef( StringRef(a.c_str()) );
	}

	virtual void timeReturn() {
		//tTimeString a;
		const CHAR* a;
		for (int i = 0; i < timingLoops * 4; i++) {
			a = timeReturn_f().str<CHAR>();
		}
	}

	String timeConditionalReturn_f(StringRef a, int i) {
		if (i % 2 == 0) {
			a.assignCheapReference("Yuhuu!");
		}
		return PassStringRef( a );
	}

	virtual void timeConditionalReturn() {
		tTimeString b = generateString<CHAR, txtLength>();
		const CHAR* a;
		for (int i = 0; i < timingLoops * 4; i++) {
			a = timeConditionalReturn_f(b, i).str<CHAR>();
		}
	}

	String timeComplex_f(const StringRef& a, const StringRef& b, int i) {
		String c = timeConditionalReturn_f(a, i);
		c = timeConditionalReturn_f(a.str<CHAR>(), i);
		timePassAndModify_f(c);
		timePassByReference_f(a);
		timePassByReference_f(b.str<CHAR>());
		timePassByReference_f(c);
		c = timeReturn_f();
		if (i % 2 == 0) {
			return PassStringRef( c );
		} else {
			return PassStringRef( StringRef(b.str<CHAR>()) );
		}
	}

	virtual void timeComplex() {
		tTimeString b = generateString<CHAR, txtLength>();
		tTimeString a;
		for (int i = 0; i < timingLoops * 2; i++) {
			a = timeComplex_f(a, b, i);
		}
	}

};

// --------------------------------------------------------

template <class CHARTYPE>
class TimeStringSTL : public TimeString< STLString<CHARTYPE> > {
	CPPUNIT_TEST_SUB_SUITE( TimeStringSTL, TimeString< STLString<CHARTYPE> > );
	CPPUNIT_TEST_SUITE_END();

	virtual void timeFindChar() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString b = testString(L"456");
		static int c;
		for (int i = 0; i < timingLoops; i++) {
			c = a.find_last_of( b );
		}

	}

	virtual void timeFind_noCase() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString b = generateString<CHAR, txtLength / 4>();
		static int c;
		for (int i = 0; i < timingLoops / 4; i++) {
			tTimeString a1 = a;
			tTimeString b1 = b;
			_changeCase(a1);
			_changeCase(b1);
			c = a1.find(b1, 2);
		}
	}

	virtual void timeCompare_noCase() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString b = generateString<CHAR, txtLength>();
		static bool c;
		for (int i = 0; i < timingLoops / 4; i++) {
			tTimeString a1 = a;
			tTimeString b1 = b;
			_changeCase(a1);
			_changeCase(b1);
			c = (a1 < b1);
		}

	}

	inline void _changeCase(tTimeString & a) {
		std::locale loc;
		for (unsigned int j = 0; j < a.length(); j++) {
			a[j] = std::toupper( a[j], loc );
		}
	}

	virtual void timeChangeCase() {
		tTimeString a = generateString<CHAR, txtLength>();
		static const CHAR* c;
		for (int i = 0; i < timingLoops / 4; i++) {
			tTimeString b = a;
			_changeCase(b);
			c = b.c_str();
		}
	}

	virtual void timeReplace() {
		tTimeString a = generateString<CHAR, txtLength>();
		tTimeString b = generateString<CHAR, txtLength / 2>();

		for (int i = 0; i < timingLoops; i++) {
			a.replace(txtLength / 4, txtLength / 2, b);
		}

	}


	void timePassByReference_f(const tString& a) {
		//static tTimeString b;
		//b = a;
		static const CHAR* b;
		b = a.c_str();
	}

	virtual void timePassByReference() {
		tString a = generateString<CHAR, txtLength>();
		for (int i = 0; i < timingLoops * 10; i++) {
			timePassByReference_f(a);
		}
	}

	virtual void timePassOtherByReference() {
		CStdStr<CHAR> a = generateString<CHAR, txtLength>();
		for (int i = 0; i < timingLoops * 10; i++) {
			timePassByReference_f(a);
		}
	}

	virtual void timePassPointerByReference() {
		tString a = generateString<CHAR, txtLength>();
		const CHAR* ch = a.c_str();
		for (int i = 0; i < timingLoops * 2; i++) {
			timePassByReference_f(ch);
		}
	}

	void timePassAndModify_f(tTimeString a) {
		static const CHAR* b;
		a += "ha!";
		b = a.c_str();
	}

	virtual void timePassAndModify() {
		tTimeString a = generateString<CHAR, txtLength>();
		for (int i = 0; i < timingLoops * 4; i++) {
			timePassAndModify_f(a);
		}
	}

	tTimeString timeReturn_f() {
		static tString a = generateString<CHAR, txtLength>();
		return a;
	}

	virtual void timeReturn() {
		const CHAR* a;
		for (int i = 0; i < timingLoops * 4; i++) {
			a = timeReturn_f().c_str();
		}
	}

	tTimeString timeConditionalReturn_f(const tTimeString& a, int i) {
		if (i % 2 == 0) {
			return "Yuhuu!";
		}
		return a;
	}

	virtual void timeConditionalReturn() {
		tTimeString b = generateString<CHAR, txtLength>();
		const CHAR* a;
		for (int i = 0; i < timingLoops * 4; i++) {
			a = timeConditionalReturn_f(b, i).c_str();
		}
	}

	tTimeString timeComplex_f(const tTimeString& a, const tTimeString& b, int i) {
		tTimeString c = timeConditionalReturn_f(a, i);
		c = timeConditionalReturn_f(a.c_str(), i+1);
		timePassAndModify_f(c);
		timePassByReference_f(a);
		timePassByReference_f(b.c_str());
		timePassByReference_f(c);
		c = timeReturn_f();
		if (i % 2 == 0) {
			return c;
		} else {
			return b.c_str();
		}
	}

	virtual void timeComplex() {
		tTimeString b = generateString<CHAR, txtLength>();
		tTimeString a;
		for (int i = 0; i < timingLoops * 4; i++) {
			a = timeComplex_f(a, b, i);
		}
	}


};


CPPUNIT_TEST_SUITE_REGISTRATION( TimeStringStamina<CharType> );
CPPUNIT_TEST_SUITE_REGISTRATION( TimeStringSTL<CharType> );
//CPPUNIT_TEST_SUITE_REGISTRATION( TimeStringStamina<WCharType> );

