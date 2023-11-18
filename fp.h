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
typedef uint16_t FPUint16;
typedef uint32_t FPUint32;
typedef int32_t FPInt32;
typedef uint64_t FPUint64;


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
	const FPUint32 ExpIterationsCount, /* Number of iterations in exp. The more the slower/more precise results. Set to 0 to enable table lookup instead */ \
	class EventHandler, /* Class for handling erroneous situations */ \
	const bool Atan2UsingAtan  /* Calculate atan2 using atan function */

#define FP_DEF \
	/* RoundDivided */ true, \
	/* SqrtIterationsCount */ 16, \
	/* ExpIterationsCount */ 0, \
	/* EventHandler */ Numero::FixedPointSegfaultEventHandler, \
	/* Atan2UsingAtan */ true

#define FP_TEST /* Test suite  */ true, 32, 0, FixedPointTests, true
#define FP_EXP /* To expose template arguments in FixedPointT class */  \
	public: static constexpr const bool RoundDivided = RoundDivided;\
	private: static constexpr const FPUint32 SqrtIterationsCount = SqrtIterationsCount;\
	private: static constexpr const FPUint32 ExpIterationsCount = ExpIterationsCount;

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

// Parameters of FixedPoint type
#define FP_DEC \
	const bool RoundDivided,  /* Round last bit in division operations */ \
	const FPUint32 SqrtIterationsCount, /* Number of iterations in sqrt. The more the slower/more precise results */ \
	const FPUint32 ExpIterationsCount, /* Number of iterations in exp. The more the slower/more precise results. Set to 0 to enable table lookup instead */ \
	class EventHandler, /* Class for handling erroneous situations */ \
	const bool Atan2UsingAtan  /* Calculate atan2 using atan function */

#define FP_EXP /* To expose template arguments in FixedPointT class */  \
	public: static constexpr const bool RoundDivided = RoundDivided;\
	private: static constexpr const FPUint32 SqrtIterationsCount = SqrtIterationsCount;\
	private: static constexpr const FPUint32 ExpIterationsCount = ExpIterationsCount;

#include <limits>
#include <string>

namespace Numero {

	class EventHandlerNoEvents {

	public:

		static void Overflow() {}

		static void Underflow() {}

		static void MalformedValue() {}

		static void DivideByZero() {}

		static const void NegativeSqrt() {}

		static const void UndefinedAtan2() {}

		static constexpr const bool ThrowAdditionOverflow = false;

		static constexpr const bool ThrowSubtractionOverflow = false;

		static constexpr const bool ThrowMultiplicationOverflow = false;
	};

	template<const bool CanOverflowBaseType, typename ShiftValueType, typename BaseType, typename EventHandler = EventHandlerNoEvents>
	class CompositeUintT {

		struct TTypeN {

			enum TRelation {
				Lesser = 0,
				Equals = 1,
				Greater = 2
			};

			enum TRelationCombined {
				LesserLesser = (TRelation::Lesser << 2) | TRelation::Lesser,
				EqualsLesser = (TRelation::Equals << 2) | TRelation::Lesser,
				GreaterLesser = (TRelation::Greater << 2) | TRelation::Lesser,

				LesserEquals = (TRelation::Lesser << 2) | TRelation::Equals,
				EqualsEquals = (TRelation::Equals << 2) | TRelation::Equals,
				GreaterEquals = (TRelation::Greater << 2) | TRelation::Equals,

				LesserGreater = (TRelation::Lesser << 2) | TRelation::Greater,
				EqualsGreater = (TRelation::Equals << 2) | TRelation::Greater,
				GreaterGreater = (TRelation::Greater << 2) | TRelation::Greater
			};

			enum TZero {
				Zero = 0,
				Positive = 1
			};

			enum TZeroCombined {
				ZeroZero = (TZero::Zero << 1) | TZero::Zero,
				ZeroPositive = (TZero::Zero << 1) | TZero::Positive,
				PositiveZero = (TZero::Positive << 1) | TZero::Zero,
				PositivePositive = (TZero::Positive << 1) | TZero::Positive
			};

			enum TZeroCombined2 {
				ZeroZeroZeroZero = (TZeroCombined::ZeroZero << 2) | TZeroCombined::ZeroZero,
				ZeroZeroZeroPositive = (TZeroCombined::ZeroZero << 2) | TZeroCombined::ZeroPositive,
				ZeroZeroPositiveZero = (TZeroCombined::ZeroZero << 2) | TZeroCombined::PositiveZero,
				ZeroZeroPositivePositive = (TZeroCombined::ZeroZero << 2) | TZeroCombined::PositivePositive,

				ZeroPositiveZeroZero = (TZeroCombined::ZeroPositive << 2) | TZeroCombined::ZeroZero,
				ZeroPositiveZeroPositive = (TZeroCombined::ZeroPositive << 2) | TZeroCombined::ZeroPositive,
				ZeroPositivePositiveZero = (TZeroCombined::ZeroPositive << 2) | TZeroCombined::PositiveZero,
				ZeroPositivePositivePositive = (TZeroCombined::ZeroPositive << 2) | TZeroCombined::PositivePositive,

				PositiveZeroZeroZero = (TZeroCombined::PositiveZero << 2) | TZeroCombined::ZeroZero,
				PositiveZeroZeroPositive = (TZeroCombined::PositiveZero << 2) | TZeroCombined::ZeroPositive,
				PositiveZeroPositiveZero = (TZeroCombined::PositiveZero << 2) | TZeroCombined::PositiveZero,
				PositiveZeroPositivePositive = (TZeroCombined::PositiveZero << 2) | TZeroCombined::PositivePositive,

				PositivePositiveZeroZero = (TZeroCombined::PositivePositive << 2) | TZeroCombined::ZeroZero,
				PositivePositiveZeroPositive = (TZeroCombined::PositivePositive << 2) | TZeroCombined::ZeroPositive,
				PositivePositivePositiveZero = (TZeroCombined::PositivePositive << 2) | TZeroCombined::PositiveZero,
				PositivePositivePositivePositive = (TZeroCombined::PositivePositive << 2) | TZeroCombined::PositivePositive

			};
		};

		using Relation = typename TTypeN::TRelation;
		using RelationCombined = typename TTypeN::TRelationCombined;
		using TZero = typename TTypeN::TZero;
		using ZeroCombined = typename TTypeN::TZeroCombined;
		using ZeroCombined2 = typename TTypeN::TZeroCombined2;

		static constexpr const inline Relation GetRelation(const BaseType& x, const BaseType& y) {
			return static_cast<Relation>((x == y) + ((x > y) << 1));
		}

		static constexpr const inline RelationCombined GetRelation(const Relation high, const Relation low) {
			return static_cast<RelationCombined>((static_cast<char>(high) << 2) | static_cast<char>(low));
		}

		static constexpr const inline RelationCombined GetRelation(const CompositeUintT x, const CompositeUintT y) {
			return GetRelation(GetRelation(x.high, y.high), GetRelation(x.low, y.low));
		}

		static constexpr const inline TZero GetZeroRelation(const BaseType& x) {
			return static_cast<TZero>(x != 0);
		}

		static constexpr const inline ZeroCombined GetRelation(const TZero high, const TZero low) {
			return static_cast<ZeroCombined>((static_cast<char>(high) << 1) | static_cast<char>(low));
		}

		static constexpr const inline ZeroCombined2 GetRelation(const ZeroCombined high, const ZeroCombined low) {
			return static_cast<ZeroCombined2>((static_cast<char>(high) << 2) | static_cast<char>(low));
		}

		static constexpr const inline ZeroCombined GetZeroRelation(const BaseType& high, const BaseType& low) {
			return GetRelation(GetZeroRelation(high), GetZeroRelation(low));
		}

		static constexpr const inline ZeroCombined2 GetZeroRelation2(const CompositeUintT x, const CompositeUintT y) {
			return GetRelation(GetZeroRelation(GetZeroRelation(x.high), GetZeroRelation(x.low)), GetZeroRelation(GetZeroRelation(y.high), GetZeroRelation(y.low)));
		}

		static constexpr const ShiftValueType BitsCount = 8 * sizeof(BaseType);

		static constexpr const BaseType BaseMax = static_cast<BaseType>(-1);

		static_assert(BitsCount <= sizeof(BaseType) * 8, "Invalid bits count value");

		static constexpr const ShiftValueType BitsCountHalf = BitsCount / 2;
		static constexpr const ShiftValueType BitsCount2 = BitsCount * 2;
		static constexpr const ShiftValueType BaseMaxHalf = BaseMax >> BitsCountHalf;
		static constexpr const BaseType TopBit = BaseType(1) << (BaseType(BitsCount - 1));

	public:

		CompositeUintT(const CompositeUintT& another) : high(another.high), low(another.low) {

		}

		explicit constexpr CompositeUintT(const BaseType& high, const BaseType low) : high(high), low(low) {

		}

		explicit constexpr CompositeUintT(const BaseType& low) : high(0), low(low) {

		}

		constexpr const CompositeUintT& operator=(const CompositeUintT& another) {
			high = another.high;
			low = another.low;
			return *this;
		}

		constexpr bool operator==(CompositeUintT another) const {
			return high == another.high && low == another.low;
		}

		constexpr bool operator!=(CompositeUintT another) const {
			return high != another.high || low != another.low;
		}

		constexpr bool operator>=(CompositeUintT another) const {
			switch (GetRelation(*this, another)) {
			case RelationCombined::LesserLesser: return false;
			case RelationCombined::EqualsLesser: return false;
			case RelationCombined::GreaterLesser: return true;
			case RelationCombined::LesserEquals: return false;
			case RelationCombined::EqualsEquals: return true;
			case RelationCombined::GreaterEquals: return true;
			case RelationCombined::LesserGreater: return false;
			case RelationCombined::EqualsGreater: return true;
			case RelationCombined::GreaterGreater: return true;
			default:
				EventHandler::MalformedValue();
				return false;
			}
		}

		constexpr bool operator>(CompositeUintT another) const {
			switch (GetRelation(*this, another)) {
			case RelationCombined::LesserLesser: return false;
			case RelationCombined::EqualsLesser: return false;
			case RelationCombined::GreaterLesser: return true;
			case RelationCombined::LesserEquals: return false;
			case RelationCombined::EqualsEquals: return false;
			case RelationCombined::GreaterEquals: return true;
			case RelationCombined::LesserGreater: return false;
			case RelationCombined::EqualsGreater: return true;
			case RelationCombined::GreaterGreater: return true;
			default:
				EventHandler::MalformedValue();
				return false;
			}
		}

		constexpr bool operator<=(CompositeUintT another) const {
			switch (GetRelation(*this, another)) {
			case RelationCombined::LesserLesser: return true;
			case RelationCombined::EqualsLesser: return true;
			case RelationCombined::GreaterLesser: return false;
			case RelationCombined::LesserEquals: return true;
			case RelationCombined::EqualsEquals: return true;
			case RelationCombined::GreaterEquals: return false;
			case RelationCombined::LesserGreater: return true;
			case RelationCombined::EqualsGreater: return false;
			case RelationCombined::GreaterGreater: return false;
			default:
				EventHandler::MalformedValue();
				return false;
			}
		}

		constexpr bool operator<(CompositeUintT another) const {
			switch (GetRelation(*this, another)) {
			case RelationCombined::LesserLesser: return true;
			case RelationCombined::EqualsLesser: return true;
			case RelationCombined::GreaterLesser: return false;
			case RelationCombined::LesserEquals: return true;
			case RelationCombined::EqualsEquals: return false;
			case RelationCombined::GreaterEquals: return false;
			case RelationCombined::LesserGreater: return true;
			case RelationCombined::EqualsGreater: return false;
			case RelationCombined::GreaterGreater: return false;
			default:
				EventHandler::MalformedValue();
				return false;
			}
		}

		constexpr const CompositeUintT operator+(const CompositeUintT& another) const {
			BaseType carry;
			const BaseType nextLow = Add<BaseType, BaseMax, CanOverflowBaseType>(low, another.low, carry);
			const BaseType nextHigh = Add<BaseType, BaseMax, CanOverflowBaseType>(high, carry, carry);
			if (EventHandler::ThrowAdditionOverflow) {
				if (carry > 0) {
					EventHandler::Overflow();
				}
			}
			const BaseType nextHigh2 = Add<BaseType, BaseMax, CanOverflowBaseType>(nextHigh, another.high, carry);
			if (EventHandler::ThrowAdditionOverflow) {
				if (carry > 0) {
					EventHandler::Overflow();
				}
			}
			return CompositeUintT(nextHigh2, nextLow);
		}

		constexpr const CompositeUintT operator-(const CompositeUintT& another) const {
			switch (GetRelation(*this, another)) {
			case RelationCombined::LesserLesser:
			{
				if (EventHandler::ThrowSubtractionOverflow) {
					EventHandler::Underflow();
				}
				return CompositeUintT((BaseMax - (another.high - high)), (BaseMax - (another.low - low)) + 1);
			}
			case RelationCombined::EqualsLesser:
			{
				if (EventHandler::ThrowSubtractionOverflow) {
					EventHandler::Underflow();
				}
				return CompositeUintT(BaseMax, (BaseMax - (another.low - low)) + 1);
			}
			case RelationCombined::GreaterLesser:
			{
				return CompositeUintT((high - 1) - another.high, BaseMax - (another.low - low) + 1);
			}
			case RelationCombined::LesserEquals:
			{
				if (EventHandler::ThrowSubtractionOverflow) {
					EventHandler::Underflow();
				}
				return CompositeUintT((BaseMax - (another.high - high)) + 1, 0);
			}
			case RelationCombined::EqualsEquals: return CompositeUintT(0);
			case RelationCombined::GreaterEquals: return CompositeUintT(high - another.high, 0);
			case RelationCombined::LesserGreater:
			{
				if (EventHandler::ThrowSubtractionOverflow) {
					EventHandler::Underflow();
				}
				return CompositeUintT((BaseMax - (another.high - high)) + 1, low - another.low);
			}
			case RelationCombined::EqualsGreater: return CompositeUintT(0, low - another.low);
			case RelationCombined::GreaterGreater: return CompositeUintT(high - another.high, low - another.low);
			default:
				EventHandler::MalformedValue();
				return CompositeUintT(0);
			}
		}

