#pragma once

#ifndef CV_INFO_TECH_H
#define CV_INFO_TECH_H

/*  advc.003x: Cut from CvInfos.h. Just CvTechInfo b/c I want to precompile this one.
	advc.inl: Inlined most of the getters. */
class CvTechInfo : public CvInfoBase
{
friend class CvXMLLoadUtility;
public: // advc: All the const functions are exposed to Python except those added by mods
	CvTechInfo();
	~CvTechInfo();

	inline int getAdvisorType() const { return m_iAdvisorType; }
	inline int getAIWeight() const { return m_iAIWeight; }
	inline int getAITradeModifier() const { return m_iAITradeModifier; }
	inline int getResearchCost() const { return m_iResearchCost; }
	int getAdvancedStartCost() const;
	int getAdvancedStartCostIncrease() const;
	inline int getEra() const { return m_iEra; }
	inline int getTradeRoutes() const { return m_iTradeRoutes; }
	inline int getFeatureProductionModifier() const { return m_iFeatureProductionModifier; }
	inline int getWorkerSpeedModifier() const { return m_iWorkerSpeedModifier; }
	//DPII < Maintenance Modifier >
    int getMaintenanceModifier() const;
    int getDistanceMaintenanceModifier() const;
    int getNumCitiesMaintenanceModifier() const;
    int getCoastalDistanceMaintenanceModifier() const;
    //DPII < Maintenance Modifier >
	inline int getFirstFreeUnitClass() const { return m_iFirstFreeUnitClass; }
	inline int getHealth() const { return m_iHealth; }
	inline int getHappiness() const { return m_iHappiness; }
	inline int getFirstFreeTechs() const { return m_iFirstFreeTechs; }
	inline int getAssetValue() const { return m_iAssetValue; }
	inline int getPowerValue() const { return m_iPowerValue; }

	int getGridX() const;
	int getGridY() const;

	inline bool isRepeat() const { return m_bRepeat; }
	inline bool isTrade() const { return m_bTrade; }
	inline bool isDisable() const { return m_bDisable; }
	inline bool isGoodyTech() const { return m_bGoodyTech; }
	inline bool isExtraWaterSeeFrom() const { return m_bExtraWaterSeeFrom; }
	inline bool isMapCentering() const { return m_bMapCentering; }
	inline bool isMapVisible() const { return m_bMapVisible; }
	inline bool isMapTrading() const { return m_bMapTrading; }
	inline bool isTechTrading() const { return m_bTechTrading; }
	inline bool isGoldTrading() const { return m_bGoldTrading; }
	inline bool isOpenBordersTrading() const { return m_bOpenBordersTrading; }
	inline bool isDefensivePactTrading() const { return m_bDefensivePactTrading; }
	inline bool isPermanentAllianceTrading() const { return m_bPermanentAllianceTrading; }
	inline bool isVassalStateTrading() const { return m_bVassalStateTrading; }
	inline bool isBridgeBuilding() const { return m_bBridgeBuilding; }
	inline bool isIrrigation() const { return m_bIrrigation; }
	/* Population Limit ModComp - Beginning */
	inline bool isNoPopulationLimit() const { return m_bNoPopulationLimit; }
	//original - keldath	
	//bool isNoPopulationLimit() const;						// Exposed to Python
	/* Population Limit ModComp - End */
	inline bool isIgnoreIrrigation() const { return m_bIgnoreIrrigation; }
	inline bool isWaterWork() const { return m_bWaterWork; }
	inline bool isRiverTrade() const { return m_bRiverTrade; }

	std::wstring getQuote() const;
	void setQuoteKey(const TCHAR* szVal);
	void setSound(const TCHAR* szVal);
	const TCHAR* getSound() const;
	void setSoundMP(const TCHAR* szVal);
	const TCHAR* getSoundMP() const;

	// Array access:

	int getDomainExtraMoves(int i) const;
	// <Tech Bonus Mod- civic info>
    int getYieldModifier(int i) const;                // Exposed to Python
    int* getYieldModifierArray() const;
//kmod rewrite
    int getCommerceModifier(int i) const;				// Exposed to Python
 	int* getCommerceModifierArray() const;
	// <Tech Bonus Mod>
	int getFlavorValue(int i) const;
	int getPrereqOrTechs(int i) const;
	inline bool isAnyPrereqOrTech() const { return (m_piPrereqOrTechs != NULL); } // advc.003t
	int getPrereqAndTechs(int i) const;
	inline bool isAnyPrereqAndTech() const { return (m_piPrereqAndTechs != NULL); } // advc.003t
	// K-Mod, exposed to Python
	int getSpecialistExtraCommerce(int i) const;
	int* getSpecialistExtraCommerceArray() const;
	// K-Mod end
	bool isCommerceFlexible(int i) const;
	bool isTerrainTrade(int i) const;
	inline bool isAnyTerrainTrade() const { return (m_pbTerrainTrade != NULL); } // advc.003t

	#if SERIALIZE_CVINFOS
	void read(FDataStreamBase* );
	void write(FDataStreamBase* );
	#endif
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

protected:
	int m_iAdvisorType;
	int m_iAIWeight;
	int m_iAITradeModifier;
	int m_iResearchCost;
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
	int m_iEra;
	int m_iTradeRoutes;
	int m_iFeatureProductionModifier;
	int m_iWorkerSpeedModifier;
	//DPII < Maintenance Modifier >
	int m_iMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iCoastalDistanceMaintenanceModifier;
	//DPII < Maintenance Modifier >
	int m_iFirstFreeUnitClass;
	int m_iHealth;
	int m_iHappiness;
	int m_iFirstFreeTechs;
	int m_iAssetValue;
	int m_iPowerValue;

	int m_iGridX;
	int m_iGridY;

	bool m_bRepeat;
	bool m_bTrade;
	bool m_bDisable;
	bool m_bGoodyTech;
	bool m_bExtraWaterSeeFrom;
	bool m_bMapCentering;
	bool m_bMapVisible;
	bool m_bMapTrading;
	bool m_bTechTrading;
	bool m_bGoldTrading;
	bool m_bOpenBordersTrading;
	bool m_bDefensivePactTrading;
	bool m_bPermanentAllianceTrading;
	bool m_bVassalStateTrading;
	bool m_bBridgeBuilding;
	bool m_bIrrigation;
	/* Population Limit ModComp - Beginning */
	bool m_bNoPopulationLimit;
	/* Population Limit ModComp - End */
	bool m_bIgnoreIrrigation;
	bool m_bWaterWork;
	bool m_bRiverTrade;

	CvString m_szQuoteKey;
	CvString m_szSound;
	CvString m_szSoundMP;

	int* m_piDomainExtraMoves;
	// <Tech Bonus Mod Start>
	int* m_piYieldModifier;
	int* m_piCommerceModifier;
	// <Tech Bonus Mod End>
	int* m_piFlavorValue;

	int* m_piPrereqOrTechs;
	int* m_piPrereqAndTechs;

	int* m_piSpecialistExtraCommerce; // K-Mod
	bool* m_pbCommerceFlexible;
	bool* m_pbTerrainTrade;
};

#endif
