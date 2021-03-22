#include "CvGameCoreDLL.h"
#include "CvGameCoreUtils.h"
#include "CvGamePlay.h"
#include "CvUnitAI.h"
#include "CvSelectionGroupAI.h"
#include "CvTeamAI.h"
#include "CityPlotIterator.h"
#include "BBAILog.h" // advc.007
#include "CvInfo_GameOption.h"

using std::vector; // advc

// <advc.003g>
int roundToMultiple(double d, int iMultiple)
{
	int r = (int)(d + 0.5 * iMultiple);
	return r - r % iMultiple;
}

bool bernoulliSuccess(double pr, char const* pszLog, bool bAsync, int iData1, int iData2)
{
	int iChancePerMyriad = round(pr * 10000.0);
	// These two checks are just for better performance
	if(iChancePerMyriad >= 10000)
		return true;
	if(iChancePerMyriad <= 0)
		return false;
	if(pszLog != NULL && strlen(pszLog) <= 0)
		pszLog = "bs";
	if(bAsync)
		return (GC.getASyncRand().get(10000, pszLog) < iChancePerMyriad);
	return GC.getGame().
			getSorenRandNum(10000, pszLog, iData1, iData2) < iChancePerMyriad;
}

double percentileRank(vector<double>& distribution, double score,
	bool bSorted, bool bScorePartOfDistribution)
{
	if(!bSorted)
		std::sort(distribution.begin(), distribution.end());
	int n = (int)distribution.size();
	int iLEq = 0; // less or equal
	for(int i = 0; i < n; i++)
	{
		if(distribution[i] <= score)
			iLEq++;
		else break;
	}
	if(bScorePartOfDistribution) 
	{
		iLEq++;
		n++;
	}
	else if(n == 0)
		return 1;
	return iLEq / (double)n;
} // </advc.003g>
/*	advc: Akin to natGetDeterministicRandom (deleted from CvCity.cpp). For reference,
	the implementation of that function was:
	srand(7297 * iSeedX + 2909  * iSeedY);
	return (rand() % (iMax - iMin)) + iMin; */
int intHash(vector<int> const& x, PlayerTypes ePlayer)
{
	int const iPrime = 31;
	int iHashVal = 0;
	for (size_t i = 0; i < x.size(); i++)
	{
		iHashVal += x[i];
		iHashVal *= iPrime;
	}
	int iCapitalIndex = -1;
	if (ePlayer != NO_PLAYER)
	{
		CvCity* pCapital = GET_PLAYER(ePlayer).getCapital();
		if (pCapital != NULL)
			iCapitalIndex = GC.getMap().plotNum(pCapital->getPlot());
	}
	if (iCapitalIndex >= 0)
	{
		iHashVal += iCapitalIndex;
		iHashVal *= iPrime;
	}
	return iHashVal;
}
// advc.035:
void contestedPlots(vector<CvPlot*>& r, TeamTypes t1, TeamTypes t2)
{
	if(!GC.getDefineBOOL(CvGlobals::OWN_EXCLUSIVE_RADIUS))
		return;
	// Sufficient to check plots around the teams' cities
	vector<CvCity const*> apCities;
	for(int i = 0; i < MAX_CIV_PLAYERS; i++)
	{
		CvPlayer& kMember = GET_PLAYER((PlayerTypes)i);
		if(!kMember.isAlive() || (kMember.getTeam() != t1 && kMember.getTeam() != t2))
			continue;
		FOR_EACH_CITY(c, kMember)
			apCities.push_back(c);
	}
	std::set<int> seenPlots; // To avoid duplicates
	for(size_t i = 0; i < apCities.size(); i++)
	{
		CvCity const& c = *apCities[i];
		for(CityPlotIter it(c, false); it.hasNext(); ++it)
		{
			CvPlot& p = *it;
			if(p.isCity())
				continue;
			PlayerTypes eSecondOwner = p.getSecondOwner();
			PlayerTypes eOwner = p.getOwner();
			if(eOwner == NO_PLAYER || eSecondOwner == NO_PLAYER)
				continue;
			TeamTypes eTeam = TEAMID(eOwner);
			TeamTypes eSecondTeam = TEAMID(eSecondOwner);
			if(eTeam == eSecondTeam)
				continue;
			if((eTeam == t1 && eSecondTeam == t2) || (eTeam == t2 && eSecondTeam == t1))
			{
				int iPlotID = p.getX() * 1000 + p.getY();
				if(seenPlots.count(iPlotID) <= 0)
				{
					seenPlots.insert(iPlotID);
					r.push_back(&p);
				}
			}
		}
	}
} 
// advc.004w: I'm not positive that there isn't already a function like this somewhere
void applyColorToString(CvWString& s, char const* szColor, bool bLink)
{
	if(bLink)
		s.Format(L"<link=literal>%s</link>", s.GetCString());
	s.Format(SETCOLR L"%s" ENDCOLR, TEXT_COLOR(szColor), s.GetCString());
}

// advc.002i: Formula by T. Riemersma (CompuPhase.com) via Wikipedia ("Color difference")
float colorDifference(NiColorA const& c1, NiColorA const& c2)
{
	float channelWeights[] = { 2, 4, 3 }; // R, G, B
	if (c1.r + c2.r > 1)
		std::swap(channelWeights[0], channelWeights[2]);
	float fDiff = 0;
	fDiff += SQR(c1.r - c2.r) * channelWeights[0];
	fDiff += SQR(c1.g - c2.g) * channelWeights[1];
	fDiff += SQR(c1.b - c2.b) * channelWeights[2];
	return fDiff;
}

DirectionTypes cardinalDirectionToDirection(CardinalDirectionTypes eCard)
{
	switch (eCard)
	{
	case CARDINALDIRECTION_NORTH:
		return DIRECTION_NORTH;
	case CARDINALDIRECTION_EAST:
		return DIRECTION_EAST;
	case CARDINALDIRECTION_SOUTH:
		return DIRECTION_SOUTH;
	case CARDINALDIRECTION_WEST:
		return DIRECTION_WEST;
	}
	return NO_DIRECTION;
}

bool isCardinalDirection(DirectionTypes eDirection)
{
	switch (eDirection)
	{
	case DIRECTION_EAST:
	case DIRECTION_NORTH:
	case DIRECTION_SOUTH:
	case DIRECTION_WEST:
		return true;
	}
	return false;
}

DirectionTypes estimateDirection(int iDX, int iDY)
{
	const int displacementSize = 8;
	static float sqrt2 = 1 / sqrt(2.0f);
	//													N			NE			E			SE				S			SW				W			NW
	static float displacements[displacementSize][2] = {{0, 1}, {sqrt2, sqrt2}, {1, 0}, {sqrt2, -sqrt2}, {0, -1}, {-sqrt2, -sqrt2}, {-1, 0}, {-sqrt2, sqrt2}};
	float maximum = 0;
	int maximumIndex = -1;
	for(int i=0;i<displacementSize;i++)
	{
		float dotProduct = iDX * displacements[i][0] + iDY * displacements[i][1];
		if(dotProduct > maximum)
		{
			maximum = dotProduct;
			maximumIndex = i;
		}
	}

	return (DirectionTypes) maximumIndex;
}

DirectionTypes estimateDirection(const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	CvMap const& m = GC.getMap();
	return estimateDirection(
			m.dxWrap(pToPlot->getX() - pFromPlot->getX()),
			m.dyWrap(pToPlot->getY() - pFromPlot->getY()));
}

bool atWar(TeamTypes eTeamA, TeamTypes eTeamB)
{
	return (eTeamA != NO_TEAM && eTeamB != NO_TEAM && GET_TEAM(eTeamA).isAtWar(eTeamB));
}
// advc: No longer needed
/*bool isPotentialEnemy(TeamTypes eOurTeam, TeamTypes eTheirTeam)
{
	FAssert(eOurTeam != NO_TEAM);
	if (eTheirTeam == NO_TEAM)
		return false;
	// advc: Moved into new CvTeamAI function
	return GET_TEAM(eOurTeam).AI_mayAttack(eTheirTeam);
}*/

