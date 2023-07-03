#pragma once
//
//    FILE: Histogram.h
//  AUTHOR: Rob Tillaart
// VERSION: 1.0.0
// PURPOSE: Histogram library for Arduino
//    DATE: 2012-11-10


#include "Arduino.h"

#define HISTOGRAM_LIB_VERSION       (F("1.0.0"))


template <typename T>
class Histogram
{
public:
  Histogram(const uint16_t length, float * bounds)
  {
    _bounds = bounds;
    _length = length + 1;
    _data = (T *) malloc((_length) * sizeof(T));
    if (_data != NULL)
    {
      clear();
    }
    else
    {
      _length = 0;
    }
    _count = 0;
  }

  ~Histogram()
  {
    if (_data != NULL)
    {
      free(_data);
    }
  }

  void clear(T value = 0)
  {
    for (uint16_t i = 0; i < _length; i++)
    {
      _data[i] = value;
    }
    _count = 0;
  }

  bool add(const float value)
  {
    if (_length == 0) return false;
    uint16_t index = find(value);
    //  detect overflow
    T oldValue = _data[index];
    //  volatile prevents faulty optimization of the comparison
    volatile T newValue = oldValue + 1;
    if (newValue < oldValue) return false;
    _data[index]++;
    _count++;
    return true;
  }

  bool sub(const float value)
  {
    if (_length == 0) return false;
    uint16_t index = find(value);
    //  detect underflow
    T oldValue = _data[index];
    volatile T newValue = oldValue - 1;
    if (newValue > oldValue) return false;
    _data[index]--;
    _count++;
    return true;
  }

  void setBucket(const uint16_t index, T value = 0)
  {
    _data[index] = value;
  }

  //  number of buckets
  uint16_t size()
  {
    return _length;
  }

  //  number of values added to all buckets
  uint32_t count()
  {
    return _count;
  }

  //  number of values added to single bucket
  T bucket(const uint16_t index)
  {
    if (index > _length) return 0;
    return _data[index];
  }

  float frequency(const uint16_t index)
  {
    if ((_count == 0) || (_length == 0)) return NAN;
    if (index > _length) return 0;   //  differs from PMF()
    return (1.0 * _data[index]) / _count;
  }

  float PMF(const float value)
  {
    if ((_count == 0) || (_length == 0)) return NAN;
    uint16_t index = find(value);
    return (1.0 * _data[index]) / _count;
  }

  float CDF(const float value)
  {
    if ((_count == 0) || (_length == 0)) return NAN;
    //  TODO: could be done in one loop?
    uint16_t index = find(value);
    int32_t  sum = 0;
    for (uint16_t i = 0; i <= index; i++)
    {
      sum += _data[i];
    }
    return (1.0 * sum) / _count;
  }

  float VAL(const float probability)
  {
    if ((_count == 0) || (_length == 0)) return NAN;

    float prob = probability;
    if      (prob < 0.0) prob = 0.0;
    else if (prob > 1.0) prob = 1.0;

    prob *= _count;
    int32_t sum = 0;
    for (uint16_t i = 0; i < _length; i++)
    {
      sum += _data[i];
      if ((sum >= prob) && (i < (_length - 1)))
      {
        return _bounds[i];
      }
    }
    return INFINITY;
  }

  uint16_t find(const float value)
  {
    if (_length <= 0) return -1;

    for (uint16_t i = 0; i < (_length - 1); i++)
    {
      if (_bounds[i] >= value)
      {
        return i;
      }
    }
    return _length - 1;
  }

  uint16_t findMin()
  {
    if (_length <= 0) return -1;
    uint16_t index = 0;
    uint16_t n = _length;
    while (n > 1)
    {
      n--;
      if (_data[n] < _data[index]) index = n;
    }
    return index;
  }

  uint16_t findMax()
  {
    if (_length <= 0) return -1;
    uint16_t index = 0;
    uint16_t n = _length;
    while (n > 1)
    {
      n--;
      if (_data[n] > _data[index]) index = n;
    }
    return index;
  }

  uint16_t countLevel(const T level)
  {
    if (_length <= 0) return -1;

    uint16_t buckets = 0;
    uint16_t n = _length;
    while (n > 0)
    {
      n--;
      if (_data[n] == level) buckets++;
    }
    return buckets;
  }

  uint16_t countAbove(const T level)
  {
    if (_length <= 0) return -1;

    uint16_t buckets = 0;
    uint16_t n = _length;
    while (n > 0)
    {
      n--;
      if (_data[n] > level) buckets++;
    }
    return buckets;
  }

  uint16_t countBelow(const T level)
  {
    if (_length <= 0) return -1;

    uint16_t buckets = 0;
    uint16_t n = _length;
    while (n > 0)
    {
      n--;
      if (_data[n] < level) buckets++;
    }
    return buckets;
  }


protected:
  T *       _data;
  float *   _bounds;
  uint16_t  _length;
  uint32_t  _count;
};


//  -- END OF FILE --

