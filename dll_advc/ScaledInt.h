#pragma once

#ifndef SCALED_INT_H
#define SCALED_INT_H

// advc.fract: New file

// Large lookup table, but ScaledInt.h will eventually be precompiled.
#include "FixedPointPowTables.h"
//#include <boost/mpl/if.hpp>
/*	Perhaps better not to include if.hpp for this one struct.
	Might also want to generalize it to choose between more than two types.
	Mostly copied from if.hpp (but it's named 'if_c' there): */
template<bool bCondition, typename T1, typename T2>
struct choose_type { typedef T1 type; };
template<typename T1, typename T2>
struct choose_type<true,T1,T2> { typedef T1 type; };
template<typename T1, typename T2>
struct choose_type<false,T1,T2> { typedef T2 type; };

// For a static buffer shared by all instantiations of ScaledInt
template<typename Dummy> // Dummy param just so that static member can be defined in header
class ScaledIntBase {
protected:
	static CvString szBuf;
};
template<typename Dummy>
CvString ScaledIntBase<Dummy>::szBuf = "";

/*  class ScaledInt: Approximates a fractional number as an integer multiplied by a
	scale factor. For fixed-point arithmetic that can't lead to network sync issues.
	Performance: Generally comparable to floating-point types when the scale factor is
	a power of 2; see ScaledIntTest.cpp.
	Overloads commonly used arithmetic operators and offers some conveniences that the
	built-in types don't have, e.g. abs, clamp, approxEquals, bernoulliSuccess (coin flip).
	Compile-time converter from double: macro 'fixp'
	Conversion from percentage: macro 'per100' (also 'per1000', 'per10000')
	scaled_int and scaled_uint typedefs for default precision.

	In code that uses Hungarian notation, I propose the prefix 'r' for
	ScaledInt variables, or more generally for any types that represent
	rational numbers without a floating point.

	The difference between ScaledInt and boost::rational is that the latter allows
	the denominator to change at runtime, which allows for greater accuracy but
	isn't as fast. */

/*  SCALE is the factor by which integer numbers are multiplied when converted
	to a ScaledInt (see constructor from int) and thus determines the precision
	of fractional numbers and affects the numeric limits (MAX, MIN) - the higher
	SCALE, the greater the precision and the tighter the limits.

	INT is the type of the underlying integer variable. Has to be an integral type.
	Both parameters are mostly internal to the implementation of ScaledInt. The public
	interface assumes that the client code works mostly with int, with types that
	can be cast implicitly to int and with double literals (see fixp macro).
	There are no operators allowing ScaledInt instances of different SCALE values or
	different INT types to be mixed. That said, there is a non-explicit constructor
	for conversion, so some of the existing operators will work for operands with
	differing template parameters.

	For unsigned INT types, internal integer divisions are rounded to the nearest INT
	in order to improve precision. For signed INT types, this isn't guaranteed. (But
	ScaledInt::round always rounds to the nearest integer.) Using an unsigned INT type
	also speeds up multiplication.

	EnumType (optional): If an enum type is given, the resulting ScaledInt type will
	be incompatible with ScaledInt types that use a different enum type.
	See usage example (MovementPtS) in ScaledIntTest.

	Tbd. - See the replies and "To be done" in the initial post:
	forums.civfanatics.com/threads/class-for-fixed-point-arithmetic.655037/
*/
template<typename IntType, IntType SCALE, typename EnumType = int>
class ScaledInt : ScaledIntBase<void>
{
	BOOST_STATIC_ASSERT(sizeof(IntType) == 4 || sizeof(IntType) <= 2);
	// Larger type for intermediate results of multiplications
	typedef typename choose_type<(sizeof(IntType) >= 4), __int64,int>::type LongType;
	typedef typename choose_type<(sizeof(IntType) >= 4), unsigned __int64,uint>::type ULongType;
public:
	static IntType MAX() { return INTMAX / SCALE; }
	static IntType MIN() { return INTMIN / SCALE; }

