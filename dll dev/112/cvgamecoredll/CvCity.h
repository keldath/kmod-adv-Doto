#pragma once

// city.h

#ifndef CIV4_CITY_H
#define CIV4_CITY_H

#include "CvDLLEntity.h"

class CvPlot;
class CvPlotGroup;
class CvArea;
class CvGenericBuilding;
class CvArtInfoBuilding;
class CvCityAI; // advc.003u
class CvCivilization; // advc.003w


class CvCity : public CvDLLEntity
{
public:
	virtual ~CvCity();

	void setupGraphical();
	void kill(bool bUpdatePlotGroups, /* advc.001: */ bool bBumpUnits = true);									// Exposed to Python
	void doTurn();
	void doRevolt(); // advc: previously in CvPlot::doCulture
	// K-Mod. public for the "insert culture" espionage mission. (I've also changed the functionality of it quite a bit.)
	void doPlotCultureTimes100(bool bUpdate, PlayerTypes ePlayer, int iCultureRateTimes100, bool bCityCulture);

	bool isCitySelected();
	DllExport bool canBeSelected() const;
	DllExport void updateSelectedCity(bool bTestProduction);
	void setInvestigate(bool b); // advc.103

	void updateYield();
	void updateVisibility();

	void createGreatPeople(UnitTypes eGreatPersonUnit, bool bIncrementThreshold,								// Exposed to Python
			 bool bIncrementExperience) const;
	void doTask(TaskTypes eTask, int iData1 = -1, int iData2 = -1, bool bOption = false,						// Exposed to Python
			bool bAlt = false, bool bShift = false, bool bCtrl = false);
	void chooseProduction(UnitTypes eTrainUnit = NO_UNIT,														// Exposed to Python
			BuildingTypes eConstructBuilding = NO_BUILDING,
			ProjectTypes eCreateProject = NO_PROJECT,
			bool bFinish = false, bool bFront = false);
	// <advc.003u> Moved from CvCityAI b/c it's also used to trigger human choose-production popups
	bool isChooseProductionDirty() const
	{
		return m_bChooseProductionDirty;
	}
	void setChooseProductionDirty(bool bNewValue)
	{
		m_bChooseProductionDirty = bNewValue;
	} // </advc.003u>

	CityPlotTypes getCityPlotIndex(CvPlot const* pPlot) const;													// Exposed to Python
	CvPlot* getCityIndexPlot(CityPlotTypes ePlot) const;														// Exposed to Python

	bool canWork(CvPlot const* pPlot) const;																	// Exposed to Python
	void verifyWorkingPlot(CityPlotTypes ePlot);
	void verifyWorkingPlots();
	void clearWorkingOverride(CityPlotTypes ePlot);																// Exposed to Python
	int countNumImprovedPlots(ImprovementTypes eImprovement = NO_IMPROVEMENT, bool bPotential = false) const;																			// Exposed to Python
	int countNumWaterPlots() const;																				// Exposed to Python
	int countNumRiverPlots() const;																				// Exposed to Python

	int findPopulationRank() const;																				// Exposed to Python
	int findBaseYieldRateRank(YieldTypes eYield) const;															// Exposed to Python
	int findYieldRateRank(YieldTypes eYield) const;																// Exposed to Python
	int findCommerceRateRank(CommerceTypes eCommerce) const;													// Exposed to Python
/************************************************************************************************/
/* REVDCM                                 05/05/10                                phungus420    */
/*                                                                                              */
/* CanTrain                                                                                     */
/************************************************************************************************/
	bool isForceObsoleteUnitClassAvailable(UnitTypes eUnit) const;						// Exposed to Python
	bool isPlotTrainable(UnitTypes eUnit, bool bContinue, bool bTestVisible) const;						// Exposed to Python
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/

	UnitTypes allUpgradesAvailable(UnitTypes eUnit, int iUpgradeCount = 0,										// Exposed to Python
			BonusTypes eAssumeVailable = NO_BONUS) const; // advc.001u
	bool isWorldWondersMaxed() const;																			// Exposed to Python
	bool isTeamWondersMaxed() const;																			// Exposed to Python
	bool isNationalWondersMaxed() const;																		// Exposed to Python
	int getNumNationalWondersLeft() const; // advc.004w, advc.131
	bool isBuildingsMaxed() const;																				// Exposed to Python

	void verifyProduction(); // advc.064d: public wrapper for doCheckProduction
	bool canTrain(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false,							// Exposed to Python
			bool bIgnoreCost = false, bool bIgnoreUpgrades = false,
			bool bCheckAirUnitCap = true, // advc.001b
			BonusTypes eAssumeVailable = NO_BONUS) const; // advc.001u
	bool canUpgradeTo(UnitTypes eUnit) const; // advc.001b
	bool canTrain(UnitCombatTypes eUnitCombat) const;
	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false,											// Exposed to Python
			bool bTestVisible = false, bool bIgnoreCost = false,
			bool bIgnoreTech = false) const; // K-Mod
//Tholish UnbuildableBuildingDeletion START
	bool canKeep(BuildingTypes eBuilding) const;
//Tholish UnbuildableBuildingDeletion END
	bool canCreate(ProjectTypes eProject, bool bContinue = false, bool bTestVisible = false) const;				// Exposed to Python
	bool canMaintain(ProcessTypes eProcess, bool bContinue = false) const;										// Exposed to Python
	bool canJoin() const;																						// Exposed to Python

	int getFoodTurnsLeft() const;																				// Exposed to Python
	bool isProduction() const { return (headOrderQueueNode() != NULL); } // advc.inl							// Exposed to Python
	bool isProductionLimited() const;																			// Exposed to Python
	bool isProductionUnit() const;																				// Exposed to Python
	bool isProductionBuilding() const;																			// Exposed to Python
	bool isProductionProject() const;																			// Exposed to Python
	bool isProductionProcess() const;																			// Exposed to Python

	bool canContinueProduction(OrderData order);																// Exposed to Python
	int getProductionExperience(UnitTypes eUnit = NO_UNIT) const;												// Exposed to Python
	void addProductionExperience(CvUnit* pUnit, bool bConscript = false);										// Exposed to Python

	UnitTypes getProductionUnit() const;																		// Exposed to Python
	UnitAITypes getProductionUnitAI() const;																	// Exposed to Python
	BuildingTypes getProductionBuilding() const;																// Exposed to Python
	ProjectTypes getProductionProject() const;																	// Exposed to Python
	ProcessTypes getProductionProcess() const;																	// Exposed to Python
	const wchar* getProductionName() const;																		// Exposed to Python
	const wchar* getProductionNameKey() const;																	// Exposed to Python

	bool isFoodProduction() const;																				// Exposed to Python
	bool isFoodProduction(UnitTypes eUnit) const;																// Exposed to Python
	int getFirstUnitOrder(UnitTypes eUnit) const;																// Exposed to Python
	int getFirstBuildingOrder(BuildingTypes eBuilding) const;													// Exposed to Python
	int getFirstProjectOrder(ProjectTypes eProject) const;														// Exposed to Python
	int getNumTrainUnitAI(UnitAITypes eUnitAI) const;															// Exposed to Python

	int getProduction() const;																					// Exposed to Python
	int getProductionNeeded() const;																			// Exposed to Python
	int getProductionNeeded(UnitTypes eUnit) const;
	int getProductionNeeded(BuildingTypes eBuilding) const;
	int getProductionNeeded(ProjectTypes eProject) const;
	//int getGeneralProductionTurnsLeft() const; // advc: Redundant; use the function below.					// Exposed to Python
	int getProductionTurnsLeft() const;																			// Exposed to Python
	int getProductionTurnsLeft(UnitTypes eUnit, int iNum) const;												// Exposed to Python
	int getProductionTurnsLeft(BuildingTypes eBuilding, int iNum) const;										// Exposed to Python
	int getProductionTurnsLeft(ProjectTypes eProject, int iNum) const;											// Exposed to Python
	int getProductionTurnsLeft(int iProductionNeeded, int iProduction,
			int iFirstProductionDifference, int iProductionDifference) const;
	int sanitizeProductionTurns(int iTurns, OrderTypes eOrder = NO_ORDER,
			int iData = -1, bool bAssert = false) const; // advc.004x
	void setProduction(int iNewValue);																			// Exposed to Python
	void changeProduction(int iChange);																			// Exposed to Python

	int getProductionModifier() const;																			// Exposed to Python
	int getProductionModifier(UnitTypes eUnit) const;															// Exposed to Python
	int getProductionModifier(BuildingTypes eBuilding) const;													// Exposed to Python
	int getProductionModifier(ProjectTypes eProject) const;														// Exposed to Python
	// advc.003j: Vanilla Civ 4 declaration that never had an implementation
	//int getOverflowProductionDifference(int iProductionNeeded, int iProduction, int iProductionModifier, int iDiff, int iModifiedProduction) const;
	int getProductionDifference(int iProductionNeeded, int iProduction,
			int iProductionModifier, bool bFoodProduction, bool bOverflow,
			// <advc.064bc>
			bool bIgnoreFeatureProd = false, bool bIgnoreYieldRate = false,
			bool bForceFeatureProd = false, int* piFeatureProd = NULL) const;
			// </advc.064bc>
	int getCurrentProductionDifference(bool bIgnoreFood, bool bOverflow,										// Exposed to Python
			// <advc.064bc>
			bool bIgnoreFeatureProd = false, bool bIgnoreYieldRate = false,
			bool bForceFeatureProd = false, int* iFeatureProdReturn = NULL) const;
			// </advc.064bc>
	int getExtraProductionDifference(int iExtra) const															// Exposed to Python
	{	// advc.inl:
		return getExtraProductionDifference(iExtra, getProductionModifier());
	}

	bool canHurry(HurryTypes eHurry, bool bTestVisible = false) const;											// Exposed to Python
	void hurry(HurryTypes eHurry);																				// Exposed to Python
	// <advc.064b>
	int overflowCapacity(int iProductionModifier, int iPopulationChange = 0) const;
	int computeOverflow(int iRawOverflow, int iProductionModifier, OrderTypes eOrderType,
			int* piProductionGold = NULL, int* piLostProduction = NULL,
			int iPopulationChange = 0) const;
	inline int minPlotProduction() const
	{	// Let pop-hurry ignore guaranteed production
		return 0;/*GC.getInfo(YIELD_PRODUCTION).getMinCity()*/
	} // (exposed to Python) </advc.064b>  <advc.064>
	bool hurryOverflow(HurryTypes eHurry, int* piProduction, int* piGold,
			bool bCountThisTurn = false) const;		// (exposed to Python)
	// </advc.064>
	// <advc.912d>
	bool canPopRush() const;
	void changePopRushCount(int iChange);
	// </advc.912d>
	UnitTypes getConscriptUnit() const;																			// Exposed to Python
	CvUnit* initConscriptedUnit();
	int getConscriptPopulation() const;																			// Exposed to Python
	int conscriptMinCityPopulation() const;																		// Exposed to Python
	int flatConscriptAngerLength() const;																		// Exposed to Python
	bool canConscript() const;																					// Exposed to Python
	void conscript();																							// Exposed to Python
/*************************************************************************************************/
/** INFLUENCE_DRIVEN_WAR                   04/16/09                                johnysmith    */
/**                                                                                              */
/** Original Author Moctezuma              Start                                                 */
/*************************************************************************************************/
	// ------ BEGIN InfluenceDrivenWar -------------------------------
	void emergencyConscript();
	// ------ END InfluenceDrivenWar ---------------------------------
