//
//    FILE: unit_test_001.cpp
//  AUTHOR: Rob Tillaart
//    DATE: 2020-12-03
// PURPOSE: unit tests for the Histogram
//          https://github.com/RobTillaart/Histogram
//          https://github.com/Arduino-CI/arduino_ci/blob/master/REFERENCE.md
//

// supported assertions
// https://github.com/Arduino-CI/arduino_ci/blob/master/cpp/unittest/Assertion.h#L33-L42
// ----------------------------
// assertEqual(expected, actual)
// assertNotEqual(expected, actual)
// assertLess(expected, actual)
// assertMore(expected, actual)
// assertLessOrEqual(expected, actual)
// assertMoreOrEqual(expected, actual)
// assertTrue(actual)
// assertFalse(actual)
// assertNull(actual)
// assertNotNull(actual)

#include <ArduinoUnitTests.h>

#define assertEqualFloat(arg1, arg2, arg3)  assertOp("assertEqualFloat", "expected", fabs(arg1 - arg2), compareLessOrEqual, "<=", "actual", arg3)
#define assertEqualINF(arg)  assertOp("assertEqualINF", "expected", INFINITY, compareEqual, "==", "actual", arg)
#define assertEqualNAN(arg)  assertOp("assertEqualNAN", "expected", true, compareEqual, "==", "actual", isnan(arg))


#include "Arduino.h"
#include "histogram.h"



unittest_setup()
{
}

unittest_teardown()
{
}

/*
unittest(test_new_operator)
{
  assertEqualINF(exp(800));
  assertEqualINF(0.0/0.0);
  assertEqualINF(42);
  
  assertEqualNAN(INFINITY - INFINITY);
  assertEqualNAN(0.0/0.0);
  assertEqualNAN(42);
}
*/

unittest(test_constructor)
{
  float diceValues[] = { 1.5, 2.5, 3.5, 4.5, 5.5 };
  Histogram hist(5, diceValues);
  assertEqual(6, hist.size());
  assertEqual(0, hist.count());
  
  for (int i = 0; i < 6; i++)
  {
    assertEqual(0, hist.bucket(i));
  }
}

unittest(test_dice)
{
  float diceValues[] = { 1.5, 2.5, 3.5, 4.5, 5.5 };
  Histogram hist(5, diceValues);
  
  for (int d = 0; d < 70; d++)
  {
    hist.add( d % 7 ); // simulate dice 
  }
  
  assertEqual(6, hist.size());
  assertEqual(100, hist.count());
  
  for (int i = 0; i < 6; i++)
  {
    assertEqual(10, hist.bucket(i));
  }
  
  hist.clear();
  assertEqual(6, hist.size());
  assertEqual(0, hist.count());
  
  for (int i = 0; i < 6; i++)
  {
    assertEqual(0, hist.bucket(i));
  }
}

unittest_main()

// --------
