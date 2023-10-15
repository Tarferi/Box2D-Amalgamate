#pragma once
#define BOX2D_USE_FIXED_POINT
#define BOX2D_EXTERNAL

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


// FPM
namespace fpm {

	//MIT License
	//
	// Copyright (c) 2019 Mike Lankamp
	// 
	// Permission is hereby granted, free of charge, to any person obtaining a copy
	// of this software and associated documentation files (the "Software"), to deal
	// in the Software without restriction, including without limitation the rights
	// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	// copies of the Software, and to permit persons to whom the Software is
	// furnished to do so, subject to the following conditions:
	// 
	// The above copyright notice and this permission notice shall be included in all
	// copies or substantial portions of the Software.
	// 
	// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	// SOFTWARE.

	//! Fixed-point number type
	//! \tparam BaseType         the base integer type used to store the fixed-point number. This can be a signed or unsigned type.
	//! \tparam IntermediateType the integer type used to store intermediate results during calculations.
	//! \tparam FractionBits     the number of bits of the BaseType used to store the fraction
	//! \tparam EnableRounding   enable rounding of LSB for multiplication, division, and type conversion
	template <typename BaseType, typename IntermediateType, unsigned int FractionBits, bool EnableRounding = true>
	class fixed {
		static_assert(std::is_integral<BaseType>::value, "BaseType must be an integral type");
		static_assert(FractionBits > 0, "FractionBits must be greater than zero");
		static_assert(FractionBits <= sizeof(BaseType) * 8 - 1, "BaseType must at least be able to contain entire fraction, with space for at least one integral bit");
		static_assert(sizeof(IntermediateType) > sizeof(BaseType), "IntermediateType must be larger than BaseType");
		static_assert(std::is_signed<IntermediateType>::value == std::is_signed<BaseType>::value, "IntermediateType must have same signedness as BaseType");

		// Although this value fits in the BaseType in terms of bits, if there's only one integral bit, this value
		// is incorrect (flips from positive to negative), so we must extend the size to IntermediateType.
		static constexpr IntermediateType FRACTION_MULT = IntermediateType(1) << FractionBits;

		struct raw_construct_tag {};
		constexpr inline fixed(BaseType val, raw_construct_tag) noexcept : m_value(val) {}

	public:
		inline fixed() noexcept = default;

		// Converts an integral number to the fixed-point type.
		// Like static_cast, this truncates bits that don't fit.
		template <typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
		constexpr inline explicit fixed(T val) noexcept
			: m_value(static_cast<BaseType>(FRACTION_MULT * val)) {
		}

		// Converts an floating-point number to the fixed-point type.
		// Like static_cast, this truncates bits that don't fit.
		template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
		constexpr inline explicit fixed(T val) noexcept
			: m_value(static_cast<BaseType>((EnableRounding) ?
				(val >= 0.0) ? (val * FRACTION_MULT + T{ 0.5 }) : (val * FRACTION_MULT - T{ 0.5 })
				: (val * FRACTION_MULT))) {
		}

		// Constructs from another fixed-point type with possibly different underlying representation.
		// Like static_cast, this truncates bits that don't fit.
		template <typename B, typename I, unsigned int F, bool R>
		constexpr inline explicit fixed(fixed<B, I, F, R> val) noexcept
			: m_value(from_fixed_point<F>(val.raw_value()).raw_value()) {
		}

		// Explicit conversion to a floating-point type
		template <typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
		constexpr inline explicit operator T() const noexcept {
			return static_cast<T>(static_cast<IntermediateType>(m_value) / FRACTION_MULT);
		}

		// Explicit conversion to an integral type
		template <typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
		constexpr inline explicit operator T() const noexcept {
			return static_cast<T>(static_cast<BaseType>(static_cast<IntermediateType>(m_value) / FRACTION_MULT));
		}

		// Returns the raw underlying value of this type.
		// Do not use this unless you know what you're doing.
		constexpr inline BaseType raw_value() const noexcept {
			return m_value;
		}

		//! Constructs a fixed-point number from another fixed-point number.
		//! \tparam NumFractionBits the number of bits used by the fraction in \a value.
		//! \param value the integer fixed-point number
		template <unsigned int NumFractionBits, typename T, typename std::enable_if<(NumFractionBits > FractionBits)>::type* = nullptr>
		static constexpr inline fixed from_fixed_point(T value) noexcept {
			// To correctly round the last bit in the result, we need one more bit of information.
			// We do this by multiplying by two before dividing and adding the LSB to the real result.
			return (EnableRounding) ? fixed(static_cast<BaseType>(
				value / (T(1) << (NumFractionBits - FractionBits)) +
				(value / (T(1) << (NumFractionBits - FractionBits - 1)) % 2)),
				raw_construct_tag{}) :
				fixed(static_cast<BaseType>(value / (T(1) << (NumFractionBits - FractionBits))),
					raw_construct_tag{});
		}

		template <unsigned int NumFractionBits, typename T, typename std::enable_if<(NumFractionBits <= FractionBits)>::type* = nullptr>
		static constexpr inline fixed from_fixed_point(T value) noexcept {
			return fixed(static_cast<BaseType>(
				value * (T(1) << (FractionBits - NumFractionBits))),
				raw_construct_tag{});
		}

		// Constructs a fixed-point number from its raw underlying value.
		// Do not use this unless you know what you're doing.
		static constexpr inline fixed from_raw_value(BaseType value) noexcept {
			return fixed(value, raw_construct_tag{});
		}

