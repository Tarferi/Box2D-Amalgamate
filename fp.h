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


#pragma push_macro("FP_TEST")
#ifdef FP_TEST
#undef FP_TEST
#endif
#pragma push_macro("FP_DEC")
#ifdef FP_DEC
#undef FP_DEC
#endif
#pragma push_macro("FP_DEF")
#ifdef FP_DEF
#undef FP_DEF
#endif
#pragma push_macro("FP_EXP")
#ifdef FP_EXP
#undef FP_EXP
#endif

// Parameters of FixedPoint type
#define FP_DEC \
	const bool RoundDivided,  /* Round last bit in division operations */ \
	const FPUint32 SqrtIterationsCount, /* Number of iterations in sqrt. The more the slower/more precise results */ \
	class EventHandler, /* Class for handling erroneous situations */ \
	const bool Atan2UsingAtan  /* Calculate atan2 using atan function */

#define FP_DEF \
	/* RoundDivided */ true, \
	/* SqrtIterationsCount */ 32, \
	/* EventHandler */ FixedPointSegfaultEventHandler, \
	/* Atan2UsingAtan */ true

#define FP_TEST /* Test suite  */ true, 32, FixedPointTests, true
#define FP_EXP /* To expose template arguments in FixedPointT class */  \
	public: static constexpr const bool RoundDivided = RoundDivided;\
	private: static constexpr const FPUint32 SqrtIterationsCount = SqrtIterationsCount;

#pragma push_macro("F_CONST")
#ifdef F_CONST
#undef F_CONST
#endif
#define F_CONST(left, num, len, original) (DefaultFixedPoint::FixedPointValue<left, DefaultFixedPoint::Inverse((FPUint32)num, DefaultFixedPoint::InverseLength(len))>())

#pragma push_macro("FLT_MAX")
#ifdef FLT_MAX
#undef FLT_MAX
#endif
#define FLT_MAX DefaultFixedPoint::MAX_()

#pragma push_macro("FLT_MIM")
#ifdef FLT_MIN
#undef FLT_MIN
#endif
#define FLT_MIN DefaultFixedPoint::MIN_()

#pragma push_macro("FLT_EPSILON")
#ifdef FLT_EPSILON
#undef FLT_EPSILON
#endif
#define FLT_EPSILON  DefaultFixedPoint::EPSILON()

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

class FixedPointIgnoreAllErrors {

public:

	static void Overflow() { }

	static void MalformedValue() { }

	static void DivideByZero() { }

	static const void NegativeSqrt() { }

	static const void UndefinedAtan2() { }

};

class FixedPointIgnoreOverflow {

public:

	static void Overflow() { 
	
	}

	static void MalformedValue() {
		Crash();
	}

	static void DivideByZero() {
		Crash();
	}

	static const void NegativeSqrt() { 
		Crash();
	}

	static const void UndefinedAtan2() { 
		Crash();
	}

private:

	static const void Crash() {
		((FPUint64*)0)[0] = 0;
	}

};

class FixedPointSegfaultEventHandler {

public:

	static void Overflow() {
		Crash();
		return;
	}

	static void MalformedValue() {
		Crash();
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

private:

	static const void Crash() {
		((FPUint64*)0)[0] = 0;
	}

};

template<FP_DEC>
class FixedPointT {

	struct TTypeN {
		enum TType : char {
			Positive,
			Negative,
			Zero,
			NaN,
			Inf,
			NegInf
		};

		enum TCombinedType : char {
			PositivePositive, PositiveNegative, PositiveZero, PositiveNaN, PositiveInf, PositiveNegInf,
			NegativePositive, NegativeNegative, NegativeZero, NegativeNaN, NegativeInf, NegativeNegInf,
			ZeroPositive, ZeroNegative, ZeroZero, ZeroNaN, ZeroInf, ZeroNegInf,
			NaNPositive, NaNNegative, NaNZero, NaNNaN, NaNInf, NaNNegInf,
			InfPositive, InfNegative, InfZero, InfNaN, InfInf, InfNegInf,
			NegInfPositive, NegInfNegative, NegInfZero, NegInfNaN, NegInfInf, NegInfNegInf
		};
	};

	using Type = typename TTypeN::TType;
	using CombinedType = typename TTypeN::TCombinedType;

#ifdef USE_FPM
	using FPT = fpm::fixed<FPUint64, FPUint128, 32, RoundDivided>;
#else
	using FPT = FPUint64;
#endif

public:

	template<const FPUint32 left, const FPUint32 right=0, const bool tnegative=false>
	static constexpr const FixedPointT FixedPointValue() {

		Type(*GetType)() = []() {

			if (tnegative) {
				if (left > 0 || right > 0) {
					return Type::Negative;
				} else {
					return Type::Zero;
				}
			} else if (left > 0 || right > 0) {
				return Type::Positive;
			} else {
				return Type::Zero;
			}

		};

		FPT(*FPT_Create)() = []() {
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
				EventHandler::Overflow();
				return FPT(0);
			}
			FPUint32 cLeft = static_cast<FPUint32>(left);
			FPUint32 cNum = static_cast<FPUint32>(num);
			FPUint32 cTen = 10;
			FPT fp = FixedPointT::FromWholeValue(cLeft);
			FPT fNum = FixedPointT::FromWholeValue(cNum);
			while (num > 0 && pow > 0) {
				fNum = FixedPointT::FPT_DIV(fNum, FixedPointT::FromWholeValue(cTen));
				pow--;
			}
			return fp + fNum;

		};

		return FixedPointT(GetType(), FPT_Create());
	}
	
	template<const FPUint64 extraValue, const bool negative=false>
	static constexpr const FixedPointT FixedPointEncodedValue() {

		Type(*GetType)() = []() {
			if (extraValue == 0) {
				return Type::Zero;
			} else if (negative) {
				return Type::Negative;
			} else {
				return Type::Positive;
			}
		};

		FPT(*FPT_Create)() = []() {
			// Values are encoded in extra value

#ifdef USE_FPM
			return FPT::from_raw_value(extraValue);
#else
			return (FPT)extraValue;
#endif

		};
	
		return FixedPointT(GetType(), FPT_Create());
	}

	template<const FPInt32 nominator, const FPInt32 denominator>
	static constexpr const FixedPointT FixedPointFractionValue() {
		constexpr const Type nType = nominator == 0 ? Type::Zero : nominator > 0 ? Type::Positive : Type::Negative;
		constexpr const Type dType = denominator == 0 ? Type::Zero : denominator > 0 ? Type::Positive : Type::Negative;
		constexpr FPUint32 cNominator = static_cast<FPUint32>(nominator < 0 ? -nominator : nominator);
		constexpr FPUint32 cDenominator = static_cast<FPUint32>(denominator < 0 ? -denominator : denominator);
		constexpr FPT vNom = FromWholeValue(cNominator);
		constexpr FPT vDen = FromWholeValue(cDenominator);
		constexpr FixedPointT nom(nType, vNom);
		constexpr FixedPointT den(dType, vDen);
		return nom / den;
	}

	FP_EXP;

private:

	static constexpr const FixedPointT asinDataFirst() { return FixedPointValue<0, 0>(); }
	static constexpr const FixedPointT asinDataLast() { return FixedPointValue<1, 0>(); }
	static constexpr const FPUint64 asinData[] = {
		0x0000000000, 0x000100002A, 0x0002000154, 0x0003000480, 0x0004000AAA, 0x00050014D6, 0x0006002402, 0x000700392E, 0x000800555E, 0x0009007990,
		0x000A00A6C6, 0x000B00DE04, 0x000C012048, 0x000D016E96, 0x000E01C9F2, 0x000F02335E, 0x001002ABDE, 0x0011033476, 0x001203CE2A, 0x0013047A02,
		0x0014053902, 0x0015060C30, 0x001606F496, 0x001707F33C, 0x0018090928, 0x00190A3768, 0x001A0B7F02, 0x001B0CE106, 0x001C0E5E80, 0x001D0FF87C,
		0x001E11B008, 0x001F138638, 0x0020157C18, 0x00211792BC, 0x002219CB36, 0x00231C269E, 0x00241EA606, 0x0025214A86, 0x0026241536, 0x0027270732,
		0x00282A2196, 0x00292D657A, 0x002A30D400, 0x002B346E48, 0x002C383572, 0x002D3C2AA4, 0x002E404F00, 0x002F44A3B0, 0x00304929DA, 0x00314DE2AA,
		0x003252CF4C, 0x003357F0EE, 0x00345D48C4, 0x003562D7FE, 0x0036689FD2, 0x00376EA178, 0x003874DE26, 0x00397B571C, 0x003A820D96, 0x003B8902D6,
		0x003C90381E, 0x003D97AEB6, 0x003E9F67E4, 0x003FA764F6, 0x0040AFA738, 0x0041B82FFA, 0x0042C10094, 0x0043CA1A5A, 0x0044D37EA6, 0x0045DD2ED4,
		0x0046E72C48, 0x0047F17864, 0x0048FC148E, 0x004A070232, 0x004B1242BE, 0x004C1DD7A2, 0x004D29C256, 0x004E360452, 0x004F429F12, 0x00504F941A,
		0x00515CE4EE, 0x00526A9316, 0x005378A022, 0x0054870DA4, 0x005595DD32, 0x0056A51068, 0x0057B4A8E4, 0x0058C4A84C, 0x0059D5104C, 0x005AE5E290,
		0x005BF720CC, 0x005D08CCBC, 0x005E1AE81C, 0x005F2D74B2, 0x0060407448, 0x006153E8AC, 0x006267D3B6, 0x00637C3742, 0x0064911530, 0x0065A66F6C,
		0x0066BC47E2, 0x0067D2A08A, 0x0068E97B60, 0x006A00DA68, 0x006B18BFB0, 0x006C312D46, 0x006D4A2548, 0x006E63A9D8, 0x006F7DBD1E, 0x007098614E,
		0x0071B398A2, 0x0072CF6560, 0x0073EBC9D0, 0x007508C84C, 0x0076266330, 0x0077449CE6, 0x00786377DE, 0x007982F698, 0x007AA31B96, 0x007BC3E96E,
		0x007CE562BA, 0x007E078A22, 0x007F2A6258, 0x00804DEE1D, 0x008172303B, 0x0082972B89, 0x0083BCE2EB, 0x0084E35953, 0x00860A91C1, 0x0087328F43,
		0x00885B54F3, 0x008984E5FB, 0x008AAF4595, 0x008BDA770B, 0x008D067DB7, 0x008E335CFF, 0x008F611863, 0x00908FB36D, 0x0091BF31BD, 0x0092EF9703,
		0x009420E705, 0x0095532599, 0x00968656AF, 0x0097BA7E49, 0x0098EFA07D, 0x009A25C17D, 0x009B5CE589, 0x009C951105, 0x009DCE4863, 0x009F089033,
		0x00A043ED23, 0x00A18063F5, 0x00A2BDF98D, 0x00A3FCB2E9, 0x00A53C9527, 0x00A67DA585, 0x00A7BFE95F, 0x00A9036631, 0x00AA4821A1, 0x00AB8E2171,
		0x00ACD56B8F, 0x00AE1E0609, 0x00AF67F719, 0x00B0B34523, 0x00B1FFF6B7, 0x00B34E128D, 0x00B49D9F93, 0x00B5EEA4DF, 0x00B74129C1, 0x00B89535BB,
		0x00B9EAD083, 0x00BB42020D, 0x00BC9AD281, 0x00BDF54A4B, 0x00BF517217, 0x00C0AF52CF, 0x00C20EF5A9, 0x00C3706421, 0x00C4D3A7FF, 0x00C638CB5D,
		0x00C79FD8A7, 0x00C908DAA3, 0x00CA73DC6D, 0x00CBE0E987, 0x00CD500DD5, 0x00CEC155A5, 0x00D034CDB3, 0x00D1AA832F, 0x00D32283C7, 0x00D49CDDA3,
		0x00D6199F79, 0x00D798D889, 0x00D91A98AF, 0x00DA9EF05F, 0x00DC25F0B9, 0x00DDAFAB8F, 0x00DF3C3369, 0x00E0CB9B9D, 0x00E25DF84F, 0x00E3F35E81,
		0x00E58BE425, 0x00E727A027, 0x00E8C6AA81, 0x00EA691C47, 0x00EC0F0FC3, 0x00EDB8A081, 0x00EF65EB67, 0x00F1170ED5, 0x00F2CC2AB3, 0x00F485609D,
		0x00F642D3F7, 0x00F804AA15, 0x00F9CB0A65, 0x00FB961E97, 0x00FD6612C7, 0x00FF3B15BB, 0x010115591B, 0x0102F511B1, 0x0104DA77AB, 0x0106C5C6F7,
		0x0108B73F99, 0x010AAF2611, 0x010CADC3D3, 0x010EB367C1, 0x0110C066D1, 0x0112D51CA9, 0x0114F1EC65, 0x0117174183, 0x01194590DD, 0x011B7D59DB,
		0x011DBF27DB, 0x01200B93CB, 0x0122634617, 0x0124C6F8F5, 0x0127377B23, 0x0129B5B337, 0x012C42A3A1, 0x012EDF6FAB, 0x01318D618F, 0x01344DF235,
		0x013722D2FD, 0x013A0DFA49, 0x013D11B3FB, 0x014030B73B, 0x01436E4417, 0x0146CE4C71, 0x014A55AE31, 0x014E0A887B, 0x0151F4BD11, 0x01561EBD9B,
		0x015A96E349, 0x015F71D7FD, 0x0164CF5513, 0x016AE4C631, 0x01721A5D85, 0x017B7D33B7, 0x01921FB544
	};

