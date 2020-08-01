#pragma once

#ifndef CV_INFO_TERRAIN_H
#define CV_INFO_TERRAIN_H

/*  advc.003x: Cut from CvInfos.h. Info classes related to the natural terrain
	of individual non-city tiles and terrain improvements:
	CvTerrainInfo, CvFeatureInfo, CvBonusInfo, CvBonusClassInfo,
	CvRouteInfo, CvImprovementInfo, CvImprovementBonusInfo, CvGoodyInfo
	and CvBuildInfo (via include).
	(Infos related to the map options are handled by CvInfo_GameOption.h.
	Left in CvInfo_Misc.h: CvRouteModelInfo and CvRiverModelInfo - those are
	only used by the EXE.) */

#include "CvInfo_Build.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvTerrainInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTerrainInfo : public CvInfoBase
{
public: // All the const functions are exposed to Python except for those related to sound
		// advc.inl: Inlined most of the getters
	CvTerrainInfo();
	~CvTerrainInfo();

	inline int getMovementCost() const { return m_iMovementCost; }
	inline int getSeeFromLevel() const { return m_iSeeFromLevel; }
	inline int getSeeThroughLevel() const { return m_iSeeThroughLevel; }
	inline int getBuildModifier() const { return m_iBuildModifier; }
	inline int getDefenseModifier() const { return m_iDefenseModifier; }

/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: New Tag Definition                                                               **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
	int getHealthPercent() const;							// Exposed to Python
	int getTurnDamage() const;							// Exposed to Python
/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/

	inline bool isWater() const { return m_bWater; }
	inline bool isImpassable() const { return m_bImpassable; }
	inline bool isFound() const { return m_bFound; }
	inline bool isFoundCoast() const { return m_bFoundCoast; }
	inline bool isFoundFreshWater() const { return m_bFoundFreshWater; }

	/*****************************************************************************************************/
	/**  Author: TheLadiesOgre                                                                          **/
	/**  Date: 15.10.2009                                                                               **/
	/**  ModComp: TLOTags                                                                               **/
	/**  Reason Added: New Tag Definition                                                               **/
	/**  Notes:                                                                                         **/
	/*****************************************************************************************************/
	bool isRequiresFlatlands () const;							// Exposed to Python
	/*****************************************************************************************************/
	/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
	/*****************************************************************************************************/

	DllExport const TCHAR* getArtDefineTag() const;

	int getWorldSoundscapeScriptId() const;

	int getYield(int i) const;
	int getRiverYieldChange(int i) const;
	int getHillsYieldChange(int i) const;
	int get3DAudioScriptFootstepIndex(int i) const;

	const CvArtInfoTerrain* getArtInfo() const;
	const TCHAR* getButton() const;

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iMovementCost;
	int m_iSeeFromLevel;
	int m_iSeeThroughLevel;
	int m_iBuildModifier;
	int m_iDefenseModifier;

/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: New Tag Definition                                                               **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
	int m_iHealthPercent;
	int m_iTurnDamage;
/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/

	bool m_bWater;
	bool m_bImpassable;
	bool m_bFound;
	bool m_bFoundCoast;
	bool m_bFoundFreshWater;
	/*****************************************************************************************************/
	/**  Author: TheLadiesOgre                                                                          **/
	/**  Date: 15.10.2009                                                                               **/
	/**  ModComp: TLOTags                                                                               **/
	/**  Reason Added: New Tag Definition                                                               **/
	/**  Notes:                                                                                         **/
	/*****************************************************************************************************/
	bool m_bRequiresFlatlands;
	/*****************************************************************************************************/
	/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
	/*****************************************************************************************************/

	int m_iWorldSoundscapeScriptId;

	int* m_piYields;
	int* m_piRiverYieldChange;
	int* m_piHillsYieldChange;
	int* m_pi3DAudioScriptFootstepIndex;

private:
	CvString m_szArtDefineTag;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvFeatureInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvFeatureInfo : public CvInfoBase
{
public: /*  All the const functions are exposed to Python except for those dealing with art
			and those added by mods */
		// advc.inl: inlined some functions w/o giving it much thought
	CvFeatureInfo();
	~CvFeatureInfo();

	inline int getMovementCost() const { return m_iMovementCost; }
	inline int getSeeThroughChange() const { return m_iSeeThroughChange; }
	inline int getHealthPercent() const { return m_iHealthPercent; }
	int getAppearanceProbability() const;
	int getDisappearanceProbability() const;
	int getGrowthProbability() const;
	inline int getDefenseModifier() const { return m_iDefenseModifier; }
	// advc.012:
	inline int getRivalDefenseModifier() const { return m_iRivalDefenseModifier; }
	int getAdvancedStartRemoveCost() const;
	int getTurnDamage() const;
	int getWarmingDefense() const; //GWmod new xml field M.A. // Exposed to Python

	bool isNoCoast() const;
	bool isNoRiver() const;
	bool isNoRiverSide() const; // advc.129b
	bool isNoAdjacent() const;
	bool isRequiresFlatlands() const;
	bool isRequiresRiver() const;
	bool isRequiresRiverSide() const; // advc.129b
	bool isAddsFreshWater() const;
	inline bool isImpassable() const { return m_bImpassable; }
	bool isNoCity() const;
	inline bool isNoImprovement() const { return m_bNoImprovement; }
	bool isVisibleAlways() const;
	bool isNukeImmune() const;
	const TCHAR* getOnUnitChangeTo() const;

	const TCHAR* getArtDefineTag() const;

	int getWorldSoundscapeScriptId() const;

	const TCHAR* getEffectType() const;
	int getEffectProbability() const;

	inline int getYieldChange(int i) const
	{
		FAssertBounds(0, NUM_YIELD_TYPES, i);
		return m_piYieldChange[i]; // advc: Don't branch to check for NULL
	}
	inline int getRiverYieldChange(int i) const
	{
		FAssertBounds(0, NUM_YIELD_TYPES, i);
		return m_piRiverYieldChange[i]; // advc: see above
	}
	inline int getHillsYieldChange(int i) const
	{
		FAssertBounds(0, NUM_YIELD_TYPES, i);
		return m_piHillsYieldChange[i]; // advc: see above
	}
	int get3DAudioScriptFootstepIndex(int i) const;

	bool isTerrain(int i) const;
	int getNumVarieties() const;

	DllExport const CvArtInfoFeature* getArtInfo() const;
	const TCHAR* getButton() const;

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iMovementCost;
	int m_iSeeThroughChange;
	int m_iHealthPercent;
	int m_iAppearanceProbability;
	int m_iDisappearanceProbability;
	int m_iGrowthProbability;
	int m_iDefenseModifier;
	int m_iRivalDefenseModifier; // advc.012
	int m_iAdvancedStartRemoveCost;
	int m_iTurnDamage;
	int m_iWarmingDefense; //GWMod

	bool m_bNoCoast;
	bool m_bNoRiver;
	bool m_bNoRiverSide; // advc.129b
	bool m_bNoAdjacent;
	bool m_bRequiresFlatlands;
	bool m_bRequiresRiver;
	bool m_bRequiresRiverSide; // advc.129b
	bool m_bAddsFreshWater;
	bool m_bImpassable;
	bool m_bNoCity;
	bool m_bNoImprovement;
	bool m_bVisibleAlways;
	bool m_bNukeImmune;
	CvString m_szOnUnitChangeTo;

	int m_iWorldSoundscapeScriptId;

	CvString m_szEffectType;
	int m_iEffectProbability;

	int* m_piYieldChange;
	int* m_piRiverYieldChange;
	int* m_piHillsYieldChange;
	int* m_pi3DAudioScriptFootstepIndex;

	bool* m_pbTerrain;

private:
	CvString m_szArtDefineTag;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvBonusInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBonusInfo : public CvInfoBase
{
public: // All the const functions are exposed to Python
		// advc.inl: inlined a few getters w/o giving it much thought
	CvBonusInfo();
	virtual ~CvBonusInfo();

	BonusClassTypes getBonusClassType() const { return m_eBonusClassType; }
	int getChar() const;
	void setChar(int i);
	inline TechTypes getTechReveal() const { return m_eTechReveal; }
	inline TechTypes getTechCityTrade() const { return m_eTechCityTrade; }
	inline TechTypes getTechObsolete() const { return m_eTechObsolete; }
	TechTypes getTechImprove(bool bWater) const; // advc.003w
	int getAITradeModifier() const;
	int getAIObjective() const;
	inline int getHealth() const { return m_iHealth; }
	inline int getHappiness() const { return m_iHappiness; }
	int getMinAreaSize() const;
	int getMinLatitude() const;
	int getMaxLatitude() const;
	int getPlacementOrder() const;
	int getConstAppearance() const;
	int getRandAppearance1() const;
	int getRandAppearance2() const;
	int getRandAppearance3() const;
	int getRandAppearance4() const;
	int getPercentPerPlayer() const;
	int getTilesPer() const;
	int getMinLandPercent() const;
	int getUniqueRange() const;
	int getGroupRange() const;
	int getGroupRand() const;

	bool isOneArea() const;
	bool isHills() const;
//===NM=====Mountain Mod===0=====
/*	DllExport also had 	// Exposed to Python */ 
    bool isPeaks() const;							
//===NM=====Mountain Mod===X=====
	bool isFlatlands() const;
	bool isNoRiverSide() const;
	bool isNormalize() const;

	const TCHAR* getArtDefineTag() const;

	int getYieldChange(int i) const;
	int* getYieldChangeArray();
	// advc.003j: Unused in DLL and XML; wasn't even read from XML.
	//int getImprovementChange(int i) const;

	bool isTerrain(int i) const;
	bool isFeature(int i) const;
	bool isFeatureTerrain(int i) const;

	const TCHAR* getButton() const;
	DllExport const CvArtInfoBonus* getArtInfo() const;
	#if SERIALIZE_CVINFOS
	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	#endif
	bool read(CvXMLLoadUtility* pXML);
	void updateCache(BonusTypes eBonus); // advc.003w

protected:
	BonusClassTypes m_eBonusClassType;
	int m_iChar;
	TechTypes m_eTechReveal;
	TechTypes m_eTechCityTrade;
	TechTypes m_eTechObsolete;
	std::pair<TechTypes,TechTypes> m_eeTechImprove; // advc.003w
	int m_iAITradeModifier;
	int m_iAIObjective;
	int m_iHealth;
	int m_iHappiness;
	int m_iMinAreaSize;
	int m_iMinLatitude;
	int m_iMaxLatitude;
	int m_iPlacementOrder;
	int m_iConstAppearance;
	int m_iRandAppearance1;
	int m_iRandAppearance2;
	int m_iRandAppearance3;
	int m_iRandAppearance4;
	int m_iPercentPerPlayer;
	int m_iTilesPer;
	int m_iMinLandPercent;
	int m_iUniqueRange;
	int m_iGroupRange;
	int m_iGroupRand;

	bool m_bOneArea;
	bool m_bHills;
//===NM=====Mountain Mod===0=====
	bool m_bPeaks;
//===NM=====Mountain Mod===X=====
	bool m_bFlatlands;
	bool m_bNoRiverSide;
	bool m_bNormalize;

	CvString m_szArtDefineTag;

	int* m_piYieldChange;

	bool* m_pbTerrain;
	bool* m_pbFeature;
	bool* m_pbFeatureTerrain;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvBonusClassInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBonusClassInfo : public CvInfoBase
{
public:
	CvBonusClassInfo();

	int getUniqueRange() const; // Exposed to Python
	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iUniqueRange;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvRouteInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRouteInfo : public CvInfoBase
{
public: // All the const functions are exposed to Python except those added by mods
	CvRouteInfo();
	~CvRouteInfo();

	int getAdvancedStartCost() const;
	int getAdvancedStartCostIncrease() const;

	int getValue() const;
	// advc.inl: inlined these three
	inline int getMovementCost() const { return m_iMovementCost; }
	inline int getFlatMovementCost() const { return m_iFlatMovementCost; }
	inline int getPrereqBonus() const { return m_iPrereqBonus; }

	int getYieldChange(int i) const;
	int getTechMovementChange(int i) const;
	int getPrereqOrBonus(int i) const;
	inline bool isAnyPrereqOrBonus() const { return (m_piPrereqOrBonuses != NULL); } // advc.003t

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;

	int m_iValue;
	int m_iMovementCost;
	int m_iFlatMovementCost;
	int m_iPrereqBonus;

	int* m_piYieldChange;
	int* m_piTechMovementChange;
	int* m_piPrereqOrBonuses;
};

class CvImprovementBonusInfo;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvImprovementInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvImprovementInfo : /* advc.tag: */ public CvXMLInfo
{
public: /*  All the const functions are exposed to Python except those dealing with sound,
			Advanced Start and those added by mods */ // advc.inl: Inlined many of the getters
	CvImprovementInfo();
	~CvImprovementInfo();
	// <advc.tag>
	enum IntElementTypes
	{
		HealthPercent = CvXMLInfo::NUM_INT_ELEMENT_TYPES, // advc.901
		GWFeatureProtection, // advc.055
		NUM_INT_ELEMENT_TYPES
	};
	enum BoolElementTypes // unused so far
	{
		NUM_BOOL_ELEMENT_TYPES = CvXMLInfo::NUM_BOOL_ELEMENT_TYPES
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

	int getAdvancedStartCost() const;
	int getAdvancedStartCostIncrease() const;

	int getTilesPerGoody() const;
	int getGoodyUniqueRange() const;
	int getFeatureGrowthProbability() const;
	inline int getUpgradeTime() const { return m_iUpgradeTime; }
	inline int getAirBombDefense() const { return m_iAirBombDefense; }
	inline int getDefenseModifier() const { return m_iDefenseModifier; }
	inline int getHappiness() const { return m_iHappiness; }
	int getPillageGold() const;
	ImprovementTypes getImprovementPillage() const { return m_eImprovementPillage; }
	ImprovementTypes getImprovementUpgrade() const { return m_eImprovementUpgrade; }
	// < JCultureControl Mod Start >
	int getCultureBorderRange() const;				// Exposed to Python
	void setCultureBorderRange(int i);
	int getCultureControlStrength() const;				// Exposed to Python
	void setCultureControlStrength(int i);
	int getCultureControlCenterTileBonus() const;				// Exposed to Python
	void setCultureControlCenterTileBonus(int i);
	bool isSpreadCultureControl() const;				// Exposed to Python
	// < JCultureControl Mod End >
	
	// Deliverator
	int getAddsFreshWaterInRadius() const;				// Exposed to Python
	void setAddsFreshWaterInRadius(int i);
	// Deliverator
	inline bool isActsAsCity() const { return m_bActsAsCity; }
	inline bool isHillsMakesValid() const { return m_bHillsMakesValid; }
//===NM=====Mountain Mod===0=====
	inline bool isPeakMakesValid() const { return m_bPeakMakesValid; };
//===NM=====Mountain Mod===X=====
// davidlallen: mountain limitations next line
	inline bool isPeakMakesInvalid() const { return m_bPeakMakesInvalid; };
//===NM=====Mountain Mod===X=====
	inline bool isFreshWaterMakesValid() const { return m_bFreshWaterMakesValid; }
	inline bool isRiverSideMakesValid() const { return m_bRiverSideMakesValid; }
	inline bool isNoFreshWater() const { return m_bNoFreshWater; }
	inline bool isRequiresFlatlands() const { return m_bRequiresFlatlands; }
	DllExport inline bool isRequiresRiverSide() const { return m_bRequiresRiverSide; }
	inline bool isRequiresIrrigation() const { return m_bRequiresIrrigation; }
	inline bool isCarriesIrrigation() const { return m_bCarriesIrrigation; }
	inline bool isRequiresFeature() const { return m_bRequiresFeature; }
	inline bool isWater() const { return m_bWater; }
	DllExport inline bool isGoody() const { return m_bGoody; }
	inline bool isPermanent() const { return m_bPermanent; }
	inline bool isOutsideBorders() const { return m_bOutsideBorders; }

	const TCHAR* getArtDefineTag() const;

	int getWorldSoundscapeScriptId() const;
	// < JImprovementLimit Mod Start >
	bool isNotInsideBorders() const;				// Exposed to Python
    int getMakesInvalidRange() const;                // Exposed to Python
    int getImprovementRequired() const;                // Exposed to Python
    void setImprovementRequired(int iImprovementType);
    // < JImprovementLimit Mod End >
	// Array access:

	int getPrereqNatureYield(int i) const;
	int* getPrereqNatureYieldArray();
	int getYieldChange(int i) const;
	int* getYieldChangeArray();
	int getRiverSideYieldChange(int i) const;
	int* getRiverSideYieldChangeArray();
	int getHillsYieldChange(int i) const;
	int* getHillsYieldChangeArray();
	int getIrrigatedYieldChange(int i) const;
	int* getIrrigatedYieldChangeArray(); // For Moose - CvWidgetData XXX

	bool getTerrainMakesValid(int i) const;
	inline bool isAnyTerrainMakesValid() const { return (m_pbTerrainMakesValid != NULL); } // advc.003t
	bool getFeatureMakesValid(int i) const;
	inline bool isAnyFeatureMakesValid() const { return (m_pbFeatureMakesValid != NULL); } // advc.003t

	int getTechYieldChanges(int i, int j) const;
	int* getTechYieldChangesArray(int i) /* advc: */ const;
	int getRouteYieldChanges(int i, int j) const;
	// For Moose - CvWidgetData XXX
	int* getRouteYieldChangesArray(int i) /* advc: */ const;

	int getImprovementBonusYield(int iBonus, int iYield) const;
	bool isImprovementBonusMakesValid(int i) const;
	bool isImprovementBonusTrade(int i) const;
	int getImprovementBonusDiscoverRand(int i) const;

	/*	advc.003w: Moved from CvGameCoreUtils; still exposed to Python through CyGameCoreUtils.
		Renamed from "finalImprovementUpgrade".
		Can't turn it into a non-static function b/c a CvImprovementInfo object
		doesn't know its own ImprovementTypes id. */
	static ImprovementTypes finalUpgrade(ImprovementTypes eImprov);

	const TCHAR* getButton() const;
	DllExport const CvArtInfoImprovement* getArtInfo() const;
	#if SERIALIZE_CVINFOS
	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	#endif
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

protected:
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;

	int m_iTilesPerGoody;
	int m_iGoodyUniqueRange;
	int m_iFeatureGrowthProbability;
	int m_iUpgradeTime;
	int m_iAirBombDefense;
	int m_iDefenseModifier;
	int m_iHappiness;
	int m_iPillageGold;
	ImprovementTypes m_eImprovementPillage;
	ImprovementTypes m_eImprovementUpgrade;
	int m_iWorldSoundscapeScriptId;

	CvString m_szArtDefineTag;

	// < JCultureControl Mod Start >
	int m_iCultureBorderRange;
	int m_iCultureControlStrength;
	int m_iCultureControlCenterTileBonus;
	bool m_bSpreadCultureControl;
	// < JCultureControl Mod End >
	// Deliverator fresh water
	int m_iAddsFreshWaterInRadius;
	// Deliverator
	bool m_bActsAsCity;
	bool m_bHillsMakesValid;
//===NM=====Mountain Mod===0=====
	bool m_bPeakMakesValid;
//===NM=====Mountain Mod===X=====
// davidlallen: mountain limitations next line
	bool m_bPeakMakesInvalid;
//===NM=====Mountain Mod===X=====
	bool m_bFreshWaterMakesValid;
	bool m_bRiverSideMakesValid;
	bool m_bNoFreshWater;
	bool m_bRequiresFlatlands;
	bool m_bRequiresRiverSide;
	bool m_bRequiresIrrigation;
	bool m_bCarriesIrrigation;
	bool m_bRequiresFeature;
	bool m_bWater;
	bool m_bGoody;
	bool m_bPermanent;
	bool m_bOutsideBorders;

	// < JImprovementLimit Mod Start >
	bool m_bNotInsideBorders;
	int m_iMakesInvalidRange;
	int m_iImprovementRequired;
	// < JImprovementLimit Mod End >
	int* m_piPrereqNatureYield;
	int* m_piYieldChange;
	int* m_piRiverSideYieldChange;
	int* m_piHillsYieldChange;
	int* m_piIrrigatedChange;

	bool* m_pbTerrainMakesValid;
	bool* m_pbFeatureMakesValid;

	int** m_ppiTechYieldChanges;
	int** m_ppiRouteYieldChanges;

	CvImprovementBonusInfo* m_paImprovementBonus;

	void addElements(std::vector<XMLElement*>& r) const; // advc.tag
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvImprovementBonusInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvImprovementBonusInfo : public CvInfoBase
{
friend class CvImprovementInfo;
friend class CvXMLLoadUtility;
public: // The const functions are exposed to Python
	CvImprovementBonusInfo();
	~CvImprovementBonusInfo();

	int getDiscoverRand() const;
	bool isBonusMakesValid() const;
	bool isBonusTrade() const;
	int getYieldChange(int i) const;

	#if SERIALIZE_CVINFOS
	void read(FDataStreamBase* stream);
	void write(FDataStreamBase* stream);
	#endif
protected:
	int m_iDiscoverRand;
	bool m_bBonusMakesValid;
	bool m_bBonusTrade;
	int* m_piYieldChange;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvGoodyInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGoodyInfo : public CvInfoBase
{
public: // The const functions are exposed to Python
	CvGoodyInfo();

	int getGold() const;
	int getGoldRand1() const;
	int getGoldRand2() const;
	int getMapOffset() const;
	int getMapRange() const;
	int getMapProb() const;
	int getExperience() const;
	int getHealing() const;
	int getDamagePrereq() const;
	int getBarbarianUnitProb() const;
	int getMinBarbarians() const;
	int getUnitClassType() const;
	int getBarbarianUnitClass() const;

	bool isTech() const;
	bool isBad() const;

	const TCHAR* getSound() const;

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iGold;
	int m_iGoldRand1;
	int m_iGoldRand2;
	int m_iMapOffset;
	int m_iMapRange;
	int m_iMapProb;
	int m_iExperience;
	int m_iHealing;
	int m_iDamagePrereq;
	int m_iBarbarianUnitProb;
	int m_iMinBarbarians;
	int m_iUnitClassType;
	int m_iBarbarianUnitClass;

	bool m_bTech;
	bool m_bBad;

	CvString m_szSound;
};

#endif
