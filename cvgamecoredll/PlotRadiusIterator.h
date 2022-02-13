#pragma once

#ifndef PLOT_RADIUS_ITERATOR_H
#define PLOT_RADIUS_ITERATOR_H

// advc.plotr: New header

#include "CvMap.h"
//doto enhanced city size mylon -  required for NearbyCityIter (Doto version)
#include "CvCity.h" 

/*	SpiralPlotIterator: Iterator over the CvPlot instances within a radius around
	a central plot or unit. If bINCIRCLE is false, then the radius is measured
	according to the stepDistance metric, which results in a square of plots.
	The center plot doesn't count toward the radius, so e.g. iRadius=3 yields
	a 7x7 square.

	If bINCIRCLE is true, then the plotDistance metric is used, which corresponds
	to a range of plots that approximates the incircle of a square of the same radius.
	The implementation for bINCIRCLE=true isn't terribly efficient: A square is
	generated and plots outside of the plotDistance radius are skipped.
	(That's also what the BtS code did.)
	There are derived classes at the end of this file that hide the template parameter.
	For the special case of iterating over a city radius, see CityPlotIterator.h.
	If iRadius is known to be 1 at compile time and the center is excluded, then
	one of the macros in PlotAdjListTraversal.h should be preferred; they're faster.

	The order of traversal corresponds to a north-east-south-west (clockwise) spiral
	away from the center.
	NULL plots are skipped, but CvUnitAI::AI_plotValid isn't checked - should only be
	checked when considering to move into a tile, and even then there can be faster
	alternatives (see comments at the definition of AI_plotValid). */

// Abstract base class for some behavior shared with ScanLinePlotIterator
class SquareOfPlotsIterator
{
protected:
	SquareOfPlotsIterator(CvPlot const& kCenter, int iRadius)
	:	m_pCenter(&kCenter), m_iRadius(iRadius), m_pNext(NULL)
	{}

public:
	bool hasNext() const
	{
		return (m_pNext != NULL);
	}

	CvPlot& operator*() const
	{
		return *m_pNext;
	}

	CvPlot* operator->() const
	{
		return m_pNext;
	}

	int currStepDist() const
	{
		return ::stepDistance(m_pCenter, m_pNext);
	}

	int currPlotDist() const
	{
		return ::plotDistance(m_pCenter, m_pNext);
	}

	int radius() const
	{
		return m_iRadius;
	}

protected:
	CvPlot* m_pNext;
	CvPlot const* m_pCenter;
	int m_iRadius;
};

template<bool bINCIRCLE = false>
class SpiralPlotIterator : public SquareOfPlotsIterator
{
public:
	SpiralPlotIterator(CvPlot const& kCenter, int iRadius, bool bIncludeCenter = true)
	:	SquareOfPlotsIterator(kCenter, iRadius)
	{
		init(bIncludeCenter);
	}

	SpiralPlotIterator(CvUnit const& kUnit, int iRadius, bool bIncludeCenter = true)
	:	SquareOfPlotsIterator(getUnitPlot(kUnit), iRadius)
	{
		init(bIncludeCenter);
	}

	SpiralPlotIterator(int x, int y, int iRadius, bool bIncludeCenter = true)
	:	SquareOfPlotsIterator(*GC.getMap().plot(x, y), iRadius)
	{
		init(bIncludeCenter);
	}

	SpiralPlotIterator& operator++()
	{
		computeNext();
		return *this;
	}

	int currXDist() const
	{
		/*	World-wrap isn't applied to m_iCurrX (i.e. it can be off the map),
			hence no need to check world wrap (CvMap::xDistance) here. */
		return abs(m_pCenter->getX() - m_iCurrX);
	}