		constexpr CompositeUintT operator*(const CompositeUintT& another) const {
			switch (GetZeroRelation2(*this, another)) {
			case ZeroCombined2::ZeroZeroZeroZero: return *this;
			case ZeroCombined2::ZeroZeroZeroPositive: return *this;
			case ZeroCombined2::ZeroZeroPositiveZero: return *this;
			case ZeroCombined2::ZeroZeroPositivePositive: return *this;

			case ZeroCombined2::ZeroPositiveZeroZero: return another;

			case ZeroCombined2::ZeroPositiveZeroPositive:
			{
				BaseType lowCarry;
				BaseType nextLow = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(low, another.low, lowCarry);
				return CompositeUintT(lowCarry, nextLow);
			}

			case ZeroCombined2::ZeroPositivePositiveZero:
			{
				BaseType highCarry;
				BaseType nextHigh = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(low, another.high, highCarry);
				if (EventHandler::ThrowMultiplicationOverflow) {
					EventHandler::Overflow();
				}
				return CompositeUintT(nextHigh, 0);
			}

			case ZeroCombined2::ZeroPositivePositivePositive:
			{
				BaseType lowCarry, highCarry, lowAddedCarry;
				BaseType nextLow = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(low, another.low, lowCarry);
				BaseType nextHigh = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(low, another.high, highCarry);
				if (lowCarry > 0) {
					nextHigh = Add<BaseType, BaseMax, CanOverflowBaseType>(nextHigh, lowCarry, lowAddedCarry);
					if (EventHandler::ThrowMultiplicationOverflow) {
						if (lowAddedCarry > 0) {
							EventHandler::Overflow();
						} else if (highCarry > 0) {
							EventHandler::Overflow();
						}
					}
				} else if (EventHandler::ThrowMultiplicationOverflow && highCarry > 0) {
					EventHandler::Overflow();
				}
				return CompositeUintT(nextHigh, nextLow);
			}

			case ZeroCombined2::PositiveZeroZeroZero: return another;

			case ZeroCombined2::PositiveZeroZeroPositive:
			{
				BaseType carry;
				BaseType nextHigh = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(high, another.low, carry);
				if (EventHandler::ThrowMultiplicationOverflow) {
					if (carry > 0) {
						EventHandler::Overflow();
					}
				}
				return CompositeUintT(nextHigh, 0);
			}

			case ZeroCombined2::PositiveZeroPositiveZero:
				if (EventHandler::ThrowMultiplicationOverflow) {
					EventHandler::Overflow();
				}
				return CompositeUintT(0);

			case ZeroCombined2::PositiveZeroPositivePositive:
			{
				if (EventHandler::ThrowMultiplicationOverflow) {
					EventHandler::Overflow();
				}
				BaseType lowCarry;
				BaseType nextLow = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(high, another.low, lowCarry);
				return CompositeUintT(nextLow, 0);
			}

			case ZeroCombined2::PositivePositiveZeroZero: return another;

			case ZeroCombined2::PositivePositiveZeroPositive:
			{
				BaseType lowCarry, highCarry, lowAddedCarry;
				BaseType nextLow = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(low, another.low, lowCarry);
				BaseType nextHigh = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(high, another.low, highCarry);
				if (lowCarry > 0) {
					nextHigh = Add<BaseType, BaseMax, CanOverflowBaseType>(nextHigh, lowCarry, lowAddedCarry);
					if (EventHandler::ThrowMultiplicationOverflow) {
						if (lowAddedCarry > 0) {
							EventHandler::Overflow();
						} else if (highCarry > 0) {
							EventHandler::Overflow();
						}
					}
				} else if (EventHandler::ThrowMultiplicationOverflow && highCarry > 0) {
					EventHandler::Overflow();
				}
				return CompositeUintT(nextHigh, nextLow);
			}

			case ZeroCombined2::PositivePositivePositiveZero:
			{
				if (EventHandler::ThrowMultiplicationOverflow) {
					EventHandler::Overflow();
				}
				BaseType lowCarry;
				BaseType nextLow = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(low, another.high, lowCarry);
				return CompositeUintT(nextLow, 0);
			}

			case ZeroCombined2::PositivePositivePositivePositive:
			{
				if (EventHandler::ThrowMultiplicationOverflow) {
					EventHandler::Overflow();
				}
				BaseType lowCarry, highCarry;
				BaseType nextLow = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(low, another.low, lowCarry);

				BaseType nextHigh1 = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(low, another.high, highCarry);
				BaseType nextHigh2 = Mult<BaseType, BaseMax, BitsCount, CanOverflowBaseType>(high, another.low, highCarry);

				BaseType totalHigh = Add<BaseType, BaseMax, CanOverflowBaseType>(nextHigh1, nextHigh2, highCarry);
				if (lowCarry) {
					totalHigh = Add<BaseType, BaseMax, CanOverflowBaseType>(totalHigh, lowCarry, highCarry);
				}
				return CompositeUintT(totalHigh, nextLow);
			}

			default:
				EventHandler::MalformedValue();
				return CompositeUintT(0);
			}
		}

		constexpr const CompositeUintT operator/(const CompositeUintT& another) const {
			CompositeUintT dummy{ 0 };
			return DivMod<true, false>(another, dummy);
		}

		constexpr const CompositeUintT operator%(const CompositeUintT& another) const {
			CompositeUintT dummy{ 0 };
			DivMod<false, true>(another, dummy);
			return dummy;
		}

		constexpr const CompositeUintT operator&(const CompositeUintT& another) const {
			return CompositeUintT(high & another.high, low & another.low);
		}

		constexpr const CompositeUintT operator|(const CompositeUintT& another) const {
			return CompositeUintT(high | another.high, low | another.low);
		}

		constexpr const CompositeUintT operator^(const CompositeUintT& another) const {
			return CompositeUintT(high ^ another.high, low ^ another.low);
		}

		constexpr const CompositeUintT operator~() const {
			return CompositeUintT(~high. ~low);
		}

		constexpr const CompositeUintT operator>>(const CompositeUintT& another) const {
			if (another.low == 0) {
				return *this;
			} if (another.low == BitsCount) {
				return CompositeUintT(0, high);
			} else if (another.low < BitsCount) {
				return CompositeUintT(high >> another.low, (high << (BitsCount - another.low)) | (low >> another.low));
			} else if (another.low < 2 * BitsCount) {
				return CompositeUintT(0, high >> (another.low - BitsCount));
			} else {
				return CompositeUintT(0);
			}
		}

		constexpr const CompositeUintT operator<<(const CompositeUintT& another) const {
			return ShiftLeft(*this, another.low);
		}

		template<const bool WantDiv = true, const bool WantMod = true>
		constexpr const CompositeUintT DivMod(const CompositeUintT& another, CompositeUintT& mod) const {

			auto defDiv = [&]() {
				BaseType j = LeadingZeroes(*this);
				CompositeUintT bits{ *this };
				bits.ShiftLeft<false>(j);
				CompositeUintT quotient{ 0, 0 };
				CompositeUintT remainder{ 0, 0 };
				for (; j < BitsCount2; j++) {
					remainder.ShiftLeft(1);
					if (bits.high & TopBit) {
						remainder.low |= 1;
					}
					bits.ShiftLeft(1);
					quotient.ShiftLeft(1);
					if (remainder >= another) {
						quotient.Inc();
						remainder -= another;
					}
				}
				mod = remainder;
				return quotient;
			};

			switch (GetZeroRelation2(*this, another)) {
			case ZeroCombined2::ZeroZeroZeroZero: EventHandler::DivideByZero(); return another;
			case ZeroCombined2::ZeroZeroZeroPositive: mod = *this; return *this;
			case ZeroCombined2::ZeroZeroPositiveZero: mod = *this; return *this;
			case ZeroCombined2::ZeroZeroPositivePositive: mod = *this; return *this;


			case ZeroCombined2::ZeroPositiveZeroZero: EventHandler::DivideByZero(); mod = another; return another;
			case ZeroCombined2::ZeroPositiveZeroPositive: mod = CompositeUintT(0, low % another.low); return CompositeUintT(0, low / another.low);

			case ZeroCombined2::ZeroPositivePositiveZero:  mod = CompositeUintT(0, low); return CompositeUintT(0);
			case ZeroCombined2::ZeroPositivePositivePositive:  mod = CompositeUintT(0, low); return CompositeUintT(0);

			case ZeroCombined2::PositiveZeroZeroZero: EventHandler::DivideByZero();  mod = another; return another;
			case ZeroCombined2::PositiveZeroZeroPositive:
			{
				/*
				if (high > another.low) {
					if (another.low <= BaseMaxHalf) {
						// High order nibble is zero

						BaseType highDivHighMod;
						BaseType highDivHigh = DivMod(high, another.low, highDivHighMod);

						highDivHighMod++;
					} else {
						BaseType divHigh = another.low >> BitsCountHalf;
						BaseType divLow = another.low & BaseMaxHalf;

						BaseType highDivHighMod;
						BaseType highDivHigh = DivMod(high, divHigh, highDivHighMod);

						BaseType highDivLowMod;
						BaseType highDivLow = DivMod(high, divLow, highDivLowMod);

						BaseType highMod = (highDivHigh & BitsCountHalf) + highDivHighMod;
						highDivHigh >>= BitsCountHalf;

						BaseType highDivLow0 = high / divLow;
					}

				} else if (high == another.low) {
					mod = CompositeUintT(0);
					return CompositeUintT(1, 0);
				} else {
					if (CanOverflowBaseType) {
						BaseType lowDiv = (high * (BaseMax + 1)) / another.low;

						// mod = ?
						return CompositeUintT(0, lowDiv);
					} else if(another.low < BaseMaxHalf) {
						BaseType highMod;
						BaseType highDiv = DivMod(high, another.low, highMod);
						highMod <<= BitsCountHalf;

						BaseType lowHighMod;
						BaseType lowHigh = DivMod(highMod, another.low, lowHighMod);
						lowHighMod <<= BitsCountHalf;

						BaseType lowLowMod;
						BaseType lowLow = DivMod(lowHighMod, another.low, lowLowMod);

						BaseType rLow = (lowHigh << BitsCountHalf) | lowLow;

						mod = CompositeUintT(0, lowLowMod);
						return CompositeUintT(highDiv, rLow);
					} else {
						// Cannot divide
					}
				}
				*/
				return defDiv();
			}

			case ZeroCombined2::PositiveZeroPositiveZero:
			{
				return defDiv();
			}

			case ZeroCombined2::PositiveZeroPositivePositive:
			{
				return defDiv();
			}

			case ZeroCombined2::PositivePositiveZeroZero: EventHandler::DivideByZero(); mod = another; return another;

			case ZeroCombined2::PositivePositiveZeroPositive:
			{
				/*
				// Math
				if (another.low <= BaseMaxHalf) {
					BaseType highHigh = high >> BitsCountHalf;
					BaseType highLow = high & BaseMaxHalf;

					BaseType lowHigh = low >> BitsCountHalf;
					BaseType lowLow = low & BaseMaxHalf;

					BaseType mHighHigh;
					BaseType rHighHigh = DivMod(highHigh, another.low, mHighHigh);

					highLow += mHighHigh << BitsCountHalf;

					BaseType mHighLow;
					BaseType rHighLow = DivMod(highLow, another.low, mHighLow);

					lowHigh += mHighLow << BitsCountHalf;

					BaseType mLowHigh;
					BaseType rLowHigh = DivMod(lowHigh, another.low, mLowHigh);

					lowLow += mLowHigh << BitsCountHalf;

					BaseType mLowLow;
					BaseType rLowLow = DivMod(lowLow, another.low, mLowLow);

					BaseType rHigh = (rHighHigh << BitsCountHalf) | rHighLow;
					BaseType rLow = (rLowHigh << BitsCountHalf) | rLowLow;

					mod = CompositeUintT(mLowLow);
					return CompositeUintT(rHigh, rLow);
				} else {
					return defDiv();
				}
				*/
				return defDiv();
			}

			case ZeroCombined2::PositivePositivePositiveZero:
			{
				/*
				if (high < another.high) {
					mod = CompositeUintT(high, low);
					return CompositeUintT(0, 0);
				} else {
					BaseType rLow;
					BaseType res = DivMod(high, another.high, rLow);
					CompositeUintT mod = CompositeUintT(rLow, low);
					return CompositeUintT(0, res);
				}
				*/
				return defDiv();
			}

			case ZeroCombined2::PositivePositivePositivePositive:
			{
				return defDiv();
			}
			default:
				EventHandler::MalformedValue();
				return CompositeUintT(0);
			}
		}

		constexpr CompositeUintT operator++(int) {
			CompositeUintT result(high, low);
			Inc();
			return result;
		}

		constexpr const CompositeUintT& operator++() {
			Inc();
			return *this;
		}

		constexpr CompositeUintT operator--(int) {
			CompositeUintT result(high, low);
			Dec();
			return result;
		}

		constexpr const CompositeUintT& operator--() {
			Dec();
			return *this;
		}

		constexpr CompositeUintT& operator+=(const CompositeUintT& another) {
			*this = *this + another;
			return *this;
		}

		constexpr CompositeUintT& operator-=(const CompositeUintT& another) {
			*this = *this - another;
			return *this;
		}

		constexpr CompositeUintT& operator*=(const CompositeUintT& another) {
			*this = *this + another;
			return *this;
		}

		constexpr CompositeUintT& operator/=(const CompositeUintT& another) {
			*this = *this / another;
			return *this;
		}

		constexpr CompositeUintT& operator%=(const CompositeUintT& another) {
			*this = this % another;
			return *this;
		}

		constexpr CompositeUintT& operator&=(const CompositeUintT& another) {
			*this = *this & another;
			return *this;
		}

		constexpr CompositeUintT& operator|=(const CompositeUintT& another) {
			*this = *this | another;
			return *this;
		}

		constexpr CompositeUintT& operator^=(const CompositeUintT& another) {
			*this = *this ^ another;
			return *this;
		}

		constexpr CompositeUintT& operator>>=(const CompositeUintT& another) {
			*this = *this << another;
			return *this;
		}

		constexpr CompositeUintT& operator<<=(const CompositeUintT& another) {
			*this = *this << another;
			return *this;
		}

		constexpr const BaseType GetHigh() const {
			return high;
		}

		constexpr const BaseType GetLow() const {
			return low;
		}

		template<const bool Checked = true>
		constexpr void ShiftLeft(BaseType shift) {
			if (shift == 0) {
				return;
			} if (shift == BitsCount) {
				high = low;
				low = 0;
			} else if (shift < BitsCount) {
				high = (low >> (BitsCount - shift)) | (high << shift);
				low = low << shift;
			} else if (shift < 2 * BitsCount) {
				high = low << (shift - BitsCount);
				low = 0;
			} else {
				if (Checked) {
					// Undefined shift
					EventHandler::UndefinedShift();
				}
				shift %= BitsCount;
				ShiftLeft<Checked>(shift);
			}
		}

	private:
		BaseType high;
		BaseType low;

		template<typename BaseTypeX, const BaseTypeX BaseMaxX, const bool CanOverflowBaseType>
		static constexpr const BaseTypeX Add(BaseTypeX x1, BaseTypeX x2, BaseTypeX& high) {
			constexpr const BaseTypeX ONE(1);
			constexpr const BaseTypeX ZERO(0);

			const BaseTypeX subLimit(BaseMaxX - x2);
			if (CanOverflowBaseType) {
				high = x1 > subLimit ? ONE : ZERO;
				return x1 + x2;
			} else {
				if (x1 > subLimit) {
					// Carry
					high = ONE;
					return (x1 - subLimit) - ONE;
				} else {
					// No carry
					high = ZERO;
					return x2 + x1;
				}
			}
		}

		template<typename BaseTypeX, const BaseTypeX BaseMaxX, const ShiftValueType BitsCountX, const bool CanOverflowBaseTypeX>
		static constexpr const BaseTypeX Mult(BaseTypeX x1, BaseTypeX x2, BaseTypeX& high) {
			struct {
				const BaseTypeX _BaseMaxX;
				const ShiftValueType _BitsCountX;
				const bool _CanOverflowBaseTypeX;
			} Template{ BaseMaxX, BitsCountX, CanOverflowBaseTypeX };

			BaseType maxMultiplicableByLow = BaseMaxX / x1;

			if (x2 <= maxMultiplicableByLow) {
				// No overflow
				high = 0;
				return BaseTypeX(x1 * x2);
			} else {
				constexpr const ShiftValueType BitsCountHalfX = BitsCountX / 2;
				constexpr const ShiftValueType BaseHalfMaxX = BaseMaxX >> BitsCountHalfX;

				// Some overflow
				const BaseTypeX A = x1 >> BitsCountHalfX;
				const BaseTypeX B = x1 & BaseHalfMaxX;

				const BaseTypeX C = x2 >> BitsCountHalfX;
				const BaseTypeX D = x2 & BaseHalfMaxX;

				// Will never overflow
				const BaseTypeX Q1 = D * B;
				const BaseTypeX Q2 = D * A;
				const BaseTypeX Q3 = C * B;
				const BaseTypeX Q4 = C * A;

				const BaseTypeX W1L = Q1 >> BitsCountHalfX;
				const BaseTypeX W1R = Q1 & BaseHalfMaxX;

				const BaseTypeX W2L = Q2 >> BitsCountHalfX;
				const BaseTypeX W2R = Q2 & BaseHalfMaxX;

				const BaseTypeX W3L = Q3 >> BitsCountHalfX;
				const BaseTypeX W3R = Q3 & BaseHalfMaxX;

				const BaseTypeX W4L = Q4 >> BitsCountHalfX;
				const BaseTypeX W4R = Q4 & BaseHalfMaxX;

				const BaseTypeX E1 = W1R;

				const BaseTypeX E2_ = W1L + W2R + W3R;
				const BaseTypeX E2 = E2_ & BaseHalfMaxX;
				const BaseTypeX E2C = E2_ >> BitsCountHalfX;

				const BaseTypeX E3_ = W2L + W3L + W4R + E2C;
				const BaseTypeX E3 = E3_ & BaseHalfMaxX;
				const BaseTypeX E3C = E3_ >> BitsCountHalfX;

				const BaseTypeX E4_ = W4L + E3C;
				const BaseTypeX E4 = E4_ & BaseHalfMaxX;
				const BaseTypeX E4C = E4_ >> BitsCountHalfX;

				const BaseTypeX resultLow = (E2 << BitsCountHalfX) | E1;
				const BaseTypeX resultHigh = (E4 << BitsCountHalfX) | E3;

				high = resultHigh;
				return resultLow;
			}
		}

