#pragma once

// team.h

#ifndef CIV4_TEAM_H
#define CIV4_TEAM_H

class CvArea;

class CvTeam /* advc.003e: */ : private boost::noncopyable
{
public:
	// <advc.003u>
	static inline CvTeam& getTeam(TeamTypes eTeam)
	{
		FAssertBounds(0, MAX_TEAMS, eTeam);
		// Needs to be inline and I don't want to include CvTeamAI.h here
		return *reinterpret_cast<CvTeam*>(m_aTeams[eTeam]);
	}
	// static functions moved from CvTeamAI
	static void initStatics();
	static void freeStatics(); // </advc.003u>

	// <kekm.26>
	static void queueWar(TeamTypes eAttackingTeam, TeamTypes eDefendingTeam,
			bool bNewDiplo, WarPlanTypes eWarPlan, bool bPrimaryDOW = true);
	static void triggerWars(/* advc: */ bool bForceUpdateAttitude = false);
	// </kekm.26>

	explicit CvTeam(TeamTypes eID);
	virtual ~CvTeam();

	DllExport void init(TeamTypes eID);
	DllExport void reset(TeamTypes eID = NO_TEAM, bool bConstructorCall = false);

	void resetPlotAndCityData(); // BETTER_BTS_AI_MOD, 12/30/08, jdog5000
	void addTeam(TeamTypes eTeam);																								// Exposed to Python
	void shareItems(TeamTypes eTeam);
	void shareCounters(TeamTypes eTeam);
	void processBuilding(BuildingTypes eBuilding, int iChange);

	void doTurn();

	void updateYield();
	void updatePowerHealth();
	void updateCommerce();

	bool canChangeWarPeace(TeamTypes eTeam, bool bAllowVassal = false) const;																			// Exposed to Python
	DllExport bool canDeclareWar(TeamTypes eTeam) const;																// Exposed to Python
	bool canEventuallyDeclareWar(TeamTypes eTeam) const; // bbai, Exposed to Python
	void declareWar(TeamTypes eTeam, bool bNewDiplo, WarPlanTypes eWarPlan,
			bool bPrimaryDoW = true, // K-Mod added bPrimaryDoW, Exposed to Python
			PlayerTypes eSponsor = NO_PLAYER, // advc.100
			bool bRandomEvent = false); // advc.106g
	void makePeace(TeamTypes eTarget, bool bBumpUnits = true,																		// Exposed to Python
			TeamTypes eBroker = NO_TEAM, // advc.100b
			bool bCapitulate = false, // advc.034
			CLinkList<TradeData> const* pReparations = NULL, // advc.039
			bool bRandomEvent = false); // advc.106g
	bool canContact(TeamTypes eTeam,
			bool bCheckWillingness = false) const; // K-Mod, Exposed to Python
	void meet(TeamTypes eTeam, bool bNewDiplo,																			// Exposed to Python
			FirstContactData* pData = NULL); // advc.071
	void signPeaceTreaty(TeamTypes eTeam, bool bForce = false); // K-Mod (advc: bForce)
	void signOpenBorders(TeamTypes eTeam);																				// Exposed to Python
	void signDisengage(TeamTypes otherId); // advc.034
	void signDefensivePact(TeamTypes eTeam);																			// Exposed to Python
	bool canSignDefensivePact(TeamTypes eTeam) /* advc: */ const;

	int getAssets() const;																															// Exposed to Python
	int getPower(bool bIncludeVassals) const;																																// Exposed to Python
	int getDefensivePower(TeamTypes eExcludeTeam = NO_TEAM) const;						// Exposed to Python
	int getEnemyPower() const;
	int getNumNukeUnits() const;																												// Exposed to Python
	int getVotes(VoteTypes eVote, VoteSourceTypes eVoteSource) const;
	bool isVotingMember(VoteSourceTypes eVoteSource) const;
	bool isFullMember(VoteSourceTypes eVoteSource) const;

	/*  advc: Added a default value for bIgnoreMinors to all WarPlan functions.
		getAtWarCount renamed to getNumWars to avoid confusion with CvTeamAI::AI_getAtWarCounter. */
	// BETTER_BTS_AI_MOD, 01/10/09, jdog5000: bIgnoreVassals added
	int getNumWars(bool bIgnoreMinors = true, bool bIgnoreVassals = false) const;																				// Exposed to Python
	// advc: Replaced by CvTeamAI::AI_countWarPlans
	//int getWarPlanCount(WarPlanTypes eWarPlan, bool bIgnoreMinors = true) const;
	int getHasMetCivCount(bool bIgnoreMinors = true) const;																		// Exposed to Python

