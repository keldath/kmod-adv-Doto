#pragma once

#ifndef CIV4_AREA_H
#define CIV4_AREA_H

class CvCity;
class CvPlot;


class CvArea
{
public:

	CvArea();
	void init(bool bWater);
	virtual ~CvArea();
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	int calculateTotalBestNatureYield() const;															// Exposed to Python

	int countCoastalLand() const;																		// Exposed to Python
	int countNumUniqueBonusTypes() const;																// Exposed to Python
	int countHasReligion(ReligionTypes eReligion, PlayerTypes eOwner = NO_PLAYER) const;				// Exposed to Python
	int countHasCorporation(CorporationTypes eCorporation, PlayerTypes eOwner = NO_PLAYER) const;		// Exposed to Python																					// Exposed to Python

	__forceinline int getID() const { return m_iID; }													// Exposed to Python
	void setID(int iID);
	
	__forceinline bool isWater() const { return m_bWater; }												// Exposed to Python
	inline bool isLake() const																			// Exposed to Python
	{	// <advc.030>
		//return (isWater() && (getNumTiles() <= GC.getLAKE_MAX_AREA_SIZE()));
		return m_bLake;		
	}
	void updateLake(bool bCheckRepr = true);
	void setRepresentativeArea(int eArea);
	// Should only be needed for computing the equivalence classes
	int getRepresentativeArea() const;
	bool canBeEntered(CvArea const& kFrom, CvUnit const* u = NULL) const;
	// </advc.030>

	inline PlotNumTypes getNumTiles() const																// Exposed to Python
	{ return (PlotNumTypes)m_iNumTiles; } // advc.enum: Return type was int
	void changeNumTiles(int iChange);
	void changeNumOwnedTiles(int iChange);
	inline PlotNumTypes getNumOwnedTiles() const														// Exposed to Python
	{ return (PlotNumTypes)m_iNumOwnedTiles; } // advc.enum
	inline PlotNumTypes getNumUnownedTiles() const														// Exposed to Python
	{
		return (PlotNumTypes)(getNumTiles() - getNumOwnedTiles()); // advc.enum
	}
	// <advc.300>
	std::pair<int,int> countOwnedUnownedHabitableTiles( // advc.021b: Exposed to Python as getNumHabitableTiles
			bool bIgnoreBarb = false) const;
	int getNumCivCities() const
	{
		return getNumCities() - getCitiesPerPlayer(BARBARIAN_PLAYER);
	}
	int countCivs(bool bSubtractOCC = false) const; // with at least 1 city
	bool hasAnyAreaPlayerBonus(BonusTypes eBonus) const;
	int getBarbarianCitiesEverCreated() const;
	void reportBarbarianCityCreated();
	// </advc.300>

	void changeNumRiverEdges(int iChange);																// Exposed to Python
	inline int getNumRiverEdges() const { return m_iNumRiverEdges; }									// Exposed to Python

	void changeNumStartingPlots(int iChange);
	// advc.enum: return type was int
	inline PlotNumTypes getNumStartingPlots() const { return (PlotNumTypes)m_iNumStartingPlots; }		// Exposed to Python
	
	inline int getNumUnits() const { return m_iNumUnits; }												// Exposed to Python
	inline int getNumCities() const { return m_iNumCities; }											// Exposed to Python
	int getUnitsPerPlayer(PlayerTypes eIndex) const { return m_aiUnitsPerPlayer.get(eIndex); }			// Exposed to Python
	void changeUnitsPerPlayer(PlayerTypes eIndex, int iChange);
	// advc: Unused; removed.
	/*int getAnimalsPerPlayer(PlayerTypes eIndex) const;												// Exposed to Python
	void changeAnimalsPerPlayer(PlayerTypes eIndex, int iChange);*/
	int getCitiesPerPlayer(PlayerTypes eIndex,															// Exposed to Python
			bool bCheckAdjacentCoast = false) const; // advc.030b
	void changeCitiesPerPlayer(PlayerTypes eIndex, int iChange);
	int getTotalPopulation() const { return m_iTotalPopulation; }										// Exposed to Python
	int getPopulationPerPlayer(PlayerTypes eIndex) const												// Exposed to Python
	{
		return m_aiPopulationPerPlayer.get(eIndex);
	}
	void changePopulationPerPlayer(PlayerTypes eIndex, int iChange);

