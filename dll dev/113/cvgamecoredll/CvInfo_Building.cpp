// advc.003x: Cut from CvInfos.cpp

#include "CvGameCoreDLL.h"
#include "CvInfo_Building.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"

CvBuildingInfo::CvBuildingInfo() :
m_iBuildingClassType(NO_BUILDINGCLASS),
m_iVictoryPrereq(NO_VICTORY),
m_iFreeStartEra(NO_ERA),
m_iMaxStartEra(NO_ERA),
m_iObsoleteTech(NO_TECH),
m_iPrereqAndTech(NO_TECH),
m_iNoBonus(NO_BONUS),
m_iPowerBonus(NO_BONUS),
m_iFreeBonus(NO_BONUS),
m_iNumFreeBonuses(0),
//Doto-prereqMust+tholish
m_iPrereqMustAll(0),
// < Doto-Building Resource Converter Start >
m_paiRequiredInputBonuses(NULL),
m_paiBuildingOutputBonuses(NULL),
// < Doto-Building Resource Converter End   >
m_iFreeBuildingClass(NO_BUILDINGCLASS),
m_iFreePromotion(NO_PROMOTION),
m_iCivicOption(NO_CIVICOPTION),
m_iAIWeight(0),
m_iProductionCost(0),
m_iHurryCostModifier(0),
m_iHurryAngerModifier(0),
m_iAdvancedStartCost(0),
m_iAdvancedStartCostIncrease(0),
m_iMinAreaSize(0),
m_iNumCitiesPrereq(0),
/************************************************************************************************/
/* Doto-City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
m_iNumCitySizeBldPrereq(0),
/************************************************************************************************/
/* Doto-City Size Prerequisite                  END                                                  */
/************************************************************************************************/
m_iNumTeamsPrereq(0),
m_iUnitLevelPrereq(0),
m_iMinLatitude(0),
m_iMaxLatitude(90),
m_iGreatPeopleRateModifier(0),
m_iGreatGeneralRateModifier(0),
m_iDomesticGreatGeneralRateModifier(0),
m_iGlobalGreatPeopleRateModifier(0),
m_iAnarchyModifier(0),
m_iGoldenAgeModifier(0),
m_iGlobalHurryModifier(0),
m_iFreeExperience(0),
m_iGlobalFreeExperience(0),
/* Doto-Population Limit ModComp - Beginning */
m_iPopulationLimitChange(0),
/* Doto-Population Limit ModComp - End */
m_iFoodKept(0),
m_iAirlift(0),
m_iAirModifier(0),
m_iAirUnitCapacity(0),
m_iNukeModifier(0),
m_iNukeExplosionRand(0),
m_iFreeSpecialist(0),
m_iAreaFreeSpecialist(0),
m_iGlobalFreeSpecialist(0),
m_iHappiness(0),
m_iAreaHappiness(0),
m_iGlobalHappiness(0),
m_iStateReligionHappiness(0),
m_iWorkerSpeedModifier(0),
m_iMilitaryProductionModifier(0),
m_iSpaceProductionModifier(0),
m_iGlobalSpaceProductionModifier(0),
m_iTradeRoutes(0),
m_iCoastalTradeRoutes(0),
m_iAreaTradeRoutes(0), // advc.310
m_iTradeRouteModifier(0),
m_iForeignTradeRouteModifier(0),
m_iAssetValue(0),
m_iPowerValue(0),
m_iSpecialBuildingType(NO_SPECIALBUILDING),
m_iAdvisorType(NO_ADVISOR),
/********************************************************************************/
/**		Doto-REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanConstruct															*/
/********************************************************************************/
m_iPrereqGameOption(NO_GAMEOPTION),										
m_iNotGameOption(NO_GAMEOPTION),
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
m_iHolyCity(NO_RELIGION),
m_iReligionType(NO_RELIGION),
m_iStateReligion(NO_RELIGION),
m_iPrereqReligion(NO_RELIGION),
m_iPrereqCorporation(NO_CORPORATION),
m_iFoundsCorporation(NO_CORPORATION),
m_iGlobalReligionCommerce(0),
m_iGlobalCorporationCommerce(0),
m_iPrereqAndBonus(NO_BONUS),
//Doto-Shqype Vicinity Bonus Add
//m_iPrereqVicinityBonus(NO_BONUS),  
m_iGreatPeopleUnitClass(NO_UNITCLASS),
m_iGreatPeopleRateChange(0),
m_iConquestProbability(0),
m_iMaintenanceModifier(0),
//Doto-DPII < Maintenance Modifier >
m_iGlobalMaintenanceModifier(0),
m_iAreaMaintenanceModifier(0),
m_iOtherAreaMaintenanceModifier(0),
m_iDistanceMaintenanceModifier(0),
m_iNumCitiesMaintenanceModifier(0),
m_iCoastalDistanceMaintenanceModifier(0),
m_iConnectedCityMaintenanceModifier(0),
//Doto-DPII < Maintenance Modifier >
m_iWarWearinessModifier(0),
m_iGlobalWarWearinessModifier(0),
m_iEnemyWarWearinessModifier(0),
m_iHealRateChange(0),
m_iHealth(0),
m_iAreaHealth(0),
m_iGlobalHealth(0),
m_iGlobalPopulationChange(0),
m_iFreeTechs(0),
/*** Doto-HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave START ***/
m_iFreeSpecificTech(NO_TECH),
/*** Doto-HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave END ***/
m_iDefenseModifier(0),
m_iBombardDefenseModifier(0),
m_iAllCityDefenseModifier(0),
m_iEspionageDefenseModifier(0),
m_iMissionType(NO_MISSION),
m_iVoteSourceType(NO_VOTESOURCE),
m_fVisibilityPriority(0.0f),
m_bTeamShare(false),
m_bWater(false),
m_bRiver(false),
m_bPower(false),
m_bDirtyPower(false),
m_bAreaCleanPower(false),
m_bAreaBorderObstacle(false),
m_bConditional(false), // advc.310
m_bForceTeamVoteEligible(false),
m_bCapital(false),
m_bGovernmentCenter(false),
m_bGoldenAge(false),
m_bMapCentering(false),
m_bNoUnhappiness(false),
//m_bNoUnhealthyPopulation(false),
m_iUnhealthyPopulationModifier(0), // K-Mod
m_bBuildingOnlyHealthy(false),
m_bNeverCapture(false),
m_bNukeImmune(false),
m_bPrereqReligion(false),
m_bCenterInCity(false),
m_bStateReligion(false),
m_bAllowsNukes(false),
m_piPrereqAndTechs(NULL),
m_piPrereqOrBonuses(NULL),
//Doto-Shqype Vicinity Bonus Add
//m_piPrereqOrVicinityBonuses(NULL),  
m_piProductionTraits(NULL),
m_piHappinessTraits(NULL),
m_piSeaPlotYieldChange(NULL),
m_piRiverPlotYieldChange(NULL),
m_piGlobalSeaPlotYieldChange(NULL),
m_piYieldChange(NULL),
m_piYieldModifier(NULL),
m_piPowerYieldModifier(NULL),
m_piAreaYieldModifier(NULL),
m_piGlobalYieldModifier(NULL),
m_piCommerceChange(NULL),
m_piObsoleteSafeCommerceChange(NULL),
m_piCommerceChangeDoubleTime(NULL),
m_piCommerceModifier(NULL),
m_piGlobalCommerceModifier(NULL),
m_piSpecialistExtraCommerce(NULL),
m_piStateReligionCommerce(NULL),
m_piCommerceHappiness(NULL),
m_piReligionChange(NULL),
m_piSpecialistCount(NULL),
m_piFreeSpecialistCount(NULL),
m_piBonusHealthChanges(NULL),
m_piBonusHappinessChanges(NULL),
m_piBonusProductionModifier(NULL),
m_piUnitCombatFreeExperience(NULL),
m_piDomainFreeExperience(NULL),
m_piDomainProductionModifier(NULL),
m_piBuildingHappinessChanges(NULL),
m_piPrereqNumOfBuildingClass(NULL),
m_piFlavorValue(NULL),
m_piImprovementFreeSpecialist(NULL),
m_pbCommerceFlexible(NULL),
m_pbCommerceChangeOriginalOwner(NULL),
m_pbBuildingClassNeededInCity(NULL),
m_ppaiSpecialistYieldChange(NULL),
m_ppaiBonusYieldModifier(NULL),
// Doto-davidlallen: building bonus yield, commerce start
m_iBonusConsumed(NO_BONUS),
m_paiCommerceProduced(NULL),
m_paiYieldProduced(NULL),
// Doto-davidlallen: building bonus yield, commerce end
// UNOFFICIAL_PATCH, Efficiency, 06/27/10, Afforess & jdog5000: START
m_bAnySpecialistYieldChange(false),
m_bAnyBonusYieldModifier(false)
// UNOFFICIAL_PATCH: END
{}

CvBuildingInfo::~CvBuildingInfo()
{
	// < Doto-Building Resource Converter Start >
	SAFE_DELETE_ARRAY(m_paiRequiredInputBonuses);
	SAFE_DELETE_ARRAY(m_paiBuildingOutputBonuses);
	// < Doto-Building Resource Converter End   >
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_piPrereqOrBonuses);
//	SAFE_DELETE_ARRAY(m_piPrereqOrVicinityBonuses);  //Shqype Vicinity Bonus Add
	SAFE_DELETE_ARRAY(m_piProductionTraits);
	SAFE_DELETE_ARRAY(m_piHappinessTraits);
	SAFE_DELETE_ARRAY(m_piSeaPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piRiverPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piGlobalSeaPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piPowerYieldModifier);
	SAFE_DELETE_ARRAY(m_piAreaYieldModifier);
	SAFE_DELETE_ARRAY(m_piGlobalYieldModifier);
	SAFE_DELETE_ARRAY(m_piCommerceChange);
	SAFE_DELETE_ARRAY(m_piObsoleteSafeCommerceChange);
	SAFE_DELETE_ARRAY(m_piCommerceChangeDoubleTime);
	SAFE_DELETE_ARRAY(m_piCommerceModifier);
	SAFE_DELETE_ARRAY(m_piGlobalCommerceModifier);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerce);
	SAFE_DELETE_ARRAY(m_piStateReligionCommerce);
	SAFE_DELETE_ARRAY(m_piCommerceHappiness);
	SAFE_DELETE_ARRAY(m_piReligionChange);
	SAFE_DELETE_ARRAY(m_piSpecialistCount);
	SAFE_DELETE_ARRAY(m_piFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_piBonusHealthChanges);
	SAFE_DELETE_ARRAY(m_piBonusHappinessChanges);
	SAFE_DELETE_ARRAY(m_piBonusProductionModifier);
	SAFE_DELETE_ARRAY(m_piUnitCombatFreeExperience);
	SAFE_DELETE_ARRAY(m_piDomainFreeExperience);
	SAFE_DELETE_ARRAY(m_piDomainProductionModifier);
	SAFE_DELETE_ARRAY(m_piBuildingHappinessChanges);
	SAFE_DELETE_ARRAY(m_piPrereqNumOfBuildingClass);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piImprovementFreeSpecialist);
	SAFE_DELETE_ARRAY(m_pbCommerceFlexible);
	SAFE_DELETE_ARRAY(m_pbCommerceChangeOriginalOwner);
	SAFE_DELETE_ARRAY(m_pbBuildingClassNeededInCity);

	if (m_ppaiSpecialistYieldChange != NULL)
	{
		for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
			SAFE_DELETE_ARRAY(m_ppaiSpecialistYieldChange[i]);
		SAFE_DELETE_ARRAY(m_ppaiSpecialistYieldChange);
	}
	if (m_ppaiBonusYieldModifier != NULL)
	{
		for(int i = 0;i < GC.getNumBonusInfos(); i++)
			SAFE_DELETE_ARRAY(m_ppaiBonusYieldModifier[i]);
		SAFE_DELETE_ARRAY(m_ppaiBonusYieldModifier);
	}
	// Doto-davidlallen: building bonus yield, commerce start
	SAFE_DELETE_ARRAY(m_paiCommerceProduced);
	SAFE_DELETE_ARRAY(m_paiYieldProduced);
	// Doto-davidlallen: building bonus yield, commerce end
}
// advc.tag:
void CvBuildingInfo::addElements(std::vector<XMLElement*>& r) const
{
	CvHotkeyInfo::addElements(r);
	r.push_back(new IntElement(RaiseDefense, "RaiseDefense", 0)); // advc.004c
}

// advc.003w:
bool CvBuildingInfo::isTechRequired(TechTypes eTech) const
{
	if (getPrereqAndTech() == eTech)
		return true;

	for (int i = 0; i < GC.getNUM_BUILDING_AND_TECH_PREREQS(); i++)
	{
		if (getPrereqAndTechs(i) == eTech)
			return true;
	}

	SpecialBuildingTypes eSpecial = getSpecialBuildingType();
	if (eSpecial != NO_SPECIALBUILDING && GC.getInfo(eSpecial).getTechPrereq() == eTech)
		return true;

	return false;
}

int CvBuildingInfo::getDomesticGreatGeneralRateModifier() const
{	// <advc.310>
	if(!m_bEnabledDomesticGreatGeneralRateModifier && m_bConditional)
		return 0; // </advc.310>
	return m_iDomesticGreatGeneralRateModifier;
}
// advc.310:
int CvBuildingInfo::getAreaTradeRoutes() const
{
	if(!m_bEnabledAreaTradeRoutes && m_bConditional)
		return 0;
	return m_iAreaTradeRoutes;
} // </advc.310>

//Doto-DPII < Maintenance Modifier >
//due to advc changes - i dont know its this is the place- keldath
int CvBuildingInfo::getGlobalMaintenanceModifier() const
{
    return m_iGlobalMaintenanceModifier;
}

int CvBuildingInfo::getAreaMaintenanceModifier() const
{
    return m_iAreaMaintenanceModifier;
}

