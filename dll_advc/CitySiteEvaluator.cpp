#include "CvGameCoreDLL.h"
#include "CitySiteEvaluator.h"
#include "CoreAI.h"
#include "CvCityAI.h"
#include "CvCivilization.h"
#include "PlotRange.h"
#include "CvArea.h"
#include "CvInfo_City.h"
#include "CvInfo_Terrain.h"
#include "CvInfo_GameOption.h"
#include "CvInfo_Civics.h"
#include "BBAILog.h"

// advc: New file; see comment in the header.

static int const iDEFAULT_BARB_DISCOURAGED_RANGE = 8; // advc.303

#define IFLOG if (gFoundLogLevel > 0 && AIFoundValue::isLoggingEnabled()) // advc.031c

// Body cut from K-Mod's CvPlayerAI::CvFoundSettings::CvFoundSettings
CitySiteEvaluator::CitySiteEvaluator(CvPlayerAI const& kPlayer, int iMinRivalRange,
	bool bStartingLoc) :
	m_kPlayer(kPlayer), m_iMinRivalRange(iMinRivalRange), m_bStartingLoc(bStartingLoc),
	m_bAmbitious(false), m_bFinancial(false), m_bDefensive(false),
	m_bSeafaring(false), m_bExpansive(false), /* advc.007: */ m_bDebug(false),
	m_bAdvancedStart(false), /* advc.303: */ m_iBarbDiscouragedRange(iDEFAULT_BARB_DISCOURAGED_RANGE)
{
	m_bAdvancedStart = (kPlayer.getAdvancedStartPoints() > 0);
	m_bEasyCulture = (kPlayer.getNumCities() <= 0 || // advc.108: from MNAI (was =bStartingLoc)
			m_bAdvancedStart); // advc.031
	m_bAllSeeing = (bStartingLoc || kPlayer.isBarbarian());

	CvLeaderHeadInfo const* pPersonality = NULL;
	CvCivilization const& kCiv = kPlayer.getCivilization();

	if (!m_bStartingLoc)
	{
		// advc.001: Make sure that personality isn't used for human or StartingLoc
		if (!kPlayer.isHuman() || m_bDebug)
			pPersonality = &GC.getInfo(kPlayer.getPersonalityType());
		FOR_EACH_ENUM(Trait)
		{
			if (!kPlayer.hasTrait(eLoopTrait))
				continue;

			CvTraitInfo const& kTrait = GC.getInfo(eLoopTrait);
			if (kTrait.getCommerceChange(COMMERCE_CULTURE) > 0)
			{
				m_bEasyCulture = true;
				if (pPersonality != NULL && pPersonality->getBasePeaceWeight() <= 5)
					m_bAmbitious = true;
			}
			if (kTrait.getExtraYieldThreshold(YIELD_COMMERCE) > 0)
				m_bFinancial = true;
			if (kTrait.isAnyFreePromotion()) // advc.003t
			{
				FOR_EACH_ENUM(Promotion)
				{
					if (kTrait.isFreePromotion(eLoopPromotion))
					{
						// aggressive, protective... it doesn't really matter to me.
						if (pPersonality != NULL && pPersonality->getBasePeaceWeight() >= 5)
							m_bDefensive = true;
					}
				}
			}
			if (pPersonality != NULL && pPersonality->getMaxWarRand() <= 150) // advc.opt: moved up
			{
				for (int i = 0; i < kCiv.getNumUnits(); i++)
				{
					UnitTypes eFoundUnit = kCiv.unitAt(i);
					if (GC.getInfo(eFoundUnit).isFound() &&
						GC.getInfo(eFoundUnit).getProductionTraits(eLoopTrait) &&
						kPlayer.canTrain(eFoundUnit))
					{
						m_bAmbitious = true;
					}
				}
			}
		}
		// seafaring test for unique unit and unique building
		if (kPlayer.getCoastalTradeRoutes() > 0)
			m_bSeafaring = true;
		if (!m_bSeafaring)
		{
			for (int i = 0; i < kCiv.getNumUniqueUnits(); i++) // advc.003w
			{
				UnitTypes eUniqueUnit = kCiv.uniqueUnitAt(i);
				if (GC.getInfo(eUniqueUnit).getDomainType() == DOMAIN_SEA)
				{
					m_bSeafaring = true;
					break;
				}
			}
		}
		if (!m_bSeafaring)
		{
			for (int i = 0; i < kCiv.getNumUniqueBuildings(); i++) // advc.003w
			{
				BuildingTypes eUniqueBuilding = kCiv.uniqueBuildingAt(i);
				if (GC.getInfo(eUniqueBuilding).isWater())
				{
					m_bSeafaring = true;
					break;
				}
			}
		}
		// Easy culture: culture process, free culture or easy artists
		if (!m_bEasyCulture)
		{
			FOR_EACH_ENUM(Process)
			{
				CvProcessInfo const& kProcess = GC.getInfo(eLoopProcess);
				if (GET_TEAM(kPlayer.getTeam()).isHasTech((TechTypes)kProcess.getTechPrereq()) &&
					kProcess.getProductionToCommerceModifier(COMMERCE_CULTURE) > 0)
				{
					m_bEasyCulture = true;
					break;
				}
			}
		}
		if (!m_bEasyCulture)
		{
			FOR_EACH_ENUM(Building)
			{
				if (kPlayer.isBuildingFree(eLoopBuilding) && GC.getInfo(eLoopBuilding).
					getObsoleteSafeCommerceChange(COMMERCE_CULTURE) > 0)
				{
					m_bEasyCulture = true;
					break;
				}
			}
		}
		if (!m_bEasyCulture)
		{
			FOR_EACH_ENUM(Specialist)
			{
				if (kPlayer.isSpecialistValid(eLoopSpecialist) &&
					kPlayer.specialistCommerce(eLoopSpecialist, COMMERCE_CULTURE) > 0)
				{
					m_bEasyCulture = true;
					break;
				}
			}
		}
	}

	if (pPersonality != NULL && kPlayer.AI_getFlavorValue(FLAVOR_GROWTH) > 0)
		m_bExpansive = true;

	m_iClaimThreshold = 100; // will be converted into culture units
	if (!bStartingLoc)
	{
		int iCitiesTarget = std::max(1,
				GC.getInfo(GC.getMap().getWorldSize()).getTargetNumCities());
		m_iClaimThreshold = 100 +
				100 * kPlayer.getCurrentEra() / std::max(1, GC.getNumEraInfos() - 1);
		m_iClaimThreshold += 80 * std::max(0,
				iCitiesTarget - kPlayer.getNumCities()) / iCitiesTarget;

		m_iClaimThreshold *= (!m_bEasyCulture ? 100 :
				(kPlayer.getCurrentEra() < 2 ? 200 : 150));
		m_iClaimThreshold *= (m_bAmbitious ? 150 : 100);
		m_iClaimThreshold /= 10000;
	}
	m_iClaimThreshold *= 2 * GC.getGame().getCultureThreshold((CultureLevelTypes)
			std::min(2, GC.getNumCultureLevelInfos() - 1));
	// note: plot culture is roughly 10x city culture (cf. CvCity::doPlotCultureTimes100)

	if (m_bAdvancedStart)
	{
		FAssert(GC.getGame().isOption(GAMEOPTION_ADVANCED_START));
		if (m_bStartingLoc)
			m_bAdvancedStart = false;
	}
	IFLOG logSettings();
}


short CitySiteEvaluator::evaluate(CvPlot const& kPlot) const
{
	AIFoundValue foundVal(kPlot, *this);
	return foundVal.get();
}


short CitySiteEvaluator::evaluate(int iX, int iY) const
{
	CvPlot const* pPlot = GC.getMap().plot(iX, iY);
	if (pPlot == NULL)
		return 0;
	return evaluate(*pPlot);
}

// <advc.300>
void CitySiteEvaluator::discourageBarbarians(int iRange)
{
	m_iBarbDiscouragedRange = iRange;
} // </advc.300>

// <advc.007>
void CitySiteEvaluator::setDebug(bool b)
{
	m_bDebug = b;
} // </advc.007>

// <advc.031c>
short CitySiteEvaluator::evaluateWithLogging(int iX, int iY) const
{
	AIFoundValue::setLoggingEnabled(true);
	int r = evaluate(iX, iY);
	AIFoundValue::setLoggingEnabled(false);
	return r;
}


void CitySiteEvaluator::log(int iX, int iY)
{
	/*  Important to ignore other city sites. Because, when CvPlayerAI::
		AI_updateCitySites computes the found value of the best site,
		none of the other sites are chosen yet. Here, all sites are chosen. */
	setDebug(true);
	if (isStartingLoc())
	{
		// Starting plot already assigned => normalizing
		if (getPlayer().getStartingPlot() != NULL &&
			getPlayer().getStartingPlot()->at(iX, iY))
		{
			logBBAI("\n\nNormalizing starting plot (%d,%d)", iX, iY);
		}
		// About to be assigned
		else logBBAI("\n\nStarting location found at (%d,%d)", iX, iY);
	}
	else
	{
		logBBAI("\n\n%S is about to found a city at (%d,%d); turn %d (year %d)", getPlayer().debugName(),
				iX, iY, GC.getGame().getGameTurn(), GC.getGame().getGameTurnYear());
		logBBAI("Lower bound for found value: %d", getPlayer().AI_getMinFoundValue());
	}
	evaluateWithLogging(iX, iY);
	if (isStartingLoc() || getPlayer().isBarbarian())
		return;
	{
		CvPlot const* pNextBestSite = NULL;
		int iBest = 0;
		for (int i = 0; i < getPlayer().AI_getNumCitySites(); i++)
		{
			CvPlot const& kLoopPlot = *getPlayer().AI_getCitySite(i);
			if (kLoopPlot.at(iX, iY))
				continue;
			int iValue = evaluate(kLoopPlot);
			if (iValue > iBest)
			{
				pNextBestSite = &kLoopPlot;
				iBest = iValue;
			}
		}
		if (pNextBestSite != NULL)
		{
			int iNextX = pNextBestSite->getX();
			int iNextY = pNextBestSite->getY();
			logBBAI("\nNext best site: %d (%d,%d)", iBest, iNextX, iNextY);
			evaluateWithLogging(iNextX, iNextY);
		}
	}
	{
		CvPlot const* pBestAdjSite = NULL;
		int iBest = 0;
		FOR_EACH_ENUM(Direction)
		{
			CvPlot const* pAdj = plotDirection(iX, iY, eLoopDirection);
			if (pAdj == NULL)
				continue;
			int iValue = evaluate(*pAdj);
			if (iValue > iBest)
			{
				pBestAdjSite = pAdj;
				iBest = iValue;
			}
		}
		if (pBestAdjSite != NULL)
		{
			int iAdjX = pBestAdjSite->getX();
			int iAdjY = pBestAdjSite->getY();
			logBBAI("\nBest site adjacent to (%d,%d): %d (%d,%d)", iX, iY, iBest, iAdjX, iAdjY);
			evaluateWithLogging(iAdjX, iAdjY);
		}
	}
}


bool AIFoundValue::bLoggingEnabled = false;


void AIFoundValue::setLoggingEnabled(bool b)
{
	bLoggingEnabled = b;
} // </advc.031c>

AIFoundValue::AIFoundValue(CvPlot const& kPlot, CitySiteEvaluator const& kSettings) :
	kPlot(kPlot), kArea(kPlot.getArea()), kSet(kSettings), kPlayer(kSet.getPlayer()),
	eTeam(kPlayer.getTeam()), ePlayer(kPlayer.getID()), kTeam(GET_TEAM(eTeam)),
	kGame(GC.getGame()), iX(kPlot.getX()), iY(kPlot.getY()), m_iResult(0)
{
	PROFILE_FUNC();
	if (!kPlayer.canFound(iX, iY))
		return;

	bBarbarian = kPlayer.isBarbarian();
	eEra = kPlayer.getCurrentEra();
	bCoastal = kPlot.isCoastalLand(-1);
	iAreaCities = kArea.getCitiesPerPlayer(ePlayer);
	pCapital = kPlayer.AI_getCapitalCity();
	// <advc.108> Barbarians shouldn't distinguish between earlier and later cities
	iCities = (bBarbarian ? 5 : kPlayer.getNumCities());
	FAssert(iCities > 0 || !kPlayer.isFoundedFirstCity());
	// </advc.108>
	/*  advc: New variable, BtS comment moved up. Originally, kPlot.isStartingPlot()
		was used; K-Mod corrected that to pPlot==getStartingPlot()
		("isStartingPlot is not automatically set"). */
	//"nice hacky way to avoid messing with normalizer"
	bNormalize = (kSet.isStartingLoc() && &kPlot == kPlayer.getStartingPlot());

	bFirstColony = false;
	iUnrevealedTiles = 0;

	m_iResult = evaluate();
}

/*  Body from CvPlayerAI::AI_foundValue_bulk, split into subroutines.
	More refactoring could be done; for one thing, some of the functions have
	long (return) parameter lists. Should probably introduce a class CityPlot
	for the data (and behavior) in the plot evaluation loop. At least it's
	no longer a single 1000-LoC function inside the largest GameCore class.
	K-Mod: Heavily edited (some changes marked, others not.)
	note, this function is called for every revealed plot for every player
	at the start of every turn. try to not make it too slow! */