	int getBuildingGoodHealth(PlayerTypes eIndex) const													// Exposed to Python
	{
		return m_aiBuildingGoodHealth.get(eIndex);
	}
	void changeBuildingGoodHealth(PlayerTypes eIndex, int iChange);
	int getBuildingBadHealth(PlayerTypes eIndex) const													// Exposed to Python
	{
		return m_aiBuildingBadHealth.get(eIndex);
	}
	void changeBuildingBadHealth(PlayerTypes eIndex, int iChange);
	int getBuildingHappiness(PlayerTypes eIndex) const													// Exposed to Python
	{
		return m_aiBuildingHappiness.get(eIndex);
	}
	void changeBuildingHappiness(PlayerTypes eIndex, int iChange);
	// <advc.310>
	int getTradeRoutes(PlayerTypes eIndex) const														// Exposed to Python
	{
		return m_aiTradeRoutes.get(eIndex);
	}
	void changeTradeRoutes(PlayerTypes eIndex, int iChange);
	// </advc.310>
	int getFreeSpecialist(PlayerTypes eIndex) const														// Exposed to Python
	{
		return m_aiFreeSpecialist.get(eIndex);
	}
	void changeFreeSpecialist(PlayerTypes eIndex, int iChange);

	int getPower(PlayerTypes eIndex) const																// Exposed to Python
	{
		return m_aiPower.get(eIndex);
	}
	void changePower(PlayerTypes eIndex, int iChange);

	int getBestFoundValue(PlayerTypes eIndex) const														// Exposed to Python
	{
		return m_aiBestFoundValue.get(eIndex);
	}
	void setBestFoundValue(PlayerTypes eIndex, int iNewValue);
	
	//DPII < Maintenance Modifiers >
    int getMaintenanceModifier(PlayerTypes eIndex) const;
    void changeMaintenanceModifier(PlayerTypes eIndex, int iChange);

 /*   int getHomeAreaMaintenanceModifier(PlayerTypes eIndex) const;
    void changeHomeAreaMaintenanceModifier(PlayerTypes eIndex, int iChange);
    void setHomeAreaMaintenanceModifier(PlayerTypes eIndex, int iNewValue);

    int getOtherAreaMaintenanceModifier(PlayerTypes eIndex) const;
    void changeOtherAreaMaintenanceModifier(PlayerTypes eIndex, int iChange);
    void setOtherAreaMaintenanceModifier(PlayerTypes eIndex, int iNewValue);
*/
//    int getTotalAreaMaintenanceModifier(PlayerTypes ePlayer) const;
/*
    bool isHomeArea(PlayerTypes eIndex) const;
    void setHomeArea(PlayerTypes ePlayer, CvArea* pOldHomeArea,CvArea* pNewHomeArea);
  */  //DPII < Maintenance Modifiers >

	inline int getNumUnrevealedTiles(TeamTypes eIndex) const											// Exposed to Python
	{
		return getNumTiles() - getNumRevealedTiles(eIndex);
	}
	int getNumRevealedTiles(TeamTypes eIndex) const														// Exposed to Python
	{
		return m_aiNumRevealedTiles.get(eIndex);
	}
	void changeNumRevealedTiles(TeamTypes eIndex, int iChange);

	int getCleanPowerCount(TeamTypes eIndex) const
	{
		return m_aiCleanPowerCount.get(eIndex);
	}
	bool isCleanPower(TeamTypes eIndex) const															// Exposed to Python
	{
		return (getCleanPowerCount(eIndex) > 0);
	}
	void changeCleanPowerCount(TeamTypes eIndex, int iChange);

	int getBorderObstacleCount(TeamTypes eIndex) const
	{
		return m_aiBorderObstacleCount.get(eIndex);
	}
	bool isBorderObstacle(TeamTypes eIndex) const														// Exposed to Python
	{
		return (getBorderObstacleCount(eIndex) > 0);
	}
	void changeBorderObstacleCount(TeamTypes eIndex, int iChange);

	AreaAITypes getAreaAIType(TeamTypes eIndex) const													// Exposed to Python
	{
		return m_aeAreaAIType.get(eIndex);
	}
	void setAreaAIType(TeamTypes eIndex, AreaAITypes eNewValue);
	/*	advc.003u: Renamed these two from get/setTargetCity
		and changed types to CvCityAI. Target cities are an AI thing. */
	CvCityAI* AI_getTargetCity(PlayerTypes eIndex) const;												// Exposed to Python
	void AI_setTargetCity(PlayerTypes eIndex, CvCity* pNewValue);