	bool allWarsShared(TeamTypes eOther, // kekm.3
			/*  advc.130f: If false, check only if the war enemies of this team
				are included in those of otherId (set inclusion). */
			bool bCheckBothWays = true) const;
	bool hasMetHuman() const;																														// Exposed to Python
	bool isInContactWithBarbarians() const; // advc.302
	int getDefensivePactCount(TeamTypes eObs = NO_TEAM) const;																									// Exposed to Python
	int getVassalCount(TeamTypes eObs = NO_TEAM) const;
	// advc.opt, advc.inl:
	inline bool isAVassal() const { return (m_eMaster != NO_TEAM); }																						// Exposed to Python
	bool canVassalRevolt(TeamTypes eMaster) const;
	bool isLossesAllowRevolt(TeamTypes eMaster) const; // advc.112
	int getUnitClassMaking(UnitClassTypes eUnitClass) const;														// Exposed to Python
	int getUnitClassCountPlusMaking(UnitClassTypes eIndex) const;												// Exposed to Python
	int getBuildingClassMaking(BuildingClassTypes eBuildingClass) const;								// Exposed to Python
	int getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const;								// Exposed to Python
	int getHasReligionCount(ReligionTypes eReligion) const;															// Exposed to Python
	int getHasCorporationCount(CorporationTypes eCorporation) const;															// Exposed to Python

	int countTotalCulture() const;																			// Exposed to Python

	int countNumUnitsByArea(CvArea const& kArea) const;																				// Exposed to Python
	int countNumCitiesByArea(CvArea const& kArea) const;																			// Exposed to Python
	int countTotalPopulationByArea(CvArea const& kArea) const;																// Exposed to Python
	int countPowerByArea(CvArea const& kArea) const;																					// Exposed to Python
	int countNumAIUnitsByArea(CvArea const& kArea, UnitAITypes eUnitAI) const;								// Exposed to Python
	int countEnemyDangerByArea(CvArea const& kArea, TeamTypes eEnemyTeam = NO_TEAM) const;																		// Exposed to Python
	EraTypes getCurrentEra() const; // advc.112b
	// K-Mod:
	int getTypicalUnitValue(UnitAITypes eUnitAI, DomainTypes eDomain = NO_DOMAIN) const;

	int getResearchCost(TechTypes eTech, bool bGlobalModifiers = true, bool bTeamSizeModifiers = true) const; // (K-Mod added bools) Exposed to Python
	int getResearchLeft(TechTypes eTech) const;																// Exposed to Python

	bool hasHolyCity(ReligionTypes eReligion) const;																		// Exposed to Python
	bool hasHeadquarters(CorporationTypes eCorporation) const;																		// Exposed to Python
	bool hasBonus(BonusTypes eBonus) const;
	bool isBonusObsolete(BonusTypes eBonus) const;

	bool isHuman() const;																																// Exposed to Python
	// advc: (The Barbarians aren't a proper civ)
	inline bool isMajorCiv() const { return (!isBarbarian() && !isMinorCiv()); }
	// advc.inl
	inline bool isBarbarian() const { return (m_eID == BARBARIAN_TEAM); }																				// Exposed to Python
	// <advc.003m> cached
	inline bool isMinorCiv() const { return m_bMinorTeam; }																							// Exposed to Python
	void updateMinorCiv() { m_bMinorTeam = checkMinorCiv(); }
	// </advc.003m>  <advc.opt> This gets called a lot. Now precomputed.
	PlayerTypes getLeaderID() const { return m_eLeader; }																					// Exposed to Python
	void updateLeaderID(); // </advc.opt>
	PlayerTypes getSecretaryID() const;																									// Exposed to Python
	HandicapTypes getHandicapType() const;																							// Exposed to Python
	CvWString getName() const;																								// Exposed to Python
	CvWString getReplayName() const; // K-Mod

	DllExport inline int getNumMembers() const { return m_iNumMembers; } // advc.inl																	// Exposed to Python
	void changeNumMembers(int iChange);

	// advc.inl: In-line definitions for most of the get..Count and is... functions below
	int getAliveCount() const { return m_iAliveCount; } // advc.155: Exposed to Python
	inline int isAlive() const { return (m_iAliveCount > 0); }																// Exposed to Python
	void changeAliveCount(int iChange);

	inline int getEverAliveCount() const { return m_iEverAliveCount; }			
	inline bool isEverAlive() const { return (getEverAliveCount() > 0); } // advc: return type was int												// Exposed to Python
	void changeEverAliveCount(int iChange);

	inline int getNumCities() const { return m_iNumCities; }																							// Exposed to Python
	void changeNumCities(int iChange);

	int getTotalPopulation(bool bCheckVassals = true) const;																											// Exposed to Python
	void changeTotalPopulation(int iChange);

	int getTotalLand(bool bCheckVassals = true) const;																														// Exposed to Python
	void changeTotalLand(int iChange);

	int getNukeInterception() const;																										// Exposed to Python
	void changeNukeInterception(int iChange);																			// Exposed to Python

