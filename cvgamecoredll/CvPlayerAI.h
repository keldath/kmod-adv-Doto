#pragma once

#ifndef CIV4_PLAYER_AI_H
#define CIV4_PLAYER_AI_H

#include "CvPlayer.h"
#include "UWAI.h" // advc.104
#include "AIStrategies.h" // advc.enum

class CvDeal;
class CvCityAI;
class CvUnitAI;
class CvSelectionGroupAI;

/*	<advc.003u> Overwrite definition in CvPlayer.h (should perhaps instead define a
	new macro "PLAYERAI" - a lot of call locations to change though ...) */
#undef GET_PLAYER
#define GET_PLAYER(x) CvPlayerAI::AI_getPlayer(x)
// </advc.003u>

class CvPlayerAI : public CvPlayer
{
public:
	// advc.003u: Renamed from getPlayer
	static inline CvPlayerAI& AI_getPlayer(PlayerTypes ePlayer) // advc.inl
	{
		FAssertBounds(0, MAX_PLAYERS, ePlayer);
		return *m_aPlayers[ePlayer];
	}
	// Only for the EXE:
	DllExport static CvPlayerAI& getPlayerNonInl(PlayerTypes ePlayer);
	DllExport static bool areStaticsInitialized();

	explicit CvPlayerAI(PlayerTypes eID);
	~CvPlayerAI();
	void AI_init();
	void AI_uninit();
	void AI_reset(bool bConstructor);
	void AI_updatePersonality(); // advc.104

	// <advc.003u> Access to AI-type members. Code mostly duplicated from CvPlayer.
	inline CvCityAI* AI_getCapitalCity() const {
		return AI_getCity(m_iCapitalCityID);
	}
	inline CvCityAI* AI_firstCity(int *pIterIdx) const {
		return m_cities.AI_beginIter(pIterIdx);
	}
	inline CvCityAI* AI_nextCity(int *pIterIdx) const {
		return m_cities.AI_nextIter(pIterIdx);
	}
	inline CvCityAI* AI_getCity(int iID) const {
		return m_cities.AI_getAt(iID);
	}
	inline CvUnitAI* AI_firstUnit(int *pIterIdx) const {
		return m_units.AI_beginIter(pIterIdx);
	}
	inline CvUnitAI* AI_nextUnit(int *pIterIdx) const {
		return m_units.AI_nextIter(pIterIdx);
	}
	inline CvUnitAI* AI_getUnit(int iID) const {
		return m_units.AI_getAt(iID);
	}
	inline CvSelectionGroupAI* AI_firstSelectionGroup(int *pIterIdx) const {
		return m_selectionGroups.AI_beginIter(pIterIdx);
	}
	inline CvSelectionGroupAI* AI_nextSelectionGroup(int *pIterIdx) const {
		return m_selectionGroups.AI_nextIter(pIterIdx);
	}
	inline CvSelectionGroupAI* AI_getSelectionGroup(int iID) const {
		return m_selectionGroups.AI_getAt(iID);
	}
	// </advc.003u>
	int AI_getFlavorValue(FlavorTypes eFlavor) const;

	void AI_updateCacheData(); // K-Mod

	void AI_doTurnPre();
	void AI_doTurnPost();
	void AI_doTurnUnitsPre();
	void AI_doTurnUnitsPost();

	void AI_doPeace();
	// advc.134a:
	bool AI_upholdPeaceOffer(PlayerTypes eHuman, CvDiploParameters const& kOffer) const;

	void AI_updateFoundValues(bool bStartingLoc = false);
	void AI_updateAreaTargets();

	int AI_movementPriority(CvSelectionGroupAI const& kGroup) const;
	void AI_unitUpdate();

	void AI_makeAssignWorkDirty();
	void AI_assignWorkingPlots();
	void AI_updateAssignWork();

	void AI_makeProductionDirty();
	#if 0 // advc
	void AI_doCentralizedProduction(); // K-Mod. (not used)
	#endif
	void AI_conquerCity(CvCityAI& kCity);
	double AI_razeMemoryScore(CvCity const& c) const; // advc.130q
	bool AI_acceptUnit(CvUnit const& kUnit) const;
	bool AI_captureUnit(UnitTypes eUnit, CvPlot const& kPlot) const;

	DomainTypes AI_unitAIDomainType(UnitAITypes eUnitAI) const;

	int AI_yieldWeight(YieldTypes eYield, const CvCity* pCity = 0) const; // K-Mod added city argument
	int AI_commerceWeight(CommerceTypes eCommerce, const CvCityAI* pCity = NULL) const;
	void AI_updateCommerceWeights(); // K-Mod

	// <advc.035>
	bool AI_isPlotContestedByRival(CvPlot const& kPlot,
			PlayerTypes eRival = NO_PLAYER) const; // </advc.035>
	short AI_foundValue(int iX, int iY, int iMinRivalRange = -1, bool bStartingLoc = false) const;		// Exposed to Python
	// advc: Replaced by the CitySiteEvaluator class
	//struct CvFoundSettings { ... } // K-Mod
	//short AI_foundValue_bulk(int iX, int iY, const CvFoundSettings& kSet) const; // K-Mod
	//int AI_countDeadlockedBonuses(CvPlot const* pPlot) const;

	bool AI_isAreaAlone(CvArea const& kArea) const;
	bool AI_isCapitalAreaAlone() const;
	bool AI_isPrimaryArea(CvArea const& kArea) const;
	int AI_militaryWeight(CvArea const* pArea = NULL) const;
	bool AI_feelsSafe() const; // advc.109

	int AI_targetCityValue(CvCity const* pCity, bool bRandomize, bool bIgnoreAttackers = false) const;
	CvCityAI* AI_findTargetCity(CvArea const& kArea) const;
	int AI_cityWonderVal(CvCity const& c) const; // advc.104d
	scaled AI_assetVal(CvCityAI const& c, bool bConquest) const; // advc

