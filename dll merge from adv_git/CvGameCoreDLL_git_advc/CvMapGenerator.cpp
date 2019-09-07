#include "CvGameCoreDLL.h"
#include "CvMapGenerator.h"
#include "CvGameAI.h"
#include "CvMap.h"
#include "CvAreaList.h" // advc.003s
#include "CvFractal.h"
#include "CvInfo_Terrain.h"
#include "CvInfo_GameOption.h"


CvMapGenerator* CvMapGenerator::m_pInst = NULL; // static


CvMapGenerator& CvMapGenerator::GetInstance() // singleton accessor
{
	if (m_pInst==NULL)
		m_pInst = new CvMapGenerator;
	return *m_pInst;
}


CvMapGenerator::CvMapGenerator() {}


CvMapGenerator::~CvMapGenerator() {}


bool CvMapGenerator::canPlaceBonusAt(BonusTypes eBonus, int iX, int iY, bool bIgnoreLatitude)  // advc: style changes
{
	PROFILE_FUNC();

	CvMap& m = GC.getMap();
	CvPlot* pPlot = m.plot(iX, iY);
	if(pPlot == NULL)
		return false;
	CvArea* pArea = pPlot->area();

	if(!pPlot->canHaveBonus(eBonus, bIgnoreLatitude))
		return false;
	{
		bool bOverride=false;
		bool r = GC.getPythonCaller()->canPlaceBonusAt(*pPlot, bOverride);
		if (bOverride)
			return r;
	}
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++) {
		CvPlot* pLoopPlot = plotDirection(iX, iY, ((DirectionTypes)iI));
		if(pLoopPlot == NULL)
			continue;
		BonusTypes eLoopBonus = pLoopPlot->getBonusType();
		if(eLoopBonus != NO_BONUS && eLoopBonus != eBonus)
			return false;
	}

	CvBonusInfo& pInfo = GC.getBonusInfo(eBonus);
	CvBonusClassInfo& pClassInfo = GC.getBonusClassInfo((BonusClassTypes)
			pInfo.getBonusClassType());

	if(pPlot->isWater()) {
		if(((m.getNumBonusesOnLand(eBonus) * 100) / (m.getNumBonuses(eBonus) + 1)) <
				pInfo.getMinLandPercent())
			return false;
	}

	int const iRange = pClassInfo.getUniqueRange();
	for (int iDX = -iRange; iDX <= iRange; iDX++)
	for (int iDY = -iRange; iDY <= iRange; iDY++) {
		CvPlot* pLoopPlot = plotXY(iX, iY, iDX, iDY);
		if(pLoopPlot == NULL || pLoopPlot->area() != pArea)
			continue;
		if (plotDistance(iX, iY, pLoopPlot->getX(), pLoopPlot->getY()) <= iRange) {
			BonusTypes eOtherBonus = pLoopPlot->getBonusType();
			if(eOtherBonus == NO_BONUS)
				continue;
			// Make sure there are none of the same bonus nearby:
			if(eBonus == eOtherBonus)
				return false;
			// Make sure there are no bonuses of the same class nearby:
			if(GC.getBonusInfo(eOtherBonus).getBonusClassType() ==
					pInfo.getBonusClassType())
				return false;
		}
	}
	// <advc.129> Prevent more than one adjacent copy regardless of range.
	int iFound = 0;
	for(int i = 0; i < NUM_DIRECTION_TYPES; i++) {
		CvPlot* pp = plotDirection(iX, iY, (DirectionTypes)i);
		if(pp == NULL) continue; CvPlot const& p = *pp;
		if(p.area() != pArea)
			continue;
		if(p.getBonusType() == eBonus) {
			iFound++;
			if(iFound >= 2)
				return false;
			/*  A single adjacent copy could already have another adjacent copy.
				However, if that's prohibited, clusters of more than 2 resources
				won't be placed at all. (They're only placed around one central
				tile, which also gets the resource.) Better to change the placement
				pattern then (addUniqueBonusType). */
			/*for(int j = 0; j < NUM_DIRECTION_TYPES; j++) {
				CvPlot* pp2 = plotDirection(p.getX(), p.getY(),
						(DirectionTypes)j);
				if(pp2 == NULL) continue; CvPlot const& p2 = *pp2;
				if(p2.area() != pArea) continue;
				if(p2.getBonusType() == eBonus)
					return false;
			}*/
		}
	} // </advc.129>

	return true;
}