/*************************************************************************************************/
/** INFLUENCE_DRIVEN_WAR                   04/16/09                                johnysmith    */
/**                                                                                              */
/** Original Author Moctezuma              End                                                   */
/*************************************************************************************************/
	int getBonusHealth(BonusTypes eBonus) const;																// Exposed to Python
	int getBonusHappiness(BonusTypes eBonus) const;																// Exposed to Python
	int getBonusPower(BonusTypes eBonus, bool bDirty) const;													// Exposed to Python
	int getBonusYieldRateModifier(YieldTypes eIndex, BonusTypes eBonus) const;									// Exposed to Python

	void processBonus(BonusTypes eBonus, int iChange);
	void processBuilding(BuildingTypes eBuilding, int iChange, bool bObsolete = false, bool checkKeep = true);
///prereqMust+tholish
	void UNprocessBuilding(BuildingTypes eBuilding, int iChange, bool bObsolete = false);
///prereqMust+tholish -- unused -older trial that worked
	void defuseBuilding(BuildingTypes eBuilding);
	void activateBuilding(BuildingTypes eBuilding);
	void processProcess(ProcessTypes eProcess, int iChange);
	void processSpecialist(SpecialistTypes eSpecialist, int iChange);
	void processVoteSource(VoteSourceTypes eVoteSource, bool bActive);

	HandicapTypes getHandicapType() const;																		// Exposed to Python
	CivilizationTypes getCivilizationType() const;																// Exposed to Python
	CvCivilization const& getCivilization() const; // advc.003w
	LeaderHeadTypes getPersonalityType() const;																	// Exposed to Python
	DllExport ArtStyleTypes getArtStyleType() const;															// Exposed to Python
	CitySizeTypes getCitySizeType() const;																		// Exposed to Python
	DllExport const CvArtInfoBuilding* getBuildingArtInfo(BuildingTypes eBuilding) const;
	DllExport float getBuildingVisibilityPriority(BuildingTypes eBuilding) const;

	bool hasTrait(TraitTypes eTrait) const;																		// Exposed to Python
	bool isBarbarian() const;																					// Exposed to Python
	bool isHuman() const;																						// Exposed to Python
	DllExport bool isVisible(TeamTypes eTeam, bool bDebug) const;												// Exposed to Python
	// advc: Make bDebug=false the default
	inline bool isVisible(TeamTypes eTeam) const
	{
		return isVisible(eTeam, false);
	}

	bool isCapital() const;																						// Exposed to Python
	bool isPrereqBonusSea() const; // advc
	/* advc: -1 means use MIN_WATER_SIZE_FOR_OCEAN. Removed MIN_WATER_SIZE_FOR_OCEAN
	   from all calls to this function (except those from Python). */
	bool isCoastal(int iMinWaterSize = -1) const;																// Exposed to Python
	bool isDisorder() const;																					// Exposed to Python
	bool isHolyCity(ReligionTypes eIndex) const;																// Exposed to Python
	bool isHolyCity() const;																					// Exposed to Python
	bool hasShrine(ReligionTypes eReligion) const;
	bool isHeadquarters(CorporationTypes eIndex) const;															// Exposed to Python
	bool isHeadquarters() const;																				// Exposed to Python
	void setHeadquarters(CorporationTypes eIndex);

	int getOvercrowdingPercentAnger(int iExtra = 0) const;														// Exposed to Python
	int getNoMilitaryPercentAnger() const;																		// Exposed to Python
	int getCulturePercentAnger() const;																			// Exposed to Python
	int getReligionPercentAnger() const;																		// Exposed to Python
	/*  advc.104: Moved parts of getReligionPercentAnger() into a subroutine.
		getReligionPercentAnger(PlayerTypes) doesn't check if the city owner is
		at war with ePlayer; can be used for predicting anger caused by a DoW. */
	double getReligionPercentAnger(PlayerTypes ePlayer) const;
	int getHurryPercentAnger(int iExtra = 0) const;																// Exposed to Python
	int getConscriptPercentAnger(int iExtra = 0) const;															// Exposed to Python
	int getDefyResolutionPercentAnger(int iExtra = 0) const;
	int getWarWearinessPercentAnger() const;																	// Exposed to Python
	int getLargestCityHappiness() const;																		// Exposed to Python
	int getVassalHappiness() const;																				// Exposed to Python
	int getVassalUnhappiness() const;																			// Exposed to Python
	int unhappyLevel(int iExtra = 0) const;																		// Exposed to Python
	int happyLevel() const;																						// Exposed to Python
	int angryPopulation(int iExtra = 0, /* advc.104: */ bool bIgnoreCultureRate = false) const;					// Exposed to Python
	int visiblePopulation() const;
	int totalFreeSpecialists() const;																			// Exposed to Python
	int extraPopulation() const;																				// Exposed to Python
	int extraSpecialists() const;																				// Exposed to Python
	int extraFreeSpecialists() const;																			// Exposed to Python

	int unhealthyPopulation(bool bNoAngry = false, int iExtra = 0) const;										// Exposed to Python
	int totalGoodBuildingHealth() const;																		// Exposed to Python
	int totalBadBuildingHealth() const;																			// Exposed to Python
	int goodHealth() const;																						// Exposed to Python
	int badHealth(bool bNoAngry = false, int iExtra = 0) const;													// Exposed to Python
	int healthRate(bool bNoAngry = false, int iExtra = 0) const;												// Exposed to Python
	int foodConsumption(bool bNoAngry = false, int iExtra = 0) const;											// Exposed to Python
	int foodDifference(bool bBottom = true, bool bIgnoreProduction = false) const;	// Exposed to Python, K-Mod added bIgnoreProduction
	int growthThreshold(/* advc.064b: */int iPopulationChange = 0) const;										// Exposed to Python

	int productionLeft() const { return (getProductionNeeded() - getProduction()); } // advc.inl				// Exposed to Python
	int hurryCost(bool bExtra) const // advc.inl																// Exposed to Python
	{
		return getHurryCost(bExtra, productionLeft(),
				getHurryCostModifier(), getProductionModifier());
	}
	int getHurryCostModifier(bool bIgnoreNew = false) const;
	int hurryGold(HurryTypes eHurry) const // advc.inl															// Exposed to Python
	{
		return getHurryGold(eHurry, hurryCost(false));
	}
	int hurryPopulation(HurryTypes eHurry) const // advc.inl													// Exposed to Python
	{
		return getHurryPopulation(eHurry, hurryCost(true));
	}
	int hurryProduction(HurryTypes eHurry) const;																// Exposed to Python
	int flatHurryAngerLength() const;																			// Exposed to Python
	int hurryAngerLength(HurryTypes eHurry) const;																// Exposed to Python
	int maxHurryPopulation() const;																				// Exposed to Python

	static int cultureDistance(int iDX, int iDY); // advc: static												// Exposed to Python
	enum GrievanceTypes { GRIEVANCE_HURRY, GRIEVANCE_CONSCRIPT, GRIEVANCE_RELIGION }; // advc.101
	int cultureStrength(PlayerTypes ePlayer,																	// Exposed to Python
			std::vector<GrievanceTypes>* paGrievances = NULL) const; // advc.101
	int cultureGarrison(PlayerTypes ePlayer) const;																// Exposed to Python
	PlayerTypes calculateCulturalOwner() const; // advc.099c

	int getNumBuilding(BuildingTypes eIndex) const;																// Exposed to Python
	int getNumBuilding(BuildingClassTypes eBuildingClass) const; // advc.003w
	int getNumActiveBuilding(BuildingTypes eIndex) const;														// Exposed to Python
	bool hasActiveWorldWonder() const																			// Exposed to Python
	{
		return (getNumActiveWorldWonders(1) > 0); // advc
	}
	// UNOFFICIAL_PATCH, Bugfix, 03/04/10, Mongoose & jdog5000:
	int getNumActiveWorldWonders(/* advc: */ int iStopCountAt = MAX_INT,
			PlayerTypes eOwner = NO_PLAYER) const; // advc.104d: Hypothetical owner

	int getReligionCount() const;																				// Exposed to Python
	int getCorporationCount() const;																			// Exposed to Python
	static CvCity* fromIDInfo(IDInfo id); // advc
	// <advc.inl>
	DllExport inline int getID() const { return m_iID; }														// Exposed to Python
	inline int getIndex() const { return (getID() & FLTA_INDEX_MASK); }
	DllExport inline IDInfo getIDInfo() const { return IDInfo(getOwner(), getID()); }
	// </advc.inl>
	void setID(int iID);
	int plotNum() const; // advc.104

	int getXExternal() const; // advc.inl: Exported through .def file											// Exposed to Python
	inline int getX() const { return m_iX; } // advc.inl: Renamed from getX_INLINE
	int getYExternal() const; // advc.inl: Exported through .def file											// Exposed to Python
	inline int getY() const { return m_iY; } // advc.inl: Renamed from getY_INLINE

	bool at(int iX, int iY) const  { return (getX() == iX && getY() == iY); } // advc.inl						// Exposed to Python
	bool at(CvPlot const* pPlot) const // advc: const CvPlot*													// Exposed to Python as atPlot
	{
		return (plot() == pPlot); // advc.inl
	}  // <advc>
	bool at(CvPlot const& kPlot) const
	{
		return (plot() == &kPlot);
	} // </advc>
	DllExport __forceinline CvPlot* plot() const { return m_pPlot; } // advc.opt: cached						// Exposed to Python
	__forceinline CvPlot& getPlot() const { return *m_pPlot; } // advc
	void updatePlot(); // advc.opt
	CvPlotGroup* plotGroup(PlayerTypes ePlayer) const;
	bool isConnectedTo(CvCity const* pCity) const;																// Exposed to Python
	bool isConnectedToCapital(PlayerTypes ePlayer = NO_PLAYER) const;											// Exposed to Python
	// <advc>
	inline CvArea* area() const { return m_pArea; }																// Exposed to Python
	//int getArea() const;
	inline CvArea& getArea() const { return *m_pArea; }
	inline bool isArea(CvArea const& kArea) const { return (area() == &kArea); }
	inline bool sameArea(CvCity const& kOther) const { return (area() == kOther.area()); }
	void updateArea();
	// </advc>
	// BETTER_BTS_AI_MOD, 01/02/09, jdog5000: START
	CvArea* waterArea(bool bNoImpassable = false) const;														// Exposed to Python
	CvArea* secondWaterArea() const;
	CvArea* sharedWaterArea(CvCity* pCity) const;
	bool isBlockaded() const;
	// BETTER_BTS_AI_MOD: END

	CvPlot* getRallyPlot() const;																				// Exposed to Python
	void setRallyPlot(CvPlot* pPlot);

	// advc.inl: Inlined most of the getters below (w/o adding inline keyword though)

	int getGameTurnFounded() const { return m_iGameTurnFounded; }												// Exposed to Python
	void setGameTurnFounded(int iNewValue);
	int getGameTurnAcquired() const { return m_iGameTurnAcquired; }												// Exposed to Python
	void setGameTurnAcquired(int iNewValue);

	inline int getPopulation() const { return m_iPopulation; }													// Exposed to Python
	void setPopulation(int iNewValue);																			// Exposed to Python
	void changePopulation(int iChange);		
	/* Population Limit ModComp - Beginning */
	int getPopulationLimit() const;														// Exposed to Python
	int getPopulationLimitChange() const;														// Exposed to Python
