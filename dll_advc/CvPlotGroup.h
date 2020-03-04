#pragma once

// plotGroup.h

#ifndef CIV4_PLOT_GROUP_H
#define CIV4_PLOT_GROUP_H

#include "LinkedList.h"

class CvPlot;


class CvPlotGroup
{
public:

	CvPlotGroup();
	virtual ~CvPlotGroup();

	void init(int iID, PlayerTypes eOwner, CvPlot* pPlot);
	void uninit();
	void reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall=false);

	void addPlot(CvPlot* pPlot, /* advc.064d: */ bool bVerifyProduction = true);
	void removePlot(CvPlot* pPlot, bool bVerifyProduction = true);
	void recalculatePlots(/* advc.064d: */ bool bVerifyProduction = true);

	inline int getID() const { return m_iID; } // advc.inl
	void setID(int iID);

	//PlayerTypes getOwner() const;
	// advc.inl: The EXE doesn't call this, so no need for an external version.
	inline PlayerTypes getOwner() const { return m_eOwner; }

	int getNumBonuses(BonusTypes eBonus) const { return m_paiNumBonuses.get(eBonus); } // advc.inl
	bool hasBonus(BonusTypes eBonus) { return(getNumBonuses(eBonus) > 0); } // advc.inl
	//< Building Resource Converter Start >
	//f1rpo 096 - added a var here to pass an param to avoid a loop - keldath
	void changeNumBonuses(BonusTypes eBonus, int iChange,
       bool bUpdateBuildings = true);
	//< Building Resource Converter End   >
	void verifyCityProduction(); // advc.064d

	void insertAtEndPlots(XYCoords xy);
	CLLNode<XYCoords>* deletePlotsNode(CLLNode<XYCoords>* pNode);
	// advc.inl
	inline CLLNode<XYCoords>* nextPlotsNode(CLLNode<XYCoords>* pNode)
	{
		return m_plots.next(pNode);
	} // <advc.003s> Safer in 'for' loops
	inline CLLNode<XYCoords> const* nextPlotsNode(CLLNode<XYCoords> const* pNode)
	{
		return m_plots.next(pNode);
	} // </advc.003s>
	int getLengthPlots() { return m_plots.getLength(); } // advc.inl
	CLLNode<XYCoords>* headPlotsNode() { return m_plots.head(); } // advc.inl

	// for serialization
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

protected:
	static int m_iRecalculating; // advc.064d
	int m_iID;
	PlayerTypes m_eOwner;
	EnumMap<BonusTypes,int> m_paiNumBonuses; // advc.enum
	CLinkList<XYCoords> m_plots;
};

#endif