	static constexpr const FixedPointT sinDataFirst() { return FixedPointValue<0, 0>(); }
	static constexpr const FixedPointT sinDataLast() { return FixedPointValue<1, 6>(); }
	static constexpr const FPUint64 sinData[] = { // From 0 to 1.6
		0x0000000000, 0x00019998EA, 0x0003332DBC, 0x0004CCBA5E, 0x0006663AB4, 0x0007FFAAAA, 0x0009990626, 0x000B324910, 0x000CCB6F50, 0x000E6474D0,
		0x000FFD5576, 0x0011960D2E, 0x00132E97E2, 0x0014C6F178, 0x00165F15E0, 0x0017F70102, 0x00198EAECA, 0x001B261B26, 0x001CBD4200, 0x001E541F4A,
		0x001FEAAEEE, 0x002180ECDE, 0x002316D508, 0x0024AC635E, 0x00264193D2, 0x0027D66258, 0x00296ACAE0, 0x002AFEC964, 0x002C9259D4, 0x002E25782A,
		0x002FB8205E, 0x00314A4E6A, 0x0032DBFE46, 0x00346D2BEE, 0x0035FDD362, 0x00378DF09C, 0x00391D7FA0, 0x003AAC7C6C, 0x003C3AE304, 0x003DC8AF6C,
		0x003F55DDA8, 0x0040E269C2, 0x00426E4FC2, 0x0043F98BB2, 0x004584199C, 0x00470DF592, 0x0048971BA2, 0x004A1F87DC, 0x004BA73656, 0x004D2E2324,
		0x004EB44A5C, 0x005039A81A, 0x0051BE3878, 0x005341F792, 0x0054C4E188, 0x005646F27E, 0x0057C82694, 0x00594879F2, 0x005AC7E8C0, 0x005C466F26,
		0x005DC40954, 0x005F40B378, 0x0060BC69C4, 0x006237286C, 0x0063B0EBA4, 0x006529AFA6, 0x0066A170B0, 0x0068182AFE, 0x00698DDAD0, 0x006B027C6A,
		0x006C760C14, 0x006DE88614, 0x006F59E6B6, 0x0070CA2A4A, 0x0072394D22, 0x0073A74B8E, 0x00751421E8, 0x00767FCC8A, 0x0077EA47D2, 0x007953901E,
		0x007ABBA1D0, 0x007C227952, 0x007D88130A, 0x007EEC6B66, 0x00804F7ED7, 0x0081B149CF, 0x008311C8C1, 0x008470F82D, 0x0085CED48B, 0x00872B5A5D,
		0x0088868625, 0x0089E0546F, 0x008B38C1C1, 0x008C8FCAAB, 0x008DE56BBF, 0x008F39A191, 0x00908C68BD, 0x0091DDBDDD, 0x00932D9D93, 0x00947C0483,
		0x0095C8EF55, 0x0097145AB3, 0x00985E434D, 0x0099A6A5D9, 0x009AED7F0D, 0x009C32CBA3, 0x009D76885B, 0x009EB8B1F9, 0x009FF94543, 0x00A1383F05,
		0x00A2759C0F, 0x00A3B15933, 0x00A4EB734B, 0x00A623E72F, 0x00A75AB1C5, 0x00A88FCFEB, 0x00A9C33E8F, 0x00AAF4FA9B, 0x00AC250101, 0x00AD534EB7,
		0x00AE7FE0B5, 0x00AFAAB3FD, 0x00B0D3C591, 0x00B1FB1277, 0x00B32097BD, 0x00B4445271, 0x00B5663FA9, 0x00B6865C81, 0x00B7A4A615, 0x00B8C1198B,
		0x00B9DBB407, 0x00BAF472B7, 0x00BC0B52CF, 0x00BD205181, 0x00BE336C09, 0x00BF449FA9, 0x00C053E9A3, 0x00C1614741, 0x00C26CB5D3, 0x00C37632A9,
		0x00C47DBB21, 0x00C5834C93, 0x00C686E463, 0x00C7887FFB, 0x00C8881CC3, 0x00C985B82F, 0x00CA814FB7, 0x00CB7AE0D5, 0x00CC72690B, 0x00CD67E5E1,
		0x00CE5B54DD, 0x00CF4CB397, 0x00D03BFF9F, 0x00D1293693, 0x00D2145615, 0x00D2FD5BC9, 0x00D3E4455D, 0x00D4C9107F, 0x00D5ABBAE5, 0x00D68C424D,
		0x00D76AA479, 0x00D846DF2B, 0x00D920F035, 0x00D9F8D565, 0x00DACE8C93, 0x00DBA2139D, 0x00DC736863, 0x00DD4288D1, 0x00DE0F72D1, 0x00DEDA2459,
		0x00DFA29B61, 0x00E068D5E7, 0x00E12CD1F1, 0x00E1EE8D89, 0x00E2AE06BF, 0x00E36B3BA9, 0x00E4262A61, 0x00E4DED10B, 0x00E5952DCB, 0x00E6493ED3,
		0x00E6FB0251, 0x00E7AA7681, 0x00E85799A1, 0x00E90269F5, 0x00E9AAE5C9, 0x00EA510B6D, 0x00EAF4D937, 0x00EB964D85, 0x00EC3566B9, 0x00ECD2233D,
		0x00ED6C817D, 0x00EE047FF1, 0x00EE9A1D11, 0x00EF2D575F, 0x00EFBE2D63, 0x00F04C9DAB, 0x00F0D8A6C7, 0x00F1624753, 0x00F1E97DEF, 0x00F26E493F,
		0x00F2F0A7F1, 0x00F37098B5, 0x00F3EE1A45, 0x00F4692B61, 0x00F4E1CACB, 0x00F557F751, 0x00F5CBAFC1, 0x00F63CF2F7, 0x00F6ABBFCF, 0x00F718152B,
		0x00F781F1F9, 0x00F7E9552B, 0x00F84E3DB3, 0x00F8B0AA95, 0x00F9109ACF, 0x00F96E0D71, 0x00F9C90187, 0x00FA21762D, 0x00FA776A7B, 0x00FACADD99,
		0x00FB1BCEB1, 0x00FB6A3CF3, 0x00FBB62795, 0x00FBFF8DD5, 0x00FC466EFB, 0x00FC8ACA4B, 0x00FCCC9F1D, 0x00FD0BECC3, 0x00FD48B29D, 0x00FD82F00F,
		0x00FDBAA485, 0x00FDEFCF6F, 0x00FE227047, 0x00FE528689, 0x00FE8011BB, 0x00FEAB1167, 0x00FED38521, 0x00FEF96C81, 0x00FF1CC727, 0x00FF3D94B5,
		0x00FF5BD4D9, 0x00FF778747, 0x00FF90ABB5, 0x00FFA741E7, 0x00FFBB499F, 0x00FFCCC2AB, 0x00FFDBACDF, 0x00FFE80813, 0x00FFF1D42B, 0x00FFF91109,
		0x00FFFDBE9F, 0x00FFFFDCDF, 0x00FFFF6BC5, 0x00FFFC6B4F, 0x00FFF6DB87, 0x00FFEEBC7B, 0x00FFE40E3F
	};


