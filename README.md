# Box2D Amalgamate
\
This project is based on [Box2D](https://github.com/erincatto/box2d) and [fpm](https://github.com/MikeLankamp/fpm).
To build a customized amalgamate of Box2D, simply copy Amalgamate.bat (and fp.h) into Box2D root directory and run it.

To make use of fixed point arithmetic, build Box2D with `fixed_point_support=1` (see Amalgamate.bat) and define `BOX2D_USE_FIXED_POINT` in your header file.


### Fixed point arithmetics support
There is no use of `float` nor `double` anywhere in Box2D and FPM. 
\
Underlying `FixedPoint` type is implemented as signed Q32_32 with special `inf` value support. As oposed to fpm, which does not handle overflow in any way, `FixedPoint`
has custom `Overflow` method (and others, such as `DivideByZero`, `NegativeSqrt`, `UndefinedAtan2`). Default overflow behavior is to return `inf`.
\
To construct `FixedPoint` you can use `FixedPointValue<X, Y>`, where `X` is the whole-number part and `Y` is decimal part in reversed decimal order.
\
Another way to construct `FixedPoint` type is to use `FixedPointFractionValue<Nominator, Denominator>`.
\
\
To construct `3.25` using `FixedPointValue`: 
```C++
FixedPoint value = FixedPointValue<3, 52>();
```
using `FixedPointFractionValue`: 
```C++
FixedPoint value = FixedPointFractionValue<325, 100>();
```
Creating runtime values: 
```C++
int x = 325;
FixedPoint value(x);
value /= FixedPointValue<100, 0>;
```

To convert from `FixedPoint` use `GetWholeValue` method.

### Limitations

#### Arithmetics
Lowest representable value is roughly `-4294967295.99999999976716935634613037109375`.\
Highest representable value is roughly `4294967295.99999999976716935634613037109375`.\
Smallest representable value is roughly `0.00000000023283064365386962890625`.\
Overflowing lowest/highest values calls for `Overflow` method and generates `Inf` value.
Default behavior for overflow situation is to cause null pointer dereference.

#### Box2D compromises
Using fixed point arithmetics disables b2Dump method.


**This project is by no way production ready.**