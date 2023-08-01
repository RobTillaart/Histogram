#pragma once
//
//    FILE: Histogram.h
//  AUTHOR: Rob Tillaart
// VERSION: 1.0.0
// PURPOSE: Histogram library for Arduino
//    DATE: 2012-11-10


#include "Arduino.h"

#define HISTOGRAM_LIB_VERSION       (F("1.0.0"))

//  return STATUS add(), sub(), clear(), setBucket();
#define HISTO_OK                     0x00    //  idem
#define HISTO_FULL                   0x01    //  just got full
#define HISTO_ERR_FULL               0xFF    //  over- underflow
#define HISTO_ERR_LENGTH             0xFE    //  constructor issue.


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
    _status = HISTO_OK;
    _maxBucket = 2147483647;
  }

  ~Histogram()
  {
    if (_data != NULL)
    {
      free(_data);
    }
  }


//  resets all counters to value (default 0)
  uint8_t clear(T value = 0)
  {
    for (uint16_t i = 0; i < _length; i++)
    {
      _data[i] = value;
    }
    _count = 0;
    _status = HISTO_OK;
    if (value == _maxBucket) _status = HISTO_FULL;
    return _status;
  }


  uint8_t add(const float value)
  {
    if (_length == 0)
    {
      _status = HISTO_ERR_LENGTH;
      return _status;
    }
    uint16_t index = find(value);
    if (_data[index] == _maxBucket)
    {
      _status = HISTO_ERR_FULL;
      return _status;
    }
    _data[index]++;
    _count++;
    _status = HISTO_OK;
    if (_data[index] == _maxBucket) _status = HISTO_FULL;
    return _status;
  }


  uint8_t sub(const float value)
  {
    if (_length == 0)
    {
      _status = HISTO_ERR_LENGTH;
      return _status;
    }
    uint16_t index = find(value);
    if (_data[index] == -_maxBucket)
    {
      _status = HISTO_ERR_FULL;
      return _status;
    }
    _data[index]--;
    _count++;
    _status = HISTO_OK;
    if (_data[index] == _maxBucket) _status = HISTO_FULL;
    return _status;
  }

  uint8_t setBucket(const uint16_t index, T value = 0)
  {
    _data[index] = value;
    _status = HISTO_OK;
    if (value == _maxBucket) _status = HISTO_FULL;
    return _status;
  }


  uint8_t  status()
  {
    return _status;
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


  //  EXPERIMENTAL
  //  returns the probability of the bucket of a value
  float PMF(const float value)
  {
    if ((_count == 0) || (_length == 0)) return NAN;
    uint16_t index = find(value);
    return (1.0 * _data[index]) / _count;
  }


  //  EXPERIMENTAL
  //  returns the cumulative probability of
  //  values <= value
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

  //  EXPERIMENTAL
  //  returns the value of the original array for
  //  which the CDF is at least probability.
  //  must start at 0.
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


  int32_t sum()
  {
    int32_t _sum = 0;
    for (uint16_t i = 0; i < _length; i++)
    {
      _sum += _data[i];
    }
    return _sum;
  }


// returns the bucket number for value
// - binary search, more memory ;  faster for #buckets > 20 ?
// uint16_t Histogram::find(const float value)
// {
  // if (_length <= 0) return -1;

  // uint16_t low = 0, high = _length;
  // uint16_t mid;
  // while (high - low > 1)
  // {
    // mid = (low + high)/2;
    // if (_bounds[mid] > value)
    // {
      // high = mid;
    // }
    // else
    // {
      // low = mid;
    // }
  // }
  // if (_bounds[mid] > value) return mid;
  // return _length - 1;
// }


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


  //  experimental use with care
  int32_t getMaxBucket()
  {
    return _maxBucket;
  }


  void setMaxBucket(int32_t value)
  {
    _maxBucket = value;
  }



protected:
  float *   _bounds;
  T *       _data;
  uint16_t  _length;
  uint32_t  _count;
  int32_t   _maxBucket;
  uint8_t   _status;
};


//  -- END OF FILE --