	/*	Factory function for creating fractions (with wrapper macros per100).
		Numerator and denominator as template parameters ensure
		that the conversion to SCALE happens at compile time, so that
		floating-point math can be used for maximal accuracy.
		When the denominator isn't known at compile time, use ctor(int,int). */
	template<int iNUM, int iDEN>
	static inline ScaledInt fromRational()
	{
		BOOST_STATIC_ASSERT(bSIGNED || (iDEN >= 0 && iNUM >= 0));
		return fromDouble(iNUM / static_cast<double>(iDEN));
	}
	template<int iDEN>
	static inline ScaledInt fromRational(int iNum)
	{
		BOOST_STATIC_ASSERT(bSIGNED || iDEN >= 0);
		if (!bSIGNED)
		{
			FAssert(iNum >= 0);
		}
		if (sizeof(IntType) < sizeof(int))
		{
			if (bSIGNED)
			{
				FAssert(iNum >= static_cast<int>(INTMIN));
			}
			FAssert(iNum <= static_cast<int>(INTMAX));
		}
		ScaledInt<IntType,iDEN> rRational;
		rRational.m_i = static_cast<IntType>(iNum);
		return rRational;
	}

	__forceinline static ScaledInt max(ScaledInt r1, ScaledInt r2)
	{
		return std::max(r1, r2);
	}
	__forceinline static ScaledInt min(ScaledInt r1, ScaledInt r2)
	{
		return std::min(r1, r2);
	}

	__forceinline ScaledInt() : m_i(0) {}
	__forceinline ScaledInt(int i) : m_i(SCALE * i)
	{
		// (Not sure if these assertions should be kept permanently)
		if (!bSIGNED)
		{
			FAssert(i >= 0);
		}
		FAssertBounds(INTMIN / SCALE, INTMAX / SCALE + 1, i);
	}
	__forceinline ScaledInt(uint u) : m_i(SCALE * u)
	{
		FAssert(u <= INTMAX / SCALE);
	}
	// Conversion between scales and int types
	__forceinline ScaledInt(int iNum, int iDen)
	{
		m_i = toScale(iNum, iDen);
	}
	template<typename OtherIntType, OtherIntType FROM_SCALE>
	__forceinline ScaledInt(ScaledInt<OtherIntType,FROM_SCALE> rOther)
	{
		if (!bSIGNED && rOther.bSIGNED)
		{
			FAssert(rOther.m_i >= 0);
		}
		if (FROM_SCALE == SCALE)
			m_i = static_cast<IntType>(rOther.m_i);
		else
		{
			FAssertBounds(INTMIN / SCALE, INTMAX / SCALE + 1, rOther.m_i);
			m_i = static_cast<IntType>((rOther.m_i * SCALE +
				// Only round to nearest when unsigned (avoid branching)
				(bSIGNED ? 0/*(FROM_SCALE / (rOther.m_i > 0 ? 2 : -2))*/ :
				FROM_SCALE / 2)) / FROM_SCALE);
		}
	}

	__forceinline int getInt() const
	{
		// Conversion to int shouldn't be extremely frequent; take the time to round.
		return round();
	}
	int round() const
	{
		return (m_i + SCALE / (!bSIGNED || m_i > 0 ? 2 : -2)) / SCALE;
	}
	// Cast operator - better require explicit calls to getInt.
	/*__forceinline operator int() const
	{
		return getInt();
	}*/
	bool isInt() const
	{
		return (m_i % SCALE == 0);
	}

	__forceinline int getPercent() const
	{
		return toScaleRound(m_i, SCALE, 100);
	}
	__forceinline int getPermille() const
	{
		return toScaleRound(m_i, SCALE, 1000);
	}
	__forceinline int roundToMultiple(int iMultiple) const
	{
		return toScaleRound(m_i, SCALE * iMultiple, 1) * iMultiple;
	}
	__forceinline double getDouble() const
	{
		return m_i / static_cast<double>(SCALE);
	}
	__forceinline float getFloat() const
	{
		return m_i / static_cast<float>(SCALE);
	}
	CvString const& str(int iDen = SCALE)
	{
		if (iDen == 1)
			szBuf.Format("%s%d", isInt() ? "" : "ca. ", round());
		else if (iDen == 100)
			szBuf.Format("%d percent", getPercent());
		else if (iDen == 1000)
			szBuf.Format("%d permille", getPermille());
		else szBuf.Format("%d/%d", toScale(m_i, SCALE, iDen), iDen);
		return szBuf;
	}

