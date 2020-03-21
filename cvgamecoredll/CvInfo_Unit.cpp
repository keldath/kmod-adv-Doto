// advc.003x: Cut from CvInfos.cpp

#include "CvGameCoreDLL.h"
#include "CvInfo_Unit.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLXMLIFaceBase.h"
//VET_Andera412_Blocade_Unit
//in order for the game optionto work - this file is needed, explained by f1rpo for 097.
#include "CvGame.h" 
//VET_Andera412_Blocade_Unit


bool CvUnitInfo::m_bCanAnyMoveAllTerrain = false; // advc.opt (static initialization)

CvUnitInfo::CvUnitInfo() 
:
/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
m_pbPrereqOrCivics(NULL),
m_pbPrereqAndCivics(NULL),
/**
 ** End: Unit Civic Prereq
 **/
m_iAIWeight(0),
m_iProductionCost(0),
m_iHurryCostModifier(0),
m_iAdvancedStartCost(100), // advc (from MNAI)
m_iAdvancedStartCostIncrease(0),
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
m_iNumCitySizeUnitPrereq(0),
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
m_iMinAreaSize(0),
m_iMoves(0),
m_iAirRange(0),
m_iAirUnitCap(0),
m_iDropRange(0),
m_iNukeRange(0),
m_iWorkRate(0),
m_iBaseDiscover(0),
m_iDiscoverMultiplier(0),
m_iBaseHurry(0),
m_iHurryMultiplier(0),
m_iBaseTrade(0),
m_iTradeMultiplier(0),
m_iGreatWorkCulture(0),
m_iEspionagePoints(0),
m_iCombat(0),
m_iCombatLimit(0),
m_iAirCombat(0),
m_iAirCombatLimit(0),
m_iXPValueAttack(0),
m_iXPValueDefense(0),
m_iFirstStrikes(0),
m_iChanceFirstStrikes(0),
m_iInterceptionProbability(0),
m_iEvasionProbability(0),
m_iWithdrawalProbability(0),
m_iCollateralDamage(0),
m_iCollateralDamageLimit(0),
m_iCollateralDamageMaxUnits(0),
m_iCityAttackModifier(0),
m_iCityDefenseModifier(0),
m_iAnimalCombatModifier(0),
m_iBarbarianCombatModifier(0), // advc.315c
m_iHillsAttackModifier(0),
m_iHillsDefenseModifier(0),
m_iBombRate(0),
m_iBombardRate(0),
m_iSpecialCargo(0),
m_iDomainCargo(0),
m_iCargoSpace(0),
m_iConscriptionValue(0),
m_iCultureGarrisonValue(0),
m_iExtraCost(0),
m_iAssetValue(0),
m_iPowerValue(0),
m_iUnitClassType(NO_UNITCLASS),
m_iSpecialUnitType(NO_SPECIALUNIT),
m_iUnitCaptureClassType(NO_UNITCLASS),
m_iUnitCombatType(NO_UNITCOMBAT),
m_iDomainType(NO_DOMAIN),
m_iDefaultUnitAIType(NO_UNITAI),
m_iInvisibleType(NO_INVISIBLE),
m_iAdvisorType(NO_ADVISOR),
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
//missing pre civic
m_iMaxStartEra(NO_ERA),
m_iForceObsoleteTech(NO_TECH),
m_bStateReligion(false),
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
m_iPrereqBuilding(NO_BUILDING),
m_iPrereqAndTech(NO_TECH),
m_iPrereqAndBonus(NO_BONUS),
//m_iPrereqVicinityBonus(NO_BONUS),  //Shqype Vicinity Bonus Add
m_iGroupSize(0),
m_iGroupDefinitions(0),
m_iUnitMeleeWaveSize(0),
m_iUnitRangedWaveSize(0),
m_iNumUnitNames(0),
m_iCommandType(NO_COMMAND),
// <kmodx>
m_iLeaderExperience(0),
m_iLeaderPromotion(NO_PROMOTION),
// </kmodx>
m_bAnimal(false),
m_bFoodProduction(false),
m_bNoBadGoodies(false),
m_bOnlyDefensive(false),
m_bOnlyAttackAnimals(false), // advc.315a
m_bOnlyAttackBarbarians(false), // advc.315b
m_bNoCapture(false),
m_bQuickCombat(false),
m_bRivalTerritory(false),
m_bMilitaryHappiness(false),
m_bMilitarySupport(false),
m_bMilitaryProduction(false),
m_bPillage(false),
m_bSpy(false),
m_bSabotage(false),
m_bDestroy(false),
m_bStealPlans(false),
m_bInvestigate(false),
m_bCounterSpy(false),
m_bFound(false),
m_bGoldenAge(false),
m_bInvisible(false),
m_bFirstStrikeImmune(false),
m_bNoDefensiveBonus(false),
m_bIgnoreBuildingDefense(false),
m_bCanMoveImpassable(false),
m_bCanMoveAllTerrain(false),
//Mountains mod
m_bCanMovePeak(false), //Deliverator
m_bFlatMovementCost(false),
m_bIgnoreTerrainCost(false),
m_bNukeImmune(false),
m_bPrereqBonuses(false),
m_bPrereqReligion(false),
m_bMechanized(false),
m_bRenderBelowWater(false),
m_bRenderAlways(false),
m_bSuicide(false),
m_bLineOfSight(false),
m_bHiddenNationality(false),
m_bAlwaysHostile(false),
m_bNoRevealMap(false),
m_fUnitMaxSpeed(0.0f),
m_fUnitPadTime(0.0f),
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
//m_pbPrereqOrCivics(NULL),
m_pbPrereqBuildingClass(NULL),
m_piPrereqBuildingClassOverrideTech(NULL),
m_piPrereqBuildingClassOverrideEra(NULL),
m_pbForceObsoleteUnitClass(NULL),
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
m_pbUpgradeUnitClass(NULL),
m_pbTargetUnitClass(NULL),
m_pbTargetUnitCombat(NULL),
m_pbDefenderUnitClass(NULL),
m_pbDefenderUnitCombat(NULL),
m_piFlankingStrikeUnitClass(NULL),
m_pbUnitAIType(NULL),
m_pbNotUnitAIType(NULL),
m_pbBuilds(NULL),
m_piReligionSpreads(NULL),
m_piCorporationSpreads(NULL),
m_piTerrainPassableTech(NULL),
m_piFeaturePassableTech(NULL),
m_pbGreatPeoples(NULL),
m_pbBuildings(NULL),
//m_pbForceBuildings(NULL), // advc.003t
m_pbTerrainImpassable(NULL),
m_pbFeatureImpassable(NULL),
m_piPrereqAndTechs(NULL),
m_piPrereqOrBonuses(NULL),
//m_piPrereqOrVicinityBonuses(NULL),  //Shqype Vicinity Bonus Add
m_piProductionTraits(NULL),
m_piFlavorValue(NULL),
m_piTerrainAttackModifier(NULL),
m_piTerrainDefenseModifier(NULL),
m_piFeatureAttackModifier(NULL),
m_piFeatureDefenseModifier(NULL),
m_piUnitClassAttackModifier(NULL),
m_piUnitClassDefenseModifier(NULL),
m_piUnitCombatModifier(NULL),
m_piUnitCombatCollateralImmune(NULL),
m_piDomainModifier(NULL),
m_piBonusProductionModifier(NULL),
m_piUnitGroupRequired(NULL),
m_pbTerrainNative(NULL),
m_pbFeatureNative(NULL),
m_pbFreePromotions(NULL),
m_paszEarlyArtDefineTags(NULL),
m_paszLateArtDefineTags(NULL),
m_paszMiddleArtDefineTags(NULL),
m_paszUnitNames(NULL)
{
	m_piSpeedBonuses[0] = m_piSpeedBonuses[1] = NULL; // advc.905b
}


CvUnitInfo::~CvUnitInfo()
{
/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
	SAFE_DELETE_ARRAY(m_pbPrereqOrCivics);
	SAFE_DELETE_ARRAY(m_pbPrereqAndCivics);
/**
 ** End: Unit Civic Prereq
 **/
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
//	SAFE_DELETE_ARRAY(m_pbPrereqOrCivics);
	SAFE_DELETE_ARRAY(m_pbPrereqBuildingClass);
	SAFE_DELETE_ARRAY(m_piPrereqBuildingClassOverrideTech);
	SAFE_DELETE_ARRAY(m_piPrereqBuildingClassOverrideEra);
	SAFE_DELETE_ARRAY(m_pbForceObsoleteUnitClass);
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
	SAFE_DELETE_ARRAY(m_pbUpgradeUnitClass);
	SAFE_DELETE_ARRAY(m_pbTargetUnitClass);
	SAFE_DELETE_ARRAY(m_pbTargetUnitCombat);
	SAFE_DELETE_ARRAY(m_pbDefenderUnitClass);
	SAFE_DELETE_ARRAY(m_pbDefenderUnitCombat);
	SAFE_DELETE_ARRAY(m_piFlankingStrikeUnitClass);
	SAFE_DELETE_ARRAY(m_pbUnitAIType);
	SAFE_DELETE_ARRAY(m_pbNotUnitAIType);
	SAFE_DELETE_ARRAY(m_pbBuilds);
	SAFE_DELETE_ARRAY(m_piReligionSpreads);
	SAFE_DELETE_ARRAY(m_piCorporationSpreads);
	SAFE_DELETE_ARRAY(m_piTerrainPassableTech);
	SAFE_DELETE_ARRAY(m_piFeaturePassableTech);
	SAFE_DELETE_ARRAY(m_pbGreatPeoples);
	SAFE_DELETE_ARRAY(m_pbBuildings);
	//SAFE_DELETE_ARRAY(m_pbForceBuildings); // advc.003t
	SAFE_DELETE_ARRAY(m_pbTerrainImpassable);
	SAFE_DELETE_ARRAY(m_pbFeatureImpassable);
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_piPrereqOrBonuses);
//	SAFE_DELETE_ARRAY(m_piPrereqOrVicinityBonuses);  //Shqype Vicinity Bonus Add
	// <advc.905b>
	SAFE_DELETE_ARRAY(m_piSpeedBonuses[0]);
	SAFE_DELETE_ARRAY(m_piSpeedBonuses[1]);
	// </advc.905b>
	SAFE_DELETE_ARRAY(m_piProductionTraits);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piTerrainAttackModifier);
	SAFE_DELETE_ARRAY(m_piTerrainDefenseModifier);
	SAFE_DELETE_ARRAY(m_piFeatureAttackModifier);
	SAFE_DELETE_ARRAY(m_piFeatureDefenseModifier);
	SAFE_DELETE_ARRAY(m_piUnitClassAttackModifier);
	SAFE_DELETE_ARRAY(m_piUnitClassDefenseModifier);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifier);
	SAFE_DELETE_ARRAY(m_piUnitCombatCollateralImmune);
	SAFE_DELETE_ARRAY(m_piDomainModifier);
	SAFE_DELETE_ARRAY(m_piBonusProductionModifier);
	SAFE_DELETE_ARRAY(m_piUnitGroupRequired);
	SAFE_DELETE_ARRAY(m_pbTerrainNative);
	SAFE_DELETE_ARRAY(m_pbFeatureNative);
	SAFE_DELETE_ARRAY(m_pbFreePromotions);
	SAFE_DELETE_ARRAY(m_paszEarlyArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszLateArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszMiddleArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszUnitNames);
}

//keldath QA-DONE
/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
bool CvUnitInfo::isPrereqOrCivics(int i) const
{
	FAssertMsg(i < GC.getNumCivicInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbPrereqOrCivics ? m_pbPrereqOrCivics[i] : false;
}

bool CvUnitInfo::isPrereqAndCivics(int i) const
{
	FAssertMsg(i < GC.getNumCivicInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbPrereqAndCivics ? m_pbPrereqAndCivics[i] : false;
}

//This is for the readpass3
int CvUnitInfo::isPrereqOrCivicsVectorSize()					{return m_aszPrereqOrCivicsforPass3.size();}
CvString CvUnitInfo::isPrereqOrCivicsNamesVectorElement(int i)	{return m_aszPrereqOrCivicsforPass3[i];}
int CvUnitInfo::isPrereqOrCivicsValuesVectorElement(int i)		{return m_abPrereqOrCivicsforPass3[i];}
int CvUnitInfo::isPrereqAndCivicsVectorSize()					{return m_aszPrereqAndCivicsforPass3.size();}
CvString CvUnitInfo::isPrereqAndCivicsNamesVectorElement(int i)	{return m_aszPrereqAndCivicsforPass3[i];}
int CvUnitInfo::isPrereqAndCivicsValuesVectorElement(int i)		{return m_abPrereqAndCivicsforPass3[i];}
/**
 ** End: Unit Civic Prereq
 **/
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
int CvUnitInfo::getNumCitySizeUnitPrereq() const
{
	return m_iNumCitySizeUnitPrereq;
}
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/**********************/

int CvUnitInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}

int CvUnitInfo::getAdvancedStartCostIncrease() const
{
	return m_iAdvancedStartCostIncrease;
}

void CvUnitInfo::setCombat(int iNum)
{
	m_iCombat = iNum;
}
// advc.003w:
bool CvUnitInfo::isTechRequired(TechTypes eTech) const
{
	if (getPrereqAndTech() == eTech)
		return true;
	// <advc.003t>
	if (!isAnyPrereqAndTech())
		return false; // </advc.003t>
	for (int i = 0; i < GC.getNUM_UNIT_AND_TECH_PREREQS(); i++)
	{
		if (getPrereqAndTechs(i) == eTech)
			return true;
	}
	return false;
}

int CvUnitInfo::getGroupSize() const
{
	return m_iGroupSize;
}

int CvUnitInfo::getGroupDefinitions() const
{
	return m_iGroupDefinitions;
}

int CvUnitInfo::getMeleeWaveSize() const
{
	return m_iUnitMeleeWaveSize;
}

int CvUnitInfo::getRangedWaveSize() const
{
	return m_iUnitRangedWaveSize;
}

int CvUnitInfo::getNumUnitNames() const
{
	return m_iNumUnitNames;
}

void CvUnitInfo::setInvisible(bool bEnable)
{
	m_bInvisible = bEnable;
}

bool CvUnitInfo::isRenderBelowWater() const
{
	return m_bRenderBelowWater;
}

bool CvUnitInfo::isRenderAlways() const
{
	return m_bRenderAlways;
}

float CvUnitInfo::getUnitMaxSpeed() const
{
	return m_fUnitMaxSpeed;
}

float CvUnitInfo::getUnitPadTime() const
{
	return m_fUnitPadTime;
}

CommandTypes CvUnitInfo::getCommandType() const
{
	return (CommandTypes)m_iCommandType;
}

void CvUnitInfo::setCommandType(CommandTypes eNewType)
{
	m_iCommandType = (CommandTypes)eNewType;
}
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
int CvUnitInfo::getMaxStartEra() const				
{
	return m_iMaxStartEra;
}

int CvUnitInfo::getForceObsoleteTech() const
{
	return m_iForceObsoleteTech;
}

bool CvUnitInfo::isStateReligion() const				
{
	return m_bStateReligion;
}

int CvUnitInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}

int CvUnitInfo::getNotGameOption() const
{
	return m_iNotGameOption;
}
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
//Shqype Vicinity Bonus Start
/*int CvUnitInfo::getPrereqVicinityBonus() const			
{
	return m_iPrereqVicinityBonus;
}*/
//Shqype Vicinity Bonus End
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
/*bool CvUnitInfo::isPrereqOrCivics(int i) const	
{
	FAssertMsg(i < GC.getNumCivicInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbPrereqOrCivics ? m_pbPrereqOrCivics[i] : false;
}
*/
bool CvUnitInfo::isPrereqBuildingClass(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbPrereqBuildingClass ? m_pbPrereqBuildingClass[i] : false;
}

int CvUnitInfo::getPrereqBuildingClassOverrideTech(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqBuildingClassOverrideTech ? m_piPrereqBuildingClassOverrideTech[i] : -1;
}

int CvUnitInfo::getPrereqBuildingClassOverrideEra(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqBuildingClassOverrideEra ? m_piPrereqBuildingClassOverrideEra[i] : -1;
}

bool CvUnitInfo::getForceObsoleteUnitClass(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbForceObsoleteUnitClass ? m_pbForceObsoleteUnitClass[i] : false;
}
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/