short AIFoundValue::evaluate()
{
	IFLOG logSite();

	if (!isSiteValid() || !computeOverlap())
	{
		IFLOG logBBAI("Site disregarded");
		return 0;
	}

	// Bad tiles, unrevealed tiles, first colony
	int iBadTiles = 0;
	int iLandTiles = 0; // advc.031
	// <advc.040>
	bool bFirstColony = isPrioritizeAsFirstColony();
	IFLOG if(bFirstColony) logBBAI("First colony");
	// Scope for countBadTiles return parameters
	{
		int iRevealedDecentLand = 0;
		// </advc.040>
		int iUnrevealed = 0;
		iBadTiles = countBadTiles(iUnrevealed, iLandTiles, iRevealedDecentLand);
		iUnrevealedTiles = iUnrevealed;
		/*  <advc.040> Make sure we do naval exploration near the site before
			sending a Settler */
		if (iRevealedDecentLand < 4)
		{
			IFLOG if(bFirstColony) logBBAI("Only %d decent revealed land tiles; first-colony logic disabled.", iRevealedDecentLand);
			bFirstColony = false;
		} // </advc.040>
	}
	if (isTooManyBadTiles(iBadTiles))
	{
		IFLOG logBBAI("Too many bad tiles");
		return 0;
	}

	int iValue = baseCityValue();
	IFLOG logBBAI("Base city value: %d", iValue);

	IFLOG logBBAI("Evaluate city radius ...");
	std::vector<int> aiPlotValues(NUM_CITY_PLOTS, 0);

	std::vector<int> aiBonusCount(GC.getNumBonusInfos(), 0);
	int iResourceValue = 0;
	int aiSpecialYield[NUM_YIELD_TYPES] = {0, 0, 0}; // advc: Instead of individual variables
	/*  advc (comment): So that we can tell whether the city
		will be able to work its high-yield tiles */
	int iSpecialFoodPlus = 0;
	int iSpecialFoodMinus = 0;
	int iSpecialYieldTiles = 0; // advc.031: Number of tiles with special yield

	// advc: TeamMateTakenTiles code deleted (was dead code b/c of K-Mod changes)
	//int iYieldLostHere = 0; // advc: unused
	int iTakenTiles = 0;
	//bool bNeutralTerritory = true;
	bool bAnyForeignOwned = false; // advc.040: Replacing the above
	// <advc.031>
	bool bAnyGrowthBonus = false;
	int iStealPercent = 0;
	int iRiverTiles = 0;
	int iGreenTiles = 0;
	int iTotalFeatureProduction = 0; // </advc.031>
	int iHealth = 0;

	// K-Mod. (used to devalue cities which are unable to get any production.)
	int iBaseProductionTimes100 = 0; // (advc.031: Times100)

	FOR_EACH_ENUM(CityPlot)
	{
		// <advc.031>
		bool bShare = false;
		bool bSteal = false; // </advc.031>
		bool bCityRadius = false;
		bool bForeignOwned = false;
		if (!isUsablePlot(eLoopCityPlot, iTakenTiles, bCityRadius, bForeignOwned,
			bAnyForeignOwned, bShare, bSteal))
		{
			continue;
		}
		CvPlot const& p = *plotCity(iX, iY, eLoopCityPlot);
		bool const bHome = isHome(p);
		// advc.035: The own-exclusive-radius rule only helps if the radii don't overlap
		bool const bOwnExcl = (GC.getDefineBOOL(CvGlobals::OWN_EXCLUSIVE_RADIUS) &&
				!bCityRadius && bForeignOwned);

		bool bRemovableFeature = false; // K-Mod
		bool bPersistentFeature = false; // advc: was "eventuallyRemovable" in K-Mod
		int iFeatureProduction = 0; // advc.031
		FeatureTypes const eFeature = p.getFeatureType();
		bRemovableFeature = isRemovableFeature(p, bPersistentFeature,
					/* <advc.031> */ iFeatureProduction);
		bool bCanNeverImprove = false;
		if (eFeature != NO_FEATURE && GC.getInfo(eFeature).isNoImprovement())
			bCanNeverImprove = true;
		/*  (Non-resource water also can't be improved; that's already covered by
			K-Mod code in evaluateYield) </advc.031> */
		if (!bHome)
		{
			FAssert(eFeature != NO_FEATURE || (!bRemovableFeature && !bPersistentFeature));
			int const iFeatureHealth = (eFeature == NO_FEATURE ? 0 :
					GC.getInfo(eFeature).getHealthPercent());
			if (!bRemovableFeature || iFeatureHealth > 0)
				iHealth += iFeatureHealth;
/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: Enable Terrain Health Modifiers                                                  **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
			//keldath re write according to the feature above - by f1rpo
			//keldath QA
			TerrainTypes const eTerrain = p.getTerrainType();
			FAssert(eFeature != NO_FEATURE || (!bRemovableFeature && !bPersistentFeature));
			int const iTerrainHealth = (eFeature == NO_TERRAIN ? 0 :
					GC.getInfo(eTerrain).getHealthPercent());
				iHealth += iTerrainHealth;	
/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/
		}
		BonusTypes const eBonus = getBonus(p);
		// <advc.031> (This was much coarser in K-Mod)
		bool bCanTradeBonus = false;
		bool bCanSoonTradeBonus = false;
		bool bCanImproveBonus = false;
		bool bCanSoonImproveBonus = false;
		int aiBonusImprovementYield[NUM_YIELD_TYPES] = {0, 0, 0};
		ImprovementTypes eBonusImprovement = NO_IMPROVEMENT;
		{
			bool bImprovementRemovesFeature = false;
			eBonusImprovement = getBonusImprovement(eBonus, p,
					bCanTradeBonus, bCanSoonTradeBonus, aiBonusImprovementYield,
					bCanImproveBonus, bCanSoonImproveBonus, bImprovementRemovesFeature);
			if (!bImprovementRemovesFeature && eBonusImprovement != NO_IMPROVEMENT)
				iFeatureProduction = 0;
		} // </advc.031>

		int iCultureModifier = 100;
		if (!bHome) // advc.031: Shouldn't matter on home plot
		{
			iCultureModifier = calculateCultureModifier(
					p, bForeignOwned, bShare, bCityRadius, bSteal,
					abFlip[eLoopCityPlot], bOwnExcl, iTakenTiles, iStealPercent);
			FAssertBounds(0, 101, iCultureModifier);
		}  // <advc.031>
		iFeatureProduction = (iFeatureProduction * iCultureModifier) / 100;
		iTotalFeatureProduction += iFeatureProduction; // </advc.031>
		// K-Mod note. This used to be called iTempValue.
		int iPlotValue = 0;
		/*  advc: Renamed from aiYield for clarity. Improvement yields are only predicted
			for resource plots (aiSpecialYield). */
		int aiNatureYield[NUM_YIELD_TYPES];
		FOR_EACH_ENUM(Yield)
		{
			aiNatureYield[eLoopYield] = //p->getYield(eLoopYield);  // K-Mod replacement:
					p.calculateNatureYield(eLoopYield,
					/*	advc.031: NO_TEAM makes calculateNatureYield ignore the resource
						(whereas CvPlot::getBonusType treats NO_TEAM as all-revealing) */
					eBonus == NO_BONUS ? NO_TEAM : eTeam,
					!bPersistentFeature || bHome); // advc.031: City removes all features
			// <advc.031> Replacing deleted K-Mod code
			if (bHome)
			{
				aiNatureYield[eLoopYield] += p.calculateCityPlotYieldChange(
						eLoopYield, aiNatureYield[eLoopYield], 1);
			} // </advc.031>
		}
		if (bHome)
		{
			iBaseProductionTimes100 += 100 * aiNatureYield[YIELD_PRODUCTION];
			iSpecialFoodPlus += std::max(0, aiNatureYield[YIELD_FOOD] -
					GC.getFOOD_CONSUMPTION_PER_POPULATION());
		}
		else
		{
			FOR_EACH_ENUM(Yield)
			{
				// K-Mod (start): adjust for removable features
				if (eFeature != NO_FEATURE && !bPersistentFeature)
				{
					CvFeatureInfo const& kFeature = GC.getInfo(eFeature);
					if (bRemovableFeature)
						iPlotValue += 10 * kFeature.getYieldChange(eLoopYield);
					else if (kFeature.getYieldChange(eLoopYield) < 0)
					{
						iPlotValue -= (eBonus != NO_BONUS ? 25 : 5);
						// advc.031: was 30 *...
						iPlotValue += 25 * kFeature.getYieldChange(eLoopYield);
					}
				} // K-Mod end
				// <advc.031>
				if (kPlayer.getExtraYieldThreshold(eLoopYield) > 0 &&
					aiNatureYield[eLoopYield] >= kPlayer.getExtraYieldThreshold(eLoopYield))
				{
					aiNatureYield[eLoopYield] += GC.getDefineINT(CvGlobals::EXTRA_YIELD);
				} // </advc.031>
			}
			// K-Mod. add non-home plot production to BaseProduction.
			if (!bShare)
				iBaseProductionTimes100 += 100 * aiNatureYield[YIELD_PRODUCTION];
			if (!bSteal)
			{
				iBaseProductionTimes100 += estimateImprovementProduction(
						p, bPersistentFeature);
			}
			if (!p.isWater())
			{
				// freshwater health (home plot) will be counted later
				iPlotValue += evaluateFreshWater(p, aiNatureYield, bSteal,
						iRiverTiles, iGreenTiles);
			}
		}
		iPlotValue += evaluateYield(aiNatureYield, &p, bCanNeverImprove); // (K-Mod: iTempValue in BtS)
		if (bHome)
		{
			// <advc.031> Count home plot yield twice b/c it's immediately available
			iPlotValue *= 2;
			iValue += iPlotValue;
			if (eBonus != NO_BONUS)
			{
				// Replacing K-Mod code that had adjusted the home plot yield
				iValue += foundOnResourceValue( // (result normally negative)
						eBonusImprovement == NO_IMPROVEMENT ? NULL : aiBonusImprovementYield);
			} // </advc.031>
		}
		else
		{
			iPlotValue = applyCultureModifier(p, iPlotValue, iCultureModifier, bShare);
			aiPlotValues[eLoopCityPlot] = iPlotValue; // Sum these up later
		}
		bool const bEasyAccess = /* K-Mod (!!): */ ((p.isWater() && (bCoastal ||
				p.getArea().getCitiesPerPlayer(ePlayer, true) > 0)) || // advc.031
				p.sameArea(kPlot) || p.getArea().getCitiesPerPlayer(ePlayer) > 0);
		IFLOG logPlot(p, iPlotValue, aiNatureYield, iCultureModifier, eBonus, eBonusImprovement,
				bCanTradeBonus, bCanSoonTradeBonus, bCanImproveBonus, bCanSoonImproveBonus,
				bEasyAccess, iFeatureProduction, bPersistentFeature, bRemovableFeature);
		// <advc.031>
		if (bShare)
		{
			IFLOG if(eBonus!=NO_BONUS) logBBAI("Resource ignored (shared tile)");
			continue;
		}
		// Was 33 flat; the modifier is now computed differently.
		if (iCultureModifier <= (bSteal ? 40 : 20) &&// </advc.031>
			!bOwnExcl) // advc.035
		{
			IFLOG if(eBonus!=NO_BONUS) logBBAI("Resource ignored (unlikely to flip)");
			continue;
		}
		if (eBonus != NO_BONUS) // advc.040: Same-area checks moved into nonYieldBonusValue
		{
			if (!bBarbarian && // advc.303: Barbarians don't care about resource trade
				// advc.031: Otherwise we can already trade the resource
				p.getOwner() != ePlayer)
			{
				int iBonusValue = nonYieldBonusValue(p, eBonus,
						bCanTradeBonus, bCanSoonTradeBonus, bEasyAccess,
						bAnyGrowthBonus, aiBonusCount, iCultureModifier);
				IFLOG if(iBonusValue!=0) logBBAI("+%d non-yield resource value (%S)",
						iBonusValue, GC.getInfo(eBonus).getDescription());
				//iValue += (iBonusValue + 10);
				iResourceValue += iBonusValue; // K-Mod
			}
			/*if (p.isWater())
				iValue += (bCoastal ? 0 : -800);*/ // (was ? 100 : -800)
			// <advc.031> Replacing the above
			if (p.isWater() && !bCoastal)
			{
				int const iPenalty = 165;
				IFLOG logBBAI("-%d from water resource near non-coastal site", iPenalty);
				iValue -= iPenalty;
			} // </advc.031>
		}
		if (!bHome) // (Home plot was handled upfront)
		{
			int iEffectiveFood = aiNatureYield[YIELD_FOOD];
			// K-Mod: lighthouse kludge
			if (bCoastal && bEasyAccess &&p.isWater() &&
				aiNatureYield[YIELD_COMMERCE] > 1)
			{
				iEffectiveFood++;
			}
			int iSpecialYieldModifier = iCultureModifier;
			// </advc.040>
			if (!bEasyAccess)
				iSpecialYieldModifier /= 2; // <advc.040>  <advc.031>
			if (eBonus != NO_BONUS)
			{
				if (!bCanSoonImproveBonus)
					iSpecialYieldModifier = (iSpecialYieldModifier * 3) / 10;
				else if (!bCanImproveBonus)
					iSpecialYieldModifier = (iSpecialYieldModifier * 3) / 4;
			} // </advc.031>
			calculateSpecialYields(p,
					eBonusImprovement == NO_IMPROVEMENT ? NULL : aiBonusImprovementYield,
					aiNatureYield, iSpecialYieldModifier, iEffectiveFood,
					aiSpecialYield, iSpecialFoodPlus, iSpecialFoodMinus, iSpecialYieldTiles);
		}
	}

	iValue += sumUpPlotValues(aiPlotValues);
	// A sensible order (CITY_HOME_PLOT first) isn't guaranteed anymore, hence:
	aiPlotValues.clear();
	// advc.031: Preserve this for later
	int iNonYieldResourceVal = std::max(0, iResourceValue);
	if (iSpecialYieldTiles > 0) // advc.031
	{
		iResourceValue += evaluateSpecialYields(aiSpecialYield, iSpecialYieldTiles,
				iSpecialFoodPlus, iSpecialFoodMinus);
	}
	if (isTooManyTakenTiles(iTakenTiles, iResourceValue, iValue < 780))
	{
		IFLOG logBBAI("Too many taken tiles (%d)", iTakenTiles);
		return 0;
	}
	iValue += std::max(0, iResourceValue);

	iValue += evaluateLongTermHealth(iHealth); // (may increase iHealth)
	iValue += evaluateFeatureProduction(iTotalFeatureProduction); // advc.031

	if (bCoastal)
	{
		iValue += evaluateSeaAccess(
				// advc: Same conditions as in BtS ...
				kArea.getCitiesPerPlayer(ePlayer) == 0 &&
				!bAnyForeignOwned && iResourceValue > 0 && kSet.isSeafaring(),
				// <advc.031>
				0.7 * ::dRange(
				std::min(iBaseProductionTimes100/100.0 + aiSpecialYield[YIELD_PRODUCTION],
				// Probably can't work high-production tiles when there is no food
				4.0 * (1 + iGreenTiles + iSpecialFoodPlus)) / 18.0, 0.1, 2.0),
				iLandTiles); // </advc.031>
	}
	iValue += evaluateDefense();
	IFLOG logBBAI("=%d in total before modifiers", iValue);

	if (!kSet.isStartingLoc())
	{
		/*  <advc.031> Moved down to the other modifiers. But don't adjust the
			non-yield resource value -- don't need food for a trade connection. */
		iValue = adjustToFood(iValue - iNonYieldResourceVal, iSpecialFoodPlus,
				iSpecialFoodMinus, iGreenTiles) + iNonYieldResourceVal;
		// </advc.031>
	}

	iBaseProductionTimes100 += 100 * aiSpecialYield[YIELD_PRODUCTION]; // K-Mod
	iValue = adjustToProduction(iValue, iBaseProductionTimes100);

	// <advc.031>
	if (iResourceValue <= 0 && iSpecialFoodPlus <= 0 &&
		iUnrevealedTiles < 5) // advc.040
	{
		int iMultiplier = 60;
		if (iRiverTiles >= 4)
			std::min(100, iMultiplier + 6 * iRiverTiles);
		iValue = ::round((iMultiplier * iValue) / 100.0);
		IFLOG logBBAI("Times %d percent because the site offers nothing special", iMultiplier);
	} // </advc.031>
	/*  advc.108: Obsoletion check added. Probably better not to let players start on
		a hidden resource; i.e. don't check this->getBonus(kPlot) != NO_BONUS. */
	if (kPlot.getNonObsoleteBonusType(eTeam) != NO_BONUS)
	{
		int iModifier = 100;
		if (kSet.isStartingLoc())
			iModifier = 50;
		else // advc.031: I don't think both penalties should be applied
			if (kSet.isAdvancedStart())
		{
			iModifier = 70;
		}
		iValue *= iModifier;
		iValue /= 100;
		IFLOG if(iModifier!=100) logBBAI("Times %d percent for starting on a resource", iModifier);
	}
	if (kSet.isStartingLoc())
		iValue = adjustToStartingSurroundings(iValue); // (advc: Moved down a bit)
	if (bBarbarian)
		iValue = adjustToBarbarianSurroundings(iValue);
	else if (!kSet.isStartingLoc())
		iValue = adjustToCivSurroundings(iValue, iStealPercent);

	if (iValue <= 0)
		return 1;

	iValue = adjustToCitiesPerArea(iValue);

	if (!kSet.isStartingLoc())
		iValue = adjustToBonusCount(iValue, aiBonusCount);

	iValue = adjustToBadTiles(iValue, iBadTiles /* advc.031: */ + (4 * iTakenTiles) / 10
			-(bBarbarian ? 2 : (4 + iGreenTiles + iSpecialFoodPlus))); // advc.303
	iValue = adjustToBadHealth(iValue, iHealth);

	// advc: BtS code (iDifferentAreaTile) deleted
	// (disabled by K-Mod. This kind of stuff is already taken into account.)

	FAssert(iValue >= 0);
	IFLOG logBBAI("Bottom line (found-city value): %d\n", iValue);
	return std::max<short>(1, ::intToShort(iValue));
}