int CvBuildingInfo::getOtherAreaMaintenanceModifier() const
{
    return m_iOtherAreaMaintenanceModifier;
}

int CvBuildingInfo::getDistanceMaintenanceModifier() const
{
    return m_iDistanceMaintenanceModifier;
}

int CvBuildingInfo::getNumCitiesMaintenanceModifier() const
{
    return m_iNumCitiesMaintenanceModifier;
}

int CvBuildingInfo::getCoastalDistanceMaintenanceModifier() const
{
    return m_iCoastalDistanceMaintenanceModifier;
}

int CvBuildingInfo::getConnectedCityMaintenanceModifier() const
{
    return m_iConnectedCityMaintenanceModifier;
}
//Doto-DPII < Maintenance Modifier >

void CvBuildingInfo::setMissionType(MissionTypes eNewType)
{
	m_iMissionType = eNewType;
}

float CvBuildingInfo::getVisibilityPriority() const
{
	return m_fVisibilityPriority;
}

bool CvBuildingInfo::isAreaBorderObstacle() const
{	// <advc.310>
	if(!m_bEnabledAreaBorderObstacle && m_bConditional)
		return false; // </advc.310>
	return m_bAreaBorderObstacle;
}

const TCHAR* CvBuildingInfo::getConstructSound() const
{
	return m_szConstructSound;
}

const TCHAR* CvBuildingInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

const TCHAR* CvBuildingInfo::getMovieDefineTag() const
{
	return m_szMovieDefineTag;
}


int CvBuildingInfo::getYieldChange(YieldTypes eYield) const
{
	FAssertBounds(0, NUM_YIELD_TYPES, eYield);
	return m_piYieldChange ?
			(YieldTypes)m_piYieldChange[eYield] : 0; // advc.003t
}

int CvBuildingInfo::getYieldModifier(YieldTypes eYield) const
{
	FAssertBounds(0, NUM_YIELD_TYPES, eYield);
	return m_piYieldModifier ?
			(YieldTypes)m_piYieldModifier[eYield] : 0; // advc.003t
}

int CvBuildingInfo::getPowerYieldModifier(YieldTypes eYield) const
{
	FAssertBounds(0, NUM_YIELD_TYPES, eYield);
	return m_piPowerYieldModifier ?
			(YieldTypes)m_piPowerYieldModifier[eYield] : 0; // advc.003t
}

int CvBuildingInfo::getAreaYieldModifier(YieldTypes eYield) const
{
	FAssertBounds(0, NUM_YIELD_TYPES, eYield);
	return m_piAreaYieldModifier ?
			(YieldTypes)m_piAreaYieldModifier[eYield] : 0; // advc.003t
}

int CvBuildingInfo::getGlobalYieldModifier(YieldTypes eYield) const
{
	FAssertBounds(0, NUM_YIELD_TYPES, eYield);
	return m_piGlobalYieldModifier ?
			(YieldTypes)m_piGlobalYieldModifier[eYield] : 0; // advc.003t
}

int CvBuildingInfo::getSeaPlotYieldChange(YieldTypes eYield) const
{
	FAssertBounds(0, NUM_YIELD_TYPES, eYield);
	return m_piSeaPlotYieldChange ?
			(YieldTypes)m_piSeaPlotYieldChange[eYield] : 0; // advc.003t
}

int CvBuildingInfo::getRiverPlotYieldChange(YieldTypes eYield) const
{
	FAssertBounds(0, NUM_YIELD_TYPES, eYield);
	return m_piRiverPlotYieldChange ?
			(YieldTypes)m_piRiverPlotYieldChange[eYield] : 0; // advc.003t
}

int CvBuildingInfo::getGlobalSeaPlotYieldChange(YieldTypes eYield) const
{
	FAssertBounds(0, NUM_YIELD_TYPES, eYield);
	return m_piGlobalSeaPlotYieldChange ?
			(YieldTypes)m_piGlobalSeaPlotYieldChange[eYield] : 0; // advc.003t
}

int CvBuildingInfo::getCommerceChange(CommerceTypes eCommerce) const
{
	FAssertBounds(0, NUM_COMMERCE_TYPES, eCommerce);
	return m_piCommerceChange ?
			(CommerceTypes)m_piCommerceChange[eCommerce] : 0; // advc.003t
}

int CvBuildingInfo::getObsoleteSafeCommerceChange(CommerceTypes eCommerce) const
{
	FAssertBounds(0, NUM_COMMERCE_TYPES, eCommerce);
	return m_piObsoleteSafeCommerceChange ?
			(CommerceTypes)m_piObsoleteSafeCommerceChange[eCommerce] : 0; // advc.003t
}

int CvBuildingInfo::getCommerceChangeDoubleTime(CommerceTypes eCommerce) const
{
	FAssertBounds(0, NUM_COMMERCE_TYPES, eCommerce);
	return m_piCommerceChangeDoubleTime ?
			(CommerceTypes)m_piCommerceChangeDoubleTime[eCommerce]
			: 0; // advc.003t: Was -1. 0 means infinity here.
}

int CvBuildingInfo::getCommerceModifier(CommerceTypes eCommerce) const
{
	FAssertBounds(0, NUM_COMMERCE_TYPES, eCommerce);
	return m_piCommerceModifier ?
			(CommerceTypes)m_piCommerceModifier[eCommerce] : 0; // advc.003t
}

int CvBuildingInfo::getGlobalCommerceModifier(CommerceTypes eCommerce) const
{
	FAssertBounds(0, NUM_COMMERCE_TYPES, eCommerce);
	return m_piGlobalCommerceModifier ?
			(CommerceTypes)m_piGlobalCommerceModifier[eCommerce] : 0; // advc.003t
}

int CvBuildingInfo::getSpecialistExtraCommerce(CommerceTypes eCommerce) const
{
	FAssertBounds(0, NUM_COMMERCE_TYPES, eCommerce);
	return m_piSpecialistExtraCommerce ?
			(CommerceTypes)m_piSpecialistExtraCommerce[eCommerce] : 0; // advc.003t
}

int CvBuildingInfo::getStateReligionCommerce(CommerceTypes eCommerce) const
{
	FAssertBounds(0, NUM_COMMERCE_TYPES, eCommerce);
	return m_piStateReligionCommerce ?
			(CommerceTypes)m_piStateReligionCommerce[eCommerce] : 0; // advc.003t
}

int CvBuildingInfo::getCommerceHappiness(CommerceTypes eCommerce) const
{
	FAssertBounds(0, NUM_COMMERCE_TYPES, eCommerce);
	return m_piCommerceHappiness ?
			(CommerceTypes)m_piCommerceHappiness[eCommerce] : 0; // advc.003t
}

int CvBuildingInfo::getReligionChange(int i) const
{
	FAssertBounds(0, GC.getNumReligionInfos(), i);
	return m_piReligionChange ? m_piReligionChange[i]
			: 0; // advc.003t: Was -1. This one acts as a boolean actually.
}

int CvBuildingInfo::getSpecialistCount(int i) const
{
	FAssertBounds(0, GC.getNumSpecialistInfos(), i);
	return m_piSpecialistCount ? m_piSpecialistCount[i] : 0; // advc.003t
}

int CvBuildingInfo::getFreeSpecialistCount(int i) const
{
	FAssertBounds(0, GC.getNumSpecialistInfos(), i);
	return m_piFreeSpecialistCount ? m_piFreeSpecialistCount[i] : 0; // advc.003t
}

int CvBuildingInfo::getBonusHealthChanges(int i) const
{
	FAssertBounds(0, GC.getNumBonusInfos(), i);
	return m_piBonusHealthChanges ? m_piBonusHealthChanges[i] : 0; // advc.003t
}

int CvBuildingInfo::getBonusHappinessChanges(int i) const
{
	FAssertBounds(0, GC.getNumBonusInfos(), i);
	return m_piBonusHappinessChanges ? m_piBonusHappinessChanges[i] : 0; // advc.003t
}

int CvBuildingInfo::getBonusProductionModifier(int i) const
{
	FAssertBounds(0, GC.getNumBonusInfos(), i);
	return m_piBonusProductionModifier ? m_piBonusProductionModifier[i] : 0; // advc.003t
}

int CvBuildingInfo::getUnitCombatFreeExperience(int i) const
{
	FAssertBounds(0, GC.getNumUnitCombatInfos(), i);
	return m_piUnitCombatFreeExperience ? m_piUnitCombatFreeExperience[i] : 0; // advc.003t
}

int CvBuildingInfo::getDomainFreeExperience(int i) const
{
	FAssertBounds(0, NUM_DOMAIN_TYPES, i);
	return m_piDomainFreeExperience ? m_piDomainFreeExperience[i] : 0; // advc.003t
}

int CvBuildingInfo::getDomainProductionModifier(int i) const
{
	FAssertBounds(0, NUM_DOMAIN_TYPES, i);
	return m_piDomainProductionModifier ? m_piDomainProductionModifier[i] : 0; // advc.003t
}

TechTypes CvBuildingInfo::getPrereqAndTechs(int i) const
{
	FAssertBounds(0, GC.getNUM_BUILDING_AND_TECH_PREREQS(), i);
	return m_piPrereqAndTechs ? (TechTypes)m_piPrereqAndTechs[i] : NO_TECH; // advc.003t
}

BonusTypes CvBuildingInfo::getPrereqOrBonuses(int i) const
{
	FAssertBounds(0, GC.getNUM_BUILDING_PREREQ_OR_BONUSES(), i);
	return m_piPrereqOrBonuses ? (BonusTypes)m_piPrereqOrBonuses[i] : NO_BONUS; // advc.003t
}

int CvBuildingInfo::getProductionTraits(int i) const
{
	FAssertBounds(0, GC.getNumTraitInfos(), i);
	return m_piProductionTraits ? m_piProductionTraits[i]
			: 0; // advc.003t: Was -1. This is the production discount percentage.
}

int CvBuildingInfo::getHappinessTraits(int i) const
{
	FAssertBounds(0, GC.getNumTraitInfos(), i);
	return m_piHappinessTraits ? m_piHappinessTraits[i]
			: 0; // advc.003t: Was -1. This is the happiness from trait.
}

int CvBuildingInfo::getBuildingHappinessChanges(int i) const
{
	FAssertBounds(0, GC.getNumBuildingClassInfos(), i);
	return m_piBuildingHappinessChanges ? m_piBuildingHappinessChanges[i] : 0; // advc.003t
}

int CvBuildingInfo::getPrereqNumOfBuildingClass(int i) const
{
	FAssertBounds(0, GC.getNumBuildingClassInfos(), i);
	return m_piPrereqNumOfBuildingClass ? m_piPrereqNumOfBuildingClass[i] : 0; // advc.003t
}