	void write(FDataStreamBase* pStream) const
	{
		pStream->Write(m_i);
	}
	void read(FDataStreamBase* pStream)
	{
		pStream->Read(&m_i);
	}

	__forceinline void mulDiv(int iMultiplier, int iDivisor)
	{
		if (!bSIGNED)
		{
			FAssert(iMultiplier >= 0 && iDivisor >= 0);
		}
		m_i = toScale(m_i, iDivisor, iMultiplier);
	}

	// Bernoulli trial (coin flip) with success probability equal to m_i/SCALE
	bool bernoulliSuccess(CvRandom& kRand, char const* szLog,
		int iLogData1 = -1, int iLogData2 = -1) const
	{
		// Guards for better performance and to avoid unnecessary log output
		if (m_i <= 0)
			return false;
		if (m_i >= SCALE)
			return true;
		return (kRand.getInt(SCALE, szLog, iLogData1, iLogData2) < m_i);
	}

	ScaledInt pow(int iExp) const
	{
		if (iExp < 0)
			return 1 / powNonNegative(-iExp);
		return powNonNegative(iExp);
	}
	ScaledInt pow(ScaledInt rExp) const
	{
		FAssert(!isNegative());
		if (rExp.bSIGNED && rExp.isNegative())
			return 1 / powNonNegative(-rExp);
		return powNonNegative(rExp);
	}
	__forceinline ScaledInt sqrt() const
	{
		FAssert(!isNegative());
		return powNonNegative(fromRational<1,2>());
	}

	__forceinline ScaledInt abs() const
	{
		ScaledInt r;
		r.m_i = std::abs(m_i);
		return r;
	}

	template<typename LoType, typename HiType>
	__forceinline void clamp(LoType lo, HiType hi)
	{
		FAssert(lo <= hi);
		increaseTo(lo);
		decreaseTo(hi);
	}
	template<typename LoType>
	__forceinline void increaseTo(LoType lo)
	{
		// (std::max doesn't allow differing types)
		if (*this < lo)
			*this = lo;
	}
	template<typename HiType>
	__forceinline void decreaseTo(HiType hi)
	{
		if (*this > hi)
			*this = hi;
	}
	template<typename LoType, typename HiType>
	__forceinline ScaledInt clamped(LoType lo, HiType hi) const
	{
		ScaledInt rCopy(*this);
		rCopy.clamp(lo, hi);
		return rCopy;
	}
	template<typename LoType>
	__forceinline ScaledInt increasedTo(LoType lo) const
	{
		ScaledInt rCopy(*this);
		rCopy.increaseTo(lo);
		return rCopy;
	}
	template<typename HiType>
	__forceinline ScaledInt decreasedTo(HiType hi) const
	{
		ScaledInt rCopy(*this);
		rCopy.decreaseTo(hi);
		return rCopy;
	}

	template<typename NumType, typename Epsilon>
	__forceinline bool approxEquals(NumType num, Epsilon e) const
	{
		// Can't be allowed for floating point types; will have to use fixp to wrap.
		BOOST_STATIC_ASSERT(!std::numeric_limits<int>::has_infinity);
		return ((*this - num).abs() <= e);
	}

	__forceinline bool isPositive() const { return (m_i > 0); }
	__forceinline bool isNegative() const { return (bSIGNED && m_i < 0); }

	__forceinline ScaledInt operator-() { return ScaledInt(-m_i); }