		//
		// Constants
		//
		static constexpr fixed e() { return from_fixed_point<61>(6267931151224907085ll); }
		static constexpr fixed pi() { return from_fixed_point<61>(7244019458077122842ll); }
		static constexpr fixed half_pi() { return from_fixed_point<62>(7244019458077122842ll); }
		static constexpr fixed two_pi() { return from_fixed_point<60>(7244019458077122842ll); }

		//
		// Arithmetic member operators
		//

		//constexpr inline fixed operator-() const noexcept {
		//	return fixed::from_raw_value(-m_value);
		//}

		inline fixed& operator+=(const fixed& y) noexcept {
			m_value += y.m_value;
			return *this;
		}

		template <typename I, typename std::enable_if<std::is_integral<I>::value>::type* = nullptr>
		inline fixed& operator+=(I y) noexcept {
			m_value += static_cast<BaseType>(FRACTION_MULT * static_cast<IntermediateType>(y));
			return *this;
		}

		inline fixed& operator-=(const fixed& y) noexcept {
			m_value -= y.m_value;
			return *this;
		}

		template <typename I, typename std::enable_if<std::is_integral<I>::value>::type* = nullptr>
		inline fixed& operator-=(I y) noexcept {
			m_value -= static_cast<BaseType>(FRACTION_MULT * static_cast<IntermediateType>(y));
			return *this;
		}

		inline fixed& operator*=(const fixed& y) noexcept {
			if (EnableRounding) {
				// Normal fixed-point multiplication is: x * y / 2**FractionBits.
				// To correctly round the last bit in the result, we need one more bit of information.
				// We do this by multiplying by two before dividing and adding the LSB to the real result.
				auto value = (static_cast<IntermediateType>(m_value) * y.m_value) / (FRACTION_MULT / 2);
				m_value = static_cast<BaseType>((value / 2) + (value % 2));
			} else {
				auto value = (static_cast<IntermediateType>(m_value) * y.m_value) / FRACTION_MULT;
				m_value = static_cast<BaseType>(value);
			}
			return *this;
		}

		template <typename I, typename std::enable_if<std::is_integral<I>::value>::type* = nullptr>
		inline fixed& operator*=(I y) noexcept {
			m_value *= y;
			return *this;
		}

		inline fixed& operator/=(const fixed& y) noexcept {
			assert(y.m_value != 0);
			if (EnableRounding) {
				// Normal fixed-point division is: x * 2**FractionBits / y.
				// To correctly round the last bit in the result, we need one more bit of information.
				// We do this by multiplying by two before dividing and adding the LSB to the real result.
				auto value = (static_cast<IntermediateType>(m_value) * FRACTION_MULT * 2) / y.m_value;
				m_value = static_cast<BaseType>((value / 2) + (value % 2));
			} else {
				auto value = (static_cast<IntermediateType>(m_value) * FRACTION_MULT) / y.m_value;
				m_value = static_cast<BaseType>(value);
			}
			return *this;
		}

		template <typename I, typename std::enable_if<std::is_integral<I>::value>::type* = nullptr>
		inline fixed& operator/=(I y) noexcept {
			m_value /= y;
			return *this;
		}

	private:
		BaseType m_value;
	};

	//
	// Convenience typedefs
	//

	using fixed_16_16 = fixed<std::int32_t, std::int64_t, 16>;
	using fixed_24_8 = fixed<std::int32_t, std::int64_t, 8>;
	using fixed_8_24 = fixed<std::int32_t, std::int64_t, 24>;

	//
	// Addition
	//

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline fixed<B, I, F, R> operator+(const fixed<B, I, F, R>& x, const fixed<B, I, F, R>& y) noexcept {
		return fixed<B, I, F, R>(x) += y;
	}

	template <typename B, typename I, unsigned int F, bool R, typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	constexpr inline fixed<B, I, F, R> operator+(const fixed<B, I, F, R>& x, T y) noexcept {
		return fixed<B, I, F, R>(x) += y;
	}

	template <typename B, typename I, unsigned int F, bool R, typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	constexpr inline fixed<B, I, F, R> operator+(T x, const fixed<B, I, F, R>& y) noexcept {
		return fixed<B, I, F, R>(y) += x;
	}

	//
	// Subtraction
	//

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline fixed<B, I, F, R> operator-(const fixed<B, I, F, R>& x, const fixed<B, I, F, R>& y) noexcept {
		return fixed<B, I, F, R>(x) -= y;
	}

	template <typename B, typename I, unsigned int F, bool R, typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	constexpr inline fixed<B, I, F, R> operator-(const fixed<B, I, F, R>& x, T y) noexcept {
		return fixed<B, I, F, R>(x) -= y;
	}

	template <typename B, typename I, unsigned int F, bool R, typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	constexpr inline fixed<B, I, F, R> operator-(T x, const fixed<B, I, F, R>& y) noexcept {
		return fixed<B, I, F, R>(x) -= y;
	}

	//
	// Multiplication
	//

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline fixed<B, I, F, R> operator*(const fixed<B, I, F, R>& x, const fixed<B, I, F, R>& y) noexcept {
		return fixed<B, I, F, R>(x) *= y;
	}

	template <typename B, typename I, unsigned int F, bool R, typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	constexpr inline fixed<B, I, F, R> operator*(const fixed<B, I, F, R>& x, T y) noexcept {
		return fixed<B, I, F, R>(x) *= y;
	}

	template <typename B, typename I, unsigned int F, bool R, typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	constexpr inline fixed<B, I, F, R> operator*(T x, const fixed<B, I, F, R>& y) noexcept {
		return fixed<B, I, F, R>(y) *= x;
	}