int CvBuildingInfo::getFlavorValue(int i) const
{
	FAssertBounds(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0; // advc.003t
}

int CvBuildingInfo::getImprovementFreeSpecialist(int i) const
{
	FAssertBounds(0, GC.getNumImprovementInfos(), i);
	return m_piImprovementFreeSpecialist ? m_piImprovementFreeSpecialist[i] : 0; // advc.003t
}

bool CvBuildingInfo::isCommerceFlexible(int i) const
{
	FAssertBounds(0, NUM_COMMERCE_TYPES, i);
	return m_pbCommerceFlexible ? m_pbCommerceFlexible[i] : false;
}

bool CvBuildingInfo::isCommerceChangeOriginalOwner(int i) const
{
	FAssertBounds(0, NUM_COMMERCE_TYPES, i);
	return m_pbCommerceChangeOriginalOwner ? m_pbCommerceChangeOriginalOwner[i] : false;
}

bool CvBuildingInfo::isBuildingClassNeededInCity(int i) const
{
	FAssertBounds(0, GC.getNumBuildingClassInfos(), i);
	return m_pbBuildingClassNeededInCity ? m_pbBuildingClassNeededInCity[i] : false;
}

int CvBuildingInfo::getSpecialistYieldChange(int i, int j) const
{
	FAssertBounds(0, GC.getNumSpecialistInfos(), i);
	FAssertBounds(0, NUM_YIELD_TYPES, j);
	return m_ppaiSpecialistYieldChange ? m_ppaiSpecialistYieldChange[i][j] : 0; // advc.003t
}

int* CvBuildingInfo::getSpecialistYieldChangeArray(int i) const
{
	FAssertBounds(0, GC.getNumSpecialistInfos(), i);
	return m_ppaiSpecialistYieldChange[i];
}

int CvBuildingInfo::getBonusYieldModifier(int i, int j) const
{
	FAssertBounds(0, GC.getNumBonusInfos(), i);
	FAssertBounds(0, NUM_YIELD_TYPES, j);
	return m_ppaiBonusYieldModifier ? m_ppaiBonusYieldModifier[i][j] : 0; // advc.003t
}

int* CvBuildingInfo::getBonusYieldModifierArray(int i) const
{
	FAssertBounds(0, GC.getNumBonusInfos(), i);
	return m_ppaiBonusYieldModifier[i];
}

const TCHAR* CvBuildingInfo::getButton() const
{
	const CvArtInfoBuilding * pBuildingArtInfo;
	pBuildingArtInfo = getArtInfo();
	if (pBuildingArtInfo != NULL)
		return pBuildingArtInfo->getButton();

	return NULL;
}

const CvArtInfoBuilding* CvBuildingInfo::getArtInfo() const
{
	return ARTFILEMGR.getBuildingArtInfo(getArtDefineTag());
}

const CvArtInfoMovie* CvBuildingInfo::getMovieInfo() const
{
	const TCHAR* pcTag = getMovieDefineTag();
	if (pcTag != NULL && _tcscmp(pcTag, "NONE") != 0)
		return ARTFILEMGR.getMovieArtInfo(pcTag);
	
	return NULL;
}

const TCHAR* CvBuildingInfo::getMovie() const
{
	const CvArtInfoMovie* pArt;
	pArt = getMovieInfo();
	if (pArt != NULL)
		return pArt->getPath();

	return NULL;
}
// < Doto-Building Resource Converter Start >

bool CvBuildingInfo::isRequiredInputBonus(int iBonus) const
{
	FAssertMsg(iBonus < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(iBonus > -1, "Index out of bounds");

	return (getRequiredInputBonusValue(iBonus) > 0);
}

int CvBuildingInfo::getRequiredInputBonusValue(int iBonus) const
{
	FAssertMsg(iBonus < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(iBonus > -1, "Index out of bounds");
	return m_paiRequiredInputBonuses ?  m_paiRequiredInputBonuses[iBonus] : -1;
}

int CvBuildingInfo::getRequiredInputBonusCount() const
{
	int requiredInputBonusCount = 0;
	int iI;

	for(iI=0; iI < GC.getNumBonusInfos() ; iI++)
	{
		if(getRequiredInputBonusValue(iI) > 0)
		{
			requiredInputBonusCount += 1;
		}
	}

	return requiredInputBonusCount;
}

bool CvBuildingInfo::isBuildingOutputBonus(int iBonus) const
{
	FAssertMsg(iBonus < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(iBonus > -1, "Index out of bounds");

	return (getBuildingOutputBonusValues(iBonus) > 0);
}

int CvBuildingInfo::getBuildingOutputBonusValues(int iBonus) const
{
	FAssertMsg(iBonus < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(iBonus > -1, "Index out of bounds");
	return m_paiBuildingOutputBonuses ? m_paiBuildingOutputBonuses[iBonus] : -1;
}

int CvBuildingInfo::getBuildingOutputBonusCount() const
{
	int buildingOutputBonusCount = 0;
	int iI;

	for(iI=0; iI < GC.getNumBonusInfos() ; iI++)
	{
		if(getBuildingOutputBonusValues(iI) > 0)
		{
			buildingOutputBonusCount += 1;
		}
	}

	return buildingOutputBonusCount;
}
// < Doto-Building Resource Converter End   >

/************************************************************************************************/
/* Doto-City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
/*moved to the .h file - keldath
int CvBuildingInfo::getNumCitySizeBldPrereq() const
{
	return m_iNumCitySizeBldPrereq;
}
*/
/************************************************************************************************/
/* Doto-City Size Prerequisite                  END                                                  */
/************************************************************************************************/
/* Doto-Population Limit ModComp - Beginning */
/*moved to .h - keldath
int CvBuildingInfo::getPopulationLimitChange() const
{
	return m_iPopulationLimitChange;
}
*/
/* Doto-Population Limit ModComp - End */

/********************************************************************************/
/**		Doto-REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanConstruct															*/
/********************************************************************************/
/* moved to .h file - keldath
int CvBuildingInfo::getPrereqGameOption() const					
{
	return m_iPrereqGameOption;
}

int CvBuildingInfo::getNotGameOption() const			
{
	return m_iNotGameOption;
}
*/
/********************************************************************************/
/**		Doto-REVDCM									END								*/
/********************************************************************************/

//Doto-Shqype Vicinity Bonus Start
/*int CvBuildingInfo::getPrereqVicinityBonus() const		
{
	return m_iPrereqVicinityBonus;
}*/
//Doto-Shqype Vicinity Bonus End
/*** Doto-HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave START ***/
/*moved to .h file - kedlath
int CvBuildingInfo::getFreeSpecificTech() const
{
	return m_iFreeSpecificTech;
}
*/
/*** Doto-HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave END ***/
//Doto-Shqype Vicinity Bonus Start
//int CvBuildingInfo::getPrereqOrVicinityBonuses(int i) const		
//{
//	FAssertMsg(i < GC.getNUM_BUILDING_PREREQ_OR_BONUSES(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	return m_piPrereqOrVicinityBonuses ? m_piPrereqOrVicinityBonuses[i] : -1;
//}
//Doto-Shqype Vicinity Bonus End

// Doto-davidlallen: building bonus yield, commerce start
int CvBuildingInfo::getBonusConsumed() const {
	return m_iBonusConsumed;
}

int CvBuildingInfo::getCommerceProduced(int i) const {
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiCommerceProduced ? m_paiCommerceProduced[i] : 0;
}

int CvBuildingInfo::getYieldProduced(int i) const {
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiYieldProduced ? m_paiYieldProduced[i] : 0;
}
// Doto-davidlallen: building bonus yield, commerce end

// advc.008e:
bool CvBuildingInfo::nameNeedsArticle() const
{
	if(!isWorldWonder())
		return false; // Should only be called for wonders really
	CvWString szKey = getTextKeyWide();
	CvWString szText = gDLL->getText(szKey + L"_NA");
	/*  If an _NA key exists, then gDLL will return a dot. If it doesn't, then
		an article should be used. */
	return (szText.compare(L".") != 0);
}
#if SERIALIZE_CVINFOS
void CvBuildingInfo::read(FDataStreamBase* stream)
{
	CvHotkeyInfo::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);

	stream->Read(&m_iBuildingClassType);
	stream->Read(&m_iVictoryPrereq);
	stream->Read(&m_iFreeStartEra);
	stream->Read(&m_iMaxStartEra);
	stream->Read(&m_iObsoleteTech);
	stream->Read(&m_iPrereqAndTech);
	stream->Read(&m_iNoBonus);
	stream->Read(&m_iPowerBonus);
	stream->Read(&m_iFreeBonus);
	stream->Read(&m_iNumFreeBonuses);
//Doto-prereqMust+tholish
	stream->Read(&m_iPrereqMustAll);
// < Doto-Building Resource Converter Start >	
	SAFE_DELETE_ARRAY(m_paiRequiredInputBonuses);
	m_paiRequiredInputBonuses = new int[GC.getNumBonusInfos()];
	stream->Read(GC.getNumBonusInfos(), m_paiRequiredInputBonuses);
	
	SAFE_DELETE_ARRAY(m_paiBuildingOutputBonuses);
	m_paiBuildingOutputBonuses = new int[GC.getNumBonusInfos()];
	stream->Read(GC.getNumBonusInfos(), m_paiBuildingOutputBonuses);
// < Doto-Building Resource Converter End   >
	stream->Read(&m_iFreeBuildingClass);
	stream->Read(&m_iFreePromotion);
	stream->Read(&m_iCivicOption);
	stream->Read(&m_iAIWeight);
	stream->Read(&m_iProductionCost);
	stream->Read(&m_iHurryCostModifier);
	stream->Read(&m_iHurryAngerModifier);
	stream->Read(&m_iAdvancedStartCost);
	stream->Read(&m_iAdvancedStartCostIncrease);
	stream->Read(&m_iMinAreaSize);
	stream->Read(&m_iNumCitiesPrereq);
/************************************************************************************************/
/* Doto-City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	stream->Read(&m_iNumCitySizeBldPrereq);
/************************************************************************************************/
/* Doto-City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	stream->Read(&m_iNumTeamsPrereq);
	stream->Read(&m_iUnitLevelPrereq);
	stream->Read(&m_iMinLatitude);
	stream->Read(&m_iMaxLatitude);
	stream->Read(&m_iGreatPeopleRateModifier);
	stream->Read(&m_iGreatGeneralRateModifier);
	stream->Read(&m_iDomesticGreatGeneralRateModifier);
	stream->Read(&m_iGlobalGreatPeopleRateModifier);
	stream->Read(&m_iAnarchyModifier);
	stream->Read(&m_iGoldenAgeModifier);
	stream->Read(&m_iGlobalHurryModifier);
	stream->Read(&m_iFreeExperience);
	stream->Read(&m_iGlobalFreeExperience);
	/* Doto-Population Limit ModComp - Beginning */
	stream->Read(&m_iPopulationLimitChange);
	/* Doto-Population Limit ModComp - End */
	stream->Read(&m_iFoodKept);
	stream->Read(&m_iAirlift);
	stream->Read(&m_iAirModifier);
	stream->Read(&m_iAirUnitCapacity);
	stream->Read(&m_iNukeModifier);
	stream->Read(&m_iNukeExplosionRand);
	stream->Read(&m_iFreeSpecialist);
	stream->Read(&m_iAreaFreeSpecialist);
	stream->Read(&m_iGlobalFreeSpecialist);
	stream->Read(&m_iHappiness);
	stream->Read(&m_iAreaHappiness);
	stream->Read(&m_iGlobalHappiness);
	stream->Read(&m_iStateReligionHappiness);
	stream->Read(&m_iWorkerSpeedModifier);
	stream->Read(&m_iMilitaryProductionModifier);
	stream->Read(&m_iSpaceProductionModifier);
	stream->Read(&m_iGlobalSpaceProductionModifier);
	stream->Read(&m_iTradeRoutes);
	stream->Read(&m_iCoastalTradeRoutes);
	stream->Read(&m_iAreaTradeRoutes); // advc.310
	stream->Read(&m_iTradeRouteModifier);
	stream->Read(&m_iForeignTradeRouteModifier);
	stream->Read(&m_iAssetValue);
	stream->Read(&m_iPowerValue);
	stream->Read(&m_iSpecialBuildingType);
	stream->Read(&m_iAdvisorType);
/********************************************************************************/
/**		Doto-REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanConstruct															*/
/********************************************************************************/
	stream->Read(&m_iPrereqGameOption);
	stream->Read(&m_iNotGameOption);
/********************************************************************************/
/**		Doto-REVDCM									END								*/
/********************************************************************************/
	stream->Read(&m_iHolyCity);
	stream->Read(&m_iReligionType);
	stream->Read(&m_iStateReligion);
	stream->Read(&m_iPrereqReligion);
	stream->Read(&m_iPrereqCorporation);
	stream->Read(&m_iFoundsCorporation);
	stream->Read(&m_iGlobalReligionCommerce);
	stream->Read(&m_iGlobalCorporationCommerce);
	stream->Read(&m_iPrereqAndBonus);
//Doto-Shqype Vicinity Bonus Add
//	stream->Read(&m_iPrereqVicinityBonus);  
	stream->Read(&m_iGreatPeopleUnitClass);
	stream->Read(&m_iGreatPeopleRateChange);
	stream->Read(&m_iConquestProbability);
	stream->Read(&m_iMaintenanceModifier);
	//Doto-DPII < Maintenance Modifier >
	stream->Read(&m_iGlobalMaintenanceModifier);
	stream->Read(&m_iAreaMaintenanceModifier);
	stream->Read(&m_iOtherAreaMaintenanceModifier);//keldath fix?
	stream->Read(&m_iDistanceMaintenanceModifier);
	stream->Read(&m_iNumCitiesMaintenanceModifier);
	stream->Read(&m_iDistanceMaintenanceModifier);
	stream->Read(&m_iConnectedCityMaintenanceModifier);
	//Doto-DPII < Maintenance Modifier >
	stream->Read(&m_iWarWearinessModifier);
	stream->Read(&m_iGlobalWarWearinessModifier);
	stream->Read(&m_iEnemyWarWearinessModifier);
	stream->Read(&m_iHealRateChange);
	stream->Read(&m_iHealth);
	stream->Read(&m_iAreaHealth);
	stream->Read(&m_iGlobalHealth);
	stream->Read(&m_iGlobalPopulationChange);
	stream->Read(&m_iFreeTechs);
/*** Doto-HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave START ***/
	stream->Read(&m_iFreeSpecificTech);
/*** Doto-HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave END ***/
	stream->Read(&m_iDefenseModifier);
	stream->Read(&m_iBombardDefenseModifier);
	stream->Read(&m_iAllCityDefenseModifier);
	stream->Read(&m_iEspionageDefenseModifier);
	stream->Read(&m_iMissionType);
	stream->Read(&m_iVoteSourceType);
	stream->Read(&m_fVisibilityPriority);
	stream->Read(&m_bTeamShare);
	stream->Read(&m_bWater);
	stream->Read(&m_bRiver);
	stream->Read(&m_bPower);
	stream->Read(&m_bDirtyPower);
	stream->Read(&m_bAreaCleanPower);
	stream->Read(&m_bAreaBorderObstacle);
	stream->Read(&m_bConditional); // advc.310
	stream->Read(&m_bForceTeamVoteEligible);
	stream->Read(&m_bCapital);
	stream->Read(&m_bGovernmentCenter);
	stream->Read(&m_bGoldenAge);
	stream->Read(&m_bMapCentering);
	stream->Read(&m_bNoUnhappiness);
	//stream->Read(&m_bNoUnhealthyPopulation);
	stream->Read(&m_iUnhealthyPopulationModifier); // K-Mod
	stream->Read(&m_bBuildingOnlyHealthy);
	stream->Read(&m_bNeverCapture);
	stream->Read(&m_bNukeImmune);
	stream->Read(&m_bPrereqReligion);
	stream->Read(&m_bCenterInCity);
	stream->Read(&m_bStateReligion);
	stream->Read(&m_bAllowsNukes);
	stream->ReadString(m_szConstructSound);
	stream->ReadString(m_szArtDefineTag);
	stream->ReadString(m_szMovieDefineTag);
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	m_piPrereqAndTechs = new int[GC.getNUM_BUILDING_AND_TECH_PREREQS()];
	stream->Read(GC.getNUM_BUILDING_AND_TECH_PREREQS(), m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_piPrereqOrBonuses);
	m_piPrereqOrBonuses = new int[GC.getNUM_BUILDING_PREREQ_OR_BONUSES()];
	stream->Read(GC.getNUM_BUILDING_PREREQ_OR_BONUSES(), m_piPrereqOrBonuses);
//Doto-Shqype Vicinity Bonus Start
//	SAFE_DELETE_ARRAY(m_piPrereqOrVicinityBonuses);
//	m_piPrereqOrVicinityBonuses = new int[GC.getNUM_BUILDING_PREREQ_OR_BONUSES()];
//	stream->Read(GC.getNUM_BUILDING_PREREQ_OR_BONUSES(), m_piPrereqOrVicinityBonuses);
//Doto-Shqype Vicinity Bonus End
	SAFE_DELETE_ARRAY(m_piProductionTraits);
	m_piProductionTraits = new int[GC.getNumTraitInfos()];
	stream->Read(GC.getNumTraitInfos(), m_piProductionTraits);
	SAFE_DELETE_ARRAY(m_piHappinessTraits);
	m_piHappinessTraits = new int[GC.getNumTraitInfos()];
	stream->Read(GC.getNumTraitInfos(), m_piHappinessTraits);
	SAFE_DELETE_ARRAY(m_piSeaPlotYieldChange);
	m_piSeaPlotYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_piSeaPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piRiverPlotYieldChange);
	m_piRiverPlotYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_piRiverPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piGlobalSeaPlotYieldChange);
	m_piGlobalSeaPlotYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_piGlobalSeaPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	m_piYieldChange = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldModifier);
	m_piYieldModifier = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piPowerYieldModifier);
	m_piPowerYieldModifier = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_piPowerYieldModifier);
	SAFE_DELETE_ARRAY(m_piAreaYieldModifier);
	m_piAreaYieldModifier = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_piAreaYieldModifier);
	SAFE_DELETE_ARRAY(m_piGlobalYieldModifier);
	m_piGlobalYieldModifier = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_piGlobalYieldModifier);
	SAFE_DELETE_ARRAY(m_piCommerceChange);
	m_piCommerceChange = new int[NUM_COMMERCE_TYPES];
	stream->Read(NUM_COMMERCE_TYPES, m_piCommerceChange);
	SAFE_DELETE_ARRAY(m_piObsoleteSafeCommerceChange);
	m_piObsoleteSafeCommerceChange = new int[NUM_COMMERCE_TYPES];
	stream->Read(NUM_COMMERCE_TYPES, m_piObsoleteSafeCommerceChange);
	SAFE_DELETE_ARRAY(m_piCommerceChangeDoubleTime);
	m_piCommerceChangeDoubleTime = new int[NUM_COMMERCE_TYPES];
	stream->Read(NUM_COMMERCE_TYPES, m_piCommerceChangeDoubleTime);
	SAFE_DELETE_ARRAY(m_piCommerceModifier);
	m_piCommerceModifier = new int[NUM_COMMERCE_TYPES];
	stream->Read(NUM_COMMERCE_TYPES, m_piCommerceModifier);
	SAFE_DELETE_ARRAY(m_piGlobalCommerceModifier);
	m_piGlobalCommerceModifier = new int[NUM_COMMERCE_TYPES];
	stream->Read(NUM_COMMERCE_TYPES, m_piGlobalCommerceModifier);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerce);
	m_piSpecialistExtraCommerce = new int[NUM_COMMERCE_TYPES];
	stream->Read(NUM_COMMERCE_TYPES, m_piSpecialistExtraCommerce);
	SAFE_DELETE_ARRAY(m_piStateReligionCommerce);
	m_piStateReligionCommerce = new int[NUM_COMMERCE_TYPES];
	stream->Read(NUM_COMMERCE_TYPES, m_piStateReligionCommerce);
	SAFE_DELETE_ARRAY(m_piCommerceHappiness);
	m_piCommerceHappiness = new int[NUM_COMMERCE_TYPES];
	stream->Read(NUM_COMMERCE_TYPES, m_piCommerceHappiness);
	SAFE_DELETE_ARRAY(m_piReligionChange);
	m_piReligionChange = new int[GC.getNumReligionInfos()];
	stream->Read(GC.getNumReligionInfos(), m_piReligionChange);
	SAFE_DELETE_ARRAY(m_piSpecialistCount);
	m_piSpecialistCount = new int[GC.getNumSpecialistInfos()];
	stream->Read(GC.getNumSpecialistInfos(), m_piSpecialistCount);
	SAFE_DELETE_ARRAY(m_piFreeSpecialistCount);
	m_piFreeSpecialistCount = new int[GC.getNumSpecialistInfos()];
	stream->Read(GC.getNumSpecialistInfos(), m_piFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_piBonusHealthChanges);
	m_piBonusHealthChanges = new int[GC.getNumBonusInfos()];
	stream->Read(GC.getNumBonusInfos(), m_piBonusHealthChanges);
	SAFE_DELETE_ARRAY(m_piBonusHappinessChanges);
	m_piBonusHappinessChanges = new int[GC.getNumBonusInfos()];
	stream->Read(GC.getNumBonusInfos(), m_piBonusHappinessChanges);
	SAFE_DELETE_ARRAY(m_piBonusProductionModifier);
	m_piBonusProductionModifier = new int[GC.getNumBonusInfos()];
	stream->Read(GC.getNumBonusInfos(), m_piBonusProductionModifier);
	SAFE_DELETE_ARRAY(m_piUnitCombatFreeExperience);
	m_piUnitCombatFreeExperience = new int[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_piUnitCombatFreeExperience);
	SAFE_DELETE_ARRAY(m_piDomainFreeExperience);
	m_piDomainFreeExperience = new int[NUM_DOMAIN_TYPES];
	stream->Read(NUM_DOMAIN_TYPES, m_piDomainFreeExperience);
	SAFE_DELETE_ARRAY(m_piDomainProductionModifier);
	m_piDomainProductionModifier = new int[NUM_DOMAIN_TYPES];
	stream->Read(NUM_DOMAIN_TYPES, m_piDomainProductionModifier);
	SAFE_DELETE_ARRAY(m_piBuildingHappinessChanges);
	m_piBuildingHappinessChanges = new int[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_piBuildingHappinessChanges);
	SAFE_DELETE_ARRAY(m_piPrereqNumOfBuildingClass);
	m_piPrereqNumOfBuildingClass = new int[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_piPrereqNumOfBuildingClass);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	m_piFlavorValue = new int[GC.getNumFlavorTypes()];
	stream->Read(GC.getNumFlavorTypes(), m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piImprovementFreeSpecialist);
	m_piImprovementFreeSpecialist = new int[GC.getNumImprovementInfos()];
	stream->Read(GC.getNumImprovementInfos(), m_piImprovementFreeSpecialist);
	SAFE_DELETE_ARRAY(m_pbCommerceFlexible);
	m_pbCommerceFlexible = new bool[NUM_COMMERCE_TYPES];
	stream->Read(NUM_COMMERCE_TYPES, m_pbCommerceFlexible);
	SAFE_DELETE_ARRAY(m_pbCommerceChangeOriginalOwner);
	m_pbCommerceChangeOriginalOwner = new bool[NUM_COMMERCE_TYPES];
	stream->Read(NUM_COMMERCE_TYPES, m_pbCommerceChangeOriginalOwner);
	SAFE_DELETE_ARRAY(m_pbBuildingClassNeededInCity);
	m_pbBuildingClassNeededInCity = new bool[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_pbBuildingClassNeededInCity);
	if (m_ppaiSpecialistYieldChange != NULL)
	{
		for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
			SAFE_DELETE_ARRAY(m_ppaiSpecialistYieldChange[i]);
		SAFE_DELETE_ARRAY(m_ppaiSpecialistYieldChange);
	}
	m_ppaiSpecialistYieldChange = new int*[GC.getNumSpecialistInfos()];
	for(int i = 0;i < GC.getNumSpecialistInfos(); i++)
	{
		m_ppaiSpecialistYieldChange[i]  = new int[NUM_YIELD_TYPES];
		stream->Read(NUM_YIELD_TYPES, m_ppaiSpecialistYieldChange[i]);
	} // UNOFFICIAL_PATCH, Efficiency, 06/27/10, Afforess & jdog5000: START
	m_bAnySpecialistYieldChange = false;
	for(int i = 0; !m_bAnySpecialistYieldChange && i < GC.getNumSpecialistInfos(); i++)
	{
		for(int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if (m_ppaiSpecialistYieldChange[i][j] != 0)
			{
				m_bAnySpecialistYieldChange = true;
				break;
			}
		}
	} // UNOFFICIAL_PATCH: END
	if (m_ppaiBonusYieldModifier != NULL)
	{
		for(int i = 0; i < GC.getNumBonusInfos(); i++)
			SAFE_DELETE_ARRAY(m_ppaiBonusYieldModifier[i]);
		SAFE_DELETE_ARRAY(m_ppaiBonusYieldModifier);
	}
	m_ppaiBonusYieldModifier = new int*[GC.getNumBonusInfos()];
	for(int i = 0; i < GC.getNumBonusInfos(); i++)
	{
		m_ppaiBonusYieldModifier[i]  = new int[NUM_YIELD_TYPES];
		stream->Read(NUM_YIELD_TYPES, m_ppaiBonusYieldModifier[i]);
	} 