	static constexpr const FixedPointT sqrtDataFirst() { return FixedPointValue<0, 0>(); }
	static constexpr const FixedPointT sqrtDataLast() { return FixedPointEncodedValue<0xFFFFFFFF>(); }
	static constexpr const FPUint64 sqrtData[] = {
		0x0000000000000000, 0x00000FFFFFFFE802, 0x000016A09E665D4B, 0x00001BB67AE82EBA, 0x00001FFFFFFFD002, 0x000023C6EF36FA3F,
		0x000027311C27D777, 0x00002A54FF536673, 0x00002D413CCCBA95, 0x00002FFFFFFFB802, 0x00003298B07568D3, 0x00003510E527AB45,
		0x0000376CF5D05D77, 0x000039B056886C2A, 0x00003BDDD42276B2, 0x00003DF7BD6241AA, 0x00003FFFFFFFA002, 0x000041F83D9A5CBE,
		0x000043E1DB3317E2, 0x000045BE0CD1289A, 0x0000478DDE6DF47E, 0x000049523AE3E67E, 0x00004B0BF164E0CA, 0x00004CBBB9D568F8,
		0x00004E62384FAEF0, 0x00004FFFFFFF8802, 0x00005195957BCE60, 0x0000532370B88C31, 0x000054A9FEA6CCE6, 0x00005629A292222A,
		0x000057A2B7485723, 0x0000591590151D68, 0x00005A827999752A, 0x00005BE9BA850166, 0x00005D4B9436429E, 0x00005EA84345C10C,
		0x00005FFFFFFF7002, 0x00006152FECCFD75, 0x000062A170934827, 0x000063EB8304D56E, 0x0000653160EAD1A3, 0x000066733265E64F,
		0x000067B11D27FCBF, 0x000068EB44A7D923, 0x00006A21CA4F568A, 0x00006B54CDA4EEBD, 0x00006C846C71207A, 0x00006DB0C2E031C7,
		0x00006ED9EBA0BAEC, 0x00006FFFFFFF5802, 0x0000712317FFD275, 0x000072434A7409A9, 0x00007360AD10D857, 0x0000747B54812D35,
		0x0000759354778669, 0x000076A8BFBDF98B, 0x000077BBA844ED64, 0x000078CC1F30A60A, 0x000079DA34E5C04A, 0x00007AE5F914B624,
		0x00007BEF7AC48353, 0x00007CF6C85C7E61, 0x00007DFBEFAD7844, 0x00007EFEFDFA3357, 0x00007FFFFFFF4002, 0x000080FF01FA4C57,
		0x000081FC0FB0F2C6, 0x000082F734771231, 0x000083F07B34B97E, 0x000084E7EE6BAF26, 0x000085DD983C9CB4, 0x000086D1826BE59E,
		0x000087C3B6662FC2, 0x000088B43D44A396, 0x000089A31FD0E995, 0x00008A906688E9A6, 0x00008B7C19A25134, 0x00008C66410DE404,
		0x00008D4EE47A9B93, 0x00008E360B589872, 0x00008F1BBCDBE8FC, 0x00008FFFFFFF2802, 0x000090E2DB85F66D, 0x000091C455FF523A,
		0x000092A475C7CCFC, 0x00009383410BA42F, 0x00009460BDC8BD31, 0x0000953CF1D086BD, 0x00009617E2C9C192, 0x000096F1963231D0,
		0x000097CA11603A74, 0x000098A15984645B, 0x0000997773AAD1ED, 0x00009A4C64BCA0B6, 0x00009B20318139E8, 0x00009BF2DE9F92C1,
		0x00009CC4709F5DE2, 0x00009D94EBEA2E55, 0x00009E6454CC8D0C, 0x00009F32AF7701CF, 0x00009FFFFFFF1002, 0x0000A0CC4A60281C,
		0x0000A197927C8E82, 0x0000A261DC1E37F8, 0x0000A32B2AF79CBF, 0x0000A3F382A48260, 0x0000A4BAE6AABCED, 0x0000A5815A7AE811,
		0x0000A646E1711863, 0x0000A70B7ED58537, 0x0000A7CF35DD2BAF, 0x0000A89209AA6ADC, 0x0000A953FD4D99CA, 0x0000AA1513C59761,
		0x0000AAD5500054AB, 0x0000AB94B4DB5989, 0x0000AC5345244451, 0x0000AD1103994481, 0x0000ADCDF2E9909A, 0x0000AE8A15B5D79E,
		0x0000AF456E90AE43, 0x0000AFFFFFFEF802, 0x0000B0B9CC784C3C, 0x0000B172D66757CB, 0x0000B22B202A3ACD, 0x0000B2E2AC12E33B,
		0x0000B3997C676415, 0x0000B44F93624998, 0x0000B504F332EA57, 0x0000B5B99DFDB59C, 0x0000B66D95DC7EF4, 0x0000B720DCDEC72B,
		0x0000B7D3750A02C9, 0x0000B8856059DE29, 0x0000B936A0C07F35, 0x0000B9E73826C4FD, 0x0000BA97286C8539, 0x0000BB467368C7A4,
		0x0000BBF51AE9FF81, 0x0000BCA320B64335, 0x0000BD50868B8219, 0x0000BDFD4E1FB885, 0x0000BEA979212251, 0x0000BF5509366B93,
		0x0000BFFFFFFEE002, 0x0000C0AA5F1298AA, 0x0000C1542802A874, 0x0000C1FD5C59471B, 0x0000C2A5FD99FAEA, 0x0000C34E0D41C124,
		0x0000C3F58CC7355C, 0x0000C49C7D9AB762, 0x0000C542E126904F, 0x0000C5E8B8CF163F, 0x0000C68E05F2CF30, 0x0000C732C9EA92BB,
		0x0000C7D70609AAD9, 0x0000C87ABB9DF3CF, 0x0000C91DEBEFFB06, 0x0000C9C098431D25, 0x0000CA62C1D5A347, 0x0000CB0469E0DF54,
		0x0000CBA5919947A2, 0x0000CC463A2E91C2, 0x0000CCE664CBCC9E, 0x0000CD86129779CC, 0x0000CE2544B3A64B, 0x0000CEC3FC3E027C,
		0x0000CF623A4FF980, 0x0000CFFFFFFEC802, 0x0000D09D4E5B9245, 0x0000D13A267379D0, 0x0000D1D6894FB249, 0x0000D27277F595FA,
		0x0000D30DF366B9B8, 0x0000D3A8FCA1003D, 0x0000D443949EAD14, 0x0000D4DDBC56770A, 0x0000D57774BB9A0F, 0x0000D610BEBDE8C3,
		0x0000D6A99B49DD7A, 0x0000D7420B48AAE2, 0x0000D7DA0FA04C3A, 0x0000D871A933951E, 0x0000D908D8E240F5, 0x0000D99F9F890200,
		0x0000DA35FE01900C, 0x0000DACBF522B6BC, 0x0000DB6185C0638E, 0x0000DBF6B0ABB37D, 0x0000DC8B76B30055, 0x0000DD1FD8A1EDBC,
		0x0000DDB3D74175D8, 0x0000DE477357F5C3, 0x0000DEDAADA939A7, 0x0000DF6D86F68890, 0x0000DFFFFFFEB002, 0x0000E092197E0F3E,
		0x0000E123D42EA25A, 0x0000E1B530C80CFB, 0x0000E2462FFFA4EC, 0x0000E2D6D2887C6B, 0x0000E36719136C37, 0x0000E3F7044F1D77,
		0x0000E48694E81355, 0x0000E515CB88B468, 0x0000E5A4A8D953F3, 0x0000E6332D803AE3, 0x0000E6C15A21B0AD, 0x0000E74F2F6003DD,
		0x0000E7DCADDB9297, 0x0000E869D632D2D0, 0x0000E8F6A9025A6D, 0x0000E98326E4E72D, 0x0000EA0F50736665, 0x0000EA9B2644FC93,
		0x0000EB26A8EF0CD5, 0x0000EBB1D9054026, 0x0000EC3CB7198C83, 0x0000ECC743BC3BE5, 0x0000ED517F7BF315, 0x0000EDDB6AE5B857,
		0x0000EE650684FA07, 0x0000EEEE52E394FA, 0x0000EF775089DAC7, 0x0000EFFFFFFE9802, 0x0000F08861C71A30, 0x0000F110766735D2,
		0x0000F1983E614C16, 0x0000F21FBA36509C, 0x0000F2A6EA65CEFF, 0x0000F32DCF6DF058, 0x0000F3B469CB8094, 0x0000F43AB9F9F3BF,
		0x0000F4C0C0736B1F, 0x0000F5467DB0BA54, 0x0000F5CBF2296C46, 0x0000F6511E53C801, 0x0000F6D602A4D588, 0x0000F75A9F906279,
		0x0000F7DEF58906A9, 0x0000F863050028A9, 0x0000F8E6CE660239, 0x0000F96A5229A492, 0x0000F9ED90B8FCC0, 0x0000FA708A80D7B9,
		0x0000FAF33FECE68D, 0x0000FB75B167C268, 0x0000FBF7DF5AF086, 0x0000FC79CA2EE618, 0x0000FCFB724B0C17, 0x0000FD7CD815C30D,
		0x0000FDFDFBF466AE, 0x0000FE7EDE4B518F, 0x0000FEFF7F7DE09F, 0x0000FF7FDFEE76BA, 0x0000000000000000
	};

	constexpr FixedPointT(Type type, const FPT value) : type(type), value(value) {
	}
	
	constexpr FixedPointT(Type type) : type(type), value(0) {
	}

	static constexpr const FPT FromWholeValue(FPUint32 value) {
#ifdef USE_FPM
		return FPT(value);
#else
		return static_cast<FPUint64>(value) << FPUint64(32);
#endif
	}

	static constexpr const Type TypeFromFPT(FPT value) {
		if (value == 0) {
			return Type::Zero;
		} else if (value < 0) {
			return Type::Negative;
		} else {
			return Type::Positive;
		}
	}

	static constexpr FPT FPT_DIV(FPT x, FPT y) {
#ifdef USE_FPM
		return x / y;
#else
		FixedPointT fX(TypeFromFPT(x), x);
		FixedPointT fY(TypeFromFPT(y), y);
		FixedPointT res = fX / fY;
		return res.value;
#endif
	}

	static const constexpr CombinedType CombineTypes(Type t1, Type t2) {
		char x1 = static_cast<char>(t1);
		char x2 = static_cast<char>(t2);

		char sz = static_cast<char>(Type::NegInf) + 1; // must be LAST
		CombinedType ct = static_cast<CombinedType>((x1 * sz) + x2);
		return ct;
	}

	explicit FixedPointT(FPUint64 arData, bool, bool, bool) : type(Type::Positive), value(FPT(arData)) {
		// For constructing raw instances
	}

public:

	constexpr FixedPointT() : type(Type::Zero), value(0) {
	}

	constexpr FixedPointT(const FixedPointT& another) : type(another.type), value(another.value) {
	
	}

	explicit FixedPointT(FPInt32 value) : type(value < 0 ? Type::Negative : value == 0 ? Type::Zero : Type::Positive), value(FromWholeValue((FPUint32)(value < 0 ? -value : value))) {

	}

	static constexpr FixedPointT Inf() {
		return FixedPointT(Type::Inf);
	}
	
	static constexpr FixedPointT NegativeInf() {
		return FixedPointT(Type::NegInf);
	}
	
	static constexpr FixedPointT NaN() {
		return FixedPointT(Type::NaN);
	}
	
	constexpr const FPUint32 GetWholeValue(bool& negative) const {
		negative = type == Type::Negative;
#ifdef USE_FPM
		return static_cast<FPUint32>(value);
#else
		return value >> 32;
#endif
	}