	__forceinline bool operator<(ScaledInt rOther) const
	{
		return (m_i < rOther.m_i);
	}
	__forceinline bool operator>(ScaledInt rOther) const
	{
		return (m_i > rOther.m_i);
	}
    __forceinline bool operator==(ScaledInt rOther) const
	{
		return (m_i == rOther.m_i);
	}
	__forceinline bool operator!=(ScaledInt rOther) const
	{
		return (m_i != rOther.m_i);
	}
	__forceinline bool operator<=(ScaledInt rOther) const
	{
		return (m_i <= rOther.m_i);
	}
	__forceinline bool operator>=(ScaledInt rOther) const
	{
		return (m_i >= rOther.m_i);
	}

	/*	Make comparisons with int exact - to be consistent with int-float comparisons.
		(The alternative would be to compare i with this->getInt()). */
	__forceinline bool operator<(int i) const
	{
		return (m_i < scaleForComparison(i));
	}
    __forceinline bool operator>(int i) const
	{
		return (m_i > scaleForComparison(i));
	}
    __forceinline bool operator==(int i) const
	{
		return (m_i == scaleForComparison(i));
	}
	__forceinline bool operator!=(int i) const
	{
		return (m_i != scaleForComparison(i));
	}
	__forceinline bool operator<=(int i) const
	{
		return (m_i <= scaleForComparison(i));
	}
    __forceinline bool operator>=(int i) const
	{
		return (m_i >= scaleForComparison(i));
	}
	__forceinline bool operator<(uint i) const
	{
		return (m_i < scaleForComparison(u));
	}
    __forceinline bool operator>(uint u) const
	{
		return (m_i > scaleForComparison(u));
	}
    __forceinline bool operator==(uint u) const
	{
		return (m_i == scaleForComparison(u));
	}
	__forceinline bool operator!=(uint u) const
	{
		return (m_i != scaleForComparison(i));
	}
	__forceinline bool operator<=(uint u) const
	{
		return (m_i <= scaleForComparison(u));
	}
    __forceinline bool operator>=(uint u) const
	{
		return (m_i >= scaleForComparison(u));
	}

	/*	Can't guarantee here that only const expressions are used.
		So floating-point operands will have to be wrapped in fixp. */
	/*__forceinline bool operator<(double d) const
	{
		return (getDouble() < d);
	}
    __forceinline bool operator>(double d) const
	{
		return (getDouble() > d);
	}*/

	__forceinline ScaledInt& operator+=(ScaledInt rOther)
	{
		// Maybe uncomment this for some special occasion
		/*FAssert(rOther <= 0 || m_i <= INTMAX - rOther.m_i);
		FAssert(rOther >= 0 || m_i >= INTMIN + rOther.m_i);*/
		m_i += rOther.m_i;
		return *this;
	}

	__forceinline ScaledInt& operator-=(ScaledInt rOther)
	{
		/*FAssert(rOther >= 0 || m_i <= INTMAX + rOther.m_i);
		FAssert(rOther <= 0 || m_i >= INTMIN - rOther.m_i);*/
		m_i -= rOther.m_i;
		return *this;
	}

	__forceinline ScaledInt& operator/=(ScaledInt rOther)
	{
		m_i = toScale(m_i, rOther.m_i, SCALE);
	}

	__forceinline ScaledInt& operator*=(ScaledInt rOther)
	{
		/*	For IntType=int and SCALE=1024, this would overflow already when squaring 45.5.
			Perhaps do it only when std::numeric_limits<IntType>::digits is
			greater than some number?
			Or add a bUNSAFE_MULT template parameter for client code where speed
			is essential and overflow impossible. */
		/*FAssertBounds(INTMIN / rOther.m_i, INTMAX / rOther.m_i + 1, m_i);
		m_i *= rOther.m_i;
		if (!bSIGNED) // (For signed rounding, see ROUND_DIVIDE in CvGameCoreUtils.h)
			m_i += SCALE / 2;
		m_i /= SCALE;*/

		if (bSIGNED)
		{
			/*LongType lNum = m_i;
			lNum *= rOther.m_i;
			lNum /= SCALE; // This will call _alldiv, which involves a div.
			FAssert(lNum >= INTMIN && lNum <= INTMAX);
			m_i = static_cast<IntType>(lNum);*/
			// For bSIGNED, MulDiv seems to be a bit faster than the above.
			int iNum = MulDiv(m_i, rOther.m_i, SCALE);
			// -1 indicates overflow, but could also be the legit result of -1/SCALE times 1/SCALE.
			FAssert(iNum != -1 || (m_i * rOther.m_i) / SCALE == -1);
			m_i = iNum;
		}
		else
		{
			ULongType lNum = m_i;
			lNum *= rOther.m_i;
			lNum += SCALE / 2; // Round to nearest
			lNum /= SCALE;
			FAssert(lNum >= INTMIN && lNum <= INTMAX);
			m_i = static_cast<IntType>(lNum);
		}
		return *this;
	}