// Doto-davidlallen: building bonus yield, commerce start
	stream->Read(&m_iBonusConsumed);
	SAFE_DELETE_ARRAY(m_paiCommerceProduced);
	m_paiCommerceProduced = new int[NUM_COMMERCE_TYPES];
	stream->Read(NUM_COMMERCE_TYPES, m_paiCommerceProduced);
	SAFE_DELETE_ARRAY(m_paiYieldProduced);
	m_paiYieldProduced = new int[NUM_YIELD_TYPES];
	stream->Read(NUM_YIELD_TYPES, m_paiYieldProduced);
// Doto-davidlallen: building bonus yield, commerce end
	// UNOFFICIAL_PATCH, Efficiency, 06/27/10, Afforess & jdog5000: START
	m_bAnyBonusYieldModifier = false;
	for(int i = 0; !m_bAnyBonusYieldModifier && i < GC.getNumBonusInfos(); i++)
	{
		for(int j=0; j < NUM_YIELD_TYPES; j++)
		{
			if (m_ppaiBonusYieldModifier[i][j] != 0)
			{
				m_bAnyBonusYieldModifier = true;
				break;
			}
		}
	} // UNOFFICIAL_PATCH: END
}

void CvBuildingInfo::write(FDataStreamBase* stream)
{
	CvHotkeyInfo::write(stream);
	uint uiFlag=0;
	stream->Write(uiFlag);

	stream->Write(m_iBuildingClassType);
	stream->Write(m_iVictoryPrereq);
	stream->Write(m_iFreeStartEra);
	stream->Write(m_iMaxStartEra);
	stream->Write(m_iObsoleteTech);
	stream->Write(m_iPrereqAndTech);
	stream->Write(m_iNoBonus);
	stream->Write(m_iPowerBonus);
	stream->Write(m_iFreeBonus);
	stream->Write(m_iNumFreeBonuses);
//Doto-prereqMust+tholish
	stream->Write(m_iPrereqMustAll);
	// < Doto-Building Resource Converter Start >	
	stream->Write(GC.getNumBonusInfos(), m_paiRequiredInputBonuses);
	stream->Write(GC.getNumBonusInfos(), m_paiBuildingOutputBonuses);
	// < Doto-Building Resource Converter End   >
	stream->Write(m_iFreeBuildingClass);
	stream->Write(m_iFreePromotion);
	stream->Write(m_iCivicOption);
	stream->Write(m_iAIWeight);
	stream->Write(m_iProductionCost);
	stream->Write(m_iHurryCostModifier);
	stream->Write(m_iHurryAngerModifier);
	stream->Write(m_iAdvancedStartCost);
	stream->Write(m_iAdvancedStartCostIncrease);
	stream->Write(m_iMinAreaSize);
	stream->Write(m_iNumCitiesPrereq);
/************************************************************************************************/
/* Doto-City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	stream->Write(m_iNumCitySizeBldPrereq);
/************************************************************************************************/
/* Doto-City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	stream->Write(m_iNumTeamsPrereq);
	stream->Write(m_iUnitLevelPrereq);
	stream->Write(m_iMinLatitude);
	stream->Write(m_iMaxLatitude);
	stream->Write(m_iGreatPeopleRateModifier);
	stream->Write(m_iGreatGeneralRateModifier);
	stream->Write(m_iDomesticGreatGeneralRateModifier);
	stream->Write(m_iGlobalGreatPeopleRateModifier);
	stream->Write(m_iAnarchyModifier);
	stream->Write(m_iGoldenAgeModifier);
	stream->Write(m_iGlobalHurryModifier);
	stream->Write(m_iFreeExperience);
	stream->Write(m_iGlobalFreeExperience);
	/* Doto-Population Limit ModComp - Beginning */
	stream->Write(m_iPopulationLimitChange);
	/* Doto-Population Limit ModComp - End */
	stream->Write(m_iFoodKept);
	stream->Write(m_iAirlift);
	stream->Write(m_iAirModifier);
	stream->Write(m_iAirUnitCapacity);
	stream->Write(m_iNukeModifier);
	stream->Write(m_iNukeExplosionRand);
	stream->Write(m_iFreeSpecialist);
	stream->Write(m_iAreaFreeSpecialist);
	stream->Write(m_iGlobalFreeSpecialist);
	stream->Write(m_iHappiness);
	stream->Write(m_iAreaHappiness);
	stream->Write(m_iGlobalHappiness);
	stream->Write(m_iStateReligionHappiness);
	stream->Write(m_iWorkerSpeedModifier);
	stream->Write(m_iMilitaryProductionModifier);
	stream->Write(m_iSpaceProductionModifier);
	stream->Write(m_iGlobalSpaceProductionModifier);
	stream->Write(m_iTradeRoutes);
	stream->Write(m_iCoastalTradeRoutes);
	stream->Write(m_iAreaTradeRoutes); // advc.310
	stream->Write(m_iTradeRouteModifier);
	stream->Write(m_iForeignTradeRouteModifier);
	stream->Write(m_iAssetValue);
	stream->Write(m_iPowerValue);
	stream->Write(m_iSpecialBuildingType);
	stream->Write(m_iAdvisorType);
/********************************************************************************/
/**		Doto-REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanConstruct															*/
/********************************************************************************/
	stream->Write(m_iPrereqGameOption);
	stream->Write(m_iNotGameOption);
/********************************************************************************/
/**		Doto-REVDCM									END								*/
/********************************************************************************/
	stream->Write(m_iHolyCity);
	stream->Write(m_iReligionType);
	stream->Write(m_iStateReligion);
	stream->Write(m_iPrereqReligion);
	stream->Write(m_iPrereqCorporation);
	stream->Write(m_iFoundsCorporation);
	stream->Write(m_iGlobalReligionCommerce);
	stream->Write(m_iGlobalCorporationCommerce);
	stream->Write(m_iPrereqAndBonus);
//Doto-Shqype Vicinity Bonus Add
//	stream->Write(m_iPrereqVicinityBonus);  
	stream->Write(m_iGreatPeopleUnitClass);
	stream->Write(m_iGreatPeopleRateChange);
	stream->Write(m_iConquestProbability);
	stream->Write(m_iMaintenanceModifier);
	//Doto-DPII < Maintenance Modifier >
	stream->Write(m_iGlobalMaintenanceModifier);
	stream->Write(m_iAreaMaintenanceModifier);
	stream->Write(m_iOtherAreaMaintenanceModifier);
	stream->Write(m_iDistanceMaintenanceModifier);
	stream->Write(m_iNumCitiesMaintenanceModifier);
	stream->Write(m_iCoastalDistanceMaintenanceModifier);
	stream->Write(m_iConnectedCityMaintenanceModifier);
	//Doto-DPII < Maintenance Modifier >
	stream->Write(m_iWarWearinessModifier);
	stream->Write(m_iGlobalWarWearinessModifier);
	stream->Write(m_iEnemyWarWearinessModifier);
	stream->Write(m_iHealRateChange);
	stream->Write(m_iHealth);
	stream->Write(m_iAreaHealth);
	stream->Write(m_iGlobalHealth);
	stream->Write(m_iGlobalPopulationChange);
	stream->Write(m_iFreeTechs);
	/*** Doto-HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave START ***/
	stream->Write(m_iFreeSpecificTech);
	/*** Doto-HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave END ***/
	stream->Write(m_iDefenseModifier);
	stream->Write(m_iBombardDefenseModifier);
	stream->Write(m_iAllCityDefenseModifier);
	stream->Write(m_iEspionageDefenseModifier);
	stream->Write(m_iMissionType);
	stream->Write(m_iVoteSourceType);

	stream->Write(m_fVisibilityPriority);

	stream->Write(m_bTeamShare);
	stream->Write(m_bWater);
	stream->Write(m_bRiver);
	stream->Write(m_bPower);
	stream->Write(m_bDirtyPower);
	stream->Write(m_bAreaCleanPower);
	stream->Write(m_bAreaBorderObstacle);
	stream->Write(m_bConditional); // advc.310
	stream->Write(m_bForceTeamVoteEligible);
	stream->Write(m_bCapital);
	stream->Write(m_bGovernmentCenter);
	stream->Write(m_bGoldenAge);
	stream->Write(m_bMapCentering);
	stream->Write(m_bNoUnhappiness);
	//stream->Write(m_bNoUnhealthyPopulation);
	stream->Write(m_iUnhealthyPopulationModifier); // K-Mod
	stream->Write(m_bBuildingOnlyHealthy);
	stream->Write(m_bNeverCapture);
	stream->Write(m_bNukeImmune);
	stream->Write(m_bPrereqReligion);
	stream->Write(m_bCenterInCity);
	stream->Write(m_bStateReligion);
	stream->Write(m_bAllowsNukes);
	stream->WriteString(m_szConstructSound);
	stream->WriteString(m_szArtDefineTag);
	stream->WriteString(m_szMovieDefineTag);
	stream->Write(GC.getNUM_BUILDING_AND_TECH_PREREQS(), m_piPrereqAndTechs);
	stream->Write(GC.getNUM_BUILDING_PREREQ_OR_BONUSES(), m_piPrereqOrBonuses);
//Doto-Shqype Vicinity Bonus Add
	//	stream->Write(GC.getNUM_BUILDING_PREREQ_OR_BONUSES(), m_piPrereqOrVicinityBonuses);  
	stream->Write(GC.getNumTraitInfos(), m_piProductionTraits);
	stream->Write(GC.getNumTraitInfos(), m_piHappinessTraits);
	stream->Write(NUM_YIELD_TYPES, m_piSeaPlotYieldChange);
	stream->Write(NUM_YIELD_TYPES, m_piRiverPlotYieldChange);
	stream->Write(NUM_YIELD_TYPES, m_piGlobalSeaPlotYieldChange);
	stream->Write(NUM_YIELD_TYPES, m_piYieldChange);
	stream->Write(NUM_YIELD_TYPES, m_piYieldModifier);
	stream->Write(NUM_YIELD_TYPES, m_piPowerYieldModifier);
	stream->Write(NUM_YIELD_TYPES, m_piAreaYieldModifier);
	stream->Write(NUM_YIELD_TYPES, m_piGlobalYieldModifier);
	stream->Write(NUM_COMMERCE_TYPES, m_piCommerceChange);
	stream->Write(NUM_COMMERCE_TYPES, m_piObsoleteSafeCommerceChange);
	stream->Write(NUM_COMMERCE_TYPES, m_piCommerceChangeDoubleTime);
	stream->Write(NUM_COMMERCE_TYPES, m_piCommerceModifier);
	stream->Write(NUM_COMMERCE_TYPES, m_piGlobalCommerceModifier);
	stream->Write(NUM_COMMERCE_TYPES, m_piSpecialistExtraCommerce);
	stream->Write(NUM_COMMERCE_TYPES, m_piStateReligionCommerce);
	stream->Write(NUM_COMMERCE_TYPES, m_piCommerceHappiness);
	stream->Write(GC.getNumReligionInfos(), m_piReligionChange);
	stream->Write(GC.getNumSpecialistInfos(), m_piSpecialistCount);
	stream->Write(GC.getNumSpecialistInfos(), m_piFreeSpecialistCount);
	stream->Write(GC.getNumBonusInfos(), m_piBonusHealthChanges);
	stream->Write(GC.getNumBonusInfos(), m_piBonusHappinessChanges);
	stream->Write(GC.getNumBonusInfos(), m_piBonusProductionModifier);
	stream->Write(GC.getNumUnitCombatInfos(), m_piUnitCombatFreeExperience);
	stream->Write(NUM_DOMAIN_TYPES, m_piDomainFreeExperience);
	stream->Write(NUM_DOMAIN_TYPES, m_piDomainProductionModifier);
	stream->Write(GC.getNumBuildingClassInfos(), m_piBuildingHappinessChanges);
	stream->Write(GC.getNumBuildingClassInfos(), m_piPrereqNumOfBuildingClass);
	stream->Write(GC.getNumFlavorTypes(), m_piFlavorValue);
	stream->Write(GC.getNumImprovementInfos(), m_piImprovementFreeSpecialist);
	stream->Write(NUM_COMMERCE_TYPES, m_pbCommerceFlexible);
	stream->Write(NUM_COMMERCE_TYPES, m_pbCommerceChangeOriginalOwner);
	stream->Write(GC.getNumBuildingClassInfos(), m_pbBuildingClassNeededInCity);
	for(int i = 0 ;i < GC.getNumSpecialistInfos(); i++)
		stream->Write(NUM_YIELD_TYPES, m_ppaiSpecialistYieldChange[i]);
	for(int i = 0; i < GC.getNumBonusInfos(); i++)
		stream->Write(NUM_YIELD_TYPES, m_ppaiBonusYieldModifier[i]);
	// Doto-davidlallen: building bonus yield, commerce start
	stream->Write(m_iBonusConsumed);
	stream->Write(NUM_COMMERCE_TYPES, m_paiCommerceProduced);
	stream->Write(NUM_YIELD_TYPES, m_paiYieldProduced);
	// Doto-davidlallen: building bonus yield, commerce end
}
#endif

