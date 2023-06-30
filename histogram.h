#pragma once
//
//    FILE: Histogram.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.4.0
// PURPOSE: Histogram library for Arduino
//    DATE: 2012-11-10


#include "Arduino.h"

#define HISTOGRAM_LIB_VERSION       (F("0.4.0"))


class Histogram
{
public:
  Histogram(const uint16_t length, float * bounds);
  ~Histogram();

  void  clear(int32_t value = 0);
  bool  add(const float value);
  bool  sub(const float value);
  virtual void setBucket(const uint16_t index, int32_t value = 0);

  //  number of buckets
  uint16_t size();

  //  number of values added to all buckets
  uint32_t count();

  //  number of values added to single bucket
  int32_t  bucket(const uint16_t index);

  float    frequency(const uint16_t index);
  float    PMF(const float value);
  float    CDF(const float value);
  float    VAL(const float probability);

  uint16_t find(const float value);
  uint16_t findMin();
  uint16_t findMax();
  uint16_t countLevel(const int32_t level);
  uint16_t countAbove(const int32_t level);
  uint16_t countBelow(const int32_t level);


protected:
  float *   _bounds;
  int32_t * _data;
  uint16_t  _length;
  uint32_t  _count;
  int32_t   _maxBucket = 2147483647;
};


//////////////////////////////////////////////////////////////
//
// DERIVED CLASS
//
class Histogram16 : public Histogram
{
public:
  Histogram16(const uint16_t length, float * bounds);
  ~Histogram16();

  void setBucket(const uint16_t index, int16_t value = 0);

protected:
  int16_t * _data;
  int16_t   _maxBucket = 32767;
};


class Histogram8 : public Histogram
{
public:
  Histogram8(const uint16_t length, float * bounds);
  ~Histogram8();

  void setBucket(const uint16_t index, int8_t value = 0);

protected:
  int8_t * _data;
  int8_t   _maxBucket = 127;
};


//  -- END OF FILE --