	const constexpr FixedPointT operator+(const FixedPointT& another) const {
		CombinedType ctype = CombineTypes(type, another.type);

		switch (ctype) {
		case CombinedType::PositivePositive: 
			if (value > 0xFFFFFFFFFFFFFFFF - another.value) {
				// Would overflow
				EventHandler::Overflow();
				return Inf();
			}
			return FixedPointT(Type::Positive, value + another.value);
		case CombinedType::PositiveNegative: return (*this) - (-another);
		case CombinedType::PositiveZero: return *this;
		case CombinedType::PositiveNaN: return NaN();
		case CombinedType::PositiveInf: return another; // Infinity
		case CombinedType::PositiveNegInf: return another; // -Infinity
		case CombinedType::NegativePositive: return another - (-*this);
		case CombinedType::NegativeNegative: return -((-*this) + (-another));
		case CombinedType::NegativeZero: return *this;
		case CombinedType::NegativeNaN: return NaN();
		case CombinedType::NegativeInf: return another; // Infinity
		case CombinedType::NegativeNegInf: return another; // -Infinity
		case CombinedType::ZeroPositive: return another;
		case CombinedType::ZeroNegative: return -another;
		case CombinedType::ZeroZero: return *this;
		case CombinedType::ZeroNaN: return NaN();
		case CombinedType::ZeroInf: return another; // Infinity
		case CombinedType::ZeroNegInf: return another; // -Infinity
		case CombinedType::NaNPositive: return NaN();
		case CombinedType::NaNNegative: return NaN();
		case CombinedType::NaNZero: return NaN();
		case CombinedType::NaNNaN: return NaN();
		case CombinedType::NaNInf: return NaN();
		case CombinedType::NaNNegInf: return NaN();
		case CombinedType::InfPositive: return *this; // Infinity
		case CombinedType::InfNegative: return *this; // Infinity
		case CombinedType::InfZero: return *this; // Infinity
		case CombinedType::InfNaN: return NaN();
		case CombinedType::InfInf: return *this; // Infinity
		case CombinedType::InfNegInf: return NaN();
		case CombinedType::NegInfPositive: return *this; // -Infinity
		case CombinedType::NegInfNegative: return *this; // -Infinity
		case CombinedType::NegInfZero: return *this; // -Infinity
		case CombinedType::NegInfNaN: return NaN();
		case CombinedType::NegInfInf: return NaN();
		case CombinedType::NegInfNegInf: return *this; // -Infinity
		default: EventHandler::MalformedValue(); return NaN();
		}
	}

	const constexpr FixedPointT operator-(const FixedPointT& another) const {
		CombinedType ctype = CombineTypes(type, another.type);

		switch (ctype) {
		case CombinedType::PositivePositive:
			if (value == another.value) {
				return ZERO();
			} else if (value > another.value) {
				return FixedPointT(Type::Positive, value - another.value);
			} else { // value < another.value
				return FixedPointT(Type::Negative, another.value - value);
			}

		case CombinedType::PositiveNegative:  return *this + (-another);
		case CombinedType::PositiveZero: return *this;
		case CombinedType::PositiveNaN: return NaN();
		case CombinedType::PositiveInf: return NegativeInf();
		case CombinedType::PositiveNegInf: return Inf();
		case CombinedType::NegativePositive: return -((-*this) + another);
		case CombinedType::NegativeNegative: return (-another) - (-*this);
		case CombinedType::NegativeZero: return *this;
		case CombinedType::NegativeNaN: return NaN();
		case CombinedType::NegativeInf: return NegativeInf();
		case CombinedType::NegativeNegInf: return Inf();
		case CombinedType::ZeroPositive: return -another;
		case CombinedType::ZeroNegative: return -another;
		case CombinedType::ZeroZero: return *this;
		case CombinedType::ZeroNaN: return NaN();
		case CombinedType::ZeroInf: return NegativeInf();
		case CombinedType::ZeroNegInf: return Inf();
		case CombinedType::NaNPositive: return NaN();
		case CombinedType::NaNNegative: return NaN();
		case CombinedType::NaNZero: return NaN();
		case CombinedType::NaNNaN: return NaN();
		case CombinedType::NaNInf: return NaN();
		case CombinedType::NaNNegInf: return NaN();
		case CombinedType::InfPositive: return *this; // Infinity
		case CombinedType::InfNegative: return *this; // Infinity
		case CombinedType::InfZero: return *this; // Infinity
		case CombinedType::InfNaN: return NaN();
		case CombinedType::InfInf: return NaN();
		case CombinedType::InfNegInf: return *this; // Infinity
		case CombinedType::NegInfPositive: return *this; // -Infinity
		case CombinedType::NegInfNegative: return *this; // -Infinity
		case CombinedType::NegInfZero: return *this; // -Infinity
		case CombinedType::NegInfNaN: return NaN();
		case CombinedType::NegInfInf: return *this; // -Infinity
		case CombinedType::NegInfNegInf: return NaN();
		default: EventHandler::MalformedValue(); return NaN();
		}
	}

	const constexpr FixedPointT operator-() const {
		switch (type) {
		case Type::Positive:
			return FixedPointT(Type::Negative, value);
	
		case Type::Negative:
			return FixedPointT(Type::Positive, value);
		
		case Type::Inf:
			return NegativeInf();

		case Type::NegInf:
			return Inf();

		case Type::Zero:
		case Type::NaN:
			return *this;

		default: EventHandler::MalformedValue(); return NaN();
		}
	}
	
	const constexpr FixedPointT operator*(const FixedPointT& another) const {
		CombinedType ctype = CombineTypes(type, another.type);
		Type neg = Type::Inf;
		switch (ctype) {
		case CombinedType::PositivePositive:
		case CombinedType::NegativeNegative:
			neg = Type::Positive;
			break;

		case CombinedType::PositiveNegative:
		case CombinedType::NegativePositive:
			neg = Type::Negative;
			break;
		}


		switch (ctype) {
		case CombinedType::PositivePositive:
		case CombinedType::NegativeNegative:
		case CombinedType::PositiveNegative:
		case CombinedType::NegativePositive:
		{
#ifdef USE_FPM
			FPUint128 x = static_cast<FPUint128>(value.raw_value());
			FPUint128 y = static_cast<FPUint128>(another.value.raw_value());
#else
			FPUint128 x = static_cast<FPUint128>(value);
			FPUint128 y = static_cast<FPUint128>(another.value);
#endif

			FPUint128 result = (x * y) >> 32;
			if (result > 0xFFFFFFFFFFFFFFFF) {
				EventHandler::Overflow();
				return Inf();
			}

#ifdef USE_FPM
			FPT res = FPT::from_raw_value(static_cast<FPUint64>(result));
#else
			FPT res = static_cast<FPUint64>(result);
#endif

			return FixedPointT(neg, res);
		}

		case CombinedType::PositiveZero: return another;
		case CombinedType::PositiveNaN: return NaN();
		case CombinedType::PositiveInf: return another; // Infinity
		case CombinedType::PositiveNegInf: return another; // -Infinity
		case CombinedType::NegativeZero: return another; // 0
		case CombinedType::NegativeNaN: return NaN();
		case CombinedType::NegativeInf: return NegativeInf();
		case CombinedType::NegativeNegInf: return Inf();
		case CombinedType::ZeroPositive: return *this; // 0
		case CombinedType::ZeroNegative: return *this; // 0
		case CombinedType::ZeroZero: return *this; // 0
		case CombinedType::ZeroNaN: return NaN();
		case CombinedType::ZeroInf: return NaN();
		case CombinedType::ZeroNegInf: return NaN();
		case CombinedType::NaNPositive: return NaN();
		case CombinedType::NaNNegative: return NaN();
		case CombinedType::NaNZero: return NaN();
		case CombinedType::NaNNaN: return NaN();
		case CombinedType::NaNInf: return NaN();
		case CombinedType::NaNNegInf: return NaN();
		case CombinedType::InfPositive: return *this; // Infinity
		case CombinedType::InfNegative: return NegativeInf();
		case CombinedType::InfZero: return NaN();
		case CombinedType::InfNaN: return NaN();
		case CombinedType::InfInf: return *this; // Infinity
		case CombinedType::InfNegInf: return another; // -Infinity
		case CombinedType::NegInfPositive: return *this; // -Infinity
		case CombinedType::NegInfNegative: return Inf();
		case CombinedType::NegInfZero: return NaN();
		case CombinedType::NegInfNaN: return NaN();
		case CombinedType::NegInfInf: return *this; // -Infinity
		case CombinedType::NegInfNegInf: return Inf();
		default: EventHandler::MalformedValue(); return NaN();
		}
	}
	
	const constexpr FixedPointT operator/(const FixedPointT& another) const {
		CombinedType ctype = CombineTypes(type, another.type);

		Type neg = Type::Inf;
		switch (ctype) {
		case CombinedType::PositivePositive:
		case CombinedType::NegativeNegative:
			neg = Type::Positive;
			break;

		case CombinedType::PositiveNegative:
		case CombinedType::NegativePositive:
			neg = Type::Negative;
			break;
		}

		switch (ctype) {
		case CombinedType::PositivePositive:
		case CombinedType::NegativeNegative:
		case CombinedType::PositiveNegative:
		case CombinedType::NegativePositive:
		{
#ifdef USE_FPM
			FPT res = value / another.value;
#else 
			FPUint128 res128;
			if (RoundDivided) {
				FPUint128 scaled_dividend = FPUint128(value) << FPUint128(33);
				res128 = scaled_dividend / another.value;
				res128 = (res128 >> 1) + (res128 % 2);
			} else {
				FPUint128 scaled_dividend = FPUint128(value) << FPUint128(32);
				res128 = scaled_dividend / another.value;
			}
			if (res128 > 0xFFFFFFFFFFFFFFFF) {
				EventHandler::Overflow();
				return Inf();
			}
			FPT res = static_cast<FPUint64>(res128);
#endif
			return FixedPointT(neg, res);
		}

		case CombinedType::PositiveZero: EventHandler::DivideByZero(); return Inf();
		case CombinedType::PositiveNaN: return NaN();
		case CombinedType::PositiveInf: return ZERO();
		case CombinedType::PositiveNegInf: return ZERO();
		case CombinedType::NegativeZero: EventHandler::DivideByZero(); return NegativeInf();
		case CombinedType::NegativeNaN: return NaN();
		case CombinedType::NegativeInf: return ZERO();
		case CombinedType::NegativeNegInf: return ZERO();
		case CombinedType::ZeroPositive: return *this; // 0
		case CombinedType::ZeroNegative: return *this; // 0
		case CombinedType::ZeroZero: EventHandler::DivideByZero(); return NaN();
		case CombinedType::ZeroNaN:return NaN();
		case CombinedType::ZeroInf: return *this; // 0
		case CombinedType::ZeroNegInf:return *this; // 0
		case CombinedType::NaNPositive:return NaN();
		case CombinedType::NaNNegative:return NaN();
		case CombinedType::NaNZero: EventHandler::DivideByZero(); return  NaN();
		case CombinedType::NaNNaN:return NaN();
		case CombinedType::NaNInf: return NaN();
		case CombinedType::NaNNegInf:return NaN();
		case CombinedType::InfPositive: return *this; // Infinity
		case CombinedType::InfNegative:return NegativeInf();
		case CombinedType::InfZero: EventHandler::DivideByZero(); return *this; // Infinity
		case CombinedType::InfNaN: return NaN();
		case CombinedType::InfInf: return NaN();
		case CombinedType::InfNegInf: return NaN();
		case CombinedType::NegInfPositive: return *this; // -Infinity
		case CombinedType::NegInfNegative: return Inf();
		case CombinedType::NegInfZero: EventHandler::DivideByZero(); return *this; // -Infinity
		case CombinedType::NegInfNaN: return NaN();
		case CombinedType::NegInfInf: return NaN();
		case CombinedType::NegInfNegInf: return NaN();
		default: EventHandler::MalformedValue(); return NaN();
		}
	}
	
