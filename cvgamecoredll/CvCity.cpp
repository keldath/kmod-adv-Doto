#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvCityAI.h"
#include "PlotRange.h"
#include "CvArea.h"
#include "CoreAI.h"
#include "CvUnit.h"
#include "CvSelectionGroup.h"
#include "CvInfo_City.h"
#include "CvInfo_Terrain.h"
#include "CvInfo_GameOption.h"
#include "CvInfo_Civics.h"
#include "CvPopupInfo.h"
#include "CvGameTextMgr.h"
#include "CvBugOptions.h" // advc.060
#include "BBAILog.h" // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
#include "CvCityMacros.h" //mylon doto version

CvCity::CvCity() // advc.003u: Merged with the deleted reset function
{
	createEntity(this); // create and attach entity to city

	m_iID = 0;
	m_iX = INVALID_PLOT_COORD;
	m_iY = INVALID_PLOT_COORD;
	m_ePlot = NO_PLOT_NUM; // advc.opt
	m_iRallyX = INVALID_PLOT_COORD;
	m_iRallyY = INVALID_PLOT_COORD;
	m_iGameTurnFounded = 0;
	m_iGameTurnAcquired = 0;
	m_iPopulation = 0;
	/* Population Limit ModComp - Beginning */
	m_iPopulationLimitChange = 0;
	/* Population Limit ModComp - End */
	m_iHighestPopulation = 0;
	m_iWorkingPopulation = 0;
	m_iSpecialistPopulation = 0;
	m_iNumGreatPeople = 0;
	m_iBaseGreatPeopleRate = 0;
	m_iGreatPeopleRateModifier = 0;
	m_iGreatPeopleProgress = 0;
	m_iNumWorldWonders = 0;
	m_iNumTeamWonders = 0;
	m_iNumNationalWonders = 0;
	m_iNumBuildings = 0;
	m_iGovernmentCenterCount = 0;
	m_iMaintenance = 0;
	m_iMaintenanceModifier = 0;
	//DPII < Maintenance Modifiers >
	m_iLocalDistanceMaintenanceModifier = 0;
	m_iLocalCoastalDistanceMaintenanceModifier  = 0;
	m_iLocalConnectedCityMaintenanceModifier  = 0;
	m_iLocalHomeAreaMaintenanceModifier  = 0;
	m_iLocalOtherAreaMaintenanceModifier  = 0;
	//DPII < Maintenance Modifiers >
	m_iWarWearinessModifier = 0;
	m_iHurryAngerModifier = 0;
	m_iHealRate = 0;
	m_iEspionageHealthCounter = 0;
	m_iEspionageHappinessCounter = 0;
	m_iFreshWaterGoodHealth = 0;
	m_iFreshWaterBadHealth = 0;
	m_iSurroundingGoodHealth = 0;
	m_iSurroundingBadHealth = 0;
/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: Enable Terrain Health Modifiers                                                  **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
	m_iTerrainGoodHealth = 0;
	m_iTerrainBadHealth = 0;
/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**                                                                                              */
/*************************************************************************************************/	
	m_iSpecialistGoodHealth = 0;
	m_iSpecialistBadHealth = 0;
	m_iSpecialistHappiness = 0;
	m_iSpecialistUnhappiness = 0;
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/
	m_iBuildingGoodHealth = 0;
	m_iBuildingBadHealth = 0;
	m_iPowerGoodHealth = 0;
	m_iPowerBadHealth = 0;
	m_iBonusGoodHealth = 0;
	m_iBonusBadHealth = 0;
	m_iHurryAngerTimer = 0;
	m_iConscriptAngerTimer = 0;
	m_iDefyResolutionAngerTimer = 0;
	m_iHappinessTimer = 0;
	m_iMilitaryHappinessUnits = 0;
	m_iBuildingGoodHappiness = 0;
	m_iBuildingBadHappiness = 0;
	m_iExtraBuildingGoodHappiness = 0;
	m_iExtraBuildingBadHappiness = 0;
	m_iExtraBuildingGoodHealth = 0;
	m_iExtraBuildingBadHealth = 0;
	m_iSurroundingGoodHappiness = 0;
	m_iSurroundingBadHappiness = 0;
	m_iBonusGoodHappiness = 0;
	m_iBonusBadHappiness = 0;
	m_iReligionGoodHappiness = 0;
	m_iReligionBadHappiness = 0;
	// < Civic Infos Plus Start >
	m_iReligionGoodHealth = 0;
	m_iReligionBadHealth = 0;
	// < Civic Infos Plus End   >
	m_iExtraHappiness = 0;
	m_iExtraHealth = 0;
	m_iNoUnhappinessCount = 0;
	m_iUnhealthyPopulationModifier = 0; // K-Mod
	m_iBuildingOnlyHealthyCount = 0;
	m_iFood = 0;
	m_iFoodKept = 0;
	m_iMaxFoodKeptPercent = 0;
	m_iOverflowProduction = 0;
	m_iFeatureProduction = 0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iExtraTradeRoutes = 0;
	m_iTradeRouteModifier = 0;
	m_iForeignTradeRouteModifier = 0;
	m_iBuildingDefense = 0;
	m_iBuildingBombardDefense = 0;
	m_iFreeExperience = 0;
	m_iCurrAirlift = 0;
	m_iMaxAirlift = 0;
	m_iAirModifier = 0;
	m_iAirUnitCapacity = 0;
	m_iNukeModifier = 0;
	m_iFreeSpecialist = 0;
	m_iPowerCount = 0;
	m_iDirtyPowerCount = 0;
	m_iDefenseDamage = 0;
	m_iLastDefenseDamage = 0;
	m_iOccupationTimer = 0;
	m_iCultureUpdateTimer = 0;
	m_iCitySizeBoost = 0;
	m_iSpecialistFreeExperience = 0;
	m_iEspionageDefenseModifier = 0;
	m_iPopRushHurryCount = 0; // advc.912d
	m_iMostRecentOrder = -1; // advc.004x

//doto enhanced city size mylon
	m_iRadius = -1;
	m_eCityPlots = NO_CITYPLOT;

	m_bNeverLost = true;
	m_bBombarded = false;
	m_bDrafted = false;
	m_bAirliftTargeted = false;
	m_bWeLoveTheKingDay = false;
	m_bCitizensAutomated = true;
	m_bProductionAutomated = false;
	m_bWallOverride = false;
	m_bInfoDirty = true;
	m_bLayoutDirty = false;
	m_bPlundered = false;
	m_bInvestigate = false; // advc.103
	m_bMostRecentUnit = false; // advc.004x
	m_bChooseProductionDirty = false;

	m_eOwner = NO_PLAYER;
	m_ePreviousOwner = NO_PLAYER;
	m_eOriginalOwner = NO_PLAYER;
	m_eCultureLevel = NO_CULTURELEVEL;

//	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
//	{
		// < Civic Infos Plus Start >
		// no need for these f1 advc
		//m_aiBuildingYieldChange[iI] = 0;
		//m_aiStateReligionYieldRateModifier[iI] = 0;
		//m_aiNonStateReligionYieldRateModifier[iI] = 0;
		// < Civic Infos Plus End   >
//	}
//	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
//	{
		// < Civic Infos Plus Start >
		//no need for these f1 advc
		//m_aiStateReligionCommerceRateModifier[iI] = 0;
		//m_aiNonStateReligionCommerceRateModifier[iI] = 0;
		//m_aiBuildingCommerceChange[iI] = 0;
		// < Civic Infos Plus End   >
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**																								**/
	/**																								**/
	/*************************************************************************************************/
//		m_aiExtraSpecialistCommerce[iI] = 0;
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/
		
//	}
	m_aTradeCities.resize(GC.getDefineINT(CvGlobals::MAX_TRADE_ROUTES));

	// Rank cache
	m_iPopulationRank = -1;
	m_bPopulationRankValid = false;
}

CvCity::~CvCity() // advc.003u: Merged with the deleted uninit function
{
	removeEntity(); // remove entity from engine
	destroyEntity(); // delete CvCityEntity and detach from us

	m_orderQueue.clear();
}


void CvCity::init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits,
	bool bUpdatePlotGroups, /* advc.ctr: */ int iOccupationTimer)
{
	//reset(iID, eOwner, kPlot.getX(), kPlot.getY());
	// <advc.003u> Reset merged into constructor
	m_iID = iID;
	m_eOwner = eOwner;
	m_eOriginalOwner = eOwner;
//doto enhanced city size mylon
//not using these anymore
//	CivilizationTypes eCiv = GET_PLAYER(m_eOwner).getCivilizationType();
//	int diam = GC.getCivilizationInfo(eCiv).getMaxCityRadius();
	// will be handled by updateCultureLevel
	//updateRadius();
	//m_abWorkingPlot.resize(NUM_CITY_PLOTS);
//doto enhanced city size mylon
	m_iX = iX;
	m_iY = iY;
	// </advc.003u>
	updatePlot(); // advc.opt
	setupGraphical();

	CvPlayer& kOwner = GET_PLAYER(getOwner());
	setName(kOwner.getNewCityName(), /* advc.106k: */ false, true);

	setEverOwned(getOwner(), true);
	/*  advc.ctr: To prevent updateCultureLevel from bumping units in
		surrounding tiles after trading a city under occupation. Don't call
		setOccupationTimer though -- don't need all those updates. */
	m_iOccupationTimer = iOccupationTimer;
	// <advc.908b>
	if (bUpdatePlotGroups) // new city (not acquired from someone else)
		initTraitCulture(); // </advc.908b>
	updateCultureLevel(false);

	CvPlot& kPlot = GC.getMap().getPlot(iX, iY);
	{
		int const iFreeCityPlotCulture = GC.getDefineINT("FREE_CITY_CULTURE");
		if (kPlot.getCulture(getOwner()) < iFreeCityPlotCulture)
			kPlot.setCulture(getOwner(), iFreeCityPlotCulture, bBumpUnits, false);
	}
	kPlot.setOwner(getOwner(), bBumpUnits, false);
	kPlot.setPlotCity(this);

	int const iFreeCityAdjacentCulture = GC.getDefineINT("FREE_CITY_ADJACENT_CULTURE");
	FOR_EACH_ADJ_PLOT_VAR(getPlot())
	{
		if (pAdj->getCulture(getOwner()) < iFreeCityAdjacentCulture)
		{
			pAdj->setCulture(getOwner(), iFreeCityAdjacentCulture,
					// advc.opt: Updated in the next line in any case
					false/*was bBumpUnits*/, false);
		}
		pAdj->updateCulture(bBumpUnits, false);
	}
	if (GET_TEAM(getTeam()).isAVassal()) // advc (replacing a loop over "all" masters)
	{
		kPlot.changeAdjacentSight(GET_TEAM(getTeam()).getMasterTeam(),
				GC.getDefineINT(CvGlobals::PLOT_VISIBILITY_RANGE), true, NULL, false);
	}
	if (GC.getPythonCaller()->isCitiesDestroyFeatures(iX, iY) && kPlot.isFeature())
		kPlot.setFeatureType(NO_FEATURE);

	kPlot.setImprovementType(NO_IMPROVEMENT);
	kPlot.updateCityRoute(false);

	for (TeamIter<ALIVE> it; it.hasNext(); ++it)
	{
		if (kPlot.isVisible(it->getID()))
			setRevealed(it->getID(), true);
	}

	changeMilitaryHappinessUnits(kPlot.plotCount(PUF_isMilitaryHappiness));

	FOR_EACH_ENUM(Commerce)
	{
		changeCommerceHappinessPer(eLoopCommerce,
				GC.getInfo(eLoopCommerce).getInitialHappiness());
	}

	getArea().changeCitiesPerPlayer(getOwner(), 1);
	// <advc.030b>
	{
		CvArea* pWaterArea = waterArea(true);
		if(pWaterArea != NULL)
			pWaterArea->changeCitiesPerPlayer(getOwner(), 1);
	} // </advc.030b>

	CvGame& kGame = GC.getGame(); // advc
	GET_TEAM(getTeam()).changeNumCities(1);
	kGame.changeNumCities(1);
	setGameTurnFounded(kGame.getGameTurn());
	setGameTurnAcquired(kGame.getGameTurn());

	changePopulation(/* advc.004b: */ initialPopulation()); // (Moved into subroutine)

	changeAirUnitCapacity(GC.getDefineINT(CvGlobals::CITY_AIR_UNIT_CAPACITY));

	updateFreshWaterHealth();
	updateSurroundingHealthHappiness();
	updatePowerHealth();

	kOwner.updateMaintenance();

	GC.getMap().updateWorkingCity();
	kGame.AI_makeAssignWorkDirty();
	//kOwner.setFoundedFirstCity(true); // advc.104: Moved to CvPlayer::initCity

	// advc: Moved down
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		if (kOwner.isBuildingFree(kCiv.buildingAt(i)))
			setNumFreeBuilding(kCiv.buildingAt(i), 1);
	}
	if (kGame.isFinalInitialized() && kOwner.getNumCities() == 1)
	{
		for (int i = 0; i < kCiv.getNumBuildings(); i++)
		{
			BuildingTypes eBuilding = kCiv.buildingAt(i);
			if (kCiv.isFreeBuilding(eBuilding))
				setNumRealBuilding(eBuilding, 1);
		}
		if (!isHuman() /* advc.250b: */ && !kGame.isOption(GAMEOPTION_SPAH))
			changeOverflowProduction(GC.getDefineINT("INITIAL_AI_CITY_PRODUCTION"), 0);
		// <advc.124g>
		if (isHuman() && isActiveOwned() &&
			kOwner.getCurrentResearch() == NO_TECH)
		{
			kOwner.chooseTech();
		} // </advc.124g>
	}

	updateEspionageVisibility(false);

	if (bUpdatePlotGroups)
		kGame.updatePlotGroups();

	GC.getLogger().logCityBuilt(*this); // advc.003t: Moved down so that just a single param needs to be passed
}

// graphical only setup
void CvCity::setupGraphical()
{
	if (!GC.IsGraphicsInitialized())
		return;

	CvDLLEntity::setup();

	setInfoDirty(true);
	setLayoutDirty(true);
}

// advc.095: Based on CvUnit::reloadEntity
void CvCity::reloadEntity()
{
	FAssert(GC.IsGraphicsInitialized());
	removeEntity();
	destroyEntity();
	createEntity(this);
	setupGraphical();
}


void CvCity::kill(bool bUpdatePlotGroups, /* advc.001: */ bool bBumpUnits)
{
	CvPlot& kPlot = *plot();

	if (isCitySelected())
		gDLL->UI().clearSelectedCities();

	for (CityPlotIter it(*this); it.hasNext(); ++it)
	{
		if (it->getWorkingCityOverride() == this)
			it->setWorkingCityOverride(NULL);
	}

	setCultureLevel(NO_CULTURELEVEL, false);

	FOR_EACH_ENUM(Building)
	{
		setNumRealBuilding(eLoopBuilding, 0);
		setNumFreeBuilding(eLoopBuilding, 0);
	}
	FOR_EACH_ENUM(Specialist)
		setFreeSpecialistCount(eLoopSpecialist, 0);
	FOR_EACH_ENUM(Yield)
	{
		setTradeYield(eLoopYield, 0);
		setCorporationYield(eLoopYield, 0);
	}
	FOR_EACH_ENUM(Religion)
	{
		setHasReligion(eLoopReligion, false, false, true);
		if (isHolyCity(eLoopReligion))
			GC.getGame().setHolyCity(eLoopReligion, NULL, false);
	}
	FOR_EACH_ENUM(Corporation)
	{
		setHasCorporation(eLoopCorporation, false, false);
		if (isHeadquarters(eLoopCorporation))
			GC.getGame().setHeadquarters(eLoopCorporation, NULL, false);
	}

	setPopulation(0);
	AI().AI_assignWorkingPlots();
	clearOrderQueue();

	// remember the visibility before we take away the city from the plot below
	std::vector<bool> abEspionageVisibility(TeamIter<>::count(), false);
	for (TeamIter<CIV_ALIVE> it; it.hasNext(); ++it)
		abEspionageVisibility[it->getID()] = getEspionageVisibility(it->getID());
	/*  UNOFFICIAL_PATCH, Bugfix, 08/04/09, jdog5000:
		Need to clear trade routes of dead city, else they'll be claimed for the owner forever. */
	clearTradeRoutes();

	kPlot.setPlotCity(NULL);
	kPlot.setRuinsName(getName()); // advc.005c

	// UNOFFICIAL_PATCH, replace floodplains after city is removed, 03/04/10, jdog5000: START
	if (kPlot.getBonusType() == NO_BONUS &&
		GC.getDefineBOOL("FLOODPLAIN_AFTER_RAZE")) // advc.129b
	{
		FOR_EACH_ENUM(Feature)
		{
			if (GC.getInfo(eLoopFeature).isRequiresRiver() &&
				kPlot.canHaveFeature(eLoopFeature) &&
				GC.getInfo(eLoopFeature).getAppearanceProbability() == 10000)
			{
				kPlot.setFeatureType(eLoopFeature);
				break;
			}
		}
	} // UNOFFICIAL_PATCH: END
	// <advc.104>
	for (PlayerAIIter<MAJOR_CIV> it; it.hasNext(); ++it)
		it->AI_cityKilled(*this); // </advc.104>
	getArea().changeCitiesPerPlayer(getOwner(), -1);
	// <advc.030b>
	CvArea* pWaterArea = waterArea(true);
	/*  Can't really handle ice melted by global warming, but at least ensure
		that CitiesPerPlayer doesn't become negative. */
	if(pWaterArea != NULL && pWaterArea->getCitiesPerPlayer(getOwner(), true) > 0)
		pWaterArea->changeCitiesPerPlayer(getOwner(), -1); // </advc.030b>
	GET_TEAM(getTeam()).changeNumCities(-1);

	GC.getGame().changeNumCities(-1);

	FAssert(getWorkingPopulation() == 0);
	FAssert(!isWorkingPlot(CITY_HOME_PLOT));
	FAssert(getSpecialistPopulation() == 0);
	FAssert(getNumGreatPeople() == 0);
	FAssert(getBaseYieldRate(YIELD_FOOD) == 0);
	FAssert(getBaseYieldRate(YIELD_PRODUCTION) == 0);
	FAssert(getBaseYieldRate(YIELD_COMMERCE) == 0);
	FAssert(!isProduction());

	PlayerTypes const eOwner = getOwner();
	bool const bCapital = isCapital();
	// <advc.106> Moved up so that the old capital can be announced
	if (bCapital)
		GET_PLAYER(eOwner).findNewCapital(); // </advc.106>
	kPlot.setImprovementType(GC.getRUINS_IMPROVEMENT());
	// < JCultureControl Mod Start >
	if (GC.getGame().isOption(GAMEOPTION_CULTURE_CONTROL))
	{
		kPlot.setImprovementOwner(getOriginalOwner());
		//keldathQA-DONE
		kPlot.addCultureControl(getOriginalOwner(), (ImprovementTypes) GC.getRUINS_IMPROVEMENT(), true);
	}
	// < JCultureControl Mod End >
	CvEventReporter::getInstance().cityLost(this);
	GET_PLAYER(getOwner()).deleteCity(getID());

	kPlot.updateCulture(/*true*/ bBumpUnits, false); // advc.001
	/*	advc (note): setCultureLevel already updates culture in surrounding plots.
		I think this loop is only needed for rare cases where kPlot is part of a
		circle of plots granting ownership over a plot that is otherwise
		out of ownership range (see the end of CvPlot::calculateCulturalOwner). */
	FOR_EACH_ADJ_PLOT_VAR(kPlot)
		pAdj->updateCulture(true, false);

	if (GET_TEAM(eOwner).isAVassal()) // advc: Replacing a loop over "all" masters
	{
		kPlot.changeAdjacentSight(GET_TEAM(eOwner).getMasterTeam(),
				GC.getDefineINT(CvGlobals::PLOT_VISIBILITY_RANGE), false, NULL, false);
	}
	for (TeamIter<CIV_ALIVE> it; it.hasNext(); ++it)
	{
		if (abEspionageVisibility[it->getID()])
		{
			kPlot.changeAdjacentSight(it->getID(), GC.getDefineINT(
					CvGlobals::PLOT_VISIBILITY_RANGE), false, NULL, false);
		}
	}

	GET_PLAYER(eOwner).updateMaintenance();
	GC.getMap().updateWorkingCity();
	GC.getGame().AI_makeAssignWorkDirty();

	if (bCapital)
	{
		//GET_PLAYER(eOwner).findNewCapital(); // advc.106: Moved up
		GET_TEAM(eOwner).resetVictoryProgress();
	}

	if (bUpdatePlotGroups)
		GC.getGame().updatePlotGroups();

	if (isActiveOwned())
		gDLL->UI().setDirty(SelectionButtons_DIRTY_BIT, true);
}


void CvCity::doTurn()
{
	PROFILE("CvCity::doTurn()");

	if (!isBombarded())
		changeDefenseDamage(-GC.getDefineINT(CvGlobals::CITY_DEFENSE_DAMAGE_HEAL_RATE));
	setLastDefenseDamage(getDefenseDamage());
	setBombarded(false);
	setPlundered(false);
	setDrafted(false);
	setAirliftTargeted(false);
	setCurrAirlift(0);
	m_bInvestigate = false; // advc.103

	AI().AI_doTurn();

	CvPlayer const& kOwner = GET_PLAYER(getOwner());

	// <advc.106k>
	if(!m_szPreviousName.empty() && m_szName.compare(m_szPreviousName) != 0)
	{
		FAssert(isHuman());
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, kOwner.getID(),
				gDLL->getText("TXT_KEY_MISC_CITY_RENAMED",
				m_szPreviousName.GetCString(), m_szName.GetCString()),
				getX(), getY(), kOwner.getPlayerTextColor());
		m_szPreviousName.clear();
	} // </advc.106k>
	bool const bForceProduction = true; // advc.064d
	/*bForceProduction =*/ doCheckProduction();

	doGrowth();
	doCulture();
	//doPlotCulture(false, kOwner.getID(), getCommerceRate(COMMERCE_CULTURE));
	// <K-Mod>
	doPlotCultureTimes100(false, kOwner.getID(),
			getCommerceRateTimes100(COMMERCE_CULTURE), true); // </K-Mod>
	doProduction(!bForceProduction);
	doDecay();
	doReligion();
	doGreatPeople();
	doMeltdown();

	updateEspionageVisibility(true);

	if (!isDisorder())
	{
		for (WorkablePlotIter it(*this); it.hasNext(); ++it)
		{
			if (it->isBeingWorked())
				it->doImprovement();
		}
	}  // <advc.004x>
	else
	{
		if(isHuman() && !isProduction() && !isProductionAutomated() &&
			kOwner.getAnarchyTurns() == 1 &&
			GC.getGame().getGameState() != GAMESTATE_EXTENDED)
		{
			UnitTypes eMostRecentUnit = NO_UNIT;
			BuildingTypes eMostRecentBuilding = NO_BUILDING;
			ProjectTypes eMostRecentProject = NO_PROJECT;
			if(m_iMostRecentOrder >= 0)
			{
				if(m_bMostRecentUnit)
					eMostRecentUnit = (UnitTypes)m_iMostRecentOrder;
				else if(m_iMostRecentOrder >= GC.getNumBuildingInfos())
				{
					eMostRecentProject = (ProjectTypes)
							(m_iMostRecentOrder - GC.getNumBuildingInfos());
				}
				else eMostRecentBuilding = (BuildingTypes)m_iMostRecentOrder;
			}
			chooseProduction(eMostRecentUnit, eMostRecentBuilding,
					eMostRecentProject, m_iMostRecentOrder >= 0);
		}
	} // </advc.004x>
// < Building Resource Converter Start >
	//frpo1 added a check to avoid infinite loop
	//keldath
	//if (bUpdateBuildings)
    //	processBuildingBonuses();
	//processBuildingBonuses(); // f1rpo: Shouldn't be needed. Could fall back on this though if the other calls turn out to be too expensive.
	// < Building Resource Converter End   >
	
	if (getCultureUpdateTimer() > 0)
		changeCultureUpdateTimer(-1);

	/*  advc.023: Now handled in doRevolt (which is called at the start of a round,
		not the end of a player turn) */
	/*if (getOccupationTimer() > 0)
		changeOccupationTimer(-1);*/

	if (getHurryAngerTimer() > 0)
		changeHurryAngerTimer(-1);

	if (getConscriptAngerTimer() > 0)
		changeConscriptAngerTimer(-1);

	if (getDefyResolutionAngerTimer() > 0)
		changeDefyResolutionAngerTimer(-1);

	if (getHappinessTimer() > 0)
		changeHappinessTimer(-1);

	if (getEspionageHealthCounter() > 0)
		changeEspionageHealthCounter(-1);

	if (getEspionageHappinessCounter() > 0)
		changeEspionageHappinessCounter(-1);

	updateSurroundingHealthHappiness(); // advc.901

	if (isOccupation() || angryPopulation() > 0 || healthRate() < 0)
		setWeLoveTheKingDay(false);
	else if (getPopulation() >= GC.getDefineINT("WE_LOVE_THE_KING_POPULATION_MIN_POPULATION") &&
		SyncRandNum(GC.getDefineINT("WE_LOVE_THE_KING_RAND")) < getPopulation())
	{
		setWeLoveTheKingDay(true);
	}
	else setWeLoveTheKingDay(false);

	CvEventReporter::getInstance().cityDoTurn(this, kOwner.getID());

	#ifdef _DEBUG // XXX
	FOR_EACH_ENUM(Yield)
	{	// <advc> Want to see the value in the debugger
		int iBaseYieldRate = getBaseYieldRate(eLoopYield);
		FAssert(iBaseYieldRate >= 0);
		FAssert(getYieldRate(eLoopYield) >= 0);
		// advc.104u: Code moved into auxiliary function
		int iCount = calculateBaseYieldRate(eLoopYield);
		FAssert(iCount == iBaseYieldRate); // </advc>
	}
	FOR_EACH_ENUM(Commerce)
	{
		FAssert(getBuildingCommerce(eLoopCommerce) >= 0);
		FAssert(getSpecialistCommerce(eLoopCommerce) >= 0);
		FAssert(getReligionCommerce(eLoopCommerce) >= 0);
		FAssert(getCorporationCommerce(eLoopCommerce) >= 0);
		FAssert(kOwner.getFreeCityCommerce(eLoopCommerce) >= 0);
	}
	FOR_EACH_ENUM(Bonus)
	{
		FAssert(isNoBonus(eLoopBonus) ||
				getNumBonuses(eLoopBonus) >= (!isConnectedToCapital() ? 0 :
				kOwner.getBonusImport(eLoopBonus) - kOwner.getBonusExport(eLoopBonus)));
	}
	#endif // XXX
}

// advc.104u: Cut, pasted, refactored from the end of CvCity::doTurn
int CvCity::calculateBaseYieldRate(YieldTypes eYield)
{
	int iR = 0;
	for (WorkingPlotIter it(*this); it.hasNext(); ++it)
	{
		CvPlot const* pPlot = getCityIndexPlot(it.currID());
		if(pPlot != NULL)
			iR += pPlot->getYield(eYield);
	}
	FOR_EACH_ENUM(Specialist)
	{
		iR += GET_PLAYER(getOwner()).specialistYield(eLoopSpecialist, eYield) *
				(getSpecialistCount(eLoopSpecialist) +
				getFreeSpecialistCount(eLoopSpecialist));
	}
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes eBuilding = kCiv.buildingAt(i);
		CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
		iR += getNumActiveBuilding(eBuilding) * (kBuilding.getYieldChange(eYield) +
				getBuildingYieldChange(kBuilding.getBuildingClassType(), eYield));
	}
	iR += getTradeYield(eYield);
	iR += getCorporationYield(eYield);
	return iR;
}

// advc: Code cut and pasted from CvPlot::doCulture; also refactored.
void CvCity::doRevolt()
{
	PROFILE_FUNC();
	// <advc.023>
	{
		scaled rDecrementProb = probabilityOccupationDecrement();
		if (SyncRandSuccess(rDecrementProb))
		{
			changeOccupationTimer(-1);
			return;
		}
	} // </advc.023>  <advc.099c>
	PlayerTypes eCulturalOwner = getPlot().calculateCulturalOwner();
	PlayerTypes eOwnerIgnRange = eCulturalOwner;
	if (GC.getDefineBOOL(CvGlobals::REVOLTS_IGNORE_CULTURE_RANGE))
		eOwnerIgnRange = getPlot().calculateCulturalOwner(true);
	// If not within culture range, can revolt but not flip.
	bool bCanFlip = (eOwnerIgnRange == eCulturalOwner);
	eCulturalOwner = eOwnerIgnRange;
	// </advc.099c>
	/*  <advc.101> To avoid duplicate code in CvDLLWidgetData::parseNationalityHelp,
		compute the revolt probability in a separate function. */
	{
		scaled rRevoltProb = revoltProbability();
		if (!SyncRandSuccess(rRevoltProb))
			return;
	} // </advc.101>
	damageGarrison(eCulturalOwner); // advc: Code moved into subroutine
	if (bCanFlip /* advc.099 */ && canCultureFlip(eCulturalOwner))
	{
		if (GET_PLAYER(eCulturalOwner).isOneCityChallenge())
			kill(true);
		else getPlot().setOwner(eCulturalOwner, true, true); // will delete pCity
		/*  advc (comment): setOwner doesn't actually delete this object; just
			calls CvCity::kill. Messages also handled by setOwner (through
			CvPlayer::acquireCity). */
		return;
	}
	/*	K-Mod, 11/jan/11: Changed number of revolt turns to not depend on iCityStrength,
		because iCityStrength can be huge. */
	/*pCity->changeOccupationTimer(GC.getDefineINT("BASE_REVOLT_OCCUPATION_TURNS")
	+ ((iCityStrength * GC.getDefineINT("REVOLT_OCCUPATION_TURNS_PERCENT")) /
	100));*/ // BtS
	// <advc.023>
	/*  Removed factor 2 from the second summand. K-Mod 1.45 changelog said:
		'Decreased the duration of revolts from
		 3+2*(previous revolts) to 2+(previous revolts). (...)
		 The net effect of these changes is that (...) the number of turns spent
		 in revolt will be similar to before (...).'
		This is just what I need now that the occupation timer decreases
		probabilistically, but it wasn't committed to the K-Mod repository;
		so I'm adding it here. */
	int iTurnsOccupation = GC.getDefineINT("BASE_REVOLT_OCCUPATION_TURNS") +
			getNumRevolts(eCulturalOwner); // </advc.023>
	// K-Mod end
	changeNumRevolts(eCulturalOwner, 1);
	if (!isOccupation()) // advc.023: Don't prolong revolt
		changeOccupationTimer(iTurnsOccupation);
	CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_REVOLT_IN_CITY",
			GET_PLAYER(eCulturalOwner).getCivilizationAdjective(),
			getNameKey());
	// <advc.023>
	/*  Population loss if pCity should flip, but can't. We know at this point
		that the current revolt doesn't flip the city; but can it ever flip? */
	if ((!bCanFlip || !canCultureFlip(eCulturalOwner, false)) &&
		getNumRevolts(eCulturalOwner) > GC.getDefineINT(CvGlobals::NUM_WARNING_REVOLTS) &&
		getPopulation() > 1)
	{
		changePopulation(-1);
		// Says "Citizens have been killed"
		szBuffer.append(L" " + gDLL->getText("TXT_KEY_EVENT_HURRICANE_2"));
	} // </advc.023>
	/*  <advc.101> BtS comment: "XXX announce for all seen cities?"
		I guess that's better. Mustn't startle third parties with color
		and sound though. */
	for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
	{
		CvPlayer const& kObs = *it;
		bool bAffected = (kObs.getID() == eCulturalOwner ||
				kObs.getID() == getOwner());
		if (!bAffected && !isRevealed(kObs.getTeam()))
			continue;
		InterfaceMessageTypes eMsg = MESSAGE_TYPE_INFO;
		LPCTSTR szSound = NULL;
		// Color of both the text and the flashing icon
		ColorTypes eColor = NO_COLOR;
		if (bAffected)
		{
			eMsg = MESSAGE_TYPE_MINOR_EVENT;
			szSound = "AS2D_CITY_REVOLT";
			eColor = GC.getColorType(kObs.getID() == getOwner() ? "RED" : "GREEN");
		}
		gDLL->UI().addMessage(kObs.getID(), false, -1, szBuffer, getPlot(),
				szSound, eMsg, ARTFILEMGR.getInterfaceArtPath("INTERFACE_RESISTANCE"), eColor);
	} // </advc.101>
}

// advc: Cut from CvPlot::doCulture; simplified.
void CvCity::damageGarrison(PlayerTypes eRevoltSource)
{
	FOR_EACH_UNIT_VAR_IN(pUnit, getPlot())
	{
		/*	advc.101: OK in BtS b/c Barbarian cities would flip on the first revolt.
			Now killed only when the city flips through CvPlot::setOwner. */
		/*if (pUnit->isBarbarian())
			pUnit->kill(false, eRevoltSource);
		else*/
		if(pUnit->canDefend())
			pUnit->changeDamage(pUnit->currHitPoints() / 2, eRevoltSource);
	}
}

bool CvCity::isCitySelected()
{
	return gDLL->UI().isCitySelected(this);
}


bool CvCity::canBeSelected() const
{
	CvGame const& kGame = GC.getGame();

	if (m_bInvestigate || // advc.103
		isActiveTeam() || kGame.isDebugMode())
	{
		return true;
	}
	TeamTypes const eActiveTeam = kGame.getActiveTeam();
	if (eActiveTeam != NO_TEAM && getPlot().isInvestigate(eActiveTeam))
		return true;

	FOR_EACH_ENUM(EspionageMission)
	{
		if (GC.getInfo(eLoopEspionageMission).isPassive() &&
			GC.getInfo(eLoopEspionageMission).isInvestigateCity() &&
			GET_PLAYER(kGame.getActivePlayer()).canDoEspionageMission(
			eLoopEspionageMission, getOwner(), plot()))
		{
			return true;
		}
	}
	return false;
}


void CvCity::updateSelectedCity(bool bTestProduction)
{
	for (CityPlotIter it(*this); it.hasNext(); ++it)
	{
		it->updateShowCitySymbols();
	}
	if (bTestProduction)
	{
		if (isActiveOwned() && !isProduction() &&
			!isProductionAutomated()) // K-Mod
		{
			chooseProduction(NO_UNIT, NO_BUILDING, NO_PROJECT, false, true);
		}
	}
}

// advc.103:
void CvCity::setInvestigate(bool b)
{
	m_bInvestigate = b;
}


void CvCity::updateYield()
{
	for (CityPlotIter it(*this); it.hasNext(); ++it)
	{
		it->updateYield();
	}
}

// XXX kill this?
void CvCity::updateVisibility()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
		return;

	FAssert(GC.getGame().getActiveTeam() != NO_TEAM);

	CvDLLEntity::setVisible(isRevealed(GC.getGame().getActiveTeam(), true));
}


void CvCity::createGreatPeople(UnitTypes eGreatPersonUnit, bool bIncrementThreshold,
	bool bIncrementExperience) /* advc: */ const
{
	GET_PLAYER(getOwner()).createGreatPeople(eGreatPersonUnit, bIncrementThreshold,
			bIncrementExperience, getPlot());
}


void CvCity::doTask(TaskTypes eTask, int iData1, int iData2, bool bOption,
	bool bAlt, bool bShift, bool bCtrl)
{
	bool bCede = false; // advc.ctr
	switch (eTask)
	{
	case TASK_RAZE:
		GET_PLAYER(getOwner()).raze(*this);
		break;

	case TASK_DISBAND:
		GET_PLAYER(getOwner()).disband(*this);
		break;
	// <advc.ctr>
	case TASK_CEDE: // Meaning (in this context): as part of a peace deal
		bCede = true;
		// fall through // </advc.ctr>
	case TASK_GIFT:
		if (getLiberationPlayer() == iData1)
			liberate(false, /* advc.ctr: */ bCede);
		else
		{
			GET_PLAYER((PlayerTypes)iData1).acquireCity(this, false, true, true,
					bCede, bCede); // advc.ctr
		}
		break;

	case TASK_LIBERATE:
		liberate(iData1 != 0);
		break;

	case TASK_SET_AUTOMATED_CITIZENS:
		setCitizensAutomated(bOption);
		break;

	case TASK_SET_AUTOMATED_PRODUCTION:
		setProductionAutomated(bOption, bAlt && bShift && bCtrl);
		break;

	case TASK_SET_EMPHASIZE:
		AI().AI_setEmphasize((EmphasizeTypes)iData1, bOption);
		break;

	case TASK_CHANGE_SPECIALIST:
		alterSpecialistCount(((SpecialistTypes)iData1), iData2);
		break;

	case TASK_CHANGE_WORKING_PLOT:
		alterWorkingPlot((CityPlotTypes)iData1);
		break;

	case TASK_CLEAR_WORKING_OVERRIDE:
		clearWorkingOverride((CityPlotTypes)iData1);
		break;

	case TASK_HURRY:
		hurry((HurryTypes)iData1);
		break;

	case TASK_CONSCRIPT:
		conscript();
		break;

	case TASK_CLEAR_ORDERS:
		clearOrderQueue();
		break;

	case TASK_RALLY_PLOT:
		setRallyPlot(GC.getMap().plot(iData1, iData2));
		break;

	case TASK_CLEAR_RALLY_PLOT:
		setRallyPlot(NULL);
		break;

	default:
		FErrorMsg("eTask failed to match a valid option");
	}
}


void CvCity::chooseProduction(UnitTypes eTrainUnit, BuildingTypes eConstructBuilding,
	ProjectTypes eCreateProject, bool bFinish, bool bFront)
{
	// K-Mod. don't create the popup if the city is in disorder
	FAssert(isHuman() && !isProductionAutomated());
	if (isOccupation()) // advc.004x: was isDisorder
	{
		setChooseProductionDirty(true);
		return;
	}
	setChooseProductionDirty(false);
	// K-Mod end
	CvPopupInfo* pPopupInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSEPRODUCTION);
	pPopupInfo->setData1(getID());
	pPopupInfo->setOption1(bFinish);

	if (eTrainUnit != NO_UNIT)
	{
		pPopupInfo->setData2(ORDER_TRAIN);
		pPopupInfo->setData3(eTrainUnit);
	}
	else if (eConstructBuilding != NO_BUILDING)
	{
		pPopupInfo->setData2(ORDER_CONSTRUCT);
		pPopupInfo->setData3(eConstructBuilding);
	}
	else if (eCreateProject != NO_PROJECT)
	{
		pPopupInfo->setData2(ORDER_CREATE);
		pPopupInfo->setData3(eCreateProject);
	}
	else
	{
		pPopupInfo->setData2(NO_ORDER);
		pPopupInfo->setData3(NO_UNIT);
	}

	gDLL->UI().addPopup(pPopupInfo, getOwner(), false, bFront);
}
//doto enhanced city size mylon
// f1rpo amended this func - see below.
//is supposed to return NO_CITYPLOT when kPlot is outside the radius. This function relies on that (maybe others do too):
// Not going to work correctly if CvMap:: plotCityXY assumes MAX_CITY_RADIUS. However, it should be sufficient (and easier) to add a city radius check to CvCity::getCityPlotIndex:
// advc.enum: Return type was int
//CityPlotTypes CvCity::getCityPlotIndex(CvPlot const& kPlot) const
//{
//	return GC.getMap().plotCityXY(getX(), getY(), kPlot);
//}
//doto enhanced city size mylon
CityPlotTypes CvCity::getCityPlotIndex(CvPlot const& kPlot) const
{
	if (plotDistance(&kPlot, plot()) > getRadius())
		return NO_CITYPLOT;
	return GC.getMap().plotCityXY(getX(), getY(), kPlot);
}
//doto enhanced city size mylon
CvPlot* CvCity::getCityIndexPlot(CityPlotTypes ePlot) const // advc.enum: CityPlotTypes
{
	return GC.getMap().plotCity(getX(), getY(), ePlot);
}


bool CvCity::canWork(CvPlot const& kPlot) const
{
	if (kPlot.getWorkingCity() != this)
		return false;
//doto enhanced city size mylon
	//FAssert(getCityPlotIndex(kPlot) != NO_CITYPLOT);
	FAssertBounds(0, NUM_CITY_PLOTS, getCityPlotIndex(kPlot));
	// Just in case FAssertMsg below doesn't end the function?
	/*if (getCityPlotIndex(kPlot) >= NUM_CITY_PLOTS)
		return false;*/
//doto enhanced city size mylon
	if (kPlot.plotCheck(PUF_canSiege, getOwner()) != NULL)
		return false;

	if (kPlot.isWater())
	{
		if (!GET_TEAM(getTeam()).isWaterWork())
			return false;

		if (kPlot.getBlockadedCount(getTeam()) > 0)
		{   /*  <advc.124> Should work, but I don't want units to overwrite
				blockades after all. */
			/*bool bDefended = false;
			for(int i = 0; i < MAX_CIV_PLAYERS; i++) {
				PlayerTypes eLoopPlayer = (PlayerTypes)i;
				if(GET_PLAYER(eLoopPlayer).isAlive() && GET_PLAYER(eLoopPlayer).getMasterTeam() ==
						GET_PLAYER(getOwner()).getMasterTeam() &&
						kPlot.isVisibleOtherUnit(eLoopPlayer)) {
					isDefended = true;
					break;
				}
			}
			if(!isDefended)*/ // </advc.124>
				return false;
		}

		// Replaced by blockade mission, above
		/*if (!kPlot.plotCheck(PUF_canDefend, -1, -1, NO_PLAYER, getTeam())) {
			FOR_EACH_ADJ_PLOT(kPlot) {
				if (pAdj->isWater() && pAdj->plotCheck(PUF_canSiege, getOwner()) != NULL)
					return false;
		} }*/
	}

	if (!kPlot.hasYield())
		return false;

	return true;
}


void CvCity::verifyWorkingPlot(CityPlotTypes ePlot) // advc.enum: CityPlotTypes
{
//doto enhanced city size mylon
//	FAssertEnumBounds(ePlot);
	FAssertBounds(0, NUM_CITY_PLOTS, ePlot);
	if (isWorkingPlot(ePlot))
	{
		CvPlot* pPlot = getCityIndexPlot(ePlot);
		if (pPlot != NULL && !canWork(*pPlot))
		{
			setWorkingPlot(ePlot, false);
			AI_setAssignWorkDirty(true);
		}
	}
}


void CvCity::verifyWorkingPlots()
{
	//FOR_EACH_ENUM(CityPlot)
//doto enhanced city size mylon
	FOR_EACH_CITYPLOT
		verifyWorkingPlot(eLoopCityPlot);
}


void CvCity::clearWorkingOverride(CityPlotTypes ePlot) // advc.enum: CityPlotTypes
{
	CvPlot* pPlot = getCityIndexPlot(ePlot);
	if (pPlot != NULL)
		pPlot->setWorkingCityOverride(NULL);
}


int CvCity::countNumImprovedPlots(ImprovementTypes eImprovement, bool bPotential) const
{
	int iCount = 0;
//doto enhanced city size mylon - defined in the custom doto cityplotiter file
	for (WorkablePlotIter it(*this); it.hasNext(); ++it)
	{
		CvPlot const& kPlot = *it;
		if (eImprovement != NO_IMPROVEMENT)
		{
			if (kPlot.getImprovementType() == eImprovement ||
				(bPotential && kPlot.canHaveImprovement(eImprovement, getTeam())))
			{
				iCount++;
			}
		}
		else if (kPlot.isImproved())
			iCount++;
	}
	return iCount;
}


int CvCity::countNumWaterPlots() const
{
	int iCount = 0;
	for (WorkablePlotIter it(*this); it.hasNext(); ++it)
	{
		if (it->isWater())
			iCount++;
	}
	return iCount;
}

int CvCity::countNumRiverPlots() const
{
	int iCount = 0;
	for (WorkablePlotIter it(*this); it.hasNext(); ++it)
	{
		if (it->isRiver())
			iCount++;
	}
	return iCount;
}


int CvCity::findPopulationRank() const
{
	if (m_bPopulationRankValid)
		return m_iPopulationRank;

	/*int iRank = 1;
	FOR_EACH_CITY(pLoopCity, GET_PLAYER(getOwner())) {
		if ((pLoopCity->getPopulation() > getPopulation()) ||
				((pLoopCity->getPopulation() == getPopulation()) && (pLoopCity->getID() < getID())))
			iRank++;
	}
	// shenanigans are to get around the const check
	m_bPopulationRankValid = true;
	m_iPopulationRank = iRank;*/ // BtS
	// K-Mod. Set all ranks at the same time.
	CvPlayer const& kPlayer = GET_PLAYER(getOwner());
	std::vector<std::pair<int,int> > aiiCityScores;
	FOR_EACH_CITY(pLoopCity, kPlayer)
	{
		aiiCityScores.push_back(std::make_pair(
				-pLoopCity->getPopulation(), pLoopCity->getID()));
	}
	// note: we are sorting by minimum of _negative_ score, and then by min cityID.
	std::sort(aiiCityScores.begin(), aiiCityScores.end());
	FAssert(aiiCityScores.size() == kPlayer.getNumCities());
	for (size_t i = 0; i < aiiCityScores.size(); i++)
	{
		CvCity* pLoopCity = kPlayer.getCity(aiiCityScores[i].second);
		pLoopCity->m_iPopulationRank = i + 1;
		pLoopCity->m_bPopulationRankValid = true;
	}
	FAssert(m_bPopulationRankValid);
	// K-Mod end
	return m_iPopulationRank;
}


int CvCity::findBaseYieldRateRank(YieldTypes eYield) const
{
	if (m_abBaseYieldRankValid.get(eYield))
		return m_aiBaseYieldRank.get(eYield); // advc
	/*int iRate = getBaseYieldRate(eYield);
	int iRank = 1;
	FOR_EACH_CITY(pLoopCity, GET_PLAYER(getOwner()) {
		if ((pLoopCity->getBaseYieldRate(eYield) > iRate) ||
			((pLoopCity->getBaseYieldRate(eYield) == iRate) && (pLoopCity->getID() < getID())))
			iRank++;
	}
	m_abBaseYieldRankValid.set(eYield, true);
	m_aiBaseYieldRank.set(eYield, iRank);*/ // BtS
	// K-Mod. Set all ranks at the same time.
	CvPlayer const& kPlayer = GET_PLAYER(getOwner());
	std::vector<std::pair<int, int> > city_scores;
	FOR_EACH_CITY(pLoopCity, kPlayer)
		city_scores.push_back(std::make_pair(-pLoopCity->getBaseYieldRate(eYield), pLoopCity->getID()));
	// note: we are sorting by minimum of _negative_ score, and then by min cityID.
	std::sort(city_scores.begin(), city_scores.end());
	FAssert(city_scores.size() == kPlayer.getNumCities());
	for (size_t i = 0; i < city_scores.size(); i++)
	{
		CvCity* pLoopCity = kPlayer.getCity(city_scores[i].second);
		pLoopCity->m_aiBaseYieldRank.set(eYield, i + 1);
		pLoopCity->m_abBaseYieldRankValid.set(eYield, true);
	}
	FAssert(m_abBaseYieldRankValid.get(eYield));
	// K-Mod end
	return m_aiBaseYieldRank.get(eYield);
}


int CvCity::findYieldRateRank(YieldTypes eYield) const
{
	if (m_abYieldRankValid.get(eYield))
		return m_aiYieldRank.get(eYield); // advc

	/*int iRate = getYieldRate(eYield);
	int iRank = 1;
	FOR_EACH_CITY(pLoopCity, GET_PLAYER(getOwner())) {
		if ((pLoopCity->getYieldRate(eYield) > iRate) ||
			((pLoopCity->getYieldRate(eYield) == iRate) && (pLoopCity->getID() < getID())))
			iRank++;
	}
	m_abYieldRankValid.set(eYield, true);
	m_aiYieldRank.set(eYield, iRank);*/ // BtS
	// K-Mod. Set all ranks at the same time.
	CvPlayer const& kPlayer = GET_PLAYER(getOwner());
	std::vector<std::pair<int,int> > city_scores;
	FOR_EACH_CITY(pLoopCity, kPlayer)
		city_scores.push_back(std::make_pair(-pLoopCity->getYieldRate(eYield), pLoopCity->getID()));
	// note: we are sorting by minimum of _negative_ score, and then by min cityID.
	std::sort(city_scores.begin(), city_scores.end());
	FAssert(city_scores.size() == kPlayer.getNumCities());
	for (size_t i = 0; i < city_scores.size(); i++)
	{
		CvCity* pLoopCity = kPlayer.getCity(city_scores[i].second);
		pLoopCity->m_aiYieldRank.set(eYield, i + 1);
		pLoopCity->m_abYieldRankValid.set(eYield, true);
	}
	FAssert(m_abYieldRankValid.get(eYield));
	// K-Mod end
	return m_aiYieldRank.get(eYield);
}


int CvCity::findCommerceRateRank(CommerceTypes eCommerce) const
{
	if (m_abCommerceRankValid.get(eCommerce))
		return m_aiCommerceRank.get(eCommerce);
	/*int iRate = getCommerceRateTimes100(eCommerce);
	int iRank = 1;
	FOR_EACH_CITY(pLoopCity, GET_PLAYER(getOwner()) {
		if ((pLoopCity->getCommerceRateTimes100(eCommerce) > iRate) ||
				((pLoopCity->getCommerceRateTimes100(eCommerce) == iRate) && (pLoopCity->getID() < getID())))
			iRank++;
	}
	m_abCommerceRankValid.set(eCommerce, true);
	m_aiCommerceRank.set(eCommerce, iRank);*/ // BtS
	// K-Mod. Set all ranks at the same time.
	CvPlayer const& kPlayer = GET_PLAYER(getOwner());
	std::vector<std::pair<int, int> > city_scores;
	FOR_EACH_CITY(pLoopCity, kPlayer)
		city_scores.push_back(std::make_pair(-pLoopCity->getCommerceRateTimes100(eCommerce), pLoopCity->getID()));
	// note: we are sorting by minimum of _negative_ score, and then by min cityID.
	std::sort(city_scores.begin(), city_scores.end());
	FAssert(city_scores.size() == kPlayer.getNumCities());
	for (size_t i = 0; i < city_scores.size(); i++)
	{
		CvCity* pLoopCity = kPlayer.getCity(city_scores[i].second);
		pLoopCity->m_aiCommerceRank.set(eCommerce, i + 1);
		pLoopCity->m_abCommerceRankValid.set(eCommerce, true);
	}
	FAssert(m_abCommerceRankValid.get(eCommerce));
	// K-Mod end
	return m_aiCommerceRank.get(eCommerce);
}
/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* CanTrain                                                                                     */
/************************************************************************************************/
bool CvCity::isPlotTrainable(UnitTypes eUnit, bool bContinue, bool bTestVisible) const
{
	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
	CvPlayer& pPlayer = GET_PLAYER(getOwner());
	int iI;

	if (!bTestVisible)
	{
		if (kUnit.isStateReligion())
		{
			if (pPlayer.getStateReligion() != NO_RELIGION)
			{
				if(!(isHasReligion(pPlayer.getStateReligion())))
				{
					return false;
				}
			}
		}

		for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			if (kUnit.isPrereqBuildingClass(iI))
			{
				if(pPlayer.isBuildingClassRequiredToTrain(BuildingClassTypes(iI), eUnit))
				{
					if (!(getNumBuilding((BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI))))
					{
						return false;
					}
				}
			}
		}

		if (kUnit.getPrereqBuilding() != NO_BUILDING)
		{

			if(!(getNumBuilding((BuildingTypes)(kUnit.getPrereqBuilding()))))
			{
				SpecialBuildingTypes eSpecialBuilding = ((SpecialBuildingTypes)(GC.getBuildingInfo((BuildingTypes)(kUnit.getPrereqBuilding())).getSpecialBuildingType()));

				if ((eSpecialBuilding == NO_SPECIALBUILDING) || !(pPlayer.isSpecialBuildingNotRequired(eSpecialBuilding)))
				{
					return false;
				}
			}
		}

	}

	return true;
}

//Returns true if the city can train a unit, or any upgrade for that unit that forces it obsolete
bool CvCity::isForceObsoleteUnitClassAvailable(UnitTypes eUnit) const
{
	UnitTypes eLoopUnit = NO_UNIT;
	int iI;
	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
	CvCivilizationInfo& kCivilization = GC.getCivilizationInfo(getCivilizationType());

	FAssertMsg(eUnit != NO_UNIT, "eUnit is expected to be assigned (not NO_UNIT)");

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		if (kUnit.getForceObsoleteUnitClass(iI))
		{
			eLoopUnit = (UnitTypes)kCivilization.getCivilizationUnits(iI);
			if(eLoopUnit == NO_UNIT)
			{
				continue;
			}
			CvUnitInfo& kLoopUnit = GC.getUnitInfo(eLoopUnit);

			if (canTrain(eLoopUnit, false, false, false, true))
			{
				return true;
			}
				
			int iJ;
				
			for (iJ = 0; iJ < GC.getNumUnitClassInfos(); iJ++)
			{
				if (kLoopUnit.getUpgradeUnitClass(iJ))
				{
					eLoopUnit = (UnitTypes)kCivilization.getCivilizationUnits(iJ);
						
					if (eLoopUnit != NO_UNIT)
					{
						if (canTrain(eLoopUnit, false, false, false, true))
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/

// Returns one of the upgrades
UnitTypes CvCity::allUpgradesAvailable(UnitTypes eUnit, int iUpgradeCount,
	BonusTypes eAssumeAvailable) const // advc.001u
{
	PROFILE_FUNC(); // advc

	FAssert(eUnit != NO_UNIT);

	CvCivilization const& kCiv = getCivilization();
	if (iUpgradeCount > kCiv.getNumUnits()) // (advc.003w: was >GC.getNumUnitClassInfos())
		return NO_UNIT;

	UnitTypes eUpgradeUnit = NO_UNIT;
	bool bUpgradeFound = false;
	bool bUpgradeAvailable = false;
	bool bUpgradeUnavailable = false;
	if (GC.getInfo(eUnit).isAnyUpgradeUnitClass()) // advc.003t
	{
		for (int i = 0; i < kCiv.getNumUnits(); i++)
		{
			if (!GC.getInfo(eUnit).getUpgradeUnitClass(kCiv.unitClassAt(i)))
				continue;
	
			bUpgradeFound = true;
			UnitTypes eTempUnit = allUpgradesAvailable(kCiv.unitAt(i), iUpgradeCount + 1,
					eAssumeAvailable); // advc.001u
			if (eTempUnit != NO_UNIT)
			{
				eUpgradeUnit = eTempUnit;
				bUpgradeAvailable = true;
			}
			else bUpgradeUnavailable = true;
		}
	}

	if (iUpgradeCount > 0)
	{
		if (bUpgradeFound && bUpgradeAvailable)
		{
			FAssert(eUpgradeUnit != NO_UNIT);
			return eUpgradeUnit;
		}
		if(canTrain(eUnit, false, false, false, true, /* advc.001b: */ false,
			eAssumeAvailable)) // advc.001u
		{
			return eUnit;
		}
	}
	else if (bUpgradeFound && !bUpgradeUnavailable)
		return eUpgradeUnit;

	return NO_UNIT;
}

// <advc.001b>
bool CvCity::canUpgradeTo(UnitTypes eUnit) const
{
	return canTrain(eUnit, false, false, true, false, false);
}// </advc.001b>

bool CvCity::isWorldWondersMaxed() const
{
	if (GET_PLAYER(getOwner()).isOneCityChallenge())
		return false;
	if (GC.getDefineINT(CvGlobals::MAX_WORLD_WONDERS_PER_CITY) == -1)
		return false;
	if (getNumWorldWonders() >= GC.getDefineINT(CvGlobals::MAX_WORLD_WONDERS_PER_CITY))
		return true;
	return false;
}

//Wonder Limit - Doto
bool CvCity::isCultureWorldWondersMaxed() const
{
	if (!GC.getGame().isOption(GAMEOPTION_CULTURE_WONDER_LIMIT))
		return false;
	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
		return false;
	//poor == 1
	int cultureLevelId = getCultureLevel();
	int winderCount = getNumWorldWonders();

	//THIS GAME OPTION will allow construction of 1 wonder, if 
	//all of the wonders are obsilete on a city except 1.
	//this is a way for not blocking winders for cities with ancient none working wonders...
	if (GC.getGame().isOption(GAMEOPTION_CULTURE_WONDER_LIMIT_OBSOLETE))
	{
		int obsoletecount = 0;
		FOR_EACH_ENUM(Building)
		{
			obsoletecount += GET_TEAM(getTeam()).isObsoleteBuilding(eLoopBuilding)
				&& GC.getInfo(eLoopBuilding).isWorldWonder() ? 1 : 0;
		}
		if ((winderCount - obsoletecount) == 1)
			return false;
	}
	//unused but kept it.
	int cuktureWLimit =  GC.getInfo(getCultureLevel()).getmaxWonderCultureLimit();
	//CvWString cultureLevelDesc = GC.getInfo(getCultureLevel()).getTextKeyWide();
	//wonder amount according to the level of culture - controlled from the globaldefines file
	//CultureLevelTypes
	if (cultureLevelId == 0)
		return true;
	//i was thinking of limit based on world size or speed... left it off for now
	//CvWString speed = GC.getInfo(GC.getGame().getGameSpeedType()).getDescription();
	/*CvWString worldSize = GC.getInfo(GC.getMap().getWorldSize()).getDescription();

	if (worldSize == L"WORLDSIZE_LARGE" )
		winderCount += 1;
	else if (worldSize == L"WORLDSIZE_STANDARD")
		winderCount += 2;
	else if (worldSize == L"WORLDSIZE_SMALL")
		winderCount += 3;
	else if (worldSize == L"WORLDSIZE_TINY" || worldSize == L"WORLDSIZE_TINY")
		winderCount += 4;*/

	if (winderCount >= cuktureWLimit)
		return true;
/* method 2
	else if(cultureLevelId == 1 && numOfwonders > GC.getDefineINT(CvGlobals::MAX_WORLD_WONDERS_CULTURE_POOR))
		return false;
	else if(cultureLevelId == 2 && numOfwonders > GC.getDefineINT(CvGlobals::MAX_WORLD_WONDERS_CULTURE_FLEDGLING))
		return false;
	else if(cultureLevelId == 3 && numOfwonders >  GC.getDefineINT(CvGlobals::MAX_WORLD_WONDERS_CULTURE_DEVELOPING))
		return false;
	else if(cultureLevelId == 4 && numOfwonders >  GC.getDefineINT(CvGlobals::MAX_WORLD_WONDERS_CULTURE_REFINED))
		return false;
	else if(cultureLevelId == 5 && numOfwonders >  GC.getDefineINT(CvGlobals::MAX_WORLD_WONDERS_CULTURE_INFLUENTIAL))
		return false;
	else if (cultureLevelId == 6 && numOfwonders >  GC.getDefineINT(CvGlobals::MAX_WORLD_WONDERS_CULTURE_LEGENDARY))
		return false;
*/
	return false;
}
//Wonder Limit - Doto


bool CvCity::isTeamWondersMaxed() const
{
	if (GET_PLAYER(getOwner()).isOneCityChallenge())
		return false;
	if (GC.getDefineINT(CvGlobals::MAX_TEAM_WONDERS_PER_CITY) == -1)
		return false;
	if (getNumTeamWonders() >= GC.getDefineINT(CvGlobals::MAX_TEAM_WONDERS_PER_CITY))
		return true;
	return false;
}


bool CvCity::isNationalWondersMaxed() const
{	// <advc.004w>
	int iLeft = getNumNationalWondersLeft();
	if(iLeft < 0)
		return false;
	return (iLeft == 0);
	// Moved the remainder of this function into getNumNationalWondersLeft
}

int CvCity::getNumNationalWondersLeft() const
{
	int iMaxNumWonders = (GET_PLAYER(getOwner()).isOneCityChallenge() ?
			GC.getDefineINT(CvGlobals::MAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC) :
			GC.getDefineINT(CvGlobals::MAX_NATIONAL_WONDERS_PER_CITY));
	if(iMaxNumWonders < 0)
		return -1;
	return std::max(0, iMaxNumWonders - getNumNationalWonders());
} // </advc.004w>


bool CvCity::isBuildingsMaxed() const
{
	// <advc.opt> -1 unless a mod-mod changes it
	static int const iMaxBuildingsPerCity = GC.getDefineINT("MAX_BUILDINGS_PER_CITY");
	if (iMaxBuildingsPerCity < 0)
		return false; // </advc.opt>
	if (GET_PLAYER(getOwner()).isOneCityChallenge())
		return false;
	return (getNumBuildings() >= iMaxBuildingsPerCity);
}

// advc.064d:
void CvCity::verifyProduction()
{
	if(isProduction()) // Only want to address invalid orders here; no production is OK.
	{
		/*  This doesn't check all preconditions, just the ones that could change
			throughout a (human) turn. Don't want the AI to choose a new order
			b/c it might be another civ's turn, and, in that case,
			CvPlayerAI::AI_chooseProduction isn't guaranteed to work correctly. */
		checkCanContinueProduction(true, HUMAN_CHOOSE);
	}
}


bool CvCity::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades,
	bool bCheckAirUnitCap, // advc.001b
	BonusTypes eAssumeAvailable) const // advc.001u
{
	//PROFILE_FUNC(); // advc.003o
	FAssert(eUnit != NO_UNIT); // advc

	if (GC.getPythonCaller()->canTrainOverride(
		*this, eUnit, bContinue,  bTestVisible, bIgnoreCost, bIgnoreUpgrades))
	{
		return true;
	}

	if(!GET_PLAYER(getOwner()).canTrain(eUnit, bContinue, bTestVisible, bIgnoreCost))
		return false;

	if(!getPlot().canTrain(eUnit, bContinue, bTestVisible, /* advc.001b: */ bCheckAirUnitCap,
		eAssumeAvailable)) // advc.001u
	{
		return false;
	}
	// advc.opt: Moved down. Seems a bit slower than CvPlot::canTrain.
	if (!bIgnoreUpgrades)
	{
		if (allUpgradesAvailable(eUnit) != NO_UNIT)
			return false;
	}

	if (GC.getPythonCaller()->cannotTrainOverride(
		*this, eUnit, bContinue, bTestVisible, bIgnoreCost, bIgnoreUpgrades))
	{
		return false;
	}
/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* CanTrain                                                                                     */
/************************************************************************************************/
	if (isForceObsoleteUnitClassAvailable(eUnit))
	{
		return false;
	}

	if (!(isPlotTrainable(eUnit, bContinue, bTestVisible)))
	{
		return false;
	}
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	if (getPopulation() < GC.getUnitInfo(eUnit).getNumCitySizeUnitPrereq())
	{
		return false;
	}
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
//Shqype Bonus Vicinity Start
//	if (GC.getUnitInfo(eUnit).getPrereqVicinityBonus() != NO_BONUS)
//	{
//		for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
//		{
//			CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);
			//if (pLoopPlot->getBonusType() == GC.getUnitInfo(eUnit).getPrereqVicinityBonus())
			//{
//			if (pLoopPlot->isHasValidBonus((BonusTypes)GC.getUnitInfo(eUnit).getPrereqVicinityBonus()))
				//if (pLoopPlot->isHasValidBonus())
//			{
//				return true;
//			}
			//}
//		}
//		return false;
//	}
//Shqype Vicinity Bonus End	
	return true;
}

bool CvCity::canTrain(UnitCombatTypes eUnitCombat) const
{
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumUnits(); i++)
	{
		UnitTypes eUnit = kCiv.unitAt(i);
		if (GC.getInfo(eUnit).getUnitCombatType() == eUnitCombat &&
			canTrain(eUnit))
		{
			return true;
		}
	}
	return false;
}

// advc: Refactored
bool CvCity::canConstruct(BuildingTypes eBuilding, bool bContinue,
	bool bTestVisible, bool bIgnoreCost, bool bIgnoreTech) const
{
	FAssert(eBuilding != NO_BUILDING); // advc

	if (GC.getPythonCaller()->canConstructOverride(
		*this, eBuilding, bContinue, bTestVisible, bIgnoreCost))
	{
		return true;
	}
	if(!GET_PLAYER(getOwner()).canConstruct(eBuilding, bContinue, bTestVisible, bIgnoreCost, bIgnoreTech))
		return false;

	if (getNumBuilding(eBuilding) >= GC.getDefineINT(CvGlobals::CITY_MAX_NUM_BUILDINGS))
		return false;

	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	if (kBuilding.isPrereqReligion() && //getReligionCount() > 0
		getReligionCount() <= 0) // K-Mod (bugfix)
	{
		return false;
	}
	{
		ReligionTypes ePrereqReligion = kBuilding.getPrereqReligion();
		if (ePrereqReligion != NO_RELIGION && !isHasReligion(ePrereqReligion))
			return false;
	}
	if (kBuilding.isStateReligion())
	{
		ReligionTypes eStateReligion = GET_PLAYER(getOwner()).getStateReligion();
		if(eStateReligion == NO_RELIGION || !isHasReligion(eStateReligion))
			return false;
	}
	{
		CorporationTypes ePrereqCorp = kBuilding.getPrereqCorporation();
		if (ePrereqCorp != NO_CORPORATION)
		{
			if(!isHasCorporation(ePrereqCorp))
				return false;
		}
	}

	CorporationTypes eFoundCorp = kBuilding.getFoundsCorporation();
	if (eFoundCorp != NO_CORPORATION)
	{
		FOR_EACH_ENUM(Corporation)
		{
			if (isHeadquarters(eLoopCorporation) &&
				GC.getGame().isCompetingCorporation(eLoopCorporation, eFoundCorp))
			{
				return false;
			}
		}
	}
	if (!getPlot().canConstruct(eBuilding))
		return false;

	if (kBuilding.isGovernmentCenter() && isGovernmentCenter())
		return false;

	if (!bTestVisible)
	{
		if (!bContinue && getFirstBuildingOrder(eBuilding) != -1)
			return false;

		BuildingClassTypes eBuildingClass = (BuildingClassTypes)kBuilding.getBuildingClassType();
		CvBuildingClassInfo const& kBuildingClass = GC.getInfo(eBuildingClass);
		if (!kBuildingClass.isNoLimit())
		{
			if (kBuildingClass.isWorldWonder())
			{
				if(isWorldWondersMaxed())
					return false;
//doto wonder limit
				if (isCultureWorldWondersMaxed())
					return false;
			}
			else if (kBuildingClass.isTeamWonder())
			{
				if(isTeamWondersMaxed())
					return false;
			}
			else if (kBuildingClass.isNationalWonder())
			{
				if(isNationalWondersMaxed())
					return false;
			}
			else if(isBuildingsMaxed())
				return false;
		}
		{
			ReligionTypes eHolyCityReligion = kBuilding.getHolyCity();
			if (eHolyCityReligion != NO_RELIGION && !isHolyCity(eHolyCityReligion))
				return false;
		}
		if (kBuilding.getPrereqAndBonus() != NO_BONUS &&
			!hasBonus(kBuilding.getPrereqAndBonus()))
		{
			return false;
		}
//Shqype Vicinity Bonus Start
/*		if (kBuilding.getPrereqVicinityBonus() != NO_BONUS)
		{
			for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);
				if (pLoopPlot != NULL && pLoopPlot->getBonusType() == kBuilding.getPrereqVicinityBonus())
				{
					CvCity* pCity = GC.getMapINLINE().findCity(getX(), getY(), getOwner(), NO_TEAM, false);
					if (pLoopPlot != NULL && pLoopPlot->isHasValidBonus() && pLoopPlot != NULL && pLoopPlot->isConnectedTo(pCity))
					{
						return true;
					}
				}
			}
			return false;
		}*/
//Shqype Vicinity Bonus End
		if (eFoundCorp != NO_CORPORATION)
		{
			bool bValidBonus = false;
			for (int i = 0; i < GC.getInfo(eFoundCorp).getNumPrereqBonuses(); i++)
			{
				if (hasBonus(GC.getInfo(eFoundCorp).getPrereqBonus(i)))
				{
					bValidBonus = true;
					break;
				}
			}
			if(!bValidBonus)
				return false;
		}

		if(getPlot().getLatitude() > kBuilding.getMaxLatitude())
			return false;
		if(getPlot().getLatitude() < kBuilding.getMinLatitude())
			return false;
		{
			bool bPrereqBonus = false;
			bool bValidBonus = false;
			for (int i = 0; i < kBuilding.getNumPrereqOrBonuses(); i++)
			{
				bPrereqBonus = true;
				if(hasBonus(kBuilding.getPrereqOrBonuses(i)))
					bValidBonus = true;
			}
			if(bPrereqBonus && !bValidBonus)
				return false;
		}

//Shqype Vicinity Bonus Start
/*		bRequiresBonus = false;
		bNeedsBonus = true;

		for (iI = 0; iI < GC.getNUM_BUILDING_PREREQ_OR_BONUSES(); iI++)
		{
			if (kBuilding.getPrereqOrVicinityBonuses(iI) != NO_BONUS)
			{
				bRequiresBonus = true;

				for (int iJ = 0; iJ < NUM_CITY_PLOTS; ++iJ)
				{
					CvPlot* pLoopPlot = plotCity(getX(), getY(), iJ);
					if (pLoopPlot != NULL && pLoopPlot->getBonusType() == kBuilding.getPrereqOrVicinityBonuses(iI))
					{
						CvCity* pCity = GC.getMapINLINE().findCity(getX(), getY(), getOwner(), NO_TEAM, false);
						if (pLoopPlot->isHasValidBonus() && pLoopPlot->isConnectedTo(pCity))
						{
			                bNeedsBonus = false;
							return true;
						}
					}
				}
				bNeedsBonus = true;
			}
		}

		if (bRequiresBonus && bNeedsBonus)
		{
			return false;
		}*/
//Shqype Vicinity Bonus End

		FOR_EACH_NON_DEFAULT_KEY(kBuilding.
			isBuildingClassNeededInCity(), BuildingClass)
		{
			/*BuildingTypes ePrereqBuilding = (BuildingTypes)GC.getInfo(getCivilizationType()).getCivilizationBuildings(eLoopBuildingClass);
			if (ePrereqBuilding != NO_BUILDING) {
				if(getNumBuilding(ePrereqBuilding) <= 0)
						//&& (bContinue || (getFirstBuildingOrder(ePrereqBuilding) == -1))) // advc (comment): This was already commented out in Vanilla Civ 4
					return false;
			}*/  // <advc.003w>
			if (getNumBuilding(eLoopBuildingClass) <= 0)
				return false; // </advc.003w>
		}
	}
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
		if (getPopulation() < kBuilding.getNumCitySizeBldPrereq())
		{
			return false;
		}
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	if (GC.getPythonCaller()->cannotConstructOverride(
		*this, eBuilding, bContinue, bTestVisible, bIgnoreCost))
	{
		return false;
	}
	return true;
}


bool CvCity::canCreate(ProjectTypes eProject, bool bContinue, bool bTestVisible) const
{
	if (GC.getPythonCaller()->canCreateOverride(*this, eProject, bContinue, bTestVisible))
		return true;
	if (!GET_PLAYER(getOwner()).canCreate(eProject, bContinue, bTestVisible))
		return false;
	if (GC.getPythonCaller()->cannotCreateOverride(*this, eProject, bContinue, bTestVisible))
		return false;
	return true;
}


bool CvCity::canMaintain(ProcessTypes eProcess, bool bContinue) const
{
	if (GC.getPythonCaller()->canMaintainOverride(*this, eProcess, bContinue))
		return true;
	if (!GET_PLAYER(getOwner()).canMaintain(eProcess, bContinue))
		return false;
	if (GC.getPythonCaller()->cannotMaintainOverride(*this, eProcess, bContinue))
		return false;
	return true;
}


bool CvCity::canJoin() const
{
	return true;
}


int CvCity::getFoodTurnsLeft() const
{
	int const iFoodDiff = foodDifference(); // advc.opt
	// <advc.189>
	if (iFoodDiff == 0)
		return MAX_INT;
	if (iFoodDiff < 0) // Turns to starvation
	{
		int iFoodLeft = getFood();
		int iTurnsLeft = intdiv::uceil(iFoodLeft, -iFoodDiff);
		return std::min(-1, -iTurnsLeft);
	} // </advc.189>
	int iFoodLeft = growthThreshold() - getFood();
//doto advc fix for assert
	if (iFoodLeft < 0)
		FErrorMsg("keldath -  check this out");
		return 1;
	int iTurnsLeft = intdiv::uceil(iFoodLeft, iFoodDiff);
	return std::max(1, iTurnsLeft);
}


bool CvCity::isProductionLimited() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return GC.getInfo((UnitTypes)pOrderNode->m_data.iData1).isLimited();
			break;

		case ORDER_CONSTRUCT:
			return GC.getInfo((BuildingTypes)pOrderNode->m_data.iData1).isLimited();
			break;

		case ORDER_CREATE:
			return GC.getInfo((ProjectTypes)pOrderNode->m_data.iData1).isLimited();
			break;

		case ORDER_MAINTAIN:
			break;

		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
		}
	}
	return false;
}


bool CvCity::isProductionUnit() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
		return (pOrderNode->m_data.eOrderType == ORDER_TRAIN);
	return false;
}


bool CvCity::isProductionBuilding() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
		return (pOrderNode->m_data.eOrderType == ORDER_CONSTRUCT);
	return false;
}


bool CvCity::isProductionProject() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
		return (pOrderNode->m_data.eOrderType == ORDER_CREATE);
	return false;
}


bool CvCity::isProductionProcess() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
		return (pOrderNode->m_data.eOrderType == ORDER_MAINTAIN);
	return false;
}


bool CvCity::canContinueProduction(OrderData order)
{
	switch (order.eOrderType)
	{
	case ORDER_TRAIN:
		return canTrain((UnitTypes)(order.iData1), true);
		break;

	case ORDER_CONSTRUCT:
		return canConstruct((BuildingTypes)(order.iData1), true);
		break;

	case ORDER_CREATE:
		return canCreate((ProjectTypes)(order.iData1), true);
		break;

	case ORDER_MAINTAIN:
		return canMaintain((ProcessTypes)(order.iData1), true);
		break;

	default:
		FErrorMsg("order.eOrderType failed to match a valid option");
	}
	return false;
}


int CvCity::getProductionExperience(UnitTypes eUnit, /* <advc.002f> */
	bool bScore) const
{
	FAssert(!bScore || eUnit == NO_UNIT); // </advc.002f>
	CvPlayer const& kOwner = GET_PLAYER(getOwner());
	int iExperience = getFreeExperience();
	iExperience += kOwner.getFreeExperience();
	iExperience += getSpecialistFreeExperience(); // K-Mod (moved from below)
	if (kOwner.getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(kOwner.getStateReligion()))
			iExperience += kOwner.getStateReligionFreeExperience();
	}
	if (eUnit != NO_UNIT)
	{
		if (GC.getInfo(eUnit).getUnitCombatType() != NO_UNITCOMBAT)
		{
			iExperience += getUnitCombatFreeExperience((UnitCombatTypes)
					GC.getInfo(eUnit).getUnitCombatType());
		}
		iExperience += getDomainFreeExperience(GC.getInfo(eUnit).getDomainType());
		//iExperience += getSpecialistFreeExperience();
	}
	// <advc.002f> AI score for comparing cities wrt. their free XP
	else if (bScore)
	{
		int iNonGenericXP = 0;
		FOR_EACH_ENUM(UnitCombat)
			iNonGenericXP += getUnitCombatFreeExperience(eLoopUnitCombat);
		FOR_EACH_ENUM(Domain)
			iNonGenericXP += getDomainFreeExperience(eLoopDomain);
		iExperience += iExperience + (iNonGenericXP + 1) / 2;
	} // </advc.002f>
	return std::max(0, iExperience);
}


void CvCity::addProductionExperience(CvUnit* pUnit, bool bConscript)
{
	if (pUnit->canAcquirePromotionAny())
	{
		pUnit->changeExperience(getProductionExperience(pUnit->getUnitType()) /
				(bConscript ? 2 : 1));
	}
	if (isAnyFreePromotion()) // advc.opt
	{
		FOR_EACH_ENUM(Promotion)
		{
			if (isFreePromotion(eLoopPromotion))
			{
				if (pUnit->getUnitCombatType() != NO_UNITCOMBAT &&
					GC.getInfo(eLoopPromotion).getUnitCombat(pUnit->getUnitCombatType()))
				{
					pUnit->setHasPromotion(eLoopPromotion, true);
				}
			}
		}
	}
	pUnit->testPromotionReady();
}


UnitTypes CvCity::getProductionUnit() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return ((UnitTypes)(pOrderNode->m_data.iData1));
			break;
		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
			break;
		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
		}
	}
	return NO_UNIT;
}


UnitAITypes CvCity::getProductionUnitAI() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return ((UnitAITypes)(pOrderNode->m_data.iData2));
		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
			break;
		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
		}
	}
	return NO_UNITAI;
}


BuildingTypes CvCity::getProductionBuilding() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			break;
		case ORDER_CONSTRUCT:
			return ((BuildingTypes)(pOrderNode->m_data.iData1));
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
			break;
		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
		}
	}
	return NO_BUILDING;
}


ProjectTypes CvCity::getProductionProject() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
		case ORDER_CONSTRUCT:
			break;
		case ORDER_CREATE:
			return ((ProjectTypes)(pOrderNode->m_data.iData1));
		case ORDER_MAINTAIN:
			break;
		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
		}
	}
	return NO_PROJECT;
}


ProcessTypes CvCity::getProductionProcess() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
			break;
		case ORDER_MAINTAIN:
			return ((ProcessTypes)(pOrderNode->m_data.iData1));
		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
		}
	}
	return NO_PROCESS;
}


const wchar* CvCity::getProductionName() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return GC.getInfo((UnitTypes) pOrderNode->m_data.iData1).getDescription();
		case ORDER_CONSTRUCT:
			return GC.getInfo((BuildingTypes) pOrderNode->m_data.iData1).getDescription();
		case ORDER_CREATE:
			return GC.getInfo((ProjectTypes) pOrderNode->m_data.iData1).getDescription();
		case ORDER_MAINTAIN:
			return GC.getInfo((ProcessTypes) pOrderNode->m_data.iData1).getDescription();
		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
		}
	}
	return L"";
}


const wchar* CvCity::getProductionNameKey() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return GC.getInfo((UnitTypes) pOrderNode->m_data.iData1).getTextKeyWide();
		case ORDER_CONSTRUCT:
			return GC.getInfo((BuildingTypes) pOrderNode->m_data.iData1).getTextKeyWide();
		case ORDER_CREATE:
			return GC.getInfo((ProjectTypes) pOrderNode->m_data.iData1).getTextKeyWide();
		case ORDER_MAINTAIN:
			return GC.getInfo((ProcessTypes) pOrderNode->m_data.iData1).getTextKeyWide();
		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
		}
	}
	return L"";
}


bool CvCity::isFoodProduction() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();

	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return isFoodProduction((UnitTypes)(pOrderNode->m_data.iData1));
			break;

		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
			break;

		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return false;
}


bool CvCity::isFoodProduction(UnitTypes eUnit) const
{
	if (GC.getInfo(eUnit).isFoodProduction())
		return true;
	if (GET_PLAYER(getOwner()).isMilitaryFoodProduction())
	{
		if (GC.getInfo(eUnit).isMilitaryProduction())
			return true;
	}
	return false;
}


int CvCity::getFirstUnitOrder(UnitTypes eUnit) const
{
	int iCount = 0;
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	while (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_TRAIN)
		{
			if (pOrderNode->m_data.iData1 == eUnit)
				return iCount;
		}
		iCount++;
		pOrderNode = nextOrderQueueNode(pOrderNode);
	}
	return -1;
}


int CvCity::getFirstBuildingOrder(BuildingTypes eBuilding) const
{
	int iCount = 0;
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	while (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_CONSTRUCT)
		{
			if (pOrderNode->m_data.iData1 == eBuilding)
				return iCount;
		}
		iCount++;
		pOrderNode = nextOrderQueueNode(pOrderNode);
	}
	return -1;
}


int CvCity::getFirstProjectOrder(ProjectTypes eProject) const
{
	int iCount = 0;
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	while (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_CREATE)
		{
			if (pOrderNode->m_data.iData1 == eProject)
				return iCount;
		}
		iCount++;
		pOrderNode = nextOrderQueueNode(pOrderNode);
	}
	return -1;
}


int CvCity::getNumTrainUnitAI(UnitAITypes eUnitAI) const
{
	int iCount = 0;
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	while (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_TRAIN)
		{
			if (pOrderNode->m_data.iData2 == eUnitAI)
				iCount++;
		}
		pOrderNode = nextOrderQueueNode(pOrderNode);
	}
	return iCount;
}


int CvCity::getProduction() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getUnitProduction((UnitTypes)(pOrderNode->m_data.iData1));
		case ORDER_CONSTRUCT:
			return getBuildingProduction((BuildingTypes)(pOrderNode->m_data.iData1));
		case ORDER_CREATE:
			return getProjectProduction((ProjectTypes)(pOrderNode->m_data.iData1));
		case ORDER_MAINTAIN:
			break;
		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
		}
	}
	return 0;
}


int CvCity::getProductionNeeded() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionNeeded((UnitTypes)(pOrderNode->m_data.iData1));
		case ORDER_CONSTRUCT:
			return getProductionNeeded((BuildingTypes)(pOrderNode->m_data.iData1));
		case ORDER_CREATE:
			return getProductionNeeded((ProjectTypes)(pOrderNode->m_data.iData1));
		case ORDER_MAINTAIN:
			break;
		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
		}
	}
	return MAX_INT;
}

int CvCity::getProductionNeeded(UnitTypes eUnit) const
{
	return GET_PLAYER(getOwner()).getProductionNeeded(eUnit);
}


int CvCity::getProductionNeeded(BuildingTypes eBuilding) const
{
	int iProductionNeeded = GET_PLAYER(getOwner()).getProductionNeeded(eBuilding);
	int iPythonModifier = GC.getPythonCaller()->buildingCostMod(*this, eBuilding);
	if (iPythonModifier > 1)
	{
		iProductionNeeded *= iPythonModifier;
		iProductionNeeded /= 100;
	}
	return iProductionNeeded;
}

int CvCity::getProductionNeeded(ProjectTypes eProject) const
{
	return GET_PLAYER(getOwner()).getProductionNeeded(eProject);
}

/*  advc: Deleted. Did exactly the same thing as getProductionTurnsLeft below
	except returning 0 instead of MAX_INT for processes. Was unused in the DLL.
	Python export still in place (see CyCity.cpp). */
/*int CvCity::getGeneralProductionTurnsLeft() const {
	// ...
	return 0;
} */


int CvCity::getProductionTurnsLeft() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		int iData = pOrderNode->m_data.iData1;
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionTurnsLeft((UnitTypes)iData, 0);
		case ORDER_CONSTRUCT:
			return getProductionTurnsLeft((BuildingTypes)iData, 0);
		case ORDER_CREATE:
			return getProductionTurnsLeft((ProjectTypes)iData, 0);
		case ORDER_MAINTAIN:
			break;
		default: FErrorMsg("Unknown order type");
		}
	}
	return MAX_INT;
}


int CvCity::getProductionTurnsLeft(UnitTypes eUnit, int iNum) const
{
	int iProduction = 0;
	int iFirstUnitOrder = getFirstUnitOrder(eUnit);
	if (iFirstUnitOrder == -1 || iFirstUnitOrder == iNum)
		iProduction += getUnitProduction(eUnit);

	int iProductionNeeded = getProductionNeeded(eUnit);
	int iProductionModifier = getProductionModifier(eUnit);
	// advc.064b: Moved into (yet another) auxiliary function
	return getProductionTurnsLeft(iProductionNeeded, iProduction,
			iProductionModifier, isFoodProduction(eUnit), iNum);
}


int CvCity::getProductionTurnsLeft(BuildingTypes eBuilding, int iNum) const
{
	int iProduction = 0;
	int iFirstBuildingOrder = getFirstBuildingOrder(eBuilding);
	if (iFirstBuildingOrder == -1 || iFirstBuildingOrder == iNum)
		iProduction += getBuildingProduction(eBuilding);

	int iProductionNeeded = getProductionNeeded(eBuilding);
	int iProductionModifier = getProductionModifier(eBuilding);
	// advc.064b:
	return getProductionTurnsLeft(iProductionNeeded, iProduction,
			iProductionModifier, false, iNum);
}


int CvCity::getProductionTurnsLeft(ProjectTypes eProject, int iNum) const
{
	int iProduction = 0;
	int iFirstProjectOrder = getFirstProjectOrder(eProject);
	if (iFirstProjectOrder == -1 || iFirstProjectOrder == iNum)
		iProduction += getProjectProduction(eProject);

	int iProductionNeeded = getProductionNeeded(eProject);
	int iProductionModifier = getProductionModifier(eProject);
	// advc.064b:
	return getProductionTurnsLeft(iProductionNeeded, iProduction,
			iProductionModifier, false, iNum);
}

/*  <advc.064b> New auxiliary function; some code cut from
	getProductionTurnsLeft(UnitTypes,int). Added a bit of code to predict
	overflow for queued orders (iNum>0); BtS never did that. */
int CvCity::getProductionTurnsLeft(int iProductionNeeded, int iProduction,
	int iProductionModifier, bool bFoodProduction, int iNum) const
{
	int iFirstProductionDifference = 0;
	// Per-turn production assuming no overflow and feature production
	int iProductionDifference = getProductionDifference(iProductionNeeded,
			iProduction, iProductionModifier, bFoodProduction, false, true);
	if (iNum > 1)
	{
		// Not going to predict overflow here
		iFirstProductionDifference = iProductionDifference;
	}
	else if (iNum == 1 && getProductionTurnsLeft() <= 1)
	{
		// Current production about to finish; predict overflow.
		OrderTypes eCurrentOrder = getOrderData(0).eOrderType;
		if(eCurrentOrder == ORDER_TRAIN || eCurrentOrder == ORDER_CONSTRUCT ||
			eCurrentOrder == ORDER_CREATE)
		{
			int iFeatureProduction = 0;
			int iCurrentProductionDifference = getCurrentProductionDifference(false,
					true, false, false, false, &iFeatureProduction);
			int iRawOverflow = getProduction() + iCurrentProductionDifference
					- getProductionNeeded();
			int iOverflow = computeOverflow(iRawOverflow, getProductionModifier(),
					eCurrentOrder);
			/*  This ignores that some production modifiers are applied to overflow,
				but it's certainly better than ignoring overflow altogether. */
			iFirstProductionDifference = iProductionDifference + iOverflow +
					((getFeatureProduction() - iFeatureProduction) *
					(100 + iProductionModifier)) / 100;
		}
	}
	else if (iNum == 1)
		iFirstProductionDifference = iProductionDifference;
	if (iNum <= 0)
	{
		iFirstProductionDifference = getProductionDifference(iProductionNeeded,
				iProduction, iProductionModifier, bFoodProduction, true, false);
	}
	return getProductionTurnsLeft(iProductionNeeded, iProduction,
			iFirstProductionDifference, iProductionDifference);
} // </advc.064b>

// <advc.064d> Body cut from popOrder
void CvCity::doPopOrder(CLLNode<OrderData>* pOrder)
{
	bool bStart = false;
	if(pOrder == headOrderQueueNode())
	{
		bStart = true;
		stopHeadOrder();
	}
	m_orderQueue.deleteNode(pOrder);
	if(bStart)
		startHeadOrder();
} // </advc.064d>

int CvCity::getProductionTurnsLeft(int iProductionNeeded, int iProduction, int iFirstProductionDifference, int iProductionDifference) const
{
	int iProductionLeft = std::max(0,
			iProductionNeeded - iProduction - iFirstProductionDifference);

	if (iProductionDifference == 0)
	{
		//return iProductionLeft + 1;
		return MAX_INT; // advc.004x
	}
	int iTurnsLeft = (iProductionLeft / iProductionDifference);
	if (iTurnsLeft * iProductionDifference < iProductionLeft)
		iTurnsLeft++; // advc (comment): rounds up

	iTurnsLeft++;

	return std::max(1, iTurnsLeft);
}

// advc.004x:
int CvCity::sanitizeProductionTurns(int iTurns, OrderTypes eOrder, int iData, bool bAssert) const
{
	if(iTurns < MAX_INT)
		return iTurns;
	FAssert(!bAssert);
	int iR = 1;
	switch(eOrder)
	{
	case ORDER_TRAIN: return iR + getProductionNeeded((UnitTypes)iData);
	case ORDER_CONSTRUCT: return iR + getProductionNeeded((BuildingTypes)iData);
	case ORDER_CREATE: return iR + getProductionNeeded((ProjectTypes)iData);
	case ORDER_MAINTAIN: return getProductionNeeded(); // (mustn't add 1 to that)
	default: return iR + getProductionNeeded();
	}
}


void CvCity::setProduction(int iNewValue)
{
	if (isProductionUnit())
		setUnitProduction(getProductionUnit(), iNewValue);
	else if (isProductionBuilding())
		setBuildingProduction(getProductionBuilding(), iNewValue);
	else if (isProductionProject())
		setProjectProduction(getProductionProject(), iNewValue);
	// <advc.004x>
	m_iMostRecentOrder = -1;
	m_bMostRecentUnit = false; // </advc.004x>
}


void CvCity::changeProduction(int iChange)
{
	if (isProductionUnit())
		changeUnitProduction(getProductionUnit(), iChange);
	else if (isProductionBuilding())
		changeBuildingProduction(getProductionBuilding(), iChange);
	else if (isProductionProject())
		changeProjectProduction(getProductionProject(), iChange);
}


int CvCity::getProductionModifier() const
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionModifier((UnitTypes)(pOrderNode->m_data.iData1));
		case ORDER_CONSTRUCT:
			return getProductionModifier((BuildingTypes)(pOrderNode->m_data.iData1));
		case ORDER_CREATE:
			return getProductionModifier((ProjectTypes)(pOrderNode->m_data.iData1));
		case ORDER_MAINTAIN:
			break;
		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
		}
	}
	return 0;
}


int CvCity::getProductionModifier(UnitTypes eUnit) const
{
	int iMultiplier = GET_PLAYER(getOwner()).getProductionModifier(eUnit);
	iMultiplier += getDomainProductionModifier((DomainTypes)
			GC.getInfo(eUnit).getDomainType());
	if (GC.getInfo(eUnit).isMilitaryProduction())
		iMultiplier += getMilitaryProductionModifier();

	if (GC.getInfo(eUnit).isAnyBonusProductionModifier()) // advc.003t
	{
		FOR_EACH_ENUM(Bonus)
		{
			if (hasBonus(eLoopBonus))
				iMultiplier += GC.getInfo(eUnit).getBonusProductionModifier(eLoopBonus);
		}
	}
	CvPlayer const& kOwner = GET_PLAYER(getOwner());
	if (kOwner.getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(kOwner.getStateReligion()))
			iMultiplier += kOwner.getStateReligionUnitProductionModifier();
	}
	//return std::max(0, iMultiplier);
	return std::max(-50, iMultiplier); // UNOFFICIAL_PATCH (for mods), 05/10/10, jdog5000
}


int CvCity::getProductionModifier(BuildingTypes eBuilding) const
{
	int iMultiplier = GET_PLAYER(getOwner()).getProductionModifier(eBuilding);
	FOR_EACH_NON_DEFAULT_PAIR(GC.getInfo(eBuilding).
		getBonusProductionModifier(), Bonus, int)
	{
		if (hasBonus(perBonusVal.first))
			iMultiplier += perBonusVal.second;
	}
	CvPlayer const& kOwner = GET_PLAYER(getOwner());
	if (kOwner.getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(kOwner.getStateReligion()))
			iMultiplier += kOwner.getStateReligionBuildingProductionModifier();
	}
	// return std::max(0, iMultiplier);
	return std::max(-50, iMultiplier); // UNOFFICIAL_PATCH (for mods), 05/10/10, jdog5000
}


int CvCity::getProductionModifier(ProjectTypes eProject) const
{
	int iMultiplier = GET_PLAYER(getOwner()).getProductionModifier(eProject);
	if (GC.getInfo(eProject).isSpaceship())
		iMultiplier += getSpaceProductionModifier();

	FOR_EACH_NON_DEFAULT_PAIR(GC.getInfo(eProject).
		getBonusProductionModifier(), Bonus, int)
	{
		if (hasBonus(perBonusVal.first))
			iMultiplier += perBonusVal.second;
	}
	// return std::max(0, iMultiplier);
	return std::max(-50, iMultiplier); // UNOFFICIAL_PATCH (for mods), 05/10/10, jdog5000
}


int CvCity::getProductionDifference(int iProductionNeeded, int iProduction,
	int iProductionModifier, bool bFoodProduction, bool bOverflow,  // <advc.064bc>
	bool bIgnoreFeatureProd, bool bIgnoreYieldRate,
	bool bForceFeatureProd, int* piFeatureProd) const // </advc.064bc>
{
	if (isDisorder() /* advc.004x: */ && !bForceFeatureProd)
		return 0;

	int iFoodProduction = (bFoodProduction ?
			std::max(0, getYieldRate(YIELD_FOOD) - foodConsumption(
			/* advc.001 (K-Mod bug): */ true)) : 0);

	int iModifier = getBaseYieldRateModifier(YIELD_PRODUCTION, iProductionModifier);
	int iRate = /* advc.064: */ (bIgnoreYieldRate ? 0 :
			getBaseYieldRate(YIELD_PRODUCTION));
	//int iOverflow = (bOverflow ? (getOverflowProduction() + getFeatureProduction()) : 0);
	// <advc.064b> Replacing the above
	int iOverflow = 0;
	if (bOverflow)
		iOverflow = getOverflowProduction();
	int iFeatureProduction = 0;
	FAssert(!bIgnoreFeatureProd || (!bForceFeatureProd && piFeatureProd == NULL));
	if (!bIgnoreFeatureProd)
	{
		if (bForceFeatureProd)
			iFeatureProduction = getFeatureProduction();
		else /* Compute needed feature production (derived from the formula in the
				return statement) */
		{
			// Don't know what to call this; I need it in order to handle rounding.
			int iTmp = 100 * (iProductionNeeded - iProduction - iFoodProduction) -
					iOverflow * (100 + iProductionModifier);
			int iFeatureProdNeeded =  iTmp / iModifier - iRate;
			if(iTmp % iModifier != 0)
				iFeatureProdNeeded++;
			iFeatureProduction = ::range(iFeatureProdNeeded, 0, getFeatureProduction());
		}
	}
	if(piFeatureProd != NULL)
		*piFeatureProd = iFeatureProduction;
	/*  Replacing the BtS formula below. The BaseYieldRateModifier is now already
		applied when the overflow is generated; see comment in unmodifyOverflow. */
	return iFoodProduction +
			((iRate + iFeatureProduction) * iModifier +
			iOverflow * (100 + iProductionModifier)) / 100;
	// </advc.064b>
	//return ((iRate + iOverflow) * iModifier) / 100 + iFoodProduction;
}


int CvCity::getCurrentProductionDifference(bool bIgnoreFood, bool bOverflow,  // <advc.064bc>
	bool bIgnoreFeatureProd, bool bIgnoreYieldRate,
	bool bForceFeatureProd, int* piFeatureProd) const // </advc.064bc>
{
	return getProductionDifference(getProductionNeeded(), getProduction(),
			getProductionModifier(), !bIgnoreFood && isFoodProduction(), bOverflow,
			bIgnoreFeatureProd, bIgnoreYieldRate, bForceFeatureProd, piFeatureProd);
}


bool CvCity::canHurry(HurryTypes eHurry, bool bTestVisible) const
{
	if(!GET_PLAYER(getOwner()).canHurry(eHurry) &&   // advc.912d:
		(GC.getInfo(eHurry).getProductionPerPopulation() <= 0 || !canPopRush()))
	{
		return false;
	}
	if(isDisorder())
		return false;
	// <advc.064b> Add overflow and features
	if(getCurrentProductionDifference(true, true, false, true, true) + getProduction() +
		minPlotProduction() >= /* </advc.064b> */ getProductionNeeded())
	{
		return false;
	}
	// K-Mod. moved this check outside of !bTestVisible.
	if(!isProductionUnit() && !isProductionBuilding())
		return false;
	if(!bTestVisible)
	{
		// <advc.064b> Only the iHurryGold <= 0 check is new
		if(GC.getInfo(eHurry).getGoldPerProduction() > 0)
		{
			int iHurryGold = hurryGold(eHurry);
			if(iHurryGold <= 0)
				return false;
			if(GET_PLAYER(getOwner()).getGold() < iHurryGold)
				return false;
		} // </advc.064b>
		if(maxHurryPopulation() < hurryPopulation(eHurry))
			return false;
	}
	return true;
}

/*  advc.003j (comment): Not currently called b/c of a K-Mod change in
	CvCityAI::AI_bestUnitAI. Tbd.: I wonder if karadoc's reasoning ("inaccurate")
	also applies to calls to canHurryBuilding. */
bool CvCity::canHurryUnit(HurryTypes eHurry, UnitTypes eUnit, bool bIgnoreNew) const
{
	if (!GET_PLAYER(getOwner()).canHurry(eHurry) &&   // advc.912d
		(GC.getInfo(eHurry).getProductionPerPopulation() <= 0 || !canPopRush()))
	{
		return false;
	}
	if (isDisorder())
		return false;

	if (getUnitProduction(eUnit) >= getProductionNeeded(eUnit))
		return false;

	if (GET_PLAYER(getOwner()).getGold() <
		getHurryGold(eHurry, getHurryCost(false, eUnit, bIgnoreNew)))
	{
		return false;
	}
	if (maxHurryPopulation() <
		getHurryPopulation(eHurry, getHurryCost(true, eUnit, bIgnoreNew)))
	{
		return false;
	}
	return true;
}

bool CvCity::canHurryBuilding(HurryTypes eHurry, BuildingTypes eBuilding, bool bIgnoreNew) const
{
	if (!GET_PLAYER(getOwner()).canHurry(eHurry) &&   // advc.912d
		(GC.getInfo(eHurry).getProductionPerPopulation() <= 0 || !canPopRush()))
	{
		return false;
	}
	if (isDisorder())
		return false;

	if (getBuildingProduction(eBuilding) >= getProductionNeeded(eBuilding))
		return false;

	if (GET_PLAYER(getOwner()).getGold() < getHurryGold(eHurry, getHurryCost(false, eBuilding, bIgnoreNew)))
		return false;

	if (maxHurryPopulation() < getHurryPopulation(eHurry, getHurryCost(true, eBuilding, bIgnoreNew)))
		return false;

	return true;
}


void CvCity::hurry(HurryTypes eHurry)
{
	if (!canHurry(eHurry))
		return;

	int iHurryGold = hurryGold(eHurry);
	int iHurryPopulation = hurryPopulation(eHurry);
	int iHurryAngerLength = hurryAngerLength(eHurry);

	changeProduction(hurryProduction(eHurry));
	GET_PLAYER(getOwner()).changeGold(-(iHurryGold));
	changePopulation(-(iHurryPopulation));

	changeHurryAngerTimer(iHurryAngerLength);

	if (gCityLogLevel >= 2) // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
	{
		CvWStringBuffer szBuffer; CvWString szString;
		if (isProductionUnit())
			szString = GC.getInfo(getProductionUnit()).getDescription();
		else if (isProductionBuilding())
			szString = GC.getInfo(getProductionBuilding()).getDescription();
		else if (isProductionProject())
			szString = GC.getInfo(getProductionProject()).getDescription();
		logBBAI("    City %S hurrying production of %S at cost of %d pop, %d gold, %d anger length", getName().GetCString(), szString.GetCString(), iHurryPopulation, iHurryGold, iHurryAngerLength);
	}

	if (isActiveOwned() && isCitySelected())
		gDLL->UI().setDirty(SelectionButtons_DIRTY_BIT, true);

	CvEventReporter::getInstance().cityHurry(this, eHurry);
}

// <advc.064b>
int CvCity::overflowCapacity(int iProductionModifier, int iPopulationChange) const
{
	int iBound1 = getCurrentProductionDifference(false, false, true); // as in BtS/K-Mod
	// New: Take out build-specific modifiers
	iBound1 = unmodifyOverflow(iBound1, iProductionModifier);
	// Was the production cost of the completed order in BtS/K-Mod
	int iBound2 = growthThreshold(iPopulationChange);
	return std::max(iBound1, iBound2);
}

// Based on code (BtS/unoffical patch) cut from popOrder
int CvCity::computeOverflow(int iRawOverflow, int iProductionModifier,
	OrderTypes eOrderType, int* piProductionGold, int* piLostProduction,
	int iPopulationChange) const
{
	if(piProductionGold != NULL)
		*piProductionGold = 0;
	if(piLostProduction != NULL)
		*piLostProduction = 0;
	/*  (BtS and the unofficial patch cap overflow before factoring out the modifiers.
		However, they also apply generic production modifiers to the overflow once
		work on the next order starts, which AdvCiv no longer does.) */
	int iOverflow = unmodifyOverflow(iRawOverflow, iProductionModifier);
	int iMaxOverflow = overflowCapacity(iProductionModifier, iPopulationChange);
	int iLostProduction = iOverflow - iMaxOverflow;
	iOverflow = std::min(iMaxOverflow, iOverflow);
	if(iLostProduction <= 0)
		return iOverflow;
	if(piLostProduction != NULL)
		*piLostProduction = iLostProduction;
	if(piProductionGold != NULL)
		*piProductionGold = (iLostProduction * failGoldPercent(eOrderType)) / 100;
	return iOverflow;
} // </advc.064b>

// BUG - Hurry Overflow: (advc.064)
bool CvCity::hurryOverflow(HurryTypes eHurry, int* piProduction, int* piGold, bool bCountThisTurn) const
{
	if(piProduction != NULL)
		*piProduction = 0;
	if(piGold != NULL)
		*piGold = 0;

	if(!canHurry(eHurry))
		return false;

	if(GC.getInfo(eHurry).getProductionPerPopulation() == 0)
		return true;

	int iTotal, iCurrent, iModifier;
	OrderTypes eOrder = NO_ORDER; // advc.064b
	if(isProductionUnit())
	{
		UnitTypes eUnit = getProductionUnit();
		iTotal = getProductionNeeded(eUnit);
		iCurrent = getUnitProduction(eUnit);
		iModifier = getProductionModifier(eUnit);
		eOrder = ORDER_TRAIN;
	}
	else if(isProductionBuilding())
	{
		BuildingTypes eBuilding = getProductionBuilding();
		iTotal = getProductionNeeded(eBuilding);
		iCurrent = getBuildingProduction(eBuilding);
		iModifier = getProductionModifier(eBuilding);
		eOrder = ORDER_CONSTRUCT;
	}
	else if (isProductionProject())
	{
		ProjectTypes eProject = getProductionProject();
		iTotal = getProductionNeeded(eProject);
		iCurrent = getProjectProduction(eProject);
		iModifier = getProductionModifier(eProject);
		eOrder = ORDER_CREATE;
	}
	else return false;
	int iHurry = hurryProduction(eHurry);
	// <advc.064b>
	int iRawOverflow = iCurrent + iHurry - iTotal +
			/*  BUG ignores chops and previous overflow if !bCountThisTurn, but these
				are entirely predictable, so they shouldn't depend on the "this turn"
				option. However: Feature production can no longer contribute to
				overflow, so ignore that in any case. */
			getCurrentProductionDifference(!bCountThisTurn, true, true, !bCountThisTurn);
	if (piProduction != NULL)
	{
		*piProduction = computeOverflow(iRawOverflow, iModifier, eOrder, piGold, NULL,
				-hurryPopulation(eHurry)); // </advc.064b>
	}
	return true;
}

// <advc.912d>
bool CvCity::canPopRush() const
{
	return (m_iPopRushHurryCount > 0 || GET_PLAYER(getOwner()).canPopRush());
}

void CvCity::changePopRushCount(int iChange)
{
	m_iPopRushHurryCount += iChange;
} // </advc.912d>


UnitTypes CvCity::getConscriptUnit() const
{
	int iBestValue = 0;
	UnitTypes eBestUnit = NO_UNIT;
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumUnits(); i++)
	{
		UnitTypes eLoopUnit = kCiv.unitAt(i);
		if (canTrain(eLoopUnit))
		{
			int iValue = GC.getInfo(eLoopUnit).getConscriptionValue();
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestUnit = eLoopUnit;
			}
		}
	}

	UnitTypes ePythonUnit = GC.getPythonCaller()->conscriptUnitOverride(getOwner());
	if (ePythonUnit != NO_UNIT)
		eBestUnit = ePythonUnit;

	return eBestUnit;
}


int CvCity::getConscriptPopulation() const
{
	UnitTypes eConscriptUnit = getConscriptUnit();
	if (eConscriptUnit == NO_UNIT)
		return 0;

	if (GC.getDefineINT(CvGlobals::CONSCRIPT_POPULATION_PER_COST) == 0)
		return 0;

	return std::max(1, ((GC.getInfo(eConscriptUnit).getProductionCost()) /
			GC.getDefineINT(CvGlobals::CONSCRIPT_POPULATION_PER_COST)));
}


int CvCity::conscriptMinCityPopulation() const
{
	static int const iCONSCRIPT_MIN_CITY_POPULATION = GC.getDefineINT("CONSCRIPT_MIN_CITY_POPULATION"); // advc.opt
	return getConscriptPopulation() + iCONSCRIPT_MIN_CITY_POPULATION;
}


int CvCity::flatConscriptAngerLength() const
{
	static int const iConscriptAngerDivisor = GC.getDefineINT("CONSCRIPT_ANGER_DIVISOR"); // advc.opt
	int iAnger = iConscriptAngerDivisor;
	iAnger *= GC.getInfo(GC.getGame().getGameSpeedType()).getHurryConscriptAngerPercent();
	iAnger /= 100;

	return std::max(1, iAnger);
}


bool CvCity::canConscript() const
{
	if (isDisorder())
		return false;

	if (isDrafted())
		return false;

	if (GET_PLAYER(getOwner()).getConscriptCount() >= GET_PLAYER(getOwner()).getMaxConscript())
		return false;

	if (getPopulation() <= getConscriptPopulation())
		return false;

	if (getPopulation() < conscriptMinCityPopulation())
		return false;

	static int const iCONSCRIPT_MIN_CULTURE_PERCENT = GC.getDefineINT("CONSCRIPT_MIN_CULTURE_PERCENT"); // advc.opt
	if (getPlot().calculateTeamCulturePercent(getTeam()) < iCONSCRIPT_MIN_CULTURE_PERCENT)
		return false;

	if (getConscriptUnit() == NO_UNIT)
		return false;

	return true;
}

CvUnit* CvCity::initConscriptedUnit()
{
	UnitTypes eConscriptUnit = getConscriptUnit();
	if (eConscriptUnit == NO_UNIT)
		return NULL;

	CvPlayerAI& kOwner = GET_PLAYER(getOwner());

	UnitAITypes eCityAI = NO_UNITAI;
	if (kOwner.AI_unitValue(eConscriptUnit, UNITAI_ATTACK, area()) > 0)
		eCityAI = UNITAI_ATTACK;
	else if (kOwner.AI_unitValue(eConscriptUnit, UNITAI_CITY_DEFENSE, area()) > 0)
		eCityAI = UNITAI_CITY_DEFENSE;
	else if (kOwner.AI_unitValue(eConscriptUnit, UNITAI_CITY_COUNTER, area()) > 0)
		eCityAI = UNITAI_CITY_COUNTER;
	else if (kOwner.AI_unitValue(eConscriptUnit, UNITAI_CITY_SPECIAL, area()) > 0)
		eCityAI = UNITAI_CITY_SPECIAL;
	else eCityAI = NO_UNITAI;

	CvUnit* pUnit = kOwner.initUnit(eConscriptUnit, getX(), getY(), eCityAI);
	FAssertMsg(pUnit != NULL, "Failed to allocate CvUnit object");

	addProductionExperience(pUnit, true);
	pUnit->setMoves(0);
	// K-Mod, 26/Jun/2011: Conscription counts as building the unit
	CvEventReporter::getInstance().unitBuilt(this, pUnit);

	return pUnit;
}


void CvCity::conscript()
{
	if (!canConscript())
		return;

	int const iPopChange = -getConscriptPopulation();
	int const iAngerLength = flatConscriptAngerLength();
	changePopulation(iPopChange);
	changeConscriptAngerTimer(iAngerLength);

	setDrafted(true);

	GET_PLAYER(getOwner()).changeConscriptCount(1);

	CvUnit* pUnit = initConscriptedUnit();
	FAssert(pUnit != NULL);

	if (pUnit != NULL)
	{
		if (isActiveOwned() &&
			isHuman()) // advc.127, advc.706
		{
			/*	advc.001o: Conscription had caused the main map to black out.
				The true cause of that problem may lie elsewhere, but switching
				these two statements is at least a workaround. */
			gDLL->UI().selectUnit(pUnit, true, false, true);
			gDLL->UI().lookAt(getPlot().getPoint(), CAMERALOOKAT_NORMAL); // K-Mod
		}
		if (gCityLogLevel >= 2 && !isHuman()) logBBAI("      City %S does conscript of a %S at cost of %d pop, %d anger", getName().GetCString(), pUnit->getName().GetCString(), iPopChange, iAngerLength); // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
	}
}


int CvCity::getBonusHealth(BonusTypes eBonus) const
{
	int iHealth = GC.getInfo(eBonus).getHealth();
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes const eBuilding = kCiv.buildingAt(i);
		iHealth += getNumActiveBuilding(eBuilding) *
				GC.getInfo(eBuilding).getBonusHealthChanges(eBonus);
	}
	return iHealth;
}


int CvCity::getBonusHappiness(BonusTypes eBonus) const
{
	int iHappiness = GC.getInfo(eBonus).getHappiness();
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes const eBuilding = kCiv.buildingAt(i);
		iHappiness += getNumActiveBuilding(eBuilding) *
				GC.getInfo(eBuilding).getBonusHappinessChanges(eBonus);
	}
	return iHappiness;
}


int CvCity::getBonusPower(BonusTypes eBonus, bool bDirty) const
{
	int iCount = 0;
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes const eBuilding = kCiv.buildingAt(i);
		if (getNumActiveBuilding(eBuilding) > 0 &&
			GC.getInfo(eBuilding).getPowerBonus() == eBonus &&
			GC.getInfo(eBuilding).isDirtyPower() == bDirty)
		{
			iCount += getNumActiveBuilding(eBuilding);
		}
	}
	return iCount;
}


int CvCity::getBonusYieldRateModifier(YieldTypes eYield, BonusTypes eBonus) const
{
	int iModifier = 0;
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes const eBuilding = kCiv.buildingAt(i);
		iModifier += getNumActiveBuilding(eBuilding) *
				GC.getInfo(eBuilding).getBonusYieldModifier(eBonus, eYield);
	}
	return iModifier;
}


void CvCity::processBonus(BonusTypes eBonus, int iChange)
{
	CvCivilization const& kCiv = getCivilization();
	{
		int iValue = GC.getInfo(eBonus).getHealth();
		int iGoodValue = std::max(0, iValue);
		int iBadValue = std::min(0, iValue);
		for (int i = 0; i < kCiv.getNumBuildings(); i++)
		{
			BuildingTypes const eBuilding = kCiv.buildingAt(i);
			iValue = GC.getInfo(eBuilding).getBonusHealthChanges(eBonus) *
					getNumActiveBuilding(eBuilding);
			if (iValue >= 0)
				iGoodValue += iValue;
			else iBadValue += iValue;
		}  // <advc.001w>
		if ((iGoodValue != 0 || iBadValue != 0) && isActiveOwned())
		{
			int iHealth = healthRate();
			if((iHealth < 0) != (iHealth + (iGoodValue - iBadValue) * iChange < 0))
				gDLL->UI().setDirty(CityInfo_DIRTY_BIT, true);
		} // </advc.001w>
		changeBonusGoodHealth(iGoodValue * iChange);
		changeBonusBadHealth(iBadValue * iChange);
	}
	{
		int iValue = GC.getInfo(eBonus).getHappiness();
		int iGoodValue = std::max(0, iValue);
		int iBadValue = std::min(0, iValue);
		for (int i = 0; i < kCiv.getNumBuildings(); i++)
		{
			BuildingTypes const eBuilding = kCiv.buildingAt(i);
			iValue = GC.getInfo(eBuilding).getBonusHappinessChanges(eBonus) *
					getNumActiveBuilding(eBuilding);
			if (iValue >= 0)
				iGoodValue += iValue;
			else iBadValue += iValue;
		}  // <advc.001w>
		if ((iGoodValue != 0 || iBadValue != 0) && isActiveOwned())
		{
			int iHappy = happyLevel() - unhappyLevel();
			if((iHappy < 0) != (iHappy + (iGoodValue - iBadValue) * iChange < 0))
				gDLL->UI().setDirty(CityInfo_DIRTY_BIT, true);
		} // </advc.001w>
		changeBonusGoodHappiness(iGoodValue * iChange);
		changeBonusBadHappiness(iBadValue * iChange);
	}

	changePowerCount(getBonusPower(eBonus, true) * iChange, true);
	changePowerCount(getBonusPower(eBonus, false) * iChange, false);

	FOR_EACH_ENUM(Yield)
	{
		changeBonusYieldRateModifier(eLoopYield,
				getBonusYieldRateModifier(eLoopYield, eBonus) * iChange);
	}
}


void CvCity::processBuilding(BuildingTypes eBuilding, int iChange, bool bObsolete,bool checkKeep)
{
	// <advc>
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	CvGame const& kGame = GC.getGame();
	CvPlayer& kOwner = GET_PLAYER(getOwner());
//tholish-Keldath inactive buildings start
//remeber - update here and in unprocessbuildings new additions!!!
//if the prereq of a building is no more - it will stop providing.
//if you cant keep the building, dont update its stuff.
	// <advc>		
	bool buildingStatus = true;
	bool obsoleteCheck = GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding);
	if (GC.getGame().isOption(GAMEOPTION_BUILDING_DELETION) &&
		!obsoleteCheck &&
		GC.getInfo(eBuilding).getPrereqMustAll() > 0 && 
		getNumRealBuilding(eBuilding) > 0
	)
	{	
		buildingStatus = checkKeep;
	}
	
if (buildingStatus == true)
{
//tholish-Keldath inactive buildings end
	if (!obsoleteCheck || bObsolete)
	{
		if (iChange > 0)
		{
			CorporationTypes eCorporation = kBuilding.getFoundsCorporation();
			if (eCorporation != NO_CORPORATION && !kGame.isCorporationFounded(eCorporation))
				setHeadquarters(eCorporation);
		}

		if (kBuilding.getNoBonus() != NO_BONUS)
			changeNoBonusCount(kBuilding.getNoBonus(), iChange);

		if (kBuilding.getFreeBonus() != NO_BONUS)
		{
			changeFreeBonus(kBuilding.getFreeBonus(),
					kGame.getNumFreeBonuses(eBuilding) * iChange);
		}

		if (kBuilding.getFreePromotion() != NO_PROMOTION)
		{
			changeFreePromotionCount(kBuilding.getFreePromotion(), iChange);
		}
		// <advc.912d>
		if (kGame.isOption(GAMEOPTION_NO_SLAVERY) && kOwner.isHuman() &&
			kBuilding.getHurryAngerModifier() < 0)
		{
			changePopRushCount(iChange);
		}
		changeMaxFoodKeptPercent(//kBuilding.getFoodKept() * iChange
				kOwner.getFoodKept(eBuilding) * iChange); // </advc.912d>	
		changeEspionageDefenseModifier(kBuilding.getEspionageDefenseModifier() * iChange);
		changeGreatPeopleRateModifier(kBuilding.getGreatPeopleRateModifier() * iChange);
		changeFreeExperience(kBuilding.getFreeExperience() * iChange);
		changeMaxAirlift(kBuilding.getAirlift() * iChange);
		changeAirModifier(kBuilding.getAirModifier() * iChange);
		changeAirUnitCapacity(kBuilding.getAirUnitCapacity() * iChange);
		changeNukeModifier(kBuilding.getNukeModifier() * iChange);
		changeFreeSpecialist(kBuilding.getFreeSpecialist() * iChange);
		changeMaintenanceModifier(kBuilding.getMaintenanceModifier() * iChange);
		//DPII < Maintenance Modifiers >
		changeLocalDistanceMaintenanceModifier(kBuilding.getLocalDistanceMaintenanceModifier() * iChange);
		changeLocalCoastalDistanceMaintenanceModifier(kBuilding.getLocalCoastalDistanceMaintenanceModifier() * iChange);
		changeLocalConnectedCityMaintenanceModifier(kBuilding.getLocalConnectedCityMaintenanceModifier() * iChange);
		changeLocalHomeAreaMaintenanceModifier(kBuilding.getLocalHomeAreaMaintenanceModifier() * iChange);
		changeLocalOtherAreaMaintenanceModifier(kBuilding.getLocalOtherAreaMaintenanceModifier() * iChange);
		//DPII < Maintenance Modifiers >
		changeWarWearinessModifier(kBuilding.getWarWearinessModifier() * iChange);
		changeHurryAngerModifier(kBuilding.getHurryAngerModifier() * iChange);
		changeHealRate(kBuilding.getHealRateChange() * iChange);
		/* Population Limit ModComp - Beginning */
		changePopulationLimitChange(kGame.getAdjustedPopulationLimitChange(kBuilding.getPopulationLimitChange()) * iChange);
		/* Population Limit ModComp - End */
		if (kBuilding.getHealth() > 0)
			changeBuildingGoodHealth(kBuilding.getHealth() * iChange);
		else changeBuildingBadHealth(kBuilding.getHealth() * iChange);
		if (kBuilding.getHappiness() > 0)
			changeBuildingGoodHappiness(kBuilding.getHappiness() * iChange);
		else changeBuildingBadHappiness(kBuilding.getHappiness() * iChange);
		if (kBuilding.getReligionType() != NO_RELIGION)
		{
			changeStateReligionHappiness(kBuilding.getReligionType(),
					kBuilding.getStateReligionHappiness() * iChange);
		}
		changeMilitaryProductionModifier(kBuilding.getMilitaryProductionModifier() * iChange);
		changeSpaceProductionModifier(kBuilding.getSpaceProductionModifier() * iChange);
		changeExtraTradeRoutes(kBuilding.getTradeRoutes() * iChange);
		changeTradeRouteModifier(kBuilding.getTradeRouteModifier() * iChange);
		changeForeignTradeRouteModifier(kBuilding.getForeignTradeRouteModifier() * iChange);
		changePowerCount(kBuilding.isPower() ? iChange : 0, kBuilding.isDirtyPower());
		changeGovernmentCenterCount(kBuilding.isGovernmentCenter() ? iChange : 0);
		changeNoUnhappinessCount(kBuilding.isNoUnhappiness() ? iChange : 0);
		//changeNoUnhealthyPopulationCount(kBuilding.isNoUnhealthyPopulation() ? iChange : 0);
		changeUnhealthyPopulationModifier(kBuilding.getUnhealthyPopulationModifier() * iChange); // K-Mod
		changeBuildingOnlyHealthyCount(kBuilding.isBuildingOnlyHealthy() ? iChange : 0);

		FOR_EACH_ENUM2(Yield, y)
		{
			changeSeaPlotYield(y, kBuilding.getSeaPlotYieldChange(y) * iChange);
			changeRiverPlotYield(y, kBuilding.getRiverPlotYieldChange(y) * iChange);
			changeBaseYieldRate(y, (kBuilding.getYieldChange(y) +
					getBuildingYieldChange(kBuilding.getBuildingClassType(), y)) * iChange);
			changeYieldRateModifier(y, kBuilding.getYieldModifier(y) * iChange);
			changePowerYieldRateModifier(y, kBuilding.getPowerYieldModifier(y) * iChange);
			// < Civic Infos Plus Start >
			//keldath QA-DONE
			changeBuildingYieldChange(kBuilding.getBuildingClassType(), y, kOwner.getBuildingYieldChange(eBuilding, y));
			// < Civic Infos Plus End   >
		}

		FOR_EACH_ENUM(Commerce)
		{
			changeCommerceRateModifier(eLoopCommerce,
					kBuilding.getCommerceModifier(eLoopCommerce) * iChange);
			changeCommerceHappinessPer(eLoopCommerce,
					kBuilding.getCommerceHappiness(eLoopCommerce) * iChange);
			// < Civic Infos Plus Start >
			//keldath QA-DONE
			changeBuildingCommerceChange(kBuilding.getBuildingClassType(), eLoopCommerce, 
					kOwner.getBuildingCommerceChange(eBuilding, eLoopCommerce));
			// < Civic Infos Plus End   >
		}
		FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
			getReligionChange(), Religion, int)
		{
			changeReligionInfluence(perReligionVal.first,
					perReligionVal.second * iChange);
		}
		FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
			getSpecialistCount(), Specialist, int)
		{
			changeMaxSpecialistCount(perSpecialistVal.first,
					perSpecialistVal.second * iChange);
		}
		FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
			getFreeSpecialistCount(), Specialist, int)
		{
			changeFreeSpecialistCount(perSpecialistVal.first,
					perSpecialistVal.second * iChange);
		}
		FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
			getImprovementFreeSpecialist(), Improvement, int)
		{
			changeImprovementFreeSpecialists(perImprovementVal.first,
					perImprovementVal.second * iChange);
		}
		FOR_EACH_ENUM(Bonus)
		{
			if (!hasBonus(eLoopBonus))
				continue;
			int const iBonusHealthChange = kBuilding.getBonusHealthChanges(eLoopBonus);
			if (iBonusHealthChange > 0)
				changeBonusGoodHealth(iChange * iBonusHealthChange);
			else changeBonusBadHealth(iChange * iBonusHealthChange);
			int const iBonusHappyChange = kBuilding.getBonusHappinessChanges(eLoopBonus);
			if (iBonusHappyChange > 0)
				changeBonusGoodHappiness(iChange * iBonusHappyChange);
			else changeBonusBadHappiness(iChange * iBonusHappyChange);
			if (kBuilding.getPowerBonus() == eLoopBonus)
				changePowerCount(iChange, kBuilding.isDirtyPower());
			if (kBuilding.getBonusYieldModifier().isAnyNonDefault()) // advc.opt
			{
				FOR_EACH_ENUM(Yield)
				{
					changeBonusYieldRateModifier(eLoopYield, iChange *
							kBuilding.getBonusYieldModifier(eLoopBonus, eLoopYield));
				}
			}
		}

		FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
			getUnitCombatFreeExperience(), UnitCombat, int)
		{
			changeUnitCombatFreeExperience(perUnitCombatVal.first,
					iChange * perUnitCombatVal.second);
		}

		FOR_EACH_ENUM(Domain)
		{
			changeDomainFreeExperience(eLoopDomain, iChange *
					kBuilding.getDomainFreeExperience(eLoopDomain));
			changeDomainProductionModifier(eLoopDomain, iChange *
					kBuilding.getDomainProductionModifier(eLoopDomain));
		}

		updateExtraBuildingHappiness();
		updateExtraBuildingHealth();

		kOwner.changeAssets(iChange * kBuilding.getAssetValue());

		getArea().changePower(getOwner(), iChange * kBuilding.getPowerValue());
		kOwner.changePower(kBuilding.getPowerValue() * iChange);

		for (MemberIter it(getTeam()); it.hasNext(); ++it)
		{
			if (kBuilding.isTeamShare() || it->getID() == getOwner())
				it->processBuilding(eBuilding, iChange, getArea());
		}
		GET_TEAM(getTeam()).processBuilding(eBuilding, iChange);
		GC.getGame().processBuilding(eBuilding, iChange);
	}

	if (!bObsolete)
	{
		/*  <advc.004c> Can avoid an update (which loops over all buildings)
			in the most common circumstances */
		if (kBuilding.getDefenseModifier() != 0 && getBuildingDefense() > 0)
			updateBuildingDefense();
		else // </advc.004c>
			changeBuildingDefense(kBuilding.getDefenseModifier() * iChange);
		// <advc.004c>
		if (kBuilding.get(CvBuildingInfo::RaiseDefense) > 0)
		{
			if (iChange >= 0)
			{
				changeBuildingDefense(std::max(kBuilding.get(CvBuildingInfo::RaiseDefense),
						getBuildingDefense()) - getBuildingDefense());
			}
			else updateBuildingDefense();
		} // </advc.004c>
		changeBuildingBombardDefense(kBuilding.getBombardDefenseModifier() * iChange);
		// advc.051: Moved
		//changeBaseGreatPeopleRate(kBuilding.getGreatPeopleRateChange() * iChange);
		if (kBuilding.getGreatPeopleUnitClass() != NO_UNITCLASS)
		{
			UnitTypes eGreatPeopleUnit = getCivilization().getUnit(
					kBuilding.getGreatPeopleUnitClass());
			if (eGreatPeopleUnit != NO_UNIT)
			{
				changeGreatPeopleUnitRate(eGreatPeopleUnit,
						kBuilding.getGreatPeopleRateChange() * iChange);
				/*  advc.051: Moved from above. Barbarians can obtain wonders, but
					don't have GP units, and shouldn't have a positive base GP rate. */
				changeBaseGreatPeopleRate(kBuilding.getGreatPeopleRateChange() * iChange);
			}
		}
		BuildingClassTypes eBuildingClass = kBuilding.getBuildingClassType();
		GET_TEAM(getTeam()).changeBuildingClassCount(eBuildingClass, iChange);
		kOwner.changeBuildingClassCount(eBuildingClass, iChange);

		kOwner.changeWondersScore(kGame.getWonderScore(
				kBuilding.getBuildingClassType()) * iChange);
		// <advc.004w>
		if (kGame.getCurrentLayer() == GLOBE_LAYER_RESOURCE)
		{
			// Update text of resource indicators (CvGameTextMgr::setBonusExtraHelp)
			PlayerTypes eDirtyPlayer = NO_PLAYER;
			if (kBuilding.isNationalWonder() && isActiveOwned())
				eDirtyPlayer = getOwner();
			else if (kBuilding.isWorldWonder())
				eDirtyPlayer = kGame.getActivePlayer();
			if (eDirtyPlayer != NO_PLAYER)
			{
				gDLL->UI().setDirty(GlobeLayer_DIRTY_BIT, true);
				// advc.003p:
				GET_PLAYER(getOwner()).setBonusHelpDirty();
			}
		} // </advc.004w>
	}
//tholish-Keldath inactive buildings
}	
if (buildingStatus == false)
{
	UNprocessBuilding(eBuilding, iChange, bObsolete);		
}
//tholish-Keldath inactive buildings
	updateBuildingCommerce();
	// < Building Resource Converter Start >
	//frpo1 added a check to avoid infinite loop
	//if (isBuildingBonus(eBuilding)) // f1rpo
	//	processBuildingBonuses();
	// < Building Resource Converter End   >
	setLayoutDirty(true);
}


void CvCity::processProcess(ProcessTypes eProcess, int iChange)
{
	FOR_EACH_ENUM(Commerce)
	{
		changeProductionToCommerceModifier(eLoopCommerce, iChange *
				GC.getInfo(eProcess).getProductionToCommerceModifier(eLoopCommerce));
	}
}


void CvCity::processSpecialist(SpecialistTypes eSpecialist, int iChange)
{
	CvSpecialistInfo const& kSpecialist = GC.getInfo(eSpecialist);
	UnitClassTypes eGPClass = (UnitClassTypes)kSpecialist.getGreatPeopleUnitClass();
	if (eGPClass != NO_UNITCLASS)
	{
		UnitTypes eGreatPeopleUnit = getCivilization().getUnit(eGPClass);
		if (eGreatPeopleUnit != NO_UNIT)
		{
			changeGreatPeopleUnitRate(eGreatPeopleUnit, kSpecialist.getGreatPeopleRateChange() * iChange);
			/*  advc.051: Moved from below. Barbarians don't have GP units; if they
				still end up with a specialist, the base GP rate shouldn't count it. */
			changeBaseGreatPeopleRate(kSpecialist.getGreatPeopleRateChange() * iChange);
		}
	}
	// advc.051: Moved into the block above
	//changeBaseGreatPeopleRate(kSpecialist.getGreatPeopleRateChange() * iChange);

	FOR_EACH_ENUM(Yield)
	{
		changeBaseYieldRate(eLoopYield, iChange *
				kSpecialist.getYieldChange(eLoopYield));
	}
	FOR_EACH_ENUM(Commerce)
	{
		changeSpecialistCommerce(eLoopCommerce, iChange *
				kSpecialist.getCommerceChange(eLoopCommerce));
	}
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**																								**/
	/**																								**/
	/*************************************************************************************************/
	updateSpecialistCivicExtraCommerce();
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/
	updateExtraSpecialistYield();
	changeSpecialistFreeExperience(kSpecialist.getExperience() * iChange);
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**                                                                                              */
/*************************************************************************************************/
	if (kSpecialist.getHealth() > 0)
	{
		changeSpecialistGoodHealth(kSpecialist.getHealth() * iChange);
	}
	else
	{
		changeSpecialistBadHealth(kSpecialist.getHealth() * iChange);
	}
	if (kSpecialist.getHappiness() > 0)
	{
		changeSpecialistHappiness(kSpecialist.getHappiness() * iChange);
	}
	else
	{
		changeSpecialistUnhappiness(kSpecialist.getHappiness() * iChange);
	}
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/

}

// <advc.004c>
void CvCity::updateBuildingDefense()
{
	int iMaxRaise = 0;
	int iModifier = 0;
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes eBuilding = kCiv.buildingAt(i);
		if (getNumBuilding(eBuilding) <= 0)
			continue;
		// (No obsoletion to worry about)
		iMaxRaise = std::max(iMaxRaise, GC.getInfo(eBuilding).get(CvBuildingInfo::RaiseDefense));
		iModifier += GC.getInfo(eBuilding).getDefenseModifier();
	}
	changeBuildingDefense(std::max(iModifier, iMaxRaise) - getBuildingDefense());
} // </advc.004c>


HandicapTypes CvCity::getHandicapType() const
{
	return GET_PLAYER(getOwner()).getHandicapType();
}


CivilizationTypes CvCity::getCivilizationType() const
{
	return GET_PLAYER(getOwner()).getCivilizationType();
}

// advc.003w:
CvCivilization const& CvCity::getCivilization() const
{
	return GET_PLAYER(getOwner()).getCivilization();
}


LeaderHeadTypes CvCity::getPersonalityType() const
{
	return GET_PLAYER(getOwner()).getPersonalityType();
}


ArtStyleTypes CvCity::getArtStyleType() const
{
	//return GET_PLAYER(getOwner()).getArtStyleType();
	// <advc.005f> Replacing the above
	PlayerTypes eArtPlayer = getOwner();
	static bool const bENABLE_005F = GC.getDefineBOOL("ENABLE_005F");
	if(bENABLE_005F)
	{
		PlayerTypes eCultOwner = calculateCulturalOwner();
		if(eCultOwner != NO_PLAYER)
			eArtPlayer = eCultOwner;
		else FAssert(eCultOwner != NO_PLAYER);
	}
	return GET_PLAYER(eArtPlayer).getArtStyleType(); // </advc.005f>
}


CitySizeTypes CvCity::getCitySizeType() const
{
	return (CitySizeTypes)range((getPopulation() / 7), 0, NUM_CITYSIZE_TYPES - 1);
}

const CvArtInfoBuilding* CvCity::getBuildingArtInfo(BuildingTypes eBuilding) const
{
	return GC.getInfo(eBuilding).getArtInfo();
}

float CvCity::getBuildingVisibilityPriority(BuildingTypes eBuilding) const
{
	return GC.getInfo(eBuilding).getVisibilityPriority();
}

bool CvCity::hasTrait(TraitTypes eTrait) const
{
	return GET_PLAYER(getOwner()).hasTrait(eTrait);
}


bool CvCity::isBarbarian() const
{
	return GET_PLAYER(getOwner()).isBarbarian();
}


bool CvCity::isHuman() const
{
	return GET_PLAYER(getOwner()).isHuman();
}


bool CvCity::isVisible(TeamTypes eTeam, bool bDebug) const
{
	return getPlot().isVisible(eTeam, bDebug);
}


bool CvCity::isCapital() const
{
	return (GET_PLAYER(getOwner()).getCapital() == this);
}

// advc: Cut from CvPlot::canTrain
bool CvCity::isPrereqBonusSea() const
{
	FOR_EACH_ADJ_PLOT(getPlot())
	{
		if (pAdj->isWater() &&
			!pAdj->isImpassable() && // advc.041
			pAdj->getArea().isAnyBonus())
		{
			return true;
		}
	}
	return false;
}

bool CvCity::isCoastal(int iMinWaterSize) const
{
	return getPlot().isCoastalLand(iMinWaterSize);
}


bool CvCity::isDisorder() const
{
	return (isOccupation() || GET_PLAYER(getOwner()).isAnarchy());
}

// advc:
bool CvCity::isNoMaintenance() const
{
	return (isDisorder() || isWeLoveTheKingDay() || getPopulation() <= 0);
}


bool CvCity::isHolyCity(ReligionTypes eReligion) const
{
	return (GC.getGame().getHolyCity(eReligion) == this);
}


bool CvCity::isHolyCity() const
{
	FOR_EACH_ENUM(Religion)
	{
		if (isHolyCity(eLoopReligion))
			return true;
	}
	return false;
}


bool CvCity::isHeadquarters(CorporationTypes eCorp) const
{
	return (GC.getGame().getHeadquarters(eCorp) == this);
}

void CvCity::setHeadquarters(CorporationTypes eCorp)
{
	GC.getGame().setHeadquarters(eCorp, this, /* advc.106e: */ false);
	UnitClassTypes eFreeClass = (UnitClassTypes)GC.getInfo(eCorp).getFreeUnitClass();
	if (eFreeClass != NO_UNITCLASS)
	{
		UnitTypes eFreeUnit = getCivilization().getUnit(eFreeClass);
		if (eFreeUnit != NO_UNIT)
			GET_PLAYER(getOwner()).initUnit(eFreeUnit, getX(), getY());
	}
}

bool CvCity::isHeadquarters() const
{
	FOR_EACH_ENUM(Corporation)
	{
		if (isHeadquarters(eLoopCorporation))
			return true;
	}
	return false;
}


int CvCity::getOvercrowdingPercentAnger(int iExtra) const
{
	int iAnger = 0;
	int iOvercrowding = (getPopulation() + iExtra);
	if (iOvercrowding > 0)
	{
		iAnger += (iOvercrowding * GC.getPERCENT_ANGER_DIVISOR()) /
				std::max(1, getPopulation() + iExtra) + 1;
	}
	return iAnger;
}


int CvCity::getNoMilitaryPercentAnger() const
{	// <advc.500b>
	static bool const bDEMAND_BETTER_PROTECTION = GC.getDefineBOOL(
			"DEMAND_BETTER_PROTECTION");
	if (!bDEMAND_BETTER_PROTECTION) // </advc.500b>
	{
		int iAnger = 0;
		if (getMilitaryHappinessUnits() == 0)
		{
			iAnger += //GC.getDefineINT(CvGlobals::NO_MILITARY_PERCENT_ANGER);
					GET_TEAM(getTeam()).getNoMilitaryAnger(); // advc.500c
		}
		return iAnger;
	}
	// <advc.500b>
	static scaled const rPOP_PERCENT = per100(GC.getDefineINT(
			"DEMAND_BETTER_PROTECTION_POP_PERCENT"));
	scaled rTargetGarrStr = getPopulation() * rPOP_PERCENT;
	scaled rActualGarrStr = defensiveGarrison(rTargetGarrStr);
	if (rActualGarrStr >= rTargetGarrStr)
		return 0;
	/* Currently (as per vanilla) 334, meaning 33.4% of the population get angry.
	   The caller adds up all the anger percentages (actually permillages)
	   before rounding, so rounding shouldn't be a concern in this function. */
	int iMaxAnger = GET_TEAM(getTeam()).getNoMilitaryAnger(); // advc.500c
	return iMaxAnger - (iMaxAnger * rActualGarrStr / rTargetGarrStr).floor();
	// </advc.500b>
}


int CvCity::getCulturePercentAnger() const
{
	int iTotalCulture = getPlot().getTotalCulture(); // advc.opt: was countTotalCulture
	if (iTotalCulture == 0)
		return 0;

	int iAngryCulture = 0;
	static int const iCulturePercentAnger = GC.getDefineINT("CULTURE_PERCENT_ANGER"); // advc.opt
	// <advc.099>
	static int const iAngerModCB = GC.getDefineINT("CLOSED_BORDERS_CULTURE_ANGER_MODIFIER");
	static int const iAngerModWar = GC.getDefineINT("AT_WAR_CULTURE_ANGER_MODIFIER");
	for (PlayerIter<EVER_ALIVE,NOT_SAME_TEAM_AS> it(getTeam()); it.hasNext(); ++it)
	{
		CvPlayer const& kRival = *it;
		int iCulture = getPlot().getCulture(kRival.getID());
		if(iCulture <= 0)
			continue;
		int iRelationsModifier = 0;
		if (!kRival.isBarbarian())
		{
			if (kRival.isAlive() && ::atWar(kRival.getTeam(), getTeam()))
				iRelationsModifier += iAngerModWar + iAngerModCB;
			else
			{
				bool const bCapitulatedVassal = (GET_TEAM(kRival.getTeam()).isCapitulated() &&
						GET_TEAM(kRival.getTeam()).isVassal(getTeam()));
				bool const bOB = GET_TEAM(getTeam()).isOpenBorders(kRival.getTeam());
				if ((!bOB && !bCapitulatedVassal) || !kRival.isAlive())
					iRelationsModifier += iAngerModCB;
			}
		}
		iCulture *= std::max(0, iRelationsModifier + 100);
		iCulture /= 100;
		iAngryCulture += iCulture;
	} // </advc.099>
	return (iCulturePercentAnger * iAngryCulture) / iTotalCulture;
}

// <advc.104>
int CvCity::getReligionPercentAnger() const
{
	scaled r = 0;
	for (PlayerIter<CIV_ALIVE,ENEMY_OF> it(getTeam()); it.hasNext(); ++it)
	{
		r += getReligionPercentAnger(it->getID());
	}
	return r.round();
}


scaled CvCity::getReligionPercentAnger(PlayerTypes ePlayer) const
{
	// Replacing BtS code; fewer rounding artifacts.
	CvGame const& kGame = GC.getGame();
	if (kGame.getNumCities() == 0 || getReligionCount() == 0)
		return 0;
	CvPlayer const& kPlayer = GET_PLAYER(ePlayer);
	ReligionTypes eReligion = kPlayer.getStateReligion();
	if (eReligion == NO_RELIGION || !isHasReligion(eReligion))
		return 0;
	scaled rSameReligionCityRatio(kPlayer.getHasReligionCount(eReligion),
			kGame.getNumCities());
	static int const iRELIGION_PERCENT_ANGER = GC.getDefineINT("RELIGION_PERCENT_ANGER", 800); // advc.opt
	scaled rAngerFactor(iRELIGION_PERCENT_ANGER, getReligionCount());
	return rSameReligionCityRatio * rAngerFactor;
} // </advc.104>


int CvCity::getHurryPercentAnger(int iExtra) const
{
	if (getHurryAngerTimer() == 0)
		return 0;
	return 1 + (((((getHurryAngerTimer() - 1) / flatHurryAngerLength()) + 1) *
			GC.getDefineINT(CvGlobals::HURRY_POP_ANGER) * GC.getPERCENT_ANGER_DIVISOR()) /
			std::max(1, getPopulation() + iExtra));
}


int CvCity::getConscriptPercentAnger(int iExtra) const
{
	if (getConscriptAngerTimer() == 0)
		return 0;

	return 1 + (((((getConscriptAngerTimer() - 1) / flatConscriptAngerLength()) + 1) *
			GC.getDefineINT(CvGlobals::CONSCRIPT_POP_ANGER) * GC.getPERCENT_ANGER_DIVISOR()) /
			std::max(1, getPopulation() + iExtra));
}


int CvCity::getDefyResolutionPercentAnger(int iExtra) const
{
	if (getDefyResolutionAngerTimer() == 0)
		return 0;
	// advc.opt:
	static int const iDEFY_RESOLUTION_POP_ANGER = GC.getDefineINT("DEFY_RESOLUTION_POP_ANGER");
	return 1 + (((((getDefyResolutionAngerTimer() - 1) / flatDefyResolutionAngerLength()) + 1) *
			iDEFY_RESOLUTION_POP_ANGER * GC.getPERCENT_ANGER_DIVISOR()) /
			std::max(1, getPopulation() + iExtra));
}


int CvCity::getWarWearinessPercentAnger() const
{
	int iAnger = GET_PLAYER(getOwner()).getWarWearinessPercentAnger();
	iAnger *= std::max(0, getWarWearinessModifier() +
			GET_PLAYER(getOwner()).getWarWearinessModifier() + 100);
	iAnger /= 100;
	return iAnger;
}


int CvCity::getLargestCityHappiness() const
{
	if (findPopulationRank() <= GC.getInfo(GC.getMap().getWorldSize()).getTargetNumCities())
		return GET_PLAYER(getOwner()).getLargestCityHappiness();
	return 0;
}

int CvCity::getVassalHappiness() const
{	// <advc.003n>
	if(isBarbarian())
		return 0; // </advc.003n>
	int iHappy = 0;
	for (TeamIter<ALIVE,VASSAL_OF> it(getTeam()); it.hasNext(); ++it)
	{
		if (!it->isCapitulated()) // advc.142
		{
			iHappy += GC.getDefineINT(CvGlobals::VASSAL_HAPPINESS);
			break; // advc.142
		}
	}
	return iHappy;
}

int CvCity::getVassalUnhappiness() const
{
	// <advc.opt> Replacing loop over "all" masters
	if(GET_TEAM(getTeam()).isAVassal())
		return GC.getDefineINT(CvGlobals::VASSAL_HAPPINESS);
	return 0; // </advc.opt>
}


int CvCity::unhappyLevel(int iExtra) const
{
	if (isNoUnhappiness())
		return 0; // advc

	int iAngerPercent = 0;
	iAngerPercent += getOvercrowdingPercentAnger(iExtra);
	iAngerPercent += getNoMilitaryPercentAnger();
	iAngerPercent += getCulturePercentAnger();
	iAngerPercent += getReligionPercentAnger();
	iAngerPercent += getHurryPercentAnger(iExtra);
	iAngerPercent += getConscriptPercentAnger(iExtra);
	iAngerPercent += getDefyResolutionPercentAnger(iExtra);
	iAngerPercent += getWarWearinessPercentAnger();
	/*  K-Mod, 5/jan/11: global warming anger _percent_; as part per 100.
		Unfortunately, people who made the rest of the game used anger percent to mean part per 1000
		so I have to multiply my GwPercentAnger by 10 to make it fit in. */
	iAngerPercent += std::max(0, GET_PLAYER(getOwner()).getGwPercentAnger()*10);

	FOR_EACH_ENUM(Civic)
	{
		iAngerPercent += GET_PLAYER(getOwner()).getCivicPercentAnger(eLoopCivic);
	}

	int iUnhappiness = (iAngerPercent * (getPopulation() + iExtra)) / GC.getPERCENT_ANGER_DIVISOR();

	iUnhappiness -= std::min(0, getLargestCityHappiness());
	iUnhappiness -= std::min(0, getMilitaryHappiness());
	iUnhappiness -= std::min(0, getCurrentStateReligionHappiness());
	iUnhappiness -= std::min(0, getBuildingBadHappiness());
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**                                                                                              */
/*************************************************************************************************/
	iUnhappiness -= std::min(0, getSpecialistUnhappiness());
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/
	iUnhappiness -= std::min(0, getExtraBuildingBadHappiness());
	iUnhappiness -= std::min(0, getSurroundingBadHappiness());
	iUnhappiness -= std::min(0, getBonusBadHappiness());
	iUnhappiness -= std::min(0, getReligionBadHappiness());
	iUnhappiness -= std::min(0, getCommerceHappiness());
	iUnhappiness -= std::min(0, getArea().getBuildingHappiness(getOwner()));
	iUnhappiness -= std::min(0, GET_PLAYER(getOwner()).getBuildingHappiness());
	iUnhappiness -= std::min(0, (getExtraHappiness() + GET_PLAYER(getOwner()).getExtraHappiness()));
	iUnhappiness -= std::min(0, GC.getInfo(getHandicapType()).getHappyBonus());
	iUnhappiness += std::max(0, getVassalUnhappiness());
	iUnhappiness += std::max(0, getEspionageHappinessCounter());

	return std::max(0, iUnhappiness);
}


int CvCity::happyLevel() const
{
	int iHappiness = 0;
	iHappiness += std::max(0, getLargestCityHappiness());
	iHappiness += std::max(0, getMilitaryHappiness());
	iHappiness += std::max(0, getCurrentStateReligionHappiness());
	iHappiness += std::max(0, getBuildingGoodHappiness());
	iHappiness += std::max(0, getExtraBuildingGoodHappiness());
	iHappiness += std::max(0, getSurroundingGoodHappiness());
	iHappiness += std::max(0, getBonusGoodHappiness());
	iHappiness += std::max(0, getReligionGoodHappiness());
	iHappiness += std::max(0, getCommerceHappiness());
	iHappiness += std::max(0, getArea().getBuildingHappiness(getOwner()));
	iHappiness += std::max(0, GET_PLAYER(getOwner()).getBuildingHappiness());
	iHappiness += std::max(0, (getExtraHappiness() + GET_PLAYER(getOwner()).getExtraHappiness()));
	iHappiness += std::max(0, GC.getInfo(getHandicapType()).getHappyBonus());
	iHappiness += std::max(0, getVassalHappiness());
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**                                                                                              */
/*************************************************************************************************/
	iHappiness += std::max(0, getSpecialistHappiness());
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/

	if (getHappinessTimer() > 0)
	{
		static int const iTEMP_HAPPY = GC.getDefineINT("TEMP_HAPPY"); // advc.opt
		iHappiness += iTEMP_HAPPY;
	}

	return std::max(0, iHappiness);
}


int CvCity::angryPopulation(int iExtra, /* advc.104: */ bool bIgnoreCultureRate) const
{
	return range((unhappyLevel(iExtra) - happyLevel() + (bIgnoreCultureRate ?
			// advc.104: Actually, ignore tales of "villainy" too.
			getCommerceHappiness() - getEspionageHappinessCounter() : 0)),
			0, (getPopulation() + iExtra));
}


int CvCity::visiblePopulation() const
{
	return getPopulation() - angryPopulation() - getWorkingPopulation();
}
//doto enhanced city mylon count pop working limit
int CvCity::extraVisiblePopulationMylon() const
{	
	//int a = getPopulation();
	//int b = angryPopulation();
	//int c = getWorkingPopulation();
	//int d = extraFreeSpecialists();
	if (getWorkingPopulation() > MAX_WORK_TILES)
	{
		return 1;
	}
	return 0;
}

int CvCity::totalFreeSpecialists() const
{
	int iCount = 0;
	if (getPopulation() > 0)
	{
		iCount += getFreeSpecialist();
		iCount += getArea().getFreeSpecialist(getOwner());
		iCount += GET_PLAYER(getOwner()).getFreeSpecialist();
		if (isAnyImprovementFreeSpecialist()) // advc.opt
		{
			FOR_EACH_ENUM(Improvement)
			{
				int iNumSpecialistsPerImprovement = getImprovementFreeSpecialists(
						eLoopImprovement);
				if (iNumSpecialistsPerImprovement > 0)
				{
					iCount += iNumSpecialistsPerImprovement *
							countNumImprovedPlots(eLoopImprovement);
				}
			}
		}
	}
	return iCount;
}


int CvCity::extraPopulation() const
{	
//doto mylon test
	//extraVisiblePopulationMylon();
	return visiblePopulation() + std::min(0, extraFreeSpecialists());
}


int CvCity::extraSpecialists() const
{
	return visiblePopulation() + extraFreeSpecialists();
}


int CvCity::extraFreeSpecialists() const
{
	return totalFreeSpecialists() - getSpecialistPopulation();
}


int CvCity::unhealthyPopulation(bool bNoAngry, int iExtra) const
{
	/*if (isNoUnhealthyPopulation())
		return 0;
	return std::max(0, getPopulation() + iExtra - (bNoAngry ? angryPopulation(iExtra) : 0));*/ // BtS
	// K-Mod, 27/dec/10: replaced with UnhealthyPopulationModifier
	int iUnhealth = getPopulation() + iExtra - (bNoAngry ? angryPopulation(iExtra) : 0);
	iUnhealth *= std::max(0, 100 + getUnhealthyPopulationModifier());
	return intdiv::uround(std::max(0, iUnhealth), 100);
}


int CvCity::totalGoodBuildingHealth() const
{
	return (getBuildingGoodHealth() + getArea().getBuildingGoodHealth(getOwner()) +
			GET_PLAYER(getOwner()).getBuildingGoodHealth() + getExtraBuildingGoodHealth());
}


int CvCity::totalBadBuildingHealth() const
{
	if (!isBuildingOnlyHealthy())
	{
		return (getBuildingBadHealth() + getArea().getBuildingBadHealth(getOwner()) +
				GET_PLAYER(getOwner()).getBuildingBadHealth() + getExtraBuildingBadHealth());
	}

	return 0;
}


int CvCity::goodHealth() const
{
	int iTotalHealth = 0;
	int iHealth = getFreshWaterGoodHealth();
	if (iHealth > 0)
		iTotalHealth += iHealth;

	iHealth = getSurroundingGoodHealth();
	if (iHealth > 0)
		iTotalHealth += iHealth;

/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: Enable Terrain Health Modifiers                                                  **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
	iHealth = getTerrainGoodHealth();
	if (iHealth > 0)
	{
		iTotalHealth += iHealth;
	}
/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**                                                                                              */
/*************************************************************************************************/
	iHealth = getSpecialistGoodHealth();
	if (iHealth > 0)
	{
		iTotalHealth += iHealth;
	}
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/
// < Civic Infos Plus Start >
	iHealth = getReligionGoodHealth();
	if (iHealth > 0)
	{
		iTotalHealth += iHealth;
	}
//getNonStateReligionExtraHealth unmarked by keldath from v104
	iHealth = GET_PLAYER(getOwner()).getNonStateReligionExtraHealth();
	if (iHealth > 0)
	{
		iTotalHealth += iHealth;
	}
	// < Civic Infos Plus End   >
	iHealth = getPowerGoodHealth();
	if (iHealth > 0)
		iTotalHealth += iHealth;

	iHealth = getBonusGoodHealth();
	if (iHealth > 0)
		iTotalHealth += iHealth;

	iHealth = totalGoodBuildingHealth();
	if (iHealth > 0)
		iTotalHealth += iHealth;

	iHealth = GET_PLAYER(getOwner()).getExtraHealth() + getExtraHealth();
	if (iHealth > 0)
		iTotalHealth += iHealth;

	iHealth = GC.getInfo(getHandicapType()).getHealthBonus();
	if (iHealth > 0)
		iTotalHealth += iHealth;

	return iTotalHealth;
}


int CvCity::badHealth(bool bNoAngry, int iExtra) const
{
	int iTotalHealth = 0;
	int iHealth = getEspionageHealthCounter();
	if (iHealth > 0)
		iTotalHealth -= iHealth;

	iHealth = getFreshWaterBadHealth();
	if (iHealth < 0)
		iTotalHealth += iHealth;

	iHealth = getSurroundingBadHealth();
	if (iHealth < 0)
		iTotalHealth += iHealth;

/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: Enable Terrain Health Modifiers                                                  **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
	iHealth = getTerrainBadHealth();
	if (iHealth > 0)
	{
		iTotalHealth += iHealth;
	}

/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**                                                                                              */
/*************************************************************************************************/
	iHealth = getSpecialistBadHealth();
	if (iHealth < 0)
	{
		iTotalHealth += iHealth;
	}
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/
	iHealth = getPowerBadHealth();
	if (iHealth < 0)
		iTotalHealth += iHealth;
 // < Civic Infos Plus Start >
// unmarked by keldath from v104	
	iHealth = GET_PLAYER(getOwner()).getStateReligionExtraHealth();
	if (iHealth < 0)
	{
		iTotalHealth += iHealth;
	}
	iHealth = GET_PLAYER(getOwner()).getNonStateReligionExtraHealth();
	if (iHealth < 0)
	{
		iTotalHealth += iHealth;
	}
// < Civic Infos Plus End   >
	iHealth = getBonusBadHealth();
	if (iHealth < 0)
		iTotalHealth += iHealth;

	iHealth = totalBadBuildingHealth();
	if (iHealth < 0)
		iTotalHealth += iHealth;

	iHealth = GET_PLAYER(getOwner()).getExtraHealth() + getExtraHealth();
	if (iHealth < 0)
		iTotalHealth += iHealth;

	iHealth = GC.getInfo(getHandicapType()).getHealthBonus();
	if (iHealth < 0)
		iTotalHealth += iHealth;
	/*	advc.001 (from Better BUG AI, fix by Fuyu):
		Already counted by totalBadBuildingHealth. */
	/*iHealth = getExtraBuildingBadHealth();
	if (iHealth < 0)
		iTotalHealth += iHealth;*/

	return (unhealthyPopulation(bNoAngry, iExtra) - iTotalHealth);
}


int CvCity::healthRate(bool bNoAngry, int iExtra) const
{
	return std::min(0, (goodHealth() - badHealth(bNoAngry, iExtra)));
}


int CvCity::foodConsumption(bool bNoAngry, int iExtra) const
{
	return ((getPopulation() + iExtra - (bNoAngry ? angryPopulation(iExtra) : 0)) *
			GC.getFOOD_CONSUMPTION_PER_POPULATION()) - healthRate(bNoAngry, iExtra);
}


int CvCity::foodDifference(bool bBottom, bool bIgnoreProduction) const
{
	if (isDisorder())
		return 0;

	int iDifference;
	//if (isFoodProduction())
	if (!bIgnoreProduction && isFoodProduction()) // K-Mod
		iDifference = std::min(0, getYieldRate(YIELD_FOOD) - foodConsumption());
	else iDifference = (getYieldRate(YIELD_FOOD) - foodConsumption());

	if (bBottom)
	{
		if (getPopulation() == 1 && getFood() == 0)
			iDifference = std::max(0, iDifference);
	}

	return iDifference;
}


int CvCity::growthThreshold(/* advc.064b: */ int iPopulationChange) const
{
	return (GET_PLAYER(getOwner()).getGrowthThreshold(getPopulation()
			+ iPopulationChange)); // advc.064b
}


int CvCity::getHurryCostModifier(bool bIgnoreNew) const
{
	int iModifier = 100;
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		switch (pOrderNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			iModifier = getHurryCostModifier((UnitTypes)
					pOrderNode->m_data.iData1, bIgnoreNew);
			break;
		case ORDER_CONSTRUCT:
			iModifier = getHurryCostModifier((BuildingTypes)
					pOrderNode->m_data.iData1, bIgnoreNew);
			break;
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
			break;
		default:
			FErrorMsg("pOrderNode->m_data.eOrderType did not match a valid option");
		}
	}
	return iModifier;
}


int CvCity::getHurryCostModifier(UnitTypes eUnit, bool bIgnoreNew) const
{
	return getHurryCostModifier(GC.getInfo(eUnit).getHurryCostModifier(),
			getUnitProduction(eUnit), bIgnoreNew);
}


int CvCity::getHurryCostModifier(BuildingTypes eBuilding, bool bIgnoreNew) const
{
	return getHurryCostModifier(GC.getInfo(eBuilding).getHurryCostModifier(),
			getBuildingProduction(eBuilding), bIgnoreNew);
}


int CvCity::getHurryCostModifier(int iBaseModifier, int iProduction, bool bIgnoreNew) const
{
	int iModifier = 100;
	iModifier *= std::max(0, iBaseModifier + 100);
	iModifier /= 100;
	if (iProduction == 0 && !bIgnoreNew)
	{
		static int const iNEW_HURRY_MODIFIER = GC.getDefineINT("NEW_HURRY_MODIFIER"); // advc.opt
		iModifier *= std::max(0, iNEW_HURRY_MODIFIER + 100);
		iModifier /= 100;
	}
	iModifier *= std::max(0, GET_PLAYER(getOwner()).getHurryModifier() + 100);
	iModifier /= 100;
	return iModifier;
}


int CvCity::getHurryCost(bool bExtra, UnitTypes eUnit, bool bIgnoreNew) const
{
	int iProductionLeft = getProductionNeeded(eUnit) - getUnitProduction(eUnit);
	return getHurryCost(bExtra, iProductionLeft, getHurryCostModifier(eUnit, bIgnoreNew),
			getProductionModifier(eUnit));
}


int CvCity::getHurryCost(bool bExtra, BuildingTypes eBuilding, bool bIgnoreNew) const
{
	int iProductionLeft = getProductionNeeded(eBuilding) - getBuildingProduction(eBuilding);
	return getHurryCost(bExtra, iProductionLeft, getHurryCostModifier(eBuilding, bIgnoreNew),
			getProductionModifier(eBuilding));
}

int CvCity::getHurryCost(bool bExtra, int iProductionLeft, int iHurryModifier, int iModifier) const
{
	// <advc.064b>
	if (iProductionLeft == MAX_INT)
		return 0;
	iProductionLeft -= getCurrentProductionDifference(bExtra, true, false, bExtra, true);
	if(bExtra) // City yield rate uncertain if pop is sacrificed (bExtra) ...
	{	// ... but city production is going to be at least 1
		iProductionLeft -= minPlotProduction();
	}
	if(iProductionLeft <= 0)
		return 0;
	// </advc.064b>
	int iProduction = intdiv::uceil(iProductionLeft * iHurryModifier, 100);

	if (bExtra)
	{
		/*int iExtraProduction = getExtraProductionDifference(iProduction, iModifier);
		if (iExtraProduction > 0)
			iProduction = intdiv::uceil(SQR(iProduction), iExtraProduction);*/
		/*	<advc.001> Same as above, but rounds more intuitively, e.g.
			when iProduction=45 and iModifier=50 (settler at 55/100 production with
			Imperialistic trait). Based on a similar change in the CloseToHome mod. */
		iProduction = getInverseProductionDifference(iProduction,
				/*  advc.064c (note): Passing 0 here instead of iModifier would
					only apply generic modifiers */
				iModifier); // </advc.001>
	}

	return std::max(0, iProduction);
}


int CvCity::getHurryGold(HurryTypes eHurry, int iHurryCost) const
{
	if (GC.getInfo(eHurry).getGoldPerProduction() == 0)
		return 0;
	int iGold = iHurryCost * GC.getInfo(eHurry).getGoldPerProduction();
	return std::max(0, iGold); // advc.064b: lower bound was 1
}


int CvCity::getHurryPopulation(HurryTypes eHurry, int iHurryCost) const
{
	if (GC.getInfo(eHurry).getProductionPerPopulation() == 0)
		return 0;
	int iPopulation = (iHurryCost - 1) / GC.getGame().getProductionPerPopulation(eHurry);
	return std::max(1, iPopulation + 1);
}

int CvCity::hurryProduction(HurryTypes eHurry) const
{
	bool bPopRush = (GC.getInfo(eHurry).getProductionPerPopulation() > 0);
	//int iProductionNeeded = productionLeft();
	// <advc.064b> ^Don't always need to generate that much production
	/*  If pop is to be removed, we can't rely on tile yields. (However, in that case,
		iProductionNeeded is only used for the assertion at the end. There's similar
		code for pop rushing in CvCity::getHurryCost.) */
	int iProductionDifference = getCurrentProductionDifference(bPopRush, true, false,
			bPopRush, true);
	if(bPopRush)
		iProductionDifference += minPlotProduction();
	int iProductionNeeded = std::max(0, getProductionNeeded() - getProduction() -
			iProductionDifference);
	// </advc.064b>
	if (!bPopRush)
		return iProductionNeeded;

	int iProduction = (100 * getExtraProductionDifference(hurryPopulation(eHurry) *
			GC.getGame().getProductionPerPopulation(eHurry)
			/*  advc.064c (comment): Passing 0 as a second arg would mean that only
				generic modifiers apply */
			)) / std::max(1, getHurryCostModifier());
	FAssert(iProduction >= iProductionNeeded);
	return iProduction;
}


int CvCity::flatHurryAngerLength() const
{
	int iAnger = GC.getGame().getHurryAngerLength();
	iAnger *= std::max(0, 100 + getHurryAngerModifier());
	iAnger /= 100;
	return std::max(1, iAnger);
}


int CvCity::hurryAngerLength(HurryTypes eHurry) const
{
	if (GC.getInfo(eHurry).isAnger())
		return flatHurryAngerLength();
	return 0;
}


int CvCity::maxHurryPopulation() const
{
	return std::min(3, // advc.064c: Allow at most 3 pop to be sacrificed at once
			getPopulation() / 2);
}


int CvCity::cultureDistance(int iDX, int iDY)
{
	return std::max(1, plotDistance(0, 0, iDX, iDY));
}

// advc.101: Replaced most of the code, but it's still the same structure as in BtS.
int CvCity::cultureStrength(PlayerTypes ePlayer,
	std::vector<GrievanceTypes>* paGrievances) const // Out parameter for UI support
{
	//int iStrength = 1 + getHighestPopulation() * 2; // BtS
	scaled rPopulation = getPopulation();
	CvGame const& kGame = GC.getGame();
	CvGameSpeedInfo const& kSpeed = GC.getInfo(kGame.getGameSpeedType());
	int iTimeOwned = kGame.getGameTurn() - getGameTurnAcquired();
	scaled rTimeRatio = iTimeOwned / scaled::max(1,
			fixp(0.75) * kSpeed.getGoldenAgePercent());
	scaled rTimeFactor = scaled::min(1, rTimeRatio);
	rPopulation += // Gradually shift to highest pop, then back to actual pop.
			(fixp(0.5) - (rTimeFactor - fixp(0.5)).abs()) *
			(getHighestPopulation() - rPopulation);
	scaled rEraFactor; // BtS used the game era here
	{
		/*	ePlayer can be dead, but the tech count should still be good.
			(CvPlayer::getTechScore / CvGame::getMaxTech would give
			too much weight to late-game tech.) */
		scaled rPlayerTechPercent(GET_TEAM(ePlayer).getTechCount(), GC.getNumTechInfos());
		scaled rOwnerTechPercent(GET_TEAM(getOwner()).getTechCount(), GC.getNumTechInfos());
		scaled rTechPercent = std::max(rPlayerTechPercent,
				((rTimeFactor + 1) / 2) * rOwnerTechPercent);
		// Decrease resolution - to avoid frequent changes in revolt chance
		int const iGrain = 10;
		int iTechPercentQuantized = intdiv::uround(rTechPercent.getPercent(), iGrain);
		if (iTechPercentQuantized > 0)
		{
			rEraFactor = iTechPercentQuantized * scaled(GC.getNumEraInfos() - 1, iGrain);
			rEraFactor.exponentiate(fixp(4/3.));
		}
		rEraFactor++;
	}
	CvPlot const& kCityPlot = getPlot();
	bool bCanFlip = (canCultureFlip(ePlayer, false) &&
			ePlayer == kCityPlot.calculateCulturalOwner());
	scaled rStrengthFromInnerRadius;
	CvPlayer const& kOwner = GET_PLAYER(getOwner());
	// <advc.099c>
	if (ePlayer == BARBARIAN_PLAYER)
		rEraFactor /= 2; // </advc.099c>
	int iStolenInner = 0;
	FOR_EACH_ADJ_PLOT(getPlot())
	{
		// <advc.035>
		PlayerTypes eLoopOwner = pAdj->getOwner();
		if (GC.getDefineBOOL(CvGlobals::OWN_EXCLUSIVE_RADIUS) && eLoopOwner != NO_PLAYER &&
			!GET_TEAM(eLoopOwner).isAtWar(kOwner.getTeam()))
		{
			PlayerTypes const eSecondOwner = pAdj->getSecondOwner();
			if(eSecondOwner != eLoopOwner) // Checked only for easier debugging
				eLoopOwner = eSecondOwner;
		} // </advc.035>
		if (bCanFlip &&
			eLoopOwner == ePlayer) // advc.035
		{
			rStrengthFromInnerRadius += rEraFactor * scaled::clamp(
					rTimeRatio - fixp(2/3.), 0, fixp(3.5));
			iStolenInner++;
		}
		scaled rCap = fixp(0.25) + fixp(0.75) * rTimeFactor;
		rStrengthFromInnerRadius += rEraFactor * scaled::clamp(per100(
				pAdj->calculateCulturePercent(ePlayer) -
				pAdj->calculateCulturePercent(getOwner())), 0, rCap);
	}
	/*	Don't make cities that stay small for lack of workable tiles
		too easy to hold onto */
	rPopulation.increaseTo(scaled(iStolenInner, 2));
	// To put a cap on the early-game revolt chance in large cities:
	rPopulation.decreaseTo(rEraFactor * fixp(5/3.));
	scaled rStrength = 1 + 2 * rPopulation;
	/*	Era should also affect strength from population
		(only affects strength from surrounding tiles in BtS). */
	rStrength *= scaled::max(2, rEraFactor + 1).sqrt() / 2;
	rStrengthFromInnerRadius /= fixp(1.5); // To even out the change above
	if (!isBarbarian()) // Put engulfed Barbarian cities out of their misery
		rStrengthFromInnerRadius.decreaseTo(fixp(5/3.) * rStrength);
	rStrength += rStrengthFromInnerRadius;
	int const iHurryAnger = (getHurryPercentAnger() * getPopulation()) / 1000;
	FAssert((iHurryAnger == 0) == (getHurryAngerTimer() == 0));
	int const iConscriptAnger = (getConscriptPercentAnger() * getPopulation()) / 3000;
	int iAngry = iHurryAnger + iConscriptAnger;
	iAngry = std::min(iAngry, getPopulation());
	/*  HurryAnger also factors into GrievanceModifier below, but for small cities
		(where Slavery is most effective), this constant bonus matters more. */
	rStrength += 3 * iAngry;
	/*  K-Mod, 7/jan/11, karadoc
		changed so that culture strength asymptotes as the attacking culture approaches 100% */
	//iStrength *= std::max(0, (GC.getDefineINT("REVOLT_TOTAL_CULTURE_MODIFIER") * (kCityPlot.getCulture(ePlayer) - kCityPlot.getCulture(getOwner()))) / (kCityPlot.getCulture(getOwner()) + 1) + 100); //  K-Mod end
	// Restored BtS formula (more or less)
	rStrength *= scaled::max(0, 1 + scaled(
			/*  Don't like the multiplicative interaction between this and the grievances;
				now added to grievances. */
			//GC.getDefineINT("REVOLT_TOTAL_CULTURE_MODIFIER") *
			kCityPlot.getCulture(ePlayer) - kCityPlot.getCulture(getOwner()),
			kCityPlot.getCulture(ePlayer)));
	// New: Reduce strength if far less culture than some third party
	scaled rThirdPartyModifier = fixp(8/5.) * scaled(
			kCityPlot.calculateTeamCulturePercent(TEAMID(ePlayer)),
			kCityPlot.calculateTeamCulturePercent(kCityPlot.findHighestCultureTeam()));
	if(rThirdPartyModifier < 1)
		rStrength *= rThirdPartyModifier;
	// Also/ further reduce strength if owner has almost as much culture as ePlayer
	scaled rSecondPartyModifier(
			kCityPlot.getCulture(ePlayer),
			std::max(1, kCityPlot.getCulture(getOwner())));
	rSecondPartyModifier--;
	rSecondPartyModifier.clamp(0, 1);
	rStrength *= rSecondPartyModifier;

	/*	Culture strength too low in the late game - or garrison strength too high, but
		garrison strength is what players can directly control; needs to be simple, linear.
		Make this adjustment before applying grievance modifiers (or any other modifiers
		that the UI communicates to the player). */
	static scaled const rFOREIGN_CULTURE_STRENGTH_EXPONENT = per100(
			GC.getDefineINT("FOREIGN_CULTURE_STRENGTH_EXPONENT"));
	rStrength.exponentiate(rFOREIGN_CULTURE_STRENGTH_EXPONENT);
	rStrength *= per100(GC.getInfo(kOwner.getHandicapType()).get(
			CvHandicapInfo::ForeignCultureStrength));

	static scaled const rREVOLT_OFFENSE_STATE_RELIGION_MODIFIER = per100(
			GC.getDefineINT("REVOLT_OFFENSE_STATE_RELIGION_MODIFIER"));
	static scaled const rREVOLT_DEFENSE_STATE_RELIGION_MODIFIER = per100(
			GC.getDefineINT("REVOLT_DEFENSE_STATE_RELIGION_MODIFIER"));
	static scaled const rREVOLT_TOTAL_CULTURE_MODIFIER = per100(
			GC.getDefineINT("REVOLT_TOTAL_CULTURE_MODIFIER")); // 100
	scaled rGrievanceModifier;
	ReligionTypes const eOwnerStateReligion = kOwner.getStateReligion();
	CvPlayer const& kPlayer = GET_PLAYER(ePlayer);
	if (kPlayer.isAlive() &&
		(!GET_TEAM(kPlayer.getTeam()).isCapitulated() ||
		kPlayer.getMasterTeam() != getTeam()) &&
		kPlayer.getStateReligion() != NO_RELIGION &&
		isHasReligion(kPlayer.getStateReligion()))
	{
		/*iStrength *= std::max(0, GC.getDefineINT("REVOLT_OFFENSE_STATE_RELIGION_MODIFIER") + 100);
		iStrength /= 100;*/ // BtS
		rGrievanceModifier += rREVOLT_OFFENSE_STATE_RELIGION_MODIFIER;
	}
	else
	{
		/*  Religion offense might still make sense even when ePlayer is defated, but
			can't expect the owner to memorize the state religions of defeated players.
			Instead, count some religion offense also when the owner's state religion
			is absent and at least one religion is in the city. This makes (some) sense
			whether ePlayer is defeated or not. */
		if(eOwnerStateReligion != NO_RELIGION && !isHasReligion(eOwnerStateReligion))
		{
			FOR_EACH_ENUM(Religion)
			{
				if(isHasReligion(eLoopReligion) && eLoopReligion != eOwnerStateReligion)
				{
					rGrievanceModifier += rREVOLT_OFFENSE_STATE_RELIGION_MODIFIER * fixp(2/3.);
					break;
				}
			}
		}
	}
	if(eOwnerStateReligion != NO_RELIGION && isHasReligion(eOwnerStateReligion))
	{
		/*iStrength *= std::max(0, GC.getDefineINT("REVOLT_DEFENSE_STATE_RELIGION_MODIFIER") + 100);
		iStrength /= 100;*/ // BtS
		/*  The OFFENSE modifier was originally set to 100 in XML and
			DEFENSE to -50, and they were cancelling out when both applied
			(multiplication by 100+100 and then by 100-50). I'm changing the values
			in XML so that they cancel out when added up. */
		rGrievanceModifier += rREVOLT_DEFENSE_STATE_RELIGION_MODIFIER;
	} /* No state religion is still better than some oppressive state religion that
		 the city doesn't share. */
	else if(eOwnerStateReligion == NO_RELIGION && rGrievanceModifier > 0)
		rGrievanceModifier += rREVOLT_DEFENSE_STATE_RELIGION_MODIFIER * fixp(3/5.);
	if (paGrievances != NULL && rGrievanceModifier > 0)
		paGrievances->push_back(GRIEVANCE_RELIGION);
	rGrievanceModifier += rREVOLT_TOTAL_CULTURE_MODIFIER - 1;
	if (iAngry > 0)
	{
		rGrievanceModifier += scaled(iAngry).sqrt() /
				(per100(kSpeed.getHurryConscriptAngerPercent()) + 1);
		if (paGrievances != NULL)
		{
			if (iHurryAnger > 0)
				paGrievances->push_back(GRIEVANCE_HURRY);
			if (iConscriptAnger > 0)
				paGrievances->push_back(GRIEVANCE_CONSCRIPT);
		}
	} // <advc.099c>
	if(GET_TEAM(getTeam()).isCapitulated() && bCanFlip)
		rGrievanceModifier += 1; // </advc.099c>
	// The defense modifier should only halve culture strength, not reduce by 100%
	if(rGrievanceModifier < 0)
		rGrievanceModifier /= 2;
	rStrength *= (1 + scaled::max(-1, rGrievanceModifier));

	return rStrength.round();
}

// advc.101: Changed entirely. The parameter is no longer used.
int CvCity::cultureGarrison(PlayerTypes ePlayer) const
{
	/*  BtS makes Barbarian units ineligible as culture garrison
		through CvPlot::doCulture (now renamed to CvPlot::doRevolts).
		Easier to do it here -- but I do want them to be eligible. */
	/*if(isBarbarian())
		return 0;*/
	int iGarrison = 0; // was 1
	FOR_EACH_UNIT_IN(pUnit, getPlot())
	{
		if (pUnit->getTeam() == getTeam()) // advc.184: Had been counting all units
			iGarrison += pUnit->garrisonStrength();
	}
	/*if (atWar(TEAMID(ePlayer), getTeam()))
		iGarrison *= 2;*/ // advc.023: commented out
	return intdiv::uround(iGarrison, 100);
}

// advc.099c:
PlayerTypes CvCity::calculateCulturalOwner() const
{
	return getPlot().calculateCulturalOwner(GC.getDefineBOOL(CvGlobals::REVOLTS_IGNORE_CULTURE_RANGE));
}


int CvCity::getNumBuilding(BuildingTypes eBuilding) const
{
	return std::min(GC.getDefineINT(CvGlobals::CITY_MAX_NUM_BUILDINGS),
			getNumRealBuilding(eBuilding) + getNumFreeBuilding(eBuilding));
}

// advc.003w:
int CvCity::getNumBuilding(BuildingClassTypes eBuildingClass) const
{
	BuildingTypes eBuilding = getCivilization().getBuilding(eBuildingClass);
	if (eBuilding == NO_BUILDING)
		return 0;
	return getNumBuilding(eBuilding);
}


int CvCity::getNumActiveBuilding(BuildingTypes eBuilding) const
{
	if (GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding))
		return 0;
	return getNumBuilding(eBuilding);
}

// UNOFFICIAL_PATCH, War tactics AI, 03/04/10, Mongoose & jdog5000:
int CvCity::getNumActiveWorldWonders(int iStopCountAt, // advc
	PlayerTypes eOwner) const // advc.104d
{
	PROFILE_FUNC(); // advc.opt (tbd.): Cache the return value
	// advc.104d:
	CvTeam const& kObsTeam = GET_TEAM(eOwner == NO_PLAYER ? getTeam() : TEAMID(eOwner));

	int iCount = 0;
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes const eBuilding = kCiv.buildingAt(i);
		if (GC.getInfo(eBuilding).isWorldWonder())
		{
			if (getNumRealBuilding(eBuilding) > 0 &&
				!kObsTeam.isObsoleteBuilding(eBuilding)) // advc.104d
			{
				iCount++;
				// <advc> So that hasActiveWorldWonder can defer to getNumActiveWorldWonders
				if (iCount >= iStopCountAt)
					return iCount; // </advc>
			}
		}
	}
	return iCount;
}

// advc:
CvCity* CvCity::fromIDInfo(IDInfo id)
{
	return ::getCity(id);
}


void CvCity::setID(int iID)
{
	m_iID = iID;
}

// advc.opt: Update cached CvPlot, CvArea pointer and plot index.
void CvCity::updatePlot()
{
	m_pPlot = GC.getMap().plotSoren(getX(), getY());
	if (m_pPlot != NULL)
		m_ePlot = GC.getMap().plotNum(getX(), getY());
	updateArea();
}


CvPlotGroup* CvCity::plotGroup(PlayerTypes ePlayer) const
{
	return getPlot().getPlotGroup(ePlayer);
}


bool CvCity::isConnectedTo(CvCity const& kCity) const
{
	return getPlot().isConnectedTo(kCity);
}


bool CvCity::isConnectedToCapital(PlayerTypes ePlayer) const
{
	return getPlot().isConnectedToCapital(ePlayer);
}

// advc: For handling area recalculation (after a WorldBuilder terrain change)
void CvCity::updateArea()
{
	m_pArea = (plot() == NULL ? NULL :getPlot().area());
}

// BETTER_BTS_AI_MOD, General AI, 01/02/09, jdog5000: param added
CvArea* CvCity::waterArea(bool bNoImpassable) const
{
	return getPlot().waterArea(bNoImpassable);
}

// BETTER_BTS_AI_MOD, General AI, 01/02/09, jdog5000: Expose plot function through city
CvArea* CvCity::secondWaterArea() const
{
	return getPlot().secondWaterArea();
}

// advc.003j (comment): Unused; may or may not work correctly.
// BETTER_BTS_AI_MOD, General AI, 01/02/09, jdog5000: START
// Find the largest water area shared by this city and other city, if any
CvArea* CvCity::sharedWaterArea(CvCity* pOtherCity) const
{
	CvArea* pWaterArea = waterArea(true);
	if (pWaterArea == NULL)
		return NULL; // advc

	CvArea* pOtherWaterArea = pOtherCity->waterArea(true);
	if (pOtherWaterArea != NULL)
	{
		if (pWaterArea == pOtherWaterArea)
			return pWaterArea;

		CvArea* pSecondWaterArea = secondWaterArea();
		CvArea* pOtherSecondWaterArea = pOtherCity->secondWaterArea();
		if (pSecondWaterArea != NULL && pSecondWaterArea == pOtherWaterArea)
			return pSecondWaterArea;
		else if (pOtherSecondWaterArea != NULL && pWaterArea == pOtherSecondWaterArea)
			return pWaterArea;
		else if (pSecondWaterArea != NULL && pOtherSecondWaterArea != NULL &&
			pSecondWaterArea == pOtherSecondWaterArea)
		{
			return pSecondWaterArea;
		}
	}
	return NULL;
}


bool CvCity::isBlockaded() const
{
	FOR_EACH_ADJ_PLOT(getPlot())
	{
		if (pAdj->getBlockadedCount(getTeam()) > 0)
			return true;
	}
	return false;
} // BETTER_BTS_AI_MOD: END


CvPlot* CvCity::getRallyPlot() const
{
	return GC.getMap().plotSoren(m_iRallyX, m_iRallyY);
}


void CvCity::setRallyPlot(CvPlot* pPlot)
{
	if (getRallyPlot() == pPlot)
		return; // advc

	if (pPlot != NULL)
	{
		m_iRallyX = pPlot->getX();
		m_iRallyY = pPlot->getY();
	}
	else
	{
		m_iRallyX = INVALID_PLOT_COORD;
		m_iRallyY = INVALID_PLOT_COORD;
	}

	if (isCitySelected())
		gDLL->UI().setDirty(ColoredPlots_DIRTY_BIT, true);

}


void CvCity::setGameTurnFounded(int iNewValue)
{
	if (getGameTurnFounded() != iNewValue)
	{
		m_iGameTurnFounded = iNewValue;
		FAssert(getGameTurnFounded() >= 0);

		GC.getMap().updateWorkingCity();
	}
}


void CvCity::setGameTurnAcquired(int iNewValue)
{
	m_iGameTurnAcquired = iNewValue;
	FAssert(getGameTurnAcquired() >= 0);
}


void CvCity::setPopulation(int iNewValue)
{
/* Population Limit ModComp - Beginning : The game must warn the city's owner that the population limit is reached */		
	CvWString szBuffer;
/* Population Limit ModComp - End */
	int const iOldPopulation = getPopulation();
	if (iOldPopulation == iNewValue)
		return; // advc

	m_iPopulation = iNewValue;
	FAssert(getPopulation() >= 0);
	GET_PLAYER(getOwner()).invalidatePopulationRankCache();
	if (getPopulation() > getHighestPopulation())
		setHighestPopulation(getPopulation());
/* Population Limit ModComp - Beginning : The game must warn the city's owner that the population limit is reached */
	if (getPopulation() == getPopulationLimit())
	{
		szBuffer = gDLL->getText("TXT_KEY_CITY_GET_LIMITED", getNameKey(), getPopulationLimit());
		gDLL->getInterfaceIFace()->addMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNSIGN", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_LIMIT_CROSS")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX(), getY(), true, true);
	}
/* Population Limit ModComp - End */

	getArea().changePopulationPerPlayer(getOwner(), getPopulation() - iOldPopulation);
	GET_PLAYER(getOwner()).changeTotalPopulation(getPopulation() - iOldPopulation);
	GET_TEAM(getTeam()).changeTotalPopulation(getPopulation() - iOldPopulation);
	GC.getGame().changeTotalPopulation(getPopulation() - iOldPopulation);

	if (iOldPopulation > 0)
		getArea().changePower(getOwner(), -GC.getGame().getPopulationPower(iOldPopulation));
	if (getPopulation() > 0)
		getArea().changePower(getOwner(), GC.getGame().getPopulationPower(getPopulation()));

	getPlot().updateYield();

	updateMaintenance();

	if ((iOldPopulation == 1 && getPopulation() > 1) ||
		(getPopulation() == 1 && iOldPopulation > 1) ||
		(getPopulation() > iOldPopulation &&
		GET_PLAYER(getOwner()).getNumCities() <= 2))
	{
		if (!isHuman())
			setChooseProductionDirty(true);
	}

	GET_PLAYER(getOwner()).AI_makeAssignWorkDirty();

	setInfoDirty(true);
	setLayoutDirty(true);

	getPlot().plotAction(PUF_makeInfoBarDirty);

	if (isActiveOwned() && isCitySelected())
	{
		gDLL->UI().setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->UI().setDirty(CityScreen_DIRTY_BIT, true);
	}

	//updateGenericBuildings();
}


void CvCity::changePopulation(int iChange)
{
	setPopulation(getPopulation() + iChange);
}
/* Population Limit ModComp - Beginning */
int CvCity::getPopulationLimit() const
{
	if (GET_TEAM(getTeam()).isNoPopulationLimit())
	{
		return MAX_INT;
	}
			
	return GC.getInfo(getHandicapType()).getPopulationLimit() + getPopulationLimitChange();
}

int CvCity::getPopulationLimitChange() const
{
	return m_iPopulationLimitChange;
}

void CvCity::setPopulationLimitChange(int iNewValue)
{
	if (getPopulationLimitChange() != iNewValue)
	{
		m_iPopulationLimitChange = iNewValue;

		AI_setAssignWorkDirty(true);
	}
}
void CvCity::changePopulationLimitChange(int iChange)
{
	setPopulationLimitChange(getPopulationLimitChange() + iChange);
}
/* Population Limit ModComp - End */
// advc: Return type was long. Not helpful since sizeof(int)==sizeof(long).
int CvCity::getRealPopulation() const
{
	long long iPop = (long long)(std::pow((float)getPopulation(), 2.8f) * 1000);
	return truncIntCast<int>(iPop);
}


void CvCity::setHighestPopulation(int iNewValue)
{
 	m_iHighestPopulation = iNewValue;
	FAssert(getHighestPopulation() >= 0);
}


void CvCity::changeWorkingPopulation(int iChange)
{
//doto enhanced mylon limit pop
	if (getWorkingPopulation() > MAX_WORK_TILES)
		return;
	m_iWorkingPopulation += iChange;
	FAssert(getWorkingPopulation() >= 0);
}


void CvCity::changeSpecialistPopulation(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistPopulation += iChange;
		FAssert(getSpecialistPopulation() >= 0);
		GET_PLAYER(getOwner()).invalidateYieldRankCache();
		updateCommerce();
	}
}


void CvCity::changeNumGreatPeople(int iChange)
{
	if (iChange != 0)
	{
		m_iNumGreatPeople += iChange;
		FAssert(getNumGreatPeople() >= 0);
		updateCommerce();
	}
}


int CvCity::getGreatPeopleRate() const
{
	if (isDisorder())
		return 0;
	return (getBaseGreatPeopleRate() * getTotalGreatPeopleRateModifier()) / 100;
}


int CvCity::getTotalGreatPeopleRateModifier() const
{
	int iModifier = getGreatPeopleRateModifier();
	iModifier += GET_PLAYER(getOwner()).getGreatPeopleRateModifier();

	CvPlayer const& kOwner = GET_PLAYER(getOwner());
	if (kOwner.getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(kOwner.getStateReligion()))
			iModifier += kOwner.getStateReligionGreatPeopleRateModifier();
	}

	if (kOwner.isGoldenAge())
	{
		static int const iGOLDEN_AGE_GREAT_PEOPLE_MODIFIER = GC.getDefineINT("GOLDEN_AGE_GREAT_PEOPLE_MODIFIER"); // advc.opt
		iModifier += iGOLDEN_AGE_GREAT_PEOPLE_MODIFIER;
	}

	return std::max(0, iModifier + 100);
}


void CvCity::changeBaseGreatPeopleRate(int iChange)
{
	m_iBaseGreatPeopleRate = (m_iBaseGreatPeopleRate + iChange);
	FAssert(getBaseGreatPeopleRate() >= 0);
}


void CvCity::changeGreatPeopleRateModifier(int iChange)
{
	m_iGreatPeopleRateModifier += iChange;
}

// BUG - Building Additional Great People - start
/*  Returns the total additional great people rate that adding one of the given buildings will provide.
	Doesn't check if the building can be constructed in this city. */
int CvCity::getAdditionalGreatPeopleRateByBuilding(BuildingTypes eBuilding) const
{
	int iRate = getBaseGreatPeopleRate();
	int iModifier = getTotalGreatPeopleRateModifier();
	int iExtra = ((iRate + getAdditionalBaseGreatPeopleRateByBuilding(eBuilding)) *
			(iModifier + getAdditionalGreatPeopleRateModifierByBuilding(eBuilding)) / 100) -
			(iRate * iModifier / 100);

	return iExtra;
}

/*  Returns the additional great people rate that adding one of the given buildings will provide.
	Doesn't check if the building can be constructed in this city. */
int CvCity::getAdditionalBaseGreatPeopleRateByBuilding(BuildingTypes eBuilding) const
{
	CvBuildingInfo& kBuilding = GC.getInfo(eBuilding);
	int iExtraRate = kBuilding.getGreatPeopleRateChange();
	if (!GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding))
	{
		FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
			getFreeSpecialistCount(), Specialist, int)
		{
			iExtraRate += getAdditionalBaseGreatPeopleRateBySpecialist(
					perSpecialistVal.first, perSpecialistVal.second);
		}
	}
	return iExtraRate;
}

// Doesn't check if the building can be constructed in this city.
int CvCity::getAdditionalGreatPeopleRateModifierByBuilding(BuildingTypes eBuilding) const
{
	int iExtraModifier = 0;
	if (!GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding))
	{
		CvBuildingInfo& kBuilding = GC.getInfo(eBuilding);
		iExtraModifier += kBuilding.getGreatPeopleRateModifier();
		iExtraModifier += kBuilding.getGlobalGreatPeopleRateModifier();
	}

	return iExtraModifier;
}
// BUG - Building Additional Great People - end


// BUG - Specialist Additional Great People - start
/*  Returns the total additional great people rate that
	changing the number of the given specialist will provide/remove. */
int CvCity::getAdditionalGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange) const
{
	int iRate = getBaseGreatPeopleRate();
	int iModifier = getTotalGreatPeopleRateModifier();
	int iExtraRate = getAdditionalBaseGreatPeopleRateBySpecialist(eSpecialist, iChange);
	int iExtra = ((iRate + iExtraRate) * iModifier / 100) - (iRate * iModifier / 100);
	return iExtra;
}

/*  Returns the additional great people rate that
	changing the number of the given specialist will provide/remove. */
int CvCity::getAdditionalBaseGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange) const
{
	return iChange * GC.getInfo(eSpecialist).getGreatPeopleRateChange();
}
// BUG - Specialist Additional Great People - end

void CvCity::changeGreatPeopleProgress(int iChange)
{
	// <advc.078>
	if(m_iGreatPeopleProgress <= 0 && iChange > 0)
		GET_PLAYER(getOwner()).reportFirstGPP(); // </advc.078>
	m_iGreatPeopleProgress += iChange;
	FAssert(getGreatPeopleProgress() >= 0);
}


void CvCity::changeNumWorldWonders(int iChange)
{
	m_iNumWorldWonders += iChange;
	FAssert(getNumWorldWonders() >= 0);
}


void CvCity::changeNumTeamWonders(int iChange)
{
	m_iNumTeamWonders += iChange;
	FAssert(getNumTeamWonders() >= 0);
}


void CvCity::changeNumNationalWonders(int iChange)
{
	m_iNumNationalWonders += iChange;
	FAssert(getNumNationalWonders() >= 0);
}


void CvCity::changeNumBuildings(int iChange)
{
	m_iNumBuildings += iChange;
	FAssert(getNumBuildings() >= 0);
}


void CvCity::changeGovernmentCenterCount(int iChange)
{
	if (iChange != 0)
	{
		m_iGovernmentCenterCount += iChange;
		FAssert(getGovernmentCenterCount() >= 0);
		GET_PLAYER(getOwner()).updateMaintenance();
	}
}

// BUG - Building Saved Maintenance:
/*  Returns the total additional gold from saved maintenance times 100
	that adding one of the given buildings will provide.
	Doesn't check if the building can be constructed in this city. */
int CvCity::getSavedMaintenanceTimes100ByBuilding(BuildingTypes eBuilding) const
{
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	int iModifier = kBuilding.getMaintenanceModifier();
	if (iModifier != 0 && !isNoMaintenance())
	{
		int iNewMaintenance = calculateBaseMaintenanceTimes100() *
				std::max(0, getMaintenanceModifier() + iModifier + 100) / 100;
		return intdiv::round((getMaintenanceTimes100() - iNewMaintenance) *
				(100 + GET_PLAYER(getOwner()).calculateInflationRate()), 100); // K-Mod
	}
	return 0;
}


void CvCity::updateMaintenance()
{
	PROFILE_FUNC();

	int iOldMaintenance = getMaintenanceTimes100();
	int iNewMaintenance = 0;

	if (!isNoMaintenance())
	{
		iNewMaintenance = (calculateBaseMaintenanceTimes100() *
				std::max(0, getMaintenanceModifier() + 100)) / 100;
	}

	if (iOldMaintenance != iNewMaintenance)
	{
		FAssert(iOldMaintenance >= 0);
		FAssert(iNewMaintenance >= 0);

		m_iMaintenance = iNewMaintenance;
		FAssert(getMaintenance() >= 0);

		GET_PLAYER(getOwner()).changeTotalMaintenance(getMaintenanceTimes100() - iOldMaintenance);
	}
}

int CvCity::calculateDistanceMaintenance() const
{
	return calculateDistanceMaintenanceTimes100() / 100;
}

// advc.104: Added parameter
int CvCity::calculateDistanceMaintenanceTimes100(PlayerTypes eOwner) const
{
	// advc.004b: BtS code moved into new static function
//doto-dpii maintanence tags
	int iLocalDistance = getLocalDistanceMaintenanceModifier();
	return CvCity::calculateDistanceMaintenanceTimes100(getPlot(),
			eOwner == NO_PLAYER ? getOwner() : eOwner, getPopulation(), iLocalDistance);
}

int CvCity::calculateNumCitiesMaintenance() const
{
	return calculateNumCitiesMaintenanceTimes100() / 100;
}

// advc.104: Added parameter
int CvCity::calculateNumCitiesMaintenanceTimes100(PlayerTypes eOwner) const
{
	// advc.004b: BtS code moved into new static function
	return calculateNumCitiesMaintenanceTimes100(getPlot(),
			eOwner == NO_PLAYER ? getOwner() : eOwner,
			getPopulation());
}

int CvCity::calculateColonyMaintenance() const
{
	return calculateColonyMaintenanceTimes100() / 100;
}

// advc.104: Added parameter
int CvCity::calculateColonyMaintenanceTimes100(PlayerTypes eOwner) const
{
	// advc.004b: BtS code moved into new static function
	return calculateColonyMaintenanceTimes100(*plot(),
			eOwner == NO_PLAYER ? getOwner() : eOwner,
			getPopulation());
}


int CvCity::calculateCorporationMaintenance() const
{
	return calculateCorporationMaintenanceTimes100() / 100;
}

int CvCity::calculateCorporationMaintenanceTimes100() const
{
	int iMaintenance = 0;
	FOR_EACH_ENUM(Corporation)
	{
		if (isActiveCorporation(eLoopCorporation))
			iMaintenance += calculateCorporationMaintenanceTimes100(eLoopCorporation);
	}
	FAssert(iMaintenance >= 0);
	return iMaintenance;
}
//DPII < Maintenance Modifiers >
int CvCity::calculateHomeAreaMaintanance() const
{
	return calculateHomeAreaMaintenanceTimes100() / 100;
}
int CvCity::calculateHomeAreaMaintenanceTimes100(PlayerTypes eOwner) const
{
	int iLocalHomeArea = getLocalHomeAreaMaintenanceModifier();
	bool capitalcity = isCapital();
	return CvCity::calculateHomeAreaMaintenanceTimes100(getArea(), eOwner == NO_PLAYER ? getOwner() : eOwner, iLocalHomeArea, capitalcity);
}
int CvCity::calculateOtherAreaMaintanance() const
{
	return calculateOtherAreaMaintenanceTimes100() / 100;
}
int CvCity::calculateOtherAreaMaintenanceTimes100(PlayerTypes eOwner) const
{
	int iLocalOtherArea = getLocalOtherAreaMaintenanceModifier();
	return CvCity::calculateOtherAreaMaintenanceTimes100(getArea(), eOwner == NO_PLAYER ? getOwner() : eOwner, iLocalOtherArea);
}
int CvCity::calculateConnectedMaintanance() const
{
	return calculateConnectedMaintenanceTimes100() / 100;
}
int CvCity::calculateConnectedMaintenanceTimes100(PlayerTypes eOwner) const
{
	bool iCheckConnection = (isConnectedToCapital() && !(isCapital()));
	int iLocalConnected = getLocalConnectedCityMaintenanceModifier();
	return CvCity::calculateConnectedMaintenanceTimes100(eOwner == NO_PLAYER ? getOwner() : eOwner, iCheckConnection, iLocalConnected);
}
int CvCity::calculateCoastalMaintanance() const
{
	return calculateCoastalMaintenanceTimes100() / 100;
}
int CvCity::calculateCoastalMaintenanceTimes100(PlayerTypes eOwner) const
{
	int localCoastal = getLocalCoastalDistanceMaintenanceModifier();
	bool capitalcitynConn = (!isCapital() && !isConnectedToCapital());
	return CvCity::calculateCoastalMaintenanceTimes100(localCoastal, getPlot(), eOwner == NO_PLAYER ? getOwner() : eOwner, capitalcitynConn);
}
//DPII < Maintenance Modifiers >
int CvCity::calculateCorporationMaintenanceTimes100(CorporationTypes eCorporation) const
{
	int iMaintenance = 0;
	FOR_EACH_ENUM(Commerce)
	{
		iMaintenance += 100 * GC.getInfo(eCorporation).
				getHeadquarterCommerce(eLoopCommerce);
	}

	int iNumBonuses = 0;
	for (int i = 0; i < GC.getInfo(eCorporation).getNumPrereqBonuses(); i++)
	{
		iNumBonuses += getNumBonuses(GC.getInfo(eCorporation).getPrereqBonus(i));
	}

	int iBonusMaintenance = GC.getInfo(eCorporation).getMaintenance() * iNumBonuses;
	iBonusMaintenance *= GC.getInfo(GC.getMap().getWorldSize()).
			getCorporationMaintenancePercent();
	iBonusMaintenance /= 100;
	iMaintenance += iBonusMaintenance;

	iMaintenance *= getPopulation() + 17;
	iMaintenance /= 18;

	iMaintenance *= GC.getInfo(getHandicapType()).getCorporationMaintenancePercent();
	iMaintenance /= 100;

	iMaintenance *= std::max(0, GET_PLAYER(getOwner()).
			getCorporationMaintenanceModifier() + 100);
	iMaintenance /= 100;

	int iInflation = GET_PLAYER(getOwner()).calculateInflationRate() + 100;
	if (iInflation > 0)
	{
		iMaintenance *= 100;
		iMaintenance /= iInflation;
	}

	/*	K-Mod note. This assert (and others like it) can fail sometimes during the process
		of updating plot groups; because the # of bonuses can be temporarily negative. */
	FAssert(iMaintenance >= 0);
	return iMaintenance;
}


int CvCity::calculateBaseMaintenanceTimes100(
	/* <advc.ctr> */ PlayerTypes eOwner) const
{
	if (eOwner == NO_PLAYER)
		eOwner = getOwner(); // </advc.ctr>
	int iR = calculateDistanceMaintenanceTimes100(eOwner) +
			calculateNumCitiesMaintenanceTimes100(eOwner) +
//DOTO-dpii < Maintenance Modifiers >
			calculateColonyMaintenanceTimes100(eOwner) +
			calculateHomeAreaMaintenanceTimes100(eOwner) + 
			calculateOtherAreaMaintenanceTimes100(eOwner) +
			calculateConnectedMaintenanceTimes100(eOwner) + 
			calculateCoastalMaintenanceTimes100(eOwner) +
			(std::max(0,(GET_PLAYER(getOwner()).getGlobalMaintenanceModifier()) + 100) / 100);
	// advc.ctr: Anticipating corporation maintenance gets too complicated
	if (eOwner == getOwner())
		iR += calculateCorporationMaintenanceTimes100();
	return iR;
}


void CvCity::changeMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iMaintenanceModifier += iChange;
		updateMaintenance();
	}
}

//DOTO-DPII < Maintenance Modifiers >
void CvCity::changeLocalDistanceMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iLocalDistanceMaintenanceModifier += iChange;
		updateMaintenance();
	}
}
void CvCity::changeLocalCoastalDistanceMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iLocalCoastalDistanceMaintenanceModifier += iChange;
		updateMaintenance();
	}
}
void CvCity::changeLocalConnectedCityMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iLocalConnectedCityMaintenanceModifier += iChange;
		updateMaintenance();
	}
}
void CvCity::changeLocalHomeAreaMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iLocalHomeAreaMaintenanceModifier += iChange;
		updateMaintenance();
	}
}
void CvCity::changeLocalOtherAreaMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iLocalOtherAreaMaintenanceModifier += iChange;
		updateMaintenance();
	}
}
//DOTO-DPII < Maintenance Modifiers >

void CvCity::changeWarWearinessModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iWarWearinessModifier += iChange;
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeHurryAngerModifier(int iChange)
{
	if (iChange == 0)
		return;

	int iRatio = 0;
	if (m_iHurryAngerTimer > 0)
	{
		iRatio = (100 * (m_iHurryAngerTimer - 1)) /
				std::max(1, 100 + getHurryAngerModifier());
	}
	m_iHurryAngerModifier += iChange;
	if (m_iHurryAngerTimer > 0)
	{
		m_iHurryAngerTimer = (iRatio *
				std::max(1, 100 + getHurryAngerModifier())) / 100 + 1;
	}
}


void CvCity::changeHealRate(int iChange)
{
	m_iHealRate += iChange;
	FAssert(getHealRate() >= 0);
}


void CvCity::changeEspionageHealthCounter(int iChange)
{
	m_iEspionageHealthCounter += iChange;
}


void CvCity::changeEspionageHappinessCounter(int iChange)
{
	m_iEspionageHappinessCounter += iChange;
}


void CvCity::updateFreshWaterHealth()
{
	int iNewGoodHealth = 0;
	int iNewBadHealth = 0;

	if (getPlot().isFreshWater())
	{
		if (GC.getDefineINT(CvGlobals::FRESH_WATER_HEALTH_CHANGE) > 0)
			iNewGoodHealth += GC.getDefineINT(CvGlobals::FRESH_WATER_HEALTH_CHANGE);
		else iNewBadHealth += GC.getDefineINT(CvGlobals::FRESH_WATER_HEALTH_CHANGE);
	}

	if (getFreshWaterGoodHealth() != iNewGoodHealth || getFreshWaterBadHealth() != iNewBadHealth)
	{
		m_iFreshWaterGoodHealth = iNewGoodHealth;
		m_iFreshWaterBadHealth = iNewBadHealth;
		FAssert(getFreshWaterGoodHealth() >= 0);
		FAssert(getFreshWaterBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (isActiveTeam())
			setInfoDirty(true);
	}
}

// advc.901: Use one update function for both health and happiness
void CvCity::updateSurroundingHealthHappiness()
{
	PROFILE_FUNC(); // advc.901: Now called once per turn; seems fine.
	int iNewGoodHappiness = 0;
	int iNewBadHappiness = 0;
	CvTeam const& kTeam = GET_TEAM(getTeam()); // advc.901
	for (CityPlotIter it(*this); it.hasNext(); ++it)
	{
		CvPlot const& kPlot = *it;
		{
			FeatureTypes eFeature = kPlot.getFeatureType();
			if (eFeature != NO_FEATURE)
			{
				int iHappy = GET_PLAYER(getOwner()).getFeatureHappiness(eFeature);
				if (kTeam.canAccessHappyHealth(kPlot, iHappy)) // advc.901
					(iHappy > 0 ? iNewGoodHappiness : iNewBadHappiness) += iHappy;
			}
		}
		{
			ImprovementTypes eImprovement = kPlot.getImprovementType();
			if (eImprovement != NO_IMPROVEMENT)
			{
				int iHappy = GC.getInfo(eImprovement).getHappiness();
				if (kTeam.canAccessHappyHealth(kPlot, iHappy)) // advc.901
					(iHappy > 0 ? iNewGoodHappiness : iNewBadHappiness) += iHappy;
			}
		}
	}
	if (getSurroundingGoodHappiness() != iNewGoodHappiness)
	{
		m_iSurroundingGoodHappiness = iNewGoodHappiness;
		FAssert(getSurroundingGoodHappiness() >= 0);
		AI_setAssignWorkDirty(true);
	}
	if (getSurroundingBadHappiness() != iNewBadHappiness)
	{
		m_iSurroundingBadHappiness = iNewBadHappiness;
		FAssert(getSurroundingBadHappiness() <= 0);
		AI_setAssignWorkDirty(true);
	}

	std::pair<int,int> iiGoodBad = calculateSurroundingHealth(); // advc.901: Moved into new function
	if (getSurroundingGoodHealth() != iiGoodBad.first ||
		getSurroundingBadHealth() != iiGoodBad.second)
	{
		m_iSurroundingGoodHealth = iiGoodBad.first;
		m_iSurroundingBadHealth = iiGoodBad.second;
		FAssert(getSurroundingGoodHealth() >= 0);
		FAssert(getSurroundingBadHealth() <= 0);
		AI_setAssignWorkDirty(true);
		if (isActiveTeam())
			setInfoDirty(true);
	}
}

// advc.901: Body cut from updateFeatureHealth; the parameter is new.
std::pair<int,int> CvCity::calculateSurroundingHealth(int iGoodExtraPercent, int iBadExtraPercent) const
{
	int iGoodHealth = iGoodExtraPercent;
	int iBadHealth = iBadExtraPercent;
	CvTeam const& kTeam = GET_TEAM(getTeam()); // advc.901
	for (CityPlotIter it(*this); it.hasNext(); ++it)
	{
		CvPlot const& kPlot = *it;
		{
			FeatureTypes eFeature = kPlot.getFeatureType();
			if (eFeature != NO_FEATURE)
			{
				int iHealthPercent = GC.getInfo(eFeature).getHealthPercent();
				if (kTeam.canAccessHappyHealth(kPlot, iHealthPercent)) // advc.901
					(iHealthPercent > 0 ? iGoodHealth : iBadHealth) += iHealthPercent;
			}
		}  // <advc.901> (based on updateFeatureHappiness)
		ImprovementTypes eImprovement = kPlot.getImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			int iHealthPercent = GC.getInfo(eImprovement).get(CvImprovementInfo::HealthPercent);
			if (kTeam.canAccessHappyHealth(kPlot, iHealthPercent))
				(iHealthPercent > 0 ? iGoodHealth : iBadHealth) += iHealthPercent;
		} // </advc.901>
	}
	return std::make_pair(iGoodHealth / 100, iBadHealth / 100);
}

/*  <advc.901> This was previously handled (badly) by CvCityAI::AI_getImprovementValue.
	Returns the change in health, rounded (iHealthChange) and as a percentage
	(iHealthChangePercent), and the change in happiness (iHappyChange) that would
	result from replacing eOldImprov with eNewImprov in kPlot and possibly
	removing kPlot's feature.
	This function is for AI purposes (no need to tally good and bad effects
	separately), the one below is for UI purposes. */
void CvCity::calculateHealthHappyChange(CvPlot const& kPlot,
	ImprovementTypes eNewImprov, ImprovementTypes eOldImprov, bool bRemoveFeature,
	int& iHappyChange, int& iHealthChange, int& iHealthPercentChange) const
{
	int iGoodHappyChange, iBadHappyChange, iGoodHealthChange, iBadHealthChange,
			iGoodHealthPercentChange, iBadHealthPercentChange;
	goodBadHealthHappyChange(kPlot, eNewImprov, eOldImprov, bRemoveFeature,
			iGoodHappyChange, iBadHappyChange, iGoodHealthChange, iBadHealthChange,
			iGoodHealthPercentChange, iBadHealthPercentChange);
	iHappyChange = iGoodHappyChange + iBadHappyChange;
	iHealthChange = iGoodHealthChange + iBadHealthChange;
	iHealthPercentChange = iGoodHealthPercentChange + iBadHealthPercentChange;
}

/*  Positive and negative effects need to be tallied separately for "correct"
	rounding - as in BtS: bad jungle health and good forest health are rounded
	separately. */
void CvCity::goodBadHealthHappyChange(CvPlot const& kPlot, ImprovementTypes eNewImprov,
	ImprovementTypes eOldImprov, bool bRemoveFeature, int& iHappyChange,
	int& iUnhappyChange, int& iGoodHealthChange, int& iBadHealthChange,
	int& iGoodHealthPercentChange, int& iBadHealthPercentChange) const
{
	iHappyChange = iUnhappyChange = iGoodHealthChange = iBadHealthChange =
			iGoodHealthPercentChange = iBadHealthPercentChange = 0;
	if (eNewImprov == eOldImprov && !bRemoveFeature)
		return;

	CvTeam const& kTeam = GET_TEAM(getTeam());
	int iFeatureHappy = 0;
	int iFeatureGoodHealthPercent = 0;
	int iFeatureUnhappy = 0;
	int iFeatureBadHealthPercent = 0;
	if (bRemoveFeature)
	{
		FeatureTypes eFeature = kPlot.getFeatureType();
		if (eFeature != NO_FEATURE)
		{
			int iHappy = GET_PLAYER(getOwner()).getFeatureHappiness(eFeature);
			if (kTeam.canAccessHappyHealth(kPlot, iHappy))
				(iHappy > 0 ? iFeatureHappy : iFeatureUnhappy) = iHappy;
			int iHealthPercent = GC.getInfo(eFeature).getHealthPercent();
			if (kTeam.canAccessHappyHealth(kPlot, iHealthPercent))
			{
				(iHealthPercent > 0 ? iFeatureGoodHealthPercent :
						iFeatureBadHealthPercent) += iHealthPercent;
			}
		}
	}
	int iNewHappy = 0;
	int iNewUnhappy = 0;
	int iNewGoodHealthPercent = 0;
	int iNewBadHealthPercent = 0;
	if (eNewImprov != NO_IMPROVEMENT)
	{
		int iHappy = GC.getInfo(eNewImprov).getHappiness();
		if (kTeam.canAccessHappyHealth(kPlot, iHappy))	
			(iHappy > 0 ? iNewHappy : iNewUnhappy) = iHappy;
		int iHealthPercent = GC.getInfo(eNewImprov).get(CvImprovementInfo::HealthPercent);
		if (kTeam.canAccessHappyHealth(kPlot, iHealthPercent))
			(iHealthPercent > 0 ? iNewGoodHealthPercent : iNewBadHealthPercent) += iHealthPercent;
	}
	int iOldHappy = 0;
	int iOldUnhappy = 0;
	int iOldGoodHealthPercent = 0;
	int iOldBadHealthPercent = 0;
	if (eOldImprov != NO_IMPROVEMENT)
	{
		int iHappy = GC.getInfo(eOldImprov).getHappiness();
		if (kTeam.canAccessHappyHealth(kPlot, iHappy))
			(iHappy > 0 ? iOldHappy : iOldUnhappy) = iHappy;
		int iHealthPercent = GC.getInfo(eOldImprov).get(CvImprovementInfo::HealthPercent);
		if (kTeam.canAccessHappyHealth(kPlot, iHealthPercent))
			(iHealthPercent > 0 ? iOldGoodHealthPercent : iOldBadHealthPercent) += iHealthPercent;
	}
	iHappyChange = iNewHappy - iOldHappy - iFeatureHappy;
	iUnhappyChange = iNewUnhappy - iOldUnhappy - iFeatureUnhappy;

	iGoodHealthPercentChange = iNewGoodHealthPercent - iOldGoodHealthPercent - iFeatureGoodHealthPercent;
	iBadHealthPercentChange = iNewBadHealthPercent - iOldBadHealthPercent - iFeatureBadHealthPercent;
	std::pair<int,int> iiNewHealth = calculateSurroundingHealth(iGoodHealthPercentChange, iBadHealthPercentChange);
	std::pair<int,int> iiOldHealth = calculateSurroundingHealth();
	iGoodHealthChange = iiNewHealth.first - iiOldHealth.first;
	iBadHealthChange = iiNewHealth.second - iiOldHealth.second;
}// </advc.901>
/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: Enable Terrain Health Modifiers                                                  **/
/**  Notes:   DOTO-                                                                                       **/
/*****************************************************************************************************/
int CvCity::getTerrainGoodHealth() const
{
	return m_iTerrainGoodHealth;
}

int CvCity::getTerrainBadHealth() const
{
	return m_iTerrainBadHealth;
}
//keldath - tried to fix syntax from org, but f1rpo did it better below.
/*void CvCity::updateTerrainHealth()
{
//	CvPlot* pLoopPlot;
	TerrainTypes eTerrain;
	int iNewGoodHealth;
	int iNewBadHealth;

	iNewGoodHealth = 0;
	iNewBadHealth = 0;

	for (CityPlotIter it(*this); it.hasNext(); ++it)
	{
		CvPlot const& kPlot = *it;
	//	if (kPlot != NULL)
	//	{
			eTerrain = kPlot.getTerrainType();

			if (eTerrain != NO_TERRAIN)
			{
				if (GC.getInfo(eTerrain).getHealthPercent() > 0)
				{
					iNewGoodHealth += GC.getInfo(eTerrain).getHealthPercent();
				}
				else
				{
					iNewBadHealth += GC.getInfo(eTerrain).getHealthPercent();
				}
			}
	//	}
	}
*/
//f1rpo 097 cleaner syntax
void CvCity::updateTerrainHealth()
{
   int iNewGoodHealth = 0;
   int iNewBadHealth = 0;
   for (CityPlotIter it(*this); it.hasNext(); ++it)
   {
       int iHealthPercent = GC.getInfo(it->getTerrainType()).getHealthPercent();
       if(iHealthPercent > 0)
       	 iNewGoodHealth += iHealthPercent;
       else if (iHealthPercent < 0)
       	 iNewBadHealth += iHealthPercent;
   }
	iNewGoodHealth /= 100;
	iNewBadHealth /= 100;

	if ((getTerrainGoodHealth() != iNewGoodHealth) || (getTerrainBadHealth() != iNewBadHealth))
	{
		m_iTerrainGoodHealth = iNewGoodHealth;
		m_iTerrainBadHealth = iNewBadHealth;
		FAssert(getTerrainGoodHealth() >= 0);
		FAssert(getTerrainBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}
/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags   DOTO-                                                          **/
/*****************************************************************************************************/

/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**                    DOTO-                                                                          */
/*************************************************************************************************/
int CvCity::getSpecialistGoodHealth() const
{
	return m_iSpecialistGoodHealth;
}

int CvCity::getSpecialistBadHealth() const
{
	return m_iSpecialistBadHealth;
}

int CvCity::getSpecialistHappiness() const
{
	return m_iSpecialistHappiness;
}

int CvCity::getSpecialistUnhappiness() const
{
	return m_iSpecialistUnhappiness;
}

void CvCity::changeSpecialistGoodHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistGoodHealth += iChange;
		FAssert(getSpecialistGoodHealth() >= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

void CvCity::changeSpecialistBadHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistBadHealth += iChange;
		FAssert(getSpecialistBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

void CvCity::changeSpecialistHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistHappiness += iChange;
		FAssert(getSpecialistHappiness() >= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

void CvCity::changeSpecialistUnhappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistUnhappiness += iChange;
		FAssert(getSpecialistUnhappiness() >= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}
/*************************************************************************************************/
/** Specialists Enhancements   DOTO-                       END                                              */
/*************************************************************************************************/

// BUG - Actual Effects - start
/*	Returns the additional angry population caused by the given happiness changes.
	Positive values for iBad mean an increase in unhappiness. */
int CvCity::getAdditionalAngryPopuplation(int iGood, int iBad) const
{
	int iHappy = happyLevel();
	int iUnhappy = unhappyLevel();
	int iPop = getPopulation();
	return range((iUnhappy + iBad) - (iHappy + iGood), 0, iPop) -
			range(iUnhappy - iHappy, 0, iPop);
}

/*  Returns the additional spoiled food caused by the given health changes.
	Positive values for iBad mean an increase in unhealthiness. */
int CvCity::getAdditionalSpoiledFood(int iGood, int iBad) const
{
	int iHealthy = goodHealth();
	int iUnhealthy = badHealth();
	int iRate = iHealthy - iUnhealthy;
	return std::min(0, iRate) - std::min(0, iRate + iGood - iBad);
}

// Returns the additional starvation caused by the given spoiled food.
int CvCity::getAdditionalStarvation(int iSpoiledFood) const
{
	int iFood = getYieldRate(YIELD_FOOD) - foodConsumption();
	if (iSpoiledFood > 0)
	{
		if (iFood <= 0)
			return iSpoiledFood;
		if (iSpoiledFood > iFood)
			return iSpoiledFood - iFood;
	}
	else if (iSpoiledFood < 0)
	{
		if (iFood < 0)
			return std::max(iFood, iSpoiledFood);
	}
	return 0;
} // BUG - Actual Effects - end

/*  <advc.001c> These two functions are based on code cut from
	CvGameTextMgr::parseGreatPeopleHelp */
int CvCity::GPTurnsLeft() const
{
	if(getGreatPeopleRate() <= 0)
		return -1;
	int iGPPLeft = GET_PLAYER(getOwner()).greatPeopleThreshold(false) - getGreatPeopleProgress();
	if (iGPPLeft <= 0)
		return 0;
	int iR = iGPPLeft / getGreatPeopleRate();
	if (iR * getGreatPeopleRate() < iGPPLeft)
		iR++;
	return iR;
}

void CvCity::GPProjection(std::vector<std::pair<UnitTypes,int> >& aeiProjection) const
{
	if (isDisorder())
		return;
	CvCivilization const& kCiv = getCivilization();
//doto 100 assert fix - when civic with food prod for units is on
	int const iTurnsLeft = GPTurnsLeft() < 0 ? 0 : GPTurnsLeft();
	/*  (advc.001c: Can't use getGreatPeopleProgress() b/c the
		per-unit progress values won't add up to that in old saves.) */
	int iTotalUnitProgress = 0;
	for (int i = 0; i < kCiv.getNumUnits(); i++)
		iTotalUnitProgress += getGreatPeopleUnitProgress(kCiv.unitAt(i));
	/*	GPP total of the city on the turn that the GP will be born.
		(Usually greater than GET_PLAYER(getOwner()).greatPeopleThreshold().) */
	int iProjectedTotal = iTotalUnitProgress + iTurnsLeft * getGreatPeopleRate();
	int iRoundedPercentages = 0;
	FOR_EACH_ENUM(Unit)
	{
		scaled rShare = (getGreatPeopleUnitProgress(eLoopUnit) +
				per100(iTurnsLeft * getGreatPeopleUnitRate(eLoopUnit) *
				getTotalGreatPeopleRateModifier())) / iProjectedTotal;
		if (rShare > 0)
		{
			/*	I've seen 103 here with the Parthenon (150% modifier).
				Haven't investigated in detail, but I'm guessing that the calculation
				in the loop has fewer rounding errors than the one before the loop. */
			int iPercent = std::min(100, rShare.getPercent());
			iRoundedPercentages += iPercent;
			aeiProjection.push_back(std::make_pair(eLoopUnit, iPercent));
		}
	}
	if (!aeiProjection.empty())
	{
		int iTotalError = 100 - iRoundedPercentages;
		if (iTotalError >= 2)
		{
			FAssert(iTotalError == 2);
			/*	I'm too lazy to handle that error properly. Adding up to 99% is OK.
				Otherwise add 1 to the GP with the highest share. */
			size_t iArgmax = 0;
			int iMax = 0;
			for (size_t i = 0; i < aeiProjection.size(); i++)
			{
				if (aeiProjection[i].second > iMax)
				{
					iMax = aeiProjection[i].second;
					iArgmax = i;
				}
			}
			aeiProjection[iArgmax].second += iTotalError - 1;
		}
	}
} // </advc.001c>


int CvCity::getBuildingHealth(BuildingTypes eBuilding) const
{
	int iHealth = getBuildingGoodHealth(eBuilding);
	if (!isBuildingOnlyHealthy())
		iHealth += getBuildingBadHealth(eBuilding);
	return iHealth;
}

int CvCity::getBuildingGoodHealth(BuildingTypes eBuilding) const
{
	int iHealth = std::max(0, GC.getInfo(eBuilding).getHealth());
	iHealth += std::max(0, getBuildingHealthChange(
			GC.getInfo(eBuilding).getBuildingClassType()));
	iHealth += std::max(0, GET_PLAYER(getOwner()).getExtraBuildingHealth(eBuilding));
	return iHealth;
}

int CvCity::getBuildingBadHealth(BuildingTypes eBuilding) const
{
	if (isBuildingOnlyHealthy())
		return 0;
	int iHealth = std::min(0, GC.getInfo(eBuilding).getHealth());
	iHealth += std::min(0, getBuildingHealthChange(
			GC.getInfo(eBuilding).getBuildingClassType()));
	iHealth += std::min(0, GET_PLAYER(getOwner()).getExtraBuildingHealth(eBuilding));
	return iHealth;
}

void CvCity::changeBuildingGoodHealth(int iChange)
{
	if (iChange == 0)
		return;
	m_iBuildingGoodHealth += iChange;
	FAssert(getBuildingGoodHealth() >= 0);
	AI_setAssignWorkDirty(true);
	if (isActiveTeam())
		setInfoDirty(true);
}


void CvCity::changeBuildingBadHealth(int iChange)
{
	if (iChange == 0)
		return;
	m_iBuildingBadHealth += iChange;
	FAssert(getBuildingBadHealth() <= 0);
	AI_setAssignWorkDirty(true);
	if (isActiveTeam())
		setInfoDirty(true);
}


void CvCity::updatePowerHealth()
{
	int iNewGoodHealth = 0;
	int iNewBadHealth = 0;

	if (isPower())
	{
		int iPowerHealth = GC.getDefineINT(CvGlobals::POWER_HEALTH_CHANGE);
		if (iPowerHealth > 0)
			iNewGoodHealth += iPowerHealth;
		else iNewBadHealth += iPowerHealth;
	}

	if (isDirtyPower())
	{
		int iDirtyPowerHealth = GC.getDefineINT(CvGlobals::DIRTY_POWER_HEALTH_CHANGE);
		if (iDirtyPowerHealth > 0)
			iNewGoodHealth += iDirtyPowerHealth;
		else iNewBadHealth += iDirtyPowerHealth;
	}

	if (getPowerGoodHealth() != iNewGoodHealth ||
		getPowerBadHealth() != iNewBadHealth)
	{
		m_iPowerGoodHealth = iNewGoodHealth;
		m_iPowerBadHealth = iNewBadHealth;
		FAssert(getPowerGoodHealth() >= 0);
		FAssert(getPowerBadHealth() <= 0);
		AI_setAssignWorkDirty(true);
		if (isActiveTeam())
			setInfoDirty(true);
	}
}


void CvCity::changeBonusGoodHealth(int iChange)
{
	if (iChange == 0)
		return; // advc
	m_iBonusGoodHealth += iChange;
	FAssert(getBonusGoodHealth() >= 0);
	AI_setAssignWorkDirty(true);
	if (isActiveTeam())
		setInfoDirty(true);
}


void CvCity::changeBonusBadHealth(int iChange)
{
	if (iChange == 0)
		return; // advc
	m_iBonusBadHealth += iChange;
	FAssert(getBonusBadHealth() <= 0);
	AI_setAssignWorkDirty(true);
	if (isActiveTeam())
		setInfoDirty(true);
}

// DOTO-< Civic Infos Plus Start >
int CvCity::getReligionGoodHealth() const
{
	return m_iReligionGoodHealth;
}

int CvCity::getReligionBadHealth() const
{
	return m_iReligionBadHealth;
}

int CvCity::getReligionHealth(ReligionTypes eReligion) const
{
	int iHealth;

	iHealth = 0;

	if (isHasReligion(eReligion))
	{
		if (eReligion == GET_PLAYER(getOwner()).getStateReligion())
		{
			iHealth += GET_PLAYER(getOwner()).getStateReligionExtraHealth();
		}
		else
		{
			iHealth += GET_PLAYER(getOwner()).getNonStateReligionExtraHealth();
		}
	}

	return iHealth;
}

void CvCity::updateReligionHealth()
{
	int iNewReligionGoodHealth;
	int iNewReligionBadHealth;
	int iChange;
	int iI;

	iNewReligionGoodHealth = 0;
	iNewReligionBadHealth = 0;

	for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		iChange = getReligionHealth((ReligionTypes)iI);

		if (iChange > 0)
		{
			iNewReligionGoodHealth += iChange;
		}
		else
		{
			iNewReligionBadHealth += iChange;
		}
	}

	if (getReligionGoodHealth() != iNewReligionGoodHealth)
	{
		m_iReligionGoodHealth = iNewReligionGoodHealth;
		FAssert(getReligionGoodHealth() >= 0);

		AI_setAssignWorkDirty(true);
	}

	if (getReligionBadHealth() != iNewReligionBadHealth)
	{
		m_iReligionBadHealth = iNewReligionBadHealth;
		FAssert(getReligionBadHealth() <= 0);

		AI_setAssignWorkDirty(true);
	}
}
// DOTO-< Civic Infos Plus End   >

int CvCity::getMilitaryHappiness() const
{
	return (getMilitaryHappinessUnits() * GET_PLAYER(getOwner()).getHappyPerMilitaryUnit())
			/ 2; // advc.912c
}


void CvCity::changeMilitaryHappinessUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iMilitaryHappinessUnits += iChange;
		FAssert(getMilitaryHappinessUnits() >= 0);
		AI_setAssignWorkDirty(true);
		// <advc.004> Update the unhappiness indicator
		if (isActiveOwned())
			gDLL->UI().setDirty(CityInfo_DIRTY_BIT, true); // </advc.004>
	}
}


int CvCity::getBuildingHappiness(BuildingTypes eBuilding) const
{
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	int iHappiness = kBuilding.getHappiness();

	if (kBuilding.getReligionType() != NO_RELIGION)
	{
		if (kBuilding.getReligionType() == GET_PLAYER(getOwner()).getStateReligion())
			iHappiness += kBuilding.getStateReligionHappiness();
	}
	iHappiness += GET_PLAYER(getOwner()).getExtraBuildingHappiness(eBuilding);

	FOR_EACH_ENUM(Commerce)
	{
		iHappiness += (kBuilding.getCommerceHappiness(eLoopCommerce) *
				GET_PLAYER(getOwner()).getCommercePercent(eLoopCommerce)) / 100;
	}
	iHappiness += getBuildingHappyChange(kBuilding.getBuildingClassType());

	return iHappiness;
}


void CvCity::changeBuildingGoodHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingGoodHappiness += iChange;
		FAssert(getBuildingGoodHappiness() >= 0);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeBuildingBadHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBadHappiness += iChange;
		FAssert(getBuildingBadHappiness() <= 0);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::updateExtraBuildingHappiness()
{
	int iNewExtraBuildingGoodHappiness = 0;
	int iNewExtraBuildingBadHappiness = 0;

	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes const eBuilding = kCiv.buildingAt(i);
		int iChange = getNumActiveBuilding(eBuilding) *
				GET_PLAYER(getOwner()).getExtraBuildingHappiness(eBuilding);
		if (iChange > 0)
			iNewExtraBuildingGoodHappiness += iChange;
		else iNewExtraBuildingBadHappiness += iChange;
	}

	if (getExtraBuildingGoodHappiness() != iNewExtraBuildingGoodHappiness)
	{
		m_iExtraBuildingGoodHappiness = iNewExtraBuildingGoodHappiness;
		FAssert(getExtraBuildingGoodHappiness() >= 0);
		AI_setAssignWorkDirty(true);
	}
	if (getExtraBuildingBadHappiness() != iNewExtraBuildingBadHappiness)
	{
		m_iExtraBuildingBadHappiness = iNewExtraBuildingBadHappiness;
		FAssert(getExtraBuildingBadHappiness() <= 0);
		AI_setAssignWorkDirty(true);
	}
}

/*  BETTER_BTS_AI_MOD, City AI, 02/24/10, EmperorFool: START
	(BUG - Building Additional Happiness) */
namespace // advc: These had been declared as global functions in the header
{
	// Adds iValue to iGood if it is positive or its negative to iBad if it is negative.
	void addGoodOrBad(int iValue, int& iGood, int& iBad)
	{
		if (iValue > 0)
			iGood += iValue;
		else if (iValue < 0)
			iBad -= iValue;
	}

	// Subtracts iValue from iGood if it is positive or its negative from iBad if it is negative.
	void subtractGoodOrBad(int iValue, int& iGood, int& iBad)
	{
		if (iValue > 0)
			iGood -= iValue;
		else if (iValue < 0)
			iBad += iValue;
	}
}

/*	Returns the total additional happiness that adding one of the given buildings
	will provide and sets the good and bad levels individually.
	Doesn't reset iGood or iBad to zero.
	Doesn't check if the building can be constructed in this city. */
int CvCity::getAdditionalHappinessByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad) const
{
	CvBuildingInfo& kBuilding = GC.getInfo(eBuilding);
	CvPlayer const& kOwner = GET_PLAYER(getOwner());

	int const iStarting = iGood - iBad;
	int const iStartingBad = iBad;

	// Basic
	addGoodOrBad(kBuilding.getHappiness(), iGood, iBad);

	// Building Class
	addGoodOrBad(getBuildingHappyChange(kBuilding.getBuildingClassType()), iGood, iBad);

	// Player Building
	addGoodOrBad(kOwner.getExtraBuildingHappiness(eBuilding), iGood, iBad);

	// Area
	addGoodOrBad(kBuilding.getAreaHappiness(), iGood, iBad);

	// Religion
	if (kBuilding.getReligionType() != NO_RELIGION &&
		kBuilding.getReligionType() == kOwner.getStateReligion())
	{
		iGood += kBuilding.getStateReligionHappiness();
	}

	// Bonus
	FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
		getBonusHappinessChanges(), Bonus, int)
	{
		BonusTypes const eLoopBonus = perBonusVal.first;
		if ((hasBonus(eLoopBonus) || kBuilding.getFreeBonus() == eLoopBonus) &&
			kBuilding.getNoBonus() != eLoopBonus)
		{
			addGoodOrBad(perBonusVal.second, iGood, iBad);
		}
	}
	// Commerce
	FOR_EACH_ENUM(Commerce)
	{
		addGoodOrBad(kBuilding.getCommerceHappiness(eLoopCommerce) *
				kOwner.getCommercePercent(eLoopCommerce) / 100, iGood, iBad);
	}

	// War Weariness Modifier
	if (kBuilding.getWarWearinessModifier() != 0)
	{
		int iBaseAngerPercent = 0;
		iBaseAngerPercent += getOvercrowdingPercentAnger();
		iBaseAngerPercent += getNoMilitaryPercentAnger();
		iBaseAngerPercent += getCulturePercentAnger();
		iBaseAngerPercent += getReligionPercentAnger();
		iBaseAngerPercent += getHurryPercentAnger();
		iBaseAngerPercent += getConscriptPercentAnger();
		iBaseAngerPercent += getDefyResolutionPercentAnger();
		FOR_EACH_ENUM(Civic)
		{
			iBaseAngerPercent += kOwner.getCivicPercentAnger(eLoopCivic);
		}

		int iCurrentAngerPercent = iBaseAngerPercent + getWarWearinessPercentAnger();
		int iCurrentUnhappiness = iCurrentAngerPercent * getPopulation() /
				GC.getPERCENT_ANGER_DIVISOR();

		int iNewWarAngerPercent = kOwner.getWarWearinessPercentAnger();
		iNewWarAngerPercent *= std::max(0, kBuilding.getWarWearinessModifier() +
				getWarWearinessModifier() + kOwner.getWarWearinessModifier() + 100);
		iNewWarAngerPercent /= 100;
		int iNewAngerPercent = iBaseAngerPercent + iNewWarAngerPercent;
		int iNewUnhappiness = iNewAngerPercent * getPopulation() / GC.getPERCENT_ANGER_DIVISOR();

		iBad += iNewUnhappiness - iCurrentUnhappiness;
	}

	// K-Mod. If the city is immune to unhappiness, then clear the "bad" from this building.
	if (isNoUnhappiness())
		iBad = iStartingBad;
	// K-Mod end

	// No Unhappiness
	if (kBuilding.isNoUnhappiness())
	{
		// override extra unhappiness and completely negate all existing unhappiness
		iBad = iStartingBad - unhappyLevel();
	}

	return iGood - iBad - iStarting;
}

/*	Returns the total additional health that adding one of the given buildings
	will provide and sets the good and bad levels individually.
	Doesn't reset iGood or iBad to zero.
	Doesn't check if the building can be constructed in this city. */
int CvCity::getAdditionalHealthByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad,
	bool bAssumeStrategicBonuses) const // advc.001h
{
	CvBuildingInfo& kBuilding = GC.getInfo(eBuilding);
	CvPlayer const& kOwner = GET_PLAYER(getOwner());

	int const iStarting = iGood - iBad;
	int const iStartingBad = iBad;

	// Basic
	addGoodOrBad(kBuilding.getHealth(), iGood, iBad);

	// Building Class
	addGoodOrBad(getBuildingHealthChange(kBuilding.getBuildingClassType()), iGood, iBad);

	// Player Building
	addGoodOrBad(kOwner.getExtraBuildingHealth(eBuilding), iGood, iBad);

	// Area
	addGoodOrBad(kBuilding.getAreaHealth(), iGood, iBad);

	// No Unhealthiness from Buildings
	if (isBuildingOnlyHealthy())
	{
		// undo bad from this building
		iBad = iStartingBad;
	}
	if (kBuilding.isBuildingOnlyHealthy())
	{
		// undo bad from this and all existing buildings
		iBad = iStartingBad + totalBadBuildingHealth();
	}

	// Bonus
	FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
		getBonusHealthChanges(), Bonus, int)
	{
		BonusTypes const eLoopBonus = perBonusVal.first;
		if ((hasBonus(eLoopBonus) || kBuilding.getFreeBonus() == eLoopBonus ||
			// <advc.001h>
			(bAssumeStrategicBonuses &&
			GC.getInfo(eLoopBonus).getHappiness() == 0 &&
			GC.getInfo(eLoopBonus).getHealth() == 0)) && // </advc.001h>
			kBuilding.getNoBonus() != eLoopBonus)
		{
			addGoodOrBad(perBonusVal.second, iGood, iBad);
		}
	}
	// advc.001h: Need this several times
	BonusTypes ePowBonus = kBuilding.getPowerBonus();
	// Power
	if (kBuilding.isPower() || kBuilding.isAreaCleanPower() ||
		(ePowBonus != NO_BONUS && (hasBonus(ePowBonus) ||
		// <advc.001h>
		(bAssumeStrategicBonuses &&
		GC.getInfo(ePowBonus).getHappiness() == 0 &&
		GC.getInfo(ePowBonus).getHealth() == 0)))) // </advc.001h>
	{
		// adding power
		if (!isPower())
		{
			addGoodOrBad(GC.getDefineINT(CvGlobals::POWER_HEALTH_CHANGE), iGood, iBad);

			// adding dirty power
			if (kBuilding.isDirtyPower())
			{
				addGoodOrBad(GC.getDefineINT(
						CvGlobals::DIRTY_POWER_HEALTH_CHANGE), iGood, iBad);
			}
		}
		/*	advc.001h: Count change from dirty to clean only if we
			already have the resource (i.e. Uranium) */
		else if(ePowBonus == NO_BONUS || hasBonus(ePowBonus))
		{
			// replacing dirty power with clean power
			if (isDirtyPower() && !kBuilding.isDirtyPower())
			{
				subtractGoodOrBad(GC.getDefineINT(
						CvGlobals::DIRTY_POWER_HEALTH_CHANGE), iGood, iBad);
			}
		}
	}
	// No Unhealthiness from Population
	/*if (kBuilding.isNoUnhealthyPopulation())
		iBad -= getPopulation();*/ // BtS
	/*  K-Mod, 27/dec/10, karadoc (start)
		replaced NoUnhealthyPopulation with UnhealthyPopulationModifier */
	// Modified unhealthiness from population
	int iEffectiveModifier = 0;
	if (kBuilding.getUnhealthyPopulationModifier() + getUnhealthyPopulationModifier() < -100)
		iEffectiveModifier = std::min(0, -100 - getUnhealthyPopulationModifier());
	else iEffectiveModifier = std::max(-100, kBuilding.getUnhealthyPopulationModifier());
	iBad += intdiv::round(getPopulation() * iEffectiveModifier, 100);
	// K-Mod end
	return iGood - iBad - iStarting;
}
// BETTER_BTS_AI_MOD: END

void CvCity::updateExtraBuildingHealth()
{
	int iNewExtraBuildingGoodHealth = 0;
	int iNewExtraBuildingBadHealth = 0;

	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes const eBuilding = kCiv.buildingAt(i);
		int iChange = getNumActiveBuilding(eBuilding) *
				GET_PLAYER(getOwner()).getExtraBuildingHealth(eBuilding);
		if (iChange > 0)
			iNewExtraBuildingGoodHealth += iChange;
		else iNewExtraBuildingBadHealth += iChange;
	}

	if (getExtraBuildingGoodHealth() != iNewExtraBuildingGoodHealth)
	{
		m_iExtraBuildingGoodHealth = iNewExtraBuildingGoodHealth;
		FAssert(getExtraBuildingGoodHealth() >= 0);
		AI_setAssignWorkDirty(true);
	}
	if (getExtraBuildingBadHealth() != iNewExtraBuildingBadHealth)
	{
		m_iExtraBuildingBadHealth = iNewExtraBuildingBadHealth;
		FAssert(getExtraBuildingBadHealth() <= 0);
		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getBonusGoodHappiness(/* advc.912c: */ bool bIgnoreModifier) const
{
	//return m_iBonusGoodHappiness;
	// <advc.912c> Replacing the above
	int r = m_iBonusGoodHappiness;
	if (!bIgnoreModifier)
		r = (r * (100 + GET_PLAYER(getOwner()).getLuxuryModifier())) / 100;
	return r; // </advc.912c>
}


void CvCity::changeBonusGoodHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBonusGoodHappiness += iChange;
		FAssert(getBonusGoodHappiness() >= 0);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeBonusBadHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBonusBadHappiness += iChange;
		FAssert(getBonusBadHappiness() <= 0);
		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getReligionHappiness(ReligionTypes eReligion) const
{
	int iHappiness = 0;
	if (isHasReligion(eReligion))
	{
		if (eReligion == GET_PLAYER(getOwner()).getStateReligion())
			iHappiness += GET_PLAYER(getOwner()).getStateReligionHappiness();
		else iHappiness += GET_PLAYER(getOwner()).getNonStateReligionHappiness();
	}
	return iHappiness;
}


void CvCity::updateReligionHappiness()
{
	int iNewReligionGoodHappiness = 0;
	int iNewReligionBadHappiness = 0;
	FOR_EACH_ENUM(Religion)
	{
		int iChange = getReligionHappiness(eLoopReligion);
		if (iChange > 0)
			iNewReligionGoodHappiness += iChange;
		else iNewReligionBadHappiness += iChange;
	}

	if (getReligionGoodHappiness() != iNewReligionGoodHappiness)
	{
		m_iReligionGoodHappiness = iNewReligionGoodHappiness;
		FAssert(getReligionGoodHappiness() >= 0);
		AI_setAssignWorkDirty(true);
	}

	if (getReligionBadHappiness() != iNewReligionBadHappiness)
	{
		m_iReligionBadHappiness = iNewReligionBadHappiness;
		FAssert(getReligionBadHappiness() <= 0);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeExtraHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHappiness += iChange;
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeExtraHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHealth += iChange;
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeHurryAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iHurryAngerTimer += iChange;
		FAssert(getHurryAngerTimer() >= 0);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeConscriptAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iConscriptAngerTimer += iChange;
		FAssert(getConscriptAngerTimer() >= 0);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeDefyResolutionAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iDefyResolutionAngerTimer += iChange;
		FAssert(getDefyResolutionAngerTimer() >= 0);
		AI_setAssignWorkDirty(true);
	}
}

int CvCity::flatDefyResolutionAngerLength() const
{
	static int const iDEFY_RESOLUTION_ANGER_DIVISOR = GC.getDefineINT("DEFY_RESOLUTION_ANGER_DIVISOR"); // advc.opt
	int iAnger = iDEFY_RESOLUTION_ANGER_DIVISOR;

	iAnger *= GC.getInfo(GC.getGame().getGameSpeedType()).getHurryConscriptAngerPercent();
	iAnger /= 100;

	return std::max(1, iAnger);
}


void CvCity::changeHappinessTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iHappinessTimer += iChange;
		FAssert(getHappinessTimer() >= 0);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeNoUnhappinessCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoUnhappinessCount += iChange;
		FAssert(getNoUnhappinessCount() >= 0);
		AI_setAssignWorkDirty(true);
	}
}

/*int CvCity::getNoUnhealthyPopulationCount()	const {
	return m_iNoUnhealthyPopulationCount;
}
bool CvCity::isNoUnhealthyPopulation() const {
	if (GET_PLAYER(getOwner()).isNoUnhealthyPopulation())
		return true;
	return (getNoUnhealthyPopulationCount() > 0);
}
void CvCity::changeNoUnhealthyPopulationCount(int iChange) {
	if (iChange != 0) {
		m_iNoUnhealthyPopulationCount += iChange;
		FAssert(getNoUnhealthyPopulationCount() >= 0);
		AI_setAssignWorkDirty(true);
	}
}*/ // BtS
/*  K-Mod, 27/dec/10, karadoc
	replace NoUnhealthyPopulation with UnhealthyPopulationModifier */
int CvCity::getUnhealthyPopulationModifier() const
{
	return m_iUnhealthyPopulationModifier + GET_PLAYER(getOwner()).getUnhealthyPopulationModifier();
}


void CvCity::changeUnhealthyPopulationModifier(int iChange)
{
	m_iUnhealthyPopulationModifier += iChange;
}// K-Mod end


bool CvCity::isBuildingOnlyHealthy() const
 {
	if (GET_PLAYER(getOwner()).isBuildingOnlyHealthy())
		return true;
	return (getBuildingOnlyHealthyCount() > 0);
}


void CvCity::changeBuildingOnlyHealthyCount(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingOnlyHealthyCount += iChange;
		FAssert(getBuildingOnlyHealthyCount() >= 0);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::setFood(int iNewValue)
{
	if (getFood() != iNewValue)
	{
		m_iFood = iNewValue;
		if (isActiveTeam())
			setInfoDirty(true);
	}
}


void CvCity::changeFood(int iChange)
{
	setFood(getFood() + iChange);
}


void CvCity::setFoodKept(int iNewValue)
{
	m_iFoodKept = iNewValue;
}


void CvCity::changeFoodKept(int iChange)
{
	setFoodKept(getFoodKept() + iChange);
}


void CvCity::changeMaxFoodKeptPercent(int iChange)
{
	m_iMaxFoodKeptPercent += iChange;
	FAssert(getMaxFoodKeptPercent() >= 0);
}


void CvCity::setOverflowProduction(int iNewValue)
{
	m_iOverflowProduction = iNewValue;
	FAssert(getOverflowProduction() >= 0);
}


void CvCity::changeOverflowProduction(int iChange, int iProductionModifier)
{
	setOverflowProduction(getOverflowProduction() +
			// advc.064b: Moved into new function
			unmodifyOverflow(iChange, iProductionModifier));
}

// <advc.064b> Cut from changeOverflowProduction
int CvCity::unmodifyOverflow(int iRawOverflow, int iProductionModifier) const
{
	return (100 * iRawOverflow) / std::max(1,
			//getBaseYieldRateModifier(YIELD_PRODUCTION, iProductionModifier)
			/*  Keep the BaseYieldRateModifier; same treatment as Processes, and
				that's what we want for production gold too. However, I'm also
				changing the code that uses up overflow production (see getProductionDifference)
				so that the BaseYieldRateModifier isn't applied for a second time.
				On the bottom line, it's almost the same as what the Unofficial Patch,
				does, but results in more intuitive help text. */
			iProductionModifier + 100);
} // </advc.064b>


void CvCity::setFeatureProduction(int iNewValue)
{
	m_iFeatureProduction = iNewValue;
	FAssert(getFeatureProduction() >= 0);
}


void CvCity::changeFeatureProduction(int iChange)
{
	setFeatureProduction(getFeatureProduction() + iChange);
}


void CvCity::changeMilitaryProductionModifier(int iChange)
{
	m_iMilitaryProductionModifier += iChange;
}


void CvCity::changeSpaceProductionModifier(int iChange)
{
	m_iSpaceProductionModifier += iChange;
}


void CvCity::changeExtraTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraTradeRoutes += iChange;
		FAssert(getExtraTradeRoutes() >= 0);
		updateTradeRoutes();
	}
}


void CvCity::changeTradeRouteModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iTradeRouteModifier += iChange;
		updateTradeRoutes();
	}
}


void CvCity::changeForeignTradeRouteModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iForeignTradeRouteModifier += iChange;
		updateTradeRoutes();
	}
}

// K-Mod, 26/sep/10: Trade culture calculation
int CvCity::getTradeCultureRateTimes100() const // advc: unused param iLevel removed
{
	// <k146>
	//int iPercent = getCultureLevel();
	// Note: GC.getNumCultureLevelInfos() is 7 with the standard xml, which means legendary culture is level 6.
	// So we have 3, 4, 4, 5, 5, 6, 6
	int iPercent = (GC.getNumCultureLevelInfos() + getCultureLevel()) / 2;
	if (iPercent > 0)
	{
		// (originally this was 1% of culture rate for each culture level.) // </k146>
		return (m_aiCommerceRate.get(COMMERCE_CULTURE) * iPercent) / 100;
	}
	return 0;
} // K-Mod end


void CvCity::changeBuildingDefense(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingDefense += iChange;
		FAssert(getBuildingDefense() >= 0);
		setInfoDirty(true);
		getPlot().plotAction(PUF_makeInfoBarDirty);
	}
}

// BUG - Building Additional Defense - start
int CvCity::getAdditionalDefenseByBuilding(BuildingTypes eBuilding) const
{
	CvBuildingInfo& kBuilding = GC.getInfo(eBuilding);
	int iDefense = std::max(
			std::max(kBuilding.get(CvBuildingInfo::RaiseDefense), // advc.004c
			getBuildingDefense() + kBuilding.getDefenseModifier()),
			getNaturalDefense()) +
			GET_PLAYER(getOwner()).getCityDefenseModifier() +
			kBuilding.getAllCityDefenseModifier();

	// doesn't take bombardment into account
	return iDefense - getTotalDefense(false);
} // BUG - Building Additional Defense - end


void CvCity::changeBuildingBombardDefense(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBombardDefense += iChange;
		FAssert(getBuildingBombardDefense() >= 0);
	}
}


void CvCity::changeFreeExperience(int iChange)
{
	m_iFreeExperience += iChange;
	FAssert(getFreeExperience() >= 0);
}


void CvCity::setCurrAirlift(int iNewValue)
{
	m_iCurrAirlift = iNewValue;
	FAssert(getCurrAirlift() >= 0);
}


void CvCity::changeCurrAirlift(int iChange)
{
	setCurrAirlift(getCurrAirlift() + iChange);
}


void CvCity::changeMaxAirlift(int iChange)
{
	m_iMaxAirlift += iChange;
	FAssert(getMaxAirlift() >= 0);
}


void CvCity::changeAirModifier(int iChange)
{
	m_iAirModifier += iChange;
}


int CvCity::getAirUnitCapacity(TeamTypes eTeam) const
{
	return (getTeam() == eTeam ? m_iAirUnitCapacity :
			GC.getDefineINT(CvGlobals::CITY_AIR_UNIT_CAPACITY));
}


void CvCity::changeAirUnitCapacity(int iChange)
{
	m_iAirUnitCapacity += iChange;
	FAssert(getAirUnitCapacity(getTeam()) >= 0);
}


void CvCity::changeNukeModifier(int iChange)
{
	m_iNukeModifier += iChange;
}


void CvCity::changeFreeSpecialist(int iChange)
{
	if (iChange != 0)
	{
		m_iFreeSpecialist += iChange;
		FAssert(getFreeSpecialist() >= 0);
		AI_setAssignWorkDirty(true);
	}
}


bool CvCity::isAreaCleanPower() const
{
	return getArea().isCleanPower(getTeam());
}


bool CvCity::isDirtyPower() const
{
	return (isPower() && getDirtyPowerCount() == getPowerCount() && !isAreaCleanPower());
}


void CvCity::changePowerCount(int iChange, bool bDirty)
{
	if(iChange == 0)
		return;

	bool bOldPower = isPower();
	bool bOldDirtyPower = isDirtyPower();

	m_iPowerCount += iChange;
	FAssert(getPowerCount() >= 0);
	if (bDirty)
	{
		m_iDirtyPowerCount += iChange;
		FAssert(getDirtyPowerCount() >= 0);
	}

	if (bOldPower != isPower())
	{
		GET_PLAYER(getOwner()).invalidateYieldRankCache();
		updateCommerce();
		if (isActiveTeam())
			setInfoDirty(true);
	}

	if (bOldDirtyPower != isDirtyPower() || bOldPower != isPower())
		updatePowerHealth();
}


void CvCity::changeDefenseDamage(int iChange)
{
	if (iChange == 0)
		return;
	m_iDefenseDamage = range(m_iDefenseDamage + iChange, 0, GC.getMAX_CITY_DEFENSE_DAMAGE());
	if (iChange > 0)
		setBombarded(true);
	setInfoDirty(true);
	getPlot().plotAction(PUF_makeInfoBarDirty);
}

void CvCity::changeDefenseModifier(int iChange)
{
	if(iChange == 0)
	{
		setBombarded(true); // advc.004c: Set bombarded even if no change
		return;
	}
	int iTotalDefense = getTotalDefense(false);
	if(iTotalDefense <= 0)
		return;
	//changeDefenseDamage(-(GC.getMAX_CITY_DEFENSE_DAMAGE() * iChange) / iTotalDefense);
	// <advc.004c> Replacing the above
	int iDefenseDmg = getDefenseDamage();
	int iDefenseMod = getDefenseModifier(false);
	iChange = -iDefenseDmg + (GC.getMAX_CITY_DEFENSE_DAMAGE() * iTotalDefense -
			(iDefenseMod + iChange) * GC.getMAX_CITY_DEFENSE_DAMAGE()) / iTotalDefense;
	changeDefenseDamage(iChange);
	// </advc.004c>
}

/*  advc.003j (comment): Not currently used (b/c K-Mod has removed AI_finalOddsThreshold).
	Gets set in CvCity::doTurn. */
int CvCity::getLastDefenseDamage() const
{
	return m_iLastDefenseDamage;
}


void CvCity::setLastDefenseDamage(int iNewValue)
{
	m_iLastDefenseDamage = iNewValue;
}


bool CvCity::isBombardable(const CvUnit* pUnit) const
{
	if (pUnit != NULL && !pUnit->isEnemy(getTeam()))
		return false;

	return (getDefenseModifier(false) > 0 ||
			// advc.004c: Don't give away 0 defense in the fog of war to human attacker
			(pUnit != NULL && pUnit->isHuman() && !isVisible(pUnit->getTeam())) ||
			/*  advc.004c: Allow bombarding defenseless cities (to keep their def at 0),
				except cities that have nothing to recover.
				Don't allow the AI to do this b/c the AI can't tell if it matters.
				Much easier to prevent 0-bombardment by the AI here than in CvUnitAI. */
			(getTotalDefense(false) > 0 && pUnit->isHuman() && !isBombarded()));
}


int CvCity::getNaturalDefense() const
{
	if (getCultureLevel() == NO_CULTURELEVEL)
		return 0;
	return GC.getInfo(getCultureLevel()).getCityDefenseModifier();
}


int CvCity::getTotalDefense(bool bIgnoreBuilding) const
{
	return std::max(bIgnoreBuilding ? 0 : getBuildingDefense(), getNaturalDefense()) +
			GET_PLAYER(getOwner()).getCityDefenseModifier();
}


int CvCity::getDefenseModifier(bool bIgnoreBuilding) const
{
	if (isOccupation())
		return 0;
	return ((getTotalDefense(bIgnoreBuilding) *
			(GC.getMAX_CITY_DEFENSE_DAMAGE() - getDefenseDamage())) /
			GC.getMAX_CITY_DEFENSE_DAMAGE());
}


void CvCity::setOccupationTimer(int iNewValue)
{
	if(getOccupationTimer() == iNewValue)
		return;

	bool bOldOccupation = isOccupation();

	m_iOccupationTimer = iNewValue;
	FAssert(getOccupationTimer() >= 0);

	if (bOldOccupation != isOccupation())
	{
		updateCorporation();
		updateMaintenance();
		updateTradeRoutes();
		updateCommerce(); // K-Mod
		/*	<advc.908b> (Intended for conquered cities; will have no effect
			in other situations.) */
		if (!isOccupation())
			initTraitCulture(); // </advc.908b>
		updateCultureLevel(true);

		AI_setAssignWorkDirty(true);
		// K-Mod
		if (isHuman() && !isDisorder() && isChooseProductionDirty() &&
			!isProduction() && !isProductionAutomated())
		{
			chooseProduction();
		} // K-Mod end
	}

	setInfoDirty(true);
}


void CvCity::changeOccupationTimer(int iChange)
{
	setOccupationTimer(getOccupationTimer() + iChange);
}


void CvCity::setCultureUpdateTimer(int iNewValue)
{
	m_iCultureUpdateTimer = iNewValue;
	FAssert(getOccupationTimer() >= 0);
}


void CvCity::changeCultureUpdateTimer(int iChange)
{
	setCultureUpdateTimer(getCultureUpdateTimer() + iChange);
}


int CvCity::getCitySizeBoost() const
{
	return m_iCitySizeBoost;
}


void CvCity::setCitySizeBoost(int iBoost)
{
	if (getCitySizeBoost() != iBoost)
	{
		m_iCitySizeBoost = iBoost;
		setLayoutDirty(true);
	}
}

// (advc: unused; the Python function also unused)
bool CvCity::isNeverLost() const
{
	return m_bNeverLost;
}


void CvCity::setNeverLost(bool bNewValue)
{
	m_bNeverLost = bNewValue;
}


void CvCity::setBombarded(bool bNewValue)
{
	m_bBombarded = bNewValue;
}


void CvCity::setDrafted(bool bNewValue)
{
	m_bDrafted = bNewValue;
}


void CvCity::setAirliftTargeted(bool bNewValue)
{
	m_bAirliftTargeted = bNewValue;
}


void CvCity::setPlundered(bool bNewValue)
{
	if (bNewValue != isPlundered())
	{
		m_bPlundered = bNewValue;
		updateTradeRoutes();
	}
}


bool CvCity::isWeLoveTheKingDay() const
{
	return m_bWeLoveTheKingDay;
}


void CvCity::setWeLoveTheKingDay(bool bNewValue)
{
	if (isWeLoveTheKingDay() == bNewValue)
		return;

	m_bWeLoveTheKingDay = bNewValue;
	updateMaintenance();
	CivicTypes eCivic = NO_CIVIC;
	FOR_EACH_ENUM(Civic)
	{
		if (GET_PLAYER(getOwner()).isCivic(eLoopCivic))
		{
			if (!CvWString(GC.getInfo(eLoopCivic).getWeLoveTheKing()).empty())
			{
				eCivic = eLoopCivic;
				break;
			}
		}
	}
	if (eCivic != NO_CIVIC)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_CELEBRATE",
				getNameKey(), GC.getInfo(eCivic).getWeLoveTheKing());
		gDLL->UI().addMessage(getOwner(), false, -1, szBuffer, getPlot(),
				"AS2D_WELOVEKING", MESSAGE_TYPE_MINOR_EVENT,
				ARTFILEMGR.getInterfaceArtPath("INTERFACE_HAPPY_PERSON"));
	}
}


void CvCity::setCitizensAutomated(bool bNewValue)
{
	if (isCitizensAutomated() != bNewValue)
	{
		m_bCitizensAutomated = bNewValue;
		if (isCitizensAutomated())
			AI().AI_assignWorkingPlots();
		else
		{
			FOR_EACH_ENUM(Specialist)
				setForceSpecialistCount(eLoopSpecialist, 0);
		}
		if (isActiveOwned() && isCitySelected())
			gDLL->UI().setDirty(SelectionButtons_DIRTY_BIT, true);
	}
}


void CvCity::setProductionAutomated(bool bNewValue, bool bClear)
{
	if (isProductionAutomated() == bNewValue)
		return;

	m_bProductionAutomated = bNewValue;
	if (isActiveOwned() && isCitySelected())
	{
		gDLL->UI().setDirty(SelectionButtons_DIRTY_BIT, true);

		// if automated and not network game and all 3 modifiers down, clear the queue and choose again
		if (bNewValue && bClear)
			clearOrderQueue();
	}
	if (!isProduction())
		AI().AI_chooseProduction();
}

/*	allows you to programmatically specify a cities walls
	rather than having them be generated automagically */
bool CvCity::isWallOverride() const
{
	return m_bWallOverride;
}


void CvCity::setWallOverride(bool bOverride)
{
	if (isWallOverride() != bOverride)
	{
		m_bWallOverride = bOverride;
		setLayoutDirty(true);
	}
}


bool CvCity::isInfoDirty() const
{
	return m_bInfoDirty;
}


void CvCity::setInfoDirty(bool bNewValue)
{
	m_bInfoDirty = bNewValue;
}


bool CvCity::isLayoutDirty() const
{
	return m_bLayoutDirty;
}


void CvCity::setLayoutDirty(bool bNewValue)
{
	m_bLayoutDirty = bNewValue;
}


void CvCity::setPreviousOwner(PlayerTypes eNewValue)
{
	m_ePreviousOwner = eNewValue;
}


void CvCity::setOriginalOwner(PlayerTypes eNewValue)
{
	m_eOriginalOwner = eNewValue;
}


TeamTypes CvCity::getTeam() const
{
	/*  advc.inl (comment): Not inlined b/c I don't want to include
		CvPlayer.h and CvTeam.h in CvCity.h. Should perhaps cache
		the owning team instead (as it's already done for CvPlot::getTeam). */
	return TEAMID(getOwner());
}


int CvCity::getCultureThreshold(CultureLevelTypes eLevel)
{
	if (eLevel == NO_CULTURELEVEL)
		return 1;
	return std::max(1,
			GC.getGame().getCultureThreshold((CultureLevelTypes)
			std::min(eLevel + 1, GC.getNumCultureLevelInfos() - 1)));
}

//doto enhanced city size mylon
// Mylon Mega Mod
CityPlotTypes CvCity::maxCityPlots() const
{
	return GET_PLAYER(m_eOwner).numCityPlots();
}
CityPlotTypes CvCity::cityPlotCountForRadius(int iRadius)
{
	switch (iRadius)
	{
	case 1: return NUM_INNER_PLOTS;
	// (Not defined to be the same as NUM_CITY_PLOTS in this translation unit)
	case 2: return NUM_CITYPLOT_TYPES;
	case 3: return (CityPlotTypes)37;
	case 4: return MAX_CITY_PLOTS;
	default: FErrorMsg("Invalid city radius"); return NO_CITYPLOT;
   	}
}
int CvCity::maxRadius() const
{
	return GET_PLAYER(m_eOwner).cityRadius();
}
void CvCity::updateRadius()
{
	m_iRadius = std::min(maxRadius(),
			getCultureLevel() == NO_CULTURELEVEL ? maxRadius() :
			GC.getCultureLevelInfo(getCultureLevel()).getCityRadius());
	m_eCityPlots = cityPlotCountForRadius(getRadius());
}
// Mylon Mega Mod End

void CvCity::setCultureLevel(CultureLevelTypes eNewValue, bool bUpdatePlotGroups)
{
	CultureLevelTypes const eOldValue = getCultureLevel();
	if (eOldValue == eNewValue)
		return;
	m_eCultureLevel = eNewValue;
//doto enhanced city size mylon - start
	{
		updateRadius();
		int const iNewCityPlots = numCityPlots() - (int)m_abWorkingPlot.size();
		if (iNewCityPlots > 0)
		{
			for (int i = 0; i < iNewCityPlots; i++)
				m_abWorkingPlot.push_back(false);
			/*	Change it back again briefly (not super elegant)
				and reset this city as the city occupying the city plot
				in order to force an update of cached city radius counts.
				A little wasteful, but we don't do this often. */
			if (getPlot().isCity()) // I.e. don't do it during city initialization
			{
				m_eCultureLevel = eOldValue;
				updateRadius();
				getPlot().setPlotCity(NULL);
				m_eCultureLevel = eNewValue;
				updateRadius();
				getPlot().setPlotCity(this);
				FOR_EACH_CITYPLOT
				{
					/*	Normally only done for plots we take ownership of.
						Need to do it also for plots previously owned. */
					getCityIndexPlot(eLoopCityPlot)->updateWorkingCity();
				}
			}
			AI().AI_updateRadius();
		}
		else FAssertMsg(iNewCityPlots == 0, "Shrinking city radius not supported");
	}
//doto enhanced city size mylon
	if (eOldValue != NO_CULTURELEVEL)
	{
		/*	advc (note): The order of processing is important here b/c
			plot ownership won't be cleared so long as a tile is surrounded by
			owned tiles (even when a plot is no longer in range of any city);
			see CvPlot::calculateCulturalOwner. */
		for (ScanLinePlotIterator itPlot(getPlot(), eOldValue);
			itPlot.hasNext(); ++itPlot)
		{
			int iCultureRange = cultureDistance(itPlot.currXDist(), itPlot.currYDist());
			if (iCultureRange > getCultureLevel() && iCultureRange <= eOldValue)
			{
				FAssert(iCultureRange <= GC.getNumCultureLevelInfos());
				itPlot->changeCultureRangeCities(getOwner(), (CultureLevelTypes)
						iCultureRange, -1, bUpdatePlotGroups);
			}
		}
	}
	if (getCultureLevel() != NO_CULTURELEVEL)
	{
		for (ScanLinePlotIterator itPlot(getPlot(), getCultureLevel());
			itPlot.hasNext(); ++itPlot)
		{
			int iCultureRange = cultureDistance(itPlot.currXDist(), itPlot.currYDist());
			if (iCultureRange > eOldValue && iCultureRange <= getCultureLevel())
			{
				FAssert(iCultureRange <= GC.getNumCultureLevelInfos());
				itPlot->changeCultureRangeCities(getOwner(), (CultureLevelTypes)
						iCultureRange, 1, bUpdatePlotGroups);
			}
		}
	}
	if (GC.getGame().isFinalInitialized() && getCultureLevel() > eOldValue &&
		getCultureLevel() > 1)
	{
		CvWString szBuffer(gDLL->getText("TXT_KEY_MISC_BORDERS_EXPANDED", getNameKey()));
		gDLL->UI().addMessage(getOwner(), false, -1, szBuffer, getPlot(),
				"AS2D_CULTUREEXPANDS", MESSAGE_TYPE_MINOR_EVENT,
				GC.getInfo(COMMERCE_CULTURE).getButton());
		// <advc.106>
		// To replace hardcoded getCultureLevel()==GC.getNumCultureLevelInfos()-1
		int iVictoryCultureLevel = NO_CULTURELEVEL;
		FOR_EACH_ENUM(Victory)
		{
			if (!GC.getGame().isVictoryValid(eLoopVictory))
				continue;
			int iLevel = GC.getInfo(eLoopVictory).getCityCulture();
			if (iLevel > 0 && (iVictoryCultureLevel <= 0 || iVictoryCultureLevel > iLevel))
				iVictoryCultureLevel = iLevel;
		}
		if (getCultureLevel() == iVictoryCultureLevel && iVictoryCultureLevel > 0)
		{	// Cut from below. Use this for the replay message as well.
			CvWString szMsg(gDLL->getText("TXT_KEY_MISC_CULTURE_LEVEL", getNameKey(),
					GC.getInfo(getCultureLevel()).getTextKeyWide()));
			// </advc.106>
			for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
			{
				CvPlayer const& kObs = *it;
				if (isRevealed(kObs.getTeam()) /* advc.127: */ || kObs.isSpectator())
				{
					gDLL->UI().addMessage(kObs.getID(), false, -1, szMsg, getPlot(),
							"AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT,
							GC.getInfo(COMMERCE_CULTURE).getButton(),
							GC.getColorType("HIGHLIGHT_TEXT"));
				}
				else
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_CULTURE_LEVEL_UNKNOWN",
							GC.getInfo(getCultureLevel()).getTextKeyWide());
					gDLL->UI().addMessage(kObs.getID(), false, -1, szBuffer,
							"AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT,
							GC.getInfo(COMMERCE_CULTURE).getButton(),
							GC.getColorType("HIGHLIGHT_TEXT"));
				}
			} // <advc.106>
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwner(), szMsg,
					getX(), getY(), GC.getColorType("HIGHLIGHT_TEXT")); // </advc.106>
		}
		// ONEVENT - Culture growth
		CvEventReporter::getInstance().cultureExpansion(this, getOwner());
		//Stop Build Culture
		/*if (isProductionProcess()) {
			if (GC.getInfo(getProductionProcess()).getProductionToCommerceModifier(COMMERCE_CULTURE) > 0)
					popOrder(0, false, true);
		}*/ // BtS
		/*	K-Mod does this in a different way, to avoid an overflow bug.
			(And a different way to the Unofficial Patch, to avoid OOS) */
	}
}


void CvCity::updateCultureLevel(bool bUpdatePlotGroups)
{
	if (getCultureUpdateTimer() > 0)
		return;

	CultureLevelTypes eCultureLevel = (CultureLevelTypes)0;
	if (!isOccupation())
	{	// advc: Moved into subroutine
		eCultureLevel = calculateCultureLevel(getOwner());
	}
	setCultureLevel(eCultureLevel, bUpdatePlotGroups);
}

/*	advc: Cut from updateCultureLevel. Unlike getCulture(PlayerTypes), this function
	will always recalculate the culture level. */
CultureLevelTypes CvCity::calculateCultureLevel(PlayerTypes ePlayer) const
{
	int const iCultureTimes100 = getCultureTimes100(ePlayer);
	FOR_EACH_ENUM_REV(CultureLevel)
	{
		if(iCultureTimes100 >= 100 * GC.getGame().getCultureThreshold(eLoopCultureLevel))
			return eLoopCultureLevel;
	}
	FErrorMsg("No culture level threshold met");
	return (CultureLevelTypes)0;
}

// advc:
CultureLevelTypes CvCity::getCultureLevel(PlayerTypes ePlayer) const
{
	if (ePlayer == getOwner())
		return getCultureLevel();
	return calculateCultureLevel(ePlayer);
}

// advc.042: Mostly cut and pasted from CvDLLWidgetData::parseCultureHelp
int CvCity::getCultureTurnsLeft() const
{
	int iCultureRateTimes100 = getCommerceRateTimes100(COMMERCE_CULTURE);
	if(iCultureRateTimes100 <= 0)
		return -1;
	int iCultureLeftTimes100 = 100 * getCultureThreshold()
			- getCultureTimes100(getOwner());
	if(iCultureLeftTimes100 <= 0)
		return -1;
	int iR = (iCultureLeftTimes100 + iCultureRateTimes100 - 1) / iCultureRateTimes100;
	FAssert(iR != 0);
	return iR;
}

// advc.908b:
void CvCity::initTraitCulture()
{
	FOR_EACH_ENUM(Trait)
	{
		if (!GET_PLAYER(getOwner()).hasTrait(eLoopTrait))
			continue;
		int iFreeCityCulture = GC.getGame().freeCityCultureFromTrait(eLoopTrait)
				- getCulture(getOwner());
		if (iFreeCityCulture > 0)
			setCulture(getOwner(), iFreeCityCulture, true, false);
	}
}


void CvCity::changeSeaPlotYield(YieldTypes eYield, int iChange)
{
	if (iChange != 0)
	{
		m_aiSeaPlotYield.add(eYield, iChange);
		FAssert(getSeaPlotYield(eYield) >= 0);
		updateYield();
	}
}


void CvCity::changeRiverPlotYield(YieldTypes eYield, int iChange)
{
	if (iChange != 0)
	{
		m_aiRiverPlotYield.add(eYield, iChange);
		FAssert(getRiverPlotYield(eYield) >= 0);
		updateYield();
	}
}

// BUG - Building Additional Yield - start
/*	Returns the total additional yield that adding one of the given buildings will provide.
	Doesn't check if the building can be constructed in this city. */
int CvCity::getAdditionalYieldByBuilding(YieldTypes eYield, BuildingTypes eBuilding) const
{
	int iRate = getBaseYieldRate(eYield);
	int iModifier = getBaseYieldRateModifier(eYield);
	int iExtra = ((iRate + getAdditionalBaseYieldRateByBuilding(eYield, eBuilding)) *
		(iModifier + getAdditionalYieldRateModifierByBuilding(eYield, eBuilding)) / 100) -
		(iRate * iModifier / 100);
	return iExtra;
}

/*	Returns the additional yield rate that adding one of the given buildings will provide.
	Doesn't check if the building can be constructed in this city.
	advc: _MOD_FRACTRADE removed */
int CvCity::getAdditionalBaseYieldRateByBuilding(YieldTypes eYield,
	BuildingTypes eBuilding) const
{
	if (GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding))
		return 0;

	int iExtraRate = 0;
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	// <advc.179> Some overlap with code in CvGameTextMgr::buildBuildingReligionYieldString
	CvCivilization const& kCiv = getCivilization();
	FOR_EACH_ENUM2(VoteSource, eVS)
	{
		if (eVS != kBuilding.getVoteSourceType())
			continue;
		CvVoteSourceInfo& kVS = GC.getInfo(eVS);
		if (kVS.getReligionYield(eYield) != 0)
		{
			ReligionTypes eVSReligion =
					// This would be NO_RELIGION b/c the vote source doesn't exist yet:
					//GC.getGame().getVoteSourceReligion(eVS);
					GET_PLAYER(getOwner()).getStateReligion();
			if (eVSReligion != NO_RELIGION)
			{
				// Akin to processVoteSource
				for(int i = 0; i < kCiv.getNumBuildings(); i++)
				{
					BuildingTypes eBuilding = kCiv.buildingAt(i);
					if(getNumBuilding(eBuilding) > 0 &&
						!GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding))
					{
						if(GC.getInfo(eBuilding).getReligionType() == eVSReligion)
							iExtraRate += kVS.getReligionYield(eYield);
					}
				}
			}
		}
	} // </advc.179>
	if (kBuilding.getSeaPlotYieldChange(eYield) != 0)
	{
		int iChange = kBuilding.getSeaPlotYieldChange(eYield);
		for (WorkingPlotIter it(*this); it.hasNext(); ++it)
		{
			if (it->isWater())
				iExtraRate += iChange;
		}
	}
	if (kBuilding.getRiverPlotYieldChange(eYield) != 0)
	{
		int iChange = kBuilding.getRiverPlotYieldChange(eYield);
		for (WorkingPlotIter it(*this); it.hasNext(); ++it)
		{
			if (it->isRiver())
				iExtraRate += iChange;
		}
	}
	iExtraRate += kBuilding.getYieldChange(eYield);
	iExtraRate += getBuildingYieldChange(kBuilding.getBuildingClassType(), eYield);

	// Trade
	int iPlayerTradeYieldModifier = GET_PLAYER(getOwner()).getTradeYieldModifier(eYield);
	if (iPlayerTradeYieldModifier > 0 && (kBuilding.getTradeRouteModifier() != 0 ||
		kBuilding.getForeignTradeRouteModifier() != 0))
	{
		int iTotalTradeYield = 0;
		int iNewTotalTradeYield = 0;
		int iTradeProfitDivisor = 10000;
		for (int iI = 0; iI < getTradeRoutes(); ++iI)
		{
			CvCity* pCity = getTradeCity(iI);
			if (pCity == NULL)
				continue;
			int iTradeProfit = getBaseTradeProfit(pCity);
			int iTradeModifier = totalTradeModifier(pCity);
			int iTradeYield = ((iTradeProfit * iTradeModifier) / iTradeProfitDivisor) *
					(iPlayerTradeYieldModifier / 100);
			iTotalTradeYield += iTradeYield;
			iTradeModifier += kBuilding.getTradeRouteModifier();
			if (pCity->getOwner() != getOwner())
				iTradeModifier += kBuilding.getForeignTradeRouteModifier();
			int iNewTradeYield = ((iTradeProfit * iTradeModifier) / iTradeProfitDivisor) *
					(iPlayerTradeYieldModifier / 100);
			iNewTotalTradeYield += iNewTradeYield;
		}
		iExtraRate += iNewTotalTradeYield - iTotalTradeYield;
	}

	FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
		getFreeSpecialistCount(), Specialist, int)
	{
		iExtraRate += getAdditionalBaseYieldRateBySpecialist(eYield,
				perSpecialistVal.first, perSpecialistVal.second);
	}

	return iExtraRate;
}

/*	Returns the additional yield rate modifier that adding one of
	the given buildings will provide. Doesn't check if the building can be
	constructed in this city. */
int CvCity::getAdditionalYieldRateModifierByBuilding(YieldTypes eYield,
	BuildingTypes eBuilding) const
{
	if (GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding))
		return 0; // advc

	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	int iExtraModifier = kBuilding.getYieldModifier(eYield);
	if (isPower())
		iExtraModifier += kBuilding.getPowerYieldModifier(eYield);
	else
	{
		if (kBuilding.isPower() || kBuilding.isAreaCleanPower() ||
			(kBuilding.getPowerBonus() != NO_BONUS &&
			hasBonus(kBuilding.getPowerBonus())))
		{
			CvCivilization const& kCiv = getCivilization();
			for (int i = 0; i < kCiv.getNumBuildings(); i++)
			{
				BuildingTypes eBuilding = kCiv.buildingAt(i);
				iExtraModifier += getNumActiveBuilding(eBuilding) *
						GC.getInfo(eBuilding).getPowerYieldModifier(eYield);
			}
		}
	}
	if (eYield == YIELD_PRODUCTION)
	{
		iExtraModifier += kBuilding.getMilitaryProductionModifier();
		iExtraModifier += kBuilding.getSpaceProductionModifier();
		iExtraModifier += kBuilding.getGlobalSpaceProductionModifier();

		int iMaxModifier = 0;
		FOR_EACH_ENUM(Domain)
		{
			iMaxModifier = std::max<int>(iMaxModifier,
					kBuilding.getDomainProductionModifier(eLoopDomain));
		}
		iExtraModifier += iMaxModifier;
	}
	FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
		getBonusYieldModifier(), Bonus, YieldPercentMap)
	{
		if (hasBonus(perBonusVal.first))
			iExtraModifier += perBonusVal.second[eYield];
	}
	return iExtraModifier;
}
// BUG - Building Additional Yield - end

// BUG - Specialist Additional Yield - start
/*	Returns the total additional yield that changing the number
	of given specialists will provide/remove. */
int CvCity::getAdditionalYieldBySpecialist(YieldTypes eYield, SpecialistTypes eSpecialist, int iChange) const
{
	int iRate = getBaseYieldRate(eYield);
	int iModifier = getBaseYieldRateModifier(eYield);
	int iExtra = ((iRate + getAdditionalBaseYieldRateBySpecialist(eYield, eSpecialist, iChange)) *
			iModifier / 100) - (iRate * iModifier / 100);
	return iExtra;
}

/*	Returns the additional yield rate that changing the number
	of given specialists will provide/remove. */
int CvCity::getAdditionalBaseYieldRateBySpecialist(YieldTypes eYield, SpecialistTypes eSpecialist, int iChange) const
{
	CvSpecialistInfo& kSpecialist = GC.getInfo(eSpecialist);
	return iChange * (kSpecialist.getYieldChange(eYield) +
			GET_PLAYER(getOwner()).getSpecialistExtraYield(eSpecialist, eYield));
}
// BUG - Specialist Additional Yield - end

int CvCity::getBaseYieldRateModifier(YieldTypes eYield, int iExtra) const
{
	int iModifier = getYieldRateModifier(eYield);
	iModifier += getBonusYieldRateModifier(eYield);

	if (isPower())
		iModifier += getPowerYieldRateModifier(eYield);

	iModifier += getArea().getYieldRateModifier(getOwner(), eYield);
	iModifier += GET_PLAYER(getOwner()).getYieldRateModifier(eYield);
// DOTO-< Civic Infos Plus Start >
    if (GET_PLAYER(getOwner()).getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(GET_PLAYER(getOwner()).getStateReligion()))
		{
			iModifier += getStateReligionYieldRateModifier(eYield);
		}

	else
		{
			iModifier += getNonStateReligionYieldRateModifier(eYield);
		}
	}

// DOTO-< Civic Infos Plus End   >

	if (isCapital())
		iModifier += GET_PLAYER(getOwner()).getCapitalYieldRateModifier(eYield);

	iModifier += iExtra;

	// note: player->invalidateYieldRankCache() must be called for anything that is checked here
	// so if any extra checked things are added here, the cache needs to be invalidated

	return std::max(0, iModifier + 100);
}


void CvCity::setBaseYieldRate(YieldTypes eYield, int iNewValue)
{
	if (getBaseYieldRate(eYield) == iNewValue)
		return; // advc

	FAssert(iNewValue >= 0);
	//FAssert((iNewValue * 100) / 100 >= 0); // advc: ??

	m_aiBaseYieldRate.set(eYield, iNewValue);
	FAssert(getYieldRate(eYield) >= 0);

	updateCommerce();

	if (isActiveTeam())
	{
		setInfoDirty(true);
		if (isCitySelected())
		{
			gDLL->UI().setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->UI().setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}


void CvCity::changeBaseYieldRate(YieldTypes eYield, int iChange)
{
	setBaseYieldRate(eYield, getBaseYieldRate(eYield) + iChange);
}

// DOTO-< Civic Infos Plus Start >
void CvCity::updateBuildingYieldChange(CivicTypes eCivic, int iChange)
{

    int iBuildingYieldChange;
	int iI, iJ;

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		iBuildingYieldChange = 0;

		for (iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)

        {
			iBuildingYieldChange = getNumActiveBuilding((BuildingTypes)iJ) * GC.getCivicInfo(eCivic).getBuildingYieldChanges((BuildingTypes)iJ, (YieldTypes)iI) * iChange;

            changeBuildingYieldChange((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iJ).getBuildingClassType(), (YieldTypes)iI, iBuildingYieldChange);
        }
	}

	AI_setAssignWorkDirty(true);

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		setInfoDirty(true);
	}
}
// DOTO-< Civic Infos Plus End   >

void CvCity::changeYieldRateModifier(YieldTypes eYield, int iChange)
{
	if (iChange == 0)
		return;

	m_aiYieldRateModifier.add(eYield, iChange);
	FAssert(getYieldRate(eYield) >= 0);
	GET_PLAYER(getOwner()).invalidateYieldRankCache(eYield);

	if (eYield == YIELD_COMMERCE)
		updateCommerce();
	AI_setAssignWorkDirty(true);

	if (isActiveTeam())
		setInfoDirty(true);
}


void CvCity::changePowerYieldRateModifier(YieldTypes eYield, int iChange)
{
	if (iChange == 0)
		return;

	m_aiPowerYieldRateModifier.add(eYield, iChange);
	FAssert(getYieldRate(eYield) >= 0);
	GET_PLAYER(getOwner()).invalidateYieldRankCache(eYield);

	if (eYield == YIELD_COMMERCE)
		updateCommerce();
	AI_setAssignWorkDirty(true);

	if (isActiveTeam())
		setInfoDirty(true);
}

// DOTO-< Civic Infos Plus Start >
//change from f1 advc to acctually work - keldath
int CvCity::getStateReligionYieldRateModifier(YieldTypes eYield) const {
   return GET_PLAYER(getOwner()).getStateReligionYieldRateModifier(eYield);
}
/*
int CvCity::getStateReligionYieldRateModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiStateReligionYieldRateModifier[eIndex];
}
*/

void CvCity::changeStateReligionYieldRateModifier(YieldTypes eYield, int iChange)
{
	FAssertMsg(eYield >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		//no need anymore - f1 advc
		//m_aiStateReligionYieldRateModifier[eIndex] = (m_aiStateReligionYieldRateModifier[eIndex] + iChange);
		FAssert(getYieldRate(eYield) >= 0);

		GET_PLAYER(getOwner()).invalidateYieldRankCache(eYield);

		if (eYield == YIELD_COMMERCE)
		{
			updateCommerce();
		}

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

//changed from f1 advc to acctualy work - keldath
int CvCity::getNonStateReligionYieldRateModifier(YieldTypes eYield) const {
   return GET_PLAYER(getOwner()).getNonStateReligionYieldRateModifier(eYield);
}
/*
int CvCity::getNonStateReligionYieldRateModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");
	return m_aiNonStateReligionYieldRateModifier[eIndex];
}
*/

void CvCity::changeNonStateReligionYieldRateModifier(YieldTypes eYield, int iChange)
{
	FAssertMsg(eYield >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eYield < NUM_YIELD_TYPES, "eIndex expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		//no need anymore - f1 advc
		//m_aiNonStateReligionYieldRateModifier[eIndex] = (m_aiNonStateReligionYieldRateModifier[eIndex] + iChange);
		FAssert(getYieldRate(eYield) >= 0);

		GET_PLAYER(getOwner()).invalidateYieldRankCache(eYield);

		if (eYield == YIELD_COMMERCE)
		{
			updateCommerce();
		}

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}
// DOTO-< Civic Infos Plus End   >
void CvCity::changeBonusYieldRateModifier(YieldTypes eYield, int iChange)
{
	if (iChange == 0)
		return;

	m_aiBonusYieldRateModifier.add(eYield, iChange);
	FAssert(getYieldRate(eYield) >= 0);
	GET_PLAYER(getOwner()).invalidateYieldRankCache(eYield);

	if (eYield == YIELD_COMMERCE)
		updateCommerce();
	AI_setAssignWorkDirty(true);

	if (isActiveTeam())
		setInfoDirty(true);
}


int CvCity::totalTradeModifier(CvCity const* pOtherCity) const
{
	static int const iCAPITAL_TRADE_MODIFIER = GC.getDefineINT("CAPITAL_TRADE_MODIFIER"); // advc.opt
	int iModifier = 100;

	iModifier += getTradeRouteModifier();
	iModifier += getPopulationTradeModifier();

	if (isConnectedToCapital())
		iModifier += iCAPITAL_TRADE_MODIFIER;

	if (pOtherCity != NULL)
	{
		if (!sameArea(*pOtherCity))
			iModifier += GC.getDefineINT(CvGlobals::OVERSEAS_TRADE_MODIFIER);

		if (getTeam() != pOtherCity->getTeam())
		{
			iModifier += getForeignTradeRouteModifier();
			iModifier += getPeaceTradeModifier(pOtherCity->getTeam());
		}
	}

	return iModifier;
}


int CvCity::getPopulationTradeModifier() const
{
	static int const iOUR_POPULATION_TRADE_MODIFIER = GC.getDefineINT("OUR_POPULATION_TRADE_MODIFIER"); // advc.opt
	static int const iOUR_POPULATION_TRADE_MODIFIER_OFFSET = GC.getDefineINT("OUR_POPULATION_TRADE_MODIFIER_OFFSET"); // advc.opt
	return std::max(0, (getPopulation() + iOUR_POPULATION_TRADE_MODIFIER_OFFSET) * iOUR_POPULATION_TRADE_MODIFIER);
}


int CvCity::getPeaceTradeModifier(TeamTypes eTeam) const
{
	FAssert(NO_TEAM != eTeam);
	FAssert(eTeam != getTeam());

	if (atWar(eTeam, getTeam()))
		return 0;

	static int const iFOREIGN_TRADE_FULL_CREDIT_PEACE_TURNS = GC.getDefineINT("FOREIGN_TRADE_FULL_CREDIT_PEACE_TURNS"); // advc.opt
	static int const iFOREIGN_TRADE_MODIFIER = GC.getDefineINT("FOREIGN_TRADE_MODIFIER"); // advc.opt
	int iPeaceTurns = std::min(iFOREIGN_TRADE_FULL_CREDIT_PEACE_TURNS, GET_TEAM(getTeam()).
			//AI_getAtPeaceCounter(eTeam)
			getTurnsAtPeace(eTeam)); // advc.130k

	if (GC.getGame().getElapsedGameTurns() <= iPeaceTurns)
		return iFOREIGN_TRADE_MODIFIER;

	return (iFOREIGN_TRADE_MODIFIER * iPeaceTurns) /
			std::max(1, iFOREIGN_TRADE_FULL_CREDIT_PEACE_TURNS);
}


int CvCity::getBaseTradeProfit(CvCity const* pCity) const // advc: const CvCity*
{
	static int const iTHEIR_POPULATION_TRADE_PERCENT = GC.getDefineINT("THEIR_POPULATION_TRADE_PERCENT"); // advc.opt
	static int const iTRADE_PROFIT_PERCENT = GC.getDefineINT("TRADE_PROFIT_PERCENT"); // advc.opt
	int iProfit = std::min(pCity->getPopulation() * iTHEIR_POPULATION_TRADE_PERCENT,
			plotDistance(getX(), getY(), pCity->getX(), pCity->getY()) *
			GC.getInfo(GC.getMap().getWorldSize()).getTradeProfitPercent());

	iProfit *= iTRADE_PROFIT_PERCENT;
	iProfit /= 100;

	iProfit = std::max(100, iProfit);
	return iProfit;
}


int CvCity::calculateTradeProfitTimes100(CvCity const* pCity) const // advc: const CvCity*
{
	int iProfit = getBaseTradeProfit(pCity);
	iProfit *= totalTradeModifier(pCity);
	iProfit /= /*10000*/100; // advc.004: Increase precision; function renamed accordingly.
	return iProfit;
}


int CvCity::calculateTradeYield(YieldTypes eYield, int iTradeProfit) const
{
	if (iTradeProfit > 0 && GET_PLAYER(getOwner()).getTradeYieldModifier(eYield) > 0)
		return (iTradeProfit * GET_PLAYER(getOwner()).getTradeYieldModifier(eYield)) / 100;
	return 0;
}

// BULL - Trade Hover:  (advc: simplified a bit, _MOD_FRACTRADE removed)
/*  Adds the yield and count for each trade route with eWithPlayer to the
	int references (out parameters). */
void CvCity::calculateTradeTotals(YieldTypes eYield, int& iDomesticYield, int& iDomesticRoutes,
	int& iForeignYield, int& iForeignRoutes, PlayerTypes eWithPlayer) const
{
	if(isDisorder())
		return;

	int iNumTradeRoutes = getTradeRoutes();
	for (int i = 0; i < iNumTradeRoutes; i++)
	{
		CvCity* pTradeCity = getTradeCity(i);
		if (pTradeCity != NULL && (eWithPlayer == NO_PLAYER ||
			pTradeCity->getOwner() == eWithPlayer))
		{
			int iTradeYield = calculateTradeYield(//YIELD_COMMERCE
					eYield, // advc.001
					calculateTradeProfit(pTradeCity));
			if (pTradeCity->getOwner() == getOwner())
			{
				iDomesticYield += iTradeYield;
				iDomesticRoutes++;
			}
			else
			{
				iForeignYield += iTradeYield;
				iForeignRoutes++;
			}
		}
	}
}


void CvCity::setTradeYield(YieldTypes eYield, int iNewValue)
{
	int const iOldValue = getTradeYield(eYield);
	if (iOldValue != iNewValue)
	{
		m_aiTradeYield.set(eYield, iNewValue);
		FAssert(getTradeYield(eYield) >= 0);
		changeBaseYieldRate(eYield, iNewValue - iOldValue);
	}
}


int CvCity::getExtraSpecialistYield(YieldTypes eYield, SpecialistTypes eSpecialist) const
{
	return (getSpecialistCount(eSpecialist) + getFreeSpecialistCount(eSpecialist)) *
			GET_PLAYER(getOwner()).getSpecialistExtraYield(eSpecialist, eYield);
}


void CvCity::updateExtraSpecialistYield(YieldTypes eYield)
{
	int iNewYield = 0;
	FOR_EACH_ENUM(Specialist)
		iNewYield += getExtraSpecialistYield(eYield, eLoopSpecialist);

	int const iChange = iNewYield - getExtraSpecialistYield(eYield); // advc
	if (iChange != 0)
	{
		m_aiExtraSpecialistYield.set(eYield, iNewYield);
		FAssert(getExtraSpecialistYield(eYield) >= 0);
		changeBaseYieldRate(eYield, iChange);
	}
}


void CvCity::updateExtraSpecialistYield()
{
	FOR_EACH_ENUM(Yield)
		updateExtraSpecialistYield(eLoopYield);
}
/*************************************************************************************************/
/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
/**																								**/
/**																								**/
/*************************************************************************************************/	
int CvCity::getSpecialistCivicExtraCommerce(CommerceTypes eCommerce) const
{
	//FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	//FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiExtraSpecialistCommerce.get(eCommerce);
}

int CvCity::getSpecialistCivicExtraCommerceBySpecialist(CommerceTypes eCommerce, SpecialistTypes eSpecialist) const
{
	/*FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos expected to be >= 0");
	*/return ((getSpecialistCount(eSpecialist) + getFreeSpecialistCount(eSpecialist)) * GET_PLAYER(getOwner()).getSpecialistCivicExtraCommerce(eSpecialist, eCommerce));
}

void CvCity::updateSpecialistCivicExtraCommerce(CommerceTypes eCommerce)
{
	int iOldCommerce;
	int iNewCommerce;
	int iI;

	FAssertMsg(eCommerce >= 0, "eCommerce expected to be >= 0");
	FAssertMsg(eCommerce< NUM_COMMERCE_TYPES, "eCommerce expected to be < NUM_COMMERCE_TYPES");

	iOldCommerce = getSpecialistCivicExtraCommerce(eCommerce);

	iNewCommerce = 0;

	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iNewCommerce += getSpecialistCivicExtraCommerceBySpecialist(eCommerce, ((SpecialistTypes)iI));
	}

	if (iOldCommerce != iNewCommerce)
	{
		//method changed by f1rpo //keldath -done
		m_aiExtraSpecialistCommerce.set(eCommerce, iNewCommerce);
		FAssert(getSpecialistCivicExtraCommerce(eCommerce) >= 0);

		changeSpecialistCommerce(eCommerce, (iNewCommerce - iOldCommerce));
	}
}

void CvCity::updateSpecialistCivicExtraCommerce()
{
	int iI;
	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		updateSpecialistCivicExtraCommerce((CommerceTypes)iI);
	}
}
/*************************************************************************************************/
/**	CMEDIT: End																					**/
/*************************************************************************************************/

int CvCity::getCommerceRate(CommerceTypes eCommerce) const
{
	return getCommerceRateTimes100(eCommerce) / 100;
}


int CvCity::getCommerceRateTimes100(CommerceTypes eCommerce) const
{
	int iRate = m_aiCommerceRate.get(eCommerce);
	if (GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		if (eCommerce == COMMERCE_CULTURE)
			iRate += m_aiCommerceRate.get(COMMERCE_ESPIONAGE);
		else if (eCommerce == COMMERCE_ESPIONAGE)
			iRate = 0;
	}
	return iRate;
}


int CvCity::getCommerceFromPercent(CommerceTypes eCommerce, int iYieldRate) const
{
	int iCommerceRate = (iYieldRate *
			GET_PLAYER(getOwner()).getCommercePercent(eCommerce)) / 100;
	if (eCommerce == COMMERCE_GOLD)
	{
		iCommerceRate += (iYieldRate - iCommerceRate -
				getCommerceFromPercent(COMMERCE_RESEARCH, iYieldRate) -
				getCommerceFromPercent(COMMERCE_CULTURE, iYieldRate) -
				getCommerceFromPercent(COMMERCE_ESPIONAGE, iYieldRate));
	}
	return iCommerceRate;
}


int CvCity::getBaseCommerceRateTimes100(CommerceTypes eCommerce) const
{
	int iBaseCommerceRate = getCommerceFromPercent(eCommerce,
			getYieldRate(YIELD_COMMERCE) * 100);
	iBaseCommerceRate += 100 * ((getSpecialistPopulation() + getNumGreatPeople()) *
			GET_PLAYER(getOwner()).getSpecialistExtraCommerce(eCommerce));
	iBaseCommerceRate += 100 *
			(getBuildingCommerce(eCommerce) +
			getSpecialistCommerce(eCommerce) +
			getReligionCommerce(eCommerce) +
			getCorporationCommerce(eCommerce) +
			GET_PLAYER(getOwner()).getFreeCityCommerce(eCommerce));
	return iBaseCommerceRate;
}


int CvCity::getTotalCommerceRateModifier(CommerceTypes eCommerce) const
{
	CvPlayer const& kOwner = GET_PLAYER(getOwner());
	return std::max(0, getCommerceRateModifier(eCommerce) +
			kOwner.getCommerceRateModifier(eCommerce) +
// < Civic Infos Plus Start >
//changed by f1 advc for civc info plus code - keldath      
           ((kOwner.getStateReligion() != NO_RELIGION
           && isHasReligion(kOwner.getStateReligion())) ?
           getStateReligionCommerceRateModifier(eCommerce) :
           getNonStateReligionCommerceRateModifier(eCommerce)) +
			(isCapital() ? kOwner.getCapitalCommerceRateModifier(eCommerce) : 0) + 100);
}


void CvCity::updateCommerce(CommerceTypes eCommerce)
{
	{
		int const iOldRate = m_aiCommerceRate.get(eCommerce);
		int iNewRate = 0;
		if (!isDisorder())
		{
			iNewRate += (getBaseCommerceRateTimes100(eCommerce) *
					getTotalCommerceRateModifier(eCommerce)) / 100;
			iNewRate += getYieldRate(YIELD_PRODUCTION) *
					getProductionToCommerceModifier(eCommerce);
		}
		if (iOldRate == iNewRate)
			return;

		m_aiCommerceRate.set(eCommerce, iNewRate);
		FAssert(m_aiCommerceRate.get(eCommerce) >= 0);
		GET_PLAYER(getOwner()).changeCommerceRateTimes100(eCommerce,
				iNewRate - iOldRate);
	}
	GET_PLAYER(getOwner()).invalidateCommerceRankCache(eCommerce);
	if (isCitySelected())
	{
		gDLL->UI().setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->UI().setDirty(CityScreen_DIRTY_BIT, true);
	}
}


void CvCity::updateCommerce()
{
	GET_PLAYER(getOwner()).invalidateYieldRankCache();
	FOR_EACH_ENUM(Commerce)
		updateCommerce(eLoopCommerce);
}
// < Civic Infos Plus Start >

void CvCity::updateBuildingCommerceChange(CivicTypes eCivic, int iChange)
{
    int iBuildingCommerceChange;
	int iI, iJ;

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		iBuildingCommerceChange = 0;

		for (iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)

        {
			iBuildingCommerceChange = getNumActiveBuilding((BuildingTypes)iJ) * GC.getCivicInfo(eCivic).getBuildingCommerceChanges((BuildingTypes)iJ, (CommerceTypes)iI) * iChange;

            changeBuildingCommerceChange((BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iJ).getBuildingClassType(), (CommerceTypes)iI, iBuildingCommerceChange);
        }
	}

	AI_setAssignWorkDirty(true);

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		setInfoDirty(true);
	}
}
// < Civic Infos Plus End   >

void CvCity::changeProductionToCommerceModifier(CommerceTypes eCommerce, int iChange)
{
	if (iChange != 0)
	{
		m_aiProductionToCommerceModifier.add(eCommerce, iChange);
		updateCommerce(eCommerce);
	}
}


int CvCity::getBuildingCommerceByBuilding(CommerceTypes eCommerce, BuildingTypes eBuilding) const
{
	/*	K-Mod. I've rearranged some stuff so that bonus commerce does
		not get doubled at the end. (eg. the bonus culture that the Sistine Chapel
		gives to religious buildings should not be doubled.) */

	if (getNumBuilding(eBuilding) <= 0)
		return 0;
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	if (kBuilding.isCommerceChangeOriginalOwner(eCommerce) &&
		getBuildingOriginalOwner(eBuilding) != getOwner())
	{
		return 0;
	}

	int iTimeFactor = 1;
	// <advc.098>
	bool const bDelayedCommerceIncrease =
			(kBuilding.getCommerceChangeDoubleTime(eCommerce) > 0); // </advc.098>
	if (bDelayedCommerceIncrease &&
		getBuildingOriginalTime(eBuilding) != iBuildingOriginalTimeUnknown &&
		GC.getGame().getGameTurnYear() - getBuildingOriginalTime(eBuilding) >=
		kBuilding.getCommerceChangeDoubleTime(eCommerce))
	{
		iTimeFactor = 2;
	}
	/*	there are just two components which get multiplied by the time factor:
		1) the "safe" commerce and 2) the standard commerce;
		the rest of the components are bonuses which should not be doubled. */

	int iCommerceRate = kBuilding.getObsoleteSafeCommerceChange(eCommerce) *
			getNumBuilding(eBuilding);
	// <advc.098>
	if (bDelayedCommerceIncrease &&
		GC.getDefineBOOL(CvGlobals::DOUBLE_OBSOLETE_BUILDING_COMMERCE))
	{
		if (GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding))
			iCommerceRate *= 2;
	}
	else // </advc.098>
		iCommerceRate *= iTimeFactor; // 1)

	if (getNumActiveBuilding(eBuilding) > 0)
	{
		iCommerceRate += kBuilding.getCommerceChange(eCommerce) *
				getNumActiveBuilding(eBuilding) * iTimeFactor; // 2)
		iCommerceRate += getBuildingCommerceChange(
				kBuilding.getBuildingClassType(), eCommerce) *
				getNumActiveBuilding(eBuilding);

		if (kBuilding.getReligionType() != NO_RELIGION &&
			kBuilding.getReligionType() == GET_PLAYER(getOwner()).getStateReligion())
		{
			iCommerceRate += GET_PLAYER(getOwner()).
					getStateReligionBuildingCommerce(eCommerce) *
					getNumActiveBuilding(eBuilding);
		}
		{
			ReligionTypes const eGlobalCommerceReligion = kBuilding.getGlobalReligionCommerce();
			if (eGlobalCommerceReligion != NO_RELIGION)
			{
				iCommerceRate += (GC.getInfo(eGlobalCommerceReligion).
						getGlobalReligionCommerce(eCommerce) *
						GC.getGame().countReligionLevels(eGlobalCommerceReligion)) *
						getNumActiveBuilding(eBuilding);
			}
		}
		{
			CorporationTypes const eGlobalCommerceCorp = kBuilding.getGlobalCorporationCommerce();
			if (eGlobalCommerceCorp != NO_CORPORATION)
			{
				iCommerceRate += (GC.getInfo(eGlobalCommerceCorp).
						getHeadquarterCommerce(eCommerce) *
						GC.getGame().countCorporationLevels(eGlobalCommerceCorp)) *
						getNumActiveBuilding(eBuilding);
			}
		}
	}
	return iCommerceRate;
}


void CvCity::updateBuildingCommerce()
{
	FOR_EACH_ENUM(Commerce)
	{	// advc: Moved into new function
		updateBuildingCommerce(eLoopCommerce);
	}
}

// advc.opt: Cut from updateBuildingCommerce above
void CvCity::updateBuildingCommerce(CommerceTypes eCommerce)
{
	int iNewBuildingCommerce = 0;
	FOR_EACH_ENUM(Building)
	{
		iNewBuildingCommerce += getBuildingCommerceByBuilding(
				eCommerce, eLoopBuilding);
	}
	if (getBuildingCommerce(eCommerce) != iNewBuildingCommerce)
	{
		m_aiBuildingCommerce.set(eCommerce, iNewBuildingCommerce);
		FAssert(getBuildingCommerce(eCommerce) >= 0);
		updateCommerce(eCommerce);
	}
}

// BUG - Building Additional Commerce - start
/*  Returns the total additional commerce times 100 that
	adding one of the given buildings will provide.
	Doesn't check if the building can be constructed in this city.
	Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE. */
int CvCity::getAdditionalCommerceTimes100ByBuilding(CommerceTypes eCommerce,
	BuildingTypes eBuilding) const
{
	int iExtraRate = getAdditionalBaseCommerceRateByBuilding(eCommerce, eBuilding);
	int iExtraModifier = getAdditionalCommerceRateModifierByBuilding(eCommerce, eBuilding);
	if (iExtraRate == 0 && iExtraModifier == 0)
		return 0;

	int iRateTimes100 = getBaseCommerceRateTimes100(eCommerce);
	int iModifier = getTotalCommerceRateModifier(eCommerce);
	int iExtraTimes100 = ((iModifier + iExtraModifier) *
			(100 * iExtraRate + iRateTimes100) / 100)
			- (iModifier * iRateTimes100 / 100);

	return iExtraTimes100;
}

/*  Returns the additional base commerce rate constructing the given building will provide.
	Doesn't check if the building can be constructed in this city.
	Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE. */
int CvCity::getAdditionalBaseCommerceRateByBuilding(CommerceTypes eCommerce,
	BuildingTypes eBuilding) const
{
	bool bNoEspionage = GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE);
	if (bNoEspionage && eCommerce == COMMERCE_ESPIONAGE)
		return 0;

	int iExtraRate = getAdditionalBaseCommerceRateByBuildingImpl(eCommerce, eBuilding);
	if (bNoEspionage && eCommerce == COMMERCE_CULTURE)
	{
		iExtraRate += getAdditionalBaseCommerceRateByBuildingImpl(
				COMMERCE_ESPIONAGE, eBuilding);
	}
	return iExtraRate;
}

/*  Returns the additional base commerce rate constructing the given building will provide.
	Doesn't check if the building can be constructed in this city. */
int CvCity::getAdditionalBaseCommerceRateByBuildingImpl(CommerceTypes eCommerce,
	BuildingTypes eBuilding) const
{
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);

	int iExtraRate = kBuilding.getObsoleteSafeCommerceChange(eCommerce);
	if (GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding))
		return iExtraRate;

	iExtraRate += kBuilding.getCommerceChange(eCommerce);
	iExtraRate += getBuildingCommerceChange(kBuilding.getBuildingClassType(), eCommerce);
	CvPlayer const& kOwner = GET_PLAYER(getOwner());
	if (kBuilding.getReligionType() != NO_RELIGION)
	{
		if (kBuilding.getReligionType() == kOwner.getStateReligion())
			iExtraRate += kOwner.getStateReligionBuildingCommerce(eCommerce);
	}
	{
		ReligionTypes const eGlobalCommerceReligion = kBuilding.getGlobalReligionCommerce();
		if (eGlobalCommerceReligion != NO_RELIGION)
		{
			iExtraRate += GC.getInfo(eGlobalCommerceReligion).
					getGlobalReligionCommerce(eCommerce) *
					GC.getGame().countReligionLevels(eGlobalCommerceReligion);
		}
	}
	{
		CorporationTypes eGlobalCommerceCorp = kBuilding.getGlobalCorporationCommerce();
		if (eGlobalCommerceCorp != NO_CORPORATION)
		{
			iExtraRate += GC.getInfo(eGlobalCommerceCorp).
					getHeadquarterCommerce(eCommerce) *
					GC.getGame().countCorporationLevels(eGlobalCommerceCorp);
		}
	}
	// ignore double-time check since this assumes you are building it this turn

	FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
		getFreeSpecialistCount(), Specialist, int)
	{
		iExtraRate += getAdditionalBaseCommerceRateBySpecialistImpl(eCommerce,
				perSpecialistVal.first, perSpecialistVal.second);
	}

	return iExtraRate;
}

/*  Doesn't check if the building can be constructed in this city.
	Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE. */
int CvCity::getAdditionalCommerceRateModifierByBuilding(CommerceTypes eCommerce,
	BuildingTypes eBuilding) const
{
	bool const bNoEspionage = GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE);
	if (bNoEspionage && eCommerce == COMMERCE_ESPIONAGE)
		return 0;

	int iExtraModifier = getAdditionalCommerceRateModifierByBuildingImpl(
			eCommerce, eBuilding);
	if (bNoEspionage && eCommerce == COMMERCE_CULTURE)
	{
		iExtraModifier += getAdditionalCommerceRateModifierByBuildingImpl(
				COMMERCE_ESPIONAGE, eBuilding);
	}
	return iExtraModifier;
}

/*  Returns the additional commerce rate modifier constructing the given building will provide.
	Doesn't check if the building can be constructed in this city. */
int CvCity::getAdditionalCommerceRateModifierByBuildingImpl(CommerceTypes eCommerce,
	BuildingTypes eBuilding) const
{
	if (!GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding))
	{
		CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
		return kBuilding.getCommerceModifier(eCommerce) +
				kBuilding.getGlobalCommerceModifier(eCommerce);
	}
	return 0;
}
// BUG - Building Additional Commerce - end
// < Civic Infos Plus Start >
//change from f1 advc to acctualyl work - keldath
int CvCity::getStateReligionCommerceRateModifier(CommerceTypes eCommerce) const {
   return GET_PLAYER(getOwner()).getStateReligionCommerceRateModifier(eCommerce);
}
/*
int CvCity::getStateReligionCommerceRateModifier(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiStateReligionCommerceRateModifier[eIndex];
}
*/

void CvCity::changeStateReligionCommerceRateModifier(CommerceTypes eCommerce, int iChange)
{
	FAssertMsg(eCommerce >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eCommerce < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");

	if (iChange != 0)
	{
		//no need anymore - f1 advc
		//m_aiStateReligionCommerceRateModifier[eIndex] = (m_aiStateReligionCommerceRateModifier[eIndex] + iChange);
		FAssert(getCommerceRate(eCommerce) >= 0);
		updateCommerce(eCommerce);
	}
}
//change from f1 advc to acctually work - keldath
int CvCity::getNonStateReligionCommerceRateModifier(CommerceTypes eCommerce) const {
   return GET_PLAYER(getOwner()).getNonStateReligionCommerceRateModifier(eCommerce);
}
/*
int CvCity::getNonStateReligionCommerceRateModifier(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");
	return m_aiNonStateReligionCommerceRateModifier[eIndex];
}
*/

void CvCity::changeNonStateReligionCommerceRateModifier(CommerceTypes eCommerce, int iChange)
{	
	if (iChange != 0)
	{
		//no need anymore - f1 advc
		//m_aiNonStateReligionCommerceRateModifier[eIndex] = (m_aiNonStateReligionCommerceRateModifier[eIndex] + iChange);
		FAssert(getCommerceRate(eCommerce) >= 0);

		updateCommerce(eCommerce);	
	}
}
// < Civic Infos Plus End   >
void CvCity::changeSpecialistCommerce(CommerceTypes eCommerce, int iChange)
{
	if (iChange != 0)
	{
		m_aiSpecialistCommerce.add(eCommerce, iChange);
		FAssert(getSpecialistCommerce(eCommerce) >= 0);
		updateCommerce(eCommerce);
	}
}

// BUG - Specialist Additional Commerce - start
/*  Returns the total additional commerce times 100 that changing the number
	of given specialists will provide/remove.
	Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE. */
int CvCity::getAdditionalCommerceTimes100BySpecialist(CommerceTypes eCommerce,
	SpecialistTypes eSpecialist, int iChange) const
{
	int iExtraRate = getAdditionalBaseCommerceRateBySpecialist(
			eCommerce, eSpecialist, iChange);
	if (iExtraRate == 0)
		return 0;

	int iRateTimes100 = getBaseCommerceRateTimes100(eCommerce);
	int iModifier = getTotalCommerceRateModifier(eCommerce);
	int iExtraTimes100 = (iModifier * (100 * iExtraRate + iRateTimes100) / 100) -
			(iModifier * iRateTimes100 / 100);

	return iExtraTimes100;
}

/*  Returns the additional base commerce rate that changing the number
	of given specialists will provide/remove.
	Takes the NO_ESPIONAGE game option into account for CULTURE and ESPIONAGE. */
int CvCity::getAdditionalBaseCommerceRateBySpecialist(CommerceTypes eCommerce,
	SpecialistTypes eSpecialist, int iChange) const
{
	bool const bNoEspionage = GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE);
	if (bNoEspionage && eCommerce == COMMERCE_ESPIONAGE)
		return 0;

	int iExtraRate = getAdditionalBaseCommerceRateBySpecialistImpl(
			eCommerce, eSpecialist, iChange);
	if (bNoEspionage && eCommerce == COMMERCE_CULTURE)
	{
		iExtraRate += getAdditionalBaseCommerceRateBySpecialistImpl(
				COMMERCE_ESPIONAGE, eSpecialist, iChange);
	}
	return iExtraRate;
}

/*	Returns the additional base commerce rate that changing the number
	of given specialists will provide/remove. */
int CvCity::getAdditionalBaseCommerceRateBySpecialistImpl(CommerceTypes eCommerce,
	SpecialistTypes eSpecialist, int iChange) const
{
	CvSpecialistInfo const& kSpecialist = GC.getInfo(eSpecialist);
	return iChange * (kSpecialist.getCommerceChange(eCommerce) +
			GET_PLAYER(getOwner()).getSpecialistExtraCommerce(eCommerce));
}
// BUG - Specialist Additional Commerce - end

int CvCity::getReligionCommerceByReligion(CommerceTypes eCommerce,
	ReligionTypes eReligion, /* advc: */ bool bForce) const
{
	int iCommerceRate = 0;
	// advc.172: Commented out
	/*if (GET_PLAYER(getOwner()).getStateReligion() == eReligion ||
		GET_PLAYER(getOwner()).getStateReligion() == NO_RELIGION)*/
	{
		if (isHasReligion(eReligion) /* advc: */ || bForce)
		{
			iCommerceRate += GC.getInfo(eReligion).getStateReligionCommerce(eCommerce);
			if (isHolyCity(eReligion))
				iCommerceRate += GC.getInfo(eReligion).getHolyCityCommerce(eCommerce);
		}
	}
	return iCommerceRate;
}

// XXX can this be simplified???
void CvCity::updateReligionCommerce(CommerceTypes eCommerce)
{
	int iNewReligionCommerce = 0;
	FOR_EACH_ENUM(Religion)
	{	// advc.172: No longer cumulative; use max.
		iNewReligionCommerce = std::max(iNewReligionCommerce,
				getReligionCommerceByReligion(eCommerce, eLoopReligion));
	}
	if (getReligionCommerce(eCommerce) != iNewReligionCommerce)
	{
		m_aiReligionCommerce.set(eCommerce, iNewReligionCommerce);
		FAssert(getReligionCommerce(eCommerce) >= 0);
		updateCommerce(eCommerce);
	}
}


void CvCity::updateReligionCommerce()
{
	FOR_EACH_ENUM(Commerce)
		updateReligionCommerce(eLoopCommerce);
}


void CvCity::setCorporationYield(YieldTypes eYield, int iNewValue)
{
	int const iOldValue = getCorporationYield(eYield);
	if (iOldValue != iNewValue)
	{
		m_aiCorporationYield.set(eYield, iNewValue);
		FAssert(getCorporationYield(eYield) >= 0);
		changeBaseYieldRate(eYield, iNewValue - iOldValue);
	}
}


int CvCity::getCorporationYieldByCorporation(YieldTypes eYield,
	CorporationTypes eCorporation) const
{
	int iYieldRate = 0;
	if (isActiveCorporation(eCorporation) && !isDisorder())
	{
		for (int i = 0; i < GC.getInfo(eCorporation).getNumPrereqBonuses(); i++)
		{
			BonusTypes const eBonus = GC.getInfo(eCorporation).getPrereqBonus(i);
			if (getNumBonuses(eBonus) > 0)
			{
				iYieldRate += (GC.getInfo(eCorporation).getYieldProduced(eYield) *
						getNumBonuses(eBonus) * GC.getInfo(GC.getMap().getWorldSize()).
						getCorporationMaintenancePercent()) / 100;
			}
		}
	}
	return intdiv::uceil(iYieldRate, 100);
}

int CvCity::getCorporationCommerceByCorporation(CommerceTypes eCommerce,
	CorporationTypes eCorporation) const
{
	int iCommerceRate = 0;
	if (isActiveCorporation(eCorporation) && !isDisorder())
	{
		for (int i = 0; i < GC.getInfo(eCorporation).getNumPrereqBonuses(); i++)
		{
			BonusTypes const eBonus = GC.getInfo(eCorporation).getPrereqBonus(i);
			if (getNumBonuses(eBonus) > 0)
			{
				iCommerceRate += (GC.getInfo(eCorporation).
						getCommerceProduced(eCommerce) *
						getNumBonuses(eBonus) * GC.getInfo(GC.getMap().getWorldSize()).
						getCorporationMaintenancePercent()) / 100;
			}
		}
	}
	return intdiv::uceil(iCommerceRate, 100);
}

void CvCity::updateCorporationCommerce(CommerceTypes eCommerce)
{
	int iNewRate = 0;
	FOR_EACH_ENUM(Corporation)
		iNewRate += getCorporationCommerceByCorporation(eCommerce, eLoopCorporation);

// davidlallen: building bonus yield, commerce start
	for (int eBldg = 0; eBldg < GC.getNumBuildingInfos(); ++eBldg)
	{
		if (getNumRealBuilding((BuildingTypes)eBldg) > 0)
		{
			int eBonus = GC.getBuildingInfo((BuildingTypes)eBldg).getBonusConsumed();
			if (NO_BONUS != eBonus)
			{
				iNewRate += GC.getBuildingInfo((BuildingTypes)eBldg).getCommerceProduced(eCommerce) * getNumBonuses((BonusTypes)eBonus) / 100;
			}
		}
	}
// davidlallen: building bonus yield, commerce end

	if (getCorporationCommerce(eCommerce) != iNewRate)
	{
		m_aiCorporationCommerce.set(eCommerce, iNewRate);
		FAssert(getCorporationCommerce(eCommerce) >= 0);
		updateCommerce(eCommerce);
	}
}

void CvCity::updateCorporationYield(YieldTypes eYield)
{
	int const iOldRate = getCorporationYield(eYield);
	int iNewRate = 0;

	FOR_EACH_ENUM(Corporation)
		iNewRate += getCorporationYieldByCorporation(eYield, eLoopCorporation);
// davidlallen: building bonus yield, commerce start
	for (int eBldg = 0; eBldg < GC.getNumBuildingInfos(); ++eBldg)
	{
		if (getNumRealBuilding((BuildingTypes)eBldg) > 0)
		{
			int eBonus = GC.getBuildingInfo((BuildingTypes)eBldg).getBonusConsumed();
			if (NO_BONUS != eBonus)
			{
				iNewRate += GC.getBuildingInfo((BuildingTypes)eBldg).getYieldProduced(eYield) * getNumBonuses((BonusTypes)eBonus) / 100;
			}
		}
	}
// davidlallen: building bonus yield, commerce end

	if (iOldRate != iNewRate)
	{
		m_aiCorporationYield.set(eYield, iNewRate);
		FAssert(getCorporationYield(eYield) >= 0);
		changeBaseYieldRate(eYield, iNewRate - iOldRate);
	}
}


void CvCity::updateCorporation(/* advc.064d: */ bool bVerifyProduction)
{
	updateCorporationBonus(/* advc.06d: */ bVerifyProduction);
	updateBuildingCommerce();

	FOR_EACH_ENUM(Yield)
		updateCorporationYield(eLoopYield);

	FOR_EACH_ENUM(Commerce)
		updateCorporationCommerce(eLoopCommerce);

	updateMaintenance();
}


void CvCity::updateCorporationBonus(/* advc.064d: */ bool bVerifyProduction)
{
	EagerEnumMap<BonusTypes,int> aiLastCorpProducedBonuses;
	EagerEnumMap<BonusTypes,bool> abHadBonuses;
	FOR_EACH_ENUM(Bonus)
	{
		abHadBonuses.set(eLoopBonus, hasBonus(eLoopBonus));
		aiLastCorpProducedBonuses.set(eLoopBonus, getNumBonuses(eLoopBonus));
	}
	m_aiNumCorpProducedBonuses.reset();
	EagerEnumMap<BonusTypes,int> aiExtraCorpProducedBonuses;
	CvTeam const& kTeam = GET_TEAM(getTeam());
	for (int iIter = 0; iIter < GC.getNumCorporationInfos(); iIter++)
	{
		FOR_EACH_ENUM(Corporation)
		{
			BonusTypes eBonusProduced = GC.getInfo(eLoopCorporation).getBonusProduced();
			if (eBonusProduced == NO_BONUS ||
				kTeam.isBonusObsolete(eBonusProduced) ||
				!kTeam.isHasTech(GC.getInfo(eBonusProduced).getTechCityTrade()))
			{
				continue;
			}
			if (isHasCorporation(eLoopCorporation) &&
				GET_PLAYER(getOwner()).isActiveCorporation(eLoopCorporation))
			{
				for (int i = 0; i < GC.getInfo(eLoopCorporation).getNumPrereqBonuses(); i++)
				{
					BonusTypes eBonusConsumed = GC.getInfo(eLoopCorporation).
							getPrereqBonus(i);
					aiExtraCorpProducedBonuses.add(eBonusProduced,
							aiLastCorpProducedBonuses.get(eBonusConsumed));
				}
			}
		}
		bool bChanged = false;
		FOR_EACH_ENUM2(Bonus, e)
		{
			if (aiExtraCorpProducedBonuses.get(e) != 0)
			{
				m_aiNumCorpProducedBonuses.add(e, aiExtraCorpProducedBonuses.get(e));
				bChanged = true;
			}
			aiLastCorpProducedBonuses.set(e, aiExtraCorpProducedBonuses.get(e));
		}
		aiExtraCorpProducedBonuses.reset();
		if (!bChanged)
			break;
		FAssertMsg(iIter < GC.getNumCorporationInfos() - 1, "cyclical resource dependency");
	}

	FOR_EACH_ENUM(Bonus)
	{
		if (abHadBonuses.get(eLoopBonus) != hasBonus(eLoopBonus))
		{
			if (hasBonus(eLoopBonus))
				processBonus(eLoopBonus, 1);
			else
			{
				processBonus(eLoopBonus, -1);
				// <advc.064d>
				if (bVerifyProduction)
					verifyProduction(); // </advc.064d>
			}
		}
	}
}


void CvCity::changeCommerceRateModifier(CommerceTypes eCommerce, int iChange)
{
	if (iChange != 0)
	{
		m_aiCommerceRateModifier.add(eCommerce, iChange);
		updateCommerce(eCommerce);
		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getCommerceHappinessByType(CommerceTypes eCommerce) const
{
	return ((getCommerceHappinessPer(eCommerce) *
			GET_PLAYER(getOwner()).getCommercePercent(eCommerce)) / 100);
}


int CvCity::getCommerceHappiness() const
{
	int iHappiness = 0;
	FOR_EACH_ENUM(Commerce)
		iHappiness += getCommerceHappinessByType(eLoopCommerce);
	return iHappiness;
}


void CvCity::changeCommerceHappinessPer(CommerceTypes eCommerce, int iChange)
{
	if (iChange != 0)
	{
		m_aiCommerceHappinessPer.add(eCommerce, iChange);
		FAssert(getCommerceHappinessPer(eCommerce) >= 0);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeDomainFreeExperience(DomainTypes eDomain, int iChange)
{
	m_aiDomainFreeExperience.add(eDomain, iChange);
	FAssert(getDomainFreeExperience(eDomain) >= 0);
}


void CvCity::changeDomainProductionModifier(DomainTypes eDomain, int iChange)
{
	m_aiDomainProductionModifier.add(eDomain, iChange);
}


int CvCity::countTotalCultureTimes100() const
{
	int iR = 0;
	for (PlayerIter<EVER_ALIVE> it; it.hasNext(); ++it) // advc.099: was ALIVE
	{
		iR += getCultureTimes100(it->getID());
	}
	return iR;
}


PlayerTypes CvCity::findHighestCulture() const
{
	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestValue = 0;
	for (PlayerIter<EVER_ALIVE> it; it.hasNext(); ++it) // advc.099: was ALIVE
	{
		int iValue = getCultureTimes100(it->getID());
		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			eBestPlayer = it->getID();
		}

	}
	return eBestPlayer;
}

/*	advc.101:
	Doesn't check if city will flip. Doesn't take into account that the revolt test
	is skipped when decreasing the occupation timer.
	Difference from getRevoltTestProbability: that function only returns the
	probability of the first revolt test, not the second one based on
	culture garrison.
	If bIgnoreWar is set, a probability is returned even if the city can't revolt
	(i.e. when it's already in occupation and at war with the cultural owner).
	If bIgnoreGarrison is set, culture garrison strength is treated as 0.
	If bIgnoreOccupation is set, the probability is computed assuming that the city
	isn't in occupation. */
scaled CvCity::revoltProbability(bool bIgnoreWar,
	bool bIgnoreGarrison, bool bIgnoreOccupation) const // advc.023
{
	PlayerTypes eCulturalOwner = calculateCulturalOwner(); // advc.099c
	static bool const bBARBS_REVOLT = GC.getDefineBOOL("BARBS_REVOLT");
	if (eCulturalOwner == NO_PLAYER || TEAMID(eCulturalOwner) == getTeam() ||
		// advc.099c: Barbarian revolts
		(eCulturalOwner == BARBARIAN_PLAYER && !bBARBS_REVOLT))
	{
		return 0;
	}
	// <advc.023>
	scaled rOccupationFactor = 1;
	if (isOccupation() && !bIgnoreOccupation)
	{
		if(!bIgnoreWar && !isBarbarian() && !GET_TEAM(getTeam()).isMinorCiv() &&
			GET_PLAYER(eCulturalOwner).isAlive() &&
			GET_TEAM(getTeam()).isAtWar(TEAMID(eCulturalOwner)))
		{
			return 0;
		}
		rOccupationFactor = fixp(0.5);
	} // </advc.023>
	int iCityStrength = cultureStrength(eCulturalOwner);
	int iGarrison =
			(bIgnoreGarrison ? 0 : // advc.023
			cultureGarrison(eCulturalOwner));
	if(iCityStrength <= iGarrison)
		return 0;
	/*  About the two revolt tests: I guess the first one checks if the city tries
		to revolt, and the second if the garrison can stop the revolt.
		Restored the BtS formula for the second test. */
	scaled r = scaled::max(0, (1 - scaled(iGarrison, iCityStrength))) *
			getRevoltTestProbability() * rOccupationFactor;
	// Don't use probabilities that are too small to be displayed
	if (r.getPermille() < 1)
		return 0;
	if (r.getPermille() > 999)
		return 1;
	return r;
}

// advc.023:
scaled CvCity::probabilityOccupationDecrement() const
{
	if(!isOccupation())
		return 0;
	// While at war, require an occupying force.
	if(getMilitaryHappinessUnits() <= 0 && GET_PLAYER(getOwner()).isMajorCiv())
	{
		PlayerTypes eCulturalOwner = calculateCulturalOwner();
		if(eCulturalOwner != NO_PLAYER && GET_TEAM(eCulturalOwner).isAtWar(getTeam()))
			return 0;
	}
	scaled r;
	scaled rRevoltProb = revoltProbability(true, false, true);
	if (rRevoltProb < 1)
		r = (1 - rRevoltProb).pow(GC.getDefineINT(CvGlobals::OCCUPATION_COUNTDOWN_EXPONENT));
	// Don't use probabilities that are too small to be displayed
	if (r.getPermille() < 1)
		return 0;
	if (r.getPermille() > 999)
		return 1;
	return r;
}

// K-Mod: The following function defines whether or not the city is allowed to flip to the given player
bool CvCity::canCultureFlip(PlayerTypes eToPlayer, /* advc.101: */ bool bCheckPriorRevolts) const
{
	/*if (isBarbarian())
		return true;*/ // advc.101: Commented out
	// <advc.099c>
	if (eToPlayer == NO_PLAYER)
		eToPlayer = getPlot().calculateCulturalOwner();
	else if (!getPlot().isWithinCultureRange(eToPlayer))
		return false;
	if(eToPlayer == NO_PLAYER || eToPlayer == getOwner() ||
		!GET_PLAYER(eToPlayer).isAlive() || eToPlayer == BARBARIAN_PLAYER ||
		GET_TEAM(eToPlayer).isVassal(getTeam()))
	{
		return false;
	} // </advc.099c>
	return (!GC.getGame().isOption(GAMEOPTION_NO_CITY_FLIPPING) &&
			// advc.101: City flipping option negated (now has inverse meaning)
			(!GC.getGame().isOption(GAMEOPTION_NO_FLIPPING_AFTER_CONQUEST) ||
			getPreviousOwner() == NO_PLAYER ||
			TEAMID(getPreviousOwner()) != TEAMID(eToPlayer)) && // advc
			(!bCheckPriorRevolts || // advc.101
			getNumRevolts(eToPlayer) >= GC.getDefineINT(CvGlobals::NUM_WARNING_REVOLTS)
			- (isBarbarian() ? 1 : 0))); // advc.101
}


int CvCity::calculateCulturePercent(PlayerTypes ePlayer) const
{
	int iTotalCulture = countTotalCultureTimes100();
	if (iTotalCulture > 0)
		return (getCultureTimes100(ePlayer) * 100) / iTotalCulture;
	return 0;
}


int CvCity::calculateTeamCulturePercent(TeamTypes eTeam) const
{
	int iR = 0;
	for (PlayerIter<EVER_ALIVE,MEMBER_OF> it(eTeam); it.hasNext(); ++it) // advc.099: was ALIVE
	{
		iR += calculateCulturePercent(it->getID());
	}
	return iR;
}


void CvCity::setCulture(PlayerTypes ePlayer, int iNewValue, bool bPlots,
	bool bUpdatePlotGroups)
{
	setCultureTimes100(ePlayer, 100 * iNewValue, bPlots, bUpdatePlotGroups);
}

// K-Mod, 26/sep/10: fixed so that plots actually get the culture difference
void CvCity::setCultureTimes100(PlayerTypes ePlayer, int iNewValue, bool bPlots,
	bool bUpdatePlotGroups)
{
	int const iOldValue = getCultureTimes100(ePlayer);
	if (iNewValue == iOldValue)
		return;
	m_aiCulture.set(ePlayer, iNewValue);
	FAssert(getCultureTimes100(ePlayer) >= 0);
	updateCultureLevel(bUpdatePlotGroups);
	if (bPlots)
	{
		//doPlotCulture(true, ePlayer, 0);
		//doPlotCulture(true, ePlayer, (iNewValue - iOldValue) / 100);
		doPlotCultureTimes100(true, ePlayer, iNewValue - iOldValue, false);
		/*	note: this function no longer applies free city culture.
			also, note that if a city's culture is decreased to zero,
			there will probably still be some residual plot culture around the city.
			this is because the culture level on the way up
			will be higher than it is on the way down. */
	}
}


void CvCity::changeCulture(PlayerTypes ePlayer, int iChange, bool bPlots,
	bool bUpdatePlotGroups)
{
	setCultureTimes100(ePlayer, getCultureTimes100(ePlayer) + 100 * iChange,
			bPlots, bUpdatePlotGroups);
}


void CvCity::changeCultureTimes100(PlayerTypes ePlayer, int iChange, bool bPlots,
	bool bUpdatePlotGroups)
{
	setCultureTimes100(ePlayer, getCultureTimes100(ePlayer) + iChange,
			bPlots, bUpdatePlotGroups);
}

// advc.099c: To the current cultural owner
int CvCity::getNumRevolts() const
{
	PlayerTypes eCultOwner = calculateCulturalOwner();
	if(eCultOwner == NO_PLAYER)
		return 0;
	return getNumRevolts(eCultOwner);
} // </advc.099c>


void CvCity::changeNumRevolts(PlayerTypes ePlayer, int iChange)
{
	m_aiNumRevolts.add(ePlayer, iChange);
	FAssert(getNumRevolts(ePlayer) >= 0);
}


scaled CvCity::getRevoltTestProbability() const // advc.101: Return type was int
{
	// <advc.101>
	CvGame const& kGame = GC.getGame();
	// Was based on getVictoryDelayPercent() in K-Mod
	scaled rSpeedFactor = per100(GC.getInfo(kGame.getGameSpeedType()).getGoldenAgePercent());
	// Need to cut cities recently acquired by Barbarians some slack
	if (isBarbarian() && kGame.getGameTurn() - getGameTurnAcquired() < 8 * rSpeedFactor)
		return 0; // </advc.101>
	static scaled const rREVOLT_TEST_PROB = per100(GC.getDefineINT("REVOLT_TEST_PROB")); // advc.opt
	scaled r = rREVOLT_TEST_PROB * per100(100 - getRevoltProtection());
	r /= rSpeedFactor;
	r.decreaseTo(1); // advc.101: Upper bound used to be handled by the caller
	return r;
}

// advc.101: Cut from getRevoltTestProbability
int CvCity::getRevoltProtection() const
{
	int iBestModifier = 0;
	for (CLLNode<IDInfo> const* pUnitNode = getPlot().headUnitNode(); pUnitNode != NULL;
		pUnitNode = getPlot().nextUnitNode(pUnitNode))
	{
		CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
		if (pLoopUnit->getRevoltProtection() > iBestModifier)
			iBestModifier = pLoopUnit->getRevoltProtection();
	}
	return range(iBestModifier, 0, 100);
}

// advc: Cut from CvPlot::setOwner
void CvCity::addRevoltFreeUnits()
{
	UnitTypes eBestUnit = AI().AI_bestUnitAI(UNITAI_CITY_DEFENSE);
	if (eBestUnit == NO_UNIT)
		eBestUnit = AI().AI_bestUnitAI(UNITAI_ATTACK);

	if (eBestUnit != NO_UNIT)
	{
		int iFreeUnits = (GC.getDefineINT("BASE_REVOLT_FREE_UNITS") +
				(getHighestPopulation() * GC.getDefineINT("REVOLT_FREE_UNITS_PERCENT")) / 100);
		for (int i = 0; i < iFreeUnits; i++)
		{
			GET_PLAYER(getOwner()).initUnit(eBestUnit, getX(), getY(), UNITAI_CITY_DEFENSE);
		}
	}
}


void CvCity::setEverOwned(PlayerTypes ePlayer, bool bNewValue)
{
	m_abEverOwned.set(ePlayer, bNewValue);
}


void CvCity::setTradeRoute(PlayerTypes ePlayer, bool bNewValue)
{
	m_abTradeRoute.set(ePlayer, bNewValue);
}


bool CvCity::isRevealed(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
		return true;
	return isRevealed(eTeam); // advc.inl: Call inline version
}


void CvCity::setRevealed(TeamTypes eTeam, bool bNewValue)
{
	if(isRevealed(eTeam) == bNewValue)
		return;
	// <advc.130n>
	for (MemberAIIter itMember(eTeam); itMember.hasNext(); ++itMember)
		itMember->AI_updateIdeologyAttitude(-1, *this); // </advc.130n>
	m_abRevealed.set(eTeam, bNewValue);
	// <advc.130n>
	for (MemberAIIter itMember(eTeam); itMember.hasNext(); ++itMember)
		itMember->AI_updateIdeologyAttitude(1, *this); // </advc.130n>
	// K-Mod
	if (bNewValue)
	{
		GET_TEAM(eTeam).makeHasSeen(getTeam());
		// <advc.130w>
		if (GET_TEAM(eTeam).isMajorCiv() &&
			GET_TEAM(getTeam()).isMajorCiv() &&
			getPlot().findHighestCultureTeam() != getTeam())
		{
			GET_TEAM(eTeam).AI_updateAttitude(getTeam());
		} // </advc.130w>
	} // K-Mod end

	updateVisibility();

	if (eTeam == GC.getGame().getActiveTeam())
	{
		for (CityPlotIter it(*this); it.hasNext(); ++it)
		{
			it->updateSymbols();
		}
	}
}


void CvCity::setEspionageVisibility(TeamTypes eTeam, bool bNewValue, bool bUpdatePlotGroups)
{
	if (getEspionageVisibility(eTeam) != bNewValue)
	{
		getPlot().updateSight(false, bUpdatePlotGroups);
		m_abEspionageVisibility.set(eTeam, bNewValue);
		getPlot().updateSight(true, bUpdatePlotGroups);
	}
}

void CvCity::updateEspionageVisibility(bool bUpdatePlotGroups)
{
	std::vector<EspionageMissionTypes> aMission;
	FOR_EACH_ENUM(EspionageMission)
	{
		if (GC.getInfo(eLoopEspionageMission).isPassive() &&
			GC.getInfo(eLoopEspionageMission).getVisibilityLevel() > 0)
		{
			aMission.push_back(eLoopEspionageMission);
		}
	}
	for (TeamIter<CIV_ALIVE,NOT_SAME_TEAM_AS> itTeam(getTeam()); itTeam.hasNext(); ++itTeam)
	{
		TeamTypes eOtherTeam = itTeam->getID();
		if (!isRevealed(eOtherTeam))
			continue;

		bool bVisibility = false;
		for (MemberIter itMember(eOtherTeam); itMember.hasNext() &&
			!bVisibility; ++itMember)
		{
			CvPlayer& kMember = *itMember;
			for (size_t i = 0; i < aMission.size(); i++)
			{
				if (kMember.canDoEspionageMission(aMission[i], getOwner(), plot()))
				{
					bVisibility = true;
					break;
				}
			}
		}
		setEspionageVisibility(eOtherTeam, bVisibility, bUpdatePlotGroups);
	}
}

const wchar* CvCity::getNameKey() const
{
	return m_szName;
}


const CvWString CvCity::getName(uint uiForm) const
{
	return gDLL->getObjectText(m_szName, uiForm, true);
}


void CvCity::setName(const wchar* szNewValue, bool bFound, /* advc.106k: */ bool bInitial)
{
	CvWString szName(szNewValue);
	gDLL->stripSpecialCharacters(szName);
	// K-Mod. stripSpecialCharacters apparently doesn't count '%' as a special character
	// however, strings with '%' in them will cause the game to crash. So I'm going to strip them out.
	for (CvWString::iterator it = szName.begin(); it != szName.end(); )
	{
		if (*it == '%')
			it = szName.erase(it);
		else ++it;
	} // K-Mod end

	if (!szName.empty())
	{
		if (GET_PLAYER(getOwner()).isCityNameValid(szName, false))
		{	// <advc.106k>
			if(bInitial)
				m_szPreviousName.clear();
			else if(m_szPreviousName.empty())
				m_szPreviousName = m_szName; // </advc.106k>
			m_szName = szName;

			setInfoDirty(true);
			if (isCitySelected())
				gDLL->UI().setDirty(CityScreen_DIRTY_BIT, true);
		}
		if (bFound)
			doFoundMessage();
	}
}


void CvCity::doFoundMessage()
{
	CvWString szBuffer;
	/*	advc.106b: Seems that messages aren't supposed to be shown during advanced start
		and changing the message type causes the EXE to display the message. */
	if (GET_PLAYER(getOwner()).getAdvancedStartPoints() < 0)
	{
		szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_HAS_BEEN_FOUNDED", getNameKey());
		gDLL->UI().addMessage(getOwner(), false, -1, szBuffer,
				ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(),
				MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY, // advc.106b
				NULL, NO_COLOR, getX(), getY());
	}
	szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_IS_FOUNDED", getNameKey());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_CITY_FOUNDED, getOwner(),
			szBuffer, getX(), getY(),
			//(ColorTypes)GC.getInfoTypeForString("COLOR_ALT_HIGHLIGHT_TEXT")
			// advc.106: Use ALT_HIGHLIGHT for research-related stuff now
			GET_PLAYER(getOwner()).getPlayerTextColor());
}


std::string CvCity::getScriptData() const
{
	return m_szScriptData;
}


void CvCity::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}


void CvCity::changeNoBonusCount(BonusTypes eBonus, int iChange)
{
	if (iChange == 0)
		return; // advc

	if (getNumBonuses(eBonus) > 0)
	{
		processBonus(eBonus, -1);
		verifyProduction(); // advc.064d
	}

	m_aiNoBonus.add(eBonus, iChange);
	FAssert(getNoBonusCount(eBonus) >= 0);

	if (getNumBonuses(eBonus) > 0)
		processBonus(eBonus, 1);

	updateCorporation();

	AI_setAssignWorkDirty(true);
	setInfoDirty(true);
}


void CvCity::changeFreeBonus(BonusTypes eBonus, int iChange)
{
	if (iChange != 0)
	{
		getPlot().updatePlotGroupBonus(false, /* advc.064d: */ false);
		m_aiFreeBonus.add(eBonus, iChange);
		FAssert(getFreeBonus(eBonus) >= 0);
		getPlot().updatePlotGroupBonus(true);
	}
}

int CvCity::getNumBonuses(BonusTypes eBonus) const
{
	if (isNoBonus(eBonus))
		return 0;
	return m_aiNumBonuses.get(eBonus) + //m_aiNumCorpProducedBonuses.get(eBonus);
			// advc.003j: Don't want to leave this function entirely unused
			getNumCorpProducedBonuses(eBonus);
}


//< Building Resource Converter Start >
//f1rpo 096 - added a var here to pass an param to avoid a loop - keldath
void CvCity::changeNumBonuses(BonusTypes eBonus, int iChange,
	bool bVerifyProduction,bool bUpdateBuildings
//< Building Resource Converter end >
	) // advc.064d
{
	if (iChange == 0)
		return;

	bool const bOldHasBonus = hasBonus(eBonus);
	m_aiNumBonuses.add(eBonus, iChange);
	if (bOldHasBonus != hasBonus(eBonus))
	{
		if (hasBonus(eBonus))
			processBonus(eBonus, 1);
		else processBonus(eBonus, -1);
	}
//< Building Resource Converter Start >
	if (bUpdateBuildings && isBuildingBonus(eBonus))
		processBuildingBonuses(); // </f1rpo>
//< Building Resource Converter end >
	if (isCorporationBonus(eBonus))
		updateCorporation(/* advc.06d: */ bVerifyProduction);
}

// advc.149:
int CvCity::countUniqueBonuses() const
{
	int r = 0;
	FOR_EACH_ENUM(Bonus)
	{
		if (hasBonus(eLoopBonus))
			r++;
	}
	return r;
}


bool CvCity::isCorporationBonus(BonusTypes eBonus) const
{
	FOR_EACH_ENUM(Corporation)
	{
		if (!GET_PLAYER(getOwner()).isActiveCorporation(eLoopCorporation))
			continue;
		for (int i = 0; i < GC.getInfo(eLoopCorporation).getNumPrereqBonuses(); i++)
		{
			if (GC.getInfo(eLoopCorporation).getPrereqBonus(i) == eBonus &&
				isHasCorporation(eLoopCorporation))
			{
				return true;
			}
		}
	}
	// davidlallen: building bonus yield, commerce start
	for (int iBldg = 0; iBldg < GC.getNumBuildingInfos(); ++iBldg)
	{
		if (getNumRealBuilding((BuildingTypes)iBldg) > 0)
		{
			if (eBonus == GC.getBuildingInfo((BuildingTypes)iBldg).getBonusConsumed())
				{
					return true;
				}
		}
	}
	// davidlallen: building bonus yield, commerce end
	return false;
}

bool CvCity::isActiveCorporation(CorporationTypes eCorporation) const
{
	FAssert(eCorporation >= 0 && eCorporation < GC.getNumCorporationInfos());

	if (!isHasCorporation(eCorporation))
		return false;

	if (!GET_PLAYER(getOwner()).isActiveCorporation(eCorporation))
		return false;

	for (int i = 0; i < GC.getInfo(eCorporation).getNumPrereqBonuses(); i++)
	{
		if (getNumBonuses(GC.getInfo(eCorporation).getPrereqBonus(i)) > 0)
			return true;
	}

	return false;
}

// f1rpo:
//start of f1rpo refactor code < Building Resource Converter start >
//this section replaces the whole part below - f1rpo has optimized the code.
bool CvCity::isBuildingBonus(BonusTypes eBonus) const
{
	for (int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		 BuildingTypes eBuilding = (BuildingTypes)i;
		 if (getNumBuilding(eBuilding) > 0 && GC.getBuildingInfo(eBuilding).
				getRequiredInputBonusValue(eBonus) > 0)
			return true;
	}
	return false;
}

// f1rpo:
bool CvCity::isBuildingBonus(BuildingTypes eBuilding) const
{
	for (int i = 0; i < GC.getNumBonusInfos(); i++)
	{
		 BonusTypes eBonus = (BonusTypes)i;
		 if (getNumBonuses(eBonus) > 0 && GC.getBuildingInfo(eBuilding).
				getRequiredInputBonusValue(eBonus) > 0)
			return true;
	}
	return false;
}
// < Building Resource Converter Start > // f1rpo: refactored; comments added.
int CvCity::getBuildingOutputBonus(BonusTypes eBonus) const
{
//   FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex, "CvCity::getBuildingOutputBonus");
   return m_paiBuildingOutputBonuses.get(eBonus);
}
void CvCity::processBuildingBonuses()
{
   resetBuildingOutputBonuses();

   bool* abBuildingProcessed = new bool[GC.getNumBuildingInfos()]();
   /*  Process each building up to NumBuildings times (not NumBuildingInfos!)
	   because processing a building may add resources to the city through
	   changeNumBonuses that another building might be able to use. */
   // However, this is too slow and won't really work (see fixme comments below), so:
   //for (int i = 0; i < getNumBuildings(); i++)
   {
	   for (int j = 0; j < GC.getNumBuildingInfos(); j++)
	   {
		   BuildingTypes eBuilding = (BuildingTypes)j;
		   if (abBuildingProcessed[eBuilding])
			   continue;
		   if (getNumBuilding(eBuilding) <= 0)
		   {
			   abBuildingProcessed[eBuilding] = true;
			   continue;
		   }
		   CvBuildingInfo const& kBuilding = GC.getBuildingInfo(eBuilding);

		   int iReqMet = MAX_INT;
		   for (int k = 0; k < GC.getNumBonusInfos(); k++)
		   {
			   BonusTypes eBonus = (BonusTypes)k;
			   int iReq = kBuilding.getRequiredInputBonusValue(eBonus);
			   if (iReq <= 0)
				   continue;
			  /*  Fixme: getNumBonuses does not include the resources
				   generated by other buildings */
			   int iHave = getNumBonuses(eBonus);
			   if (iHave < iReq)
			   {
				   iReqMet = MAX_INT;
				   break;
			   }
			   iReqMet = std::min(iReqMet, iHave / iReq);
		   }

		   if (iReqMet > 0 && iReqMet < MAX_INT)
		   {
			   for (int k = 0; k < GC.getNumBonusInfos(); k++)
			   {
				   BonusTypes eBonus = (BonusTypes)k;
				   int iOutputPerReqMet = kBuilding.getBuildingOutputBonusValues(eBonus);
				   if (iOutputPerReqMet <= 0)
					   continue;
				   //int m_paiBuildingOutputBonuses.get(eBonus) = iReqMet * iOutputPerReqMet;	
				  //f1rpo enummap change 
				  m_paiBuildingOutputBonuses.set(eBonus, iReqMet * iOutputPerReqMet);
				  changeNumBonuses(eBonus, getBuildingOutputBonus(eBonus), false);

			   }
			   /*  Fixme: What if a building processed in a later iteration
				   outputs resources that would increase iReqMet for
				   eBuilding? For correctness, the definition of
				   abBuildingProcessed may have to be moved into the inner
				   building loop; but that's going to be much slower. */
			   abBuildingProcessed[eBuilding] = true;
		   }
		   // Otherwise, another processed building may yet provide the required bonuses.
	   }
   }
   SAFE_DELETE_ARRAY(abBuildingProcessed);
}


void CvCity::resetBuildingOutputBonuses()
{
   // This branch should not be necessary; memory is allocated in CvCity::reset.
  //f1rpo - removed check - 
	//keldath QA2 
	/*
		That code block should be removed then. It's for lazy allocation (create BuildingOutputBonuses array only once it is needed), and that's what EnumMap does in any case internally.
	*/
	/* if (m_paiBuildingOutputBonuses == NULL)
   {
	   FAssert(m_paiBuildingOutputBonuses != NULL);
	   m_paiBuildingOutputBonuses = new int[GC.getNumBonusInfos()]();
   }
   else*/
 //  {
	   for (int i = 0; i < GC.getNumBonusInfos(); i++)
	   {
		   if (m_paiBuildingOutputBonuses.get((BonusTypes)i)/*m_paiBuildingOutputBonuses[i]*/ != 0)
		   {
			   changeNumBonuses((BonusTypes)i, -m_paiBuildingOutputBonuses.get((BonusTypes)i)/*m_paiBuildingOutputBonuses[i]*/, false);
			  // m_paiBuildingOutputBonuses[i] = 0;
			  m_paiBuildingOutputBonuses.set((BonusTypes)i, 0);

		   }
	   }
//   }
//end of f1rpo refactor code < Building Resource Converter end >
//keldath change - changeNumBonuses2 from changeNumBonuses
//i duplicated a the func in plotgroup - to avoid infinite loop.
// < Building Resource Converter Start >
/*
void CvCity::processBuildingBonuses()
{
	int iI, iJ, iBuildingProducedBonusAmount, iTempRequiredInputBonusValue, iCityBonusAmount;

	int iK = 0;

	bool bBuildingProducedBonus = false;

	bool* pabBuildingProcessed = new bool[GC.getNumBuildingInfos()];

	for(iI=0;iI<GC.getNumBuildingInfos();iI++)
	{
		pabBuildingProcessed[iI] = false;
	}

	iBuildingProducedBonusAmount = 99999;

	resetBuildingOutputBonuses();

	for(iK = GC.getNumBuildingInfos();iK >= 0; iK--)
	{
		for(iI = 0;iI<GC.getNumBuildingInfos(); iI++)
		{
			if(pabBuildingProcessed[iI])
			{
				continue;
			}

			if(getNumBuilding((BuildingTypes)iI) == 0)
			{
				pabBuildingProcessed[iI] = true;
				continue;
			}

			for(iJ = 0;iJ<GC.getNumBonusInfos();iJ++)
			{
				if(!GC.getBuildingInfo((BuildingTypes)iI).isRequiredInputBonus((BonusTypes)iJ))
				{
					continue;
				}

				iCityBonusAmount = getNumBonuses((BonusTypes)iJ);

				if(iCityBonusAmount == 0)
				{
					bBuildingProducedBonus = false;
					break;
				}

				iTempRequiredInputBonusValue = GC.getBuildingInfo((BuildingTypes)iI).getRequiredInputBonusValue((BonusTypes)iJ);
				
				//fix by f1rpo 096 - keldath
				if (iCityBonusAmount < iTempRequiredInputBonusValue)
				//org code
				//if((iCityBonusAmount/iTempRequiredInputBonusValue) == 0)
				{
					bBuildingProducedBonus = false;
					break;
				}

				if((iCityBonusAmount/iTempRequiredInputBonusValue) < iBuildingProducedBonusAmount)
				{
					iBuildingProducedBonusAmount = iCityBonusAmount/iTempRequiredInputBonusValue;
					bBuildingProducedBonus = true;
				}
			}

			if(iBuildingProducedBonusAmount > 0 && bBuildingProducedBonus)
			{
				for(iJ = 0;iJ<GC.getNumBonusInfos();iJ++)
				{
					if(!GC.getBuildingInfo((BuildingTypes)iI).isBuildingOutputBonus((BonusTypes)iJ))
					{
						continue;
					}
					m_paiBuildingOutputBonuses[iJ] = iBuildingProducedBonusAmount*GC.getBuildingInfo((BuildingTypes)iI).getBuildingOutputBonusValues((BonusTypes)iJ);
					//f1rpo 096 fix - avoid infinite loop - the check for change bonus, moved to be calledfrom
					changeNumBonuses((BonusTypes)iJ, m_paiBuildingOutputBonuses[iJ],false);
					//changeNumBonuses((BonusTypes)iJ, m_paiBuildingOutputBonuses[iJ]);
					
				}
				pabBuildingProcessed[iI] = true;
			}

			bBuildingProducedBonus = false;
			iBuildingProducedBonusAmount = 99999;
		}
	}
	SAFE_DELETE_ARRAY(pabBuildingProcessed);
}

int CvCity::getBuildingOutputBonus(BonusTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex expected to be < GC.getNumBonusInfos()");
	return m_paiBuildingOutputBonuses[eIndex];
}

void CvCity::resetBuildingOutputBonuses()
{
	int iI;
	if(m_paiBuildingOutputBonuses)
	{
		for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			if(m_paiBuildingOutputBonuses[iI] != 0){
				//f1rpo 096 fix - avoid infinite loop - the check for change bonus, moved to be calledfrom
				changeNumBonuses((BonusTypes)iI, -m_paiBuildingOutputBonuses[iI],false);
				//changeNumBonuses((BonusTypes)iI, -m_paiBuildingOutputBonuses[iI]);
				m_paiBuildingOutputBonuses[iI] = 0;
			}
		}
	}
	else
	{
		m_paiBuildingOutputBonuses = new int[GC.getNumBonusInfos()];
		for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_paiBuildingOutputBonuses[iI] = 0;
		}
	} */
}
// < Building Resource Converter End   >
void CvCity::setBuildingProduction(BuildingTypes eBuilding, int iNewValue)
{
	if (getBuildingProduction(eBuilding) == iNewValue)
		return;

	m_aiBuildingProduction.set(eBuilding, iNewValue);
	FAssert(getBuildingProduction(eBuilding) >= 0);

	if (isActiveTeam())
		setInfoDirty(true);

	if (isActiveOwned() && isCitySelected())
		gDLL->UI().setDirty(CityScreen_DIRTY_BIT, true);
}


void CvCity::changeBuildingProduction(BuildingTypes eBuilding, int iChange)
{
	setBuildingProduction(eBuilding, getBuildingProduction(eBuilding) + iChange);
}


void CvCity::setBuildingProductionTime(BuildingTypes eBuilding, int iNewValue)
{
	m_aiBuildingProductionTime.set(eBuilding, iNewValue);
	FAssert(getBuildingProductionTime(eBuilding) >= 0);
}


void CvCity::changeBuildingProductionTime(BuildingTypes eBuilding, int iChange)
{
	setBuildingProductionTime(eBuilding, getBuildingProductionTime(eBuilding) + iChange);
}


void CvCity::setProjectProduction(ProjectTypes eProject, int iNewValue)
{
	if (getProjectProduction(eProject) == iNewValue)
		return;

	m_aiProjectProduction.set(eProject, iNewValue);
	FAssert(getProjectProduction(eProject) >= 0);

	if (isActiveTeam())
		setInfoDirty(true);

	if (isActiveOwned() && isCitySelected())
		gDLL->UI().setDirty(CityScreen_DIRTY_BIT, true);
}


void CvCity::changeProjectProduction(ProjectTypes eProject, int iChange)
{
	setProjectProduction(eProject, getProjectProduction(eProject) + iChange);
}


void CvCity::setUnitProduction(UnitTypes eUnit, int iNewValue)
{
	if (getUnitProduction(eUnit) == iNewValue)
		return;

	m_aiUnitProduction.set(eUnit, iNewValue);
	FAssert(getUnitProduction(eUnit) >= 0);

	if (isActiveTeam())
		setInfoDirty(true);

	if (isActiveOwned() && isCitySelected())
		gDLL->UI().setDirty(CityScreen_DIRTY_BIT, true);
}


void CvCity::changeUnitProduction(UnitTypes eUnit, int iChange)
{
	setUnitProduction(eUnit, getUnitProduction(eUnit) + iChange);
}


void CvCity::setUnitProductionTime(UnitTypes eUnit, int iNewValue)
{
	m_aiUnitProductionTime.set(eUnit, iNewValue);
	FAssert(getUnitProductionTime(eUnit) >= 0);
}


void CvCity::changeUnitProductionTime(UnitTypes eUnit, int iChange)
{
	setUnitProductionTime(eUnit, getUnitProductionTime(eUnit) + iChange);
}

// BULL - Production Decay - start
/*	advc.094: Using the preprocessor is less bad than keeping two copies
	of this moderately complex code */
#define IMPLEMENT_BUG_PRODUCTION_DECAY_GETTERS(ORDER, Order, Verb) \
/* Returns true if the given order will decay this turn */ \
bool CvCity::is##Order##ProductionDecay(Order##Types e##Order) const \
{ \
	return (/*isHuman() &&*/ /* advc.094: Not the place to check this */ \
			getProduction##Order() != e##Order && \
			get##Order##Production(e##Order) > 0 && \
			100 * get##Order##ProductionTime(e##Order) >= \
			GC.getDefineINT(CvGlobals::ORDER##_PRODUCTION_DECAY_TIME) * \
			GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).get##Verb##Percent()); \
} \
\
/*	Returns the amount by which the given order will decay once it reaches the limit. */ \
/*	Ignores whether or not the order will actually decay this turn. */ \
int CvCity::get##Order##ProductionDecay(Order##Types e##Order) const \
{ \
	int const iProduction = get##Order##Production(e##Order); \
	return iProduction - \
			(iProduction * \
			GC.getDefineINT(CvGlobals::ORDER##_PRODUCTION_DECAY_PERCENT)) / 100; \
} \
\
/* Returns the number of turns left before the given order will decay */ \
int CvCity::get##Order##ProductionDecayTurns(Order##Types e##Order) const \
{ \
	return 1 + std::max(0, \
			intdiv::uceil( \
			GC.getDefineINT(CvGlobals::ORDER##_PRODUCTION_DECAY_TIME) * \
			GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).get##Verb##Percent(), \
			100) \
			- get##Order##ProductionTime(e##Order)); \
}
IMPLEMENT_BUG_PRODUCTION_DECAY_GETTERS(BUILDING, Building, Construct)
IMPLEMENT_BUG_PRODUCTION_DECAY_GETTERS(UNIT, Unit, Train)
// BULL - Production Decay - end

// advc.opt: Taking advantage of EnumMap.hasContent
bool CvCity::isAnyProductionProgress(OrderTypes eOrder) const
{
	switch(eOrder)
	{
	case ORDER_CONSTRUCT: return m_aiBuildingProduction.isAnyNonDefault();
	case ORDER_CREATE:	return m_aiProjectProduction.isAnyNonDefault();
	case ORDER_MAINTAIN: return false; // Can't make progress on a process
	case ORDER_TRAIN: return m_aiUnitProduction.isAnyNonDefault();
	default: FErrorMsg("Unknown type of production order");
		return false;
	}
}


void CvCity::setGreatPeopleUnitRate(UnitTypes eUnit, int iNewValue)
{
	if (GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE) &&
		GC.getInfo(eUnit).getEspionagePoints() > 0)
	{
		return;
	}
	m_aiGreatPeopleUnitRate.set(eUnit, iNewValue);
	FAssert(getGreatPeopleUnitRate(eUnit) >= 0);
}


void CvCity::changeGreatPeopleUnitRate(UnitTypes eUnit, int iChange)
{
	setGreatPeopleUnitRate(eUnit, getGreatPeopleUnitRate(eUnit) + iChange);
}


void CvCity::setGreatPeopleUnitProgress(UnitTypes eUnit, int iNewValue)
{
	m_aiGreatPeopleUnitProgress.set(eUnit, iNewValue);
	FAssert(getGreatPeopleUnitProgress(eUnit) >= 0);
}


void CvCity::changeGreatPeopleUnitProgress(UnitTypes eUnit, int iChange)
{
	setGreatPeopleUnitProgress(eUnit, getGreatPeopleUnitProgress(eUnit) + iChange);
}


void CvCity::setSpecialistCount(SpecialistTypes eSpecialist, int iNewValue)
{
	int const iOldValue = getSpecialistCount(eSpecialist);
	if (iOldValue == iNewValue)
		return;

	m_aiSpecialistCount.set(eSpecialist, iNewValue);
	FAssert(getSpecialistCount(eSpecialist) >= 0);

	changeSpecialistPopulation(iNewValue - iOldValue);
	processSpecialist(eSpecialist, (iNewValue - iOldValue));

	if (isCitySelected())
		gDLL->UI().setDirty(CitizenButtons_DIRTY_BIT, true);
}


void CvCity::changeSpecialistCount(SpecialistTypes eSpecialist, int iChange)
{
//doto mylon pop working limit -> if the specialist is citizen
//and there are above 21 worked tiles - do not lower the count - do now allow 
//to loose the count or add a working tile above the limit
//	if (eSpecialist == GC.getDEFAULT_SPECIALIST() 
//			&&GC.getDEFAULT_SPECIALIST() != NO_SPECIALIST
//			&& 	iChange < 1
//			&& getWorkingPopulation() > MAX_WORK_TILES)
//		return;
	setSpecialistCount(eSpecialist, getSpecialistCount(eSpecialist) + iChange);
}


void CvCity::alterSpecialistCount(SpecialistTypes eSpecialist, int iChange)
{
	if(iChange == 0)
		return;

	if (isCitizensAutomated())
	{
		if (getForceSpecialistCount(eSpecialist) + iChange < 0)
			setCitizensAutomated(false);
	}

	if (isCitizensAutomated())
	{
		changeForceSpecialistCount(eSpecialist, iChange +
		/*	<advc.121> We're making sure below not to remove the specialist
			we've just added, but afterwards we let the AI juggle the citizens;
			so it can still happen that a human adds e.g. a 2nd scientist
			- the 1st having been assigned by the AI - and ends up with just
			1 scientist. Let's assume that any previously assigned specialists
			of type eSpecialist should also be forced. */
				(iChange <= 0 ? 0 :
				std::max(getSpecialistCount(eSpecialist)
				- getForceSpecialistCount(eSpecialist), 0))); // </advc.121>
		//return;
		/*	(K-Mod. Without the following block,
			extra care is needed inside AI_assignWorkingPlots.) */
	}
	if (iChange > 0)
	{
		for (int i = 0; i < iChange; i++)
		{
			if (extraPopulation() > 0 || AI().AI_removeWorstCitizen(eSpecialist))
			{
				if (isSpecialistValid(eSpecialist, 1))
					changeSpecialistCount(eSpecialist, 1);
			}
		}
	}
	else
	{
		for (int i = 0; i < -iChange; i++)
		{
			if (getSpecialistCount(eSpecialist) <= 0)
				continue;

			changeSpecialistCount(eSpecialist, -1);

			if (eSpecialist != GC.getDEFAULT_SPECIALIST() &&
				GC.getDEFAULT_SPECIALIST() != NO_SPECIALIST)
			{
				changeSpecialistCount(GC.getDEFAULT_SPECIALIST(), 1);
				continue;
			}
			if (extraFreeSpecialists() > 0)
			{
				AI().AI_addBestCitizen(false, true);
				continue;
			}
			int iNumCanWorkPlots = 0;
			for (CityPlotIter it(*this, false); it.hasNext(); ++it)
			{
				if (!isWorkingPlot(it.currID()) && canWork(*it))
					iNumCanWorkPlots++;
			}
			if (iNumCanWorkPlots > 0)
				AI().AI_addBestCitizen(true, false);
			else AI().AI_addBestCitizen(false, true);
		}
	}
}


bool CvCity::isSpecialistValid(SpecialistTypes eSpecialist, int iExtra) const
{
	return (getSpecialistCount(eSpecialist) + iExtra <= getMaxSpecialistCount(eSpecialist) ||
		GET_PLAYER(getOwner()).isSpecialistValid(eSpecialist) ||
		eSpecialist == GC.getDEFAULT_SPECIALIST());
}


void CvCity::changeMaxSpecialistCount(SpecialistTypes eSpecialist, int iChange)
{
//doto mylon - check if this needs the same rule as changeSpecialistCount
	if (iChange != 0)
	{
		//m_aiMaxSpecialistCount.set(eSpecialist, std::max(0, m_aiMaxSpecialistCount.get(eSpecialist) + iChange));
		// <advc>
		m_aiMaxSpecialistCount.add(eSpecialist, iChange);
		FAssert(m_aiMaxSpecialistCount.get(eSpecialist) >= 0); // </advc>
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::setForceSpecialistCount(SpecialistTypes eSpecialist, int iNewValue)
{
	if (getForceSpecialistCount(eSpecialist) == iNewValue)
		return;

	//m_aiForceSpecialistCount.set(eSpecialist, std::max(0, iNewValue));
	// <advc>
	FAssert(iNewValue >= 0);
	m_aiForceSpecialistCount.set(eSpecialist, iNewValue); // </advc>

	if (isCitySelected())
		gDLL->UI().setDirty(Help_DIRTY_BIT, true);
	AI_setAssignWorkDirty(true);
}


void CvCity::changeForceSpecialistCount(SpecialistTypes eSpecialist, int iChange)
{
//doto mylon - check if this needs the same rule as changeSpecialistCount
	setForceSpecialistCount(eSpecialist, getForceSpecialistCount(eSpecialist) + iChange);
}


int CvCity::getAddedFreeSpecialistCount(SpecialistTypes eSpecialist) const
{
	int iR = getFreeSpecialistCount(eSpecialist);
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes eBuilding = kCiv.buildingAt(i);
		int iFreeSpecialists = GC.getInfo(eBuilding).getFreeSpecialistCount(eSpecialist);
		if (iFreeSpecialists > 0)
			iR -= getNumActiveBuilding(eBuilding) * iFreeSpecialists;
	}
	FAssert(iR >= 0);
	return std::max(0, iR);
}


void CvCity::setFreeSpecialistCount(SpecialistTypes eSpecialist, int iNewValue)
{
	int const iOldValue = getFreeSpecialistCount(eSpecialist);
	if (iOldValue == iNewValue)
		return;

	m_aiFreeSpecialistCount.set(eSpecialist, iNewValue);
	FAssert(getFreeSpecialistCount(eSpecialist) >= 0);

	changeNumGreatPeople(iNewValue - iOldValue);
	processSpecialist(eSpecialist, iNewValue - iOldValue);

	if (isCitySelected())
		gDLL->UI().setDirty(CitizenButtons_DIRTY_BIT, true);
}


void CvCity::changeFreeSpecialistCount(SpecialistTypes eSpecialist, int iChange)
{
//doto mylon - check if this needs the same rule as changeSpecialistCount
	setFreeSpecialistCount(eSpecialist, getFreeSpecialistCount(eSpecialist) + iChange);
}


void CvCity::changeImprovementFreeSpecialists(ImprovementTypes eImprov, int iChange)
{
	//m_aiImprovementFreeSpecialists.set(eImprov, std::max(0, m_aiImprovementFreeSpecialists.get(eImprov) + iChange));
	// <advc>
	m_aiImprovementFreeSpecialists.add(eImprov, iChange);
	FAssert(m_aiImprovementFreeSpecialists.get(eImprov) >= 0); // </advc>
}


void CvCity::changeReligionInfluence(ReligionTypes eReligion, int iChange)
{
	m_aiReligionInfluence.add(eReligion, iChange);
	FAssert(getReligionInfluence(eReligion) >= 0);
}


int CvCity::getCurrentStateReligionHappiness() const
{
	if (GET_PLAYER(getOwner()).getStateReligion() != NO_RELIGION)
		return getStateReligionHappiness(GET_PLAYER(getOwner()).getStateReligion());
	return 0;
}


void CvCity::changeStateReligionHappiness(ReligionTypes eReligion, int iChange)
{
	if (iChange != 0)
	{
		m_aiStateReligionHappiness.add(eReligion, iChange);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeUnitCombatFreeExperience(UnitCombatTypes eUnitCombat, int iChange)
{
	m_aiUnitCombatFreeExperience.add(eUnitCombat, iChange);
	FAssert(getUnitCombatFreeExperience(eUnitCombat) >= 0);
}


void CvCity::changeFreePromotionCount(PromotionTypes ePromo, int iChange)
{
	m_aiFreePromotionCount.add(ePromo, iChange);
	FAssert(getFreePromotionCount(ePromo) >= 0);
}


void CvCity::changeSpecialistFreeExperience(int iChange)
{
	m_iSpecialistFreeExperience += iChange;
	FAssert(m_iSpecialistFreeExperience >= 0);
}


void CvCity::changeEspionageDefenseModifier(int iChange)
{
	m_iEspionageDefenseModifier += iChange;
}

// advc:
int CvCity::cultureTimes100InsertedByMission(EspionageMissionTypes eMission) const
{
	int iCultureAmount = GC.getInfo(eMission).getCityInsertCultureAmountFactor() *
			countTotalCultureTimes100();
	iCultureAmount /= 100;
	return std::max(100, iCultureAmount);
}


bool CvCity::isWorkingPlot(CvPlot const& kPlot) const
{
	// advc.enum: Use CityPlotTypes instead of int
	CityPlotTypes eIndex = getCityPlotIndex(kPlot);
	if (eIndex != NO_CITYPLOT)
		return isWorkingPlot(eIndex);
	return false;
}


void CvCity::setWorkingPlot(CityPlotTypes ePlot, bool bNewValue, bool test) // advc.enum: CityPlotTypes
{
	if (isWorkingPlot(ePlot) == bNewValue)
		return;
	// <advc.064b> To avoid unnecessary update of city screen
	bool bSelected = isCitySelected();
	int iOldTurns = -1;
	if(bSelected && GET_PLAYER(getOwner()).canGoldRush())
		iOldTurns = getProductionTurnsLeft();
	// </advc.064b>
//mylon enhanced cities doto advc version
	//m_abWorkingPlot.set(ePlot, bNewValue);
	FAssertBounds(0, NUM_CITY_PLOTS, ePlot);
	m_abWorkingPlot[ePlot] = bNewValue;
	
//doto mylon pop count limit working
	// if the working plots are above 21 and the setWorkingPlot was called with a change the tile to working (true)
	// make sure to remove the worst tile before setting up a new one,
	int iGrowthValue = AI().AI_growthValuePerFood();
	int iWorstValue = MAX_INT;
	CityPlotTypes eWorstPlot = NO_CITYPLOT;
	if (extraVisiblePopulationMylon() == 1 
		&& bNewValue /* && !test*/)
	{	
	// check all the plots we are working
		for (WorkingPlotIter it(*this, false); it.hasNext(); ++it)
		{
			int iValue = AI().AI_plotValue(*it, true, false, false, iGrowthValue);
			if (iValue < iWorstValue)
			{
				iWorstValue = iValue;
				eWorstPlot = it.currID();
			}
		}
		m_abWorkingPlot[eWorstPlot] = false;
		CvPlot* pPlot = getCityIndexPlot(ePlot);
		if (pPlot != NULL)
		{
			FAssertMsg(pPlot->getWorkingCity() == this, "WorkingCity is expected to be this");
/*
			if (isWorkingPlot(ePlot))
			{
				if (ePlot != CITY_HOME_PLOT)
					changeWorkingPopulation(1);
				FOR_EACH_ENUM(Yield)
					changeBaseYieldRate(eLoopYield, pPlot->getYield(eLoopYield));
				// update plot builder special case where a plot is being worked but is (a) unimproved  or (b) un-bonus'ed
				pPlot->updatePlotBuilder();
			}
			else
			{
*/				if (ePlot != CITY_HOME_PLOT)
					changeWorkingPopulation(-1);
				FOR_EACH_ENUM(Yield)
					changeBaseYieldRate(eLoopYield, -pPlot->getYield(eLoopYield));
//			}

			if (isActiveTeam() || GC.getGame().isDebugMode())
				pPlot->updateSymbolDisplay();
		}
		if (bSelected)
		{
			gDLL->UI().setDirty(InfoPane_DIRTY_BIT, true);
			gDLL->UI().setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->UI().setDirty(ColoredPlots_DIRTY_BIT, true);
			// <advc.064b>
			if(iOldTurns >= 0 && getProductionTurnsLeft() != iOldTurns)
				gDLL->UI().setDirty(SelectionButtons_DIRTY_BIT, true);
		// </advc.064b>
		}
	}
	CvPlot* pPlot = getCityIndexPlot(ePlot);
	if (pPlot != NULL)
	{
		FAssertMsg(pPlot->getWorkingCity() == this, "WorkingCity is expected to be this");

		if (isWorkingPlot(ePlot))
		{
			if (ePlot != CITY_HOME_PLOT)
				changeWorkingPopulation(1);
			FOR_EACH_ENUM(Yield)
				changeBaseYieldRate(eLoopYield, pPlot->getYield(eLoopYield));
			// update plot builder special case where a plot is being worked but is (a) unimproved  or (b) un-bonus'ed
			pPlot->updatePlotBuilder();
		}
		else
		{
			if (ePlot != CITY_HOME_PLOT)
				changeWorkingPopulation(-1);
			FOR_EACH_ENUM(Yield)
				changeBaseYieldRate(eLoopYield, -pPlot->getYield(eLoopYield));
		}

		if (isActiveTeam() || GC.getGame().isDebugMode())
			pPlot->updateSymbolDisplay();
	}
	if (bSelected)
	{
		gDLL->UI().setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->UI().setDirty(CityScreen_DIRTY_BIT, true);
		gDLL->UI().setDirty(ColoredPlots_DIRTY_BIT, true);
		// <advc.064b>
		if(iOldTurns >= 0 && getProductionTurnsLeft() != iOldTurns)
			gDLL->UI().setDirty(SelectionButtons_DIRTY_BIT, true);
		// </advc.064b>
	}

	//doto mylon pop count limit working
	// if the working plots are above 21 and the setWorkingPlot was called with a change the tile to working (true)
	// make sure to remove the worst tile before setting up a new one,
//	if (extraVisiblePopulationMylon() == 1 && bNewValue /* && !test*/)
//	{
//		AI().AI_removeWorstCitizen();
//		return;
//	}
}


void CvCity::setWorkingPlot(CvPlot& kPlot, bool bNewValue)
{
	setWorkingPlot(getCityPlotIndex(kPlot), bNewValue);
}


void CvCity::alterWorkingPlot(CityPlotTypes ePlot)
{
	if (ePlot == CITY_HOME_PLOT)
	{
		setCitizensAutomated(true);
		return;
	}
	CvPlot* pPlot = getCityIndexPlot(ePlot);
	if (pPlot == NULL)
		return;
	if (!canWork(*pPlot))
	{
		if (pPlot->getOwner() == getOwner())
			pPlot->setWorkingCityOverride(this);
		return;
	}

	setCitizensAutomated(false);
	if (isWorkingPlot(ePlot))
	{
		setWorkingPlot(ePlot, false);
		if (GC.getDEFAULT_SPECIALIST() != NO_SPECIALIST)
			changeSpecialistCount(GC.getDEFAULT_SPECIALIST(), 1);
		else AI().AI_addBestCitizen(false, true);
	}
	else if (extraPopulation() > 0 || AI().AI_removeWorstCitizen())
		setWorkingPlot(ePlot, true);
}

// advc.003w:
int CvCity::getNumRealBuilding(BuildingClassTypes eBuildingClass) const
{
	BuildingTypes eBuilding = getCivilization().getBuilding(eBuildingClass);
	if (eBuilding == NO_BUILDING)
		return 0;
	return getNumRealBuilding(eBuilding);
}


void CvCity::setNumRealBuilding(BuildingTypes eBuilding, int iNewValue,
	bool bEndOfTurn) // advc.001x
{
	setNumRealBuildingTimed(eBuilding, iNewValue, true, getOwner(),
			GC.getGame().getGameTurnYear(), /* advc.001x: */ bEndOfTurn);
}


void CvCity::setNumRealBuildingTimed(BuildingTypes eBuilding, int iNewValue, bool bFirst,
	PlayerTypes eOriginalOwner, int iOriginalTime, /* advc.001x: */ bool bEndOfTurn)
{

	int const iChange = iNewValue - getNumRealBuilding(eBuilding);
	if(iChange == 0)
		return;

	int const iOldNumBuilding = getNumBuilding(eBuilding);
	m_aiNumRealBuilding.set(eBuilding, iNewValue);
	if (getNumRealBuilding(eBuilding) > 0)
	{
		m_aeBuildingOriginalOwner.set(eBuilding, eOriginalOwner);
		m_aiBuildingOriginalTime.set(eBuilding, iOriginalTime);
	}
	else
	{
		m_aeBuildingOriginalOwner.set(eBuilding, NO_PLAYER);
		m_aiBuildingOriginalTime.set(eBuilding, iBuildingOriginalTimeUnknown);
	}
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	if (iOldNumBuilding != getNumBuilding(eBuilding))
	{
		if (getNumRealBuilding(eBuilding) > 0 && kBuilding.isStateReligion())
		{
			FOR_EACH_ENUM(VoteSource)
			{
				if (kBuilding.getVoteSourceType() == eLoopVoteSource &&
					GC.getGame().getVoteSourceReligion(eLoopVoteSource) == NO_RELIGION)
				{
					FAssert(GET_PLAYER(getOwner()).getStateReligion() != NO_RELIGION);
					GC.getGame().setVoteSourceReligion(eLoopVoteSource,
							GET_PLAYER(getOwner()).getStateReligion(), true);
				}
			}
		}
		// advc (note): Not the same as iChange, which only counts NumRealBuilding.
		processBuilding(eBuilding, getNumBuilding(eBuilding) - iOldNumBuilding);
	}
	if (!GC.getInfo(kBuilding.getBuildingClassType()).isNoLimit())
	{
		if (kBuilding.isWorldWonder())
			changeNumWorldWonders(iChange);
		else if (kBuilding.isTeamWonder())
			changeNumTeamWonders(iChange);
		else if (kBuilding.isNationalWonder())
			changeNumNationalWonders(iChange);
		else changeNumBuildings(iChange);
	}
	if (iChange > 0 && bFirst)
	{
		if (kBuilding.isCapital())
			GET_PLAYER(getOwner()).setCapital(this);

		if (GC.getGame().isFinalInitialized() && !gDLL->GetWorldBuilderMode())
		{
			CvWString szBuffer;
			CvPlayer& kOwner = GET_PLAYER(getOwner());

			if (kBuilding.isGoldenAge())
			{
				//kOwner.changeGoldenAgeTurns(iChange * (kOwner.getGoldenAgeLength() + 1));
				// <advc.001x>
				if (bEndOfTurn)
				{
					for (int i = 0; i < iChange; i++)
						kOwner.startGoldenAgeDelayed();
				}
				else kOwner.changeGoldenAgeTurns(iChange * kOwner.getGoldenAgeLength());
				// </advc.001x>
			}
			if (kBuilding.getGlobalPopulationChange() != 0)
			{
				for (MemberAIIter it(getTeam()); it.hasNext(); ++it)
				{
					CvPlayerAI const& kMember = *it;
					if (kMember.getID() != getOwner() && !kBuilding.isTeamShare())
						continue;
					FOR_EACH_CITYAI_VAR(pLoopCity, kMember)
					{
						pLoopCity->setPopulation(std::max(1, pLoopCity->getPopulation() +
								iChange * kBuilding.getGlobalPopulationChange()));
						// so that subsequent cities don't starve with the extra citizen working nothing
						pLoopCity->AI_updateAssignWork();
					}
				}
			}
			FOR_EACH_NON_DEFAULT_PAIR(kBuilding.
				getReligionChange(), Religion, int)
			{
				if (perReligionVal.second > 0)
					setHasReligion(perReligionVal.first, true, true, true);
			}
			int const iFreeTechs = kBuilding.getFreeTechs() * iChange; // advc
			if (iFreeTechs > 0)
			{
				if (!isHuman())
				{
					for (int i = 0; i < iFreeTechs; i++)
						GET_PLAYER(getOwner()).AI_chooseFreeTech();
				}
				else
				{
					szBuffer = gDLL->getText(  // <advc.008e>
							kBuilding.nameNeedsArticle() ?
							"TXT_KEY_MISC_COMPLETED_WONDER_CHOOSE_TECH_THE" :
							"TXT_KEY_MISC_COMPLETED_WONDER_CHOOSE_TECH", // </advc.008e>
							kBuilding.getTextKeyWide());
					GET_PLAYER(getOwner()).chooseTech(iFreeTechs, szBuffer.GetCString());
				}
			}
/*** HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave START ***/
			TechTypes eFreeSpecificTech = (TechTypes) kBuilding.getFreeSpecificTech();
			if (eFreeSpecificTech != NO_TECH)
			{
				GET_TEAM(getTeam()).setHasTech(eFreeSpecificTech, true, getOwner(), true, true);
			}
/*** HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave START ***/

			if (kBuilding.isWorldWonder())
			{
				szBuffer = gDLL->getText(  // <advc.008e>
						kBuilding.nameNeedsArticle() ?
						"TXT_KEY_MISC_COMPLETES_WONDER_THE" :
						"TXT_KEY_MISC_COMPLETES_WONDER", // </advc.008e>
						GET_PLAYER(getOwner()).getNameKey(), kBuilding.getTextKeyWide());
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwner(), szBuffer,
						getX(), getY(), GC.getColorType("BUILDING_TEXT"));
				// <advc.106>
				for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
				{
					CvPlayer& kObs = *it;
					bool const bRevealed = isRevealed(kObs.getTeam(), true);
					bool const bMet = GET_TEAM(kObs.getID()).isHasMet(getTeam());
					if (bRevealed /* advc.127: */ || kObs.isSpectator())
					{	// <advc.008e>
						szBuffer = gDLL->getText(kBuilding.nameNeedsArticle() ?
								"TXT_KEY_MISC_WONDER_COMPLETED_CITY_THE" :
								"TXT_KEY_MISC_WONDER_COMPLETED_CITY", // </advc.008e>
								GET_PLAYER(getOwner()).getNameKey(),
								kBuilding.getTextKeyWide(), getName().GetCString());
					}
					else if (bMet)
					{	// <advc.008e>
						szBuffer = gDLL->getText(kBuilding.nameNeedsArticle() ?
								"TXT_KEY_MISC_WONDER_COMPLETED_THE" :
								"TXT_KEY_MISC_WONDER_COMPLETED", // </advc.008e>
								GET_PLAYER(getOwner()).getNameKey(),
								kBuilding.getTextKeyWide());
					}
					else // <advc.008>
					{
						szBuffer = gDLL->getText(kBuilding.nameNeedsArticle() ?
								"TXT_KEY_MISC_WONDER_COMPLETED_UNKNOWN_THE" :
								"TXT_KEY_MISC_WONDER_COMPLETED_UNKNOWN",
								kBuilding.getTextKeyWide());
					}
					gDLL->UI().addMessage(kObs.getID(), false,
							-1, szBuffer, "AS2D_WONDER_BUILDING_BUILD",
							MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY, // advc.106b
							kBuilding.getArtInfo()->getButton(),
							GC.getColorType("BUILDING_TEXT"),
							// Indicate location only if revealed.
							bRevealed ? getX() : -1, bRevealed ? getY() : -1,
							bRevealed, bRevealed);
				} // </advc.106>
			}
		}
		if (kBuilding.isAllowsNukes())
			GC.getGame().makeNukesValid(true);

		GC.getGame().incrementBuildingClassCreatedCount(kBuilding.getBuildingClassType());
		/*  advc: Increments greater than 1 aren't supported. As for decrements:
			a building that gets destroyed has nevertheless been created ...
			That said, removing and recreating world wonders through WB will
			lead to failed assertions in CvGame::isBuildingClassMaxedOut. */
		FAssert(iChange == 1);
	}

	if (bFirst) //great wall
	{
		if (kBuilding.isAreaBorderObstacle() ||
		/*  advc.310: Show GW as 3D model even when playing w/o Barbarians
			(BorderObstacle ability disabled through CvBuildingInfo then). */
			kBuilding.getAreaTradeRoutes() > 0)
		{
			int iCountExisting = 0;
			CvCivilization const& kCiv = getCivilization();
			for (int i = 0; i < kCiv.getNumBuildings(); i++)
			{
				BuildingTypes eLoopBuilding = kCiv.buildingAt(i);
				if (eBuilding != eLoopBuilding &&
					GC.getInfo(eLoopBuilding).isAreaBorderObstacle())
				{
					iCountExisting += getNumRealBuilding(eLoopBuilding);
				}
			}
			//if (iCountExisting == 1 && iNewValue == 0)
			/*  advc.001: m_aiNumRealBuilding.get(eBuilding) has already been decreased
				(and the loop above skips eBuilding anyway), so iCountExisting isn't
				going to be 1. */
			if (iCountExisting <= 0 && iOldNumBuilding > 0 && iNewValue <= 0)
				gDLL->getEngineIFace()->RemoveGreatWall(this);
			else if (iCountExisting == 0 && iNewValue > 0)
			{
				//gDLL->getEngineIFace()->AddGreatWall(this);
				addGreatWall(); // advc.310
			}
		}
	}
}


void CvCity::setNumFreeBuilding(BuildingTypes eBuilding, int iNewValue)
{
	if(getNumFreeBuilding(eBuilding) == iNewValue)
		return;

	int const iOldNumBuilding = getNumBuilding(eBuilding);
	m_aiNumFreeBuilding.set(eBuilding, iNewValue);
	if (iOldNumBuilding != getNumBuilding(eBuilding))
		processBuilding(eBuilding, iNewValue - iOldNumBuilding);
}


void CvCity::setHasReligion(ReligionTypes eReligion, bool bNewValue, bool bAnnounce, bool bArrows,
	PlayerTypes eSpreadPlayer) // advc.106e
{
	if (isHasReligion(eReligion) == bNewValue)
		return;

	FOR_EACH_ENUM(VoteSource)
		processVoteSource(eLoopVoteSource, false);
	// <advc.130n>
	for (PlayerAIIter<MAJOR_CIV> itPlayer; itPlayer.hasNext(); ++itPlayer)
	{
		if (isRevealed(itPlayer->getTeam()))
			itPlayer->AI_updateIdeologyAttitude(-1, *this);
	} // </advc.130n>
	m_abHasReligion.set(eReligion, bNewValue);
	// <advc.130n>
	for (PlayerAIIter<MAJOR_CIV> itPlayer; itPlayer.hasNext(); ++itPlayer)
	{
		if (isRevealed(itPlayer->getTeam()))
			itPlayer->AI_updateIdeologyAttitude(1, *this);
	} // </advc.130n>
	FOR_EACH_ENUM(VoteSource)
		processVoteSource(eLoopVoteSource, true);

	CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	kOwner.changeHasReligionCount(eReligion, isHasReligion(eReligion) ? 1 : -1);
	updateMaintenance();
	updateReligionHappiness();
	updateReligionCommerce();

	AI_setAssignWorkDirty(true);
	setInfoDirty(true);
	// <advc.106e>
	int const iOwnerEra = kOwner.getCurrentEra();
	int const iEraThresh = GC.getDefineINT("STOP_RELIGION_SPREAD_ANNOUNCE_ERA");
	bool const bAnnounceStateReligionSpread = (GC.getDefineINT(
			"ANNOUNCE_STATE_RELIGION_SPREAD") > 0);
	// </advc.106e>
	if (isHasReligion(eReligion))
	{
		CvGame& kGame = GC.getGame();
		kGame.makeReligionFounded(eReligion, kOwner.getID());
		if (bAnnounce)
		{
			if (kGame.getHolyCity(eReligion) != this)
			{
				for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
				{
					CvPlayer const& kObs = *it;
					if (!isRevealed(kObs.getTeam()))
						continue;
					// <advc.106e>
					if ((iOwnerEra < iEraThresh && (bAnnounceStateReligionSpread ||
						kOwner.getStateReligion() != eReligion)) ||
						eSpreadPlayer == kObs.getID() || // </advc.106e>
						kOwner.getID() == kObs.getID() ||
						// advc.106e: Disable this clause
						//(kObs.getStateReligion() == eReligion) ||
						kObs.hasHolyCity(eReligion))
					{
						CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_RELIGION_SPREAD",
								GC.getInfo(eReligion).getTextKeyWide(), getNameKey());
						gDLL->UI().addMessage(kObs.getID(), false,
								-1, // (K-Mod note: event time was originally "long".)
								szBuffer, GC.getInfo(eReligion).getSound(),
								MESSAGE_TYPE_MINOR_EVENT, // advc.106b: was MAJOR
								GC.getInfo(eReligion).getButton(), NO_COLOR,
								getX(), getY(), bArrows, bArrows);
					}
				}
			}
			if (isHuman() && kOwner.getHasReligionCount(eReligion) == 1)
			{
				if (kOwner.canConvert(eReligion) && kOwner.getStateReligion() == NO_RELIGION)
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHANGERELIGION);
					pInfo->setData1(eReligion);
					gDLL->UI().addPopup(pInfo, kOwner.getID());
				}
				/*	<advc.004w> Update text of resource indicators
					(CvGameTextMgr::setBonusExtraHelp) */
				if (isActiveOwned() && kGame.getCurrentLayer() == GLOBE_LAYER_RESOURCE)
				{
					gDLL->UI().setDirty(GlobeLayer_DIRTY_BIT, true);
					// advc.003p:
					kOwner.setBonusHelpDirty();
				} // </advc.004w>
			}
		}
	} // K-Mod start
	else // religion removed
	{
		if (bAnnounce)
		{	// <advc.106e> Announce removed religion to other civs as well
			for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
			{
				CvPlayer const& kObs = *it;
				if (!isRevealed(kObs.getTeam()))
					continue;
				if (eSpreadPlayer != kObs.getID() && iOwnerEra >= iEraThresh &&
					kOwner.getID() != kObs.getID() && !kObs.hasHolyCity(eReligion))
				{
					continue;
				} // </advc.106e>
				CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_RELIGION_REMOVE",
						GC.getInfo(eReligion).getTextKeyWide(), getNameKey());
				gDLL->UI().addMessage(
						kObs.getID(), // advc.106e: was getOwner() in K-Mod
						false, -1, szBuffer, "AS2D_BLIGHT",
						MESSAGE_TYPE_MINOR_EVENT, // advc.106b: was MAJOR
						GC.getInfo(eReligion).getButton(), GC.getColorType("RED"),
						getX(), getY(), bArrows, bArrows);
			}
		}
	} // K-Mod end
	if (bNewValue)
		CvEventReporter::getInstance().religionSpread(eReligion, kOwner.getID(), this);
	else CvEventReporter::getInstance().religionRemove(eReligion, kOwner.getID(), this);
}

// K-Mod: A rating for how strong a religion can take hold in this city
int CvCity::getReligionGrip(ReligionTypes eReligion) const
{
	PROFILE_FUNC();
	if (!GC.getGame().isReligionFounded(eReligion))
		return 0;
	// <advc.opt>
	static int const iRELIGION_INFLUENCE_POPULATION_WEIGHT = GC.getDefineINT("RELIGION_INFLUENCE_POPULATION_WEIGHT");
	static int const iRELIGION_INFLUENCE_STATE_RELIGION_WEIGHT = GC.getDefineINT("RELIGION_INFLUENCE_STATE_RELIGION_WEIGHT");
	static int const iRELIGION_INFLUENCE_BUILDING_WEIGHT = GC.getDefineINT("RELIGION_INFLUENCE_BUILDING_WEIGHT");
	static int const iRELIGION_INFLUENCE_SHRINE_WEIGHT = GC.getDefineINT("RELIGION_INFLUENCE_SHRINE_WEIGHT");
	static int const iRELIGION_INFLUENCE_DISTANCE_WEIGHT = GC.getDefineINT("RELIGION_INFLUENCE_DISTANCE_WEIGHT");
	static int const iRELIGION_INFLUENCE_TIME_SCALE = GC.getDefineINT("RELIGION_INFLUENCE_TIME_SCALE");
	static int const iRELIGION_INFLUENCE_TIME_WEIGHT = GC.getDefineINT("RELIGION_INFLUENCE_TIME_WEIGHT");
	// </advc.opt>
	int iScore = 0;

	if (isHasReligion(eReligion))
	{
		int iTempScore = iRELIGION_INFLUENCE_POPULATION_WEIGHT * getPopulation();
		// <advc.099c> Try to satisfy foreign pop
		if (revoltProbability(true, false, true) > 0)
		{
			PlayerTypes cultOwner = calculateCulturalOwner();
			if(GET_PLAYER(cultOwner).getStateReligion() == eReligion)
				iTempScore *= 2;
		}
		iScore += iTempScore; // </advc.099c>
		if (GET_PLAYER(getOwner()).getStateReligion() == eReligion)
			iScore += iRELIGION_INFLUENCE_STATE_RELIGION_WEIGHT;
	}

	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes eBuilding = kCiv.buildingAt(i);
		if (GC.getInfo(eBuilding).getPrereqReligion() == eReligion)
		{
			iScore += iRELIGION_INFLUENCE_BUILDING_WEIGHT *
					getNumActiveBuilding(eBuilding);
		}
	}

	CvCity* pHolyCity = GC.getGame().getHolyCity(eReligion);
	if (pHolyCity && isConnectedTo(*pHolyCity))
	{
		if (pHolyCity->hasShrine(eReligion))
			iScore += iRELIGION_INFLUENCE_SHRINE_WEIGHT;

		int iDistance = plotDistance(getX(), getY(), pHolyCity->getX(), pHolyCity->getY());
		iScore += iRELIGION_INFLUENCE_DISTANCE_WEIGHT *
				// <advc.140> was maxPlotDistance
				(GC.getMap().maxTypicalDistance() - iDistance) /
				GC.getMap().maxTypicalDistance(); // </advc.140>
	}

	int iCurrentTurn = GC.getGame().getGameTurn();
	int iTurnFounded = GC.getGame().getReligionGameTurnFounded(eReligion);
	int iTimeScale = iRELIGION_INFLUENCE_TIME_SCALE * GC.getInfo(GC.getGame().
			getGameSpeedType()).getVictoryDelayPercent()/100;
	iScore += iRELIGION_INFLUENCE_TIME_WEIGHT * (iTurnFounded + iTimeScale) /
			(iCurrentTurn + iTimeScale);

	return iScore; // note. the random part is not included in this function.
}

// advc: Renamed from "processVoteSourceBonus"
void CvCity::processVoteSource(VoteSourceTypes eVoteSource, bool bActive)
{
	if (!GET_PLAYER(getOwner()).isLoyalMember(eVoteSource))
		return;

	if (!GC.getGame().isDiploVote(eVoteSource))
		return;

	ReligionTypes const eReligion = GC.getGame().getVoteSourceReligion(eVoteSource);
	{
		SpecialistTypes const eSpecialist = GC.getInfo(eVoteSource).getFreeSpecialist();
		if (eSpecialist != NO_SPECIALIST)
		{
			if (eReligion == NO_RELIGION || isHasReligion(eReligion))
				changeFreeSpecialistCount(eSpecialist, bActive ? 1 : -1);
		}
	}
	if (eReligion == NO_RELIGION || !isHasReligion(eReligion))
		return;

	FOR_EACH_ENUM(Yield)
	{
		int iChange = GC.getInfo(eVoteSource).getReligionYield(eLoopYield);
		if (!bActive)
			iChange = -iChange;
		if (iChange != 0)
		{
			FOR_EACH_ENUM(Building)
			{
				if (GC.getInfo(eLoopBuilding).getReligionType() == eReligion)
				{
					changeBuildingYieldChange(CvCivilization::buildingClass(eLoopBuilding),
							eLoopYield, iChange);
				}
			}
		}
	}
	FOR_EACH_ENUM(Commerce)
	{
		int iChange = GC.getInfo(eVoteSource).getReligionCommerce(eLoopCommerce);
		if (!bActive)
			iChange = -iChange;
		if (iChange != 0)
		{
			FOR_EACH_ENUM(Building)
			{
				if (GC.getInfo(eLoopBuilding).getReligionType() == eReligion)
				{
					changeBuildingCommerceChange(CvCivilization::buildingClass(eLoopBuilding),
							eLoopCommerce, iChange);
				}
			}
		}
	}
}


void CvCity::setHasCorporation(CorporationTypes eCorp, bool bNewValue, bool bAnnounce, bool bArrows)
{
	if (isHasCorporation(eCorp) == bNewValue)
		return;

	if (bNewValue)
	{
		bool bReplacedHeadquarters = false;
		FOR_EACH_ENUM(Corporation)
		{
			if (eLoopCorporation != eCorp && isHasCorporation(eLoopCorporation) &&
				GC.getGame().isCompetingCorporation(eLoopCorporation, eCorp))
			{
				if (GC.getGame().getHeadquarters(eLoopCorporation) == this)
				{
					GC.getGame().replaceCorporation(eLoopCorporation, eCorp);
					bReplacedHeadquarters = true;
				}
				else setHasCorporation(eLoopCorporation, false, false);
			}
		}
		if (bReplacedHeadquarters)
			return; // already set the corporation in this city
	}

	m_abHasCorporation.set(eCorp, bNewValue);
	GET_PLAYER(getOwner()).changeHasCorporationCount(eCorp, isHasCorporation(eCorp) ? 1 : -1);

	CvCity* pHeadquarters = GC.getGame().getHeadquarters(eCorp);
	if (pHeadquarters != NULL)
		pHeadquarters->updateCorporation();

	updateCorporation();

	AI_setAssignWorkDirty(true);
	setInfoDirty(true);

	if (isHasCorporation(eCorp))
		GC.getGame().makeCorporationFounded(eCorp, getOwner());

	if (bAnnounce)
	{
		CvCorporationInfo const& kCorp = GC.getInfo(eCorp);
		for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
		{
			CvPlayer const& kObs = *it;
			// <advc.106e>
			if(kObs.hasHeadquarters(eCorp))
				getPlot().setRevealed(kObs.getTeam(), true, false, NO_TEAM, false);
			//if (getOwner() == kObs.getID() || kObs.hasHeadquarters(eCorp)) // BtS
			if (isRevealed(kObs.getTeam())) // </advc.106e>
			{
				if (getOwner() == kObs.getID())
				{
					CvWStringBuffer szBonusString;
					GAMETEXT.setCorporationHelpCity(szBonusString, eCorp, this);

					CvWString szBonusList;
					bool bFirst = true;
					for (int i = 0; i < kCorp.getNumPrereqBonuses(); i++)
					{
						BonusTypes eBonus = kCorp.getPrereqBonus(i);
						CvWString szTemp;
						szTemp.Format(L"%s", GC.getInfo(eBonus).getDescription());
						setListHelp(szBonusList, L"", szTemp, L", ", bFirst);
					}
					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CORPORATION_SPREAD_BONUS",
							kCorp.getTextKeyWide(), szBonusString.getCString(),
							getNameKey(), szBonusList.GetCString());
					gDLL->UI().addMessage(kObs.getID(), false, -1, szBuffer, kCorp.getSound(),
							MESSAGE_TYPE_MINOR_EVENT, kCorp.getButton(),
							NO_COLOR, getX(), getY(), bArrows, bArrows);
				}
				/*	K-Mod. We don't need two announcements every time a corp spreads.
					So I've put the general announcement inside this 'else' block. */
				else
				{
					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CORPORATION_SPREAD",
							kCorp.getTextKeyWide(), getNameKey());
					gDLL->UI().addMessage(kObs.getID(), false, -1, szBuffer,
							kCorp.getSound(),
							MESSAGE_TYPE_MINOR_EVENT, // advc.106b: was MAJOR
							kCorp.getButton(), NO_COLOR, getX(), getY(), bArrows, bArrows);
				} // K-Mod end
			}
		}
	}
	if (bNewValue)
		CvEventReporter::getInstance().corporationSpread(eCorp, getOwner(), this);
	else CvEventReporter::getInstance().corporationRemove(eCorp, getOwner(), this);
}


CvCity* CvCity::getTradeCity(int iIndex) const
{
	FAssertBounds(0, m_aTradeCities.size(), iIndex);
	return getCity(m_aTradeCities[iIndex]);
}


int CvCity::getTradeRoutes() const
{
	/*  <advc.123e> 0 trade routes result in 0 profit from plundering; see
		CvUnit::collectBlockadeGold */
	if(getOwner() == BARBARIAN_PLAYER)
		return 0; // </advc.123e>

	int iTradeRoutes = GC.getGame().getTradeRoutes();
	iTradeRoutes += GET_PLAYER(getOwner()).getTradeRoutes();
	// advc.310: Continental TR no longer included in player TR
	iTradeRoutes += getArea().getTradeRoutes(getOwner());
	if (isCoastal())
		iTradeRoutes += GET_PLAYER(getOwner()).getCoastalTradeRoutes();
	iTradeRoutes += getExtraTradeRoutes();

	return std::min(iTradeRoutes, GC.getDefineINT(CvGlobals::MAX_TRADE_ROUTES));
}


void CvCity::clearTradeRoutes()
{
	for (size_t i = 0; i < m_aTradeCities.size(); i++)
	{
		CvCity* pLoopCity = getTradeCity(i);
		if (pLoopCity != NULL)
			pLoopCity->setTradeRoute(getOwner(), false);
		m_aTradeCities[i].reset();
	}
}

// XXX eventually, this needs to be done when roads are built/destroyed...
void CvCity::updateTradeRoutes()  // advc: refactored
{
	int const iMaxTradeRoutes = GC.getDefineINT(CvGlobals::MAX_TRADE_ROUTES);
	CvPlayer const& kOwner = GET_PLAYER(getOwner());

	clearTradeRoutes();

	if (!isDisorder() && !isPlundered())
	{
		int iTradeRoutes = getTradeRoutes();
		FAssert(iTradeRoutes <= (int)m_aTradeCities.size()); // advc
		static bool const bIgnorePlotGroups = (GC.getDefineBOOL("IGNORE_PLOT_GROUP_FOR_TRADE_ROUTES")); // advc.opt
		int* aiBestValue = new int[iMaxTradeRoutes](); // value-initialize
		for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
		{
			CvPlayer const& kPartner = *it;
			if(!kOwner.canHaveTradeRoutesWith(kPartner.getID()))
				continue;

			FOR_EACH_CITY(pLoopCity, kPartner)
			{
				if(pLoopCity == this)
					continue;
				/*  <advc.124> A connection along revealed tiles ensures that the
					city tile is revealed, but not that the city is revealed. */
				if(pLoopCity->isDisorder() || !pLoopCity->isRevealed(getTeam()))
					continue; // <advc.124>
				if(pLoopCity->isTradeRoute(kOwner.getID()) &&
					getTeam() != kPartner.getTeam())
				{
					continue;
				}
				if(!bIgnorePlotGroups &&
					pLoopCity->plotGroup(kOwner.getID()) != plotGroup(kOwner.getID()))
				{
					continue;
				}
				/*	advc: Times 100 so that there are fewer ties (which, currently,
					are broken arbitrarily based on player and city id). */
				int iValue = calculateTradeProfitTimes100(pLoopCity);
				for (int i = 0; i < iTradeRoutes; i++)
				{
					if(iValue <= aiBestValue[i])
						continue;
					for (int j = iTradeRoutes - 1; j > i; j--)
					{
						aiBestValue[j] = aiBestValue[j - 1];
						FAssertBounds(1, m_aTradeCities.size(), j);
						m_aTradeCities[j] = m_aTradeCities[j - 1];
					}
					aiBestValue[i] = iValue;
					m_aTradeCities[i] = pLoopCity->getIDInfo();
					break;
				}
			}
		}
		SAFE_DELETE_ARRAY(aiBestValue);
	}

	int iTradeProfit = 0;
	for (int i = 0; i < iMaxTradeRoutes; i++)
	{
		CvCity* pLoopCity = getTradeCity(i);
		if (pLoopCity != NULL)
		{
			pLoopCity->setTradeRoute(kOwner.getID(), true);
			iTradeProfit += calculateTradeProfit(pLoopCity);
		}
	}
	FOR_EACH_ENUM(Yield)
	{
		// XXX could take this out if handled when CvPlotGroup changes...
		setTradeYield(eLoopYield, calculateTradeYield(eLoopYield, iTradeProfit));
	}
}


void CvCity::clearOrderQueue()
{
	while (headOrderQueueNode() != NULL)
	{
		popOrder(0);
	}
	if (isActiveTeam() || GC.getGame().isDebugMode())
		setInfoDirty(true);
}


void CvCity::pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave,
	bool bPop, int iPosition, bool bForce)
{
	OrderData order;
	bool bBuildingUnit = false;
	bool bBuildingBuilding = false;

	if (bPop)
		popOrder(0);

	bool bValid = false;

	switch (eOrder)
	{
	case ORDER_TRAIN:
	{
		UnitTypes const eUnit = (UnitTypes)iData1; // advc
		if (canTrain(eUnit) || bForce)
		{
			if (iData2 == NO_UNITAI)
				iData2 = GC.getInfo(eUnit).getDefaultUnitAIType();
			UnitAITypes const eUnitAI = (UnitAITypes)iData2; // advc
			GET_PLAYER(getOwner()).changeUnitClassMaking(GC.getInfo(eUnit).getUnitClassType(), 1);
			getArea().changeNumTrainAIUnits(getOwner(), eUnitAI, 1);
			GET_PLAYER(getOwner()).AI_changeNumTrainAIUnits(eUnitAI, 1);

			bValid = true;
			bBuildingUnit = true;
			CvEventReporter::getInstance().cityBuildingUnit(this, eUnit);
			if (gCityLogLevel >= 1)
			{
				CvWString szString;
				getUnitAIString(szString, eUnitAI);
				logBBAI("    City %S pushes production of unit %S with UNITAI %S", getName().GetCString(), GC.getInfo(eUnit).getDescription(), szString.GetCString());
			}
		}
		break;
	}
	case ORDER_CONSTRUCT:
	{
		BuildingTypes const eBuilding = (BuildingTypes)iData1; // advc
		if (canConstruct(eBuilding) || bForce)
		{
			GET_PLAYER(getOwner()).changeBuildingClassMaking(
					GC.getInfo(eBuilding).getBuildingClassType(), 1);
			bValid = true;
			bBuildingBuilding = true;
			CvEventReporter::getInstance().cityBuildingBuilding(this, eBuilding);
			if (gCityLogLevel >= 1) logBBAI("    City %S pushes production of building %S", getName().GetCString(), GC.getInfo(eBuilding).getDescription());
		}
		break;
	}
	case ORDER_CREATE:
	{
		ProjectTypes const eProject = (ProjectTypes)iData1; // advc
		if (canCreate(eProject) || bForce)
		{
			GET_TEAM(getTeam()).changeProjectMaking(eProject, 1);
			bValid = true;
			if (gCityLogLevel >= 1) logBBAI("    City %S pushes production of project %S", getName().GetCString(), GC.getInfo(eProject).getDescription());
		}
		break;
	}
	case ORDER_MAINTAIN:
	{
		ProcessTypes const eProcess = (ProcessTypes)iData1; // advc
		if (canMaintain(eProcess) || bForce)
		{
			bValid = true;
			/*	K-Mod. For culture processes, use iData2 to flag the current culture level
				so that we know when to stop. We could do a similar thing with
				research processes and tech... but let's not. */
			if (isHuman() && GC.getInfo(eProcess).
				getProductionToCommerceModifier(COMMERCE_CULTURE) > 0)
			{
				FAssert(iData2 == -1);
				iData2 = getCultureLevel();
			}
			// K-Mod end
			if (gCityLogLevel >= 1) logBBAI("    City %S pushes production of process %S", getName().GetCString(), GC.getInfo((ProcessTypes)iData1).getDescription());
		}
		break;
	}
	default:
		FErrorMsg("iOrder did not match a valid option");
	}

	if (!bValid)
		return;

	bool const bWasEmpty = (m_orderQueue.getLength() == 0); // advc.004x
	order.eOrderType = eOrder;
	order.iData1 = iData1;
	order.iData2 = iData2;
	order.bSave = bSave;

	/*if (bAppend)
		m_orderQueue.insertAtEnd(order);
	else {
		stopHeadOrder();
		m_orderQueue.insertAtBeginning(order);
	}
	if (!bAppend || (getOrderQueueLength() == 1))
		startHeadOrder();*/ // BtS
	// K-Mod
	if (iPosition == 0 || getOrderQueueLength() == 0)
	{
		stopHeadOrder();
		m_orderQueue.insertAtBeginning(order);
		startHeadOrder();
	}
	else if (iPosition < 0 || iPosition >= getOrderQueueLength())
		m_orderQueue.insertAtEnd(order);
	else m_orderQueue.insertBefore(order, m_orderQueue.nodeNum(iPosition));
	// K-Mod end

	// Why does this cause a crash??? 
	// <advc.test> Uncommented. Was added and commented out by the Warlords expansion.
	if (bBuildingUnit)
		CvEventReporter::getInstance().cityBuildingUnit(this, (UnitTypes)iData1);
	else if (bBuildingBuilding)
		CvEventReporter::getInstance().cityBuildingBuilding(this, (BuildingTypes)iData1);
	// </advc.test>
	if (isActiveTeam() || GC.getGame().isDebugMode())
	{
		setInfoDirty(true);
		if (isCitySelected())
		{
			gDLL->UI().setDirty(InfoPane_DIRTY_BIT, true);
			gDLL->UI().setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->UI().setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->UI().setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
	// <advc.004x>
	if (bWasEmpty && isActiveOwned())
		GET_PLAYER(getOwner()).killAll(BUTTONPOPUP_CHOOSEPRODUCTION, getID());
	// </advc.004x>
}


void CvCity::popOrder(int iNum, bool bFinish,
	ChooseProductionPlayers eChoose, // advc.064d (was bool bChoose)
	bool bEndOfTurn) // advc.001x
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	bool const bWasFoodProduction = isFoodProduction();

	if (iNum == -1)
		iNum = (getOrderQueueLength() - 1);

	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	{
		int iCount = 0;
		while (pOrderNode != NULL)
		{
			if (iCount == iNum)
				break;
			iCount++;
			pOrderNode = nextOrderQueueNode(pOrderNode);
		}
	}
	if (pOrderNode == NULL)
		return;

	if (bFinish && pOrderNode->m_data.bSave)
	{
		pushOrder(pOrderNode->m_data.eOrderType, pOrderNode->m_data.iData1,
				pOrderNode->m_data.iData2, true, false, /* K-Mod: */ -1);
	}

	UnitTypes eTrainUnit = NO_UNIT;
	BuildingTypes eConstructBuilding = NO_BUILDING;
	ProjectTypes eCreateProject = NO_PROJECT;
	int iMaxedBuildingOrProject = NO_BUILDING; // advc.123f

	OrderTypes eOrderType = pOrderNode->m_data.eOrderType;
	switch(eOrderType)
	{
	case ORDER_TRAIN:
	{
		eTrainUnit = (UnitTypes)pOrderNode->m_data.iData1;
		UnitAITypes eTrainAIUnit = (UnitAITypes)pOrderNode->m_data.iData2;
		FAssert(eTrainUnit != NO_UNIT);
		FAssert(eTrainAIUnit != NO_UNITAI);
		kOwner.changeUnitClassMaking(GC.getInfo(eTrainUnit).getUnitClassType(), -1);
		getArea().changeNumTrainAIUnits(getOwner(), eTrainAIUnit, -1);
		kOwner.AI_changeNumTrainAIUnits(eTrainAIUnit, -1);
		/*  <advc.113b> So that the new worker can already be taken into account
			for choosing the next order */
		if(eTrainAIUnit == UNITAI_WORKER)
			AI().AI_changeWorkersHave(1); // </advc.113b>
		doPopOrder(pOrderNode); // advc.064d (see case ORDER_CONSTRUCT)
		if(!bFinish)
			break;
		// <advc.064b> Moved into new function
		handleOverflow(getUnitProduction(eTrainUnit) - getProductionNeeded(eTrainUnit),
				getProductionModifier(eTrainUnit), eOrderType);
		// 14 March 2019: K-Mod multi-production code removed (including bugfix advc.001v)
		// Instead restored (two lines):
		setUnitProduction(eTrainUnit, 0);
		setUnitProductionTime(eTrainUnit, 0); // EmperorFool, Bugfix, 06/10/10
		// </advc.064b>
		CvUnit* pUnit = kOwner.initUnit(eTrainUnit, getX(), getY(), eTrainAIUnit);
		pUnit->finishMoves();
		addProductionExperience(pUnit);
		CvPlot* pRallyPlot = getRallyPlot(); // (advc.001b: moved up)
		if (GC.getInfo(eTrainUnit).getDomainType() == DOMAIN_AIR &&
			getPlot().countNumAirUnits(getTeam()) > getAirUnitCapacity(getTeam()))
		{
			// <advc.001b>
			if (pRallyPlot != NULL && pUnit->canMoveInto(*pRallyPlot))
				pUnit->move(*pRallyPlot, false, true);
			if (pUnit->at(*plot()))
			{
				pUnit->jumpToNearestValidPlot(); // (as in BtS)
				bool const bDead = pUnit->isDead();
				if (isActiveOwned())
				{
					CvWString szMsg(gDLL->getText("TXT_KEY_AIR_CAPACITY_EXCEEDED",
							pUnit->getNameKey(), getNameKey(), gDLL->getText(bDead ?
							"TXT_KEY_AIR_UNIT_SCRAPPED" : "TXT_KEY_AIR_UNIT_MOVED").GetCString()));
					gDLL->UI().addMessage(getOwner(), false, -1, szMsg, NULL, MESSAGE_TYPE_INFO,
							pUnit->getButton(), NO_COLOR,
							bDead ? getX() : pUnit->getX(), bDead ? getY() : pUnit->getY(), true, true);
				}
				if (bDead)
					break;
			}
			/*	The jump has already spent all moves. Spending all moves twice makes
				the unit unable to move on the next turn. Cf. CvUnit::doTurn. */
			pUnit->changeMoves(pUnit->maxMoves());
			// </advc.001b>
		}
		if (pRallyPlot != NULL/* advc.001b: */ && pUnit->at(getPlot()))
			pUnit->getGroup()->pushMission(MISSION_MOVE_TO, pRallyPlot->getX(), pRallyPlot->getY());
		if (kOwner.isHumanOption(PLAYEROPTION_START_AUTOMATED))
			pUnit->automate(AUTOMATE_BUILD);
		if (kOwner.isHumanOption(PLAYEROPTION_MISSIONARIES_AUTOMATED))
			pUnit->automate(AUTOMATE_RELIGION);
		CvEventReporter::getInstance().unitBuilt(this, pUnit);
		if (gCityLogLevel >= 1) // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
		{
			CvWString szString; getUnitAIString(szString, pUnit->AI_getUnitAIType());
			logBBAI("    City %S finishes production of unit %S with UNITAI %S", getName().GetCString(), pUnit->getName(0).GetCString(), szString.GetCString());
		}
		break;
	}
	case ORDER_CONSTRUCT:
	{
		eConstructBuilding = (BuildingTypes)pOrderNode->m_data.iData1;
		BuildingClassTypes eBuildingClass = GC.getInfo(eConstructBuilding).getBuildingClassType();
		kOwner.changeBuildingClassMaking(eBuildingClass, -1);
		CvBuildingClassInfo const& kConstructClass = GC.getInfo(eBuildingClass);
		/*  advc.064d: processBuilding may now call verifyProduction. Important that
			the constructed building is no longer in the queue at that point. */
		doPopOrder(pOrderNode);
		if(!bFinish)
			break;
		if (kOwner.isBuildingClassMaxedOut(eBuildingClass,
			// UNOFFICIAL_PATCH, Bugfix, 10/08/09, davidlallen & jdog5000:
			kConstructClass.getExtraPlayerInstances()))
		{
			kOwner.removeBuildingClass(eBuildingClass);
		}
		setNumRealBuilding(eConstructBuilding, getNumRealBuilding(eConstructBuilding) + 1,
				bEndOfTurn); // advc.001x
		// <advc.064b> Moved into new function
		handleOverflow(getBuildingProduction(eConstructBuilding) -
				getProductionNeeded(eConstructBuilding),
				getProductionModifier(eConstructBuilding), eOrderType);
		// </advc.064b>
		setBuildingProduction(eConstructBuilding, 0);
		setBuildingProductionTime(eConstructBuilding, 0); // Bugfix, 06/10/10, EmperorFool
		// <advc.123f>
		if (kConstructClass.isWorldWonder() &&
			GC.getGame().isBuildingClassMaxedOut(eBuildingClass))
		{
			iMaxedBuildingOrProject = eConstructBuilding;
		} // </advc.123f>
		CvEventReporter::getInstance().buildingBuilt(this, eConstructBuilding);
		if (gCityLogLevel >= 1) // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
			logBBAI("    City %S finishes production of building %S", getName().GetCString(), GC.getInfo(eConstructBuilding).getDescription());
		break;
	}
	case ORDER_CREATE:
	{
		eCreateProject = (ProjectTypes)pOrderNode->m_data.iData1;
		GET_TEAM(getTeam()).changeProjectMaking(eCreateProject, -1);
		doPopOrder(pOrderNode); // advc.064d
		if(!bFinish)
			break;
		/*	Event reported to Python before the project is built, so that we can
			show the movie before awarding free techs, for example */
		CvEventReporter::getInstance().projectBuilt(this, eCreateProject);
		GET_TEAM(getTeam()).changeProjectCount(eCreateProject, 1);
		if (GC.getInfo(eCreateProject).isSpaceship())
		{
			bool bNeedsArtType = true;
			VictoryTypes eVictory = GC.getInfo(eCreateProject).getVictoryPrereq();
			if (NO_VICTORY != eVictory && GET_TEAM(getTeam()).canLaunch(eVictory))
			{
				if(isHuman())
				{
					CvPopupInfo* pInfo = NULL;
					if (GC.getGame().isNetworkMultiPlayer())
					{
						pInfo = new CvPopupInfo(BUTTONPOPUP_LAUNCH,
								GC.getInfo(eCreateProject).getVictoryPrereq());
					}
					else
					{
						pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN, eCreateProject);
						pInfo->setText(L"showSpaceShip");
						bNeedsArtType = false;
					}
					gDLL->UI().addPopup(pInfo, getOwner());
				}
				else kOwner.AI_launch(eVictory);
			}
			else //show the spaceship progress
			{
				if (isHuman() &&
					BUGOption::isEnabled("TechWindow__ShowSSScreen", false)) // advc.060
				{
					if(!GC.getGame().isNetworkMultiPlayer())
					{
						CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN, eCreateProject);
						pInfo->setText(L"showSpaceShip");
						gDLL->UI().addPopup(pInfo, getOwner());
						bNeedsArtType = false;
					}
				}
			}
			if(bNeedsArtType)
			{
				int iDefaultArtType = GET_TEAM(getTeam()).getProjectDefaultArtType(eCreateProject);
				int iProjectCount = GET_TEAM(getTeam()).getProjectCount(eCreateProject);
				GET_TEAM(getTeam()).setProjectArtType(eCreateProject, iProjectCount - 1, iDefaultArtType);
			}
		}
		// <advc.064b> Moved into new function
		handleOverflow(getProjectProduction(eCreateProject) -
				getProductionNeeded(eCreateProject),
				getProductionModifier(eCreateProject), eOrderType);
		// </advc.064b>
		setProjectProduction(eCreateProject, 0);
		// <advc.123f>
		if(GC.getGame().isProjectMaxedOut(eCreateProject))
			iMaxedBuildingOrProject = eCreateProject; // </advc.123f>
		if (gCityLogLevel >= 1) logBBAI("    City %S finishes production of project %S", getName().GetCString(), GC.getInfo(eCreateProject).getDescription()); // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
		break;
	}
	case ORDER_MAINTAIN:
		doPopOrder(pOrderNode); // advc.064d
		break;

	default:
		FErrorMsg("Unknown production order type");
		doPopOrder(pOrderNode); // advc.064d
	}
	/*  advc.064d: (BtS code moved into auxiliary function doPopOrder; called
		from within the switch block)  */
	pOrderNode = NULL; // for safety
	// <advc.123f> Fail gold from great wonders and world projects
	if (iMaxedBuildingOrProject != NO_BUILDING)
	{
		bool bProject = (eOrderType == ORDER_CREATE);
		ProjectTypes eProject = (bProject ? (ProjectTypes)iMaxedBuildingOrProject : NO_PROJECT);
		BuildingTypes eBuilding = (bProject ? NO_BUILDING :
				(BuildingTypes)iMaxedBuildingOrProject);
		BuildingClassTypes eBuildingClass = (bProject ? NO_BUILDINGCLASS :
				(BuildingClassTypes)GC.getInfo(eBuilding).getBuildingClassType());
		for (PlayerIter<CIV_ALIVE> it; it.hasNext(); ++it) // Not: Barbarians
		{
			CvPlayer& kPlayer = *it;
			if (kPlayer.getID() == getOwner()) // No fail gold for the city owner
				continue;
			// Just for efficiency
			if (!bProject && kPlayer.getBuildingClassMaking(eBuildingClass) <= 0)
				continue;
			FOR_EACH_CITY_VAR(pCity, kPlayer)
			{
				// Fail gold only for queued orders
				/*  If a mod-mod allows e.g. a wonder with up to 2 instances
					and p is building 2 instances in parallel when this city
					finishes 1 instance, abort only 1 of p's instances:
					the one with less production invested. */
				CvCity* pMinProductionCity = NULL;
				int iMinProduction = -1; // 0 production can happen
				for (int j = 0; j < pCity->getOrderQueueLength(); j++)
				{
					OrderData* pOrder = pCity->getOrderFromQueue(j);
					if (pOrder == NULL || pOrder->eOrderType !=
						(bProject ? ORDER_CREATE : ORDER_CONSTRUCT) ||
						pOrder->iData1 != iMaxedBuildingOrProject)
					{
						continue;
					}
					int iProductionInvested = (bProject ?
							pCity->getProjectProduction(eProject) :
							pCity->getBuildingProduction(eBuilding));
					if(iProductionInvested > iMinProduction)
					{
						iMinProduction = iProductionInvested;
						pMinProductionCity = pCity;
					}
				}
				if (pMinProductionCity != NULL)
				{
					// 0 fail gold for teammates
					if(kPlayer.getTeam() == getTeam())
						iMinProduction = 0;
					/*  No fail gold for overflow
						(Tbd.: Add the overflow to OverflowProduction?) */
					iMinProduction = std::min(iMinProduction,
							bProject ? getProductionNeeded(eProject) :
							getProductionNeeded(eBuilding));
					pMinProductionCity->failProduction(iMaxedBuildingOrProject,
							iMinProduction, bProject);
				}
			}
		}
	} // </advc.123f>

	CvDLLInterfaceIFaceBase& kUI = gDLL->UI();
	if (isActiveTeam() || GC.getGame().isDebugMode())
	{
		setInfoDirty(true);
		if (isCitySelected())
		{
			kUI.setDirty(InfoPane_DIRTY_BIT, true);
			kUI.setDirty(SelectionButtons_DIRTY_BIT, true);
			kUI.setDirty(CityScreen_DIRTY_BIT, true);
		}
	}

	bool bMessage = false;
	if (getOrderQueueLength() == 0 &&
		// <advc.064d>
		(eChoose == ALL_CHOOSE ||
		(eChoose != NONE_CHOOSE && (eChoose == HUMAN_CHOOSE) == isHuman()))) // </advc.064d>
	{
		if (!isHuman() || isProductionAutomated())
			AI().AI_chooseProduction();
		else
		{
			if (bWasFoodProduction)
				AI().AI_assignWorkingPlots();

			chooseProduction(eTrainUnit, eConstructBuilding, eCreateProject, bFinish);
			// <advc.004x> Remember the order in case the popup needs to be delayed
			m_bMostRecentUnit = (eTrainUnit != NO_UNIT);
			if(m_bMostRecentUnit)
				m_iMostRecentOrder = eTrainUnit;
			else if(eCreateProject != NO_PROJECT)
				m_iMostRecentOrder = GC.getNumBuildingInfos() + eCreateProject;
			else m_iMostRecentOrder = eConstructBuilding; // </advc.004x>
			bMessage = true;
		}
	}
	if (bFinish && !bMessage)
	{
		wchar szBuffer[1024]; // (advc: why not CvWString?)
		TCHAR szSound[1024];
		LPCSTR szIcon = NULL;
		if (eTrainUnit != NO_UNIT)
		{
			swprintf(szBuffer, gDLL->getText(
					GC.getInfo(eTrainUnit).isLimited() ?
					"TXT_KEY_MISC_TRAINED_UNIT_IN_LIMITED" :
					"TXT_KEY_MISC_TRAINED_UNIT_IN",
					GC.getInfo(eTrainUnit).getTextKeyWide(),
					getNameKey()).GetCString());
			strcpy(szSound, GC.getInfo(eTrainUnit).getArtInfo(
					0, kOwner.getCurrentEra(), NO_UNIT_ARTSTYLE)->getTrainSound());
			szIcon = kOwner.getUnitButton(eTrainUnit);
		}
		else if (eConstructBuilding != NO_BUILDING)
		{
			swprintf(szBuffer, gDLL->getText(
					GC.getInfo(eConstructBuilding).isLimited() ?
					"TXT_KEY_MISC_CONSTRUCTED_BUILD_IN_LIMITED" :
					"TXT_KEY_MISC_CONSTRUCTED_BUILD_IN",
					GC.getInfo(eConstructBuilding).getTextKeyWide(),
					getNameKey()).GetCString());
			strcpy(szSound, GC.getInfo(eConstructBuilding).getConstructSound());
			szIcon = GC.getInfo(eConstructBuilding).getButton();
		}
		else if (eCreateProject != NO_PROJECT)
		{
			swprintf(szBuffer, gDLL->getText(
					GC.getInfo(eCreateProject).isLimited() ?
					// <advc.008e>
					(GC.getInfo(eCreateProject).nameNeedsArticle() ?
					"TXT_KEY_MISC_CREATED_PROJECT_IN_LIMITED_THE" :
					"TXT_KEY_MISC_CREATED_PROJECT_IN_LIMITED") // </advc.008e>
					: "TXT_KEY_MISC_CREATED_PROJECT_IN",
					GC.getInfo(eCreateProject).getTextKeyWide(),
					getNameKey()).GetCString());
			strcpy(szSound, GC.getInfo(eCreateProject).getCreateSound());
			szIcon = GC.getInfo(eCreateProject).getButton();
		}
		if (isProduction())
		{
			wchar szTempBuffer[1024];
			swprintf(szTempBuffer, gDLL->getText(isProductionLimited() ?
					"TXT_KEY_MISC_WORK_HAS_BEGUN_LIMITED" :
					"TXT_KEY_MISC_WORK_HAS_BEGUN",
					getProductionNameKey()).GetCString());
			wcscat(szBuffer, szTempBuffer);
		}
		kUI.addMessage(getOwner(), false, -1, szBuffer,
				getPlot(), szSound, MESSAGE_TYPE_INFO, // advc.106b: was MINOR_EVENT
				szIcon);
	}

	if (isActiveTeam() || GC.getGame().isDebugMode())
	{
		setInfoDirty(true);
		if (isCitySelected())
		{
			kUI.setDirty(InfoPane_DIRTY_BIT, true);
			kUI.setDirty(SelectionButtons_DIRTY_BIT, true);
			kUI.setDirty(CityScreen_DIRTY_BIT, true);
			kUI.setDirty(PlotListButtons_DIRTY_BIT, true);
		}
	}
}


void CvCity::startHeadOrder()
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_MAINTAIN)
			processProcess((ProcessTypes)pOrderNode->m_data.iData1, 1);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::stopHeadOrder()
{
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	if (pOrderNode != NULL)
	{
		if (pOrderNode->m_data.eOrderType == ORDER_MAINTAIN)
			processProcess((ProcessTypes)pOrderNode->m_data.iData1, -1);
	}
}


OrderData* CvCity::getOrderFromQueue(int iIndex) /* advc: */ const
{
	CLLNode<OrderData>* pOrderNode = m_orderQueue.nodeNum(iIndex);
	if (pOrderNode != NULL)
		return &pOrderNode->m_data;
	return NULL;
}


OrderData CvCity::getOrderData(int iIndex) const
{
	int iCount = 0;
	CLLNode<OrderData>* pNode = headOrderQueueNode();
	while (pNode != NULL)
	{
		if (iIndex == iCount)
			return pNode->m_data;
		iCount++;
		pNode = nextOrderQueueNode(pNode);
	}
	OrderData kData;
	kData.eOrderType = NO_ORDER;
	kData.iData1 = -1;
	kData.iData2 = -1;
	kData.bSave = false;
	return kData;
}


void CvCity::setWallOverridePoints(const std::vector< std::pair<float, float> >& kPoints)
{
	m_kWallOverridePoints = kPoints;
	setLayoutDirty(true);
}

// <advc.310>
void CvCity::addGreatWall(int iAttempt)
{
	if (GC.getDefineINT("GREAT_WALL_GRAPHIC_MODE") != 1)
	{
		gDLL->getEngineIFace()->AddGreatWall(this);
		return;
	}
	// All plots orthogonally adjacent to a (desired) wall segment
	std::set<int> aiWallPlots;
	/*	To determine if we have enough segments in the end.
		But I think it's better to place too few segments than
		to relax the placement rules arbitrarily ... */
	/*int iInside = 0;
	int iOutside = 0;*/
	CvMap& kMap = GC.getMap();
	for (int i = 0; i < kMap.numPlots(); i++)
	{
		CvPlot const& kInside = kMap.getPlotByIndex(i);
		if(!isArea(kInside.getArea()) || kInside.getOwner() != getOwner() || // as in BtS
			kInside.isImpassable()) // new: don't wall off peaks
		{
			continue;
		}
		/*	Add kInside if we find an adjacent pOutside such that the two should
			be separated by a segment */
		bool bFound = false;
		FOR_EACH_ORTH_ADJ_PLOT2(pOutside, kInside)
		{
			bool bValid = needsGreatWallSegment(kInside, *pOutside, iAttempt);
			if (!bValid && pOutside->sameArea(kInside) && !pOutside->isImpassable()
				/*	Commenting this condition out extends the wall by 1 tile into
					unhabitable terrain. Otherwise, it looks like Barbarians could just
					slip around the margins. */
				/*&& pOutside->isHabitable()*/)
			{
				/*	Also try the two plots that are orthogonally adjacent to pOutside
					and diagonally adjacent to kInside. The segment will still go between
					kInside and pOutside b/c there are no diagonal segments.
					(But never if pOutside is water or impassable; see checks above.) */
				FOR_EACH_ORTH_ADJ_PLOT(*pOutside)
				{
					if (stepDistance(pAdj, &kInside) == 1 &&
						needsGreatWallSegment(kInside, *pAdj, iAttempt))
					{
						bValid = true;
						break;
					}
				}
			}
			if (bValid)
			{
				aiWallPlots.insert(pOutside->plotNum());
				bFound = true;
				//iOutside++;
			}
		}
		if(bFound)
		{
			aiWallPlots.insert(kInside.plotNum());
			//iInside++;
		}
	}
	/*if (iAttempt == 0 && std::min(iInside, iOutside) <= 2)
	{
		addGreatWall(iAttempt + 1);
		return;
	}*/
	/*  Hack: Use a dummy CvArea object to prevent CvEngine from placing segments
		along plots not in aiWallPlots. */
	CvArea* pTmpArea = kMap.addArea();
	pTmpArea->init(false);
	/*  The city plot needs to be in the area as well b/c CvEngine will consider
		only tiles in the same CvArea as the Great Wall city */
	aiWallPlots.insert(plotNum());
	for(std::set<int>::iterator it = aiWallPlots.begin(); it != aiWallPlots.end(); ++it)
		kMap.plotByIndex(*it)->setArea(pTmpArea, /*bProcess=*/false);
	gDLL->getEngineIFace()->AddGreatWall(this);
	// Restore actual area
	for(std::set<int>::iterator it = aiWallPlots.begin(); it != aiWallPlots.end(); ++it)
		kMap.plotByIndex(*it)->setArea(area(), false);
	kMap.deleteArea(pTmpArea->getID());
}

// Helper function for placing Great Wall segments
bool CvCity::needsGreatWallSegment(/* not currently used: */CvPlot const& kInside,
	CvPlot const& kOutside, int iAttempt) const
{
	if(!isArea(kOutside.getArea()) || kOutside.isImpassable() ||
		!kOutside.isHabitable(true))
	{
		return false;
	}
	PlayerTypes eOutsideOwner = kOutside.getOwner();
	if(eOutsideOwner != NO_PLAYER && eOutsideOwner != BARBARIAN_PLAYER) // Not: any civ
		return false;
	if (iAttempt > 0)
		return true;
	/*	To avoid creating a Gaza strip, require a third plot that must be habitable,
		adjacent to the outside plot but not adjacent to our territory. */
	FOR_EACH_ADJ_PLOT2(pThird, kOutside)
	{
		if(!isArea(pThird->getArea()) ||
			pThird->isImpassable() || !pThird->isHabitable(true))
		{
			continue;
		}
		PlayerTypes eThirdOwner = pThird->getOwner();
		if((eThirdOwner == NO_PLAYER || eThirdOwner == BARBARIAN_PLAYER) &&
			!pThird->isAdjacentPlayer(getOwner(), true))
		{
			return true;
		}
	}
	return false;
} // </advc.310>


const std::vector< std::pair<float, float> >& CvCity::getWallOverridePoints() const
{
	return m_kWallOverridePoints;
}


void CvCity::doGrowth()
{
	if (GC.getPythonCaller()->doGrowth(*this))
		return;
/* Population Limit ModComp - Beginning : The population level can't grow if the limit is reached */
	if (getPopulation() >= getPopulationLimit() && foodDifference() >= 0)
	{
		return;
	}
/* Population Limit ModComp - End */

	int iDiff = foodDifference();
	changeFood(iDiff);
	if(iDiff > 0) // advc.160: Don't empty the Granary when insufficient food
		changeFoodKept(iDiff);

	setFoodKept(range(getFoodKept(), 0, (growthThreshold() * getMaxFoodKeptPercent()) / 100));

	if (getFood() >= growthThreshold())
	{
		if (AI().AI_isEmphasizeAvoidGrowth())
			setFood(growthThreshold());
		else
		{
			changeFood(-std::max(0, growthThreshold() - getFoodKept()));
			changePopulation(1);
			CvEventReporter::getInstance().cityGrowth(this, getOwner());
		}
	}
	else if (getFood() < 0)
	{
		changeFood(-getFood());
		if (getPopulation() > 1)
		{
			changePopulation(-1);
			// <advc.160>
			changeFood(getFoodKept());
			setFoodKept(0); // </advc.160>
		}
	}
}


void CvCity::doCulture()
{
	if (GC.getPythonCaller()->doCulture(*this))
		return;
	// <advc.099b>
	if (isOccupation())
		return; // </advc.099b>
	// <advc.125>
	static int const iUseKModTradeCulture = GC.getDefineINT("USE_KMOD_TRADE_CULTURE");
	if (iUseKModTradeCulture != 0 && // </advc.125>
		// K-Mod, 26/sep/10, 31/oct/10 - Trade culture: START
		getCultureLevel() > 0 /* advc.098: */ && !isDisorder())
	{
		/*	add up the culture contribution for each player before applying it
			so that we avoid excessive calls to change culture and reduce rounding errors */
		EagerEnumMap<PlayerTypes,int> aiTradeCultureTimes100;
		for (int i = 0; i < GC.getDefineINT(CvGlobals::MAX_TRADE_ROUTES); i++)
		{
			CvCity* pLoopCity = getTradeCity(i);
			if(pLoopCity != NULL)
			{	// foreign and domestic
				//if (pLoopCity->getOwner() != getOwner())
				aiTradeCultureTimes100.add(pLoopCity->getOwner(),
						pLoopCity->getTradeCultureRateTimes100());
			}
		}
		for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it) // advc.003n: MAJOR
		{
			PlayerTypes ePlayer = it->getID();
			if (aiTradeCultureTimes100.get(ePlayer) > 0)
			{
				// <advc.125>
				if(iUseKModTradeCulture < 0 && getPlot().getCulture(ePlayer) <= 0)
					continue; // </advc.125>
				// plot culture only.
				//changeCultureTimes100(ePlayer, aiTradeCultureTimes100.get(ePlayer), false, false);
				doPlotCultureTimes100(false, ePlayer, aiTradeCultureTimes100.get(ePlayer), false);
			}
		}
		// K-Mod END
	}
	changeCultureTimes100(getOwner(), getCommerceRateTimes100(COMMERCE_CULTURE), false, true);
//KNOEDELbegin cultural_golden_age 1/2
	if (GC.getGame().isOption(GAMEOPTION_CULTURE_GOLDEN_AGE)) 
	{
		GET_PLAYER(getOwner()).changeCultureGoldenAgeProgress(getCommerceRate(COMMERCE_CULTURE));
	}
//KNOEDELend oy vey truth be told I have no idea what to do about this here
}

/*	This function has essentially been rewritten for K-Mod.
	(and it used to not be 'times 100') */
void CvCity::doPlotCultureTimes100(bool bUpdate, PlayerTypes ePlayer,
	int iCultureRateTimes100, bool bCityCulture)
{
	PROFILE_FUNC(); // advc: Let's keep an eye on this
	if (GC.getPythonCaller()->doPlotCultureTimes100(*this, ePlayer, bUpdate, iCultureRateTimes100))
		return;
	CultureLevelTypes const eCultureLevel = getCultureLevel(ePlayer);

	/*  K-Mod, 30/oct/10: start
		increased culture range, added a percentage based distance bonus
		(decreasing the importance of the flat rate bonus).
	Experimental culture profile ...
	Ae^(-bx). A = 10 (no effect), b = log(full_range_ratio)/range */
	//const int iOuterRatio = 10;
	//const double iB = log((double)iOuterRatio)/iCultureRange;
	/*	(iScale-1)(iDistance - iRange)^2/(iRange^2) + 1
		This approximates the exponential pretty well */
	int const iScale = plotCultureScale();
	int const iCultureRange = eCultureLevel + plotCultureExtraRange();

	if (bCityCulture)
	{
		FAssertMsg(iCultureRateTimes100 >= 0, "dubious");
		// <advc.098> Putting this BtS define back to use (with new semantics)
		static int const iCITY_FREE_CULTURE_GROWTH_FACTOR = GC.getDefineINT("CITY_FREE_CULTURE_GROWTH_FACTOR");
		iCultureRateTimes100 += iCITY_FREE_CULTURE_GROWTH_FACTOR * 100; // </advc.098>
	}

	if (eCultureLevel == NO_CULTURELEVEL ||
		(bCityCulture && isDisorder()) || // advc.098
		//getCultureTimes100(ePlayer) <= 0
		abs(iCultureRateTimes100 * iScale) < 100)
	{
		return;
	}
	// <advc.025>
	scaled rCultureToMaster = 1;
	if (GET_TEAM(getTeam()).isCapitulated())
	{
		static scaled const rCAPITULATED_TO_MASTER_CULTURE_PERCENT = per100(
				GC.getDefineINT("CAPITULATED_TO_MASTER_CULTURE_PERCENT"));
		rCultureToMaster = rCAPITULATED_TO_MASTER_CULTURE_PERCENT;
	} // </advc.025>
	for (SquareIter itPlot(getPlot(), iCultureRange); itPlot.hasNext(); ++itPlot)
	{
		int iDistance = cultureDistance(itPlot.currXDist(), itPlot.currYDist());
		if(iDistance > iCultureRange)
			continue;
		CvPlot& p = *itPlot;
		if ((p.isWater() && // advc.098
			!p.isPotentialCityWorkForArea(getArea())) ||
			(!isArea(p.getArea()) && iDistance > eCultureLevel + 1)) // advc.098
		{
			continue;
		}
		// <advc.098>
		if (iDistance > eCultureLevel && !p.isOwned())
			continue; // </advc.098>
		// BtS formula for culture to add (the 20 was "CITY_FREE_CULTURE_GROWTH_FACTOR"):
		//eCultureLevel < iDistance ? 0 : (iCultureRateTimes100/100 + 1 + (eCultureLevel - iDistance) * 20;
		// K-Mod 1.45 (older K-Mod code deleted):
		//int iCultureToAdd = iCultureRateTimes100*((iScale-1)*(iDistance-iCultureRange)*(iDistance-iCultureRange) + iCultureRange*iCultureRange)/(100*iCultureRange*iCultureRange);
		/*  <advc.001> Secure this against overflow from culture added through WB.
			(K-Mod 1.46 does so too - by using floating-point math.) */
		scaled rDistFactor(SQR(iCultureRange - iDistance), SQR(iCultureRange));
		/*	Observation (rephrasing a deleted K-Mod comment):
			If iDistance is 0, then we add the culture rate times iScale.
			(However, culture distance is always at least 1 - even for the city plot.)
			If iDistance equals iCultureRange, then we add the culture rate (times 1). */
		scaled rCultureToAdd = per100(iCultureRateTimes100) * (1 + rDistFactor * (iScale - 1));
		// </advc.001>  <advc.025>
		if (p.getTeam() != getTeam() &&
			p.getTeam() == GET_TEAM(getTeam()).getMasterTeam())
		{
			rCultureToAdd *= rCultureToMaster;
		} // </advc.025>
		// <kekm.23> Loss of tile culture upon city trade
		if (rCultureToAdd.isNegative())
			rCultureToAdd.increaseTo(-p.getCulture(ePlayer)); // </kekm.23>
		p.changeCulture(ePlayer, rCultureToAdd.floor(), bUpdate || !p.isOwned());
	} // K-Mod end
}


bool CvCity::doCheckProduction()
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	CvCivilization const& kCiv = getCivilization();

	if (isAnyProductionProgress(ORDER_TRAIN)) // advc.opt
	{
		for (int i = 0; i < kCiv.getNumUnits(); i++)
		{
			UnitTypes eUnit = kCiv.unitAt(i);
			if (getUnitProduction(eUnit) <= 0)
				continue;
			if (kOwner.isProductionMaxedUnitClass(kCiv.unitClass(eUnit)))
			{	// advc.123f: Commented out (fail gold from national units)
				/*int iProductionGold = ((getUnitProduction(eUnit) * GC.getDefineINT("MAXED_UNIT_GOLD_PERCENT")) / 100);
				if (iProductionGold > 0) {
					kOwner.changeGold(iProductionGold);
					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED", getNameKey(), GC.getInfo(eUnit).getTextKeyWide(), iProductionGold);
					gDLL->UI().addMessage(getOwner(), false, -1, szBuffer, getPlot(), "AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getInfo(COMMERCE_GOLD).getButton(), GC.getColorType("RED"));
				}*/
				setUnitProduction(eUnit, 0);
			}
		}
	}
	if (isAnyProductionProgress(ORDER_CONSTRUCT)) // advc.opt
	{
		for (int i = 0; i < kCiv.getNumBuildings(); i++)
		{
			BuildingTypes eBuilding = kCiv.buildingAt(i);
			if (getBuildingProduction(eBuilding) <= 0)
				continue;
			if (kOwner.isProductionMaxedBuildingClass(kCiv.buildingClass(eBuilding)))
			{	// advc.123f: Commented out. Fail gold now handled in popOrder.
				/*iProductionGold = ((getBuildingProduction(eBuilding) * GC.getDefineINT("MAXED_BUILDING_GOLD_PERCENT")) / 100);
				if(iProductionGold > 0) {
					kOwner.changeGold(iProductionGold);
					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED", getNameKey(), GC.getInfo(eBuilding).getTextKeyWide(), iProductionGold);
					gDLL->UI().addMessage(getOwner(), false, -1, szBuffer, getPlot(), "AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getInfo(COMMERCE_GOLD).getButton(), GC.getColorType("RED"));
				}*/
				setBuildingProduction(eBuilding, 0);
			}
		}
	}
	if (isAnyProductionProgress(ORDER_CREATE)) // advc.opt
	{
		FOR_EACH_ENUM(Project)
		{
			if (getProjectProduction(eLoopProject) <= 0)
				continue;
			if (kOwner.isProductionMaxedProject(eLoopProject))
			{	// advc.123f: Commented out. Fail gold now handled in popOrder.
				/*iProductionGold = ((getProjectProduction(eLoopProject) * GC.getDefineINT("MAXED_BUILDING_GOLD_PERCENT")) / 100);
				if(iProductionGold > 0) {
					kOwner.changeGold(iProductionGold);
					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED", getNameKey(), GC.getInfo(eLoopProject).getTextKeyWide(), iProductionGold);
					gDLL->UI().addMessage(getOwner(), false, -1, szBuffer, getPlot(), "AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getInfo(COMMERCE_GOLD).getButton(), GC.getColorType("RED"));
				}*/
				setProjectProduction(eLoopProject, 0);
			}
		}
	}
	if (!isProduction() && !isDisorder() && isHuman() && !isProductionAutomated())
	{
		chooseProduction();
		return true;
	}
	// <advc.064d> Moved into new functions
	upgradeProduction(); 
	return checkCanContinueProduction(false); // </advc.064d>
}

// advc.064d: Cut from doCheckProduction
void CvCity::upgradeProduction()
{
	CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumUnits(); i++)
	{
		UnitTypes eUnit = kCiv.unitAt(i);
		if (getFirstUnitOrder(eUnit) == -1)
			continue;

		UnitTypes eUpgradeUnit = allUpgradesAvailable(eUnit);
		if (eUpgradeUnit == NO_UNIT)
			continue;
		FAssert(eUpgradeUnit != eUnit);

		int iUpgradeProduction = getUnitProduction(eUnit);
		setUnitProduction(eUnit, 0);
		setUnitProduction(eUpgradeUnit, iUpgradeProduction);

		CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
		while (pOrderNode != NULL)
		{
			if (pOrderNode->m_data.eOrderType == ORDER_TRAIN &&
				pOrderNode->m_data.iData1 == eUnit)
			{
				kOwner.changeUnitClassMaking(GC.getInfo((UnitTypes)
						pOrderNode->m_data.iData1).getUnitClassType(), -1);
				pOrderNode->m_data.iData1 = eUpgradeUnit;
				if (kOwner.AI_unitValue(eUpgradeUnit, (UnitAITypes)pOrderNode->m_data.iData2, area()) == 0)
				{
					getArea().changeNumTrainAIUnits(getOwner(), (UnitAITypes)pOrderNode->m_data.iData2, -1);
					kOwner.AI_changeNumTrainAIUnits(((UnitAITypes)pOrderNode->m_data.iData2), -1);
					pOrderNode->m_data.iData2 = GC.getInfo(eUpgradeUnit).getDefaultUnitAIType();
					getArea().changeNumTrainAIUnits(getOwner(), (UnitAITypes)pOrderNode->m_data.iData2, 1);
					kOwner.AI_changeNumTrainAIUnits((UnitAITypes)pOrderNode->m_data.iData2, 1);
				}
				// advc (note): pOrderNode may have changed
				kOwner.changeUnitClassMaking(GC.getInfo((UnitTypes)
						pOrderNode->m_data.iData1).getUnitClassType(), 1);
			}
			pOrderNode = nextOrderQueueNode(pOrderNode);
		}
	}
}

// advc.064d: Cut from doCheckProduction
bool CvCity::checkCanContinueProduction(bool bCheckUpgrade,
	ChooseProductionPlayers eChoose)
{
	bool bAllContinued = true;
	for (int i = getOrderQueueLength() - 1; i >= 0; i--)
	{
		OrderData* pOrder = getOrderFromQueue(i);
		if (pOrder != NULL && !canContinueProduction(*pOrder))
		{
			if (bCheckUpgrade)
			{
				upgradeProduction();
				return checkCanContinueProduction(false, eChoose);
			}
			popOrder(i, false, eChoose);
			bAllContinued = false;
		}
	}  // <advc.004x> Relaunch choose-production popups
	if (!bAllContinued)
		GET_PLAYER(getOwner()).killAll(NO_BUTTONPOPUP); // </advc.004x>
	return bAllContinued;
}


void CvCity::doProduction(bool bAllowNoProduction)
{
	if (GC.getPythonCaller()->doProduction(*this))
		return;

	if (!isHuman() || isProductionAutomated())
	{
		if (!isProduction() || isProductionProcess() ||
			isChooseProductionDirty())
		{
			AI().AI_chooseProduction();
		}
	}

	if (!bAllowNoProduction && !isProduction())
		return;

	if (isProductionProcess())
	{
		// K-Mod. End the culture process if our borders have expanded.
		// (This function is called after "doResearch" etc.)
		OrderData const& kOrder = headOrderQueueNode()->m_data;
		if (kOrder.iData2 > 0 && GC.getInfo((ProcessTypes)kOrder.iData1).
			getProductionToCommerceModifier(COMMERCE_CULTURE) > 0 &&
			getCultureLevel() > kOrder.iData2)
		{
			popOrder(0, false, ALL_CHOOSE);
		}
		// K-Mod end
		return;
	}

	if (isDisorder())
		return;

	if (isProduction())
	{
		int iFeatureProductionUsed=0; // advc.064b
		changeProduction(getCurrentProductionDifference(false, true, false,
				false, false, &iFeatureProductionUsed)); // advc.064b
		setOverflowProduction(0);
		//setFeatureProduction(0);
		changeFeatureProduction(-iFeatureProductionUsed); // advc.064b

		if (getProduction() >= getProductionNeeded())
			popOrder(0, true, ALL_CHOOSE);
	}
	else
	{
		changeOverflowProduction(getCurrentProductionDifference(false, false,
				/* advc.064b: */ true), getProductionModifier());
	}
}


void CvCity::doDecay()
{
	FOR_EACH_ENUM(Building)
	{
		if (getProductionBuilding() == eLoopBuilding)
			continue;

//DOTO -tholish-Keldath inactive buildings START
		if (GC.getInfo(eLoopBuilding).getPrereqMustAll() > 0 &&
				getNumRealBuilding(eLoopBuilding) > 0 && 
					GC.getGame().isOption(GAMEOPTION_BUILDING_DELETION) &&
					eLoopBuilding != NULL && eLoopBuilding != 0	//exclude the first building - palace
		)
		{
			//keldath extended building inactive
			//if the prereq of a building is no more - it will stop providing.
			//works for bonux for now
			//added fix by f1 from advc thank you so much!- keldath
			bool keepit = canKeep(eLoopBuilding);
			bool buildingStatus = m_aiBuildingeActive.get(eLoopBuilding);
			if (!keepit && buildingStatus)
			{
				m_aiBuildingeActive.set(eLoopBuilding, false);
				processBuilding(eLoopBuilding, 1, false, keepit);
			}
			else if (keepit && !buildingStatus)
			{
				m_aiBuildingeActive.set(eLoopBuilding,true);
				processBuilding(eLoopBuilding, 1, false, keepit);
			}
		}
//DOTO -tholish-Keldath inactive buildings END

		if (getBuildingProduction(eLoopBuilding) > 0)
		{
			changeBuildingProductionTime(eLoopBuilding, 1);
			if (isHuman())
			{
				int const iGameSpeedPercent = GC.getInfo(GC.getGame().
						getGameSpeedType()). getConstructPercent();
				if (100 * getBuildingProductionTime(eLoopBuilding) >
					GC.getDefineINT(CvGlobals::BUILDING_PRODUCTION_DECAY_TIME) * iGameSpeedPercent)
				{
					int iProduction = getBuildingProduction(eLoopBuilding);
					int const iDecayPercent = GC.getDefineINT(CvGlobals::BUILDING_PRODUCTION_DECAY_PERCENT);
					setBuildingProduction(eLoopBuilding, iProduction -
							(iProduction * (100 - iDecayPercent) + iGameSpeedPercent - 1) /
							iGameSpeedPercent);
				}
			}
		}
		else setBuildingProductionTime(eLoopBuilding, 0);
	}
	FOR_EACH_ENUM(Unit)
	{
		if (getProductionUnit() == eLoopUnit)
			continue;
		if (getUnitProduction(eLoopUnit) > 0)
		{
			changeUnitProductionTime(eLoopUnit, 1);
			if (isHuman())
			{
				int const iGameSpeedPercent = GC.getInfo(GC.getGame().
						getGameSpeedType()).getTrainPercent();
				if (100 * getUnitProductionTime(eLoopUnit) >
					GC.getDefineINT(CvGlobals::UNIT_PRODUCTION_DECAY_TIME) * iGameSpeedPercent)
				{
					int iProduction = getUnitProduction(eLoopUnit);
					int const iDecayPercent = GC.getDefineINT(CvGlobals::UNIT_PRODUCTION_DECAY_PERCENT);
					setUnitProduction(eLoopUnit, iProduction -
							(iProduction * (100 - iDecayPercent) + iGameSpeedPercent - 1) /
							iGameSpeedPercent);
				}
			}
		}
		else setUnitProductionTime(eLoopUnit, 0);
	}
}

// K-Mod. I've completely rewritten this function, and deleted the old code.
void CvCity::doReligion()
{
	if (GC.getPythonCaller()->doReligion(*this))
		return;

	// gives some of the top religions a shot at spreading to the city.
	int iChances = 1 - getReligionCount() + (getCultureLevel() >= 4 ? 1 : 0) +
			(getPopulation() + 3) / 8;
	// (breakpoints at pop = 5, 13, 21, ...)

	if (iChances <= 0)
		return;

	std::vector<std::pair<int, ReligionTypes> > religion_grips;
	// weakest religion already in the city
	ReligionTypes eWeakestReligion = NO_RELIGION;
	int iWeakestGrip = MAX_INT;
	static int const iRandomWeight = GC.getDefineINT("RELIGION_INFLUENCE_RANDOM_WEIGHT");
	static int const iDivisorBase = GC.getDefineINT("RELIGION_SPREAD_DIVISOR_BASE");
	static int const iDistanceFactor = GC.getDefineINT("RELIGION_SPREAD_DISTANCE_FACTOR");

	FOR_EACH_ENUM(Religion)
	{
		if (!GC.getGame().isReligionFounded(eLoopReligion))
			continue;

		if (isHasReligion(eLoopReligion))
		{
			// if we have this religion; check to see if it weakest one we have...
			if (!isHolyCity(eLoopReligion))
			{
				// ... only if it isn't the holy city.
				int iGrip = getReligionGrip(eLoopReligion);
				if (iGrip < iWeakestGrip)
				{
					iWeakestGrip = iGrip;
					eWeakestReligion = eLoopReligion;
				}
			}
		}
		else if (!GET_PLAYER(getOwner()).isNoNonStateReligionSpread() ||
			GET_PLAYER(getOwner()).getStateReligion() == eLoopReligion)
		{
			/*	if we don't have the religion, and the religion
				is allowed to spread here, add it to the list. */
			int iGrip = getReligionGrip(eLoopReligion);
			// only half the weight for self-spread
			iGrip += SyncRandNum(iRandomWeight / 2);
			religion_grips.push_back(std::make_pair(iGrip, eLoopReligion));
		}
	}

	iChances = std::min(iChances, (int)religion_grips.size());
	std::partial_sort(religion_grips.begin(), religion_grips.begin() + iChances,
			religion_grips.end(), std::greater<std::pair<int,ReligionTypes> >());

	for (int i = 0; i < iChances; i++)
	{
		int iLoopGrip = religion_grips[i].first;
		ReligionTypes eLoopReligion = religion_grips[i].second;

		// give up if there is already a stronger religion in the city.
		if (eWeakestReligion != NO_RELIGION && iWeakestGrip >= iLoopGrip)
			break;

		FAssert(eLoopReligion != NO_RELIGION);
		FAssert(!isHasReligion(eLoopReligion));
		FAssert(!GET_PLAYER(getOwner()).isNoNonStateReligionSpread() ||
				GET_PLAYER(getOwner()).getStateReligion() == eLoopReligion);

		int iMaxChance = 0;
		for (PlayerIter<ALIVE,KNOWN_TO> itPlayer(getTeam());
			itPlayer.hasNext(); ++itPlayer)
		{
			FOR_EACH_CITY(pLoopCity, *itPlayer)
			{
				if (!pLoopCity->isConnectedTo(*this))
					continue;

				int iSpread = pLoopCity->getReligionInfluence(eLoopReligion);
				iSpread *= GC.getInfo(eLoopReligion).getSpreadFactor();
				if (iSpread > 0)
				{
					/*iSpread /= std::max(1, (GC.getDefineINT("RELIGION_SPREAD_DISTANCE_DIVISOR", 100) *
							plotDistance(plot(), pLoopCity->plot())) / GC.getMap().maxPlotDistance() - 5);*/
					/*	K-Mod. The original formula basically divided the spread
						by the percent of max distance. In my view, this produced
						too much spread at short distance and too little at long. */
					int iDivisor = std::max(1, iDivisorBase);
					iDivisor *= 100 + (100 * iDistanceFactor *
							plotDistance(plot(), pLoopCity->plot())) /
							GC.getMap().maxTypicalDistance();  // advc.140: was maxPlotDistance
					iDivisor /= 125; // advc.140: was 100

					// now iDivisor is in the range [1, 1+iDistanceFactor] * iDivisorBase
					/*	this is approximately in the range [5, 50], depending on what the xml value are.
						(the value currently being tested and tuned.) */
					iSpread /= iDivisor;
					// K-Mod end
					//iSpread /= getReligionCount() + 1; // (already commented out in original SDK)
					iMaxChance = std::max(iMaxChance, iSpread);
				}
			}
		}
		// <advc.173>
		scaled rSpreadProb = per100(iMaxChance);
		if (GET_PLAYER(getOwner()).getHasReligionCount(eLoopReligion) <= 0)
		{	/*	Make first spread of a religion to a player less dependent
				on the number of cities owned by that player */
			int iTargetCities = GC.getInfo(GC.getMap().getWorldSize()).getTargetNumCities();
			int iCities = GET_PLAYER(getOwner()).getNumCities();
			if (iCities < iTargetCities) // Few cities: increase spread prob
				rSpreadProb *= 2 - scaled(iCities, iTargetCities);
			if (iCities > iTargetCities) // Many cities: decrease spread prob
				rSpreadProb *= scaled(iTargetCities, iCities);
		} // </advc.173>
		/*	advc.173 (note): Missionaries only get slowed down by getTrainPercent().
			But I don't think we want to double down on quicker proselytization
			by making natural spread faster as well. Units having a bigger role
			is normal for Marathon. */
		rSpreadProb /= GC.getGame().gameSpeedMultiplier();
		// K-Mod. Give a bonus for the first few cities?
		/*int iReligionCities = GC.getGame().countReligionLevels(eLoopReligion);
		if (iReligionCities < 3) {
			iChancePercent *= 2 + iReligionCities;
			iChancePercent /= 1 + iReligionCities;
		}*/
		// <advc> (No functional change)
		static scaled const rRELIGION_SPREAD_DIV = std::max(scaled::epsilon(),
				per100(GC.getDefineINT("RELIGION_SPREAD_RAND")));
		rSpreadProb /= rRELIGION_SPREAD_DIV;
		if (SyncRandSuccess(rSpreadProb))
		{	// </advc>
			setHasReligion(eLoopReligion, true, true, true);
			if (iWeakestGrip < iLoopGrip)
			{
				FAssert(eWeakestReligion != NO_RELIGION);
				/*	If the existing religion is weak compared to the new religion,
					the existing religion can get removed. */
				int iOdds = getReligionCount() * 100 * (iLoopGrip - iWeakestGrip) /
						std::max(1, iLoopGrip);
				if (SyncRandSuccess100(iOdds))
				{
					setHasReligion(eWeakestReligion, false, true, true);
					break; // end the loop
				}
			}
			else
			{
				iWeakestGrip = iLoopGrip;
				eWeakestReligion = eLoopReligion;
			}
			iChances--;
		}
	}
}


void CvCity::doGreatPeople()
{
	if (GC.getPythonCaller()->doGreatPeople(*this))
		return;

	if (isDisorder())
		return;

	CvCivilization const& kCiv = getCivilization();

	changeGreatPeopleProgress(getGreatPeopleRate());
	// <advc> Verify that GreatPeopleRate is the sum of the GreatPeopleUnitRates
	#ifdef FASSERT_ENABLE
		int iTotalUnitRate = 0;
	#endif // </advc>
	/*	(advc.001c: Don't check the same for Progress values b/c it won't be true
		for old saves) */
	int const iTotalGreatPeopleRateModifier = getTotalGreatPeopleRateModifier(); // advc.001c
	for (int i = 0; i < kCiv.getNumUnits(); i++)
	{
		UnitTypes eUnit = kCiv.unitAt(i);
		int iUnitRate = getGreatPeopleUnitRate(eUnit); // advc
		changeGreatPeopleUnitProgress(eUnit, (iUnitRate *
				/*	advc.001c: Seems like an oversight - per-unit progress
					should include modifiers. */
				iTotalGreatPeopleRateModifier) / 100);
		// <advc>
		#ifdef FASSERT_ENABLE
			iTotalUnitRate += iUnitRate;
		#endif // </advc>
	}
	FAssert(iTotalUnitRate == getBaseGreatPeopleRate()); // advc
	if (getGreatPeopleProgress() >= GET_PLAYER(getOwner()).greatPeopleThreshold())
	{
		int iTotalGreatPeopleUnitProgress = 0;
		for (int i = 0; i < kCiv.getNumUnits(); i++)
		{
			iTotalGreatPeopleUnitProgress += getGreatPeopleUnitProgress(kCiv.unitAt(i));
		}
		/*	<advc> Should only happen in old savegames (due to a bug in AdvCiv 0.97).
			NB: If we break savegame compatibility, then we should call
			CvRandom::weightedChoice here (to avoid code duplication). */
		bool const bOverflow = (iTotalGreatPeopleUnitProgress > CvRandom::getRange());
		if (bOverflow)
			iTotalGreatPeopleUnitProgress = per100(iTotalGreatPeopleUnitProgress).ceil();
		// </advc>
		UnitTypes eGreatPeopleUnit = NO_UNIT;
		{
			int iGreatPeopleUnitRand = SyncRandNum(iTotalGreatPeopleUnitProgress);
			for (int i = 0; i < kCiv.getNumUnits(); i++)
			{
				UnitTypes eLoopGP = kCiv.unitAt(i);
				int iLoopProgress = getGreatPeopleUnitProgress(eLoopGP);
				// <advc> (see above)
				if (bOverflow)
					iLoopProgress = per100(iLoopProgress).ceil(); // </advc>
				if (iGreatPeopleUnitRand < iLoopProgress)
				{
					eGreatPeopleUnit = eLoopGP;
					break;
				}
				iGreatPeopleUnitRand -= iLoopProgress;
			}
		}
		if (eGreatPeopleUnit != NO_UNIT)
		{
			changeGreatPeopleProgress(-GET_PLAYER(getOwner()).greatPeopleThreshold(false));
			for (int i = 0; i < kCiv.getNumUnits(); i++)
			{
				setGreatPeopleUnitProgress(kCiv.unitAt(i), 0);
			}
			createGreatPeople(eGreatPeopleUnit, true, false);
		}
	}
}


void CvCity::doMeltdown()
{
	if (GC.getPythonCaller()->doMeltdown(*this))
		return;

	bool bMessage = false; // advc
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes eDangerBuilding = kCiv.buildingAt(i);
		if (!isMeltdownBuilding(eDangerBuilding))
			continue;
		// advc.opt: Roll the dice before checking for a safe power source.
		int const iOddsDivisor = GC.getInfo(eDangerBuilding).getNukeExplosionRand();
		{
			// advc.652: Adjust to game speed
			scaled rNukeProb = 1 / (iOddsDivisor * GC.getGame().gameSpeedMultiplier());
			if (!SyncRandSuccess(rNukeProb) ||
				isMeltdownBuildingSuperseded(eDangerBuilding)) // kekm5
			{
				continue;
			}
		}
		if (getNumRealBuilding(eDangerBuilding) > 0)
			setNumRealBuilding(eDangerBuilding, 0);
		getPlot().nukeExplosion(1, /* K-Mod: */ 0, false);
		bMessage = true;
		break;
	}
	if (!bMessage)
		return;
	CvWString szBuffer(gDLL->getText("TXT_KEY_MISC_MELTDOWN_CITY", getNameKey()));
	// advc.106: Notify all players
	for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
	{
		CvPlayer const& kObs = *it;
		if (isRevealed(kObs.getTeam()) /* advc.127: */ || kObs.isSpectator())
		{
			bool bAffected = kObs.getID() == getOwner();
			gDLL->UI().addMessage(kObs.getID(), false, -1, szBuffer, getPlot(),
					"AS2D_MELTDOWN",
					// advc.106: Was MINOR (and only the owner was notified)
					bAffected ? MESSAGE_TYPE_MAJOR_EVENT : MESSAGE_TYPE_MINOR_EVENT,
					ARTFILEMGR.getInterfaceArtPath("INTERFACE_UNHEALTHY_PERSON"),
					GC.getColorType(bAffected ? "RED" : "BUILDING_TEXT"));
		}
	}
	// <advc.106> Replay msg - or maybe not. Doesn't really affect the cause of the game.
	/*GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwner(), szBuffer,
			getX(), getY(), GET_PLAYER(getOwner()).getPlayerTextColor());*/ // </advc.106>
}

// advc.652: Kek-Mod code moved into subroutine
bool CvCity::isMeltdownBuilding(BuildingTypes eBuilding) const
{
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	// <kekm.5>
	if (kBuilding.getNukeExplosionRand() <= 0)
		return false;
	if (getNumActiveBuilding(eBuilding) <= 0) // getNumBuilding in Kek-Mod
		return false;
	if (isAreaCleanPower() ||
		(!kBuilding.isPower() && kBuilding.getPowerBonus() == NO_BONUS) ||
		(kBuilding.getPowerBonus() != NO_BONUS &&
		!hasBonus(kBuilding.getPowerBonus())))
	{
		return false;
	} // </kekm.5>
	return true;
}

/*	advc.652: Kek-Mod code moved into subroutine. Caller should ensure that
	eDangerBuilding can cause a meltdown when assuming that no other power source
	exists in this city. */
bool CvCity::isMeltdownBuildingSuperseded(BuildingTypes eDangerBuilding) const
{
	CvBuildingInfo const& kDangerBuilding = GC.getInfo(eDangerBuilding);
	CvCivilization const& kCiv = getCivilization();
	// <kekm.5>
	// Check for hydroplant (or any modded plant with less severe drawbacks)
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes eSafeBuilding = kCiv.buildingAt(i);
		if (eDangerBuilding == eSafeBuilding ||
			getNumActiveBuilding(eSafeBuilding) <= 0)
		{
			continue;
		}
		CvBuildingInfo const& kSafeBuilding = GC.getInfo(eSafeBuilding);
		if(!kSafeBuilding.isPower() &&
			(kSafeBuilding.getPowerBonus() == NO_BONUS ||
			!hasBonus(kSafeBuilding.getPowerBonus())))
		{
			continue;
		}
		if (kDangerBuilding.isDirtyPower() && !kSafeBuilding.isDirtyPower())
			return true;

		if (kDangerBuilding.isDirtyPower() != kSafeBuilding.isDirtyPower())
			continue;
		if (kSafeBuilding.getNukeExplosionRand() == 0)
			return true;
		if (kDangerBuilding.getNukeExplosionRand() >
			kSafeBuilding.getNukeExplosionRand())
		{
			continue;
		}
		if (kDangerBuilding.getNukeExplosionRand() < kSafeBuilding.getNukeExplosionRand())
			return true;
		if (eDangerBuilding < eSafeBuilding)
			return true;
	}
	return false;
}


void CvCity::read(FDataStreamBase* pStream)
{
	uint uiFlag=0;
	pStream->Read(&uiFlag);

	pStream->Read(&m_iID);
	pStream->Read(&m_iX);
	pStream->Read(&m_iY);
	updatePlot(); // advc.opt
	pStream->Read(&m_iRallyX);
	pStream->Read(&m_iRallyY);
	pStream->Read(&m_iGameTurnFounded);
	pStream->Read(&m_iGameTurnAcquired);
	pStream->Read(&m_iPopulation);
	/* DOTO-Population Limit ModComp - Beginning */
	pStream->Read(&m_iPopulationLimitChange);
	/* DOTO-Population Limit ModComp - End */
	pStream->Read(&m_iHighestPopulation);
	pStream->Read(&m_iWorkingPopulation);
	pStream->Read(&m_iSpecialistPopulation);
	pStream->Read(&m_iNumGreatPeople);
	pStream->Read(&m_iBaseGreatPeopleRate);
	pStream->Read(&m_iGreatPeopleRateModifier);
	pStream->Read(&m_iGreatPeopleProgress);
	pStream->Read(&m_iNumWorldWonders);
	pStream->Read(&m_iNumTeamWonders);
	pStream->Read(&m_iNumNationalWonders);
	pStream->Read(&m_iNumBuildings);
	pStream->Read(&m_iGovernmentCenterCount);
	pStream->Read(&m_iMaintenance);
	pStream->Read(&m_iMaintenanceModifier);
	//DOTO-DPII < Maintenance Modifiers >
	pStream->Read(&m_iLocalDistanceMaintenanceModifier);
	pStream->Read(&m_iLocalCoastalDistanceMaintenanceModifier);
	pStream->Read(&m_iLocalConnectedCityMaintenanceModifier);
	pStream->Read(&m_iLocalHomeAreaMaintenanceModifier);
	pStream->Read(&m_iLocalOtherAreaMaintenanceModifier);
	//DOTO-DPII < Maintenance Modifiers >
	pStream->Read(&m_iWarWearinessModifier);
	pStream->Read(&m_iHurryAngerModifier);
	pStream->Read(&m_iHealRate);
	pStream->Read(&m_iEspionageHealthCounter);
	pStream->Read(&m_iEspionageHappinessCounter);
	pStream->Read(&m_iFreshWaterGoodHealth);
	pStream->Read(&m_iFreshWaterBadHealth);
	pStream->Read(&m_iSurroundingGoodHealth);
	pStream->Read(&m_iSurroundingBadHealth);
/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: Enable Terrain Health Modifiers                                                  **/
/**  Notes:   DOTO-                                                                                      **/
/*****************************************************************************************************/
	pStream->Read(&m_iTerrainGoodHealth);
	pStream->Read(&m_iTerrainBadHealth);
/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**        DOTO-                                                                                      */
/*************************************************************************************************/
	pStream->Read(&m_iSpecialistGoodHealth);
	pStream->Read(&m_iSpecialistBadHealth);
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/
	pStream->Read(&m_iBuildingGoodHealth);
	pStream->Read(&m_iBuildingBadHealth);
	pStream->Read(&m_iPowerGoodHealth);
	pStream->Read(&m_iPowerBadHealth);
// DOTO-< Civic Infos Plus Start >
	pStream->Read(&m_iReligionGoodHealth);
	pStream->Read(&m_iReligionBadHealth);
// DOTO-< Civic Infos Plus End   >
	pStream->Read(&m_iBonusGoodHealth);
	pStream->Read(&m_iBonusBadHealth);
	pStream->Read(&m_iHurryAngerTimer);
	pStream->Read(&m_iConscriptAngerTimer);
	pStream->Read(&m_iDefyResolutionAngerTimer);
	pStream->Read(&m_iHappinessTimer);
	pStream->Read(&m_iMilitaryHappinessUnits);
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                                                                                              */
/**  DOTO-                                                                                            */
/*************************************************************************************************/
	pStream->Read(&m_iSpecialistHappiness);
	pStream->Read(&m_iSpecialistUnhappiness);
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/
	pStream->Read(&m_iBuildingGoodHappiness);
	pStream->Read(&m_iBuildingBadHappiness);
	pStream->Read(&m_iExtraBuildingGoodHappiness);
	pStream->Read(&m_iExtraBuildingBadHappiness);
	pStream->Read(&m_iExtraBuildingGoodHealth);
	pStream->Read(&m_iExtraBuildingBadHealth);
	pStream->Read(&m_iSurroundingGoodHappiness);
	pStream->Read(&m_iSurroundingBadHappiness);
	pStream->Read(&m_iBonusGoodHappiness);
	pStream->Read(&m_iBonusBadHappiness);
	pStream->Read(&m_iReligionGoodHappiness);
	pStream->Read(&m_iReligionBadHappiness);
	pStream->Read(&m_iExtraHappiness);
	pStream->Read(&m_iExtraHealth);
	pStream->Read(&m_iNoUnhappinessCount);
	//pStream->Read(&m_iNoUnhealthyPopulationCount);
	pStream->Read(&m_iUnhealthyPopulationModifier); // K-Mod
	pStream->Read(&m_iBuildingOnlyHealthyCount);
	pStream->Read(&m_iFood);
	pStream->Read(&m_iFoodKept);
	pStream->Read(&m_iMaxFoodKeptPercent);
	pStream->Read(&m_iOverflowProduction);
	pStream->Read(&m_iFeatureProduction);
	pStream->Read(&m_iMilitaryProductionModifier);
	pStream->Read(&m_iSpaceProductionModifier);
	pStream->Read(&m_iExtraTradeRoutes);
	pStream->Read(&m_iTradeRouteModifier);
	pStream->Read(&m_iForeignTradeRouteModifier);
	pStream->Read(&m_iBuildingDefense);
	pStream->Read(&m_iBuildingBombardDefense);
	pStream->Read(&m_iFreeExperience);
	pStream->Read(&m_iCurrAirlift);
	pStream->Read(&m_iMaxAirlift);
	pStream->Read(&m_iAirModifier);
	pStream->Read(&m_iAirUnitCapacity);
	pStream->Read(&m_iNukeModifier);
	pStream->Read(&m_iFreeSpecialist);
	pStream->Read(&m_iPowerCount);
	pStream->Read(&m_iDirtyPowerCount);
	pStream->Read(&m_iDefenseDamage);
	pStream->Read(&m_iLastDefenseDamage);
	pStream->Read(&m_iOccupationTimer);
	pStream->Read(&m_iCultureUpdateTimer);
	pStream->Read(&m_iCitySizeBoost);
	pStream->Read(&m_iSpecialistFreeExperience);
	pStream->Read(&m_iEspionageDefenseModifier);

	pStream->Read(&m_bNeverLost);
	pStream->Read(&m_bBombarded);
	pStream->Read(&m_bDrafted);
	pStream->Read(&m_bAirliftTargeted);
	pStream->Read(&m_bWeLoveTheKingDay);
	pStream->Read(&m_bCitizensAutomated);
	pStream->Read(&m_bProductionAutomated);
	pStream->Read(&m_bWallOverride);
	// m_bInfoDirty not saved...
	// m_bLayoutDirty not saved...
	pStream->Read(&m_bPlundered);
	// <advc.103>
	if(uiFlag >= 6)
		pStream->Read(&m_bInvestigate);
	// </advc.103> <advc.003u>
	if (uiFlag >= 7)
		pStream->Read(&m_bChooseProductionDirty);
	// </advc.003u>
	pStream->Read((int*)&m_eOwner);
	pStream->Read((int*)&m_ePreviousOwner);
	pStream->Read((int*)&m_eOriginalOwner);
	pStream->Read((int*)&m_eCultureLevel);

	if (uiFlag >= 12)
	{
		m_aiSeaPlotYield.read(pStream);
		m_aiRiverPlotYield.read(pStream);
		m_aiBaseYieldRate.read(pStream);
		m_aiYieldRateModifier.read(pStream);
		m_aiPowerYieldRateModifier.read(pStream);
		m_aiBonusYieldRateModifier.read(pStream);
	// DOTO-< Civic Infos Plus Start >
	//no need for these - f1 advc - STILL TRUE?
		m_aiBuildingYieldChange.read(pStream);
		m_aiStateReligionYieldRateModifier.read(pStream);
		m_aiNonStateReligionYieldRateModifier.read(pStream);
	// DOTO-< Civic Infos Plus End   >
		m_aiTradeYield.read(pStream);
		m_aiCorporationYield.read(pStream);
		m_aiExtraSpecialistYield.read(pStream);
		m_aiCommerceRate.read(pStream);
		m_aiProductionToCommerceModifier.read(pStream);
		m_aiBuildingCommerce.read(pStream);
		m_aiSpecialistCommerce.read(pStream);
		m_aiReligionCommerce.read(pStream);
	// DOTO-< Civic Infos Plus Start >
	//no need for these f1 advc - STILL TRUE?
		m_aiStateReligionCommerceRateModifier.read(pStream);
		m_aiNonStateReligionCommerceRateModifier.read(pStream);
		m_aiBuildingCommerceChange.read(pStream);
	// DOTO-< Civic Infos Plus End   >
		m_aiCorporationCommerce.read(pStream);
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**DOTO-																								**/
	/**																								**/
	/*************************************************************************************************/	 
		m_aiExtraSpecialistCommerce.read(pStream);
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/
		m_aiCommerceRateModifier.read(pStream);
		m_aiCommerceHappinessPer.read(pStream);
		m_aiDomainFreeExperience.read(pStream);
		m_aiDomainProductionModifier.read(pStream);
		m_aiCulture.read(pStream);
		m_aiNumRevolts.read(pStream);
		m_abEverOwned.read(pStream);
		m_abTradeRoute.read(pStream);
		m_abRevealed.read(pStream);
		m_abEspionageVisibility.read(pStream);
	}
	else
	{
		m_aiSeaPlotYield.readArray<int>(pStream);
		m_aiRiverPlotYield.readArray<int>(pStream);
		m_aiBaseYieldRate.readArray<int>(pStream);
		m_aiYieldRateModifier.readArray<int>(pStream);
		m_aiPowerYieldRateModifier.readArray<int>(pStream);
		m_aiBonusYieldRateModifier.readArray<int>(pStream);
	// DOTO-< Civic Infos Plus Start >
	//no need for these - f1 advc - STILL TRUE?
		m_aiBuildingYieldChange.readArray<int>(pStream);
		m_aiStateReligionYieldRateModifier.readArray<int>(pStream);
		m_aiNonStateReligionYieldRateModifier.readArray<int>(pStream);
	// DOTO-< Civic Infos Plus End   >
		m_aiTradeYield.readArray<int>(pStream);
		m_aiCorporationYield.readArray<int>(pStream);
		m_aiExtraSpecialistYield.readArray<int>(pStream);
		m_aiCommerceRate.readArray<int>(pStream);
		m_aiProductionToCommerceModifier.readArray<int>(pStream);
		m_aiBuildingCommerce.readArray<int>(pStream);
		m_aiSpecialistCommerce.readArray<int>(pStream);
		m_aiReligionCommerce.readArray<int>(pStream);
	// DOTO-< Civic Infos Plus Start >
	//no need for these f1 advc - STILL TRUE?
		m_aiStateReligionCommerceRateModifier.readArray<int>(pStream);
		m_aiNonStateReligionCommerceRateModifier.readArray<int>(pStream);
		m_aiBuildingCommerceChange.readArray<int>(pStream);
	// DOTO-< Civic Infos Plus End   >
		m_aiCorporationCommerce.readArray<int>(pStream);
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**DOTO-																								**/
	/**																								**/
	/*************************************************************************************************/	 
		m_aiExtraSpecialistCommerce.readArray<int>(pStream);
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/
		m_aiCommerceRateModifier.readArray<int>(pStream);
		m_aiCommerceHappinessPer.readArray<int>(pStream);
		m_aiDomainFreeExperience.readArray<int>(pStream);
		m_aiDomainProductionModifier.readArray<int>(pStream);
		m_aiCulture.readArray<int>(pStream);
		m_aiNumRevolts.readArray<int>(pStream);
		m_abEverOwned.readArray<bool>(pStream);
		m_abTradeRoute.readArray<bool>(pStream);
		m_abRevealed.readArray<bool>(pStream);
		m_abEspionageVisibility.readArray<bool>(pStream);
	}

	pStream->ReadString(m_szName);
	// <advc.106k>
	if(uiFlag >= 5)
		pStream->ReadString(m_szPreviousName); // </advc.106k>
	pStream->ReadString(m_szScriptData);
	if (uiFlag >= 12)
	{
		m_aiNoBonus.read(pStream);
		m_aiFreeBonus.read(pStream);
		m_aiNumBonuses.read(pStream);
	// DOTO-< Building Resource Converter Start >
		m_paiBuildingOutputBonuses.read(pStream);
	// DOTO-< Building Resource Converter End   >
		m_aiNumCorpProducedBonuses.read(pStream);
		m_aiProjectProduction.read(pStream);
		m_aiBuildingProduction.read(pStream);
		m_aiBuildingProductionTime.read(pStream);
		m_aeBuildingOriginalOwner.read(pStream);
//tholish-Keldath inactive buildings
		m_aiBuildingeActive.read(pStream);
		m_aiBuildingOriginalTime.read(pStream);
		m_aiUnitProduction.read(pStream);
		m_aiUnitProductionTime.read(pStream);
		m_aiGreatPeopleUnitRate.read(pStream);
		m_aiGreatPeopleUnitProgress.read(pStream);
		m_aiSpecialistCount.read(pStream);
		m_aiMaxSpecialistCount.read(pStream);
		m_aiForceSpecialistCount.read(pStream);
		m_aiFreeSpecialistCount.read(pStream);
		m_aiImprovementFreeSpecialists.read(pStream);
		m_aiReligionInfluence.read(pStream);
		m_aiStateReligionHappiness.read(pStream);
		m_aiUnitCombatFreeExperience.read(pStream);
		m_aiFreePromotionCount.read(pStream);
		m_aiNumRealBuilding.read(pStream);
		m_aiNumFreeBuilding.read(pStream);
//mylon enhanced cities doto advc version
		pStream->Read(&m_iRadius);
		pStream->Read((int*)&m_eCityPlots);
		//m_abWorkingPlot.read(pStream);
		// (Only the uiFlag>=12 branch matters, can delete the other branch.)
		m_abWorkingPlot.resize(NUM_CITY_PLOTS); // (not allocated by ctor)
		FOR_EACH_CITYPLOT
		{
			bool bWorkingPlot;
			pStream->Read(&bWorkingPlot);
			m_abWorkingPlot[eLoopCityPlot] = bWorkingPlot;
		}
//mylon enhanced cities doto advc version
		m_abHasReligion.read(pStream);
		m_abHasCorporation.read(pStream);
	}
	else
	{
		m_aiNoBonus.readArray<int>(pStream);
		m_aiFreeBonus.readArray<int>(pStream);
		m_aiNumBonuses.readArray<int>(pStream);
	// DOTO-< Building Resource Converter Start >
		m_paiBuildingOutputBonuses.readArray<int>(pStream);
	// DOTO-< Building Resource Converter End   >
		m_aiNumCorpProducedBonuses.readArray<int>(pStream);
		m_aiProjectProduction.readArray<int>(pStream);
		m_aiBuildingProduction.readArray<int>(pStream);
		m_aiBuildingProductionTime.readArray<int>(pStream);
		m_aeBuildingOriginalOwner.readArray<int>(pStream);
//tholish-Keldath inactive buildings
		m_aiBuildingeActive.readArray<int>(pStream);
		EagerEnumMap<BuildingTypes,int> aiBuildingOriginalTime;
		aiBuildingOriginalTime.readArray<int>(pStream);
		FOR_EACH_ENUM(Building)
		{
			int iTimeRead = aiBuildingOriginalTime.get(eLoopBuilding);
			m_aiBuildingOriginalTime.set(eLoopBuilding,
					iTimeRead == MIN_INT ? iBuildingOriginalTimeUnknown : iTimeRead);
		}
		m_aiUnitProduction.readArray<int>(pStream);
		m_aiUnitProductionTime.readArray<int>(pStream);
		m_aiGreatPeopleUnitRate.readArray<int>(pStream);
		m_aiGreatPeopleUnitProgress.readArray<int>(pStream);
		m_aiSpecialistCount.readArray<int>(pStream);
		m_aiMaxSpecialistCount.readArray<int>(pStream);
		m_aiForceSpecialistCount.readArray<int>(pStream);
		m_aiFreeSpecialistCount.readArray<int>(pStream);
		m_aiImprovementFreeSpecialists.readArray<int>(pStream);
		m_aiReligionInfluence.readArray<int>(pStream);
		m_aiStateReligionHappiness.readArray<int>(pStream);
		m_aiUnitCombatFreeExperience.readArray<int>(pStream);
		m_aiFreePromotionCount.readArray<int>(pStream);
		m_aiNumRealBuilding.readArray<int>(pStream);
		m_aiNumFreeBuilding.readArray<int>(pStream);
//mylon enhanced cities doto advc version
		// (Only the uiFlag>=12 branch matters, can delete the other branch.)
		//m_abWorkingPlot.readArray<bool>(pStream);
		m_abHasReligion.readArray<bool>(pStream);
		m_abHasCorporation.readArray<bool>(pStream);
	}
	for (size_t i = 0; i < m_aTradeCities.size(); i++)
	{
		pStream->Read((int*)&m_aTradeCities[i].eOwner);
		pStream->Read(&m_aTradeCities[i].iID);
		m_aTradeCities[i].validateOwner(); // advc.opt
	}

	m_orderQueue.Read(pStream);

	pStream->Read(&m_iPopulationRank);
	pStream->Read(&m_bPopulationRankValid);
	if (uiFlag >= 12)
	{
		m_aiBaseYieldRank.read(pStream);
		m_abBaseYieldRankValid.read(pStream);
		m_aiYieldRank.read(pStream);
		m_abYieldRankValid.read(pStream);
		m_aiCommerceRank.read(pStream);
		m_abCommerceRankValid.read(pStream);
	}
	else
	{
		m_aiBaseYieldRank.readArray<int>(pStream);
		m_abBaseYieldRankValid.readArray<bool>(pStream);
		m_aiYieldRank.readArray<int>(pStream);
		m_abYieldRankValid.readArray<bool>(pStream);
		m_aiCommerceRank.readArray<int>(pStream);
		m_abCommerceRankValid.readArray<bool>(pStream);
	}

	int iNumElts=-1;

	pStream->Read(&iNumElts);
	m_aEventsOccured.clear();
	for (int i = 0; i < iNumElts; ++i)
	{
		EventTypes eEvent;
		pStream->Read((int*)&eEvent);
		m_aEventsOccured.push_back(eEvent);
	}

	if (uiFlag >= 12)
	{
		m_aeiiBuildingYieldChange.read(pStream);
		m_aeiiBuildingCommerceChange.read(pStream);
		m_aeiBuildingHappyChange.read(pStream);
		m_aeiBuildingHealthChange.read(pStream);
	}
	else
	{
		// (advc: BtS had actually read int but written size_t)
		size_t uiSize;
		pStream->Read(&uiSize);
		for (size_t i = 0; i < uiSize; i++)
		{
			int iBuildingClass, iYield, iChange;
			pStream->Read(&iBuildingClass);
			pStream->Read(&iYield);
			pStream->Read(&iChange);
			m_aeiiBuildingYieldChange.set(
					static_cast<BuildingClassTypes>(iBuildingClass),
					static_cast<YieldTypes>(iYield),
					iChange);
		}
		pStream->Read(&uiSize);
		for (size_t i = 0; i < uiSize; i++)
		{
			int iBuildingClass, iCommerce, iChange;
			pStream->Read(&iBuildingClass);
			pStream->Read(&iCommerce);
			pStream->Read(&iChange);
			m_aeiiBuildingCommerceChange.set(
					static_cast<BuildingClassTypes>(iBuildingClass),
					static_cast<CommerceTypes>(iCommerce),
					iChange);
		}
		m_aeiBuildingHappyChange.readPair<size_t,int,int>(pStream);
		m_aeiBuildingHealthChange.readPair<size_t,int,int>(pStream);
	}
	// <advc.912d>
	if(uiFlag >= 4)
		pStream->Read(&m_iPopRushHurryCount);
	if (uiFlag < 15 &&
		(uiFlag < 9 || // Everyone had gotten only 40% from Granary prior to version 9
		// After version 9, players affected by No Slavery had been exempt
		!isHuman() || !GC.getGame().isOption(GAMEOPTION_NO_SLAVERY)))
	{	// Now everyone gets 50% again, just like in BtS.
		m_iMaxFoodKeptPercent = (m_iMaxFoodKeptPercent * 5) / 4;
	} // </advc.912d>
	// <advc.004x>
	if(uiFlag >= 2)
	{
		pStream->Read(&m_iMostRecentOrder);
		pStream->Read(&m_bMostRecentUnit);
	} // </advc.004x>
	// <advc.030b>
	if(uiFlag < 3)
	{
		CvArea* pWaterArea = waterArea(true);
		if(pWaterArea != NULL)
			pWaterArea->changeCitiesPerPlayer(getOwner(), 1);
	} // </advc.030b>
	// <advc.310>
	if (uiFlag < 8)
	{
		BuildingTypes eVersailles = (BuildingTypes)GC.getInfoTypeForString(
				"BUILDING_VERSAILLES", true);
		if (eVersailles != NO_BUILDING && getNumBuilding(eVersailles) > 0)
		{
			CvBuildingInfo const& kVersailles = GC.getInfo(eVersailles);
			int iRateChange = kVersailles.getGreatPeopleRateChange();
			UnitClassTypes eOldGPClass = (UnitClassTypes)GC.getInfoTypeForString(
						"UNITCLASS_MERCHANT", true);
			UnitClassTypes eNewGPClass = (UnitClassTypes)kVersailles.getGreatPeopleUnitClass();
			/*	To provide some safety against messing up savegames of a mod-mod
				that may not have adopted this XML change */
			if (eNewGPClass == GC.getInfoTypeForString("UNITCLASS_GREAT_SPY", true) &&
				iRateChange == 2 && eOldGPClass != NO_UNITCLASS)
			{
				UnitTypes eOldGPUnit = getCivilization().getUnit(eOldGPClass);
				UnitTypes eNewGPUnit = getCivilization().getUnit(eNewGPClass);
				if (eOldGPUnit != NO_UNIT && eNewGPUnit != NO_UNIT)
				{
					changeGreatPeopleUnitRate(eOldGPUnit, -iRateChange);
					changeGreatPeopleUnitRate(eNewGPUnit, iRateChange);
				}
			}
		}
	} // </advc.310>
	/*	<advc.911a>  (Reminder: All this junk can and should be
		deleted as soon as savegame compatibility gets broken by a release) */
	if (uiFlag < 10)
	{
		SpecialistTypes eSpy = (SpecialistTypes)GC.getInfoTypeForString(
				"SPECIALIST_SPY", true);
		if (eSpy != NO_SPECIALIST)
		{
			std::vector<std::pair<BuildingClassTypes,int> > aeiSpyChanges;
			aeiSpyChanges.push_back(std::make_pair((BuildingClassTypes)
					GC.getInfoTypeForString("BUILDINGCLASS_COURTHOUSE", true),
					1));
			aeiSpyChanges.push_back(std::make_pair((BuildingClassTypes)
					GC.getInfoTypeForString("BUILDINGCLASS_JAIL", true),
					-1));
			for (size_t i = 0; i < aeiSpyChanges.size(); i++)
			{
				BuildingClassTypes const eClass = aeiSpyChanges[i].first;
				if (eClass == NO_BUILDINGCLASS || getNumBuilding(eClass) <= 0)
					continue;
				FOR_EACH_ENUM(Building)
				{
					if (GC.getInfo(eLoopBuilding).getBuildingClassType() == eClass &&
						!GET_TEAM(getTeam()).isObsoleteBuilding(eLoopBuilding))
					{
						changeMaxSpecialistCount(eSpy, getNumBuilding(eLoopBuilding) *
								aeiSpyChanges[i].second);
					}

				}
			}
		} // </advc.911a>
		// <advc.908b>
		BuildingTypes eHippodrome = (BuildingTypes)
					GC.getInfoTypeForString("BUILDING_BYZANTINE_HIPPODROME", true);
		if (eHippodrome != NO_BUILDING && getNumBuilding(eHippodrome) > 0 &&
			!GET_TEAM(getTeam()).isObsoleteBuilding(eHippodrome))
		{
			SpecialistTypes eArtist = (SpecialistTypes)GC.getInfoTypeForString(
					"SPECIALIST_ARTIST", true);
			if (eArtist != NO_SPECIALIST)
				changeMaxSpecialistCount(eArtist, getNumBuilding(eHippodrome));
		}
		std::vector<std::pair<BuildingClassTypes,int> > aeiCultureHappyMults;
		aeiCultureHappyMults.push_back(std::make_pair((BuildingClassTypes)
				GC.getInfoTypeForString("BUILDINGCLASS_THEATRE", true),
				50));
		aeiCultureHappyMults.push_back(std::make_pair((BuildingClassTypes)
				GC.getInfoTypeForString("BUILDINGCLASS_COLOSSEUM", true),
				200));
		for (size_t i = 0; i < aeiCultureHappyMults.size(); i++)
		{
			BuildingClassTypes const eClass = aeiCultureHappyMults[i].first;
			if (eClass == NO_BUILDINGCLASS || getNumBuilding(eClass) <= 0)
				continue;
			FOR_EACH_ENUM(Building)
			{
				if (GC.getInfo(eLoopBuilding).getBuildingClassType() == eClass &&
					!GET_TEAM(getTeam()).isObsoleteBuilding(eLoopBuilding))
				{
					int iNewHappy = GC.getInfo(eLoopBuilding).
							getCommerceHappiness(COMMERCE_CULTURE);
					int iOldHappy = (iNewHappy * 100) / aeiCultureHappyMults[i].second;
					changeCommerceHappinessPer(COMMERCE_CULTURE,
							(iNewHappy - iOldHappy) * getNumBuilding(eLoopBuilding));
				}
			}
		} // </advc.908b>
		// <advc.160>
		if (GC.getGame().isOption(GAMEOPTION_NO_SLAVERY) && isHuman())
		{
			BuildingClassTypes eGranary = (BuildingClassTypes)
					GC.getInfoTypeForString("BUILDINGCLASS_GRANARY", true);
			if (eGranary != NO_BUILDINGCLASS)
			{
				int iGranaries = getNumBuilding(eGranary);
				if (iGranaries > 0 && getMaxFoodKeptPercent() >= 40)
					changeMaxFoodKeptPercent(10 * iGranaries);
			}
		} 
	} // </advc.160>
	// <advc.201>, advc.200, advc.098
	if (uiFlag < 13)
	{
		bool bUpdate = true;
		SpecialBuildingTypes eCath = (SpecialBuildingTypes)GC.getInfoTypeForString(
				"SPECIALBUILDING_CATHEDRAL");
		if (eCath != NO_SPECIALBUILDING &&
			getCommerceRateModifier(COMMERCE_CULTURE) >= 40) // to save time
		{
			CvCivilization const& kCiv = getCivilization();
			for (int i = 0; i < kCiv.getNumBuildings(); i++)
			{
				if (getNumBuilding(kCiv.buildingAt(i)) <= 0)
					continue;
				if (GC.getInfo(kCiv.buildingAt(i)).getSpecialBuildingType() != eCath)
					continue;
				changeCommerceRateModifier(COMMERCE_CULTURE, 10);
				bUpdate = false;
			}
		}
		if (bUpdate)
			updateBuildingCommerce(COMMERCE_CULTURE);
		if (uiFlag < 11)
		{
			CorporationTypes eCreativeConstr = (CorporationTypes)
						GC.getInfoTypeForString("CORPORATION_4", true);
			if (eCreativeConstr != NO_CORPORATION && isHasCorporation(eCreativeConstr))
				updateCorporationCommerce(COMMERCE_CULTURE);
		}
	} // </advc.201>
	// <advc.179>
	if (uiFlag < 14)
	{
		VoteSourceTypes eAP = (VoteSourceTypes)GC.getInfoTypeForString("DIPLOVOTE_POPE");
		if (eAP != NO_VOTESOURCE)
		{
			ReligionTypes eAPReligion = GC.getGame().getVoteSourceReligion(eAP);
			if (eAPReligion != NO_RELIGION && isHasReligion(eAPReligion) &&
				GET_PLAYER(getOwner()).isLoyalMember(eAP) && GC.getGame().isDiploVote(eAP))
			{
				FOR_EACH_ENUM(Building)
				{
					if (GC.getInfo(eLoopBuilding).getReligionType() == eAPReligion)
					{
						setBuildingYieldChange(CvCivilization::buildingClass(eLoopBuilding),
								/*	Set the yield to 1 rather than subtracting 1.
									To address a bug that had existed in AdvCiv 1.00
									and had, in some circumstances, set the yield to 0. */
								YIELD_PRODUCTION, 1);
					}
				}
			}
		}
		BuildingTypes eAPBuilding = (BuildingTypes)GC.getInfoTypeForString("BUILDING_APOSTOLIC_PALACE");
		SpecialistTypes ePriest = (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_PRIEST");
		if (eAPBuilding != NO_BUILDING && ePriest != NO_SPECIALIST)
		{
			char const* aszShrineNames[] = { "BUILDING_JEWISH_SHRINE", "BUILDING_CHRISTIAN_SHRINE",
					"BUILDING_ISLAMIC_SHRINE", "BUILDING_HINDU_SHRINE", "BUILDING_BUDDHIST_SHRINE",
					"BUILDING_CONFUCIAN_SHRINE", "BUILDING_TAOIST_SHRINE" };
			for (int i = 0; i < ARRAYSIZE(aszShrineNames); i++)
			{
				BuildingTypes eShrine = (BuildingTypes)GC.getInfoTypeForString(aszShrineNames[i]);
				if (eShrine == NO_BUILDING)
					continue;
				// Shrines lose one priest slot each
				changeMaxSpecialistCount(ePriest, -1 * getNumBuilding(eShrine));
			}
			changeMaxSpecialistCount(ePriest, 2 * getNumBuilding(eAPBuilding));
		} // </advc.179>
		// <advc.exp.1>
		// (Change to MAX_CITY_COUNT_FOR_MAINTENANCE can only affect large civs)
		if (GET_PLAYER(getOwner()).getNumCities() > 20 ||
			// <advc.exp.2> (Changes to the unique buildings)
			getCivilizationType() == GC.getInfoTypeForString("CIVILIZATION_ZULU") ||
			getCivilizationType() == GC.getInfoTypeForString("CIVILIZATION_HOLY_ROMAN"))
			// </advc.exp.2>
		{
			updateMaintenance();
		}
	} // </advc.exp.1>
}

void CvCity::write(FDataStreamBase* pStream)
{
	PROFILE_FUNC(); // advc
	REPRO_TEST_BEGIN_WRITE(CvString::format("City(%d,%d)", getX(), getY()));
	uint uiFlag;
	//uiFlag = 1; // K-Mod
	//uiFlag = 2; // advc.004x
	//uiFlag = 3; // advc.030b
	//uiFlag = 4; // advc.912d
	//uiFlag = 5; // advc.106k
	//uiFlag = 6; // advc.103
	//uiFlag = 7; // advc.003u: m_bChooseProductionDirty
	//uiFlag = 8; // advc.310
	//uiFlag = 9; // advc.912d (adjust food kept; NB: should've adjusted MaxFoodKept)
	//uiFlag = 10; // advc.911a, advc.908b
	//uiFlag = 11; // advc.201, advc.098
	//uiFlag = 12; // advc.enum: new enum map save behavior
	//uiFlag = 13; // advc.201: Cathedrals restored to BtS stats
	//uiFlag = 14; // advc.179, advc.exp.1, advc.exp.2
	uiFlag = 15; // advc.912d: Partly reverted, adjust MaxFoodKept.
	pStream->Write(uiFlag);

	pStream->Write(m_iID);
	pStream->Write(m_iX);
	pStream->Write(m_iY);
	pStream->Write(m_iRallyX);
	pStream->Write(m_iRallyY);
	pStream->Write(m_iGameTurnFounded);
	pStream->Write(m_iGameTurnAcquired);
	pStream->Write(m_iPopulation);
	/* DOTO-Population Limit ModComp - Beginning */
	pStream->Write(m_iPopulationLimitChange);
	/* DOTO-Population Limit ModComp - End */
	pStream->Write(m_iHighestPopulation);
	pStream->Write(m_iWorkingPopulation);
	pStream->Write(m_iSpecialistPopulation);
	pStream->Write(m_iNumGreatPeople);
	pStream->Write(m_iBaseGreatPeopleRate);
	pStream->Write(m_iGreatPeopleRateModifier);
	pStream->Write(m_iGreatPeopleProgress);
	pStream->Write(m_iNumWorldWonders);
	pStream->Write(m_iNumTeamWonders);
	pStream->Write(m_iNumNationalWonders);
	pStream->Write(m_iNumBuildings);
	pStream->Write(m_iGovernmentCenterCount);
	pStream->Write(m_iMaintenance);
	pStream->Write(m_iMaintenanceModifier);
	//DOTO-DPII < Maintenance Modifiers >
	pStream->Write(m_iLocalDistanceMaintenanceModifier);
	pStream->Write(m_iLocalCoastalDistanceMaintenanceModifier);
	pStream->Write(m_iLocalConnectedCityMaintenanceModifier);
	pStream->Write(m_iLocalHomeAreaMaintenanceModifier);
	pStream->Write(m_iLocalOtherAreaMaintenanceModifier);
	//DOTO-DPII < Maintenance Modifiers >
	pStream->Write(m_iWarWearinessModifier);
	pStream->Write(m_iHurryAngerModifier);
	pStream->Write(m_iHealRate);
	pStream->Write(m_iEspionageHealthCounter);
	pStream->Write(m_iEspionageHappinessCounter);
	pStream->Write(m_iFreshWaterGoodHealth);
	pStream->Write(m_iFreshWaterBadHealth);
	pStream->Write(m_iSurroundingGoodHealth);
	pStream->Write(m_iSurroundingBadHealth);
/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: Enable Terrain Health Modifiers                                                  **/
/**  Notes: DOTO-                                                                                        **/
/*****************************************************************************************************/
	pStream->Write(m_iTerrainGoodHealth);
	pStream->Write(m_iTerrainBadHealth);
/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**                      DOTO-                                                                        */
/**                                                                                              */
/*************************************************************************************************/
	pStream->Write(m_iSpecialistGoodHealth);
	pStream->Write(m_iSpecialistBadHealth);
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/
	pStream->Write(m_iBuildingGoodHealth);
	pStream->Write(m_iBuildingBadHealth);
	pStream->Write(m_iPowerGoodHealth);
	pStream->Write(m_iPowerBadHealth);
	// < Civic Infos Plus Start >
	pStream->Write(m_iReligionGoodHealth);
	pStream->Write(m_iReligionBadHealth);
	// < Civic Infos Plus End   >
	pStream->Write(m_iBonusGoodHealth);
	pStream->Write(m_iBonusBadHealth);
	pStream->Write(m_iHurryAngerTimer);
	pStream->Write(m_iConscriptAngerTimer);
	pStream->Write(m_iDefyResolutionAngerTimer);
	pStream->Write(m_iHappinessTimer);
	pStream->Write(m_iMilitaryHappinessUnits);
/*************************************************************************************************/
/** Specialists Enhancements, by Supercheese 10/9/09                                                   */
/**              DOTO-                                                                                */
/**                                                                                              */
/*************************************************************************************************/
	pStream->Write(m_iSpecialistHappiness);
	pStream->Write(m_iSpecialistUnhappiness);
/*************************************************************************************************/
/** Specialists Enhancements                          END                                              */
/*************************************************************************************************/
	pStream->Write(m_iBuildingGoodHappiness);
	pStream->Write(m_iBuildingBadHappiness);
	pStream->Write(m_iExtraBuildingGoodHappiness);
	pStream->Write(m_iExtraBuildingBadHappiness);
	pStream->Write(m_iExtraBuildingGoodHealth);
	pStream->Write(m_iExtraBuildingBadHealth);
	pStream->Write(m_iSurroundingGoodHappiness);
	pStream->Write(m_iSurroundingBadHappiness);
	pStream->Write(m_iBonusGoodHappiness);
	pStream->Write(m_iBonusBadHappiness);
	pStream->Write(m_iReligionGoodHappiness);
	pStream->Write(m_iReligionBadHappiness);
	pStream->Write(m_iExtraHappiness);
	pStream->Write(m_iExtraHealth);
	pStream->Write(m_iNoUnhappinessCount);
	//pStream->Write(m_iNoUnhealthyPopulationCount);
	pStream->Write(m_iUnhealthyPopulationModifier); // K-Mod
	pStream->Write(m_iBuildingOnlyHealthyCount);
	pStream->Write(m_iFood);
	pStream->Write(m_iFoodKept);
	pStream->Write(m_iMaxFoodKeptPercent);
	pStream->Write(m_iOverflowProduction);
	pStream->Write(m_iFeatureProduction);
	pStream->Write(m_iMilitaryProductionModifier);
	pStream->Write(m_iSpaceProductionModifier);
	pStream->Write(m_iExtraTradeRoutes);
	pStream->Write(m_iTradeRouteModifier);
	pStream->Write(m_iForeignTradeRouteModifier);
	pStream->Write(m_iBuildingDefense);
	pStream->Write(m_iBuildingBombardDefense);
	pStream->Write(m_iFreeExperience);
	pStream->Write(m_iCurrAirlift);
	pStream->Write(m_iMaxAirlift);
	pStream->Write(m_iAirModifier);
	pStream->Write(m_iAirUnitCapacity);
	pStream->Write(m_iNukeModifier);
	pStream->Write(m_iFreeSpecialist);
	pStream->Write(m_iPowerCount);
	pStream->Write(m_iDirtyPowerCount);
	pStream->Write(m_iDefenseDamage);
	pStream->Write(m_iLastDefenseDamage);
	pStream->Write(m_iOccupationTimer);
	pStream->Write(m_iCultureUpdateTimer);
	pStream->Write(m_iCitySizeBoost);
	pStream->Write(m_iSpecialistFreeExperience);
	pStream->Write(m_iEspionageDefenseModifier);

	pStream->Write(m_bNeverLost);
	pStream->Write(m_bBombarded);
	pStream->Write(m_bDrafted);
	pStream->Write(m_bAirliftTargeted);
	pStream->Write(m_bWeLoveTheKingDay);
	pStream->Write(m_bCitizensAutomated);
	pStream->Write(m_bProductionAutomated);
	pStream->Write(m_bWallOverride);
	// m_bInfoDirty not saved...
	// m_bLayoutDirty not saved...
	pStream->Write(m_bPlundered);
	pStream->Write(m_bInvestigate); // advc.103
	pStream->Write(m_bChooseProductionDirty); // advc.003u

	pStream->Write(m_eOwner);
	pStream->Write(m_ePreviousOwner);
	pStream->Write(m_eOriginalOwner);
	pStream->Write(m_eCultureLevel);

	m_aiSeaPlotYield.write(pStream);
	m_aiRiverPlotYield.write(pStream);
	m_aiBaseYieldRate.write(pStream);
	m_aiYieldRateModifier.write(pStream);
	m_aiPowerYieldRateModifier.write(pStream);
	m_aiBonusYieldRateModifier.write(pStream);
	// DOTO-< Civic Infos Plus Start >
	// no need for these - f1 advc - STILL TRUE?
	m_aiBuildingYieldChange.write(pStream);
	m_aiStateReligionYieldRateModifier.write(pStream);
	m_aiNonStateReligionYieldRateModifier.write(pStream);
	// DOTO-< Civic Infos Plus End   >
	m_aiTradeYield.write(pStream);
	m_aiCorporationYield.write(pStream);
	m_aiExtraSpecialistYield.write(pStream);
	m_aiCommerceRate.write(pStream);
	m_aiProductionToCommerceModifier.write(pStream);
	m_aiBuildingCommerce.write(pStream);
	m_aiSpecialistCommerce.write(pStream);
	m_aiReligionCommerce.write(pStream);
	// DOTO-< Civic Infos Plus Start >
	//no need for these f1 advc - STILL TRUE?
	m_aiStateReligionCommerceRateModifier.write(pStream);
	m_aiNonStateReligionCommerceRateModifier.write(pStream);
	m_aiBuildingCommerceChange.write(pStream);
	// DOTO-< Civic Infos Plus End   >
	m_aiCorporationCommerce.write(pStream);
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**																								**/
	/**	DOTO-																							**/
	/*************************************************************************************************/ 
	m_aiExtraSpecialistCommerce.write(pStream);
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/
	m_aiCommerceRateModifier.write(pStream);
	m_aiCommerceHappinessPer.write(pStream);
	m_aiDomainFreeExperience.write(pStream);
	m_aiDomainProductionModifier.write(pStream);
	m_aiCulture.write(pStream);
	m_aiNumRevolts.write(pStream);

	m_abEverOwned.write(pStream);
	m_abTradeRoute.write(pStream);
	m_abRevealed.write(pStream);
	m_abEspionageVisibility.write(pStream);

	pStream->WriteString(m_szName);
	pStream->WriteString(m_szPreviousName); // advc.106k
	pStream->WriteString(m_szScriptData);

	m_aiNoBonus.write(pStream);
	m_aiFreeBonus.write(pStream);
	m_aiNumBonuses.write(pStream);
	// DOTO-< Building Resource Converter Start >
	m_paiBuildingOutputBonuses.write(pStream);
	// DOTO-< Building Resource Converter End   >
	m_aiNumCorpProducedBonuses.write(pStream);
	m_aiProjectProduction.write(pStream);
	m_aiBuildingProduction.write(pStream);
	m_aiBuildingProductionTime.write(pStream);
	m_aeBuildingOriginalOwner.write(pStream);
//tholish-Keldath inactive buildings
	m_aiBuildingeActive.write(pStream);
	m_aiBuildingOriginalTime.write(pStream);
	m_aiUnitProduction.write(pStream);
	m_aiUnitProductionTime.write(pStream);
	m_aiGreatPeopleUnitRate.write(pStream);
	m_aiGreatPeopleUnitProgress.write(pStream);
	m_aiSpecialistCount.write(pStream);
	m_aiMaxSpecialistCount.write(pStream);
	m_aiForceSpecialistCount.write(pStream);
	m_aiFreeSpecialistCount.write(pStream);
	m_aiImprovementFreeSpecialists.write(pStream);
	m_aiReligionInfluence.write(pStream);
	m_aiStateReligionHappiness.write(pStream);
	m_aiUnitCombatFreeExperience.write(pStream);
	m_aiFreePromotionCount.write(pStream);
	m_aiNumRealBuilding.write(pStream);
	m_aiNumFreeBuilding.write(pStream);
//mylon enhanced cities doto advc version
	pStream->Write(m_iRadius);
	pStream->Write(m_eCityPlots);
	//m_abWorkingPlot.write(pStream);
	FOR_EACH_CITYPLOT
	{
		pStream->Write(m_abWorkingPlot[eLoopCityPlot]);
	}
//mylon enhanced cities doto advc version
	m_abHasReligion.write(pStream);
	m_abHasCorporation.write(pStream);

	for (size_t i = 0; i < m_aTradeCities.size(); i++)
	{
		pStream->Write(m_aTradeCities[i].eOwner);
		pStream->Write(m_aTradeCities[i].iID);
	}

	m_orderQueue.Write(pStream);
	/*	These caches are (or ought to be) reliably invalidated,
		don't need to be in-sync at all times. */
	REPRO_TEST_END_WRITE();
	pStream->Write(m_iPopulationRank);
	pStream->Write(m_bPopulationRankValid);
	m_aiBaseYieldRank.write(pStream);
	m_abBaseYieldRankValid.write(pStream);
	m_aiYieldRank.write(pStream);
	m_abYieldRankValid.write(pStream);
	m_aiCommerceRank.write(pStream);
	m_abCommerceRankValid.write(pStream);
	REPRO_TEST_BEGIN_WRITE(CvString::format("City(%d,%d) pt2", getX(), getY()));
	pStream->Write(m_aEventsOccured.size());
	for (std::vector<EventTypes>::iterator it = m_aEventsOccured.begin();
		it != m_aEventsOccured.end(); ++it)
	{
		pStream->Write(*it);
	}

	m_aeiiBuildingYieldChange.write(pStream);
	m_aeiiBuildingCommerceChange.write(pStream);
	m_aeiBuildingHappyChange.write(pStream);
	m_aeiBuildingHealthChange.write(pStream);

	pStream->Write(m_iPopRushHurryCount); // advc.912d
	// <advc.004x>
	pStream->Write(m_iMostRecentOrder);
	pStream->Write(m_bMostRecentUnit); // </advc.004x>
	REPRO_TEST_END_WRITE();
}


class VisibleBuildingComparator
{
public:
	bool operator() (BuildingTypes e1, BuildingTypes e2)
	{
		if (GC.getInfo(e1).getVisibilityPriority() > GC.getInfo(e2).getVisibilityPriority())
			return true;
		else if (GC.getInfo(e1).getVisibilityPriority() == GC.getInfo(e2).getVisibilityPriority())
		{
			//break ties by building type higher building type
			if (e1 > e2)
				return true;
		}
		return false;
	}
};

/*	Fill the kVisible array with buildings that you want shown in city,
	as well as the number of generics
	This function is called whenever CvCity::setLayoutDirty() is called. */
void CvCity::getVisibleBuildings(std::list<BuildingTypes>& kChosenVisible,
	int& iChosenNumGenerics)
{
	// <advc.045>
	TeamTypes const eActiveTeam = GC.getGame().getActiveTeam();
	bool const bAllVisible = (eActiveTeam != NO_TEAM &&
			isAllBuildingsVisible(eActiveTeam, true)); // </advc.045>
	std::vector<BuildingTypes> kVisible;
	CvCivilization const& kCiv = getCivilization();
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes const eBuilding = kCiv.buildingAt(i);
		// <advc.045>
		if(getNumBuilding(eBuilding) <= 0)
			continue;
		// Copied these two checks from Rise of Mankind
		CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
		bool const bWonder = kBuilding.isLimited();
		bool const bDefense = (kBuilding.getDefenseModifier() > 0 ||
				kBuilding.get(CvBuildingInfo::RaiseDefense) > 0); // advc.004c
		if (!bAllVisible && !bWonder && !bDefense)
		{
			bool bVisibleYieldChange = false;
			YieldChangeMap aiSeaPlotYieldChanges = kBuilding.getSeaPlotYieldChange();
			YieldChangeMap aiRiverPlotYieldChanges = kBuilding.getRiverPlotYieldChange();
			FOR_EACH_ENUM(Yield)
			{
				if(aiRiverPlotYieldChanges[eLoopYield] != 0 ||
					aiSeaPlotYieldChanges[eLoopYield] != 0)
				{
					bVisibleYieldChange = true;
					break;
				}
			}
			if(!bVisibleYieldChange)
				continue;
		} // </advc.045>
		kVisible.push_back(eBuilding);
	}

	// sort the visible ones by decreasing priority
	VisibleBuildingComparator kComp;
	std::sort(kVisible.begin(), kVisible.end(), kComp);

	// how big is this city, in terms of buildings?
	// general rule: no more than fPercentUnique percent of a city can be uniques
	int iTotalVisibleBuildings;
	if (::stricmp(GC.getDefineSTRING("GAME_CITY_SIZE_METHOD"), "METHOD_EXPONENTIAL") == 0)
	{
		int iCityScaleMod = (int)(std::pow((float)getPopulation(),
				GC.getDefineFLOAT("GAME_CITY_SIZE_EXP_MODIFIER")) * 2);
		iTotalVisibleBuildings = 10 + iCityScaleMod;
	}
	else
	{
		float fLo = GC.getDefineFLOAT("GAME_CITY_SIZE_LINMAP_AT_0");
		float fHi = GC.getDefineFLOAT("GAME_CITY_SIZE_LINMAP_AT_50");
		iTotalVisibleBuildings = (int)(((fHi - fLo) / 50.0f) * getPopulation() + fLo);
	}
	float fMaxUniquePercent = GC.getDefineFLOAT("GAME_CITY_SIZE_MAX_PERCENT_UNIQUE");
	int iMaxNumUniques = (int)(fMaxUniquePercent * iTotalVisibleBuildings);

	// compute how many buildings are generics vs. unique Civ buildings?
	int const iNumUniques = std::min((int)kVisible.size(), iMaxNumUniques);
	int const iNumGenerics = iTotalVisibleBuildings - iNumUniques + getCitySizeBoost();

	// return
	iChosenNumGenerics = iNumGenerics;
	for(int i = 0; i < iNumUniques; i++)
	{
		kChosenVisible.push_back(kVisible[i]);
	}
}

// advc.045:
bool CvCity::isAllBuildingsVisible(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
		return true;
	if (GC.getDefineBOOL(CvGlobals::TREAT_REVEALED_BUILDINGS_AS_VISIBLE) &&
		isRevealed(eTeam, bDebug))
	{
		return true;
	}
	return (getTeam() == eTeam || getPlot().isInvestigate(eTeam) ||
			getPlot().plotCheck(NULL, -1, -1, NO_PLAYER, eTeam) > 0 != NULL);
}

// (advc: natGetDeterministicRandom deleted; should use 'hash' in CvGameCoreUtils instead.)

/*	Fill the kEffectNames array with references to effects in CIV4EffectInfos.xml
	to have a city play a given set of effects. This is called whenever the interface
	updates the city billboard or when the zoom level changes. */
void CvCity::getVisibleEffects(ZoomLevelTypes eCurZoom,
	std::vector<const TCHAR*>& kEffectNames)
{
	if (isOccupation() && isVisible(getTeam()) == true)
	{
		if (eCurZoom  == ZOOM_DETAIL)
		{
			kEffectNames.push_back("EFFECT_CITY_BIG_BURNING_SMOKE");
			kEffectNames.push_back("EFFECT_CITY_FIRE");
		}
		else kEffectNames.push_back("EFFECT_CITY_BIG_BURNING_SMOKE");
		return;
	}

	if (isActiveTeam() || GC.getGame().isDebugMode())
	{
		if (angryPopulation() > 0)
			kEffectNames.push_back("EFFECT_CITY_BURNING_SMOKE");

		if (healthRate() < 0)
			kEffectNames.push_back("EFFECT_CITY_DISEASED");

		if (isWeLoveTheKingDay())
		{	// <advc> Cut from deleted free function natGetDeterministicRandom
			srand(7297 * getX() + 2909  * getY());
			int iSeed = (rand() % 32767); // </advc>
			CvRandom kRand;
			kRand.init(iSeed);

			// fireworks
			static TCHAR const* szFireworkEffects[] = // advc: static
			{
				"FIREWORKS_RED_LARGE_SLOW",
				"FIREWORKS_RED_SMALL_FAST",
				"FIREWORKS_GREEN_LARGE_SLOW",
				"FIREWORKS_GREEN_SMALL_FAST",
				"FIREWORKS_PURPLE_LARGE_SLOW",
				"FIREWORKS_PURPLE_SMALL_FAST",
				"FIREWORKS_YELLOW_LARGE_SLOW",
				"FIREWORKS_YELLOW_SMALL_FAST",
				"FIREWORKS_BLUE_LARGE_SLOW",
				"FIREWORKS_BLUE_SMALL_FAST"
			};
			int const iNumEffects = ARRAYSIZE(szFireworkEffects);
			for (int i = 0; i < (iNumEffects < 3 ? iNumEffects : 3); i++)
			{
				kEffectNames.push_back(szFireworkEffects[kRand.get(iNumEffects)]);
			}
		}
	}
}


void CvCity::getCityBillboardSizeIconColors(NiColorA& kDotColor, NiColorA& kTextColor) const
{
	PlayerColorTypes const ePlayerColor = //GET_PLAYER(getOwner()).getPlayerColor())
		/*  advc.001: CvPlayer::getPlayerColor will return the Barbarian color
			if the city owner hasn't been met (city revealed through map trade) */
			GC.getInitCore().getColor(getOwner());
	NiColorA kPlayerColor = GC.getInfo(GC.getInfo(ePlayerColor).
			getColorTypePrimary()).getColor();
	NiColorA kGrowing(0.73f, 1,0.73f, 1);
	NiColorA kShrinking(1, 0.73f, 0.73f, 1);
	NiColorA kStagnant(0.83f, 0.83f, 0.83f, 1);
	NiColorA kUnknown(.5f, .5f, .5f, 1);
	NiColorA kWhite(1, 1, 1, 1);
	NiColorA kBlack(0, 0, 0, 1);

	if (isActiveTeam() /* advc.127: */ && isHuman())
	{
		kTextColor = kBlack;
		int const iFoodDifference = foodDifference();
		int const iFood = getFood();
		int const iGrowthThreshold = growthThreshold();
		/* DOTO-Population Limit ModComp - Beginning */
		if (iFoodDifference < 0 && getPopulation() < getPopulationLimit())
		{
			if (iFoodDifference == -1 && iFood * 100 >= 75 * iGrowthThreshold)
				kDotColor = kStagnant;
			else kDotColor = kShrinking;
		}
		/* DOTO-Population Limit ModComp  */
		else if (iFoodDifference > 0 && getPopulation() < getPopulationLimit())
		{	// <advc.002f>
			if (AI().AI_isEmphasizeAvoidGrowth())
			{
				if (iFood + iFoodDifference >= iGrowthThreshold) // Food being wasted
					kDotColor = kShrinking;
				else kDotColor = kStagnant; // Food will be wasted - and not growing
			} // </advc.002f>
			else kDotColor = kGrowing;
		}
		/* DOTO-Population Limit ModComp  */
		else if (iFoodDifference == 0 || getPopulation() >= getPopulationLimit())
		{
			kDotColor = kStagnant;
			kTextColor = kBlack;
		}
		else kDotColor = kStagnant;
	}
	else
	{
		kDotColor = kPlayerColor;
		NiColorA kPlayerSecondaryColor = GC.getInfo(
				GC.getInfo(ePlayerColor). // advc.001
				getColorTypeSecondary()).getColor();
		kTextColor = kPlayerSecondaryColor;
	}
}


const TCHAR* CvCity::getCityBillboardProductionIcon() const
{
	if (!canBeSelected() || !isProduction())
	{
		return ARTFILEMGR.getInterfaceArtInfo(
				"INTERFACE_BUTTONS_NOPRODUCTION")->getPath();
	}
	CLLNode<OrderData>* pOrderNode = headOrderQueueNode();
	TCHAR const* szIcon = NULL;
	switch (pOrderNode->m_data.eOrderType)
	{
	case ORDER_TRAIN:
	{
		UnitTypes eType = getProductionUnit();
		szIcon = GET_PLAYER(getOwner()).getUnitButton(eType);
		break;
	}
	case ORDER_CONSTRUCT:
	{
		BuildingTypes eType = getProductionBuilding();
		szIcon = GC.getInfo(eType).getButton();
		break;
	}
	case ORDER_CREATE:
	{
		ProjectTypes eType = getProductionProject();
		szIcon = GC.getInfo(eType).getButton();
		break;
	}
	case ORDER_MAINTAIN:
	{
		ProcessTypes eType = getProductionProcess();
		szIcon = GC.getInfo(eType).getButton();
		break;
	}
	default: FAssert(false);
	}
	return szIcon;
}


bool CvCity::getFoodBarPercentages(std::vector<float>& afPercentages) const
{
	if (!canBeSelected())
		return false;

	afPercentages.resize(NUM_INFOBAR_TYPES, 0.0f);
	if (foodDifference() < 0)
	{
		afPercentages[INFOBAR_STORED] = std::max(0, getFood() + foodDifference()) /
				(float)growthThreshold();
		afPercentages[INFOBAR_RATE_EXTRA] = std::min(-foodDifference(), getFood()) /
				(float)growthThreshold();
	}
	else
	{
		afPercentages[INFOBAR_STORED] = getFood() / (float) growthThreshold();
		afPercentages[INFOBAR_RATE] = foodDifference() / (float) growthThreshold();
	}
	return true;
}


bool CvCity::getProductionBarPercentages(std::vector<float>& afPercentages) const
{
	if (!canBeSelected())
		return false;

	if (!isProductionProcess())
	{
		afPercentages.resize(NUM_INFOBAR_TYPES, 0.0f);
		int iProductionDiffNoFood = getCurrentProductionDifference(true, true);
		int iProductionDiffJustFood = getCurrentProductionDifference(false, true)
				- iProductionDiffNoFood;
		afPercentages[INFOBAR_STORED] = getProduction() / (float) getProductionNeeded();
		afPercentages[INFOBAR_RATE] = iProductionDiffNoFood / (float) getProductionNeeded();
		afPercentages[INFOBAR_RATE_EXTRA] = iProductionDiffJustFood / (float) getProductionNeeded();
	}

	return true;
}


NiColorA CvCity::getBarBackgroundColor() const
{
	if (::atWar(getTeam(), GC.getGame().getActiveTeam()))
		return NiColorA(0.5f, 0, 0, 0.5f); // red
	return NiColorA(0, 0, 0, 0.5f);
}


bool CvCity::isStarCity() const
{
	return isCapital();
}


int CvCity::getTriggerValue(EventTriggerTypes eTrigger) const
{
	if (!GC.getPythonCaller()->canTriggerEvent(*this, eTrigger))
		return MIN_INT;

	CvEventTriggerInfo& kTrigger = GC.getInfo(eTrigger);

	if (kTrigger.getNumBuildings() > 0 && kTrigger.getNumBuildingsRequired() > 0)
	{
		bool bFoundValid = false;
		for (int i = 0; i < kTrigger.getNumBuildingsRequired(); i++)
		{
			BuildingClassTypes eBuildingClass = (BuildingClassTypes)
					kTrigger.getBuildingRequired(i);
			if (eBuildingClass != NO_BUILDINGCLASS &&
				getNumRealBuilding(eBuildingClass) > 0) // advc.003w: simplified
			{
				bFoundValid = true;
				break;
			}
		}
		if (!bFoundValid)
			return MIN_INT;
	}


	if (getReligionCount() < kTrigger.getNumReligions())
		return MIN_INT;

	if (kTrigger.getNumReligions() > 0 && kTrigger.getNumReligionsRequired() > 0)
	{
		bool bFoundValid = false;
		for (int i = 0; i < kTrigger.getNumReligionsRequired(); i++)
		{
			if (!kTrigger.isStateReligion() ||
				kTrigger.getReligionRequired(i) == GET_PLAYER(getOwner()).getStateReligion())
			{
				ReligionTypes eReligion = (ReligionTypes)kTrigger.getReligionRequired(i);
				if (isHasReligion(eReligion))
				{
					if (!kTrigger.isHolyCity() || isHolyCity(eReligion))
						bFoundValid = true;
				}
			}
		}
		if (!bFoundValid)
			return MIN_INT;
	}

	if (getCorporationCount() < kTrigger.getNumCorporations())
		return MIN_INT;

	if (kTrigger.getNumCorporations() > 0 && kTrigger.getNumCorporationsRequired() > 0)
	{
		bool bFoundValid = false;
		for (int i = 0; i < kTrigger.getNumCorporationsRequired(); i++)
		{
			CorporationTypes eCorp = (CorporationTypes)kTrigger.getCorporationRequired(i);
			if (isHasCorporation(eCorp))
			{
				if (!kTrigger.isHeadquarters() || isHeadquarters(eCorp))
					bFoundValid = true;
			}
		}
		if (!bFoundValid)
			return MIN_INT;
	}

	if (kTrigger.getMinPopulation() > 0)
	{
		if (getPopulation() < kTrigger.getMinPopulation())
			return MIN_INT;
	}

	if (kTrigger.getMaxPopulation() > 0)
	{
		if (getPopulation() > kTrigger.getMaxPopulation())
			return MIN_INT;
	}

	if (kTrigger.getAngry() > 0)
	{
		if (unhappyLevel(0) - happyLevel() < kTrigger.getAngry())
			return MIN_INT;
	}
	else if (kTrigger.getAngry() < 0)
	{
		if (happyLevel() - unhappyLevel(0) < -kTrigger.getAngry())
			return MIN_INT;
	}

	if (kTrigger.getUnhealthy() > 0)
	{
		if (badHealth(false, 0) - goodHealth() < kTrigger.getUnhealthy())
		{
			return MIN_INT;
		}
	}
	else if (kTrigger.getUnhealthy() < 0)
	{
		if (goodHealth() - badHealth(false, 0) < -kTrigger.getUnhealthy())
			return MIN_INT;
	}

	if (kTrigger.isPrereqEventCity() && kTrigger.getNumPrereqEvents() > 0)
	{
		bool bFoundValid = true;

		for (int i = 0; i < kTrigger.getNumPrereqEvents(); i++)
		{
			if (!isEventOccured((EventTypes)kTrigger.getPrereqEvent(i)))
			{
				bFoundValid = false;
				break;
			}
		}
		if (!bFoundValid)
			return MIN_INT;
	}

	if (getFood() == 0 && kTrigger.getCityFoodWeight() > 0)
		return MIN_INT;
	return getFood() * kTrigger.getCityFoodWeight();
}


bool CvCity::canApplyEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const
{
	CvEventInfo& kEvent = GC.getInfo(eEvent);
	if (!kEvent.isCityEffect() && !kEvent.isOtherPlayerCityEffect())
		return true;

	if (kTriggeredData.m_iCityId == -1 && kEvent.isCityEffect())
		return false;

	if (kTriggeredData.m_iOtherPlayerCityId  == -1 && kEvent.isOtherPlayerCityEffect())
		return false;

	if (kEvent.getFood() + ((100 + kEvent.getFoodPercent()) * getFood()) / 100 < 0)
		return false;

	if (kEvent.getPopulationChange() + getPopulation() <= 0)
		return false;

	if (100 * kEvent.getCulture() + getCultureTimes100(getOwner()) < 0)
		return false;

	BuildingClassTypes const eBuildingClass = (BuildingClassTypes)kEvent.getBuildingClass();
	if (eBuildingClass != NO_BUILDINGCLASS)
	{
		BuildingTypes eBuilding = getCivilization().getBuilding(eBuildingClass);
		if (eBuilding == NO_BUILDING)
			return false;

		if (kEvent.getBuildingChange() > 0)
		{
			if (getNumBuilding(eBuilding) >= GC.getDefineINT(CvGlobals::CITY_MAX_NUM_BUILDINGS))
				return false;
		}
		else if (kEvent.getBuildingChange() < 0)
		{
			if (getNumRealBuilding(eBuilding) + kEvent.getBuildingChange() < 0)
				return false;
		}
	}

	if (kEvent.getMaxNumReligions() != -1 &&
		getReligionCount() > kEvent.getMaxNumReligions())
	{
		return false;
	}

	if (kEvent.getMinPillage() > 0)
	{
		int iNumImprovements = 0;
		for (CityPlotIter it(*this, false); it.hasNext(); ++it)
		{
			if (it->getOwner() == getOwner() && it->isImproved() &&
				!GC.getInfo(it->getImprovementType()).isPermanent())
			{
				iNumImprovements++;
			}
		}
		if (iNumImprovements < kEvent.getMinPillage())
			return false;
	}

	return true;
}


void CvCity::applyEvent(EventTypes eEvent,
	EventTriggeredData const& kTriggeredData, bool bClear)
{
	if (!canApplyEvent(eEvent, kTriggeredData))
		return;

	setEventOccured(eEvent, true);
	CvEventInfo const& kEvent = GC.getInfo(eEvent);
	if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
	{
		if (kEvent.getHappy() != 0)
			changeExtraHappiness(kEvent.getHappy());

		if (kEvent.getHealth() != 0)
			changeExtraHealth(kEvent.getHealth());

		if (kEvent.getHurryAnger() != 0)
			changeHurryAngerTimer(kEvent.getHurryAnger() * flatHurryAngerLength());

		if (kEvent.getHappyTurns() != 0)
			changeHappinessTimer(kEvent.getHappyTurns());

		if (kEvent.getFood() != 0 || kEvent.getFoodPercent() != 0)
			changeFood(kEvent.getFood() + (kEvent.getFoodPercent() * getFood()) / 100);

		if (kEvent.getPopulationChange() != 0)
			changePopulation(kEvent.getPopulationChange());

		if (kEvent.getRevoltTurns() > 0)
		{
			changeCultureUpdateTimer(kEvent.getRevoltTurns());
			changeOccupationTimer(kEvent.getRevoltTurns());
		}

		if (kEvent.getSpaceProductionModifier() != 0)
			changeSpaceProductionModifier(kEvent.getSpaceProductionModifier());

		if (kEvent.getMaxPillage() > 0)
		{
			FAssert(kEvent.getMaxPillage() >= kEvent.getMinPillage());
			int iNumPillage = kEvent.getMinPillage() +
					SyncRandNum(kEvent.getMaxPillage() - kEvent.getMinPillage());

			int iNumPillaged = 0;

			for (int i = 0; i < iNumPillage; i++)
			{
				int iRandOffset = SyncRandNum(NUM_CITY_PLOTS);
				//FOR_EACH_ENUM(CityPlot)
//mylon enhanced cities doto advc version
				FOR_EACH_CITYPLOT
				{
					CityPlotTypes const ePlot = (CityPlotTypes)
							((eLoopCityPlot + iRandOffset) % NUM_CITY_PLOTS);
					if (ePlot == CITY_HOME_PLOT)
						continue;
					CvPlot* pPlot = getCityIndexPlot(ePlot);
					if (pPlot == NULL || pPlot->getOwner() != getOwner())
						continue;
					if (pPlot->isImproved() &&
						!GC.getInfo(pPlot->getImprovementType()).isPermanent())
					{
						CvWString szBuffer = gDLL->getText(
								"TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED",
								GC.getInfo(pPlot->getImprovementType()).getTextKeyWide());
						gDLL->UI().addMessage(getOwner(), false, -1, szBuffer, *pPlot,
								"AS2D_PILLAGED", MESSAGE_TYPE_INFO,
								GC.getInfo(pPlot->getImprovementType()).getButton(),
								GC.getColorType("RED"));
						pPlot->setImprovementType(NO_IMPROVEMENT);
						iNumPillaged++;
						break;
					}
				}
			}

			PlayerTypes eOtherPlayer = kTriggeredData.m_eOtherPlayer;
			if (!kEvent.isCityEffect() && kEvent.isOtherPlayerCityEffect())
				eOtherPlayer = kTriggeredData.m_ePlayer;

			if (eOtherPlayer != NO_PLAYER)
			{
				CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_NUM_CITY_IMPROVEMENTS_DESTROYED",
						iNumPillaged, GET_PLAYER(getOwner()).getCivilizationAdjectiveKey());
				gDLL->UI().addMessage(eOtherPlayer, false, -1, szBuffer,
						"AS2D_PILLAGED", MESSAGE_TYPE_INFO);
			}
		}

		FOR_EACH_ENUM(Specialist)
		{
			if (kEvent.getFreeSpecialistCount(eLoopSpecialist) > 0)
			{
				changeFreeSpecialistCount(eLoopSpecialist,
						kEvent.getFreeSpecialistCount(eLoopSpecialist));
			}
		}
		if (kEvent.getCulture() != 0)
		{
			changeCulture(getOwner(), kEvent.getCulture(), true, true);
//KNOEDELbegin cultural_golden_age 2/2
			if (GC.getGame().isOption(GAMEOPTION_CULTURE_GOLDEN_AGE))
			{
				GET_PLAYER(getOwner()).changeCultureGoldenAgeProgress(kEvent.getCulture());	//KNOEDEL
			}
//KNOEDELbegin cultural_golden_age 2/2		
		}
	}
	{
		UnitClassTypes eUnitClass = (UnitClassTypes)kEvent.getUnitClass(); // advc
		if (eUnitClass != NO_UNITCLASS)
		{
			UnitTypes eUnit = getCivilization().getUnit(eUnitClass);
			if (eUnit != NO_UNIT)
			{
				for (int i = 0; i < kEvent.getNumUnits(); i++)
				{
					GET_PLAYER(getOwner()).initUnit(eUnit, getX(), getY());
				}
			}
		}
	}
	{
		BuildingClassTypes eBuildingClass = (BuildingClassTypes)kEvent.getBuildingClass(); // advc
		if (eBuildingClass != NO_BUILDINGCLASS)
		{
			BuildingTypes eBuilding = getCivilization().getBuilding(eBuildingClass);
			if (eBuilding != NO_BUILDING)
			{
				if (kEvent.getBuildingChange() != 0)
				{
					setNumRealBuilding(eBuilding, getNumRealBuilding(eBuilding) +
							kEvent.getBuildingChange());
				}
			}
		}
	}
	FOR_EACH_NON_DEFAULT_PAIR(kEvent.
		getBuildingYieldChange(), BuildingClass, YieldChangeMap)
	{
		FOR_EACH_NON_DEFAULT_PAIR(perBuildingClassVal.second, Yield, int)
		{
			changeBuildingYieldChange(perBuildingClassVal.first,
					perYieldVal.first, perYieldVal.second);
		}
	}
	FOR_EACH_NON_DEFAULT_PAIR(kEvent.
		getBuildingCommerceChange(), BuildingClass, CommerceChangeMap)
	{
		FOR_EACH_NON_DEFAULT_PAIR(perBuildingClassVal.second, Commerce, int)
		{
			changeBuildingCommerceChange(perBuildingClassVal.first,
					perCommerceVal.first, perCommerceVal.second);
		}
	}
	FOR_EACH_NON_DEFAULT_PAIR(kEvent.getBuildingHappyChange(), BuildingClass, int)
	{
		setBuildingHappyChange(perBuildingClassVal.first, perBuildingClassVal.second);
	}
	FOR_EACH_NON_DEFAULT_PAIR(kEvent.getBuildingHealthChange(), BuildingClass, int)
	{
		setBuildingHealthChange(perBuildingClassVal.first, perBuildingClassVal.second);
	}

	if (bClear)
	{
		FOR_EACH_ENUM(Event)
			setEventOccured(eLoopEvent, false);
	}
}


bool CvCity::isEventOccured(EventTypes eEvent) const
{
	for (size_t i = 0; i < m_aEventsOccured.size(); i++)
	{
		if (m_aEventsOccured[i] == eEvent)
			return true;
	}
	return false;
}


void CvCity::setEventOccured(EventTypes eEvent, bool bOccured)
{
	for (std::vector<EventTypes>::iterator it = m_aEventsOccured.begin();
		it != m_aEventsOccured.end(); ++it)
	{
		if (*it == eEvent)
		{
			if (!bOccured)
				m_aEventsOccured.erase(it);
			return;
		}
	}
	if (bOccured)
		m_aEventsOccured.push_back(eEvent);
}

// advc.003y: Ported from CvEventManager.py; Python code there deleted.
void CvCity::doPartisans()
{
	if (GC.getGame().isOption(GAMEOPTION_NO_EVENTS))
		return;
	/*	(advc.001: The population check in CvEventManager had had no effect b/c of a typo.
		Note that population loss from conquest will have already occurred when razing.) */
	if (getPopulation() <= 1 || isBarbarian())
		return;
	// advc: Was based on city culture instead of plot culture
	PlayerTypes eHighestCulturePlayer = getPlot().findHighestCulturePlayer();
	if (eHighestCulturePlayer == getOwner() || eHighestCulturePlayer == BARBARIAN_PLAYER)
		return;
	CvPlayer& kPartisanPlayer = GET_PLAYER(eHighestCulturePlayer);
	// advc.099: alive check
	if (!kPartisanPlayer.isAlive() || kPartisanPlayer.getNumCities() <= 0)
		return;
	if (!::atWar(getTeam(), kPartisanPlayer.getTeam()))
		return;
	// advc: Important now that getNumPartisanUnits subtracts 1 from the culture level
	if (getNumPartisanUnits(kPartisanPlayer.getID()) <= 0)
		return;

	EventTriggerTypes eTrigger = (EventTriggerTypes)GC.getInfoTypeForString("EVENTTRIGGER_PARTISANS");
	FAssert(eTrigger != NO_EVENTTRIGGER);
	if (!GC.getGame().isEventActive(eTrigger) ||
		/*  Non-negative probability means, apparently, that the event is
			not supposed to be triggered manually like this. */
		kPartisanPlayer.getEventTriggerWeight(eTrigger) >= 0)
	{
		return;
	}
	kPartisanPlayer.initTriggeredData(eTrigger, /*bFire=*/true, -1, getX(), getY(), getOwner(), getID());
}

/*	advc.003y: Ported from CvRandomEventInterface.py; Python code there deleted.
	Subtracting 1 is an idea by CFC user SmokeyTheBear. It means e.g. that 0 units
	appear when culture is "poor". */
int CvCity::getNumPartisanUnits(PlayerTypes ePartisanPlayer) const
{
	// advc.001: The culture level computation in Python was erroneous
	return std::max(0, calculateCultureLevel(ePartisanPlayer) - 1);
}


bool CvCity::hasShrine(ReligionTypes eReligion) const
{
	bool bHasShrine = false;

	// note, for normal XML, this count will be one, there is only one shrine of each religion
	int	shrineBuildingCount = GC.getGame().getShrineBuildingCount(eReligion);
	for (int i = 0; i < shrineBuildingCount; i++)
	{
		BuildingTypes eBuilding = GC.getGame().getShrineBuilding(i, eReligion);
		if (getNumBuilding(eBuilding) > 0)
		{
			bHasShrine = true;
			break;
		}
	}
	return bHasShrine;
}


void CvCity::invalidateYieldRankCache(YieldTypes eYield)
{
	if (eYield == NO_YIELD)
	{
		FOR_EACH_ENUM(Yield)
		{
			m_abBaseYieldRankValid.set(eLoopYield, false);
			m_abYieldRankValid.set(eLoopYield, false);
		}
	}
	else
	{
		m_abBaseYieldRankValid.set(eYield, false);
		m_abYieldRankValid.set(eYield, false);
	}
}


void CvCity::invalidateCommerceRankCache(CommerceTypes eCommerce)
{
	if (eCommerce == NO_COMMERCE)
	{
		FOR_EACH_ENUM(Commerce)
			m_abCommerceRankValid.set(eLoopCommerce, false);
	}
	else m_abCommerceRankValid.set(eCommerce, false);
}


void CvCity::setBuildingYieldChange(BuildingClassTypes eBuildingClass,
	YieldTypes eYield, int iChange)
{
	BuildingTypes eBuilding = getCivilization().getBuilding(eBuildingClass); // advc.003w
	// advc: Simplified this function a lot
	if (eBuilding == NO_BUILDING)
		return;
	int const iOldChange = m_aeiiBuildingYieldChange.get(eBuildingClass, eYield);
	if (iOldChange == iChange)
		return;
	m_aeiiBuildingYieldChange.set(eBuildingClass, eYield, iChange);
	if (getNumActiveBuilding(eBuilding) > 0)
	{
		changeBaseYieldRate(eYield,
				(iChange - iOldChange) * getNumActiveBuilding(eBuilding));
	}
}


void CvCity::changeBuildingYieldChange(BuildingClassTypes eBuildingClass,
	YieldTypes eYield, int iChange)
{
	setBuildingYieldChange(eBuildingClass, eYield,
			getBuildingYieldChange(eBuildingClass, eYield) + iChange);
}


void CvCity::setBuildingCommerceChange(BuildingClassTypes eBuildingClass,
	CommerceTypes eCommerce, int iChange)
{	// advc: Simplified this function a lot
	BuildingTypes eBuilding = getCivilization().getBuilding(eBuildingClass);
	if (eBuilding == NO_BUILDING)
		return;
	int const iOldChange = m_aeiiBuildingCommerceChange.get(eBuildingClass, eCommerce);
	if (iOldChange == iChange)
		return;
	m_aeiiBuildingCommerceChange.set(eBuildingClass, eCommerce, iChange);
	if (getNumActiveBuilding(eBuilding) > 0)
		updateBuildingCommerce(/* advc.opt: */ eCommerce);
}


void CvCity::changeBuildingCommerceChange(BuildingClassTypes eBuildingClass,
	CommerceTypes eCommerce, int iChange)
{
	setBuildingCommerceChange(eBuildingClass, eCommerce,
			getBuildingCommerceChange(eBuildingClass, eCommerce) + iChange);
}


void CvCity::setBuildingHappyChange(BuildingClassTypes eBuildingClass, int iChange)
{
	BuildingTypes eBuilding = getCivilization().getBuilding(eBuildingClass); // advc.003w
	// advc: Simplified this function a lot
	int const iOldChange = m_aeiBuildingHappyChange.get(eBuildingClass);
	if (iOldChange == iChange)
		return;
	m_aeiBuildingHappyChange.set(eBuildingClass, iChange);
	/*	UNOFFICIAL_PATCH (Bugfix), 10/22/09, jdog5000: START
		(advc: BtS code deleted) */
	if (getNumActiveBuilding(eBuilding) > 0)
	{
		if (iOldChange > 0)
			changeBuildingGoodHappiness(-iOldChange);
		else if (iOldChange < 0)
			changeBuildingBadHappiness(-iOldChange);
		if (iChange > 0)
			changeBuildingGoodHappiness(iChange);
		else if (iChange < 0)
			changeBuildingBadHappiness(iChange);
	} // UNOFFICIAL_PATCH: END
}


void CvCity::setBuildingHealthChange(BuildingClassTypes eBuildingClass, int iChange)
{
	BuildingTypes eBuilding = getCivilization().getBuilding(eBuildingClass); // advc.003w
	// advc: Simplified this function a lot
	int const iOldChange = m_aeiBuildingHealthChange.get(eBuildingClass);
	if (iOldChange == iChange)
		return;
	m_aeiBuildingHealthChange.set(eBuildingClass, iChange);
	/*	UNOFFICIAL_PATCH (Bugfix), 10/22/09, jdog5000: START
		(advc: BtS code deleted) */
	if (getNumActiveBuilding(eBuilding) > 0)
	{
		if (iOldChange > 0)
			changeBuildingGoodHealth(-iOldChange);
		else if (iOldChange < 0)
			changeBuildingBadHealth(-iOldChange);
		if (iChange > 0)
			changeBuildingGoodHealth(iChange);
		else if (iChange < 0)
			changeBuildingBadHealth(iChange);
	} // UNOFFICIAL_PATCH: END
}

// advc.071:
void CvCity::meetNewOwner(TeamTypes eOtherTeam, TeamTypes eNewOwner) const
{
	if (!isRevealed(eOtherTeam) || GET_TEAM(eOtherTeam).isHasMet(eNewOwner))
		return;
	FirstContactData fcData;
	fcData.x1 = getX();
	fcData.y1 = getY();
	GET_TEAM(eOtherTeam).meet(eNewOwner, true, &fcData);
}


void CvCity::liberate(bool bConquest, /* advc.ctr: */ bool bPeaceDeal)
{
	PlayerTypes ePlayer = getLiberationPlayer(bConquest);
	if(ePlayer == NO_PLAYER)
		return; // advc
	// kekm.23: No longer used
	/*CvPlot* pPlot = plot();
	int iOldOwnerCulture = getCultureTimes100(getOwner());
	bool bPreviouslyOwned = isEverOwned(ePlayer);*/ // K-Mod, for use below
	int iOldMasterLand = 0;
	int iOldVassalLand = 0;
	CvTeam& kLiberationTeam = GET_TEAM(ePlayer); // advc

	if (kLiberationTeam.isVassal(getTeam()))
	{
		iOldMasterLand = GET_TEAM(getTeam()).getTotalLand();
		iOldVassalLand = std::max(10, // advc.112: Lower bound added
				kLiberationTeam.getTotalLand(false));
	}

	CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_LIBERATED", getNameKey(),
			GET_PLAYER(getOwner()).getNameKey(),
			GET_PLAYER(ePlayer).getCivilizationAdjectiveKey());
	for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
	{
		CvPlayer const& kObs = *it;
		// advc.071: Meet before the announcement (with indicator at city coordinates)
		meetNewOwner(kObs.getTeam(), TEAMID(ePlayer));
		if (isRevealed(kObs.getTeam()) /* advc.127: */ || kObs.isSpectator())
		{
			gDLL->UI().addMessage(kObs.getID(), false, -1, szBuffer, getPlot(),
					"AS2D_REVOLTEND",
					MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY, // advc.106b
					ARTFILEMGR.getInterfaceArtPath("WORLDBUILDER_CITY_EDIT"),
					GC.getColorType("HIGHLIGHT_TEXT"));
		}
	}
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwner(), szBuffer,
			getX(), getY(), GC.getColorType("HIGHLIGHT_TEXT"));
	// <advc.ctr>
	if (!bPeaceDeal)
		GET_PLAYER(ePlayer).AI_rememberLiberation(*this, bConquest); // </advc.ctr>
	GET_PLAYER(ePlayer).acquireCity(this, false, true, true);

	if (kLiberationTeam.isVassal(getTeam()))
	{
		int iNewMasterLand = GET_TEAM(getTeam()).getTotalLand();
		int iNewVassalLand = std::max(10, // advc.112: Lower bound added
				kLiberationTeam.getTotalLand(false));

		kLiberationTeam.setMasterPower(kLiberationTeam.getMasterPower() +
				iNewMasterLand - iOldMasterLand);
		kLiberationTeam.setVassalPower(kLiberationTeam.getVassalPower() +
				iNewVassalLand - iOldVassalLand);
	}
	GET_PLAYER(ePlayer).AI_updateAttitude(getOwner()); // advc.ctr
	// kekm.23: setCulture now done by advc.ctr in acquireCity
	/*if (NULL != pPlot) {
		CvCity* pCity = pPlot->getPlotCity();
		if (NULL != pCity) {
			// K-Mod, 7/jan/11:
			// This mechanic was exploitable. Players could increase their culture indefinitely in a single turn by gifting cities backwards and forwards.
			// I've attempted to close the exploit.
			if (!bPreviouslyOwned)
				pCity->setCultureTimes100(ePlayer, pCity->getCultureTimes100(ePlayer) + iOldOwnerCulture / 2, true, true);
			// K-Mod end
		}
		if (kLiberationTeam.isAVassal()) {
			for (int i = 0; i < GC.getDefineINT("COLONY_NUM_FREE_DEFENDERS"); ++i)
				pCity->initConscriptedUnit();
		}
	}*/
}


PlayerTypes CvCity::getLiberationPlayer(bool bConquest) const  // advc: refactoring changes
{
	PROFILE_FUNC(); // advc: Fine so far; not frequently called.
	if (isCapital())
		return NO_PLAYER;

	CvPlayer const& kOwner = GET_PLAYER(getOwner());

	for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
	{
		CvPlayer& kLoopPlayer = *it;
		if (kLoopPlayer.getParent() == kOwner.getID())
		{
			if (kLoopPlayer.hasCapital())
			{
				if (sameArea(*kLoopPlayer.getCapital()))
					return kLoopPlayer.getID();
			}
		}
	}

	// advc: This is pointless b/c the splitEmpirePlayer is never alive
	/*if (kOwner.canSplitEmpire() && kOwner.canSplitArea(getArea())) {
		PlayerTypes eSplitPlayer = kOwner.getSplitEmpirePlayer(getArea());
		if (eSplitPlayer != NO_PLAYER && GET_PLAYER(eSplitPlayer).isAlive())
			return eSplitPlayer;
	}*/

	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestValue = 0;
	int const iTotalCityCultureTimes100 = countTotalCultureTimes100();
	// K-Mod - Base culture which is added to dilute the true culture values
	int const iBaseCulture = (GC.getNumCultureLevelInfos() <= 1 ? 100 :
			50 * GC.getInfo((CultureLevelTypes)1).getSpeedThreshold(GC.getGame().getGameSpeedType()));
	// K-Mod end

	// advc.ctr (comment): I guess liberation to a minor civ would be OK
	for (PlayerIter<CIV_ALIVE> it; it.hasNext(); ++it)
	{
		CvPlayer const& kLoopPlayer = *it;
		// K-Mod. I've flattened the if blocks into if! continue conditions.
		if (!kLoopPlayer.canReceiveTradeCity())
			continue;
		/*  <advc.ctr> The iBaseCulture term added by K-Mod had allowed liberation
			at culture level 0, i.e. to players who never owned the city. */
		if (!isEverOwned(kLoopPlayer.getID()))
			continue; // </advc.ctr>
		int iCapitalDistance = -1;
		{
			CvCity* pCapital = kLoopPlayer.getCapital();
			if (pCapital == NULL)
				continue;
			iCapitalDistance = ::plotDistance(getX(), getY(),
					pCapital->getX(), pCapital->getY());
			if (!sameArea(*pCapital))
				iCapitalDistance *= 2;
		}
		int iCultureScore = getCultureTimes100(kLoopPlayer.getID()) /* K-Mod: */ + iBaseCulture;
		if (bConquest && kLoopPlayer.getID() == getOriginalOwner())
		{
			iCultureScore *= 3;
			iCultureScore /= 2;
		}
		if (kLoopPlayer.getTeam() == getTeam() ||
			GET_TEAM(kLoopPlayer.getTeam()).isVassal(getTeam()) ||
			GET_TEAM(getTeam()).isVassal(kLoopPlayer.getTeam()))
		{
			// K-Mod: I don't see why the total culture should be used in this way. (I haven't changed anything)
			/*iCultureTimes100 *= 2;
			iCultureTimes100 = (iCultureTimes100 + iTotalCultureTimes100) / 2;*/
			/*  advc: Simplified (no functional change). I guess this is a sort of
				50% bonus for non-rivals. */
			iCultureScore += iTotalCityCultureTimes100 / 2;
		}
		// K-Mod - adjust culture score based on plot ownership.
		iCultureScore *= 100 + getPlot().calculateTeamCulturePercent(kLoopPlayer.getTeam());
		iCultureScore /= 100;
		// K-Mod end

		//int iValue = std::max(100, iCultureTimes100) / std::max(1, iCapitalDistance);
		int iValue = iCultureScore / std::max(1, iCapitalDistance); // K-Mod (minimum culture moved higher up)
		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			eBestPlayer = kLoopPlayer.getID();
		}
	}

	if (eBestPlayer != NO_PLAYER)
	{
		if (kOwner.getID() == eBestPlayer)
			return NO_PLAYER;
	}

	return eBestPlayer;
}

// advc.003j: Unused since karadoc rewrote CvCityAI::AI_yieldValue
/*int CvCity::getBestYieldAvailable(YieldTypes eYield) const
{
	// ... (body deleted)
}*/


bool CvCity::isAutoRaze(/* <advc> */ PlayerTypes eConqueror) const
{
	CvPlayer const& kAssumedOwner = GET_PLAYER(eConqueror == NO_PLAYER ?
			getOwner() : eConqueror);
	if (kAssumedOwner.isOneCityChallenge()) // </advc>
		return true;
	if (GC.getGame().isOption(GAMEOPTION_NO_CITY_RAZING))
		return false;
	if (GC.getGame().getMaxCityElimination() > 0)
		return true;
	if (getHighestPopulation() == 1)
		return true;
	return false;
}


int CvCity::getMusicScriptId() const
{
	bool bHappy = true;
	if (isActiveTeam())
	{
		if (angryPopulation() > 0)
			bHappy = false;
	}
	else
	{
		if (GET_TEAM(GC.getGame().getActiveTeam()).isAtWar(getTeam()))
			bHappy = false;
	}
	CvLeaderHeadInfo const& kLeaderInfo = GC.getInfo(GET_PLAYER(getOwner()).getLeaderType());
	EraTypes eCurEra = GET_PLAYER(getOwner()).getCurrentEra();
	if (bHappy)
		return (kLeaderInfo.getDiploPeaceMusicScriptIds(eCurEra));
	return (kLeaderInfo.getDiploWarMusicScriptIds(eCurEra));
}


int CvCity::getSoundscapeScriptId() const
{
	return	// advc.002q:
			(!BUGOption::isEnabled("CityScreen__CitySoundScapes", true) ? -1 :
			GC.getInfo(GET_PLAYER(getOwner()).getCurrentEra()).
			getCitySoundscapeScriptId(getCitySizeType()));
}


void CvCity::cheat(bool bCtrl, bool bAlt, bool bShift)
{
	//if (gDLL->getChtLvl() > 0)
	if(GC.getGame().isDebugMode()) // advc.007b
	{
		if (bCtrl)
			changeCulture(getOwner(), 10, true, true);
		else if (bShift)
			changePopulation(1);
		else
		{
			popOrder(0, true,
					NONE_CHOOSE, // advc.001x
					false);
		}
	}
}


void CvCity::getBuildQueue(std::vector<std::string>& astrQueue) const
{
	CLLNode<OrderData>* pNode = headOrderQueueNode();
	while (pNode != NULL)
	{
		switch (pNode->m_data.eOrderType)
		{
		case ORDER_TRAIN:
			astrQueue.push_back(GC.getInfo((UnitTypes)pNode->m_data.iData1).getType());
			break;
		case ORDER_CONSTRUCT:
			astrQueue.push_back(GC.getInfo((BuildingTypes)pNode->m_data.iData1).getType());
			break;
		case ORDER_CREATE:
			astrQueue.push_back(GC.getInfo((ProjectTypes)pNode->m_data.iData1).getType());
			break;
		case ORDER_MAINTAIN:
			astrQueue.push_back(GC.getInfo((ProcessTypes)pNode->m_data.iData1).getType());
			break;
		default:
			FAssert(false);
		}
		pNode = nextOrderQueueNode(pNode);
	}
}

// <advc.004b> See CvCity.h
int CvCity::initialPopulation()
{
	return GC.getDefineINT("INITIAL_CITY_POPULATION") +
			GC.getInfo(GC.getGame().getStartEra()).getFreePopulation();
/* DOTO-Population Limit ModComp - Beginning : The new cities can't have a population level higher than the authorized limit */			
	/*return std::min((GC.getDefineINT("INITIAL_CITY_POPULATION") +
			GC.getEraInfo(GC.getGame().getStartEra()).getFreePopulation()), getPopulationLimit());*/
/* DOTO-Population Limit ModComp - End */
}

// advc.004b, advc.104: Parameters added
int CvCity::calculateDistanceMaintenanceTimes100(CvPlot const& kCityPlot,
//DOTO-dpii ADDED LOCAL DISTANCE MODIFER
	PlayerTypes eOwner, int iPopulation, int iLocalDistance)
{
	if(iPopulation < 0)
		iPopulation = initialPopulation();
	/*CvCity* pLoopCity;
	int iWorstCityMaintenance;
	int iBestCapitalMaintenance;
	int iTempMaintenance;
	int iLoop;
	iWorstCityMaintenance = 0;
	iBestCapitalMaintenance = MAX_INT;
	for (pLoopCity = GET_PLAYER(getOwner()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwner()).nextCity(&iLoop))
		iTempMaintenance = 100 * (GC.getDefineINT("MAX_DISTANCE_CITY_MAINTENANCE") *
				plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY()));*/ // BtS
	// <K-Mod> 17/dec/10 - improved efficiency
	int iTempMaintenance = 100 *
			/*	advc (note): MAX_DISTANCE_CITY_MAINTENANCE is named a bit misleadingly.
				It's simply a (multiplicative) factor. The ratio of maintenance distance to
				maxTypicalDistance can be regarded as another factor, normally between 0 and 1.
				Insofar, MAX_DISTANCE_CITY_MAINTENANCE is the maximal result of multiplying
				those two factors. */
			GC.getDefineINT(CvGlobals::MAX_DISTANCE_CITY_MAINTENANCE) *	
			// K-Mod: Moved the search for maintenance distance to a separate function
			calculateMaintenanceDistance(&kCityPlot, eOwner);
	// unaltered bts code
	{
		iTempMaintenance *= (iPopulation + 7);
		iTempMaintenance /= 10;

		//DOTO- dpii ADDED LOCAL DISTANCE MODIFER
		iTempMaintenance *= std::max(0, iLocalDistance + 100);
		iTempMaintenance /= 100;
		//DOTO- dpii 
		iTempMaintenance *= std::max(0, (GET_PLAYER(eOwner).getDistanceMaintenanceModifier() + 100));
		iTempMaintenance /= 100;

		iTempMaintenance *= GC.getInfo(GC.getMap().getWorldSize()).getDistanceMaintenancePercent();
		iTempMaintenance /= 100;

		iTempMaintenance *= GC.getInfo(GET_PLAYER(eOwner).getHandicapType()).getDistanceMaintenancePercent();
		iTempMaintenance /= 100;

		iTempMaintenance /= GC.getMap().maxTypicalDistance(); // advc.140: was maxPlotDistance
	}
	/*iWorstCityMaintenance = std::max(iWorstCityMaintenance, iTempMaintenance);
		if (pLoopCity->isGovernmentCenter())
			iBestCapitalMaintenance = std::min(iBestCapitalMaintenance, iTempMaintenance);
	}
	iTempMaintenance = std::min(iWorstCityMaintenance, iBestCapitalMaintenance);*/ // BtS
	// </K-Mod>
	FAssert(iTempMaintenance >= 0);
	return iTempMaintenance;
}

// K-Mod. new function to help with maintenance calculations
// advc.004b, advc.104: Parameters added
int CvCity::calculateMaintenanceDistance(CvPlot const* pCityPlot, PlayerTypes eOwner)
{
	int iLongestDist = 0;
	int iShortestGovernment = MAX_INT;
	FOR_EACH_CITY(pLoopCity, GET_PLAYER(eOwner))
	{
		int iDist = plotDistance(pCityPlot, pLoopCity->plot());
		iLongestDist = std::max(iLongestDist, iDist);
		if(pLoopCity->isGovernmentCenter())
			iShortestGovernment = std::min(iShortestGovernment, iDist);
	}
	int iR = std::min(iLongestDist, iShortestGovernment);
	// <advc.140> Upper bound added
	int iCap = GC.getMap().maxMaintenanceDistance();
	return std::min(iR, iCap); // </advc.140>
}

// advc.004b, advc.104: Parameters added
int CvCity::calculateNumCitiesMaintenanceTimes100(
	CvPlot const& kCityPlot, // (unused - not relevant for NumCitiesMaintenance)
	PlayerTypes eOwner, int iPopulation, int iExtraCities)
{
	if(iPopulation < 0)
		iPopulation = initialPopulation();

	scaled rNumCitiesFactor = 1;

	rNumCitiesFactor.mulDiv(iPopulation + 17, 18);

	CvWorldInfo const& kWorld = GC.getInfo(GC.getMap().getWorldSize());
	rNumCitiesFactor *= per100(kWorld.getNumCitiesMaintenancePercent());

	CvPlayer const& kOwner = GET_PLAYER(eOwner);
	rNumCitiesFactor *= per100(GC.getInfo(kOwner.getHandicapType()).
			getNumCitiesMaintenancePercent());
	// <advc.140>
	scaled rCrowdednessFactor(GC.getGame().getCivPlayersEverAlive(),
			GC.getGame().getRecommendedPlayers());
	rNumCitiesFactor *= rCrowdednessFactor.sqrt(); // </advc.140>

	int iNumVassalCities = 0;
	for (PlayerIter<ALIVE,VASSAL_OF> it(kOwner.getTeam()); it.hasNext(); ++it)
		iNumVassalCities += it->getNumCities();
	iNumVassalCities /= std::max(1, MemberIter::count(kOwner.getTeam()));
	int iMaintenanceCities = kOwner.getNumCities() + iExtraCities +
			// K-Mod, 04/sep/10: Reduced vassal maintenance
			iNumVassalCities / 2;
	// K-Mod: Removed maintenance cap  // advc.exp: I've disabled it through XML instead
	scaled rMaxMaintenanceCities = GC.getInfo(GET_PLAYER(eOwner).getHandicapType()).
			getMaxNumCitiesMaintenance();
	// <advc.exp.1> Upper bound set through GlobalDefines
	static int const iMAX_CITY_COUNT_FOR_MAINTENANCE = GC.getDefineINT("MAX_CITY_COUNT_FOR_MAINTENANCE");
	rMaxMaintenanceCities.decreaseTo(scaled(iMAX_CITY_COUNT_FOR_MAINTENANCE) /
			(1 + per100(kWorld.getNumCitiesMaintenancePercent())));
	iMaintenanceCities = std::min(iMaintenanceCities, rMaxMaintenanceCities.round());
	// </advc.exp.1>
	scaled rNumCitiesMaintenance = iMaintenanceCities * rNumCitiesFactor;

	rNumCitiesMaintenance *= per100(std::max(0, kOwner.getNumCitiesMaintenanceModifier() + 100));
	FAssert(rNumCitiesMaintenance >= 0);
	return rNumCitiesMaintenance.getPercent();
}

// advc.004b, advc.104: Parameters added
int CvCity::calculateColonyMaintenanceTimes100(CvPlot const& kCityPlot,
	PlayerTypes eOwner, int iPopulation, int iExtraCities)
{
	if(iPopulation < 0)
		iPopulation = initialPopulation();
	HandicapTypes const eOwnerHandicap = GET_PLAYER(eOwner).getHandicapType();
	CvArea const& kCityArea = kCityPlot.getArea();

	if (GC.getGame().isOption(GAMEOPTION_NO_VASSAL_STATES))
		return 0;
	{
		CvCity* pCapital = GET_PLAYER(eOwner).getCapital();
		if (pCapital != NULL && pCapital->isArea(kCityArea))
			return 0;
	}
	int iNumCitiesPercent = 100;

	iNumCitiesPercent *= (iPopulation + 17);
	iNumCitiesPercent /= 18;

	iNumCitiesPercent *= GC.getInfo(GC.getMap().getWorldSize()).getColonyMaintenancePercent();
	iNumCitiesPercent /= 100;

	iNumCitiesPercent *= GC.getInfo(eOwnerHandicap).getColonyMaintenancePercent();
	iNumCitiesPercent /= 100;
//doto  - very strange calc -> (iNumCities * iNumCities) //fixed by advc 0 but the same, using sqr
	int iNumCities = (kCityArea.getCitiesPerPlayer(eOwner) - 1 + iExtraCities) *
			iNumCitiesPercent;
	int iMaintenance = SQR(iNumCities) / 100;
	//iMaintenance = std::min(iMaintenance, (GC.getInfo(getHandicapType()).getMaxColonyMaintenance() * calculateDistanceMaintenanceTimes100()) / 100);
	/*  K-Mod, 17/dec/10: Changed colony maintenance cap to not include
		distance maintenance modifiers (such as state property) */
	int iMaintenanceCap = 100 * GC.getDefineINT(CvGlobals::MAX_DISTANCE_CITY_MAINTENANCE) *
			calculateMaintenanceDistance(&kCityPlot, eOwner);

	iMaintenanceCap *= (iPopulation + 7);
	iMaintenanceCap /= 10;

	iMaintenanceCap *= GC.getInfo(GC.getMap().getWorldSize()).getDistanceMaintenancePercent();
	iMaintenanceCap /= 100;

	iMaintenanceCap *= GC.getInfo(eOwnerHandicap).getDistanceMaintenancePercent();
	iMaintenanceCap /= 100;

	iMaintenanceCap /= GC.getMap().maxTypicalDistance(); // advc.140: was maxPlotDistance

	iMaintenanceCap *= GC.getInfo(eOwnerHandicap).getMaxColonyMaintenance();
	iMaintenanceCap /= 100;

	iMaintenance = std::min(iMaintenance, iMaintenanceCap);
	// K-Mod end
	FAssert(iMaintenance >= 0);
	return iMaintenance;
}
//DOTODPII < Maintenance Modifiers >
int CvCity::calculateHomeAreaMaintenanceTimes100(CvArea const& kArea, PlayerTypes eOwner, int iLocalHomeArea, bool capitalcity)
{
	CvPlayer const& kOwner = GET_PLAYER(eOwner);
	CvCity* pCapital = kOwner.getCapitalCity();
	int iTempMaintenance = 0;
	if (pCapital != NULL && !capitalcity)//capital should pay for being in its own area...
	{
		if (pCapital->isArea(kArea))
		{
			iTempMaintenance = std::max(0, GET_PLAYER(eOwner).getHomeAreaMaintenanceModifier() + 100);
			iTempMaintenance /= 100;
			iTempMaintenance += std::max(0, iLocalHomeArea + 100) ;
			iTempMaintenance /= 100;
		}
	}
	return iTempMaintenance;
}

int CvCity::calculateOtherAreaMaintenanceTimes100(CvArea const& kArea, PlayerTypes eOwner, int iLocalOtherArea)
{
	CvPlayer const& kOwner = GET_PLAYER(eOwner);
	CvCity* pCapital = kOwner.getCapitalCity();
	int iTempMaintenance = 0;
	if (pCapital != NULL)
	{//!pCapital->isArea(kArea)(pCapital->area() != &kArea)
		if (!pCapital->isArea(kArea))
		{
			iTempMaintenance = std::max(0, GET_PLAYER(eOwner).getOtherAreaMaintenanceModifier() + 100);
			iTempMaintenance /= 100;
			iTempMaintenance += std::max(0, iLocalOtherArea + 100);
			iTempMaintenance /= 100;
		}
	}
	return iTempMaintenance;
}
int CvCity::calculateConnectedMaintenanceTimes100(PlayerTypes eOwner, bool iCheckConnection, int iLocalConnected)
{
	int iTempMaintenance = 0;
		if (!iCheckConnection)
		{
			iTempMaintenance = std::max(0, GET_PLAYER(eOwner).getConnectedCityMaintenanceModifier() + 100);
			iTempMaintenance /= 100;
			iTempMaintenance += std::max(0, iLocalConnected + 100);
			iTempMaintenance /= 100;
		}
	return iTempMaintenance;
}

int CvCity::calculateCoastalMaintenanceTimes100(int localCoastal, CvPlot const& kCityPlot, PlayerTypes eOwner,bool capitalcitynConn)
{
		int iTempMaintenance = 0;// = 100;
		if (kCityPlot.isCoastalLand(GC.getDefineINT(CvGlobals::MIN_WATER_SIZE_FOR_OCEAN)) && capitalcitynConn) //dont apply to capital and not if its connected to capital
		{
			iTempMaintenance = std::max(0, GET_PLAYER(eOwner).getCoastalDistanceMaintenanceModifier() + 100);
			iTempMaintenance /= 100;
			iTempMaintenance += std::max(0, localCoastal + 100);
			iTempMaintenance /= 100;
			return iTempMaintenance;
		}
		return 0;
}

//DOTODPII < Maintenance Modifiers >
// advc.500b:
scaled CvCity::defensiveGarrison(
	scaled rStopCountingAt) const // Param important for performance
{
	/*  Time is acceptable - but not negligible (slightly above 1% of an AI turn) -
		with rOUTDATED_PERCENT < 1. Perhaps fine if I keep it at 1 in XML.
		Otherwise, I should simply cache the result of getNoMilitaryPercentAnger. */
	PROFILE_FUNC();

	scaled r = 0;
	CvPlayer const& kOwner = GET_PLAYER(getOwner());
	// ("Obsolete" isn't really the right term for units)
	static scaled const rOUTDATED_PERCENT = per100(GC.getDefineINT(
			"DEMAND_BETTER_PROTECTION_OBSOLETE_PERCENT"));
	CvCity const* pCapital = kOwner.getCapital();
	if (pCapital == this)
		pCapital = NULL;
	FOR_EACH_UNIT_IN(pUnit, getPlot())
	{
		// Exclude naval units but not Explorer and Gunship
		if (!pUnit->isMilitaryHappiness() &&
			pUnit->getUnitInfo().getCultureGarrisonValue() <= 0)
		{
			continue;
		}
		scaled rDefStr = per100(pUnit->maxCombatStr(plot(), NULL, NULL, true));
		if (rOUTDATED_PERCENT != 1 &&
			allUpgradesAvailable(pUnit->getUnitType()) != NO_UNIT ||
			/*	Check capital too. Player might remove access to e.g. Copper
				to avoid Warrior obsoletion penalty, but also cutting the capital
				off from a strategic resource will rarely be worthwhile. */
			(pCapital != NULL && pCapital->allUpgradesAvailable(pUnit->getUnitType())))
		{
			rDefStr *= rOUTDATED_PERCENT;
		}
		r += rDefStr;
		if (rStopCountingAt > 0 && r > rStopCountingAt)
			return r;
	}
	return r;
}

// advc.123f:
void CvCity::failProduction(int iOrderData, int iInvestedProduction, bool bProject)
{
	// Based on code in doCheckProduction
	if (bProject)
		setProjectProduction((ProjectTypes)iOrderData, 0);
	else setBuildingProduction((BuildingTypes)iOrderData, 0);
	OrderTypes eOrderType = (bProject ? ORDER_CREATE : ORDER_CONSTRUCT);
	for (int i = getOrderQueueLength() - 1; i >= 0; i--)
	{
		OrderData* od = getOrderFromQueue(i);
		if (od != NULL && od->eOrderType == eOrderType && od->iData1 == iOrderData)
		{
			FAssert(!canContinueProduction(*od));
			popOrder(i, false, HUMAN_CHOOSE);
		}
	}
	int iGoldPercent = failGoldPercent(eOrderType);
	if (iGoldPercent <= 0)
		return;
	int iProductionGold = (iInvestedProduction * iGoldPercent) / 100;
	GET_PLAYER(getOwner()).changeGold(iProductionGold);
	CvWString szMsg = gDLL->getText("TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED",
			getNameKey(), bProject ?
			GC.getInfo((ProjectTypes)iOrderData).getTextKeyWide() :
			GC.getInfo((BuildingTypes)iOrderData).getTextKeyWide(),
			iProductionGold);
	gDLL->UI().addMessage(getOwner(), false, -1, szMsg, getPlot(), "AS2D_WONDERGOLD",
			MESSAGE_TYPE_MINOR_EVENT, GC.getInfo(COMMERCE_GOLD).getButton(),
			GC.getColorType("RED"));
}

// <advc.064b>
int CvCity::failGoldPercent(OrderTypes eOrder) const // Fail and overflow gold
{
	/*  To make any future changes to the process conversion rates easier, tie
		fail gold to the Wealth process. */
	int r = GC.getInfo((ProcessTypes)0).getProductionToCommerceModifier(COMMERCE_GOLD);
	switch(eOrder)
	{
	case ORDER_TRAIN: r *= GC.getDefineINT("MAXED_UNIT_GOLD_PERCENT"); break;
	case ORDER_CONSTRUCT: r *= GC.getDefineINT("MAXED_BUILDING_GOLD_PERCENT"); break;
	case ORDER_CREATE: r *= GC.getDefineINT("MAXED_PROJECT_GOLD_PERCENT"); break;
	default: r *= 100; FAssert(false);
	}
	return r / 100;
}


void CvCity::handleOverflow(int iRawOverflow, int iProductionModifier, OrderTypes eOrderType)
{
	if(iRawOverflow < 0) // Can happen through the "+" cheat (Debug mode)
		return;
	FAssert(getOverflowProduction() == 0);
	int iProductionGold = 0;
	int iLostProduction = 0;
	int iOverflow = computeOverflow(iRawOverflow, iProductionModifier, eOrderType,
			&iProductionGold, &iLostProduction);
	if(iOverflow <= 0)
		return;
	setOverflowProduction(iOverflow);
	if(iProductionGold > 0 || iLostProduction > 0)
		payOverflowGold(iLostProduction, iProductionGold);
}


void CvCity::payOverflowGold(int iLostProduction, int iProductionGold)
{
	CvPlayer& kOwner = GET_PLAYER(getOwner());
	kOwner.changeGold(iProductionGold);
	CvWString szMsg(gDLL->getText("TXT_KEY_MISC_OVERFLOW_GOLD", iLostProduction,
			getNameKey(), iProductionGold));
	gDLL->UI().addMessage(getOwner(), false, -1, szMsg,
			"AS2D_WONDERGOLD", MESSAGE_TYPE_INFO, GC.getInfo(COMMERCE_GOLD).
			getButton(), NO_COLOR, getX(), getY(), false, false);
} // </advc.064b>
/*************************************************************************************************/
/** INFLUENCE_DRIVEN_WAR                   04/16/09                                johnysmith    */
/**   DOTO                                                                                           */
/** Original Author Moctezuma              Start                                                 */
/*************************************************************************************************/
// ------ BEGIN InfluenceDrivenWar -------------------------------
void CvCity::emergencyConscript()
{
	CvUnit* pUnit;
	UnitAITypes eCityAI;
	UnitTypes eConscriptUnit;

	int iEmergencyDraftMinPopulation = 2; // minimal city size to draft militia 
	if (GC.getDefineINT("IDW_EMERGENCY_DRAFT_MIN_POPULATION"))
		iEmergencyDraftMinPopulation = GC.getDefineINT("IDW_EMERGENCY_DRAFT_MIN_POPULATION");

	if (getPopulation() < iEmergencyDraftMinPopulation)
	{
		return;
	}

	if (getConscriptUnit() == NO_UNIT)
	{
		return;
	}

	changePopulation(-1);

	float fEmergencyDraftAngerMultiplier = 0.5; // default: 50% of normal conscription anger
	if (GC.getDefineFLOAT("IDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER"))
		fEmergencyDraftAngerMultiplier = GC.getDefineFLOAT("IDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER");
		
	changeConscriptAngerTimer(int(flatConscriptAngerLength() * fEmergencyDraftAngerMultiplier));

	eConscriptUnit = getConscriptUnit();

	if (GET_PLAYER(getOwner()).AI_unitValue(eConscriptUnit, UNITAI_CITY_DEFENSE, area()) > 0)
	{
		eCityAI = UNITAI_CITY_DEFENSE;
	}
	else if (GET_PLAYER(getOwner()).AI_unitValue(eConscriptUnit, UNITAI_CITY_COUNTER, area()) > 0)
	{
		eCityAI = UNITAI_CITY_COUNTER;
	}
	else if (GET_PLAYER(getOwner()).AI_unitValue(eConscriptUnit, UNITAI_CITY_SPECIAL, area()) > 0)
	{
		eCityAI = UNITAI_CITY_SPECIAL;
	}
	else
	{
		eCityAI = NO_UNITAI;
	}

	pUnit = GET_PLAYER(getOwner()).initUnit(eConscriptUnit, getX(), getY(), eCityAI);
	FAssertMsg(pUnit != NULL, "pUnit expected to be assigned (not NULL)");

	addProductionExperience(pUnit, true);

	pUnit->setMoves(0);

	float fEmergencyDraftStrength = 0.25f; // default: 25% of full health: represents very low training level
	if (GC.getDefineFLOAT("IDW_EMERGENCY_DRAFT_STRENGTH"))
		fEmergencyDraftStrength = GC.getDefineFLOAT("IDW_EMERGENCY_DRAFT_STRENGTH");

	pUnit->setDamage(int((1 - fEmergencyDraftStrength) * pUnit->maxHitPoints()), getOwner());
}
// ------ END InfluenceDrivenWar ---------------------------------
/*************************************************************************************************/
/** INFLUENCE_DRIVEN_WAR                   04/16/09                                johnysmith    */
/**     DOTO                                                                                         */
/** Original Author Moctezuma              End                                                   */
/*************************************************************************************************/
//DOTO -tholish-Keldath inactive buildings START
//Tholish UnbuildableBuildingDeletion START
bool CvCity::canKeep(BuildingTypes eBuilding) const
{
	BuildingTypes ePrereqBuilding;
	bool bRequiresBonus;
	bool bNeedsBonus;
	int iI;
	CorporationTypes eCorporation;

	if (eBuilding == NO_BUILDING)
	{
		return false;
	}
	
	if (!(GET_PLAYER(getOwner()).canKeep(eBuilding)))
	{
			return false;
	}

/*	if (getNumBuilding(eBuilding) >= GC.getDefineINT(CvGlobals::CITY_MAX_NUM_BUILDINGS))
	{
		return false;
	}
	*/
	if (GC.getBuildingInfo(eBuilding).isPrereqReligion())
	{
		if (getReligionCount() > 0)
		{
			return false;
		}
	}

	if (GC.getBuildingInfo(eBuilding).isStateReligion())
	{
		ReligionTypes eStateReligion = GET_PLAYER(getOwner()).getStateReligion();
		if (NO_RELIGION == eStateReligion || !isHasReligion(eStateReligion))
		{
			return false;
		}
	}

	if (GC.getBuildingInfo(eBuilding).getPrereqReligion() != NO_RELIGION)
	{
		if (!(isHasReligion((ReligionTypes)(GC.getBuildingInfo(eBuilding).getPrereqReligion()))))
		{
			return false;
		}
	}

	eCorporation = (CorporationTypes)GC.getBuildingInfo(eBuilding).getPrereqCorporation();
	if (eCorporation != NO_CORPORATION)
	{
		if (!isHasCorporation(eCorporation))
		{
			return false;
		}
	}

	eCorporation = (CorporationTypes)GC.getBuildingInfo(eBuilding).getFoundsCorporation();
	if (eCorporation != NO_CORPORATION)
	{
		if (GC.getGame().isCorporationFounded(eCorporation))
		{
			return false;
		}

		for (int iCorporation = 0; iCorporation < GC.getNumCorporationInfos(); ++iCorporation)
		{
			if (isHeadquarters((CorporationTypes)iCorporation))
			{
				if (GC.getGame().isCompetingCorporation((CorporationTypes)iCorporation, eCorporation))
				{
					return false;
				}
			}
		}
	}

	if (!getPlot().canConstruct(eBuilding))
	{
		return false;
	}


		if (GC.getBuildingInfo(eBuilding).getPrereqAndBonus() != NO_BONUS)
		{
			if (!hasBonus((BonusTypes)GC.getBuildingInfo(eBuilding).getPrereqAndBonus()))
			{
				return false;
			}
		}

		eCorporation = (CorporationTypes)GC.getBuildingInfo(eBuilding).getFoundsCorporation();
		//doto-advc 099 syntax change
		if (eCorporation != NO_CORPORATION)
		{
			CvCorporationInfo& kCorporation = GC.getInfo(eCorporation);//moved - doto fix
			if (GC.getGame().isCorporationFounded(eCorporation))
			{
				return false;
			}

			if (GET_PLAYER(getOwner()).isNoCorporations())
			{
				return false;
			}

			bool bValid = false;
			for (int i = 0; i < kCorporation.getNumPrereqBonuses(); ++i)
			{
				BonusTypes eBonus = (BonusTypes)GC.getCorporationInfo(eCorporation).getPrereqBonus(i);
				if (NO_BONUS != eBonus)
				{
					if (hasBonus(eBonus))
					{
						bValid = true;
						break;
					}
				}
			}

			if (!bValid)
			{
				return false;
			}
		}

		bRequiresBonus = false;
		bNeedsBonus = true;
		//doto advc 099 sybtax change
		CvBuildingInfo& kBuilding = GC.getInfo(eBuilding);
		for (iI = 0; iI < kBuilding.getNumPrereqOrBonuses(); iI++)
		{
			if (GC.getBuildingInfo(eBuilding).getPrereqOrBonuses(iI) != NO_BONUS)
			{
				bRequiresBonus = true;

				if (hasBonus((BonusTypes)GC.getBuildingInfo(eBuilding).getPrereqOrBonuses(iI)))
				{
					bNeedsBonus = false;
				}
			}
		}

		if (bRequiresBonus && bNeedsBonus)
		{
			return false;
		}

		for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			if (/*GC.getBuildingInfo(eBuilding).isBuildingClassNeededInCity(iI)*/
				GC.getBuildingInfo(eBuilding).isBuildingClassNeededInCity((BuildingClassTypes)iI)
			//	(BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iI) getNumBuilding((BuildingTypes)			
				)
			{
				ePrereqBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));

				if (ePrereqBuilding != NO_BUILDING)
				{
					if (0 == getNumBuilding(ePrereqBuilding) /* && (bContinue || (getFirstBuildingOrder(ePrereqBuilding) == -1))*/)
					{
						return false;
					}
				}
			}
		}


return true;
}
//DOTO -tholish-Keldath inactive buildings  END
//DOTO -tholish-Keldath inactive buildings START
//DOTO ACTIVE BULDINGS + THOLISH
//this is a duplicated function , that sets to 0 all the items, if the prereq for a building (cankeep)
//is false.
// i marked some off so they wont be affected fro safety.
void CvCity::UNprocessBuilding(BuildingTypes eBuilding,int iChange, bool bObsolete)
{
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	CvGame const& kGame = GC.getGame();
	CvPlayer& kOwner = GET_PLAYER(getOwner()); // </advc>
	//doto108 maybe if the value sent - is negetive, its wrong to do double negative?
	if(iChange < 0)
		iChange *= -1;
		
	//iChange = 0;
	if (!GET_TEAM(getTeam()).isObsoleteBuilding(eBuilding) || bObsolete)
	{
		
		if (kBuilding.getNoBonus() != NO_BONUS)
			changeNoBonusCount(kBuilding.getNoBonus(), iChange);

		if (kBuilding.getFreeBonus() != NO_BONUS && kGame.getNumFreeBonuses(eBuilding) > 0)
		{
			changeFreeBonus(kBuilding.getFreeBonus(),
					kGame.getNumFreeBonuses(eBuilding) * iChange);
		}

		if (kBuilding.getFreePromotion() != NO_PROMOTION)
		{
			changeFreePromotionCount(kBuilding.getFreePromotion(), iChange);
		}// <advc.912d>
/*
		if(kGame.isOption(GAMEOPTION_NO_SLAVERY) && kOwner.isHuman() &&
			kBuilding.getHurryAngerModifier() < 0)
		{
			changePopRushCount(iChange);
		}
*/ 
		// </advc.912d>
		int getEspionageDefenseModifier	= kBuilding.getEspionageDefenseModifier();
		int getGreatPeopleRateModifier	= kBuilding.getGreatPeopleRateModifier();
		int getFreeExperience	= kBuilding.getFreeExperience();
	//	int getFoodKept	= kBuilding.getFoodKept();
//		int getAirlift	= kBuilding.getAirlift();
		int getAirModifier	= kBuilding.getAirModifier();
//		int getAirUnitCapacity	= kBuilding.getAirUnitCapacity();
		int getNukeModifier	= kBuilding.getNukeModifier();
//		int getFreeSpecialist	= kBuilding.getFreeSpecialist();
		int getMaintenanceModifier	= kBuilding.getMaintenanceModifier();
		//DPII < Maintenance Modifiers >
		int getLocalDistanceMaintenance = kBuilding.getLocalDistanceMaintenanceModifier();
		int getLocalCoastalDistanceMaintenance = kBuilding.getLocalCoastalDistanceMaintenanceModifier();
		int getLocalConnectedCityMaintenance = kBuilding.getLocalConnectedCityMaintenanceModifier();
		int getLocalHomeAreaMaintenance = kBuilding.getLocalHomeAreaMaintenanceModifier();
		int getLocalOtherAreaMaintenance = kBuilding.getLocalOtherAreaMaintenanceModifier();
		//DPII < Maintenance Modifiers >
		int getWarWearinessModifier	= kBuilding.getWarWearinessModifier();
		int getHurryAngerModifier	= kBuilding.getHurryAngerModifier();
		int getHealRateChange = kBuilding.getHealRateChange();
		if (getEspionageDefenseModifier != 0)
			changeEspionageDefenseModifier(getEspionageDefenseModifier * iChange);
		if (getGreatPeopleRateModifier != 0) 
			changeGreatPeopleRateModifier(getGreatPeopleRateModifier * iChange);
		if (getFreeExperience != 0)
			changeFreeExperience(getFreeExperience * iChange);
/*
		if (getFoodKept != 0)
			changeMaxFoodKeptPercent(getFoodKept * iChange);
*/
/*
		if (getAirlift != 0)
			changeMaxAirlift(getAirlift * iChange);
*/
		if (getAirModifier != 0)
			changeAirModifier(getAirModifier * iChange);
/*
		if (getAirUnitCapacity != 0)
			changeAirUnitCapacity(getAirUnitCapacity * iChange);
*/
		if (getNukeModifier != 0)
			changeNukeModifier(getNukeModifier * iChange);
/*
		if (getFreeSpecialist != 0)
			changeFreeSpecialist(getFreeSpecialist * iChange);
*/
		if (getMaintenanceModifier != 0)
			changeMaintenanceModifier(getMaintenanceModifier * iChange);
		//DPII < Maintenance Modifiers >
		if (getLocalDistanceMaintenance != 0)
			changeLocalDistanceMaintenanceModifier(getLocalDistanceMaintenance * iChange);
		if (getLocalCoastalDistanceMaintenance != 0)
			changeLocalCoastalDistanceMaintenanceModifier(getLocalCoastalDistanceMaintenance * iChange);
		if (getLocalConnectedCityMaintenance != 0)
			changeLocalConnectedCityMaintenanceModifier(getLocalConnectedCityMaintenance * iChange);
		if (getLocalHomeAreaMaintenance != 0)
			changeLocalHomeAreaMaintenanceModifier(getLocalHomeAreaMaintenance * iChange);
		if (getLocalOtherAreaMaintenance != 0)
			changeLocalOtherAreaMaintenanceModifier(getLocalOtherAreaMaintenance * iChange);
		//DPII < Maintenance Modifiers >
		if (getWarWearinessModifier != 0)
			changeWarWearinessModifier(getWarWearinessModifier * iChange);
		if (getHurryAngerModifier != 0)
			changeHurryAngerModifier(getHurryAngerModifier * iChange);
		if (getHealRateChange != 0)
			changeHealRate(getHealRateChange * iChange);
		/* Population Limit ModComp - Beginning */
//keep this one anyway - i dont know what will happen...
//		changePopulationLimitChange(kGame.getAdjustedPopulationLimitChange(kBuilding.getPopulationLimitChange()) * iChange);
		/* Population Limit ModComp - End */
		if (kBuilding.getHealth() > 0)
			changeBuildingGoodHealth(kBuilding.getHealth() * iChange);
		else changeBuildingBadHealth(kBuilding.getHealth() * iChange);
		if (kBuilding.getHappiness() > 0)
			changeBuildingGoodHappiness(kBuilding.getHappiness() * iChange);
		else changeBuildingBadHappiness(kBuilding.getHappiness() * iChange);	
		if (kBuilding.getReligionType() != NO_RELIGION)
		{
			changeStateReligionHappiness(kBuilding.getReligionType(),
					kBuilding.getStateReligionHappiness() * iChange);
		}
		changeMilitaryProductionModifier(kBuilding.getMilitaryProductionModifier() * iChange);
		changeSpaceProductionModifier(kBuilding.getSpaceProductionModifier() * iChange);
		changeExtraTradeRoutes(kBuilding.getTradeRoutes() * iChange);
		changeTradeRouteModifier(kBuilding.getTradeRouteModifier() * iChange);
		changeForeignTradeRouteModifier(kBuilding.getForeignTradeRouteModifier() * iChange);
//keldath - safer to always set it to 0
		changePowerCount(kBuilding.isPower() ? 0 : 0, kBuilding.isDirtyPower());
//keldath - dont you dare changing the gov building :)
//		changeGovernmentCenterCount(kBuilding.isGovernmentCenter() ? iChange : 0);
//keldath - set it always to 0 it safer.
		changeNoUnhappinessCount(kBuilding.isNoUnhappiness() ? 0 : 0);
		//changeNoUnhealthyPopulationCount(kBuilding.isNoUnhealthyPopulation() ? iChange : 0);
		changeUnhealthyPopulationModifier(kBuilding.getUnhealthyPopulationModifier() * iChange); // K-Mod
//keldath - safer to set it to 0 always
		changeBuildingOnlyHealthyCount(kBuilding.isBuildingOnlyHealthy() ? 0 : 0);


		FOR_EACH_ENUM2(Yield, y)
		{
			int getSeaPlotYieldChange = kBuilding.getSeaPlotYieldChange(y);
			int getRiverPlotYieldChange = kBuilding.getRiverPlotYieldChange(y);
			int getYieldChangegetBuildingYieldChange = kBuilding.getYieldChange(y) +
				getBuildingYieldChange(kBuilding.getBuildingClassType(), y);
			int getYieldModifier = kBuilding.getYieldModifier(y);
			int getPowerYieldModifier = kBuilding.getPowerYieldModifier(y);
			int getBuildingYieldChangeO = kOwner.getBuildingYieldChange(eBuilding, y);

			if (getSeaPlotYieldChange != 0)
				changeSeaPlotYield(y, getSeaPlotYieldChange * iChange);
			if(getRiverPlotYieldChange != 0)
				changeRiverPlotYield(y, getRiverPlotYieldChange * iChange);
			if (getYieldChangegetBuildingYieldChange !=0)
				changeBaseYieldRate(y, (getYieldChangegetBuildingYieldChange) * iChange);
			if(getYieldModifier != 0)
				changeYieldRateModifier(y, getYieldModifier * iChange);
			if (getPowerYieldModifier !=0)
				changePowerYieldRateModifier(y, getPowerYieldModifier * iChange);
			// < Civic Infos Plus Start >
			//keldath QA-DONE
			if (getBuildingYieldChangeO !=0)
				changeBuildingYieldChange(kBuilding.getBuildingClassType(), y, getBuildingYieldChangeO);
			// < Civic Infos Plus End   >
		}

		FOR_EACH_ENUM(Commerce)
		{
			int getCommerceModifier = kBuilding.getCommerceModifier(eLoopCommerce);
			int getCommerceHappiness = kBuilding.getCommerceHappiness(eLoopCommerce);
			int getBuildingCommerceChange = kOwner.getBuildingCommerceChange(eBuilding, eLoopCommerce);
			if (getCommerceModifier != 0)
				changeCommerceRateModifier(eLoopCommerce,
					getCommerceModifier * iChange);
			if (getCommerceHappiness != 0)
				changeCommerceHappinessPer(eLoopCommerce,
					getCommerceHappiness * iChange);
			// < Civic Infos Plus Start >
			//keldath QA-DONE
			if (getBuildingCommerceChange !=0)
				changeBuildingCommerceChange(kBuilding.getBuildingClassType(), eLoopCommerce, 
					getBuildingCommerceChange);
			// < Civic Infos Plus End   >
		}
		/*
	doto 109 - isAnyReligionChange was removed or moved in advc 1.00
		if (kBuilding.isAnyReligionChange()) // advc.003t
		{
			FOR_EACH_ENUM(Religion)
			{
				int getReligionChange = kBuilding.getReligionChange(eLoopReligion);
				if (getReligionChange !=0)
					changeReligionInfluence(eLoopReligion,
						kBuilding.getReligionChange(eLoopReligion) * iChange);
			}
		}
		*/
/*		FOR_EACH_ENUM(Specialist)
		{
			int getSpecialistCount = kBuilding.getSpecialistCount(eLoopSpecialist);
			int getFreeSpecialistCount = kBuilding.getFreeSpecialistCount(eLoopSpecialist);
			if (getSpecialistCount !=0)
				changeMaxSpecialistCount(eLoopSpecialist,
				getSpecialistCount * iChange);
			if(getFreeSpecialistCount != 0)
				changeFreeSpecialistCount(eLoopSpecialist,
					kBuilding.getFreeSpecialistCount(eLoopSpecialist) * iChange);
		}
		if (kBuilding.isAnyImprovementFreeSpecialist()) // advc.003t
		{
			FOR_EACH_ENUM(Improvement)
			{
				int getImprovementFreeSpecialist  = kBuilding.getImprovementFreeSpecialist(eLoopImprovement);
				if (getImprovementFreeSpecialist !=0 )
					changeImprovementFreeSpecialists(eLoopImprovement,
					getImprovementFreeSpecialist * iChange);
			}
		}
*/
		FOR_EACH_ENUM(Bonus)
		{
			if (!hasBonus(eLoopBonus))
				continue; // advc

			int getBonusHealthChanges = kBuilding.getBonusHealthChanges(eLoopBonus);
			if (getBonusHealthChanges !=0)
			{
				if (getBonusHealthChanges > 0)
				{
					changeBonusGoodHealth(iChange *
						getBonusHealthChanges);
				}
				else
				{
					changeBonusBadHealth(iChange *
						getBonusHealthChanges);
				}
			}
			int getBonusHappinessChanges = (kBuilding.getBonusHappinessChanges(eLoopBonus));
			if (getBonusHappinessChanges > 0)
			{
				changeBonusGoodHappiness(iChange *
					getBonusHappinessChanges);
			}
			else
			{
				changeBonusBadHappiness(iChange *
					getBonusHappinessChanges);
			}
			int dirty = kBuilding.isDirtyPower();
			if (kBuilding.getPowerBonus() == eLoopBonus && dirty !=0)
				changePowerCount(iChange, dirty);

			FOR_EACH_ENUM(Yield)
			{
				int getBonusYieldModifier = kBuilding.getBonusYieldModifier(eLoopBonus, eLoopYield);
				if(getBonusYieldModifier !=0)
					changeBonusYieldRateModifier(eLoopYield, iChange * 
						getBonusYieldModifier);
			}
		}

		FOR_EACH_ENUM(UnitCombat)
		{
			changeUnitCombatFreeExperience(eLoopUnitCombat, iChange *
					kBuilding.getUnitCombatFreeExperience(eLoopUnitCombat));
		}

		FOR_EACH_ENUM(Domain)
		{
			int exp = kBuilding.getDomainFreeExperience(eLoopDomain);
			int prod = kBuilding.getDomainProductionModifier(eLoopDomain);
			if (exp != 0)
				changeDomainFreeExperience(eLoopDomain, iChange *
					exp);
			if (prod !=0)
				changeDomainProductionModifier(eLoopDomain, iChange *
					kBuilding.getDomainProductionModifier(eLoopDomain));
		}

		updateExtraBuildingHappiness();
		updateExtraBuildingHealth();

//		kOwner.changeAssets(iChange * kBuilding.getAssetValue());

//		getArea().changePower(getOwner(), iChange * kBuilding.getPowerValue());
//		kOwner.changePower(kBuilding.getPowerValue() * iChange);
/* leave the team alone - affect only to the player
		for (MemberIter it(getTeam()); it.hasNext(); ++it)
		{
			if (kBuilding.isTeamShare() || it->getID() == getOwner())
			{
				it->processBuilding(eBuilding, iChange, getArea());
			}
		}
		GET_TEAM(getTeam()).processBuilding(eBuilding, iChange);
		GC.getGame().processBuilding(eBuilding, iChange);
*/	}

	if (!bObsolete)
	{
		/*  <advc.004c> Can avoid an update (which loops over all buildings)
			in the most common circumstances */
		int getDefenseModifier = kBuilding.getDefenseModifier();
		if (getDefenseModifier != 0 && getBuildingDefense() > 0)
			updateBuildingDefense();
		else if (getDefenseModifier !=0)// </advc.004c>
			changeBuildingDefense(getDefenseModifier * iChange);
		// <advc.004c>
		if (kBuilding.get(CvBuildingInfo::RaiseDefense) > 0)
		{
			if (iChange >= 0)
			{
				changeBuildingDefense(std::max(kBuilding.get(CvBuildingInfo::RaiseDefense),
						getBuildingDefense()) - getBuildingDefense());
			}
			else updateBuildingDefense();
		} // </advc.004c>
		int bomb = kBuilding.getBombardDefenseModifier();
		if (bomb !=0)
			changeBuildingBombardDefense(bomb * iChange);
		// advc.051: Moved
		//changeBaseGreatPeopleRate(kBuilding.getGreatPeopleRateChange() * iChange);
		if (kBuilding.getGreatPeopleUnitClass() != NO_UNITCLASS)
		{
			UnitTypes eGreatPeopleUnit = getCivilization().getUnit((UnitClassTypes)
					kBuilding.getGreatPeopleUnitClass());
			if (eGreatPeopleUnit != NO_UNIT)
			{
				int getGreatPeopleRateChange = kBuilding.getGreatPeopleRateChange();
				if (getGreatPeopleRateChange != 0)
				{
					changeGreatPeopleUnitRate(eGreatPeopleUnit,
						getGreatPeopleRateChange * iChange);
					/*  advc.051: Moved from above. Barbarians can obtain wonders, but
						don't have GP units, and shouldn't have a positive base GP rate. */
					changeBaseGreatPeopleRate(getGreatPeopleRateChange * iChange);
				}
			}
		}
		// <advc.004w>
/* leave the counts for buildings as is, safer this way.
		if (kGame.getCurrentLayer() == GLOBE_LAYER_RESOURCE)
		{
			// Update text of resource indicators (CvGameTextMgr::setBonusExtraHelp)
			PlayerTypes eDirtyPlayer = NO_PLAYER;
			if (kBuilding.isNationalWonder() &&
				getOwner() == kGame.getActivePlayer())
			{
				eDirtyPlayer = getOwner();
			}
			else if (kBuilding.isWorldWonder())
				eDirtyPlayer = kGame.getActivePlayer();
			if (eDirtyPlayer != NO_PLAYER)
			{
				gDLL->UI().setDirty(GlobeLayer_DIRTY_BIT, true);
				// advc.003p:
				GET_PLAYER(getOwner()).setBonusHelpDirty();
			}
		} // </advc.004w>
*/
	}
}
//DOTO active buildings