TechTypes CvUnitInfo::getPrereqAndTechs(int i) const
{
	FAssertBounds(0, GC.getNUM_UNIT_AND_TECH_PREREQS(), i);
	return m_piPrereqAndTechs ? (TechTypes)m_piPrereqAndTechs[i] : NO_TECH; // advc.003t
}
//Shqype Vicinity Bonus Start
/*int CvUnitInfo::getPrereqOrVicinityBonuses(int i) const
{
	FAssertMsg(i < GC.getNUM_UNIT_PREREQ_OR_BONUSES(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqOrVicinityBonuses ? m_piPrereqOrVicinityBonuses[i] : -1;
}*/
//Shqype Vicinity Bonus End

BonusTypes CvUnitInfo::getPrereqOrBonuses(int i) const
{
	FAssertBounds(0, GC.getNUM_UNIT_PREREQ_OR_BONUSES(), i);
	return m_piPrereqOrBonuses ? (BonusTypes)m_piPrereqOrBonuses[i] : NO_BONUS; // advc.003t
}
// <advc.905b>
BonusTypes CvUnitInfo::getSpeedBonuses(int i) const {

	FAssertBounds(0, GC.getNUM_UNIT_SPEED_BONUSES(), i);
	return m_piSpeedBonuses[0] ? (BonusTypes)m_piSpeedBonuses[0][i] : NO_BONUS;
}

int CvUnitInfo::getExtraMoves(int i) const {

	FAssertBounds(0, GC.getNUM_UNIT_SPEED_BONUSES(), i);
	return m_piSpeedBonuses[1] ? m_piSpeedBonuses[1][i] : 0;
} // </advc.905b>

int CvUnitInfo::getProductionTraits(int i) const
{
	FAssertBounds(0, GC.getNumTraitInfos(), i);
	return m_piProductionTraits ? m_piProductionTraits[i] : 0; // advc.003t
}