	int getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const								// Exposed to Python
	{
		return m_aaiYieldRateModifier.get(eIndex1, eIndex2);
	}
	void changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const								// Exposed to Python
	{
		return m_aaiNumTrainAIUnits.get(eIndex1, eIndex2);
	}
	void changeNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange);

	int getNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const;									// Exposed to Python
	void changeNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange);

	int getNumBonuses(BonusTypes eBonus) const															// Exposed to Python
	{
		return m_aiBonuses.get(eBonus);
	}
	int getNumTotalBonuses() const																		// Exposed to Python
	{
		return m_aiBonuses.getTotal();
	}
	bool isAnyBonus() const { return m_aiBonuses.hasContent(); } // advc.opt
	void changeNumBonuses(BonusTypes eBonus, int iChange);
	// advc.opt: No longer used
	/*int getNumImprovements(ImprovementTypes eImprovement) const;										// Exposed to Python
	void changeNumImprovements(ImprovementTypes eImprovement, int iChange);*/

protected:

	int m_iID;
	int m_iNumTiles;
	int m_iNumOwnedTiles;
	int m_iNumRiverEdges;
	int m_iNumUnits;
	int m_iNumCities;
	int m_iTotalPopulation;
	int m_iNumStartingPlots;
	int m_iBarbarianCitiesEver; // advc.300

	bool m_bWater;
	// <advc.030>
	bool m_bLake;
	int m_iRepresentativeAreaId;
	// </advc.030>  // <advc.enum> Tbd.: Use <...,short> for all of these?
	EnumMap<PlayerTypes,int> m_aiUnitsPerPlayer;
	EnumMap<PlayerTypes,int> m_aiCitiesPerPlayer;
	EnumMap<PlayerTypes,int> m_aiPopulationPerPlayer;
	EnumMap<PlayerTypes,int> m_aiBuildingGoodHealth;
	EnumMap<PlayerTypes,int> m_aiBuildingBadHealth;
	EnumMap<PlayerTypes,int> m_aiBuildingHappiness;
	EnumMap<PlayerTypes,int> m_aiTradeRoutes; // advc.310
	EnumMap<PlayerTypes,int> m_aiFreeSpecialist;
	EnumMap<PlayerTypes,int> m_aiPower;
	EnumMap<PlayerTypes,int> m_aiBestFoundValue;
	//DPII < Maintenance Modifiers 096 style>
	EnumMap<PlayerTypes,int> m_aiMaintenanceModifier;
/*	EnumMap<PlayerTypes,int> m_aiHomeAreaMaintenanceModifier;
	EnumMap<PlayerTypes,int> m_aiOtherAreaMaintenanceModifier;
	EnumMap<PlayerTypes, bool> m_abHomeArea;*/
/*	int* m_aiMaintenanceModifier;
	int* m_aiHomeAreaMaintenanceModifier;
	int* m_aiOtherAreaMaintenanceModifier;
	bool* m_abHomeArea;*/
	//DPII < Maintenance Modifiers >
	EnumMap<TeamTypes,int> m_aiNumRevealedTiles;
	EnumMap<TeamTypes,int> m_aiCleanPowerCount;
	EnumMap<TeamTypes,int> m_aiBorderObstacleCount;
	EnumMap<TeamTypes,AreaAITypes> m_aeAreaAIType;
	EnumMap<BonusTypes,int> m_aiBonuses;
	//EnumMap<ImprovementTypes,int> m_aiImprovements; // advc.opt: was only used for CvMapGenerator::addGoodies
	EnumMap2D<PlayerTypes,YieldTypes,short> m_aaiYieldRateModifier;
	EnumMap2D<PlayerTypes,UnitAITypes,int> m_aaiNumTrainAIUnits;
	EnumMap2D<PlayerTypes,UnitAITypes,int> m_aaiNumAIUnits; // </advc.enum>

	IDInfo* m_aTargetCities;

	void uninit();
	void reset(int iID = 0, bool bWater = false, bool bConstructorCall = false);
};

#endif
