#pragma once
#define BOX2D_USE_FIXED_POINT
#define BOX2D_EXTERNAL
//#define USE_FPM


#ifdef BOX2D_USE_FIXED_POINT
#define BOX2D_NO_TIMER
#endif


#define BOX2D_INCLUDES_ONLY
#include "lib_box2d.h"
#undef BOX2D_INCLUDES_ONLY

#ifdef BOX2D_EXTERNAL

// INCLUDE_POST_HEADERS_BEGIN

#ifdef BOX2D_USE_FIXED_POINT
#include <limits>
#include <string>
typedef uint8_t FPUint8;
typedef uint32_t FPUint32;
typedef int32_t FPInt32;
typedef uint64_t FPUint64;

#pragma push_macro("F_VALUE")
#ifdef F_VALUE
#undef F_VALUE
#endif
#define F_VALUE(...) ((FixedPoint)(FixedPointValue<__VA_ARGS__>()))

#pragma push_macro("F_CONST")
#ifdef F_CONST
#undef F_CONST
#endif
#define F_CONST(left, right, original) F_VALUE(left, right)

#pragma push_macro("F_INVERSE")
#ifdef F_INVERSE
#undef F_INVERSE
#endif
#define F_INVERSE(num, len) FixedPoint::Inverse((FPUint32)num, FixedPoint::InverseLength(len))

#pragma push_macro("FLT_MAX")
#ifdef FLT_MAX
#undef FLT_MAX
#endif
#define FLT_MAX F_VALUE(0, 0, false, 1, 0xFFFFFFFFFFFFFFFF)

#pragma push_macro("FLT_MIM")
#ifdef FLT_MIN
#undef FLT_MIN
#endif
#define FLT_MIN F_VALUE(0, 0, true, 1, 0xFFFFFFFFFFFFFFFF)

#pragma push_macro("FLT_EPSILON")
#ifdef FLT_EPSILON
#undef FLT_EPSILON
#endif
#define FLT_EPSILON F_VALUE(0, 0, false, 1, 0x0000000000000001)

class FPUint128 {

	constexpr FPUint128(FPUint64 high, FPUint64 low) : low(low), high(high) {

	}

public:
	
	constexpr FPUint128() {

	}

	template<typename T>
	constexpr FPUint128(T value) : low(static_cast<FPUint64>(value)) {

	}

	constexpr bool operator!=(FPUint128 another) const {
		return !(*this == another);
	}

	constexpr bool operator>=(FPUint128 another) const {
		return another <= *this;
	}

	constexpr bool operator>(FPUint128 another) const {
		return another < *this;
	}

	constexpr FPUint128 operator++(int) {
		FPUint128 result{ *this };
		*this += 1;
		return result;
	}

	constexpr FPUint128& operator>>=(const FPUint128& another) {
		*this = *this >> another;
		return *this;
	}

	constexpr FPUint128 operator>>(FPUint128 another) const {
		if (another >= 128) {
			return {};
		} else if (another == 0) {
			return *this;
		} else {
			std::uint64_t n{ another.low };
			if (n >= 64) {
				return { 0, high >> (n - 64) };
			} else {
				return { high >> n, (high << (64 - n)) | (low >> n) };
			}
		}
	}

	constexpr FPUint128& operator+=(const FPUint128& another) {
		*this = *this + another;
		return *this;
	}

	constexpr FPUint128& operator-=(const FPUint128& another) {
		*this = *this - another;
		return *this;
	}

	constexpr FPUint128 operator<<(FPUint128 another) const {
		if (another >= 128) {
			return {};
		} else if (another == 0) {
			return *this;
		} else {
			std::uint64_t n{ another.low };
			if (n >= 64) {
				return { low << (n - 64), 0 };
			} else {
				return { (high << n) | (low >> (64 - n)), low << n };
			}
		}
	}

	constexpr FPUint128 operator+(FPUint128 another) const {
		FPUint64 lower{ (low & ~topBit) + (another.low & ~topBit) };
		bool carry = ((lower >> 63) + (low >> 63) + (another.low >> 63)) > 1;
		return FPUint128(high + another.high + carry, low + another.low);
	}
	