	__forceinline ScaledInt& operator++()
	{
		(*this) += 1;
		return *this;
	}
	__forceinline ScaledInt& operator--()
	{
		(*this) -= 1;
		return *this;
	}
	__forceinline ScaledInt operator++(int)
	{
		ScaledInt rCopy(*this);
		(*this) += 1;
		return rCopy;
	}
	__forceinline ScaledInt operator--(int)
	{
		ScaledInt rCopy(*this);
		(*this) -= 1;
		return rCopy;
	}

	__forceinline ScaledInt& operator+=(int i)
	{
		(*this) += ScaledInt(i);
		return (*this);
	}
	__forceinline ScaledInt& operator-=(int i)
	{
		(*this) -= ScaledInt(i);
		return (*this);
	}
	__forceinline ScaledInt& operator*=(int i)
	{
		(*this) *= ScaledInt(i);
		return (*this);
	}
	__forceinline ScaledInt& operator/=(int i)
	{
		(*this) /= ScaledInt(i);
		return (*this);
	}
	__forceinline ScaledInt& operator+=(uint u)
	{
		(*this) += ScaledInt(u);
		return (*this);
	}
	__forceinline ScaledInt& operator-=(uint u)
	{
		(*this) -= ScaledInt(u);
		return (*this);
	}
	__forceinline ScaledInt& operator*=(uint u)
	{
		(*this) *= ScaledInt(u);
		return (*this);
	}
	__forceinline ScaledInt& operator/=(uint u)
	{
		(*this) /= ScaledInt(u);
		return (*this);
	}

	__forceinline ScaledInt operator+(int i) const
	{
		ScaledInt rCopy(*this);
		rCopy += i;
		return rCopy;
	}
	__forceinline ScaledInt operator-(int i) const
	{
		ScaledInt rCopy(*this);
		rCopy -= i;
		return rCopy;
	}
	__forceinline ScaledInt operator*(int i) const
	{
		ScaledInt rCopy(*this);
		rCopy *= i;
		return rCopy;
	}
	__forceinline ScaledInt operator/(int i) const
	{
		ScaledInt rCopy(*this);
		rCopy /= i;
		return rCopy;
	}
	__forceinline ScaledInt operator+(uint u) const
	{
		ScaledInt rCopy(*this);
		rCopy += u;
		return rCopy;
	}
	__forceinline ScaledInt operator-(uint u) const
	{
		ScaledInt rCopy(*this);
		rCopy -= u;
		return rCopy;
	}
	__forceinline ScaledInt operator*(uint u) const
	{
		ScaledInt rCopy(*this);
		rCopy *= u;
		return rCopy;
	}
	__forceinline ScaledInt operator/(uint u) const
	{
		ScaledInt rCopy(*this);
		rCopy /= u;
		return rCopy;
	}
	IntType m_i;
	static bool const bSIGNED = std::numeric_limits<IntType>::is_signed;
	static IntType const INTMIN;
	static IntType const INTMAX;
private: // advc.tmp: Made the data members public - until the friend issue can be resolved
	
	// MSVC compiler bug: Can't befriend a dependent-argument template
	/*template<typename OtherIntType, OtherIntType OTHER_SCALE, typename OtherEnumType>
	friend class ScaledInt;*/