int CvUnitInfo::getFlavorValue(int i) const
{
	FAssertBounds(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0; // advc.003t
}

int CvUnitInfo::getTerrainAttackModifier(int i) const
{
	FAssertBounds(0, GC.getNumTerrainInfos(), i);
	return m_piTerrainAttackModifier ? m_piTerrainAttackModifier[i] : 0; // advc.003t
}

int CvUnitInfo::getTerrainDefenseModifier(int i) const
{
	FAssertBounds(0, GC.getNumTerrainInfos(), i);
	return m_piTerrainDefenseModifier ? m_piTerrainDefenseModifier[i] : 0; // advc.003t
}

int CvUnitInfo::getFeatureAttackModifier(int i) const
{
	FAssertBounds(0, GC.getNumFeatureInfos(), i);
	return m_piFeatureAttackModifier ? m_piFeatureAttackModifier[i] : 0; // advc.003t
}

int CvUnitInfo::getFeatureDefenseModifier(int i) const
{
	FAssertBounds(0, GC.getNumFeatureInfos(), i);
	return m_piFeatureDefenseModifier ? m_piFeatureDefenseModifier[i] : 0; // advc.003t
}

int CvUnitInfo::getUnitClassAttackModifier(int i) const
{
	FAssertBounds(0, GC.getNumUnitClassInfos(), i);
	return m_piUnitClassAttackModifier ? m_piUnitClassAttackModifier[i] : 0; // advc.003t
}

int CvUnitInfo::getUnitClassDefenseModifier(int i) const
{
	FAssertBounds(0, GC.getNumUnitClassInfos(), i);
	return m_piUnitClassDefenseModifier ? m_piUnitClassDefenseModifier[i] : 0; // advc.003t
}

int CvUnitInfo::getUnitCombatModifier(int i) const
{
	FAssertBounds(0, GC.getNumUnitCombatInfos(), i);
	return m_piUnitCombatModifier ? m_piUnitCombatModifier[i] : 0; // advc.003t
}

int CvUnitInfo::getUnitCombatCollateralImmune(int i) const
{
	FAssertBounds(0, GC.getNumUnitCombatInfos(), i);
	return m_piUnitCombatCollateralImmune ? m_piUnitCombatCollateralImmune[i]
			: 0; // advc.003t: Was -1. This one acts as a boolean actually.
}

int CvUnitInfo::getDomainModifier(int i) const
{
	FAssertBounds(0, NUM_DOMAIN_TYPES, i);
	return m_piDomainModifier ? m_piDomainModifier[i] : 0; // advc.003t
}

int CvUnitInfo::getBonusProductionModifier(int i) const
{
	FAssertBounds(0, GC.getNumBonusInfos(), i);
	return m_piBonusProductionModifier ? m_piBonusProductionModifier[i] : 0; // advc.003t
}

int CvUnitInfo::getUnitGroupRequired(int i) const
{
	FAssertBounds(0, getGroupDefinitions(), i);
	return m_piUnitGroupRequired ? m_piUnitGroupRequired[i] : NULL;
}

bool CvUnitInfo::getUpgradeUnitClass(int i) const
{
	FAssertBounds(0, GC.getNumUnitClassInfos(), i);
	return m_pbUpgradeUnitClass ? m_pbUpgradeUnitClass[i] : false;
}

bool CvUnitInfo::getTargetUnitClass(int i) const
{
	FAssertBounds(0, GC.getNumUnitClassInfos(), i);
	return m_pbTargetUnitClass ? m_pbTargetUnitClass[i] : false;
}

bool CvUnitInfo::getTargetUnitCombat(int i) const
{
	FAssertBounds(0, GC.getNumUnitCombatInfos(), i);
	return m_pbTargetUnitCombat ? m_pbTargetUnitCombat[i] : false;
}

bool CvUnitInfo::getDefenderUnitClass(int i) const
{
	FAssertBounds(0, GC.getNumUnitClassInfos(), i);
	return m_pbDefenderUnitClass ? m_pbDefenderUnitClass[i] : false;
}

bool CvUnitInfo::getDefenderUnitCombat(int i) const
{
	FAssertBounds(0, GC.getNumUnitCombatInfos(), i);
	return m_pbDefenderUnitCombat ? m_pbDefenderUnitCombat[i] : false;
}

bool CvUnitInfo::getUnitAIType(int i) const
{
	FAssertBounds(0, NUM_UNITAI_TYPES, i);
	return m_pbUnitAIType ? m_pbUnitAIType[i] : false;
}

int CvUnitInfo::getFlankingStrikeUnitClass(int i) const
{
	FAssertBounds(0, GC.getNumUnitClassInfos(), i);
	return m_piFlankingStrikeUnitClass ? m_piFlankingStrikeUnitClass[i]
			: 0; // advc.003t: Was -1. This is called iFlankingStrength in XML.
}

bool CvUnitInfo::getNotUnitAIType(int i) const
{
	FAssertBounds(0, NUM_UNITAI_TYPES, i);
	return m_pbNotUnitAIType ? m_pbNotUnitAIType[i] : false;
}

bool CvUnitInfo::getBuilds(int i) const
{
	FAssertBounds(0, GC.getNumBuildInfos(), i);
	return m_pbBuilds ? m_pbBuilds[i] : false;
}

int CvUnitInfo::getReligionSpreads(int i) const
{
	FAssertBounds(0, GC.getNumReligionInfos(), i);
	return m_piReligionSpreads ? m_piReligionSpreads[i] :
			0; // advc.003t: Was -1. This is called iReligionSpread in XML.
}

int CvUnitInfo::getCorporationSpreads(int i) const
{
	FAssertBounds(0, GC.getNumCorporationInfos(), i);
	return m_piCorporationSpreads ? m_piCorporationSpreads[i] : 0; // advc.003t
}

TechTypes CvUnitInfo::getTerrainPassableTech(int i) const
{
	FAssertBounds(0, GC.getNumTerrainInfos(), i);
	return m_piTerrainPassableTech ? (TechTypes)m_piTerrainPassableTech[i] : NO_TECH; // advc.003t
}

TechTypes CvUnitInfo::getFeaturePassableTech(int i) const
{
	FAssertBounds(0, GC.getNumFeatureInfos(), i);
	return m_piFeaturePassableTech ? (TechTypes)m_piFeaturePassableTech[i] : NO_TECH; // advc.003t
}

bool CvUnitInfo::getGreatPeoples(int i) const
{
	FAssertBounds(0, GC.getNumSpecialistInfos(), i);
	return m_pbGreatPeoples ? m_pbGreatPeoples[i] : false;
}

bool CvUnitInfo::getBuildings(int i) const
{
	FAssertBounds(0, GC.getNumBuildingInfos(), i);
	return m_pbBuildings ? m_pbBuildings[i] : false;
}

bool CvUnitInfo::getForceBuildings(int i) const
{
	/*FAssertBounds(0, GC.getNumBuildingInfos(), i, "CvUnitInfo::getForceBuildings");
	return m_pbForceBuildings ? m_pbForceBuildings[i] : false;*/
	/*  advc.003t: I've removed the tag from XML, but don't want to break Python
		code (yet). Specifically, the BtS CvCorporationScreen.py calls
		getForceBuldings in line 153. */
	return false;
}

bool CvUnitInfo::getTerrainImpassable(int i) const
{
	FAssertBounds(0, GC.getNumTerrainInfos(), i);
	return m_pbTerrainImpassable ? m_pbTerrainImpassable[i] : false;
}

bool CvUnitInfo::getFeatureImpassable(int i) const
{
	FAssertBounds(0, GC.getNumFeatureInfos(), i);
	return m_pbFeatureImpassable ? m_pbFeatureImpassable[i] : false;
}

bool CvUnitInfo::getTerrainNative(int i) const
{
	FAssertBounds(0, GC.getNumTerrainInfos(), i);
	return m_pbTerrainNative ? m_pbTerrainNative[i] : false;
}

bool CvUnitInfo::getFeatureNative(int i) const
{
	FAssertBounds(0, GC.getNumFeatureInfos(), i);
	return m_pbFeatureNative ? m_pbFeatureNative[i] : false;
}

bool CvUnitInfo::getFreePromotions(int i) const
{
	FAssertBounds(0, GC.getNumPromotionInfos(), i);
	return m_pbFreePromotions ? m_pbFreePromotions[i] : false;
}

int CvUnitInfo::getLeaderPromotion() const
{
	return m_iLeaderPromotion;
}

int CvUnitInfo::getLeaderExperience() const
{
	return m_iLeaderExperience;
}

bool CvUnitInfo::isPromotionValid(PromotionTypes ePromotion, bool bLeader) const
{
	CvPromotionInfo& kPromotion = GC.getInfo(ePromotion);

	if (getFreePromotions(ePromotion))
		return true;

	if (getUnitCombatType() == NO_UNITCOMBAT)
		return false;

	if (!bLeader && kPromotion.isLeader())
		return false;

	if (!kPromotion.getUnitCombat(getUnitCombatType()))
		return false;

	if (isOnlyDefensive())
	{
		if (kPromotion.getCityAttackPercent() != 0 ||
			kPromotion.getWithdrawalChange() != 0 ||
			kPromotion.getCollateralDamageChange() != 0 ||
			kPromotion./*isBlitz()*/getBlitz() != 0 || // advc.164
			kPromotion.isAmphib() ||
			kPromotion.isRiver() ||
			kPromotion.getHillsAttackPercent() != 0)
		{
			return false;
		}
	}

//MOD@VET_Andera412_Blocade_Unit-begin1/2
	if (!GC.getGame().isOption(GAMEOPTION_BLOCADE_UNIT))
	{
		if (kPromotion.isUnblocade())
		{
			return false;
		}
	}	
//MOD@VET_Andera412_Blocade_Unit-end1/2

	if (isIgnoreTerrainCost() && kPromotion.getMoveDiscountChange() != 0)
		return false;
	// advc.164: Check this in CvUnit::isPromotionValid instead
	/*if (getMoves() == 1 && kPromotion.isBlitz())
		return false;*/

	if (getCollateralDamage() == 0 || getCollateralDamageLimit() == 0 ||
		getCollateralDamageMaxUnits() == 0)
	{
		if (kPromotion.getCollateralDamageChange() != 0)
			return false;
	}

	if (getInterceptionProbability() == 0 &&
		kPromotion.getInterceptChange() != 0)
	{
		return false;
	}

	if (kPromotion.getPrereqPromotion() != NO_PROMOTION &&
		!isPromotionValid((PromotionTypes)kPromotion.getPrereqPromotion(), bLeader))
	{
		return false;
	}

	PromotionTypes ePrereq1 = (PromotionTypes)kPromotion.getPrereqOrPromotion1();
	PromotionTypes ePrereq2 = (PromotionTypes)kPromotion.getPrereqOrPromotion2();	
	/*if (NO_PROMOTION != ePrereq1 || NO_PROMOTION != ePrereq2) {
		bool bValid = false;
		if (!bValid) {
			if (NO_PROMOTION != ePrereq1 && isPromotionValid(ePrereq1, eUnit, bLeader))
				bValid = true;
		}
		if (!bValid) {
			if (NO_PROMOTION != ePrereq2 && isPromotionValid(ePrereq2, eUnit, bLeader))
				bValid = true;
		}*/ // BtS
	// K-Mod, 14/jan/11: third optional preq.
	PromotionTypes ePrereq3 = (PromotionTypes)kPromotion.getPrereqOrPromotion3();
	if (ePrereq1 != NO_PROMOTION || ePrereq2 != NO_PROMOTION || ePrereq3 != NO_PROMOTION)
	{
		bool bValid = false;
		if (ePrereq1 != NO_PROMOTION && isPromotionValid(ePrereq1, bLeader))
			bValid = true;
		if (ePrereq2 != NO_PROMOTION && isPromotionValid(ePrereq2, bLeader))
			bValid = true;
		if (ePrereq3 != NO_PROMOTION && isPromotionValid(ePrereq3, bLeader))
			bValid = true;
		if (!bValid)
			return false;
	} // K-Mod end

	return true;
}

const TCHAR* CvUnitInfo::getEarlyArtDefineTag(int i, UnitArtStyleTypes eStyle) const
{
	FAssertBounds(0, getGroupDefinitions(), i);
	if (NO_UNIT_ARTSTYLE != eStyle)
	{
		int iIndex = GC.getInfoTypeForString(getType());
		if (iIndex != -1)
		{
			const TCHAR* pcTag = GC.getInfo(eStyle).getEarlyArtDefineTag(i, iIndex);
			if (pcTag != NULL)
				return pcTag;
		}
	}
	return (m_paszEarlyArtDefineTags) ? m_paszEarlyArtDefineTags[i] : NULL;
}

void CvUnitInfo::setEarlyArtDefineTag(int i, const TCHAR* szVal)
{
	FAssertBounds(0, getGroupDefinitions(), i);
	m_paszEarlyArtDefineTags[i] = szVal;
}

const TCHAR* CvUnitInfo::getLateArtDefineTag(int i, UnitArtStyleTypes eStyle) const
{
	FAssertBounds(0, getGroupDefinitions(), i);
	if (NO_UNIT_ARTSTYLE != eStyle)
	{
		int iIndex = GC.getInfoTypeForString(getType());
		if (iIndex != -1)
		{
			const TCHAR* pcTag = GC.getInfo(eStyle).getLateArtDefineTag(i, iIndex);
			if (pcTag != NULL)
				return pcTag;
		}
	}
	return (m_paszLateArtDefineTags) ? m_paszLateArtDefineTags[i] : NULL;
}

void CvUnitInfo::setLateArtDefineTag(int i, const TCHAR* szVal)
{
	FAssertBounds(0, getGroupDefinitions(), i);
	m_paszLateArtDefineTags[i] = szVal;
}

const TCHAR* CvUnitInfo::getMiddleArtDefineTag(int i, UnitArtStyleTypes eStyle) const
{
	FAssertBounds(0, getGroupDefinitions(), i);
	if (NO_UNIT_ARTSTYLE != eStyle)
	{
		int iIndex = GC.getInfoTypeForString(getType());
		if (iIndex != -1)
		{
			const TCHAR* pcTag = GC.getInfo(eStyle).getMiddleArtDefineTag(i, iIndex);
			if (pcTag != NULL)
				return pcTag;
		}
	}
	return (m_paszMiddleArtDefineTags) ? m_paszMiddleArtDefineTags[i] : NULL;
}

void CvUnitInfo::setMiddleArtDefineTag(int i, const TCHAR* szVal)
{
	FAssertBounds(0, getGroupDefinitions(), i);
	m_paszMiddleArtDefineTags[i] = szVal;
}

const TCHAR* CvUnitInfo::getUnitNames(int i) const
{
	FAssertBounds(0, getNumUnitNames(), i);
	return (m_paszUnitNames) ? m_paszUnitNames[i] : NULL;
}

const TCHAR* CvUnitInfo::getFormationType() const
{
	return m_szFormationType;
}

const TCHAR* CvUnitInfo::getButton() const
{
	return m_szArtDefineButton;
}

void CvUnitInfo::updateArtDefineButton()
{
	m_szArtDefineButton = getArtInfo(0, NO_ERA, NO_UNIT_ARTSTYLE)->getButton();
}

const CvArtInfoUnit* CvUnitInfo::getArtInfo(int i, EraTypes eEra, UnitArtStyleTypes eStyle) const
{
	if (eEra > GC.getNumEraInfos() / 2 && !CvString(getLateArtDefineTag(i, eStyle)).empty())
		return ARTFILEMGR.getUnitArtInfo(getLateArtDefineTag(i, eStyle));
	else if (eEra > GC.getNumEraInfos() / 4 && !CvString(getMiddleArtDefineTag(i, eStyle)).empty())
		return ARTFILEMGR.getUnitArtInfo(getMiddleArtDefineTag(i, eStyle));
	return ARTFILEMGR.getUnitArtInfo(getEarlyArtDefineTag(i, eStyle));
}
#if SERIALIZE_CVINFOS
void CvUnitInfo::read(FDataStreamBase* stream)
{
	CvHotkeyInfo::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);

/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
	SAFE_DELETE_ARRAY(m_pbPrereqOrCivics);
	m_pbPrereqOrCivics = new bool[GC.getNumCivicInfos()];
	stream->Read(GC.getNumCivicInfos(), m_pbPrereqOrCivics);
	
	SAFE_DELETE_ARRAY(m_pbPrereqAndCivics);
	m_pbPrereqAndCivics = new bool[GC.getNumCivicInfos()];
	stream->Read(GC.getNumCivicInfos(), m_pbPrereqAndCivics);
/**
 ** End: Unit Civic Prereq
 **/

	stream->Read(&m_iAIWeight);
	stream->Read(&m_iProductionCost);
	stream->Read(&m_iHurryCostModifier);
	stream->Read(&m_iAdvancedStartCost);
	stream->Read(&m_iAdvancedStartCostIncrease);
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	stream->Read(&m_iNumCitySizeUnitPrereq);
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	stream->Read(&m_iMinAreaSize);
	stream->Read(&m_iMoves);
	stream->Read(&m_iAirRange);
	stream->Read(&m_iAirUnitCap);
	stream->Read(&m_iDropRange);
	stream->Read(&m_iNukeRange);
	stream->Read(&m_iWorkRate);
	stream->Read(&m_iBaseDiscover);
	stream->Read(&m_iDiscoverMultiplier);
	stream->Read(&m_iBaseHurry);
	stream->Read(&m_iHurryMultiplier);
	stream->Read(&m_iBaseTrade);
	stream->Read(&m_iTradeMultiplier);
	stream->Read(&m_iGreatWorkCulture);
	stream->Read(&m_iEspionagePoints);
	stream->Read(&m_iCombat);
	stream->Read(&m_iCombatLimit);
	stream->Read(&m_iAirCombat);
	stream->Read(&m_iAirCombatLimit);
	stream->Read(&m_iXPValueAttack);
	stream->Read(&m_iXPValueDefense);
	stream->Read(&m_iFirstStrikes);
	stream->Read(&m_iChanceFirstStrikes);
	stream->Read(&m_iInterceptionProbability);
	stream->Read(&m_iEvasionProbability);
	stream->Read(&m_iWithdrawalProbability);
	stream->Read(&m_iCollateralDamage);
	stream->Read(&m_iCollateralDamageLimit);
	stream->Read(&m_iCollateralDamageMaxUnits);
	stream->Read(&m_iCityAttackModifier);
	stream->Read(&m_iCityDefenseModifier);
	stream->Read(&m_iAnimalCombatModifier);
	// <advc.315c>
	if(uiFlag >= 2)
		stream->Read(&m_iBarbarianCombatModifier); // </advc.315c>
	stream->Read(&m_iHillsAttackModifier);
	stream->Read(&m_iHillsDefenseModifier);
	stream->Read(&m_iBombRate);
	stream->Read(&m_iBombardRate);
	stream->Read(&m_iSpecialCargo);
	stream->Read(&m_iDomainCargo);
	stream->Read(&m_iCargoSpace);
	stream->Read(&m_iConscriptionValue);
	stream->Read(&m_iCultureGarrisonValue);
	stream->Read(&m_iExtraCost);
	stream->Read(&m_iAssetValue);
	stream->Read(&m_iPowerValue);
	stream->Read(&m_iUnitClassType);
	stream->Read(&m_iSpecialUnitType);
	stream->Read(&m_iUnitCaptureClassType);
	stream->Read(&m_iUnitCombatType);
	stream->Read(&m_iDomainType);
	stream->Read(&m_iDefaultUnitAIType);
	stream->Read(&m_iInvisibleType);
	int iNumInvisibleTypes;
	stream->Read(&iNumInvisibleTypes);
	for(int i = 0; i < iNumInvisibleTypes; i++)
	{
		int iSeeInvisibleType;
		stream->Read(&iSeeInvisibleType);
		m_aiSeeInvisibleTypes.push_back(iSeeInvisibleType);
	}
	stream->Read(&m_iAdvisorType);
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
	//missing civic
	stream->Read(&m_iMaxStartEra);
	stream->Read(&m_iForceObsoleteTech);
	stream->Read(&m_bStateReligion);
	stream->Read(&m_iPrereqGameOption);
	stream->Read(&m_iNotGameOption);
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
	stream->Read(&m_iHolyCity);
	stream->Read(&m_iReligionType);
	stream->Read(&m_iStateReligion);
	stream->Read(&m_iPrereqReligion);
	stream->Read(&m_iPrereqCorporation);
	stream->Read(&m_iPrereqBuilding);
	stream->Read(&m_iPrereqAndTech);
	stream->Read(&m_iPrereqAndBonus);
//	stream->Read(&m_iPrereqVicinityBonus);  //Shqype Vicinity Bonus Add
	stream->Read(&m_iGroupSize);
	stream->Read(&m_iGroupDefinitions);
	stream->Read(&m_iUnitMeleeWaveSize);
	stream->Read(&m_iUnitRangedWaveSize);
	stream->Read(&m_iNumUnitNames);
	stream->Read(&m_iCommandType);
	stream->Read(&m_bAnimal);
	stream->Read(&m_bFoodProduction);
	stream->Read(&m_bNoBadGoodies);
	stream->Read(&m_bOnlyDefensive);
	// <advc.315>
	if(uiFlag >= 2) {
		stream->Read(&m_bOnlyAttackAnimals); // advc.315a
		stream->Read(&m_bOnlyAttackBarbarians); // advc.315b
	} // </advc.315>
	stream->Read(&m_bNoCapture);
	stream->Read(&m_bQuickCombat);
	stream->Read(&m_bRivalTerritory);
	stream->Read(&m_bMilitaryHappiness);
	stream->Read(&m_bMilitarySupport);
	stream->Read(&m_bMilitaryProduction);
	stream->Read(&m_bPillage);
	stream->Read(&m_bSpy);
	stream->Read(&m_bSabotage);
	stream->Read(&m_bDestroy);
	stream->Read(&m_bStealPlans);
	stream->Read(&m_bInvestigate);
	stream->Read(&m_bCounterSpy);
	stream->Read(&m_bFound);
	stream->Read(&m_bGoldenAge);
	stream->Read(&m_bInvisible);
	stream->Read(&m_bFirstStrikeImmune);
	stream->Read(&m_bNoDefensiveBonus);
	stream->Read(&m_bIgnoreBuildingDefense);
	stream->Read(&m_bCanMoveImpassable);
	stream->Read(&m_bCanMoveAllTerrain);
	// <advc.opt>
	if (m_bCanMoveAllTerrain)
		m_bCanAnyMoveAllTerrain = true; // </advc.opt>
	stream->Read(&m_bCanMovePeak); //Deliverator
	stream->Read(&m_bFlatMovementCost);
	stream->Read(&m_bIgnoreTerrainCost);
	stream->Read(&m_bNukeImmune);
	stream->Read(&m_bPrereqBonuses);
	stream->Read(&m_bPrereqReligion);
	stream->Read(&m_bMechanized);
	stream->Read(&m_bRenderBelowWater);
	stream->Read(&m_bRenderAlways);
	stream->Read(&m_bSuicide);
	stream->Read(&m_bLineOfSight);
	stream->Read(&m_bHiddenNationality);
	stream->Read(&m_bAlwaysHostile);
	stream->Read(&m_bNoRevealMap);
	stream->Read(&m_fUnitMaxSpeed);
	stream->Read(&m_fUnitPadTime);
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	m_piPrereqAndTechs = new int[GC.getNUM_UNIT_AND_TECH_PREREQS()];
	stream->Read(GC.getNUM_UNIT_AND_TECH_PREREQS(), m_piPrereqAndTechs);
//Shqype Vicinity Bonus Start
//	SAFE_DELETE_ARRAY(m_piPrereqOrVicinityBonuses);
//	m_piPrereqOrVicinityBonuses = new int[GC.getNUM_UNIT_PREREQ_OR_BONUSES()];
//	stream->Read(GC.getNUM_UNIT_PREREQ_OR_BONUSES(), m_piPrereqOrVicinityBonuses);
//Shqype Vicinity Bonus End
	SAFE_DELETE_ARRAY(m_piPrereqOrBonuses);
	m_piPrereqOrBonuses = new int[GC.getNUM_UNIT_PREREQ_OR_BONUSES()];
	stream->Read(GC.getNUM_UNIT_PREREQ_OR_BONUSES(), m_piPrereqOrBonuses);
	// <advc.905b>
	SAFE_DELETE_ARRAY(m_piSpeedBonuses[0]);
	SAFE_DELETE_ARRAY(m_piSpeedBonuses[1]);
	m_piSpeedBonuses[0] = new int[GC.getNUM_UNIT_PREREQ_OR_BONUSES()];
	m_piSpeedBonuses[1] = new int[GC.getNUM_UNIT_PREREQ_OR_BONUSES()];
	if(uiFlag >= 3) {
		stream->Read(GC.getNUM_UNIT_PREREQ_OR_BONUSES(), m_piSpeedBonuses[0]);
		stream->Read(GC.getNUM_UNIT_PREREQ_OR_BONUSES(), m_piSpeedBonuses[1]);
	}
	else for(int i = 0; i < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); i++) {
		m_piSpeedBonuses[0][i] = -1;
		m_piSpeedBonuses[1][i] = 0;
	} // </advc.905b>
	SAFE_DELETE_ARRAY(m_piProductionTraits);
	m_piProductionTraits = new int[GC.getNumTraitInfos()];
	stream->Read(GC.getNumTraitInfos(), m_piProductionTraits);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	m_piFlavorValue = new int[GC.getNumFlavorTypes()];
	stream->Read(GC.getNumFlavorTypes(), m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piTerrainAttackModifier);
	m_piTerrainAttackModifier = new int[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_piTerrainAttackModifier);
	SAFE_DELETE_ARRAY(m_piTerrainDefenseModifier);
	m_piTerrainDefenseModifier = new int[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_piTerrainDefenseModifier);
	SAFE_DELETE_ARRAY(m_piFeatureAttackModifier);
	m_piFeatureAttackModifier = new int[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_piFeatureAttackModifier);
	SAFE_DELETE_ARRAY(m_piFeatureDefenseModifier);
	m_piFeatureDefenseModifier = new int[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_piFeatureDefenseModifier);
	SAFE_DELETE_ARRAY(m_piUnitClassAttackModifier);
	m_piUnitClassAttackModifier = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_piUnitClassAttackModifier);
	SAFE_DELETE_ARRAY(m_piUnitClassDefenseModifier);
	m_piUnitClassDefenseModifier = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_piUnitClassDefenseModifier);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifier);
	m_piUnitCombatModifier = new int[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_piUnitCombatModifier);
	SAFE_DELETE_ARRAY(m_piUnitCombatCollateralImmune);
	m_piUnitCombatCollateralImmune = new int[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_piUnitCombatCollateralImmune);
	SAFE_DELETE_ARRAY(m_piDomainModifier);
	m_piDomainModifier = new int[NUM_DOMAIN_TYPES];
	stream->Read(NUM_DOMAIN_TYPES, m_piDomainModifier);
	SAFE_DELETE_ARRAY(m_piBonusProductionModifier);
	m_piBonusProductionModifier = new int[GC.getNumBonusInfos()];
	stream->Read(GC.getNumBonusInfos(), m_piBonusProductionModifier);
	SAFE_DELETE_ARRAY(m_piUnitGroupRequired);
	m_piUnitGroupRequired = new int[m_iGroupDefinitions];
	stream->Read(m_iGroupDefinitions, m_piUnitGroupRequired);
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
/*	SAFE_DELETE_ARRAY(m_pbPrereqOrCivics);
	m_pbPrereqOrCivics = new bool[GC.getNumCivicInfos()];
	stream->Read(GC.getNumCivicInfos(), m_pbPrereqOrCivics);
*/
	SAFE_DELETE_ARRAY(m_pbPrereqBuildingClass);
	m_pbPrereqBuildingClass = new bool[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_pbPrereqBuildingClass);

	SAFE_DELETE_ARRAY(m_piPrereqBuildingClassOverrideTech);
	m_piPrereqBuildingClassOverrideTech = new int[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_piPrereqBuildingClassOverrideTech);

	SAFE_DELETE_ARRAY(m_piPrereqBuildingClassOverrideEra);
	m_piPrereqBuildingClassOverrideEra = new int[GC.getNumBuildingClassInfos()];
	stream->Read(GC.getNumBuildingClassInfos(), m_piPrereqBuildingClassOverrideEra);

	SAFE_DELETE_ARRAY(m_pbForceObsoleteUnitClass);
	m_pbForceObsoleteUnitClass = new bool[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_pbForceObsoleteUnitClass);
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
	SAFE_DELETE_ARRAY(m_pbUpgradeUnitClass);
	m_pbUpgradeUnitClass = new bool[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_pbUpgradeUnitClass);
	SAFE_DELETE_ARRAY(m_pbTargetUnitClass);
	m_pbTargetUnitClass = new bool[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_pbTargetUnitClass);
	SAFE_DELETE_ARRAY(m_pbTargetUnitCombat);
	m_pbTargetUnitCombat = new bool[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_pbTargetUnitCombat);
	SAFE_DELETE_ARRAY(m_pbDefenderUnitClass);
	m_pbDefenderUnitClass = new bool[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_pbDefenderUnitClass);
	SAFE_DELETE_ARRAY(m_pbDefenderUnitCombat);
	m_pbDefenderUnitCombat = new bool[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_pbDefenderUnitCombat);
	SAFE_DELETE_ARRAY(m_piFlankingStrikeUnitClass);
	m_piFlankingStrikeUnitClass = new int[GC.getNumUnitClassInfos()];
	stream->Read(GC.getNumUnitClassInfos(), m_piFlankingStrikeUnitClass);
	SAFE_DELETE_ARRAY(m_pbUnitAIType);
	m_pbUnitAIType = new bool[NUM_UNITAI_TYPES];
	stream->Read(NUM_UNITAI_TYPES, m_pbUnitAIType);
	SAFE_DELETE_ARRAY(m_pbNotUnitAIType);
	m_pbNotUnitAIType = new bool[NUM_UNITAI_TYPES];
	stream->Read(NUM_UNITAI_TYPES, m_pbNotUnitAIType);
	SAFE_DELETE_ARRAY(m_pbBuilds);
	m_pbBuilds = new bool[GC.getNumBuildInfos()];
	stream->Read(GC.getNumBuildInfos(), m_pbBuilds);
	SAFE_DELETE_ARRAY(m_piReligionSpreads);
	m_piReligionSpreads = new int[GC.getNumReligionInfos()];
	stream->Read(GC.getNumReligionInfos(), m_piReligionSpreads);
	SAFE_DELETE_ARRAY(m_piCorporationSpreads);
	m_piCorporationSpreads = new int[GC.getNumCorporationInfos()];
	stream->Read(GC.getNumCorporationInfos(), m_piCorporationSpreads);
	SAFE_DELETE_ARRAY(m_piTerrainPassableTech);
	m_piTerrainPassableTech = new int[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_piTerrainPassableTech);
	SAFE_DELETE_ARRAY(m_piFeaturePassableTech);
	m_piFeaturePassableTech = new int[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_piFeaturePassableTech);
	SAFE_DELETE_ARRAY(m_pbGreatPeoples);
	m_pbGreatPeoples = new bool[GC.getNumSpecialistInfos()];
	stream->Read(GC.getNumSpecialistInfos(), m_pbGreatPeoples);
	SAFE_DELETE_ARRAY(m_pbBuildings);
	m_pbBuildings = new bool[GC.getNumBuildingInfos()];
	stream->Read(GC.getNumBuildingInfos(), m_pbBuildings);
	// advc.003t:
	/*SAFE_DELETE_ARRAY(m_pbForceBuildings);
	m_pbForceBuildings = new bool[GC.getNumBuildingInfos()];
	stream->Read(GC.getNumBuildingInfos(), m_pbForceBuildings);*/
	SAFE_DELETE_ARRAY(m_pbTerrainNative);
	m_pbTerrainNative = new bool[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_pbTerrainNative);
	SAFE_DELETE_ARRAY(m_pbFeatureNative);
	m_pbFeatureNative = new bool[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_pbFeatureNative);
	SAFE_DELETE_ARRAY(m_pbTerrainImpassable);
	m_pbTerrainImpassable = new bool[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_pbTerrainImpassable);
	SAFE_DELETE_ARRAY(m_pbFeatureImpassable);
	m_pbFeatureImpassable = new bool[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_pbFeatureImpassable);
	SAFE_DELETE_ARRAY(m_pbFreePromotions);
	m_pbFreePromotions = new bool[GC.getNumPromotionInfos()];
	stream->Read(GC.getNumPromotionInfos(), m_pbFreePromotions);
	stream->Read(&m_iLeaderPromotion);
	stream->Read(&m_iLeaderExperience);
	SAFE_DELETE_ARRAY(m_paszEarlyArtDefineTags);
	m_paszEarlyArtDefineTags = new CvString [m_iGroupDefinitions];
	stream->ReadString(m_iGroupDefinitions, m_paszEarlyArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszLateArtDefineTags);
	m_paszLateArtDefineTags = new CvString [m_iGroupDefinitions];
	stream->ReadString(m_iGroupDefinitions, m_paszLateArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszMiddleArtDefineTags);
	m_paszMiddleArtDefineTags = new CvString [m_iGroupDefinitions];
	stream->ReadString(m_iGroupDefinitions, m_paszMiddleArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszUnitNames);
	m_paszUnitNames = new CvString[m_iNumUnitNames];
	stream->ReadString(m_iNumUnitNames, m_paszUnitNames);
	stream->ReadString(m_szFormationType);
	updateArtDefineButton();
}