/*	int getPopulationLimit() const;														// Exposed to Python
	int getPopulationLimitChange() const;*/														// Exposed to Python
	void setPopulationLimitChange(int iNewValue);										// Exposed to Python
	void changePopulationLimitChange(int iChange);										// Exposed to Python
	/* Population Limit ModComp - End */																	// Exposed to Python
	long getRealPopulation() const;																				// Exposed to Python
	int getHighestPopulation() const { return m_iHighestPopulation; }											// Exposed to Python
	void setHighestPopulation(int iNewValue);
	int getWorkingPopulation() const { return m_iWorkingPopulation; }											// Exposed to Python
	void changeWorkingPopulation(int iChange);
	int getSpecialistPopulation() const { return m_iSpecialistPopulation; }										// Exposed to Python
	void changeSpecialistPopulation(int iChange);

	int getNumGreatPeople() const { return m_iNumGreatPeople; }													// Exposed to Python
	void changeNumGreatPeople(int iChange);
	int getBaseGreatPeopleRate() const { return m_iBaseGreatPeopleRate; }										// Exposed to Python
	int getGreatPeopleRate() const;																				// Exposed to Python
	int getTotalGreatPeopleRateModifier() const;																// Exposed to Python
	void changeBaseGreatPeopleRate(int iChange);																// Exposed to Python
	int getGreatPeopleRateModifier() const { return m_iGreatPeopleRateModifier; }								// Exposed to Python
	void changeGreatPeopleRateModifier(int iChange);
	// BUG - Building Additional Great People - start
	int getAdditionalGreatPeopleRateByBuilding(BuildingTypes eBuilding) const;
	int getAdditionalBaseGreatPeopleRateByBuilding(BuildingTypes eBuilding) const;
	int getAdditionalGreatPeopleRateModifierByBuilding(BuildingTypes eBuilding) const;
	// BUG - Building Additional Great People - end
	// BUG - Specialist Additional Great People - start
	int getAdditionalGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange = 1) const;
	int getAdditionalBaseGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange = 1) const;
	// BUG - Specialist Additional Great People - end
	int getGreatPeopleProgress() const { return m_iGreatPeopleProgress; }										// Exposed to Python
	void changeGreatPeopleProgress(int iChange);																// Exposed to Python

	int getNumWorldWonders() const { return m_iNumWorldWonders; }												// Exposed to Python
	void changeNumWorldWonders(int iChange);
	int getNumTeamWonders() const { return m_iNumTeamWonders; }													// Exposed to Python
	void changeNumTeamWonders(int iChange);
	int getNumNationalWonders() const { return m_iNumNationalWonders; }											// Exposed to Python
	void changeNumNationalWonders(int iChange);
	int getNumBuildings() const { return m_iNumBuildings; }														// Exposed to Python
	void changeNumBuildings(int iChange);

	int getGovernmentCenterCount() const { return m_iGovernmentCenterCount; }			
	bool isGovernmentCenter() const { return (getGovernmentCenterCount() > 0); }								// Exposed to Python
	void changeGovernmentCenterCount(int iChange);
	// BUG - Building Saved Maintenance:
	int getSavedMaintenanceTimes100ByBuilding(BuildingTypes eBuilding) const;
	int getMaintenance() const { return m_iMaintenance / 100; }													// Exposed to Python
	int getMaintenanceTimes100() const { return m_iMaintenance; }												// Exposed to Python
	void updateMaintenance();
	int calculateDistanceMaintenance() const;																	// Exposed to Python
	int calculateNumCitiesMaintenance() const;																	// Exposed to Python
	int calculateColonyMaintenance() const;																		// Exposed to Python
	int calculateCorporationMaintenance() const;																// Exposed to Python
	/* <advc.104> Added an optional parameter to allow the computation of
	   projected maintenance for cities yet to be conquered. */
	int calculateDistanceMaintenanceTimes100(PlayerTypes eOwner = NO_PLAYER) const;								// Exposed to Python
	int calculateColonyMaintenanceTimes100(PlayerTypes eOwner = NO_PLAYER) const;
	int calculateNumCitiesMaintenanceTimes100(PlayerTypes eOwner = NO_PLAYER) const;							// Exposed to Python									// Exposed to Python
	// </advc.104>
	// <advc.004b> A projection for cities yet to be founded
	static int calculateDistanceMaintenanceTimes100(CvPlot const& kCityPlot,
			PlayerTypes eOwner, int iPopulation = -1);
	static int calculateNumCitiesMaintenanceTimes100(CvPlot const& kCityPlot,
			PlayerTypes eOwner, int iPopulation = -1, int iExtraCities = 0);
	static int calculateColonyMaintenanceTimes100(CvPlot const& kCityPlot,
			PlayerTypes eOwner, int iPopulation = -1, int iExtraCities = 0);
	static int initialPopulation();
	// </advc.004b>
	int calculateCorporationMaintenanceTimes100(CorporationTypes eCorporation) const;							// Exposed to Python
	int calculateCorporationMaintenanceTimes100() const;														// Exposed to Python
	int calculateBaseMaintenanceTimes100() const;
	int getMaintenanceModifier() const { return m_iMaintenanceModifier; }										// Exposed to Python
	void changeMaintenanceModifier(int iChange);

	int getWarWearinessModifier() const { return m_iWarWearinessModifier; }										// Exposed to Python
	void changeWarWearinessModifier(int iChange);
	int getHurryAngerModifier() const { return m_iHurryAngerModifier; }											// Exposed to Python
	void changeHurryAngerModifier(int iChange);

	int getHealRate() const { return m_iHealRate; }																// Exposed to Python
	void changeHealRate(int iChange);

	int getEspionageHealthCounter() const { return m_iEspionageHealthCounter; }									// Exposed to Python
	void changeEspionageHealthCounter(int iChange);																// Exposed to Python
	int getEspionageHappinessCounter() const { return m_iEspionageHappinessCounter; }							// Exposed to Python
	void changeEspionageHappinessCounter(int iChange);															// Exposed to Python

	int getFreshWaterGoodHealth() const { return m_iFreshWaterGoodHealth; }										// Exposed to Python
	int getFreshWaterBadHealth() const { return m_iFreshWaterBadHealth; }										// Exposed to Python
	void updateFreshWaterHealth();
	// advc.901: Renamed everything with "featureHealth/Happiness" in its name
	int getSurroundingGoodHealth() const { return m_iSurroundingGoodHealth; }									// Exposed to Python
	int getSurroundingBadHealth() const { return m_iSurroundingBadHealth; }										// Exposed to Python
	void updateSurroundingHealthHappiness();
	// <advc.901>
	void calculateHealthHappyChange(CvPlot const& kPlot, ImprovementTypes eNewImprov,
			ImprovementTypes eOldImprov, bool bRemoveFeature, int& iHappyChange,
			int& iHealthChange, int& iHealthPercentChange) const;
	void goodBadHealthHappyChange(CvPlot const& kPlot, ImprovementTypes eNewImprov,
			ImprovementTypes eOldImprov, bool bRemoveFeature, int& iHappyChange,
			int& iUnhappyChange, int& iGoodHealthChange, int& iBadHealthChange,
			int& iGoodHealthPercentChange, int& iBadHealthPercentChange) const;
	// </advc.901>
/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: Enable Terrain Health Modifiers                                                  **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
	int getTerrainGoodHealth() const;																					// Exposed to Python
	int getTerrainBadHealth() const;																					// Exposed to Python
	void updateTerrainHealth();
