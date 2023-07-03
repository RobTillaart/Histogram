
[![Arduino CI](https://github.com/RobTillaart/Histogram/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![Arduino-lint](https://github.com/RobTillaart/Histogram/actions/workflows/arduino-lint.yml/badge.svg)](https://github.com/RobTillaart/Histogram/actions/workflows/arduino-lint.yml)
[![JSON check](https://github.com/RobTillaart/Histogram/actions/workflows/jsoncheck.yml/badge.svg)](https://github.com/RobTillaart/Histogram/actions/workflows/jsoncheck.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/Histogram/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/Histogram.svg?maxAge=3600)](https://github.com/RobTillaart/Histogram/releases)


# Histogram

Arduino library for creating histograms math.


## Description

One of the main applications for the Arduino board is reading and logging of sensor data.
We often want to make a histogram of this data to get insight of the distribution of the
measurements. This is where this Histogram library comes in.

The Histogram distributes the values added to it into buckets and keeps count per bucket.

If you need more quantitative analysis, you might need the statistics library, 
- https://github.com/RobTillaart/Statistic


#### Related

- https://github.com/RobTillaart/Correlation
- https://github.com/RobTillaart/GST - Golden standard test metrics
- https://github.com/RobTillaart/Histogram
- https://github.com/RobTillaart/RunningAngle
- https://github.com/RobTillaart/RunningAverage
- https://github.com/RobTillaart/RunningMedian
- https://github.com/RobTillaart/statHelpers - combinations & permutations
- https://github.com/RobTillaart/Statistic


#### Working

When the class is initialized an array of the boundaries to define the borders of the
buckets is passed to the constructor. This array should be declared global as the
Histogram class does not copy the values to keep memory usage low. This allows to change
the boundaries runtime, so after a **clear()**, a new Histogram can be created.

The values in the boundary array do not need to be equidistant (equal in size)
but they need to be in ascending order.

Internally the library does not record the individual values, only the count per bucket.
If a new value is added - **add()** or **sub()** - the class checks in which bucket it 
belongs and the buckets counter is increased.

The **sub()** function is used to decrease the count of a bucket and it can cause the 
count to become below zero. Although seldom used but still depending on the application 
it can be useful. E.g. when you want to compare two value generating streams, you let 
one stream **add()** and the other **sub()**. If the histogram of both streams is 
similar they should cancel each other out (more or less), and the value of all buckets 
should be around 0. \[not tried\].

The **frequency()** function may be removed to reduce footprint as it can be calculated 
with the formula **(1.0 \* bucket(i))/count()**.


#### Experimental: Histogram8 Histogram16

Histogram8 and Histogram16 are derived classes with same interface but smaller buckets. 
Histogram can count to ± 2^31 while often ± 2^15 or even ± 2^7 is sufficient. 
Saves substantial memory.

|  class name   |  length  |  count/bucket  |  max memory  |
|:--------------|---------:|---------------:|-------------:|
|  Histogram    |   65534  |  ± 2147483647  |      260 KB  |
|  Histogram8   |   65534  |  ± 127         |       65 KB  |
|  Histogram16  |   65534  |  ± 32767       |      130 KB  |


The difference is the **\_data** array, to reduce the memory footprint.

Note: max memory is without the boundary array.

Performance optimizations are possible too however not essential for 
the experimental version.


## Interface 

```cpp
#include "histogram.h"
```

### Constructor

- **Histogram(uint16_t length, float \*bounds)** constructor, get an array of boundary values and array length. 
Length should be less than 65534.
- **Histogram8(uint16_t length, float \*bounds)** idem as above.
- **Histogram16(uint16_t length, float \*bounds)** idem as above.
- **~Histogram()** destructor.
- **~Histogram8()** destructor.
- **~Histogram16()** destructor.


### Base

- **void clear(float value = 0)** reset all bucket counters to value (default 0).
- **void setBucket(const uint16_t index, int32_t value = 0)** store / overwrite a value of bucket.
- **bool add(float value)** add a value, increase count of bucket.
returns false if either length is zero, or bucket is full (+MAXINT).
- **bool sub(float value)** 'add' a value, but decrease count (subtract).
Returns false if either length is zero, or bucket is -full (-MAXINT).
- **uint16_t size()** returns number of buckets.
- **uint32_t count()** returns total number of values added (or subtracted).
- **int32_t bucket(uint16_t index)** returns the count of single bucket, can be negative due to **sub()**
- **float frequency(uint16_t index)** returns the relative frequency of a bucket, always between 0.0 and 1.0.


### Helper functions

- **uint16_t find(float value)** returns the index of the bucket for value.
- **uint16_t findMin()** returns the (first) index of the bucket with the minimum value.
- **uint16_t findMax()** returns the (first) index of the bucket with the maximum value.
- **uint16_t countLevel(int32_t level)** returns the number of buckets with exact that level (count).
- **uint16_t countAbove(int32_t level)** returns the number of buckets above level.
- **uint16_t countBelow(int32_t level)** returns the number of buckets below level.


### Probability Distribution Functions

There are three functions:

- **float PMF(float value)** Probability Mass Function. 
Quite similar to **frequency()**, but uses a value as parameter.
- **float CDF(float value)** Cumulative Distribution Function. 
Returns the sum of frequencies <= value. Always between 0.0 and 1.0.
- **float VAL(float probability)** Value Function, is **CDF()** inverted. 
Returns the value of the original array for which the CDF is at least probability.

As the Arduino typical uses a small number of buckets these functions are quite 
coarse and/or inaccurate (linear interpolation within bucket is still to be investigated)

Note **PDF()** is a continuous function and therefore not applicable in discrete histogram.


- https://en.wikipedia.org/wiki/Probability_mass_function  PMF()
- https://en.wikipedia.org/wiki/Cumulative_distribution_function CDF() + VAL()


## Operation

See examples


## Future


#### Must

- improve documentation

#### Should

- investigate performance - **find()** the right bucket. 
  - Binary search is faster 
  - need testing.
- improve accuracy - linear interpolation for **PMF()**, **CDF()** and **VAL()**
- performance - merge loops in **PMF()**
- performance - reverse loops - compare to zero.


#### Could

- **saturation()** indication of the whole histogram
  - count / nr of bins?
- percentage readOut == frequency()
  - int32_t total() = 100%
  - **float getBucketPercent(idx)**
- template class <bucketsizeType>.
- **getMaxBucket()**? or just type (8, 16, 32)


#### Wont

- merge bins
- 2D histograms ? e.g. positions on a grid.
  - see SparseMatrix

