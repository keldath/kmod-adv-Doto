#pragma once

#ifndef CITY_PLOT_ITERATOR_H
#define CITY_PLOT_ITERATOR_H

// advc.enum: New classes

#include "CvMap.h"
#include "CvCity.h"

/*  Iterator over the CvPlot objects in the city radius around a plot in the center.
	NULL plots - plots whose coordinates are past the edges of the map - are skipped.
	Dereferencing the iterator yields a CvPlot reference. For accessing the city plot id
	(see aaiXYCityPlot in CvGlobals::init), use the currID function.
	Performance: Adds about twice as much overhead as the BtS loops, which can be more
	aggressively optimized. I've tried precomputing the entire sequence, but that's even
	slower (by another factor of 2; I guess b/c the plots end up getting traversed twice). */

/*	Template parameters (there are derived classes for the commonly needed combinations):
	- Only include plots that the center city can work (CvPlot::getWorkingCity).
	- Only include plots that the center city is currently working (CvCity::isWorkingPlot).
	- Randomize the order of the plot and plot id (currID) sequence through a
	  CvRandom object. Otherwise, the sequence is ordered by ascending plot id. */
enum WorkingPlotTypes
{
	ANY_CITY_PLOT,
	WORKABLE_PLOT,
	WORKING_PLOT
};

template<WorkingPlotTypes eWORKING_PLOT_TYPE = ANY_CITY_PLOT, bool bRAND_ORDER = false>
class CityPlotIterator
{
public:
	CityPlotIterator(CvCity const& kCity, bool bIncludeHomePlot = true) :
		m_ePos(NO_CITYPLOT), m_pNext(NULL),
		m_iCenterX(kCity.getX()), m_iCenterY(kCity.getY())
	{
//doto enhanced city size mylon
// added this for cases that there is no city in the loop type (we have 3 here)
//the case in the compute ises the m_pcity SO if its null it will use other default.
//which i decided to be the NUM_CITY_PLOTS_2 which is the original radius city loop count of 21.
		m_pCity = NULL;
		
		if (!bIncludeHomePlot)
			++m_ePos;
		if (eWORKING_PLOT_TYPE != ANY_CITY_PLOT)
			m_pCity = &kCity;
		if (bRAND_ORDER)
			FAssert(!bRAND_ORDER);
		computeNext();
	}

	CityPlotIterator(CvPlot const& kCenter, bool bIncludeHomePlot = true) :
		m_ePos(NO_CITYPLOT), m_pNext(NULL),
		m_iCenterX(kCenter.getX()), m_iCenterY(kCenter.getY())
	{
//doto enhanced city size mylon
// added this for cases that there is no city in the loop type (we have 3 here)
//the case in the compute ises the m_pcity SO if its null it will use other default.
//which i decided to be the NUM_CITY_PLOTS_2 which is the original radius city loop count of 21.
		m_pCity = NULL;

		if (!bIncludeHomePlot)
			++m_ePos;
		if (eWORKING_PLOT_TYPE != ANY_CITY_PLOT)
			FAssert(eWORKING_PLOT_TYPE == ANY_CITY_PLOT);
		if (bRAND_ORDER)
			FAssert(!bRAND_ORDER);
		computeNext();
	}

	// Constructor (only) for RAND_ORDER
	CityPlotIterator(int iX, int iY, CvRandom& pRandom, bool bIncludeHomePlot = true) :
		m_ePos(NO_CITYPLOT), m_pNext(NULL), m_iCenterX(iX), m_iCenterY(iY),
		m_pRandom(&pRandom)
	{	
//doto enhanced city size mylon
// added this for cases that there is no city in the loop type (we have 3 here)
//the case in the compute ises the m_pcity SO if its null it will use other default.
//which i decided to be the NUM_CITY_PLOTS_2 which is the original radius city loop count of 21.
		m_pCity = NULL;

		if (eWORKING_PLOT_TYPE != ANY_CITY_PLOT)
			FAssert(eWORKING_PLOT_TYPE == ANY_CITY_PLOT);
		if (bRAND_ORDER)
			shuffle(bIncludeHomePlot);
		else FAssert(bRAND_ORDER);

		computeNext();
	}

	~CityPlotIterator()
	{
		if (bRAND_ORDER)
			delete[] m_aiShuffledIndices;
	}

	bool hasNext() const
	{
		return (m_pNext != NULL);
	}

	CityPlotIterator& operator++()
	{
		computeNext();
		return *this;
	}

	CvPlot& operator*() const
	{
		return *m_pNext;
	}

	CvPlot* operator->() const
	{
		return m_pNext;
	}