/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**                                                                                              */
/*************************************************************************************************/
	int getSpecialistGoodHealth() const;																			// Exposed to Python
	int getSpecialistBadHealth() const;														// Exposed to Python
	int getSpecialistHappiness() const;																			// Exposed to Python
	int getSpecialistUnhappiness() const;														// Exposed to Python
	void changeSpecialistGoodHealth(int iChange);
	void changeSpecialistBadHealth(int iChange);
	void changeSpecialistHappiness(int iChange);
	void changeSpecialistUnhappiness(int iChange);
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/
	// BUG - Actual Effects - start
	int getAdditionalAngryPopuplation(int iGood, int iBad) const;
	int getAdditionalSpoiledFood(int iGood, int iBad) const;
	int getAdditionalStarvation(int iSpoiledFood) const;
	// BUG - Actual Effects - end
	// <advc.001c>
	int GPTurnsLeft() const;
	void GPProjection(std::vector<std::pair<UnitTypes,int> >& r) const; // (exposed to Python)
	// </advc.001c>
	int getBuildingGoodHealth() const { return m_iBuildingGoodHealth; }											// Exposed to Python
	int getBuildingBadHealth() const { return m_iBuildingBadHealth; }											// Exposed to Python
	int getBuildingHealth(BuildingTypes eBuilding) const;														// Exposed to Python
	int getBuildingGoodHealth(BuildingTypes eBuilding) const;
	int getBuildingBadHealth(BuildingTypes eBuilding) const;
	void changeBuildingGoodHealth(int iChange);
	void changeBuildingBadHealth(int iChange);

	int getPowerGoodHealth() const { return m_iPowerGoodHealth; }												// Exposed to Python
	int getPowerBadHealth() const { return m_iPowerBadHealth; }													// Exposed to Python
	void updatePowerHealth();

	int getBonusGoodHealth() const { return m_iBonusGoodHealth; }												// Exposed to Python
	int getBonusBadHealth() const { return m_iBonusBadHealth; }													// Exposed to Python
	void changeBonusGoodHealth(int iChange);
	void changeBonusBadHealth(int iChange);
    // < Civic Infos Plus Start >
    int getReligionGoodHealth() const;																	// Exposed to Python
	int getReligionBadHealth() const;																	// Exposed to Python
	int getReligionHealth(ReligionTypes eReligion) const;							// Exposed to Python
	void updateReligionHealth();
	// < Civic Infos Plus End   >

	int getMilitaryHappiness() const;																			// Exposed to Python
	int getMilitaryHappinessUnits() const { return m_iMilitaryHappinessUnits; }									// Exposed to Python
	void changeMilitaryHappinessUnits(int iChange);

	int getBuildingGoodHappiness() const { return m_iBuildingGoodHappiness; }									// Exposed to Python
	int getBuildingBadHappiness() const { return m_iBuildingBadHappiness; }										// Exposed to Python
	int getBuildingHappiness(BuildingTypes eBuilding) const;							// Exposed to Python
	void changeBuildingGoodHappiness(int iChange);
	void changeBuildingBadHappiness(int iChange);

	int getExtraBuildingGoodHappiness() const { return m_iExtraBuildingGoodHappiness; }							// Exposed to Python
	int getExtraBuildingBadHappiness() const { return m_iExtraBuildingBadHappiness; }							// Exposed to Python
	void updateExtraBuildingHappiness();
	// BETTER_BTS_AI_MOD, from BUG, 02/24/10, EmperorFool:
	int getAdditionalHappinessByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad) const;

	int getExtraBuildingGoodHealth() const { return m_iExtraBuildingGoodHealth; }								// Exposed to Python
	int getExtraBuildingBadHealth() const { return m_iExtraBuildingBadHealth; }									// Exposed to Python
	void updateExtraBuildingHealth();

	// BETTER_BTS_AI_MOD, from BUG, 02/24/10, EmperorFool:
	int getAdditionalHealthByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad,
			bool bAssumeStrategicBonuses = false) const; // advc.001h

	int getSurroundingGoodHappiness() const { return m_iSurroundingGoodHappiness; }								// Exposed to Python
	int getSurroundingBadHappiness() const { return m_iSurroundingBadHappiness; }								// Exposed to Python
	//void updateFeatureHappiness(); // advc.901: Replaced by updateSurroundingHealthHappiness

	int getBonusGoodHappiness(/* advc.912c: */ bool bIgnoreModifier = false) const;								// Exposed to Python
	int getBonusBadHappiness() const { return m_iBonusBadHappiness; }											// Exposed to Python
	void changeBonusGoodHappiness(int iChange);
	void changeBonusBadHappiness(int iChange);

	int getReligionGoodHappiness() const { return m_iReligionGoodHappiness; }									// Exposed to Python
	int getReligionBadHappiness() const { return m_iReligionBadHappiness; }										// Exposed to Python
	int getReligionHappiness(ReligionTypes eReligion) const;													// Exposed to Python
	void updateReligionHappiness();

	int getExtraHappiness() const { return m_iExtraHappiness; }													// Exposed to Python
	void changeExtraHappiness(int iChange);																		// Exposed to Python
	int getExtraHealth() const { return m_iExtraHealth; }														// Exposed to Python
	void changeExtraHealth(int iChange);																		// Exposed to Python

	int getHurryAngerTimer() const { return m_iHurryAngerTimer; }												// Exposed to Python
	void changeHurryAngerTimer(int iChange);																	// Exposed to Python
	int getConscriptAngerTimer() const { return m_iConscriptAngerTimer; }										// Exposed to Python
	void changeConscriptAngerTimer(int iChange);																// Exposed to Python
	int getDefyResolutionAngerTimer() const { return m_iDefyResolutionAngerTimer; }								// Exposed to Python
	void changeDefyResolutionAngerTimer(int iChange);															// Exposed to Python
	int flatDefyResolutionAngerLength() const;																	// Exposed to Python
	int getHappinessTimer() const { return m_iHappinessTimer; }													// Exposed to Python
	void changeHappinessTimer(int iChange);																		// Exposed to Python
	int getNoUnhappinessCount() const { return m_iNoUnhappinessCount; }			
	bool isNoUnhappiness() const { return (getNoUnhappinessCount() > 0); }										// Exposed to Python
	void changeNoUnhappinessCount(int iChange);
	/*int getNoUnhealthyPopulationCount() const;
	bool isNoUnhealthyPopulation() const;																		// Exposed to Python
	void changeNoUnhealthyPopulationCount(int iChange);*/ // BtS
	/*  K-Mod, 27/dec/10, karadoc
		replace NoUnhealthyPopulation with UnhealthyPopulationModifier */
	int getUnhealthyPopulationModifier() const; // Exposed to Python
	void changeUnhealthyPopulationModifier(int iChange);
	// K-Mod end

	int getBuildingOnlyHealthyCount() const { return m_iBuildingOnlyHealthyCount; }			
	bool isBuildingOnlyHealthy() const;																				// Exposed to Python
	void changeBuildingOnlyHealthyCount(int iChange);

	int getFood() const { return m_iFood; }																			// Exposed to Python
	void setFood(int iNewValue);																					// Exposed to Python
	void changeFood(int iChange);																					// Exposed to Python
	int getFoodKept() const { return m_iFoodKept; }																	// Exposed to Python
	void setFoodKept(int iNewValue);
	void changeFoodKept(int iChange);
	int getMaxFoodKeptPercent() const { return m_iMaxFoodKeptPercent; }												// Exposed to Python
	void changeMaxFoodKeptPercent(int iChange);

	int getOverflowProduction() const { return m_iOverflowProduction; }												// Exposed to Python
	void setOverflowProduction(int iNewValue);																		// Exposed to Python
	void changeOverflowProduction(int iChange, int iProductionModifier);
	// advc.064b:
	int unmodifyOverflow(int iRawOverflow, int iProductionModifier) const;

	int getFeatureProduction() const { return m_iFeatureProduction; }												// Exposed to Python
	void setFeatureProduction(int iNewValue);																		// Exposed to Python
	void changeFeatureProduction(int iChange);

	int getMilitaryProductionModifier() const { return m_iMilitaryProductionModifier; }								// Exposed to Python
	void changeMilitaryProductionModifier(int iChange);
	int getSpaceProductionModifier() const { return m_iSpaceProductionModifier; }									// Exposed to Python
	void changeSpaceProductionModifier(int iChange);

	int getExtraTradeRoutes() const { return m_iExtraTradeRoutes; }													// Exposed to Python
	void changeExtraTradeRoutes(int iChange);																		// Exposed to Python
	int getTradeRouteModifier() const { return m_iTradeRouteModifier; }												// Exposed to Python
	void changeTradeRouteModifier(int iChange);
	int getForeignTradeRouteModifier() const { return m_iForeignTradeRouteModifier; }								// Exposed to Python
	void changeForeignTradeRouteModifier(int iChange);

	// K-Mod, 26/sep/10 (Trade culture calculation)
	int getTradeCultureRateTimes100() const;																		// Exposed to Python

	int getBuildingDefense() const { return m_iBuildingDefense; }													// Exposed to Python
	void changeBuildingDefense(int iChange);
	// BUG - Building Additional Defense:
	int getAdditionalDefenseByBuilding(BuildingTypes eBuilding) const;

	int getBuildingBombardDefense() const { return m_iBuildingBombardDefense; }										// Exposed to Python
	void changeBuildingBombardDefense(int iChange);

	int getFreeExperience() const { return m_iFreeExperience; }														// Exposed to Python
	void changeFreeExperience(int iChange);

	int getCurrAirlift() const { return m_iCurrAirlift; }															// Exposed to Python
	void setCurrAirlift(int iNewValue);
	void changeCurrAirlift(int iChange);
	int getMaxAirlift() const { return m_iMaxAirlift; }																// Exposed to Python
	void changeMaxAirlift(int iChange);

	int getAirModifier() const { return m_iAirModifier; }															// Exposed to Python
	void changeAirModifier(int iChange);

	int getAirUnitCapacity(TeamTypes eTeam) const;																	// Exposed to Python
	void changeAirUnitCapacity(int iChange);																		// Exposed to Python

	int getNukeModifier() const { return m_iNukeModifier; }															// Exposed to Python
	void changeNukeModifier(int iChange);

	int getFreeSpecialist() const { return m_iFreeSpecialist; }														// Exposed to Python
	void changeFreeSpecialist(int iChange);

	int getPowerCount() const { return m_iPowerCount; }			
	bool isPower() const { return (getPowerCount() > 0 || isAreaCleanPower()); }									// Exposed to Python
	bool isAreaCleanPower() const;																					// Exposed to Python
	int getDirtyPowerCount() const { return m_iDirtyPowerCount; }			
	bool isDirtyPower() const;																						// Exposed to Python
	void changePowerCount(int iChange, bool bDirty);

	bool isAreaBorderObstacle() const;																				// Exposed to Python

	int getDefenseDamage() const { return m_iDefenseDamage; }														// Exposed to Python
	void changeDefenseDamage(int iChange);																			// Exposed to Python
	void changeDefenseModifier(int iChange);																		// Exposed to Python
	int getLastDefenseDamage() const;																				// Exposed to Python
	void setLastDefenseDamage(int iNewValue);

	bool isBombardable(const CvUnit* pUnit) const;																	// Exposed to Python
	int getNaturalDefense() const;																					// Exposed to Python
	int getTotalDefense(bool bIgnoreBuilding) const;																// Exposed to Python
	int getDefenseModifier(bool bIgnoreBuilding) const;																// Exposed to Python

	int getOccupationTimer() const { return m_iOccupationTimer; }													// Exposed to Python
	bool isOccupation() const { return (getOccupationTimer() > 0); }												// Exposed to Python
	void setOccupationTimer(int iNewValue);																			// Exposed to Python
	void changeOccupationTimer(int iChange);																		// Exposed to Python

	int getCultureUpdateTimer() const { return m_iCultureUpdateTimer; }												// Exposed to Python
	void setCultureUpdateTimer(int iNewValue);
	void changeCultureUpdateTimer(int iChange);																		// Exposed to Python

	int getCitySizeBoost() const;
	void setCitySizeBoost(int iBoost);

	bool isNeverLost() const;																						// Exposed to Python
	void setNeverLost(bool bNewValue);																				// Exposed to Python

	bool isBombarded() const { return m_bBombarded; }																// Exposed to Python
	void setBombarded(bool bNewValue);																				// Exposed to Python

	bool isDrafted() const { return m_bDrafted; }																	// Exposed to Python
	void setDrafted(bool bNewValue);																				// Exposed to Python

	bool isAirliftTargeted() const { return m_bAirliftTargeted; }													// Exposed to Python
	void setAirliftTargeted(bool bNewValue);																		// Exposed to Python

	bool isPlundered() const { return m_bPlundered; }																// Exposed to Python
	void setPlundered(bool bNewValue);																				// Exposed to Python

	bool isWeLoveTheKingDay() const;																				// Exposed to Python
	void setWeLoveTheKingDay(bool bNewValue);

	bool isCitizensAutomated() const { return m_bCitizensAutomated; }												// Exposed to Python
	void setCitizensAutomated(bool bNewValue);																		// Exposed to Python
	bool isProductionAutomated() const { return m_bProductionAutomated; }											// Exposed to Python
	void setProductionAutomated(bool bNewValue, bool bClear);														// Exposed to Python

	// allows you to programmatically specify a cities walls rather than having them be generated automagically
	DllExport bool isWallOverride() const;
	void setWallOverride(bool bOverride);

	DllExport bool isInfoDirty() const;
	DllExport void setInfoDirty(bool bNewValue);
	DllExport bool isLayoutDirty() const;
	DllExport void setLayoutDirty(bool bNewValue);

	PlayerTypes getOwnerExternal() const; // advc.inl: Exported through .def file									// Exposed to Python
	inline PlayerTypes getOwner() const { return m_eOwner; } // advc.inl: Renamed from getOwnerINLINE
	DllExport TeamTypes getTeam() const;																			// Exposed to Python
	PlayerTypes getPreviousOwner() const { return m_ePreviousOwner; }												// Exposed to Python
	void setPreviousOwner(PlayerTypes eNewValue);
	PlayerTypes getOriginalOwner() const { return m_eOriginalOwner; }												// Exposed to Python
	void setOriginalOwner(PlayerTypes eNewValue);

	CultureLevelTypes getCultureLevel() const { return m_eCultureLevel; }											// Exposed to Python
	int getCultureThreshold() const { return getCultureThreshold(getCultureLevel()); }								// Exposed to Python
	static int getCultureThreshold(CultureLevelTypes eLevel);
	void setCultureLevel(CultureLevelTypes eNewValue, bool bUpdatePlotGroups);
	void updateCultureLevel(bool bUpdatePlotGroups);
	CultureLevelTypes calculateCultureLevel(PlayerTypes ePlayer) const; // advc.130f
	int getNumPartisanUnits(PlayerTypes ePartisanPlayer) const; // advc.003y
	int getCultureTurnsLeft() const; // advc.042

	int getSeaPlotYield(YieldTypes eIndex) const { return m_aiSeaPlotYield.get(eIndex); }							// Exposed to Python
	void changeSeaPlotYield(YieldTypes eIndex, int iChange);
	int getRiverPlotYield(YieldTypes eIndex) const { return m_aiRiverPlotYield.get(eIndex); }						// Exposed to Python
	void changeRiverPlotYield(YieldTypes eIndex, int iChange);

	// BUG - Building Additional Yield - start
	int getAdditionalYieldByBuilding(YieldTypes eYield, BuildingTypes eBuilding) const;
	int getAdditionalBaseYieldRateByBuilding(YieldTypes eYield, BuildingTypes eBuilding) const;
	int getAdditionalYieldRateModifierByBuilding(YieldTypes eYield, BuildingTypes eBuilding) const;
	// BUG - Building Additional Yield - end
	// BUG - Specialist Additional Yield - start
	int getAdditionalYieldBySpecialist(YieldTypes eYield, SpecialistTypes eSpecialist, int iChange = 1) const;
	int getAdditionalBaseYieldRateBySpecialist(YieldTypes eYield, SpecialistTypes eSpecialist, int iChange = 1) const;
	// BUG - Specialist Additional Yield - end

	int getBaseYieldRate(YieldTypes eIndex) const { return m_aiBaseYieldRate.get(eIndex); }							// Exposed to Python
	int getBaseYieldRateModifier(YieldTypes eIndex, int iExtra = 0) const;											// Exposed to Python
	int getYieldRate(YieldTypes eIndex) const																		// Exposed to Python
	{
		return (getBaseYieldRate(eIndex) * getBaseYieldRateModifier(eIndex)) / 100;
	}
	void setBaseYieldRate(YieldTypes eIndex, int iNewValue);														// Exposed to Python
	void changeBaseYieldRate(YieldTypes eIndex, int iChange);														// Exposed to Python
	int calculateBaseYieldRate(YieldTypes eIndex); // advc.104u
	int getYieldRateModifier(YieldTypes eIndex) const { return m_aiYieldRateModifier.get(eIndex); }					// Exposed to Python
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);

	int getPowerYieldRateModifier(YieldTypes eIndex) const															// Exposed to Python
	{
		return m_aiPowerYieldRateModifier.get(eIndex);
	}
	void changePowerYieldRateModifier(YieldTypes eIndex, int iChange);
	int getBonusYieldRateModifier(YieldTypes eIndex) const															// Exposed to Python
	{
		return m_aiBonusYieldRateModifier.get(eIndex);
	}
	void changeBonusYieldRateModifier(YieldTypes eIndex, int iChange);
	// < Civic Infos Plus Start >
