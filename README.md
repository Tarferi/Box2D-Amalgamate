# Box2D Amalgamate
\
This project is based on [Box2D](https://github.com/erincatto/box2d).
To build a customized amalgamate of Box2D, simply copy Amalgamate.bat (and fp.h) into Box2D root directory and run it.

To make use of fixed point arithmetic, build Box2D with `fixed_point_support=1` (see Amalgamate.bat) and define `BOX2D_USE_FIXED_POINT` in your header file.


### Fixed point arithmetics support
There is no use of `float` nor `double` anywhere in Box2D.
\
If you wish to use faster and more precise arithmetics, `USE_FPM` and provide [fpm](https://github.com/MikeLankamp/fpm) to be used instead of provided arithmetics.
\
Underlying `FixedPointT` type is implemented as signed Q32_32 with `inf`, `-inf` and `NaN` values support.
As opposed to fpm, which does not handle overflow in any way, `FixedPointT` handles overflows, divisions by zero and other invalid arithmetic operations with a callback methods.
Invalid arithmetic operations result in `inf`, `-inf` or `NaN`.


### Fixed point structure
Type `FixedPointT` is defined as follows:
```cpp
template<const bool RoundDivided, const FPUint32 SqrtIterationsCount, class EventHandler, const bool Atan2UsingAtan>
class FixedPointT;
```
To use this class, it's recommended to define own types or use these predefined with default settings:
```cpp
typedef FixedPointT<FP_DEF> DefaultFixedPoint;

template<const FPUint32 left, const FPUint32 right=0, const bool negative = false>
static constexpr const DefaultFixedPoint(*DefaultFixedPointValue)() = &DefaultFixedPoint::FixedPointValue<left, right, negative>;

template<const FPInt32 nominator, const FPInt32 denominator>
static constexpr const DefaultFixedPoint(*DefaultFixedPointFractionValue)() = &DefaultFixedPoint::FixedPointFractionValue<nominator, denominator>;
```
To construct `FixedPoint` (or the default `DefaultFixedPoint`) you can use `FixedPointValue<X, Y>`, where `X` is the whole-number part and `Y` is decimal part in reversed decimal order.
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

To convert from `FixedPoint` to integer values, use `GetWholeValue` method.

### Error handling
When arithmetic operation results in exception, EventHandler method is called.
```cpp

class FixedPointEventHandler {
public:
	static void Overflow();
	static void MalformedValue();
	static void DivideByZero();
	static const void NegativeSqrt();
	static const void UndefinedAtan2();
};
```
Note that `MalformedValue` occurs when underlying memory is overwritten with invalid values.
Provided are these 3 stock EventHandlers:
- `FixedPointSegfaultEventHandler` - Dereferences null pointer on every method causing application to crash. Useful for debug.
- `FixedPointIgnoreOverflow` - Same as above but does nothing on `Overflow`.
- `FixedPointIgnoreAllErrors` - Ignores all errors.


### Limitations

#### Arithmetics
Lowest representable value is roughly `-4294967295.99999999976716935634613037109375`.\
Highest representable value is roughly `4294967295.99999999976716935634613037109375`.\
Smallest representable value is roughly `0.00000000023283064365386962890625`.

#### Box2D compromises
Using fixed point arithmetics disables b2Dump method.


**This project is by no way production ready.**