	// BETTER_BTS_AI_MOD, 08/20/09, jdog5000: START
	bool isSafeRangeCacheValid() const; // K-Mod
	// <advc.104>
	struct LowHPCounter
	{	// Param: threshold for "low" hitpoints
		LowHPCounter(int iMaxHP = 60) : m_iMaxHP(iMaxHP), m_iCount(0) { FAssert(iMaxHP > 0); }
		inline int getHPThreshold() const { return m_iMaxHP; }
		inline int get() const { return m_iCount; }
		inline void increment() { m_iCount++; }
		private:
		int m_iCount;
		int m_iMaxHP;
	}; // </advc.104>
	int AI_getPlotDanger(CvPlot const& kPlot, int iRange = -1, bool bTestMoves = true,
			int iLimit = MAX_INT, // advc  <advc.104>
			bool bCheckBorder = true, LowHPCounter* pLowHPCounter = NULL,
			PlayerTypes eAttackPlayer = NO_PLAYER) const; // </advc.104>
	bool AI_isAnyPlotDanger(CvPlot const& kPlot, int iRange = -1, bool bTestMoves = true,
		bool bCheckBorder = true) const // K-Mod
	{	// advc: Merged with the plot danger counting function
		return (AI_getPlotDanger(kPlot, iRange, bTestMoves, 1, bCheckBorder) > 0);
	}
	//int AI_getUnitDanger(CvUnit* pUnit, int iRange = -1, bool bTestMoves = true, bool bAnyDanger = true) const;
	// BETTER_BTS_AI_MOD: END
	int AI_getWaterDanger(CvPlot* pPlot, int iRange, bool bTestMoves = true) const;

	bool AI_avoidScience() const;
	int AI_financialTroubleMargin() const; // advc.110
	bool AI_isFinancialTrouble() const;							// Exposed to Python
	//int AI_goldTarget() const;
	int AI_goldTarget(bool bUpgradeBudgetOnly = false) const; // K-Mod

	TechTypes AI_bestTech(int iMaxPathLength = 1, bool bFreeTech = false, bool bAsync = false,
			TechTypes eIgnoreTech = NO_TECH, AdvisorTypes eIgnoreAdvisor = NO_ADVISOR,
			PlayerTypes eFromPlayer = NO_PLAYER) const; // advc.144

	// BETTER_BTS_AI_MOD, Tech AI, 03/18/10, jdog5000: START
	int AI_techValue(TechTypes eTech, int iPathLength, bool bFreeTech, bool bAsync,
			const std::vector<int>& viBonusClassRevealed,
			const std::vector<int>& viBonusClassUnrevealed,
			const std::vector<int>& viBonusClassHave,
			PlayerTypes eFromPlayer = NO_PLAYER) const; // advc.144
	int AI_obsoleteBuildingPenalty(TechTypes eTech, bool bConstCache) const; // K-Mod
	int AI_techBuildingValue(TechTypes eTech, bool bConstCache, bool& bEnablesWonder) const; // Rewritten for K-Mod
	int AI_techUnitValue(TechTypes eTech, int iPathLength, bool &bEnablesUnitWonder) const;
	// BETTER_BTS_AI_MOD: END
	// k146:
	int AI_techProjectValue(TechTypes eTech, int iPathLength, bool &bEnablesProjectWonder) const;
	int AI_cultureVictoryTechValue(TechTypes eTech) const;

	void AI_chooseFreeTech();
	void AI_chooseResearch();

	DllExport DiploCommentTypes AI_getGreeting(PlayerTypes ePlayer) const;
	bool AI_isWillingToTalk(PlayerTypes ePlayer,
			bool bAsync = false) const; // advc.104l
	int AI_refuseToTalkTurns(PlayerTypes ePlayer) const; // advc.104i
	bool AI_demandRebukedSneak(PlayerTypes ePlayer) const;
	bool AI_demandRebukedWar(PlayerTypes ePlayer) const;
	bool AI_hasTradedWithTeam(TeamTypes eTeam) const;

	void AI_updateAttitude(); // K-Mod (for all players)
	void AI_updateAttitude(PlayerTypes ePlayer,		// K-Mod
			bool bUpdateWorstEnemy = true); // advc.130e
	void AI_changeCachedAttitude(PlayerTypes ePlayer, int iChange); // K-Mod
	AttitudeTypes AI_getAttitude(PlayerTypes ePlayer, bool bForced = true) const		// Exposed to Python
	{	// advc.inl
		FAssert(ePlayer != getID());
		return (AI_getAttitudeFromValue(AI_getAttitudeVal(ePlayer, bForced)));
	}
	int AI_getAttitudeVal(PlayerTypes ePlayer, bool bForced = true) const;
	static AttitudeTypes AI_getAttitudeFromValue(int iAttitudeVal);

	int AI_calculateStolenCityRadiusPlots(PlayerTypes ePlayer,
			bool bOnlyNonWorkable = false) const; // advc.147
	void AI_updateCloseBorderAttitude(); // K-Mod
	void AI_updateCloseBorderAttitude(PlayerTypes ePlayer); // K-Mod
	int AI_getCloseBordersAttitude(PlayerTypes ePlayer) const;
	int warSuccessAttitudeDivisor() const; // advc.130y, advc.sha
	int AI_getWarAttitude(PlayerTypes ePlayer,
			int iPartialSum = MIN_INT) const; // advc.sha
	int AI_getPeaceAttitude(PlayerTypes ePlayer) const;
	int AI_getSameReligionAttitude(PlayerTypes ePlayer) const;
	int AI_getDifferentReligionAttitude(PlayerTypes ePlayer) const;
	int AI_getBonusTradeAttitude(PlayerTypes ePlayer) const;
	int AI_getOpenBordersAttitude(PlayerTypes ePlayer) const;
	int AI_getDefensivePactAttitude(PlayerTypes ePlayer) const;
	int AI_getRivalDefensivePactAttitude(PlayerTypes ePlayer) const;
	int AI_getRivalVassalAttitude(PlayerTypes ePlayer) const;
	int AI_getShareWarAttitude(PlayerTypes ePlayer) const;
	int AI_getFavoriteCivicAttitude(PlayerTypes ePlayer) const;
	int AI_getTradeAttitude(PlayerTypes ePlayer) const;
	int AI_getRivalTradeAttitude(PlayerTypes ePlayer) const;
	int AI_getBonusTradeCounter(TeamTypes eTo) const; // advc.130p
	int AI_getMemoryAttitude(PlayerTypes ePlayer, MemoryTypes eMemory) const;
	//int AI_getColonyAttitude(PlayerTypes ePlayer) const; // advc.130r
	// BEGIN: Show Hidden Attitude Mod 01/22/2010
	int AI_getFirstImpressionAttitude(PlayerTypes ePlayer) const;
	int AI_getTeamSizeAttitude(PlayerTypes ePlayer) const;
	// advc.sha: One function for both BetterRank and WorseRank
	int AI_getRankDifferenceAttitude(PlayerTypes ePlayer) const;
	//int AI_getLowRankAttitude(PlayerTypes ePlayer) const; // advc.sha
	int AI_getLostWarAttitude(PlayerTypes ePlayer) const;
	//int AI_getKnownPlayerRank(PlayerTypes ePlayer) const; // advc.sha
	// END: Show Hidden Attitude Mod
	int AI_getExpansionistAttitude(PlayerTypes ePlayer) const; // advc.130w
//dune wars - hated civs
	int AI_getHatedCivicAttitude(PlayerTypes ePlayer) const; //a1021
	int AI_getFavoriteCivilizationAttitude(PlayerTypes ePlayer) const; //a1021
	int AI_getHatedCivilizationAttitude(PlayerTypes ePlayer) const; //a1021	
//dune wars - hated civs
	PlayerVoteTypes AI_diploVote(const VoteSelectionSubData& kVoteData, VoteSourceTypes eVoteSource, bool bPropose);