	int currYDist() const
	{
		return abs(m_pCenter->getY() - m_iCurrY);
	}

protected:
	void computeNext()
	{
		//PROFILE_FUNC(); //  Immediate sample stack overflow (b/c recursive?).
						  //  TSC_PROFILE suggests 10 mio. calls in a late-game turn.
		m_iPos++;
		if (m_iPos >= m_iMaxPos)
		{
			m_pNext = NULL;
			return;
		}
		// Change direction?
		if (m_iConsecMoves * (m_iConsecMoves + 1) >= m_iPos)
		{
			// Modulo by a constant power of 2: should be fast
			m_eDir = static_cast<CardinalDirectionTypes>(
					(m_eDir + 1) % NUM_CARDINALDIRECTION_TYPES);
			m_iConsecMoves = 1;
		}
		else m_iConsecMoves++;
		/*	Can't use CvMap::plotCardinalDirection b/c we need the current coordinates
			even if they're off the map */
		m_iCurrX += GC.getPlotCardinalDirectionX()[m_eDir];
		m_iCurrY += GC.getPlotCardinalDirectionY()[m_eDir];
		m_pNext = GC.getMap().plotValidXY(m_iCurrX, m_iCurrY);
		if (m_pNext == NULL)
		{
			computeNext();
			return; // tail recursion
		}
		if (bINCIRCLE)
		{
			if (currPlotDist() > m_iRadius)
				computeNext();
		}
	}

private:
	int m_iPos;
	int m_iMaxPos;
	int m_iCurrX, m_iCurrY;
	CardinalDirectionTypes m_eDir;
	int m_iConsecMoves;

	void init(bool bIncludeCenter)
	{
		/*	I had written a const and non-const version (as derived classes that
			set a "PlotType" template parameter), but CityPlotIterator and
			AgentIterator don't have const versions either, and const-correctness
			would make it harder to add more iterator classes in the future.
			So I've removed those derived classes. Const-casting CvPlot should
			generally be safe. */
		m_pNext = const_cast<CvPlot*>(m_pCenter);
		m_iCurrX = m_pNext->getX();
		m_iCurrY = m_pNext->getY();
		m_iMaxPos = SQR(2 * m_iRadius + 1) - 1;
		m_iPos = -1;
		// (We'll immediately turn NORTH after kCenter)
		m_eDir = CARDINALDIRECTION_WEST;
		m_iConsecMoves = 0;
		if (!bIncludeCenter)
		{
			m_iMaxPos--;
			computeNext();
		}
	}
	// To avoid including CvUnit.h
	CvPlot& getUnitPlot(CvUnit const& kUnit) const;
};

/*	Traverses the square of plots in reading direction.
	The implementation is pretty bare-bones b/c I need it only in one specific
	context so far. */
class ScanLinePlotIterator : public SquareOfPlotsIterator
{
	int m_iDeltaX, m_iDeltaY;
	int m_iCurrXDist, m_iCurrYDist;
public:
	ScanLinePlotIterator(CvPlot const& kCenter, int iRadius)
	:	SquareOfPlotsIterator(kCenter, iRadius),
		m_iDeltaX(-iRadius), m_iDeltaY(iRadius),
		m_iCurrXDist(iRadius), m_iCurrYDist(iRadius)
	{
		computeNext();
	}

	ScanLinePlotIterator& operator++()
	{
		computeNext();
		return *this;
	}

	int currXDist() const
	{
		return m_iCurrXDist;
	}

	int currYDist() const
	{
		return m_iCurrYDist;
	}

protected:
	void computeNext()
	{
		m_pNext = GC.getMap().plotXY(m_pCenter, m_iDeltaX, m_iDeltaY);
		// Need to set these before updating the delta values
		m_iCurrXDist = abs(m_iDeltaX);
		m_iCurrYDist = abs(m_iDeltaY);
		if (m_iDeltaX == m_iRadius)
		{
			if (m_iDeltaY <= -m_iRadius)
			{
				if (m_iDeltaY < -m_iRadius)
					m_pNext = NULL;
				// Next plot has been set to the lower right corner
				m_iDeltaY--; // Signal that scan is complete
				return;
			}
			m_iDeltaX = -m_iRadius; // carriage return
			m_iDeltaY--; // go down one row
		}
		else m_iDeltaX++; // go right one column
		if (m_pNext == NULL) // Past the edges of the map
			computeNext();
	}
};

// The standard square and circle iterators use spiral traversal ...

class SquareIter : public SpiralPlotIterator<false>
{
public:
	SquareIter(CvPlot const& kCenter, int iRadius, bool bIncludeCenter = true) :
			SpiralPlotIterator<false>(kCenter, iRadius, bIncludeCenter) {}

	SquareIter(CvUnit const& kUnit, int iRadius, bool bIncludeCenter = true) :
			SpiralPlotIterator<false>(kUnit, iRadius, bIncludeCenter) {}

	SquareIter(int x, int y, int iRadius, bool bIncludeCenter = true) :
			SpiralPlotIterator<false>(x, y, iRadius, bIncludeCenter) {}