bool CvMapGenerator::canPlaceGoodyAt(ImprovementTypes eImprovement, int iX, int iY)
{
	PROFILE_FUNC();

	FAssertMsg(eImprovement != NO_IMPROVEMENT, "Improvement is not assigned a valid value");
	FAssertMsg(GC.getImprovementInfo(eImprovement).isGoody(), "ImprovementType eImprovement is expected to be a goody");

	if (GC.getGame().isOption(GAMEOPTION_NO_GOODY_HUTS))
		return false;

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (!pPlot->canHaveImprovement(eImprovement, NO_TEAM))
		return false;
	{
		bool bOverride=false;
		bool r = GC.getPythonCaller()->canPlaceGoodyAt(*pPlot, bOverride);
		if (bOverride)
			return r;
	}
	if (pPlot->getImprovementType() != NO_IMPROVEMENT || pPlot->getBonusType() != NO_BONUS ||
			pPlot->isImpassable())
		return false;

	int iUniqueRange = GC.getImprovementInfo(eImprovement).getGoodyUniqueRange();
	for (int iDX = -iUniqueRange; iDX <= iUniqueRange; iDX++)
	{
		for (int iDY = -iUniqueRange; iDY <= iUniqueRange; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(iX, iY, iDX, iDY);
			if (pLoopPlot != NULL && pLoopPlot->getImprovementType() == eImprovement)
				return false;
		}
	}

	return true;
}


void CvMapGenerator::addGameElements()
{
	addRivers();
	gDLL->logMemState("CvMapGen after add rivers");

	addLakes();
	gDLL->logMemState("CvMapGen after add lakes");

	addFeatures();
	gDLL->logMemState("CvMapGen after add features");

	addBonuses();
	gDLL->logMemState("CvMapGen after add bonuses");

	addGoodies();
	gDLL->logMemState("CvMapGen after add goodies");

	// Call for Python to make map modifications after it's been generated
	afterGeneration();
}


void CvMapGenerator::addLakes()
{
	PROFILE_FUNC();

	if (GC.getPythonCaller()->addLakes())
		return;

	gDLL->NiTextOut("Adding Lakes...");
	int const iLAKE_PLOT_RAND = GC.getDefineINT("LAKE_PLOT_RAND"); // advc.opt
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		//gDLL->callUpdater(); // advc.opt: Not needed I reckon
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);
		FAssertMsg(pLoopPlot != NULL, "LoopPlot is not assigned a valid value");

		if (!pLoopPlot->isWater())
		{
			if (!pLoopPlot->isCoastalLand())
			{
				if (!pLoopPlot->isRiver())
				{
					if (GC.getGame().getMapRandNum(iLAKE_PLOT_RAND, "addLakes") == 0)
					{
						pLoopPlot->setPlotType(PLOT_OCEAN);
					}
				}
			}
		}
	}
}

void CvMapGenerator::addRivers()  // advc: Refactored
{
	PROFILE_FUNC();

	if (GC.getPythonCaller()->addRivers())
		return;

	gDLL->NiTextOut("Adding Rivers...");

	int const iRiverSourceRange = GC.getDefineINT("RIVER_SOURCE_MIN_RIVER_RANGE");
	int const iSeaWaterRange = GC.getDefineINT("RIVER_SOURCE_MIN_SEAWATER_RANGE");
	int const iPlotsPerRiverEdge =  GC.getDefineINT("PLOTS_PER_RIVER_EDGE");
	// advc.129: Randomize the traversal order
	int* aiShuffledIndices = ::shuffle(GC.getMap().numPlots(), GC.getGame().getMapRand());
	for (int iPass = 0; iPass < 4; iPass++)
	{
		int iRiverSourceRangeLoop = (iPass <= 1 ? iRiverSourceRange : iRiverSourceRange / 2);
		int iSeaWaterRangeLoop =  (iPass <= 1 ? iSeaWaterRange : iSeaWaterRange / 2);

		for (int i = 0; i < GC.getMap().numPlots(); i++)
		{
			CvPlot const* pLoopPlot = GC.getMap().plotByIndex(
					aiShuffledIndices[i]); // advc.129
			if (pLoopPlot->isWater())
				continue;

			bool bValid;
			switch(iPass)
			{
			case 0:
				bValid = (pLoopPlot->isHills() || pLoopPlot->isPeak());
				break;
			case 1:
				bValid = (!pLoopPlot->isCoastalLand() &&
						GC.getGame().getMapRandNum(8, "addRivers") == 0);
				break;
			case 2:
				bValid =  ((pLoopPlot->isHills() || pLoopPlot->isPeak()) &&
					  pLoopPlot->area()->getNumRiverEdges() < 1 +
					  pLoopPlot->area()->getNumTiles() / iPlotsPerRiverEdge);
				break;
			case 3:
				bValid = (pLoopPlot->area()->getNumRiverEdges() < 1 +
						pLoopPlot->area()->getNumTiles() / iPlotsPerRiverEdge);
				break;
			default: FAssertMsg(false, "Invalid iPass");
			}
			if (!bValid)
				continue;

			gDLL->callUpdater(); // advc.opt: Moved down; shouldn't need to update the UI in every iteration.
			if (!GC.getMap().findWater(pLoopPlot, iRiverSourceRange, true) &&
				!GC.getMap().findWater(pLoopPlot, iSeaWaterRange, false))
			{
				CvPlot* pStartPlot = pLoopPlot->getInlandCorner();
				if (pStartPlot != NULL)
					doRiver(pStartPlot);
			}
		}
	}
	SAFE_DELETE_ARRAY(aiShuffledIndices); // advc.129
}