	int AI_dealVal(PlayerTypes eFromPlayer, CLinkList<TradeData> const& kList,
			bool bIgnoreAnnual = false,
			int iChange = 1, /* advc: was called iExtra, which didn't make sense
								and differed from the parameter name in CvPlayerAI.cpp. */
			bool bIgnoreDiscount = false, // advc.550a
			bool bIgnorePeace = false, // advc.130p
			bool bCountLiberation = false) const; // advc.ctr
	bool AI_goldDeal(CLinkList<TradeData> const& kList) const;
	bool isAnnualDeal(CLinkList<TradeData> const& itemList) const; // advc.705
	bool AI_considerOffer(PlayerTypes ePlayer, CLinkList<TradeData> const& kTheyGive,
			CLinkList<TradeData> const& kWeGive, int iChange = 1, /* advc.133: */ int iDealAge = 0,
			// <advc.130o> May change diplo memory if true; const qualifier removed.
			bool bHypothetical = false);
	// const wrapper
	bool AI_considerHypotheticalOffer(PlayerTypes ePlayer, CLinkList<TradeData> const& kTheyGive,
			CLinkList<TradeData> const& kWeGive, int iChange = 1, /* advc.133: */ int iDealAge = 0) const
	{
		return const_cast<CvPlayerAI*>(this)->AI_considerOffer(ePlayer, kTheyGive, kWeGive, iChange, iDealAge, true);
	} // </advc.130o>
	double AI_prDenyHelp() const; // advc.144
	int AI_tradeAcceptabilityThreshold(PlayerTypes eTrader) const; // K-Mod
	int AI_maxGoldTrade(PlayerTypes ePlayer, /* advc.134a: */ bool bTeamTrade = false) const;
	int AI_maxGoldPerTurnTrade(PlayerTypes ePlayer) const;								// Exposed to Python
	int AI_goldPerTurnTradeVal(int iGoldPerTurn) const;
	int AI_bonusVal(BonusTypes eBonus, int iChange,
			bool bAssumeEnabled = false, // K-Mod
			// advc.036: Whether baseBonusVal is computed for a resource trade
			bool bTrade = false) const;
	int AI_baseBonusVal(BonusTypes eBonus, /* advc.036: */ bool bTrade = false) const;
	int AI_bonusTradeVal(BonusTypes eBonus, PlayerTypes eFromPlayer, int iChange,
			bool bExtraHappyOrHealth = false) const; // advc.036
	DenialTypes AI_bonusTrade(BonusTypes eBonus, PlayerTypes eToPlayer,
			int iChange = 0) const; // advc.133
	// advc.210e: Exposed to Python
	int AI_corporationBonusVal(BonusTypes eBonus, /* advc.036: */ bool bTrade = false) const;
	int AI_goldForBonus(BonusTypes eBonus, PlayerTypes eBonusOwner) const; // advc.036
	// <advc.ctr>
	enum LiberationWeightTypes
	{
		LIBERATION_WEIGHT_ZERO, // 0 trade value when eToPlayer is the liberation player
		LIBERATION_WEIGHT_REDUCED,
		LIBERATION_WEIGHT_FULL
	}; // </advc.ctr>
	int AI_cityTradeVal(CvCityAI const& kCity,  // <advc.ctr>
			PlayerTypes eToPlayer = NO_PLAYER,
			// (The function will ignore this in some situations)
			LiberationWeightTypes eLibWeight = LIBERATION_WEIGHT_ZERO,
			bool bConquest = false) const; // </advc.ctr>
	DenialTypes AI_cityTrade(CvCityAI const& kCity, PlayerTypes ePlayer) const;

	int AI_stopTradingTradeVal(TeamTypes eTradeTeam, PlayerTypes ePlayer,
			bool bWarTrade = false) const; // advc.104o
	DenialTypes AI_stopTradingTrade(TeamTypes eTradeTeam, PlayerTypes ePlayer) const;

	int AI_civicTradeVal(CivicTypes eCivic, PlayerTypes ePlayer) const;
	DenialTypes AI_civicTrade(CivicTypes eCivic, PlayerTypes ePlayer) const;

	int AI_religionTradeVal(ReligionTypes eReligion, PlayerTypes ePlayer) const;
	DenialTypes AI_religionTrade(ReligionTypes eReligion, PlayerTypes ePlayer) const;

	int AI_unitImpassableCount(UnitTypes eUnit) const;