//removed by f1 advc - keldath
	int getStateReligionYieldRateModifier(YieldTypes eIndex) const;											// Exposed to Python
	void changeStateReligionYieldRateModifier(YieldTypes eIndex, int iChange);

	int getStateReligionCommerceRateModifier(CommerceTypes eIndex) const;
	void changeStateReligionCommerceRateModifier(CommerceTypes eIndex, int iChange);

	int getNonStateReligionYieldRateModifier(YieldTypes eIndex) const;											// Exposed to Python
	void changeNonStateReligionYieldRateModifier(YieldTypes eIndex, int iChange);

	int getNonStateReligionCommerceRateModifier(CommerceTypes eIndex) const;
	void changeNonStateReligionCommerceRateModifier(CommerceTypes eIndex, int iChange);

	void updateBuildingYieldChange(CivicTypes eCivic, int iChange);
	void updateBuildingCommerceChange(CivicTypes eCivic, int iChange);
	// < Civic Infos Plus End   >

	int getTradeYield(YieldTypes eIndex) const { return m_aiTradeYield.get(eIndex); }								// Exposed to Python
	int totalTradeModifier(CvCity const* pOtherCity = NULL) const;													// Exposed to Python
	int getPopulationTradeModifier() const;
	int getPeaceTradeModifier(TeamTypes eTeam) const;
	int getBaseTradeProfit(CvCity const* pCity) const;
	int calculateTradeProfit(CvCity const* pCity) const																// Exposed to Python
	{
		return calculateTradeProfitTimes100(pCity) / 100;
	}
	int calculateTradeProfitTimes100(CvCity const* pCity) const; // advc.004
	int calculateTradeYield(YieldTypes eIndex, int iTradeProfit) const;												// Exposed to Python
	// BULL - Trade Hover - start
	void calculateTradeTotals(YieldTypes eIndex, int& iDomesticYield, int& iDomesticRoutes,
			int& iForeignYield, int& iForeignRoutes, PlayerTypes eWithPlayer = NO_PLAYER) const;
	// BULL - Trade Hover - end
	void setTradeYield(YieldTypes eIndex, int iNewValue);

	int getExtraSpecialistYield(YieldTypes eIndex) const															// Exposed to Python
	{
		return m_aiExtraSpecialistYield.get(eIndex);
	}
	int getExtraSpecialistYield(YieldTypes eIndex, SpecialistTypes eSpecialist) const;								// Exposed to Python
	void updateExtraSpecialistYield(YieldTypes eYield);
	void updateExtraSpecialistYield();

/*************************************************************************************************/
/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
/**																								**/
/**																								**/
/*************************************************************************************************/
	int getSpecialistCivicExtraCommerce(CommerceTypes eIndex) const;
	int getSpecialistCivicExtraCommerceBySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist) const;					// Exposed to Python
	void updateSpecialistCivicExtraCommerce(CommerceTypes eCommerce);
	void updateSpecialistCivicExtraCommerce();