// pStartPlot = the plot at whose SE corner the river is starting
void CvMapGenerator::doRiver(CvPlot *pStartPlot, CardinalDirectionTypes eLastCardinalDirection, CardinalDirectionTypes eOriginalCardinalDirection, int iThisRiverID)
{
	if (iThisRiverID == -1)
	{
		iThisRiverID = GC.getMap().getNextRiverID();
		GC.getMap().incrementNextRiverID();
	}

	int iOtherRiverID = pStartPlot->getRiverID();
	if (iOtherRiverID != -1 && iOtherRiverID != iThisRiverID)
	{
		return; // Another river already exists here; can't branch off of an existing river!
	}

	CvPlot *pRiverPlot = NULL;
	CvPlot *pAdjacentPlot = NULL;

	CardinalDirectionTypes eBestCardinalDirection = NO_CARDINALDIRECTION;

	if (eLastCardinalDirection==CARDINALDIRECTION_NORTH)
	{
		pRiverPlot = pStartPlot;
		if (pRiverPlot == NULL)
		{
			return;
		}
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_EAST);
		if ((pAdjacentPlot == NULL) || pRiverPlot->isWOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}

		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setWOfRiver(true, eLastCardinalDirection);
		pRiverPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_NORTH);
	}
	else if (eLastCardinalDirection==CARDINALDIRECTION_EAST)
	{
		pRiverPlot = plotCardinalDirection(pStartPlot->getX(), pStartPlot->getY(), CARDINALDIRECTION_EAST);
		if (pRiverPlot == NULL)
		{
			return;
		}
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_SOUTH);
		if ((pAdjacentPlot == NULL) || pRiverPlot->isNOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}

		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setNOfRiver(true, eLastCardinalDirection);
	}
	else if (eLastCardinalDirection==CARDINALDIRECTION_SOUTH)
	{
		pRiverPlot = plotCardinalDirection(pStartPlot->getX(), pStartPlot->getY(), CARDINALDIRECTION_SOUTH);
		if (pRiverPlot == NULL)
		{
			return;
		}
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_EAST);
		if ((pAdjacentPlot == NULL) || pRiverPlot->isWOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}

		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setWOfRiver(true, eLastCardinalDirection);
	}

	else if (eLastCardinalDirection==CARDINALDIRECTION_WEST)
	{
		pRiverPlot = pStartPlot;
		if (pRiverPlot == NULL)
		{
			return;
		}
		pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_SOUTH);
		if ((pAdjacentPlot == NULL) || pRiverPlot->isNOfRiver() || pRiverPlot->isWater() || pAdjacentPlot->isWater())
		{
			return;
		}

		pStartPlot->setRiverID(iThisRiverID);
		pRiverPlot->setNOfRiver(true, eLastCardinalDirection);
		pRiverPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), CARDINALDIRECTION_WEST);
	}
	else
	{
		//FAssertMsg(false, "Illegal direction type");
		// River is starting here, set the direction in the next step
		pRiverPlot = pStartPlot;

		GC.getPythonCaller()->riverStartCardinalDirection(*pRiverPlot, eBestCardinalDirection);
	}

	if (pRiverPlot == NULL)
	{
		return; // The river has flowed off the edge of the map. All is well.
	}
	else if (pRiverPlot->hasCoastAtSECorner())
	{
		return; // The river has flowed into the ocean. All is well.
	}

	if (eBestCardinalDirection == NO_CARDINALDIRECTION)
	{
		int iBestValue = MAX_INT;

		for (int iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; iI++)
		{
			if (getOppositeCardinalDirection((CardinalDirectionTypes)iI) != eOriginalCardinalDirection)
			{
				if (getOppositeCardinalDirection((CardinalDirectionTypes)iI) != eLastCardinalDirection)
				{
					pAdjacentPlot = plotCardinalDirection(pRiverPlot->getX(), pRiverPlot->getY(), ((CardinalDirectionTypes)iI));
					if (pAdjacentPlot != NULL)
					{
						int iValue = getRiverValueAtPlot(pAdjacentPlot);
						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							eBestCardinalDirection = (CardinalDirectionTypes)iI;
						}
					}
				}
			}
		}
	}

	if (eBestCardinalDirection != NO_CARDINALDIRECTION)
	{
		if  (eOriginalCardinalDirection	== NO_CARDINALDIRECTION)
		{
			eOriginalCardinalDirection = eBestCardinalDirection;
		}
		doRiver(pRiverPlot, eBestCardinalDirection, eOriginalCardinalDirection, iThisRiverID);
	}
}
//Note from Blake:
//Iustus wrote this function, it ensures that a new river actually
//creates fresh water on the passed plot. Quite useful really
//Although I veto'd its use since I like that you don't always
//get fresh water starts.
// pFreshWaterPlot = the plot we want to give a fresh water river
//
bool CvMapGenerator::addRiver(CvPlot* pFreshWaterPlot)
{
	FAssertMsg(pFreshWaterPlot != NULL, "NULL plot parameter");

	// cannot have a river flow next to water
	if (pFreshWaterPlot->isWater())
	{
		return false;
	}

	// if it already has a fresh water river, then success! we done
	if (pFreshWaterPlot->isRiver())
	{
		return true;
	}

	bool bSuccess = false;

	// randomize the order of directions
	int aiShuffle[NUM_CARDINALDIRECTION_TYPES];
	shuffleArray(aiShuffle, NUM_CARDINALDIRECTION_TYPES, GC.getGame().getMapRand());

	// make two passes, once for each flow direction of the river
	int iNWFlowPass = GC.getGame().getMapRandNum(2, "addRiver");
	for (int iPass = 0; !bSuccess && iPass <= 1; iPass++)
	{
		// try placing a river edge in each direction, in random order
		for (int iI = 0; !bSuccess && iI < NUM_CARDINALDIRECTION_TYPES; iI++)
		{
			CardinalDirectionTypes eRiverDirection = NO_CARDINALDIRECTION;
			CvPlot *pRiverPlot = NULL;

			switch (aiShuffle[iI])
			{
			case CARDINALDIRECTION_NORTH:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_NORTH);
					eRiverDirection = CARDINALDIRECTION_WEST;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_NORTHWEST);
					eRiverDirection = CARDINALDIRECTION_EAST;
				}
				break;

			case CARDINALDIRECTION_EAST:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = pFreshWaterPlot;
					eRiverDirection = CARDINALDIRECTION_NORTH;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_NORTH);
					eRiverDirection = CARDINALDIRECTION_SOUTH;
				}
				break;

			case CARDINALDIRECTION_SOUTH:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = pFreshWaterPlot;
					eRiverDirection = CARDINALDIRECTION_WEST;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_WEST);
					eRiverDirection = CARDINALDIRECTION_EAST;
				}
				break;

			case CARDINALDIRECTION_WEST:
				if (iPass == iNWFlowPass)
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_WEST);
					eRiverDirection = CARDINALDIRECTION_NORTH;
				}
				else
				{
					pRiverPlot = plotDirection(pFreshWaterPlot->getX(), pFreshWaterPlot->getY(), DIRECTION_NORTHWEST);
					eRiverDirection = CARDINALDIRECTION_SOUTH;
				}
				break;

			default:
				FAssertMsg(false, "invalid cardinal direction");
			}

			if (pRiverPlot != NULL && !pRiverPlot->hasCoastAtSECorner())
			{
				// try to make the river
				doRiver(pRiverPlot, eRiverDirection, eRiverDirection, -1);

				// if it succeeded, then we will be a river now!
				if (pFreshWaterPlot->isRiver())
				{
					bSuccess = true;
				}
			}
		}
	}

	return bSuccess;
}