	static __forceinline IntType toScale(int iNum, int iFromScale, int iToScale = SCALE)
	{
		// Akin to code in ctor(ScaledInt) and operator*=(ScaledInt)
		if (bSIGNED)
		{
			int i = MulDiv(iNum, iToScale, iFromScale);
			FAssert(i != -1 || (iNum * iToScale) / SCALE == -1);
			return static_cast<IntType>(i);
		}
		else
		{
			ULongType lNum = iNum;
			lNum *= iToScale;
			lNum += iFromScale / 2;
			lNum /= iFromScale;
			FAssert(lNum >= INTMIN && lNum <= INTMAX);
			return static_cast<IntType>(lNum);
		}
	}
	static inline int toScaleRound(int iNum, int iFromScale, int iToScale = SCALE)
	{
		// OK so long as toScale uses MulDiv (which rounds to nearest)
		return static_cast<int>(toScale(iNum, iFromScale, iToScale));
	}

	ScaledInt powNonNegative(int iExp) const
	{
		ScaledInt rCopy(*this);
		/*  This can be done faster in general by squaring.
			However, I doubt that it would be faster for
			the small exponents I'm expecting to deal with*/
		ScaledInt r = 1;
		for (int i = 0; i < iExp; i++)
			r *= rCopy;
		return r;
	}
	/*	Custom algorithm.
		There is a reasonably recent paper "A Division-Free Algorithm for Fixed-Point
		Power Exponential Function in Embedded System" [sic] based on Newton's method.
		That's probably faster and more accurate, but an implementation isn't
		spelled out. Perhaps tbd. */
	ScaledInt powNonNegative(ScaledInt rExp) const
	{
		/*	Base 0 or too close to it to make a difference given the precision of the algorithm.
			Fixme: rExp could also be close to 0. Should somehow use x=y*z => b^x = (b^y)^z. */
		if (m_i < SCALE / 64)
			return 0;
		/*	Recall that: If x=y+z, then b^x=(b^y)*(b^z).
						 If b=a*c, then b^x=(a^x)*(c^x). */
		// Split rExp into the sum of an integer and a (scaled) fraction between 0 and 1
		// Running example: 5.2^2.1 at SCALE 1024, i.e. (5325/1024)^(2150/1024)
		IntType expInt = rExp.m_i / SCALE; // 2 in the example
		// Use uint in all local ScaledInt variables for more accurate rounding
		ScaledInt<uint,128> rExpFrac(rExp - expInt); // Ex.: 13/128
		/*	Factorize the base into powers of 2 and, as the last factor, the base divided
			by the product of the 2-bases. */
		ScaledInt<uint,SCALE> rProductOfPowersOfTwo(1);
		IntType baseDiv = 1;
		// Look up approximate result of 2^rExpFrac in precomputed table
		FAssertBounds(0, 128, rExpFrac.m_i); // advc.tmp: Don't keep this assert permanently
		ScaledInt<uint,256> rPowOfTwo; // Ex.: Array position [13] is 19, so rPowOfTwo=19/256
		rPowOfTwo.m_i = FixedPointPowTables::powersOfTwoNormalized_256[rExpFrac.m_i];
		++rPowOfTwo; // Denormalize (Ex.: 275/256; approximating 2^0.1)
		/*	Tbd.: Try replacing this loop with _BitScanReverse (using the /EHsc compiler flag).
			Or perhaps not available in MSVC03? See: github.com/danaj/Math-Prime-Util/pull/10/
			*/
		while (baseDiv < *this)
		{
			baseDiv *= 2;
			rProductOfPowersOfTwo *= rPowOfTwo;
		} // Ex.: baseDiv=8 and rProductOfPowersOfTwo=1270/1024, approximating (2^0.1)^3.
		ScaledInt<uint,256> rLastFactor(1);
		// Look up approximate result of ((*this)/baseDiv)^rExpFrac in precomputed table
		int iLastBaseTimes64 = (ScaledInt<uint,64>(*this / baseDiv)).m_i; // Ex.: 42/64 approximating 5.2/8
		FAssertBounds(0, 64+1, iLastBaseTimes64); // advc.tmp: Don't keep this assert permanently
		if (rExpFrac.m_i != 0 && iLastBaseTimes64 != 64)
		{
			// Could be prone to cache misses :(
			rLastFactor.m_i = FixedPointPowTables::powersUnitInterval_256
					[iLastBaseTimes64-1][rExpFrac.m_i-1] + 1; // Table and values are shifted by 1
			// Ex.: Position [41][12] is 244, i.e. rLastFactor=245/256. Approximation of (5.2/8)^0.1
		}
		ScaledInt r(ScaledInt<uint,SCALE>(pow(expInt)) *
				rProductOfPowersOfTwo * ScaledInt<uint,SCALE>(rLastFactor));
		return r;
		/*	Ex.: First factor is 27691/1024, approximating 5.2^2,
			second factor: 1270/1024, approximating (2^0.1)^3,
			last factor: 980/1024, approximating (5.2/8)^0.1.
			Result: 32867/1024, which is ca. 32.097, whereas 5.2^2.1 is ca. 31.887. */
	}

