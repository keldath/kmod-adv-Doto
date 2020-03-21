#pragma once

#ifndef CV_INFO_UNIT_H
#define CV_INFO_UNIT_H

/*  advc.003x: Cut from CvInfos.h. Unit-related classes:
	CvUnitInfo
	CvUnitArtStyleInfo (not derived from CvAssetInfoBase; doesn't belong in CvInfo_Asset.h)
	CvUnitClassInfo (for the mapping to CvUnitInfo, CvCivilizationInfo gets precompiled)
	CvSpecialUnitInfo
	CvPromotionInfo
	CvEspionageMissionInfo
	CvBuildInfo (via CvInfo_Build.h) */
#include "CvInfo_Build.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvUnitClassInfo  // advc: Moved up for inline function calls from CvUnit
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitClassInfo : public CvInfoBase
{
public: // All the const functions are exposed to Python. advc:inl: All inlined.
	CvUnitClassInfo();

	inline int getMaxGlobalInstances() const
	{
		return m_iMaxGlobalInstances;
	}
	inline bool isWorldUnit() const // advc.003w: Replacing global isWorldUnitClass
	{
		return (getMaxGlobalInstances() != -1);
	}
	inline int getMaxTeamInstances() const
	{
		return m_iMaxTeamInstances;
	}
	inline bool isTeamUnit() const // advc.003w: Replacing global isTeamUnitClass
	{
		return (getMaxTeamInstances() != -1);
	}
	inline int getMaxPlayerInstances() const
	{
		return m_iMaxPlayerInstances;
	}
	inline bool isNationalUnit() const // advc.003w: Replacing global isNationalUnitClass
	{
		return (getMaxPlayerInstances() != -1);
	}
	inline bool isLimited() const // advc.003w: Replacing global isLimitedUnitClass
	{
		return (isWorldUnit() || isTeamUnit() || isNationalUnit());
	}
	int getInstanceCostModifier() const
	{
		return m_iInstanceCostModifier;
	}
	UnitTypes getDefaultUnit() const // advc.003x: Renamed from getDefaultUnitIndex
	{
		return (UnitTypes)m_iDefaultUnitIndex;
	}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass3();

protected:
	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iMaxPlayerInstances;
	int m_iInstanceCostModifier;
	int m_iDefaultUnitIndex;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvUnitInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitInfo : public CvHotkeyInfo
{
public: /*  All const functions are exposed to Python except some related to art and those added by mods.
			Replaced int return types with enum. advc.inl: Inlined most of the non-array getters. */
	CvUnitInfo();
	~CvUnitInfo();

/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
	bool isPrereqOrCivics(int iCivic) const;
	bool isPrereqAndCivics(int iCivic) const;

	std::vector<CvString> m_aszPrereqOrCivicsforPass3;
	std::vector<bool> m_abPrereqOrCivicsforPass3;
	
	int isPrereqOrCivicsVectorSize();
	CvString isPrereqOrCivicsNamesVectorElement(int i);
	int isPrereqOrCivicsValuesVectorElement(int i);
	
	int isPrereqAndCivicsVectorSize();
	CvString isPrereqAndCivicsNamesVectorElement(int i);
	int isPrereqAndCivicsValuesVectorElement(int i);
	
	std::vector<CvString> m_aszPrereqAndCivicsforPass3;
	std::vector<bool> m_abPrereqAndCivicsforPass3;

	bool readPass3();
protected:
	bool* m_pbPrereqOrCivics;
	bool* m_pbPrereqAndCivics;
public:
/**
 ** End: Unit Civic Prereq
 **/
	int getAIWeight() const { return m_iAIWeight; }
	inline int getProductionCost() const { return m_iProductionCost; }
	int getHurryCostModifier() const { return m_iHurryCostModifier; }
	int getAdvancedStartCost() const;
	int getAdvancedStartCostIncrease() const;
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	int getNumCitySizeUnitPrereq() const;  // Exposed to Python
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	int getMinAreaSize() const { return m_iMinAreaSize; }
	inline int getMoves() const { return m_iMoves; }
	inline int getAirRange() const { return m_iAirRange; }
	int getAirUnitCap() const { return m_iAirUnitCap; }
	int getDropRange() const { return m_iDropRange; }
	int getNukeRange() const { return m_iNukeRange; }
	inline int getWorkRate() const { return m_iWorkRate; }
	int getBaseDiscover() const { return m_iBaseDiscover; }
	int getDiscoverMultiplier() const { return m_iDiscoverMultiplier; }
	int getBaseHurry() const { return m_iBaseHurry; }
	int getHurryMultiplier() const { return m_iHurryMultiplier; }
	int getBaseTrade() const { return m_iBaseTrade; }
	int getTradeMultiplier() const { return m_iTradeMultiplier; }
	int getGreatWorkCulture() const { return m_iGreatWorkCulture; }
	int getEspionagePoints() const { return m_iEspionagePoints; }
	inline int getCombat() const { return m_iCombat; }
	void setCombat(int iNum);
	inline int getCombatLimit() const { return m_iCombatLimit; }
	inline int getAirCombat() const { return m_iAirCombat; }
	inline int getAirCombatLimit() const { return m_iAirCombatLimit; }
	int getXPValueAttack() const { return m_iXPValueAttack; }
	int getXPValueDefense() const { return m_iXPValueDefense; }
	inline int getFirstStrikes() const { return m_iFirstStrikes; }
	inline int getChanceFirstStrikes() const { return m_iChanceFirstStrikes; }
	int getInterceptionProbability() const { return m_iInterceptionProbability; }
	int getEvasionProbability() const { return m_iEvasionProbability; }
	inline int getWithdrawalProbability() const { return m_iWithdrawalProbability; }
	inline int getCollateralDamage() const { return m_iCollateralDamage; }
	int getCollateralDamageLimit() const { return m_iCollateralDamageLimit; }
	int getCollateralDamageMaxUnits() const { return m_iCollateralDamageMaxUnits; }
	int getCityAttackModifier() const { return m_iCityAttackModifier; }
	int getCityDefenseModifier() const { return m_iCityDefenseModifier; }
	int getAnimalCombatModifier() const { return m_iAnimalCombatModifier; }
	int getBarbarianCombatModifier() const // advc.315c
	{
		return m_iBarbarianCombatModifier;
	}
	int getHillsAttackModifier() const { return m_iHillsAttackModifier; }
	int getHillsDefenseModifier() const { return m_iHillsDefenseModifier; }
	int getBombRate() const { return m_iBombRate; }
	int getBombardRate() const { return m_iBombardRate; }
	SpecialUnitTypes getSpecialCargo() const { return (SpecialUnitTypes)m_iSpecialCargo; }
	DomainTypes getDomainCargo() const { return (DomainTypes)m_iDomainCargo; }

	int getCargoSpace() const { return m_iCargoSpace; }
	int getConscriptionValue() const { return m_iConscriptionValue; }
	int getCultureGarrisonValue() const { return m_iCultureGarrisonValue; }
	int getExtraCost() const { return m_iExtraCost; }
	int getAssetValue() const { return m_iAssetValue; }
	int getPowerValue() const { return m_iPowerValue; }

	inline UnitClassTypes getUnitClassType() const { return (UnitClassTypes)m_iUnitClassType; }
	SpecialUnitTypes getSpecialUnitType() const { return (SpecialUnitTypes)m_iSpecialUnitType; }
	UnitClassTypes getUnitCaptureClassType() const { return (UnitClassTypes)m_iUnitCaptureClassType; }
	inline UnitCombatTypes getUnitCombatType() const { return (UnitCombatTypes)m_iUnitCombatType; }
	// advc.inl: force-inlined for CvArea::canBeEntered
	__forceinline DomainTypes getDomainType() const { return (DomainTypes)m_iDomainType; }
	UnitAITypes getDefaultUnitAIType() const { return (UnitAITypes)m_iDefaultUnitAIType; }
	inline InvisibleTypes getInvisibleType() const { return (InvisibleTypes)m_iInvisibleType; }
	InvisibleTypes getSeeInvisibleType(int i) const
	{
		FAssertBounds(0, m_aiSeeInvisibleTypes.size(), i);
		return (InvisibleTypes)m_aiSeeInvisibleTypes[i];
	}
	inline int getNumSeeInvisibleTypes() const { return (int)m_aiSeeInvisibleTypes.size(); }
	AdvisorTypes getAdvisorType() const { return (AdvisorTypes)m_iAdvisorType; }	
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain 																*/
/********************************************************************************/
	int getMaxStartEra() const;							// Exposed to Python
	int getForceObsoleteTech() const;									// Exposed to Python
	bool isStateReligion() const;				// Exposed to Python
	int getPrereqGameOption() const;									// Exposed to Python
	int getNotGameOption() const;									// Exposed to Python
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
	ReligionTypes getHolyCity() const { return (ReligionTypes)m_iHolyCity; }
	ReligionTypes getReligionType() const { return (ReligionTypes)m_iReligionType; }
	ReligionTypes getStateReligion() const { return (ReligionTypes)m_iStateReligion; }
	ReligionTypes getPrereqReligion() const { return (ReligionTypes)m_iPrereqReligion; }
	CorporationTypes getPrereqCorporation() const { return (CorporationTypes)m_iPrereqCorporation; }
	BuildingTypes getPrereqBuilding() const { return (BuildingTypes)m_iPrereqBuilding; }
	TechTypes getPrereqAndTech() const { return (TechTypes)m_iPrereqAndTech; }
//	int getPrereqVicinityBonus() const;  //Shqype Vicinity Bonus Add
	bool isTechRequired(TechTypes eTech) const; // advc.003w: Replacing global isTechRequiredForUnit
	BonusTypes getPrereqAndBonus() const { return (BonusTypes)m_iPrereqAndBonus; }
	int getGroupSize() const; // the initial number of individuals in the unit group
	int getGroupDefinitions() const; // the number of UnitMeshGroups for this unit
	int getMeleeWaveSize() const;
	int getRangedWaveSize() const;
	int getNumUnitNames() const;
	CommandTypes getCommandType() const;
	void setCommandType(CommandTypes eNewType);

	inline bool isAnimal() const { return m_bAnimal; }
	bool isFoodProduction() const { return m_bFoodProduction; }
	bool isNoBadGoodies() const { return m_bNoBadGoodies; }
	bool isOnlyDefensive() const { return m_bOnlyDefensive; }
	bool isOnlyAttackAnimals() const // advc.315a
	{
		return m_bOnlyAttackAnimals;
	}
	bool isOnlyAttackBarbarians() const // advc.315b
	{
		return m_bOnlyAttackBarbarians;
	}
	bool isNoCapture() const { return m_bNoCapture; }
	bool isQuickCombat() const { return m_bQuickCombat; }
	bool isRivalTerritory() const { return m_bRivalTerritory; }
	bool isMilitaryHappiness() const { return m_bMilitaryHappiness; }
	bool isMilitarySupport() const { return m_bMilitarySupport; }
	bool isMilitaryProduction() const { return m_bMilitaryProduction; }
	bool isPillage() const { return m_bPillage; }
	inline bool isSpy() const { return m_bSpy; }
	bool isSabotage() const { return m_bSabotage; }
	bool isDestroy() const { return m_bDestroy; }
	bool isStealPlans() const { return m_bStealPlans; }
	bool isInvestigate() const { return m_bInvestigate; }
	bool isCounterSpy() const { return m_bCounterSpy; }
	bool isFound() const { return m_bFound; }
	bool isGoldenAge() const { return m_bGoldenAge; }
	inline bool isInvisible() const { return m_bInvisible; }
	void setInvisible(bool bEnable) ;
	bool isFirstStrikeImmune() const { return m_bFirstStrikeImmune; }
	bool isNoDefensiveBonus() const { return m_bNoDefensiveBonus; }
	bool isIgnoreBuildingDefense() const { return m_bIgnoreBuildingDefense; }
	// advc.inl: force-inlined for CvArea::canBeEntered. Renamed from "isCanMoveImpassable"
	__forceinline bool canMoveImpassable() const { return m_bCanMoveImpassable; }
	inline bool isCanMoveAllTerrain() const { return m_bCanMoveAllTerrain; }
//Deliverator mountains mod
	inline bool isCanMovePeak() const { return m_bCanMovePeak; }
	bool isFlatMovementCost() const { return m_bFlatMovementCost; }
	bool isIgnoreTerrainCost() const { return m_bIgnoreTerrainCost; }
	bool isNukeImmune() const { return m_bNukeImmune; }
	bool isPrereqBonuses() const { return m_bPrereqBonuses; }
	bool isPrereqReligion() const { return m_bPrereqReligion; }
	bool isMechUnit() const { return m_bMechanized; }
	bool isRenderBelowWater() const;
	bool isRenderAlways() const;
	bool isSuicide() const { return m_bSuicide; }
	bool isLineOfSight() const { return m_bLineOfSight; }
	bool isHiddenNationality() const { return m_bHiddenNationality; }
	bool isAlwaysHostile() const { return m_bAlwaysHostile; }
	inline bool isNoRevealMap() const { return m_bNoRevealMap; }

	float getUnitMaxSpeed() const;
	float getUnitPadTime() const;

	// Array access:

	TechTypes getPrereqAndTechs(int i) const;
	inline bool isAnyPrereqAndTech() const { return (m_piPrereqAndTechs != NULL); } // advc.003t
	BonusTypes getPrereqOrBonuses(int i) const;
	inline bool isAnyPrereqOrBonus() const { return (m_piPrereqOrBonuses != NULL); } // advc.003t
	// <advc.905b>
//	int getPrereqOrVicinityBonuses(int i) const;  //Shqype Vicinity Bonus Add
	BonusTypes getSpeedBonuses(int i) const;
	int getExtraMoves(int i) const;
	// </advc.905b>
	int getProductionTraits(int i) const;
	int getFlavorValue(int i) const;
	int getTerrainAttackModifier(int i) const;
	int getTerrainDefenseModifier(int i) const;
	int getFeatureAttackModifier(int i) const;
	int getFeatureDefenseModifier(int i) const;
	int getUnitClassAttackModifier(int i) const;
	inline bool isAnyUnitClassAttackModifier() const { return (m_piUnitClassAttackModifier != NULL); } // advc.003t
	int getUnitClassDefenseModifier(int i) const;
	inline bool isAnyUnitClassDefenseModifier() const { return (m_piUnitClassDefenseModifier != NULL); } // advc.003t
	int getUnitCombatModifier(int i) const;
	int getUnitCombatCollateralImmune(int i) const;
	int getDomainModifier(int i) const;
	int getBonusProductionModifier(int i) const;
	inline bool isAnyBonusProductionModifier() const { return (m_piBonusProductionModifier != NULL); } // advc.003t
	int getUnitGroupRequired(int i) const;
	int getReligionSpreads(int i) const;
	int getCorporationSpreads(int i) const;
	TechTypes getTerrainPassableTech(int i) const;
	inline bool isAnyTerrainPassableTech() const { return (m_piTerrainPassableTech != NULL); } // advc.003t
	TechTypes getFeaturePassableTech(int i) const;
	int getFlankingStrikeUnitClass(int i) const;
	inline bool isAnyFlankingStrikeUnitClass() const { return (m_piFlankingStrikeUnitClass != NULL); } // advc.003t
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain 																*/
/********************************************************************************/
//	bool isPrereqOrCivics(int i) const;				// Exposed to Python
	bool isPrereqBuildingClass(int i) const; 				//Exposed to Python
	int getPrereqBuildingClassOverrideTech(int i) const; 				//Exposed to Python
	int getPrereqBuildingClassOverrideEra(int i) const; 				//Exposed to Python
	bool getForceObsoleteUnitClass(int i) const; 				//Exposed to Python
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
	bool getUpgradeUnitClass(int i) const;
	inline bool isAnyUpgradeUnitClass() const { return (m_pbUpgradeUnitClass != NULL); } // advc.003t
	bool getTargetUnitClass(int i) const;
	inline bool isAnyTargetUnitClass() const { return (m_pbTargetUnitClass != NULL); } // advc.003t
	bool getTargetUnitCombat(int i) const;
	bool getDefenderUnitClass(int i) const;
	inline bool isAnyDefenderUnitClass() const { return (m_pbDefenderUnitClass != NULL); } // advc.003t
	bool getDefenderUnitCombat(int i) const;
	bool getUnitAIType(int i) const;
	bool getNotUnitAIType(int i) const;
	inline bool isAnyNotUnitAIType() const { return (m_pbNotUnitAIType != NULL); } // advc.003t
	bool getBuilds(int i) const;
	inline bool isAnyBuilds() const { return (m_pbBuilds != NULL); } // advc.003t
	bool getGreatPeoples(int i) const;
	bool getBuildings(int i) const;
	inline bool isAnyBuildings() const { return (m_pbBuildings != NULL); } // advc.003t
	bool getForceBuildings(int i) const; // advc.003t: Dummy function
	bool getTerrainImpassable(int i) const;
	inline bool isAnyTerrainImpassable() const { return (m_pbTerrainImpassable != NULL); } // advc.003t
	bool getFeatureImpassable(int i) const;
	inline bool isAnyFeatureImpassable() const { return (m_pbFeatureImpassable != NULL); } // advc.003t
	bool getTerrainNative(int i) const;
	bool getFeatureNative(int i) const;
	bool getFreePromotions(int i) const;
	inline bool isAnyFreePromotions() const { return (m_pbFreePromotions != NULL); } // advc.003t
	int getLeaderPromotion() const;
	int getLeaderExperience() const;

	// <advc.003w>
	// Wrappers around CvUnitClass; for convenience.
	inline bool isWorldUnit() const
	{
		return GC.getInfo(getUnitClassType()).isWorldUnit();
	}
	inline bool isTeamUnit() const
	{
		return GC.getInfo(getUnitClassType()).isTeamUnit();
	}
	inline bool isLimited() const
	{
		return GC.getInfo(getUnitClassType()).isLimited();
	}
	// Moved from CvGameCoreUtils.h. Still exposed to Python through CyGameCoreUtils.
	bool isPromotionValid(PromotionTypes ePromotion, bool bLeader) const;
	// </advc.003w>

	const TCHAR* getEarlyArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setEarlyArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getLateArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setLateArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getMiddleArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setMiddleArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getUnitNames(int i) const;
	const TCHAR* getFormationType() const;

	const TCHAR* getButton() const;
	void updateArtDefineButton();

	const CvArtInfoUnit* getArtInfo(int i, EraTypes eEra, UnitArtStyleTypes eStyle) const;
	#if SERIALIZE_CVINFOS
	void read(FDataStreamBase*);
	void write(FDataStreamBase*);
	#endif
	bool read(CvXMLLoadUtility* pXML);
	// <advc.315>
	inline bool isMostlyDefensive() const
	{
		return isOnlyDefensive() || isOnlyAttackAnimals() || isOnlyAttackBarbarians();
	} // </advc.315>
	// advc.opt:
	static inline bool canAnyMoveAllTerrain() { return m_bCanAnyMoveAllTerrain; }

protected:
	static bool m_bCanAnyMoveAllTerrain; // advc.opt

	int m_iAIWeight;
	int m_iProductionCost;
	int m_iHurryCostModifier;
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	int m_iNumCitySizeUnitPrereq;
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	int m_iMinAreaSize;
	int m_iMoves;
	int m_iAirRange;
	int m_iAirUnitCap;
	int m_iDropRange;
	int m_iNukeRange;
	int m_iWorkRate;
	int m_iBaseDiscover;
	int m_iDiscoverMultiplier;
	int m_iBaseHurry;
	int m_iHurryMultiplier;
	int m_iBaseTrade;
	int m_iTradeMultiplier;
	int m_iGreatWorkCulture;
	int m_iEspionagePoints;
	int m_iCombat;
	int m_iCombatLimit;
	int m_iAirCombat;
	int m_iAirCombatLimit;
	int m_iXPValueAttack;
	int m_iXPValueDefense;
	int m_iFirstStrikes;
	int m_iChanceFirstStrikes;
	int m_iInterceptionProbability;
	int m_iEvasionProbability;
	int m_iWithdrawalProbability;
	int m_iCollateralDamage;
	int m_iCollateralDamageLimit;
	int m_iCollateralDamageMaxUnits;
	int m_iCityAttackModifier;
	int m_iCityDefenseModifier;
	int m_iAnimalCombatModifier;
	int m_iBarbarianCombatModifier; // advc.315
	int m_iHillsAttackModifier;
	int m_iHillsDefenseModifier;
	int m_iBombRate;
	int m_iBombardRate;
	int m_iSpecialCargo;

	int m_iDomainCargo;
	int m_iCargoSpace;
	int m_iConscriptionValue;
	int m_iCultureGarrisonValue;
	int m_iExtraCost;
	int m_iAssetValue;
	int m_iPowerValue;
	int m_iUnitClassType;
	int m_iSpecialUnitType;
	int m_iUnitCaptureClassType;
	int m_iUnitCombatType;
	int m_iDomainType;
	int m_iDefaultUnitAIType;
	int m_iInvisibleType;
	int m_iAdvisorType;
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain 																*/
/********************************************************************************/
	int m_iMaxStartEra;
	int m_iForceObsoleteTech;
	bool m_bStateReligion;
	int m_iPrereqGameOption;
	int m_iNotGameOption;
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
	int m_iHolyCity;
	int m_iReligionType;
	int m_iStateReligion;
	int m_iPrereqReligion;
	int m_iPrereqCorporation;
	int m_iPrereqBuilding;
	int m_iPrereqAndTech;
	int m_iPrereqAndBonus;
//	int m_iPrereqVicinityBonus;  //Shqype Vicinity Bonus Add
	int m_iGroupSize;
	int m_iGroupDefinitions;
	int m_iUnitMeleeWaveSize;
	int m_iUnitRangedWaveSize;
	int m_iNumUnitNames;
	int m_iCommandType;
	int m_iLeaderExperience;

	bool m_bAnimal;
	bool m_bFoodProduction;
	bool m_bNoBadGoodies;
	bool m_bOnlyDefensive;
	bool m_bOnlyAttackAnimals; // advc.315a
	bool m_bOnlyAttackBarbarians; // advc.315b
	bool m_bNoCapture;
	bool m_bQuickCombat;
	bool m_bRivalTerritory;
	bool m_bMilitaryHappiness;
	bool m_bMilitarySupport;
	bool m_bMilitaryProduction;
	bool m_bPillage;
	bool m_bSpy;
	bool m_bSabotage;
	bool m_bDestroy;
	bool m_bStealPlans;
	bool m_bInvestigate;
	bool m_bCounterSpy;
	bool m_bFound;
	bool m_bGoldenAge;
	bool m_bInvisible;
	bool m_bFirstStrikeImmune;
	bool m_bNoDefensiveBonus;
	bool m_bIgnoreBuildingDefense;
	bool m_bCanMoveImpassable;
	bool m_bCanMoveAllTerrain;
//Mountains mod
	bool m_bCanMovePeak; //Deliverator	
	bool m_bFlatMovementCost;
	bool m_bIgnoreTerrainCost;
	bool m_bNukeImmune;
	bool m_bPrereqBonuses;
	bool m_bPrereqReligion;
	bool m_bMechanized;
	bool m_bRenderBelowWater;
	bool m_bRenderAlways;
	bool m_bSuicide;
	bool m_bLineOfSight;
	bool m_bHiddenNationality;
	bool m_bAlwaysHostile;
	bool m_bNoRevealMap;
	int m_iLeaderPromotion;

	float m_fUnitMaxSpeed;
	float m_fUnitPadTime;

	int* m_piPrereqAndTechs;
	int* m_piPrereqOrBonuses;
//	int* m_piPrereqOrVicinityBonuses;  //Shqype Vicinity Bonus Add
	int* m_piSpeedBonuses[2]; // advc.905b
	int* m_piProductionTraits;
	int* m_piFlavorValue;
	int* m_piTerrainAttackModifier;
	int* m_piTerrainDefenseModifier;
	int* m_piFeatureAttackModifier;
	int* m_piFeatureDefenseModifier;
	int* m_piUnitClassAttackModifier;
	int* m_piUnitClassDefenseModifier;
	int* m_piUnitCombatModifier;
	int* m_piUnitCombatCollateralImmune;
	int* m_piDomainModifier;
	int* m_piBonusProductionModifier;
	int* m_piUnitGroupRequired;
	int* m_piReligionSpreads;
	int* m_piCorporationSpreads;
	int* m_piTerrainPassableTech;
	int* m_piFeaturePassableTech;
	int* m_piFlankingStrikeUnitClass;

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain 																*/
/********************************************************************************/
//	bool* m_pbPrereqOrCivics;
	bool* m_pbPrereqBuildingClass;
	int* m_piPrereqBuildingClassOverrideTech;
	int* m_piPrereqBuildingClassOverrideEra;
	bool* m_pbForceObsoleteUnitClass;
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	bool* m_pbUpgradeUnitClass;
	bool* m_pbTargetUnitClass;
	bool* m_pbTargetUnitCombat;
	bool* m_pbDefenderUnitClass;
	bool* m_pbDefenderUnitCombat;
	bool* m_pbUnitAIType;
	bool* m_pbNotUnitAIType;
	bool* m_pbBuilds;
	bool* m_pbGreatPeoples;
	bool* m_pbBuildings;
	//bool* m_pbForceBuildings; // advc.003t
	bool* m_pbTerrainNative;
	bool* m_pbFeatureNative;
	bool* m_pbTerrainImpassable;
	bool* m_pbFeatureImpassable;
	bool* m_pbFreePromotions;

	CvString* m_paszEarlyArtDefineTags;
	CvString* m_paszLateArtDefineTags;
	CvString* m_paszMiddleArtDefineTags;
	CvString* m_paszUnitNames;
	CvString m_szFormationType;
	CvString m_szArtDefineButton;

	std::vector<int> m_aiSeeInvisibleTypes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvUnitArtStyleInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitArtStyleInfo : public CvInfoBase // advc.enum: Renamed from CvUnitArtStyleTypeInfo (to be consistent with other info class/ enum names)
{
public:
	const TCHAR* getEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
	const TCHAR* getLateArtDefineTag(int i, int j) const;
	void setLateArtDefineTag(int i, int j, const TCHAR* szVal);
	const TCHAR* getMiddleArtDefineTag(int i, int j) const;
	void setMiddleArtDefineTag(int i, int j, const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);

protected:
	struct ArtDefneTag
	{
		int iMeshIndex;
		int iUnitType;
		CvString szTag;
	};
	typedef std::vector<ArtDefneTag> ArtDefineArray;
	ArtDefineArray m_azEarlyArtDefineTags;
	ArtDefineArray m_azLateArtDefineTags;
	ArtDefineArray m_azMiddleArtDefineTags;
};
typedef CvUnitArtStyleInfo CvUnitArtStyleTypeInfo; // advc.enum: for any legacy code

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvSpecialUnitInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialUnitInfo : public CvInfoBase
{
public:

	CvSpecialUnitInfo();
	~CvSpecialUnitInfo();

	bool isValid() const;
	bool isCityLoad() const;

	bool isCarrierUnitAIType(int i) const; // Exposed to Python
	int getProductionTraits(int i) const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

protected:
	bool m_bValid;
	bool m_bCityLoad;

	bool* m_pbCarrierUnitAITypes;
	int* m_piProductionTraits;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvPromotionInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPromotionInfo :	public CvHotkeyInfo
{
public: // All the const functions are exposed to Python
	CvPromotionInfo();
	~CvPromotionInfo();

	int getLayerAnimationPath() const;
	int getPrereqPromotion() const;
	void setPrereqPromotion(int i);
	int getPrereqOrPromotion1() const;
	void setPrereqOrPromotion1(int i);
	int getPrereqOrPromotion2() const;
	void setPrereqOrPromotion2(int i);
	// K-Mod, 7/jan/11, karadoc
	int getPrereqOrPromotion3() const;
	void setPrereqOrPromotion3(int i); // K-Mod end
	int getTechPrereq() const;
	int getStateReligionPrereq() const;
	int getVisibilityChange() const;
	int getMovesChange() const;
	int getMoveDiscountChange() const;
	int getAirRangeChange() const;
	int getInterceptChange() const;
	int getEvasionChange() const;
	int getWithdrawalChange() const;
	int getCargoChange() const;
	int getCollateralDamageChange() const;
	int getBombardRateChange() const;
	int getFirstStrikesChange() const;
	int getChanceFirstStrikesChange() const;
	int getEnemyHealChange() const;
	int getNeutralHealChange() const;
	int getFriendlyHealChange() const;
	int getSameTileHealChange() const;
	int getAdjacentTileHealChange() const;
	int getCombatPercent() const;
	int getCityAttackPercent() const;
	int getCityDefensePercent() const;
	int getHillsAttackPercent() const;
	int getHillsDefensePercent() const;
	int getCommandType() const;
	void setCommandType(int iNewType);

	int getRevoltProtection() const;
	int getCollateralDamageProtection() const;
	int getPillageChange() const;
	int getUpgradeDiscount() const;
	int getExperiencePercent() const;
	int getKamikazePercent() const;

	bool isLeader() const;
	// advc.164: was isBlitz
	int getBlitz() const;
	bool isAmphib() const;
//MOD@VET_Andera412_Blocade_Unit-begin1/2
	bool isUnblocade() const;
//MOD@VET_Andera412_Blocade_Unit-end1/2
	bool isRiver() const;
	bool isEnemyRoute() const;
	bool isAlwaysHeal() const;
	bool isHillsDoubleMove() const;
	bool isImmuneToFirstStrikes() const;

	const TCHAR* getSound() const;
	void setSound(const TCHAR* szVal);

	// Array access:

	int getTerrainAttackPercent(int i) const;
	int getTerrainDefensePercent(int i) const;
	int getFeatureAttackPercent(int i) const;
	int getFeatureDefensePercent(int i) const;
	int getUnitCombatModifierPercent(int i) const;
	int getDomainModifierPercent(int i) const;

	bool getTerrainDoubleMove(int i) const;
	bool getFeatureDoubleMove(int i) const;
	bool getUnitCombat(int i) const;
	#if SERIALIZE_CVINFOS
	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	#endif
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

protected:
	int m_iLayerAnimationPath;
	int m_iPrereqPromotion;
	int m_iPrereqOrPromotion1;
	int m_iPrereqOrPromotion2;
	int m_iPrereqOrPromotion3; // K-Mod

	int m_iTechPrereq;
	int m_iStateReligionPrereq;
	int m_iVisibilityChange;
	int m_iMovesChange;
	int m_iMoveDiscountChange;
	int m_iAirRangeChange;
	int m_iInterceptChange;
	int m_iEvasionChange;
	int m_iWithdrawalChange;
	int m_iCargoChange;
	int m_iCollateralDamageChange;
	int m_iBombardRateChange;
	int m_iFirstStrikesChange;
	int m_iChanceFirstStrikesChange;
	int m_iEnemyHealChange;
	int m_iNeutralHealChange;
	int m_iFriendlyHealChange;
	int m_iSameTileHealChange;
	int m_iAdjacentTileHealChange;
	int m_iCombatPercent;
	int m_iCityAttackPercent;
	int m_iCityDefensePercent;
	int m_iHillsAttackPercent;
	int m_iHillsDefensePercent;
	int m_iCommandType;
	int m_iRevoltProtection;
	int m_iCollateralDamageProtection;
	int m_iPillageChange;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iKamikazePercent;

	bool m_bLeader;
	//bool m_bBlitz;
	int m_iBlitz; // advc.164
	bool m_bAmphib;		
//MOD@VET_Andera412_Blocade_Unit-begin2/2
	bool m_bUnblocade;
//MOD@VET_Andera412_Blocade_Unit-end2/2
	bool m_bRiver;
	bool m_bEnemyRoute;
	bool m_bAlwaysHeal;
	bool m_bHillsDoubleMove;
	bool m_bImmuneToFirstStrikes;

	CvString m_szSound;

	int* m_piTerrainAttackPercent;
	int* m_piTerrainDefensePercent;
	int* m_piFeatureAttackPercent;
	int* m_piFeatureDefensePercent;
	int* m_piUnitCombatModifierPercent;
	int* m_piDomainModifierPercent;

	bool* m_pbTerrainDoubleMove;
	bool* m_pbFeatureDoubleMove;
	bool* m_pbUnitCombat;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvEspionageMissionInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEspionageMissionInfo : public CvInfoBase
{
public:
	CvEspionageMissionInfo();

	inline int getCost() const { return m_iCost; } // advc.inl
	inline bool isPassive() const { return m_bIsPassive; } // advc.inl
	bool isTwoPhases() const;
	bool isTargetsCity() const;
	bool isSelectPlot() const;

	int getTechPrereq() const;
	int getVisibilityLevel() const;
	bool isInvestigateCity() const;
	bool isSeeDemographics() const;
	bool isNoActiveMissions() const;
	bool isSeeResearch() const;

	bool isDestroyImprovement() const;
	int getDestroyBuildingCostFactor() const;
	int getDestroyUnitCostFactor() const;
	int getDestroyProjectCostFactor() const;
	int getDestroyProductionCostFactor() const;
	int getBuyUnitCostFactor() const;
	int getBuyCityCostFactor() const;
	int getStealTreasuryTypes() const;
	int getCityInsertCultureAmountFactor() const;
	int getCityInsertCultureCostFactor() const;
	int getCityPoisonWaterCounter() const;
	int getCityUnhappinessCounter() const;
	int getCityRevoltCounter() const;
	int getBuyTechCostFactor() const;
	int getSwitchCivicCostFactor() const;
	int getSwitchReligionCostFactor() const;
	int getPlayerAnarchyCounter() const;
	int getCounterespionageNumTurns() const;
	int getCounterespionageMod() const;
	int getDifficultyMod() const;
	inline bool isReturnToCapital() const { return m_bReturnToCapital; } // advc.103

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iCost;
	bool m_bIsPassive;
	bool m_bIsTwoPhases;
	bool m_bTargetsCity;
	bool m_bSelectPlot;

	int m_iTechPrereq;
	int m_iVisibilityLevel;
	bool m_bInvestigateCity;
	bool m_bSeeDemographics;
	bool m_bNoActiveMissions;
	bool m_bSeeResearch;

	bool m_bDestroyImprovement;
	int m_iDestroyBuildingCostFactor;
	int m_iDestroyUnitCostFactor;
	int m_iDestroyProjectCostFactor;
	int m_iDestroyProductionCostFactor;
	int m_iBuyUnitCostFactor;
	int m_iBuyCityCostFactor;
	int m_iStealTreasuryTypes;
	int m_iCityInsertCultureAmountFactor;
	int m_iCityInsertCultureCostFactor;
	int m_iCityPoisonWaterCounter;
	int m_iCityUnhappinessCounter;
	int m_iCityRevoltCounter;
	int m_iBuyTechCostFactor;
	int m_iSwitchCivicCostFactor;
	int m_iSwitchReligionCostFactor;
	int m_iPlayerAnarchyCounter;
	int m_iCounterespionageNumTurns;
	int m_iCounterespionageMod;
	int m_iDifficultyMod;
	bool m_bReturnToCapital; // advc.103
};

#endif