bool CvBuildingInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvHotkeyInfo::read(pXML))
		return false;
//Doto-code for placegolder buldingd
	std::string szType(getType());
	bool bPlaceHolder = (szType.rfind("BUILDING_PLACEHOLDER", 0) == 0);
	if(bPlaceHolder)
		//pXML->setAssertMandatory(false); // </advc.006b> //CODE FROM 095 - KELDATH ADJUSTMENT
		pXML->setAssertMandatoryEnabled(false);
	CvString szTextVal;
//keldath-qa10-the above was changed by f1rpo 097
// < Doto-Civic Infos Plus Start > //ADDED BY KELDATH	
	int j=0;						//loop counter
	int k=0;						//loop counter
	int iNumSibs=0;				// the number of siblings the current xml node has
//	int iNumChildren;				// the number of children the current node has
// < Doto-Civic Infos Plus END > //ADDED BY KELDATH
	pXML->SetInfoIDFromChildXmlVal(m_iBuildingClassType, "BuildingClass");
	pXML->SetInfoIDFromChildXmlVal(m_iSpecialBuildingType, "SpecialBuildingType");
	pXML->SetInfoIDFromChildXmlVal(m_iAdvisorType, "Advisor");
	pXML->GetChildXmlValByName(m_szArtDefineTag, "ArtDefineTag");
	pXML->GetChildXmlValByName(m_szMovieDefineTag, "MovieDefineTag");
/********************************************************************************/
/**		Doto-REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanConstruct															*/
/********************************************************************************/
	pXML->GetChildXmlValByName(szTextVal, "PrereqGameOption",	""); // f1rpo
	m_iPrereqGameOption = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "NotGameOption",""); // f1rpo
	m_iNotGameOption = pXML->FindInInfoClass(szTextVal);