		static constexpr const CompositeUintT& DivMod(CompositeUintT a, CompositeUintT b, CompositeUintT& mod) {
			return a.DivMod(b, mod);
		}

		template<const bool Checked = true>
		static constexpr const CompositeUintT ShiftLeft(const CompositeUintT value, BaseType shift) {
			CompositeUintT tmp(value);
			tmp.ShiftLeft(shift);
			return tmp;
		}

		constexpr const void Inc() {
			if (low == BaseMax) {
				low = 0;
				if (high == BaseMax) {
					if (EventHandler::ThrowAdditionOverflow) {
						EventHandler::Overflow();
					}
					high = 0;
				} else {
					high++;
				}
			} else {
				low++;
			}
		}

		constexpr const void Dec() {
			if (low == 0) {
				low = BaseMax;
				if (high == 0) {
					if (EventHandler::ThrowSubtractionOverflow) {
						EventHandler::Underflow();
					}
					high = BaseMax;
				} else {
					high--;
				}
			} else {
				low--;
			}
		}

		template<typename T>
		static constexpr inline const T DivMod(T a, T b, T& mod) {
			ldiv_t res = std::ldiv(static_cast<long>(a), static_cast<long>(b));
			mod = static_cast<T>(res.rem);
			return static_cast<T>(res.quot);
		}

		static constexpr const BaseType LeadingZeroes(CompositeUintT x) {
			return x.high == 0 ? BitsCount + LeadingZeroes(x.low) : LeadingZeroes(x.high);
		}

		static constexpr const BaseType LeadingZeroes(uint8_t& x) {

			// complexity 4447
			constexpr const unsigned char mapping[]{
				4, 3, 2, 2, 1, 1, 1, 1,
				0, 0, 0, 0, 0, 0, 0, 0,
				0
			};
			const uint8_t l = mapping[x & 0b1111];
			const uint8_t h = mapping[x >> 4];
			const uint8_t t = ((h == 4) - 1) & 0b1111; // if h == 4, t = 0b0000, else t = 0b1111
			const uint8_t r = (h & t) + (l & ~t) + (4 & ~t);
			return r;


			/*
			// complexily 4436
			uint8_t res = 0;
			res += x != 0;
			x >>= 1;
			res += x != 0;
			x >>= 1;
			res += x != 0;
			x >>= 1;
			res += x != 0;
			x >>= 1;
			res += x != 0;
			x >>= 1;
			res += x != 0;
			x >>= 1;
			res += x != 0;
			x >>= 1;
			res += x != 0;
			return 8 - res;
			*/

		}

		static constexpr const BaseType LeadingZeroes(uint16_t& x) {
			uint8_t h = x >> 8;
			uint8_t l = x & 0xff;
			return h == 0 ? 8 + LeadingZeroes(l) : LeadingZeroes(h);
		}

		static constexpr const BaseType LeadingZeroes(uint32_t& x) {
			uint16_t h = x >> 16;
			uint16_t l = x & 0xffff;
			return h == 0 ? 16 + LeadingZeroes(l) : LeadingZeroes(h);
		}

		static constexpr const BaseType LeadingZeroes(uint64_t& x) {
			constexpr const auto deBruijn = 0x07edd5e59a4e28c2;
			constexpr const unsigned char mapping[64]{
				63, 00, 58,  1, 59, 47, 53,  2, 60, 39,
				48, 27, 54, 33, 42,  3, 61, 51, 37, 40,
				49, 18, 28, 20, 55, 30, 34, 11, 43,	14,
				22,  4, 62, 57, 46, 52, 38, 26, 32, 41,
				50, 36, 17, 19, 29, 10, 13, 21,	56, 45,
				25, 31, 35, 16,  9, 12, 44, 24, 15,  8,
				23,  7,  6,  5
			};
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
			auto base2Log = mapping[(x * deBruijn) >> 58];
			return BaseType(63 - base2Log);
		}

	};

}

namespace Numero {

	class FPUint128TEVT {

	public:

		static void MalformedValue() {}

		static void DivideByZero() {}

		static void UndefinedShift() {}

		static void Overflow() {}

		static void Underflow() {}

		static constexpr const bool ThrowAdditionOverflow = false;

		static constexpr const bool ThrowSubtractionOverflow = false;

		static constexpr const bool ThrowMultiplicationOverflow = false;

	};

	typedef CompositeUintT<false, uint32_t, FPUint64, FPUint128TEVT> FPUint128T;

	class FixedPointIgnoreAllErrors {

	public:

		static void Overflow() {}

		static void Underflow() {}

		static void MalformedValue() {}

		static void DivideByZero() {}

		static const void NegativeSqrt() {}

		static const void UndefinedAtan2() {}

	};

	class FixedPointIgnoreOverflow {

	public:

		static void Overflow() {

		}