	constexpr FPUint128 operator*(FPUint128 another) const {
		FPUint64 mask32 = 0xffffffff;
		if (high == 0 && another.high == 0) {
			FPUint64 x0 = low & mask32;
			FPUint64 x1 = low >> 32;
			FPUint64 y0 = another.low & mask32;
			FPUint64  y1 = another.low >> 32;

			FPUint128 x0y0(x0 * y0);
			FPUint128 x0y1(x0 * y1);
			FPUint128 x1y0(x1 * y0);
			FPUint128 x1y1(x1 * y1);
			return x0y0 + ((x0y1 + x1y0) << 32) + (x1y1 << 64);
		} else {
			FPUint64 x0 = low & mask32;
			FPUint64 x1 = low >> 32;
			FPUint64 x2 = high & mask32;
			FPUint64 x3 = high >> 32;

			FPUint64 y0 = another.low & mask32;
			FPUint64 y1 = another.low >> 32;
			FPUint64 y2 = another.high & mask32;
			FPUint64 y3 = another.high >> 32;

			FPUint128 x0y0(x0 * y0);
			FPUint128 x0y1(x0 * y1);
			FPUint128 x0y2(x0 * y2);
			FPUint128 x0y3(x0 * y3);

			FPUint128 x1y0(x1 * y0);
			FPUint128 x1y1(x1 * y1);
			FPUint128 x1y2(x1 * y2);

			FPUint128 x2y0(x2 * y0);
			FPUint128 x2y1(x2 * y1);

			FPUint128 x3y0{ x3 * y0 };
			return x0y0 + ((x0y1 + x1y0) << 32) + ((x0y2 + x1y1 + x2y0) << 64) + ((x0y3 + x1y2 + x2y1 + x3y0) << 96);
		}
	}

	constexpr FPUint128 operator/(FPUint128 another) const {
		FPUint64 j = LeadingZeroes();
		FPUint128 bits{ *this };
		bits <<= j;
		FPUint128 numerator;
		FPUint128 quotient;
		for (; j < 128; ++j) {
			numerator <<= (FPUint64)1;
			if (bits.high & topBit) {
				numerator.low |= 1;
			}
			bits <<= (FPUint64)1;
			quotient <<= (FPUint64)1;
			if (numerator >= another) {
				++quotient;
				numerator -= another;
			}
		}
		return quotient;
	}

	constexpr FPUint128 operator%(FPUint128 another) const {
		FPUint32 j = LeadingZeroes();
		FPUint128 bits(*this);
		bits <<= j;
		FPUint128 remainder{};
		for (; j < 128; ++j) {
			remainder <<= 1;
			if (bits.high & topBit) {
				remainder.low |= 1;
			}
			bits <<= 1;
			if (remainder >= another) {
				remainder -= another;
			}
		}
		return remainder;
	}

	constexpr explicit operator FPUint64() {
		return low;
	}

private:

	// Operators used within this class only

	constexpr FPUint128& operator<<=(const FPUint128& another) {
		*this = *this << another;
		return *this;
	}

	constexpr FPUint128 operator++() {
		*this += 1;
		return *this;
	}

	constexpr bool operator==(FPUint128 another) const {
		return low == another.low && high == another.high;
	}

	constexpr bool operator<(FPUint128 another) const {
		return high < another.high || (high == another.high && low < another.low);
	}

	constexpr bool operator<=(FPUint128 another) const {
		return !(*this > another);
	}

	constexpr FPUint128 operator-(FPUint128 another) const {
		// Make negative by hand, because of C2248
		FPUint128 tmp = another;
		tmp.high = ~tmp.high;
		tmp.low = ~tmp.low;
		tmp++;
		return *this + tmp;
	}

private:

	FPUint64 high = 0;
	FPUint64 low = 0;

	static constexpr FPUint64 topBit{ ((FPUint64)1) << 63 };

	static constexpr FPUint64 deBruijn = 0x07edd5e59a4e28c2;

	static constexpr FPUint8 mapping[64]{
		63, 00, 58,  1, 59, 47, 53,  2, 60, 39,
		48, 27, 54, 33, 42,  3, 61, 51, 37, 40,
		49, 18, 28, 20, 55, 30, 34, 11, 43,	14,
		22,  4, 62, 57, 46, 52, 38, 26, 32, 41, 
		50, 36, 17, 19, 29, 10, 13, 21,	56, 45,
		25, 31, 35, 16,  9, 12, 44, 24, 15,  8,
		23,  7,  6,  5 };

	static constexpr FPUint32 LeadingZeroBitCount(FPUint64 x) {
		if (!x) {
			return 64;
		}

		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		x |= x >> 32;
		x -= x >> 1;
		FPUint32 base2Log = mapping[(x * deBruijn) >> 58];
		return 63 - base2Log;
	}

	constexpr FPUint32 LeadingZeroes() const {
		if (high == 0) {
			return 64 + LeadingZeroBitCount(low);
		} else {
			return LeadingZeroBitCount(high);
		}
	}
};

class HighestBitPositions {

public:

	static constexpr FPInt32 GetHighestBitPosition(FPUint32& value) {
		return GetHighestBitPosition32(value);
	}
		
