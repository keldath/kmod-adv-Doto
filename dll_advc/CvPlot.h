#pragma once

// CvPlot.h

#ifndef CIV4_PLOT_H
#define CIV4_PLOT_H

#pragma warning( disable: 4251 ) // needs to have dll-interface to be used by clients of class

class CvArea;
class CvMap;
class CvPlotBuilder;
class CvRoute;
class CvRiver;
class CvCity;
class CvPlotGroup;
class CvFeature;
class CvUnit;
class CvSymbol;
class CvFlagEntity;

typedef bool (*ConstPlotUnitFunc)(const CvUnit* pUnit, int iData1, int iData2);
typedef bool (*PlotUnitFunc)(CvUnit* pUnit, int iData1, int iData2);

class CvPlot
{
public:

	CvPlot();
	~CvPlot();

	void init(int iX, int iY);
	void setupGraphical();
	void updateGraphicEra();

	void erase();																																								// Exposed to Python

	DllExport float getPointX() const;
	DllExport float getPointY() const;
	DllExport NiPoint3 getPoint() const;																																	// Exposed to Python

	float getSymbolSize() const;
	DllExport float getSymbolOffsetX(int iID) const;
	DllExport float getSymbolOffsetY(int iID) const;

	void doTurn();
	void doImprovement();

	void updateCulture(bool bBumpUnits, bool bUpdatePlotGroups);
	void updateFog();
	void updateVisibility();
	void updateSymbolDisplay();
	void updateSymbolVisibility();
	void updateSymbols();
	void updateMinimapColor();
	void updateCenterUnit();

	void verifyUnitValidPlot();
	void forceBumpUnits(); // K-Mod

	void nukeExplosion(int iRange, CvUnit* pNukeUnit = NULL,
			bool bBomb = true); //  K-Mod added bBomb, Exposed to Python

	bool isConnectedTo(CvCity const* pCity) const;																												// Exposed to Python
	bool isConnectedToCapital(PlayerTypes ePlayer = NO_PLAYER) const;																			// Exposed to Python
	int getPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const;													// Exposed to Python
	bool isPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const								// Exposed to Python
	{
		return (getPlotGroupConnectedBonus(ePlayer, eBonus) > 0); // advc.inl
	}
	bool isAdjacentPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const;				// Exposed to Python
	void updatePlotGroupBonus(bool bAdd, /* advc.064d: */ bool bVerifyProduction = true);

	//bool isAdjacentToArea(int iAreaID) const; // advc: use the one below instead
	bool isAdjacentToArea(CvArea const& kArea) const;																						// Exposed to Python
	bool shareAdjacentArea(const CvPlot* pPlot) const;																					// Exposed to Python
	bool isAdjacentToLand() const;																															// Exposed to Python
	// advc.003t: Default was -1, which now means MIN_WATER_SIZE_FOR_OCEAN.
	bool isCoastalLand(int iMinWaterSize = 0) const;																																	// Exposed to Python

	bool isVisibleWorked() const;
	bool isWithinTeamCityRadius(TeamTypes eTeam, PlayerTypes eIgnorePlayer = NO_PLAYER) const;	// Exposed to Python

	DllExport bool isLake() const;																															// Exposed to Python
	bool isFreshWater() const;																												// Exposed to Python
	bool isPotentialIrrigation() const;																													// Exposed to Python
	bool canHavePotentialIrrigation() const;																										// Exposed to Python
	DllExport bool isIrrigationAvailable(bool bIgnoreSelf = false) const;													// Exposed to Python

	bool isRiverMask() const;
	DllExport bool isRiverCrossingFlowClockwise(DirectionTypes eDirection) const;
	bool isRiverSide() const;																																		// Exposed to Python
	bool isRiver() const { return (getRiverCrossingCount() > 0); } // advc.inl												// Exposed to Python
	bool isRiverConnection(DirectionTypes eDirection) const;																// Exposed to Python
	// advc.500:
	bool isConnectRiverSegments() const;
	// advc.121: A kind of canal detection
	bool isConnectSea() const;

	CvPlot* getNearestLandPlotInternal(int iDistance) const;
	CvArea* getNearestLandArea() const;																															// Exposed to Python
	CvPlot* getNearestLandPlot() const { return getNearestLandPlotInternal(0); } // advc.inl									// Exposed to Python

	int seeFromLevel(TeamTypes eTeam) const;																										// Exposed to Python
	int seeThroughLevel() const;																																// Exposed to Python
	void changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement,
			CvUnit const* pUnit, bool bUpdatePlotGroups);
	bool canSeePlot(CvPlot const* pPlot, TeamTypes eTeam, int iRange, DirectionTypes eFacingDirection) const;
	bool canSeeDisplacementPlot(TeamTypes eTeam, int dx, int dy,
			int originalDX, int originalDY, bool firstPlot, bool outerRing) const;
	bool shouldProcessDisplacementPlot(int dx, int dy, int range, DirectionTypes eFacingDirection) const;
	void updateSight(bool bIncrement, bool bUpdatePlotGroups);
	void updateSeeFromSight(bool bIncrement, bool bUpdatePlotGroups);

	bool canHaveBonus(BonusTypes eBonus, bool bIgnoreLatitude = false,																						// Exposed to Python
			bool bIgnoreFeature = false) const; // advc.129
	bool canHaveImprovement(ImprovementTypes eImprovement,														// Exposed to Python
			TeamTypes eTeam = NO_TEAM, bool bPotential = false,
			BuildTypes eBuild = NO_BUILD, bool bAnyBuild = true) const; // dlph.9
	// < JImprovementLimit Mod Start >
	bool isImprovementInRange(ImprovementTypes eImprovement, int iRange, bool bCheckBuildProgress) const;               // Exposed to Python
	bool isImprovementAncestor(ImprovementTypes eImprovement, ImprovementTypes eCheckImprovement) const;               // Exposed to Python
	// < JImprovementLimit Mod End >
	bool canBuild(BuildTypes eBuild, PlayerTypes ePlayer = NO_PLAYER, bool bTestVisible = false) const;														// Exposed to Python
	int getBuildTime(BuildTypes eBuild,																																										// Exposed to Python
			PlayerTypes ePlayer) const; // advc.251
	
