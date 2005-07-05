// TestSuite.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


class TimingListener : public CppUnit::TestListener
{
public:
	void startTest( CppUnit::Test *test )
	{
		_ticks = GetTickCount();
	}

	void endTest( CppUnit::Test *test )
	{
		int elapsed = GetTickCount() - _ticks;
		std::cout << " -- elapsed: " << elapsed << " ms" << std::endl;
	}
private:
	int _ticks;
};



int _tmain(int argc, _TCHAR* argv[])
{
  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  TimingListener timing;
  controller.addListener( &timing );

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
  outputter.write(); 

  getch();

  return result.wasSuccessful() ? 0 : 1; 
}