	static constexpr FPInt32 GetHighestBitPosition(FPUint64& value) {
		return GetHighestBitPosition64(value);
	}
		
	static constexpr FPInt32 GetHighestBitPosition(FPUint128& value) {
		return GetHighestBitPosition128(value);
	}

private:

	// https://en.wikipedia.org/wiki/De_Bruijn_sequence
	static constexpr FPUint32 keepHighestBit(FPUint32 n) {
		n |= (n >> 1);
		n |= (n >> 2);
		n |= (n >> 4);
		n |= (n >> 8);
		n |= (n >> 16);
		return n - (n >> 1);
	}
	
	// https://en.wikipedia.org/wiki/De_Bruijn_sequence
	static constexpr FPUint8 BitPositionLookup[32] =
	{ // hash table
		 0,  1, 16,  2, 29, 17,  3, 22, 
		30, 20, 18, 11, 13,  4,  7, 23,
		31, 15, 28, 21, 19, 10, 12,  6,
		14, 27,  9,  5, 26,  8, 25, 24
	};

	static constexpr FPInt32 GetHighestBitPosition32(FPUint32 value) {
		FPInt32 res = BitPositionLookup[(keepHighestBit(value) * 0x06EB14F9U) >> 27];
		return res;
	}

	static constexpr FPInt32 GetHighestBitPosition64(FPUint64 value) {
		return value > FPUint64(0xFFFFFFFF) ? 32 + GetHighestBitPosition32(static_cast<FPUint32>(value >> 32)) : GetHighestBitPosition32(static_cast<FPUint32>(value));
	}

	static constexpr FPInt32 GetHighestBitPosition128(FPUint128 value) {
		return value > FPUint128(0xFFFFFFFFFFFFFFFF) ? 64 + GetHighestBitPosition64(static_cast<FPUint64>(value >> 64)) : GetHighestBitPosition64(static_cast<FPUint64>(value));
	}

};

class FixedPoint {
	template<const FPUint32 left, const FPUint32 right, const bool negative, const FPUint8 extraType, const FPUint64 extraValue>
	friend class FixedPointValue;

	template<const FPInt32 nominator, const FPInt32 denominator>
	friend class FixedPointFractionValue;

#ifdef USE_FPM
	using FPT = fpm::fixed<FPUint64, FPUint128, 32, true>;
#else
	using FPT = FPUint64;
#endif

	constexpr FixedPoint(bool negative, const FPT value) : inf(false), negative(negative), value(value) {
	}
	
	constexpr FixedPoint(bool inf, bool negative) : inf(true), negative(negative), value(0) {
	}

	static void Overflow() {
		Crash();
		return;
	}
	
	static void DivideByZero() {
		Crash();
		return;
	}

	static void NegativeSqrt() {
		Crash();
		return;
	}

	static void UndefinedAtan2() {
		Crash();
		return;
	}

	static void Crash() {
		((FPUint64*)0)[0] = 0;
	}

	static constexpr const FPT FromWholeValue(FPInt32 value) {
#ifdef USE_FPM
		return FPT(value);
#else
		return static_cast<FPUint64>(value < 0 ? -value : value) << FPUint64(32);
#endif
	}

	static constexpr FPT FPT_DIV(FPT x, FPT y) {
#ifdef USE_FPM
		return x / y;
#else
		FixedPoint fX(false, x);
		FixedPoint fY(false, y);
		FixedPoint res = fX / fY;
		return res.value;
#endif
	}

public:

	constexpr FixedPoint() : inf(false), negative(false), value(0) {
	}

	constexpr FixedPoint(const FixedPoint& another) : inf(another.inf), negative(another.negative), value(another.value) {
	
	}

	explicit FixedPoint(FPInt32 value) : negative(value < 0), inf(false), value(FromWholeValue(value)) {

	}

	static constexpr FixedPoint Inf() {
		return FixedPoint(true, false);
	}
	
	static constexpr FixedPoint NegativeInf() {
		return FixedPoint(true, true);
	}
	
	constexpr const FPUint32 GetWholeValue(bool& negative) const {
		negative = this->negative;
#ifdef USE_FPM
		return static_cast<FPUint32>(value);
#else
		return value >> 32;
#endif
	}