	static __forceinline LongType scaleForComparison(int i)
	{
		// If LongType is too slow, we'd have to return an int after checking:
		//FAssertBounds(MIN_INT / SCALE, MAX_INT / SCALE + 1, i);
		LongType lNum = i;
		return lNum * SCALE;
	}
	static __forceinline LongType scaleForComparison(uint u)
	{
		//FAssertBounds(MIN_INT / SCALE, MAX_INT / SCALE + 1, u);
		ULongType lNum = u;
		return lNum * SCALE;
	}

	static __forceinline ScaledInt fromDouble(double d)
	{
		ScaledInt r;
		r.m_i = static_cast<IntType>(d * SCALE + (d > 0 ? 0.5 : -0.5));
		return r;
	}
};

/*	To unclutter template parameter lists and make it easier to add more parameters.
	Un-defined at the end of the file. */
#define ScaledInt_PARAMS typename IntType, IntType SCALE, typename EnumType
#define ScaledInt_T ScaledInt<IntType,SCALE,EnumType>

template<ScaledInt_PARAMS>
IntType const ScaledInt_T::INTMAX = std::numeric_limits<IntType>::max();
template<ScaledInt_PARAMS>
IntType const ScaledInt_T::INTMIN = std::numeric_limits<IntType>::min();
/*	INTMAX and INTMIN aren't compile-time constants (they would be in C++11).
	We also don't have SIZE_MAX (cstdint) and
	no boost::integer_traits<IntType>::const_max. */
//BOOST_STATIC_ASSERT(SCALE*SCALE < INTMAX);

template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator+(ScaledInt_T rLeft, ScaledInt_T rRight)
{
	rLeft += rRight;
	return rLeft;
}
template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator-(ScaledInt_T rLeft, ScaledInt_T rRight)
{
	rLeft -= rRight;
	return rLeft;
}
template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator*(ScaledInt_T rLeft, ScaledInt_T rRight)
{
	rLeft *= rRight;
	return rLeft;
}
template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator/(ScaledInt_T rLeft, ScaledInt_T rRight)
{
	rLeft /= rRight;
	return rLeft;
}

/*	Commutativity
	Tbd.: Try using boost/operators.hpp instead:
	equality_comparable with itself, int and uint (both ways); incrementable; decrementable;
	addable to int, uint, double; int and uint addable to ScaledInt; same for
	subtractable, divisible, multipliable.
	However, boost uses reference parameters for everything. */
template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator+(int i, ScaledInt_T r)
{
	return r + i;
}
/*	As we don't implement an int cast operator, assignment to int
	should be forbidden as well. (No implicit getInt.) */