// Deliverator
	void changeFreshWater(int iChange);
	void changeFreshWaterInRadius(int iChange, int Radius);
// Deliverator
	int getBuildTurnsLeft(BuildTypes eBuild, /* advc.251: */ PlayerTypes ePlayer,
			int iNowExtra, int iThenExtra,																			// Exposed to Python
			// <advc.011c>
			bool bIncludeUnits = true) const;
	int getBuildTurnsLeft(BuildTypes eBuild, PlayerTypes ePlayer) const;
	// </advc.011c>
	int getFeatureProduction(BuildTypes eBuild, TeamTypes eTeam, CvCity** ppCity,																// Exposed to Python
			CvPlot const* pCityPlot = NULL, PlayerTypes eCityOwner = NO_PLAYER) const; // advc.031

	DllExport CvUnit* getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER,													// Exposed to Python
		const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bTestPotentialEnemy = false,
		/*  advc.028: Replacing unused bTestCanMove. False by default b/c invisible units are
			generally able to defend - they just choose not to (CvUnit::isBetterDefenderThan). */
		bool bTestVisible = false) const  // <advc> Need some more params
	{
		return getBestDefender(eOwner, eAttackingPlayer, pAttacker, bTestAtWar,
				bTestPotentialEnemy, bTestVisible,
				// advc.089: bTestCanAttack=true by default
				true, false);
	}
	CvUnit* getBestDefender(PlayerTypes eOwner,
			PlayerTypes eAttackingPlayer, CvUnit const* pAttacker,
			bool bTestEnemy, bool bTestPotentialEnemy,
			bool bTestVisible, // advc.028
			bool bTestCanAttack, bool bAny = false) const; // </advc>
	// BETTER_BTS_AI_MOD, Lead From Behind (UncutDragon), 02/21/10, jdog5000:
	bool hasDefender(bool bTestCanAttack, PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bTestPotentialEnemy = false
			/*,bool bTestCanMove = false*/) const; // advc: param unused (and doesn't make sense to me)
	// disabled by K-Mod:
	//int AI_sumStrength(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, DomainTypes eDomainType = NO_DOMAIN, bool bDefensiveBonuses = true, bool bTestAtWar = false, bool bTestPotentialEnemy = false) const;
	CvUnit* getSelectedUnit() const;																																// Exposed to Python
	int getUnitPower(PlayerTypes eOwner = NO_PLAYER) const;																					// Exposed to Python

	int defenseModifier(TeamTypes eDefender, bool bIgnoreBuilding,												// Exposed to Python
		/*  advc.012: NO_TEAM means rival defense applies; moved bHelp to the
			end b/c that parameter is rarely set */
			TeamTypes eAttacker = NO_TEAM, bool bHelp = false,
			bool bGarrisonStrength = false) const; // advc.500b
	int movementCost(const CvUnit* pUnit, const CvPlot* pFromPlot,														// Exposed to Python
			bool bAssumeRevealed = true) const; // advc.001i

	int getExtraMovePathCost() const;																																// Exposed to Python
	void changeExtraMovePathCost(int iChange);																																// Exposed to Python

	bool isAdjacentOwned() const;																																		// Exposed to Python
	bool isAdjacentPlayer(PlayerTypes ePlayer, bool bLandOnly = false) const;												// Exposed to Python
	bool isAdjacentTeam(TeamTypes eTeam, bool bLandOnly = false) const;															// Exposed to Python
	bool isWithinCultureRange(PlayerTypes ePlayer) const;																						// Exposed to Python
	int getNumCultureRangeCities(PlayerTypes ePlayer) const;																				// Exposed to Python

	// BETTER_BTS_AI_MOD, General AI, 11/30/08, jdog5000: START
			// advc: const qualifier added to these two
	bool isHasPathToEnemyCity(TeamTypes eAttackerTeam, bool bIgnoreBarb = true) const;
	bool isHasPathToPlayerCity(TeamTypes eMoveTeam, PlayerTypes eOtherPlayer = NO_PLAYER) const;
	int calculatePathDistanceToPlot(TeamTypes eTeam, // <advc.104b>
			CvPlot const& kTargetPlot, TeamTypes eTargetTeam = NO_TEAM,
			DomainTypes eDomain = NO_DOMAIN, int iMaxPath = -1) const; // </advc.104b>
	// BETTER_BTS_AI_MOD: END
	// BETTER_BTS_AI_MOD, Efficiency, 08/21/09, jdog5000: START
	// Plot danger cache (rewritten for K-Mod to fix bugs and improvement performance)
	inline int getActivePlayerSafeRangeCache() const { return m_iActivePlayerSafeRangeCache; }
	inline void setActivePlayerSafeRangeCache(int range) const { m_iActivePlayerSafeRangeCache = range; } // advc: const
	inline bool getBorderDangerCache(TeamTypes eTeam) const { return m_abBorderDangerCache.get(eTeam); }
	inline void setBorderDangerCache(TeamTypes eTeam, bool bNewValue) const { m_abBorderDangerCache.set(eTeam, bNewValue); } // advc: const
	void invalidateBorderDangerCache();
	// BETTER_BTS_AI_MOD: END
	PlayerTypes calculateCulturalOwner(
			bool bIgnoreCultureRange = false, // advc.099c
			bool bOwnExclusiveRadius = false) const; // advc.035

	void plotAction(PlotUnitFunc func, int iData1 = -1, int iData2 = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM);
	int plotCount(ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1) const;
	CvUnit* plotCheck(ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1) const;

	inline bool isOwned() const	/* advc.inl */																																		// Exposed to Python
	{
		return (getOwner() != NO_PLAYER);
	}
	bool isBarbarian() const { return (getOwner() == BARBARIAN_PLAYER); } // advc.inl											// Exposed to Python
	bool isRevealedBarbarian() const;																							// Exposed to Python

	bool isVisible(TeamTypes eTeam, bool bDebug) const;																			// Exposed to Python
	// advc: Make bDebug=false the default
	inline bool isVisible(TeamTypes eTeam) const
	{
		return isVisible(eTeam, false);
	}
	DllExport bool isActiveVisible(bool bDebug) const;																								// Exposed to Python
	bool isVisibleToCivTeam() const;																																// Exposed to Python
	// <advc.706>
	static bool isAllFog() { return m_bAllFog; }
	static void setAllFog(bool b) { m_bAllFog = b; } // </advc.706>
	// <advc.300>
	bool isCivUnitNearby(int iRadius) const;
	void getAdjacentLandAreaIds(std::set<int>& r) const;
	CvPlot const* nearestInvisiblePlot(bool bOnlyLand, int iMaxPlotDist, TeamTypes eObserver) const;
	// </advc.300>
	bool isVisibleToWatchingHuman() const;																														// Exposed to Python
	bool isAdjacentVisible(TeamTypes eTeam, bool bDebug) const;																				// Exposed to Python
	bool isAdjacentNonvisible(TeamTypes eTeam) const;																				// Exposed to Python

	DllExport bool isGoody(TeamTypes eTeam = NO_TEAM) const;																					// Exposed to Python
	bool isRevealedGoody(TeamTypes eTeam = NO_TEAM) const;																						// Exposed to Python
	void removeGoody();																																								// Exposed to Python

	DllExport bool isCity(bool bCheckImprovement, TeamTypes eForTeam = NO_TEAM) const;																																		// Exposed to Python
	// advc.inl: Faster version, inlined for CvArea::canBeEntered
	__forceinline bool isCity() const
	{	// (Should perhaps simply turn m_plotCity into a CvCity pointer.)
		return (m_plotCity.iID != NO_PLAYER); // avoid ::getCity call
	}
	bool isFriendlyCity(const CvUnit& kUnit, bool bCheckImprovement) const;																												// Exposed to Python
	bool isEnemyCity(const CvUnit& kUnit) const;																													// Exposed to Python

	bool isOccupation() const;																																				// Exposed to Python
	bool isBeingWorked() const;																															// Exposed to Python

	inline bool isUnit() const { return (getNumUnits() > 0); } // advc.inl														// Exposed to Python
	bool isInvestigate(TeamTypes eTeam) const;																												// Exposed to Python
	bool isVisibleEnemyDefender(const CvUnit* pUnit) const;																						// Exposed to Python
	CvUnit *getVisibleEnemyDefender(PlayerTypes ePlayer) const;
	int getNumDefenders(PlayerTypes ePlayer) const;																										// Exposed to Python
	int getNumVisibleEnemyDefenders(const CvUnit* pUnit) const;																				// Exposed to Python
	// (advc: getNumVisiblePotentialEnemyDefenders has become CvUnitAI::AI_countEnemyDefenders)
	DllExport bool isVisibleEnemyUnit(PlayerTypes ePlayer) const;																			// Exposed to Python
	// advc.ctr:
	bool isVisibleEnemyCityAttacker(PlayerTypes eDefender, TeamTypes eAssumePeace = NO_TEAM) const;
	// (advc: isVisiblePotentialEnemyUnit has become CvTeamAI::AI_mayAttack(CvPlot const&))
	DllExport int getNumVisibleUnits(PlayerTypes ePlayer) const;
	bool isVisibleEnemyUnit(const CvUnit* pUnit) const;
	// advc.004l:
	bool isVisibleEnemyUnit(CvUnit const* pUnit, CvUnit const* pPotentialEnemy) const;
	bool isVisibleOtherUnit(PlayerTypes ePlayer) const;																								// Exposed to Python
	DllExport bool isFighting() const;																																// Exposed to Python

	bool canHaveFeature(FeatureTypes eFeature,																								// Exposed to Python
			bool bIgnoreCurrentFeature = false) const; // advc.055
	// advc.inl
	DllExport inline bool isRoute() const																																		// Exposed to Python
	{
		return (getRouteType() != NO_ROUTE);
	}
	bool isValidRoute(const CvUnit* pUnit,																											// Exposed to Python
			bool bAssumeRevealed) const; // advc.001i
	// advc.inl
	inline bool isTradeNetworkImpassable(TeamTypes eTeam) const																														// Exposed to Python
	{
		return (isImpassable() && !isRiverNetwork(eTeam));
	}
	bool isNetworkTerrain(TeamTypes eTeam) const;																											// Exposed to Python
	bool isBonusNetwork(TeamTypes eTeam) const;																												// Exposed to Python
	bool isTradeNetwork(TeamTypes eTeam) const;																												// Exposed to Python
	bool isTradeNetworkConnected(CvPlot const& kOther, TeamTypes eTeam) const; // advc: param was CvPlot const*								// Exposed to Python
	bool isRiverNetwork(TeamTypes eTeam) const;

	bool isValidDomainForLocation(const CvUnit& unit) const;																					// Exposed to Python
	bool isValidDomainForAction(const CvUnit& unit) const;																						// Exposed to Python
	bool isImpassable() const;																															// Exposed to Python

	int getXExternal() const; // advc.inl: Exported through .def file																					// Exposed to Python
	inline int getX() const { return m_iX; } // advc.inl: Renamed from getX_INLINE
	int getYExternal() const; // advc.inl: Exported through .def file																					// Exposed to Python
	inline int getY() const { return m_iY; } // advc.inl: Renamed from getY_INLINE
	bool at(int iX, int iY) const {  return (getX() == iX && getY() == iY); } // advc.inl								// Exposed to Python
	int getMapIndex() const; // advc
	int getLatitude() const;																																					// Exposed to Python
	void setLatitude(int iLatitude); // advc.tsl	(exposed to Python)
	int getFOWIndex() const;

	//int getArea() const;
	// <advc>
	inline CvArea& getArea() const { return *m_pArea; }
	// (This had called CvMap::getArea in BtS)
	inline CvArea* area() const { return m_pArea; }													// Exposed to Python
	inline bool isArea(CvArea const& kArea) const { return (area() == &kArea); }
	inline bool sameArea(CvPlot const& kPlot) const { return isArea(kPlot.getArea()); }
	void initArea(); // </advc>
	CvArea* waterArea(
			// BETTER_BTS_AI_MOD, General AI, 01/02/09, jdog5000
			bool bNoImpassable = false) const;
	CvArea* secondWaterArea() const;
	void setArea(CvArea* pArea = NULL, /* advc.310: */ bool bProcess = true);

	DllExport int getFeatureVariety() const;																													// Exposed to Python

	int getOwnershipDuration() const { return m_iOwnershipDuration; } // advc.inl											// Exposed to Python
	bool isOwnershipScore() const;																																		// Exposed to Python
	void setOwnershipDuration(int iNewValue);																													// Exposed to Python
	void changeOwnershipDuration(int iChange);																												// Exposed to Python

	int getImprovementDuration() const { return m_iImprovementDuration; } // advc.inl										// Exposed to Python
	void setImprovementDuration(int iNewValue);																												// Exposed to Python
	void changeImprovementDuration(int iChange);																											// Exposed to Python

	int getUpgradeProgress() const { return m_iUpgradeProgress; } // advc.inl												// Exposed to Python
	int getUpgradeTimeLeft(ImprovementTypes eImprovement, PlayerTypes ePlayer) const;										// Exposed to Python
	void setUpgradeProgress(int iNewValue);																														// Exposed to Python
	void changeUpgradeProgress(int iChange);																													// Exposed to Python

	int getForceUnownedTimer() const { return m_iForceUnownedTimer; } // advc.inl											// Exposed to Python
	bool isForceUnowned() const;																																			// Exposed to Python
	void setForceUnownedTimer(int iNewValue);																													// Exposed to Python
	void changeForceUnownedTimer(int iChange);																												// Exposed to Python

	inline int getCityRadiusCount() const																																		// Exposed to Python
	{
		return m_iCityRadiusCount; // advc.inl
	}
	inline bool isCityRadius() const												// Exposed to Python (K-Mod: changed to bool)
	{
		return (getCityRadiusCount() > 0); // advc.inl
	}
	void changeCityRadiusCount(int iChange);

	bool isStartingPlot() const;																																			// Exposed to Python
	void setStartingPlot(bool bNewValue);																															// Exposed to Python

	DllExport bool isNOfRiver() const;																																// Exposed to Python
	void setNOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir);											// Exposed to Python

	DllExport bool isWOfRiver() const;																																// Exposed to Python
	void setWOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir);											// Exposed to Python

	DllExport CardinalDirectionTypes getRiverNSDirection() const;																			// Exposed to Python
	DllExport CardinalDirectionTypes getRiverWEDirection() const;																			// Exposed to Python

	CvPlot* getInlandCorner() const;																																	// Exposed to Python
	bool hasCoastAtSECorner() const;

	bool isIrrigated() const { return m_bIrrigated; } // advc.inl															// Exposed to Python
	void setIrrigated(bool bNewValue);
	void updateIrrigated();

	bool isPotentialCityWork() const { return m_bPotentialCityWork; } // advc.inl											// Exposed to Python
	bool isPotentialCityWorkForArea(CvArea const& kArea) const;																												// Exposed to Python
	void updatePotentialCityWork();

	bool isShowCitySymbols() const;
	void updateShowCitySymbols();

	bool isFlagDirty() const;																																										// Exposed to Python
	void setFlagDirty(bool bNewValue);																																					// Exposed to Python

	inline TeamTypes getTeam() const																																	// Exposed to Python
	{	// <advc.opt> Now cached
		return (TeamTypes)m_eTeam;
	}
	void updateTeam(); // </advc.opt>
	PlayerTypes getOwnerExternal() const; // advc.inl: Exported through .def file																					// Exposed to Python
	inline PlayerTypes getOwner() const // advc.inl: Renamed from getOwnerINLINE
	{
		return (PlayerTypes)m_eOwner;
	}
	void setOwner(PlayerTypes eNewValue, bool bCheckUnits, bool bUpdatePlotGroup);
	/*  <advc.035> The returned player becomes the owner if war/peace changes
		between that player and the current owner */
	PlayerTypes getSecondOwner() const;
	void setSecondOwner(PlayerTypes eNewValue); // </advc.035>
	int exclusiveRadius(PlayerTypes ePlayer) const; // advc.099b
	// advc.inl: inline all plotType accessors
	inline PlotTypes getPlotType() const																																			// Exposed to Python
	{
		return (PlotTypes)m_ePlotType;
	}
	DllExport inline bool isWater() const																																								// Exposed to Python
	{
		return (getPlotType() == PLOT_OCEAN);
	}
	inline bool isFlatlands() const																																											// Exposed to Python
	{
		return (getPlotType() == PLOT_LAND);
	}
	DllExport inline bool isHills() const																																								// Exposed to Python
	{
		return (getPlotType() == PLOT_HILLS);
	}
	DllExport inline bool isPeak() const																																								// Exposed to Python
	{
		return (getPlotType() == PLOT_PEAK);
	}
	void setPlotType(PlotTypes eNewValue, bool bRecalculate = true, bool bRebuildGraphics = true);			// Exposed to Python
	// advc.inl
	DllExport inline TerrainTypes getTerrainType() const																																	// Exposed to Python
	{
		return (TerrainTypes)m_eTerrainType;
	}
	void setTerrainType(TerrainTypes eNewValue, bool bRecalculate = true, bool bRebuildGraphics = true);	// Exposed to Python
	// advc.inl
	DllExport inline FeatureTypes getFeatureType() const																																	// Exposed to Python
	{
		return (FeatureTypes)m_eFeatureType;
	}  // <advc>
	inline bool isFeature() const
	{
		return (getFeatureType() != NO_FEATURE);
	} // </advc>
	void setFeatureType(FeatureTypes eNewValue, int iVariety = -1);																				// Exposed to Python
	void setFeatureDummyVisibility(const char *dummyTag, bool show);																				// Exposed to Python
	void addFeatureDummyModel(const char *dummyTag, const char *modelTag);
	void setFeatureDummyTexture(const char *dummyTag, const char *textureTag);
	CvString pickFeatureDummyTag(int mouseX, int mouseY);
	void resetFeatureModel();

	DllExport BonusTypes getBonusType(TeamTypes eTeam = NO_TEAM) const;																							// Exposed to Python
	BonusTypes getNonObsoleteBonusType(TeamTypes eTeam = NO_TEAM, bool bCheckConnected = false) const;																	// Exposed to Python
	void setBonusType(BonusTypes eNewValue);																															// Exposed to Python

	DllExport inline ImprovementTypes getImprovementType() const																													// Exposed to Python
	{
		return (ImprovementTypes)m_eImprovementType;
	}  // <advc>
	inline bool isImproved() const
	{
		return (getImprovementType() != NO_IMPROVEMENT);
	} // </advc>
	void setImprovementType(ImprovementTypes eNewValue);																									// Exposed to Python
	// advc.inl
	inline RouteTypes getRouteType() const																																			// Exposed to Python
	{
		return (RouteTypes)m_eRouteType;
	}
	void setRouteType(RouteTypes eNewValue, bool bUpdatePlotGroup);																															// Exposed to Python
	void updateCityRoute(bool bUpdatePlotGroup);

	DllExport CvCity* getPlotCity() const;																												// Exposed to Python
	CvCityAI* AI_getPlotCity() const;
	void setPlotCity(CvCity* pNewValue);
	void setRuinsName(const CvWString& szName); // advc.005c
	const wchar* getRuinsName() const; // advc.005c
	CvCity* getWorkingCity() const;																														// Exposed to Python
	void updateWorkingCity();
	CvCity* getWorkingCityOverride() const;																								// Exposed to Python
	void setWorkingCityOverride(const CvCity* pNewValue);
	// <advc.003u>
	CvCityAI* AI_getWorkingCity() const;
	CvCityAI* AI_getWorkingCityOverrideAI() const; // </advc.003u>

	int getRiverID() const;																																							// Exposed to Python
	void setRiverID(int iNewValue);																																			// Exposed to Python

	int getMinOriginalStartDist() const;																																// Exposed to Python
	void setMinOriginalStartDist(int iNewValue);

	int getReconCount() const;																																					// Exposed to Python
	void changeReconCount(int iChange);

	int getRiverCrossingCount() const;																																	// Exposed to Python
	void changeRiverCrossingCount(int iChange);

	bool isHabitable(bool bIgnoreSea = false) const; // advc.300
	//short* getYield() { return m_aiYield; } // advc.enum: now an EnumMap
	// advc.inl
	DllExport inline int getYield(YieldTypes eIndex) const																										// Exposed to Python
	{
		return m_aiYield.get(eIndex);
	}
	int calculateNatureYield(YieldTypes eIndex, TeamTypes eTeam, bool bIgnoreFeature = false) const;		// Exposed to Python
	int calculateBestNatureYield(YieldTypes eIndex, TeamTypes eTeam) const;															// Exposed to Python
	int calculateTotalBestNatureYield(TeamTypes eTeam) const;																						// Exposed to Python
	// BETTER_BTS_AI_MOD, City AI, 10/06/09, jdog5000:
	int calculateImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer, bool bOptimal = false, bool bBestRoute = false) const;	// Exposed to Python
	// advc.enum: Return type changed to char (was int)
	char calculateYield(YieldTypes eIndex, bool bDisplay = false) const;												// Exposed to Python
	bool hasYield() const { return m_aiYield.hasContent(); } // advc.enum												// Exposed to Python
	void updateYield();
	int calculateCityPlotYieldChange(YieldTypes eYield, int iYield, int iCityPopulation) const;
	// int calculateMaxYield(YieldTypes eYield) const; // disabled by K-Mod
	int getYieldWithBuild(BuildTypes eBuild, YieldTypes eYield, bool bWithUpgrade) const;

	// advc.inl
	inline int getCulture(PlayerTypes eIndex) const { return m_aiCulture.get(eIndex); }															// Exposed to Python
	inline int getTotalCulture() const { return m_iTotalCulture; } // advc.opt
	int countFriendlyCulture(TeamTypes eTeam) const;
	TeamTypes findHighestCultureTeam() const;																														// Exposed to Python
	PlayerTypes findHighestCulturePlayer(
			bool bAlive = false) const; // advc.035
	int calculateCulturePercent(PlayerTypes eIndex) const;																		// Exposed to Python
	int calculateTeamCulturePercent(TeamTypes eIndex) const;																						// Exposed to Python
	void setCulture(PlayerTypes eIndex, int iNewValue, bool bUpdate, bool bUpdatePlotGroups);																		// Exposed to Python
	void changeCulture(PlayerTypes eIndex, int iChange, bool bUpdate);																	// Exposed to Python
