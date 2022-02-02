#pragma once

#ifndef ARITHMETIC_UTILS_H
#define ARITHMETIC_UTILS_H

// advc: In part cut from CvGameCoreUtils.h

#define SQR(x) ((x)*(x))

/*	(advc: Can also use ScaledNum. Maybe these functions read a bit better in
	code that doesn't use fractions much.) */
namespace intdiv
{
	// Replacing K-Mod's (incorrectly implemented) ROUND_DIVIDE (CvGlobals.h)
	inline int round(int iDividend, int iDivisor)
	{
		int iSign = ((iDividend ^ iDivisor) >= 0 ? 1 : -1);
		return (iDividend + iSign * iDivisor / 2) / iDivisor;
	}

	// The "u" functions are only for nonnegative numbers ...
	inline int uround(int iDividend, int iDivisor)
	{
		FAssert((iDividend ^ iDivisor) >= 0); // Both negative is OK
		return (iDividend + iDivisor / 2) / iDivisor;
	}

	inline int uceil(int iDividend, int iDivisor)
	{
		FAssert(iDividend >= 0 && iDivisor > 0);
		return (iDividend + iDivisor - 1) / iDivisor;
	}
}

/*	advc.opt: MSVC produces branches for std::max and std::min.
	There is a cmov instruction, but 32-bit MSVC won't generate that.
	(There might be an intrinsic that we could use though.)
	Tbd.: Replace more std::max, std::min calls with this? */
namespace branchless
{
	inline int max(int x, int y)
	{
		return x ^ ((x ^ y) & -(x < y));
	}

	inline int min(int x, int y)
	{
		return y + ((x - y) & -(x < y));
	}
}

inline int range(int iNum, int iLow, int iHigh)
{
	FAssert(iHigh >= iLow);

	if (iNum < iLow)
		return iLow;
	else if (iNum > iHigh)
		return iHigh;
	else return iNum;
}

inline float range(float fNum, float fLow, float fHigh)
{
	FAssert(fHigh >= fLow);

	if (fNum < fLow)
		return fLow;
	else if (fNum > fHigh)
		return fHigh;
	else return fNum;
}

// advc.003g:
inline double dRange(double d, double low, double high)
{
	if(d < low)
		return low;
	if(d > high)
		return high;
	return d;
}

// <advc.003g>
namespace stats // Seems too generic, but what else to name it?
{
	template<typename T>
	T median(std::vector<T>& kSamples, bool bSorted = false)
	{
		//PROFILE("stats::median"); // OK - Called often, but also quite fast.
		FAssert(!kSamples.empty());
		if (!bSorted)
			std::sort(kSamples.begin(), kSamples.end());
		int iMedian = kSamples.size() / 2;
		if (kSamples.size() % 2 != 0)
			return kSamples[iMedian];
		return (kSamples[iMedian] + kSamples[iMedian - 1]) / 2;
	}
	template<typename T>
	T max(std::vector<T> const& kSamples)
	{
		FAssert(!kSamples.empty());
		T r = kSamples[0];
		for(size_t i = 1; i < kSamples.size(); i++)
			r = std::max(r, kSamples[i]);
		return r;
	}
	template<typename T>
	T min(std::vector<T> const& kSamples)
	{
		FAssert(!kSamples.empty());
		T r = kSamples[0];
		for(size_t i = 1; i < kSamples.size(); i++)
			r = std::min(r, kSamples[i]);
		return r;
	}
	template<typename T>
	T mean(std::vector<T> const& kSamples)
	{
		FAssert(!kSamples.empty());
		T r = 0;
		for(size_t i = 0; i < kSamples.size(); i++)
			r += kSamples[i];
		return r / static_cast<T>(kSamples.size());
	}
	// Count the number of samples in the closed interval [nLower,nUpper]
	template<typename T>
	int intervalFreq(std::vector<T> const& kSamples, T nLower, T nUpper)
	{
		int r = 0;
		for(size_t i = 0; i < kSamples.size(); i++)
		{
			if (kSamples[i] >= nLower && kSamples[i] <= nUpper)
				r++;
		}
		return r;
	}
	template<typename T> // (see e.g. Wikipedia)
	T percentileRank(std::vector<T>& kDistribution, T tScore, bool bSorted = false)
	{
		if (!bSorted)
			std::sort(kDistribution.begin(), kDistribution.end());
		int iLesserScores = 0;
		int iSz = (int)kDistribution.size();
		for (int i = 0; i < iSz; i++)
		{
			if (kDistribution[i] < tScore)
				iLesserScores++;
			else break;
		}
		T tDiv = iSz;
		return iLesserScores / tDiv;
	} 
}

namespace fmath
{
	inline int round(double d) { return (int)((d >= 0 ? 0.5 : -0.5) + d); }
	inline int roundToMultiple(double d, int iMultiple)
	{
		int r = (int)(d + 0.5 * iMultiple);
		return r - r % iMultiple;
	}
}

#endif