	const constexpr FixedPoint operator+(const FixedPoint& another) const {
		if (inf || another.inf) {
			return Inf();
		}

		if (negative) {
			if (another.negative) {
				// Make positive, add, make negative
				// -this + (-another) -> -(this+another)
				return -((-*this) + (-another));
			} else {
				// Subtract
				// -this + another -> another - this
				return another - (-*this);
			}
		} else if (another.negative) {
			// Subtract
			// this + (-another) -> this - (-another)
			return (*this) - (-another);
		} else {
			// Add, keep positive
			if (value > 0xFFFFFFFFFFFFFFFF - another.value) {
				// Would overflow
				Overflow();
				return Inf();
			}

			FPT res = value + another.value;
			return FixedPoint(false, res);
		}
	}

	const constexpr FixedPoint operator-(const FixedPoint& another) const;

	const constexpr FixedPoint operator-() const {
		if (inf) {
			return *this;
		}
		return FixedPoint(!negative, value);
	}
	
	const constexpr FixedPoint operator*(const FixedPoint& another) const;
	
	const constexpr FixedPoint operator/(const FixedPoint& another) const {
		if (inf || another.inf) {
			return Inf();
		}
		if (another.IsZero()) {
			DivideByZero();
			return Inf();
		}
		bool neg = (negative || another.negative) && negative != another.negative;
#ifdef USE_FPM
		FPT res = value / another.value;
#else 
		FPUint128 scaled_dividend = FPUint128(value) << FPUint128(32);
		FPUint128 res128 = scaled_dividend / another.value;
		if (res128 > 0xFFFFFFFFFFFFFFFF) {
			Overflow();
			return Inf();
		}
		FPT res = static_cast<FPUint64>(res128);
#endif
		return FixedPoint(neg, res);
	}
	
	const constexpr FixedPoint operator%(const FixedPoint& another) const {
		if (inf || another.inf) {
			return Inf();
		}
		if (another.IsZero()) {
			DivideByZero();
			return Inf();
		}
		bool neg = (negative || another.negative) && negative != another.negative;
#ifdef USE_FPM
		FPT res = fpm::fmod(value, another.value);
		return FixedPoint(neg, res);
#else
		if (value < another.value) {
			return *this;
		}

		FPUint64 quotient = value / another.value;
		return FixedPoint(neg, value - (quotient * another.value));
#endif
	}

	const constexpr FixedPoint& operator+=(const FixedPoint& another) {
		*this = *this + another;
		return *this;
	}

	const constexpr FixedPoint& operator-=(const FixedPoint& another) {
		*this = *this - another;
		return *this;
	}

	const constexpr FixedPoint& operator*=(const FixedPoint& another) {
		*this = *this * another;
		return *this;
	}

	const constexpr FixedPoint& operator/=(const FixedPoint& another) {
		*this = *this / another;
		return *this;
	}

	const constexpr bool operator==(const FixedPoint another) const {
		return negative == another.negative && value == another.value && inf == another.inf;
	}
	
	const constexpr bool operator!=(const FixedPoint another) const {
		return !(*this == another);
	}
	
	const constexpr bool operator>=(const FixedPoint another) const {
		if (inf || another.inf) {
			return false;
		}
		if (negative) {
			if (another.negative) {
				return value <= another.value;
			} else {
				return false;
			}
		} else if (another.negative) {
			return true;
		} else {
			return value >= another.value;
		}
		return false;
	}
	
	const constexpr bool operator>(const FixedPoint another) const {
		if (inf || another.inf) {
			return false;
		}
		if (negative) {
			if (another.negative) {
				return value < another.value;
			} else {
				return false;
			}
		} else if (another.negative) {
			return true;
		} else {
			return value > another.value;
		}
		return false;
	}
	
	const constexpr bool operator<=(const FixedPoint another) const {
		if (inf || another.inf) {
			return false;
		}
		return !operator>(another);
	}
	
	const constexpr bool operator<(const FixedPoint another) const {
		if (inf || another.inf) {
			return false;
		}
		return !operator>=(another);
	}

	const constexpr bool IsInf() const {
		return inf;
	}

	const constexpr bool IsNegative() const {
		return negative;
	}

	const constexpr FixedPoint Floor() {
		if (inf) {
			return *this;
		}
#ifdef USE_FPM
		return FixedPoint(negative, fpm::floor(value));
#else
		value = value & 0xFFFFFFFF00000000;
		return FixedPoint(negative, value);
#endif
	}

	const constexpr FixedPoint Abs() {
		if (inf) {
			return Inf();
		}
		return FixedPoint(false, value);
	}

	const constexpr bool AboveZero() const {
#ifdef USE_FPM
		return !inf && !negative && value.raw_value() > 0;
#else
		return !inf && !negative && value > 0;
#endif
	}
	
	const constexpr bool BelowZero() const {
#ifdef USE_FPM
		return !inf && negative && value.raw_value() > 0;
#else
		return !inf && negative && value > 0;
#endif
	}