bool AIFoundValue::isSiteValid() const
{
	//if (!kSet.isStartingLoc() && !kSet.isAdvancedStart())
	if(iCities > 0 && !bBarbarian) // advc.108
	{
		if (!bCoastal && iAreaCities == 0)
		{
			IFLOG logBBAI("First colony in area must be coastal");
			return false;
		}
	}

	int iMinRivalRange = kSet.getMinRivalRange();
	if (iMinRivalRange != -1)
	{
		for (SquareIter it(kPlot, iMinRivalRange); it.hasNext(); ++it)
		{
			if (it->plotCheck(PUF_isOtherTeam, ePlayer) != NULL)
			{
				IFLOG logBBAI("Rival plot (%d,%d) found in MinRivalRange", it->getX(), it->getY());
				return false;
			}
		}
	}
	if (kSet.isStartingLoc())
	{
		if (kPlot.isGoody())
		{
			IFLOG logBBAI("Can't start on goody hut");
			return false;
		}
		FOR_EACH_ENUM(CityPlot)
		{
			CvPlot const* p = plotCity(iX, iY, eLoopCityPlot);
			if (p == NULL)
			{
				IFLOG logBBAI("Can't start near the edge of a flat map");
				return false;
			}
		}
	}
	else // advc.031: Not relevant for StartingLoc
	{
		int iOwnedTiles = 0;
		FOR_EACH_ENUM(CityPlot)
		{
			CvPlot const* p = plotCity(iX, iY, eLoopCityPlot);
			if (p == NULL)
				iOwnedTiles += 2;
			else if (p->isOwned() && p->getTeam() != eTeam)
			{
				/*  advc.035 (comment): Would be good to check abFlip[i] here,
					but computeOverlap is a little costly. */
				iOwnedTiles++;
				/*  <advc.031> Count tiles only half if they're in our inner ring
					and can't be worked by any foreign city. */
				if (!adjacentOrSame(*p, kPlot) || p->isCityRadius())
					iOwnedTiles++; // </advc.031>
			}
		}
		//if(iOwnedTiles > NUM_CITY_PLOTS / 3)
		/*  <advc.031> Most owned tiles are counted twice now, and I want sth.
			closer to half of the tiles being owned. */
		int const iMaxOwnedTimes100 = 82 * NUM_CITY_PLOTS;
		if (100 * iOwnedTiles > iMaxOwnedTimes100) // </advc.031>
		{
			IFLOG logBBAI("%d tiles owned by other teams; allowed (times 100): %d", iOwnedTiles, iMaxOwnedTimes100);
			return false;
		}
	}
	return true;
}

// Returns false when there is too much overlap
bool AIFoundValue::computeOverlap()
{
	/*	K-Mod explanation of city site adjustment:
		Any plot which is otherwise within the radius of a city site is basically
		treated as if it's within an existing city radius */
	/*  advc.031: Changed to int in order to store the site id
		(only used for logging currently though) */
	aiCitySiteRadius.resize(NUM_CITY_PLOTS, -1);
	/*  <advc.035>
		Need to distinguish tiles within the radius of one of our team's cities
		from those within just any city radius. */
	abOwnCityRadius.resize(NUM_CITY_PLOTS, false);
	// Whether the tile flips to us once we settle near it
	abFlip.resize(NUM_CITY_PLOTS, false); // </advc.035>
	// K-Mod. bug fixes etc. (original code deleted)
	if (!kSet.isStartingLoc() &&
		!kSet.isDebug() && // advc.007
		!bBarbarian) // advc.303: Barbarians don't have city sites
	{
		for (int iSite = 0; iSite < kPlayer.AI_getNumCitySites(); iSite++)
		{
			CvPlot const* pCitySitePlot = kPlayer.AI_getCitySite(iSite);
			if (pCitySitePlot == &kPlot)
				continue;
			FAssert(pCitySitePlot != NULL);
			if (plotDistance(&kPlot, pCitySitePlot) <=
				GC.getDefineINT(CvGlobals::MIN_CITY_RANGE) &&
				pCitySitePlot->sameArea(kPlot))
			{
				IFLOG logBBAI("Too close to one of the sites we've already chosen");
				return false;
			}
			for (CityPlotIter it(kPlot); it.hasNext(); ++it)
			{
				if (plotDistance(&(*it), pCitySitePlot) <= CITY_PLOTS_RADIUS)
				{
					//Plot is inside the radius of a city site
					aiCitySiteRadius[it.currID()] = iSite;
				}
			}
		}
	} // K-Mod (bugfixes etc.) end
	// <advc.035> (also for advc.031)
	if (!kSet.isStartingLoc() && !bBarbarian)
	{
		FOR_EACH_CITY(c, kPlayer)
		{
			FOR_EACH_ENUM(CityPlot)
			{
				CvPlot const* p = plotCity(iX, iY, eLoopCityPlot);
				if(p != NULL && plotDistance(p->getX(), p->getY(),
						c->getX(), c->getY()) <= CITY_PLOTS_RADIUS)
					abOwnCityRadius[eLoopCityPlot] = true;
			}
		}
		if (GC.getDefineBOOL(CvGlobals::OWN_EXCLUSIVE_RADIUS))
		{
			FOR_EACH_ENUM(CityPlot)
			{
				CvPlot const* p = plotCity(iX, iY, eLoopCityPlot);
				abFlip[eLoopCityPlot] = (!abOwnCityRadius[eLoopCityPlot] &&
						p != NULL && p->isOwned() &&
						p->isCityRadius() && p->getTeam() != eTeam &&
						(p->getSecondOwner() == ePlayer ||
						/*  The above is enough b/c the tile may not be within the
							culture range of one of our cities; but it will be once
							the new city is founded. */
						p->findHighestCulturePlayer(true) == ePlayer));
			}
		} // </advc.035>
	}
	return true;
}

// <advc.040>
bool AIFoundValue::isPrioritizeAsFirstColony() const
{

	return (bCoastal && iAreaCities <= 0 && !bBarbarian && iCities > 0 &&
			(kPlot.isFreshWater() ||
			/*  Don't prioritize colonies in tundra, snow and desert b/c
				these are likely surrounded by more (unrevealed) bad terrain. */
			kPlot.calculateNatureYield(YIELD_FOOD, eTeam, true) +
			kPlot.calculateNatureYield(YIELD_PRODUCTION, eTeam, true) > 1));
} // </advc.040>


int AIFoundValue::countBadTiles(int& iUnrevealed, /* advc.031: */ int& iLand,
	int& iRevealedDecentLand) const // advc.040
{
	int iBadTiles = 0;
	FOR_EACH_ENUM(CityPlot)
	{
		CvPlot const* p = plotCity(iX, iY, eLoopCityPlot);
		/*  <advc.031> NULL and impassable count as 2 bad tiles (as in BtS),
			but don't cheat with unrevealed tiles. */
		if (p == NULL)
		{
			iBadTiles += 2;
			continue;
		}
		if (!isRevealed(*p))
		{
			iUnrevealed++; // advc.040
			continue;
		}
		if (isHome(*p))
			continue;
		// <advc.303>
		if (bBarbarian && !adjacentOrSame(*p, kPlot))
		{
			/*  Rational Barbarians wouldn't mind settling one off the coast,
				but human players do mind, and some really hate this.
				Therefore, count the outer ring coast as bad if !bCoastal. */
			if(p != NULL && p->isWater() && !bCoastal &&
					p->calculateBestNatureYield(YIELD_FOOD, eTeam) <= 1)
				iBadTiles++;
			continue;
		} // </advc.303>

		if (!p->isWater())
			iLand++;

		if (p->isImpassable())
		{
			iBadTiles += 2;
		} // </advc.031>
		// K-Mod (original code deleted)
		else if (//!p->isFreshWater() &&
		/*  advc.031: Flood plains have high nature yield anyway,
			so this is about snow.
			Bonus check added (Incense). */
			getBonus(*p) == NO_BONUS &&
			(p->calculateBestNatureYield(YIELD_FOOD, eTeam) == 0 ||
			p->calculateTotalBestNatureYield(eTeam) <= 1))
		{
			/*  advc.031: Snow hills will count for BaseProduction, but, generally
				they're really bad. */
			iBadTiles += 2;
		}
		else if (p->isWater() && p->calculateBestNatureYield(YIELD_FOOD, eTeam) <= 1)
		{	/*  <advc.031> Removed the bCoastal check from the
				condition above b/c I want to count ocean tiles as
				half bad even when the city is at the coast. */
			if(!kSet.isSeafaring() &&
					p->calculateBestNatureYield(YIELD_COMMERCE, eTeam) <= 1)
				iBadTiles++;
			if(!bCoastal)
				iBadTiles++; // </advc.031>
		}
		else if (p->isOwned())
		{
			if(!abFlip[eLoopCityPlot]) // advc.035
			{
				if (p->getTeam() != eTeam || p->isBeingWorked())
					iBadTiles++;
				/*  (K-Mod) note: this final condition is...
					not something I intend to keep permanently. */
				// advc.031: poof
				/*else if (p->isCityRadius() || aiCitySiteRadius[iI] >= 0)
					iBadTiles++;*/
			} // advc.035
		}
		else iRevealedDecentLand++; // advc.040
	}
	iBadTiles /= 2;
	IFLOG (iUnrevealed > 0 ? logBBAI("Bad tiles: %d known bad, %d unrevealed", iBadTiles, iUnrevealedTiles) :
							 logBBAI("Bad tiles: %d", iBadTiles));
	return iBadTiles;
}

// Returns true if the site should be disregarded
bool AIFoundValue::isTooManyBadTiles(int iBadTiles) const
{
	if (kSet.isStartingLoc())
		return false;
	if (2 * iBadTiles <= NUM_CITY_PLOTS && kArea.getNumTiles() > 2 &&
			(!bBarbarian || iBadTiles <= 3)) // advc.303
		return false;

	bool bHasGoodBonus = false;
	int iMediocreBonuses = 0; // advc.031
	int iFreshWaterTiles = (kPlot.isFreshWater() ? 1 : 0); // advc.031 (count kPlot twice)
	for (CityPlotIter it(kPlot); it.hasNext(); ++it)
	{
		CvPlot const& p = *it;
		if(!isRevealed(p))
			continue;
		// <advc.303>
		if(bBarbarian && !adjacentOrSame(p, kPlot))
			continue; // </advc.303>
		if(p.isOwned() &&
			// <advc.031>
			(p.getOwner() != ePlayer ||
			p.getCityRadiusCount() > 0)) // </advc.031>
		{
			continue;
		}
		BonusTypes eBonus = getBonus(p); // advc.108
		if (eBonus != NO_BONUS &&
			!kTeam.isBonusObsolete(eBonus)) // K-Mod
		{
			if ((kPlayer.getNumTradeableBonuses(eBonus) == 0 ||
				kPlayer.AI_bonusVal(eBonus, 1, true) > 10 ||
				GC.getInfo(eBonus).getYieldChange(YIELD_FOOD) > 0) &&
				// <advc.031> Moved from above
				(p.isWater() || p.isArea(kArea) ||
				p.getArea().getCitiesPerPlayer(ePlayer) > 0)) // </advc.031>
			{
				bHasGoodBonus = true;
				break;
			} // <advc.031>
			else iMediocreBonuses++;
		}
		if(p.isFreshWater())
			iFreshWaterTiles++; // </advc.031>
	}

	return (!bHasGoodBonus && /* advc.040: */ !bFirstColony &&
			iFreshWaterTiles < 3 && iMediocreBonuses < 2); // advc.031
}


int AIFoundValue::baseCityValue() const
{
	// advc.031: was 800 in K-Mod and 1000 before K-Mod
	int r = 150;
	// <advc.040>
	if (bFirstColony && iUnrevealedTiles > 0)
	{
		int const iFirstColonyValue = 55 * std::min(5, iUnrevealedTiles);
		IFLOG logBBAI("+%d base value for unrevealed tiles near first colony", iFirstColonyValue);
		r += iFirstColonyValue;
	} // </advc.040>
	// <advc.108>
	else if(iCities <= 0)
	{
		int const iCapitalValue = ::round(50 * std::sqrt((double)iUnrevealedTiles));
		IFLOG if(iCapitalValue>0) logBBAI("+%d base value for unrevealed tiles near initial city", iCapitalValue);
		r += iCapitalValue;
	} // </advc.108>
	return r;
}


bool AIFoundValue::isUsablePlot(CityPlotTypes ePlot, int& iTakenTiles, bool& bCityRadius,
	bool& bForeignOwned, bool& bAnyForeignOwned, bool& bShare, bool& bSteal) const
{
	CvPlot const* p = plotCity(iX, iY, ePlot);
	if (p == NULL)
	{
		iTakenTiles++;
		return false;
	}
	bool const bInnerRing = adjacentOrSame(*p, kPlot);
	// <advc.303>
	if (bBarbarian && !bInnerRing)
		return false; // </advc.303>
	/*	advc.031: Moved up. If we can't see the tile, we don't really know if it's
		already taken by a rival. (Will find out when our Settler gets there.) */
	if (!isRevealed(*p))
	{
		IFLOG logBBAI("Unrevealed plot skipped: (%d,%d)", p->getX(), p->getY());
		return false;
	}
	if (isHome(*p))
		return true;
	// <advc.035>
	if (abFlip[ePlot])
	{
		IFLOG logBBAI("Assumed to flip: %S", p->debugStr());
		return true;
	}
	// </advc.035>
	// <advc.031>
	bCityRadius = p->isCityRadius();
	PlayerTypes const eOwner = p->getOwner();
	bForeignOwned = (eOwner != NO_PLAYER && eOwner != ePlayer);
	bSteal = (bCityRadius && bForeignOwned);
	 // </advc.031>
	if (bCityRadius || aiCitySiteRadius[ePlot] >= 0)
	{
		iTakenTiles++;
		// <advc.040>
		if(bForeignOwned)
			bAnyForeignOwned = true; // </advc.040>
		//return false;
	}
	/*  K-Mod Note: it kind of sucks that no value is counted for taken tiles.
		Tile sharing / stealing should be allowed. */
	// <advc.031> ^Exactly
	/*  Still don't allow tiles to be shared between (planned) city sites
		-- too difficult to estimate how many tiles each site will need. */
	if (aiCitySiteRadius[ePlot] >= 0)
	{
		IFLOG logBBAI("%S reserved for higher-priority site at (%d,%d)", p->debugStr(),
				kPlayer.AI_getCitySite(aiCitySiteRadius[ePlot])->getX(), kPlayer.AI_getCitySite(aiCitySiteRadius[ePlot])->getY());
		return false;
	}
	bool bOtherInnerRing = false;
	CvCityAI const* pOtherCity = NULL;
	if (bCityRadius)
	{
		if (bBarbarian)
		{
			IFLOG logBBAI("(%d,%d) is in the radius of another city", p->getX(), p->getY());
			return false;
		}
		pOtherCity = p->AI_getWorkingCity();
		if (pOtherCity == NULL && abOwnCityRadius[ePlot])
		{
			IFLOG logBBAI("(%d,%d) is in the radius of a %S city whose borders haven't expanded yet",
					p->getX(), p->getY(), kPlayer.debugCivDescr());
			/*  Difficult to judge whether tile sharing makes sense;
				better wait for borders to expand. */
			return false;
		}
		if (pOtherCity != NULL && (kTeam.AI_deduceCitySite(pOtherCity) ||
			/*  At the start of the game, a single revealed tile should
				be enough to locate the city. */
			iCities == 0))
		{
			bOtherInnerRing = adjacentOrSame(*p, *pOtherCity->plot());
			FAssert(!bInnerRing || !bOtherInnerRing || !pOtherCity->isArea(kArea));
			if (bForeignOwned && (bOtherInnerRing ||
				// Don't try to overlap with team member or master
				TEAMID(eOwner) == eTeam ||
				kTeam.isVassal(TEAMID(eOwner))))
			{
				IFLOG logBBAI("Don't count on stealing %S from %S", p->debugStr(), cityName(*pOtherCity));
				return false;
			}
			if (!bForeignOwned)
				bShare = true;
			// Else let iCultureModifier handle bSteal
		}
	}
	if (!bShare)
		return true;

	if(p->isBeingWorked() || bOtherInnerRing)
	{
		IFLOG logBBAI("Don't want to take (%d,%d) away from %S", p->getX(), p->getY(), cityName(*pOtherCity));
		return false;
	}
	CvPlot const* pOtherPlot = pOtherCity->plot();
	CityPlotTypes const eOtherPlotIndex = pOtherCity->getCityPlotIndex(p);
	if (GC.getCityPlotPriority()[ePlot] >= GC.getCityPlotPriority()[eOtherPlotIndex])
	{
		IFLOG logBBAI("%S has higher priority for (%d,%d)", cityName(*pOtherCity), p->getX(), p->getY());
		return false;
	}
	// Check if the other city is going to need the tile in the medium term
	if (pOtherCity->AI_isGoodPlot(eOtherPlotIndex) &&
		pOtherCity->AI_countGoodPlots() -
		pOtherCity->getPopulation() +
		pOtherCity->getSpecialistPopulation() <= 3)
	{
		IFLOG logBBAI("Don't want to take (%d,%d); %S might need it soon", p->getX(), p->getY(), cityName(*pOtherCity));
		return false;
	}
	// Else let the caller deal with bShare
	/*  (No special treatment for bShare&&bSteal; we'll
		probably win the tile if we have two cities near it.) */
	return true;
	// </advc.031>
}

