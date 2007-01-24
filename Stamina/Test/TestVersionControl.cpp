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
#include "..\VersionControl.h"
#include "..\Object.h"
#include "..\Helpers.h"

const Stamina::ModuleVersion testVersion = Stamina::ModuleVersion(Stamina::versionModule, "TestAuto", Stamina::Version(4,3,2,1));

STAMINA_REGISTER_VERSION(test, testVersion);

using namespace Stamina;

using std::string;
using namespace std;

#include <Stamina\Console.h>

class TestVersionControl : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestVersionControl );
  
	CPPUNIT_TEST( testRegister );
	CPPUNIT_TEST( testAutoRegister );
	CPPUNIT_TEST( testClassRegister );

    CPPUNIT_TEST_SUITE_END();

protected:
  

public:

	void testUp() {
//		OutputDebugString(stringf("%x", &__version__iSharedObject).c_str());
	}

	void setUp() {
		//(&__version__iSharedObject)[0];
		return;
		testUp();
	}
	void tearDown() {
	}

protected:


	void testRegister() {
		OutputDebugString("AutoRegistered!\n");
		ModuleVersion mv (versionModule, "Test", Version(1,2,3,4));

		registerVersion(mv);

		CPPUNIT_ASSERT( getVersion(versionModule, "Test") == mv.getVersion() );
	}

	void testAutoRegister() {
		CPPUNIT_ASSERT( getVersion<iObject>() == iObject::staticClassInfo().getVersion() );
		CPPUNIT_ASSERT( getVersion<iSharedObject>() == iObject::staticClassInfo().getVersion() );
		CPPUNIT_ASSERT( getVersion<iLockableObject>() == iObject::staticClassInfo().getVersion() );

		CPPUNIT_ASSERT( getVersion(testVersion.getCategory(), testVersion.getName()) == testVersion.getVersion() );

	}

	void testClassRegister() {

		VersionControl::instance()->clear();

		registerVersion<iSharedObject>();

		CPPUNIT_ASSERT( getVersion<iObject>() == iObject::staticClassInfo().getVersion() );
		CPPUNIT_ASSERT( getVersion<iSharedObject>() == iObject::staticClassInfo().getVersion() );
		CPPUNIT_ASSERT( getVersion<iLockableObject>() == iObject::staticClassInfo().getVersion() );

	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( TestVersionControl );