// < JCultureControl Mod Start >
	PlayerTypes getImprovementOwner() const;               // Exposed to Python
	void setImprovementOwner(PlayerTypes eNewValue);               // Exposed to Python
//keldath QA - im totally not sure of this m_aiCultureControl
	inline int getCultureControl(PlayerTypes eIndex) const { return m_aiCultureControl.get(eIndex); }															// Exposed to Python
	//int getCultureControl(PlayerTypes eIndex) const;             // Exposed to Python
	int countTotalCultureControl() const;             // Exposed to Python
	PlayerTypes findHighestCultureControlPlayer() const;             // Exposed to Python

	int calculateCultureControlPercent(PlayerTypes eIndex) const;             // Exposed to Python
	int calculateTeamCultureControlPercent(TeamTypes eIndex) const;             // Exposed to Python
	void setCultureControl(PlayerTypes eIndex, int iNewValue, bool bUpdate, bool bUpdatePlotGroups);             // Exposed to Python
	void changeCultureControl(PlayerTypes eIndex, int iChange, bool bUpdate);             // Exposed to Python

	void addCultureControl(PlayerTypes ePlayer, ImprovementTypes eImprovement, bool bUpdateInterface);               // Exposed to Python
	void clearCultureControl(PlayerTypes ePlayer, ImprovementTypes eImprovement, bool bUpdateInterface);               // Exposed to Python
	void updateCultureControl(int iCenterX, int iCenterY, int iUpdateRange, bool bUpdateInterface);
	// < JCultureControl Mod End >


	int countNumAirUnits(TeamTypes eTeam) const;																					// Exposed to Python
	int airUnitSpaceAvailable(TeamTypes eTeam) const;
	// <advc.081>
	int countHostileUnits(PlayerTypes ePlayer, bool bPlayer, bool bTeam,
			bool bNeutral, bool bHostile) const; // </advc.081>
	int getFoundValue(PlayerTypes eIndex,												// Exposed to Python
			bool bRandomize = false) const; // advc.052
	bool isBestAdjacentFound(PlayerTypes eIndex);										// Exposed to Python
	// K-Mod: I've changed iNewValue to be 'short' instead of 'int', so that it matches the cache.
	void setFoundValue(PlayerTypes eIndex, short iNewValue);
	
	// advc.inl: 2x inline
	inline int getPlayerCityRadiusCount(PlayerTypes eIndex) const																							// Exposed to Python
	{
		return m_aiPlayerCityRadiusCount.get(eIndex);
	}
	inline bool isPlayerCityRadius(PlayerTypes eIndex) const																									// Exposed to Python
	{
		return (getPlayerCityRadiusCount(eIndex) > 0);
	}
	void changePlayerCityRadiusCount(PlayerTypes eIndex, int iChange);

	CvPlotGroup* getPlotGroup(PlayerTypes ePlayer) const;
	// <advc.inl> New function. Can't inline the above w/o including CvPlayer.h.
	inline bool isSamePlotGroup(CvPlot const& kOther, PlayerTypes ePlayer) const
	{
		return (m_aiPlotGroup.get(ePlayer) == kOther.m_aiPlotGroup.get(ePlayer));
	} // </advc.inl>
	CvPlotGroup* getOwnerPlotGroup() const;
	void setPlotGroup(PlayerTypes ePlayer, CvPlotGroup* pNewValue,
			bool bVerifyProduction = true); // advc.064d
	void updatePlotGroup(/* advc.064d: */ bool bVerifyProduction = false);
	void updatePlotGroup(PlayerTypes ePlayer, bool bRecalculate = true,
			bool bVerifyProduction = true); // advc.064d

	// advc.inl
	inline int getVisibilityCount(TeamTypes eTeam) const { return m_aiVisibilityCount.get(eTeam); }																				// Exposed to Python
	void changeVisibilityCount(TeamTypes eTeam, int iChange,												// Exposed to Python
			InvisibleTypes eSeeInvisible, bool bUpdatePlotGroups,
			CvUnit const* pUnit = NULL); // advc.071
	// advc.inl
	inline int getStolenVisibilityCount(TeamTypes eTeam) const																								// Exposed to Python
	{
		return m_aiStolenVisibilityCount.get(eTeam);
	}
	void changeStolenVisibilityCount(TeamTypes eTeam, int iChange);
	// advc.inl
	inline int getBlockadedCount(TeamTypes eTeam) const																								// Exposed to Python
	{
		return m_aiBlockadedCount.get(eTeam);
	}
	void changeBlockadedCount(TeamTypes eTeam, int iChange);

	DllExport PlayerTypes getRevealedOwner(TeamTypes eTeam, bool bDebug) const;													// Exposed to Python
	// <advc.inl> Faster implementation for non-UI code
	inline PlayerTypes getRevealedOwner(TeamTypes eTeam) const
	{
		return m_aiRevealedOwner.get(eTeam);
	} // </advc.inl>
	TeamTypes getRevealedTeam(TeamTypes eTeam, bool bDebug) const;														// Exposed to Python
	void setRevealedOwner(TeamTypes eTeam, PlayerTypes eNewValue);
	void updateRevealedOwner(TeamTypes eTeam);

	DllExport bool isRiverCrossing(DirectionTypes eIndex) const;																				// Exposed to Python
	void updateRiverCrossing(DirectionTypes eIndex);
	void updateRiverCrossing();

	DllExport bool isRevealed(TeamTypes eTeam, bool bDebug) const;																								// Exposed to Python
	// <advc.inl> Faster implementation for non-UI code
	inline bool isRevealed(TeamTypes eTeam) const
	{
		return m_abRevealed.get(eTeam);
	} // </advc.inl>
	void setRevealed(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly,									// Exposed to Python
			TeamTypes eFromTeam, bool bUpdatePlotGroup);
	bool isAdjacentRevealed(TeamTypes eTeam,																// Exposed to Python
			bool bSkipOcean = false) const; // advc.250c
	bool isAdjacentNonrevealed(TeamTypes eTeam) const;																				// Exposed to Python

	ImprovementTypes getRevealedImprovementType(TeamTypes eTeam, bool bDebug) const;					// Exposed to Python
	// <advc.inl> Faster implementation for non-UI code
	inline ImprovementTypes getRevealedImprovementType(TeamTypes eTeam) const
	{
		return m_aeRevealedImprovementType.get(eTeam);
	} // </advc.inl>
	void setRevealedImprovementType(TeamTypes eTeam, ImprovementTypes eNewValue);
	RouteTypes getRevealedRouteType(TeamTypes eTeam, bool bDebug) const;											// Exposed to Python
	// <advc.inl> Faster implementation for non-UI code
	inline RouteTypes getRevealedRouteType(TeamTypes eTeam) const
	{
		return m_aeRevealedRouteType.get(eTeam);
	} // </advc.inl>
	void setRevealedRouteType(TeamTypes eTeam, RouteTypes eNewValue);
	// advc.inl
	inline int getBuildProgress(BuildTypes eBuild) const																											// Exposed to Python
	{
		return m_aiBuildProgress.get(eBuild);
	}
	bool changeBuildProgress(BuildTypes eBuild, int iChange,									// Exposed to Python
			//TeamTypes eTeam = NO_TEAM
			PlayerTypes ePlayer); // advc.251
	bool decayBuildProgress(bool bTest = false); // advc.011

	void updateFeatureSymbolVisibility();
	void updateFeatureSymbol(bool bForce = false);
	/*  The plot layout contains bonuses and improvements ---
		it is, like the city layout, passively computed by LSystems */
	DllExport bool isLayoutDirty() const;
	DllExport void setLayoutDirty(bool bDirty);
	DllExport bool isLayoutStateDifferent() const;
	DllExport void setLayoutStateToCurrent();
	bool updatePlotBuilder();

	DllExport void getVisibleImprovementState(ImprovementTypes& eType, bool& bWorked);				// determines how the improvement state is shown in the engine
	DllExport void getVisibleBonusState(BonusTypes& eType, bool& bImproved, bool& bWorked);		// determines how the bonus state is shown in the engine
	bool shouldUsePlotBuilder();
	CvPlotBuilder* getPlotBuilder() { return m_pPlotBuilder; }

	DllExport CvRoute* getRouteSymbol() const;
	void updateRouteSymbol(bool bForce = false, bool bAdjacent = false);

	DllExport CvRiver* getRiverSymbol() const;
	void updateRiverSymbol(bool bForce = false, bool bAdjacent = false);
	void updateRiverSymbolArt(bool bAdjacent = true);

	CvFeature* getFeatureSymbol() const;

	DllExport CvFlagEntity* getFlagSymbol() const;
	CvFlagEntity* getFlagSymbolOffset() const;
	DllExport void updateFlagSymbol();

	DllExport CvUnit* getCenterUnit() const { return m_pCenterUnit; } // advc.inl
	DllExport CvUnit* getDebugCenterUnit() const;
	void setCenterUnit(CvUnit* pNewValue);
	// advc.enum: 2nd param was int iRangeIndex
	int getCultureRangeCities(PlayerTypes eOwnerIndex, CultureLevelTypes eRangeIndex) const														// Exposed to Python
	{
		return m_aaiCultureRangeCities.get(eOwnerIndex, eRangeIndex);
	}
	bool isCultureRangeCity(PlayerTypes eOwnerIndex, CultureLevelTypes eRangeIndex) const															// Exposed to Python
	{
		return (getCultureRangeCities(eOwnerIndex, eRangeIndex) > 0);
	}
	void changeCultureRangeCities(PlayerTypes eOwnerIndex, CultureLevelTypes eRangeIndex,
			int iChange, bool bUpdatePlotGroups);
	// advc.inl: 2x inline
	inline int getInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible) const										// Exposed to Python
	{
		return m_aaiInvisibleVisibilityCount.get(eTeam, eInvisible);
	}
	inline bool isInvisibleVisible(TeamTypes eTeam, InvisibleTypes eInvisible) const														// Exposed to Python
	{
		return (getInvisibleVisibilityCount(eTeam, eInvisible) > 0);
	}
	void changeInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible, int iChange);					// Exposed to Python

	inline int getNumUnits() const { return m_units.getLength(); } // advc.inl												// Exposed to Python
	CvUnit* getUnitByIndex(int iIndex) const;																													// Exposed to Python
	void addUnit(CvUnit const& kUnit, bool bUpdate = true);
	void removeUnit(CvUnit* pUnit, bool bUpdate = true);
	// advc.inl: 2x inline
	DllExport inline CLLNode<IDInfo>* nextUnitNode(CLLNode<IDInfo>* pNode) const
	{
		return m_units.next(pNode);
	}
	inline CLLNode<IDInfo>* prevUnitNode(CLLNode<IDInfo>* pNode) const
	{
		return m_units.prev(pNode);
	}
	// <advc.003s> Safer in 'for' loops
	inline CLLNode<IDInfo> const* nextUnitNode(CLLNode<IDInfo> const* pNode) const
	{
		return m_units.next(pNode);
	}
	inline CLLNode<IDInfo> const* prevUnitNode(CLLNode<IDInfo> const* pNode) const
	{
		return m_units.prev(pNode);
	} // </advc.003s>
	DllExport CLLNode<IDInfo>* headUnitNode() const { return m_units.head(); } // advc.inl
	CLLNode<IDInfo>* tailUnitNode() const { return m_units.tail(); } // advc.inl

	int getNumSymbols() const;
	CvSymbol* getSymbol(int iID) const;
	CvSymbol* addSymbol();

	void deleteSymbol(int iID);
	void deleteAllSymbols();

	// Script data needs to be a narrow string for pickling in Python
	CvString getScriptData() const;																											// Exposed to Python
	void setScriptData(const char* szNewValue);																					// Exposed to Python

	bool canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer) const;
	bool canApplyEvent(EventTypes eEvent) const;
	void applyEvent(EventTypes eEvent);

	bool canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible,
			bool bCheckAirUnitCap = true, // advc.001b
			BonusTypes eAssumeAvailable = NO_BONUS) const; // advc.001u
	bool isEspionageCounterSpy(TeamTypes eTeam) const;

	DllExport int getAreaIdForGreatWall() const;
	DllExport int getSoundScriptId() const;
	DllExport int get3DAudioScriptFootstepIndex(int iFootstepTag) const;
	DllExport float getAqueductSourceWeight() const;  // used to place aqueducts on the map
	DllExport bool shouldDisplayBridge(CvPlot* pToPlot, PlayerTypes ePlayer) const;
	DllExport bool checkLateEra() const;
	void killRandomUnit(PlayerTypes eOwner, DomainTypes eDomain); // advc.300