	SquareIter& operator++()
	{
		computeNext();
		return *this;
	}
};

class PlotCircleIter : public SpiralPlotIterator<true>
{
public:
	PlotCircleIter(CvPlot const& kCenter, int iRadius, bool bIncludeCenter = true) :
			SpiralPlotIterator<true>(kCenter, iRadius, bIncludeCenter) {}

	PlotCircleIter(CvUnit const& kUnit, int iRadius, bool bIncludeCenter = true) :
			SpiralPlotIterator<true>(kUnit, iRadius, bIncludeCenter) {}

	PlotCircleIter(int x, int y, int iRadius, bool bIncludeCenter = true) :
			SpiralPlotIterator<true>(x, y, iRadius, bIncludeCenter) {}

	PlotCircleIter& operator++()
	{
		computeNext();
		return *this;
	}
};

//mylon - 
// NearbyCityIter3city new iterator that will check in its radius to see if there are any cities
// if so it will check if that plot have a city and compare the distance 
// in order to determine if the plot effect will affect a city.
//
//class NearbyCityIter3city
//{
//public:
//	NearbyCityIter3city(CvPlot const& kPlot)
//		: m_pPlotCircle(new PlotCircleIter(kPlot, MAX_CITY_RADIUS /*4*/)),
//		m_pNext(NULL)
//	{
//		computeNext();
//	}
//
//	~NearbyCityIter3city() { delete m_pPlotCircle; }
//
//	bool hasNext() const
//	{
//		return (m_pNext != NULL);
//	}
//
//	NearbyCityIter3city& operator++()
//	{
//		computeNext();
//		return *this;
//	}
//
//	CvPlot& operator*() const
//	{
//		return *m_pNext;
//	}
//
//	CvPlot* operator->() const
//	{
//		return m_pNext;
//	}
//
//protected:
//	void computeNext()
//	{
//		//m_pNext = GC.getMap().plotXY(m_pCenter, m_iDeltaX, m_iDeltaY);
//		if (!m_pPlotCircle->hasNext())
//		{
//			m_pNext = NULL;
//			return;
//		}
//
//		CvPlot* pPlot = GC.getMap().getPlot(m_pPlotCircle->getX(), m_pPlotCircle->getY());
//		//CvPlot* pPlot = m_pPlotCircle->next();
//		if (pPlot == NULL || !pPlot->isCity())
//		{
//			computeNext();
//			return;
//		}
//		pCity = pPlot->getPlotCity();
//		//m_pNext = pPlot->getPlotCity();
//		if (m_pPlotCircle->currPlotDist() > pCity->maxRadius())
//		{
//			computeNext();
//		}
//	}
//
//private:
//	PlotCircleIter * m_pPlotCircle;
//	//int m_pPlotCircle;
//	//CityPlotTypes m_pPlotCircle;
//	//CvCity* m_pNext;
//	CvCity* pCity;
//	CvPlot*  m_pNext;
//	//CvCity const* pCity
//};
//
//class PlotCityIter
//{
//public:
//	PlotCityIter(CvPlot const& kPlot) :
//		m_ePos(NO_CITYPLOT), m_pNext(NULL),
//		m_iCenterX(kPlot.getX()), m_iCenterY(kPlot.getY())
//	{
//		computeNext();
//	}
//
//	bool hasNext() const
//	{
//		return (m_pNext != NULL);
//	}
//
//	PlotCityIter& operator++()
//	{
//		computeNext();
//		return *this;
//	}
//
//	CvPlot& operator*() const
//	{
//		return *m_pNext;
//	}
//
//	CvPlot* operator->() const
//	{
//		return m_pNext;
//	}
//
//protected:
//	void computeNext()
//	{
//		if (m_ePos + 1 >= MAX_CITY_PLOTS /*4*/)
//		{
//			m_pNext = NULL;
//			return;
//		}
//		++m_ePos;
//		CvPlot* pPlot = GC.getMap().plotCity(m_iCenterX, m_iCenterY, m_ePos);
//		if (pPlot == NULL || !pPlot->isCity())
//		{
//			computeNext();
//			return;
//		}
//		m_pNext = pPlot;
//		int cityRadius = pPlot->getPlotCity()->maxRadius();
//		if (plotDistance(m_iCenterX, m_iCenterY, pPlot->getX(), pPlot->getY()) > cityRadius)
//		{
//			computeNext();
//		}
//		
//	}
//
//private:
//	CityPlotTypes m_ePos;
//	CvPlot* m_pNext;
//	int m_iCenterX, m_iCenterY;
//};
//mylon
/*	Goes through all CvCity instances on the map that have a given CvPlot
	in their radius. (This does not imply that the cities are currently able
	to work the plot.) Uses a CityPlotIter internally b/c relevant cities
	can only exist within a city radius around the given plot. */
	//mylon
	//changes by f1rpo for 4city radius