// FUNCTION: getBinomialCoefficient
// Needed for getCombatOdds
// Returns int value, being the possible number of combinations
// of k draws out of a population of n
// Written by DeepO
// Modified by Jason Winokur to keep the intermediate factorials small
__int64 getBinomialCoefficient(int iN, int iK)
{
	__int64 iTemp = 1;
	//take advantage of symmetry in combination, eg. 15C12 = 15C3
	iK = std::min(iK, iN - iK);

	//eg. 15C3 = (15 * 14 * 13) / (1 * 2 * 3) = 15 / 1 * 14 / 2 * 13 / 3 = 455
	for(int i=1;i<=iK;i++)
		iTemp = (iTemp * (iN - i + 1)) / i;

	// Make sure iTemp fits in an integer (and thus doesn't overflow)
	FAssert(iTemp < MAX_INT);

	return iTemp;
}

// FUNCTION: getCombatOdds
// Calculates combat odds, given two units
// Returns value from 0-1000
// Written by DeepO
int getCombatOdds(const CvUnit* pAttacker, const CvUnit* pDefender)
{
	float fOddsEvent;
	float fOddsAfterEvent;
	int iAttackerStrength;
	int iAttackerFirepower;
	int iDefenderStrength;
	int iDefenderFirepower;
	int iDefenderOdds;
	int iAttackerOdds;
	int iStrengthFactor;
	int iDamageToAttacker;
	int iDamageToDefender;
	int iNeededRoundsAttacker;
	int iNeededRoundsDefender;
	int iMaxRounds;
	int iAttackerLowFS;
	int iAttackerHighFS;
	int iDefenderLowFS;
	int iDefenderHighFS;
	int iFirstStrikes;
	int iDefenderHitLimit;
	int iI;
	int iJ;
	int iI3;
	int iI4;
	int iOdds = 0;

	// setup battle, calculate strengths and odds
	//////

	//Added ST
	iAttackerStrength = pAttacker->currCombatStr(NULL, NULL);
	iAttackerFirepower = pAttacker->currFirepower(NULL, NULL);

	iDefenderStrength = pDefender->currCombatStr(pDefender->plot(), pAttacker);
	iDefenderFirepower = pDefender->currFirepower(pDefender->plot(), pAttacker);

	FAssert((iAttackerStrength + iDefenderStrength) > 0);
	FAssert((iAttackerFirepower + iDefenderFirepower) > 0);

	iDefenderOdds = ((GC.getCOMBAT_DIE_SIDES() * iDefenderStrength) / (iAttackerStrength + iDefenderStrength));

	if (iDefenderOdds == 0)
	{
		return 1000;
	}

	iAttackerOdds = GC.getCOMBAT_DIE_SIDES() - iDefenderOdds;

	if (iAttackerOdds == 0)
	{
		return 0;
	}

	iStrengthFactor = ((iAttackerFirepower + iDefenderFirepower + 1) / 2);

	// calculate damage done in one round
	//////

	iDamageToAttacker = std::max(1,((GC.getCOMBAT_DAMAGE() * (iDefenderFirepower + iStrengthFactor)) / (iAttackerFirepower + iStrengthFactor)));
	iDamageToDefender = std::max(1,((GC.getCOMBAT_DAMAGE() * (iAttackerFirepower + iStrengthFactor)) / (iDefenderFirepower + iStrengthFactor)));

	// calculate needed rounds.
	// Needed rounds = round_up(health/damage)
	//////

	iDefenderHitLimit = pDefender->maxHitPoints() - pAttacker->combatLimit();

	iNeededRoundsAttacker = (std::max(0, pDefender->currHitPoints() - iDefenderHitLimit) + iDamageToDefender - 1) / iDamageToDefender;
	iNeededRoundsDefender = (pAttacker->currHitPoints() + iDamageToAttacker - 1) / iDamageToAttacker;
	iMaxRounds = iNeededRoundsAttacker + iNeededRoundsDefender - 1;

	// calculate possible first strikes distribution.
	// We can't use the getCombatFirstStrikes() function (only one result,
	// no distribution), so we need to mimic it.
	//////

	iAttackerLowFS = (pDefender->immuneToFirstStrikes()) ? 0 : pAttacker->firstStrikes();
	iAttackerHighFS = (pDefender->immuneToFirstStrikes()) ? 0 : (pAttacker->firstStrikes() + pAttacker->chanceFirstStrikes());

	iDefenderLowFS = (pAttacker->immuneToFirstStrikes()) ? 0 : pDefender->firstStrikes();
	iDefenderHighFS = (pAttacker->immuneToFirstStrikes()) ? 0 : (pDefender->firstStrikes() + pDefender->chanceFirstStrikes());

	// UncutDragon
	if (GC.getDefineBOOL(CvGlobals::LFB_ENABLE))
		return LFBgetCombatOdds(iAttackerLowFS, iAttackerHighFS, iDefenderLowFS, iDefenderHighFS, iNeededRoundsAttacker, iNeededRoundsDefender, iAttackerOdds);
	// /UncutDragon

	// For every possible first strike event, calculate the odds of combat.
	// Then, add these to the total, weighted to the chance of that first
	// strike event occurring
	//////

	for (iI = iAttackerLowFS; iI < iAttackerHighFS + 1; iI++)
	{
		for (iJ = iDefenderLowFS; iJ < iDefenderHighFS + 1; iJ++)
		{
			// for every possible combination of fs results, calculate the chance

			if (iI >= iJ)
			{
				// Attacker gets more or equal first strikes than defender

				iFirstStrikes = iI - iJ;

				// For every possible first strike getting hit, calculate both
				// the chance of that event happening, as well as the rest of
				// the chance assuming the event has happened. Multiply these
				// together to get the total chance (Bayes rule).
				// iI3 counts the number of successful first strikes
				//////

				for (iI3 = 0; iI3 < (iFirstStrikes + 1); iI3++)
				{
					// event: iI3 first strikes hit the defender

					// calculate chance of iI3 first strikes hitting: fOddsEvent
					// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
					// this needs to be in floating point math
					//////

					fOddsEvent = ((float)getBinomialCoefficient(iFirstStrikes, iI3)) * pow((((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES()), iI3) * pow((1.0f - (((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES())), (iFirstStrikes - iI3));

					// calculate chance assuming iI3 first strike hits: fOddsAfterEvent
					//////

					if (iI3 >= iNeededRoundsAttacker)
					{
						fOddsAfterEvent = 1;
					}
					else
					{
						fOddsAfterEvent = 0;

						// odds for _at_least_ (iNeededRoundsAttacker - iI3) (the remaining hits
						// the attacker needs to make) out of (iMaxRounds - iI3) (the left over
						// rounds) is the sum of each _exact_ draw
						//////

						for (iI4 = (iNeededRoundsAttacker - iI3); iI4 < (iMaxRounds - iI3 + 1); iI4++)
						{
							// odds of exactly iI4 out of (iMaxRounds - iI3) draws.
							// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
							// this needs to be in floating point math
							//////

							fOddsAfterEvent += ((float)getBinomialCoefficient((iMaxRounds - iI3), iI4)) * pow((((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES()), iI4) * pow((1.0f - (((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES())), ((iMaxRounds - iI3) - iI4));
						}
					}

					// Multiply these together, round them properly, and add
					// the result to the total iOdds
					//////

					iOdds += ((int)(1000.0 * (fOddsEvent*fOddsAfterEvent + 0.0005)));
				}
			}
			else // (iI < iJ)
			{
				// Attacker gets less first strikes than defender

				iFirstStrikes = iJ - iI;

				// For every possible first strike getting hit, calculate both
				// the chance of that event happening, as well as the rest of
				// the chance assuming the event has happened. Multiply these
				// together to get the total chance (Bayes rule).
				// iI3 counts the number of successful first strikes
				//////

				for (iI3 = 0; iI3 < (iFirstStrikes + 1); iI3++)
				{
					// event: iI3 first strikes hit the defender

					// First of all, check if the attacker is still alive.
					// Otherwise, no further calculations need to occur
					/////

					if (iI3 < iNeededRoundsDefender)
					{
						// calculate chance of iI3 first strikes hitting: fOddsEvent
						// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
						// this needs to be in floating point math
						//////

						fOddsEvent = ((float)getBinomialCoefficient(iFirstStrikes, iI3)) * pow((((float)iDefenderOdds) / GC.getCOMBAT_DIE_SIDES()), iI3) * pow((1.0f - (((float)iDefenderOdds) / GC.getCOMBAT_DIE_SIDES())), (iFirstStrikes - iI3));

						// calculate chance assuming iI3 first strike hits: fOddsAfterEvent
						//////

						fOddsAfterEvent = 0;

						// odds for _at_least_ iNeededRoundsAttacker (the remaining hits
						// the attacker needs to make) out of (iMaxRounds - iI3) (the left over
						// rounds) is the sum of each _exact_ draw
						//////

						for (iI4 = iNeededRoundsAttacker; iI4 < (iMaxRounds - iI3 + 1); iI4++)
						{

							// odds of exactly iI4 out of (iMaxRounds - iI3) draws.
							// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
							// this needs to be in floating point math
							//////

							fOddsAfterEvent += ((float)getBinomialCoefficient((iMaxRounds - iI3), iI4)) * pow((((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES()), iI4) * pow((1.0f - (((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES())), ((iMaxRounds - iI3) - iI4));
						}

						// Multiply these together, round them properly, and add
						// the result to the total iOdds
						//////

						iOdds += ((int)(1000.0 * (fOddsEvent*fOddsAfterEvent + 0.0005)));
					}
				}
			}
		}
	}

	// Weigh the total to the number of possible combinations of first strikes events
	// note: the integer math breaks down when #FS > 656 (with a die size of 1000)
	//////

	iOdds /= (((pDefender->immuneToFirstStrikes()) ? 0 : pAttacker->chanceFirstStrikes()) + 1) * (((pAttacker->immuneToFirstStrikes()) ? 0 : pDefender->chanceFirstStrikes()) + 1);

	// finished!
	//////

	return iOdds;
}

// K-Mod:
int estimateCollateralWeight(CvPlot const* pPlot, TeamTypes eAttackTeam,
	TeamTypes eDefenceTeam)
{
	int iBaseCollateral = GC.getDefineINT(CvGlobals::COLLATERAL_COMBAT_DAMAGE); // normally 10

	/*	Collateral damage does not depend on any kind of strength bonus -
		so when a unit takes collateral damage, their bonuses are effectively wasted.
		Therefore, I'm going to inflate the value of collateral damage based
		on a rough estimate of the defenders bonuses. */
	if (pPlot == NULL)
		return iBaseCollateral * 110; // advc

	TeamTypes ePlotBonusTeam = eDefenceTeam;
	if (ePlotBonusTeam == NO_TEAM)
		ePlotBonusTeam = (pPlot->getTeam() == eAttackTeam ? NO_TEAM : pPlot->getTeam());

	iBaseCollateral *= (pPlot->isCity() ? 130 : 110) +
			pPlot->defenseModifier(ePlotBonusTeam, false,
			eAttackTeam); // advc.012

	// Estimate the average collateral damage reduction of the units on the plot
	int iResistanceSum = 0;
	int iUnits = 0;

	for (CLLNode<IDInfo> const* pNode = pPlot->headUnitNode(); pNode != NULL;
		pNode = pPlot->nextUnitNode(pNode))
	{
		CvUnit const& kLoopUnit = *::getUnit(pNode->m_data);
		if (!kLoopUnit.canDefend(pPlot))
			continue;
		if (eDefenceTeam != NO_TEAM && kLoopUnit.getTeam() != eDefenceTeam)
			continue;
		if (eAttackTeam != NO_TEAM && kLoopUnit.getTeam() == eAttackTeam)
			continue;

		iUnits++;
		/*	Kludge! I'm only checking for immunity against the unit's own combat type.
			Ideally we'd know what kind of collateral damage we're expecting to be hit by,
			and check for immunity vs that.
			Or we could check all types... But the reality is, there are always
			going to be mods and fringe cases where the esitmate is inaccurate.
			And currently in K-Mod, all instances of immunity are to the unit's own type anyway.
			Whichever way we do the estimate, cho-ku-nu is going to mess it up anyway.
			(Unless I change the game mechanics.) */
		if ( // advc.001: Animals have no unit combat type (K146 also fixes this)
			kLoopUnit.getUnitCombatType() != NO_UNITCOMBAT &&
			kLoopUnit.getUnitInfo().getUnitCombatCollateralImmune(kLoopUnit.getUnitCombatType()))
		{
			iResistanceSum += 100;
		}
		else iResistanceSum += kLoopUnit.getCollateralDamageProtection();
	}
	if (iUnits > 0)
	{
		iBaseCollateral = iBaseCollateral * (iUnits * 100 - iResistanceSum) /
				(iUnits * 100);
	}

	return iBaseCollateral; // note, a factor of 100 is included in the result.
}


void setTradeItem(TradeData* pItem, TradeableItems eItemType, int iData)
{
	pItem->m_eItemType = eItemType;
	pItem->m_iData = iData;
	pItem->m_bOffering = false;
	pItem->m_bHidden = false;
}


void setListHelp(wchar* szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst)
{
	if (bFirst)
		wcscat(szBuffer, szStart);
	else wcscat(szBuffer, szSeparator);
	wcscat(szBuffer, szItem);
}

void setListHelp(CvWString& szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst)
{
	if (bFirst)
		szBuffer += szStart;
	else szBuffer += szSeparator;
	szBuffer += szItem;
}

void setListHelp(CvWStringBuffer& szBuffer, const wchar* szStart, const wchar* szItem, const wchar* szSeparator, bool bFirst)
{
	if (bFirst)
		szBuffer.append(szStart);
	else szBuffer.append(szSeparator);
	szBuffer.append(szItem);
}

bool PUF_isGroupHead(const CvUnit* pUnit, int iData1, int iData2)
{
	return (pUnit->isGroupHead());
}

bool PUF_isPlayer(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	// <advc.061>
	TeamTypes eForTeam = (TeamTypes)iData2;
	PlayerTypes eOwner = (PlayerTypes)iData1;
	if(eForTeam == NO_TEAM || eOwner == NO_PLAYER || eForTeam == TEAMID(eOwner))
	{
		// </advc.061>
		return (pUnit->getOwner() == iData1);
	} // <advc.061>
	return (pUnit->getOwner() == iData1 && !pUnit->isInvisible(eForTeam, false) &&
			!pUnit->getUnitInfo().isHiddenNationality()); // </advc.061>
}

bool PUF_isTeam(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getTeam() == iData1);
}

bool PUF_isCombatTeam(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	FAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");

	return (TEAMID(pUnit->getCombatOwner((TeamTypes)iData2, pUnit->getPlot())) ==
			iData1 && !pUnit->isInvisible((TeamTypes)iData2, false, false));
}

bool PUF_isOtherPlayer(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getOwner() != iData1);
}

bool PUF_isOtherTeam(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	TeamTypes eTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	if (pUnit->canCoexistWithEnemyUnit(eTeam))
	{
		return false;
	}

	return (pUnit->getTeam() != eTeam);
}

bool PUF_isEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	FAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");

	TeamTypes eOtherTeam = TEAMID((PlayerTypes)iData1);
	TeamTypes eOurTeam = TEAMID(pUnit->getCombatOwner(eOtherTeam));
	if (pUnit->canCoexistWithEnemyUnit(eOtherTeam))
		return false;
	return (iData2 ? eOtherTeam != eOurTeam : GET_TEAM(eOtherTeam).isAtWar(eOurTeam));
}
// advc.ctr:
bool PUF_isEnemyCityAttacker(const CvUnit* pUnit, int iData1, int iData2)
{
	if(iData2 >= 0)
	{
		CvTeam const& kAssumePeace = GET_TEAM((TeamTypes)iData2);
		if(GET_TEAM(pUnit->getTeam()).getMasterTeam() == kAssumePeace.getMasterTeam())
			return false;
	}
	CvUnitInfo& u = pUnit->getUnitInfo();
	if(u.getCargoSpace() <= 0 || u.getSpecialCargo() != NO_SPECIALUNIT)
	{
		if(u.getDomainType() != DOMAIN_LAND)
			return false;
		if(u.isOnlyDefensive() || u.getCombat() <= 0)
			return false;
	}
	return PUF_isEnemy(pUnit, iData1, false);
}

bool PUF_isVisible(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return !pUnit->isInvisible(TEAMID((PlayerTypes)iData1), false);
}

bool PUF_isVisibleDebug(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return !(pUnit->isInvisible(GET_PLAYER((PlayerTypes)iData1).getTeam(), true));
}

bool PUF_canSiege(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return pUnit->canSiege(TEAMID((PlayerTypes)iData1));
}

bool PUF_isPotentialEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	FAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");
	// advc: Switched the Our/Other variable names. iData1 is the team whose war plans matter.
	TeamTypes eOurTeam = TEAMID((PlayerTypes)iData1);
	TeamTypes eOtherTeam = TEAMID(pUnit->getCombatOwner(eOurTeam));
	if (pUnit->canCoexistWithEnemyUnit(eOurTeam))
		return false;
	return (iData2 ? eOurTeam != eOtherTeam : GET_TEAM(eOurTeam).AI_mayAttack(eOtherTeam));
}

bool PUF_canDeclareWar(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	FAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");

	TeamTypes eOtherTeam = TEAMID((PlayerTypes)iData1);
	TeamTypes eOurTeam = TEAMID(pUnit->getCombatOwner(eOtherTeam));
	if (pUnit->canCoexistWithEnemyUnit(eOtherTeam))
		return false;

	return (iData2 ? false : GET_TEAM(eOtherTeam).canDeclareWar(eOurTeam));
}

bool PUF_canDefend(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->canDefend();
}

bool PUF_cannotDefend(const CvUnit* pUnit, int iData1, int iData2)
{
	return !(pUnit->canDefend());
}

bool PUF_canDefendGroupHead(const CvUnit* pUnit, int iData1, int iData2)
{
	return (PUF_canDefend(pUnit, iData1, iData2) && PUF_isGroupHead(pUnit, iData1, iData2));
}

bool PUF_canDefendEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	FAssertMsg(iData2 != -1, "Invalid data argument, should be >= 0");
	return (PUF_canDefend(pUnit, iData1, iData2) && PUF_isEnemy(pUnit, iData1, iData2));
}

bool PUF_canDefendPotentialEnemy(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (PUF_canDefend(pUnit, iData1, iData2) && PUF_isPotentialEnemy(pUnit, iData1, iData2));
}

bool PUF_canAirAttack(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->canAirAttack();
}

bool PUF_canAirDefend(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->canAirDefend();
}

bool PUF_isFighting(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->isFighting();
}

bool PUF_isAnimal( const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->isAnimal();
}

bool PUF_isMilitaryHappiness(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->isMilitaryHappiness();
}

bool PUF_isInvestigate(const CvUnit* pUnit, int iData1, int iData2)
{	// <advc.103>
	if(pUnit->hasMoved())
		return false; // </advc.103>
	return pUnit->isInvestigate();
}

bool PUF_isCounterSpy(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->isCounterSpy();
}

bool PUF_isSpy(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->isSpy();
}

bool PUF_isDomainType(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getDomainType() == iData1);
}

bool PUF_isUnitType(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->getUnitType() == iData1);
}

bool PUF_isUnitAIType(const CvUnit* pUnit, int iData1, int iData2)
{
	FAssertMsg(iData1 != -1, "Invalid data argument, should be >= 0");
	return (pUnit->AI_getUnitAIType() == iData1);
}

bool PUF_isCityAIType(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->AI().AI_isCityAIType();
}

bool PUF_isNotCityAIType(const CvUnit* pUnit, int iData1, int iData2)
{
	return !(PUF_isCityAIType(pUnit, iData1, iData2));
}
// advc.003j (comment): unused
bool PUF_isSelected(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->IsSelected();
}

bool PUF_makeInfoBarDirty(CvUnit* pUnit, int iData1, int iData2)
{
	pUnit->setInfoBarDirty(true);
	return true;
}

/*bool PUF_isNoMission(const CvUnit* pUnit, int iData1, int iData2)
{
	//return (pUnit->getGroup()->getActivityType() != ACTIVITY_MISSION); // BtS
	return (pUnit->getGroup()->AI_getMissionAIType() == NO_MISSIONAI); // K-Mod
}*/
/*  <advc.113b> The above won't do for counting the workers available to a city:
	Doesn't count retreated workers and does count workers in cargo. */
/*  Replacement: Count pUnit if its mission plot has the given city as its
	working city. If pUnit has no mission, then it's counted if its current plot
	has the given city as its working city. */
bool PUF_isMissionPlotWorkingCity(const CvUnit* pUnit, int iCity, int iCityOwner)
{
	CvCity* pCity = GET_PLAYER((PlayerTypes)iCityOwner).getCity(iCity);
	if(pCity == NULL || pUnit->isCargo())
		return false;
	CvPlot* pMissionPlot = pUnit->AI().AI_getGroup()->AI_getMissionAIPlot();
	if(pMissionPlot == NULL)
		pMissionPlot = pUnit->plot();
	return (pMissionPlot->getWorkingCity() == pCity);
} // </advc.113b>

bool PUF_isFiniteRange(const CvUnit* pUnit, int iData1, int iData2)
{
	return ((pUnit->getDomainType() != DOMAIN_AIR) || (pUnit->getUnitInfo().getAirRange() > 0));
}
// BETTER_BTS_AI_MOD, General AI, 01/15/09, jdog5000: START
bool PUF_isAvailableUnitAITypeGroupie(const CvUnit* pUnit, int iData1, int iData2)
{
	return ((PUF_isUnitAITypeGroupie(pUnit,iData1,iData2)) && !(pUnit->isCargo()));
}

bool PUF_isUnitAITypeGroupie(const CvUnit* pUnit, int iData1, int iData2)
{
	CvUnit* pGroupHead = pUnit->getGroup()->getHeadUnit();
	return (PUF_isUnitAIType(pGroupHead,iData1,iData2));
}

bool PUF_isFiniteRangeAndNotJustProduced(const CvUnit* pUnit, int iData1, int iData2)
{
	return (PUF_isFiniteRange(pUnit,iData1,iData2) && ((GC.getGame().getGameTurn() - pUnit->getGameTurnCreated()) > 1));
} // BETTER_BTS_AI_MOD: END
// <K-Mod>
bool PUF_isMissionAIType(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->AI().AI_getGroup()->AI_getMissionAIType() == iData1;
}

bool PUF_isAirIntercept(const CvUnit* pUnit, int iData1, int iData2)
{
	return pUnit->getDomainType() == DOMAIN_AIR && pUnit->getGroup()->getActivityType() == ACTIVITY_INTERCEPT;
} // </K-Mod>

// advc.003j (comment): Unused
int baseYieldToSymbol(int iNumYieldTypes, int iYieldStack)
{
	return iNumYieldTypes * GC.getDefineINT("MAX_YIELD_STACK") + iYieldStack;
}
/*  advc.003j: Vanilla Civ 4 function that used to be a DLLExport;
	certainly unused since BtS, and doesn't sound too useful. */
/*bool isPickableName(const TCHAR* szName) {
	if (szName) {
		int iLen = _tcslen(szName);
		if (!_tcsicmp(&szName[iLen-6], "NOPICK"))
			return false;
	}
	return true;
}*/

// create an array of shuffled numbers
int* shuffle(int iNum, CvRandom& rand)
{
	int* piShuffle = new int[iNum];
	shuffleArray(piShuffle, iNum, rand);
	return piShuffle;
}


void shuffleArray(int* piShuffle, int iNum, CvRandom& rand)
{
	for (int iI = 0; iI < iNum; iI++)
		piShuffle[iI] = iI;

	for (int iI = 0; iI < iNum; iI++)
	{
		int iJ = (rand.get(iNum - iI, NULL) + iI);
		if (iI != iJ)
		{
			int iTemp = piShuffle[iI];
			piShuffle[iI] = piShuffle[iJ];
			piShuffle[iJ] = iTemp;
		}
	}
}

// advc.enum: Caller needs to set the vector size
void shuffleVector(std::vector<int>& aiIndices, CvRandom& rand)
{
	std11::iota(aiIndices.begin(), aiIndices.end(), 0);
	int const iSize = (int)aiIndices.size();
	for (int i = 0; i < iSize; i++)
		std::swap(aiIndices[i], aiIndices[rand.get(iSize - i, NULL) + i]);
}


int getTurnYearForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed)
{
	return getTurnMonthForGame(iGameTurn, iStartYear, eCalendar, eSpeed) /
			std::max(1, GC.getNumMonthInfos()); // advc: max
}


int getTurnMonthForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed)
{
	int iTurnMonth = iStartYear * GC.getNumMonthInfos();

	switch (eCalendar)
	{
	case CALENDAR_DEFAULT:
	{
		int iTurnCount = 0;

		for (int iI = 0; iI < GC.getInfo(eSpeed).getNumTurnIncrements(); iI++)
		{
			if (iGameTurn > iTurnCount + GC.getInfo(eSpeed).getGameTurnInfo(iI).iNumGameTurnsPerIncrement)
			{
				iTurnMonth += (GC.getInfo(eSpeed).getGameTurnInfo(iI).iMonthIncrement *
						GC.getInfo(eSpeed).getGameTurnInfo(iI).iNumGameTurnsPerIncrement);
				iTurnCount += GC.getInfo(eSpeed).getGameTurnInfo(iI).iNumGameTurnsPerIncrement;
			}
			else
			{
				iTurnMonth += (GC.getInfo(eSpeed).getGameTurnInfo(iI).iMonthIncrement * (iGameTurn - iTurnCount));
				iTurnCount += (iGameTurn - iTurnCount);
				break;
			}
		}

		if (iGameTurn > iTurnCount)
		{
			iTurnMonth += (GC.getInfo(eSpeed).getGameTurnInfo(GC.getInfo(eSpeed).getNumTurnIncrements() - 1).iMonthIncrement *
					(iGameTurn - iTurnCount));
		}
		break;
	}
	case CALENDAR_BI_YEARLY:
		iTurnMonth += (2 * iGameTurn * GC.getNumMonthInfos());
		break;

	case CALENDAR_YEARS:
	case CALENDAR_TURNS:
		iTurnMonth += iGameTurn * GC.getNumMonthInfos();
		break;

	case CALENDAR_SEASONS:
		iTurnMonth += (iGameTurn * GC.getNumMonthInfos()) / std::max(1, GC.getNumSeasonInfos()); // advc: max
		break;

	case CALENDAR_MONTHS:
		iTurnMonth += iGameTurn;
		break;

	case CALENDAR_WEEKS:
		iTurnMonth += iGameTurn / std::max(1, GC.getDefineINT("WEEKS_PER_MONTHS")); // advc: max
		break;

	default:
		FAssert(false);
	}

	return iTurnMonth;
}


void getDirectionTypeString(CvWString& szString, DirectionTypes eDirectionType)
{
	/*  advc.007: Turned this comment
		"these string functions should only be used under chipotle cheat code (not internationalized)"
		into an assertion: */
	FAssertMsg(gLogBBAI || GC.getGame().isDebugMode(), "getDirectionTypeString should only be used for Debug output");
	switch (eDirectionType)
	{
	case NO_DIRECTION: szString = L"NO_DIRECTION"; break;

	case DIRECTION_NORTH: szString = L"north"; break;
	case DIRECTION_NORTHEAST: szString = L"northeast"; break;
	case DIRECTION_EAST: szString = L"east"; break;
	case DIRECTION_SOUTHEAST: szString = L"southeast"; break;
	case DIRECTION_SOUTH: szString = L"south"; break;
	case DIRECTION_SOUTHWEST: szString = L"southwest"; break;
	case DIRECTION_WEST: szString = L"west"; break;
	case DIRECTION_NORTHWEST: szString = L"northwest"; break;

	default: szString = CvWString::format(L"UNKNOWN_DIRECTION(%d)", eDirectionType); break;
	}
}

void getCardinalDirectionTypeString(CvWString& szString, CardinalDirectionTypes eDirectionType)
{
	getDirectionTypeString(szString, cardinalDirectionToDirection(eDirectionType));
}

// advc.007: Removed the "ACTIVITY_" prefix from the strings b/c it takes up too much space.
void getActivityTypeString(CvWString& szString, ActivityTypes eActivityType)
{
	FAssertMsg(gLogBBAI || GC.getGame().isDebugMode(), "getActivityTypeString should only be used for Debug output"); // advc.007
	switch (eActivityType)
	{
	case NO_ACTIVITY: szString = L"NO_ACTIVITY"; break;

	case ACTIVITY_AWAKE: szString = L"AWAKE"; break;
	case ACTIVITY_HOLD: szString = L"HOLD"; break;
	case ACTIVITY_SLEEP: szString = L"SLEEP"; break;
	case ACTIVITY_HEAL: szString = L"HEAL"; break;
	case ACTIVITY_SENTRY: szString = L"SENTRY"; break;
	case ACTIVITY_INTERCEPT: szString = L"INTERCEPT"; break;
	case ACTIVITY_MISSION: szString = L"MISSION"; break;
// K-Mod. There were some missing activity strings...
/*#define case_string(x) case x: szString = L#x; break;
	case_string(ACTIVITY_PATROL)
	case_string(ACTIVITY_PLUNDER)
#undef case_string*/
// K-Mod end
	// <advc.007>
	case ACTIVITY_PATROL: szString = L"PATROL"; break;
	case ACTIVITY_PLUNDER: szString = L"PLUNDER"; break;
	// </advc.007>
	case ACTIVITY_BOARDED: szString = L"BOARDED"; break; // advc.075
	default: szString = CvWString::format(L"UNKNOWN_ACTIVITY(%d)", eActivityType); break;
	}
}

void getMissionTypeString(CvWString& szString, MissionTypes eMissionType)
{
	FAssertMsg(gLogBBAI || GC.getGame().isDebugMode(), "getMissionTypeString should only be used for Debug output"); // advc.007
	switch (eMissionType)
	{
	case NO_MISSION: szString = L"NO_MISSION"; break;

	case MISSION_MOVE_TO: szString = L"MISSION_MOVE_TO"; break;
	case MISSION_ROUTE_TO: szString = L"MISSION_ROUTE_TO"; break;
	case MISSION_MOVE_TO_UNIT: szString = L"MISSION_MOVE_TO_UNIT"; break;
	case MISSION_SKIP: szString = L"MISSION_SKIP"; break;
	case MISSION_SLEEP: szString = L"MISSION_SLEEP"; break;
	case MISSION_FORTIFY: szString = L"MISSION_FORTIFY"; break;
	case MISSION_PLUNDER: szString = L"MISSION_PLUNDER"; break;
	case MISSION_AIRPATROL: szString = L"MISSION_AIRPATROL"; break;
	case MISSION_SEAPATROL: szString = L"MISSION_SEAPATROL"; break;
	case MISSION_HEAL: szString = L"MISSION_HEAL"; break;
	case MISSION_SENTRY_HEAL: szString = L"MISSION_SENTRY_HEAL"; break; // advc.004l
	case MISSION_SENTRY: szString = L"MISSION_SENTRY"; break;
	case MISSION_AIRLIFT: szString = L"MISSION_AIRLIFT"; break;
	case MISSION_NUKE: szString = L"MISSION_NUKE"; break;
	case MISSION_RECON: szString = L"MISSION_RECON"; break;
	case MISSION_PARADROP: szString = L"MISSION_PARADROP"; break;
	case MISSION_AIRBOMB: szString = L"MISSION_AIRBOMB"; break;
	case MISSION_BOMBARD: szString = L"MISSION_BOMBARD"; break;
	case MISSION_PILLAGE: szString = L"MISSION_PILLAGE"; break;
	case MISSION_SABOTAGE: szString = L"MISSION_SABOTAGE"; break;
	case MISSION_DESTROY: szString = L"MISSION_DESTROY"; break;
	case MISSION_STEAL_PLANS: szString = L"MISSION_STEAL_PLANS"; break;
	case MISSION_FOUND: szString = L"MISSION_FOUND"; break;
	case MISSION_SPREAD: szString = L"MISSION_SPREAD"; break;
	case MISSION_SPREAD_CORPORATION: szString = L"MISSION_SPREAD_CORPORATION"; break;
	case MISSION_JOIN: szString = L"MISSION_JOIN"; break;
	case MISSION_CONSTRUCT: szString = L"MISSION_CONSTRUCT"; break;
	case MISSION_DISCOVER: szString = L"MISSION_DISCOVER"; break;
	case MISSION_HURRY: szString = L"MISSION_HURRY"; break;
	case MISSION_TRADE: szString = L"MISSION_TRADE"; break;
	case MISSION_GREAT_WORK: szString = L"MISSION_GREAT_WORK"; break;
	case MISSION_INFILTRATE: szString = L"MISSION_INFILTRATE"; break;
	case MISSION_GOLDEN_AGE: szString = L"MISSION_GOLDEN_AGE"; break;
	case MISSION_BUILD: szString = L"MISSION_BUILD"; break;
	case MISSION_LEAD: szString = L"MISSION_LEAD"; break;
	case MISSION_ESPIONAGE: szString = L"MISSION_ESPIONAGE"; break;
	case MISSION_DIE_ANIMATION: szString = L"MISSION_DIE_ANIMATION"; break;

	case MISSION_BEGIN_COMBAT: szString = L"MISSION_BEGIN_COMBAT"; break;
	case MISSION_END_COMBAT: szString = L"MISSION_END_COMBAT"; break;
	case MISSION_AIRSTRIKE: szString = L"MISSION_AIRSTRIKE"; break;
	case MISSION_SURRENDER: szString = L"MISSION_SURRENDER"; break;
	case MISSION_CAPTURED: szString = L"MISSION_CAPTURED"; break;
	case MISSION_IDLE: szString = L"MISSION_IDLE"; break;
	case MISSION_DIE: szString = L"MISSION_DIE"; break;
	case MISSION_DAMAGE: szString = L"MISSION_DAMAGE"; break;
	case MISSION_MULTI_SELECT: szString = L"MISSION_MULTI_SELECT"; break;
	case MISSION_MULTI_DESELECT: szString = L"MISSION_MULTI_DESELECT"; break;

	default: szString = CvWString::format(L"UNKOWN_MISSION(%d)", eMissionType); break;
	}
}

// advc.007: Removed the "MISSIONAI_" prefix from the strings b/c it takes up too much space.
void getMissionAIString(CvWString& szString, MissionAITypes eMissionAI)
{
	FAssertMsg(gLogBBAI || GC.getGame().isDebugMode(), "getMissionAIString should only be used for Debug output"); // advc.007
	switch (eMissionAI)
	{
	case NO_MISSIONAI: szString = L"NO_MISSIONAI"; break;

	case MISSIONAI_SHADOW: szString = L"SHADOW"; break;
	case MISSIONAI_GROUP: szString = L"GROUP"; break;
	case MISSIONAI_LOAD_ASSAULT: szString = L"LOAD_ASSAULT"; break;
	case MISSIONAI_LOAD_SETTLER: szString = L"LOAD_SETTLER"; break;
	case MISSIONAI_LOAD_SPECIAL: szString = L"LOAD_SPECIAL"; break;
	case MISSIONAI_GUARD_CITY: szString = L"GUARD_CITY"; break;
	case MISSIONAI_GUARD_BONUS: szString = L"GUARD_BONUS"; break;
	case MISSIONAI_GUARD_SPY: szString = L"GUARD_SPY"; break;
	case MISSIONAI_ATTACK_SPY: szString = L"ATTACK_SPY"; break;
	case MISSIONAI_SPREAD: szString = L"SPREAD"; break;
	case MISSIONAI_CONSTRUCT: szString = L"CONSTRUCT"; break;
	case MISSIONAI_HURRY: szString = L"HURRY"; break;
	case MISSIONAI_GREAT_WORK: szString = L"GREAT_WORK"; break;
	case MISSIONAI_EXPLORE: szString = L"EXPLORE"; break;
	case MISSIONAI_BLOCKADE: szString = L"BLOCKADE"; break;
	case MISSIONAI_PILLAGE: szString = L"PILLAGE"; break;
	case MISSIONAI_FOUND: szString = L"FOUND"; break;
	case MISSIONAI_BUILD: szString = L"BUILD"; break;
	case MISSIONAI_ASSAULT: szString = L"ASSAULT"; break;
	case MISSIONAI_CARRIER: szString = L"CARRIER"; break;
	case MISSIONAI_PICKUP: szString = L"PICKUP"; break;
// K-Mod
#define mission_string(x) case x: szString = L#x; break;
	mission_string(MISSIONAI_GUARD_COAST)
	mission_string(MISSIONAI_REINFORCE)
	mission_string(MISSIONAI_SPREAD_CORPORATION)
	mission_string(MISSIONAI_RECON_SPY)
	mission_string(MISSIONAI_JOIN_CITY)
	mission_string(MISSIONAI_TRADE)
	mission_string(MISSIONAI_INFILTRATE)
	mission_string(MISSIONAI_CHOKE)
	mission_string(MISSIONAI_HEAL)
	mission_string(MISSIONAI_RETREAT)
	mission_string(MISSIONAI_PATROL)
	mission_string(MISSIONAI_DEFEND)
	mission_string(MISSIONAI_COUNTER_ATTACK)
	mission_string(MISSIONAI_UPGRADE)
	mission_string(MISSIONAI_STRANDED)
#undef mission_string
// K-Mod end
	default: szString = CvWString::format(L"UNKOWN_MISSION_AI(%d)", eMissionAI); break;
	}
}

void getUnitAIString(CvWString& szString, UnitAITypes eUnitAI)
{
	FAssertMsg(gLogBBAI || GC.getGame().isDebugMode(), "getUnitAIString should only be used for Debug output"); // advc.007

	// note, GC.getInfo(eUnitAI).getDescription() is a international friendly way to get string (but it will be longer)

	switch (eUnitAI)
	{
	case NO_UNITAI: szString = L"no unitAI"; break;

	case UNITAI_UNKNOWN: szString = L"unknown"; break;
	case UNITAI_ANIMAL: szString = L"animal"; break;
	case UNITAI_SETTLE: szString = L"settle"; break;
	case UNITAI_WORKER: szString = L"worker"; break;
	case UNITAI_ATTACK: szString = L"attack"; break;
	case UNITAI_ATTACK_CITY: szString = L"attack city"; break;
	case UNITAI_COLLATERAL: szString = L"collateral"; break;
	case UNITAI_PILLAGE: szString = L"pillage"; break;
	case UNITAI_RESERVE: szString = L"reserve"; break;
	case UNITAI_COUNTER: szString = L"counter"; break;
	case UNITAI_CITY_DEFENSE: szString = L"city defense"; break;
	case UNITAI_CITY_COUNTER: szString = L"city counter"; break;
	case UNITAI_CITY_SPECIAL: szString = L"city special"; break;
	case UNITAI_EXPLORE: szString = L"explore"; break;
	case UNITAI_MISSIONARY: szString = L"missionary"; break;
	case UNITAI_PROPHET: szString = L"prophet"; break;
	case UNITAI_ARTIST: szString = L"artist"; break;
	case UNITAI_SCIENTIST: szString = L"scientist"; break;
	case UNITAI_GENERAL: szString = L"general"; break;
	case UNITAI_MERCHANT: szString = L"merchant"; break;
	case UNITAI_ENGINEER: szString = L"engineer"; break;
	case UNITAI_GREAT_SPY: szString = L"great spy"; break; // K-Mod
	case UNITAI_SPY: szString = L"spy"; break;
	case UNITAI_ICBM: szString = L"icbm"; break;
	case UNITAI_WORKER_SEA: szString = L"worker sea"; break;
	case UNITAI_ATTACK_SEA: szString = L"attack sea"; break;
	case UNITAI_RESERVE_SEA: szString = L"reserve sea"; break;
	case UNITAI_ESCORT_SEA: szString = L"escort sea"; break;
	case UNITAI_EXPLORE_SEA: szString = L"explore sea"; break;
	case UNITAI_ASSAULT_SEA: szString = L"assault sea"; break;
	case UNITAI_SETTLER_SEA: szString = L"settler sea"; break;
	case UNITAI_MISSIONARY_SEA: szString = L"missionary sea"; break;
	case UNITAI_SPY_SEA: szString = L"spy sea"; break;
	case UNITAI_CARRIER_SEA: szString = L"carrier sea"; break;
	case UNITAI_MISSILE_CARRIER_SEA: szString = L"missile carrier"; break;
	case UNITAI_PIRATE_SEA: szString = L"pirate sea"; break;
	case UNITAI_ATTACK_AIR: szString = L"attack air"; break;
	case UNITAI_DEFENSE_AIR: szString = L"defense air"; break;
	case UNITAI_CARRIER_AIR: szString = L"carrier air"; break;
	case UNITAI_MISSILE_AIR: szString = L"missile air"; break; // K-Mod (this string was missing)
	case UNITAI_PARADROP: szString = L"paradrop"; break;
	case UNITAI_ATTACK_CITY_LEMMING: szString = L"attack city lemming"; break;

	default: szString = CvWString::format(L"unknown(%d)", eUnitAI); break;
	}
}

// Lead From Behind by UncutDragon
typedef vector<int> LFBoddsAttOdds;
typedef vector<LFBoddsAttOdds> LFBoddsDefRounds;
typedef vector<LFBoddsDefRounds> LFBoddsAttRounds;
typedef vector<LFBoddsAttRounds> LFBoddsFirstStrike;
int LFBlookupCombatOdds(int iFirstStrikes, int iNeededRoundsAttacker, int iNeededRoundsDefender, int iAttackerOdds);
int LFBlookupCombatOdds(LFBoddsFirstStrike* pOdds, int iFSIndex, int iFirstStrikes, int iNeededRoundsAttacker, int iNeededRoundsDefender, int iAttackerOdds);
int LFBlookupCombatOdds(LFBoddsAttOdds* pOdds, int iOddsIndex, int iFirstStrikes, int iNeededRoundsAttacker, int iNeededRoundsDefender, int iAttackerOdds);
int LFBcalculateCombatOdds(int iFirstStrikes, int iNeededRoundsAttacker, int iNeededRoundsDefender, int iAttackerOdds);

const int LFB_ODDS_INTERVAL_SIZE = 16;
const int LFB_ODDS_EXTRA_ACCURACY = 32;
LFBoddsFirstStrike pOddsCacheFSPos;
LFBoddsFirstStrike pOddsCacheFSNeg;

// gets the combat odds using precomputed attacker/defender values instead of unit pointers
int LFBgetCombatOdds(int iAttackerLowFS,	int iAttackerHighFS, int iDefenderLowFS, int iDefenderHighFS, int iNeededRoundsAttacker, int iNeededRoundsDefender, int iAttackerOdds)
{
	int iDefenderOdds;
	bool bFlip = false;
	int iFirstStrikes;
	int iI;
	int iJ;
	int iOdds = 0;

	// Essentially, this means we're attacking with a seige engine and the defender is already at or below the max combat limit
	// We're not allowed to attack regardless, since we can't do any damage - just return 100%
	if (iNeededRoundsAttacker == 0)
		return 1000;
	// Because the best defender code calls us from the defender's perspective, we also need to check 'defender' rounds zero
	if (iNeededRoundsDefender == 0)
		return 0;

	// If attacker has better than even chance to hit, we just flip it and calculate defender's chance to win
	// This reduces how much we cache considerably (by half just from the fact we're only dealing with half the odds
	// - but additionally, iNeededRounds'Defender' is guaranteed to stay low - at most 5 with standard settings).
	iDefenderOdds = GC.getCOMBAT_DIE_SIDES() - iAttackerOdds;
	if (iAttackerOdds > iDefenderOdds)
		bFlip = true;

	// This is basically the two outside loops at the end of the standard getCombatOdds
	// We just call our cache lookup in the middle (flipped if necessary) instead of the actual computation
	for (iI = iAttackerLowFS; iI < iAttackerHighFS + 1; iI++)
	{
		for (iJ = iDefenderLowFS; iJ < iDefenderHighFS + 1; iJ++)
		{
			iFirstStrikes = iI - iJ;
			if (bFlip)
				iOdds += LFBlookupCombatOdds(-iFirstStrikes, iNeededRoundsDefender, iNeededRoundsAttacker, iDefenderOdds);
			else
				iOdds += LFBlookupCombatOdds(iFirstStrikes, iNeededRoundsAttacker, iNeededRoundsDefender, iAttackerOdds);
		}
	}

	// Odds are a straight average of all the FS combinations (since all are equally possible)
	iOdds /= ((iAttackerHighFS - iAttackerLowFS + 1) * (iDefenderHighFS - iDefenderLowFS + 1));

	// Now that we have the final odds, we can remove the extra accuracy, rounding off
	iOdds = (iOdds + (LFB_ODDS_EXTRA_ACCURACY/2)) / LFB_ODDS_EXTRA_ACCURACY;

	// If we flipped the perspective in the computation/lookup, need to flip it back now
	if (bFlip)
		iOdds = 1000 - iOdds;

	return iOdds;
}

// lookup the combat odds in the cache for a specific sub-result
int LFBlookupCombatOdds(int iFirstStrikes, int iNeededRoundsAttacker, int iNeededRoundsDefender, int iAttackerOdds)
{
	int iOdds = 0;

	// We actually maintain two caches - one for positive first strikes (plus zero), and one for negative
	// This just makes the indices (and growing the array as needed) easy
	if (iFirstStrikes < 0)
		iOdds = LFBlookupCombatOdds(&pOddsCacheFSNeg, (-iFirstStrikes)-1, iFirstStrikes, iNeededRoundsAttacker, iNeededRoundsDefender, iAttackerOdds);
	else
		iOdds = LFBlookupCombatOdds(&pOddsCacheFSPos, iFirstStrikes, iFirstStrikes, iNeededRoundsAttacker, iNeededRoundsDefender, iAttackerOdds);

	return iOdds;
}

int LFBlookupCombatOdds(LFBoddsFirstStrike* pOdds, int iFSIndex, int iFirstStrikes, int iNeededRoundsAttacker, int iNeededRoundsDefender, int iAttackerOdds)
{
	// Grow the arrays as needed
	// First dimension is the first strikes
	int iInsert = iFSIndex - (int)(*pOdds).size() + 1;
	if (iInsert > 0)
	{
		LFBoddsAttRounds pAdd;
		(*pOdds).insert((*pOdds).end(), iInsert, pAdd);
	}

	// Second dimension is the attacker rounds (starting at 1)
	LFBoddsAttRounds* pAttRounds = &((*pOdds)[iFSIndex]);
	iInsert = iNeededRoundsAttacker - (int)(*pAttRounds).size();
	if (iInsert > 0)
	{
		LFBoddsDefRounds pAdd;
		(*pAttRounds).insert((*pAttRounds).end(), iInsert, pAdd);
	}

	// Third dimension is the defender rounds (starting at 1)
	LFBoddsDefRounds* pDefRounds = &((*pAttRounds)[iNeededRoundsAttacker-1]);
	iInsert = iNeededRoundsDefender - (int)(*pDefRounds).size();
	if (iInsert > 0)
	{
		LFBoddsAttOdds pAdd;
		(*pDefRounds).insert((*pDefRounds).end(), iInsert, pAdd);
	}

	// Fourth (last) dimension is the odds index (odds/16)
	LFBoddsAttOdds* pAttOdds = &((*pDefRounds)[iNeededRoundsDefender-1]);

	// Round down to the nearest interval
	int iMinOddsIndex = iAttackerOdds / LFB_ODDS_INTERVAL_SIZE;
	int iMinOddsValue = iMinOddsIndex * LFB_ODDS_INTERVAL_SIZE;

	// Lookup the odds for the rounded down value
	int iOdds = LFBlookupCombatOdds(pAttOdds, iMinOddsIndex, iFirstStrikes, iNeededRoundsAttacker, iNeededRoundsDefender, iMinOddsValue);

	// If we happened to hit an interval exactly, we're done
	if (iMinOddsValue < iAttackerOdds)
	{
		// 'Round up' to the nearest interval - we don't actually need to compute it, we know
		// it's one more than the rounded down interval
		int iMaxOddsIndex = iMinOddsIndex+1;
		int iMaxOddsValue = iMinOddsValue+LFB_ODDS_INTERVAL_SIZE;

		// Lookup the odds for the rounded up value
		int iMaxOdds = LFBlookupCombatOdds(pAttOdds, iMaxOddsIndex, iFirstStrikes, iNeededRoundsAttacker, iNeededRoundsDefender, iMaxOddsValue);

		// Do a simple weighted average on the two odds
		//iOdds += (((iAttackerOdds - iMinOddsValue) * (iMaxOdds - iOdds)) / LFB_ODDS_INTERVAL_SIZE);
		iOdds += ((iAttackerOdds - iMinOddsValue) * (iMaxOdds - iOdds) + LFB_ODDS_INTERVAL_SIZE/2) / LFB_ODDS_INTERVAL_SIZE; // K-Mod. (rounded rather than truncated)
	}

	return iOdds;
}

int LFBlookupCombatOdds(LFBoddsAttOdds* pOdds, int iOddsIndex, int iFirstStrikes, int iNeededRoundsAttacker, int iNeededRoundsDefender, int iAttackerOdds)
{
	int iNotComputed = -1;

	// Index 0 -> AttackerOdds 0 -> no chance to win
	if (iOddsIndex == 0)
		return 0;

	// We don't store all possible indices, just what we need/use
	// So use position 0 to keep track of what index we start with
	int iFirstIndex = iOddsIndex;
	if ((*pOdds).size() == 0)
		(*pOdds).push_back(iFirstIndex);
	else
		iFirstIndex = (*pOdds)[0];

	int iRealIndex = iOddsIndex - iFirstIndex + 1;

	// Index is before the start of our array
	int iInsert = -iRealIndex+1;
	if (iInsert > 0)
	{
		(*pOdds).insert((*pOdds).begin()+1, iInsert, iNotComputed);
		iFirstIndex -= iInsert;
		iRealIndex = 1;
		(*pOdds)[0] = iFirstIndex;
	}

	// Index is past the end of our array
	iInsert = iRealIndex - (int)(*pOdds).size() + 1;
	if (iInsert > 0)
		(*pOdds).insert((*pOdds).end(), iInsert, iNotComputed);

	// Retrieve the odds from the array
	int iOdds = (*pOdds)[iRealIndex];

	// Odds aren't cached yet - need to actually calculate them
	if (iOdds == iNotComputed)
	{
		iOdds = LFBcalculateCombatOdds(iFirstStrikes, iNeededRoundsAttacker, iNeededRoundsDefender, iAttackerOdds);
		(*pOdds)[iRealIndex] = iOdds;
	}

	return iOdds;
}

// Perform the actual odds calculation (basically identical to the default algorithm, except that we retain a little more accuracy)
int LFBcalculateCombatOdds(int iFirstStrikes, int iNeededRoundsAttacker, int iNeededRoundsDefender, int iAttackerOdds)
{
	float fOddsEvent;
	float fOddsAfterEvent;
	int iMaxRounds = iNeededRoundsAttacker + iNeededRoundsDefender - 1;
	int iOdds = 0;
	int iI3;
	int iI4;

	// This part is basically the inside of the outer two loops at the end of the standard getCombatOdds
	if (iFirstStrikes > 0)
	{
		// Attacker gets more or equal first strikes than defender

		// For every possible first strike getting hit, calculate both
		// the chance of that event happening, as well as the rest of
		// the chance assuming the event has happened. Multiply these
		// together to get the total chance (Bayes rule).
		// iI3 counts the number of successful first strikes
		//////

		for (iI3 = 0; iI3 < (iFirstStrikes + 1); iI3++)
		{
			// event: iI3 first strikes hit the defender

			// calculate chance of iI3 first strikes hitting: fOddsEvent
			// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
			// this needs to be in floating point math
			//////

			fOddsEvent = ((float)getBinomialCoefficient(iFirstStrikes, iI3)) * std::pow((((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES()), iI3) * std::pow((1.0f - (((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES())), (iFirstStrikes - iI3));

			// calculate chance assuming iI3 first strike hits: fOddsAfterEvent
			//////

			if (iI3 >= iNeededRoundsAttacker)
			{
				fOddsAfterEvent = 1;
			}
			else
			{
				fOddsAfterEvent = 0;

				// odds for _at_least_ (iNeededRoundsAttacker - iI3) (the remaining hits
				// the attacker needs to make) out of (iMaxRounds - iI3) (the left over
				// rounds) is the sum of each _exact_ draw
				//////

				for (iI4 = (iNeededRoundsAttacker - iI3); iI4 < (iMaxRounds - iI3 + 1); iI4++)
				{
					// odds of exactly iI4 out of (iMaxRounds - iI3) draws.
					// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
					// this needs to be in floating point math
					//////

					fOddsAfterEvent += ((float)getBinomialCoefficient((iMaxRounds - iI3), iI4)) * std::pow((((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES()), iI4) * std::pow((1.0f - (((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES())), ((iMaxRounds - iI3) - iI4));
				}
			}

			// Multiply these together, round them properly, and add
			// the result to the total iOdds
			//////

			iOdds += ((int)((1000.0 * fOddsEvent * fOddsAfterEvent * (float)LFB_ODDS_EXTRA_ACCURACY) + 0.5));
		}
	}
	else // (iI < iJ)
	{
		// Attacker gets less first strikes than defender
		int iDefenderOdds = GC.getCOMBAT_DIE_SIDES() - iAttackerOdds;
		iFirstStrikes *= -1;

		// For every possible first strike getting hit, calculate both
		// the chance of that event happening, as well as the rest of
		// the chance assuming the event has happened. Multiply these
		// together to get the total chance (Bayes rule).
		// iI3 counts the number of successful first strikes
		//////

		for (iI3 = 0; iI3 < (iFirstStrikes + 1); iI3++)
		{
			// event: iI3 first strikes hit the defender

			// First of all, check if the attacker is still alive.
			// Otherwise, no further calculations need to occur
			/////

			if (iI3 < iNeededRoundsDefender)
			{
				// calculate chance of iI3 first strikes hitting: fOddsEvent
				// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
				// this needs to be in floating point math
				//////

				fOddsEvent = ((float)getBinomialCoefficient(iFirstStrikes, iI3)) * std::pow((((float)iDefenderOdds) / GC.getCOMBAT_DIE_SIDES()), iI3) * std::pow((1.0f - (((float)iDefenderOdds) / GC.getCOMBAT_DIE_SIDES())), (iFirstStrikes - iI3));

				// calculate chance assuming iI3 first strike hits: fOddsAfterEvent
				//////

				fOddsAfterEvent = 0;

				// odds for _at_least_ iNeededRoundsAttacker (the remaining hits
				// the attacker needs to make) out of (iMaxRounds - iI3) (the left over
				// rounds) is the sum of each _exact_ draw
				//////

				for (iI4 = iNeededRoundsAttacker; iI4 < (iMaxRounds - iI3 + 1); iI4++)
				{

					// odds of exactly iI4 out of (iMaxRounds - iI3) draws.
					// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
					// this needs to be in floating point math
					//////

					fOddsAfterEvent += ((float)getBinomialCoefficient((iMaxRounds - iI3), iI4)) * std::pow((((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES()), iI4) * std::pow((1.0f - (((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES())), ((iMaxRounds - iI3) - iI4));
				}

				// Multiply these together, round them properly, and add
				// the result to the total iOdds
				//////

				iOdds += ((int)((1000.0 * fOddsEvent * fOddsAfterEvent * (float)LFB_ODDS_EXTRA_ACCURACY)+0.5));
			}
		}
	}

	return iOdds;
}
// lfb end