//    bool isHasValidBonus() const;  //Shqype Vicinity Bonus Add
	wchar const* debugStr() const; // advc.031c

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
	// advc.003h: Adopted from We The People mod (devolution)
	static void setMaxVisibilityRangeCache();

//MOD@VET_Andera412_Blocade_Unit-begin1/1
	bool isWithBlocaders(const CvPlot* pFromPlot, const CvPlot* pToPlot, const CvUnit* const pUnit, bool bToWater) const; // ���� �� �� ����� ��������� ��������� � ����� ������� ������ �����
	bool isBlocade(const CvPlot* pFromPlot, const CvUnit* const pUnit) const; // ����������� �� ���� ���������� ������� � ������� �����������
//MOD@VET_Andera412_Blocade_Unit-end1/1
protected:

	short m_iX;
	short m_iY;
	int m_iRiverID;
	int m_iTotalCulture; // advc.opt
	// Deliverator fresh water
	int m_iFreshWaterAmount;	// Deliverator	
	short m_iFeatureVariety;
	short m_iOwnershipDuration;
	short m_iImprovementDuration;
	short m_iUpgradeProgress;
	short m_iForceUnownedTimer;
	short m_iTurnsBuildsInterrupted; // advc.011
	// advc (note): Only the Boreal, Highlands and Rainforest map scripts use this value
	short m_iMinOriginalStartDist;
	short m_iReconCount;
	char m_iLatitude; // advc.tsl
	// advc.opt: These two were short int
	char m_iCityRadiusCount;
	char m_iRiverCrossingCount;

	bool m_bStartingPlot:1;