	const constexpr FixedPointT operator%(const FixedPointT& another) const {
		CombinedType ctype = CombineTypes(type, another.type);

		Type neg = Type::Inf;
		switch (ctype) {
		case CombinedType::PositivePositive:
		case CombinedType::NegativeNegative:
			neg = Type::Positive;
			break;

		case CombinedType::PositiveNegative:
		case CombinedType::NegativePositive:
			neg = Type::Negative;
			break;
		}

		switch (ctype) {
		case CombinedType::PositivePositive:
		case CombinedType::NegativeNegative:
		case CombinedType::PositiveNegative:
		case CombinedType::NegativePositive:
		{
#ifdef USE_FPM
			FPT res = fpm::fmod(value, another.value);
			return FixedPoint(neg, res);
#else
			if (value < another.value) {
				return *this;
			}

			FPUint64 quotient = value / another.value;
			return FixedPointT(neg, value - (quotient * another.value));
#endif
		}

		case CombinedType::PositiveZero: EventHandler::DivideByZero(); return NaN();
		case CombinedType::PositiveNaN: return NaN();
		case CombinedType::PositiveInf: return *this; 
		case CombinedType::PositiveNegInf: return *this;
		case CombinedType::NegativeZero: EventHandler::DivideByZero(); return NaN();
		case CombinedType::NegativeNaN: return NaN();
		case CombinedType::NegativeInf: return *this;
		case CombinedType::NegativeNegInf: return *this;
		case CombinedType::ZeroPositive: return *this;
		case CombinedType::ZeroNegative: return *this;
		case CombinedType::ZeroZero: EventHandler::DivideByZero(); return NaN();
		case CombinedType::ZeroNaN: return NaN();
		case CombinedType::ZeroInf: return *this; 
		case CombinedType::ZeroNegInf: return *this; 
		case CombinedType::NaNPositive: return NaN();
		case CombinedType::NaNNegative: return NaN();
		case CombinedType::NaNZero: EventHandler::DivideByZero(); return NaN();
		case CombinedType::NaNNaN: return NaN();
		case CombinedType::NaNInf: return NaN();
		case CombinedType::NaNNegInf: return NaN();
		case CombinedType::InfPositive: return NaN();
		case CombinedType::InfNegative: return NaN();
		case CombinedType::InfZero: EventHandler::DivideByZero(); return NaN();
		case CombinedType::InfNaN: return NaN();
		case CombinedType::InfInf: return NaN();
		case CombinedType::InfNegInf: return NaN();
		case CombinedType::NegInfPositive: return NaN();
		case CombinedType::NegInfNegative: return NaN();
		case CombinedType::NegInfZero: EventHandler::DivideByZero(); return NaN();
		case CombinedType::NegInfNaN: return NaN();
		case CombinedType::NegInfInf: return NaN();
		case CombinedType::NegInfNegInf: return NaN();
		default: EventHandler::MalformedValue(); return NaN();
		}
	}

	const constexpr FixedPointT& operator+=(const FixedPointT& another) {
		*this = *this + another;
		return *this;
	}

	const constexpr FixedPointT& operator-=(const FixedPointT& another) {
		*this = *this - another;
		return *this;
	}

	const constexpr FixedPointT& operator*=(const FixedPointT& another) {
		*this = *this * another;
		return *this;
	}

	const constexpr FixedPointT& operator/=(const FixedPointT& another) {
		*this = *this / another;
		return *this;
	}

	const constexpr bool operator==(const FixedPointT another) const {
		CombinedType ctype = CombineTypes(type, another.type);

		switch (ctype) {
		case CombinedType::PositivePositive: return value == another.value;
		case CombinedType::PositiveNegative: return false;
		case CombinedType::PositiveZero: return false;
		case CombinedType::PositiveNaN: return false;
		case CombinedType::PositiveInf: return false;
		case CombinedType::PositiveNegInf: return false;
		case CombinedType::NegativePositive: return false;
		case CombinedType::NegativeNegative: return value == another.value;
		case CombinedType::NegativeZero: return false;
		case CombinedType::NegativeNaN: return false;
		case CombinedType::NegativeInf: return false;
		case CombinedType::NegativeNegInf: return false;
		case CombinedType::ZeroPositive: return false;
		case CombinedType::ZeroNegative: return false;
		case CombinedType::ZeroZero: return true;
		case CombinedType::ZeroNaN: return false;
		case CombinedType::ZeroInf: return false;
		case CombinedType::ZeroNegInf: return false;
		case CombinedType::NaNPositive: return false;
		case CombinedType::NaNNegative: return false;
		case CombinedType::NaNZero: return false;
		case CombinedType::NaNNaN: return false;
		case CombinedType::NaNInf: return false;
		case CombinedType::NaNNegInf: return false;
		case CombinedType::InfPositive: return false;
		case CombinedType::InfNegative: return false;
		case CombinedType::InfZero: return false;
		case CombinedType::InfNaN: return false;
		case CombinedType::InfInf: return true;
		case CombinedType::InfNegInf: return false;
		case CombinedType::NegInfPositive: return false;
		case CombinedType::NegInfNegative: return false;
		case CombinedType::NegInfZero: return false;
		case CombinedType::NegInfNaN: return false;
		case CombinedType::NegInfInf: return false;
		case CombinedType::NegInfNegInf: return true;
		default: EventHandler::MalformedValue(); return false;
		}
	}
	
	const constexpr bool operator!=(const FixedPointT another) const {
		CombinedType ctype = CombineTypes(type, another.type);

		switch (ctype) {
		case CombinedType::PositivePositive: return value != another.value;
		case CombinedType::PositiveNegative: return true;
		case CombinedType::PositiveZero: return true;
		case CombinedType::PositiveNaN: return true;
		case CombinedType::PositiveInf: return true;
		case CombinedType::PositiveNegInf: return true;
		case CombinedType::NegativePositive: return true;
		case CombinedType::NegativeNegative: return value != another.value;
		case CombinedType::NegativeZero: return true;
		case CombinedType::NegativeNaN: return true;
		case CombinedType::NegativeInf: return true;
		case CombinedType::NegativeNegInf: return true;
		case CombinedType::ZeroPositive: return true;
		case CombinedType::ZeroNegative: return true;
		case CombinedType::ZeroZero: return false;
		case CombinedType::ZeroNaN: return true;
		case CombinedType::ZeroInf: return true;
		case CombinedType::ZeroNegInf: return true;
		case CombinedType::NaNPositive: return true;
		case CombinedType::NaNNegative: return true;
		case CombinedType::NaNZero: return true;
		case CombinedType::NaNNaN: return true;
		case CombinedType::NaNInf: return true;
		case CombinedType::NaNNegInf: return true;
		case CombinedType::InfPositive: return true;
		case CombinedType::InfNegative: return true;
		case CombinedType::InfZero: return true;
		case CombinedType::InfNaN: return true;
		case CombinedType::InfInf: return false;
		case CombinedType::InfNegInf: return true;
		case CombinedType::NegInfPositive: return true;
		case CombinedType::NegInfNegative: return true;
		case CombinedType::NegInfZero: return true;
		case CombinedType::NegInfNaN: return true;
		case CombinedType::NegInfInf: return true;
		case CombinedType::NegInfNegInf: return false;
		default: EventHandler::MalformedValue(); return false;
		}
	}
	
	const constexpr bool operator>=(const FixedPointT another) const {
		CombinedType ctype = CombineTypes(type, another.type);

		switch (ctype) {
		case CombinedType::PositivePositive: return value >= another.value;
		case CombinedType::PositiveNegative: return true;
		case CombinedType::PositiveZero: return true;
		case CombinedType::PositiveNaN: return false;
		case CombinedType::PositiveInf: return false;
		case CombinedType::PositiveNegInf: return true;
		case CombinedType::NegativePositive: return false;
		case CombinedType::NegativeNegative: return value <= another.value;
		case CombinedType::NegativeZero: return false;
		case CombinedType::NegativeNaN: return false;
		case CombinedType::NegativeInf: return false;
		case CombinedType::NegativeNegInf: return true;
		case CombinedType::ZeroPositive: return false;
		case CombinedType::ZeroNegative: return true;
		case CombinedType::ZeroZero: return true;
		case CombinedType::ZeroNaN: return false;
		case CombinedType::ZeroInf: return false;
		case CombinedType::ZeroNegInf: return true;
		case CombinedType::NaNPositive: return false;
		case CombinedType::NaNNegative: return false;
		case CombinedType::NaNZero: return false;
		case CombinedType::NaNNaN: return false;
		case CombinedType::NaNInf: return false;
		case CombinedType::NaNNegInf: return false;
		case CombinedType::InfPositive: return true;
		case CombinedType::InfNegative: return true;
		case CombinedType::InfZero: return true;
		case CombinedType::InfNaN: return false;
		case CombinedType::InfInf: return true;
		case CombinedType::InfNegInf: return true;
		case CombinedType::NegInfPositive: return false;
		case CombinedType::NegInfNegative: return false;
		case CombinedType::NegInfZero: return false;
		case CombinedType::NegInfNaN: return false;
		case CombinedType::NegInfInf: return false;
		case CombinedType::NegInfNegInf: return true;
		default: EventHandler::MalformedValue(); return false;
		}
	}
	
