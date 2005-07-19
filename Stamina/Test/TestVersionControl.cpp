#include <stdafx.h>

#include <string>
#include <cppunit/extensions/HelperMacros.h>
//#include "..\VersionControl.h"
#include "..\Object.h"

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
	void setUp() {
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