/*************************************************************************************************/
/**	CMEDIT: End																					**/
/*************************************************************************************************/
	int getCommerceRate(CommerceTypes eIndex) const;																// Exposed to Python
	int getCommerceRateTimes100(CommerceTypes eIndex) const;														// Exposed to Python
	int getCommerceFromPercent(CommerceTypes eIndex, int iYieldRate) const;											// Exposed to Python
	int getBaseCommerceRate(CommerceTypes eIndex) const																// Exposed to Python
	{
		return (getBaseCommerceRateTimes100(eIndex) / 100);
	}
	int getBaseCommerceRateTimes100(CommerceTypes eIndex) const;													// Exposed to Python
	int getTotalCommerceRateModifier(CommerceTypes eIndex) const;													// Exposed to Python
	void updateCommerce(CommerceTypes eIndex);
	void updateCommerce();

	int getProductionToCommerceModifier(CommerceTypes eIndex) const													// Exposed to Python
	{
		return m_aiProductionToCommerceModifier.get(eIndex);
	}
	void changeProductionToCommerceModifier(CommerceTypes eIndex, int iChange);

	int getBuildingCommerce(CommerceTypes eIndex) const { return m_aiBuildingCommerce.get(eIndex); }				// Exposed to Python
	int getBuildingCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const;							// Exposed to Python
	void updateBuildingCommerce();
	// BUG - Building Additional Commerce - start
	int getAdditionalCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const
	{
		return getAdditionalCommerceTimes100ByBuilding(eIndex, eBuilding) / 100;
	}
	int getAdditionalCommerceTimes100ByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	int getAdditionalBaseCommerceRateByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	int getAdditionalBaseCommerceRateByBuildingImpl(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	int getAdditionalCommerceRateModifierByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	int getAdditionalCommerceRateModifierByBuildingImpl(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	// BUG - Building Additional Commerce - end
	int getSpecialistCommerce(CommerceTypes eIndex) const															// Exposed to Python
	{
		return m_aiSpecialistCommerce.get(eIndex);
	}
	void changeSpecialistCommerce(CommerceTypes eIndex, int iChange);												// Exposed to Python
	// BUG - Specialist Additional Commerce - start
	int getAdditionalCommerceBySpecialist(CommerceTypes eIndex,														// Exposed to Python
		SpecialistTypes eSpecialist, int iChange = 1) const
	{
		return getAdditionalCommerceTimes100BySpecialist(eIndex, eSpecialist, iChange) / 100;
	}
	int getAdditionalCommerceTimes100BySpecialist(CommerceTypes eIndex,												// Exposed to Python
			SpecialistTypes eSpecialist, int iChange = 1) const;
	int getAdditionalBaseCommerceRateBySpecialist(CommerceTypes eIndex,												// Exposed to Python
			SpecialistTypes eSpecialist, int iChange = 1) const;
	int getAdditionalBaseCommerceRateBySpecialistImpl(CommerceTypes eIndex,
			SpecialistTypes eSpecialist, int iChange = 1) const;
	// BUG - Specialist Additional Commerce - end

	int getReligionCommerce(CommerceTypes eIndex) const																// Exposed to Python
	{
		return m_aiReligionCommerce.get(eIndex);
	}
	int getReligionCommerceByReligion(CommerceTypes eIndex, ReligionTypes eReligion) const;							// Exposed to Python
	void updateReligionCommerce(CommerceTypes eIndex);
	void updateReligionCommerce();

	void setCorporationYield(YieldTypes eIndex, int iNewValue);
	int getCorporationCommerce(CommerceTypes eIndex) const															// Exposed to Python
	{
		return m_aiCorporationCommerce.get(eIndex);
	}
	int getCorporationCommerceByCorporation(CommerceTypes eIndex, CorporationTypes eCorporation) const;				// Exposed to Python
	int getCorporationYield(YieldTypes eIndex) const { return m_aiCorporationYield.get(eIndex); }					// Exposed to Python
	int getCorporationYieldByCorporation(YieldTypes eIndex, CorporationTypes eCorporation) const;					// Exposed to Python
	void updateCorporation();
	void updateCorporationCommerce(CommerceTypes eIndex);
	void updateCorporationYield(YieldTypes eIndex);
	void updateCorporationBonus();

	int getCommerceRateModifier(CommerceTypes eIndex) const															// Exposed to Python
	{
		return m_aiCommerceRateModifier.get(eIndex);
	}
	void changeCommerceRateModifier(CommerceTypes eIndex, int iChange);

	int getCommerceHappinessPer(CommerceTypes eIndex) const															// Exposed to Python
	{
		return m_aiCommerceHappinessPer.get(eIndex);
	}
	int getCommerceHappinessByType(CommerceTypes eIndex) const;														// Exposed to Python
	int getCommerceHappiness() const;																				// Exposed to Python
	void changeCommerceHappinessPer(CommerceTypes eIndex, int iChange);

	int getDomainFreeExperience(DomainTypes eIndex) const															// Exposed to Python
	{
		return m_aiDomainFreeExperience.get(eIndex);
	}
	void changeDomainFreeExperience(DomainTypes eIndex, int iChange);

	int getDomainProductionModifier(DomainTypes eIndex) const														// Exposed to Python
	{
		return m_aiDomainProductionModifier.get(eIndex);
	}
	void changeDomainProductionModifier(DomainTypes eIndex, int iChange);

	int getCulture(PlayerTypes eIndex) const																		// Exposed to Python
	{	// advc: Delegate to the Times100 function
		return getCultureTimes100(eIndex) / 100;
	}
	inline int getCultureTimes100(PlayerTypes eIndex) const															// Exposed to Python
	{
		return m_aiCulture.get(eIndex);
	}
	int countTotalCultureTimes100() const;																			// Exposed to Python
	PlayerTypes findHighestCulture() const;																			// Exposed to Python
	// advc.101:  (advc.ctr: exposed to Python)
	double revoltProbability( // <advc.023>
			bool bIgnoreWar = false, bool biIgnoreGarrison = false,
			bool bIgnoreOccupation = false) const;
	double probabilityOccupationDecrement() const; // </advc.023>
	// K-Mod: (advc.ctr: exposed to Python)
	bool canCultureFlip(PlayerTypes eToPlayer /* <advc.101> */ = NO_PLAYER,
			bool bCheckPriorRevolts = true) const; // </advc.101>
	int calculateCulturePercent(PlayerTypes eIndex) const;															// Exposed to Python
	int calculateTeamCulturePercent(TeamTypes eIndex) const;														// Exposed to Python
	void setCulture(PlayerTypes eIndex, int iNewValue, bool bPlots, bool bUpdatePlotGroups);						// Exposed to Python
	void setCultureTimes100(PlayerTypes eIndex, int iNewValue, bool bPlots, bool bUpdatePlotGroups);				// Exposed to Python
	void changeCulture(PlayerTypes eIndex, int iChange, bool bPlots, bool bUpdatePlotGroups);						// Exposed to Python
	void changeCultureTimes100(PlayerTypes eIndex, int iChange, bool bPlots, bool bUpdatePlotGroups);				// Exposed to Python

	int getNumRevolts(PlayerTypes eIndex) const
	{
		return m_aiNumRevolts.get(eIndex);
	}
	int getNumRevolts() const; // advc.099c
	void changeNumRevolts(PlayerTypes eIndex, int iChange);
	double getRevoltTestProbability() const; // advc.101: Now between 0 and 1
	int getRevoltProtection() const; // advc.101
	void addRevoltFreeUnits(); // advc

	bool isTradeRoute(PlayerTypes eIndex) const																		// Exposed to Python
	{
		return m_abTradeRoute.get(eIndex);
	}
	void setTradeRoute(PlayerTypes eIndex, bool bNewValue);

	bool isEverOwned(PlayerTypes eIndex) const																		// Exposed to Python
	{
		return m_abEverOwned.get(eIndex);
	}
	void setEverOwned(PlayerTypes eIndex, bool bNewValue);

	DllExport bool isRevealed(TeamTypes eIndex, bool bDebug) const;													// Exposed to Python
	// <advc.inl> Faster implementation for non-UI code
	inline bool isRevealed(TeamTypes eToTeam) const
	{
		return m_abRevealed.get(eToTeam);
	} // </advc.inl>
	void setRevealed(TeamTypes eIndex, bool bNewValue);																// Exposed to Python

	bool getEspionageVisibility(TeamTypes eTeam) const																// Exposed to Python
	{
		return m_abEspionageVisibility.get(eTeam);
	}  // <advc.opt>
	bool isAnyEspionageVisibility() const
	{
		return m_abEspionageVisibility.hasContent();
	} // </advc.opt>
	void setEspionageVisibility(TeamTypes eTeam, bool bVisible, bool bUpdatePlotGroups);
	void updateEspionageVisibility(bool bUpdatePlotGroups);

	DllExport const CvWString getName(uint uiForm = 0) const;														// Exposed to Python
	DllExport const wchar* getNameKey() const;																		// Exposed to Python
	void setName(const wchar* szNewValue, bool bFound = false,														// Exposed to Python
			bool bInitial = false); // advc.106k
	void doFoundMessage();

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const;																				// Exposed to Python
	void setScriptData(std::string szNewValue);																		// Exposed to Python

	int getFreeBonus(BonusTypes eIndex) const																		// Exposed to Python
	{
		return m_aiFreeBonus.get(eIndex);
	}  // <advc.opt>
	bool isAnyFreeBonus() const
	{
		return m_aiFreeBonus.hasContent();
	} // </advc.opt>
	void changeFreeBonus(BonusTypes eIndex, int iChange);															// Exposed to Python
	int getNumBonuses(BonusTypes eIndex) const;																		// Exposed to Python
	bool hasBonus(BonusTypes eIndex) const																			// Exposed to Python
	{
		return (getNumBonuses(eIndex) > 0);
	}
	//< Building Resource Converter Start >
	//f1rpo 096 - added a var here to pass an param to avoid a loop - keldath
	void changeNumBonuses(BonusTypes eBonus, int iChange,
       bool bUpdateBuildings = true);
	//< Building Resource Converter End   >
//	void changeNumBonuses(BonusTypes eIndex, int iChange);
	int countUniqueBonuses() const; // advc.149
	int getNumCorpProducedBonuses(BonusTypes eIndex) const
	{
		return m_aiNumCorpProducedBonuses.get(eIndex);
	}
	bool isCorporationBonus(BonusTypes eBonus) const;
	bool isActiveCorporation(CorporationTypes eCorporation) const;
	//< Building Resource Converter Start >
	bool isBuildingBonus(BonusTypes eBonus) const; // f1rpo
	bool isBuildingBonus(BuildingTypes eBuilding) const; // f1rpo
	//< Building Resource Converter end >
	// < Building Resource Converter Start >
	void processBuildingBonuses();
	int getBuildingOutputBonus(BonusTypes eIndex) const;														// Exposed to Python
	protected: void resetBuildingOutputBonuses(); public:
	// < Building Resource Converter End   >
	int getBuildingProduction(BuildingTypes eIndex) const															// Exposed to Python
	{
		return m_aiBuildingProduction.get(eIndex);
	}
	void setBuildingProduction(BuildingTypes eIndex, int iNewValue);												// Exposed to Python
	void changeBuildingProduction(BuildingTypes eIndex, int iChange);												// Exposed to Python

	int getBuildingProductionTime(BuildingTypes eIndex) const														// Exposed to Python
	{
		return m_aiBuildingProductionTime.get(eIndex);
	}
	void setBuildingProductionTime(BuildingTypes eIndex, int iNewValue);											// Exposed to Python
	void changeBuildingProductionTime(BuildingTypes eIndex, int iChange);											// Exposed to Python

	int getProjectProduction(ProjectTypes eIndex) const																// Exposed to Python
	{
		return m_aiProjectProduction.get(eIndex);
	}
	void setProjectProduction(ProjectTypes eIndex, int iNewValue);													// Exposed to Python
	void changeProjectProduction(ProjectTypes eIndex, int iChange);													// Exposed to Python

	int getBuildingOriginalOwner(BuildingTypes eIndex) const														// Exposed to Python
	{
		return m_aiBuildingOriginalOwner.get(eIndex);
	}
	int getBuildingOriginalTime(BuildingTypes eIndex) const															// Exposed to Python
	{
		return m_aiBuildingOriginalTime.get(eIndex);
	}

	int getUnitProduction(UnitTypes eIndex) const																	// Exposed to Python
	{
		return m_aiUnitProduction.get(eIndex);
	}
	void setUnitProduction(UnitTypes eIndex, int iNewValue);														// Exposed to Python
	void changeUnitProduction(UnitTypes eIndex, int iChange);														// Exposed to Python

	int getUnitProductionTime(UnitTypes eIndex) const																// Exposed to Python
	{
		return m_aiUnitProductionTime.get(eIndex);
	}
	void setUnitProductionTime(UnitTypes eIndex, int iNewValue);													// Exposed to Python
	void changeUnitProductionTime(UnitTypes eIndex, int iChange);													// Exposed to Python

	bool isAnyProductionProgress(OrderTypes eOrder) const; // advc.opt

	int getGreatPeopleUnitRate(UnitTypes eIndex) const																// Exposed to Python
	{
		return m_aiGreatPeopleUnitRate.get(eIndex);
	}
	void setGreatPeopleUnitRate(UnitTypes eIndex, int iNewValue);
	void changeGreatPeopleUnitRate(UnitTypes eIndex, int iChange);

	int getGreatPeopleUnitProgress(UnitTypes eIndex) const															// Exposed to Python
	{
		return m_aiGreatPeopleUnitProgress.get(eIndex);
	}
	void setGreatPeopleUnitProgress(UnitTypes eIndex, int iNewValue);												// Exposed to Python
	void changeGreatPeopleUnitProgress(UnitTypes eIndex, int iChange);												// Exposed to Python

	int getSpecialistCount(SpecialistTypes eIndex) const															// Exposed to Python
	{
		return m_aiSpecialistCount.get(eIndex);
	}
	void setSpecialistCount(SpecialistTypes eIndex, int iNewValue);
	void changeSpecialistCount(SpecialistTypes eIndex, int iChange);
	void alterSpecialistCount(SpecialistTypes eIndex, int iChange);													// Exposed to Python

	int getMaxSpecialistCount(SpecialistTypes eIndex) const															// Exposed to Python
	{
		return m_aiMaxSpecialistCount.get(eIndex);
	}
	bool isSpecialistValid(SpecialistTypes eIndex, int iExtra = 0) const;											// Exposed to Python
	void changeMaxSpecialistCount(SpecialistTypes eIndex, int iChange);

	int getForceSpecialistCount(SpecialistTypes eIndex) const														// Exposed to Python
	{
		return m_aiForceSpecialistCount.get(eIndex);
	}
	bool isSpecialistForced() const;																				// Exposed to Python
	void setForceSpecialistCount(SpecialistTypes eIndex, int iNewValue);											// Exposed to Python
	void changeForceSpecialistCount(SpecialistTypes eIndex, int iChange);											// Exposed to Python

	int getFreeSpecialistCount(SpecialistTypes eIndex) const														// Exposed to Python
	{
		return m_aiFreeSpecialistCount.get(eIndex);
	}
	void setFreeSpecialistCount(SpecialistTypes eIndex, int iNewValue);												// Exposed to Python
	void changeFreeSpecialistCount(SpecialistTypes eIndex, int iChange);											// Exposed to Python
	int getAddedFreeSpecialistCount(SpecialistTypes eIndex) const;													// Exposed to Python

	int getImprovementFreeSpecialists(ImprovementTypes eIndex) const												// Exposed to Python
	{
		return m_aiImprovementFreeSpecialists.get(eIndex);
	}  // <advc.opt>
	bool isAnyImprovementFreeSpecialist() const
	{
		return m_aiImprovementFreeSpecialists.hasContent();
	} // </advc.opt>
	void changeImprovementFreeSpecialists(ImprovementTypes eIndex, int iChange);									// Exposed to Python

	int getReligionInfluence(ReligionTypes eIndex) const															// Exposed to Python
	{
		return m_aiReligionInfluence.get(eIndex);
	}
	void changeReligionInfluence(ReligionTypes eIndex, int iChange);												// Exposed to Python

	int getCurrentStateReligionHappiness() const;																	// Exposed to Python
	int getStateReligionHappiness(ReligionTypes eIndex) const														// Exposed to Python
	{
		return m_aiStateReligionHappiness.get(eIndex);
	}
	void changeStateReligionHappiness(ReligionTypes eIndex, int iChange);											// Exposed to Python

	int getUnitCombatFreeExperience(UnitCombatTypes eIndex) const													// Exposed to Python
	{
		return m_aiUnitCombatFreeExperience.get(eIndex);
	}
	void changeUnitCombatFreeExperience(UnitCombatTypes eIndex, int iChange);

	int getFreePromotionCount(PromotionTypes eIndex) const															// Exposed to Python
	{
		return m_aiFreePromotionCount.get(eIndex);
	}
	bool isFreePromotion(PromotionTypes eIndex) const																// Exposed to Python
	{
		return (getFreePromotionCount(eIndex) > 0);
	}  // <advc.opt>
	bool isAnyFreePromotion() const
	{
		return m_aiFreePromotionCount.hasContent();
	} // </advc.opt>
	void changeFreePromotionCount(PromotionTypes eIndex, int iChange);

	int getSpecialistFreeExperience() const																			// Exposed to Python
	{
		return m_iSpecialistFreeExperience;
	}
	void changeSpecialistFreeExperience(int iChange);

	int getEspionageDefenseModifier() const																			// Exposed to Python
	{
		return m_iEspionageDefenseModifier;
	}
	void changeEspionageDefenseModifier(int iChange);

	bool isWorkingPlot(CityPlotTypes ePlot) const																	// Exposed to Python
	{
		return m_abWorkingPlot.get(ePlot);
	}
	bool isWorkingPlot(const CvPlot* pPlot) const;																	// Exposed to Python
	void setWorkingPlot(CityPlotTypes ePlot, bool bNewValue);
	void setWorkingPlot(CvPlot* pPlot, bool bNewValue);
	void alterWorkingPlot(CityPlotTypes ePlot);																		// Exposed to Python

	int getNumRealBuilding(BuildingTypes eIndex) const																// Exposed to Python
	{
		return m_aiNumRealBuilding.get(eIndex);
	}
	int getNumRealBuilding(BuildingClassTypes eBuildingClass) const; // advc.003w
	void setNumRealBuilding(BuildingTypes eBuilding, int iNewValue,													// Exposed to Python
			bool bEndOfTurn = false); // advc.001x
	void setNumRealBuildingTimed(BuildingTypes eIndex, int iNewValue, bool bFirst,
			PlayerTypes eOriginalOwner, int iOriginalTime, /* advc.001x */ bool bEndOfTurn = false);

	bool isValidBuildingLocation(BuildingTypes eIndex) const;

	int getNumFreeBuilding(BuildingTypes eIndex) const																// Exposed to Python
	{
		return m_aiNumFreeBuilding.get(eIndex);
	}
	void setNumFreeBuilding(BuildingTypes eIndex, int iNewValue);

	bool isHasReligion(ReligionTypes eIndex) const																	// Exposed to Python
	{
		return m_abHasReligion.get(eIndex);
	}
	void setHasReligion(ReligionTypes eReligion, bool bNewValue, bool bAnnounce, bool bArrows = true,
			PlayerTypes eSpreadPlayer = NO_PLAYER); // advc.106e
	int getReligionGrip(ReligionTypes eReligion) const; // K-Mod
	bool isHasCorporation(CorporationTypes eIndex) const															// Exposed to Python
	{
		return m_abHasCorporation.get(eIndex);
	}
	void setHasCorporation(CorporationTypes eIndex, bool bNewValue,
			bool bAnnounce, bool bArrows = true);

	CvCity* getTradeCity(int iIndex) const;																			// Exposed to Python
	int getTradeRoutes() const;																						// Exposed to Python
	void clearTradeRoutes();
	void updateTradeRoutes();

	void clearOrderQueue();																							// Exposed to Python
	//void pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce = false);
	// K-Mod. (the old version is still exposed to Python)
	void pushOrder(OrderTypes eOrder, int iData1, int iData2 = -1, bool bSave = false,
			bool bPop = false, int iPosition = 0, bool bForce = false);
	void popOrder(int iNum, bool bFinish = false, bool bChoose = false,												// Exposed to Python
			bool bEndOfTurn = true); // advc.001x
	void startHeadOrder();
	void stopHeadOrder();
	int getOrderQueueLength() /* advc: */ const																		// Exposed to Python
	{
		return m_orderQueue.getLength(); // advc.inl
	}
	OrderData* getOrderFromQueue(int iIndex) const;																	// Exposed to Python
	CLLNode<OrderData>* nextOrderQueueNode(CLLNode<OrderData>* pNode) const
	{
		return m_orderQueue.next(pNode); // advc.inl
	}  // <advc.003s>
	CLLNode<OrderData> const* nextOrderQueueNode(CLLNode<OrderData> const* pNode) const
	{
		return m_orderQueue.next(pNode);
	} // </advc.003s>
	CLLNode<OrderData>* headOrderQueueNode() const
	{
		return m_orderQueue.head(); // advc.inl
	}
	DllExport int getNumOrdersQueued() const
	{
		return m_orderQueue.getLength(); // advc.inl
	}
	DllExport OrderData getOrderData(int iIndex) const;

	// fill the kVisible array with buildings that you want shown in city, as well as the number of generics
	// This function is called whenever CvCity::setLayoutDirty() is called
	DllExport void getVisibleBuildings(std::list<BuildingTypes>& kVisible, int& iNumGenerics);
	bool isAllBuildingsVisible(TeamTypes eTeam, bool bDebug) const; // advc.045

	// Fill the kEffectNames array with references to effects in the CIV4EffectInfos.xml to have a
	// city play a given set of effects. This is called whenever the interface updates the city billboard
	// or when the zoom level changes
	DllExport void getVisibleEffects(ZoomLevelTypes eCurrentZoom, std::vector<const TCHAR*>& kEffectNames);

	// Billboard appearance controls
	DllExport void getCityBillboardSizeIconColors(NiColorA& kDotColor, NiColorA& kTextColor) const;
	DllExport const TCHAR* getCityBillboardProductionIcon() const;
	DllExport bool getFoodBarPercentages(std::vector<float>& afPercentages) const;
	DllExport bool getProductionBarPercentages(std::vector<float>& afPercentages) const;
	DllExport NiColorA getBarBackgroundColor() const;
	DllExport bool isStarCity() const;

	// Exposed to Python
	void setWallOverridePoints(const std::vector< std::pair<float, float> >&
			kPoints); // points are given in world space ... i.e. PlotXToPointX, etc
	DllExport const std::vector< std::pair<float, float> >& getWallOverridePoints() const;

	int getTriggerValue(EventTriggerTypes eTrigger) const;
	bool canApplyEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const;
	void applyEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData, bool bClear);
	bool isEventOccured(EventTypes eEvent) const;
	void setEventOccured(EventTypes eEvent, bool bOccured);
	void doPartisans(); // advc.003y

	int getBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield) const;							// Exposed to Python
	void setBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange);					// Exposed to Python
	void changeBuildingYieldChange(BuildingClassTypes eBuildingClass, YieldTypes eYield, int iChange);
	int getBuildingCommerceChange(BuildingClassTypes eBuildingClass, CommerceTypes eCommerce) const;				// Exposed to Python
	void setBuildingCommerceChange(BuildingClassTypes eBuildingClass, CommerceTypes eCommerce, int iChange);		// Exposed to Python
	void changeBuildingCommerceChange(BuildingClassTypes eBuildingClass, CommerceTypes eCommerce, int iChange);
	int getBuildingHappyChange(BuildingClassTypes eBuildingClass) const;											// Exposed to Python
	void setBuildingHappyChange(BuildingClassTypes eBuildingClass, int iChange);								  	// Exposed to Python
	int getBuildingHealthChange(BuildingClassTypes eBuildingClass) const;											// Exposed to Python
	void setBuildingHealthChange(BuildingClassTypes eBuildingClass, int iChange);									// Exposed to Python

	PlayerTypes getLiberationPlayer(bool bConquest /* advc: */ = false) const;										// Exposed to Python
	void liberate(bool bConquest, /* advc.ctr: */ bool bPeaceDeal = false);											// Exposed to Python
	void meetNewOwner(TeamTypes eOtherTeam, TeamTypes eNewOwner) const; // advc.071

	void changeNoBonusCount(BonusTypes eBonus, int iChange);														// Exposed to Python
	int getNoBonusCount(BonusTypes eBonus) const
	{
		return m_aiNoBonus.get(eBonus);
	}
	bool isNoBonus(BonusTypes eBonus) const																			// Exposed to Python
	{
		return (getNoBonusCount(eBonus) > 0);
	}

	bool isAutoRaze() const;

	DllExport int getMusicScriptId() const;
	DllExport int getSoundscapeScriptId() const;
	DllExport void cheat(bool bCtrl, bool bAlt, bool bShift);

	DllExport void getBuildQueue(std::vector<std::string>& astrQueue) const;

	void invalidatePopulationRankCache() { m_bPopulationRankValid = false; } // advc.inl
	void invalidateYieldRankCache(YieldTypes eYield = NO_YIELD);
	void invalidateCommerceRankCache(CommerceTypes eCommerce = NO_COMMERCE);
	//int getBestYieldAvailable(YieldTypes eYield) const; // advc.003j: obsolete

	// <advc.003u>
	// virtual for FFreeListTrashArray
	virtual void read(FDataStreamBase* pStream); 
	virtual void write(FDataStreamBase* pStream);
	__forceinline CvCityAI& AI()
	{	//return *static_cast<CvCityAI*>(const_cast<CvCity*>(this));
		/*  The above won't work in an inline function b/c the compiler doesn't know
			that CvCityAI is derived from CvCity */
		return *reinterpret_cast<CvCityAI*>(this);
	}
	__forceinline CvCityAI const& AI() const
	{	//return *static_cast<CvCityAI const*>(this);
		return *reinterpret_cast<CvCityAI const*>(this);
	}
	/*  Keep one pure virtual function to make the class abstract; remove all
		the others - the EXE doesn't call them. */ // </advc.003u>
	virtual void AI_setAssignWorkDirty(bool bNewValue) = 0;