	const constexpr bool IsZero() const {
#ifdef USE_FPM
		return !inf && !negative && value.raw_value() == 0;
#else
		return !inf && !negative && value == 0;
#endif
	}

	static constexpr FixedPoint cos(FixedPoint fp);
	
	static constexpr FixedPoint sin(FixedPoint fp);
	
	static constexpr FixedPoint sqrt(FixedPoint fp);
	
	static constexpr FixedPoint fmod(FixedPoint fp, FixedPoint mod) {
		if (fp.inf || mod.inf) {
			return Inf();
		}
		return fp % mod;
	}
	
	static constexpr FixedPoint exp(FixedPoint fp);
	
	static constexpr FixedPoint floor(FixedPoint fp) {
		return fp.Floor();
	}

	static constexpr FixedPoint atan2(FixedPoint fp, FixedPoint fp2);
	
	static constexpr bool isfinite(FixedPoint fp) {
		return !fp.IsInf();
	}

// Helper functions

	template<size_t N>
	static constexpr size_t InverseLength(char const (&)[N]) {
		return N - 1;
	}

	static constexpr FPUint32 Inverse(FPUint32 value, size_t order_t) {
		FPUint32 order = static_cast<FPUint32>(order_t);

		FPUint64 res = 0;
		for (FPUint32 i = 0; i < order; i++) {
			res *= 10;
			res += value % 10;
			value /= 10;
		}

		return static_cast<FPUint32>(res);
	}

	void constexpr PrintValue(char* buffer, const FPUint32 bufferSize_) const;

private:

	// functions for internal use

private:
	bool negative;
	bool inf;
	FPT value;

	static FixedPoint F_ZERO;
	static FixedPoint F_ONE;
	static FixedPoint F_THREE;
	static FixedPoint F_FOUR;
	static FixedPoint F_TEN;
	
	static FixedPoint F_PI;
	static FixedPoint F_2PI;
	static FixedPoint F_HALF_PI;
	static FixedPoint F_E;

};

template<const FPUint32 left, const FPUint32 right, const bool negative=false, const FPUint8 extraType=0, const FPUint64 extraValue=0>
class FixedPointValue {

	using FPT = FixedPoint::FPT;

public:

	constexpr FixedPointValue() : value(FPT_Create()) {

	}

	constexpr operator FixedPoint() {
		return FixedPoint(negative, value);
	}

private:

	const FPT value;

	static constexpr FPT FPT_Create() {
		switch (extraType) {
		case 0:
		{
			// Values in left and right, based in 10

			FPUint32 tmp = right;
			FPUint64 pow = 0;
			FPUint64 ten = 10;
			FPUint64 num = 0;

			while (tmp > 0) {
				num *= 10;
				num += tmp % 10;
				tmp /= 10;
				pow++;
			}
			if (num > 0xFFFFFFFF) {
				FixedPoint::Overflow();
				return FPT(0);
			}
			
			FPT fp = FixedPoint::FromWholeValue(left);
			FPT fNum = FixedPoint::FromWholeValue(static_cast<FPUint32>(num));
			while (pow > 0) {
				fNum = FixedPoint::FPT_DIV(fNum, FixedPoint::FromWholeValue(10));
				pow--;
			}
			return fp + fNum;
		}
		case 1:
		{
			// Values are encoded in extra value

#ifdef USE_FPM
			return FPT::from_raw_value(extraValue);
#else
			return (FPT)extraValue;
#endif

		}
		default:
			FixedPoint::Crash();
#ifdef USE_FPM
			return FPT::from_raw_value(0);
#else
			return (FPT)0;
#endif
		}
	}

};

template<const FPInt32 nominator, const FPInt32 denominator>
class FixedPointFractionValue {
	
	using FPT = FixedPoint::FPT;

public:

	constexpr FixedPointFractionValue() {

	}

	constexpr operator FixedPoint() {
		constexpr FPT vNom = FixedPoint::FromWholeValue(nominator < 0 ? -nominator : nominator);
		constexpr FPT vDen = FixedPoint::FromWholeValue(denominator < 0 ? -denominator : denominator);
		constexpr FixedPoint nom(nominator < 0, vNom);
		constexpr FixedPoint den(denominator < 0, vDen);
		return nom / den;
	}

};

#ifdef BOX2D_IMPL
FixedPoint FixedPoint::F_ZERO = F_VALUE(0, 0);
FixedPoint FixedPoint::F_ONE F_VALUE(1, 0);
FixedPoint FixedPoint::F_THREE F_VALUE(3, 0);
FixedPoint FixedPoint::F_FOUR F_VALUE(4, 0);
FixedPoint FixedPoint::F_TEN F_VALUE(10, 0);
FixedPoint FixedPoint::F_PI F_VALUE(3, 295141);
FixedPoint FixedPoint::F_2PI F_VALUE(6, 481382);
FixedPoint FixedPoint::F_HALF_PI F_VALUE(1, 697075);
FixedPoint FixedPoint::F_E F_VALUE(1, 28182817);
#endif