void CvUnitInfo::write(FDataStreamBase* stream)
{
	CvHotkeyInfo::write(stream);
	uint uiFlag=1;
	uiFlag = 2; // advc.315
	uiFlag = 3; // advc.905b
	stream->Write(uiFlag);
/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
	stream->Write(GC.getNumCivicInfos(), m_pbPrereqOrCivics);
	stream->Write(GC.getNumCivicInfos(), m_pbPrereqAndCivics);
/**
 ** End: Unit Civic Prereq
 **/
	stream->Write(m_iAIWeight);
	stream->Write(m_iProductionCost);
	stream->Write(m_iHurryCostModifier);
	stream->Write(m_iAdvancedStartCost);
	stream->Write(m_iAdvancedStartCostIncrease);
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	stream->Write(m_iNumCitySizeUnitPrereq);
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	stream->Write(m_iMinAreaSize);
	stream->Write(m_iMoves);
	stream->Write(m_iAirRange);
	stream->Write(m_iAirUnitCap);
	stream->Write(m_iDropRange);
	stream->Write(m_iNukeRange);
	stream->Write(m_iWorkRate);
	stream->Write(m_iBaseDiscover);
	stream->Write(m_iDiscoverMultiplier);
	stream->Write(m_iBaseHurry);
	stream->Write(m_iHurryMultiplier);
	stream->Write(m_iBaseTrade);
	stream->Write(m_iTradeMultiplier);
	stream->Write(m_iGreatWorkCulture);
	stream->Write(m_iEspionagePoints);
	stream->Write(m_iCombat);
	stream->Write(m_iCombatLimit);
	stream->Write(m_iAirCombat);
	stream->Write(m_iAirCombatLimit);
	stream->Write(m_iXPValueAttack);
	stream->Write(m_iXPValueDefense);
	stream->Write(m_iFirstStrikes);
	stream->Write(m_iChanceFirstStrikes);
	stream->Write(m_iInterceptionProbability);
	stream->Write(m_iEvasionProbability);
	stream->Write(m_iWithdrawalProbability);
	stream->Write(m_iCollateralDamage);
	stream->Write(m_iCollateralDamageLimit);
	stream->Write(m_iCollateralDamageMaxUnits);
	stream->Write(m_iCityAttackModifier);
	stream->Write(m_iCityDefenseModifier);
	stream->Write(m_iAnimalCombatModifier);
	stream->Write(m_iBarbarianCombatModifier); // advc.315c
	stream->Write(m_iHillsAttackModifier);
	stream->Write(m_iHillsDefenseModifier);
	stream->Write(m_iBombRate);
	stream->Write(m_iBombardRate);
	stream->Write(m_iSpecialCargo);
	stream->Write(m_iDomainCargo);
	stream->Write(m_iCargoSpace);
	stream->Write(m_iConscriptionValue);
	stream->Write(m_iCultureGarrisonValue);
	stream->Write(m_iExtraCost);
	stream->Write(m_iAssetValue);
	stream->Write(m_iPowerValue);
	stream->Write(m_iUnitClassType);
	stream->Write(m_iSpecialUnitType);
	stream->Write(m_iUnitCaptureClassType);
	stream->Write(m_iUnitCombatType);
	stream->Write(m_iDomainType);
	stream->Write(m_iDefaultUnitAIType);
	stream->Write(m_iInvisibleType);
	stream->Write((int)m_aiSeeInvisibleTypes.size());
	for(int i = 0; i < (int)m_aiSeeInvisibleTypes.size(); i++)
		stream->Write(m_aiSeeInvisibleTypes[i]);
	stream->Write(m_iAdvisorType);
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
//missing civic
	stream->Write(m_iMaxStartEra);
	stream->Write(m_iForceObsoleteTech);
	stream->Write(m_bStateReligion);
	stream->Write(m_iPrereqGameOption);
	stream->Write(m_iNotGameOption);
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
	stream->Write(m_iHolyCity);
	stream->Write(m_iReligionType);
	stream->Write(m_iStateReligion);
	stream->Write(m_iPrereqReligion);
	stream->Write(m_iPrereqCorporation);
	stream->Write(m_iPrereqBuilding);
	stream->Write(m_iPrereqAndTech);
	stream->Write(m_iPrereqAndBonus);
//	stream->Write(m_iPrereqVicinityBonus);  //Shqype Vicinity Bonus Add
	stream->Write(m_iGroupSize);
	stream->Write(m_iGroupDefinitions);
	stream->Write(m_iUnitMeleeWaveSize);
	stream->Write(m_iUnitRangedWaveSize);
	stream->Write(m_iNumUnitNames);
	stream->Write(m_iCommandType);
	stream->Write(m_bAnimal);
	stream->Write(m_bFoodProduction);
	stream->Write(m_bNoBadGoodies);
	stream->Write(m_bOnlyDefensive);
	stream->Write(m_bOnlyAttackAnimals); // advc.315a
	stream->Write(m_bOnlyAttackBarbarians); // advc.315b
	stream->Write(m_bNoCapture);
	stream->Write(m_bQuickCombat);
	stream->Write(m_bRivalTerritory);
	stream->Write(m_bMilitaryHappiness);
	stream->Write(m_bMilitarySupport);
	stream->Write(m_bMilitaryProduction);
	stream->Write(m_bPillage);
	stream->Write(m_bSpy);
	stream->Write(m_bSabotage);
	stream->Write(m_bDestroy);
	stream->Write(m_bStealPlans);
	stream->Write(m_bInvestigate);
	stream->Write(m_bCounterSpy);
	stream->Write(m_bFound);
	stream->Write(m_bGoldenAge);
	stream->Write(m_bInvisible);
	stream->Write(m_bFirstStrikeImmune);
	stream->Write(m_bNoDefensiveBonus);
	stream->Write(m_bIgnoreBuildingDefense);
	stream->Write(m_bCanMoveImpassable);
	stream->Write(m_bCanMoveAllTerrain);
	stream->Write(m_bCanMovePeak); //Deliverator
	stream->Write(m_bFlatMovementCost);
	stream->Write(m_bIgnoreTerrainCost);
	stream->Write(m_bNukeImmune);
	stream->Write(m_bPrereqBonuses);
	stream->Write(m_bPrereqReligion);
	stream->Write(m_bMechanized);
	stream->Write(m_bRenderBelowWater);
	stream->Write(m_bRenderAlways);
	stream->Write(m_bSuicide);
	stream->Write(m_bLineOfSight);
	stream->Write(m_bHiddenNationality);
	stream->Write(m_bAlwaysHostile);
	stream->Write(m_bNoRevealMap);
	stream->Write(m_fUnitMaxSpeed);
	stream->Write(m_fUnitPadTime);
	stream->Write(GC.getNUM_UNIT_AND_TECH_PREREQS(), m_piPrereqAndTechs);
	stream->Write(GC.getNUM_UNIT_PREREQ_OR_BONUSES(), m_piPrereqOrBonuses);
	//	stream->Write(GC.getNUM_UNIT_PREREQ_OR_BONUSES(), m_piPrereqOrVicinityBonuses);  //Shqype Vicinity Bonus Add
	// <advc.905b>
	stream->Write(GC.getNUM_UNIT_PREREQ_OR_BONUSES(), m_piSpeedBonuses[0]);
	stream->Write(GC.getNUM_UNIT_PREREQ_OR_BONUSES(), m_piSpeedBonuses[1]);
	// </advc.905b>
	stream->Write(GC.getNumTraitInfos(), m_piProductionTraits);
	stream->Write(GC.getNumFlavorTypes(), m_piFlavorValue);
	stream->Write(GC.getNumTerrainInfos(), m_piTerrainAttackModifier);
	stream->Write(GC.getNumTerrainInfos(), m_piTerrainDefenseModifier);
	stream->Write(GC.getNumFeatureInfos(), m_piFeatureAttackModifier);
	stream->Write(GC.getNumFeatureInfos(), m_piFeatureDefenseModifier);
	stream->Write(GC.getNumUnitClassInfos(), m_piUnitClassAttackModifier);
	stream->Write(GC.getNumUnitClassInfos(), m_piUnitClassDefenseModifier);
	stream->Write(GC.getNumUnitCombatInfos(), m_piUnitCombatModifier);
	stream->Write(GC.getNumUnitCombatInfos(), m_piUnitCombatCollateralImmune);
	stream->Write(NUM_DOMAIN_TYPES, m_piDomainModifier);
	stream->Write(GC.getNumBonusInfos(), m_piBonusProductionModifier);
	stream->Write(m_iGroupDefinitions, m_piUnitGroupRequired);
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
//	stream->Write(GC.getNumCivicInfos(), m_pbPrereqOrCivics);
	stream->Write(GC.getNumBuildingClassInfos(), m_pbPrereqBuildingClass);
	stream->Write(GC.getNumBuildingClassInfos(), m_piPrereqBuildingClassOverrideTech);
	stream->Write(GC.getNumBuildingClassInfos(), m_piPrereqBuildingClassOverrideEra);
	stream->Write(GC.getNumUnitClassInfos(), m_pbForceObsoleteUnitClass);
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
	stream->Write(GC.getNumUnitClassInfos(), m_pbUpgradeUnitClass);
	stream->Write(GC.getNumUnitClassInfos(), m_pbTargetUnitClass);
	stream->Write(GC.getNumUnitCombatInfos(), m_pbTargetUnitCombat);
	stream->Write(GC.getNumUnitClassInfos(), m_pbDefenderUnitClass);
	stream->Write(GC.getNumUnitCombatInfos(), m_pbDefenderUnitCombat);
	stream->Write(GC.getNumUnitClassInfos(), m_piFlankingStrikeUnitClass);
	stream->Write(NUM_UNITAI_TYPES, m_pbUnitAIType);
	stream->Write(NUM_UNITAI_TYPES, m_pbNotUnitAIType);
	stream->Write(GC.getNumBuildInfos(), m_pbBuilds);
	stream->Write(GC.getNumReligionInfos(), m_piReligionSpreads);
	stream->Write(GC.getNumCorporationInfos(), m_piCorporationSpreads);
	stream->Write(GC.getNumTerrainInfos(), m_piTerrainPassableTech);
	stream->Write(GC.getNumFeatureInfos(), m_piFeaturePassableTech);
	stream->Write(GC.getNumSpecialistInfos(), m_pbGreatPeoples);
	stream->Write(GC.getNumBuildingInfos(), m_pbBuildings);
	//stream->Write(GC.getNumBuildingInfos(), m_pbForceBuildings); // advc.003t
	stream->Write(GC.getNumTerrainInfos(), m_pbTerrainNative);
	stream->Write(GC.getNumFeatureInfos(), m_pbFeatureNative);
	stream->Write(GC.getNumTerrainInfos(), m_pbTerrainImpassable);
	stream->Write(GC.getNumFeatureInfos(), m_pbFeatureImpassable);
	stream->Write(GC.getNumPromotionInfos(), m_pbFreePromotions);
	stream->Write(m_iLeaderPromotion);
	stream->Write(m_iLeaderExperience);
	stream->WriteString(m_iGroupDefinitions, m_paszEarlyArtDefineTags);
	stream->WriteString(m_iGroupDefinitions, m_paszLateArtDefineTags);
	stream->WriteString(m_iGroupDefinitions, m_paszMiddleArtDefineTags);
	stream->WriteString(m_iNumUnitNames, m_paszUnitNames);
	stream->WriteString(m_szFormationType);
}
#endif

bool CvUnitInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvHotkeyInfo::read(pXML))
		return false;

	CvString szTextVal;