	const constexpr bool operator>(const FixedPointT another) const {
		CombinedType ctype = CombineTypes(type, another.type);

		switch (ctype) {
		case CombinedType::PositivePositive: return value > another.value;
		case CombinedType::PositiveNegative: return true;
		case CombinedType::PositiveZero: return true;
		case CombinedType::PositiveNaN: return false;
		case CombinedType::PositiveInf: return false;
		case CombinedType::PositiveNegInf: return true;
		case CombinedType::NegativePositive: return false;
		case CombinedType::NegativeNegative: return value < another.value;
		case CombinedType::NegativeZero: return false;
		case CombinedType::NegativeNaN: return false;
		case CombinedType::NegativeInf: return false;
		case CombinedType::NegativeNegInf: return true;
		case CombinedType::ZeroPositive: return false;
		case CombinedType::ZeroNegative: return true;
		case CombinedType::ZeroZero: return false;
		case CombinedType::ZeroNaN: return false;
		case CombinedType::ZeroInf: return false;
		case CombinedType::ZeroNegInf: return true;
		case CombinedType::NaNPositive: return false;
		case CombinedType::NaNNegative: return false;
		case CombinedType::NaNZero: return false;
		case CombinedType::NaNNaN: return false;
		case CombinedType::NaNInf: return false;
		case CombinedType::NaNNegInf: return false;
		case CombinedType::InfPositive: return true;
		case CombinedType::InfNegative: return true;
		case CombinedType::InfZero: return true;
		case CombinedType::InfNaN: return false;
		case CombinedType::InfInf: return false;
		case CombinedType::InfNegInf: return true;
		case CombinedType::NegInfPositive: return false;
		case CombinedType::NegInfNegative: return false;
		case CombinedType::NegInfZero: return false;
		case CombinedType::NegInfNaN: return false;
		case CombinedType::NegInfInf: return false;
		case CombinedType::NegInfNegInf: return false;
		default: EventHandler::MalformedValue(); return false;
		}
	}
	
	const constexpr bool operator<=(const FixedPointT another) const {
		CombinedType ctype = CombineTypes(type, another.type);

		switch (ctype) {
		case CombinedType::PositivePositive: return value <= another.value;
		case CombinedType::PositiveNegative: return false;
		case CombinedType::PositiveZero: return false;
		case CombinedType::PositiveNaN: return false;
		case CombinedType::PositiveInf: return true;
		case CombinedType::PositiveNegInf: return false;
		case CombinedType::NegativePositive: return true;
		case CombinedType::NegativeNegative: return value >= another.value;
		case CombinedType::NegativeZero: return true;
		case CombinedType::NegativeNaN: return false;
		case CombinedType::NegativeInf: return true;
		case CombinedType::NegativeNegInf: return false;
		case CombinedType::ZeroPositive: return true;
		case CombinedType::ZeroNegative: return false;
		case CombinedType::ZeroZero: return true;
		case CombinedType::ZeroNaN: return false;
		case CombinedType::ZeroInf: return true;
		case CombinedType::ZeroNegInf: return false;
		case CombinedType::NaNPositive: return false;
		case CombinedType::NaNNegative: return false;
		case CombinedType::NaNZero: return false;
		case CombinedType::NaNNaN: return false;
		case CombinedType::NaNInf: return false;
		case CombinedType::NaNNegInf: return false;
		case CombinedType::InfPositive: return false;
		case CombinedType::InfNegative: return false;
		case CombinedType::InfZero: return false;
		case CombinedType::InfNaN: return false;
		case CombinedType::InfInf: return true;
		case CombinedType::InfNegInf: return false;
		case CombinedType::NegInfPositive: return true;
		case CombinedType::NegInfNegative: return true;
		case CombinedType::NegInfZero: return true;
		case CombinedType::NegInfNaN: return false;
		case CombinedType::NegInfInf: return true;
		case CombinedType::NegInfNegInf: return true;
		default: EventHandler::MalformedValue(); return false;
		}
	}
	
	const constexpr bool operator<(const FixedPointT another) const {
		CombinedType ctype = CombineTypes(type, another.type);

		switch (ctype) {
		case CombinedType::PositivePositive: return value < another.value;
		case CombinedType::PositiveNegative: return false;
		case CombinedType::PositiveZero: return false;
		case CombinedType::PositiveNaN: return false;
		case CombinedType::PositiveInf: return true;
		case CombinedType::PositiveNegInf: return false;
		case CombinedType::NegativePositive: return true;
		case CombinedType::NegativeNegative: return value > another.value;
		case CombinedType::NegativeZero: return true;
		case CombinedType::NegativeNaN: return false;
		case CombinedType::NegativeInf: return true;
		case CombinedType::NegativeNegInf: return false;
		case CombinedType::ZeroPositive: return true;
		case CombinedType::ZeroNegative: return false;
		case CombinedType::ZeroZero: return false;
		case CombinedType::ZeroNaN: return false;
		case CombinedType::ZeroInf: return true;
		case CombinedType::ZeroNegInf: return false;
		case CombinedType::NaNPositive: return false;
		case CombinedType::NaNNegative: return false;
		case CombinedType::NaNZero: return false;
		case CombinedType::NaNNaN: return false;
		case CombinedType::NaNInf: return false;
		case CombinedType::NaNNegInf: return false;
		case CombinedType::InfPositive: return false;
		case CombinedType::InfNegative: return false;
		case CombinedType::InfZero: return false;
		case CombinedType::InfNaN: return false;
		case CombinedType::InfInf: return false;
		case CombinedType::InfNegInf: return false;
		case CombinedType::NegInfPositive: return true;
		case CombinedType::NegInfNegative: return true;
		case CombinedType::NegInfZero: return true;
		case CombinedType::NegInfNaN: return false;
		case CombinedType::NegInfInf: return true;
		case CombinedType::NegInfNegInf: return false;
		default: EventHandler::MalformedValue(); return false;
		}
	}

	const constexpr bool IsFinite() const {
		return type != Type::Inf && type != Type::NegInf && type != Type::NaN;
	}

	const constexpr bool IsInf() const {
		return type == Type::Inf;
	}
	
	const constexpr bool IsNegativeInf() const {
		return type == Type::Inf;
	}
	
	const constexpr bool IsNaN() const {
		return type == Type::NaN;
	}
	
	const constexpr FixedPointT Floor() {
		switch (type) {
		case Type::Positive:
#ifdef USE_FPM
			return FixedPointT(negative, fpm::floor(value));
#else
			return FixedPointT(type, value & 0xFFFFFFFF00000000);
#endif
		case Type::Negative:
		{
			FixedPointT tmp = -(*this);
			return -(tmp.Ceil());
		}
		case Type::Zero:
		case Type::NaN:
		case Type::Inf:
		case Type::NegInf:
			return *this;

		default: EventHandler::MalformedValue(); return NaN();
		}
	}
	
	const constexpr FixedPointT Ceil() {
		switch (type) {
		case Type::Positive:
		{
#ifdef USE_FPM
			return FixedPoint(negative, fpm::floor(value));
#else
			FPUint64 tmp = value & 0x00000000FFFFFFFF;
			FPUint64 res = value - tmp;
			res += tmp > 0 ? FPUint64(1) << 32 : 0;
			return FixedPointT(type, res);
#endif
		}
		case Type::Negative:
		{
			FixedPointT tmp = -(*this);
			return -(tmp.Floor());
		}

		case Type::Zero:
		case Type::NaN:
		case Type::Inf:
		case Type::NegInf:
			return *this;

		default: EventHandler::MalformedValue(); return NaN();
		}
	}

	const constexpr FixedPointT Abs() {
		switch (type) {
		case Type::Negative:
			return FixedPointT(Type::Positive, value);

		case Type::NegInf:
			return Inf();

		case Type::Positive:
		case Type::Zero:
		case Type::NaN:
		case Type::Inf:
			return *this;

		default: EventHandler::MalformedValue(); return NaN();
		}
	}

	const constexpr bool AboveZero() const {
		return *this > ZERO;
	}
	
	const constexpr bool BelowZero() const {
		return *this < ZERO;
	}

	const constexpr bool IsZero() const {
		return type == Type::Zero;
	}

	static constexpr FixedPointT cos(FixedPointT fp) {
		switch (fp.type) {

		case Type::Positive:
			return sin(fp + HALF_PI());

		case Type::Negative:
			return cos(-fp);

		case Type::Zero:
			return ONE();

		case Type::NaN:
			return fp;

		case Type::Inf:
		case Type::NegInf:
			return NaN();

		default: EventHandler::MalformedValue(); return NaN();
		}
	}
	
	static constexpr FixedPointT sin(FixedPointT fp) {
		switch (fp.type) {

		case Type::Positive:
			if (fp >= TWO_PI()) {
				return sin(fp % (TWO_PI()));
			} else  if (fp < HALF_PI()) {
#ifdef USE_FPM
				return FixedPoint(false, fpm::sin(tmp.value));
#else
				return tableValue(fp, sinData, sinDataFirst(), sinDataLast());
#endif
			} else if (fp < PI()) {
				return sin(PI() - fp);
			} else if (fp == PI()) {
				return ZERO();
			} else { // tmp < F_2PI
				return -sin(fp - PI());
			}

		case Type::Negative:
			return -sin(-fp);

		case Type::Zero:
			return ZERO();

		case Type::NaN:
			return fp;

		case Type::Inf:
		case Type::NegInf:
			return NaN();

		default: EventHandler::MalformedValue(); return NaN();
		}

	}
	
	static constexpr FixedPointT asin(FixedPointT fp) {
		switch (fp.type) {

		case Type::Positive:
			if (fp > ONE()) {
				return NaN();
			}
			return tableValue(fp, asinData, asinDataFirst(), asinDataLast());
		case Type::Negative:
			return -sin(-fp);

		case Type::Zero:
			return ZERO();

		case Type::NaN:
			return fp;

		case Type::Inf:
		case Type::NegInf:
			return NaN();

		default: EventHandler::MalformedValue(); return NaN();
		}

	}
	
	static constexpr FixedPointT atan(FixedPointT fp) {
		switch (fp.type) {

		case Type::Positive:
		{
			FixedPointT fpfp = fp * fp;
			return asin(fp / sqrt(fpfp + ONE()));
		}
		case Type::Negative:
			return -sin(-fp);

		case Type::Zero:
			return ZERO();

		case Type::NaN:
			return fp;

		case Type::Inf:
			return FixedPointEncodedValue<0x01921FB544>();

		case Type::NegInf:
			return FixedPointEncodedValue<0x01921FB544, true>();

		default: EventHandler::MalformedValue(); return NaN();
		}

	}
	
	static constexpr FixedPointT sqrt(FixedPointT fp) {
		switch (fp.type) {

		case Type::Positive:
		{
#ifdef USE_FPM
			return FixedPoint(false, fpm::sqrt(fp.value));
#else

			if (SqrtIterationsCount > 0) {
				if (fp < sqrtDataLast()) {
					FixedPointT F_TWO = FixedPointValue<2, 0>();

					FixedPointT x1;
					FixedPointT x2;
					FixedPointT mid = tableValue(fp, sqrtData, sqrtDataFirst(), sqrtDataLast(), x1, x2);
					if (mid == x1 || mid == x2) {
						return mid;
					}

					for (FPUint32 i = 0; x2 > x1 && i < SqrtIterationsCount; i++) {
						mid = x1 + ((x2 - x1) / F_TWO);
						FixedPointT mid2 = mid * mid;
						if (mid2 > fp && x2 != mid) {
							x2 = mid;
						} else if (mid2 < fp && x1 != mid) {
							x1 = mid;
						} else {
							break;
						}
					}
					return mid;
				}
			} else {

			}

			// Finding the square root of an integer in base-2, from:
			// https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29

			// Shift by F first because it's fixed-point.
			FPUint128 num = FPUint128(fp.value) << FPUint128(32);
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
			return FixedPointT(Type::Positive, FPT(res));
#endif
		}

		case Type::Negative:
			EventHandler::NegativeSqrt();
			return NaN();

		case Type::Zero:
			return ZERO();

		case Type::NaN:
			return fp;

		case Type::Inf:
			return fp;

		case Type::NegInf:
			return NaN();

		default: EventHandler::MalformedValue(); return NaN();
		}
	}
	
