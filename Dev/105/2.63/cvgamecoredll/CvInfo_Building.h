#pragma once

#ifndef CV_INFO_BUILDING_H
#define CV_INFO_BUILDING_H

/*  advc.003x: Cut from CvInfos.h. Building-related classes:
	CvBuildingInfo
	CvBuildingClassInfo (for the mapping to CvBuildingInfo, CvCivilizationInfo gets precompiled)
	CvSpecialBuildingInfo
	CvVoteSourceInfo (tied to a building)
	CvVoteInfo (should stay with CvVoteSourceInfo)
	CvProjectInfo (very similar to a building) */
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvBuildingClassInfo  // advc: Moved up for inline function calls from CvBuilding
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildingClassInfo : public CvInfoBase
{
public: // All the const functions are exposed to Python; advc.inl: Inlined most of them.
	CvBuildingClassInfo();
	~CvBuildingClassInfo();

	inline int getMaxGlobalInstances() const
	{
		return m_iMaxGlobalInstances;
	}
	inline bool isWorldWonder() const // advc.003w: Replacing global isWorldWonderClass
	{
		return (getMaxGlobalInstances() != -1);
	}
	inline int getMaxTeamInstances() const
	{
		return m_iMaxTeamInstances;
	}
	inline bool isTeamWonder() const // advc.003w: Replacing global isTeamWonderClass
	{
		return (getMaxTeamInstances() != -1);
	}
	inline int getMaxPlayerInstances() const
	{
		return m_iMaxPlayerInstances;
	}
	inline bool isNationalWonder() const // advc.003w: Replacing global isNationalWonderClass
	{
		return (getMaxPlayerInstances() != -1);
	}
	inline bool isLimited() const // advc.003w: Replacing global isLimitedWonderClass
	{
		return (isWorldWonder() || isTeamWonder() || isNationalWonder());
	}
	int getExtraPlayerInstances() const
	{
		return m_iExtraPlayerInstances;
	}
	BuildingTypes getDefaultBuilding() const // advc.003x: Renamed from getDefaultBuildingIndex
	{
		return (BuildingTypes)m_iDefaultBuildingIndex;
	}
	bool isNoLimit() const
	{
		return m_bNoLimit;
	}
	int getLimit() const; // advc.003w: Replacing global limitedWonderClassLimit

	bool isMonument() const;
	int getVictoryThreshold(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	bool readPass3();

protected:
	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iMaxPlayerInstances;
	int m_iExtraPlayerInstances;
	int m_iDefaultBuildingIndex;

	bool m_bNoLimit;
	bool m_bMonument;

	int* m_piVictoryThreshold;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvBuildingInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildingInfo : public CvHotkeyInfo
{
public: /*	All the const functions are exposed to Python. advc.inl: Inlined most of those.
			Integers in signatures replaced with enum types (except for most of the array
			accessors - tbd.). */
	CvBuildingInfo();
	~CvBuildingInfo();
	// <advc.tag>
	enum IntElementTypes
	{
		RaiseDefense = CvHotkeyInfo::NUM_INT_ELEMENT_TYPES, // advc.004c
		NUM_INT_ELEMENT_TYPES
	};
	enum BoolElementTypes // unused so far
	{
		NUM_BOOL_ELEMENT_TYPES = CvHotkeyInfo::NUM_BOOL_ELEMENT_TYPES
	};
	using CvXMLInfo::get; // unhide
	__forceinline int get(IntElementTypes e) const
	{
		return get(static_cast<CvXMLInfo::IntElementTypes>(e));
	}
	__forceinline int get(BoolElementTypes e) const
	{
		return get(static_cast<CvXMLInfo::BoolElementTypes>(e));
	} // </advc.tag>

	inline BuildingClassTypes getBuildingClassType() const
	{
		return (BuildingClassTypes)m_iBuildingClassType;
	}
	int getVictoryPrereq() const { return m_iVictoryPrereq; }
	EraTypes getFreeStartEra() const { return (EraTypes)m_iFreeStartEra; }
	EraTypes getMaxStartEra() const { return (EraTypes)m_iMaxStartEra; }
	TechTypes getObsoleteTech() const { return (TechTypes)m_iObsoleteTech; }
	TechTypes getPrereqAndTech() const { return (TechTypes)m_iPrereqAndTech; }
	bool isTechRequired(TechTypes eTech) const; // advc.003w: Replacing global isTechRequiredForBuilding
	BonusTypes getNoBonus() const { return (BonusTypes)m_iNoBonus; }
	BonusTypes getPowerBonus() const { return (BonusTypes)m_iPowerBonus; }
	BonusTypes getFreeBonus() const { return (BonusTypes)m_iFreeBonus; }
	int getNumFreeBonuses() const { return m_iNumFreeBonuses; }
	// < Building Resource Converter Start >
	bool isRequiredInputBonus(int iBonus) const;			// Exposed to Python
	int getRequiredInputBonusValue(int iBonus) const;		// Exposed to Python
	int getRequiredInputBonusCount() const;						// Exposed to Python
	bool isBuildingOutputBonus(int iBonus) const;			// Exposed to Python
	int getBuildingOutputBonusValues(int iBonus) const;	// Exposed to Python
	int getBuildingOutputBonusCount() const;						// Exposed to Python
	// < Building Resource Converter End   >
	BuildingClassTypes getFreeBuildingClass() const { return (BuildingClassTypes)m_iFreeBuildingClass; }
	PromotionTypes getFreePromotion() const { return (PromotionTypes)m_iFreePromotion; }
	CivicOptionTypes getCivicOption() const { return (CivicOptionTypes)m_iCivicOption; }
	int getAIWeight() const { return m_iAIWeight; }
	int getProductionCost() const { return m_iProductionCost; }
	int getHurryCostModifier() const { return m_iHurryCostModifier; }
	int getHurryAngerModifier() const { return m_iHurryAngerModifier; }
	int getAdvancedStartCost() const { return m_iAdvancedStartCost; }
	int getAdvancedStartCostIncrease() const { return m_iAdvancedStartCostIncrease; }
	int getMinAreaSize() const { return m_iMinAreaSize; }
	int getNumCitiesPrereq() const { return m_iNumCitiesPrereq; }
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	int getNumCitySizeBldPrereq() const { return m_iNumCitySizeBldPrereq; }
	//ORG - keldath
	//int getNumCitySizeBldPrereq() const;  // Exposed to Python
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	int getNumTeamsPrereq() const { return m_iNumTeamsPrereq; }
	int getUnitLevelPrereq() const { return m_iUnitLevelPrereq; }
	int getMinLatitude() const { return m_iMinLatitude; }
	int getMaxLatitude() const { return m_iMaxLatitude; }
	int getGreatPeopleRateModifier() const { return m_iGreatPeopleRateModifier; }
	int getGreatGeneralRateModifier() const { return m_iGreatGeneralRateModifier; }
	int getDomesticGreatGeneralRateModifier() const;
	int getGlobalGreatPeopleRateModifier() const { return m_iGlobalGreatPeopleRateModifier; }
	int getAnarchyModifier() const { return m_iAnarchyModifier; }
	int getGoldenAgeModifier() const { return m_iGoldenAgeModifier; }
	int getGlobalHurryModifier() const { return m_iGlobalHurryModifier; }
	int getFreeExperience() const { return m_iFreeExperience; }
	int getGlobalFreeExperience() const { return m_iGlobalFreeExperience; }
	int getFoodKept() const { return m_iFoodKept; }
	/* Population Limit ModComp - Beginning */
	int getPopulationLimitChange() const { return m_iPopulationLimitChange; }
	//orgiginal - keldath
	//DllExport int getPopulationLimitChange() const;				// Exposed to Python
	/* Population Limit ModComp - End */
	int getAirlift() const { return m_iAirlift; }
	int getAirModifier() const { return m_iAirModifier; }
	int getAirUnitCapacity() const { return m_iAirUnitCapacity; }
	int getNukeModifier() const { return m_iNukeModifier; }
	int getNukeExplosionRand() const { return m_iNukeExplosionRand; }
	int getFreeSpecialist() const { return m_iFreeSpecialist; }
	int getAreaFreeSpecialist() const { return m_iAreaFreeSpecialist; }
	int getGlobalFreeSpecialist() const { return m_iGlobalFreeSpecialist; }
	int getHappiness() const { return m_iHappiness; }
	int getAreaHappiness() const { return m_iAreaHappiness; }
	int getGlobalHappiness() const { return m_iGlobalHappiness; }
	int getStateReligionHappiness() const { return m_iStateReligionHappiness; }
	int getWorkerSpeedModifier() const { return m_iWorkerSpeedModifier; }
	int getMilitaryProductionModifier() const { return m_iMilitaryProductionModifier; }
	int getSpaceProductionModifier() const { return m_iSpaceProductionModifier; }
	int getGlobalSpaceProductionModifier() const { return m_iGlobalSpaceProductionModifier; }
	int getTradeRoutes() const { return m_iTradeRoutes; }
	int getCoastalTradeRoutes() const { return m_iCoastalTradeRoutes; }
	int getAreaTradeRoutes() const; // advc.310: Renamed; was getGlobalTradeRoutes.
	int getTradeRouteModifier() const { return m_iTradeRouteModifier; }
	int getForeignTradeRouteModifier() const { return m_iForeignTradeRouteModifier; }
	int getAssetValue() const { return m_iAssetValue; }
	int getPowerValue() const { return m_iPowerValue; }
	SpecialBuildingTypes getSpecialBuildingType() const
	{
		return (SpecialBuildingTypes)m_iSpecialBuildingType;
	}
	AdvisorTypes getAdvisorType() const { return (AdvisorTypes)m_iAdvisorType; }
	
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanConstruct															*/
/********************************************************************************/
	int getPrereqGameOption() const { return m_iPrereqGameOption; }
	int getNotGameOption() const { return m_iNotGameOption; }
	//original - keldath
	//int getPrereqGameOption() const;				// Exposed to Python
	//int getNotGameOption() const;				// Exposed to Python
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/
	ReligionTypes getHolyCity() const { return (ReligionTypes)m_iHolyCity; }
	ReligionTypes getReligionType() const { return (ReligionTypes)m_iReligionType; }
	ReligionTypes getStateReligion() const { return (ReligionTypes)m_iStateReligion; }
	ReligionTypes getPrereqReligion() const { return (ReligionTypes)m_iPrereqReligion; }
	CorporationTypes getPrereqCorporation() const
	{
		return (CorporationTypes)m_iPrereqCorporation;
	}
	CorporationTypes getFoundsCorporation() const
	{
		return (CorporationTypes)m_iFoundsCorporation;
	}
	ReligionTypes getGlobalReligionCommerce() const
	{
		return (ReligionTypes)m_iGlobalReligionCommerce;
	}
	CorporationTypes getGlobalCorporationCommerce() const
	{
		return (CorporationTypes)m_iGlobalCorporationCommerce;
	}
	BonusTypes getPrereqAndBonus() const { return (BonusTypes)m_iPrereqAndBonus; }
//	int getPrereqVicinityBonus() const;  //Shqype Vicinity Bonus Add
	int getGreatPeopleUnitClass() const { return m_iGreatPeopleUnitClass; }
	int getGreatPeopleRateChange() const { return m_iGreatPeopleRateChange; }
	int getConquestProbability() const { return m_iConquestProbability; }
	int getMaintenanceModifier() const { return m_iMaintenanceModifier; }
	//DPII < Maintenance Modifiers >
	int getGlobalMaintenanceModifier() const;
    int getAreaMaintenanceModifier() const;
    int getOtherAreaMaintenanceModifier() const;
	int getDistanceMaintenanceModifier() const;
    int getNumCitiesMaintenanceModifier() const;
    int getCoastalDistanceMaintenanceModifier() const;
    int getConnectedCityMaintenanceModifier() const;
	//DPII < Maintenance Modifiers >
	int getWarWearinessModifier() const { return m_iWarWearinessModifier; }
	int getGlobalWarWearinessModifier() const { return m_iGlobalWarWearinessModifier; }
	int getEnemyWarWearinessModifier() const { return m_iEnemyWarWearinessModifier; }
	int getHealRateChange() const { return m_iHealRateChange; }
	int getHealth() const { return m_iHealth; }
	int getAreaHealth() const { return m_iAreaHealth; }
	int getGlobalHealth() const { return m_iGlobalHealth; }
	int getGlobalPopulationChange() const { return m_iGlobalPopulationChange; }
	int getFreeTechs() const { return m_iFreeTechs; }
/*** HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave START ***/
	int getFreeSpecificTech() const { return m_iFreeSpecificTech; }
	//original - keldath
	//int getFreeSpecificTech() const;
/*** HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave END ***/
	inline int getDefenseModifier() const { return m_iDefenseModifier; }
	int getBombardDefenseModifier() const { return m_iBombardDefenseModifier; }
	int getAllCityDefenseModifier() const { return m_iAllCityDefenseModifier; }
	int getEspionageDefenseModifier() const { return m_iEspionageDefenseModifier; }
	MissionTypes getMissionType() const { return (MissionTypes)m_iMissionType; }
	void setMissionType(MissionTypes eNewType);
	VoteSourceTypes getVoteSourceType() const { return (VoteSourceTypes)m_iVoteSourceType; }

	float getVisibilityPriority() const;

	bool isTeamShare() const { return m_bTeamShare; }
	bool isWater() const { return m_bWater; }
	bool isRiver() const { return m_bRiver; }
	bool isPower() const { return m_bPower; }
	bool isDirtyPower() const { return m_bDirtyPower; }
	bool isAreaCleanPower() const { return m_bAreaCleanPower; }
	bool isAreaBorderObstacle() const;
	bool isForceTeamVoteEligible() const { return m_bForceTeamVoteEligible; }
	bool isCapital() const { return m_bCapital; }
	bool isGovernmentCenter() const { return m_bGovernmentCenter; }
	bool isGoldenAge() const { return m_bGoldenAge; }
	bool isMapCentering() const { return m_bMapCentering; }
	bool isNoUnhappiness() const { return m_bNoUnhappiness; }
	//bool isNoUnhealthyPopulation() const;
	int getUnhealthyPopulationModifier() const // K-Mod, Exposed to Python
	{
		return m_iUnhealthyPopulationModifier;
	}
	bool isBuildingOnlyHealthy() const { return m_bBuildingOnlyHealthy; }
	bool isNeverCapture() const { return m_bNeverCapture; }
	bool isNukeImmune() const { return m_bNukeImmune; }
	bool isPrereqReligion() const { return m_bPrereqReligion; }
	bool isCenterInCity() const { return m_bCenterInCity; }
	bool isStateReligion() const { return m_bStateReligion; }
	bool isAllowsNukes() const { return m_bAllowsNukes; }

	const TCHAR* getConstructSound() const;
	void setConstructSound(const TCHAR* szVal);
	const TCHAR* getArtDefineTag() const;
	void setArtDefineTag(const TCHAR* szVal);
	const TCHAR* getMovieDefineTag() const;
	void setMovieDefineTag(const TCHAR* szVal);

	// Array access:
	friend void CyInfoPythonInterface1(); // advc.003x

	int getYieldChange(YieldTypes eYield) const;
	iPY_WRAP(YieldChange, Yield)
	int* getYieldChangeArray() const { return m_piYieldChange; }
	int getYieldModifier(YieldTypes eYield) const;
	iPY_WRAP(YieldModifier, Yield)
	int* getYieldModifierArray() const { return m_piYieldModifier; }
	int getPowerYieldModifier(YieldTypes eYield) const;
	iPY_WRAP(PowerYieldModifier, Yield)
	int* getPowerYieldModifierArray() const { return m_piPowerYieldModifier; }
	int getAreaYieldModifier(YieldTypes eYield) const; // (not exposed to Python)
	int* getAreaYieldModifierArray() const { return m_piAreaYieldModifier; }
	int getGlobalYieldModifier(YieldTypes eYield) const;
	iPY_WRAP(GlobalYieldModifier, Yield)
	int* getGlobalYieldModifierArray() const { return m_piGlobalYieldModifier; }
	int getSeaPlotYieldChange(YieldTypes eYield) const;
	iPY_WRAP(SeaPlotYieldChange, Yield)
	int* getSeaPlotYieldChangeArray() const { return m_piSeaPlotYieldChange; }
	int getRiverPlotYieldChange(YieldTypes eYield) const;
	iPY_WRAP(RiverPlotYieldChange, Yield)
	int* getRiverPlotYieldChangeArray() const { return m_piRiverPlotYieldChange; }
	int getGlobalSeaPlotYieldChange(YieldTypes eYield) const;
	iPY_WRAP(GlobalSeaPlotYieldChange, Yield)
	int* getGlobalSeaPlotYieldChangeArray() const { return m_piGlobalSeaPlotYieldChange; }

	int getCommerceChange(CommerceTypes eCommerce) const;
	iPY_WRAP(CommerceChange, Commerce)
	int* getCommerceChangeArray() const { return m_piCommerceChange; }
	int getObsoleteSafeCommerceChange(CommerceTypes eCommerce) const;
	iPY_WRAP(ObsoleteSafeCommerceChange, Commerce)
	int* getObsoleteSafeCommerceChangeArray() const { return m_piObsoleteSafeCommerceChange; }
	int getCommerceChangeDoubleTime(CommerceTypes eCommerce) const;
	iPY_WRAP(CommerceChangeDoubleTime, Commerce)
	int getCommerceModifier(CommerceTypes eCommerce) const;
	iPY_WRAP(CommerceModifier, Commerce)
	int* getCommerceModifierArray() const { return m_piCommerceModifier; }
	int getGlobalCommerceModifier(CommerceTypes eCommerce) const;
	iPY_WRAP(GlobalCommerceModifier, Commerce)
	int* getGlobalCommerceModifierArray() const { return m_piGlobalCommerceModifier; }
	int getSpecialistExtraCommerce(CommerceTypes eCommerce) const; // (not exposed to Python)
	int* getSpecialistExtraCommerceArray() const { return m_piSpecialistExtraCommerce; }
	int getStateReligionCommerce(CommerceTypes eCommerce) const;
	iPY_WRAP(StateReligionCommerce, Commerce)
	int* getStateReligionCommerceArray() const { return m_piStateReligionCommerce; }
	int getCommerceHappiness(CommerceTypes eCommerce) const;
	iPY_WRAP(CommerceHappiness, Commerce)

	int getReligionChange(int i) const;
	inline bool isAnyReligionChange() const { return (m_piReligionChange != NULL); } // advc.003t
	int getSpecialistCount(int i) const;
	int getFreeSpecialistCount(int i) const;
	int getBonusHealthChanges(int i) const;
	inline bool isAnyBonusHealthChanges() const { return (m_piBonusHealthChanges != NULL); } // advc.003t
	int getBonusHappinessChanges(int i) const;
	inline bool isAnyBonusHappinessChanges() const { return (m_piBonusHappinessChanges != NULL); } // advc.003t
	int getBonusProductionModifier(int i) const;
	inline bool isAnyBonusProductionModifier() const { return (m_piBonusProductionModifier != NULL); } // advc.003t
	int getUnitCombatFreeExperience(int i) const;
	int getDomainFreeExperience(int i) const;
	int getDomainProductionModifier(int i) const;
	TechTypes getPrereqAndTechs(int i) const;
	inline bool isAnyPrereqAndTech() const { return (m_piPrereqAndTechs != NULL); } // advc.003t
	BonusTypes getPrereqOrBonuses(int i) const;
	inline bool isAnyPrereqOrBonus() const { return (m_piPrereqOrBonuses != NULL); } // advc.003t
//	int getPrereqOrVicinityBonuses(int i) const;  //Shqype Vicinity Bonus Add
	int getProductionTraits(int i) const;
	int getHappinessTraits(int i) const;
	int getBuildingHappinessChanges(int i) const;
	inline bool isAnyBuildingHappinessChanges() const { return (m_piBuildingHappinessChanges != NULL); } // advc.003t
	int getPrereqNumOfBuildingClass(int i) const;
	inline bool isAnyPrereqNumOfBuildingClass() const { return (m_piPrereqNumOfBuildingClass != NULL); } // advc.003t
	int getFlavorValue(int i) const;
	int getImprovementFreeSpecialist(int i) const;
	inline bool isAnyImprovementFreeSpecialist() const { return (m_piImprovementFreeSpecialist != NULL); } // advc.003t

	bool isCommerceFlexible(int i) const;
	bool isCommerceChangeOriginalOwner(int i) const;
	bool isBuildingClassNeededInCity(int i) const;
	inline bool isAnyBuildingClassNeededInCity() const { return (m_pbBuildingClassNeededInCity != NULL); } // advc.003t

	int getSpecialistYieldChange(int i, int j) const;
	int* getSpecialistYieldChangeArray(int i) const;
	int getBonusYieldModifier(int i, int j) const;
	int* getBonusYieldModifierArray(int i) const;
	// davidlallen: building bonus yield, commerce start
	int getBonusConsumed() const;
	int getCommerceProduced(int i) const;
	int getYieldProduced(int i) const;
	// davidlallen: building bonus yield, commerce end
	// UNOFFICIAL_PATCH, Efficiency, 06/27/10, Afforess & jdog5000: START  // advc.003t: inlined
	inline bool isAnySpecialistYieldChange() const { return m_bAnySpecialistYieldChange; }
	inline bool isAnyBonusYieldModifier() const { return m_bAnyBonusYieldModifier; }
	// UNOFFICIAL_PATCH: END
	// <advc.003w> for convenience
	inline bool isWorldWonder() const
	{
		return GC.getInfo(getBuildingClassType()).isWorldWonder();
	}
	inline bool isTeamWonder() const
	{
		return GC.getInfo(getBuildingClassType()).isTeamWonder();
	}
	inline bool isNationalWonder() const
	{
		return GC.getInfo(getBuildingClassType()).isNationalWonder();
	}
	inline bool isLimited() const
	{
		return GC.getInfo(getBuildingClassType()).isLimited();
	} // </advc.003w>

	// Other

	const CvArtInfoBuilding* getArtInfo() const;
	const CvArtInfoMovie* getMovieInfo() const;
	const TCHAR* getButton() const;
	const TCHAR* getMovie() const;

	bool nameNeedsArticle() const; // advc.008e

	#if SERIALIZE_CVINFOS
	void read(FDataStreamBase*);
	void write(FDataStreamBase*);
	#endif
	bool read(CvXMLLoadUtility* pXML);
	// <advc.310>
	static void setDomesticGreatGeneralRateModifierEnabled(bool b);
	static void setAreaTradeRoutesEnabled(bool b);
	static void setAreaBorderObstacleEnabled(bool b);
	// </advc.310>

protected:
	int m_iBuildingClassType;
	int m_iVictoryPrereq;
	int m_iFreeStartEra;
	int m_iMaxStartEra;
	int m_iObsoleteTech;
	int m_iPrereqAndTech;
	int m_iNoBonus;
	int m_iPowerBonus;
	int m_iFreeBonus;
	int m_iNumFreeBonuses;		
	// < Building Resource Converter Start >
	int* m_paiRequiredInputBonuses;
	int* m_paiBuildingOutputBonuses;
	// < Building Resource Converter End   >
	int m_iFreeBuildingClass;
	int m_iFreePromotion;
	int m_iCivicOption;
	int m_iAIWeight;
	int m_iProductionCost;
	int m_iHurryCostModifier;
	int m_iHurryAngerModifier;
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
	int m_iMinAreaSize;
	int m_iNumCitiesPrereq;
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	int m_iNumCitySizeBldPrereq;
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	int m_iNumTeamsPrereq;
	int m_iUnitLevelPrereq;
	int m_iMinLatitude;
	int m_iMaxLatitude;
	int m_iGreatPeopleRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iDomesticGreatGeneralRateModifier;
	int m_iGlobalGreatPeopleRateModifier;
	int m_iAnarchyModifier;
	int m_iGoldenAgeModifier;
	int m_iGlobalHurryModifier;
	int m_iFreeExperience;
	int m_iGlobalFreeExperience;				
	/* Population Limit ModComp - Beginning */
	int m_iPopulationLimitChange;
	/* Population Limit ModComp - End */
	int m_iFoodKept;
	int m_iAirlift;
	int m_iAirModifier;
	int m_iAirUnitCapacity;
	int m_iNukeModifier;
	int m_iNukeExplosionRand;
	int m_iFreeSpecialist;
	int m_iAreaFreeSpecialist;
	int m_iGlobalFreeSpecialist;
	int m_iHappiness;
	int m_iAreaHappiness;
	int m_iGlobalHappiness;
	int m_iStateReligionHappiness;
	int m_iWorkerSpeedModifier;
	int m_iMilitaryProductionModifier;
	int m_iSpaceProductionModifier;
	int m_iGlobalSpaceProductionModifier;
	int m_iTradeRoutes;
	int m_iCoastalTradeRoutes;
	int m_iAreaTradeRoutes; // advc.310: was m_iGlobalTradeRoutes
	int m_iTradeRouteModifier;
	int m_iForeignTradeRouteModifier;
	int m_iAssetValue;
	int m_iPowerValue;
	int m_iSpecialBuildingType;
	int m_iAdvisorType;
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanConstruct															*/
/********************************************************************************/
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
	int m_iFoundsCorporation;
	int m_iGlobalReligionCommerce;
	int m_iGlobalCorporationCommerce;
	int m_iPrereqAndBonus;
//	int m_iPrereqVicinityBonus;  //Shqype Vicinity Bonus Add
	int m_iGreatPeopleUnitClass;
	int m_iGreatPeopleRateChange;
	int m_iConquestProbability;
	int m_iMaintenanceModifier;
	//DPII < Maintenance Modifier >
	int m_iGlobalMaintenanceModifier;
	int m_iAreaMaintenanceModifier;
	int m_iOtherAreaMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iCoastalDistanceMaintenanceModifier;
	int m_iConnectedCityMaintenanceModifier;
	//DPII < Maintenance Modifier >
	int m_iWarWearinessModifier;
	int m_iGlobalWarWearinessModifier;
	int m_iEnemyWarWearinessModifier;
	int m_iHealRateChange;
	int m_iHealth;
	int m_iAreaHealth;
	int m_iGlobalHealth;
	int m_iGlobalPopulationChange;
	int m_iFreeTechs;	
/*** HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave START ***/
	int m_iFreeSpecificTech;
/*** HISTORY IN THE MAKING COMPONENT: MOCTEZUMA'S SECRET TECHNOLOGY 5 October 2007 by Grave END ***/
	int m_iDefenseModifier;
	int m_iBombardDefenseModifier;
	int m_iAllCityDefenseModifier;
	int m_iEspionageDefenseModifier;
	int m_iMissionType;
	int m_iVoteSourceType;

	float m_fVisibilityPriority;

	bool m_bTeamShare;
	bool m_bWater;
	bool m_bRiver;
	bool m_bPower;
	bool m_bDirtyPower;
	bool m_bAreaCleanPower;
	bool m_bAreaBorderObstacle;
	bool m_bForceTeamVoteEligible;
	bool m_bCapital;
	bool m_bGovernmentCenter;
	bool m_bGoldenAge;
	bool m_bMapCentering;
	bool m_bNoUnhappiness;
	int m_iUnhealthyPopulationModifier; // K-Mod: was m_bNoUnhealthyPopulation
	bool m_bBuildingOnlyHealthy;
	bool m_bNeverCapture;
	bool m_bNukeImmune;
	bool m_bPrereqReligion;
	bool m_bCenterInCity;
	bool m_bStateReligion;
	bool m_bAllowsNukes;

	CvString m_szConstructSound;
	CvString m_szArtDefineTag;
	CvString m_szMovieDefineTag;

	int* m_piPrereqAndTechs;
	int* m_piPrereqOrBonuses;
//	int* m_piPrereqOrVicinityBonuses;  //Shqype Vicinity Bonus Add
	int* m_piProductionTraits;
	int* m_piHappinessTraits;
	int* m_piSeaPlotYieldChange;
	int* m_piRiverPlotYieldChange;
	int* m_piGlobalSeaPlotYieldChange;
	int* m_piYieldChange;
	int* m_piYieldModifier;
	int* m_piPowerYieldModifier;
	int* m_piAreaYieldModifier;
	int* m_piGlobalYieldModifier;
	int* m_piCommerceChange;
	int* m_piObsoleteSafeCommerceChange;
	int* m_piCommerceChangeDoubleTime;
	int* m_piCommerceModifier;
	int* m_piGlobalCommerceModifier;
	int* m_piSpecialistExtraCommerce;
	int* m_piStateReligionCommerce;
	int* m_piCommerceHappiness;
	int* m_piReligionChange;
	int* m_piSpecialistCount;
	int* m_piFreeSpecialistCount;
	int* m_piBonusHealthChanges;
	int* m_piBonusHappinessChanges;
	int* m_piBonusProductionModifier;
	int* m_piUnitCombatFreeExperience;
	int* m_piDomainFreeExperience;
	int* m_piDomainProductionModifier;
	int* m_piBuildingHappinessChanges;
	int* m_piPrereqNumOfBuildingClass;
	int* m_piFlavorValue;
	int* m_piImprovementFreeSpecialist;

	bool* m_pbCommerceFlexible;
	bool* m_pbCommerceChangeOriginalOwner;
	bool* m_pbBuildingClassNeededInCity;

	int** m_ppaiSpecialistYieldChange;
	int** m_ppaiBonusYieldModifier;
	// davidlallen: building bonus yield, commerce start
	int m_iBonusConsumed;
	int* m_paiCommerceProduced;
	int* m_paiYieldProduced;
	// davidlallen: building bonus yield, commerce end
	// UNOFFICIAL_PATCH, Efficiency, 06/27/10, Afforess & jdog5000: START
	bool m_bAnySpecialistYieldChange;
	bool m_bAnyBonusYieldModifier;
	// UNOFFICIAL_PATCH: END

	void addElements(std::vector<XMLElement*>& r) const; // advc.tag
	// <advc.310>
	static bool m_bEnabledAreaBorderObstacle;
	static bool m_bEnabledAreaTradeRoutes;
	static bool m_bEnabledDomesticGreatGeneralRateModifier;
	// </advc.310>
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvSpecialBuildingInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialBuildingInfo : public CvInfoBase
{
public: // All the const functions are exposed to Python
	CvSpecialBuildingInfo();
	virtual ~CvSpecialBuildingInfo();

	int getObsoleteTech() const;
	int getTechPrereq() const;
	int getTechPrereqAnyone() const;

	bool isValid() const;

	int getProductionTraits(int i) const;

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iObsoleteTech;
	int m_iTechPrereq;
	int m_iTechPrereqAnyone;

	bool m_bValid;

	int* m_piProductionTraits;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvVoteSourceInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVoteSourceInfo : public CvInfoBase
{
public: // The const functions are exposed to Python
	CvVoteSourceInfo();
	virtual ~CvVoteSourceInfo();

	int getVoteInterval() const;
	int getFreeSpecialist() const;
	int getCivic() const;
	const CvWString getPopupText() const; // (not exposed to python)
	const CvWString getSecretaryGeneralText() const;

	std::wstring pyGetSecretaryGeneralText() { return getSecretaryGeneralText(); }

	int getReligionYield(int i) const;
	int getReligionCommerce(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	bool readPass3();

protected:
	int m_iVoteInterval;
	int m_iFreeSpecialist;
	int m_iCivic;

	int* m_aiReligionYields;
	int* m_aiReligionCommerces;

	CvString m_szPopupText;
	CvString m_szSecretaryGeneralText;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvVoteInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVoteInfo :	public CvInfoBase
{
public: // All the const functions are exposed to Python
	CvVoteInfo();
	virtual ~CvVoteInfo();

	int getPopulationThreshold() const;
	int getStateReligionVotePercent() const;
	int getTradeRoutes() const;
	int getMinVoters() const;

	bool isSecretaryGeneral() const;
	bool isVictory() const;
	bool isFreeTrade() const;
	bool isNoNukes() const;
	bool isCityVoting() const;
	bool isCivVoting() const;
	bool isDefensivePact() const;
	bool isOpenBorders() const;
	bool isForcePeace() const;
	bool isForceNoTrade() const;
	bool isForceWar() const;
	bool isAssignCity() const;

	bool isForceCivic(int i) const;
	bool isVoteSourceType(int i) const;

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iPopulationThreshold;
	int m_iStateReligionVotePercent;
	int m_iTradeRoutes;
	int m_iMinVoters;

	bool m_bSecretaryGeneral;
	bool m_bVictory;
	bool m_bFreeTrade;
	bool m_bNoNukes;
	bool m_bCityVoting;
	bool m_bCivVoting;
	bool m_bDefensivePact;
	bool m_bOpenBorders;
	bool m_bForcePeace;
	bool m_bForceNoTrade;
	bool m_bForceWar;
	bool m_bAssignCity;

	bool* m_pbForceCivic;
	bool* m_abVoteSourceTypes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvProjectInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProjectInfo : public CvInfoBase
{
public: // All const functions are exposed to Python. advc.inl: Most of them inlined.
	CvProjectInfo();
	~CvProjectInfo();

	int getVictoryPrereq() const
	{
		return m_iVictoryPrereq;
	}
	int getTechPrereq() const
	{
		return m_iTechPrereq;
	}
	int getAnyoneProjectPrereq() const
	{
		return m_iAnyoneProjectPrereq;
	}
	inline int getMaxGlobalInstances() const
	{
		return m_iMaxGlobalInstances;
	}
	inline bool isWorldProject() const // advc.003w: Replacing global isWorldProject(ProjectTypes)
	{
		return (getMaxGlobalInstances() != -1);
	}
	inline int getMaxTeamInstances() const
	{
		return m_iMaxTeamInstances;
	}
	inline bool isTeamProject() const // advc.003w: Replacing global isTeamProject(ProjectTypes)
	{
		return (getMaxTeamInstances() != -1);
	}
	inline bool isLimited() const // advc.003w: Replacing global isLimitedProject(ProjectTypes)
	{
		return (isWorldProject() || isTeamProject());
	} 
	int getProductionCost() const
	{
		return m_iProductionCost;
	}
	int getNukeInterception() const
	{
		return m_iNukeInterception;
	}
	int getTechShare() const
	{
		return m_iTechShare;
	}
	//DPII < Maintenance Modifier had DllExport on all but no need according to  f1rpo>
    int getGlobalMaintenanceModifier() const;
    int getDistanceMaintenanceModifier() const;
    int getNumCitiesMaintenanceModifier() const;
    int getConnectedCityMaintenanceModifier() const;
    //DPII < Maintenance Modifier >
	int getEveryoneSpecialUnit() const
	{
		return m_iEveryoneSpecialUnit;
	}
	int getEveryoneSpecialBuilding() const
	{
		return m_iEveryoneSpecialBuilding;
	}
	int getVictoryDelayPercent() const
	{
		return m_iVictoryDelayPercent;
	}
	int getSuccessRate() const
	{
		return m_iSuccessRate;
	}
// davidlallen: project civilization and free unit start
	int getCivilization() const;
	int getFreeUnit() const;
// davidlallen: project civilization and free unit end

	bool isSpaceship() const
	{
		return m_bSpaceship;
	}
	bool isAllowsNukes() const
	{
		return m_bAllowsNukes;
	}

	const char* getMovieArtDef() const;
	const TCHAR* getCreateSound() const;
	void setCreateSound(const TCHAR* szVal);

	bool nameNeedsArticle() const;

	// Arrays access:

	int getBonusProductionModifier(int i) const;
	int getVictoryThreshold(int i) const;
	int getVictoryMinThreshold(int i) const;
	int getProjectsNeeded(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

protected:
	int m_iVictoryPrereq;
	int m_iTechPrereq;
	int m_iAnyoneProjectPrereq;
	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iProductionCost;
	int m_iNukeInterception;
	int m_iTechShare;
	//DPII < Maintenance Modifiers >
	int m_iGlobalMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iConnectedCityMaintenanceModifier;
	//DPII < Maintenance Modifiers >
	int m_iEveryoneSpecialUnit;
	int m_iEveryoneSpecialBuilding;
	int m_iVictoryDelayPercent;
	int m_iSuccessRate;
// davidlallen: project civilization and free unit start
	int m_iCivilization;
	int m_iFreeUnit;
// davidlallen: project civilization and free unit end

	bool m_bSpaceship;
	bool m_bAllowsNukes;

	CvString m_szCreateSound;
	CvString m_szMovieArtDef;

	int* m_piBonusProductionModifier;
	int* m_piVictoryThreshold;
	int* m_piVictoryMinThreshold;
	int* m_piProjectsNeeded;
};

#endif