	int getForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource) const;																				// Exposed to Python
	bool isForceTeamVoteEligible(VoteSourceTypes eVoteSource) const;																								// Exposed to Python
	void changeForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource, int iChange);												// Exposed to Python

	int getExtraWaterSeeFromCount() const { return m_iExtraWaterSeeFromCount; }																		// Exposed to Python
	bool isExtraWaterSeeFrom() const { return (getExtraWaterSeeFromCount() > 0); }																						// Exposed to Python
	void changeExtraWaterSeeFromCount(int iChange);																// Exposed to Python

	int getMapTradingCount() const { return m_iMapTradingCount; }																					// Exposed to Python
	bool isMapTrading() const { return (getMapTradingCount() > 0); }																					// Exposed to Python
	void changeMapTradingCount(int iChange);																			// Exposed to Python

	int getTechTradingCount() const { return m_iTechTradingCount; }																	// Exposed to Python
	bool isTechTrading() const { return (getTechTradingCount() > 0); }																	// Exposed to Python
	void changeTechTradingCount(int iChange);																			// Exposed to Python

	int getGoldTradingCount() const { return m_iGoldTradingCount; }																// Exposed to Python
	bool isGoldTrading() const { return (getGoldTradingCount() > 0); }																					// Exposed to Python
	void changeGoldTradingCount(int iChange);																			// Exposed to Python

	int getOpenBordersTradingCount() const { return m_iOpenBordersTradingCount; }															// Exposed to Python
	bool isOpenBordersTrading() const { return (getOpenBordersTradingCount() > 0); }										// Exposed to Python
	void changeOpenBordersTradingCount(int iChange);															// Exposed to Python

	int getDefensivePactTradingCount() const { return m_iDefensivePactTradingCount; }										// Exposed to Python
	bool isDefensivePactTrading() const { return (getDefensivePactTradingCount() > 0); }															// Exposed to Python
	void changeDefensivePactTradingCount(int iChange);														// Exposed to Python

	int getPermanentAllianceTradingCount() const { return m_iPermanentAllianceTradingCount; }										// Exposed to Python
	bool isPermanentAllianceTrading() const;																						// Exposed to Python
	void changePermanentAllianceTradingCount(int iChange);												// Exposed to Python

	int getVassalTradingCount() const { return m_iVassalTradingCount; }														// Exposed to Python
	bool isVassalStateTrading() const;																						// Exposed to Python
	void changeVassalTradingCount(int iChange);												// Exposed to Python

	int getBridgeBuildingCount() const { return m_iBridgeBuildingCount; }																	// Exposed to Python
	bool isBridgeBuilding() const { return (getBridgeBuildingCount() > 0); }												// Exposed to Python
	void changeBridgeBuildingCount(int iChange);																			// Exposed to Python

	int getIrrigationCount() const { return m_iIrrigationCount; }																			// Exposed to Python
	bool isIrrigation() const { return (getIrrigationCount() > 0); }														// Exposed to Python
	void changeIrrigationCount(int iChange);																			// Exposed to Python

/* Population Limit ModComp - Beginning */
	int getNoPopulationLimitCount() const;																											// Exposed to Python
	bool isNoPopulationLimit() const;																								// Exposed to Python
	void changeNoPopulationLimitCount(int iChange);	// Exposed to Python
