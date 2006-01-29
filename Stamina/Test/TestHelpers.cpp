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

$Id: $

*/

#include <stdafx.h>

#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "..\Helpers.h"

using namespace Stamina;

using std::string;
using namespace std;

#include <Stamina\Console.h>

class TestHelpers : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestHelpers );
  
	CPPUNIT_TEST( testFind_noCase );

    CPPUNIT_TEST_SUITE_END();

protected:
  

public:
	void setUp() {
	}
	void tearDown() {
	}

protected:


	void testFind_noCase() {
		CPPUNIT_ASSERT_EQUAL((size_t)3, find_noCase("123¹æœ123", "¥æŒ"));
		CPPUNIT_ASSERT_EQUAL((size_t)-1, find_noCase("123¹æœ123", "c"));
		CPPUNIT_ASSERT_EQUAL((size_t)5, find_noCase("123¹æ¥Æœ123", "¥æŒ"));
		CPPUNIT_ASSERT_EQUAL((size_t)3, find_noCase(L"123¹æœ123", L"¥æŒ"));
		CPPUNIT_ASSERT_EQUAL((size_t)-1, find_noCase(L"123¹æœ123", L"c"));
		CPPUNIT_ASSERT_EQUAL((size_t)5, find_noCase(L"123¹æ¥Æœ123", L"¥æŒ"));

		CPPUNIT_ASSERT_EQUAL((size_t)-1, find_noCase("123¹æœ123", ""));
		CPPUNIT_ASSERT_EQUAL((size_t)-1, find_noCase("123¹æœ123", " 123¹æœ123 "));
		CPPUNIT_ASSERT_EQUAL((size_t)-1, find_noCase("", "a"));
	}


};

CPPUNIT_TEST_SUITE_REGISTRATION( TestHelpers );