/********************************************************************************/
/**		Doto-REVDCM									END								*/
/********************************************************************************/
	pXML->SetInfoIDFromChildXmlVal(m_iHolyCity, "HolyCity");
	pXML->SetInfoIDFromChildXmlVal(m_iReligionType, "ReligionType");
	pXML->SetInfoIDFromChildXmlVal(m_iStateReligion, "StateReligion");
	pXML->SetInfoIDFromChildXmlVal(m_iPrereqReligion, "PrereqReligion");
	pXML->SetInfoIDFromChildXmlVal(m_iPrereqCorporation, "PrereqCorporation");
	pXML->SetInfoIDFromChildXmlVal(m_iFoundsCorporation, "FoundsCorporation");
	pXML->SetInfoIDFromChildXmlVal(m_iGlobalReligionCommerce, "GlobalReligionCommerce");
	pXML->SetInfoIDFromChildXmlVal(m_iGlobalCorporationCommerce, "GlobalCorporationCommerce");
	pXML->SetInfoIDFromChildXmlVal(m_iVictoryPrereq, "VictoryPrereq");
	pXML->SetInfoIDFromChildXmlVal(m_iFreeStartEra, "FreeStartEra");
	pXML->SetInfoIDFromChildXmlVal(m_iMaxStartEra, "MaxStartEra");
	pXML->SetInfoIDFromChildXmlVal(m_iObsoleteTech, "ObsoleteTech");
	pXML->SetInfoIDFromChildXmlVal(m_iPrereqAndTech, "PrereqTech");

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"TechTypes"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			pXML->InitList(&m_piPrereqAndTechs, GC.getNUM_BUILDING_AND_TECH_PREREQS(), -1);
			bool bAnyReq = false; // advc.003t
			if (iNumSibs > 0)
			{
				CvString szTextVal;
				if (pXML->GetChildXmlVal(szTextVal))
				{
					FAssertMsg(iNumSibs <= GC.getNUM_BUILDING_AND_TECH_PREREQS(), "For loop iterator is greater than array size");
					for (int j = 0; j < iNumSibs; j++)
					{
						m_piPrereqAndTechs[j] = pXML->FindInInfoClass(szTextVal);
						// <advc.003t>
						if (m_piPrereqAndTechs[j] != NO_TECH)
							bAnyReq = true; // </advc.003t>
						if (!pXML->GetNextXmlVal(szTextVal))
							break;
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			} // <advc.003t>
			if (!bAnyReq)
				SAFE_DELETE_ARRAY(m_piPrereqAndTechs); // </advc.003t>
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

//Doto-Shqype Vicinity Bonus Start
/*	pXML->GetChildXmlValByName(szTextVal, "VicinityBonus");
	m_iPrereqVicinityBonus = pXML->FindInInfoClass(szTextVal);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"PrereqVicinityBonuses"))
	{
		if (pXML->SkipToNextVal())
		{
			iNumChildren = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			pXML->InitList(&m_piPrereqOrVicinityBonuses, GC.getNUM_BUILDING_PREREQ_OR_BONUSES(), -1);

			if (0 < iNumChildren)
			{
				if (pXML->GetChildXmlVal(szTextVal))
				{
					FAssertMsg((iNumChildren <= GC.getNUM_BUILDING_PREREQ_OR_BONUSES()),"For loop iterator is greater than array size");
					for (j=0;j<iNumChildren;j++)
					{
						m_piPrereqOrVicinityBonuses[j] = pXML->FindInInfoClass(szTextVal);
						if (!pXML->GetNextXmlVal(szTextVal))
						{
							break;
						}
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}*/
//Doto-Shqype Vicinity Bonus End
	pXML->SetInfoIDFromChildXmlVal(m_iPrereqAndBonus, "Bonus");

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"PrereqBonuses"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumChildren = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			pXML->InitList(&m_piPrereqOrBonuses, GC.getNUM_BUILDING_PREREQ_OR_BONUSES(), -1);
			bool bAnyReq = false; // advc.003t
			if (iNumChildren > 0)
			{
				CvString szTextVal;
				if (pXML->GetChildXmlVal(szTextVal))
				{
					FAssertMsg(iNumChildren <= GC.getNUM_BUILDING_PREREQ_OR_BONUSES(), "For loop iterator is greater than array size");
					for (int j = 0; j < iNumChildren; j++)
					{
						m_piPrereqOrBonuses[j] = pXML->FindInInfoClass(szTextVal);
						// <advc.003t>
						if (m_piPrereqOrBonuses[j] != NO_BONUS)
							bAnyReq = true; // </advc.003t>
						if (!pXML->GetNextXmlVal(szTextVal))
							break;
					}
					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			} // <advc.003t>
			if (!bAnyReq)
				SAFE_DELETE_ARRAY(m_piPrereqOrBonuses); // </advc.003t>
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	pXML->SetVariableListTagPair(&m_piProductionTraits, "ProductionTraits", GC.getNumTraitInfos());
	pXML->SetVariableListTagPair(&m_piHappinessTraits, "HappinessTraits", GC.getNumTraitInfos());

	pXML->SetInfoIDFromChildXmlVal(m_iNoBonus, "NoBonus");
	pXML->SetInfoIDFromChildXmlVal(m_iPowerBonus, "PowerBonus");
	pXML->SetInfoIDFromChildXmlVal(m_iFreeBonus, "FreeBonus");

	pXML->GetChildXmlValByName(&m_iNumFreeBonuses, "iNumFreeBonuses");
//Doto-prereqMust+tholish
	pXML->GetChildXmlValByName(&m_iPrereqMustAll, "iPrereqMustAll",0);
// < BDoto-uilding Resource Converter Start >
//keldath qa2 - done - from f1rpo - The sizeof... parameter was unused, so I've removed it.
//	pXML->SetVariableListTagPair(&m_paiRequiredInputBonuses, "RequiredInputBonuses", sizeof(GC.getBonusInfo((BonusTypes)0)), GC.getNumBonusInfos());
//	pXML->SetVariableListTagPair(&m_paiBuildingOutputBonuses, "BuildingOutputBonuses", sizeof(GC.getBonusInfo((BonusTypes)0)), GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_paiRequiredInputBonuses, "RequiredInputBonuses", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_paiBuildingOutputBonuses, "BuildingOutputBonuses", GC.getNumBonusInfos());
	// < Doto-Building Resource Converter End   >
	pXML->SetInfoIDFromChildXmlVal(m_iFreeBuildingClass, "FreeBuilding");
	pXML->SetInfoIDFromChildXmlVal(m_iFreePromotion, "FreePromotion");
	pXML->SetInfoIDFromChildXmlVal(m_iCivicOption, "CivicOption");
	pXML->SetInfoIDFromChildXmlVal(m_iGreatPeopleUnitClass, "GreatPeopleUnitClass");
	pXML->SetInfoIDFromChildXmlVal(m_iVoteSourceType, "DiploVoteType");

	pXML->GetChildXmlValByName(&m_iGreatPeopleRateChange, "iGreatPeopleRateChange");
	pXML->GetChildXmlValByName(&m_bTeamShare, "bTeamShare");
	pXML->GetChildXmlValByName(&m_bWater, "bWater");
	pXML->GetChildXmlValByName(&m_bRiver, "bRiver");
	pXML->GetChildXmlValByName(&m_bPower, "bPower");
	pXML->GetChildXmlValByName(&m_bDirtyPower, "bDirtyPower");
	pXML->GetChildXmlValByName(&m_bAreaCleanPower, "bAreaCleanPower");
	pXML->GetChildXmlValByName(&m_bAreaBorderObstacle, "bBorderObstacle");
	pXML->GetChildXmlValByName(&m_bConditional, "bConditional", false); // advc.310
	pXML->GetChildXmlValByName(&m_bForceTeamVoteEligible, "bForceTeamVoteEligible");
	pXML->GetChildXmlValByName(&m_bCapital, "bCapital");
	pXML->GetChildXmlValByName(&m_bGovernmentCenter, "bGovernmentCenter");
	pXML->GetChildXmlValByName(&m_bGoldenAge, "bGoldenAge");
	pXML->GetChildXmlValByName(&m_bAllowsNukes, "bAllowsNukes");
	pXML->GetChildXmlValByName(&m_bMapCentering, "bMapCentering");
	pXML->GetChildXmlValByName(&m_bNoUnhappiness, "bNoUnhappiness");
	//pXML->GetChildXmlValByName(&m_bNoUnhealthyPopulation, "bNoUnhealthyPopulation");
	pXML->GetChildXmlValByName(&m_iUnhealthyPopulationModifier, "iUnhealthyPopulationModifier"); // K-Mod
	pXML->GetChildXmlValByName(&m_bBuildingOnlyHealthy, "bBuildingOnlyHealthy");
	pXML->GetChildXmlValByName(&m_bNeverCapture, "bNeverCapture");
	pXML->GetChildXmlValByName(&m_bNukeImmune, "bNukeImmune");
	pXML->GetChildXmlValByName(&m_bPrereqReligion, "bPrereqReligion");
	pXML->GetChildXmlValByName(&m_bCenterInCity, "bCenterInCity");
	pXML->GetChildXmlValByName(&m_bStateReligion, "bStateReligion");
	pXML->GetChildXmlValByName(&m_iAIWeight, "iAIWeight");
	pXML->GetChildXmlValByName(&m_iProductionCost, "iCost");
	pXML->GetChildXmlValByName(&m_iHurryCostModifier, "iHurryCostModifier");
	pXML->GetChildXmlValByName(&m_iHurryAngerModifier, "iHurryAngerModifier");
	pXML->GetChildXmlValByName(&m_iAdvancedStartCost, "iAdvancedStartCost");
	pXML->GetChildXmlValByName(&m_iAdvancedStartCostIncrease, "iAdvancedStartCostIncrease");
	pXML->GetChildXmlValByName(&m_iMinAreaSize, "iMinAreaSize");
	pXML->GetChildXmlValByName(&m_iConquestProbability, "iConquestProb");
	pXML->GetChildXmlValByName(&m_iNumCitiesPrereq, "iCitiesPrereq");
/************************************************************************************************/
/* Doto-City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	pXML->GetChildXmlValByName(&m_iNumCitySizeBldPrereq, "iCitySizeBldPrereq", 0);
/************************************************************************************************/
/* Doto-City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	pXML->GetChildXmlValByName(&m_iNumTeamsPrereq, "iTeamsPrereq");
	pXML->GetChildXmlValByName(&m_iUnitLevelPrereq, "iLevelPrereq");
	pXML->GetChildXmlValByName(&m_iMinLatitude, "iMinLatitude");
	pXML->GetChildXmlValByName(&m_iMaxLatitude, "iMaxLatitude", 90);
	pXML->GetChildXmlValByName(&m_iGreatPeopleRateModifier, "iGreatPeopleRateModifier");
	pXML->GetChildXmlValByName(&m_iGreatGeneralRateModifier, "iGreatGeneralRateModifier");
	pXML->GetChildXmlValByName(&m_iDomesticGreatGeneralRateModifier, "iDomesticGreatGeneralRateModifier");
	pXML->GetChildXmlValByName(&m_iGlobalGreatPeopleRateModifier, "iGlobalGreatPeopleRateModifier");
	pXML->GetChildXmlValByName(&m_iAnarchyModifier, "iAnarchyModifier");
	pXML->GetChildXmlValByName(&m_iGoldenAgeModifier, "iGoldenAgeModifier");
	pXML->GetChildXmlValByName(&m_iGlobalHurryModifier, "iGlobalHurryModifier");
	pXML->GetChildXmlValByName(&m_iFreeExperience, "iExperience");
	pXML->GetChildXmlValByName(&m_iGlobalFreeExperience, "iGlobalExperience");
	/* Population Limit ModComp - Beginning */
	pXML->GetChildXmlValByName(&m_iPopulationLimitChange, "iPopulationLimitChange", 0);
	/* Population Limit ModComp - End */
	pXML->GetChildXmlValByName(&m_iFoodKept, "iFoodKept");
	pXML->GetChildXmlValByName(&m_iAirlift, "iAirlift");
	pXML->GetChildXmlValByName(&m_iAirModifier, "iAirModifier");
	pXML->GetChildXmlValByName(&m_iAirUnitCapacity, "iAirUnitCapacity");
	pXML->GetChildXmlValByName(&m_iNukeModifier, "iNukeModifier");
	pXML->GetChildXmlValByName(&m_iNukeExplosionRand, "iNukeExplosionRand");
	pXML->GetChildXmlValByName(&m_iFreeSpecialist, "iFreeSpecialist");
	pXML->GetChildXmlValByName(&m_iAreaFreeSpecialist, "iAreaFreeSpecialist");
	pXML->GetChildXmlValByName(&m_iGlobalFreeSpecialist, "iGlobalFreeSpecialist");
	pXML->GetChildXmlValByName(&m_iMaintenanceModifier, "iMaintenanceModifier");
	//Doto-DPII < Maintenance Modifier >
    pXML->GetChildXmlValByName(&m_iGlobalMaintenanceModifier, "iGlobalMaintenanceModifier", 0);
    pXML->GetChildXmlValByName(&m_iAreaMaintenanceModifier, "iAreaMaintenanceModifier", 0);
    pXML->GetChildXmlValByName(&m_iOtherAreaMaintenanceModifier, "iOtherAreaMaintenanceModifier", 0);
	pXML->GetChildXmlValByName(&m_iDistanceMaintenanceModifier, "iDistanceMaintenanceModifier", 0);
	pXML->GetChildXmlValByName(&m_iNumCitiesMaintenanceModifier, "iNumCitiesMaintenanceModifier", 0);
	pXML->GetChildXmlValByName(&m_iCoastalDistanceMaintenanceModifier, "iCoastalDistanceMaintenanceModifier", 0);
	pXML->GetChildXmlValByName(&m_iConnectedCityMaintenanceModifier, "iConnectedCityMaintenanceModifier", 0);
	//Doto-DPII < Maintenance Modifier >
	pXML->GetChildXmlValByName(&m_iWarWearinessModifier, "iWarWearinessModifier");
	pXML->GetChildXmlValByName(&m_iGlobalWarWearinessModifier, "iGlobalWarWearinessModifier");
	pXML->GetChildXmlValByName(&m_iEnemyWarWearinessModifier, "iEnemyWarWearinessModifier");
	pXML->GetChildXmlValByName(&m_iHealRateChange, "iHealRateChange");
	pXML->GetChildXmlValByName(&m_iHealth, "iHealth");
	pXML->GetChildXmlValByName(&m_iAreaHealth, "iAreaHealth");
	pXML->GetChildXmlValByName(&m_iGlobalHealth, "iGlobalHealth");
	pXML->GetChildXmlValByName(&m_iHappiness, "iHappiness");
	pXML->GetChildXmlValByName(&m_iAreaHappiness, "iAreaHappiness");
	pXML->GetChildXmlValByName(&m_iGlobalHappiness, "iGlobalHappiness");
	pXML->GetChildXmlValByName(&m_iStateReligionHappiness, "iStateReligionHappiness");
	pXML->GetChildXmlValByName(&m_iWorkerSpeedModifier, "iWorkerSpeedModifier");
	pXML->GetChildXmlValByName(&m_iMilitaryProductionModifier, "iMilitaryProductionModifier");
	pXML->GetChildXmlValByName(&m_iSpaceProductionModifier, "iSpaceProductionModifier");
	pXML->GetChildXmlValByName(&m_iGlobalSpaceProductionModifier, "iGlobalSpaceProductionModifier");
	pXML->GetChildXmlValByName(&m_iTradeRoutes, "iTradeRoutes");
	pXML->GetChildXmlValByName(&m_iCoastalTradeRoutes, "iCoastalTradeRoutes");
	pXML->GetChildXmlValByName(&m_iAreaTradeRoutes, "iAreaTradeRoutes"); // advc.310
	pXML->GetChildXmlValByName(&m_iTradeRouteModifier, "iTradeRouteModifier");
	pXML->GetChildXmlValByName(&m_iForeignTradeRouteModifier, "iForeignTradeRouteModifier");
	pXML->GetChildXmlValByName(&m_iGlobalPopulationChange, "iGlobalPopulationChange");
	pXML->GetChildXmlValByName(&m_iFreeTechs, "iFreeTechs");
	/*** Doto-HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave START ***/
	pXML->GetChildXmlValByName(szTextVal, "FreeSpecificTech", "NONE");
	m_iFreeSpecificTech = pXML->FindInInfoClass(szTextVal);
	/*** Doto-HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave END ***/
	pXML->GetChildXmlValByName(&m_iDefenseModifier, "iDefense");
	pXML->GetChildXmlValByName(&m_iBombardDefenseModifier, "iBombardDefense");
	pXML->GetChildXmlValByName(&m_iAllCityDefenseModifier, "iAllCityDefense");
	pXML->GetChildXmlValByName(&m_iEspionageDefenseModifier, "iEspionageDefense");
	pXML->GetChildXmlValByName(&m_iAssetValue, "iAsset");
	pXML->GetChildXmlValByName(&m_iPowerValue, "iPower");
	pXML->GetChildXmlValByName(&m_fVisibilityPriority, "fVisibilityPriority");

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "SeaPlotYieldChanges"))
	{
		pXML->SetYields(&m_piSeaPlotYieldChange);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piSeaPlotYieldChange, NUM_YIELD_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "RiverPlotYieldChanges"))
	{
		pXML->SetYields(&m_piRiverPlotYieldChange);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piRiverPlotYieldChange, NUM_YIELD_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "GlobalSeaPlotYieldChanges"))
	{
		pXML->SetYields(&m_piGlobalSeaPlotYieldChange);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piGlobalSeaPlotYieldChange, NUM_YIELD_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "YieldChanges"))
	{
		pXML->SetYields(&m_piYieldChange);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piYieldChange, NUM_YIELD_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifier);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piYieldModifier, NUM_YIELD_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "PowerYieldModifiers"))
	{
		pXML->SetYields(&m_piPowerYieldModifier);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piPowerYieldModifier, NUM_YIELD_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "AreaYieldModifiers"))
	{
		pXML->SetYields(&m_piAreaYieldModifier);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piAreaYieldModifier, NUM_YIELD_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"GlobalYieldModifiers"))
	{
		pXML->SetYields(&m_piGlobalYieldModifier);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piGlobalYieldModifier, NUM_YIELD_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"CommerceChanges"))
	{
		pXML->SetCommerce(&m_piCommerceChange);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piCommerceChange, NUM_COMMERCE_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"ObsoleteSafeCommerceChanges"))
	{
		pXML->SetCommerce(&m_piObsoleteSafeCommerceChange);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piObsoleteSafeCommerceChange, NUM_COMMERCE_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"CommerceChangeDoubleTimes"))
	{
		pXML->SetCommerce(&m_piCommerceChangeDoubleTime);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piCommerceChangeDoubleTime, NUM_COMMERCE_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifier);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piCommerceModifier, NUM_COMMERCE_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"GlobalCommerceModifiers"))
	{
		pXML->SetCommerce(&m_piGlobalCommerceModifier);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piGlobalCommerceModifier, NUM_COMMERCE_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"SpecialistExtraCommerces"))
	{
		pXML->SetCommerce(&m_piSpecialistExtraCommerce);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piSpecialistExtraCommerce, NUM_COMMERCE_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"StateReligionCommerces"))
	{
		pXML->SetCommerce(&m_piStateReligionCommerce);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piStateReligionCommerce, NUM_COMMERCE_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"CommerceHappinesses"))
	{
		pXML->SetCommerce(&m_piCommerceHappiness);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_piCommerceHappiness, NUM_COMMERCE_TYPES);

	pXML->SetVariableListTagPair(&m_piReligionChange, "ReligionChanges", GC.getNumReligionInfos());

	pXML->SetVariableListTagPair(&m_piSpecialistCount, "SpecialistCounts", GC.getNumSpecialistInfos());
	pXML->SetVariableListTagPair(&m_piFreeSpecialistCount, "FreeSpecialistCounts", GC.getNumSpecialistInfos());

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"CommerceFlexibles"))
	{
		pXML->SetCommerce(&m_pbCommerceFlexible);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_pbCommerceFlexible, NUM_COMMERCE_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"CommerceChangeOriginalOwners"))
	{
		pXML->SetCommerce(&m_pbCommerceChangeOriginalOwner);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_pbCommerceChangeOriginalOwner, NUM_COMMERCE_TYPES);

	pXML->GetChildXmlValByName(m_szConstructSound, "ConstructSound");

	pXML->SetVariableListTagPair(&m_piBonusHealthChanges, "BonusHealthChanges", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_piBonusHappinessChanges, "BonusHappinessChanges", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_piBonusProductionModifier, "BonusProductionModifiers", GC.getNumBonusInfos());

	pXML->SetVariableListTagPair(&m_piUnitCombatFreeExperience, "UnitCombatFreeExperiences", GC.getNumUnitCombatInfos());

	pXML->SetVariableListTagPair(&m_piDomainFreeExperience, "DomainFreeExperiences", NUM_DOMAIN_TYPES);
	pXML->SetVariableListTagPair(&m_piDomainProductionModifier, "DomainProductionModifiers", NUM_DOMAIN_TYPES);

	pXML->SetVariableListTagPair(&m_piPrereqNumOfBuildingClass, "PrereqBuildingClasses", GC.getNumBuildingClassInfos());
	pXML->SetVariableListTagPair(&m_pbBuildingClassNeededInCity, "BuildingClassNeededs", GC.getNumBuildingClassInfos());

	// UNOFFICIAL_PATCH, Efficiency, 06/27/10, Afforess & jdog5000: START
	m_bAnySpecialistYieldChange = false;
	pXML->Init2DIntList(&m_ppaiSpecialistYieldChange, GC.getNumSpecialistInfos(), NUM_YIELD_TYPES);
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"SpecialistYieldChanges"))
	{
		int iNumChildren = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"SpecialistYieldChange"))
		{
			for(int j = 0; j < iNumChildren; j++)
			{
				CvString szTextVal;
				pXML->GetChildXmlValByName(szTextVal, "SpecialistType");
				int k = pXML->FindInInfoClass(szTextVal);
				if (k > -1)
				{
					SAFE_DELETE_ARRAY(m_ppaiSpecialistYieldChange[k]);
					if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"YieldChanges"))
					{
						pXML->SetYields(&m_ppaiSpecialistYieldChange[k]);
						gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
					}
					else pXML->InitList(&m_ppaiSpecialistYieldChange[k], NUM_YIELD_TYPES);
				}
				if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
					break;
			}

			for(int ii = 0; !m_bAnySpecialistYieldChange && ii < GC.getNumSpecialistInfos(); ii++)
			{
				for(int ij = 0; ij < NUM_YIELD_TYPES; ij++)
				{
					if (m_ppaiSpecialistYieldChange[ii][ij] != 0)
					{
						m_bAnySpecialistYieldChange = true;
						break;
					}
				}
			}
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	m_bAnyBonusYieldModifier = false;
	pXML->Init2DIntList(&m_ppaiBonusYieldModifier, GC.getNumBonusInfos(), NUM_YIELD_TYPES);
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"BonusYieldModifiers"))
	{
		int iNumChildren = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"BonusYieldModifier"))
		{
			for(int j = 0; j < iNumChildren; j++)
			{
				CvString szTextVal;
				pXML->GetChildXmlValByName(szTextVal, "BonusType");
				int k = pXML->FindInInfoClass(szTextVal);
				if (k > -1)
				{
					SAFE_DELETE_ARRAY(m_ppaiBonusYieldModifier[k]);
					if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"YieldModifiers"))
					{
						pXML->SetYields(&m_ppaiBonusYieldModifier[k]);
						gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
					}
					else pXML->InitList(&m_ppaiBonusYieldModifier[k], NUM_YIELD_TYPES);

				}

				if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
					break;
			}

			for(int ii = 0; !m_bAnyBonusYieldModifier && ii < GC.getNumBonusInfos(); ii++)
			{
				for(int ij=0; ij < NUM_YIELD_TYPES; ij++)
				{
					if (m_ppaiBonusYieldModifier[ii][ij] != 0)
					{
						m_bAnyBonusYieldModifier = true;
						break;
					}
				}
			}
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	// UNOFFICIAL_PATCH: END

	pXML->SetVariableListTagPair(&m_piFlavorValue, "Flavors", GC.getNumFlavorTypes());
	pXML->SetVariableListTagPair(&m_piImprovementFreeSpecialist, "ImprovementFreeSpecialists", GC.getNumImprovementInfos());
	pXML->SetVariableListTagPair(&m_piBuildingHappinessChanges, "BuildingHappinessChanges", GC.getNumBuildingClassInfos());