/* Population Limit ModComp - End */

	int getIgnoreIrrigationCount() const { return m_iIgnoreIrrigationCount; }																// Exposed to Python
	bool isIgnoreIrrigation() const { return (getIgnoreIrrigationCount() > 0); }														// Exposed to Python
	void changeIgnoreIrrigationCount(int iChange);																// Exposed to Python

	int getWaterWorkCount() const { return m_iWaterWorkCount; }																	// Exposed to Python
	bool isWaterWork() const { return (getWaterWorkCount() > 0); }																		// Exposed to Python
	void changeWaterWorkCount(int iChange);																				// Exposed to Python

	int getVassalPower() const { return m_iVassalPower; }																// Exposed to Python
	void setVassalPower(int iPower) { m_iVassalPower = iPower; }														// Exposed to Python
	int getMasterPower() const { return m_iMasterPower; }																// Exposed to Python
	void setMasterPower(int iPower) { m_iMasterPower = iPower; }														// Exposed to Python

	int getEnemyWarWearinessModifier() const { return m_iEnemyWarWearinessModifier; }																								// Exposed to Python
	void changeEnemyWarWearinessModifier(int iChange);																	// Exposed to Python
	void changeWarWeariness(TeamTypes eOtherTeam, const CvPlot& kPlot, int iFactor);

	bool isMapCentering() const { return m_bMapCentering; }																	// Exposed to Python
	void setMapCentering(bool bNewValue);																					// Exposed to Python

	TeamTypes getID() const { return m_eID; } // advc.inl																// Exposed to Python

	int getStolenVisibilityTimer(TeamTypes eIndex) const { return m_aiStolenVisibilityTimer.get(eIndex); }
	bool isStolenVisibility(TeamTypes eIndex) const { return (getStolenVisibilityTimer(eIndex) > 0); }																		// Exposed to Python
	void setStolenVisibilityTimer(TeamTypes eIndex, int iNewValue);
	void changeStolenVisibilityTimer(TeamTypes eIndex, int iChange);

	int getWarWeariness(TeamTypes eIndex, bool bUseEnemyModifer = false) const;			// Exposed to Python. K-Mod added bUseEnemyModifier.
	void setWarWeariness(TeamTypes eIndex, int iNewValue);												// Exposed to Python
	void changeWarWeariness(TeamTypes eIndex, int iChange);												// Exposed to Python
	// advc.enum: Params changed to TeamTypes (x3)
	int getTechShareCount(TeamTypes eIndex) const;																						// Exposed to Python
	bool isTechShare(TeamTypes eIndex) const;																									// Exposed to Python
	bool isAnyTechShare() const { return m_aiTechShareCount.hasContent(); } // advc.opt
	void changeTechShareCount(TeamTypes eIndex, int iChange);														// Exposed to Python

	int getCommerceFlexibleCount(CommerceTypes eIndex) const;														// Exposed to Python
	bool isCommerceFlexible(CommerceTypes eIndex) const;																// Exposed to Python
	void changeCommerceFlexibleCount(CommerceTypes eIndex, int iChange);					// Exposed to Python
    // < Civic Infos Plus Start >
    int getYieldRateModifier(YieldTypes eIndex);																								// Exposed to Python
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);

	int getCommerceRateModifier(CommerceTypes eIndex);																								// Exposed to Python
	void changeCommerceRateModifier(CommerceTypes eIndex, int iChange);
	// < Civic Infos Plus End   >
	int getExtraMoves(DomainTypes eIndex) const;																				// Exposed to Python
	void changeExtraMoves(DomainTypes eIndex, int iChange);								// Exposed to Python

	inline bool isHasMet(TeamTypes eIndex) const // advc.inl													// Exposed to Python
	{
		return m_abHasMet.get(eIndex);
	}
	void makeHasMet(TeamTypes eIndex, bool bNewDiplo,
			FirstContactData* pData = NULL); // advc.071
	bool isHasSeen(TeamTypes eIndex) const { return m_abHasSeen.get(eIndex); }; // K-Mod
	void makeHasSeen(TeamTypes eIndex) { m_abHasSeen.set(eIndex, true); }; // K-Mod
	// <advc.134a>
	bool isAtWarExternal(TeamTypes eIndex) const; // Exported through .def file
	inline bool isAtWar(TeamTypes eIndex) const																	// Exposed to Python
	{
		return m_abAtWar.get(eIndex);
	} // </advc.134a>
	void setAtWar(TeamTypes eIndex, bool bNewValue);
	bool hasJustDeclaredWar(TeamTypes eIndex) const; // advc.162

	bool isPermanentWarPeace(TeamTypes eIndex) const { return m_abPermanentWarPeace.get(eIndex); } // advc.inl									// Exposed to Python
	void setPermanentWarPeace(TeamTypes eIndex, bool bNewValue);									// Exposed to Python

	bool canTradeWith(TeamTypes eWhoTo) const; // advc
	bool isFreeTrade(TeamTypes eIndex) const;																	// Exposed to Python
	inline bool isOpenBorders(TeamTypes eIndex) const																// Exposed to Python
	{
		return m_abOpenBorders.get(eIndex); // advc.inl
	}
	void setOpenBorders(TeamTypes eIndex, bool bNewValue);
	// <advc.034>
	inline bool isDisengage(TeamTypes eIndex) const { return m_abDisengage.get(eIndex);}
	void setDisengage(TeamTypes eIndex, bool bNewValue);
	void cancelDisengage(TeamTypes otherId);
	// </advc.034>
	// advc.inl
	inline bool isDefensivePact(TeamTypes eIndex) const { return m_abDefensivePact.get(eIndex); }															// Exposed to Python
	void setDefensivePact(TeamTypes eIndex, bool bNewValue);

	bool isForcePeace(TeamTypes eIndex) const { return m_abForcePeace.get(eIndex); } // advc.inl										// Exposed to Python
	void setForcePeace(TeamTypes eIndex, bool bNewValue);
	int turnsOfForcedPeaceRemaining(TeamTypes eOther) const; // advc.104

	bool isVassal(TeamTypes eMaster) const																// Exposed to Python
	{
		return (m_eMaster == eMaster); // advc.opt, advc.inl
	}
	void setVassal(TeamTypes eMaster, bool bNewValue, bool bCapitulated);
	TeamTypes getMasterTeam() const // advc.155: Exposed to Python
	{	// K-Mod. Return the team which is the master of this team. (if this team is free, return getID())
		return (m_eMaster == NO_TEAM ? getID() : m_eMaster); // advc.opt
	}
	void assignVassal(TeamTypes eVassal, bool bSurrender) const;																// Exposed to Python
	void freeVassal(TeamTypes eVassal) const;																// Exposed to Python

	inline bool isCapitulated() const // advc.130v: Exposed to Python
	{	// advc.inl: inline, disable K-Mod assertion
		//FAssert(!m_bCapitulated || isAVassal()); // K-Mod
		return m_bCapitulated;
	}  // <advc>
	bool isCapitulated(TeamTypes eMaster)
	{
		return (isCapitulated() && isVassal(eMaster));
	} // </advc>
	int getRouteChange(RouteTypes eIndex) const																				// Exposed to Python
	{
		return m_aiRouteChange.get(eIndex); // advc.inl
	}
	void changeRouteChange(RouteTypes eIndex, int iChange);												// Exposed to Python

	int getProjectCount(ProjectTypes eIndex) const														// Exposed to Python
	{
		return m_aiProjectCount.get(eIndex); // advc.inl
	}
	DllExport int getProjectDefaultArtType(ProjectTypes eIndex) const;
	DllExport void setProjectDefaultArtType(ProjectTypes eIndex, int iValue);
	DllExport int getProjectArtType(ProjectTypes eIndex, int number) const;
	DllExport void setProjectArtType(ProjectTypes eIndex, int number, int value);
	bool isProjectMaxedOut(ProjectTypes eIndex, int iExtra = 0) const;									// Exposed to Python
	DllExport bool isProjectAndArtMaxedOut(ProjectTypes eIndex) const;
	void changeProjectCount(ProjectTypes eIndex, int iChange);							// Exposed to Python
	DllExport void finalizeProjectArtTypes();

	int getProjectMaking(ProjectTypes eIndex) const																		// Exposed to Python
	{
		return m_aiProjectMaking.get(eIndex); // advc.inl
	}
	void changeProjectMaking(ProjectTypes eIndex, int iChange);

	int getUnitClassCount(UnitClassTypes eIndex) const																	// Exposed to Python
	{
		return m_aiUnitClassCount.get(eIndex); // advc.inl
	}
	bool isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra = 0) const;							// Exposed to Python
	void changeUnitClassCount(UnitClassTypes eIndex, int iChange);

	int getBuildingClassCount(BuildingClassTypes eIndex) const													// Exposed to Python
	{
		return m_aiBuildingClassCount.get(eIndex); // advc.inl
	}
	bool isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra = 0) const;			// Exposed to Python
	void changeBuildingClassCount(BuildingClassTypes eIndex, int iChange);

	int getObsoleteBuildingCount(BuildingTypes eIndex) const
	{
		return m_aiObsoleteBuildingCount.get(eIndex); // advc.inl
	}
	bool isObsoleteBuilding(BuildingTypes eIndex) const																// Exposed to Python
	{
		return (getObsoleteBuildingCount(eIndex) > 0); // advc.inl
	}
	void changeObsoleteBuildingCount(BuildingTypes eIndex, int iChange);

	int getResearchProgress(TechTypes eIndex) const;																						// Exposed to Python
	void setResearchProgress(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer);									// Exposed to Python
	void changeResearchProgress(TechTypes eIndex, int iChange, PlayerTypes ePlayer);								// Exposed to Python
	int changeResearchProgressPercent(TechTypes eIndex, int iPercent, PlayerTypes ePlayer);

	int getTechCount(TechTypes eIndex) const;																										// Exposed to Python
	// BETTER_BTS_AI_MOD, General AI, 07/27/09, jdog5000:
	int getBestKnownTechScorePercent() const;

	// advc.inl: inline (x2)
	inline int getTerrainTradeCount(TerrainTypes eIndex) const
	{
		return m_aiTerrainTradeCount.get(eIndex);
	}
	inline bool isTerrainTrade(TerrainTypes eIndex) const																												// Exposed to Python
	{
		return (getTerrainTradeCount(eIndex) > 0);
	}
	void changeTerrainTradeCount(TerrainTypes eIndex, int iChange);

	int getRiverTradeCount() const;
	inline bool isRiverTrade() const																												// Exposed to Python
	{
		//return (getRiverTradeCount() > 0);
		return true; // advc.124 (and inline)
	}
	void changeRiverTradeCount(int iChange);

	int getVictoryCountdown(VictoryTypes eIndex) const;																							// Exposed to Python
	void setVictoryCountdown(VictoryTypes eIndex, int iTurnsLeft);
	void changeVictoryCountdown(VictoryTypes eIndex, int iChange);
	bool isAnyVictoryCountdown() const; // advc.opt
	int getVictoryDelay(VictoryTypes eVictory) const;
	bool canLaunch(VictoryTypes eVictory) const;									// Exposed to Python
	void setCanLaunch(VictoryTypes eVictory, bool bCan);
	int getLaunchSuccessRate(VictoryTypes eVictory) const;								// Exposed to Python
	void resetVictoryProgress();
	bool hasSpaceshipArrived() const; // K-Mod, Exposed to Python

	bool isParent(TeamTypes eChildTeam) const;

	bool isHasTech(TechTypes eIndex) const;																																			// Exposed to Python
	void setHasTech(TechTypes eTech, bool bNewValue, PlayerTypes ePlayer,				// Exposed to Python
			bool bFirst, bool bAnnounce, /* advc.121: */ bool bEndOfTurn = false);
	/* advc.004a: A hack that allows other classes to pretend that a team knows
	   a tech for some computation. Should be toggled back afterwards. */
	inline void setHasTechTemporarily(TechTypes eTech, bool b) { m_abHasTech.set(eTech, b); }
	int getTechCount() const { return m_iTechCount; } // advc.101
	// <advc.134a>
	void advancePeaceOfferStage(TeamTypes eAITeam = NO_TEAM);
	bool isPeaceOfferStage(int iStage, TeamTypes eOffering) const;
	// </advc.134a>
	bool isNoTradeTech(TechTypes eIndex) const;																														// Exposed to Python
	void setNoTradeTech(TechTypes eIndex, bool bNewValue);																					// Exposed to Python

	int getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const										// Exposed to Python
	{
		return m_aaiImprovementYieldChange.get(eIndex1, eIndex2); // advc.inl
	}
	void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);		// Exposed to Python

	bool doesImprovementConnectBonus(ImprovementTypes eImprovement, BonusTypes eBonus) const; // K-Mod
	// <advc>
	bool canPeacefullyEnter(TeamTypes eTerritoryOwner) const
	{
		return (isOpenBorders(eTerritoryOwner) || isFriendlyTerritory(eTerritoryOwner));
	} // </advc>
	bool isFriendlyTerritory(TeamTypes eTerritoryOwner) const; // advc: param renamed
	bool canAccessHappyHealth(CvPlot const& kPlot, int iHealthOrHappy) const; // advc.901

	int getEspionageModifier(TeamTypes eTarget) const;								// Exposed to Python (though CyGameCoreUtils)
	int getEspionagePointsAgainstTeam(TeamTypes eIndex) const;																							// Exposed to Python
	void setEspionagePointsAgainstTeam(TeamTypes eIndex, int iValue);																							// Exposed to Python
	void changeEspionagePointsAgainstTeam(TeamTypes eIndex, int iChange);																				// Exposed to Python
	bool canSeeTech(TeamTypes eOther) const; // advc.120d

	int getTotalUnspentEspionage() const; // K-Mod

	int getEspionagePointsEver() const;																							// Exposed to Python
	void setEspionagePointsEver(int iValue);																							// Exposed to Python
	void changeEspionagePointsEver(int iChange);																				// Exposed to Python

	int getCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex) const;																							// Exposed to Python
	void setCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex, int iValue);																		// Exposed to Python
	void changeCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex, int iChange);																// Exposed to Python

	int getCounterespionageModAgainstTeam(TeamTypes eIndex) const;																							// Exposed to Python
	void setCounterespionageModAgainstTeam(TeamTypes eIndex, int iValue);																		// Exposed to Python
	void changeCounterespionageModAgainstTeam(TeamTypes eIndex, int iChange);																// Exposed to Python

	void verifySpyUnitsValidPlot();

	void setForceRevealedBonus(BonusTypes eBonus, bool bRevealed);
	bool isForceRevealedBonus(BonusTypes eBonus) const;
	bool isBonusRevealed(BonusTypes eBonus) const; // K-Mod. (the definitive answer)

	void revealSurroundingPlots(CvPlot const& kCenter, int iRange) const; // advc.108

	int countNumHumanGameTurnActive() const;
	void setTurnActive(bool bNewValue, bool bTurn = true);
	bool isTurnActive() const;

	bool hasShrine(ReligionTypes eReligion);

	DllExport void getCompletedSpaceshipProjects(std::map<ProjectTypes, int>& mapProjects) const;
	DllExport int getProjectPartNumber(ProjectTypes projectType, bool bAssert) const;
	DllExport bool hasLaunched() const;
	// advc:
	bool hasTechToClear(FeatureTypes eFeature, TechTypes eCurrentResearch = NO_TECH) const;
	void testCircumnavigated(); // advc.136a: Made public
	/*  <advc.127b> Both return -1 if no team member has a capital or
		(eObserver!=NO_TEAM) if none is revealed to eObserver. */
	int getCapitalX(TeamTypes eObserver, bool bDebug = false) const;
	int getCapitalY(TeamTypes eObserver, bool bDebug = false) const;
	CvCity* getLeaderCapital(TeamTypes eObserver, bool bDebug = false) const;
	// </advc.127b>
	void finalizeInit(); // advc.003m
	// <advc.003u>
	__forceinline CvTeamAI& AI()
	{	//return *static_cast<CvTeamAI*>(const_cast<CvTeam*>(this));
		/*  The above won't work in an inline function b/c the compiler doesn't know
			that CvTeamAI is derived from CvTeam */
		return *reinterpret_cast<CvTeamAI*>(this);
	}
	__forceinline CvTeamAI const& AI() const
	{	//return *static_cast<CvTeamAI const*>(this);
		return *reinterpret_cast<CvTeamAI const*>(this);
	} // </advc.003u>