	int AI_unitValue(UnitTypes eUnit, UnitAITypes eUnitAI, CvArea const* pArea = NULL) const;			// Exposed to Python
	int AI_totalUnitAIs(UnitAITypes eUnitAI) const;														// Exposed to Python
	int AI_totalAreaUnitAIs(CvArea const& kArea, UnitAITypes eUnitAI) const;							// Exposed to Python
	int AI_totalWaterAreaUnitAIs(CvArea const& kArea, UnitAITypes eUnitAI) const;						// Exposed to Python
	// advc.081:
	int AI_totalWaterAreaUnitAIs(CvArea const& kArea, std::vector<UnitAITypes> const& aeUnitAI) const;
	int AI_countCargoSpace(UnitAITypes eUnitAI) const;

	int AI_neededExplorers(CvArea const& kArea) const;
	void AI_updateNeededExplorers(); // advc.opt
	// <advc.017b>
	bool AI_isExcessSeaExplorers(CvArea const& kWaterArea, int iChange = 0) const;
	bool AI_isOutdatedUnit(UnitTypes eUnit, UnitAITypes eRole, CvArea const* pArea = NULL) const;
	// </advc.017b>
	// <advc.042> Moved from CvPlayer and iLookAhead param added
	int AI_countUnimprovedBonuses(CvArea const& kArea, CvPlot* pFromPlot = NULL, int iLookAhead = 0) const;														// Exposed to Python
	int AI_countOwnedBonuses(BonusTypes eBonus) const;																											// Exposed to Python
	// </advc.042>
	int AI_neededWorkers(CvArea const& kArea) const;
	int AI_neededMissionaries(CvArea const& kArea, ReligionTypes eReligion) const;
	int AI_neededExecutives(CvArea const& kArea, CorporationTypes eCorporation) const;
	int AI_unitCostPerMil() const; // K-Mod
	int AI_maxUnitCostPerMil(CvArea const* pArea = NULL, int iBuildProb = -1) const; // K-Mod
	int AI_nukeWeight() const; // K-Mod
	int AI_nukeDangerDivisor() const; // dlph.16
	bool AI_isLandWar(CvArea const& kArea) const; // K-Mod
	bool AI_isFocusWar(CvArea const* pArea = NULL) const; // advc.105

	int AI_missionaryValue(ReligionTypes eReligion, CvArea const* pArea = NULL/*, PlayerTypes* peBestPlayer = NULL*/) const;
	int AI_executiveValue(CorporationTypes eCorporation, CvArea const* pArea = NULL,
			PlayerTypes* peBestPlayer = NULL, bool bSpreadOnly = false) const;
	// advc.171:
	bool AI_isTargetForMissionaries(PlayerTypes eTarget, ReligionTypes eReligion) const;
	int AI_corporationValue(CorporationTypes eCorporation, CvCityAI const* pCity = NULL) const;