// Doto-davidlallen: building bonus yield, commerce start
	pXML->GetChildXmlValByName(szTextVal, "BonusConsumed",
		""); // f1rpo
	m_iBonusConsumed = pXML->FindInInfoClass(szTextVal);
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "CommerceProduced"))
	{
		pXML->SetCommerce(&m_paiCommerceProduced);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else
	{
		pXML->InitList(&m_paiCommerceProduced, NUM_COMMERCE_TYPES);
	}
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"YieldProduced"))
	{
		pXML->SetYields(&m_paiYieldProduced);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else
	{
		pXML->InitList(&m_paiYieldProduced, NUM_YIELD_TYPES);
	}
// Doto-davidlallen: building bonus yield, commerce end
	// Doto-<advc.006b> and keldath
	if(bPlaceHolder)
		//pXML->setAssertMandatory(true); // </advc.006b>
		//pXML->setAssertMandatory(true); // </advc.006b> //CODE FROM 095 - KELDATH ADJUSTMENT
		pXML->setAssertMandatoryEnabled(true);
	return true;
}
// <advc.310>
bool CvBuildingInfo::m_bEnabledDomesticGreatGeneralRateModifier = true;
bool CvBuildingInfo::m_bEnabledAreaTradeRoutes = true;
bool CvBuildingInfo::m_bEnabledAreaBorderObstacle = true;
void CvBuildingInfo::setDomesticGreatGeneralRateModifierEnabled(bool b) {
	m_bEnabledDomesticGreatGeneralRateModifier = b;
}
void CvBuildingInfo::setAreaTradeRoutesEnabled(bool b) {
	m_bEnabledAreaTradeRoutes = b;
}
void CvBuildingInfo::setAreaBorderObstacleEnabled(bool b) {
	m_bEnabledAreaBorderObstacle = b;
} // </advc.310>

CvBuildingClassInfo::CvBuildingClassInfo() :
m_iMaxGlobalInstances(0),
m_iMaxTeamInstances(0),
m_iMaxPlayerInstances(0),
m_iExtraPlayerInstances(0),
m_iDefaultBuildingIndex(NO_BUILDING),
m_bNoLimit(false),
m_bMonument(false),
m_piVictoryThreshold(NULL)
{}

CvBuildingClassInfo::~CvBuildingClassInfo()
{
	SAFE_DELETE_ARRAY(m_piVictoryThreshold);
}

bool CvBuildingClassInfo::isMonument() const
{
	return m_bMonument;
}

/*  advc (comment): Unused in XML. Number of buildings of this class required for
	victory i as a necessary (not sufficient) condition. */
int CvBuildingClassInfo::getVictoryThreshold(int i) const
{
	FAssertMsg(i < GC.getNumVictoryInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piVictoryThreshold ? m_piVictoryThreshold[i] : 0; // advc.003t
}

// advc: Moved from CvGameCoreUtils, renamed from "limitedWonderClassLimit".
int CvBuildingClassInfo::getLimit() const
{
	int iCount = 0;
	bool bLimited = false;

	int iMax = getMaxGlobalInstances();
	if (iMax != -1)
	{
		iCount += iMax;
		bLimited = true;
	}

	iMax = getMaxTeamInstances();
	if (iMax != -1)
	{
		iCount += iMax;
		bLimited = true;
	}

	iMax = getMaxPlayerInstances();
	if (iMax != -1)
	{
		iCount += iMax;
		bLimited = true;
	}

	return (bLimited ? iCount : -1);
}

bool CvBuildingClassInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->GetChildXmlValByName(&m_iMaxGlobalInstances, "iMaxGlobalInstances");
	pXML->GetChildXmlValByName(&m_iMaxTeamInstances, "iMaxTeamInstances");
	pXML->GetChildXmlValByName(&m_iMaxPlayerInstances, "iMaxPlayerInstances");
	pXML->GetChildXmlValByName(&m_iExtraPlayerInstances, "iExtraPlayerInstances");

	pXML->GetChildXmlValByName(&m_bNoLimit, "bNoLimit");
	pXML->GetChildXmlValByName(&m_bMonument, "bMonument");

	pXML->SetVariableListTagPair(&m_piVictoryThreshold, "VictoryThresholds", GC.getNumVictoryInfos());

	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, "DefaultBuilding");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	return true;
}

bool CvBuildingClassInfo::readPass3()
{
	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}

	m_iDefaultBuildingIndex = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);
	m_aszExtraXMLforPass3.clear();

	return true;
}

CvSpecialBuildingInfo::CvSpecialBuildingInfo() :
m_iObsoleteTech(NO_TECH),
m_iTechPrereq(NO_TECH),
m_iTechPrereqAnyone(NO_TECH),
m_bValid(false),
m_piProductionTraits(NULL)
{}

CvSpecialBuildingInfo::~CvSpecialBuildingInfo()
{
	SAFE_DELETE_ARRAY(m_piProductionTraits);
}

int CvSpecialBuildingInfo::getObsoleteTech() const
{
	return m_iObsoleteTech;
}

int CvSpecialBuildingInfo::getTechPrereq() const
{
	return m_iTechPrereq;
}

int CvSpecialBuildingInfo::getTechPrereqAnyone() const
{
	return m_iTechPrereqAnyone;
}

bool CvSpecialBuildingInfo::isValid() const
{
	return m_bValid;
}

int CvSpecialBuildingInfo::getProductionTraits(int i) const
{
	FAssertBounds(0, GC.getNumTraitInfos(), i);
	return m_piProductionTraits ? m_piProductionTraits[i]
			: 0; // advc.003t: Was -1. This is the production discount percentage.
}

bool CvSpecialBuildingInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->SetInfoIDFromChildXmlVal(m_iObsoleteTech, "ObsoleteTech");
	pXML->SetInfoIDFromChildXmlVal(m_iTechPrereq, "TechPrereq");
	pXML->SetInfoIDFromChildXmlVal(m_iTechPrereqAnyone, "TechPrereqAnyone");

	pXML->GetChildXmlValByName(&m_bValid, "bValid");

	pXML->SetVariableListTagPair(&m_piProductionTraits, "ProductionTraits", GC.getNumTraitInfos());

	return true;
}

CvVoteSourceInfo::CvVoteSourceInfo() :
	m_iVoteInterval(0),
	m_iFreeSpecialist(NO_SPECIALIST),
	m_iCivic(NO_CIVIC),
	m_aiReligionYields(NULL),
	m_aiReligionCommerces(NULL)
{}

CvVoteSourceInfo::~CvVoteSourceInfo()
{
	SAFE_DELETE_ARRAY(m_aiReligionYields);
	SAFE_DELETE_ARRAY(m_aiReligionCommerces);
}

int CvVoteSourceInfo::getVoteInterval() const
{
	return m_iVoteInterval;
}

int CvVoteSourceInfo::getFreeSpecialist() const
{
	return m_iFreeSpecialist;
}

int CvVoteSourceInfo::getCivic() const
{
	return m_iCivic;
}

int CvVoteSourceInfo::getReligionYield(int i) const
{
	FAssert(i >= 0 && i < NUM_YIELD_TYPES);
	return m_aiReligionYields[i];
}