protected:
	// <advc.003u>
	CvCity();
	/*  Subclasses need to call this; not called by base.
		May also want to override it. </advc.003u> */
	virtual void init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits, bool bUpdatePlotGroups,
			int iOccupationTimer); // advc.ctr

	// <advc> Moved here for quicker inspection in debugger
	CvWString m_szName;
	PlayerTypes m_eOwner; // </advc>
	int m_iID;
	int m_iX;
	int m_iY;
	int m_iRallyX;
	int m_iRallyY;
	int m_iGameTurnFounded;
	int m_iGameTurnAcquired;
	int m_iPopulation;
	/* Population Limit ModComp - Beginning */
	int m_iPopulationLimitChange;
	/* Population Limit ModComp - End */
	int m_iHighestPopulation;
	int m_iWorkingPopulation;
	int m_iSpecialistPopulation;
	int m_iNumGreatPeople;
	int m_iBaseGreatPeopleRate;
	int m_iGreatPeopleRateModifier;
	int m_iGreatPeopleProgress;
	int m_iNumWorldWonders;
	int m_iNumTeamWonders;
	int m_iNumNationalWonders;
	int m_iNumBuildings;
	int m_iGovernmentCenterCount;
	int m_iMaintenance;
	int m_iMaintenanceModifier;
	int m_iWarWearinessModifier;
	int m_iHurryAngerModifier;
	int m_iHealRate;
	int m_iEspionageHealthCounter;
	int m_iEspionageHappinessCounter;
	int m_iFreshWaterGoodHealth;
	int m_iFreshWaterBadHealth;
	int m_iSurroundingGoodHealth;
	int m_iSurroundingBadHealth;
/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: New Tag Definition                                                               **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
	int m_iTerrainGoodHealth;
	int m_iTerrainBadHealth;
/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**                                                                                              */
/*************************************************************************************************/
	int m_iSpecialistGoodHealth;
	int m_iSpecialistBadHealth;
	int m_iSpecialistHappiness;
	int m_iSpecialistUnhappiness;
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/
	int m_iBuildingGoodHealth;
	int m_iBuildingBadHealth;
	// < Civic Infos Plus Start >
	int m_iReligionGoodHealth;
	int m_iReligionBadHealth;
	// < Civic Infos Plus Start >
	int m_iPowerGoodHealth;
	int m_iPowerBadHealth;
	int m_iBonusGoodHealth;
	int m_iBonusBadHealth;
	int m_iHurryAngerTimer;
	int m_iConscriptAngerTimer;
	int m_iDefyResolutionAngerTimer;
	int m_iHappinessTimer;
	int m_iMilitaryHappinessUnits;
	int m_iBuildingGoodHappiness;
	int m_iBuildingBadHappiness;
	int m_iExtraBuildingGoodHappiness;
	int m_iExtraBuildingBadHappiness;
	int m_iExtraBuildingGoodHealth;
	int m_iExtraBuildingBadHealth;
	int m_iSurroundingGoodHappiness;
	int m_iSurroundingBadHappiness;
	int m_iBonusGoodHappiness;
	int m_iBonusBadHappiness;
	int m_iReligionGoodHappiness;
	int m_iReligionBadHappiness;
	int m_iExtraHappiness;
	int m_iExtraHealth;
	int m_iNoUnhappinessCount;
	int m_iUnhealthyPopulationModifier; // K-Mod
	int m_iBuildingOnlyHealthyCount;
	int m_iFood;
	int m_iFoodKept;
	int m_iMaxFoodKeptPercent;
	int m_iOverflowProduction;
	int m_iFeatureProduction;
	int m_iMilitaryProductionModifier;
	int m_iSpaceProductionModifier;
	int m_iExtraTradeRoutes;
	int m_iTradeRouteModifier;
	int m_iForeignTradeRouteModifier;
	int m_iBuildingDefense;
	int m_iBuildingBombardDefense;
	int m_iFreeExperience;
	int m_iCurrAirlift;
	int m_iMaxAirlift;
	int m_iAirModifier;
	int m_iAirUnitCapacity;
	int m_iNukeModifier;
	int m_iFreeSpecialist;
	int m_iPowerCount;
	int m_iDirtyPowerCount;
	int m_iDefenseDamage;
	int m_iLastDefenseDamage;
	int m_iOccupationTimer;
	int m_iCultureUpdateTimer;
	int m_iCitySizeBoost;
	int m_iSpecialistFreeExperience;
	int m_iEspionageDefenseModifier;
	int m_iPopRushHurryCount; // advc.912d
	int m_iMostRecentOrder; // advc.004x

	bool m_bNeverLost;
	bool m_bBombarded;
	bool m_bDrafted;
	bool m_bAirliftTargeted;
	bool m_bWeLoveTheKingDay;
	bool m_bCitizensAutomated;
	bool m_bProductionAutomated;
	bool m_bWallOverride;
	bool m_bInfoDirty;
	bool m_bLayoutDirty;
	bool m_bPlundered;
	bool m_bInvestigate; // advc.103: Refers to the active team
	bool m_bMostRecentUnit; // advc.004x
	bool m_bChooseProductionDirty; // advc.003u: Moved from CvCityAI

	PlayerTypes m_ePreviousOwner;
	PlayerTypes m_eOriginalOwner;
	CultureLevelTypes m_eCultureLevel;

	// <advc.enum> (Tbd.: short int would suffice; except for m_aiCulture.)
	EnumMap<YieldTypes,int> m_aiSeaPlotYield;
	EnumMap<YieldTypes,int> m_aiRiverPlotYield;
	EnumMap<YieldTypes,int> m_aiBaseYieldRate;
	EnumMap<YieldTypes,int> m_aiYieldRateModifier;
	EnumMap<YieldTypes,int> m_aiPowerYieldRateModifier;
	EnumMap<YieldTypes,int> m_aiBonusYieldRateModifier;
    // < Civic Infos Plus Start >
	//removed by f1 advc - keldath
	//int* m_aiBuildingYieldChange;
	//int* m_aiStateReligionYieldRateModifier;
	//int* m_aiNonStateReligionYieldRateModifier;
	EnumMap<YieldTypes,int> m_aiBuildingYieldChange;
	EnumMap<YieldTypes,int> m_aiStateReligionYieldRateModifier;
	EnumMap<YieldTypes,int> m_aiNonStateReligionYieldRateModifier;
	// < Civic Infos Plus End >
	EnumMap<YieldTypes,int> m_aiTradeYield;
	EnumMap<YieldTypes,int> m_aiCorporationYield;
	EnumMap<YieldTypes,int> m_aiExtraSpecialistYield;
	EnumMap<CommerceTypes,int> m_aiCommerceRate;
	EnumMap<CommerceTypes,int> m_aiProductionToCommerceModifier;
	EnumMap<CommerceTypes,int> m_aiBuildingCommerce;
	EnumMap<CommerceTypes,int> m_aiSpecialistCommerce;
	EnumMap<CommerceTypes,int> m_aiReligionCommerce;
	// < Civic Infos Plus Start >
	//removed by f1 advc - keldath
	//int* m_aiBuildingCommerceChange;
	//int* m_aiStateReligionCommerceRateModifier;
	//int* m_aiNonStateReligionCommerceRateModifier;
	EnumMap<CommerceTypes,int> m_aiBuildingCommerceChange;	
	EnumMap<CommerceTypes,int> m_aiStateReligionCommerceRateModifier;	
	EnumMap<CommerceTypes,int> m_aiNonStateReligionCommerceRateModifier;	
	// < Civic Infos Plus End   >