// based on K-Mod code
bool AIFoundValue::isRemovableFeature(CvPlot const& p, bool& bPersistent,
	int& iFeatureProduction) const // advc.031
{
	bPersistent = false;
	iFeatureProduction = 0;
	// <advc.031>
	if (isHome(p))
		return true; // </advc.031>
	FeatureTypes const eFeature = p.getFeatureType();
	if (eFeature == NO_FEATURE)
		return false;

	CvFeatureInfo const& kFeature = GC.getInfo(eFeature);
	bPersistent = true;
	FOR_EACH_ENUM(Build)
	{
		CvBuildInfo const& kBuild = GC.getInfo(eLoopBuild);
		if (!kBuild.isFeatureRemove(eFeature))
			continue;

		bPersistent = false;
		// <advc.031>
		CvTerrainInfo const& kTerrain = GC.getInfo(p.getTerrainType());
		int iTerrainYieldSum = 0;
		int iFeatureYieldSum = 0;
		FOR_EACH_ENUM(Yield)
		{
			iTerrainYieldSum += kTerrain.getYield(eLoopYield);
			iFeatureYieldSum += kFeature.getYieldChange(eLoopYield);
		}
		// Count feature production only if the feature is (presumably) worth removing
		if (iTerrainYieldSum > iFeatureYieldSum)
		{
			CvCity* pDummy;
			iFeatureProduction = p.getFeatureProduction(eLoopBuild, eTeam, &pDummy, &kPlot, ePlayer);
			if (p.getTeam() == eTeam)
				iFeatureProduction /= 3; // Can already chop it
		}
		// CurrentResearch should be good enough
		TechTypes eTech1 = kBuild.getTechPrereq();
		TechTypes eTech2 = kBuild.getFeatureTech(eFeature);
		// </advc.031>
		if (kTeam.isHasTech(eTech1) &&
			kTeam.isHasTech(eTech2)) // advc.001: This check was missing
		{
			return true;
		}
		// <advc.031>
		for (MemberIter it(eTeam); it.hasNext(); ++it)
		{
			CvPlayer const& kMember = *it;
			if (kMember.getCurrentResearch() == eTech1 &&
				kMember.getCurrentResearch() == eTech2)
			{
				return true;
			}
		} // </advc.031>
	}
	return false;
}

// (replacing all CvPlot::isRevealed calls)
bool AIFoundValue::isRevealed(CvPlot const& p) const
{
	return (kSet.isAllSeeing() || p.isRevealed(eTeam));
}

// (replacing all CvPlot::getBonusType and getNonObsoleteBonusType calls)
BonusTypes AIFoundValue::getBonus(CvPlot const& p) const
{
	bool bRevealBonus = (kSet.isStartingLoc() &&
			// advc.108: Don't factor in unrevealed bonuses
			kGame.getStartingPlotNormalizationLevel() > CvGame::NORMALIZE_LOW);
	return p.getBonusType(bRevealBonus ? NO_TEAM : eTeam);
}

/*  <advc.031> Rewritten. K-Mod had added a crucial isImprovementBonusTrade check, but
	other important checks were still missing. */
ImprovementTypes AIFoundValue::getBonusImprovement(BonusTypes eBonus, CvPlot const& p,
	bool& bCanTrade, bool& bCanTradeSoon, int* aiYield,
	bool& bCanImprove, bool& bCanImproveSoon, bool& bRemoveFeature) const
{
	bCanTrade = bCanTradeSoon = bCanImprove = bCanImproveSoon = bRemoveFeature = false;
	if (eBonus == NO_BONUS)
		return NO_IMPROVEMENT;

	ImprovementTypes eBestImprovement = NO_IMPROVEMENT;
	int iBestYield = -1;
	FeatureTypes eFeature = p.getFeatureType();
	FOR_EACH_ENUM(Build)
	{
		CvBuildInfo const& kBuild = GC.getInfo(eLoopBuild);
		ImprovementTypes eImprovement = kBuild.getImprovement();
		if (eImprovement == NO_IMPROVEMENT)
			continue;
		CvImprovementInfo const& kImprovement = GC.getInfo(eImprovement);
		TechTypes const eBuildPrereq = kBuild.getTechPrereq();
		if (eImprovement == NO_IMPROVEMENT ||
			!kImprovement.isImprovementBonusMakesValid(eBonus) ||
			!kImprovement.isImprovementBonusTrade(eBonus) ||
			!isNearTech(eBuildPrereq))
		{
			continue;
		}
		TechTypes const eFeaturePrereq = (eFeature == NO_FEATURE ? NO_TECH :
				kBuild.getFeatureTech(eFeature));
		if (!isNearTech(eFeaturePrereq))
			continue;
		bCanTradeSoon = true;
		bCanImproveSoon = true;
		if (kTeam.isHasTech(eBuildPrereq) && kTeam.isHasTech(eFeaturePrereq))
		{
			bCanTrade = true;
			bCanImprove = true;
		}
		else if (bCanTrade) // Prefer currently available build - regardless of yield
			continue;
		int iYieldValue = 0;
		bool bRemove = (eFeature != NO_FEATURE && kBuild.isFeatureRemove(eFeature));
		FOR_EACH_ENUM(Yield) // Make sure we're not picking Fort over a yield improvement
		{
			iYieldValue += kImprovement.getYieldChange(eLoopYield) +
					kImprovement.getImprovementBonusYield(eBonus, eLoopYield);
			if (!bRemove && eFeature != NO_FEATURE)
				iYieldValue += GC.getInfo(eFeature).getYieldChange(eLoopYield);
		}
		if (iYieldValue > iBestYield)
		{
			eBestImprovement = eImprovement;
			iBestYield = iYieldValue;
			bRemoveFeature = bRemove;
		}
	}
	TechTypes eTradeTech = (TechTypes)GC.getInfo(eBonus).getTechCityTrade();
	bCanTradeSoon = (bCanTradeSoon && isNearTech(eTradeTech));
	bCanTrade = (bCanTrade && bCanTradeSoon && kTeam.isHasTech(eTradeTech));
	/*  <advc.108> Depending on the NormalizationLevel, eBonus can be unrevealed
		when placing starting locations. */
	if (kSet.isStartingLoc())
	{
		TechTypes eRevealTech = (TechTypes)GC.getInfo(eBonus).getTechReveal();
		if (!isNearTech(eTradeTech))
		{
			bCanTradeSoon = false;
			bCanImproveSoon = false;
		}
		if (!kTeam.isHasTech(eRevealTech))
		{
			bCanTrade = false;
			bCanImprove = false;
		}
	} // </advc.108>
	if (eBestImprovement == NO_IMPROVEMENT)
		return NO_IMPROVEMENT;
	FOR_EACH_ENUM(Yield)
	{
		aiYield[eLoopYield] = GC.getInfo(eBestImprovement).
				getImprovementBonusYield(eBonus, eLoopYield);
		/*	(We don't count GC.getInfo(eBestImprovement).getYieldChange().
			Most tiles get some improvement; that's not special. Problem with this:
			overestimates improvements with 0 yield change, i.e. Plantation, Pasture.) */
		if (!bRemoveFeature && eFeature != NO_FEATURE)
		{
			aiYield[eLoopYield] += GC.getInfo(eFeature).getYieldChange(eLoopYield);
			if (p.isRiver())
				aiYield[eLoopYield] += GC.getInfo(eFeature).getRiverYieldChange(eLoopYield);
		}
		else if (p.isRiver())
			aiYield[eLoopYield] += GC.getInfo(p.getTerrainType()).getRiverYieldChange(eLoopYield);
	}
	return eBestImprovement;
} // </advc.031>


bool AIFoundValue::isNearTech(TechTypes eTech) const
{
	return (eTech == NO_TECH || kTeam.isHasTech(eTech) ||
		kPlayer.getCurrentResearch() == eTech ||
		/*  <advc.108> With our first city, we can wait a bit a longer for the proper tech.
			(Not when starting in a later era though; research starts out slow then.) */
		(iCities <= 0 && eEra >= GC.getInfo(eTech).getEra() && kGame.getStartEra() <= 0) ||
		// </advc.108>
		// The HasTech and CurrentResearch checks are redundant, I think, but faster.
		kPlayer.canResearch(eTech, false, false, true));
}

// (not much pre-AdvCiv code left)
int AIFoundValue::calculateCultureModifier(CvPlot const& p, bool bForeignOwned,
	bool bShare, bool bCityRadius, bool bSteal, bool bFlip, bool bOwnExcl,
	int& iTakenTiles, int& iStealPercent) const
{
	scaled_int r = 100;
	/*  K-Mod note: iClaimThreshold is bigger for bEasyCulture and bAmbitious civs.
		Also note, if the multiplier was to be properly used for unowned plots,
		it would need to take into account the proximity of foreign cities and so on.
		(similar to the iForeignProximity calculation) */
	// advc.031: I'm including unowned plots, but only those in a foreign city radius.
	if ((!bForeignOwned && (bShare || !bCityRadius)) /* advc.035: */ || bFlip)
		return r.round();

	int iOurCulture = p.getCulture(ePlayer);
	int iOtherCulture = std::max(1,
			/*  advc.031: Could find out the owner of the bCityRadius
				city in the loop that computes abFlip, but it should be
				fine to assume that the owner has very little tile culture. */
			(!bForeignOwned ? 1 :
			p.getCulture(p.getOwner())));
	// <advc.031> Don't settle near rival capital (e.g. 2nd starting settler, OCC)
	if (bCityRadius && kGame.getElapsedGameTurns() <= 5)
		iOtherCulture = std::max(iOtherCulture, 200);
	CvCity* pForeignCity = p.getWorkingCity();
	if (bForeignOwned)
	{
		if ((pForeignCity != NULL && pForeignCity->isCapital()) ||
			// Likely to struggle with a single colony against multiple rival cities
			(iAreaCities <= 0 && p.getArea().getCitiesPerPlayer(p.getOwner()) > 1))
		{
			iOtherCulture = (3 * iOtherCulture) / 2;
		}
	} // </advc.031>
	r.mulDiv(iOurCulture + kSet.getClaimThreshold(), iOtherCulture + kSet.getClaimThreshold());
	if (!bOwnExcl) // advc.035: The above is OK if cities own their exclusive radius ...
	{	// <advc.031>
		// ... but w/o that rule, it's too optimistic when iOurCulture is small.
		r = (r + (iOurCulture * 100) / kSet.getClaimThreshold()) / 2;
	}
	// Take into account at least some factors that determine each side's culture rate
	scaled_int rRateModifier = 1;
	/*	Don't be _too_ optimistic against Barbarians - a civ (a rival - or us) will
		probably conquer pForeignCity eventually and might not raze it. */
	if (bForeignOwned && p.isBarbarian())
		rRateModifier *= fixp(1.55);
	else if(pForeignCity != NULL &&
		// advc.303: Barbarian borders won't expand
		!adjacentOrSame(p, *pForeignCity->plot()))
	{
		rRateModifier *= fixp(1.8);
	}
	if (!p.isArea(kArea))
		rRateModifier /= fixp(1.3);
	else if (pForeignCity != NULL && !pForeignCity->isArea(p.getArea()))
		rRateModifier *= fixp(1.3);
	/*  K-Mod had done *5/4 if EasyCulture; I think only free culture will really
		swing culture wars. */
	int iFreeForeignCulture = (bForeignOwned ? GET_PLAYER(p.getOwner()).
			getFreeCityCommerce(COMMERCE_CULTURE) : 0);
	if (pForeignCity != NULL && pForeignCity->isCapital())
	{
		/*  Tbd.: Should just use pForeignCity->getCommerceRate(COMMERCE_CULTURE).
			advc.045 hides foreign buildings, but it should, in theory, be possible
			to infer culture buildings from visible tile culture percentages.
			But will then also have to predict our own culture rate. */
		static BuildingTypes const eCapitalBuilding = GC.getCivilizationInfo(
				pForeignCity->getCivilizationType()).
				getCivilizationBuildings(GC.getDefineINT("CAPITAL_BUILDINGCLASS"));
		if (eCapitalBuilding != NO_BUILDING)
		{
			iFreeForeignCulture += GC.getInfo(eCapitalBuilding).
					getCommerceChange(COMMERCE_CULTURE); // (normally 0)
			iFreeForeignCulture += GC.getInfo(eCapitalBuilding).
					getObsoleteSafeCommerceChange(COMMERCE_CULTURE);
		}
	}
	scaled_int const rFreeForeignCultureModifier(iFreeForeignCulture + 7, 7);
	// Extra pessimism about tiles that another civ is able to work (borders already expanded)
	if (bSteal)
	{
		rRateModifier = fixp(0.62);
		if (pForeignCity != NULL && adjacentOrSame(p, *pForeignCity->plot()))
			rRateModifier /= fixp(1.5);
		if (bForeignOwned)
			rRateModifier *= rFreeForeignCultureModifier;
	}
	if (adjacentOrSame(p, kPlot))
		rRateModifier *= fixp(1.5); // as in K-Mod
	else rRateModifier *= rFreeForeignCultureModifier;	
	if (bCityRadius && !bShare)
	{
		r *= rRateModifier * fixp(0.8);
		iStealPercent += std::min(75, r.round());
	}
	else r *= rRateModifier;
	// </advc.031>
	r.decreaseTo(100);
	// advc.031: Moved up
	if (r < (iAreaCities > 0 ? 25 : 50) &&
		!bSteal) // advc.031: Already counted as iTakenTiles
	{
		//discourage hopeless cases, especially on other continents.
		iTakenTiles += (iAreaCities > 0 ? 1 : 2);
	}
	// <advc.099b>
	if (!bCityRadius && bForeignOwned)
	{
		scaled_int rExclRadiusWeight = GC.AI_getGame().AI_exclusiveRadiusWeight(
				::plotDistance(&p, &kPlot));
		r *= 1 + rExclRadiusWeight;
		r.decreaseTo(100);
		// </advc.099b>  <advc.035>
		if (bOwnExcl)
		{
			// (The discouragement above is still useful for avoiding revolts)
			r.increaseTo(65);
		} // </advc.035>
	}
	return r.round();
}

/*	An estimate of how much production an improvement might add
	in the medium term if production is prioritized. Precision: times 100.
	Note: Any additional production from improving a bonus resource
	is counted as iSpecialProduction elsewhere. This function ignores
	bonus resources. */