const constexpr FixedPoint FixedPoint::operator-(const FixedPoint& another) const {
	if (inf || another.inf) {
		return Inf();
	}

	if (negative) {
		if (another.negative) {
			// -this - (-another) -> another - this
			return (-another) - (-*this);
		} else {
			// -this - another -> - (this + another)
			return -((-*this) + another);
		}
	} else if (another.negative) {
		// this - (-another) -> this + another
		return *this + (-another);
	} else { // Both positive
		if (value == another.value) {
			return F_ZERO;
		} else if (value > another.value) {
			FPT res = value - another.value;
			return FixedPoint(false, res);
		} else { // value < another.value
			FPT res = another.value - value;
			return FixedPoint(true, res);
		}
	}
}

const constexpr FixedPoint FixedPoint::operator*(const FixedPoint& another) const {
	if (inf || another.inf) {
		return Inf();
	}
	if (IsZero() || another.IsZero()) {
		return F_ZERO;
	}

	bool neg = (negative || another.negative) && negative != another.negative;

#ifdef USE_FPM
	FPUint128 x = static_cast<FPUint128>(value.raw_value());
	FPUint128 y = static_cast<FPUint128>(another.value.raw_value());
#else
	FPUint128 x = static_cast<FPUint128>(value);
	FPUint128 y = static_cast<FPUint128>(another.value);
#endif

	FPUint128 result = (x * y) >> 32;
	if (result > 0xFFFFFFFFFFFFFFFF) {
		Overflow();
		return Inf();
	}

#ifdef USE_FPM
	FPT res = FPT::from_raw_value(static_cast<FPUint64>(result));
#else
	FPT res = static_cast<FPUint64>(result);
#endif
	
	return FixedPoint(neg, res);
}

constexpr FixedPoint FixedPoint::cos(FixedPoint fp) {
	if (fp.inf) {
		return fp;
	}
	if (fp.AboveZero()) {
		return sin(fp + F_HALF_PI);
	} else if (fp.IsZero()) {
		return F_ONE;
	} else { // FP < 0
		return cos(fp.Abs());
	}
}

constexpr FixedPoint FixedPoint::sin(FixedPoint fp) {
	if (fp.inf) {
		return fp;
	}
	if (fp.BelowZero()) {
		return -sin(-fp);
	} else if (fp >= F_2PI) {
		return sin(fp % (F_2PI));
	} else  if (fp < F_HALF_PI) {
#ifdef USE_FPM
		return FixedPoint(false, fpm::sin(tmp.value));
#else
		// https://en.wikipedia.org/wiki/Bh%C4%81skara_I%27s_sine_approximation_formula
		fp = (fp * FixedPointValue<180, 0>()) / F_PI;
		FixedPoint nom = F_FOUR * fp* ((FixedPoint)FixedPointValue<180, 0>() - fp);
		FixedPoint den = (FixedPoint)FixedPointValue<40500, 0>() - (fp * ((FixedPoint)FixedPointValue<180, 0>() - fp));
		return nom / den;
#endif
	} else if (fp < F_PI) {
		return sin(F_PI - fp);
	} else if (fp == F_PI) {
		return F_ZERO;
	} else { // tmp < F_2PI
		return -sin(fp - F_PI);
	}
}

constexpr FixedPoint FixedPoint::sqrt(FixedPoint fp) {
	if (fp.inf) {
		return fp;
	}
	if (fp.AboveZero()) {
#ifdef USE_FPM
		return FixedPoint(false, fpm::sqrt(fp.value));
#else

		// Finding the square root of an integer in base-2, from:
		// https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29

		// Shift by F first because it's fixed-point.
		FPUint128 num  = FPUint128(fp.value) << FPUint128(32);
		FPUint128 res = 0;

		// "bit" starts at the greatest power of four that's less than the argument.
		for (FPUint128 bit = FPUint128(1) << ((HighestBitPositions::GetHighestBitPosition(fp.value) + 32) & FPUint64(~1)); bit != 0; bit >>= 2) {
			const FPUint128 val = res + bit;
			res >>= 1;
			if (num >= val) {
				num -= val;
				res += bit;
			}
		}

		// Round the last digit up if necessary
		if (num > res) {
			res++;
		}

		return FixedPoint(false, FPT(res));

#endif
	} else if (fp.IsZero()) {
		return fp;
	} else {
		// Sqrt of negative number
		NegativeSqrt();
		return Inf();
	}
}

