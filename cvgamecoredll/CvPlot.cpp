#include "CvGameCoreDLL.h"
#include "CvPlot.h"
#include "CvPlotGroup.h"
#include "PlotRange.h"
#include "CoreAI.h"
#include "CvCityAI.h"
#include "CitySiteEvaluator.h"
#include "CvArea.h"
#include "CvUnit.h"
#include "FAStarFunc.h"//super forts needed//doto
#include "CvSelectionGroup.h"
#include "CvInfo_City.h"
#include "CvInfo_Terrain.h"
#include "CvInfo_GameOption.h"
#include "CvInfo_Civics.h" // for calculateImprovementYieldChange with bOptimal=true
#include "Trigonometry.h"
#include "CvDLLSymbolIFaceBase.h"
#include "CvDLLPlotBuilderIFaceBase.h"
#include "CvDLLFlagEntityIFaceBase.h"

/*	advc.make: I've added safeIntCast calls in a few places that looked at least
	slightly hazardous. Beyond that, explicit casts would only add clutter.
	NB: Disabling this warning entirely does unfortunately not only allow implicit
	conversions from larger to smaller int types but also float-to-int conversions. */
#pragma warning(disable: 244) // "type conversion: possible loss of data"

bool CvPlot::m_bAllFog = false; // advc.706
int CvPlot::m_iMaxVisibilityRangeCache = -1; // advc.003h
#define NO_BUILD_IN_PROGRESS (-2) // advc.011

// advc:
namespace
{
	__inline PlayerTypes getActivePlayer()
	{
		return GC.getGame().getActivePlayer();
	}
	__inline TeamTypes getActiveTeam()
	{
		return GC.getGame().getActiveTeam();
	}
}


CvPlot::CvPlot() // advc: Merged with the deleted reset function
{
	// BETTER_BTS_AI_MOD, Efficiency (plot danger cache), 08/21/09, jdog5000: START
	m_iActivePlayerSafeRangeCache = -1;
	// BETTER_BTS_AI_MOD: END

	m_pFeatureSymbol = NULL;
	m_pPlotBuilder = NULL;
	m_pRouteSymbol = NULL;
	m_pRiverSymbol = NULL;
	m_pFlagSymbol = NULL;
	m_pFlagSymbolOffset = NULL;
	m_pCenterUnit = NULL;
	m_szScriptData = NULL;
	m_szMostRecentCityName = NULL; // advc.005c
	// <advc.003s>
	m_paAdjList = NULL;
	m_iAdjPlots = 0; // </advc.003s>

	m_iX = 0;
	m_iY = 0;
	m_iPlotNum = static_cast<PlotNumInt>(NO_PLOT_NUM); // advc.opt
	m_pArea = NULL;
//DOTO Deliverator	
	m_iFreshWaterAmount = 0; // Deliverator
//keldath QA-DONE
	// < JCultureControl Mod Start >
	//m_aiCultureControl = NULL;
	// < JCultureControl Mod End >
// Super Forts *culture*
	//m_aiCultureRangeForts = NULL; // Super Forts *culture*
	m_iFeatureVariety = 0;
	m_iOwnershipDuration = 0;
	m_iImprovementDuration = 0;
	m_iUpgradeProgress = 0;
	m_iForceUnownedTimer = 0;
	m_iTurnsBuildsInterrupted = NO_BUILD_IN_PROGRESS; // advc.011
	m_iCityRadiusCount = 0;
	m_iRiverID = -1;
	m_iMinOriginalStartDist = -1;
	m_iReconCount = 0;
	m_iRiverCrossingCount = 0;
	m_iLatitude = -1; // advc.tsl
	m_iTotalCulture = 0; // advc.opt
	
// Super Forts begin *canal* *choke*
	m_iCanalValue = 0;
	m_iChokeValue = 0;
	// Super Forts end
	// Super Forts begin *bombard*
	m_iDefenseDamage = 0;
	m_bBombarded = false;
// Super Forts end

	m_bStartingPlot = false;
//keldath f1rpo-The relevant info is in m_ePlotType and can be accessed through isHills() and isPeak(); that was already the case in BtS. m_bHills was, essentially, unused.
//	m_bHills = false; 
//===NM=====Mountains Mod===0=====
//	m_bPeaks = false;
//===NM=====Mountains Mod===X=====
	m_bNOfRiver = false;
	m_bWOfRiver = false;
	m_bIrrigated = false;
	m_bImpassable = false; // advc.opt
	m_bAnyIsthmus = false; // advc.opt
	m_bPotentialCityWork = false;
	m_bShowCitySymbols = false;
	m_bFlagDirty = false;
	m_bPlotLayoutDirty = false;
	m_bLayoutStateWorked = false;
	m_eSecondOwner = NO_PLAYER; // advc.035
	m_eOwner = NO_PLAYER;
	// < JCultureControl Mod Start >
	m_eImprovementOwner = NO_PLAYER;
	// < JCultureControl Mod End >
	m_eTeam = NO_TEAM; // advc.opt
	m_ePlotType = PLOT_OCEAN;
	m_eTerrainType = NO_TERRAIN;
	m_eFeatureType = NO_FEATURE;
	m_eBonusType = NO_BONUS;
	m_eImprovementType = NO_IMPROVEMENT;
	m_eRouteType = NO_ROUTE;
	m_eRiverNSDirection = NO_CARDINALDIRECTION;
	m_eRiverWEDirection = NO_CARDINALDIRECTION;

	m_plotCity.reset();
	m_workingCity.reset();
	m_workingCityOverride.reset();
}


CvPlot::~CvPlot() // advc: Merged with the deleted uninit function
{
	SAFE_DELETE_ARRAY(m_szScriptData);
	SAFE_DELETE_ARRAY(m_szMostRecentCityName); // advc.005c
//keldath QA this was onCvPlot::uninit() - i think im using f1rpo method now...hope its a good merge.
	// < JCultureControl Mod Start >
	//SAFE_DELETE_ARRAY(m_aiCultureControl);
	// < JCultureControl Mod End >
// Super Forts *culture*
//	SAFE_DELETE_ARRAY(m_aiCultureRangeForts); // Super Forts *culture*
// Super Forts *culture*
	SAFE_DELETE_ARRAY(m_paAdjList); // advc.003s

	gDLL->getFeatureIFace()->destroy(m_pFeatureSymbol);
	if(m_pPlotBuilder != NULL)
		gDLL->getPlotBuilderIFace()->destroy(m_pPlotBuilder);
	gDLL->getRouteIFace()->destroy(m_pRouteSymbol);
	gDLL->getRiverIFace()->destroy(m_pRiverSymbol);
	gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
	gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);
	m_pCenterUnit = NULL;

	deleteAllSymbols();
	m_units.clear();
}


void CvPlot::init(int iX, int iY)
{
	m_iX = safeIntCast<short>(iX);
	m_iY = safeIntCast<short>(iY);
	updatePlotNum(); // advc.opt
	updateLatitude(); // advc.tsl
}

// advc.opt:
void CvPlot::updatePlotNum()
{
	if (getX() != INVALID_PLOT_COORD && getY() != INVALID_PLOT_COORD)
		m_iPlotNum = (PlotNumInt)GC.getMap().plotNum(getX(), getY());
}

// advc.003s:
void CvPlot::initAdjList()
{
	std::vector<CvPlot*> apAdjList;
	apAdjList.reserve(NUM_DIRECTION_TYPES);
	/*	To ensure that diagonally adjacent plots receive only odd indices
		(for fast loops over only orthogonal or only diagonal neighbors),
		enumerate the adjacent plots starting with the northern neighbor
		for one half of the map, and starting with the southern neighbor
		for the other half of the map. Continue clockwise in any case.
		Which half should start in the north depends on the world wrap.
		(It's the margins that cause problems.)
		[Alt. idea: Go N-W-E-S and NE-SE-SW-NW, alternating between these
		two sequences whenever a (non-NULL) plot has been added to m_paAdjList.] */
	CvMap const& kMap = GC.getMap();
	int const iWidth = kMap.getGridWidth();
	int const iHeight = kMap.getGridHeight();
	int iStartDirection = DIRECTION_NORTH;
	if (!kMap.isWrap())
	{
		// Diagonal split: southwestern half starts in the south
		if (getX() * iHeight > getY() * iWidth)
			iStartDirection = DIRECTION_SOUTH;
	}
	else if (kMap.isWrapX() && !kMap.isWrapY())
	{
		// Southern half starts in the south
		if (2 * getY() < iHeight)
			iStartDirection = DIRECTION_SOUTH;
		/*	If the map is a single row, we'll have plots
			with 2 orthogonal and 0 diagonal neighbors. */
		FAssert(iHeight > 1);
	}
	else if (kMap.isWrapY() && !kMap.isWrapX())
	{
		// Eastern half starts in the south
		if (2 * getX() > iWidth)
			iStartDirection = DIRECTION_SOUTH;
		/*	If the map is a single column, we'll have plots
			with 2 orthogonal and 0 diagonal neighbors. */
		FAssert(iWidth > 1);
	}
	//else ... // If there are no margins, then we can start wherever.

	FOR_EACH_ENUM(Direction)
	{
		DirectionTypes eOffsetDir = (DirectionTypes)
				((eLoopDirection + iStartDirection) % NUM_DIRECTION_TYPES);
		CvPlot* p = plotDirection(getX(), getY(), eOffsetDir);
		if (p != NULL)
			apAdjList.push_back(p);
	}
	m_iAdjPlots = (char)apAdjList.size();
	m_paAdjList = new CvPlot*[m_iAdjPlots];
	for (int i = 0; i < m_iAdjPlots; i++)
		m_paAdjList[i] = apAdjList[i];
	#ifdef FASSERT_ENABLE // advc.test
	/*FOR_EACH_ENUM(Direction)
	{
		CvPlot const* p = plotDirection(getX(), getY(), eLoopDirection);
		if (p == NULL)
			continue;
		bool bOrth = (p->getX() == getX() || p->getY() == getY());
		{
			int i;
			for (i = 0; i < numAdjacentPlots(); i++)
			{
				if (getAdjacentPlotUnchecked(i) == p)
					break;
			}
			FAssert(i % 2 == (bOrth ? 0 : 1));
		}
	}*/
	#endif
}


void CvPlot::setupGraphical()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
		return;

	updateSymbols();
	updateFeatureSymbol();
	updateRiverSymbol();
	updateMinimapColor();

	updateVisibility();
	updateCenterUnit(); // K-Mod (This is required now that CvMap::updateCenterUnit doesn't always update the whole map.)
}

void CvPlot::updateGraphicEra()
{
	if(m_pRouteSymbol != NULL)
		gDLL->getRouteIFace()->updateGraphicEra(m_pRouteSymbol);

	if(m_pFlagSymbol != NULL)
		gDLL->getFlagEntityIFace()->updateGraphicEra(m_pFlagSymbol);
}

void CvPlot::erase()
{
	CLinkList<IDInfo> oldUnits;
	{
		for (CLLNode<IDInfo> const* pUnitNode = headUnitNode(); pUnitNode != NULL;
			pUnitNode = nextUnitNode(pUnitNode))
		{
			oldUnits.insertAtEnd(pUnitNode->m_data);
		}
	}
	// kill units
	CLLNode<IDInfo>* pUnitNode = oldUnits.head();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = oldUnits.next(pUnitNode);
		if (pLoopUnit == NULL)
		{
			FAssertMsg(pLoopUnit != NULL, "Can this happen?"); // advc
			continue;
		}
		pLoopUnit->kill(false);
	}

	CvCity* pCity = getPlotCity();
	if (pCity != NULL)
		pCity->kill(false);

	setBonusType(NO_BONUS);
	setImprovementType(NO_IMPROVEMENT);
	setRouteType(NO_ROUTE, false);
	setFeatureType(NO_FEATURE);

	// disable rivers
	setNOfRiver(false, NO_CARDINALDIRECTION);
	setWOfRiver(false, NO_CARDINALDIRECTION);
	setRiverID(-1);
}


float CvPlot::getPointX() const
{
	return GC.getMap().plotXToPointX(getX());
}


float CvPlot::getPointY() const
{
	return GC.getMap().plotYToPointY(getY());
}


NiPoint3 CvPlot::getPoint() const
{
	NiPoint3 pt3Point;

	pt3Point.x = getPointX();
	pt3Point.y = getPointY();
	pt3Point.z = gDLL->getEngineIFace()->GetHeightmapZ(pt3Point);

	return pt3Point;
}


float CvPlot::getSymbolSize() const
{
	if (isVisibleWorked())
	{
		if (isShowCitySymbols())
			return 1.6f;
		return 1.2f;
	}
	else
	{
		if (isShowCitySymbols())
			return 1.2f;
		return 0.8f;
	}
}


float CvPlot::getSymbolOffsetX(int iOffset) const
{
	return 40.0f + iOffset * 28.0f * getSymbolSize() - GC.getPLOT_SIZE() / 2.0f;
}


float CvPlot::getSymbolOffsetY(int iOffset) const
{
	return -(GC.getPLOT_SIZE() / 2.0f) + 50.0f;
}


void CvPlot::doTurn()
{
	PROFILE_FUNC();

	if (getForceUnownedTimer() > 0)
		changeForceUnownedTimer(-1);

	if (isOwned())
		changeOwnershipDuration(1);
// Super Forts begin *upgrade*		
	if (!isFortImprovement())
	{
		if (isImproved())//org advc code
			changeImprovementDuration(1);
	}
	else
	{
// Super Forts begin *upgrade*
		if (getOwner() != NO_PLAYER)
		{
			if (isImproved() && isOwned()
				&& !GET_PLAYER(getOwner()).isAnarchy()//ADDDED - its a ctd if theres anarchy - 
				//advc merged doupgradeimprovement into doimprovement
				)//added own check to avoid cts
			{
				changeImprovementDuration(1);
				// Super Forts begin *upgrade*
				if (!isBeingWorked())
				{
					doImprovement(true);
					//doto adjustment - code crashes if this executes
					//advc merged the funcs but the part of the doimprovement crashes the game upon this execution
					//so i added a bool check so only the upgarde part will be done
					//doImprovementUpgrade();
				}
			// Super Forts end
			}
		}
// Super Forts end
// Super Forts begin *bombard*
		if (!isBombarded() && getDefenseDamage() > 0)
		{
			changeDefenseDamage(-(GC.getDefineINT("CITY_DEFENSE_DAMAGE_HEAL_RATE")));
		}
		setBombarded(false);
	}
// Super Forts end
	doFeature();
	doCulture();
	verifyUnitValidPlot();

	/*if (!isOwned())
		doImprovementUpgrade();*/ // advc (comment): Was already commented out in BtS

	// advc: This sounds pretty slow and I don't think I've ever needed it
	/*#ifdef _DEBUG // XXX
	FOR_EACH_UNIT_IN(pUnit, *this)
	{
		FAssertMsg(pUnit->at(*this), "pUnit is expected to be at the current plot instance");
	}*/
	//#endif // XXX
}

//super forts doto - added bool 
void CvPlot::doImprovement(bool bsuperForts)
{
	PROFILE_FUNC();
	
// doto add - Super Forts begin - check wihout bool - ctd on first turn
	if(!bsuperForts)
		FAssert(isBeingWorked());
// doto add - Super Forts begin - check wihout bool - ctd on first turn

	CvPlayer const& kOwner = GET_PLAYER(getOwner());
	FAssert(!kOwner.isAnarchy()); // advc

	if (isImproved() && getBonusType() == NO_BONUS)
	{
		FOR_EACH_ENUM(Bonus)
		{
			CvBonusInfo const& kLoopBonus = GC.getInfo(eLoopBonus);
			if (!GET_TEAM(kOwner.getTeam()).isHasTech(kLoopBonus.getTechReveal()))
				continue;
			/*if (GC.getInfo(getImprovementType()).getImprovementBonusDiscoverRand(eLoopBonus) > 0) { // BtS
				if (SyncRandOneChanceIn(GC.getInfo(getImprovementType()).getImprovementBonusDiscoverRand(eLoopBonus))) {*/
			// UNOFFICIAL_PATCH, Gamespeed scaling, 03/04/10, jdog5000: START
			int iOdds = GC.getInfo(getImprovementType()).
					getImprovementBonusDiscoverRand(eLoopBonus);
			if (iOdds <= 0)
				continue;
			// <advc.rom3>
			//Afforess: check for valid terrains for this bonus before discovering it
			if (!canHaveBonus(eLoopBonus), false, /* advc.129: */ true)
				continue; // </advc.rom3>
			iOdds *= GC.getInfo(GC.getGame().getGameSpeedType()).getVictoryDelayPercent();
			iOdds /= 100;
			if (SyncRandOneChanceIn(iOdds))
			{	// UNOFFICIAL_PATCH: END
				setBonusType(eLoopBonus);
				CvCity* pCity = GC.getMap().findCity(getX(), getY(), kOwner.getID(), NO_TEAM, false);
				if (pCity != NULL)
				{
					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_DISCOVERED_NEW_RESOURCE",
							kLoopBonus.getTextKeyWide(), pCity->getNameKey());
					gDLL->UI().addMessage(kOwner.getID(), false, -1, szBuffer, *this,
							"AS2D_DISCOVERBONUS", MESSAGE_TYPE_MINOR_EVENT, kLoopBonus.getButton());
				}
				break;
			}
		}
	}
	/*	advc: doImprovementUpgrade merged into doImprovement.
		Makes clearer which conditions are ensured by the caller. */
	if (!isImproved())
		return;
	ImprovementTypes eImprovementUpdrade = GC.getInfo(getImprovementType()).
			getImprovementUpgrade();
	// doto add - Super Forts begin *upgrade* - added if-else statement
	CvImprovementInfo &kImprovementInfo = GC.getImprovementInfo(getImprovementType());
	ImprovementTypes eImprovementUpgrade_sf = (ImprovementTypes)kImprovementInfo.getImprovementUpgrade();
	// doto add -Super Forts begin *upgrade* - added if-else statement

	//doto qa108 super forts 0 check mnai code here - its different!
	if (eImprovementUpdrade != NO_IMPROVEMENT)
	{
	   //CvImprovementInfo &kImprovementUpgradeInfo = GC.getImprovementInfo(eImprovementUpgrade_sf);
	// Super Forts begin *upgrade* - added if-else statement
		if(isFortImprovement() 
			&& kImprovementInfo.isUpgradeRequiresFortify())
		{
			bool bDefenderFound = false;
			CLinkList<IDInfo> oldUnits;
			CLLNode<IDInfo>* pUnitNode = headUnitNode();
			CvUnit* pLoopUnit;

			while (pUnitNode != NULL)
			{
				oldUnits.insertAtEnd(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);
			}

			pUnitNode = oldUnits.head();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);
				if(pLoopUnit->getFortifyTurns() > 0)
				{
					if (pLoopUnit->getTeam() == getTeam() && pLoopUnit->canDefend())
					{
						bDefenderFound = true;
						break;
					}
				}
			}

			if(bDefenderFound)
			{
				changeUpgradeProgress(GET_PLAYER(getOwner()).getImprovementUpgradeRate());

				if (getUpgradeProgress() >= GC.getGame().getImprovementUpgradeTime(getImprovementType()))
				{
					setImprovementType(eImprovementUpgrade_sf);
				}
			}
		}
		else
		{
		//original code: 
		/*	advc: Caller already ensures isBeingWorked (and isOwned()).
			In other words, improvement upgrades outside of borders
			aren't correctly implemented, and I'm not going to fix that. */
		//if (isBeingWorked() || GC.getInfo(eImprovementUpdrade).isOutsideBorders())
		changeUpgradeProgress(kOwner.getImprovementUpgradeRate());
		if (getUpgradeProgress() >= GC.getGame().
			getImprovementUpgradeTime(getImprovementType()) * 100)
		{
			setImprovementType(eImprovementUpdrade);
			// < JCultureControl Mod Start >
			if (getImprovementOwner() != NO_PLAYER && GC.getGame().isOption(GAMEOPTION_CULTURE_CONTROL))
			{
                      addCultureControl(getImprovementOwner(), eImprovementUpdrade, true);
			}
			// < JCultureControl Mod End >
		}
//super forts doto		
		}
	}
}

/*	advc (caveat): When this function is called in a loop for a range of plots,
	the order of traversal can be important b/c calculateCulturalOwner will grant
	ownership of a plot to a player that owns all surrounding plots.
	So far, this has only been a problem in CvCity::setCultureLevel. */
void CvPlot::updateCulture(bool bBumpUnits, bool bUpdatePlotGroups)
{
	PROFILE_FUNC(); // advc (There's a plot group update when the owner changes)
// Super Forts begin *culture*
	if(/*isCityExternal(true) */
		//doto change - added improvement specific check
		(isCity() ||
		(isFortImprovement()))
		/*removed by keldath- 
		caused pillage and surronding plots to not get culture
		|| (getOwner() != NO_PLAYER)*/)
		return;
// Super Forts end
	// <advc.035>
	PlayerTypes eCulturalOwner = calculateCulturalOwner();
	setSecondOwner(eCulturalOwner);
	if(GC.getDefineBOOL(CvGlobals::OWN_EXCLUSIVE_RADIUS) && eCulturalOwner != NO_PLAYER)
	{
		PlayerTypes eSecondOwner = calculateCulturalOwner(false, true);
		if(eSecondOwner != NO_PLAYER)
		{
			if(!GET_TEAM(eSecondOwner).isAtWar(TEAMID(eCulturalOwner)))
				eCulturalOwner = eSecondOwner;
			else setSecondOwner(eSecondOwner);
		}
		else
		{
			FAssertMsg(eSecondOwner != NO_PLAYER, "eCulturalOwner!=NO_PLAYER"
				" should imply eSecondOwner!=NO_PLAYER");
		}
	}
	setOwner(eCulturalOwner, // </advc.035>
			bBumpUnits, bUpdatePlotGroups);
}


void CvPlot::updateFog()
{
	//PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
		return;

	FAssert(getActiveTeam() != NO_TEAM);

	if (isRevealed(getActiveTeam()))
	{
		if (gDLL->UI().isBareMapMode())
			gDLL->getEngineIFace()->LightenVisibility(getFOWIndex());
		else
		{
			static bool const bCityScreenFogEnabled = GC.getDefineBOOL("CITY_SCREEN_FOG_ENABLED"); // advc.opt: static
			if (bCityScreenFogEnabled && gDLL->UI().isCityScreenUp() &&
					gDLL->UI().getHeadSelectedCity() != getWorkingCity())
				gDLL->getEngineIFace()->DarkenVisibility(getFOWIndex());
			else if (isActiveVisible(false))
				gDLL->getEngineIFace()->LightenVisibility(getFOWIndex());
			else gDLL->getEngineIFace()->DarkenVisibility(getFOWIndex());
		}
	}
	else gDLL->getEngineIFace()->BlackenVisibility(getFOWIndex());
}


void CvPlot::updateVisibility()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
		return;

	setLayoutDirty(true);

	updateSymbolVisibility();
	updateFeatureSymbolVisibility();
	updateRouteSymbol();

	CvCity* pCity = getPlotCity();
	if (pCity != NULL)
		pCity->updateVisibility();
	updateCenterUnit(); // advc.061, advc.001w
}


void CvPlot::updateSymbolDisplay()
{
	//PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
		return;

	for (int iLoop = 0; iLoop < getNumSymbols(); iLoop++)
	{
		CvSymbol* pLoopSymbol = getSymbol(iLoop);

		if (pLoopSymbol != NULL)
		{
			if (isShowCitySymbols())
				gDLL->getSymbolIFace()->setAlpha(pLoopSymbol, (isVisibleWorked()) ? 1.0f : 0.8f);
			else gDLL->getSymbolIFace()->setAlpha(pLoopSymbol, (isVisibleWorked()) ? 0.8f : 0.6f);
			gDLL->getSymbolIFace()->setScale(pLoopSymbol, getSymbolSize());
			gDLL->getSymbolIFace()->updatePosition(pLoopSymbol);
		}
	}
}


void CvPlot::updateSymbolVisibility()
{
	//PROFILE_FUNC();
	if (!GC.IsGraphicsInitialized())
		return;

	for (int iLoop = 0; iLoop < getNumSymbols(); iLoop++)
	{
		CvSymbol* pLoopSymbol = getSymbol(iLoop);

		if (pLoopSymbol != NULL)
		{
			if (isRevealed(getActiveTeam(), true) &&
				(isShowCitySymbols() ||
				(gDLL->UI().isShowYields() && !gDLL->UI().isCityScreenUp())))
			{
				gDLL->getSymbolIFace()->Hide(pLoopSymbol, false);
			}
			else gDLL->getSymbolIFace()->Hide(pLoopSymbol, true);
		}
	}
}


void CvPlot::updateSymbols()
{
	//PROFILE_FUNC();
	if (!GC.IsGraphicsInitialized())
		return;

	deleteAllSymbols();

	int aiYieldRate[NUM_YIELD_TYPES];
	int iMaxYieldRate = 0;
	FOR_EACH_ENUM(Yield)
	{
		int iYieldRate = calculateYield(eLoopYield, true);
		aiYieldRate[eLoopYield] = iYieldRate;
		iMaxYieldRate = std::max(iMaxYieldRate, iYieldRate);
	}

	if (iMaxYieldRate > 0)
	{
		static int iMAX_YIELD_STACK = GC.getDefineINT("MAX_YIELD_STACK"); // advc.opt: static
		int iLayers = iMaxYieldRate / iMAX_YIELD_STACK + 1;

		CvSymbol* pSymbol = NULL;
		for (int i = 0; i < iLayers; i++)
		{
			pSymbol = addSymbol();
			FOR_EACH_ENUM(Yield)
			{
				int iYieldRate = aiYieldRate[eLoopYield] - (iMAX_YIELD_STACK * i);
				iYieldRate = range(iYieldRate, 0, iMAX_YIELD_STACK);
				if (aiYieldRate[eLoopYield])
					gDLL->getSymbolIFace()->setTypeYield(pSymbol, eLoopYield, iYieldRate);
			}
		}
		for (int i = 0; i < getNumSymbols(); i++)
		{
			gDLL->getSymbolIFace()->init(getSymbol(i),
					gDLL->getSymbolIFace()->getID(pSymbol), i, (SymbolTypes)0, this);
		}
	}

	updateSymbolDisplay();
	updateSymbolVisibility();
}


void CvPlot::updateMinimapColor()
{
	//PROFILE_FUNC();
	if (!GC.IsGraphicsInitialized())
		return;
	// <advc.002a>
	CvMap::MinimapSettings const& kSettings = GC.getMap().getMinimapSettings();
	float fAlpha = (isWater() ? kSettings.getWaterAlpha() : kSettings.getLandAlpha());
	// </advc.002a>
	gDLL->UI().setMinimapColor(MINIMAPMODE_TERRITORY, getX(), getY(),
			plotMinimapColor(), fAlpha);
}


void CvPlot::updateCenterUnit()
{
	if (!GC.IsGraphicsInitialized())
		return;

	if (!isActiveVisible(true))
	{
		setCenterUnit(NULL);
		return;
	}

	setCenterUnit(getSelectedUnit());

	/*if (getCenterUnit() == NULL) {
		setCenterUnit(getBestDefender(getActivePlayer(), NO_PLAYER,
				NULL, false, false, true));
	}*/
	PlayerTypes eActivePlayer = getActivePlayer();
	if (getCenterUnit() == NULL)
		setCenterUnit(getBestDefender(eActivePlayer));

	if (getCenterUnit() == NULL)
	{	// <advc.028>
		CvUnit* pBestDef = getBestDefender(NO_PLAYER, eActivePlayer,
				gDLL->UI().getHeadSelectedUnit(), true,
				false, true); // advc.061
		if(pBestDef != NULL)
			setCenterUnit(pBestDef); // </advc.028>
	} // disabled by K-Mod. I don't think it's relevant whether or not the best defender can move.
	/*  advc.001: Enabled again, instead disabled the one with bTestCanMove=true --
		I think that's the one karadoc had meant to disable. */

	if (getCenterUnit() == NULL)
	{
		//setCenterUnit(getBestDefender(NO_PLAYER, getActivePlayer(), gDLL->UI().getHeadSelectedUnit()));
		// <advc.028> Replacing the above
		CvUnit* pBestDef = getBestDefender(NO_PLAYER, eActivePlayer,
				gDLL->UI().getHeadSelectedUnit(), false,
				false, true); // advc.061
		if(pBestDef != NULL)
			setCenterUnit(pBestDef); // </advc.028>
	}

	if (getCenterUnit() == NULL)
	{
		//setCenterUnit(getBestDefender(NO_PLAYER, getActivePlayer()));
		// <advc.028> Replacing the above
		CvUnit* pBestDef = getBestDefender(NO_PLAYER, eActivePlayer, NULL, false,
				false, true); // advc.061
		if(pBestDef != NULL)
			setCenterUnit(pBestDef); // </advc.028>
	}
}


void CvPlot::verifyUnitValidPlot()
{
	//PROFILE_FUNC(); // advc.003o
	std::vector<std::pair<PlayerTypes, int> > bumpedGroups; // K-Mod

	std::vector<CvUnit*> apUnits;
	FOR_EACH_UNIT_VAR_IN(pUnit, *this)
	{
		FAssert(pUnit != NULL); // advc: was if(pUnit!=NULL)
		apUnits.push_back(pUnit);
	}

	std::vector<CvUnit*>::iterator it = apUnits.begin();
	while (it != apUnits.end())
	{
		CvUnit& kUnit = **it;
		bool bErased = false;
		if (kUnit.atPlot(this) && !kUnit.isCargo() &&
			!kUnit.isInCombat())
		{
			//if (!isValidDomainForLocation(*pLoopUnit) ||
			if (!kUnit.isValidDomain(*this) || // advc
				!kUnit.canEnterTerritory(getTeam(), false, area()))
			{
				if (!kUnit.jumpToNearestValidPlot(true))
					bErased = true;
				// <K-Mod>
				else
				{
					bumpedGroups.push_back(std::make_pair(
							kUnit.getOwner(), kUnit.getGroupID()));
				} // </K-Mod>
			}
		}
		if (bErased)
			it = apUnits.erase(it);
		else ++it;
	}

	if (isOwned())
	{
		it = apUnits.begin();
		while (it != apUnits.end())
		{
			CvUnit& kUnit = **it;
			bool bErased = false;
			if (kUnit.atPlot(this) && !kUnit.isInCombat())
			{
				if (kUnit.getTeam() != getTeam() && (getTeam() == NO_TEAM ||
					!GET_TEAM(getTeam()).isVassal(kUnit.getTeam())))
				{
					if (isVisibleEnemyUnit(&kUnit) &&
						!kUnit.isInvisible(getTeam(), false))
					{
						if (!kUnit.jumpToNearestValidPlot(true, false,
							/*	advc.163: Normally, the jump shouldn't be free,
								but I don't want humans to troll AI stacks with e.g.
								a worker placed on the path of an invading stack. */
							!kUnit.isHuman()))
						{
							bErased = true;
						}
						// <K-Mod>
						else
						{
							bumpedGroups.push_back(std::make_pair(
									kUnit.getOwner(), kUnit.getGroupID()));
						} // </K-Mod>
					}
				}
			}
			if (bErased)
				it = apUnits.erase(it);
			else ++it;
		}
	}

	// K-Mod
	// first remove duplicate group numbers
	std::sort(bumpedGroups.begin(), bumpedGroups.end());
	bumpedGroups.erase(std::unique(bumpedGroups.begin(),
			bumpedGroups.end()), bumpedGroups.end());
	// now divide the broken groups
	for (size_t i = 0; i < bumpedGroups.size(); i++)
	{
		CvSelectionGroup* pLoopGroup = GET_PLAYER(bumpedGroups[i].first).
				getSelectionGroup(bumpedGroups[i].second);
		if (pLoopGroup != NULL)
			pLoopGroup->regroupSeparatedUnits();
	} // K-Mod end
}

/*  K-Mod, 2/jan/11, karadoc:
	forceBumpUnits() forces all units off the plot, onto a nearby plot */
void CvPlot::forceBumpUnits()
{
	/*	Note: this function is almost certainly not optimal.
		I just took the code from another function and I don't want to mess it up.
		(advc: Presumably, he based it on verifyUnitValidPlot above.) */
	std::vector<CvUnit*> apUnits;
	FOR_EACH_UNIT_VAR_IN(pUnit, *this)
	{
		FAssert(pUnit != NULL); // advc: was if(pUnit!=NULL)
		apUnits.push_back(pUnit);
	}

	std::vector<CvUnit*>::iterator it = apUnits.begin();
	while (it != apUnits.end())
	{
		CvUnit& kUnit = **it; // advc: NULL check removed
		bool bErased = false;
		if (kUnit.at(*this) && !kUnit.isCargo() &&
			!kUnit.isInCombat())
		{
			if (!kUnit.jumpToNearestValidPlot(true, true))
				bErased = true;
		}
		if (bErased)
			it = apUnits.erase(it);
		else ++it;
	}
}

/*	K-Mod. Added bBomb argument.
	bBomb signals that the explosion should damage units, buildings, and city population.
	(I've also tidied up the code a little bit) */
void CvPlot::nukeExplosion(int iRange, CvUnit* pNukeUnit, bool bBomb)
{
	// <advc.opt>
	static int const iNUKE_FALLOUT_PROB = GC.getDefineINT("NUKE_FALLOUT_PROB");
	static int const iNUKE_UNIT_DAMAGE_BASE = GC.getDefineINT(CvGlobals::NUKE_UNIT_DAMAGE_BASE);
	static int const iNUKE_UNIT_DAMAGE_RAND_1 = GC.getDefineINT(CvGlobals::NUKE_UNIT_DAMAGE_RAND_1);
	static int const iNUKE_UNIT_DAMAGE_RAND_2 = GC.getDefineINT(CvGlobals::NUKE_UNIT_DAMAGE_RAND_2);
	static int const iNUKE_BUILDING_DESTRUCTION_PROB = GC.getDefineINT(CvGlobals::NUKE_BUILDING_DESTRUCTION_PROB);
	static int const iNUKE_POPULATION_DEATH_BASE = GC.getDefineINT("NUKE_POPULATION_DEATH_BASE");
	static int const iNUKE_POPULATION_DEATH_RAND_1 = GC.getDefineINT("NUKE_POPULATION_DEATH_RAND_1");
	static int const iNUKE_POPULATION_DEATH_RAND_2 = GC.getDefineINT("NUKE_POPULATION_DEATH_RAND_2");
	// </advc.opt>
	// <advc.650> For announcing effects of a nuke explosion
	typedef std::pair<CvPlot const*,CvWString> NukeEffect;
	std::vector<NukeEffect> aImprovementDestroyed;
	std::vector<NukeEffect> aFeatureDestroyed;
	std::vector<std::vector<NukeEffect> > aaUnitDamaged(MAX_PLAYERS);
	std::vector<std::vector<NukeEffect> > aaUnitKilled(MAX_PLAYERS);
	std::vector<NukeEffect> aBuildingDestroyed;
	std::vector<NukeEffect> aCitizensKilled;
	// </advc.650>
	for (SquareIter it(*this, iRange); it.hasNext(); ++it)
	{
		CvPlot& p = *it;

		// (if we remove roads, don't remove them on the city... XXX)

		CvCity* pCity = p.getPlotCity();
		if (pCity == NULL)
		{
			if (!p.isWater() && !p.isImpassable())
			{
				if (p.getFeatureType() == NO_FEATURE ||
					!GC.getInfo(p.getFeatureType()).isNukeImmune())
				{
					if (SyncRandSuccess100(iNUKE_FALLOUT_PROB))
					{	// <advc.650>
						if (p.isImproved())
						{
							aImprovementDestroyed.push_back(NukeEffect(&p,
									GC.getInfo(p.getImprovementType()).getDescription()));
						}
						if (p.isFeature() &&
							p.getFeatureType() != GC.getDefineINT("NUKE_FEATURE"))
						{
							aFeatureDestroyed.push_back(NukeEffect(&p,
									GC.getInfo(p.getFeatureType()).getDescription()));
						} // </advc.650>
						p.setImprovementType(NO_IMPROVEMENT);
						p.setFeatureType((FeatureTypes)GC.getDefineINT("NUKE_FEATURE"));
					}
				}
			}
		}
		// K-Mod. If this is not a bomb, then we're finished with this plot.
		if (!bBomb)
			continue;
		// <advc.650>
		for (TeamAIIter<ALIVE> itTeam; itTeam.hasNext(); ++itTeam)
		{
			/*	Strictly speaking, we should check the same conditions as in
				CvUnit::nuke, i.e. whether the nuke owner and plot have been met,
				but that's awkward to implement. A very minor AI cheat. */
			if (isRevealed(itTeam->getID()))
				itTeam->AI_rememberNukeExplosion(p);
		} // </advc.650>

		FOR_EACH_UNIT_VAR_IN(pLoopUnit, p)
		{	/*	<advc> BtS had used two loops here and a temporary list.
				We do need a NULL check to deal with units killed while in cargo. */
			if (pLoopUnit == NULL)
				continue; // </advc>
			if (pLoopUnit == pNukeUnit)
				continue;
			// <kekm.7>
			TeamTypes eAttackingTeam = NO_TEAM;
			if(pNukeUnit != NULL)
				eAttackingTeam = TEAMID(pNukeUnit->getOwner());
			// </kekm.7>
			if (!pLoopUnit->isNukeImmune() && !pLoopUnit->isDelayedDeath() &&
				// <kekm.7>
				// Nukes target only enemy and own units.
				//Needed because blocking by neutral players disabled.
				(eAttackingTeam == NO_TEAM ||
				pLoopUnit->isEnemy(eAttackingTeam) ||
				eAttackingTeam == TEAMID(pLoopUnit->getOwner())))
				// </kekm.7>
			{
				int iNukeDamage = (iNUKE_UNIT_DAMAGE_BASE +
						SyncRandNum(iNUKE_UNIT_DAMAGE_RAND_1) +
						SyncRandNum(iNUKE_UNIT_DAMAGE_RAND_2));
				if (pCity != NULL)
				{
					iNukeDamage *= std::max(0, pCity->getNukeModifier() + 100);
					iNukeDamage /= 100;
				}
				if (pLoopUnit->canFight() || pLoopUnit->airBaseCombatStr() > 0)
				{	// <advc.650>
					bool const bLethal = pLoopUnit->isLethalDamage(iNukeDamage);
					(bLethal ? aaUnitKilled : aaUnitDamaged)[pLoopUnit->getOwner()].
							push_back(NukeEffect(&p, pLoopUnit->getName()));
					if (bLethal && pLoopUnit->hasCargo())
					{
						std::vector<CvUnit*> apCargo;
						pLoopUnit->getCargoUnits(apCargo);
						for (size_t i = 0; i < apCargo.size(); i++)
						{
							aaUnitKilled[apCargo[i]->getOwner()].push_back(
									NukeEffect(&p, apCargo[i]->getName()));
						}
					}
					// </advc.650>
					pLoopUnit->changeDamage(iNukeDamage, pNukeUnit != NULL ?
							pNukeUnit->getOwner() : NO_PLAYER);
				}
				//else if (iNukeDamage >= GC.getDefineINT("NUKE_NON_COMBAT_DEATH_THRESHOLD"))
				// <kekm.20>
				else if (SyncRandNum(100) * 100 <
					std::max(0, ((pCity == NULL ? 0 : pCity->getNukeModifier()) + 100)) *
					(iNUKE_UNIT_DAMAGE_BASE - 1 + (iNUKE_UNIT_DAMAGE_RAND_1 +
					iNUKE_UNIT_DAMAGE_RAND_2 - 1) / 2)) // </kekm.20>
				{
					// <advc.650>
					aaUnitKilled[pLoopUnit->getOwner()].push_back(
							NukeEffect(&p, pLoopUnit->getName())); // </advc.650>
					pLoopUnit->kill(false, pNukeUnit != NULL ? pNukeUnit->getOwner() : NO_PLAYER);
				}
			}
		}
		if (pCity == NULL)
			continue;

		FOR_EACH_ENUM2(Building, eBuilding)
		{
			if (pCity->getNumRealBuilding(eBuilding) > 0 &&
				!GC.getInfo(eBuilding).isNukeImmune() &&
				SyncRandSuccess100(iNUKE_BUILDING_DESTRUCTION_PROB))
			{
				// <advc.650>
				aBuildingDestroyed.push_back(NukeEffect(&p,
						GC.getInfo(eBuilding).getDescription())); // </advc.650>
				pCity->setNumRealBuilding(eBuilding,
						pCity->getNumRealBuilding(eBuilding) - 1);
			}
		}
		int iNukedPopulation = (pCity->getPopulation() *
				(iNUKE_POPULATION_DEATH_BASE +
				SyncRandNum(iNUKE_POPULATION_DEATH_RAND_1) +
				SyncRandNum(iNUKE_POPULATION_DEATH_RAND_2)))
				/ 100;
		iNukedPopulation *= std::max(0, (pCity->getNukeModifier() + 100));
		iNukedPopulation /= 100;
		int iPopChange = -std::min(pCity->getPopulation() - 1, iNukedPopulation);
		// advc.650:
		aCitizensKilled.push_back(NukeEffect(&p, CvWString::format(L"%d", -iPopChange)));
		pCity->changePopulation(iPopChange);
	}
	if (bBomb) // K-Mod
	{
		GC.getGame().changeNukesExploded(1);
		CvEventReporter::getInstance().nukeExplosion(this, pNukeUnit);
	}
	// <advc.650>
	// Could replace this with L" " - but I think line breaks make it easier to read.
	#define SZSEP NEWLINE
	for (PlayerIter<MAJOR_CIV> itPlayer; itPlayer.hasNext(); ++itPlayer)
	{
		TeamTypes const eObs = itPlayer->getTeam();
		CvWString szEffects;
		bool bTeamUnitAffected = false;
		bool bEnemyUnitAffected = false;
		for (PlayerIter<ALIVE> itUnitOwner; itUnitOwner.hasNext(); ++itUnitOwner)
		{
			PlayerTypes const eUnitOwner = itUnitOwner->getID();
			std::vector<NukeEffect> aUnitDamaged(aaUnitDamaged[eUnitOwner]);
			std::vector<NukeEffect> aUnitKilled(aaUnitKilled[eUnitOwner]);
			wchar const* szCivAdj = GET_PLAYER(eUnitOwner).
					getCivilizationAdjectiveKey();
			bool bListUnits = (TEAMID(eUnitOwner) == eObs ||
					GET_TEAM(eUnitOwner).isAtWar(eObs));
			{
				std::vector<CvWString*> apStrings;
				for (size_t i = 0; i < aUnitKilled.size(); i++)
				{
					if (aUnitKilled[i].first->isVisible(eObs))
						apStrings.push_back(&aUnitKilled[i].second);
				}
				if (apStrings.size() > 8 || (!bListUnits && !apStrings.empty()))
				{
					bListUnits = false;
					szEffects.append(gDLL->getText(
							apStrings.size() == 1 ?
							"TXT_KEY_NUKE_EFFECT_ONE_UNIT_DESTROYED" :
							"TXT_KEY_NUKE_EFFECT_NUM_UNITS_DESTROYED",
							(int)apStrings.size(), szCivAdj));
					szEffects.append(L" ");
				}
				else if (!apStrings.empty())
				{
					szEffects.append(gDLL->getText(
							"TXT_KEY_NUKE_EFFECT_UNITS_DESTROYED", szCivAdj));
					szEffects.append(SZSEP);
					for (size_t i = 0; i < apStrings.size(); i++)
					{
						szEffects.append(*apStrings[i]);
						szEffects.append(i + 1 == apStrings.size() ? L"."SZSEP : L", ");
					}
					if (TEAMID(eUnitOwner) == eObs)
						bTeamUnitAffected = true;
					else if (GET_TEAM(eUnitOwner).isAtWar(eObs))
						bEnemyUnitAffected = true;
				}
			}
			{
				std::vector<CvWString*> apStrings;
				for (size_t i = 0; i < aUnitDamaged.size(); i++)
				{
					if (aUnitDamaged[i].first->isVisible(eObs))
						apStrings.push_back(&aUnitDamaged[i].second);
				}
				if (apStrings.size() > 2 || (!bListUnits && !apStrings.empty()))
				{
					szEffects.append(gDLL->getText(
							apStrings.size() == 1 ?
							"TXT_KEY_NUKE_EFFECT_ONE_UNIT_DAMAGED" :
							"TXT_KEY_NUKE_EFFECT_NUM_UNITS_DAMAGED",
							(int)apStrings.size(), szCivAdj));
					szEffects.append(SZSEP);
				}
				else if (!apStrings.empty())
				{
					szEffects.append(gDLL->getText(
							"TXT_KEY_NUKE_EFFECT_UNITS_DAMAGED", szCivAdj));
					szEffects.append(SZSEP);
					for (size_t i = 0; i < apStrings.size(); i++)
					{
						szEffects.append(*apStrings[i]);
						szEffects.append(i + 1 == apStrings.size() ? L"."SZSEP : L", ");
					}
					if (TEAMID(eUnitOwner) == eObs)
						bTeamUnitAffected = true;
					else if (GET_TEAM(eUnitOwner).isAtWar(eObs))
						bEnemyUnitAffected = true;
				}
			}
		}
		bool bTeamCityAffected = false;
		bool bEnemyCityAffected = false;
		{
			std::vector<CvWString*> apStrings;
			for (size_t i = 0; i < aCitizensKilled.size(); i++)
			{
				CvCity const* pCity = aCitizensKilled[i].first->getPlotCity();
				if (pCity != NULL && pCity->isRevealed(eObs))
				{
					apStrings.push_back(&aCitizensKilled[i].second);
					if (pCity->getTeam() == eObs)
						bTeamCityAffected = true;
					else if (GET_TEAM(eObs).isAtWar(pCity->getTeam()))
						bEnemyCityAffected = true;
				}
			}
			if (!apStrings.empty())
			{
				for (size_t i = 0; i < apStrings.size(); i++)
				{
					szEffects.append(gDLL->getText(
							"TXT_KEY_NUKE_EFFECT_NUM_CITIZENS_KILLED",
							apStrings[i]->c_str()));
					szEffects.append(L" ");
				}
			}
		}
		{
			bool bAnyCityRevealed = false;
			bool bListBuildings = false;
			std::vector<CvWString*> apStrings;
			for (size_t i = 0; i < aBuildingDestroyed.size(); i++)
			{
				CvCity const* pCity = aBuildingDestroyed[i].first->getPlotCity();
				if (pCity == NULL)
					continue;
				if (pCity->getTeam() == eObs || GET_TEAM(pCity->getTeam()).isAtWar(eObs))
					bListBuildings = true;
				if (pCity->isAllBuildingsVisible(eObs, false))
					apStrings.push_back(&aBuildingDestroyed[i].second);
				if (pCity->isRevealed(eObs))
				{
					bAnyCityRevealed = true;
					if (pCity->getTeam() == eObs)
						bTeamCityAffected = true;
					else if (GET_TEAM(eObs).isAtWar(pCity->getTeam()))
						bEnemyCityAffected = true;
				}
			}
			if (apStrings.size() > 12 || (!bListBuildings && apStrings.size() > 1))
			{
				szEffects.append(gDLL->getText(
						"TXT_KEY_NUKE_EFFECT_NUM_BUILDINGS_DESTROYED",
						(int)apStrings.size()));
				szEffects.append(SZSEP);
			}
			else if (!apStrings.empty())
			{
				szEffects.append(gDLL->getText(
						"TXT_KEY_NUKE_EFFECT_BUILDINGS_DESTROYED"));
				szEffects.append(L" ");
				for (size_t i = 0; i < apStrings.size(); i++)
				{
					szEffects.append(*apStrings[i]);
						szEffects.append(i + 1 == apStrings.size() ? L"."SZSEP : L", ");
				}
			}
			else if (bAnyCityRevealed && iNUKE_BUILDING_DESTRUCTION_PROB > 0)
			{
				szEffects.append(gDLL->getText(
						"TXT_KEY_NUKE_EFFECT_BUILDINGS_MAYBE_DESTROYED"));
				szEffects.append(SZSEP);
			}
		}
		{
			bool bShowImprovements = false;
			std::vector<CvWString*> apStrings;
			for (size_t i = 0; i < aImprovementDestroyed.size(); i++)
			{
				if (aImprovementDestroyed[i].first->isVisible(eObs))
				{
					apStrings.push_back(&aImprovementDestroyed[i].second);
					TeamTypes eImprovTeam = aImprovementDestroyed[i].first->getTeam();
					if (eImprovTeam != NO_TEAM &&
						(eImprovTeam == eObs || GET_TEAM(eImprovTeam).isAtWar(eObs)))
					{
						bShowImprovements = true;
					}
				}
			}
			if (!bShowImprovements)
				apStrings.clear();
			if (apStrings.size() > 5)
			{
				szEffects.append(gDLL->getText(
						"TXT_KEY_NUKE_EFFECT_NUM_IMPROVEMENTS_DESTROYED",
						(int)apStrings.size()));
				szEffects.append(SZSEP);
			}
			else if (!apStrings.empty())
			{
				szEffects.append(gDLL->getText(
						"TXT_KEY_NUKE_EFFECT_IMPROVEMENTS_DESTROYED"));
				szEffects.append(L" ");
				for (size_t i = 0; i < apStrings.size(); i++)
				{
					szEffects.append(*apStrings[i]);
					szEffects.append(i + 1 == apStrings.size() ? L". " : L", ");
				}
			}
		}
		{
			bool bShowFeatures = false;
			std::vector<CvWString*> apStrings;
			for (size_t i = 0; i < aFeatureDestroyed.size(); i++)
			{
				if (aFeatureDestroyed[i].first->isVisible(eObs))
				{
					apStrings.push_back(&aFeatureDestroyed[i].second);
					TeamTypes eFeatureTeam = aFeatureDestroyed[i].first->getTeam();
					if (eFeatureTeam != NO_TEAM &&
						(eFeatureTeam == eObs || GET_TEAM(eFeatureTeam).isAtWar(eObs)))
					{
						bShowFeatures = true;
					}
				}
			}
			if (!bShowFeatures)
				apStrings.clear();
			if (apStrings.size() > 5)
			{
				szEffects.append(gDLL->getText(
						"TXT_KEY_NUKE_EFFECT_NUM_FEATURES_DESTROYED",
						(int)apStrings.size()));
				szEffects.append(SZSEP);
			}
			else if (!apStrings.empty())
			{
				szEffects.append(gDLL->getText(
						"TXT_KEY_NUKE_EFFECT_FEATURES_DESTROYED"));
				szEffects.append(L" ");
				for (size_t i = 0; i < apStrings.size(); i++)
				{
					szEffects.append(*apStrings[i]);
					szEffects.append(i + 1 == apStrings.size() ? L"."SZSEP : L", ");
				}
			}
		}
		if (szEffects.empty())
			continue;
		CvWString szMsg = gDLL->getText("TXT_KEY_NUKE_EFFECTS_START");
		szMsg.append(SZSEP);
		// Drop final separator
		szMsg.append(szEffects.substr(0, szEffects.length() - 1));
		ColorTypes eMsgColor = GC.getColorType(
				bTeamCityAffected || (bTeamUnitAffected && !bEnemyCityAffected) ? "RED" :
				/*	A bit distasteful to use green color here?
					But it looks strange to show a positive combat outcome in white ... */
				(bEnemyCityAffected || bEnemyUnitAffected ? "GREEN" : "WHITE"));
		gDLL->UI().addMessage(itPlayer->getID(),
				bTeamCityAffected || bTeamUnitAffected ||
				bEnemyCityAffected || bEnemyUnitAffected,
				bTeamUnitAffected || bEnemyUnitAffected ?
				GC.getDefineINT(CvGlobals::EVENT_MESSAGE_TIME_LONG) : -1,
				szMsg, *this, NULL, MESSAGE_TYPE_INFO, NULL, eMsgColor, false, false);
	}
	#undef SZSEP
	// </advc.650>
}


bool CvPlot::isConnectedTo(CvCity const& kCity) const
{
	// Super Forts begin *AI_worker* (had to remove the assert and replace it with an if-else)
	if (!isFortImprovement())
	{
		//original advc code
		FAssert(isOwned());
		return (isSamePlotGroup(kCity.getPlot(), getOwner()) ||
			isSamePlotGroup(kCity.getPlot(), kCity.getOwner()));
	}
	else if(isOwned())
	{
		return (isSamePlotGroup(kCity.getPlot(), getOwner()) ||
			isSamePlotGroup(kCity.getPlot(), kCity.getOwner()));
	}
	else
	{
		return false;
	}
	// Super Forts end
}


bool CvPlot::isConnectedToCapital(PlayerTypes ePlayer) const
{
	if (ePlayer == NO_PLAYER)
		ePlayer = getOwner();

	if (ePlayer != NO_PLAYER)
	{
		CvCity* pCapital = GET_PLAYER(ePlayer).getCapital();
		if (pCapital != NULL)
			return isConnectedTo(*pCapital);
	}

	return false;
}


int CvPlot::getPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	FAssert(ePlayer != NO_PLAYER && eBonus != NO_BONUS);

	CvPlotGroup* pPlotGroup = getPlotGroup(ePlayer);
	if (pPlotGroup != NULL)
		return pPlotGroup->getNumBonuses(eBonus);
	return 0;
}


bool CvPlot::isAdjacentPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	/*	K-Mod. Allow this plot to have whatever resources are
		available in the city working the plot. (The purpose of this is to
		allow railroads to be built the 'oil' from Standard Ethanol.) */
	CvCity* pCity = getWorkingCity();
	if (pCity != NULL && pCity->getOwner() == ePlayer && pCity->hasBonus(eBonus))
		return true;
	// K-Mod end
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isPlotGroupConnectedBonus(ePlayer, eBonus))
			return true;
	}
	return false;
}


void CvPlot::updatePlotGroupBonus(bool bAdd, /* advc.064d: */ bool bVerifyProduction)
{
	PROFILE_FUNC();

	if (!isOwned())
		return;

	CvPlotGroup* pPlotGroup = getPlotGroup(getOwner());
	if(pPlotGroup == NULL)
		return;

	CvCity* pPlotCity = getPlotCity();
	if (pPlotCity != NULL)
	{
		if (pPlotCity->isAnyFreeBonus()) // advc.opt
		{
			FOR_EACH_ENUM(Bonus)
			{
				if (!GET_TEAM(getTeam()).isBonusObsolete(eLoopBonus))
				{
					pPlotGroup->changeNumBonuses(eLoopBonus,
							pPlotCity->getFreeBonus(eLoopBonus) * (bAdd ? 1 : -1));
				}
			}
		}
		if (pPlotCity->isCapital())
		{
			FOR_EACH_ENUM(Bonus)
			{
				pPlotGroup->changeNumBonuses(eLoopBonus,
						GET_PLAYER(getOwner()).getBonusExport(eLoopBonus) *
						(bAdd ? -1 : 1));
				pPlotGroup->changeNumBonuses(eLoopBonus,
						GET_PLAYER(getOwner()).getBonusImport(eLoopBonus) *
						(bAdd ? 1 : -1));
			}
		}
	}

	/*eNonObsoleteBonus = getNonObsoleteBonusType(getTeam());
	if (eNonObsoleteBonus != NO_BONUS) {
		if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getInfo(eNonObsoleteBonus).getTechCityTrade()))) {
			if (isCity(true, getTeam()) || ((getImprovementType() != NO_IMPROVEMENT) && GC.getInfo(getImprovementType()).isImprovementBonusTrade(eNonObsoleteBonus))) {
				if ((pPlotGroup != NULL) && isBonusNetwork(getTeam()))
					pPlotGroup->changeNumBonuses(eNonObsoleteBonus, ((bAdd) ? 1 : -1));
	} } }*/ // BtS
	/*	K-Mod. Just trying to standardize the code to reduce the potential for mistakes.
		There are no functionality changes here. */
	BonusTypes eBonus = getNonObsoleteBonusType(getTeam(), true);
	if (eBonus != NO_BONUS && pPlotGroup && isBonusNetwork(getTeam()))
		pPlotGroup->changeNumBonuses(eBonus, bAdd ? 1 : -1);
	// K-Mod end
	// <advc.064d>
	if (bVerifyProduction)
		pPlotGroup->verifyCityProduction(); // </advc.064d>
}

// advc: Merged with isAdjacentToArea(int)
bool CvPlot::isAdjacentToArea(CvArea const& kArea) const
{
	PROFILE_FUNC();
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isArea(kArea))
			return true;
	}
	return false;
}


bool CvPlot::shareAdjacentArea(CvPlot const* pPlot) const
{
	PROFILE_FUNC();

	CvArea const* pLastArea = NULL;
	FOR_EACH_ADJ_PLOT(*this)
	{
		CvArea const* pCurrArea = pAdj->area();
		if (pCurrArea != pLastArea)
		{
			if (pPlot->isAdjacentToArea(*pCurrArea))
				return true;
			pLastArea = pCurrArea;
		}
	}
	return false;
}


bool CvPlot::isAdjacentToLand() const
{
	//PROFILE_FUNC(); // advc: Shouldn't be a concern now that we have an adjacency list
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (!pAdj->isWater())
			return true;
	}
	return false;
}


bool CvPlot::isCoastalLand(int iMinWaterSize) const
{
	//PROFILE_FUNC(); // advc.003o: Called very frequently from pathfinding code

	if (isWater())
		return false;
	// <advc.003t>
	if (iMinWaterSize < 0) // Tbd.: Cache the result for iMinWaterSize -1 and 0?
		iMinWaterSize = GC.getDefineINT(CvGlobals::MIN_WATER_SIZE_FOR_OCEAN);
	// </advc.003t>
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isWater() /* advc.030: */ && !pAdj->isImpassable() &&
			pAdj->getArea().getNumTiles() >= iMinWaterSize)
		{
			return true;
		}
	}
	return false;
}


bool CvPlot::isVisibleWorked() const
{
	if (isBeingWorked())
	{
		if (isActiveTeam() || GC.getGame().isDebugMode())
			return true;
	}
	return false;
}


bool CvPlot::isWithinTeamCityRadius(TeamTypes eTeam, PlayerTypes eIgnorePlayer) const
{
	PROFILE_FUNC();

	for (MemberIter itMember(eTeam); itMember.hasNext(); ++itMember)
	{
		if (eIgnorePlayer == NO_PLAYER || itMember->getID() != eIgnorePlayer)
		{
			if (isPlayerCityRadius(itMember->getID()))
				return true;
		}
	}

	return false;
}


bool CvPlot::isLake() const
{
	/*	advc (fixme): Can be NULL while recalculating areas at game start.
		As in BtS. Not really a problem, but ideally setArea shouldn't call
		updateIrrigated (which calls isLake) while recalculating areas;
		should call it once on each plot in the end instead. */
	return (area() == NULL ? false : getArea().isLake());
}

// Deliverator fresh water - dune

void CvPlot::changeFreshWater(int iChange)
{
	m_iFreshWaterAmount += iChange;
}

void CvPlot::changeFreshWaterInRadius(int iChange, int iRadius)
{
	CvPlot* pLoopPlot;
	int iDX, iDY;

	if (iRadius == 0) {
		changeFreshWater(iChange);
	}
	else if (iRadius > 0) {
		for (iDX = -iRadius; iDX <= iRadius; iDX++)
		{
			for (iDY = -iRadius; iDY <= iRadius; iDY++)
			{
				if (!(abs(iDX) + abs(iDY) >= iRadius + 2)) {

					pLoopPlot = plotXY(getX(), getY(), iDX, iDY);

					if (pLoopPlot != NULL)
					{
						pLoopPlot->changeFreshWater(iChange);
					}
				}
			}
		}
	}
}
// XXX if this changes need to call updateIrrigated and CvCity::updateFreshWaterHealth
// XXX precalculate this???
bool CvPlot::isFreshWater() const
{
/* deliverator fresh water - keldath*/
	if(m_iFreshWaterAmount > 0)
	{
		return (m_iFreshWaterAmount > 0); 
	}
/* Original code - commented out by Deliverator removed marker here to rmeove fresh water from lakes and ocean ->*/

	/*	advc: Called frequently enough that I don't want to keep profiling it.
		Caching this in a variable bool m_bFreshWater:1 would save some time,
		but not quite trivial to keep it updated, so probably not worth it. */
	//PROFILE_FUNC();
	// davidlallen: fresh water on ocean: remove test isLand()
    if (isWater())
		return false;

	// davidlallen: fresh water on ocean: remove test isLand()
   if (isImpassable())
		return false;

	if (isRiver())
		return true;

	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isLake())
			return true;
		if (pAdj->isFeature() &&
			GC.getInfo(pAdj->getFeatureType()).isAddsFreshWater())
		{
			return true;
		}
	}

	return false;/* if u want no fresh water from lakes and ocean mark out up to here ->*/
}

// advc.108:
bool CvPlot::isAdjacentFreshWater() const
{
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isFreshWater())
			return true;
	}
	return false;
}

// advc.041: (no longer just a matter of area size)
bool CvPlot::isAdjacentSaltWater() const
{
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isWater() && !pAdj->isLake())
			return true;
	}
	return false;
}


bool CvPlot::isPotentialIrrigation() const
{
	// advc: 2nd condition was !isHills. Mods might allow cities on peaks.
	//doto-keldath- left it out
	//if ((isCity() && isFlatlands()) ||
	//===NM=====Mountains Mod===0=====keldath - i hope thats ok
	if ((isCity() && !(isHills() || !(GC.getGame().isOption(GAMEOPTION_MOUNTAINS) && isPeak())))||
	//===NM=====Mountains Mod===0=====
		(isImproved() && GC.getInfo(getImprovementType()).isCarriesIrrigation()))
	{
		if (isOwned() && GET_TEAM(getTeam()).isIrrigation())
			return true;
	}

	return false;
}


bool CvPlot::canHavePotentialIrrigation() const
{
	//PROFILE_FUNC(); // advc (not called very frequently)
//===NM=====Mountains Mods===0=====keldath - i hope thats ok
	if (isCity() && !(isHills() || !(GC.getGame().isOption(GAMEOPTION_MOUNTAINS) && isPeak())))
//===NM=====Mountains Mods===0=====
		return true;
	// <advc.opt>
	if(isWater())
		return false; // </advc.opt>
	// advc: 2nd condition was !isHills. Mods might allow cities on peaks.
	if (isCity() && isFlatlands())
		return true;
	FOR_EACH_ENUM(Improvement)
	{
		if (GC.getInfo(eLoopImprovement).isCarriesIrrigation())
		{
			if (canHaveImprovement(eLoopImprovement, NO_TEAM, true))
				return true;
		}
	}
	return false;
}


bool CvPlot::isIrrigationAvailable(bool bIgnoreSelf) const
{
	if (!bIgnoreSelf && isIrrigated())
		return true;
	if (isFreshWater())
		return true;
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isIrrigated())
			return true;
	}
	return false;
}


bool CvPlot::isRiverMask() const
{
	if (isNOfRiver() || isWOfRiver())
		return true;

	CvPlot* pPlot = plotDirection(getX(), getY(), DIRECTION_EAST);
	if (pPlot != NULL && pPlot->isNOfRiver())
		return true;

	pPlot = plotDirection(getX(), getY(), DIRECTION_SOUTH);
	if (pPlot != NULL && pPlot->isWOfRiver())
		return true;

	return false;
}


bool CvPlot::isRiverCrossingFlowClockwise(DirectionTypes eDirection) const
{
	switch(eDirection)
	{
	case DIRECTION_NORTH:
	{
		CvPlot* pPlot = plotDirection(getX(), getY(), DIRECTION_NORTH);
		if (pPlot != NULL)
			return (pPlot->getRiverWEDirection() == CARDINALDIRECTION_EAST);
		break;
	}
	case DIRECTION_EAST:
		return (getRiverNSDirection() == CARDINALDIRECTION_SOUTH);
	case DIRECTION_SOUTH:
		return (getRiverWEDirection() == CARDINALDIRECTION_WEST);
	case DIRECTION_WEST:
	{
		CvPlot* pPlot = plotDirection(getX(), getY(), DIRECTION_WEST);
		if(pPlot != NULL)
			return (pPlot->getRiverNSDirection() == CARDINALDIRECTION_NORTH);
		break;
	}
	default: FAssert(false);
	}

	return false;
}


bool CvPlot::isRiverSide() const
{
	FOR_EACH_ORTH_ADJ_PLOT(*this)
	{
		if (isRiverCrossing(directionXY(*this, *pAdj)))
			return true;
	}
	return false;
}


bool CvPlot::isRiverConnection(DirectionTypes eDirection) const
{
	switch (eDirection)
	{
	case NO_DIRECTION: return false; // advc.opt (instead of checking it upfront)
	case DIRECTION_NORTH:
		return (isRiverCrossing(DIRECTION_EAST) || isRiverCrossing(DIRECTION_WEST));
	case DIRECTION_NORTHEAST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_EAST));
	case DIRECTION_EAST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_SOUTH));
	case DIRECTION_SOUTHEAST:
		return (isRiverCrossing(DIRECTION_SOUTH) || isRiverCrossing(DIRECTION_EAST));
	case DIRECTION_SOUTH:
		return (isRiverCrossing(DIRECTION_EAST) || isRiverCrossing(DIRECTION_WEST));
	case DIRECTION_SOUTHWEST:
		return (isRiverCrossing(DIRECTION_SOUTH) || isRiverCrossing(DIRECTION_WEST));
	case DIRECTION_WEST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_SOUTH));
	case DIRECTION_NORTHWEST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_WEST));
	default:
		FAssert(false);
		return false;
	}
}


CvPlot* CvPlot::getNearestLandPlotInternal(int iDistance) const
{
	if (iDistance > GC.getMap().getGridHeight() && iDistance > GC.getMap().getGridWidth())
		return NULL;

	for (int iDX = -iDistance; iDX <= iDistance; iDX++)
	{
		for (int iDY = -iDistance; iDY <= iDistance; iDY++)
		{
			if (abs(iDX) + abs(iDY) == iDistance)
			{
				CvPlot* pPlot = plotXY(getX(), getY(), iDX, iDY);
				if (pPlot != NULL)
				{
					if (!pPlot->isWater())
						return pPlot;
				}
			}
		}
	}
	return getNearestLandPlotInternal(iDistance + 1);
}


CvArea* CvPlot::getNearestLandArea() const
{
	CvPlot* pPlot = getNearestLandPlot();
	return (pPlot == NULL ? NULL : pPlot->area());
}


int CvPlot::seeFromLevel(TeamTypes eTeam) const
{
	int iLevel = GC.getInfo(getTerrainType()).getSeeFromLevel();
	// Super Forts begin *vision*
	if (getImprovementType() != NO_IMPROVEMENT && isFortImprovement())
	{
		iLevel += GC.getImprovementInfo(getImprovementType()).getSeeFrom();
	}
	// Super Forts end

	if (isPeak())
		iLevel += GC.getDefineINT(CvGlobals::PEAK_SEE_FROM_CHANGE);
	if (isHills())
		iLevel += GC.getDefineINT(CvGlobals::HILLS_SEE_FROM_CHANGE);
	if (isWater())
	{
		iLevel += GC.getDefineINT(CvGlobals::SEAWATER_SEE_FROM_CHANGE);
		if (GET_TEAM(eTeam).isExtraWaterSeeFrom())
			iLevel++;
	}
	return iLevel;
}


int CvPlot::seeThroughLevel() const
{
	int iLevel = GC.getInfo(getTerrainType()).getSeeThroughLevel();
	if (isFeature())
		iLevel += GC.getInfo(getFeatureType()).getSeeThroughChange();
	switch (getPlotType()) // advc.opt: was a sequence of conditionals
	{
	case PLOT_HILLS:
		iLevel += GC.getDefineINT(CvGlobals::HILLS_SEE_THROUGH_CHANGE);
		break;
	case PLOT_PEAK:
		iLevel += GC.getDefineINT(CvGlobals::PEAK_SEE_THROUGH_CHANGE);
		break;
	case PLOT_OCEAN:
		iLevel += GC.getDefineINT(CvGlobals::SEAWATER_SEE_FROM_CHANGE);
		break;
	}
	return iLevel;
}


void CvPlot::changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement,
	CvUnit const* pUnit, bool bUpdatePlotGroups)
{
	PROFILE_FUNC(); // advc: See comment in canSeeDisplacementPlot

	bool const bAerial = (pUnit != NULL && pUnit->getDomainType() == DOMAIN_AIR);

	DirectionTypes eFacingDirection = NO_DIRECTION;
	if (!bAerial && pUnit != NULL)
		eFacingDirection = pUnit->getFacingDirection(true);

	// fill invisible types
	std::vector<InvisibleTypes> aeSeeInvisibleTypes;
	if (pUnit != NULL)
	{
		for (int i = 0; i < pUnit->getNumSeeInvisibleTypes(); i++)
			aeSeeInvisibleTypes.push_back(pUnit->getSeeInvisibleType(i));
	}
	if (aeSeeInvisibleTypes.empty())
		aeSeeInvisibleTypes.push_back(NO_INVISIBLE);

	// check one extra outer ring
	if (!bAerial)
		iRange++;

	for (size_t i = 0; i < aeSeeInvisibleTypes.size(); i++)
	{
		for (int iDX = -iRange; iDX <= iRange; iDX++)
		{
			for (int iDY = -iRange; iDY <= iRange; iDY++)
			{
				// check if in facing direction
				if (bAerial ||
					shouldProcessDisplacementPlot(iDX, iDY, /*iRange - 1,*/ eFacingDirection))
				{
					bool bOuterRing = false;
					if (abs(iDX) == iRange || abs(iDY) == iRange)
						bOuterRing = true;

					// check if anything blocking the plot
					if (bAerial ||
						canSeeDisplacementPlot(eTeam, iDX, iDY, iDX, iDY, true, bOuterRing))
					{
						CvPlot* pPlot = plotXY(getX(), getY(), iDX, iDY);
						if (pPlot != NULL)
						{
							pPlot->changeVisibilityCount(eTeam, bIncrement ? 1 : -1,
									aeSeeInvisibleTypes[i], bUpdatePlotGroups,
									pUnit); // advc.071
						}
					}
				}

				if (eFacingDirection != NO_DIRECTION)
				{	// always reveal adjacent plots when using line of sight
					if (abs(iDX) <= 1 && abs(iDY) <= 1)
					{
						CvPlot* pPlot = plotXY(getX(), getY(), iDX, iDY);
						if (pPlot != NULL)
						{
							pPlot->changeVisibilityCount(
									eTeam, 1, aeSeeInvisibleTypes[i], bUpdatePlotGroups,
									pUnit); // advc.071
							pPlot->changeVisibilityCount(
									eTeam, -1, aeSeeInvisibleTypes[i], bUpdatePlotGroups,
									pUnit); // advc.071
						}
					}
				}
			}
		}
	}
}


bool CvPlot::canSeePlot(CvPlot const* pPlot, TeamTypes eTeam, int iRange,
	DirectionTypes eFacingDirection) const
{
	PROFILE_FUNC(); // advc.test: See comment in canSeeDisplacementPlot
	if (pPlot == NULL)
		return false;

	iRange++;

	//find displacement
	int dx = pPlot->getX() - getX();
	int dy = pPlot->getY() - getY();
	CvMap const& kMap = GC.getMap();
	dx = kMap.dxWrap(dx); //world wrap
	dy = kMap.dyWrap(dy);

	//check if in facing direction
	if (shouldProcessDisplacementPlot(dx, dy,/* iRange - 1,*/ eFacingDirection))
	{
		bool bOuterRing = false;
		if (abs(dx) == iRange || abs(dy) == iRange)
			bOuterRing = true;

		//check if anything blocking the plot
		if (canSeeDisplacementPlot(eTeam, dx, dy, dx, dy, true, bOuterRing))
			return true;
	}

	return false;
}


namespace
{
	//sign function taken from FirePlace - JW
	//template<class T> __forceinline T getSign( T x ) { return (( x < 0 ) ? T(-1) : x > 0 ? T(1) : T(0)); }
	// advc: Moved from CvGameCoreUtils.h b/c it was only used here. Then replaced with:
	__inline int getSign(int x) { return (x > 0) - (x < 0); }
}
bool CvPlot::canSeeDisplacementPlot(TeamTypes eTeam, int iDX, int iDY,
	int iOriginalDX, int iOriginalDY, bool bFirstPlot, bool bOuterRing) const
{
	/*	advc (note): This gets called very frequently.
		Could adopt some optimizations from "We the People":
		cache seeThroughLevel(), remove support for direction of sight
		(in changeAdjacentSight). But I don't think it's quite bad enough to bother. */
	//PROFILE_FUNC();

	CvPlot const* pPlot = ::plotXY(getX(), getY(), iDX, iDY);
	if (pPlot == NULL)
		return false;

	// base case is current plot
	if(iDX == 0 && iDY == 0)
		return true;

	/*	find closest of three points (1, 2, 3) to original line
		from Start (S) to End (E).
		The diagonal is computed first as that guarantees a change in position.
		-------------
		|   | 2 | S |
		-------------
		| E | 1 | 3 |
		------------- */
	int aaiDisplacements[3][2] =
	{
		{iDX - getSign(iDX), iDY - getSign(iDY)},
		{iDX - getSign(iDX), iDY},
		{iDX, iDY - getSign(iDY)}
	};
	int aiAllClosest[3];
	int iClosest = -1;
	for (int i = 0; i < 3; i++)
	{
		//more accurate, but less structured on a grid
		//int iTempClosest = abs(aaiDisplacements[i][0] * iOriginalDX - aaiDisplacements[i][1] * iOriginalDY);
		// cross product
		aiAllClosest[i] = abs(aaiDisplacements[i][0] * iDY
				- aaiDisplacements[i][1] * iDX);
		if(iClosest == -1 || aiAllClosest[i] < iClosest)
			iClosest = aiAllClosest[i];
	}

	// iterate through all minimum plots to see if any of them are passable
	for (int i = 0; i < 3; i++)
	{
		int iNextDX = aaiDisplacements[i][0];
		int iNextDY = aaiDisplacements[i][1];
		if (iNextDX == iDX && iNextDY == iDY) // make sure we change plots
			continue;
		if (aiAllClosest[i] == iClosest && canSeeDisplacementPlot(
			eTeam, iNextDX, iNextDY, iOriginalDX, iOriginalDY, false, false))
		{
			int iFromLevel = seeFromLevel(eTeam);
			int iThroughLevel = pPlot->seeThroughLevel();
			if (bOuterRing) // check strictly higher level
			{
				CvPlot const* pPassThroughPlot = plotXY(
						getX(), getY(), iNextDX, iNextDY);
				int iPassThroughLevel = pPassThroughPlot->seeThroughLevel();
				if (iFromLevel >= iPassThroughLevel)
				{
					// either we can see through to it or it is high enough to see from far
					if(iFromLevel > iPassThroughLevel ||
						pPlot->seeFromLevel(eTeam) > iFromLevel)
					{
						return true;
					}
				}
			}
			else
			{
				// we can clearly see this level
				if (iFromLevel >= iThroughLevel)
					return true;
				// we can also see it if it is the first plot that is too tall
				if (bFirstPlot)
					return true;
			}
		}
	}

	return false;
}


bool CvPlot::shouldProcessDisplacementPlot(int iDX, int iDY, //int iRange, // advc: unused
	DirectionTypes eFacingDirection) const
{
	if (eFacingDirection == NO_DIRECTION)
		return true;

	if (iDX == 0 && iDY == 0) // always process this plot
		return true;

	//								N		NE		E		SE		 S			SW		W		NW
	int aaiDisplacements[8][2] = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
	int iDirectionX = aaiDisplacements[eFacingDirection][0];
	int iDirectionY = aaiDisplacements[eFacingDirection][1];

	// compute angle off of direction
	int iCrossProduct = iDirectionX * iDY - iDirectionY * iDX;
	int iDotProduct = iDirectionX * iDX + iDirectionY * iDY;

	float fTheta = atan2((float)iCrossProduct, (float)iDotProduct);
	float fSpread = 60 * (float)M_PI / 180;
	if (abs(iDX) <= 1 && abs(iDY) <= 1) // close plots use wider spread
		fSpread = 90 * (float)M_PI / 180;

	if (fTheta >= -fSpread / 2 && fTheta <= fSpread / 2)
		return true;
	return false;

	/*DirectionTypes eLeftDirection = GC.getTurnLeftDirection(eFacingDirection);
	DirectionTypes eRightDirection = GC.getTurnRightDirection(eFacingDirection);
	//test which sides of the line equation (cross product)
	int iLeftSide = aaiDdisplacements[eLeftDirection][0] * iDY - aaiDisplacements[eLeftDirection][1] * iDX;
	int iRightSide = displacements[eRightDirection][0] * iDY - displacements[rightDirection][1] * iDX;
	if(iLeftSide <= 0 && iRightSide >= 0)
		return true;
	return false;*/
}


void CvPlot::updateSight(bool bIncrement, bool bUpdatePlotGroups)
{
	PROFILE_FUNC(); // advc: Slow-ish, but only called a few hundred times per game turn.

	CvCity* pCity = getPlotCity();
	if (pCity != NULL)
	{	// Religion - Disabled with BtS Espionage System
		/*for (iI = 0; iI < GC.getNumReligionInfos(); ++iI) {
			if (pCity->isHasReligion((ReligionTypes)iI)) {
				CvCity* pHolyCity = GC.getGame().getHolyCity((ReligionTypes)iI);
				if (pHolyCity != NULL) {
					if (GET_PLAYER(pHolyCity->getOwner()).getStateReligion() == iI)
						changeAdjacentSight(pHolyCity->getTeam(), GC.getDefineINT(CvGlobals::PLOT_VISIBILITY_RANGE), bIncrement, NULL, bUpdatePlotGroups);
				} } }*/

		// Master
		// <advc.opt> Replacing a loop over "all" our masters
		if (GET_TEAM(getTeam()).isAVassal())
		{
			changeAdjacentSight(GET_TEAM(getTeam()).getMasterTeam(),
					GC.getDefineINT(CvGlobals::PLOT_VISIBILITY_RANGE),
					bIncrement, NULL, bUpdatePlotGroups);
		}

		// Espionage Effect
		if (pCity->isAnyEspionageVisibility()) // advc.opt
		{
			for (TeamIter<CIV_ALIVE> it; it.hasNext(); ++it)
			{
				if (pCity->getEspionageVisibility(it->getID()))
				{
					// Passive Effect: enough EPs provide visibility into someone's cities
					changeAdjacentSight(it->getID(),
							GC.getDefineINT(CvGlobals::PLOT_VISIBILITY_RANGE),
							bIncrement, NULL, bUpdatePlotGroups);
				}
			}
		}
	}

	// Owned
	if (isOwned())
	{
		changeAdjacentSight(getTeam(), GC.getDefineINT(CvGlobals::PLOT_VISIBILITY_RANGE),
				bIncrement, NULL, bUpdatePlotGroups);
	}

	// Unit
	FOR_EACH_UNIT_IN(pUnit, *this)
	{
		changeAdjacentSight(pUnit->getTeam(), pUnit->visibilityRange(),
				bIncrement, pUnit, bUpdatePlotGroups);
	}

	if (getReconCount() > 0)
	{
		int const iRange = GC.getDefineINT(CvGlobals::RECON_VISIBILITY_RANGE);
		for (PlayerIter<ALIVE> it; it.hasNext(); ++it) // advc: exclude dead
		{
			/*	<advc.opt> Going through all units to find the recon owner(s) seems
				pretty wasteful. Perhaps a little better: Go through groups
				and disregard non-air groups. */
			FOR_EACH_GROUP(pGroup, *it)
			{
				DomainTypes const eGroupDomain = pGroup->getDomainType();
				if (eGroupDomain != DOMAIN_AIR && eGroupDomain != DOMAIN_IMMOBILE)
					continue;
				FOR_EACH_UNIT_IN(pAirUnit, *pGroup) // </advc.opt>
				{
					if (pAirUnit->getReconPlot() == this)
					{
						changeAdjacentSight(pAirUnit->getTeam(), iRange, bIncrement,
								pAirUnit, bUpdatePlotGroups);
					}
				}
			}
		}
	}
}

// advc.003h: Cut and pasted from CvPlot::updateSeeFromSight
void CvPlot::setMaxVisibilityRangeCache()
{
	int iRange = GC.getDefineINT(CvGlobals::UNIT_VISIBILITY_RANGE) + 1;
	for(int iPromotion = 0; iPromotion < GC.getNumPromotionInfos(); iPromotion++)
		iRange += GC.getInfo((PromotionTypes)iPromotion).getVisibilityChange();
	iRange = std::max(GC.getDefineINT(CvGlobals::RECON_VISIBILITY_RANGE) + 1, iRange);
	m_iMaxVisibilityRangeCache = iRange;
}


void CvPlot::updateSeeFromSight(bool bIncrement, bool bUpdatePlotGroups)
{
	for (SquareIter it(*this, /* advc.003h: */ m_iMaxVisibilityRangeCache);
		it.hasNext(); ++it)
	{
		it->updateSight(bIncrement, bUpdatePlotGroups);
	}
}


bool CvPlot::canHaveBonus(BonusTypes eBonus, bool bIgnoreLatitude,
	bool bIgnoreFeature, // advc.129
	bool bIgnoreCurrentBonus) const // advc.tsl
{
	if (eBonus == NO_BONUS)
		return true;

	if (!bIgnoreCurrentBonus && // advc.tsl
		getBonusType() != NO_BONUS)
		return false;
//Mountains Start
	if (!GC.getGame().isOption(GAMEOPTION_MOUNTAINS))
	{
		if (isPeak())
		{
			return false;
		}
	}
	CvBonusInfo const& kBonus = GC.getInfo(eBonus);
	// <advc.129>
	if(bIgnoreFeature)
	{
		if(!kBonus.isFeatureTerrain(getTerrainType()) &&
			!kBonus.isTerrain(getTerrainType()))
		{
			return false;
		}
	}
	else /* </advc.129> */ if (isFeature())
	{
		if (!kBonus.isFeature(getFeatureType()))
			return false;
		if (!kBonus.isFeatureTerrain(getTerrainType()))
			return false;
	}
	else if (!kBonus.isTerrain(getTerrainType()))
		return false;

	if (isHills())
	{
		if (!kBonus.isHills())
			return false;
	}
//Mountains Start
	else if (isPeak())
	{
		if (GC.getGame().isOption(GAMEOPTION_MOUNTAINS))//AND Option
		{
			if (!(kBonus.isPeaks()))
			{
				return false;
			}
		}
	}
	else if (isFlatlands())
	{
		if (!kBonus.isFlatlands())
			return false;
	}

	if (kBonus.isNoRiverSide() && isRiverSide())
		return false;

	if (kBonus.getMinAreaSize() != -1)
	{
		if (getArea().getNumTiles() < kBonus.getMinAreaSize())
			return false;
	}

	if (!bIgnoreLatitude)
	{
		if (getLatitude() > kBonus.getMaxLatitude())
			return false;

		if (getLatitude() < kBonus.getMinLatitude())
			return false;
	}

	if (!isPotentialCityWork())
		return false;

	return true;
}


bool CvPlot::canHaveImprovement(ImprovementTypes eImprovement, TeamTypes eTeam, bool bPotential,
	BuildTypes eBuild, bool bAnyBuild) const // kekm.9
{
	/*  K-Mod, 21/dec/10, karadoc
		changed to check for NO_IMPROVEMENT rather than just assume the input is an actual improvement */
	// FAssertMsg(eImprovement != NO_IMPROVEMENT, "Improvement is not assigned a valid value");
	if (eImprovement == NO_IMPROVEMENT)
		return true;
	// K-Mod end

	// <kekm.9>
	FAssertMsg(!bAnyBuild || eBuild == NO_BUILD,
			"expected: if bAnyBuild is true then eBuild is NO_BUILD");
	FAssertMsg(eBuild == NO_BUILD ||
			GC.getInfo(eBuild).getImprovement() == eImprovement,
			"expected that eBuild matches eImprovement");
	// </kekm.9>

	if (isCity())
		return false;

	if (isImpassable())
		return false;

	if (GC.getInfo(eImprovement).isWater() != isWater())
		return false;

	if (isFeature())
	{
		if (GC.getInfo(getFeatureType()).isNoImprovement())
			return false;
	}

	if (getBonusType(eTeam) != NO_BONUS && GC.getInfo(eImprovement).isImprovementBonusMakesValid(getBonusType(eTeam)))
		return true;

	if (GC.getInfo(eImprovement).isNoFreshWater() && isFreshWater())
		return false;

	if (GC.getInfo(eImprovement).isRequiresFlatlands() && !isFlatlands())
		return false;

	if (GC.getInfo(eImprovement).isRequiresFeature() && !isFeature())
		return false;
	{
		bool bValid = false;
		if (GC.getInfo(eImprovement).isHillsMakesValid() && isHills())
			bValid = true;
//===NM=====Mountains Mod===0=====
		if (GC.getGame().isOption(GAMEOPTION_MOUNTAINS))
		{
			if (GC.getInfo(eImprovement).isPeakMakesValid() && isPeak())
				bValid = true;
// davidlallen: mountain limitations start
			if (GC.getInfo(eImprovement).isPeakMakesInvalid() && isPeak())
			//	return false;
				bValid = false;
// davidlallen: mountain limitations end
		}
//===NM=====Mountains Mod===X=====
		if (GC.getInfo(eImprovement).isFreshWaterMakesValid() && isFreshWater())
			bValid = true;
		if (GC.getInfo(eImprovement).isRiverSideMakesValid() && isRiverSide())
			bValid = true;
		if (GC.getInfo(eImprovement).getTerrainMakesValid(getTerrainType()))
			bValid = true;
		if (isFeature() && GC.getInfo(eImprovement).getFeatureMakesValid(getFeatureType()))
			bValid = true;
		if (!bValid)
			return false;
	}
	if (GC.getInfo(eImprovement).isRequiresRiverSide())
	{
		bool bValid = false;
		FOR_EACH_ORTH_ADJ_PLOT(*this)
		{
			if (pAdj->getImprovementType() != eImprovement &&
				isRiverCrossing(directionXY(*this, *pAdj)))
			{
				bValid = true;
				break;
			}
		}
		if (!bValid)
			return false;
	}

	/*for (iI = 0; iI < NUM_YIELD_TYPES; ++iI) {
		if (calculateNatureYield(((YieldTypes)iI), eTeam) < GC.getInfo(eImprovement).getPrereqNatureYield(iI))
			return false;
	}*/
	// <kekm.9> Replacing the above
	bool bFound = false;
	bool bBuildable = false;
	if (eBuild == NO_BUILD && !bAnyBuild)
	{
		FOR_EACH_ENUM(Yield)
		{
			if (calculateNatureYield(eLoopYield, eTeam) <
				GC.getInfo(eImprovement).getPrereqNatureYield(eLoopYield))
			{
				return false;
			}
		}
	}
	else if (eBuild != NO_BUILD)
	{
		FOR_EACH_ENUM(Yield)
		{
			if (calculateNatureYield(eLoopYield, eTeam, !isFeature() ||
				GC.getInfo(eBuild).isFeatureRemove(getFeatureType())) <
				GC.getInfo(eImprovement).getPrereqNatureYield(eLoopYield))
			{
				return false;
			}
		}
	}
	else
	{
		FOR_EACH_ENUM(Build)
		{
			CvBuildInfo const& kLoopBuild = GC.getInfo(eLoopBuild);
			if (kLoopBuild.getImprovement() == eImprovement)
			{
				bBuildable = true;
				bool bValid = true;
				FOR_EACH_ENUM(Yield)
				{
					if (calculateNatureYield(eLoopYield, eTeam, !isFeature() ||
						kLoopBuild.isFeatureRemove(getFeatureType())) <
						GC.getInfo(eImprovement).getPrereqNatureYield(eLoopYield))
					{
						bValid = false;
						break;
					}
				}
				if (bValid)
				{
					bFound = true;
					break;
				}
			}
		}

		if (bBuildable && !bFound)
			return false;
 	} // </kekm.9>

	if (getTeam() == NO_TEAM || !GET_TEAM(getTeam()).isIgnoreIrrigation())
	{
		if (!bPotential && GC.getInfo(eImprovement).isRequiresIrrigation() &&
			!isIrrigationAvailable())
		{
			return false;
		}
	}

	return true;
}

// < JImprovementLimit Mod Start >
bool CvPlot::isImprovementAncestor(ImprovementTypes eImprovement, ImprovementTypes eCheckImprovement) const
{
    if (eImprovement == NO_IMPROVEMENT || eCheckImprovement == NO_IMPROVEMENT)
    {
        return false;
    }
    ImprovementTypes eLoopReqImprovement;
    ImprovementTypes eLastImprovement = eCheckImprovement;
    bool bFound = false;
    while (true)
    {
        eLoopReqImprovement = (ImprovementTypes) GC.getImprovementInfo(eLastImprovement).getImprovementRequired();
        if (eLoopReqImprovement == NO_IMPROVEMENT)
        {
            break;
        }
        if (eImprovement == eLoopReqImprovement)
        {
            bFound = true;
            break;
        }
        else
        {
            eLastImprovement = eLoopReqImprovement;
        }
    }
    return bFound;
}

bool CvPlot::isImprovementInRange(ImprovementTypes eImprovement, int iRange, bool bCheckBuildProgress) const
{
    if (eImprovement == NO_IMPROVEMENT)
    {
        return false;
    }
    if (iRange < 1)
    {
        return false;
    }
    ImprovementTypes eLoopImprovement;
    // Should prob make eBuild into an array of eBuilds so we can check the progress on all Builds that build this eImprovement
    BuildTypes eBuild = NO_BUILD;
    if (bCheckBuildProgress)
    {
        for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
        {
            eLoopImprovement = (ImprovementTypes) GC.getBuildInfo((BuildTypes) iI).getImprovement();
            if (eLoopImprovement == eImprovement)
            {
                eBuild = ((BuildTypes) iI);
                break;
            }
        }
    }
    int iDX, iDY;
    CvPlot* pLoopPlot;
    bool bInvalid = false;
    for (iDX = -iRange; iDX <= iRange; iDX++)
    {
        for (iDY = -iRange; iDY <= iRange; iDY++)
        {
            if (iDX == 0 && iDY == 0)
            {
                continue;
            }
            pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
            if (pLoopPlot != NULL)
            {
                if (pLoopPlot->getImprovementType() == eImprovement)
                {
                    bInvalid = true;
                    break;
                }
                else if (bCheckBuildProgress && eBuild != NO_BUILD)
                {
                    if (pLoopPlot->getBuildProgress(eBuild) > 0)
                    {
                        bInvalid = true;
                        break;
                    }
                }
            }
        }
    }
    if (bInvalid)
    {
        return true;
    }
    return false;
}
// < JImprovementLimit Mod End >
bool CvPlot::canBuild(BuildTypes eBuild, PlayerTypes ePlayer, bool bTestVisible,
	bool bIgnoreFoW) const // advc.181
{
	if (eBuild == NO_BUILD)
		return false;
	{
		bool bPyOverride=false;
		bool bCanBuild = GC.getPythonCaller()->canBuild(*this, eBuild, ePlayer, bPyOverride);
		if (bPyOverride)
			return bCanBuild;
	}
	bool bValid = false;
	ImprovementTypes const eImprovement = GC.getInfo(eBuild).getImprovement();
	if (eImprovement != NO_IMPROVEMENT)
	{
		if (!canHaveImprovement(eImprovement, TEAMID(ePlayer), bTestVisible,
			eBuild, false)) // kekm.9
		{
			return false;
		}
		ImprovementTypes const eOldImprov = (bIgnoreFoW ? getImprovementType() :
				getRevealedImprovementType(TEAMID(ePlayer)));
// Super Forts begin *build*
		if (GC.getGame().isOption(GAMEOPTION_SUPER_FORTS) && GC.getInfo(eImprovement).getUniqueRange() > 0)
		{
			int iUniqueRange = GC.getImprovementInfo(eImprovement).getUniqueRange();
			for (SquareIter it(*this, iUniqueRange); it.hasNext(); ++it)
			{
				CvPlot& pLoopPlot = *it;
				if (&pLoopPlot != NULL && pLoopPlot.getImprovementType() != NO_IMPROVEMENT)
				{
					//doto qa108 super forts - advc changed finalimprovementupgrade i copied other ref i found and used it like this.
					//no idea if its ok..
					if (CvImprovementInfo::finalUpgrade(pLoopPlot.getImprovementType()) == CvImprovementInfo::finalUpgrade((ImprovementTypes)eImprovement))
					{
						return false;
					}
				}
			}
		}
// Super Forts end
		if (eOldImprov != NO_IMPROVEMENT) // advc.181
		//if (isImproved())
		{
			if (GC.getInfo(eOldImprov).isPermanent())
				return false;
			// Super Forts begin *AI_worker* - prevent forts from being built over when outside culture range
			if (isFortImprovement() //doto change
				/*GC.getImprovementInfo(getImprovementType()).isActsAsCity()*/)
			{
				if (!isWithinCultureRange(ePlayer) && !(getCultureRangeForts(ePlayer) > 1))
				{
					return false;
				}
			}
			// Super Forts end	
			if (eOldImprov == eImprovement)
				return false;
			ImprovementTypes eFinalImprovementType = CvImprovementInfo::
					finalUpgrade(eOldImprov);
			if (eFinalImprovementType != NO_IMPROVEMENT)
			{
				if (eFinalImprovementType == CvImprovementInfo::finalUpgrade(eImprovement))
					return false;
			}
		}
 // < JImprovementLimit Mod Start >
        ImprovementTypes eReqImprovement = (ImprovementTypes) GC.getImprovementInfo(eImprovement).getImprovementRequired();
        if (eReqImprovement != NO_IMPROVEMENT)
        {
            if (getImprovementType() != eReqImprovement)
            {
                return false;
            }
        }
// < JImprovementLimit Mod End >
		if (!bTestVisible)
		{
// < JImprovementLimit Mod Start >
//ImprovementTypes eImprovement = (ImprovementTypes) GC.getBuildInfo(eBuild).getImprovement();
            if (eImprovement != NO_IMPROVEMENT)
            {
                if (GC.getImprovementInfo(eImprovement).getMakesInvalidRange() > 0)
                {
                    if (isImprovementInRange(eImprovement, GC.getImprovementInfo(eImprovement).getMakesInvalidRange(), true))
                    {
                        return false;
                    }
                }

                if (GC.getImprovementInfo(eImprovement).isNotInsideBorders())
                {
                    if (getOwner() != NO_PLAYER)
                    {
                        return false;
                    }
                }
            }
            // < JImprovementLimit Mod End >
			if (TEAMID(ePlayer) != getTeam())
			{
				//outside borders can't be built in other's culture
				if (GC.getInfo(eImprovement).isOutsideBorders())
				{
					if (getTeam() != NO_TEAM)
						return false;
				}
				else return false; //only buildable in own culture
			}
// Super Forts begin *AI_worker* - prevent workers from two different players from building a fort in the same plot
			if(isFortImprovement()//doto change
				/*GC.getImprovementInfo(eImprovement).isActsAsCity()*/)
			{
				CLinkList<IDInfo> oldUnits;
				CLLNode<IDInfo>* pUnitNode = headUnitNode();
				CvUnit* pLoopUnit;

				while (pUnitNode != NULL)
				{
					oldUnits.insertAtEnd(pUnitNode->m_data);
					pUnitNode = nextUnitNode(pUnitNode);
				}

				pUnitNode = oldUnits.head();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = nextUnitNode(pUnitNode);
					if(pLoopUnit->getOwner() != ePlayer)	
					{
						if(pLoopUnit->getBuildType() != NO_BUILD)
						{
							ImprovementTypes eImprovementBuild = (ImprovementTypes)(GC.getBuildInfo(pLoopUnit->getBuildType()).getImprovement());
							if(eImprovementBuild != NO_IMPROVEMENT)
							{
								if(GC.getImprovementInfo(eImprovementBuild).isActsAsCity())
								{
									return false;
								}
							}
						}
					}
				}
			}
// Super Forts end
		}
		bValid = true;
	}

	RouteTypes const eRoute = GC.getInfo(eBuild).getRoute();
	if (eRoute != NO_ROUTE)
	{
		// <advc.181>
		RouteTypes const eOldRoute = (bIgnoreFoW ? getRouteType() :
				getRevealedRouteType(TEAMID(ePlayer)));
		if (eOldRoute != NO_ROUTE) // </advc.181>
		//if (isRoute())
		{
			if (GC.getInfo(eOldRoute).getValue() >= GC.getInfo(eRoute).getValue())
				return false;
		}
		if (!bTestVisible)
		{
			if (GC.getInfo(eRoute).getPrereqBonus() != NO_BONUS)
			{
				if (!isAdjacentPlotGroupConnectedBonus(ePlayer,
					GC.getInfo(eRoute).getPrereqBonus()))
				{
					return false;
				}
			}
			bool bFoundValid = true;
			for (int i = 0; i < GC.getInfo(eRoute).getNumPrereqOrBonuses(); i++)
			{
				bFoundValid = false;
				if (isAdjacentPlotGroupConnectedBonus(ePlayer,
					GC.getInfo(eRoute).getPrereqOrBonus(i)))
				{
					bFoundValid = true;
					break;
				}
			}
			if (!bFoundValid)
				return false;
		}
		bValid = true;
	}

	if (isFeature())
	{
		if (GC.getInfo(eBuild).isFeatureRemove(getFeatureType()))
		{	/*if (isOwned() && (GET_PLAYER(ePlayer).getTeam() != getTeam()) && !atWar(GET_PLAYER(ePlayer).getTeam(), getTeam()))
				return false;
			bValid = true;*/
			// <advc.119> Replacing the above
			if(getTeam() == TEAMID(ePlayer))
				bValid = true;
			else return false; // </advc.119>
		}
	}

	return bValid;
}


int CvPlot::getBuildTime(BuildTypes eBuild, /* advc.251: */ PlayerTypes ePlayer) const
{
	int iTime = GC.getInfo(eBuild).getTime();
	if (isFeature())
		iTime += GC.getInfo(eBuild).getFeatureTime(getFeatureType());

//===NM=====Mountains Mod===0=====
	if (isPeak() && GC.getGame().isOption(GAMEOPTION_MOUNTAINS))
	{
		iTime *= std::max(0, (GC.getDefineINT("PEAK_BUILD_TIME_MODIFIER") + 100));
		iTime /= 100;
	}
//===NM=====Mountains Mod===X=====
	iTime *= std::max(0, GC.getInfo(getTerrainType()).getBuildModifier() + 100);
	iTime /= 100;
	// <advc.251>
	iTime = (int)(GC.getInfo(GET_PLAYER(ePlayer).getHandicapType()).
			getBuildTimePercent() * 0.01 * iTime);
	iTime -= (iTime % 50); // Round down to a multiple of 50
	// </advc.251>
	iTime *= GC.getInfo(GC.getGame().getGameSpeedType()).getBuildPercent();
	iTime /= 100;

	iTime *= GC.getInfo(GC.getGame().getStartEra()).getBuildPercent();
	iTime /= 100;

	return iTime;
}


int CvPlot::getBuildTurnsLeft(BuildTypes eBuild, /* advc.251: */ PlayerTypes ePlayer,
	int iNowExtra, int iThenExtra, /* advc.011c: */ bool bIncludeUnits) const
{
	int iNowBuildRate = iNowExtra;
	int iThenBuildRate = iThenExtra;

	if (bIncludeUnits) // advc.011c
	{
		FOR_EACH_UNIT_IN(pUnit, *this)
		{
			if (pUnit->getBuildType() == eBuild)
			{
				if (pUnit->canMove())
					iNowBuildRate += pUnit->workRate(false);
				iThenBuildRate += pUnit->workRate(true);
			}
		}
	}
	if (iThenBuildRate == 0)
	{
		//this means it will take forever under current circumstances
		return MAX_INT;
	}

	int iBuildLeft = getBuildTime(eBuild, /* advc.251: */ ePlayer);
	iBuildLeft -= getBuildProgress(eBuild);
	iBuildLeft -= iNowBuildRate;

	iBuildLeft = std::max(0, iBuildLeft);

	int iTurnsLeft = iBuildLeft / iThenBuildRate;
	if (iTurnsLeft * iThenBuildRate < iBuildLeft)
		iTurnsLeft++;
	iTurnsLeft++;

	return std::max(1, iTurnsLeft);
}

// advc.011c:
int CvPlot::getBuildTurnsLeft(BuildTypes eBuild, PlayerTypes ePlayer) const
{
	int iWorkRate = GET_PLAYER(ePlayer).getWorkRate(eBuild);
	if(iWorkRate > 0)
	{
		return getBuildTurnsLeft(eBuild, /* advc.251: */ ePlayer,
				iWorkRate, iWorkRate, false);
	}
	return MAX_INT;
}


int CvPlot::getFeatureProduction(BuildTypes eBuild, TeamTypes eTeam, CvCity** ppCity,
	CvPlot const* pCityPlot, PlayerTypes eCityOwner) const // advc.031
{
	*ppCity = NULL; // advc: Don't rely on caller to do this
	if (!isFeature())
		return 0;
	// <advc.031>
	FAssert((pCityPlot == NULL) == (eCityOwner == NO_PLAYER));
	FAssert(eCityOwner == NO_PLAYER || TEAMID(eCityOwner) == eTeam);
	int iPopulation = 1;
	if (pCityPlot == NULL) // </advc.031>
	{
		*ppCity = getWorkingCity();
		if (*ppCity == NULL)
			*ppCity = GC.getMap().findCity(getX(), getY(), NO_PLAYER, eTeam, false);
		if (*ppCity == NULL)
			return 0;
		// <advc.031>
		CvCity const& kCity = **ppCity;
		pCityPlot = kCity.plot();
		eCityOwner = kCity.getOwner();
		iPopulation = kCity.getPopulation(); // </advc.031>
	}
	
	int iProduction = (GC.getInfo(eBuild).getFeatureProduction(getFeatureType()) -
			std::max(0, plotDistance(this, pCityPlot) - 2) * 5);

	iProduction *= std::max(0, GET_PLAYER(eCityOwner).getFeatureProductionModifier() + 100);
	iProduction /= 100;

	iProduction *= GC.getInfo(GC.getGame().getGameSpeedType()).getFeatureProductionPercent();
	iProduction /= 100;
	// <advc.opt>
	static int const iBASE_FEATURE_PRODUCTION_PERCENT = GC.getDefineINT("BASE_FEATURE_PRODUCTION_PERCENT");
	static int const iFEATURE_PRODUCTION_PERCENT_MULTIPLIER = GC.getDefineINT("FEATURE_PRODUCTION_PERCENT_MULTIPLIER");
	static int const iDIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT = GC.getDefineINT("DIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT");
	// </advc.opt>
	iProduction *= std::min(100, iBASE_FEATURE_PRODUCTION_PERCENT +
			iFEATURE_PRODUCTION_PERCENT_MULTIPLIER * iPopulation);
	iProduction /= 100;

	if (getTeam() != eTeam)
	{
		iProduction *= iDIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT;
		iProduction /= 100;
	}

	return std::max(0, iProduction);
}


CvUnit* CvPlot::getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer,
	CvUnit const* pAttacker, bool bTestEnemy, bool bTestPotentialEnemy,
	/* advc.028: */ bool bTestVisible,
	bool bTestCanAttack, bool bTestAny) const // advc: new params (for CvPlot::hasDefender)
{
	// <advc> Ensure consistency of parameters
	if (pAttacker != NULL)
	{
		FAssert(pAttacker->getOwner() == eAttackingPlayer);
		eAttackingPlayer = pAttacker->getOwner();
	}
	// isEnemy implies isPotentialEnemy
	FAssert(!bTestEnemy || !bTestPotentialEnemy); // </advc>
	// BETTER_BTS_AI_MOD, Lead From Behind (UncutDragon), 02/21/10, jdog5000
	int iBestUnitRank = -1;
	CvUnit* pBestUnit = NULL;
	FOR_EACH_UNIT_VAR_IN(pLoopUnit, *this)
	{
		CvUnit& kUnit = *pLoopUnit;
		if (eOwner != NO_PLAYER && kUnit.getOwner() != eOwner)
			continue;
		if (kUnit.isCargo()) // advc: Was previously only checked with bTestCanMove - i.e. never.
			continue;
		// <advc> Moved the other conditions into CvUnit::canBeAttackedBy (new function)
		if(eAttackingPlayer == NO_PLAYER || kUnit.canBeAttackedBy(eAttackingPlayer,
			pAttacker, bTestEnemy, bTestPotentialEnemy, /* advc.028: */ bTestVisible,
			bTestCanAttack))
		{
			if (bTestAny)
				return &kUnit; // </advc>
			if (kUnit.isBetterDefenderThan(pBestUnit, pAttacker,
				&iBestUnitRank, // UncutDragon
				bTestVisible)) // advc.061
			{
				pBestUnit = &kUnit;
			}
		}
	}
	// BETTER_BTS_AI_MOD: END
	return pBestUnit;
}


CvUnit* CvPlot::getSelectedUnit() const
{
	FOR_EACH_UNIT_VAR_IN(pUnit, *this)
	{
		if (pUnit->IsSelected())
			return pUnit;
	}
	return NULL;
}


int CvPlot::getUnitPower(PlayerTypes eOwner) const
{
	int iCount = 0;
	FOR_EACH_UNIT_IN(pUnit, *this)
	{
		if (eOwner == NO_PLAYER || pUnit->getOwner() == eOwner)
			iCount += pUnit->getUnitInfo().getPowerValue();
	}
	return iCount;
}


// Super Forts begin *bombard*
//doto keldath change - added the plot to be checked for isenemy.
bool CvPlot::isBombardable(const CvUnit* pUnit, CvPlot* kPlot) const
{	
	ImprovementTypes eImprovement;

	if (pUnit != NULL && 
		!pUnit->isEnemy(getTeam(), *this)
	//	pUnit->isEnemy(kPlot->getTeam(), kPlot)
		)
	{
		return false;
	}

	eImprovement = getImprovementType();
	if(eImprovement == NO_IMPROVEMENT)
	{
		return false;
	}
	else
	{
		if(GC.getImprovementInfo(eImprovement).isBombardable())
		{
			return (getDefenseDamage() < GC.getImprovementInfo(eImprovement).getDefenseModifier());
		}
	}
	return false;
}

bool CvPlot::isBombarded() const
{
	return m_bBombarded;
}

void CvPlot::setBombarded(bool bNewValue)
{
	m_bBombarded = bNewValue;
}

int CvPlot::getDefenseDamage() const																
{
	return m_iDefenseDamage;
}

void CvPlot::changeDefenseDamage(int iChange)
{
	if ((iChange != 0) && (getImprovementType() != NO_IMPROVEMENT))
	{
		m_iDefenseDamage = range((m_iDefenseDamage + iChange), 0, GC.getImprovementInfo(getImprovementType()).getDefenseModifier());

		if (iChange > 0)
		{
			setBombarded(true);
		}
	}
}
// Super Forts end

// Super Forts begin *culture*
int CvPlot::getCultureRangeForts(PlayerTypes ePlayer) const
{
	/* removed - now enuymmap
	if (NULL == m_aiCultureRangeForts)
	{
		return 0;
	}
	*/
	return m_aiCultureRangeForts.get(ePlayer);
}

void CvPlot::setCultureRangeForts(PlayerTypes ePlayer, int iNewValue)
{
	if (getCultureRangeForts(ePlayer) != iNewValue)
	{
		//sagis trial
		if (iNewValue == -1)
		{
			//m_aiCultureRangeForts[ePlayer] = 0;
			m_aiCultureRangeForts.set(ePlayer,0);
			changeCulture(ePlayer, 0, false);
			return;
		}
		//removed - did the same for culture control - uses enumaps - so its not req
		/*if(NULL == m_aiCultureRangeForts)
		else {
			//m_aiCultureRangeForts = new short[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				//m_aiCultureRangeForts[iI] = 0;
				//if (GET_PLAYER((PlayerTypes)iI).isAlive())
				m_aiCultureRangeForts.set(GET_PLAYER((PlayerTypes)iI, 0)
			}
		}*/

		//keldath advc change to enumap
		//m_aiCultureRangeForts[ePlayer] = iNewValue;
		m_aiCultureRangeForts.set(ePlayer,iNewValue);
		if(getCulture(ePlayer) == 0)
		{
			changeCulture(ePlayer, 1, false);
		}
	}
}

void CvPlot::changeCultureRangeForts(PlayerTypes ePlayer, int iChange)
{
	if (0 != iChange)
	{
		setCultureRangeForts(ePlayer, (getCultureRangeForts(ePlayer) + iChange));
	}
}

bool CvPlot::isWithinFortCultureRange(PlayerTypes ePlayer) const
{
	return (getCultureRangeForts(ePlayer) > 0);
}

void CvPlot::changeCultureRangeFortsWithinRange(PlayerTypes ePlayer, int iChange, int iRange, bool bUpdate)
{
	CvPlot* pLoopPlot;
	int iDX, iDY;
	int iCultureDistance;

	if ((0 != iChange) && (iRange >= 0))
	{
		for (iDX = -iRange; iDX <= iRange; iDX++)
		{
			for (iDY = -iRange; iDY <= iRange; iDY++)
			{
				iCultureDistance = plotDistance(0, 0, iDX, iDY);

				if(iCultureDistance <= iRange)
				{
					pLoopPlot = plotXY(getX(), getY(), iDX, iDY);

					if (pLoopPlot != NULL)
					{
						pLoopPlot->changeCultureRangeForts(ePlayer, iChange);

						if(bUpdate)
						{
							pLoopPlot->updateCulture(true,true);
						}
					}
				}
			}
		}
	}
}

void CvPlot::doImprovementCulture()
{
	CvPlot* pLoopPlot;
	int iDX, iDY;
	int iCultureDistance, iCulture, iCultureRange;
	ImprovementTypes eImprovement;
	PlayerTypes ePlayer;

	eImprovement = getImprovementType();
	if (eImprovement != NO_IMPROVEMENT)
	{
		ePlayer = getOwner();
		if(ePlayer != NO_PLAYER)
		{
			iCulture = GC.getImprovementInfo(eImprovement).getCulture();
			if(iCulture > 0)
			{
				iCultureRange = GC.getImprovementInfo(eImprovement).getCultureRange();
				
				if(iCultureRange > 0)
				{
					for (iDX = -iCultureRange; iDX <= iCultureRange; iDX++)
					{
						for (iDY = -iCultureRange; iDY <= iCultureRange; iDY++)
						{
							iCultureDistance = plotDistance(0, 0, iDX, iDY);

							if(iCultureDistance <= iCultureRange)
							{
								pLoopPlot = plotXY(getX(), getY(), iDX, iDY);

								if (pLoopPlot != NULL)
								{
									int iChange = ((iCultureRange - ((iCultureDistance == 0) ? 1 : iCultureDistance))*iCulture) + iCulture;
									pLoopPlot->changeCulture(ePlayer,iChange,false);
								}
							}
						}
					}
				}
				else
				{
					changeCulture(ePlayer,iCulture,false);
				}
			}
		}
	}
}
// Super Forts end

// Super Forts begin *canal* *choke*
int CvPlot::countRegionPlots(const CvPlot* pInvalidPlot) const
{
	int iCount = 0;
	int iInvalidPlot = (pInvalidPlot == NULL) ? 0 : GC.getMap().plotNum(pInvalidPlot->getX(), pInvalidPlot->getY()) + 1;
	FAStar* pRegionFinder = gDLL->getFAStarIFace()->create();
	gDLL->getFAStarIFace()->Initialize(pRegionFinder, GC.getMap().getGridWidth(), GC.getMap().getGridHeight(), GC.getMap().isWrapX(), GC.getMap().isWrapY(), 
		NULL, NULL, NULL, stepValid, NULL, countPlotGroup, NULL);
	gDLL->getFAStarIFace()->SetData(pRegionFinder, &iCount);
	// Note to self: for GeneratePath() should bReuse be true or false?
	gDLL->getFAStarIFace()->GeneratePath(pRegionFinder, getX(), getY(), -1, -1, false, iInvalidPlot, false);
	gDLL->getFAStarIFace()->destroy(pRegionFinder);
	return iCount;
}

int CvPlot::countAdjacentPassableSections(bool bWater) const
{
	CvPlot* pAdjacentPlot;
	int iPassableSections = 0;
	bool bInPassableSection = false;

	// Are we looking for water passages or land passages?
	if(bWater)
	{
		bool bPlotIsWater = isWater();
		// This loop is for water
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
			if(pAdjacentPlot != NULL)
			{
				if(pAdjacentPlot->isWater())
				{
					// Don't count diagonal hops across land isthmus
					if (bPlotIsWater && !isCardinalDirection((DirectionTypes)iI))
					{
						if (!(GC.getMap().plot(getX(), pAdjacentPlot->getY())->isWater()) && !(GC.getMap().plot(pAdjacentPlot->getX(), getY())->isWater()))
						{
							continue;
						}
					}
					if(pAdjacentPlot->isImpassable())
					{
						if(isCardinalDirection((DirectionTypes)iI))
						{
							bInPassableSection = false;
						}
					}
					else if(!bInPassableSection)
					{
						bInPassableSection = true;
						++iPassableSections;
					}
				}
				else
				{
					bInPassableSection = false;
				}
			}
		}
	}
	else
	{
		// This loop is for land
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
			if(pAdjacentPlot != NULL)
			{
				if(pAdjacentPlot->isWater() || pAdjacentPlot->isImpassable())
				{	
					if(isCardinalDirection((DirectionTypes)iI))
					{
						bInPassableSection = false;
					}
				}
				else if(!bInPassableSection)
				{
					bInPassableSection = true;
					++iPassableSections;
				}
			}
		}
	}
	// Corner Case Correction
	pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_NORTH);
	if(pAdjacentPlot != NULL && (bWater == pAdjacentPlot->isWater()) && !pAdjacentPlot->isImpassable())
	{
		pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_NORTHWEST);
		if(pAdjacentPlot != NULL && (bWater == pAdjacentPlot->isWater()))
		{
			if(pAdjacentPlot->isImpassable())
			{
				pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_WEST);
				if(pAdjacentPlot != NULL && (bWater == pAdjacentPlot->isWater()) && !pAdjacentPlot->isImpassable())
				{
					--iPassableSections;
				}
			}
			else
			{
				--iPassableSections;
			}
		}
		else if(!bWater)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_WEST);
			if(pAdjacentPlot != NULL && !pAdjacentPlot->isWater() && !pAdjacentPlot->isImpassable())
			{
				--iPassableSections;
			}
		}
	}
	return iPassableSections;
}

int CvPlot::countImpassableCardinalDirections() const
{
	CvPlot* pAdjacentPlot;
	int iCount = 0;
	for(int iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotCardinalDirection(getX(), getY(), ((CardinalDirectionTypes)iI));
		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isImpassable() || (area() != pAdjacentPlot->area()))
			{
				++iCount;
			}
		}
	}
	return iCount;
}
// Super Forts end

// Super Forts begin *canal*
int CvPlot::getCanalValue() const																
{
	return m_iCanalValue;
}

void CvPlot::setCanalValue(int iNewValue)
{
	m_iCanalValue = iNewValue;
}

void CvPlot::calculateCanalValue()
{
	bool bInWaterSection;
	CvPlot *pAdjacentPlot, *apPlotsToCheck[4];
	int iWaterSections, iPlotsFound, iMaxDistance;
	int iCanalValue = 0;

	if(isCoastalLand() && !isImpassable())
	{
		iWaterSections = countAdjacentPassableSections(true);
		if(iWaterSections > 1)
		{
			iMaxDistance = 0;
			iPlotsFound = 0;
			bInWaterSection = false;
			// Find appropriate plots to be used for path distance calculations
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
				if(pAdjacentPlot != NULL)
				{
					if(pAdjacentPlot->isWater())
					{
						if(pAdjacentPlot->isImpassable())
						{
							if(isCardinalDirection((DirectionTypes)iI))
							{
								bInWaterSection = false;
							}
						}
						else if(!bInWaterSection)
						{
							bInWaterSection = true;
							apPlotsToCheck[iPlotsFound] = pAdjacentPlot;
							if((++iPlotsFound) == iWaterSections)
								break;
						}
					}
					else
					{
						bInWaterSection = false;
					}
				}
			}
			// Find the max path distance out of all possible pairs of plots
			for (int iI = 0; iI < (iPlotsFound - 1); ++iI)
			{
				for (int iJ = iI + 1; iJ < iPlotsFound; ++iJ)
				{
					if(!apPlotsToCheck[iI]->isLake() || !apPlotsToCheck[iJ]->isLake())
					{
						int iDistance = GC.getMap().calculatePathDistance(apPlotsToCheck[iI], apPlotsToCheck[iJ]);
						if(iDistance == -1)
						{
						
							// If no path was found then value is based off the number of plots in the region minus a minimum area
							iDistance = std::min(apPlotsToCheck[iI]->countRegionPlots(), apPlotsToCheck[iJ]->countRegionPlots()) - 7;
							iDistance *= 4;
						}
						else
						{
							// Path already would have required 2 steps, and I don't care that much about saving just 1 or 2 moves
							iDistance -= 4;
						}
						if(iDistance > iMaxDistance)
						{
							iMaxDistance = iDistance;
						}
					}
				}
			}
			iCanalValue = iMaxDistance * (iPlotsFound - 1);
		}
	}

	setCanalValue(iCanalValue);
}
// Super Forts end

// Super Forts begin *choke*
int CvPlot::getChokeValue() const
{
	return m_iChokeValue;
}

void CvPlot::setChokeValue(int iNewValue)
{
	m_iChokeValue = iNewValue;
}

void CvPlot::calculateChokeValue()
{
	bool bInPassableSection;
	CvPlot *pAdjacentPlot, *apPlotsToCheck[4];
	int iPassableSections, iPlotsFound, iMaxDistance;
	int iChokeValue = 0;
	bool bWater = isWater();

	if(!isImpassable() && countImpassableCardinalDirections() > 1)
	{
		iPassableSections = countAdjacentPassableSections(bWater);
		if(iPassableSections > 1)
		{
			iMaxDistance = 0;
			iPlotsFound = 0;
			bInPassableSection = false;
			// Find appropriate plots to be used for path distance calculations
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
				if(pAdjacentPlot != NULL)
				{
					if(pAdjacentPlot->isWater() == bWater)
					{	
						// Don't count diagonal hops across land isthmus
						if (bWater && !isCardinalDirection((DirectionTypes)iI))
						{
							if (!(GC.getMap().plot(getX(), pAdjacentPlot->getY())->isWater()) && !(GC.getMap().plot(pAdjacentPlot->getX(), getY())->isWater()))
							{
								continue;
							}
						}
						if(pAdjacentPlot->isImpassable())
						{
							if(isCardinalDirection((DirectionTypes)iI))
							{
								bInPassableSection = false;
							}
						}
						else if(!bInPassableSection)
						{
							bInPassableSection = true;
							apPlotsToCheck[iPlotsFound] = pAdjacentPlot;
							if((++iPlotsFound) == iPassableSections)
								break;
						}
					}
					else if(bWater || isCardinalDirection((DirectionTypes)iI))
					{
						bInPassableSection = false;
					}
				}
			}
			// Find the max path distance out of all possible pairs of plots
			for (int iI = 0; iI < (iPlotsFound - 1); ++iI)
			{
				for (int iJ = iI + 1; iJ < iPlotsFound; ++iJ)
				{
					int iDistance = GC.getMap().calculatePathDistance(apPlotsToCheck[iI], apPlotsToCheck[iJ], this);
					if(iDistance == -1)
					{
						// If no path was found then value is based off the number of plots in the region minus a minimum area
						iDistance = std::min(apPlotsToCheck[iI]->countRegionPlots(this), apPlotsToCheck[iJ]->countRegionPlots(this)) - 4;
						iDistance *= 4;
					}
					else
					{
						// Path already would have required 2 steps, but we forced the enemy to go another way so there is some value
						iDistance -= 1;
					}
					if(iDistance > iMaxDistance)
					{
						iMaxDistance = iDistance;
					}
				}
			}
			iChokeValue = iMaxDistance * (iPlotsFound - 1);
		}
	}

	setChokeValue(iChokeValue);
}
// Super Forts end

int CvPlot::defenseModifier(TeamTypes eDefender, bool bIgnoreBuilding,
	TeamTypes eAttacker, // advc.012
	bool bHelp, /* advc.500b: */ bool bGarrisonStrength) const
{
	int iModifier = GC.getInfo(getTerrainType()).getDefenseModifier();
	FeatureTypes eFeature = getFeatureType();
	if (eFeature != NO_FEATURE)
	{
		iModifier += GC.getInfo(eFeature).getDefenseModifier();
		// <advc.012>
		if(eAttacker == NO_TEAM  || getTeam() != eAttacker)
			iModifier += GC.getInfo(eFeature).getRivalDefenseModifier();
		// </advc.012>
	}
	if (isHills())
		iModifier += GC.getDefineINT(CvGlobals::HILLS_EXTRA_DEFENSE);
	ImprovementTypes eImprovement;
	if (bHelp)
		eImprovement = getRevealedImprovementType(getActiveTeam());
	else eImprovement = getImprovementType();
	// <advc.183> No city defense should apply then
	if (eDefender == NO_TEAM)
	{
		FAssert(!bGarrisonStrength); // advc.500b
		return iModifier;
	}
	/*	Note: The rest of this function (city defense) needs to be
		consistent with CvTeam::isCityDefense. */ // </advc.183>
	if (eImprovement != NO_IMPROVEMENT)
	{	// advc.183: OutsideBorders check added
		if ((!isOwned() && GC.getInfo(eImprovement).isOutsideBorders()) ||
			// advc.183: was isFriendlyTerritory
			GET_TEAM(eDefender).isAlliedTerritory(getTeam(), eAttacker))
		{
			// Super Forts begin *bombard*
			if (GC.getGame().isOption(GAMEOPTION_SUPER_FORTS))
			{
				iModifier += GC.getInfo(eImprovement).getDefenseModifier() - getDefenseDamage();
			}
			else
			{
				iModifier += GC.getInfo(eImprovement).getDefenseModifier();
			}
			// Super Forts end
		}
	}
//mountains mod
	if (GC.getGame().isOption(GAMEOPTION_MOUNTAINS))//AND Mountain Option
	{
		if (isPeak())
		{
			iModifier += GC.getDefineINT(CvGlobals::PEAK_EXTRA_DEFENSE);
		}
	}

	if (!bHelp)
	{
		CvCity* pCity = getPlotCity();
		if (pCity != NULL)
		{	// <advc.500b>
			if (bGarrisonStrength)
				iModifier += pCity->getBuildingDefense();
			else // </advc.500b>
			{	// advc.183: Open Borders shouldn't be enough
				if (GET_TEAM(eDefender).isAlliedTerritory(getTeam(), eAttacker))
					iModifier += pCity->getDefenseModifier(bIgnoreBuilding);
			}
		}
	}

	return iModifier;
}


int CvPlot::movementCost(CvUnit const& kUnit, CvPlot const& kFrom,
	bool bAssumeRevealed) const // advc.001i
{
	// <advc.162>
	if(kUnit.isInvasionMove(kFrom, *this))
		return kUnit.movesLeft(); // </advc.162>

	if (kUnit.flatMovementCost() /*||
		advc.opt: Now covered by the above
		//kUnit.getDomainType() == DOMAIN_AIR*/)
	{
		return GC.getMOVE_DENOMINATOR();
	}
	TeamTypes const eTeam = kUnit.getTeam(); // advc.opt
	/*if (kUnit.isHuman()) {
		if (!isRevealed(eTeam, false))
			return kUnit.maxMoves();
	}*/
	// K-Mod. Why let the AI cheat this?
	if ( /* advc.001i: The K-Mod condition is OK, but now that the pathfinder passes
			bAssumeRevealed=false, it's cleaner to check that too. */
		!bAssumeRevealed &&
		!isRevealed(eTeam))
	{
		/*if (!pFromPlot->isRevealed(eTeam, false))
			return kUnit.maxMoves();
		else return GC.getMOVE_DENOMINATOR() + 1;
		*/ // (further weight adjustments are now done in the pathfinder's moveCost function.)
		return GC.getMOVE_DENOMINATOR();
	} // K-Mod end

	//if (!kFromPlot.isValidDomainForLocation(kUnit))
	if (!kUnit.isValidDomain(kFrom)) // advc
		return kUnit.maxMoves();
	//if (!isValidDomainForAction(kUnit))
	if (!kUnit.isValidDomain(isWater())) // advc
		return GC.getMOVE_DENOMINATOR();

	FAssert(kUnit.getDomainType() != DOMAIN_IMMOBILE);

	int iRegularCost;
	if (kUnit.ignoreTerrainCost())
		iRegularCost = 1;
	else
	{
		iRegularCost = (!isFeature() ?
				GC.getInfo(getTerrainType()).getMovementCost() :
				GC.getInfo(getFeatureType()).getMovementCost());
		if (isHills())
			iRegularCost += GC.getDefineINT(CvGlobals::HILLS_EXTRA_MOVEMENT);
//===NM=====Mountains Mod===0=====
		if (GC.getGame().isOption(GAMEOPTION_MOUNTAINS))
		{
			if (isPeak())
				iRegularCost += GC.getDefineINT(CvGlobals::PEAK_EXTRA_MOVEMENT);
		}
//===NM=====Mountains Mod===X=====

		if (iRegularCost > 0)
			iRegularCost = std::max(1, iRegularCost - kUnit.getExtraMoveDiscount());
	}

	bool bHasTerrainCost = (iRegularCost > 1);
	int const iBaseMoves = kUnit.baseMoves(); // advc.opt
	iRegularCost = std::min(iRegularCost, iBaseMoves);
	iRegularCost *= GC.getMOVE_DENOMINATOR();
	if (bHasTerrainCost)
	{
		if ((!isFeature() ? kUnit.isTerrainDoubleMove(getTerrainType()) :
			kUnit.isFeatureDoubleMove(getFeatureType())) ||
//			(isHills() && kUnit.isHillsDoubleMove()))
//===NM=====Mountains Mod===0=====
			(((GC.getGame().isOption(GAMEOPTION_MOUNTAINS) && isPeak()) || isHills()) && kUnit.isHillsDoubleMove())) // Deliverator - Hijacked, Hills -> Peak + hills by keldath
//===NM=====Mountains Mod===0=====		
		{
			iRegularCost /= 2;
		}
	}
	int iRouteCost, iRouteFlatCost;
	// <advc.001i> Pass along bAssumeRevealed
	if (kFrom.isValidRoute(&kUnit, bAssumeRevealed) &&
		isValidRoute(&kUnit, bAssumeRevealed) && // </advc.001i>
		(GET_TEAM(eTeam).isBridgeBuilding() ||
		!kFrom.isRiverCrossing(directionXY(kFrom, *this))))
	{	// <advc.001i>
		RouteTypes eFromRoute = (bAssumeRevealed ? kFrom.getRouteType() :
				kFrom.getRevealedRouteType(eTeam));
		CvRouteInfo const& kFromRoute = GC.getInfo(eFromRoute);
		RouteTypes eToRoute = (bAssumeRevealed ? getRouteType() :
				getRevealedRouteType(eTeam));
		CvRouteInfo const& kToRoute = GC.getInfo(eToRoute);
		iRouteCost = std::max(
				kFromRoute.getMovementCost() +
				GET_TEAM(eTeam).getRouteChange(eFromRoute),
				kToRoute.getMovementCost() +
				GET_TEAM(eTeam).getRouteChange(eToRoute));
		// </advc.001i>
		iRouteFlatCost = std::max(
				kFromRoute.getFlatMovementCost() * iBaseMoves,
				kToRoute.getFlatMovementCost() * iBaseMoves);
	}
	else
	{
		iRouteCost = MAX_INT;
		iRouteFlatCost = MAX_INT;
	}

	return std::max(1, std::min(iRegularCost, std::min(iRouteCost, iRouteFlatCost)));
}


int CvPlot::getExtraMovePathCost() const
{
	return GC.getGame().getPlotExtraCost(getX(), getY());
}


void CvPlot::changeExtraMovePathCost(int iChange)
{
	GC.getGame().changePlotExtraCost(getX(), getY(), iChange);
}


bool CvPlot::isAdjacentOwned() const
{
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isOwned())
			return true;
	}
	return false;
}


bool CvPlot::isAdjacentPlayer(PlayerTypes ePlayer, bool bLandOnly) const
{
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->getOwner() == ePlayer)
		{
			if (!bLandOnly || !(pAdj->isWater()))
				return true;
		}
	}
	return false;
}


bool CvPlot::isAdjacentTeam(TeamTypes eTeam, bool bLandOnly) const
{
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->getTeam() == eTeam)
		{
			if (!bLandOnly || !pAdj->isWater())
				return true;
		}
	}
	return false;
}


bool CvPlot::isWithinCultureRange(PlayerTypes ePlayer) const
{
	FOR_EACH_ENUM(CultureLevel)
	{
		if (isCultureRangeCity(ePlayer, eLoopCultureLevel))
			return true;
	}
	/*	<advc.099c> Make sure that all city plots are considered to be
		within their owner's culture range */
	if (isCity() && getOwner() == ePlayer)
		return true; // </advc.099c>
	return false;
}


int CvPlot::getNumCultureRangeCities(PlayerTypes ePlayer) const
{
	int iCount = 0;
	FOR_EACH_ENUM(CultureLevel)
		iCount += getCultureRangeCities(ePlayer, eLoopCultureLevel);
	return iCount;
}

/*	K-Mod (rewrite of a bbai function)
	I've changed the purpose of this function - because this is the way it is always used. */
void CvPlot::invalidateBorderDangerCache()
{
	for (SquareIter itPlot(*this, CvPlayerAI::BORDER_DANGER_RANGE);
		itPlot.hasNext(); ++itPlot)
	{
		for (TeamIter<> itTeam; itTeam.hasNext(); ++itTeam)
		{
			itPlot->setBorderDangerCache(itTeam->getID(), false);
		}
	}
}


PlayerTypes CvPlot::calculateCulturalOwner(/* advc.099c: */ bool bIgnoreCultureRange,
	bool bOwnExclusiveRadius) const // advc.035
{
	PROFILE_FUNC();
	/*  advc.001: When a city is captured, the tiles in its culture range (but I
		think not the city plot itself) are set to unowned for 2 turns. This leads
		to 0% revolt chance for a turn or two when a city is razed and a new city
		is immediately founded near the ruins. Adding an isCity check to avoid confusion. */

// Super Forts begin *culture* - doto to advc adjustment
	bool superForts = isFortImprovement();//GC.getGame().isOption(GAMEOPTION_SUPER_FORTS);
	if ((!isCity()
		|| (!superForts))
		&& isForceUnowned())
		return NO_PLAYER;
// Super Forts begin *culture* - doto to advc adjustment

	// <advc.035>
	EagerEnumMap<PlayerTypes,bool> abCityRadius;
	bool bAnyCityRadius = false;
	if (bOwnExclusiveRadius)
	{
		for (NearbyCityIter itCity(*this); itCity.hasNext(); ++itCity)
		{
			if (itCity->isOccupation())
				continue;
			PlayerTypes eCityOwner = itCity->getOwner();
			// Super Forts begin *culture* - modified if statement
			if (isWithinCultureRange(eCityOwner) 
			|| (superForts && isWithinFortCultureRange(eCityOwner)))
			// Super Forts end
			{
				abCityRadius.set(eCityOwner, true);
				bAnyCityRadius = true;
			}
		}
	} // </advc.035>

	int iBestCulture = 0;
	PlayerTypes eBestPlayer = NO_PLAYER;
	for (PlayerIter<EVER_ALIVE> itPlayer; itPlayer.hasNext(); ++itPlayer)
	{
		PlayerTypes const ePlayer = itPlayer->getID();
		// <advc.035>
		if (bOwnExclusiveRadius && bAnyCityRadius && !abCityRadius.get(ePlayer))
			continue; // </advc.035>
		if (itPlayer->isAlive() /* advc.099c: */ || bIgnoreCultureRange)
		{
			int iCulture = getCulture(ePlayer);
			/*  <advc.035> When the range of a city expands, tile ownership is updated
				before tile culture is spread. If the expansion is sudden (WorldBuilder,
				perhaps also culture bomb), 0 tile culture in the new range is possible. */
			if (bOwnExclusiveRadius && iBestCulture == 0 && abCityRadius.get(ePlayer))
				iBestCulture = -1;
			if (iCulture <= iBestCulture) // </advc.035>
				continue;
			if (/* advc.099c: */ bIgnoreCultureRange ||
				isWithinCultureRange(ePlayer)||
// Super Forts begin *culture* - modified if statement
			 	(superForts && isWithinFortCultureRange(ePlayer)))
// Super Forts end	
			{
				if (iCulture > iBestCulture ||
					(iCulture == iBestCulture && getOwner() == ePlayer))
				{
					iBestCulture = iCulture;
					eBestPlayer = ePlayer;
				}
			}
		}
	}

	if (!isCity() && eBestPlayer != NO_PLAYER)
	{
		CvCity* pBestCity = NULL;
		int iBestPriority = MAX_INT;
		for (NearbyCityIter itCity(*this); itCity.hasNext(); ++itCity)
		{
			if (itCity->getTeam() != TEAMID(eBestPlayer) &&
				!GET_TEAM(eBestPlayer).isVassal(itCity->getTeam()))
			{
				continue;
			}
			if (getCulture(itCity->getOwner()) <= 0)
				continue;
			/*	advc.099c: 099c cares only about city plot culture, but for consistency,
				I'm also implementing the IgnoreCultureRange switch for non-city tiles. */
			if (!bIgnoreCultureRange &&
				!isWithinCultureRange(itCity->getOwner()))
			{
				continue;
			}
			int iPriority = itCity.cityPlotPriority();
			// give priority to master over vassal
			if (itCity->getTeam() == TEAMID(eBestPlayer))
			{	// advc: Was hardcoded as 5 w/ comment "priority ranges from 0 to 4"
				iPriority += GC.maxCityPlotPriority() + 1;
			}
			if (iPriority < iBestPriority ||
				(iPriority == iBestPriority && itCity->getOwner() == eBestPlayer))
			{
				iBestPriority = iPriority;
				pBestCity = &*itCity;
			}
		}

		if (pBestCity != NULL)
			eBestPlayer = pBestCity->getOwner();
	}
// < JCultureControl Mod Start >
	if (!isCity() && GC.getGame().isOption(GAMEOPTION_CULTURE_CONTROL))
	{
	    if (eBestPlayer == NO_PLAYER)
	    {
            if (findHighestCultureControlPlayer() != NO_PLAYER)
            {
                eBestPlayer = findHighestCultureControlPlayer();
            }
        }
	}
// < JCultureControl Mod End >
	if(eBestPlayer != NO_PLAYER)
		return eBestPlayer;
//doto change for superforts
//added bvalid for super forts and changed the loop from advc.
	int bValid = true;
//doto change for superforts	
	FOR_EACH_ORTH_ADJ_PLOT(*this)
	{
		if (pAdj->isOwned())
		{
			if (eBestPlayer == NO_PLAYER)
			{
				eBestPlayer = pAdj->getOwner();
			}
			else if (eBestPlayer != pAdj->getOwner())
			{
//doto change for superforts
				//return NO_PLAYER; //org from advc
				bValid = false;
				break;
			}
		}
		else //return NO_PLAYER; //org from advc
		{
//doto change for superforts
			bValid = false;
			break;	
		}
	}
//doto change for superforts
// Super Forts begin *culture*
	if (!bValid || 
		superForts && !GET_PLAYER(eBestPlayer).isAlive())
	{
			eBestPlayer = NO_PLAYER;
	}
// Super Forts end	

	return eBestPlayer;
}


void CvPlot::plotAction(PlotUnitFunc func, int iData1, int iData2,
	PlayerTypes eOwner, TeamTypes eTeam)
{
	FOR_EACH_UNIT_VAR_IN(pUnit, *this)
	{
		if (eOwner == NO_PLAYER || pUnit->getOwner() == eOwner)
		{
			if (eTeam == NO_TEAM || pUnit->getTeam() == eTeam)
				func(pUnit, iData1, iData2);
		}
	}
}


int CvPlot::plotCount(ConstPlotUnitFunc funcA, int iData1A, int iData2A,
	PlayerTypes eOwner, TeamTypes eTeam,
	ConstPlotUnitFunc funcB, int iData1B, int iData2B) const
{
	int iCount = 0;
	FOR_EACH_UNIT_IN(pUnit, *this)
	{
		if (eOwner == NO_PLAYER || pUnit->getOwner() == eOwner)
		{
			if (eTeam == NO_TEAM || pUnit->getTeam() == eTeam)
			{
				if (funcA == NULL || funcA(pUnit, iData1A, iData2A))
				{
					if (funcB == NULL || funcB(pUnit, iData1B, iData2B))
						iCount++;
				}
			}
		}
	}
	return iCount;
}


CvUnit* CvPlot::plotCheck(ConstPlotUnitFunc funcA, int iData1A, int iData2A,
	PlayerTypes eOwner, TeamTypes eTeam,
	ConstPlotUnitFunc funcB, int iData1B, int iData2B) const
{
	FOR_EACH_UNIT_VAR_IN(pUnit, *this)
	{
		if (eOwner == NO_PLAYER || pUnit->getOwner() == eOwner)
		{
			if (eTeam == NO_TEAM || pUnit->getTeam() == eTeam)
			{	// advc.045: Missing NULL check added. Bug? Tagging advc.001.
				if (funcA == NULL || funcA(pUnit, iData1A, iData2A))
				{
					if (funcB == NULL || funcB(pUnit, iData1B, iData2B))
						return pUnit;
				}
			}
		}
	}
	return NULL;
}


bool CvPlot::isRevealedBarbarian() const
{
	return (getRevealedOwner(getActiveTeam(), true) == BARBARIAN_PLAYER);
}


bool CvPlot::isVisible(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
		return true;

	if (eTeam == NO_TEAM)
		return false;

	return (getVisibilityCount(eTeam) > 0 || getStolenVisibilityCount(eTeam) > 0);
}

// <advc.300>
// Mostly cut from CvMap::syncRandPlot. Now ignores Barbarian units.
bool CvPlot::isCivUnitNearby(int iRadius) const
{
	if (iRadius < 0)
		return false;
	for (SquareIter it(*this, iRadius); it.hasNext(); ++it)
	{
		if (it->isUnit())
		{
			CvUnit* pAnyUnit = it->plotCheck(PUF_isVisible, BARBARIAN_PLAYER);
			if (pAnyUnit == NULL)
				continue;
			if (pAnyUnit->getOwner() != BARBARIAN_PLAYER)
				return true;
		}
	}
	return false;
}


CvPlot const* CvPlot::nearestInvisiblePlot(bool bOnlyLand, int iMaxPlotDist, TeamTypes eObserver) const
{
	if (!isVisible(eObserver))
		return this;
	CvPlot* r = NULL;
	CvMap const& m = GC.getMap();
	// Process plots in a spiral pattern (for performance reasons)
	for (int d = 1; d <= iMaxPlotDist; d++)
	{
		int iShortestDist = iMaxPlotDist + 1;
		for (int dx = -d; dx <= d; dx++)
		{
			for (int dy = -d; dy <= d; dy++)
			{
				// Don't process plots repeatedly:
				if (::abs(dx) < d && ::abs(dy) < d)
					continue;
				CvPlot* pPlot = m.plot(getX() + dx, getY() + dy);
				if (pPlot == NULL) continue; CvPlot const& p = *pPlot;
				if (p.isVisible(eObserver) || (bOnlyLand && p.isWater()) ||
					(p.isOwned() && p.getOwner() != BARBARIAN_PLAYER))
				{
					continue;
				}
				int iPlotDist = ::plotDistance(pPlot, this);
				if (iPlotDist < iShortestDist)
				{
					iShortestDist = iPlotDist;
					r = pPlot;
				}
			}
		}
		if(r != NULL)
			return r;
	}
	return NULL;
} // </advc.300>


bool CvPlot::isActiveVisible(bool bDebug) const
{	// <advc.706>
	if (m_bAllFog)
		return false; // </advc.706>
	return isVisible(getActiveTeam(), bDebug);
}


bool CvPlot::isVisibleToCivTeam() const
{
	for (TeamIter<CIV_ALIVE> itTeam; itTeam.hasNext(); ++itTeam)
	{
		if (isVisible(itTeam->getID()))
			return true;
	}
	return false;
}


bool CvPlot::isVisibleToWatchingHuman() const
{
	for (TeamIter<HUMAN> itTeam; itTeam.hasNext(); ++itTeam)
	{
		if (isVisible(itTeam->getID()))
			return true;
	}
	return false;
}


bool CvPlot::isAdjacentVisible(TeamTypes eTeam, bool bDebug) const
{
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isVisible(eTeam, bDebug))
			return true;
	}
	return false;
}


bool CvPlot::isAdjacentNonvisible(TeamTypes eTeam) const
{
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (!pAdj->isVisible(eTeam))
			return true;
	}
	return false;
}


bool CvPlot::isGoody(TeamTypes eTeam) const
{
	if (eTeam != NO_TEAM && GET_TEAM(eTeam).isBarbarian())
		return false;

	return (!isImproved() ? false : GC.getInfo(getImprovementType()).isGoody());
}


bool CvPlot::isRevealedGoody(TeamTypes eTeam) const
{
	if (eTeam == NO_TEAM)
		return isGoody();

	if (GET_TEAM(eTeam).isBarbarian())
		return false;

	return ((getRevealedImprovementType(eTeam) == NO_IMPROVEMENT) ? false :
		GC.getInfo(getRevealedImprovementType(eTeam)).isGoody());
}


void CvPlot::removeGoody()
{
	// <advc>
	if (!isGoody())
		return; // </advc>
	setImprovementType(NO_IMPROVEMENT);
	/*	<advc.001> This works around an issue w/ Debug mode: The EXE does (apparently)
		not update the layout of plots unrevealed to the active player (AP).
		So when a rival of the AP enters a goody hut previously revealed to the AP
		through Debug mode (but by then hidden again), the layout doesn't get updated,
		and when the AP (eventually) reveals the plot through exploration, the goody
		hut graphic briefly appears before the layout gets updated. */
	if (!isRevealed(getActiveTeam(), true) && GC.IsGraphicsInitialized() &&
		m_pPlotBuilder != NULL)
	{
		/*	The only way I've found to get rid of the goody hut graphic.
			(Doing this whenever any plot becomes unrevealed might slow down repeated
			toggling of Debug mode. Let's only deal with the annoying hut issue.) */
		gDLL->getPlotBuilderIFace()->destroy(m_pPlotBuilder);
	} // </advc.001>
}
//super forts - doto keldath and blocade mod
//considered using this , since advc changed the org fn.
//in the end i added in the isfort below. easier for me to use
//though i could use the citydefense or revealolit something that f1rpo created
// advc: Deprecated; see comment in header.
bool CvPlot::isCityExternal(bool bCheckImprovement, TeamTypes eForTeam) const
{
	FAssertMsg(!bCheckImprovement && eForTeam == NO_TEAM, "Does the EXE ever use those params?");
	if (!bCheckImprovement)
		return isCity();
	if (!isImproved() || !GC.getInfo(getImprovementType()).isActsAsCity())
		return false;
	if (eForTeam == NULL)
		return true;
	return GET_TEAM(eForTeam).isBase(*this);
}
//super forts - doto keldath
bool CvPlot::isFortImprovement() const
{
	if (!GC.getGame().isOption(GAMEOPTION_SUPER_FORTS))
		return false;
	if (!isOwned())
		return false;
	if (!isImproved())
		return false;
	if (!GC.getInfo(getImprovementType()).isActsAsCity())
		return false;
	return true;
}

bool CvPlot::isOccupation() const
{
	CvCity* pCity = getPlotCity();
	if (pCity != NULL)
		return pCity->isOccupation();
	return false;
}


bool CvPlot::isBeingWorked() const
{
	CvCity* pWorkingCity = getWorkingCity();
	if (pWorkingCity != NULL)
		return pWorkingCity->isWorkingPlot(*this);
	return false;
}


bool CvPlot::isVisibleEnemyDefender(const CvUnit* pUnit) const
{
	return (plotCheck(PUF_canDefendEnemy, pUnit->getOwner(), pUnit->isAlwaysHostile(*this),
			NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner()) != NULL);
}


int CvPlot::getNumVisibleEnemyDefenders(const CvUnit* pUnit) const
{
	return plotCount(PUF_canDefendEnemy, pUnit->getOwner(), pUnit->isAlwaysHostile(*this),
			NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner());
}

// advc.ctr:
bool CvPlot::isVisibleEnemyCityAttacker(PlayerTypes eDefender, TeamTypes eAssumePeace,
	int iRange) const
{
	//PROFILE_FUNC(); // (rarely called so far)
	for (SquareIter it(*this, iRange, true); it.hasNext(); ++it)
	{
		if (it->plotCheck(PUF_isEnemyCityAttacker, eDefender, eAssumePeace,
			NO_PLAYER, NO_TEAM, PUF_isVisible, eDefender) != NULL)
		{
			return true;
		}
	}
	return false;
}

/*	advc (note): Not used internally. Some of the getPlotList... functions
	(available to the DLL through CvDLLInterfaceIFaceBase) call it, and it seems
	that the EXE also exposes it to Python. */
int CvPlot::getNumVisibleUnits(PlayerTypes ePlayer) const
{
	return plotCount(PUF_isVisibleDebug, ePlayer);
}


bool CvPlot::isVisibleEnemyUnit(const CvUnit* pUnit) const
{
	return (plotCheck(PUF_isEnemy, pUnit->getOwner(), pUnit->isAlwaysHostile(*this),
			NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner()) != NULL);
}

// advc.004l: Same checks as above, just doesn't loop through all units.
bool CvPlot::isVisibleEnemyUnit(CvUnit const* pUnit, CvUnit const* pPotentialEnemy) const
{
	return (PUF_isEnemy(pPotentialEnemy, pUnit->getOwner(), pUnit->isAlwaysHostile(*this)) &&
			!pPotentialEnemy->isInvisible(pUnit->getTeam(), false));
}


bool CvPlot::canHaveFeature(FeatureTypes eFeature,
	bool bIgnoreCurrentFeature) const // advc.055
{
	if (eFeature == NO_FEATURE)
		return true;

	if (isFeature() /* advc.055: */ && !bIgnoreCurrentFeature)
		return false;

	if (isPeak())
		return false;

	if (isCity())
		return false;

	CvFeatureInfo const& kFeature = GC.getInfo(eFeature);
	if (!kFeature.isTerrain(getTerrainType()))
		return false;

	if (kFeature.isNoCoast() && isCoastalLand())
		return false;

	if (kFeature.isNoRiver() && isRiver())
		return false;
	// <advc.129b>
	if (kFeature.isNoRiverSide() && isRiverSide())
		return false; // </advc.129b>

	if (kFeature.isRequiresFlatlands() && isHills())
		return false;

	if (kFeature.isNoAdjacent())
	{
		FOR_EACH_ADJ_PLOT(*this)
		{
			if (pAdj->getFeatureType() == eFeature)
				return false;
		}
	}

	if (kFeature.isRequiresRiver() && !isRiver())
		return false;
	// <advc.129b>
	if(kFeature.isRequiresRiverSide() && !isRiverSide())
		return false; // </advc.129b>

	return true;
}

// advc: rewritten
bool CvPlot::isValidRoute(CvUnit const* pUnit,
	/* <advc.001i> */ bool bAssumeRevealed) const
{
	//if (!isRoute()) return false;
	CvTeam const& kTeam = GET_TEAM(pUnit->getOwner());
	RouteTypes const eRoute = (bAssumeRevealed ? getRouteType() :
			getRevealedRouteType(kTeam.getID()));
	if (eRoute == NO_ROUTE) // </advc.001i>
		return false;
	if (!isOwned() || pUnit->isEnemyRoute())
		return true;
	return (!pUnit->isEnemy(getTeam(), *this) &&
		!kTeam.isDisengage(getTeam())); // advc.034
}


bool CvPlot::isRiverNetwork(TeamTypes eTeam) const
{
	if (!isRiver())
		return false;

	if (GET_TEAM(eTeam).isRiverTrade())
		return true;

	/*  advc.124 (comment): This is what allows trade along owned river tiles
		without any prereq tech. */
	if (getTeam() == eTeam)
		return true;

	return false;
}

bool CvPlot::isNetworkTerrain(TeamTypes eTeam) const
{
	if (GET_TEAM(eTeam).isTerrainTrade(getTerrainType()))
		return true;

	if (isWater() && getTeam() == eTeam)
		return true;

	return false;
}


bool CvPlot::isBonusNetwork(TeamTypes eTeam) const
{
	if (isRoute() /* advc.124: */ && getRevealedRouteType(eTeam) != NO_ROUTE)
		return true;

	if (isRiverNetwork(eTeam))
		return true;

	if (isNetworkTerrain(eTeam))
		return true;

	return false;
}


bool CvPlot::isTradeNetwork(TeamTypes eTeam) const
{
	//PROFILE_FUNC(); // advc.003o

	//if (!isOwned())
	if (!isRevealed(eTeam)) // advc.124
		return false;

	if (isOwned() && GET_TEAM(eTeam).isAtWar(getTeam()) &&
		/*  advc.124: War blocks trade, but blockades against the plot owner
			override this. If these blockades also affect eTeam, trade is again
			blocked (by the next conditional). */
		getBlockadedCount(getTeam()) <= 0)
	{
		return false;
	}
	if (getBlockadedCount(eTeam) > 0)
		return false;

	if (isTradeNetworkImpassable(eTeam))
		return false;
	//return isBonusNetwork(eTeam);
	/*	<advc.124> (bugfix?): A friendly Fort shouldn't require a route to connect with
		water tiles. (To connect a resource on the Fort tile, a route is still needed.) */
	if (isBonusNetwork(eTeam))
		return true;
	// The first check (inlined) is only done to save time
	return (isImproved() && GET_TEAM(eTeam).isRevealedCityTrade(*this)); // </advc.124>
}


bool CvPlot::isTradeNetworkConnected(CvPlot const& kOther, TeamTypes eTeam) const
{
	//PROFILE_FUNC(); // advc.003o

	if ((getTeam() != NO_TEAM && GET_TEAM(eTeam).isAtWar(getTeam())
		// advc.124:
		&& getBlockadedCount(getTeam()) <= getBlockadedCount(eTeam))
		||
		(kOther.getTeam() != NO_TEAM && GET_TEAM(eTeam).isAtWar(kOther.getTeam())
		// advc.124:
		&& kOther.getBlockadedCount(kOther.getTeam()) <= kOther.getBlockadedCount(eTeam)))
	{
		return false;
	}
	if (isTradeNetworkImpassable(eTeam) || kOther.isTradeNetworkImpassable(eTeam))
		return false;

	//if (!isOwned()) { // advc.124 (commented out)
	if (!isRevealed(eTeam) || !kOther.isRevealed(eTeam))
		return false;
	// <advc.opt> Moved up
	bool const bNetworkTerrain = isNetworkTerrain(eTeam);
	bool const bOtherNetworkTerrain = kOther.isNetworkTerrain(eTeam);
	if (bNetworkTerrain && bOtherNetworkTerrain)
		return true; // </advc.opt>

	if (isRoute() /* advc.124: */ && getRevealedRouteType(eTeam) != NO_ROUTE)
	{
		if (kOther.isRoute() &&
			kOther.getRevealedRouteType(eTeam) != NO_ROUTE) // advc.124
		{
			return true;
		}
	}

	if (bOtherNetworkTerrain && //isCity(true, eTeam)
		GET_TEAM(eTeam).isRevealedCityTrade(*this)) // advc.124
	{
		return true;
	}
	if (bNetworkTerrain)
	{
		//if (kOther.isCity(true, eTeam))
		if (GET_TEAM(eTeam).isRevealedCityTrade(kOther)) // advc.124
			return true;

		if (kOther.isRiverNetwork(eTeam))
		{
			/*	advc (comment): Could argue that the river direction should matter here.
				If a river flows away from a water plot, then, at least graphically,
				they're not quite connected. (Same goes for the isRiverNetwork branch below.) */
			if (kOther.isRiverConnection(directionXY(kOther, *this)))
				return true;
		}
		/*	<advc.124> Case 1: kOther has a route and this plot has network terrain.
			(Note: The isCityRadius check is just for performance.) */
		if (kOther.isRoute() && kOther.getTeam() == eTeam &&
			kOther.isCityRadius())
		{
			CvCity const* pWorkingCity = kOther.getWorkingCity();
			if (pWorkingCity != NULL &&
				pWorkingCity->getOwner() == kOther.getOwner() &&
				!pWorkingCity->isArea(kOther.getArea()))
			{
				return true;
			}
		} // </advc.124>
	}

	if (isRiverNetwork(eTeam))
	{
		if (bOtherNetworkTerrain)
		{
			if (isRiverConnection(directionXY(*this, kOther)))
				return true;
		}

		if (isRiverConnection(directionXY(*this, kOther)) ||
			kOther.isRiverConnection(directionXY(kOther, *this)))
		{
			if (kOther.isRiverNetwork(eTeam))
				return true;
		}
	}

	// <advc.124> Case 2: This plot has a route and kOther has network terrain
	if (isRoute() && isCityRadius() && getTeam() == eTeam && bOtherNetworkTerrain)
	{
		CvCity const* pWorkingCity = getWorkingCity();
		if (pWorkingCity != NULL &&
			pWorkingCity->getOwner() == getOwner() &&
			!pWorkingCity->isArea(getArea()))
		{
			return true;
		}
	} // </advc.124>

	return false;
}

// advc.opt:
void CvPlot::updateImpassable()
{
//===NM=====Mountains Mod===X=====
	m_bImpassable = ((isPeak() && !GC.getGame().isOption(GAMEOPTION_MOUNTAINS))  || 
			(!isFeature() ?
			(getTerrainType() != NO_TERRAIN && GC.getInfo(getTerrainType()).isImpassable()) :
			GC.getInfo(getFeatureType()).isImpassable()));
}

// advc.opt:
void CvPlot::updateAnyIsthmus()
{
	if (!isWater()) // Only store isthmus info at water plots (don't need it for land)
	{
		m_bAnyIsthmus = false;
		return;
	}
	CvMap const& kMap = GC.getMap();
	FOR_EACH_ENUM(CardinalDirection)
	{
		/*	Look for a pair of orthogonally adjacent land plots that
			are diagonally adjacent to each other */
		CvPlot const* pOrthAdj1 = plotCardinalDirection(getX(), getY(),
				eLoopCardinalDirection);
		if (pOrthAdj1 == NULL || pOrthAdj1->isWater())
			continue;
		CvPlot const* pOrthAdj2 = plotCardinalDirection(getX(), getY(),
				(CardinalDirectionTypes)
				((eLoopCardinalDirection + 1) % NUM_CARDINALDIRECTION_TYPES));
		if (pOrthAdj2 == NULL || pOrthAdj2->isWater())
			continue;
		// Check for a water plot adjacent to all three plots
		int iDiagX, iDiagY;
		if (pOrthAdj1->getX() == getX())
		{
			iDiagX = pOrthAdj2->getX();
			iDiagY = pOrthAdj1->getY();
		}
		else
		{
			iDiagX = pOrthAdj1->getX();
			iDiagY = pOrthAdj2->getY();
		}
		if (kMap.getPlot(iDiagX, iDiagY).isWater())
		{
			m_bAnyIsthmus = true;
			return;
		}
	}
	m_bAnyIsthmus = false;
}
/*
bool CvPlot::isImpassable() const
{
	//PROFILE_FUNC(); // advc.003o: Most of the calls come from pathfinding
//===NM=====Mountains Mod===X=====
	if (!GC.getGame().isOption(GAMEOPTION_MOUNTAINS))//AND Mountains Option
	{	
		if (isPeak())
		{
			return true;
		}
	}
//===NM=====Mountains Mod===X=====
	return (!isFeature() ?
			// advc.opt: Check NO_TERRAIN only if NO_FEATURE
			(getTerrainType() != NO_TERRAIN && GC.getInfo(getTerrainType()).isImpassable()) :
			GC.getInfo(getFeatureType()).isImpassable());
}
*/

// advc.tsl:
void CvPlot::setLatitude(int iLatitude)
{
	if (iLatitude < 0 || iLatitude > 90)
	{
		FErrorMsg("Latitude should be in the interval [0,90]");
		iLatitude = range(iLatitude, 0, 90);
	}
	m_iLatitude = safeIntCast<char>(iLatitude);
}

// advc.tsl:
void CvPlot::updateLatitude()
{
	m_iLatitude = calculateLatitude();
}


int CvPlot::getLatitude() const
{
	return m_iLatitude; // advc.tsl
}

// advc.tsl: was getLatitude()
char CvPlot::calculateLatitude() const
{
	CvMap const& kMap = GC.getMap();
	scaled rLatitude;
	int iCoord = getY();
	int iDim = kMap.getGridHeight();
	if (!kMap.isWrapX() && kMap.isWrapY()) // Tilted axis
	{
		iCoord = getX();
		iDim = kMap.getGridWidth();
	}
	int const iTop = kMap.getTopLatitude();
	int const iBottom = kMap.getBottomLatitude();
	/*	UNOFFICIAL_PATCH (UP), Bugfix, 07/12/09, Temudjin & jdog5000:
		Subtract 1 from dimensions */
	/*rLatitude = scaled(iCoord, iDim - 1);
	rLatitude *= iTop - iBottom;
	rLatitude += iBottom;*/
	/*	<advc.129> Let top and bottom latitude refer to the _edges_ of the map.
		I.e. the topmost and bottommost _row_ will receive smaller (absolute)
		latitude values. */
	rLatitude = iBottom + scaled(iTop - iBottom, iDim) *
			(iCoord + fixp(0.5)); // </advc.129>
	/*	<advc.tsl> Compress southern hemisphere so that the equator is less
		central and fewer civs get placed there? Doesn't seem to help much.
		Nor does shaving off the south pole (by decreasing m_iBottomLatitude
		in CvMap::reset). */
	/*if (GC.getGame().isOption(GAMEOPTION_TRUE_STARTS) && iTop == -iBottom &&
		iTop > 0 && MapRandSuccess(fixp(2/3.)))
	{
		scaled rEquator = iDim * fixp(0.45);
		rLatitude = iBottom +
				(scaled::min(iCoord, rEquator - 1) + fixp(0.5))
				* -iBottom / rEquator +
				scaled::max(0, iCoord - (rEquator - 1) + fixp(0.5))
				* iTop / (iDim - rEquator);
	}*/ // </advc.tsl>
	rLatitude = rLatitude.abs();
	rLatitude.decreaseTo(90);
	return safeIntCast<char>(rLatitude.round());
}


int CvPlot::getFOWIndex() const
{
	CvMap const& kMap = GC.getMap();
	return (((kMap.getGridHeight() - 1) - getY()) *
			kMap.getGridWidth() * LANDSCAPE_FOW_RESOLUTION * LANDSCAPE_FOW_RESOLUTION) +
			(getX() * LANDSCAPE_FOW_RESOLUTION);
}

// advc: Let's us know that the CvArea objects have been initialized
void CvPlot::initArea()
{
	m_pArea = GC.getMap().getArea(m_iArea);
	FAssert(m_pArea != NULL);
}


CvArea* CvPlot::waterArea(
	bool bNoImpassable) const // BETTER_BTS_AI_MOD, General AI, 01/02/09, jdog5000
{
	if (isWater())
		return area();

	int iBestValue = 0;
	CvArea* pBestArea = NULL;
	FOR_EACH_ADJ_PLOT(*this)
	{
		// advc.030b: Can be NULL while recalculating areas at game start
		if (pAdj->area() != NULL &&
			pAdj->isWater() &&
			// BETTER_BTS_AI_MOD, General AI, 01/02/09, jdog5000
			(!bNoImpassable || !pAdj->isImpassable()))
		{
			int iValue = pAdj->getArea().getNumTiles();
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestArea = pAdj->area();
			}
		}
	}
	return pBestArea;
}

CvArea* CvPlot::secondWaterArea() const
{
	FAssert(!isWater());

	CvArea* pWaterArea = waterArea();
	int iBestValue = 0;
	CvArea* pBestArea = NULL;
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isWater() &&
			/*  advc.031: Same as in waterArea, except that I see no need for a bNoImpassable
				parameter here - water areas blocked by ice should always be excluded. */
			!pAdj->isImpassable() &&
			!pAdj->isArea(*pWaterArea))
		{
			int iValue = pAdj->getArea().getNumTiles();
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestArea = pAdj->area();
			}
		}
	}
	return pBestArea;
}
/* this entire part replaces the above if you want no fresh water from lakes and oceans */	
/* dune wars	- unmark this for no fresh water on lakes andocean //
//	FAssert(!isWater());

	iBestValue = 0;
	pBestArea = NULL;

// i dont know what this does so i left it out - keldath
// dune wars - start
	if (!isWater()) 
	{	
// dune wars - 	
		for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				if (pAdjacentPlot->isWater() && (pAdjacentPlot->getArea() != pWaterArea->getID()))
				{
					iValue = pAdjacentPlot->area()->getNumTiles();

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestArea = pAdjacentPlot->area();		
					}
				}
			}
		}
// dune wars - 		
	}
// dune wars - end	*/
//	return pBestArea;	

// advc: Works quite differently now that CvPlot::m_iArea is gone
void CvPlot::setArea(CvArea* pArea, /* advc.310: */ bool bProcess)
{
	if (area() == pArea)
		return;
	// <advc.310>
	if (!bProcess)
	{
		m_pArea = pArea;
		return;
	} // </advc.310>
	if (area() != NULL)
		processArea(getArea(), -1);
	m_pArea = pArea;
	// advc: Update cached CvArea pointers (even if pArea==NULL)
	if (isCity())
		getPlotCity()->updateArea();
	FOR_EACH_UNIT_VAR_IN(pUnit, *this)
	{
		pUnit->updateArea();
	}
	if (area() != NULL)
	{
		processArea(getArea(), 1);
		updateIrrigated();
		updateYield();
	}
}


int CvPlot::getFeatureVariety() const
{
	FAssert(!isFeature() ||
			m_iFeatureVariety < GC.getInfo(getFeatureType()).getArtInfo()->getNumVarieties());
	FAssert(m_iFeatureVariety >= 0);
	return m_iFeatureVariety;
}


bool CvPlot::isOwnershipScore() const
{
	static int const iOWNERSHIP_SCORE_DURATION_THRESHOLD = GC.getDefineINT("OWNERSHIP_SCORE_DURATION_THRESHOLD"); // advc.opt
	return (getOwnershipDuration() >= iOWNERSHIP_SCORE_DURATION_THRESHOLD);
}


void CvPlot::setOwnershipDuration(int iNewValue)
{
	if(getOwnershipDuration() == iNewValue)
		return;

	bool bOldOwnershipScore = isOwnershipScore();

	m_iOwnershipDuration = safeIntCast<short>(iNewValue);
	FAssert(getOwnershipDuration() >= 0);

	if (bOldOwnershipScore != isOwnershipScore() &&
		isOwned() && !isWater())
	{
		GET_PLAYER(getOwner()).changeTotalLandScored(isOwnershipScore() ? 1 : -1);
	}
}


void CvPlot::changeOwnershipDuration(int iChange)
{
	setOwnershipDuration(getOwnershipDuration() + iChange);
}


void CvPlot::setImprovementDuration(int iNewValue)
{
	m_iImprovementDuration = safeIntCast<short>(iNewValue);
	FAssert(getImprovementDuration() >= 0);
}


void CvPlot::changeImprovementDuration(int iChange)
{
	setImprovementDuration(getImprovementDuration() + iChange);
}

// advc.912f: Now returns negative value when player's upgrade rate is 0
int CvPlot::getUpgradeTimeLeft(ImprovementTypes eImprovement, PlayerTypes ePlayer) const
{
	int iUpgradeLeft = GC.getGame().getImprovementUpgradeTime(eImprovement)
			* 100 // advc.912f
			- (getImprovementType() == eImprovement ? getUpgradeProgress() : 0);
	// <advc.912f>
	int iUpgradeRate = (ePlayer == NO_PLAYER ? 100 :
			GET_PLAYER(ePlayer).getImprovementUpgradeRate());
	int iTurnsLeft = scaled(iUpgradeLeft, iUpgradeRate > 0 ? iUpgradeRate : -100).ceil();
	return (iTurnsLeft == 0 ? 1 : iTurnsLeft);
	// </advc.912f>
}


void CvPlot::setUpgradeProgress(int iNewValue)
{
	m_iUpgradeProgress = iNewValue;
	FAssert(getUpgradeProgress() >= 0);
}


void CvPlot::changeUpgradeProgress(int iChange)
{
	setUpgradeProgress(getUpgradeProgress() + iChange);
}


bool CvPlot::isForceUnowned() const
{
	return (getForceUnownedTimer() > 0);
}


void CvPlot::setForceUnownedTimer(int iNewValue)
{
	m_iForceUnownedTimer = safeIntCast<short>(iNewValue);
	FAssert(getForceUnownedTimer() >= 0);
}


void CvPlot::changeForceUnownedTimer(int iChange)
{
	setForceUnownedTimer(getForceUnownedTimer() + iChange);
}


void CvPlot::changeCityRadiusCount(int iChange)
{
	m_iCityRadiusCount += iChange;
	FAssert(getCityRadiusCount() >= 0);
}


bool CvPlot::isStartingPlot() const
{
	return m_bStartingPlot;
}


void CvPlot::setStartingPlot(bool bNewValue)
{
	m_bStartingPlot = bNewValue;
}


bool CvPlot::isNOfRiver() const
{
	return m_bNOfRiver;
}


void CvPlot::setNOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir)
{
	if (isNOfRiver() == bNewValue && eRiverDir == m_eRiverWEDirection)
		return; // advc

	if (isNOfRiver() != bNewValue)
	{
		updatePlotGroupBonus(false, /* advc.064d: */ false);
		m_bNOfRiver = bNewValue;
		updatePlotGroupBonus(true);

		updateRiverCrossing();
		updateYield();

		FOR_EACH_ADJ_PLOT_VAR(*this)
		{
			pAdj->updateRiverCrossing();
			pAdj->updateYield();
		}
		getArea().changeNumRiverEdges((isNOfRiver()) ? 1 : -1);
	}
	FAssertMsg(eRiverDir == CARDINALDIRECTION_WEST || eRiverDir == CARDINALDIRECTION_EAST || eRiverDir == NO_CARDINALDIRECTION ||
			/*  <advc.006> The Earth scenarios have one erratic river segment at
				the (western) Canopic branch of the Nile Delta. Not a problem,
				it just looks odd. I'm excluding that segment from the assertion. */
			(getX() == 68 && getY() == 39 &&
			eRiverDir == CARDINALDIRECTION_NORTH), // </advc.006>
			"invalid parameter");
	m_eRiverWEDirection = eRiverDir;

	updateRiverSymbol(true, true);
}


bool CvPlot::isWOfRiver() const
{
	return m_bWOfRiver;
}


void CvPlot::setWOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir)
{
	if (isWOfRiver() == bNewValue && eRiverDir == m_eRiverNSDirection)
		return; // advc

	if (isWOfRiver() != bNewValue)
	{
		updatePlotGroupBonus(false, /* advc.064d: */ false);
		m_bWOfRiver = bNewValue;
		updatePlotGroupBonus(true);

		updateRiverCrossing();
		updateYield();

		FOR_EACH_ADJ_PLOT_VAR(*this)
		{
			pAdj->updateRiverCrossing();
			pAdj->updateYield();
		}
		getArea().changeNumRiverEdges(isWOfRiver() ? 1 : -1);
	}

	FAssertMsg(eRiverDir == CARDINALDIRECTION_NORTH || eRiverDir == CARDINALDIRECTION_SOUTH || eRiverDir == NO_CARDINALDIRECTION, "invalid parameter");
	m_eRiverNSDirection = eRiverDir;

	updateRiverSymbol(true, true);
}


CardinalDirectionTypes CvPlot::getRiverNSDirection() const
{
	return (CardinalDirectionTypes)m_eRiverNSDirection;
}


CardinalDirectionTypes CvPlot::getRiverWEDirection() const
{
	return (CardinalDirectionTypes)m_eRiverWEDirection;
}


// This function finds an *inland* corner of this plot at which to place a river.
// It then returns the plot with that corner at its SE.

CvPlot* CvPlot::getInlandCorner() const
{
	CvPlot* pRiverPlot = NULL; // Plot through whose SE corner we want the river to run
	int aiShuffle[4];
	mapRand().shuffle(aiShuffle, ARRAYSIZE(aiShuffle));
	CvMap const& kMap = GC.getMap();
	for (int i = 0; i < ARRAYSIZE(aiShuffle); i++)
	{
		switch (aiShuffle[i])
		{
		case 0:
			pRiverPlot = kMap.plotSoren(getX(), getY()); break;
		case 1:
			pRiverPlot = kMap.plotDirection(getX(), getY(), DIRECTION_NORTH); break;
		case 2:
			pRiverPlot = kMap.plotDirection(getX(), getY(), DIRECTION_NORTHWEST); break;
		case 3:
			pRiverPlot = kMap.plotDirection(getX(), getY(), DIRECTION_WEST); break;
		}
		if (pRiverPlot != NULL && !pRiverPlot->hasCoastAtSECorner())
			break;
		pRiverPlot = NULL;
	}
	return pRiverPlot;
}


bool CvPlot::hasCoastAtSECorner() const
{
	if (isWater())
		return true;

	CvPlot* pAdj = plotDirection(getX(), getY(), DIRECTION_EAST);
	if (pAdj != NULL && pAdj->isWater())
		return true;

	pAdj = plotDirection(getX(), getY(), DIRECTION_SOUTHEAST);
	if (pAdj != NULL && pAdj->isWater())
		return true;

	pAdj = plotDirection(getX(), getY(), DIRECTION_SOUTH);
	if (pAdj != NULL && pAdj->isWater())
		return true;

	return false;
}


void CvPlot::setIrrigated(bool bNewValue)
{
	if(isIrrigated() == bNewValue)
		return;
	m_bIrrigated = bNewValue;
	FOR_EACH_ADJ_PLOT_VAR(*this)
	{
		pAdj->updateYield();
		pAdj->setLayoutDirty(true);
	}
}


void CvPlot::updateIrrigated()
{
	//if (area() == NULL) return;
	FAssert(area() != NULL); // advc
	// advc.pf: Rather handle the pathfinding in CvMap
	GC.getMap().updateIrrigated(*this);
}


bool CvPlot::isPotentialCityWorkForArea(CvArea const& kArea) const
{
	PROFILE_FUNC();

	static bool const bWATER_POTENTIAL_CITY_WORK_FOR_AREA = GC.getDefineBOOL("WATER_POTENTIAL_CITY_WORK_FOR_AREA"); // advc.opt

	for (CityPlotIter it(*this); it.hasNext(); ++it)
	{
		if (it->isArea(kArea) && // advc.opt: rearranged
			(!it->isWater() || bWATER_POTENTIAL_CITY_WORK_FOR_AREA))
		{
			return true;
		}
	}
	return false;
}


void CvPlot::updatePotentialCityWork()
{
	PROFILE_FUNC();

	bool bValid = false;
	for (CityPlotIter it(*this);
		!bValid && it.hasNext(); ++it)
	{
		//if (!it->isWater())
		if (it->canEverFound()) // advc.129d
			bValid = true;
	}

	if (isPotentialCityWork() != bValid)
	{
		m_bPotentialCityWork = bValid;
		updateYield();
	}
}


bool CvPlot::isShowCitySymbols() const
{
	return m_bShowCitySymbols;
}


void CvPlot::updateShowCitySymbols()
{
	bool bNewShowCitySymbols = false;
	for (NearbyCityIter itCity(*this); itCity.hasNext(); ++itCity)
	{
		if (itCity->isCitySelected() && gDLL->UI().isCityScreenUp())
		{
			if (itCity->canWork(*this))
			{
				bNewShowCitySymbols = true;
				break;
			}
		}
	}
	if (isShowCitySymbols() != bNewShowCitySymbols)
	{
		m_bShowCitySymbols = bNewShowCitySymbols;
		updateSymbolDisplay();
		updateSymbolVisibility();
	}
}


bool CvPlot::isFlagDirty() const
{
	return m_bFlagDirty;
}


void CvPlot::setFlagDirty(bool bNewValue)
{
	m_bFlagDirty = bNewValue;
}


void CvPlot::setOwner(PlayerTypes eNewValue, bool bCheckUnits, bool bUpdatePlotGroup)
{
	PROFILE_FUNC();

	if(getOwner() == eNewValue)
		return;
	PlayerTypes eOldOwner = getOwner(); // advc.ctr
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_PLOT_OWNER_CHANGE, eNewValue, (char*)NULL, getX(), getY());

	CvCity* pOldCity = getPlotCity();
	if (pOldCity != NULL)  // advc: Removed some assertions and NULL/NO_... checks in this block
	{
		/*  advc.101: Include pre-revolt owner in messages (sometimes not easy
			to tell once the city has flipped, and in replays). */
		wchar const* szOldOwnerDescr = GET_PLAYER(pOldCity->getOwner()).
				getCivilizationDescriptionKey();
		CvWString szBuffer(gDLL->getText("TXT_KEY_MISC_CITY_REVOLTED_JOINED",
				pOldCity->getNameKey(), GET_PLAYER(eNewValue).getCivilizationDescriptionKey(),
				szOldOwnerDescr)); // advc.101
		gDLL->UI().addMessage(getOwner(), false, -1, szBuffer, *this,
				"AS2D_CULTUREFLIP", MESSAGE_TYPE_MAJOR_EVENT, 
				ARTFILEMGR.getInterfaceArtPath("WORLDBUILDER_CITY_EDIT"),
				GC.getColorType("RED"));
		gDLL->UI().addMessage(eNewValue, false, -1, szBuffer, *this,
				"AS2D_CULTUREFLIP",
				MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY, // advc.106b
				ARTFILEMGR.getInterfaceArtPath("WORLDBUILDER_CITY_EDIT"),
				GC.getColorType("GREEN"));
		// <advc.101> Tell other civs about it (akin to code in CvCity::doRevolt)
		for (PlayerIter<MAJOR_CIV> it; it.hasNext(); ++it)
		{
			CvPlayer const& kObs = *it;
			if (kObs.getID() == getOwner() || kObs.getID() == eNewValue ||
				(!isRevealed(kObs.getTeam()) && /* advc.127: */ !kObs.isSpectator())) 
			{
				continue;
			}
			ColorTypes eColor = NO_COLOR;
			InterfaceMessageTypes eMsgType = MESSAGE_TYPE_MAJOR_EVENT;
			if (GET_TEAM(eNewValue).isVassal(kObs.getTeam()))
				eColor = GC.getColorType("GREEN");
			else if (GET_TEAM(pOldCity->getTeam()).isVassal(kObs.getTeam()))
				eColor = GC.getColorType("RED");
			else eMsgType = MESSAGE_TYPE_MAJOR_EVENT_LOG_ONLY; // advc.106b
			gDLL->UI().addMessage(kObs.getID(), false, -1, szBuffer, *this,
					0, eMsgType, ARTFILEMGR.getInterfaceArtPath("WORLDBUILDER_CITY_EDIT"),
					eColor);
		} // </advc.101>
		szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_REVOLTS_JOINS", pOldCity->getNameKey(),
				GET_PLAYER(eNewValue).getCivilizationDescriptionKey(),
				szOldOwnerDescr); // advc.101
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwner(),
				szBuffer, getX(), getY());
				// advc.106: Use ALT_HIGHLIGHT for research-related stuff now
				//,GC.getColorType("ALT_HIGHLIGHT_TEXT")
		FAssert(pOldCity->getOwner() != eNewValue);
		GET_PLAYER(eNewValue).acquireCity(pOldCity, false, false, bUpdatePlotGroup); // will delete the pointer
		pOldCity = NULL;
		CvCity* pNewCity = getPlotCity();
		CLinkList<IDInfo> oldUnits;
		{
			for (CLLNode<IDInfo> const* pUnitNode = headUnitNode(); pUnitNode != NULL;
				pUnitNode = nextUnitNode(pUnitNode))
			{
				oldUnits.insertAtEnd(pUnitNode->m_data);
			}
		}
		CLLNode<IDInfo>* pUnitNode = oldUnits.head();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = oldUnits.next(pUnitNode);
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->isEnemy(pNewCity->getTeam(), *this))
				{
					FAssert(pLoopUnit->getTeam() != pNewCity->getTeam());
					/*	<advc.101> Kill only Barbarians (which no longer get killed
						by CvCity::damageGarrison) */
					if (pLoopUnit->isBarbarian())
						pLoopUnit->kill(false, /*pNewCity->getPlayer()*/NO_PLAYER);
					else pLoopUnit->jumpToNearestValidPlot(); // </advc.101>
				}
			}
		}
		pNewCity->addRevoltFreeUnits(); // advc: Moved into new function
	}
	else
	{
		setOwnershipDuration(0);
		if (isOwned())
		{
			changeAdjacentSight(getTeam(), GC.getDefineINT(CvGlobals::PLOT_VISIBILITY_RANGE),
					false, NULL, bUpdatePlotGroup);
			getArea().changeNumOwnedTiles(-1);
			GC.getMap().changeOwnedPlots(-1);

			if (!isWater())
			{
				GET_PLAYER(getOwner()).changeTotalLand(-1);
				GET_TEAM(getTeam()).changeTotalLand(-1);
				if (isOwnershipScore())
					GET_PLAYER(getOwner()).changeTotalLandScored(-1);
			}

			if (isImproved())
			{
				GET_PLAYER(getOwner()).changeImprovementCount(getImprovementType(), -1);
				// Super Forts begin *culture*
				if (isFortImprovement() )
				{
					changeCultureRangeFortsWithinRange(getOwner(), -1, GC.getInfo(getImprovementType()).getCultureRange(), false);
				}
				// Super Forts end
			}
			updatePlotGroupBonus(false, /* advc.064d: */ false);
		}
		FOR_EACH_UNIT_VAR_IN(pUnit, *this)
		{
			if (pUnit->getTeam() != getTeam() && (getTeam() == NO_TEAM ||
				!GET_TEAM(getTeam()).isVassal(pUnit->getTeam())))
			{
				GET_PLAYER(pUnit->getOwner()).changeNumOutsideUnits(-1);
			}

			if (pUnit->isBlockading() &&
				// advc.033: Owner change shouldn't always disrupt blockade
				!pUnit->canPlunder(pUnit->getPlot()))
			{
				pUnit->setBlockading(false);
				pUnit->getGroup()->clearMissionQueue();
				pUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
			}
		}

		m_eOwner = eNewValue;
		updateTeam(); // advc.opt

		setWorkingCityOverride(NULL);
		updateWorkingCity();

		if (isOwned())
		{
			changeAdjacentSight(getTeam(), GC.getDefineINT(CvGlobals::PLOT_VISIBILITY_RANGE),
					true, NULL, bUpdatePlotGroup);
			getArea().changeNumOwnedTiles(1);
			GC.getMap().changeOwnedPlots(1);

			if (!isWater())
			{
				GET_PLAYER(getOwner()).changeTotalLand(1);
				GET_TEAM(getTeam()).changeTotalLand(1);
				if (isOwnershipScore())
					GET_PLAYER(getOwner()).changeTotalLandScored(1);
			}

			if (isImproved())
			{
				GET_PLAYER(getOwner()).changeImprovementCount(getImprovementType(), 1);
				// Super Forts begin *culture*
				if (isFortImprovement() //doto change
					/*GC.getInfo(getImprovementType()).isActsAsCity()*/)
				{
					changeCultureRangeFortsWithinRange(getOwner(), 1, GC.getInfo(getImprovementType()).getCultureRange(), true);
				}
				// Super Forts end
			}	
			updatePlotGroupBonus(true, /* advc.064d */ false);
		}
		FOR_EACH_UNIT_VAR_IN(pUnit, *this)
		{
			if (pUnit->getTeam() != getTeam() && (getTeam() == NO_TEAM ||
				!GET_TEAM(getTeam()).isVassal(pUnit->getTeam())))
			{
				GET_PLAYER(pUnit->getOwner()).changeNumOutsideUnits(1);
			}
		}

		for (TeamIter<ALIVE> it; it.hasNext(); ++it)
		{
			updateRevealedOwner(it->getID());
		}

		updateIrrigated();
		updateYield();

		if (bUpdatePlotGroup)
			updatePlotGroup();

		if (bCheckUnits)
			verifyUnitValidPlot();

		if (isOwned())
		{
			if (isGoody())
				GET_PLAYER(getOwner()).doGoody(this, NULL);

			for (TeamIter<CIV_ALIVE> it; it.hasNext();++it)
			{
				CvTeam& kLoopTeam = *it;
				if (isVisible(kLoopTeam.getID()))
				{
					FirstContactData fcData(this); // advc.071
					kLoopTeam.meet(getTeam(), true, /* advc.071: */ &fcData);
				}
			}
		}
		if (GC.getGame().isDebugMode())
		{
			updateMinimapColor();
			gDLL->UI().setDirty(GlobeLayer_DIRTY_BIT, true);
			gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
		}
	}
	// <advc.ctr> Wake up sleeping/ fortified human units
	if (eOldOwner != NO_PLAYER && GET_PLAYER(eOldOwner).isHuman() &&
		getOwner() != eOldOwner)
	{
		FOR_EACH_UNIT_VAR_IN(pUnit, *this)
		{
			CvSelectionGroup& kGroup = *pUnit->getGroup();
			if (kGroup.getOwner() == eOldOwner && kGroup.getLengthMissionQueue() <= 0 &&
				(kGroup.getActivityType() == ACTIVITY_SLEEP ||
				kGroup.getActivityType() == ACTIVITY_SENTRY))
			{
				kGroup.setActivityType(ACTIVITY_AWAKE);
			}
		}
	} // </advc.ctr>
	invalidateBorderDangerCache(); // K-Mod. (based on BBAI)
	updateSymbols();

//keldath - color city states plots - START
	NiColorA color = GC.getInfo(GC.getInfo(GET_PLAYER(getOwner()).getPlayerColor()).getColorTypePrimary()).getColor();
	bool changeIt = false;
	if (!isOwned())
	{
		// THE COLOR ISNT NEEDED here, i didnt know how to make a default null for this type of variable
		//so the function gets it - but there is no use for it.
		//same goes for the getplot
		//the bool is to clear or not to clear existing color
		color = (GC.getInfo(GC.getColorType("black")).getColor());
		changeIt = true;
	}
	GC.getGame().updateCityStatesColoredPlots(changeIt, *this, color);
//keldath - color city states plots - END

}

// <advc.035>
PlayerTypes CvPlot::getSecondOwner() const
{
	if(isCity())
		return getPlotCity()->getOwner();
	return (PlayerTypes)m_eSecondOwner;
}


void CvPlot::setSecondOwner(PlayerTypes eNewValue)
{
	m_eSecondOwner = (char)eNewValue;
} // </advc.035>


void CvPlot::setPlotType(PlotTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)
{
	bool bRecalculateAreas = false; // advc.030
	static TerrainTypes const eLAND_TERRAIN = (TerrainTypes)GC.getDefineINT("LAND_TERRAIN"); // advc.opt
	if (getPlotType() == eNewValue)
		return;

	if (getPlotType() == PLOT_OCEAN || eNewValue == PLOT_OCEAN)
		erase();

	bool const bWasWater = isWater();
	bool const bWasImpassable = isImpassable(); // advc.030

	updateSeeFromSight(false, true);

	m_ePlotType = eNewValue;

	updateImpassable(); // advc.opt
	updateYield();
	updatePlotGroup();

	updateSeeFromSight(true, true);

	if (getTerrainType() == NO_TERRAIN ||
		GC.getInfo(getTerrainType()).isWater() != isWater())
	{
		setTerrainType(isWater() ? GC.getWATER_TERRAIN(isAdjacentToLand()) :
				eLAND_TERRAIN, bRecalculate, bRebuildGraphics);
	}

	GC.getMap().resetPathDistance();

	if (bWasWater != isWater())
	{
		if (bRecalculate)
		{
			FOR_EACH_ADJ_PLOT_VAR(*this)
			{
				if (pAdj->isWater())
				{
					pAdj->setTerrainType(GC.getWATER_TERRAIN(pAdj->isAdjacentToLand()),
							bRecalculate, bRebuildGraphics);
				}
			}
		}
		FOR_EACH_ADJ_PLOT_VAR(*this)
		{
			pAdj->updateYield();
			pAdj->updatePlotGroup();
		}

		// (advc.129d: updatePotentialCityWork moved down)

		GC.getMap().changeLandPlots(isWater() ? -1 : 1);

		if (getBonusType() != NO_BONUS)
			GC.getMap().changeNumBonusesOnLand(getBonusType(), isWater() ? -1 : 1);

		if (isOwned())
		{
			GET_PLAYER(getOwner()).changeTotalLand(isWater() ? -1 : 1);
			GET_TEAM(getTeam()).changeTotalLand(isWater() ? -1 : 1);
		}

		if (bRecalculate)
		{
			CvArea* pNewArea = NULL;
			bRecalculateAreas = false;

			// XXX might want to change this if we allow diagonal water movement...
			if (isWater())
			{
				FOR_EACH_ORTH_ADJ_PLOT(*this)
				{
					if (!pAdj->getArea().isWater())
						continue;
					if (pNewArea == NULL)
						pNewArea = pAdj->area();
					else if (pNewArea != pAdj->area())
					{
						bRecalculateAreas = true;
						break;
					}
				}
			}
			else
			{
				FOR_EACH_ADJ_PLOT(*this)
				{
					if (pAdj->getArea().isWater())
						continue;
					if (pNewArea == NULL)
						pNewArea = pAdj->area();
					else if (pNewArea != pAdj->area())
					{
						bRecalculateAreas = true;
						break;
					}
				}
			}

			if (!bRecalculateAreas)
			{
				CvArea const* pLastArea = NULL;
				{	// advc: Was "pLoopPlot"; don't reuse.
					CvPlot const* pLastPlot = plotDirection(getX(), getY(),
							(DirectionTypes)(NUM_DIRECTION_TYPES - 1));
					if (pLastPlot != NULL)
						pLastArea = pLastPlot->area();
				}
				int iAreaCount = 0;
				FOR_EACH_ENUM(Direction)
				{
					CvPlot const* pCurrPlot = plotDirection(getX(), getY(),
							eLoopDirection);
					CvArea* pCurrArea = NULL;
					if (pCurrPlot != NULL)
						pCurrArea = pCurrPlot->area();
					if (pCurrArea != pLastArea)
						iAreaCount++;
					pLastArea = pCurrArea;
				}
				if (iAreaCount > 2)
					bRecalculateAreas = true;
			}
			if (bRecalculateAreas)
				GC.getMap().recalculateAreas();
			else
			{
				CvArea* pOldArea = area(); // advc
				setArea(NULL);
				if (pOldArea != NULL && pOldArea->getNumTiles() == 1)
					GC.getMap().deleteArea(pOldArea->getID());
				if (pNewArea == NULL)
				{
					pNewArea = GC.getMap().addArea();
					pNewArea->init(isWater());
				}
				setArea(pNewArea);
			}
		}
	}
	// <advc.129d>
	if (isWater() != bWasWater || isImpassable() != bWasImpassable)
	{	// Moved from above. Now also needed when impassable status changes.
		for (CityPlotIter it(*this); it.hasNext(); ++it)
		{
			it->updatePotentialCityWork();
		}
	} // </advc.129d>
	// <advc.030>
	if (!isWater() && bWasImpassable != isImpassable() &&
		!bRecalculateAreas && bRecalculate)
	{
		/*  When removing a peak, it's easy enough to tell whether we need to
			recalc, but too much work to come up with conditions for recalc
			when placing a peak; will have to always recalc. */
		if (isPeak())
			GC.getMap().recalculateAreas();
		else
		{
			FOR_EACH_ADJ_PLOT(*this)
			{
				if (!pAdj->isWater() && !sameArea(*pAdj))
				{
					GC.getMap().recalculateAreas();
					break;
				}
			}
		}
	} // </advc.030>
	if (bRebuildGraphics && GC.IsGraphicsInitialized())
	{
		//Update terrain graphical
		gDLL->getEngineIFace()->RebuildPlot(getX(), getY(), true, true);
		//gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true); //minimap does a partial update
		//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);

		updateFeatureSymbol();
		setLayoutDirty(true);
		updateRouteSymbol(false, true);
		updateRiverSymbol(false, true);
	}
}


void CvPlot::setTerrainType(TerrainTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)
{
	if(getTerrainType() == eNewValue)
		return;

	bool bUpdateSight = (getTerrainType() != NO_TERRAIN && // advc
			eNewValue != NO_TERRAIN &&
			(GC.getInfo(getTerrainType()).getSeeFromLevel() !=
			GC.getInfo(eNewValue).getSeeFromLevel() ||
			GC.getInfo(getTerrainType()).getSeeThroughLevel() !=
			GC.getInfo(eNewValue).getSeeThroughLevel()));

	if (bUpdateSight)
		updateSeeFromSight(false, true);

	m_eTerrainType = eNewValue;

	updateImpassable(); // advc.opt
	updateYield();
	updatePlotGroup();

	if (bUpdateSight)
		updateSeeFromSight(true, true);

	if (bRebuildGraphics && GC.IsGraphicsInitialized())
	{
		//Update terrain graphics
		gDLL->getEngineIFace()->RebuildPlot(getX(), getY(), false, true);
		//gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true); //minimap does a partial update
		//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
	}

	if (GC.getInfo(getTerrainType()).isWater() != isWater())
	{
		setPlotType(GC.getInfo(getTerrainType()).isWater() ? PLOT_OCEAN : PLOT_LAND,
				bRecalculate, bRebuildGraphics);
	}
/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 15.10.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: Enable isRequiresFlatlands for Terrain                                           **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
	//mountains back in service addition- keldath

		if (isHills() || isPeak())
		{
			if (GC.getTerrainInfo(getTerrainType()).isRequiresFlatlands() != isFlatlands())
			{
				setPlotType(((GC.getTerrainInfo(getTerrainType()).isRequiresFlatlands()) ? PLOT_LAND : PLOT_HILLS), bRecalculate, bRebuildGraphics);
			}
		}

/*****************************************************************************************************/
/**  TheLadiesOgre; 15.10.2009; TLOTags                                                             **/
/*****************************************************************************************************/
}


void CvPlot::setFeatureType(FeatureTypes eNewValue, int iVariety)
{
	if (eNewValue != NO_FEATURE)
	{
		if (iVariety == -1)
		{
			iVariety = ((GC.getInfo(eNewValue).getArtInfo()->getNumVarieties() *
					((getLatitude() * 9) / 8)) / 90);
		}

		iVariety = range(iVariety, 0, (GC.getInfo(eNewValue).getArtInfo()->getNumVarieties() - 1));
	}
	else iVariety = 0;

	FeatureTypes eOldFeature = getFeatureType();
	if(eOldFeature == eNewValue && m_iFeatureVariety == iVariety)
		return; // advc

	bool bUpdateSight = false;

	if (eOldFeature == NO_FEATURE || eNewValue == NO_FEATURE ||
		GC.getInfo(eOldFeature).getSeeThroughChange() != GC.getInfo(eNewValue).getSeeThroughChange())
	{
		bUpdateSight = true;
	}

	if (bUpdateSight)
		updateSeeFromSight(false, true);

	m_eFeatureType = eNewValue;
	m_iFeatureVariety = iVariety;

	updateImpassable(); // advc.opt
	updateYield();

	if (bUpdateSight)
		updateSeeFromSight(true, true);

	updateFeatureSymbol();

	if ((eOldFeature != NO_FEATURE && GC.getInfo(eOldFeature).getArtInfo()->isRiverArt()) ||
		(isFeature() && GC.getInfo(getFeatureType()).getArtInfo()->isRiverArt()))
	{
		updateRiverSymbolArt(true);
	}

// Deliverator fresh water
	if (((eOldFeature != NO_FEATURE) && (GC.getFeatureInfo(eOldFeature).isAddsFreshWater())) &&
			 ((getFeatureType() == NO_FEATURE) || !(GC.getFeatureInfo(getFeatureType()).isAddsFreshWater())))
	{
		changeFreshWaterInRadius(-1, 1);
	}

	if (((eOldFeature == NO_FEATURE) || !(GC.getFeatureInfo(eOldFeature).isAddsFreshWater())) &&
			 ((getFeatureType() != NO_FEATURE) && (GC.getFeatureInfo(getFeatureType()).isAddsFreshWater())))
	{
		changeFreshWaterInRadius(1, 1);
	}
// Deliverator
	for (NearbyCityIter itCity(*this); itCity.hasNext(); ++itCity)
	{
		itCity->updateSurroundingHealthHappiness();
	}
//feature on improvement - doto
	if (eNewValue != NO_FEATURE)
	{
		if (!isFeature() && isImproved() && !GC.getFeatureInfo(eNewValue).isOnImprovement())
		{
			if (GC.getInfo(getImprovementType()).isRequiresFeature())
				setImprovementType(NO_IMPROVEMENT);
		}
	}
//feature on improvement - doto
}


void CvPlot::setFeatureDummyVisibility(const char *dummyTag, bool show)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
		gDLL->getFeatureIFace()->setDummyVisibility(m_pFeatureSymbol, dummyTag, show);
}


void CvPlot::addFeatureDummyModel(const char *dummyTag, const char *modelTag)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
		gDLL->getFeatureIFace()->addDummyModel(m_pFeatureSymbol, dummyTag, modelTag);
}


void CvPlot::setFeatureDummyTexture(const char *dummyTag, const char *textureTag)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
		gDLL->getFeatureIFace()->setDummyTexture(m_pFeatureSymbol, dummyTag, textureTag);
}


CvString CvPlot::pickFeatureDummyTag(int mouseX, int mouseY)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
		return gDLL->getFeatureIFace()->pickDummyTag(m_pFeatureSymbol, mouseX, mouseY);
	return NULL;
}


void CvPlot::resetFeatureModel()
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
		gDLL->getFeatureIFace()->resetModel(m_pFeatureSymbol);
}


BonusTypes CvPlot::getBonusType(TeamTypes eTeam) const
{
	BonusTypes eR = (BonusTypes)m_eBonusType;
	if (eTeam != NO_TEAM && eR != NO_BONUS)
	{
		if (!GET_TEAM(eTeam).isBonusRevealed(eR)) // K-Mod: moved into helper function
			return NO_BONUS;
	}
	return eR;
}


BonusTypes CvPlot::getNonObsoleteBonusType(TeamTypes eTeam,
	bool bCheckConnected) const // K-Mod
{
	FAssert(eTeam != NO_TEAM);
//doto super forts
//on kill civ, city change owner is executed, 
//due to change culture.
//so i added a safe check from an assert that popped when the team was dead
	if(GC.getGame().isOption(GAMEOPTION_SUPER_FORTS) && !GET_TEAM(eTeam).isAlive())
		return NO_BONUS;
//end of doto super forts
	FAssert(GET_TEAM(eTeam).isAlive()); // K-Mod

	BonusTypes eBonus = getBonusType(eTeam);
	if (eBonus == NO_BONUS)
		return NO_BONUS; // advc
	if (GET_TEAM(eTeam).isBonusObsolete(eBonus))
		return NO_BONUS;

	// K-Mod
	if (bCheckConnected)
	{
		/*	note: this checks whether the bonus is connected for the owner of the plot,
			from the point of view of eTeam. */
		TeamTypes ePlotTeam = getTeam();
		if (ePlotTeam == NO_TEAM ||
			!GET_TEAM(ePlotTeam).isHasTech(GC.getInfo(eBonus).getTechCityTrade()))
		{
			return NO_BONUS;
		}
		/*	note: this function is used inside CvPlot::updatePlotGroupBonuses,
			which is called during CvPlot::setImprovementType between when
			the improvement is changed and the revealed improvement type is updated...
			therefore when eTeam == ePlotTeam, we use the real improvement,
			not the revealed one. */
		ImprovementTypes eImprovement = (eTeam == NO_TEAM || eTeam == ePlotTeam ?
				getImprovementType() : getRevealedImprovementType(eTeam));

		FAssert(ePlotTeam != eTeam || eImprovement == getImprovementType());

		if (!isCity() && !GET_TEAM(ePlotTeam).doesImprovementConnectBonus(eImprovement, eBonus))
			return NO_BONUS;
	} // K-Mod end

	return eBonus;
}


void CvPlot::setBonusType(BonusTypes eNewValue)
{
	if(getBonusType() == eNewValue)
		return;

	if (getBonusType() != NO_BONUS)
	{
		getArea().changeNumBonuses(getBonusType(), -1);
		GC.getMap().changeNumBonuses(getBonusType(), -1);

		if (!isWater())
			GC.getMap().changeNumBonusesOnLand(getBonusType(), -1);
	}

	updatePlotGroupBonus(false, /* advc.064d: */ false);
	m_eBonusType = eNewValue;
	updatePlotGroupBonus(true);

	if (getBonusType() != NO_BONUS)
	{
		getArea().changeNumBonuses(getBonusType(), 1);
		GC.getMap().changeNumBonuses(getBonusType(), 1);
		if (!isWater())
			GC.getMap().changeNumBonusesOnLand(getBonusType(), 1);
	}

	updateYield();
	setLayoutDirty(true);
	gDLL->UI().setDirty(GlobeLayer_DIRTY_BIT, true);
}


void CvPlot::setImprovementType(ImprovementTypes eNewValue,
	bool bUpdateInFoW) // advc.055
{
	ImprovementTypes const eOldImprovement = getImprovementType();
	if(getImprovementType() == eNewValue)
		return;
	// <advc.183>
	bool const bActedAsCity = (eOldImprovement != NO_IMPROVEMENT &&
			GC.getInfo(eOldImprovement).isActsAsCity()); // </advc.183>
	// <advc.004z>
	bool const bWasGoody = (eOldImprovement != NO_IMPROVEMENT &&
			GC.getInfo(eOldImprovement).isGoody()); // </advc.004z>

	if (eOldImprovement != NO_IMPROVEMENT)
	{	// advc.opt:
		/*if (area())
			getArea().changeNumImprovements(eOldImprovement, -1);*/
		if (isOwned())
		{
			GET_PLAYER(getOwner()).changeImprovementCount(eOldImprovement, -1);
		// Super Forts begin *culture*
			if (isFortImprovement())
			{
				changeCultureRangeFortsWithinRange(getOwner(), -1, GC.getInfo(getImprovementType()).getCultureRange(), true);
			}
	    }
		// Super Forts end
	}
	// Super Forts begin *vision*
	if (isFortImprovement())
	{
		updateSight(false, true);
	}
	// Super Forts end
	
	updatePlotGroupBonus(false, /* advc.064d: */ false);
	m_eImprovementType = eNewValue;
	updatePlotGroupBonus(true);

// < JCultureControl Mod Start >
	if (eOldImprovement != NO_IMPROVEMENT && getImprovementOwner() != NO_PLAYER && GC.getGame().isOption(GAMEOPTION_CULTURE_CONTROL))
	{
		clearCultureControl(getImprovementOwner(), eOldImprovement, true);
	}
// < JCultureControl Mod End >
	if (!isImproved())
	{
		setImprovementDuration(0);
// < JCultureControl Mod Start >
        if (getImprovementOwner() != NO_PLAYER && GC.getGame().isOption(GAMEOPTION_CULTURE_CONTROL))
        {
            setImprovementOwner(NO_PLAYER);
        }
// < JCultureControl Mod End >
	}
// Deliverator fresh water
	// Remove fresh water from old improvement
	if (eOldImprovement != NO_IMPROVEMENT) {
		int oldFreshWaterRadius = GC.getImprovementInfo(eOldImprovement).getAddsFreshWaterInRadius();	
		if (oldFreshWaterRadius >= 0) {
			changeFreshWaterInRadius(-1, oldFreshWaterRadius);
		}
	}

	// Add fresh water for new improvement
	if (eNewValue != NO_IMPROVEMENT) {
		int newFreshWaterRadius = GC.getImprovementInfo(eNewValue).getAddsFreshWaterInRadius();
		if (newFreshWaterRadius >= 0) {
			changeFreshWaterInRadius(1, newFreshWaterRadius);
		}
	}
// Deliverator
	setUpgradeProgress(0);

	for (TeamIter<ALIVE> it; it.hasNext(); ++it)
	{
		TeamTypes const eTeam = it->getID();
		// <advc>
		if (!isRevealed(eTeam))
			continue; // </advc>
		if (isVisible(eTeam) || /* advc.055: */ bUpdateInFoW)
			setRevealedImprovementType(eTeam, getImprovementType());
		/*	<advc.183> Can't keep destroyed forts secret now that air movement is
			based on the revealed improvement type (cf. CvUnit::canMoveInto). */
		else if (bActedAsCity)
			setRevealedImprovementType(eTeam, NO_IMPROVEMENT); // </advc.183>
	}

	if (isImproved())
	{	// advc.opt:
		/*if (area())
			getArea().changeNumImprovements(getImprovementType(), 1);*/
		if (isOwned())
		{
			GET_PLAYER(getOwner()).changeImprovementCount(getImprovementType(), 1);
			// Super Forts begin *culture*
			if (isFortImprovement())
			{
					changeCultureRangeFortsWithinRange(getOwner(), 1, GC.getInfo(getImprovementType()).getCultureRange(), true);
			}
			// Super Forts end
		}
	}
	// Super Forts begin *vision*
	if (isFortImprovement())
	{
		updateSight(true, true);
	}
	// Super Forts end

	updateIrrigated();
	updateYield();

	for (NearbyCityIter itCity(*this); itCity.hasNext(); ++itCity)
	{
		itCity->updateSurroundingHealthHappiness();
	}

	/*	Building or removing a fort will now force a plotgroup update to
		verify resource connections. */
	if ((isImproved() &&
		GC.getInfo(getImprovementType()).isActsAsCity()) !=
		bActedAsCity)
	{
		updatePlotGroup(/* advc.064d: */ true);
	}

	if (bActedAsCity)
		verifyUnitValidPlot();

	if (GC.getGame().isDebugMode())
		setLayoutDirty(true);

	if (isImproved())
	{
		CvEventReporter::getInstance().improvementBuilt(
				getImprovementType(), getX(), getY());
	}
	else
	{
		CvEventReporter::getInstance().improvementDestroyed(
				eOldImprovement, getOwner(), getX(), getY());
	}
	// <advc.004z> Update resource indicators (if we must)
	if (bWasGoody && (!isImproved() || !GC.getInfo(getImprovementType()).isGoody()) &&
		GC.getGame().getCurrentLayer() == GLOBE_LAYER_RESOURCE &&
		isVisibleToWatchingHuman() &&
		GET_PLAYER(getActivePlayer()).showGoodyOnResourceLayer())
	{
		gDLL->UI().setDirty(GlobeLayer_DIRTY_BIT, true);
	} // </advc.004z>
	CvCity* pWorkingCity = getWorkingCity();
	if (pWorkingCity != NULL)
	{
		if ((eNewValue != NO_IMPROVEMENT &&
			pWorkingCity->getImprovementFreeSpecialists(eNewValue) > 0)	||
			(eOldImprovement != NO_IMPROVEMENT &&
			pWorkingCity->getImprovementFreeSpecialists(eOldImprovement) > 0))
		{
			pWorkingCity->AI_setAssignWorkDirty(true);
		}
	}

	gDLL->UI().setDirty(CitizenButtons_DIRTY_BIT, true);
}


void CvPlot::setRouteType(RouteTypes eNewValue, bool bUpdatePlotGroups)
{
	if(getRouteType() == eNewValue)
		return;

	bool const bOldRoute = isRoute(); // XXX is this right???

	updatePlotGroupBonus(false, /* advc.064d: */ false);
	m_eRouteType = eNewValue;
	updatePlotGroupBonus(true);

	for (TeamIter<ALIVE> itTeam; itTeam.hasNext(); ++itTeam)
	{
		if (isVisible(itTeam->getID()))
			setRevealedRouteType(itTeam->getID(), getRouteType());
	}

	updateYield();

	if (bUpdatePlotGroups)
	{
		if (bOldRoute != isRoute())
			updatePlotGroup(/* advc.064d: */ bOldRoute);
	}

	if (GC.getGame().isDebugMode())
		updateRouteSymbol(true, true);

	if (isRoute())
		CvEventReporter::getInstance().routeBuilt(getRouteType(), getX(), getY());

	// K-Mod. Fixing a bug in the border danger cache from BBAI.
	if (bOldRoute && !isRoute())
		invalidateBorderDangerCache();
	// K-Mod end
}


void CvPlot::updateCityRoute(bool bUpdatePlotGroup)
{
	if (!isCity())
		return;

	FAssert(isOwned());

	RouteTypes eCityRoute = GET_PLAYER(getOwner()).getBestRoute();
	if (eCityRoute == NO_ROUTE)
	{	// <advc.opt>
		static const RouteTypes eINITIAL_CITY_ROUTE_TYPE = (RouteTypes)
				GC.getDefineINT("INITIAL_CITY_ROUTE_TYPE"); // </advc.opt>
		eCityRoute = eINITIAL_CITY_ROUTE_TYPE;
	}
	setRouteType(eCityRoute, bUpdatePlotGroup);
}


CvCity* CvPlot::getPlotCity() const
{
	return ::getCity(m_plotCity);
}

// <advc.003u>
CvCityAI* CvPlot::AI_getPlotCity() const
{
	return ::AI_getCity(m_plotCity);
} // </advc.003u>


void CvPlot::setPlotCity(CvCity* pNewCity)
{
	CvCity* pOldCity = getPlotCity();
	if (pOldCity == pNewCity)
		return;
	if (pOldCity != NULL)
	{
		for (CityPlotIter itPlot(*pOldCity); itPlot.hasNext(); ++itPlot)
		{
			itPlot->changeCityRadiusCount(-1);
			itPlot->changePlayerCityRadiusCount(pOldCity->getOwner(), -1);
		}
	}
	updatePlotGroupBonus(false, /* advc.064d: */ false);
	if (pOldCity != NULL)
	{
		CvPlotGroup* pPlotGroup = getPlotGroup(getOwner());
		if (pPlotGroup != NULL)
		{
			FOR_EACH_ENUM(Bonus)
			{
				getPlotCity()->changeNumBonuses((eLoopBonus),
						-pPlotGroup->getNumBonuses(eLoopBonus));
			}			
			// < Building Resource Converter Start >
			getPlotCity()->processBuildingBonuses();
			// < Building Resource Converter End   >
		}
	}
	if (pNewCity == NULL)
		m_plotCity.reset();
	else
	{
		m_plotCity = pNewCity->getIDInfo();
		FAssert(getPlotCity() == pNewCity);
		CvPlotGroup* pPlotGroup = getPlotGroup(getOwner());
		if (pPlotGroup != NULL)
		{
			FOR_EACH_ENUM(Bonus)
			{
				getPlotCity()->changeNumBonuses(eLoopBonus,
						pPlotGroup->getNumBonuses(eLoopBonus));
			}
			// < Building Resource Converter Start >
			getPlotCity()->processBuildingBonuses();
			// < Building Resource Converter End   
		}
	}
	updatePlotGroupBonus(true);
	if (pNewCity != NULL)
	{
		for (CityPlotIter itPlot(*pNewCity); itPlot.hasNext(); ++itPlot)
		{
			itPlot->changeCityRadiusCount(1);
			itPlot->changePlayerCityRadiusCount(pNewCity->getOwner(), 1);
		}
	}
	updateIrrigated();
	updateYield();
	updateMinimapColor();
}

// <advc.005c>
void CvPlot::setRuinsName(CvWString const& szName)
{
	SAFE_DELETE_ARRAY(m_szMostRecentCityName);
	if (szName.empty())
		return;
	// Copying szName.c_str() into a C string takes some work
	wchar* szBuffer = new wchar[szName.length() + 1]; // +1 for \0
	wcsncpy(szBuffer, szName.c_str(), szName.length() + 1);
	m_szMostRecentCityName = szBuffer;
	FAssert(wcslen(m_szMostRecentCityName) == szName.length());
}


const wchar* CvPlot::getRuinsName() const
{
	return m_szMostRecentCityName;
} // </advc.005c>


CvCity* CvPlot::getWorkingCity() const
{
	return ::getCity(m_workingCity);
}


CvCity* CvPlot::getWorkingCityOverride() const
{
	return ::getCity(m_workingCityOverride);
}

// <advc.003u>
CvCityAI* CvPlot::AI_getWorkingCity() const
{
	return ::AI_getCity(m_workingCity);
}


CvCityAI* CvPlot::AI_getWorkingCityOverrideAI() const
{
	return ::AI_getCity(m_workingCityOverride);
} // </advc.003u>

void CvPlot::updateWorkingCity()
{
	CvCity const* pBestCity = getPlotCity();
	if (pBestCity == NULL)
	{
		pBestCity = getWorkingCityOverride();
		FAssert(pBestCity == NULL || pBestCity->getOwner() == getOwner());
	}

	if (pBestCity == NULL && isOwned())
		pBestCity = defaultWorkingCity(); // advc: Moved into new function

	CvCity* pOldWorkingCity = getWorkingCity();
	if (pOldWorkingCity == pBestCity)
		return;

	if (pOldWorkingCity != NULL)
		pOldWorkingCity->setWorkingPlot(*this, false);

	if (pBestCity != NULL)
	{
		FAssert(isOwned());
		FAssert(!isBeingWorked());
		m_workingCity = pBestCity->getIDInfo();
	}
	else m_workingCity.reset();

	if (pOldWorkingCity != NULL)
		pOldWorkingCity->AI_setAssignWorkDirty(true);
	if (getWorkingCity() != NULL)
		getWorkingCity()->AI_setAssignWorkDirty(true);

	updateYield();
	updateFog();
	updateShowCitySymbols();

	if (isActiveOwned())
	{
		if (gDLL->getGraphicOption(GRAPHICOPTION_CITY_RADIUS))
		{
			//if (gDLL->UI().canSelectionListFound())
			// <advc.004h>
			CvUnit* pHeadSelectedUnit = gDLL->UI().getHeadSelectedUnit();
			if(pHeadSelectedUnit != NULL && pHeadSelectedUnit->isFound()) // </advc.004h>
				gDLL->UI().setDirty(ColoredPlots_DIRTY_BIT, true);
		}
	}
}

// advc: Cut from updateWorkingCity (for advc.ctr)
CvCity const* CvPlot::defaultWorkingCity() const
{
	CvCity const* pBestCity = NULL;
	int iBestPriority = MAX_INT;
	for (NearbyCityIter itCity(*this); itCity.hasNext(); ++itCity)
	{
		if (itCity->getOwner() != getOwner())
			continue;
		int const iPriority = itCity.cityPlotPriority();
		if (iPriority < iBestPriority ||
			(iPriority == iBestPriority &&
			// XXX use getGameTurnAcquired() instead???
			(itCity->getGameTurnFounded() < pBestCity->getGameTurnFounded() ||
			(itCity->getGameTurnFounded() == pBestCity->getGameTurnFounded() &&
			itCity->getID() < pBestCity->getID()))))
		{
			iBestPriority = iPriority;
			pBestCity = &*itCity;
		}
	}
	return pBestCity;
}


void CvPlot::setWorkingCityOverride( const CvCity* pNewValue)
{
	if (getWorkingCityOverride() == pNewValue)
		return; // advc

	if (pNewValue != NULL)
	{
		FAssert(pNewValue->getOwner() == getOwner());
		m_workingCityOverride = pNewValue->getIDInfo();
	}
	else m_workingCityOverride.reset();

	updateWorkingCity();
}


short CvPlot::getRiverID() const
{
	return m_iRiverID;
}


void CvPlot::setRiverID(short iNewValue)
{
	m_iRiverID = iNewValue;
}


int CvPlot::getMinOriginalStartDist() const
{
	return m_iMinOriginalStartDist;
}


void CvPlot::setMinOriginalStartDist(int iNewValue)
{
	m_iMinOriginalStartDist = safeIntCast<short>(iNewValue);
}


int CvPlot::getReconCount() const
{
	return m_iReconCount;
}


void CvPlot::changeReconCount(int iChange)
{
	m_iReconCount += iChange;
	FAssert(getReconCount() >= 0);
}


int CvPlot::getRiverCrossingCount() const
{
	return m_iRiverCrossingCount;
}


void CvPlot::changeRiverCrossingCount(int iChange)
{
	m_iRiverCrossingCount += iChange;
	FAssert(getRiverCrossingCount() >= 0);
}

// advc.300:
bool CvPlot::isHabitable(bool bIgnoreSea) const
{
	if (getTerrainType() == NO_TERRAIN) // Can be called during map gen
		return false;
	if (calculateNatureYield(YIELD_FOOD, NO_TEAM, false, true) <= 0)
		return false;
	if (!isWater() || isLake())
		return true;
	if (bIgnoreSea)
		return false;
	// Count shelf as habitable, but not arctic shelf or adj. only to one land corner.
	int iAdjHabitableLand = 0;
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isHabitable(true))
			iAdjHabitableLand++;
		if (iAdjHabitableLand >= 2)
			return true;
	}
	return false;
}


int CvPlot::calculateNatureYield(YieldTypes eYield, TeamTypes eTeam, bool bIgnoreFeature,
	bool bIgnoreHills) const // advc.300
{
	// advc.016: Cut from calculateYield
	int iYield = GC.getGame().getPlotExtraYield(getX(), getY(), eYield);
	if (isImpassable())
	{
		//return 0;
		/*  advc.016: Impassable tiles with extra yields can be worked -
			as in BtS. This allows Python modders to make peaks workable. */
		return iYield;
	}
	iYield += GC.getInfo(getTerrainType()).getYield(eYield);
	bool const bHills = (isHills() /* advc.300 */ && !bIgnoreHills);
	if (bHills)
		iYield += GC.getInfo(eYield).getHillsChange();
	else if (isPeak())
		iYield += GC.getInfo(eYield).getPeakChange();
	else if (isLake())
		iYield += GC.getInfo(eYield).getLakeChange();
	if (eTeam != NO_TEAM)
	{
		BonusTypes eBonus = getBonusType(eTeam);
		if (eBonus != NO_BONUS)
			iYield += GC.getInfo(eBonus).getYieldChange(eYield);
	}
	if (isRiver())
	{
		/* <advc.500a> No change to the assigned value, but add it twice if more than
		   one river. */
		int iYieldPerRiver = ((bIgnoreFeature || !isFeature()) ?
				GC.getInfo(getTerrainType()).getRiverYieldChange(eYield) :
				GC.getInfo(getFeatureType()).getRiverYieldChange(eYield));
		int iRivers = 1;
		/*if(isConnectRiverSegments()) // Disabled for now
			iRivers++;*/
		iYield += iRivers * iYieldPerRiver; // </advc.500a>
	}

	if (bHills)
	{
		iYield += ((bIgnoreFeature || !isFeature()) ?
				GC.getInfo(getTerrainType()).getHillsYieldChange(eYield) :
				GC.getInfo(getFeatureType()).getHillsYieldChange(eYield));
	}

	if (!bIgnoreFeature)
	{
		if (isFeature())
			iYield += GC.getInfo(getFeatureType()).getYieldChange(eYield);
	}

	return std::max(0, iYield);
}


int CvPlot::calculateBestNatureYield(YieldTypes eIndex, TeamTypes eTeam) const
{
	return std::max(calculateNatureYield(eIndex, eTeam, false),
			calculateNatureYield(eIndex, eTeam, true));
}


int CvPlot::calculateTotalBestNatureYield(TeamTypes eTeam) const
{
	return calculateBestNatureYield(YIELD_FOOD, eTeam) +
			calculateBestNatureYield(YIELD_PRODUCTION, eTeam) +
			calculateBestNatureYield(YIELD_COMMERCE, eTeam);
}

/*	advc: Params bBestRoute (BBAI) and bOptimal (Vanilla Civ 4) removed b/c
	they've been obsoleted by K-Mod. Had been used by CvCityAI. */
int CvPlot::calculateImprovementYieldChange(
	ImprovementTypes eImprovement, YieldTypes eYield,
	PlayerTypes ePlayer) const
{
	PROFILE_FUNC();

	CvImprovementInfo const& kImpr = GC.getInfo(eImprovement);
	int iYield = kImpr.getYieldChange(eYield);
	if (isRiverSide())
		iYield += kImpr.getRiverSideYieldChange(eYield);
	if (isHills())
		iYield += kImpr.getHillsYieldChange(eYield);
	if (isIrrigationAvailable())
		iYield += kImpr.getIrrigatedYieldChange(eYield);
	/*	<advc.182> Compute the yield of the plot owner. Fall back on ePlayer
		only for (apparently) unowned tiles. Use the map knowledge of ePlayer. */
	TeamTypes const eObs = (ePlayer == NO_PLAYER ? NO_TEAM : TEAMID(ePlayer));
	PlayerTypes eRevealedOwner = (eObs == NO_TEAM ? getOwner() :
			getRevealedOwner(eObs));
	PlayerTypes eYieldPlayer = (eRevealedOwner == NO_PLAYER ? ePlayer :
			eRevealedOwner); // </advc.182>
	{	// <advc.001i>
		RouteTypes eRoute = (eObs == NO_TEAM ? getRouteType() :
				getRevealedRouteType(eObs)); // </advc.001i>
		if (eRoute != NO_ROUTE)
			iYield += kImpr.getRouteYieldChanges(eRoute, eYield);
	}
	
	if (eYieldPlayer == NO_PLAYER)
	{
		FOR_EACH_ENUM(Tech)
		{
			iYield += kImpr.getTechYieldChanges(eLoopTech, eYield);
		}
		/*	K-Mod (note): this doesn't calculate the 'optimal' yield, because it
			will count negative effects and it will count effects from competing civics. */
		/*FOR_EACH_ENUM(Civic)
			iYield += GC.getInfo(eLoopCivic).getImprovementYieldChanges(eImprovement, eYield);*/
		// <advc.001> What he said ... (Though this whole branch isn't used much.)
		EagerEnumMap<CivicOptionTypes,int> aiMaxYieldPerCivicOption;
		FOR_EACH_ENUM(Civic)
		{
			int iYieldChange = GC.getInfo(eLoopCivic).
					getImprovementYieldChanges(eImprovement, eYield);
			if (iYieldChange > 0)
			{
				CivicOptionTypes eLoopCivicOption = GC.getInfo(eLoopCivic).
						getCivicOptionType();
				aiMaxYieldPerCivicOption.set(eLoopCivicOption, std::max(
						aiMaxYieldPerCivicOption.get(eLoopCivicOption), iYieldChange));
			}
		}
		FOR_EACH_ENUM(CivicOption)
		{
			iYield += aiMaxYieldPerCivicOption.get(eLoopCivicOption);
		} // </advc.001>
	}
	else
	{
		iYield += GET_PLAYER(eYieldPlayer).
				getImprovementYieldChange(eImprovement, eYield);
		iYield += GET_TEAM(eYieldPlayer).
				getImprovementYieldChange(eImprovement, eYield);
	}
	//if (ePlayer != NO_PLAYER) // advc.182
	{
		BonusTypes eBonus = getBonusType(eObs/*TEAMID(ePlayer)*/); // advc.182
		if (eBonus != NO_BONUS)
			iYield += kImpr.getImprovementBonusYield(eBonus, eYield);
	}
	return iYield;
}


int CvPlot::calculateYield(YieldTypes eYield, bool bDisplay) const
{
	if (getTerrainType() == NO_TERRAIN)  // (advc: Can happen during map initialization)
		return 0;

	if (!isPotentialCityWork())
		return 0;

	if (bDisplay && GC.getGame().isDebugMode()) // (advc.129d: Moved down)
		return getYield(eYield);

	PlayerTypes ePlayer;
	ImprovementTypes eImprovement;
	RouteTypes eRoute;
	if (bDisplay)
	{
		ePlayer = getRevealedOwner(getActiveTeam());
		eImprovement = getRevealedImprovementType(getActiveTeam());
		eRoute = getRevealedRouteType(getActiveTeam());
		if (ePlayer == NO_PLAYER)
			ePlayer = getActivePlayer();
	}
	else
	{
		ePlayer = getOwner();
		eImprovement = getImprovementType();
		eRoute = getRouteType();
	}
	int iNatureYield = // advc.908a: Preserve this for later
			calculateNatureYield(eYield,
			bDisplay ? getActiveTeam() : // advc.182
			/*	(advc: Note that NO_TEAM means that bonus resources are ignored.
				In most other places it means god mode.) */
			(ePlayer != NO_PLAYER ? TEAMID(ePlayer) : NO_TEAM));

	int iYield = iNatureYield;

	if (eImprovement != NO_IMPROVEMENT)
	{
		iYield += calculateImprovementYieldChange(eImprovement, eYield, //ePlayer
				bDisplay ? getActivePlayer() : ePlayer); // advc.182
	}
	if (eRoute != NO_ROUTE)
		iYield += GC.getInfo(eRoute).getYieldChange(eYield);

	if (ePlayer != NO_PLAYER)
	{
		if (isWater() && !isImpassable())
		{
			iYield += GET_PLAYER(ePlayer).getSeaPlotYield(eYield);
			CvCity* pWorkingCity = getWorkingCity();
			if (pWorkingCity != NULL)
			{
				if (!bDisplay || pWorkingCity->isRevealed(getActiveTeam()))
					iYield += pWorkingCity->getSeaPlotYield(eYield);
			}
		}

		if (isRiver() && !isImpassable())
		{
			CvCity* pWorkingCity = getWorkingCity();
			if (pWorkingCity != NULL)
			{
				if (!bDisplay || pWorkingCity->isRevealed(getActiveTeam()))
					iYield += pWorkingCity->getRiverPlotYield(eYield);
			}
		}

		CvCity* pCity = getPlotCity(); // advc: Moved down (no functional change intended)
		if (pCity != NULL &&
			(!bDisplay || pCity->isRevealed(getActiveTeam())))
		{
			// advc.031: Moved into new function
			iYield += calculateCityPlotYieldChange(eYield, iYield, pCity->getPopulation());
		}
	}

	// advc.016: Now factored into NatureYield
	//iYield += GC.getGame().getPlotExtraYield(getX(), getY(), eYield);

	if (ePlayer != NO_PLAYER)
	{
		{
			int iThresh = GET_PLAYER(ePlayer).getExtraYieldThreshold(eYield);
			if (iThresh > 0)
			{
				if(iYield >= iThresh)
					iYield += GC.getDefineINT(CvGlobals::EXTRA_YIELD);
			}
		}
		// <advc.908a>
		{
			int iThresh = GET_PLAYER(ePlayer).getExtraYieldNaturalThreshold(eYield);
			if (iThresh > 0)
			{
				if (iYield >= iThresh || iNatureYield + 1 >= iThresh)
					iYield += GC.getDefineINT(CvGlobals::EXTRA_YIELD);
			}
		} // </advc.908a>
		if (GET_PLAYER(ePlayer).isGoldenAge())
		{
			if (iYield >= GC.getInfo(eYield).getGoldenAgeYieldThreshold())
				iYield += GC.getInfo(eYield).getGoldenAgeYield();
		}
	}

	iYield = std::max(0, iYield);
	FAssert(iYield <= MAX_CHAR); // advc
	return iYield;
}

// advc.031: Cut from calculateYield
int CvPlot::calculateCityPlotYieldChange(YieldTypes eYield, int iYield,
	int iCityPopulation) const
{
	int const iOldYield = iYield;
	CvYieldInfo const& kYield = GC.getInfo(eYield);
	iYield += kYield.getCityChange();
	if (kYield.getPopulationChangeDivisor() != 0)
	{
		iYield += ((iCityPopulation +
				kYield.getPopulationChangeOffset()) /
				kYield.getPopulationChangeDivisor());
	}
	iYield = std::max(iYield, GC.getInfo(eYield).getMinCity());
	return iYield - iOldYield;
}


void CvPlot::updateYield()
{
	FAssert(area() != NULL); // advc
	bool bChange = false;
	FOR_EACH_ENUM2(Yield, eYield)
	{
		char iNewYield = calculateYield(eYield);
		if (getYield(eYield) == iNewYield)
			continue;

		int iOldYield = getYield(eYield);
		m_aiYield.set(eYield, iNewYield);
		FAssert(getYield(eYield) >= 0);

		CvCity* pWorkingCity = getWorkingCity();
		if (pWorkingCity != NULL)
		{
			if (isBeingWorked())
				pWorkingCity->changeBaseYieldRate(eYield, getYield(eYield) - iOldYield);
			pWorkingCity->AI_setAssignWorkDirty(true);
		}
		bChange = true;
	}

	if (bChange)
		updateSymbols();
}


void CvPlot::updateTeam() // advc.opt: What getTeam used to do
{
	m_eTeam = (isOwned() ? TEAMID(getOwner()) : NO_TEAM);
}


TeamTypes CvPlot::findHighestCultureTeam() const
{
	PlayerTypes eBestPlayer = findHighestCulturePlayer();
	if (eBestPlayer == NO_PLAYER)
		return NO_TEAM;
	return TEAMID(eBestPlayer);
}


PlayerTypes CvPlot::findHighestCulturePlayer(/* advc.035: */ bool bAlive) const
{
	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestValue = 0;
	// advc.099: EVER_ALIVE
	for (PlayerIter<EVER_ALIVE> itPlayer; itPlayer.hasNext(); ++itPlayer)
	{
		if (!bAlive || itPlayer->isAlive()) // advc.035
		{
			int iValue = getCulture(itPlayer->getID());
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPlayer = itPlayer->getID();
			}
		}
	}
	return eBestPlayer;
}


int CvPlot::calculateCulturePercent(PlayerTypes ePlayer) const
{
	int iTotalCulture = getTotalCulture(); // advc.opt: was countTotalCulture
	if (iTotalCulture <= 0)
		return 0;
	return (getCulture(ePlayer) * 100) / iTotalCulture;
}


int CvPlot::calculateTeamCulturePercent(TeamTypes eTeam) const
{
	int iTeamCulturePercent = 0;
	/*  advc.099: Was isAlive. (BtS doesn't call this function, but I'm
		using it for 130w.) */
	for (PlayerIter<EVER_ALIVE,MEMBER_OF> itMember(eTeam);
		itMember.hasNext(); ++itMember)
	{
		iTeamCulturePercent += calculateCulturePercent(itMember->getID());
	}
	return iTeamCulturePercent;
}


void CvPlot::setCulture(PlayerTypes eIndex, int iNewValue, bool bUpdate,
	bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	FAssert(eIndex >= 0);
	FAssert(eIndex < MAX_PLAYERS);

	if(getCulture(eIndex) == iNewValue)
		return;
	 // <advc.opt>
	if(GET_PLAYER(eIndex).isEverAlive())
		m_iTotalCulture += iNewValue - m_aiCulture.get(eIndex); // </advc.opt>
	m_aiCulture.set(eIndex, iNewValue);
	FAssert(getCulture(eIndex) >= 0);

	if(bUpdate)
		updateCulture(true, bUpdatePlotGroups);

	CvCity* pCity = getPlotCity();
	if(pCity != NULL)
		pCity->AI_setAssignWorkDirty(true);
}


void CvPlot::changeCulture(PlayerTypes eIndex, int iChange, bool bUpdate)
{
	if(iChange != 0)
		setCulture(eIndex, getCulture(eIndex) + iChange, bUpdate, true);
}
// < JCultureControl Mod Start >
PlayerTypes CvPlot::getImprovementOwner() const
{
	return (PlayerTypes) m_eImprovementOwner;
}


void CvPlot::setImprovementOwner(PlayerTypes eNewValue)
{
    if (getImprovementOwner() != eNewValue)
    {
        m_eImprovementOwner = eNewValue;
    }
}

int CvPlot::countTotalCultureControl() const
{
	int iTotalCultureControl;
	int iI;

	iTotalCultureControl = 0;

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iTotalCultureControl += getCultureControl((PlayerTypes)iI);
		}
	}

	return iTotalCultureControl;
}


PlayerTypes CvPlot::findHighestCultureControlPlayer() const
{
	if (getImprovementOwner() != NO_PLAYER && getImprovementType() != NO_IMPROVEMENT)
	{
	    if (GC.getDefineINT("CULTURE_CONTROL_IMPROVEMENT_ALWAYS_KEEP_OWNER_BORDER") > 0)
	    {
	        return getImprovementOwner();
	    }
	}

	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestValue = 0;

	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			int iValue = getCultureControl((PlayerTypes)iI);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPlayer = (PlayerTypes)iI;
			}
		}
	}

    if (iBestValue > 0)
    {
        int iCount = 0;
        for (int iI = 0; iI < MAX_PLAYERS; ++iI)
        {
            if (GET_PLAYER((PlayerTypes)iI).isAlive())
            {
                int iValue = getCultureControl((PlayerTypes)iI);

                if (iValue == iBestValue)
                {
                    if (iCount < 1)
                    {
                        iCount += 1;
                    }
                    else
                    {
                        eBestPlayer = NO_PLAYER;
                        break;
                    }
                }
            }
        }
    }

	return eBestPlayer;
}

int CvPlot::calculateCultureControlPercent(PlayerTypes eIndex) const
{
	int iTotalCultureControl;

	iTotalCultureControl = countTotalCultureControl();

	if (iTotalCultureControl > 0)
	{
		return ((getCultureControl(eIndex) * 100) / iTotalCultureControl);
	}

	return 0;
}


int CvPlot::calculateTeamCultureControlPercent(TeamTypes eIndex) const
{
	int iTeamCultureControlPercent;
	int iI;

	iTeamCultureControlPercent = 0;

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == eIndex)
			{
				iTeamCultureControlPercent += calculateCultureControlPercent((PlayerTypes)iI);
			}
		}
	}

	return iTeamCultureControlPercent;
}


void CvPlot::setCultureControl(PlayerTypes eIndex, int iNewValue, bool bUpdate, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	CvCity* pCity;

	FAssertMsg(eIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "iIndex is expected to be within maximum bounds (invalid Index)");

	if (iNewValue >= 0 && getCultureControl(eIndex) != iNewValue)
	{
		//comment out - suggested by f1rpo - after keldath qa2-done
		/*if(NULL == m_aiCultureControl)	
		{
			//m_aiCultureControl = new int[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				//m_aiCultureControl[iI] = 0;
				m_aiCultureControl.set(GET_PLAYER((PlayerTypes)iI,0)
			}
		}
		*/
		//m_aiCultureControl[eIndex] = iNewValue;
		//f1rpo fix due to enummaps
		m_aiCultureControl.set(eIndex, iNewValue);
		FAssert(getCultureControl(eIndex) >= 0);

		if (bUpdate)
		{
			updateCulture(true, bUpdatePlotGroups);
		}

		pCity = getPlotCity();

		if (pCity != NULL)
		{
			pCity->AI_setAssignWorkDirty(true);
		}
	}
}


void CvPlot::changeCultureControl(PlayerTypes eIndex, int iChange, bool bUpdate)
{
	if (iChange != 0)
	{
	    if ((getCultureControl(eIndex) + iChange) >= 0)
	    {
            setCultureControl(eIndex, (getCultureControl(eIndex) + iChange), bUpdate, true);
	    }
	    else
	    {
            setCultureControl(eIndex, 0, bUpdate, true);
	    }
	}
}

void CvPlot::addCultureControl(PlayerTypes ePlayer, ImprovementTypes eImprovement, bool bUpdateInterface)
{
    if (ePlayer != NO_PLAYER && eImprovement != NO_IMPROVEMENT)
    {
        if (GC.getImprovementInfo(eImprovement).isSpreadCultureControl() && GC.getImprovementInfo(eImprovement).getCultureBorderRange() > -1)
        {
            int iRange = GC.getImprovementInfo(eImprovement).getCultureBorderRange();
            int iStrength = GC.getImprovementInfo(eImprovement).getCultureControlStrength();
            int iCenterTileBonus = GC.getImprovementInfo(eImprovement).getCultureControlCenterTileBonus();
            int iDX, iDY;
            CvPlot* pLoopPlot;
            for (iDX = -iRange; iDX <= iRange; iDX++)
            {
                for (iDY = -iRange; iDY <= iRange; iDY++)
                {
                    // This will make it skip the 4 corner Plots
                    if ((GC.getDefineINT("CULTURE_CONTROL_IMPROVEMENT_CULTURE_BORDER_SQUARE") < 1 && iRange > 1) && (iDX == iRange || iDX == -iRange) && (iDY == iRange || iDY == -iRange))
                    {
                        continue;
                    }
                    pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
                    if (pLoopPlot != NULL)
                    {
                        if (iStrength > 0)
                        {
                            pLoopPlot->changeCultureControl(ePlayer, iStrength, bUpdateInterface);
                        }
                        if (iCenterTileBonus > 0 && iDX == 0 && iDY == 0)
                        {
                            pLoopPlot->changeCultureControl(ePlayer, iCenterTileBonus, bUpdateInterface);
                        }
                    }
                }
            }
        }
    }
}

void CvPlot::clearCultureControl(PlayerTypes ePlayer, ImprovementTypes eImprovement, bool bUpdateInterface)
{
    if (ePlayer != NO_PLAYER && eImprovement != NO_IMPROVEMENT)
    {
        if (GC.getImprovementInfo(eImprovement).isSpreadCultureControl() && GC.getImprovementInfo(eImprovement).getCultureBorderRange() > -1)
        {
            int iRange = GC.getImprovementInfo(eImprovement).getCultureBorderRange();
            int iStrength = GC.getImprovementInfo(eImprovement).getCultureControlStrength();
            int iCenterTileBonus = GC.getImprovementInfo(eImprovement).getCultureControlCenterTileBonus();
            int iDX, iDY;
            CvPlot* pLoopPlot;
            for (iDX = -iRange; iDX <= iRange; iDX++)
            {
                for (iDY = -iRange; iDY <= iRange; iDY++)
                {
                    // This will make it skip the 4 corner Plots
                    if ((GC.getDefineINT("CULTURE_CONTROL_IMPROVEMENT_CULTURE_BORDER_SQUARE") < 1 && iRange > 1) && (iDX == iRange || iDX == -iRange) && (iDY == iRange || iDY == -iRange))
                    {
                        continue;
                    }
                    pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
                    if (pLoopPlot != NULL)
                    {
                        if (iStrength > 0)
                        {
                            pLoopPlot->changeCultureControl(ePlayer, -iStrength, bUpdateInterface);
                        }
                        if (iCenterTileBonus > 0 && iDX == 0 && iDY == 0)
                        {
                            pLoopPlot->changeCultureControl(ePlayer, -iCenterTileBonus, bUpdateInterface);
                        }
                    }
                }
            }
        }
    }
}

void CvPlot::updateCultureControl(int iCenterX, int iCenterY, int iUpdateRange, bool bUpdateInterface)
{
    /*CvPlot* pCenterPlot = plotXY(iCenterX, iCenterY, 0, 0);
    if (pCenterPlot != NULL)
    {
        int iRange;
        if (iUpdateRange > 0)
        {
            iRange = iUpdateRange;
        }
        else if (GC.getDefineINT("CULTURE_CONTROL_IMPROVEMENT_REMOVED_DEFAULT_UPDATE_RANGE") > 0)
        {
            iRange = GC.getDefineINT("CULTURE_CONTROL_IMPROVEMENT_REMOVED_DEFAULT_UPDATE_RANGE");
        }
        else
        {
            iRange = 10;
        }
        if (iRange > 0)
        {
            int iDX, iDY;
            CvPlot* pLoopPlot;
            for (iDX = -iRange; iDX <= iRange; iDX++)
            {
                for (iDY = -iRange; iDY <= iRange; iDY++)
                {
                    pLoopPlot = plotXY(iCenterX, iCenterY, iDX, iDY);
                    if (pLoopPlot != NULL)
                    {
                        if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT && pLoopPlot->getCultureControlOwner() != NO_PLAYER && pLoopPlot->getCultureControlX() == pLoopPlot->getX() && pLoopPlot->getCultureControlY() == pLoopPlot->getY())
                        {
                            pLoopPlot->addCultureControl(pLoopPlot->getCultureControlOwner(), pLoopPlot->getX(), pLoopPlot->getY(), pLoopPlot->getImprovementType(), bUpdateInterface);
                        }
                    }
                }
            }
        }
    }*/
}
// < JCultureControl Mod End >

int CvPlot::getFoundValue(PlayerTypes eIndex, /* advc.052: */ bool bRandomize) const
{
	FAssertBounds(0, MAX_PLAYERS, eIndex);

	if (m_aiFoundValue.get(eIndex) == -1)
	{
		short iValue = GC.getPythonCaller()->AI_foundValue(eIndex, *this);
		if (iValue == -1)
			m_aiFoundValue.set(eIndex, GET_PLAYER(eIndex).AI_foundValue(getX(), getY(), -1, true));
		if (m_aiFoundValue.get(eIndex) > getArea().getBestFoundValue(eIndex))
			getArea().setBestFoundValue(eIndex, m_aiFoundValue.get(eIndex));
	}
	//return m_aiFoundValue[eIndex];
	// <advc.052>
	int iResult = m_aiFoundValue.get(eIndex);
	if(bRandomize && !GET_PLAYER(eIndex).isHuman() && GC.getGame().isScenario())
	{	// Randomly change the value by +/- 1.5%
		scaled const rPlusMinus = per1000(15);
		std::vector<int> aiHashInput;
		/*  Base the random multiplier on a number that is unique
			per game, but doesn't change throughout a game. */
		aiHashInput.push_back(GC.getGame().getInitialRandSeed().first);
		aiHashInput.push_back(getX());
		aiHashInput.push_back(getY());
		aiHashInput.push_back(eIndex);
		scaled rRandMult = 1 - rPlusMinus + 2 * rPlusMinus * scaled::hash(aiHashInput);
		iResult = (iResult * rRandMult).round();
	}
	return iResult;
	// </advc.052>
}


bool CvPlot::isBestAdjacentFound(PlayerTypes eIndex) /* advc: */ const
{
	CitySiteEvaluator citySiteEval(GET_PLAYER(eIndex));
	int iPlotValue = citySiteEval.evaluate(*this);
	if (iPlotValue == 0)
		return false;

	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isRevealed(TEAMID(eIndex)))
		{
			//if (pAdjacentPlot->getFoundValue(eIndex) >= getFoundValue(eIndex))
			if (citySiteEval.evaluate(*pAdj) > iPlotValue)
				return false;
		}
	}
	return true;
}


void CvPlot::setFoundValue(PlayerTypes eIndex, short iNewValue)
{
	m_aiFoundValue.set(eIndex, iNewValue);
}

// advc: Cut from CvPlayer::canFound (for advc.027)
bool CvPlot::canFound(bool bTestVisible,
	TeamTypes eTeam) const // advc.181
{
	if (!canEverFound()) // advc.129d: Moved into another new function
		return false;
	/*	<advc.181> Info leaks and AI cheats are already addressed elsewhere,
		but doesn't hurt to make sure. */
	if (eTeam != NO_TEAM && !isRevealed(eTeam))
		return false; // </advc.181>
	
	if (isFeature() && GC.getInfo(getFeatureType()).isNoCity())
		return false; // (advc.opt: Moved down)

	if (bTestVisible)
		return true;

	for (SquareIter it(*this, GC.getDefineINT(CvGlobals::MIN_CITY_RANGE));
		it.hasNext(); ++it)
	{
		if (it->isCity() && it->sameArea(*this) &&
			(eTeam == NO_TEAM || it->getPlotCity()->isRevealed(eTeam))) // advc.181
		{
			return false;
		}
	}

	return true;
}

/*	advc.129d: Cut from CvPlayer::canFound.
	Maybe not never ever; e.g. impassable status could change in mods. */
bool CvPlot::canEverFound() const
{
	if (isImpassable())
		return false;
	// advc.opt: Water check moved up
	/*  UNOFFICIAL_PATCH, Bugfix, 02/16/10, EmperorFool & jdog5000:
		(canFoundCitiesOnWater callback handling was incorrect and ignored isWater() if it returned true) */
	if (isWater())
	{
		if (GC.getPythonCaller()->canFoundWaterCity(*this))
		{
			FErrorMsg("The AdvCiv mod probably does not support cities on water"); // advc
			return true;
		}
		return false; // advc.opt
	}
	CvTerrainInfo const& kTerrain = GC.getInfo(getTerrainType());
	if (kTerrain.isFound())
		return true;
	if (kTerrain.isFoundCoast() && isCoastalLand())
		return true;
	if (kTerrain.isFoundFreshWater() && isFreshWater())
		return true;
	return false;
}


void CvPlot::changePlayerCityRadiusCount(PlayerTypes eIndex, int iChange)
{
	m_aiPlayerCityRadiusCount.add(eIndex, iChange);
	FAssert(getPlayerCityRadiusCount(eIndex) >= 0);
}


CvPlotGroup* CvPlot::getPlotGroup(PlayerTypes ePlayer) const
{
	//PROFILE_FUNC();
	/*  advc.003o: Inlining would require CvPlayer.h to be included in CvPlot.h.
		Instead, I've redirected some calls to a new inline function isSamePlotGroup. */
	return GET_PLAYER(ePlayer).getPlotGroup(m_aiPlotGroup.get(ePlayer));
}


CvPlotGroup* CvPlot::getOwnerPlotGroup() const
{
	if (getOwner() == NO_PLAYER)
		return NULL;
	return getPlotGroup(getOwner());
}


void CvPlot::setPlotGroup(PlayerTypes ePlayer, CvPlotGroup* pNewValue,
	bool bVerifyProduction) // advc.064d
{
	CvPlotGroup* pOldPlotGroup = getPlotGroup(ePlayer);
	if (pOldPlotGroup == pNewValue)
		return;

	CvCity* pCity = getPlotCity();
	if (ePlayer == getOwner())
		updatePlotGroupBonus(false, /* advc.064d: */ false);

	if (pOldPlotGroup != NULL && pCity != NULL && pCity->getOwner() == ePlayer)
	{
		FOR_EACH_ENUM(Bonus)
		{
			pCity->changeNumBonuses(eLoopBonus, -pOldPlotGroup->getNumBonuses(eLoopBonus),
					false); // advc.064d (handled by CvPlotGroup::recalculatePlots ... I hope)
		}
		// < Building Resource Converter Start >
		pCity->processBuildingBonuses();
		// < Building Resource Converter End   >
	}

	if (pNewValue == NULL)
		m_aiPlotGroup.set(ePlayer, FFreeList::INVALID_INDEX);
	else m_aiPlotGroup.set(ePlayer, pNewValue->getID());

	if (getPlotGroup(ePlayer) != NULL && pCity != NULL && pCity->getOwner() == ePlayer)
	{
		FOR_EACH_ENUM(Bonus)
			pCity->changeNumBonuses(eLoopBonus, getPlotGroup(ePlayer)->getNumBonuses(eLoopBonus));
		// < Building Resource Converter Start >
		pCity->processBuildingBonuses();
		// < Building Resource Converter End   >
	}

	if (ePlayer == getOwner())
		updatePlotGroupBonus(true, /* advc.064d: */ bVerifyProduction);
}


void CvPlot::updatePlotGroup(/* advc.064d: */ bool bVerifyProduction)
{
	PROFILE_FUNC();
	for (PlayerIter<ALIVE> it; it.hasNext(); ++it)
	{
		CvPlayer& kPlayer = *it;
		updatePlotGroup(kPlayer.getID(), /* <advc.064d> */ true, false);
		/*  When recalculation of plot groups starts with updatePlotGroup, then
			bVerifyProduction sometimes interrupts city production prematurely;
			not sure why exactly. Will have to verify all cities instead. */
		if (bVerifyProduction && kPlayer.isHuman() && getOwner() == kPlayer.getID())
			kPlayer.verifyCityProduction(); // </advc.064d>
	}
}


void CvPlot::updatePlotGroup(PlayerTypes ePlayer, bool bRecalculate,
	bool bVerifyProduction) // advc.064d
{
	if (!GC.getGame().isFinalInitialized())
		return;

	TeamTypes const eTeam = TEAMID(ePlayer);

	CvPlotGroup* pPlotGroup = getPlotGroup(ePlayer);
	if (pPlotGroup != NULL)
	{
		if (bRecalculate)
		{
			bool bConnected = false;
			if (isTradeNetwork(eTeam))
			{
				bConnected = true;
				FOR_EACH_ADJ_PLOT(*this)
				{
					if (pAdj->getPlotGroup(ePlayer) == pPlotGroup &&
						!isTradeNetworkConnected(*pAdj, eTeam))
					{
						bConnected = false;
						break;
					}
				}
			}
			if (!bConnected)
			{
				bool bEmpty = (pPlotGroup->getLengthPlots() == 1);
				FAssert(pPlotGroup->getLengthPlots() > 0);

				pPlotGroup->removePlot(this, /* advc.064d: */ bVerifyProduction);
				if (!bEmpty)
					pPlotGroup->recalculatePlots(/* advc.064d: */ bVerifyProduction);
			}
		}
		pPlotGroup = getPlotGroup(ePlayer);
	}

	if (!isTradeNetwork(eTeam))
		return;

	CvMap& kMap = GC.getMap();
	FOR_EACH_ADJ_PLOT(*this)
	{
		CvPlotGroup* pAdjPlotGroup = pAdj->getPlotGroup(ePlayer);
		if (pAdjPlotGroup != NULL && pAdjPlotGroup != pPlotGroup &&
			isTradeNetworkConnected(*pAdj, eTeam))
		{
			if (pPlotGroup == NULL)
			{
				pAdjPlotGroup->addPlot(this, /* advc.064d: */ bVerifyProduction);
				pPlotGroup = pAdjPlotGroup;
				FAssert(getPlotGroup(ePlayer) == pPlotGroup);
			}
			else
			{
				FAssert(getPlotGroup(ePlayer) == pPlotGroup);
				kMap.combinePlotGroups(ePlayer, pPlotGroup, pAdjPlotGroup,
						bVerifyProduction); // advc.064d
				pPlotGroup = getPlotGroup(ePlayer);
				FAssert(pPlotGroup != NULL);
			}
		}
	}
	if (pPlotGroup == NULL)
		GET_PLAYER(ePlayer).initPlotGroup(this);
}


void CvPlot::changeVisibilityCount(TeamTypes eTeam, int iChange,
	InvisibleTypes eSeeInvisible, bool bUpdatePlotGroups,
	CvUnit const* pUnit) // advc.071
{
	if(iChange == 0)
		return;

	bool const bOldVisible = isVisible(eTeam);

	m_aiVisibilityCount.add(eTeam, iChange);
	//FAssert(getVisibilityCount(eTeam) >= 0);
	/*  <advc.006> Had some problems here with the Earth1000AD scenario as the
		initial cities were being placed and over the first few turns.
		The problems remain unresolved. */
	/*	advc.001: Also works around a problem with nukeExplosion replacing
		a sight-blocking feature with fallout. */
	if(getVisibilityCount(eTeam) < 0)
	{
//super forts - doto - assert ignore
		if (!GC.getGame().isOption(GAMEOPTION_SUPER_FORTS))
			FAssert(m_aiVisibilityCount.get(eTeam) >= 0);
		m_aiVisibilityCount.set(eTeam, 0);
	} // </advc.006>

	if (eSeeInvisible != NO_INVISIBLE)
		changeInvisibleVisibilityCount(eTeam, eSeeInvisible, iChange);

	if (bOldVisible == isVisible(eTeam))
		return;

	if (isVisible(eTeam))
	{
		setRevealed(eTeam, true, false, NO_TEAM, bUpdatePlotGroups);
		FOR_EACH_ADJ_PLOT_VAR(*this)
		{
			//pAdj->updateRevealedOwner(eTeam);
			/*  K-Mod: updateRevealedOwner simply checks for a visible adj. plot.
				But we've already checked that, so lets go right to the punch. */
			pAdj->setRevealedOwner(eTeam, pAdj->getOwner());
		}

		if (getTeam() != NO_TEAM)
		{	// advc.071:
			FirstContactData fcData(this, pUnit == NULL ? NULL : pUnit->plot(), pUnit);
			GET_TEAM(getTeam()).meet(eTeam, true, /* advc.071: */ &fcData);
		}
		// K-Mod. Meet the owner of any units you can see.
		{
			PROFILE("CvPlot::changeVisibility -- meet units"); // (this is new, so I want to time it.)
			FOR_EACH_UNIT_IN(pLoopUnit, *this)
			{	// <advc.opt> No problem here according to the profiler, but still:
				if (GET_TEAM(eTeam).isHasMet(TEAMID(pLoopUnit->getOwner()))) // (all inlined)
					continue; // </advc.opt>
				// advc.071:
				FirstContactData fcData(this, pUnit == NULL ? NULL : pUnit->plot(), pUnit, pLoopUnit);
				GET_TEAM(eTeam).meet(TEAMID(pLoopUnit->getVisualOwner(eTeam)), true,
						&fcData); // advc.071
			}
		} // K-Mod end
	}

	CvCity* pCity = getPlotCity();
	if (pCity != NULL)
		pCity->setInfoDirty(true);

	for (TeamIter<ALIVE> itTeam; itTeam.hasNext(); ++itTeam)
	{
		if (itTeam->isStolenVisibility(eTeam))
			changeStolenVisibilityCount(itTeam->getID(), isVisible(eTeam) ? 1 : -1);
	}

	if (eTeam == getActiveTeam())
	{
		updateFog();
		updateMinimapColor();
		updateCenterUnit();
	}
}


void CvPlot::changeStolenVisibilityCount(TeamTypes eTeam, int iChange)
{
	if(iChange == 0)
		return;

	bool bOldVisible = isVisible(eTeam);

	m_aiStolenVisibilityCount.add(eTeam, iChange);
	FAssert(getStolenVisibilityCount(eTeam) >= 0);

	if (bOldVisible != isVisible(eTeam))
	{
		if (isVisible(eTeam))
			setRevealed(eTeam, true, false, NO_TEAM, true);

		CvCity* pCity = getPlotCity();
		if (pCity != NULL)
			pCity->setInfoDirty(true);

		if (eTeam == getActiveTeam())
		{
			updateFog();
			updateMinimapColor();
			updateCenterUnit();
		}
	}
}


void CvPlot::changeBlockadedCount(TeamTypes eTeam, int iChange)
{
	if (iChange == 0)
		return;

	m_aiBlockadedCount.add(eTeam, iChange);
	// BETTER_BTS_AI_MOD, Bugfix, 06/01/09, jdog5000: START
	// Hack so that never get negative blockade counts as a result of fixing issue causing
	// rare permanent blockades.
	if (getBlockadedCount(eTeam) < 0)
	{
		FAssert(isWater());
		m_aiBlockadedCount.set(eTeam, 0);
	}
	// BETTER_BTS_AI_MOD: END
	CvCity* pWorkingCity = getWorkingCity();
	if (pWorkingCity != NULL)
		pWorkingCity->AI_setAssignWorkDirty(true);
}


PlayerTypes CvPlot::getRevealedOwner(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
		return getOwner();

	return getRevealedOwner(eTeam); // advc.inl: Call inline version
}


TeamTypes CvPlot::getRevealedTeam(TeamTypes eTeam, bool bDebug) const
{
	PlayerTypes eRevealedOwner = getRevealedOwner(eTeam, bDebug);
	if (eRevealedOwner != NO_PLAYER)
		return TEAMID(eRevealedOwner);
	return NO_TEAM;
}


void CvPlot::setRevealedOwner(TeamTypes eTeam, PlayerTypes eNewValue)
{
	if (getRevealedOwner(eTeam) == eNewValue)
		return;

	m_aiRevealedOwner.set(eTeam, eNewValue);
	// K-Mod
	if (eNewValue != NO_PLAYER)
	{
		GET_TEAM(eTeam).makeHasSeen(TEAMID(eNewValue)); // K-Mod end
		// <advc.001> Goody can't exist on owned tile
		ImprovementTypes eRevImprov = getRevealedImprovementType(eTeam);
		if (eRevImprov != NO_IMPROVEMENT && GC.getInfo(eRevImprov).isGoody())
			setRevealedImprovementType(eTeam, NO_IMPROVEMENT); // </advc.001>
	}
	if (eTeam == getActiveTeam())
	{
		updateMinimapColor();
		if (GC.IsGraphicsInitialized())
		{
			gDLL->UI().setDirty(GlobeLayer_DIRTY_BIT, true);
			gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
		}
	}
}


void CvPlot::updateRevealedOwner(TeamTypes eTeam)
{
	bool bRevealed = false;
	if (isVisible(eTeam))
		bRevealed = true;
	if (!bRevealed)
	{
		FOR_EACH_ADJ_PLOT(*this)
		{
			if (pAdj->isVisible(eTeam))
			{
				bRevealed = true;
				break;
			}
		}
	}
	if (bRevealed)
		setRevealedOwner(eTeam, getOwner());
}


void CvPlot::updateRiverCrossing(DirectionTypes eDirection)
{
	FAssertEnumBounds(eDirection);

	CvPlot* pCornerPlot = NULL;
	bool bValid = false;
	CvPlot* pPlot = plotDirection(getX(), getY(), eDirection);
	if ((pPlot == NULL || !pPlot->isWater()) && !isWater())
	{
		switch (eDirection)
		{
		case DIRECTION_NORTH:
			if (pPlot != NULL)
				bValid = pPlot->isNOfRiver();
			break;

		case DIRECTION_NORTHEAST:
			pCornerPlot = plotDirection(getX(), getY(), DIRECTION_NORTH);
			break;

		case DIRECTION_EAST:
			bValid = isWOfRiver();
			break;

		case DIRECTION_SOUTHEAST:
			pCornerPlot = this;
			break;

		case DIRECTION_SOUTH:
			bValid = isNOfRiver();
			break;

		case DIRECTION_SOUTHWEST:
			pCornerPlot = plotDirection(getX(), getY(), DIRECTION_WEST);
			break;

		case DIRECTION_WEST:
			if (pPlot != NULL)
				bValid = pPlot->isWOfRiver();
			break;

		case DIRECTION_NORTHWEST:
			pCornerPlot = plotDirection(getX(), getY(), DIRECTION_NORTHWEST);
			break;

		default:
			FAssert(false);
			break;
		}

		if (pCornerPlot != NULL)
		{
			CvPlot* pNorthEastPlot = plotDirection(pCornerPlot->getX(), pCornerPlot->getY(),
					DIRECTION_EAST);
			CvPlot* pSouthEastPlot = plotDirection(pCornerPlot->getX(), pCornerPlot->getY(),
					DIRECTION_SOUTHEAST);
			CvPlot* pSouthWestPlot = plotDirection(pCornerPlot->getX(), pCornerPlot->getY(),
					DIRECTION_SOUTH);
			CvPlot* pNorthWestPlot = pCornerPlot;

			if (pSouthWestPlot && pNorthWestPlot && pSouthEastPlot && pNorthEastPlot)
			{
				if (pSouthWestPlot->isWOfRiver() && pNorthWestPlot->isWOfRiver())
					bValid = true;
				else if (pNorthEastPlot->isNOfRiver() && pNorthWestPlot->isNOfRiver())
					bValid = true;
				else if (eDirection == DIRECTION_NORTHEAST || eDirection == DIRECTION_SOUTHWEST)
				{
					if (pNorthEastPlot->isNOfRiver() && (pNorthWestPlot->isWOfRiver() || pNorthWestPlot->isWater()))
						bValid = true;
					else if ((pNorthEastPlot->isNOfRiver() || pSouthEastPlot->isWater()) && pNorthWestPlot->isWOfRiver())
						bValid = true;
					else if (pSouthWestPlot->isWOfRiver() && (pNorthWestPlot->isNOfRiver() || pNorthWestPlot->isWater()))
						bValid = true;
					else if ((pSouthWestPlot->isWOfRiver() || pSouthEastPlot->isWater()) && pNorthWestPlot->isNOfRiver())
						bValid = true;
				}
				else
				{
					FAssert(eDirection == DIRECTION_SOUTHEAST || eDirection == DIRECTION_NORTHWEST);

					if (pNorthWestPlot->isNOfRiver() && (pNorthWestPlot->isWOfRiver() || pNorthEastPlot->isWater()))
						bValid = true;
					else if ((pNorthWestPlot->isNOfRiver() || pSouthWestPlot->isWater()) && pNorthWestPlot->isWOfRiver())
						bValid = true;
					else if (pNorthEastPlot->isNOfRiver() && (pSouthWestPlot->isWOfRiver() || pSouthWestPlot->isWater()))
						bValid = true;
					else if ((pNorthEastPlot->isNOfRiver() || pNorthEastPlot->isWater()) && pSouthWestPlot->isWOfRiver())
						bValid = true;
				}
			}

		}
	}

	if (isRiverCrossing(eDirection) != bValid)
	{
		m_abRiverCrossing.set(eDirection, bValid);
		changeRiverCrossingCount(isRiverCrossing(eDirection) ? 1 : -1);
	}
}


void CvPlot::updateRiverCrossing()
{
	FOR_EACH_ENUM(Direction)
		updateRiverCrossing(eLoopDirection);
}


bool CvPlot::isRevealed(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
		return true;
	return isRevealed(eTeam); // advc.inl: Call inline version
}


void CvPlot::setRevealed(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly, TeamTypes eFromTeam, bool bUpdatePlotGroup)
{
	FAssertBounds(0, MAX_TEAMS, eTeam);

	CvCity* pCity = getPlotCity();
	bool bOldValue = isRevealed(eTeam); // advc.124
	if (bOldValue != bNewValue)
	{
		m_abRevealed.set(eTeam, bNewValue);
		getArea().changeNumRevealedTiles(eTeam, isRevealed(eTeam) ? 1 : -1);
	}  // <advc.124> Need to update plot group if any revealed info changes
	if (bUpdatePlotGroup &&
		(bOldValue != bNewValue ||
		getRevealedOwner(eTeam) != getOwner() ||
		getRevealedImprovementType(eTeam) != getImprovementType() ||
		getRevealedRouteType(eTeam) != getRouteType() ||
		(pCity != NULL && !pCity->isRevealed(eTeam)))) // </advc.124>
	{
		for (MemberIter itMember(eTeam); itMember.hasNext(); ++itMember)
		{
			updatePlotGroup(itMember->getID());
		}
	}
	if (bOldValue != bNewValue) // advc.124
	{
		if (eTeam == getActiveTeam())
		{
			updateSymbols();
			updateFog();
			updateVisibility();

			gDLL->UI().setDirty(MinimapSection_DIRTY_BIT, true);
			gDLL->UI().setDirty(GlobeLayer_DIRTY_BIT, true);
		}

		if (isRevealed(eTeam, false))
		{	// ONEVENT - PlotRevealed
			CvEventReporter::getInstance().plotRevealed(this, eTeam);
		}
	}

	if (bTerrainOnly)
		return;


	if (!isRevealed(eTeam))
	{
		setRevealedOwner(eTeam, NO_PLAYER);
		setRevealedImprovementType(eTeam, NO_IMPROVEMENT);
		setRevealedRouteType(eTeam, NO_ROUTE);

		if (pCity != NULL)
			pCity->setRevealed(eTeam, false);
		return; // advc
	}

	if (eFromTeam == NO_TEAM)
	{
		setRevealedOwner(eTeam, getOwner());
		setRevealedImprovementType(eTeam, getImprovementType());
		setRevealedRouteType(eTeam, getRouteType());

		if (pCity != NULL)
			pCity->setRevealed(eTeam, true);
	}
	else
	{
		if (getRevealedOwner(eFromTeam) == getOwner())
			setRevealedOwner(eTeam, getRevealedOwner(eFromTeam));

		if (getRevealedImprovementType(eFromTeam) == getImprovementType())
			setRevealedImprovementType(eTeam, getRevealedImprovementType(eFromTeam));

		if (getRevealedRouteType(eFromTeam) == getRouteType())
			setRevealedRouteType(eTeam, getRevealedRouteType(eFromTeam));

		if (pCity != NULL && pCity->isRevealed(eFromTeam))
			pCity->setRevealed(eTeam, true);
	}
}


bool CvPlot::isAdjacentRevealed(TeamTypes eTeam,
	bool bSkipOcean) const // advc.205c
{
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (pAdj->isRevealed(eTeam) /* <advc.250c> */ && (!bSkipOcean ||
			pAdj->getTerrainType() != GC.getWATER_TERRAIN(false))) // </advc.250c>
		{
			return true;
		}
	}
	return false;
}


bool CvPlot::isAdjacentNonrevealed(TeamTypes eTeam) const
{
	FOR_EACH_ADJ_PLOT(*this)
	{
		if (!pAdj->isRevealed(eTeam))
			return true;
	}
	return false;
}


ImprovementTypes CvPlot::getRevealedImprovementType(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
		return getImprovementType();

	return getRevealedImprovementType(eTeam); // advc.inl: Call inline version
}


void CvPlot::setRevealedImprovementType(TeamTypes eTeam, ImprovementTypes eNewValue)
{
	if (getRevealedImprovementType(eTeam) == eNewValue)
		return;

	m_aeRevealedImprovementType.set(eTeam, eNewValue);

	if (eTeam == getActiveTeam())
	{
		updateSymbols();
		setLayoutDirty(true);
		//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
	}
}


RouteTypes CvPlot::getRevealedRouteType(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
		return getRouteType();

	return getRevealedRouteType(eTeam); // advc.inl: Call inline version
}


void CvPlot::setRevealedRouteType(TeamTypes eTeam, RouteTypes eNewValue)
{
	FAssertBounds(0, MAX_TEAMS, eTeam);

	if (getRevealedRouteType(eTeam) == eNewValue)
		return;

	m_aeRevealedRouteType.set(eTeam, eNewValue);

	if (eTeam == getActiveTeam())
	{
		updateSymbols();
		updateRouteSymbol(true, true);
	}
}

// Returns true if build finished...
bool CvPlot::changeBuildProgress(BuildTypes eBuild, int iChange,
	/*TeamTypes eTeam*/ /* advc.251: */ PlayerTypes ePlayer)
{
	CvWString szBuffer;

	if(iChange == 0)
		return false;

	TeamTypes eTeam = TEAMID(ePlayer); // advc.251

	m_aiBuildProgress.add(eBuild, iChange);
	FAssert(getBuildProgress(eBuild) >= 0);

	/*  advc.011: Meaning that the interruption timer is reset,
		and starts counting again next turn. */
	m_iTurnsBuildsInterrupted = -1;

	if(getBuildProgress(eBuild) < getBuildTime(eBuild, /* advc.251: */ ePlayer))
		return false;

	m_aiBuildProgress.set(eBuild, 0);
	CvBuildInfo const& kBuild = GC.getInfo(eBuild);

	if (kBuild.getImprovement() != NO_IMPROVEMENT)
		setImprovementType(kBuild.getImprovement());

	if (kBuild.getRoute() != NO_ROUTE)
		setRouteType(kBuild.getRoute(), true);

	if (isFeature() && kBuild.isFeatureRemove(getFeatureType()))
	{
		FAssert(eTeam != NO_TEAM);
		CvCity* pCity;
		int iProduction = getFeatureProduction(eBuild, eTeam, &pCity);
		if (iProduction > 0)
		{
			pCity->changeFeatureProduction(iProduction);

			szBuffer = gDLL->getText("TXT_KEY_MISC_CLEARING_FEATURE_BONUS",
					GC.getInfo(getFeatureType()).getTextKeyWide(),
					iProduction, pCity->getNameKey());
			gDLL->UI().addMessage(pCity->getOwner(), false, -1, szBuffer, *this,
					ARTFILEMGR.getInterfaceArtPath("WORLDBUILDER_CITY_EDIT"),
					MESSAGE_TYPE_INFO, GC.getInfo(getFeatureType()).getButton(), NO_COLOR);
		}
		// Python Event
		CvEventReporter::getInstance().plotFeatureRemoved(this, getFeatureType(), pCity);

		setFeatureType(NO_FEATURE);
	}

	return true;
}

// <advc.011>
bool CvPlot::isBuildProgressDecaying(bool bWarn) const
{
	if (m_iTurnsBuildsInterrupted <= NO_BUILD_IN_PROGRESS)
		return false;
	int const iDelay = GC.getDefineINT(CvGlobals::DELAY_UNTIL_BUILD_DECAY);
	if (m_iTurnsBuildsInterrupted > NO_BUILD_IN_PROGRESS &&
		m_iTurnsBuildsInterrupted + (bWarn ? 1 : 0) < iDelay)
	{
		return false;
	}
	FOR_EACH_ENUM(Build)
	{
		if (m_aiBuildProgress.get(eLoopBuild) > 0)
			return true;
	}
	return false;
}


void CvPlot::decayBuildProgress()
{
	if (m_iTurnsBuildsInterrupted > NO_BUILD_IN_PROGRESS &&
		m_iTurnsBuildsInterrupted < GC.getDefineINT(CvGlobals::DELAY_UNTIL_BUILD_DECAY))
	{
		m_iTurnsBuildsInterrupted++;
	}
	if (!isBuildProgressDecaying())
		return;
	bool bAnyInProgress = false;
	FOR_EACH_ENUM(Build)
	{
		if (m_aiBuildProgress.get(eLoopBuild) > 0)
		{
			m_aiBuildProgress.add(eLoopBuild, -1);
			bAnyInProgress = true;
		}
	}
	// Explicitly suspend decay (just for better performance)
	if (!bAnyInProgress)
		m_iTurnsBuildsInterrupted = NO_BUILD_IN_PROGRESS;
} // </advc.011>


void CvPlot::updateFeatureSymbolVisibility()
{
	//PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
		return;

	if (m_pFeatureSymbol == NULL)
		return;

	bool bVisible = isRevealed(getActiveTeam(), true);
	if (isFeature())
	{
		if(GC.getInfo(getFeatureType()).isVisibleAlways())
			bVisible = true;
	}

	bool bWasVisible = !gDLL->getFeatureIFace()->IsHidden(m_pFeatureSymbol);
	if (bWasVisible != bVisible)
	{
		gDLL->getFeatureIFace()->Hide(m_pFeatureSymbol, !bVisible);
		gDLL->getEngineIFace()->MarkPlotTextureAsDirty(getX(), getY());
	}
}


void CvPlot::updateFeatureSymbol(bool bForce)
{
	//PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
		return;

	gDLL->getEngineIFace()->RebuildTileArt(getX(), getY());

	FeatureTypes eFeature = getFeatureType();
	if (eFeature == NO_FEATURE ||
		GC.getInfo(eFeature).getArtInfo()->isRiverArt() ||
		GC.getInfo(eFeature).getArtInfo()->getTileArtType() != TILE_ART_TYPE_NONE)
	{
		gDLL->getFeatureIFace()->destroy(m_pFeatureSymbol);
		return;
	}

	if (bForce || m_pFeatureSymbol == NULL ||
		gDLL->getFeatureIFace()->getFeature(m_pFeatureSymbol) != eFeature)
	{
		gDLL->getFeatureIFace()->destroy(m_pFeatureSymbol);
		m_pFeatureSymbol = gDLL->getFeatureIFace()->createFeature();
		FAssert(m_pFeatureSymbol != NULL);
		gDLL->getFeatureIFace()->init(m_pFeatureSymbol, 0, 0, eFeature, this);
		updateFeatureSymbolVisibility();
	} //update position and contours:
	else gDLL->getFeatureIFace()->updatePosition(m_pFeatureSymbol);
}


CvRoute* CvPlot::getRouteSymbol() const
{
	return m_pRouteSymbol;
}

/*	XXX route symbols don't really exist anymore... advc (comment): I think this just means
	that this function and m_pRouteSymbol should be renamed. */
void CvPlot::updateRouteSymbol(bool bForce, bool bAdjacent)
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
		return;

	if (bAdjacent)
	{
		FOR_EACH_ADJ_PLOT_VAR(*this)
		{
			pAdj->updateRouteSymbol(bForce, false);
			//pAdj->setLayoutDirty(true);
		}
	}

	RouteTypes eRoute = getRevealedRouteType(getActiveTeam(), true);
	if (eRoute == NO_ROUTE)
	{
		gDLL->getRouteIFace()->destroy(m_pRouteSymbol);
		return;
	}

	if (bForce || m_pRouteSymbol == NULL ||
		gDLL->getRouteIFace()->getRoute(m_pRouteSymbol) != eRoute)
	{
		PROFILE("updateRouteSymbol.RouteIFace::destroy/init");
		gDLL->getRouteIFace()->destroy(m_pRouteSymbol);
		m_pRouteSymbol = gDLL->getRouteIFace()->createRoute();
		FAssert(m_pRouteSymbol != NULL);
		gDLL->getRouteIFace()->init(m_pRouteSymbol, 0, 0, eRoute, this);
		setLayoutDirty(true);
	} //update position and contours:
	else gDLL->getRouteIFace()->updatePosition(m_pRouteSymbol);
}


CvRiver* CvPlot::getRiverSymbol() const
{
	return m_pRiverSymbol;
}


CvFeature* CvPlot::getFeatureSymbol() const
{
	return m_pFeatureSymbol;
}


void CvPlot::updateRiverSymbol(bool bForce, bool bAdjacent)
{
	if (!GC.IsGraphicsInitialized())
		return;

	if (bAdjacent)
	{
		FOR_EACH_ADJ_PLOT_VAR(*this)
		{
			pAdj->updateRiverSymbol(bForce, false);
			//pAdj->setLayoutDirty(true);
		}
	}

	if (!isRiverMask())
	{
		gDLL->getRiverIFace()->destroy(m_pRiverSymbol);
		return;
	}

	if (bForce || m_pRiverSymbol == NULL)
	{
		// create river
		gDLL->getRiverIFace()->destroy(m_pRiverSymbol);
		m_pRiverSymbol = gDLL->getRiverIFace()->createRiver();
		FAssert(m_pRiverSymbol != NULL);
		gDLL->getRiverIFace()->init(m_pRiverSymbol, 0, 0, 0, this);

		// force tree cuts for adjacent plots
		DirectionTypes affectedDirections[] =
		{
			NO_DIRECTION, DIRECTION_EAST, DIRECTION_SOUTHEAST, DIRECTION_SOUTH
		};
		for (int i = 0; i < 4; i++)
		{
			CvPlot const* pAdj = plotDirection(getX(), getY(), affectedDirections[i]);
			if (pAdj != NULL)
				gDLL->getEngineIFace()->ForceTreeOffsets(pAdj->getX(), pAdj->getY());
		}

		// cut out canyons
		gDLL->getEngineIFace()->RebuildRiverPlotTile(getX(), getY(), true, false);

		// recontour adjacent rivers
		FOR_EACH_ADJ_PLOT(*this)
		{
			if(pAdj->m_pRiverSymbol != NULL)
			{	// update position and contours
				gDLL->getRiverIFace()->updatePosition(pAdj->m_pRiverSymbol);
			}
		}
		// update the symbol
		setLayoutDirty(true);
	}
	// recontour rivers - update position and contours
	gDLL->getRiverIFace()->updatePosition(m_pRiverSymbol);
}


void CvPlot::updateRiverSymbolArt(bool bAdjacent)
{
	// this is used to update floodplain features
	gDLL->getEntityIFace()->setupFloodPlains(m_pRiverSymbol);
	if (bAdjacent)
	{
		FOR_EACH_ADJ_PLOT(*this)
		{
			if (pAdj->m_pRiverSymbol != NULL)
				gDLL->getEntityIFace()->setupFloodPlains(pAdj->m_pRiverSymbol);
		}
	}
}


CvFlagEntity* CvPlot::getFlagSymbol() const
{
	return m_pFlagSymbol;
}

CvFlagEntity* CvPlot::getFlagSymbolOffset() const
{
	return m_pFlagSymbolOffset;
}

void CvPlot::updateFlagSymbol()
{
	//PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
		return;

	PlayerTypes ePlayer = NO_PLAYER;
	PlayerTypes ePlayerOffset = NO_PLAYER;

	//CvUnit* pCenterUnit = getCenterUnit();
	CvUnit* pCenterUnit = getDebugCenterUnit(); // K-Mod

	//get the plot's unit's flag
	if (pCenterUnit != NULL)
		ePlayer = pCenterUnit->getVisualOwner();

	//get moving unit's flag
	if (gDLL->UI().getSingleMoveGotoPlot() == this)
	{
		if(ePlayer == NO_PLAYER)
			ePlayer = getActivePlayer();
		else ePlayerOffset = getActivePlayer();
	}

	//don't put two of the same flags
	if(ePlayerOffset == ePlayer)
		ePlayerOffset = NO_PLAYER;

	//destroy old flags
	if (ePlayer == NO_PLAYER)
		gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
	if (ePlayerOffset == NO_PLAYER)
		gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);

	//create and/or update unit's flag
	if (ePlayer != NO_PLAYER)
	{
		if (m_pFlagSymbol == NULL ||
			gDLL->getFlagEntityIFace()->getPlayer(m_pFlagSymbol) != ePlayer)
		{
			if (m_pFlagSymbol != NULL)
				gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
			m_pFlagSymbol = gDLL->getFlagEntityIFace()->create(ePlayer);
			if (m_pFlagSymbol != NULL)
				gDLL->getFlagEntityIFace()->setPlot(m_pFlagSymbol, this, false);
		}

		if (m_pFlagSymbol != NULL &&
			/*	advc.001: It seems that the update can occur while quick-loading,
				resulting in a crash. Maybe only when debugging. Specifically, I
				quick-loaded after stopping and continuing in pathfinding code. */
			((pCenterUnit != NULL && pCenterUnit->getEntity() != NULL) || getNumUnits() == 0))
		{
			gDLL->getFlagEntityIFace()->updateUnitInfo(m_pFlagSymbol, this, false);
		}
	}


	if (ePlayerOffset == NO_PLAYER)
		return; // advc

	//create and/or update offset flag
	if (m_pFlagSymbolOffset == NULL || gDLL->getFlagEntityIFace()->getPlayer(m_pFlagSymbolOffset) != ePlayerOffset)
	{
		if (m_pFlagSymbolOffset != NULL)
			gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);
		m_pFlagSymbolOffset = gDLL->getFlagEntityIFace()->create(ePlayerOffset);
		if (m_pFlagSymbolOffset != NULL)
			gDLL->getFlagEntityIFace()->setPlot(m_pFlagSymbolOffset, this, true);
	}

	if (m_pFlagSymbolOffset != NULL)
		gDLL->getFlagEntityIFace()->updateUnitInfo(m_pFlagSymbolOffset, this, true);
}

// advc.127c: For CvPlayer::setFlagDecal
void CvPlot::clearFlagSymbol()
{
	if (m_pFlagSymbol == NULL)
		return;
	CvDLLFlagEntityIFaceBase& kFlagEntityIFace = *gDLL->getFlagEntityIFace();
	kFlagEntityIFace.destroy(m_pFlagSymbol);
	m_pFlagSymbol = NULL; // So that updateFlagSymbols will update the flag

	/*	It seems that the EXE maintains some sort of cache or object pool.
		Temporarily assigning the flag of a player whose flag decal will not
		change dynamically seems to clear that cache. Note that these calls
		don't change the state of this CvPlot object; it's all external. */
	CvFlagEntity* pTmpFlag = kFlagEntityIFace.create(BARBARIAN_PLAYER);
	kFlagEntityIFace.setPlot(pTmpFlag, this, false);
	kFlagEntityIFace.destroy(pTmpFlag);
	setFlagDirty(true); // Will cause the EXE to call updateFlagSymbols
}


CvUnit* CvPlot::getDebugCenterUnit() const
{
	CvUnit* pCenterUnit = getCenterUnit();
	if (pCenterUnit == NULL)
	{
		if (GC.getGame().isDebugMode())
		{
			CLLNode<IDInfo>* pUnitNode = headUnitNode();
			if(pUnitNode == NULL)
				pCenterUnit = NULL;
			else pCenterUnit = ::getUnit(pUnitNode->m_data);
		}
	}
	return pCenterUnit;
}


void CvPlot::setCenterUnit(CvUnit* pNewValue)
{
	CvUnit* pOldValue = getCenterUnit();
	if (pOldValue == pNewValue)
		return; // advc

	m_pCenterUnit = pNewValue;
	updateMinimapColor();
	setFlagDirty(true);
	if (getCenterUnit() != NULL)
		getCenterUnit()->setInfoBarDirty(true);
}


void CvPlot::changeCultureRangeCities(PlayerTypes eOwnerIndex, CultureLevelTypes eRangeIndex,
	int iChange, bool bUpdatePlotGroups)
{
	if(iChange == 0)
		return;

	bool bOldCultureRangeCity = isCultureRangeCity(eOwnerIndex, eRangeIndex);
	m_aaiCultureRangeCities.add(eOwnerIndex, eRangeIndex, iChange);
	FAssert(m_aaiCultureRangeCities.get(eOwnerIndex, eRangeIndex) >= 0); // advc
	if (bOldCultureRangeCity != isCultureRangeCity(eOwnerIndex, eRangeIndex))
		updateCulture(true, bUpdatePlotGroups);
}


void CvPlot::changeInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible, int iChange)
{
	if(iChange == 0)
		return;

	bool bOldInvisibleVisible = isInvisibleVisible(eTeam, eInvisible);
	m_aaiInvisibleVisibilityCount.add(eTeam, eInvisible, iChange);
	FAssert(m_aaiInvisibleVisibilityCount.get(eTeam, eInvisible) >= 0); // advc
	if (bOldInvisibleVisible != isInvisibleVisible(eTeam, eInvisible))
	{
		if (eTeam == getActiveTeam())
			updateCenterUnit();
	}
}


CvUnit* CvPlot::getUnitByIndex(int iIndex) const
{
	CLLNode<IDInfo>* pUnitNode = m_units.nodeNum(iIndex);
	if (pUnitNode != NULL)
		return ::getUnit(pUnitNode->m_data);
	return NULL;
}


void CvPlot::addUnit(CvUnit const& kUnit, bool bUpdate)
{
	FAssert(kUnit.at(getX(), getY()));

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit const& kLoopUnit = *::getUnit(pUnitNode->m_data);
		if (!kLoopUnit.isBeforeUnitCycle(kUnit))
			break;
		pUnitNode = nextUnitNode(pUnitNode);
	}

	if (pUnitNode != NULL)
		m_units.insertBefore(kUnit.getIDInfo(), pUnitNode);
	else m_units.insertAtEnd(kUnit.getIDInfo());

	if (bUpdate)
	{
		updateCenterUnit();
		setFlagDirty(true);
	}
}


void CvPlot::removeUnit(CvUnit* pUnit, bool bUpdate)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		if (::getUnit(pUnitNode->m_data) == pUnit)
		{
			FAssert(::getUnit(pUnitNode->m_data)->at(getX(), getY()));
			m_units.deleteNode(pUnitNode);
			break;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	if (bUpdate)
	{
		updateCenterUnit();
		setFlagDirty(true);
	}
}


CLLNode<IDInfo>* CvPlot::nextUnitNodeExternal(CLLNode<IDInfo>* pNode) const
{
	return m_units.next(pNode);
}


CvSymbol* CvPlot::getSymbol(int iID) const
{
	return m_symbols[iID];
}


CvSymbol* CvPlot::addSymbol()
{
	CvSymbol* pSym=gDLL->getSymbolIFace()->createSymbol();
	m_symbols.push_back(pSym);
	return pSym;
}


void CvPlot::deleteSymbol(int iID)
{
	m_symbols.erase(m_symbols.begin()+iID);
}


void CvPlot::deleteAllSymbols()
{
	for(int i = 0; i < getNumSymbols(); i++)
	{
		gDLL->getSymbolIFace()->destroy(m_symbols[i]);
	}
	m_symbols.clear();
}

CvString CvPlot::getScriptData() const
{
	return m_szScriptData;
}

void CvPlot::setScriptData(const char* szNewValue)
{
	SAFE_DELETE_ARRAY(m_szScriptData);
	m_szScriptData = _strdup(szNewValue);
}

void CvPlot::doFeature()
{
	PROFILE_FUNC();

	if (isFeature())
	{
		int iProbability = GC.getInfo(getFeatureType()).getDisappearanceProbability();
		if (iProbability > 0)
		{	//if (SyncRandNum(10000) < iProbability)
			// UNOFFICIAL_PATCH, Gamespeed scaling, 03/04/10, jdog5000
			int iRoll = (10000 * GC.getInfo(GC.getGame().getGameSpeedType()).
					getVictoryDelayPercent()) / 100;
			if (SyncRandNum(iRoll) < iProbability) // UNOFFICIAL_PATCH: END
				setFeatureType(NO_FEATURE);
		}
	}
	else if (!isUnit() && !isImproved())
	{
		FOR_EACH_ENUM(Feature)
		{
			if (!canHaveFeature(eLoopFeature))
				continue;
			if (getBonusType() != NO_BONUS &&
				!GC.getInfo(getBonusType()).isFeature(eLoopFeature))
			{
				continue;
			}
			int iProbability = 0;
			FOR_EACH_ORTH_ADJ_PLOT(*this)
			{
				if (pAdj->getFeatureType() != eLoopFeature)
					continue;
				if (pAdj->isImproved())
				{
					iProbability += GC.getInfo(pAdj->getImprovementType()).
							getFeatureGrowthProbability();
				}
				else iProbability += GC.getInfo(eLoopFeature).getGrowthProbability();
			}
			static int const iFEATURE_GROWTH_MODIFIER = GC.getDefineINT("FEATURE_GROWTH_MODIFIER");
			iProbability *= std::max(0, iFEATURE_GROWTH_MODIFIER + 100);
			iProbability /= 100;

			if (isRoute())
			{
				static int const iROUTE_FEATURE_GROWTH_MODIFIER = GC.getDefineINT("ROUTE_FEATURE_GROWTH_MODIFIER");
				iProbability *= std::max(0, iROUTE_FEATURE_GROWTH_MODIFIER + 100);
				iProbability /= 100;
			}
			if (iProbability > 0)
			{
				//if (SyncRandNum(10000) < iProbability)
				// UNOFFICIAL_PATCH, Gamespeed scaling, 03/04/10, jdog5000: START
				int iRoll = (10000 * GC.getInfo(GC.getGame().getGameSpeedType()).
						getVictoryDelayPercent()) / 100;
				if (syncRand().get(iRoll, "CvPlot::doFeature",
					getX(), getY()) < iProbability) // advc.007: Log coordinates
				// UNOFFICIAL_PATCH: END
				{
					setFeatureType(eLoopFeature);
					CvCity* pCity = GC.getMap().findCity(getX(), getY(), getOwner(), NO_TEAM, false);
					if (pCity != NULL &&
						/*	advc.106: K-Mod had added an isVisible check,
							but features aren't affected by fog of war. */
						isRevealed(TEAMID(pCity->getOwner())))
					{
						// Tell the owner of this city.
						CvWString szBuffer(gDLL->getText("TXT_KEY_MISC_FEATURE_GROWN_NEAR_CITY",
								GC.getInfo(eLoopFeature).getTextKeyWide(), pCity->getNameKey()));
						gDLL->UI().addMessage(/*getOwner()*/ pCity->getOwner(), // K-Mod (bugfix)
								false, -1, szBuffer, *this, "AS2D_FEATUREGROWTH", MESSAGE_TYPE_INFO,
								GC.getInfo(eLoopFeature).getButton());
					}
					break;
				}
			}
		}
	}
}

void CvPlot::doImprovementControl()
{
	// Super Forts begin *culture*
	doImprovementCulture();
	CLLNode<IDInfo>* pUnitNode;
	PlayerTypes eCulturalOwner;
	CvUnit* pLoopUnit;
	CvWString szBuffer;
	
	ImprovementTypes eImprovement = getImprovementType();
	if(eImprovement != NO_IMPROVEMENT)
	{
		// Check for a fort culture flip
		if(GC.getImprovementInfo(eImprovement).isActsAsCity() && (getOwnershipDuration() > GC.getDefineINT("SUPER_FORTS_DURATION_BEFORE_REVOLT")))
		{
			eCulturalOwner = calculateCulturalOwner();
			if(eCulturalOwner != NO_PLAYER)
			{
				if(GET_PLAYER(eCulturalOwner).getTeam() != getTeam())
				{
					bool bDefenderFound = false;
					CLinkList<IDInfo> oldUnits;
					pUnitNode = headUnitNode();

					while (pUnitNode != NULL)
					{
						oldUnits.insertAtEnd(pUnitNode->m_data);
						pUnitNode = nextUnitNode(pUnitNode);
					}

					pUnitNode = oldUnits.head();

					while (pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = nextUnitNode(pUnitNode);
						if(pLoopUnit->canDefend(this))
						{
							if(pLoopUnit->getOwner() == getOwner())
							{
								bDefenderFound = true;
								break;
							}
						}
					}
					if(!bDefenderFound)
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_REVOLTED_JOINED", GC.getImprovementInfo(getImprovementType()).getText(), GET_PLAYER(eCulturalOwner).getCivilizationDescriptionKey());
						gDLL->getInterfaceIFace()->addMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREFLIP", MESSAGE_TYPE_INFO, GC.getImprovementInfo(getImprovementType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX(), getY(), true, true);
						gDLL->getInterfaceIFace()->addMessage(eCulturalOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREFLIP", MESSAGE_TYPE_INFO, GC.getImprovementInfo(getImprovementType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX(), getY(), true, true);
						setOwner(eCulturalOwner,true,true);
					}
				}
			}
		}
	}
	// Super Forts end
}
void CvPlot::doCulture()
{
	if (isFortImprovement())
	{
		doImprovementControl(); 
		/*keldath doto - this entore code was here
		so i exported it to a separated fn, couldnt find a proper name since theres already
		doImprovementCulture...*/
	}
	// <advc> Moved the bulk of the code into a new CvCity member function
	CvCity* pPlotCity = getPlotCity();
	if(pPlotCity != NULL)
		pPlotCity->doRevolt(); // </advc>
	doCultureDecay(); // advc.099b
	updateCulture(true, true);
}

// advc.099b:
void CvPlot::doCultureDecay()
{
	PROFILE_FUNC();
	if (getTotalCulture() <= 0)
		return;
	int const iExclDecay = GC.getDefineINT(CvGlobals::CITY_RADIUS_DECAY);
	// advc.099:
	static int const iBaseDecayPerMill = GC.getDefineINT("TILE_CULTURE_DECAY_PER_MILL");
	bool abInRadius[MAX_CIV_PLAYERS] = {false};
	bool bInAnyRadius = false;
	int iMaxRadiusCulture = 0;
	int iMinDist = 10;
	/*  To avoid ownership oscillation and to avoid making players worried that a
		tile might flip */
	int const iCulturePercentThresh = 55;
	if (iExclDecay != 0 && isOwned() && !isCity())
	{
		CvCity* pWorkingCity = getWorkingCity();
		if (pWorkingCity == NULL ||
			// To save time:
			calculateCulturePercent(pWorkingCity->getOwner()) < iCulturePercentThresh)
		{
			for (NearbyCityIter itCity(*this); itCity.hasNext(); ++itCity)
			{
				PlayerTypes const eCityOwner = itCity->getOwner();
				if (eCityOwner != BARBARIAN_PLAYER)
				{
					iMinDist = std::min(iMinDist, plotDistance(itCity->plot(), this));
					iMaxRadiusCulture = std::max(iMaxRadiusCulture, getCulture(eCityOwner));
					abInRadius[eCityOwner] = true;
					bInAnyRadius = true;
				}
			}
		}
	}
	FOR_EACH_ENUM(Player)
	{
		int iCulture = getCulture(eLoopPlayer);
		if (iCulture <= 0)
			continue;
		int iDecayPerMill = iBaseDecayPerMill;
		if (bInAnyRadius && !abInRadius[eLoopPlayer] &&
			!isImpassable() && // Doesn't matter if no one can work impassable plots
			iCulture >
			scaled(100 - iCulturePercentThresh, iCulturePercentThresh) * iMaxRadiusCulture)
		{
			scaled rExclDecay;
			if(iMinDist <= 2)
				rExclDecay += iExclDecay;
			if(iMinDist <= 1)
				rExclDecay += iExclDecay;
			if (iCulture < iMaxRadiusCulture)
			{
				// Gradually throttle decay when approaching the threshold
				rExclDecay.mulDiv(iCulture, iMaxRadiusCulture);
			}
			iDecayPerMill += rExclDecay.round();
		}
		iCulture -= (iCulture * iDecayPerMill) / 1000;
		setCulture(eLoopPlayer, iCulture, false, false);
	}
}

/*	advc.099b: Return value:
	 0: city tile belonging to ePlayer
	-1: Not in the exclusive radius of ePlayer. I.e. not in the radius of any
		ePlayer city or also in the radius of another player's city.
	 1 or 2: In the exclusive radius of ePlayer.
	 1: In an inner ring
	 2: Not in any inner ring */
int CvPlot::exclusiveRadius(PlayerTypes ePlayer) const
{
	if (isCity())
	{
		if(getOwner() == ePlayer)
			return 0;
		return -1;
	}
	int iRadius = -1;
	for (NearbyCityIter itCity(*this); itCity.hasNext(); ++itCity)
	{
		if (itCity->getOwner() == ePlayer)
			iRadius = plotDistance(itCity->plot(), this);
		else return -1;
	}
	return iRadius;
}

/*	advc: Replacing the old getArea function. Not public b/c CvAreas shouldn't be
	routinely passed and accessed by id. */
int CvPlot::areaID() const
{
	return (m_pArea == NULL ? FFreeList::INVALID_INDEX : m_pArea->getID());
}


void CvPlot::processArea(CvArea& kArea, int iChange)
{
	// XXX not updating getBestFoundValue() or getAreaAIType()...

	if (iChange == 0)
	{
		FAssert(iChange != 0);
		return;
	}

	kArea.changeNumTiles(iChange);
	if (isOwned())
		kArea.changeNumOwnedTiles(iChange);

	if (isNOfRiver())
		kArea.changeNumRiverEdges(iChange);
	if (isWOfRiver())
		kArea.changeNumRiverEdges(iChange);

	if (getBonusType() != NO_BONUS)
		kArea.changeNumBonuses(getBonusType(), iChange);
	// advc.opt:
	/*if (getImprovementType() != NO_IMPROVEMENT)
		kArea.changeNumImprovements(getImprovementType(), iChange);*/

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		PlayerTypes ePlayer = (PlayerTypes)i;
		if (GET_PLAYER(ePlayer).getStartingPlot() == this)
			kArea.changeNumStartingPlots(iChange);
		kArea.changePower(ePlayer, getUnitPower(ePlayer) * iChange);
		kArea.changeUnitsPerPlayer(ePlayer, plotCount(PUF_isPlayer, ePlayer) * iChange);
		// advc: No longer kept track of
		//kArea.changeAnimalsPerPlayer(ePlayer, plotCount(PUF_isAnimal, -1, -1, ePlayer) * iChange);
		for (int j = 0; j < NUM_UNITAI_TYPES; j++)
		{
			UnitAITypes eUnitAI = (UnitAITypes)j;
			kArea.changeNumAIUnits(ePlayer, eUnitAI,
					plotCount(PUF_isUnitAIType, eUnitAI, -1, ePlayer) * iChange);
		}
	}
	FOR_EACH_ENUM(Team)
	{
		if (isRevealed(eLoopTeam))
			kArea.changeNumRevealedTiles(eLoopTeam, iChange);
	}

	CvCity* pCity = getPlotCity();
	if (pCity == NULL)
		return;

	// XXX make sure all of this (esp. the changePower()) syncs up...
	kArea.changePower(pCity->getOwner(), iChange *
			GC.getGame().getPopulationPower(pCity->getPopulation()));

	kArea.changeCitiesPerPlayer(pCity->getOwner(), iChange);
	// <advc.030b>
	CvArea* pWaterArea = waterArea(true);
	/*  Fixme: During CvMap::recalculateAreas, for iChange=-1, the call above
		could fail to locate an adjacent water area because the area of all
		adjacent water tiles may already have been set to NULL. A subsequent
		processArea call with iChange=1 would then lead to an incorrect
		city count. I think this can only happen in a scenario with preplaced
		cities though, and I'm not sure what to do about it. */
	if(pWaterArea != NULL)
	{
		if(iChange > 0 || (iChange < 0 &&
			// See comment in CvCity::kill
			pWaterArea->getCitiesPerPlayer(getOwner()) > 0, true))
		{
			pWaterArea->changeCitiesPerPlayer(getOwner(), iChange);
		}
	} // </advc.030b>
	kArea.changePopulationPerPlayer(pCity->getOwner(), pCity->getPopulation() * iChange);

	for (int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		BuildingTypes eBuilding = (BuildingTypes)i;
		int const iTotalChange = iChange * pCity->getNumActiveBuilding(eBuilding);
		if (iTotalChange <= 0)
			continue;
		CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
		kArea.changePower(pCity->getOwner(), kBuilding.getPowerValue() * iTotalChange);
		if (kBuilding.getAreaHealth() > 0)
			kArea.changeBuildingGoodHealth(pCity->getOwner(), kBuilding.getAreaHealth() * iTotalChange);
		else kArea.changeBuildingBadHealth(pCity->getOwner(), kBuilding.getAreaHealth() * iTotalChange);
		kArea.changeBuildingHappiness(pCity->getOwner(), kBuilding.getAreaHappiness() * iTotalChange);
		kArea.changeFreeSpecialist(pCity->getOwner(), kBuilding.getAreaFreeSpecialist() * iTotalChange);
		kArea.changeCleanPowerCount(pCity->getTeam(), kBuilding.isAreaCleanPower() ? iTotalChange : 0);
		kArea.changeBorderObstacleCount(pCity->getTeam(), kBuilding.isAreaBorderObstacle() ? iTotalChange : 0);
		FOR_EACH_ENUM(Yield)
		{
			kArea.changeYieldRateModifier(pCity->getOwner(), eLoopYield,
					kBuilding.getAreaYieldModifier(eLoopYield) * iTotalChange);
		}
	}
	FOR_EACH_ENUM(UnitAI)
	{
		kArea.changeNumTrainAIUnits(pCity->getOwner(), eLoopUnitAI,
				pCity->getNumTrainUnitAI(eLoopUnitAI) * iChange);
	}
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (kArea.AI_getTargetCity((PlayerTypes)i) == pCity)
			kArea.AI_setTargetCity((PlayerTypes)i, NULL);
	}
}


ColorTypes CvPlot::plotMinimapColor()
{
	// <advc.opt> (Probably quite unnecessary)
	static ColorTypes const eColorClear = GC.getColorType("CLEAR");
	static ColorTypes const eColorWhite = GC.getColorType("WHITE");
	// </advc.opt>
	if (getActivePlayer() == NO_PLAYER)
		return eColorClear;

	CvCity* pCity = getPlotCity();
	TeamTypes eActiveTeam = getActiveTeam();
	if (pCity != NULL && pCity->isRevealed(eActiveTeam, true))
		return eColorWhite;

	if (isActiveVisible(true) &&
		GC.getMap().getMinimapSettings().isShowUnits()) // advc.002a
	{
		CvUnit* pCenterUnit = getDebugCenterUnit();
		if (pCenterUnit != NULL)
		{
			return GC.getInfo(GET_PLAYER(pCenterUnit->getVisualOwner()).
					getKnownPlayerColor()).getColorTypePrimary();
		}
	}
	// kekm.21: Removed !isRevealedBarbarian() clause
	if (getRevealedOwner(eActiveTeam, true) != NO_PLAYER)
	{
		return GC.getInfo(GET_PLAYER(getRevealedOwner(eActiveTeam, true)).
				getKnownPlayerColor()).getColorTypePrimary();
	}
	return eColorClear;
}

// read object from a stream. used during load.
void CvPlot::read(FDataStreamBase* pStream)
{
	//reset(); // advc: Constructor handles that

	uint uiFlag=0;
	pStream->Read(&uiFlag);
	pStream->Read(&m_iX);
	pStream->Read(&m_iY);
	// <advc.opt>
	if (uiFlag >= 12)
		pStream->Read(&m_iPlotNum);
	else updatePlotNum();
	// </advc.opt>
	pStream->Read(&m_iArea);
// Deliverator	freshwater
	pStream->Read(&m_iFreshWaterAmount); // Deliverator		
	pStream->Read(&m_iFeatureVariety);
	pStream->Read(&m_iOwnershipDuration);
	pStream->Read(&m_iImprovementDuration);
	// <advc.912f>
	if (uiFlag < 9)
	{
		short iUpgradeProgress;
		pStream->Read(&iUpgradeProgress);
		m_iUpgradeProgress = iUpgradeProgress;
		m_iUpgradeProgress *= 100;
	} // </advc.912f>
	else pStream->Read(&m_iUpgradeProgress);
	pStream->Read(&m_iForceUnownedTimer);
	// <advc.opt>
	if (uiFlag < 5)
	{
		short sTmp; pStream->Read(&sTmp);
		m_iCityRadiusCount = safeIntCast<char>(sTmp);
	}
	else pStream->Read(&m_iCityRadiusCount);
	int iRiver;
	pStream->Read(&iRiver);
	if (iRiver < MIN_SHORT || iRiver > MAX_SHORT)
		m_iRiverID = -1;
	else m_iRiverID = static_cast<short>(iRiver);
	// </advc.opt>
	pStream->Read(&m_iMinOriginalStartDist);
	pStream->Read(&m_iReconCount);
	// <advc.opt>
	if (uiFlag < 5)
	{
		short sTmp; pStream->Read(&sTmp);
		m_iRiverCrossingCount = safeIntCast<char>(sTmp);
	}
	else pStream->Read(&m_iRiverCrossingCount); // </advc.opt>
	// <advc.tsl>
	if(uiFlag >= 3)
	{
		if (uiFlag < 5)
		{
			short sTmp;
			pStream->Read(&sTmp);
			m_iLatitude = safeIntCast<char>(sTmp);
		}
		else pStream->Read(&m_iLatitude);
	}
	else m_iLatitude = calculateLatitude(); // </advc.tsl>

	bool bVal;
	// Super Forts begin *canal* *choke*
	pStream->Read(&m_iCanalValue);
	pStream->Read(&m_iChokeValue);
	// Super Forts end
	// Super Forts begin *bombard*
	pStream->Read(&m_iDefenseDamage);
	pStream->Read(&m_bBombarded);
	// Super Forts end
	
	pStream->Read(&bVal);
	m_bStartingPlot = bVal;
	if(uiFlag < 4) // advc.opt: m_bHills removed
		pStream->Read(&bVal);
//keldath f1rpo-The relevant info is in m_ePlotType and can be accessed through isHills() and isPeak(); that was already the case in BtS. m_bHills was, essentially, unused.
//===NM=====Mountains Mod===0=====
/*	pStream->Read(&bVal);
	m_bHills = bVal;
	pStream->Read(&bVal);
	m_bPeaks = bVal;
*/
//===NM=====Mountains Mod===X=====
	pStream->Read(&bVal);
	m_bNOfRiver = bVal;
	pStream->Read(&bVal);
	m_bWOfRiver = bVal;
	pStream->Read(&bVal);
	m_bIrrigated = bVal;
	// <advc.opt>
	if (uiFlag >= 7)
	{
		pStream->Read(&bVal);
		m_bImpassable = bVal;
	}
	if (uiFlag >= 8) // else CvMap::read will handle it
	{
		pStream->Read(&bVal);
		m_bAnyIsthmus = bVal;
	} // </advc.opt>
	pStream->Read(&bVal);
	m_bPotentialCityWork = bVal;
	// m_bShowCitySymbols not saved
	// m_bFlagDirty not saved
	// m_bPlotLayoutDirty not saved
	// m_bLayoutStateWorked not saved

	pStream->Read(&m_eOwner);
	// < JCultureControl Mod Start >
	pStream->Read(&m_eImprovementOwner);
	// < JCultureControl Mod End >
	// <advc.opt>
	if (uiFlag >= 6)
		pStream->Read(&m_eTeam);
	else updateTeam(); // </advc.opt>
	if (uiFlag < 5)
	{
		short sTmp;
		pStream->Read(&sTmp);
		m_ePlotType = safeIntCast<char>(sTmp);
		pStream->Read(&sTmp);
		m_eTerrainType = safeIntCast<char>(sTmp);
		pStream->Read(&sTmp);
		m_eFeatureType = safeIntCast<char>(sTmp);
	}
	else
	{ // </advc.opt>
		pStream->Read(&m_ePlotType);
		pStream->Read(&m_eTerrainType);
		pStream->Read(&m_eFeatureType);
	}  // <advc.opt>
	if (uiFlag < 7)
		updateImpassable(); // </advc.opt>
	pStream->Read(&m_eBonusType);
	FAssertInfoEnum((BonusTypes)m_eBonusType); // advc
	// <advc.opt>
	if (uiFlag < 5)
	{
		short sTmp;
		pStream->Read(&sTmp);
		m_eImprovementType = safeIntCast<char>(sTmp);
	}
	else pStream->Read(&m_eImprovementType);
	if (uiFlag < 5)
	{
		short sTmp;
		pStream->Read(&sTmp);
		m_eRouteType = safeIntCast<char>(sTmp);
	}
	else pStream->Read(&m_eRouteType); // </advc.opt>
	pStream->Read(&m_eRiverNSDirection);
	pStream->Read(&m_eRiverWEDirection);
	// <advc.035>
	if(uiFlag >= 1)
		pStream->Read(&m_eSecondOwner);
	else m_eSecondOwner = m_eOwner;
	if (!GC.getDefineBOOL(CvGlobals::OWN_EXCLUSIVE_RADIUS))
		m_eSecondOwner = m_eOwner; // </advc.035>
	pStream->Read((int*)&m_plotCity.eOwner);
	pStream->Read(&m_plotCity.iID);
	pStream->Read((int*)&m_workingCity.eOwner);
	pStream->Read(&m_workingCity.iID);
	pStream->Read((int*)&m_workingCityOverride.eOwner);
	pStream->Read(&m_workingCityOverride.iID);
	// <advc.opt>
	m_plotCity.validateOwner();
	m_workingCity.validateOwner();
	m_workingCityOverride.validateOwner(); // </advc.opt>

	if (uiFlag >= 13)
		m_aiYield.read(pStream);
	else if (uiFlag < 5)
		m_aiYield.readArray<short>(pStream);
	else m_aiYield.readArray<char>(pStream);
	// BETTER_BTS_AI_MOD, Efficiency (plot danger cache), 08/21/09, jdog5000: START
	/*	K-Mod. I've changed the purpose of invalidateBorderDangerCache.
		It is no longer appropriate for this. */
	/*m_iActivePlayerNoBorderDangerCache = false;
	invalidateBorderDangerCache();*/
	// BETTER_BTS_AI_MOD: END
	if (uiFlag >= 13)
	{
		m_aiCulture.read(pStream);
	// < JCultureControl Mod Start >
		m_aiCultureControl.read(pStream);
	// < JCultureControl Mod End >
	// Super Forts begin *culture*
		m_aiCultureRangeForts.read(pStream);
	// Super Forts end
		m_aiFoundValue.read(pStream);
		m_aiPlayerCityRadiusCount.read(pStream);
		m_aiPlotGroup.read(pStream);
		m_aiVisibilityCount.read(pStream);
		m_aiStolenVisibilityCount.read(pStream);
		m_aiBlockadedCount.read(pStream);
		m_aiRevealedOwner.read(pStream);
		m_abRiverCrossing.read(pStream);
		m_abRevealed.read(pStream);
		m_aeRevealedImprovementType.read(pStream);
		m_aeRevealedRouteType.read(pStream);
	}
	else
	{
		m_aiCulture.readLazyArray<char,int>(pStream);
	// < JCultureControl Mod Start >
		m_aiCultureControl.readArray<int>(pStream);
	// < JCultureControl Mod End >
	// Super Forts begin *culture*
		m_aiCultureRangeForts.readArray<short>(pStream);
	// Super Forts end
		m_aiFoundValue.readLazyArray<char,short>(pStream);
		if (uiFlag < 10)
			m_aiPlayerCityRadiusCount.readLazyArray<char,char>(pStream);
		else m_aiPlayerCityRadiusCount.readPair<short,short,char>(pStream);
		m_aiPlotGroup.readLazyArray<char,int>(pStream);
		m_aiVisibilityCount.readLazyArray<char,short>(pStream);
		if (uiFlag < 10)
		{
			m_aiStolenVisibilityCount.readLazyArray<char,short>(pStream);
			m_aiBlockadedCount.readLazyArray<char,short>(pStream);
		}
		else
		{
			m_aiStolenVisibilityCount.readPair<short,short,short>(pStream);
			m_aiBlockadedCount.readPair<short,short,short>(pStream);
		}
		m_aiRevealedOwner.readLazyArray<char,char>(pStream);
		m_abRiverCrossing.readLazyArray<char,bool>(pStream);
		m_abRevealed.readLazyArray<char,bool>(pStream);
		if (uiFlag < 5)
			m_aeRevealedImprovementType.readLazyArray<char,short>(pStream);
		else if (uiFlag < 10)
			m_aeRevealedImprovementType.readLazyArray<char,char>(pStream);
		else m_aeRevealedImprovementType.readPair<short,short,short>(pStream);
		if (uiFlag == 10)
		{	// List-based map didn't work out well here
			ListEnumMap<TeamTypes,RouteTypes> aeRevealedRouteType;
			aeRevealedRouteType.readPair<short,short,short>(pStream);
			if (aeRevealedRouteType.isAnyNonDefault())
			{
				FOR_EACH_ENUM(Team)
					m_aeRevealedRouteType.set(eLoopTeam, aeRevealedRouteType.get(eLoopTeam));
			}
		}
		else
		{
			if (uiFlag < 5)
				m_aeRevealedRouteType.readLazyArray<char,short>(pStream);
			else m_aeRevealedRouteType.readLazyArray<char,char>(pStream);
		}
	}
	m_szScriptData = pStream->ReadString();
	if (uiFlag >= 13)
		m_aiBuildProgress.read(pStream);
	else m_aiBuildProgress.readLazyArray<int,short>(pStream);
	// <advc.011>
	if (uiFlag < 5)
	{
		int iTmp; pStream->Read(&iTmp);
		m_iTurnsBuildsInterrupted = safeIntCast<short>(iTmp);
	}
	else pStream->Read(&m_iTurnsBuildsInterrupted); // </advc.011>
	// <advc.005c>
	CvWString szTmp;
	pStream->ReadString(szTmp);
	if (!szTmp.empty())
		setRuinsName(szTmp); // </advc.005c>
	// <advc.opt>
	if (uiFlag >= 2)
		pStream->Read(&m_iTotalCulture);
	else if (m_aiCulture.isAnyNonDefault()) // just to save time
	{
		/*  CvPlayer objects aren't loaded yet. Players that have never been alive
			should have 0 culture, so we don't really need to check isEverAlive. */
		FOR_EACH_ENUM(Player)
			m_iTotalCulture += m_aiCulture.get(eLoopPlayer);
	} // </advc.opt>
	if (uiFlag >= 13)
	{
		m_aaiCultureRangeCities.read(pStream);
		m_aaiInvisibleVisibilityCount.read(pStream);
	}
	else if (uiFlag >= 10)
	{
		m_aaiCultureRangeCities.readTuple<short,short,char>(pStream);
		m_aaiInvisibleVisibilityCount.readTuple<short,short,short>(pStream);
	}
	else
	{
		m_aaiCultureRangeCities.readLazyArray<char,int,char>(pStream);
		m_aaiInvisibleVisibilityCount.readLazyArray<char,int,short>(pStream);
		
	}
	m_units.Read(pStream);
}

// write object to a stream. used during save.
void CvPlot::write(FDataStreamBase* pStream)
{
	PROFILE_FUNC(); // advc
	uint uiFlag;
	//uiFlag = 1; // advc.035
	//uiFlag = 2; // advc.opt
	//uiFlag = 3; // advc.tsl
	//uiFlag = 4; // advc.opt: m_bHills removed
	//uiFlag = 5; // advc.opt, advc.011, advc.enum: some int or short members turned into short or char
	//uiFlag = 6; // advc.opt: m_eTeam
	//uiFlag = 7; // advc.opt: m_bImpassable
	//uiFlag = 8; // advc.opt: m_bAnyIsthmus
	//uiFlag = 9; // advc.912f
	//uiFlag = 10; // advc.enum (sparse maps 1)
	//uiFlag = 11; // advc.enum (sparse maps 2)
	//uiFlag = 12; // advc.opt (m_iPlotNum)
	uiFlag = 13; // advc.enum: new enum map save behavior
	pStream->Write(uiFlag);
	REPRO_TEST_BEGIN_WRITE(CvString::format("Plot pt1(%d,%d)", getX(), getY()).GetCString());
	pStream->Write(m_iX);
	pStream->Write(m_iY);
	pStream->Write(m_iPlotNum); // advc.opt
	pStream->Write(areaID());

	pStream->Write(m_iFreshWaterAmount); // Deliverator	
	pStream->Write(m_iFeatureVariety);
	pStream->Write(m_iOwnershipDuration);
	pStream->Write(m_iImprovementDuration);
	pStream->Write(m_iUpgradeProgress);
	pStream->Write(m_iForceUnownedTimer);
	pStream->Write(m_iCityRadiusCount);
	pStream->Write((int)m_iRiverID); // advc.opt (cast)
	pStream->Write(m_iMinOriginalStartDist);
	pStream->Write(m_iReconCount);
	pStream->Write(m_iRiverCrossingCount);
	pStream->Write(m_iLatitude); // advc.tsl

// Super Forts begin *canal* *choke*
	pStream->Write(m_iCanalValue);
	pStream->Write(m_iChokeValue);
	// Super Forts end
	// Super Forts begin *bombard*
	pStream->Write(m_iDefenseDamage);
	pStream->Write(m_bBombarded);
	// Super Forts end

	pStream->Write(m_bStartingPlot);
//keldath f1rpo-The relevant info is in m_ePlotType and can be accessed through isHills() and isPeak(); that was already the case in BtS. m_bHills was, essentially, unused.
//	pStream->Write(m_bHills);
//===NM=====Mountains Mod===0=====
//	pStream->Write(m_bPeaks);
//===NM=====Mountains Mod===X=====
	pStream->Write(m_bNOfRiver);
	pStream->Write(m_bWOfRiver);
	pStream->Write(m_bIrrigated);
	pStream->Write(m_bImpassable); // advc.opt
	pStream->Write(m_bAnyIsthmus); // advc.opt
	pStream->Write(m_bPotentialCityWork);
	// m_bShowCitySymbols not saved
	// m_bFlagDirty not saved
	// m_bPlotLayoutDirty not saved
	// m_bLayoutStateWorked not saved

	pStream->Write(m_eOwner);
	// < JCultureControl Mod Start >
	pStream->Write(m_eImprovementOwner);
	// < JCultureControl Mod End >
	pStream->Write(m_eTeam); // advc.opt
	pStream->Write(m_ePlotType);
	pStream->Write(m_eTerrainType);
	pStream->Write(m_eFeatureType);
	pStream->Write(m_eBonusType);
	pStream->Write(m_eImprovementType);
	pStream->Write(m_eRouteType);
	pStream->Write(m_eRiverNSDirection);
	pStream->Write(m_eRiverWEDirection);
	// <advc.035>
	/*	To be consistent with code in read that resets m_eSecondOwner in case
		that the option was disabled in between saving and reloading. */
	if(!GC.getDefineBOOL(CvGlobals::OWN_EXCLUSIVE_RADIUS))
		m_eSecondOwner = m_eOwner;
	pStream->Write(m_eSecondOwner); // <advc.035>
	pStream->Write(m_plotCity.eOwner);
	pStream->Write(m_plotCity.iID);
	pStream->Write(m_workingCity.eOwner);
	pStream->Write(m_workingCity.iID);
	pStream->Write(m_workingCityOverride.eOwner);
	pStream->Write(m_workingCityOverride.iID);
	REPRO_TEST_END_WRITE();
	REPRO_TEST_BEGIN_WRITE(CvString::format("Plot pt2(%d,%d)", getX(), getY()).GetCString());
	m_aiYield.write(pStream);
	m_aiCulture.write(pStream);
//< JCultureControl Mod start >
	m_aiCultureControl.write(pStream);
   // < JCultureControl Mod End >
	// Super Forts begin *culture*//if i put option on this - ctd
	m_aiCultureRangeForts.write(pStream);;
	// Super Forts end
	m_aiFoundValue.write(pStream);
	m_aiPlayerCityRadiusCount.write(pStream);
	m_aiPlotGroup.write(pStream);
	m_aiVisibilityCount.write(pStream);
	m_aiStolenVisibilityCount.write(pStream);
	m_aiBlockadedCount.write(pStream);
	m_aiRevealedOwner.write(pStream);
	m_abRiverCrossing.write(pStream);
	m_abRevealed.write(pStream);
	m_aeRevealedImprovementType.write(pStream);
	m_aeRevealedRouteType.write(pStream);

	pStream->WriteString(m_szScriptData);

	m_aiBuildProgress.write(pStream);

	pStream->Write(m_iTurnsBuildsInterrupted); // advc.011
	// <advc.005c>
	std::wstring szTmp;
	if (m_szMostRecentCityName != NULL)
		szTmp = m_szMostRecentCityName;
	pStream->WriteString(szTmp); // </advc.005c>
	pStream->Write(m_iTotalCulture); // advc.opt

	m_aaiCultureRangeCities.write(pStream);
	m_aaiInvisibleVisibilityCount.write(pStream);

	m_units.Write(pStream);
	REPRO_TEST_END_WRITE();
}


void CvPlot::setLayoutDirty(bool bDirty)
{
	if (!GC.IsGraphicsInitialized())
		return;

	if (isLayoutDirty() != bDirty)
	{
		m_bPlotLayoutDirty = bDirty;
		if (isLayoutDirty() && m_pPlotBuilder == NULL)
		{
			if (!updatePlotBuilder())
				m_bPlotLayoutDirty = false;
		}
	}
}


bool CvPlot::updatePlotBuilder()
{
	if (GC.IsGraphicsInitialized() && shouldUsePlotBuilder())
	{
		if (m_pPlotBuilder == NULL) // we need a plot builder... but it doesn't exist
		{
			m_pPlotBuilder = gDLL->getPlotBuilderIFace()->create();
			gDLL->getPlotBuilderIFace()->init(m_pPlotBuilder, this);
		}
		return true;
	}
	return false;
}

/*  The plot layout contains bonuses and improvements --
	it is, like the city layout, passively computed by LSystems */
bool CvPlot::isLayoutDirty() const
{
	return m_bPlotLayoutDirty;
}


bool CvPlot::isLayoutStateDifferent() const
{
	return (m_bLayoutStateWorked != isBeingWorked());
}


void CvPlot::setLayoutStateToCurrent()
{
	m_bLayoutStateWorked = isBeingWorked();
}

// determines how the improvement state is shown in the engine
void CvPlot::getVisibleImprovementState(ImprovementTypes& eType, bool& bWorked)
{
	eType = NO_IMPROVEMENT;
	bWorked = false;

	if (getActiveTeam() == NO_TEAM)
		return;

	eType = getRevealedImprovementType(getActiveTeam(), true);
	if (eType == NO_IMPROVEMENT)
	{
		if (isActiveVisible(true))
		{
			if (isBeingWorked() && !isCity())
			{
				if (isWater())
					eType = (ImprovementTypes)GC.getDefineINT("WATER_IMPROVEMENT");
				else eType = (ImprovementTypes)GC.getDefineINT("LAND_IMPROVEMENT");
			}
		}
	}

	// worked state
	if (isActiveVisible(false) && isBeingWorked())
		bWorked = true;
}

// determines how the bonus state is shown in the engine
void CvPlot::getVisibleBonusState(BonusTypes& eType, bool& bImproved, bool& bWorked)
{
	eType = NO_BONUS;
	bImproved = false;
	bWorked = false;

	if (getActiveTeam() == NO_TEAM)
		return;

	if (GC.getGame().isDebugMode())
		eType = getBonusType();
	else if (isRevealed(getActiveTeam()))
		eType = getBonusType(getActiveTeam());

	if (eType != NO_BONUS) // improved and worked states ...
	{
		ImprovementTypes eRevealedImprovement = getRevealedImprovementType(
				getActiveTeam(), true);
		if (eRevealedImprovement != NO_IMPROVEMENT &&
			GC.getInfo(eRevealedImprovement).isImprovementBonusTrade(eType))
		{
			bImproved = true;
			bWorked = isBeingWorked();
		}
	}
}


bool CvPlot::shouldUsePlotBuilder()
{
	bool bBonusImproved; bool bBonusWorked; bool bImprovementWorked;
	BonusTypes eBonusType;
	ImprovementTypes eImprovementType;
	getVisibleBonusState(eBonusType, bBonusImproved, bBonusWorked);
	getVisibleImprovementState(eImprovementType, bImprovementWorked);
	return (eBonusType != NO_BONUS || eImprovementType != NO_IMPROVEMENT);
}

/* This function has been disabled by K-Mod, because it
	doesn't work correctly and so using it is just a magnet for bugs. */
//int CvPlot::calculateMaxYield(YieldTypes eYield) const
//{ /* advc: body deleted */ }

int CvPlot::getYieldWithBuild(BuildTypes eBuild, YieldTypes eYield, bool bWithUpgrade) const
{
	int iYieldRate = 0;

	bool bIgnoreFeature = false;
	if (isFeature())
	{
		if (GC.getInfo(eBuild).isFeatureRemove(getFeatureType()))
			bIgnoreFeature = true;
	}
	int iNatureYield = // advc.908a: Preserve this for later
			calculateNatureYield(eYield, getTeam(), bIgnoreFeature);
	iYieldRate += iNatureYield;

	ImprovementTypes eImprovement = GC.getInfo(eBuild).getImprovement();
	// K-Mod. if the build doesn't have its own improvement - use the existing one!
	if (eImprovement == NO_IMPROVEMENT)
	{
		eImprovement = getImprovementType();
		/*	note: unfortunately this won't account for the fact that
			some builds will destroy the existing improvement
			without creating a new one. eg. chopping a forest which has a lumbermill.
			I'm sorry about that. I may correct it later. */
	}
	// K-Mod end

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (bWithUpgrade)
		{
			/*	in the case that improvements upgrade, use 2 upgrade levels higher
				for the yield calculations. */
			/*ImprovementTypes eUpgradeImprovement = GC.getInfo(eImprovement).getImprovementUpgrade();
			if (eUpgradeImprovement != NO_IMPROVEMENT) {
				//unless it's commerce on a low food tile, in which case only use 1 level higher
				if ((eYield != YIELD_COMMERCE) || (getYield(YIELD_FOOD) >= GC.getFOOD_CONSUMPTION_PER_POPULATION())) {
					ImprovementTypes eUpgradeImprovement2 = (ImprovementTypes)GC.getInfo(eUpgradeImprovement).getImprovementUpgrade();
					if (eUpgradeImprovement2 != NO_IMPROVEMENT)
						eUpgradeImprovement = eUpgradeImprovement2;
				}
			}
			if ((eUpgradeImprovement != NO_IMPROVEMENT) && (eUpgradeImprovement != eImprovement))
				eImprovement = eUpgradeImprovement;*/
			// <k146> stuff that. Just use 2 levels.
			ImprovementTypes eFinalImprovement = CvImprovementInfo::finalUpgrade(eImprovement);
			if (eFinalImprovement != NO_IMPROVEMENT)
				eImprovement = eFinalImprovement;
			// </k146>
		}

		iYieldRate += calculateImprovementYieldChange(
				eImprovement, eYield, getOwner());
	}

	RouteTypes const eRoute = GC.getInfo(eBuild).getRoute();
	if (eRoute != NO_ROUTE)
	{
		//eImprovement = getImprovementType(); // disabled by K-Mod
		if (eImprovement != NO_IMPROVEMENT)
		{
			CvImprovementInfo const& kImprov = GC.getInfo(eImprovement);
			FOR_EACH_ENUM(Yield)
			{
				iYieldRate += kImprov.getRouteYieldChanges(eRoute, eLoopYield);
				if (isRoute())
					iYieldRate -= kImprov.getRouteYieldChanges(getRouteType(), eLoopYield);
			}
		}
	}

	/*	K-Mod. Count the 'extra yield' for financial civs.
		(Don't bother with golden-age bonuses.) */
	int iThreshold = GET_PLAYER(getOwner()).getExtraYieldThreshold(eYield);
	if (iThreshold > 0 &&
		(iYieldRate > iThreshold || iNatureYield >= iThreshold)) // advc.908a
	{
		iYieldRate += GC.getDefineINT(CvGlobals::EXTRA_YIELD);
	} // K-Mod end

	//return iYieldRate;
	return std::max(0, iYieldRate); // K-Mod - so that it matches calculateYield()
}


bool CvPlot::canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer) const
{
	FAssert(GC.getInfo(eTrigger).isPlotEventTrigger());

	CvEventTriggerInfo& kTrigger = GC.getInfo(eTrigger);
	if (kTrigger.isOwnPlot() && getOwner() != ePlayer)
		return false;

	if (kTrigger.getPlotType() != NO_PLOT)
	{
		if (getPlotType() != kTrigger.getPlotType())
			return false;
	}

	if (kTrigger.getNumFeaturesRequired() > 0)
	{
		bool bFoundValid = false;
		for (int i = 0; i < kTrigger.getNumFeaturesRequired(); ++i)
		{
			if (kTrigger.getFeatureRequired(i) == getFeatureType())
			{
				bFoundValid = true;
				break;
			}
		}
		if (!bFoundValid)
			return false;
	}
	if (kTrigger.getNumTerrainsRequired() > 0)
	{
		bool bFoundValid = false;
		for (int i = 0; i < kTrigger.getNumTerrainsRequired(); ++i)
		{
			if (kTrigger.getTerrainRequired(i) == getTerrainType())
			{
				bFoundValid = true;
				break;
			}
		}
		if (!bFoundValid)
			return false;
	}
	if (kTrigger.getNumImprovementsRequired() > 0)
	{
		bool bFoundValid = false;
		for (int i = 0; i < kTrigger.getNumImprovementsRequired(); ++i)
		{
			if (kTrigger.getImprovementRequired(i) == getImprovementType())
			{
				bFoundValid = true;
				break;
			}
		}
		if (!bFoundValid)
			return false;
	}
	if (kTrigger.getNumBonusesRequired() > 0)
	{
		bool bFoundValid = false;
		for (int i = 0; i < kTrigger.getNumBonusesRequired(); ++i)
		{
			if (kTrigger.getBonusRequired(i) == getBonusType(kTrigger.isOwnPlot() ?
				GET_PLAYER(ePlayer).getTeam() : NO_TEAM))
			{
				bFoundValid = true;
				break;
			}
		}
		if (!bFoundValid)
			return false;
	}
	if (kTrigger.getNumRoutesRequired() > 0)
	{
		bool bFoundValid = false;
		if (getPlotCity() == NULL)
		{
			for (int i = 0; i < kTrigger.getNumRoutesRequired(); ++i)
			{
				if (kTrigger.getRouteRequired(i) == getRouteType())
				{
					bFoundValid = true;
					break;
				}
			}
		}
		if (!bFoundValid)
			return false;
	}
	if (kTrigger.isUnitsOnPlot())
	{
		bool bFoundValid = false;
		FOR_EACH_UNIT_IN(pUnit, *this)
		{
			if (pUnit->getOwner() == ePlayer &&
				pUnit->getTriggerValue(eTrigger, this, false) != -1)
			{
				bFoundValid = true;
				break;
			}
		}
		if (!bFoundValid)
			return false;
	}
	if (kTrigger.isPrereqEventCity() && kTrigger.getNumPrereqEvents() > 0)
	{
		bool bFoundValid = true;
		for (int iI = 0; iI < kTrigger.getNumPrereqEvents(); ++iI)
		{
			const EventTriggeredData* pTriggeredData = GET_PLAYER(ePlayer).
					getEventOccured((EventTypes)kTrigger.getPrereqEvent(iI));
			if (NULL == pTriggeredData || pTriggeredData->m_iPlotX != getX() ||
				pTriggeredData->m_iPlotY != getY())
			{
				bFoundValid = false;
				break;
			}
		}
		if (!bFoundValid)
			return false;
	}

	return true;
}


bool CvPlot::canApplyEvent(EventTypes eEvent) const
{
	CvEventInfo& kEvent = GC.getInfo(eEvent);
	if (kEvent.getFeatureChange() > 0)
	{
		if (kEvent.getFeature()!= NO_FEATURE)
		{
			if (isImproved() || !canHaveFeature((FeatureTypes)kEvent.getFeature()))
				return false;
		}
	}
	else if (kEvent.getFeatureChange() < 0)
	{
		if (!isFeature())
			return false;
	}
	if (kEvent.getImprovementChange() > 0)
	{
		if (kEvent.getImprovement() != NO_IMPROVEMENT)
		{
			if (!canHaveImprovement((ImprovementTypes)kEvent.getImprovement(), getTeam()))
				return false;
		}
	}
	else if (kEvent.getImprovementChange() < 0)
	{
		if (!isImproved())
			return false;
	}
	if (kEvent.getBonusChange() > 0)
	{
		if (kEvent.getBonus() != NO_BONUS)
		{
			if (!canHaveBonus((BonusTypes)kEvent.getBonus(), false))
				return false;
		}
	}
	else if (kEvent.getBonusChange() < 0)
	{
		if (getBonusType() == NO_BONUS)
			return false;
	}
	if (kEvent.getRouteChange() < 0)
	{
		if (!isRoute())
			return false;

		if (isCity())
			return false;
	}

	return true;
}


void CvPlot::applyEvent(EventTypes eEvent)
{
	CvEventInfo& kEvent = GC.getInfo(eEvent);
	if (kEvent.getFeatureChange() > 0)
	{
		if (NO_FEATURE != kEvent.getFeature())
			setFeatureType((FeatureTypes)kEvent.getFeature());
	}
	else if (kEvent.getFeatureChange() < 0)
		setFeatureType(NO_FEATURE);

	if (kEvent.getImprovementChange() > 0)
	{
		if (NO_IMPROVEMENT != kEvent.getImprovement())
			setImprovementType((ImprovementTypes)kEvent.getImprovement());
	}
	else if (kEvent.getImprovementChange() < 0)
		setImprovementType(NO_IMPROVEMENT);

	if (kEvent.getBonusChange() > 0)
	{
		if (NO_BONUS != kEvent.getBonus())
			setBonusType((BonusTypes)kEvent.getBonus());
	}
	else if (kEvent.getBonusChange() < 0)
		setBonusType(NO_BONUS);

	if (kEvent.getRouteChange() > 0)
	{
		if (NO_ROUTE != kEvent.getRoute())
			setRouteType((RouteTypes)kEvent.getRoute(), true);
	}
	else if (kEvent.getRouteChange() < 0)
		setRouteType(NO_ROUTE, true);

	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		int iChange = kEvent.getPlotExtraYield(i);
		if (iChange != 0)
			GC.getGame().setPlotExtraYield(m_iX, m_iY, (YieldTypes)i, iChange);
	}
}


bool CvPlot::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible,
	bool bCheckAirUnitCap, // advc.001b
	BonusTypes eAssumeAvailable) const // advc.001u
{
	CvCity const* pCity = getPlotCity();
/************************************************************************************************/
/* City Size Prerequisite - 3 Jan 2012     START                                OrionVeteran    */
/************************************************************************************************/
	if (GC.getInfo(eUnit).getNumCitySizeUnitPrereq() > 0)
	{
		if (NULL == pCity || pCity->getPopulation() < GC.getInfo(eUnit).getNumCitySizeUnitPrereq())
		{
			return false;
		}
	}
/************************************************************************************************/
/* City Size Prerequisite                  END                                                  */
/************************************************************************************************/
	bool const bCity = (pCity != NULL);
	CvUnitInfo const& kUnit = GC.getInfo(eUnit);
	if (kUnit.isPrereqReligion())
	{
		if (!bCity || //pCity->getReligionCount() > 0)
			pCity->getReligionCount() == 0) // K-Mod
		{
			return false;
		}
	}
	if (kUnit.getPrereqReligion() != NO_RELIGION)
	{
		if (!bCity || !pCity->isHasReligion(kUnit.getPrereqReligion()))
			return false;
	}
	if (kUnit.getPrereqCorporation() != NO_CORPORATION)
	{
		if (!bCity || !pCity->isActiveCorporation(kUnit.getPrereqCorporation()))
			return false;
	}

	if (kUnit.isPrereqBonuses())
	{
		if (kUnit.getDomainType() == DOMAIN_SEA)
		{	// advc: Moved to CvCity
			if (bCity && !pCity->isPrereqBonusSea())
				return false;
		}
		else
		{
			//if (getArea().getNumTotalBonuses() > 0)
			if(!getArea().isAnyBonus()) // advc.001
				return false;
		}
	}
	/*if (isCity()) {
		// ...
	}
	else if (getArea().getNumTiles() < kUnit.getMinAreaSize())
		return false;
	/*  <advc.041> Replacing the above (moved into CvCityAI::AI_bestUnitAI). I.e.
		treat MinAreaSize as a mere recommendation for the AI rather than a game rule.
		NB: MinAreaSize is still enforced as a rule for buildings.
		The last clause above should perhaps be checked by the AI before
		upgrading units; but AI sea units can end up in small water areas only via
		WorldBuilder [or possibly teleport -> advc.046], so I'm not bothering with this. */
	if (bCity)
	{
		/*  Don't allow any ships to be trained at lakes, except Work Boat
			(if there are resources in the lake; already checked above). */
		if (kUnit.getDomainType() == DOMAIN_SEA && !kUnit.isPrereqBonuses() &&
			!isAdjacentSaltWater())
		{
			return false;
		}
	}
	else // </advc.041>
	{
		if (kUnit.getDomainType() == DOMAIN_SEA)
		{
			if (!isWater())
				return false;
		}
		else if (kUnit.getDomainType() == DOMAIN_LAND)
		{
			if (isWater())
				return false;
		}
		else // advc (comment): Upgrade air units only in cities
			return false;
	}

	if (bTestVisible)
		return true;

	if (kUnit.getHolyCity() != NO_RELIGION)
	{
		if (!bCity || !pCity->isHolyCity(kUnit.getHolyCity()))
			return false;
	}

	if (kUnit.getPrereqBuilding() != NO_BUILDING)
	{
		if (!bCity)
			return false;
		BuildingTypes ePrereqBuilding = kUnit.getPrereqBuilding();
		if (pCity->getNumBuilding(ePrereqBuilding) == 0)
		{
			SpecialBuildingTypes eSpecialBuilding = GC.getInfo(ePrereqBuilding).getSpecialBuildingType();
			if (eSpecialBuilding == NO_SPECIALBUILDING ||
				!GET_PLAYER(getOwner()).isSpecialBuildingNotRequired(eSpecialBuilding))
			{
				return false;
			}
		}
	}
	BonusTypes ePrereqAndBonus = kUnit.getPrereqAndBonus();
	if(ePrereqAndBonus != NO_BONUS &&
		ePrereqAndBonus != eAssumeAvailable) // advc.001u
	{
		if (!bCity)
		{
			if (!isPlotGroupConnectedBonus(getOwner(), ePrereqAndBonus))
				return false;
		}
		else if (!pCity->hasBonus(ePrereqAndBonus))
			return false;
	}

	bool bRequiresBonus = false;
	bool bNeedsBonus = true;
	for (int i = 0; i < kUnit.getNumPrereqOrBonuses(); i++)
	{
		BonusTypes const ePrereqOrBonus = kUnit.getPrereqOrBonuses(i);
		if (ePrereqOrBonus != eAssumeAvailable) // advc.001u
		{
			bRequiresBonus = true;
			if (bCity)
			{
				if (pCity->hasBonus(ePrereqOrBonus))
				{
					bNeedsBonus = false;
					break;
				}
			}
			else if (isPlotGroupConnectedBonus(getOwner(), ePrereqOrBonus))
			{
				bNeedsBonus = false;
				break;
			}
		}
	}
	if (bRequiresBonus && bNeedsBonus)
		return false;
//Shqype Vicinity Bonus Start
/*		if (GC.getUnitInfo(eUnit).getPrereqVicinityBonus() != NO_BONUS)
		{
			if (NULL == pCity)
			{
				if (!isHasValidBonus())
				//if (!isPlotGroupConnectedBonus(getOwner(), (BonusTypes)GC.getUnitInfo(eUnit).getPrereqVicinityBonus()))
				{
					return false;
				}
			}
			else
			{
				if (GC.getUnitInfo(eUnit).getPrereqVicinityBonus() != NO_BONUS)
				{
					for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
					{
						CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);
						if (pLoopPlot->getBonusType() == GC.getUnitInfo(eUnit).getPrereqVicinityBonus())
						{
							CvCity* pCity = GC.getMap().findCity(getX(), getY(), getOwner(), NO_TEAM, false);
							if (pLoopPlot->isHasValidBonus() && pLoopPlot->isConnectedTo(pCity))
							{
								return true;
							}
						}
					}
					return false;
				}
			}
		}
		
		bRequiresBonus = false;
		bNeedsBonus = true;

		for (int iI = 0; iI < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); ++iI)
		{
			if (GC.getUnitInfo(eUnit).getPrereqOrVicinityBonuses(iI) != NO_BONUS)
			{
				bRequiresBonus = true;

				if (NULL == pCity)
				{
					if (!isHasValidBonus())
					{
						bNeedsBonus = false;
						break;
					}
				}
				else
				{
					for (int iJ = 0; iJ < NUM_CITY_PLOTS; ++iJ)
					{
						CvPlot* pLoopPlot = plotCity(getX(), getY(), iJ);
						if (pLoopPlot->getBonusType() == GC.getUnitInfo(eUnit).getPrereqOrVicinityBonuses(iI))
						{
							CvCity* pCity = GC.getMap().findCity(getX(), getY(), getOwner(), NO_TEAM, false);
							if (pLoopPlot->isHasValidBonus() && pLoopPlot->isConnectedTo(pCity))
							{
								bNeedsBonus = false;
								return true;
							}
						}
					}
					bNeedsBonus = true;
				}
			}
		}

		if (bRequiresBonus && bNeedsBonus)
		{
			return false;
		}*/
//Shqype Vicinity Bonus End
	// <advc.001b>
	if (bCheckAirUnitCap &&
		GC.getDefineBOOL(CvGlobals::CAN_TRAIN_CHECKS_AIR_UNIT_CAP) &&
		airUnitSpaceAvailable(getTeam()) < kUnit.getAirUnitCap())
	{
		return false;
	} // </advc.001b>

	return true;
}

/*	advc: Replacing CvCity::isValidBuildingLocation.
	Body cut from there (incl. the comment). */
bool CvPlot::canConstruct(BuildingTypes eBuilding) const
{
	CvBuildingInfo const& kBuilding = GC.getInfo(eBuilding);
	/*	If both the river and water flags are set, then
		we require one of the two conditions, not both. */
	if (kBuilding.isWater())
	{
		if (!kBuilding.isRiver() || !isRiver())
		{
			if (!isCoastalLand(kBuilding.getMinAreaSize()))
				return false;
		}
	}
	else
	{
		if (getArea().getNumTiles() < kBuilding.getMinAreaSize())
			return false;
		if (kBuilding.isRiver())
		{
			if (!isRiver())
				return false;
		}
	}
	return true;
}


int CvPlot::countFriendlyCulture(TeamTypes eTeam) const
{
	int iTotal = 0;
	for (PlayerIter<ALIVE> itPlayer; itPlayer.hasNext(); ++itPlayer)
	{
		if (GET_TEAM(eTeam).canPeacefullyEnter(itPlayer->getTeam()))
			iTotal += getCulture(itPlayer->getID());
	}
	/*	<advc.099> Count defeated teammates as friendly, but don't rely on
		any other relations info to be preserved posthumously. */
	for (PlayerIter<EVER_ALIVE> itDeadPlayer; itDeadPlayer.hasNext(); ++itDeadPlayer)
	{
		if (!itDeadPlayer->isAlive() && itDeadPlayer->getTeam() == eTeam)
			iTotal += getCulture(itDeadPlayer->getID());
	} // </advc.099>
	return iTotal;
}


int CvPlot::countNumAirUnits(TeamTypes eTeam) const
{
	int iCount = 0;
	FOR_EACH_UNIT_IN(pUnit, *this)
	{
		if (DOMAIN_AIR == pUnit->getDomainType() && !pUnit->isCargo() &&
			pUnit->getTeam() == eTeam)
		{
			iCount += GC.getInfo(pUnit->getUnitType()).getAirUnitCap();
		}
	}
	return iCount;
}


int CvPlot::airUnitSpaceAvailable(TeamTypes eTeam) const
{
	int iMaxUnits = 0;
	CvCity* pCity = getPlotCity();
	if (pCity != NULL)
		iMaxUnits = pCity->getAirUnitCapacity(getTeam());
	else iMaxUnits = GC.getDefineINT(CvGlobals::CITY_AIR_UNIT_CAPACITY);
	return iMaxUnits - countNumAirUnits(eTeam);
}

// advc.081: Cut from CvPlayerAI::AI_countNumAreaHostileUnits
int CvPlot::countHostileUnits(PlayerTypes ePlayer, bool bPlayer, bool bTeam,
	bool bNeutral, bool bHostile) const
{
	TeamTypes eTeam = TEAMID(ePlayer);
	if(!isVisible(eTeam))
		return 0;
	if((bPlayer && getOwner() == ePlayer) ||
		(bTeam && getTeam() == eTeam) || (bNeutral && !isOwned()) ||
		(bHostile && isOwned() && GET_TEAM(eTeam).isAtWar(getTeam())))
	{
		return plotCount(PUF_isEnemy, ePlayer, false, NO_PLAYER, NO_TEAM,
				PUF_isVisible, ePlayer);
	}
	return 0;
}


bool CvPlot::isEspionageCounterSpy(TeamTypes eTeam) const
{
	CvCity const* pPlotCity = getPlotCity();
	if (pPlotCity != NULL && pPlotCity->getTeam() == eTeam)
	{
		if (pPlotCity->getEspionageDefenseModifier() > 0)
			return true;
	}
	// advc.opt: was plotCount... > 0
	return (plotCheck(PUF_isCounterSpy, -1, -1, NO_PLAYER, eTeam) != NULL);
}


int CvPlot::getAreaIdForGreatWall() const
{
	return areaID();
}


int CvPlot::getSoundScriptId() const
{
	int iScriptId = -1;
	if (isActiveVisible(true))
	{
		if (getImprovementType() != NO_IMPROVEMENT)
			iScriptId = GC.getInfo(getImprovementType()).getWorldSoundscapeScriptId();
		else if (isFeature())
			iScriptId = GC.getInfo(getFeatureType()).getWorldSoundscapeScriptId();
		else if (getTerrainType() != NO_TERRAIN)
			iScriptId = GC.getInfo(getTerrainType()).getWorldSoundscapeScriptId();
	}
	return iScriptId;
}


int CvPlot::get3DAudioScriptFootstepIndex(int iFootstepTag) const
{
	if (isFeature())
		return GC.getInfo(getFeatureType()).get3DAudioScriptFootstepIndex(iFootstepTag);

	if (getTerrainType() != NO_TERRAIN)
		return GC.getInfo(getTerrainType()).get3DAudioScriptFootstepIndex(iFootstepTag);

	return -1;
}


float CvPlot::getAqueductSourceWeight() const
{
	PROFILE_FUNC(); // (advc: Fine - EXE seems to call this only a few times per turn)
	// <advc.002p> Imperfect workaround for a missing same-area check in the EXE
	for (CityPlotIter itPlot(*this, false); itPlot.hasNext(); ++itPlot)
	{	/*	Sources adjacent to a city are OK (and need to be skipped here
			because an adjacent Peak could belong to a different area
			- because of change advc.030) */
		if (itPlot.currID() <= NUM_INNER_PLOTS)
			continue;
		if (itPlot->isCity() && !itPlot->isArea(getArea()))
		{
			static BuildingClassTypes const eAqueduct = (BuildingClassTypes)
					GC.getInfoTypeForString("BUILDINGCLASS_AQUEDUCT", true);
			if (eAqueduct != NO_BUILDINGCLASS &&
				itPlot->getPlotCity()->getNumBuilding(eAqueduct) > 0)
			{
				return -1;
			}
		}
	} // </advc.002p>
	float fWeight = 0.0f;
	// Deliverator - changed next line fresh water fresh water
	if (isLake() || isPeak() || (getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(getImprovementType()).getAddsFreshWaterInRadius() >= 0)
	//keldath added from bts
		|| isFeature() && GC.getInfo(getFeatureType()).isAddsFreshWater())
	{
		fWeight = 1.0f;
	}
	else if (isHills())
		fWeight = 0.67f;

	return fWeight;
}


bool CvPlot::shouldDisplayBridge(CvPlot* pToPlot, PlayerTypes ePlayer) const
{
	TeamTypes const eObservingTeam = TEAMID(ePlayer);
	TeamTypes const eOurTeam = getRevealedTeam(eObservingTeam, true);
	TeamTypes eOtherTeam = NO_TEAM;
	if (pToPlot != NULL)
		eOtherTeam = pToPlot->getRevealedTeam(eObservingTeam, true);

	if (eOurTeam == eObservingTeam || eOtherTeam == eObservingTeam ||
		(eOurTeam == NO_TEAM && eOtherTeam == NO_TEAM))
	{
		return GET_TEAM(eObservingTeam).isBridgeBuilding();
	}

	if (eOurTeam == NO_TEAM)
		return GET_TEAM(eOtherTeam).isBridgeBuilding();

	if (eOtherTeam == NO_TEAM)
		return GET_TEAM(eOurTeam).isBridgeBuilding();

	return (GET_TEAM(eOurTeam).isBridgeBuilding() && GET_TEAM(eOtherTeam).isBridgeBuilding());
}


bool CvPlot::checkLateEra() const
{
	PlayerTypes eBestPlayer = getOwner();
	if (eBestPlayer == NO_PLAYER)
	{
		eBestPlayer = getActivePlayer();
		int iBestCulture = getCulture(eBestPlayer);
		// advc: ALIVE - shouldn't rely on era of dead players
		for (PlayerIter<ALIVE> itPlayer; itPlayer.hasNext(); ++itPlayer)
		{
			int iLoopCulture = getCulture(itPlayer->getID());
			if (iLoopCulture > iBestCulture)
			{
				iBestCulture = iLoopCulture;
				eBestPlayer = itPlayer->getID();
			}
		}
	}
	return (GET_PLAYER(eBestPlayer).getCurrentEra() * 2 > GC.getNumEraInfos());
}

// advc.300:
void CvPlot::killRandomUnit(PlayerTypes eOwner, DomainTypes eDomain)
{
	CvUnit* pVictim = NULL;
	int iBestValue = -1;
	FOR_EACH_UNIT_VAR_IN(pUnit, *this)
	{
		if (pUnit->getOwner() == eOwner && pUnit->getDomainType() == eDomain)
		{
			int iValue = SyncRandNum(1000);
			if (iValue > iBestValue)
			{
				pVictim = pUnit;
				iBestValue = iValue;
			}
		}
	}
	if (pVictim != NULL)
		pVictim->kill(false);
}


// BETTER_BTS_AI_MOD, Lead From Behind (UncutDragon), 02/21/10, jdog5000: START
bool CvPlot::hasDefender(bool bTestCanAttack, PlayerTypes eOwner, PlayerTypes eAttackingPlayer,
	CvUnit const* pAttacker, bool bTestEnemy, bool bTestPotentialEnemy) const
{
	/*  advc: BBAI had repeated parts of getBestDefender here. To avoid that, I've moved
		bTestAttack into getBestDefender and gave that function a "bTestAny" param. */
	return (getBestDefender(eOwner, eAttackingPlayer, pAttacker, bTestEnemy,
			bTestPotentialEnemy, false, bTestCanAttack, /*bTestAny=*/true) != NULL);
} // BETTER_BTS_AI_MOD: END

// <advc.500a>
#if 0 // disabled for now
bool CvPlot::isConnectRiverSegments() const
{
	bool cr[NUM_DIRECTION_TYPES];
	for(int i = 0; i < NUM_DIRECTION_TYPES; i++) {
		DirectionTypes dir = (DirectionTypes)i;
		cr[i] = isRiverCrossing(dir);
	}
	/*  River crossings in two directions and no connection between the
		two segments along this tile */
	return ((cr[DIRECTION_WEST] && cr[DIRECTION_EAST] &&
			    !cr[DIRECTION_NORTH] && !cr[DIRECTION_SOUTH]) ||
		   (cr[DIRECTION_NORTH] && cr[DIRECTION_SOUTH] &&
				!cr[DIRECTION_WEST] && !cr[DIRECTION_EAST]) ||
		   (cr[DIRECTION_SOUTHWEST] && cr[DIRECTION_NORTHEAST] &&
			    !(cr[DIRECTION_WEST] && cr[DIRECTION_NORTH]) &&
				!(cr[DIRECTION_EAST] && cr[DIRECTION_SOUTH])) ||
		   (cr[DIRECTION_SOUTHEAST] && cr[DIRECTION_NORTHWEST] &&
			    !(cr[DIRECTION_WEST] && cr[DIRECTION_SOUTH]) &&
				!(cr[DIRECTION_EAST] && cr[DIRECTION_NORTH])) ||
		   (cr[DIRECTION_NORTHWEST] && cr[DIRECTION_NORTHEAST] &&
				!cr[DIRECTION_NORTH] && !cr[DIRECTION_SOUTH]) ||
		   (cr[DIRECTION_SOUTHWEST] && cr[DIRECTION_NORTHWEST] &&
				!cr[DIRECTION_WEST] && !cr[DIRECTION_EAST]) ||
		   (cr[DIRECTION_SOUTHWEST] && cr[DIRECTION_SOUTHEAST] &&
				!cr[DIRECTION_SOUTH] && !cr[DIRECTION_NORTH]) ||
		   (cr[DIRECTION_SOUTHEAST] && cr[DIRECTION_NORTHEAST] &&
				!cr[DIRECTION_EAST] && !cr[DIRECTION_WEST]));

}
#endif // </advc.500a>

// <advc.121>
bool CvPlot::isConnectSea() const
{
	if(isWater())
		return false;
	/* Circle through the adjacent plots clockwise, looking for changes
	   from land to sea or vice versa. */
	static DirectionTypes eClockwise[NUM_DIRECTION_TYPES] = {
		DIRECTION_NORTH, DIRECTION_NORTHEAST, DIRECTION_EAST,
		DIRECTION_SOUTHEAST, DIRECTION_SOUTH, DIRECTION_SOUTHWEST,
		DIRECTION_WEST, DIRECTION_NORTHWEST
	};
	bool bPreviousSea = false;
	int iChanges = 0;
	for(int i = 0; i < NUM_DIRECTION_TYPES; i++)
	{
		CvPlot* pPlot = plotDirection(getX(), getY(), eClockwise[i]);
		if(pPlot == NULL)
			continue;
		bool bSea = pPlot->isWater() && !pPlot->isLake();
		if(bSea != bPreviousSea && i > 0)
			iChanges++;
		if(iChanges > 2)
			return true;
		bPreviousSea = bSea;
	}
	return false;
	/* Checking for a connection between different water areas would be easy enough
	   to do, but shortening paths within a water area can also be valuable. */
} // </advc.121>

/*  advc.031c: For found value log; but could also find other uses, possibly through
	optional call parameters. */
wchar const* CvPlot::debugStr() const
{
	static std::wostringstream out; // Perhaps not needed; safer this way?
	out.str(L"");
	if (isCity())
	{
		out << getPlotCity()->getName().c_str();
		return out.str().c_str();
	}
	out << L"[" << getX() << L"," << getY() << L"]";
	if (isPeak())
	{
		out << L" " << L"Peak";
		return out.str().c_str();
	}
	if (getBonusType() != NO_BONUS)
		out << L" " << GC.getInfo(getBonusType()).getDescription();
	if (isRiver())
		out << L" River";
	out << L" " << GC.getInfo(getTerrainType()).getDescription();
	if (isHills())
		out << L" Hill";
	if (isFeature())
		out << L" " << GC.getInfo(getFeatureType()).getDescription();
	if (isOwned())
		out << L" (" << GET_PLAYER(getOwner()).getCivilizationShortDescription() << L")";
	return out.str().c_str();
}
//Shqype Vicinity Bonus Start
/*bool CvPlot::isHasValidBonus() const
{
	if (getBonusType() == NO_BONUS)
	{
		return false;
	}
	if (getImprovementType() == NO_IMPROVEMENT)
	{
		return false;
	}
	if (!isBonusNetwork(getTeam()))
	{
		return false;
	}
	if (!isWithinTeamCityRadius(getTeam()))
	{
		return false;
	}
	if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes)m_eBonusType).getTechReveal())))
	{
        if (GC.getImprovementInfo(getImprovementType()).isImprovementBonusMakesValid(getBonusType()))
		{
			return true;
		}
	}

	return false;
}*/
//Shqype Vicinity Bonus End
//MOD@VET_Andera412_Blocade_Unit-begin1/1
bool CvPlot::isWithBlocaders(const CvPlot* pFromPlot, const CvPlot* pToPlot, const CvUnit* const pUnit, bool bToWater) const //
{
	if (GC.getGame().isOption(GAMEOPTION_BLOCADE_UNIT))
	{	
		if (isWater() == bToWater)
		{
			TeamTypes eTeam = pUnit->getTeam();
			if (!isVisible(eTeam, false))
			{
				//doto advc adjustment 099
				if (/*doto keldath trial : pUnit->isEnemyCity(*pToPlot)*/ 
					/* original : isEnemyCity(*pUnit)*/ 
					/*f1rpo suggestion:*/pUnit->isEnemyCity(*this) /*&& getPlotCity()->isRevealed(pUnit->getTeam())*/
					&& (pUnit->getDomainType() == DOMAIN_LAND) && (pUnit->getInvisibleType() == NO_INVISIBLE)) // 
				{
					if (!isRiverCrossing(directionXY(*this, *pToPlot)))
						return true;
				}
				return false;
			}
			CLLNode<IDInfo>* pUnitNode = headUnitNode();
			if (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit;
				CvTeam& kTeam = GET_TEAM(eTeam);
				TeamTypes eLoopTeam = NO_TEAM;
				TeamTypes eTempTeam;
				InvisibleTypes eOurInvisible = pUnit->getInvisibleType();
				//bool bDifWater = (isWater() != pToPlot->isWater());
//doto keldath - attempt to try and fix an assert err
				bool bNotRiverCrossing = false;//!isRiverCrossing(directionXY(*this, *pToPlot));
				if (!pToPlot->isWater() && pUnit->getDomainType() == DOMAIN_LAND)
				{
					bNotRiverCrossing = isRiverCrossing(directionXY(*this, *pToPlot));
				}
				bool bHiddenNationality = pUnit->getUnitInfo().isHiddenNationality();
				bool bTeam, bDefenders, bOurVisible, bEnemyVisible;
				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					if (!pLoopUnit->isAnimal())
					{
						eTempTeam = pLoopUnit->getTeam();
						/*if (eLoopTeam != eTempTeam)//
						{
							eLoopTeam = eTempTeam;
							bDefenders = false;
							bOurVisible = false;
							bEnemyVisible = false;
						}*/
						eLoopTeam = eTempTeam;
						bTeam = (eLoopTeam == eTempTeam);
						bDefenders = bDefenders & (bTeam);
						bOurVisible = bOurVisible & (bTeam);
						bEnemyVisible = bEnemyVisible & (bTeam);
						if ((eLoopTeam != eTeam) && (kTeam.isAtWar(eLoopTeam) || pLoopUnit->getUnitInfo().isHiddenNationality() | bHiddenNationality))/** && !pUnit->isInvisible(eLoopTeam, false)*/
						{
							if ((pLoopUnit->canFight()) && !pLoopUnit->cannotMoveFromTo(this, pToPlot) && (pLoopUnit->getTransportUnit() == NULL) &&
								(bNotRiverCrossing || pLoopUnit->isRiver()))
								{bDefenders = true;}
							if ((eOurInvisible == NO_INVISIBLE) || pFromPlot->isInvisibleVisible(eLoopTeam, eOurInvisible))
								{bOurVisible = true;}
							if ((pLoopUnit->getInvisibleType() == NO_INVISIBLE) || isInvisibleVisible(eTeam, pLoopUnit->getInvisibleType()))
								{bEnemyVisible = true;}
						}
						if (bDefenders & bOurVisible & bEnemyVisible)
							{return true;}
					}
					pUnitNode = nextUnitNode(pUnitNode);
				}
			}
		}
	}	
	return false;
}

bool CvPlot::isBlocade(const CvPlot* pFromPlot, const CvUnit* const pUnit) const//(CvGameCoreUtils::pathValid)
{
	if (GC.getGame().isOption(GAMEOPTION_BLOCADE_UNIT))
	{
		//doto change to avoid errors in scenarios with no settler or worker
		//see advc comment on BUILDINGCLASS_AIRPORT in cvgametextmgr.cpp file
		bool bHideAssert = false;
		if (GC.getGame().isOption(GAMEOPTION_SCENARIO_PLAY))
		{
			bHideAssert = true;
		}
		if (pUnit->isAnimal() || pUnit->alwaysInvisible() || pUnit->isUnblocade()
//DOTO = ADDED BY KELDATH setllers and workers should be affected.
			|| pUnit->getUnitClassType() == GC.getInfoTypeForString("UNITCLASS_SETTLER", bHideAssert)
			|| pUnit->getUnitClassType() == GC.getInfoTypeForString("UNITCLASS_WORKER", bHideAssert)
			)
			{return false;}
		//keldath 099 advc adjustment - prev it checked also improvement?
		if (/*keldath trial: !pUnit->isPlotValid(pUnit->getPlot()) */
			/*f1rpo suggestion */
			isOwned() && !GET_TEAM(pUnit->getTeam()).isCityDefense(*this)
			/*consider this : CvTeam::isCityHeal(CvPlot const& kPlot)*/
			/*original : !isFriendlyCity(*pUnit, true)*/)
		{
			TeamTypes eOurTeam = pUnit->getTeam();
			bool bPlotWithoutOurUnit_Wrap = true;
			//keldath qa5 - fix usage of calls from globals.
			//bool bPlotWithoutEnemy = (GC.getBLOCADE_UNIT() > 1);//bPlotWithoutEnemy = true//(bPlotWithoutEnemy && (GC.getBLOCADE_UNIT() > 1))
			bool bPlotWithoutEnemy = GC.getDefineINT(CvGlobals::BLOCADE_UNIT) > 1;
			if (isVisible(eOurTeam, false))
			{
				bool bOurTeam;
				CLLNode<IDInfo>* pUnitNode = headUnitNode();
				CvUnit* pLoopUnit;
				TeamTypes eLoopTeam;
				InvisibleTypes eInvisible;
				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					eLoopTeam = pLoopUnit->getTeam();
					bOurTeam = (eLoopTeam == eOurTeam);
					if (bOurTeam && pLoopUnit->canFight())//
					{
						bPlotWithoutOurUnit_Wrap = false;
						break;
					}
					else if ((GET_TEAM(eLoopTeam).isAtWar(eOurTeam) || ((!bOurTeam) & pUnit->getUnitInfo().isHiddenNationality())) && pLoopUnit->canFight())//
					{
						eInvisible = pLoopUnit->getInvisibleType();
						if ((eInvisible == NO_INVISIBLE) || isInvisibleVisible(eOurTeam, eInvisible))
						{
							bPlotWithoutEnemy = false;
							break;
						}
					}
					pUnitNode = nextUnitNode(pUnitNode);
				}
			}
			if (bPlotWithoutOurUnit_Wrap)
			{
				int iPlotX, iPlotY, iFromX, iFromY;
				CvMap& kMap = GC.getMap();

				iPlotX = getX();
				iPlotY = getY();
				iFromX = pFromPlot->getX();
				iFromY = pFromPlot->getY();
				///byte iI;
				if (iFromX == iPlotX)
				{
				/**	bPlotWithoutOurUnit_Wrap = kMap.isWrapX();
					iI = 0;
					goto A;*/

					if (bPlotWithoutEnemy)
					{
						int iX = iFromX;
						++iX;
						if (iX != kMap.getGridWidth())
						{
							if (kMap.plotSoren(iX, iPlotY)->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
								{return true;}
						}
						else
						{
							if (kMap.isWrapX())
							{
								iX = 0;
								if (kMap.plotSoren(iX, iPlotY)->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
									{return true;}
							}
						}

						iX = iFromX;
						--iX;
						if (iX != -1)
						{
							if (kMap.plotSoren(iX, iPlotY)->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
								{return true;}
						}
						else
						{
							if (kMap.isWrapX())
							{
								iX += kMap.getGridWidth();
								if (kMap.plotSoren(iX, iPlotY)->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
									{return true;}
							}
						}
					}
				}
				else if (iFromY == iPlotY)
				{
				/**	bPlotWithoutOurUnit_Wrap = kMap.isWrapY();
					iI = 1;
					goto A;*/

					if (bPlotWithoutEnemy)
					{
						int iY = iFromY;
						++iY;
						if (iY != kMap.getGridHeight())
						{
							if (kMap.plotSoren(iPlotX, iY)->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
								{return true;}
						}
						else
						{
							if (kMap.isWrapY())
							{
								iY = 0;
								if (kMap.plotSoren(iPlotX, iY)->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
									{return true;}
							}
						}
						iY = iFromY;
						--iY;
						if (iY != -1)
						{
							if (kMap.plotSoren(iPlotX, iY)->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
								{return true;}
						}
						else
						{
							if (kMap.isWrapY())
							{
								iY += kMap.getGridHeight();
								if (kMap.plotSoren(iPlotX, iY)->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
									{return true;}
							}
						}
					}
				}
				else if ((kMap.plotSoren(iFromX, iPlotY)->isWithBlocaders(pFromPlot, this, pUnit, isWater())) ||
						(kMap.plotSoren(iPlotX, iFromY)->isWithBlocaders(pFromPlot, this, pUnit, isWater())))
					{return true;}
			/**	return false;
	A:
				if (bPlotWithoutEnemy && (GC.getBLOCADE_UNIT() > 1))
				{
					int iSize;
					int iCoord;
					int aiXY[2];
					if (iI)// iI == 0
					{
						iSize = kMap.getGridWidth();
						aiXY[0] = iFromX;
						aiXY[1] = iPlotY;
					}
					else// iI == 1
					{
						iSize = kMap.getGridHeight();
						aiXY[0] = iPlotX;
						aiXY[1] = iFromY;
					}
					iCoord = aiXY[iI];

					aiXY[iI]++;
					if (aiXY[iI] == iSize)
					{
						if (bPlotWithoutOurUnit_Wrap)
						{
							aiXY[iI] = 0;
							if (kMap.getPlotSoren(aiXY[0], aiXY[1])->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
								{return true;}
						}
					}
					else if (kMap.getPlotSoren(aiXY[0], aiXY[1])->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
						{return true;}
					iCoord--;// aiXY[iI]--
					aiXY[iI] = iCoord;
					if (aiXY[iI] == -1)
					{
						if (bPlotWithoutOurUnit_Wrap)
						{
							aiXY[iI] += iSize;
							if (kMap.getPlotSoren(aiXY[0], aiXY[1])->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
								{return true;}
						}
					}
					else if (kMap.getPlotSoren(aiXY[0], aiXY[1])->isWithBlocaders(pFromPlot, this, pUnit, isWater()))
						{return true;}
				}*/
			}
		}
	}	
	return false;
}
//MOD@VET_Andera412_Blocade_Unit-end1/1