//class NearbyCityIter3city
//{
//public:
//	NearbyCityIter3city(CvPlot const& kPlot)
//		: m_kCenter(kPlot),
//		m_kPlotCircle(*new PlotCircleIter(kPlot, MAX_CITY_RADIUS)),
//		m_pNext(NULL)
//	{
//		if (kPlot.isCityRadius()) // save time
//			computeNext();
//	}
//
//	~NearbyCityIter3city() { delete &m_kPlotCircle; }
//
//	bool hasNext() const
//	{
//		return (m_pNext != NULL);
//	}
//
//	NearbyCityIter3city& operator++()
//	{
//		computeNext();
//		return *this;
//	}
//
//	CvCity& operator*() const
//	{
//		return *m_pNext;
//	}
//
//	CvCity* operator->() const
//	{
//		return m_pNext;
//	}
//
//	// Priority value of the current city for working the center plot
//	int cityPlotPriority() const
//	{
//		/*	This is the city plot index of the current city's plot as seen
//			from a (hypothetical) city in the center. Should really be the
//			city plot index of the center as seen from the current city,
//			but it doesn't matter b/c city plot priorities are symmetrical.
//			The BtS code (at the call locations) had also relied on that. */
//		CityPlotTypes eCityPlot = m_pNext->getCityPlotIndex(m_kCenter);
//		FAssertBounds(0, MAX_CITY_PLOTS, eCityPlot);
//		return GC.getCityPlotPriority()[eCityPlot];
//	}
//
//private:
//	void computeNext()
//	{
//		if (!m_kPlotCircle.hasNext())
//		{
//			m_pNext = NULL;
//			return;
//		}
//		CvPlot& kCityPlot = *m_kPlotCircle;
//		int const iCurrDist = m_kPlotCircle.currPlotDist();
//		++m_kPlotCircle;
//		if (!kCityPlot.isCity())
//		{
//			computeNext();
//			return;
//		}
//		m_pNext = kCityPlot.getPlotCity();
//		if (iCurrDist > m_pNext->maxRadius())
//			computeNext();
//	}
//
//	CvPlot const& m_kCenter; // just for cityPlotPriority
//	PlotCircleIter& m_kPlotCircle;
//	CvCity* m_pNext;
//};

class NearbyCityIter
{
public:
	NearbyCityIter(CvPlot const& kPlot)
		: m_kCenter(kPlot),
		m_kPlotCircle(*new PlotCircleIter(kPlot, MAX_CITY_RADIUS)),
		m_pNext(NULL)
	{
		if (kPlot.isCityRadius()) // save time
			computeNext();
	}

	~NearbyCityIter() { delete &m_kPlotCircle; }

	bool hasNext() const
	{
		return (m_pNext != NULL);
	}

	NearbyCityIter& operator++()
	{
		computeNext();
		return *this;
	}

	CvCity& operator*() const
	{
		return *m_pNext;
	}

	CvCity* operator->() const
	{
		return m_pNext;
	}

	int cityPlotPriority() const
	{
		CityPlotTypes eCityPlot = m_pNext->getCityPlotIndex(m_kCenter);
		FAssertBounds(0, MAX_CITY_PLOTS, eCityPlot);
		return GC.getCityPlotPriority()[eCityPlot];
	}

private:
	void computeNext()
	{
		if (!m_kPlotCircle.hasNext())
		{
			m_pNext = NULL;
			return;
		}
		CvPlot& kCityPlot = *m_kPlotCircle;
		int const iCurrDist = m_kPlotCircle.currPlotDist();
		++m_kPlotCircle;
		if (!kCityPlot.isCity())
		{
			computeNext();
			return;
		}
		m_pNext = kCityPlot.getPlotCity();
		if (iCurrDist > m_pNext->getRadius())
			computeNext();
	}

	CvPlot const& m_kCenter; // just for cityPlotPriority
	PlotCircleIter& m_kPlotCircle;
	CvCity* m_pNext;
};
#endif
