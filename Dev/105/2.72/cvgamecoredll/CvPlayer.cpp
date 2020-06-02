// player.cpp

#include "CvGameCoreDLL.h"
#include "CvPlayer.h"
#include "CvAgents.h" // advc.agent
#include "CoreAI.h"
#include "CvCityAI.h"
#include "CvUnitAI.h"
#include "CvSelectionGroupAI.h"
#include "CvDeal.h"
#include "CvTalkingHeadMessage.h"
#include "UWAIAgent.h" // advc.104
#include "PlotRange.h"
#include "CvArea.h"
#include "CvInfo_All.h"
#include "CvDiploParameters.h"
#include "CvPopupInfo.h"
#include "CvGameTextMgr.h"
#include "RiseFall.h" // advc.700
#include "AdvCiv4lerts.h" // advc.210
#include "CvBugOptions.h" // advc.106b
#include "CvDLLFlagEntityIFaceBase.h" // BBAI
#include "BBAILog.h" // BBAI

// advc.003u: Statics moved from CvPlayerAI
CvPlayerAI** CvPlayer::m_aPlayers = NULL;

void CvPlayer::initStatics()
{
	/*  advc.003u (comment): If further concrete classes derived from CvPlayer are
		added, then this function will have to decide which constructor to call. */
	m_aPlayers = new CvPlayerAI*[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
		m_aPlayers[i] = new CvPlayerAI((PlayerTypes)i);
}

void CvPlayer::freeStatics()
{
	for (int i = 0; i < MAX_PLAYERS; i++)
		SAFE_DELETE(m_aPlayers[i]);
	SAFE_DELETE_ARRAY(m_aPlayers);
}


CvPlayer::CvPlayer(/* advc.003u: */ PlayerTypes eID) :
	m_pCivilization(NULL) // advc.003w
{
	m_aiSeaPlotYield = new int[NUM_YIELD_TYPES];
	m_aiYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiCapitalYieldRateModifier = new int[NUM_YIELD_TYPES];
	// < Civic Infos Plus Start >
	m_aiStateReligionYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiStateReligionCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	m_aiNonStateReligionYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiNonStateReligionCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	// < Civic Infos Plus End   >
	m_aiExtraYieldThreshold = new int[NUM_YIELD_TYPES];
	m_aiTradeYieldModifier = new int[NUM_YIELD_TYPES];
	// < Civic Infos Plus Start >
	m_aiSpecialistExtraYield = new int[NUM_YIELD_TYPES];
	// < Civic Infos Plus End   >
	m_aiFreeCityCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiCommercePercent = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceRate = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	m_aiCapitalCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	m_aiStateReligionBuildingCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiSpecialistExtraCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceFlexibleCount = new int[NUM_COMMERCE_TYPES];
	m_aiGoldPerTurnByPlayer = new int[MAX_PLAYERS];
	m_aiEspionageSpendingWeightAgainstTeam = new int[MAX_TEAMS];

	m_abFeatAccomplished = new bool[NUM_FEAT_TYPES];
	m_abOptions = new bool[NUM_PLAYEROPTION_TYPES];

	m_paiBonusExport = NULL;
	m_paiBonusImport = NULL;
	m_paiImprovementCount = NULL;
	m_paiFreeBuildingCount = NULL;
	m_paiExtraBuildingHappiness = NULL;
	m_paiExtraBuildingHealth = NULL;
	m_paiFeatureHappiness = NULL;
	m_paiUnitClassCount = NULL;
	m_paiUnitClassMaking = NULL;
	m_paiBuildingClassCount = NULL;
	m_paiBuildingClassMaking = NULL;
	m_paiHurryCount = NULL;
	m_paiSpecialBuildingNotRequiredCount = NULL;
	m_paiHasCivicOptionCount = NULL;
	m_paiNoCivicUpkeepCount = NULL;
	m_paiHasReligionCount = NULL;
	m_paiHasCorporationCount = NULL;
	m_paiUpkeepCount = NULL;
	m_paiSpecialistValidCount = NULL;

	m_pabResearchingTech = NULL;
	m_pabLoyalMember = NULL;

	m_paeCivics = NULL;

	m_ppaaiSpecialistExtraYield = NULL;
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**																								**/
	/**																								**/
	/*************************************************************************************************/
	m_ppaaiSpecialistCivicExtraCommerce = NULL;
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/
	
	m_ppaaiImprovementYieldChange = NULL;

	m_aszBonusHelp = NULL; // advc.003p
// < Civic Infos Plus Start >
	m_ppaaiBuildingYieldChange = NULL;
	m_ppaaiBuildingCommerceChange = NULL;
// < Civic Infos Plus End   >
	m_bDisableHuman = false; // bbai
	m_iChoosingFreeTechCount = 0; // K-Mod

	reset(eID, true);
}


CvPlayer::~CvPlayer()
{
	uninit();

	SAFE_DELETE_ARRAY(m_aiSeaPlotYield);
	SAFE_DELETE_ARRAY(m_aiYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiCapitalYieldRateModifier);
	// < Civic Infos Plus Start >
	SAFE_DELETE_ARRAY(m_aiStateReligionYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiSpecialistExtraYield);
	SAFE_DELETE_ARRAY(m_aiNonStateReligionYieldRateModifier);
	// < Civic Infos Plus End   >
	SAFE_DELETE_ARRAY(m_aiExtraYieldThreshold);
	SAFE_DELETE_ARRAY(m_aiTradeYieldModifier);
	SAFE_DELETE_ARRAY(m_aiFreeCityCommerce);
	SAFE_DELETE_ARRAY(m_aiCommercePercent);
	SAFE_DELETE_ARRAY(m_aiCommerceRate);
	SAFE_DELETE_ARRAY(m_aiCommerceRateModifier);
	SAFE_DELETE_ARRAY(m_aiCapitalCommerceRateModifier);
	// < Civic Infos Plus Start >
	SAFE_DELETE_ARRAY(m_aiStateReligionCommerceRateModifier);
	SAFE_DELETE_ARRAY(m_aiNonStateReligionCommerceRateModifier);
	// < Civic Infos Plus End   >
	SAFE_DELETE_ARRAY(m_aiStateReligionBuildingCommerce);
	SAFE_DELETE_ARRAY(m_aiSpecialistExtraCommerce);
	SAFE_DELETE_ARRAY(m_aiCommerceFlexibleCount);
	SAFE_DELETE_ARRAY(m_aiGoldPerTurnByPlayer);
	SAFE_DELETE_ARRAY(m_aiEspionageSpendingWeightAgainstTeam);
	SAFE_DELETE_ARRAY(m_abFeatAccomplished);
	SAFE_DELETE_ARRAY(m_abOptions);
	SAFE_DELETE(m_pCivilization); // advc.003w
}


void CvPlayer::init(PlayerTypes eID)
{
	reset(eID); // Reset serialized data

	initContainers(); // advc.003q: Moved into a subroutine for initInGame

	setupGraphical();

	// Init team data ...
	FAssert(getTeam() != NO_TEAM);
	GET_TEAM(getTeam()).changeNumMembers(1);
	GET_TEAM(getTeam()).updateMinorCiv(); // advc.003m

	// Init other player data ...
	// <advc.003q> Moved into a subroutine for initInGame
	if(!initOtherData())
		return;
	resetCivTypeEffects(true); // There was already a subroutine for this
	// </advc.003q>

	AI().AI_init();
}

// advc.003q: Cut from CvPlayer::init
void CvPlayer::initContainers()
{
	m_plotGroups.init();
	m_cities.init();
	m_units.init();
	m_selectionGroups.init();
	m_eventsTriggered.init();
}

/*  advc.003q: Cut from CvPlayer::init and refactored a bit. Returns false if
	data not initialized due to slot status. */
bool CvPlayer::initOtherData()
{
	SlotStatus eStatus = GC.getInitCore().getSlotStatus(getID());
	if (eStatus != SS_TAKEN && eStatus != SS_COMPUTER)
		return false;
	initAlerts(); // advc.210
	setAlive(true);
	LeaderHeadTypes ePersonality = NO_LEADER; // advc.104: Moved up
	if (GC.getGame().isOption(GAMEOPTION_RANDOM_PERSONALITIES) &&
		!isBarbarian() && !isMinorCiv())
	{
		int iBestValue = 0;
		int const iBARBARIAN_LEADER = GC.getDefineINT("BARBARIAN_LEADER"); // advc.opt
		for (int iI = 0; iI < GC.getNumLeaderHeadInfos(); iI++)
		{
			if (iI == iBARBARIAN_LEADER) // XXX minor civ???
				continue;

			int iValue = (1 + GC.getGame().getSorenRandNum(10000, "Choosing Personality"));
			for (int iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes)iJ).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iJ).getPersonalityType() == iI)
						iValue /= 2;
				}
			}
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				ePersonality = ((LeaderHeadTypes)iI);
			}
		}
	}
	// <advc.104> Ensure that AI weights are updated
	if (ePersonality == NO_LEADER)
		ePersonality = getPersonalityType();
	setPersonalityType(ePersonality); // </advc.104>

	changeBaseFreeUnits(GC.getDefineINT("INITIAL_BASE_FREE_UNITS"));
	changeBaseFreeMilitaryUnits(GC.getDefineINT("INITIAL_BASE_FREE_MILITARY_UNITS"));
	changeFreeUnitsPopulationPercent(GC.getDefineINT("INITIAL_FREE_UNITS_POPULATION_PERCENT"));
	changeFreeMilitaryUnitsPopulationPercent(GC.getDefineINT("INITIAL_FREE_MILITARY_UNITS_POPULATION_PERCENT"));
	changeGoldPerUnit(GC.getDefineINT("INITIAL_GOLD_PER_UNIT"));
	changeTradeRoutes(GC.getDefineINT("INITIAL_TRADE_ROUTES"));
	changeStateReligionHappiness(GC.getDefineINT("INITIAL_STATE_RELIGION_HAPPINESS"));
	changeNonStateReligionHappiness(GC.getDefineINT("INITIAL_NON_STATE_RELIGION_HAPPINESS"));

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		YieldTypes eYield = (YieldTypes)iI;
		changeTradeYieldModifier(eYield, GC.getInfo(eYield).getTradeModifier());
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		CommerceTypes eCommerce = (CommerceTypes)iI;
		setCommercePercent(eCommerce, GC.getInfo(eCommerce).getInitialPercent(), true);
	}
	/*  advc.003q: Moved into a (sub-)subroutine - except for the setCivics code;
		that's handled (only) by resetCivTypeEffects. */
	processTraits(1);
	return true;
}


/*  BETTER_BTS_AI_MOD, 12/30/08, jdog5000: START
	(copy of CvPlayer::init but with modifications for use in the middle of a game) */
void CvPlayer::initInGame(PlayerTypes eID)
{
	int iI, iJ;
	reset(eID);
	initContainers(); // advc.003q: New subroutine to avoid code duplication
	setupGraphical();

	// BBAI: Some effects on team necessary if this is the only member of the team
	int iOtherTeamMembers = 0;
	for (iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if (iI != getID())
		{
			if (TEAMID((PlayerTypes)iI) == getTeam())
			{
				iOtherTeamMembers++;
			}
		}
	}
	bool bTeamInit = false;
	if (iOtherTeamMembers == 0 || GET_TEAM(getTeam()).getNumMembers() == 0)
	{
		bTeamInit = true;
		GET_TEAM(getTeam()).init(getTeam());
		GET_TEAM(getTeam()).resetPlotAndCityData();
	}
	if (bTeamInit || (GET_TEAM(getTeam()).getNumMembers() == iOtherTeamMembers))
	{
		GET_TEAM(getTeam()).changeNumMembers(1);
	}
	// <advc.003q>
	// End of BBAI team effects
	if(!initOtherData()) // New subroutine to avoid code duplication
		return;

	GC.getAgents().colonyCreated(getID()); // advc.agent
	// <advc.104r>
	if(getUWAI.isEnabled())
		getUWAI.initNewCivInGame(getID());
	// </advc.104r>
	/*  I've kept the initialization of random event data out of initOtherData
		b/c the BBAI code handles that part differently (cf. resetCivTypeEffects). */
	// </advc.003q>
	// Reset all triggers at first, set those whose events have fired in next block
	for (iI = 0; iI < GC.getNumEventTriggerInfos(); iI++)
		resetTriggerFired((EventTriggerTypes)iI);

	for (iI = 0; iI < GC.getNumEventInfos(); iI++)
	{
		//resetEventOccured((EventTypes)iI, false); // BtS
		// Has global trigger fired already?
		const EventTriggeredData* pEvent = NULL;
		for (iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
		{
			if (iJ == getID())
				continue; // advc

			pEvent = GET_PLAYER((PlayerTypes)iJ).getEventOccured((EventTypes)iI);
			if (pEvent == NULL)
				continue; // advc

			CvEventTriggerInfo& kTrigger = GC.getInfo(pEvent->m_eTrigger);
			if (kTrigger.isGlobal())
			{
				setTriggerFired(*pEvent, false, false);
				break;
			}
			else if (kTrigger.isTeam() && GET_PLAYER((PlayerTypes)iJ).getTeam() == getTeam())
			{
				setTriggerFired(*pEvent, false, false);
				break;
			}
		}
		resetEventOccured((EventTypes)iI, false);
	}
	for (int i = 0; i < GC.getNumCivicOptionInfos(); i++)
	{
		CivicOptionTypes eCivicOption = (CivicOptionTypes)i;
		setCivics(eCivicOption, getCivilization().getInitialCivic(eCivicOption));
	}

	resetPlotAndCityData(); // BBAI
	AI().AI_init();
}

// <advc.210>
void CvPlayer::initAlerts(bool bSilentCheck)
{
	if (!isHuman()) // advc.test: Hopefully OK this way in networked multiplayer
		return;
	if (!m_paAlerts.empty())
	{
		// OK if this happens when the active player is defeated during Auto Play
		FAssertMsg(!isAlive() && isHumanDisabled(), "initAlerts called redundantly");
		uninitAlerts();
	}
	/*  The order of this array needs to correspond to the ids returned
		by the AdvCiv4lers.getID functions in Civ4lerts.py */
	m_paAlerts.push_back(new WarTradeAlert(getID())); // advc.210a
	m_paAlerts.push_back(new RevoltAlert(getID())); // advc.210b
	m_paAlerts.push_back(new BonusThirdPartiesAlert(getID())); // advc.210d
	m_paAlerts.push_back(new CityTradeAlert(getID())); // advc.ctr
	if (bSilentCheck)
	{
		for (size_t i = 0; i < m_paAlerts.size(); i++)
			checkAlert(i, true);
	}
}


void CvPlayer::uninitAlerts()
{
	for(size_t i = 0; i < m_paAlerts.size(); i++)
		SAFE_DELETE(m_paAlerts[i]);
	m_paAlerts.clear();
} // </advc.210>

// Reset all data for this player stored in plot and city objects
void CvPlayer::resetPlotAndCityData()
{
	CvMap const& kMap = GC.getMap();
	for (int iPlot = 0; iPlot < kMap.numPlots(); ++iPlot)
	{
		CvPlot& kPlot = kMap.getPlotByIndex(iPlot);

		kPlot.setCulture(getID(), 0, false, false);
		kPlot.setFoundValue(getID(), 0);

		CvCity* pLoopCity = kPlot.getPlotCity();
		if (pLoopCity != NULL)
		{
			pLoopCity->setCulture(getID(), 0, false, false);
			pLoopCity->changeNumRevolts(getID(), -pLoopCity->getNumRevolts(getID()));
			pLoopCity->setEverOwned(getID(), false);
			pLoopCity->setTradeRoute(getID(), false);
		}
	}
}
// BETTER_BTS_AI_MOD: END

void CvPlayer::uninit()
{
	SAFE_DELETE_ARRAY(m_paiBonusExport);
	SAFE_DELETE_ARRAY(m_paiBonusImport);
	SAFE_DELETE_ARRAY(m_paiImprovementCount);
	SAFE_DELETE_ARRAY(m_paiFreeBuildingCount);
	SAFE_DELETE_ARRAY(m_paiExtraBuildingHappiness);
	SAFE_DELETE_ARRAY(m_paiExtraBuildingHealth);
	SAFE_DELETE_ARRAY(m_paiFeatureHappiness);
	SAFE_DELETE_ARRAY(m_paiUnitClassCount);
	SAFE_DELETE_ARRAY(m_paiUnitClassMaking);
	SAFE_DELETE_ARRAY(m_paiBuildingClassCount);
	SAFE_DELETE_ARRAY(m_paiBuildingClassMaking);
	SAFE_DELETE_ARRAY(m_paiHurryCount);
	SAFE_DELETE_ARRAY(m_paiSpecialBuildingNotRequiredCount);
	SAFE_DELETE_ARRAY(m_paiHasCivicOptionCount);
	SAFE_DELETE_ARRAY(m_paiNoCivicUpkeepCount);
	SAFE_DELETE_ARRAY(m_paiHasReligionCount);
	SAFE_DELETE_ARRAY(m_paiHasCorporationCount);
	SAFE_DELETE_ARRAY(m_paiUpkeepCount);
	SAFE_DELETE_ARRAY(m_paiSpecialistValidCount);

	SAFE_DELETE_ARRAY(m_pabResearchingTech);
	SAFE_DELETE_ARRAY(m_pabLoyalMember);

	SAFE_DELETE_ARRAY(m_paeCivics);

	m_triggersFired.clear();

	if (m_ppaaiSpecialistExtraYield != NULL)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiSpecialistExtraYield[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiSpecialistExtraYield);
	}
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**																								**/
	/**																								**/
	/*************************************************************************************************/
	if (m_ppaaiSpecialistCivicExtraCommerce != NULL)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiSpecialistCivicExtraCommerce[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiSpecialistCivicExtraCommerce);
	}
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/	
	if (m_ppaaiImprovementYieldChange != NULL)
	{
		for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiImprovementYieldChange[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiImprovementYieldChange);
	}
	// <advc.003p>
	if(m_aszBonusHelp != NULL)
	{
		for(int i = 0; i < GC.getNumBonusInfos(); i++)
			SAFE_DELETE(m_aszBonusHelp[i]);
		SAFE_DELETE_ARRAY(m_aszBonusHelp);
	} // </advc.003p>
// < Civic Infos Plus Start >
	if (m_ppaaiBuildingYieldChange != NULL)
	{
		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiBuildingYieldChange[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiBuildingYieldChange);
	}

	if (m_ppaaiBuildingCommerceChange != NULL)
	{
		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiBuildingCommerceChange[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiBuildingCommerceChange);
	}
// < Civic Infos Plus End   >
	m_groupCycle.clear();
	m_researchQueue.clear();
	m_cityNames.clear();

	m_plotGroups.uninit();
	m_cities.uninit();
	m_units.uninit();
	m_selectionGroups.uninit();
	m_eventsTriggered.uninit();

	clearMessages();
	clearPopups();
	clearDiplomacy();
	uninitAlerts(); // advc.210
}

// Initialize data members that are serialized.
void CvPlayer::reset(PlayerTypes eID, bool bConstructorCall)
{
	int iI, iJ;

	uninit();

	m_iStartingX = INVALID_PLOT_COORD;
	m_iStartingY = INVALID_PLOT_COORD;
	m_iTotalPopulation = 0;
	m_iTotalLand = 0;
	m_iTotalLandScored = 0;
	m_iGold = 0;
	m_iGoldPerTurn = 0;
	m_iAdvancedStartPoints = -1;
	m_iGoldenAgeTurns = 0;
	m_iScheduledGoldenAges = 0; // advc.001x
	m_iNumUnitGoldenAges = 0;
	m_iStrikeTurns = 0;
	m_iAnarchyTurns = 0;
	m_iMaxAnarchyTurns = 0;
	m_iAnarchyModifier = 0;
	m_iGoldenAgeModifier = 0;
	m_iGlobalHurryModifier = 0;
	m_iGreatPeopleCreated = 0;
	m_iGreatGeneralsCreated = 0;
	m_iGreatPeopleThresholdModifier = 0;
	m_iGreatGeneralsThresholdModifier = 0;
	m_iGreatPeopleRateModifier = 0;
	m_iGreatGeneralRateModifier = 0;
	m_iDomesticGreatGeneralRateModifier = 0;
	m_iStateReligionGreatPeopleRateModifier = 0;
	m_iMaxGlobalBuildingProductionModifier = 0;
	m_iMaxTeamBuildingProductionModifier = 0;
	m_iMaxPlayerBuildingProductionModifier = 0;
	m_iFreeExperience = 0;
	m_iFeatureProductionModifier = 0;
	m_iWorkerSpeedModifier = 0;
	m_iImprovementUpgradeRateModifier = 0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iCityDefenseModifier = 0;
	m_iNumNukeUnits = 0;
	m_iNumOutsideUnits = 0;
	m_iBaseFreeUnits = 0;
	m_iBaseFreeMilitaryUnits = 0;
	m_iFreeUnitsPopulationPercent = 0;
	m_iFreeMilitaryUnitsPopulationPercent = 0;
	m_iGoldPerUnit = 0;
	m_iGoldPerMilitaryUnit = 0;
	m_iExtraUnitCost = 0;
	m_iNumMilitaryUnits = 0;
	m_iHappyPerMilitaryUnit = 0;
	m_iLuxuryModifier = 0; // advc.912c
	m_iMilitaryFoodProductionCount = 0;
	m_iConscriptCount = 0;
	m_iMaxConscript = 0;
	m_iHighestUnitLevel = 1;
	m_iOverflowResearch = 0;
	//m_iNoUnhealthyPopulationCount = 0;
	m_iUnhealthyPopulationModifier = 0; // K-Mod
	m_iExpInBorderModifier = 0;
	m_iBuildingOnlyHealthyCount = 0;
	//DPII < Maintenance Modifier >
	m_iMaintenanceModifier = 0;
	m_iCoastalDistanceMaintenanceModifier = 0;
	m_iConnectedCityMaintenanceModifier = 0;
	//DPII < Maintenance Modifier >
	m_iDistanceMaintenanceModifier = 0;
	m_iNumCitiesMaintenanceModifier = 0;
	m_iCorporationMaintenanceModifier = 0;
	m_iTotalMaintenance = 0;
	m_iUpkeepModifier = 0;
	m_iLevelExperienceModifier = 0;
	m_iExtraHealth = 0;
	m_iBuildingGoodHealth = 0;
	m_iBuildingBadHealth = 0;
	m_iExtraHappiness = 0;
	m_iBuildingHappiness = 0;
	m_iLargestCityHappiness = 0;
	m_iWarWearinessPercentAnger = 0;
	m_iWarWearinessModifier = 0;
	m_iGwPercentAnger = 0; // K-Mod
	m_iFreeSpecialist = 0;
	m_iNoForeignTradeCount = 0;
	m_iNoCorporationsCount = 0;
	m_iNoForeignCorporationsCount = 0;
	m_iCoastalTradeRoutes = 0;
	m_iTradeRoutes = 0;
	m_iRevolutionTimer = 0;
	m_iConversionTimer = 0;
	m_iStateReligionCount = 0;
	m_iNoNonStateReligionSpreadCount = 0;
	m_iStateReligionHappiness = 0;
	m_iNonStateReligionHappiness = 0;
	// < Civic Infos Plus Start >
	m_iStateReligionExtraHealth = 0;
	m_iNonStateReligionExtraHealth = 0;
	// < Civic Infos Plus End   >
	m_iStateReligionUnitProductionModifier = 0;
	m_iStateReligionBuildingProductionModifier = 0;
	m_iStateReligionFreeExperience = 0;
	m_iCapitalCityID = FFreeList::INVALID_INDEX;
	m_iCitiesLost = 0;
	m_iWinsVsBarbs = 0;
	m_iAssets = 0;
	m_iPower = 0;
	m_iPopulationScore = 0;
	m_iLandScore = 0;
	m_iTechScore = 0;
	m_iWondersScore = 0;
	m_iCombatExperience = 0;
	m_iPopRushHurryCount = 0;
	m_iGoldRushHurryCount = 0; // advc.064b
	m_iInflationModifier = 0;
	m_uiStartTime = 0;

	m_bAlive = false;
	m_bEverAlive = false;
	m_bTurnActive = false;
	m_bAutoMoves = false;
	m_bEndTurn = false;
	m_bPbemNewTurn = false;
	m_bExtendedGame = false;
	m_bFoundedFirstCity = false;
	m_bAnyGPPEver = false; // advc.078
	m_bStrike = false;
	m_bDisableHuman = false; // bbai
	m_iChoosingFreeTechCount = 0; // K-Mod
	m_iNewMessages = 0; // advc.106b
	m_bAutoPlayJustEnded = false; // advc.127
	m_bSavingReplay = false; // advc.106i
	m_bScoreboardExpanded = false; // advc.085
	m_eReminderPending = NO_CIVIC; // advc.004x
	m_iCultureGoldenAgeProgress = 0;	//KNOEDEL CULTURAL_GOLDEN_AGE 1/8
	m_iCultureGoldenAgesStarted = 2;	//KNOEDEL CULTURAL_GOLDEN_AGE 2/8
	m_eID = eID;
	updateTeamType();
	updateHuman();
	if (m_eID != NO_PLAYER)
		m_ePersonalityType = GC.getInitCore().getLeader(m_eID); //??? Is this repeated data???
	else m_ePersonalityType = NO_LEADER;
	m_eCurrentEra = ((EraTypes)0);  //??? Is this repeated data???
	m_eLastStateReligion = NO_RELIGION;
	m_eParent = NO_PLAYER;

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSeaPlotYield[iI] = 0;
		m_aiYieldRateModifier[iI] = 0;
		m_aiCapitalYieldRateModifier[iI] = 0;
		m_aiExtraYieldThreshold[iI] = 0;
		m_aiTradeYieldModifier[iI] = 0;
		// < Civic Infos Plus Start >
		m_aiStateReligionYieldRateModifier[iI] = 0;
		m_aiNonStateReligionYieldRateModifier[iI] = 0;
		m_aiSpecialistExtraYield[iI] = 0;
		// < Civic Infos Plus End   >
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiFreeCityCommerce[iI] = 0;
		m_aiCommercePercent[iI] = 0;
		m_aiCommerceRate[iI] = 0;
		m_aiCommerceRateModifier[iI] = 0;
		m_aiCapitalCommerceRateModifier[iI] = 0;
		m_aiStateReligionBuildingCommerce[iI] = 0;
		m_aiSpecialistExtraCommerce[iI] = 0;
		m_aiCommerceFlexibleCount[iI] = 0;
		// < Civic Infos Plus Start >
		m_aiStateReligionCommerceRateModifier[iI] = 0;
		m_aiNonStateReligionCommerceRateModifier[iI] = 0;
		// < Civic Infos Plus End   >
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiGoldPerTurnByPlayer[iI] = 0;
		if (!bConstructorCall && getID() != NO_PLAYER)
		{
			GET_PLAYER((PlayerTypes) iI).m_aiGoldPerTurnByPlayer[getID()] = 0;
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiEspionageSpendingWeightAgainstTeam[iI] = 0; // advc.120: Instead of 1

		if (!bConstructorCall && getTeam() != NO_TEAM)
		{
			for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes) iJ).getTeam() == iI)
				{
					GET_PLAYER((PlayerTypes) iJ).setEspionageSpendingWeightAgainstTeam(getTeam(), 0); // advc.120
				}
			}
		}
	}

	for (iI = 0; iI < NUM_FEAT_TYPES; iI++)
	{
		m_abFeatAccomplished[iI] = false;
	}

	for (iI = 0; iI < NUM_PLAYEROPTION_TYPES; iI++)
	{
		m_abOptions[iI] = false;
	}

	m_szScriptData = "";

	if (!bConstructorCall)
	{
		FAssertMsg(0 < GC.getNumBonusInfos(), "GC.getNumBonusInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiBonusExport==NULL, "about to leak memory, CvPlayer::m_paiBonusExport");
		m_paiBonusExport = new int [GC.getNumBonusInfos()];
		FAssertMsg(m_paiBonusImport==NULL, "about to leak memory, CvPlayer::m_paiBonusImport");
		m_paiBonusImport = new int [GC.getNumBonusInfos()];
		for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_paiBonusExport[iI] = 0;
			m_paiBonusImport[iI] = 0;
		}

		FAssertMsg(0 < GC.getNumImprovementInfos(), "GC.getNumImprovementInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiImprovementCount==NULL, "about to leak memory, CvPlayer::m_paiImprovementCount");
		m_paiImprovementCount = new int [GC.getNumImprovementInfos()];
		for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_paiImprovementCount[iI] = 0;
		}

		FAssertMsg(m_paiFreeBuildingCount==NULL, "about to leak memory, CvPlayer::m_paiFreeBuildingCount");
		m_paiFreeBuildingCount = new int [GC.getNumBuildingInfos()];
		FAssertMsg(m_paiExtraBuildingHappiness==NULL, "about to leak memory, CvPlayer::m_paiExtraBuildingHappiness");
		m_paiExtraBuildingHappiness = new int [GC.getNumBuildingInfos()];
		FAssertMsg(m_paiExtraBuildingHealth==NULL, "about to leak memory, CvPlayer::m_paiExtraBuildingHealth");
		m_paiExtraBuildingHealth = new int [GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_paiFreeBuildingCount[iI] = 0;
			m_paiExtraBuildingHappiness[iI] = 0;
			m_paiExtraBuildingHealth[iI] = 0;
		}

		FAssertMsg(m_paiFeatureHappiness==NULL, "about to leak memory, CvPlayer::m_paiFeatureHappiness");
		m_paiFeatureHappiness = new int [GC.getNumFeatureInfos()];
		for (iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			m_paiFeatureHappiness[iI] = 0;
		}

		FAssertMsg(m_paiUnitClassCount==NULL, "about to leak memory, CvPlayer::m_paiUnitClassCount");
		m_paiUnitClassCount = new int [GC.getNumUnitClassInfos()];
		FAssertMsg(m_paiUnitClassMaking==NULL, "about to leak memory, CvPlayer::m_paiUnitClassMaking");
		m_paiUnitClassMaking = new int [GC.getNumUnitClassInfos()];
		for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			m_paiUnitClassCount[iI] = 0;
			m_paiUnitClassMaking[iI] = 0;
		}

		FAssertMsg(m_paiBuildingClassCount==NULL, "about to leak memory, CvPlayer::m_paiBuildingClassCount");
		m_paiBuildingClassCount = new int [GC.getNumBuildingClassInfos()];
		FAssertMsg(m_paiBuildingClassMaking==NULL, "about to leak memory, CvPlayer::m_paiBuildingClassMaking");
		m_paiBuildingClassMaking = new int [GC.getNumBuildingClassInfos()];
		for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			m_paiBuildingClassCount[iI] = 0;
			m_paiBuildingClassMaking[iI] = 0;
		}

		FAssertMsg(m_paiHurryCount==NULL, "about to leak memory, CvPlayer::m_paiHurryCount");
		m_paiHurryCount = new int [GC.getNumHurryInfos()];
		for (iI = 0; iI < GC.getNumHurryInfos(); iI++)
		{
			m_paiHurryCount[iI] = 0;
		}

		FAssertMsg(m_paiSpecialBuildingNotRequiredCount==NULL, "about to leak memory, CvPlayer::m_paiSpecialBuildingNotRequiredCount");
		m_paiSpecialBuildingNotRequiredCount = new int [GC.getNumSpecialBuildingInfos()];
		for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
		{
			m_paiSpecialBuildingNotRequiredCount[iI] = 0;
		}

		FAssertMsg(m_paiHasCivicOptionCount==NULL, "about to leak memory, CvPlayer::m_paiHasCivicOptionCount");
		m_paiHasCivicOptionCount = new int[GC.getNumCivicOptionInfos()];
		FAssertMsg(m_paiNoCivicUpkeepCount==NULL, "about to leak memory, CvPlayer::m_paiNoCivicUpkeepCount");
		m_paiNoCivicUpkeepCount = new int[GC.getNumCivicOptionInfos()];
		FAssertMsg(m_paeCivics==NULL, "about to leak memory, CvPlayer::m_paeCivics");
		m_paeCivics = new CivicTypes [GC.getNumCivicOptionInfos()];
		for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			m_paiHasCivicOptionCount[iI] = 0;
			m_paiNoCivicUpkeepCount[iI] = 0;
			m_paeCivics[iI] = NO_CIVIC;
		}

		FAssertMsg(m_paiHasReligionCount==NULL, "about to leak memory, CvPlayer::m_paiHasReligionCount");
		m_paiHasReligionCount = new int[GC.getNumReligionInfos()];
		for (iI = 0;iI < GC.getNumReligionInfos();iI++)
		{
			m_paiHasReligionCount[iI] = 0;
		}

		FAssertMsg(m_paiHasCorporationCount==NULL, "about to leak memory, CvPlayer::m_paiHasCorporationCount");
		m_paiHasCorporationCount = new int[GC.getNumCorporationInfos()];
		for (iI = 0;iI < GC.getNumCorporationInfos();iI++)
		{
			m_paiHasCorporationCount[iI] = 0;
		}

		FAssertMsg(m_pabResearchingTech==NULL, "about to leak memory, CvPlayer::m_pabResearchingTech");
		m_pabResearchingTech = new bool[GC.getNumTechInfos()];
		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			m_pabResearchingTech[iI] = false;
		}

		FAssertMsg(m_pabLoyalMember==NULL, "about to leak memory, CvPlayer::m_pabLoyalMember");
		m_pabLoyalMember = new bool[GC.getNumVoteSourceInfos()];
		for (iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
		{
			m_pabLoyalMember[iI] = true;
		}

		FAssertMsg(0 < GC.getNumUpkeepInfos(), "GC.getNumUpkeepInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiUpkeepCount==NULL, "about to leak memory, CvPlayer::m_paiUpkeepCount");
		m_paiUpkeepCount = new int[GC.getNumUpkeepInfos()];
		for (iI = 0; iI < GC.getNumUpkeepInfos(); iI++)
		{
			m_paiUpkeepCount[iI] = 0;
		}

		FAssertMsg(0 < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiSpecialistValidCount==NULL, "about to leak memory, CvPlayer::m_paiSpecialistValidCount");
		m_paiSpecialistValidCount = new int[GC.getNumSpecialistInfos()];
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_paiSpecialistValidCount[iI] = 0;
		}

		FAssertMsg(0 < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_ppaaiSpecialistExtraYield==NULL, "about to leak memory, CvPlayer::m_ppaaiSpecialistExtraYield");
		m_ppaaiSpecialistExtraYield = new int*[GC.getNumSpecialistInfos()];
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_ppaaiSpecialistExtraYield[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiSpecialistExtraYield[iI][iJ] = 0;
			}
		}
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**																								**/
	/**																								**/
	/*************************************************************************************************/
		FAssertMsg(0 < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_ppaaiSpecialistCivicExtraCommerce==NULL, "about to leak memory, CvPlayer::m_ppaaiSpecialistCivicExtraCommerce");
		m_ppaaiSpecialistCivicExtraCommerce = new int*[GC.getNumSpecialistInfos()];
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_ppaaiSpecialistCivicExtraCommerce[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppaaiSpecialistCivicExtraCommerce[iI][iJ] = 0;
			}
		}
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/

		FAssertMsg(m_ppaaiImprovementYieldChange==NULL, "about to leak memory, CvPlayer::m_ppaaiImprovementYieldChange");
		m_ppaaiImprovementYieldChange = new int*[GC.getNumImprovementInfos()];
		for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_ppaaiImprovementYieldChange[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiImprovementYieldChange[iI][iJ] = 0;
			}
		}
		// <advc.003p>
		FAssert(m_aszBonusHelp == NULL);
		m_aszBonusHelp = new CvWString*[GC.getNumBonusInfos()];
		for(int i = 0; i < GC.getNumBonusInfos(); i++)
			m_aszBonusHelp[i] = NULL;
		// </advc.003p>
		// < Civic Infos Plus Start >
		FAssertMsg(m_ppaaiBuildingYieldChange==NULL, "about to leak memory, CvPlayer::m_ppaaiBuildingYieldChange");
		m_ppaaiBuildingYieldChange = new int*[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppaaiBuildingYieldChange[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiBuildingYieldChange[iI][iJ] = 0;
			}
		}

		FAssertMsg(m_ppaaiBuildingCommerceChange==NULL, "about to leak memory, CvPlayer::m_ppaaiBuildingCommerceChange");
		m_ppaaiBuildingCommerceChange = new int*[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppaaiBuildingCommerceChange[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppaaiBuildingCommerceChange[iI][iJ] = 0;
			}
		}
        // < Civic Infos Plus End   >

		m_mapEventsOccured.clear();
		m_mapEventCountdown.clear();
		m_aFreeUnitCombatPromotions.clear();
		m_aFreeUnitClassPromotions.clear();
		m_aVote.clear();
		m_aUnitExtraCosts.clear();
		m_triggersFired.clear();
		// <advc.106b>
		for(size_t i = 0; i < m_aMajorMsgs.size(); i++)
			SAFE_DELETE(m_aMajorMsgs[i]);
		m_aMajorMsgs.clear(); // </advc.106b>
	}

	m_plotGroups.removeAll();
	m_cities.removeAll();
	m_units.removeAll();
	m_selectionGroups.removeAll();
	m_eventsTriggered.removeAll();

	if (!bConstructorCall)
		AI().AI_reset(false);
}


// CHANGE_PLAYER, 08/17/08, jdog5000: START
// for stripping obsolete trait bonuses
// for complete reset, use in conjunction with addTraitBonuses
//void CvPlayer::clearTraitBonuses() { ... }
//void CvPlayer::addTraitBonuses() { ... }
// advc.003q: Combined into a single function, which CvPlayer::init can call as well.
void CvPlayer::processTraits(int iChange)
{
	int iI, iJ;
	FAssert(GC.getNumTraitInfos() > 0);
	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (!hasTrait((TraitTypes)iI))
			continue;

		changeExtraHealth(iChange * GC.getInfo((TraitTypes)iI).getHealth());
		changeExtraHappiness(iChange * GC.getInfo((TraitTypes)iI).getHappiness());

		for (iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
		{
			BuildingTypes eBuilding = (BuildingTypes)iJ;
			changeExtraBuildingHappiness(eBuilding, iChange * GC.getInfo(eBuilding).getHappinessTraits(iI));
		}

		CvTraitInfo const& kTrait = GC.getInfo((TraitTypes)iI);
		changeUpkeepModifier(iChange * kTrait.getUpkeepModifier());
		changeLevelExperienceModifier(iChange * kTrait.getLevelExperienceModifier());
		changeGreatPeopleRateModifier(iChange * kTrait.getGreatPeopleRateModifier());
		changeGreatGeneralRateModifier(iChange * kTrait.getGreatGeneralRateModifier());
		changeDomesticGreatGeneralRateModifier(iChange * kTrait.getDomesticGreatGeneralRateModifier());

		changeMaxGlobalBuildingProductionModifier(iChange * kTrait.getMaxGlobalBuildingProductionModifier());
		changeMaxTeamBuildingProductionModifier(iChange * kTrait.getMaxTeamBuildingProductionModifier());
		changeMaxPlayerBuildingProductionModifier(iChange * kTrait.getMaxPlayerBuildingProductionModifier());

		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			YieldTypes eYield = (YieldTypes)iJ;
			changeTradeYieldModifier(eYield, iChange * kTrait.getTradeYieldModifier(eYield));
		}
		for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			CommerceTypes eCommerce = (CommerceTypes)iJ;
			changeFreeCityCommerce(eCommerce, iChange * kTrait.getCommerceChange(eCommerce));
			changeCommerceRateModifier(eCommerce, iChange * kTrait.getCommerceModifier(eCommerce));
		}
		for (iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
		{
			CivicOptionTypes eCivicOption = (CivicOptionTypes)iJ;
			if (GC.getInfo(eCivicOption).getTraitNoUpkeep(iI))
				changeNoCivicUpkeepCount(eCivicOption, iChange);
		}
	}

	/*  advc.003q: BBAI had only put the update calls into addTraitBonuses,
		but I reckon that they're also needed when clearing traits. And the
		setUnitExtraCost update wasn't in the BBAI code at all, but is probably
		needed for the Imperialistic trait. */

	updateMaxAnarchyTurns();

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
		updateExtraYieldThreshold((YieldTypes)iI);

	// advc.003q: setCivics code removed; a change in traits should not reset civics.

	CvCivilization const& kCiv = getCivilization(); // advc.003w
	for (int i = 0; i < kCiv.getNumUnits(); i++)
	{
		UnitTypes eUnit = kCiv.unitAt(i);
		if (GC.getInfo(eUnit).isFound())
			setUnitExtraCost((UnitClassTypes)iI, getNewCityProductionValue());
	}
}

// for changing the personality of the player
void CvPlayer::changePersonalityType()
{
	if(isBarbarian())
		return;

	if(!GC.getGame().isOption(GAMEOPTION_RANDOM_PERSONALITIES))
	{
		setPersonalityType(getLeaderType());
		return;
	}

	int iBestValue = 0;
	LeaderHeadTypes eBestPersonality = NO_LEADER;
	int const iBARBARIAN_LEADER = GC.getDefineINT("BARBARIAN_LEADER"); // advc.opt
	for (int iI = 0; iI < GC.getNumLeaderHeadInfos(); iI++)
	{
		if (iI == iBARBARIAN_LEADER) // XXX minor civ???
			continue;

		int iValue = (1 + GC.getGame().getSorenRandNum(10000, "Choosing Personality"));
		for (int iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
		{
			if (GET_PLAYER((PlayerTypes)iJ).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iJ).getPersonalityType() == (LeaderHeadTypes)iI)
					iValue /= 2;
			}
		}
		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			eBestPersonality = ((LeaderHeadTypes)iI);
		}
	}
	if (eBestPersonality != NO_LEADER)
		setPersonalityType(eBestPersonality);
}

// reset state of event logic, unit prices
void CvPlayer::resetCivTypeEffects(/* advc.003q: */ bool bInit)  // advc: style changes
{
	CvCivilization const& kCiv = getCivilization(); // advc.003w
	if (/* <advc.003q> */ bInit /* </advc.003q> */ || !isAlive())
	{
		for (int i = 0; i < GC.getNumCivicOptionInfos(); i++)
		{
			CivicOptionTypes eCivicOption = (CivicOptionTypes)i;
			setCivics(eCivicOption, kCiv.getInitialCivic(eCivicOption));
		}

		for (int i = 0; i < GC.getNumEventInfos(); i++)
			resetEventOccured((EventTypes)i, false);

		for (int i = 0; i < GC.getNumEventTriggerInfos(); i++)
		{
			EventTriggerTypes eEventTrigger = (EventTriggerTypes)i;
			if(bInit || // advc.003q
				(!GC.getInfo(eEventTrigger).isGlobal() &&
				(!GC.getInfo(eEventTrigger).isTeam() ||
				GET_TEAM(getTeam()).getNumMembers() == 1)))
			{
				resetTriggerFired(eEventTrigger);
			}
		}
	}
	for (int i = 0; i < kCiv.getNumUnits(); i++)
	{
		UnitTypes eUnit = kCiv.unitAt(i);
		if (GC.getInfo(eUnit).isFound())
			setUnitExtraCost(kCiv.unitClassAt(i), getNewCityProductionValue());
	}
}

// for switching the leaderhead of this player
void CvPlayer::changeLeader(LeaderHeadTypes eNewLeader)
{
	LeaderHeadTypes eOldLeader = getLeaderType();

	if (eOldLeader == eNewLeader)
		return;

	//clearTraitBonuses(); // Clear old traits
	processTraits(-1); // advc.003q
	GC.getInitCore().setLeader(getID(), eNewLeader);
	// addTraitBonuses(); // Add new traits
	processTraits(1); // advc.003q

	// Set new personality
	changePersonalityType();

	if (isAlive() || isEverAlive())
	{
		gDLL->UI().setDirty(HighlightPlot_DIRTY_BIT, true);
		gDLL->UI().setDirty(CityInfo_DIRTY_BIT, true);
		gDLL->UI().setDirty(UnitInfo_DIRTY_BIT, true);
		gDLL->UI().setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->UI().setDirty(Flag_DIRTY_BIT, true);
		gDLL->UI().setDirty(MinimapSection_DIRTY_BIT, true);
		gDLL->UI().setDirty(Score_DIRTY_BIT, true);
		gDLL->UI().setDirty(Foreign_Screen_DIRTY_BIT, true);
	}
	/*  advc.104: Re-initializing the AI here was probably always a bad idea,
		but definitely mustn't re-initialize UWAI. Instead call AI_updatePersonality
		from setPersonalityType. */
	//AI().AI_init();
}

// (advc: Moved up so that all the CHANGE_PLAYER code dated 08/17/08 is in one place)
// for changing whether this player is human or not
void CvPlayer::setIsHuman(bool bNewValue, /* advc.127c: */ bool bUpdateAI)
{
	// <advc.706> Make sure that these are consistent
	if(!bNewValue)
		m_bDisableHuman = false; // </advc.706>
	if (bNewValue == isHuman())
		return;

	GC.getInitCore().setSlotStatus(getID(), bNewValue ? SS_TAKEN :
			SS_COMPUTER); // or SS_OPEN for multiplayer?
	// <advc.210> Only human players need alerts
	if (bNewValue)
	{
		/*  Tbd. (advc.106c): The Python Civ4lerts should also do a silent check
			here; to avoid alerts right after assuming control of the player. */
		initAlerts(true);
	}
	else uninitAlerts(); // </advc.210>
	if (bUpdateAI)
		AI().AI_setHuman(bNewValue);
}
// CHANGE_PLAYER: END
// CHANGE_PLAYER, 05/09/09, jdog5000: START
// for changing the civilization of this player
void CvPlayer::changeCiv(CivilizationTypes eNewCiv)  // advc: style changes
{
	CivilizationTypes eOldCiv = getCivilizationType();
	if (eOldCiv == eNewCiv)
		return;

	PlayerColorTypes eColor = (PlayerColorTypes)GC.getInfo(eNewCiv).getDefaultPlayerColor();
	PlayerColorTypes const eBarbarianColor = (PlayerColorTypes)GC.getInfo((CivilizationTypes)
			GC.getDefineINT("BARBARIAN_CIVILIZATION")).getDefaultPlayerColor();
	for (int i = 0; i < MAX_CIV_PLAYERS; i++)
	{
		CvPlayer const& kPlayer = GET_PLAYER((PlayerTypes)i);
		if (eColor == NO_PLAYERCOLOR || (kPlayer.getPlayerColor() == eColor &&
			kPlayer.getID() != getID()))
		{
			FOR_EACH_ENUM(PlayerColor)
			{
				if (eLoopPlayerColor == eBarbarianColor)
					continue;

				bool bValid = true;
				for (int j = 0; j < MAX_CIV_PLAYERS; j++)
				{
					if (GET_PLAYER((PlayerTypes)j).getPlayerColor() == eLoopPlayerColor)
					{
						bValid = false;
						break;
					}
				}
				if (bValid)
				{
					eColor = eLoopPlayerColor;
					i = MAX_CIV_PLAYERS;
					break;
				}
			}
		}
	}

	CvInitCore& kInitCore = GC.getInitCore();
	kInitCore.setCiv(getID(), eNewCiv);
	kInitCore.setColor(getID(), eColor);
	setCivilization(eNewCiv); // advc.003u
	resetCivTypeEffects(/* advc.003q: */ false);
	CvDLLInterfaceIFaceBase& kUI = *gDLL->getInterfaceIFace();
	if (isAlive()) // if the player is alive and showing on scoreboard, etc
	{
		// change colors, graphics, flags, units
		kInitCore.setArtStyle(getID(), (ArtStyleTypes)GC.getInfo(eNewCiv).getArtStyleType());
		// advc.127c: The new setFlagDecal function can handle the flag update
		setFlagDecal(GC.getInfo(eNewCiv).getFlagTexture(), true);
		/*kInitCore.setFlagDecal(getID(), GC.getInfo(eNewCiv).getFlagTexture());
		// Force update of units flags
		bool bAuto = m_bDisableHuman;
		m_bDisableHuman = true;
		EraTypes eEra = getCurrentEra();
		setCurrentEra((EraTypes)(eEra + (eEra == 0 ? 1 : -1)));
		setCurrentEra(eEra);
		m_bDisableHuman = bAuto;
		kUI.makeInterfaceDirty();
		kUI.setDirty(Flag_DIRTY_BIT, true);*/

		// dirty all of this player's cities...
		FOR_EACH_CITY_VAR(pLoopCity, *this)
			pLoopCity->setLayoutDirty(true);

		//update unit eras
		FOR_EACH_UNIT_VAR(pLoopUnit, *this)
		{
			pLoopUnit->reloadEntity();
			// (advc: Deleted flag update code that wasn't working and had already been commented out)
		}

		if (getID() == GC.getGame().getActivePlayer())
			kUI.setDirty(Soundtrack_DIRTY_BIT, true);
		kUI.makeInterfaceDirty();

		// Need to force redraw
		CvDLLEngineIFaceBase& kEngine = *gDLL->getEngineIFace();
		kEngine.SetDirty(CultureBorders_DIRTY_BIT, true);
		kEngine.SetDirty(MinimapTexture_DIRTY_BIT, true);
		kEngine.SetDirty(GlobeTexture_DIRTY_BIT, true);
		kEngine.SetDirty(GlobePartialTexture_DIRTY_BIT, true);

		kUI.setDirty(ColoredPlots_DIRTY_BIT, true);
		kUI.setDirty(HighlightPlot_DIRTY_BIT, true);
		kUI.setDirty(CityInfo_DIRTY_BIT, true);
		kUI.setDirty(UnitInfo_DIRTY_BIT, true);
		kUI.setDirty(InfoPane_DIRTY_BIT, true);
		kUI.setDirty(GlobeLayer_DIRTY_BIT, true);
		// <advc.003p>
		if(getID() == GC.getGame().getActivePlayer())
			setBonusHelpDirty(); // </advc.003p>
		kUI.setDirty(MinimapSection_DIRTY_BIT, true);
		kEngine.SetDirty(MinimapTexture_DIRTY_BIT, true);
		kUI.setDirty(Score_DIRTY_BIT, true);
		kUI.setDirty(Foreign_Screen_DIRTY_BIT, true);
		kUI.setDirty(SelectionSound_DIRTY_BIT, true);
		kUI.setDirty(GlobeInfo_DIRTY_BIT, true);
	}
	else if (isEverAlive())
	{
		// Not currently alive, but may show on some people's scoreboard or graphs
		// change colors
		kUI.setDirty(InfoPane_DIRTY_BIT, true);
		kUI.setDirty(Score_DIRTY_BIT, true);
	}
	setupGraphical();
} // CHANGE_PLAYER: END


void CvPlayer::setupGraphical() // graphical only setup
{
	if (!GC.IsGraphicsInitialized())
		return;

	// Set up m_cities
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->setupGraphical();

	// Set up m_units
	FOR_EACH_UNIT_VAR(pLoopUnit, *this)
		pLoopUnit->setupGraphical();
}


void CvPlayer::initFreeState()
{
	setGold(0);

	if(!GC.getGame().isOption(GAMEOPTION_ADVANCED_START)) // advc.250c
		changeGold(GC.getInfo(getHandicapType()).getStartingGold());
	changeGold(GC.getInfo(GC.getGame().getStartEra()).getStartingGold());

	clearResearchQueue();
}


void CvPlayer::initFreeUnits()
{	// <dlph.28>
	if(isBarbarian())
		return; // </dlph.28>
	// <advc>
	CvGame const& kGame = GC.getGame();
	int const iStartingUnitMultiplier = GC.getInfo(kGame.getStartEra()).
			getStartingUnitMultiplier(); // </advc>
	// <advc.027> Goody huts no longer block starting sites
	CvPlot* pStartingPlot = getStartingPlot();
	if (pStartingPlot != NULL && !kGame.isScenario() && pStartingPlot->isGoody())
		pStartingPlot->setImprovementType(NO_IMPROVEMENT);
	else FAssert(pStartingPlot != NULL); // (can this happen?)
	// </advc.027>
	if (kGame.isOption(GAMEOPTION_ADVANCED_START) &&
		(!isHuman() || !kGame.isOption(GAMEOPTION_SPAH))) // advc.250b
	{
		int iPoints = kGame.getNumAdvancedStartPoints();
		// advc.250b (comment): Disabled through Handicap XML
		iPoints *= GC.getInfo(getHandicapType()).getAdvancedStartPointsMod();
		iPoints /= 100;

		if (!isHuman() /* advc.250b: */ && !kGame.isOption(GAMEOPTION_SPAH))
		{	/*  advc.250b, advc.001: Was this->getHandicapType(), i.e. Noble, which
				means that this code block did nothing. */
			iPoints *= GC.getInfo(kGame.getHandicapType()).getAIAdvancedStartPercent();
			iPoints /= 100;
		}
		/*  <advc.250c> Civs in Advanced Start can place a city even if they don't
			have enough start points, but I prefer to enforce a minimum. */
		if (iPoints > 0)
		{
			int iMinPoints = GC.getInitCore().getAdvancedStartMinPoints();
			if(iPoints < iMinPoints)
				iPoints = iMinPoints;
		} // </advc.250c>
		setAdvancedStartPoints(iPoints);

		// Starting visibility
		if (pStartingPlot != NULL)
		{
			/*  advc.108: BtS code moved into a new function (b/c I need the same
				behavior elsewhere). */
			GET_TEAM(getID()).revealSurroundingPlots(*pStartingPlot,
					GC.getDefineINT("ADVANCED_START_SIGHT_RANGE"));
		}
	}
	else
	{
		CvCivilization const& kCiv = getCivilization(); // advc.003w
		for (int i = 0; i < kCiv.getNumUnits(); i++)
		{
			UnitTypes eLoopUnit = kCiv.unitAt(i);
			int iFreeCount = kCiv.getNumFreeUnits(eLoopUnit);
			iFreeCount *= (iStartingUnitMultiplier + (!isHuman() ?
					GC.getInfo(kGame.getHandicapType()).
					getAIStartingUnitMultiplier() : 0));
			for (int iJ = 0; iJ < iFreeCount; iJ++)
				addFreeUnit(eLoopUnit);
		}
		int iFreeCount = GC.getInfo(kGame.getStartEra()).getStartingDefenseUnits();
		iFreeCount += GC.getInfo(getHandicapType()).getStartingDefenseUnits();
		// <advc.126>
		CvHandicapInfo const& kGameHandicap = GC.getInfo(kGame.getHandicapType());
		int iFreeAIDefenders = 0;
		// </advc.126>
		if (!isHuman())
		{   // <advc.126>
			iFreeAIDefenders = kGameHandicap.getAIStartingDefenseUnits();
			if(iFreeAIDefenders > 0)
				iFreeAIDefenders += kGame.getStartEra();
			iFreeCount += iFreeAIDefenders;
			// <advc.126>
		}

		if(iFreeCount > 0)
			addFreeUnitAI(UNITAI_CITY_DEFENSE, iFreeCount);

		iFreeCount = GC.getInfo(kGame.getStartEra()).getStartingWorkerUnits();
		iFreeCount += GC.getInfo(getHandicapType()).getStartingWorkerUnits();

		if (!isHuman())
		{   // <advc.126>
			int iFreeAIWorkers = kGameHandicap.getAIStartingWorkerUnits();
			if(iFreeAIWorkers > 0)
				iFreeAIWorkers += kGame.getStartEra() / 2;
			iFreeCount += iFreeAIWorkers;
			// <advc.126>
		}

		if (iFreeCount > 0)
		{
			addFreeUnitAI(UNITAI_WORKER, iFreeCount);
		}

		iFreeCount = GC.getInfo(kGame.getStartEra()).getStartingExploreUnits();
		iFreeCount += GC.getInfo(getHandicapType()).getStartingExploreUnits();

		if (!isHuman())
		{   // <advc.126>
			int iFreeAIExplorers = kGameHandicap.getAIStartingExploreUnits();
			/*  Need at least one addl. when starting in Classical era or later
				b/c the AI doesn't use its free defenders for exploration. */
			if(iFreeAIDefenders > 0)
				iFreeAIExplorers += (kGame.getStartEra() + 2)  / 3;
			iFreeCount += iFreeAIExplorers;
			// <advc.126>
		}

		if (iFreeCount > 0)
		{
			addFreeUnitAI(UNITAI_EXPLORE, iFreeCount);
		}
	}
}


void CvPlayer::addFreeUnitAI(UnitAITypes eUnitAI, int iCount)
{
	UnitTypes eBestUnit = NO_UNIT;
	int iBestValue = 0;
	CvCivilization const& kCiv = getCivilization(); // advc.003w
	for (int i = 0; i < kCiv.getNumUnits(); i++)
	{
		UnitTypes eLoopUnit = kCiv.unitAt(i);
		if (canTrain(eLoopUnit))
		{
			bool bValid = true;

			if (GC.getInfo(eLoopUnit).getPrereqAndBonus() != NO_BONUS)
			{
				bValid = false;
			}

			for (int iJ = 0; iJ < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); iJ++)
			{
				if (GC.getInfo(eLoopUnit).getPrereqOrBonuses(iJ) != NO_BONUS)
				{
					bValid = false;
				}
			}

			// <advc.307> Machine Gun not useful enough against Barbarians
			if(eUnitAI == UNITAI_CITY_DEFENSE &&
				GC.getInfo(eLoopUnit).isOnlyDefensive())
				bValid = false; // </advc.307>

			if (bValid)
			{
				int iValue = AI().AI_unitValue(eLoopUnit, eUnitAI, NULL);
				// <advc.250e> No Archer for exploration
				if(eUnitAI == UNITAI_EXPLORE)
					iValue -= AI().AI_unitValue(eLoopUnit, UNITAI_CITY_DEFENSE, NULL);
				// </advc.250e>
				if (iValue > iBestValue)
				{
					eBestUnit = eLoopUnit;
					iBestValue = iValue;
				}
			}
		}
	}

	if (eBestUnit != NO_UNIT)
	{
		for (int iI = 0; iI < iCount; iI++)
			addFreeUnit(eBestUnit, eUnitAI);
	}
}


void CvPlayer::addFreeUnit(UnitTypes eUnit, UnitAITypes eUnitAI)
{
	// advc.108: Right-hand side cut from below
	bool const bFound = (eUnitAI == UNITAI_SETTLE) ||
			(GC.getInfo(eUnit).getDefaultUnitAIType() == UNITAI_SETTLE);

	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		if (bFound) // advc.108: No functional change
		{
			if (AI().AI_getNumAIUnits(UNITAI_SETTLE) >= 1)
				return;
		}
	}

	CvPlot* pStartingPlot = getStartingPlot();
	if(pStartingPlot == NULL)
		return;

	CvPlot* pBestPlot = NULL;
	//if (isHuman())
	if(eUnitAI == UNITAI_EXPLORE && // advc.108
		!GC.getInfo(eUnit).isFound() &&
		(GC.getPythonCaller()->isHumanExplorerPlacementRandomized() ||
		!isHuman())) // advc.108
	{
		//int iRandOffset = GC.getGame().getSorenRandNum(NUM_CITY_PLOTS, "Place Units (Player)");
		for (CityPlotRandIter it(*pStartingPlot, GC.getGame().getSorenRand(), true);
			it.hasNext(); ++it)
		{
			CvPlot& kLoopPlot = *it;
			if (!kLoopPlot.isGoody() &&
				!kLoopPlot.isImpassable() && !kLoopPlot.isUnit() &&
				kLoopPlot.sameArea(*pStartingPlot) &&
				// advc.108: Don't place the unit across a large bay
				kLoopPlot.calculatePathDistanceToPlot(getTeam(), *pStartingPlot, 3) <= 3)
			{
				pBestPlot = &kLoopPlot;
				break;
			}
		}
	}

	if (pBestPlot == NULL)
		pBestPlot = pStartingPlot;

	// <advc.108> Centered on the Settler, not on the starting plot.
	if(bFound)
	{
		GET_TEAM(getID()).revealSurroundingPlots(*pBestPlot,
				GC.getDefineINT("START_SIGHT_RANGE"));
	} // </advc.108>
	initUnit(eUnit, pBestPlot->getX(), pBestPlot->getY(), eUnitAI);
}

// advc.opt: Now only a wrapper. I'm keeping it around for Python exporting.
int CvPlayer::startingPlotRange() const
{
	return GC.getGame().getStartingPlotRange();
}


bool CvPlayer::startingPlotWithinRange(CvPlot const& kPlot, PlayerTypes ePlayer, int iRange, int iPass) const // advc: 1st param was CvPlot*
{
	//PROFILE_FUNC();

	//XXX changes to AI_foundValue (which are far more flexible) make this function
	//    redundant but it is still called from Python.
	return false;
}

int CvPlayer::startingPlotDistanceFactor(CvPlot const& kPlot, PlayerTypes ePlayer, int iRange) const // advc: 1st param was CvPlot*
{
	PROFILE_FUNC();

	FAssert(ePlayer != getID());

	int iValue = 1000;
	CvPlot* pStartingPlot = getStartingPlot();
	if (pStartingPlot == NULL)
		return iValue; // advc

	if (GC.getGame().isTeamGame())
	{
		if (GET_PLAYER(ePlayer).getTeam() == getTeam())
		{
			static int const iOWN_TEAM_STARTING_MODIFIER = GC.getDefineINT("OWN_TEAM_STARTING_MODIFIER"); // advc.opt
			iRange *= iOWN_TEAM_STARTING_MODIFIER;
			iRange /= 100;
		}
		else
		{
			static int const iRIVAL_TEAM_STARTING_MODIFIER = GC.getDefineINT("RIVAL_TEAM_STARTING_MODIFIER"); // advc.opt
			iRange *= iRIVAL_TEAM_STARTING_MODIFIER;
			iRange /= 100;
		}
	}

	int iDistance = ::stepDistance(&kPlot, pStartingPlot);
	if (!pStartingPlot->sameArea(kPlot))
	{
		iDistance *= 4;
		iDistance /= 3;
	}

	iValue *= iDistance;
	iValue /= iRange ;

	return std::max(1, iValue);
}

// <advc.027>
int CvPlayer::coastRiverStartingAreaScore(CvArea const& a) const
{
	int r = 0;
	// Loop based on CvArea::countCoastalLand
	CvMap const& kMap = GC.getMap();
	for(int i = 0; i < kMap.numPlots(); i++)
	{
		CvPlot const& p = kMap.getPlotByIndex(i);
		if(p.isPeak() || !p.isArea(a))
			continue;
		int iTotalYield = p.calculateTotalBestNatureYield(getTeam());
		int iFoodYield = p.calculateBestNatureYield(YIELD_FOOD, getTeam());
		if((iFoodYield > 0 && iTotalYield >= 2) || iTotalYield >= 3)
		{
			if(p.isCoastalLand())
				r += 2;
			if(p.isRiver())
				r++;
		}
	}
	return r;
} // </advc.027>

// <dlph.35>
class VectorPairSecondGreaterComparator
{
public:
	bool operator() (const std::pair<int,int> &a, const std::pair<int,int> &b)
	{
		return (a.second >= b.second);
	}
}; // </dlph.35>
// Returns the id of the best area, or -1 if it doesn't matter:
//int CvPlayer::findStartingArea() const
// dlph.35: "Returns a vector of all starting areas sorted by their value (instead of one best starting area)."
std::vector<std::pair<int,int> > CvPlayer::findStartingAreas(  // advc: style changes
	bool* pbFoundByMapScript) const // advc.027
{
	PROFILE_FUNC();
	// <advc.027>
	if (pbFoundByMapScript != NULL)
		*pbFoundByMapScript = false; // </advc.027>
	std::vector<std::pair<int,int> > areas_by_value; // dlph.35
	{
		CvArea* pyArea = GC.getPythonCaller()->findStartingArea(getID());
		if (pyArea != NULL)
		{
			areas_by_value.push_back(std::make_pair(pyArea->getID(), 1)); // dlph.35
			// <advc.027>
			if (pbFoundByMapScript != NULL)
				*pbFoundByMapScript = true; // </advc.027>
			return areas_by_value; // dlph.35
		}
	}
	// find best land area
	//int iBestValue = 0; int iBestArea = -1; // dlph.35
	FOR_EACH_AREA(pLoopArea)
	{
		if (pLoopArea->isWater())
			continue;

		// iNumPlayersOnArea is the number of players starting on the area, plus this player
		int iNumPlayersOnArea = (pLoopArea->getNumStartingPlots() + 1);
		// <advc.027>
		int iTileValue = 1 + pLoopArea->calculateTotalBestNatureYield() +
				/*2 * pLoopArea->countCoastalLand() +
				pLoopArea->getNumRiverEdges() +*/
				coastRiverStartingAreaScore(*pLoopArea) + // Replacing the above
				// New: factor in bonus resources
				::round(pLoopArea->getNumTotalBonuses() * 1.5) +
				pLoopArea->getNumTiles() / 2; // Halved
		// </advc.027>
		int iValue = iTileValue / iNumPlayersOnArea;
		iValue *= std::min(NUM_CITY_PLOTS + 1, pLoopArea->getNumTiles() + 1);
		iValue /= (NUM_CITY_PLOTS + 1);

		if (iNumPlayersOnArea <= 2)
		{
			iValue *= 4;
			iValue /= 3;
		}
		/*if (iValue > iBestValue) {
			iBestValue = iValue;
			iBestArea = pLoopArea->getID();
		}*/ // dlph.35:
		areas_by_value.push_back(std::make_pair(pLoopArea->getID(), iValue));
	}
	//return iBestArea; // <dlph.35>
	VectorPairSecondGreaterComparator kComparator;
	std::sort(areas_by_value.begin(), areas_by_value.end(), kComparator);
	areas_by_value.resize(8); // advc: No need to pass around every little island
	return areas_by_value; // </dlph.35>
}


CvPlot* CvPlayer::findStartingPlot(bool bRandomize,
	bool* pbPlotFoundByMapScript, bool* pbAreaFoundByMapScript) // advc.027
{
	PROFILE_FUNC();
	// <advc.027>
	if (pbPlotFoundByMapScript != NULL)
		*pbPlotFoundByMapScript = false;
	if (pbAreaFoundByMapScript != NULL)
		*pbAreaFoundByMapScript = false;
	// </advc.027>
	{
		CvPlot* r = GC.getPythonCaller()->findStartingPlot(getID());
		if (r != NULL)
		{	// <advc.027>
			if (pbPlotFoundByMapScript != NULL)
				*pbPlotFoundByMapScript = true; // </advc.027>
			return r;
		}
	}
	//int iBestArea = -1;
	// dlph.35: "This function is adjusted to work with a list of possible starting areas instead of a single one."
	std::vector<std::pair<int,int> > areas_by_value;
	bool bNew = false;
	if (getStartingPlot() != NULL)
	{
		//iBestArea = getStartingPlot()->getArea().getID(); // dlph.35:
		areas_by_value.push_back(std::make_pair(getStartingPlot()->getArea().getID(), 1));
		setStartingPlot(NULL, true);
		bNew = true;
	}

	AI().AI_updateFoundValues(true);//this sets all plots found values to -1

	if (!bNew)
	{
		//iBestArea = findStartingArea();
		areas_by_value = findStartingAreas( // dlph.35
				pbAreaFoundByMapScript); // advc.027
	}
	/*  <advc.140> Cut and pasted from CvMap::maxPlotDistance. I've changed that
		function, but I think the original formula might be needed here.
		I'm not sure I understand the purpose of this outer loop. */
	// ^dlph.35 replaces the outer loop
	/*int iMaxPlotDist = std::max(1, ::plotDistance(0, 0, ((m.isWrapX()) ?
			(m.getGridWidth() / 2) : (m.getGridWidth() - 1)),
			((m.isWrapY()) ? (m.getGridHeight() / 2) :
			(m.getGridHeight() - 1))));
	for(int iPass = 0; iPass < iMaxPlotDist; iPass++)*/ // </advc.140>
	/*  <dlph.35> "First pass avoids starting locations that have very little food
		(before normalization) to avoid starting on the edge of very bad terrain." */
	int const iStartingRange = GC.getDefineINT("ADVANCED_START_SIGHT_RANGE");
	CvMap const& kMap = GC.getMap();
	int const iMaxPass = 1;
	for(int iPass = 0; iPass <= iMaxPass; iPass++)
	{
		for(size_t iJ = 0; iJ < areas_by_value.size(); iJ++)
		{ // </dlph.35>
			CvPlot *pBestPlot = NULL;
			int iBestValue = iMaxPass - iPass; // advc: was 0 flat
			for (int iI = 0; iI < kMap.numPlots(); iI++)
			{
				CvPlot* pLoopPlot = kMap.plotByIndex(iI);
				//if (iBestArea == -1 || pLoopPlot->getArea() == iBestArea)
				// <dlph.35>
				if (pLoopPlot->getArea().getID() != areas_by_value[iJ].first)
					continue;
				if (iPass == 0) // "Avoid very bad terrain in the first pass."
				{
					int iTotalFood = 0;
					int iLandPlots = 0;
					for (PlotCircleIter it(*pLoopPlot, iStartingRange); it.hasNext(); ++it)
					{
						CvPlot const& kCheckPlot = *it;
						if (!kCheckPlot.isWater())
						{
							iLandPlots++;
							iTotalFood += kCheckPlot.calculateBestNatureYield(
									YIELD_FOOD, NO_TEAM);
						}
					}
					if (iTotalFood < std::max(1, iLandPlots) / 2)
						continue;
				} // </dlph.35>
				//the distance factor is now done inside foundValue
				int iValue = pLoopPlot->getFoundValue(getID());
				if (bRandomize && iValue > 0)
				{	/*  advc (comment): That's a high random portion (high found values tend
						to range between 3000 and 5000), but I'm not sure which map scripts
						(if any) use bRandomize=true, so I'm not changing this. */
					iValue += GC.getGame().getSorenRandNum(10000, "Randomize Starting Location");
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;
				}
			}

			if (pBestPlot != NULL)
				return pBestPlot;
		} // dlph.35: end of areas_by_value loop
		FAssertMsg(iPass != 0, "CvPlayer::findStartingPlot - could not find starting plot in first pass.");
	}

	FAssertMsg(false, "Could not find starting plot.");
	return NULL;
}


CvPlotGroup* CvPlayer::initPlotGroup(CvPlot* pPlot)
{
	CvPlotGroup* pPlotGroup = addPlotGroup();
	FAssert(pPlotGroup != NULL);
	pPlotGroup->init(pPlotGroup->getID(), getID(), pPlot);
	return pPlotGroup;
}


CvCity* CvPlayer::initCity(int iX, int iY, bool bBumpUnits, bool bUpdatePlotGroups,
	int iOccupationTimer) // advc.ctr
{
	//PROFILE_FUNC(); // advc.003o
	CvCityAI* pCity = m_cities.AI_add(); // advc.003u: was = addCity()
	if (pCity == NULL)
	{
		FAssertMsg(pCity != NULL, "FLTA failed to allocate storage");
		return NULL;
	}
	FAssertMsg(!GC.getMap().plot(iX, iY)->isCity(), "No city is expected at this plot when initializing new city");
	pCity->init(pCity->getID(), getID(), iX, iY, bBumpUnits, bUpdatePlotGroups,
			iOccupationTimer); // advc.ctr
	/*  advc.104: Moved out of CvCity::init so that the new city is
		already fully initialized */
	setFoundedFirstCity(true);
	return pCity;
}


void CvPlayer::acquireCity(CvCity* pOldCity, bool bConquest, bool bTrade, bool bUpdatePlotGroups,  // advc: style changes; comments (there was only one before)
	bool bPeaceDeal) // advc.ctr
{
	CvPlot& kCityPlot = *pOldCity->plot();
	{	// Kill ICBMs
		CLinkList<IDInfo> oldUnits; // (I doubt that it's necessary to copy the plot's unit list here)
		{
			for (CLLNode<IDInfo> const* pUnitNode = kCityPlot.headUnitNode(); pUnitNode != NULL;
				pUnitNode = kCityPlot.nextUnitNode(pUnitNode))
			{
				oldUnits.insertAtEnd(pUnitNode->m_data);
			}
		}
		CLLNode<IDInfo>* pUnitNode = oldUnits.head();
		while (pUnitNode != NULL)
		{
			CvUnit& kUnit = *::getUnit(pUnitNode->m_data);
			pUnitNode = oldUnits.next(pUnitNode);
			if (kUnit.getTeam() != getTeam())
			{
				if (kUnit.getDomainType() == DOMAIN_IMMOBILE)
					kUnit.kill(false, getID());
			}
		}
	}
	if (bConquest) // Force unowned after conquest
	{
		int const iRange = pOldCity->getCultureLevel();
		for (int iDX = -iRange; iDX <= iRange; iDX++)
		{
			for (int iDY = -iRange; iDY <= iRange; iDY++)
			{
				if (CvCity::cultureDistance(iDX, iDY) > iRange)
					continue;

				CvPlot* pLoopPlot = ::plotXY(kCityPlot.getX(), kCityPlot.getY(), iDX, iDY);
				if (pLoopPlot == NULL)
					continue;

				if (pLoopPlot->getOwner() != pOldCity->getOwner() ||
					pLoopPlot->getNumCultureRangeCities(pOldCity->getOwner()) != 1)
				{
					continue;
				}
				bool bForceUnowned = false;
				for (int i = 0; i < MAX_PLAYERS; i++)
				{
					CvPlayer const& kThirdParty = GET_PLAYER((PlayerTypes)i);
					if (!kThirdParty.isAlive() || kThirdParty.getTeam() == getTeam() ||
						kThirdParty.getTeam() == pOldCity->getTeam())
					{
						continue;
					}
					if (pLoopPlot->getNumCultureRangeCities(kThirdParty.getID()) > 0)
					{
						bForceUnowned = true;
						break;
					}
				}
				if (bForceUnowned)
				{
					static int const iFORCE_UNOWNED_CITY_TIMER = GC.getDefineINT("FORCE_UNOWNED_CITY_TIMER"); // advc.opt
					pLoopPlot->setForceUnownedTimer(iFORCE_UNOWNED_CITY_TIMER);
				}
			}
		}
	}

	// Update city counts
	if (pOldCity->getOriginalOwner() == pOldCity->getOwner())
		GET_PLAYER(pOldCity->getOriginalOwner()).changeCitiesLost(1);
	else if (pOldCity->getOriginalOwner() == getID())
		GET_PLAYER(pOldCity->getOriginalOwner()).changeCitiesLost(-1);

	if (bConquest) // City-captured announcements, replay msg
	{
		CvWString szBuffer(gDLL->getText("TXT_KEY_MISC_CAPTURED_CITY", pOldCity->getNameKey()));
		gDLL->UI().addMessage(getID(), true, -1, szBuffer, pOldCity->getPlot(),
				"AS2D_CITYCAPTURE", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtPath(
				"WORLDBUILDER_CITY_EDIT"), GC.getColorType("GREEN"));
		CvWString szName;
		szName.Format(L"%s (%s)", pOldCity->getName().GetCString(), GET_PLAYER(pOldCity->getOwner()).getReplayName());
		CvWString szCapturedBy(gDLL->getText("TXT_KEY_MISC_CITY_CAPTURED_BY",
				szName.GetCString(), getCivilizationDescriptionKey()));
		for (int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			CvPlayer const& kObs = GET_PLAYER((PlayerTypes)i);
			if (!kObs.isAlive() || kObs.getID() == getID())
				continue;
			if (pOldCity->isRevealed(kObs.getTeam()) ||
				kObs.isSpectator()) // advc.127
			{
				gDLL->UI().addMessage(kObs.getID(), false, -1, szCapturedBy, pOldCity->getPlot(),
						"AS2D_CITYCAPTURED", /* advc.106b: */ MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY,
						ARTFILEMGR.getInterfaceArtPath("WORLDBUILDER_CITY_EDIT"),
						GC.getColorType("RED"));
			}
		}
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szCapturedBy,
				pOldCity->getX(), pOldCity->getY(), GC.getColorType("WARNING_TEXT"));
	} // <advc.ctr> City-ceded announcement, replay msg
	else if (bTrade &&  // CvCity::liberate handles liberation announcement and replay msg.
		pOldCity->getLiberationPlayer() != getID())
	{
		CvWString szHasCeded;
		{	// Need to cache these locally
			CvWString szOldOwnerReplayName = GET_PLAYER(pOldCity->getOwner()).getReplayName();
			CvWString szNewOwnerReplayName = getReplayName();
			szHasCeded = gDLL->getText("TXT_KEY_MISC_CITY_CEDED_TO",
					/*	Don't obscure any names; isRevealed in the loop implies
						isHasSeen (though not isHasMet). */
					szOldOwnerReplayName.GetCString(), pOldCity->getNameKey(),
					szNewOwnerReplayName.GetCString());
		}
		// Don't announce if there's a reparations announcement
		if (!bPeaceDeal || !GC.getDefineBOOL(CvGlobals::ANNOUNCE_REPARATIONS))
		{
			for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
			{
				CvPlayer const& kObs = *it;
				if (kObs.getID() == getID() || kObs.getID() == pOldCity->getOwner())
					continue;
				if (!pOldCity->isRevealed(kObs.getTeam()) &&
					!kObs.isSpectator()) // advc.127
				{
					continue;
				}
				/*	advc.071: Meet before the announcement (with indicator at
					city coordinates). Callee will perform the relevant checks. */
				pOldCity->meetNewOwner(kObs.getTeam(), getTeam());
				gDLL->UI().addMessage(kObs.getID(), false, -1, szHasCeded, kCityPlot,
						NULL, /* advc.106b: */ MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY,
						ARTFILEMGR.getInterfaceArtPath("WORLDBUILDER_CITY_EDIT"),
						GC.getColorType("HIGHLIGHT_TEXT"));
			}
		}
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szHasCeded,
				kCityPlot.getX(), kCityPlot.getY(), GC.getColorType("HIGHLIGHT_TEXT"));
	} // </advc.ctr>

	// Capture gold
	int iCaptureGold = 0;
	if (bConquest)
		iCaptureGold = doCaptureGold(*pOldCity); // advc.003y: Moved into subroutine

	// Deactivate vote sources before killing old city
	for (int i = 0; i < GC.getNumVoteSourceInfos(); i++)
		pOldCity->processVoteSource((VoteSourceTypes)i, false);

	/*  Preserve city data in temporary variables (but not any data handled by the
		CvCity::process... functions) */
	bool* pabHasReligion = new bool[GC.getNumReligionInfos()];
	bool* pabHolyCity = new bool[GC.getNumReligionInfos()];
	bool* pabHasCorporation = new bool[GC.getNumCorporationInfos()];
	bool* pabHeadquarters = new bool[GC.getNumCorporationInfos()];
	int* paiNumRealBuilding = new int[GC.getNumBuildingInfos()];
	int* paiBuildingOriginalOwner = new int[GC.getNumBuildingInfos()];
	int* paiBuildingOriginalTime = new int[GC.getNumBuildingInfos()];

	PlayerTypes eOldOwner = pOldCity->getOwner();
	PlayerTypes eOriginalOwner = pOldCity->getOriginalOwner();
	PlayerTypes eHighestCulturePlayer = pOldCity->findHighestCulture();
	bool bRecapture = (eHighestCulturePlayer != NO_PLAYER ?
			(GET_PLAYER(eHighestCulturePlayer).getTeam() == getTeam()) : false);
	int iGameTurnFounded = pOldCity->getGameTurnFounded();
	int iPopulation = pOldCity->getPopulation();
	int iHighestPopulation = pOldCity->getHighestPopulation();
	int iHurryAngerTimer = pOldCity->getHurryAngerTimer();
	int iConscriptAngerTimer = pOldCity->getConscriptAngerTimer();
	int iDefyResolutionAngerTimer = pOldCity->getDefyResolutionAngerTimer();
	int iOccupationTimer = pOldCity->getOccupationTimer();
	CvWString szName(pOldCity->getNameKey());
	int iDamage = pOldCity->getDefenseDamage();
	bool bBombarded = pOldCity->isBombarded(); // advc.004c
	int iOldCityId = pOldCity->getID();

	std::vector<int> aeFreeSpecialists;
	for (int i = 0; i < GC.getNumSpecialistInfos(); i++)
		aeFreeSpecialists.push_back(pOldCity->getAddedFreeSpecialistCount((SpecialistTypes)i));
	
	bool abEverOwned[MAX_PLAYERS];
	int aiCulture[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		abEverOwned[i] = pOldCity->isEverOwned((PlayerTypes)i);
		aiCulture[i] = pOldCity->getCultureTimes100((PlayerTypes)i);
	}
	abEverOwned[getID()] = true;

	for (int i = 0; i < GC.getNumReligionInfos(); i++)
	{
		pabHasReligion[i] = pOldCity->isHasReligion((ReligionTypes)i);
		pabHolyCity[i] = pOldCity->isHolyCity((ReligionTypes)i);
	}
	for (int i = 0; i < GC.getNumCorporationInfos(); i++)
	{
		pabHasCorporation[i] = pOldCity->isHasCorporation((CorporationTypes)i);
		pabHeadquarters[i] = pOldCity->isHeadquarters((CorporationTypes)i);
	}
	for (int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		paiNumRealBuilding[i] = pOldCity->getNumRealBuilding((BuildingTypes)i);
		paiBuildingOriginalOwner[i] = pOldCity->getBuildingOriginalOwner((BuildingTypes)i);
		paiBuildingOriginalTime[i] = pOldCity->getBuildingOriginalTime((BuildingTypes)i);
	}
	// <advc.001f>
	bool m_abRevealed[MAX_TEAMS];
	for(int i = 0; i < MAX_TEAMS; i++)
		m_abRevealed[i] = pOldCity->isRevealed((TeamTypes)i);
	// </advc.001f>
	std::vector<BuildingYieldChange> aBuildingYieldChange;
	std::vector<BuildingCommerceChange> aBuildingCommerceChange;
	BuildingChangeArray aBuildingHappyChange;
	BuildingChangeArray aBuildingHealthChange;
	for (int i = 0; i < GC.getNumBuildingClassInfos(); i++)
	{
		BuildingClassTypes eBuildingClass = (BuildingClassTypes)i;
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
		{
			BuildingYieldChange kChange;
			kChange.eBuildingClass = eBuildingClass;
			kChange.eYield = (YieldTypes)iYield;
			kChange.iChange = pOldCity->getBuildingYieldChange(eBuildingClass, (YieldTypes)iYield);
			if (kChange.iChange != 0)
				aBuildingYieldChange.push_back(kChange);
		}
		for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
		{
			BuildingCommerceChange kChange;
			kChange.eBuildingClass = (BuildingClassTypes)i;
			kChange.eCommerce = (CommerceTypes)iCommerce;
			kChange.iChange = pOldCity->getBuildingCommerceChange(eBuildingClass, (CommerceTypes)iCommerce);
			if (kChange.iChange != 0)
				aBuildingCommerceChange.push_back(kChange);
		}
		int iChange = pOldCity->getBuildingHappyChange(eBuildingClass);
		if (iChange != 0)
			aBuildingHappyChange.push_back(std::make_pair(eBuildingClass, iChange));
		iChange = pOldCity->getBuildingHealthChange(eBuildingClass);
		if (iChange != 0)
			aBuildingHealthChange.push_back(std::make_pair(eBuildingClass, iChange));
	}

	pOldCity->kill(false, /* advc.001: */ false); // Don't bump units yet
	pOldCity = NULL; // advc: Shouldn't access that past this point

	if (bTrade) // Repercussions of cession: tile culture, war success (city culture: further down)
	{
		// <advc.ctr>
		for (CityPlotIter it(kCityPlot); it.hasNext(); ++it)
		{
			int iConvertedCulture = cultureConvertedUponCityTrade(
					kCityPlot, *it, eOldOwner, getID());
			it->changeCulture(eOldOwner, -iConvertedCulture, false);
			it->changeCulture(getID(), iConvertedCulture, false);
		}
		// BtS code replaced by the loop above // </advc.ctr>
		/*for (int iDX = -1; iDX <= 1; iDX++) {
			for (int iDY = -1; iDY <= 1; iDY++) {
				CvPlot* pLoopPlot = ::plotXY(kCityPlot.getX(), kCityPlot.getY(), iDX, iDY);
				if (pLoopPlot != NULL)
					pLoopPlot->setCulture(eOldOwner, 0, false, false);
			}
		}*/
		// <advc.123d>
		for(int i = 0; i < MAX_CIV_TEAMS; i++)
		{
			CvTeam const& t = GET_TEAM((TeamTypes)i);
			if(!t.isAlive() || t.isMinorCiv() || t.getID() == TEAMID(eOldOwner) ||
				t.getID() == getTeam() || !t.isAtWar(TEAMID(eOldOwner)))
			{
				continue;
			}
			bool bEverOwned = false;
			for(int j = 0; j < MAX_CIV_PLAYERS; j++)
			{
				CvPlayer const& kMember = GET_PLAYER((PlayerTypes)j);
				if(kMember.isAlive() && kMember.getTeam() == t.getID() &&
					abEverOwned[kMember.getID()])
				{
					bEverOwned = true;
					break;
				}
			}
			if(bEverOwned)
			{
				GET_TEAM(eOldOwner).AI_changeWarSuccess(t.getID(),
						-std::min(GC.getWAR_SUCCESS_CITY_CAPTURING(),
						GET_TEAM(eOldOwner).AI_getWarSuccess(t.getID())));
			}
		} // </advc.123d>
	}

	// Create new city and assign data from temporary variables
	CvCity* pNewCity = initCity(kCityPlot.getX(), kCityPlot.getY(), !bConquest, false,
			// advc.ctr: Moved (way) up
			(bTrade && !bRecapture) ? iOccupationTimer : 0);
	FAssert(pNewCity != NULL);
	CvCityAI& kNewCity = pNewCity->AI(); // advc.003u

	kNewCity.setPreviousOwner(eOldOwner);
	kNewCity.setOriginalOwner(eOriginalOwner);
	kNewCity.setGameTurnFounded(iGameTurnFounded);
	kNewCity.setPopulation((bConquest && !bRecapture) ? std::max(1, (iPopulation - 1)) : iPopulation);
	kNewCity.setHighestPopulation(iHighestPopulation);
	kNewCity.setName(szName, /* advc.106k: */ false, true);
	kNewCity.setNeverLost(false);
	kNewCity.changeDefenseDamage(iDamage);
	/*  advc.004c: The above will set the city as bombarded if iDamage>0.
		But iDamage>0 doesn't imply that the city was recently bombarded. */
	kNewCity.setBombarded(bBombarded);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		PlayerTypes eLoopPlayer = (PlayerTypes)i;
		kNewCity.setEverOwned(eLoopPlayer, abEverOwned[eLoopPlayer]);
		kNewCity.setCultureTimes100(eLoopPlayer, aiCulture[eLoopPlayer], false, false);
	} // <dlph.23>
	if(bTrade) // Further repercussions of cession: city culture
	{
		int iOldOwnerCulture = kNewCity.getCultureTimes100(eOldOwner);
		int iNewOwnerCulture = kNewCity.getCultureTimes100(getID());
		// Round down to a multiple of 100
		int iConvertedCulture = (iOldOwnerCulture / 300) * 100;
		kNewCity.setCultureTimes100(getID(),
				iNewOwnerCulture + iConvertedCulture, true, false);
		kNewCity.setCultureTimes100(eOldOwner,
				iOldOwnerCulture - iConvertedCulture, true, false);
	} // </dlph.23>

	// Destruction of buildings
	for (int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		BuildingTypes eBuilding = (BuildingTypes)i;
		if (paiNumRealBuilding[eBuilding] <= 0)
			continue;

		CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
		BuildingClassTypes eBuildingClass = kBuilding.getBuildingClassType();
		// Can't acquire another civ's unique building
		if (!kBuilding.isWorldWonder()) // So that Barbarians can capture wonders
			eBuilding = getCivilization().getBuilding(eBuildingClass);
		if (eBuilding == NO_BUILDING)
			continue;

		// Can acquire never-capture buildings only through city trade
		if (!bTrade && kBuilding.isNeverCapture())
			continue;

		if (isProductionMaxedBuildingClass(eBuildingClass, true) ||
			!kNewCity.isValidBuildingLocation(eBuilding))
		{
			continue;
		}
		// Capture roll unless recapture
		int iOdds = kBuilding.getConquestProbability();
		if (!bConquest || bRecapture || (iOdds > 0 &&
			iOdds > GC.getGame().getSorenRandNum(100, "Capture Probability")))
		{
			kNewCity.setNumRealBuildingTimed(eBuilding, std::min(GC.getDefineINT(CvGlobals::CITY_MAX_NUM_BUILDINGS),
					kNewCity.getNumRealBuilding(eBuilding) + paiNumRealBuilding[eBuilding]),
					false, (PlayerTypes)paiBuildingOriginalOwner[eBuilding], paiBuildingOriginalTime[eBuilding]);
		}
	}

	for (std::vector<BuildingYieldChange>::iterator it = aBuildingYieldChange.begin(); it != aBuildingYieldChange.end(); ++it)
		kNewCity.setBuildingYieldChange(it->eBuildingClass, it->eYield, it->iChange);
	for (std::vector<BuildingCommerceChange>::iterator it = aBuildingCommerceChange.begin(); it != aBuildingCommerceChange.end(); ++it)
		kNewCity.setBuildingCommerceChange(it->eBuildingClass, it->eCommerce, it->iChange);
	for (BuildingChangeArray::iterator it = aBuildingHappyChange.begin(); it != aBuildingHappyChange.end(); ++it)
		kNewCity.setBuildingHappyChange(it->first, it->second);
	for (BuildingChangeArray::iterator it = aBuildingHealthChange.begin(); it != aBuildingHealthChange.end(); ++it)
		kNewCity.setBuildingHealthChange(it->first, it->second);
	for (int i = 0; i < GC.getNumSpecialistInfos(); i++)
		kNewCity.changeFreeSpecialistCount((SpecialistTypes)i, aeFreeSpecialists[i]);
	for (int i = 0; i < GC.getNumReligionInfos(); i++)
	{
		if (pabHasReligion[i])
			kNewCity.setHasReligion(((ReligionTypes)i), true, false, true);
		if (pabHolyCity[i])
			GC.getGame().setHolyCity(((ReligionTypes)i), &kNewCity, false);
	}
	for (int i = 0; i < GC.getNumCorporationInfos(); i++)
	{
		if (pabHasCorporation[i])
			kNewCity.setHasCorporation((CorporationTypes)i, true, false);
		if (pabHeadquarters[i])
			GC.getGame().setHeadquarters((CorporationTypes)i, &kNewCity, false);
	}
	if (bTrade)
	{
		//if (isHuman() || getTeam() == TEAMID(eOldOwner))
		// advc.ctr: Only copy the anger timers if a reset to 0 could be exploited
		if (isHuman() && (GET_TEAM(eOldOwner).isHuman() || TEAMID(eOldOwner) == getTeam()))
		{
			kNewCity.changeHurryAngerTimer(iHurryAngerTimer);
			kNewCity.changeConscriptAngerTimer(iConscriptAngerTimer);
			kNewCity.changeDefyResolutionAngerTimer(iDefyResolutionAngerTimer);
		}
		/*if (!bRecapture) // advc.ctr: Moved up
			kNewCity.changeOccupationTimer(iOccupationTimer);*/
	}

	if (bConquest) // Set occupation timer, bump units
	{
		int iTeamCulturePercent = kNewCity.calculateTeamCulturePercent(getTeam());
		if (iTeamCulturePercent < GC.getDefineINT("OCCUPATION_CULTURE_PERCENT_THRESHOLD"))
		{
			int iPopPercent = GC.getDefineINT("OCCUPATION_TURNS_POPULATION_PERCENT");
			kNewCity.changeOccupationTimer(
				/*  advc.023: Population size as upper bound, and iPopPercent set to
					0 through XML. (Im multiplying by 1+100*iPopPercent so that the
					upper bound has no effect if iPopPercent is set back to 50 in XML.)
					NB: iTeamCulturePercent is city culture, not tile culture;
					only relevant when a city is reconquered. */
				std::min(kNewCity.getPopulation() * (1 + iPopPercent * 100),
				((GC.getDefineINT("BASE_OCCUPATION_TURNS") +
				((kNewCity.getPopulation() * iPopPercent) / 100)) *
				(100 - iTeamCulturePercent)) / 100));
		}
		GC.getMap().verifyUnitValidPlot();
	}
	// Update visibility, plot groups
	//pCityPlot->setRevealed(GET_PLAYER(eOldOwner).getTeam(), true, false, NO_TEAM, false);
	/*  <advc.001f> Need to reveal the new city so that the raze message is
		properly delivered. Reveal the tile once the raze decision is through
		(too early here). */
	for(int i = 0; i < MAX_TEAMS; i++)
	{
		if(m_abRevealed[i])
			kNewCity.setRevealed((TeamTypes)i, true);
	} // </advc.001f>
	kNewCity.updateEspionageVisibility(false);
	if (bUpdatePlotGroups)
		GC.getGame().updatePlotGroups();

	// Notify observers
	// <advc.104>
	if(getUWAI.isEnabled() || getUWAI.isEnabled(true))
	{
		for(int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)i);
			if(kPlayer.isAlive() && !kPlayer.isMinorCiv())
				kPlayer.uwai().getCache().reportCityOwnerChanged(&kNewCity, eOldOwner);
		}
	} // </advc.104>
	CvEventReporter::getInstance().cityAcquired(eOldOwner, getID(), &kNewCity, bConquest, bTrade);
	if (gPlayerLogLevel >= 1) // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
		logBBAI("  Player %d (%S) acquires city %S bConq %d bTrade %d", getID(), getCivilizationDescription(0), kNewCity.getName(0).GetCString(), bConquest, bTrade);

	// Deallocate temporary city data
	SAFE_DELETE_ARRAY(pabHasReligion);
	SAFE_DELETE_ARRAY(pabHolyCity);
	SAFE_DELETE_ARRAY(pabHasCorporation);
	SAFE_DELETE_ARRAY(pabHeadquarters);
	SAFE_DELETE_ARRAY(paiNumRealBuilding);
	SAFE_DELETE_ARRAY(paiBuildingOriginalOwner);
	SAFE_DELETE_ARRAY(paiBuildingOriginalTime);

	// Allow razing, disbanding
	if (bConquest)
	{
		bool bRazeImpossible = false; // advc.003y
		if (GC.getPythonCaller()->canRaze(kNewCity, getID()))
		{	// auto raze based on game rules
			if (kNewCity.isAutoRaze())
			{
				if (iCaptureGold > 0)
				{
					CvWString szBuffer(gDLL->getText("TXT_KEY_MISC_PILLAGED_CITY",
							iCaptureGold, kNewCity.getNameKey()));
					gDLL->UI().addMessage(getID(), true, -1, szBuffer,
							kNewCity.getPlot(), "AS2D_CITYRAZE",
							MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY, // advc.106b
							ARTFILEMGR.getInterfaceArtPath("WORLDBUILDER_CITY_EDIT"),
							GC.getColorType("GREEN"));
				}
				kNewCity.doTask(TASK_RAZE);
			}
			else if (!isHuman())
				AI().AI_conquerCity(kNewCity); // could delete the pointer...
			else
			{	// popup raze option
				bool bRaze = canRaze(kNewCity);
				// <advc> Simplified
				PlayerTypes eLiberationPlayer = kNewCity.getLiberationPlayer(true);
				bool bGift = (eLiberationPlayer != NO_PLAYER &&
						eLiberationPlayer != getID() &&
						GET_TEAM(getTeam()).canPeacefullyEnter(TEAMID(eLiberationPlayer)));
				// </advc>  <advc.ctr> Make sure that the ownership change is legal
				if (bGift)
				{	// Don't check denial though; recipient can't refuse.
					bGift = canTradeCityTo(eLiberationPlayer, kNewCity, true);
				} // </advc.ctr>
				if (bRaze || bGift)
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_RAZECITY);
					pInfo->setData1(kNewCity.getID());
					//pInfo->setData2(eLiberationPlayer);
					// advc: To communicate bGift to CvDLLButtonPopup::launchRazeCityPopup
					pInfo->setData2(bGift ? eLiberationPlayer : NO_PLAYER);
					pInfo->setData3(iCaptureGold);
					gDLL->UI().addPopup(pInfo, getID());
				} // <advc.003y> (based on K-Mod code)
				else bRazeImpossible = true;
			}
		}
		else bRazeImpossible = true;
		if (bRazeImpossible) // </advc.003y>
		{
			// K-Mod. properly handle the case where python says we can't raze the city
			CvEventReporter::getInstance().cityAcquiredAndKept(getID(), &kNewCity);
			if (isHuman())
				kNewCity.chooseProduction();
			// K-Mod end
		}
	}
	else if (!bTrade)
	{
		if (isHuman())
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_DISBANDCITY);
			pInfo->setData1(kNewCity.getID());
			gDLL->UI().addPopup(pInfo, getID());
		}
		else CvEventReporter::getInstance().cityAcquiredAndKept(getID(), &kNewCity);
	} // <advc.004x>
	if(bTrade && isHuman())
		kNewCity.chooseProduction(); // </advc.004x>

	// Forcing events that deal with the old city not to expire just because we conquered that city [BtS comment]
	for (CvEventMap::iterator it = m_mapEventsOccured.begin(); it != m_mapEventsOccured.end(); ++it)
	{
		EventTriggeredData &triggerData = it->second;
		if(triggerData.m_eOtherPlayer == eOldOwner && triggerData.m_iOtherPlayerCityId == iOldCityId)
			triggerData.m_iOtherPlayerCityId = -1;
	}
	// <advc.001f>
	for(int i = 0; i < MAX_TEAMS; i++)
	{
		if(m_abRevealed[i])
			kCityPlot.setRevealed((TeamTypes)i, true, false, NO_TEAM, false);
	} // </advc.001f>
	/* <advc.001> Elimination otherwise happens only in CvGame::update, which
	   appears to be called only during the human player's turn. That means a dead
	   AI player can get one more turn before being eliminated. Not normally a problem
	   because there isn't much that a civ without cities can do, but I've had a
	   case where a dead player arranged a vassal agreement. */
	if(eOldOwner != NO_PLAYER)
		GET_PLAYER(eOldOwner).verifyAlive(); // </advc.001>
	// <advc.130w>
	AI().AI_updateCityAttitude(kCityPlot);
	GET_PLAYER(eOldOwner).AI_updateCityAttitude(kCityPlot); // </advc.130w>
}

// advc.ctr:
int CvPlayer::cultureConvertedUponCityTrade(CvPlot const& kCityPlot, CvPlot const& kPlot,
	PlayerTypes eOldOwner, PlayerTypes eNewOwner, bool bIgnorePriority) const
{
	bool bConvert = false;
	if (bIgnorePriority)
		bConvert = (::plotDistance(&kCityPlot, &kPlot) <= 2);
	else
	{
		// Always convert culture in the inner radius
		bConvert = ::adjacentOrSame(kPlot, kCityPlot);
		if (!bConvert)
		{
			// Outer circle: Based on plot priority when contested
			CvCity const* pDefaultWorkingCity = kPlot.defaultWorkingCity();
			bConvert = (pDefaultWorkingCity != NULL &&
					/*	Plot has to be assigned to the traded city or to
						a city of the new owner */
					(pDefaultWorkingCity->at(kCityPlot) ||
					pDefaultWorkingCity->getOwner() == eNewOwner));
		}
	}
	if (!bConvert)
		return 0;
	return std::min(kPlot.getCulture(eOldOwner) / 2,
			2 * kPlot.getCulture(getID()));
}


void CvPlayer::killCities()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->kill(false);
	GC.getGame().updatePlotGroups();
}


CvWString CvPlayer::getNewCityName() const
{
	CvWString szName;
	/*for (pNode = headCityNameNode(); (pNode != NULL); pNode = nextCityNameNode(pNode)) {
		szName = gDLL->getText(pNode->m_data);
		if (isCityNameValid(szName, true)) {
			szName = pNode->m_data;
			break;
		}
	}*/ // BtS
	// K-Mod
	for (CLLNode<CvWString>* pNode = headCityNameNode(); pNode && szName.empty(); pNode = nextCityNameNode(pNode))
	{
		szName = gDLL->getText(pNode->m_data); // (temp use of the buffer)
		if (isCityNameValid(szName, true))
			szName = pNode->m_data;
		else szName.clear(); // clear the buffer if the name is not valid!
	}
	// Note: unfortunately, the name-skipping system in getCivilizationCityName does not apply here.
	// K-Mod end

	if (szName.empty())
		getCivilizationCityName(szName, getCivilizationType());

	if (szName.empty())
	{
		// Pick a name from another random civ
		int iRandOffset = GC.getGame().getSorenRandNum(GC.getNumCivilizationInfos(), "Place Units (Player)");
		for (int iI = 0; iI < GC.getNumCivilizationInfos(); iI++)
		{
			int iLoopName = ((iI + iRandOffset) % GC.getNumCivilizationInfos());
			getCivilizationCityName(szName, ((CivilizationTypes)iLoopName));
			if (!szName.empty())
				break;
		}
	}
	if (szName.empty())
		szName = "TXT_KEY_CITY";

	return szName;
}


void CvPlayer::getCivilizationCityName(CvWString& szBuffer, CivilizationTypes eCivilization) const
{
	int iRandOffset;
	int iLoopName;
	int iI;

	/* original bts code
	if (isBarbarian() || isMinorCiv())
		iRandOffset = GC.getGame().getSorenRandNum(GC.getInfo(eCivilization).getNumCityNames(), "Place Units (Player)");
	else iRandOffset = 0;*/
	// K-Mod
	if (eCivilization != getCivilizationType() || isBarbarian() || isMinorCiv())
		iRandOffset = GC.getGame().getSorenRandNum(GC.getInfo(eCivilization).getNumCityNames(), "City name offset");
	else
		iRandOffset = std::max(0, getPlayerRecord()->getNumCitiesBuilt() - getNumCityNames()); // note: the explicit city names list is checked before this function is called.
	// K-Mod end

	for (iI = 0; iI < GC.getInfo(eCivilization).getNumCityNames(); iI++)
	{
		iLoopName = ((iI + iRandOffset) % GC.getInfo(eCivilization).getNumCityNames());

		CvWString szName = gDLL->getText(GC.getInfo(eCivilization).getCityNames(iLoopName));

		if (isCityNameValid(szName, true))
		{
			szBuffer = GC.getInfo(eCivilization).getCityNames(iLoopName);
			break;
		}
	}
}


bool CvPlayer::isCityNameValid(CvWString& szName, bool bTestDestroyed) const
{
	if (bTestDestroyed)
	{
		if (GC.getGame().isDestroyedCityName(szName))
			return false;

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			// <advc.opt>
			if (!kLoopPlayer.isEverAlive())
				continue; // </advc.opt>
			FOR_EACH_CITY(pLoopCity, kLoopPlayer)
			{
				if (pLoopCity->getName() == szName)
					return false;
			}
		}
	}
	else
	{
		FOR_EACH_CITY(pLoopCity, *this)
		{
			if (pLoopCity->getName() == szName)
				return false;
		}
	}
	return true;
}


CvUnit* CvPlayer::initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection)
{
	//PROFILE_FUNC(); // advc.003o

	CvUnitAI* pUnit = m_units.AI_add(); // advc.003u: was = addUnit()
	if (pUnit == NULL)
	{
		FAssertMsg(pUnit != NULL, "FLTA failed to allocate storage");
		return NULL;
	}
	FAssert(eUnit != NO_UNIT);
	pUnit->init(pUnit->getID(), eUnit, (UnitAITypes)
			(eUnitAI == NO_UNITAI ? GC.getInfo(eUnit).getDefaultUnitAIType() : eUnitAI),
			getID(), iX, iY, eFacingDirection);
	return pUnit;
}


void CvPlayer::disbandUnit(bool bAnnounce)
{
	int iBestValue = MAX_INT;
	CvUnit* pBestUnit = NULL;
	FOR_EACH_UNIT_VAR(pLoopUnit, *this)
	{
		if (pLoopUnit->hasCargo() || pLoopUnit->isGoldenAge() ||
			pLoopUnit->getUnitInfo().getProductionCost() <= 0)
		{
			continue;
		}
		if (!pLoopUnit->isMilitaryHappiness() || !pLoopUnit->getPlot().isCity() ||
			pLoopUnit->getPlot().plotCount(PUF_isMilitaryHappiness, -1, -1, getID()) > 1)
		{
			int iValue = (10000 + GC.getGame().getSorenRandNum(1000, "Disband Unit"));
			iValue += (pLoopUnit->getUnitInfo().getProductionCost() * 5);
			iValue += (pLoopUnit->getExperience() * 20);
			iValue += (pLoopUnit->getLevel() * 100);

			if (pLoopUnit->canDefend() && pLoopUnit->getPlot().isCity()
				/*  advc.001s: I suppose this clause is intended for
				potential city defenders */
				&& pLoopUnit->getDomainType() == DOMAIN_LAND)
				iValue *= 2;

			if (pLoopUnit->getPlot().getTeam() == pLoopUnit->getTeam())
				iValue *= 3;

			switch (pLoopUnit->AI_getUnitAIType())
			{
			case UNITAI_UNKNOWN:
			case UNITAI_ANIMAL:
				break;

			case UNITAI_SETTLE:
				iValue *= 20;
				break;

			case UNITAI_WORKER:
				iValue *= 10;
				break;

			case UNITAI_ATTACK:
			case UNITAI_ATTACK_CITY:
			case UNITAI_COLLATERAL:
			case UNITAI_PILLAGE:
			case UNITAI_RESERVE:
			case UNITAI_COUNTER:
				iValue *= 2;
				break;

			case UNITAI_CITY_DEFENSE:
			case UNITAI_CITY_COUNTER:
			case UNITAI_CITY_SPECIAL:
			case UNITAI_PARADROP:
				iValue *= 6;
				break;

			case UNITAI_EXPLORE:
				iValue *= 15;
				break;

			case UNITAI_MISSIONARY:
				iValue *= 8;
				break;

			case UNITAI_PROPHET:
			case UNITAI_ARTIST:
			case UNITAI_SCIENTIST:
			case UNITAI_GENERAL:
			case UNITAI_MERCHANT:
			case UNITAI_ENGINEER:
			case UNITAI_GREAT_SPY: // K-Mod
				break;

			case UNITAI_SPY:
				iValue *= 12;
				break;

			case UNITAI_ICBM:
				iValue *= 4;
				break;

			case UNITAI_WORKER_SEA:
				iValue *= 18;
				break;

			case UNITAI_ATTACK_SEA:
			case UNITAI_RESERVE_SEA:
			case UNITAI_ESCORT_SEA:
				break;

			case UNITAI_EXPLORE_SEA:
				iValue *= 25;
				break;

			case UNITAI_ASSAULT_SEA:
			case UNITAI_SETTLER_SEA:
			case UNITAI_MISSIONARY_SEA:
			case UNITAI_SPY_SEA:
			case UNITAI_CARRIER_SEA:
			case UNITAI_MISSILE_CARRIER_SEA:
				iValue *= 5;
				break;

			case UNITAI_PIRATE_SEA:
			case UNITAI_ATTACK_AIR:
				break;

			case UNITAI_DEFENSE_AIR:
			case UNITAI_CARRIER_AIR:
			case UNITAI_MISSILE_AIR:
				iValue *= 3;
				break;

			default:
				FAssert(false);
			}

			if (pLoopUnit->getUnitInfo().getExtraCost() > 0)
				iValue /= (pLoopUnit->getUnitInfo().getExtraCost() + 1);

			if (iValue < iBestValue)
			{
				iBestValue = iValue;
				pBestUnit = pLoopUnit;
			}
		}
	}

	if (pBestUnit != NULL)
	{
		wchar szBuffer[1024];
		swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_UNIT_DISBANDED_NO_MONEY",
				pBestUnit->getNameKey()).GetCString());
		gDLL->UI().addMessage(getID(), false, -1, szBuffer, pBestUnit->getPlot(),
				"AS2D_UNITDISBANDED", MESSAGE_TYPE_MINOR_EVENT, pBestUnit->getButton(),
				GC.getColorType("RED"));

		FAssert(!pBestUnit->isGoldenAge());

		pBestUnit->kill(false);
	}
}


void CvPlayer::killUnits()
{
	FOR_EACH_UNIT_VAR(pLoopUnit, *this)
		pLoopUnit->kill(false);
}


// XXX should pUnit be a CvSelectionGroup???
// Returns the next unit in the cycle...
CvSelectionGroup* CvPlayer::cycleSelectionGroups(CvUnit* pUnit, bool bForward,
	bool bWorkers, bool* pbWrap)
{
	FAssert(GC.getGame().getActivePlayer() == getID() && isHuman());
	// <advc.004h>
	if(pUnit->canFound())
		pUnit->updateFoundingBorder(true); // </advc.004h>
	// K-Mod
	bool bDummy;
	// this means we can just use bWrap directly and it will update *pbWrap if need be.
	bool& bWrap = pbWrap ? *pbWrap : bDummy;
	std::set<int>& cycled_groups = GC.getGame().getActivePlayerCycledGroups();
	// K-Mod end

	/* if (pbWrap != NULL)
		*pbWrap = false;*/
	bWrap = false;

	CLLNode<int>* pSelectionGroupNode = headGroupCycleNode();
	if (pUnit != NULL)
	{
		while (pSelectionGroupNode != NULL)
		{
			if (getSelectionGroup(pSelectionGroupNode->m_data) == pUnit->getGroup())
			{
				// K-Mod
				if (isTurnActive())
					cycled_groups.insert(pSelectionGroupNode->m_data);
				//
				if (bForward)
					pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
				else pSelectionGroupNode = previousGroupCycleNode(pSelectionGroupNode);
				break;
			}
			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
		}
	}

	if (pSelectionGroupNode == NULL)
	{
		if (bForward)
			pSelectionGroupNode = headGroupCycleNode();
		else pSelectionGroupNode = tailGroupCycleNode();

		/* if (pbWrap != NULL)
			*pbWrap = true;*/ // disabled by K-Mod
	}

	if(pSelectionGroupNode == NULL)
		return NULL;

	CLLNode<int>* pFirstSelectionGroupNode = pSelectionGroupNode;
	while (true)
	{
		CvSelectionGroup* pLoopSelectionGroup = getSelectionGroup(pSelectionGroupNode->m_data);
		FAssertMsg(pLoopSelectionGroup != NULL, "LoopSelectionGroup is not assigned a valid value");

		if (pLoopSelectionGroup->readyToSelect()
				&& cycled_groups.count(pSelectionGroupNode->m_data) == 0) // K-Mod
		{
			if (!bWorkers || pLoopSelectionGroup->hasWorker())
			{
				/*if (pUnit && pLoopSelectionGroup == pUnit->getGroup()) {
					if (pbWrap != NULL)
						*pbWrap = true;
				}*/
				return pLoopSelectionGroup;
			}
		}

		if (bForward)
		{
			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);

			if (pSelectionGroupNode == NULL)
			{
				pSelectionGroupNode = headGroupCycleNode();
				/* if (pbWrap != NULL)
					*pbWrap = true;*/
			}
		}
		else
		{
			pSelectionGroupNode = previousGroupCycleNode(pSelectionGroupNode);

			if (pSelectionGroupNode == NULL)
			{
				pSelectionGroupNode = tailGroupCycleNode();
				/* if (pbWrap != NULL)
					*pbWrap = true;*/
			}
		}

		if (pSelectionGroupNode == pFirstSelectionGroupNode)
		{
			// break;
			// K-Mod
			if (bWrap)
				break;
			else
			{
				cycled_groups.clear();
				bWrap = true;
			}
			// K-Mod end
		}
	} //

	return NULL;
}


bool CvPlayer::hasTrait(TraitTypes eTrait) const
{
	FAssert(getLeaderType() >= 0);
	FAssert(eTrait >= 0);
	return GC.getInfo(getLeaderType()).hasTrait(eTrait);
}

// AI_AUTO_PLAY_MOD, 07/09/08, jdog5000: START
void CvPlayer::setHumanDisabled(bool bNewVal)
{
	// <advc.127>
	m_bAutoPlayJustEnded = true;
	CvGame& kGame = GC.getGame();
	// Not sure if this is needed:
	bool const bActive = (kGame.getActivePlayer() == getID());
	CvWString szReplayText;
	if (bNewVal && !m_bDisableHuman)
	{
		AI().AI_setHuman(false);
		if(bActive)
		{	// advc.004h:
			gDLL->getEngineIFace()->clearAreaBorderPlots(AREA_BORDER_LAYER_FOUNDING_BORDER);
			gDLL->UI().clearQueuedPopups();
			szReplayText = gDLL->getText("TXT_KEY_AUTO_PLAY_STARTED");
		}
	}
	else if (!bNewVal && m_bDisableHuman)
	{
		AI().AI_setHuman(true);
		m_iNewMessages = 0; // Don't open Event Log when coming out of Auto Play
		if(bActive)
			szReplayText = gDLL->getText("TXT_KEY_AUTO_PLAY_ENDED");
	}
	if(!szReplayText.empty())
	{
		kGame.addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szReplayText,
				-1, -1, GC.getColorType("HIGHLIGHT_TEXT"));
	} // </advc.127>
	m_bDisableHuman = bNewVal;
	updateHuman();
}


bool CvPlayer::isHumanDisabled() /* advc.127: */ const
{
	return m_bDisableHuman;
}

// <advc.127>
bool CvPlayer::isSpectator() const
{
	return isHumanDisabled() && GC.getGame().isDebugMode();
}


bool CvPlayer::isAutoPlayJustEnded() const
{
	return m_bAutoPlayJustEnded;
} // </advc.127>
// AI_AUTO_PLAY_MOD: END

void CvPlayer::updateHuman()
{
	if (getID() == NO_PLAYER)
		m_bHuman = false;
	else
	{
		// AI_AUTO_PLAY_MOD, 09/01/07, MRGENIE:
		if (m_bDisableHuman)
			m_bHuman = false;
		else // AI_AUTO_PLAY_MOD: END
			m_bHuman = GC.getInitCore().getHuman(getID());
	}
}

// K-Mod
static bool concealUnknownCivs()
{
	return GC.getGame().getActiveTeam() != NO_TEAM &&
			//gDLL->getChtLvl() == 0
			// advc.135c: Replacing the above (which doesn't work in multiplayer)
			!GC.getGame().isDebugMode() &&
			!gDLL->GetWorldBuilderMode();
}

// <advc.106i>
void CvPlayer::setSavingReplay(bool b)
{
	m_bSavingReplay = b;
} // </advc.106i>


const wchar* CvPlayer::getName(uint uiForm) const
{
	if (GC.getInitCore().getLeaderName(getID(), uiForm).empty() ||
		(GC.getGame().isMPOption(MPOPTION_ANONYMOUS) && isAlive() &&
		GC.getGame().getGameState() == GAMESTATE_ON))
	{
		return GC.getInfo(getLeaderType()).getDescription(uiForm);
	}
	else
	{	// <advc.106i>
		if(m_bSavingReplay)
		{
			CvWString const szKey = "TXT_KEY_REPLAY_PREFIX";
			CvWString szPrefix = gDLL->getText(szKey);
			// No prefix if the key isn't present
			if(szKey.compare(szPrefix) == 0)
				szPrefix = L"";
			/*  Same hack as above. Don't call gDll->getModName b/c then I'd have
				to deal with narrow/wide string conversion. */
			static CvWString r; // Important to put the assignment on a separate line
			r = szPrefix + GC.getInitCore().getLeaderName(getID(), uiForm);
			return r;
		} // </advc.106i>
		return GC.getInitCore().getLeaderName(getID(), uiForm);
	}
}

// advc.058: New function; unused, see getKnownCivDescription.
const wchar* CvPlayer::getKnownName(TeamTypes eObserver) const
{
	// advc.058: Moved from getName
	// K-Mod. Conceal the leader name of unmet players.
	if (concealUnknownCivs() &&
		!GET_TEAM(eObserver == NO_TEAM ? GC.getGame().getActiveTeam() : eObserver).
		isHasSeen(getTeam()))
	{	// hack to stop the string from going out of scope.
		static CvWString szUnknown = gDLL->getText("TXT_KEY_UNKNOWN");
		return szUnknown;
	} // K-Mod end
	return getName();
}

// K-Mod. Player name to be used in replay
const wchar* CvPlayer::getReplayName(uint uiForm) const
{
	if (GC.getInitCore().getLeaderName(getID(), uiForm).empty() ||
		(GC.getGame().isMPOption(MPOPTION_ANONYMOUS) && isAlive() &&
		GC.getGame().getGameState() == GAMESTATE_ON))
	{
		return GC.getInfo(getLeaderType()).getDescription(uiForm);
	}
	return GC.getInitCore().getLeaderName(getID(), uiForm)./*advc:*/GetCString();
} // K-Mod end


const wchar* CvPlayer::getNameKey() const
{
	if ((GC.getInitCore().getLeaderNameKey(getID()).empty() ||
		GC.getGame().isMPOption(MPOPTION_ANONYMOUS) && isAlive()) &&
		/*  advc.001p: Had a crash here while loading a save from within a game with
			a higher player count than in the save. CvInitCore had already been reset.
			Can perhaps only occur with a debugger attached that slows the DLL down. */
		getLeaderType() != NO_LEADER)
	{
		return GC.getInfo(getLeaderType()).getTextKeyWide();
	}
	return GC.getInitCore().getLeaderNameKey(getID());
}


const wchar* CvPlayer::getCivilizationDescription(uint uiForm) const
{
	if (GC.getInitCore().getCivDescription(getID(), uiForm).empty())
		return GC.getInfo(getCivilizationType()).getDescription(uiForm);
	return GC.getInitCore().getCivDescription(getID(), uiForm);
}

/*	advc.058: New function. Currently unused because civ descriptions
	don't currently need to be concealed anywhere. (The Python screens
	take care of that themselves.) */
wchar const* CvPlayer::getKnownCivDescription(TeamTypes eObserver) const
{
	/*	advc.058: Moved from getCivilizationDescription, active team replaced
		with eObserver. */
	// K-Mod. Conceal the civilization of unmet players.
	if (concealUnknownCivs() &&
		!GET_TEAM(eObserver == NO_TEAM ? GC.getGame().getActiveTeam() : eObserver).
		isHasSeen(getTeam()))
	{	// hack to stop the string from going out of scope.
		static CvWString string = gDLL->getText("TXT_KEY_TOPCIVS_UNKNOWN");
		return string;
	} // K-Mod end
	return getCivilizationDescription();
}


const wchar* CvPlayer::getCivilizationDescriptionKey() const
{
	if (GC.getInitCore().getCivDescriptionKey(getID()).empty())
		return GC.getInfo(getCivilizationType()).getTextKeyWide();
	return GC.getInitCore().getCivDescriptionKey(getID());
}


const wchar* CvPlayer::getCivilizationShortDescription(uint uiForm) const
{
	if (GC.getInitCore().getCivShortDesc(getID(), uiForm).empty())
		return GC.getInfo(getCivilizationType()).getShortDescription(uiForm);
	return GC.getInitCore().getCivShortDesc(getID(), uiForm);
}

// advc.058: New function; unused, see getKnownCivDescription.
const wchar* CvPlayer::getKnownCivShortDescription(TeamTypes eObserver) const
{
	// advc.058: Moved from getCivilizationShortDescription
	// K-Mod. Conceal the civilization of unmet players.
	if (concealUnknownCivs() &&
		!GET_TEAM(eObserver == NO_TEAM ? GC.getGame().getActiveTeam() : eObserver).
		isHasSeen(getTeam()))
	{
		static CvWString szUnknown = gDLL->getText("TXT_KEY_UNKNOWN");
		return szUnknown;
	} // K-Mod end
	return getCivilizationShortDescription();
}


const wchar* CvPlayer::getCivilizationShortDescriptionKey() const
{
	if (GC.getInitCore().getCivShortDescKey(getID()).empty())
		return GC.getInfo(getCivilizationType()).getShortDescriptionKey();
	return GC.getInitCore().getCivShortDescKey(getID());
}


const wchar* CvPlayer::getCivilizationAdjective(uint uiForm) const
{
	if (GC.getInitCore().getCivAdjective(getID(), uiForm).empty())
		return GC.getInfo(getCivilizationType()).getAdjective(uiForm);
	return GC.getInitCore().getCivAdjective(getID(), uiForm);
}

const wchar* CvPlayer::getCivilizationAdjectiveKey() const
{
	if (GC.getInitCore().getCivAdjectiveKey(getID()).empty())
		return GC.getInfo(getCivilizationType()).getAdjectiveKey();
	return GC.getInitCore().getCivAdjectiveKey(getID());
}


CvWString CvPlayer::getFlagDecal() const
{
	if (GC.getInitCore().getFlagDecal(getID()).empty())
		return GC.getInfo(getCivilizationType()).getFlagTexture();
	return GC.getInitCore().getFlagDecal(getID());
}

bool CvPlayer::isWhiteFlag() const
{
	if (GC.getInitCore().getFlagDecal(getID()).empty())
		return GC.getInfo(getCivilizationType()).getArtInfo()->isWhiteFlag();
	return GC.getInitCore().getWhiteFlag(getID());
}

/*	advc.127c: Wrapper for CvInitCore::setFlagDecal that can (bUpdate=true)
	attempt to clear all flag symbols on the map in order to update
	the flag graphics. */
void CvPlayer::setFlagDecal(CvWString const& szFlagDecal, bool bUpdate)
{
	if (&szFlagDecal == &GC.getInitCore().getFlagDecal(getID()))
		return;
	GC.getInitCore().setFlagDecal(getID(), szFlagDecal);
	if (!bUpdate)
		return;
	gDLL->UI().setDirty(Flag_DIRTY_BIT, true);
	if (isBarbarian())
	{
		FAssertMsg(false, "CvPlot::clearFlagSymbol might not work correctly when"
				" updating flag symbols after changing the Barbarian flag decal.");
		return;
	}
	for (int i = 0; i < GC.getMap().numPlots(); i++)
	{
		GC.getMap().plotByIndex(i)->clearFlagSymbol();
	}
}


const wchar* CvPlayer::getStateReligionName(uint uiForm) const
{
	return GC.getInfo(getStateReligion()).getDescription(uiForm);
}

const wchar* CvPlayer::getStateReligionKey() const
{
	if (getStateReligion() != NO_RELIGION)
		return GC.getInfo(getStateReligion()).getTextKeyWide();
	return L"TXT_KEY_MISC_NO_STATE_RELIGION";
}


const CvWString CvPlayer::getBestAttackUnitName(uint uiForm) const
{
	return gDLL->getObjectText((CvString)getBestAttackUnitKey(), uiForm, true);
}


const CvWString CvPlayer::getWorstEnemyName() const
{
	TeamTypes eWorstEnemy = GET_TEAM(getTeam()).AI_getWorstEnemy();
	if (eWorstEnemy != NO_TEAM)
		return GET_TEAM(eWorstEnemy).getName();
	return "";
}

const wchar* CvPlayer::getBestAttackUnitKey() const
{	// advc.079: Code moved into subroutine
	UnitTypes eBestUnit = AI().AI_getBestAttackUnit();
	if (eBestUnit != NO_UNIT)
		return GC.getInfo(eBestUnit).getTextKeyWide();
	return L"TXT_KEY_MISC_NO_UNIT";
}

ArtStyleTypes CvPlayer::getArtStyleType() const
{
	if (GC.getInitCore().getArtStyle(getID()) == NO_ARTSTYLE)
		return ((ArtStyleTypes)GC.getInfo(getCivilizationType()).getArtStyleType());
	return GC.getInitCore().getArtStyle(getID());
}

const TCHAR* CvPlayer::getUnitButton(UnitTypes eUnit) const
{
	return GC.getInfo(eUnit).getArtInfo(0, getCurrentEra(), (UnitArtStyleTypes) GC.getInfo(getCivilizationType()).getUnitArtStyleType())->getButton();
}

/*  advc (comment): Contains the entire sequence of an AI turn, and is called
	when a human player ends his/her turn. */
void CvPlayer::doTurn()  // advc: style changes
{
	PROFILE_FUNC();

	FAssert(isAlive());
	FAssertMsg(!hasBusyUnit() || GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS) ||
			GC.getGame().isSimultaneousTeamTurns(), "End of turn with busy units in a sequential-turn game");
	CvGame& g = GC.getGame();
	// <advc.106b>
	if (!g.isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
		g.setInBetweenTurns(true);
	if (isHuman() && //getStartOfTurnMessageLimit() >= 0 && // The message should be helpful even if the log doesn't auto-open
		g.getElapsedGameTurns() > 0 && !m_listGameMessages.empty())
	{
		gDLL->UI().addMessage(getID(), false, 0, gDLL->getText("TXT_KEY_END_TURN_MSG"), 0,
				MESSAGE_TYPE_EOT, 0, GC.getColorType("LIGHT_GREY"));
	}
	if (isHuman())
		m_iNewMessages = 0;
	/*  This way, NewMessages is never reset for non-humans. It is reset in
		setHumanDisabled though, i.e. when coming out of AI Auto Play. */
	if (isHuman())
		gDLL->UI().clearEventMessages();
	// </advc.106b>

	CvEventReporter::getInstance().beginPlayerTurn(g.getGameTurn(), getID());
	/*  advc.127: Only needs to be true when Civ4lerts are checked, which is done
		in response to beginPlayerTurn. */
	m_bAutoPlayJustEnded = false;

	//doUpdateCacheOnTurn(); // advc: removed
	g.verifyDeals();
	AI().AI_doTurnPre();

	if (getRevolutionTimer() > 0)
		changeRevolutionTimer(-1);
	if (getConversionTimer() > 0)
		changeConversionTimer(-1);
	setConscriptCount(0);

	AI().AI_assignWorkingPlots();

	//if (0 == GET_TEAM(getTeam()).getHasMetCivCount(true) || g.isOption(GAMEOPTION_NO_ESPIONAGE))
	// K-Mod.
	if (isCommerceFlexible(COMMERCE_ESPIONAGE) &&
		(GET_TEAM(getTeam()).getHasMetCivCount(true) == 0 ||
		g.isOption(GAMEOPTION_NO_ESPIONAGE))) //
	{
		setCommercePercent(COMMERCE_ESPIONAGE, 0); // (note: not forced)
	}
	verifyGoldCommercePercent();
	doGold();
	doResearch();
	doEspionagePoints();

	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->doTurn();

	if (getGoldenAgeTurns() > 0)
		changeGoldenAgeTurns(-1);
	if (getAnarchyTurns() > 0)
		changeAnarchyTurns(-1);
//KNOEDELbegin CULTURAL_GOLDEN_AGE 3/8
//	
//
if (g.isOption(GAMEOPTION_CULTURE_GOLDEN_AGE))
	{
		if (getCultureGoldenAgeProgress() >= getCultureGoldenAgeThreshold())
		{
			changeCultureGoldenAgeProgress(-getCultureGoldenAgeThreshold());
			incrementCultureGoldenAgeStarted();
			changeGoldenAgeTurns(getGoldenAgeLength());
		}
	}
//KNOEDELend
	verifyCivics();
	doChangeCivicsPopup(NO_CIVIC); // advc.004x
	//verifyStateReligion(); // dlph.10: disabled for now

	updateTradeRoutes();
	updateWarWearinessPercentAnger();
	// <advc.011>
	if(GC.getDefineINT(CvGlobals::DELAY_UNTIL_BUILD_DECAY) > 0)
		decayBuildProgress();
	// </advc.011>
	doEvents();
	/* advc.136a: Moved here from CvTeam::doTurn. (CvPlayer::doTurn happens at the
	   end of a human turn, CvTeam::doTurn at the beginning.) Doing it in
	   CvPlot::setRevealed would be nice, but possibly too slow - game already tends
	   to hang for a moment after map trades. */
	GET_TEAM(getTeam()).testCircumnavigated();
	// <advc.029>
	FOR_EACH_UNIT_VAR(u, *this)
		u->doTurnPost(); // </advc.029>
	// <advc.004l>
	FOR_EACH_GROUP_VAR(gr, *this)
		gr->doTurnPost(); // </advc.004l>
	/*  <advc.034> Cancel disengagement agreements at the end of a round, i.e.
		at the end of the Barbarian turn. */
	int iDisengageLength = GC.getDefineINT(CvGlobals::DISENGAGE_LENGTH);
	if(isBarbarian() && iDisengageLength > 0)
	{
		FOR_EACH_DEAL_VAR(d)
		{
			if(d->isDisengage() && d->turnsToCancel() <= 1)
			{
				/*  Still one turn to cancel, but that turn is practically over.
					Set to 0 turns to cancel so that the deal-canceled message
					says "0 turns". */
				d->setInitialGameTurn(d->getInitialGameTurn() - 1);
				d->kill();
			}
		}
	}
	else if(iDisengageLength < 0) // See GlobalDefines_advc.xml about this
	{
		CvTeam& kOurTeam = GET_TEAM(getTeam());
		for(int i = 0; i < MAX_CIV_TEAMS; i++)
		{
			TeamTypes tId = (TeamTypes)i;
			if(kOurTeam.isDisengage(tId))
				kOurTeam.cancelDisengage(tId);
		}
	} // </advc.034>
	/*  advc.074: Make sure (if only for performance) that we don't keep expecting
		a cancel-trade popup that never comes (the importer may have died) */
	m_cancelingExport.clear();
	int iGameTurn = g.getGameTurn();
	updateEconomyHistory(iGameTurn, calculateTotalCommerce());
	updateIndustryHistory(iGameTurn, calculateTotalYield(YIELD_PRODUCTION));
	updateAgricultureHistory(iGameTurn, calculateTotalYield(YIELD_FOOD));
	updatePowerHistory(iGameTurn, getPower());
	updateCultureHistory(iGameTurn, countTotalCulture());
	updateEspionageHistory(iGameTurn, GET_TEAM(getTeam()).getEspionagePointsEver());
	expireMessages();  // turn log

	showForeignPromoGlow(false); // advc.002e: To match call in doWarnings
	gDLL->UI().setDirty(CityInfo_DIRTY_BIT, true);

	AI().AI_doTurnPost();
	// <advc.700>
	if(g.isOption(GAMEOPTION_RISE_FALL))
		g.getRiseFall().atTurnEnd(getID()); // </advc.700>

	if (g.isDebugMode()) // BETTER_BTS_AI_MOD, Debug, 07/08/09, jdog5000
		g.updateColoredPlots();

	CvEventReporter::getInstance().endPlayerTurn(iGameTurn, getID());
}


void CvPlayer::doTurnUnits()
{
	PROFILE_FUNC();

	AI().AI_doTurnUnitsPre();
	FOR_EACH_GROUP_VAR(pLoopSelectionGroup, *this)
		pLoopSelectionGroup->doDelayedDeath();

	for (int iPass = 0; iPass < 4; iPass++)
	{
		FOR_EACH_GROUP_VAR(pLoopSelectionGroup, *this)
		{
			switch (pLoopSelectionGroup->getDomainType())
			{
			case DOMAIN_AIR:
				if (iPass == 1)
					pLoopSelectionGroup->doTurn();
				break;
			case DOMAIN_SEA:
				if (iPass == 2)
					pLoopSelectionGroup->doTurn();
				break;
			case DOMAIN_LAND:
				if (iPass == 3)
					pLoopSelectionGroup->doTurn();
				break;
			case DOMAIN_IMMOBILE:
				if (iPass == 0)
					pLoopSelectionGroup->doTurn();
				break;
			case NO_DOMAIN:
				FAssertMsg(pLoopSelectionGroup->getHeadUnit() == NULL, "Unit with no Domain");
			default:
				if (iPass == 3)
					pLoopSelectionGroup->doTurn();
			}
		}
	}

	// K-Mod. (currently unused)
	/*FOR_EACH_GROUP_VAR(pLoopSelectionGroup, *this)
		updateGroupCycle(pLoopSelectionGroup);*/
	// K-Mod end

	if (getID() == GC.getGame().getActivePlayer())
	{
		gDLL->getFAStarIFace()->ForceReset(&GC.getInterfacePathFinder());
		gDLL->UI().setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->UI().setDirty(SelectionButtons_DIRTY_BIT, true);
	}

	gDLL->UI().setDirty(UnitInfo_DIRTY_BIT, true);

	AI().AI_doTurnUnitsPost();
}


void CvPlayer::verifyCivics()  // advc: refactored
{
	if (isAnarchy())
		return;

	FOR_EACH_ENUM(CivicOption)
	{
		if (canDoCivics(getCivics(eLoopCivicOption)))
			continue; // verified

		FOR_EACH_ENUM(Civic)
		{
			if (GC.getInfo(eLoopCivic).getCivicOptionType() == eLoopCivicOption &&
				canDoCivics(eLoopCivic))
			{
					setCivics(eLoopCivicOption, eLoopCivic);
					break;
			}
		}
	}
}

// <dlph.10>
void CvPlayer::verifyStateReligion()
{
	if(!isAnarchy() && !canDoReligion(getStateReligion()))
		setLastStateReligion(NO_RELIGION);
} // </dlph.10>

// <advc.064d>
void CvPlayer::verifyCityProduction()
{
	FOR_EACH_CITY_VAR(c, *this)
		c->verifyProduction();
} // </advc.064d>


void CvPlayer::updatePlotGroups()
{
	PROFILE_FUNC();

	if(!GC.getGame().isFinalInitialized())
		return;

	int iLoop;
	for(CvPlotGroup* pLoopPlotGroup = firstPlotGroup(&iLoop);
		pLoopPlotGroup != NULL; pLoopPlotGroup = nextPlotGroup(&iLoop))
	{
		pLoopPlotGroup->recalculatePlots();
	}
	CvMap const& kMap = GC.getMap();
	for(int iI = 0; iI < kMap.numPlots(); iI++)
		kMap.getPlotByIndex(iI).updatePlotGroup(getID(), false, /* advc.064d: */ false);

	verifyCityProduction(); // advc.064d
	updateTradeRoutes();
}


void CvPlayer::updateYield()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateYield();
}


void CvPlayer::updateMaintenance()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateMaintenance();
}


void CvPlayer::updatePowerHealth()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updatePowerHealth();
}


void CvPlayer::updateExtraBuildingHappiness()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateExtraBuildingHappiness();
}


void CvPlayer::updateExtraBuildingHealth()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateExtraBuildingHealth();
}


void CvPlayer::updateFeatureHappiness()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateSurroundingHealthHappiness();
}


void CvPlayer::updateReligionHappiness()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateReligionHappiness();
}


void CvPlayer::updateExtraSpecialistYield()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateExtraSpecialistYield();
}


void CvPlayer::updateCommerce(CommerceTypes eCommerce)
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateCommerce(eCommerce);
}


void CvPlayer::updateCommerce()
{
	PROFILE_FUNC(); // K-Mod

	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateCommerce();
}

/*************************************************************************************************/
/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
/**																								**/
/**																								**/
/*************************************************************************************************/
void CvPlayer::updateSpecialistCivicExtraCommerce()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateSpecialistCivicExtraCommerce();
	}
}
/*************************************************************************************************/
/**	CMEDIT: End																					**/
/*************************************************************************************************/


void CvPlayer::updateBuildingCommerce()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateBuildingCommerce();
}


void CvPlayer::updateReligionCommerce()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateReligionCommerce();
}


void CvPlayer::updateCorporation()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->updateCorporation();
}

// advc.003j: Vanilla Civ 4 function that was, apparently, never used.
/*void CvPlayer::updateCityPlotYield()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->getPlot().updateYield();
}*/


void CvPlayer::updateCitySight(bool bIncrement, bool bUpdatePlotGroups)
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->getPlot().updateSight(bIncrement, bUpdatePlotGroups);
}


void CvPlayer::updateTradeRoutes()
{
	PROFILE_FUNC();

	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->clearTradeRoutes();

	CLinkList<int> cityList;
	FOR_EACH_CITY_VAR(pLoopCity, *this)
	{
		int iTotalTradeModifier = pLoopCity->totalTradeModifier();

		CLLNode<int>* pCityNode = cityList.head();
		while (pCityNode != NULL)
		{
			CvCity* pListCity = getCity(pCityNode->m_data);
			if (iTotalTradeModifier > pListCity->totalTradeModifier())
			{
				cityList.insertBefore(pLoopCity->getID(), pCityNode);
				break;
			}
			else pCityNode = cityList.next(pCityNode);
		}

		if (pCityNode == NULL)
			cityList.insertAtEnd(pLoopCity->getID());
	}

	CLLNode<int>* pCityNode = cityList.head();
	while (pCityNode != NULL)
	{
		getCity(pCityNode->m_data)->updateTradeRoutes();
		pCityNode = cityList.next(pCityNode);
	}
}

void CvPlayer::updatePlunder(int iChange, bool bUpdatePlotGroups)
{
	FOR_EACH_UNIT_VAR(pLoopUnit, *this)
	{
		if (pLoopUnit->isBlockading())
			pLoopUnit->updatePlunder(iChange, bUpdatePlotGroups);
	}
}

void CvPlayer::updateTimers()
{
	FOR_EACH_GROUP_VAR(pLoopSelectionGroup, *this)
		pLoopSelectionGroup->updateTimers(); // could destroy the selection group...

	// if a unit was busy, perhaps it was not quite deleted yet, give it one more try
	if (getNumSelectionGroups() > getNumUnits())
	{
		FOR_EACH_GROUP_VAR(pLoopSelectionGroup, *this)
			pLoopSelectionGroup->doDelayedDeath(); // could destroy the selection group...
	}

	FAssert(getNumSelectionGroups() <= getNumUnits());
}


bool CvPlayer::hasReadyUnit(bool bAny) const
{
	PROFILE_FUNC();

	FOR_EACH_GROUP(pLoopSelectionGroup, *this)
	{
		if (pLoopSelectionGroup->readyToMove(bAny) &&
				!pLoopSelectionGroup->isAutomated()) // K-Mod
			return true;
	}
	return false;
}


bool CvPlayer::hasAutoUnit() const
{
	PROFILE_FUNC();

	FOR_EACH_GROUP(pLoopSelectionGroup, *this)
	{
		if (pLoopSelectionGroup->readyToAuto())
			return true;
	}
	return false;
}


bool CvPlayer::hasBusyUnit() const
{
	//PROFILE_FUNC();
	FOR_EACH_GROUP(pLoopSelectionGroup, *this)
	{
		if (pLoopSelectionGroup->isBusy())
		{
			/*if (pLoopSelectionGroup->getNumUnits() == 0) {
				pLoopSelectionGroup->kill();
				return false;
			}*/ // BtS - disabled by K-Mod. isBusy returns false if there are no units in the group.
			return true;
		}
	}
	return false;
}

void CvPlayer::chooseTech(int iDiscover, CvWString szText, bool bFront)
{
	// K-mod
	FAssert(isHuman());
	if (iDiscover > 0)
		changeChoosingFreeTechCount(1); // note: if iDiscover is > 1, this function will be called again with iDiscover-=1
	// K-Mod end

	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSETECH);
	if (NULL != pInfo)
	{
		pInfo->setData1(iDiscover);
		pInfo->setText(szText);
		gDLL->UI().addPopup(pInfo, getID(), false, bFront);
	}
}


int CvPlayer::calculateScore(bool bFinal, bool bVictory) const
{
	PROFILE_FUNC();

	if (!isAlive() || GET_TEAM(getTeam()).getNumMembers() <= 0)
		return 0;
	// <advc.707>
	if(GC.getGame().isOption(GAMEOPTION_RISE_FALL) && bFinal)
		return GC.getGame().getRiseFall().getNormalizedFinalScore();
	// </advc.707>
	/*  <advc.003y> Ported from CvGameUtils.py; callback disabled by default.
		Apart from CvGame::getScoreComponent, the auxiliary functions were already
		in the DLL. */
	static int const iSCORE_POPULATION_FACTOR = GC.getDefineINT("SCORE_POPULATION_FACTOR");
	static int const iSCORE_LAND_FACTOR = GC.getDefineINT("SCORE_LAND_FACTOR");
	static int const iSCORE_TECH_FACTOR = GC.getDefineINT("SCORE_TECH_FACTOR");
	static int const iSCORE_WONDER_FACTOR = GC.getDefineINT("SCORE_WONDER_FACTOR");
	CvGame const& g = GC.getGame();
	int iPopulationScore = g.getScoreComponent(getPopScore(), g.getInitPopulation(),
			g.getMaxPopulation(), iSCORE_POPULATION_FACTOR, true, bFinal, bVictory);
	int iLandScore = g.getScoreComponent(getLandScore(), g.getInitLand(),
			g.getMaxLand(), iSCORE_LAND_FACTOR, true, bFinal, bVictory);
	int iTechScore = g.getScoreComponent(getTechScore(), g.getInitTech(),
			g.getMaxTech(), iSCORE_TECH_FACTOR, true, bFinal, bVictory);
	int iWondersScore = g.getScoreComponent(getWondersScore(), g.getInitWonders(),
			g.getMaxWonders(), iSCORE_WONDER_FACTOR, false, bFinal, bVictory);
	int r = iPopulationScore + iLandScore + iWondersScore + iTechScore;

	GC.getPythonCaller()->doPlayerScore(getID(), bFinal, bVictory, r); // </advc.003y>

	return r;
}


int CvPlayer::findBestFoundValue() const
{
	int iBestValue = 0;
	FOR_EACH_AREA(pLoopArea)
	{
		int iValue = pLoopArea->getBestFoundValue(getID());
		if (iValue > iBestValue)
			iBestValue = iValue;
	}
	return iBestValue;
}


int CvPlayer::upgradeAllPrice(UnitTypes eUpgradeUnit, UnitTypes eFromUnit) const
{
	int iPrice = 0;
	FOR_EACH_UNIT(pLoopUnit, *this)
	{
		if (pLoopUnit->getUnitType() == eFromUnit)
		{
			if (pLoopUnit->canUpgrade(eUpgradeUnit, true))
				iPrice += pLoopUnit->upgradePrice(eUpgradeUnit);
		}
	}
	return iPrice;
}

// <advc.080> Based on upgradeAllPrice
int CvPlayer::upgradeAllXPChange(UnitTypes eUpgradeUnit, UnitTypes eFromUnit) const
{
	int r = 0;
	FOR_EACH_UNIT(u, *this)
	{
		if(u->getUnitType() == eFromUnit && u->canUpgrade(eUpgradeUnit, true))
			r += u->upgradeXPChange(eUpgradeUnit);
	}
	return r;
} // </advc.080>

int CvPlayer::countReligionSpreadUnits(CvArea const* pArea,
	ReligionTypes eReligion, bool bIncludeTraining) const
{
	PROFILE_FUNC();

	int iCount = 0;
	FOR_EACH_UNIT(pLoopUnit, *this)
	{
		if (pLoopUnit->isArea(*pArea))
		{
			if (pLoopUnit->getUnitInfo().getReligionSpreads(eReligion) > 0)
				iCount++;
		}
	}

	// bbai
	if (bIncludeTraining)
	{
		FOR_EACH_CITY(pLoopCity, *this)
		{
			UnitTypes eUnit = pLoopCity->getProductionUnit();
			if (eUnit != NO_UNIT)
			{
				if (GC.getInfo(eUnit).getReligionSpreads(eReligion) > 0)
					iCount++;
			}
		}
	}
	// bbai end

	return iCount;
}

int CvPlayer::countCorporationSpreadUnits(CvArea const* pArea,
	CorporationTypes eCorporation, bool bIncludeTraining) const
{
	PROFILE_FUNC();

	int iCount = 0;
	FOR_EACH_UNIT(pLoopUnit, *this)
	{
		//if (pLoopUnit->area() == pArea)
		if (pArea == NULL || pLoopUnit->isArea(*pArea)) // K-Mod
		{
			if (pLoopUnit->getUnitInfo().getCorporationSpreads(eCorporation) > 0)
				iCount++;
		}
	}

	// bbai
	if (bIncludeTraining)
	{
		FOR_EACH_CITY(pLoopCity, *this)
		{
			if (pArea == NULL || pLoopCity->isArea(*pArea)) // K-Mod
			{
				UnitTypes eUnit = pLoopCity->getProductionUnit();
				if (eUnit != NO_UNIT)
				{
					if(GC.getInfo(eUnit).getCorporationSpreads(eCorporation) > 0)
						iCount++;
				}
			}
		}
	} // bbai end

	return iCount;
}

int CvPlayer::countNumCoastalCities() const
{
	int iCount = 0;
	FOR_EACH_CITY(pLoopCity, *this)
	{
		if (pLoopCity->isCoastal())
			iCount++;
	}
	return iCount;
}


int CvPlayer::countNumCoastalCitiesByArea(CvArea const& kArea) const
{
	int iCount = 0;
	FOR_EACH_CITY(pLoopCity, *this)
	{
		if (pLoopCity->isCoastal())
		{
			if (pLoopCity->isArea(kArea) || pLoopCity->getPlot().isAdjacentToArea(kArea))
				iCount++;
		}
	}
	return iCount;
}


int CvPlayer::countTotalCulture() const
{
	int iCount = 0;
	FOR_EACH_CITY(pLoopCity, *this)
		iCount += pLoopCity->getCultureTimes100(getID());
	return iCount/100;
}


int CvPlayer::countCityFeatures(FeatureTypes eFeature) const
{
	PROFILE_FUNC();

	int iCount = 0;
	FOR_EACH_CITY(pLoopCity, *this)
	{
		for (CityPlotIter it(*pLoopCity); it.hasNext(); ++it)
		{
			if (it->getFeatureType() == eFeature)
				iCount++;
		}
	}
	return iCount;
}


int CvPlayer::countNumBuildings(BuildingTypes eBuilding) const
{
	PROFILE_FUNC();

	int iCount = 0;
	FOR_EACH_CITY(pLoopCity, *this)
	{
		if (pLoopCity->getNumBuilding(eBuilding) > 0)
			iCount += pLoopCity->getNumBuilding(eBuilding);
	}
	return iCount;
}


int CvPlayer::countNumCitiesConnectedToCapital() const
{
	int iCount = 0;
	FOR_EACH_CITY(pLoopCity, *this)
	{
		if (pLoopCity->isConnectedToCapital())
			iCount++;
	}
	return iCount;
}

// K-Mod
bool CvPlayer::doesImprovementConnectBonus(ImprovementTypes eImprovement, BonusTypes eBonus) const
{
	return GET_TEAM(getTeam()).doesImprovementConnectBonus(eImprovement, eBonus);
} // K-Mod end

bool CvPlayer::canContact(PlayerTypes ePlayer, /* K-Mod: */ bool bCheckWillingness) const
{
	if (ePlayer == getID())
		return false;

	if (!isAlive() || !(GET_PLAYER(ePlayer).isAlive()))
		return false;

	if (isBarbarian() || GET_PLAYER(ePlayer).isBarbarian())
		return false;

	if (isMinorCiv() || GET_PLAYER(ePlayer).isMinorCiv())
		return false;

	if (getTeam() != TEAMID(ePlayer))
	{
		if (!GET_TEAM(getTeam()).isHasMet(TEAMID(ePlayer)))
			return false;

		if (::atWar(getTeam(), TEAMID(ePlayer)))
		{
			if (!GET_TEAM(getTeam()).canChangeWarPeace(TEAMID(ePlayer)))
				return false;
		}

		if (isHuman() || GET_PLAYER(ePlayer).isHuman())
		{
			if (GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR))
				return false;
		}
	}

	// <K-Mod> (moved here by advc)
	if (bCheckWillingness)
		return (AI().AI_isWillingToTalk(ePlayer) && GET_PLAYER(ePlayer).AI_isWillingToTalk(getID()));
	// </K-Mod>

	return true;
}


void CvPlayer::contact(PlayerTypes ePlayer)
{
	if (!canContact(ePlayer) || isTurnDone())
	{
		return;
	}

	if (GET_PLAYER(ePlayer).isHuman())
	{
		if (GC.getGame().isPbem() || GC.getGame().isHotSeat() || (GC.getGame().isPitboss() && !gDLL->isConnected(GET_PLAYER(ePlayer).getNetID())))
		{
			if (gDLL->isMPDiplomacy())
			{
				gDLL->beginMPDiplomacy(ePlayer, false, false);
			}
		}
		else
		{
			if (gDLL->UI().isFlashing(ePlayer))
			{
				if (!gDLL->getInterfaceIFace()->isDiplomacyLocked())
				{
					gDLL->getInterfaceIFace()->setDiplomacyLocked(true);
					gDLL->sendContactCiv(NETCONTACT_RESPONSE, ePlayer);
				}
			}
			else
			{
				gDLL->sendContactCiv(NETCONTACT_INITIAL, ePlayer);
			}
		}
	}
	else
	{
		CvDiploParameters* pDiplo = new CvDiploParameters(ePlayer);
		FAssert(pDiplo != NULL);
		if (GC.ctrlKey())
			pDiplo->setDiploComment(GC.getAIDiploCommentType("TRADING"));
		gDLL->getInterfaceIFace()->setDiploQueue(pDiplo, GC.getGame().getActivePlayer());
	}
}


void CvPlayer::handleDiploEvent(DiploEventTypes eDiploEvent, PlayerTypes ePlayer, int iData1, int iData2)
{
	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");

	switch (eDiploEvent)
	{
	case DIPLOEVENT_CONTACT:
		// advc.003j: Obsolete
		/*AI_setFirstContact(ePlayer, true);
		GET_PLAYER(ePlayer).AI_setFirstContact(getID(), true);*/
		break;

	case DIPLOEVENT_AI_CONTACT:
		break;

	case DIPLOEVENT_FAILED_CONTACT:
		// advc.003j: Obsolete
		/*AI_setFirstContact(ePlayer, true);
		GET_PLAYER(ePlayer).AI_setFirstContact(getID(), true);*/
		break;

	case DIPLOEVENT_GIVE_HELP:
		// advc.130j:
		AI().AI_rememberEvent(ePlayer, MEMORY_GIVE_HELP);
		/*	advc.104m: Was forcePeace(ePlayer) originally. The peace treaty is
			now part of the help deal. (Signing another one wouldn't hurt
			because of change advc.032; it's just unnecessary.) */
		//GET_TEAM(getTeam()).signPeaceTreaty(TEAMID(ePlayer));
		break;

	case DIPLOEVENT_REFUSED_HELP:
		// advc.130j:
		AI().AI_rememberEvent(ePlayer, MEMORY_REFUSED_HELP);
		break;

	case DIPLOEVENT_ACCEPT_DEMAND:
		// advc.130j:
		AI().AI_rememberEvent(ePlayer, MEMORY_ACCEPT_DEMAND);
		/*  advc.130o, advc.104: So that the AI can tell if a demand was
			_recently_ accepted. Add only 1 memory though. */
		GET_PLAYER(ePlayer).AI_changeMemoryCount(getID(), MEMORY_MADE_DEMAND_RECENT, 1);
		/*  advc (comment): This event (and its counterpart REJECTED_DEMAND)
			is only triggered when a human accepts an AI demand. When a human
			demands something from the AI, DIPLOEVENT_MADE_DEMAND triggers
			(and that one doesn't trigger when the AI makes a demand). */
		// advc.104m: (see note under DIPLOEVENT_GIVE_HELP above)
		//GET_TEAM(getTeam()).signPeaceTreaty(TEAMID(ePlayer));
		break;

	case DIPLOEVENT_REJECTED_DEMAND:
		FAssertMsg(GET_PLAYER(ePlayer).getTeam() != getTeam(), "shouldn't call this function on our own team");
		// advc.130j:
		AI().AI_rememberEvent(ePlayer, MEMORY_REJECTED_DEMAND);
		if (AI().AI_demandRebukedSneak(ePlayer))
			GET_TEAM(getTeam()).AI_setWarPlan(GET_PLAYER(ePlayer).getTeam(), WARPLAN_PREPARING_LIMITED);
		break;

	case DIPLOEVENT_DEMAND_WAR:
		FAssertMsg(GET_PLAYER(ePlayer).getTeam() != getTeam(), "shouldn't call this function on our own team");
		if (gTeamLogLevel >= 2) // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
			logBBAI("    Team %d (%S) declares war on team %d due to DIPLOEVENT", getTeam(), getCivilizationDescription(0), ePlayer);
		GET_TEAM(getTeam()).declareWar(GET_PLAYER(ePlayer).getTeam(), false, WARPLAN_LIMITED);
		break;

	case DIPLOEVENT_CONVERT:
		// advc.130j:
		AI().AI_rememberEvent(ePlayer, MEMORY_ACCEPTED_RELIGION);
		GET_PLAYER(ePlayer).convert(getStateReligion());
		break;

	case DIPLOEVENT_NO_CONVERT:
		// advc.130j:
		AI().AI_rememberEvent(ePlayer, MEMORY_DENIED_RELIGION);
		break;

	case DIPLOEVENT_REVOLUTION:
	{
		AI().AI_changeMemoryCount(ePlayer, MEMORY_ACCEPTED_CIVIC, 1);

		CivicTypes* paeNewCivics = new CivicTypes[GC.getNumCivicOptionInfos()];
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
			paeNewCivics[iI] = GET_PLAYER(ePlayer).getCivics((CivicOptionTypes)iI);

		FAssertMsg(GC.getInfo(getPersonalityType()).getFavoriteCivic() != NO_CIVIC, "getFavoriteCivic() must be valid");

		paeNewCivics[GC.getInfo((CivicTypes)(GC.getInfo(getPersonalityType())).getFavoriteCivic()).getCivicOptionType()] = ((CivicTypes)(GC.getInfo(getPersonalityType()).getFavoriteCivic()));

		GET_PLAYER(ePlayer).revolution(paeNewCivics, true);

		SAFE_DELETE_ARRAY(paeNewCivics);
		break;
	}
	case DIPLOEVENT_NO_REVOLUTION:
		// advc.130j:
		AI().AI_rememberEvent(ePlayer, MEMORY_DENIED_CIVIC);
		break;

	case DIPLOEVENT_JOIN_WAR:
		// advc.130j:
		AI().AI_rememberEvent(ePlayer, MEMORY_ACCEPTED_JOIN_WAR);
		// advc.146:
		GET_TEAM(getTeam()).signPeaceTreaty(TEAMID(ePlayer));
		if (gTeamLogLevel >= 2) // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
			logBBAI("    Team %d (%S) declares war on team %d due to DIPLOEVENT", getTeam(), getCivilizationDescription(0), ePlayer);
		GET_TEAM(ePlayer).declareWar((TeamTypes)iData1, false, WARPLAN_DOGPILE,
				true, getID()); // advc.100
		for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
		{
			CvPlayerAI& kAttacked = GET_PLAYER((PlayerTypes)iI);
			if(!kAttacked.isAlive() || kAttacked.getTeam() != (TeamTypes)iData1)
				continue; // advc
			// advc.130j:
			kAttacked.AI_rememberEvent(getID(), MEMORY_HIRED_WAR_ALLY);
		}
		break;

	case DIPLOEVENT_NO_JOIN_WAR:
		AI().AI_rememberEvent(ePlayer, MEMORY_DENIED_JOIN_WAR); // advc.130j
		break;

	case DIPLOEVENT_STOP_TRADING:
		AI().AI_rememberEvent(ePlayer, MEMORY_ACCEPTED_STOP_TRADING); // advc.130j
		GET_PLAYER(ePlayer).stopTradingWithTeam((TeamTypes)iData1);
		// <advc.130f> We also stop trading (unless ePlayer is our capitulated vassal)
		if(!GET_TEAM(ePlayer).isCapitulated() || !GET_TEAM(ePlayer).isVassal(getTeam()))
			stopTradingWithTeam((TeamTypes)iData1, false); // </advc.130f>
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == ((TeamTypes)iData1))
				{   // advc.130j:
					GET_PLAYER((PlayerTypes)iI).AI_rememberEvent(getID(), MEMORY_HIRED_TRADE_EMBARGO);
				}
			}
		}
		break;

	case DIPLOEVENT_NO_STOP_TRADING:
		AI().AI_rememberEvent(ePlayer, MEMORY_DENIED_STOP_TRADING); // advc.130j
		break;

	case DIPLOEVENT_ASK_HELP:
		// advc.130o: Now the same as DIPLOEVENT_MADE_DEMAND
		//break; // Don't break
	case DIPLOEVENT_MADE_DEMAND:
		/*  <advc.130o> Moved the handling of MEMORY_MADE_DEMAND (non-recent) to
			CvPlayerAI::AI_considerOffer */
		if (getTeam() != TEAMID(ePlayer)) // advc.155: Handled in AI_considerOffer
		{
			// <advc.130j>
			int iDemandRecentMem = AI().AI_getMemoryCount(ePlayer, MEMORY_MADE_DEMAND_RECENT);
			if (iDemandRecentMem <= 0)
				AI().AI_rememberEvent(ePlayer, MEMORY_MADE_DEMAND_RECENT);
			// Only remember it half if already remembered as recent, and cap at 2.
			else if (iDemandRecentMem < 2)
				AI().AI_changeMemoryCount(ePlayer, MEMORY_MADE_DEMAND_RECENT, 1);
		} // </advc.130o> </advc.130j>
		// <advc.144>
		if (iData1 > 0) // Let proxy AI remember when a human request is granted
		{
			GET_PLAYER(ePlayer).AI_rememberEvent(getID(), // advc.130j
					eDiploEvent == DIPLOEVENT_ASK_HELP ?
					MEMORY_GIVE_HELP : MEMORY_ACCEPT_DEMAND);
		} // </advc.144>
		break;

	case DIPLOEVENT_MADE_DEMAND_VASSAL:
		break;

	case DIPLOEVENT_RESEARCH_TECH:
		pushResearch(((TechTypes)iData1), true);
		break;

	case DIPLOEVENT_TARGET_CITY:
	{
		CvCityAI* pCity = GET_PLAYER((PlayerTypes)iData1).AI_getCity(iData2);
		if (pCity != NULL)
		{
			pCity->getArea().AI_setTargetCity(getID(), pCity);
			AI().AI_setCityTargetTimer(GC.getDefineINT(CvGlobals::PEACE_TREATY_LENGTH)); // K-Mod
		}
		break;
	} // K-Mod
	case DIPLOEVENT_SET_WARPLAN:
	{
		CvTeamAI& kOurTeam = GET_TEAM(getTeam());
		FAssert(kOurTeam.getNumWars() <= 0);
		if (iData1 == NO_TEAM)
		{
			FAssert(iData2 == NO_WARPLAN);
			for (TeamTypes i = (TeamTypes)0; i < MAX_CIV_TEAMS; i=(TeamTypes)(i+1))
			{
				//if (!kOurTeam.isAtWar(i)) // advc: Redundant b/c of bWar=false param
				kOurTeam.AI_setWarPlan(i, NO_WARPLAN, false);
			}
		}
		else kOurTeam.AI_setWarPlan((TeamTypes)iData1, (WarPlanTypes)iData2, false);
		break;
	} // K-Mod end

	default:
		FAssert(false);
		break;
	}
}


bool CvPlayer::canTradeWith(PlayerTypes eWhoTo) const
{
	// advc: Team-level checks moved to new CvTeam function
	if(!GET_TEAM(getTeam()).canTradeWith(TEAMID(eWhoTo)) && !canTradeNetworkWith(eWhoTo))
		return false;
	// <advc.ctr>
	CLinkList<TradeData> items;
	// Speed is not an issue; canTradeWith is only called during human-AI diplo.
	buildTradeTable(eWhoTo, items);
	if(items.getLength() > 0)
		return true;
	items.clear();
	GET_PLAYER(eWhoTo).buildTradeTable(getID(), items);
	return (items.getLength() > 0); // </advc.ctr>
}


bool CvPlayer::canReceiveTradeCity() const
{
	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
		return false;
	return true;
}


bool CvPlayer::canTradeItem(PlayerTypes eWhoTo, TradeData item, bool bTestDenial) const  // advc: Style changes; assertions added.
{
	PROFILE_FUNC(); // advc: To keep an eye on items that aren't profiled separately

	/*  <advc.opt> Moved the clauses that don't depend on item.m_iData into a
		subroutine so that client code can check them upfront before e.g.
		calling canTradeItem for every technology. */
	if (!canPossiblyTradeItem(eWhoTo, item.m_eItemType))
		return false; // </advc.opt>

	bool bValid = false; // advc.opt: TradeDenial check moved down
	CvTeam const& kOurTeam = GET_TEAM(getTeam());
	CvTeam const& kToTeam = GET_TEAM(eWhoTo);
	switch (item.m_eItemType)
	{
	case TRADE_TECHNOLOGIES:
	{
		PROFILE("CvPlayer::canTradeItem.TECH");
		FAssertBounds(0, GC.getNumTechInfos(), item.m_iData);
		TechTypes eTech = (TechTypes)item.m_iData;
		if (GC.getInfo(eTech).isTrade() && kOurTeam.isHasTech(eTech) &&
			!kOurTeam.isNoTradeTech(eTech) && !kToTeam.isHasTech(eTech) &&
			GET_PLAYER(eWhoTo).canResearch(eTech, true))
		{
			bValid = true;
		}
		break;
	}
	case TRADE_RESOURCES:
	{
		//PROFILE("CvPlayer::canTradeItem.RESOURCES");
		FAssertBounds(0, GC.getNumBonusInfos(), item.m_iData);
		BonusTypes eBonus = (BonusTypes)item.m_iData;
		if (!kToTeam.isBonusObsolete(eBonus) &&
			!kOurTeam.isBonusObsolete(eBonus))
		{
			//bCanTradeAll=(isHuman() || getTeam() == TEAMID(eWhoTo) || kOurTeam.isVassal(kToTeam.getID()));
			bool const bCanTradeAll = true; // advc.036
			if (getNumTradeableBonuses(eBonus) > (bCanTradeAll ? 0 : 1))
			{	// if (GET_PLAYER(eWhoTo).getNumAvailableBonuses(eBonus) == 0)
				bValid = true;
			}
		}
		break;
	}
	case TRADE_CITIES:
	{
		CvCity const* pCity = getCity(item.m_iData);
		if (pCity == NULL)
		{
			FAssert(pCity != NULL);
			break;
		}
		bValid = canTradeCityTo(eWhoTo, *pCity); // advc.ctr;
		break;
	}
	case TRADE_GOLD:
		FAssert(item.m_iData >= 0); // (advc: 0 is OK as an unspecified amount)
		if (getGold() >= item.m_iData)
			bValid = true;
		break;
	case TRADE_GOLD_PER_TURN:
		FAssert(item.m_iData >= 0);
		bValid = true;
	case TRADE_MAPS:
		bValid = true;
	case TRADE_VASSAL:
		// advc.112: Make sure that only capitulation is possible between war enemies
		if (!kToTeam.isAtWar(getTeam()))
			bValid = true;
	case TRADE_SURRENDER:
	{
		bool bForce = (item.m_iData == 1); // Used by CvDeal::startTeamTrade
		if ((kToTeam.isAtWar(getTeam()) || bForce) && item.m_eItemType == TRADE_SURRENDER)
			bValid = true;
		break;
	}
	case TRADE_PEACE:
	{
		FAssertBounds(0, MAX_CIV_TEAMS, item.m_iData);
		TeamTypes eTargetTeam = (TeamTypes)item.m_iData;
		if (kToTeam.isHasMet(eTargetTeam) && //kOurTeam.isHasMet(eTargetTeam) && // advc: redundant
			kOurTeam.isAtWar(eTargetTeam))
		{
			bValid = true;
		}
		break;
	}
	case TRADE_WAR:
	{
		FAssertBounds(0, MAX_CIV_TEAMS, item.m_iData);
		TeamTypes eTargetTeam = (TeamTypes)item.m_iData;
		if (!GET_TEAM(eTargetTeam).isAVassal() &&
			kOurTeam.isHasMet(eTargetTeam) && kToTeam.isHasMet(eTargetTeam) &&
			kOurTeam.canDeclareWar(eTargetTeam))
		{
			bValid = true;
		}
		break;
	}
	case TRADE_EMBARGO:
	{
		//PROFILE("CvPlayer::canTradeItem.EMBARGO");
		FAssertBounds(0, MAX_CIV_TEAMS, item.m_iData);
		TeamTypes eTargetTeam = (TeamTypes)item.m_iData;
		if (!kOurTeam.isHuman() &&
			kOurTeam.isHasMet(eTargetTeam) && kToTeam.isHasMet(eTargetTeam) &&
			canStopTradingWithTeam(eTargetTeam) &&
			// <advc.130f>
			(!GET_PLAYER(eWhoTo).isTradingWithTeam(eTargetTeam, true) ||
			(kOurTeam.isCapitulated() && kOurTeam.isVassal(kToTeam.getID()))))
			// </advc.130f>
		{
			bValid = true;
		}
		break;
	}
	case TRADE_CIVIC:
	{
		//PROFILE("CvPlayer::canTradeItem.CIVIC");
		FAssertBounds(0, GC.getNumCivicInfos(), item.m_iData);
		CivicTypes eCivic = (CivicTypes)item.m_iData;
		if (GET_PLAYER(eWhoTo).isCivic(eCivic) /* <advc.132> */ ||
			/*  canForceCivic double-checks everything checked here already,
				plus some clauses that I've added there. */
			((kOurTeam.isVassal(kToTeam.getID()) ||
			kOurTeam.isAtWar(kToTeam.getID())) &&
			GET_PLAYER(eWhoTo).canForceCivics(getID(), eCivic))) // </advc.132>
		{
			if (canDoCivics(eCivic) && !isCivic(eCivic) && canRevolution(NULL))
				bValid = true;
		}
		break;
	}
	case TRADE_RELIGION:
	{
		//PROFILE("CvPlayer::canTradeItem.RELIGION");
		FAssertBounds(0, GC.getNumReligionInfos(), item.m_iData);
		ReligionTypes eReligion = (ReligionTypes)item.m_iData;
		if (GET_PLAYER(eWhoTo).getStateReligion() == eReligion /* <advc.132> */ ||
			// Same thing as for civics above
			((kOurTeam.isVassal(kToTeam.getID()) ||
			kOurTeam.isAtWar(kToTeam.getID())) &&
			GET_PLAYER(eWhoTo).canForceReligion(getID(), eReligion))) // </advc.132>
		{
			if (canConvert(eReligion))
				bValid = true;
		}
		break;
	}
	// advc.opt: The rest is handled by canPossiblyTradeItem
	case TRADE_OPEN_BORDERS:
	case TRADE_DEFENSIVE_PACT:
	case TRADE_PERMANENT_ALLIANCE:
	case TRADE_PEACE_TREATY:
	// <advc.034>
	case TRADE_DISENGAGE:
		bValid = true; // </advc.034>
	}
	// advc.opt: (denial check moved down)
	return (bValid && (!bTestDenial || getTradeDenial(eWhoTo, item) == NO_DENIAL));
}

/*  advc.opt: Cut from canTradeItem.
	'False' guarantees that no trade is possible, but 'true' only means that some
	trade might be possible. */
bool CvPlayer::canPossiblyTradeItem(PlayerTypes eWhoTo, TradeableItems eItemType) const // advc.opt
{
	PROFILE_FUNC();
	CvTeam const& kOurTeam = GET_TEAM(getTeam());
	CvTeam const& kToTeam = GET_TEAM(eWhoTo);
	switch (eItemType)
	{
	case TRADE_TECHNOLOGIES:
		return (!GC.getGame().isOption(GAMEOPTION_NO_TECH_TRADING) &&
				(kOurTeam.isTechTrading() || kToTeam.isTechTrading()));
	case TRADE_RESOURCES: return canTradeNetworkWith(eWhoTo);
	case TRADE_CITIES:
		return (GET_PLAYER(eWhoTo).canReceiveTradeCity() &&
				GC.getGame().getMaxCityElimination() <= 0);
	case TRADE_GOLD:
	case TRADE_GOLD_PER_TURN:
		return (kOurTeam.isGoldTrading() || kToTeam.isGoldTrading());
	case TRADE_MAPS:
		return ((getTeam() != kToTeam.getID()) &&
				(kOurTeam.isMapTrading() || kToTeam.isMapTrading()));
	case TRADE_VASSAL:
	case TRADE_SURRENDER:
	{
		static bool const bBBAI_HUMAN_AS_VASSAL_OPTION = GC.getDefineBOOL("BBAI_HUMAN_AS_VASSAL_OPTION"); // advc.opt
		if (isHuman() && !GET_PLAYER(eWhoTo).isHuman() &&
			!bBBAI_HUMAN_AS_VASSAL_OPTION) // BETTER_BTS_AI_MOD, Customization, 12/06/09, jdog5000
		{
			return false;
		}
		if (!kToTeam.isVassalStateTrading()) // the master must possess the tech
			return false;
		if (kOurTeam.isAVassal() || kToTeam.isAVassal() || getTeam() == kToTeam.getID())
			return false;
		if (!kToTeam.isAtWar(getTeam()) && eItemType == TRADE_VASSAL)
			return true;
		// TRADE_SURRENDER has an override for peacetime; can't check that here.
		return true;
	}
	case TRADE_WAR:
		if (kOurTeam.isAtWar(kToTeam.getID())) // advc.100
			return false;
		// fall through
	case TRADE_PEACE:
		return (!kOurTeam.isHuman() && !kOurTeam.isAVassal() &&
				!kToTeam.isCapitulated()); // advc.130v: Make sure that capitulated vassals can't broker war/ peace
	case TRADE_EMBARGO:
		return !kToTeam.isCapitulated(); // advc.130v: Make sure that capitulated vassals can't negotiate embargoes
	case TRADE_RELIGION:
	case TRADE_CIVIC:
		//if (!ourTeam.isHuman()
				// UNOFFICIAL_PATCH, Diplomacy, 10/22/09, denev & jdog5000
				//|| getTeam() == theirTeam.getID())
		// <advc.155> Replacing the patched code above
		return ((getTeam() == kToTeam.getID()) ?
			(!isHuman() || GET_PLAYER(eWhoTo).isHuman()) :
			/*  Don't allow humans from one team to manipulate the civics/ religion
				of AI civs on another human team. */
			!kOurTeam.isHuman()); // </advc.155>
	case TRADE_OPEN_BORDERS:
		return (getTeam() != kToTeam.getID() && !kOurTeam.isAtWar(kToTeam.getID()) &&
				!kOurTeam.isOpenBorders(kToTeam.getID()) &&
				(kOurTeam.isOpenBordersTrading() || kToTeam.isOpenBordersTrading()));
	case TRADE_DEFENSIVE_PACT:
		if (!kOurTeam.isAVassal() && !kToTeam.isAVassal() &&
			getTeam() != kToTeam.getID() && //!kToTeam.isVassal(getTeam()) // advc: redundant
			!kOurTeam.isAtWar(kToTeam.getID()) &&
			!kOurTeam.isDefensivePact(kToTeam.getID()) &&
			(kOurTeam.isDefensivePactTrading() || kToTeam.isDefensivePactTrading()))
		{
			/*  <dlph.3> 'Added possibility of signing defensive pact while in war
				if BBAI defensive pact option is >= 1' */
			if ((kOurTeam.getNumWars() <= 0 && kToTeam.getNumWars() <= 0) ||
				(GC.getDefineINT(CvGlobals::BBAI_DEFENSIVE_PACT_BEHAVIOR) >= 1
				/*  advc: Prohibit DP when not all wars shared?
					Enough to have the AI refuse such pacts I think
					(in CvTeamAI::AI_defensivePactTrade). */
					//&& GET_TEAM(getID()).allWarsShared(theirTeam.getID())
				)) // </dlph.3>
			{
				if (kOurTeam.canSignDefensivePact(kToTeam.getID()))
					return true;
			}
		}
		return false;
	case TRADE_PERMANENT_ALLIANCE:
		return (!kOurTeam.isAVassal() && !kToTeam.isAVassal() &&
				getTeam() != kToTeam.getID() && //!kToTeam.isVassal(getTeam()) // advc: redundant
				!kOurTeam.isAtWar(kToTeam.getID()) &&
				#ifndef TEST_PERMANENT_ALLIANCES // advc.test
				(kOurTeam.isPermanentAllianceTrading() || kToTeam.isPermanentAllianceTrading()) &&
				#endif
				kOurTeam.getNumMembers() == 1 && kToTeam.getNumMembers() == 1);
	case TRADE_PEACE_TREATY:
		return kOurTeam.canChangeWarPeace(kToTeam.getID()); // advc.130v
	// <advc.034>
	case TRADE_DISENGAGE:
		return (!kToTeam.isDisengage(getTeam()) &&
				!kToTeam.isAtWar(getTeam()) &&
				!kToTeam.isOpenBorders(getTeam()) &&
				!kToTeam.isVassal(getTeam()) &&
				!kOurTeam.isVassal(kToTeam.getID()) &&
				kToTeam.isHasMet(getTeam()));
	// </advc.034>
	default:
		FAssertMsg(false, "Unknown trade item type");
		return false;
	}
}

// advc.ctr:
bool CvPlayer::canTradeCityTo(PlayerTypes eRecipient, CvCity const& kCity, bool bConquest) const
{
	PROFILE_FUNC(); // advc: Fine so far; not frequently called.
	CvPlayer const& kRecipient = GET_PLAYER(eRecipient);
	/*	canTradeItem already checks this through canPossiblyTradeItem, so this
		is redundant. However, I also need to check the city trade conditions
		from CvPlayer::acquireCity, so this can't be helped unless I add the
		bConquest param to canTradeItem, which I find inelegant. */
	if (!kRecipient.canReceiveTradeCity())
		return false;

	if (kCity.isCapital() || !kCity.isRevealed(kRecipient.getTeam()))
		return false;
	if (kCity.getLiberationPlayer(bConquest) == eRecipient) // as in BtS
		return true;
	/*	Can't trade so long as the previous owner hasn't accepted the loss
		(let's ignore kCity.getOriginalOwner()) */
	PlayerTypes ePreviousOwner = kCity.getPreviousOwner();
	if (ePreviousOwner != NO_PLAYER)
	{
		TeamTypes ePreviousTeam = TEAMID(ePreviousOwner);
		if (ePreviousTeam != kRecipient.getTeam() &&
			GET_TEAM(getTeam()).isAtWar(ePreviousTeam) &&
			!GET_TEAM(eRecipient).isAtWar(ePreviousTeam))
		{
			return false;
		}
	}
	CvPlot const& kCityPlot = *kCity.plot();
	if (kCityPlot.calculateCulturePercent(eRecipient) <
		GC.getDefineINT(CvGlobals::CITY_TRADE_CULTURE_THRESH))
	{
		return false;
	}
	if (!GET_TEAM(eRecipient).isAtWar(getTeam()) &&
		// Prevent back-and-forth trades between humans
		(kCity.isEverOwned(eRecipient) && isHuman() &&
		kRecipient.isHuman() ? 1 : 2) * kCityPlot.getCulture(eRecipient) <=
		kCityPlot.getCulture(getID()))
	{
		return false;
	}
	if (GET_TEAM(eRecipient).isVassal(getTeam()) &&
		kCityPlot.getCulture(eRecipient) <= kCityPlot.getCulture(getID()))
	{
		return false;
	}
	return true;
}


DenialTypes CvPlayer::getTradeDenial(PlayerTypes eWhoTo, TradeData item) const
{
	PROFILE_FUNC(); // (advc: BONUS_TRADE is the only -minor- concern currently)
	const CvTeamAI& kOurTeam = GET_TEAM(getTeam()); // K-Mod

	// K-Mod note: I've changed it so that AI players on human teams can be contacted when not at war.
	// So.. as a follow up on that change, I'm making the AI deny trades which affect the team, not just the player.
	switch (item.m_eItemType)
	{
	case TRADE_TECHNOLOGIES:
		// K-Mod
		if (!isHuman() && kOurTeam.isHuman())
			return DENIAL_MYSTERY;
		// K-Mod end
		return kOurTeam.AI_techTrade((TechTypes)item.m_iData, TEAMID(eWhoTo));

	case TRADE_RESOURCES:
		return AI().AI_bonusTrade((BonusTypes)item.m_iData, eWhoTo,
				1); // advc.036

	case TRADE_CITIES:
	{
		CvCityAI* pCity = AI().AI_getCity(item.m_iData);
		if (pCity != NULL)
			return AI().AI_cityTrade(*pCity, eWhoTo);
		break;
	}
	case TRADE_GOLD:
	case TRADE_GOLD_PER_TURN:
		break;

	case TRADE_MAPS:
		return kOurTeam.AI_mapTrade(TEAMID(eWhoTo));

	case TRADE_SURRENDER:
		return kOurTeam.AI_surrenderTrade(TEAMID(eWhoTo),
				// advc.104o: No functional change
				CvTeamAI::VASSAL_POWER_MOD_SURRENDER);

	case TRADE_VASSAL:
		// K-Mod
		if (!isHuman() && kOurTeam.isHuman())
			return DENIAL_MYSTERY; // K-Mod end
		return kOurTeam.AI_vassalTrade(TEAMID(eWhoTo));

	case TRADE_PEACE:
		return kOurTeam.AI_makePeaceTrade((TeamTypes)item.m_iData, TEAMID(eWhoTo));

	case TRADE_WAR:
		return kOurTeam.AI_declareWarTrade((TeamTypes)item.m_iData, TEAMID(eWhoTo));

	case TRADE_EMBARGO:
		return AI().AI_stopTradingTrade((TeamTypes)item.m_iData, eWhoTo);

	case TRADE_CIVIC:
		return AI().AI_civicTrade((CivicTypes)item.m_iData, eWhoTo);

	case TRADE_RELIGION:
		return AI().AI_religionTrade((ReligionTypes)item.m_iData, eWhoTo);

	case TRADE_OPEN_BORDERS:
		return kOurTeam.AI_openBordersTrade(TEAMID(eWhoTo));

	case TRADE_DEFENSIVE_PACT:
		// K-Mod
		if (!isHuman() && kOurTeam.isHuman())
			return DENIAL_MYSTERY;
		// K-Mod end
		return kOurTeam.AI_defensivePactTrade(TEAMID(eWhoTo));

	case TRADE_PERMANENT_ALLIANCE:
		// K-Mod
		if (!isHuman() && kOurTeam.isHuman())
			return DENIAL_MYSTERY;
		// K-Mod end
		return kOurTeam.AI_permanentAllianceTrade(TEAMID(eWhoTo));

	case TRADE_PEACE_TREATY:
		// K-Mod
		if (kOurTeam.AI_refusePeace(TEAMID(eWhoTo)))
			return DENIAL_VICTORY;
		// K-Mod end
		break;
	/*  <advc.034> Can't be traded voluntarily anyway, but NO_DENIAL suppresses
		explanation text in CvDLLWidgetData::parseTradeItem */
	case TRADE_DISENGAGE:
		return NO_DENIAL; // <advc.034>
	}

	return NO_DENIAL;
}


bool CvPlayer::canTradeNetworkWith(PlayerTypes ePlayer) const
{
	// <advc.124>
	PROFILE_FUNC();
	CvCity* pOurCap = getCapitalCity();
	CvPlayer const& kThey = GET_PLAYER(ePlayer);
	CvCity* pTheirCap = kThey.getCapitalCity();
	if(pOurCap != NULL && pTheirCap != NULL)
	{
		FOR_EACH_CITY(c, kThey)
		{
			if(pOurCap->isConnectedTo(c) && c->isConnectedToCapital(ePlayer))
				return true;
		}
		FOR_EACH_CITY(c, *this)
		{
			if(pTheirCap->isConnectedTo(c) && c->isConnectedToCapital(getID()))
				return true;
		}
		// Replaced BtS code:
		/*if (pOurCapitalCity->isConnectedToCapital(ePlayer))
			return true;*/
	} // </advc.124>

	return false;
}


int CvPlayer::getNumAvailableBonuses(BonusTypes eBonus) const
{
	CvPlotGroup* pPlotGroup = (getCapitalCity() != NULL ?
			getCapitalCity()->getPlot().getOwnerPlotGroup() : NULL);
	if (pPlotGroup != NULL)
		return pPlotGroup->getNumBonuses(eBonus);

	return 0;
}


int CvPlayer::getNumTradeableBonuses(BonusTypes eBonus) const
{
	return (getNumAvailableBonuses(eBonus) - getBonusImport(eBonus));
}

bool CvPlayer::hasBonus(BonusTypes eBonus) const
{
	FOR_EACH_CITY(pLoopCity, *this)
	{
		if (pLoopCity->hasBonus(eBonus))
			return true;
	}
	return false;
}

int CvPlayer::getNumTradeBonusImports(PlayerTypes eFromPlayer) const // advc: Simplified using the new CvDeal interface
{
	FAssert(eFromPlayer != getID());
	int iCount = 0;
	FOR_EACH_DEAL(pLoopDeal)
	{
		if (!pLoopDeal->isBetween(getID(), eFromPlayer))
			continue;

		for (CLLNode<TradeData> const* pNode = pLoopDeal->headGivesNode(eFromPlayer);
			pNode != NULL; pNode = pLoopDeal->nextGivesNode(pNode, eFromPlayer))
		{
			if (pNode->m_data.m_eItemType == TRADE_RESOURCES)
				iCount++;
		}
	}
	return iCount;
}

/*  advc: Renamed parameters. Note about the semantics (same as in BtS):
	- This functions checks only whether we're giving something to eToTeam, not vice versa.
	- It checks if this player is trading with any member of eToTeam. Likewise,
	  an embargo will affect only the enacting player - but the entire target team.
	Body simplified using the new CvDeal interface. */
bool CvPlayer::isTradingWithTeam(TeamTypes eToTeam, bool bIncludeUncancelable) const
{
	if (eToTeam == getTeam())
		return false;

	FOR_EACH_DEAL(pLoopDeal)
	{
		if (!pLoopDeal->isBetween(getID(), eToTeam))
			continue;
		// advc: Un(!)cancelable
		if (!bIncludeUncancelable && !pLoopDeal->isCancelable(getID()))
			continue;

		if (pLoopDeal->getGivesList(getID()).getLength() > 0 && !pLoopDeal->isPeaceDeal()
				&& !pLoopDeal->isDisengage()) // advc.034
			return true;
	}
	return false;
}


bool CvPlayer::canStopTradingWithTeam(TeamTypes eTeam, bool bContinueNotTrading) const
{
	// <advc.130d> Replacing the two conditions below.
	if(getMasterTeam() == GET_TEAM(eTeam).getMasterTeam())
		return false; // </advc.130d>
	/*if (eTeam == getTeam())
		return false;
	if (GET_TEAM(getTeam()).isVassal(eTeam))
		return false;*/
	if(bContinueNotTrading)
	{
		// <advc.130f>
		// Allow resolutions to overrule turns-to-cancel
		return true;
	}
	return isTradingWithTeam(eTeam, true);
	// BtS code: // </advc.130f>
	/*if (!isTradingWithTeam(eTeam, false)) {
		if (bContinueNotTrading && !isTradingWithTeam(eTeam, true))
			return true;
		return false;
	}
	return true;*/
}


void CvPlayer::stopTradingWithTeam(TeamTypes eTeam, /* advc.130f: */ bool bDiploPenalty)
{
	FAssert(eTeam != getTeam());
	bool bDealCanceled = false; // advc.130f
	FOR_EACH_DEAL_VAR(pLoopDeal)
	{
		if (!pLoopDeal->isBetween(getID(), eTeam))
			continue;

		if (pLoopDeal->isEverCancelable(getID()) // advc.130f
			&& !pLoopDeal->isPeaceDeal()
			&& !pLoopDeal->isDisengage()) // advc.034
		{
			pLoopDeal->kill();
			bDealCanceled = true; // advc.130f
		}
	}
	// <advc.130f>
	if(!bDealCanceled && bDiploPenalty)
		return;
	// </advc.130f>
	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++) // advc.003n: was MAX_PLAYERS
	{   CvPlayerAI& kTargetMember = GET_PLAYER((PlayerTypes)iI); // advc
		if (!kTargetMember.isAlive() || kTargetMember.getTeam() != eTeam)
			continue;
		// <advc.130j>
		if(bDiploPenalty) // advc.130f: RECENT causes only refusal to talk
			kTargetMember.AI_rememberEvent(getID(), MEMORY_STOPPED_TRADING);
		// Don't rememberEvent - not supposed to be based on attitude
		kTargetMember.AI_setMemoryCount(getID(), MEMORY_STOPPED_TRADING_RECENT, 2);
	} // </advc.130j>
}

// <advc.130f>
bool CvPlayer::isAnyDealTooRecentToCancel(TeamTypes eTeam) const
{
	FOR_EACH_DEAL(d)
	{
		if(d->isBetween(getID(), eTeam) && !d->isCancelable(getID()) &&
				!d->isPeaceDeal() && !d->isDisengage()) // advc.034
			return true;
	}
	return false;
} // </advc.130f>

void CvPlayer::killAllDeals()
{
	FOR_EACH_DEAL_VAR(pLoopDeal)
	{
		if (pLoopDeal->involves(getID()))
			pLoopDeal->kill();
	}
}


void CvPlayer::findNewCapital()  // advc style changes
{
	BuildingTypes eCapitalBuilding = getCivilization().getBuilding((BuildingClassTypes)
			GC.getDefineINT("CAPITAL_BUILDINGCLASS"));
	if (eCapitalBuilding == NO_BUILDING)
		return;

	CvCity* pOldCapital = getCapitalCity();

	int iBestValue = 0;
	CvCity* pBestCity = NULL;
	FOR_EACH_CITY_VAR(pLoopCity, *this)
	{
		if (pLoopCity == pOldCapital ||
				pLoopCity->getNumRealBuilding(eCapitalBuilding) != 0)
			continue;

		int iValue = pLoopCity->getPopulation() * 4;
		iValue += pLoopCity->getYieldRate(YIELD_FOOD);
		iValue += (pLoopCity->getYieldRate(YIELD_PRODUCTION) * 3);
		iValue += (pLoopCity->getYieldRate(YIELD_COMMERCE) * 2);
		iValue += pLoopCity->getCultureLevel();
		iValue += pLoopCity->getReligionCount();
		iValue += pLoopCity->getCorporationCount();
		iValue += (pLoopCity->getNumGreatPeople() * 2);

		iValue *= (pLoopCity->calculateCulturePercent(getID()) + 100);
		iValue /= 100;

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			pBestCity = pLoopCity;
		}
	}

	if (pBestCity != NULL)
	{
		if (pOldCapital != NULL)
			pOldCapital->setNumRealBuilding(eCapitalBuilding, 0);
		FAssertMsg(!(pBestCity->getNumRealBuilding(eCapitalBuilding)), "(pBestCity->getNumRealBuilding(eCapitalBuilding)) did not return false as expected");
		pBestCity->setNumRealBuilding(eCapitalBuilding, 1);
	}
}


int CvPlayer::getNumGovernmentCenters() const
{
	int iCount = 0;
	FOR_EACH_CITY(pLoopCity, *this)
	{
		if (pLoopCity->isGovernmentCenter())
			iCount++;
	}
	return iCount;
}


bool CvPlayer::canRaze(CvCity const& kCity) const // advc: param was CvCity*
{
	if (!kCity.isAutoRaze())
	{
		if (GC.getGame().isOption(GAMEOPTION_NO_CITY_RAZING))
			return false;

		if (kCity.getOwner() != getID())
			return false;
		//Keldath QA2
		//f1rpo explain this replaces the lines below
		//also added the option check.
		/************************************************************************************************/
		/* REVOLUTIONDCM_MOD                         02/17/10                           jdog5000        */
		/*                                                                                              */
		/*influence driven war                                                                                              */
		/************************************************************************************************/
		// Change for IDW, so AI may raze cities it captures
			//keldath qa4 - error on  error C2664 : 'CvPlot::isCultureRangeCity' : cannot convert parameter 2 from 'const int' to 'CultureLevelTypes'
		if (!GC.getGame().isOption(GAMEOPTION_INFLUENCE_DRIVEN_WAR) || kCity.isEverOwned(getID()) || kCity.plot()->isCultureRangeCity(getID(), (CultureLevelTypes)std::max(0,GC.getNumCultureLevelInfos() - 1)))
	//can also be this syntax using a small func added by f1rpo to cvinfo_city.h - keldath entry
	//	if (!GC.getGame().isOption(GAMEOPTION_INFLUENCE_DRIVEN_WAR) || kCity.isEverOwned(getID()) || kCity.getPlot().isCultureRangeCity(getID(), CvCultureLevelInfo::finalCultureLevel()))
		{
			if (kCity.calculateTeamCulturePercent(getTeam()) >= GC.getDefineINT("RAZING_CULTURAL_PERCENT_THRESHOLD"))
			{
				return false;
			}
		}
		/************************************************************************************************/
		/* REVOLUTIONDCM_MOD                         END                                 Glider1        */
		/************************************************************************************************/
		//static int const iRAZING_CULTURAL_PERCENT_THRESHOLD = GC.getDefineINT("RAZING_CULTURAL_PERCENT_THRESHOLD"); // advc.opt
		//if (kCity.calculateTeamCulturePercent(getTeam()) >= iRAZING_CULTURAL_PERCENT_THRESHOLD)
		//	return false;

	}
	/*  advc.003y (note): This Python function is also called from acquireCity.
		acquireCity should arguably call CvPlayer::canRaze instead. But that seems
		difficult to untangle. */
	if (!GC.getPythonCaller()->canRaze(kCity, getID()))
		return false;

	return true;
}


void CvPlayer::raze(CvCity& kCity) // advc: param was CvCity*
{
	if (!canRaze(kCity))
		return;

	FAssert(kCity.getOwner() == getID());

	AI().AI_processRazeMemory(kCity); // advc.003n: Moved into subroutine

	wchar szBuffer[1024];
	swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_DESTROYED_CITY",
			kCity.getNameKey()).GetCString());
	gDLL->UI().addMessage(getID(), true, -1, szBuffer, kCity.getPlot(),
			"AS2D_CITYRAZE", MESSAGE_TYPE_MAJOR_EVENT,
			ARTFILEMGR.getInterfaceArtPath("WORLDBUILDER_CITY_EDIT"),
			GC.getColorType("GREEN"));

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer const& kObs = GET_PLAYER((PlayerTypes)iI); // advc
		if (!kObs.isAlive() || iI == getID())
			continue;
		if (kCity.isRevealed(kObs.getTeam()) /* advc.127: */ || kObs.isSpectator())
		{
			swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_CITY_HAS_BEEN_RAZED_BY",
					kCity.getNameKey(), getCivilizationDescriptionKey()).GetCString());
			gDLL->UI().addMessage((PlayerTypes)iI, false, -1, szBuffer, kCity.getPlot(),
					"AS2D_CITYRAZED",
					MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY, // advc.106b
					ARTFILEMGR.getInterfaceArtPath("WORLDBUILDER_CITY_EDIT"),
					GC.getColorType("RED"));
		}
	}

	swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_CITY_RAZED_BY",
			kCity.getNameKey(), getCivilizationDescriptionKey()).GetCString());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer,
			kCity.getX(), kCity.getY(), GC.getColorType("WARNING_TEXT"));

	kCity.doPartisans(); // advc.003y
	CvEventReporter::getInstance().cityRazed(&kCity, getID());
	CvPlot const& kCityPlot = *kCity.plot(); // advc.130w
	disband(kCity);
	AI().AI_updateCityAttitude(kCityPlot); // advc.130w
}


void CvPlayer::disband(CvCity& kCity) // advc: param was CvCity*
{
	if (getNumCities() == 1)
		setFoundedFirstCity(false);
	GC.getGame().addDestroyedCityName(kCity.getName());
	kCity.kill(true);
}


bool CvPlayer::canReceiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit) const
{
	CvGoodyInfo const& goody = GC.getInfo(eGoody); // advc
	// <advc.314>
	CvGame const& g = GC.getGame();
	int const iTrainHalved = (GC.getInfo(g.getGameSpeedType()).
			getTrainPercent() + 100) / 2;
	bool bVeryEarlyGame = (100 * g.getGameTurn() < 20 * iTrainHalved);
	bool bVeryVeryEarlyGame = (100 * g.getGameTurn() < 10 * iTrainHalved);
	if (goody.getExperience() > 0)
	{
		if (pUnit == NULL || !pUnit->canAcquirePromotionAny() ||
				//(GC.getGame().getElapsedGameTurns() < 10)
				bVeryVeryEarlyGame) // advc.314
			return false;
	}

	if (goody.getDamagePrereq() > 0)
	{
		if (pUnit == NULL || pUnit->getDamage() < (pUnit->maxHitPoints() *
				goody.getDamagePrereq()) / 100)
			return false;
	} // advc.314:
	bool bAnyGold = (goody.getGold() + goody.getGoldRand1() + goody.getGoldRand2() > 0);
	if (goody.isTech())
	{
		bool bTechFound = false;
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{	// advc.114: Replacing the conditions below
			if(isGoodyTech((TechTypes)iI, bAnyGold))
			{
				/*if (GC.getInfo((TechTypes) iI).isGoodyTech()) {
					//if (canResearch((TechTypes)iI), false, true)*/ // K-Mod
				if (canResearch((TechTypes)iI, false, true)) // advc
				{
					bTechFound = true;
					break;
				}
			}
		}

		if (!bTechFound)
			return false;
	}

	if (goody.isBad())
	{
		if (pUnit == NULL || pUnit->isNoBadGoodies())
			return false;
	}
	// <advc.314>
	if(bVeryEarlyGame && goody.getMinBarbarians() > 1)
		return false;
	/*  Moved up and added the era clause; a single free unit in the Medieval
		era isn't going to be a problem. */
	bool bEarlyMP = (GC.getGame().isGameMultiPlayer() &&
			GC.getGame().getCurrentEra() < 2);
	// No free unit from Spy when hut guarded
	if(!goody.isBad() && (goody.getUnitClassType() != NO_UNITCLASS ||
		goody.getMinBarbarians() > 0) && pPlot->isVisibleEnemyUnit(getID()) &&
			pUnit != NULL && pUnit->isSpy())
		return false;
	UnitClassTypes eGoodyUnitClass = (UnitClassTypes)goody.getUnitClassType();
	if (eGoodyUnitClass!= NO_UNITCLASS)
	{
		UnitTypes eUnit = getCivilization().getUnit(eGoodyUnitClass);
		if (eUnit == NO_UNIT)
			return false;
		CvUnitInfo const& kUnit = GC.getInfo(eUnit); // advc
		if (kUnit.getCombat() > 0 && /* advc.315: */ !kUnit.isMostlyDefensive())
		{
			if (bEarlyMP || // advc.314
					//GC.getGame().getElapsedGameTurns() < 20
					// advc.314: Replacing the above
					bVeryEarlyGame)
				return false;
		} // <advc.314> I guess a Worker with a slow WorkRate would be OK
		if(bVeryEarlyGame && kUnit.getWorkRate() > 30)
			return false; // </advc.314>
		if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
		{
			if (kUnit.isFound())
				return false;
		}
	} // <advc.314> Free unit and no UnitClassType given
	else if(!goody.isBad() && goody.getMinBarbarians() > 0 &&
			(bEarlyMP || bVeryEarlyGame))
		return false; // </advc.314>
	if ((goody.getBarbarianUnitClass() != NO_UNITCLASS
		// <advc.314> Hostile unit w/o any UnitClassType given
		|| (goody.getMinBarbarians() > 0 && goody.getBarbarianUnitProb() > 0))
		// BarbarianUnitClass has a different use now when !isBad
		&& goody.isBad()) // </advc.314>
	{
		if (GC.getGame().isOption(GAMEOPTION_NO_BARBARIANS))
			return false;

		if (getNumCities() == 0)
			return false;

		if (getNumCities() == 1)
		{
			CvCity* pCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), NO_PLAYER, getTeam());
			if (pCity != NULL)
			{
				if (plotDistance(pPlot->getX(), pPlot->getY(), pCity->getX(), pCity->getY()) <=
						// advc.314 (comment):
						8 - getNumCities()) // = 7 b/c of the NumCities==1 check
					return false;
			}
		}
	} // <advc.315d> No Scout from a Scout if already 2 Scouts
	if(pUnit != NULL && pUnit->isNoBadGoodies() && AI().AI_getNumAIUnits(UNITAI_EXPLORE) >= 2)
	{
		UnitClassTypes eUnitClass = (UnitClassTypes)goody.getUnitClassType();
		if(eUnitClass != NO_UNITCLASS)
		{
			UnitTypes eUnit = getCivilization().getUnit(eUnitClass);
			if(eUnit != NO_UNIT && GC.getInfo(eUnit).isNoBadGoodies())
				return false;
		}
	} // </advc.315d>
	// <advc.315e> No map reveal at the edges of a non-wrapping map
	if(goody.getMapProb() > 0)
	{
		int const iRange = 3;
		for(int dx = -iRange; dx <= iRange; dx++)
		{
			for(int dy = -iRange; dy <= iRange; dy++)
			{
				CvPlot* pLoopPlot = ::plotXY(pPlot->getX(), pPlot->getY(), dx, dy);
				if(pLoopPlot == NULL)
					return false;
			}
		}
	} // </advc.315e>
	return true;
}


void CvPlayer::receiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit,
		bool bNoRecursion) // advc.314
{
	CvWString szBuffer;
	CvWString szTempBuffer;

	FAssert(canReceiveGoody(pPlot, eGoody, pUnit));
	// <advc>
	CvGoodyInfo const& goody = GC.getInfo(eGoody);
	CvGame& g = GC.getGame();
	// </advc>
	szBuffer = goody.getDescription();

	// <advc.314>
	double prUpgrade = (goody.isBad() ? 0 : goody.getBarbarianUnitProb()  / 100.0);
	/*  Coefficient chosen such that it's 100% on turn 150. Would be better to
		program that computation and set the 100%-turn in XML ... */
	prUpgrade *= (0.225 * (g.goodyHutEffectFactor(false) - 1));
	/*  Meaning that an upgraded version of 'goody' should be used, or, if there
		is none, that an additional outcome should be rolled. */
	bool bUpgrade = ::bernoulliSuccess(prUpgrade, "advc.314");
	// </advc.314>
	int iGold = goody.getGold() + g.getSorenRandNum(goody.getGoldRand1(), "Goody Gold 1") +
			g.getSorenRandNum(goody.getGoldRand2(), "Goody Gold 2");
	//iGold  = (iGold * GC.getInfo(g.getGameSpeedType()).getGrowthPercent()) / 100;
	// advc.314: Replacing the above
	iGold = (int)(iGold * g.goodyHutEffectFactor());
	if (iGold != 0
			// advc.314: isTech means that iGold is the research progress
			&& !goody.isTech())
	{
		changeGold(iGold);
		szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_GOLD", iGold);
	}
	/*  advc.134: (Moved the addMessage down b/c some of the handlers
		have custom messages) */

	int iRange = goody.getMapRange();
	if (iRange > 0)
	{
		int const iOffset = goody.getMapOffset();
		CvPlot* pBestPlot = NULL;
		if (iOffset > 0)
		{
			int iBestValue = 0;
			for (SquareIter it(*pPlot, iOffset, false); it.hasNext(); ++it)
			{
				CvPlot& kLoopPlot = *it;
				if (kLoopPlot.isRevealed(getTeam()))
					continue; // advc
				int iValue = 1 + g.getSorenRandNum(10000, "Goody Map");
				iValue *= it.currPlotDist();
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = &kLoopPlot;
				}
			}
		}
		if (pBestPlot == NULL)
			pBestPlot = pPlot;

		for (PlotCircleIter it(*pBestPlot, iRange, false); it.hasNext(); ++it)
		{
			if (g.getSorenRandNum(100, "Goody Map") < goody.getMapProb())
				it->setRevealed(getTeam(), true, false, NO_TEAM, true);
		}
	}

	if (pUnit != NULL)
		pUnit->changeExperience(goody.getExperience());

	if (pUnit != NULL)
		pUnit->changeDamage(-(goody.getHealing()));

	if (goody.isTech())
	{
		TechTypes eBestTech = NO_TECH;
		int iBestValue = 0;
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{	// advc.314: Replacing the conditions below
			if(isGoodyTech((TechTypes)iI, iGold > 0))
			/*if (GC.getInfo((TechTypes) iI).isGoodyTech()) {
				//if (canResearch((TechTypes)iI), false, true) { // K-Mod */
			{
				int iValue = (1 + g.getSorenRandNum(10000, "Goody Tech"));
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestTech = ((TechTypes)iI);
				}
			}
		}
		FAssertMsg(eBestTech != NO_TECH, "BestTech is not assigned a valid value");
		// <advc.314> Most of the code from here on is modified
		CvTeam& kOurTeam = GET_TEAM(getTeam());
		if(iGold <= 0 || 0.8 * kOurTeam.getResearchLeft(eBestTech) <= iGold)
		{
			kOurTeam.setHasTech(eBestTech, true, getID(), true, true);
			if(isSignificantDiscovery(eBestTech))
				kOurTeam.setNoTradeTech(eBestTech, true);
		}
		else
		{
			kOurTeam.changeResearchProgress(eBestTech, iGold, getID());
			szBuffer = gDLL->getText("TXT_KEY_MISC_PROGRESS_TOWARDS_TECH", iGold,
					GC.getInfo(eBestTech).getDescription());
		}
	}
	std::vector<UnitTypes> aeBestUnits;
	CvCivilization const& kCiv = GET_PLAYER(BARBARIAN_PLAYER).getCivilization();
	/*  When units need to be placed, but no unit class is given.
		Times 3 b/c MinBarbarians is only a lower bound. */
	for(int i = 0; i < 3 * goody.getMinBarbarians(); i++)
	{
		UnitTypes eBestUnit = NO_UNIT;
		if(goody.getUnitClassType() == NO_UNITCLASS &&
			goody.getBarbarianUnitClass() == NO_UNITCLASS)
		{
			int iBestValue = 0;
			for (int j = 0; j < kCiv.getNumUnits(); j++)
			{
				UnitTypes eUnit = kCiv.unitAt(j);
				CvUnitInfo const& kUnit = GC.getInfo(eUnit);
				if(kUnit.getDomainType() != DOMAIN_LAND)
					continue;
				if(kUnit.getPrereqOrBonuses(0) == NO_BONUS &&
					kUnit.getPrereqAndBonus() == NO_BONUS &&
					kUnit.getCombat() > 0 &&
					(kUnit.getPrereqAndTech() == NO_TECH || // pre-Industrial:
					GC.getInfo(kUnit.getPrereqAndTech()).getEra() <= 3) &&
					GET_PLAYER(BARBARIAN_PLAYER).canTrain(eUnit, false, true))
				{
					int iValue = kUnit.getCombat() + (goody.isBad() ?
							// Randomize hostile units a bit
							g.getSorenRandNum(10, "advc.314") : 0);
					if(iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestUnit = eUnit;
					}
				}
			}
			FAssert(eBestUnit != NO_UNIT);
			aeBestUnits.push_back(eBestUnit);
		}
	}
	if (goody.getUnitClassType() != NO_UNITCLASS ||
		// Pick a unit based on Barbarian tech then
		(!goody.isBad() && goody.getMinBarbarians() > 0))
	{
		UnitTypes eUnit = NO_UNIT; // Declaration moved down
		UnitClassTypes eUnitClass = (UnitClassTypes)goody.getUnitClassType();
		if(eUnitClass != NO_UNITCLASS)
		{
			// Interpret BarbarianUnitClass as an upgrade to replace UnitClassType
			if(bUpgrade && goody.getBarbarianUnitClass() != NO_UNITCLASS)
			{
				eUnitClass = (UnitClassTypes)goody.getBarbarianUnitClass();
				// Upgrade applied, don't roll an additional outcome.
				bNoRecursion = true;
			}
			eUnit = getCivilization().getUnit(eUnitClass);
		}
		/*  Let MinBarbarians > 1 generate more than 1 unit
			(though I'm not using this so far; not tested either) */
		for(int i = 0; i < std::max(1, goody.getMinBarbarians()); i++)
		{
			UnitTypes eLoopUnit = eUnit;
			if(eLoopUnit == NO_UNIT && !aeBestUnits.empty())
				eLoopUnit = aeBestUnits[std::min((int)aeBestUnits.size() - 1, i)];
			FAssert(eLoopUnit != NO_UNIT);
			if(eLoopUnit != NO_UNIT)
			{
				CvUnit* pNewUnit = initUnit(eLoopUnit, pPlot->getX(), pPlot->getY());
				if(pNewUnit == NULL)
					continue;
				else FAssert(pNewUnit != NULL);
				szBuffer = gDLL->getText("TXT_KEY_GOODY_FREE_UNIT",
						GC.getInfo(eLoopUnit).getDescription());
				addGoodyMsg(szBuffer, *pPlot, goody.getSound());
				szBuffer.clear();
				if(pNewUnit->canAcquirePromotionAny())
					promoteFreeUnit(*pNewUnit, prUpgrade);
			}
		}
	}
	if(!szBuffer.empty()) // Moved from higher up
		addGoodyMsg(szBuffer, *pPlot, goody.getSound());
	/*  If not isBad, then BarbarianUnitClass has a different meaning, which is
		handled above. */
	if(goody.isBad() && (goody.getBarbarianUnitClass() != NO_UNITCLASS ||
		// Will use eBestUnit in this case
		(goody.getMinBarbarians() > 0 && goody.getBarbarianUnitProb() > 0)))
	{
		UnitTypes eUnit = NO_UNIT;
		UnitClassTypes eUnitClass = (UnitClassTypes)goody.getBarbarianUnitClass();
		if(eUnitClass != NO_UNITCLASS)
			eUnit = GET_PLAYER(BARBARIAN_PLAYER).getCivilization().getUnit(eUnitClass);
		int iMinBarbs = std::max((int)(std::sqrt(goody.getMinBarbarians() *
				g.goodyHutEffectFactor(false))), goody.getMinBarbarians());
		// Increase the probability proportional to the change to MinBarbs
		int iProb = (goody.getBarbarianUnitProb() * iMinBarbs) / goody.getMinBarbarians();
		int iMaxBarbs = iMinBarbs + 2; // upper bound for iPass=0
		int iBarbCount = 0;
		for(int iPass = 0; iPass < 2; iPass++)
		{
			if(iBarbCount >= iMinBarbs)
				continue;
			FOR_EACH_ENUM(Direction)
			{
				CvPlot* pLoopPlot = plotDirection(pPlot->getX(), pPlot->getY(), eLoopDirection);
				if(pLoopPlot == NULL || !pLoopPlot->sameArea(*pPlot) ||
					pLoopPlot->isImpassable() || pLoopPlot->getNumUnits() > 0)
				{
					continue;
				}
				if(iPass > 0 || (g.getSorenRandNum(100, "Goody Barbs") < iProb))
				{
					UnitTypes eLoopUnit = eUnit;
					if(eLoopUnit == NO_UNIT && !aeBestUnits.empty())
					{
						eLoopUnit = aeBestUnits[
								std::min((int)aeBestUnits.size() - 1, iBarbCount)];
					}
					FAssert(eLoopUnit != NO_UNIT);
					if(eLoopUnit != NO_UNIT) {
						GET_PLAYER(BARBARIAN_PLAYER).initUnit(eLoopUnit,
								pLoopPlot->getX(), pLoopPlot->getY(),
								((pLoopPlot->isWater()) ? UNITAI_ATTACK_SEA :
								UNITAI_ATTACK));
						iBarbCount++;
					}
					if ((iPass > 0 && iBarbCount >= iMinBarbs) || iBarbCount >= iMaxBarbs)
						break;
				}
			}
		}
	}
	if(bUpgrade && !bNoRecursion)
		doGoody(pPlot, pUnit, eGoody); // </advc.314>
}


void CvPlayer::doGoody(CvPlot* pPlot, CvUnit* pUnit, /* advc.314: */ GoodyTypes eTaboo)
{
	if (GC.getPythonCaller()->doGoody(*pPlot, pUnit, getID()))
		return;

	FAssert(pPlot->isGoody() /* advc.314: */ || eTaboo != NO_GOODY);
	pPlot->removeGoody();
	// <advc>
	if(isBarbarian())
	{
		FAssertMsg(pPlot->isOwned(), "Barbarians should remove hut only when receiving a city");
		return;
	} // </advc>
	int const iAttempts = GC.getDefineINT("NUM_DO_GOODY_ATTEMPTS"); // advc.opt
	for (int iI = 0; iI < iAttempts; iI++)
	{
		if (GC.getInfo(getHandicapType()).getNumGoodies() <= 0)
			continue; // advc
		GoodyTypes eGoody = (GoodyTypes)GC.getInfo(getHandicapType()).
				getGoodies(GC.getGame().getSorenRandNum(
				GC.getInfo(getHandicapType()).getNumGoodies(), "Goodies"));
		FAssert(eGoody >= 0 && eGoody < GC.getNumGoodyInfos());
		// <advc.314>
		if(eGoody == eTaboo || (eTaboo != NO_GOODY && GC.getInfo(eGoody).isBad() !=
			GC.getInfo(eTaboo).isBad())) // Don't pair a good with a bad outcome
		{
			continue;
		} // </advc.314>
		if (canReceiveGoody(pPlot, eGoody, pUnit))
		{
			receiveGoody(pPlot, eGoody, pUnit, /* advc.314: */ eTaboo != NO_GOODY);
			// advc (note): pUnit can be NULL here, but a CyUnit for Python can still be created.
			CvEventReporter::getInstance().goodyReceived(getID(), pPlot, pUnit, eGoody);
			break;
		}
	}
}


bool CvPlayer::canFound(int iX, int iY, bool bTestVisible) const  // advc: some style changes
{
	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	// <advc>
	if (pPlot == NULL)
	{
		FAssert(pPlot != NULL);
		return false;
	} // </advc>

	if (pPlot->isImpassable())
		return false;

//===NM=====Mountains Mod===0=====
	if (pPlot->isPeak())
	{
		if (GC.getGame().isOption(GAMEOPTION_MOUNTAINS))//AND Mountains Option
			{
			//if (GC.getDefineINT(CvGlobals::MIN_CITY_RANGE)== 0)	
			if (GC.getDefineINT("PEAK_CAN_FOUND_CITY") == 0)
			{
				return false;
			}
		}
	}
//===NM=====Mountains Mod===X=====

	bool bValid = false; // advc.opt: Water check moved up
	/*  UNOFFICIAL_PATCH, Bugfix, 02/16/10, EmperorFool & jdog5000:
		(canFoundCitiesOnWater callback handling was incorrect and ignored isWater() if it returned true) */
	if (pPlot->isWater())
	{
		if (GC.getPythonCaller()->canFoundWaterCity(*pPlot))
		{
			bValid = true;
			FAssertMsg(false, "The AdvCiv mod probably does not support cities on water"); // advc
		}
		else return false; // advc.opt
	}

	if (GC.getGame().isFinalInitialized() && getNumCities() > 0 &&
		GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		return false; // (advc.opt: Moved down)
	}

	if (pPlot->isOwned() && pPlot->getOwner() != getID())
		return false;
	if (pPlot->isFeature() && GC.getInfo(pPlot->getFeatureType()).isNoCity())
		return false; // (advc.opt: Moved down)
	if (!bValid)
	{
		if (GC.getInfo(pPlot->getTerrainType()).isFound())
			bValid = true;
	}
	if (!bValid)
	{
		if (GC.getInfo(pPlot->getTerrainType()).isFoundCoast() && pPlot->isCoastalLand())
			bValid = true;
	}
	if (!bValid)
	{
		if (GC.getInfo(pPlot->getTerrainType()).isFoundFreshWater() &&
			pPlot->isFreshWater())
		{
			bValid = true;
		}
	}

	if (!bValid)
		return false;
	// advc.003y: Moved down
	if (GC.getPythonCaller()->cannotFoundCityOverride(*pPlot, getID()))
		return false;

	if (bTestVisible)
		return true;

	for (SquareIter it(*pPlot, GC.getDefineINT(CvGlobals::MIN_CITY_RANGE)); it.hasNext(); ++it)
	{
		if (it->isCity() && it->sameArea(*pPlot))
			return false;
	}
	return true;
}


void CvPlayer::found(int iX, int iY)  // advc: some style changes
{
	if (!canFound(iX, iY))
		return;
	// <advc.031c>
	if (gFoundLogLevel > 0 && !isHuman() &&
			// (advc.108 forces founding in place in scenarios)
			(getNumCities() > 0 || !GC.getGame().isScenario()))
		AI().logFoundValue(iX, iY); // </advc.031c>

	CvCity* pCity = initCity(iX, iY, true, true);
	FAssertMsg(pCity != NULL, "City is not assigned a valid value");
	CvGame const& g = GC.getGame();

	if (isBarbarian())
	{
		UnitTypes eDefenderUnit = pCity->AI().AI_bestUnitAI(UNITAI_CITY_DEFENSE);
		if (eDefenderUnit == NO_UNIT)
			eDefenderUnit = pCity->AI().AI_bestUnitAI(UNITAI_ATTACK);

		if (eDefenderUnit != NO_UNIT)
		{
			for (int i = 0; i < GC.getInfo(g.getHandicapType()).getBarbarianInitialDefenders(); i++)
			{
				initUnit(eDefenderUnit, iX, iY, UNITAI_CITY_DEFENSE);
			}
		}
	}

	CvCivilization const& kCiv = getCivilization(); // advc.003w
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes eLoopBuilding = kCiv.buildingAt(i);
		if(g.isFreeStartEraBuilding(eLoopBuilding))
		{
			if (pCity->canConstruct(eLoopBuilding))
				pCity->setNumRealBuilding(eLoopBuilding, 1);
		}
	}

	if (getAdvancedStartPoints() >= 0)
	{	// Free border expansion for Creative
		bool bCreative = false;
		for (int i = 0; i < GC.getNumTraitInfos(); i++)
		{
			TraitTypes eTrait = (TraitTypes)i;
			if (hasTrait(eTrait))
			{
				if (GC.getInfo(eTrait).getCommerceChange(COMMERCE_CULTURE) > 0)
				{
					bCreative = true;
					break;
				}
			}
		}

		if (bCreative)
		{
			for (int i = 0; i < GC.getNumCultureLevelInfos(); i++)
			{
				int iCulture = g.getCultureThreshold((CultureLevelTypes)i);
				if (iCulture > 0)
				{
					pCity->setCulture(getID(), iCulture, true, true);
					break;
				}
			}
		}
	}

	if (isHuman() && getAdvancedStartPoints() < 0)
	{
		pCity->chooseProduction(
				/*  advc.124g: Somehow, the choose-production popup comes up twice
					(despite being pushed only once) in multiplayer if the
					choose-tech popup is handled before choose-production. */
				NO_UNIT, NO_BUILDING, NO_PROJECT, false, g.isNetworkMultiPlayer());
	}
	else pCity->doFoundMessage();
	// <advc.210c>
	for (int i = 0; i < MAX_CIV_PLAYERS; i++)
	{
		CvPlayer const& kObs = GET_PLAYER((PlayerTypes)i);
		if (kObs.isAlive() && pCity->getOwner() != kObs.getID() &&
			pCity->isRevealed(kObs.getTeam()))
		{
			gDLL->UI().addMessage(kObs.getID(), false, -1,
					gDLL->getText("TXT_KEY_MORECIV4LERTS_CITY_FOUNDED",
					getNameKey(), pCity->getNameKey()),
					pCity->getPlot(), 0, MESSAGE_TYPE_INFO,
					ARTFILEMGR.getInterfaceArtPath("WORLDBUILDER_CITY_EDIT"));

		}
	} // </advc.210c>
	if (!CvPlot::isAllFog()) // advc.706: Suppress name-city popup
		CvEventReporter::getInstance().cityBuilt(pCity);
	if (gPlayerLogLevel >= 1 || /* advc.031c: */ gFoundLogLevel >= 1) // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000:
		logBBAI("  Player %d (%S) founds new city %S at %d, %d", getID(), getCivilizationDescription(0), pCity->getName(0).GetCString(), iX, iY);
}


bool CvPlayer::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost) const
{
	//PROFILE_FUNC(); // advc.003o
	/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
	if (!hasValidCivics(eUnit))
	{
		return false;
	}
/**
 ** End: Unit Civic Prereq
 **/

	UnitClassTypes eUnitClass = GC.getInfo(eUnit).getUnitClassType();

	/*	K-Mod note. This assert can fail if team games when checking whether this city can
		upgrade a unit to one of our team member's UUs. */
	//FAssert(GC.getInfo(getCivilizationType()).getCivilizationUnits(eUnitClass) == eUnit);
	if (getCivilization().getUnit(eUnitClass) != eUnit)
		return false;

	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		if (GC.getInfo(eUnit).isFound())
			return false;
	}

	if (!GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getInfo(eUnit).getPrereqAndTech()))
		return false;

	for (int i = 0; i < GC.getNUM_UNIT_AND_TECH_PREREQS(eUnit); i++)
	{
		if (GC.getInfo(eUnit).getPrereqAndTechs(i) != NO_TECH &&
			!GET_TEAM(getTeam()).isHasTech(GC.getInfo(eUnit).getPrereqAndTechs(i)))
		{
			return false;
		}
	}

/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* CanTrain  -new unit options                                                                                   */
/************************************************************************************************/
	if (GC.getInfo(eUnit).getMaxStartEra() != NO_ERA)
	{
		if (GC.getGame().getStartEra() > GC.getInfo(eUnit).getMaxStartEra())
		{
			return false;
		}
	}

	if (GC.getInfo(eUnit).getForceObsoleteTech() != NO_TECH)
	{
		if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getInfo(eUnit).getForceObsoleteTech())))
		{
			return false;
		}
	}

	if (GC.getInfo(eUnit).getPrereqGameOption() != NO_GAMEOPTION)
	{
		if (!(GC.getGame().isOption((GameOptionTypes)GC.getInfo(eUnit).getPrereqGameOption())))
		{
			return false;
		}
	}
	
	if (GC.getInfo(eUnit).getNotGameOption() != NO_GAMEOPTION)
	{
		if (GC.getGame().isOption((GameOptionTypes)GC.getInfo(eUnit).getNotGameOption()))
		{
			return false;
		}
	}
/*
	bool bNoReqCivic = true;
	bool bHasReqTech = false;
	for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (GC.getInfo(eUnit).isPrereqOrCivics(iI))
		{
			bNoReqCivic = false;
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getCivicInfo(CivicTypes(iI))).getTechPrereq()))
			{
				bHasReqTech = true;
			}
		}
	}
	if (!bNoReqCivic && !bHasReqTech)
	{
		return false;
	}*/

/*		bool bValid = false;
		bool bNoReqCivic = true;
		for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			if (GC.getUnitInfo(eUnit).isPrereqOrCivics(iI))
			{
				bNoReqCivic = false;
				if (isCivic(CivicTypes(iI)))
				{
					bValid = true;
				}
			}
		}
		if (!bNoReqCivic && !bValid)
		{
			return false;
		}
*/
//MERGED DUPLICATE CODES - KELDATH
	if (GC.getInfo(eUnit).isStateReligion())
		{
			if (getStateReligion() == NO_RELIGION)
			{
				return false;
			}
		}
/************************************************************************************************/
/* REVDCM                                  END CanTrain                                         */
/************************************************************************************************/

	if (GC.getInfo(eUnit).getStateReligion() != NO_RELIGION)
	{
		if (getStateReligion() != GC.getInfo(eUnit).getStateReligion())
			return false;
	}
	// <advc> Some checks moved to CvGame
	if (!GC.getGame().canTrain(eUnit, bIgnoreCost, bTestVisible))
		return false; // </advc>

	/*if (GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass))
		return false;
	if (isUnitClassMaxedOut(eUnitClass))
		return false;*/ // BtS - disabled by K-Mod.
	/*	Note that unlike the global limit, these two limits apply to the
		number of units currently alive rather than the total ever trained.
		Therefore these limits should be ignored for the visibility test. */

	if (!bTestVisible)
	{
		if (GC.getGame().isUnitClassMaxedOut(eUnitClass,
			GET_TEAM(getTeam()).getUnitClassMaking(eUnitClass) + (bContinue ? -1 : 0)))
		{
			return false;
		}
		if (GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass,
			GET_TEAM(getTeam()).getUnitClassMaking(eUnitClass) + (bContinue ? -1 : 0)))
		{
			return false;
		}
		if (isUnitClassMaxedOut(eUnitClass,
			getUnitClassMaking(eUnitClass) + (bContinue ? -1 : 0)))
		{
			return false;
		}
	}

	return true;
}


bool CvPlayer::canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreTech) const  // advc: style changes
{
	/*  advc: Global checks moved into new function. These are all fast (now that
		CivTeamsEverAlive is cached). */
	if(!GC.getGame().canConstruct(eBuilding, bIgnoreCost, bTestVisible))
		return false;

	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	BuildingClassTypes eBuildingClass = kBuilding.getBuildingClassType();
	// (advc.003w: Usually, the caller checks this now, but can't rely on that.)
	if (getCivilization().getBuilding(eBuildingClass) != eBuilding)
		return false;
/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* CanConstruct       building need option                                                                          */
/************************************************************************************************/
/* moved to cvgame suggested by f1 advc - keldath
	if (GC.getBuildingInfo(eBuilding).getPrereqGameOption() != NO_GAMEOPTION)
	{
		if (!(g.isOption((GameOptionTypes)GC.getBuildingInfo(eBuilding).getPrereqGameOption())))
		{
			return false;
		}
	}

	if (GC.getBuildingInfo(eBuilding).getNotGameOption() != NO_GAMEOPTION)
	{
		if (g.isOption((GameOptionTypes)GC.getBuildingInfo(eBuilding).getNotGameOption()))
		{
			return false;
		}
	}
*/
/************************************************************************************************/
/* REVDCM                                  END CanContstruct                                    */
/************************************************************************************************/

	CvTeamAI& kOurTeam = GET_TEAM(getTeam());
	if (!bIgnoreTech) // K-Mod
	{
		if (!kOurTeam.isHasTech(kBuilding.getPrereqAndTech()))
			return false;

		for (int i = 0; i < GC.getNUM_BUILDING_AND_TECH_PREREQS(); i++)
		{
			if (!kOurTeam.isHasTech(kBuilding.getPrereqAndTechs(i)))
				return false;
		}
	}

	if (kOurTeam.isObsoleteBuilding(eBuilding))
		return false;
	{
		SpecialBuildingTypes eSpecial = kBuilding.getSpecialBuildingType();
		if (eSpecial != NO_SPECIALBUILDING && !kOurTeam.isHasTech((TechTypes)
			GC.getInfo(eSpecial).getTechPrereq()))
		{
			return false;
		}
	}
	{
		ReligionTypes ePrereqStateReligion = GC.getInfo(eBuilding).getStateReligion();
		if (ePrereqStateReligion != NO_RELIGION && ePrereqStateReligion != getStateReligion())
			return false;
	}
	{
		VictoryTypes ePrereqVictory = (VictoryTypes)kBuilding.getVictoryPrereq();
		if (ePrereqVictory != NO_VICTORY)
		{
			if (isMinorCiv() || isBarbarian())
				return false;

			if (kOurTeam.getVictoryCountdown(ePrereqVictory) >= 0)
				return false;
		}
	}
	{
		CorporationTypes eFoundCorp = kBuilding.getFoundsCorporation();
		if (eFoundCorp != NO_CORPORATION && isNoCorporations())
			return false;
	}
	// <dlph.19> (advc: simplified)
	if (kBuilding.isCapital() && GC.getGame().getGameState() == GAMESTATE_ON &&
		GET_TEAM(getTeam()).isAnyVictoryCountdown()) // advc.opt
	{
		return false;
	} // </dlph.19>
	if (kOurTeam.isBuildingClassMaxedOut(eBuildingClass))
		return false;

	if (isBuildingClassMaxedOut(eBuildingClass))
		return false;

	CvCivilization const& kCiv = getCivilization(); // advc.003w
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes ePrereqBuilding = kCiv.buildingAt(i);
		BuildingClassTypes ePrereqClass = kCiv.buildingClassAt(i);
		if (ePrereqBuilding != NO_BUILDING && kOurTeam.isObsoleteBuilding(ePrereqBuilding) &&
				getBuildingClassCount(ePrereqClass) <
				getBuildingClassPrereqBuilding(eBuilding, ePrereqClass, 0))
			return false;
	}

	if(bTestVisible)
		return true;

	if (getHighestUnitLevel() < kBuilding.getUnitLevelPrereq())
		return false;
	{
		int iTeamMaking = kOurTeam.getBuildingClassMaking(eBuildingClass);
		int iTeamMakingCont = iTeamMaking + (bContinue ? -1 : 0);
		if (GC.getGame().isBuildingClassMaxedOut(eBuildingClass, iTeamMakingCont))
			return false;
		if (kOurTeam.isBuildingClassMaxedOut(eBuildingClass, iTeamMakingCont))
			return false;
		int iMaking = getBuildingClassMaking(eBuildingClass);
		int iMakingCont = iMaking + (bContinue ? -1 : 0);
		if (isBuildingClassMaxedOut(eBuildingClass, iMakingCont))
			return false;
		for (int i = 0; i < GC.getNumBuildingClassInfos(); i++)
		{
			BuildingClassTypes ePrereqClass = (BuildingClassTypes)i;
			if (getBuildingClassCount(ePrereqClass) < getBuildingClassPrereqBuilding(
					eBuilding, ePrereqClass, bContinue ? 0 : iMaking))
				return false;
		}
	}

	return true;
}


bool CvPlayer::canCreate(ProjectTypes eProject, bool bContinue, bool bTestVisible) const
{
	if (isBarbarian())
	{
		return false;
	}

	if (GC.getInfo(eProject).getProductionCost() == -1)
	{
		return false;
	}
	// davidlallen: project civilization and free unit start
	int eCiv = GC.getProjectInfo(eProject).getCivilization();
	if ((eCiv != -1) && (eCiv != getCivilizationType()))
	{
		return false;
	}
	// davidlallen: project civilization and free unit end

	if (!(GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getInfo(eProject).getTechPrereq()))))
	{
		return false;
	}

	if (GC.getInfo(eProject).getVictoryPrereq() != NO_VICTORY)
	{
		if (!(GC.getGame().isVictoryValid((VictoryTypes)(GC.getInfo(eProject).getVictoryPrereq()))))
		{
			return false;
		}

		if (isMinorCiv())
		{
			return false;
		}

		if (GET_TEAM(getTeam()).getVictoryCountdown((VictoryTypes)GC.getInfo(eProject).getVictoryPrereq()) >= 0)
		{
			return false;
		}
	}

	if (GC.getGame().isProjectMaxedOut(eProject))
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
	{
		return false;
	}

	if (!bTestVisible)
	{
		if (GC.getGame().isProjectMaxedOut(eProject, (GET_TEAM(getTeam()).getProjectMaking(eProject) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject, (GET_TEAM(getTeam()).getProjectMaking(eProject) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		if (GC.getGame().isNoNukes())
		{
			if (GC.getInfo(eProject).isAllowsNukes())
			{
				return false; // advc.opt
				/*for (iI = 0; iI < GC.getNumUnitInfos(); iI++) {
					if (GC.getInfo((UnitTypes)iI).getNukeRange() != -1)
						return false;
				}*/
			}
		}

		if (GC.getInfo(eProject).getAnyoneProjectPrereq() != NO_PROJECT)
		{
			if (GC.getGame().getProjectCreatedCount((ProjectTypes)(GC.getInfo(eProject).getAnyoneProjectPrereq())) == 0)
			{
				return false;
			}
		}

		for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			if (GET_TEAM(getTeam()).getProjectCount((ProjectTypes)iI) < GC.getInfo(eProject).getProjectsNeeded(iI))
			{
				return false;
			}
		}
	}

	return true;
}


bool CvPlayer::canMaintain(ProcessTypes eProcess, bool bContinue) const
{
	if (!GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getInfo(eProcess).getTechPrereq()))
	{
		return false;
	}

	return true;
}


bool CvPlayer::isProductionMaxedUnitClass(UnitClassTypes eUnitClass) const
{
	if (eUnitClass == NO_UNITCLASS)
	{
		return false;
	}

	if (GC.getGame().isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	if (isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	return false;
}


bool CvPlayer::isProductionMaxedBuildingClass(BuildingClassTypes eBuildingClass, bool bAcquireCity) const
{
	if (eBuildingClass == NO_BUILDINGCLASS)
	{
		return false;
	}

	if (!bAcquireCity)
	{
		if (GC.getGame().isBuildingClassMaxedOut(eBuildingClass))
		{
			return true;
		}
	}

	if (GET_TEAM(getTeam()).isBuildingClassMaxedOut(eBuildingClass))
	{
		return true;
	}

	if (isBuildingClassMaxedOut(eBuildingClass, ((bAcquireCity) ? GC.getInfo(eBuildingClass).getExtraPlayerInstances() : 0)))
	{
		return true;
	}

	return false;
}


bool CvPlayer::isProductionMaxedProject(ProjectTypes eProject) const
{
	if (eProject == NO_PROJECT)
	{
		return false;
	}

	if (GC.getGame().isProjectMaxedOut(eProject))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
	{
		return true;
	}

	return false;
}


int CvPlayer::getProductionNeeded(UnitTypes eUnit,
	int iExtraInstances) const // advc.104
{
	UnitClassTypes eUnitClass = GC.getInfo(eUnit).getUnitClassType();
	FAssert(NO_UNITCLASS != eUnitClass);

	int iProductionNeeded = GC.getInfo(eUnit).getProductionCost();

	iProductionNeeded *= 100 + (GC.getInfo(eUnitClass).getInstanceCostModifier() *
			(getUnitClassCount(eUnitClass) + /* advc.104: */ iExtraInstances));
	iProductionNeeded /= 100;

	static int const iUNIT_PRODUCTION_PERCENT = GC.getDefineINT("UNIT_PRODUCTION_PERCENT"); // advc.opt
	iProductionNeeded *= iUNIT_PRODUCTION_PERCENT;
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getInfo(GC.getGame().getGameSpeedType()).getTrainPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getInfo(GC.getGame().getStartEra()).getTrainPercent();
	iProductionNeeded /= 100;
	/*  <advc.107> Code moved into auxiliary function b/c I need this modifier
		in AI_getTotalFloatingDefendersNeeded. */
	iProductionNeeded = ::roundToMultiple(iProductionNeeded *
			trainingModifierFromHandicap(GC.getInfo(eUnitClass).isWorldUnit()),
			isHuman() ? 5 : 1); // advc.251
	// </advc.107>
	// advc.251 (comment): See getNewCityProductionValue
	iProductionNeeded += getUnitExtraCost(eUnitClass);

	int iPyMod = GC.getPythonCaller()->unitCostMod(getID(), eUnit);
	if (iPyMod > 0)
	{
		iProductionNeeded *= iPyMod;
		iProductionNeeded /= 100;
	}
	return std::max(1, iProductionNeeded);
}


int CvPlayer::getProductionNeeded(BuildingTypes eBuilding) const
{
	CvGame const& g = GC.getGame(); // advc
	int iProductionNeeded = GC.getInfo(eBuilding).getProductionCost();

	static int const iBUILDING_PRODUCTION_PERCENT = GC.getDefineINT("BUILDING_PRODUCTION_PERCENT"); // advc.opt
	iProductionNeeded *= iBUILDING_PRODUCTION_PERCENT;
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getInfo(g.getGameSpeedType()).getConstructPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getInfo(g.getStartEra()).getConstructPercent();
	iProductionNeeded /= 100;
	// <advc.251>
	iProductionNeeded = ::roundToMultiple(0.01 * iProductionNeeded *
			GC.getInfo(getHandicapType()).getConstructPercent(),
			isHuman() ? 5 : 1);
	if (!isHuman()) // Barbarians too
	{
		CvHandicapInfo const& h = GC.getInfo(g.getHandicapType());
		int iAIModifier = //h.getAIPerEraModifier() * getCurrentEra()
				g.AIHandicapAdjustment();
		if(GC.getInfo(eBuilding).isWorldWonder())
			iAIModifier += h.getAIWorldConstructPercent();
		else iAIModifier += h.getAIConstructPercent();
		iProductionNeeded *= iAIModifier;
		iProductionNeeded /= 100;
	} // </advc.251>
/* Population Limit ModComp - Beginning : these code lines adjust buildings' costs to make them cheaper */
// option added by keldath after f1rpo suggestion
	if(!GC.getGame().isOption(GAMEOPTION_NO_POPULATION_LIMIT)){
			if (GC.getBuildingInfo(eBuilding).getPopulationLimitChange() != 0)
	{
		iProductionNeeded -= (((GC.getGame().getGameTurn() / 10) * (GC.getGame().getAdjustedPopulationLimitChange(GC.getBuildingInfo(eBuilding).getPopulationLimitChange()) * 10)) / 100);
		if (!(isHuman()))
		{
			iProductionNeeded /= 2;
		}
	}
}
	/* Population Limit ModComp - End */
	return std::max(1, iProductionNeeded);
}


int CvPlayer::getProductionNeeded(ProjectTypes eProject) const
{
	CvGame const& g = GC.getGame(); // advc
	// <advc.251>
	int const iBaseCost = GC.getInfo(eProject).getProductionCost();
	int iProductionNeeded = iBaseCost; // </advc.251>

	static int const iPROJECT_PRODUCTION_PERCENT = GC.getDefineINT("PROJECT_PRODUCTION_PERCENT"); // advc.opt
	iProductionNeeded *= iPROJECT_PRODUCTION_PERCENT;
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getInfo(g.getGameSpeedType()).getCreatePercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getInfo(g.getStartEra()).getCreatePercent();
	iProductionNeeded /= 100;
	// <advc.251>
	iProductionNeeded = ::roundToMultiple(0.01 * iProductionNeeded *
			GC.getInfo(getHandicapType()).getCreatePercent(),
			isHuman() ? (iBaseCost > 500 ? 50 : 5) : 1);
	if(!isHuman() && !isBarbarian())
	{
		CvHandicapInfo const& h = GC.getInfo(g.getHandicapType());
		int iAIModifier = //h.getAIPerEraModifier() * getCurrentEra()
				g.AIHandicapAdjustment();
		if(GC.getInfo(eProject).isWorldProject())
			iAIModifier += h.getAIWorldCreatePercent();
		else iAIModifier += h.getAICreatePercent();
		iProductionNeeded *= iAIModifier;
		iProductionNeeded /= 100;
	} // </advc.251>

	return std::max(1, iProductionNeeded);
}

int CvPlayer::getProductionModifier(UnitTypes eUnit) const
{
	int iMultiplier = 0;
	if (GC.getInfo(eUnit).isMilitaryProduction())
		iMultiplier += getMilitaryProductionModifier();

	FOR_EACH_ENUM(Trait)
	{
		if (!hasTrait(eLoopTrait))
			continue; // advc
		iMultiplier += GC.getInfo(eUnit).getProductionTraits(eLoopTrait);
		if (GC.getInfo(eUnit).getSpecialUnitType() != NO_SPECIALUNIT)
		{
			iMultiplier += GC.getInfo(GC.getInfo(eUnit).getSpecialUnitType()).
					getProductionTraits(eLoopTrait);
		}
	}

	return iMultiplier;
}

int CvPlayer::getProductionModifier(BuildingTypes eBuilding) const
{
	int iMultiplier = 0;
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	FOR_EACH_ENUM(Trait)
	{
		if (!hasTrait(eLoopTrait))
			continue; // advc
		iMultiplier += kBuilding.getProductionTraits(eLoopTrait);
		if (GC.getInfo(eBuilding).getSpecialBuildingType() != NO_SPECIALBUILDING)
		{
			iMultiplier += GC.getInfo(GC.getInfo(eBuilding).getSpecialBuildingType()).
					getProductionTraits(eLoopTrait);
		}
	}
	if (kBuilding.isWorldWonder())
		iMultiplier += getMaxGlobalBuildingProductionModifier();
	if (kBuilding.isTeamWonder())
		iMultiplier += getMaxTeamBuildingProductionModifier();
	if (kBuilding.isNationalWonder())
		iMultiplier += getMaxPlayerBuildingProductionModifier();

	return iMultiplier;
}

int CvPlayer::getProductionModifier(ProjectTypes eProject) const
{
	int iMultiplier = 0;
	if (GC.getInfo(eProject).isSpaceship())
		iMultiplier += getSpaceProductionModifier();
	return iMultiplier;
}

// advc.107: Cut from getProductionNeeded; refactored.
double CvPlayer::trainingModifierFromHandicap(bool bWorldClass) const
{
	// <advc.251>
	int iPercent = GC.getInfo(getHandicapType()).getTrainPercent();
	if(!isHuman()) // Apply this also to Barbarians // </advc.251>
	{
		CvHandicapInfo& h = GC.getInfo(GC.getGame().getHandicapType());
		int iAIPercent = //h.getAIPerEraModifier() * getCurrentEra()
				GC.getGame().AIHandicapAdjustment();
		if(bWorldClass)
			iAIPercent += h.getAIWorldTrainPercent();
		else iAIPercent += h.getAITrainPercent();
		iPercent *= iAIPercent;
		iPercent /= 100;
	}
	return 0.01 * std::max(iPercent, 1);
}

int CvPlayer::getBuildingClassPrereqBuilding(BuildingTypes eBuilding, BuildingClassTypes ePrereqBuildingClass, int iExtra) const
{
	CvBuildingInfo& kBuilding = GC.getInfo(eBuilding);

	int iPrereqs = kBuilding.getPrereqNumOfBuildingClass(ePrereqBuildingClass);

	// dont bother with the rest of the calcs if we have no prereqs
	if (iPrereqs < 1)
		return 0;

	BuildingClassTypes eBuildingClass = kBuilding.getBuildingClassType();

	iPrereqs *= std::max(0, (GC.getInfo(GC.getMap().getWorldSize()).getBuildingClassPrereqModifier() + 100));
	//iPrereqs /= 100;
	iPrereqs = (int)std::ceil(iPrereqs / 100.0); // advc.140: Round up

	if (!kBuilding.isLimited())
	{
		iPrereqs *= getBuildingClassCount(GC.getInfo(eBuilding).getBuildingClassType()) +
				iExtra + 1;
	}

	if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
		iPrereqs = std::min(1, iPrereqs);

	return iPrereqs;
}


void CvPlayer::removeBuildingClass(BuildingClassTypes eBuildingClass)
{
	BuildingTypes eBuilding = getCivilization().getBuilding(eBuildingClass);
	if (eBuilding == NO_BUILDING)
		return;

	FOR_EACH_CITY_VAR(pLoopCity, *this)
	{
		if (pLoopCity->getNumRealBuilding(eBuilding) > 0)
		{
			pLoopCity->setNumRealBuilding(eBuilding,
					// UNOFFICIAL_PATCH, Bugfix, 02/16/10, EmperorFool
					pLoopCity->getNumRealBuilding(eBuilding) - 1);
			break;
		}
	}
}

// courtesy of the Gourd Bros...
void CvPlayer::processBuilding(BuildingTypes eBuilding, int iChange, CvArea& kArea)
{
	//DPII < Maintenance Modifiers >
    //CvArea* pArea = isArea(kArea)
    CvArea* pLoopArea = NULL;
	int iLoop = 0;

	//FAssert(iChange == 1 || iChange == -1);

    //DPII < Maintenance Modifiers >
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding); // advc

	if (kBuilding.getFreeBuildingClass() != NO_BUILDINGCLASS)
	{
		BuildingTypes eFreeBuilding = getCivilization().getBuilding((BuildingClassTypes)
				kBuilding.getFreeBuildingClass());
		// advc.303: Barbarians can't receive the Monuments from Stonehenge
		if(eFreeBuilding != NO_BUILDING)
			changeFreeBuildingCount(eFreeBuilding, iChange);
	}

	if (kBuilding.getCivicOption() != NO_CIVICOPTION)
	{	// <advc.004x>
		CivicOptionTypes eCivicOption = kBuilding.getCivicOption();
		CivicTypes eNewCivic = NO_CIVIC;
		if(iChange > 0 && isHuman() && !gDLL->GetWorldBuilderMode())
		{
			for(int i = 0; i < GC.getNumCivicInfos(); i++)
			{
				CivicTypes eCivic = (CivicTypes)i;
				if(GC.getInfo(eCivic).getCivicOptionType() == eCivicOption)
				{
					if(!canDoCivics(eCivic))
					{
						eNewCivic = eCivic;
						break;
					}
				}
			}
		} // </advc.004x>
		changeHasCivicOptionCount(eCivicOption, iChange);
		// <advc.004x>
		if(eNewCivic != NO_CIVIC)
			doChangeCivicsPopup(eNewCivic); // </advc.004x>
	}

	//DPII < Maintenance Modifiers >
    changeMaintenanceModifier(kBuilding.getGlobalMaintenanceModifier() * iChange);
	changeDistanceMaintenanceModifier(kBuilding.getDistanceMaintenanceModifier() * iChange);
	changeNumCitiesMaintenanceModifier(kBuilding.getNumCitiesMaintenanceModifier() * iChange);
	changeCoastalDistanceMaintenanceModifier(kBuilding.getCoastalDistanceMaintenanceModifier() * iChange);
	changeConnectedCityMaintenanceModifier(kBuilding.getConnectedCityMaintenanceModifier() * iChange);
	//DPII < Maintenance Modifiers >
	changeGreatPeopleRateModifier(kBuilding.getGlobalGreatPeopleRateModifier() * iChange);
	changeGreatGeneralRateModifier(kBuilding.getGreatGeneralRateModifier() * iChange);
	changeDomesticGreatGeneralRateModifier(kBuilding.getDomesticGreatGeneralRateModifier() * iChange);
	changeAnarchyModifier(kBuilding.getAnarchyModifier() * iChange);
	changeGoldenAgeModifier(kBuilding.getGoldenAgeModifier() * iChange);
	changeHurryModifier(kBuilding.getGlobalHurryModifier() * iChange);
	changeFreeExperience(kBuilding.getGlobalFreeExperience() * iChange);
	changeWarWearinessModifier(kBuilding.getGlobalWarWearinessModifier() * iChange);
	kArea.changeFreeSpecialist(getID(), (kBuilding.getAreaFreeSpecialist() * iChange));
	changeFreeSpecialist(kBuilding.getGlobalFreeSpecialist() * iChange);
	changeCoastalTradeRoutes(kBuilding.getCoastalTradeRoutes() * iChange);
	//changeTradeRoutes(kBuilding.getGlobalTradeRoutes() * iChange);
	// advc.310: Now per area
	kArea.changeTradeRoutes(getID(), kBuilding.getAreaTradeRoutes() * iChange);
	if (kBuilding.getAreaHealth() > 0)
	{
		kArea.changeBuildingGoodHealth(getID(), (kBuilding.getAreaHealth() * iChange));
	}
	else
	{
		kArea.changeBuildingBadHealth(getID(), (kBuilding.getAreaHealth() * iChange));
	}
	if (kBuilding.getGlobalHealth() > 0)
	{
		changeBuildingGoodHealth(kBuilding.getGlobalHealth() * iChange);
	}
	else
	{
		changeBuildingBadHealth(kBuilding.getGlobalHealth() * iChange);
	}
	kArea.changeBuildingHappiness(getID(), (kBuilding.getAreaHappiness() * iChange));
	changeBuildingHappiness(kBuilding.getGlobalHappiness() * iChange);
	changeWorkerSpeedModifier(kBuilding.getWorkerSpeedModifier() * iChange);
	changeSpaceProductionModifier(kBuilding.getGlobalSpaceProductionModifier() * iChange);
	changeCityDefenseModifier(kBuilding.getAllCityDefenseModifier() * iChange);
	kArea.changeCleanPowerCount(getTeam(), ((kBuilding.isAreaCleanPower()) ? iChange : 0));
	kArea.changeBorderObstacleCount(getTeam(), ((kBuilding.isAreaBorderObstacle()) ? iChange : 0));
	
	//DPII < Maintenance Modifiers >
    kArea.changeMaintenanceModifier(getID(), (kBuilding.getAreaMaintenanceModifier() * iChange));

    if (kBuilding.getOtherAreaMaintenanceModifier() != 0)
    {
        for (pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
        {
            if (!(pLoopArea == &kArea))
            {
                pLoopArea->changeMaintenanceModifier(getID(), (kBuilding.getOtherAreaMaintenanceModifier()  * iChange));
            }
        }
    }
    //DPII < Maintenance Modifiers >

	FOR_EACH_ENUM2(Yield, y)
	{
		changeSeaPlotYield(y, kBuilding.getGlobalSeaPlotYieldChange(y) * iChange);
		kArea.changeYieldRateModifier(getID(), y, kBuilding.getAreaYieldModifier(y) * iChange);
		changeYieldRateModifier(y, kBuilding.getGlobalYieldModifier(y) * iChange);
	}
	FOR_EACH_ENUM2(Commerce, c)
	{
		changeCommerceRateModifier(c, kBuilding.getGlobalCommerceModifier(c) * iChange);
		changeSpecialistExtraCommerce(c, kBuilding.getSpecialistExtraCommerce(c) * iChange);
		changeStateReligionBuildingCommerce(c, kBuilding.getStateReligionCommerce(c) * iChange);
		changeCommerceFlexibleCount(c, kBuilding.isCommerceFlexible(c) ? iChange : 0);
	}
	if (kBuilding.isAnyBuildingHappinessChanges()) // advc.003t
	{
		CvCivilization const& kCiv = getCivilization(); // advc.003w
		for (int i = 0; i < kCiv.getNumBuildings(); i++)
		{
			BuildingTypes eOldBuilding = kCiv.buildingAt(i);
			changeExtraBuildingHappiness(eOldBuilding,
					kBuilding.getBuildingHappinessChanges(eOldBuilding) * iChange);
		}
	}
	FOR_EACH_ENUM(Specialist)
	{
		FOR_EACH_ENUM(Yield)
		{
			changeSpecialistExtraYield(eLoopSpecialist, eLoopYield,
					GC.getInfo(eBuilding).getSpecialistYieldChange(
					eLoopSpecialist, eLoopYield) * iChange);
		}
	}
}


bool CvPlayer::canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestEra, bool bTestVisible) const
{
	//PROFILE_FUNC(); // advc.003o
	if (!pPlot->canBuild(eBuild, getID(), bTestVisible))
		return false;

	if (GC.getInfo(eBuild).getTechPrereq() != NO_TECH)
	{	// advc:
		TechTypes ePrereqTech = GC.getInfo(eBuild).getTechPrereq();
		if (!GET_TEAM(getTeam()).isHasTech(ePrereqTech))
		{
			if ((!bTestEra && !bTestVisible) ||
				getCurrentEra() + 1 < GC.getInfo(ePrereqTech).getEra())
			{
				return false;
			}
		}
	}

	if (!bTestVisible)
	{
		if (pPlot->isFeature())
		{
			if (!GET_TEAM(getTeam()).isHasTech(GC.getInfo(eBuild).
				getFeatureTech(pPlot->getFeatureType())))
			{
				return false;
			}
		}

		if (std::max(0, getGold()) < getBuildCost(pPlot, eBuild))
			return false;
	}

	return true;
}


int CvPlayer::getBuildCost(const CvPlot* pPlot, BuildTypes eBuild) const
{
	FAssert(eBuild >= 0 && eBuild < GC.getNumBuildInfos());

	if (pPlot->getBuildProgress(eBuild) > 0)
		return 0;

	return std::max(0, GC.getInfo(eBuild).getCost() * (100 + calculateInflationRate())) / 100;
}


RouteTypes CvPlayer::getBestRoute(const CvPlot* pPlot,
	BuildTypes* peBestBuild) const // advc.121
{
	PROFILE_FUNC();

	int iBestValue = 0;
	RouteTypes eBestRoute = NO_ROUTE;
	BuildTypes eBestBuild = NO_BUILD; // advc.121

	// BBAI TODO: Efficiency: Could cache this, decent savings on large maps
	// Perhaps save best route type per player each turn, then just check that
	// one first and only check others if can't do best.

	// K-Mod: I've reversed the order of iteration because the best builds are usually at the end.
	FOR_EACH_ENUM_REV(Build)
	{
		RouteTypes const eLoopRoute = GC.getInfo(eLoopBuild).getRoute();
		if (eLoopRoute == NO_ROUTE)
			continue; // advc
		// K-Mod: I've swapped the order of the if statments, because the value check is much faster. (faster trumps convention)
		int iValue = GC.getInfo(eLoopRoute).getValue();
		if (iValue > iBestValue)
		{
			if (pPlot != NULL ?
				(pPlot->getRouteType() == eLoopRoute || canBuild(pPlot, eLoopBuild)) :
				GET_TEAM(getTeam()).isHasTech(GC.getInfo(eLoopBuild).getTechPrereq()))
			{
				iBestValue = iValue;
				eBestRoute = eLoopRoute;
				eBestBuild = eLoopBuild; // advc.121
			}
		}
	}
	// <advc.121>
	if (peBestBuild != NULL)
		*peBestBuild = eBestBuild; // </advc.121>
	return eBestRoute;
}


int CvPlayer::getImprovementUpgradeRate() const
{
	int iRate = 1; // XXX

	iRate *= std::max(0, (getImprovementUpgradeRateModifier() + 100));
	iRate /= 100;

	return iRate;
}


int CvPlayer::calculateTotalYield(YieldTypes eYield) const
{
	PROFILE_FUNC();

	int iTotalCommerce = 0;
	FOR_EACH_CITY(pLoopCity, *this)
		iTotalCommerce += pLoopCity->getYieldRate(eYield);
	return iTotalCommerce;
}


int CvPlayer::calculateTotalCityHappiness() const
{
	int iTotalHappiness = 0;
	FOR_EACH_CITY(pLoopCity, *this)
		iTotalHappiness += pLoopCity->happyLevel();
	return iTotalHappiness;
}


int CvPlayer::calculateTotalExports(YieldTypes eYield) const
{
	int iTotalExports = 0;
	FOR_EACH_CITY(pLoopCity, *this)
	{
		for (int iTradeLoop = 0; iTradeLoop < pLoopCity->getTradeRoutes(); iTradeLoop++)
		{
			CvCity* pTradeCity = pLoopCity->getTradeCity(iTradeLoop);
			if (pTradeCity != NULL)
			{
				if (pTradeCity->getOwner() != getID())
					iTotalExports += pLoopCity->calculateTradeYield(eYield, pLoopCity->calculateTradeProfit(pTradeCity));
			}
		}
	}
	return iTotalExports;
}


int CvPlayer::calculateTotalImports(YieldTypes eYield) const
{
	int iTotalImports = 0;
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		if (iPlayerLoop == getID())
			continue;

		FOR_EACH_CITY(pLoopCity, GET_PLAYER((PlayerTypes)iPlayerLoop))
		{
			for (int iTradeLoop = 0; iTradeLoop < pLoopCity->getTradeRoutes(); iTradeLoop++)
			{
				CvCity* pTradeCity = pLoopCity->getTradeCity(iTradeLoop);
				if (pTradeCity != NULL)
				{
					if (pTradeCity->getOwner() == getID())
						iTotalImports += pLoopCity->calculateTradeYield(eYield, pLoopCity->calculateTradeProfit(pTradeCity));
				}
			}
		}
	}
	return iTotalImports;
}


int CvPlayer::calculateTotalCityUnhappiness() const
{
	int iTotalUnhappiness = 0;
	FOR_EACH_CITY(pLoopCity, *this)
		iTotalUnhappiness += pLoopCity->unhappyLevel();
	return iTotalUnhappiness;
}


int CvPlayer::calculateTotalCityHealthiness() const
{
	int iTotalHealthiness = 0;
	FOR_EACH_CITY(pLoopCity, *this)
		iTotalHealthiness += pLoopCity->goodHealth();
	return iTotalHealthiness;
}

int CvPlayer::calculateTotalCityUnhealthiness() const
{
	int iTotalUnhealthiness = 0;
	FOR_EACH_CITY(pLoopCity, *this)
		iTotalUnhealthiness += pLoopCity->badHealth();
	return iTotalUnhealthiness;
}

/* K-Mod
	calculate the pollution output of a civ.
	iTypes is a bit-field whose members are POLLUTION_POPULTION, _BUILDINGS, _BONUS, _POWER. (and _ALL) */
int CvPlayer::calculatePollution(int iTypes) const
{
	int iTotal = 0;

	int iBuildingWeight = ((iTypes & POLLUTION_BUILDINGS) == 0)?0 :GC.getDefineINT("GLOBAL_WARMING_BUILDING_WEIGHT");
	int iBonusWeight = ((iTypes & POLLUTION_BONUSES) == 0)?0 :GC.getDefineINT("GLOBAL_WARMING_BONUS_WEIGHT");
	int iPowerWeight = ((iTypes & POLLUTION_POWER) == 0)?0 :GC.getDefineINT("GLOBAL_WARMING_POWER_WEIGHT");
	int iPopWeight = ((iTypes & POLLUTION_POPULATION) == 0)?0 :GC.getDefineINT("GLOBAL_WARMING_POPULATION_WEIGHT");

	FOR_EACH_CITY(pCity, *this)
	{
		// note: "bad health" values are negative, except for population! (crazy, but true. Who writes this junk?)
		iTotal -=
			(pCity->totalBadBuildingHealth() * iBuildingWeight)
			+ (pCity->getBonusBadHealth() * iBonusWeight)
			+ (pCity->getPowerBadHealth() * iPowerWeight)
			- (pCity->unhealthyPopulation() * iPopWeight);
	}

	return iTotal;
}


int CvPlayer::getGwPercentAnger() const
{
	return m_iGwPercentAnger;
}


void CvPlayer::setGwPercentAnger(int iNewValue)
{
	if (iNewValue != m_iGwPercentAnger)
	{
		m_iGwPercentAnger = iNewValue;
		AI_makeAssignWorkDirty();
	}
} // K-Mod end

// K-Mod
int CvPlayer::getUnitCostMultiplier() const
{
	int iMultiplier = 100;
	iMultiplier *= GC.getInfo(getHandicapType()).getUnitCostPercent();
	iMultiplier /= 100;

	if (!isHuman() && !isBarbarian())
	{
		iMultiplier *= GC.getInfo(GC.getGame().getHandicapType()).getAIUnitCostPercent();
		iMultiplier /= 100;
		// advc.251: Gold costs are no longer adjusted to handicap
		/*iMultiplier *= std::max(0, ((GC.getInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iMultiplier /= 100;*/
	}

	return iMultiplier;
}
// K-Mod end

int CvPlayer::calculateUnitCost(int& iFreeUnits, int& iFreeMilitaryUnits, int& iPaidUnits,
	int& iPaidMilitaryUnits, int& iUnitCost, int& iMilitaryCost, int& iExtraCost,
	int iExtraPop, int iExtraUnits) const // advc.004b
{
	iFreeUnits = GC.getInfo(getHandicapType()).getFreeUnits();

	iFreeUnits += getBaseFreeUnits();
	iFreeUnits += (((getTotalPopulation()
		+ iExtraPop) // advc.004b
		* getFreeUnitsPopulationPercent()) / 100);

	iFreeMilitaryUnits = getBaseFreeMilitaryUnits();
	iFreeMilitaryUnits += (((getTotalPopulation()
		+ iExtraPop) // advc.004b
		* getFreeMilitaryUnitsPopulationPercent()) / 100);

	/*if (!isHuman()) {
		if (GET_TEAM(getTeam()).hasMetHuman()) {
			iFreeUnits += getNumCities(); // XXX
			iFreeMilitaryUnits += getNumCities(); // XXX
		}
	}*/ // BtS - Hidden AI bonus removed by BBAI.

	iPaidUnits = std::max(0, getNumUnits() - iFreeUnits
			+ iExtraUnits); // advc.004b
	iPaidMilitaryUnits = std::max(0, getNumMilitaryUnits()
			+ iExtraUnits // advc.004b
			- iFreeMilitaryUnits);
	//iSupport = 0;
	/*iBaseUnitCost = iPaidUnits * getGoldPerUnit();
	iMilitaryCost = iPaidMilitaryUnits * getGoldPerMilitaryUnit();
	iExtraCost = getExtraUnitCost();
	iSupport = iMilitaryCost + iBaseUnitCost + iExtraCost;
	iSupport *= GC.getInfo(getHandicapType()).getUnitCostPercent();
	iSupport /= 100;
	if (!isHuman() && !isBarbarian()) {
		iSupport *= GC.getInfo(GC.getGame().getHandicapType()).getAIUnitCostPercent();
		iSupport /= 100;
		iSupport *= std::max(0, ((GC.getInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iSupport /= 100;
	}*/ // BtS
	// K-Mod. GoldPerUnit, etc, are now done as percentages.
	// Also, "UnitCostPercent" handicap modifiers now apply directly to unit cost only, not military or extra cost.
	// (iBaseUnitCost is no longer fed back to the caller. Only the modified cost is.)
	iUnitCost = iPaidUnits * getGoldPerUnit() * getUnitCostMultiplier() / 10000;
	iMilitaryCost = iPaidMilitaryUnits * getGoldPerMilitaryUnit() / 100;
	// <advc.912b>
	if(!isHuman() && !isBarbarian())
	{
		iMilitaryCost = ::round(iMilitaryCost * 0.01 * GC.getInfo(
				GC.getGame().getHandicapType()).getAIUnitSupplyPercent());
	} // </advc.912b>
	iExtraCost = getExtraUnitCost() / 100;
	int iSupport = iUnitCost + iMilitaryCost + iExtraCost;
	// K-Mod end

	FAssert(iSupport >= 0);

	return std::max(0, iSupport);
}


int CvPlayer::calculateUnitCost(
		int iExtraPop, int iExtraUnits) const // advc.004b
{
	if(isAnarchy())
		return 0;

	/*  advc (note): Several distinct variables need to be passed b/c
		calculateUnitCost uses them for intermediate results */
	int iFreeUnits, iFreeMilitaryUnits, iPaidUnits, iPaidMilitaryUnits,
			iMilitaryCost, iBaseUnitCost, iExtraCost;
	return calculateUnitCost(iFreeUnits, iFreeMilitaryUnits, iPaidUnits, iPaidMilitaryUnits,
			iBaseUnitCost, iMilitaryCost, iExtraCost,
			iExtraPop, iExtraUnits); // advc.004b
}

int CvPlayer::calculateUnitSupply(/* advc.004b: */ int iExtraOutsideUnits) const
{
	if(isAnarchy())
		return 0;
	int iPaidUnits, iBaseSupplyCost;
	return calculateUnitSupply(iPaidUnits, iBaseSupplyCost,
			iExtraOutsideUnits); // advc.004b
}

int CvPlayer::calculateUnitSupply(int& iPaidUnits, int& iBaseSupplyCost,
		int iExtraOutsideUnits) const // advc.004b
{
	static int iINITIAL_FREE_OUTSIDE_UNITS = GC.getDefineINT("INITIAL_FREE_OUTSIDE_UNITS"); // advc.opt
	static int iINITIAL_OUTSIDE_UNIT_GOLD_PERCENT = GC.getDefineINT("INITIAL_OUTSIDE_UNIT_GOLD_PERCENT"); // advc.opt

	iPaidUnits = std::max(0, getNumOutsideUnits()
		+ iExtraOutsideUnits // advc.004b
		- iINITIAL_FREE_OUTSIDE_UNITS);

	iBaseSupplyCost = iPaidUnits * iINITIAL_OUTSIDE_UNIT_GOLD_PERCENT;
	iBaseSupplyCost /= 100;
	int iSupply = iBaseSupplyCost;
	if (!isHuman() && !isBarbarian())
	{
		iSupply *= GC.getInfo(GC.getGame().getHandicapType()).getAIUnitSupplyPercent();
		iSupply /= 100;
		// advc.250d: Commented out
		/*iSupply *= std::max(0, ((GC.getInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iSupply /= 100;*/
	}
	FAssert(iSupply >= 0);
	return iSupply;
}


int CvPlayer::calculatePreInflatedCosts() const
{
	return calculateUnitCost() + calculateUnitSupply() + getTotalMaintenance() + getCivicUpkeep() +
			GC.getPythonCaller()->extraExpenses(getID());
}


int CvPlayer::calculateInflationRate() const
{
	CvGame const& g = GC.getGame();
	int iTurns = (g.getGameTurn() + g.getElapsedGameTurns()) / 2;
	if (g.getMaxTurns() > 0)
		iTurns = std::min(g.getMaxTurns(), iTurns);
	iTurns += GC.getInfo(g.getGameSpeedType()).getInflationOffset();
	if (iTurns <= 0)
		return 0;

	int iInflationPerTurnTimes10000 = GC.getInfo(g.getGameSpeedType()).getInflationPercent();
	iInflationPerTurnTimes10000 *= GC.getInfo(getHandicapType()).getInflationPercent();
	iInflationPerTurnTimes10000 /= 100;

	int iModifier = m_iInflationModifier;
	if (!isHuman() && !isBarbarian())
	{
		int iAIModifier = GC.getInfo(g.getHandicapType()).getAIInflationPercent();
		// advc.251: Gold costs are no longer adjusted to handicap
		/*iAIModifier *= std::max(0, ((GC.getInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iAIModifier /= 100;*/
		iModifier += iAIModifier - 100;
	}

	iInflationPerTurnTimes10000 *= std::max(0, 100 + iModifier);
	iInflationPerTurnTimes10000 /= 100;

	// Keep up to second order terms in binomial series
	int iRatePercent = (iTurns * iInflationPerTurnTimes10000) / 100;
	iRatePercent += (iTurns * (iTurns - 1) * iInflationPerTurnTimes10000 *
			iInflationPerTurnTimes10000) / 2000000;
	FAssert(iRatePercent >= 0);
	return iRatePercent;
}


int CvPlayer::calculateInflatedCosts() const
{
	PROFILE_FUNC();

	int iCosts = calculatePreInflatedCosts();
	iCosts *= std::max(0, (calculateInflationRate() + 100));
	iCosts /= 100;
	return iCosts;
}


/*int CvPlayer::calculateBaseNetGold() const { // (see calculateGoldRate)
	int iNetGold = (getCommerceRate(COMMERCE_GOLD) + getGoldPerTurn());
	iNetGold -= calculateInflatedCosts();
	return iNetGold;
}*/

int CvPlayer::calculateResearchModifier(TechTypes eTech,  // <advc.910>
		int* piFromOtherKnown, int* piFromPaths, int* piFromTeam) const
{
	// So that the caller isn't required to provide the pointer params
	int iFromOtherKnown, iFromPaths, iFromTeam;
	if(piFromOtherKnown == NULL)
		piFromOtherKnown = &iFromOtherKnown;
	if(piFromPaths == NULL)
		piFromPaths = &iFromPaths;
	if(piFromTeam == NULL)
		piFromTeam = &iFromTeam;
	*piFromOtherKnown = *piFromPaths = *piFromTeam = 0;
	// </advc.910>
	int iModifier = 100;
	if(NO_TECH == eTech)
		return iModifier;
//kedlath - suggested by f1rpo :
//Kind of redundant: AdvCiv has TECH_COST_NOTRADE_MODIFIER, which gets applied in CvTeam::getResearchCost. (Also only applies after era 0.)
// ALN DuneWars Start
	// this should have been done a long time ago
	// since techs are passed around, the tech pace is quicker, lets clamp it a little
/*	if (!GC.getGame().isOption(GAMEOPTION_NO_TECH_TRADING) && GC.getGame().getCurrentEra() > 0)
	{
		iModifier -= 10;
	}
*/// ALN End
	// BETTER_BTS_AI_MOD, Tech Diffusion, 07/27/09, jdog5000: START
/* Tech Diffusion  GAMEOPTION*/
	//static bool const bTECH_DIFFUSION_ENABLE = GC.getDefineBOOL("TECH_DIFFUSION_ENABLE");
	if (GC.getGame().isOption(GAMEOPTION_TECH_DIFFUSION))
	//if (bTECH_DIFFUSION_ENABLE)
	{
		scaled rKnownExp; // advc: BBAI had used floating-point math
		// Tech flows better through open borders
		for (TeamIter<CIV_ALIVE,KNOWN_TO> it(getTeam()); it.hasNext(); ++it)
		{
			CvTeam const& kTechTeam = *it;
			if (!kTechTeam.isHasTech(eTech))
				continue;
			rKnownExp += fixp(0.5);
			if (GET_TEAM(getTeam()).isFriendlyTerritory(kTechTeam.getID()))
				rKnownExp += fixp(1.5);
			else if (kTechTeam.isAtWar(getTeam()) || GET_TEAM(getTeam()).isVassal(kTechTeam.getID()))
				rKnownExp += fixp(0.5);
		}
		static int const iTechDiffMod = GC.getDefineINT("TECH_DIFFUSION_KNOWN_TEAM_MODIFIER", 30);
		if (rKnownExp > 0)
		{
			*piFromOtherKnown += // advc.910
					iTechDiffMod - (iTechDiffMod * fixp(0.85).pow(rKnownExp)).round();
		}
		// Tech flows downhill to those who are far behind
		int iTechScorePercent = GET_TEAM(getTeam()).getBestKnownTechScorePercent();
		static int const iWelfareThreshold = GC.getDefineINT("TECH_DIFFUSION_WELFARE_THRESHOLD", 88);
		if (iTechScorePercent < iWelfareThreshold)
		{
			if (rKnownExp > 0)
			{
				static int const iWelfareModifier = GC.getDefineINT("TECH_DIFFUSION_WELFARE_MODIFIER", 30);
				*piFromOtherKnown += // advc.910
						(iWelfareModifier * GC.getGame().getCurrentEra() *
						(iWelfareThreshold - iTechScorePercent)) / 200;
			}
		}
	}
	else
	{	// BtS tech diffusion
		int iKnownCount = 0;
		for (TeamIter<CIV_ALIVE,KNOWN_TO> it(getTeam()); it.hasNext(); ++it)
		{
			if (it->isHasTech(eTech))
				iKnownCount++;
		}
		int const iPossibleKnownCount = TeamIter<CIV_ALIVE>::count();
		if (iPossibleKnownCount > 0)
		{
			static int iTECH_COST_TOTAL_KNOWN_TEAM_MODIFIER = GC.getDefineINT("TECH_COST_TOTAL_KNOWN_TEAM_MODIFIER"); // advc.opt
			*piFromOtherKnown += // advc.910
				(iTECH_COST_TOTAL_KNOWN_TEAM_MODIFIER * iKnownCount) / iPossibleKnownCount;
		}
	}
	iModifier += *piFromOtherKnown; // advc.910

	int iPossiblePaths = 0;
	int iUnknownPaths = 0;
	for (int i = 0; i < GC.getNUM_OR_TECH_PREREQS(); i++)
	{
		TechTypes eOrTech = (TechTypes)GC.getInfo(eTech).getPrereqOrTechs(i); // advc
		if (eOrTech != NO_TECH)
		{
			if (!GET_TEAM(getTeam()).isHasTech(eOrTech))
				iUnknownPaths++;
			iPossiblePaths++;
		}
	}
	FAssert(iPossiblePaths >= iUnknownPaths);
	if(iPossiblePaths > iUnknownPaths)
	{
		*piFromPaths += // advc.910
				GC.getDefineINT(CvGlobals::TECH_COST_FIRST_KNOWN_PREREQ_MODIFIER);
		iPossiblePaths--;
		*piFromPaths += (iPossiblePaths - iUnknownPaths) *
				GC.getDefineINT(CvGlobals::TECH_COST_KNOWN_PREREQ_MODIFIER);
	}
	// BETTER_BTS_AI_MOD: END
	iModifier += *piFromPaths;
	// <advc.156>
	for (MemberIter it(getTeam()); it.hasNext(); ++it)
	{
		CvPlayer const& kMember = *it;
		if (kMember.getID() != getID() && kMember.getCurrentResearch() == eTech)
		{
			*piFromTeam = // advc.910
					GC.getDefineINT(CvGlobals::RESEARCH_MODIFIER_EXTRA_TEAM_MEMBER); // advc.210
			break; // Or should the penalty stack?
		}
	}
	iModifier += *piFromTeam; // advc.910
	// </advc.156>
	iModifier -= groundbreakingPenalty(eTech); // advc.groundbr
	return iModifier;
}

/* int CvPlayer::calculateBaseNetResearch(TechTypes eTech) const {  // (see calculateResearchRate)
	TechTypes eResearchTech;
	if(eTech != NO_TECH)
		eResearchTech = eTech;
	else eResearchTech = getCurrentResearch();
	return (((GC.getDefineINT("BASE_RESEARCH_RATE") + getCommerceRate(COMMERCE_RESEARCH)) * calculateResearchModifier(eResearchTech)) / 100);
} */

// advc.groundbr: Loosely based on CvTeam::getSpreadResearchModifier in DoC
int CvPlayer::groundbreakingPenalty(TechTypes eTech) const
{
	bool const bAIEnable = GC.getDefineBOOL(CvGlobals::AI_GROUNDBREAKING_PENALTY_ENABLE);
	bool const bHumanEnable = GC.getDefineBOOL(CvGlobals::HUMAN_GROUNDBREAKING_PENALTY_ENABLE);
	if (isHuman() ? !bHumanEnable : !bAIEnable)
		return 0;
	EraTypes const eCurrentEra = getCurrentEra();
	EraTypes const eStartEra = GC.getGame().getStartEra();
	if (eCurrentEra <= eStartEra)
		return 0;
	int const iMaxPenalty = GC.getInfo(eCurrentEra).get(isHuman() ?
			CvEraInfo::HumanMaxGroundbreakingPenalty :
			CvEraInfo::AIMaxGroundbreakingPenalty);
	if (iMaxPenalty == 0)
		return 0;
	// For comments see TechDiffusion_GlobalDefines.xml or the original code (DoC mod)
	int iTotal = 0;
	int iHasTech = 0;
	for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
	{
		CvTeam const& kLoopTeam = GET_TEAM(it->getTeam());
		if (kLoopTeam.isCapitulated())
			continue;
		iTotal++;
		if (kLoopTeam.isHasTech(eTech))
			iHasTech++;
	}
	scaled const rPercentile(iTotal, 4); // quartile
	if (iHasTech >= rPercentile)
		return 0;
	return (iMaxPenalty * (rPercentile - iHasTech) / rPercentile).round();
}


int CvPlayer::calculateGoldRate() const
{	/*int iRate = 0;
	if(isCommerceFlexible(COMMERCE_RESEARCH))
		iRate = calculateBaseNetGold();
	else iRate = std::min(0, (calculateBaseNetResearch() + calculateBaseNetGold()));
	return iRate;*/ // BtS
	// K-Mod. (Just moved from calculateBaseNetGold.)
	int iNetGold = getCommerceRate(COMMERCE_GOLD) + getGoldPerTurn();
	iNetGold -= calculateInflatedCosts();
	return iNetGold;
	// K-Mod end
}


int CvPlayer::calculateResearchRate(TechTypes eTech) const
{	// <advc.004x> No BASE_RESEARCH_RATE either
	if(!isResearch())
		return 0; // </advc.004x>
	/*int iRate = 0;
	if(isCommerceFlexible(COMMERCE_RESEARCH))
		iRate = calculateBaseNetResearch(eTech);
	else iRate = std::max(1, (calculateBaseNetResearch(eTech) + calculateBaseNetGold()));
	return iRate;*/ // BtS
	// K-Mod. (Just moved from calculateBaseNetResearch.)
	// Note: the original code had a floor of 1. This version does not.
	TechTypes eResearchTech;
	if(eTech != NO_TECH)
		eResearchTech = eTech;
	else eResearchTech = getCurrentResearch();
	// advc.004x: BASE_RESEARCH_RATE now handled by getCommerceRate
	return (getCommerceRate(COMMERCE_RESEARCH) *
			calculateResearchModifier(eResearchTech)) / 100;
	// K-Mod end
}

int CvPlayer::calculateTotalCommerce() const
{	//int iTotalCommerce = calculateBaseNetGold() + calculateBaseNetResearch();
	/*int iTotalCommerce = calculateGoldRate() + calculateResearchRate(); // K-Mod
	for (int i = 0; i < NUM_COMMERCE_TYPES; ++i) {
		if (COMMERCE_GOLD != i && COMMERCE_RESEARCH != i)
			iTotalCommerce += getCommerceRate((CommerceTypes)i);
	}
	return iTotalCommerce;*/
	/*  <advc.004s> Replacing the K-Mod code above.
		Of course this isn't really "total commerce" anymore. */
	int r = getCommerceRate(COMMERCE_GOLD) + getGoldPerTurn() +
			getCommerceRate(COMMERCE_RESEARCH);
	return r;
	// </advc.004s>
}

bool CvPlayer::isResearch() const
{
	if (!GC.getPythonCaller()->canDoResearch(getID()))
		return false;
	return (isFoundedFirstCity() /* advc.004x: */ && !isAnarchy());
}


bool CvPlayer::canEverResearch(TechTypes eTech) const
{
	if (GC.getInfo(eTech).isDisable())
		return false;

	if (GC.getInfo(getCivilizationType()).isCivilizationDisableTechs(eTech))
		return false;

	if (GC.getPythonCaller()->cannotResearchOverride(getID(), eTech, false))
		return false;

	return true;
}


TechTypes CvPlayer::getDiscoveryTech(UnitTypes eUnit) const
{
	TechTypes eBestTech = NO_TECH;
	int iBestValue = 0;
	FOR_EACH_ENUM(Tech)
	{
		if (canResearch(eLoopTech))
		{
			int iValue = 0;
			FOR_EACH_ENUM(Flavor)
			{
				iValue += (GC.getInfo(eLoopTech).getFlavorValue(eLoopFlavor) *
						GC.getInfo(eUnit).getFlavorValue(eLoopFlavor));
			}
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestTech = eLoopTech;
			}
		}
	}
	return eBestTech;
}


/*** HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave START ***/
bool CvPlayer::canEverTrade(TechTypes eTech) const
{
	if (GC.getCivilizationInfo(getCivilizationType()).isCivilizationDisableTechs(eTech))
	{
		return false;
	}

/* 
//no need to use this arglists since python isnt used anyways with this mod
//f1rpo suggestion
	CyArgsList argsList;
	argsList.add(getID());
	argsList.add(eTech);
	argsList.add(false);
*/
//	long lResult=0;
//Python call. Potentially slow. There is
//in AdvCiv, which should have the same effect and only calls Python if the callback is enabled in XML. (No need for the argsList and lResult variables then.)
/*	gDLL->getPythonIFace()->callFunction(PYGameModule, "cannotResearch", argsList.makeFunctionArgs(), &lResult);
	if (lResult == 1)
	{
		return false;
	}
*/
	//keldath-QA2-done
	//from f1rpo USE_CANNOT_RESEARCH_CALLBACK
	//But I'd only enable it if cannotResearch actually does something in Python. I can't find that function in your old Doto files, so the author (Grave) probably only added the Python call in order to be consistent with CvPlayer::canEverResearch.
	//keldath qa3 - i got that code years ago, cant get the original, cant tell...
	//as it is now? no harm? i prefer not to use callback thing.
/*
		f1rpo -> I'd leave it as it is (with the callback disabled in XML). The point is apparently, that, when a Python modder uses (and enables) the cannotResearch callback to prevent some player from researching a tech, that player will also be prohibited from receiving that tech through trade. You should remove the argsList stuff though; CvPythonCaller::cannotResearchOverride doesn't need it.
*/	
	if (GC.getPythonCaller()->cannotResearchOverride(getID(), eTech, false))
		return false;
	return true;
}
/*** HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave END ***/

bool CvPlayer::canResearch(TechTypes eTech, bool bTrade, bool bFree,
		bool bCouldResearchAgain) const // advc.126
{
	if (GC.getPythonCaller()->canResearchOverride(getID(), eTech, bTrade))
		return true;

	/*  advc.004x: Commented out - shouldn't matter here.
		(And wouldn't prevent players from queuing up research during anarchy or
		before founding a city) */
	/*if(!isResearch() && !bFree && getAdvancedStartPoints() < 0)
		return false;*/

	if (GET_TEAM(getTeam()).isHasTech(eTech) /* advc.126: */ && !bCouldResearchAgain)
		return false;

	bool bFoundPossible = false;
	bool bFoundValid = false;
	for (int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(eTech); iI++)
	{
		TechTypes ePrereq = (TechTypes)GC.getInfo(eTech).getPrereqOrTechs(iI);
		// <advc.126> Cycle detection
		if(ePrereq == eTech)
		{
			FAssert(false);
			continue;
		} // </advc.126>
		if (ePrereq != NO_TECH)
		{
			bFoundPossible = true;

			if (GET_TEAM(getTeam()).isHasTech(ePrereq)
					// advc.126: Don't check recursively (for execution speed concerns)
					&& (bCouldResearchAgain || canResearch(ePrereq, false, true, true)))
			{
				if (!bTrade || GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING) || !GET_TEAM(getTeam()).isNoTradeTech(ePrereq))
				{
					bFoundValid = true;
					break;
				}
			}
		}
	}

	if (bFoundPossible && !bFoundValid)
		return false;

	for (int iI = 0; iI < GC.getNUM_AND_TECH_PREREQS(eTech); iI++)
	{
		TechTypes ePrereq = (TechTypes)GC.getInfo(eTech).getPrereqAndTechs(iI);
		if (ePrereq != NO_TECH)
		{
			if (!GET_TEAM(getTeam()).isHasTech(ePrereq)
					// advc.126:
					|| (bCouldResearchAgain && !canResearch(ePrereq, false, true, true)))
				return false;

			if (bTrade && !GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING) && GET_TEAM(getTeam()).isNoTradeTech(ePrereq))
				return false;
		}
	}

/*** HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave START ***/
	if (bTrade)
	{
		if (!canEverTrade(eTech))
		{
			return false;
		}
	}
	else
	{
	if (!canEverResearch(eTech))
		return false;
	}
/*** HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave END ***/

	return true;
}


TechTypes CvPlayer::getCurrentResearch() const
{
	CLLNode<TechTypes>* pResearchNode = headResearchQueueNode();
	if (pResearchNode != NULL)
		return pResearchNode->m_data;
	return NO_TECH;
}


bool CvPlayer::isCurrentResearchRepeat() const
{
	TechTypes eCurrentResearch = getCurrentResearch();
	if (eCurrentResearch == NO_TECH)
		return false;
	return GC.getInfo(eCurrentResearch).isRepeat();
}


bool CvPlayer::isNoResearchAvailable() const
{
	if (getCurrentResearch() != NO_TECH)
		return false;

	FOR_EACH_ENUM(Tech)
	{
		if (canResearch(eLoopTech))
			return false;
	}
	return true;
}


int CvPlayer::getResearchTurnsLeft(TechTypes eTech, bool bOverflow) const
{
	int iTurnsLeft = getResearchTurnsLeftTimes100(eTech, bOverflow);
	if(iTurnsLeft >= 0) // advc.004x
		iTurnsLeft = (iTurnsLeft + 99) / 100; // round up
	//return std::max(1, iTurnsLeft);
	return iTurnsLeft; // advc.004x: -1 now means infinitely many turns
}

int CvPlayer::getResearchTurnsLeftTimes100(TechTypes eTech, bool bOverflow) const  // advc: style changes
{
	// <advc>
	if (GET_TEAM(getTeam()).isHasTech(eTech))
		return 0; // </advc>
	int iResearchRate = 0;
	int iOverflow = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer const& kMember = GET_PLAYER((PlayerTypes)iI);
		if(!kMember.isAlive() || kMember.getTeam() != getTeam() ||
			!kMember.isResearch()) // advc.004x
		{
			continue;
		}
		if(iI == getID() || kMember.getCurrentResearch() == eTech)
		{
			//iResearchRate += GET_PLAYER((PlayerTypes)iI).calculateResearchRate(eTech);
			// K-Mod (replacing the minimum which used to be in calculateResearchRate)
				// advc.004x: Ensure only a non-negative rate (not positive)
			iResearchRate += std::max(0, kMember.calculateResearchRate(eTech));
			iOverflow += (kMember.getOverflowResearch() *
					calculateResearchModifier(eTech)) / 100;
		}
	}
	// BETTER_BTS_AI_MOD, Tech AI, 03/18/10, jdog5000: START
	// Mainly just so debug display shows sensible value
	int iResearchLeft = GET_TEAM(getTeam()).getResearchLeft(eTech);
	if(bOverflow)
		iResearchLeft -= iOverflow;
	// <advc.004x>
	if (iResearchLeft <= 0)
		return 1; // 1/100. getResearchTurnsLeft will round that up.
	// </advc.004x>
	iResearchLeft *= 100;
	if(iResearchRate <= 0)
	{
		//return iResearchLeft;
		return -1; // advc.004x
	} // BETTER_BTS_AI_MOD: END

	int iTurnsLeft = iResearchLeft / iResearchRate;
	if(iTurnsLeft * iResearchRate < iResearchLeft)
		iTurnsLeft++;
	//return std::max(1, iTurnsLeft);
	return iTurnsLeft; // advc.004x
}

// <advc.104>  (also used for advc.079)
bool CvPlayer::canSeeTech(PlayerTypes eOther) const
{
	// Partly based on drawTechDeals in ExoticForeignAdvisor.py
	if(eOther == NO_PLAYER)
		return false;
	CvTeam const& ourTeam = GET_TEAM(getTeam());
	CvTeam const& otherTeam = GET_TEAM(eOther);
	if(ourTeam.getID() == otherTeam.getID())
		return true;
	if(otherTeam.isVassal(ourTeam.getID()))
		return true; // Can see tech through "we'd like you to research ..."
	// advc.553: Make tech visible despite No Tech Trading (as in BtS)
	//REOPENED BY KELDATH - i dont wish to see techsif i cant trade them :)
	if(GC.getGame().isOption(GAMEOPTION_NO_TECH_TRADING) && !canSeeResearch(eOther))
		return false;
	if(!ourTeam.isAlive() || !otherTeam.isAlive() ||
			ourTeam.isBarbarian() || otherTeam.isBarbarian() ||
			ourTeam.isMinorCiv() || otherTeam.isMinorCiv())
		return false;
	return ourTeam.isHasMet(otherTeam.getID()) &&
			(ourTeam.isTechTrading() || otherTeam.isTechTrading());
} // </advc.104>

// K-Mod. Return true if this player can see what ePlayer is researching
bool CvPlayer::canSeeResearch(PlayerTypes ePlayer, /* advc.085: */ bool bCheckPoints) const
{
	FAssertBounds(0, MAX_PLAYERS, ePlayer);
	const CvPlayer& kOther = GET_PLAYER(ePlayer);

	if (kOther.getTeam() == getTeam() || GET_TEAM(kOther.getTeam()).isVassal(getTeam()))
		return true;

	if (!GET_TEAM(getTeam()).isHasMet(kOther.getTeam()))
		return false;

	if (!GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		for (EspionageMissionTypes i = (EspionageMissionTypes)0; i < GC.getNumEspionageMissionInfos(); i = (EspionageMissionTypes)(i+1))
		{
			CvEspionageMissionInfo& kMissionInfo = GC.getInfo(i);

			if (kMissionInfo.isSeeResearch() && kMissionInfo.isPassive() &&
				canDoEspionageMission(i, ePlayer, NULL, 0, NULL,
				bCheckPoints)) // advc.085
			{
				return true;
			}
		}
	}
	return false;
}

// return true if this player can see ePlayer's demographics (power graph, culture graph, etc.)
bool CvPlayer::canSeeDemographics(PlayerTypes ePlayer, /* advc.085: */ bool bCheckPoints) const 
{
	FAssertBounds(0, MAX_PLAYERS, ePlayer);
	const CvPlayer& kOther = GET_PLAYER(ePlayer);

	if (kOther.getTeam() == getTeam() || GET_TEAM(kOther.getTeam()).isVassal(getTeam()))
		return true;

	if (!GET_TEAM(getTeam()).isHasMet(kOther.getTeam()))
		return false;

	if (GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
		return true;

	for (EspionageMissionTypes i = (EspionageMissionTypes)0; i < GC.getNumEspionageMissionInfos(); i = (EspionageMissionTypes)(i+1))
	{
		CvEspionageMissionInfo& kMissionInfo = GC.getInfo(i);

		if (kMissionInfo.isSeeDemographics() && kMissionInfo.isPassive() &&
			canDoEspionageMission(i, ePlayer, NULL, 0, NULL,
			bCheckPoints)) // advc.085
		{
			return true;
		}
	}
	return false;
} // K-Mod end

/*  advc.085: !bDemographics means: return espionage needed to see research.
	Mix of code from canSeeDemographics, canSeeResearch and canDoEspionageMission. */
int CvPlayer::espionageNeededToSee(PlayerTypes ePlayer, bool bDemographics) const
{
	int r = MAX_INT;
	if(!(bDemographics ?
			canSeeDemographics(ePlayer, false) : canSeeResearch(ePlayer, false)))
		return r;
	int iEspionagePoints = GET_TEAM(getTeam()).getEspionagePointsAgainstTeam(TEAMID(ePlayer));
	for(int i = 0; i < GC.getNumEspionageMissionInfos(); i++)
	{
		EspionageMissionTypes eLoopMission = (EspionageMissionTypes)i;
		CvEspionageMissionInfo& kMission = GC.getInfo(eLoopMission);
		if(!kMission.isPassive())
			continue;
		if(bDemographics ? kMission.isSeeDemographics() : kMission.isSeeResearch())
		{
			r = std::min(r, getEspionageMissionCost(
					eLoopMission, ePlayer, NULL, 0, NULL) - iEspionagePoints);
		}
	}
	return r;
}

// advc.550e:
bool CvPlayer::isSignificantDiscovery(TechTypes eTech) const
{
	// (K-Mod comment moved here from CvDeal::startTrade)
	// Only adjust tech_from_any memory if this is a tech from a recent era
	// and the team receiving the tech isn't already more than 2/3 of the way through.
	// (This is to prevent the AI from being crippled by human players selling them lots of tech scraps.)
	// Note: the current game era is the average of all the player eras, rounded down. (It no longer includes barbs.)
	// advc: I'm going to use the recipient's era instead, the rest is as in K-Mod.
	return GC.getInfo(eTech).getEra() >= getCurrentEra() - 1 &&
			GET_TEAM(getTeam()).getResearchLeft(eTech) >
			GET_TEAM(getTeam()).getResearchCost(eTech) / 3;
}

bool CvPlayer::isCivic(CivicTypes eCivic) const
{
	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (getCivics((CivicOptionTypes)iI) == eCivic)
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::canDoCivics(CivicTypes eCivic) const
{
	PROFILE_FUNC();

	/*  UNOFFICIAL_PATCH, Tech AI (Bugfix), 02/16/10, jdog5000: START
		Circumvents second crash bug in simultaneous turns MP games */
	if (eCivic == NO_CIVIC)
		return true; // UNOFFICIAL_PATCH: END

	if (GC.getGame().isForceCivicOption((CivicOptionTypes)GC.getInfo(eCivic).getCivicOptionType()))
		return GC.getGame().isForceCivic(eCivic);

	if (GC.getPythonCaller()->canDoCivicOverride(getID(), eCivic))
		return true;

	if (!isHasCivicOption((CivicOptionTypes)GC.getInfo(eCivic).getCivicOptionType()) &&
		!GET_TEAM(getTeam()).isHasTech(GC.getInfo(eCivic).getTechPrereq()))
	{
		return false;
	}
	if (GC.getPythonCaller()->cannotDoCivicOverride(getID(), eCivic))
		return false;
	// <advc.912d>
	if(GC.getGame().isOption(GAMEOPTION_NO_SLAVERY) && isHuman())
	{
		FOR_EACH_ENUM(Hurry)
		{
			if(GC.getInfo(eCivic).isHurry(eLoopHurry) &&
				GC.getInfo(eLoopHurry).getProductionPerPopulation() > 0)
			{
				return false;
			}
		}
	} // </advc.912d>
	return true;
}


bool CvPlayer::canRevolution(CivicTypes* paeNewCivics) const
{
	if (isAnarchy())
	{
		return false;
	}

	if (getRevolutionTimer() > 0)
	{
		return false;
	}

	if (paeNewCivics == NULL)
	{	// XXX is this necessary?
		// ^advc: Only for the call in CvPlayer::doAdvancedStartAction I think
		for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			if (canDoCivics((CivicTypes)iI))
			{
				if (getCivics((CivicOptionTypes)GC.getInfo((CivicTypes) iI).getCivicOptionType()) != iI)
				{
					return true;
				}
			}
		}
	}
	else
	{
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); ++iI)
		{
			if (GC.getGame().isForceCivicOption((CivicOptionTypes)iI))
			{
				if (!GC.getGame().isForceCivic(paeNewCivics[iI]))
				{
					return false;
				}
			}

			if (getCivics((CivicOptionTypes)iI) != paeNewCivics[iI])
			{
				return true;
			}
		}
	}

	return false;
}


void CvPlayer::revolution(CivicTypes* paeNewCivics, bool bForce)
{
	if (!bForce && !canRevolution(paeNewCivics))
		return;

	int iAnarchyLength = getCivicAnarchyLength(paeNewCivics);
	if (iAnarchyLength > 0)
	{
		changeAnarchyTurns(iAnarchyLength);
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
			setCivics((CivicOptionTypes)iI, paeNewCivics[iI]);
	}
	else
	{
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
			setCivics((CivicOptionTypes)iI, paeNewCivics[iI]);
	}

	setRevolutionTimer(std::max(1, ((100 + getAnarchyModifier()) * GC.getDefineINT("MIN_REVOLUTION_TURNS")) / 100) + iAnarchyLength);

	if (getID() == GC.getGame().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(Popup_DIRTY_BIT, true); // to force an update of the civic chooser popup
		// <advc.004x>
		killAll(BUTTONPOPUP_CHANGECIVIC);
		if(iAnarchyLength > 0)
		{
			killAll(BUTTONPOPUP_CHOOSEPRODUCTION);
			killAll(BUTTONPOPUP_CHOOSETECH);
		}
	} // </advc.004x>
}


int CvPlayer::getCivicPercentAnger(CivicTypes eCivic, bool bIgnore) const  // advc: style changes
{
	if (GC.getInfo(eCivic).getCivicPercentAnger() == 0)
		return 0;

	CivicOptionTypes eCivicOption = (CivicOptionTypes)GC.getInfo(eCivic).getCivicOptionType();
	if (!bIgnore && getCivics(eCivicOption) == eCivic)
		return 0;

	int iCount = 0;
	int iPossibleCount = 0;
	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		CvPlayer const& kOther = GET_PLAYER((PlayerTypes)iI);
		if (!kOther.isAlive() || kOther.getTeam() == getTeam())
			continue;
		if (kOther.getCivics(eCivicOption) == eCivic)
			iCount += kOther.getNumCities();
		iPossibleCount += kOther.getNumCities();
	}

	if (iPossibleCount <= 0)
		return 0;

	return (GC.getInfo(eCivic).getCivicPercentAnger() * iCount) / iPossibleCount;
}


bool CvPlayer::canDoReligion(ReligionTypes eReligion) const
{
	/*if (GET_TEAM(getTeam()).getHasReligionCount(eReligion) == 0)
		return false;
	return true;*/
	// advc.132c: Replacing the above
	return (getHasReligionCount(eReligion) != 0);
}


bool CvPlayer::canChangeReligion() const
{
	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (canConvert((ReligionTypes)iI))
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::canConvert(ReligionTypes eReligion) const
{
	if (isBarbarian())
	{
		return false;
	}

	if (isAnarchy())
	{
		return false;
	}

	if (getConversionTimer() > 0)
	{
		return false;
	}

	if (!isStateReligion())
	{
		return false;
	}

	if (getLastStateReligion() == eReligion)
	{
		return false;
	}

	if (eReligion != NO_RELIGION)
	{
		if (!canDoReligion(eReligion))
		{
			return false;
		}
	}

	// davidlallen religion forbidden to civilization start
	if (eReligion != NO_RELIGION)
	{
		if (GC.getCivilizationInfo(getCivilizationType()).isForbidden(eReligion))
		{
			return false;
		}
	}
	// davidlallen religion forbidden to civilization end
	return true;
}


void CvPlayer::convert(ReligionTypes eReligion, /* <advc.001v> */ bool bForce)
{
	if(!bForce && /* </advc.001v> */ !canConvert(eReligion))
		return;

	int iAnarchyLength = getReligionAnarchyLength();

	changeAnarchyTurns(iAnarchyLength);

	setLastStateReligion(eReligion);

	setConversionTimer(std::max(1, ((100 + getAnarchyModifier()) * GC.getDefineINT("MIN_CONVERSION_TURNS")) / 100) + iAnarchyLength);
	// <advc.004x>
	if(getID() == GC.getGame().getActivePlayer())
	{
		killAll(BUTTONPOPUP_CHANGERELIGION);
		if(iAnarchyLength > 0) {
			killAll(BUTTONPOPUP_CHOOSEPRODUCTION);
			killAll(BUTTONPOPUP_CHOOSETECH);
		}
	} // </advc.004x>
}


bool CvPlayer::hasHolyCity(ReligionTypes eReligion) const
{
	CvCity* pHolyCity = GC.getGame().getHolyCity(eReligion);
	if (pHolyCity != NULL)
		return (pHolyCity->getOwner() == getID());
	return false;
}


int CvPlayer::countHolyCities() const
{
	int iCount = 0;
	FOR_EACH_ENUM(Religion)
	{
		if (hasHolyCity(eLoopReligion))
			iCount++;
	}
	return iCount;
}


void CvPlayer::foundReligion(ReligionTypes eReligion, ReligionTypes eSlotReligion, bool bAward)  // advc: some style changes
{
	if (eReligion == NO_RELIGION)
		return;

	CvReligionInfo const& kSlotReligion = GC.getInfo(eSlotReligion);
	CvGame& g = GC.getGame();
	if (g.isReligionFounded(eReligion))
	{
		if (isHuman())
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_FOUND_RELIGION, eSlotReligion);
			if (pInfo != NULL)
				gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
		}
		else foundReligion(AI().AI_chooseReligion(), eSlotReligion, bAward);
		return;
	}
	g.setReligionSlotTaken(eSlotReligion, true);

	bool bStarting = (kSlotReligion.getTechPrereq() == NO_TECH ||
			GC.getInfo((TechTypes)kSlotReligion.getTechPrereq()).getEra() < g.getStartEra());

	int iBestValue = 0;
	CvCity* pBestCity = NULL;
	FOR_EACH_CITY_VAR(pLoopCity, *this)
	{
		if (bStarting && pLoopCity->isHolyCity())
			continue;

		int iValue = 10;
		iValue += pLoopCity->getPopulation();
		iValue += g.getSorenRandNum(GC.getDefineINT("FOUND_RELIGION_CITY_RAND"), "Found Religion");
		iValue /= (pLoopCity->getReligionCount() + 1);
		if (pLoopCity->isCapital())
			iValue /= 8;
		iValue = std::max(1, iValue);

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			pBestCity = pLoopCity;
		}
	}

	if (pBestCity == NULL)
		return;

	g.setHolyCity(eReligion, pBestCity, true);
	if (bAward && kSlotReligion.getNumFreeUnits() > 0)
	{
		UnitTypes eFreeUnit = getCivilization().getUnit((UnitClassTypes)
				GC.getInfo(eReligion).getFreeUnitClass());
		if (eFreeUnit != NO_UNIT)
		{
			for (int i = 0; i < kSlotReligion.getNumFreeUnits(); i++)
				initUnit(eFreeUnit, pBestCity->getX(), pBestCity->getY());
		}
	}
}


bool CvPlayer::hasHeadquarters(CorporationTypes eCorporation) const
{
	CvCity* pHeadquarters = GC.getGame().getHeadquarters(eCorporation);

	FAssert(eCorporation != NO_CORPORATION);

	if (pHeadquarters != NULL)
	{
		return (pHeadquarters->getOwner() == getID());
	}

	return false;
}


int CvPlayer::countHeadquarters() const
{
	int iCount = 0;
	FOR_EACH_ENUM(Corporation)
	{
		if (hasHeadquarters(eLoopCorporation))
			iCount++;
	}
	return iCount;
}


int CvPlayer::countCorporations(CorporationTypes eCorporation,
	CvArea const* pArea) const // K-Mod
{
	int iCount = 0;
	FOR_EACH_CITY(pLoopCity, *this)
	{
		if (pArea == NULL || pLoopCity->isArea(*pArea)) // K-Mod
		{
			if (pLoopCity->isHasCorporation(eCorporation))
				iCount++;
		}
	}
	return iCount;
}


void CvPlayer::foundCorporation(CorporationTypes eCorporation)
{
	if (GC.getGame().isCorporationFounded(eCorporation))
		return;

	bool bStarting = ((GC.getInfo(eCorporation).getTechPrereq() == NO_TECH) || (GC.getInfo((TechTypes) GC.getInfo(eCorporation).getTechPrereq()).getEra() < GC.getGame().getStartEra()));

	int iBestValue = 0;
	CvCity* pBestCity = NULL;
	FOR_EACH_CITY_VAR(pLoopCity, *this)
	{
		if (!bStarting || !(pLoopCity->isHeadquarters()))
		{
			int iValue = 10;
			iValue += pLoopCity->getPopulation();

			for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
			{
				if (NO_BONUS != GC.getInfo(eCorporation).getPrereqBonus(i))
				{
					iValue += 10 * pLoopCity->getNumBonuses((BonusTypes)
							GC.getInfo(eCorporation).getPrereqBonus(i));
				}
			}

			iValue += GC.getGame().getSorenRandNum(GC.getDefineINT("FOUND_CORPORATION_CITY_RAND"), "Found Corporation");
			iValue /= (pLoopCity->getCorporationCount() + 1);
			iValue = std::max(1, iValue);
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestCity = pLoopCity;
			}
		}
	}

	if (pBestCity != NULL)
		pBestCity->setHeadquarters(eCorporation);
}


int CvPlayer::getCivicAnarchyLength(CivicTypes* paeNewCivics, /* advc.132: */ bool bIgnoreGoldenAge) const
{

	if(getMaxAnarchyTurns() == 0)
		return 0;

	if(/* <advc.132> */ !bIgnoreGoldenAge && /* </advc.132> */ isGoldenAge())
		return 0;

	int iAnarchyLength = 0;

	bool bChange = false;

	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (paeNewCivics[iI] != getCivics((CivicOptionTypes)iI))
		{
			iAnarchyLength += GC.getInfo(paeNewCivics[iI]).getAnarchyLength();

			bChange = true;
		}
	}

	if (bChange)
	{
		static int const iBASE_CIVIC_ANARCHY_LENGTH = GC.getDefineINT("BASE_CIVIC_ANARCHY_LENGTH"); // advc.opt
		iAnarchyLength += iBASE_CIVIC_ANARCHY_LENGTH;
		iAnarchyLength += ((getNumCities() * GC.getInfo(GC.getMap().
				getWorldSize()).getNumCitiesAnarchyPercent()) / 100);
	}

	iAnarchyLength = ((iAnarchyLength * std::max(0, getAnarchyModifier() + 100)) / 100);

	if(iAnarchyLength == 0)
		return 0;

	iAnarchyLength *= GC.getInfo(GC.getGame().getGameSpeedType()).
			getAnarchyPercent();
	iAnarchyLength /= 100;

	iAnarchyLength *= GC.getInfo(GC.getGame().getStartEra()).
			getAnarchyPercent();
	iAnarchyLength /= 100;

	return range(iAnarchyLength, 1, getMaxAnarchyTurns());
}


int CvPlayer::getReligionAnarchyLength(/* advc.132: */ bool ignoreGoldenAge) const
{
	if(getMaxAnarchyTurns() == 0)
		return 0;

	if(/* <advc.132> */ !ignoreGoldenAge && /* </advc.132> */ isGoldenAge())
		return 0;

	static int const iBASE_RELIGION_ANARCHY_LENGTH = GC.getDefineINT("BASE_RELIGION_ANARCHY_LENGTH"); // advc.opt
	int iAnarchyLength = iBASE_RELIGION_ANARCHY_LENGTH;
	iAnarchyLength += ((getNumCities() * GC.getInfo(GC.getMap().
			getWorldSize()).getNumCitiesAnarchyPercent()) / 100);

	iAnarchyLength = ((iAnarchyLength * std::max(0, getAnarchyModifier() + 100)) / 100);

	if(iAnarchyLength == 0)
		return 0;

	iAnarchyLength *= GC.getInfo(GC.getGame().getGameSpeedType()).getAnarchyPercent();
	iAnarchyLength /= 100;

	iAnarchyLength *= GC.getInfo(GC.getGame().getStartEra()).
			getAnarchyPercent();
	iAnarchyLength /= 100;

	return range(iAnarchyLength, 1, getMaxAnarchyTurns());
}



int CvPlayer::unitsRequiredForGoldenAge() const
{
	static int const iBASE_GOLDEN_AGE_UNITS = GC.getDefineINT("BASE_GOLDEN_AGE_UNITS"); // advc.opt
	static int const iGOLDEN_AGE_UNITS_MULTIPLIER = GC.getDefineINT("GOLDEN_AGE_UNITS_MULTIPLIER"); // advc.opt
	return (iBASE_GOLDEN_AGE_UNITS + getNumUnitGoldenAges() * iGOLDEN_AGE_UNITS_MULTIPLIER);
}


int CvPlayer::unitsGoldenAgeCapable() const
{
	int iCount = 0;
	FOR_EACH_UNIT(pLoopUnit, *this)
	{
		if (pLoopUnit->isGoldenAge())
			iCount++;
	}
	return iCount;
}

// Rewritten for K-Mod. (The only functionality difference is that unit class is now used rather unit type. But this version is far more efficient.)
int CvPlayer::unitsGoldenAgeReady() const
{
	PROFILE_FUNC();

	std::set<UnitClassTypes> golden_age_units;
	FOR_EACH_UNIT(pLoopUnit, *this)
	{
		if (pLoopUnit->isGoldenAge() && golden_age_units.count(pLoopUnit->getUnitClassType()) == 0)
			golden_age_units.insert(pLoopUnit->getUnitClassType());
	}
	return golden_age_units.size();
}


void CvPlayer::killGoldenAgeUnits(CvUnit* pUnitAlive)
{
	bool* pabUnitUsed = new bool[GC.getNumUnitInfos()];
	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		pabUnitUsed[iI] = false;
	}

	int iUnitsRequired = unitsRequiredForGoldenAge();

	if (pUnitAlive != NULL)
	{
		pabUnitUsed[pUnitAlive->getUnitType()] = true;
		iUnitsRequired--;
	}

	for (int iI = 0; iI < iUnitsRequired; iI++)
	{
		int iBestValue = 0;
		CvUnit* pBestUnit = NULL;
		FOR_EACH_UNIT_VAR(pLoopUnit, *this)
		{
			if (!pLoopUnit->isGoldenAge())
				continue;
			if (!pabUnitUsed[pLoopUnit->getUnitType()])
			{
				int iValue = 10000;

				iValue /= plotDistance(pLoopUnit->plot(), pUnitAlive->plot()) + 1;

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestUnit = pLoopUnit;
				}
			}
		}

		FAssert(pBestUnit != NULL);
		if (pBestUnit != NULL)
		{
			pabUnitUsed[pBestUnit->getUnitType()] = true;

			pBestUnit->kill(true);

			//play animations
			if (pBestUnit->getPlot().isActiveVisible(false))
			{
				//kill removes bestUnit from any groups
				pBestUnit->getGroup()->pushMission(MISSION_GOLDEN_AGE, 0);
			}
		}
	}

	SAFE_DELETE_ARRAY(pabUnitUsed);
}


int CvPlayer::greatPeopleThreshold(bool bMilitary) const
{
	int iThreshold;

	if (bMilitary)
	{
		static int const iGREAT_GENERALS_THRESHOLD = GC.getDefineINT("GREAT_GENERALS_THRESHOLD"); // advc.opt
		iThreshold = ((iGREAT_GENERALS_THRESHOLD *
				std::max(0, getGreatGeneralsThresholdModifier() + 100)) / 100);
	}
	else
	{
		static int const iGREAT_PEOPLE_THRESHOLD = GC.getDefineINT("GREAT_PEOPLE_THRESHOLD"); // advc.opt
		iThreshold = ((iGREAT_PEOPLE_THRESHOLD *
				std::max(0, getGreatPeopleThresholdModifier() + 100)) / 100);
	}
	CvGame const& kGame = GC.getGame(); // advc
	iThreshold *= GC.getInfo(kGame.getGameSpeedType()).getGreatPeoplePercent();
	if (bMilitary)
		iThreshold /= std::max(1, GC.getInfo(kGame.getGameSpeedType()).getTrainPercent());
	else iThreshold /= 100;

	iThreshold *= GC.getInfo(kGame.getStartEra()).getGreatPeoplePercent();
	iThreshold /= 100;
	// <advc.251>
	iThreshold = ::roundToMultiple(0.01 * iThreshold * GC.getInfo(
			getHandicapType()).getGPThresholdPercent(),
			isHuman() ? 5 : 1);
	if (!isHuman() && !isBarbarian())
	{
		iThreshold = ::round(0.01 * iThreshold * GC.getInfo(
				kGame.getHandicapType()).getAIGPThresholdPercent());
	} // </advc.251>

	return std::max(1, iThreshold);
}


int CvPlayer::specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	return (GC.getInfo(eSpecialist).getYieldChange(eYield) + getSpecialistExtraYield(eSpecialist, eYield));
}


int CvPlayer::specialistCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const
{
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**																								**/
	/**																								**/
	/*************************************************************************************************/
//<Original Code>	
	//return (GC.getInfo(eSpecialist).getCommerceChange(eCommerce) + getSpecialistExtraCommerce(eCommerce));
	//return (GC.getSpecialistInfo(eSpecialist).getCommerceChange(eCommerce) + getSpecialistExtraCommerce(eCommerce));
		
	return (GC.getInfo(eSpecialist).getCommerceChange(eCommerce) + getSpecialistExtraCommerce(eCommerce) + getSpecialistCivicExtraCommerce (eSpecialist, eCommerce));
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/
}


CvPlot* CvPlayer::getStartingPlot() const
{
	return GC.getMap().plotSoren(m_iStartingX, m_iStartingY);
}


void CvPlayer::setStartingPlot(CvPlot* pNewValue, bool bUpdateStartDist)
{
	CvPlot* pOldStartingPlot = getStartingPlot();
	if(pOldStartingPlot == pNewValue)
		return; // advc

	if (pOldStartingPlot != NULL)
	{
		pOldStartingPlot->getArea().changeNumStartingPlots(-1);
		if (bUpdateStartDist)
			GC.getMap().updateMinOriginalStartDist(pOldStartingPlot->getArea());
	}

	if (pNewValue == NULL)
	{
		m_iStartingX = INVALID_PLOT_COORD;
		m_iStartingY = INVALID_PLOT_COORD;
	}
	else
	{
		int iX = pNewValue->getX();
		int iY = pNewValue->getY();
		// <advc.031c>
		if (gFoundLogLevel > 0 && !GC.getInitCore().isScenario() &&
			m_iStartingX == INVALID_PLOT_COORD)
		{
			AI().logFoundValue(iX, iY, true);
		} // </advc.031c>
		m_iStartingX = iX;
		m_iStartingY = iY;

		getStartingPlot()->getArea().changeNumStartingPlots(1);
		if (bUpdateStartDist)
			GC.getMap().updateMinOriginalStartDist(getStartingPlot()->getArea());
	}
	FAssert(pNewValue == NULL || !pNewValue->isWater()); // advc.021b
}

// < Civic Infos Plus Start >

int CvPlayer::getBuildingYieldChange(BuildingTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiBuildingYieldChange[eIndex1][eIndex2];
}


void CvPlayer::changeBuildingYieldChange(BuildingTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ppaaiBuildingYieldChange[eIndex1][eIndex2] = (m_ppaaiBuildingYieldChange[eIndex1][eIndex2] + iChange);
		FAssert(getBuildingYieldChange(eIndex1, eIndex2) >= 0);
	}
}


int CvPlayer::getStateReligionYieldRateModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiStateReligionYieldRateModifier[eIndex];
}


void CvPlayer::changeStateReligionYieldRateModifier(YieldTypes eIndex, int iChange)
{
	CvCity* pLoopCity;
	int iLoop;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiStateReligionYieldRateModifier[eIndex] = (m_aiStateReligionYieldRateModifier[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);

		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->changeStateReligionYieldRateModifier(eIndex, iChange);
		}
	}
}

int CvPlayer::getStateReligionCommerceRateModifier(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiStateReligionCommerceRateModifier[eIndex];
}


void CvPlayer::changeStateReligionCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	CvCity* pLoopCity;
	int iLoop;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiStateReligionCommerceRateModifier[eIndex] = (m_aiStateReligionCommerceRateModifier[eIndex] + iChange);
		FAssert(getStateReligionCommerceRateModifier(eIndex) >= 0);

		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->changeStateReligionCommerceRateModifier(eIndex, iChange);
		}
	}
}

int CvPlayer::getNonStateReligionYieldRateModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiNonStateReligionYieldRateModifier[eIndex];
}


void CvPlayer::changeNonStateReligionYieldRateModifier(YieldTypes eIndex, int iChange)
{
	CvCity* pLoopCity;
	int iLoop;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiNonStateReligionYieldRateModifier[eIndex] = (m_aiNonStateReligionYieldRateModifier[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);

		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->changeNonStateReligionYieldRateModifier(eIndex, iChange);
		}
	}
}


int CvPlayer::getNonStateReligionCommerceRateModifier(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiNonStateReligionCommerceRateModifier[eIndex];
}


void CvPlayer::changeNonStateReligionCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	CvCity* pLoopCity;
	int iLoop;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiNonStateReligionCommerceRateModifier[eIndex] = (m_aiNonStateReligionCommerceRateModifier[eIndex] + iChange);
		FAssert(getNonStateReligionCommerceRateModifier(eIndex) >= 0);

		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->changeNonStateReligionCommerceRateModifier(eIndex, iChange);
		}
	}
}


int CvPlayer::getBuildingCommerceChange(BuildingTypes eIndex1, CommerceTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_COMMERCE_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiBuildingCommerceChange[eIndex1][eIndex2];
}


void CvPlayer::changeBuildingCommerceChange(BuildingTypes eIndex1, CommerceTypes eIndex2, int iChange)
{
//	CvCity* pLoopCity;
//	int iLoop;

	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_COMMERCE_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ppaaiBuildingCommerceChange[eIndex1][eIndex2] = (m_ppaaiBuildingCommerceChange[eIndex1][eIndex2] + iChange);
		FAssert(getBuildingCommerceChange(eIndex1, eIndex2) >= 0);
	}
}
// < Civic Infos Plus End   >
int CvPlayer::getTotalPopulation() const
{
	return m_iTotalPopulation;
}


int CvPlayer::getAveragePopulation() const
{
	if (getNumCities() == 0)
	{
		return 0;
	}

	//return ((getTotalPopulation() / getNumCities()) + 1);
	// advc.131: The above is 100% off at the start of a game
	return ::round(getTotalPopulation() / getNumCities());
}


void CvPlayer::changeTotalPopulation(int iChange)
{
	// <advc>
	if (iChange == 0)
		return; // <advc>
	CvGame const& kGame = GC.getGame();
	changeAssets(-kGame.getPopulationAsset(getTotalPopulation()));
	changePower(-kGame.getPopulationPower(getTotalPopulation()));
	changePopScore(-kGame.getPopulationScore(getTotalPopulation()));

	m_iTotalPopulation += iChange;
	FAssert(getTotalPopulation() >= 0);

	changeAssets(kGame.getPopulationAsset(getTotalPopulation()));
	changePower(kGame.getPopulationPower(getTotalPopulation()));
	changePopScore(kGame.getPopulationScore(getTotalPopulation()));
}


long CvPlayer::getRealPopulation() const
{
	long long iTotalPopulation = 0;
	FOR_EACH_CITY(pLoopCity, *this)
		iTotalPopulation += pLoopCity->getRealPopulation();
	return ::longLongToInt(iTotalPopulation); // advc
}


int CvPlayer::getTotalLand() const
{
	return m_iTotalLand;
}


void CvPlayer::changeTotalLand(int iChange)
{
	m_iTotalLand = (m_iTotalLand + iChange);
	FAssert(getTotalLand() >= 0);
}


int CvPlayer::getTotalLandScored() const
{
	return m_iTotalLandScored;
}


void CvPlayer::changeTotalLandScored(int iChange)
{
	if (iChange == 0)
		return; // advc

	CvGame const& kGame = GC.getGame();
	changeAssets(-kGame.getLandPlotsAsset(getTotalLandScored()));
	changeLandScore(-kGame.getLandPlotsScore(getTotalLandScored()));

	m_iTotalLandScored += iChange;
	FAssert(getTotalLandScored() >= 0);

	changeAssets(kGame.getLandPlotsAsset(getTotalLandScored()));
	changeLandScore(kGame.getLandPlotsScore(getTotalLandScored()));
}


int CvPlayer::getGold() const
{
	return m_iGold;
}


void CvPlayer::setGold(int iNewValue)
{
	if (getGold() != iNewValue)
	{
		m_iGold = iNewValue;
		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


void CvPlayer::changeGold(int iChange)
{
	setGold(getGold() + iChange);
}


int CvPlayer::getGoldPerTurn() const
{
	return m_iGoldPerTurn;
}


int CvPlayer::getAdvancedStartPoints() const
{
	return m_iAdvancedStartPoints;
}


void CvPlayer::setAdvancedStartPoints(int iNewValue)
{
	if (getAdvancedStartPoints() != iNewValue)
	{
		m_iAdvancedStartPoints = iNewValue;

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

void CvPlayer::changeAdvancedStartPoints(int iChange)
{
	setAdvancedStartPoints(getAdvancedStartPoints() + iChange);
}


int CvPlayer::getGoldenAgeTurns() const
{
	return m_iGoldenAgeTurns;
}


bool CvPlayer::isGoldenAge() const
{
	return (getGoldenAgeTurns() > 0);
}


void CvPlayer::changeGoldenAgeTurns(int iChange)
{
	if(iChange == 0)
		return;

	CvWString szBuffer;

	bool const bOldGoldenAge = isGoldenAge();
	m_iGoldenAgeTurns += iChange;
	FAssert(getGoldenAgeTurns() >= 0);

	if (bOldGoldenAge != isGoldenAge())
	{
		if (isGoldenAge())
		{
			changeAnarchyTurns(-getAnarchyTurns());
			// K-Mod. Allow the AI to reconsider their civics. (a golden age is a good time for reform!)
			if (!isHuman() && getMaxAnarchyTurns() != 0 && getAnarchyModifier() + 100 > 0)
				AI().AI_setCivicTimer(0); // K-Mod end
		}

		updateYield();

		if (isGoldenAge())
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_GOLDEN_AGE_BEGINS", getNameKey());
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer,
					-1, -1, GC.getColorType("HIGHLIGHT_TEXT"));

			CvEventReporter::getInstance().goldenAge(getID());
		}
		else
		{
			CvEventReporter::getInstance().endGoldenAge(getID());
		}

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayer const& kObs = GET_PLAYER((PlayerTypes)iI); // advc
			if (!kObs.isAlive())
				continue; // advc

			if (GET_TEAM(getTeam()).isHasMet(kObs.getTeam()) /* advc.127: */ || kObs.isSpectator())
			{
				if (isGoldenAge())
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_GOLDEN_AGE_HAS_BEGUN", getNameKey());
					gDLL->UI().addMessage(kObs.getID(), kObs.getID() == getID(), -1,
							szBuffer, "AS2D_GOLDAGESTART",
							MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY, // advc.106b
							NULL, GC.getColorType("HIGHLIGHT_TEXT"),
							// advc.127b:
							getCapitalX(kObs.getTeam(), true), getCapitalY(kObs.getTeam(), true));
				}
				else
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_GOLDEN_AGE_ENDED", getNameKey());
					gDLL->UI().addMessage(kObs.getID(), false, -1, szBuffer, "AS2D_GOLDAGEEND",
							MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getColorType("HIGHLIGHT_TEXT"),
							// advc.127b:
							getCapitalX(kObs.getTeam(), true), getCapitalY(kObs.getTeam(), true));
				}
			}
		}
	}

	if (getID() == GC.getGame().getActivePlayer())
		gDLL->UI().setDirty(GameData_DIRTY_BIT, true);
}
// advc.001x:
void CvPlayer::startGoldenAgeDelayed()
{
	m_iScheduledGoldenAges++;
}

int CvPlayer::getGoldenAgeLength() const
{
	return (GC.getGame().goldenAgeLength() * std::max(0, 100 + getGoldenAgeModifier())) / 100;
}

int CvPlayer::getNumUnitGoldenAges() const
{
	return m_iNumUnitGoldenAges;
}


void CvPlayer::changeNumUnitGoldenAges(int iChange)
{
	m_iNumUnitGoldenAges = (m_iNumUnitGoldenAges + iChange);
	FAssert(getNumUnitGoldenAges() >= 0);
}


int CvPlayer::getStrikeTurns() const
{
	return m_iStrikeTurns;
}


void CvPlayer::changeStrikeTurns(int iChange)
{
	m_iStrikeTurns = (m_iStrikeTurns + iChange);
	FAssert(getStrikeTurns() >= 0);
}


int CvPlayer::getAnarchyTurns() const
{
	return m_iAnarchyTurns;
}


bool CvPlayer::isAnarchy() const
{
	return (getAnarchyTurns() > 0);
}


void CvPlayer::changeAnarchyTurns(int iChange) // advc: Refactored
{
	if(iChange == 0)
		return;

	CvGame const& g = GC.getGame();
	if(getID() == g.getActivePlayer())
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);

	bool bOldAnarchy = isAnarchy();
	m_iAnarchyTurns += iChange;
	FAssert(getAnarchyTurns() >= 0);
	if(bOldAnarchy == isAnarchy())
		return;

	if(getID() == g.getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
		// advc.004x:
		gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
	}
	if(getTeam() == g.getActiveTeam())
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);

	updateCommerce();
	updateMaintenance();
	updateTradeRoutes();
	updateCorporation();
	GC.getGame().updateTradeRoutes(); // advc.124
	AI_makeAssignWorkDirty();

	if (isAnarchy())
	{
		gDLL->UI().addMessage(getID(), true, -1,
				gDLL->getText("TXT_KEY_MISC_REVOLUTION_HAS_BEGUN").GetCString(), "AS2D_REVOLTSTART",
				MESSAGE_TYPE_MINOR_EVENT, // advc.106b: was MAJOR
				NULL, GC.getColorType("WARNING_TEXT"),
				getCapitalX(getTeam()), getCapitalY(getTeam())); // advc.127b
	}
	else
	{
		gDLL->UI().addMessage(getID(), false, -1,
				gDLL->getText("TXT_KEY_MISC_REVOLUTION_OVER").GetCString(), "AS2D_REVOLTEND",
				MESSAGE_TYPE_MINOR_EVENT, NULL, NO_COLOR, // advc.004g: Was COLOR_WARNING_TEXT
				getCapitalX(getTeam()), getCapitalY(getTeam())); // advc.127b
		// K-Mod. trigger production/research popups that have been suppressed.
		if (isHuman())
		{
			if (isResearch() && getCurrentResearch() == NO_TECH)
				chooseTech();
			FOR_EACH_CITYAI_VAR(pLoopCity, AI())
			{
				if (pLoopCity->isChooseProductionDirty() &&
					!pLoopCity->isProduction() && !pLoopCity->isDisorder() &&
					!pLoopCity->isProductionAutomated())
				{
					pLoopCity->chooseProduction();
				}
			}
		} // K-Mod end
	}
}


int CvPlayer::getMaxAnarchyTurns() const
{
	return m_iMaxAnarchyTurns;
}


void CvPlayer::updateMaxAnarchyTurns()
{
	int iBestValue = GC.getDefineINT("MAX_ANARCHY_TURNS");
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			if (GC.getInfo((TraitTypes)iI).getMaxAnarchy() >= 0)
			{
				if (GC.getInfo((TraitTypes)iI).getMaxAnarchy() < iBestValue)
				{
					iBestValue = GC.getInfo((TraitTypes)iI).getMaxAnarchy();
				}
			}
		}
	}
	m_iMaxAnarchyTurns = iBestValue;
	FAssert(getMaxAnarchyTurns() >= 0);
}


int CvPlayer::getAnarchyModifier() const
{
	return m_iAnarchyModifier;
}


void CvPlayer::changeAnarchyModifier(int iChange)
{
	if (iChange == 0)
		return;

	/*setRevolutionTimer(std::max(0, ((100 + iChange) * getRevolutionTimer()) / 100));
	setConversionTimer(std::max(0, ((100 + iChange) * getConversionTimer()) / 100));*/ // BtS
	// K-Mod. The original code is wrong, and it is missing the anarchy length change.
	changeRevolutionTimer(getRevolutionTimer() * iChange / std::max(1, 100+getAnarchyModifier()));
	changeConversionTimer(getConversionTimer() * iChange / std::max(1, 100+getAnarchyModifier()));
	changeAnarchyTurns(getAnarchyTurns() * iChange / std::max(1, 100+getAnarchyModifier()));
	// K-Mod end

	m_iAnarchyModifier += iChange;
}


int CvPlayer::getGoldenAgeModifier() const
{
	return m_iGoldenAgeModifier;
}


void CvPlayer::changeGoldenAgeModifier(int iChange)
{	// <advc>
	if (iChange == 0)
		return; // </advc>
	// K-Mod. If we are currently in a golden age, adjust its duration!
	changeGoldenAgeTurns(getGoldenAgeTurns() * iChange / std::max(1, 100 + getGoldenAgeModifier()));

	m_iGoldenAgeModifier += iChange;
}


int CvPlayer::getHurryModifier() const
{
	return m_iGlobalHurryModifier;
}


void CvPlayer::changeHurryModifier(int iChange)
{
	m_iGlobalHurryModifier = (m_iGlobalHurryModifier + iChange);
}


int CvPlayer::getGreatPeopleCreated() const
{
	return m_iGreatPeopleCreated;
}


void CvPlayer::incrementGreatPeopleCreated()
{
	m_iGreatPeopleCreated++;
}

int CvPlayer::getGreatGeneralsCreated() const
{
	return m_iGreatGeneralsCreated;
}

void CvPlayer::incrementGreatGeneralsCreated()
{
	m_iGreatGeneralsCreated++;
}

int CvPlayer::getGreatPeopleThresholdModifier() const
{
	return m_iGreatPeopleThresholdModifier;
}


void CvPlayer::changeGreatPeopleThresholdModifier(int iChange)
{
	m_iGreatPeopleThresholdModifier = (m_iGreatPeopleThresholdModifier + iChange);
}


int CvPlayer::getGreatGeneralsThresholdModifier() const
{
	return m_iGreatGeneralsThresholdModifier;
}


void CvPlayer::changeGreatGeneralsThresholdModifier(int iChange)
{
	m_iGreatGeneralsThresholdModifier += iChange;
}


int CvPlayer::getGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}


void CvPlayer::changeGreatPeopleRateModifier(int iChange)
{
	m_iGreatPeopleRateModifier = (m_iGreatPeopleRateModifier + iChange);
}


int CvPlayer::getGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}


void CvPlayer::changeGreatGeneralRateModifier(int iChange)
{
	m_iGreatGeneralRateModifier += iChange;
}


int CvPlayer::getDomesticGreatGeneralRateModifier() const
{
	static int const iCOMBAT_EXPERIENCE_IN_BORDERS_PERCENT = GC.getDefineINT("COMBAT_EXPERIENCE_IN_BORDERS_PERCENT"); // advc.opt
	return (iCOMBAT_EXPERIENCE_IN_BORDERS_PERCENT + m_iDomesticGreatGeneralRateModifier);
}


void CvPlayer::changeDomesticGreatGeneralRateModifier(int iChange)
{
	m_iDomesticGreatGeneralRateModifier += iChange;
}


int CvPlayer::getStateReligionGreatPeopleRateModifier() const
{
	return m_iStateReligionGreatPeopleRateModifier;
}


void CvPlayer::changeStateReligionGreatPeopleRateModifier(int iChange)
{
	m_iStateReligionGreatPeopleRateModifier = (m_iStateReligionGreatPeopleRateModifier + iChange);
}


int CvPlayer::getMaxGlobalBuildingProductionModifier() const
{
	return m_iMaxGlobalBuildingProductionModifier;
}


void CvPlayer::changeMaxGlobalBuildingProductionModifier(int iChange)
{
	m_iMaxGlobalBuildingProductionModifier = (m_iMaxGlobalBuildingProductionModifier + iChange);
}


int CvPlayer::getMaxTeamBuildingProductionModifier() const
{
	return m_iMaxTeamBuildingProductionModifier;
}


void CvPlayer::changeMaxTeamBuildingProductionModifier(int iChange)
{
	m_iMaxTeamBuildingProductionModifier = (m_iMaxTeamBuildingProductionModifier + iChange);
}


int CvPlayer::getMaxPlayerBuildingProductionModifier() const
{
	return m_iMaxPlayerBuildingProductionModifier;
}


void CvPlayer::changeMaxPlayerBuildingProductionModifier(int iChange)
{
	m_iMaxPlayerBuildingProductionModifier = (m_iMaxPlayerBuildingProductionModifier + iChange);
}


int CvPlayer::getFreeExperience() const
{
	return m_iFreeExperience;
}


void CvPlayer::changeFreeExperience(int iChange)
{
	m_iFreeExperience = (m_iFreeExperience + iChange);
}


int CvPlayer::getFeatureProductionModifier() const
{
	return m_iFeatureProductionModifier;
}


void CvPlayer::changeFeatureProductionModifier(int iChange)
{
	m_iFeatureProductionModifier = (m_iFeatureProductionModifier + iChange);
}


int CvPlayer::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}


void CvPlayer::changeWorkerSpeedModifier(int iChange)
{
	m_iWorkerSpeedModifier = (m_iWorkerSpeedModifier + iChange);
}

// <advc.011c>
int CvPlayer::getWorkRate(BuildTypes eBuild) const
{
	int iRate = 0;
	CvCivilization const& kCiv = getCivilization(); // advc.003w
	for (int i = 0; i < kCiv.getNumUnits(); i++)
	{
		UnitTypes eUnit = kCiv.unitAt(i);
		CvUnitInfo& kUnit = GC.getInfo(eUnit);
		if (kUnit.getBuilds(eBuild))
		{
			iRate = kUnit.getWorkRate();
			break;
		}
	}
	iRate *= std::max(0, 100 + getWorkerSpeedModifier());
	iRate /= 100;
	if(!isHuman() && !isBarbarian())
	{
		iRate *= std::max(0, 100 + GC.getInfo(GC.getGame().getHandicapType()).
				getAIWorkRateModifier() + 100);
		iRate /= 100;
	}
	return iRate;
} // </advc.011c>


int CvPlayer::getImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}


void CvPlayer::changeImprovementUpgradeRateModifier(int iChange)
{
	m_iImprovementUpgradeRateModifier = (m_iImprovementUpgradeRateModifier + iChange);
}


int CvPlayer::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}


void CvPlayer::changeMilitaryProductionModifier(int iChange)
{
	m_iMilitaryProductionModifier = (m_iMilitaryProductionModifier + iChange);
}


int CvPlayer::getSpaceProductionModifier() const
{
	return m_iSpaceProductionModifier;
}


void CvPlayer::changeSpaceProductionModifier(int iChange)
{
	m_iSpaceProductionModifier = (m_iSpaceProductionModifier + iChange);
}


int CvPlayer::getCityDefenseModifier() const
{
	return m_iCityDefenseModifier;
}


void CvPlayer::changeCityDefenseModifier(int iChange)
{
	m_iCityDefenseModifier = (m_iCityDefenseModifier + iChange);
}

/************************************************************************************************/
/* REVDCM                                 09/02/10                                phungus420    */
/*                                                                                              */
/* Player Functions              cantrain                                                               */
/************************************************************************************************/
bool CvPlayer::isBuildingClassRequiredToTrain(BuildingClassTypes eBuildingClass, UnitTypes eUnit) const
{
	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
	if (kUnit.isPrereqBuildingClass(eBuildingClass))
	{
		TechTypes eOverrideTech = (TechTypes) kUnit.getPrereqBuildingClassOverrideTech(eBuildingClass);
		if (eOverrideTech != NO_TECH && GET_TEAM(getTeam()).isHasTech(TechTypes(eOverrideTech)) )
		{
			return false;
		}
		EraTypes eOverrideEra = (EraTypes) kUnit.getPrereqBuildingClassOverrideEra(eBuildingClass);
		if (eOverrideEra != NO_ERA && EraTypes(getCurrentEra()) >= eOverrideEra)
		{
			return false;
		}
		return true;
	}
	return false;
}
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/

int CvPlayer::getNumNukeUnits() const
{
	return m_iNumNukeUnits;
}


void CvPlayer::changeNumNukeUnits(int iChange)
{
	m_iNumNukeUnits = (m_iNumNukeUnits + iChange);
	FAssert(getNumNukeUnits() >= 0);
}


int CvPlayer::getNumOutsideUnits() const
{
	return m_iNumOutsideUnits;
}


void CvPlayer::changeNumOutsideUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iNumOutsideUnits += iChange;
		FAssert(getNumOutsideUnits() >= 0);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getBaseFreeUnits() const
{
	return m_iBaseFreeUnits;
}


void CvPlayer::changeBaseFreeUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iBaseFreeUnits = (m_iBaseFreeUnits + iChange);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getBaseFreeMilitaryUnits() const
{
	return m_iBaseFreeMilitaryUnits;
}


void CvPlayer::changeBaseFreeMilitaryUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iBaseFreeMilitaryUnits = (m_iBaseFreeMilitaryUnits + iChange);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getFreeUnitsPopulationPercent() const
{
	return m_iFreeUnitsPopulationPercent;
}


void CvPlayer::changeFreeUnitsPopulationPercent(int iChange)
{
	if (iChange != 0)
	{
		m_iFreeUnitsPopulationPercent = (m_iFreeUnitsPopulationPercent + iChange);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getFreeMilitaryUnitsPopulationPercent() const
{
	return m_iFreeMilitaryUnitsPopulationPercent;
}


void CvPlayer::changeFreeMilitaryUnitsPopulationPercent(int iChange)
{
	if (iChange != 0)
	{
		m_iFreeMilitaryUnitsPopulationPercent = (m_iFreeMilitaryUnitsPopulationPercent + iChange);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

// K-Mod
int CvPlayer::getTypicalUnitValue(UnitAITypes eUnitAI, DomainTypes eDomain) const
{
	//UnitTypes eBestUnit = NO_UNIT;
	int iHighestValue = 0;

	CvCivilization const& kCiv = getCivilization(); // advc.003w
	for (int i = 0; i < kCiv.getNumUnits(); i++)
	{
		UnitTypes eLoopUnit = kCiv.unitAt(i);
		if ((eUnitAI == NO_UNITAI || GC.getInfo(eLoopUnit).getUnitAIType(eUnitAI)) &&
			(eDomain == NO_DOMAIN || GC.getInfo(eLoopUnit).getDomainType() == eDomain) &&
			canTrain(eLoopUnit))
		{
			// Note: currently the above checks do not consider any resource prerequites.
			int iValue = GC.AI_getGame().AI_combatValue(eLoopUnit);
			if (iValue > iHighestValue)
			{
				iHighestValue = iValue;
				//eBestUnit = eLoopUnit;
			}
		}
	}

	return iHighestValue;
}

int CvPlayer::getGoldPerUnit() const
{	// <dlph.14>
	if(isBarbarian())
		return 0; // </dlph.14>
	return m_iGoldPerUnit;
}


void CvPlayer::changeGoldPerUnit(int iChange)
{
	if (iChange != 0)
	{
		m_iGoldPerUnit = (m_iGoldPerUnit + iChange);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getGoldPerMilitaryUnit() const
{	// <dlph.14>
	if(isBarbarian())
		return 0; // </dlph.14>
	return m_iGoldPerMilitaryUnit;
}


void CvPlayer::changeGoldPerMilitaryUnit(int iChange)
{
	if (iChange != 0)
	{
		m_iGoldPerMilitaryUnit = (m_iGoldPerMilitaryUnit + iChange);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getExtraUnitCost() const
{
	return m_iExtraUnitCost;
}


void CvPlayer::changeExtraUnitCost(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraUnitCost = (m_iExtraUnitCost + iChange);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getNumMilitaryUnits() const
{
	return m_iNumMilitaryUnits;
}


void CvPlayer::changeNumMilitaryUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iNumMilitaryUnits = (m_iNumMilitaryUnits + iChange);
		FAssert(getNumMilitaryUnits() >= 0);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getHappyPerMilitaryUnit() const
{
	return m_iHappyPerMilitaryUnit;
}


void CvPlayer::changeHappyPerMilitaryUnit(int iChange)
{
	if (iChange != 0)
	{
		m_iHappyPerMilitaryUnit = (m_iHappyPerMilitaryUnit + iChange);

		AI_makeAssignWorkDirty();
	}
}

// <advc.912c>
int CvPlayer::getLuxuryModifier() const	{ return m_iLuxuryModifier; }

void CvPlayer::changeLuxuryModifier(int iChange)
{
	if(iChange == 0)
		return;
	m_iLuxuryModifier += iChange;
	m_iLuxuryModifier = ::range(m_iLuxuryModifier, -100, 1000);
	AI_makeAssignWorkDirty();
} // </advc.912c>


int CvPlayer::getMilitaryFoodProductionCount() const
{
	return m_iMilitaryFoodProductionCount;
}


bool CvPlayer::isMilitaryFoodProduction() const
{
	return (getMilitaryFoodProductionCount() > 0);
}


void CvPlayer::changeMilitaryFoodProductionCount(int iChange)
{
	if (iChange != 0)
	{
		m_iMilitaryFoodProductionCount = (m_iMilitaryFoodProductionCount + iChange);
		FAssert(getMilitaryFoodProductionCount() >= 0);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getHighestUnitLevel()	const
{
	return m_iHighestUnitLevel;
}


void CvPlayer::setHighestUnitLevel(int iNewValue)
{
	m_iHighestUnitLevel = iNewValue;
	FAssert(getHighestUnitLevel() >= 0);
}


int CvPlayer::getMaxConscript() const
{
	return m_iMaxConscript;
}


void CvPlayer::changeMaxConscript(int iChange)
{
	m_iMaxConscript = (m_iMaxConscript + iChange);
	FAssert(getMaxConscript() >= 0);
}


int CvPlayer::getConscriptCount() const
{
	return m_iConscriptCount;
}


void CvPlayer::setConscriptCount(int iNewValue)
{
	m_iConscriptCount = iNewValue;
	FAssert(getConscriptCount() >= 0);
}


void CvPlayer::changeConscriptCount(int iChange)
{
	setConscriptCount(getConscriptCount() + iChange);
}


int CvPlayer::getOverflowResearch() const
{
	return m_iOverflowResearch;
}


void CvPlayer::setOverflowResearch(int iNewValue)
{
	m_iOverflowResearch = iNewValue;
	FAssert(getOverflowResearch() >= 0);
}


void CvPlayer::changeOverflowResearch(int iChange)
{
	setOverflowResearch(getOverflowResearch() + iChange);
}


/*int CvPlayer::getNoUnhealthyPopulationCount() const {
	return m_iNoUnhealthyPopulationCount;
}
bool CvPlayer::isNoUnhealthyPopulation() const {
	return (getNoUnhealthyPopulationCount() > 0);
}
void CvPlayer::changeNoUnhealthyPopulationCount(int iChange) {
	if (iChange != 0) {
		m_iNoUnhealthyPopulationCount = (m_iNoUnhealthyPopulationCount + iChange);
		FAssert(getNoUnhealthyPopulationCount() >= 0);

		AI_makeAssignWorkDirty();
	}
}*/ // BtS
// K-Mod, 27/dec/10: replace with UnhealthyPopulationModifier
int CvPlayer::getUnhealthyPopulationModifier() const
{
	return m_iUnhealthyPopulationModifier;
}


void CvPlayer::changeUnhealthyPopulationModifier(int iChange)
{
	m_iUnhealthyPopulationModifier += iChange;
}
// K-Mod end

int CvPlayer::getExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}


void CvPlayer::changeExpInBorderModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iExpInBorderModifier += iChange;
		FAssert(getExpInBorderModifier() >= 0);
	}
}


int CvPlayer::getBuildingOnlyHealthyCount() const
{
	return m_iBuildingOnlyHealthyCount;
}


bool CvPlayer::isBuildingOnlyHealthy() const
{
	return (getBuildingOnlyHealthyCount() > 0);
}


void CvPlayer::changeBuildingOnlyHealthyCount(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingOnlyHealthyCount = (m_iBuildingOnlyHealthyCount + iChange);
		FAssert(getBuildingOnlyHealthyCount() >= 0);

		AI_makeAssignWorkDirty();
	}
}
//DPII < Maintenance Modifiers >
int CvPlayer::getMaintenanceModifier()
{
    return m_iMaintenanceModifier;
}

void CvPlayer::changeMaintenanceModifier(int iChange)
{
    if (iChange != 0)
    {
        m_iMaintenanceModifier = (m_iMaintenanceModifier + iChange);

        updateMaintenance();
    }
}

int CvPlayer::getCoastalDistanceMaintenanceModifier()
{
    return m_iCoastalDistanceMaintenanceModifier;
}

void CvPlayer::changeCoastalDistanceMaintenanceModifier(int iChange)
{
    if (iChange != 0)
    {
        m_iCoastalDistanceMaintenanceModifier = (m_iCoastalDistanceMaintenanceModifier + iChange);

        updateMaintenance();
    }
}

int CvPlayer::getConnectedCityMaintenanceModifier()
{
    return m_iConnectedCityMaintenanceModifier;
}

void CvPlayer::changeConnectedCityMaintenanceModifier(int iChange)
{
    if (iChange != 0)
    {
        m_iConnectedCityMaintenanceModifier = (m_iConnectedCityMaintenanceModifier + iChange);

        updateMaintenance();
    }
}
//DPII < Maintenance Modifiers >

int CvPlayer::getDistanceMaintenanceModifier() const
{
	return m_iDistanceMaintenanceModifier;
}


void CvPlayer::changeDistanceMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iDistanceMaintenanceModifier += iChange;

		updateMaintenance();
	}
}


int CvPlayer::getNumCitiesMaintenanceModifier() const
{
	return m_iNumCitiesMaintenanceModifier;
}


void CvPlayer::changeNumCitiesMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iNumCitiesMaintenanceModifier += iChange;

		updateMaintenance();
	}
}


int CvPlayer::getCorporationMaintenanceModifier() const
{
	return m_iCorporationMaintenanceModifier;
}


void CvPlayer::changeCorporationMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iCorporationMaintenanceModifier += iChange;

		updateMaintenance();
	}
}


int CvPlayer::getTotalMaintenance() const
{
	return m_iTotalMaintenance / 100;
}

// <advc.004b>
int CvPlayer::getTotalMaintenanceTimes100() const
{
	return m_iTotalMaintenance;
} // </advc.004b>

void CvPlayer::changeTotalMaintenance(int iChange)
{
	m_iTotalMaintenance += iChange;
	FAssert(m_iTotalMaintenance >= 0);
}


int CvPlayer::getUpkeepModifier() const
{
	return m_iUpkeepModifier;
}


void CvPlayer::changeUpkeepModifier(int iChange)
{
	m_iUpkeepModifier = (m_iUpkeepModifier + iChange);
}


int CvPlayer::getLevelExperienceModifier() const
{
	return m_iLevelExperienceModifier;
}

void CvPlayer::changeLevelExperienceModifier(int iChange)
{
	m_iLevelExperienceModifier += iChange;
}



int CvPlayer::getExtraHealth() const
{
	return m_iExtraHealth;
}


void CvPlayer::changeExtraHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHealth = (m_iExtraHealth + iChange);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getBuildingGoodHealth() const
{
	return m_iBuildingGoodHealth;
}


void CvPlayer::changeBuildingGoodHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingGoodHealth = (m_iBuildingGoodHealth + iChange);
		FAssert(getBuildingGoodHealth() >= 0);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getBuildingBadHealth() const
{
	return m_iBuildingBadHealth;
}


void CvPlayer::changeBuildingBadHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBadHealth = (m_iBuildingBadHealth + iChange);
		FAssert(getBuildingBadHealth() <= 0);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getExtraHappiness() const
{
	return m_iExtraHappiness;
}


void CvPlayer::changeExtraHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHappiness = (m_iExtraHappiness + iChange);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getBuildingHappiness() const
{
	return m_iBuildingHappiness;
}


void CvPlayer::changeBuildingHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingHappiness = (m_iBuildingHappiness + iChange);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getLargestCityHappiness() const
{
	return m_iLargestCityHappiness;
}


void CvPlayer::changeLargestCityHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iLargestCityHappiness = (m_iLargestCityHappiness + iChange);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getWarWearinessPercentAnger() const
{
	return m_iWarWearinessPercentAnger;
}


void CvPlayer::updateWarWearinessPercentAnger()
{
	int iNewWarWearinessPercentAnger = 0;
	if (!isBarbarian() && !isMinorCiv())
	{
		for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			CvTeam& kTeam = GET_TEAM((TeamTypes)iI);
			if (kTeam.isAlive() && !kTeam.isMinorCiv())
			{
				if (kTeam.isAtWar(getTeam()))
				{
					//iNewWarWearinessPercentAnger += (GET_TEAM(getTeam()).getWarWeariness((TeamTypes)iI) * std::max(0, 100 + kTeam.getEnemyWarWearinessModifier())) / 10000;
					iNewWarWearinessPercentAnger += GET_TEAM(getTeam()).getWarWeariness((TeamTypes)iI, true) / 100; // K-Mod
				}
			}
		}
	}
	iNewWarWearinessPercentAnger = getModifiedWarWearinessPercentAnger(iNewWarWearinessPercentAnger);
	if (getWarWearinessPercentAnger() != iNewWarWearinessPercentAnger)
	{
		m_iWarWearinessPercentAnger = iNewWarWearinessPercentAnger;
		AI_makeAssignWorkDirty();
	}
}

int CvPlayer::getModifiedWarWearinessPercentAnger(int iWarWearinessPercentAnger) const
{
	static int const iBASE_WAR_WEARINESS_MULTIPLIER = GC.getDefineINT("BASE_WAR_WEARINESS_MULTIPLIER"); // advc.opt
	iWarWearinessPercentAnger *= iBASE_WAR_WEARINESS_MULTIPLIER;

	if (GC.getGame().isOption(GAMEOPTION_ALWAYS_WAR) || GC.getGame().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE))
	{
		static int const iFORCED_WAR_WAR_WEARINESS_MODIFIER = GC.getDefineINT("FORCED_WAR_WAR_WEARINESS_MODIFIER"); // advc.opt
		iWarWearinessPercentAnger *= std::max(0, iFORCED_WAR_WAR_WEARINESS_MODIFIER + 100);
		iWarWearinessPercentAnger /= 100;
	}

	if (GC.getGame().isGameMultiPlayer())
	{
		static int const iMULTIPLAYER_WAR_WEARINESS_MODIFIER = GC.getDefineINT("MULTIPLAYER_WAR_WEARINESS_MODIFIER"); // advc.opt
		iWarWearinessPercentAnger *= std::max(0, iMULTIPLAYER_WAR_WEARINESS_MODIFIER + 100);
		iWarWearinessPercentAnger /= 100;
	}

	iWarWearinessPercentAnger *= std::max(0, (GC.getInfo(GC.getMap().getWorldSize()).getWarWearinessModifier() + 100));
	iWarWearinessPercentAnger /= 100;

	if (!isHuman() && !isBarbarian() && !isMinorCiv())
	{
		iWarWearinessPercentAnger *= GC.getInfo(GC.getGame().getHandicapType()).getAIWarWearinessPercent();
		iWarWearinessPercentAnger /= 100;
		// advc.251: No longer adjusted to handicap
		/*iWarWearinessPercentAnger *= std::max(0, ((GC.getInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iWarWearinessPercentAnger /= 100;*/
	}

	return iWarWearinessPercentAnger;
}


int CvPlayer::getWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}


void CvPlayer::changeWarWearinessModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iWarWearinessModifier = (m_iWarWearinessModifier + iChange);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getFreeSpecialist() const
{
	return m_iFreeSpecialist;
}


void CvPlayer::changeFreeSpecialist(int iChange)
{
	if (iChange != 0)
	{
		m_iFreeSpecialist = (m_iFreeSpecialist + iChange);
		FAssert(getFreeSpecialist() >= 0);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getNoForeignTradeCount() const
{
	return m_iNoForeignTradeCount;
}


bool CvPlayer::isNoForeignTrade() const
{
	return (getNoForeignTradeCount() > 0);
}


void CvPlayer::changeNoForeignTradeCount(int iChange)
{
	if (iChange == 0)
		return;
	m_iNoForeignTradeCount += iChange;
	FAssert(getNoForeignTradeCount() >= 0);
	if(!isAnarchy()) // advc.124: Update them when anarchy ends
		GC.getGame().updateTradeRoutes();
}


int CvPlayer::getNoCorporationsCount() const
{
	return m_iNoCorporationsCount;
}


bool CvPlayer::isNoCorporations() const
{
	return (getNoCorporationsCount() > 0);
}


void CvPlayer::changeNoCorporationsCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoCorporationsCount += iChange;
		FAssert(getNoCorporationsCount() >= 0);

		updateCorporation();
	}
}


int CvPlayer::getNoForeignCorporationsCount() const
{
	return m_iNoForeignCorporationsCount;
}


bool CvPlayer::isNoForeignCorporations() const
{
	return (getNoForeignCorporationsCount() > 0);
}


void CvPlayer::changeNoForeignCorporationsCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoForeignCorporationsCount += iChange;
		FAssert(getNoForeignCorporationsCount() >= 0);

		updateCorporation();
	}
}


int CvPlayer::getCoastalTradeRoutes() const
{
	return m_iCoastalTradeRoutes;
}


void CvPlayer::changeCoastalTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iCoastalTradeRoutes = (m_iCoastalTradeRoutes + iChange);
		FAssert(getCoastalTradeRoutes() >= 0);

		updateTradeRoutes();
	}
}


int CvPlayer::getTradeRoutes() const
{
	return m_iTradeRoutes;
}


void CvPlayer::changeTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iTradeRoutes = (m_iTradeRoutes + iChange);
		FAssert(getTradeRoutes() >= 0);

		updateTradeRoutes();
	}
}


int CvPlayer::getRevolutionTimer() const
{
	return m_iRevolutionTimer;
}


void CvPlayer::setRevolutionTimer(int iNewValue)
{
	if (getRevolutionTimer() != iNewValue)
	{
		m_iRevolutionTimer = iNewValue;
		FAssert(getRevolutionTimer() >= 0);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
		}
	}
}


void CvPlayer::changeRevolutionTimer(int iChange)
{
	setRevolutionTimer(getRevolutionTimer() + iChange);
}


int CvPlayer::getConversionTimer() const
{
	return m_iConversionTimer;
}


void CvPlayer::setConversionTimer(int iNewValue)
{
	if (getConversionTimer() != iNewValue)
	{
		m_iConversionTimer = iNewValue;
		FAssert(getConversionTimer() >= 0);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
		}
	}
}


void CvPlayer::changeConversionTimer(int iChange)
{
	setConversionTimer(getConversionTimer() + iChange);
}


int CvPlayer::getStateReligionCount() const
{
	return m_iStateReligionCount;
}


bool CvPlayer::isStateReligion() const
{
	return (getStateReligionCount() > 0);
}


void CvPlayer::changeStateReligionCount(int iChange)
{
	if (iChange != 0)
	{
		// religion visibility now part of espionage
		//GC.getGame().updateCitySight(false, true);

		m_iStateReligionCount = (m_iStateReligionCount + iChange);
		FAssert(getStateReligionCount() >= 0);

		// religion visibility now part of espionage
		//GC.getGame().updateCitySight(true, true);

		updateMaintenance();
		updateReligionHappiness();
		updateReligionCommerce();

		GC.getGame().AI_makeAssignWorkDirty();

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


int CvPlayer::getNoNonStateReligionSpreadCount() const
{
	return m_iNoNonStateReligionSpreadCount;
}


bool CvPlayer::isNoNonStateReligionSpread() const
{
	return (getNoNonStateReligionSpreadCount() > 0);
}


void CvPlayer::changeNoNonStateReligionSpreadCount(int iChange)
{
	m_iNoNonStateReligionSpreadCount = (m_iNoNonStateReligionSpreadCount + iChange);
	FAssert(getNoNonStateReligionSpreadCount() >= 0);
}


int CvPlayer::getStateReligionHappiness() const
{
	return m_iStateReligionHappiness;
}


void CvPlayer::changeStateReligionHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iStateReligionHappiness = (m_iStateReligionHappiness + iChange);

		updateReligionHappiness();
	}
}


int CvPlayer::getNonStateReligionHappiness() const
{
	return m_iNonStateReligionHappiness;
}


void CvPlayer::changeNonStateReligionHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iNonStateReligionHappiness = (m_iNonStateReligionHappiness + iChange);

		updateReligionHappiness();
	}
}

// < Civic Infos Plus Start >
int CvPlayer::getStateReligionExtraHealth() const
{
	return m_iStateReligionExtraHealth;
}


void CvPlayer::changeStateReligionExtraHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iStateReligionExtraHealth = (m_iStateReligionExtraHealth + iChange);

		updateReligionHealth();
	}
}

void CvPlayer::updateReligionHealth()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateReligionHealth();
	}
}

int CvPlayer::getNonStateReligionExtraHealth() const
{
	return m_iNonStateReligionExtraHealth;
}


void CvPlayer::changeNonStateReligionExtraHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iNonStateReligionExtraHealth = (m_iNonStateReligionExtraHealth + iChange);

		updateReligionHealth();
	}
}
// < Civic Infos Plus End   >
int CvPlayer::getStateReligionUnitProductionModifier() const
{
	return m_iStateReligionUnitProductionModifier;
}


void CvPlayer::changeStateReligionUnitProductionModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iStateReligionUnitProductionModifier = (m_iStateReligionUnitProductionModifier + iChange);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getStateReligionBuildingProductionModifier() const
{
	return m_iStateReligionBuildingProductionModifier;
}


void CvPlayer::changeStateReligionBuildingProductionModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iStateReligionBuildingProductionModifier = (m_iStateReligionBuildingProductionModifier + iChange);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getStateReligionFreeExperience() const
{
	return m_iStateReligionFreeExperience;
}


void CvPlayer::changeStateReligionFreeExperience(int iChange)
{
	m_iStateReligionFreeExperience = (m_iStateReligionFreeExperience + iChange);
}


void CvPlayer::setCapitalCity(CvCity* pNewCapitalCity)
{
	CvCity* pOldCapitalCity = getCapitalCity();
	if(pOldCapitalCity == pNewCapitalCity)
		return;

	bool bUpdatePlotGroups = (pOldCapitalCity == NULL || pNewCapitalCity == NULL ||
			pOldCapitalCity->getPlot().getOwnerPlotGroup() !=
			pNewCapitalCity->getPlot().getOwnerPlotGroup());
	if (bUpdatePlotGroups)
	{
		if (pOldCapitalCity != NULL)
			pOldCapitalCity->getPlot().updatePlotGroupBonus(false, /* advc.064d: */ false);
		if (pNewCapitalCity != NULL)
			pNewCapitalCity->getPlot().updatePlotGroupBonus(false);
	}

	if (pNewCapitalCity != NULL)
		m_iCapitalCityID = pNewCapitalCity->getID();
	else m_iCapitalCityID = FFreeList::INVALID_INDEX;

	if (bUpdatePlotGroups)
	{
		if (pOldCapitalCity != NULL)
			pOldCapitalCity->getPlot().updatePlotGroupBonus(true, /* advc.064d: */ false);
		if (pNewCapitalCity != NULL)
			pNewCapitalCity->getPlot().updatePlotGroupBonus(true);
	}
//DPII < Maintenance Modifier >
		if (pOldCapitalCity != NULL)
		{
            if ((pOldCapitalCity->area()) != (pNewCapitalCity->area()))
            {
                pNewCapitalCity->area()->setHomeArea(getID(), pOldCapitalCity->area());
            }
		}
		else
		{
            pNewCapitalCity->area()->setHomeArea(getID(), NULL);
		}
//DPII < Maintenance Modifier >
	updateMaintenance();
	updateTradeRoutes();

	if (pOldCapitalCity != NULL)
	{
		pOldCapitalCity->updateCommerce();
		pOldCapitalCity->setInfoDirty(true);
	}
	if (pNewCapitalCity != NULL)
	{
		pNewCapitalCity->updateCommerce();
		pNewCapitalCity->setInfoDirty(true);
	}
}

// <advc.127b>
int CvPlayer::getCapitalX(TeamTypes eObserver, bool bDebug) const
{
	CvCity* cap = getCapitalCity();
	if(cap == NULL || (eObserver != NO_TEAM && !cap->isRevealed(eObserver, bDebug)))
		return -1;
	return cap->getX();
}

int CvPlayer::getCapitalY(TeamTypes eObserver, bool bDebug) const
{
	CvCity* cap = getCapitalCity();
	if(cap == NULL || (eObserver != NO_TEAM && !cap->isRevealed(eObserver, bDebug)))
		return -1;
	return cap->getY();
}

int CvPlayer::getCapitalX(PlayerTypes eObserver, bool bDebug) const
{
	return getCapitalX(eObserver == NO_PLAYER ? NO_TEAM : TEAMID(eObserver), bDebug);
}
int CvPlayer::getCapitalY(PlayerTypes eObserver, bool bDebug) const
{
	return getCapitalY(eObserver == NO_PLAYER ? NO_TEAM : TEAMID(eObserver), bDebug);
} // </advc.127b>


int CvPlayer::getCitiesLost() const
{
	return m_iCitiesLost;
}


void CvPlayer::changeCitiesLost(int iChange)
{
	m_iCitiesLost = (m_iCitiesLost + iChange);
}


int CvPlayer::getWinsVsBarbs() const
{
	return m_iWinsVsBarbs;
}


void CvPlayer::changeWinsVsBarbs(int iChange)
{
	m_iWinsVsBarbs = (m_iWinsVsBarbs + iChange);
	FAssert(getWinsVsBarbs() >= 0);
}


int CvPlayer::getAssets() const
{
	return m_iAssets;
}


void CvPlayer::changeAssets(int iChange)
{
	m_iAssets = (m_iAssets + iChange);
	FAssert(getAssets() >= 0);
}


int CvPlayer::getPower() const
{
	return m_iPower;
}


void CvPlayer::changePower(int iChange)
{
	m_iPower = (m_iPower + iChange);
	FAssert(getPower() >= 0);
}


int CvPlayer::getPopScore(bool bCheckVassal) const
{
	if (bCheckVassal && isAVassal())
	{
		return m_iPopulationScore / 2;
	}

	int iVassalScore = 0;

	if (bCheckVassal)
	{
		for (int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			if (i != getID())
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)i);
				if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).isVassal(getTeam()))
				{
					iVassalScore += kLoopPlayer.getPopScore(false) / 2;
				}
			}
		}
	}

	return (m_iPopulationScore + iVassalScore / std::max(1, GET_TEAM(getTeam()).getNumMembers()));
}

void CvPlayer::changePopScore(int iChange)
{
	if (iChange != 0)
	{
		m_iPopulationScore += iChange;
		FAssert(getPopScore() >= 0);

		GC.getGame().setScoreDirty(true);
	}
}


int CvPlayer::getLandScore(bool bCheckVassal) const
{
	if (bCheckVassal && isAVassal())
	{
		return m_iLandScore / 2;
	}

	int iVassalScore = 0;

	if (bCheckVassal)
	{
		for (int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			if (i != getID())
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)i);
				if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).isVassal(getTeam()))
				{
					iVassalScore += kLoopPlayer.getLandScore(false) / 2;
				}
			}
		}
	}

	return (m_iLandScore + iVassalScore  / std::max(1, GET_TEAM(getTeam()).getNumMembers()));
}


void CvPlayer::changeLandScore(int iChange)
{
	if (iChange != 0)
	{
		m_iLandScore += iChange;
		FAssert(getLandScore() >= 0);

		GC.getGame().setScoreDirty(true);
	}
}


int CvPlayer::getWondersScore() const
{
	return m_iWondersScore;
}


void CvPlayer::changeWondersScore(int iChange)
{
	if (iChange != 0)
	{
		m_iWondersScore += iChange;
		FAssert(getWondersScore() >= 0);

		GC.getGame().setScoreDirty(true);
	}
}


int CvPlayer::getTechScore() const
{
	return m_iTechScore;
}


void CvPlayer::changeTechScore(int iChange)
{
	if (iChange != 0)
	{
		m_iTechScore += iChange;
		FAssert(getTechScore() >= 0);

		GC.getGame().setScoreDirty(true);
	}
}

int CvPlayer::getCombatExperience() const
{
	return m_iCombatExperience;
}

void CvPlayer::setCombatExperience(int iExperience)
{
	FAssert(iExperience >= 0);
	// <advc>
	if(iExperience == getCombatExperience())
		return;
	m_iCombatExperience = iExperience;
	if(isBarbarian())
		return;
	CvGame& g = GC.getGame();
	// </advc>

	int iExperienceThreshold = greatPeopleThreshold(true);
	if (m_iCombatExperience >= iExperienceThreshold && iExperienceThreshold > 0)
	{	// create great person
		CvCity const* pBestCity = NULL;
		int iBestValue = MAX_INT;
		FOR_EACH_CITY(pLoopCity, *this)
		{
			int iValue = 4 * g.getSorenRandNum(getNumCities(), "Warlord City Selection");

			for (int i = 0; i < NUM_YIELD_TYPES; i++)
			{
				iValue += pLoopCity->findYieldRateRank((YieldTypes)i);
			}
			iValue += pLoopCity->findPopulationRank();

			if (iValue < iBestValue)
			{
				pBestCity = pLoopCity;
				iBestValue = iValue;
			}
		}

		if (pBestCity != NULL)
		{
			int iRandOffset = g.getSorenRandNum(GC.getNumUnitInfos(), "Warlord Unit Generation");
			for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
			{
				UnitTypes eLoopUnit = (UnitTypes)((iI + iRandOffset) % GC.getNumUnitInfos());
				if (GC.getInfo(eLoopUnit).getLeaderExperience() > 0 || GC.getInfo(eLoopUnit).getLeaderPromotion() != NO_PROMOTION)
				{
					pBestCity->createGreatPeople(eLoopUnit, false, true);
					setCombatExperience(getCombatExperience() - iExperienceThreshold);
					break;
				}
			}
		}
	} // <advc.078>
	if(getID() == g.getActivePlayer() && BUGOption::isEnabled("MainInterface__Combat_Counter", false))
		gDLL->UI().setDirty(GameData_DIRTY_BIT, true);
	// </advc.078>
}

void CvPlayer::changeCombatExperience(int iChange)
{
	setCombatExperience(getCombatExperience() + iChange);
}


bool CvPlayer::isConnected() const
{
	return gDLL->isConnected(getNetID());
}

int CvPlayer::getNetID() const
{
	return GC.getInitCore().getNetID(getID());
}


void CvPlayer::setNetID(int iNetID)
{
	GC.getInitCore().setNetID(getID(), iNetID);
}


uint CvPlayer::getStartTime() const
{
	return m_uiStartTime;
}


void CvPlayer::setStartTime(uint uiStartTime)
{
	m_uiStartTime = uiStartTime;
}


uint CvPlayer::getTotalTimePlayed() const
{
	return ((timeGetTime() - m_uiStartTime)/1000);
}


bool CvPlayer::isMinorCiv() const
{
	return GC.getInitCore().getMinorNationCiv(m_eID);
}


void CvPlayer::setAlive(bool bNewValue)  // advc: some style changes
{
	if(isAlive() == bNewValue)
		return;

	bool const bEverAlive = isEverAlive();
	m_bAlive = bNewValue;
	CvGame& g = GC.getGame();
	// <advc.opt>
	if (isAlive() && !bEverAlive && getParent() == NO_PLAYER && !isBarbarian())
		g.changeCivPlayersEverAlive(1);
	GET_TEAM(getTeam()).updateLeaderID(); // </advc.opt>
	GET_TEAM(getTeam()).changeAliveCount(isAlive() ? 1 : -1);
	// <advc.agent>
	if (!isAlive())
		GC.getAgents().playerDefeated(getID());
	else if (bEverAlive && getParent() == NO_PLAYER) // Colonies are handled by initInGame
	{
		GC.getAgents().playerRevived(getID());
		// <advc.104r> (UWAI data gets deleted upon death)
		if(getUWAI.isEnabled())
			getUWAI.processNewCivInGame(getID()); // </advc.104r>
	}
	// </advc.agent>
	// Report event to Python
	CvEventReporter::getInstance().setPlayerAlive(getID(), bNewValue);

	if (isAlive())
	{
		if (!isEverAlive())
		{
			m_bEverAlive = true;
			GET_TEAM(getTeam()).changeEverAliveCount(1);
		}
		if (getNumCities() <= 0)
			setFoundedFirstCity(false);
		updatePlotGroups();
		if (g.isMPOption(MPOPTION_SIMULTANEOUS_TURNS) ||
			g.getNumGameTurnActive() == 0 ||
			(g.isSimultaneousTeamTurns() && GET_TEAM(getTeam()).isTurnActive()))
		{
			setTurnActive(true);
		}
		gDLL->openSlot(getID());
		if(!isBarbarian()) // advc.003n
		{	// K-Mod. Attitude cache
			for (PlayerTypes i = (PlayerTypes)0; i < MAX_CIV_PLAYERS; i=(PlayerTypes)(i+1))
			{
				/*GET_PLAYER(i).AI_updateAttitude(getID());
				AI().AI_updateAttitude(i);*/
				/*  advc.001: E.g. AI_getRankDifferenceAttitude can change between
					any two civs. */
				if (GET_PLAYER(i).isAlive() && /* advc.003n: */ !GET_PLAYER(i).isMinorCiv())
					GET_PLAYER(i).AI_updateAttitude();
			} // K-Mod end
		}
	}
	else
	{
		// <advc.001> CvTeam::makePeace does this, but here they missed it.
		for(int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			CvPlayer& kWarEnemy = GET_PLAYER((PlayerTypes)i);
			if(kWarEnemy.isAlive() && kWarEnemy.getID() != getID() &&
				!kWarEnemy.isMinorCiv() && GET_TEAM(kWarEnemy.getTeam()).isAtWar(getTeam()))
			{
				kWarEnemy.updateWarWearinessPercentAnger();
			}
		} // </advc.001>
		clearResearchQueue();
		clearPopups(); // advc
		killUnits();
		killCities();
		killAllDeals();

		setTurnActive(false);

		gDLL->endMPDiplomacy();
		gDLL->endDiplomacy();

		if (!isHuman())
			gDLL->closeSlot(getID());

		if (g.getElapsedGameTurns() > 0)
		{
			if (!isBarbarian())
			{
				CvWString szBuffer
					//= gDLL->getText("TXT_KEY_MISC_CIV_DESTROYED", getCivilizationAdjectiveKey());
					// advc.099: Replacing the above
					= gDLL->getText("TXT_KEY_PLAYER_ELIMINATED", getNameKey());
				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						gDLL->UI().addMessage((PlayerTypes)iI, false, -1, szBuffer,
								"AS2D_CIVDESTROYED", MESSAGE_TYPE_MAJOR_EVENT, NULL,
								GC.getColorType("WARNING_TEXT"));
					}
				}
				g.addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer, -1, -1,
						GC.getColorType("WARNING_TEXT"));
				// <advc.104>
				if ((getUWAI.isEnabled() || getUWAI.isEnabled(true)) && !isMinorCiv())
					AI().uwai().uninit(); // </advc.104>
			}
		}
	}

	g.setScoreDirty(true);
	// <advc.700>
	if(g.isOption(GAMEOPTION_RISE_FALL))
		g.getRiseFall().reportElimination(getID()); // </advc.700>
}


void CvPlayer::verifyAlive()
{
	CvGame const& kGame = GC.getGame();
	if (!isAlive())
	{
		if (getNumCities() > 0 || getNumUnits() > 0)
			setAlive(true);
		return; // advc
	}
	bool bKill = false; // advc: Removed superfluous code
	if (!isBarbarian())
	{
		if (getNumCities() == 0 && getAdvancedStartPoints() < 0)
		{
			//if (getNumUnits() == 0 || (!kGame.isOption(GAMEOPTION_COMPLETE_KILLS) && isFoundedFirstCity()))
			// advc.701: COMPLETE_KILLS option removed (replacing the line above)
			if (getNumUnits() <= 0 || isFoundedFirstCity())
				bKill = true;
		}
	}
	if (!bKill && !isBarbarian() && kGame.getMaxCityElimination() > 0 &&
		getCitiesLost() >= kGame.getMaxCityElimination())
	{
		bKill = true;
	}
	if (bKill)
	{	// <advc.127> Defeat of active player during AI Auto Play
		if (isHumanDisabled() && kGame.getActivePlayer() == getID() &&
			!kGame.isOption(GAMEOPTION_RISE_FALL)) // advc.701
		{
			// Automatically change active player
			PlayerTypes eNextAlive = getID();
			for (int iPass = 0; iPass < 2; iPass++)
			{
				bool bAllowMinor = (iPass == 1);
				do // Akin to RiseFall::nextCivAlive
				{
					eNextAlive = (PlayerTypes)((eNextAlive + 1) % MAX_CIV_PLAYERS);
				} while(eNextAlive != getID() &&
					(!GET_PLAYER(eNextAlive).isAlive() || GET_PLAYER(eNextAlive).isHuman() ||
					GET_PLAYER(eNextAlive).isHumanDisabled() ||
					(!bAllowMinor && GET_PLAYER(eNextAlive).isMinorCiv())));
				if (eNextAlive != getID())
					break;
			}
			if (eNextAlive != getID())
			{
				setIsHuman(false);
				GC.getGame().changeHumanPlayer(eNextAlive);
				GET_PLAYER(eNextAlive).setHumanDisabled(true);
			}
			else
			{
				FAssertMsg(kGame.isGameMultiPlayer(), "No other civ alive left?");
				// (Let the original AI Auto Play code handle it)
			}
		} // </advc.127>
		setAlive(false);
	}
}


bool CvPlayer::isTurnActive() const
{
	return m_bTurnActive;
}

void CvPlayer::setTurnActiveForPbem(bool bActive)
{
	FAssertMsg(GC.getGame().isPbem(), "You are using setTurnActiveForPbem. Are you sure you know what you're doing?");

	// does nothing more than to set the member variable before saving the game
	// the rest of the turn will be performed upon loading the game
	// This allows the player to browse the game in paused mode after he has generated the save
	if (isTurnActive() != bActive)
	{
		m_bTurnActive = bActive;
		GC.getGame().changeNumGameTurnActive(isTurnActive() ? 1 : -1);
		// BETTER_BTS_AI_MOD, Efficiency (plot danger cache), 08/21/09, jdog5000: START
		//if (GC.getGame().getNumGameTurnActive() != 1)  // advc (comment): This BBAI line had already been commented out in BBAI
		GC.getMap().invalidateActivePlayerSafeRangeCache();
		// BETTER_BTS_AI_MOD: END
	}
}

// This structure of this function has been changed for K-Mod.
void CvPlayer::setTurnActive(bool bNewValue, bool bDoTurn)
{
	if (isTurnActive() == bNewValue)
		return;

	m_bTurnActive = bNewValue;
	CvGame& kGame = GC.getGame();
	if (isTurnActive())
	{
		FAssert(isAlive());
		// <advc.001x> (or move this to the very end of doTurn?)
		changeGoldenAgeTurns(getGoldenAgeLength() * m_iScheduledGoldenAges);
		m_iScheduledGoldenAges = 0;
		// </advc.001x>
		// K-Mod
		AI().AI_updateCacheData();
		onTurnLogging(); // bbai logging
		// K-Mod end
		GC.getLogger().logTurnActive(getID()); // advc.003t

		setEndTurn(false);
		kGame.resetTurnTimer();

		if (gDLL->IsPitbossHost()) // If we are the Pitboss, send this player an email
		{	// If this guy is not currently connected, try sending him an email
			if (isHuman() && !isConnected())
				GC.getPythonCaller()->sendEmailReminder(getPbemEmailAddress());
		}

		if ((kGame.isHotSeat() || kGame.isPbem()) && isHuman() && bDoTurn)
		{
			gDLL->getInterfaceIFace()->clearEventMessages();
			// advc.135a: Commented out
			//gDLL->getEngineIFace()->setResourceLayer(false);
			kGame.setActivePlayer(getID());
		}

		kGame.changeNumGameTurnActive(1);

		if (bDoTurn)
		{
			if (isAlive() && !isHuman() && !isBarbarian() && (getAdvancedStartPoints() >= 0))
				AI().AI_doAdvancedStart();

			if (kGame.getElapsedGameTurns() > 0)
			{
				if (isAlive())
				{
					if (kGame.isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
						doTurn();
					// K-Mod. Call CvTeam::doTurn at the start of this team's turn. ie. when the leader's turn is activated.
					// Note: in simultaneous turns mode this is called by CvGame::doTurn,
					// because from here we can't tell which player in each team will be activated first.
					else
					{
						if (GET_TEAM(getTeam()).getLeaderID() == getID())
							GET_TEAM(getTeam()).doTurn();
					}
					// K-Mod end

					doTurnUnits();
				}
			}

			if (getID() == kGame.getActivePlayer() && kGame.getElapsedGameTurns() > 0)
			{
				if (kGame.isNetworkMultiPlayer())
				{
					gDLL->UI().addMessage(getID(), true, -1,
							gDLL->getText("TXT_KEY_MISC_TURN_BEGINS").GetCString(),
							"AS2D_NEWTURN", MESSAGE_TYPE_DISPLAY_ONLY);
				}
				else gDLL->getInterfaceIFace()->playGeneralSound("AS2D_NEWTURN");
			}
			// <advc.706> Skip warnings and messages if only pausing for civ selection
			if (!kGame.isOption(GAMEOPTION_RISE_FALL) ||
				!kGame.getRiseFall().isSelectingCiv()) // </advc.706>
			{
				doWarnings();
				// <advc.106b>
				if (isHuman())
				{
					validateDiplomacy(); // advc.001e
					if(kGame.getActivePlayer() == getID())
					{
						/*  Make sure that Python events like Civ4lerts are
							triggered before processing messages */
						CyArgsList pyArgs;
						pyArgs.add(kGame.getTurnSlice());
						CvEventReporter::getInstance().genericEvent("gameUpdate", pyArgs.makeFunctionArgs());
						postProcessMessages();
					}
				} // </advc.106b>
			}
			// <advc.044>
			if (isHuman() || isHumanDisabled())
			{
				// <advc.700>
				if(kGame.isOption(GAMEOPTION_RISE_FALL))
					kGame.getRiseFall().atActiveTurnStart();
				// </advc.700>
				else kGame.autoSave(); // advc.106l
			} // </advc.044>
			// <advc.106b> Clear messages in any case (in particular during AIAutoPlay)
			for (size_t i = 0; i < m_aMajorMsgs.size(); i++)
				SAFE_DELETE(m_aMajorMsgs[i]);
			m_aMajorMsgs.clear(); // </106b>
		}

		if (getID() == kGame.getActivePlayer())
		{
			if (gDLL->getInterfaceIFace()->getLengthSelectionList() == 0)
				kGame.cycleSelectionGroups_delayed(1, false);
			gDLL->getInterfaceIFace()->setDirty(SelectionCamera_DIRTY_BIT, true);
		}
	}
	else
	{
		GC.getLogger().logTurnActive(getID()); // advc.003t

		if (getID() == kGame.getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setForcePopup(false);
			gDLL->getInterfaceIFace()->clearQueuedPopups();
			gDLL->getInterfaceIFace()->flushTalkingHeadMessages();
		}

		if (getID() == kGame.getActivePlayer())
			startProfilingDLL(true); // start profiling DLL if desired

		kGame.changeNumGameTurnActive(-1);

		if (bDoTurn)
		{
			if (!kGame.isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
			{
				if (isAlive())
					doTurn();

				if ((kGame.isPbem() || kGame.isHotSeat()) && isHuman() &&
					kGame.countHumanPlayersAlive() > 1)
				{
					kGame.setHotPbemBetweenTurns(true);
				}

				if (kGame.isSimultaneousTeamTurns())
				{
					if (!GET_TEAM(getTeam()).isTurnActive())
					{
						for (int iI = (getTeam() + 1); iI < MAX_TEAMS; iI++)
						{
							if (GET_TEAM((TeamTypes)iI).isAlive())
							{
								GET_TEAM((TeamTypes)iI).setTurnActive(true);
								break;
							}
						}
					}
				}
				else
				{
					for (int iI = (getID() + 1); iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							if (kGame.isPbem() && GET_PLAYER((PlayerTypes)iI).isHuman())
							{
								if (!kGame.getPbemTurnSent())
									gDLL->sendPbemTurn((PlayerTypes)iI);
							}
							else GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
							break;
						}
					}
				}
			}
		}
	}
	gDLL->getInterfaceIFace()->updateCursorType();
	gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	GC.getMap().invalidateActivePlayerSafeRangeCache();
}

// K-Mod. The body of this function use to be part of setTurnActive.
// I've moved it here just to improve the readability of that function.
// (This logging is from bbai.)
void CvPlayer::onTurnLogging() const
{
	if (gPlayerLogLevel > 0)
	{
		logBBAI("Player %d (%S) setTurnActive for turn %d (%d %s)", getID(), getCivilizationDescription(0), GC.getGame().getGameTurn(), std::abs(GC.getGame().getGameTurnYear()), GC.getGame().getGameTurnYear()>0 ? "AD" : "BC");

		if (GC.getGame().getGameTurn() > 0 && (GC.getGame().getGameTurn() % 25) == 0 && !isBarbarian())
		{
			CvWStringBuffer szBuffer;
			GAMETEXT.setScoreHelp(szBuffer, getID());
			logBBAI("%S", szBuffer);

			int iGameTurn = GC.getGame().getGameTurn();
			logBBAI("  Total Score: %d, Population Score: %d (%d total pop), Land Score: %d, Tech Score: %d, Wonder Score: %d", calculateScore(), getPopScore(false), getTotalPopulation(), getLandScore(false), getTechScore(), getWondersScore());

			int iEconomy = 0;
			int iProduction = 0;
			int iAgri = 0;
			int iCount = 0;
			for (int iI = 1; iI <= 5; iI++)
			{
				if (iGameTurn - iI >= 0)
				{
					iEconomy += getEconomyHistory(iGameTurn - iI);
					iProduction += getIndustryHistory(iGameTurn - iI);
					iAgri += getAgricultureHistory(iGameTurn - iI);
					iCount++;
				}
			}
			iEconomy /= std::max(1, iCount);
			iProduction /= std::max(1, iCount);
			iAgri /= std::max(1, iCount);

			logBBAI("  Economy avg: %d,  Industry avg: %d,  Agriculture avg: %d", iEconomy, iProduction, iAgri);
		}
	}

	if (gPlayerLogLevel >= 2)
	{
		CvWStringBuffer szBuffer;

		logBBAI("    Player %d (%S) has %d cities, %d pop, %d power, %d tech percent", getID(), getCivilizationDescription(0), getNumCities(), getTotalPopulation(), getPower(), GET_TEAM(getTeam()).getBestKnownTechScorePercent());
		if (AI().AI_isFinancialTrouble())
			logBBAI("    Financial trouble!");

		szBuffer.append(CvWString::format(L"    Team %d has met: ", getTeam()));

		for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			if (iI != getTeam() && GET_TEAM(getTeam()).isHasMet((TeamTypes)iI))
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					szBuffer.append(CvWString::format(L"%d,", iI));
				}
			}
		}

		if (GET_TEAM(getTeam()).getVassalCount() > 0)
		{
			szBuffer.append(CvWString::format(L";  vassals: "));

			for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
			{
				if (iI != getTeam() && GET_TEAM((TeamTypes)iI).isVassal(getTeam()))
				{
					if (GET_TEAM((TeamTypes)iI).isAlive())
					{
						szBuffer.append(CvWString::format(L"%d,", iI));
					}
				}
			}
		}

		if (GET_TEAM(getTeam()).getNumWars(false) > 0)
		{
			szBuffer.append(CvWString::format(L";  at war with: "));

			for(int iI = 0; iI < MAX_CIV_TEAMS; iI++)
			{
				if(iI != getTeam() && GET_TEAM(getTeam()).isAtWar((TeamTypes)iI))
				{
					if (GET_TEAM((TeamTypes)iI).isAlive())
					{
						szBuffer.append(CvWString::format(L"%d,", iI));
					}
				}
			}
		}

		if (GET_TEAM(getTeam()).AI_isAnyWarPlan())
		{
			szBuffer.append(CvWString::format(L";  planning war with: "));

			for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
			{
				if (iI != getTeam() && !GET_TEAM(getTeam()).isAtWar((TeamTypes)iI) && GET_TEAM(getTeam()).AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive())
					{
						szBuffer.append(CvWString::format(L"%d,", iI));
					}
				}
			}
		}

		logBBAI("%S", szBuffer.getCString());

		szBuffer.clear();

		if (GET_TEAM(getTeam()).AI_isAnyWarPlan()) logBBAI("    Enemy power perc: %d (%d with others reduction)", GET_TEAM(getTeam()).AI_getEnemyPowerPercent(), GET_TEAM(getTeam()).AI_getEnemyPowerPercent(true));
	}
} // K-Mod end

bool CvPlayer::isAutoMoves() const
{
	return m_bAutoMoves;
}


void CvPlayer::setAutoMoves(bool bNewValue)
{
	if (isAutoMoves() == bNewValue)
		return; // advc

	m_bAutoMoves = bNewValue;

	if (!isAutoMoves())
	{
		if (isEndTurn() || !isHuman())
		{
			setTurnActive(false);
		}
		else
		{
			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);
				//GC.getGame().cycleSelectionGroups_delayed(1, false, true); // this is a subtle case. I think it's best to just use the normal delay
			}
		}
	}
}


bool CvPlayer::isEndTurn() const
{
	return m_bEndTurn;
}


void CvPlayer::setEndTurn(bool bNewValue)
{
	if (isEndTurn() != bNewValue)
	{
		FAssertMsg(isTurnActive(), "isTurnActive is expected to be true");

		m_bEndTurn = bNewValue;

		if (isEndTurn())
		{
			setAutoMoves(true);
		}
	}
}

bool CvPlayer::isTurnDone() const
{
	// if this returns true, popups and diplomacy will wait to appear until next turn
	if (!GC.getGame().isPbem() && !GC.getGame().isHotSeat())
	{
		return false;
	}
	if (!isHuman())
	{
		return true;
	}
	if (!isEndTurn())
	{
		return false;
	}
	return (!isAutoMoves());
}

bool CvPlayer::isExtendedGame() const
{
	return m_bExtendedGame;
}


void CvPlayer::makeExtendedGame()
{
	m_bExtendedGame = true;
}


bool CvPlayer::isFoundedFirstCity() const
{
	return m_bFoundedFirstCity;
}


void CvPlayer::setFoundedFirstCity(bool bNewValue)
{
	if (isFoundedFirstCity() == bNewValue)
		return;

	m_bFoundedFirstCity = bNewValue;
	if (getID() == GC.getGame().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
	}
	/*  <advc.104> So that rivals (with higher ids) can immediately evaluate
		war plans against this player  */
	if(!isBarbarian() && getParent() == NO_PLAYER && getUWAI.isEnabled() &&
			GC.getGame().isFinalInitialized()) // No update while setting up a scenario
		AI().uwai().getCache().update(); // </advc.104>
}

// <advc.078>
bool CvPlayer::isAnyGPPEver() const
{
	return m_bAnyGPPEver;
}

void CvPlayer::reportFirstGPP()
{
	m_bAnyGPPEver = true;
} // </advc.078>


bool CvPlayer::isStrike() const
{
	return m_bStrike;
}


void CvPlayer::setStrike(bool bNewValue)
{
	if (isStrike() != bNewValue)
	{
		m_bStrike = bNewValue;

		if (isStrike())
		{
			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->UI().addMessage(getID(), false, -1,
						gDLL->getText("TXT_KEY_MISC_UNITS_ON_STRIKE").GetCString(),
						"AS2D_STRIKE", MESSAGE_TYPE_MINOR_EVENT, NULL,
						GC.getColorType("WARNING_TEXT"));
				gDLL->UI().setDirty(GameData_DIRTY_BIT, true);
			}
		}
	}
}


HandicapTypes CvPlayer::getHandicapType() const
{
	// <advc.127>
	if (isHumanDisabled() && // <advc.706>
		// With R&F, Ctrl+Shift+X still leads to Auto Play with AI handicap.
		(!GC.getGame().isOption(GAMEOPTION_RISE_FALL) ||
		!GC.getGame().getRiseFall().hasRetired())) // </advc.706>
	{
		return GC.getGame().getAIHandicap();
	} // </advc.127>
	return GC.getInitCore().getHandicap(getID());
}


CivilizationTypes CvPlayer::getCivilizationType() const
{
	return GC.getInitCore().getCiv(getID());
}

// <advc.003w>
void CvPlayer::setCivilization(CivilizationTypes eCivilization)
{
	SAFE_DELETE(m_pCivilization);
	if (eCivilization != NO_CIVILIZATION)
		m_pCivilization = new CvCivilization(GC.getInfo(eCivilization));
} // </advc.003w>


LeaderHeadTypes CvPlayer::getLeaderType() const
{
	return GC.getInitCore().getLeader(getID());
}


LeaderHeadTypes CvPlayer::getPersonalityType() const
{
	return m_ePersonalityType;
}


void CvPlayer::setPersonalityType(LeaderHeadTypes eNewValue)
{
	m_ePersonalityType = eNewValue;
	AI().AI_updatePersonality(); // advc.104
}


void CvPlayer::setCurrentEra(EraTypes eNewValue)
{
	if (getCurrentEra() == eNewValue)
		return; // advc

	EraTypes eOldEra = m_eCurrentEra;
	m_eCurrentEra = eNewValue;

	if (GC.getGame().getActiveTeam() != NO_TEAM)
	{
		CvMap const& kMap = GC.getMap();
		for (int iI = 0; iI < kMap.numPlots(); iI++)
		{
			CvPlot& kPlot = kMap.getPlotByIndex(iI);
			kPlot.updateGraphicEra();
			if (kPlot.getRevealedImprovementType(GC.getGame().getActiveTeam(), true) != NO_IMPROVEMENT)
			{
				if (kPlot.getOwner() == getID() || (!kPlot.isOwned() &&
					getID() == GC.getGame().getActivePlayer()))
				{
					kPlot.setLayoutDirty(true);
				}
			}
		}
	}

	// dirty all of this player's cities...
	FOR_EACH_CITY_VAR(pLoopCity, *this)
	{
		//if (pLoopCity->getOwner() == getID())
		FAssert(pLoopCity->getOwner() == getID()); // K-Mod
		pLoopCity->setLayoutDirty(true);
	}

	//update unit eras
	FOR_EACH_UNIT_VAR(pLoopUnit, *this)
	{
		gDLL->getEntityIFace()->updateGraphicEra(pLoopUnit->getUnitEntity(), eOldEra);
	}

	//update main interface flag
	gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);

	if (getID() == GC.getGame().getActivePlayer())
		gDLL->getInterfaceIFace()->setDirty(Soundtrack_DIRTY_BIT, true);

	if (isHuman() && getCurrentEra() != GC.getGame().getStartEra() && !GC.getGame().isNetworkMultiPlayer())
	{
		if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
			if (NULL != pInfo)
			{
				pInfo->setData1(eNewValue);
				pInfo->setText(L"showEraMovie");
				addPopup(pInfo);
			}
		}
	} // <advc.106>
	if (GC.getDefineBOOL("SHOW_ENTERED_ERA_IN_REPLAY"))
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_SOMEONE_ENTERED_ERA",
				getNameKey(), GC.getInfo(eNewValue).getTextKeyWide());
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer,
				-1, -1, GC.getColorType("ALT_HIGHLIGHT_TEXT"));
	} // </advc.106>
	// <advc.106n> Save pre-Industrial minimap terrain for replay
	if (GC.getGame().isFinalInitialized() &&
		getCurrentEra() >= GC.getDefineINT("REPLAY_TEXTURE_ERA"))
	{
		CvMap& kMap = GC.getMap();
		if (kMap.getReplayTexture() == NULL)
			kMap.updateReplayTexture();
	} // </advc.106n>
}


ReligionTypes CvPlayer::getLastStateReligion() const
{
	return m_eLastStateReligion;
}


ReligionTypes CvPlayer::getStateReligion() const
{
	return ((isStateReligion()) ? getLastStateReligion() : NO_RELIGION);
}


void CvPlayer::setLastStateReligion(ReligionTypes eNewValue)
{
	if(getLastStateReligion() == eNewValue)
		return;
	FAssert(!isBarbarian()); // advc.003n
	// religion visibility now part of espionage
	//GC.getGame().updateCitySight(false, true);

	ReligionTypes eOldReligion = getLastStateReligion();
	m_eLastStateReligion = eNewValue;

	// religion visibility now part of espionage
	//GC.getGame().updateCitySight(true, true);

	updateMaintenance();
	updateReligionHappiness();
	updateReligionCommerce();

	GC.getGame().updateSecretaryGeneral();

	GC.getGame().AI_makeAssignWorkDirty();

	gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);

	if (!GC.getGame().isFinalInitialized())
		return; // advc
	if (gDLL->isDiplomacy() && (gDLL->getDiplomacyPlayer() == getID()))
	{
		gDLL->updateDiplomacyAttitude(true);
	}

	if (!isBarbarian()
		/*  advc.150a: Message superfluous when already reported switch to
			civic that prohibits state religion. */
		&& isStateReligion())
	{
		CvWString szBuffer;
		// <advc.150a> Sufficient to set szBuffer once (always the same message)
		if(eNewValue == NO_RELIGION)
			szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_RENOUNCE_RELIGION",
					getNameKey(), GC.getInfo(eOldReligion).getTextKeyWide());
		else
		{
			szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_CONVERT_RELIGION",
					getNameKey(), GC.getInfo(eNewValue).getTextKeyWide());
			// <advc.151>
			if(eOldReligion != NO_RELIGION)
			{
				szBuffer += L" " + gDLL->getText("TXT_KEY_MISC_AND_RENOUNCE_RELIGION",
						GC.getInfo(eOldReligion).getTextKeyWide());
			} // </advc.151>
		} // </advc.150a>
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayer const& kObs = GET_PLAYER((PlayerTypes)iI);
			if (!kObs.isAlive())
				continue; // advc
			if (GET_TEAM(getTeam()).isHasMet(kObs.getTeam()) ||
				 kObs.isSpectator()) // advc.127
			{
				gDLL->UI().addMessage(kObs.getID(), false, -1, szBuffer,
						"AS2D_RELIGION_CONVERT", MESSAGE_TYPE_MAJOR_EVENT,
						// <advc.127b>
						NULL, NO_COLOR, getCapitalX(kObs.getTeam(), true),
						getCapitalY(kObs.getTeam(), true)); // </advc.127b>
			}
		}
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer);
	}
	// advc.064d: Production of religious buildings and units may no longer be valid
	verifyCityProduction();

	// Python Event
	CvEventReporter::getInstance().playerChangeStateReligion(getID(), eNewValue, eOldReligion);
	// <K-Mod>
	for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it) // advc.003n
	{
		CvPlayerAI& kLoopPlayer = *it;
		if (kLoopPlayer.getStateReligion() != NO_RELIGION)
		{
			AI().AI_updateAttitude(kLoopPlayer.getID());
			kLoopPlayer.AI_updateAttitude(getID());
		}
	} // </K-Mod end>
}

PlayerTypes CvPlayer::getParent() const
{
	return m_eParent;
}

void CvPlayer::setParent(PlayerTypes eParent)
{
	if(m_eParent == eParent)
		return; // advc.opt
	m_eParent = eParent;
	// <advc.opt>
	FAssert(isAlive());
	/*  Wouldn't be too difficult to support NO_PLAYER, but it's not currently used -
		once a parent, ever a parent. */
	FAssert(eParent != NO_PLAYER);
	if(!isAlive() || eParent == NO_PLAYER)
		return;
	GC.getGame().changeCivPlayersEverAlive(-1);
	/*  Might later break free and find a permanent ally, but, upon creation,
		the colonial vassal is going to be in a singleton team. */
	FAssert(GET_TEAM(getTeam()).getNumMembers() == 1);
	GC.getGame().changeCivTeamsEverAlive(-1);
	// </advc.opt>
}

// <advc> More convenient access to CvTeam::getMasterTeam
TeamTypes CvPlayer::getMasterTeam() const
{
	return GET_TEAM(getTeam()).getMasterTeam();
}


bool CvPlayer::isAVassal() const
{
	return GET_TEAM(getTeam()).isAVassal();
} // </advc>


void CvPlayer::updateTeamType()
{
	if(getID() == NO_PLAYER)
		m_eTeamType = NO_TEAM;
	else m_eTeamType = GC.getInitCore().getTeam(getID());
}


void CvPlayer::setTeam(TeamTypes eTeam)
{
	FAssert(eTeam != NO_TEAM);
	FAssert(getTeam() != NO_TEAM);
	TeamTypes oldTeam = getTeam(); // advc.opt
	GET_TEAM(getTeam()).changeNumMembers(-1);
	if (isAlive())
		GET_TEAM(getTeam()).changeAliveCount(-1);
	if (isEverAlive())
		GET_TEAM(getTeam()).changeEverAliveCount(-1);
	GET_TEAM(getTeam()).changeNumCities(-(getNumCities()));
	GET_TEAM(getTeam()).changeTotalPopulation(-(getTotalPopulation()));
	GET_TEAM(getTeam()).changeTotalLand(-(getTotalLand()));

	GC.getInitCore().setTeam(getID(), eTeam);

	GET_TEAM(getTeam()).changeNumMembers(1);
	if (isAlive())
		GET_TEAM(getTeam()).changeAliveCount(1);
	if (isEverAlive())
		GET_TEAM(getTeam()).changeEverAliveCount(1);
	// <advc.opt>
	if(getTeam() != oldTeam)
	{
		GET_TEAM(getTeam()).updateLeaderID();
		GET_TEAM(oldTeam).updateLeaderID();
	} // </advc.opt>
	GET_TEAM(getTeam()).changeNumCities(getNumCities());
	GET_TEAM(getTeam()).changeTotalPopulation(getTotalPopulation());
	GET_TEAM(getTeam()).changeTotalLand(getTotalLand());
	// K-Mod Attitude cache
	if (GC.getGame().isFinalInitialized() /* advc.003n: */ && !isBarbarian())
	{
		for (PlayerTypes i = (PlayerTypes)0; i < MAX_CIV_PLAYERS; i=(PlayerTypes)(i+1))
		{
			AI().AI_updateAttitude(i);
			GET_PLAYER(i).AI_updateAttitude(getID());
		}
	} // K-Mod end
}


PlayerColorTypes CvPlayer::getPlayerColor() const
{
	return GC.getInitCore().getColor(getID());
}

/*	advc.058: New function; see getKnownCivDescription. (But this one here
	is actually used.) */
PlayerColorTypes CvPlayer::getKnownPlayerColor(TeamTypes eObserver) const
{
	// advc.058: Moved from getPlayerColor
	// K-Mod. Conceal the player colour of unmet players.
	if (concealUnknownCivs() &&
		!GET_TEAM(eObserver == NO_TEAM ? GC.getGame().getActiveTeam() : eObserver).
		isHasSeen(getTeam()))
	{
		return GC.getInitCore().getColor(BARBARIAN_PLAYER);
	} // K-Mod end
	return getPlayerColor();
}

// advc.058: Better conceal colors when the EXE asks (as K-Mod did too)
PlayerColorTypes CvPlayer::getPlayerColorExternal() const
{
	return getKnownPlayerColor();
}


int CvPlayer::getPlayerTextColorR() const
{	// advc: Round to nearest; also in the other getPlayerTextColor functions.
	return ::round(GC.getInfo(GC.getInfo(getPlayerColor()).getTextColorType()).
			getColor().r * 255);
}


int CvPlayer::getPlayerTextColorG() const
{
	return ::round(GC.getInfo(GC.getInfo(getPlayerColor()).getTextColorType()).
			getColor().g * 255);
}


int CvPlayer::getPlayerTextColorB() const
{
	return ::round(GC.getInfo(GC.getInfo(getPlayerColor()).getTextColorType()).
			getColor().b * 255);
}


int CvPlayer::getPlayerTextColorA() const
{
	return ::round(GC.getInfo(GC.getInfo(getPlayerColor()).getTextColorType()).
			getColor().a * 255);
}

// advc.106:
ColorTypes CvPlayer::getPlayerTextColor() const
{
	return GC.getInfo(getPlayerColor()).getTextColorType();
} 


int CvPlayer::getSeaPlotYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiSeaPlotYield[eIndex];
}


void CvPlayer::changeSeaPlotYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiSeaPlotYield[eIndex] = (m_aiSeaPlotYield[eIndex] + iChange);

		updateYield();
	}
}


int CvPlayer::getYieldRateModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiYieldRateModifier[eIndex];
}


void CvPlayer::changeYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiYieldRateModifier[eIndex] = (m_aiYieldRateModifier[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);

		if (eIndex == YIELD_COMMERCE)
		{
			updateCommerce();
		}

		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getCapitalYieldRateModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalYieldRateModifier[eIndex];
}


void CvPlayer::changeCapitalYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange == 0)
		return; // advc

	m_aiCapitalYieldRateModifier[eIndex] += iChange;
	invalidateYieldRankCache(eIndex);

	CvCity* pCapitalCity = getCapitalCity();
	if (pCapitalCity == NULL)
		return;

	if (eIndex == YIELD_COMMERCE)
		pCapitalCity->updateCommerce();
	pCapitalCity->AI_setAssignWorkDirty(true);
	if (pCapitalCity->getTeam() == GC.getGame().getActiveTeam())
		pCapitalCity->setInfoDirty(true);
}


int CvPlayer::getExtraYieldThreshold(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiExtraYieldThreshold[eIndex];
}


void CvPlayer::updateExtraYieldThreshold(YieldTypes eIndex)
{
	int iBestValue;
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	iBestValue = 0;

	FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::updateExtraYieldThreshold");
	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			if (GC.getInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex) > 0)
			{
				if ((iBestValue == 0) || (GC.getInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex) < iBestValue))
				{
					iBestValue = GC.getInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex);
				}
			}
		}
	}

	if (getExtraYieldThreshold(eIndex) != iBestValue)
	{
		m_aiExtraYieldThreshold[eIndex] = iBestValue;
		FAssert(getExtraYieldThreshold(eIndex) >= 0);

		updateYield();
	}
}


int CvPlayer::getTradeYieldModifier(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiTradeYieldModifier[eIndex];
}


void CvPlayer::changeTradeYieldModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiTradeYieldModifier[eIndex] = (m_aiTradeYieldModifier[eIndex] + iChange);

		updateTradeRoutes();
	}
}


int CvPlayer::getFreeCityCommerce(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiFreeCityCommerce[eIndex];
}


void CvPlayer::changeFreeCityCommerce(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiFreeCityCommerce[eIndex] = (m_aiFreeCityCommerce[eIndex] + iChange);
		FAssert(getFreeCityCommerce(eIndex) >= 0);

		updateCommerce(eIndex);
	}
}


int CvPlayer::getCommercePercent(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCommercePercent[eIndex];
}

// K-Mod. This function has been rewritten to enforce the rules of flexible / inflexible commerce types.
// (not all changes marked)
bool CvPlayer::setCommercePercent(CommerceTypes eIndex, int iNewValue, bool bForce)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!bForce && !isCommerceFlexible(eIndex))
	{
		FAssertMsg(false, "setCommercePercent called without permission.");
		return false; // can't change percent
	}

	int iOldValue = getCommercePercent(eIndex);
	iNewValue = range(iNewValue, 0, 100);

	if (iOldValue == iNewValue)
		return false;

	m_aiCommercePercent[eIndex] = iNewValue;

	int iTotalCommercePercent = 0;

	for (CommerceTypes i = (CommerceTypes)0; i < NUM_COMMERCE_TYPES; i = (CommerceTypes)(i+1))
	{
		iTotalCommercePercent += getCommercePercent(i);
	}

	for (CommerceTypes i = (CommerceTypes)0; iTotalCommercePercent != 100 && i < NUM_COMMERCE_TYPES; i = (CommerceTypes)(i+1))
	{
		if (i != eIndex && isCommerceFlexible(i))
		{
			FAssert(bForce || isCommerceFlexible(i));
			int iAdjustment = std::min(m_aiCommercePercent[i], iTotalCommercePercent - 100);
			m_aiCommercePercent[i] -= iAdjustment;
			iTotalCommercePercent -= iAdjustment;
		}
	}
	// if we couldn't balance the books, we need to do a second pass, with fewer restrictions
	if (iTotalCommercePercent != 100)
	{
		for (CommerceTypes i = (CommerceTypes)0; iTotalCommercePercent != 100 && i < NUM_COMMERCE_TYPES; i = (CommerceTypes)(i+1))
		{
			if (bForce ? i != eIndex : isCommerceFlexible(i))
			{
				FAssert(bForce || isCommerceFlexible(i));
				int iAdjustment = std::min(m_aiCommercePercent[i], iTotalCommercePercent - 100);
				m_aiCommercePercent[i] -= iAdjustment;
				iTotalCommercePercent -= iAdjustment;
			}
		}
	}
	FAssert(100 == iTotalCommercePercent);

	if (iOldValue == getCommercePercent(eIndex))
		return false;

	updateCommerce();
	// K-Mod. For human players, update commerce weight immediately so that they can see effects on working plots, etc.
	if (isHuman() && isTurnActive())
		AI().AI_updateCommerceWeights();
	// K-Mod end
	AI_makeAssignWorkDirty();

	/*if (getTeam() == GC.getGame().getActiveTeam()) {
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Financial_Screen_DIRTY_BIT, true);
	}*/ // BtS
	// K-Mod
	if (getTeam() == GC.getGame().getActiveTeam())
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true); // research turns left?

	if (getID() == GC.getGame().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Financial_Screen_DIRTY_BIT, true);
		// <advc.120c>
		// For slider on Espionage screen
		gDLL->getInterfaceIFace()->setDirty(Espionage_Advisor_DIRTY_BIT, true);
		// Redraw +/- buttons if espionage set to 0
		gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true);
		/*  There seems to be some sort of race condition here. Occasionally
			(rarely?), the espionage screen isn't correctly updated when the
			slider goes from 0 to 10. Also, the order of these two setDirty calls
			seems to matter. Dirtying the Espionage_Advisor first works better. */
		// </advc.120c>
	}
	// K-Mod end

	return true;
}


bool CvPlayer::changeCommercePercent(CommerceTypes eIndex, int iChange)
{
	return setCommercePercent(eIndex, (getCommercePercent(eIndex) + iChange));
}


int CvPlayer::getCommerceRate(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	int iRate = m_aiCommerceRate[eIndex];
	if (GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		if (eIndex == COMMERCE_CULTURE)
			iRate += m_aiCommerceRate[COMMERCE_ESPIONAGE];
		else if (eIndex == COMMERCE_ESPIONAGE)
			iRate = 0;
	}  // <advc.004x>
	iRate /= 100;
	if(eIndex == COMMERCE_RESEARCH)
	{	// advc.910:
		static int const iBASE_RESEARCH_RATE = GC.getDefineINT("BASE_RESEARCH_RATE");
		iRate += iBASE_RESEARCH_RATE;
	}
	return iRate; // </advc.004x>
}


void CvPlayer::changeCommerceRate(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCommerceRate[eIndex] += iChange;
		FAssert(getCommerceRate(eIndex) >= 0);
		if (getID() == GC.getGame().getActivePlayer())
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
	}
}

int CvPlayer::getCommerceRateModifier(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCommerceRateModifier[eIndex];
}


void CvPlayer::changeCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCommerceRateModifier[eIndex] = (m_aiCommerceRateModifier[eIndex] + iChange);

		updateCommerce(eIndex);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getCapitalCommerceRateModifier(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalCommerceRateModifier[eIndex];
}


void CvPlayer::changeCapitalCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange == 0)
		return;

	m_aiCapitalCommerceRateModifier[eIndex] = (m_aiCapitalCommerceRateModifier[eIndex] + iChange);

	CvCity* pCapitalCity = getCapitalCity();

	if (pCapitalCity != NULL)
	{
		pCapitalCity->updateCommerce();

		pCapitalCity->AI_setAssignWorkDirty(true);
	}
}


int CvPlayer::getStateReligionBuildingCommerce(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiStateReligionBuildingCommerce[eIndex];
}


void CvPlayer::changeStateReligionBuildingCommerce(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiStateReligionBuildingCommerce[eIndex] = (m_aiStateReligionBuildingCommerce[eIndex] + iChange);
		FAssert(getStateReligionBuildingCommerce(eIndex) >= 0);

		updateCommerce(eIndex);
	}
}


int CvPlayer::getSpecialistExtraCommerce(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiSpecialistExtraCommerce[eIndex];
}


void CvPlayer::changeSpecialistExtraCommerce(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiSpecialistExtraCommerce[eIndex] = (m_aiSpecialistExtraCommerce[eIndex] + iChange);
		FAssert(getSpecialistExtraCommerce(eIndex) >= 0);

		updateCommerce(eIndex);

		AI_makeAssignWorkDirty();
	}
}


// < Civic Infos Plus Start >
int CvPlayer::getSpecialistExtraYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiSpecialistExtraYield[eIndex];
}


void CvPlayer::changeSpecialistExtraYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiSpecialistExtraYield[eIndex] = (m_aiSpecialistExtraYield[eIndex] + iChange);
		FAssert(getSpecialistExtraYield(eIndex) >= 0);

		updateYield();

		AI_makeAssignWorkDirty();
	}
}
// < Civic Infos Plus End   >

int CvPlayer::getCommerceFlexibleCount(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCommerceFlexibleCount[eIndex];
}


bool CvPlayer::isCommerceFlexible(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	/* if (!isFoundedFirstCity())
		return false;
	if (eIndex == COMMERCE_ESPIONAGE) {
		if (0 == GET_TEAM(getTeam()).getHasMetCivCount(true) || GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
			return false;
	} */ // Disabled by K-Mod. (I don't want to enforce these conditions in such a fundamental way.)

	return (GC.getInfo(eIndex).isFlexiblePercent() || (getCommerceFlexibleCount(eIndex) > 0) || GET_TEAM(getTeam()).isCommerceFlexible(eIndex));
}


void CvPlayer::changeCommerceFlexibleCount(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCommerceFlexibleCount[eIndex] = (m_aiCommerceFlexibleCount[eIndex] + iChange);
		FAssert(getCommerceFlexibleCount(eIndex) >= 0);

		if (!isCommerceFlexible(eIndex))
		{
			//setCommercePercent(eIndex, 0);
			setCommercePercent(eIndex, GC.getInfo(eIndex).getInitialPercent(), true); // K-Mod
		}

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getGoldPerTurnByPlayer(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiGoldPerTurnByPlayer[eIndex];
}


void CvPlayer::changeGoldPerTurnByPlayer(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_iGoldPerTurn = (m_iGoldPerTurn + iChange);
		m_aiGoldPerTurnByPlayer[eIndex] = (m_aiGoldPerTurnByPlayer[eIndex] + iChange);

		if (getID() == GC.getGame().getActivePlayer())
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);

		if (!isHuman() /* advc: */ && isAlive())
		{			// ^Can get called while canceling deals of a dying player
			AI().AI_doCommerce();
		}
	}
}


bool CvPlayer::isFeatAccomplished(FeatTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_FEAT_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abFeatAccomplished[eIndex];
}


void CvPlayer::setFeatAccomplished(FeatTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_FEAT_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abFeatAccomplished[eIndex] = bNewValue;
}


bool CvPlayer::isOption(PlayerOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_PLAYEROPTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	/*  <advc.127> AI Auto-Play should probably disable player options temporarily,
		but I don't think it does and this looks like a quick way to fix the problem.
		AI players can't have options. Tagging advc.001. */
	if(!isHuman())
		return false; // </advc.127>
	return m_abOptions[eIndex];
}


void CvPlayer::setOption(PlayerOptionTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_PLAYEROPTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abOptions[eIndex] = bNewValue;
}

bool CvPlayer::isPlayable() const
{
	return GC.getInitCore().getPlayableCiv(getID());
}

void CvPlayer::setPlayable(bool bNewValue)
{
	GC.getInitCore().setPlayableCiv(getID(), bNewValue);
}


int CvPlayer::getBonusExport(BonusTypes eIndex) const
{
	/*	advc.opt (tbd.): Add isAnyBonusExport, isAnyBonusImport functions for (e.g.?)
		CvPlot::updatePlotGroupBonus. Once m_paiBonusExport has been turned into an EnumMap
		(cf. CvCity::isAnyFreeBonus). */
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBonusExport[eIndex];
}


void CvPlayer::changeBonusExport(BonusTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	if(iChange == 0)
		return; // advc
	CvCity* pCapitalCity = getCapitalCity();
	if (pCapitalCity != NULL)
		pCapitalCity->getPlot().updatePlotGroupBonus(false, /* advc.064d: */ false);

	m_paiBonusExport[eIndex] = (m_paiBonusExport[eIndex] + iChange);
	FAssert(getBonusExport(eIndex) >= 0);
	if (pCapitalCity != NULL)
		pCapitalCity->getPlot().updatePlotGroupBonus(true);

	AI().AI_updateBonusValue(); // advc.036
}


int CvPlayer::getBonusImport(BonusTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBonusImport[eIndex];
}


void CvPlayer::changeBonusImport(BonusTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange == 0)
		return; // advc
	CvCity* pCapitalCity = getCapitalCity();
	if (pCapitalCity != NULL)
		pCapitalCity->getPlot().updatePlotGroupBonus(false, /* advc.064d: */ false);

	m_paiBonusImport[eIndex] = (m_paiBonusImport[eIndex] + iChange);
	FAssert(getBonusImport(eIndex) >= 0);
	if (pCapitalCity != NULL)
		pCapitalCity->getPlot().updatePlotGroupBonus(true);

	AI().AI_updateBonusValue(); // advc.036
}


int CvPlayer::getImprovementCount(ImprovementTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiImprovementCount[eIndex];
}


void CvPlayer::changeImprovementCount(ImprovementTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiImprovementCount[eIndex] = (m_paiImprovementCount[eIndex] + iChange);
//	FAssert(getImprovementCount(eIndex) >= 0);
//jculturecontrol fix by f1 advc - keldath
	FAssert(getImprovementCount(eIndex) >= 0 || (getImprovementCount(eIndex) >= -1 && GC.getGame().isOption(GAMEOPTION_CULTURE_CONTROL)));

}


int CvPlayer::getFreeBuildingCount(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiFreeBuildingCount[eIndex];
}


bool CvPlayer::isBuildingFree(BuildingTypes eIndex)	const
{
	return (getFreeBuildingCount(eIndex) > 0);
}


void CvPlayer::changeFreeBuildingCount(BuildingTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if(iChange == 0)
		return;

	int iOldFreeBuildingCount = getFreeBuildingCount(eIndex);

	m_paiFreeBuildingCount[eIndex] = (m_paiFreeBuildingCount[eIndex] + iChange);
	FAssert(getFreeBuildingCount(eIndex) >= 0);

	if (iOldFreeBuildingCount == 0)
	{
		FAssert(getFreeBuildingCount(eIndex) > 0);
		FOR_EACH_CITY_VAR(pLoopCity, *this)
			pLoopCity->setNumFreeBuilding(eIndex, 1);
	}
	else if (getFreeBuildingCount(eIndex) == 0)
	{
		FAssert(iOldFreeBuildingCount > 0);
		FOR_EACH_CITY_VAR(pLoopCity, *this)
			pLoopCity->setNumFreeBuilding(eIndex, 0);
	}

}


int CvPlayer::getExtraBuildingHappiness(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiExtraBuildingHappiness[eIndex];
}


void CvPlayer::changeExtraBuildingHappiness(BuildingTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiExtraBuildingHappiness[eIndex] += iChange;

		updateExtraBuildingHappiness();
	}
}

int CvPlayer::getExtraBuildingHealth(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiExtraBuildingHealth[eIndex];
}


void CvPlayer::changeExtraBuildingHealth(BuildingTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiExtraBuildingHealth[eIndex] += iChange;

		updateExtraBuildingHealth();
	}
}


int CvPlayer::getFeatureHappiness(FeatureTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiFeatureHappiness[eIndex];
}


void CvPlayer::changeFeatureHappiness(FeatureTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiFeatureHappiness[eIndex] = (m_paiFeatureHappiness[eIndex] + iChange);

		updateFeatureHappiness();
	}
}


int CvPlayer::getUnitClassCount(UnitClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassCount[eIndex];
}


bool CvPlayer::isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!GC.getInfo(eIndex).isNationalUnit())
		return false;

	FAssert(getUnitClassCount(eIndex) <= GC.getInfo(eIndex).getMaxPlayerInstances());

	return (getUnitClassCount(eIndex) + iExtra >= GC.getInfo(eIndex).getMaxPlayerInstances());
}


void CvPlayer::changeUnitClassCount(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiUnitClassCount[eIndex] = (m_paiUnitClassCount[eIndex] + iChange);
	FAssert(getUnitClassCount(eIndex) >= 0);
}


int CvPlayer::getUnitClassMaking(UnitClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassMaking[eIndex];
}


void CvPlayer::changeUnitClassMaking(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiUnitClassMaking[eIndex] = (m_paiUnitClassMaking[eIndex] + iChange);
		FAssert(getUnitClassMaking(eIndex) >= 0);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Help_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getUnitClassCountPlusMaking(UnitClassTypes eIndex) const
{
	return (getUnitClassCount(eIndex) + getUnitClassMaking(eIndex));
}


int CvPlayer::getBuildingClassCount(BuildingClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingClassCount[eIndex];
}


bool CvPlayer::isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!GC.getInfo(eIndex).isNationalWonder())
		return false;

	FAssertMsg(getBuildingClassCount(eIndex) <= (GC.getInfo(eIndex).getMaxPlayerInstances() + GC.getInfo(eIndex).getExtraPlayerInstances()), "BuildingClassCount is expected to be less than or match the number of max player instances plus extra player instances");

	return (getBuildingClassCount(eIndex) + iExtra >=
			GC.getInfo(eIndex).getMaxPlayerInstances() + GC.getInfo(eIndex).getExtraPlayerInstances());
}


void CvPlayer::changeBuildingClassCount(BuildingClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiBuildingClassCount[eIndex] = (m_paiBuildingClassCount[eIndex] + iChange);
	FAssert(getBuildingClassCount(eIndex) >= 0);
}


int CvPlayer::getBuildingClassMaking(BuildingClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingClassMaking[eIndex];
}


void CvPlayer::changeBuildingClassMaking(BuildingClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiBuildingClassMaking[eIndex] = (m_paiBuildingClassMaking[eIndex] + iChange);
		FAssert(getBuildingClassMaking(eIndex) >= 0);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Help_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const
{
	return (getBuildingClassCount(eIndex) + getBuildingClassMaking(eIndex));
}


int CvPlayer::getHurryCount(HurryTypes eIndex) const
{
	FAssert(eIndex >= 0);
	FAssert(eIndex < GC.getNumHurryInfos());
	return m_paiHurryCount[eIndex];
}


bool CvPlayer::canHurry(HurryTypes eIndex) const
{
	return (getHurryCount(eIndex) > 0);
}


bool CvPlayer::canPopRush() const
{
	return (m_iPopRushHurryCount > 0);
}

// <advc.064b>
bool CvPlayer::canGoldRush() const
{
	return (m_iGoldRushHurryCount > 0);
} // </advc.064b>


void CvPlayer::changeHurryCount(HurryTypes eIndex, int iChange)
{
	FAssert(eIndex >= 0);
	FAssert(eIndex < GC.getNumHurryInfos());

	int oldHurryCount = m_paiHurryCount[eIndex];
	m_paiHurryCount[eIndex] = (m_paiHurryCount[eIndex] + iChange);
	FAssert(getHurryCount(eIndex) >= 0);

	// if we just went from 0 to 1 (or the reverse)
	if ((oldHurryCount > 0) != (m_paiHurryCount[eIndex] > 0))
	{
		// does this hurry reduce population?
		if (GC.getInfo(eIndex).getProductionPerPopulation() > 0)
		{
			m_iPopRushHurryCount += iChange;
			FAssert(m_iPopRushHurryCount >= 0);
		} // <advc.064b>
		if(GC.getInfo(eIndex).getGoldPerProduction() > 0)
		{
			m_iGoldRushHurryCount += iChange;
			FAssert(m_iGoldRushHurryCount >= 0);
		} // </advc.064b>
	}
}

int CvPlayer::getSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiSpecialBuildingNotRequiredCount[eIndex];
}


bool CvPlayer::isSpecialBuildingNotRequired(SpecialBuildingTypes eIndex) const
{
	return (getSpecialBuildingNotRequiredCount(eIndex) > 0);
}


void CvPlayer::changeSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiSpecialBuildingNotRequiredCount[eIndex] = (m_paiSpecialBuildingNotRequiredCount[eIndex] + iChange);
	FAssert(getSpecialBuildingNotRequiredCount(eIndex) >= 0);
}


int CvPlayer::getHasCivicOptionCount(CivicOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiHasCivicOptionCount[eIndex];
}


bool CvPlayer::isHasCivicOption(CivicOptionTypes eIndex) const
{
	return (getHasCivicOptionCount(eIndex) > 0);
}


void CvPlayer::changeHasCivicOptionCount(CivicOptionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiHasCivicOptionCount[eIndex] = (m_paiHasCivicOptionCount[eIndex] + iChange);
	FAssert(getHasCivicOptionCount(eIndex) >= 0);
}


int CvPlayer::getNoCivicUpkeepCount(CivicOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNoCivicUpkeepCount[eIndex];
}


bool CvPlayer::isNoCivicUpkeep(CivicOptionTypes eIndex) const
{
	return (getNoCivicUpkeepCount(eIndex) > 0);
}


void CvPlayer::changeNoCivicUpkeepCount(CivicOptionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiNoCivicUpkeepCount[eIndex] = (m_paiNoCivicUpkeepCount[eIndex] + iChange);
		FAssert(getNoCivicUpkeepCount(eIndex) >= 0);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getHasReligionCount(ReligionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiHasReligionCount[eIndex];
}


int CvPlayer::countTotalHasReligion() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		iCount += getHasReligionCount((ReligionTypes)iI);
	}

	return iCount;
}

int CvPlayer::getHasCorporationCount(CorporationTypes eIndex) const
{
	if (!isActiveCorporation(eIndex))
	{
		return 0;
	}

	return m_paiHasCorporationCount[eIndex];
}


int CvPlayer::countTotalHasCorporation() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		iCount += getHasCorporationCount((CorporationTypes)iI);
	}

	return iCount;
}

bool CvPlayer::isActiveCorporation(CorporationTypes eIndex) const
{
	if (isNoCorporations())
	{
		return false;
	}

	if (isNoForeignCorporations() && !hasHeadquarters(eIndex))
	{
		return false;
	}

	return true;
}


int CvPlayer::findHighestHasReligionCount() const
{
	int iBestValue = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		int iValue = getHasReligionCount((ReligionTypes)iI);

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
		}
	}

	return iBestValue;
}


void CvPlayer::changeHasReligionCount(ReligionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiHasReligionCount[eIndex] = (m_paiHasReligionCount[eIndex] + iChange);
		FAssert(getHasReligionCount(eIndex) >= 0);

		GC.getGame().updateBuildingCommerce();

		GC.getGame().AI_makeAssignWorkDirty();
	}
}


// <advc.132> Body basically from CvPlayerAI::AI_religionTrade (thresholds tweaked)
bool CvPlayer::isMajorReligion(ReligionTypes eReligion) const
{
	int iReligionCities = getHasReligionCount(eReligion);
	if(getStateReligion() == NO_RELIGION)
		return iReligionCities > getNumCities() / 3;
	int iOldReligionCities = getHasReligionCount(getStateReligion());
	/*  Not necessarily equal to the number of cities with neither religion,
		but close enough. */
	int iOtherCities = getNumCities() - iReligionCities - iOldReligionCities;
	return (iReligionCities > iOtherCities / 3 +
			std::max(0, iOldReligionCities - 1) / 2);
} // </advc.132>


void CvPlayer::changeHasCorporationCount(CorporationTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCorporationInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiHasCorporationCount[eIndex] += iChange;
		FAssert(getHasCorporationCount(eIndex) >= 0);

		GC.getGame().updateBuildingCommerce();

		GC.getGame().AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getUpkeepCount(UpkeepTypes eIndex) const
{
	FAssertMsg(false, "m_paiUpkeepCount is unused"); // advc.003j
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUpkeepInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_paiUpkeepCount != NULL, "m_paiUpkeepCount is not expected to be equal with NULL");
	return m_paiUpkeepCount[eIndex];
}


void CvPlayer::changeUpkeepCount(UpkeepTypes eIndex, int iChange)
{
	FAssertMsg(false, "m_paiUpkeepCount is unused"); // advc.003j
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUpkeepInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		FAssertMsg(m_paiUpkeepCount != NULL, "m_paiUpkeepCount is not expected to be equal with NULL");
		m_paiUpkeepCount[eIndex] = (m_paiUpkeepCount[eIndex] + iChange);
		FAssertMsg(getUpkeepCount(eIndex) >= 0, "getUpkeepCount(eIndex) is expected to be non-negative (invalid Index)");

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getSpecialistValidCount(SpecialistTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_paiSpecialistValidCount != NULL, "m_paiSpecialistValidCount is not expected to be equal with NULL");
	return m_paiSpecialistValidCount[eIndex];
}


bool CvPlayer::isSpecialistValid(SpecialistTypes eIndex) const
{
	return (getSpecialistValidCount(eIndex) > 0);
}


void CvPlayer::changeSpecialistValidCount(SpecialistTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		FAssertMsg(m_paiSpecialistValidCount != NULL, "m_paiSpecialistValidCount is not expected to be equal with NULL");
		m_paiSpecialistValidCount[eIndex] = (m_paiSpecialistValidCount[eIndex] + iChange);
		FAssertMsg(getSpecialistValidCount(eIndex) >= 0, "getSpecialistValidCount(eIndex) is expected to be non-negative (invalid Index)");

		AI_makeAssignWorkDirty();
	}
}


bool CvPlayer::isResearchingTech(TechTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabResearchingTech[eIndex];
}


void CvPlayer::setResearchingTech(TechTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isResearchingTech(eIndex) != bNewValue)
	{
		m_pabResearchingTech[eIndex] = bNewValue;

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Popup_DIRTY_BIT, true); // to check whether we still need the tech chooser popup
		}
	}
}

bool CvPlayer::isLoyalMember(VoteSourceTypes eVoteSource) const
{
	FAssertMsg(eVoteSource >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eVoteSource < GC.getNumVoteSourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabLoyalMember[eVoteSource];
}

void CvPlayer::setLoyalMember(VoteSourceTypes eVoteSource, bool bNewValue)
{
	FAssertMsg(eVoteSource >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eVoteSource < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isLoyalMember(eVoteSource) != bNewValue)
	{
		processVoteSource(eVoteSource, false);
		m_pabLoyalMember[eVoteSource] = bNewValue;
		processVoteSource(eVoteSource, true);

		GC.getGame().updateSecretaryGeneral();
	}
}

CivicTypes CvPlayer::getCivics(CivicOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paeCivics[eIndex];
}


int CvPlayer::getSingleCivicUpkeep(CivicTypes eCivic, bool bIgnoreAnarchy,
		int iExtraCities) const // advc.004b
{
	if (eCivic == NO_CIVIC)
		return 0;

	if (isNoCivicUpkeep((CivicOptionTypes)GC.getInfo(eCivic).getCivicOptionType()))
		return 0;

	if (GC.getInfo(eCivic).getUpkeep() == NO_UPKEEP)
		return 0;


	if (!bIgnoreAnarchy && isAnarchy())
		return 0;

	static int const iUPKEEP_POPULATION_OFFSET = GC.getDefineINT("UPKEEP_POPULATION_OFFSET"); // advc.opt
	static int const iUPKEEP_CITY_OFFSET = GC.getDefineINT("UPKEEP_CITY_OFFSET"); // advc.opt

	int iUpkeep = 0;
	iUpkeep += (std::max(0, getTotalPopulation() +
			(iExtraCities * CvCity::initialPopulation()) + // advc.004b
			iUPKEEP_POPULATION_OFFSET - GC.getInfo(eCivic).getCivicOptionType()) *
			GC.getInfo((UpkeepTypes)GC.getInfo(eCivic).getUpkeep()).
			getPopulationPercent()) / 100;
	iUpkeep += (std::max(0, getNumCities() + /* advc.004b: */ iExtraCities + 
			iUPKEEP_CITY_OFFSET + GC.getInfo(eCivic).getCivicOptionType() -
			GC.getNumCivicOptionInfos() / 2) * GC.getInfo((UpkeepTypes)
			GC.getInfo(eCivic).getUpkeep()).getCityPercent()) / 100;

	iUpkeep *= std::max(0, (getUpkeepModifier() + 100));
	iUpkeep /= 100;

	iUpkeep *= GC.getInfo(getHandicapType()).getCivicUpkeepPercent();
	iUpkeep /= 100;

	if (!isHuman() && !isBarbarian())
	{
		iUpkeep *= GC.getInfo(GC.getGame().getHandicapType()).getAICivicUpkeepPercent();
		iUpkeep /= 100;
		// advc.251: Gold costs are no longer adjusted to handicap
		/*iUpkeep *= std::max(0, ((GC.getInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iUpkeep /= 100;*/
	}

	return std::max(0, iUpkeep);
}


int CvPlayer::getCivicUpkeep(CivicTypes* paeCivics, bool bIgnoreAnarchy,
	int iExtraCities) const // advc.004b
{
	if (paeCivics == NULL)
		paeCivics = m_paeCivics;

	int iTotalUpkeep = 0;
	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		iTotalUpkeep += getSingleCivicUpkeep(paeCivics[iI], bIgnoreAnarchy,
				iExtraCities); // advc.400b
	}

	return iTotalUpkeep;
}


void CvPlayer::setCivics(CivicOptionTypes eIndex, CivicTypes eNewValue)
{
	CvWString szBuffer;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eNewValue >= 0, "eNewValue is expected to be non-negative (invalid Index)");
	FAssertMsg(eNewValue < GC.getNumCivicInfos(), "eNewValue is expected to be within maximum bounds (invalid Index)");

	CivicTypes eOldCivic = getCivics(eIndex);

	if(eOldCivic == eNewValue)
		return;

	bool bWasStateReligion = isStateReligion(); // advc.106

	m_paeCivics[eIndex] = eNewValue;
	if (eOldCivic != NO_CIVIC)
		processCivics(eOldCivic, -1);
	if (getCivics(eIndex) != NO_CIVIC)
		processCivics(getCivics(eIndex), 1);

/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
	int iLoop;

		if (eNewValue != NO_CIVIC && eOldCivic != NO_CIVIC)
		{
			CvCivicInfo& kCivic = GC.getCivicInfo(getCivics(eIndex));
			CvCivicInfo& kOldCivic = GC.getCivicInfo(eOldCivic);
		}
	//defined above keldath change
	//	CvWString szBuffer;
		for (CvUnit* pLoopUnit = firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = nextUnit(&iLoop))
		{
			bool validCivics = hasValidCivics(pLoopUnit->getUnitType());
			if (!validCivics && pLoopUnit->isCivicEnabled())
			{
				pLoopUnit->setCivicEnabled(false);
				szBuffer = gDLL->getText("TXT_KEY_CIVIC_DISABLED_UNIT", pLoopUnit->getUnitType(), pLoopUnit->getNameKey());
				//fix by f1 to kmod style -keldth
				//gDLL->getInterfaceIFace()->addMessageExternal(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, pLoopUnit->getUnitInfo().getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pLoopUnit->getX(), pLoopUnit->getY(), true, true);
				gDLL->getInterfaceIFace()->addMessage (getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, pLoopUnit->getUnitInfo().getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pLoopUnit->getX(), pLoopUnit->getY(), true, true);
			}
			else if (validCivics && !pLoopUnit->isCivicEnabled())
			{
				pLoopUnit->setCivicEnabled(true);
				szBuffer = gDLL->getText("TXT_KEY_CIVIC_ENABLED_UNIT", pLoopUnit->getUnitType(), pLoopUnit->getNameKey());
				//fix by f1 to kmod style -keldth
				//gDLL->getInterfaceIFace()->addMessageExternal(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, pLoopUnit->getUnitInfo().getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pLoopUnit->getX(), pLoopUnit->getY(), true, true);
				gDLL->getInterfaceIFace()->addMessage (getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, pLoopUnit->getUnitInfo().getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pLoopUnit->getX(), pLoopUnit->getY(), true, true);
			}
		}
/**
 ** End: Unit Civic Prereq
 **/
	CvGame& g = GC.getGame();
	g.updateSecretaryGeneral();
	g.AI_makeAssignWorkDirty();

	if(!g.isFinalInitialized() || /* advc.003n: */ isBarbarian())
		return;

	if (gDLL->isDiplomacy() && (gDLL->getDiplomacyPlayer() == getID()))
		gDLL->updateDiplomacyAttitude(true);

	if (getCivics(eIndex) != NO_CIVIC)
	{	/* BtS code (which erroneously blocked the message for certain civic switches)
		if (getCivics(eIndex) != GC.getInfo(getCivilizationType()).getCivilizationInitialCivics(eIndex))*/
		if (eOldCivic != NO_CIVIC) // K-Mod
		{	// <advc.151> Moved out of the loop
			szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_ADOPTED_CIVIC", getNameKey(),
					GC.getInfo(getCivics(eIndex)).getTextKeyWide());
			// <advc.106>
			bool bRenounce = (!GC.getInfo(getCivics(eIndex)).isStateReligion() &&
					GC.getInfo(eOldCivic).isStateReligion() && bWasStateReligion &&
					getLastStateReligion() != NO_RELIGION);
			if(bRenounce)
			{
				szBuffer += L" " +  gDLL->getText("TXT_KEY_MISC_AND_RENOUNCE_RELIGION",
						GC.getInfo(getLastStateReligion()).getTextKeyWide());
			}
			else // </advc.106>
			if(eOldCivic != getCivilization().getInitialCivic(eIndex))
			{
				szBuffer += L" " + gDLL->getText("TXT_KEY_MISC_AND_ABOLISH_CIVIC",
						GC.getInfo(eOldCivic).getTextKeyWide());
			} // </advc.151>
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				CvPlayer const& kObs = GET_PLAYER((PlayerTypes)iI);
				if (!kObs.isAlive() || !GET_TEAM(getTeam()).isHasMet(kObs.getTeam()))
					continue; // advc
				gDLL->UI().addMessage(kObs.getID(), false, -1, szBuffer, "AS2D_CIVIC_ADOPT",
						bRenounce ? MESSAGE_TYPE_MAJOR_EVENT : // advc.106
						MESSAGE_TYPE_MINOR_EVENT, // advc.106b
						// advc.127b:
						NULL, NO_COLOR, getCapitalX(kObs.getTeam()), getCapitalY(kObs.getTeam()));
			}
			if (bRenounce) // advc.106
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_ADOPTED_CIVIC", getNameKey(),
						GC.getInfo(getCivics(eIndex)).getTextKeyWide());
				// <advc.106>
				szBuffer += L" " + gDLL->getText("TXT_KEY_MISC_AND_RENOUNCE_RELIGION",
						GC.getInfo(getLastStateReligion()).getTextKeyWide());
				// </advc.106>
				g.addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer);
			}
		}
	} // K-Mod. (environmentalism can change this. It's nice to see the effects immediately.)
	GC.getGame().updateGwPercentAnger();
	// <K-Mod>
	for (PlayerIter<MAJOR_CIV> it(getTeam()); it.hasNext(); ++it)
	{
		AI().AI_updateAttitude(it->getID());
		it->AI_updateAttitude(getID());
	} // </K-Mod>
}


int CvPlayer::getSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	FAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 expected to be < NUM_YIELD_TYPES");
	return m_ppaaiSpecialistExtraYield[eIndex1][eIndex2];
}


void CvPlayer::changeSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	FAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_ppaaiSpecialistExtraYield[eIndex1][eIndex2] = (m_ppaaiSpecialistExtraYield[eIndex1][eIndex2] + iChange);
		FAssert(getSpecialistExtraYield(eIndex1, eIndex2) >= 0);

		updateExtraSpecialistYield();

		AI_makeAssignWorkDirty();
	}
}

/*************************************************************************************************/
/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
/**																								**/
/**																								**/
/*************************************************************************************************/
int CvPlayer::getSpecialistCivicExtraCommerce(SpecialistTypes eIndex1, CommerceTypes eIndex) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	FAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE__TYPES");
	return m_ppaaiSpecialistCivicExtraCommerce [eIndex1][eIndex];
}


void CvPlayer::changeSpecialistCivicExtraCommerce(SpecialistTypes eIndex1, CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	FAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex expected to be < NUM_COMMERCE_TYPES");

	if (iChange != 0)
	{
		m_ppaaiSpecialistCivicExtraCommerce[eIndex1][eIndex] = (m_ppaaiSpecialistCivicExtraCommerce[eIndex1][eIndex] + iChange);
		FAssert(getSpecialistCivicExtraCommerce(eIndex1, eIndex) >= 0);

		updateSpecialistCivicExtraCommerce();

		AI_makeAssignWorkDirty();
	}
}
/*************************************************************************************************/
/**	CMEDIT: End																					**/
/*************************************************************************************************/

int CvPlayer::getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiImprovementYieldChange[eIndex1][eIndex2];
}


void CvPlayer::changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ppaaiImprovementYieldChange[eIndex1][eIndex2] = (m_ppaaiImprovementYieldChange[eIndex1][eIndex2] + iChange);
		// FAssert(getImprovementYieldChange(eIndex1, eIndex2) >= 0); // Towns in K-Mod get -1 commerce with serfdom.

		updateYield();
	}
}


// K-Mod. I've changed this function from using pUnit to using pGroup.
// I've also rewritten most of the code, to give more natural ordering, and to be more robust and readable code.
void CvPlayer::updateGroupCycle(CvSelectionGroup const& kGroup)  // advc: const reference param
{
	PROFILE_FUNC();

	CvPlot const* pPlot = kGroup.plot();
	if (pPlot == NULL || !kGroup.isCycleGroup())
		return;

	CvUnit const& kUnit = *kGroup.getHeadUnit();

	//removeGroupCycle(kGroup.getID()); // will be removed while we reposition it

	CLLNode<int>* pBestSelectionGroupNode = NULL;
	int iBestCost = MAX_INT;
	CvSelectionGroup const* pPreviousGroup = NULL;
	CLLNode<int>* pSelectionGroupNode = headGroupCycleNode();
	while (pSelectionGroupNode != NULL)
	{
		CvSelectionGroup const& kNextGroup = *getSelectionGroup(pSelectionGroupNode->m_data);

		// if we find our group in the list, remove it.
		if (&kNextGroup == &kGroup)
			pSelectionGroupNode = deleteGroupCycleNode(pSelectionGroupNode);
		else if (kNextGroup.isCycleGroup() && kNextGroup.canAllMove())
		{
			//int iCost = pPreviousGroup->groupCycleDistance(pGroup) + pGroup->groupCycleDistance(pNextGroup) - pPreviousGroup->groupCycleDistance(pNextGroup);
			int iCost = kGroup.groupCycleDistance(kNextGroup) +
					(pPreviousGroup == NULL ? 3 :
					pPreviousGroup->groupCycleDistance(kGroup) -
					pPreviousGroup->groupCycleDistance(kNextGroup));
			if (iCost < iBestCost)
			{
				iBestCost = iCost;
				pBestSelectionGroupNode = pSelectionGroupNode;
			}
			pPreviousGroup = &kNextGroup;
			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
		}
		else pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
	}
	if (pPreviousGroup)
	{
		FAssert(pPreviousGroup->isCycleGroup() && pPreviousGroup->canAllMove());
		int iCost = pPreviousGroup->groupCycleDistance(kGroup) + 3; // cost for being at the end of the list.
		if (iCost < iBestCost)
			pBestSelectionGroupNode = 0;
	}

	if (pBestSelectionGroupNode)
		m_groupCycle.insertBefore(kUnit.getGroupID(), pBestSelectionGroupNode);
	else m_groupCycle.insertAtEnd(kUnit.getGroupID());
}


void CvPlayer::removeGroupCycle(int iID)
{
	CLLNode<int>* pSelectionGroupNode = headGroupCycleNode();

	while (pSelectionGroupNode != NULL)
	{
		if (pSelectionGroupNode->m_data == iID)
		{
			pSelectionGroupNode = deleteGroupCycleNode(pSelectionGroupNode);
			break;
		}
		else
		{
			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
		}
	}
}

// K-Mod
void CvPlayer::refreshGroupCycleList()
{
	std::vector<CvSelectionGroup*> update_list;

	CLLNode<int>* pNode = headGroupCycleNode();
	while (pNode)
	{
		CvSelectionGroup* pLoopGroup = getSelectionGroup(pNode->m_data);
		CvUnit* pLoopHead = pLoopGroup->getHeadUnit();
		if (pLoopHead && pLoopGroup->isCycleGroup() && pLoopGroup->canAllMove() && (pLoopHead->hasMoved() || (pLoopHead->isCargo() && pLoopHead->getTransportUnit()->hasMoved())))
		{
			update_list.push_back(pLoopGroup);
			pNode = deleteGroupCycleNode(pNode);
		}
		else pNode = nextGroupCycleNode(pNode);
	}

	for (size_t i = 0; i < update_list.size(); i++)
	{
		updateGroupCycle(*update_list[i]);
	}
}
// K-Mod end

CLLNode<int>* CvPlayer::deleteGroupCycleNode(CLLNode<int>* pNode)
{
	return m_groupCycle.deleteNode(pNode);
}


CLLNode<int>* CvPlayer::nextGroupCycleNode(CLLNode<int>* pNode) const
{
	return m_groupCycle.next(pNode);
}


CLLNode<int>* CvPlayer::previousGroupCycleNode(CLLNode<int>* pNode) const
{
	return m_groupCycle.prev(pNode);
}


CLLNode<int>* CvPlayer::headGroupCycleNode() const
{
	return m_groupCycle.head();
}


CLLNode<int>* CvPlayer::tailGroupCycleNode() const
{
	return m_groupCycle.tail();
}


//	Finds the path length from this tech type to one you already know
int CvPlayer::findPathLength(TechTypes eTech, bool bCost) const
{
	int iNumSteps = 0;
	int iShortestPath = 0;
	int iPathLength = 0;
	if (GET_TEAM(getTeam()).isHasTech(eTech) || isResearchingTech(eTech))
	{
		//	We have this tech, no reason to add this to the pre-reqs
		//	Base case return 0, we know it...
		return 0;
	}

	//	Cycle through the and paths and add up their tech lengths
	for (int i = 0; i < GC.getNUM_AND_TECH_PREREQS(eTech); i++)
	{
		TechTypes ePreReq = (TechTypes)GC.getInfo(eTech).getPrereqAndTechs(i);
		if (ePreReq != NO_TECH)
		{
			iPathLength += findPathLength(ePreReq, bCost);
		}
	}

	TechTypes eShortestOr = NO_TECH;
	iShortestPath = MAX_INT;
	//	Find the shortest OR tech
	for (int i = 0; i < GC.getNUM_OR_TECH_PREREQS(); i++)
	{
		//	Grab the tech
		TechTypes ePreReq = (TechTypes)GC.getInfo(eTech).getPrereqOrTechs(i);

		//	If this is a valid tech
		if (ePreReq != NO_TECH)
		{
			//	Recursively find the path length (takes into account all ANDs)
			// k146 (note): This will double-count any shared AND-prepreqs.
			iNumSteps = findPathLength(ePreReq, bCost);

			//	If the prereq is a valid tech and its the current shortest, mark it as such
			if (iNumSteps < iShortestPath)
			{
				eShortestOr = ePreReq;
				iShortestPath = iNumSteps;
			}
		}
	}

	//	If the shortest OR is a valid tech, add the steps to it...
	if (eShortestOr != NO_TECH)
	{
		iPathLength += iShortestPath;
	}

	return (iPathLength + ((bCost) ? GET_TEAM(getTeam()).getResearchCost(eTech) : 1));
}


//	Function specifically for python/tech chooser screen
int CvPlayer::getQueuePosition(TechTypes eTech) const
{
	int i = 1;
	CLLNode<TechTypes>* pResearchNode;

	for (pResearchNode = headResearchQueueNode(); pResearchNode; pResearchNode = nextResearchQueueNode(pResearchNode))
	{
		if (pResearchNode->m_data == eTech)
		{
			return i;
		}
		i++;
	}

	return -1;
}


void CvPlayer::clearResearchQueue()
{
	m_researchQueue.clear();

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		setResearchingTech((TechTypes)iI, false);
	}

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


//	Pushes research onto the queue.  If it is an append if will put it
//	and its pre-reqs into the queue.  If it is not an append it will change
//	research immediately and should be used with clear.  Clear will clear the entire queue.
bool CvPlayer::pushResearch(TechTypes eTech, bool bClear, /* advc.004x: */ bool bKillPopup)
{
	FAssertMsg(eTech != NO_TECH, "Tech is not assigned a valid value");

	if (GET_TEAM(getTeam()).isHasTech(eTech) || isResearchingTech(eTech))
	{
		//	We have this tech, no reason to add this to the pre-reqs
		return true;
	}

	if (!canEverResearch(eTech))
		return false;

	bool bWasEmpty = (m_researchQueue.getLength() == 0); // advc.004x
	//	Pop the entire queue...
	if (bClear)
		clearResearchQueue();

	//	Add in all the pre-reqs for the and techs...
	for (int i = 0; i < GC.getNUM_AND_TECH_PREREQS(eTech); i++)
	{
		TechTypes ePreReq = (TechTypes)GC.getInfo(eTech).getPrereqAndTechs(i);
		if (ePreReq != NO_TECH)
		{
			if (!pushResearch(ePreReq))
				return false;
		}
	}

	// Will return the shortest path of all the or techs.  Tie breaker goes to the first one...
	TechTypes eShortestOr = NO_TECH;
	int iShortestPath = MAX_INT;
	bool bOrPrereqFound = false;
	//	Cycle through all the OR techs
	for (int i = 0; i < GC.getNUM_OR_TECH_PREREQS(eTech); i++)
	{
		TechTypes ePreReq = (TechTypes)GC.getInfo(eTech).getPrereqOrTechs(i);

		if (ePreReq == NO_TECH)
			continue; // advc

		bOrPrereqFound = true;

		//	If the pre-req exists, and we have it, it is the shortest path, get out, we're done
		if (GET_TEAM(getTeam()).isHasTech(ePreReq))
		{
			eShortestOr = ePreReq;
			break;
		}

		if (canEverResearch(ePreReq))
		{
			//	Find the length of the path to this pre-req
			int iNumSteps = findPathLength(ePreReq);

			//	If this pre-req is a valid tech, and its the shortest current path, set it as such
			if (iNumSteps < iShortestPath)
			{
				eShortestOr = ePreReq;
				iShortestPath = iNumSteps;
			}
		}
	}

	//	If the shortest path tech is valid, push it (and its children) on to the research queue recursively
	if (eShortestOr != NO_TECH)
	{
		if (!pushResearch(eShortestOr))
			return false;
	}
	else if (bOrPrereqFound)
		return false;

	//	Insert this tech at the end of the queue
	m_researchQueue.insertAtEnd(eTech);

	setResearchingTech(eTech, true);

	//	Set the dirty bits
	if (getTeam() == GC.getGame().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		// <advc.004x>
		if(bKillPopup && bWasEmpty && getID() == GC.getGame().getActivePlayer())
			killAll(BUTTONPOPUP_CHOOSETECH, 0); // </advc.004x>
	}
	// ONEVENT - Tech selected (any)
	CvEventReporter::getInstance().techSelected(eTech, getID());
	return true;
}


//	If bHead is true we delete the entire queue...
void CvPlayer::popResearch(TechTypes eTech)
{
	CLLNode<TechTypes>* pResearchNode;
	for (pResearchNode = headResearchQueueNode(); pResearchNode; pResearchNode = nextResearchQueueNode(pResearchNode))
	{
		if (pResearchNode->m_data == eTech)
		{
			m_researchQueue.deleteNode(pResearchNode);
			break;
		}
	}

	setResearchingTech(eTech, false);

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


int CvPlayer::getLengthResearchQueue() const
{
	return m_researchQueue.getLength();
}


CLLNode<TechTypes>* CvPlayer::nextResearchQueueNode(CLLNode<TechTypes>* pNode) const
{
	return m_researchQueue.next(pNode);
}


CLLNode<TechTypes>* CvPlayer::headResearchQueueNode() const
{
	return m_researchQueue.head();
}


CLLNode<TechTypes>* CvPlayer::tailResearchQueueNode() const
{
	return m_researchQueue.tail();
}


void CvPlayer::addCityName(const CvWString& szName)
{
	m_cityNames.insertAtEnd(szName);
}


int CvPlayer::getNumCityNames() const
{
	return m_cityNames.getLength();
}


CvWString CvPlayer::getCityName(int iIndex) const
{
	CLLNode<CvWString>* pCityNameNode = m_cityNames.nodeNum(iIndex);
	if (pCityNameNode != NULL)
		return pCityNameNode->m_data;
	return L"";
}


CLLNode<CvWString>* CvPlayer::nextCityNameNode(CLLNode<CvWString>* pNode) const
{
	return m_cityNames.next(pNode);
}


CLLNode<CvWString>* CvPlayer::headCityNameNode() const
{
	return m_cityNames.head();
}


CvPlotGroup* CvPlayer::addPlotGroup()
{
	return (CvPlotGroup*)m_plotGroups.add();
}


void CvPlayer::deleteCity(int iID)
{
	m_cities.removeAt(iID);
}


void CvPlayer::deleteUnit(int iID)
{
	m_units.removeAt(iID);
}


CvSelectionGroup* CvPlayer::addSelectionGroup()
{
	CvSelectionGroupAI* pGroup = m_selectionGroups.AI_add();
	/*	K-Mod. Make sure that group gets added to the group cycle list.
		(we can update the specific position in the cycle list later;
		but it's important to get it into the list.) */
	m_groupCycle.insertAtEnd(pGroup->getID());
	return pGroup;
}


void CvPlayer::deleteSelectionGroup(int iID)
{
	bool bRemoved = m_selectionGroups.removeAt(iID);

	FAssertMsg(bRemoved, "could not find group, delete failed");
}

EventTriggeredData* CvPlayer::firstEventTriggered(int *pIterIdx, bool bRev) const
{
	return (!bRev ? m_eventsTriggered.beginIter(pIterIdx) : m_eventsTriggered.endIter(pIterIdx));
}

EventTriggeredData* CvPlayer::nextEventTriggered(int *pIterIdx, bool bRev) const
{
	return (!bRev ? m_eventsTriggered.nextIter(pIterIdx) : m_eventsTriggered.prevIter(pIterIdx));
}

int CvPlayer::getNumEventsTriggered() const
{
	return m_eventsTriggered.getCount();
}

EventTriggeredData* CvPlayer::getEventTriggered(int iID) const
{
	return (EventTriggeredData*)m_eventsTriggered.getAt(iID);
}

EventTriggeredData* CvPlayer::addEventTriggered()
{
	return (EventTriggeredData*)m_eventsTriggered.add();
}

void CvPlayer::deleteEventTriggered(int iID)
{
	m_eventsTriggered.removeAt(iID);
}


void CvPlayer::addMessage(const CvTalkingHeadMessage& message)
{
	// <advc.706> Remove messages arriving during interlude from display immediately
	CvGame const& g = GC.getGame();
	if(g.isOption(GAMEOPTION_RISE_FALL) && g.getActivePlayer() == getID() &&
		g.getRiseFall().getInterludeCountdown() >= 0)
	{
		gDLL->getInterfaceIFace()->clearEventMessages();
	} // </advc.706>
	m_listGameMessages.push_back(message);
	// <advc.106b>
	// Special treatment only for events in other civs' turns.
	if(!g.isInBetweenTurns() && g.getActivePlayer() == getID())
		return;
	/* DISPLAY_ONLY, COMBAT, CHAT, QUEST don't show up on the Event tab
	   of the Turn Log, and therefore shouldn't count.
	   (That is assuming that quests also send INFO messages, which I haven't
	   verified - tbd.) */
	InterfaceMessageTypes eMessage = message.getMessageType();
	if(eMessage == MESSAGE_TYPE_INFO || eMessage == MESSAGE_TYPE_MINOR_EVENT ||
		eMessage == MESSAGE_TYPE_MAJOR_EVENT || eMessage == MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY)
	{
		m_iNewMessages++; // See comment in postProcessBeginTurnEvents
	}
	if(eMessage == MESSAGE_TYPE_MAJOR_EVENT)
	{
		/*  Need to make a copy b/c, apparently, the EXE deletes the original
			before postProcessBeginTurnEvents gets called. */
		CvTalkingHeadMessage* pCopy = new CvTalkingHeadMessage(message.getTurn(),
				message.getLength(), message.getDescription(),
				// Don't play it twice
				message.getSoundPlayed() ? NULL : message.getSound(),
				MESSAGE_TYPE_MAJOR_EVENT, message.getIcon(), message.getFlashColor(),
				message.getX(), message.getY(), message.getOffScreenArrows(),
				message.getOnScreenArrows());
		m_aMajorMsgs.push_back(pCopy);
	} // </advc.106b>
}

// <advc.106b>
void CvPlayer::postProcessMessages()
{
	/* Determining how many messages are being displayed:
	   - showMissedMessages doesn't help, that's only for Hotseat games.
	   - m_listGameMessages: Those are all messages in the log, and there is
		 no way of telling which messages have already been displayed; setShown
		 is apparently called when a message is added to the log, which happens
		 right after triggering it.
	   => Need to track messages triggered during other civs' turns manually
		 (m_iNewMessages). */
	/*  Don't show the log at game start (also: the BUG setting for the limit
		isn't available at the start of the 0th turn) */
	int iLimit = (GC.getGame().getElapsedGameTurns() <= 0 ? MAX_INT :
			getStartOfTurnMessageLimit());
	/* Finishing a tech should generate a message, which is rather superfluous
	   b/c of the splash screen. Don't want to suppress it b/c it should go
	   into the log, but don't count it when deciding whether to open the log
	   b/c the tech finished message doesn't take up much attention. */
	if(getCurrentResearch() == NO_TECH)
		m_iNewMessages--;
	// Don't open the Turn Log when there's only first-contact diplo
	bool bRelevantDiplo = false;
	TCHAR const* relevantNonOffers[] = { "CANCEL_DEAL", "RELIGION_PRESSURE",
			"CIVIC_PRESSURE", "JOIN_WAR", "STOP_TRADING",
	};
	if(!m_listDiplomacy.empty() && m_iNewMessages > 0)
	{
		for(CvDiploQueue::const_iterator it = m_listDiplomacy.begin(); it !=
			m_listDiplomacy.end(); it++)
		{
			CvDiploParameters* dp = *it;
			if(dp == NULL)
			{
				FAssert(dp != NULL);
				continue;
			}
			if(dp->getHumanDiplo() || dp->getOurOfferList().getLength() > 0 ||
				dp->getTheirOfferList().getLength() > 0)
			{
				bRelevantDiplo = true;
				break;
			}
			for(int i = 0; i < sizeof(relevantNonOffers) / sizeof(TCHAR*); i++)
			{
				if(dp->getDiploComment() == GC.getAIDiploCommentType(relevantNonOffers[i]))
				{
					bRelevantDiplo = true;
					break;
				}
			}
		}
	}
	if(!GC.getGame().getAIAutoPlay() && iLimit >= 0 && (m_iNewMessages > iLimit ||
		(m_iNewMessages > 0 && (bRelevantDiplo ||
		/*  Hotseat seems to show messages only if there hasn't been another
			human turn since the message was triggered (can't check that here;
			have to show the Turn Log in all cases). */
		GC.getGame().isHotSeat()))))
	{
		gDLL->UI().clearEventMessages();
		if(!GC.getGame().isHotSeat())
		{
			/*  Show major events even if the Turn Log gets opened. As with
				NewMessages, CvPlayer needs to keep track of the recent messages;
				use aMajorMsgs for that. */
			for(size_t i = 0; i < m_aMajorMsgs.size(); i++)
				gDLL->UI().showMessage(*m_aMajorMsgs[i]);
		}
		gDLL->UI().showTurnLog();
	}
	// Clear messages in any case
	for(size_t i = 0; i < m_aMajorMsgs.size(); i++)
		SAFE_DELETE(m_aMajorMsgs[i]);
	m_aMajorMsgs.clear();
	GC.getGame().setInBetweenTurns(false);
}

int CvPlayer::getStartOfTurnMessageLimit() const
{
	if(!BUGOption::isEnabled("MainInterface__AutoOpenEventLog", true))
		return -1;
	int r = BUGOption::getValue("MainInterface__MessageLimit", 3);
	if(!isOption(PLAYEROPTION_MINIMIZE_POP_UPS) &&
		GC.getDefineINT("MESSAGE_LIMIT_WITHOUT_MPU") == 0)
	{
		return -1;
	}
	return r;
} // </advc.106b>


void CvPlayer::clearMessages()
{
	m_listGameMessages.clear();
}


const CvMessageQueue& CvPlayer::getGameMessages() const
{
	return m_listGameMessages;
}


void CvPlayer::expireMessages()
{
	CvMessageQueue::iterator it = m_listGameMessages.begin();
	bool bFoundExpired = false;
	while(it != m_listGameMessages.end())
	{
		CvTalkingHeadMessage& message = *it;
		if (GC.getGame().getGameTurn() >= message.getExpireTurn(
			isHuman() || isHumanDisabled())) // advc.700
		{
			it = m_listGameMessages.erase(it);
			bFoundExpired = true;
		}
		else ++it;
	}
	if (bFoundExpired)
		gDLL->getInterfaceIFace()->dirtyTurnLog(getID());
}


void CvPlayer::addPopup(CvPopupInfo* pInfo, bool bFront)
{
	if (!isHuman())
	{
		SAFE_DELETE(pInfo);
		return;
	}  // <advc.004x>
	ButtonPopupTypes eType = pInfo->getButtonPopupType();
	if (eType == BUTTONPOPUP_CHANGERELIGION)
		killAll(BUTTONPOPUP_CHANGERELIGION);
	else if (eType == BUTTONPOPUP_CHANGECIVIC)
		killAll(BUTTONPOPUP_CHANGECIVIC);
	else if (eType == BUTTONPOPUP_CHOOSETECH)
		killAll(BUTTONPOPUP_CHOOSETECH, 0);
	else if (eType == BUTTONPOPUP_PYTHON_SCREEN)
	{
		CvGame& kGame = GC.getGame();
		if (kGame.getElapsedGameTurns() <= 0 && kGame.getActivePlayer() != NO_PLAYER)
		{
			// Must be DawnOfMan then
			kGame.setDawnOfManShown(true);
			bFront = true;
			GET_PLAYER(kGame.getActivePlayer()).doChangeCivicsPopup(NO_CIVIC);
		}
	} // </advc.004x>
	if (bFront)
		m_listPopups.push_front(pInfo);
	else m_listPopups.push_back(pInfo);
}


void CvPlayer::clearPopups()
{
	CvPopupQueue::iterator it;
	for (it = m_listPopups.begin(); it != m_listPopups.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	m_listPopups.clear();
}


CvPopupInfo* CvPlayer::popFrontPopup()
{
	CvPopupInfo* pInfo = NULL;
	if (!m_listPopups.empty())
	{
		pInfo = m_listPopups.front();
		m_listPopups.pop_front();
	}
	return pInfo;
}


const CvPopupQueue& CvPlayer::getPopups() const
{
	// advc.test:
	FAssertMsg(GC.getGame().getActivePlayer() == getID(),
			"Just to see under which circumstances the EXE adds popups to AI players");
	return m_listPopups;
}


void CvPlayer::addDiplomacy(CvDiploParameters* pDiplo)
{
	if (pDiplo != NULL)
		m_listDiplomacy.push_back(pDiplo);
	else FAssert(pDiplo != NULL); // advc.test
}


void CvPlayer::clearDiplomacy()
{
	CvDiploQueue::iterator it;
	for (it = m_listDiplomacy.begin(); it != m_listDiplomacy.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	m_listDiplomacy.clear();
}


const CvDiploQueue& CvPlayer::getDiplomacy() const
{
	return m_listDiplomacy;
}


CvDiploParameters* CvPlayer::popFrontDiplomacy()
{
	CvDiploParameters* pDiplo = NULL;
	if (!m_listDiplomacy.empty())
	{
		pDiplo = m_listDiplomacy.front();
		m_listDiplomacy.pop_front();
	}
	return pDiplo;
}

/*  advc.001e: The EXE is supposed to double-check all AI-to-human offers at the
	start of the human turn, i.e. just before displaying them. This function
	addresses errors and omissions in the EXE code. */
void CvPlayer::validateDiplomacy()
{
	bool bDone = false;
	while (!bDone && !m_listDiplomacy.empty())
	{
		for (CvDiploQueue::iterator it = m_listDiplomacy.begin(); it != m_listDiplomacy.end(); ++it)
		{
			CvDiploParameters* pDiplo = *it;
			CLLNode<TradeData> const* pNode = pDiplo->getOurOfferList().head(); // Can be NULL!
			// Worst enemy may have changed
			if (pDiplo->getDiploComment() == GC.getAIDiploCommentType("STOP_TRADING") &&
				pDiplo->getData() != GET_TEAM(pDiplo->getWhoTalkingTo()).AI_getWorstEnemy())
			{
				CvPlayerAI& who = GET_PLAYER(pDiplo->getWhoTalkingTo());
				/*  Recipient isn't getting contacted after all. Ideally,
					this should also be done for any contact attempts that the
					EXE cancels, but it's difficult to identify these here. */
				who.AI_changeContactTimer(getID(), CONTACT_STOP_TRADING,
						-who.AI_getContactTimer(getID(), CONTACT_STOP_TRADING));
				// Have to delete it here b/c the EXE will no longer have a pointer to it
				m_listDiplomacy.remove(pDiplo);
				delete pDiplo;
				bDone = false;
				break;
			}
			// AI may send offer for DP and then receive a DoW
			else if (pNode != NULL && pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT &&
				!GET_TEAM(getTeam()).allWarsShared(TEAMID(pDiplo->getWhoTalkingTo())))
			{
				CvPlayerAI& who = GET_PLAYER(pDiplo->getWhoTalkingTo());
				who.AI_changeContactTimer(getID(), CONTACT_DEFENSIVE_PACT,
						-who.AI_getContactTimer(getID(), CONTACT_DEFENSIVE_PACT));
				m_listDiplomacy.remove(pDiplo);
				delete pDiplo;
				bDone = false;
				break;
			}
			bDone = true;
		}
	}
	/*  <advc.134a> The EXE discards all offers for peace and surrender. Since this
		is done right before displaying the diplo popup, it doesn't help to reinsert
		the offer into m_listDiplomacy -- need to prevent the removal by the EXE. */
	std::vector<CvDiploParameters*> apInvalid;
	int iValidPeaceOffers = 0;
	for (CvDiploQueue::iterator it = m_listDiplomacy.begin(); it != m_listDiplomacy.end(); ++it)
	{
		CvDiploParameters* pDiplo = *it;
		if (pDiplo->getDiploComment() != GC.getAIDiploCommentType("OFFER_PEACE"))
			continue;
		/*  Make sure that this isn't a peace offer that the EXE will discard for
			valid reasons. I.e. check everything that the EXE checks.
			(Based on tracing DLL calls in the debugger.) */
		PlayerTypes eWho = pDiplo->getWhoTalkingTo();
		CvPlayerAI const& who = GET_PLAYER(eWho);
		// This is what the EXE seems to get wrong
		if (!::atWar(getTeam(), who.getTeam()))
		{
			apInvalid.push_back(pDiplo);
			continue;
		}
		if (!canContact(eWho) || !who.canContact(getID()))
		{
			apInvalid.push_back(pDiplo);
			continue;
		}
		CLLNode<TradeData> const* pNode = NULL;
		bool bCapitulate = false;
		bool bValid = true;
		for (pNode = pDiplo->getOurOfferList().head(); pNode != NULL;
			pNode = pDiplo->getOurOfferList().next(pNode))
		{
			/*  Also test trade denial (although the EXE doesn't do that);
				important for capitulation. */
			if (!canTradeItem(eWho, pNode->m_data, true))
			{
				bValid = false;
				break;
			}
			if (pNode->m_data.m_eItemType == TRADE_SURRENDER)
				bCapitulate = true;
		}
		if (!bValid)
		{
			apInvalid.push_back(pDiplo);
			continue;
		}
		for (pNode = pDiplo->getTheirOfferList().head(); pNode != NULL;
				pNode = pDiplo->getTheirOfferList().next(pNode))
		{
			if (!who.canTradeItem(getID(), pNode->m_data, true))
			{
				bValid = false;
				break;
			}
			if (pNode->m_data.m_eItemType == TRADE_SURRENDER)
				bCapitulate = true;
		}
		if (!bValid)
		{
			apInvalid.push_back(pDiplo);
			continue;
		}
		/*  Finally, check if the offer still makes sense for eWho.
			(Sth. the EXE doesn't check.) */
		if (!bCapitulate && !who.AI_upholdPeaceOffer(getID(), *pDiplo))
		{
			apInvalid.push_back(pDiplo);
			continue;
		}
		if (iValidPeaceOffers == 0)
		{
			/*  advancePeaceOfferStage supports only one peace offer per turn
				(this could be amended) */
			GET_TEAM(getTeam()).advancePeaceOfferStage(TEAMID(eWho));
			iValidPeaceOffers++;
		}
		else apInvalid.push_back(pDiplo);
	}
	for (size_t i = 0; i < apInvalid.size(); i++)
	{
		CvPlayerAI& who = GET_PLAYER(apInvalid[i]->getWhoTalkingTo());
		who.AI_changeContactTimer(getID(), CONTACT_PEACE_TREATY,
				-who.AI_getContactTimer(getID(), CONTACT_PEACE_TREATY));
		m_listDiplomacy.remove(apInvalid[i]);
		delete apInvalid[i];
	}
	// </advc.134a>
}


void CvPlayer::showSpaceShip()
{
	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
	pInfo->setData1(-1);
	pInfo->setText(L"showSpaceShip");
	addPopup(pInfo);
}


void CvPlayer::clearSpaceShipPopups()
{
	//clear all spaceship popups
	CvPopupQueue::iterator it;
	for (it = m_listPopups.begin(); it != m_listPopups.end(); )
	{
		CvPopupInfo* pInfo = *it;
		if (NULL != pInfo)
		{
			if(pInfo->getText().compare(L"showSpaceShip") == 0)
			{
				it = m_listPopups.erase(it);
				SAFE_DELETE(pInfo);
			}
			else it++;
		}
		else it++;
	}
}

// <advc.004x> Partly cut and pasted from CvTeam::setHasTech
void CvPlayer::doChangeCivicsPopup(CivicTypes eCivic)
{
	if(!isHuman()) // Forget reminder during Auto Play
	{
		m_eReminderPending = NO_CIVIC;
		return;
	}
	if(!canRevolution(NULL) || GC.getGame().isAboutToShowDawnOfMan())
	{
		m_eReminderPending = (CivicTypes)std::max(m_eReminderPending, eCivic);
		return;
	}
	if(eCivic == NO_CIVIC) // Then we're only supposed to check for a pending reminder
	{
		if(m_eReminderPending != NO_CIVIC)
			doChangeCivicsPopup(m_eReminderPending);
		return;
	}
	m_eReminderPending = NO_CIVIC;
	CivicOptionTypes eCivicOption = (CivicOptionTypes)GC.getInfo(eCivic).
			getCivicOptionType();
	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHANGECIVIC);
	if(pInfo != NULL)
	{
		pInfo->setData1(eCivicOption);
		pInfo->setData2(eCivic);
		gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
	}
} // </advc.004x>


int CvPlayer::getScoreHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapScoreHistory.find(iTurn);
	if (it != m_mapScoreHistory.end())
		return it->second;
	return 0;
}

void CvPlayer::updateScoreHistory(int iTurn, int iBestScore)
{
	m_mapScoreHistory[iTurn] = iBestScore;
}

int CvPlayer::getEconomyHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapEconomyHistory.find(iTurn);
	if (it != m_mapEconomyHistory.end())
		return it->second;
	return 0;
}

// <advc.004s>
void CvPlayer::updateHistoryMovingAvg(CvTurnScoreMap& history, int iGameTurn, int iNewSample) {

	int iOldSamples = std::min(2, iGameTurn - 1); // Not sure if i=0 would be a valid sample
	int iSamples = iOldSamples;
	int iSum = 0;
	// Discard NewSample if in anarchy
	if(!isAnarchy() || iGameTurn == 0)
	{
		iSum += iNewSample;
		iSamples++;
	}
	for(int i = iGameTurn - 1; i >= iGameTurn - iOldSamples; i--)
		iSum += history[i];
	history[iGameTurn] = ::round(iSum / (double)std::max(1, iSamples));
}
// </advc.004s>

void CvPlayer::updateEconomyHistory(int iTurn, int iBestEconomy)
{
	//m_mapEconomyHistory[iTurn] = iBestEconomy;
	// advc.004s: Replacing the above
	updateHistoryMovingAvg(m_mapEconomyHistory, iTurn, iBestEconomy);
}

int CvPlayer::getIndustryHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapIndustryHistory.find(iTurn);
	if (it != m_mapIndustryHistory.end())
		return it->second;
	return 0;
}

void CvPlayer::updateIndustryHistory(int iTurn, int iBestIndustry)
{
	//m_mapIndustryHistory[iTurn] = iBestIndustry;
	// advc.004s: Replacing the above
	updateHistoryMovingAvg(m_mapIndustryHistory, iTurn, iBestIndustry);
}

int CvPlayer::getAgricultureHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapAgricultureHistory.find(iTurn);
	if (it != m_mapAgricultureHistory.end())
		return it->second;
	return 0;
}

void CvPlayer::updateAgricultureHistory(int iTurn, int iBestAgriculture)
{
	//m_mapAgricultureHistory[iTurn] = iBestAgriculture;
	// advc.004s: Replacing the above
	updateHistoryMovingAvg(m_mapAgricultureHistory, iTurn, iBestAgriculture);
}

int CvPlayer::getPowerHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapPowerHistory.find(iTurn);
	if (it != m_mapPowerHistory.end())
		return it->second;
	return 0;
}

void CvPlayer::updatePowerHistory(int iTurn, int iBestPower)
{
	m_mapPowerHistory[iTurn] = iBestPower;
}

int CvPlayer::getCultureHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapCultureHistory.find(iTurn);
	if (it != m_mapCultureHistory.end())
		return it->second;
	return 0;
}

void CvPlayer::updateCultureHistory(int iTurn, int iBestCulture)
{
	//m_mapCultureHistory[iTurn] = iBestCulture;
	// advc.004s: Replacing the above
	updateHistoryMovingAvg(m_mapCultureHistory, iTurn, iBestCulture);
}

int CvPlayer::getEspionageHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapEspionageHistory.find(iTurn);
	if (it != m_mapEspionageHistory.end())
		return it->second;
	return 0;
}

void CvPlayer::updateEspionageHistory(int iTurn, int iBestEspionage)
{
	//m_mapEspionageHistory[iTurn] = iBestEspionage;
	// advc.004s: Replacing the above
	updateHistoryMovingAvg(m_mapEspionageHistory, iTurn, iBestEspionage);
}

// K-Mod. Note, this function is a friend of CvEventReporter, so that it can access the data we need.
// (This saves us from having to use the built-in CyStatistics class)
const CvPlayerRecord* CvPlayer::getPlayerRecord() const
{
	return CvEventReporter::getInstance().
			/*	advc.make: CvEventReporter::getPlayerRecord added.
				We're no longer a friend of CvEventReporter. */
			/*m_kStatistics.*/getPlayerRecord(getID());
}
// K-Mod end

std::string CvPlayer::getScriptData() const
{
	return m_szScriptData;
}

void CvPlayer::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}

const CvString CvPlayer::getPbemEmailAddress() const
{
	return GC.getInitCore().getEmail(getID());
}

void CvPlayer::setPbemEmailAddress(const char* szAddress)
{
	GC.getInitCore().setEmail(getID(), szAddress);
}

const CvString CvPlayer::getSmtpHost() const
{
	return GC.getInitCore().getSmtpHost(getID());
}

void CvPlayer::setSmtpHost(const char* szHost)
{
	GC.getInitCore().setSmtpHost(getID(), szHost);
}


void CvPlayer::doGold()
{
	if (GC.getPythonCaller()->doGold(getID()))
		return;

	// <advc.300> Let CvGame::killBarbarian handle overcrowding
	if(isBarbarian())
		return; // </advc.300>

	int iGoldChange = calculateGoldRate();

	//FAssert(isHuman() || isBarbarian() || ((getGold() + iGoldChange) >= 0) || isAnarchy());
	/*  advc.131: Disabled b/c all of these can be OK (except isBarbarian, which is
		now handled upfront) */

	changeGold(iGoldChange);
	bool bStrike = false;
	if (getGold() < 0)
	{
		setGold(0);
		if (!isBarbarian() && getNumCities() > 0)
			bStrike = true;
	}

	if (bStrike)
	{
		setStrike(true);
		changeStrikeTurns(1);

		if (getStrikeTurns() > 1)
		{
			int iDisbandUnit = (getStrikeTurns() / 2); // XXX mod?
			// advc.131: Can happen, but should be quite rare.
			FAssert(isHuman() || isAnarchy());
			for (int iI = 0; iI < iDisbandUnit; iI++)
			{
				disbandUnit(true);
				if (calculateGoldRate() >= 0)
					break;
			}
		}
	}
	else setStrike(false);
}


void CvPlayer::doResearch()
{
	if (GC.getPythonCaller()->doResearch(getID()))
		return;

	if (isResearch() /* K-Mod: */ && !isAnarchy())
	{
		bool bForceResearchChoice = false;

		if (getCurrentResearch() == NO_TECH /* K-Mod: */ && isHuman())
		{
			if (getID() == GC.getGame().getActivePlayer())
			{
				chooseTech();
			}
			//if (GC.getGame().getElapsedGameTurns() > 4) { // advc.124g (commented out)
			AI().AI_chooseResearch();
			bForceResearchChoice = true;
		}

		TechTypes eCurrentTech = getCurrentResearch();
		if (eCurrentTech == NO_TECH)
		{
			int iOverflow = (100 * calculateResearchRate()) /
					std::max(1, calculateResearchModifier(eCurrentTech));
			changeOverflowResearch(iOverflow);
		}
		else
		{
			int iOverflowResearch = (getOverflowResearch() *
					calculateResearchModifier(eCurrentTech)) / 100;
			setOverflowResearch(0);
			GET_TEAM(getTeam()).changeResearchProgress(eCurrentTech,
					// K-Mod (replacing the minimum which used to be in calculateResearchRate)
					std::max(1, calculateResearchRate()) +
					iOverflowResearch, getID());
		}

		if (bForceResearchChoice)
			clearResearchQueue();
	}
}

void CvPlayer::doEspionagePoints()  // advc: some style changes
{
	if (getCommerceRate(COMMERCE_ESPIONAGE) <= 0)
		return;

	GET_TEAM(getTeam()).changeEspionagePointsEver(getCommerceRate(COMMERCE_ESPIONAGE));

	int iSpending = 0;
	// Divide up Espionage between Teams
	for (int iLoop = 0; iLoop < MAX_CIV_TEAMS; iLoop++)
	{
		if (getTeam() == iLoop || !GET_TEAM((TeamTypes)iLoop).isAlive())
			continue;

		if (GET_TEAM(getTeam()).isHasMet((TeamTypes)iLoop))
		{
			iSpending = getEspionageSpending((TeamTypes)iLoop);
			if (iSpending > 0)
			{
				GET_TEAM(getTeam()).changeEspionagePointsAgainstTeam((TeamTypes)iLoop, iSpending);
			}
		}
	}
}

int CvPlayer::getEspionageSpending(TeamTypes eAgainstTeam) const  // advc: style changes
{
	PROFILE_FUNC(); // advc (runtime seems totally negligible)
	int iTotalWeight = 0; // Get sum of all weights to be used later on
	int iBestWeight = 0;
	bool bFoundTeam = false;
	for (int i = 0; i < MAX_CIV_TEAMS; i++)
	{
		CvTeam const& kOther = GET_TEAM((TeamTypes)i);
		if (!kOther.isAlive() || kOther.getID() == getTeam() ||
				!kOther.isHasMet(getTeam()))
			continue;

		if (kOther.getID() == eAgainstTeam)
			bFoundTeam = true;

		int iWeight = getEspionageSpendingWeightAgainstTeam(kOther.getID());
		iBestWeight = std::max(iBestWeight, iWeight);
		iTotalWeight += iWeight;
	}

	if (!bFoundTeam) // The player requested is not valid
		return -1;

	int iSpendingValue = 0;
	int const iTotalPoints = getCommerceRate(COMMERCE_ESPIONAGE);
	int iAvailablePoints = iTotalPoints;
	// Split up Espionage Point budget based on weights (if any weights have been assigned)
	if (iTotalWeight > 0)
	{
		for (int i = 0; i < MAX_CIV_TEAMS; i++)
		{
			CvTeam const& kOther = GET_TEAM((TeamTypes)i);
			if (!kOther.isAlive() || kOther.getID() == getTeam() ||
					!kOther.isHasMet(getTeam()))
				continue;

			int iChange = iTotalPoints * getEspionageSpendingWeightAgainstTeam(kOther.getID()) / iTotalWeight;
			iAvailablePoints -= iChange;
			if (kOther.getID() == eAgainstTeam)
				iSpendingValue += iChange;
		}
	}

	// Divide remainder evenly among top Teams
	while (iAvailablePoints > 0) // advc (comment): bFoundTeam=true ensures termination
	{
		for (int i = 0; i < MAX_CIV_TEAMS; i++)
		{
			CvTeam const& kOther = GET_TEAM((TeamTypes)i);
			if (!kOther.isAlive() || kOther.getID() == getTeam() ||
					!kOther.isHasMet(getTeam()))
				continue;

			if (getEspionageSpendingWeightAgainstTeam(kOther.getID()) == iBestWeight)
			{
				if (kOther.getID() == eAgainstTeam)
					iSpendingValue++;
				iAvailablePoints--;
				if (iAvailablePoints <= 0)
					break;
			}
		}
	}
	return iSpendingValue;
}

bool CvPlayer::canDoEspionageMission(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pUnit,
	bool bCheckPoints) const // advc.085
{
	if (getID() == eTargetPlayer || NO_PLAYER == eTargetPlayer)
		return false;

	if (!GET_PLAYER(eTargetPlayer).isAlive() || !GET_TEAM(getTeam()).isHasMet(TEAMID(eTargetPlayer)))
		return false;

	// K-Mod. Bugfix
	if (pUnit && pPlot && !pUnit->canEspionage(pPlot, false))
		return false;
	// K-Mod end

	CvEspionageMissionInfo& kMission = GC.getInfo(eMission);
	// Need Tech Prereq, if applicable
	if (kMission.getTechPrereq() != NO_TECH)
	{
		if (!GET_TEAM(getTeam()).isHasTech((TechTypes)kMission.getTechPrereq()))
			return false;
	}

	int iCost = getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iExtraData, pUnit);
	if (iCost < 0)
		return false;

	int iEspionagePoints = GET_TEAM(getTeam()).getEspionagePointsAgainstTeam(TEAMID(eTargetPlayer));
	// <advc.085>
	if (!bCheckPoints)
		return true;
	// </advc.085>
	if (iEspionagePoints < iCost)
		return false;
	if (iEspionagePoints <= 0)
		return false;
	return true;
}

// <advc.120d> Mostly cut-and-paste from getEspionageMissionBaseCost
TechTypes CvPlayer::getStealCostTech(PlayerTypes eTargetPlayer) const
{
	TechTypes r = NO_TECH;
	if(eTargetPlayer == NO_PLAYER)
		return r;
	int iProdCost = MAX_INT;
	for(int iTech = 0; iTech < GC.getNumTechInfos(); iTech++)
	{
		TechTypes eTech = (TechTypes)iTech;
		if(canStealTech(eTargetPlayer, eTech))
		{
			int iCost = GET_TEAM(getTeam()).getResearchCost(eTech);
			if(iCost < iProdCost)
			{
				iProdCost = iCost;
				r = eTech;
			}
		}
	}
	return r;
}


bool CvPlayer::canSpy() const
{
	for(int i = 0; i < GC.getNumUnitInfos(); i++)
	{
		UnitTypes eUnit = (UnitTypes)i;
		CvUnitInfo const& u = GC.getInfo(eUnit);
		if(u.isSpy() && canTrain(eUnit))
			return true;
	}
	FOR_EACH_UNIT(u, *this)
	{
		if(u->isSpy())
			return true;
	}
	return false;
} // </advc.120d>

int CvPlayer::getEspionageMissionCost(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer,
	const CvPlot* pPlot, int iExtraData, const CvUnit* pSpyUnit) const
{
	int iMissionCost = getEspionageMissionBaseCost(eMission, eTargetPlayer, pPlot, iExtraData, pSpyUnit);
	if (iMissionCost < 0)
		return -1;

	// Multiply cost of mission * number of team members
	//iMissionCost *= GET_TEAM(getTeam()).getNumMembers(); // K-Mod
	// dlph.33/advc
	iMissionCost = adjustMissionCostToTeamSize(iMissionCost, eTargetPlayer);

	iMissionCost *= getEspionageMissionCostModifier(eMission, eTargetPlayer, pPlot, iExtraData, pSpyUnit);
	iMissionCost /= 100;

	return std::max(0, iMissionCost);
}

// advc: Auxiliary function for dlph.33
int CvPlayer::adjustMissionCostToTeamSize(int iBaseCost, PlayerTypes eTargetPlayer) const
{
	// Don't compute anything when the teams have equal size
	int iOurTeamSize = GET_TEAM(getTeam()).getNumMembers();
	int iTheirTeamSize = GET_TEAM(eTargetPlayer).getNumMembers();
	if(iOurTeamSize == iTheirTeamSize)
		return iBaseCost;
	// Tie it to the tech cost modifier
	double extraTeamMemberModifier = GC.getDefineINT(CvGlobals::TECH_COST_EXTRA_TEAM_MEMBER_MODIFIER) / 100.0;
	/* <dlph.33> "New formula for espionage costs in team. Essentially, I want costs
		to scale with 1+0.5(number of members - 1), but since there are two teams
		(and two directions) involved, it will scale with the square root of the
		ratio of those values. Idea for formula by Fran." */
	return (int)(iBaseCost * std::sqrt(
			(1 + extraTeamMemberModifier * (iOurTeamSize - 1)) /
			(1 + extraTeamMemberModifier * (iTheirTeamSize - 1)))); // </dlph.33>
}


int CvPlayer::getEspionageMissionBaseCost(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pSpyUnit) const
{
	if (eMission == NO_ESPIONAGEMISSION) // K-Mod
		return -1;

	CvEspionageMissionInfo& kMission = GC.getInfo(eMission);
	int iBaseMissionCost = kMission.getCost();

	// -1 means this mission is disabled
	if (iBaseMissionCost == -1)
		return -1;

	CvCity* pCity = NULL;
	if (NULL != pPlot)
		pCity = pPlot->getPlotCity();

	if (kMission.isSelectPlot())
	{
		if (NULL == pPlot)
			return -1;

		if (!pPlot->isRevealed(getTeam()))
			return -1;
	}

	if (NULL == pCity && kMission.isTargetsCity())
		return -1;

	int iMissionCost = -1;

	if (kMission.getStealTreasuryTypes() > 0)
	{
		// Steal Treasury
		//int iNumTotalGold = (GET_PLAYER(eTargetPlayer).getGold() * kMission.getStealTreasuryTypes()) / 100;
		int iNumTotalGold
			= 0; // kmodx: Missing initialization

		if (NULL != pCity)
		{
			/* iNumTotalGold *= pCity->getPopulation();
			iNumTotalGold /= std::max(1, GET_PLAYER(eTargetPlayer).getTotalPopulation()); */
			// K-Mod
			iNumTotalGold = getEspionageGoldQuantity(eMission, eTargetPlayer, pCity);
		}

		if (iNumTotalGold > 0)
			iMissionCost = (iBaseMissionCost * iNumTotalGold) / 100;
	}
	else if (kMission.getBuyTechCostFactor() > 0)
	{
		// Buy (Steal) Tech
		TechTypes eTech = (TechTypes)iExtraData;
		// <advc.120d> Original code moved into auxiliary function
		iMissionCost = -1;
		if(eTech == NO_TECH)
			eTech = getStealCostTech(eTargetPlayer);
		int iProdCost = (eTech == NO_TECH ? -1 : GET_TEAM(getTeam()).//getResearchCost(eTech)
				getResearchLeft(eTech)); // advc.120i: Count own progress
		// </advc.120d>
		if (NO_TECH != eTech && canStealTech(eTargetPlayer, eTech))
			iMissionCost = iBaseMissionCost + ((100 + kMission.getBuyTechCostFactor()) * iProdCost) / 100;
	}
	else if (kMission.getSwitchCivicCostFactor() > 0)
	{
		// Switch Civics
		CivicTypes eCivic = (CivicTypes)iExtraData;

		/*if (NO_CIVIC == eCivic) {
			for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic) {
				if (canForceCivics(eTargetPlayer, (CivicTypes)iCivic)) {
					eCivic = (CivicTypes)iCivic;
					break;
				}
			}
		}
		if (NO_CIVIC != eCivic)*/
		/*  <advc.132> Replacing the above: Need to compute a cost for every civic
			(like for the DestroyBuilding mission) */
		int iMinCost = MAX_INT;
		for(int i = 0; i < GC.getNumCivicInfos(); i++)
		{
			CivicTypes eLoopCivic = (CivicTypes)i;
			if(eCivic != NO_CIVIC && eLoopCivic != eCivic)
				continue; // </advc.132>
			if (canForceCivics(eTargetPlayer, eLoopCivic))
			{
				//iMissionCost = iBaseMissionCost + (kMission.getSwitchCivicCostFactor() * GC.getInfo(GC.getGame().getGameSpeedType()).getAnarchyPercent()) / 10000;
				// K-Mod
				int iLoopCost = iBaseMissionCost + (kMission.getSwitchCivicCostFactor() * GET_PLAYER(eTargetPlayer).getTotalPopulation() * (GC.getInfo(GC.getGame().getGameSpeedType()).getAnarchyPercent()
						/* advc.132: Add 100 to one of the modifiers
						   (doesn't matter which). */
						+ (isCivic(eLoopCivic) ? 0 : 100))
					) / 10000;
				iMinCost = std::min(iMinCost, iLoopCost); // advc.132
			}
		} // <advc.132>
		if(iMinCost < MAX_INT)
			iMissionCost = iMinCost; // </advc.132>
	}
	else if (kMission.getSwitchReligionCostFactor() > 0)
	{
		// Switch Religions
		ReligionTypes eReligion = (ReligionTypes)iExtraData;
		/*if (NO_RELIGION == eReligion) {
			for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); ++iReligion) {
				if (canForceReligion(eTargetPlayer, (ReligionTypes)iReligion)) {
					eReligion = (ReligionTypes)iReligion;
					break;
				}
			}
		}
		if (NO_RELIGION != eReligion) {*/
		// <advc.132> Replacing the above: Need to compute a cost for every religion
		int iMinCost = MAX_INT;
		for(int i = 0; i < GC.getNumReligionInfos(); i++)
		{
			ReligionTypes eLoopReligion = (ReligionTypes)i;
			if(eReligion != NO_RELIGION && eLoopReligion != eReligion)
				continue;
			if (canForceReligion(eTargetPlayer, eLoopReligion)
					&& (pCity == NULL || pCity->isHasReligion(eLoopReligion)))
			{// </advc.132>
				//iMissionCost = iBaseMissionCost + (kMission.getSwitchReligionCostFactor() * GC.getInfo(GC.getGame().getGameSpeedType()).getAnarchyPercent()) / 10000;
				// K-Mod
				int iLoopCost = iBaseMissionCost + (kMission.getSwitchReligionCostFactor() * GET_PLAYER(eTargetPlayer).getTotalPopulation() * (GC.getInfo(GC.getGame().getGameSpeedType()).getAnarchyPercent()
						// advc.132: As above for civics
						+ (getStateReligion() == eLoopReligion ? 0 : 100))
					) / 10000;
				ReligionTypes eCurrentReligion = GET_PLAYER(eTargetPlayer).getStateReligion();
				// amplify the mission cost if we are trying to switch to a minority religion.
				if (eCurrentReligion != NO_RELIGION)
				{
					// maybe getReligionPopulation would be slightly better, but it's a bit slower.
					int iCurrent = GET_PLAYER(eTargetPlayer).getHasReligionCount(eCurrentReligion);
					int iNew = GET_PLAYER(eTargetPlayer).getHasReligionCount(eLoopReligion);
					int iCitiesTarget = GC.getInfo(GC.getMap().getWorldSize()).getTargetNumCities();
					FAssert(//iCurrent > 0 && // advc.006: Possible to lose all
											  // cities of the current state religion.
						iNew > 0);
					/*iMissionCost *= std::max(iCurrent, iNew) + iCitiesTarget;
					iMissionCost /= iNew + iCitiesTarget;*/
					/*  <advc.132> Replacing the above (to increase the impact of
						the iCurrent/iNew ratio) */
					iLoopCost *= iCurrent + iCitiesTarget;
					iLoopCost /= std::max(1, iNew + iCitiesTarget / 2);
					// </advc.132>
				} // K-Mod end
				iMinCost = std::min(iMinCost, iLoopCost); // advc.132
			}
		} // <advc.132>
		if(iMinCost < MAX_INT)
			iMissionCost = iMinCost; // </advc.132>
	}
	else if (kMission.getDestroyUnitCostFactor() > 0)
	{
		// Destroys Unit
		CvUnit const* pUnit = GET_PLAYER(eTargetPlayer).getUnit(iExtraData);
		int iCost = MAX_INT;
		if (pUnit == NULL && pPlot != NULL)
		{
			for (CLLNode<IDInfo> const* pUnitNode = pPlot->headUnitNode();
				pUnitNode != NULL; pUnitNode = pPlot->nextUnitNode(pUnitNode))
			{
				CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
				if (canSpyDestroyUnit(eTargetPlayer, *pLoopUnit))
				{
					int iValue = getProductionNeeded(pLoopUnit->getUnitType());
					if (iValue < iCost)
					{
						iCost = iValue;
						pUnit = pLoopUnit;
					}
				}
			}
		}
		else iCost = getProductionNeeded(pUnit->getUnitType());

		if (pUnit != NULL)
		{
			if (canSpyDestroyUnit(eTargetPlayer, *pUnit))
			{
				iMissionCost = iBaseMissionCost +
						((100 + kMission.getDestroyUnitCostFactor()) * iCost) / 100;
			}
		}
	}
	else if (kMission.getDestroyProjectCostFactor() > 0)
	{
		ProjectTypes eProject = (ProjectTypes) iExtraData;
		int iCost = MAX_INT;

		if (NO_PROJECT == eProject)
		{
			for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
			{
				if (canSpyDestroyProject(eTargetPlayer, (ProjectTypes)iProject))
				{
					int iValue = getProductionNeeded((ProjectTypes)iProject);

					if (iValue < iCost)
					{
						iCost = iValue;
						eProject = (ProjectTypes)iProject;
					}
				}
			}
		}
		else iCost = getProductionNeeded(eProject);

		if (NO_PROJECT != eProject)
		{
			if (canSpyDestroyProject(eTargetPlayer, eProject))
			{
				iMissionCost = iBaseMissionCost + ((100 + kMission.getDestroyProjectCostFactor()) * iCost) / 100;
			}
		}
	}
	else if (kMission.getDestroyProductionCostFactor() > 0)
	{
		FAssert(NULL != pCity);
		if (NULL != pCity)
		{
			iMissionCost = iBaseMissionCost + ((100 + kMission.getDestroyProductionCostFactor()) * pCity->getProduction()) / 100;
		}
	}
	else if (kMission.getBuyUnitCostFactor() > 0)
	{
		// Buy Unit
		CvUnit const* pUnit = GET_PLAYER(eTargetPlayer).getUnit(iExtraData);
		int iCost = MAX_INT;

		if (pUnit == NULL && pPlot != NULL)
		{
			for (CLLNode<IDInfo> const* pUnitNode = pPlot->headUnitNode();
				pUnitNode != NULL; pUnitNode = pPlot->nextUnitNode(pUnitNode))
				{
					CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
					if (canSpyBribeUnit(eTargetPlayer, *pLoopUnit))
					{
						int iValue = getProductionNeeded(pLoopUnit->getUnitType());
						if (iValue < iCost)
						{
							iCost = iValue;
							pUnit = pLoopUnit;
						}
					}
				}
		}
		else iCost = getProductionNeeded(pUnit->getUnitType());
		if (pUnit != NULL)
		{
			if (canSpyBribeUnit(eTargetPlayer, *pUnit))
			{
				iMissionCost = iBaseMissionCost +
						((100 + kMission.getBuyUnitCostFactor()) * iCost) / 100;
			}
		}
	}
	else if (kMission.getDestroyBuildingCostFactor() > 0)
	{
		BuildingTypes eBuilding = (BuildingTypes) iExtraData;
		int iCost = MAX_INT;

		if (NO_BUILDING == eBuilding)
		{
			for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
			{
				if (NULL != pCity && pCity->getNumRealBuilding((BuildingTypes)iBuilding) > 0)
				{
					if (canSpyDestroyBuilding(eTargetPlayer, (BuildingTypes)iBuilding))
					{
						int iValue = getProductionNeeded((BuildingTypes)iBuilding);

						if (iValue < iCost)
						{
							iCost = iValue;
							eBuilding = (BuildingTypes)iBuilding;
						}
					}
				}
			}
		}
		else iCost = getProductionNeeded(eBuilding);

		if (NO_BUILDING != eBuilding)
		{
			if (NULL != pCity && pCity->getNumRealBuilding(eBuilding) > 0)
			{
				if (canSpyDestroyBuilding(eTargetPlayer, eBuilding))
				{
					iMissionCost = iBaseMissionCost + ((100 + kMission.getDestroyBuildingCostFactor()) * iCost) / 100;
				}
			}
		}
	}
	else if (kMission.getBuyCityCostFactor() > 0)
	{
		// Buy City
		if (NULL != pCity)
		{
			iMissionCost = iBaseMissionCost + (kMission.getBuyCityCostFactor() * GC.getInfo(GC.getGame().getGameSpeedType()).getGrowthPercent()) / 10000;
		}
	}
	else if (kMission.getCityInsertCultureCostFactor() > 0)
	{
		// Insert Culture into City
		if (NULL != pPlot && pPlot->getCulture(getID()) > 0)
		{
			int iCultureAmount = kMission.getCityInsertCultureAmountFactor() *  pCity->countTotalCultureTimes100();
			iCultureAmount /= 10000;
			iCultureAmount = std::max(1, iCultureAmount);
			iMissionCost = iBaseMissionCost + (kMission.getCityInsertCultureCostFactor() * iCultureAmount) / 100;
		}
	}
	else if (kMission.isDestroyImprovement())
	{
		if (pPlot != NULL && !pPlot->isCity())
		{
			if (pPlot->isImproved() || pPlot->isRoute())
			{
				iMissionCost = (iBaseMissionCost * GC.getInfo(
						GC.getGame().getGameSpeedType()).getBuildPercent()) / 100;
			}
		}
	}
	else if (kMission.getCityPoisonWaterCounter() > 0)
	{
		FAssert(pCity != NULL);
		// Cannot poison a city's water supply if it's already poisoned (value is negative when active)
		if (pCity != NULL && pCity->getEspionageHealthCounter() <= 0)
			iMissionCost = iBaseMissionCost;
	}

	// Make city unhappy
	else if (kMission.getCityUnhappinessCounter() > 0)
	{
		FAssert(pCity != NULL);
		// Cannot make a city unhappy if you've already done it (value is negative when active)
		if (pCity != NULL && pCity->getEspionageHappinessCounter() <= 0)
			iMissionCost = iBaseMissionCost;
	}

	// Make city Revolt
	else if (kMission.getCityRevoltCounter() > 0)
	{
		FAssert(pCity != NULL);
		// Cannot make a city revolt if it's already revolting
		if (pCity != NULL && pCity->getOccupationTimer() == 0)
			iMissionCost = iBaseMissionCost;
	}
	else if (kMission.getCounterespionageMod() > 0)
	{
		if (GET_TEAM(getTeam()).getCounterespionageTurnsLeftAgainstTeam(GET_PLAYER(eTargetPlayer).getTeam()) <= 0)
		{
			iMissionCost = (iBaseMissionCost * GC.getInfo(GC.getGame().getGameSpeedType()).getResearchPercent()) / 100;
		}
	}
	else if (kMission.getPlayerAnarchyCounter() > 0)
	{
		// Player anarchy timer: can't add more turns of anarchy to player already in the midst of it
		if (!GET_PLAYER(eTargetPlayer).isAnarchy())
		{
			iMissionCost = (iBaseMissionCost * GC.getInfo(GC.getGame().getGameSpeedType()).getAnarchyPercent()) / 100;
		}
	}
	else if (kMission.isPassive())
	{
		iMissionCost = (iBaseMissionCost * (100 + GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).getEspionagePointsAgainstTeam(getTeam()))) / 100;
	}
	else
	{
		iMissionCost = (iBaseMissionCost * GC.getInfo(GC.getGame().getGameSpeedType()).getResearchPercent()) / 100;
	}

	if (iMissionCost < 0)
	{
		return -1;
	}

	return iMissionCost;
}


int CvPlayer::getEspionageMissionCostModifier(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pSpyUnit) const  // advc: some style changes
{
	// K-Mod. I've altered this function to give a generic answer when NO_ESPIONAGEMISSION is passed.

	// <advc.opt>
	static int const iESPIONAGE_CITY_POP_EACH_MOD = GC.getDefineINT("ESPIONAGE_CITY_POP_EACH_MOD");
	static int const iESPIONAGE_CITY_TRADE_ROUTE_MOD = GC.getDefineINT("ESPIONAGE_CITY_TRADE_ROUTE_MOD");
	static int const iESPIONAGE_CITY_RELIGION_STATE_MOD = GC.getDefineINT("ESPIONAGE_CITY_RELIGION_STATE_MOD");
	static int const iESPIONAGE_CITY_HOLY_CITY_MOD = GC.getDefineINT("ESPIONAGE_CITY_HOLY_CITY_MOD");
	static int const iESPIONAGE_CULTURE_MULTIPLIER_MOD = GC.getDefineINT("ESPIONAGE_CULTURE_MULTIPLIER_MOD");
	static int const iESPIONAGE_DISTANCE_MULTIPLIER_MOD = GC.getDefineINT("ESPIONAGE_DISTANCE_MULTIPLIER_MOD");
	static int const iESPIONAGE_EACH_TURN_UNIT_COST_DECREASE = GC.getDefineINT("ESPIONAGE_EACH_TURN_UNIT_COST_DECREASE");
	// </advc.opt>

	int iModifier = 100;

	CvCity* pCity = NULL;
	if (pPlot != NULL)
		pCity = pPlot->getPlotCity();

	if (NO_PLAYER == eTargetPlayer)
		eTargetPlayer = getID();

	const CvTeam& kTargetTeam = GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()); // (moved from the bottom of the function)

	//if (pCity != NULL && kMission.isTargetsCity())
	if (pCity != NULL && (eMission == NO_ESPIONAGEMISSION || GC.getInfo(eMission).isTargetsCity()))
	{
		// City Population
		iModifier *= 100 + (iESPIONAGE_CITY_POP_EACH_MOD * (pCity->getPopulation() - 1));
		iModifier /= 100;

		// Trade Route
		if (pCity->isTradeRoute(getID()))
		{
			iModifier *= 100 + iESPIONAGE_CITY_TRADE_ROUTE_MOD;
			iModifier /= 100;
		}

		ReligionTypes eReligion = getStateReligion();
		if (NO_RELIGION != eReligion)
		{
			int iReligionModifier = 0;

			// City has Your State Religion
			if (pCity->isHasReligion(eReligion))
			{
				if (GET_PLAYER(eTargetPlayer).getStateReligion() != eReligion)
					iReligionModifier += iESPIONAGE_CITY_RELIGION_STATE_MOD;

				if (hasHolyCity(eReligion))
					iReligionModifier += iESPIONAGE_CITY_HOLY_CITY_MOD;
			}

			iModifier *= 100 + iReligionModifier;
			iModifier /= 100;

		}

		// City's culture affects cost
		/*iModifier *= 100 - (pCity->getCultureTimes100(getID()) * GC.getDefineINT("ESPIONAGE_CULTURE_MULTIPLIER_MOD")) / std::max(1, pCity->getCultureTimes100(eTargetPlayer) + pCity->getCultureTimes100(getID()));
		iModifier /= 100;*/ // BtS

		iModifier *= 100 + pCity->getEspionageDefenseModifier();
		iModifier /= 100;
	}

	if (pPlot != NULL)
	{
		// K-Mod. Culture Mod. (Based on plot culture rather than city culture.)
		if (eMission == NO_ESPIONAGEMISSION || GC.getInfo(eMission).isSelectPlot() ||
			GC.getInfo(eMission).isTargetsCity())
		{
			iModifier *= 100 - (pPlot->getCulture(getID()) *
					iESPIONAGE_CULTURE_MULTIPLIER_MOD) /
					std::max(1, pPlot->getCulture(eTargetPlayer) +
					pPlot->getCulture(getID()));
			iModifier /= 100;
		}
		// K-Mod end

		// Distance mod
		int const iMaxPlotDistance = GC.getMap().maxTypicalDistance(); // advc.140: was maxPlotDistance
		int iDistance = iMaxPlotDistance;

		CvCity* pOurCapital = getCapitalCity();
		if (NULL != pOurCapital)
		{
			//if (kMission.isSelectPlot() || kMission.isTargetsCity())
			if (eMission == NO_ESPIONAGEMISSION ||
				GC.getInfo(eMission).isSelectPlot() ||
				GC.getInfo(eMission).isTargetsCity())
			{
				iDistance = ::plotDistance(pOurCapital->plot(), pPlot);
			}
			else
			{
				CvCity* pTheirCapital = GET_PLAYER(eTargetPlayer).getCapitalCity();
				if (pTheirCapital != NULL)
				{
					iDistance = ::plotDistance(pOurCapital->plot(), pTheirCapital->plot());
				}
			}
		}

		iModifier *= (iDistance + iMaxPlotDistance) *
				iESPIONAGE_DISTANCE_MULTIPLIER_MOD / iMaxPlotDistance;
		iModifier /= 100;
	}

	// Spy presence mission cost alteration
	if (NULL != pSpyUnit)
	{
		iModifier *= 100 - (pSpyUnit->getFortifyTurns() * iESPIONAGE_EACH_TURN_UNIT_COST_DECREASE);
		iModifier /= 100;
	}

	// My points VS. Your points to mod cost
	/*int iTargetPoints = kTargetTeam.getEspionagePointsEver();
	int iOurPoints = GET_TEAM(getTeam()).getEspionagePointsEver();
		iModifier *= (GC.getDefineINT("ESPIONAGE_SPENDING_MULTIPLIER") * (2 * iTargetPoints + iOurPoints)) / std::max(1, iTargetPoints + 2 * iOurPoints);
		iModifier /= 100;
	}*/ // BtS
	// K-Mod. use the dedicated function that exists for this modifier, for consistency.
	iModifier *= GET_TEAM(getTeam()).getEspionageModifier(kTargetTeam.getID());
	iModifier /= 100;
	// K-Mod end

	// Counterespionage Mission Mod
	/* if (kTargetTeam.getCounterespionageModAgainstTeam(getTeam()) > 0) {
		iModifier *= kTargetTeam.getCounterespionageModAgainstTeam(getTeam());
		iModifier /= 100;
	} */
	// K-Mod
	iModifier *= 100 + std::max(-100, kTargetTeam.getCounterespionageModAgainstTeam(getTeam()));
	iModifier /= 100;
	// K-Mod end

	return iModifier;
}


bool CvPlayer::doEspionageMission(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CvPlot* pPlot, int iExtraData, CvUnit* pSpyUnit)
{
	if (!canDoEspionageMission(eMission, eTargetPlayer, pPlot, iExtraData, pSpyUnit))
	{
		return false;
	}

	TeamTypes eTargetTeam = NO_TEAM;
	if (NO_PLAYER != eTargetPlayer)
	{
		eTargetTeam = GET_PLAYER(eTargetPlayer).getTeam();
	}

	CvEspionageMissionInfo& kMission = GC.getInfo(eMission);

	bool bSomethingHappened = false;
	bool bAggressiveMission = true; // advc.120
	bool bShowExplosion = false;
	CvWString szBuffer;

	int iMissionCost = getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iExtraData, pSpyUnit);


	//////////////////////////////
	// Destroy Improvement

	if (kMission.isDestroyImprovement())
	{
		if (pPlot != NULL)
		{
			// Blow it up
			if (pPlot->isImproved())
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED",
						GC.getInfo(pPlot->getImprovementType()).getDescription()).GetCString();
				pPlot->setImprovementType(GC.getInfo(pPlot->getImprovementType()).getImprovementPillage());
				bSomethingHappened = true;
			}
			else if (pPlot->isRoute())
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED",
						GC.getInfo(pPlot->getRouteType()).getDescription()).GetCString();
				pPlot->setRouteType(NO_ROUTE, true);
				bSomethingHappened = true;
			}

			if (bSomethingHappened)
				bShowExplosion = true;
		}
	}

	//////////////////////////////
	// Destroy Building

	if (kMission.getDestroyBuildingCostFactor() > 0)
	{
		BuildingTypes eTargetBuilding = (BuildingTypes)iExtraData;

		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED_IN", GC.getInfo(eTargetBuilding).getDescription(), pCity->getNameKey()).GetCString();
				pCity->setNumRealBuilding(eTargetBuilding, pCity->getNumRealBuilding(eTargetBuilding) - 1);

				bSomethingHappened = true;
				bShowExplosion = true;
				// K-Mod
				if (!isHuman() || pCity->isProductionAutomated())
					pCity->setChooseProductionDirty(true);
				// K-Mod end
			}
		}
	}

	//////////////////////////////
	// Destroy Project

	if (kMission.getDestroyProjectCostFactor() > 0)
	{
		ProjectTypes eTargetProject = (ProjectTypes)iExtraData;

		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED_IN", GC.getInfo(eTargetProject).getDescription(), pCity->getNameKey()).GetCString();
				GET_TEAM(eTargetTeam).changeProjectCount(eTargetProject, -1);

				bSomethingHappened = true;
				bShowExplosion = true;
			}
		}
	}

	//////////////////////////////
	// Destroy Production

	if (kMission.getDestroyProductionCostFactor() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_PRODUCTION_DESTROYED_IN", pCity->getProductionName(), pCity->getNameKey());
				pCity->setProduction(0);

				bSomethingHappened = true;
				bShowExplosion = true;
				// K-Mod
				if (!isHuman()) // not for automated cities
					pCity->setChooseProductionDirty(true);
				// K-Mod end
			}
		}
	}

	//////////////////////////////
	// Destroy Unit

	if (kMission.getDestroyUnitCostFactor() > 0)
	{
		if (NO_PLAYER != eTargetPlayer)
		{
			int iTargetUnitID = iExtraData;
			CvUnit* pUnit = GET_PLAYER(eTargetPlayer).getUnit(iTargetUnitID);
			if (NULL != pUnit)
			{
				FAssert(pUnit->atPlot(pPlot));
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED", pUnit->getNameKey()).GetCString();
				pUnit->kill(false, getID());

				bSomethingHappened = true;
				bShowExplosion = true;
			}
		}
	}

	//////////////////////////////
	// Buy Unit

	if (kMission.getBuyUnitCostFactor() > 0)
	{
		if (NO_PLAYER != eTargetPlayer)
		{
			int iTargetUnitID = iExtraData;
			CvUnit* pUnit = GET_PLAYER(eTargetPlayer).getUnit(iTargetUnitID);
			if (NULL != pUnit)
			{
				FAssert(pUnit->atPlot(pPlot));

				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_UNIT_BOUGHT", pUnit->getNameKey()).GetCString();

				UnitTypes eUnitType = pUnit->getUnitType();
				int iX = pUnit->getX();
				int iY = pUnit->getY();
				pUnit->kill(false, getID());
				initUnit(eUnitType, iX, iY, NO_UNITAI);

				bSomethingHappened = true;
			}
		}
	}

	//////////////////////////////
	// Buy City

	if (kMission.getBuyCityCostFactor() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_BOUGHT", pCity->getNameKey()).GetCString();
				acquireCity(pCity, false, true, true);

				bSomethingHappened = true;
			}
		}
	}

	//////////////////////////////
	// Insert Culture into City

	if (kMission.getCityInsertCultureCostFactor() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_CULTURE_INSERTED", pCity->getNameKey()).GetCString();

				/*int iCultureAmount = kMission.getCityInsertCultureAmountFactor() * pCity->countTotalCultureTimes100();
				iCultureAmount /= 10000;
				iCultureAmount = std::max(1, iCultureAmount);
				int iNumTurnsApplied = (GC.getDefineINT("GREAT_WORKS_CULTURE_TURNS") * GC.getInfo(GC.getGame().getGameSpeedType()).getUnitGreatWorkPercent()) / 100;
				for (int i = 0; i < iNumTurnsApplied; ++i)
					pCity->changeCulture(getID(), iCultureAmount / iNumTurnsApplied, true, true);
				if (iNumTurnsApplied > 0)
					pCity->changeCulture(getID(), iCultureAmount % iNumTurnsApplied, false, true);
				}*/ // BtS
				// K-Mod. apply culture in one hit. We don't need fake 'free city culture' anymore.
				int iCultureTimes100 = std::max(1, kMission.getCityInsertCultureAmountFactor() * pCity->countTotalCultureTimes100() / 100);

				//pCity->changeCultureTimes100(getID(), iCultureTimes100, true, true);
				pCity->doPlotCultureTimes100(true, getID(), iCultureTimes100, false); // plot culture only.
				// K-Mod end

				bSomethingHappened = true;
			}

		}
	}

	//////////////////////////////
	// Poison City's Water Supply

	if (kMission.getCityPoisonWaterCounter() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_POISONED", pCity->getNameKey()).GetCString();
				pCity->changeEspionageHealthCounter(kMission.getCityPoisonWaterCounter());

				bShowExplosion = true;
				bSomethingHappened = true;
			}
		}
	}

	//////////////////////////////
	// Make city Unhappy

	if (kMission.getCityUnhappinessCounter() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_UNHAPPY", pCity->getNameKey()).GetCString();
				pCity->changeEspionageHappinessCounter(kMission.getCityUnhappinessCounter());

				bShowExplosion = true;
				bSomethingHappened = true;
			}
		}
	}

	//////////////////////////////
	// Make city Revolt

	if (kMission.getCityRevoltCounter() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_REVOLT", pCity->getNameKey()).GetCString();
				pCity->changeCultureUpdateTimer(kMission.getCityRevoltCounter());
				pCity->changeOccupationTimer(kMission.getCityRevoltCounter());

				bSomethingHappened = true;
				bShowExplosion = true;

				if (gUnitLogLevel >= 2 && !isHuman())
				{
					logBBAI("      Spy for player %d (%S) causes revolt in %S, owned by %S (%d)", getID(), getCivilizationDescription(0), pCity->getName().GetCString(), GET_PLAYER(pCity->getOwner()).getCivilizationDescription(0), pCity->getOwner());
				}
			}
		}
	}

	//////////////////////////////
	// Steal Treasury

	if (kMission.getStealTreasuryTypes() > 0)
	{
		if (NO_PLAYER != eTargetPlayer)
		{
			//int iNumTotalGold = (GET_PLAYER(eTargetPlayer).getGold() * kMission.getStealTreasuryTypes()) / 100;
			int iNumTotalGold
					= 0; // kmodx

			if (NULL != pPlot)
			{
				CvCity* pCity = pPlot->getPlotCity();

				if (NULL != pCity)
				{
					/* iNumTotalGold *= pCity->getPopulation();
					iNumTotalGold /= std::max(1, GET_PLAYER(eTargetPlayer).getTotalPopulation()); */
					// K-Mod. Make stealing gold still worthwhile against large civs.
					iNumTotalGold = getEspionageGoldQuantity(eMission, eTargetPlayer, pCity);
				}
			}

			szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_STEAL_TREASURY",
					iNumTotalGold); // advc.004i
			changeGold(iNumTotalGold);
			if (NO_PLAYER != eTargetPlayer)
				GET_PLAYER(eTargetPlayer).changeGold(-iNumTotalGold);

			bSomethingHappened = true;
		}
	}

	//////////////////////////////
	// Buy (Steal) Tech

	if (kMission.getBuyTechCostFactor() > 0)
	{
		TechTypes eTech = (TechTypes)iExtraData;
		szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_TECH_BOUGHT",
				GC.getInfo(eTech).getDescription()).GetCString();
		GET_TEAM(getTeam()).setHasTech(eTech, true, getID(), false, true);
		if(isSignificantDiscovery(eTech)) // advc.550e
			GET_TEAM(getTeam()).setNoTradeTech(eTech, true);
		bSomethingHappened = true;
	}

	//////////////////////////////
	// Switch Civic

	if (kMission.getSwitchCivicCostFactor() > 0)
	{
		if (eTargetPlayer != NO_PLAYER)
		{
			announceEspionageToThirdParties(eMission, eTargetPlayer); // advc.120f
			int iCivic = iExtraData;
			szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SWITCH_CIVIC",
					GC.getInfo((CivicTypes)iCivic).getDescription()).GetCString();
			GET_PLAYER(eTargetPlayer).setCivics((CivicOptionTypes)GC.getInfo((CivicTypes)iCivic).
					getCivicOptionType(), (CivicTypes)iCivic);
			GET_PLAYER(eTargetPlayer).setRevolutionTimer(std::max(1,
					((100 + GET_PLAYER(eTargetPlayer).getAnarchyModifier()) *
					GC.getDefineINT("MIN_REVOLUTION_TURNS")) / 100));
			bSomethingHappened = true;
		}
	}

	//////////////////////////////
	// Switch Religion

	if (kMission.getSwitchReligionCostFactor() > 0)
	{
		if (NO_PLAYER != eTargetPlayer)
		{
			announceEspionageToThirdParties(eMission, eTargetPlayer); // advc.120f
			int iReligion = iExtraData;
			szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SWITCH_RELIGION",
					GC.getInfo((ReligionTypes) iReligion).getDescription()).GetCString();
			GET_PLAYER(eTargetPlayer).setLastStateReligion((ReligionTypes)iReligion);
			GET_PLAYER(eTargetPlayer).setConversionTimer(std::max(1,
					((100 + GET_PLAYER(eTargetPlayer).getAnarchyModifier()) *
					GC.getDefineINT("MIN_CONVERSION_TURNS")) / 100));
			bSomethingHappened = true;
		}
	}

	//////////////////////////////
	// Player Anarchy

	if (kMission.getPlayerAnarchyCounter() > 0)
	{
		if (NO_PLAYER != eTargetPlayer)
		{
			int iTurns = (kMission.getPlayerAnarchyCounter() * GC.getInfo(GC.getGame().getGameSpeedType()).getAnarchyPercent()) / 100;
			szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_PLAYER_ANARCHY").GetCString();
			GET_PLAYER(eTargetPlayer).changeAnarchyTurns(iTurns);

			bSomethingHappened = true;
		}
	}

	//////////////////////////////
	// Counterespionage

	if (kMission.getCounterespionageNumTurns() > 0 && kMission.getCounterespionageMod() > 0)
	{
		if (NO_TEAM != eTargetTeam)
		{
			szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_COUNTERESPIONAGE").GetCString();
			int iTurns = (kMission.getCounterespionageNumTurns() * GC.getInfo(GC.getGame().getGameSpeedType()).getResearchPercent()) / 100;
			GET_TEAM(getTeam()).changeCounterespionageTurnsLeftAgainstTeam(eTargetTeam, iTurns);
			GET_TEAM(getTeam()).changeCounterespionageModAgainstTeam(eTargetTeam, kMission.getCounterespionageMod());
			// <advc.120>
			if(!bSomethingHappened)
				bAggressiveMission = false; // </advc.120>
			bSomethingHappened = true;

		}
	}

	// <advc.103>
	if(kMission.isInvestigateCity() && pPlot != NULL) {
		CvCity* pCity = pPlot->getPlotCity();
		if(pCity != NULL) {
			pCity->setInvestigate(true);
			gDLL->getInterfaceIFace()->selectCity(pCity);
			bSomethingHappened = true;
		}
	} // </advc.103>

	int iHave = 0;
	if (NO_TEAM != eTargetTeam)
	{
		iHave = GET_TEAM(getTeam()).getEspionagePointsAgainstTeam(eTargetTeam);

		if (bSomethingHappened)
		{
			GET_TEAM(getTeam()).changeEspionagePointsAgainstTeam(eTargetTeam, -iMissionCost);
		}
	}

	if (bShowExplosion)
	{
		if (pPlot)
		{
			if (pPlot->isVisible(GC.getGame().getActiveTeam()) &&
				GC.getGame().getActiveTeam() == getTeam()) // advc.120i
			{
				EffectTypes eEffect = GC.getInfo(GC.getInfo(MISSION_BOMBARD).getEntityEvent()).getEffectType();
				gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
				gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_CITY_EXPLOSION", pPlot->getPoint());
			}
		}
	}

	if (bSomethingHappened)
	{
		int iX = -1;
		int iY = -1;
		if (NULL != pPlot)
		{
			iX = pPlot->getX();
			iY = pPlot->getY();
		}
		// advc.103: The city screen having opened is confirmation enough
		if (!kMission.isInvestigateCity())
		{
			gDLL->UI().addMessage(getID(), true, -1,
					gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_PERFORMED"),
					"AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO,
					ARTFILEMGR.getInterfaceArtPath("ESPIONAGE_BUTTON"),
					GC.getColorType("GREEN"), iX, iY, true, true);
		}
	}
	else if (getID() == GC.getGame().getActivePlayer())
	{
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TEXT);
		if (iHave < iMissionCost)
			pInfo->setText(gDLL->getText("TXT_KEY_ESPIONAGE_TOO_EXPENSIVE", iMissionCost, iHave));
		else pInfo->setText(gDLL->getText("TXT_KEY_ESPIONAGE_CANNOT_DO_MISSION"));
		addPopup(pInfo);
	}

	if (bSomethingHappened && !szBuffer.empty())
	{
		int iX = -1;
		int iY = -1;
		if (NULL != pPlot)
		{
			iX = pPlot->getX();
			iY = pPlot->getY();
		}

		if (NO_PLAYER != eTargetPlayer)
		{	// <advc.120>
			ColorTypes eColor = NO_COLOR;
			if(bAggressiveMission)
				eColor = GC.getColorType("RED");
			// </advc.120>
			gDLL->UI().addMessage(eTargetPlayer, /*true*/ false, // advc.120i
					-1, szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_INFO,
					ARTFILEMGR.getInterfaceArtPath("ESPIONAGE_BUTTON"),
					eColor, iX, iY, true, true);
		}
	}

	return bSomethingHappened;
}

int CvPlayer::getEspionageSpendingWeightAgainstTeam(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiEspionageSpendingWeightAgainstTeam[eIndex];
}

void CvPlayer::setEspionageSpendingWeightAgainstTeam(TeamTypes eIndex, int iValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	//FAssert(iValue >= 0);

	iValue = std::min(std::max(0, iValue), 99);

	if (iValue != getEspionageSpendingWeightAgainstTeam(eIndex))
	{
		m_aiEspionageSpendingWeightAgainstTeam[eIndex] = iValue;

		gDLL->getInterfaceIFace()->setDirty(Espionage_Advisor_DIRTY_BIT, true);
	}
}

void CvPlayer::changeEspionageSpendingWeightAgainstTeam(TeamTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	setEspionageSpendingWeightAgainstTeam(eIndex, getEspionageSpendingWeightAgainstTeam(eIndex) + iChange);
}

void CvPlayer::doAdvancedStartAction(AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd,
	int iData2) // advc.250c
{
	if (getAdvancedStartPoints() < 0)
		return;

	CvPlot* pPlot = GC.getMap().plot(iX, iY);

	if (getNumCities() == 0)
	{
		switch (eAction)
		{
		case ADVANCEDSTARTACTION_EXIT:
			//Try to build this player's empire
			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setBusy(true);
			}
			AI().AI_doAdvancedStart(true);
			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setBusy(false);
			}
			break;
		case ADVANCEDSTARTACTION_AUTOMATE:
		case ADVANCEDSTARTACTION_CITY:
			break;
		default:
			// The first action must be to place a city
			// so players can't lose by spending everything
			return;
		}
	}

	switch (eAction)
	{
	case ADVANCEDSTARTACTION_EXIT:
		changeGold(getAdvancedStartPoints());
		setAdvancedStartPoints(-1);
		if (GC.getGame().getActivePlayer() == getID())
		{
			gDLL->getInterfaceIFace()->setInAdvancedStart(false);
		}

		if (isHuman())
		{
			FOR_EACH_CITY_VAR(pCity, *this)
				pCity->chooseProduction();

			chooseTech();

			if (canRevolution(NULL))
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHANGECIVIC);
				if (NULL != pInfo)
				{
					gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
				}
			}
		}
		break;
	case ADVANCEDSTARTACTION_AUTOMATE:
		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setBusy(true);
		}
		AI().AI_doAdvancedStart(true);
		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setBusy(false);
		}
		break;
	case ADVANCEDSTARTACTION_UNIT:
		{
			if(pPlot == NULL)
				return;

			UnitTypes eUnit = (UnitTypes) iData;
			int iCost = getAdvancedStartUnitCost(eUnit, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add unit to the map
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					CvUnit* pUnit = initUnit(eUnit, iX, iY,
							(UnitAITypes)iData2); // advc.250c
					if (NULL != pUnit)
					{
						pUnit->finishMoves();
						changeAdvancedStartPoints(-iCost);
					}
				}
			}

			// Remove unit from the map
			else
			{
				// If cost is -1 we already know this unit isn't present
				if (iCost != -1)
				{
					CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
					while (pUnitNode != NULL)
					{
						CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pPlot->nextUnitNode(pUnitNode);

						if (pLoopUnit->getUnitType() == eUnit)
						{
							pLoopUnit->kill(false);
							changeAdvancedStartPoints(iCost);
							return;
						}
					}
				}

				// Proper unit not found above, delete first found
				CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
				if (pUnitNode != NULL)
				{
					CvUnit* pUnit = ::getUnit(pUnitNode->m_data);
					iCost = getAdvancedStartUnitCost(pUnit->getUnitType(), false,
							pPlot); // dlph.11
					FAssertMsg(iCost != -1, "If this is -1 then that means it's going to try to delete a unit which shouldn't exist");
					pUnit->kill(false);
					changeAdvancedStartPoints(iCost);
				}
			}

			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_CITY:
		{
			if(pPlot == NULL)
				return;

			int iCost = getAdvancedStartCityCost(bAdd, pPlot);

			if (iCost < 0)
			{
				return;
			}

			// Add City to the map
			if (bAdd)
			{
				if (getNumCities() == 0)
				{
					PlayerTypes eClosestPlayer = NO_PLAYER;
					int iMinDistance = MAX_INT;
					for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
						if (kPlayer.isAlive())
						{
							if (kPlayer.getTeam() == getTeam())
							{
								if (kPlayer.getNumCities() == 0)
								{
									FAssert(kPlayer.getStartingPlot() != NULL);
									int iDistance = plotDistance(iX, iY, kPlayer.getStartingPlot()->getX(), kPlayer.getStartingPlot()->getY());
									if (iDistance < iMinDistance)
									{
										eClosestPlayer = kPlayer.getID();
										iMinDistance = iDistance;
									}
								}
							}
						}
					}
					FAssertMsg(eClosestPlayer != NO_PLAYER, "Self at a minimum should always be valid");
					if (eClosestPlayer != getID())
					{
						CvPlot* pTempPlot = GET_PLAYER(eClosestPlayer).getStartingPlot();
						GET_PLAYER(eClosestPlayer).setStartingPlot(getStartingPlot(), false);
						setStartingPlot(pTempPlot, false);
					}
				}
				if (getAdvancedStartPoints() >= iCost || getNumCities() == 0)
				{
					found(iX, iY);
					changeAdvancedStartPoints(-std::min(iCost, getAdvancedStartPoints()));
					GC.getGame().updateColoredPlots();
					// advc.250c: Commented out
					/*CvCity* pCity = pPlot->getPlotCity();
					if (pCity != NULL)
					{
						if (pCity->getPopulation() > 1)
						{
							pCity->setFood(pCity->growthThreshold() / 2);
						}
					}*/
				}
			}

			// Remove City from the map
			else
			{
				pPlot->setRouteType(NO_ROUTE, true);
				pPlot->getPlotCity()->kill(true);
				pPlot->setImprovementType(NO_IMPROVEMENT);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_POP:
		{
			if(pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL)
			{
				int iCost = getAdvancedStartPopCost(bAdd, pCity);

				if (iCost < 0)
				{
					return;
				}

				bool bPopChanged = false;
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->changePopulation(1);
						changeAdvancedStartPoints(-iCost);
						bPopChanged = true;
					}
				}
				else
				{
					pCity->changePopulation(-1);
					changeAdvancedStartPoints(iCost);
					bPopChanged = true;
				}

				if (bPopChanged)
				{
					pCity->setHighestPopulation(pCity->getPopulation());
					// advc.250c, dlph: Commented out
					/*if (pCity->getPopulation() == 1) {
						pCity->setFood(0);
						pCity->setFoodKept(0);
					}
					else if (pCity->getPopulation() > 1) {
						pCity->setFood(pCity->growthThreshold() / 2);
						pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
					}*/
				}
			}
		}
		break;
	case ADVANCEDSTARTACTION_CULTURE:
		{
			if(pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL)
			{
				int iCost = getAdvancedStartCultureCost(bAdd, pCity);

				if (iCost < 0)
				{
					return;
				}

				// Add Culture to the City
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->setCulture(getID(), pCity->getCultureThreshold(), true, true);
						changeAdvancedStartPoints(-iCost);
					}
				}

				// Remove Culture from the city
				else
				{
					CultureLevelTypes eLevel = (CultureLevelTypes)std::max(0, pCity->getCultureLevel() - 1);
					pCity->setCulture(getID(), CvCity::getCultureThreshold(eLevel), true, true);
					changeAdvancedStartPoints(iCost);
				}
			}
		}
		break;
	case ADVANCEDSTARTACTION_BUILDING:
		{
			if(pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL)
			{
				BuildingTypes eBuilding = (BuildingTypes) iData;
				int iCost = getAdvancedStartBuildingCost(eBuilding, bAdd, pCity);

				if (iCost < 0)
				{
					return;
				}

				// Add Building to the City
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->setNumRealBuilding(eBuilding, pCity->getNumRealBuilding(eBuilding)+1);
						changeAdvancedStartPoints(-iCost);
						if (GC.getInfo(eBuilding).getFoodKept() != 0)
						{
							pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
						}
					}
				}

				// Remove Building from the map
				else
				{
					pCity->setNumRealBuilding(eBuilding, pCity->getNumRealBuilding(eBuilding)-1);
					changeAdvancedStartPoints(iCost);
					if (GC.getInfo(eBuilding).getFoodKept() != 0)
					{
						pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
					}
				}
			}

			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_ROUTE:
		{
			if(pPlot == NULL)
				return;

			RouteTypes eRoute = (RouteTypes) iData;
			int iCost = getAdvancedStartRouteCost(eRoute, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add Route to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					pPlot->setRouteType(eRoute, true);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Route from the Plot
			else
			{
				if (pPlot->getRouteType() != eRoute)
				{
					eRoute = pPlot->getRouteType();
					iCost = getAdvancedStartRouteCost(eRoute, bAdd);
				}

				if (iCost < 0)
				{
					return;
				}

				pPlot->setRouteType(NO_ROUTE, true);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_IMPROVEMENT:
		{
			if(pPlot == NULL)
				return;

			ImprovementTypes eImprovement = (ImprovementTypes) iData;
			int iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add Improvement to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost && pPlot->isFeature())
				{
					FOR_EACH_ENUM(Build)
					{
						ImprovementTypes eLoopImprovement = GC.getInfo(eLoopBuild).getImprovement();
						if (eImprovement != eLoopImprovement)
							continue;
						if (GC.getInfo(eLoopBuild).isFeatureRemove(pPlot->getFeatureType()) &&
							canBuild(pPlot, eLoopBuild))
						{
							pPlot->setFeatureType(NO_FEATURE);
							break;
						}
					}
					pPlot->setImprovementType(eImprovement);
					changeAdvancedStartPoints(-iCost);
  					// < JCultureControl Mod Start >
				    if (eImprovement != NO_IMPROVEMENT && GC.getGame().isOption(GAMEOPTION_CULTURE_CONTROL))
                    {
                        if (GC.getImprovementInfo(eImprovement).isSpreadCultureControl())
                        {
                            pPlot->setImprovementOwner(getID());
                            pPlot->addCultureControl(getID(), eImprovement, true);
                        }
                    }
				    // < JCultureControl Mod End >
				}
			}
			else // Remove Improvement from the Plot
			{
				if (pPlot->getImprovementType() != eImprovement)
				{
					eImprovement = pPlot->getImprovementType();
					iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);
				}
				if (iCost < 0)
					return;

				pPlot->setImprovementType(NO_IMPROVEMENT);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGame().getActivePlayer())
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
		}
		break;
	case ADVANCEDSTARTACTION_TECH:
		{
			TechTypes eTech = (TechTypes) iData;
			int iCost = getAdvancedStartTechCost(eTech, bAdd);
			if (iCost < 0)
				return;

			// Add Tech to team
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					GET_TEAM(getTeam()).setHasTech(eTech, true, getID(), false, false);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Tech from the Team
			else
			{
				GET_TEAM(getTeam()).setHasTech(eTech, false, getID(), false, false);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGame().getActivePlayer())
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
		}
		break;
	case ADVANCEDSTARTACTION_VISIBILITY:
		{
			if(pPlot == NULL)
				return;

			int iCost = getAdvancedStartVisibilityCost(bAdd, pPlot);
			if (iCost < 0)
				return;

			// Add Visibility to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					pPlot->setRevealed(getTeam(), true, true, NO_TEAM, true);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Visibility from the Plot
			else
			{
				pPlot->setRevealed(getTeam(), false, true, NO_TEAM, true);
				changeAdvancedStartPoints(iCost);
			}
		}
		break;
	default:
		FAssert(false);
		break;
	}
}

// Adding or removing a unit
int CvPlayer::getAdvancedStartUnitCost(UnitTypes eUnit, bool bAdd, CvPlot const* pPlot) const
{
	if (getNumCities() <= 0)
		return -1;

	int iCost = (getProductionNeeded(eUnit) *
			//GC.getInfo(eUnit).getAdvancedStartCost() / 100;
			// advc.250c:
			3 * GC.getInfo(eUnit).getAdvancedStartCost()) / 200;

	if (iCost < 0)
		return -1;

	if (pPlot == NULL)
	{
		if (bAdd)
		{
			bool bValid = false;
			FOR_EACH_CITY(pLoopCity, *this)
			{
				if (pLoopCity->canTrain(eUnit))
				{
					bValid = true;
					break;
				}
			}

			if (!bValid)
				return -1;
		}
	}
	else
	{
		CvCity* pCity = NULL;
		if (GC.getDefineINT("ADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES") == 0)
		{
			pCity = pPlot->getPlotCity();
			if (pCity == NULL || pCity->getOwner() != getID())
				return -1;

			iCost *= 100;
			iCost /= std::max(1, 100 + pCity->getProductionModifier(eUnit));
		}
		else
		{
			if (pPlot->getOwner() != getID())
				return -1;

			iCost *= 100;
			iCost /= std::max(1, 100 + getProductionModifier(eUnit));
		}


		if (bAdd)
		{
			int iMaxUnitsPerCity = GC.getDefineINT("ADVANCED_START_MAX_UNITS_PER_CITY");
			if (iMaxUnitsPerCity >= 0)
			{
				if (GC.getInfo(eUnit).isMilitarySupport() &&
					getNumMilitaryUnits() >= iMaxUnitsPerCity * getNumCities())
				{
					return -1;
				}
			}

			if (pCity != NULL)
			{
				if (!pCity->canTrain(eUnit))
					return -1;
			}
			else
			{
				if (!pPlot->canTrain(eUnit, false, false))
					return -1;

				if (pPlot->isImpassable() && !GC.getInfo(eUnit).canMoveImpassable())
					return -1;
				//mountains mod start
				// Deliverator - peak mod
				if (GC.getGame().isOption(GAMEOPTION_MOUNTAINS))
				{
					if (pPlot->isPeak() && !(GC.getUnitInfo(eUnit).isCanMovePeak() || GC.getInfo(eUnit).canMoveImpassable()))
					{
						return -1;
					}	
					// Deliverator				
				}
				//mountains mod end
				if (pPlot->isFeature())
				{
					if (GC.getInfo(eUnit).getFeatureImpassable(pPlot->getFeatureType()))
					{
						TechTypes eTech = GC.getInfo(eUnit).getFeaturePassableTech(
								pPlot->getFeatureType());
						if (eTech == NO_TECH || !GET_TEAM(getTeam()).isHasTech(eTech))
							return -1;
					}
				}
				else
				{
					if (GC.getInfo(eUnit).getTerrainImpassable(pPlot->getTerrainType()))
					{
						TechTypes eTech = GC.getInfo(eUnit).getTerrainPassableTech(pPlot->getTerrainType());
						if (eTech == NO_TECH || !GET_TEAM(getTeam()).isHasTech(eTech))
							return -1;
					}
				}
			}
		}
		// Must be this unit at plot in order to remove
		else
		{
			bool bUnitFound = false;
			for (CLLNode<IDInfo> const* pUnitNode = pPlot->headUnitNode();
				pUnitNode != NULL; pUnitNode = pPlot->nextUnitNode(pUnitNode))
			{
				CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
				if (pLoopUnit->getUnitType() == eUnit)
					bUnitFound = true;
			}
			if (!bUnitFound)
				return -1;
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (GC.getInfo(eUnit).getAdvancedStartCostIncrease() != 0)
	{
		int iUnits = 0;
		FOR_EACH_UNIT(pLoopUnit, *this)
		{
			if (pLoopUnit->getUnitType() == eUnit)
				iUnits++;
		}

		if (!bAdd)
			iUnits--;

		if (iUnits > 0)
		{
			iCost *= 100 + GC.getInfo(eUnit).getAdvancedStartCostIncrease() * iUnits;
			iCost /= 100;
		}
	}

	return adjustAdvStartPtsToSpeed(iCost); // advc.250c
}

// Adding or removing a City
int CvPlayer::getAdvancedStartCityCost(bool bAdd, CvPlot const* pPlot) const
{
	int iNumCities = getNumCities();

	int iCost = getNewCityProductionValue();

	iCost = ::round(1.5 * iCost); // advc.250c

	if (iCost < 0)
		return -1;

	if (pPlot != NULL)
	{
		// Need valid plot to found on if adding
		if (bAdd)
		{
			if (!canFound(pPlot->getX(), pPlot->getY(), false))
				return -1;
		}
		// Need your own city present to remove
		else
		{
			if (pPlot->isCity())
			{
				if (pPlot->getPlotCity()->getOwner() != getID())
					return -1;
			}
			else return -1;
		}

		// Is there a distance limit on how far a city can be placed from a player's start/another city?
		if (GC.getDefineINT("ADVANCED_START_CITY_PLACEMENT_MAX_RANGE") > 0)
		{
			PlayerTypes eClosestPlayer = NO_PLAYER;
			int iClosestDistance = MAX_INT;
			for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
				if (kPlayer.isAlive())
				{
					CvPlot* pStartingPlot = kPlayer.getStartingPlot();
					if (NULL != pStartingPlot)
					{
						int iDistance = ::plotDistance(pPlot->getX(), pPlot->getY(), pStartingPlot->getX(), pStartingPlot->getY());
						if (iDistance <= GC.getDefineINT("ADVANCED_START_CITY_PLACEMENT_MAX_RANGE"))
						{
							if (iDistance < iClosestDistance || (iDistance == iClosestDistance && getTeam() != kPlayer.getTeam()))
							{
								iClosestDistance = iDistance;
								eClosestPlayer = kPlayer.getID();
							}
						}
					}
				}
			}

			if (NO_PLAYER == eClosestPlayer || TEAMID(eClosestPlayer) != getTeam())
				return -1;
			//Only allow founding a city at someone else's start point if
			//We have no cities and they have no cities.
			if (getID() != eClosestPlayer && (getNumCities() > 0 || GET_PLAYER(eClosestPlayer).getNumCities() > 0))
				return -1;
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (GC.getDefineINT("ADVANCED_START_CITY_COST_INCREASE") != 0)
	{
		if (!bAdd)
			--iNumCities;

		if (iNumCities > 0)
		{
			iCost *= 100 + GC.getDefineINT("ADVANCED_START_CITY_COST_INCREASE") * iNumCities;
			iCost /= 100;
		}
	} // <advc.250c>
	if(getNumCities() > 0) // <dlph>
	{
		for(int i = 0; i < GC.getNumUnitInfos(); i++)
		{
			UnitTypes eUnit = (UnitTypes)i;
			if(GC.getInfo(eUnit).isFound())
			{
				iCost *= 100;
				iCost /= std::max(1, 100 + getProductionModifier(eUnit));
				break;
			}
		} // </dlph>
	}
	return adjustAdvStartPtsToSpeed(iCost); // </advc.250c>
}

// Adding or removing Population
int CvPlayer::getAdvancedStartPopCost(bool bAdd, CvCity const* pCity) const
{
	if (getNumCities() <= 0)
		return -1;

	int iCost = (getGrowthThreshold(1) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;

	if (pCity != NULL)
	{
		if (pCity->getOwner() != getID())
			return -1;
		int iPopulation = pCity->getPopulation();
		// Need to have Population to remove it
		if (!bAdd)
		{
			iPopulation--;
			if (iPopulation < GC.getDefineINT("INITIAL_CITY_POPULATION") +
				GC.getInfo(GC.getGame().getStartEra()).getFreePopulation())
			{
				return -1;
			}
		}

		iCost = (getGrowthThreshold(iPopulation) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;

		// Increase cost if the XML defines that additional Pop will cost more
		if (GC.getDefineINT("ADVANCED_START_POPULATION_COST_INCREASE") != 0)
		{
			iPopulation--;
			if (iPopulation > 0)
			{
				iCost *= 100 + GC.getDefineINT("ADVANCED_START_POPULATION_COST_INCREASE") * iPopulation;
				iCost /= 100;
			}
		}
	}

	return adjustAdvStartPtsToSpeed(iCost); // advc.250c
}

// Adding or removing Culture
int CvPlayer::getAdvancedStartCultureCost(bool bAdd, CvCity const* pCity) const
{	// <advc>
	if(getNumCities() == 0)
		return -1;
	int iCost = GC.getDefineINT("ADVANCED_START_CULTURE_COST");
	if(iCost < 0)
		return -1;
	if(pCity == NULL)
		return adjustAdvStartPtsToSpeed(iCost); // advc.250c
	if(pCity->getOwner() != getID())
		return -1; // </advc>

	// Need to have enough culture to remove it
	if(!bAdd && pCity->getCultureLevel() <= 0)
		return -1;

	int iCulture=-1;
	if (bAdd)
	{
		iCulture = CvCity::getCultureThreshold((CultureLevelTypes)
				(pCity->getCultureLevel() + 1)) - pCity->getCulture(getID());
	}
	else
	{
		iCulture = pCity->getCulture(getID()) - CvCity::getCultureThreshold((CultureLevelTypes)
				(pCity->getCultureLevel() - 1));
	}
	iCost *= iCulture;
	iCost /= std::max(1, GC.getInfo(GC.getGame().getGameSpeedType()).
			getHurryPercent());
	return adjustAdvStartPtsToSpeed(iCost); // advc.250c
}

// Adding or removing a Building from a city
int CvPlayer::getAdvancedStartBuildingCost(BuildingTypes eBuilding, bool bAdd, CvCity const* pCity) const
{
	if (getNumCities() <= 0)
		return -1;

	int iCost = (getProductionNeeded(eBuilding) * GC.getInfo(eBuilding).getAdvancedStartCost()) / 100;
	iCost = ::round(iCost * 1.5); // advc.250c
	if (iCost < 0)
		return -1;

	if (GC.getGame().isFreeStartEraBuilding(eBuilding)) // advc
	{
		// you get this building for free
		return -1;
	}

	if (NULL == pCity)
	{
		if (bAdd)
		{
			bool bValid = false;
			FOR_EACH_CITY(pLoopCity, *this)
			{
				if (pLoopCity->canConstruct(eBuilding))
				{
					bValid = true;
					break;
				}
			}
			if (!bValid)
				return -1;
		}
	}
	if (NULL != pCity)
	{
		if (pCity->getOwner() != getID())
			return -1;

		iCost *= 100;
		iCost /= std::max(1, 100 + pCity->getProductionModifier(eBuilding));

		if (bAdd)
		{
			if (!pCity->canConstruct(eBuilding, true, false, false))
				return -1;
		}
		else
		{
			if (pCity->getNumRealBuilding(eBuilding) <= 0)
				return -1;

			// Check other buildings in this city and make sure none of them require this one ...

			// Loop through Buildings to see which are present
			FOR_EACH_ENUM(Building)
			{
				if (pCity->getNumBuilding(eLoopBuilding) > 0)
				{
					// Loop through present Building's requirements
					for (int iBuildingClassPrereqLoop = 0; iBuildingClassPrereqLoop <
						GC.getNumBuildingClassInfos(); iBuildingClassPrereqLoop++)
					{
						if (GC.getInfo(eLoopBuilding).isBuildingClassNeededInCity(iBuildingClassPrereqLoop))
						{
							if ((getCivilization().getBuilding((BuildingClassTypes)
								iBuildingClassPrereqLoop)) == eBuilding)
							{
								return -1;
							}
						}
					}
				}
			}
		}
	}

	// Increase cost if the XML defines that additional Buildings will cost more
	if (GC.getInfo(eBuilding).getAdvancedStartCostIncrease() != 0)
	{
		int iBuildings = countNumBuildings(eBuilding);
		if (!bAdd)
			iBuildings--;

		if (iBuildings > 0)
		{
			iCost *= 100 + GC.getInfo(eBuilding).getAdvancedStartCostIncrease() *
					std::max(0, iBuildings - getNumCities());
			iCost /= 100;
		}
	}

	return adjustAdvStartPtsToSpeed(iCost); // advc.250c
}

// Adding or removing Route
int CvPlayer::getAdvancedStartRouteCost(RouteTypes eRoute, bool bAdd, CvPlot const* pPlot) const
{
	if (getNumCities() <= 0 || eRoute == NO_ROUTE)
		return -1;

	int iCost = GC.getInfo(eRoute).getAdvancedStartCost();
	if (iCost < 0)
		return -1; // cannot be purchased through Advanced Start

	// <advc.250c>
	iCost *= GC.getDefineINT("ADVANCED_START_WORKER_BUILD_MODIFIER");
	iCost /= 100; // </advc.250c>

	iCost *= GC.getInfo(GC.getGame().getGameSpeedType()).getBuildPercent();
	iCost /= 100;

	if (pPlot != NULL)
	{
		if (pPlot->isCity())
			return -1;

		if (bAdd)
		{
			if (pPlot->isImpassable() || pPlot->isWater())
				return -1;
			// Can't place twice
			if (pPlot->getRouteType() == eRoute)
				return -1;
		}
		else
		{
			// Need Route to remove it
			if (pPlot->getRouteType() != eRoute)
				return -1;
		}

		if (pPlot->getOwner() != getID())
			return -1;
	}

	// Tech requirement
	FOR_EACH_ENUM(Build)
	{
		if (GC.getInfo(eLoopBuild).getRoute() == eRoute)
		{
			if (!GET_TEAM(getTeam()).isHasTech(GC.getInfo(eLoopBuild).getTechPrereq()))
				return -1;
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (GC.getInfo(eRoute).getAdvancedStartCostIncrease() != 0)
	{
		int iNumRoutes = 0;
		for (int i = 0; i < GC.getMap().numPlots(); i++)
		{
			CvPlot const& kLoopPlot = GC.getMap().getPlotByIndex(i);
			// <advc.001>
			if (kLoopPlot.getOwner() != getID())
				continue; // </advc.001>
			if (kLoopPlot.getRouteType() == eRoute)
				iNumRoutes++;
		}
		if (!bAdd)
			iNumRoutes--;

		if (iNumRoutes > 0)
		{
			iCost *= 100 + GC.getInfo(eRoute).getAdvancedStartCostIncrease() * iNumRoutes;
			iCost /= 100;
		}
	}

	return adjustAdvStartPtsToSpeed(iCost); // advc.250c
}

// Adding or removing Improvement
int CvPlayer::getAdvancedStartImprovementCost(ImprovementTypes eImprovement, bool bAdd, CvPlot const* pPlot) const
{
	if (eImprovement == NO_IMPROVEMENT || getNumCities() <= 0)
		return -1;

	int iNumImprovements = 0;
	int iCost = GC.getInfo(eImprovement).getAdvancedStartCost();
	if (iCost < 0)
		return -1; // Cannot be purchased through Advanced Start

	// <advc.250c>
	iCost *= GC.getDefineINT("ADVANCED_START_WORKER_BUILD_MODIFIER");
	iCost /= 100; // </advc.250c>

	iCost *= GC.getInfo(GC.getGame().getGameSpeedType()).getBuildPercent();
	iCost /= 100;

	// Can this Improvement be on our plot?
	if (pPlot != NULL)
	{
		if (bAdd)
		{
			if (!pPlot->canHaveImprovement(eImprovement, getTeam(), false))
				return -1;

			bool bValid = false;

			FOR_EACH_ENUM(Build)
			{
				CvBuildInfo const& kBuild = GC.getInfo(eLoopBuild);
				ImprovementTypes eLoopImprovement = (ImprovementTypes)kBuild.getImprovement();
				if (eImprovement == eLoopImprovement && canBuild(pPlot, eLoopBuild))
				{
					bValid = true;
					FeatureTypes eFeature = pPlot->getFeatureType();
					if (eFeature != NO_FEATURE && kBuild.isFeatureRemove(eFeature))
						iCost += GC.getInfo(eFeature).getAdvancedStartRemoveCost();
					break;
				}
			}

			if (!bValid)
				return -1;

			// Can't place twice
			if (pPlot->getImprovementType() == eImprovement)
				return -1;
		}
		else
		{
			// Need this improvement in order to remove it
			if (pPlot->getImprovementType() != eImprovement)
				return -1;
		}

		// Must be owned by me
		if (pPlot->getOwner() != getID())
			return -1;
	}

	// Tech requirement
	FOR_EACH_ENUM(Build)
	{
		if (GC.getInfo(eLoopBuild).getImprovement() == eImprovement)
		{
			if (!GET_TEAM(getTeam()).isHasTech(GC.getInfo(eLoopBuild).getTechPrereq()))
				return -1;
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (GC.getInfo(eImprovement).getAdvancedStartCostIncrease() != 0)
	{
		for (int i = 0; i < GC.getMap().numPlots(); i++)
		{
			CvPlot const& kLoopPlot = GC.getMap().getPlotByIndex(i);
			// <advc.001>
			if (kLoopPlot.getOwner() != getID())
				continue; // </advc.001>
			if (kLoopPlot.getImprovementType() == eImprovement)
				iNumImprovements++;
		}
		if (!bAdd)
			iNumImprovements--;

		if (iNumImprovements > 0)
		{
			iCost *= 100 + GC.getInfo(eImprovement).getAdvancedStartCostIncrease() * iNumImprovements;
			iCost /= 100;
		}
	}

	return adjustAdvStartPtsToSpeed(iCost); // advc.250c
}

// Adding or removing Tech
int CvPlayer::getAdvancedStartTechCost(TechTypes eTech, bool bAdd) const 
{
	if (eTech == NO_TECH || getNumCities() <= 0)
		return -1;

	int iCost = (GET_TEAM(getTeam()).getResearchCost(eTech) * GC.getInfo(eTech).getAdvancedStartCost()) / 100;
	if (iCost < 0)
		return -1;

	if (bAdd)
	{
		if (!canResearch(eTech, false))
			return -1;
	}
	else if (!bAdd)
	{
		if (!GET_TEAM(getTeam()).isHasTech(eTech))
			return -1;

		// Search through all techs to see if any of the currently owned ones requires this tech
		FOR_EACH_ENUM(Tech)
		{
			if (GET_TEAM(getTeam()).isHasTech(eLoopTech))
			{
				for (int iPrereqLoop = 0; iPrereqLoop < GC.getNUM_OR_TECH_PREREQS(); iPrereqLoop++)
				{
					if (GC.getInfo(eLoopTech).getPrereqOrTechs(iPrereqLoop) == eTech)
						return -1;
				}
				for (int iPrereqLoop = 0; iPrereqLoop < GC.getNUM_AND_TECH_PREREQS(); iPrereqLoop++)
				{
					if (GC.getInfo(eLoopTech).getPrereqAndTechs(iPrereqLoop) == eTech)
						return -1;
				}
			}
		}

		// If player has placed anything on the map which uses this tech then you cannot remove it

		FOR_EACH_UNIT(pLoopUnit, *this)
		{
			if (pLoopUnit->getUnitInfo().getPrereqAndTech() == eTech)
				return -1;

			for (int i = 0; i < GC.getNUM_UNIT_AND_TECH_PREREQS(); i++)
			{
				if (pLoopUnit->getUnitInfo().getPrereqAndTechs(i) == eTech)
					return -1;
			}
		}

		FOR_EACH_CITY(pLoopCity, *this)
		{
			FOR_EACH_ENUM(Building)
			{
				if (pLoopCity->getNumRealBuilding(eLoopBuilding) > 0)
				{
					if (GC.getInfo(eLoopBuilding).getPrereqAndTech() == eTech)
						return -1;

					for (int iI = 0; iI < GC.getNUM_BUILDING_AND_TECH_PREREQS(); iI++)
					{
						if (GC.getInfo(eLoopBuilding).getPrereqAndTechs(iI) == eTech)
							return -1;
					}
				}
			}
		}

	}

	// Increase cost if the XML defines that additional units will cost more
	if (GC.getInfo(eTech).getAdvancedStartCostIncrease() != 0)
	{
		int iTechs = 0;
		FOR_EACH_ENUM(Tech)
		{
			if (GET_TEAM(getTeam()).isHasTech(eLoopTech))
				iTechs++;
		}

		if (!bAdd)
			iTechs--;

		if (iTechs > 0)
		{
			iCost *= 100 + GC.getInfo(eTech).getAdvancedStartCostIncrease() * iTechs;
			iCost /= 100;
		}
	}

	return adjustAdvStartPtsToSpeed(iCost); // advc.250c
}

// Adding or removing Visibility
int CvPlayer::getAdvancedStartVisibilityCost(bool bAdd, CvPlot const* pPlot) const
{
	if (getNumCities() <= 0)
		return -1;

	int iCost = GC.getDefineINT("ADVANCED_START_VISIBILITY_COST");
	// This denotes Visibility may not be purchased through Advanced Start
	if (iCost == -1)
		return -1;

	// Valid Plot?
	if(pPlot != NULL)
	{
		if(bAdd)
		{
			if(pPlot->isRevealed(getTeam()))
				return -1;
			if(!pPlot->isAdjacentRevealed(getTeam(),
					GC.getGame().getStartEra() < 4)) // advc.250c
				return -1;
		}
		else if (!pPlot->isRevealed(getTeam()))
			return -1;
	}

	// Increase cost if the XML defines that additional units will cost more
	if (GC.getDefineINT("ADVANCED_START_VISIBILITY_COST_INCREASE") != 0)
	{
		int iVisible = -NUM_CITY_PLOTS; // advc.210c
		for (int i = 0; i < GC.getMap().numPlots(); i++)
		{
			CvPlot const& kLoopPlot = GC.getMap().getPlotByIndex(i);
			if (kLoopPlot.isRevealed(getTeam()))
				iVisible++;
		}

		if (!bAdd)
			iVisible--;

		if (iVisible > 0)
		{
			iCost *= 100 + GC.getDefineINT("ADVANCED_START_VISIBILITY_COST_INCREASE") * iVisible;
			iCost /= 100;
		}
	}

	return adjustAdvStartPtsToSpeed(iCost); // advc.250c
}

// <advc.250c> Should arguably be at CvGame, but more convenient to use at CvPlayer.
int CvPlayer::adjustAdvStartPtsToSpeed(int iPoints)
{
	iPoints *= 100;
	return std::max(0, iPoints / GC.getInfo(GC.getGame().getGameSpeedType()).
			getGrowthPercent());
} // </advc.250c>


void CvPlayer::doWarnings()
{
	//update enemy unit in your territory glow
	FOR_EACH_UNIT_VAR(pLoopUnit, *this)
	{
		//update glow
		gDLL->getEntityIFace()->updateEnemyGlow(pLoopUnit->getUnitEntity());
	}

	//update enemy units close to your territory
	int iMaxCount = range(((getNumCities() + 4) / 7), 2, 5);
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		if (iMaxCount == 0)
			break;

		CvPlot const& kPlot = GC.getMap().getPlotByIndex(iI);
		if (kPlot.isAdjacentPlayer(getID()) && !kPlot.isCity() &&
			kPlot.isVisible(getTeam()))
		{
			CvUnit const* pUnit = kPlot.getVisibleEnemyDefender(getID());
			if (pUnit != NULL && !pUnit->isAnimal())
			{
				CvCity* pNearestCity = GC.getMap().findCity(kPlot.getX(), kPlot.getY(),
						getID(), NO_TEAM, !kPlot.isWater());
				if (pNearestCity != NULL)
				{
					wchar szBuffer[1024];
					swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_ENEMY_TROOPS_SPOTTED",
							pNearestCity->getNameKey()).GetCString());
					gDLL->UI().addMessage(getID(), true, -1, szBuffer, kPlot,
							"AS2D_ENEMY_TROOPS", MESSAGE_TYPE_INFO, pUnit->getButton(),
							GC.getColorType("RED"));
					iMaxCount--;
				}
			}
		}
	} // <advc.002e>
	if(GC.IsGraphicsInitialized())
		showForeignPromoGlow(true); // </advc.002e>
}


void CvPlayer::verifyGoldCommercePercent()
{
	/*while ((getGold() + calculateGoldRate()) < 0) {
		changeCommercePercent(COMMERCE_GOLD, GC.getDefineINT("COMMERCE_PERCENT_CHANGE_INCREMENTS"));
		if (getCommercePercent(COMMERCE_GOLD) == 100)
			break;
	}*/ // BtS
	// K-Mod
	bool bValid = isCommerceFlexible(COMMERCE_GOLD);
	while (bValid && getCommercePercent(COMMERCE_GOLD) < 100 && getGold() + calculateGoldRate() < 0)
	{
		bValid = changeCommercePercent(COMMERCE_GOLD, GC.getDefineINT("COMMERCE_PERCENT_CHANGE_INCREMENTS"));
	} // K-Mod end
}

/*  <advc.003y> Ported to C++ from CvGameUtils.py; callback disabled by default.
	On-screen message to be handled by the caller; hence the return value. */
int CvPlayer::doCaptureGold(CvCity const& kOldCity) // "old": city still fully intact
{
	int iCaptureGold = 0;
	if (!GC.getPythonCaller()->captureGold(kOldCity, iCaptureGold))
	{
		FAssert(iCaptureGold == 0);
		static int const iBASE_CAPTURE_GOLD = GC.getDefineINT("BASE_CAPTURE_GOLD");
		static int const iCAPTURE_GOLD_PER_POPULATION = GC.getDefineINT("CAPTURE_GOLD_PER_POPULATION");
		static int const iCAPTURE_GOLD_RAND1 = GC.getDefineINT("CAPTURE_GOLD_RAND1");
		static int const iCAPTURE_GOLD_RAND2 = GC.getDefineINT("CAPTURE_GOLD_RAND2");
		static int const iCAPTURE_GOLD_MAX_TURNS = GC.getDefineINT("CAPTURE_GOLD_MAX_TURNS");
		iCaptureGold += iBASE_CAPTURE_GOLD;
		iCaptureGold += kOldCity.getPopulation() * iCAPTURE_GOLD_PER_POPULATION;
		CvGame& g = GC.getGame();
		iCaptureGold += g.getSorenRandNum(iCAPTURE_GOLD_RAND1, "Capture Gold 1");
		iCaptureGold += g.getSorenRandNum(iCAPTURE_GOLD_RAND2, "Capture Gold 2");
		if (iCAPTURE_GOLD_MAX_TURNS > 0)
		{
			iCaptureGold *= ::range(g.getGameTurn() - kOldCity.getGameTurnAcquired(),
					0, iCAPTURE_GOLD_MAX_TURNS);
			iCaptureGold /= iCAPTURE_GOLD_MAX_TURNS;
		}
	}
	changeGold(iCaptureGold);
	return iCaptureGold;
} // </advc.003y>


void CvPlayer::processCivics(CivicTypes eCivic, int iChange)
{
	//DPII < Maintenance Modifiers >
    CvArea* pLoopArea = NULL;

    int iLoop2 = 0;
	//keldath changed iloop to iloop2 cause theres alreayd one for civics plus
	// i dony know if it will work.......so.....
	//int iLoop = 0;
    //DPII < Maintenance Modifiers >
	int iI, iJ;
	// < Civic Infos Plus Start >
	CvCity* pLoopCity;
	int iLoop;
	// < Civic Infos Plus End   >
	changeGreatPeopleRateModifier(GC.getInfo(eCivic).getGreatPeopleRateModifier() * iChange);
	changeGreatGeneralRateModifier(GC.getInfo(eCivic).getGreatGeneralRateModifier() * iChange);
	changeDomesticGreatGeneralRateModifier(GC.getInfo(eCivic).getDomesticGreatGeneralRateModifier() * iChange);
	changeStateReligionGreatPeopleRateModifier(GC.getInfo(eCivic).getStateReligionGreatPeopleRateModifier() * iChange);
	changeDistanceMaintenanceModifier(GC.getInfo(eCivic).getDistanceMaintenanceModifier() * iChange);
	changeNumCitiesMaintenanceModifier(GC.getInfo(eCivic).getNumCitiesMaintenanceModifier() * iChange);
	changeCorporationMaintenanceModifier(GC.getInfo(eCivic).getCorporationMaintenanceModifier() * iChange);
	changeExtraHealth(GC.getInfo(eCivic).getExtraHealth() * iChange);
	changeExtraHappiness(GC.getInfo(eCivic).getExtraHappiness() * iChange); // K-Mod
	changeFreeExperience(GC.getInfo(eCivic).getFreeExperience() * iChange);
	changeWorkerSpeedModifier(GC.getInfo(eCivic).getWorkerSpeedModifier() * iChange);
	changeImprovementUpgradeRateModifier(GC.getInfo(eCivic).getImprovementUpgradeRateModifier() * iChange);
	changeMilitaryProductionModifier(GC.getInfo(eCivic).getMilitaryProductionModifier() * iChange);
	changeBaseFreeUnits(GC.getInfo(eCivic).getBaseFreeUnits() * iChange);
	changeBaseFreeMilitaryUnits(GC.getInfo(eCivic).getBaseFreeMilitaryUnits() * iChange);
	changeFreeUnitsPopulationPercent(GC.getInfo(eCivic).getFreeUnitsPopulationPercent() * iChange);
	changeFreeMilitaryUnitsPopulationPercent(GC.getInfo(eCivic).getFreeMilitaryUnitsPopulationPercent() * iChange);
	changeGoldPerUnit(GC.getInfo(eCivic).getGoldPerUnit() * iChange);
	changeGoldPerMilitaryUnit(GC.getInfo(eCivic).getGoldPerMilitaryUnit() * iChange);
	changeHappyPerMilitaryUnit(GC.getInfo(eCivic).getHappyPerMilitaryUnit() * iChange);
	// <advc.912c>
	changeLuxuryModifier(GC.getInfo(eCivic).getLuxuryModifier() * iChange);
	if(GC.getInfo(eCivic).getLuxuryModifier() != 0)
		AI().AI_updateBonusValue(); // </advc.912c>
	changeMilitaryFoodProductionCount((GC.getInfo(eCivic).isMilitaryFoodProduction()) ? iChange : 0);
	changeMaxConscript(GC.getGame().getMaxConscript(eCivic) * iChange);
	//changeNoUnhealthyPopulationCount((GC.getInfo(eCivic).isNoUnhealthyPopulation()) ? iChange : 0);
	changeUnhealthyPopulationModifier(GC.getInfo(eCivic).getUnhealthyPopulationModifier() * iChange); // K-Mod
	changeBuildingOnlyHealthyCount((GC.getInfo(eCivic).isBuildingOnlyHealthy()) ? iChange : 0);
	changeLargestCityHappiness(GC.getInfo(eCivic).getLargestCityHappiness() * iChange);
	changeWarWearinessModifier(GC.getInfo(eCivic).getWarWearinessModifier() * iChange);
	changeFreeSpecialist(GC.getInfo(eCivic).getFreeSpecialist() * iChange);
	changeTradeRoutes(GC.getInfo(eCivic).getTradeRoutes() * iChange);
	changeNoForeignTradeCount(GC.getInfo(eCivic).isNoForeignTrade() * iChange);
	changeNoCorporationsCount(GC.getInfo(eCivic).isNoCorporations() * iChange);
	changeNoForeignCorporationsCount(GC.getInfo(eCivic).isNoForeignCorporations() * iChange);
	changeStateReligionCount((GC.getInfo(eCivic).isStateReligion()) ? iChange : 0);
	changeNoNonStateReligionSpreadCount((GC.getInfo(eCivic).isNoNonStateReligionSpread()) ? iChange : 0);
	changeStateReligionHappiness(GC.getInfo(eCivic).getStateReligionHappiness() * iChange);
	changeNonStateReligionHappiness(GC.getInfo(eCivic).getNonStateReligionHappiness() * iChange);
	//DPII < Maintenance Modifiers >
		if ((GC.getInfo(eCivic).getOtherAreaMaintenanceModifier() != 0) || (GC.getInfo(eCivic).getHomeAreaMaintenanceModifier() != 0))
		{
			for (pLoopArea = GC.getMap().firstArea(&iLoop2); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop2))
		//keldath changed iloop to iloop2 cause theres alreayd one for civics plus
		//for (pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
			{
				if (pLoopArea->isHomeArea(getID()))
				{
					pLoopArea->changeHomeAreaMaintenanceModifier(getID(), (GC.getInfo(eCivic).getHomeAreaMaintenanceModifier()  * iChange));
				}
				else
				{
					pLoopArea->changeOtherAreaMaintenanceModifier(getID(), (GC.getInfo(eCivic).getOtherAreaMaintenanceModifier()  * iChange));
				}
			}
		}
	//DPII < Maintenance Modifiers >
	
	// < Civic Infos Plus Start >
	changeStateReligionExtraHealth(GC.getCivicInfo(eCivic).getStateReligionExtraHealth() * iChange);
	changeNonStateReligionExtraHealth(GC.getCivicInfo(eCivic).getNonStateReligionExtraHealth() * iChange);
	// < Civic Infos Plus End   >
	changeStateReligionUnitProductionModifier(GC.getInfo(eCivic).getStateReligionUnitProductionModifier() * iChange);
	changeStateReligionBuildingProductionModifier(GC.getInfo(eCivic).getStateReligionBuildingProductionModifier() * iChange);
	changeStateReligionFreeExperience(GC.getInfo(eCivic).getStateReligionFreeExperience() * iChange);
	changeExpInBorderModifier(GC.getInfo(eCivic).getExpInBorderModifier() * iChange);

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		 // < Civic Infos Plus Start >
		changeStateReligionYieldRateModifier(((YieldTypes)iI), (GC.getInfo(eCivic).getStateReligionYieldModifier(iI) * iChange));
		changeNonStateReligionYieldRateModifier(((YieldTypes)iI), (GC.getInfo(eCivic).getNonStateReligionYieldModifier(iI) * iChange));

		for (iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
        {
            changeSpecialistExtraYield((SpecialistTypes) iJ, (YieldTypes)iI, (GC.getInfo(eCivic).getSpecialistExtraYield(iI) * iChange));
        }
		// < Civic Infos Plus End   >
		changeYieldRateModifier(((YieldTypes)iI), (GC.getInfo(eCivic).getYieldModifier(iI) * iChange));
		changeCapitalYieldRateModifier(((YieldTypes)iI), (GC.getInfo(eCivic).getCapitalYieldModifier(iI) * iChange));
		changeTradeYieldModifier(((YieldTypes)iI), (GC.getInfo(eCivic).getTradeYieldModifier(iI) * iChange));
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		// < Civic Infos Plus Start >
		changeStateReligionCommerceRateModifier(((CommerceTypes)iI), (GC.getInfo(eCivic).getStateReligionCommerceModifier(iI) * iChange));
		changeNonStateReligionCommerceRateModifier(((CommerceTypes)iI), (GC.getInfo(eCivic).getNonStateReligionCommerceModifier(iI) * iChange));
		// < Civic Infos Plus End   >
		changeCommerceRateModifier(((CommerceTypes)iI), (GC.getInfo(eCivic).getCommerceModifier(iI) * iChange));
		changeCapitalCommerceRateModifier(((CommerceTypes)iI), (GC.getInfo(eCivic).getCapitalCommerceModifier(iI) * iChange));
		changeSpecialistExtraCommerce(((CommerceTypes)iI), (GC.getInfo(eCivic).getSpecialistExtraCommerce(iI) * iChange));
	} // <advc.003t>
	if (GC.getInfo(eCivic).isAnyBuildingHappinessChanges() ||
		GC.getInfo(eCivic).isAnyBuildingHealthChanges()) // </advc.003t>
	{
		CvCivilization const& kCiv = getCivilization(); // advc.003w
		for (int i = 0; i < kCiv.getNumBuildings(); i++)
		{
			// < Civic Infos Plus Start >
	    	for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
			changeBuildingYieldChange(((BuildingTypes)iI), ((YieldTypes)iJ), (GC.getInfo(eCivic).getBuildingYieldChanges(iI, iJ) * iChange));
			}

			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				changeBuildingCommerceChange(((BuildingTypes)iI), ((CommerceTypes)iJ), (GC.getInfo(eCivic).getBuildingCommerceChanges(iI, iJ) * iChange));
			}
			// < Civic Infos Plus End   >
			BuildingTypes eOurBuilding = kCiv.buildingAt(i);
			BuildingClassTypes eLoopClass = kCiv.buildingClassAt(i);
			changeExtraBuildingHappiness(eOurBuilding, GC.getInfo(eCivic).getBuildingHappinessChanges(eLoopClass) * iChange);
			changeExtraBuildingHealth(eOurBuilding, GC.getInfo(eCivic).getBuildingHealthChanges(eLoopClass) * iChange);
		}
	}

	for (iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		changeFeatureHappiness(((FeatureTypes)iI), (GC.getInfo(eCivic).getFeatureHappinessChanges(iI) * iChange));
	}

	for (iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		changeHurryCount(((HurryTypes)iI), ((GC.getInfo(eCivic).isHurry(iI)) ? iChange : 0));
	}

	for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
	{
		changeSpecialBuildingNotRequiredCount(((SpecialBuildingTypes)iI), ((GC.getInfo(eCivic).isSpecialBuildingNotRequired(iI)) ? iChange : 0));
	}

	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		changeSpecialistValidCount(((SpecialistTypes)iI), ((GC.getInfo(eCivic).isSpecialistValid(iI)) ? iChange : 0));
	}
// < Civic Infos Plus Start >
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for(iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			pLoopCity->changeFreeSpecialistCount((SpecialistTypes)iI, (GC.getCivicInfo(eCivic).getFreeSpecialistCount(iI) * iChange));
		}
		pLoopCity->updateBuildingCommerceChange(eCivic, iChange);
		pLoopCity->updateBuildingYieldChange(eCivic, iChange);
	}
// < Civic Infos Plus End   >
	for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (GC.getInfo(eCivic).getImprovementYieldChanges(iI, iJ) * iChange));
		}
	}
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**																								**/
	/**																								**/
	/*************************************************************************************************/	
	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeSpecialistExtraYield(((SpecialistTypes)iI), ((YieldTypes)iJ), (GC.getCivicInfo(eCivic).getSpecialistYieldChange(iI, iJ) * iChange));
		}
	}

	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			changeSpecialistCivicExtraCommerce(((SpecialistTypes)iI), ((CommerceTypes)iJ), (GC.getCivicInfo(eCivic).getSpecialistCommerceChange(iI, iJ) * iChange));
		}
	}
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/
}

void CvPlayer::showMissedMessages()
{
	CvMessageQueue::iterator it = m_listGameMessages.begin();
	while (it != m_listGameMessages.end())
	{
		CvTalkingHeadMessage& msg = *it;
		if (!msg.getShown())
		{
			msg.setShown(true);
			gDLL->getInterfaceIFace()->showMessage(msg);
		}
		++it;
	}
}

bool CvPlayer::isPbemNewTurn() const
{
	return m_bPbemNewTurn;
}

void CvPlayer::setPbemNewTurn(bool bNew)
{
	m_bPbemNewTurn = bNew;
}

// read object from a stream. used during load
void CvPlayer::read(FDataStreamBase* pStream)
{
	int iI=0;

	reset();

	uint uiFlag=0;
	pStream->Read(&uiFlag);

	pStream->Read(&m_iStartingX);
	pStream->Read(&m_iStartingY);
	pStream->Read(&m_iTotalPopulation);
	pStream->Read(&m_iTotalLand);
	pStream->Read(&m_iTotalLandScored);
	pStream->Read(&m_iGold);
	pStream->Read(&m_iGoldPerTurn);
	pStream->Read(&m_iAdvancedStartPoints);
	pStream->Read(&m_iGoldenAgeTurns);
	// <advc.001x>
	if (uiFlag >= 11)
		pStream->Read(&m_iScheduledGoldenAges); // </advc.001x>
	pStream->Read(&m_iNumUnitGoldenAges);
	pStream->Read(&m_iStrikeTurns);
	pStream->Read(&m_iAnarchyTurns);
	pStream->Read(&m_iMaxAnarchyTurns);
	pStream->Read(&m_iAnarchyModifier);
	pStream->Read(&m_iGoldenAgeModifier);
	pStream->Read(&m_iGlobalHurryModifier);
	pStream->Read(&m_iGreatPeopleCreated);
	pStream->Read(&m_iGreatGeneralsCreated);
	pStream->Read(&m_iGreatPeopleThresholdModifier);
	pStream->Read(&m_iGreatGeneralsThresholdModifier);
	pStream->Read(&m_iGreatPeopleRateModifier);
	pStream->Read(&m_iGreatGeneralRateModifier);
	pStream->Read(&m_iDomesticGreatGeneralRateModifier);
	pStream->Read(&m_iStateReligionGreatPeopleRateModifier);
	pStream->Read(&m_iMaxGlobalBuildingProductionModifier);
	pStream->Read(&m_iMaxTeamBuildingProductionModifier);
	pStream->Read(&m_iMaxPlayerBuildingProductionModifier);
	pStream->Read(&m_iFreeExperience);
	pStream->Read(&m_iFeatureProductionModifier);
	pStream->Read(&m_iWorkerSpeedModifier);
	pStream->Read(&m_iImprovementUpgradeRateModifier);
	pStream->Read(&m_iMilitaryProductionModifier);
	pStream->Read(&m_iSpaceProductionModifier);
	pStream->Read(&m_iCityDefenseModifier);
	pStream->Read(&m_iNumNukeUnits);
	pStream->Read(&m_iNumOutsideUnits);
	pStream->Read(&m_iBaseFreeUnits);
	pStream->Read(&m_iBaseFreeMilitaryUnits);
	pStream->Read(&m_iFreeUnitsPopulationPercent);
	pStream->Read(&m_iFreeMilitaryUnitsPopulationPercent);
	pStream->Read(&m_iGoldPerUnit);
	pStream->Read(&m_iGoldPerMilitaryUnit);
	// K-Mod
	if (uiFlag < 3)
	{
		m_iGoldPerUnit *= 100;
		m_iGoldPerMilitaryUnit *= 100;
	}
	// K-Mod end
	pStream->Read(&m_iExtraUnitCost);
	pStream->Read(&m_iNumMilitaryUnits);
	pStream->Read(&m_iHappyPerMilitaryUnit);
	// <advc.912c>
	if(uiFlag >= 6)
		pStream->Read(&m_iLuxuryModifier); // </advc.912c>
	pStream->Read(&m_iMilitaryFoodProductionCount);
	pStream->Read(&m_iConscriptCount);
	pStream->Read(&m_iMaxConscript);
	pStream->Read(&m_iHighestUnitLevel);
	pStream->Read(&m_iOverflowResearch);
	//pStream->Read(&m_iNoUnhealthyPopulationCount);
	pStream->Read(&m_iUnhealthyPopulationModifier); // K-Mod
	pStream->Read(&m_iExpInBorderModifier);
	pStream->Read(&m_iBuildingOnlyHealthyCount);
	//DPII < Maintenance Modifiers >
	pStream->Read(&m_iMaintenanceModifier);
	pStream->Read(&m_iCoastalDistanceMaintenanceModifier);
	pStream->Read(&m_iConnectedCityMaintenanceModifier);
	//DPII < Maintenance Modifiers >
	pStream->Read(&m_iDistanceMaintenanceModifier);
	pStream->Read(&m_iNumCitiesMaintenanceModifier);
	pStream->Read(&m_iCorporationMaintenanceModifier);
	pStream->Read(&m_iTotalMaintenance);
	pStream->Read(&m_iUpkeepModifier);
	pStream->Read(&m_iLevelExperienceModifier);
	pStream->Read(&m_iExtraHealth);
	pStream->Read(&m_iBuildingGoodHealth);
	pStream->Read(&m_iBuildingBadHealth);
	pStream->Read(&m_iExtraHappiness);
	pStream->Read(&m_iBuildingHappiness);
	pStream->Read(&m_iLargestCityHappiness);
	pStream->Read(&m_iWarWearinessPercentAnger);
	pStream->Read(&m_iWarWearinessModifier);
	pStream->Read(&m_iGwPercentAnger); // K-Mod
	pStream->Read(&m_iFreeSpecialist);
	pStream->Read(&m_iNoForeignTradeCount);
	pStream->Read(&m_iNoCorporationsCount);
	pStream->Read(&m_iNoForeignCorporationsCount);
	pStream->Read(&m_iCoastalTradeRoutes);
	pStream->Read(&m_iTradeRoutes);
	pStream->Read(&m_iRevolutionTimer);
	pStream->Read(&m_iConversionTimer);
	pStream->Read(&m_iStateReligionCount);
	pStream->Read(&m_iNoNonStateReligionSpreadCount);
	pStream->Read(&m_iStateReligionHappiness);
	pStream->Read(&m_iNonStateReligionHappiness);
	// < Civic Infos Plus Start >
	pStream->Read(&m_iStateReligionExtraHealth);
	pStream->Read(&m_iNonStateReligionExtraHealth);
	// < Civic Infos Plus End   >
	pStream->Read(&m_iStateReligionUnitProductionModifier);
	pStream->Read(&m_iStateReligionBuildingProductionModifier);
	pStream->Read(&m_iStateReligionFreeExperience);
	pStream->Read(&m_iCapitalCityID);
	pStream->Read(&m_iCitiesLost);
	pStream->Read(&m_iWinsVsBarbs);
	pStream->Read(&m_iAssets);
	pStream->Read(&m_iPower);
	pStream->Read(&m_iPopulationScore);
	pStream->Read(&m_iLandScore);
	pStream->Read(&m_iWondersScore);
	pStream->Read(&m_iTechScore);
	pStream->Read(&m_iCombatExperience);
	// <advc.004x>
	if(uiFlag >= 8)
	{
		int tmp=-1;
		pStream->Read(&tmp);
		m_eReminderPending = (CivicTypes)tmp;
	} // </advc.004x>
	pStream->Read(&m_bAlive);
	pStream->Read(&m_bEverAlive);
	pStream->Read(&m_bTurnActive);
	pStream->Read(&m_bAutoMoves);
	pStream->Read(&m_bEndTurn);
	pStream->Read(&m_bPbemNewTurn);
	pStream->Read(&m_bExtendedGame);
	pStream->Read(&m_bFoundedFirstCity);
	// <advc.078>
	if(uiFlag >= 9)
		pStream->Read(&m_bAnyGPPEver); // </advc.078>
	pStream->Read(&m_bStrike);
	// K-Mod
	if (uiFlag >= 4)
		pStream->Read(&m_iChoosingFreeTechCount);
	else if (uiFlag >= 2)
	{
		bool bFreeTech = false;
		pStream->Read(&bFreeTech);
		m_iChoosingFreeTechCount = bFreeTech ? 1 : 0;
	}
	// K-Mod end

	pStream->Read((int*)&m_eID);
	pStream->Read((int*)&m_ePersonalityType);
	pStream->Read((int*)&m_eCurrentEra);
	pStream->Read((int*)&m_eLastStateReligion);
	pStream->Read((int*)&m_eParent);
	updateTeamType(); //m_eTeamType not saved
	updateHuman();

	pStream->Read(NUM_YIELD_TYPES, m_aiSeaPlotYield);
	pStream->Read(NUM_YIELD_TYPES, m_aiYieldRateModifier);
	pStream->Read(NUM_YIELD_TYPES, m_aiCapitalYieldRateModifier);
	// < Civic Infos Plus Start >
    pStream->Read(NUM_YIELD_TYPES, m_aiSpecialistExtraYield);
    pStream->Read(NUM_YIELD_TYPES, m_aiStateReligionYieldRateModifier);
    pStream->Read(NUM_YIELD_TYPES, m_aiNonStateReligionYieldRateModifier);
    // < Civic Infos Plus End   >
	pStream->Read(NUM_YIELD_TYPES, m_aiExtraYieldThreshold);
	pStream->Read(NUM_YIELD_TYPES, m_aiTradeYieldModifier);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiFreeCityCommerce);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCommercePercent);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCommerceRate);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCommerceRateModifier);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCapitalCommerceRateModifier);
	// < Civic Infos Plus Start >
    pStream->Read(NUM_COMMERCE_TYPES, m_aiStateReligionCommerceRateModifier);
    pStream->Read(NUM_COMMERCE_TYPES, m_aiNonStateReligionCommerceRateModifier);
    // < Civic Infos Plus End   >
	pStream->Read(NUM_COMMERCE_TYPES, m_aiStateReligionBuildingCommerce);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiSpecialistExtraCommerce);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCommerceFlexibleCount);
	pStream->Read(MAX_PLAYERS, m_aiGoldPerTurnByPlayer);
	pStream->Read(MAX_TEAMS, m_aiEspionageSpendingWeightAgainstTeam);

	pStream->Read(NUM_FEAT_TYPES, m_abFeatAccomplished);
	pStream->Read(NUM_PLAYEROPTION_TYPES, m_abOptions);

	pStream->ReadString(m_szScriptData);

	pStream->Read(GC.getNumBonusInfos(), m_paiBonusExport);
	pStream->Read(GC.getNumBonusInfos(), m_paiBonusImport);
	pStream->Read(GC.getNumImprovementInfos(), m_paiImprovementCount);
	pStream->Read(GC.getNumBuildingInfos(), m_paiFreeBuildingCount);
	pStream->Read(GC.getNumBuildingInfos(), m_paiExtraBuildingHappiness);
	pStream->Read(GC.getNumBuildingInfos(), m_paiExtraBuildingHealth);
	pStream->Read(GC.getNumFeatureInfos(), m_paiFeatureHappiness);
	pStream->Read(GC.getNumUnitClassInfos(), m_paiUnitClassCount);
	pStream->Read(GC.getNumUnitClassInfos(), m_paiUnitClassMaking);
	pStream->Read(GC.getNumBuildingClassInfos(), m_paiBuildingClassCount);
	pStream->Read(GC.getNumBuildingClassInfos(), m_paiBuildingClassMaking);
	pStream->Read(GC.getNumHurryInfos(), m_paiHurryCount);
	pStream->Read(GC.getNumSpecialBuildingInfos(), m_paiSpecialBuildingNotRequiredCount);
	pStream->Read(GC.getNumCivicOptionInfos(), m_paiHasCivicOptionCount);
	pStream->Read(GC.getNumCivicOptionInfos(), m_paiNoCivicUpkeepCount);
	pStream->Read(GC.getNumReligionInfos(), m_paiHasReligionCount);
	pStream->Read(GC.getNumCorporationInfos(), m_paiHasCorporationCount);
	pStream->Read(GC.getNumUpkeepInfos(), m_paiUpkeepCount);
	pStream->Read(GC.getNumSpecialistInfos(), m_paiSpecialistValidCount);

	FAssert(GC.getNumTechInfos() > 0);
	pStream->Read(GC.getNumTechInfos(), m_pabResearchingTech);

	pStream->Read(GC.getNumVoteSourceInfos(), m_pabLoyalMember);

	for (iI=0;iI<GC.getNumCivicOptionInfos();iI++)
	{
		pStream->Read((int*)&m_paeCivics[iI]);
	}

	for (iI=0;iI<GC.getNumSpecialistInfos();iI++)
	{
		pStream->Read(NUM_YIELD_TYPES, m_ppaaiSpecialistExtraYield[iI]);
	}
	// < Civic Infos Plus Start >
	for (iI=0;iI<GC.getNumBuildingInfos();iI++)
	{
		pStream->Read(NUM_YIELD_TYPES, m_ppaaiBuildingYieldChange[iI]);
	}

	for (iI=0;iI<GC.getNumBuildingInfos();iI++)
	{
		pStream->Read(NUM_COMMERCE_TYPES, m_ppaaiBuildingCommerceChange[iI]);
	}
	// < Civic Infos Plus End   >
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**																								**/
	/**																								**/
	/*************************************************************************************************/	
	for (iI=0;iI<GC.getNumSpecialistInfos();iI++)
	{
		pStream->Read(NUM_COMMERCE_TYPES, m_ppaaiSpecialistCivicExtraCommerce[iI]);
	}
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/

	for (iI=0;iI<GC.getNumImprovementInfos();iI++)
	{
		pStream->Read(NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[iI]);
	}

	m_groupCycle.Read(pStream);
	m_researchQueue.Read(pStream);

	{
		m_cityNames.clear();
		CvWString szBuffer;
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			pStream->ReadString(szBuffer);
			m_cityNames.insertAtEnd(szBuffer);
		}
	}

	ReadStreamableFFreeListTrashArray(m_plotGroups, pStream);
	ReadStreamableFFreeListTrashArray(m_cities, pStream);
	ReadStreamableFFreeListTrashArray(m_units, pStream);
	ReadStreamableFFreeListTrashArray(m_selectionGroups, pStream);
	ReadStreamableFFreeListTrashArray(m_eventsTriggered, pStream);

	{
		CvMessageQueue::_Alloc::size_type iSize;
		pStream->Read(&iSize);
		for (CvMessageQueue::_Alloc::size_type i = 0; i < iSize; i++)
		{
			CvTalkingHeadMessage message;
			message.read(*pStream);
			m_listGameMessages.push_back(message);
		}
	}

	{
		clearPopups();
		CvPopupQueue::_Alloc::size_type iSize;
		pStream->Read(&iSize);
		for (CvPopupQueue::_Alloc::size_type i = 0; i < iSize; i++)
		{
			CvPopupInfo* pInfo = new CvPopupInfo();
			if (NULL != pInfo)
			{
				pInfo->read(*pStream);
				m_listPopups.push_back(pInfo);
			}
		}
	}

	{
		clearDiplomacy();
		CvDiploQueue::_Alloc::size_type iSize;
		pStream->Read(&iSize);
		for (CvDiploQueue::_Alloc::size_type i = 0; i < iSize; i++)
		{
			CvDiploParameters* pDiplo = new CvDiploParameters(NO_PLAYER);
			if (NULL != pDiplo)
			{
				pDiplo->read(*pStream);
				m_listDiplomacy.push_back(pDiplo);
				// <advc.074> (see comment in CvPlayerAI::AI_doDeals)
				if (pDiplo->getDiploComment() == GC.getAIDiploCommentType("CANCEL_DEAL"))
				{
					for (CLLNode<TradeData> const* pNode = pDiplo->getOurOfferList().head();
						pNode != NULL; pNode =  pDiplo->getOurOfferList().next(pNode))
					{
						if (pNode->m_data.m_eItemType == TRADE_RESOURCES)
						{
							m_cancelingExport.insertAtEnd(std::make_pair(
									pDiplo->getWhoTalkingTo(),
									(BonusTypes)pNode->m_data.m_iData));
						}
					}
				} // </advc.074>
			}
		}
	}

	{
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapScoreHistory[iTurn] = iScore;
		}
	}

	{
		m_mapEconomyHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapEconomyHistory[iTurn] = iScore;
		}
	}

	{
		m_mapIndustryHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapIndustryHistory[iTurn] = iScore;
		}
	}

	{
		m_mapAgricultureHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapAgricultureHistory[iTurn] = iScore;
		}
	}

	{
		m_mapPowerHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapPowerHistory[iTurn] = iScore;
		}
	}

	{
		m_mapCultureHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapCultureHistory[iTurn] = iScore;
		}
	}

	{
		m_mapEspionageHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapEspionageHistory[iTurn] = iScore;
		}
	}

	{
		m_mapEventsOccured.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			EventTriggeredData kData;
			EventTypes eEvent;
			pStream->Read((int*)&eEvent);
			kData.read(pStream);
			m_mapEventsOccured[eEvent] = kData;
		}
	}

	{
		m_mapEventCountdown.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			EventTriggeredData kData;
			EventTypes eEvent;
			pStream->Read((int*)&eEvent);
			kData.read(pStream);
			m_mapEventCountdown[eEvent] = kData;
		}
	}

	{
		m_aFreeUnitCombatPromotions.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iUnitCombat;
			int iPromotion;
			pStream->Read(&iUnitCombat);
			pStream->Read(&iPromotion);
			m_aFreeUnitCombatPromotions.push_back(std::make_pair((UnitCombatTypes)iUnitCombat, (PromotionTypes)iPromotion));
		}
	}

	{
		m_aFreeUnitClassPromotions.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iUnitClass;
			int iPromotion;
			pStream->Read(&iUnitClass);
			pStream->Read(&iPromotion);
			m_aFreeUnitClassPromotions.push_back(std::make_pair((UnitClassTypes)iUnitClass, (PromotionTypes)iPromotion));
		}
	}

	{
		m_aVote.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iId;
			PlayerVoteTypes eVote;
			pStream->Read(&iId);
			pStream->Read((int*)&eVote);
			m_aVote.push_back(std::make_pair(iId, eVote));
		}
	}

	{
		m_aUnitExtraCosts.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iCost;
			UnitClassTypes eUnit;
			pStream->Read((int*)&eUnit);
			pStream->Read(&iCost);
			m_aUnitExtraCosts.push_back(std::make_pair(eUnit, iCost));
		}
	}

	if (uiFlag > 0)
	{
		m_triggersFired.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTrigger;
			pStream->Read(&iTrigger);
			m_triggersFired.push_back((EventTriggerTypes)iTrigger);
		}
	}
	else
	{
		int iNumEventTriggers = std::min(176, GC.getNumEventTriggerInfos()); // yuck, hardcoded number of eventTriggers in the epic game in initial release
		for (iI=0; iI < iNumEventTriggers; iI++)
		{
			bool bTriggered;
			pStream->Read(&bTriggered);
			if (bTriggered)
			{
				m_triggersFired.push_back((EventTriggerTypes)iI);
			}
		}
	}

	if (!isBarbarian())
	{
		// Get the NetID from the initialization structure
		setNetID(gDLL->getAssignedNetworkID(getID()));
	}

	pStream->Read(&m_iPopRushHurryCount);
	// <advc.064b>
	if(uiFlag >= 10)
		pStream->Read(&m_iGoldRushHurryCount);
	else
	{
		for(int i = 0; i < GC.getNumHurryInfos(); i++)
		{
			if(GC.getInfo((HurryTypes)i).getGoldPerProduction() > 0)
			{
				m_iGoldRushHurryCount = m_paiHurryCount[i];
				break;
			}
		}
	} // </advc.064b>
	pStream->Read(&m_iInflationModifier);
//KNOEDEL CULTURAL_GOLDEN_AGE
	pStream->Read(&m_iCultureGoldenAgeProgress);	//KNOEDEL CULTURAL_GOLDEN_AGE 4/8
	pStream->Read(&m_iCultureGoldenAgesStarted);	//KNOEDEL CULTURAL_GOLDEN_AGE 5/8

	if(!isAlive())
		return; // advc
	initAlerts(); // advc.210
	/*  <advc.706> Loading into retirement. Can't do this in RiseFall::read b/c
		CvPlayer::reset has to be through first. */
	CvGame& g = GC.getGame();
	if(g.isOption(GAMEOPTION_RISE_FALL) && g.getRiseFall().hasRetired())
		g.getRiseFall().retire(); // </advc.706>
	// <advc.908a>
	if(uiFlag < 5 && !isBarbarian())
	{
		for(int i = 0; i < NUM_YIELD_TYPES; i++)
			updateExtraYieldThreshold((YieldTypes)i);
	} // </advc.908a>
	// <advc.912c>
	if(uiFlag <= 6)
	{
		for(int i = 0; i < GC.getNumCivicInfos(); i++)
		{
			CivicTypes eCivic = (CivicTypes)i;
			if(!isCivic(eCivic))
				continue;
			CvCivicInfo& kCivic = GC.getInfo(eCivic);
			if(kCivic.getLuxuryModifier() <= 0)
				continue;
			int iPreviousHappyPer = (uiFlag < 6 ? 1 : 0);
			int iPreviousLux =  (uiFlag < 6 ? 0 : 50);
			changeHappyPerMilitaryUnit(kCivic.getHappyPerMilitaryUnit() -
					iPreviousHappyPer);
			changeLuxuryModifier(kCivic.getLuxuryModifier() - iPreviousLux);
		}
	} // </advc.912c>
}

// save object to a stream
void CvPlayer::write(FDataStreamBase* pStream)
{
	PROFILE_FUNC(); // advc
	int iI;
	REPRO_TEST_BEGIN_WRITE(CvString::format("PlayerPt1(%d)", getID()));
	uint uiFlag = 4;
	uiFlag = 5; // advc.908a
	uiFlag = 7; // advc.912c (6 used up for a test version)
	uiFlag = 8; // advc.004x
	uiFlag = 9; // advc.078
	uiFlag = 10; // advc.064b
	uiFlag = 11; // advc.001x
	pStream->Write(uiFlag);

	pStream->Write(m_iStartingX);
	pStream->Write(m_iStartingY);
	pStream->Write(m_iTotalPopulation);
	pStream->Write(m_iTotalLand);
	pStream->Write(m_iTotalLandScored);
	pStream->Write(m_iGold);
	pStream->Write(m_iGoldPerTurn);
	pStream->Write(m_iAdvancedStartPoints);
	pStream->Write(m_iGoldenAgeTurns);
	pStream->Write(m_iScheduledGoldenAges); // advc.001x
	pStream->Write(m_iNumUnitGoldenAges);
	pStream->Write(m_iStrikeTurns);
	pStream->Write(m_iAnarchyTurns);
	pStream->Write(m_iMaxAnarchyTurns);
	pStream->Write(m_iAnarchyModifier);
	pStream->Write(m_iGoldenAgeModifier);
	pStream->Write(m_iGlobalHurryModifier);
	pStream->Write(m_iGreatPeopleCreated);
	pStream->Write(m_iGreatGeneralsCreated);
	pStream->Write(m_iGreatPeopleThresholdModifier);
	pStream->Write(m_iGreatGeneralsThresholdModifier);
	pStream->Write(m_iGreatPeopleRateModifier);
	pStream->Write(m_iGreatGeneralRateModifier);
	pStream->Write(m_iDomesticGreatGeneralRateModifier);
	pStream->Write(m_iStateReligionGreatPeopleRateModifier);
	pStream->Write(m_iMaxGlobalBuildingProductionModifier);
	pStream->Write(m_iMaxTeamBuildingProductionModifier);
	pStream->Write(m_iMaxPlayerBuildingProductionModifier);
	pStream->Write(m_iFreeExperience);
	pStream->Write(m_iFeatureProductionModifier);
	pStream->Write(m_iWorkerSpeedModifier);
	pStream->Write(m_iImprovementUpgradeRateModifier);
	pStream->Write(m_iMilitaryProductionModifier);
	pStream->Write(m_iSpaceProductionModifier);
	pStream->Write(m_iCityDefenseModifier);
	pStream->Write(m_iNumNukeUnits);
	pStream->Write(m_iNumOutsideUnits);
	pStream->Write(m_iBaseFreeUnits);
	pStream->Write(m_iBaseFreeMilitaryUnits);
	pStream->Write(m_iFreeUnitsPopulationPercent);
	pStream->Write(m_iFreeMilitaryUnitsPopulationPercent);
	pStream->Write(m_iGoldPerUnit);
	pStream->Write(m_iGoldPerMilitaryUnit);
	pStream->Write(m_iExtraUnitCost);
	pStream->Write(m_iNumMilitaryUnits);
	pStream->Write(m_iHappyPerMilitaryUnit);
	pStream->Write(m_iLuxuryModifier); // advc.912c
	pStream->Write(m_iMilitaryFoodProductionCount);
	pStream->Write(m_iConscriptCount);
	pStream->Write(m_iMaxConscript);
	pStream->Write(m_iHighestUnitLevel);
	pStream->Write(m_iOverflowResearch);
	//pStream->Write(m_iNoUnhealthyPopulationCount);
	pStream->Write(m_iUnhealthyPopulationModifier); // K-Mod
	pStream->Write(m_iExpInBorderModifier);
	pStream->Write(m_iBuildingOnlyHealthyCount);
	//DPII < Maintenance Modifiers >
	pStream->Write(m_iMaintenanceModifier);
	pStream->Write(m_iCoastalDistanceMaintenanceModifier);
	pStream->Write(m_iConnectedCityMaintenanceModifier);
	//DPII < Maintenance Modifiers >
	pStream->Write(m_iDistanceMaintenanceModifier);
	pStream->Write(m_iNumCitiesMaintenanceModifier);
	pStream->Write(m_iCorporationMaintenanceModifier);
	pStream->Write(m_iTotalMaintenance);
	pStream->Write(m_iUpkeepModifier);
	pStream->Write(m_iLevelExperienceModifier);
	pStream->Write(m_iExtraHealth);
	pStream->Write(m_iBuildingGoodHealth);
	pStream->Write(m_iBuildingBadHealth);
	pStream->Write(m_iExtraHappiness);
	pStream->Write(m_iBuildingHappiness);
	pStream->Write(m_iLargestCityHappiness);
	pStream->Write(m_iWarWearinessPercentAnger);
	pStream->Write(m_iWarWearinessModifier);
	pStream->Write(m_iGwPercentAnger); // K-Mod
	pStream->Write(m_iFreeSpecialist);
	pStream->Write(m_iNoForeignTradeCount);
	pStream->Write(m_iNoCorporationsCount);
	pStream->Write(m_iNoForeignCorporationsCount);
	pStream->Write(m_iCoastalTradeRoutes);
	pStream->Write(m_iTradeRoutes);
	pStream->Write(m_iRevolutionTimer);
	pStream->Write(m_iConversionTimer);
	pStream->Write(m_iStateReligionCount);
	pStream->Write(m_iNoNonStateReligionSpreadCount);
	pStream->Write(m_iStateReligionHappiness);
	pStream->Write(m_iNonStateReligionHappiness);
	// < Civic Infos Plus Start >
	pStream->Write(m_iStateReligionExtraHealth);
	pStream->Write(m_iNonStateReligionExtraHealth);
	// < Civic Infos Plus End   >
	pStream->Write(m_iStateReligionUnitProductionModifier);
	pStream->Write(m_iStateReligionBuildingProductionModifier);
	pStream->Write(m_iStateReligionFreeExperience);
	pStream->Write(m_iCapitalCityID);
	pStream->Write(m_iCitiesLost);
	pStream->Write(m_iWinsVsBarbs);
	pStream->Write(m_iAssets);
	pStream->Write(m_iPower);
	pStream->Write(m_iPopulationScore);
	pStream->Write(m_iLandScore);
	pStream->Write(m_iWondersScore);
	pStream->Write(m_iTechScore);
	pStream->Write(m_iCombatExperience);
	pStream->Write(m_eReminderPending); // advc.004x

	pStream->Write(m_bAlive);
	pStream->Write(m_bEverAlive);
	pStream->Write(m_bTurnActive);
	pStream->Write(m_bAutoMoves);
	pStream->Write(m_bEndTurn);
	pStream->Write(m_bPbemNewTurn && GC.getGame().isPbem());
	pStream->Write(m_bExtendedGame);
	pStream->Write(m_bFoundedFirstCity);
	pStream->Write(m_bAnyGPPEver); // advc.078
	pStream->Write(m_bStrike);
	pStream->Write(m_iChoosingFreeTechCount); // K-Mod (bool for 2 <= uiFlag < 4. then int.)

	pStream->Write(m_eID);
	pStream->Write(m_ePersonalityType);
	pStream->Write(m_eCurrentEra);
	pStream->Write(m_eLastStateReligion);
	pStream->Write(m_eParent);
	//m_eTeamType not saved

	pStream->Write(NUM_YIELD_TYPES, m_aiSeaPlotYield);
	pStream->Write(NUM_YIELD_TYPES, m_aiYieldRateModifier);
	pStream->Write(NUM_YIELD_TYPES, m_aiCapitalYieldRateModifier);
	// < Civic Infos Plus Start >
    pStream->Write(NUM_YIELD_TYPES, m_aiSpecialistExtraYield);
    pStream->Write(NUM_YIELD_TYPES, m_aiStateReligionYieldRateModifier);
    pStream->Write(NUM_YIELD_TYPES, m_aiNonStateReligionYieldRateModifier);
    // < Civic Infos Plus End   >
	pStream->Write(NUM_YIELD_TYPES, m_aiExtraYieldThreshold);
	pStream->Write(NUM_YIELD_TYPES, m_aiTradeYieldModifier);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiFreeCityCommerce);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCommercePercent);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCommerceRate);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCommerceRateModifier);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCapitalCommerceRateModifier);
	// < Civic Infos Plus Start >
    pStream->Write(NUM_COMMERCE_TYPES, m_aiStateReligionCommerceRateModifier);
    pStream->Write(NUM_COMMERCE_TYPES, m_aiNonStateReligionCommerceRateModifier);
    // < Civic Infos Plus End   >
	pStream->Write(NUM_COMMERCE_TYPES, m_aiStateReligionBuildingCommerce);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiSpecialistExtraCommerce);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCommerceFlexibleCount);
	pStream->Write(MAX_PLAYERS, m_aiGoldPerTurnByPlayer);
	pStream->Write(MAX_TEAMS, m_aiEspionageSpendingWeightAgainstTeam);

	pStream->Write(NUM_FEAT_TYPES, m_abFeatAccomplished);
	pStream->Write(NUM_PLAYEROPTION_TYPES, m_abOptions);

	pStream->WriteString(m_szScriptData);

	FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlayer::write");
	pStream->Write(GC.getNumBonusInfos(), m_paiBonusExport);
	pStream->Write(GC.getNumBonusInfos(), m_paiBonusImport);
	pStream->Write(GC.getNumImprovementInfos(), m_paiImprovementCount);
	pStream->Write(GC.getNumBuildingInfos(), m_paiFreeBuildingCount);
	pStream->Write(GC.getNumBuildingInfos(), m_paiExtraBuildingHappiness);
	pStream->Write(GC.getNumBuildingInfos(), m_paiExtraBuildingHealth);
	pStream->Write(GC.getNumFeatureInfos(), m_paiFeatureHappiness);
	pStream->Write(GC.getNumUnitClassInfos(), m_paiUnitClassCount);
	pStream->Write(GC.getNumUnitClassInfos(), m_paiUnitClassMaking);
	pStream->Write(GC.getNumBuildingClassInfos(), m_paiBuildingClassCount);
	pStream->Write(GC.getNumBuildingClassInfos(), m_paiBuildingClassMaking);
	pStream->Write(GC.getNumHurryInfos(), m_paiHurryCount);
	pStream->Write(GC.getNumSpecialBuildingInfos(), m_paiSpecialBuildingNotRequiredCount);
	pStream->Write(GC.getNumCivicOptionInfos(), m_paiHasCivicOptionCount);
	pStream->Write(GC.getNumCivicOptionInfos(), m_paiNoCivicUpkeepCount);
	pStream->Write(GC.getNumReligionInfos(), m_paiHasReligionCount);
	pStream->Write(GC.getNumCorporationInfos(), m_paiHasCorporationCount);
	pStream->Write(GC.getNumUpkeepInfos(), m_paiUpkeepCount);
	pStream->Write(GC.getNumSpecialistInfos(), m_paiSpecialistValidCount);

	FAssertMsg((0 < GC.getNumTechInfos()), "GC.getNumTechInfos() is not greater than zero but it is expected to be in CvPlayer::write");
	pStream->Write(GC.getNumTechInfos(), m_pabResearchingTech);

	pStream->Write(GC.getNumVoteSourceInfos(), m_pabLoyalMember);

	for (iI=0;iI<GC.getNumCivicOptionInfos();iI++)
	{
		pStream->Write(m_paeCivics[iI]);
	}

	for (iI=0;iI<GC.getNumSpecialistInfos();iI++)
	{
		pStream->Write(NUM_YIELD_TYPES, m_ppaaiSpecialistExtraYield[iI]);
	}
	// < Civic Infos Plus Start >
	for (iI=0;iI<GC.getNumBuildingInfos();iI++)
	{
		pStream->Write(NUM_YIELD_TYPES, m_ppaaiBuildingYieldChange[iI]);
	}

	for (iI=0;iI<GC.getNumBuildingInfos();iI++)
	{
		pStream->Write(NUM_COMMERCE_TYPES, m_ppaaiBuildingCommerceChange[iI]);
	}
	// < Civic Infos Plus End   >	
	/*************************************************************************************************/
	/**	CMEDIT: Civic Specialist Yield & Commerce Changes											**/
	/**																								**/
	/**																								**/
	/*************************************************************************************************/
	for (iI=0;iI<GC.getNumSpecialistInfos();iI++)
	{
		pStream->Write(NUM_COMMERCE_TYPES, m_ppaaiSpecialistCivicExtraCommerce[iI]);
	}
	/*************************************************************************************************/
	/**	CMEDIT: End																					**/
	/*************************************************************************************************/

	for (iI=0;iI<GC.getNumImprovementInfos();iI++)
	{
		pStream->Write(NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[iI]);
	}

	m_groupCycle.Write(pStream);
	m_researchQueue.Write(pStream);

	{
		CLLNode<CvWString>* pNode;
		uint iSize = m_cityNames.getLength();
		pStream->Write(iSize);
		pNode = m_cityNames.head();
		while (pNode != NULL)
		{
			pStream->WriteString(pNode->m_data);
			pNode = m_cityNames.next(pNode);
		}
	}
	{
		PROFILE("CvPlayer::Write.FLTA"); // advc
		WriteStreamableFFreeListTrashArray(m_plotGroups, pStream);
		REPRO_TEST_END_WRITE();
		WriteStreamableFFreeListTrashArray(m_cities, pStream);
		WriteStreamableFFreeListTrashArray(m_units, pStream);
		WriteStreamableFFreeListTrashArray(m_selectionGroups, pStream);
		WriteStreamableFFreeListTrashArray(m_eventsTriggered, pStream);
	}
	{
		CvMessageQueue::_Alloc::size_type iSize = m_listGameMessages.size();
		pStream->Write(iSize);
		CvMessageQueue::iterator it;
		for (it = m_listGameMessages.begin(); it != m_listGameMessages.end(); ++it)
		{
			CvTalkingHeadMessage& message = *it;
			message.write(*pStream);
		}
	}
	{
		CvPopupQueue currentPopups;
		// Don't save open popups in MP to avoid having different state on different machines
		if (GC.getGame().isNetworkMultiPlayer())
			currentPopups.clear();
		else gDLL->getInterfaceIFace()->getDisplayedButtonPopups(currentPopups);
		/*	<advc.001> Don't store popups for AI players. The EXE sometimes adds popups
			to AI players through CvPlayer::getPopups; not sure when and why. Those popups
			linger and appear when switching to an AI player through Alt+Z. */
		if (GC.getGame().getActivePlayer() != getID())
		{
			currentPopups.clear();
			clearPopups();
		} // </advc.001>
		CvPopupQueue::_Alloc::size_type iSize = m_listPopups.size() + currentPopups.size();
		pStream->Write(iSize);
		CvPopupQueue::iterator it;
		for (it = currentPopups.begin(); it != currentPopups.end(); ++it)
		{
			CvPopupInfo* pInfo = *it;
			if (pInfo != NULL)
				pInfo->write(*pStream);
		}
		for (it = m_listPopups.begin(); it != m_listPopups.end(); ++it)
		{
			CvPopupInfo* pInfo = *it;
			if (pInfo != NULL)
				pInfo->write(*pStream);
		}
	}
	REPRO_TEST_BEGIN_WRITE(CvString::format("PlayerPt2(%d)", getID())); // (skip popups, messages)
	{
		CvDiploQueue::_Alloc::size_type iSize = m_listDiplomacy.size();
		pStream->Write(iSize);
		CvDiploQueue::iterator it;
		for (it = m_listDiplomacy.begin(); it != m_listDiplomacy.end(); ++it)
		{
			CvDiploParameters* pDiplo = *it;
			if (pDiplo != NULL)
				pDiplo->write(*pStream);
		}
	}

	{
		uint iSize = m_mapScoreHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapScoreHistory.begin(); it != m_mapScoreHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapEconomyHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapEconomyHistory.begin(); it != m_mapEconomyHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapIndustryHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapIndustryHistory.begin(); it != m_mapIndustryHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapAgricultureHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapAgricultureHistory.begin(); it != m_mapAgricultureHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapPowerHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapPowerHistory.begin(); it != m_mapPowerHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapCultureHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapCultureHistory.begin(); it != m_mapCultureHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapEspionageHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapEspionageHistory.begin(); it != m_mapEspionageHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapEventsOccured.size();
		pStream->Write(iSize);
		CvEventMap::iterator it;
		for (it = m_mapEventsOccured.begin(); it != m_mapEventsOccured.end(); ++it)
		{
			pStream->Write(it->first);
			it->second.write(pStream);
		}
	}

	{
		uint iSize = m_mapEventCountdown.size();
		pStream->Write(iSize);
		CvEventMap::iterator it;
		for (it = m_mapEventCountdown.begin(); it != m_mapEventCountdown.end(); ++it)
		{
			pStream->Write(it->first);
			it->second.write(pStream);
		}
	}

	{
		uint iSize = m_aFreeUnitCombatPromotions.size();
		pStream->Write(iSize);
		UnitCombatPromotionArray::iterator it;
		for (it = m_aFreeUnitCombatPromotions.begin(); it != m_aFreeUnitCombatPromotions.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_aFreeUnitClassPromotions.size();
		pStream->Write(iSize);
		UnitClassPromotionArray::iterator it;
		for (it = m_aFreeUnitClassPromotions.begin(); it != m_aFreeUnitClassPromotions.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_aVote.size();
		pStream->Write(iSize);
		std::vector< std::pair<int, PlayerVoteTypes> >::iterator it;
		for (it = m_aVote.begin(); it != m_aVote.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_aUnitExtraCosts.size();
		pStream->Write(iSize);
		std::vector< std::pair<UnitClassTypes, int> >::iterator it;
		for (it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_triggersFired.size();
		pStream->Write(iSize);
		std::vector<EventTriggerTypes>::iterator it;
		for (it = m_triggersFired.begin(); it != m_triggersFired.end(); ++it)
		{
			pStream->Write((*it));
		}
	}

	pStream->Write(m_iPopRushHurryCount);
	pStream->Write(m_iGoldRushHurryCount); // advc.064b
	pStream->Write(m_iInflationModifier);
//CULTURAL_GOLDEN_AGE
	pStream->Write(m_iCultureGoldenAgeProgress);	//KNOEDEL 6/8
	pStream->Write(m_iCultureGoldenAgesStarted);	//KNOEDEL 7/8
	REPRO_TEST_END_WRITE();
}

void CvPlayer::createGreatPeople(UnitTypes eGreatPersonUnit,
		bool bIncrementThreshold, bool bIncrementExperience, int iX, int iY)
{
	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (pPlot == NULL)
	{
		FAssertMsg(false, "Invalid plot in createGreatPeople()");
		return;
	}
	CvUnit* pGreatPeopleUnit = initUnit(eGreatPersonUnit, iX, iY);
	if (NULL == pGreatPeopleUnit)
	{
		FAssert(false);
		return;
	}
	CvCity* pCity = pPlot->getPlotCity();

	if (bIncrementThreshold)
	{
		incrementGreatPeopleCreated();

		changeGreatPeopleThresholdModifier(
				GC.getDefineINT("GREAT_PEOPLE_THRESHOLD_INCREASE") *
				((getGreatPeopleCreated() / 10) + 1));

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (TEAMID((PlayerTypes)iI) == getTeam())
			{
				GET_PLAYER((PlayerTypes)iI).changeGreatPeopleThresholdModifier(
						GC.getDefineINT("GREAT_PEOPLE_THRESHOLD_INCREASE_TEAM") *
						((getGreatPeopleCreated() / 10) + 1));
			}
		}
	}

	if (bIncrementExperience)
	{
		incrementGreatGeneralsCreated();
		changeGreatGeneralsThresholdModifier(
				GC.getDefineINT("GREAT_GENERALS_THRESHOLD_INCREASE") *
				((getGreatGeneralsCreated() / 10) + 1));

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (TEAMID((PlayerTypes)iI) == getTeam())
			{
				GET_PLAYER((PlayerTypes)iI).changeGreatGeneralsThresholdModifier(
						GC.getDefineINT("GREAT_GENERALS_THRESHOLD_INCREASE_TEAM") *
						((getGreatGeneralsCreated() / 10) + 1));
			}
		}
	}
	// <advc.106>
	CvPlayer const& kGPOwner = GET_PLAYER(pPlot->getOwner());
	// Use shorter message for replays
	CvWString szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_REPLAY",
			pGreatPeopleUnit->getReplayName().GetCString(),
			kGPOwner.getCivilizationDescriptionKey()); // </advc.106>
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szReplayMessage,
			iX, iY, GC.getColorType("UNIT_TEXT"));
	// Non-replay message
	CvWString szMessage;
	if (pCity != NULL)
	{
		CvWString szCity;
		szCity.Format(L"%s (%s)", pCity->getName().GetCString(),
				GET_PLAYER(pCity->getOwner()).getReplayName());
		szMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN",
				pGreatPeopleUnit->getName().GetCString(), szCity.GetCString());
	}
	else
	{
		szMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_FIELD",
				pGreatPeopleUnit->getName().GetCString());
	} // <advc.106>
	for (PlayerIter<MAJOR_CIV,KNOWN_TO> it(kGPOwner.getTeam()); it.hasNext(); ++it)
	{
		CvPlayer const& kObs = *it;
		bool const bRev = (pPlot->isRevealed(kObs.getTeam(), true) &&
				(pCity == NULL || pCity->isRevealed(kObs.getTeam(), true)));
		if (!bRev)
		{
			szMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_CIV",
					pGreatPeopleUnit->getName().GetCString(),
					kGPOwner.getCivilizationDescriptionKey());
		} // <advc.106b>
		InterfaceMessageTypes eMsgType = MESSAGE_TYPE_MINOR_EVENT;
		// Only birth of own GP is major
		// ^On second thought, make all GP births minor.
		/*if(kObs.getID() == kGPOwner.getID())
			eMsgType = MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY;*/ // </advc.106b>
		gDLL->UI().addMessage(kObs.getID(), false, -1, szMessage, "AS2D_UNIT_GREATPEOPLE",
				eMsgType, pGreatPeopleUnit->getButton(),  // <advc.106>
				//(ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"),
				NO_COLOR, // Colored through XML now
				// Indicate location only if revealed.
				bRev ? iX : -1, bRev ? iY : -1, bRev, bRev);
	} // </advc.106>

	if (pCity) // Python Event
		CvEventReporter::getInstance().greatPersonBorn(pGreatPeopleUnit, getID(), pCity);
}


const EventTriggeredData* CvPlayer::getEventOccured(EventTypes eEvent) const
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::const_iterator it = m_mapEventsOccured.find(eEvent);

	if (it == m_mapEventsOccured.end())
	{
		return NULL;
	}

	return &(it->second);
}

bool CvPlayer::isTriggerFired(EventTriggerTypes eEventTrigger) const
{
	return (std::find(m_triggersFired.begin(), m_triggersFired.end(), eEventTrigger) != m_triggersFired.end());
}

void CvPlayer::resetEventOccured(EventTypes eEvent, bool bAnnounce)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::iterator it = m_mapEventsOccured.find(eEvent);

	if (it != m_mapEventsOccured.end())
	{
		expireEvent(it->first, it->second, bAnnounce);
		m_mapEventsOccured.erase(it);
	}
}

void CvPlayer::setEventOccured(EventTypes eEvent, const EventTriggeredData& kEventTriggered, bool bOthers)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	m_mapEventsOccured[eEvent] = kEventTriggered;

	if (GC.getInfo(eEvent).isQuest())
	{
		CvWStringBuffer szMessageBuffer;
		szMessageBuffer.append(GC.getInfo(eEvent).getDescription());
		GAMETEXT.setEventHelp(szMessageBuffer, eEvent, kEventTriggered.getID(), getID());
		gDLL->getInterfaceIFace()->addQuestMessage(getID(), szMessageBuffer.getCString(), kEventTriggered.getID());
	}

	if (bOthers)
	{
		if (GC.getInfo(eEvent).isGlobal())
		{
			for (int i = 0; i < MAX_CIV_PLAYERS; i++)
			{
				if (i != getID())
				{
					GET_PLAYER((PlayerTypes)i).setEventOccured(eEvent, kEventTriggered, false);
				}
			}
		}
		else if (GC.getInfo(eEvent).isTeam())
		{
			for (int i = 0; i < MAX_CIV_PLAYERS; i++)
			{
				if (i != getID() && getTeam() == GET_PLAYER((PlayerTypes)i).getTeam())
				{
					GET_PLAYER((PlayerTypes)i).setEventOccured(eEvent, kEventTriggered, false);
				}
			}
		}
	}
}


const EventTriggeredData* CvPlayer::getEventCountdown(EventTypes eEvent) const
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::const_iterator it = m_mapEventCountdown.find(eEvent);

	if (it == m_mapEventCountdown.end())
	{
		return NULL;
	}

	return &(it->second);
}

void CvPlayer::setEventCountdown(EventTypes eEvent, const EventTriggeredData& kEventTriggered)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	m_mapEventCountdown[eEvent] = kEventTriggered;
}

void CvPlayer::resetEventCountdown(EventTypes eEvent)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::iterator it = m_mapEventCountdown.find(eEvent);

	if (it != m_mapEventCountdown.end())
	{
		m_mapEventCountdown.erase(it);
	}
}


void CvPlayer::resetTriggerFired(EventTriggerTypes eTrigger)
{
	std::vector<EventTriggerTypes>::iterator it = std::find(m_triggersFired.begin(), m_triggersFired.end(), eTrigger);

	if (it != m_triggersFired.end())
	{
		m_triggersFired.erase(it);
	}
}


void CvPlayer::setTriggerFired(const EventTriggeredData& kTriggeredData, bool bOthers, bool bAnnounce)  // advc: some style changes
{
	FAssertBounds(0, GC.getNumEventTriggerInfos(), kTriggeredData.m_eTrigger);

	CvEventTriggerInfo& kTrigger = GC.getInfo(kTriggeredData.m_eTrigger);
	if (!isTriggerFired(kTriggeredData.m_eTrigger))
	{
		m_triggersFired.push_back(kTriggeredData.m_eTrigger);
		if (bOthers)
		{
			if (kTrigger.isGlobal())
			{
				for (int i = 0; i < MAX_CIV_PLAYERS; i++)
				{
					if (i != getID())
						GET_PLAYER((PlayerTypes)i).setTriggerFired(kTriggeredData, false, false);
				}
			}
			else if (kTrigger.isTeam())
			{
				for (int i = 0; i < MAX_CIV_PLAYERS; i++)
				{
					if (i != getID() && getTeam() == TEAMID((PlayerTypes)i))
						GET_PLAYER((PlayerTypes)i).setTriggerFired(kTriggeredData, false, false);
				}
			}
		}
	}

	GC.getPythonCaller()->afterEventTriggered(kTriggeredData);

	if (bAnnounce)
	{
		CvPlot* pPlot = GC.getMap().plot(kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY);
		if (!kTriggeredData.m_szGlobalText.empty())
		{
			// advc: Moved out of the loop
			bool bMetOtherPlayer = (kTriggeredData.m_eOtherPlayer == NO_PLAYER ||
					GET_TEAM(kTriggeredData.m_eOtherPlayer).isHasMet(getTeam()));
			for (int i = 0; i < MAX_CIV_PLAYERS; i++)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)i);
				if (!kLoopPlayer.isAlive())
					continue;

				if (bMetOtherPlayer || GET_TEAM(kLoopPlayer.getTeam()).isHasMet(getTeam()))
				{
					bool bShowPlot = kTrigger.isShowPlot();
					if (bShowPlot && kLoopPlayer.getTeam() != getTeam())
					{
						if (pPlot == NULL || !pPlot->isRevealed(kLoopPlayer.getTeam(), /* advc.106: */ true))
							bShowPlot = false;
					}
					if (bShowPlot)
					{
						gDLL->UI().addMessage(kLoopPlayer.getID(), false, -1,
								kTriggeredData.m_szGlobalText, "AS2D_CIVIC_ADOPT",
								MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getColorType("WHITE"),
								kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY, true, true);
					}
					else
					{
						gDLL->UI().addMessage(kLoopPlayer.getID(), false, -1,
								kTriggeredData.m_szGlobalText, "AS2D_CIVIC_ADOPT",
								MESSAGE_TYPE_MINOR_EVENT);
					}
				}
			}
			// advc.106g: Don't show (most) random events in replays
			//GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), kTriggeredData.m_szGlobalText, kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
		}
		else if (!kTriggeredData.m_szText.empty())
		{
			if (kTrigger.isShowPlot() && NULL != pPlot && pPlot->isRevealed(getTeam(), /* advc.106: */ true))
			{
				gDLL->UI().addMessage(getID(), false, -1, kTriggeredData.m_szText,
						"AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, NO_COLOR,
						kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY, true, true);
			}
			else
			{
				gDLL->UI().addMessage(getID(), false, -1, kTriggeredData.m_szText,
						"AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL);
			}
		}
	}
}


EventTriggeredData* CvPlayer::initTriggeredData(EventTriggerTypes eEventTrigger,
	bool bFire, int iCityId, int iPlotX, int iPlotY, PlayerTypes eOtherPlayer,
	int iOtherPlayerCityId, ReligionTypes eReligion, CorporationTypes eCorporation,
	int iUnitId, BuildingTypes eBuilding)
{
	CvEventTriggerInfo& kTrigger = GC.getInfo(eEventTrigger);

	CvCity* pCity = getCity(iCityId);
	CvCity* pOtherPlayerCity = NULL;
	if (eOtherPlayer != NO_PLAYER)
		pOtherPlayerCity = GET_PLAYER(eOtherPlayer).getCity(iOtherPlayerCityId);

	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	CvUnit* pUnit = getUnit(iUnitId);

	std::vector<CvPlot*> apPlots;
	bool const bPickPlot = GC.getInfo(eEventTrigger).isPlotEventTrigger();
	if (kTrigger.isPickCity())
	{
		if (pCity == NULL)
			pCity = pickTriggerCity(eEventTrigger);
		if (pCity == NULL)
			return NULL;

		if (bPickPlot)
		{
			for (CityPlotIter it(*pCity, false); it.hasNext(); ++it)
			{
				if (it->canTrigger(eEventTrigger, getID()))
					apPlots.push_back(&(*it));
			}
		}
	}
	else
	{
		if (kTrigger.getNumBuildings() > 0 && kTrigger.getNumBuildingsRequired() > 0)
		{
			int iFoundValid = 0;
			for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
			{
				if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
				{
					iFoundValid += getBuildingClassCount((BuildingClassTypes)kTrigger.getBuildingRequired(i));
				}
			}
			if (iFoundValid < kTrigger.getNumBuildings())
				return NULL;
		}

		if (kTrigger.getNumReligions() > 0)
		{
			int iFoundValid = 0;
			if (kTrigger.getNumReligionsRequired() > 0)
			{
				for (int i = 0; i < kTrigger.getNumReligionsRequired(); ++i)
				{
					if (kTrigger.getReligionRequired(i) != NO_RELIGION)
					{
						if (getHasReligionCount((ReligionTypes)kTrigger.getReligionRequired(i)) > 0)
						{
							++iFoundValid;
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < GC.getNumReligionInfos(); ++i)
				{
					if (getHasReligionCount((ReligionTypes)i) > 0)
						++iFoundValid;
				}
			}

			if (iFoundValid < kTrigger.getNumReligions())
				return NULL;
		}

		if (kTrigger.getNumCorporations() > 0)
		{
			int iFoundValid = 0;

			if (kTrigger.getNumCorporationsRequired() > 0)
			{
				for (int i = 0; i < kTrigger.getNumCorporationsRequired(); ++i)
				{
					if (kTrigger.getCorporationRequired(i) != NO_CORPORATION)
					{
						if (getHasCorporationCount((CorporationTypes)kTrigger.getCorporationRequired(i)) > 0)
							++iFoundValid;
					}
				}
			}
			else
			{
				for (int i = 0; i < GC.getNumCorporationInfos(); ++i)
				{
					if (getHasCorporationCount((CorporationTypes)i) > 0)
						++iFoundValid;
				}
			}

			if (iFoundValid < kTrigger.getNumCorporations())
				return NULL;
		}

		if (kTrigger.getMinPopulation() > 0)
		{
			if (getTotalPopulation() < kTrigger.getMinPopulation())
				return NULL;
		}

		if (kTrigger.getMaxPopulation() > 0)
		{
			if (getTotalPopulation() > kTrigger.getMaxPopulation())
				return NULL;
		}

		if (bPickPlot)
		{
			CvMap const& kMap = GC.getMap();
			for (int iPlot = 0; iPlot < kMap.numPlots(); ++iPlot)
			{
				CvPlot& kLoopPlot = kMap.getPlotByIndex(iPlot);
				if (kLoopPlot.canTrigger(eEventTrigger, getID()))
					apPlots.push_back(&kLoopPlot);
			}
		}
	}

	if (kTrigger.isPickReligion())
	{
		if (NO_RELIGION == eReligion)
		{
			if (kTrigger.isStateReligion())
			{
				ReligionTypes eStateReligion = getStateReligion();
				if (NO_RELIGION != eStateReligion && isValidTriggerReligion(kTrigger, pCity, eStateReligion))
				{
					eReligion = getStateReligion();
				}
			}
			else
			{
				int iOffset = GC.getGame().getSorenRandNum(GC.getNumReligionInfos(), "Event pick religion");

				for (int i = 0; i < GC.getNumReligionInfos(); ++i)
				{
					int iReligion = (i + iOffset) % GC.getNumReligionInfos();

					if (isValidTriggerReligion(kTrigger, pCity, (ReligionTypes)iReligion))
					{
						eReligion = (ReligionTypes)iReligion;
						break;
					}
				}
			}
		}

		if (NO_RELIGION == eReligion)
		{
			return NULL;
		}
	}

	if (kTrigger.isPickCorporation())
	{
		if (NO_CORPORATION == eCorporation)
		{
			int iOffset = GC.getGame().getSorenRandNum(GC.getNumCorporationInfos(), "Event pick corporation");

			for (int i = 0; i < GC.getNumCorporationInfos(); ++i)
			{
				int iCorporation = (i + iOffset) % GC.getNumCorporationInfos();

				if (isValidTriggerCorporation(kTrigger, pCity, (CorporationTypes)iCorporation))
				{
					eCorporation = (CorporationTypes)iCorporation;
					break;
				}
			}
		}

		if (NO_CORPORATION == eCorporation)
		{
			return NULL;
		}
	}

	if (NULL == pPlot)
	{
		if (apPlots.size() > 0)
		{
			int iChosen = GC.getGame().getSorenRandNum(apPlots.size(), "Event pick plot");
			pPlot = apPlots[iChosen];

			if (NULL == pCity)
			{
				pCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), getID(), NO_TEAM, false);
			}
		}
		else
		{
			if (bPickPlot)
			{
				return NULL;
			}

			if (NULL != pCity)
			{
				pPlot = pCity->plot();
			}
		}
	}

	if (kTrigger.getNumBuildings() > 0)
	{
		if (NULL != pCity && NO_BUILDING == eBuilding)
		{
			std::vector<BuildingTypes> aeBuildings;
			for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
			{
				if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
				{
					BuildingTypes eTestBuilding = getCivilization().getBuilding(
							(BuildingClassTypes)kTrigger.getBuildingRequired(i));
					if (eTestBuilding != NO_BUILDING && pCity->getNumRealBuilding(eTestBuilding) > 0)
					{
						aeBuildings.push_back(eTestBuilding);
					}
				}
			}

			if (aeBuildings.size() > 0)
			{
				int iChosen = GC.getGame().getSorenRandNum(aeBuildings.size(), "Event pick building");
				eBuilding = aeBuildings[iChosen];
			}
			else
			{
				return NULL;
			}
		}
	}

	if (NULL == pUnit)
	{
		pUnit = pickTriggerUnit(eEventTrigger, pPlot, bPickPlot);
	}

	if (NULL == pUnit && kTrigger.getNumUnits() > 0)
	{
		return NULL;
	}

	if (NULL == pPlot && NULL != pUnit)
	{
		pPlot = pUnit->plot();
	}

	if (NULL == pPlot && bPickPlot)
	{
		return NULL;
	}

	if (kTrigger.getNumUnitsGlobal() > 0)
	{
		int iNumUnits = 0;
		for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (!kLoopPlayer.isAlive())
				continue;

			FOR_EACH_UNIT(pLoopUnit, kLoopPlayer)
			{
				if (pLoopUnit->getTriggerValue(eEventTrigger, pPlot, true) != MIN_INT)
					iNumUnits++;
			}
		}

		if (iNumUnits < kTrigger.getNumUnitsGlobal())
			return NULL;
	}

	if (kTrigger.getNumBuildingsGlobal() > 0)
	{
		int iNumBuildings = 0;
		for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive())
			{
				for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
				{
					if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
					{
						iNumBuildings += getBuildingClassCount((BuildingClassTypes)kTrigger.getBuildingRequired(i));
					}
				}
			}
		}

		if (iNumBuildings < kTrigger.getNumBuildingsGlobal())
		{
			return NULL;
		}
	}

	if (kTrigger.isPickPlayer())
	{
		std::vector<PlayerTypes> aePlayers;
		std::vector<CvCity*> apCities;

		if (NO_PLAYER == eOtherPlayer)
		{
			for (int i = 0; i < MAX_CIV_PLAYERS; i++)
			{	// advc.001: (from Leoreth's RFC:DoC mod)
				if (!GET_PLAYER((PlayerTypes)i).isMinorCiv() &&
						GET_PLAYER((PlayerTypes)i).canTrigger(eEventTrigger, getID(), eReligion))
				{
					if (kTrigger.isPickOtherPlayerCity())
					{
						CvCity* pBestCity = NULL;

						if (NULL != pCity)
						{
							pBestCity = GC.getMap().findCity(pCity->getX(), pCity->getY(), (PlayerTypes)i);
						}
						else
						{
							pBestCity = GET_PLAYER((PlayerTypes)i).pickTriggerCity(eEventTrigger);
						}

						if (NULL != pBestCity)
						{
							apCities.push_back(pBestCity);
							aePlayers.push_back((PlayerTypes)i);
						}
					}
					else
					{
						apCities.push_back(NULL);
						aePlayers.push_back((PlayerTypes)i);
					}
				}
			}

			if (aePlayers.size() > 0)
			{
				int iChosen = GC.getGame().getSorenRandNum(aePlayers.size(), "Event pick player");
				eOtherPlayer = aePlayers[iChosen];
				pOtherPlayerCity = apCities[iChosen];
			}
			else
			{
				return NULL;
			}
		}
	}

	EventTriggeredData* pTriggerData = addEventTriggered();

	if (NULL != pTriggerData)
	{
		pTriggerData->m_eTrigger = eEventTrigger;
		pTriggerData->m_ePlayer = getID();
		pTriggerData->m_iTurn = GC.getGame().getGameTurn();
		pTriggerData->m_iCityId = (NULL != pCity) ? pCity->getID() : -1;
		pTriggerData->m_iPlotX = (NULL != pPlot) ? pPlot->getX() : INVALID_PLOT_COORD;
		pTriggerData->m_iPlotY = (NULL != pPlot) ? pPlot->getY() : INVALID_PLOT_COORD;
		pTriggerData->m_eOtherPlayer = eOtherPlayer;
		pTriggerData->m_iOtherPlayerCityId = (NULL != pOtherPlayerCity) ? pOtherPlayerCity->getID() : -1;
		pTriggerData->m_eReligion = eReligion;
		pTriggerData->m_eCorporation = eCorporation;
		pTriggerData->m_iUnitId = (NULL != pUnit) ? pUnit->getID() : -1;
		pTriggerData->m_eBuilding = eBuilding;
	}
	else return NULL;

	if (!GC.getPythonCaller()->doEventTrigger(getID(), *pTriggerData,
		// The Python call may change these data
		pCity, pPlot, pUnit, eOtherPlayer, pOtherPlayerCity, eReligion, eCorporation, eBuilding))
	{
		deleteEventTriggered(pTriggerData->getID());
		return NULL;
	}

	std::vector<CvWString> aszTexts;
	for (int i = 0; i < kTrigger.getNumTexts(); ++i)
	{
		if (NO_ERA == kTrigger.getTextEra(i) || kTrigger.getTextEra(i) == getCurrentEra())
		{
			aszTexts.push_back(kTrigger.getText(i));
		}
	}

	if (aszTexts.size() > 0)
	{
		int iText = GC.getGame().getSorenRandNum(aszTexts.size(), "Event Text choice");

		pTriggerData->m_szText = gDLL->getText(aszTexts[iText].GetCString(),
			eOtherPlayer != NO_PLAYER ? GET_PLAYER(eOtherPlayer).getCivilizationAdjectiveKey() : L"",
			NULL != pCity ? pCity->getNameKey() : L"",
			NULL != pUnit ? pUnit->getNameKey() : L"",
			NO_RELIGION != eReligion ? GC.getInfo(eReligion).getAdjectiveKey() : L"",
			NO_BUILDING != eBuilding ? GC.getInfo(eBuilding).getTextKeyWide() : L"",
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			NULL != pPlot && NO_TERRAIN != pPlot->getTerrainType() ? GC.getInfo(pPlot->getTerrainType()).getTextKeyWide() : L"",
			NULL != pPlot && pPlot->isImproved() ? GC.getInfo(pPlot->getImprovementType()).getTextKeyWide() : L"",
			NULL != pPlot && NO_BONUS != pPlot->getBonusType() ? GC.getInfo(pPlot->getBonusType()).getTextKeyWide() : L"",
			NULL != pPlot && pPlot->isRoute() ? GC.getInfo(pPlot->getRouteType()).getTextKeyWide() : L"",
			NO_CORPORATION != eCorporation ? GC.getInfo(eCorporation).getTextKeyWide() : L""
			);

	}
	else
	{
		pTriggerData->m_szText = L"";
	}

	if (kTrigger.getNumWorldNews() > 0)
	{
		int iText = GC.getGame().getSorenRandNum(kTrigger.getNumWorldNews(), "Trigger World News choice");

		pTriggerData->m_szGlobalText = gDLL->getText(kTrigger.getWorldNews(iText).GetCString(),
			getCivilizationAdjectiveKey(),
			NULL != pCity ? pCity->getNameKey() : L"",
			pTriggerData->m_eReligion != NO_RELIGION ? GC.getInfo(pTriggerData->m_eReligion).getAdjectiveKey() : L"",
			eOtherPlayer != NO_PLAYER ? GET_PLAYER(eOtherPlayer).getCivilizationAdjectiveKey() : L"",
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			pTriggerData->m_eCorporation != NO_CORPORATION ? GC.getInfo(pTriggerData->m_eCorporation).getTextKeyWide() : L""
			);
	}
	else
	{
		pTriggerData->m_szGlobalText.clear();
	}

	if (bFire)
	{
		trigger(*pTriggerData);
	}

	return pTriggerData;
}


bool CvPlayer::canDoEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const
{
	if (eEvent == NO_EVENT)
	{
		FAssert(false);
		return false;
	}

	CvEventInfo& kEvent = GC.getInfo(eEvent);

	int iGold = std::min(getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, false), getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, true));

	if (iGold != 0)
	{
		if (iGold > 0 && NO_PLAYER != kTriggeredData.m_eOtherPlayer && kEvent.isGoldToPlayer())
		{
			if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).getGold() < iGold)
			{
				return false;
			}
		}
		else if (iGold < 0)
		{
			if (getGold() < -iGold)
			{
				return false;
			}
		}
	}

	if (kEvent.getSpaceProductionModifier() != 0)
	{
		bool bValid = false;
		for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
		{
			CvProjectInfo& kProject = GC.getInfo((ProjectTypes)iProject);
			if (kProject.isSpaceship())
			{
				if (kProject.getVictoryPrereq() != NO_VICTORY)
				{
					if (GC.getGame().isVictoryValid((VictoryTypes)(kProject.getVictoryPrereq())))
					{
						bValid = true;
						break;
					}
				}
			}
		}

		if (!bValid)
		{
			return false;
		}
	}

	if (kEvent.getEspionagePoints() > 0 && GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		return false;
	}

	if (NO_PLAYER != kTriggeredData.m_eOtherPlayer)
	{
		if (kEvent.getEspionagePoints() + GET_TEAM(getTeam()).getEspionagePointsAgainstTeam(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()) < 0)
		{
			return false;
		}
	}

	if (kEvent.getTechPercent() != 0 || kEvent.getTechCostPercent() != 0)
	{
		if (NO_TECH == getBestEventTech(eEvent, kTriggeredData.m_eOtherPlayer))
		{
			return false;
		}
	}

	if (NO_TECH != kEvent.getPrereqTech())
	{
		if (!GET_TEAM(getTeam()).isHasTech((TechTypes)kEvent.getPrereqTech()))
		{
			return false;
		}
	}

	if (NO_BONUS != kEvent.getBonusGift())
	{
		BonusTypes eBonus = (BonusTypes)kEvent.getBonusGift();
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		if (!canTradeNetworkWith(kTriggeredData.m_eOtherPlayer))
		{
			return false;
		}

		if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).getNumAvailableBonuses(eBonus) > 0)
		{
			return false;
		}

		if (getNumTradeableBonuses(eBonus) <= 1)
		{
			return false;
		}
	}
	{
		UnitClassTypes eUnitClass = (UnitClassTypes)kEvent.getUnitClass(); // advc
		if (eUnitClass != NO_UNITCLASS)
		{
			UnitTypes eUnit = getCivilization().getUnit(eUnitClass);
			if (eUnit == NO_UNIT)
				return false;
		}
	}
	if (kEvent.isCityEffect())
	{
		CvCity* pCity =	getCity(kTriggeredData.m_iCityId);
		if (NULL == pCity || !pCity->canApplyEvent(eEvent, kTriggeredData))
			return false;
	}
	else if (kEvent.isOtherPlayerCityEffect())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
			return false;

		CvCity* pCity = GET_PLAYER(kTriggeredData.m_eOtherPlayer).getCity(kTriggeredData.m_iOtherPlayerCityId);
		if (NULL == pCity || !pCity->canApplyEvent(eEvent, kTriggeredData))
		{
			return false;
		}
	}

	if (GC.getInfo(kTriggeredData.m_eTrigger).isPlotEventTrigger())
	{
		CvPlot* pPlot = GC.getMap().plot(kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY);
		if (NULL != pPlot)
		{
			if (!pPlot->canApplyEvent(eEvent))
				return false;
		}
	}

	CvUnit* pUnit = getUnit(kTriggeredData.m_iUnitId);
	if (NULL != pUnit)
	{
		if (!pUnit->canApplyEvent(eEvent))
			return false;
	}

	if (NO_BONUS != kEvent.getBonusRevealed())
	{
		if (GET_TEAM(getTeam()).isHasTech(GC.getInfo((BonusTypes)kEvent.getBonusRevealed()).
			getTechReveal()))
		{
			return false;
		}

		if (GET_TEAM(getTeam()).isForceRevealedBonus((BonusTypes)kEvent.getBonusRevealed()))
		{
			return false;
		}
	}

	if (kEvent.getConvertOwnCities() > 0)
	{
		bool bFoundValid = false;

		if (NO_RELIGION != kTriggeredData.m_eReligion)
		{
			FOR_EACH_CITY(pLoopCity, *this)
			{
				if (!pLoopCity->isHasReligion(kTriggeredData.m_eReligion))
				{
					if (-1 == kEvent.getMaxNumReligions() || pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions())
					{
						bFoundValid = true;
						break;
					}
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kEvent.getConvertOtherCities() > 0)
	{
		bool bFoundValid = false;

		if (kTriggeredData.m_eReligion != NO_RELIGION)
		{
			if (kTriggeredData.m_eOtherPlayer != NO_PLAYER)
			{
				FOR_EACH_CITY(pLoopCity, GET_PLAYER(kTriggeredData.m_eOtherPlayer))
				{
					if (!pLoopCity->isHasReligion(kTriggeredData.m_eReligion))
					{
						if (-1 == kEvent.getMaxNumReligions() || pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions())
						{
							bFoundValid = true;
							break;
						}
					}
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kEvent.getAttitudeModifier() != 0)
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam() == getTeam())
		{
			return false;
		}

		if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).isHuman())
		{
			if (kEvent.getOurAttitudeModifier() == 0)
			{
				return false;
			}
		}
	}

	if (kEvent.getTheirEnemyAttitudeModifier() != 0)
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		TeamTypes eWorstEnemy = GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		if (NO_TEAM == eWorstEnemy || eWorstEnemy == getTeam())
		{
			return false;
		}

		if (!GET_TEAM(eWorstEnemy).isAlive())
		{
			return false;
		}

		if (eWorstEnemy == getTeam())
		{
			return false;
		}
	}

	if (kEvent.isDeclareWar())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		if (!GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).canDeclareWar(getTeam()) || !GET_TEAM(getTeam()).canDeclareWar(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()))
		{
			return false;
		}
	}

	if (kEvent.isQuest())
	{
		for (int iTrigger = 0; iTrigger < GC.getNumEventTriggerInfos(); ++iTrigger)
		{
			CvEventTriggerInfo& kTrigger = GC.getInfo((EventTriggerTypes)iTrigger);
			if (!kTrigger.isRecurring())
			{
				for (int i = 0; i < kTrigger.getNumPrereqEvents(); ++i)
				{
					if (kTrigger.getPrereqEvent(i) == eEvent)
					{
						if (isTriggerFired((EventTriggerTypes)iTrigger))
						{
							return false;
						}
					}
				}
			}
		}
	}
	if (!GC.getPythonCaller()->canDoEvent(eEvent, kTriggeredData))
		return false;

	return true;
}


void CvPlayer::applyEvent(EventTypes eEvent, int iEventTriggeredId, bool bUpdateTrigger)
{
	FAssert(eEvent != NO_EVENT);

	EventTriggeredData* pTriggeredData = getEventTriggered(iEventTriggeredId);

	if (pTriggeredData == NULL)
	{
		deleteEventTriggered(iEventTriggeredId);
		return;
	}

	if (bUpdateTrigger)
	{
		setTriggerFired(*pTriggeredData, true);
	}

	if (!canDoEvent(eEvent, *pTriggeredData))
	{
		if (bUpdateTrigger)
		{
			deleteEventTriggered(iEventTriggeredId);
		}
		return;
	}

	setEventOccured(eEvent, *pTriggeredData);

	CvEventInfo& kEvent = GC.getInfo(eEvent);
	CvCity* pCity =	getCity(pTriggeredData->m_iCityId);
	CvCity* pOtherPlayerCity = NULL;

	if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		pOtherPlayerCity = GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCity(pTriggeredData->m_iOtherPlayerCityId);
	}

	int iGold = getEventCost(eEvent, pTriggeredData->m_eOtherPlayer, false);
	int iRandomGold = getEventCost(eEvent, pTriggeredData->m_eOtherPlayer, true);

	iGold += GC.getGame().getSorenRandNum(iRandomGold - iGold + 1, "Event random gold");

	if (iGold != 0)
	{
		changeGold(iGold);

		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer && kEvent.isGoldToPlayer())
		{
			GET_PLAYER(pTriggeredData->m_eOtherPlayer).changeGold(-iGold);
		}
	}

	if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		if (kEvent.getEspionagePoints() != 0)
		{
			GET_TEAM(getTeam()).changeEspionagePointsAgainstTeam(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam(), kEvent.getEspionagePoints());
		}
	}

	if (kEvent.getTechPercent() != 0)
	{
		TechTypes eBestTech = getBestEventTech(eEvent, pTriggeredData->m_eOtherPlayer);

		if (eBestTech != NO_TECH)
		{
			int iBeakers  = GET_TEAM(getTeam()).changeResearchProgressPercent(
					eBestTech, kEvent.getTechPercent(), getID());
			if (iBeakers > 0)
			{
				for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iI).getTeam()
								== getTeam()) // kmodx: was getID()
						{
							CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PROGRESS_TOWARDS_TECH",
									iBeakers, GC.getInfo(eBestTech).getTextKeyWide());
							gDLL->UI().addMessage((PlayerTypes)iI, false, -1, szBuffer, NULL,
									MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getColorType("TECH_TEXT"));
						}
					}
				}
			}
		}
	}

	if (kEvent.isGoldenAge())
	{
		changeGoldenAgeTurns(getGoldenAgeLength());
	}

	if (kEvent.getInflationModifier() != 0)
	{
		m_iInflationModifier += kEvent.getInflationModifier();
	}

	if (kEvent.getSpaceProductionModifier() != 0)
	{
		changeSpaceProductionModifier(kEvent.getSpaceProductionModifier());
	}

	if (kEvent.getFreeUnitSupport() != 0)
	{
		changeBaseFreeUnits(kEvent.getFreeUnitSupport());
	}

	if (kEvent.isDeclareWar())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			if (gTeamLogLevel >= 2) // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000
				logBBAI("    Team %d (%S) declares war on team %d due to event", GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam(), GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationDescription(0), getTeam());
			GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).declareWar(getTeam(), false, WARPLAN_LIMITED,
					true, NO_PLAYER, true); // advc.106g
		}
	}

	if (NO_BONUS != kEvent.getBonusGift())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			CLinkList<TradeData> ourList;
			CLinkList<TradeData> theirList;
			ourList.insertAtEnd(TradeData(TRADE_RESOURCES, kEvent.getBonusGift()));
			GC.getGame().implementDeal(getID(), pTriggeredData->m_eOtherPlayer, ourList, theirList);
		}
	}

	bool bClear = false;
	for (int iEvent = 0; iEvent < GC.getNumEventInfos(); ++iEvent)
	{
		if (kEvent.getClearEventChance(iEvent) > 0)
		{
			bClear = GC.getGame().getSorenRandNum(100, "Event Clear") < kEvent.getClearEventChance(iEvent);
			if (bClear)
			{
				if (kEvent.isGlobal())
				{
					for (int j = 0; j < MAX_CIV_PLAYERS; j++)
					{
						GET_PLAYER((PlayerTypes)j).resetEventOccured((EventTypes)iEvent, j != getID());
					}
				}
				else if (kEvent.isTeam())
				{
					for (int j = 0; j < MAX_CIV_PLAYERS; j++)
					{
						if (getTeam() == GET_PLAYER((PlayerTypes)j).getTeam())
						{
							GET_PLAYER((PlayerTypes)j).resetEventOccured((EventTypes)iEvent, j != getID());
						}
					}
				}
				else
				{
					resetEventOccured((EventTypes)iEvent, false);
				}
			}
		}
	}

	if (NULL != pCity && kEvent.isCityEffect())
	{
		pCity->applyEvent(eEvent, *pTriggeredData, bClear);
	}
	else if (NULL != pOtherPlayerCity && kEvent.isOtherPlayerCityEffect())
	{
		pOtherPlayerCity->applyEvent(eEvent, *pTriggeredData, bClear);
	}

	if (!kEvent.isCityEffect() && !kEvent.isOtherPlayerCityEffect())
	{
		if (kEvent.getHappy() != 0)
		{
			changeExtraHappiness(kEvent.getHappy());
		}

		if (kEvent.getHealth() != 0)
		{
			changeExtraHealth(kEvent.getHealth());
		}

		if (kEvent.getNumBuildingYieldChanges() > 0)
		{
			for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					FOR_EACH_CITY_VAR(pLoopCity, *this)
					{
						pLoopCity->changeBuildingYieldChange((BuildingClassTypes)iBuildingClass,
								(YieldTypes)iYield, kEvent.getBuildingYieldChange(iBuildingClass, iYield));
					}
				}
			}
		}

		if (kEvent.getNumBuildingCommerceChanges() > 0)
		{
			for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
			{
				for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
				{
					FOR_EACH_CITY_VAR(pLoopCity, *this)
					{
						pLoopCity->changeBuildingCommerceChange((BuildingClassTypes)iBuildingClass,
								(CommerceTypes)iCommerce, kEvent.getBuildingCommerceChange(iBuildingClass, iCommerce));
					}
				}
			}
		}

		if (kEvent.getNumBuildingHappyChanges() > 0)
		{
			CvCivilization const& kCiv = getCivilization(); // advc.003w
			for (int i = 0; i < kCiv.getNumBuildings(); i++)
			{
				BuildingTypes eBuilding = kCiv.buildingAt(i);
				BuildingClassTypes eBuildingClass = kCiv.buildingClassAt(i);
				if (kEvent.getBuildingHappyChange(eBuildingClass) != 0)
				{
					changeExtraBuildingHappiness(eBuilding, kEvent.getBuildingHappyChange(eBuildingClass));
				}
			}
		}

		if (kEvent.getNumBuildingHealthChanges() > 0)
		{
			CvCivilization const& kCiv = getCivilization(); // advc.003w
			for (int i = 0; i < kCiv.getNumBuildings(); i++)
			{
				BuildingTypes eBuilding = kCiv.buildingAt(i);
				BuildingClassTypes eBuildingClass = kCiv.buildingClassAt(i);
				if (kEvent.getBuildingHealthChange(eBuildingClass) != 0)
				{
					changeExtraBuildingHealth(eBuilding, kEvent.getBuildingHealthChange(eBuildingClass));
				}
			}
		}

		if (kEvent.getHurryAnger() != 0)
		{
			FOR_EACH_CITY_VAR(pLoopCity, *this)
				pLoopCity->changeHurryAngerTimer(kEvent.getHurryAnger() * pLoopCity->flatHurryAngerLength());
		}

		if (kEvent.getHappyTurns() > 0)
		{
			FOR_EACH_CITY_VAR(pLoopCity, *this)
				pLoopCity->changeHappinessTimer(kEvent.getHappyTurns());
		}

		if (kEvent.getMaxPillage() > 0)
		{
			FAssert(kEvent.getMaxPillage() >= kEvent.getMinPillage());
			int iNumPillage = kEvent.getMinPillage() + GC.getGame().getSorenRandNum(kEvent.getMaxPillage() - kEvent.getMinPillage(), "Pick number of event pillaged plots");

			int iNumPillaged = 0;
			for (int i = 0; i < iNumPillage; ++i)
			{
				int iRandOffset = GC.getGame().getSorenRandNum(GC.getMap().numPlots(), "Pick event pillage plot (any city)");
				for (int j = 0; j < GC.getMap().numPlots(); ++j)
				{
					int iPlot = (j + iRandOffset) % GC.getMap().numPlots();
					CvPlot* pPlot = GC.getMap().plotByIndex(iPlot);
					if (NULL != pPlot && pPlot->getOwner() == getID() && pPlot->isCity())
					{
						if (pPlot->isImproved() && !GC.getInfo(pPlot->getImprovementType()).isPermanent())
						{
							CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED",
									GC.getInfo(pPlot->getImprovementType()).getTextKeyWide());
							gDLL->UI().addMessage(getID(), false, -1, szBuffer, *pPlot, "AS2D_PILLAGED",
									MESSAGE_TYPE_INFO, GC.getInfo(pPlot->getImprovementType()).getButton(),
									GC.getColorType("RED"));
							pPlot->setImprovementType(NO_IMPROVEMENT);
							++iNumPillaged;
							break;
						}
					}
				}
			}

			if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
			{
				CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_NUM_CITY_IMPROVEMENTS_DESTROYED",
						iNumPillaged, getCivilizationAdjectiveKey());
				gDLL->UI().addMessage(pTriggeredData->m_eOtherPlayer, false, -1, szBuffer,
						"AS2D_PILLAGED", MESSAGE_TYPE_INFO);
			}
		}

		if (kEvent.getFood() != 0)
		{
			FOR_EACH_CITY_VAR(pLoopCity, *this)
				pLoopCity->changeFood(kEvent.getFood());
		}

		if (kEvent.getFoodPercent() != 0)
		{
			FOR_EACH_CITY_VAR(pLoopCity, *this)
				pLoopCity->changeFood((pLoopCity->getFood() * kEvent.getFoodPercent()) / 100);
		}

		if (kEvent.getPopulationChange() != 0)
		{
			FOR_EACH_CITY_VAR(pLoopCity, *this)
			{
				if (pLoopCity->getPopulation() + kEvent.getPopulationChange() > 0)
					pLoopCity->changePopulation(kEvent.getPopulationChange());
			}
		}

		if (kEvent.getCulture() != 0)
		{
			FOR_EACH_CITY_VAR(pLoopCity, *this)
			{
				if (pLoopCity->getCultureTimes100(pLoopCity->getOwner()) + 100 * kEvent.getCulture() > 0)
					pLoopCity->changeCulture(pLoopCity->getOwner(), kEvent.getCulture(), true, true);
			}
		}
		{
			UnitClassTypes eUnitClass = (UnitClassTypes)kEvent.getUnitClass(); // advc
			if (eUnitClass != NO_UNITCLASS)
			{
				UnitTypes eUnit = getCivilization().getUnit(eUnitClass);
				if (eUnit != NO_UNIT)
				{
					CvCity* pUnitCity = pCity;
					if (NULL == pUnitCity)
						pUnitCity = getCapitalCity();

					if (NULL != pUnitCity)
					{
						for (int i = 0; i < kEvent.getNumUnits(); ++i)
							initUnit(eUnit, pUnitCity->getX(), pUnitCity->getY());
					}
				}
			}
		}
	}

	CvPlot* pPlot = GC.getMap().plot(pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY);
	if (NULL != pPlot)
	{
		if (GC.getInfo(pTriggeredData->m_eTrigger).isPlotEventTrigger())
		{
			FAssert(pPlot->canApplyEvent(eEvent));
			pPlot->applyEvent(eEvent);
		}
	}

	CvUnit* pUnit = getUnit(pTriggeredData->m_iUnitId);
	if (NULL != pUnit)
	{
		FAssert(pUnit->canApplyEvent(eEvent));
		pUnit->applyEvent(eEvent);   // might kill the unit
	}

	for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
	{
		if (NO_PROMOTION != kEvent.getUnitCombatPromotion(i))
		{
			FOR_EACH_UNIT_VAR(pLoopUnit, *this)
			{
				if (pLoopUnit->getUnitCombatType() == i)
					pLoopUnit->setHasPromotion((PromotionTypes)kEvent.getUnitCombatPromotion(i), true);
			}
			setFreePromotion((UnitCombatTypes)i, (PromotionTypes)kEvent.getUnitCombatPromotion(i), true);
		}
	}

	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		if (kEvent.getUnitClassPromotion(i) == NO_PROMOTION)
			continue;

		FOR_EACH_UNIT_VAR(pLoopUnit, *this)
		{
			if (pLoopUnit->getUnitClassType() == i)
				pLoopUnit->setHasPromotion((PromotionTypes)kEvent.getUnitClassPromotion(i), true);
		}
		setFreePromotion((UnitClassTypes)i, (PromotionTypes)kEvent.getUnitClassPromotion(i), true);
	}

	if (kEvent.getBonusRevealed() != NO_BONUS)
	{
		GET_TEAM(getTeam()).setForceRevealedBonus((BonusTypes)kEvent.getBonusRevealed(), true);
	}

	std::vector<CvCity*> apSpreadReligionCities;

	if (kEvent.getConvertOwnCities() > 0)
	{
		if (NO_RELIGION != pTriggeredData->m_eReligion)
		{
			FOR_EACH_CITY_VAR(pLoopCity, *this)
			{
				if (!pLoopCity->isHasReligion(pTriggeredData->m_eReligion))
				{
					if (kEvent.getMaxNumReligions() == -1 || pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions())
						apSpreadReligionCities.push_back(pLoopCity);
				}
			}
		}
	}

	while ((int)apSpreadReligionCities.size() > kEvent.getConvertOwnCities())
	{
		int iChosen = GC.getGame().getSorenRandNum(apSpreadReligionCities.size(), "Even Spread Religion (own)");

		int i = 0;
		for (std::vector<CvCity*>::iterator it = apSpreadReligionCities.begin(); it != apSpreadReligionCities.end(); ++it)
		{
			if (i == iChosen)
			{
				apSpreadReligionCities.erase(it);
				break;
			}
			++i;
		}
	}

	for (std::vector<CvCity*>::iterator it = apSpreadReligionCities.begin(); it != apSpreadReligionCities.end(); ++it)
	{
		(*it)->setHasReligion(pTriggeredData->m_eReligion, true, true, false);
	}

	apSpreadReligionCities.clear();

	if (kEvent.getConvertOtherCities() > 0)
	{
		if (NO_RELIGION != pTriggeredData->m_eReligion)
		{
			if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
			{
				// kmodx: removed redundant code
				FOR_EACH_CITY_VAR(pLoopCity, GET_PLAYER(pTriggeredData->m_eOtherPlayer))
				{
					if (!pLoopCity->isHasReligion(pTriggeredData->m_eReligion))
					{
						if (kEvent.getMaxNumReligions() == -1 || pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions())
						{
							apSpreadReligionCities.push_back(pLoopCity);
						}
					}
				}
			}
		}
	}

	while ((int)apSpreadReligionCities.size() > kEvent.getConvertOtherCities())
	{
		int iChosen = GC.getGame().getSorenRandNum(apSpreadReligionCities.size(), "Even Spread Religion (other)");

		int i = 0;
		for (std::vector<CvCity*>::iterator it = apSpreadReligionCities.begin(); it != apSpreadReligionCities.end(); ++it)
		{
			if (i == iChosen)
			{
				apSpreadReligionCities.erase(it);
				break;
			}
			++i;
		}
	}

	for (std::vector<CvCity*>::iterator it = apSpreadReligionCities.begin(); it != apSpreadReligionCities.end(); ++it)
	{
		(*it)->setHasReligion(pTriggeredData->m_eReligion, true, true, false);
	}

	if (kEvent.getOurAttitudeModifier() != 0)
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			if (kEvent.getOurAttitudeModifier() > 0)
			{
				AI().AI_changeMemoryCount(pTriggeredData->m_eOtherPlayer, MEMORY_EVENT_GOOD_TO_US, kEvent.getOurAttitudeModifier());
			}
			else
			{
				AI().AI_changeMemoryCount(pTriggeredData->m_eOtherPlayer, MEMORY_EVENT_BAD_TO_US, -kEvent.getOurAttitudeModifier());
			}
		}
	}

	if (kEvent.getAttitudeModifier() != 0)
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{	// <advc.130j> Replace AI_changeMemoryCount with multiple AI_rememberEvent calls */
			CvPlayerAI& kOther = GET_PLAYER(pTriggeredData->m_eOtherPlayer);
			if(kEvent.getAttitudeModifier() > 0)
			{
				for(int i = 0; i < kEvent.getAttitudeModifier(); i++)
					kOther.AI_rememberEvent(getID(), MEMORY_EVENT_GOOD_TO_US);
			}
			else
			{
				for(int i = 0; i < -kEvent.getAttitudeModifier(); i++)
					kOther.AI_rememberEvent(getID(), MEMORY_EVENT_BAD_TO_US);
			} // </advc.130j>
		}
	}

	if (kEvent.getTheirEnemyAttitudeModifier() != 0)
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			TeamTypes eWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
			if (NO_TEAM != eWorstEnemy)
			{
				for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
				{
					CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
					if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == eWorstEnemy)
					{
						if (kEvent.getTheirEnemyAttitudeModifier() > 0)
						{
							kLoopPlayer.AI_changeMemoryCount(getID(), MEMORY_EVENT_GOOD_TO_US, kEvent.getTheirEnemyAttitudeModifier());
							AI().AI_changeMemoryCount((PlayerTypes)iPlayer, MEMORY_EVENT_GOOD_TO_US, kEvent.getTheirEnemyAttitudeModifier());
						}
						else
						{
							kLoopPlayer.AI_changeMemoryCount(getID(), MEMORY_EVENT_BAD_TO_US, -kEvent.getTheirEnemyAttitudeModifier());
							AI().AI_changeMemoryCount((PlayerTypes)iPlayer, MEMORY_EVENT_BAD_TO_US, -kEvent.getTheirEnemyAttitudeModifier());
						}
					}
				}
			}
		}
	}

	GC.getPythonCaller()->applyEvent(eEvent, *pTriggeredData);

	if (kEvent.getNumWorldNews() > 0)
	{
		int iText = GC.getGame().getSorenRandNum(kEvent.getNumWorldNews(), "Event World News choice");

		CvWString szGlobalText;

		TeamTypes eTheirWorstEnemy = NO_TEAM;
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			eTheirWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		}

		szGlobalText = gDLL->getText(kEvent.getWorldNews(iText).GetCString(),
			getCivilizationAdjectiveKey(),
			NULL != pCity ? pCity->getNameKey() : L"",
			pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"",
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			NO_RELIGION != pTriggeredData->m_eReligion ? GC.getInfo(pTriggeredData->m_eReligion).getAdjectiveKey() : L"",
			NO_TEAM != eTheirWorstEnemy ? GET_TEAM(eTheirWorstEnemy).getName().GetCString() : L"",
			NO_CORPORATION != pTriggeredData->m_eCorporation ? GC.getInfo(pTriggeredData->m_eCorporation).getTextKeyWide() : L""
			);

		for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (!kLoopPlayer.isAlive())
				continue;

			if (GET_TEAM(kLoopPlayer.getTeam()).isHasMet(getTeam()) &&
				(NO_PLAYER == pTriggeredData->m_eOtherPlayer ||
				GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).isHasMet(getTeam())))
			{
				bool bShowPlot = GC.getInfo(pTriggeredData->m_eTrigger).isShowPlot();
				if (bShowPlot)
				{
					if (kLoopPlayer.getTeam() != getTeam())
					{
						if (NULL == pPlot || !pPlot->isRevealed(kLoopPlayer.getTeam()))
							bShowPlot = false;
					}
				}
				if (bShowPlot)
				{
					gDLL->UI().addMessage((PlayerTypes)iPlayer, false, -1, szGlobalText,
							"AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, NO_COLOR,
							pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, true, true);
				}
				else
				{
					gDLL->UI().addMessage((PlayerTypes)iPlayer, false, -1, szGlobalText,
							"AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT);
				}
			}
		}
		// advc.106g: Don't show (most) random events in replays
		//GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szGlobalText, pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
	}

	if (!CvWString(kEvent.getLocalInfoTextKey()).empty())
	{
		CvWString szLocalText;

		TeamTypes eTheirWorstEnemy = NO_TEAM;
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			eTheirWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		}

		szLocalText = gDLL->getText(kEvent.getLocalInfoTextKey(),
			getCivilizationAdjectiveKey(),
			NULL != pCity ? pCity->getNameKey() : L"",
			pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"",
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			NO_RELIGION != pTriggeredData->m_eReligion ? GC.getInfo(pTriggeredData->m_eReligion).getAdjectiveKey() : L"",
			NO_TEAM != eTheirWorstEnemy ? GET_TEAM(eTheirWorstEnemy).getName().GetCString() : L"",
			NO_CORPORATION != pTriggeredData->m_eCorporation ? GC.getInfo(pTriggeredData->m_eCorporation).getTextKeyWide() : L""
			);

			if (GC.getInfo(pTriggeredData->m_eTrigger).isShowPlot())
			{
				gDLL->UI().addMessage(getID(), false, -1, szLocalText, "AS2D_CIVIC_ADOPT",
						MESSAGE_TYPE_MINOR_EVENT, NULL, NO_COLOR,
						pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, true, true);
			}
			else
			{
				gDLL->UI().addMessage(getID(), false, -1, szLocalText, "AS2D_CIVIC_ADOPT",
						MESSAGE_TYPE_MINOR_EVENT, NULL, NO_COLOR);
			}

	}

	if (!CvWString(kEvent.getOtherPlayerPopup()).empty())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			CvWString szText = gDLL->getText(kEvent.getOtherPlayerPopup(),
				getCivilizationAdjectiveKey(),
				NULL != pCity ? pCity->getNameKey() : L"",
				pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"",
				NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
				NO_RELIGION != pTriggeredData->m_eReligion ? GC.getInfo(pTriggeredData->m_eReligion).getAdjectiveKey() : L"",
				NO_CORPORATION != pTriggeredData->m_eCorporation ? GC.getInfo(pTriggeredData->m_eCorporation).getTextKeyWide() : L""
				);

			CvPopupInfo* pInfo = new CvPopupInfo();

			if (NULL != pInfo)
			{
				pInfo->setText(szText);

				GET_PLAYER(pTriggeredData->m_eOtherPlayer).addPopup(pInfo);
			}
		}
	}

	bool bDeleteTrigger = bUpdateTrigger;

	for (int iEvent = 0; iEvent < GC.getNumEventInfos(); ++iEvent)
	{
		if (kEvent.getAdditionalEventTime(iEvent) == 0)
		{
			if (kEvent.getAdditionalEventChance(iEvent) > 0)
			{
				if (canDoEvent((EventTypes)iEvent, *pTriggeredData))
				{
					if (GC.getGame().getSorenRandNum(100, "Additional Event") < kEvent.getAdditionalEventChance(iEvent))
					{
						applyEvent((EventTypes)iEvent, iEventTriggeredId, false);
					}
				}
			}
		}
		else
		{
			bool bSetTimer = true;
			if (kEvent.getAdditionalEventChance(iEvent) > 0)
			{
				if (GC.getGame().getSorenRandNum(100, "Additional Event 2") >= kEvent.getAdditionalEventChance(iEvent))
				{
					bSetTimer = false;
				}
			}

			if (bSetTimer)
			{
				EventTriggeredData kTriggered = *pTriggeredData;
				kTriggered.m_iTurn = (GC.getInfo(GC.getGame().getGameSpeedType()).getGrowthPercent() * kEvent.getAdditionalEventTime((EventTypes)iEvent)) / 100 + GC.getGame().getGameTurn();

				const EventTriggeredData* pExistingTriggered = getEventCountdown((EventTypes)iEvent);

				if (NULL != pExistingTriggered)
				{
					kTriggered.m_iTurn = std::min(kTriggered.m_iTurn, pExistingTriggered->m_iTurn);
				}

				setEventCountdown((EventTypes)iEvent, kTriggered);
				bDeleteTrigger = false;
			}
		}
	}

	if (bDeleteTrigger)
	{
		deleteEventTriggered(iEventTriggeredId);
	}
}

bool CvPlayer::isValidEventTech(TechTypes eTech, EventTypes eEvent, PlayerTypes eOtherPlayer) const
{
	CvEventInfo& kEvent = GC.getInfo(eEvent);

	if (kEvent.getTechPercent() == 0 && kEvent.getTechCostPercent() == 0)
	{
		return false;
	}

	if (kEvent.getTechPercent() < 0 &&
		(GET_TEAM(getTeam()).isHasTech(eTech) || // advc: Don't rely on getResearchProgress
		GET_TEAM(getTeam()).getResearchProgress(eTech) <= 0))
	{
		return false;
	}

	if (!canResearch(eTech))
	{
		return false;
	}
	/*  advc.004x (comment): No change here; OK with me to make all tech invalid
		during anarchy (ResearchTurnsLeft=-1). */
	if (getResearchTurnsLeft(eTech, true) < kEvent.getTechMinTurnsLeft())
	{
		return false;
	}

	if (NO_PLAYER != eOtherPlayer && !GET_TEAM(eOtherPlayer).isHasTech(eTech))
	{
		return false;
	}

	return true;
}


TechTypes CvPlayer::getBestEventTech(EventTypes eEvent, PlayerTypes eOtherPlayer) const
{
	TechTypes eBestTech = NO_TECH;
	CvEventInfo& kEvent = GC.getInfo(eEvent);

	if (kEvent.getTechPercent() == 0 && kEvent.getTechCostPercent() == 0)
	{
		return NO_TECH;
	}

	if (NO_TECH != kEvent.getTech())
	{
		eBestTech = (TechTypes)kEvent.getTech();
	}
	else
	{
		bool bFoundFlavor = false;
		for (int i = 0; i < GC.getNumFlavorTypes(); ++i)
		{
			if (kEvent.getTechFlavorValue(i) != 0)
			{
				bFoundFlavor = true;
				break;
			}
		}

		if (!bFoundFlavor)
		{
			eBestTech = getCurrentResearch();
		}
	}

	if (NO_TECH != eBestTech)
	{
		if (!isValidEventTech(eBestTech, eEvent, eOtherPlayer))
		{
			eBestTech = NO_TECH;
		}
	}
	else
	{
		int iBestValue = 0;
		for (int iTech = 0; iTech < GC.getNumTechInfos(); ++iTech)
		{
			if (isValidEventTech((TechTypes)iTech, eEvent, eOtherPlayer))
			{
				int iValue = 0;
				for (int i = 0; i < GC.getNumFlavorTypes(); ++i)
				{
					iValue += kEvent.getTechFlavorValue(i) * GC.getInfo((TechTypes)iTech).getFlavorValue(i);
				}

				if (iValue > iBestValue)
				{
					eBestTech = (TechTypes)iTech;
					iBestValue = iValue;
				}
			}
		}
	}

	return eBestTech;
}

int CvPlayer::getEventCost(EventTypes eEvent, PlayerTypes eOtherPlayer, bool bRandom) const
{
	CvEventInfo& kEvent = GC.getInfo(eEvent);

	int iGold = kEvent.getGold();
	if (bRandom)
	{
		iGold += kEvent.getRandomGold();
	}

	iGold *= std::max(0, calculateInflationRate() + 100);
	iGold /= 100;

	TechTypes eBestTech = getBestEventTech(eEvent, eOtherPlayer);

	if (NO_TECH != eBestTech)
	{
		iGold -= (kEvent.getTechCostPercent() * GET_TEAM(getTeam()).getResearchCost(eBestTech)) / 100;
	}

	return iGold;
}


void CvPlayer::doEvents()
{
	if (GC.getGame().isOption(GAMEOPTION_NO_EVENTS))
	{
		return;
	}

	if (isBarbarian() || isMinorCiv())
	{
		return;
	}
	{ // advc: scope for iterator
		CvEventMap::iterator it = m_mapEventsOccured.begin();
		while (it != m_mapEventsOccured.end())
		{
			if (checkExpireEvent(it->first, it->second))
			{
				expireEvent(it->first, it->second, true);
				it = m_mapEventsOccured.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	bool bNewEventEligible = true;
	if (GC.getGame().getElapsedGameTurns() < GC.getDefineINT("FIRST_EVENT_DELAY_TURNS"))
	{
		bNewEventEligible = false;
	}

	if (bNewEventEligible)
	{
		if (GC.getGame().getSorenRandNum(GC.getDefineINT("EVENT_PROBABILITY_ROLL_SIDES"), "Global event check") >= GC.getInfo(getCurrentEra()).getEventChancePerTurn())
		{
			bNewEventEligible = false;
		}
	}

	std::vector< std::pair<EventTriggeredData*, int> > aePossibleEventTriggerWeights;
	int iTotalWeight = 0;
	for (int i = 0; i < GC.getNumEventTriggerInfos(); ++i)
	{
		int iWeight = getEventTriggerWeight((EventTriggerTypes)i);
		if (iWeight == -1)
		{
			trigger((EventTriggerTypes)i);
		}
		else if (iWeight > 0 && bNewEventEligible)
		{
			EventTriggeredData* pTriggerData = initTriggeredData((EventTriggerTypes)i);
			if (NULL != pTriggerData)
			{
				iTotalWeight += iWeight;
				aePossibleEventTriggerWeights.push_back(std::make_pair(pTriggerData, iTotalWeight));
			}
		}
	}

	if (iTotalWeight > 0)
	{
		bool bFired = false;
		int iValue = GC.getGame().getSorenRandNum(iTotalWeight, "Event trigger");
		for (std::vector< std::pair<EventTriggeredData*, int> >::iterator it = aePossibleEventTriggerWeights.begin(); it != aePossibleEventTriggerWeights.end(); ++it)
		{
			EventTriggeredData* pTriggerData = it->first;
			if (NULL != pTriggerData)
			{
				if (iValue < it->second && !bFired)
				{
					trigger(*pTriggerData);
					bFired = true;
				}
				else
				{
					deleteEventTriggered(pTriggerData->getID());
				}
			}
		}
	}

	std::vector<int> aCleanup;
	for (int i = 0; i < GC.getNumEventInfos(); ++i)
	{
		const EventTriggeredData* pTriggeredData = getEventCountdown((EventTypes)i);
		if (NULL != pTriggeredData)
		{
			if (GC.getGame().getGameTurn() >= pTriggeredData->m_iTurn)
			{
				applyEvent((EventTypes)i, pTriggeredData->m_iId);
				resetEventCountdown((EventTypes)i);
				aCleanup.push_back(pTriggeredData->m_iId);
			}
		}
	}

	for (std::vector<int>::iterator it = aCleanup.begin(); it != aCleanup.end(); ++it)
	{
		bool bDelete = true;

		for (int i = 0; i < GC.getNumEventInfos(); ++i)
		{
			const EventTriggeredData* pTriggeredData = getEventCountdown((EventTypes)i);
			if (NULL != pTriggeredData)
			{
				if (pTriggeredData->m_iId == *it)
				{
					bDelete = false;
					break;
				}
			}
		}

		if (bDelete)
		{
			deleteEventTriggered(*it);
		}
	}
}

// <advc.011>
void CvPlayer::decayBuildProgress()
{
	CvMap const& kMap = GC.getMap();
	for(int i = 0; i < kMap.numPlots(); i++)
	{
		CvPlot& p = kMap.getPlotByIndex(i);
		if(!p.isWater() && p.getOwner() == getID())
			p.decayBuildProgress();
	}
} // </advc.011>

// <advc.002e>
void CvPlayer::showForeignPromoGlow(bool b)
{
	if(BUGOption::isEnabled("PLE__ShowPromotionGlow", false))
		return;
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		CvPlayer const& kPlayer = GET_PLAYER((PlayerTypes)i);
		if(!kPlayer.isAlive() || kPlayer.getID() == getID())
			continue;
		FOR_EACH_UNIT_VAR(u, kPlayer)
		{
			gDLL->getEntityIFace()->showPromotionGlow(u->getUnitEntity(),
					u->isPromotionReady() && b);
		}
	}
}// </advc.002e>

void CvPlayer::expireEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData, bool bFail)
{
	FAssert(getEventOccured(eEvent) == &kTriggeredData);
	FAssert(GC.getInfo(eEvent).isQuest() || GC.getGame().getGameTurn() - kTriggeredData.m_iTurn <= 4);

	if (GC.getInfo(eEvent).isQuest())
	{
		CvMessageQueue::iterator it;
		for (it = m_listGameMessages.begin(); it != m_listGameMessages.end(); ++it)
		{
			CvTalkingHeadMessage& message = *it;

			// the trigger ID is stored in the otherwise unused length field
			if (message.getLength() == kTriggeredData.getID())
			{
				m_listGameMessages.erase(it);
				gDLL->getInterfaceIFace()->dirtyTurnLog(getID());
				break;
			}
		}

		if (bFail)
		{
			gDLL->UI().addMessage(getID(), false, -1,
					gDLL->getText(GC.getInfo(eEvent).getQuestFailTextKey()),
					"AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getColorType("RED"));
		}
	}
}

bool CvPlayer::checkExpireEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const  // advc: style changes
{
	if (GC.getPythonCaller()->checkExpireEvent(eEvent, kTriggeredData))
		return true;

	CvEventInfo& kEvent = GC.getInfo(eEvent);

	if (!kEvent.isQuest())
		return (GC.getGame().getGameTurn() - kTriggeredData.m_iTurn > 2);

	CvEventTriggerInfo& kTrigger = GC.getInfo(kTriggeredData.m_eTrigger);
	FAssert(kTriggeredData.m_ePlayer != NO_PLAYER);
	CvPlayer& kPlayer = GET_PLAYER(kTriggeredData.m_ePlayer);

	if (kTrigger.isStateReligion() & kTrigger.isPickReligion() &&
			kPlayer.getStateReligion() != kTriggeredData.m_eReligion)
		return true;

	if (kTrigger.getCivic() != NO_CIVIC &&
			!kPlayer.isCivic((CivicTypes)kTrigger.getCivic()))
		return true;

	if (kTriggeredData.m_iCityId != -1 &&
			kPlayer.getCity(kTriggeredData.m_iCityId) == NULL)
		return true;

	if (kTriggeredData.m_iUnitId != -1 &&
			kPlayer.getUnit(kTriggeredData.m_iUnitId) == NULL)
		return true;

	if (kTriggeredData.m_eOtherPlayer != NO_PLAYER)
	{
		if (!GET_PLAYER(kTriggeredData.m_eOtherPlayer).isAlive())
			return true;

		if (kTriggeredData.m_iOtherPlayerCityId != -1 &&
				GET_PLAYER(kTriggeredData.m_eOtherPlayer).
				getCity(kTriggeredData.m_iOtherPlayerCityId) == NULL)
			return true;
	}

	if (kTrigger.getNumObsoleteTechs() > 0)
	{
		for (int i = 0; i < kTrigger.getNumObsoleteTechs(); i++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)kTrigger.getObsoleteTech(i)))
				return true;
		}
	}

	return false;
}


void CvPlayer::trigger(EventTriggerTypes eTrigger)
{
	initTriggeredData(eTrigger, true);
}

void CvPlayer::trigger(const EventTriggeredData& kData)
{
	if (isHuman())
	{
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_EVENT, kData.getID());
		addPopup(pInfo);
	}
	else
	{
		EventTypes eEvent = AI().AI_chooseEvent(kData.getID());
		if (NO_EVENT != eEvent)
		{
			applyEvent(eEvent, kData.getID());
		}
	}
}

bool CvPlayer::canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer, ReligionTypes eReligion) const
{
	if (!isAlive())
	{
		return false;
	}

	if (getID() == ePlayer)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	CvEventTriggerInfo& kTrigger = GC.getInfo(eTrigger);

	if (getTeam() == kPlayer.getTeam())
	{
		return false;
	}

	if (!kTrigger.isPickPlayer())
	{
		return false;
	}

	if (!GET_TEAM(getTeam()).isHasMet(kPlayer.getTeam()))
	{
		return false;
	}

	if (isHuman() && kTrigger.isOtherPlayerAI())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isAtWar(kPlayer.getTeam()) != kTrigger.isOtherPlayerWar())
	{
		return false;
	}

	if (NO_TECH != kTrigger.getOtherPlayerHasTech())
	{
		if (!GET_TEAM(getTeam()).isHasTech((TechTypes)kTrigger.getOtherPlayerHasTech()))
		{
			return false;
		}
	}

	if (kTrigger.getOtherPlayerShareBorders() > 0)
	{
		int iCount = 0;
		for (int iI = 0; iI < GC.getMap().numPlots(); ++iI)
		{
			CvPlot const& kLoopPlot = GC.getMap().getPlotByIndex(iI);
			if (!kLoopPlot.isWater())
			{
				if (kLoopPlot.getOwner() == getID() && kLoopPlot.isAdjacentPlayer(ePlayer, true))
					iCount++;
			}
		}
		if (iCount < kTrigger.getOtherPlayerShareBorders())
			return false;
	}

	if (NO_RELIGION != eReligion)
	{
		bool bHasReligion = kTrigger.isStateReligion() ? (getStateReligion() == eReligion) : (getHasReligionCount(eReligion) > 0);

		if (kTrigger.isOtherPlayerHasReligion())
		{
			if (!bHasReligion)
			{
				return false;
			}
		}

		if (kTrigger.isOtherPlayerHasOtherReligion())
		{
			if (bHasReligion)
			{
				return false;
			}

			if (kTrigger.isStateReligion() && getStateReligion() == NO_RELIGION)
			{
				return false;
			}

		}
	}

	return true;
}

CvCity* CvPlayer::pickTriggerCity(EventTriggerTypes eTrigger) const
{
	CvCity* pCity = NULL;
	std::vector<CvCity*> apCities;
	int iBestValue = MIN_INT;
	FOR_EACH_CITY_VAR(pLoopCity, *this)
	{
		int iValue = pLoopCity->getTriggerValue(eTrigger);
		if (iValue >= iBestValue && iValue != MIN_INT)
		{
			if (iValue > iBestValue)
			{
				apCities.clear();
				iBestValue = iValue;
			}
			apCities.push_back(pLoopCity);
		}
	}

	if (apCities.size() > 0)
	{
		int iChosen = GC.getGame().getSorenRandNum(apCities.size(), "Event pick city");
		pCity = apCities[iChosen];
	}

	return pCity;
}

CvUnit* CvPlayer::pickTriggerUnit(EventTriggerTypes eTrigger, CvPlot* pPlot, bool bPickPlot) const
{
	CvUnit* pUnit = NULL;
	std::vector<CvUnit*> apUnits;
	int iBestValue = MIN_INT;
	FOR_EACH_UNIT_VAR(pLoopUnit, *this)
	{
		int iValue = pLoopUnit->getTriggerValue(eTrigger, pPlot, bPickPlot);
		if (iValue >= iBestValue && iValue != MIN_INT)
		{
			if (iValue > iBestValue)
			{
				apUnits.clear();
				iBestValue = iValue;
			}
			apUnits.push_back(pLoopUnit);
		}
	}

	if (apUnits.size() > 0)
	{
		int iChosen = GC.getGame().getSorenRandNum(apUnits.size(), "Event pick unit");
		pUnit = apUnits[iChosen];
	}

	return pUnit;
}

int CvPlayer::getEventTriggerWeight(EventTriggerTypes eTrigger) const
{
	CvEventTriggerInfo& kTrigger = GC.getInfo(eTrigger);

	if (NO_HANDICAP != kTrigger.getMinDifficulty())
	{
		if (GC.getInfo(GC.getGame().getHandicapType()).
				getDifficulty() < 10 * // advc.250a
				kTrigger.getMinDifficulty())
			return 0;
	}

	if (kTrigger.isSinglePlayer() && GC.getGame().isGameMultiPlayer())
		return 0;

	if (!GC.getGame().isEventActive(eTrigger))
		return 0;

	if (kTrigger.getNumObsoleteTechs() > 0)
	{
		for (int iI = 0; iI < kTrigger.getNumObsoleteTechs(); iI++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)(kTrigger.getObsoleteTech(iI))))
				return 0;
		}
	}

	if (!kTrigger.isRecurring())
	{
		if (isTriggerFired(eTrigger))
			return 0;
	}

	if (kTrigger.getNumPrereqOrTechs() > 0)
	{
		bool bFoundValid = false;

		for (int iI = 0; iI < kTrigger.getNumPrereqOrTechs(); iI++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)(kTrigger.getPrereqOrTechs(iI))))
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
			return 0;
	}


	if (kTrigger.getNumPrereqAndTechs() > 0)
	{
		bool bFoundValid = true;

		for (int iI = 0; iI < kTrigger.getNumPrereqAndTechs(); iI++)
		{
			if (!GET_TEAM(getTeam()).isHasTech((TechTypes)(kTrigger.getPrereqAndTechs(iI))))
			{
				bFoundValid = false;
				break;
			}
		}

		if (!bFoundValid)
			return 0;
	}

	if (kTrigger.getNumPrereqEvents() > 0)
	{
		bool bFoundValid = true;
		for (int iI = 0; iI < kTrigger.getNumPrereqEvents(); iI++)
		{
			if (NULL == getEventOccured((EventTypes)kTrigger.getPrereqEvent(iI)))
			{
				bFoundValid = false;
				break;
			}
		}

		if (!bFoundValid)
			return 0;
	}

	if (NO_CIVIC != kTrigger.getCivic())
	{
		bool bFoundValid = false;

		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); ++iI)
		{
			if (getCivics((CivicOptionTypes)iI) == kTrigger.getCivic())
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
			return 0;
	}

	if (kTrigger.getMinTreasury() > 0)
	{
		if (getGold() < kTrigger.getMinTreasury())
			return 0;
	}

	if (GC.getMap().getNumLandAreas() < kTrigger.getMinMapLandmass())
		return 0;

	if (kTrigger.getMinOurLandmass() > 0 || kTrigger.getMaxOurLandmass() != -1)
	{
		int iNumLandmass = 0;
		FOR_EACH_AREA(pArea)
		{
			if (!pArea->isWater())
			{
				if (pArea->getCitiesPerPlayer(getID()) > 0)
					iNumLandmass++;
			}
		}

		if (iNumLandmass < kTrigger.getMinOurLandmass())
			return 0;

		if (kTrigger.getMaxOurLandmass() != -1 && iNumLandmass > kTrigger.getMaxOurLandmass())
			return 0;
	}

	if (kTrigger.getProbability() < 0)
	{
		return kTrigger.getProbability();
	}

	int iProbability = kTrigger.getProbability();

	if (kTrigger.isProbabilityUnitMultiply() && kTrigger.getNumUnits() > 0)
	{
		int iNumUnits = 0;
		FOR_EACH_UNIT(pLoopUnit, *this)
		{
			if (pLoopUnit->getTriggerValue(eTrigger, NULL, true) != MIN_INT)
				iNumUnits++;
		}
		iProbability *= iNumUnits;
	}

	if (kTrigger.isProbabilityBuildingMultiply() && kTrigger.getNumBuildings() > 0)
	{
		int iNumBuildings = 0;
		for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
		{
			if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
			{
				iNumBuildings += getBuildingClassCount((BuildingClassTypes)kTrigger.getBuildingRequired(i));
			}
		}

		iProbability *= iNumBuildings;
	}

	return iProbability;
}


PlayerTypes CvPlayer::getSplitEmpirePlayer(CvArea const& kArea) const // advc: was iAreaId
{
	// can't create different derivative civs on the same continent
	for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive() && kLoopPlayer.getParent() == getID())
		{
			CvCity* pLoopCapital = kLoopPlayer.getCapitalCity();
			if (pLoopCapital != NULL && pLoopCapital->isArea(kArea))
				return NO_PLAYER;
		}
	}

	PlayerTypes eNewPlayer = NO_PLAYER;

	// Try to find a player who's never been in the game before
	for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		if (!GET_PLAYER((PlayerTypes)i).isEverAlive())
		{
			eNewPlayer = (PlayerTypes)i;
			break;
		}
	}

	if (eNewPlayer == NO_PLAYER)
	{
		/*	<dlph.24> Reusing a defeated player might not work correctly. advc:
			Allow human players to try it, but don't let the AI wreck the game somehow. */
		if (!isHuman())
			return NO_PLAYER; // </dlph.24>
		for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
		{
			if (!GET_PLAYER((PlayerTypes)i).isAlive())
			{
				eNewPlayer = (PlayerTypes)i;
				break;
			}
		}
	}

	return eNewPlayer;
}

bool CvPlayer::canSplitEmpire() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_VASSAL_STATES))
		return false;

	if (isAVassal())
		return false;

	CivLeaderArray aLeaders;
	if (!getSplitEmpireLeaders(aLeaders))
		return false;

	FOR_EACH_AREA(pLoopArea)
	{
		if (canSplitArea(*pLoopArea))
			return true; // advc (simplified)
	}
	return false;
}

bool CvPlayer::canSplitArea(CvArea const& kArea) const // advc: was iAreaId
{
	PROFILE_FUNC(); // advc: Moved from CvPlayerAI::AI_doSplit

	CvCity* pCapital = getCapitalCity();
	if (pCapital == NULL)
		return false;

	if (pCapital->isArea(kArea))
		return false;

	if (kArea.getCitiesPerPlayer(getID()) == 0)
		return false;

	PlayerTypes ePlayer = getSplitEmpirePlayer(kArea);
	if (ePlayer == NO_PLAYER)
		return false;

	if (!GET_PLAYER(ePlayer).isAlive())
	{
		if (kArea.getCitiesPerPlayer(getID()) <= 1)
			return false;
	}

	return true;
}

bool CvPlayer::getSplitEmpireLeaders(CivLeaderArray& aLeaders) const
{
	aLeaders.clear();

	for (int i = 0; i < GC.getNumCivilizationInfos(); ++i)
	{
		bool bValid = true;
		if (getCivilizationType() == i)
			bValid = false;

		if (bValid)
		{
			if (!GC.getInfo((CivilizationTypes)i).isPlayable() ||
				!GC.getInfo((CivilizationTypes)i).isAIPlayable())
			{
				bValid = false;
			}
		}

		if (bValid)
		{
			for (int j = 0; j < MAX_CIV_PLAYERS; ++j)
			{
				if (getID() != j && GET_PLAYER((PlayerTypes)j).isEverAlive() && GET_PLAYER((PlayerTypes)j).getCivilizationType() == i)
				{
					bValid = false;
					break;
				}
			}
		}

		if (bValid)
		{
			for (int j = 0; j < GC.getNumLeaderHeadInfos(); ++j)
			{
				bool bLeaderValid = true;
				if (!GC.getInfo((CivilizationTypes)i).isLeaders(j) && !GC.getGame().isOption(GAMEOPTION_LEAD_ANY_CIV))
				{
					bLeaderValid = false;
				}

				if (bLeaderValid)
				{
					for (int k = 0; k < MAX_CIV_PLAYERS; ++k)
					{
						if (GET_PLAYER((PlayerTypes)k).isEverAlive() && GET_PLAYER((PlayerTypes)k).getPersonalityType() == j)
						{
							bLeaderValid = false;
						}
					}
				}

				if (bLeaderValid)
					aLeaders.push_back(std::make_pair((CivilizationTypes)i, (LeaderHeadTypes)j));
			}
		}
	}

	return (aLeaders.size() > 0);
}
/*  <advc> Don't want to use area ids in parameter lists in general.
	This wrapper is needed, however, for CvMessageData. */
bool CvPlayer::splitEmpire(int iArea)
{
	CvArea* pArea = GC.getMap().getArea(iArea);
	if (pArea == NULL)
	{
		FAssert(pArea != NULL);
		return false;
	}
	return splitEmpire(*pArea);
} // </advc>

bool CvPlayer::splitEmpire(CvArea& kArea) // advc: was iAreaId; and some other style changes
{
	if (!canSplitEmpire() || !canSplitArea(kArea))
		return false;

	PlayerTypes eNewPlayer = getSplitEmpirePlayer(kArea);
	if (eNewPlayer == NO_PLAYER)
		return false;

	bool bPlayerExists = GET_TEAM(eNewPlayer).isAlive();
	FAssert(!bPlayerExists);
	CvGame& g = GC.getGame();
	CvWString szMessage; // advc.127b
	if (!bPlayerExists)
	{
		int iBestValue = -1;
		LeaderHeadTypes eBestLeader = NO_LEADER;
		CivilizationTypes eBestCiv = NO_CIVILIZATION;

		CivLeaderArray aLeaders;
		if (getSplitEmpireLeaders(aLeaders))
		{
			CivLeaderArray::iterator it;
			for (it = aLeaders.begin(); it != aLeaders.end(); ++it)
			{
				int iValue = (1 + g.getSorenRandNum(100, "Choosing Split Personality"));
				if (GC.getInfo(getCivilizationType()).getDerivativeCiv() == it->first)
					iValue += 1000;

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestLeader = it->second;
					eBestCiv = it->first;
				}
			}
		}
		if (eBestLeader == NO_LEADER || eBestCiv == NO_CIVILIZATION)
			return false;

		szMessage = gDLL->getText("TXT_KEY_MISC_EMPIRE_SPLIT", getNameKey(), GC.getInfo(eBestCiv).getShortDescriptionKey(), GC.getInfo(eBestLeader).getTextKeyWide());
		// advc.127b: Announcement loop moved down

		g.addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szMessage,
				-1, -1, GC.getColorType("HIGHLIGHT_TEXT"));

		// remove leftover culture from old recycled player
		/*  BETTER_BTS_AI_MOD, Bugfix, 12/30/08, jdog5000: commented out
			(Clearing plot culture along with many other bits of data now handled by CvGame::addPlayer) */
		/* for (int iPlot = 0; iPlot < GC.getMap().numPlots(); ++iPlot) {
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
			pLoopPlot->setCulture(eNewPlayer, 0, false, false);
		}*/

		g.addPlayer(eNewPlayer, eBestLeader, eBestCiv);
		GET_PLAYER(eNewPlayer).setParent(getID());
		GC.getInitCore().setLeaderName(eNewPlayer, GC.getInfo(eBestLeader).getTextKeyWide());

		CvTeam& kNewTeam = GET_TEAM(GET_PLAYER(eNewPlayer).getTeam());
		for (int i = 0; i < GC.getNumTechInfos(); ++i)
		{
			TechTypes eLoopTech = (TechTypes)i;
			if (GET_TEAM(getTeam()).isHasTech(eLoopTech))
			{
				kNewTeam.setHasTech(eLoopTech, true, eNewPlayer, false, false);
				if (GET_TEAM(getTeam()).isNoTradeTech(eLoopTech) ||
						(g.isOption(GAMEOPTION_NO_TECH_BROKERING)
						&& isSignificantDiscovery(eLoopTech))) // advc.550e
					kNewTeam.setNoTradeTech(eLoopTech, true);
			}
		}
		// dlph.24: Commented out
		/*for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam) {
			CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeam);
			if (kLoopTeam.isAlive()) {
				kNewTeam.setEspionagePointsAgainstTeam((TeamTypes)iTeam, GET_TEAM(getTeam()).getEspionagePointsAgainstTeam((TeamTypes)iTeam));
				kLoopTeam.setEspionagePointsAgainstTeam(GET_PLAYER(eNewPlayer).getTeam(), kLoopTeam.getEspionagePointsAgainstTeam(getTeam()));
			}
		}*/
		kNewTeam.setEspionagePointsEver(GET_TEAM(getTeam()).getEspionagePointsEver());
		GET_TEAM(getTeam()).assignVassal(TEAMID(eNewPlayer), false);
		AI().AI_updateBonusValue();
	}

	std::vector< std::pair<int, int> > aCultures;
	CvMap const& kMap = GC.getMap();
	for (int iPlot = 0; iPlot < kMap.numPlots(); iPlot++)
	{
		CvPlot& kLoopPlot = kMap.getPlotByIndex(iPlot);
		bool bTranferPlot = false;
		if (kLoopPlot.isArea(kArea))
			bTranferPlot = true;

		if (!bTranferPlot)
		{
			CvCity* pWorkingCity = kLoopPlot.getWorkingCity();
			if (pWorkingCity != NULL && pWorkingCity->getOwner() == getID() &&
				pWorkingCity->isArea(kArea))
			{
				bTranferPlot = true;
			}
		}

		if (!bTranferPlot && kLoopPlot.isWater() && kLoopPlot.isAdjacentToArea(kArea))
			bTranferPlot = true;

		if (bTranferPlot)
		{
			int iCulture = kLoopPlot.getCulture(getID());
			if (bPlayerExists)
				iCulture = std::max(iCulture, kLoopPlot.getCulture(eNewPlayer));

			aCultures.push_back(std::make_pair(iPlot, iCulture));
		}

		if (kLoopPlot.isRevealed(getTeam()))
			kLoopPlot.setRevealed(TEAMID(eNewPlayer), true, false, getTeam(), false);
	}
	std::vector<CvCity*> apAcquiredCities; // advc.104r
	FOR_EACH_CITY_VAR(pOldCity, *this)
	{
		if (!pOldCity->isArea(kArea))
			continue;

		int iCulture = pOldCity->getCultureTimes100(getID());
		CvPlot* pPlot = pOldCity->plot();
		GET_PLAYER(eNewPlayer).acquireCity(pOldCity, false, true, false);
		// advc: acquireCity kills pOldCity. Note that it's OK to do this while traversing the city list (m_cities).
		pOldCity = NULL;
		CvCity* pNewCity = pPlot->getPlotCity();
		pNewCity->setCultureTimes100(eNewPlayer, iCulture, false, false);
		/*  <advc.104r> Want to initialize UWAI data after assigning
			cities but before creating units. Therefore move
			unit placement into a separate loop. */
		apAcquiredCities.push_back(pNewCity);	
		/*for (int i = 0; i < GC.getDefineINT("COLONY_NUM_FREE_DEFENDERS"); ++i)
			pCity->initConscriptedUnit();*/ // </advc.104r>
	}
	for (uint i = 0; i < aCultures.size(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(aCultures[i].first);
		pPlot->setCulture(eNewPlayer, aCultures[i].second, true, false);
		pPlot->setCulture(getID(), 0, true, false);

		for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
		{
			if (pPlot->getRevealedOwner((TeamTypes)iTeam) == getID())
				pPlot->setRevealedOwner((TeamTypes)iTeam, eNewPlayer);
		}
	}
	// advc.130r:
	GET_PLAYER(eNewPlayer).AI_changeMemoryCount(getID(), MEMORY_INDEPENDENCE,
			// advc.130j: Twice remembered
			2 * GC.getInfo(getPersonalityType()).getFreedomAppreciation());

	g.updatePlotGroups();
	// K-Mod
	GET_PLAYER(eNewPlayer).AI_updateAttitude(getID());
	AI().AI_updateAttitude(eNewPlayer);
	// K-Mod end
	// <advc.104r>
	for(size_t i = 0; i < apAcquiredCities.size(); i++)
	{
		for(int j = 0; j < GC.getDefineINT("COLONY_NUM_FREE_DEFENDERS"); j++)
			apAcquiredCities[i]->initConscriptedUnit();
	}
	if (getUWAI.isEnabled())
		getUWAI.processNewCivInGame(eNewPlayer); // </advc.104r>
	/*  <advc.127b> Cut and pasted here b/c I want the announcement to point
		to the new capital */
	if (!bPlayerExists)
	{
		FAssert(!szMessage.empty());
		CvCity* pNewCapital = GET_PLAYER(eNewPlayer).getCapitalCity();
		for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
		{
			CvPlayer const& kObs = GET_PLAYER((PlayerTypes)i);
			if(!kObs.isAlive())
				continue;
			if (i == getID() || i == eNewPlayer || GET_TEAM(getTeam()).isHasMet(kObs.getTeam()) ||
				kObs.isSpectator()) // advc.127
			{
				bool bRev = (pNewCapital != NULL && pNewCapital->isRevealed(
						kObs.getTeam(), true));
				LPCSTR szButton = (bRev ? ARTFILEMGR.getInterfaceArtInfo(
						"INTERFACE_CITY_BAR_CAPITAL_TEXTURE")->getPath() : NULL);
				gDLL->UI().addMessage((PlayerTypes)i, false, -1, szMessage,
						"AS2D_REVOLTEND", MESSAGE_TYPE_MAJOR_EVENT,
						szButton, NO_COLOR, bRev ? pNewCapital->getX() : -1,
						bRev ? pNewCapital->getY() : -1);
			}
		}
	} // </advc.127b>
	return true;
}

bool CvPlayer::isValidTriggerReligion(const CvEventTriggerInfo& kTrigger, CvCity* pCity, ReligionTypes eReligion) const
{
	if (kTrigger.getNumReligionsRequired() > 0)
	{
		bool bFound = false;

		for (int i = 0; i < kTrigger.getNumReligionsRequired(); ++i)
		{
			if (eReligion == kTrigger.getReligionRequired(i))
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			return false;
		}
	}

	if (pCity != NULL)
	{
		if (!pCity->isHasReligion(eReligion))
		{
			return false;
		}

		if (kTrigger.isHolyCity())
		{
			if (!pCity->isHolyCity(eReligion))
			{
				return false;
			}
		}
	}
	else
	{
		if (getHasReligionCount(eReligion) == 0)
		{
			return false;
		}

		if (kTrigger.isHolyCity())
		{
			CvCity* pHolyCity = GC.getGame().getHolyCity(eReligion);
			if (pHolyCity == NULL || pHolyCity->getOwner() != getID())
			{
				return false;
			}
		}
	}

	return true;
}

bool CvPlayer::isValidTriggerCorporation(const CvEventTriggerInfo& kTrigger, CvCity* pCity, CorporationTypes eCorporation) const
{
	if (kTrigger.getNumCorporationsRequired() > 0)
	{
		bool bFound = false;

		for (int i = 0; i < kTrigger.getNumCorporationsRequired(); ++i)
		{
			if (eCorporation == kTrigger.getCorporationRequired(i))
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			return false;
		}
	}

	if (NULL != pCity)
	{
		if (!pCity->isHasCorporation(eCorporation))
		{
			return false;
		}

		if (kTrigger.isHeadquarters())
		{
			if (!pCity->isHeadquarters(eCorporation))
			{
				return false;
			}
		}
	}
	else
	{
		/* if (getHasCorporationCount(eCorporation) > 0)
			return true;*/
		// K-Mod. (bugfix)
		if (getHasCorporationCount(eCorporation) == 0)
			return false;
		// K-Mod end

		if (kTrigger.isHeadquarters())
		{
			CvCity* pHeadquarters = GC.getGame().getHeadquarters(eCorporation);
			if (NULL == pHeadquarters || pHeadquarters->getOwner() != getID())
			{
				return false;
			}
		}
	}

	//return false;
	return true; // K-Mod. (bugfix)
}

void CvPlayer::launch(VictoryTypes eVictory)
{
	CvTeam& kTeam = GET_TEAM(getTeam());
	if (!kTeam.canLaunch(eVictory))
		return;

	kTeam.finalizeProjectArtTypes();
	kTeam.setVictoryCountdown(eVictory, kTeam.getVictoryDelay(eVictory));

	//gDLL->getEngineIFace()->AddLaunch(getID());
	// K-Mod. The spaceship launch causes pitboss to crash
	if (GC.IsGraphicsInitialized())
		gDLL->getEngineIFace()->AddLaunch(getID());
	// K-Mod end.

	kTeam.setCanLaunch(eVictory, false);

	CvCity* pCapital = getCapitalCity();
	// <advc.106> Cut from the loop below. Use this for the replay message as well.
	CvWString szMsg(gDLL->getText("TXT_KEY_VICTORY_TEAM_HAS_LAUNCHED",
			kTeam.getName().GetCString()));
	int iPlotX = -1;
	int iPlotY = -1; // </advc.106>
	for(int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		CvPlayer const& kObs = GET_PLAYER((PlayerTypes)i);
		if (!kObs.isAlive())
			continue;

		if (pCapital != NULL && pCapital->isRevealed(kObs.getTeam()))
		{
			iPlotX = pCapital->getX();
			iPlotY = pCapital->getY();
		}
		CvWString szBuffer(szMsg); // advc.106
		if (kObs.getTeam() == getTeam())
			szBuffer = gDLL->getText("TXT_KEY_VICTORY_YOU_HAVE_LAUNCHED");

		gDLL->UI().addMessage(kObs.getID(), true, -1, szBuffer, "AS2D_CULTURELEVEL",
				MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getMiscArtPath("SPACE_SHIP_BUTTON"),
				GC.getColorType("HIGHLIGHT_TEXT"), iPlotX, iPlotY, true, true);
	}
	// <advc.106>
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT,
			getID(), szMsg, iPlotX, iPlotY, GC.getColorType("HIGHLIGHT_TEXT"));
	// </advc.106>
}

bool CvPlayer::isFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion) const
{
	for (size_t i = 0; i < m_aFreeUnitCombatPromotions.size(); i++)
	{
		if (m_aFreeUnitCombatPromotions[i].first == eUnitCombat &&
			m_aFreeUnitCombatPromotions[i].second == ePromotion)
		{
			return true;
		}
	}
	return false;
}

void CvPlayer::setFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion, bool bFree)
{
	for (UnitCombatPromotionArray::iterator it = m_aFreeUnitCombatPromotions.begin();
		it != m_aFreeUnitCombatPromotions.end(); ++it)
	{
		if (it->first == eUnitCombat && it->second == ePromotion)
		{
			if (!bFree)
				m_aFreeUnitCombatPromotions.erase(it);
			return;
		}
	}
	if (bFree)
		m_aFreeUnitCombatPromotions.push_back(std::make_pair(eUnitCombat, ePromotion));
}

bool CvPlayer::isFreePromotion(UnitClassTypes eUnitClass, PromotionTypes ePromotion) const
{
	for (size_t i = 0; i < m_aFreeUnitClassPromotions.size(); i++)
	{
		if (m_aFreeUnitClassPromotions[i].first == eUnitClass &&
			m_aFreeUnitClassPromotions[i].second == ePromotion)
		{
			return true;
		}
	}
	return false;
}

void CvPlayer::setFreePromotion(UnitClassTypes eUnitClass, PromotionTypes ePromotion, bool bFree)
{
	for (UnitClassPromotionArray::iterator it = m_aFreeUnitClassPromotions.begin();
		it != m_aFreeUnitClassPromotions.end(); ++it)
	{
		if (it->first == eUnitClass && it->second == ePromotion)
		{
			if (!bFree)
				m_aFreeUnitClassPromotions.erase(it);
			return;
		}
	}
	if (bFree)
		m_aFreeUnitClassPromotions.push_back(std::make_pair(eUnitClass, ePromotion));
}

PlayerVoteTypes CvPlayer::getVote(int iId) const
{
	for (size_t i = 0; i < m_aVote.size(); i++)
	{
		if (m_aVote[i].first == iId)
			return m_aVote[i].second;
	}
	return NO_PLAYER_VOTE;
}

void CvPlayer::setVote(int iId, PlayerVoteTypes ePlayerVote)
{
	for (std::vector<std::pair<int,PlayerVoteTypes> >::
		iterator it = m_aVote.begin(); it != m_aVote.end(); ++it)
	{
		if (it->first == iId)
		{
			if (ePlayerVote == NO_PLAYER_VOTE)
			{
				m_aVote.erase(it);
			}
			else
			{
				it->second = ePlayerVote;
			}
			return;
		}
	}
	if (ePlayerVote != NO_PLAYER_VOTE)
		m_aVote.push_back(std::make_pair(iId, ePlayerVote));
}

int CvPlayer::getUnitExtraCost(UnitClassTypes eUnitClass) const
{
	for (size_t i = 0; i < m_aUnitExtraCosts.size(); i++)
	{
		if (m_aUnitExtraCosts[i].first == eUnitClass)
			return m_aUnitExtraCosts[i].second;
	}
	return 0;
}

void CvPlayer::setUnitExtraCost(UnitClassTypes eUnitClass, int iCost)
{
	for (std::vector<std::pair<UnitClassTypes,int> >::
		iterator it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
	{
		if (it->first == eUnitClass)
		{
			if (iCost == 0)
				m_aUnitExtraCosts.erase(it);
			else it->second = iCost;
			return;
		}
	}
	if (iCost != 0)
		m_aUnitExtraCosts.push_back(std::make_pair(eUnitClass, iCost));
}

// CACHE: cache frequently used values
///////////////////////////////////////
bool CvPlayer::hasShrine(ReligionTypes eReligion)
{
	bool bHasShrine = false;

	if (eReligion != NO_RELIGION)
	{
		CvCity* pHolyCity = GC.getGame().getHolyCity(eReligion);

		// if the holy city exists, and we own it
		if (pHolyCity != NULL && pHolyCity->getOwner() == getID())
			bHasShrine = pHolyCity->hasShrine(eReligion);
	}
	return bHasShrine;
}


void CvPlayer::invalidatePopulationRankCache()
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->invalidatePopulationRankCache();
}


void CvPlayer::invalidateYieldRankCache(YieldTypes eYield)
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->invalidateYieldRankCache();
}


void CvPlayer::invalidateCommerceRankCache(CommerceTypes eCommerce)
{
	FOR_EACH_CITY_VAR(pLoopCity, *this)
		pLoopCity->invalidateCommerceRankCache();
}


/*void CvPlayer::doUpdateCacheOnTurn() // advc: Remove the whole function
{	// (advc: This function and comment were added by the BtS expansion)
	// add this back, after testing without it
	// invalidateYieldRankCache();
}*/

void CvPlayer::processVoteSource(VoteSourceTypes eVoteSource, bool bActive) // advc: Renamed from "processVoteSourceBonus"
{
	FOR_EACH_CITY_VAR(pCity, *this)
		pCity->processVoteSource(eVoteSource, bActive);
}

int CvPlayer::getVotes(VoteTypes eVote, VoteSourceTypes eVoteSource) const
{
	int iVotes = 0;

	ReligionTypes eReligion = GC.getGame().getVoteSourceReligion(eVoteSource);

	if (eVote == NO_VOTE)
	{
		if (eReligion != NO_RELIGION)
			iVotes = getReligionPopulation(eReligion);
		else iVotes = getTotalPopulation();
	}
	else
	{
		if (!GC.getInfo(eVote).isVoteSourceType(eVoteSource))
			return 0;

		if (GC.getInfo(eVote).isCivVoting())
		{
			if (eReligion == NO_RELIGION || getHasReligionCount(eReligion) > 0)
				iVotes = 1;
		}
		else if (GC.getInfo(eVote).isCityVoting())
		{
			if (eReligion != NO_RELIGION)
				iVotes = getHasReligionCount(eReligion);
			else iVotes = getNumCities();
		}
		else
		{
			if (eReligion == NO_RELIGION)
				iVotes = getTotalPopulation();
			else iVotes = getReligionPopulation(eReligion);
		}

		if (eReligion != NO_RELIGION && getStateReligion() == eReligion)
		{
			iVotes *= (100 + GC.getInfo(eVote).getStateReligionVotePercent());
			iVotes /= 100;
		}
	}

	return iVotes;
}

bool CvPlayer::canDoResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const  // advc: style changes
{
	CvTeam& kOurTeam = GET_TEAM(getTeam());
	TeamTypes eSecretaryGeneral = GC.getGame().getSecretaryGeneral(eVoteSource);
	if (kData.ePlayer != NO_PLAYER)
	{
		if (!kOurTeam.isHasMet(TEAMID(kData.ePlayer)))
			return false;
	}
	CvVoteInfo const& kVote = GC.getInfo(kData.eVote);
	if (kVote.isOpenBorders())
	{
		for (TeamIter<MAJOR_CIV> it; it.hasNext(); ++it)
		{
			CvTeam const& kVotingMember = *it;
			if (!kVotingMember.isVotingMember(eVoteSource))
				continue;
			if (!kOurTeam.isOpenBordersTrading() && !kVotingMember.isOpenBordersTrading())
				return false;
			if (kOurTeam.isAtWar(kVotingMember.getID()))
				return false;
		}
	}
	else if (kVote.isDefensivePact())
	{
		for (TeamIter<MAJOR_CIV> it; it.hasNext(); ++it)
		{
			CvTeam const& kVotingMember = *it;
			if (!kVotingMember.isVotingMember(eVoteSource))
				continue;
			if (!kOurTeam.isDefensivePactTrading() && !kVotingMember.isDefensivePactTrading())
				return false;
			if ((kOurTeam.getNumWars() > 0 || kVotingMember.getNumWars() > 0) &&
				// dlph.25: 'Sometimes defensive pact can be signed while at war'
				GC.getDefineINT(CvGlobals::BBAI_DEFENSIVE_PACT_BEHAVIOR) == 0)
			{
				return false;
			} // <dlph.25>
			if(kOurTeam.isAtWar(kVotingMember.getID()) ||
				// advc: Same additional restriction as for DP between AI teams (dlph.3)
				!kOurTeam.allWarsShared(kVotingMember.getID()))
			{
				return false;
			} // </dlph.25>
			if (!kOurTeam.canSignDefensivePact(kVotingMember.getID()))
				return false;
		}
	}
	else if (kVote.isForcePeace())
	{
		CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);
		if (kPlayer.getTeam() != getTeam() &&
			kOurTeam.isAtWar(kPlayer.getTeam()))
		{
			CvTeam const& kOurMaster = GET_TEAM(getMasterTeam()); // advc.opt
			if (kOurMaster.isVotingMember(eVoteSource) &&
				!kOurMaster.canContact(kPlayer.getTeam()))
			{
				return false;
			}
		}
	}
	else if (kVote.isForceWar())
	{
		CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);
		if (!kOurTeam.isAtWar(kPlayer.getTeam()) &&
			kOurTeam.isFullMember(eVoteSource)) // dlph.25/advc
		{
			CvTeam const& kOurMaster = GET_TEAM(getMasterTeam());
			if ((kOurMaster.isFullMember(eVoteSource) && // dlph.25/advc: was isVotingMember
				!kOurMaster.canDeclareWar(kPlayer.getTeam()) &&
				// <dlph.25/advc>
				eSecretaryGeneral == kOurMaster.getID()) ||
				!kOurMaster.canEventuallyDeclareWar(kPlayer.getTeam()))
				// </advc.25/advc>
			{
				return false;
			}
		}
	}
	else if (kVote.isForceNoTrade())
	{
		CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);
		/*  <advc.130f> Don't allow players to propose resolutions that would cancel
			deals with turnsToCancel > 0 */
		if(eSecretaryGeneral == getTeam() &&
			isAnyDealTooRecentToCancel(kPlayer.getTeam()))
		{
			return false;
		} // </advc.130f>
		if (!canStopTradingWithTeam(kPlayer.getTeam(), true))
			return false;
	}
	else if (kVote.isAssignCity())
	{
		if (GET_TEAM(kData.eOtherPlayer).isVassal(TEAMID(kData.ePlayer)))
			return false;
	} // <advc.178>
	else if(kVote.isVictory() && !GC.getGame().isDiploVictoryValid())
		return false; // </advc.178>

	return true;
}

bool CvPlayer::canDefyResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const
{
	if (GC.getGame().getSecretaryGeneral(eVoteSource) == getTeam())
		return false;

	CvVoteInfo const& kVote = GC.getInfo(kData.eVote); // advc
	// <dlph.25/advc> Kek-Mod just checks isAVassal
	if(GET_TEAM(getTeam()).isCapitulated() ||
		(isAVassal() && (kVote.isForceWar() || kVote.isForcePeace())))
	{
		return false;
	} // </dlph.25/advc>
	if (kVote.isOpenBorders())
	{
		for (TeamIter<MAJOR_CIV,NOT_SAME_TEAM_AS> it(getTeam()); it.hasNext(); ++it)
		{
			CvTeam& kVotingMember = *it;
			if (!kVotingMember.isVotingMember(eVoteSource))
				continue;
			if (!kVotingMember.isOpenBorders(getTeam()))
				return true;
		}
	}
	else if (kVote.isDefensivePact())
	{
		for (TeamIter<MAJOR_CIV,NOT_SAME_TEAM_AS> it(getTeam()); it.hasNext(); ++it)
		{
			CvTeam& kVotingMember = *it;
			if (!kVotingMember.isVotingMember(eVoteSource))
				continue;
			if (!kVotingMember.isDefensivePact(getTeam()))
				return true;
		}
	}
	else if (kVote.isForceNoTrade())
	{
		return true;
	}
	else if (kVote.isForceWar())
	{
		if (!::atWar(getTeam(), TEAMID(kData.ePlayer)) &&
			// dlph.25: 'Cannot defy war declaration against itself'
			GET_TEAM(kData.ePlayer).getMasterTeam() != getMasterTeam() &&
			isFullMember(eVoteSource)) // advc
		{
			// BETTER_BTS_AI_MOD, 12/31/08, jdog5000: Vassals can't defy declarations of war
			//if (!GET_TEAM(getTeam()).isAVassal()) // dlph.25: Vassals already handled
			return true;
		}
	}
	else if (kVote.isForcePeace())
	{
		if (TEAMID(kData.ePlayer) == getTeam())
			return true;

		if (::atWar(getTeam(), TEAMID(kData.ePlayer)))
			return true;
	}
	else if (kVote.isAssignCity())
	{
		if (kData.ePlayer == getID() ||
			// dlph.25: 'You can defy resolution giving you a city'
			kData.eOtherPlayer == getID())
		{
			return true;
		}
	}
	else if (!GC.getGame().isTeamVote(kData.eVote))
	{
		return true;
	}

	return false;
}


void CvPlayer::setDefiedResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData)
{
	FAssertMsg(canDefyResolution(eVoteSource, kData),
			"OK to fail when a team member defies a resolution"); // dlph.25
	// cities get unhappiness
	FOR_EACH_CITY_VAR(pLoopCity, *this)
	{
		ReligionTypes eReligion = GC.getGame().getVoteSourceReligion(eVoteSource);
		if (NO_RELIGION == eReligion || pLoopCity->isHasReligion(eReligion))
		{
			int iAngerLength = pLoopCity->flatDefyResolutionAngerLength();
			if (NO_RELIGION != eReligion && pLoopCity->isHasReligion(eReligion))
				iAngerLength /= std::max(1, pLoopCity->getReligionCount());
			pLoopCity->changeDefyResolutionAngerTimer(iAngerLength);
		}
	}
	setLoyalMember(eVoteSource, false);
}


void CvPlayer::setEndorsedResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData)
{
	setLoyalMember(eVoteSource, true);
}

bool CvPlayer::isFullMember(VoteSourceTypes eVoteSource) const
{
	if (GC.getGame().getVoteSourceReligion(eVoteSource) != NO_RELIGION)
	{
		if (getStateReligion() != GC.getGame().getVoteSourceReligion(eVoteSource))
			return false;
	}

	if (GC.getInfo(eVoteSource).getCivic() != NO_CIVIC)
	{
		if (!isCivic((CivicTypes)GC.getInfo(eVoteSource).getCivic()))
			return false;
	}

	if (!isLoyalMember(eVoteSource))
		return false;

	return isVotingMember(eVoteSource);
}

bool CvPlayer::isVotingMember(VoteSourceTypes eVoteSource) const
{
	return (getVotes(NO_VOTE, eVoteSource) > 0);
}

PlayerTypes CvPlayer::pickConqueredCityOwner(const CvCity& kCity) const
{
	PlayerTypes eBestPlayer = kCity.getLiberationPlayer(true);
	if (eBestPlayer!= NO_PLAYER)
	{
		if (GET_TEAM(getTeam()).isVassal(TEAMID(eBestPlayer)))
			return eBestPlayer;
	}
	return getID();
}

bool CvPlayer::canHaveTradeRoutesWith(PlayerTypes ePlayer) const
{
	CvPlayer& kOtherPlayer = GET_PLAYER(ePlayer);
	if (!kOtherPlayer.isAlive())
		return false;
	if (getTeam() == kOtherPlayer.getTeam())
		return true;
	// <advc.124>
	if(kOtherPlayer.isAnarchy())
		return false; // </advc.124>

	if (!GET_TEAM(getTeam()).isFreeTrade(kOtherPlayer.getTeam()))
		return false;

	if (GET_TEAM(getTeam()).isVassal(kOtherPlayer.getTeam()))
		return true;

	if (GET_TEAM(kOtherPlayer.getTeam()).isVassal(getTeam()))
		return true;

	if (!isNoForeignTrade() && !kOtherPlayer.isNoForeignTrade())
		return true;

	return false;
}

bool CvPlayer::canStealTech(PlayerTypes eTarget, TechTypes eTech) const
{
	if (GET_TEAM(eTarget).isHasTech(eTech))
	{
		if (canResearch(eTech, /* K-Mod: */ false, true))
			return true;
	}
	return false;
}

bool CvPlayer::canForceCivics(PlayerTypes eTarget, CivicTypes eCivic) const
{	// return (GET_PLAYER(eTarget).canDoCivics(eCivic) && !GET_PLAYER(eTarget).isCivic(eCivic) && isCivic(eCivic));
	// <advc.132>
	if(!GET_PLAYER(eTarget).canDoCivics(eCivic) ||
			GET_PLAYER(eTarget).isCivic(eCivic) || !isCivic(eCivic))
		return false;
	/* Identify economy civics as those in the same column as a civic
	   granting extra trade routes (Free Market).
	   Religion civics are those that allow or disallow a state religion.
	   Marking "flexible" civics in XML (schema change) would be cleaner. */
	int iEconomyOption = -1;
	for(int i = 0; i < GC.getNumCivicInfos(); i++)
	{
		CvCivicInfo const& kLoopCivic = GC.getInfo((CivicTypes)i);
		if(kLoopCivic.getTradeRoutes() != 0)
		{
			iEconomyOption = kLoopCivic.getCivicOptionType();
			break;
		}
	}
	CvCivicInfo const& kCivic = GC.getInfo(eCivic);
	return (getCivilization().getInitialCivic((CivicOptionTypes)kCivic.
			getCivicOptionType()) != eCivic && // Not an initial civic
			(kCivic.isStateReligion() || kCivic.getNonStateReligionHappiness() > 0 ||
			kCivic.getCivicOptionType() == iEconomyOption));
}

bool CvPlayer::canForceReligion(PlayerTypes eTarget, ReligionTypes eReligion) const
{
	//return (GET_PLAYER(eTarget).canDoReligion(eReligion) && GET_PLAYER(eTarget).getStateReligion() != eReligion && getStateReligion() == eReligion);
	// K-Mod - You shouldn't be able to force a religion on an irreligious civ.
	//return (GET_PLAYER(eTarget).isStateReligion() && GET_PLAYER(eTarget).canDoReligion(eReligion) && GET_PLAYER(eTarget).getStateReligion() != eReligion && getStateReligion() == eReligion);
	// <advc.132> Rewritten based on the K-Mod condition (commented out above)
	CvPlayer const& kTarget = GET_PLAYER(eTarget);
	/*  Just the conditions from above. (Better use canConvert, which checks recent
		religion change? Would also have to use canRevolution then in canForceCivics.) */
	if(!kTarget.isStateReligion() || !kTarget.canDoReligion(eReligion) ||
			kTarget.getStateReligion() == eReligion)
		return false;
	if(getStateReligion() == eReligion)
		return true;
	// New: Accept any major religion
	return kTarget.isMajorReligion(eReligion); // </advc.132>
}

bool CvPlayer::canSpyDestroyUnit(PlayerTypes eTarget, CvUnit const& kUnit) const
{
	if (kUnit.getTeam() == getTeam())
		return false;

	if (kUnit.getUnitInfo().getProductionCost() <= 0)
		return false;

	if (!kUnit.getPlot().isVisible(getTeam()))
		return false;

	return true;
}

bool CvPlayer::canSpyBribeUnit(PlayerTypes eTarget, CvUnit const& kUnit) const
{
	if (!canSpyDestroyUnit(eTarget, kUnit))
		return false;

	// Can't buy units when at war
	if (kUnit.isEnemy(getTeam()))
		return false;

	// Can't buy units if they are not in a legal plot
	if (!GET_TEAM(getTeam()).canPeacefullyEnter(TEAMID(eTarget)))
		return false;

	for (CLLNode<IDInfo> const* pUnitNode = kUnit.getPlot().headUnitNode(); pUnitNode != NULL;
		pUnitNode = kUnit.getPlot().nextUnitNode(pUnitNode))
	{
		CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
		if (pLoopUnit != NULL && pLoopUnit != &kUnit)
		{
			if (pLoopUnit->isEnemy(getTeam()))
			{
				// If we buy the unit, we will be on the same plot as an enemy unit! Not good.
				return false;
			}
		}
	}

	return true;
}

bool CvPlayer::canSpyDestroyBuilding(PlayerTypes eTarget, BuildingTypes eBuilding) const
{
	if (GC.getInfo(eBuilding).getProductionCost() <= 0)
		return false;

	if (GC.getInfo(eBuilding).isLimited())
		return false;

	return true;
}

bool CvPlayer::canSpyDestroyProject(PlayerTypes eTarget, ProjectTypes eProject) const
{
	CvProjectInfo& kProject = GC.getInfo(eProject);
	if (kProject.getProductionCost() <= 0)
		return false;

	if (GET_TEAM(eTarget).getProjectCount(eProject) <= 0)
		return false;

	if (kProject.isWorldProject())
		return false;

	if (!kProject.isSpaceship())
		return false;
	else
	{
		VictoryTypes eVictory = (VictoryTypes)kProject.getVictoryPrereq();
		if (eVictory != NO_VICTORY)
		{
			// Can't destroy spaceship components if we have already launched
			if (GET_TEAM(eTarget).getVictoryCountdown(eVictory) >= 0)
				return false;
		}
	}
	return true;
}

// K-Mod
int CvPlayer::getEspionageGoldQuantity(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvCity* pCity) const
{
	if (!pCity || pCity->getOwner() != eTargetPlayer)
		return 0;

	CvEspionageMissionInfo& kMission = GC.getInfo(eMission);
	int iGoldStolen = (GET_PLAYER(eTargetPlayer).getGold() * kMission.getStealTreasuryTypes()) / 100;

	iGoldStolen *= 3*pCity->getPopulation();
	iGoldStolen /= std::max(1, pCity->getPopulation() + 2*GET_PLAYER(eTargetPlayer).getAveragePopulation());

	return std::min(iGoldStolen, GET_PLAYER(eTargetPlayer).getGold());
}

void CvPlayer::forcePeace(PlayerTypes ePlayer)
{
	/*if (!GET_TEAM(getTeam()).isAVassal()) {
		FAssert(GET_TEAM(getTeam()).canChangeWarPeace(GET_PLAYER(ePlayer).getTeam()));*/ // BtS
	// K-Mod: "canChangeWarPeace" can return false here if the peace team vassalates after the vote is cast.
	//if (GET_TEAM(getTeam()).canChangeWarPeace(GET_PLAYER(ePlayer).getTeam()))
	// ...
	/*	advc: Redundant code deleted; CvTeam::signPeaceTreaty does the same thing
		including, due to advc.130v, the canChangeWarPeace check.
		To avoid making forcePeace obsolete, I'm changing its semantics so that
		canChangeWarPeace is NOT checked. */
	GET_TEAM(getTeam()).signPeaceTreaty(TEAMID(ePlayer), true);
}

// advc.032:
bool CvPlayer::resetPeaceTreaty(PlayerTypes ePlayer)
{
	int iGameTurn = GC.getGame().getGameTurn();
	FOR_EACH_DEAL_VAR(d)
	{
		if(d->isBetween(getID(), ePlayer) && d->getFirstTrades() != NULL)
		{
			for(CLLNode<TradeData> const* pNode = d->getFirstTrades()->head();
				pNode != NULL; pNode = d->getFirstTrades()->next(pNode))
			{
				if(pNode->m_data.m_eItemType == TRADE_PEACE_TREATY)
				{
					d->setInitialGameTurn(iGameTurn);
					return true; // Assume that there is at most 1 peace treaty
				}
			}
		}
	}
	return false;
}

bool CvPlayer::canSpiesEnterBorders(PlayerTypes ePlayer) const
{
	FOR_EACH_ENUM(EspionageMission)
	{
		if (GC.getInfo(eLoopEspionageMission).isNoActiveMissions() &&
			GC.getInfo(eLoopEspionageMission).isPassive())
		{
			if (GET_PLAYER(ePlayer).canDoEspionageMission(
					eLoopEspionageMission, getID(), NULL, -1, NULL))
				return false;
		}
	}
	return true;
}

int CvPlayer::getReligionPopulation(ReligionTypes eReligion) const
{
	int iPopulation = 0;
	FOR_EACH_CITY(pCity, *this)
	{
		if (pCity->isHasReligion(eReligion))
			iPopulation += pCity->getPopulation();
	}
	return iPopulation;
}

int CvPlayer::getNewCityProductionValue() const
{
	int iValue = 0;
	CvCivilization const& kCiv = getCivilization(); // advc.003w
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes eBuilding = kCiv.buildingAt(i);
		if(GC.getGame().isFreeStartEraBuilding(eBuilding)) // advc
		{
			iValue += (100 * getProductionNeeded(eBuilding)) /
					std::max(1, 100 + getProductionModifier(eBuilding));
		}
	}

	iValue *= 100 + GC.getDefineINT("NEW_CITY_BUILDING_VALUE_MODIFIER");
	iValue /= 100;

	iValue += (GC.getDefineINT("ADVANCED_START_CITY_COST") * GC.getInfo(GC.getGame().getGameSpeedType()).getGrowthPercent()) / 100;

	int iPopulation = GC.getDefineINT("INITIAL_CITY_POPULATION") + GC.getInfo(GC.getGame().getStartEra()).getFreePopulation();
	for (int i = 1; i <= iPopulation; ++i)
	{
		iValue += (getGrowthThreshold(i) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;
	}
	// <advc.251>
	iValue = ::roundToMultiple(iValue *
			// Apply production modifier half (b/c getGrowthThreshold is also dependent on handicap)
			(1 + 0.5 * (trainingModifierFromHandicap(false) - 1)), isHuman() ? 5 : 1);
	// </advc.251>
	return iValue;
}

int CvPlayer::getGrowthThreshold(int iPopulation) const
{
	CvGame const& g = GC.getGame(); // advc
	// <advc.251>
	static int const iBASE_CITY_GROWTH_THRESHOLD = GC.getDefineINT("BASE_CITY_GROWTH_THRESHOLD");
	static int const iCITY_GROWTH_MULTIPLIER = GC.getDefineINT("CITY_GROWTH_MULTIPLIER");
	int iBaseThreshold = ::round(0.01 * iBASE_CITY_GROWTH_THRESHOLD * GC.getInfo(
			getHandicapType()).getBaseGrowthThresholdPercent());
	int iThreshold = iBaseThreshold + // </advc.251>
			(iPopulation * iCITY_GROWTH_MULTIPLIER);
	// <advc.251>
	int iAIModifier = 100;
	if(!isHuman()) // Also apply it to Barbarians
	{
		CvHandicapInfo const& h = GC.getInfo(g.getHandicapType());
		iAIModifier = h.getAIGrowthPercent() +
				//h.getAIPerEraModifier() * getCurrentEra()
				g.AIHandicapAdjustment();
	} // Reduce rounding error:
	iThreshold = ::round(iThreshold * 0.01 * iAIModifier *
			0.01 * GC.getInfo(g.getGameSpeedType()).getGrowthPercent() *
			0.01 * GC.getInfo(g.getStartEra()).getGrowthPercent());
	// </advc.251>
	return std::max(1, iThreshold);
}

void CvPlayer::verifyUnitStacksValid()
{
	FOR_EACH_UNIT_VAR(pLoopUnit, *this)
		pLoopUnit->verifyStackValid();
}

UnitTypes CvPlayer::getTechFreeUnit(TechTypes eTech) const  // advc: style changes
{
	UnitClassTypes eUnitClass = (UnitClassTypes)GC.getInfo(eTech).getFirstFreeUnitClass();
	if (eUnitClass == NO_UNITCLASS)
		return NO_UNIT;

	UnitTypes eUnit = getCivilization().getUnit(eUnitClass);
	if (eUnit == NO_UNIT)
		return NO_UNIT;

	if (GC.getInfo(eUnit).getEspionagePoints() > 0 &&
			GC.getGame().isOption(GAMEOPTION_NO_ESPIONAGE))
		return NO_UNIT;

	return eUnit;
}

// BULL - Trade Hover - start  // advc: style changes, _MOD_FRACTRADE removed
/*  Adds the yield and count for each trade route with eWithPlayer to the
	int references (out parameters). */
void CvPlayer::calculateTradeTotals(YieldTypes eIndex,
	int& iDomesticYield, int& iDomesticRoutes, int& iForeignYield, int& iForeignRoutes,
	PlayerTypes eWithPlayer) const
{
	FOR_EACH_CITY(pCity, *this)
	{
		pCity->calculateTradeTotals(eIndex, iDomesticYield, iDomesticRoutes,
				iForeignYield, iForeignRoutes, eWithPlayer);
	}
} // BULL - Trade Hover - end

// <advc.085>
void CvPlayer::setScoreboardExpanded(bool b)
{
	CvGame& kGame = GC.getGame();
	/*	When a popup (e.g. diplo) is open, my code for detecting whether
		the cursor has been moved away from the scoreboard causes the
		hover text box to flicker. Don't think I can fix that.
		Workaround: If the player expands the scoreboard during
		diplomacy, it remains expanded until diplomacy ends. */
	if (gDLL->UI().isFocused())
	{
		if (!BUGOption::isEnabled("Scores__ExpandOnHover", false, false))
			return;
		// Expand the scoreboard
		if (b && !m_bScoreboardExpanded)
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
			m_bScoreboardExpanded = b;
		}
		/*	Needed in conjunction with an update timer set by
			CvDLLWidgetData::doContactCiv to prevent the scoreboard
			from already getting stuck at expanded when the player
			clicks on the scoreboard to initiate diplomacy. */
		if (gDLL->isDiplomacy())
			m_bScoreboardExpanded = b;

		// Schedule callback for collapse
		kGame.setUpdateTimer(CvGame::UPDATE_COLLAPSE_SCORE_BOARD, 1);
		// Ignore callback while diplomacy ongoing
		return;
	}
	if (b)
	{
		FAssert(BUGOption::isEnabled("Scores__AlignIcons", true, false));
		if (BUGOption::isEnabled("Scores__ExpandOnHover", false, false))
		{
			/*  A delay of 1 means that the scoreboard collapses after
				two game updates (250 ms) */
			int const iDelay = 1;
			/*  So long as the mouse hovers over a scoreboard widget,
				setScoreboardExpanded(true) keeps getting called and
				the collapse timer keeps getting reset. */
			kGame.setUpdateTimer(CvGame::UPDATE_COLLAPSE_SCORE_BOARD, iDelay);
			// Scoreboard needs to be redrawn when expanding
			if (!isScoreboardExpanded())
				gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
			/*  The EXE calls CvDLLWidgetData::parseHelp when the cursor is moved
				onto a widget - but not while the cursor rests there. Workaround:
				Redraw the scoreboard and its widgets. That also causes a parseHelp
				call (if the cursor is on a widget). Must still allow game updates
				in between though (otherwise, e.g. animations will start lagging).
				Therefore, don't set the dirty bit until the next game update. */
			else kGame.setUpdateTimer(CvGame::UPDATE_DIRTY_SCORE_BOARD, iDelay - 1);
			m_bScoreboardExpanded = true;
		}
		else m_bScoreboardExpanded = false;
		return;
	}
	// Collapse callback from CvGame
	if (isScoreboardExpanded())
	{
		m_bScoreboardExpanded = false;
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		/*  For some strange reason, the HUD retains mouse focus after expanding
			the scoreboard, and this is the only remedy I was able to find
			(apart from CvInterface::makeInterfaceDirty, which is far costlier). */
		gDLL->getInterfaceIFace()->makeSelectionListDirty();
	}
}


bool CvPlayer::isScoreboardExpanded() const
{
	return m_bScoreboardExpanded;
} // </advc.085>


void CvPlayer::buildTradeTable(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourList) const
{
	PROFILE_FUNC(); // advc.opt (not frequently called)
	TradeData item;
	bool const bOtherHuman = GET_PLAYER(eOtherPlayer).isHuman(); // advc.opt

	setTradeItem(&item, TRADE_GOLD);
	if (canTradeItem(eOtherPlayer, item))
		ourList.insertAtEnd(item);

	setTradeItem(&item, TRADE_GOLD_PER_TURN);
	if (canTradeItem(eOtherPlayer, item))
		ourList.insertAtEnd(item);

	setTradeItem(&item, TRADE_MAPS, 0);
	if (canTradeItem(eOtherPlayer, item))
		ourList.insertAtEnd(item);

	setTradeItem(&item, TRADE_VASSAL, 0);
	if (canTradeItem(eOtherPlayer, item))
		ourList.insertAtEnd(item);

	setTradeItem(&item, TRADE_OPEN_BORDERS);
	if (canTradeItem(eOtherPlayer, item))
		ourList.insertAtEnd(item);

	setTradeItem(&item, TRADE_DEFENSIVE_PACT);
	if (canTradeItem(eOtherPlayer, item))
		ourList.insertAtEnd(item);

	setTradeItem(&item, TRADE_PERMANENT_ALLIANCE);
	if (canTradeItem(eOtherPlayer, item))
		ourList.insertAtEnd(item);

	if (GET_TEAM(getTeam()).isAtWar(TEAMID(eOtherPlayer)))
	{
		setTradeItem(&item, TRADE_PEACE_TREATY);
		ourList.insertAtEnd(item);

		setTradeItem(&item, TRADE_SURRENDER, 0);
		if (canTradeItem(eOtherPlayer, item))
			ourList.insertAtEnd(item);
	}
	/*  <advc.104m> Make peace treaties hidden items at peacetime
		so that the trade screen (EXE) doesn't discard them from
		tribute and help requests. */
	else
	{
		setTradeItem(&item, TRADE_PEACE_TREATY);
		item.m_bHidden = true;
		ourList.insertAtEnd(item);
	} // </advc.104m>

	//	Initial build of the inventory lists and buttons.
	//	Go through all the possible headings
	for (int i = NUM_BASIC_ITEMS; i < NUM_TRADEABLE_HEADINGS; i++)
	{	// <advc.opt>
		TradeableItems eItemType = (TradeableItems)i;
		if (!canPossiblyTradeItem(eOtherPlayer, eItemType))
			continue; // </advc.opt>
		bool bFoundItemUs = false;
		//	Build what we need to build for this item
		switch (eItemType)
		{
		case TRADE_TECHNOLOGIES:
			for (int j = 0; j < GC.getNumTechInfos(); j++)
			{
				setTradeItem(&item, TRADE_TECHNOLOGIES, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_RESOURCES:
			for (int j = 0; j < GC.getNumBonusInfos(); j++)
			{
				setTradeItem(&item, TRADE_RESOURCES, j);
				if (!canTradeItem(eOtherPlayer, item))
					continue;
				// <advc.074>
				bool bHuman = (bOtherHuman || isHuman());
				bool bValid = (!bHuman || getTradeDenial(eOtherPlayer, item) != DENIAL_JOKING);
				/*  Hack: Check if we're expecting a renegotiate-popup from the EXE.
					Don't want any resources in the canceled deal to be excluded
					from the trade table. */
				if(!bValid && !GC.getGame().isInBetweenTurns() &&
					// Probably too complicated to get this right with simultaneous turns
					!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
				{
					for(CLLNode<std::pair<PlayerTypes,BonusTypes> >* pNode =
						m_cancelingExport.head(); pNode != NULL; pNode =
						m_cancelingExport.next(pNode))
					{
						PlayerTypes eCancelPlayer = pNode->m_data.first;
						BonusTypes eCancelBonus = pNode->m_data.second;
						FAssert(eCancelPlayer != getID());
						if(eCancelPlayer == eOtherPlayer && eCancelBonus == j)
						{
							bValid = true;
							m_cancelingExport.deleteNode(pNode);
							break;
						}
					}
				}
				if(bValid) // </advc.074>
				{
					bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_CITIES:
		{
			FOR_EACH_CITY(pLoopCity, *this)
			{
				//if (AI_cityTrade(pLoopCity, eOtherPlayer) != DENIAL_NEVER) // K-Mod
				setTradeItem(&item, TRADE_CITIES, pLoopCity->getID());
				if (canTradeItem(eOtherPlayer, item))
				{
					/*  advc.ctr: If "never" cities are excluded, then "too much"
						should be excluded as well and the check should be done
						after the faster canTradeItem check. But I don't want to
						exclude either anymore. */
					/*bool bValid = !bOtherHuman;
					if (!bValid) 
					{
						DenialTypes eDenial = getTradeDenial(eOtherPlayer, item);
						if (eDenial != DENIAL_NEVER && eDenial != DENIAL_TOO_MUCH)
							bValid = true;
					}
					if (bValid)*/
					{
						bFoundItemUs = true;
						ourList.insertAtEnd(item);
					}
				}
			}
			break;
		}
		case TRADE_PEACE:
			if (!isHuman())
			{
				for (int j = 0; j < MAX_CIV_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_PEACE, j);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;

		case TRADE_WAR:
			if (!isHuman())
			{
				for (int j = 0; j < MAX_CIV_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_WAR, j);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;

		case TRADE_EMBARGO:
			if (!isHuman())
			{
				for (int j = 0; j < MAX_CIV_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_EMBARGO, j);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;

		case TRADE_CIVIC:
			for (int j = 0; j < GC.getNumCivicInfos(); j++)
			{
				setTradeItem(&item, TRADE_CIVIC, j);
				if (canTradeItem(eOtherPlayer, item))
				{	// <advc.074>
					if((!bOtherHuman && !isHuman()) ||
						getTradeDenial(eOtherPlayer, item) != DENIAL_JOKING) {
					// </advc.074>
						bFoundItemUs = true;
						ourList.insertAtEnd(item);
					}
				}
			}
			break;

		case TRADE_RELIGION:
			for (int j = 0; j < GC.getNumReligionInfos(); j++)
			{
				setTradeItem(&item, TRADE_RELIGION, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;
		}
	}
}

bool CvPlayer::getHeadingTradeString(PlayerTypes eOtherPlayer, TradeableItems eItem, CvWString& szString, CvString& szIcon) const
{
	szIcon.clear();

	switch (eItem)
	{
	case TRADE_TECHNOLOGIES:
		szString = gDLL->getText("TXT_KEY_CONCEPT_TECHNOLOGY");
		break;

	case TRADE_RESOURCES:
		szString = gDLL->getText("TXT_KEY_TRADE_RESOURCES");
		break;

	case TRADE_CITIES:
		szString = gDLL->getText("TXT_KEY_TRADE_CITIES");
		break;

	case TRADE_PEACE:
		szString = gDLL->getText("TXT_KEY_TRADE_MAKE_PEACE_WITH");
		break;

	case TRADE_WAR:
		szString = gDLL->getText("TXT_KEY_TRADE_DECLARE_WAR_ON");
		break;

	case TRADE_EMBARGO:
		szString = gDLL->getText("TXT_KEY_TRADE_STOP_TRADING_WITH");
		break;

	case TRADE_CIVIC:
		szString = gDLL->getText("TXT_KEY_TRADE_ADOPT");
		break;

	case TRADE_RELIGION:
		szString = gDLL->getText("TXT_KEY_TRADE_CONVERT");
		break;
	default:
		szString.clear();
		return false;
		break;
	}

	return true;
}


bool CvPlayer::getItemTradeString(PlayerTypes eOtherPlayer, bool bOffer,
	bool bShowingCurrent, const TradeData& zTradeData,
	CvWString& szString, CvString& szIcon) const
{
	szIcon.clear();
	// <advc.072>
	CvDeal* pDeal = NULL;
	if(bShowingCurrent)
	{
		int iTurnsLeftMode = BUGOption::getValue("Advisors__DealTurnsLeft", 3);
		if(iTurnsLeftMode == 2 || iTurnsLeftMode == 1)
		{
			TradeableItems eItemType = zTradeData.m_eItemType;
			if(CvDeal::isAnnual(eItemType) || eItemType == TRADE_PEACE_TREATY)
			{
				pDeal = GC.getGame().nextCurrentDeal(eOtherPlayer, getID(),
						eItemType, zTradeData.m_iData);
				/*  Call nextCurrentDeal for all annual deals plus peace treaties
					in order to stay in-sync with the iteration done by the EXE,
					but we're only interested in a subset here: */
				if(!CvDeal::isDual(eItemType) && eItemType != TRADE_RESOURCES &&
					eItemType != TRADE_GOLD_PER_TURN)
				{
					pDeal = NULL;
				}
			}
		}
	} // </advc.072>
	switch (zTradeData.m_eItemType)
	{
	case TRADE_GOLD:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_NUM", zTradeData.m_iData);
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_NUM", AI().AI_maxGoldTrade(eOtherPlayer));
		}
		break;
	case TRADE_GOLD_PER_TURN:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_PER_TURN_NUM", zTradeData.m_iData);
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_PER_TURN_NUM", AI().AI_maxGoldPerTurnTrade(eOtherPlayer));
		}
		break;
	case TRADE_MAPS:
		szString = gDLL->getText("TXT_KEY_TRADE_WORLD_MAP_STRING");
		break;
	case TRADE_VASSAL:
		szString = gDLL->getText("TXT_KEY_TRADE_VASSAL_STRING");
		break;
	case TRADE_SURRENDER:
		szString = gDLL->getText("TXT_KEY_TRADE_CAPITULATE_STRING");
		break;
	case TRADE_OPEN_BORDERS:
		szString = gDLL->getText("TXT_KEY_TRADE_OPEN_BORDERS_STRING");
		break;
	case TRADE_DEFENSIVE_PACT:
		szString = gDLL->getText("TXT_KEY_TRADE_DEFENSIVE_PACT_STRING");
		break;
	case TRADE_PERMANENT_ALLIANCE:
		szString = gDLL->getText("TXT_KEY_TRADE_PERMANENT_ALLIANCE_STRING");
		break;
	case TRADE_PEACE_TREATY:
		if(GET_TEAM(eOtherPlayer).isAtWar(getTeam())) // advc.072
		{
			szString = gDLL->getText("TXT_KEY_TRADE_PEACE_TREATY_STRING",
					GC.getDefineINT(CvGlobals::PEACE_TREATY_LENGTH));
		}
		else szString = gDLL->getText("TXT_KEY_TRADE_PEACE_TREATY_STR"); // advc.072
		break;
	case TRADE_TECHNOLOGIES:
		szString = GC.getInfo((TechTypes)zTradeData.m_iData).getDescription();
		szIcon = GC.getInfo((TechTypes)zTradeData.m_iData).getButton();
		break;
	case TRADE_RESOURCES:
		if (bOffer)
		{
			int iNumResources = GET_PLAYER(eOtherPlayer).getNumTradeableBonuses((BonusTypes)zTradeData.m_iData);
			if (bShowingCurrent)
			{
				++iNumResources;
			}
			szString = gDLL->getText("TXT_KEY_TRADE_RESOURCE", GC.getInfo((BonusTypes)zTradeData.m_iData).getDescription(), iNumResources);

		}
		else
		{
			szString.Format( L"%s (%d)", GC.getInfo((BonusTypes)zTradeData.m_iData).getDescription(), getNumTradeableBonuses((BonusTypes)zTradeData.m_iData));
		}
		szIcon = GC.getInfo((BonusTypes)zTradeData.m_iData).getButton();
		break;
	case TRADE_CITIES:
	{
		CvCity* pCity = NULL;
		if (bOffer)
			pCity = GET_PLAYER(eOtherPlayer).getCity(zTradeData.m_iData);
		else pCity = getCity(zTradeData.m_iData);
		if (pCity != NULL)
		{
			if (pCity->getLiberationPlayer() == //eOtherPlayer)
				(bOffer ? getID() : eOtherPlayer)) // advc.001 (bugfix?), advc.ctr
			{
				szString.Format(L"%s (%s)", pCity->getName().GetCString(),
						gDLL->getText("TXT_KEY_LIBERATE_CITY").GetCString());
			}
			else szString = gDLL->getText("TXT_KEY_CITY_OF", pCity->getNameKey());
		}
		break;
	}
	case TRADE_PEACE:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_PEACE_WITH");
			szString += GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		break;
	case TRADE_WAR:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_WAR_WITH");
			szString += GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		break;
	case TRADE_EMBARGO:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_STOP_TRADING_WITH");
			szString += L" " + GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		break;
	case TRADE_CIVIC:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_ADOPT");
			szString += GC.getInfo((CivicTypes)zTradeData.m_iData).getDescription();
		}
		else
		{
			szString = GC.getInfo((CivicTypes)zTradeData.m_iData).getDescription();
		}
		szIcon = GC.getInfo((CivicTypes)zTradeData.m_iData).getButton();
		break;
	case TRADE_RELIGION:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_CONVERT");
			szString += GC.getInfo((ReligionTypes)zTradeData.m_iData).getDescription();
		}
		else
		{
			szString = GC.getInfo((ReligionTypes)zTradeData.m_iData).getDescription();
		}
		szIcon = GC.getInfo((ReligionTypes)zTradeData.m_iData).getButton();
		break; // <advc.034>
	case TRADE_DISENGAGE:
		szString.clear();
		GAMETEXT.buildDisengageString(szString, getID(), eOtherPlayer);
		break; // </advc.034>
	default:
		szString.clear();
		return false;
	}
	// <advc.072>
	if(pDeal != NULL && pDeal->turnsToCancel() > 0)
	{
		szString.append(L" ");
		szString.append(gDLL->getText("INTERFACE_CITY_TURNS",
				pDeal->turnsToCancel()).GetCString());
	} // </advc.072>
	return true;
}

void CvPlayer::updateTradeList(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourInventory,
	CLinkList<TradeData> const& ourOffer, CLinkList<TradeData> const& theirOffer) const
{
	for (CLLNode<TradeData>* pNode = ourInventory.head(); pNode != NULL;
		pNode = ourInventory.next(pNode))
	{
		pNode->m_data.m_bHidden = false;

		// Don't show peace treaties when not at war
		if (!::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam()))
		{
			if (pNode->m_data.m_eItemType == TRADE_PEACE_TREATY ||
				pNode->m_data.m_eItemType == TRADE_SURRENDER)
			{
				pNode->m_data.m_bHidden = true;
			}
		}
		// Don't show technologies with no tech trading game option
		if (GC.getGame().isOption(GAMEOPTION_NO_TECH_TRADING) &&
			pNode->m_data.m_eItemType == TRADE_TECHNOLOGIES)
		{
			pNode->m_data.m_bHidden = true;
		}
	}

	for (CLLNode<TradeData>* pNode = ourInventory.head(); pNode != NULL; pNode = ourInventory.next(pNode))
	{
		switch (pNode->m_data.m_eItemType)
		{
		case TRADE_PEACE_TREATY:
			for (CLLNode<TradeData>* pOfferNode = ourOffer.head(); pOfferNode != NULL; pOfferNode = ourOffer.next(pOfferNode))
			{
				// Don't show vassal deals if peace treaty is already on the table
				if (CvDeal::isVassal(pOfferNode->m_data.m_eItemType))
				{
					pNode->m_data.m_bHidden = true;
					break;
				}
			}
			break;
		case TRADE_VASSAL:
		case TRADE_SURRENDER:
			for (CLLNode<TradeData>* pOfferNode = theirOffer.head(); pOfferNode != NULL; pOfferNode = theirOffer.next(pOfferNode))
			{
				// Don't show vassal deals if another type of vassal deal is on the table
				if (CvDeal::isVassal(pOfferNode->m_data.m_eItemType))
				{
					pNode->m_data.m_bHidden = true;
					break;
				}
			}

			if (!pNode->m_data.m_bHidden)
			{
				for (CLLNode<TradeData>* pOfferNode = ourOffer.head(); pOfferNode != NULL; pOfferNode = ourOffer.next(pOfferNode))
				{
					// Don't show peace deals if the other player is offering to be a vassal
					if (CvDeal::isEndWar(pOfferNode->m_data.m_eItemType))
					{
						pNode->m_data.m_bHidden = true;
						break;
					}
				}
			}
			break;
		/*// <advc.004> Only one side can pay gold [better keep gold on display I guess]
		case TRADE_GOLD:
		case TRADE_GOLD_PER_TURN:
			// Perhaps better to keep AI gold on display (human treasury is always visible anyway)
			if (isHuman())
			{
				for (CLLNode<TradeData>* pOfferNode = theirOffer.head(); pOfferNode != NULL;
					pOfferNode = theirOffer.next(pOfferNode))
				{
					TradeableItems eItemType = pOfferNode->m_data.m_eItemType;
					if (eItemType == TRADE_GOLD || eItemType == TRADE_GOLD_PER_TURN)
						pNode->m_data.m_bHidden = true;
				}
				break;
			} // </advc.004>*/
		}
	}

	if (!isHuman() || !GET_PLAYER(eOtherPlayer).isHuman())  // everything allowed in human-human trades
	{
		CLLNode<TradeData>* pFirstOffer = ourOffer.head();
		if (pFirstOffer == NULL)
			pFirstOffer = theirOffer.head();
		if (pFirstOffer != NULL)
		{
			//if (!CvDeal::isEndWar(pFirstOffer->m_data.m_eItemType) || !::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam()))
			if (!::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam())) // K-Mod
			{
				for (CLLNode<TradeData>* pNode = ourInventory.head(); pNode != NULL; pNode = ourInventory.next(pNode))
				{	// advc.ctr: Allow cities to be bought
					/*if (pFirstOffer->m_data.m_eItemType == TRADE_CITIES || pNode->m_data.m_eItemType == TRADE_CITIES)
						pNode->m_data.m_bHidden = true;
					else*/
					if (CvDeal::isAnnual(pFirstOffer->m_data.m_eItemType) != CvDeal::isAnnual(pNode->m_data.m_eItemType))
						pNode->m_data.m_bHidden = true;
				}
			}
		}
	}
}

// K-Mod. Find each item from the offer list in the inventory list - mark it as m_bOffering == true.
// (This is usually done somewhere in the game engine, or when the offer list is being generated or something.)
void CvPlayer::markTradeOffers(CLinkList<TradeData>& ourInventory, const CLinkList<TradeData>& ourOffer) const
{
	for (CLLNode<TradeData>* pOfferNode = ourOffer.head(); pOfferNode != NULL; pOfferNode = ourOffer.next(pOfferNode))
	{
		CLLNode<TradeData>* pInvNode; // (defined here just for the assertion at the end)
		for (pInvNode = ourInventory.head(); pInvNode != NULL; pInvNode = ourInventory.next(pInvNode))
		{
			if (pInvNode->m_data.m_eItemType == pOfferNode->m_data.m_eItemType &&
				pInvNode->m_data.m_iData == pOfferNode->m_data.m_iData)
			{
				pInvNode->m_data.m_bOffering = pOfferNode->m_data.m_bOffering = true;
				break;
			}
		}
		FAssertMsg(pInvNode != NULL ||
				// advc.134a: I guess it's OK that capitulation isn't part of the inventory
				pOfferNode->m_data.m_eItemType == TRADE_SURRENDER,
				"failed to find offered item in inventory");
	}
}
// K-Mod end

int CvPlayer::getIntroMusicScriptId(PlayerTypes eForPlayer) const
{
	EraTypes eEra = GET_PLAYER(eForPlayer).getCurrentEra();
	CvLeaderHeadInfo const& kLeader = GC.getInfo(getPersonalityType());
	if (GET_TEAM(eForPlayer).isAtWar(getTeam()))
		return kLeader.getDiploWarIntroMusicScriptIds(eEra);
	return kLeader.getDiploPeaceIntroMusicScriptIds(eEra);
}

int CvPlayer::getMusicScriptId(PlayerTypes eForPlayer) const
{
	EraTypes eEra = GET_PLAYER(eForPlayer).getCurrentEra();
	CvLeaderHeadInfo const& kLeader = GC.getInfo(getLeaderType());
	if (GET_TEAM(eForPlayer).isAtWar(getTeam()))
		return kLeader.getDiploWarMusicScriptIds(eEra);
	return kLeader.getDiploPeaceMusicScriptIds(eEra);
}


void CvPlayer::getGlobeLayerColors(GlobeLayerTypes eGlobeLayerType, int iOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	PROFILE_FUNC(); // advc.opt
	CvGame const& g = GC.getGame();
	/*  <advc> These get cleared by some of the subroutines, but should be
		empty to begin with. If not, there could be a memory leak. */
	FAssert(aColors.empty() && aIndicators.empty());
	FAssert(getID() == g.getActivePlayer()); // </advc>
	// <advc.706>
	if(g.isOption(GAMEOPTION_RISE_FALL) && CvPlot::isAllFog())
		return; // </advc.706>
	switch (eGlobeLayerType)
	{
	case GLOBE_LAYER_TRADE:
		getTradeLayerColors(aColors, aIndicators);
		break;
	case GLOBE_LAYER_UNIT:
		getUnitLayerColors((GlobeLayerUnitOptionTypes) iOption, aColors, aIndicators);
		break;
	case GLOBE_LAYER_RESOURCE:
		getResourceLayerColors((GlobeLayerResourceOptionTypes) iOption, aColors, aIndicators);
		break;
	case GLOBE_LAYER_RELIGION:
		getReligionLayerColors((ReligionTypes) iOption, aColors, aIndicators);
		break;
	case GLOBE_LAYER_CULTURE:
		getCultureLayerColors(aColors, aIndicators);
		break;
	default:
		FAssertMsg(false, "Unknown globe layer type");
	}
}

void CvPlayer::getTradeLayerColors(std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	CvMap const& kMap = GC.getMap();
	aColors.resize(kMap.numPlots(), NiColorA(0, 0, 0, 0));
	aIndicators.clear();
	// <advc.004z>
	static bool const bSHOW_FOREIGN = (GC.getDefineINT("FOREIGN_GROUPS_ON_TRADE_LAYER") > 0);
	static bool const bSHOW_CAPITAL_CONN = (GC.getDefineINT("CONNECTION_TO_CAPITAL_ON_TRADE_LAYER") > 0);
	// </advc.004z>
	typedef std::map< int, std::vector<int> > PlotGroupMap;
	PlotGroupMap mapPlotGroups;
	std::vector<int> aiNotConn; // advc.004z
	for (int iI = 0; iI < kMap.numPlots(); iI++)
	{
		CvPlot const& kPlot = kMap.getPlotByIndex(iI);
		PlayerTypes eOwner = kPlot.getOwner(); // advc.004z
		CvPlotGroup* pPlotGroup = kPlot.getPlotGroup(getID());
		if (pPlotGroup != NULL && kPlot.isRevealed(getTeam(), true) &&
			(kPlot.getTeam() == getTeam() ||
			// <advc.004z>
			(bSHOW_FOREIGN && !kPlot.isImpassable() &&
			eOwner != BARBARIAN_PLAYER &&
			// Get rid of insignificant groups
			(pPlotGroup->getLengthPlots() >= 5 ||
			!kPlot.isWater() || eOwner != NO_PLAYER))))
		{
			if(bSHOW_CAPITAL_CONN && kPlot.isVisible(getTeam()) &&
				kPlot.isCity() && !kPlot.getPlotCity()->isConnectedToCapital())
			{
				aiNotConn.push_back(iI);
			}
			else // </advc.004z>
				mapPlotGroups[pPlotGroup->getID()].push_back(iI);
		}
	} // <advc.004z> Use the player color for the plot group of the capital
	CvPlotGroup* pCapitalGroup = (getCapitalCity() == NULL ? NULL :
			getCapitalCity()->getPlot().getPlotGroup(getID()));
	if (pCapitalGroup != NULL)
	{
		FAssert(pCapitalGroup->getLengthPlots() > 0);
		int iCount = mapPlotGroups.count(pCapitalGroup->getID());
		if(iCount > 0)
		{
			NiColorA kColor(getPlayerTextColorR() / 255.f,
					getPlayerTextColorG() / 255.f,
					getPlayerTextColorB() / 255.f,
					getPlayerTextColorA() / 255.f * (bSHOW_FOREIGN ? 0.5f : 0.8f));
			std::vector<int>& aPlots = mapPlotGroups[pCapitalGroup->getID()];
			for(size_t i = 0; i < aPlots.size(); i++)
				aColors[aPlots[i]] = kColor;
		}
		else FAssert(iCount > 0);
	} // </advc.004z>
	CvRandom kRandom;
	kRandom.init(42);
	for (PlotGroupMap::iterator it = mapPlotGroups.begin(); it != mapPlotGroups.end(); ++it)
	{	// <advc.004z> Already handled above
		if(pCapitalGroup != NULL && it->first == pCapitalGroup->getID())
			continue; // </advc.004z>
		NiColorA kColor(kRandom.getFloat(), kRandom.getFloat(), kRandom.getFloat(),
				// advc.004z: Can't tell apart land and water at 0.8
				bSHOW_FOREIGN ? 0.5f :
				0.8f);
		std::vector<int>& aPlots = it->second;
		for (size_t i = 0; i < aPlots.size(); ++i)
		{
			aColors[aPlots[i]] = kColor;
		}
		/*  <advc.004z> The first random color is good (white), but one of the
			next few is a light blue that is too similar to the white.
			Discard one float to avoid that color. */
		kRandom.getFloat();
	}
	NiColorA kColor(0, 0, 0, 0.75f);
	for (size_t i = 0; i < aiNotConn.size(); i++)
		aColors[aiNotConn[i]] = kColor; // </advc.004z>
}

void CvPlayer::getUnitLayerColors(GlobeLayerUnitOptionTypes eOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	// <advc.004z>
	if(eOption == 2) // Somehow, eOption==GLOBE_LAYER_UNIT_DUMMY doesn't work.
		eOption = SHOW_ALL_MILITARY;
	// </advc.004z>
	CvMap const& kMap = GC.getMap();
	aColors.resize(kMap.numPlots(), NiColorA(0, 0, 0, 0));
	aIndicators.clear();

	std::vector< std::vector<float> > aafPlayerPlotStrength(MAX_PLAYERS);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			aafPlayerPlotStrength[i].resize(kMap.numPlots());
		}
	}

	float fMaxPlotStrength = 0.0f;

	// create unit plot indicators...
	// build the trade group texture
	typedef std::map<int, NiColor> GroupMap;
	GroupMap mapColors;

	// Loop through all the players  (advc: reduced indentation in the loops)
	CvWStringBuffer szBuffer;
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		if (!GET_PLAYER((PlayerTypes)iPlayer).isAlive())
			continue;
		for (int iI = 0; iI < kMap.numPlots(); ++iI)
		{
			CvPlot const& kPlot = kMap.getPlotByIndex(iI);
			int iNumUnits = kPlot.getNumUnits();
			if (iNumUnits <= 0 || !kPlot.isVisible(getTeam(), true))
				continue;

			float fPlotStrength = 0.0f;
			bool bShowIndicator = false;

			for (CLLNode<IDInfo> const* pUnitNode = kPlot.headUnitNode(); pUnitNode != NULL;
				pUnitNode = kPlot.nextUnitNode(pUnitNode))
			{
				CvUnit const* pUnit = ::getUnit(pUnitNode->m_data);
				if (pUnit->getVisualOwner() != iPlayer ||
					pUnit->isInvisible(getTeam(), true))
				{
					continue;
				}
				// now, is this unit of interest?
				bool bMilitary = //pUnit->baseCombatStr() > 0;
						// advc.004z: The above doesn't work for air units
						pUnit->canCombat();
				bool bEnemy = pUnit->isEnemy(getTeam());
				bool bOnOurTeam = pUnit->getTeam() == getTeam();
				bool bOfInterest = false;

				switch (eOption)
				{
				case SHOW_ALL_MILITARY:
				{
					bOfInterest = bMilitary;
					if (bOfInterest)
					{
						fPlotStrength += ((float) pUnit->currHitPoints() / (float) pUnit->maxHitPoints() * (float) pUnit->baseCombatStr());
					}
					break;
				}
				case SHOW_TEAM_MILITARY:
				{
					bOfInterest = bMilitary && bOnOurTeam;
					if (bOfInterest)
						fPlotStrength += ((float) pUnit->currHitPoints() / (float) pUnit->maxHitPoints() * (float) pUnit->baseCombatStr());
					break;
				}
				case SHOW_ENEMIES:
				{
					bOfInterest = bMilitary && bEnemy;
					if (bOfInterest)
						fPlotStrength += ((float) pUnit->currHitPoints() / (float) pUnit->maxHitPoints() * (float) pUnit->baseCombatStr());
					break;
				}
				case SHOW_ENEMIES_IN_TERRITORY:
				{
					bOfInterest = bMilitary;
					break;
				}
				case SHOW_PLAYER_DOMESTICS:
				{
					bOfInterest = !bMilitary;// && (pUnit->getVisualOwner() == eCurPlayer);
					break;
				}
				default:
					bOfInterest = false;
					break;
				}
				// create the indicator
				if (bOfInterest)
					bShowIndicator = true;

				fMaxPlotStrength = std::max(fPlotStrength, fMaxPlotStrength);
				aafPlayerPlotStrength[iPlayer][iI] = fPlotStrength;
			}

			if (bShowIndicator)
			{	// <advc.004z> Don't show a defender when we need !bMilitary
				CvUnit* pUnit = NULL;
				if(eOption == SHOW_PLAYER_DOMESTICS)
				{
					UnitAITypes priorityList[] = {
						UNITAI_GENERAL, UNITAI_ARTIST, UNITAI_ENGINEER,
						UNITAI_MERCHANT, UNITAI_GREAT_SPY, UNITAI_PROPHET,
						UNITAI_SCIENTIST, UNITAI_SETTLE, UNITAI_SPY,
						UNITAI_MISSIONARY, UNITAI_WORKER, UNITAI_WORKER_SEA
					};
					PlayerTypes eActivePlayer = GC.getGame().getActivePlayer();
					TeamTypes eActiveTeam = GC.getGame().getActiveTeam();
					for(int j = 0; j < sizeof(priorityList) / sizeof(UnitAITypes); j++) {
						// Use unit owner as tiebreaker
						for(int k = 0; k < 3; k++)
						{
							pUnit = kPlot.plotCheck(PUF_isUnitAIType,
									priorityList[j], -1,
									(k == 0 ? eActivePlayer : NO_PLAYER),
									(k == 1 ? eActiveTeam : NO_TEAM),
									PUF_isVisibleDebug, eActivePlayer, -1);
							if(pUnit != NULL)
								break;
						}
						if(pUnit != NULL)
							break;
					}
					if(pUnit == NULL)
					{
						pUnit = kPlot.plotCheck(PUF_cannotDefend,
								-1, -1, NO_PLAYER, NO_TEAM,
								PUF_isVisibleDebug, eActivePlayer, -1);
					}
				}
				else // </advc.004z>
					pUnit = kPlot.getBestDefender(NO_PLAYER);
				if (pUnit != NULL)
				{
					PlayerColorTypes eUnitColor = GET_PLAYER(pUnit->getVisualOwner()).
							getPlayerColor();
					const NiColorA& kColor = GC.getInfo(GC.getInfo(eUnitColor).
							getColorTypePrimary()).getColor();

					szBuffer.clear();
					GAMETEXT.setPlotListHelp(szBuffer, kPlot, true, true,
							true); // advc.061, advc.007

					CvPlotIndicatorData kIndicator;
					kIndicator.m_pUnit = pUnit;
					kIndicator.m_strLabel = "UNITS";
					kIndicator.m_strIcon = pUnit->getButton();

					if (eOption == SHOW_ENEMIES_IN_TERRITORY)
					{
						kIndicator.m_kColor.r = 1;
						/*kIndicator.m_kColor.r = 0;
						kIndicator.m_kColor.r = 0;*/
						// <advc.001> Replacing the above
						kIndicator.m_kColor.g = 0;
						kIndicator.m_kColor.b = 0; // </advc.001>
					}
					else
					{
						kIndicator.m_kColor.r = kColor.r;
						kIndicator.m_kColor.g = kColor.g;
						kIndicator.m_kColor.b = kColor.b;
					}
					kIndicator.m_strHelpText = szBuffer.getCString();

					//setup visibility
					switch (eOption)
					{
					case SHOW_ENEMIES_IN_TERRITORY:
						kIndicator.m_bTestEnemyVisibility = true;
						kIndicator.m_eVisibility = PLOT_INDICATOR_VISIBLE_ALWAYS;
						break;
					case SHOW_ENEMIES:
						kIndicator.m_eVisibility = PLOT_INDICATOR_VISIBLE_ALWAYS;
						break;
					default:
						kIndicator.m_eVisibility = PLOT_INDICATOR_VISIBLE_ONSCREEN_ONLY;
						break;
					}
					aIndicators.push_back(kIndicator);
				}
			}
		}
	}

	if (fMaxPlotStrength > 0)
	{
		for (PlayerIter<ALIVE> it; it.hasNext(); ++it)
		{
			CvPlayer const& kUnitOwner = *it;
			NiColorA const& kOwnerColor = GC.getInfo(GC.getInfo(
					kUnitOwner.getPlayerColor()).getColorTypePrimary()).getColor();
			for (int i = 0; i < kMap.numPlots(); i++)
			{
				CvPlot const& kPlot = kMap.getPlotByIndex(i);
				if (kPlot.isVisible(getTeam(), true))
				{
					float fPlotStrength = aafPlayerPlotStrength[kUnitOwner.getID()][i];
					if (fPlotStrength > 0)
					{
						float fAlpha = (fPlotStrength / fMaxPlotStrength * 0.75f + 0.25f) * 0.8f;
						if (fAlpha > aColors[i].a)
						{
							aColors[i] = kOwnerColor;
							aColors[i].a = fAlpha;
						}
					}
				}
			}
		}
	}
}

void CvPlayer::getResourceLayerColors(GlobeLayerResourceOptionTypes eOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	aColors.clear();
	aIndicators.clear();

	PlayerColorTypes ePlayerColor = getPlayerColor();
	CvWStringBuffer szBuffer;
	CvMap& kMap = GC.getMap();
	for (int iI = 0; iI < kMap.numPlots(); iI++)
	{
		CvPlot const& kPlot = kMap.getPlotByIndex(iI);
		if(!kPlot.isRevealed(getTeam(), true))
			continue;
		BonusTypes eLoopBonus = kPlot.getBonusType(
				(GC.getGame().isDebugMode()) ? NO_TEAM : getTeam());
		bool bOfInterest = false; // advc.004z
		if (eLoopBonus != NO_BONUS)
		{
			CvBonusInfo& kBonusInfo = GC.getInfo(eLoopBonus);
			switch (eOption)
			{
			case SHOW_ALL_RESOURCES:
				bOfInterest = true;
				break;
			case SHOW_STRATEGIC_RESOURCES:
				bOfInterest = (kBonusInfo.getHappiness() == 0 && kBonusInfo.getHealth() == 0);
				break;
			case SHOW_HAPPY_RESOURCES:
				bOfInterest = (kBonusInfo.getHappiness() != 0 && kBonusInfo.getHealth() == 0);
				break;
			case SHOW_HEALTH_RESOURCES:
				bOfInterest = (kBonusInfo.getHappiness() == 0 && kBonusInfo.getHealth() != 0);
				break;
			}
		} // <advc.004z>
		ImprovementTypes eImpr = NO_IMPROVEMENT;
		if(!bOfInterest && eOption == SHOW_ALL_RESOURCES &&
			isOption(PLAYEROPTION_NO_UNIT_RECOMMENDATIONS) &&
			BUGOption::isEnabled("MainInterface__TribalVillageIcons", true))
		{
			eImpr = kPlot.getRevealedImprovementType(getTeam());
			bOfInterest = (eImpr != NO_IMPROVEMENT && GC.getInfo(eImpr).
					isGoody());
		} // </advc.004z>
		if (bOfInterest)
		{
			CvPlotIndicatorData kData;
			kData.m_strLabel = "RESOURCES";
			kData.m_eVisibility = PLOT_INDICATOR_VISIBLE_ONSCREEN_ONLY;
			kData.m_strIcon = // <advc.004z>
					(eLoopBonus == NO_BONUS ? GC.getInfo(eImpr).
					getButton() // </advc.004z>
					: GC.getInfo(eLoopBonus).getButton());

			int x = kPlot.getX();
			int y = kPlot.getY();
			kData.m_Target = NiPoint2(kMap.plotXToPointX(x), kMap.plotYToPointY(y));
			PlayerTypes eOwner = kPlot.getRevealedOwner(getTeam(), true);
			if (eOwner == NO_PLAYER)
			{
				kData.m_kColor.r = 0.8f;
				kData.m_kColor.g = 0.8f;
				kData.m_kColor.b = 0.8f;
			}
			else
			{
				PlayerColorTypes eCurPlayerColor = GET_PLAYER(eOwner).getKnownPlayerColor();
				const NiColorA& kColor = GC.getInfo(GC.getInfo(eCurPlayerColor).
						getColorTypePrimary()).getColor();
				kData.m_kColor.r = kColor.r;
				kData.m_kColor.g = kColor.g;
				kData.m_kColor.b = kColor.b;
			}

			szBuffer.clear();
			// <advc.004z>
			if(eLoopBonus == NO_BONUS)
				GAMETEXT.setImprovementHelp(szBuffer, kPlot.getImprovementType());
			else // </advc.004z>
			{
				//GAMETEXT.setBonusHelp(szBuffer, eCurType, false);
				// <advc.003p> Replacing the above
				if(m_aszBonusHelp[eLoopBonus] != NULL)
					szBuffer.append(*m_aszBonusHelp[eLoopBonus]);
				else
				{
					CvWStringBuffer szTempBuffer;
					GAMETEXT.setBonusHelp(szTempBuffer, eLoopBonus, false);
					m_aszBonusHelp[eLoopBonus] = new CvWString(szTempBuffer.getCString());
					szBuffer.append(szTempBuffer);
				}
			} // </advc.003p>
			kData.m_strHelpText = szBuffer.getCString();

			aIndicators.push_back(kData);
		}
	}
}

void CvPlayer::getReligionLayerColors(ReligionTypes eSelectedReligion, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const  // advc: some style changes
{
	aColors.resize(GC.getMap().numPlots(), NiColorA(0, 0, 0, 0));
	aIndicators.clear();

	CvRandom kRandom;
	kRandom.init(42 * eSelectedReligion);
	const NiColorA kBaseColor(kRandom.getFloat(), kRandom.getFloat(), kRandom.getFloat(), 1.0f);

	for (int iI = 0; iI  < MAX_PLAYERS; iI ++)
	{
		CvPlayer const& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (!kLoopPlayer.isAlive())
			continue;

		FOR_EACH_CITY(pLoopCity, kLoopPlayer)
		{
			if (!pLoopCity->isRevealed(getTeam(), true) || !pLoopCity->isHasReligion(eSelectedReligion))
				continue;

			float fAlpha = 0.8f;
			if (!pLoopCity->isHolyCity(eSelectedReligion))
				fAlpha *= 0.5f;

			for (CityPlotIter it(*pLoopCity); it.hasNext(); ++it)
			{
				if (!it->isRevealed(getTeam(), true))
					continue; // advc
				int iIndex = GC.getMap().plotNum(*it);
				if (fAlpha > aColors[iIndex].a)
				{
					aColors[iIndex] = kBaseColor;
					aColors[iIndex].a = fAlpha;
				}
			}
		}
	}
}

void CvPlayer::getCultureLayerColors(std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{	/*  advc.004z (comment): Can increase this for more precise color proportions,
		but the color pattern used by the EXE makes higher values (e.g. 10) look strange. */
	const int iColorsPerPlot = 4;
	CvMap const& m = GC.getMap();
	aColors.resize(m.numPlots() * iColorsPerPlot, NiColorA(0, 0, 0, 0));
	aIndicators.clear();

	// find maximum total culture
	int iMaxTotalCulture = MIN_INT;
	int iMinTotalCulture = MAX_INT;
	for (int iI = 0; iI < m.numPlots(); iI++)
	{
		CvPlot const& kLoopPlot = m.getPlotByIndex(iI);
		// <advc.004z>
		if(!kLoopPlot.isVisible(getTeam(), true))
			continue; // </advc.004z>
		int iTotalCulture = kLoopPlot.getTotalCulture(); // advc.opt: was countTotalCulture
		if (iTotalCulture > iMaxTotalCulture)
			iMaxTotalCulture = iTotalCulture;
		if (iTotalCulture < iMinTotalCulture && iTotalCulture > 0)
			iMinTotalCulture = iTotalCulture;
	}
	iMinTotalCulture = 0;

	// find culture percentages
	for (int iI = 0; iI < m.numPlots(); iI++)
	{
		CvPlot const& kLoopPlot = m.getPlotByIndex(iI);
		PlayerTypes eOwner = kLoopPlot.getRevealedOwner(getTeam(), true);
		if(eOwner == NO_PLAYER)
			continue; // advc: Moved up
		// how many people own this plot?
		std::vector<std::pair<int,PlayerTypes> > plot_owners;
		//int iNumNonzeroOwners = 0;
		// K-Mod
		int iTotalCulture = kLoopPlot.getTotalCulture(); // advc.opt: was countTotalCulture
		if (iTotalCulture <= 0)
			continue;
		// K-Mod end
		// <advc.004z>
		plot_owners.push_back(std::make_pair(kLoopPlot.getCulture(eOwner), eOwner));
		bool bVisible = kLoopPlot.isVisible(getTeam(), true);
		if(bVisible)
		{ // </advc.004z>
			// dlph.21: include Barbarians; advc.099: include defeated.
			for (PlayerIter<EVER_ALIVE> it; it.hasNext(); ++it)
			{
				PlayerTypes ePlayer = it->getID();
				// <advc.004z> Owner handled above
				if(ePlayer == eOwner)
					continue; // </advc.004z>
				int iCurCultureAmount = kLoopPlot.getCulture(ePlayer);
				//if (iCurCultureAmount != 0)
				// K-Mod (to reduce visual spam from small amounts of culture)
				if (100 * iCurCultureAmount >= 20 * iTotalCulture)
				{	//iNumNonzeroOwners ++;
					plot_owners.push_back(std::make_pair(iCurCultureAmount, ePlayer));
				}
			}
		}
		//if (!plot_owners.empty())
		/*  <advc.004z> Give players with a high percentage a bigger share of
			the colored area. */
		bool baDone[MAX_PLAYERS] = {false};
		for(int iPass = 0; iPass < 2 &&
			// Try to fill plot_owners up
			plot_owners.size() < iColorsPerPlot; iPass++)
		{
			// To avoid adding to plot_owners while looping through it
			std::vector<std::pair<int,PlayerTypes> > repeated_owners;
			for(size_t i = 0; i < plot_owners.size(); i++)
			{
				if(baDone[plot_owners[i].second])
					continue; // Skip iPass==1 if iExtra added in iPass==0
				int iExtra = -1; // Already once in plot_owners
				if(iPass == 0) // Round down
					iExtra += (plot_owners[i].first * iColorsPerPlot) / iTotalCulture;
				else if(iPass == 1) // Round to nearest
				{
					iExtra += ROUND_DIVIDE(plot_owners[i].first * iColorsPerPlot,
							iTotalCulture);
				} // Respect size limit
				iExtra = std::min(iExtra, iColorsPerPlot - (int)
						(plot_owners.size() + repeated_owners.size()));
				for(int j = 0; j < iExtra; j++)
				{
					repeated_owners.push_back(plot_owners[i]);
					baDone[plot_owners[i].second] = true;
				}
			}
			/*  The more often a civ appears in plot_owners, the more pixels
				will be set to the civ's color. */
			for(size_t i = 0; i < repeated_owners.size(); i++)
				plot_owners.push_back(repeated_owners[i]);
		}
		/*	Ideally ==iColorsPerPlot, but my algorithm above can't guarantee that.
			Can be 1 greater than iColorsPerPlot because the territorial owner is
			always included. */
		FAssert(plot_owners.size() <= iColorsPerPlot + 1);
		// </advc.004z>
		for (int i = 0;
			// adv.004z: max (see comment above)
			i < std::max(iColorsPerPlot, (int)plot_owners.size()); i++)
		{
			int iCurOwnerIdx = i % plot_owners.size();
			PlayerTypes eCurOwnerID = (PlayerTypes) plot_owners[iCurOwnerIdx].second;
			int iCurCulture = plot_owners[iCurOwnerIdx].first;
			const NiColorA& kCurColor = GC.getInfo(GC.getInfo(GET_PLAYER(eCurOwnerID).
					getKnownPlayerColor()).getColorTypePrimary()).getColor();
			// damp the color by the value...
			aColors[iI * iColorsPerPlot + i] = kCurColor;
			/*  <advc.004z> Don't give away info about fogged tiles.
				Use a low factor b/c fogged tiles already look darker than
				visible tiles. */
			float blend_factor = 0.1f;
			if(bVisible) // </advc.004z>
				blend_factor = (iCurCulture - iMinTotalCulture) / (float)iMaxTotalCulture;
			blend_factor = 0.5f * std::min(1.0f, std::max(//0.0f,
					0.1f, // advc.004z
					blend_factor));
			// advc.004z: Coefficient before blend_factor was 0.8
			aColors[iI * iColorsPerPlot + i].a = std::min(0.75f * blend_factor + 0.5f, 1.0f);
		}
	}
}

// advc.003p:
void CvPlayer::setBonusHelpDirty()
{
	if(m_aszBonusHelp == NULL)
	{
		FAssert(m_aszBonusHelp != NULL);
		return;
	}
	for(int i = 0; i < GC.getNumBonusInfos(); i++)
		SAFE_DELETE(m_aszBonusHelp[i])
}


void CvPlayer::cheat(bool bCtrl, bool bAlt, bool bShift)
{
	//if (gDLL->getChtLvl() > 0)
	if(GC.getGame().isDebugMode()) // advc.007b
		GET_TEAM(getTeam()).setHasTech(getCurrentResearch(), true, getID(), true, false);
}

const CvArtInfoUnit* CvPlayer::getUnitArtInfo(UnitTypes eUnit, int iMeshGroup) const
{
	CivilizationTypes eCivilization = getCivilizationType();
	if (eCivilization == NO_CIVILIZATION)
		eCivilization = (CivilizationTypes)GC.getDefineINT("BARBARIAN_CIVILIZATION");
	// <advc.001> Redirect the call to the city owner
	if(gDLL->getInterfaceIFace()->isCityScreenUp())
	{
		PlayerTypes eCityOwner = gDLL->getInterfaceIFace()->getHeadSelectedCity()->getOwner();
		if(eCityOwner != getID())
			return GET_PLAYER(eCityOwner).getUnitArtInfo(eUnit, iMeshGroup);
	} // </advc.001>
	UnitArtStyleTypes eStyle = (UnitArtStyleTypes) GC.getInfo(eCivilization).getUnitArtStyleType();
	EraTypes eEra = getCurrentEra();
	if (eEra == NO_ERA)
		eEra = (EraTypes)0;
	return GC.getInfo(eUnit).getArtInfo(iMeshGroup, eEra, eStyle);
}

// K-Mod. I've moved the original code to a new function: CvTeam::hasSpaceshipArrived
bool CvPlayer::hasSpaceshipArrived() const
{
	return GET_TEAM(getTeam()).hasSpaceshipArrived();
}

// advc.210:
void CvPlayer::checkAlert(int iAlertID, bool bSilent)
{
	if (m_paAlerts.empty())
	{
		FAssertMsg(false, "Alerts not initialized for this player");
		return;
	}
	if(iAlertID < 0 || iAlertID > (int)m_paAlerts.size())
	{
		FAssertMsg(false, "Invalid alert");
		return;
	}
	m_paAlerts[iAlertID]->check(bSilent);
}

// advc.104: Inspired by CvTeamAI::AI_estimateTotalYieldRate
// (Tbd.: Move to CvPlayerAI)
double CvPlayer::estimateYieldRate(YieldTypes eYield, int iSamples) const
{
	//PROFILE_FUNC(); // Called very frequently; about 1.5% of the turn times (July 2019).
	CvGame const& g = GC.getGame();
	int iGameTurn = g.getGameTurn();
	int iTurnsPlayed = iGameTurn - g.getStartTurn();
	iSamples = std::min(iSamples, iTurnsPlayed - 1);
	std::vector<double> samples; // double for ::dMedian
	/* When anarchy lasts several turns, the sample may not contain a single
	   non-revolution turn. In this case, increase the sample size gradually. */
	while(samples.empty() && iSamples < iTurnsPlayed)
	{
		for(int i = 1; i <= iSamples; i++)
		{
			int iSampleIndex = iGameTurn - i;
			int h = 0;
			switch(eYield)
			{
			case YIELD_COMMERCE:
				h = getEconomyHistory(iSampleIndex);
				break;
			case YIELD_PRODUCTION:
				h = getIndustryHistory(iSampleIndex);
				break;
			case YIELD_FOOD:
				h = getAgricultureHistory(iSampleIndex);
				break;
			default: FAssert(false);
			}
			if(h > 0) // Omit revolution turns
				samples.push_back(h);
		}
		iSamples++;
	}
	if(samples.empty())
		return 0;
	return ::dMedian(samples);
}

// advc.004x:
void CvPlayer::killAll(ButtonPopupTypes ePopupType, int iData1)
{
	CvGame const& kGame = GC.getGame();
	if(getID() != kGame.getActivePlayer() || !isHuman() ||
		/*	(If outdated non-minimized popups are also a problem, then this
			check could just be removed; should work alright.) */
		!isOption(PLAYEROPTION_MINIMIZE_POP_UPS) ||
		/*	I can't get this to work in network games. The delays introduced by
			net messages cause popups to appear several times. */
		kGame.isNetworkMultiPlayer())
	{
		return;
	}
	// Preserve the popups we don't want killed in newQueue
	std::list<CvPopupInfo*> newQueue;
	for (int iPass = 0; iPass < 2; iPass++)
	{
		if(iPass == 1)
		{
			// Recall popups already launched
			gDLL->getInterfaceIFace()->getDisplayedButtonPopups(m_listPopups);
		}
		for (std::list<CvPopupInfo*>::iterator it = m_listPopups.begin(); it != m_listPopups.end(); it++)
		{
			CvPopupInfo* pPopup = *it;
			if((pPopup->getButtonPopupType() != ePopupType &&
				/*	Don't relaunch a found-religion popup in response to a
					change-religion popup. The player will already have chosen
					and founded a religion, i.e. the found-religion popup is
					essentially already done. */
				(ePopupType != BUTTONPOPUP_CHANGERELIGION || iPass < 1 ||
				pPopup->getButtonPopupType() != BUTTONPOPUP_FOUND_RELIGION) &&
				// Doesn't get minimized, never needs to be relaunched.
				(iPass < 1 || pPopup->getButtonPopupType() != BUTTONPOPUP_DECLAREWARMOVE))
				||
				(iData1 >= 0 && pPopup->getData1() != iData1))
			{
				newQueue.push_back(pPopup);
			}
			else
			{
				if (iPass <= 0)
					SAFE_DELETE(pPopup);
				// else it's still in the list of popups on display
			}
		}
		m_listPopups.clear();
	}
	// The EXE will relaunch these from m_listPopups
	gDLL->getInterfaceIFace()->clearQueuedPopups();
	for (std::list<CvPopupInfo*>::iterator it = newQueue.begin();
		it != newQueue.end(); it++)
	{
		m_listPopups.push_back(*it);
	}
}

// <advc.314>
bool CvPlayer::isGoodyTech(TechTypes techId, bool bProgress) const
{
	CvTechInfo& t = GC.getInfo(techId);
	if(!bProgress && !t.isGoodyTech())
		return false;
	if(bProgress && t.getEra() >= 4)
		return false;
	return canResearch(techId, false, true);
}

void CvPlayer::addGoodyMsg(CvWString s, CvPlot const& p, TCHAR const* sound)
{
	gDLL->UI().addMessage(
			getID(), true, -1, s, sound, MESSAGE_TYPE_MINOR_EVENT,
			ARTFILEMGR.getImprovementArtInfo("ART_DEF_IMPROVEMENT_GOODY_HUT")->getButton(),
			NO_COLOR, p.getX(), p.getY());
}

void CvPlayer::promoteFreeUnit(CvUnit& u, double pr)
{
	FeatureTypes eDefFeature = NO_FEATURE;
	for(int i = 0; i < GC.getNumFeatureInfos(); i++)
	{
		if(GC.getInfo((FeatureTypes)i).getDefenseModifier() +
		   GC.getInfo((FeatureTypes)i).getRivalDefenseModifier() > 0)
		{
			eDefFeature = (FeatureTypes)i;
			break;
		}
	}
	for(int i = 0; i < 2; i++)
	{
		if(!::bernoulliSuccess(pr, "advc.314"))
			break;
		int iBestValue = -1;
		PromotionTypes eBestPromo = NO_PROMOTION;
		PromotionTypes ePrevPromo = NO_PROMOTION;
		for(int j = 0; j < GC.getNumPromotionInfos(); j++)
		{
			PromotionTypes ePromo = (PromotionTypes)j;
			CvPromotionInfo& kPromo = GC.getInfo(ePromo);
			int iUnitCombat = 0;
			for(int k = 0; k < GC.getNumUnitCombatInfos(); k++)
			{
				if(kPromo.getUnitCombat(k))
					iUnitCombat++;
			}
			if((kPromo.getCombatPercent() <= 0 || /* No Combat2 */ j > 0) &&
				(eDefFeature == NO_FEATURE ||
				kPromo.getFeatureDefensePercent(eDefFeature) <= 0) &&
				kPromo.getHillsDefensePercent() <= 0 &&
				/*  This is the Cover promotion. Only that one, Woodsman,
					Guerilla and City Raider have exactly 3 eligible unit
					combat classes. */
				(iUnitCombat != 3 || kPromo.getCityAttackPercent() > 0))
			{
				continue;
			}
			// Second promo needs to build on the first
			/*if(ePrevPromo != NO_PROMOTION &&
					promo.getPrereqPromotion() != ePrevPromo &&
					promo.getPrereqOrPromotion1() != ePrevPromo &&
					promo.getPrereqOrPromotion2() != ePrevPromo &&
					promo.getPrereqOrPromotion2() != ePrevPromo)
				continue;*/
			if(!u.canAcquirePromotion(ePromo))
				continue;
			int iValue = GC.getGame().getSorenRandNum(100, "advc.314");
			std::vector<CvPlot const*> apSurroundings;
			apSurroundings.push_back(u.plot());
			for(int k = 0; k < NUM_DIRECTION_TYPES; k++)
			{
				CvPlot const* p = plotDirection(u.getX(), u.getY(), (DirectionTypes)k);
				if(p != NULL && !p->isWater())
					apSurroundings.push_back(p);
			}
			for(size_t k = 0; k < apSurroundings.size(); k++)
			{
				int iTmpVal = 0;
				if(eDefFeature != NO_FEATURE &&
					apSurroundings[k]->getFeatureType() == eDefFeature)
				{
					iTmpVal += kPromo.getFeatureDefensePercent(eDefFeature);
				}
				// Encourage non-Woodsman then
				else iTmpVal -= kPromo.getFeatureDefensePercent(eDefFeature) / 3;
				if(apSurroundings[k]->isHills())
					iTmpVal += kPromo.getHillsDefensePercent();
				else iTmpVal -= kPromo.getHillsDefensePercent() / 3;
				if(k == 0) // Higher weight for pPlot
					iTmpVal *= 3;
				iValue = std::max(0, iValue + iTmpVal);
			}
			if(iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPromo = ePromo;
			}
		}
		if(eBestPromo == NO_PROMOTION)
			break;
		u.setHasPromotion(eBestPromo, true);
		ePrevPromo = eBestPromo;
	}
} // </advc.314>

// <advc.120f>
void CvPlayer::announceEspionageToThirdParties(EspionageMissionTypes eMission, PlayerTypes eTarget)
{
	bool bReligion = (GC.getInfo(eMission).
			getSwitchReligionCostFactor() > 0);
	CvWString szBuffer = gDLL->getText((bReligion ?
			"TXT_KEY_ESPIONAGE_3RD_PARTY_SWITCH_RELIGION" :
			"TXT_KEY_ESPIONAGE_3RD_PARTY_SWITCH_CIVIC"),
			GET_PLAYER(eTarget).getCivilizationAdjectiveKey()).GetCString();
	int x = -1, y = -1;
	CvCity* pCapital = GET_PLAYER(eTarget).getCapitalCity();
	if (pCapital != NULL)
	{
		x = pCapital->getX();
		y = pCapital->getY();
	}
	if (GC.getDefineINT("ANNOUNCE_ESPIONAGE_REVOLUTION") > 0)
	{
		for (int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			CvPlayer const& kObs = GET_PLAYER((PlayerTypes)i);
			if(kObs.isAlive() && kObs.getID() != getID() && kObs.getID() != eTarget &&
					GET_TEAM(eTarget).isHasMet(kObs.getTeam()))
			{
				gDLL->UI().addMessage(kObs.getID(), false, -1, szBuffer,
						NULL, MESSAGE_TYPE_INFO, NULL, NO_COLOR, x, y);
			}
		}
	}
	if (bReligion)
	{
		CvWString szTmp = gDLL->getText("TXT_KEY_ESPIONAGE_REVEAL_OWNER",
				getCivilizationAdjectiveKey()).GetCString();
		szTmp += L" " + szBuffer;
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, eTarget, szTmp);
	}
} // </advc.120f>

// <advc.opt> Global; see CvPlayer.h.
CvCity* getCityExternal(IDInfo city)
{
	if (city.eOwner == NO_PLAYER)
		return NULL;
	return getCity(city);
}

CvUnit* getUnitExternal(IDInfo unit)
{
	if (unit.eOwner == NO_PLAYER)
		return NULL;
	return getUnit(unit);
} // </advc.opt>
/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
bool CvPlayer::hasValidCivics(UnitTypes eUnit) const
{
	int iI;
	bool bValidOrCivic = false;
	bool bNoReqOrCivic = true;
	bool bValidAndCivic = true;
	bool bReqAndCivic = true;
	for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (GC.getUnitInfo(eUnit).isPrereqOrCivics(iI))
		{
			bNoReqOrCivic = false;
			if (isCivic(CivicTypes(iI)))
			{
				bValidOrCivic = true;
			}
		}
		
		if (GC.getUnitInfo(eUnit).isPrereqAndCivics(iI))
		{
			bReqAndCivic = true;
			if (!isCivic(CivicTypes(iI)))
			{
				bValidAndCivic = false;
			}
		}
	}
	
	if (!bNoReqOrCivic && !bValidOrCivic)
	{
		return false;
	}

	if (bReqAndCivic && !bValidAndCivic)
	{
		return false;
	}
	
	return true;
}
/**
 ** End: Unit Civic Prereq
 **/
//Tholish UnbuildableBuildingDeletion START
bool CvPlayer::canKeep(BuildingTypes eBuilding) const
{
	if (GC.getGame().isOption(GAMEOPTION_BUILDING_DELETION))
	{
	BuildingClassTypes eBuildingClass;
	int iI;
	CvTeamAI& currentTeam = GET_TEAM(getTeam());

	eBuildingClass = ((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType()));

	FAssert(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass) == eBuilding);
	if (GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass) != eBuilding)
	{
		return false;
	}

		if (GC.getBuildingInfo(eBuilding).getProductionCost() == -1)
		{
			return false;
		}


	if (!(currentTeam.isHasTech((TechTypes)(GC.getBuildingInfo(eBuilding).getPrereqAndTech()))))
	{
		return false;
	}

	for (iI = 0; iI < GC.getNUM_BUILDING_AND_TECH_PREREQS(); iI++)
	{
		if (GC.getBuildingInfo(eBuilding).getPrereqAndTechs(iI) != NO_TECH)
		{
			if (!(currentTeam.isHasTech((TechTypes)(GC.getBuildingInfo(eBuilding).getPrereqAndTechs(iI)))))
			{
				return false;
			}
		}
	}

	if (GC.getBuildingInfo(eBuilding).getSpecialBuildingType() != NO_SPECIALBUILDING)
	{
		if (!(currentTeam.isHasTech((TechTypes)(GC.getSpecialBuildingInfo((SpecialBuildingTypes) GC.getBuildingInfo(eBuilding).getSpecialBuildingType()).getTechPrereq()))))
		{
			return false;
		}
	}

	if (GC.getBuildingInfo(eBuilding).getStateReligion() != NO_RELIGION)
	{
		if (getStateReligion() != GC.getBuildingInfo(eBuilding).getStateReligion())
		{
			return false;
		}
	}


		if (getHighestUnitLevel() < GC.getBuildingInfo(eBuilding).getUnitLevelPrereq())
		{
			return false;
		}




	return true;
	}
return false;
}
//Tholish UnbuildableBuildingDeletion END
//KNOEDELbegin CULTURAL_GOLDEN_AGE 8/8
int CvPlayer::getCultureGoldenAgeProgress() const
{
	return m_iCultureGoldenAgeProgress;
}

void CvPlayer::changeCultureGoldenAgeProgress(int iChange)
{
	m_iCultureGoldenAgeProgress = (m_iCultureGoldenAgeProgress + iChange);
	FAssert(getCultureGoldenAgeProgress() >= 0);
}

int CvPlayer::getCultureGoldenAgesStarted() const
{
	return m_iCultureGoldenAgesStarted;
}


void CvPlayer::incrementCultureGoldenAgeStarted()
{
	if (m_iCultureGoldenAgesStarted > 100)
	{
			m_iCultureGoldenAgesStarted = 200;
	}
	else
	{
			m_iCultureGoldenAgesStarted *= 2;
	}
}

int CvPlayer::getCultureGoldenAgeThreshold() const
{
	int iThreshold;

	iThreshold = (GC.getDefineINT("CULTURE_GOLDEN_AGE_THRESHOLD") * std::max(0, (getCultureGoldenAgesStarted())));

	iThreshold *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getGreatPeoplePercent();
	iThreshold /= 100;

	iThreshold *= GC.getEraInfo(GC.getGame().getStartEra()).getGreatPeoplePercent();
	iThreshold /= 100;

	iThreshold *= GC.getWorldInfo(GC.getMap().getWorldSize()).getResearchPercent();
	iThreshold /= 130; // research percent standard size

	return std::max(1, iThreshold);
}//KNOEDELend