int AIFoundValue::estimateImprovementProduction(CvPlot const& p,
	bool bPersistentFeature) const
{
	if (p.isWater())
	{
		// Tbd.: Should check for water improvements
		return 0;
	}
	//return (p.isHills() ? 200 : 100);
	/*  <advc.031> The above is pretty bad: We're not going to build
		Workshops everywhere, and it doesn't check for Peak or Desert. */
	FeatureTypes eFeature = p.getFeatureType();
	// If persistent, then production from feature is already counted above.
	if (eFeature != NO_FEATURE && !bPersistentFeature)
	{
		int iProductionChange = GC.getInfo(eFeature).getYieldChange(YIELD_PRODUCTION);
		if (iProductionChange > 0)
		{
			// 0.5 for chopping or Lumbermill (I shouldn't hardcode it like this ...)
			return iProductionChange * 100 + 50;
		}
	}
	if (iCities <= 2)
	{
		int r = 0;
		if(p.isHills())
			r += 200;
		return r;
	}
	int r = 0;
	FOR_EACH_ENUM(Improvement)
	{
		// Not a perfectly safe way to check if we can build the improvement - but fast.
		if (kPlayer.getImprovementCount(eLoopImprovement) <= 0)
			continue;
		CvImprovementInfo& kLoopImprovement = GC.getInfo(eLoopImprovement);
		int iYieldChange = kLoopImprovement.getYieldChange(YIELD_PRODUCTION) +
				kTeam.getImprovementYieldChange(eLoopImprovement, YIELD_PRODUCTION);
		// Will be less inclined to build improvement if it hurts other yields
		FOR_EACH_ENUM(Yield)
		{
			if (eLoopYield == YIELD_PRODUCTION)
				continue;
			int iOtherYield = kLoopImprovement.getYieldChange(eLoopYield);
			if (iOtherYield < 0)
				iYieldChange += iOtherYield;
		}
		/*  I'm not bothering with civics and routes here. It's OK to undercount
			b/c more production is needed by the time railroads become available.
			A Workshop may also remove a Forest; in that case, we're overcounting. */
		// Not fast; loops through all builds.
		if (iYieldChange <= 0 || !p.canHaveImprovement(eLoopImprovement, eTeam))
			continue;
		FAssertMsg(iYieldChange <= 3, "is this much production possible?");
		r = std::max(r, 100 * iYieldChange);
	}
	return r; // </advc.031>
}


int AIFoundValue::evaluateYield(int const* aiYield, CvPlot const* p,
	bool bCanNeverImprove) const
{
	int r = 0;
	int aiWeight[NUM_YIELD_TYPES] = {15, 15, 8 };
	// (note: these numbers have been adjusted for K-Mod)
	if (p != NULL && !p->isWater() && // advc.031: Exclude seafood
		(isHome(*p) || aiYield[YIELD_FOOD] >= GC.getFOOD_CONSUMPTION_PER_POPULATION())) 
	{
		r += 10;
		aiWeight[YIELD_FOOD] = 40;
		aiWeight[YIELD_PRODUCTION] = 30;
		aiWeight[YIELD_COMMERCE] = 20;
		/*if (kSet.isStartingLoc())
			r *= 2;*/ // BtS
	}
	else if (aiYield[YIELD_FOOD] == GC.getFOOD_CONSUMPTION_PER_POPULATION() - 1)
	{
		aiWeight[YIELD_FOOD] = 30;
		aiWeight[YIELD_PRODUCTION] = 25;
		aiWeight[YIELD_COMMERCE] = 12;
	}
	/*  <advc.108> For moving the starting Settler and for more
		early-game commerce in general */
	if(iCities <= 1 && eEra <= 0)
		aiWeight[YIELD_COMMERCE] += 5; // </advc.108>  <advc.303>
	if (bBarbarian)
	{
		aiWeight[YIELD_FOOD] -= 4;
		aiWeight[YIELD_PRODUCTION] += 5;
		aiWeight[YIELD_COMMERCE] -= 3;
	} // </advc.303>
	FOR_EACH_ENUM(Yield)
	{
		FAssert(aiWeight[eLoopYield] > 0); // advc.303
		int iYieldValue = aiYield[eLoopYield] * aiWeight[eLoopYield];
		/*	<advc.031> Mined resources (-1 production) get a too low value otherwise.
			See the comment at the end of getBonusImprovement. */
		if (iYieldValue < 0)
			iYieldValue /= 2; // </advc.031>
		r += iYieldValue;
	}
	if (p != NULL && p->isWater())
	{
		// K-Mod. kludge to account for lighthouse and lack of improvements.
		r /= (bCoastal ? 2 : 3);
		r += (bCoastal ? 8 * (aiYield[YIELD_COMMERCE]
				+ aiYield[YIELD_PRODUCTION]) : 0);

		if (kSet.isStartingLoc() && !bNormalize)
		{
			r += bCoastal ? 0 : -75; // advc.031: was -400 in BtS, -120 in K-Mod
			/*	(K-Mod comment: "I'm pretty much forbidding starting 1 tile
				inland non-coastal with more than a few non-lake water tiles.) */
		}
	}  // <advc.031>
	if (bCanNeverImprove)
		r = (r * 77) / 100; // </advc.031>
	return r;
}


int AIFoundValue::evaluateFreshWater(CvPlot const& p, int const* aiYield, bool bSteal,
	int& iRiverTiles, int& iGreenTiles) const
{
	int r = 0;
	// <advc.053>
	bool bLowYield = (aiYield[YIELD_FOOD] + aiYield[YIELD_PRODUCTION] +
			aiYield[YIELD_COMMERCE] / 2 < 2); // </advc.053>
	if (p.isRiver())
	{
		//r += 10; // BtS
		// <K-Mod>
		//r += (kSet.isExtraCommerceThreshold() || kSet.isStartingLoc()) ? 30 : 10;
		//r += (pPlot->isRiver() ? 15 : 0); // </K-Mod>
		// advc: Replaced by the code below
		// <advc.053>
		if (bLowYield)
		{
			if (kSet.isExtraCommerceThreshold())
				r += 5;
		}
		else // </advc.053>
		{
			// <advc.031>
			if (kSet.isExtraCommerceThreshold())
				r += 8;
			if (!bSteal)
			{
				iRiverTiles++;
				if (iCities <= 0) // advc.108
					r += 10;
				/*  I'm guessing this K-Mod clause is supposed to
					steer the AI toward settling at rivers rather
					than trying to make all river plots workable. */
				if (kPlot.isRiver())
				{
					r += (/* advc.108: */ iCities <= 0
							? 10 : 4);
				}
			} // </advc.031>
		}
	}  // in addition:
	if (p.canHavePotentialIrrigation() && /* advc.053: */ !bLowYield)
	{
		// <advc.031> was 5/5
		r += 4;
		if (p.isFreshWater())
		{
			r += 6;
			if (aiYield[YIELD_FOOD] >= GC.getFOOD_CONSUMPTION_PER_POPULATION() &&
					!bSteal)
				iGreenTiles++;
		} // <advc.031>
	}
	return r;
}

/*	<advc.031> A plot next to a resource will usually have a higher found value
	than the resource plot itself because of the improvement yields counted by
	evaluateSpecialYields. But not always - it depends on what else is in the
	city radius. Founding on a resource usually cannibalizes other potential
	city sites, so it needs to be discouraged a bit. */
int AIFoundValue::foundOnResourceValue(int const* aiBonusImprovementYield) const
{
	int r = -5;
	if (aiBonusImprovementYield == NULL)
		r -= 42; // When we can't currently improve the resource
	else
	{
		int iImprovementYieldValue = evaluateYield(aiBonusImprovementYield);
		if (iImprovementYieldValue > 0) // Make sure not to exponentiate a negative value
			r -= ::round(std::pow((double)iImprovementYieldValue, 1.5) / 4.2);
	}
	/*	In (historical) scenarios, resources are sometimes placed just so that the AI
		doesn't settle in a particular tile. Try -a little bit- to respect that. */
	if (kGame.isScenario())
		r -= 13;
	IFLOG logBBAI("%d for founding on resource", r);
	return r;
} // </advc.031>


int AIFoundValue::applyCultureModifier(CvPlot const& p, int iPlotValue, int iCultureModifier,
	bool bShare) const
{
	int r = iPlotValue;
	r *= iCultureModifier;
	r /= 100;
	// <advc.031>
	if (bShare) // bSteal is already factored into iCultureModifier
		r = ::round(r * 0.375);
	// </advc.031>
	return r;
}

// Note: aiBonusCount is only a partial count (resources evaluated up to this point)
int AIFoundValue::nonYieldBonusValue(CvPlot const& p, BonusTypes eBonus,
	bool bCanTrade, bool bCanTradeSoon, bool bEasyAccess, bool& bAnyGrowthBonus,
	std::vector<int>& aiBonusCount, int iCultureModifier) const
{
	/*int r = kPlayer.AI_bonusVal(eBonus, 1, true) * (!kSet.isStartingLoc() &&
			kPlayer.getNumTradeableBonuses(eBonus) == 0 && aiBonusCount[eBonus] == 1 ?
			80 : 20);*/ // BtS
	/*int iCount = kPlayer.getNumTradeableBonuses(eBonus) == 0 + aiBonusCount[eBonus];
	int r = AI_bonusVal(eBonus, 0, true) * 80 / (1 + 2*iCount);*/ // K-Mod
	/*  <advc.031> The "==0" looks like an error. Rather than correct that,
		I'll let AI_bonusVal handle the number of bonuses already connected (iChange=1).
		A division by NumTradeableBonuses here won't work well for strategic resources. */
	// Coefficient was 80
	int r = ::round(kPlayer.AI_bonusVal(eBonus, 1, true) * 57 /
			(1 + aiBonusCount[eBonus]));
	bool bSurplus = (kPlayer.getNumAvailableBonuses(eBonus) > 0 && aiBonusCount[eBonus] <= 0);
	// </advc.031>
	aiBonusCount[eBonus]++;
	// <advc.031>
	bool bGrowthBonus = (!bAnyGrowthBonus && bCanTradeSoon && !bSurplus &&
			(GC.getInfo(eBonus).getHappiness() +
			// Basically only luxury resources qualify
			GC.getInfo(eBonus).getHealth() / 2 > 0) &&
			(bCanTrade || p.getFeatureType() == NO_FEATURE ||
			GC.getInfo(p.getFeatureType()).getHealthPercent() >= 0));
	if (bGrowthBonus) // Reward only the first new luxury
		bAnyGrowthBonus = true;
	// </advc.031>
	/*  K-Mod: try not to make the value of strategic resources too overwhelming.
		(note: I removed a bigger value reduction from the original code.) */
	// (advc: Should perhaps make such adjustments in CvPlayerAI::AI_bonusValue instead)
	int iEarlyGameModifier = 100;
	if (kSet.isStartingLoc() && !bNormalize)
	{	// <advc.031>
		if (bGrowthBonus)
			iEarlyGameModifier = 40; // </advc.031>
		else
		{
			// (advc: Divisor was 4 in K-Mod; BtS had divided by 2 after evaluateSpecialYields.)
			iEarlyGameModifier = 
				/*  <advc.108> Don't need to decrease as much if reveal-techs are respected
						(also: advc.036 improves the evaluation of non-strategic resources) */
					(kGame.getStartingPlotNormalizationLevel() <= CvGame::NORMALIZE_LOW ? 33 : 25);
		}
	}
	else if (iCities <= 0) // For moving the starting Settler and normalization
	{
		if (bGrowthBonus)
			iEarlyGameModifier = 75;
		else iEarlyGameModifier = 55;
	} // </advc.108>
	// <advc.031> High values for strategic resources remain a problem during the early game
	else
	{
		int const iTargetCities = GC.getInfo(GC.getMap().getWorldSize()).getTargetNumCities();
		if (iCities + 1 < iTargetCities)
		{
			FAssert(iTargetCities >= 3);
			if (bGrowthBonus) // Modifier climbs to 120%, then decreases to 100%.
			{
				int iPeakCity = (iTargetCities + 1) / 2;
				int iPercentIncrement = 30 / (iPeakCity - 1);
				iEarlyGameModifier = 90 + iPercentIncrement *
						(iCities < iPeakCity ? iCities : iTargetCities - iCities - 1);
			}
			else // Modifier climbs to 100%
			{
				int iPercentIncrement = 50 / (iTargetCities - 1);
				iEarlyGameModifier = 55 + iPercentIncrement * iCities;
			}
		}
	}
	IFLOG if(iEarlyGameModifier!=100) logBBAI("Early-game modifier for non-yield resource value: %d percent", iEarlyGameModifier);
	r = (iEarlyGameModifier * r) / 100;
	/*	(cf. getBonusImprovement)
		AI_bonusValue can check for tech requirements, but it can't check requirements
		for building the improvement. Hence bAssumeEnabled=true is used in the
		AI_bonusValue call above and we take care of tech requirements ourselves. */		
	if (!bCanTrade)
	{
		if (!bSurplus)
		{
			/*  Important for high-value strategic resources that get revealed
				long before they can be traded, especially Oil.
				CvPlayerAI::AI_countOwnedBonuses is too expensive I think,
				but I'm copying a bit of code from there. */
			FOR_EACH_CITYAI(pCity, kPlayer)
			{
				if (pCity->AI_countNumBonuses(eBonus, true, true, -1) > 0)
				{
					bSurplus = true;
					break;
				}
			}
		}
		IFLOG if(bSurplus) logBBAI("Surplus resource");
		if (bCanTradeSoon)
			r *= 70;
		else r *= 33;
		r *= (bSurplus ? 30 : 100);
		r /= 100;
		// <advc.040>
		if (bEasyAccess)
			r *= 100;
		else
		{
			/*  Might be better to place a city in p.area(). But if that area
				is tiny, then accessing the resource from a different landmass
				is probably our best bet. */
			r *= (p.getArea().getNumTiles() <= 2 ? 60 : 45);
		}
		r /= 100 /* </advc.040> */ * 100;

		if (bSurplus)
			r = std::min(125, r);
	} // </advc.031>
	if (kSet.isStartingLoc())
		return r;

	// K-Mod. (original code deleted)
	if (!isHome(p))
	{	/*  <advc.031> Why halve the value of water bonuses? Perhaps because
			they're costly to improve. But that's only true in the early game.
			Because they tend to be common? AI_bonusVal takes care of that. */
		if (p.isWater()/*) {//r /= 2;*/ && eEra < 3)
		{
			int iWaterPenalty = (3 - eEra) * 16;
			r -= iWaterPenalty;
			r = std::max(r, 0);
			IFLOG logBBAI("Penalty for water resource: %d", iWaterPenalty);
		}
		// iCultureModifier should have this covered
		/*if (p.getOwner() != ePlayer && ::stepDistance(&kPlot, &p) > 1) {
			if (!kSet.isEasyCulture())
				r = (r * 3) / 4;
		}*/
		// advc.031: Don't ignore culture modifier < 60
		int iModifier = (kSet.isAmbitious() && iCultureModifier >= 60 ?
				110 : iCultureModifier);
		r = (r * iModifier) / 100;
		IFLOG if(iModifier!=iCultureModifier) logBBAI("Non-yield resource value increased b/c of ambitious personality");
	}
	else if (kSet.isAmbitious())
		r = r * 110 / 100;
	// K-Mod end
	return r;
}