	//
	// Division
	//

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline fixed<B, I, F, R> operator/(const fixed<B, I, F, R>& x, const fixed<B, I, F, R>& y) noexcept {
		return fixed<B, I, F, R>(x) /= y;
	}

	template <typename B, typename I, unsigned int F, typename T, bool R, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	constexpr inline fixed<B, I, F, R> operator/(const fixed<B, I, F, R>& x, T y) noexcept {
		return fixed<B, I, F, R>(x) /= y;
	}

	template <typename B, typename I, unsigned int F, typename T, bool R, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	constexpr inline fixed<B, I, F, R> operator/(T x, const fixed<B, I, F, R>& y) noexcept {
		return fixed<B, I, F, R>(x) /= y;
	}

	//
	// Comparison operators
	//

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool operator==(const fixed<B, I, F, R>& x, const fixed<B, I, F, R>& y) noexcept {
		return x.raw_value() == y.raw_value();
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool operator!=(const fixed<B, I, F, R>& x, const fixed<B, I, F, R>& y) noexcept {
		return x.raw_value() != y.raw_value();
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool operator<(const fixed<B, I, F, R>& x, const fixed<B, I, F, R>& y) noexcept {
		return x.raw_value() < y.raw_value();
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool operator>(const fixed<B, I, F, R>& x, const fixed<B, I, F, R>& y) noexcept {
		return x.raw_value() > y.raw_value();
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool operator<=(const fixed<B, I, F, R>& x, const fixed<B, I, F, R>& y) noexcept {
		return x.raw_value() <= y.raw_value();
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool operator>=(const fixed<B, I, F, R>& x, const fixed<B, I, F, R>& y) noexcept {
		return x.raw_value() >= y.raw_value();
	}

	namespace detail {
		// Number of base-10 digits required to fully represent a number of bits
		static constexpr int max_digits10(int bits) {
			// 8.24 fixed-point equivalent of (int)ceil(bits * std::log10(2));
			using T = long long;
			return static_cast<int>((T{ bits } *5050445 + (T{ 1 } << 24) - 1) >> 24);
		}

		// Number of base-10 digits that can be fully represented by a number of bits
		static constexpr int digits10(int bits) {
			// 8.24 fixed-point equivalent of (int)(bits * std::log10(2));
			using T = long long;
			return static_cast<int>((T{ bits } *5050445) >> 24);
		}

	} // namespace detail
} // namespace fpm

// Specializations for customization points
namespace std {
#pragma push_macro("min")
#ifdef min
#undef min
#endif
#pragma push_macro("max")
#ifdef max
#undef max
#endif

	//MIT License
	//
	// Copyright (c) 2019 Mike Lankamp
	// 
	// Permission is hereby granted, free of charge, to any person obtaining a copy
	// of this software and associated documentation files (the "Software"), to deal
	// in the Software without restriction, including without limitation the rights
	// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	// copies of the Software, and to permit persons to whom the Software is
	// furnished to do so, subject to the following conditions:
	// 
	// The above copyright notice and this permission notice shall be included in all
	// copies or substantial portions of the Software.
	// 
	// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	// SOFTWARE.

	template <typename B, typename I, unsigned int F, bool R>
	struct hash<fpm::fixed<B, I, F, R>> {
		using argument_type = fpm::fixed<B, I, F, R>;
		using result_type = std::size_t;

		result_type operator()(argument_type arg) const noexcept(noexcept(std::declval<std::hash<B>>()(arg.raw_value()))) {
			return m_hash(arg.raw_value());
		}

	private:
		std::hash<B> m_hash;
	};

	template <typename B, typename I, unsigned int F, bool R>
	struct numeric_limits<fpm::fixed<B, I, F, R>> {
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = std::numeric_limits<B>::is_signed;
		static constexpr bool is_integer = false;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr std::float_round_style round_style = std::round_to_nearest;
		static constexpr bool is_iec_559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = std::numeric_limits<B>::is_modulo;
		static constexpr int digits = std::numeric_limits<B>::digits;

		// Any number with `digits10` significant base-10 digits (that fits in
		// the range of the type) is guaranteed to be convertible from text and
		// back without change. Worst case, this is 0.000...001, so we can only
		// guarantee this case. Nothing more.
		static constexpr int digits10 = 1;

		// This is equal to max_digits10 for the integer and fractional part together.
		static constexpr int max_digits10 =
			fpm::detail::max_digits10(std::numeric_limits<B>::digits - F) + fpm::detail::max_digits10(F);

		static constexpr int radix = 2;
		static constexpr int min_exponent = 1 - F;
		static constexpr int min_exponent10 = -fpm::detail::digits10(F);
		static constexpr int max_exponent = std::numeric_limits<B>::digits - F;
		static constexpr int max_exponent10 = fpm::detail::digits10(std::numeric_limits<B>::digits - F);
		static constexpr bool traps = true;
		static constexpr bool tinyness_before = false;

		static constexpr fpm::fixed<B, I, F, R> lowest() noexcept {
			return fpm::fixed<B, I, F, R>::from_raw_value(std::numeric_limits<B>::lowest());
		};

		static constexpr fpm::fixed<B, I, F, R> min() noexcept {
			return lowest();
		}

		static constexpr fpm::fixed<B, I, F, R> max() noexcept {
			return fpm::fixed<B, I, F, R>::from_raw_value(std::numeric_limits<B>::max());
		};

		static constexpr fpm::fixed<B, I, F, R> epsilon() noexcept {
			return fpm::fixed<B, I, F, R>::from_raw_value(1);
		};

		static constexpr fpm::fixed<B, I, F, R> round_error() noexcept {
			return fpm::fixed<B, I, F, R>(1) / 2;
		};

		static constexpr fpm::fixed<B, I, F, R> denorm_min() noexcept {
			return min();
		}
	};

	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::is_specialized;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::is_signed;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::is_integer;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::is_exact;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::has_infinity;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::has_quiet_NaN;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::has_signaling_NaN;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr std::float_denorm_style numeric_limits<fpm::fixed<B, I, F, R>>::has_denorm;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::has_denorm_loss;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr std::float_round_style numeric_limits<fpm::fixed<B, I, F, R>>::round_style;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::is_iec_559;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::is_bounded;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::is_modulo;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr int numeric_limits<fpm::fixed<B, I, F, R>>::digits;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr int numeric_limits<fpm::fixed<B, I, F, R>>::digits10;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr int numeric_limits<fpm::fixed<B, I, F, R>>::max_digits10;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr int numeric_limits<fpm::fixed<B, I, F, R>>::radix;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr int numeric_limits<fpm::fixed<B, I, F, R>>::min_exponent;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr int numeric_limits<fpm::fixed<B, I, F, R>>::min_exponent10;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr int numeric_limits<fpm::fixed<B, I, F, R>>::max_exponent;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr int numeric_limits<fpm::fixed<B, I, F, R>>::max_exponent10;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::traps;
	template <typename B, typename I, unsigned int F, bool R>
	constexpr bool numeric_limits<fpm::fixed<B, I, F, R>>::tinyness_before;

#pragma pop_macro("max")
#pragma pop_macro("min")
}

// Math
namespace fpm {

	//MIT License
	//
	// Copyright (c) 2019 Mike Lankamp
	// 
	// Permission is hereby granted, free of charge, to any person obtaining a copy
	// of this software and associated documentation files (the "Software"), to deal
	// in the Software without restriction, including without limitation the rights
	// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	// copies of the Software, and to permit persons to whom the Software is
	// furnished to do so, subject to the following conditions:
	// 
	// The above copyright notice and this permission notice shall be included in all
	// copies or substantial portions of the Software.
	// 
	// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	// SOFTWARE.

	//
	// Helper functions
	//
	namespace detail {

		// Returns the index of the most-signifcant set bit
		inline long find_highest_bit(unsigned long long value) noexcept {
			assert(value != 0);
#if defined(_MSC_VER)
			unsigned long index;
#if defined(_WIN64)
			_BitScanReverse64(&index, value);
#else
			if (_BitScanReverse(&index, static_cast<unsigned long>(value >> 32)) != 0) {
				index += 32;
			} else {
				_BitScanReverse(&index, static_cast<unsigned long>(value & 0xfffffffflu));
			}
#endif
			return index;
#elif defined(__GNUC__) || defined(__clang__)
			return sizeof(value) * 8 - 1 - __builtin_clzll(value);
#else
#   error "your platform does not support find_highest_bit()"
#endif
		}

	}

	//
	// Classification methods
	//

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline int fpclassify(fixed<B, I, F, R> x) noexcept {
		return (x.raw_value() == 0) ? FP_ZERO : FP_NORMAL;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool isfinite(fixed<B, I, F, R>) noexcept {
		return true;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool isinf(fixed<B, I, F, R>) noexcept {
		return false;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool isnan(fixed<B, I, F, R>) noexcept {
		return false;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool isnormal(fixed<B, I, F, R> x) noexcept {
		return x.raw_value() != 0;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool signbit(fixed<B, I, F, R> x) noexcept {
		return x.raw_value() < 0;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool isgreater(fixed<B, I, F, R> x, fixed<B, I, F, R> y) noexcept {
		return x > y;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool isgreaterequal(fixed<B, I, F, R> x, fixed<B, I, F, R> y) noexcept {
		return x >= y;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool isless(fixed<B, I, F, R> x, fixed<B, I, F, R> y) noexcept {
		return x < y;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool islessequal(fixed<B, I, F, R> x, fixed<B, I, F, R> y) noexcept {
		return x <= y;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool islessgreater(fixed<B, I, F, R> x, fixed<B, I, F, R> y) noexcept {
		return x != y;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline bool isunordered(fixed<B, I, F, R> x, fixed<B, I, F, R> y) noexcept {
		return false;
	}

	//
	// Nearest integer operations
	//
	template <typename B, typename I, unsigned int F, bool R>
	inline fixed<B, I, F, R> ceil(fixed<B, I, F, R> x) noexcept {
		constexpr auto FRAC = B(1) << F;
		auto value = x.raw_value();
		if (value > 0) value += FRAC - 1;
		return fixed<B, I, F, R>::from_raw_value(value / FRAC * FRAC);
	}

	template <typename B, typename I, unsigned int F, bool R>
	inline fixed<B, I, F, R> floor(fixed<B, I, F, R> x) noexcept {
		constexpr auto FRAC = B(1) << F;
		auto value = x.raw_value();
		if (value < 0) value -= FRAC - 1;
		return fixed<B, I, F, R>::from_raw_value(value / FRAC * FRAC);
	}

	template <typename B, typename I, unsigned int F, bool R>
	inline fixed<B, I, F, R> trunc(fixed<B, I, F, R> x) noexcept {
		constexpr auto FRAC = B(1) << F;
		return fixed<B, I, F, R>::from_raw_value(x.raw_value() / FRAC * FRAC);
	}

	template <typename B, typename I, unsigned int F, bool R>
	inline fixed<B, I, F, R> round(fixed<B, I, F, R> x) noexcept {
		constexpr auto FRAC = B(1) << F;
		auto value = x.raw_value() / (FRAC / 2);
		return fixed<B, I, F, R>::from_raw_value(((value / 2) + (value % 2)) * FRAC);
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> nearbyint(fixed<B, I, F, R> x) noexcept {
		// Rounding mode is assumed to be FE_TONEAREST
		constexpr auto FRAC = B(1) << F;
		auto value = x.raw_value();
		const bool is_half = std::abs(value % FRAC) == FRAC / 2;
		value /= FRAC / 2;
		value = (value / 2) + (value % 2);
		value -= (value % 2) * is_half;
		return fixed<B, I, F, R>::from_raw_value(value * FRAC);
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline fixed<B, I, F, R> rint(fixed<B, I, F, R> x) noexcept {
		// Rounding mode is assumed to be FE_TONEAREST
		return nearbyint(x);
	}

	//
	// Mathematical functions
	//
	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline fixed<B, I, F, R> abs(fixed<B, I, F, R> x) noexcept {
		return (x >= fixed<B, I, F, R>{0}) ? x : -x;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline fixed<B, I, F, R> fmod(fixed<B, I, F, R> x, fixed<B, I, F, R> y) noexcept {
		return
			assert(y.raw_value() != 0),
			fixed<B, I, F, R>::from_raw_value(x.raw_value() % y.raw_value());
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline fixed<B, I, F, R> remainder(fixed<B, I, F, R> x, fixed<B, I, F, R> y) noexcept {
		return
			assert(y.raw_value() != 0),
			x - nearbyint(x / y) * y;
	}

	template <typename B, typename I, unsigned int F, bool R>
	inline fixed<B, I, F, R> remquo(fixed<B, I, F, R> x, fixed<B, I, F, R> y, int* quo) noexcept {
		assert(y.raw_value() != 0);
		assert(quo != nullptr);
		*quo = x.raw_value() / y.raw_value();
		return fixed<B, I, F, R>::from_raw_value(x.raw_value() % y.raw_value());
	}

	//
	// Manipulation functions
	//

	template <typename B, typename I, unsigned int F, bool R, typename C, typename J, unsigned int G, bool S>
	constexpr inline fixed<B, I, F, R> copysign(fixed<B, I, F, R> x, fixed<C, J, G, S> y) noexcept {
		return
			x = abs(x),
			(y >= fixed<C, J, G, S>{0}) ? x : -x;
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline fixed<B, I, F, R> nextafter(fixed<B, I, F, R> from, fixed<B, I, F, R> to) noexcept {
		return from == to ? to :
			to > from ? fixed<B, I, F, R>::from_raw_value(from.raw_value() + 1)
			: fixed<B, I, F, R>::from_raw_value(from.raw_value() - 1);
	}

	template <typename B, typename I, unsigned int F, bool R>
	constexpr inline fixed<B, I, F, R> nexttoward(fixed<B, I, F, R> from, fixed<B, I, F, R> to) noexcept {
		return nextafter(from, to);
	}

	template <typename B, typename I, unsigned int F, bool R>
	inline fixed<B, I, F, R> modf(fixed<B, I, F, R> x, fixed<B, I, F, R>* iptr) noexcept {
		const auto raw = x.raw_value();
		constexpr auto FRAC = B{ 1 } << F;
		*iptr = fixed<B, I, F, R>::from_raw_value(raw / FRAC * FRAC);
		return fixed<B, I, F, R>::from_raw_value(raw % FRAC);
	}


	//
	// Power functions
	//

	template <typename B, typename I, unsigned int F, bool R, typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
	fixed<B, I, F, R> pow(fixed<B, I, F, R> base, T exp) noexcept {
		using Fixed = fixed<B, I, F, R>;

		if (base == Fixed(0)) {
			assert(exp > 0);
			return Fixed(0);
		}

		Fixed result{ 1 };
		if (exp < 0) {
			for (Fixed intermediate = base; exp != 0; exp /= 2, intermediate *= intermediate) {
				if ((exp % 2) != 0) {
					result /= intermediate;
				}
			}
		} else {
			for (Fixed intermediate = base; exp != 0; exp /= 2, intermediate *= intermediate) {
				if ((exp % 2) != 0) {
					result *= intermediate;
				}
			}
		}
		return result;
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> pow(fixed<B, I, F, R> base, fixed<B, I, F, R> exp) noexcept {
		using Fixed = fixed<B, I, F, R>;

		if (base == Fixed(0)) {
			assert(exp > Fixed(0));
			return Fixed(0);
		}

		if (exp < Fixed(0)) {
			return 1 / pow(base, -exp);
		}

		constexpr auto FRAC = B(1) << F;
		if (exp.raw_value() % FRAC == 0) {
			// Non-fractional exponents are easier to calculate
			return pow(base, exp.raw_value() / FRAC);
		}

		// For negative bases we do not support fractional exponents.
		// Technically fractions with odd denominators could work,
		// but that's too much work to figure out.
		assert(base > Fixed(0));
		return exp2(log2(base) * exp);
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> exp(fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F, R>;
		//if (x < Fixed(0)) {
		//	return 1 / exp(-x);
		//}
		constexpr auto FRAC = B(1) << F;
		const B x_int = x.raw_value() / FRAC;
		x -= x_int;
		assert(x >= Fixed(0) && x < Fixed(1));

		constexpr auto fA = Fixed::template from_fixed_point<63>(128239257017632854ll); // 1.3903728105644451e-2
		constexpr auto fB = Fixed::template from_fixed_point<63>(320978614890280666ll); // 3.4800571158543038e-2
		constexpr auto fC = Fixed::template from_fixed_point<63>(1571680799599592947ll); // 1.7040197373796334e-1
		constexpr auto fD = Fixed::template from_fixed_point<63>(4603349000587966862ll); // 4.9909609871464493e-1
		constexpr auto fE = Fixed::template from_fixed_point<62>(4612052447974689712ll); // 1.0000794567422495
		constexpr auto fF = Fixed::template from_fixed_point<63>(9223361618412247875ll); // 9.9999887043019773e-1
		return pow(Fixed::e(), x_int) * (((((fA * x + fB) * x + fC) * x + fD) * x + fE) * x + fF);
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> exp2(fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F, R>;
		if (x < Fixed(0)) {
			return 1 / exp2(-x);
		}
		constexpr auto FRAC = B(1) << F;
		const B x_int = x.raw_value() / FRAC;
		x -= x_int;
		assert(x >= Fixed(0) && x < Fixed(1));

		constexpr auto fA = Fixed::template from_fixed_point<63>(17491766697771214ll); // 1.8964611454333148e-3
		constexpr auto fB = Fixed::template from_fixed_point<63>(82483038782406547ll); // 8.9428289841091295e-3
		constexpr auto fC = Fixed::template from_fixed_point<63>(515275173969157690ll); // 5.5866246304520701e-2
		constexpr auto fD = Fixed::template from_fixed_point<63>(2214897896212987987ll); // 2.4013971109076949e-1
		constexpr auto fE = Fixed::template from_fixed_point<63>(6393224161192452326ll); // 6.9315475247516736e-1
		constexpr auto fF = Fixed::template from_fixed_point<63>(9223371050976163566ll); // 9.9999989311082668e-1
		return Fixed(1 << x_int) * (((((fA * x + fB) * x + fC) * x + fD) * x + fE) * x + fF);
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> expm1(fixed<B, I, F, R> x) noexcept {
		return exp(x) - 1;
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> log2(fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F, R>;
		assert(x > Fixed(0));

		// Normalize input to the [1:2] domain
		B value = x.raw_value();
		const long highest = detail::find_highest_bit(value);
		if (highest >= F) {
			value >>= (highest - F);
		} else {
			value <<= (F - highest);
		}
		x = Fixed::from_raw_value(value);
		assert(x >= Fixed(1) && x < Fixed(2));

		constexpr auto fA = Fixed::template from_fixed_point<63>(413886001457275979ll); //  4.4873610194131727e-2
		constexpr auto fB = Fixed::template from_fixed_point<63>(-3842121857793256941ll); // -4.1656368651734915e-1
		constexpr auto fC = Fixed::template from_fixed_point<62>(7522345947206307744ll); //  1.6311487636297217
		constexpr auto fD = Fixed::template from_fixed_point<61>(-8187571043052183818ll); // -3.5507929249026341
		constexpr auto fE = Fixed::template from_fixed_point<60>(5870342889289496598ll); //  5.0917108110420042
		constexpr auto fF = Fixed::template from_fixed_point<61>(-6457199832668582866ll); // -2.8003640347009253
		return Fixed(highest - F) + (((((fA * x + fB) * x + fC) * x + fD) * x + fE) * x + fF);
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> log(fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F, R>;
		return log2(x) / log2(Fixed::e());
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> log10(fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F, R>;
		return log2(x) / log2(Fixed(10));
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> log1p(fixed<B, I, F, R> x) noexcept {
		return log(1 + x);
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> cbrt(fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F, R>;

		if (x == Fixed(0)) {
			return x;
		}
		if (x < Fixed(0)) {
			return -cbrt(-x);
		}
		assert(x >= Fixed(0));

		// Finding the cube root of an integer, taken from Hacker's Delight,
		// based on the square root algorithm.

		// We start at the greatest power of eight that's less than the argument.
		int ofs = ((detail::find_highest_bit(x.raw_value()) + 2 * F) / 3 * 3);
		I num = I{ x.raw_value() };
		I res = 0;

		const auto do_round = [&] {
			for (; ofs >= 0; ofs -= 3) {
				res += res;
				const I val = (3 * res * (res + 1) + 1) << ofs;
				if (num >= val) {
					num -= val;
					res++;
				}
			}
		};

		// We should shift by 2*F (since there are two multiplications), but that
		// could overflow even the intermediate type, so we have to split the
		// algorithm up in two rounds of F bits each. Each round will deplete
		// 'num' digit by digit, so after a round we can shift it again.
		num <<= F;
		ofs -= F;
		do_round();

		num <<= F;
		ofs += F;
		do_round();

		return Fixed::from_raw_value(static_cast<B>(res));
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> sqrt(fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F, R>;

		assert(x >= Fixed(0));
		if (x == Fixed(0)) {
			return x;
		}

		// Finding the square root of an integer in base-2, from:
		// https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29

		// Shift by F first because it's fixed-point.
		I num = I{ x.raw_value() } << F;
		I res = 0;

		// "bit" starts at the greatest power of four that's less than the argument.
		for (I bit = I{ 1 } << ((detail::find_highest_bit(x.raw_value()) + F) / 2 * 2); bit != 0; bit >>= 2) {
			const I val = res + bit;
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

		return Fixed::from_raw_value(static_cast<B>(res));
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> hypot(fixed<B, I, F, R> x, fixed<B, I, F, R> y) noexcept {
		assert(x != 0 || y != 0);
		return sqrt(x * x + y * y);
	}

	//
	// Trigonometry functions
	//

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> sin(fixed<B, I, F, R> x) noexcept {
		// This sine uses a fifth-order curve-fitting approximation originally
		// described by Jasper Vijn on coranac.com which has a worst-case
		// relative error of 0.07% (over [-pi:pi]).
		using Fixed = fixed<B, I, F, R>;

		// Turn x from [0..2*PI] domain into [0..4] domain
		x = fmod(x, Fixed::two_pi());
		x = x / Fixed::half_pi();

		// Take x modulo one rotation, so [-4..+4].
		if (x < Fixed(0)) {
			x += Fixed(4);
		}

		int sign = +1;
		if (x > Fixed(2)) {
			// Reduce domain to [0..2].
			sign = -1;
			x -= Fixed(2);
		}

		if (x > Fixed(1)) {
			// Reduce domain to [0..1].
			x = Fixed(2) - x;
		}

		const Fixed x2 = x * x;
		return sign * x * (Fixed::pi() - x2 * (Fixed::two_pi() - 5 - x2 * (Fixed::pi() - 3))) / 2;
	}

	template <typename B, typename I, unsigned int F, bool R>
	inline fixed<B, I, F, R> cos(fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F>;
		if (x > Fixed(0)) {  // Prevent an overflow due to the addition of pi/2
			return sin(x - (Fixed::two_pi() - Fixed::half_pi()));
		} else {
			return sin(Fixed::half_pi() + x);
		}
	}

	template <typename B, typename I, unsigned int F, bool R>
	inline fixed<B, I, F, R> tan(fixed<B, I, F, R> x) noexcept {
		auto cx = cos(x);

		// Tangent goes to infinity at 90 and -90 degrees.
		// We can't represent that with fixed-point maths.
		assert(abs(cx).raw_value() > 1);

		return sin(x) / cx;
	}

	namespace detail {

		// Calculates atan(x) assuming that x is in the range [0,1]
		template <typename B, typename I, unsigned int F, bool R>
		fixed<B, I, F, R> atan_sanitized(fixed<B, I, F, R> x) noexcept {
			using Fixed = fixed<B, I, F, R>;
			assert(x >= Fixed(0) && x <= Fixed(1));

			constexpr auto fA = Fixed::template from_fixed_point<63>(716203666280654660ll); //  0.0776509570923569
			constexpr auto fB = Fixed::template from_fixed_point<63>(-2651115102768076601ll); // -0.287434475393028
			constexpr auto fC = Fixed::template from_fixed_point<63>(9178930894564541004ll); //  0.995181681698119  (PI/4 - A - B)

			const auto xx = x * x;
			return ((fA * xx + fB) * xx + fC) * x;
		}

		// Calculate atan(y / x), assuming x != 0.
		//
		// If x is very, very small, y/x can easily overflow the fixed-point range.
		// If q = y/x and q > 1, atan(q) would calculate atan(1/q) as intermediate step
		// anyway. We can shortcut that here and avoid the loss of information, thus
		// improving the accuracy of atan(y/x) for very small x.
		template <typename B, typename I, unsigned int F, bool R>
		fixed<B, I, F, R> atan_div(fixed<B, I, F, R> y, fixed<B, I, F, R> x) noexcept {
			using Fixed = fixed<B, I, F, R>;
			assert(x != Fixed(0));

			// Make sure y and x are positive.
			// If y / x is negative (when y or x, but not both, are negative), negate the result to
			// keep the correct outcome.
			if (y < Fixed(0)) {
				if (x < Fixed(0)) {
					return atan_div(-y, -x);
				}
				return -atan_div(-y, x);
			}
			if (x < Fixed(0)) {
				return -atan_div(y, -x);
			}
			assert(y >= Fixed(0));
			assert(x > Fixed(0));

			if (y > x) {
				return Fixed::half_pi() - detail::atan_sanitized(x / y);
			}
			return detail::atan_sanitized(y / x);
		}

	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> atan(fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F, R>;
		//if (x < Fixed(0)) {
		//	return -atan(-x);
		//}

		if (x > Fixed(1)) {
			return Fixed::half_pi() - detail::atan_sanitized(Fixed(1) / x);
		}

		return detail::atan_sanitized(x);
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> asin(fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F, R>;
		assert(x >= Fixed(-1) && x <= Fixed(+1));

		const auto yy = Fixed(1) - x * x;
		if (yy == Fixed(0)) {
			return copysign(Fixed::half_pi(), x);
		}
		return detail::atan_div(x, sqrt(yy));
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> acos(fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F, R>;
		assert(x >= Fixed(-1) && x <= Fixed(+1));

		if (x == Fixed(-1)) {
			return Fixed::pi();
		}
		const auto yy = Fixed(1) - x * x;
		return Fixed(2) * detail::atan_div(sqrt(yy), Fixed(1) + x);
	}

	template <typename B, typename I, unsigned int F, bool R>
	fixed<B, I, F, R> atan2(fixed<B, I, F, R> y, fixed<B, I, F, R> x) noexcept {
		using Fixed = fixed<B, I, F, R>;
		if (x == Fixed(0)) {
			assert(y != Fixed(0));
			return (y > Fixed(0)) ? Fixed::half_pi() : -Fixed::half_pi();
		}

		auto ret = detail::atan_div(y, x);

		if (x < Fixed(0)) {
			return (y >= Fixed(0)) ? ret + Fixed::pi() : ret - Fixed::pi();
		}
		return ret;
	}

}

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

	constexpr FPUint128 operator-() const {
		FPUint128 tmp(~high, ~low);
		return tmp + 1;
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

	constexpr FPUint128 operator-(FPUint128 another) const {
		return *this + another.operator-();
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

class FixedPoint {

	template<const FPUint32 left, const FPUint32 right, const bool negative = false, const FPUint8 extraType = 0, const FPUint64 extraValue = 0>
	friend class FixedPointValue;

	template<const FPInt32 nominator, const FPInt32 denominator>
	friend class FixedPointFractionValue;

	using FPT = fpm::fixed<FPUint64, FPUint128, 32, true>;

	constexpr FixedPoint(bool negative, const FPT value) : inf(false), negative(negative), value(value) {
	}
	
	constexpr FixedPoint(bool inf, bool negative) : inf(true), negative(negative), value(0) {
	}

	static constexpr void Overflow() {
		Crash();
		return;
	}
	
	static constexpr void DivideByZero() {
		Crash();
		return;
	}

	static constexpr void NegativeSqrt() {
		Crash();
		return;
	}

	static constexpr void UndefinedAtan2() {
		Crash();
		return;
	}

	static constexpr void Crash() {
		((FPUint64*)0)[0] = 0;
	}

public:

	constexpr FixedPoint() : inf(false), negative(false), value(0) {
	}

	constexpr FixedPoint(const FixedPoint& another) : inf(another.inf), negative(another.negative), value(another.value) {
	
	}

	explicit FixedPoint(FPInt32 value) : negative(value < 0), inf(false), value(value) {

	}

	static constexpr FixedPoint Inf() {
		return FixedPoint(true, false);
	}
	
	static constexpr FixedPoint NegativeInf() {
		return FixedPoint(true, true);
	}
	
	constexpr const FPUint32 GetWholeValue(bool& negative) const {
		negative = this->negative;
		return static_cast<FPUint32>(value);
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
			if (value > 0xFFFFFFFF - another.value) {
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
	
	const constexpr  FixedPoint operator/(const FixedPoint& another) const {
		if (inf || another.inf) {
			return Inf();
		}
		if (another.IsZero()) {
			DivideByZero();
			return Inf();
		}
		bool neg = (negative || another.negative) && negative != another.negative;
		FPT res = value / another.value;
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
		FPT res = fpm::fmod(value, another.value);
		return FixedPoint(neg, res);
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
		return FixedPoint(negative, fpm::floor(value));
	}

	const constexpr FixedPoint Abs() {
		if (inf) {
			return Inf();
		}
		return FixedPoint(false, value);
	}

	const constexpr bool AboveZero() const {
		return !inf && !negative && value.raw_value() > 0;
	}
	
	const constexpr bool BelowZero() const {
		return !inf && negative && value.raw_value() > 0;
	}

	const constexpr bool IsZero() const {
		return !inf && !negative && value.raw_value() == 0;
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

};

template<const FPUint32 left, const FPUint32 right, const bool negative, const FPUint8 extraType, const FPUint64 extraValue>
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
			FPUint32 test = left;
			FPT tLeft(left);

			FPUint32 tmp = right;
			FPT pow(10);
			FPT ten(10);
			FPT num(0);

			while (tmp > 0) {
				num += tmp % 10;
				tmp /= 10;
				pow = pow * ten;
				num *= 10;
			}

			return tLeft + (num / pow);
		}
		case 1:
		{
			// Values are encoded in extra value
			return FPT::from_raw_value(extraValue);
		}
		default:
			FixedPoint::Crash();
			return FPT::from_raw_value(0);
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
		constexpr FPT vNom(nominator < 0 ? -nominator : nominator);
		constexpr FPT vDen(denominator < 0 ? -denominator : denominator);
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
	FPT res = value * another.value;


	FPUint64 x = static_cast<FPUint64>(value);
	FPUint64 y = static_cast<FPUint64>(another.value);

	// Naive ceil, close to limits is no bother
	x++;
	y++;

	// Maximum allowed number for 
	FPUint64 max = -1;
	max = max / x;
	if (y >= max) {
		Overflow();
		return Inf();
	}
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
	return FixedPoint(false, fpm::cos(fp.value));
}

constexpr FixedPoint FixedPoint::sin(FixedPoint fp) {
	if (fp.inf) {
		return fp;
	}
	FixedPoint tmp = fp % (F_2PI);
	if (tmp < F_PI) {
		return FixedPoint(false, fpm::sin(tmp.value));
	} else if (tmp == F_PI) {
		return F_ZERO;
	} else { // tmp < F_2PI
		tmp -= F_PI;
		return FixedPoint(false, fpm::sin(tmp.value)).operator-();
	}
}

constexpr FixedPoint FixedPoint::sqrt(FixedPoint fp) {
	if (fp.inf) {
		return fp;
	}
	if (fp.AboveZero()) {
		return FixedPoint(false, fpm::sqrt(fp.value));
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
		return FixedPoint(false, fpm::exp(fp.value));
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
	if (negative) {
		sprintf_s(buffer, bufferSize, "-");
		if (bufferSize > 0) {
			bufferSize--;
			buffer = &(buffer[1]);
		}
	}
	if (inf) {
		sprintf_s(buffer, bufferSize, "Inf");
		return;
	}

	//static_assert(FLT_MAX + FLT_EPSILON == FLT_MIN, "Invalid limit values");

	auto GetDigit = [](FixedPoint& fp) {
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
	char highBuffer[64];
	highBuffer[0] = '0';
	highBuffer[1] = 0;
	FPUint32 chigh = static_cast<FPUint32>(high.value);
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

	char lowBuffer[64];
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
	sprintf_s(buffer, bufferSize, "%s.%s", highBuffer, lowBuffer);
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

//typedef float FixedPoint;
//template<const int32_t nominator, const int32_t denominator>
//class FixedPointFractionValue {
//
//public:
//
//	constexpr operator FixedPoint() {
//		float fNom = (float)nominator;
//		float fDen = (float)denominator;
//		return fNom / fDen;
//	}
//
//};
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