protected:
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);
	// advc.003u: Keep one pure virtual function so that this class is abstract
	virtual void AI_makeAssignWorkDirty() = 0;
	// advc.003u: See the comments in the private section of CvPlayer.h before adding any virtual functions!

	TeamTypes m_eID; // advc: Moved up for easier access in the debugger

	static CvTeamAI** m_aTeams; // advc.003u: Moved from CvTeamAI.h; and store only pointers.

	int m_iNumMembers;
	int m_iAliveCount;
	int m_iEverAliveCount;
	int m_iNumCities;
	int m_iTotalPopulation;
	int m_iTotalLand;
	int m_iNukeInterception;
	int m_iExtraWaterSeeFromCount;
	int m_iMapTradingCount;
	int m_iTechTradingCount;
	int m_iGoldTradingCount;
	int m_iOpenBordersTradingCount;
	int m_iDefensivePactTradingCount;
	int m_iPermanentAllianceTradingCount;
	int m_iVassalTradingCount;
	int m_iBridgeBuildingCount;
	int m_iIrrigationCount;
	/* Population Limit ModComp - Beginning */
	int m_iNoPopulationLimitCount;
	/* Population Limit ModComp - End */
	int m_iIgnoreIrrigationCount;
	int m_iWaterWorkCount;
	int m_iVassalPower;
	int m_iMasterPower;
	int m_iEnemyWarWearinessModifier;
	int m_iRiverTradeCount;
	int m_iEspionagePointsEver;
	// <advc.003m>
	int m_iMajorWarEnemies; // incl. vassals
	int m_iMinorWarEnemies;
	int m_iVassalWarEnemies;
	// <advc.opt>
	TeamTypes m_eMaster;
	PlayerTypes m_eLeader;
	// </advc.opt>
	short m_iTechCount; // advc.101
	bool m_bMinorTeam;
	// </advc.003m>
	bool m_bMapCentering;
	bool m_bCapitulated;
	bool m_bAnyVictoryCountdown; // advc.opt

	// <advc.enum>
	EnumMap<TeamTypes,int> m_aiStolenVisibilityTimer; // Make this <...,char> when breaking saves
	EnumMap<TeamTypes,int> m_aiWarWeariness;
	EnumMap<TeamTypes,int> m_aiTechShareCount;
	EnumMap<TeamTypes,int> m_aiEspionagePointsAgainstTeam;
	EnumMap<TeamTypes,int> m_aiCounterespionageTurnsLeftAgainstTeam; // <...,short>?
	EnumMap<TeamTypes,int> m_aiCounterespionageModAgainstTeam; // <...,short>?

	EnumMap<CommerceTypes,int> m_aiCommerceFlexibleCount;