void AIFoundValue::calculateSpecialYields(CvPlot const& p,
	int const* aiBonusImprovementYield, int const* aiNatureYield,
	int iModifier, int iEffectiveFood, int* aiSpecialYield,
	int& iSpecialFoodPlus, int& iSpecialFoodMinus, int& iSpecialYieldTiles) const
{
	if (aiBonusImprovementYield == NULL) // K-Mod: non bonus related special food
	{
		int iSurplus = ::round( // advc.031
				std::max(0, iEffectiveFood - GC.getFOOD_CONSUMPTION_PER_POPULATION())
				* (iModifier / 100.0)); // advc.031
		iSpecialFoodPlus += iSurplus;
		aiSpecialYield[YIELD_FOOD] += iSurplus; // advc.031: A little extra love for Flood Plains
		return;
	}
	// advc.031: No need to recompute nature yield here
	int iSpecialFood = ::round((aiNatureYield[YIELD_FOOD] +
			aiBonusImprovementYield[YIELD_FOOD]) * /* advc.031: */ (iModifier / 100.0));
	aiSpecialYield[YIELD_FOOD] += iSpecialFood;
	int iFoodTemp = iSpecialFood - GC.getFOOD_CONSUMPTION_PER_POPULATION();
	// advc.031:
	iFoodTemp = ::round(iFoodTemp * iModifier / 100.0);
	iSpecialFoodPlus += std::max(0, iFoodTemp);
	iSpecialFoodMinus -= std::min(0, iFoodTemp);
	// <advc.031>
	int iSpecialProd = aiNatureYield[YIELD_PRODUCTION] +
			aiBonusImprovementYield[YIELD_PRODUCTION];
	int iSpecialComm = aiNatureYield[YIELD_COMMERCE] +
			aiBonusImprovementYield[YIELD_COMMERCE];
	/*  No functional change above. Now count the tile as special and
		apply culture modifier. */
	bool const bSpecial =  (iSpecialFood > 0 || iSpecialComm > 0 || iSpecialProd > 0);
	if (bSpecial)
		iSpecialYieldTiles++;
	/*  To avoid rounding all yields to 0, don't reduce production and commerce
		to less than 1. */
	iSpecialProd = std::max(std::min(1, iSpecialProd),
			::round(iSpecialProd * iModifier / 100.0));
	aiSpecialYield[YIELD_PRODUCTION] += iSpecialProd;
	iSpecialComm = std::max(std::min(1, iSpecialComm),
			::round(iSpecialComm * iModifier / 100.0));
	aiSpecialYield[YIELD_COMMERCE] += iSpecialComm;
	IFLOG if(bSpecial) logBBAI("Special yield: %dF%dP%dC (modifier: %d percent)",
			iSpecialFood, iSpecialProd, iSpecialComm, iModifier);
	// </advc.031>
}

// <advc.031> Weighted sum
int AIFoundValue::sumUpPlotValues(std::vector<int>& aiPlotValues) const
{
	std::sort(aiPlotValues.begin(), aiPlotValues.end(), std::greater<int>());
	// CITY_HOME_PLOT should have 0 value here, others could have negative values.
	FAssert(aiPlotValues[NUM_CITY_PLOTS - 1] <= 0);
	double const maxMultPercent = 153;
	double const minMultPercent = 47;
	double const normalizMult = 1;
	double const subtr = 29;
	double const exp = std::log(maxMultPercent - minMultPercent) /
			std::log(NUM_CITY_PLOTS - 1.0);
	int r = 0;
	FOR_EACH_ENUM(CityPlot)
	{
		int iPlotValue = aiPlotValues[eLoopCityPlot];
		if (iPlotValue > 0)
		{
			iPlotValue = std::max(iPlotValue / 3,
					::round(std::max(0.0, iPlotValue - subtr) * 0.01 *
					// Linearly decreasing multiplier:
					/*(maxMultPercent - i * ((maxMultPercent -
					minMultPercent) / (NUM_CITY_PLOTS - 1))));*/
					// Try power law instead:
					normalizMult * (minMultPercent + std::pow((double)
					std::abs(NUM_CITY_PLOTS - 1 - eLoopCityPlot), exp))));
		}
		r += iPlotValue;
	}
	IFLOG logBBAI("Weighted sum of plot values:\n+%d", r);
	return r;
} // </advc.031>

/*	Note: aiSpecialYield includes aiNatureYield. Thus, nature yield is counted twice:
	once in evaluateYield, a second time in evaluateSpecialYield. This was already
	the case in BtS and it might work out more or less correctly on the bottom line,
	but it's messy. */
int AIFoundValue::evaluateSpecialYields(int const* aiSpecialYield,
	int iSpecialYieldTiles, int iSpecialFoodPlus, int iSpecialFoodMinus) const
{
	/*return iSpecialFood*50+iSpecialProduction*50+iSpecialCommerce*50;
	if (kSet.isStartingLoc())
		r /= 2;*/ // BtS
	/*	K-mod. It's tricky to get this right. Special commerce is great
		in the early game, but not so great later on. Food is always great -
		unless we already have too much.
		iSpecialFood is whatever food happens to be associated with bonuses.
		Don't value it highly, because it's also counted in a bunch of other ways. */
	//return iSpecialFood*20+iSpecialProduction*40+iSpecialCommerce*35;
	// <advc.031>
	double weight[NUM_YIELD_TYPES] = {0.24, 0.36,
			/*  advc.108: For moving the starting Settler. Though a commercial
				resource at the second city is also valuable, so: */
			iCities <= 1 && eEra <= 0 ? 0.48 : 0.32};
	double const div = iSpecialYieldTiles;
	double fromSpecial = 0;
	FOR_EACH_ENUM(Yield)
		fromSpecial += aiSpecialYield[eLoopYield] / div * weight[eLoopYield];
	if(fromSpecial > 0)
		fromSpecial = std::pow(fromSpecial, 1.5) * 75 * iSpecialYieldTiles;
	// advc.031: Apply the BtS/K-Mod food modifier only to the special yield value
	int iFoodSurplus = std::max(0, iSpecialFoodPlus - iSpecialFoodMinus);
	int iFoodDeficit = std::max(0, iSpecialFoodMinus - iSpecialFoodPlus);
	/*r *= 100 + 20 * std::max(0, std::min(iFoodSurplus, 2 * GC.getFOOD_CONSUMPTION_PER_POPULATION()));
	r /= 100 + 20 * std::max(0, iFoodDeficit);*/ // BtS
	// K-Mod. (note that iFoodSurplus and iFoodDeficit already have the "max(0, x)" built in.
	/*r *= 100 + (kSet.isExpansive() ? 20 : 15) * std::min(
			(iFoodSurplus + iSpecialFoodPlus)/2,
			2 * GC.getFOOD_CONSUMPTION_PER_POPULATION());
	r /= 100 + (kSet.isExpansive() ? 20 : 15) * iFoodDeficit;*/ // K-Mod end
	// Turn it into a single multiplier ...
	double modifier = (100.0 + (kSet.isExpansive() ? 20 : 15) * std::min(
			(iFoodSurplus + iSpecialFoodPlus) / 2,
			2 * GC.getFOOD_CONSUMPTION_PER_POPULATION())) /
			(100.0 + (kSet.isExpansive() ? 20 : 15) * iFoodDeficit);
	// ... and reduce the impact b/c of new food modifier in adjustToFood
	modifier = (modifier + 2) / 3;
	int r = ::round(fromSpecial * modifier);
	IFLOG logBBAI("+%d from special yields %dF%dP%dC (food surplus modifier: %d percent)", r,
			aiSpecialYield[YIELD_FOOD], aiSpecialYield[YIELD_PRODUCTION], aiSpecialYield[YIELD_COMMERCE],
			::round(100 * modifier));
	return r;
	// </advc.031>
}


bool AIFoundValue::isTooManyTakenTiles(int iTaken, int iResourceValue,
	bool bLowValue) const
{
	return (((iTaken > NUM_CITY_PLOTS / 3 ||
			(bBarbarian && iTaken > 2)) &&// advc.303
			iResourceValue < 250 && bLowValue) ||
			/* <advc.031> */ (iTaken > (2 * NUM_CITY_PLOTS) / 3 &&
			iResourceValue < 800)); // </advc.031>
}

/*	<advc.031> Unlike the food modifier in evaluateSpecialYields, this modifier applies
	to all yields and takes into account grassland farms.*/
int AIFoundValue::adjustToFood(int iValue, int iSpecialFoodPlus, int iSpecialFoodMinus,
	int iGreenTiles) const
{
	double lowFoodModifier = 1;
	if (eEra < 4)
	{
		int iSpecialSurplus = (iSpecialFoodPlus - iSpecialFoodMinus + 1) / 2; // ceil
		lowFoodModifier = (8.5 + iGreenTiles + iSpecialSurplus) / 11.5;
		lowFoodModifier = ::dRange(lowFoodModifier, 0.5, 1.0);
	}
	IFLOG if(::round(100*lowFoodModifier)!=100) logBBAI("Times %d percent for lack of food "
			"(special food +/-: %d/%d, green tiles: %d)", ::round(100 * lowFoodModifier),
			iSpecialFoodPlus, iSpecialFoodMinus, iGreenTiles);
	return ::round(iValue * lowFoodModifier);
} // </advc.031>

// (adjustToBadHealth deals with short-term health)
int AIFoundValue::evaluateLongTermHealth(int& iHealthPercent) const
{
	int r = 0;
	int iFreshWaterHealth = 0;
	if (kPlot.isFreshWater())
	{
		iFreshWaterHealth = GC.getDefineINT(CvGlobals::FRESH_WATER_HEALTH_CHANGE);
		iHealthPercent += 100 * iFreshWaterHealth; // advc.031
	}
	//iValue += (iHealth / 5);
	/*  <advc.031> The above may have accounted for feature production; now
		evaluated separately elsewhere. */
	if (iHealthPercent > 0 || eEra > 1)
		r += std::min(iHealthPercent, 350) / 6;
	// Extra bonus for persistent health (as in BtS/ K-Mod): // </advc.031> 
	r += iFreshWaterHealth * 30;
	// (K-Mod (commented this out, compensated by the river bonuses I added.)
	/*if (iFreshWaterHealth > 0)
		r += 40;*/
	IFLOG if(r!=0) logBBAI("+%d from %d/100 health", r, iHealthPercent);
	return r;
}

// <advc.031>
int AIFoundValue::evaluateFeatureProduction(int iProduction) const
{
	/*  Can't chop in the very early game (would be nicer to check for
		feature removal tech and sufficient workers than to go by era) */
	int r = (iProduction * 3) / ((eEra == 0 ? 2 : eEra) + 2);
	IFLOG if(r!=0) logBBAI("+%d from %d feature production", r, iProduction);
	return r;
} // </advc.031>


int AIFoundValue::evaluateSeaAccess(bool bGoodFirstColony, double productionModifier,
	int iLandTiles) const
{
	int r = 0;
	// <advc.303>
	if (bBarbarian)
	{
		r += 350;
		IFLOG logBBAI("+%d for coastal (Barbarian)", r);
		return r;
	} // </advc.303>
	//if (!kSet.isStartingLoc())
	if (/* advc.108: */ iCities <= 0)
	{
		// BtS: "let other penalties bring this down."
		r += 360; // advc.031: was 600 in BtS, 500 in K-Mod
		if (!bNormalize && kArea.getNumStartingPlots() <= 0)
		{
			// advc.031: An inland sea will probably do an isolated civ no good
			if (GC.getMap().findBiggestArea(true) == kPlot.waterArea(true))
				r += 450; // advc.031: was 1000 in BtS, 600 in K-Mod
		}
		IFLOG logBBAI("+%d for coastal (1st city)", r);
		return r;
	}
	if (kArea.getCitiesPerPlayer(ePlayer) <= 0)
	{
		//r += (iResourceValue > 0) ? 800 : 100; // (BtS)
		// K-Mod replacement:
		//r += iResourceValue > 0 ? (kSet.isSeafaring() ? 600 : 400) : 100;
		// advc.040: Mostly handled by the bFirstColony code elsewhere now
		if (bGoodFirstColony)
		{
			r += 250;
			IFLOG logBBAI("+%d for promising first colony", r);
		}
		return 60; // advc.031: For trade routes, ability to produce ships
	}
	/*  BETTER_BTS_AI_MOD, Settler AI, 02/03/09, jdog5000: START
		(edited by K-Mod) */
	// Push players to get more coastal cities so they can build navies
	CvArea* pWaterArea = kPlot.waterArea(true);
	if (pWaterArea != NULL)
	{
		//120 + (kSet.isSeafaring() ? 160 : 0);
		r += (kSet.isSeafaring() ? 140 : 80); // advc.031
		if (kTeam.AI_isWaterAreaRelevant(*pWaterArea) &&
			/*  advc.031: Don't worry about coastal production if we
				already have many coastal cities. */
			kPlayer.countNumCoastalCities() <= iCities / 3)
		{
			//r += 120 + (kSet.isSeafaring() ? 160 : 0);
			r += (kSet.isSeafaring() ? 240 : 125); // advc.031
			/*if (kPlayer.countNumCoastalCities() < iCities / 4 ||
				kPlayer.countNumCoastalCitiesByArea(kPlot.getArea()) == 0)*/
			if (// advc.031: Disabled this clause
				//kPlayer.countNumCoastalCities() < iCities / 4 ||
				(kPlot.getArea().getCitiesPerPlayer(ePlayer) > 0 &&
				kPlayer.countNumCoastalCitiesByArea(kPlot.getArea()) == 0))
			{
				r += 200;
			}
		}
	}  // <advc.031>
	// Modify based on production (since the point is to build a navy)
	double mult = productionModifier;
	// That's the name of the .py file; not language-dependent.
	if (GC.getInitCore().getMapScriptName().compare(L"Pangaea") == 0)
		mult /= 2;
	r = ::round(r * mult);
	// Encourage canals
	int iCanal = 0;
	if (pWaterArea != NULL &&
		/*  ... but not if there is so little land that some city
			will probably create a canal in any case */
		iLandTiles >= 8)
	{
		CvArea* pWater2 = kPlot.secondWaterArea();
		if (pWater2 != NULL && pWater2 != pWaterArea)
		{
			int iSz1 = pWaterArea->getNumTiles();
			int iSz2 = pWater2->getNumTiles();
			int iSizeFactor = std::min(30, std::min(iSz1, iSz2));
			if (iSizeFactor >= GC.getDefineINT(CvGlobals::MIN_WATER_SIZE_FOR_OCEAN))
			{
				r += 9 * iSizeFactor;
				IFLOG logBBAI("Connecting waterbodies of at least size %d", iSizeFactor);
			}
		}
	} // </advc.031>
	r += 50; // advc: as in K-Mod (was 200 in BBAI)
	// BETTER_BTS_AI_MOD: END
	IFLOG logBBAI("+%d from coastal", r);
	return r;
}

/*  advc: Should perhaps merge this with the diploFactor code in adjustToCivSurroundings.
	Could then also add a chokepoint evaluation. */
int AIFoundValue::evaluateDefense() const
{
//EFCTION BELOW SUGGESTED BY F1RPO - but, prefered not t o give peaks weight wheb eval of def.
//original f1rpo code below
/*   int r = 0;
   if (kPlot.isHills())
   {*/
       /*  advc.031: Was 100+100 in K-Mod, 200 flat in BtS. Reduced b/c
           counted again for diploFactor below. */
   /*    r += 75 + (kSet.isDefensive() ? 75 : 0);
   }*/
//Mountain mod - addition by f1rpo
 /*  if (kGame.isOption(GAMEOPTION_MOUNTAINS))
   {
       FOR_EACH_ENUM(Direction)
       {
           CvPlot const* pAdj = GC.getMap().plotDirection(eLoopDirection);
           if (pAdj != NULL && pAdj->isPeak())
               r += (kSet.isDefensive() ? 15 : 8);
       }
   }
   IFLOG if(r>0) logBBAI("+%d from defensive terrain", r);

   return r;*/
   int r = 0;
   if (kPlot.isHills())
   {
	     /*  advc.031: Was 100+100 in K-Mod, 200 flat in BtS. Reduced b/c
			counted again for diploFactor below. */
	   r += 75 + (kSet.isDefensive() ? 75 : 0);
	   IFLOG logBBAI("+%d from hill defense", r);
   }
   return r;	
}