/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"PrereqOrCivics"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			bool bTemp = false;
			if (iNumSibs > 0)
			{
				if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
				{
					for (int i=0;i<iNumSibs;i++)
					{
						if (pXML->GetChildXmlVal(szTextVal))
						{
                            m_aszPrereqOrCivicsforPass3.push_back(szTextVal);
							//keldath qa3-done -it was &bTemp 
                            pXML->GetNextXmlVal(bTemp);
                            m_abPrereqOrCivicsforPass3.push_back(bTemp);
							gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
						}

						if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
						{
							break;
						}
					}

					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"PrereqAndCivics"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			bool bTemp = false;
			if (iNumSibs > 0)
			{
				if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
				{
					for (int i=0;i<iNumSibs;i++)
					{
						if (pXML->GetChildXmlVal(szTextVal))
						{
                            m_aszPrereqAndCivicsforPass3.push_back(szTextVal);
							//keldath qa3 -it was &bTemp - whats the diff?
                            pXML->GetNextXmlVal(bTemp);
                            m_abPrereqAndCivicsforPass3.push_back(bTemp);
							gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
						}

						if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
						{
							break;
						}
					}

					gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
				}
			}
		}

		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
/**
 ** End: Unit Civic Prereq
 **/

	pXML->GetChildXmlValByName(szTextVal, "Class");
	m_iUnitClassType = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "Special");
	m_iSpecialUnitType = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "Capture");
	m_iUnitCaptureClassType = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "Combat");
	m_iUnitCombatType = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "Domain");
	m_iDomainType = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "DefaultUnitAI");
	m_iDefaultUnitAIType = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "Invisible");
	m_iInvisibleType = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "SeeInvisible");
	std::vector<CvString> tokens;
	szTextVal.getTokens(",", tokens);
	for(int i = 0; i < (int)tokens.size(); i++)
	{
		int iInvisibleType = pXML->FindInInfoClass(tokens[i]);
		if(iInvisibleType != NO_INVISIBLE)
			m_aiSeeInvisibleTypes.push_back(iInvisibleType);
	}

	pXML->GetChildXmlValByName(szTextVal, "Advisor");
	m_iAdvisorType = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(&m_bAnimal, "bAnimal");
	pXML->GetChildXmlValByName(&m_bFoodProduction, "bFood");
	pXML->GetChildXmlValByName(&m_bNoBadGoodies, "bNoBadGoodies");
	pXML->GetChildXmlValByName(&m_bOnlyDefensive, "bOnlyDefensive");
	// advc.315a:
	pXML->GetChildXmlValByName(&m_bOnlyAttackAnimals, "bOnlyAttackAnimals",false, false);
	// advc.315b:
	pXML->GetChildXmlValByName(&m_bOnlyAttackBarbarians, "bOnlyAttackBarbarians",false, false);
	pXML->GetChildXmlValByName(&m_bNoCapture, "bNoCapture");
	pXML->GetChildXmlValByName(&m_bQuickCombat, "bQuickCombat");
	pXML->GetChildXmlValByName(&m_bRivalTerritory, "bRivalTerritory");
	pXML->GetChildXmlValByName(&m_bMilitaryHappiness, "bMilitaryHappiness");
	pXML->GetChildXmlValByName(&m_bMilitarySupport, "bMilitarySupport");
	pXML->GetChildXmlValByName(&m_bMilitaryProduction, "bMilitaryProduction");
	pXML->GetChildXmlValByName(&m_bPillage, "bPillage");
	pXML->GetChildXmlValByName(&m_bSpy, "bSpy");
	pXML->GetChildXmlValByName(&m_bSabotage, "bSabotage");
	pXML->GetChildXmlValByName(&m_bDestroy, "bDestroy");
	pXML->GetChildXmlValByName(&m_bStealPlans, "bStealPlans");
	pXML->GetChildXmlValByName(&m_bInvestigate, "bInvestigate");
	pXML->GetChildXmlValByName(&m_bCounterSpy, "bCounterSpy");
	pXML->GetChildXmlValByName(&m_bFound, "bFound");
	pXML->GetChildXmlValByName(&m_bGoldenAge, "bGoldenAge");
	pXML->GetChildXmlValByName(&m_bInvisible, "bInvisible");
	pXML->GetChildXmlValByName(&m_bFirstStrikeImmune, "bFirstStrikeImmune");
	pXML->GetChildXmlValByName(&m_bNoDefensiveBonus, "bNoDefensiveBonus");
	pXML->GetChildXmlValByName(&m_bIgnoreBuildingDefense, "bIgnoreBuildingDefense");
	pXML->GetChildXmlValByName(&m_bCanMoveImpassable, "bCanMoveImpassable");
	pXML->GetChildXmlValByName(&m_bCanMoveAllTerrain, "bCanMoveAllTerrain");
	// <advc.opt>
	if (m_bCanMoveAllTerrain)
		m_bCanAnyMoveAllTerrain = true; // </advc.opt>
	pXML->GetChildXmlValByName(&m_bCanMovePeak, "bCanMovePeak", false); //Deliverator
	pXML->GetChildXmlValByName(&m_bFlatMovementCost, "bFlatMovementCost");
	pXML->GetChildXmlValByName(&m_bIgnoreTerrainCost, "bIgnoreTerrainCost");
	pXML->GetChildXmlValByName(&m_bNukeImmune, "bNukeImmune");
	pXML->GetChildXmlValByName(&m_bPrereqBonuses, "bPrereqBonuses");
	pXML->GetChildXmlValByName(&m_bPrereqReligion, "bPrereqReligion");
	pXML->GetChildXmlValByName(&m_bMechanized,"bMechanized",false);
	pXML->GetChildXmlValByName(&m_bRenderBelowWater,"bRenderBelowWater",false);
	pXML->GetChildXmlValByName(&m_bRenderAlways,"bRenderAlways",false);
	pXML->GetChildXmlValByName(&m_bSuicide,"bSuicide");
	pXML->GetChildXmlValByName(&m_bLineOfSight,"bLineOfSight",false);
	pXML->GetChildXmlValByName(&m_bHiddenNationality,"bHiddenNationality",false);
	pXML->GetChildXmlValByName(&m_bAlwaysHostile,"bAlwaysHostile",false);
	pXML->GetChildXmlValByName(&m_bNoRevealMap,"bNoRevealMap",false);

	pXML->SetVariableListTagPair(&m_pbUpgradeUnitClass, "UnitClassUpgrades", GC.getNumUnitClassInfos());
	pXML->SetVariableListTagPair(&m_pbTargetUnitClass, "UnitClassTargets", GC.getNumUnitClassInfos());
	pXML->SetVariableListTagPair(&m_pbTargetUnitCombat, "UnitCombatTargets", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_pbDefenderUnitClass, "UnitClassDefenders", GC.getNumUnitClassInfos());
	pXML->SetVariableListTagPair(&m_pbDefenderUnitCombat, "UnitCombatDefenders", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_piFlankingStrikeUnitClass, "FlankingStrikes", GC.getNumUnitClassInfos());
	pXML->SetVariableListTagPair(&m_pbUnitAIType, "UnitAIs", NUM_UNITAI_TYPES);
	pXML->SetVariableListTagPair(&m_pbNotUnitAIType, "NotUnitAIs", NUM_UNITAI_TYPES);

	pXML->SetVariableListTagPair(&m_pbBuilds, "Builds", GC.getNumBuildInfos());

	pXML->SetVariableListTagPair(&m_piReligionSpreads, "ReligionSpreads", GC.getNumReligionInfos());
	pXML->SetVariableListTagPair(&m_piCorporationSpreads, "CorporationSpreads", GC.getNumCorporationInfos());

	CvString* pszTemp = NULL;
	pXML->SetVariableListTagPair(&pszTemp, "TerrainPassableTechs", GC.getNumTerrainInfos());
	if (pszTemp != NULL) // advc.003t
	{
		m_piTerrainPassableTech = new int[GC.getNumTerrainInfos()];
		for (int i = 0; i < GC.getNumTerrainInfos(); ++i)
			m_piTerrainPassableTech[i] = pszTemp[i].IsEmpty() ? NO_TECH : pXML->FindInInfoClass(pszTemp[i]);
		SAFE_DELETE_ARRAY(pszTemp);
	}
	pXML->SetVariableListTagPair(&pszTemp, "FeaturePassableTechs", GC.getNumFeatureInfos());
	if (pszTemp != NULL) // advc.003t
	{
		m_piFeaturePassableTech = new int[GC.getNumFeatureInfos()];
		for (int i = 0; i < GC.getNumFeatureInfos(); ++i)
			m_piFeaturePassableTech[i] = pszTemp[i].IsEmpty() ? NO_TECH : pXML->FindInInfoClass(pszTemp[i]);
		SAFE_DELETE_ARRAY(pszTemp);
	}
	pXML->SetVariableListTagPair(&m_pbGreatPeoples, "GreatPeoples", GC.getNumSpecialistInfos());

	pXML->SetVariableListTagPair(&m_pbBuildings, "Buildings", GC.getNumBuildingInfos());
	//pXML->SetVariableListTagPair(&m_pbForceBuildings, "ForceBuildings", GC.getNumBuildingInfos()); // advc.003t

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
	pXML->GetChildXmlValByName(szTextVal, "MaxStartEra",
		""); // f1rpo
	m_iMaxStartEra = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "ForceObsoleteTech",
		""); // f1rpo
	m_iForceObsoleteTech = pXML->FindInInfoClass(szTextVal);
	
	pXML->GetChildXmlValByName(&m_bStateReligion, "bStateReligion", false,
		false); // f1rpo
	pXML->GetChildXmlValByName(szTextVal, "PrereqGameOption",
		""); // f1rpo
	m_iPrereqGameOption = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "NotGameOption",
		""); // f1rpo
	m_iNotGameOption = pXML->FindInInfoClass(szTextVal);
//civics is handles by archid mod code.
//	pXML->SetVariableListTagPair(&m_pbPrereqOrCivics, "PrereqOrCivics", sizeof(GC.getCivicInfo((CivicTypes)0)), GC.getNumCivicInfos());
	
	/*
		keldath qa4 - ok now i got it that revdcm changed the SetVariableListTagPair2, 
		i tried t oconvert it to take advciv changes, but failed...
		so i just renamed it....in the xmlloadset file something to SetVariableListTagPair2
		its silly...but, thats what i came up with......
		seems the second chat var in the paraerters below - for example "TechOverride"
		does not exists in advc SetVariableListTagPair
		doint this rename -a allowed to compile to continue...
	*/
	
	pXML->SetVariableListTagPair(&m_pbPrereqBuildingClass, "PrereqBuildingClasses", GC.getNumBuildingClassInfos());
	
	/*pXML->SetVariableListTagPair(&m_pbPrereqBuildingClass,
			"PrereqBuildingClasses", sizeof(GC.getBuildingClassInfo((BuildingClassTypes)0)),
			GC.getNumBuildingClassInfos());
	*/
	pXML->SetVariableListTagPairRevDCM(&m_piPrereqBuildingClassOverrideTech,
			"PrereqBuildingClasses", /*sizeof(GC.getBuildingClassInfo((BuildingClassTypes)0))*/0,
			GC.getNumBuildingClassInfos(), "TechOverride", GC.getNumTechInfos());
		
	pXML->SetVariableListTagPairRevDCM(&m_piPrereqBuildingClassOverrideEra,
			"PrereqBuildingClasses", /*sizeof(GC.getBuildingClassInfo((BuildingClassTypes)0))*/0,
			GC.getNumBuildingClassInfos(), "EraOverride", GC.getNumEraInfos());

	pXML->SetVariableListTagPair(&m_pbForceObsoleteUnitClass, "ForceObsoleteUnitClasses", 
			/*sizeof(GC.getUnitClassInfo((UnitClassTypes)0)),*/ GC.getNumUnitClassInfos());
	
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	pXML->GetChildXmlValByName(szTextVal, "HolyCity");
	m_iHolyCity = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "ReligionType");
	m_iReligionType = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "StateReligion");
	m_iStateReligion = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "PrereqReligion");
	m_iPrereqReligion = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "PrereqCorporation");
	m_iPrereqCorporation = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "PrereqBuilding");
	m_iPrereqBuilding = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "PrereqTech");
	m_iPrereqAndTech = pXML->FindInInfoClass(szTextVal);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"TechTypes"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			FAssertMsg(GC.getNUM_UNIT_AND_TECH_PREREQS() > 0 , "Allocating zero or less memory in SetGlobalUnitInfo");
			pXML->InitList(&m_piPrereqAndTechs, GC.getNUM_UNIT_AND_TECH_PREREQS(), -1);
			bool bAnyReq = false; // advc.003t
			if (iNumSibs > 0)
			{
				if (pXML->GetChildXmlVal(szTextVal))
				{
					FAssertMsg(iNumSibs <= GC.getNUM_UNIT_AND_TECH_PREREQS(), "There are more siblings than memory allocated for them in SetGlobalUnitInfo");
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

	pXML->GetChildXmlValByName(szTextVal, "BonusType");
	m_iPrereqAndBonus = pXML->FindInInfoClass(szTextVal);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"PrereqBonuses"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			FAssertMsg(GC.getNUM_UNIT_PREREQ_OR_BONUSES() > 0, "Allocating zero or less memory in SetGlobalUnitInfo");
			pXML->InitList(&m_piPrereqOrBonuses, GC.getNUM_UNIT_PREREQ_OR_BONUSES(), -1);
			bool bAnyReq = false; // advc.003t
			if (iNumSibs > 0)
			{
				if (pXML->GetChildXmlVal(szTextVal))
				{
					FAssertMsg(iNumSibs <= GC.getNUM_UNIT_PREREQ_OR_BONUSES() , "There are more siblings than memory allocated for them in SetGlobalUnitInfo");
					for (int j = 0; j < iNumSibs; j++)
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
//Shqype Vicinity Bonus Start
/*	pXML->GetChildXmlValByName(szTextVal, "VicinityBonusType");
	m_iPrereqVicinityBonus = pXML->FindInInfoClass(szTextVal);

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"PrereqVicinityBonuses"))
	{
		if (pXML->SkipToNextVal())
		{
			iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			FAssertMsg((0 < GC.getNUM_UNIT_PREREQ_OR_BONUSES()),"Allocating zero or less memory in SetGlobalUnitInfo");
			pXML->InitList(&m_piPrereqOrVicinityBonuses, GC.getNUM_UNIT_PREREQ_OR_BONUSES(), -1);

			if (0 < iNumSibs)
			{
				if (pXML->GetChildXmlVal(szTextVal))
				{
					FAssertMsg((iNumSibs <= GC.getNUM_UNIT_PREREQ_OR_BONUSES()) , "There are more siblings than memory allocated for them in SetGlobalUnitInfo");
					for (j=0;j<iNumSibs;j++)
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
//Shqype Vicinity Bonus End
    /* <advc.905b> Could implement this like e.g. BonusHappinessChanges, but that
		 would mean storing one int for every (unit type, bonus type) pair. Instead,
		 do sth. similar to the code for PrereqOrBonuses above. */
	//keldath qa4 - why is there an error from f1rpo code?
	if(gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "SpeedBonuses"))
	{
		if(pXML->SkipToNextVal()) {
			pXML->InitList(&m_piSpeedBonuses[0], GC.getNUM_UNIT_PREREQ_OR_BONUSES(), -1);
			pXML->InitList(&m_piSpeedBonuses[1], GC.getNUM_UNIT_PREREQ_OR_BONUSES(), 0);
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if(iNumSibs > 0 && gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
			{
				FAssert(iNumSibs <= GC.getNUM_UNIT_PREREQ_OR_BONUSES());
				for(int j = 0; j < iNumSibs; j++)
				{
					pXML->GetChildXmlValByName(szTextVal, "BonusType");
					int iBonus = pXML->FindInInfoClass(szTextVal);
					if(iBonus > -1)
					{
						m_piSpeedBonuses[0][j] = iBonus;
						int iExtraMoves = 0;
						pXML->GetChildXmlValByName(&iExtraMoves, "iExtraMoves");
						m_piSpeedBonuses[1][j] = iExtraMoves;
					}
					if(!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
						break;
				}
				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	} // </advc.905b>

	pXML->SetVariableListTagPair(&m_piProductionTraits, "ProductionTraits", GC.getNumTraitInfos());

	pXML->SetVariableListTagPair(&m_piFlavorValue, "Flavors", GC.getNumFlavorTypes());

	pXML->GetChildXmlValByName(&m_iAIWeight, "iAIWeight");
	pXML->GetChildXmlValByName(&m_iProductionCost, "iCost");
	pXML->GetChildXmlValByName(&m_iHurryCostModifier, "iHurryCostModifier");
	pXML->GetChildXmlValByName(&m_iAdvancedStartCost, "iAdvancedStartCost", /* advc: */ 100);
	pXML->GetChildXmlValByName(&m_iAdvancedStartCostIncrease, "iAdvancedStartCostIncrease");
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	pXML->GetChildXmlValByName(&m_iNumCitySizeUnitPrereq, "iCitySizeUnitPrereq", 1);
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	pXML->GetChildXmlValByName(&m_iMinAreaSize, "iMinAreaSize");
	pXML->GetChildXmlValByName(&m_iMoves, "iMoves");
	pXML->GetChildXmlValByName(&m_iAirRange, "iAirRange");
	pXML->GetChildXmlValByName(&m_iAirUnitCap, "iAirUnitCap");
	pXML->GetChildXmlValByName(&m_iDropRange, "iDropRange");
	pXML->GetChildXmlValByName(&m_iNukeRange, "iNukeRange");
	pXML->GetChildXmlValByName(&m_iWorkRate, "iWorkRate");
	pXML->GetChildXmlValByName(&m_iBaseDiscover, "iBaseDiscover");
	pXML->GetChildXmlValByName(&m_iDiscoverMultiplier, "iDiscoverMultiplier");
	pXML->GetChildXmlValByName(&m_iBaseHurry, "iBaseHurry");
	pXML->GetChildXmlValByName(&m_iHurryMultiplier, "iHurryMultiplier");
	pXML->GetChildXmlValByName(&m_iBaseTrade, "iBaseTrade");
	pXML->GetChildXmlValByName(&m_iTradeMultiplier, "iTradeMultiplier");
	pXML->GetChildXmlValByName(&m_iGreatWorkCulture, "iGreatWorkCulture");
	pXML->GetChildXmlValByName(&m_iEspionagePoints, "iEspionagePoints");

	pXML->SetVariableListTagPair(&m_pbTerrainImpassable, "TerrainImpassables", GC.getNumTerrainInfos(), false);
	pXML->SetVariableListTagPair(&m_pbFeatureImpassable, "FeatureImpassables", GC.getNumFeatureInfos(), false);

	pXML->GetChildXmlValByName(&m_iCombat, "iCombat");
	pXML->GetChildXmlValByName(&m_iCombatLimit, "iCombatLimit");
	pXML->GetChildXmlValByName(&m_iAirCombat, "iAirCombat");
	pXML->GetChildXmlValByName(&m_iAirCombatLimit, "iAirCombatLimit");
	pXML->GetChildXmlValByName(&m_iXPValueAttack, "iXPValueAttack");
	pXML->GetChildXmlValByName(&m_iXPValueDefense, "iXPValueDefense");
	pXML->GetChildXmlValByName(&m_iFirstStrikes, "iFirstStrikes");
	pXML->GetChildXmlValByName(&m_iChanceFirstStrikes, "iChanceFirstStrikes");
	pXML->GetChildXmlValByName(&m_iInterceptionProbability, "iInterceptionProbability");
	pXML->GetChildXmlValByName(&m_iEvasionProbability, "iEvasionProbability");
	pXML->GetChildXmlValByName(&m_iWithdrawalProbability, "iWithdrawalProb");
	pXML->GetChildXmlValByName(&m_iCollateralDamage, "iCollateralDamage");
	pXML->GetChildXmlValByName(&m_iCollateralDamageLimit, "iCollateralDamageLimit");
	pXML->GetChildXmlValByName(&m_iCollateralDamageMaxUnits, "iCollateralDamageMaxUnits");
	pXML->GetChildXmlValByName(&m_iCityAttackModifier, "iCityAttack");
	pXML->GetChildXmlValByName(&m_iCityDefenseModifier, "iCityDefense");
	pXML->GetChildXmlValByName(&m_iAnimalCombatModifier, "iAnimalCombat");
	// advc.315c:
	// advc.315c: re added - aecondary valure - keldath
	pXML->GetChildXmlValByName(&m_iBarbarianCombatModifier, "iBarbarianCombat", 0);
	pXML->GetChildXmlValByName(&m_iHillsAttackModifier, "iHillsAttack");
	pXML->GetChildXmlValByName(&m_iHillsDefenseModifier, "iHillsDefense");

	pXML->SetVariableListTagPair(&m_pbTerrainNative, "TerrainNatives", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_pbFeatureNative, "FeatureNatives", GC.getNumFeatureInfos());

	pXML->SetVariableListTagPair(&m_piTerrainAttackModifier, "TerrainAttacks", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piTerrainDefenseModifier, "TerrainDefenses", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piFeatureAttackModifier, "FeatureAttacks", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_piFeatureDefenseModifier, "FeatureDefenses", GC.getNumFeatureInfos());

	pXML->SetVariableListTagPair(&m_piUnitClassAttackModifier, "UnitClassAttackMods", GC.getNumUnitClassInfos());
	pXML->SetVariableListTagPair(&m_piUnitClassDefenseModifier, "UnitClassDefenseMods", GC.getNumUnitClassInfos());

	pXML->SetVariableListTagPair(&m_piUnitCombatModifier, "UnitCombatMods", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_piUnitCombatCollateralImmune, "UnitCombatCollateralImmunes", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_piDomainModifier, "DomainMods", NUM_DOMAIN_TYPES);

	pXML->SetVariableListTagPair(&m_piBonusProductionModifier, "BonusProductionModifiers", GC.getNumBonusInfos());

	pXML->GetChildXmlValByName(&m_iBombRate, "iBombRate");
	pXML->GetChildXmlValByName(&m_iBombardRate, "iBombardRate");

	pXML->GetChildXmlValByName(szTextVal, "SpecialCargo");
	m_iSpecialCargo = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "DomainCargo");
	m_iDomainCargo = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(&m_iCargoSpace, "iCargo");
	pXML->GetChildXmlValByName(&m_iConscriptionValue, "iConscription");
	pXML->GetChildXmlValByName(&m_iCultureGarrisonValue, "iCultureGarrison");
	pXML->GetChildXmlValByName(&m_iExtraCost, "iExtraCost");
	pXML->GetChildXmlValByName(&m_iAssetValue, "iAsset");
	pXML->GetChildXmlValByName(&m_iPowerValue, "iPower");

	// Read the mesh groups elements
	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"UnitMeshGroups"))
	{
		pXML->GetChildXmlValByName( &m_iGroupSize, "iGroupSize");
		int iIndexVal = gDLL->getXMLIFace()->NumOfChildrenByTagName(pXML->GetXML(), "UnitMeshGroup");
		m_iGroupDefinitions = iIndexVal;
		// advc.006b: GetChildXmlValByName can't handle comments. Assert added to warn about that.
		FAssertMsg(m_iGroupDefinitions > 0, "XML comment inside UnitMeshGroups?");
		m_piUnitGroupRequired = new int[ iIndexVal ];
		pXML->GetChildXmlValByName(&m_iUnitMeleeWaveSize, "iMeleeWaveSize");
		pXML->GetChildXmlValByName(&m_iUnitRangedWaveSize, "iRangedWaveSize");
		pXML->GetChildXmlValByName(&m_fUnitMaxSpeed, "fMaxSpeed");
		pXML->GetChildXmlValByName(&m_fUnitPadTime, "fPadTime");
		m_paszEarlyArtDefineTags = new CvString[ iIndexVal ];
		m_paszLateArtDefineTags = new CvString[ iIndexVal ];
		m_paszMiddleArtDefineTags = new CvString[ iIndexVal ];

		if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "UnitMeshGroup"))
		{
			for (int k = 0; k < iIndexVal; k++)
			{
				pXML->GetChildXmlValByName(&m_piUnitGroupRequired[k], "iRequired");
				pXML->GetChildXmlValByName(szTextVal, "EarlyArtDefineTag");
				setEarlyArtDefineTag(k, szTextVal);
				pXML->GetChildXmlValByName(szTextVal, "LateArtDefineTag", /* advc.006b: */ "");
				setLateArtDefineTag(k, szTextVal);
				pXML->GetChildXmlValByName(szTextVal, "MiddleArtDefineTag", /* advc.006b: */ "");
				setMiddleArtDefineTag(k, szTextVal);
				gDLL->getXMLIFace()->NextSibling(pXML->GetXML());
			}
			gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	pXML->GetChildXmlValByName(m_szFormationType, "FormationType");

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"UniqueNames"))
	{
		pXML->SetStringList(&m_paszUnitNames, &m_iNumUnitNames);
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}

	pXML->SetVariableListTagPair(&m_pbFreePromotions, "FreePromotions", GC.getNumPromotionInfos());

	pXML->GetChildXmlValByName(szTextVal, "LeaderPromotion");
	m_iLeaderPromotion = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(&m_iLeaderExperience, "iLeaderExperience");

	updateArtDefineButton();

	return true;
}

/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
bool CvUnitInfo::readPass3()
{
	m_pbPrereqOrCivics = new bool[GC.getNumCivicInfos()];
	for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		m_pbPrereqOrCivics[iI] = false;
	}
	if (!m_abPrereqOrCivicsforPass3.empty() && !m_aszPrereqOrCivicsforPass3.empty())
	{
		int iNumLoad = m_abPrereqOrCivicsforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			//FAssertMsg(GC.getInfoTypeForString(m_aszPrereqOrCivicsforPass3[iI]) >= 0, "Warning, about to leak memory in CvBuildingInfo::readPass3");
			int iTempIndex = GC.getInfoTypeForString(m_aszPrereqOrCivicsforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCivicInfos())
				m_pbPrereqOrCivics[iTempIndex] = m_abPrereqOrCivicsforPass3[iI];
		}
		m_aszPrereqOrCivicsforPass3.clear();
		m_abPrereqOrCivicsforPass3.clear();
	}
	m_pbPrereqAndCivics = new bool[GC.getNumCivicInfos()];
    for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
        m_pbPrereqAndCivics[iI] = false;
	}
	if (!m_abPrereqAndCivicsforPass3.empty() && !m_aszPrereqAndCivicsforPass3.empty())
	{
		int iNumLoad = m_abPrereqAndCivicsforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			//FAssertMsg(GC.getInfoTypeForString(m_aszPrereqAndCivicsforPass3[iI]) >= 0, "Warning, about to leak memory in CvBuildingInfo::readPass3");
			int iTempIndex = GC.getInfoTypeForString(m_aszPrereqAndCivicsforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCivicInfos())
				m_pbPrereqAndCivics[iTempIndex] = m_abPrereqAndCivicsforPass3[iI];
		}
		m_aszPrereqAndCivicsforPass3.clear();
		m_abPrereqAndCivicsforPass3.clear();
	}
	
	return true;
}