	CityPlotTypes currID() const
	{
		if (bRAND_ORDER)
			return shuffledID();
		return m_ePos;
	}

protected:
	void computeNext()
	{
//doto enhanced city size mylon
		//if (m_ePos + 1 >= NUM_CITY_PLOTS)
		int plot_count = NUM_CITY_PLOTS_2;
		if (m_pCity != NULL)
		{
			plot_count = m_pCity->m_iMaxRadiusSize;
			FAssert(plot_count)
		}
		
		if (m_ePos + 1 >= plot_count)
		{
			m_pNext = NULL;
			return;
		}
		/*	Doing the increment after the termination check ensures that currID()
			can't go beyond the bounds of a NUM_CITY_PLOTS-size array, even if
			the iterator is incremented w/o checking hasNext. */
		++m_ePos;
		if (eWORKING_PLOT_TYPE == WORKING_PLOT)
		{
			if (!m_pCity->isWorkingPlot(m_ePos)) // Skip non-worked (also skips NULLs)
			{
				computeNext();
				return; // tail recursion
			}
		}
		m_pNext = nextCityPlot();
		if (m_pNext == NULL) // Skip NULLs
		{
			computeNext();
			return;
		}
		if (eWORKING_PLOT_TYPE == WORKABLE_PLOT) // Skip non-workable
		{
			if (m_pNext->getWorkingCity() != m_pCity) 
				computeNext();
		}
	}

private:
	CityPlotTypes m_ePos;
	CvPlot* m_pNext;
	int m_iCenterX, m_iCenterY;
	CvCity const* m_pCity; // not used if eWORKING_PLOT_TYPE is ANY_CITY_PLOT
	/*	Only used if bRAND_ORDER ...
		(Could move these into a separate class at the expense of having to
		duplicate the derived classes; see AgentIterator for reference.) */
	CvRandom* m_pRandom;
	int* m_aiShuffledIndices;

	void shuffle(bool bIncludeHomePlot)
	{
//doto enhanced city size mylon  m_pRandom->shuffle(NUM_CITY_PLOTS
// changes again from (m_pCity->m_iMaxRadiusSize
		m_aiShuffledIndices = m_pRandom->shuffle(NUM_CITY_PLOTS_2);
		if (!bIncludeHomePlot)
		{
			// Swap home plot to the front, then advance past it.
			for (int i = 0; i < NUM_CITY_PLOTS; i++)
			{
				if (m_aiShuffledIndices[i] == CITY_HOME_PLOT)
				{
					std::swap(m_aiShuffledIndices[0], m_aiShuffledIndices[i]);
					break;
				}
			}
			++m_ePos;
		}
	}

	CityPlotTypes shuffledID() const
	{
		return static_cast<CityPlotTypes>(m_aiShuffledIndices[m_ePos]);
	}

	CvPlot* nextCityPlot() const
	{
		CityPlotTypes ePos = m_ePos;
		if (bRAND_ORDER)
			ePos = shuffledID();
		return GC.getMap().plotCity(m_iCenterX, m_iCenterY, ePos);
	}
};

// Default iterator
class CityPlotIter : public CityPlotIterator<>
{
public:
	CityPlotIter(CvCity const& kCity, bool bIncludeHomePlot = true) :
		 CityPlotIterator<>(kCity, bIncludeHomePlot) {}

	CityPlotIter(CvPlot const& kCenter, bool bIncludeHomePlot = true) :
		CityPlotIterator<>(kCenter, bIncludeHomePlot) {}

	CityPlotIter& operator++()
	{
		computeNext();
		return *this;
	}
};

/*	Plots that the city is currently working. (The city center is always worked,
	but can be excluded through bIncludeHomePlot=false.) */
class WorkingPlotIter : public CityPlotIterator<WORKING_PLOT, false>
{
public:
	WorkingPlotIter(CvCity const& kCity, bool bIncludeHomePlot = true) :
		 CityPlotIterator<WORKING_PLOT, false>(kCity, bIncludeHomePlot) {}

	WorkingPlotIter& operator++()
	{
		computeNext();
		return *this;
	}
};

// Plots that the city can work (including those that are currently being worked)
class WorkablePlotIter : public CityPlotIterator<WORKABLE_PLOT, false>
{
public:
	WorkablePlotIter(CvCity const& kCity, bool bIncludeHomePlot = true) :
		 CityPlotIterator<WORKABLE_PLOT, false>(kCity, bIncludeHomePlot) {}

	WorkablePlotIter& operator++()
	{
		computeNext();
		return *this;
	}
};

// All plots in the city radius in a random order
class CityPlotRandIter : public CityPlotIterator<ANY_CITY_PLOT, true>
{
public:
	CityPlotRandIter(CvCity const& kCity, CvRandom& pRandom, bool bIncludeHomePlot = true) :
		CityPlotIterator<ANY_CITY_PLOT, true>(kCity.getX(), kCity.getY(),
		pRandom, bIncludeHomePlot) {}

	CityPlotRandIter(CvPlot const& kCenter, CvRandom& pRandom, bool bIncludeHomePlot = true) :
		CityPlotIterator<ANY_CITY_PLOT, true>(kCenter.getX(), kCenter.getY(),
		pRandom, bIncludeHomePlot) {}

	CityPlotRandIter& operator++()
	{
		computeNext();
		return *this;
	}
};

#endif