//keldath - why hills are out??
//	bool m_bHills:1;
//===NM=====Mountain Mod===0=====
	bool m_bPeaks:1;
//===NM=====Mountain Mod===X=====
	bool m_bNOfRiver:1;
	bool m_bWOfRiver:1;
	bool m_bIrrigated:1;
	bool m_bPotentialCityWork:1;
	bool m_bShowCitySymbols:1;
	bool m_bFlagDirty:1;
	bool m_bPlotLayoutDirty:1;
	bool m_bLayoutStateWorked:1;

	char /*PlayerTypes*/ m_eOwner;
	// < JCultureControl Mod Start >
	char /*PlayerTypes*/ m_eImprovementOwner;
	// < JCultureControl Mod End >
	char /*TeamTypes*/ m_eTeam; // advc.opt: cache the owner's team
	// advc.opt: These five were short int
	char /*PlotTypes*/ m_ePlotType;
	char /*TerrainTypes*/ m_eTerrainType;
	char /*FeatureTypes*/ m_eFeatureType;
	char /*RouteTypes*/ m_eRouteType;
	char /*ImprovementTypes*/ m_eImprovementType;
	short /*BonusTypes*/ m_eBonusType;	
	char /*CardinalDirectionTypes*/ m_eRiverNSDirection;
	char /*CardinalDirectionTypes*/ m_eRiverWEDirection;
	char /*PlayerTypes*/ m_eSecondOwner; // advc.035
	// <advc> m_pArea is enough - except while loading a savegame.
	union
	{
		CvArea* m_pArea; // This acted as a cache in BtS (was mutable)
		int m_iArea;
	}; // </advc>

	IDInfo m_plotCity;
	IDInfo m_workingCity;
	IDInfo m_workingCityOverride;

	CvWString m_szMostRecentCityName; // advc.005c
	char const* m_szScriptData; // advc: const
	// BETTER_BTS_AI_MOD, Efficiency (plot danger cache), 08/21/09, jdog5000: START
	//bool m_bActivePlayerNoDangerCache;
	mutable int m_iActivePlayerSafeRangeCache; // K-Mod (the bbai implementation was flawed)
	// <advc.enum>
	mutable EnumMap<TeamTypes,bool> m_abBorderDangerCache;
	// BETTER_BTS_AI_MOD: END  // advc: 2x mutable

	EnumMap<YieldTypes,char> m_aiYield;
	EnumMap<PlayerTypes,int> m_aiCulture;
	// < JCultureControl Mod Start >
	EnumMap<PlayerTypes,int> m_aiCultureControl;
	//keldath QA original code
	//int* m_aiCultureControl;
	// < JCultureControl Mod End >
	EnumMapDefault<PlayerTypes,int,FFreeList::INVALID_INDEX> m_aiPlotGroup;
	mutable EnumMap<PlayerTypes,short> m_aiFoundValue; // advc: mutable
	EnumMap<PlayerTypes,char> m_aiPlayerCityRadiusCount;
	EnumMap<TeamTypes,short> m_aiVisibilityCount;
	EnumMap<TeamTypes,short> m_aiStolenVisibilityCount;
	EnumMap<TeamTypes,short> m_aiBlockadedCount;
	EnumMap<TeamTypes,PlayerTypes> m_aiRevealedOwner;
	EnumMap<TeamTypes,ImprovementTypes> m_aeRevealedImprovementType;
	EnumMap<TeamTypes,RouteTypes> m_aeRevealedRouteType;
	EnumMap<TeamTypes,bool> m_abRevealed;
	EnumMap<DirectionTypes,bool> m_abRiverCrossing;
	EnumMap<BuildTypes,short> m_aiBuildProgress;
	EnumMap2D<PlayerTypes,CultureLevelTypes,char> m_aaiCultureRangeCities;
	EnumMap2D<TeamTypes,InvisibleTypes,short> m_aaiInvisibleVisibilityCount;
	// </advc.enum>
	CvFeature* m_pFeatureSymbol;
	CvRoute* m_pRouteSymbol;
	CvRiver* m_pRiverSymbol;
	CvFlagEntity* m_pFlagSymbol;
	CvFlagEntity* m_pFlagSymbolOffset;
	CvUnit* m_pCenterUnit;
	CvPlotBuilder* m_pPlotBuilder; // builds bonus resources and improvements

	CLinkList<IDInfo> m_units;
	std::vector<CvSymbol*> m_symbols;

	static bool m_bAllFog; // advc.706
	static int iMaxVisibilityRangeCache; // advc.003h

	void doFeature();
	void doCulture();

	int countTotalCulture() const; // advc.opt: Was public; replaced by getTotalCulture.
	int areaID() const;
	void processArea(CvArea& kArea, int iChange);
	char calculateLatitude() const; // advc.tsl
	void doImprovementUpgrade();
	void doCultureDecay(); // advc.099b
	ColorTypes plotMinimapColor();

	// added so under cheat mode we can access protected stuff
	friend class CvGameTextMgr;
};

#endif