	int AI_adjacentPotentialAttackers(CvPlot const& kPlot, bool bTestCanMove = false) const;
	int AI_totalMissionAIs(MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_areaMissionAIs(CvArea const& kArea, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	// advc: const CvPlot*
	int AI_plotTargetMissionAIs(CvPlot const* pPlot, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0) const;
	int AI_plotTargetMissionAIs(CvPlot const* pPlot, MissionAITypes eMissionAI, int& iClosestTargetRange, CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0) const;
	int AI_plotTargetMissionAIs(CvPlot const* pPlot, MissionAITypes* aeMissionAI, int iMissionAICount, int& iClosestTargetRange, CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0) const;
	int AI_unitTargetMissionAIs(CvUnit const* pUnit, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_unitTargetMissionAIs(CvUnit const* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_enemyTargetMissionAIs(MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_enemyTargetMissionAIs(MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_wakePlotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	// K-Mod start
	int AI_localDefenceStrength(const CvPlot* pDefencePlot, TeamTypes eDefenceTeam, DomainTypes eDomainType = DOMAIN_LAND, int iRange = 0, bool bAtTarget = true, bool bCheckMoves = false, bool bNoCache = false,
			bool bPredictPromotions = false) const; // advc.139
	int AI_localAttackStrength(const CvPlot* pTargetPlot, TeamTypes eAttackTeam, DomainTypes eDomainType = DOMAIN_LAND, int iRange = 2, bool bUseTarget = true, bool bCheckMoves = false, bool bCheckCanAttack = false,
			int* piAttackerCount = NULL) const; // advc.139
	int AI_cityTargetStrengthByPath(CvCity const* pCity, CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns) const;
	// K-Mod end
	void AI_attackMadeAgainst(CvUnit const& kDefender); // advc.139
	// BBAI start
	int AI_enemyTargetMissions(TeamTypes eTargetTeam, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_unitTargetMissionAIs(CvUnit const* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns) const;
	// BBAI end

	CivicTypes AI_bestCivic(CivicOptionTypes eCivicOption, int* iBestValue = 0) const;
	int AI_civicValue(CivicTypes eCivic) const;						// Exposed to Python

	ReligionTypes AI_bestReligion() const;
	int AI_religionValue(ReligionTypes eReligion) const;

	//EspionageMissionTypes AI_bestPlotEspionage(CvPlot* pSpyPlot, PlayerTypes& eTargetPlayer, CvPlot*& pPlot, int& iData) const;
	// K-Mod has moved AI_bestPlotEspionage to CvUnitAI::
	int AI_espionageVal(PlayerTypes eTargetPlayer, EspionageMissionTypes eMission, CvPlot* pPlot, int iData) const;
	bool AI_isMaliciousEspionageTarget(PlayerTypes eTarget) const; // K-Mod

	int AI_getPeaceWeight() const;
	void AI_setPeaceWeight(int iNewValue);

	int AI_getEspionageWeight() const;
	void AI_setEspionageWeight(int iNewValue);

	int AI_getAttackOddsChange() const;
	void AI_setAttackOddsChange(int iNewValue);

	int AI_getCivicTimer() const;
	void AI_setCivicTimer(int iNewValue);
	void AI_changeCivicTimer(int iChange);

	int AI_getReligionTimer() const;
	void AI_setReligionTimer(int iNewValue);
	void AI_changeReligionTimer(int iChange);

	int AI_getExtraGoldTarget() const;
	void AI_setExtraGoldTarget(int iNewValue);

	int AI_getNumTrainAIUnits(UnitAITypes eIndex) const;
	void AI_changeNumTrainAIUnits(UnitAITypes eIndex, int iChange);

	int AI_getNumAIUnits(UnitAITypes eIndex) const;							// Exposed to Python
	void AI_changeNumAIUnits(UnitAITypes eIndex, int iChange);

	int AI_getSameReligionCounter(PlayerTypes eIndex) const;
	void AI_changeSameReligionCounter(PlayerTypes eIndex, int iChange);

	int AI_getDifferentReligionCounter(PlayerTypes eIndex) const;
	void AI_changeDifferentReligionCounter(PlayerTypes eIndex, int iChange);

	int AI_getFavoriteCivicCounter(PlayerTypes eIndex) const;
	void AI_changeFavoriteCivicCounter(PlayerTypes eIndex, int iChange);

	int AI_getBonusTradeCounter(PlayerTypes eIndex) const;
	void AI_changeBonusTradeCounter(PlayerTypes eIndex, int iChange);
	/* <advc.130p> For code shared by AI_processPeacetimeTradeValue and
		AI_processPeacetimeGrantValue. The third parameter says which of the two
		should be changed. */
	void AI_processPeacetimeValue(PlayerTypes eIndex, int iChange, bool bGrant,
			bool bPeace = false, TeamTypes ePeaceTradeTarget = NO_TEAM,
			TeamTypes eWarTradeTarget = NO_TEAM); // </advc.130p>
	int AI_getPeacetimeTradeValue(PlayerTypes eIndex) const;
	// advc.130p: Renamed from changePeacetimeTradeValue
	void AI_processPeacetimeTradeValue(PlayerTypes eIndex, int iChange);
	int AI_getPeacetimeGrantValue(PlayerTypes eIndex) const;
	// advc.130p: Renamed from changePeacetimeTradeValue
	void AI_processPeacetimeGrantValue(PlayerTypes eIndex, int iChange);
	// <advc.130k> To make exponential decay more convenient
	void AI_setSameReligionCounter(PlayerTypes eIndex, int iValue);
	void AI_setDifferentReligionCounter(PlayerTypes eIndex, int iValue);
	void AI_setFavoriteCivicCounter(PlayerTypes eIndex, int iValue);
	void AI_setBonusTradeCounter(PlayerTypes eIndex, int iValue);
	// </advc.130k>
	int AI_getGoldTradedTo(PlayerTypes eIndex) const;
	void AI_changeGoldTradedTo(PlayerTypes eIndex, int iChange);

	int AI_getAttitudeExtra(PlayerTypes eIndex) const;							// Exposed to Python
	void AI_setAttitudeExtra(PlayerTypes eIndex, int iNewValue);				// Exposed to Python
	void AI_changeAttitudeExtra(PlayerTypes eIndex, int iChange);				// Exposed to Python

	bool AI_isFirstContact(PlayerTypes eIndex) const;
	void AI_setFirstContact(PlayerTypes eIndex, bool bNewValue);

	int AI_getContactTimer(PlayerTypes eIndex1, ContactTypes eIndex2) const;
	void AI_changeContactTimer(PlayerTypes eIndex1, ContactTypes eIndex2, int iChange);

	int AI_getMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2) const;
	void AI_changeMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2, int iChange);
	// advc: setter added
	inline void AI_setMemoryCount(PlayerTypes eAboutPlayer, MemoryTypes eMemoryType, int iValue);
	// advc.130j: Increases memory count according to (hardcoded) granularity
	void AI_rememberEvent(PlayerTypes ePlayer, MemoryTypes eMemoryType);
	void AI_rememberLiberation(CvCity const& kCity, bool bConquest); // advc.ctr
	void AI_processRazeMemory(CvCity const& kCity); // advc.003n

	// K-Mod
	int AI_getCityTargetTimer() const;
	void AI_setCityTargetTimer(int iTurns);
	// K-Mod end

	int AI_calculateGoldenAgeValue(bool bConsiderRevolution = true) const;

	void AI_doCommerce();

	EventTypes AI_chooseEvent(int iTriggeredId) const;
	void AI_launch(VictoryTypes eVictory);

	int AI_calculateCultureVictoryStage(
			int iCountdownThresh = -1) const; // advc.115

	// BETTER_BTS_AI_MOD, Victory Strategy AI, 03/17/10, jdog5000: START
	/* (functions renamed and edited for K-Mod;
		advc: 'calculate' functions moved to protected section;
		advc.enum: Renamed to emphasize distinction between victory and non-victory strats) */
	bool AI_atVictoryStage(AIVictoryStage eStage) const;
	bool AI_atVictoryStage4() const;
	bool AI_atVictoryStage3() const;
	inline AIVictoryStage AI_getVictoryStageHash() const { return m_eVictoryStageHash; }
	void AI_updateVictoryStageHash(); // K-Mod
	void AI_initStrategyRand(); // K-Mod
	int AI_getStrategyRand(int iShift) const;
	// BETTER_BTS_AI_MOD: END
	bool isCloseToReligiousVictory() const;
	bool AI_isDoStrategy(AIStrategy eStrategy, /* advc.007: */ bool bDebug = false) const;

	void AI_updateGreatPersonWeights(); // K-Mod
	int AI_getGreatPersonWeight(UnitClassTypes eGreatPerson) const; // K-Mod

	void AI_nowHasTech(TechTypes eTech);

	//int AI_goldToUpgradeAllUnits(int iExpThreshold = 0) const;
	// K-Mod
	inline int AI_getGoldToUpgradeAllUnits() const { return m_iUpgradeUnitsCachedGold; }
	void AI_updateGoldToUpgradeAllUnits();
	inline int AI_getAvailableIncome() const { return m_iAvailableIncome; }
	void AI_updateAvailableIncome();
	int AI_estimateBreakEvenGoldPercent() const;
	// K-Mod end

	int AI_goldTradeValuePercent() const;

	int AI_averageYieldMultiplier(YieldTypes eYield) const;
	int AI_averageCommerceMultiplier(CommerceTypes eCommerce) const;
	int AI_averageGreatPeopleMultiplier() const;
	int AI_averageCulturePressure() const; // K-Mod
	int AI_averageCommerceExchange(CommerceTypes eCommerce) const;

	int AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance,
			bool bConstCache = false) const; // advc.001n

	int AI_getTotalCityThreat() const;
	int AI_getTotalFloatingDefenseNeeded() const;


	int AI_getTotalAreaCityThreat(CvArea const& kArea) const;
	int AI_getAreaCultureDefendersNeeded(CvArea const& kArea) const; // advc.099c
	int AI_countNumAreaHostileUnits(CvArea const& kArea, bool bPlayer, bool bTeam, bool bNeutral, bool bHostile,
			CvPlot* pCenter = NULL) const; // advc.081
	int AI_getTotalFloatingDefendersNeeded(CvArea const& kArea,
			bool bDebug = false) const; // advc.007
	int AI_getTotalFloatingDefenders(CvArea const& kArea) const;
	int AI_getTotalAirDefendersNeeded() const; // K-Mod

	RouteTypes AI_bestAdvancedStartRoute(CvPlot* pPlot, int* piYieldValue = NULL) const;
	UnitTypes AI_bestAdvancedStartUnitAI(CvPlot const& kPlot, UnitAITypes eUnitAI) const;
	CvPlot* AI_advancedStartFindCapitalPlot();

	bool AI_advancedStartPlaceExploreUnits(bool bLand);
	void AI_advancedStartRevealRadius(CvPlot* pPlot, int iRadius);
	bool AI_advancedStartPlaceCity(CvPlot* pPlot);
	bool AI_advancedStartDoRoute(CvPlot* pFromPlot, CvPlot* pToPlot);
	void AI_advancedStartRouteTerritory();
	void AI_doAdvancedStart(bool bNoExit = false);

	int AI_getMinFoundValue() const;

	void AI_recalculateFoundValues(int iX, int iY, int iInnerRadius, int iOuterRadius) const;

	void AI_updateCitySites(int iMinFoundValueThreshold = -1, int iMaxSites = 4); // advc: default values
	void AI_invalidateCitySites(int iMinFoundValueThreshold);

	int AI_getNumCitySites() const;
	bool AI_isPlotCitySite(CvPlot const& kPlot) const; // advc: Made plot param const
	int AI_getNumAreaCitySites(CvArea const& kArea, int& iBestValue) const;
	int AI_getNumAdjacentAreaCitySites(int& iBestValue, CvArea const& kWaterArea,
			CvArea const* pExcludeArea = NULL) const;
	int AI_getNumPrimaryAreaCitySites(int iMinimumValue = 0) const; // K-Mod
	CvPlot* AI_getCitySite(int iIndex) const;
	// advc.117, advc.121:
	bool AI_isAdjacentCitySite(CvPlot const& p, bool bCheckCenter) const;
	bool AI_isAwfulSite(CvCity const& kCity) const; // advc.ctr
	bool AI_deduceCitySite(const CvCity* pCity) const; // K-Mod
	// K-Mod:
	int AI_countPotentialForeignTradeCities(bool bCheckConnected = true,
			bool bCheckForeignTradePolicy = true, CvArea const* pIgnoreArea = 0) const;

	int AI_bestAreaUnitAIValue(UnitAITypes eUnitAI, CvArea const* pArea = NULL, UnitTypes* peBestUnitType = NULL) const;
	int AI_bestCityUnitAIValue(UnitAITypes eUnitAI, CvCity const* pCity, UnitTypes* peBestUnitType = NULL) const;

	int AI_calculateTotalBombard(DomainTypes eDomain) const;

	void AI_updateBonusValue(BonusTypes eBonus);
	void AI_updateBonusValue();

	int AI_getUnitClassWeight(UnitClassTypes eUnitClass) const;
	int AI_getUnitCombatWeight(UnitCombatTypes eUnitCombat) const;
	int AI_calculateUnitAIViability(UnitAITypes eUnitAI, DomainTypes eDomain) const;

	int AI_disbandValue(CvUnitAI const& kUnit, bool bMilitaryOnly = true) const; // K-Mod

	int AI_getAttitudeWeight(PlayerTypes ePlayer) const;

	ReligionTypes AI_chooseReligion();

	int AI_getPlotAirbaseValue(CvPlot const& kPlot) const;
	int AI_getPlotCanalValue(CvPlot const& kPlot) const;

	int AI_getHappinessWeight(int iHappy, int iExtraPop, bool bPercent=false) const;
	int AI_getHealthWeight(int iHealth, int iExtraPop, bool bPercent=false) const;

	bool AI_isPlotThreatened(CvPlot* pPlot, int iRange = -1, bool bTestMoves = true) const;

	bool AI_isFirstTech(TechTypes eTech) const;

	void AI_ClearConstructionValueCache(); // K-Mod
	// k146: Used in conjuction with canTrain
	bool AI_haveResourcesToTrain(UnitTypes eUnit) const;
	UnitTypes AI_getBestAttackUnit() const; // advc.079

	// <advc.104>
	inline UWAI::Civ& uwai() { return *m_pUWAI; }
	inline UWAI::Civ const& uwai() const { return *m_pUWAI; } // </advc.104>
	// <advc.104h>
	// Returns true if peace deal implemented (or offered to human)
	bool AI_negotiatePeace(PlayerTypes eOther, int iTheirBenefit, int iOurBenefit);
	void AI_offerCapitulation(PlayerTypes eTo);
	// </advc.104h>
	bool AI_willOfferPeace(PlayerTypes eTo) const; // advc
	// advc.130h:
	bool AI_disapprovesOfDoW(TeamTypes eAggressor, TeamTypes eVictim) const;
	bool AI_isDangerFromSubmarines() const; // advc.651
	bool AI_isPiracyTarget(PlayerTypes eTarget) const; // advc.033
	/* Are there cities with sufficient production to
	   train a significant number of units of type eUnit? */
	bool AI_canBeExpectedToTrain(UnitTypes eUnit) const; // advc.104, advc.651
	bool AI_isDefenseFocusOnBarbarians(CvArea const& kArea) const; // advc.300
	// advc.001: needed for bNeighbouringReligion in AI_techValue
	bool AI_hasSharedPrimaryArea(PlayerTypes eOther) const;

	// <advc.104m>
	bool AI_proposeEmbargo(PlayerTypes eHuman);
	bool AI_contactReligion(PlayerTypes eHuman);
	bool AI_contactCivics(PlayerTypes eHuman);
	bool AI_askHelp(PlayerTypes eHuman);
	bool AI_demandTribute(PlayerTypes eHuman, AIDemandTypes eDemand);
	// </advc.104m>
	double AI_amortizationMultiplier(int iDelay) const; // advc.104, advc.031
	scaled AI_targetAmortizationTurns() const; // advc.ctr
	// advc.104r: Made public and param added
	void AI_doSplit(bool bForce = false);

	// for serialization
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

protected:
	int m_iPeaceWeight;
	int m_iEspionageWeight;
	int m_iAttackOddsChange;
	int m_iCivicTimer;
	int m_iReligionTimer;
	int m_iExtraGoldTarget;
	int m_iCityTargetTimer; // K-Mod
	bool m_bDangerFromSubmarines; // advc.651
	UWAI::Civ* m_pUWAI; // advc.104

	/*mutable int m_iStrategyHash;
	mutable int m_iStrategyHashCacheTurn;
	mutable int m_iAveragesCacheTurn;
	mutable int m_iAverageGreatPeopleMultiplier;
	mutable int *m_aiAverageYieldMultiplier;
	mutable int *m_aiAverageCommerceMultiplier;
	mutable int *m_aiAverageCommerceExchange;
	mutable int m_iUpgradeUnitsCacheTurn;
	mutable int m_iUpgradeUnitsCachedExpThreshold;
	mutable int m_iUpgradeUnitsCachedGold;*/ // BtS

	// K-Mod. The original caching method was just begging for OOS bugs.
	AIStrategy m_eStrategyHash;
	// BBAI variables (adjusted for K-Mod)
	unsigned m_iStrategyRand;
	AIVictoryStage m_eVictoryStageHash;
	// end BBAI

	int m_iAverageGreatPeopleMultiplier;

	int m_iAverageCulturePressure; // K-Mod culture pressure

	int *m_aiAverageYieldMultiplier;
	int *m_aiAverageCommerceMultiplier;
	int *m_aiAverageCommerceExchange;

	int m_iUpgradeUnitsCachedGold;
	int m_iAvailableIncome;
	// K-Mod end

	int *m_aiNumTrainAIUnits;
	int *m_aiNumAIUnits;
	int* m_aiSameReligionCounter;
	int* m_aiDifferentReligionCounter;
	int* m_aiFavoriteCivicCounter;
	int* m_aiBonusTradeCounter;
	int* m_aiPeacetimeTradeValue;
	int* m_aiPeacetimeGrantValue;
	int* m_aiGoldTradedTo;
	int* m_aiAttitudeExtra;
	// <advc.079>
	UnitTypes m_aeLastBrag[MAX_CIV_PLAYERS];
	TeamTypes m_aeLastWarn[MAX_CIV_PLAYERS]; // </advc.079>
	int* m_aiBonusValue;
	int* m_aiBonusValueTrade; // advc.036
	int* m_aiUnitClassWeights;
	int* m_aiUnitCombatWeights;
	// <advc.130c>
	bool m_abTheyFarAhead[MAX_CIV_PLAYERS];
	bool m_abTheyBarelyAhead[MAX_CIV_PLAYERS]; // </advc.130c>
	std::map<UnitClassTypes, int> m_GreatPersonWeights; // K-Mod
	std::map<int,int> m_neededExplorersByArea; // advc.opt
	static int const m_iSingleBonusTradeTolerance = 20; // advc.036
	//mutable int* m_aiCloseBordersAttitude;
	std::vector<int> m_aiCloseBordersAttitude; // K-Mod. (the original system was prone to mistakes.)
	std::vector<int> m_aiAttitude; // K-Mod

	bool* m_abFirstContact; // advc.003j: Now unused

	int** m_aaiContactTimer;
	int** m_aaiMemoryCount;

	std::vector<int> m_aiAICitySites;

	bool m_bWasFinancialTrouble;
	int m_iTurnLastProductionDirty;

	void AI_doCounter();
	void AI_doMilitary();
	void AI_doResearch();
	void AI_doCivics();
	void AI_doReligion();
	void AI_doDiplo();
	void AI_doCheckFinancialTrouble();
	/*  <advc> Overlaps with CvTeamAI::roundTradeVal. Could call that function,
		but don't want to include CvTeamAI.h here. (And want inlining.) */
	inline void AI_roundTradeVal(int& iTradeVal) const
	{
		iTradeVal -= iTradeVal % GC.getDefineINT(CvGlobals::DIPLOMACY_VALUE_REMAINDER);
	}
	bool AI_proposeJointWar(PlayerTypes eHuman); // </advc>
	// advc.130t:
	int AI_rivalPactAttitude(PlayerTypes ePlayer, bool bVassalPacts) const;
	double AI_expansionistHate(PlayerTypes ePlayer) const;
	bool AI_canBeAttackedBy(CvUnit const& u) const; // advc.315

	// <advc.130p>
	double AI_peacetimeTradeMultiplier(PlayerTypes eOtherPlayer,
			TeamTypes eOtherTeam = NO_TEAM) const;
	int AI_peacetimeTradeValDivisor(bool bRival) const;
	static int const PEACETIME_TRADE_RELATIONS_LIMIT = 4;
	/*  The change-value functions (now called 'process', e.g. AI_processPeacetimeValue)
		apply adjustments and have a side-effect on EnemyTrade and EnemyGrant values.
		These here are simple setters. */
	void AI_setPeacetimeTradeValue(PlayerTypes eIndex, int iVal);
	void AI_setPeacetimeGrantValue(PlayerTypes eIndex, int iVal);
	// </advc.130p>
	/*	advc.130x: Mode 0: same religion, 1: different religion,
		2: favorite civic. Returns the absolute value of the limit for the
		time-based religion/civics relations modifier. */
	int AI_ideologyDiploLimit(PlayerTypes eOther, int iMode) const;
	// advc.130r: Are they at war with a partner of ours?
	bool AI_atWarWithPartner(TeamTypes eOtherTeam,
			/*  advc.130h: If CheckPartnerAttacked==true, then only partners with
				war plan "attacked" or "attacked recent" count. */
			bool bCheckPartnerAttacked = false) const;
	// <advc.104h>
	int AI_negotiatePeace(PlayerTypes eRecipient, PlayerTypes eGiver, int iDelta,
			int* iGold, TechTypes* eBestTech, CvCity const** pBestCity); // </advc.104h>
	bool AI_counterPropose(PlayerTypes ePlayer, // advc: was public
			const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList,
			CLinkList<TradeData>* pTheirInventory, CLinkList<TradeData>* pOurInventory,
			CLinkList<TradeData>* pTheirCounter, CLinkList<TradeData>* pOurCounter,
			double leniency = 1) const; // advc.705
	// <advc>
	// Variant that writes the proposal into pTheirList and pOurList
	bool AI_counterPropose(PlayerTypes ePlayer, CLinkList<TradeData>& kTheyGive,
			CLinkList<TradeData>& kWeGive, bool bTheyMayGiveMore, bool bWeMayGiveMore,
			double generosity = 1) const;
	bool AI_balanceDeal(bool bGoldDeal, CLinkList<TradeData> const* pInventory,
			PlayerTypes ePlayer, int& iGreaterVal, int& iSmallerVal,
			CLinkList<TradeData>* pCounter,
			CLinkList<TradeData> const* pList,
			double leniency, // advc.705
			bool bGenerous,
			int iHappyLeft, int iHealthLeft, int iOtherListLength) const; // advc.036
	int AI_tradeValToGold(int iTradeVal, bool bOverpay, int iMaxGold = MAX_INT,
			bool* bEnough = NULL) const;
	// <advc.ctr>
	AttitudeTypes AI_cityTradeAttitudeThresh(CvCity const& kCity, PlayerTypes eToPlayer, bool bLiberate) const;
	static bool isLiberationTrade(PlayerTypes eFirst, PlayerTypes eSecond,
			CLinkList<TradeData> const& kFirstGives, CLinkList<TradeData> const& kSecondGives);
	// </advc.ctr>
	enum CancelCode { NO_CANCEL = -1, RENEGOTIATE, DO_CANCEL };
	CancelCode AI_checkCancel(CvDeal const& d, PlayerTypes ePlayer);
	bool AI_doDeals(PlayerTypes eOther);
	// </advc>
	bool AI_proposeResourceTrade(PlayerTypes eTo); // advc.133
	// advc.132:
	bool AI_checkCivicReligionConsistency(CLinkList<TradeData> const& tradeItems) const;
	// <advc.036>
	bool AI_checkResourceLimits(CLinkList<TradeData> const& kWeGive,
			CLinkList<TradeData> const& kTheyGive, PlayerTypes eThey,
			int iChange) const; // </advc.036>
	// <advc.026>
	int AI_maxGoldTradeGenerous(PlayerTypes eTo) const;
	int AI_maxGoldPerTurnTradeGenerous(PlayerTypes eTo) const;
	bool AI_checkMaxGold(CLinkList<TradeData> const& items, PlayerTypes eTo) const;
	// </advc.026>
	int AI_adjustTradeGoldToDiplo(int iGold, PlayerTypes eTo) const;
	void AI_foldDeals() const;
	void AI_foldDeals(CvDeal& d1, CvDeal& d2) const; // </advc.036>
	double AI_bonusImportValue(PlayerTypes eFrom) const; // advc.149
	int AI_anarchyTradeVal(CivicTypes eCivic = NO_CIVIC) const; // advc.132
	bool AI_isThreatFromMinorCiv() const; // advc.109
	void AI_updateDangerFromSubmarines(); // advc.651
	bool AI_cheatDangerVisibility(CvPlot const& kAt) const; // advc.128
	// <advc>
	int AI_countDangerousUnits(CvPlot const& kAttackerPlot, CvPlot const& kDefenderPlot,
			bool bTestMoves, int iLimit = MAX_INT,
			LowHPCounter* pLowHPCounter = NULL,
			PlayerTypes eAttackPlayer = NO_PLAYER) const; // </advc>
	int AI_knownRankDifference(PlayerTypes eOther) const; // advc.130c
	// advc.042: Relies on caller to reset GC.getBorderFinder()
	bool AI_isUnimprovedBonus(CvPlot const& p, CvPlot* pFromPlot, bool bCheckPath) const;
	void AI_updateCityAttitude(CvPlot const& kCityPlot); // advc.130w
	int AI_neededExplorers_bulk(CvArea const& kArea) const; // advc.opt
	// BETTER_BTS_AI_MOD, Victory Strategy AI, 03/17/10, jdog5000: START
	// (advc: moved here from the public section)
	int AI_calculateSpaceVictoryStage() const;
	int AI_calculateConquestVictoryStage() const;
	int AI_calculateDominationVictoryStage() const;
	int AI_calculateDiplomacyVictoryStage() const;
	// BETTER_BTS_AI_MOD: END
	// K-Mod. I've moved the bulk of AI_getStrategyHash into a new function: AI_updateStrategyHash.
	inline AIStrategy AI_getStrategyHash() const { return m_eStrategyHash; }
	void AI_updateStrategyHash();
	void AI_calculateAverages();

	void AI_convertUnitAITypesForCrush();
	int AI_eventValue(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const;

	void AI_doEnemyUnitData();
	//void AI_invalidateCloseBordersAttitude(); // disabled by K-Mod
	bool AI_isCommercePlot(CvPlot* pPlot) const; // advc: Was public; deprecated.
	void AI_setHumanDisabled(bool bDisabled); // advc.127
	void logFoundValue(int iX, int iY, bool bStartingLoc = false) const; // advc.031c

	friend class CvGameTextMgr;
	friend class CvPlayer; // advc.003u: So that protected functions can be called through CvPlayer::AI
};

// <advc.003u> (Counterparts to global functions in CvPlayer.h)
inline CvCityAI* AI_getCity(IDInfo city)
{
	FAssertBounds(0, MAX_PLAYERS, city.eOwner);
	return GET_PLAYER(city.eOwner).AI_getCity(city.iID);
}
inline CvUnitAI* AI_getUnit(IDInfo unit)
{
	FAssertBounds(0, MAX_PLAYERS, unit.eOwner);
	return GET_PLAYER(unit.eOwner).AI_getUnit(unit.iID);
} // </advc.003u>

#endif