/*template<ScaledInt_PARAMS>
__forceinline int& operator+=(int& i, ScaledInt_T r)
{
	i = (r + i).getInt();
	return i;
}*/
template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator-(int i, ScaledInt_T r)
{
	return ScaledInt_T(i) - r;
}
/*template<ScaledInt_PARAMS>
__forceinline int& operator-=(int& i, ScaledInt_T r)
{
	i = (ScaledInt_T(i) - r).getInt();
	return i;
}*/
template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator*(int i, ScaledInt_T r)
{
	return r * i;
}
/*template<ScaledInt_PARAMS>
__forceinline int& operator*=(int& i, ScaledInt_T r)
{
	i = (r * i).getInt();
	return i;
}*/
template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator/(int i, ScaledInt_T r)
{
	return ScaledInt_T(i) / r;
}
/*template<ScaledInt_PARAMS>
__forceinline int& operator/=(int& i, ScaledInt_T r)
{
	i = (ScaledInt_T(i) / r).getInt();
	return i;
}*/
template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator+(uint u, ScaledInt_T r)
{
	return r + u;
}
template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator-(uint u, ScaledInt_T r)
{
	return r - u;
}
template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator*(uint ui, ScaledInt_T r)
{
	return r * ui;
}
template<ScaledInt_PARAMS>
__forceinline ScaledInt_T operator/(uint u, ScaledInt_T r)
{
	return r / u;
}
template<ScaledInt_PARAMS>
 __forceinline bool operator<(int i, ScaledInt_T r)
{
	return (r > i);
}
template<ScaledInt_PARAMS>
__forceinline bool operator>(int i, ScaledInt_T r)
{
	return (r < i);
}
template<ScaledInt_PARAMS>
 __forceinline bool operator==(int i, ScaledInt_T r)
{
	return (r == i);
}
template<ScaledInt_PARAMS>
 __forceinline bool operator!=(int i, ScaledInt_T r)
{
	return (r != i);
}
template<ScaledInt_PARAMS>
__forceinline bool operator<=(int i, ScaledInt_T r)
{
	return (r >= i);
}
template<ScaledInt_PARAMS>
 __forceinline bool operator>=(int i, ScaledInt_T r)
{
	return (r <= i);
}
template<ScaledInt_PARAMS>
__forceinline bool operator<(double d, ScaledInt_T r)
{
	return (r > d);
}
template<ScaledInt_PARAMS>
__forceinline bool operator>(double d, ScaledInt_T r)
{
	return (r < d);
}

/*	1024 isn't very precise at all - but at least better than
	the percent scale normally used by BtS.
	Leads to INTMAX=2097151, i.e. ca. 2 mio. */
typedef ScaledInt<int,1024> scaled_int;
typedef ScaledInt<uint,1024> scaled_uint;

#define TYPEDEF_SCALED_ENUM(IntType,iScale,TypeName) \
	enum TypeName##Types {}; /* Not really supposed to be used anywhere else */ \
	typedef ScaledInt<IntType,iScale,TypeName##Types> TypeName;

/*	The uint versions will, unfortunately, not be called when
	the caller passes a positive signed int literal. */
__forceinline scaled_uint per100(uint iNum)
{
	return scaled_uint::fromRational<100>(iNum);
}
__forceinline scaled_int per100(int iNum)
{
	return scaled_int::fromRational<100>(iNum);
}
__forceinline scaled_uint per1000(uint iNum)
{
	return scaled_uint::fromRational<1000>(iNum);
}
__forceinline scaled_int per1000(int iNum)
{
	return scaled_int::fromRational<1000>(iNum);
}
__forceinline scaled_uint per10000(uint iNum)
{
	return scaled_uint::fromRational<10000>(iNum);
}
__forceinline scaled_int per10000(int iNum)
{
	return scaled_int::fromRational<10000>(iNum);
}
/*	For double, only const expressions are allowed. Can only
	make sure of that through a macro. The macro can't use
	(dConstExpr) >= 0 ? scaled_uint::fromRational<...> : scaled_int::fromRational<...>
	b/c the ternary-? operator has to have compatible and unambigious operands types. */
#define fixp(dConstExpr) \
		((dConstExpr) >= ((int)MAX_INT) / 10000 - 1 || \
		(dConstExpr) <= ((int)MIN_INT) / 10000 + 1 ? \
		scaled_int(-1) : \
		scaled_int::fromRational<(int)( \
		(dConstExpr) * 10000 + ((dConstExpr) > 0 ? 0.5 : -0.5)), 10000>())

#undef ScaledInt_PARAMS
#undef ScaledInt_T

#endif
