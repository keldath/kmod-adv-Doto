#pragma once

#ifndef DEPTH_FIRST_PLOT_SEARCH_H
#define DEPTH_FIRST_PLOT_SEARCH_H

/*	advc.pf: Generic implementation of depth-first search on the directed graph
	defined by adjacent plots of GC.getMap() or a subgraph thereof.
	Policy-based design (compile-time polymorphism). */

#include "CvPlot.h" // for accessing adjacency lists

/*	bFULL_TRAVERSAL needs to be set to false to allow the visit function
	to cut the search short */
template<bool bFULL_TRAVERSAL = true>
class PlotVisitor
{
public:
	// Expose param publically
	static bool const m_bFullTraversal = bFULL_TRAVERSAL;
	bool isVisited(CvPlot const& kPlot) const
	{
		FErrorMsg("Should've been hidden by a derived-class member");
		// Should check some property of kPlot here
		return true;
	}
	bool canVisit(CvPlot const& kFrom, CvPlot const& kPlot) const
	{
		/*	Returning true means that any two plots adjacent on the map
			are treated as adjacent in the graph */
		return true;
	}
	// Return false to abort the traversal. Need to set bFULL_TRAVERSAL=false then.
	bool visit(CvPlot& kPlot)
	{
		/*	Derived classes need to change kPlot or their own state here;
			otherwise, there's no point in performing the search. */
		FErrorMsg("Should've been hidden by a derived-class member");
		return true;
	}
};

/*	This class keeps track of which plots have already been visited.
	If visited plots need to be modified, then it may be faster to let isVisited
	check for modified plot data instead; should then not derive from this class.
	When deriving from this class, the visit function should call setVisited. */
template<bool bFULL_TRAVERSAL = true>
class SingleVisitPlotVisitor : public PlotVisitor<bFULL_TRAVERSAL>
{
protected:
	EagerEnumMap<PlotNumTypes,bool> m_abVisited;

public:
	bool isVisited(CvPlot const& kPlot) const
	{
		return m_abVisited.get(kPlot.plotNum());
	}
	void setVisited(CvPlot const& kPlot)
	{
		m_abVisited.set(kPlot.plotNum(), true);
	}
	// Needs to be called when using the same instance for multiple searches
	void reset()
	{
		m_abVisited.reset();
	}
};

// V should be derived from PlotVisitor
template<class V>
class DepthFirstPlotSearch
{
public:
	// For now, we can do all we need directly in the ctor.
	DepthFirstPlotSearch(CvPlot& kStart, V& kVisitor)
	{
		// Relevant when kVisitor gets used for multiple searches
		if (kVisitor.isVisited(kStart))
			return;
		kVisitor.visit(kStart);
		/*	Explicit stack b/c memory can be an issue if a map has dimensions
			considerably larger than Huge and very large areas.
			I've run out of memory with a recursive implementation (with an attached
			debugger) after about 20000 calls on a 148x148 map generated by LPlate2's
			Eyeball Planet script. With the stack, at least the Release build should
			be pretty safe. */
		std::stack<CvPlot*> stack;
		stack.push(&kStart);
		while (!stack.empty())
		{
			CvPlot& p = *stack.top();
			stack.pop();
			FOR_EACH_ADJ_PLOT_VAR(p)
			{
				CvPlot& q = *pAdj;
				if (!kVisitor.isVisited(q) && kVisitor.canVisit(p, q))
				{
					bool bCont = kVisitor.visit(q);
					if (!V::m_bFullTraversal)
					{
						if (!bCont)
							return;
					}
					stack.push(&q);
				}
			}
		}
	}
};

#endif