/**
 ** End: Unit Civic Prereq
 **/

const TCHAR* CvUnitArtStyleInfo::getEarlyArtDefineTag(int i, int j) const
{
	FAssertBounds(0, GC.getNumUnitInfos(), j);
	FAssertBounds(0, GC.getInfo((UnitTypes)j).getGroupDefinitions(), i);
	for (ArtDefineArray::const_iterator it = m_azEarlyArtDefineTags.begin(); it != m_azEarlyArtDefineTags.end(); ++it)
	{
		if (it->iMeshIndex == i && it->iUnitType == j)
			return it->szTag;
	}
	return NULL;
}

void CvUnitArtStyleInfo::setEarlyArtDefineTag(int i, int j, const TCHAR* szVal)
{
	FAssertBounds(0, GC.getNumUnitInfos(), j);
	FAssertBounds(0, GC.getInfo((UnitTypes)j).getGroupDefinitions(), i);
	for (ArtDefineArray::iterator it = m_azEarlyArtDefineTags.begin(); it != m_azEarlyArtDefineTags.end(); ++it)
	{
		if (it->iMeshIndex == i && it->iUnitType == j)
		{
			it->szTag = szVal;
			return;
		}
	}
	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azEarlyArtDefineTags.push_back(kTag);
}

const TCHAR* CvUnitArtStyleInfo::getLateArtDefineTag(int i, int j) const
{
	FAssertBounds(0, GC.getNumUnitInfos(), j);
	FAssertBounds(0, GC.getInfo((UnitTypes)j).getGroupDefinitions(), i);
	for (ArtDefineArray::const_iterator it = m_azLateArtDefineTags.begin(); it != m_azLateArtDefineTags.end(); ++it)
	{
		if (it->iMeshIndex == i && it->iUnitType == j)
			return it->szTag;
	}
	return NULL;
}

void CvUnitArtStyleInfo::setLateArtDefineTag(int i, int j, const TCHAR* szVal)
{
	FAssertBounds(0, GC.getNumUnitInfos(), j);
	FAssertBounds(0, GC.getInfo((UnitTypes)j).getGroupDefinitions(), i);
	for (ArtDefineArray::iterator it = m_azLateArtDefineTags.begin(); it != m_azLateArtDefineTags.end(); ++it)
	{
		if (it->iMeshIndex == i && it->iUnitType == j)
		{
			it->szTag = szVal;
			return;
		}
	}
	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azLateArtDefineTags.push_back(kTag);
}

const TCHAR* CvUnitArtStyleInfo::getMiddleArtDefineTag(int i, int j) const
{
	FAssertBounds(0, GC.getNumUnitInfos(), j);
	FAssertBounds(0, GC.getInfo((UnitTypes)j).getGroupDefinitions(), i);
	for (ArtDefineArray::const_iterator it = m_azMiddleArtDefineTags.begin(); it != m_azMiddleArtDefineTags.end(); ++it)
	{
		if (it->iMeshIndex == i && it->iUnitType == j)
			return it->szTag;
	}
	return NULL;
}

void CvUnitArtStyleInfo::setMiddleArtDefineTag(int i, int j, const TCHAR* szVal)
{
	FAssertBounds(0, GC.getNumUnitInfos(), j);
	FAssertBounds(0, GC.getInfo((UnitTypes)j).getGroupDefinitions(), i);
	for (ArtDefineArray::iterator it = m_azMiddleArtDefineTags.begin(); it != m_azMiddleArtDefineTags.end(); ++it)
	{
		if (it->iMeshIndex == i && it->iUnitType == j)
		{
			it->szTag = szVal;
			return;
		}
	}
	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azMiddleArtDefineTags.push_back(kTag);
}

bool CvUnitArtStyleInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
		return false;

	if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(), "StyleUnits"))
	{
		if (pXML->SkipToNextVal())
		{
			int iNumSibs = gDLL->getXMLIFace()->GetNumChildren(pXML->GetXML());
			if (gDLL->getXMLIFace()->SetToChild(pXML->GetXML()))
			{
				if (iNumSibs > 0)
				{
					for (int i = 0; i < iNumSibs; i++)
					{
						CvString szTextVal;
						pXML->GetChildXmlValByName(szTextVal, "UnitType");
						int iIndex = pXML->FindInInfoClass(szTextVal); // Unit index

						if (iIndex > -1)
						{
							int iMesh = GC.getInfo((UnitTypes)iIndex).getGroupDefinitions(); // Mesh index

							if (gDLL->getXMLIFace()->SetToChildByTagName(pXML->GetXML(),"UnitMeshGroup"))
							{
								for (int j = 0; j < iMesh; j++)
								{
									// Overwrite with the Style Art
									pXML->GetChildXmlValByName(szTextVal, "EarlyArtDefineTag");
									setEarlyArtDefineTag(j, iIndex, szTextVal);
									pXML->GetChildXmlValByName(szTextVal, "LateArtDefineTag");
									setLateArtDefineTag(j, iIndex, szTextVal);
									pXML->GetChildXmlValByName(szTextVal, "MiddleArtDefineTag");
									setMiddleArtDefineTag(j, iIndex, szTextVal);

									if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
										break;
								}
								gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
							}
						}

						if (!gDLL->getXMLIFace()->NextSibling(pXML->GetXML()))
							break;
					}
				}
				gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
			}
		}
		gDLL->getXMLIFace()->SetToParent(pXML->GetXML());
	}
	return true;
}

CvUnitClassInfo::CvUnitClassInfo() :
m_iMaxGlobalInstances(0),
m_iMaxTeamInstances(0),
m_iMaxPlayerInstances(0),
m_iInstanceCostModifier(0),
m_iDefaultUnitIndex(NO_UNIT)
{}

bool CvUnitClassInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->GetChildXmlValByName(&m_iMaxGlobalInstances, "iMaxGlobalInstances");
	pXML->GetChildXmlValByName(&m_iMaxTeamInstances, "iMaxTeamInstances");
	pXML->GetChildXmlValByName(&m_iMaxPlayerInstances, "iMaxPlayerInstances");
	pXML->GetChildXmlValByName(&m_iInstanceCostModifier, "iInstanceCostModifier");

	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, "DefaultUnit");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	return true;
}

bool CvUnitClassInfo::readPass3()
{
	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}

	m_iDefaultUnitIndex = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);
	m_aszExtraXMLforPass3.clear();

	return true;
}

CvSpecialUnitInfo::CvSpecialUnitInfo() :
m_bValid(false),
m_bCityLoad(false),
m_pbCarrierUnitAITypes(NULL),
m_piProductionTraits(NULL)
{}

CvSpecialUnitInfo::~CvSpecialUnitInfo()
{
	SAFE_DELETE_ARRAY(m_pbCarrierUnitAITypes);
	SAFE_DELETE_ARRAY(m_piProductionTraits);
}

bool CvSpecialUnitInfo::isValid() const
{
	return m_bValid;
}

bool CvSpecialUnitInfo::isCityLoad() const
{
	return m_bCityLoad;
}

bool CvSpecialUnitInfo::isCarrierUnitAIType(int i) const
{
	FAssertBounds(0, NUM_UNITAI_TYPES, i);
	return m_pbCarrierUnitAITypes ? m_pbCarrierUnitAITypes[i]
			: false; // advc.003t: was -1 (which equals true)
}

int CvSpecialUnitInfo::getProductionTraits(int i) const
{
	FAssertBounds(0, GC.getNumTraitInfos(), i);
	return m_piProductionTraits ? m_piProductionTraits[i]
			: 0; // advc.003t: Was -1. This is the production discount percentage.
}

bool CvSpecialUnitInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
		return false;

	pXML->GetChildXmlValByName(&m_bValid, "bValid");
	pXML->GetChildXmlValByName(&m_bCityLoad, "bCityLoad");
	pXML->SetVariableListTagPair(&m_pbCarrierUnitAITypes, "CarrierUnitAITypes", NUM_UNITAI_TYPES);
	pXML->SetVariableListTagPair(&m_piProductionTraits, "ProductionTraits", GC.getNumTraitInfos());
	return true;
}