	static constexpr FixedPointT fmod(FixedPointT fp, FixedPointT mod) {
		return fp % mod;
	}
	
	static constexpr FixedPointT exp(FixedPointT fp) {
		switch (fp.type) {

		case Type::Positive:
		{
#ifdef USE_FPM
			return FixedPoint(false, fpm::exp(fp.value));
#else
			FixedPointT term = ONE();
			FixedPointT result = ONE();
			for (FPUint32 i = 1; i < 5; i++) {
				FixedPointT order = FixedPointT(i);
				term *= fp / order;
				result += term;
			}
			return result;
#endif
		}

		case Type::Negative:
			// TODO: This is not a zero
			return ZERO();

		case Type::Zero:
			return ONE();

		case Type::NaN:
			return fp;

		case Type::Inf:
			return fp;

		case Type::NegInf:
			return ZERO();

		default: EventHandler::MalformedValue(); return NaN();
		}

	}
	
	static constexpr FixedPointT floor(FixedPointT fp) {
		return fp.Floor();
	}

	static constexpr FixedPointT atan2(FixedPointT fp, FixedPointT fp2) {
		static_assert(Atan2UsingAtan, "Atan2 without atan is not implemented");

		CombinedType ctype = CombineTypes(fp.type, fp2.type);

		switch (ctype) {
		case CombinedType::PositivePositive: return atan(fp / fp2);
		case CombinedType::PositiveNegative: return atan(fp / fp2) + PI();
		case CombinedType::PositiveZero: return HALF_PI();
		case CombinedType::PositiveNaN: return NaN();
		case CombinedType::PositiveInf: return ZERO();
		case CombinedType::PositiveNegInf: return PI();
		case CombinedType::NegativePositive: return atan(fp / fp2);
		case CombinedType::NegativeNegative: return atan(fp / fp2) - PI();
		case CombinedType::NegativeZero: return -HALF_PI();
		case CombinedType::NegativeNaN: return NaN();
		case CombinedType::NegativeInf: return ZERO();
		case CombinedType::NegativeNegInf: return -PI();
		case CombinedType::ZeroPositive: return fp; // 0
		case CombinedType::ZeroNegative: return PI();
		case CombinedType::ZeroZero: return fp;
		case CombinedType::ZeroNaN: return NaN();
		case CombinedType::ZeroInf: return fp; // 0
		case CombinedType::ZeroNegInf: return PI();
		case CombinedType::NaNPositive: return NaN();
		case CombinedType::NaNNegative: return NaN();
		case CombinedType::NaNZero: return NaN();
		case CombinedType::NaNNaN: return NaN();
		case CombinedType::NaNInf: return NaN();
		case CombinedType::NaNNegInf: return NaN();
		case CombinedType::InfPositive: return FixedPointEncodedValue<0x00000001921FB544>();
		case CombinedType::InfNegative: return FixedPointEncodedValue< 0x00000001921FB544>();
		case CombinedType::InfZero: return FixedPointEncodedValue<0x00000001921FB544>();
		case CombinedType::InfNaN: return NaN();
		case CombinedType::InfInf: return FixedPointEncodedValue<0x00000000C90FDAA3>();
		case CombinedType::InfNegInf: return FixedPointEncodedValue<0x000000025B2F8FE4>();
		case CombinedType::NegInfPositive: return FixedPointEncodedValue<0x00000001921FB544, true>();
		case CombinedType::NegInfNegative: return FixedPointEncodedValue<0x00000001921FB544, true>();
		case CombinedType::NegInfZero: return FixedPointEncodedValue<0x00000001921FB544, true>();
		case CombinedType::NegInfNaN: return NaN();
		case CombinedType::NegInfInf: return FixedPointEncodedValue<0x00000000C90FDAA3, true>();
		case CombinedType::NegInfNegInf: return FixedPointEncodedValue<0x000000025B2F8FE4, true>();
		}


		//if (fp.inf || fp2.inf) {
		//	return Inf();
		//}
		//FixedPoint r;
		//FixedPoint angle;
		//FixedPoint abs_y = fp.Abs() + F_VALUE(0, 1000000);
		//if (fp2.BelowZero()) {
		//	r = (fp2 + abs_y) / (abs_y - fp2);
		//	const FixedPoint THRQTR_PI = F_THREE * F_PI / F_FOUR;
		//	angle = THRQTR_PI;
		//} else {
		//	r = (fp2 - abs_y) / (fp2 + abs_y);
		//	const FixedPoint ONEQTR_PI = F_PI / F_FOUR;
		//	angle = ONEQTR_PI;
		//}
		//angle += (F_VALUE(0, 3691) * r * r - F_VALUE(0, 7189)) * r;
		//if (fp.BelowZero()) {
		//	return -angle;
		//} else {
		//	return angle;
		//}
		return ZERO();
	}
	
