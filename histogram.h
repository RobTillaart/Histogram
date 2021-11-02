#pragma once
//
//    FILE: Histogram.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.3.0
// PURPOSE: Histogram library for Arduino
//    DATE: 2012-11-10
//


#include "Arduino.h"

#define HISTOGRAM_LIB_VERSION       (F("0.3.0"))


class Histogram
{
public:
  Histogram(const int16_t length, float *bounds);
  ~Histogram();

  void  clear(int32_t value = 0);
  void  add(const float value);
  void  sub(const float value);

  // number of buckets
  inline int16_t size() { return _length; };

  // number of values added to all buckets
  inline uint32_t count() { return _count; };

  // number of values added to single bucket
  int32_t bucket(const int16_t index);

  float   frequency(const int16_t index);
  float   PMF(const float value);
  float   CDF(const float value);
  float   VAL(const float prob);

  int16_t find(const float value);

protected:
  float *   _bounds;
  int32_t * _data;
  int16_t   _length;
  uint32_t  _count;
};


// -- END OF FILE --