void CvMapGenerator::addFeatures()
{
	PROFILE("CvMapGenerator::addFeatures");

	if (GC.getPythonCaller()->addFeatures())
		return;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot& kPlot = *GC.getMap().plotByIndex(iI);
		for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
		{
			if (kPlot.canHaveFeature((FeatureTypes)iJ))
			{
				if (GC.getGame().getMapRandNum(10000, "addFeaturesAtPlot") <
						GC.getFeatureInfo((FeatureTypes)iJ).getAppearanceProbability())
					kPlot.setFeatureType((FeatureTypes)iJ);
			}
		}
	}
}

void CvMapGenerator::addBonuses()
{
	PROFILE("CvMapGenerator::addBonuses");
	gDLL->NiTextOut("Adding Bonuses...");

	CvPythonCaller const& py = *GC.getPythonCaller();
	if (py.addBonuses())
		return;
	/*  <advc.129> Only do an iteration for those PlacementOrder numbers that are
		actually used in the BonusInfos. */
	std::vector<int> aiOrdinals;
	for (int i = 0; i < GC.getNumBonusInfos(); i++)
	{
		int iOrder = GC.getBonusInfo((BonusTypes)i).getPlacementOrder();
		if (iOrder >= 0) // The negative ones aren't supposed to be placed at all
			aiOrdinals.push_back(iOrder);
	}
	::removeDuplicates(aiOrdinals);
	FAssertMsg(aiOrdinals.size() <= (uint)12, "Shuffling the bonus indices this often might be slow(?)");
	std::sort(aiOrdinals.begin(), aiOrdinals.end());
	//for (int iOrder = 0; iOrder < GC.getNumBonusInfos(); iOrder++)
	for (size_t i = 0; i < aiOrdinals.size(); i++)
	{
		int iOrder = aiOrdinals[i];
		/*  Break ties in the order randomly (perhaps better not to do this though
			if the assertion above fails) */
		int* aiShuffledIndices = ::shuffle(GC.getNumBonusInfos(), GC.getGame().getMapRand());
		// </advc.129>
		for (int j = 0; j < GC.getNumBonusInfos(); j++)
		{
			BonusTypes eBonus = (BonusTypes)aiShuffledIndices[j]; // advc.129
			//gDLL->callUpdater();
			if (GC.getBonusInfo(eBonus).getPlacementOrder() != iOrder)
				continue;

			gDLL->callUpdater(); // advc.opt: Moved down; don't need to update the UI quite so frequently.
			if (!py.addBonusType(eBonus))
			{
				if (GC.getBonusInfo(eBonus).isOneArea())
					addUniqueBonusType(eBonus);
				else addNonUniqueBonusType(eBonus);
			}
		}
		SAFE_DELETE_ARRAY(aiShuffledIndices); // advc.129
	}
}