// < Civic Infos Plus Start >
//keldath QA-DONE
	EnumMap<YieldTypes,int> m_aiYieldRateModifier;
	EnumMap<CommerceTypes,int> m_aiCommerceRateModifier;
// < Civic Infos Plus Start >
	EnumMap<DomainTypes,int> m_aiExtraMoves;
	EnumMap<VoteSourceTypes,int> m_aiForceTeamVoteEligibilityCount;
	EnumMap<RouteTypes,int> m_aiRouteChange;

	// (These largish EnumMaps should arguably map to short)
	EnumMap<ProjectTypes,int> m_aiProjectCount;
	EnumMap<ProjectTypes,int> m_aiProjectMaking;
	// Could create an enum ProjectArtTypes { NO_PROJECTART=-1 } for this, but DLLExports make this more trouble than it's worth.
	EnumMap<ProjectTypes,int> m_aiProjectDefaultArtTypes;
	EnumMap<UnitClassTypes,int> m_aiUnitClassCount;
	EnumMap<BuildingClassTypes,int> m_aiBuildingClassCount;
	EnumMap<BuildingTypes,int> m_aiObsoleteBuildingCount;
	EnumMap<TechTypes,int> m_aiResearchProgress;
	EnumMap<TechTypes,int> m_aiTechCount;
	EnumMap<TerrainTypes,int> m_aiTerrainTradeCount;
	EnumMapDefault<VictoryTypes,int,-1> m_aiVictoryCountdown;

	EnumMap2D<ImprovementTypes,YieldTypes,int> m_aaiImprovementYieldChange; // Should make this <...,char>

	EnumMap<TeamTypes,bool> m_abAtWar;
	EnumMap<TeamTypes,bool> m_abJustDeclaredWar; // advc.162
	EnumMap<TeamTypes,bool> m_abHasMet;
	EnumMap<TeamTypes,bool> m_abHasSeen; // K-Mod
	EnumMap<TeamTypes,bool> m_abPermanentWarPeace;
	EnumMap<TeamTypes,bool> m_abOpenBorders;
	EnumMap<TeamTypes,bool> m_abDisengage; // advc.034
	EnumMap<TeamTypes,bool> m_abDefensivePact;
	EnumMap<TeamTypes,bool> m_abForcePeace;
	//EnumMap<TeamTypes,bool> m_abVassal; // advc.opt: Replaced by m_eMaster
	EnumMap<VictoryTypes,bool> m_abCanLaunch;
	EnumMap<TechTypes,bool> m_abHasTech;
	EnumMap<TechTypes,bool> m_abNoTradeTech;

	std::vector<int>* m_pavProjectArtTypes; // a vector for each type of project
	std::vector<BonusTypes> m_aeRevealedBonuses;
	// <advc.134a>
	mutable TeamTypes m_eOfferingPeace;
	mutable int m_iPeaceOfferStage;
	// </advc.134a>

	// <kekm.26>
	static std::queue<TeamTypes> attacking_queue;
	static std::queue<TeamTypes> defending_queue;
	static std::queue<bool> newdiplo_queue;
	static std::queue<WarPlanTypes> warplan_queue;
	static std::queue<bool> primarydow_queue;
	static bool bTriggeringWars;
	// </kekm.26>

	void uninit();

	void doWarWeariness();
	void doBarbarianResearch(); // advc
	void updateTechShare(TechTypes eTech);
	void updateTechShare();
	void updatePlotGroupBonus(TechTypes eTech, bool bAdd); // advc

	void processTech(TechTypes eTech, int iChange, /* advc.121: */ bool bEndOfTurn);

	void cancelDefensivePacts();
	void allowDefensivePactsToBeCanceled(); // kekm.3
	// <advc.003m>
	// New name for BBAI's getAtWarCount
	int countWarEnemies(bool bIgnoreMinors = true, bool bIgnoreVassals = false) const;
	void changeAtWarCount(int iChange, bool bMinorTeam, bool bVassal);
	// New name for isMinorCiv (uncached)
	bool checkMinorCiv() const; // </advc.003m>
	// <advc.039>
	CvWString const tradeItemString(TradeableItems eItem, int iData,
			TeamTypes eFrom) const; // </advc.039>
	void announceTechToPlayers(TechTypes eIndex,
			PlayerTypes eDiscoverPlayer, // advc.156
			bool bPartial = false);