// Taking into account tiles beyond the city radius
int AIFoundValue::adjustToStartingSurroundings(int iValue) const
{
	int r = iValue;
	int iGreaterBadTile = 0;
	int const iRange = 6; // K-Mod (was 5)
	for (SquareIter it(kPlot, iRange); it.hasNext(); ++it)
	{
		CvPlot const& p = *it;
		if ((p.isWater() || p.isArea(kArea)) && it.currPlotDist() <= iRange)
		{
			/*int iTempValue = (p->getYield(YIELD_FOOD) * 15);
			iTempValue += (p->getYield(YIELD_PRODUCTION) * 11);
			iTempValue += (p->getYield(YIELD_COMMERCE) * 5);
			r += iTempValue;
			if (iTempValue < 21) {
				iGreaterBadTile += 2;
				if (p->isFeature()) {
					if (p->calculateBestNatureYield(YIELD_FOOD, eTeam) > 1)
						iGreaterBadTile--;
				}
			}*/ // BtS
			// K-Mod
			int iTempValue = 0;
			iTempValue += p.getYield(YIELD_FOOD) * 9;
			iTempValue += p.getYield(YIELD_PRODUCTION) * 5;
			iTempValue += p.getYield(YIELD_COMMERCE) * 3;
			iTempValue += p.isRiver() ? 1 : 0;
			iTempValue += p.isWater() ? -2 : 0;
			if (iTempValue < 13)
			{
				// 3 points for unworkable plots (desert, ice, far-ocean)
				// 2 points for bad plots (ocean, tundra)
				// 1 point for fixable bad plots (jungle)
				iGreaterBadTile++;
				if (p.calculateBestNatureYield(YIELD_FOOD, eTeam) < 2)
				{
					iGreaterBadTile++;
					if (iTempValue <= 0)
						iGreaterBadTile++;
				}
			}
			if (p.isWater() || p.isArea(kArea))
				r += iTempValue;
			else if (iTempValue >= 13)
				iGreaterBadTile++; // add at least 1 badness point for other islands.
			// K-Mod end
		}
	}
	IFLOG logBBAI("+%d from surroundings", r - iValue);
	if (bNormalize)
		return r; // advc
	/*iGreaterBadTile /= 2;
	if (iGreaterBadTile > 12) {
		r *= 11;
		r /= iGreaterBadTile;
	}*/ // BtS
	// K-Mod. note: the range has been extended, and the 'bad' counting has been rescaled.
	iGreaterBadTile /= 3;
	int iGreaterRangePlots = 2*(iRange*iRange + iRange) + 1;
	int iGreaterRangeFactor = iGreaterRangePlots / 6; // advc
	if (iGreaterBadTile > iGreaterRangeFactor)
	{
		r *= iGreaterRangeFactor;
		r /= iGreaterBadTile;
		IFLOG logBBAI("Times %d/%d for bad tiles in the greater range", iGreaterRangeFactor, iGreaterBadTile);
	}

	// Maybe we can make a value adjustment based on the resources and players currently in this area
	// (wip)
	// advc.031: Deleted. This should be the responsibility of CvPlayer::findStartingAreas.

	/*  advc: Unused BtS and K-Mod code dealing with WaterCount and
		MinOriginalStartDist deleted */

	int const iTempValue = r; // advc.031c
	int iMinDistanceFactor = MAX_INT;
	int const iMinRange = //startingPlotRange();
			kGame.getStartingPlotRange(); // advc.opt: Now precomputed
	//r *= 100; // (disabled by K-Mod to prevent int overflow)
	for (PlayerIter<CIV_ALIVE> it; it.hasNext(); ++it)
	{
		CvPlayer const& kOther = *it;
		if (kOther.getID() == ePlayer)
			continue;
		int iClosenessFactor = kOther.startingPlotDistanceFactor(kPlot, ePlayer, iMinRange);
		iMinDistanceFactor = std::min(iClosenessFactor, iMinDistanceFactor);
		if (iClosenessFactor < 1000)
		{
			/*r *= 2000 + iClosenessFactor;
			r /= 3000;*/
			// advc.031: If overflow is a concern ...
			r = ::round(r * ((2000 + iClosenessFactor) / 3000.0));
		}
	}
	if (iMinDistanceFactor > 1000)
	{
		//give a maximum boost of 25% for somewhat distant locations, don't go overboard.
		iMinDistanceFactor = std::min(1500, iMinDistanceFactor);
		r *= (1000 + iMinDistanceFactor);
		r /= 2000;
	}
	else if (iMinDistanceFactor < 1000)
	{
		//this is too close so penalize again.
		r *= iMinDistanceFactor;
		r /= 1000;
		r *= iMinDistanceFactor;
		r /= 1000;
	}
	IFLOG logBBAI("%d from distance to other players", r - iTempValue);
	return r;
}


int AIFoundValue::adjustToProduction(int iValue, int iBaseProductionTimes100) const
{
	// K-Mod. reduce value of cities which will struggle to get any productivity.
	// <advc.040>
	if(bFirstColony)
	{
		iBaseProductionTimes100 += std::max(iUnrevealedTiles * 50,
				(iBaseProductionTimes100 * iUnrevealedTiles) / NUM_CITY_PLOTS);
	} // </advc.040>
	// <advc.031> 
	iBaseProductionTimes100 = std::max(iBaseProductionTimes100,
			100 * GC.getInfo(YIELD_PRODUCTION).getMinCity()); // </advc.031>
	FAssert(!isRevealed(kPlot) ||
			// advc.test: I've seen this fail when there are Gems under Jungle (fixme)
			iBaseProductionTimes100 >= 100 * GC.getInfo(YIELD_PRODUCTION).getMinCity());
	int iThreshold = 850; // advc.031: was 900
	// <advc.303> Can't expect that much production from just the inner ring.
	if(bBarbarian)
		iThreshold = 400; // </advc.303>
	if (iBaseProductionTimes100 < iThreshold)
	{
		iValue = ::round(iValue * iBaseProductionTimes100 / iThreshold);
		IFLOG logBBAI("Times %d percent for low production (%d/100)",
				::round(iBaseProductionTimes100*100 / (double)iThreshold), iBaseProductionTimes100);
	} // K-Mod end
	return iValue;
}


int AIFoundValue::adjustToBarbarianSurroundings(int iValue) const
{
	int r = iValue;
	int const iRange = kSet.getBarbarianDiscouragedRange();
	if (iRange <= 0)
		return r;
	CvCity* pNearestCity = GC.getMap().findCity(iX, iY, NO_PLAYER);
	/*  <advc.303>, advc.300. Now that the outer ring isn't counted, I worry
		that an absolute penalty would reduce iValue to 0 too often. Also want
		to discourage touching borders a bit more, which the relative penalty
		should accomplish better.
		I don't see the need for special treatment of nearest city in a
		different area. CvGame avoids settling uninhabited areas anyway. */
	if (pNearestCity == NULL)
		pNearestCity = GC.getMap().findCity(iX, iY, NO_PLAYER, NO_TEAM, false);
	if (pNearestCity != NULL)
	{
		r *= std::min(iRange, plotDistance(iX, iY,
				pNearestCity->getX(), pNearestCity->getY()));
		r /= iRange;
		IFLOG if(iValue!=r) logBBAI("%d from %S being near a Barbarian site (discouraged range: %d)",
				r - iValue, cityName(*pNearestCity), iRange);
	}
	/*if (pNearestCity)
		r -= (std::max(0, (8 - plotDistance(iX, iY, pNearestCity->getX(), pNearestCity->getY()))) * 200);
	else {
		pNearestCity = GC.getMap().findCity(iX, iY, NO_PLAYER, NO_TEAM, false);
		if (pNearestCity != NULL) {
			int iDistance = plotDistance(iX, iY, pNearestCity->getX(), pNearestCity->getY());
			r -= std::min(500 * iDistance, (8000 * iDistance) / GC.getMap().maxPlotDistance());
		}
	}*/ // </advc.303>
	return r;
}


int AIFoundValue::adjustToCivSurroundings(int iValue, int iStealPercent) const
{
	// K-Mod. Adjust based on proximity to other players, and the shape of our empire.
	int iForeignProximity = 0;
	bool bFreeForeignCulture = false; // advc.031
	int iOurProximity = 0;
	CvCity const* pOurNearestCity = NULL;
	int iMaxDistanceFromCapital = 0;
	for (PlayerIter<CIV_ALIVE,KNOWN_TO> it(eTeam); it.hasNext(); ++it)
	{
		CvPlayer const& kOther = *it;
		if (kArea.getCitiesPerPlayer(kOther.getID()) <= 0 ||
				GET_TEAM(kOther.getTeam()).isVassal(eTeam))
			continue;
		int iProximity = 0;
		FOR_EACH_CITY(pLoopCity, /* *this */ kOther) // advc.001: Not this player!
		{
			if (kOther.getID() == ePlayer && pCapital != NULL)
			{
				iMaxDistanceFromCapital = std::max(iMaxDistanceFromCapital,
						plotDistance(pCapital->plot(), pLoopCity->plot()));
			}
			if (pLoopCity->isArea(kArea))
			{
				// <advc.031> Don't cheat
				if (!kSet.isAllSeeing() && !kTeam.AI_deduceCitySite(pLoopCity))
					continue; // </advc.031>
				int iDistance = plotDistance(iX, iY, pLoopCity->getX(), pLoopCity->getY());
				if (kOther.getID() == ePlayer && (pOurNearestCity == NULL ||
					iDistance < plotDistance(iX, iY,
					pOurNearestCity->getX(), pOurNearestCity->getY())))
				{
					pOurNearestCity = pLoopCity;
				}
				int iCultureRange = pLoopCity->getCultureLevel() + 3;
				if (iDistance <= iCultureRange && kTeam.AI_deduceCitySite(pLoopCity))
				{
					// cf. culture distribution in CvCity::doPlotCultureTimes100
					iProximity += 90*(iDistance-iCultureRange)*(iDistance-iCultureRange)/
							(iCultureRange*iCultureRange) + 10;
				}
			}
		}
		if (kOther.getTeam() == eTeam)
			iOurProximity = std::max(iOurProximity, iProximity);
		else if (iProximity > iForeignProximity)
		{
			iForeignProximity = iProximity;
			// advc.031:
			bFreeForeignCulture = (kOther.getFreeCityCommerce(COMMERCE_CULTURE) > 1);
		}
	}
	// Reduce the value if we are going to get squeezed out by culture.
	// Increase the value if we are hoping to block the other player!
	if (iForeignProximity > 0)
	{
		/*	As a rough guide of scale, settling 3 steps from a level 2 city
			in isolation would give a proximity of 24.
			4 steps from a level 2 city = 13
			4 steps from a level 3 city = 20 */
		int iDelta = iForeignProximity - iOurProximity;
		IFLOG logBBAI("Proximity difference (foreign minus ours): %d - %d = %d", iForeignProximity, iOurProximity, iDelta);
		if (iDelta > 50)
		{
			IFLOG logBBAI("Site disregarded: proximity difference too great");
			return 0; // we'd be crushed and eventually flipped if we settled here.
		}
		int const iTempValue = iValue; // advc.031
		bool bEasyCulture = (!bFreeForeignCulture && kSet.isEasyCulture()); // advc.031
		if (iDelta > -20 && iDelta <= (kSet.isAmbitious() ? 10 : 0) *
			(bEasyCulture ? 2 : 1))
		{
			/*	we want to get this spot before our opponents do.
				The lower our advantage, the more urgent the site is. */
			iValue *= 120 + iDelta/2 + (kSet.isAmbitious() ? 5 : 0)
					// advc.031: The 2nd city should focus more on high yields
					- (iCities <= 1 ? 12 : 0);
			iValue /= 100;
			/*  <advc.031> Don't rush to settle marginal spots (which might
				not even make the MinFoundValue cut w/o the boost above). */
			if (iTempValue < 2000)
			{
				iValue *= iTempValue;
				iValue /= 2000;
				iValue = std::max(iTempValue, iValue);
			} // </advc.031>
		}
		iDelta -= bEasyCulture ? 20 : 10;
		if (iDelta > 0)
		{
			iValue *= 100 - iDelta*3/2;
			iValue /= 100;
		}
		IFLOG if(iValue!=iTempValue) logBBAI("%d from foreign proximity", iValue - iTempValue);
		/*  <advc.031> This is not about being squeezed, but squeezing others
			and thereby angering them. StealPercent says how much we
			squeeze them (cultural strength is already taken into account). */
		if (iStealPercent >= 100)
		{
			/*  Between 130 (Alexander, G. Khan, Louis, Montezuma) and
				80 (Gandhi, Joao, Justinian). A leader who likes limited war
				should be less concerned about creating border troubles. */
			double diploFactor = (kPlayer.isHuman() && !kSet.isDebug() ? 100 :
					GC.getInfo(kPlayer.getPersonalityType()).getLimitedWarPowerRatio());
			if (kSet.isDefensive())
				diploFactor += 33;
			if (kPlot.isHills())
				diploFactor += 16;
			// The importance of a few stolen tiles decreases over time
			diploFactor += eEra * 13;
			diploFactor = 1.6 * diploFactor / iStealPercent;
			diploFactor = ::dRange(diploFactor, 0.6, 1.0);
			iValue = ::round(iValue * diploFactor);
			IFLOG logBBAI("Times %d percent (diplo modifier) from stealing %d/100 tiles",
					::round(100 * diploFactor), iStealPercent);
		} // </advc.031>
	}  // <advc.108> Avoid moving the starting settler far on crowded maps
	else if (iCities <= 0 && !kSet.isStartingLoc() &&
		kPlayer.getStartingPlot() != NULL && &kPlot != kPlayer.getStartingPlot())
	{
		int iCivAlive = PlayerIter<CIV_ALIVE>::count();
		int iRecommended = kGame.getRecommendedPlayers();
		if (iCivAlive > iRecommended && iRecommended > 0)
		{
			int iDistFromStart = plotDistance(kPlayer.getStartingPlot(), &kPlot);
			if (iDistFromStart > 1)
			{
				double multiplier = iRecommended / (double)
						(iCivAlive + std::min(iDistFromStart, 5) - 1);
				IFLOG logBBAI("Times %d percent for moving starting Settler %d tiles on a crowded map",
						::round(100 * multiplier), iDistFromStart);
				iValue = ::round(iValue * multiplier);
			}
		}
	} // </advc.108>
	// K-Mod end (the rest is original code - but I've made some edits...)

	if (pOurNearestCity != NULL)
	{
		int const iTempValue = iValue; // advc.031c
		int iDistance = ::plotDistance(&kPlot, pOurNearestCity->plot());
		/*  advc: BtS code dealing with iDistance deleted;
			K-Mod comment: Close cities are penalised in other ways */
		// K-Mod.
		/*  advc.031: Make expansive leaders indifferent about iDistance=5 vs.
			iDifference=6, but don't encourage iDistance>6. */
		int const iTargetRange = 5;//(kSet.isExpansive() ? 6 : 5);
		if (iDistance > iTargetRange +
			(kSet.isExpansive() ? 1 : 0)) // advc.031
		{	// with that max distance, we could fit a city in the middle!
			//iValue -= std::min(iTargetRange, iDistance - iTargetRange) * 400;
			int const iExcessDistance = std::min(iTargetRange, iDistance - iTargetRange);
			/*	<advc.031> Penalizing distance like this will lead to cities that aren't
				locally optimal. Can't really do anything about that here. Should perhaps
				be handled by AI_updateCitySites instead. */
			if (iExcessDistance > 0)
			{
				iValue -= 150;
				if (iExcessDistance > 1)
					iValue -= 250;
				if (iExcessDistance > 2)
					iValue -= (iExcessDistance - 2) * 325;
			} // </advc.031>
		}
		iValue *= 8 + 4*iCities;
		// 5, not iTargetRange, because 5 is better. (advc: iTargetRange is 5 now)
		iValue /= 2 + 4*iCities + std::max(iTargetRange, iDistance);
		IFLOG if(iTempValue!=iValue) logBBAI("%d from %d distance to %S",
				iValue - iTempValue, iDistance, cityName(*pOurNearestCity));

		if (!pOurNearestCity->isCapital() && pCapital != NULL)
		// K-Mod end
		{
			/*  Provide up to a 50% boost to value (80% for adv.start) for
				city sites which are relatively close to the core compared
				with the most distant city from the core (having a boost
				rather than distance penalty avoids some distortion).
				This is not primarily about maintenance but more about empire
				shape as such[, so] forbidden palace/state property are not
				[a] big deal. */ // advc: "so" added b/c the comment didn't make sense to me
			int iDistanceToCapital = ::plotDistance(pCapital->plot(), &kPlot);
			FAssert(iMaxDistanceFromCapital > 0);
			/*iValue *= 100 + (((kSet.isAdvancedStart() ? 80 : 50) * std::max(0, (iMaxDistanceFromCapital - iDistance))) / iMaxDistanceFromCapital);
			iValue /= 100;*/ // BtS
			/*  K-Mod. just a touch of flavour. (note, for a long time this
				adjustment used iDistance instead of iDistanceToCapital; and
				so I've reduced the scale to compensate) */
			/*int iShapeWeight = kSet.isAdvancedStart() ? 50 : (kSet.isAmbitious() ? 15 : 30);
			iValue *= 100 + iShapeWeight * std::max(0, iMaxDistanceFromCapital - iDistanceToCapital) / iMaxDistanceFromCapital;
			iValue /= 100 + iShapeWeight;*/
			// K-Mod end
			// <advc> I'm folding this into a single multiplier for easier debugging
			double const shapeWeight = (kSet.isAdvancedStart() ? 0.5 :
					(kSet.isAmbitious() ? 0.15 : 0.3));
			double shapeModifier = (1 + shapeWeight * std::max(0,
					iMaxDistanceFromCapital - iDistanceToCapital) /
					iMaxDistanceFromCapital) / (1 + shapeWeight);
			iValue = ::round(iValue * shapeModifier); // </advc>
			IFLOG logBBAI("Times %d percent (shape modifier); distance from capital: %d, max. distance: %d",
					::round(100 * shapeModifier), iDistanceToCapital, iMaxDistanceFromCapital);
		}
		return iValue;
	}
	pOurNearestCity = GC.getMap().findCity(iX, iY, ePlayer, eTeam, false);
	if (pOurNearestCity == NULL)
		return iValue;

	int iDistance = /* advc.031: */ std::min(GC.getMap().maxMaintenanceDistance(),
			::plotDistance(iX, iY, pOurNearestCity->getX(), pOurNearestCity->getY()));
	// <advc.031> Don't discourage settling on small nearby landmasses
	if(pCapital == NULL || pCapital->isArea(kArea) ||
		::plotDistance(&kPlot, pCapital->plot()) >= 10 ||
		kArea.getNumTiles() >= NUM_CITY_PLOTS)
	{
		int iDistPenalty = 5100 - std::min<int>(4, eEra) * 775; // (was 8000 flat)
		// </advc.031> (no functional change below)
		iDistPenalty *= iDistance;
		iDistPenalty /= GC.getMap().maxTypicalDistance(); // advc.140: was maxPlotDistance
		iDistPenalty = std::min(500 * iDistance, iDistPenalty);
		iValue -= iDistPenalty;
		IFLOG logBBAI("%d from distance penalty (%d distance to %S)", iDistPenalty, iDistance, cityName(*pOurNearestCity));
	}

	return iValue;
}


