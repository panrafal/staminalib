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
#include <cppunit/extensions/HelperMacros.h>
#include <Stamina/VersionControl.h>
#include "..\Crypt.h"
#include <Stamina\MD5.h>
#include <Stamina\Helpers.h>

using namespace Stamina;
using namespace Stamina::DT;

const int timingLoops = 2000;

class TestCrypt : public CPPUNIT_NS::TestFixture
{
  
	CPPUNIT_TEST_SUITE( TestCrypt );
  
	CPPUNIT_TEST( testXor1Long );
	CPPUNIT_TEST( testXor1Short );
	CPPUNIT_TEST( testXor2Long );
	CPPUNIT_TEST( testXor2Short );
	CPPUNIT_TEST( testXor1Timing );
	CPPUNIT_TEST( testXor2Timing );

    CPPUNIT_TEST_SUITE_END();

protected:
  
	unsigned char* _xor1_key;
	Stamina::MD5Digest _xor2_key;

	std::string _testStringLong;
	std::string _testStringShort;

public:
	void setUp() {
		_xor1_key = (unsigned char*)"abcdefghijklmno";
		_xor2_key.calculate((char*)_xor1_key);
		_testStringLong = "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean eget arcu sed tellus dapibus pellentesque. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Curabitur ac ante sed ante laoreet congue. In volutpat, urna non sagittis ullamcorper, eros justo dignissim lectus, vitae fringilla lectus ligula quis turpis. Ut non turpis nec wisi rutrum laoreet. Mauris tincidunt odio sit amet mi. Sed adipiscing. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos hymenaeos. Aenean turpis. Sed rhoncus. Morbi facilisis, wisi vel nonummy luctus, ante tellus porta neque, sed fermentum velit arcu tempor purus. Sed rhoncus tortor eu lorem. Pellentesque ornare condimentum wisi. In hac habitasse platea dictumst. Proin porttitor, odio ut tristique feugiat, sapien ipsum rutrum sem, eget consequat nisl tellus in leo. Maecenas ut lorem. Quisque tristique lorem a eros. Etiam justo.";
		_testStringShort = "\1\2\3";
	}
	void tearDown() {
	}

protected:

	void testXor1Long() {
		testXor1(_testStringLong);
	}
	void testXor1Short() {
		testXor1(_testStringShort);
	}
	void testXor1Timing() {
		for (int i = 0; i < timingLoops; i++)
			testXor1(_testStringLong);
	}
	void testXor2Long() {
		testXor2(_testStringLong);
	}
	void testXor2Short() {
		testXor2(_testStringShort);
	}
	void testXor2Timing() {
		for (int i = 0; i < timingLoops; i++)
			testXor2(_testStringLong);
	}

	// koduje -> dekoduje
	void testXor1(const std::string& testString) {
		std::string encoded = testString; 
		// xor1 nie zmienia d³ugoœci, wiêc w ten sposób bêdzie idealnie...
		Stamina::DT::xor1_encrypt(_xor1_key, (unsigned char*)encoded.c_str(), encoded.length());
		Stamina::DT::xor1_decrypt(_xor1_key, (unsigned char*)encoded.c_str(), encoded.length());
		CPPUNIT_ASSERT_EQUAL(testString, encoded);
	}

	// koduje -> dekoduje
	void testXor2(const std::string& testString) {
		std::string encoded = testString; 
		unsigned int salt = random(0, 0xFFFFFFFF);
		// xor1 nie zmienia d³ugoœci, wiêc w ten sposób bêdzie idealnie...
		Stamina::DT::xor2_encrypt(_xor2_key.getDigest(), (unsigned char*)encoded.c_str(), encoded.length(), salt);
		Stamina::DT::xor2_decrypt(_xor2_key.getDigest(), (unsigned char*)encoded.c_str(), encoded.length(), salt);
		CPPUNIT_ASSERT_EQUAL(testString, encoded);
	}


};

CPPUNIT_TEST_SUITE_REGISTRATION( TestCrypt );


