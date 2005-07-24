#include <stdafx.h>
#include <math.h>
#include <cppunit/extensions/HelperMacros.h>
#include <Stamina/VersionControl.h>
#include <Stamina/WideChar.h>
#include <Stamina/StringType.h>
//#include <Stamina/String.h>
#include <ConvertUTF.h>

using namespace Stamina;

template <typename CHAR>
class TestStringType : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestStringType<CHAR> );
  
	CPPUNIT_TEST( testIterator );

	CPPUNIT_TEST_SUITE_END();

protected:

	typedef std::basic_string<CHAR> tString;

public:

	void setUp() {

	}
	void tearDown() {
	}

	tString lowerString() {
		static std::wstring s(L"abc¹æê123");
		return keepChar<std::basic_string<CHAR> >(s);
	}

	tString upperString() {
		static std::wstring s(L"abc¥ÆÊ123");
		return keepChar<std::basic_string<CHAR> >(s);
	}

	tString testString(std::wstring s) {
		return keepChar<std::basic_string<CHAR> >(s);
	}

protected:


	void testIterator() {
		tString test = lowerString();
		StringType<CHAR, cpACP>::ConstIterator it(test.c_str());
		CPPUNIT_ASSERT( it == test.c_str() );
		CPPUNIT_ASSERT( it.getPosition() == 0 );

		for (unsigned i = 0; i < test.size(); ++i) {
			++it;
		}

		CPPUNIT_ASSERT( it.getPosition() == test.size() );
		CPPUNIT_ASSERT( it.getFoundPosition(test.c_str() + test.size()) == StringType::notFound );

	}


};


CPPUNIT_TEST_SUITE_REGISTRATION( TestStringType<char> );
CPPUNIT_TEST_SUITE_REGISTRATION( TestStringType<wchar_t> );