int AIFoundValue::adjustToCitiesPerArea(int iValue) const
{
	// <advc.130v>
	if (kTeam.isCapitulated() || iCities <= 0)
		return iValue; // </advc.130v>

	if (kArea.countCivCities() == 0) // advc.031: Had been counting Barbarian cities
	{
		//iValue *= 2;
		// K-Mod: presumably this is meant to be a bonus for being the first on a new continent.
		// But I don't want it to be a bonus for settling on tiny islands, so I'm changing it.
		int const iModifier = range(100 * (kArea.//getNumTiles()
				getNumRevealedTiles(eTeam) // advc.031: Don't cheat
				- 15) / //15
				20 // req. 40 revealed tiles for the full bonus
				, 100, 200);
		iValue *= iModifier;
		iValue /= 100;
		// K-Mod end
		IFLOG if(iModifier!=100) logBBAI("Times %d for being the first to colonize this landmass", iModifier);
	}
	/*  advc.031: BtS code deleted that was supposed to discourage colonies on
		the home continents of other civs. */
	return iValue;
}


int AIFoundValue::adjustToBonusCount(int iValue,
	std::vector<int> const& aiBonusCount) const
{
	if (iCities > 0)
	{
		int iBonusCount = 0;
		/*  <advc.052> Count bonus in the city tile double, as settling on a bonus
			is especially greedy. (I think this section is about not grabbing all
			the resources with a single city when there are a lot of resources
			in one place.) */
		if (getBonus(kPlot) != NO_BONUS)
			iBonusCount++; // </advc.052>
		int iUniqueBonusCount = 0;
		FOR_EACH_ENUM(Bonus)
		{
			iBonusCount += aiBonusCount[eLoopBonus];
			iUniqueBonusCount += (aiBonusCount[eLoopBonus] > 0 ? 1 : 0);
		}
		double modifier = 1; // advc: No functional change except rounding
		if (iBonusCount > 4) 
			modifier *= 5.0 / (1 + iBonusCount);
		/*else if (iUniqueBonusCount > 2) {
			iValue *= 5;
			iValue /= (3 + iUniqueBonusCount);
		}*/
		/*  <advc.031> I can see how multiple bonuses of the same type
			could help city specialization and thus shouldn't be discouraged as much,
			but iBonus < 5 (unique or not) shouldn't be discouraged at all. */
		if (iBonusCount + iUniqueBonusCount >= 10)
		{
			modifier *= std::max(0.7, (1 - 0.08 *
					(iBonusCount + iUniqueBonusCount - 9)));
		}
		iValue = ::round(iValue * modifier);
		IFLOG if(::round(100*modifier)!=100) logBBAI("Times %d percent for high resource count (%d resources, %d unique)",
				::round(100 * modifier), iBonusCount, iUniqueBonusCount);
	}
	if (!bBarbarian) // advc.303
	{
		int iDeadLockCount = countDeadlockedBonuses();
		if (kSet.isAdvancedStart() && iDeadLockCount > 0)
			iDeadLockCount += /*advc.031 (was 2):*/ 1;
		//iValue /= (1 + iDeadLockCount);
		// advc.031: Replacing the above, which is too harsh.
		iValue = (2 * iValue) / (2 + iDeadLockCount);
		IFLOG if(iDeadLockCount!=0) logBBAI("Times %d/%d for %d deadlocked resources", 2, 2 + iDeadLockCount, iDeadLockCount);
	}
	return iValue;
}


int AIFoundValue::adjustToBadTiles(int iValue, int iBadTiles) const
{
	int r = iValue;
	// <advc.040>
	if(bFirstColony)
		iBadTiles += iUnrevealedTiles / 2; // </advc.040>
	// <advc.031>
	if(iBadTiles > 0)
	{
		/*	A scenario is more likely to mix some very good tiles with a lot of
			bad ones. Better to be more conservative on regular maps. */
		double const exponent = (kGame.isScenario() ? 1.385 : 1.5);
		r -= ::round(std::pow((double)iBadTiles, exponent) *
				(35.0 + (kSet.isStartingLoc() ?
				50 : 0) + (iCities <= 0 ? 50 : 0))); // advc.108
		r = std::max(0, r);
	} // </advc.031>
	IFLOG if(r!=iValue) logBBAI("%d from %d bad tiles too many", r - iValue, iBadTiles);
	return r;
}

// <advc.031> Stifling bad health needs to be discouraged rigorously
int AIFoundValue::adjustToBadHealth(int iValue, int iGoodHealth) const
{
	int iBonusHealth = 0;
	if (pCapital != NULL)
		iBonusHealth += pCapital->getBonusGoodHealth();
	if (iCities > 0) // Connection to the capital can be slow/costly in the early game
		iBonusHealth = std::min(iBonusHealth, (iBonusHealth * (1 + iCities)) / 4);
	int iBadHealth = -iGoodHealth/100 - iBonusHealth -
			GC.getInfo(kPlayer.getHandicapType()).getHealthBonus();
	if (iBadHealth >= -2) // I.e. can only grow to size 2
	{
		int iDiv = std::max(1, 3 - eEra + iBadHealth);
		int iMult = 1;
		if (iDiv <= 1)
		{
			iMult = 2;
			iDiv = 3;
		}
		iValue = (iMult * iValue) / iDiv;
		IFLOG if (iDiv>1) logBBAI("Times %d/%d for bad health", iMult, iDiv);
	}
	return iValue;
} // </advc.031>

// advc: Moved from CvPlayerAI since it's only used for computing found values
int AIFoundValue::countDeadlockedBonuses() const
{
	int r = 0;
	int const iMinRange = GC.getDefineINT(CvGlobals::MIN_CITY_RANGE); // 2
	int const iRange = iMinRange * 2;
	for (SquareIter it(kPlot, iRange); it.hasNext(); ++it)
	{
		if (it.currPlotDist() > CITY_PLOTS_RADIUS)
		{
			// <advc.031>
			if (!isRevealed(*it))
				continue; // </advc.031>
			// <advc> Checks moved into subroutine
			if(isDeadlockedBonus(*it, iMinRange))
				r++; // </advc>
		}
	}
	return r;
}

// advc: Cut from countDeadlockedBonuses
bool AIFoundValue::isDeadlockedBonus(CvPlot const& kBonusPlot, int iMinRange) const
{
	if(getBonus(kBonusPlot) == NO_BONUS || kBonusPlot.isCityRadius())
		return false;
	// advc: kArea is the area of the first potential city site (kPlot) near kBonusPlot
	if(!kBonusPlot.isArea(kArea) && !kBonusPlot.isWater())
		return false;
	bool bCanFound = false;
	bool bNeverFound = true;
	//look for a city site [kOtherSite] within a city radius [around kBonusPlot]
	for (CityPlotIter it(kBonusPlot); it.hasNext(); ++it)
	{
		CvPlot const& kOtherSite = *it;
		// <advc.031> Won't want to settle on top of another resource
		if(getBonus(kOtherSite) != NO_BONUS)
			continue; // </advc.031>
		// <advc.031> Need to be able to see if there's land
		if (!isRevealed(kOtherSite) && !kOtherSite.isAdjacentRevealed(eTeam))
			continue; // </advc.031>
		//canFound usually returns very quickly
		if (kPlayer.canFound(kOtherSite.getX(), kOtherSite.getY(),
				/*  advc.031: Was false; whether to check visibility of cities that
					prevent founding in kOtherSite. */
				kSet.isAllSeeing()))
		{
			bNeverFound = false;
			if (stepDistance(&kPlot, &kOtherSite) > iMinRange ||
				// advc.031: No distance restriction for cities in different land areas
				!kPlot.sameArea(kOtherSite))
			{
				bCanFound = true;
				break;
			}
		}
	}
	return (!bNeverFound && !bCanFound);
}

// <advc.031c>
void CitySiteEvaluator::logSettings() const
{
	logBBAI("Found parameters for %S:", isStartingLoc() ?
			L"starting location" : getPlayer().debugName());
	logBBAI("Culture claim treshold: %d", getClaimThreshold());
	if (getMinRivalRange() != -1)
		logBBAI("MinRivalRange: %d", getMinRivalRange());
	// <advc.300>
	if (getBarbarianDiscouragedRange() != iDEFAULT_BARB_DISCOURAGED_RANGE)
		logBBAI("BarbarianDiscouragedRange: %d", getBarbarianDiscouragedRange());
	// </advc.300>
	if (isStartingLoc())
		logBBAI("StartingLoc");
	// <advc.007>
	if (isDebug())
		logBBAI("Ignoring other sites"); // </advc.007>
	if (isAllSeeing())
		logBBAI("All-seeing");
	if (isAdvancedStart())
		logBBAI("in Advanced Start");
	if (isEasyCulture())
		logBBAI("Easy culture");
	if (isAmbitious())
		logBBAI("Ambitious");
	if (isExtraCommerceThreshold())
		logBBAI("Financial");
	if (isDefensive())
		logBBAI("Defensive");
	if (isSeafaring())
		logBBAI("Seafaring");
	if (isExpansive())
		logBBAI("Expansive (tall)");
}

void AIFoundValue::logSite() const
{
	logBBAI("Computing found value for %S", kPlot.debugStr());
	if (bCoastal)
		logBBAI("Site is coastal");
	if (!kSet.isStartingLoc())
		logBBAI("%d other %S cities in the area, %d in total", iAreaCities, kPlayer.debugCivDescr(), iCities);
}

void AIFoundValue::logPlot(CvPlot const& p, int iPlotValue, int const* aiYield,
	int iCultureModifier, BonusTypes eBonus, ImprovementTypes eBonusImprovement,
	bool bCanTradeBonus, bool bCanSoonTradeBonus, bool bCanImproveBonus,
	bool bCanSoonImproveBonus, bool bEasyAccess,
	int iFeatureProduction, bool bPersistentFeature, bool bRemovableFeature) const
{
	int const F = YIELD_FOOD, P = YIELD_PRODUCTION, C = YIELD_COMMERCE;
	if (isHome(p))
		logBBAI("Home plot: val=%d, %dF%dP%dC", iPlotValue, aiYield[F], aiYield[P], aiYield[C]);
	else logBBAI("Plot in radius: %S; val=%d, %dF%dP%dC", p.debugStr(), iPlotValue, aiYield[F], aiYield[P], aiYield[C]);
	if (iCultureModifier != 100)
		logBBAI("Culture modifier: %d", iCultureModifier);
	if (eBonus != p.getBonusType())
	{
		FAssert(eBonus == NO_BONUS);
		logBBAI("Bonus resource hidden");
	}
	if (eBonus != NO_BONUS)
	{
		// Try not to output redundant information here ...
		if (eBonusImprovement == NO_IMPROVEMENT)
		{
			logBBAI("Can't improve resource");
			FAssert(!bCanImproveBonus && !bCanTradeBonus);
		}
		else
		{
			FAssert(bCanSoonImproveBonus);
			if (!bCanSoonTradeBonus)
				logBBAI("Can't connect resource");
			else if (!bCanTradeBonus)
				logBBAI("Can soon connect resource");
			if ((!bCanSoonTradeBonus || !bCanTradeBonus) && bCanImproveBonus)
				logBBAI("Can improve resource");
			else if (!bCanSoonTradeBonus && !bCanTradeBonus)
				logBBAI("Can soon improve resource");
		}
		FAssert(!bCanTradeBonus || bCanSoonTradeBonus);
		if (!bEasyAccess)
			logBBAI("Difficult to access");
	}
	if (isHome(p))
		return;
	if (iFeatureProduction != 0)
	{
		FAssert(!bPersistentFeature);
		logBBAI("Feature production: %d%s", iFeatureProduction, bRemovableFeature ? "" :
				" (reduced b/c can't remove yet)");
	}
	else if (bRemovableFeature)
	{
		FAssert(!bPersistentFeature);
		logBBAI("can remove feature");
	}
}

wchar const* AIFoundValue::cityName(CvCity const& kCity)
{
	static CvWString szName;
	szName = kCity.getName().GetCString();
	return szName;
}
// </advc.031c>