void CvMapGenerator::addUniqueBonusType(BonusTypes eBonusType)
{
	// K-Mod note: the areas tried stuff was originally done using an array.
	// I've rewritten it to use std::set, for no good reason. The functionality is unchanged.
	// (But it is now slightly more efficient and easier to read.)
	std::set<int> areas_tried;

	CvBonusInfo const& pBonusInfo = GC.getBonusInfo(eBonusType);
	int const iBonusCount = calculateNumBonusesToAdd(eBonusType);
	bool const bIgnoreLatitude = GC.getPythonCaller()->isBonusIgnoreLatitude();
	// advc.opt: Don't waste time trying to place land resources in the ocean
	bool const bWater = (pBonusInfo.isTerrain(GC.getWATER_TERRAIN(true)) ||
			pBonusInfo.isTerrain(GC.getWATER_TERRAIN(false)));
	FAssertMsg(pBonusInfo.isOneArea(), "addUniqueBonusType called with non-unique bonus type");

	while (true)
	{
		int iBestValue = 0;
		CvArea *pBestArea = NULL;
		FOR_EACH_AREA_VAR(pLoopArea)
		{	// <advc.opt>
			if (pLoopArea->isWater() && !bWater)
				continue; // </advc.opt>
			if (areas_tried.count(pLoopArea->getID()) == 0)
			{
				int iNumUniqueBonusesOnArea = pLoopArea->countNumUniqueBonusTypes() + 1; // number of unique bonuses starting on the area, plus this one
				int iNumTiles = pLoopArea->getNumTiles();
				//int iValue = iNumTiles / iNumUniqueBonusesOnArea;
				int iValue = (iNumTiles + (iNumTiles >= 4*NUM_CITY_PLOTS ? GC.getGame().getMapRandNum(3*NUM_CITY_PLOTS, "addUniqueBonusType area value"): 0)) / iNumUniqueBonusesOnArea; // K-Mod

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestArea = pLoopArea;
				}
			}
		}

		if (pBestArea == NULL)
			break; // can't place bonus on any area

		areas_tried.insert(pBestArea->getID());

		// Place the bonuses:

		CvMap& m = GC.getMap(); // advc (and some stype changes in the following)
		int* aiShuffledIndices = shuffle(m.numPlots(), GC.getGame().getMapRand());
		for (int iI = 0; iI < m.numPlots(); iI++)
		{
			CvPlot& kRandPlot = *m.plotByIndex(aiShuffledIndices[iI]);

			if (m.getNumBonuses(eBonusType) >= iBonusCount)
				break; // We already have enough

			if (pBestArea != kRandPlot.area())
				continue;

			int const x = kRandPlot.getX();
			int const y = kRandPlot.getY();
			if (!canPlaceBonusAt(eBonusType, x, y, bIgnoreLatitude))
				continue;

			/*	<advc.129> About to place a cluster of eBonusType. Don't place that
				cluster near an earlier cluster (or any instance) of the same bonus class
				- that's what can lead to e.g. starts with 5 Gold. canPlaceBonusAt
				can't enforce this well b/c it doesn't know whether a cluster starts
				(distance needs to be heeded) or continues (distance mustn't be heeded). */
			int iClassToAvoid = pBonusInfo.getBonusClassType();
			/*  Only resources that occur in clusters are problematic. Not sure about
				the iClassToAvoid>0 clause. 0 is the "general" bonus class containing
				all the clustered resources except for Gold, Silver, Gems which I've
				moved to a separate class "precious". I.e. currently only double clusters
				of precious bonuses are avoided. Eliminating all double clusters might
				get in the way of (early) resource trades too much, and make the map
				less exciting than it could be. */
			if(pBonusInfo.getGroupRand() > 0 && iClassToAvoid > 0) {
				bool bSkip = false;
				/*  Can't use pClassInfo.getUniqueRange() b/c this has to be
					0 for bonuses that appear in clusters. 5 hardcoded. */
				int const iDist = 5;
				for(int dx = -iDist; dx <= iDist; dx++)
				for(int dy = -iDist; dy <= iDist; dy++) {
					CvPlot* pLoopPlot = plotXY(x, y, dx, dy);
					if(pLoopPlot == NULL) continue; CvPlot& p = *pLoopPlot;
					if(p.getArea() != kRandPlot.getArea() ||
							plotDistance(x, y, p.getX(), p.getY()) > iDist)
						continue;
					BonusTypes eOtherBonus = p.getBonusType();
					if(eOtherBonus != NO_BONUS && GC.getBonusInfo(eOtherBonus).
							getBonusClassType() == iClassToAvoid &&
							GC.getBonusInfo(eOtherBonus).getGroupRand() > 0) {
						bSkip = true;
						break;
					}
				}
				if(bSkip)
					continue;
			}
			// </advc.129>
			kRandPlot.setBonusType(eBonusType);
			// advc.129: Replacing the code below
			placeGroup(eBonusType, kRandPlot, bIgnoreLatitude);
			/*for (int iDX = -(pBonusInfo.getGroupRange()); iDX <= pBonusInfo.getGroupRange(); iDX++) {
				for (int iDY = -(pBonusInfo.getGroupRange()); iDY <= pBonusInfo.getGroupRange(); iDY++) {
					if (GC.getMap().getNumBonuses(eBonusType) < iBonusCount) {
						CvPlot* pLoopPlot	= plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);
						if (pLoopPlot != NULL && (pLoopPlot->area() == pBestArea)) {
							if (canPlaceBonusAt(eBonusType, pLoopPlot->getX(), pLoopPlot->getY(), bIgnoreLatitude)) {
								if (GC.getGame().getMapRandNum(100, "addUniqueBonusType") < pBonusInfo.getGroupRand())
									pLoopPlot->setBonusType(eBonusType);
			} } } } }*/

		}
		SAFE_DELETE_ARRAY(aiShuffledIndices);
	}
}