private: // advc.003u: (See comments in the private section of CvPlayer.h)
	/*virtual void AI_initExternal();
	virtual void AI_resetExternal(bool bConstructor);
	virtual void AI_doTurnPreExternal();*/
	virtual void AI_doTurnPostExternal();
	virtual void AI_makeAssignWorkDirtyExternal();
	virtual void AI_updateAreaStrategiesExternal(
			bool bTargets = true);
	virtual bool AI_shareWarExternal(TeamTypes eTeam);
	virtual void AI_updateWorstEnemyExternal();
	virtual int AI_getAtWarCounterExternal(TeamTypes eIndex);
	virtual void AI_setAtWarCounterExternal(TeamTypes eIndex, int iNewValue);
	virtual int AI_getAtPeaceCounterExternal(TeamTypes eIndex);
	virtual void AI_setAtPeaceCounterExternal(TeamTypes eIndex, int iNewValue);
	virtual int AI_getHasMetCounterExternal(TeamTypes eIndex);
	virtual void AI_setHasMetCounterExternal(TeamTypes eIndex, int iNewValue);
	virtual int AI_getOpenBordersCounterExternal(TeamTypes eIndex);
	virtual void AI_setOpenBordersCounterExternal(TeamTypes eIndex, int iNewValue);
	virtual int AI_getDefensivePactCounterExternal(TeamTypes eIndex);
	virtual void AI_setDefensivePactCounterExternal(TeamTypes eIndex, int iNewValue);
	virtual int AI_getShareWarCounterExternal(TeamTypes eIndex);
	virtual void AI_setShareWarCounterExternal(TeamTypes eIndex, int iNewValue);
	virtual int AI_getWarSuccessExternal(TeamTypes eIndex);
	virtual void AI_setWarSuccessExternal(TeamTypes eIndex, int iNewValue);
	virtual void AI_changeWarSuccessExternal(TeamTypes eIndex, int iChange);
	virtual int AI_getEnemyPeacetimeTradeValueExternal(TeamTypes eIndex);
	virtual void AI_setEnemyPeacetimeTradeValueExternal(TeamTypes eIndex, int iNewValue);
	virtual int AI_getEnemyPeacetimeGrantValueExternal(TeamTypes eIndex);
	virtual void AI_setEnemyPeacetimeGrantValueExternal(TeamTypes eIndex, int iNewValue);
	virtual WarPlanTypes AI_getWarPlanExternal(TeamTypes eIndex);
	virtual bool AI_isChosenWarExternal(TeamTypes eIndex);
	virtual bool AI_isSneakAttackPreparingExternal(TeamTypes eIndex);
	virtual bool AI_isSneakAttackReadyExternal(TeamTypes eIndex);
	virtual void AI_setWarPlanExternal(TeamTypes eIndex, WarPlanTypes eNewValue,
			bool bWar = true);
	virtual void readExternal(FDataStreamBase* pStream);
	virtual void writeExternal(FDataStreamBase* pStream);
};

#endif