/*************************************************************************************************/
/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
/**																								**/
/**																								**/
/*************************************************************************************************/
//	int* m_aiExtraSpecialistCommerce;
	EnumMap<CommerceTypes,int> m_aiExtraSpecialistCommerce;	
/*************************************************************************************************/
/**	CMEDIT: End																					**/
/*************************************************************************************************/
	EnumMap<CommerceTypes,int> m_aiCorporationCommerce;
	EnumMap<CommerceTypes,int> m_aiCommerceRateModifier;
	EnumMap<CommerceTypes,int> m_aiCommerceHappinessPer;
	EnumMap<DomainTypes,int> m_aiDomainFreeExperience;
	EnumMap<DomainTypes,int> m_aiDomainProductionModifier;
	EnumMap<PlayerTypes,int> m_aiCulture;
	EnumMap<PlayerTypes,int> m_aiNumRevolts;
	EnumMap<BonusTypes,int> m_aiNoBonus;
	EnumMap<BonusTypes,int> m_aiFreeBonus;
	EnumMap<BonusTypes,int> m_aiNumBonuses;
	// < Building Resource Converter Start >
	//int* m_paiBuildingOutputBonuses;
	EnumMap<BonusTypes,int> m_paiBuildingOutputBonuses;
	// < Building Resource Converter End   >
	EnumMap<BonusTypes,int> m_aiNumCorpProducedBonuses;
	EnumMap<ProjectTypes,int> m_aiProjectProduction;
	EnumMap<BuildingTypes,int> m_aiBuildingProduction;
	EnumMap<BuildingTypes,int> m_aiBuildingProductionTime;
	EnumMap<BuildingTypes,int> m_aiBuildingOriginalOwner;
//prereqMust+tholish - this enum array will allow to keep tarck of shich buildings
//were set to inactive
	EnumMap<BuildingTypes,bool> m_aiBuildingeActive;
	EnumMapDefault<BuildingTypes,int,MIN_INT> m_aiBuildingOriginalTime;
	EnumMap<BuildingTypes,int> m_aiNumRealBuilding;
	EnumMap<BuildingTypes,int> m_aiNumFreeBuilding;
	EnumMap<UnitTypes,int> m_aiUnitProduction;
	EnumMap<UnitTypes,int> m_aiUnitProductionTime;
	EnumMap<UnitTypes,int> m_aiGreatPeopleUnitRate;
	EnumMap<UnitTypes,int> m_aiGreatPeopleUnitProgress;
	EnumMap<SpecialistTypes,int> m_aiSpecialistCount;
	EnumMap<SpecialistTypes,int> m_aiMaxSpecialistCount;
	EnumMap<SpecialistTypes,int> m_aiForceSpecialistCount;
	EnumMap<SpecialistTypes,int> m_aiFreeSpecialistCount;
	EnumMap<ImprovementTypes,int> m_aiImprovementFreeSpecialists;
	EnumMap<ReligionTypes,int> m_aiReligionInfluence;
	EnumMap<ReligionTypes,int> m_aiStateReligionHappiness;
	EnumMap<UnitCombatTypes,int> m_aiUnitCombatFreeExperience;
	EnumMap<PromotionTypes,int> m_aiFreePromotionCount;

	EnumMap<PlayerTypes,bool> m_abEverOwned;
	EnumMap<PlayerTypes,bool> m_abTradeRoute;
	EnumMap<TeamTypes,bool> m_abRevealed;
	EnumMap<TeamTypes,bool> m_abEspionageVisibility;
	EnumMap<CityPlotTypes,bool> m_abWorkingPlot;
	EnumMap<ReligionTypes,bool> m_abHasReligion;
	EnumMap<CorporationTypes,bool> m_abHasCorporation;
	// </advc.enum>
	CvWString m_szPreviousName; // advc.106k
	CvString m_szScriptData;
	// <advc.opt>
	CvArea* m_pArea;
	CvPlot* m_pPlot; // </advc.opt>
	std::vector<IDInfo> m_aTradeCities; // advc: was an array

	mutable CLinkList<OrderData> m_orderQueue;

	std::vector<std::pair<float, float> > m_kWallOverridePoints;

	std::vector<EventTypes> m_aEventsOccured;
	std::vector<BuildingYieldChange> m_aBuildingYieldChange;
	std::vector<BuildingCommerceChange> m_aBuildingCommerceChange;
	BuildingChangeArray m_aBuildingHappyChange;
	BuildingChangeArray m_aBuildingHealthChange;

	// Rank cache
	mutable int	m_iPopulationRank;
	mutable bool m_bPopulationRankValid;
	// <advc.enum>
	/*	Made mutable (not strictly necessary b/c findBaseYieldRateRank
		accesses them through a CvCity pointer) */
	mutable EnumMapDefault<YieldTypes,int,-1> m_aiBaseYieldRank;
	mutable EnumMap<YieldTypes,bool> m_abBaseYieldRankValid;
	mutable EnumMapDefault<YieldTypes,int,-1> m_aiYieldRank;
	mutable EnumMap<YieldTypes,bool> m_abYieldRankValid;
	mutable EnumMapDefault<CommerceTypes,int,-1> m_aiCommerceRank;
	mutable EnumMap<CommerceTypes,bool> m_abCommerceRankValid; // </advc.enum>

	void doGrowth();
	void doCulture();
	bool doCheckProduction();
	void upgradeProduction(); // advc.064d
	bool checkCanContinueProduction(bool bCheckUpgrade = true); // advc.064d
	void doProduction(bool bAllowNoProduction);
	void doDecay();
	void doReligion();
	void doGreatPeople();
	void doMeltdown();
		// advc.inl: Allow getExtraProductionDifference to be inlined ...
	int getExtraProductionDifference(int iExtra, UnitTypes eUnit) const
	{
		return getExtraProductionDifference(iExtra, getProductionModifier(eUnit));
	}
	int getExtraProductionDifference(int iExtra, BuildingTypes eBuilding) const
	{
		return getExtraProductionDifference(iExtra, getProductionModifier(eBuilding));
	}
	int getExtraProductionDifference(int iExtra, ProjectTypes eProject) const
	{
		return getExtraProductionDifference(iExtra, getProductionModifier(eProject));
	}
	int getExtraProductionDifference(int iExtra, int iModifier) const
	{
		return ((iExtra * getBaseYieldRateModifier(YIELD_PRODUCTION, iModifier)) / 100);
	}
	int getHurryCostModifier(UnitTypes eUnit, bool bIgnoreNew) const;
	int getHurryCostModifier(BuildingTypes eBuilding, bool bIgnoreNew) const;
	int getHurryCostModifier(int iBaseModifier, int iProduction, bool bIgnoreNew) const;
	int getHurryCost(bool bExtra, UnitTypes eUnit, bool bIgnoreNew) const;
	int getHurryCost(bool bExtra, BuildingTypes eBuilding, bool bIgnoreNew) const;
	int getHurryCost(bool bExtra, int iProductionLeft, int iHurryModifier, int iModifier) const;
	int getHurryPopulation(HurryTypes eHurry, int iHurryCost) const;
	int getHurryGold(HurryTypes eHurry, int iHurryCost) const;
	bool canHurryUnit(HurryTypes eHurry, UnitTypes eUnit, bool bIgnoreNew) const;
	bool canHurryBuilding(HurryTypes eHurry, BuildingTypes eBuilding, bool bIgnoreNew) const;
	// <advc.310>
	void addGreatWall(int iAttempt = 0); // Wrapper for CvEngine::AddGreatWall
	bool needsGreatWallSegment(CvPlot const& kInside, CvPlot const& kOutside,
			int iAttempt) const;
	// </advc.310>
	void updateBuildingDefense(); // advc.004c
	double defensiveGarrison(double stopCountingAt = -1) const; // advc.500b
	//int calculateMaintenanceDistance() const;
	// advc.004b: Replacing the above (which was public, but is only used internally)
	static int calculateMaintenanceDistance(CvPlot const* cityPlot, PlayerTypes owner);
	void damageGarrison(PlayerTypes eRevoltSource);
	// advc.123f:
	void failProduction(int iOrderData, int iInvestedProduction, bool bProject = false);
	// <advc.064b>
	void handleOverflow(int iRawOverflow, int iProductionModifier, OrderTypes eOrderType);
	int failGoldPercent(OrderTypes eOrder) const; // also used by 123f
	void payOverflowGold(int iLostProduction, int iGoldChange);
	int getProductionTurnsLeft(int iProductionNeeded, int iProduction,
			int iProductionModifier, bool bFoodProduction, int iNum) const;
	// </advc.064b
	void doPopOrder(CLLNode<OrderData>* pOrder); // advc.064d
	// advc.901:
	std::pair<int,int> calculateSurroundingHealth(int iExtraGoodPercent = 0, int iExtraBadPercent = 0) const;
	// BETTER_BTS_AI_MOD (from BUG), 02/24/10, EmperorFool: START
		// advc: These were declared outside of CvCity (global)
	static void addGoodOrBad(int iValue, int& iGood, int& iBad);
	static void subtractGoodOrBad(int iValue, int& iGood, int& iBad);
	// BETTER_BTS_AI_MOD: END
};

#endif