void CvMapGenerator::addNonUniqueBonusType(BonusTypes eBonusType)
{
	int iBonusCount = calculateNumBonusesToAdd(eBonusType);
	if (iBonusCount == 0)
		return;

	int *aiShuffledIndices = shuffle(GC.getMap().numPlots(), GC.getGame().getMapRand());
	// advc.129: Moved into placeGroup
	//CvBonusInfo& pBonusInfo = GC.getBonusInfo(eBonusType);
	bool const bIgnoreLatitude = GC.getPythonCaller()->isBonusIgnoreLatitude();
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot& kPlot = *GC.getMap().plotByIndex(aiShuffledIndices[iI]);
		if (!canPlaceBonusAt(eBonusType, kPlot.getX(), kPlot.getY(), bIgnoreLatitude))
			continue; // advc

		kPlot.setBonusType(eBonusType);
		iBonusCount--;
		// advc.129: Replacing the loop below
		iBonusCount -= placeGroup(eBonusType, kPlot, bIgnoreLatitude, iBonusCount);
		/*for (int iDX = -(pBonusInfo.getGroupRange()); iDX <= pBonusInfo.getGroupRange(); iDX++) {
			for (int iDY = -(pBonusInfo.getGroupRange()); iDY <= pBonusInfo.getGroupRange(); iDY++) {
				if (iBonusCount > 0) {
					CvPlot* pLoopPlot	= plotXY(kPlot.getX(), kPlot.getY(), iDX, iDY);
					if (pLoopPlot != NULL) {
						if (canPlaceBonusAt(eBonusType, pLoopPlot->getX(), pLoopPlot->getY(), bIgnoreLatitude)) {
							if (GC.getGame().getMapRandNum(100, "addNonUniqueBonusType") < pBonusInfo.getGroupRand()) {
								pLoopPlot->setBonusType(eBonusType);
								iBonusCount--;
		} } } } } }
		FAssertMsg(iBonusCount >= 0, "iBonusCount must be >= 0");*/
		if (iBonusCount == 0)
			break;
	}
	SAFE_DELETE_ARRAY(aiShuffledIndices);
}