constexpr FixedPoint FixedPoint::exp(FixedPoint fp) {
	if (fp.inf) {
		return fp;
	}
	if (fp.BelowZero()) {
		return F_ZERO;
	} else {
#ifdef USE_FPM
		return FixedPoint(false, fpm::exp(fp.value));
#else
		FixedPoint term = F_ONE;
		FixedPoint result = F_ONE;
		for (FPUint32 i = 1; i < 5; i++) {
			FixedPoint order = FixedPoint(i);
			term *= fp / order;
			result += term;
		}
		return result;
#endif
	}
}

constexpr FixedPoint FixedPoint::atan2(FixedPoint fp, FixedPoint fp2) {
	if (fp.inf || fp2.inf) {
		return Inf();
	}
	FixedPoint r;
	FixedPoint angle;
	FixedPoint abs_y = fp.Abs() + F_VALUE(0, 1000000);
	if (fp2.BelowZero()) {
		r = (fp2 + abs_y) / (abs_y - fp2);
		const FixedPoint THRQTR_PI = F_THREE * F_PI / F_FOUR;
		angle = THRQTR_PI;
	} else {
		r = (fp2 - abs_y) / (fp2 + abs_y);
		const FixedPoint ONEQTR_PI = F_PI / F_FOUR;
		angle = ONEQTR_PI;
	}
	angle += (F_VALUE(0, 3691) * r * r - F_VALUE(0, 7189)) * r;
	if (fp.BelowZero()) {
		return -angle;
	} else {
		return angle;
	}
}

constexpr void FixedPoint::PrintValue(char* buffer, const FPUint32 bufferSize_) const {
	FPUint32 bufferSize = bufferSize_;
	struct tmp {
		char** buffer;
		FPUint32* bufferSize;
		void(*Append)(struct tmp&, char);
	} tmpI{ &buffer, &bufferSize, nullptr };

	if (bufferSize > 0) {
		buffer[0] = 0;
	}

	void(*Append)(struct tmp&, char) = [](struct tmp& tmpI, char c) {
		char*& buffer = *tmpI.buffer;
		FPUint32& bufferSize = *tmpI.bufferSize;

		if (bufferSize > 1) {
			buffer[0] = c;
			buffer = &(buffer[1]);
			bufferSize--;
			buffer[0] = 0;
		}
	};
	tmpI.Append = Append;

	void(*CopyBuffer)(struct tmp&, char*, const FPUint32) = [](struct tmp& tmpI, char* data, const FPUint32 dataSize) {
		for (FPUint32 i = 0; i < dataSize; i++) {
			if (data[i] == 0) {
				break;
			}
			tmpI.Append(tmpI, data[i]);
		}
	};
	
	if (negative) {
		Append(tmpI, '-');
	}
	if (inf) {
		Append(tmpI, 'I');
		Append(tmpI, 'n');
		Append(tmpI, 'f');
		return;
	}

	char(*GetDigit)(FixedPoint&) = [](FixedPoint& fp) {
		if (fp >= F_VALUE(9, 0)) {
			return '9';
		} else if (fp >= F_VALUE(8, 0)) {
			return '8';
		} else if (fp >= F_VALUE(7, 0)) {
			return '7';
		} else if (fp >= F_VALUE(6, 0)) {
			return '6';
		} else if (fp >= F_VALUE(5, 0)) {
			return '5';
		} else if (fp >= F_VALUE(4, 0)) {
			return '4';
		} else if (fp >= F_VALUE(3, 0)) {
			return '3';
		} else if (fp >= F_VALUE(2, 0)) {
			return '2';
		} else if (fp >= F_VALUE(1, 0)) {
			return '1';
		} else {
			return '0';
		}
	};

	FixedPoint high = floor(*this);
	FixedPoint low = (*this) - high;
	if (low.BelowZero()) {
		low = -low;
	}
	char highBuffer[64] = {};
	highBuffer[0] = '0';
	highBuffer[1] = 0;
	bool highNegative = false;
	FPUint32 chigh = high.GetWholeValue(highNegative);
	FPUint32 digits = chigh > 0 ? 0 : 1;
	for (FPUint32 i = 0; i < sizeof(highBuffer) - 2 && chigh > 0; i++) {
		FPUint32 tmp = chigh % 10;
		chigh /= 10;
		highBuffer[i] = '0' + tmp;
		highBuffer[i + 1] = 0;
		digits++;
	}
	for (FPUint32 i = 0; i < digits / 2; i++) {
		char tmp = highBuffer[i];
		highBuffer[i] = highBuffer[digits - (i + 1)];
		highBuffer[digits - (i + 1)] = tmp;
	}

	char lowBuffer[64] = {};
	lowBuffer[0] = '0';
	lowBuffer[1] = 0;
	for (FPUint32 i = 0; i < sizeof(lowBuffer) - 2 && !low.IsZero(); i++) {
		low *= F_TEN;
		FixedPoint tmp = floor(low);
		low -= tmp;
		lowBuffer[i] = GetDigit(tmp);
		lowBuffer[i + 1] = 0;
		if (i + 1 == sizeof(lowBuffer) - 2) {
			if (!low.IsZero()) {
				lowBuffer[i + 1] = '1';
				lowBuffer[i + 2] = 0;
			}
		}
	}
	CopyBuffer(tmpI, highBuffer, sizeof(highBuffer));
	Append(tmpI, '.');
	CopyBuffer(tmpI, lowBuffer, sizeof(lowBuffer));
}

