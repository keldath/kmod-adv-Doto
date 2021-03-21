#include "CvGameCoreDLL.h"
#include "CvInfo_All.h"

//
// Python interface for info classes (formerly structs)
// These are simple enough to be exposed directly - no wrappers
//
// advc.003e: Added template parameters 'boost::noncopyable'
void CyInfoPythonInterface1()
{
	printToConsole("Python Extension Module - CyInfoPythonInterface1\n");

	python::class_<CvInfoBase, boost::noncopyable>("CvInfoBase")

		.def("isGraphicalOnly", &CvInfoBase::isGraphicalOnly, "bool ()")

		.def("getType", &CvInfoBase::getType, "string ()")
		.def("getButton", &CvInfoBase::getButton, "string ()")

		.def("getTextKey", &CvInfoBase::pyGetTextKey, "wstring ()")
		.def("getText", &CvInfoBase::pyGetText, "wstring ()")
		.def("getDescription", &CvInfoBase::pyGetDescription, "wstring ()")
		.def("getDescriptionForm", &CvInfoBase::pyGetDescriptionForm, "wstring ()")
		.def("getCivilopedia", &CvInfoBase::pyGetCivilopedia, "wstring ()")
		.def("getStrategy", &CvInfoBase::pyGetStrategy, "wstring ()")
		.def("getHelp", &CvInfoBase::pyGetHelp, "wstring ()")
		.def("isMatchForLink", &CvInfoBase::isMatchForLink, "bool (string)")

		;

	python::class_<CvScalableInfo, boost::noncopyable>("CvScalableInfo")
		.def("getScale", &CvScalableInfo::getScale, "float  ()")
		;

	python::class_<CvSpecialistInfo, boost::noncopyable, python::bases<CvInfoBase> >("CvSpecialistInfo")
		.def("getGreatPeopleUnitClass", &CvSpecialistInfo::getGreatPeopleUnitClass, "int ()")
		.def("getGreatPeopleRateChange", &CvSpecialistInfo::getGreatPeopleRateChange, "int ()")
		.def("getMissionType", &CvSpecialistInfo::getMissionType, "int ()")

		.def("isVisible", &CvSpecialistInfo::isVisible, "bool ()")

		.def("getYieldChange", &CvSpecialistInfo::getYieldChange, "int (int i)")
		.def("getCommerceChange", &CvSpecialistInfo::getCommerceChange, "int (int i)")
		.def("getExperience", &CvSpecialistInfo::getExperience, "int ()")
		.def("getFlavorValue", &CvSpecialistInfo::getFlavorValue, "int (int i)")

		.def("getTexture", &CvSpecialistInfo::getTexture, "string ()")
		;

	python::class_<CvTechInfo, boost::noncopyable, python::bases<CvInfoBase> >("CvTechInfo")
		.def("getAdvisorType", &CvTechInfo::getAdvisorType, "int ()")
		.def("getAIWeight", &CvTechInfo::getAIWeight, "int ()")
		.def("getAITradeModifier", &CvTechInfo::getAITradeModifier, "int ()")
		.def("getResearchCost", &CvTechInfo::getResearchCost, "int ()")
		.def("getEra", &CvTechInfo::getEra, "int ()")
		.def("getTradeRoutes", &CvTechInfo::getTradeRoutes, "int ()")
		.def("getFeatureProductionModifier", &CvTechInfo::getFeatureProductionModifier, "int ()")
		.def("getWorkerSpeedModifier", &CvTechInfo::getWorkerSpeedModifier, "int ()")
		.def("getFirstFreeUnitClass", &CvTechInfo::getFirstFreeUnitClass, "int ()")
		.def("getHealth", &CvTechInfo::getHealth, "int ()")
		.def("getHappiness", &CvTechInfo::getHappiness, "int ()")
		.def("getFirstFreeTechs", &CvTechInfo::getFirstFreeTechs, "int ()")
		.def("getAssetValue", &CvTechInfo::getAssetValue, "int ()")
		.def("getPowerValue", &CvTechInfo::getPowerValue, "int ()")

		.def("getGridX", &CvTechInfo::getGridX, "int ()")
		.def("getGridY", &CvTechInfo::getGridY, "int ()")

		.def("isRepeat", &CvTechInfo::isRepeat, "bool ()")
		.def("isTrade", &CvTechInfo::isTrade, "bool ()")
		.def("isDisable", &CvTechInfo::isDisable, "bool ()")
		.def("isGoodyTech", &CvTechInfo::isGoodyTech, "bool ()")
		.def("isExtraWaterSeeFrom", &CvTechInfo::isExtraWaterSeeFrom, "bool ()")
		.def("isMapCentering", &CvTechInfo::isMapCentering, "bool ()")
		.def("isMapVisible", &CvTechInfo::isMapVisible, "bool ()")
		.def("isMapTrading", &CvTechInfo::isMapTrading, "bool ()")
		.def("isTechTrading", &CvTechInfo::isTechTrading, "bool ()")
		.def("isGoldTrading", &CvTechInfo::isGoldTrading, "bool ()")
		.def("isOpenBordersTrading", &CvTechInfo::isOpenBordersTrading, "bool ()")
		.def("isDefensivePactTrading", &CvTechInfo::isDefensivePactTrading, "bool ()")
		.def("isPermanentAllianceTrading", &CvTechInfo::isPermanentAllianceTrading, "bool ()")
		.def("isVassalStateTrading", &CvTechInfo::isVassalStateTrading, "bool ()")
		.def("isBridgeBuilding", &CvTechInfo::isBridgeBuilding, "bool ()")
		.def("isIrrigation", &CvTechInfo::isIrrigation, "bool ()")
		/* Population Limit ModComp - Beginning */
		.def("isNoPopulationLimit", &CvTechInfo::isNoPopulationLimit, "bool ()")
		/* Population Limit ModComp - End */
		.def("isIgnoreIrrigation", &CvTechInfo::isIgnoreIrrigation, "bool ()")
		.def("isWaterWork", &CvTechInfo::isWaterWork, "bool ()")
		.def("isRiverTrade", &CvTechInfo::isRiverTrade, "bool ()")

		.def("getQuote", &CvTechInfo::getQuote, "wstring ()")
		.def("getSound", &CvTechInfo::getSound, "string ()")
		.def("getSoundMP", &CvTechInfo::getSoundMP, "string ()")

		.def("getDomainExtraMoves", &CvTechInfo::getDomainExtraMoves, "int (int i)")
		.def("getFlavorValue", &CvTechInfo::getFlavorValue, "int (int i)")
		.def("getPrereqOrTechs", &CvTechInfo::getPrereqOrTechs, "int (int i)")
		.def("getPrereqAndTechs", &CvTechInfo::getPrereqAndTechs, "int (int i)")

		.def("getSpecialistExtraCommerce", &CvTechInfo::getSpecialistExtraCommerce, "int (int i)") // K-Mod
		.def("isCommerceFlexible", &CvTechInfo::isCommerceFlexible, "bool (int i)")
		.def("isTerrainTrade", &CvTechInfo::isTerrainTrade, "bool (int i)")
		;

	python::class_<CvPromotionInfo, boost::noncopyable, python::bases<CvInfoBase> >("CvPromotionInfo")

		.def("getPrereqPromotion", &CvPromotionInfo::getPrereqPromotion, "int ()")
		.def("getPrereqOrPromotion1", &CvPromotionInfo::getPrereqOrPromotion1, "int ()")
		.def("getPrereqOrPromotion2", &CvPromotionInfo::getPrereqOrPromotion2, "int ()")
		.def("getPrereqOrPromotion3", &CvPromotionInfo::getPrereqOrPromotion3, "int ()") // K-Mod
		.def("getActionInfoIndex", &CvPromotionInfo::getActionInfoIndex, "int ()")

		.def("getTechPrereq", &CvPromotionInfo::getTechPrereq, "int ()")
		.def("getStateReligionPrereq", &CvPromotionInfo::getStateReligionPrereq, "int ()")
		.def("getVisibilityChange", &CvPromotionInfo::getVisibilityChange, "int ()")
		.def("getMovesChange", &CvPromotionInfo::getMovesChange, "int ()")
		.def("getMoveDiscountChange", &CvPromotionInfo::getMoveDiscountChange, "int ()")
		.def("getAirRangeChange", &CvPromotionInfo::getAirRangeChange, "int ()")
		.def("getInterceptChange", &CvPromotionInfo::getInterceptChange, "int ()")
		.def("getEvasionChange", &CvPromotionInfo::getEvasionChange, "int ()")
		.def("getWithdrawalChange", &CvPromotionInfo::getWithdrawalChange, "int ()")
		.def("getCargoChange", &CvPromotionInfo::getCargoChange, "int ()")
		.def("getCollateralDamageChange", &CvPromotionInfo::getCollateralDamageChange, "int ()")
		.def("getBombardRateChange", &CvPromotionInfo::getBombardRateChange, "int ()")
		.def("getFirstStrikesChange", &CvPromotionInfo::getFirstStrikesChange, "int ()")
		.def("getChanceFirstStrikesChange", &CvPromotionInfo::getChanceFirstStrikesChange, "int ()")
		.def("getEnemyHealChange", &CvPromotionInfo::getEnemyHealChange, "int ()")
		.def("getNeutralHealChange", &CvPromotionInfo::getNeutralHealChange, "int ()")
		.def("getFriendlyHealChange", &CvPromotionInfo::getFriendlyHealChange, "int ()")
		.def("getSameTileHealChange", &CvPromotionInfo::getSameTileHealChange, "int ()")
		.def("getAdjacentTileHealChange", &CvPromotionInfo::getAdjacentTileHealChange, "int ()")
		.def("getCombatPercent", &CvPromotionInfo::getCombatPercent, "int ()")
		.def("getCityAttackPercent", &CvPromotionInfo::getCityAttackPercent, "int ()")
		.def("getCityDefensePercent", &CvPromotionInfo::getCityDefensePercent, "int ()")
		.def("getHillsAttackPercent", &CvPromotionInfo::getHillsAttackPercent, "int ()")
		.def("getHillsDefensePercent", &CvPromotionInfo::getHillsDefensePercent, "int ()")
		.def("getCommandType", &CvPromotionInfo::getCommandType, "int ()")
		.def("getRevoltProtection", &CvPromotionInfo::getRevoltProtection, "int ()")
		.def("getCollateralDamageProtection", &CvPromotionInfo::getCollateralDamageProtection, "int ()")
		.def("getPillageChange", &CvPromotionInfo::getPillageChange, "int ()")
		.def("getUpgradeDiscount", &CvPromotionInfo::getUpgradeDiscount, "int ()")
		.def("getExperiencePercent", &CvPromotionInfo::getExperiencePercent, "int ()")
		.def("getKamikazePercent", &CvPromotionInfo::getKamikazePercent, "int ()")

		.def("isLeader", &CvPromotionInfo::isLeader, "bool ()")
		.def("getBlitz", &CvPromotionInfo::getBlitz, "int ()") // advc.164
		.def("isAmphib", &CvPromotionInfo::isAmphib, "bool ()")
		.def("isRiver", &CvPromotionInfo::isRiver, "bool ()")
		.def("isEnemyRoute", &CvPromotionInfo::isEnemyRoute, "bool ()")
		.def("isAlwaysHeal", &CvPromotionInfo::isAlwaysHeal, "bool ()")
		.def("isHillsDoubleMove", &CvPromotionInfo::isHillsDoubleMove, "bool ()")
		.def("isImmuneToFirstStrikes", &CvPromotionInfo::isImmuneToFirstStrikes, "bool ()")

		.def("getSound", &CvPromotionInfo::getSound, "string ()")

		// Arrays

		.def("getTerrainAttackPercent", &CvPromotionInfo::getTerrainAttackPercent, "int (int i)")
		.def("getTerrainDefensePercent", &CvPromotionInfo::getTerrainDefensePercent, "int (int i)")
		.def("getFeatureAttackPercent", &CvPromotionInfo::getFeatureAttackPercent, "int (int i)")
		.def("getFeatureDefensePercent", &CvPromotionInfo::getFeatureDefensePercent, "int (int i)")
		.def("getUnitCombatModifierPercent", &CvPromotionInfo::getUnitCombatModifierPercent, "int (int i)")
		.def("getDomainModifierPercent", &CvPromotionInfo::getDomainModifierPercent, "int (int i)")

		.def("getTerrainDoubleMove", &CvPromotionInfo::getTerrainDoubleMove, "bool (int i)")
		.def("getFeatureDoubleMove", &CvPromotionInfo::getFeatureDoubleMove, "bool (int i)")
		.def("getUnitCombat", &CvPromotionInfo::getUnitCombat, "bool (int i)")
		;

	python::class_<CvMissionInfo, boost::noncopyable, python::bases<CvInfoBase> >("CvMissionInfo")
		.def("getTime", &CvMissionInfo::getTime, "int ()")

		.def("isSound", &CvMissionInfo::isSound, "bool ()")
		.def("isTarget", &CvMissionInfo::isTarget, "bool ()")
		.def("isBuild", &CvMissionInfo::isBuild, "bool ()")
		.def("getVisible", &CvMissionInfo::getVisible, "bool ()")

		.def("getWaypoint", &CvMissionInfo::getWaypoint, "string ()")
		;

	python::class_<CvActionInfo, boost::noncopyable>("CvActionInfo")
		.def("getMissionData", &CvActionInfo::getMissionData, "int ()")
		.def("getCommandData", &CvActionInfo::getCommandData, "int ()")
		.def("getAutomateType", &CvActionInfo::getAutomateType, "int ()")

		.def("getInterfaceModeType", &CvActionInfo::getInterfaceModeType, "int ()")
		.def("getMissionType", &CvActionInfo::getMissionType, "int ()")
		.def("getCommandType", &CvActionInfo::getCommandType, "int ()")
		.def("getControlType", &CvActionInfo::getControlType, "int ()")

		.def("isConfirmCommand", &CvActionInfo::isConfirmCommand, "bool ()")
		.def("isVisible", &CvActionInfo::isVisible, "bool ()")

		.def("getHotKey", &CvActionInfo::getHotKey, "string ()")
		.def("getButton", &CvActionInfo::getButton, "string ()")
		;

	python::class_<CvUnitInfo, boost::noncopyable, python::bases<CvInfoBase, CvScalableInfo> >("CvUnitInfo")

/****************************************
 *  Archid Mod: 10 Jun 2012
 *  Functionality: Unit Civic Prereq - Archid
 *		Based on code by Afforess
 *	Source:
 *	  http://forums.civfanatics.com/downloads.php?do=file&id=15508
 *
 ****************************************/
		.def("isPrereqOrCivics", &CvUnitInfo::isPrereqOrCivics, "bool (int i)")
		.def("isPrereqAndCivics", &CvUnitInfo::isPrereqAndCivics, "bool (int i)")
/**
 ** End: Unit Civic Prereq
 **/	.def("getAIWeight", &CvUnitInfo::getAIWeight, "int ()")
		.def("getProductionCost", &CvUnitInfo::getProductionCost, "int ()")
		.def("getHurryCostModifier", &CvUnitInfo::getHurryCostModifier, "int ()")
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
		.def("getNumCitySizeUnitPrereq", &CvUnitInfo::getNumCitySizeUnitPrereq, "int ()")
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
		.def("getMinAreaSize", &CvUnitInfo::getMinAreaSize, "int ()")
		.def("getMoves", &CvUnitInfo::getMoves, "int ()")
		.def("getAirRange", &CvUnitInfo::getAirRange, "int ()")
		.def("getAirUnitCap", &CvUnitInfo::getAirUnitCap, "int ()")
		.def("getDropRange", &CvUnitInfo::getDropRange, "int ()")
		.def("getNukeRange", &CvUnitInfo::getNukeRange, "int ()")
		.def("getWorkRate", &CvUnitInfo::getWorkRate, "int ()")
		.def("getBaseDiscover", &CvUnitInfo::getBaseDiscover, "int ()")
		.def("getDiscoverMultiplier", &CvUnitInfo::getDiscoverMultiplier, "int ()")
		.def("getBaseHurry", &CvUnitInfo::getBaseHurry, "int ()")
		.def("getHurryMultiplier", &CvUnitInfo::getHurryMultiplier, "int ()")
		.def("getBaseTrade", &CvUnitInfo::getBaseTrade, "int ()")
		.def("getTradeMultiplier", &CvUnitInfo::getTradeMultiplier, "int ()")
		.def("getGreatWorkCulture", &CvUnitInfo::getGreatWorkCulture, "int ()")
		.def("getEspionagePoints", &CvUnitInfo::getEspionagePoints, "int ()")
		.def("getCombat", &CvUnitInfo::getCombat, "int ()")
		.def("setCombat", &CvUnitInfo::setCombat, "void (int)")
		.def("getCombatLimit", &CvUnitInfo::getCombatLimit, "int ()")
		.def("getAirCombat", &CvUnitInfo::getAirCombat, "int ()")
		.def("getAirCombatLimit", &CvUnitInfo::getAirCombatLimit, "int ()")
		.def("getXPValueAttack", &CvUnitInfo::getXPValueAttack, "int ()")
		.def("getXPValueDefense", &CvUnitInfo::getXPValueDefense, "int ()")
		.def("getFirstStrikes", &CvUnitInfo::getFirstStrikes, "int ()")
		.def("getChanceFirstStrikes", &CvUnitInfo::getChanceFirstStrikes, "int ()")
		.def("getInterceptionProbability", &CvUnitInfo::getInterceptionProbability, "int ()")
		.def("getEvasionProbability", &CvUnitInfo::getEvasionProbability, "int ()")
		.def("getWithdrawalProbability", &CvUnitInfo::getWithdrawalProbability, "int ()")
		.def("getCollateralDamage", &CvUnitInfo::getCollateralDamage, "int ()")
		.def("getCollateralDamageLimit", &CvUnitInfo::getCollateralDamageLimit, "int ()")
		.def("getCollateralDamageMaxUnits", &CvUnitInfo::getCollateralDamageMaxUnits, "int ()")
		.def("getCityAttackModifier", &CvUnitInfo::getCityAttackModifier, "int ()")
		.def("getCityDefenseModifier", &CvUnitInfo::getCityDefenseModifier, "int ()")
		.def("getAnimalCombatModifier", &CvUnitInfo::getAnimalCombatModifier, "int ()")
		.def("getHillsAttackModifier", &CvUnitInfo::getHillsAttackModifier, "int ()")
		.def("getHillsDefenseModifier", &CvUnitInfo::getHillsDefenseModifier, "int ()")
		.def("getBombRate", &CvUnitInfo::getBombRate, "int ()")
		.def("getBombardRate", &CvUnitInfo::getBombardRate, "int ()")
		.def("getSpecialCargo", &CvUnitInfo::getSpecialCargo, "int ()")
		.def("getDomainCargo", &CvUnitInfo::getDomainCargo, "int ()")

		.def("getCargoSpace", &CvUnitInfo::getCargoSpace, "int ()")
		.def("getConscriptionValue", &CvUnitInfo::getConscriptionValue, "int ()")
		.def("getCultureGarrisonValue", &CvUnitInfo::getCultureGarrisonValue, "int ()")
		.def("getExtraCost", &CvUnitInfo::getExtraCost, "int ()")
		.def("getAssetValue", &CvUnitInfo::getAssetValue, "int ()")
		.def("getPowerValue", &CvUnitInfo::getPowerValue, "int ()")
		.def("getUnitClassType", &CvUnitInfo::getUnitClassType, "int ()")
		.def("getSpecialUnitType", &CvUnitInfo::getSpecialUnitType, "int ()")
		.def("getUnitCaptureClassType", &CvUnitInfo::getUnitCaptureClassType, "int ()")
		.def("getUnitCombatType", &CvUnitInfo::getUnitCombatType, "int ()")
		.def("getDomainType", &CvUnitInfo::getDomainType, "int ()")
		.def("getDefaultUnitAIType", &CvUnitInfo::getDefaultUnitAIType, "int ()")
		.def("getInvisibleType", &CvUnitInfo::getInvisibleType, "int ()")
		.def("getNumSeeInvisibleTypes", &CvUnitInfo::getNumSeeInvisibleTypes, "int ()")
		.def("getSeeInvisibleType", &CvUnitInfo::getSeeInvisibleType, "int (int)")
		.def("getAdvisorType", &CvUnitInfo::getAdvisorType, "int ()")
/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* CanTrain                                                                                     */
/************************************************************************************************/
		//MISSING CIVICS
		.def("getMaxStartEra", &CvUnitInfo::getMaxStartEra, "int ()")
		.def("getForceObsoleteTech", &CvUnitInfo::getForceObsoleteTech, "int ()")
		.def("isStateReligion", &CvUnitInfo::isStateReligion, "bool ()")
		.def("getPrereqGameOption", &CvUnitInfo::getPrereqGameOption, "int ()")
		.def("getNotGameOption", &CvUnitInfo::getNotGameOption, "int ()")
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/
		.def("getHolyCity", &CvUnitInfo::getHolyCity, "int ()")
		.def("getReligionType", &CvUnitInfo::getReligionType, "int ()")
		.def("getStateReligion", &CvUnitInfo::getStateReligion, "int ()")
		.def("getPrereqReligion", &CvUnitInfo::getPrereqReligion, "int ()")
		.def("getPrereqCorporation", &CvUnitInfo::getPrereqCorporation, "int ()")
		.def("getPrereqBuilding", &CvUnitInfo::getPrereqBuilding, "int ()")
		.def("getPrereqAndTech", &CvUnitInfo::getPrereqAndTech, "int ()")
		.def("getPrereqAndBonus", &CvUnitInfo::getPrereqAndBonus, "int ()")
		.def("getGroupSize", &CvUnitInfo::getGroupSize, "int ()")
		.def("getGroupDefinitions", &CvUnitInfo::getGroupDefinitions, "int ()")
		.def("getMeleeWaveSize", &CvUnitInfo::getMeleeWaveSize, "int ()")
		.def("getRangedWaveSize", &CvUnitInfo::getRangedWaveSize, "int ()")
		.def("getNumUnitNames", &CvUnitInfo::getNumUnitNames, "int ()")
		.def("getCommandType", &CvUnitInfo::getCommandType, "int ()")

		.def("isAnimal", &CvUnitInfo::isAnimal, "bool ()")
		.def("isFoodProduction", &CvUnitInfo::isFoodProduction, "bool ()")
		.def("isNoBadGoodies", &CvUnitInfo::isNoBadGoodies, "bool ()")
		// UNOFFICIAL_PATCH, Bugfix, 03/20/10, Afforess & jdog5000:
		.def("isOnlyDefensive", &CvUnitInfo::isOnlyDefensive, "bool ()")
		.def("isNoCapture", &CvUnitInfo::isNoCapture, "bool ()")
		.def("isRivalTerritory", &CvUnitInfo::isRivalTerritory, "bool ()")
		.def("isMilitaryHappiness", &CvUnitInfo::isMilitaryHappiness, "bool ()")
		.def("isMilitarySupport", &CvUnitInfo::isMilitarySupport, "bool ()")
		.def("isMilitaryProduction", &CvUnitInfo::isMilitaryProduction, "bool ()")
		.def("isPillage", &CvUnitInfo::isPillage, "bool ()")
		.def("isSpy", &CvUnitInfo::isSpy, "bool ()")
		.def("isSabotage", &CvUnitInfo::isSabotage, "bool ()")
		.def("isDestroy", &CvUnitInfo::isDestroy, "bool ()")
		.def("isStealPlans", &CvUnitInfo::isStealPlans, "bool ()")
		.def("isInvestigate", &CvUnitInfo::isInvestigate, "bool ()")
		.def("isCounterSpy", &CvUnitInfo::isCounterSpy, "bool ()")
		.def("isFound", &CvUnitInfo::isFound, "bool ()")
		.def("isGoldenAge", &CvUnitInfo::isGoldenAge, "bool ()")
		.def("isInvisible", &CvUnitInfo::isInvisible, "bool ()")
		.def("setInvisible", &CvUnitInfo::setInvisible, "void (bool bEnable)")
		.def("isFirstStrikeImmune", &CvUnitInfo::isFirstStrikeImmune, "bool ()")
		.def("isNoDefensiveBonus", &CvUnitInfo::isNoDefensiveBonus, "bool ()")
		.def("isIgnoreBuildingDefense", &CvUnitInfo::isIgnoreBuildingDefense, "bool ()")
		.def("isCanMoveImpassable", &CvUnitInfo::canMoveImpassable, "bool ()")
		.def("isCanMoveAllTerrain", &CvUnitInfo::isCanMoveAllTerrain, "bool ()")
		.def("isFlatMovementCost", &CvUnitInfo::isFlatMovementCost, "bool ()")
		.def("isIgnoreTerrainCost", &CvUnitInfo::isIgnoreTerrainCost, "bool ()")
		.def("isNukeImmune", &CvUnitInfo::isNukeImmune, "bool ()")
		.def("isPrereqBonuses", &CvUnitInfo::isPrereqBonuses, "bool ()")
		.def("isPrereqReligion", &CvUnitInfo::isPrereqReligion, "bool ()")
		.def("isMechUnit", &CvUnitInfo::isMechUnit, "bool ()")
		.def("isRenderBelowWater", &CvUnitInfo::isRenderBelowWater, "bool ()")
		.def("isSuicide", &CvUnitInfo::isSuicide, "bool ()")
		.def("isLineOfSight", &CvUnitInfo::isLineOfSight, "bool ()")
		.def("isHiddenNationality", &CvUnitInfo::isHiddenNationality, "bool ()")
		.def("isAlwaysHostile", &CvUnitInfo::isAlwaysHostile, "bool ()")

		.def("getUnitMaxSpeed", &CvUnitInfo::getUnitMaxSpeed, "float ()")
		.def("getUnitPadTime", &CvUnitInfo::getUnitPadTime, "float ()")

		// Arrays

		.def("getPrereqAndTechs", &CvUnitInfo::getPrereqAndTechs, "int (int i)")
		.def("getPrereqOrBonuses", &CvUnitInfo::getPrereqOrBonuses, "int (int i)")
		// <advc.905b>
		.def("getSpeedBonuses", &CvUnitInfo::getSpeedBonuses, "int (int i)")
		.def("getExtraMoves", &CvUnitInfo::getExtraMoves, "int (int i)")
		// </advc.905b>
		.def("getProductionTraits", &CvUnitInfo::getProductionTraits, "int (int i)")
		.def("getFlavorValue", &CvUnitInfo::getFlavorValue, "int (int i)")
		.def("getTerrainAttackModifier", &CvUnitInfo::getTerrainAttackModifier, "int (int i)")
		.def("getTerrainDefenseModifier", &CvUnitInfo::getTerrainDefenseModifier, "int (int i)")
		.def("getFeatureAttackModifier", &CvUnitInfo::getFeatureAttackModifier, "int (int i)")
		.def("getFeatureDefenseModifier", &CvUnitInfo::getFeatureDefenseModifier, "int (int i)")
		.def("getUnitClassAttackModifier", &CvUnitInfo::getUnitClassAttackModifier, "int (int i)")
		.def("getUnitClassDefenseModifier", &CvUnitInfo::getUnitClassDefenseModifier, "int (int i)")
		.def("getUnitCombatModifier", &CvUnitInfo::getUnitCombatModifier, "int (int i)")
		.def("getDomainModifier", &CvUnitInfo::getDomainModifier, "int (int i)")
		.def("getBonusProductionModifier", &CvUnitInfo::getBonusProductionModifier, "int (int i)")
		.def("getUnitGroupRequired", &CvUnitInfo::getUnitGroupRequired, "int (int i)")
/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* CanTrain                                                                                     */
/************************************************************************************************/
//		.def("isPrereqOrCivics", &CvUnitInfo::isPrereqOrCivics, "bool (int i)")
		.def("isPrereqBuildingClass", &CvUnitInfo::isPrereqBuildingClass, "bool (int i)")
		.def("getPrereqBuildingClassOverrideTech", &CvUnitInfo::getPrereqBuildingClassOverrideTech, "int (int i)")
		.def("getPrereqBuildingClassOverrideEra", &CvUnitInfo::getPrereqBuildingClassOverrideEra, "int (int i)")
		.def("getForceObsoleteUnitClass", &CvUnitInfo::getForceObsoleteUnitClass, "bool (int i)")
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/
		.def("getUpgradeUnitClass", &CvUnitInfo::getUpgradeUnitClass, "bool (int i)")
		.def("getTargetUnitClass", &CvUnitInfo::getTargetUnitClass, "bool (int i)")
		.def("getTargetUnitCombat", &CvUnitInfo::getTargetUnitCombat, "bool (int i)")
		.def("getDefenderUnitClass", &CvUnitInfo::getDefenderUnitClass, "bool (int i)")
		.def("getDefenderUnitCombat", &CvUnitInfo::getDefenderUnitCombat, "bool (int i)")
		.def("getUnitAIType", &CvUnitInfo::getUnitAIType, "bool (int i)")
		.def("getNotUnitAIType", &CvUnitInfo::getNotUnitAIType, "bool (int i)")
		.def("getBuilds", &CvUnitInfo::getBuilds, "bool (int i)")
		.def("getReligionSpreads", &CvUnitInfo::getReligionSpreads, "int (int i)")
		.def("getCorporationSpreads", &CvUnitInfo::getCorporationSpreads, "int (int i)")
		.def("getTerrainPassableTech", &CvUnitInfo::getTerrainPassableTech, "int (int i)")
		.def("getFeaturePassableTech", &CvUnitInfo::getFeaturePassableTech, "int (int i)")
		.def("getFlankingStrikeUnitClass", &CvUnitInfo::getFlankingStrikeUnitClass, "int (int i)")
		.def("getGreatPeoples", &CvUnitInfo::getGreatPeoples, "bool (int i)")
		.def("getBuildings", &CvUnitInfo::getBuildings, "bool (int i)")
		// advc.003t:
		.def("getForceBuildings", &CvUnitInfo::getForceBuildings, "bool (int i)")
		.def("getTerrainImpassable", &CvUnitInfo::getTerrainImpassable, "bool (int i)")
		.def("getFeatureImpassable", &CvUnitInfo::getFeatureImpassable, "bool (int i)")
		.def("getTerrainNative", &CvUnitInfo::getTerrainNative, "bool (int i)")
		.def("getFeatureNative", &CvUnitInfo::getFeatureNative, "bool (int i)")
		.def("getFreePromotions", &CvUnitInfo::getFreePromotions, "bool (int i)")
		.def("getLeaderPromotion", &CvUnitInfo::getLeaderPromotion, "int ()")
		.def("getLeaderExperience", &CvUnitInfo::getLeaderExperience, "int ()")

		.def("getEarlyArtDefineTag", &CvUnitInfo::getEarlyArtDefineTag, "string (int i, UnitArtStyleTypes eStyle)")
		.def("getLateArtDefineTag", &CvUnitInfo::getLateArtDefineTag, "string (int i, UnitArtStyleTypes eStyle)")
		.def("getMiddleArtDefineTag", &CvUnitInfo::getMiddleArtDefineTag, "string (int i, UnitArtStyleTypes eStyle)")
		.def("getUnitNames", &CvUnitInfo::getUnitNames, "string (int i)")
		.def("getArtInfo", &CvUnitInfo::getArtInfo,  python::return_value_policy<python::reference_existing_object>(), "CvArtInfoUnit* (int i, bool bLate)")
		;

	python::class_<CvSpecialUnitInfo, boost::noncopyable, python::bases<CvInfoBase> >("CvSpecialUnitInfo")
		.def("isValid", &CvSpecialUnitInfo::isValid, "bool ()")
		.def("isCityLoad", &CvSpecialUnitInfo::isCityLoad, "bool ()")

		.def("isCarrierUnitAIType", &CvSpecialUnitInfo::isCarrierUnitAIType, "bool (int i)")
		.def("getProductionTraits", &CvSpecialUnitInfo::getProductionTraits, "int (int i)")
		;

	python::class_<CvCivicOptionInfo, boost::noncopyable, python::bases<CvInfoBase> >("CvCivicOptionInfo")
		.def("getTraitNoUpkeep", &CvCivicOptionInfo::getTraitNoUpkeep, "bool (int i)")
		;

	python::class_<CvCivicInfo, boost::noncopyable, python::bases<CvInfoBase> >("CvCivicInfo")

		.def("getCivicOptionType", &CvCivicInfo::getCivicOptionType, "int ()")
		.def("getAnarchyLength", &CvCivicInfo::getAnarchyLength, "int ()")
		.def("getUpkeep", &CvCivicInfo::getUpkeep, "int ()")
		.def("getAIWeight", &CvCivicInfo::getAIWeight, "int ()")
		.def("getGreatPeopleRateModifier", &CvCivicInfo::getGreatPeopleRateModifier, "int ()")
		.def("getGreatGeneralRateModifier", &CvCivicInfo::getGreatGeneralRateModifier, "int ()")
		.def("getDomesticGreatGeneralRateModifier", &CvCivicInfo::getDomesticGreatGeneralRateModifier, "int ()")
		.def("getStateReligionGreatPeopleRateModifier", &CvCivicInfo::getStateReligionGreatPeopleRateModifier, "int ()")
		.def("getDistanceMaintenanceModifier", &CvCivicInfo::getDistanceMaintenanceModifier, "int ()")
		.def("getNumCitiesMaintenanceModifier", &CvCivicInfo::getNumCitiesMaintenanceModifier, "int ()")
		.def("getCorporationMaintenanceModifier", &CvCivicInfo::getCorporationMaintenanceModifier, "int ()")
		.def("getExtraHealth", &CvCivicInfo::getExtraHealth, "int ()")
		.def("getExtraHappiness", &CvCivicInfo::getExtraHappiness, "int ()") // K-Mod
		.def("getFreeExperience", &CvCivicInfo::getFreeExperience, "int ()")
		.def("getWorkerSpeedModifier", &CvCivicInfo::getWorkerSpeedModifier, "int ()")
		.def("getImprovementUpgradeRateModifier", &CvCivicInfo::getImprovementUpgradeRateModifier, "int ()")
		.def("getMilitaryProductionModifier", &CvCivicInfo::getMilitaryProductionModifier, "int ()")
		.def("getBaseFreeUnits", &CvCivicInfo::getBaseFreeUnits, "int ()")
		.def("getBaseFreeMilitaryUnits", &CvCivicInfo::getBaseFreeMilitaryUnits, "int ()")
		.def("getFreeUnitsPopulationPercent", &CvCivicInfo::getFreeUnitsPopulationPercent, "int ()")
		.def("getFreeMilitaryUnitsPopulationPercent", &CvCivicInfo::getFreeMilitaryUnitsPopulationPercent, "int ()")
		.def("getGoldPerUnit", &CvCivicInfo::getGoldPerUnit, "int ()")
		.def("getGoldPerMilitaryUnit", &CvCivicInfo::getGoldPerMilitaryUnit, "int ()")
		.def("getHappyPerMilitaryUnit", &CvCivicInfo::getHappyPerMilitaryUnit, "int ()")
		// advc.912c:
		.def("getLuxuryModifier", &CvCivicInfo::getLuxuryModifier, "int ()")
		.def("getLargestCityHappiness", &CvCivicInfo::getLargestCityHappiness, "int ()")
		.def("getWarWearinessModifier", &CvCivicInfo::getWarWearinessModifier, "int ()")
		.def("getFreeSpecialist", &CvCivicInfo::getFreeSpecialist, "int ()")
		.def("getTradeRoutes", &CvCivicInfo::getTradeRoutes, "int ()")
		.def("getTechPrereq", &CvCivicInfo::getTechPrereq, "int ()")
		.def("getCivicPercentAnger", &CvCivicInfo::getCivicPercentAnger, "int ()")
		.def("getMaxConscript", &CvCivicInfo::getMaxConscript, "int ()")
		.def("getStateReligionHappiness", &CvCivicInfo::getStateReligionHappiness, "int ()")
		.def("getNonStateReligionHappiness", &CvCivicInfo::getNonStateReligionHappiness, "int ()")
		// < Civic Infos Plus Start > 
		.def("getStateReligionExtraHealth", &CvCivicInfo::getStateReligionExtraHealth, "int ()")
		.def("getNonStateReligionExtraHealth", &CvCivicInfo::getNonStateReligionExtraHealth, "int ()")
		// < Civic Infos Plus End   >
		.def("getStateReligionUnitProductionModifier", &CvCivicInfo::getStateReligionUnitProductionModifier, "int ()")
		.def("getStateReligionBuildingProductionModifier", &CvCivicInfo::getStateReligionBuildingProductionModifier, "int ()")
		.def("getStateReligionFreeExperience", &CvCivicInfo::getStateReligionFreeExperience, "int ()")
		.def("getExpInBorderModifier", &CvCivicInfo::getExpInBorderModifier, "bool ()")

		.def("isMilitaryFoodProduction", &CvCivicInfo::isMilitaryFoodProduction, "bool ()")
		//.def("isNoUnhealthyPopulation", &CvCivicInfo::isNoUnhealthyPopulation, "bool ()")
		.def("getUnhealthyPopulationModifier", &CvCivicInfo::getUnhealthyPopulationModifier, "int ()") // K-Mod
		.def("isBuildingOnlyHealthy", &CvCivicInfo::isBuildingOnlyHealthy, "bool ()")
		.def("isNoForeignTrade", &CvCivicInfo::isNoForeignTrade, "bool ()")
		.def("isNoCorporations", &CvCivicInfo::isNoCorporations, "bool ()")
		.def("isNoForeignCorporations", &CvCivicInfo::isNoForeignCorporations, "bool ()")
		.def("isStateReligion", &CvCivicInfo::isStateReligion, "bool ()")
		.def("isNoNonStateReligionSpread", &CvCivicInfo::isNoNonStateReligionSpread, "bool ()")

		.def("pyGetWeLoveTheKing", &CvCivicInfo::pyGetWeLoveTheKing, "wstring ()")

		// Arrays

		.def("getYieldModifier", &CvCivicInfo::getYieldModifier, "int (int i)")
		.def("getCapitalYieldModifier", &CvCivicInfo::getCapitalYieldModifier, "int (int i)")
		.def("getTradeYieldModifier", &CvCivicInfo::getTradeYieldModifier, "int (int i)")
		// < Civic Infos Plus Start >
		.def("getSpecialistExtraYields", &CvCivicInfo::getSpecialistExtraYield, "int (int i)")
		.def("getFreeSpecialistCount", &CvCivicInfo::getFreeSpecialistCount, "int (int i)")
		.def("getStateReligionYieldModifier", &CvCivicInfo::getStateReligionYieldModifier, "int (int i)")
		.def("getStateReligionCommerceModifier", &CvCivicInfo::getStateReligionCommerceModifier, "int (int i)")
		.def("getNonStateReligionYieldModifier", &CvCivicInfo::getNonStateReligionYieldModifier, "int (int i)")
		.def("getNonStateReligionCommerceModifier", &CvCivicInfo::getNonStateReligionCommerceModifier, "int (int i)")
		.def("getBuildingYieldChanges", &CvCivicInfo::getBuildingYieldChanges, "int (int i, int j)")
		.def("getBuildingCommerceChanges", &CvCivicInfo::getBuildingCommerceChanges, "int (int i, int j)")
		// < Civic Infos Plus End   >
		.def("getCommerceModifier", &CvCivicInfo::getCommerceModifier, "int (int i)")
		.def("getCapitalCommerceModifier", &CvCivicInfo::getCapitalCommerceModifier, "int (int i)")
		.def("getSpecialistExtraCommerce", &CvCivicInfo::getSpecialistExtraCommerce, "int (int i)")
		.def("getBuildingHappinessChanges", &CvCivicInfo::getBuildingHappinessChanges, "int (int i)")
		.def("getBuildingHealthChanges", &CvCivicInfo::getBuildingHealthChanges, "int (int i)")
		.def("getFeatureHappinessChanges", &CvCivicInfo::getFeatureHappinessChanges, "int (int i)")

		.def("isHurry", &CvCivicInfo::isHurry, "bool (int i)")
		.def("isSpecialBuildingNotRequired", &CvCivicInfo::isSpecialBuildingNotRequired, "bool (int i)")
		.def("isSpecialistValid", &CvCivicInfo::isSpecialistValid, "bool (int i)")

		.def("getImprovementYieldChanges", &CvCivicInfo::getImprovementYieldChanges, "int (int i, int j)")
		;

	python::class_<CvUnitClassInfo, boost::noncopyable, python::bases<CvInfoBase> >("CvUnitClassInfo")
		.def("getMaxGlobalInstances", &CvUnitClassInfo::getMaxGlobalInstances, "int ()")
		.def("getMaxTeamInstances", &CvUnitClassInfo::getMaxTeamInstances, "int ()")
		.def("getMaxPlayerInstances", &CvUnitClassInfo::getMaxPlayerInstances, "int ()")
		.def("getInstanceCostModifier", &CvUnitClassInfo::getInstanceCostModifier, "int ()")
		.def("getDefaultUnitIndex", &CvUnitClassInfo::getDefaultUnit, "int ()")
		;

	python::class_<CvBuildingInfo, boost::noncopyable, python::bases<CvInfoBase, CvScalableInfo> >("CvBuildingInfo")

		.def("getBuildingClassType", &CvBuildingInfo::getBuildingClassType, "int ()")
		.def("getVictoryPrereq", &CvBuildingInfo::getVictoryPrereq, "int ()")
		.def("getFreeStartEra", &CvBuildingInfo::getFreeStartEra, "int ()")
		.def("getMaxStartEra", &CvBuildingInfo::getMaxStartEra, "int ()")
		.def("getObsoleteTech", &CvBuildingInfo::getObsoleteTech, "int ()")
		.def("getPrereqAndTech", &CvBuildingInfo::getPrereqAndTech, "int ()")
		.def("getNoBonus", &CvBuildingInfo::getNoBonus, "int ()")
		.def("getPowerBonus", &CvBuildingInfo::getPowerBonus, "int ()")
		.def("getFreeBonus", &CvBuildingInfo::getFreeBonus, "int ()")
		.def("getNumFreeBonuses", &CvBuildingInfo::getNumFreeBonuses, "int ()")
		.def("getFreeBuildingClass", &CvBuildingInfo::getFreeBuildingClass, "int ()")
		// < Building Resource Converter Start >
		.def("isBuildingOutputBonus", &CvBuildingInfo::isRequiredInputBonus, "bool (int i)")
		.def("getRequiredInputBonusValue", &CvBuildingInfo::getRequiredInputBonusValue, "int (int i)")
		.def("getRequiredInputBonusCount", &CvBuildingInfo::getRequiredInputBonusCount, "int ()")
		.def("isBuildingOutputBonus", &CvBuildingInfo::isBuildingOutputBonus, "bool (int i)")
		.def("getBuildingOutputBonusValues", &CvBuildingInfo::getBuildingOutputBonusValues, "int (int i)")
		.def("getBuildingOutputBonusCount", &CvBuildingInfo::getBuildingOutputBonusCount, "int ()")
		// < Building Resource Converter End   >

		.def("getFreePromotion", &CvBuildingInfo::getFreePromotion, "int ()")
		.def("getCivic", &CvBuildingInfo::getCivicOption, "int ()")
		.def("getAIWeight", &CvBuildingInfo::getAIWeight, "int ()")
		.def("getProductionCost", &CvBuildingInfo::getProductionCost, "int ()")
		.def("getHurryCostModifier", &CvBuildingInfo::getHurryCostModifier, "int ()")
		.def("getHurryAngerModifier", &CvBuildingInfo::getHurryAngerModifier, "int ()")
		.def("getMinAreaSize", &CvBuildingInfo::getMinAreaSize, "int ()")
		.def("getNumCitiesPrereq", &CvBuildingInfo::getNumCitiesPrereq, "int ()")
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
		.def("getNumCitySizeBldPrereq", &CvBuildingInfo::getNumCitySizeBldPrereq, "int ()")
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
		.def("getNumTeamsPrereq", &CvBuildingInfo::getNumTeamsPrereq, "int ()")
		.def("getUnitLevelPrereq", &CvBuildingInfo::getUnitLevelPrereq, "int ()")
		.def("getMinLatitude", &CvBuildingInfo::getMinLatitude, "int ()")
		.def("getMaxLatitude", &CvBuildingInfo::getMaxLatitude, "int ()")
		.def("getGreatPeopleRateModifier", &CvBuildingInfo::getGreatPeopleRateModifier, "int ()")
		.def("getGreatGeneralRateModifier", &CvBuildingInfo::getGreatGeneralRateModifier, "int ()")
		.def("getDomesticGreatGeneralRateModifier", &CvBuildingInfo::getDomesticGreatGeneralRateModifier, "int ()")
		.def("getGlobalGreatPeopleRateModifier", &CvBuildingInfo::getGlobalGreatPeopleRateModifier, "int ()")
		.def("getAnarchyModifier", &CvBuildingInfo::getAnarchyModifier, "int ()")
		.def("getGoldenAgeModifier", &CvBuildingInfo::getGoldenAgeModifier, "int ()")
		.def("getGlobalHurryModifier", &CvBuildingInfo::getGlobalHurryModifier, "int ()")
		.def("getFreeExperience", &CvBuildingInfo::getFreeExperience, "int ()")
		.def("getGlobalFreeExperience", &CvBuildingInfo::getGlobalFreeExperience, "int ()")
		.def("getFoodKept", &CvBuildingInfo::getFoodKept, "int ()")
		.def("getAirlift", &CvBuildingInfo::getAirlift, "int ()")
		.def("getAirModifier", &CvBuildingInfo::getAirModifier, "int ()")
		.def("getAirUnitCapacity", &CvBuildingInfo::getAirUnitCapacity, "int ()")
		.def("getNukeModifier", &CvBuildingInfo::getNukeModifier, "int ()")
		.def("getNukeExplosionRand", &CvBuildingInfo::getNukeExplosionRand, "int ()")
		.def("getFreeSpecialist", &CvBuildingInfo::getFreeSpecialist, "int ()")
		.def("getAreaFreeSpecialist", &CvBuildingInfo::getAreaFreeSpecialist, "int ()")
		.def("getGlobalFreeSpecialist", &CvBuildingInfo::getGlobalFreeSpecialist, "int ()")
		.def("getHappiness", &CvBuildingInfo::getHappiness, "int ()")
		.def("getAreaHappiness", &CvBuildingInfo::getAreaHappiness, "int ()")
		.def("getGlobalHappiness", &CvBuildingInfo::getGlobalHappiness, "int ()")
		.def("getStateReligionHappiness", &CvBuildingInfo::getStateReligionHappiness, "int ()")
		.def("getWorkerSpeedModifier", &CvBuildingInfo::getWorkerSpeedModifier, "int ()")
		.def("getMilitaryProductionModifier", &CvBuildingInfo::getMilitaryProductionModifier, "int ()")
		.def("getSpaceProductionModifier", &CvBuildingInfo::getSpaceProductionModifier, "int ()")
		.def("getGlobalSpaceProductionModifier", &CvBuildingInfo::getGlobalSpaceProductionModifier, "int ()")
		.def("getTradeRoutes", &CvBuildingInfo::getTradeRoutes, "int ()")
		.def("getCoastalTradeRoutes", &CvBuildingInfo::getCoastalTradeRoutes, "int ()")
		// advc.310: Renamed; was getGlobalTradeRoutes.
		.def("getAreaTradeRoutes", &CvBuildingInfo::getAreaTradeRoutes, "int ()")
		.def("getTradeRouteModifier", &CvBuildingInfo::getTradeRouteModifier, "int ()")
		.def("getForeignTradeRouteModifier", &CvBuildingInfo::getForeignTradeRouteModifier, "int ()")
		.def("getAssetValue", &CvBuildingInfo::getAssetValue, "int ()")
		.def("getPowerValue", &CvBuildingInfo::getPowerValue, "int ()")
		.def("getSpecialBuildingType", &CvBuildingInfo::getSpecialBuildingType, "int ()")
		.def("getAdvisorType", &CvBuildingInfo::getAdvisorType, "int ()")
/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* CanConstruct                                                                                 */
/************************************************************************************************/
		.def("getPrereqGameOption", &CvBuildingInfo::getPrereqGameOption, "int ()")
		.def("getNotGameOption", &CvBuildingInfo::getNotGameOption, "int ()")
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/
		.def("getHolyCity", &CvBuildingInfo::getHolyCity, "int ()")
		.def("getReligionType", &CvBuildingInfo::getReligionType, "int ()")
		.def("getStateReligion", &CvBuildingInfo::getStateReligion, "int ()")
		.def("getPrereqReligion", &CvBuildingInfo::getPrereqReligion, "int ()")
		.def("getPrereqCorporation", &CvBuildingInfo::getPrereqCorporation, "int ()")
		.def("getFoundsCorporation", &CvBuildingInfo::getFoundsCorporation, "int ()")
		.def("getGlobalReligionCommerce", &CvBuildingInfo::getGlobalReligionCommerce, "int ()")
		.def("getGlobalCorporationCommerce", &CvBuildingInfo::getGlobalCorporationCommerce, "int ()")
		.def("getPrereqAndBonus", &CvBuildingInfo::getPrereqAndBonus, "int ()")
		.def("getGreatPeopleUnitClass", &CvBuildingInfo::getGreatPeopleUnitClass, "int ()")
		.def("getGreatPeopleRateChange", &CvBuildingInfo::getGreatPeopleRateChange, "int ()")
		.def("getConquestProbability", &CvBuildingInfo::getConquestProbability, "int ()")
		.def("getMaintenanceModifier", &CvBuildingInfo::getMaintenanceModifier, "int ()")
		.def("getWarWearinessModifier", &CvBuildingInfo::getWarWearinessModifier, "int ()")
		.def("getGlobalWarWearinessModifier", &CvBuildingInfo::getGlobalWarWearinessModifier, "int ()")
		.def("getEnemyWarWearinessModifier", &CvBuildingInfo::getEnemyWarWearinessModifier, "int ()")
		.def("getHealRateChange", &CvBuildingInfo::getHealRateChange, "int ()")
		.def("getHealth", &CvBuildingInfo::getHealth, "int ()")
		.def("getAreaHealth", &CvBuildingInfo::getAreaHealth, "int ()")
		.def("getGlobalHealth", &CvBuildingInfo::getGlobalHealth, "int ()")
		.def("getGlobalPopulationChange", &CvBuildingInfo::getGlobalPopulationChange, "int ()")
		.def("getFreeTechs", &CvBuildingInfo::getFreeTechs, "int ()")
		.def("getDefenseModifier", &CvBuildingInfo::getDefenseModifier, "int ()")
		.def("getBombardDefenseModifier", &CvBuildingInfo::getBombardDefenseModifier, "int ()")
		.def("getAllCityDefenseModifier", &CvBuildingInfo::getAllCityDefenseModifier, "int ()")
		.def("getEspionageDefenseModifier", &CvBuildingInfo::getEspionageDefenseModifier, "int ()")
		.def("getMissionType", &CvBuildingInfo::getMissionType, "int ()")
		.def("getVoteSourceType", &CvBuildingInfo::getVoteSourceType, "int ()")

		.def("isTeamShare", &CvBuildingInfo::isTeamShare, "bool ()")
		.def("isWater", &CvBuildingInfo::isWater, "bool ()")
		.def("isRiver", &CvBuildingInfo::isRiver, "bool ()")
		.def("isPower", &CvBuildingInfo::isPower, "bool ()")
		.def("isDirtyPower", &CvBuildingInfo::isDirtyPower, "bool ()")
		.def("isAreaCleanPower", &CvBuildingInfo::isAreaCleanPower, "bool ()")
		.def("isAreaBorderObstacle", &CvBuildingInfo::isAreaBorderObstacle, "bool ()")
		.def("isForceTeamVoteEligible", &CvBuildingInfo::isForceTeamVoteEligible, "bool ()")
		.def("isCapital", &CvBuildingInfo::isCapital, "bool ()")
		.def("isGovernmentCenter", &CvBuildingInfo::isGovernmentCenter, "bool ()")
		.def("isGoldenAge", &CvBuildingInfo::isGoldenAge, "bool ()")
		.def("isMapCentering", &CvBuildingInfo::isMapCentering, "bool ()")
		.def("isNoUnhappiness", &CvBuildingInfo::isNoUnhappiness, "bool ()")
		//.def("isNoUnhealthyPopulation", &CvBuildingInfo::isNoUnhealthyPopulation, "bool ()")
		.def("getUnhealthyPopulationModifier", &CvBuildingInfo::getUnhealthyPopulationModifier, "int ()") // K-Mod
		.def("isBuildingOnlyHealthy", &CvBuildingInfo::isBuildingOnlyHealthy, "bool ()")
		.def("isNeverCapture", &CvBuildingInfo::isNeverCapture, "bool ()")
		.def("isNukeImmune", &CvBuildingInfo::isNukeImmune, "bool ()")
		.def("isPrereqReligion", &CvBuildingInfo::isPrereqReligion, "bool ()")
		.def("isCenterInCity", &CvBuildingInfo::isCenterInCity, "bool ()")
		.def("isStateReligion", &CvBuildingInfo::isStateReligion, "bool ()")
		.def("isAllowsNukes", &CvBuildingInfo::isAllowsNukes, "bool ()")

		.def("getConstructSound", &CvBuildingInfo::getConstructSound, "string ()")
		.def("getHotKey", &CvBuildingInfo::getHotKey, "string ()")
		.def("getHotKeyDescription", &CvBuildingInfo::getHotKeyDescription, "string ()")
		.def("getArtDefineTag", &CvBuildingInfo::getArtDefineTag, "string ()")
		.def("getMovie", &CvBuildingInfo::getMovie, "string ()")
		.def("getMovieDefineTag", &CvBuildingInfo::getMovieDefineTag, "string ()")
		// davidlallen: building bonus yield, commerce start
		.def("getBonusConsumed", &CvBuildingInfo::getBonusConsumed, "int ()")
		.def("getCommerceProduced", &CvBuildingInfo::getCommerceProduced, "int (int i)")
		.def("getYieldProduced", &CvBuildingInfo::getYieldProduced, "int (int i)")
		// davidlallen: building bonus yield, commerce end


		// Arrays

		.def("getYieldChange", &CvBuildingInfo::py_getYieldChange, "int (int i)")
		.def("getYieldModifier", &CvBuildingInfo::py_getYieldModifier, "int (int i)")
		.def("getPowerYieldModifier", &CvBuildingInfo::py_getPowerYieldModifier, "int (int i)")
		.def("getGlobalYieldModifier", &CvBuildingInfo::py_getGlobalYieldModifier, "int (int i)")
		.def("getSeaPlotYieldChange", &CvBuildingInfo::py_getSeaPlotYieldChange, "int (int i)")
		.def("getRiverPlotYieldChange", &CvBuildingInfo::py_getRiverPlotYieldChange, "int (int i)")
		.def("getGlobalSeaPlotYieldChange", &CvBuildingInfo::py_getGlobalSeaPlotYieldChange, "int (int i)")
		.def("getCommerceChange", &CvBuildingInfo::py_getCommerceChange, "int (int i)")
		.def("getObsoleteSafeCommerceChange", &CvBuildingInfo::py_getObsoleteSafeCommerceChange, "int (int i)")
		.def("getCommerceChangeDoubleTime", &CvBuildingInfo::py_getCommerceChangeDoubleTime, "int (int i)")
		.def("getCommerceModifier", &CvBuildingInfo::py_getCommerceModifier, "int (int i)")
		.def("getGlobalCommerceModifier", &CvBuildingInfo::py_getGlobalCommerceModifier, "int (int i)")
		.def("getStateReligionCommerce", &CvBuildingInfo::py_getStateReligionCommerce, "int (int i)")
		.def("getCommerceHappiness", &CvBuildingInfo::py_getCommerceHappiness, "int (int i)")
		.def("getReligionChange", &CvBuildingInfo::getReligionChange, "int (int i)")
		.def("getSpecialistCount", &CvBuildingInfo::getSpecialistCount, "int (int i)")
		.def("getFreeSpecialistCount", &CvBuildingInfo::getFreeSpecialistCount, "int (int i)")
		.def("getBonusHealthChanges", &CvBuildingInfo::getBonusHealthChanges, "int (int i)")
		.def("getBonusHappinessChanges", &CvBuildingInfo::getBonusHappinessChanges, "int (int i)")
		.def("getBonusProductionModifier", &CvBuildingInfo::getBonusProductionModifier, "int (int i)")
		.def("getUnitCombatFreeExperience", &CvBuildingInfo::getUnitCombatFreeExperience, "int (int i)")
		.def("getDomainFreeExperience", &CvBuildingInfo::getDomainFreeExperience, "int (int i)")
		.def("getDomainProductionModifier", &CvBuildingInfo::getDomainProductionModifier, "int (int i)")
		.def("getPrereqAndTechs", &CvBuildingInfo::getPrereqAndTechs, "int (int i)")
		.def("getPrereqOrBonuses", &CvBuildingInfo::getPrereqOrBonuses, "int (int i)")
		.def("getProductionTraits", &CvBuildingInfo::getProductionTraits, "int (int i)")
		.def("getHappinessTraits", &CvBuildingInfo::getHappinessTraits, "int (int i)")
		.def("getBuildingHappinessChanges", &CvBuildingInfo::getBuildingHappinessChanges, "int (int i)")
		.def("getPrereqNumOfBuildingClass", &CvBuildingInfo::getPrereqNumOfBuildingClass, "int (int i)")
		.def("getFlavorValue", &CvBuildingInfo::getFlavorValue, "int (int i)")
		.def("getImprovementFreeSpecialist", &CvBuildingInfo::getImprovementFreeSpecialist, "int (int i)")

		.def("isCommerceFlexible", &CvBuildingInfo::isCommerceFlexible, "bool (int i)")
		.def("isCommerceChangeOriginalOwner", &CvBuildingInfo::isCommerceChangeOriginalOwner, "bool (int i)")
		.def("isBuildingClassNeededInCity", &CvBuildingInfo::isBuildingClassNeededInCity, "bool (int i)")

		.def("getSpecialistYieldChange", &CvBuildingInfo::getSpecialistYieldChange, "int (int i, int j)")
		.def("getBonusYieldModifier", &CvBuildingInfo::getBonusYieldModifier, "int (int i, int j)")

		.def("getArtInfo", &CvBuildingInfo::getArtInfo,  python::return_value_policy<python::reference_existing_object>())
		;

	python::class_<CvSpecialBuildingInfo, boost::noncopyable, python::bases<CvInfoBase> >("CvSpecialBuildingInfo")
		.def("getObsoleteTech", &CvSpecialBuildingInfo::getObsoleteTech, "int ()")
		.def("getTechPrereq", &CvSpecialBuildingInfo::getTechPrereq, "int ()")
		.def("isValid", &CvSpecialBuildingInfo::isValid, "bool ()")

		// Arrays

		.def("getProductionTraits", &CvSpecialBuildingInfo::getProductionTraits, "int (int i)")
		;
}