		static void Underflow() {

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

		static void Underflow() {
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

	class HighestBitPositions {

	public:

		template<typename EventHandler>
		static constexpr FPInt32 GetHighestBitPosition(FPUint32& value) {
			return GetHighestBitPosition32(value);
		}

		template<typename EventHandler>
		static constexpr FPInt32 GetHighestBitPosition(FPUint64& value) {
			return GetHighestBitPosition64(value);
		}

		template<typename EventHandler>
		static constexpr FPInt32 GetHighestBitPosition(FPUint128T& value) {
			return GetHighestBitPosition128<EventHandler>(value);
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

		template<typename EventHandler>
		static constexpr FPInt32 GetHighestBitPosition128(FPUint128T value) {
			return value.GetHigh() > 0 ? 64 + GetHighestBitPosition64(value.GetHigh()) : GetHighestBitPosition64(value.GetLow());
		}

	};

	struct FixedPointWrap {

#ifdef USE_FPM
		using FPT = fpm::fixed<FPUint64, FPUint128, 32, RoundDivided>;
#else
		using FPT = FPUint64;
#endif

		struct TTypeN {
			enum TType : char {
				Positive = 0,
				Negative = 1,
				Zero = 2,
				NaN = 3,
				Inf = 4,
				NegInf = 5
			};

			enum TCombinedType : char {
				PositivePositive = (TTypeN::Positive << 3) + TTypeN::Positive,
				PositiveNegative = (TTypeN::Positive << 3) + TTypeN::Negative,
				PositiveZero = (TTypeN::Positive << 3) + TTypeN::Zero,
				PositiveNaN = (TTypeN::Positive << 3) + TTypeN::NaN,
				PositiveInf = (TTypeN::Positive << 3) + TTypeN::Inf,
				PositiveNegInf = (TTypeN::Positive << 3) + TTypeN::NegInf,

				NegativePositive = (TTypeN::Negative << 3) + TTypeN::Positive,
				NegativeNegative = (TTypeN::Negative << 3) + TTypeN::Negative,
				NegativeZero = (TTypeN::Negative << 3) + TTypeN::Zero,
				NegativeNaN = (TTypeN::Negative << 3) + TTypeN::NaN,
				NegativeInf = (TTypeN::Negative << 3) + TTypeN::Inf,
				NegativeNegInf = (TTypeN::Negative << 3) + TTypeN::NegInf,

				ZeroPositive = (TTypeN::Zero << 3) + TTypeN::Positive,
				ZeroNegative = (TTypeN::Zero << 3) + TTypeN::Negative,
				ZeroZero = (TTypeN::Zero << 3) + TTypeN::Zero,
				ZeroNaN = (TTypeN::Zero << 3) + TTypeN::NaN,
				ZeroInf = (TTypeN::Zero << 3) + TTypeN::Inf,
				ZeroNegInf = (TTypeN::Zero << 3) + TTypeN::NegInf,

				NaNPositive = (TTypeN::NaN << 3) + TTypeN::Positive,
				NaNNegative = (TTypeN::NaN << 3) + TTypeN::Negative,
				NaNZero = (TTypeN::NaN << 3) + TTypeN::Zero,
				NaNNaN = (TTypeN::NaN << 3) + TTypeN::NaN,
				NaNInf = (TTypeN::NaN << 3) + TTypeN::Inf,
				NaNNegInf = (TTypeN::NaN << 3) + TTypeN::NegInf,

				InfPositive = (TTypeN::Inf << 3) + TTypeN::Positive,
				InfNegative = (TTypeN::Inf << 3) + TTypeN::Negative,
				InfZero = (TTypeN::Inf << 3) + TTypeN::Zero,
				InfNaN = (TTypeN::Inf << 3) + TTypeN::NaN,
				InfInf = (TTypeN::Inf << 3) + TTypeN::Inf,
				InfNegInf = (TTypeN::Inf << 3) + TTypeN::NegInf,

				NegInfPositive = (TTypeN::NegInf << 3) + TTypeN::Positive,
				NegInfNegative = (TTypeN::NegInf << 3) + TTypeN::Negative,
				NegInfZero = (TTypeN::NegInf << 3) + TTypeN::Zero,
				NegInfNaN = (TTypeN::NegInf << 3) + TTypeN::NaN,
				NegInfInf = (TTypeN::NegInf << 3) + TTypeN::Inf,
				NegInfNegInf = (TTypeN::NegInf << 3) + TTypeN::NegInf

			};
		};

		using Type = typename TTypeN::TType;
		using CombinedType = typename TTypeN::TCombinedType;

		Type type;
		FPT value;
	};

	template<FP_DEC>
	class FixedPointT {

		using FPUint128 = FPUint128T;
		using Type = FixedPointWrap::Type;
		using CombinedType = FixedPointWrap::CombinedType;
		using FPT = FixedPointWrap::FPT;

	public:

		template<const FPUint32 left, const FPUint32 right = 0, const bool tnegative = false>
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
				return FPTFromParts(left, right, tnegative);
			};

			return FixedPointT(GetType(), FPT_Create());
		}

		static constexpr const FixedPointWrap __Inf{ Type::Inf, 0 };
		static constexpr const FixedPointWrap __NegInf{ Type::NegInf, 0 };
		static constexpr const FixedPointWrap __NaN{ Type::NaN, 0 };

		static constexpr const FixedPointT Inf() {
			return *reinterpret_cast<FixedPointT*>((void*)&__Inf);
		}

		static constexpr const FixedPointT NegativeInf() {
			return *reinterpret_cast<FixedPointT*>((void*)&__NegInf);
		}

		static constexpr const FixedPointT NaN() {
			return *reinterpret_cast<FixedPointT*>((void*)&__NaN);
		}

		static constexpr const FPT FPTFromParts(const FPUint32 left, const FPUint32 right = 0, const bool tnegative = false) {
			// Values in left and right, based in 10
			FPUint32 tmp = right;
			FPUint64 pow = 0;
			FPUint64 ten = 10;
			FPUint64 num = 0;
			FPUint32 totalPower = 1;

			while (tmp > 0) {
				num *= 10;
				num += tmp % 10;
				tmp /= 10;
				pow++;
				totalPower *= 10;
			}
			if (num > 0xFFFFFFFF) {
				EventHandler::Overflow();
				return FPT(0);
			}
			FPUint32 cLeft = static_cast<FPUint32>(left);
			FPUint32 cNum = static_cast<FPUint32>(num);
			FPUint32 cTen = 10;
			FPT fp = FixedPointT::FromWholeValue(cLeft);
			FPUint32 fNum = cNum;
			FPUint32 dec = 0;

			for (FPUint32 i = 0; i < 32; i++) {
				dec <<= 1;
				fNum *= 2;
				if (fNum >= totalPower) {
					fNum -= totalPower;
					dec |= 1;
				}
				if (fNum == 0) {
					dec <<= 31 - i;
					break;
				}
			}
			return fp + dec;
		}

		template<const FPUint64 extraValue, const bool negative = false>
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

		static constexpr const FPT FPTFromEncoded(const FPUint64 extraValue) {
#ifdef USE_FPM
			return FPT::from_raw_value(extraValue);
#else
			return (FPT)extraValue;
#endif
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

		static constexpr const Type TypeFromPart(FPUint32 whole, FPUint32 decimal, bool negative = false) {
			if (whole == 0 && decimal == 0) {
				return Type::Zero;
			} else if (negative) {
				return Type::Negative;
			} else {
				return Type::Positive;
			}
		}

		static constexpr const Type TypeFromEncoded(FPUint64 encoded, bool negative = false) {
			if (encoded == 0) {
				return Type::Zero;
			} else if (negative) {
				return Type::Negative;
			} else {
				return Type::Positive;
			}
		}

		static constexpr const FPT FPT_DIV(FPT x, FPT y, bool& inf) {
#ifdef USE_FPM
			FPT res = x / y;
#else 
			FPUint128 res128{ 0 };
			if (RoundDivided) {
				FPUint128 scaled_dividend{ x };
				scaled_dividend.ShiftLeft(33);
				res128 = scaled_dividend / FPUint128{ y };
				res128 = (res128 >> FPUint128T{ 1 }) + FPUint128T{ res128.GetLow() & 1 };
			} else {
				FPUint128 scaled_dividend{ x };
				scaled_dividend.ShiftLeft(32);
				res128 = scaled_dividend / FPUint128{ y };
			}
			if (res128.GetHigh() > 0) {
				inf = true;
			}
			FPT res = res128.GetLow();
			return res;
#endif
		}

#pragma push_macro("V_CONST")
#ifdef V_CONST
#undef V_CONST
#endif
#define V_CONST(name, whole, dec) \
		static constexpr const FixedPointWrap __##name { TypeFromPart(whole, dec), FPTFromParts(whole, dec) }; \
		static constexpr const inline FixedPointT& name() { return *reinterpret_cast<FixedPointT*>((void*)&__##name); }
		//static constexpr const inline FixedPointT name() { return FixedPointValue<whole, dec>(); }

#pragma push_macro("V_ENC")
#ifdef V_ENC
#undef V_ENC
#endif
#define V_ENC(name, encoded, ...) \
		static constexpr const FixedPointWrap __##name { TypeFromEncoded(encoded, __VA_ARGS__), FPTFromEncoded(encoded) }; \
		static constexpr const inline FixedPointT& name() { return *reinterpret_cast<FixedPointT*>((void*)&__##name); }
		//static constexpr const inline FixedPointT name() { return FixedPointEncodedValue<encoded, __VA_ARGS__>(); }

		V_CONST(sinDataFirst, 0, 0)
		V_CONST(sinDataLast, 1, 6)
		static constexpr const FPUint32 sinData[] = { // From 0 to 1.6
			0x00000000, 0x019998EA, 0x03332DBC, 0x04CCBA5E, 0x06663AB4, 0x07FFAAAA, 0x09990626, 0x0B324910, 0x0CCB6F50,
			0x0E6474D0, 0x0FFD5576, 0x11960D2E, 0x132E97E2, 0x14C6F178, 0x165F15E0, 0x17F70102, 0x198EAECA, 0x1B261B26,
			0x1CBD4200, 0x1E541F4A, 0x1FEAAEEE, 0x2180ECDE, 0x2316D508, 0x24AC635E, 0x264193D2, 0x27D66258, 0x296ACAE0,
			0x2AFEC964, 0x2C9259D4, 0x2E25782A, 0x2FB8205E, 0x314A4E6A, 0x32DBFE46, 0x346D2BEE, 0x35FDD362, 0x378DF09C,
			0x391D7FA0, 0x3AAC7C6C, 0x3C3AE304, 0x3DC8AF6C, 0x3F55DDA8, 0x40E269C2, 0x426E4FC2, 0x43F98BB2, 0x4584199C,
			0x470DF592, 0x48971BA2, 0x4A1F87DC, 0x4BA73656, 0x4D2E2324, 0x4EB44A5C, 0x5039A81A, 0x51BE3878, 0x5341F792,
			0x54C4E188, 0x5646F27E, 0x57C82694, 0x594879F2, 0x5AC7E8C0, 0x5C466F26, 0x5DC40954, 0x5F40B378, 0x60BC69C4,
			0x6237286C, 0x63B0EBA4, 0x6529AFA6, 0x66A170B0, 0x68182AFE, 0x698DDAD0, 0x6B027C6A, 0x6C760C14, 0x6DE88614,
			0x6F59E6B6, 0x70CA2A4A, 0x72394D22, 0x73A74B8E, 0x751421E8, 0x767FCC8A, 0x77EA47D2, 0x7953901E, 0x7ABBA1D0,
			0x7C227952, 0x7D88130A, 0x7EEC6B66, 0x804F7ED7, 0x81B149CF, 0x8311C8C1, 0x8470F82D, 0x85CED48B, 0x872B5A5D,
			0x88868625, 0x89E0546F, 0x8B38C1C1, 0x8C8FCAAB, 0x8DE56BBF, 0x8F39A191, 0x908C68BD, 0x91DDBDDD, 0x932D9D93,
			0x947C0483, 0x95C8EF55, 0x97145AB3, 0x985E434D, 0x99A6A5D9, 0x9AED7F0D, 0x9C32CBA3, 0x9D76885B, 0x9EB8B1F9,
			0x9FF94543, 0xA1383F05, 0xA2759C0F, 0xA3B15933, 0xA4EB734B, 0xA623E72F, 0xA75AB1C5, 0xA88FCFEB, 0xA9C33E8F,
			0xAAF4FA9B, 0xAC250101, 0xAD534EB7, 0xAE7FE0B5, 0xAFAAB3FD, 0xB0D3C591, 0xB1FB1277, 0xB32097BD, 0xB4445271,
			0xB5663FA9, 0xB6865C81, 0xB7A4A615, 0xB8C1198B, 0xB9DBB407, 0xBAF472B7, 0xBC0B52CF, 0xBD205181, 0xBE336C09,
			0xBF449FA9, 0xC053E9A3, 0xC1614741, 0xC26CB5D3, 0xC37632A9, 0xC47DBB21, 0xC5834C93, 0xC686E463, 0xC7887FFB,
			0xC8881CC3, 0xC985B82F, 0xCA814FB7, 0xCB7AE0D5, 0xCC72690B, 0xCD67E5E1, 0xCE5B54DD, 0xCF4CB397, 0xD03BFF9F,
			0xD1293693, 0xD2145615, 0xD2FD5BC9, 0xD3E4455D, 0xD4C9107F, 0xD5ABBAE5, 0xD68C424D, 0xD76AA479, 0xD846DF2B,
			0xD920F035, 0xD9F8D565, 0xDACE8C93, 0xDBA2139D, 0xDC736863, 0xDD4288D1, 0xDE0F72D1, 0xDEDA2459, 0xDFA29B61,
			0xE068D5E7, 0xE12CD1F1, 0xE1EE8D89, 0xE2AE06BF, 0xE36B3BA9, 0xE4262A61, 0xE4DED10B, 0xE5952DCB, 0xE6493ED3,
			0xE6FB0251, 0xE7AA7681, 0xE85799A1, 0xE90269F5, 0xE9AAE5C9, 0xEA510B6D, 0xEAF4D937, 0xEB964D85, 0xEC3566B9,
			0xECD2233D, 0xED6C817D, 0xEE047FF1, 0xEE9A1D11, 0xEF2D575F, 0xEFBE2D63, 0xF04C9DAB, 0xF0D8A6C7, 0xF1624753,
			0xF1E97DEF, 0xF26E493F, 0xF2F0A7F1, 0xF37098B5, 0xF3EE1A45, 0xF4692B61, 0xF4E1CACB, 0xF557F751, 0xF5CBAFC1,
			0xF63CF2F7, 0xF6ABBFCF, 0xF718152B, 0xF781F1F9, 0xF7E9552B, 0xF84E3DB3, 0xF8B0AA95, 0xF9109ACF, 0xF96E0D71,
			0xF9C90187, 0xFA21762D, 0xFA776A7B, 0xFACADD99, 0xFB1BCEB1, 0xFB6A3CF3, 0xFBB62795, 0xFBFF8DD5, 0xFC466EFB,
			0xFC8ACA4B, 0xFCCC9F1D, 0xFD0BECC3, 0xFD48B29D, 0xFD82F00F, 0xFDBAA485, 0xFDEFCF6F, 0xFE227047, 0xFE528689,
			0xFE8011BB, 0xFEAB1167, 0xFED38521, 0xFEF96C81, 0xFF1CC727, 0xFF3D94B5, 0xFF5BD4D9, 0xFF778747, 0xFF90ABB5,
			0xFFA741E7, 0xFFBB499F, 0xFFCCC2AB, 0xFFDBACDF, 0xFFE80813, 0xFFF1D42B, 0xFFF91109, 0xFFFDBE9F, 0xFFFFDCDF,
			0xFFFF6BC5, 0xFFFC6B4F, 0xFFF6DB87, 0xFFEEBC7B, 0xFFE40E3F
		};

		V_CONST(asinDataFirst, 0, 0)
		V_CONST(asinDataLast, 1, 0)
		static constexpr const FPUint64 asinData[] = {
			0x000000000, 0x00100002A, 0x002000154, 0x003000480, 0x004000AAA, 0x0050014D6, 0x006002402, 0x00700392E, 0x00800555E,
			0x009007990, 0x00A00A6C6, 0x00B00DE04, 0x00C012048, 0x00D016E96, 0x00E01C9F2, 0x00F02335E, 0x01002ABDE, 0x011033476,
			0x01203CE2A, 0x013047A02, 0x014053902, 0x015060C30, 0x01606F496, 0x01707F33C, 0x018090928, 0x0190A3768, 0x01A0B7F02,
			0x01B0CE106, 0x01C0E5E80, 0x01D0FF87C, 0x01E11B008, 0x01F138638, 0x020157C18, 0x0211792BC, 0x02219CB36, 0x0231C269E,
			0x0241EA606, 0x025214A86, 0x026241536, 0x027270732, 0x0282A2196, 0x0292D657A, 0x02A30D400, 0x02B346E48, 0x02C383572,
			0x02D3C2AA4, 0x02E404F00, 0x02F44A3B0, 0x0304929DA, 0x0314DE2AA, 0x03252CF4C, 0x03357F0EE, 0x0345D48C4, 0x03562D7FE,
			0x036689FD2, 0x0376EA178, 0x03874DE26, 0x0397B571C, 0x03A820D96, 0x03B8902D6, 0x03C90381E, 0x03D97AEB6, 0x03E9F67E4,
			0x03FA764F6, 0x040AFA738, 0x041B82FFA, 0x042C10094, 0x043CA1A5A, 0x044D37EA6, 0x045DD2ED4, 0x046E72C48, 0x047F17864,
			0x048FC148E, 0x04A070232, 0x04B1242BE, 0x04C1DD7A2, 0x04D29C256, 0x04E360452, 0x04F429F12, 0x0504F941A, 0x0515CE4EE,
			0x0526A9316, 0x05378A022, 0x054870DA4, 0x05595DD32, 0x056A51068, 0x057B4A8E4, 0x058C4A84C, 0x059D5104C, 0x05AE5E290,
			0x05BF720CC, 0x05D08CCBC, 0x05E1AE81C, 0x05F2D74B2, 0x060407448, 0x06153E8AC, 0x06267D3B6, 0x0637C3742, 0x064911530,
			0x065A66F6C, 0x066BC47E2, 0x067D2A08A, 0x068E97B60, 0x06A00DA68, 0x06B18BFB0, 0x06C312D46, 0x06D4A2548, 0x06E63A9D8,
			0x06F7DBD1E, 0x07098614E, 0x071B398A2, 0x072CF6560, 0x073EBC9D0, 0x07508C84C, 0x076266330, 0x077449CE6, 0x0786377DE,
			0x07982F698, 0x07AA31B96, 0x07BC3E96E, 0x07CE562BA, 0x07E078A22, 0x07F2A6258, 0x0804DEE1D, 0x08172303B, 0x082972B89,
			0x083BCE2EB, 0x084E35953, 0x0860A91C1, 0x087328F43, 0x0885B54F3, 0x08984E5FB, 0x08AAF4595, 0x08BDA770B, 0x08D067DB7,
			0x08E335CFF, 0x08F611863, 0x0908FB36D, 0x091BF31BD, 0x092EF9703, 0x09420E705, 0x095532599, 0x0968656AF, 0x097BA7E49,
			0x098EFA07D, 0x09A25C17D, 0x09B5CE589, 0x09C951105, 0x09DCE4863, 0x09F089033, 0x0A043ED23, 0x0A18063F5, 0x0A2BDF98D,
			0x0A3FCB2E9, 0x0A53C9527, 0x0A67DA585, 0x0A7BFE95F, 0x0A9036631, 0x0AA4821A1, 0x0AB8E2171, 0x0ACD56B8F, 0x0AE1E0609,
			0x0AF67F719, 0x0B0B34523, 0x0B1FFF6B7, 0x0B34E128D, 0x0B49D9F93, 0x0B5EEA4DF, 0x0B74129C1, 0x0B89535BB, 0x0B9EAD083,
			0x0BB42020D, 0x0BC9AD281, 0x0BDF54A4B, 0x0BF517217, 0x0C0AF52CF, 0x0C20EF5A9, 0x0C3706421, 0x0C4D3A7FF, 0x0C638CB5D,
			0x0C79FD8A7, 0x0C908DAA3, 0x0CA73DC6D, 0x0CBE0E987, 0x0CD500DD5, 0x0CEC155A5, 0x0D034CDB3, 0x0D1AA832F, 0x0D32283C7,
			0x0D49CDDA3, 0x0D6199F79, 0x0D798D889, 0x0D91A98AF, 0x0DA9EF05F, 0x0DC25F0B9, 0x0DDAFAB8F, 0x0DF3C3369, 0x0E0CB9B9D,
			0x0E25DF84F, 0x0E3F35E81, 0x0E58BE425, 0x0E727A027, 0x0E8C6AA81, 0x0EA691C47, 0x0EC0F0FC3, 0x0EDB8A081, 0x0EF65EB67,
			0x0F1170ED5, 0x0F2CC2AB3, 0x0F485609D, 0x0F642D3F7, 0x0F804AA15, 0x0F9CB0A65, 0x0FB961E97, 0x0FD6612C7, 0x0FF3B15BB,
			0x10115591C, 0x102F511B2, 0x104DA77AC, 0x106C5C6F8, 0x108B73F9A, 0x10AAF2612, 0x10CADC3D4, 0x10EB367C2, 0x110C066D2,
			0x112D51CAA, 0x114F1EC66, 0x117174184, 0x1194590DE, 0x11B7D59DC, 0x11DBF27DC, 0x1200B93CC, 0x122634618, 0x124C6F8F6,
			0x127377B24, 0x129B5B338, 0x12C42A3A2, 0x12EDF6FAC, 0x1318D6190, 0x1344DF236, 0x13722D2FE, 0x13A0DFA4A, 0x13D11B3FC,
			0x14030B73C, 0x1436E4418, 0x146CE4C72, 0x14A55AE32, 0x14E0A887C, 0x151F4BD12, 0x1561EBD9C, 0x15A96E34A, 0x15F71D7FE,
			0x164CF5514, 0x16AE4C632, 0x1721A5D86, 0x17B7D33B8, 0x1921FB545,
		};

		V_CONST(sqrtDataFirst, 0, 0)
		V_CONST(sqrtDataLast, 0xFFFFFFFF, 0)
		static constexpr const FPUint64 sqrtData[] = {
			0x000000000000, 0x0FFFFFFFF801, 0x16A09E6673EB, 0x1BB67AE84A70, 0x1FFFFFFFF001, 0x23C6EF371E05, 0x27311C27FEA8, 0x2A54FF5390C7,
			0x2D413CCCE7D6, 0x2FFFFFFFE801, 0x3298B0759B6B, 0x3510E527E055, 0x376CF5D094E3, 0x39B05688A5DA, 0x3BDDD422B28F, 0x3DF7BD627FA1,
			0x3FFFFFFFE001, 0x41F83D9A9EB6, 0x43E1DB335BC3, 0x45BE0CD16E58, 0x478DDE6E3C0B, 0x49523AE42FD0, 0x4B0BF1652BD5, 0x4CBBB9D5B5B3,
			0x4E62384FFD52, 0x4FFFFFFFD801, 0x5195957C1FF5, 0x532370B8DF54, 0x54A9FEA7218F, 0x5629A2927853, 0x57A2B748AEC5, 0x59159015767D,
			0x5A827999CFAC, 0x5BE9BA855D4F, 0x5D4B94369FE9, 0x5EA843461FB4, 0x5FFFFFFFD001, 0x6152FECD5EC7, 0x62A17093AAC8, 0x63EB83053959,
			0x653160EB36D4, 0x667332664CC2, 0x67B11D286470, 0x68EB44A8420E, 0x6A21CA4FC0AB, 0x6B54CDA55A11, 0x6C846C718CFE, 0x6DB0C2E09F77,
			0x6ED9EBA129C5, 0x6FFFFFFFC801, 0x712318004398, 0x72434A747BEC, 0x7360AD114BB7, 0x747B5481A1B0, 0x75935477FBFC, 0x76A8BFBE7033,
			0x77BBA845651F, 0x78CC1F311ED6, 0x79DA34E63A24, 0x7AE5F9153109, 0x7BEF7AC4FF42, 0x7CF6C85CFB57, 0x7DFBEFADF63F, 0x7EFEFDFAB255,
			0x7FFFFFFFC001, 0x80FF01FACD56, 0x81FC0FB174C2, 0x82F734779528, 0x83F07B353D6E, 0x84E7EE6C340D, 0x85DD983D2291, 0x86D1826C6C6F,
			0x87C3B666B785, 0x88B43D452C4A, 0x89A31FD17338, 0x8A9066897436, 0x8B7C19A2DCB0, 0x8C66410E706A, 0x8D4EE47B28E1, 0x8E360B5926A8,
			0x8F1BBCDC7817, 0x8FFFFFFFB801, 0x90E2DB86874F, 0x91C455FFE3FE, 0x92A475C85FA0, 0x9383410C37B2, 0x9460BDC95191, 0x953CF1D11BF9,
			0x9617E2CA57A9, 0x96F19632C8C1, 0x97CA1160D23E, 0x98A15984FCFC, 0x997773AB6B64, 0x9A4C64BD3B02, 0x9B203181D508, 0x9BF2DEA02EB3,
			0x9CC4709FFAA6, 0x9D94EBEACBE9, 0x9E6454CD2B70, 0x9F32AF77A101, 0x9FFFFFFFB001, 0xA0CC4A60C8E8, 0xA197927D3019, 0xA261DC1EDA59,
			0xA32B2AF83FEA, 0xA3F382A52653, 0xA4BAE6AB61A7, 0xA5815A7B8D92, 0xA646E171BEA9, 0xA70B7ED62C42, 0xA7CF35DDD37E, 0xA89209AB136E,
			0xA953FD4E431D, 0xAA1513C64176, 0xAAD55000FF80, 0xAB94B4DC051D, 0xAC534524F0A4, 0xAD110399F192, 0xADCDF2EA3E67, 0xAE8A15B68628,
			0xAF456E915D88, 0xAFFFFFFFA801, 0xB0B9CC78FCF5, 0xB172D668093D, 0xB22B202AECF8, 0xB2E2AC13961D, 0xB3997C6817AE, 0xB44F9362FDE7,
			0xB504F3339F5B, 0xB5B99DFE6B55, 0xB66D95DD3561, 0xB720DCDF7E4B, 0xB7D3750ABA9C, 0xB885605A96AE, 0xB936A0C1386B, 0xB9E738277EE4,
			0xBA97286D3FD0, 0xBB46736982EA, 0xBBF51AEABB76, 0xBCA320B6FFD8, 0xBD50868C3F69, 0xBDFD4E207682, 0xBEA97921E0FA, 0xBF5509372AE8,
			0xBFFFFFFFA001, 0xC0AA5F135954, 0xC154280369C8, 0xC1FD5C5A0918, 0xC2A5FD9ABD8F, 0xC34E0D428472, 0xC3F58CC7F951, 0xC49C7D9B7BFE,
			0xC542E1275591, 0xC5E8B8CFDC27, 0xC68E05F395BE, 0xC732C9EB59ED, 0xC7D7060A72B0, 0xC87ABB9EBC49, 0xC91DEBF0C423, 0xC9C09843E6E5,
			0xCA62C1D66DA9, 0xCB0469E1AA58, 0xCBA5919A1347, 0xCC463A2F5E08, 0xCCE664CC9984, 0xCD8612984752, 0xCE2544B47470, 0xCEC3FC3ED13F,
			0xCF623A50C8E2, 0xCFFFFFFF9801, 0xD09D4E5C62E2, 0xD13A26744B0A, 0xD1D68950841F, 0xD27277F6686C, 0xD30DF3678CC5, 0xD3A8FCA1D3E5,
			0xD443949F8157, 0xD4DDBC574BE7, 0xD57774BC6F86, 0xD610BEBEBED3, 0xD6A99B4AB423, 0xD7420B498224, 0xD7DA0FA12414, 0xD871A9346D8F,
			0xD908D8E319FD, 0xD99F9F89DB9F, 0xDA35FE026A41, 0xDACBF5239187, 0xDB6185C13EEF, 0xDBF6B0AC8F73, 0xDC8B76B3DCE0, 0xDD1FD8A2CADB,
			0xDDB3D742538B, 0xDE477358D40A, 0xDEDAADAA1881, 0xDF6D86F767FD, 0xDFFFFFFF9001, 0xE092197EEFD0, 0xE123D42F837D, 0xE1B530C8EEB0,
			0xE24630008732, 0xE2D6D2895F41, 0xE36719144F9E, 0xE3F70450016E, 0xE48694E8F7DB, 0xE515CB89997D, 0xE5A4A8DA3997, 0xE6332D812116,
			0xE6C15A22976E, 0xE74F2F60EB2C, 0xE7DCADDC7A73, 0xE869D633BB39, 0xE8F6A9034363, 0xE98326E5D0B0, 0xEA0F50745074, 0xEA9B2645E72E,
			0xEB26A8EFF7FB, 0xEBB1D9062BD7, 0xEC3CB71A78BF, 0xECC743BD28AC, 0xED517F7CE066, 0xEDDB6AE6A632, 0xEE650685E86C, 0xEEEE52E483E8,
			0xEF77508ACA3E, 0xEFFFFFFF8801, 0xF08861C80AB8, 0xF110766826E2, 0xF1983E623DAE, 0xF21FBA3742BB, 0xF2A6EA66C1A5, 0xF32DCF6EE385,
			0xF3B469CC7448, 0xF43AB9FAE7F9, 0xF4C0C0745FDF, 0xF5467DB1AF9A, 0xF5CBF22A6211, 0xF6511E54BE52, 0xF6D602A5CC5E, 0xF75A9F9159D3,
			0xF7DEF589FE87, 0xF8630501210C, 0xF8E6CE66FB1F, 0xF96A522A9DFC, 0xF9ED90B9F6AD, 0xFA708A81D229, 0xFAF33FEDE180, 0xFB75B168BDDD,
			0xFBF7DF5BEC7D, 0xFC79CA2FE291, 0xFCFB724C0912, 0xFD7CD816C089, 0xFDFDFBF564AB, 0xFE7EDE4C500D, 0xFEFF7F7EDF9E, 0xFF7FDFEF7639,
			0xFFFFFFFF8001
		};

		V_CONST(sqrtSmallDataFirst, 0, 0)
		V_CONST(sqrtSmallDataLast, 8, 0)
		static constexpr const FPUint64 sqrtSmallData[] = {
			0x000000000, 0x02D413CCC, 0x040000000, 0x04E623850, 0x05A827998, 0x0653160EA, 0x06ED9EBA0, 0x077BBA844, 0x080000001,
			0x087C3B667, 0x08F1BBCDD, 0x09617E2CB, 0x09CC470A1, 0x0A32B2AF9, 0x0A953FD4F, 0x0AF456E91, 0x0B504F333, 0x0BA97286D,
			0x0C0000001, 0x0C542E127, 0x0CA62C1D7, 0x0CF623A51, 0x0D443949F, 0x0D908D8E3, 0x0DDB3D743, 0x0E2463001, 0x0E6C15A23,
			0x0EB26A8F1, 0x0EF77508B, 0x0F3B469CD, 0x0F7DEF58B, 0x0FBF7DF5D, 0x100000000, 0x103F81F62, 0x107E0F66A, 0x10BBB307A,
			0x10F876CCC, 0x113463FA2, 0x116F83346, 0x11A9DC8F6, 0x11E3779B8, 0x121C5B70C, 0x12548EB90, 0x128C17B92, 0x12C2FC594,
			0x12F9422C2, 0x132EEE756, 0x136406304, 0x13988E140, 0x13CC8A99A, 0x140000000, 0x1432F24FA, 0x1465655F0, 0x14975CD56,
			0x14C8DC2E4, 0x14F9E6BBC, 0x152A7FA9C, 0x155AAA002, 0x158A68A4A, 0x15B9BE5D4, 0x15E8ADD22, 0x1617398F2, 0x164564056,
			0x16732F8D0, 0x16A09E666, 0x16CDB2BBA, 0x16FA6EA16, 0x1726D4182, 0x1752E50DA, 0x177EA35D6, 0x17AA10D18, 0x17D52F244,
			0x180000001, 0x182A85007, 0x1854BFB37, 0x187EB1991, 0x18A85C24F, 0x18D1C0BE7, 0x18FAE0C15, 0x1923BD7E3, 0x194C583AD,
			0x1974B2335, 0x199CCC999, 0x19C4A8969, 0x19EC474A3, 0x1A13A9CB9, 0x1A3AD12A1, 0x1A61BE6CF, 0x1A887293F, 0x1AAEEE979,
			0x1AD533697, 0x1AFB41F43, 0x1B211B1C7, 0x1B46BFC05, 0x1B6C30B83, 0x1B916ED69, 0x1BB67AE85, 0x1BDB55B55, 0x1C0000001,
			0x1C247A85F, 0x1C48C6001, 0x1C6CE3229, 0x1C90D29D3, 0x1CB4951B5, 0x1CD82B447, 0x1CFB95BB9, 0x1D1ED5207, 0x1D41EA0E9,
			0x1D64D51E1, 0x1D8796E35, 0x1DAA2FEFB, 0x1DCCA0D0D, 0x1DEEEA117, 0x1E110C391, 0x1E3307CC5, 0x1E54DD4CF, 0x1E768D399,
			0x1E98180E9, 0x1EB97E455, 0x1EDAC054D, 0x1EFBDEB15, 0x1F1CD9CCF, 0x1F3DB2175, 0x1F5E67FDD, 0x1F7EFBEB9, 0x1F9F6E499,
			0x1FBFBF7EB, 0x1FDFEFEFF, 0x200000000, 0x201FF00FE, 0x203FC07EC, 0x205F71A9C, 0x207F03EC6, 0x209E77A06, 0x20BDCD1DE,
			0x20DD04BB4, 0x20FC1ECD4, 0x211B1BA76, 0x2139FB9B0, 0x2158BEF8C, 0x2177660F4, 0x2195F12C0, 0x21B4609B2, 0x21D2B4A72,
			0x21F0ED99A, 0x220F0BBAC, 0x222D0F514, 0x224AF8A30, 0x2268C7F46, 0x22867D88C, 0x22A419A26, 0x22C19C826, 0x22DF0668C,
			0x22FC57948, 0x23199043A, 0x2336B0B30, 0x2353B91EC, 0x2370A9C1E, 0x238D82D64, 0x23AA44954, 0x23C6EF372, 0x23E382F32,
			0x240000000, 0x241C66932, 0x2438B6E1A, 0x2454F11F8, 0x247115800, 0x248D2435A, 0x24A91D722, 0x24C501668, 0x24E0D0432,
			0x24FC8A376, 0x25182F726, 0x2533C0222, 0x254F3C744, 0x256AA495C, 0x2585F8B2A, 0x25A138F6A, 0x25BC658CC, 0x25D77E9F4,
			0x25F284584, 0x260D76E0A, 0x262856614, 0x264323022, 0x265DDCEAE, 0x267884426, 0x2693192F6, 0x26AD9BD78, 0x26C80C608,
			0x26E26AEF2, 0x26FCB7A80, 0x2716F2AF2, 0x27311C280, 0x274B3435A, 0x27653AFAC, 0x277F30996, 0x279915334, 0x27B2E8E9E,
			0x27CCABDDE, 0x27E65E2FE, 0x280000001, 0x2819916DB, 0x283312985, 0x284C839EB, 0x2865E49F7, 0x287F35B87, 0x28987707D,
			0x28B1A8AAB, 0x28CACABE3, 0x28E3DD5EF, 0x28FCE0A95, 0x2915D4B97, 0x292EB9AAF, 0x29478F991, 0x2960569EF, 0x29790ED75,
			0x2991B85C9, 0x29AA5348B, 0x29C2DFB59, 0x29DB5DBCD, 0x29F3CD779, 0x2A0C2EFEB, 0x2A24826AD, 0x2A3CC7D47, 0x2A54FF53B,
			0x2A6D29003, 0x2A8544F1B, 0x2A9D533F5, 0x2AB554005, 0x2ACD474B7, 0x2AE52D371, 0x2AFD05D9B, 0x2B14D1495, 0x2B2C8F9BB,
			0x2B4440E69, 0x2B5BE53F3, 0x2B737CBAB, 0x2B8B076DF, 0x2BA2856DB, 0x2BB9F6CE7, 0x2BD15BA47, 0x2BE8B403B, 0x2C0000001,
			0x2C173FAD1, 0x2C2E731E5, 0x2C459A66F, 0x2C5CB59A1, 0x2C73C4CA7, 0x2C8AC80AD, 0x2CA1BF6D9, 0x2CB8AB04F, 0x2CCF8AE33,
			0x2CE65F1A1, 0x2CFD27BB7, 0x2D13E4D8D, 0x2D2A96839, 0x2D413CCCF
		};

		V_CONST(sqrtSmallDataFirst2, 8, 0)
		V_CONST(sqrtSmallDataLast2, 256, 0)
		static constexpr const FPUint64 sqrtSmallData2[] = {
			0x02D413CCCF, 0x02FEAA5EAF, 0x0327026110, 0x034D6E18F0, 0x03722DDAD0, 0x0395751547, 0x03B76DAB9D, 0x03D83A4509, 0x03F7F7EFD7,
			0x0416BF51DE, 0x0434A58A94, 0x0451BCDE04, 0x046E15383C, 0x0489BC93DB, 0x04A4BF4B1B, 0x04BF2858AB, 0x04D9018C11, 0x04F253B46F,
			0x050B26C3BA, 0x052381EBF4, 0x053B6BB79A, 0x0552EA1E40, 0x056A0295FA, 0x0580BA223F, 0x059715609F, 0x05AD1893CB, 0x05C2C7AD01,
			0x05D8265463, 0x05ED37F01F, 0x0601FFAAC6, 0x06168078E4, 0x062ABD1DDE, 0x063EB83056, 0x0652741E02, 0x0665F32F26, 0x06793789A6,
			0x068C4333CF, 0x069F1816C9, 0x06B1B800E1, 0x06C424A785, 0x06D65FA91F, 0x06E86A8EBF, 0x06FA46CD93, 0x070BF5C856, 0x071D78D08C,
			0x072ED127A2, 0x0740000000, 0x0751067E00, 0x0761E5B8D6, 0x07729EBB56, 0x07833284BB, 0x0793A20957, 0x07A3EE3339, 0x07B417E2C7,
			0x07C41FEF43, 0x07D4072755, 0x07E3CE5183, 0x07F3762C9F, 0x0802FF7034, 0x08126ACCEA, 0x0821B8ECD4, 0x0830EA73DC, 0x0840000000,
			0x084EFA29A2, 0x085DD983D6, 0x086C9E9C98, 0x087B49FD16, 0x0889DC29DD, 0x089855A31B, 0x08A6B6E4C9, 0x08B50066E7, 0x08C3329D9D,
			0x08D14DF96F, 0x08DF52E761, 0x08ED41D11F, 0x08FB1B1D21, 0x0908DF2ECE, 0x09168E669E, 0x0924292234, 0x0931AFBC82, 0x093F228DDC,
			0x094C81EC1A, 0x0959CE2AAE, 0x0967079ABC, 0x09742E8B30, 0x09814348D5, 0x098E461E65, 0x099B3754A5, 0x09A817326D, 0x09B4E5FCC5,
			0x09C1A3F6ED, 0x09CE51626D, 0x09DAEE7F2D, 0x09E77B8B7B, 0x09F3F8C41B, 0x0A0066645A, 0x0A0CC4A610, 0x0A1913C1BA, 0x0A2553EE7A,
			0x0A31856224, 0x0A3DA85154, 0x0A49BCEF66, 0x0A55C36E90, 0x0A61BBFFE6, 0x0A6DA6D35C, 0x0A798417DC, 0x0A8553FB45, 0x0A9116AA73,
			0x0A9CCC514B, 0x0AA8751AC3, 0x0AB41130E7, 0x0ABFA0BCDD, 0x0ACB23E6EF, 0x0AD69AD695, 0x0AE205B271, 0x0AED64A061, 0x0AF8B7C579,
			0x0B03FF4614, 0x0B0F3B45D2, 0x0B1A6BE79C, 0x0B25914DAE, 0x0B30AB999A, 0x0B3BBAEC4C, 0x0B46BF660C, 0x0B51B92688, 0x0B5CA84CD2,
			0x0B678CF76A, 0x0B72674440, 0x0B7D3750B0, 0x0B87FD3995, 0x0B92B91B3B, 0x0B9D6B1173, 0x0BA8133787, 0x0BB2B1A84D, 0x0BBD467E1B,
			0x0BC7D1D2D5, 0x0BD253BFE7, 0x0BDCCC5E4F, 0x0BE73BC69F, 0x0BF1A210F9, 0x0BFBFF551D, 0x0C0653AA5C, 0x0C109F27AA, 0x0C1AE1E39A,
			0x0C251BF458, 0x0C2F4D6FBA, 0x0C39766B3A, 0x0C4396FBF6, 0x0C4DAF36B6, 0x0C57BF2FF0, 0x0C61C6FBC4, 0x0C6BC6AE02, 0x0C75BE5A2A,
			0x0C7FAE136E, 0x0C8995ECB3, 0x0C9375F897, 0x0C9D4E496B, 0x0CA71EF13B, 0x0CB0E801CD, 0x0CBAA98CA3, 0x0CC463A2FD, 0x0CCE1655D5,
			0x0CD7C1B5E9, 0x0CE165D3B9, 0x0CEB02BF83, 0x0CF498894F, 0x0CFE2740E3, 0x0D07AEF5CE, 0x0D112FB768, 0x0D1AA994D2, 0x0D241C9CF2,
			0x0D2D88DE7C, 0x0D36EE67EC, 0x0D404D4792, 0x0D49A58B82, 0x0D52F741A8, 0x0D5C4277B8, 0x0D65873B38, 0x0D6EC59984, 0x0D77FD9FC6,
			0x0D812F5AFB, 0x0D8A5AD7F5, 0x0D9380235B, 0x0D9C9F49A9, 0x0DA5B8572F, 0x0DAECB581B, 0x0DB7D85869, 0x0DC0DF63F7, 0x0DC9E08675,
			0x0DD2DBCB71, 0x0DDBD13E53, 0x0DE4C0EA5B, 0x0DEDAADAA9, 0x0DF68F1A37, 0x0DFF6DB3DF, 0x0E0846B254, 0x0E111A202E, 0x0E19E807DC,
			0x0E22B073B2, 0x0E2B736DE4, 0x0E34310082, 0x0E3CE93584, 0x0E459C16BC, 0x0E4E49ADE2, 0x0E56F20492, 0x0E5F95244A, 0x0E68331668,
			0x0E70CBE436, 0x0E795F96D8, 0x0E81EE3761, 0x0E8A77CEC1, 0x0E92FC65D3, 0x0E9B7C0557, 0x0EA3F6B5F3, 0x0EAC6C8035, 0x0EB4DD6C91,
			0x0EBD498363, 0x0EC5B0CCF3, 0x0ECE135169, 0x0ED67118DF, 0x0EDECA2B55, 0x0EE71E90AF, 0x0EEF6E50C5, 0x0EF7B97353, 0x0F00000000,
			0x0F0841FE5E, 0x0F107F75EC, 0x0F18B86E14, 0x0F20ECEE2C, 0x0F291CFD72, 0x0F3148A318, 0x0F396FE638, 0x0F4192CDD6, 0x0F49B160EC,
			0x0F51CBA65A, 0x0F59E1A4F2, 0x0F61F36370, 0x0F6A00E882, 0x0F720A3AC6, 0x0F7A0F60C2, 0x0F821060F3, 0x0F8A0D41BF, 0x0F92060981,
			0x0F99FABE7D, 0x0FA1EB66EF, 0x0FA9D808FF, 0x0FB1C0AAC3, 0x0FB9A55247, 0x0FC1860585, 0x0FC962CA67, 0x0FD13BA6CB, 0x0FD910A07F,
			0x0FE0E1BD43, 0x0FE8AF02C7, 0x0FF07876B1, 0x0FF83E1E97, 0x1000000000
		};

		V_CONST(atanSmallDataFirst, 0, 0)
		V_CONST(atanSmallDataLast, 8, 0)
		static constexpr const FPUint64 atanSmallData[] = {
			0x000000000, 0x007FF556E, 0x00FFAADDA, 0x017EE1826, 0x01FD5BA9A, 0x027ADDDD0, 0x02F72F696, 0x03721AEA4, 0x03EB6EBF2,
			0x0462FD68C, 0x04D89DCDC, 0x054C2B664, 0x05BD86506, 0x062C934E4, 0x06993BB0E, 0x07036D324, 0x076B19C14, 0x07D03742C,
			0x0832BF4A7, 0x0892AECDF, 0x08F005D5F, 0x094AC72C9, 0x09A2F80E7, 0x09F89FDC5, 0x0A4BC7D19, 0x0A9C7ABDD, 0x0AEAC4C39,
			0x0B36B31C9, 0x0B8053E2B, 0x0BC7B5DEB, 0x0C0CE85B9, 0x0C4FFAFFB, 0x0C90FDAA3, 0x0CD000549, 0x0D0D12F97, 0x0D48457D3,
			0x0D81A79C3, 0x0DB948DA7, 0x0DEF3876B, 0x0E23855F9, 0x0E563E2BD, 0x0E8771129, 0x0EB72BE7B, 0x0EE57C16F, 0x0F126EA27,
			0x0F3E10211, 0x0F686CBE5, 0x0F919039D, 0x0FB985E95, 0x0FE058B8F, 0x1006132E2, 0x102ABF692, 0x104E67276, 0x107113C6A,
			0x1092CE470, 0x10B39F4EC, 0x10D38F2C4, 0x10F2A5D9E, 0x1110EB006, 0x112E65FA2, 0x114B1DD5E, 0x1167195A2, 0x11825F074,
			0x119CF51B0, 0x11B6E192E, 0x11D02A2EC, 0x11E8D473C, 0x1200E5AE0, 0x121862F3E, 0x122F5127E, 0x1245B4FAE, 0x125B92EE2,
			0x1270EF55A, 0x1285CE59C, 0x129A33F96, 0x12AE240BC, 0x12C1A241C, 0x12D4B2282, 0x12E757288, 0x12F9948B6, 0x130B6D796,
			0x131CE4FC8, 0x132DFE01C, 0x133EBB59C, 0x134F1FBB0, 0x135F2DC20, 0x136EE7F2A, 0x137E50B98, 0x138D6A6CE, 0x139C374D2,
			0x13AAB9864, 0x13B8F3306, 0x13C6E650A, 0x13D494DA2, 0x13E200AEA, 0x13EF2B9EA, 0x13FC176B6, 0x1408C5C64, 0x141538520,
			0x142170A34, 0x142D70410, 0x143938A54, 0x1444CB3D6, 0x1450296AE, 0x145B54836, 0x14664DD1C, 0x14711695E, 0x147BB005C,
			0x14861B4CE, 0x1490598E0, 0x149A6BE20, 0x14A453598, 0x14AE10FC6, 0x14B7A5CA6, 0x14C112BB8, 0x14CA58C04, 0x14D378C18,
			0x14DC73A18, 0x14E54A3B8, 0x14EDFD644, 0x14F68DEA6, 0x14FEFC962, 0x15074A2A6, 0x150F7763C, 0x151784FA0, 0x151F739F6,
			0x15274400E, 0x152EF6C6E, 0x15368C950, 0x153E060A4, 0x154563C10, 0x154CA64FC, 0x1553CE48A, 0x155ADC39A, 0x1561D0AD6,
			0x1568AC2A6, 0x156F6F33A, 0x15761A488, 0x157CADE56, 0x15832A832, 0x158990974, 0x158FE0948, 0x15961AEAC, 0x159C40068,
			0x15A250520, 0x15A84C348, 0x15AE3412E, 0x15B4084F4, 0x15B9C9494, 0x15BF775E6, 0x15C512E9C, 0x15CA9C440, 0x15D013C40,
			0x15D579BE4, 0x15DACE854, 0x15E012698, 0x15E545B9A, 0x15EA68C26, 0x15EF7BCEC, 0x15F47F27E, 0x15F973152, 0x15FE57DC6,
			0x16032DC1C, 0x1607F5080, 0x160CADF02, 0x161158B9E, 0x1615F5A32, 0x161A84E90, 0x161F06C6A, 0x16237B764, 0x1627E330A,
			0x162C3E2D4, 0x16308CA2A, 0x1634CEC5A, 0x163904CA8, 0x163D2EE40, 0x16414D440, 0x1645601B0, 0x16496798E, 0x164D63EC2,
			0x16515542A, 0x16553BC8E, 0x165917AAE, 0x165CE9136, 0x1660B02C6, 0x16646D1F0, 0x16682013A, 0x166BC931C, 0x166F689FE,
			0x1672FE83E, 0x16768B032, 0x167A0E41E, 0x167D8863A, 0x1680F98BA, 0x168461DC0, 0x1687C1764, 0x168B187B8, 0x168E670C0,
			0x1691AD476, 0x1694EB4CC, 0x1698213A8, 0x169B4F2EA, 0x169E75468, 0x16A1939EC, 0x16A4AA53A, 0x16A7B980E, 0x16AAC141A,
			0x16ADC1B0C, 0x16B0BAE82, 0x16B3AD01A, 0x16B69816A, 0x16B97C3FA, 0x16BC59952, 0x16BF302EE, 0x16C200248, 0x16C4C98D0,
			0x16C78C7EC, 0x16CA49106, 0x16CCFF576, 0x16CFAF694, 0x16D2595B4, 0x16D4FD41E, 0x16D79B31A, 0x16DA333E4, 0x16DCC57BA,
			0x16DF51FD0, 0x16E1D8D54, 0x16E45A172, 0x16E6D5D4E, 0x16E94C20A, 0x16EBBD0C0, 0x16EE28A88, 0x16F08F074, 0x16F2F0390,
			0x16F54C4E6, 0x16F7A357C, 0x16F9F5650, 0x16FC42860, 0x16FE8ACA4, 0x1700CE410, 0x17030CF94, 0x17054701A, 0x17077C68E,
			0x1709AD3D4, 0x170BD98CC, 0x170E01654, 0x171024D48, 0x171243E7A, 0x17145EAC2, 0x1716752EC, 0x1718877C8, 0x171A95A1E,
			0x171C9FAB4, 0x171EA5A4E, 0x1720A79AC, 0x1722A598C, 0x17249FAA8
		};

		V_CONST(atanDataFirst, 8, 0)
		V_CONST(atanDataLast, 1024, 0)
		static constexpr const FPUint64 atanData[] = {
			0x17249FAA8, 0x17CC8CDB4, 0x182150769, 0x185463DB1, 0x187685925, 0x188EF0369, 0x18A1450C3, 0x18AF89C65, 0x18BAF59F3,
			0x18C44EED7, 0x18CC1A105, 0x18D2B2C0B, 0x18D85A8E1, 0x18DD41941, 0x18E18BEB5, 0x18E555317, 0x18E8B2E2F, 0x18EBB5F7B,
			0x18EE6C039, 0x18F0E0051, 0x18F31AFBB, 0x18F524567, 0x18F702479, 0x18F8BA03B, 0x18FA4FF27, 0x18FBC7D4B, 0x18FD24E21,
			0x18FE69E0B, 0x18FF99381, 0x1900B5005, 0x1901BF0E9, 0x1902B8FF1, 0x1903A43DD, 0x1904820D1, 0x1905538B1, 0x190619B79,
			0x1906D5773, 0x190787973, 0x190830D01, 0x1908D1C8B, 0x19096B17B, 0x1909FD461, 0x190A88D01, 0x190B0E271, 0x190B8DB29,
			0x190C07D0F, 0x190C7CD95, 0x190CED1B3, 0x190D58E01, 0x190DC06B5, 0x190E23FB5, 0x190E83C9D, 0x190EE00C5, 0x190F38F43,
			0x190F8EAF7, 0x190FE168D, 0x191031483, 0x19107E729, 0x1910C90AB, 0x19111130F, 0x191157037, 0x19119A9EB, 0x1911DC1D3,
			0x19121B97F, 0x191259267, 0x191294DE9, 0x1912CED53, 0x1913071DD, 0x19133DCAF, 0x191372EDD, 0x1913A696D, 0x1913D8D5B,
			0x191409B8F, 0x1914394E9, 0x191467A3B, 0x191494C4D, 0x1914C0BDB, 0x1914EB99B, 0x191515637, 0x19153E251, 0x191565E85,
			0x19158CB65, 0x1915B297F, 0x1915D7955, 0x1915FBB69, 0x19161F033, 0x191641827, 0x1916633B5, 0x191684345, 0x1916A473F,
			0x1916C4001, 0x1916E2DE9, 0x19170114F, 0x19171EA89, 0x19173B9E7, 0x191757FB7, 0x191773C45, 0x19178EFD9, 0x1917A9AB7,
			0x1917C3D21, 0x1917DD757, 0x1917F6995, 0x19180F419, 0x191827719, 0x19183F2CB, 0x191856765, 0x19186D519, 0x191883C19,
			0x191899C8F, 0x1918AF6AF, 0x1918C4A9F, 0x1918D988B, 0x1918EE09B, 0x1919022F7, 0x191915FC5, 0x191929729, 0x19193C945,
			0x19194F63D, 0x191961E31, 0x191974141, 0x191985F8B, 0x19199792D, 0x1919A8E45, 0x1919B9EEF, 0x1919CAB45, 0x1919DB35F,
			0x1919EB75B, 0x1919FB74F, 0x191A0B353, 0x191A1AB7D, 0x191A29FE5, 0x191A390A1, 0x191A47DC5, 0x191A56767, 0x191A64D97,
			0x191A7306D, 0x191A80FF9, 0x191A8EC4D, 0x191A9C57B, 0x191AA9B95, 0x191AB6EAB, 0x191AC3ECD, 0x191AD0C0D, 0x191ADD677,
			0x191AE9E1B, 0x191AF6307, 0x191B0254B, 0x191B0E4F3, 0x191B1A20D, 0x191B25CA7, 0x191B314CB, 0x191B3CA87, 0x191B47DE7,
			0x191B52EF7, 0x191B5DDC1, 0x191B68A53, 0x191B734B3, 0x191B7DCF1, 0x191B88313, 0x191B92725, 0x191B9C931, 0x191BA6941,
			0x191BB075B, 0x191BBA38D, 0x191BC3DDB, 0x191BCD651, 0x191BD6CF7, 0x191BE01D5, 0x191BE94F1, 0x191BF2655, 0x191BFB609,
			0x191C04413, 0x191C0D07B, 0x191C15B49, 0x191C1E483, 0x191C26C2F, 0x191C2F257, 0x191C376FD, 0x191C3FA2B, 0x191C47BE7,
			0x191C4FC33, 0x191C57B1B, 0x191C5F8A1, 0x191C674CB, 0x191C6EF9F, 0x191C76925, 0x191C7E15D, 0x191C85851, 0x191C8CE05,
			0x191C9427D, 0x191C9B5BD, 0x191CA27CD, 0x191CA98AF, 0x191CB0869, 0x191CB7701, 0x191CBE477, 0x191CC50D3, 0x191CCBC19,
			0x191CD264B, 0x191CD8F6F, 0x191CDF789, 0x191CE5E9B, 0x191CEC4AB, 0x191CF29BD, 0x191CF8DD3, 0x191CFF0F3, 0x191D0531D,
			0x191D0B457, 0x191D114A5, 0x191D17409, 0x191D1D285, 0x191D2301F, 0x191D28CD9, 0x191D2E8B7, 0x191D343BB, 0x191D39DE7,
			0x191D3F741, 0x191D44FC9, 0x191D4A783, 0x191D4FE73, 0x191D55499, 0x191D5A9F9, 0x191D5FE97, 0x191D65275, 0x191D6A593,
			0x191D6F7F7, 0x191D749A3, 0x191D79A97, 0x191D7EAD7, 0x191D83A65, 0x191D88943, 0x191D8D773, 0x191D924F9, 0x191D971D7,
			0x191D9BE0D, 0x191DA099F, 0x191DA548F, 0x191DA9EDD, 0x191DAE88D, 0x191DB31A1, 0x191DB7A1B, 0x191DBC1FB, 0x191DC0945,
			0x191DC4FF9, 0x191DC961B, 0x191DCDBAB, 0x191DD20AB, 0x191DD651D, 0x191DDA905, 0x191DDEC61, 0x191DE2F33, 0x191DE717F,
			0x191DEB345, 0x191DEF487, 0x191DF3547, 0x191DF7587, 0x191DFB545
		};

		V_CONST(expDataSmallFirst, 0, 0)
		V_CONST(expDataSmallLast, 4, 0)
		static constexpr const FPUint64 expDataSmall[] = {
			0x0100000000, 0x0104080AB4, 0x0108205600, 0x010C492368, 0x011082B576, 0x0114CD4FC8, 0x0119293706, 0x011D96B0EE, 0x012216045A,
			0x0126A7793E, 0x012B4B58B2, 0x013001ECF6, 0x0134CB8170, 0x0139A862BC, 0x013E98DEAA, 0x01439D443E, 0x0148B5E3C2, 0x014DE30EC2,
			0x015325180C, 0x01587C53C4, 0x015DE91760, 0x01636BB9A8, 0x01690492CA, 0x016EB3FC54, 0x01747A513C, 0x017A57EDE8, 0x01804D3035,
			0x01865A7773, 0x018C802477, 0x0192BE99A1, 0x0199163AD5, 0x019F876D8F, 0x01A61298E1, 0x01ACB82581, 0x01B3787DC9, 0x01BA540DBB,
			0x01C14B4313, 0x01C85E8D43, 0x01CF8E5D85, 0x01D6DB26D1, 0x01DE455DF9, 0x01E5CD799D, 0x01ED73F241, 0x01F539424D, 0x01FD1DE619,
			0x0205225BEA, 0x020D47240E, 0x02158CC0D2, 0x021DF3B68C, 0x02267C8BB0, 0x022F27C8CA, 0x0237F5F88E, 0x0240E7A7E2, 0x0249FD65E2,
			0x025337C3E6, 0x025C97559A, 0x02661CB0F6, 0x026FC86E50, 0x02799B2864, 0x0283957C61, 0x028DB809E9, 0x029803732B, 0x02A2785CD9,
			0x02AD176E45, 0x02B7E15163, 0x02C2D6B2CF, 0x02CDF841E1, 0x02D946B0B3, 0x02E4C2B42D, 0x02F06D040F, 0x02FC465B01, 0x03084F7696,
			0x0314891766, 0x0320F40108, 0x032D90FA2E, 0x033A60CCA6, 0x0347644570, 0x03549C34C4, 0x0362096E24, 0x036FACC862, 0x037D871DB6,
			0x038B994BC7, 0x0399E433B7, 0x03A868BA37, 0x03B727C791, 0x03C62247B7, 0x03D5592A53, 0x03E4CD62D1, 0x03F47FE87B, 0x040471B674,
			0x0414A3CBE2, 0x0425172BE2, 0x0435CCDDAE, 0x0446C5ECA0, 0x045803684E, 0x046986648E, 0x047B4FF992, 0x048D6143F9, 0x049FBB64D1,
			0x04B25F81C3, 0x04C54EC513, 0x04D88A5DB3, 0x04EC137F61, 0x04FFEB62B1, 0x0514134520, 0x05288C6930, 0x053D581674, 0x05527799AA,
			0x0567EC44CC, 0x057DB76F26, 0x0593DA756B, 0x05AA56B9CF, 0x05C12DA417, 0x05D860A1B3, 0x05EFF125D7, 0x0607E0A98C, 0x062030ABCC,
			0x0638E2B198, 0x0651F84616, 0x066B72FA9E, 0x06855466E1, 0x069F9E28F3, 0x06BA51E577, 0x06D57147A7, 0x06F0FE017D, 0x070CF9CBC8,
			0x0729666646, 0x07464597C2, 0x0763992E34, 0x078162FED7, 0x079FA4E64B, 0x07BE60C8AD, 0x07DD9891C3, 0x07FD4E3509, 0x081D83ADDC,
			0x083E3AFF98, 0x085F7635B4, 0x08813763E5, 0x08A380A643, 0x08C6542163, 0x08E9B40281, 0x090DA27F9C, 0x093221D7A0, 0x0957345286,
			0x097CDC417A, 0x09A31BFEFB, 0x09C9F5EF0B, 0x09F16C7F49, 0x0A19822722, 0x0A423967F8, 0x0A6B94CD3E, 0x0A9596ECAF, 0x0AC0426671,
			0x0AEB99E53F, 0x0B17A01E92, 0x0B4457D2D6, 0x0B71C3CD86, 0x0B9FE6E569, 0x0BCEC3FCB1, 0x0BFE5E0135, 0x0C2EB7EC98, 0x0C5FD4C47E,
			0x0C91B79ABB, 0x0CC4638D7B, 0x0CF7DBC785, 0x0D2C23805E, 0x0D613DFC8A, 0x0D972E8DB5, 0x0DCDF892E9, 0x0E059F78CA, 0x0E3E26B9D2,
			0x0E7791DE74, 0x0EB1E47D69, 0x0EED223BDF, 0x0F294ECDB4, 0x0F666DF5BC, 0x0FA48385ED, 0x0FE3935FA5, 0x1023A173E2, 0x1064B1C390,
			0x10A6C85FBB, 0x10E9E969CB, 0x112E1913DC, 0x11735BA0EE, 0x11B9B5652D, 0x12012AC63C, 0x1249C03B78, 0x12937A4E3F, 0x12DE5D9A37,
			0x132A6ECD9A, 0x1377B2A982, 0x13C62E0237, 0x1415E5BF6E, 0x1466DEDCB0, 0x14B91E6993, 0x150CA98A16, 0x15618576F4, 0x15B7B77DF1,
			0x160F450234, 0x1668337C9E, 0x16C2887C1F, 0x171E49A60C, 0x177B7CB680, 0x17DA2780B7, 0x183A4FEF60, 0x189BFC0511, 0x18FF31DC8B,
			0x1963F7A93A, 0x19CA53B781, 0x1A324C6D28, 0x1A9BE849C7, 0x1B072DE722, 0x1B7423F99A, 0x1BE2D1509F, 0x1C533CD708, 0x1CC56D939D,
			0x1D396AA970, 0x1DAF3B585F, 0x1E26E6FD7A, 0x1EA075138B, 0x1F1BED3378, 0x1F995714D1, 0x2018BA8E3E, 0x209A1F95FF, 0x211D8E4272,
			0x21A30ECA8D, 0x222AA98660, 0x22B466EFA5, 0x23404FA23C, 0x23CE6C5CBF, 0x245EC60104, 0x24F16594B1, 0x25865441C9, 0x261D9B5748,
			0x26B74449A9, 0x275358B388, 0x27F1E25639, 0x2892EB1A65, 0x29367D10A2, 0x29DCA27223, 0x2A8565A145, 0x2B30D12A4C, 0x2BDEEFC401,
			0x2C8FCC5059, 0x2D4371DD2C, 0x2DF9EBA4E5, 0x2EB3450F2B, 0x2F6F89B1A6, 0x302EC550B0, 0x30F103E015, 0x31B65183CB, 0x327EBA90BC,
			0x334A4B8D8E, 0x341911335E, 0x34EB186E99, 0x35C06E5FC3, 0x3699205C4F
		};

		V_CONST(expDataSmallFirst2, 4, 0)
		V_CONST(expDataSmallLast2, 9, 0)
		static constexpr const FPUint64 expDataSmall2[] = {
			0x003699205C4F, 0x0037ACCCEF3B, 0x0038C5E96D7D, 0x0039E4914B19, 0x003B08E086AA, 0x003C32F3AC2E, 0x003D62E7D7BC,
			0x003E98DAB86B, 0x003FD4EA9333, 0x0041173645E0, 0x00425FDD4A16, 0x0043AEFFB86D, 0x004504BE4B80, 0x0046613A6332,
			0x0047C49607ED, 0x00492EF3EDE4, 0x004AA0777895, 0x004C1944BE1E, 0x004D99808AD9, 0x004F215064EA, 0x0050B0DA8FF1,
			0x0052484610B8, 0x0053E7BAB117, 0x00558F6103C3, 0x00573F62684E, 0x0058F7E90F39, 0x005AB91FFDFD, 0x005C83331359,
			0x005E564F0B82, 0x006032A18494, 0x006218590300, 0x006407A4F612, 0x006600B5BC9A, 0x006803BCA9A2, 0x006A10EC0942,
			0x006C28772580, 0x006E4A924B64, 0x00707772D002, 0x0072AF4F15B7, 0x0074F25E917B, 0x007740D9D046, 0x00799AFA7C9B,
			0x007C00FB6420, 0x007E73187D70, 0x0080F18EEDDD, 0x00837C9D0F7A, 0x008614827730, 0x0088B97FFAEB, 0x008B6BD7B7F4,
			0x008E2BCD196E, 0x0090F9A4DEDD, 0x0093D5A522E3, 0x0096C015621D, 0x0099B93E8219, 0x009CC16AD877, 0x009FD8E63229,
			0x00A2FFFDDADB, 0x00A63700A480, 0x00A97E3EEF08, 0x00ACD60AB031, 0x00B03EB77B8E, 0x00B3B89A8AB3, 0x00B7440AC57E,
			0x00BAE160CAA3, 0x00BE90F6F83F, 0x00C2532974BE, 0x00C6285637D2, 0x00CA10DD13A4, 0x00CE0D1FBE2A, 0x00D21D81DAB4,
			0x00D6426903AE, 0x00DA7C3CD47E, 0x00DECB66F3B5, 0x00E330531D4C, 0x00E7AB6F2D3B, 0x00EC3D2B2A26, 0x00F0E5F9505B,
			0x00F5A64E1CEB, 0x00FA7EA05914, 0x00FF6F6925DE, 0x0104792407E4, 0x01099C4EF36F, 0x010ED96A58B9, 0x011430F93080,
			0x0119A38108CF, 0x011F318A1204, 0x0124DB9F2C23, 0x012AA24DF463, 0x01308626D301, 0x013687BD095F, 0x013CA7A6C061,
			0x0142E67D1717, 0x014944DC31B8, 0x014FC36348D7, 0x015662B4B8F6, 0x015D23761266, 0x016406502966, 0x016B0BEF26A8,
			0x017235029824, 0x0179823D8231, 0x0180F456710F, 0x01888C078AAF, 0x01904A0EA0E8, 0x01982F2D4402, 0x01A03C28D592,
			0x01A871CA9BCE, 0x01B0D0DFD52F, 0x01B95A39CC78, 0x01C20EADED34, 0x01CAEF15D87D, 0x01D3FC4F7A47, 0x01DD373D1F08,
			0x01E6A0C589DB, 0x01F039D40AF8, 0x01FA035896C2, 0x0203FE47DD25, 0x020E2B9B6186, 0x02188C519323, 0x0223216DE5DE,
			0x022DEBF8EBA9, 0x0238ED006E43, 0x0244259789A8, 0x024F96D6C6E3, 0x025B41DC3772, 0x026727CB9140, 0x027349CE4B16,
			0x027FA913B9B1, 0x028C46D12D5E, 0x02992442102C, 0x02A642A804C2, 0x02B3A34B05C1, 0x02C1477985C8, 0x02CF3088902A,
			0x02DD5FD3EA36, 0x02EBD6BE352B, 0x02FA96B110D7, 0x0309A11D3EF5, 0x0318F77AC725, 0x03289B491BA5, 0x03388E0F3ECF,
			0x0348D15BE935, 0x035966C5B0A8, 0x036A4FEB2FDA, 0x037B8E732EE5, 0x038D240CCC94, 0x039F126FA87A, 0x03B15B5C0DE4,
			0x03C4009B1FA4, 0x03D703FF04AA, 0x03EA67631596, 0x03FE2CAC0B20, 0x041255C82D70, 0x0426E4AF845D, 0x043BDB6408AD,
			0x04513BF1D644, 0x0467086F5F58, 0x047D42FDA098, 0x0493EDC85671, 0x04AB0B063346, 0x04C29CF916DB, 0x04DAA5EE46AF,
			0x04F3283EA79A, 0x050C264EF874, 0x0525A2900DE9, 0x053F9F7F0F81, 0x055A1FA5B5E0, 0x0575259A8A2E, 0x0590B40126D7,
			0x05ACCD8A7977, 0x05C974F50630, 0x05E6AD0D2C41, 0x060478AD6BF2, 0x0622DABEAE01, 0x0641D6388C45, 0x06616E219BF4,
			0x0681A58FB93F, 0x06A27FA85476, 0x06C3FFA0C0AD, 0x06E628BE83F8, 0x0708FE57A933, 0x072C83D31369, 0x0750BCA8D2DF,
			0x0775AC627BD7, 0x079B569B7EF2, 0x07C1BF018371, 0x07E8E954C317, 0x0810D96867FB, 0x08399322EC2D, 0x08631A7E7B28,
			0x088D73895556, 0x08B8A266355F, 0x08E4AB4CB7A1, 0x09119289C393, 0x093F5C7FF74C, 0x096E0DA8152A, 0x099DAA917399,
			0x09CE37E26F10, 0x09FFBA58DE5D, 0x0A3236CA891E, 0x0A65B225A0B5, 0x0A9A31713B80, 0x0ACFB9CDD293, 0x0B065075C1D6,
			0x0B3DFABDCAD1, 0x0B76BE1599DD, 0x0BB0A0084E19, 0x0BEBA63D0401, 0x0C27D67762BD, 0x0C6536982C3C, 0x0CA3CC9DD035,
			0x0CE39EA501EB, 0x0D24B2E9510F, 0x0D670FC5C58A, 0x0DAABBB57E67, 0x0DEFBD5453CF, 0x0E361B5F7C52, 0x0E7DDCB63559,
			0x0EC7085A6EF2, 0x0F11A5717B0D, 0x0F5DBB44C015, 0x0FAB51426F28, 0x0FFA6EFE3DD6, 0x104B1C32238E, 0x109D60BF1ACA,
			0x10F144ADE60E, 0x1146D02FD8BD, 0x119E0B9FA3EC, 0x11F6FF822749, 0x1251B4874609, 0x12AE338AC022, 0x130C85950FBB,
			0x136CB3DC4B09, 0x13CEC7C50A8F, 0x1432CAE353FD, 0x1498C6FB89A3, 0x1500C6035E9B, 0x156AD222CFD5, 0x15D6F5B521F3,
			0x16453B49E442, 0x16B5ADA5F8B7, 0x172857C4A130, 0x179D44D89202, 0x1814804D09E7, 0x188E15C6EF82, 0x190A1125F478,
			0x19887E85BE46, 0x1A096A3F1504, 0x1A8CE0E91807, 0x1B12EF5A78A3, 0x1B9BA2AABB33, 0x1C2708337E5E, 0x1CB52D91C8DE,
			0x1D4620A75DCE, 0x1DD9EF9C17CD, 0x1E70A8DF4AD1, 0x1F0A5B292D12, 0x1FA7157C470E
		};

		V_CONST(expDataFirst, 9, 0)
		V_CONST(expDataLast, 23, 0)
		static constexpr const FPUint64 expData[] = {
			0x00001FA7157C470E, 0x0000216E8FF33787, 0x0000234FA4B27B79, 0x0000254BC42575F9, 0x00002764736D0DDA,
			0x0000299B4D89ADF6, 0x00002BF204960586, 0x00002E6A631379CA, 0x000031064D494782, 0x000033C7C2B7611F,
			0x000036B0DF9E25B3, 0x000039C3DE9C1CA9, 0x00003D031A62F2F6, 0x000040710F8508A4, 0x000044105E5CEFEA,
			0x000047E3CD10536F, 0x00004BEE49AFCDC2, 0x00005032EC7552C3, 0x000054B4FA22E2A9, 0x00005977E6835775,
			0x00005E7F570F376A, 0x000063CF25B792EE, 0x0000696B63D91144, 0x00006F585D596E62, 0x0000759A9BF1CC85,
			0x00007C36EAA85EBF, 0x00008332597C1596, 0x00008A9241451D9C, 0x0000925C47CD27BA, 0x00009A9664229E48,
			0x0000A346E32A16B1, 0x0000AC746C717F38, 0x0000B6260748BAEC, 0x0000C063202392AA, 0x0000CB338E491AA3,
			0x0000D69F99D4E6CF, 0x0000E2B0020EA7A8, 0x0000EF6E041D0A6C, 0x0000FCE36218FE4A, 0x00010B1A6A86C978,
			0x00011A1E003AA770, 0x000129F9A2AEFB87, 0x00013AB976D27A6C, 0x00014C6A505508DA, 0x00015F19BB7A6EA1,
			0x000172D6077A63FA, 0x000187AE5175EB34, 0x00019DB2900A5AF3, 0x0001B4F39F8AF647, 0x0001CD834EEA6F5A,
			0x0001E7746D5E37EA, 0x000202DAD8C61179, 0x00021FCB8CE2E50F, 0x00023E5CB368895B, 0x00025EA5B4F6C593,
			0x000280BF4B06908A, 0x0002A4C392D94689, 0x0002CACE217855E6, 0x0002F2FC18D4B33C, 0x00031D6C3E164240,
			0x00034A3F112C4910, 0x00037996E5B0F9F1, 0x0003AB97FD33228F, 0x0003E068A2FA22DF, 0x0004183149596E98,
			0x0004531CA8AA0F03, 0x00049157E001DD5B, 0x0004D31297C183E5, 0x0005187F2613BBF4, 0x000561D2B579BC29,
			0x0005AF456D825C44, 0x000601129DCB1CDD, 0x00065778EB6C01AD, 0x0006B2BA80F10773, 0x0007131D4105F2A0,
			0x000778EAFBFB44E7, 0x0007E471A84D55EF, 0x000856039E58D9AD, 0x0008CDF7D76A8D1D, 0x00094CAA305A54CE,
			0x0009D27BAFE4CF11, 0x000A5FD2D0F939C3, 0x000AF51BD1349335, 0x000B92C903C6100A, 0x000C395328FC6130,
			0x000CE939CABED641, 0x000DA3039E392DB1, 0x000E673EEB04DF03, 0x000F3681F81EDFB1, 0x0010116B7EFD5444,
			0x0010F8A3251D4FE6, 0x0011ECD9FC65B8C7, 0x0012EECB0AC1A368, 0x0013FF3BD959FD9B, 0x00151EFD0BDC3852,
			0x00164EEB0041C9BC, 0x00178FEE7792E44A, 0x0018E2FD48259FE0, 0x001A491B19E21650, 0x001BC35A2D1BA06A,
			0x001D52DC2C977BB8, 0x001EF8D30B61B03A, 0x0020B681EF1A17EB, 0x00228D3E276CF694, 0x00247E703374A64C,
			0x00268B94D5CC8429, 0x0028B63E38288928, 0x002B00151F50DDA0, 0x002D6ADA306D42D4, 0x002FF86748997058,
			0x0032AAB0E7C983AB, 0x003583C7B0146A95, 0x003885D9FA89D007, 0x003BB33582C9930F, 0x003F0E492AA43CB4,
			0x004299A6D71E51B5, 0x004658056843CDE0, 0x004A4C42CD4D9FAD, 0x004E796636B0AACC, 0x0052E2A267C4C078,
			0x00578B5829CA2E80, 0x005C7718E22E13F9, 0x0061A9A94E08A818, 0x0067270464ED2240, 0x006CF35E65410E54,
			0x007313280C70A674, 0x00798B11FD776285, 0x0080601058576850, 0x0087975E85400101, 0x008F3683364ACC88,
			0x00974354A2E13A09, 0x009FC3FD0007F400, 0x00A8BEFF38FC9188, 0x00B23B3BEBC320C9, 0x00BC3FF6AD752531,
			0x00C6D4DB9A5A97A1, 0x00D20205360F81D8, 0x00DDD002A0361A71, 0x00EA47DE22760888, 0x00F773241CCDD768,
			0x01055BEA5583E000, 0x01140CD7B25037B1, 0x0123912C60A9C8D0, 0x0133F4CA7375D6C0, 0x0145443EFCB312D1,
			0x01578CCBAA185B91, 0x016ADC70EC0387A1, 0x017F41F8AE7E8A11, 0x0194CD01AC911EA1, 0x01AB8E0B668B5580,
			0x01C39682C47225C1, 0x01DCF8CF6E3AFEF0, 0x01F7C861E40E8FA1, 0x021419C2615E0280, 0x023202A096314A41,
			0x025199E442B91EE0, 0x0272F7BEC1DC9A81, 0x029635BD90316301, 0x02BB6EDDDD8E9F01, 0x02E2BFA1383720A0,
			0x030C4623616ED2C0, 0x033822315C333DC0, 0x03667561C7BF8921, 0x0397632E988276E1, 0x03CB111043393700,
			0x0401A69A6EFD68C0, 0x043B4D9A44400080, 0x047832366FE70880, 0x04B88310F3122CC1, 0x04FC716AD96ACDC1,
			0x05443149F158EC81, 0x058FF9A0A2FFD101, 0x05E004780484F540, 0x06348F1C4BDACCC0, 0x068DDA4BC018AEC1,
			0x06EC2A684E52FAC0, 0x074FC7ABE7EBD441, 0x07B8FE5FD2773DC1, 0x08281F17138E7100, 0x089D7EEC254FD900,
			0x091977C222CCEB00, 0x099C68899E4F0B81, 0x0A26B589562AE780, 0x0AB8C8AAFFD04B01, 0x0B5311CC63E5A000,
			0x0BF60715098C9E00, 0x0CA22550B26B6D81, 0x0D57F04EECC5B001, 0x0E17F34804D5E580, 0x0EE2C147A2B4F081,
			0x0FB8F59D6675CD80, 0x109B3453D8B28401, 0x118A2AAE0AA05200, 0x12868FAC45DFBC00, 0x1391249831A59B00,
			0x14AAB598D8908001, 0x15D41A4F00860600, 0x170E367A4C531D00, 0x1859FAA7A584FC01, 0x19B864E9740C5F00,
			0x1B2A819A30BEAE00, 0x1CB16C29E7B69E01, 0x1E4E4FF747F64500, 0x20026934E6852801, 0x21CF05DB649CD400,
			0x23B586A932551E00, 0x25B76030B1AD8401, 0x27D61BF588D3C601, 0x2A135999FE289601, 0x2C70D01D44CB9A00,
			0x2EF04F2BAD70D401, 0x3193C081BCF55C01, 0x345D296338A1EE00, 0x374EAC2747532800, 0x3A6A89DAD6E40401,
			0x3DB323FA8644E001, 0x412AFE4566ABA400, 0x44D4C0A9F94FF001, 0x48B3394EE33D9801, 0x4CC95EB8E6008401,
			0x511A520FC2574001, 0x55A96183C1C62401, 0x5A7A0AD5BCE67C01, 0x5F8FFE038EB60801, 0x64EF201B01109C00,
			0x6A9B8E355BF44C00, 0x7099A09DE04F2001, 0x76EDEE2597F39401, 0x7D9D4FA70708E800, 0x84ACE3BC6FF70800,
			0x8C2212AB81885801, 0x940292896FE3E000, 0x9C546B9AA436B801, 0xA51DFCF15C85D800, 0xAE66014EC5547801,
			0xB833944A49B3C800, 0xC28E37C30C157000, 0xCD7DD99FB305D801, 0xD90AD9E0F0EE7001, 0xE53E110B6D5C3800,
			0xF220D6EDF9312801, 0xFFBD09C93DD51800, 0x0E1D15DE6221D000, 0x1D4BFD6A6E9F8000, 0x2D5561148DED1001,
			0x3E4588D59E288001, 0x50296D5FE5108001, 0x630EC20E1ABC7000, 0x7703FF6166699000, 0x8C186E1656575000,
			0xA25C32D94F4AE000, 0xB9E05AA36A712000, 0xD2B6E7C939549000, 0xECF2DFC56FD76000, 0x08A859CA04092000,
			0x25EC8E22EBAA8001, 0x44D5E6763FB82000,
		};

		static const constexpr CombinedType CombineTypes(Type t1, Type t2) {
			return static_cast<CombinedType>((static_cast<char>(t1) << 3) | static_cast<char>(t2));
		}

		explicit FixedPointT(FPUint64 arData, bool, bool, bool) : type(Type::Positive), value(FPT(arData)) {
			// For constructing raw instances
		}

	public:

		constexpr FixedPointT() : type(Type::Zero), value(0) {
			static_assert(&(reinterpret_cast<FixedPointT*>(0)->type) == &(reinterpret_cast<FixedPointWrap*>(0)->type), "Invalid offset");
			static_assert(&(reinterpret_cast<FixedPointT*>(0)->value) == &(reinterpret_cast<FixedPointWrap*>(0)->value), "Invalid offset");
			static_assert(sizeof(reinterpret_cast<FixedPointT*>(0)->value) == sizeof(reinterpret_cast<FixedPointWrap*>(0)->value), "Invalid value type");
			static_assert(sizeof(reinterpret_cast<FixedPointT*>(0)->type) == sizeof(reinterpret_cast<FixedPointWrap*>(0)->type), "Invalid value type");
		}

		constexpr FixedPointT(const FixedPointT& another) : type(another.type), value(another.value) {

		}

		explicit FixedPointT(FPInt32 value) : type(value < 0 ? Type::Negative : value == 0 ? Type::Zero : Type::Positive), value(FromWholeValue((FPUint32)(value < 0 ? -value : value))) {

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

				FPUint128 result = (x * y) >> FPUint128(32);
				if (result.GetHigh() > 0) {
					EventHandler::Overflow();
					return Inf();
				}

#ifdef USE_FPM
				FPT res = FPT::from_raw_value(static_cast<FPUint64>(result));
#else
				FPT res = result.GetLow();
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
				bool inf = false;
				FPT res = FPT_DIV(value, another.value, inf);
				if (inf) {
					EventHandler::Overflow();
					return Inf();
				} else {
					return FixedPointT(neg, res);
				}
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
					return FixedPointT(false, fpm::sin(tmp.value));
#else
					FPT val = tableValue(fp.value, sinData, sinDataFirst().value, sinDataLast().value);
					return FixedPointT(TypeFromFPT(val), val);
#endif	
				} else if (fp == HALF_PI()) {
					return ONE();
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
			{
				if (fp > ONE()) {
					return NaN();
				}
				FPT res = tableValue(fp.value, asinData, asinDataFirst().value, asinDataLast().value);
				return FixedPointT(TypeFromFPT(res), res);
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

		static constexpr FixedPointT atan(FixedPointT fp) {
			switch (fp.type) {

			case Type::Positive:
			{
				if (fp < atanSmallDataLast()) {
					FPT res = tableValue(fp.value, atanSmallData, atanSmallDataFirst().value, atanSmallDataLast().value);
					return FixedPointT(TypeFromFPT(res), res);
				} else if (fp < atanDataLast()) {
					FPT res = tableValue(fp.value, atanData, atanDataFirst().value, atanDataLast().value);
					return FixedPointT(TypeFromFPT(res), res);
				}
				FixedPointT fpfp = fp * fp;
				return asin(fp / sqrt(fpfp + ONE()));
			}
			case Type::Negative:
				return -atan(-fp);

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
			//const FixedPointT sqrtDataLastF = sqrtDataLast();
			//const FixedPointT sqrtSmallDataLastF = sqrtSmallDataLast();
			//const FixedPointT sqrtSmallDataFirstF = sqrtSmallDataFirst();
			//const FixedPointT sqrtSmallDataLast2F = sqrtSmallDataLast2();
			//const FixedPointT sqrtSmallDataFirst2F = sqrtSmallDataFirst2();
			//const FixedPointT sqrtDataFirstF = sqrtDataFirst();
			//const FixedPointT TWOF = TWO();

			switch (fp.type) {

			case Type::Positive:
			{
#ifdef USE_FPM
				return FixedPoint(false, fpm::sqrt(fp.value));
#else

				if (SqrtIterationsCount > 0) {
					if (fp.value < sqrtDataLast().value) {

						FPT x1;
						FPT x2;
						FPT mid;
						if (fp.value <= sqrtSmallDataLast().value) {
							mid = tableValue(fp.value, sqrtSmallData, sqrtSmallDataFirst().value, sqrtSmallDataLast().value, x1, x2);
						} else if (fp.value <= sqrtSmallDataLast2().value) {
							mid = tableValue(fp.value, sqrtSmallData2, sqrtSmallDataFirst2().value, sqrtSmallDataLast2().value, x1, x2);
						} else {
							mid = tableValue(fp.value, sqrtData, sqrtDataFirst().value, sqrtDataLast().value, x1, x2);
						}
						if (mid == x1 || mid == x2) {
							return FixedPointT(mid, false, false, false);
						}
						for (FPUint32 i = 0; x2 > x1 && i < SqrtIterationsCount; i++) {
							mid = x1 + ((x2 - x1) >> 1);
							FPT mid2;
							if (mid >= 0xFFFFFFFF) {
								// Note: Ideal multiplication would shift 32 to the right, BUT there is an overflow for numbers above sqrt(64bit max).
								//       Shifting 16 places eradicates the issue at the cost of lowest 16 decimal bits
								FPUint128 mid128{ mid };
								FPUint128 res128 = mid128 * mid128;
								//FPT mid3 = mid >> 16;
								//mid2 = (mid3 * mid3);
								res128 = res128 >> FPUint128(32);
								mid2 = res128.GetLow();
							} else {
								mid2 = (mid * mid) >> 32;
							}
							//printf("X1: %d, X2: %d, MID: %d, MID2: %d\n", (FPUint32)(x1 >> 32), (FPUint32)(x2 >> 32), (FPUint32)(mid >> 32), (FPUint32)(mid2 >> 32));
							if (mid2 > fp.value && x2 != mid) {
								x2 = mid;
							} else if (mid2 < fp.value && x1 != mid) {
								x1 = mid;
							} else {
								break;
							}
						}
						return FixedPointT(mid, false, false, false);;
					}
				} else {

				}

				// Finding the square root of an integer in base-2, from:
				// https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29

				// Shift by F first because it's fixed-point.
				FPUint128 num = FPUint128(fp.value) << FPUint128(32);
				FPUint128 res(0);

				// "bit" starts at the greatest power of four that's less than the argument.
				const FPInt32 highestBit = HighestBitPositions::GetHighestBitPosition<EventHandler>(fp.value);

				FPUint128 bit1 = FPUint128(1);
				bit1.ShiftLeft((highestBit + 32) & FPUint64(~1));

				const FPUint128 ZERO{ 0 };
				const FPUint128 TWO{ 2 };

				//<< ((HighestBitPositions::GetHighestBitPosition<EventHandler>(fp.value) + 32) & FPUint64(~1));
				for (FPUint128 bit = bit1; bit != ZERO; bit >>= TWO) {
					const FPUint128 val = res + bit;
					res >>= FPUint128(1);
					if (num >= val) {
						num -= val;
						res += bit;
					}
				}

				// Round the last digit up if necessary
				if (num > res) {
					res++;
				}
				return FixedPointT(Type::Positive, FPT(res.GetLow()));
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
				if (GetWholeValueForTable(fp.value) > 19) {
					EventHandler::Overflow();
					return NaN();
				}
#ifdef USE_FPM
				return FixedPoint(false, fpm::exp(fp.value));
#else
				if (ExpIterationsCount == 0) {
					if (fp <= expDataSmallLast()) {
						FPT res = tableValue(fp.value, expDataSmall, expDataSmallFirst().value, expDataSmallLast().value);
						return FixedPointT(TypeFromFPT(res), res);
					} else if (fp <= expDataSmallLast2()) {
						FPT res = tableValue(fp.value, expDataSmall2, expDataSmallFirst2().value, expDataSmallLast2().value);
						return FixedPointT(TypeFromFPT(res), res);
					} else {
						FPT res = tableValue(fp.value, expData, expDataFirst().value, expDataLast().value);
						return FixedPointT(TypeFromFPT(res), res);
					}
				} else {
					FixedPointT term = ONE();
					FixedPointT result = ONE();
					for (FPUint32 i = 1; i < ExpIterationsCount; i++) {
						FixedPointT order = FixedPointT(i);
						term *= fp / order;
						result += term;
						char tmp[64];
						result.PrintValue(tmp, sizeof(tmp));
						printf("%d: %s\n", i, tmp);
					}
					return result;
				}
#endif

			case Type::Negative:
				return ONE() / exp(-fp);
			}

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

		V_CONST(ZERO, 0, 0)
		V_CONST(ONE, 1, 0)
		V_CONST(TWO, 2, 0)
		V_CONST(THREE, 3, 0)
		V_CONST(FOUR, 4, 0)
		V_CONST(FIVE, 5, 0)
		V_CONST(SIX, 6, 0)
		V_CONST(SEVEN, 7, 0)
		V_CONST(EIGHT, 8, 0)
		V_CONST(NINE, 9, 0)
		V_CONST(TEN, 10, 0)

		V_ENC(E, 0x00000002B7E15163)
		V_ENC(PI, 0x00000003243F6A85)
		V_ENC(TWO_PI, 0x00000006487ED50A)
		V_ENC(HALF_PI, 0x00000001921FB544)
		V_ENC(EPSILON, 1)

#pragma push_macro("MIN")
#ifdef MIN
#undef MIN
#endif
			V_ENC(MIN, 0xFFFFFFFFFFFFFFFF, true)
			V_ENC(MIN_, 0xFFFFFFFFFFFFFFFF, true)
#pragma pop_macro("MIN")

#pragma push_macro("MAX")
#ifdef MAX
#undef MAX
#endif
			V_ENC(MAX, 0xFFFFFFFFFFFFFFFF)
			V_ENC(MAX_, 0xFFFFFFFFFFFFFFFF)
#pragma pop_macro("MAX")

#pragma pop_macro("V_ENC")
#pragma pop_macro("V_CONST")

	private:

		static constexpr const inline FPUint32 GetWholeValueForTable(FixedPointT& fp) {
			bool negative = false;
			return fp.GetWholeValue(negative);
		}

		static constexpr const inline FPUint32 GetWholeValueForTable(FPT& fp) {
			return FPUint32(fp >> 32);
		}

		template<typename TableT>
		static constexpr const inline FixedPointT CreateFromTableValue(TableT value, FixedPointT&) {
			return FixedPointT(static_cast<FPUint64>(value), false, false, false);
		}

		template<typename TableT>
		static constexpr const inline FPT CreateFromTableValue(TableT value, FPT&) {
#ifdef USE_FPM
#error Not implemented
#endif
			return FPT(value);
		}

		// functions for internal use
		template<size_t N, typename TableT>
		static constexpr inline FPT tableValue(FPT& fp, const TableT(&table)[N], FPT first, FPT last) {
			static_assert(N == 257, "Invalid table size");
			FPT x1;
			FPT x2;
			return tableValue(fp, table, first, last, x1, x2);
		}

		template<size_t N, typename TableT>
		static constexpr inline FPT tableValue(FPT& fp, const TableT(&table)[N], FPT first, FPT last, FPT& x1, FPT& x2) {
			static_assert(N == 257, "Invalid table size");

			constexpr const FPUint32 sz = N - 1;
			const FPT diff = (last - first) >> 8;
			FPT dataIndexTMP = (fp - first) / diff;
			FPUint32 dataIndex = static_cast<FPUint32>(dataIndexTMP);
			FPT sdi(dataIndex);
			x1 = CreateFromTableValue(table[dataIndex], x1);
			x2 = CreateFromTableValue(table[dataIndex + 1], x1);
			const FPT x2x1 = x2 > x1 ? x2 - x1 : x1 - x2;

			const FPT part1 = x1;
			const FPT part2 = (fp * x2x1) / diff;
			const FPT part3 = sdi * x2x1;
			const FPT part4 = (first * x2x1) / diff;

			const FPT part12 = part1 + part2;
			const FPT part34 = part3 + part4;

			const FPT result = part12 - part34;

			if (part34 > part12 || (part34 < part12 && (result > x2 || result < x1))) {
				const FPT d1 = first + (sdi * diff);
				const FPT perc = (10000 * (fp - d1)) / diff;
				const FPT res = x1 + ((x2x1 * perc) / 10000);
				return res;
			}
			return result;
			//FPT result = x1 + (fp * (x2x1) / diff) - sdi * (x2x1)-(first * (x2x1) / diff);
		}

	private:
		Type type;
		FPT value;
	};
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

#define float Numero::FixedPointT<FP_DEF>
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

typedef Numero::FixedPointT<FP_DEF> DefaultFixedPoint;

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

#endif