#pragma push_macro("float")
#ifdef float
#undef float
#endif
#pragma push_macro("double")
#ifdef double
#undef double
#endif
#pragma push_macro("cosf")
#ifdef cosf
#undef cosf
#endif
#pragma push_macro("sinf")
#ifdef sinf
#undef sinf
#endif
#pragma push_macro("sqrtf")
#ifdef sqrtf
#undef sqrtf
#endif
#pragma push_macro("expf")
#ifdef expf
#undef expf
#endif
#pragma push_macro("floorf")
#ifdef floorf
#undef floorf
#endif
#pragma push_macro("atan2f")
#ifdef atan2f
#undef atan2f
#endif
#pragma push_macro("isfinite")
#ifdef isfinite
#undef isfinite
#endif
#pragma push_macro("vfprintf")
#ifdef vfprintf
#undef vfprintf
#endif

template<typename... T>
void b2Dump(const char* _string, T... vals);

#ifdef BOX2D_IMPL
template<typename... T>
void b2Dump(const char* _string, T... vals) {

}
#endif

#define float FixedPoint
#define double error type

inline float b2Abs(float a) {
	return a.Abs();
}

#define cosf FixedPoint::cos
#define sinf FixedPoint::sin
#define sqrtf FixedPoint::sqrt
#define expf FixedPoint::exp
#define floorf FixedPoint::floor
#define atan2f FixedPoint::atan2
#define isfinite FixedPoint::isfinite

#else
// No fixed point
#pragma push_macro("F_CONST")
#ifdef F_CONST
#undef F_CONST
#endif
#define F_CONST(left, right, original) original

#pragma push_macro("F_INVERSE")
#ifdef F_INVERSE
#undef F_INVERSE
#endif
#define F_INVERSE(num, len) 0
#endif

#ifdef BOX2D_NO_TIMER
class b2Timer {

public:

	b2Timer() {

	}

	const constexpr float GetMilliseconds() {
#ifdef BOX2D_USE_FIXED_POINT
		return (const float)FixedPointValue<0, 0>();
#else
		return 0.0f;
#endif
	}

	void Reset() {

	}
};
#endif


// INCLUDE_POST_HEADERS_END

#endif 

// Include box2 headers/implementation

#ifdef BOX2D_NO_OPEN
#pragma push_macro("fopen")
#ifdef fopen
#undef fopen
#endif
#define fopen(...) NULL
#endif

#define BOX2D_NO_INCLUDES
#include "lib_box2d.h"
#undef BOX2D_NO_INCLUDES

#ifdef BOX2D_NO_OPEN
#pragma pop_macro("fopen")
#endif

#ifdef BOX2D_EXTERNAL

// INCLUDE_POST_IMPL_BEGIN

// Restore defined macros

#ifdef BOX2D_USE_FIXED_POINT
#pragma pop_macro("vfprintf")
#pragma pop_macro("isfinite")
#pragma pop_macro("atan2f")
#pragma pop_macro("floorf")
#pragma pop_macro("expf")
#pragma pop_macro("sqrtf")
#pragma pop_macro("sinf")
#pragma pop_macro("cosf")
#pragma pop_macro("double")
#pragma pop_macro("float")

#pragma pop_macro("FLT_EPSILON")
#pragma pop_macro("FLT_MIM")
#pragma pop_macro("FLT_MAX")
#pragma pop_macro("F_INVERSE")
#pragma pop_macro("F_CONST")
#pragma pop_macro("F_VALUE")
#else
#pragma pop_macro("F_INVERSE")
#pragma pop_macro("F_CONST")
#endif

// INCLUDE_POST_IMPL_END

#endif
