
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

The Histogram distributes the values added to it into buckets and keeps count.

If you need more quantitative analysis, you might need the statistics library, 
a- https://github.com/RobTillaart/Statistic


#### working

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


## Interface 


### Constructor

- **Histogram(uint8_t length, float \*bounds)** constructor, get an array of boundary values and array length.
- **~Histogram()** destructor.


### Base

- **void clear(float value = 0)** reset all bucket counters to value (default 0).
- **void add(float value)** add a value, increase count of bucket.
- **void sub(float value)** 'add' a value, but decrease count (subtract).
- **uint8_t size()** number of buckets.
- **uint32_t count()** total number of values added (or subtracted).
- **int32_t bucket(uint8_t index)** returns the count of single bucket, can be negative due to **sub()**
- **float frequency(uint8_t index)** returns the relative frequency of a bucket, always between 0.0 and 1.0.


### Helper functions

- **int16_t find(float value)** returns the index of the bucket for value.
- **int16_t findMin()** returns the (first) index of the bucket with the minimum value.
- **int16_t findMax()** returns the (first) index of the bucket with the maximum value.
- **int16_t countLevel(int32_t level)** returns the number of buckets with exact that level (count).
- **int16_t countAbove(int32_t level)** returns the number of buckets above level.
- **int16_t countBelow(int32_t level)** returns the number of buckets below level.


### Probability Distribution Functions

There are three functions:

- **float PMF(float value)** Probability Mass Function. Quite similar to **frequency()**, 
but uses a value as parameter.
- **float CDF(float value)** Cumulative Distribution Function. 
Returns the sum of frequencies <= value. Always between 0.0 and 1.0.
- **float VAL(float prob)** Value Function, is **CDF()** inverted. 
Returns the value of the original array for which the CDF is at least probability.

As the Arduino typical uses a small number of buckets these functions are quite 
coarse and/or inaccurate (linear interpolation within bucket is still to be investigated)

Note **PDF()** is a continuous function and therefore not applicable in discrete histogram.


## Operation

See examples


## Future

- improve strategy for **find()** the right bucket. 
Binary search is faster but need more testing.
- investigate linear interpolation for **PMF()**, **CDF()** and **VAL()** functions to 
improve accuracy.
- improve documentation
  - explain **PMF()**, **CDF()** and **VAL()** functions.
- Copy the boundaries array?
- merge buckets
- examples for new functions.
- bucket full / overflow warning. The **add()** **sub()** should 
return a bool to indicate that a bucket is (almost) full.
- 2D histograms ? e.g. positions on a grid.


#### idea: Histogram8 Histogram 16

Histogram8 and Histogram16 class with similar interface but smaller 
number of buckets size etc. Current version can count up to ±2^31 while
often ±2^15 or even ±2^7 is sufficient. Could safe substantial memory.

| class name  | length | count/bucket | memory |
|:------------|-------:|-------------:|-------:|
| Histogram   | 32766  | ±2147483647  | 130 KB |
| Histogram8  | 254    | ±127         | 260 B  |
| Histogram16 | 254    | ±32767       | 520 B  |

The essential difference would be the **\_data** array, to reduce 
the memory footprint. So could be rather easy. 

Performance optimizations are possible too however not essential for 
the first versions I assume. 


#### expensive ideas

Expensive ideas in terms of memory or performance

- Additional values per bucket.
  - Sum, Min, Max, (average can be derived)
- separate bucket-array for sub()


