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
#include <Stamina/VersionControl.h>
#include "..\UniqueImpl.h"

using namespace Stamina;

using std::string;
using namespace std;

using namespace Unique;

#include <Stamina\Console.h>

class TestUnique : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestUnique );
  
	CPPUNIT_TEST( testRegisterDomain );
	CPPUNIT_TEST( testDomain );

    CPPUNIT_TEST_SUITE_END();

protected:
  

public:
	void setUp() {
	}
	void tearDown() {
	}

protected:


	void testRegisterDomain() {

		DomainList dl;

		dl.registerDomain( new Domain(domainDomain, "domains") );
		dl.registerDomain( new Domain(domainRange, "ranges") );
		dl.registerDomain( new Domain(domainRange, "ranges2") );
		dl.registerDomain( new Domain(domainNotFound, "notFound") );

		CPPUNIT_ASSERT( dl.getDomain(domainDomain) == true );
		CPPUNIT_ASSERT( dl.getDomain(domainRange) == true );
		CPPUNIT_ASSERT( dl.getDomain("ranges") == true );
		CPPUNIT_ASSERT( dl.getDomain("ranges2") == false );
		CPPUNIT_ASSERT( dl.getDomain(domainNotFound) == false );
		CPPUNIT_ASSERT( dl.getDomain("notFound") == false );


		dl.unregisterDomain( dl.getDomain(domainDomain) );
		dl.unregisterDomain( dl.getDomain(domainRange) );
//		dl.unregisterDomain( dl.getDomain("notFound") );

	}

	void testDomain() {

		Domain* domain = new Domain(domainNotFound, "TestDomain");
		oDomain domainObj = domain;

		Range* range = new Range((tRangeId)0x80, Range::typeBoth, 0x8, 0x81, 0x90);
		oRange rangeObj = range;

		domain->addRange(new Range(rangeMain, Range::typeBoth, 0, 1, 0xFFFF, 0x80));
		domain->addRange(range, true);
		domain->addRange(new Range(rangeStatic, Range::typeStatic, 0xF, 0x60, 0x90));
		domain->addRange(new Range(rangeUnique, Range::typeUnique, 4, 0x80, 0x100));
		CPPUNIT_ASSERT_EQUAL((size_t)4, domain->getRanges().size());
		CPPUNIT_ASSERT( domain->addRange(new Range(rangeNotFound, Range::typeUnique, 0)) == false );

		CPPUNIT_ASSERT( domain->registerId(0x60, "0x60") == true );
		CPPUNIT_ASSERT( domain->registerId(0x82, "0x82") == true );
		CPPUNIT_ASSERT_EQUAL( (tId)0x80, domain->registerName("rangeMain", rangeMain) );
		CPPUNIT_ASSERT_EQUAL( (tId)0x81, domain->registerName("range", range) );

		CPPUNIT_ASSERT_EQUAL( idNotFound, domain->registerName("rangeStatic", rangeStatic) );
		CPPUNIT_ASSERT_EQUAL( (tId)0x83, domain->registerName("rangeUnique", rangeUnique) );
		CPPUNIT_ASSERT_EQUAL( (tId)0x80, domain->registerName("rangeMain", rangeMain) );


		CPPUNIT_ASSERT_EQUAL( (tId)0x80, domain->getId("rangeMain") );
		CPPUNIT_ASSERT( domain->unregister("rangeMain") == true );
		CPPUNIT_ASSERT_EQUAL( (tId)0x84, domain->registerName("rangeMain", rangeMain) );

		CPPUNIT_ASSERT( domain->idInRange(range->getRangeId(), domain->getId("range")) );

		CPPUNIT_ASSERT_EQUAL( rangeStatic, domain->inRange(domain->getId("range"), iRange::typeBoth)->getRangeId() );

		CPPUNIT_ASSERT( domain->inRange(domain->getId("range"), iRange::typeUnique).get() == range );

	}


};

CPPUNIT_TEST_SUITE_REGISTRATION( TestUnique );