	static constexpr bool isfinite(FixedPointT fp) {
		return fp.IsFinite();
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

	void constexpr PrintValue(char* buffer, const FPUint32 bufferSize_) const {
		using FixedPoint = FixedPointT;

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

		if (type == Type::Negative || type == Type::NegInf) {
			Append(tmpI, '-');
		}
		if (type == Type::NegInf || type == Type::Inf) {
			Append(tmpI, 'I');
			Append(tmpI, 'n');
			Append(tmpI, 'f');
			return;
		} else if (type == Type::NaN) {
			Append(tmpI, 'N');
			Append(tmpI, 'a');
			Append(tmpI, 'N');
			return;
		}

		char(*GetDigit)(FixedPoint&) = [](FixedPoint& fp) {
			if (fp >= FixedPoint::FixedPointValue<9, 0>()) {
				return '9';
			} else if (fp >= FixedPoint::FixedPointValue<8, 0>()) {
				return '8';
			} else if (fp >= FixedPoint::FixedPointValue<7, 0>()) {
				return '7';
			} else if (fp >= FixedPoint::FixedPointValue<6, 0>()) {
				return '6';
			} else if (fp >= FixedPoint::FixedPointValue<5, 0>()) {
				return '5';
			} else if (fp >= FixedPoint::FixedPointValue<4, 0>()) {
				return '4';
			} else if (fp >= FixedPoint::FixedPointValue<3, 0>()) {
				return '3';
			} else if (fp >= FixedPoint::FixedPointValue<2, 0>()) {
				return '2';
			} else if (fp >= FixedPoint::FixedPointValue<1, 0>()) {
				return '1';
			} else {
				return '0';
			}
		};

		FixedPointT fp = *this;
		fp = fp.Abs();

		FixedPointT high = floor(fp);
		FixedPointT low = (fp)-high;

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
			low *= TEN();
			FixedPointT tmp = floor(low);
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

	static constexpr const FixedPointT ZERO() { return FixedPointValue<0, 0>(); }
	static constexpr const FixedPointT ONE() { return FixedPointValue<1, 0>(); }
	static constexpr const FixedPointT TWO() { return FixedPointValue<2, 0>(); }
	static constexpr const FixedPointT THREE() { return FixedPointValue<3, 0>(); }
	static constexpr const FixedPointT FOUR() { return FixedPointValue<4, 0>(); }
	static constexpr const FixedPointT FIVE() { return FixedPointValue<5, 0>(); }
	static constexpr const FixedPointT SIX() { return FixedPointValue<6, 0>(); }
	static constexpr const FixedPointT SEVEN() { return FixedPointValue<7, 0>(); }
	static constexpr const FixedPointT EIGHT() { return FixedPointValue<8, 0>(); }
	static constexpr const FixedPointT NINE() { return FixedPointValue<9, 0>(); }
	static constexpr const FixedPointT TEN() { return FixedPointValue<10, 0>(); }
	static constexpr const FixedPointT PI() { return FixedPointEncodedValue<0x00000003243F6A85>(); }
	static constexpr const FixedPointT TWO_PI() { return FixedPointEncodedValue<0x00000006487ED50A>(); }
	static constexpr const FixedPointT HALF_PI() { return FixedPointEncodedValue<0x00000001921FB544>(); }
	static constexpr const FixedPointT E() { return FixedPointValue<1, 28182817>(); }
	
	static constexpr const FixedPointT EPSILON() { return FixedPointEncodedValue<1>(); }
#pragma push_macro("MIN")
#ifdef MIN
#undef MIN
#endif
	static constexpr const FixedPointT MIN() { return FixedPointEncodedValue<0xFFFFFFFFFFFFFFFF, true>(); }
	static constexpr const FixedPointT MIN_() { return FixedPointEncodedValue<0xFFFFFFFFFFFFFFFF, true>(); }
#pragma pop_macro("MIN")

#pragma push_macro("MAX")
#ifdef MAX
#undef MAX
#endif
	static constexpr const FixedPointT MAX() { return FixedPointEncodedValue<0xFFFFFFFFFFFFFFFF>(); }
	static constexpr const FixedPointT MAX_() { return FixedPointEncodedValue<0xFFFFFFFFFFFFFFFF>(); }
#pragma pop_macro("MAX")

private:

	// functions for internal use
	template<size_t N>
	static constexpr FixedPointT tableValue(FixedPointT fp, const FPUint64 (&table)[N], FixedPointT first, FixedPointT last) {
		FixedPointT x1;
		FixedPointT x2;
		return tableValue(fp, table, first, last, x1, x2);
	}

	template<size_t N>
	static constexpr FixedPointT tableValue(FixedPointT fp, const FPUint64(&table)[N], FixedPointT first, FixedPointT last, FixedPointT& x1, FixedPointT& x2) {
		constexpr const FPUint32 sz = N - 1;
		const FixedPointT diff = (last - first) / FixedPointValue<sz, 0>();
		bool negative = false;
		FPUint32 dataIndex = (fp / diff).GetWholeValue(negative);
		FixedPointT sdi(dataIndex);
		x1 = FixedPointT(table[dataIndex], false, false, false);
		x2 = FixedPointT(table[dataIndex + 1], false, false, false);
		FixedPointT x2x1 = x2 - x1;
		return x1 + (fp * (x2x1 / diff)) - (sdi * x2x1);
	}

private:
	Type type;
	FPT value;

};

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

#define float FixedPointT<FP_DEF>
#define double error type

inline float b2Abs(float a) {
	return a.Abs();
}

#define cosf DefaultFixedPoint::cos
#define sinf DefaultFixedPoint::sin
#define sqrtf DefaultFixedPoint::sqrt
#define expf DefaultFixedPoint::exp
#define floorf DefaultFixedPoint::floor
#define atan2f DefaultFixedPoint::atan2
#define isfinite DefaultFixedPoint::isfinite

typedef FixedPointT<FP_DEF> DefaultFixedPoint;

template<const FPUint32 left, const FPUint32 right=0, const bool negative = false>
static constexpr const DefaultFixedPoint(*DefaultFixedPointValue)() = &DefaultFixedPoint::FixedPointValue<left, right, negative>;

template<const FPInt32 nominator, const FPInt32 denominator>
static constexpr const DefaultFixedPoint(*DefaultFixedPointFractionValue)() = &DefaultFixedPoint::FixedPointFractionValue<nominator, denominator>;

#else
// No fixed point
#pragma push_macro("F_CONST")
#ifdef F_CONST
#undef F_CONST
#endif
#define F_CONST(left, num, len, original) original
#endif

#ifdef BOX2D_NO_TIMER
class b2Timer {

public:

	b2Timer() {

	}

	const constexpr float GetMilliseconds() {
#ifdef BOX2D_USE_FIXED_POINT
		return (const float)DefaultFixedPointValue<0, 0>();
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
#pragma pop_macro("F_CONST")
#pragma pop_macro("FP_EXP")
#pragma pop_macro("FP_DEF")
#pragma pop_macro("FP_DEC")
#else
#pragma pop_macro("F_CONST")
#endif

// INCLUDE_POST_IMPL_END

#ifdef BOX2D_USE_FIXED_POINT
class FixedPointTests {

	using FixedPoint = FixedPointT<FP_TEST>;
	
	template<const FPUint32 left, const FPUint32 right, const bool tnegative = false>
	static constexpr const FixedPoint(*FixedPointValue)() = &FixedPoint::FixedPointValue<left, right, tnegative>;
	
	template<const FPInt32 nominator, const FPInt32 denominator>
	static constexpr const FixedPoint(*FixedPointFractionValue)() = &FixedPoint::FixedPointFractionValue<nominator, denominator>;

public:

	static void Run() {
		RunConstructors();
		RunTestFromDouble();
		RunTestArithmetics();

		RunTestSqrt();
		RunTestSin();
		RunTestCos();
		RunTestAtan();
		RunTestAtan2();
		RunTestExp();
	}

protected:

	static bool RunConstructors() {
		bool error = false;
		
		ClearErrors();
		Equals(FixedPoint(0), "0.0", error);
		Equals(FixedPoint(1), "1.0", error);
		Equals(FixedPoint(-1), "-1.0", error);
		Equals(FixedPoint(100), "100.0", error);
		Equals(FixedPoint(32767), "32767.0", error);
		Equals(FixedPoint(-1), "-1.0", error);
		
		Equals(FixedPointValue<0, 0>(), "0.0", error);
		Equals(FixedPointValue<1, 0>(), "1.0", error);
		Equals(FixedPointValue<0, 5>(), "0.5", error);
		Equals(FixedPointValue<2, 5>(), "2.5", error);
		Equals(FixedPointValue<2, 5, true>(), "-2.5", error);
		
		Equals(FixedPointValue<0, 0, true>(), "0.0", error);
		Equals(FixedPointValue<3, 521>(), "3.125", error);
		Equals(FixedPointValue<3, 521, true>(), "-3.125", error);
		
		Equals(FixedPointFractionValue<0, 1>(), "0.0", error);
		Equals(FixedPointFractionValue<1, 1>(), "1.0", error);
		Equals(FixedPointFractionValue<1, 2>(), "0.5", error);
		Equals(FixedPointFractionValue<-1, 2>(), "-0.5", error);

		if (FixedPoint::RoundDivided) {
			Equals(FixedPointFractionValue<1, 10>(), "0.1000000000931322574615478515625", error);
			Equals(FixedPointFractionValue<1, 100>(), "0.01000000000931322574615478515625", error);
			Equals(FixedPointFractionValue<1, 1000>(), "0.00099999993108212947845458984375", error);
			Equals(FixedPointFractionValue<1, 10000>(), "0.00010000006295740604400634765625", error);
			Equals(FixedPointFractionValue<1, 100000>(), "0.0000100000761449337005615234375", error);
		} else {
			Equals(FixedPointFractionValue<1, 10>(), "0.09999999986030161380767822265625", error);
			Equals(FixedPointFractionValue<1, 100>(), "0.00999999977648258209228515625", error);
			Equals(FixedPointFractionValue<1, 1000>(), "0.00099999993108212947845458984375", error);
			Equals(FixedPointFractionValue<1, 10000>(), "0.00009999983012676239013671875", error);
			Equals(FixedPointFractionValue<1, 100000>(), "0.00000999984331429004669189453125", error);
		}

		return !error;
	}

	static bool RunTestFromDouble() {
		bool error = false;
		
		ClearErrors();
		double precission = 0.0001;
		Equals(3.141529, precission, error);
		Equals(0, 0, error);
		Equals(1, 0, error);
		Equals(-1, 0, error);
		Equals(-3.141529, precission, error);
		Equals(-999, 0, error);
		Equals(365, 0, error);
		
		return !error;
	}

	static bool RunTestArithmetics() {
		bool error = false;
		
		ClearErrors();
		Equals(FixedPointValue<1, 0>() + FixedPointValue<1, 0>(), FixedPointValue<2, 0>(), error);
		Equals(FixedPointValue<1, 0>() + FixedPointValue<1, 0, true>(), FixedPointValue<0, 0>(), error);
		Equals(FixedPointValue<1, 0, true>() + FixedPointValue<1, 0>(), FixedPointValue<0, 0>(), error);
		Equals(FixedPointValue<1, 0, true>() + FixedPointValue<1, 0, true>(), FixedPointValue<2, 0, true>(), error);

		Equals(FixedPointValue<1, 0>() - FixedPointValue<1, 0>(), FixedPointValue<0, 0>(), error);
		Equals(FixedPointValue<1, 0>() - FixedPointValue<1, 0, true>(), FixedPointValue<2, 0>(), error);
		
		Equals(FixedPointValue<1, 0, true>() - FixedPointValue<1, 0>(), FixedPointValue<2, 0, true>(), error);
		Equals(FixedPointValue<1, 0, true>() - FixedPointValue<1, 0, true>(), FixedPointValue<0, 0>(), error);
		
		Equals(FixedPointFractionValue<375, 100>() + FixedPointFractionValue<585, 10>(), FixedPointValue<62, 52>(), error);
		
		ClearErrors();
		FixedPointValue<10, 0>() / FixedPointValue<0, 0>();
		if (!hadDivisionByZero) {
			error = true;
			CheckError(error);
		}
		ClearErrors();
		
		return !error;
	}

	static bool RunTestSqrt() {
		bool error = false;
		ClearErrors();


		return !error;
	}
	
	static bool RunTestSin() {
		bool error = false;


		return !error;
	}
	
	static bool RunTestCos() {
		bool error = false;


		return !error;
	}
	
	static bool RunTestAtan() {
		bool error = false;


		return !error;
	}
	
	static bool RunTestAtan2() {
		bool error = false;


		return !error;
	}
	
	static bool RunTestExp() {
		bool error = false;


		return !error;
	}

private:

	static void CheckError(bool& error) {
		if (!error) {
			if (hadError) {
				error = true;
			}
		}
		if (error) {
			printf("Has error\n");
		}
	}

	static void Equals(double d, double precission, bool& error) {
		Equals(d, FromDouble(d, error), precission, error);
	}

	static void Equals(double d, FixedPoint f, double precission, bool& error) {
		if (!error) {
			double c = FromFixedPoint(f);
			double er = c - d;
			if (er < 0) {
				er = -er;
			}
			error |= er > precission;
			CheckError(error);
		}
	}

	static void Equals(FixedPoint op1, FixedPoint op2, bool& error) {
		if (!error) {
			error |= op1 != op2;
			CheckError(error);
		}
	}

	static double FromFixedPoint(FixedPoint fp) {
		if (fp.IsNegativeInf()) {
			return -INFINITY;
		} else if (fp.IsInf()) {
			return INFINITY;
		} else if (fp.IsNaN()) {
			return NAN;
		}
		char tmp[64];
		fp.PrintValue(tmp, sizeof(tmp));
		return atof(tmp);
	}

	static FixedPoint FromDouble(double d, bool& error) {
		if (!isfinite(d)) {
			return d == -INFINITY ? FixedPoint::NegativeInf() : FixedPoint::Inf();
		} else if (d > (double)0xFFFFFFFF || d < -(double)0xFFFFFFFF) {
			error = true;
			CheckError(error);
			return FixedPoint::Inf();
		}
		int32 w = (int32)floor(d);
		d -= floor(d);
		if (d < 0) {
			d = -d;
		}
		FixedPoint fw(w);
		int32 dec = (int32)(floor(d * 1000000));
		FixedPoint dc(dec);
		FixedPoint tmp = FixedPointValue<1000000, 1>();
		dc /= tmp;
		return fw + dc;
	}

	static void Equals(FixedPoint fp, const char* result, bool& error) {
		if (!error) {
			char tmp[64];
			fp.PrintValue(tmp, sizeof(tmp));
			if (strcmp(tmp, result)) {
				error = true;
				CheckError(error);
			}
		}
	}

// Event handlers

	static bool hadError;
	static bool hadOverflow;
	static bool hadMalformedValue;
	static bool hadDivisionByZero;
	static bool hadNegativeSqrt;
	static bool hadUndefinedAtan1;

	static void ClearErrors() {
		hadError = false;
		hadOverflow = false;
		hadMalformedValue = false;
		hadDivisionByZero = false;
		hadNegativeSqrt = false;
		hadUndefinedAtan1 = false;
	}

public:

	static void Overflow() {
		hadError = true;
		hadOverflow = true;
	}

	static void MalformedValue() {
		hadError = true;
		hadMalformedValue = true;
	}

	static void DivideByZero() {
		hadError = true;
		hadDivisionByZero = true;
	}

	static void NegativeSqrt() {
		hadError = true;
		hadNegativeSqrt = true;
	}

	static void UndefinedAtan2() {
		hadError = true;
		hadUndefinedAtan1 = true;
	}
};

#pragma pop_macro("FP_TEST")

#ifdef BOX2D_IMPL
bool FixedPointTests::hadError = false;
bool FixedPointTests::hadOverflow = false;
bool FixedPointTests::hadMalformedValue = false;
bool FixedPointTests::hadDivisionByZero = false;
bool FixedPointTests::hadNegativeSqrt = false;
bool FixedPointTests::hadUndefinedAtan1 = false;
#endif
#endif

#endif