int CvVoteSourceInfo::getReligionCommerce(int i) const
{
	FAssert(i >= 0 && i < NUM_COMMERCE_TYPES);
	return m_aiReligionCommerces[i];
}

const CvWString CvVoteSourceInfo::getPopupText() const
{
	return gDLL->getText(m_szPopupText);
}

const CvWString CvVoteSourceInfo::getSecretaryGeneralText() const
{
	return gDLL->getText(m_szSecretaryGeneralText);
}

bool CvVoteSourceInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->GetChildXmlValByName(&m_iVoteInterval, "iVoteInterval");
	pXML->GetChildXmlValByName(m_szPopupText, "PopupText");
	pXML->GetChildXmlValByName(m_szSecretaryGeneralText, "SecretaryGeneralText");

	pXML->SetInfoIDFromChildXmlVal(m_iFreeSpecialist, "FreeSpecialist");
	{
		CvString szTextVal;
		pXML->GetChildXmlValByName(szTextVal, "Civic");
		m_aszExtraXMLforPass3.push_back(szTextVal);
	}
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"ReligionYields"))
	{
		pXML->SetCommerce(&m_aiReligionYields);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_aiReligionYields, NUM_YIELD_TYPES);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"ReligionCommerces"))
	{
		pXML->SetCommerce(&m_aiReligionCommerces);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	else pXML->InitList(&m_aiReligionCommerces, NUM_COMMERCE_TYPES);

	return true;
}

bool CvVoteSourceInfo::readPass3()
{
	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}

	m_iCivic = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);
	m_aszExtraXMLforPass3.clear();

	return true;
}

CvVoteInfo::CvVoteInfo() :
m_iPopulationThreshold(0),
m_iStateReligionVotePercent(0),
m_iTradeRoutes(0),
m_iMinVoters(0),
m_bSecretaryGeneral(false),
m_bVictory(false),
m_bFreeTrade(false),
m_bNoNukes(false),
m_bCityVoting(false),
m_bCivVoting(false),
m_bDefensivePact(false),
m_bOpenBorders(false),
m_bForcePeace(false),
m_bForceNoTrade(false),
m_bForceWar(false),
m_bAssignCity(false),
m_pbForceCivic(NULL),
m_abVoteSourceTypes(NULL)
{}

CvVoteInfo::~CvVoteInfo()
{
	SAFE_DELETE_ARRAY(m_pbForceCivic);
	SAFE_DELETE_ARRAY(m_abVoteSourceTypes);
}

int CvVoteInfo::getPopulationThreshold() const
{
	return m_iPopulationThreshold;
}

int CvVoteInfo::getStateReligionVotePercent() const
{
	return m_iStateReligionVotePercent;
}

int CvVoteInfo::getTradeRoutes() const
{
	return m_iTradeRoutes;
}

int CvVoteInfo::getMinVoters() const
{
	return m_iMinVoters;
}

bool CvVoteInfo::isSecretaryGeneral() const
{
	return m_bSecretaryGeneral;
}

bool CvVoteInfo::isVictory() const
{
	return m_bVictory;
}

bool CvVoteInfo::isFreeTrade() const
{
	return m_bFreeTrade;
}

bool CvVoteInfo::isNoNukes() const
{
	return m_bNoNukes;
}

bool CvVoteInfo::isCityVoting() const
{
	return m_bCityVoting;
}

bool CvVoteInfo::isCivVoting() const
{
	return m_bCivVoting;
}

bool CvVoteInfo::isDefensivePact() const
{
	return m_bDefensivePact;
}

bool CvVoteInfo::isOpenBorders() const
{
	return m_bOpenBorders;
}

bool CvVoteInfo::isForcePeace() const
{
	return m_bForcePeace;
}

bool CvVoteInfo::isForceNoTrade() const
{
	return m_bForceNoTrade;
}

bool CvVoteInfo::isForceWar() const
{
	return m_bForceWar;
}

bool CvVoteInfo::isAssignCity() const
{
	return m_bAssignCity;
}

bool CvVoteInfo::isForceCivic(int i) const
{
	FAssertBounds(0, GC.getNumCivicInfos(), i);
	return m_pbForceCivic ? m_pbForceCivic[i] : false;
}

bool CvVoteInfo::isVoteSourceType(int i) const
{
	FAssertBounds(0, GC.getNumVoteSourceInfos(), i);
	return m_abVoteSourceTypes ? m_abVoteSourceTypes[i] : false;
}

bool CvVoteInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->GetChildXmlValByName(&m_iPopulationThreshold, "iPopulationThreshold");
	pXML->GetChildXmlValByName(&m_iStateReligionVotePercent, "iStateReligionVotePercent");
	pXML->GetChildXmlValByName(&m_iTradeRoutes, "iTradeRoutes");
	pXML->GetChildXmlValByName(&m_iMinVoters, "iMinVoters");

	pXML->GetChildXmlValByName(&m_bSecretaryGeneral, "bSecretaryGeneral");
	pXML->GetChildXmlValByName(&m_bVictory, "bVictory");
	pXML->GetChildXmlValByName(&m_bFreeTrade, "bFreeTrade");
	pXML->GetChildXmlValByName(&m_bNoNukes, "bNoNukes");
	pXML->GetChildXmlValByName(&m_bCityVoting, "bCityVoting");
	pXML->GetChildXmlValByName(&m_bCivVoting, "bCivVoting");
	pXML->GetChildXmlValByName(&m_bDefensivePact, "bDefensivePact");
	pXML->GetChildXmlValByName(&m_bOpenBorders, "bOpenBorders");
	pXML->GetChildXmlValByName(&m_bForcePeace, "bForcePeace");
	pXML->GetChildXmlValByName(&m_bForceNoTrade, "bForceNoTrade");
	pXML->GetChildXmlValByName(&m_bForceWar, "bForceWar");
	pXML->GetChildXmlValByName(&m_bAssignCity, "bAssignCity");

	pXML->SetVariableListTagPair(&m_pbForceCivic, "ForceCivics", GC.getNumCivicInfos());
	pXML->SetVariableListTagPair(&m_abVoteSourceTypes, "DiploVotes", GC.getNumVoteSourceInfos());

	return true;
}

CvProjectInfo::CvProjectInfo() :
m_iVictoryPrereq(NO_VICTORY),
m_iTechPrereq(NO_TECH),
m_iAnyoneProjectPrereq(NO_PROJECT),
m_iMaxGlobalInstances(0),
m_iMaxTeamInstances(0),
m_iProductionCost(0),
m_iNukeInterception(0),
m_iTechShare(0),
//Doto-DPII < Maintenance Modifiers >
m_iGlobalMaintenanceModifier(0),
m_iDistanceMaintenanceModifier(0),
m_iNumCitiesMaintenanceModifier(0),
m_iConnectedCityMaintenanceModifier(0),
//Doto-DPII < Maintenance Modifiers >
m_iEveryoneSpecialUnit(NO_SPECIALUNIT),
m_iEveryoneSpecialBuilding(NO_SPECIALBUILDING),
m_iVictoryDelayPercent(0),
m_iSuccessRate(0),
//Doto-davidlallen: project civilization and free unit start
m_iCivilization(NO_CIVILIZATION),
m_iFreeUnit(NO_UNIT),
//Doto-davidlallen: project civilization and free unit start
m_bSpaceship(false),
m_bAllowsNukes(false),
m_piBonusProductionModifier(NULL),
m_piVictoryThreshold(NULL),
m_piVictoryMinThreshold(NULL),
m_piProjectsNeeded(NULL)
{}

CvProjectInfo::~CvProjectInfo()
{
	SAFE_DELETE_ARRAY(m_piBonusProductionModifier);
	SAFE_DELETE_ARRAY(m_piVictoryThreshold);
	SAFE_DELETE_ARRAY(m_piVictoryMinThreshold);
	SAFE_DELETE_ARRAY(m_piProjectsNeeded);
}

const char* CvProjectInfo::getMovieArtDef() const
{
	return m_szMovieArtDef;
}

const TCHAR* CvProjectInfo::getCreateSound() const
{
	return m_szCreateSound;
}

void CvProjectInfo::setCreateSound(const TCHAR* szVal)
{
	m_szCreateSound = szVal;
}
// advc.008e:
bool CvProjectInfo::nameNeedsArticle() const
{
	if(!isLimited())
		return false;
	CvWString szKey = getTextKeyWide();
	// (see comment in CvBuildingInfo::nameNeedsArticle)
	CvWString szText = gDLL->getText(szKey + L"_NA");
	return (szText.compare(L".") != 0);
} // </advc.008e>

int CvProjectInfo::getBonusProductionModifier(int i) const
{
	FAssertBounds(0, GC.getNumBonusInfos(), i);
	return m_piBonusProductionModifier ? m_piBonusProductionModifier[i] : 0; // advc.003t
}

int CvProjectInfo::getVictoryThreshold(int i) const
{
	FAssertBounds(0, GC.getNumVictoryInfos(), i);
	return m_piVictoryThreshold ? m_piVictoryThreshold[i] : 0; // advc.003t
}

int CvProjectInfo::getVictoryMinThreshold(int i) const
{
	FAssertBounds(0, GC.getNumVictoryInfos(), i);
	// <advc.003t>
	if (m_piVictoryMinThreshold == NULL)
		return getVictoryThreshold(i); // !!
	if (m_piVictoryMinThreshold[i] != 0) // </advc.003t>
		return m_piVictoryMinThreshold[i];
	return getVictoryThreshold(i);
}

int CvProjectInfo::getProjectsNeeded(int i) const
{
	FAssertBounds(0, GC.getNumProjectInfos(), i);
	return m_piProjectsNeeded ? m_piProjectsNeeded[i] : 0; // advc.003t: was false
}
//Doto-DPII < Maintenance Modifiers >
int CvProjectInfo::getGlobalMaintenanceModifier() const
{
    return m_iGlobalMaintenanceModifier;
}

int CvProjectInfo::getDistanceMaintenanceModifier() const
{
    return m_iDistanceMaintenanceModifier;
}

int CvProjectInfo::getNumCitiesMaintenanceModifier() const
{
    return m_iNumCitiesMaintenanceModifier;

}

int CvProjectInfo::getConnectedCityMaintenanceModifier() const
{
    return m_iConnectedCityMaintenanceModifier;
}
//Doto-DPII < Maintenance Modifiers >
// Doto-davidlallen: project civilization and free unit start
int CvProjectInfo::getCivilization() const
{
	return m_iCivilization;
}

int CvProjectInfo::getFreeUnit() const
{
	return m_iFreeUnit;
}
// Doto-davidlallen: project civilization and free unit end

bool CvProjectInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->SetInfoIDFromChildXmlVal(m_iVictoryPrereq, "VictoryPrereq");
	pXML->SetInfoIDFromChildXmlVal(m_iTechPrereq, "TechPrereq");

	pXML->GetChildXmlValByName(&m_iMaxGlobalInstances, "iMaxGlobalInstances");
	pXML->GetChildXmlValByName(&m_iMaxTeamInstances, "iMaxTeamInstances");
	pXML->GetChildXmlValByName(&m_iProductionCost, "iCost");
	pXML->GetChildXmlValByName(&m_iNukeInterception, "iNukeInterception");
	pXML->GetChildXmlValByName(&m_iTechShare, "iTechShare");
	//Doto-DPII < Maintenance Modifiers >
	pXML->GetChildXmlValByName(&m_iGlobalMaintenanceModifier, "iGlobalMaintenanceModifier", 0);
	pXML->GetChildXmlValByName(&m_iDistanceMaintenanceModifier, "iDistanceMaintenanceModifier", 0);
	pXML->GetChildXmlValByName(&m_iNumCitiesMaintenanceModifier, "iNumCitiesMaintenanceModifier", 0);
	pXML->GetChildXmlValByName(&m_iConnectedCityMaintenanceModifier, "iConnectedCityMaintenanceModifier", 0);
	//Doto-DPII < Maintenance Modifiers >


	pXML->SetInfoIDFromChildXmlVal(m_iEveryoneSpecialUnit, "EveryoneSpecialUnit");
	pXML->SetInfoIDFromChildXmlVal(m_iEveryoneSpecialBuilding, "EveryoneSpecialBuilding");

	pXML->GetChildXmlValByName(&m_bSpaceship, "bSpaceship");
	pXML->GetChildXmlValByName(&m_bAllowsNukes, "bAllowsNukes");
	pXML->GetChildXmlValByName(m_szMovieArtDef, "MovieDefineTag");

	pXML->SetVariableListTagPair(&m_piBonusProductionModifier, "BonusProductionModifiers", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_piVictoryThreshold, "VictoryThresholds", GC.getNumVictoryInfos());
	pXML->SetVariableListTagPair(&m_piVictoryMinThreshold, "VictoryMinThresholds", GC.getNumVictoryInfos());
	pXML->GetChildXmlValByName(&m_iVictoryDelayPercent, "iVictoryDelayPercent");
	pXML->GetChildXmlValByName(&m_iSuccessRate, "iSuccessRate");
	
	// Doto-davidlallen: project civilization and free unit start	
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, "CivilizationType",
		""); // f1rpo
	m_iCivilization = pXML->FindInInfoClass(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "FreeUnit",
		""); // f1rpo
	m_iFreeUnit = pXML->FindInInfoClass(szTextVal);
	// Doto-davidlallen: project civilization and free unit end
	pXML->GetChildXmlValByName(szTextVal, "CreateSound");
	setCreateSound(szTextVal);

	return true;
}

bool CvProjectInfo::readPass2(CvXMLLoadUtility* pXML)
{
	pXML->SetVariableListTagPair(&m_piProjectsNeeded, "PrereqProjects", GC.getNumProjectInfos());
	pXML->SetInfoIDFromChildXmlVal(m_iAnyoneProjectPrereq, "AnyonePrereqProject");

	return true;
}