CvPromotionInfo::CvPromotionInfo() :
m_iLayerAnimationPath(ANIMATIONPATH_NONE),
m_iPrereqPromotion(NO_PROMOTION),
m_iPrereqOrPromotion1(NO_PROMOTION),
m_iPrereqOrPromotion2(NO_PROMOTION),
m_iPrereqOrPromotion3(NO_PROMOTION), // K-Mod
m_iTechPrereq(NO_TECH),
m_iStateReligionPrereq(NO_RELIGION),
m_iVisibilityChange(0),
m_iMovesChange(0),
m_iMoveDiscountChange(0),
m_iAirRangeChange(0),
m_iInterceptChange(0),
m_iEvasionChange(0),
m_iWithdrawalChange(0),
m_iCargoChange(0),
m_iCollateralDamageChange(0),
m_iBombardRateChange(0),
m_iFirstStrikesChange(0),
m_iChanceFirstStrikesChange(0),
m_iEnemyHealChange(0),
m_iNeutralHealChange(0),
m_iFriendlyHealChange(0),
m_iSameTileHealChange(0),
m_iAdjacentTileHealChange(0),
m_iCombatPercent(0),
m_iCityAttackPercent(0),
m_iCityDefensePercent(0),
m_iHillsAttackPercent(0),
m_iHillsDefensePercent(0),
m_iCommandType(NO_COMMAND),
m_iRevoltProtection(0),
m_iCollateralDamageProtection(0),
m_iPillageChange(0),
m_iUpgradeDiscount(0),
m_iExperiencePercent(0),
m_iKamikazePercent(0),
m_bLeader(false),
m_iBlitz(0), // advc.164
m_bAmphib(false),
//MOD@VET_Andera412_Blocade_Unit-begin1/5
m_bUnblocade(false),
//MOD@VET_Andera412_Blocade_Unit-end1/5
m_bRiver(false),
m_bEnemyRoute(false),
m_bAlwaysHeal(false),
m_bHillsDoubleMove(false),
m_bImmuneToFirstStrikes(false),
m_piTerrainAttackPercent(NULL),
m_piTerrainDefensePercent(NULL),
m_piFeatureAttackPercent(NULL),
m_piFeatureDefensePercent(NULL),
m_piUnitCombatModifierPercent(NULL),
m_piDomainModifierPercent(NULL),
m_pbTerrainDoubleMove(NULL),
m_pbFeatureDoubleMove(NULL),
m_pbUnitCombat(NULL)
{}

CvPromotionInfo::~CvPromotionInfo()
{
	SAFE_DELETE_ARRAY(m_piTerrainAttackPercent);
	SAFE_DELETE_ARRAY(m_piTerrainDefensePercent);
	SAFE_DELETE_ARRAY(m_piFeatureAttackPercent);
	SAFE_DELETE_ARRAY(m_piFeatureDefensePercent);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifierPercent);
	SAFE_DELETE_ARRAY(m_piDomainModifierPercent);
	SAFE_DELETE_ARRAY(m_pbTerrainDoubleMove);
	SAFE_DELETE_ARRAY(m_pbFeatureDoubleMove);
	SAFE_DELETE_ARRAY(m_pbUnitCombat);
}

int CvPromotionInfo::getLayerAnimationPath() const
{
	return m_iLayerAnimationPath;
}

int CvPromotionInfo::getPrereqPromotion() const
{
	return m_iPrereqPromotion;
}

void CvPromotionInfo::setPrereqPromotion(int i)
{
	m_iPrereqPromotion = i;
}

int CvPromotionInfo::getPrereqOrPromotion1() const
{
	return m_iPrereqOrPromotion1;
}

void CvPromotionInfo::setPrereqOrPromotion1(int i)
{
	m_iPrereqOrPromotion1 = i;
}

int CvPromotionInfo::getPrereqOrPromotion2() const
{
	return m_iPrereqOrPromotion2;
}

void CvPromotionInfo::setPrereqOrPromotion2(int i)
{
	m_iPrereqOrPromotion2 = i;
}
// K-Mod, 7/jan/11
int CvPromotionInfo::getPrereqOrPromotion3() const
{
	return m_iPrereqOrPromotion3;
}

void CvPromotionInfo::setPrereqOrPromotion3(int i)
{
	m_iPrereqOrPromotion3 = i;
} // K-Mod end

int CvPromotionInfo::getTechPrereq() const
{
	return m_iTechPrereq;
}

int CvPromotionInfo::getStateReligionPrereq() const
{
	return m_iStateReligionPrereq;
}

int CvPromotionInfo::getVisibilityChange() const
{
	return m_iVisibilityChange;
}

int CvPromotionInfo::getMovesChange() const
{
	return m_iMovesChange;
}

int CvPromotionInfo::getMoveDiscountChange() const
{
	return m_iMoveDiscountChange;
}

int CvPromotionInfo::getAirRangeChange() const
{
	return m_iAirRangeChange;
}

int CvPromotionInfo::getInterceptChange() const
{
	return m_iInterceptChange;
}

int CvPromotionInfo::getEvasionChange() const
{
	return m_iEvasionChange;
}

int CvPromotionInfo::getWithdrawalChange() const
{
	return m_iWithdrawalChange;
}

int CvPromotionInfo::getCargoChange() const
{
	return m_iCargoChange;
}

int CvPromotionInfo::getCollateralDamageChange() const
{
	return m_iCollateralDamageChange;
}

int CvPromotionInfo::getBombardRateChange() const
{
	return m_iBombardRateChange;
}

int CvPromotionInfo::getFirstStrikesChange() const
{
	return m_iFirstStrikesChange;
}

int CvPromotionInfo::getChanceFirstStrikesChange() const
{
	return m_iChanceFirstStrikesChange;
}

int CvPromotionInfo::getEnemyHealChange() const
{
	return m_iEnemyHealChange;
}

int CvPromotionInfo::getNeutralHealChange() const
{
	return m_iNeutralHealChange;
}

int CvPromotionInfo::getFriendlyHealChange() const
{
	return m_iFriendlyHealChange;
}

int CvPromotionInfo::getSameTileHealChange() const
{
	return m_iSameTileHealChange;
}

int CvPromotionInfo::getAdjacentTileHealChange() const
{
	return m_iAdjacentTileHealChange;
}

int CvPromotionInfo::getCombatPercent() const
{
	return m_iCombatPercent;
}

int CvPromotionInfo::getCityAttackPercent() const
{
	return m_iCityAttackPercent;
}

int CvPromotionInfo::getCityDefensePercent() const
{
	return m_iCityDefensePercent;
}

int CvPromotionInfo::getHillsAttackPercent() const
{
	return m_iHillsAttackPercent;
}

int CvPromotionInfo::getHillsDefensePercent() const
{
	return m_iHillsDefensePercent;
}

int CvPromotionInfo::getCommandType() const
{
	return m_iCommandType;
}

void CvPromotionInfo::setCommandType(int iNewType)
{
	m_iCommandType = iNewType;
}

int CvPromotionInfo::getRevoltProtection() const
{
	return m_iRevoltProtection;
}

int CvPromotionInfo::getCollateralDamageProtection() const
{
	return m_iCollateralDamageProtection;
}

int CvPromotionInfo::getPillageChange() const
{
	return m_iPillageChange;
}

int CvPromotionInfo::getUpgradeDiscount() const
{
	return m_iUpgradeDiscount;
}

int CvPromotionInfo::getExperiencePercent() const
{
	return m_iExperiencePercent;
}

int CvPromotionInfo::getKamikazePercent() const
{
	return m_iKamikazePercent;
}

bool CvPromotionInfo::isLeader() const
{
	return m_bLeader;
}
// <advc.164> was bool
int CvPromotionInfo::getBlitz() const
{
	return m_iBlitz;
} // </advc.164>

bool CvPromotionInfo::isAmphib() const
{
	return m_bAmphib;
}

//MOD@VET_Andera412_Blocade_Unit-begin2/5
bool CvPromotionInfo::isUnblocade() const			
{
	return m_bUnblocade;
}
//MOD@VET_Andera412_Blocade_Unit-end2/5

bool CvPromotionInfo::isRiver() const		
{
	return m_bRiver;
}

bool CvPromotionInfo::isEnemyRoute() const
{
	return m_bEnemyRoute;
}

bool CvPromotionInfo::isAlwaysHeal() const
{
	return m_bAlwaysHeal;
}

bool CvPromotionInfo::isHillsDoubleMove() const
{
	return m_bHillsDoubleMove;
}

bool CvPromotionInfo::isImmuneToFirstStrikes() const
{
	return m_bImmuneToFirstStrikes;
}

const TCHAR* CvPromotionInfo::getSound() const
{
	return m_szSound;
}

void CvPromotionInfo::setSound(const TCHAR* szVal)
{
	m_szSound = szVal;
}

int CvPromotionInfo::getTerrainAttackPercent(int i) const
{
	FAssertBounds(0, GC.getNumTerrainInfos(), i);
	return m_piTerrainAttackPercent ? m_piTerrainAttackPercent[i] : 0; // advc.003t
}

int CvPromotionInfo::getTerrainDefensePercent(int i) const
{
	FAssertBounds(0, GC.getNumTerrainInfos(), i);
	return m_piTerrainDefensePercent ? m_piTerrainDefensePercent[i] : 0; // advc.003t
}

int CvPromotionInfo::getFeatureAttackPercent(int i) const
{
	FAssertBounds(0, GC.getNumFeatureInfos(), i);
	return m_piFeatureAttackPercent ? m_piFeatureAttackPercent[i] : 0; // advc.003t
}

int CvPromotionInfo::getFeatureDefensePercent(int i) const
{
	FAssertBounds(0, GC.getNumFeatureInfos(), i);
	return m_piFeatureDefensePercent ? m_piFeatureDefensePercent[i] : 0; // advc.003t
}

int CvPromotionInfo::getUnitCombatModifierPercent(int i) const
{
	FAssertBounds(0, GC.getNumUnitCombatInfos(), i);
	return m_piUnitCombatModifierPercent ? m_piUnitCombatModifierPercent[i] : 0; // advc.003t
}

int CvPromotionInfo::getDomainModifierPercent(int i) const
{
	FAssertBounds(0, NUM_DOMAIN_TYPES, i);
	return m_piDomainModifierPercent ? m_piDomainModifierPercent[i] : 0; // advc.003t
}

bool CvPromotionInfo::getTerrainDoubleMove(int i) const
{
	FAssertBounds(0, GC.getNumTerrainInfos(), i);
	return m_pbTerrainDoubleMove ? m_pbTerrainDoubleMove[i] : false;
}

bool CvPromotionInfo::getFeatureDoubleMove(int i) const
{
	FAssertBounds(0, GC.getNumFeatureInfos(), i);
	return m_pbFeatureDoubleMove ? m_pbFeatureDoubleMove[i] : false;
}

bool CvPromotionInfo::getUnitCombat(int i) const
{
	FAssertBounds(0, GC.getNumUnitCombatInfos(), i);
	return m_pbUnitCombat ? m_pbUnitCombat[i] : false;
}
#if SERIALIZE_CVINFOS
void CvPromotionInfo::read(FDataStreamBase* stream)
{
	CvHotkeyInfo::read(stream);
	uint uiFlag=0;
	stream->Read(&uiFlag);

	stream->Read(&m_iLayerAnimationPath);
	stream->Read(&m_iPrereqPromotion);
	stream->Read(&m_iPrereqOrPromotion1);
	stream->Read(&m_iPrereqOrPromotion2);
	stream->Read(&m_iPrereqOrPromotion3); // K-Mod
	stream->Read(&m_iTechPrereq);
	stream->Read(&m_iStateReligionPrereq);
	stream->Read(&m_iVisibilityChange);
	stream->Read(&m_iMovesChange);
	stream->Read(&m_iMoveDiscountChange);
	stream->Read(&m_iAirRangeChange);
	stream->Read(&m_iInterceptChange);
	stream->Read(&m_iEvasionChange);
	stream->Read(&m_iWithdrawalChange);
	stream->Read(&m_iCargoChange);
	stream->Read(&m_iCollateralDamageChange);
	stream->Read(&m_iBombardRateChange);
	stream->Read(&m_iFirstStrikesChange);
	stream->Read(&m_iChanceFirstStrikesChange);
	stream->Read(&m_iEnemyHealChange);
	stream->Read(&m_iNeutralHealChange);
	stream->Read(&m_iFriendlyHealChange);
	stream->Read(&m_iSameTileHealChange);
	stream->Read(&m_iAdjacentTileHealChange);
	stream->Read(&m_iCombatPercent);
	stream->Read(&m_iCityAttackPercent);
	stream->Read(&m_iCityDefensePercent);
	stream->Read(&m_iHillsAttackPercent);
	stream->Read(&m_iHillsDefensePercent);
	stream->Read(&m_iCommandType);
	stream->Read(&m_iRevoltProtection);
	stream->Read(&m_iCollateralDamageProtection);
	stream->Read(&m_iPillageChange);
	stream->Read(&m_iUpgradeDiscount);
	stream->Read(&m_iExperiencePercent);
	stream->Read(&m_iKamikazePercent);
	stream->Read(&m_bLeader);
	// <advc.164>
	if(uiFlag >= 1)
		stream->Read(&m_iBlitz);
	else
	{
		bool bTmp=false;
		stream->Read(&bTmp);
		if(bTmp)
			m_iBlitz = 1;
		else m_iBlitz = 0;
	} // </advc.164>
	stream->Read(&m_bAmphib);
//MOD@VET_Andera412_Blocade_Unit-begin3/5
	stream->Read(&m_bUnblocade);
//MOD@VET_Andera412_Blocade_Unit-end3/5	
	stream->Read(&m_bRiver);
	stream->Read(&m_bEnemyRoute);
	stream->Read(&m_bAlwaysHeal);
	stream->Read(&m_bHillsDoubleMove);
	stream->Read(&m_bImmuneToFirstStrikes);
	stream->ReadString(m_szSound);
	SAFE_DELETE_ARRAY(m_piTerrainAttackPercent);
	m_piTerrainAttackPercent = new int[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_piTerrainAttackPercent);
	SAFE_DELETE_ARRAY(m_piTerrainDefensePercent);
	m_piTerrainDefensePercent = new int[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_piTerrainDefensePercent);
	SAFE_DELETE_ARRAY(m_piFeatureAttackPercent);
	m_piFeatureAttackPercent = new int[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_piFeatureAttackPercent);
	SAFE_DELETE_ARRAY(m_piFeatureDefensePercent);
	m_piFeatureDefensePercent = new int[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_piFeatureDefensePercent);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifierPercent);
	m_piUnitCombatModifierPercent = new int[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_piUnitCombatModifierPercent);
	SAFE_DELETE_ARRAY(m_piDomainModifierPercent);
	m_piDomainModifierPercent = new int[NUM_DOMAIN_TYPES];
	stream->Read(NUM_DOMAIN_TYPES, m_piDomainModifierPercent);
	SAFE_DELETE_ARRAY(m_pbTerrainDoubleMove);
	m_pbTerrainDoubleMove = new bool[GC.getNumTerrainInfos()];
	stream->Read(GC.getNumTerrainInfos(), m_pbTerrainDoubleMove);
	SAFE_DELETE_ARRAY(m_pbFeatureDoubleMove);
	m_pbFeatureDoubleMove = new bool[GC.getNumFeatureInfos()];
	stream->Read(GC.getNumFeatureInfos(), m_pbFeatureDoubleMove);
	SAFE_DELETE_ARRAY(m_pbUnitCombat);
	m_pbUnitCombat = new bool[GC.getNumUnitCombatInfos()];
	stream->Read(GC.getNumUnitCombatInfos(), m_pbUnitCombat);
}