// <advc.129>
int CvMapGenerator::placeGroup(BonusTypes eBonusType, CvPlot const& kCenter,
		bool bIgnoreLatitude, int iLimit) {

	CvBonusInfo const& kBonus = GC.getBonusInfo(eBonusType);
	// The one in the center is already placed, but that doesn't count here.
	int iPlaced = 0;
	std::vector<CvPlot*> apGroupRange;
	for(int iDX = -kBonus.getGroupRange(); iDX <= kBonus.getGroupRange(); iDX++) {
		for(int iDY = -kBonus.getGroupRange(); iDY <= kBonus.getGroupRange(); iDY++) {
			CvPlot* p = plotXY(kCenter.getX(), kCenter.getY(), iDX, iDY);
			if(p != NULL && canPlaceBonusAt(eBonusType, p->getX(), p->getY(), bIgnoreLatitude))
				apGroupRange.push_back(p);
		}
	}
	// Would've been nice, but must use MapRand instead.
	//std::random_shuffle(groupRange.begin(), groupRange.end());
	int sz = (int)apGroupRange.size();
	if(sz <= 0)
		return 0;
	int* aiShuffledIndices = new int[sz];
	for(int i = 0; i < sz; i++)
		aiShuffledIndices[i] = i;
	::shuffleArray(aiShuffledIndices, sz, GC.getGame().getMapRand());
	for(int j = 0; j < sz && iLimit > 0; j++) {
		int iProb = kBonus.getGroupRand();
		iProb = ::round(iProb * std::pow(2/3.0, iPlaced));
		if (GC.getGame().getMapRandNum(100, "addNonUniqueBonusType") < iProb) {
			apGroupRange[aiShuffledIndices[j]]->setBonusType(eBonusType);
			iLimit--;
			iPlaced++;
		}
	}
	SAFE_DELETE_ARRAY(aiShuffledIndices);
	FAssert(iLimit >= 0);
	return iPlaced;
} // </advc.129>


void CvMapGenerator::addGoodies()  // advc: some style changes
{
	PROFILE("CvMapGenerator::addGoodies");

	if (GC.getPythonCaller()->addGoodies())
		return;

	gDLL->NiTextOut("Adding Goodies...");

	if (GC.getEraInfo(GC.getGame().getStartEra()).isNoGoodies())
		return;

	int* aiShuffledIndices = shuffle(GC.getMap().numPlots(), GC.getGame().getMapRand());
	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		ImprovementTypes eImprov = (ImprovementTypes)iI;
		if (!GC.getImprovementInfo(eImprov).isGoody() ||
				GC.getImprovementInfo(eImprov).getTilesPerGoody() <= 0)
			continue;

		for (int iJ = 0; iJ < GC.getMap().numPlots(); iJ++)
		{
			CvPlot& kPlot = *GC.getMap().plotByIndex(aiShuffledIndices[iJ]);
			if (kPlot.isWater())
				continue;

			CvArea const& kArea = *GC.getMap().getArea(kPlot.getArea());
			if (kArea.getNumImprovements(eImprov) <
				(kArea.getNumTiles() +
				GC.getImprovementInfo(eImprov).getTilesPerGoody() / 2) /
				GC.getImprovementInfo(eImprov).getTilesPerGoody())
			{
				if (canPlaceGoodyAt(eImprov, kPlot.getX(), kPlot.getY()))
					kPlot.setImprovementType(eImprov);
			}
		}
		gDLL->callUpdater(); // advc.opt: Moved out of the loop
	}
	SAFE_DELETE_ARRAY(aiShuffledIndices);
}


void CvMapGenerator::eraseRivers()
{
	for (int i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);
		if (pPlot->isNOfRiver())
		{
			pPlot->setNOfRiver(false, NO_CARDINALDIRECTION);
		}
		if (pPlot->isWOfRiver())
		{
			pPlot->setWOfRiver(false, NO_CARDINALDIRECTION);
		}
	}
}

void CvMapGenerator::eraseFeatures()
{
	for (int i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);
		pPlot->setFeatureType(NO_FEATURE);
	}
}

void CvMapGenerator::eraseBonuses()
{
	for (int i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);
		pPlot->setBonusType(NO_BONUS);
	}
}

void CvMapGenerator::eraseGoodies()
{
	for (int i = 0; i < GC.getMap().numPlots(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);
		if (pPlot->isGoody())
		{
			pPlot->removeGoody();
		}
	}
}

//------------------------------------------------------------------------------------------------
//
// Call python function to generate random map
// It will call applyMapData when it's done
//

void CvMapGenerator::generateRandomMap()
{
	PROFILE("generateRandomMap()");

	if (GC.getPythonCaller()->generateRandomMap())
		return;

	char buf[256];

	sprintf(buf, "Generating Random Map %S, %S...", gDLL->getMapScriptName().GetCString(), GC.getWorldInfo(GC.getMap().getWorldSize()).getDescription());
	gDLL->NiTextOut(buf);

	generatePlotTypes();
	generateTerrain();
	/* advc.300: Already done in CvMap::calculateAreas, but when calculateAreas
	   is called during map generation, tile yields aren't yet set. */
	GC.getMap().computeShelves();
}

void CvMapGenerator::generatePlotTypes()
{
	int const iNumPlots = GC.getMap().numPlots();
	int* paiPlotTypes = new int[iNumPlots];
	if (!GC.getPythonCaller()->generatePlotTypes(paiPlotTypes, iNumPlots))
	{
		for (int iI = 0; iI < iNumPlots; iI++)
			paiPlotTypes[iI] = PLOT_LAND;
	}
	setPlotTypes(paiPlotTypes);
	SAFE_DELETE_ARRAY(paiPlotTypes);
}