void CvPromotionInfo::write(FDataStreamBase* stream)
{
	CvHotkeyInfo::write(stream);
	uint uiFlag = 0;
	uiFlag = 1; // advc.164
	stream->Write(uiFlag);

	stream->Write(m_iLayerAnimationPath);
	stream->Write(m_iPrereqPromotion);
	stream->Write(m_iPrereqOrPromotion1);
	stream->Write(m_iPrereqOrPromotion2);
	stream->Write(m_iPrereqOrPromotion3); // K-Mod
	stream->Write(m_iTechPrereq);
	stream->Write(m_iStateReligionPrereq);
	stream->Write(m_iVisibilityChange);
	stream->Write(m_iMovesChange);
	stream->Write(m_iMoveDiscountChange);
	stream->Write(m_iAirRangeChange);
	stream->Write(m_iInterceptChange);
	stream->Write(m_iEvasionChange);
	stream->Write(m_iWithdrawalChange);
	stream->Write(m_iCargoChange);
	stream->Write(m_iCollateralDamageChange);
	stream->Write(m_iBombardRateChange);
	stream->Write(m_iFirstStrikesChange);
	stream->Write(m_iChanceFirstStrikesChange);
	stream->Write(m_iEnemyHealChange);
	stream->Write(m_iNeutralHealChange);
	stream->Write(m_iFriendlyHealChange);
	stream->Write(m_iSameTileHealChange);
	stream->Write(m_iAdjacentTileHealChange);
	stream->Write(m_iCombatPercent);
	stream->Write(m_iCityAttackPercent);
	stream->Write(m_iCityDefensePercent);
	stream->Write(m_iHillsAttackPercent);
	stream->Write(m_iHillsDefensePercent);
	stream->Write(m_iCommandType);
	stream->Write(m_iRevoltProtection);
	stream->Write(m_iCollateralDamageProtection);
	stream->Write(m_iPillageChange);
	stream->Write(m_iUpgradeDiscount);
	stream->Write(m_iExperiencePercent);
	stream->Write(m_iKamikazePercent);
	stream->Write(m_bLeader);
	stream->Write(m_iBlitz); // advc.164
	stream->Write(m_bAmphib);
//MOD@VET_Andera412_Blocade_Unit-begin4/5
	stream->Write(m_bUnblocade);
//MOD@VET_Andera412_Blocade_Unit-end4/5
	stream->Write(m_bRiver);
	stream->Write(m_bEnemyRoute);
	stream->Write(m_bAlwaysHeal);
	stream->Write(m_bHillsDoubleMove);
	stream->Write(m_bImmuneToFirstStrikes);
	stream->WriteString(m_szSound);
	stream->Write(GC.getNumTerrainInfos(), m_piTerrainAttackPercent);
	stream->Write(GC.getNumTerrainInfos(), m_piTerrainDefensePercent);
	stream->Write(GC.getNumFeatureInfos(), m_piFeatureAttackPercent);
	stream->Write(GC.getNumFeatureInfos(), m_piFeatureDefensePercent);
	stream->Write(GC.getNumUnitCombatInfos(), m_piUnitCombatModifierPercent);
	stream->Write(NUM_DOMAIN_TYPES, m_piDomainModifierPercent);
	stream->Write(GC.getNumTerrainInfos(), m_pbTerrainDoubleMove);
	stream->Write(GC.getNumFeatureInfos(), m_pbFeatureDoubleMove);
	stream->Write(GC.getNumUnitCombatInfos(), m_pbUnitCombat);
}
#endif
bool CvPromotionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvHotkeyInfo::read(pXML))
		return false;

	CvString szTextVal;

	pXML->GetChildXmlValByName(szTextVal, "Sound");
	setSound(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "LayerAnimationPath");
	m_iLayerAnimationPath = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "TechPrereq");
	m_iTechPrereq = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, "StateReligionPrereq");
	m_iStateReligionPrereq = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(&m_bLeader, "bLeader");
	if (m_bLeader)
		m_bGraphicalOnly = true;  // don't show in Civilopedia list of promotions
	pXML->GetChildXmlValByName(&m_iBlitz, "iBlitz"); // advc.164
	pXML->GetChildXmlValByName(&m_bAmphib, "bAmphib");
//MOD@VET_Andera412_Blocade_Unit-begin5/5
	pXML->GetChildXmlValByName(&m_bUnblocade, "bUnblocade", false);
//MOD@VET_Andera412_Blocade_Unit-end5/5
	pXML->GetChildXmlValByName(&m_bRiver, "bRiver");
	pXML->GetChildXmlValByName(&m_bEnemyRoute, "bEnemyRoute");
	pXML->GetChildXmlValByName(&m_bAlwaysHeal, "bAlwaysHeal");
	pXML->GetChildXmlValByName(&m_bHillsDoubleMove, "bHillsDoubleMove");
	pXML->GetChildXmlValByName(&m_bImmuneToFirstStrikes, "bImmuneToFirstStrikes");
	pXML->GetChildXmlValByName(&m_iVisibilityChange, "iVisibilityChange");
	pXML->GetChildXmlValByName(&m_iMovesChange, "iMovesChange");
	pXML->GetChildXmlValByName(&m_iMoveDiscountChange, "iMoveDiscountChange");
	pXML->GetChildXmlValByName(&m_iAirRangeChange, "iAirRangeChange");
	pXML->GetChildXmlValByName(&m_iInterceptChange, "iInterceptChange");
	pXML->GetChildXmlValByName(&m_iEvasionChange, "iEvasionChange");
	pXML->GetChildXmlValByName(&m_iWithdrawalChange, "iWithdrawalChange");
	pXML->GetChildXmlValByName(&m_iCargoChange, "iCargoChange");
	pXML->GetChildXmlValByName(&m_iCollateralDamageChange, "iCollateralDamageChange");
	pXML->GetChildXmlValByName(&m_iBombardRateChange, "iBombardRateChange");
	pXML->GetChildXmlValByName(&m_iFirstStrikesChange, "iFirstStrikesChange");
	pXML->GetChildXmlValByName(&m_iChanceFirstStrikesChange, "iChanceFirstStrikesChange");
	pXML->GetChildXmlValByName(&m_iEnemyHealChange, "iEnemyHealChange");
	pXML->GetChildXmlValByName(&m_iNeutralHealChange, "iNeutralHealChange");
	pXML->GetChildXmlValByName(&m_iFriendlyHealChange, "iFriendlyHealChange");
	pXML->GetChildXmlValByName(&m_iSameTileHealChange, "iSameTileHealChange");
	pXML->GetChildXmlValByName(&m_iAdjacentTileHealChange, "iAdjacentTileHealChange");
	pXML->GetChildXmlValByName(&m_iCombatPercent, "iCombatPercent");
	pXML->GetChildXmlValByName(&m_iCityAttackPercent, "iCityAttack");
	pXML->GetChildXmlValByName(&m_iCityDefensePercent, "iCityDefense");
	pXML->GetChildXmlValByName(&m_iHillsAttackPercent, "iHillsAttack");
	pXML->GetChildXmlValByName(&m_iHillsDefensePercent, "iHillsDefense");
	pXML->GetChildXmlValByName(&m_iRevoltProtection, "iRevoltProtection");
	pXML->GetChildXmlValByName(&m_iCollateralDamageProtection, "iCollateralDamageProtection");
	pXML->GetChildXmlValByName(&m_iPillageChange, "iPillageChange");
	pXML->GetChildXmlValByName(&m_iUpgradeDiscount, "iUpgradeDiscount");
	pXML->GetChildXmlValByName(&m_iExperiencePercent, "iExperiencePercent");
	pXML->GetChildXmlValByName(&m_iKamikazePercent, "iKamikazePercent");
	pXML->SetVariableListTagPair(&m_piTerrainAttackPercent, "TerrainAttacks", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piTerrainDefensePercent, "TerrainDefenses", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piFeatureAttackPercent, "FeatureAttacks", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_piFeatureDefensePercent, "FeatureDefenses", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_piUnitCombatModifierPercent, "UnitCombatMods", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_piDomainModifierPercent, "DomainMods", NUM_DOMAIN_TYPES);
	pXML->SetVariableListTagPair(&m_pbTerrainDoubleMove, "TerrainDoubleMoves", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_pbFeatureDoubleMove, "FeatureDoubleMoves", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_pbUnitCombat, "UnitCombats", GC.getNumUnitCombatInfos());

	return true;
}

bool CvPromotionInfo::readPass2(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	pXML->GetChildXmlValByName(szTextVal, "PromotionPrereq");
	m_iPrereqPromotion = GC.getInfoTypeForString(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "PromotionPrereqOr1");
	m_iPrereqOrPromotion1 = GC.getInfoTypeForString(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, "PromotionPrereqOr2");
	m_iPrereqOrPromotion2 = GC.getInfoTypeForString(szTextVal);
	// K-Mod, 7/jan/11
	pXML->GetChildXmlValByName(szTextVal, "PromotionPrereqOr3","");//keldath default :""
	m_iPrereqOrPromotion3 = GC.getInfoTypeForString(szTextVal); // K-Mod end

	return true;
}

CvEspionageMissionInfo::CvEspionageMissionInfo() // <kmodx>
	: m_iCost(0),
	m_bIsPassive(false),
	m_bIsTwoPhases(false),
	m_bTargetsCity(false),
	m_bSelectPlot(false),
	m_iTechPrereq(NO_TECH),
	m_iVisibilityLevel(0),
	m_bInvestigateCity(false),
	m_bSeeDemographics(false),
	m_bNoActiveMissions(false),
	m_bSeeResearch(false),
	m_bDestroyImprovement(false),
	m_iDestroyBuildingCostFactor(0),
	m_iDestroyUnitCostFactor(0),
	m_iDestroyProjectCostFactor(0),
	m_iDestroyProductionCostFactor(0),
	m_iBuyUnitCostFactor(0),
	m_iBuyCityCostFactor(0),
	m_iStealTreasuryTypes(0),
	m_iCityInsertCultureAmountFactor(0),
	m_iCityInsertCultureCostFactor(0),
	m_iCityPoisonWaterCounter(0),
	m_iCityUnhappinessCounter(0),
	m_iCityRevoltCounter(0),
	m_iBuyTechCostFactor(0),
	m_iSwitchCivicCostFactor(0),
	m_iSwitchReligionCostFactor(0),
	m_iPlayerAnarchyCounter(0),
	m_iCounterespionageNumTurns(0),
	m_iCounterespionageMod(0),
	m_iDifficultyMod(0), // </kmodx>
	m_bReturnToCapital(false)
{}

bool CvEspionageMissionInfo::isTwoPhases() const
{
	return m_bIsTwoPhases;
}

bool CvEspionageMissionInfo::isTargetsCity() const
{
	return m_bTargetsCity;
}

bool CvEspionageMissionInfo::isSelectPlot() const
{
	return m_bSelectPlot;
}

int CvEspionageMissionInfo::getTechPrereq() const
{
	return m_iTechPrereq;
}

int CvEspionageMissionInfo::getVisibilityLevel() const
{
	return m_iVisibilityLevel;
}

bool CvEspionageMissionInfo::isInvestigateCity() const
{
	return m_bInvestigateCity;
}

bool CvEspionageMissionInfo::isSeeDemographics() const
{
	return m_bSeeDemographics;
}

bool CvEspionageMissionInfo::isNoActiveMissions() const
{
	return m_bNoActiveMissions;
}

bool CvEspionageMissionInfo::isSeeResearch() const
{
	return m_bSeeResearch;
}

bool CvEspionageMissionInfo::isDestroyImprovement() const
{
	return m_bDestroyImprovement;
}

int CvEspionageMissionInfo::getDestroyBuildingCostFactor() const
{
	return m_iDestroyBuildingCostFactor;
}

int CvEspionageMissionInfo::getDestroyUnitCostFactor() const
{
	return m_iDestroyUnitCostFactor;
}

int CvEspionageMissionInfo::getDestroyProjectCostFactor() const
{
	return m_iDestroyProjectCostFactor;
}

int CvEspionageMissionInfo::getDestroyProductionCostFactor() const
{
	return m_iDestroyProductionCostFactor;
}

int CvEspionageMissionInfo::getBuyUnitCostFactor() const
{
	return m_iBuyUnitCostFactor;
}

int CvEspionageMissionInfo::getBuyCityCostFactor() const
{
	return m_iBuyCityCostFactor;
}

int CvEspionageMissionInfo::getStealTreasuryTypes() const
{
	return m_iStealTreasuryTypes;
}

int CvEspionageMissionInfo::getCityInsertCultureAmountFactor() const
{
	return m_iCityInsertCultureAmountFactor;
}

int CvEspionageMissionInfo::getCityInsertCultureCostFactor() const
{
	return m_iCityInsertCultureCostFactor;
}

int CvEspionageMissionInfo::getCityPoisonWaterCounter() const
{
	return m_iCityPoisonWaterCounter;
}

int CvEspionageMissionInfo::getCityUnhappinessCounter() const
{
	return m_iCityUnhappinessCounter;
}

int CvEspionageMissionInfo::getCityRevoltCounter() const
{
	return m_iCityRevoltCounter;
}

int CvEspionageMissionInfo::getBuyTechCostFactor() const
{
	return m_iBuyTechCostFactor;
}

int CvEspionageMissionInfo::getSwitchCivicCostFactor() const
{
	return m_iSwitchCivicCostFactor;
}

int CvEspionageMissionInfo::getSwitchReligionCostFactor() const
{
	return m_iSwitchReligionCostFactor;
}

int CvEspionageMissionInfo::getPlayerAnarchyCounter() const
{
	return m_iPlayerAnarchyCounter;
}

int CvEspionageMissionInfo::getCounterespionageNumTurns() const
{
	return m_iCounterespionageNumTurns;
}

int CvEspionageMissionInfo::getCounterespionageMod() const
{
	return m_iCounterespionageMod;
}

int CvEspionageMissionInfo::getDifficultyMod() const
{
	return m_iDifficultyMod;
}

bool CvEspionageMissionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
		return false;

	CvString szTextVal;

	pXML->GetChildXmlValByName(&m_iCost, "iCost");
	pXML->GetChildXmlValByName(&m_bIsPassive, "bIsPassive");
	pXML->GetChildXmlValByName(&m_bIsTwoPhases, "bIsTwoPhases");
	pXML->GetChildXmlValByName(&m_bTargetsCity, "bTargetsCity");
	pXML->GetChildXmlValByName(&m_bSelectPlot, "bSelectPlot");

	pXML->GetChildXmlValByName(szTextVal, "TechPrereq");
	m_iTechPrereq = pXML->FindInInfoClass(szTextVal);

	pXML->GetChildXmlValByName(&m_iVisibilityLevel, "iVisibilityLevel");
	pXML->GetChildXmlValByName(&m_bInvestigateCity, "bInvestigateCity");
	pXML->GetChildXmlValByName(&m_bSeeDemographics, "bSeeDemographics");
	pXML->GetChildXmlValByName(&m_bNoActiveMissions, "bNoActiveMissions");
	pXML->GetChildXmlValByName(&m_bSeeResearch, "bSeeResearch");

	pXML->GetChildXmlValByName(&m_bDestroyImprovement, "bDestroyImprovement");
	pXML->GetChildXmlValByName(&m_iDestroyBuildingCostFactor, "iDestroyBuildingCostFactor");
	pXML->GetChildXmlValByName(&m_iDestroyUnitCostFactor, "iDestroyUnitCostFactor");
	pXML->GetChildXmlValByName(&m_iDestroyProjectCostFactor, "iDestroyProjectCostFactor");
	pXML->GetChildXmlValByName(&m_iDestroyProductionCostFactor, "iDestroyProductionCostFactor");
	pXML->GetChildXmlValByName(&m_iBuyUnitCostFactor, "iBuyUnitCostFactor");
	pXML->GetChildXmlValByName(&m_iBuyCityCostFactor, "iBuyCityCostFactor");
	pXML->GetChildXmlValByName(&m_iStealTreasuryTypes, "iStealTreasuryTypes");
	pXML->GetChildXmlValByName(&m_iCityInsertCultureAmountFactor, "iCityInsertCultureAmountFactor");
	pXML->GetChildXmlValByName(&m_iCityInsertCultureCostFactor, "iCityInsertCultureCostFactor");
	pXML->GetChildXmlValByName(&m_iCityPoisonWaterCounter, "iCityPoisonWaterCounter");
	pXML->GetChildXmlValByName(&m_iCityUnhappinessCounter, "iCityUnhappinessCounter");
	pXML->GetChildXmlValByName(&m_iCityRevoltCounter, "iCityRevoltCounter");
	pXML->GetChildXmlValByName(&m_iBuyTechCostFactor, "iBuyTechCostFactor");
	pXML->GetChildXmlValByName(&m_iSwitchCivicCostFactor, "iSwitchCivicCostFactor");
	pXML->GetChildXmlValByName(&m_iSwitchReligionCostFactor, "iSwitchReligionCostFactor");
	pXML->GetChildXmlValByName(&m_iPlayerAnarchyCounter, "iPlayerAnarchyCounter");
	pXML->GetChildXmlValByName(&m_iCounterespionageNumTurns, "iCounterespionageNumTurns");
	pXML->GetChildXmlValByName(&m_iCounterespionageMod, "iCounterespionageMod");
	pXML->GetChildXmlValByName(&m_iDifficultyMod, "iDifficultyMod");
	pXML->GetChildXmlValByName(&m_bReturnToCapital, "bReturnToCapital"); // advc.103

	return true;
}