void CvMapGenerator::generateTerrain()
{
	PROFILE("generateTerrain()");

	int iNumPlots = GC.getMap().numPlots();
	std::vector<int> pyTerrain;
	if (GC.getPythonCaller()->generateTerrainTypes(pyTerrain, iNumPlots))
	{
		for (int i = 0; i < iNumPlots; i++)
		{
			//gDLL->callUpdater(); // addvc.003b
			GC.getMap().plotByIndex(i)->setTerrainType((TerrainTypes)pyTerrain[i], false, false);
		}
	}
}

// Allows for user-defined Python Actions for map generation after it's already been created
void CvMapGenerator::afterGeneration()
{
	PROFILE("CvMapGenerator::afterGeneration");
	GC.getPythonCaller()->callMapFunction("afterGeneration");
}

void CvMapGenerator::setPlotTypes(const int* paiPlotTypes)
{
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		//gDLL->callUpdater(); // advc.opt: Not needed I reckon
		GC.getMap().plotByIndex(iI)->setPlotType((PlotTypes)paiPlotTypes[iI], false, false);
	}

	GC.getMap().recalculateAreas();

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		//gDLL->callUpdater(); // advc.opt
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);
		if (pLoopPlot->isWater())
		{
			if (pLoopPlot->isAdjacentToLand())
				pLoopPlot->setTerrainType(GC.getWATER_TERRAIN(true), false, false);
			else pLoopPlot->setTerrainType(GC.getWATER_TERRAIN(false), false, false);
		}
	}
}


int CvMapGenerator::getRiverValueAtPlot(CvPlot* pPlot)
{
	bool bOverride=false;
	int pyValue = GC.getPythonCaller()->riverValue(*pPlot, bOverride);
	if (bOverride)
		return pyValue;
	int iSum = pyValue; // Add to value from Python

	iSum += (NUM_PLOT_TYPES - pPlot->getPlotType()) * 20;

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
	{
		CvPlot* pAdjacentPlot = plotDirection(pPlot->getX(), pPlot->getY(), (DirectionTypes)iI);
		if (pAdjacentPlot != NULL)
			iSum += (NUM_PLOT_TYPES - pAdjacentPlot->getPlotType());
		else iSum += (NUM_PLOT_TYPES * 10);
	}
	CvRandom riverRand;
	riverRand.init((pPlot->getX() * 43251267) + (pPlot->getY() * 8273903));
	iSum += riverRand.get(10, "River Rand");

	return iSum;
}

int CvMapGenerator::calculateNumBonusesToAdd(BonusTypes eBonusType)
{
	CvBonusInfo& pBonusInfo = GC.getBonusInfo(eBonusType);

	// Calculate iBonusCount, the amount of this bonus to be placed:

	int iRand1 = GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance1(), "calculateNumBonusesToAdd-1");
	int iRand2 = GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance2(), "calculateNumBonusesToAdd-2");
	int iRand3 = GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance3(), "calculateNumBonusesToAdd-3");
	int iRand4 = GC.getGame().getMapRandNum(pBonusInfo.getRandAppearance4(), "calculateNumBonusesToAdd-4");
	int iBaseCount = pBonusInfo.getConstAppearance() + iRand1 + iRand2 + iRand3 + iRand4;

	bool const bIgnoreLatitude = GC.getPythonCaller()->isBonusIgnoreLatitude();

	// Calculate iNumPossible, the number of plots that are eligible to have this bonus:

	int iLandTiles = 0;
	if (pBonusInfo.getTilesPer() > 0)
	{
		int iNumPossible = 0;
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			CvPlot* pPlot = GC.getMap().plotByIndex(iI);
			if (pPlot->canHaveBonus(eBonusType, bIgnoreLatitude))
			{
				iNumPossible++;
			}
		}
		// <advc.129>
		if(GC.getDefineBOOL("SUBLINEAR_BONUS_QUANTITIES")) {
			int iSubtrahend = pBonusInfo.getTilesPer(); // Typically 16 or 32
			int iRemainder = iNumPossible;
			int iResult = 0;
			/* Place one for the first, say, 16 tiles, the next after 17, then 18 ...
			   i.e. number of resources placed grows sublinearly with the number of
			   eligible plots. */
			while(true) {
				iRemainder -= iSubtrahend;
				if(iRemainder < 0)
					break;
				iResult++;
				iSubtrahend++;
			}
			iLandTiles += iResult;
		}
		else // </advc.129>
			iLandTiles += (iNumPossible / pBonusInfo.getTilesPer());
	}

	int iPlayers = (GC.getGame().countCivPlayersAlive() * pBonusInfo.getPercentPerPlayer()) / 100;
	int iBonusCount = (iBaseCount * (iLandTiles + iPlayers)) / 100;
	iBonusCount = std::max(1, iBonusCount);
	return iBonusCount;
}
