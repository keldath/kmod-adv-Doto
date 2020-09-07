// selectionGroupAI.cpp

#include "CvGameCoreDLL.h"
#include "CvSelectionGroupAI.h"
#include "CvUnitAI.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "AgentIterator.h"
#include "CvMap.h"


CvSelectionGroupAI::CvSelectionGroupAI()
{
	AI_reset();
}


CvSelectionGroupAI::~CvSelectionGroupAI()
{
	AI_uninit();
}


void CvSelectionGroupAI::AI_init()
{
	AI_reset();
}


void CvSelectionGroupAI::AI_uninit() {}


void CvSelectionGroupAI::AI_reset()
{
	AI_uninit();

	m_iMissionAIX = INVALID_PLOT_COORD;
	m_iMissionAIY = INVALID_PLOT_COORD;

	m_bForceSeparate = false;

	m_eMissionAIType = NO_MISSIONAI;

	m_missionAIUnit.reset();

	m_bGroupAttack = false;
	m_iGroupAttackX = -1;
	m_iGroupAttackY = -1;
}

// these separate function have been tweaked by K-Mod and bbai.
void CvSelectionGroupAI::AI_separate()
{
	CLLNode<IDInfo>* pEntityNode = headUnitNode();
	while (pEntityNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		pLoopUnit->joinGroup(NULL);
	}
}

void CvSelectionGroupAI::AI_separateNonAI(UnitAITypes eUnitAI)
{
	CLLNode<IDInfo>* pEntityNode = headUnitNode();
	while (pEntityNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		if (pLoopUnit->AI_getUnitAIType() != eUnitAI)
			pLoopUnit->joinGroup(NULL);
	}
}

void CvSelectionGroupAI::AI_separateAI(UnitAITypes eUnitAI)
{
	CLLNode<IDInfo>* pEntityNode = headUnitNode();
	while (pEntityNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
			pLoopUnit->joinGroup(NULL);
	}
}

bool CvSelectionGroupAI::AI_separateImpassable()
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	bool bSeparated = false;

	CLLNode<IDInfo>* pEntityNode = headUnitNode();
	while (pEntityNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		if (kPlayer.AI_isAnyImpassable(pLoopUnit->getUnitType()))
		{
			pLoopUnit->joinGroup(NULL);
			bSeparated = true;
		}
	}
	return bSeparated;
}

bool CvSelectionGroupAI::AI_separateEmptyTransports()
{
	bool bSeparated = false;

	CLLNode<IDInfo>* pEntityNode = headUnitNode();
	while (pEntityNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		if (pLoopUnit->AI_getUnitAIType() == UNITAI_ASSAULT_SEA && pLoopUnit->getCargo() == 0)
		{
			pLoopUnit->joinGroup(NULL);
			bSeparated = true;
		}
	}
	return bSeparated;
}
// bbai / K-Mod end

// Returns true if the group has become busy...
bool CvSelectionGroupAI::AI_update()
{
	PROFILE_FUNC();

	FAssert(getOwner() != NO_PLAYER);

	if (!AI_isControlled())
		return false;

	if (getNumUnits() == 0)
		return false;

	// K-Mod. (replacing the original "isForceUpdate" stuff.)
	if (isForceUpdate())
		AI_cancelGroupAttack(); // note: we haven't toggled the update flag, nor woken the group from sleep.
	// K-Mod end

	//FAssert(!(GET_PLAYER(getOwner()).isAutoMoves())); // (no longer true in K-Mod)

	//int iTempHack = 0; // XXX
	// <advc.001y> Will keep this permanently as a fallback
	int iAttempts = 0;
	int iMaxAttempts = 6 * (GET_PLAYER(getOwner()).getCurrentEra() + 1) +
			std::max(getNumUnits(), 4);
	// </advc.001y>
	bool bDead = false;
	bool bFailedAlreadyFighting = false;
	//while ((m_bGroupAttack && !bFailedAlreadyFighting) || readyToMove())
	while ((AI_isGroupAttack() && !isBusy()) || readyToMove()) // K-Mod
	{
		setForceUpdate(false); // K-Mod. Force update just means we should get into this loop at least once.
		iAttempts++;
		/*  <advc.001y> Moved out of the block below so I can see what the loop does
			before it terminates. Debugger stops in CvSelectionGroup::pushMission and
			startMission have been helpful to me. */
		FAssertMsg(iAttempts != iMaxAttempts - 5, "Unit stuck in a loop");
		if(iAttempts >= iMaxAttempts) // was > 100 </advc.001y>
		{
			CvUnit* pHeadUnit = getHeadUnit();
			if (pHeadUnit != NULL)
			{
				if (iAttempts == iMaxAttempts) // advc.001y: Don't spam the log
					GC.getLogger().logUnitStuck(*pHeadUnit); // advc.003t
				pHeadUnit->finishMoves();
			}
			break;
		}

		// if we want to force the group to attack, force another attack
		if (AI_isGroupAttack())
		{
			AI_cancelGroupAttack();
			groupAttack(m_iGroupAttackX, m_iGroupAttackY, MOVE_DIRECT_ATTACK, bFailedAlreadyFighting);
		}
		// else pick AI action
		else
		{
			CvUnitAI* pHeadUnit = AI_getHeadUnit();
			//if (pHeadUnit == NULL || pHeadUnit->isDelayedDeath())
			if (pHeadUnit == NULL || pHeadUnit->doDelayedDeath()) // K-Mod
				break;

			//resetPath();
			if (pHeadUnit->AI_update())
			{	// AI_update returns true when we should abort the loop and wait until next slice
				FAssert(!pHeadUnit->isDelayedDeath());
				break;
			}
		}

		if (doDelayedDeath())
		{
			bDead = true;
			break;
		}

		// if no longer group attacking, and force separate is true, then bail, decide what to do after group is split up
		// (UnitAI of head unit may have changed)
		if (!AI_isGroupAttack() && AI_isForceSeparate())
		{
			AI_separate();	// pointers could become invalid...
			//return true;
			return false; // K-Mod
		}
	}

	if (!bDead)
	{
		// K-Mod. this is how we deal with force update when some group members can't move.
		if (isForceUpdate())
		{
			setForceUpdate(false);
			AI_cancelGroupAttack();
			setActivityType(ACTIVITY_AWAKE);
		}
		// K-Mod end
		if (!isHuman())
		{
			bool bFollow = false;
			// <k146>
			// if we're not group attacking, then check for 'follow' action
			if (!AI_isGroupAttack() && readyToMove(true))
			{
				/*  What we do here might split the group. So to avoid problems,
					lets make a list of our units. */
				std::vector<IDInfo> originalGroup;
				for(CLLNode<IDInfo> const* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
 				{
					originalGroup.push_back(pUnitNode->m_data);
				}
				FAssert(originalGroup.size() == getNumUnits());
				bool bFirst = true;
				path_finder.Reset();
				for (std::vector<IDInfo>::iterator it = originalGroup.begin(); it != originalGroup.end(); ++it)
				{
					CvUnitAI* pLoopUnit = ::AI_getUnit(*it);
					if (pLoopUnit && pLoopUnit->getGroupID() == getID() && pLoopUnit->canMove())
					{
						if (pLoopUnit->AI_follow(bFirst))
						{
							bFollow = true;
							bFirst = true; // let the next unit start fresh.
							path_finder.Reset();
							if (!readyToMove(true))
								break;
						}
						else bFirst = false;
					}
				} // </k146>
			}

			if (doDelayedDeath())
				bDead = true;

			if (!bDead)
			{
				if (!bFollow && readyToMove(true))
					pushMission(MISSION_SKIP);
			}
		}
	}
	// <advc.test>
	/*if(GC.getRandLogging() && !GC.getGame().checkInSync()) {
		CvUnit* pHeadUnit = getHeadUnit(); // for inspection in debugger
		FAssert(false);
	}*/ // </advc.test>

	if (bDead)
	{	//return true;
		return false; // K-Mod
	}

	return (isBusy() || isCargoBusy());
}

// Returns attack odds out of 100 (the higher, the better...)
int CvSelectionGroupAI::AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy) const
{
	PROFILE_FUNC();

	FAssert(getOwner() != NO_PLAYER);
	//if (pPlot->getBestDefender(NO_PLAYER, getOwner(), NULL, !bPotentialEnemy, bPotentialEnemy) == NULL)
	// BETTER_BTS_AI_MOD, Efficiency, Lead From Behind (UncutDragon), 02/21/10, jdog5000:
	if (!pPlot->hasDefender(false, NO_PLAYER, getOwner(), NULL, !bPotentialEnemy, bPotentialEnemy))
		return 100;

	int iOdds = 0;
	CvUnit* pAttacker = AI_getBestGroupAttacker(pPlot, bPotentialEnemy, iOdds);
	if (pAttacker == NULL)
		return 0;

	return iOdds;
}

/*	K-Mod. A new odds-adjusting function to replace CvUnitAI::AI_finalOddsThreshold.
	(note: I would like to put this in CvSelectionGroupAI ... but - well -
	I don't need to say it, right?)
	advc.003u: I think CvUnitAI::AI_getGroup solves karadoc's problem; so - moved. */
int CvSelectionGroupAI::AI_getWeightedOdds(CvPlot const* pPlot, bool bPotentialEnemy)
{
	PROFILE_FUNC();
	int iOdds;
	CvUnitAI* pAttacker = AI_getBestGroupAttacker(pPlot, bPotentialEnemy, iOdds);
	if (!pAttacker)
		return 0;
	CvUnit* pDefender = pPlot->getBestDefender(NO_PLAYER, getOwner(), pAttacker,
			!bPotentialEnemy, bPotentialEnemy,
			true, false); // advc.028, advc.089 (same as in CvUnitAI::AI_attackOdds)

	if (pDefender == NULL)
		return 100;

	/*	<advc.114b>: We shouldn't adjust the odds based on an optimistic estimate
		(increased by AttackOddsChange). It leads to Warriors attacking Tanks
		because the optimistic odds are considerably above zero and the
		difference in production cost is great. I'm subtracting the AttackOddsChange
		temporarily; adding them back in after the adjustments are done.
		(A more elaborate fix would avoid adding them in the first place.) */
	int const iAttackOddsChange = GET_PLAYER(getOwner()).AI_getAttackOddsChange();
	iOdds -= iAttackOddsChange;
	/*	Require a stack of at least 3 if actual odds are below 1%. Should
		matter mostly for Barbarians, hence only this primitive condition
		(not checking if the other units could actually attack etc.). */
	if(iOdds == 0 && getNumUnits() < 3)
		return 0;
	// </advc.114b>
	// advc: The bulk of the computation is still in CvUnitAI
	int iAdjustedOdds = pAttacker->AI_opportuneOdds(iOdds, *pDefender);

	/*  one more thing... unfortunately, the sea AI isn't evolved enough
		to do without something as painful as this... */
	if (getDomainType() == DOMAIN_SEA && !hasCargo())
	{
		// I'm sorry about this. I really am. I'll try to make it better one day...
		int iDefenders = pAttacker->AI_countEnemyDefenders(*pPlot);
		iAdjustedOdds *= 2 + getNumUnits();
		iAdjustedOdds /= 3 + std::min(iDefenders/2, getNumUnits());
	}

	iAdjustedOdds += iAttackOddsChange; // advc.114b
	return range(iAdjustedOdds, 1, 99);
}


CvUnitAI* CvSelectionGroupAI::AI_getBestGroupAttacker(const CvPlot* pPlot,
	bool bPotentialEnemy, int& iUnitOdds, bool bForce, bool bNoBlitz,
	// <advc.048>
	bool bSacrifice, bool bMaxSurvival) const
{
	int const iOddsThresh = 68; // Should this be lower if bHuman?
	FAssert(!bMaxSurvival || !bSacrifice); // </advc.048>
	PROFILE_FUNC();

	int iBestValue = 0;
	int iBestOdds = 0;
	CvUnitAI* pBestUnit = NULL;
	CLLNode<IDInfo> const* pUnitNode = headUnitNode();
	bool bHuman = (pUnitNode == NULL ? true :
			GET_PLAYER(::getUnit(pUnitNode->m_data)->getOwner()).isHuman());
	FAssert(!bMaxSurvival || bHuman); // advc.048
	while (pUnitNode != NULL)
	{
		CvUnitAI& kLoopUnit = *::AI_getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (kLoopUnit.isDead())
			continue; // advc

		bool bCanAttack = false;
		if (kLoopUnit.getDomainType() == DOMAIN_AIR)
			bCanAttack = kLoopUnit.canAirAttack();
		else
		{
			bCanAttack = kLoopUnit.canAttack();
			if (bCanAttack && bNoBlitz && kLoopUnit.isBlitz() && kLoopUnit.isMadeAttack())
				bCanAttack = false;
		}
		if (!bCanAttack || (!bForce && !kLoopUnit.canMove()))
			continue;

		if (!bForce && !kLoopUnit.canMoveInto(*pPlot, true, /*bDeclareWar=*/bPotentialEnemy))
			continue;

		// BETTER_BTS_AI_MOD, Lead From Behind (UncutDragon), 02/21/10, jdog5000: START
		if (GC.getDefineBOOL(CvGlobals::LFB_ENABLE) &&
			GC.getDefineBOOL(CvGlobals::LFB_USECOMBATODDS) &&
			!bMaxSurvival) // advc.048
		{
			kLoopUnit.LFBgetBetterAttacker(&pBestUnit, pPlot, bPotentialEnemy, iBestOdds,
					iBestValue); // K-Mod.
		}
		else
		{
			int iOdds = kLoopUnit.AI_attackOdds(pPlot, bPotentialEnemy);
			int iValue = iOdds;
			FAssert(iValue > 0);

			if (kLoopUnit.collateralDamage() > 0 && /* advc.048: */ !bMaxSurvival)
			{
				int iPossibleTargets = std::min(
						pPlot->getNumVisibleEnemyDefenders(&kLoopUnit) - 1,
						kLoopUnit.collateralDamageMaxUnits());
				if (iPossibleTargets > 0)
				{
					iValue *= (100 + (kLoopUnit.collateralDamage() * iPossibleTargets) / 5);
					iValue /= 100;
				}
			}
			/*  if non-human, prefer the last unit that has the best value
				(so as to avoid splitting the group) */
			if (iValue > iBestValue || (!bHuman && iValue > 0 && iValue == iBestValue)
				/*  <advc.048> For human, use sacrifice value to break ties in order
					to match the choice made in the !bMaxSurvival branch above
					and the bSacrifice branch below. */
				|| (bHuman && iValue < iOddsThresh && iValue == iBestValue &&
				(pBestUnit == NULL || kLoopUnit.AI_sacrificeValue(pPlot) >
				kLoopUnit.AI_sacrificeValue(pPlot)))) // </advc.048>
			{
				iBestValue = iValue;
				iBestOdds = iOdds;
				pBestUnit = &kLoopUnit;
			}
		}
		// BETTER_BTS_AI_MOD: END
	}
	iUnitOdds = iBestOdds;
	// <advc.048> Cut from CvSelectionGroup::groupAttack
	if(bSacrifice)
	{
		if(iUnitOdds < iOddsThresh)
		{
			CvUnitAI* pBestSacrifice = AI_getBestGroupSacrifice(pPlot,
					bPotentialEnemy, bForce, /* advc.164: */ bNoBlitz);
			if(pBestSacrifice != NULL)
			{
				pBestUnit = pBestSacrifice;
				/*  I.e. caller mustn't use these odds. Don't want to compute them here
					if the caller doesn't need them. */
				iUnitOdds = -1;
			}
		}
	} // </advc.048>
	return pBestUnit;
}

CvUnitAI* CvSelectionGroupAI::AI_getBestGroupSacrifice(const CvPlot* pPlot,
		bool bPotentialEnemy, bool bForce, bool bNoBlitz) const
{
	int iBestValue = -1; // advc.048: was 0
	CvUnitAI* pBestUnit = NULL;

	CLLNode<IDInfo> const* pUnitNode = headUnitNode();
	// <advc.048> Copied from AI_getBestGroupAttacker
	bool bHuman = (pUnitNode == NULL ? true :
			GET_PLAYER(::getUnit(pUnitNode->m_data)->getOwner()).isHuman());
	// </advc.048>
	while (pUnitNode != NULL)
	{
		CvUnitAI* pLoopUnit = ::AI_getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!pLoopUnit->isDead())
		{
			bool bCanAttack = false;
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
				bCanAttack = pLoopUnit->canAirAttack();
			else
			{
				bCanAttack = pLoopUnit->canAttack();
				if (bCanAttack && bNoBlitz && pLoopUnit->isBlitz() && pLoopUnit->isMadeAttack())
					bCanAttack = false;
			}
			if (bCanAttack)
			{
				if (bForce || pLoopUnit->canMove())
				{
					if (bForce || pLoopUnit->canMoveInto(*pPlot, true))
					{
						int iValue = pLoopUnit->AI_sacrificeValue(pPlot);
						/* advc.006: > 0 not guaranteed if unit has no
						   production cost; changed to >= 0. */
						FAssert(iValue >= 0);

						// we want to pick the last unit of highest value, so pick the last unit with a good value
						//if (iValue >= iBestValue)
						// advc.048: As in AI_getBestGroupAttacker
						if (iValue > iBestValue || (!bHuman && iValue == iBestValue))
						{
							iBestValue = iValue;
							pBestUnit = pLoopUnit;
						}
					}
				}
			}
		}
	}
	return pBestUnit;
}
// DOTO-MOD - rangedattack-keldath START - Ranged Strike AI realism invictus
CvUnit* CvSelectionGroupAI::AI_getBestGroupRangeAttacker(const CvPlot* pPlot) const
{
	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	bool bIsHuman = (pUnitNode != NULL) ? GET_PLAYER(::getUnit(pUnitNode->m_data)->getOwner()).isHuman() : true;

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canRangeStrikeAtK(pLoopUnit->plot(), pPlot->getX(), pPlot->getY()))
		{
			CvUnit* pDefender = pLoopUnit->rangedStrikeTargetK(pPlot);

			FAssert(pDefender != NULL);
			FAssert(pDefender->canDefend());

			int iDamage = pLoopUnit->rangeCombatDamage(pDefender);//change fn

			int iUnitDamage = std::max(pDefender->getDamage(), std::min((pDefender->getDamage() + iDamage), pLoopUnit->airCombatLimit()));
			int iValue = iUnitDamage;

			if (pLoopUnit->collateralDamage() > 0)
			{
				int iPossibleTargets = std::min((pPlot->getNumVisibleEnemyDefenders(pLoopUnit) - 1), pLoopUnit->collateralDamageMaxUnits());

				if (iPossibleTargets > 0)
				{
					iValue *= (100 + ((pLoopUnit->collateralDamage() * iPossibleTargets) / 5));
					iValue /= 100;
				}
			}

			// if non-human, prefer the last unit that has the best value (so as to avoid splitting the group)
			if (iValue > iBestValue || (!bIsHuman && iValue > 0 && iValue == iBestValue))
			{
				iBestValue = iValue;
				pBestUnit = pLoopUnit;
			}
		}
	}

	return pBestUnit;
}
// MOD - END - Ranged Strike AI
// Returns ratio of strengths of stacks times 100
// (so 100 is even ratio, numbers over 100 mean this group is more powerful than the stack on a plot)
int CvSelectionGroupAI::AI_compareStacks(const CvPlot* pPlot, bool bCheckCanAttack) const
{
	FAssert(pPlot != NULL);

	int	compareRatio;
	DomainTypes eDomainType = getDomainType();

	// if not aircraft, then choose based on the plot, not the head unit (mainly for transport carried units)
	if (eDomainType != DOMAIN_AIR)
	{
		if (pPlot->isWater())
			eDomainType = DOMAIN_SEA;
		else eDomainType = DOMAIN_LAND;
	}

	compareRatio = AI_sumStrength(pPlot, eDomainType, bCheckCanAttack);
	compareRatio *= 100;

	PlayerTypes eOwner = getOwner();
	if (eOwner == NO_PLAYER)
		eOwner = getHeadOwner();

	FAssert(eOwner != NO_PLAYER);

	// K-Mod. Note. This function currently does not support bPotentialEnemy == false.
	//FAssert(bPotentialEnemy);
	int defenderSum = pPlot->isVisible(getHeadTeam()) ?
			GET_PLAYER(eOwner).AI_localDefenceStrength(pPlot, NO_TEAM, eDomainType, 0) :
			GET_TEAM(getHeadTeam()).AI_getStrengthMemory(pPlot);
	// K-Mod end
	compareRatio /= std::max(1, defenderSum);

	// K-Mod. If there are more defenders than we have attacks, but yet the ratio is still greater than 100,
	// then inflate the ratio futher to account for the fact that we are going to do significantly more damage to them than they to us.
	// The purpose of this is to give the AI extra encouragement to attack when its units are better than the defender's units.
	/* if (compareRatio > 100) {
		FAssert(getHeadUnit() && getNumUnits() > 0);
		int iDefenders = pPlot->getNumVisibleEnemyDefenders(getHeadUnit());
		if (iDefenders > getNumUnits())
			compareRatio += (compareRatio - 100) * (iDefenders - getNumUnits()) / getNumUnits();
	} */ // (currently disabled)
	// K-Mod end

	return compareRatio;
}

/*  K-Mod. I've removed bCheckMove, and changed bCheckCanAttack to include checks
	for moves, and for hasAlreadyAttacked / blitz */ // advc: style changes
/*  advc.159: No longer simply a sum of combat strength values; see the comment
	above CvPlayerAI::AI_localDefenceStrength. */
int CvSelectionGroupAI::AI_sumStrength(const CvPlot* pAttackedPlot,
	DomainTypes eDomainType, bool bCheckCanAttack) const
{
	FAssert(eDomainType != DOMAIN_AIR && eDomainType != DOMAIN_IMMOBILE); // advc: Air combat strength isn't counted
	// <K-Mod>
	bool const bDefenders = (pAttackedPlot ?
			pAttackedPlot->isVisibleEnemyUnit(getHeadOwner()) : false);
	bool const bCountCollateral = (pAttackedPlot && pAttackedPlot != plot()); // </K-Mod>
	int const iBaseCollateral = (bCountCollateral ?
			estimateCollateralWeight(pAttackedPlot, getTeam()) : 0);
	int	iSum = 0;
	for (CLLNode<IDInfo> const* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
	{
		CvUnitAI const& kLoopUnit = *::AI_getUnit(pUnitNode->m_data);
		if (kLoopUnit.isDead() ||
			// advc.opt: (If we want to count air units, then this'll have to be removed.)
			!kLoopUnit.canFight()) 
		{
			continue;
		}
		if (eDomainType != NO_DOMAIN && kLoopUnit.getDomainType() != eDomainType)
			continue; // advc: Moved up
		// K-Mod. (original checks deleted.)
		if (bCheckCanAttack)
		{
			// advc.opt: currEffectiveStr is 0 for air units anyway
			/*if (kLoopUnit.getDomainType() == DOMAIN_AIR)
			{
				if (!kLoopUnit.canAirAttack() || !kLoopUnit.canMove() ||
					(pAttackedPlot && bDefenders &&
					!kLoopUnit.canMoveInto(*pAttackedPlot, true, true)))
				{
					continue; // can't attack.
				}
			}
			else*/
			if (!kLoopUnit.canAttack() || !kLoopUnit.canMove() ||
				(pAttackedPlot && bDefenders && !kLoopUnit.canMoveInto(*pAttackedPlot, true, true)) ||
				//(!pLoopUnit->isBlitz() && pLoopUnit->isMadeAttack())
				kLoopUnit.isMadeAllAttacks()) // advc.164
			{
				continue; // can't attack.
			}
		} // K-Mod end

		// iSum += pLoopUnit->currEffectiveStr(pAttackedPlot, pLoopUnit);
		// K-Mod estimate the value of first strike, and the attack power of collateral units.
		// (cf with calculation in CvPlayerAI::AI_localAttackStrength)
		/*  <advc.159> Call AI_currEffectiveStr instead of currEffectiveStr.
			Adjustments for first strikes and collateral damage moved into
			that new function. */
		int const iUnitStr = kLoopUnit.AI_currEffectiveStr(pAttackedPlot, &kLoopUnit,
				bCountCollateral, iBaseCollateral, bCheckCanAttack);
		// </advc.159>
		iSum += iUnitStr;
		// K-Mod end
	}
	return iSum;
}

void CvSelectionGroupAI::AI_queueGroupAttack(int iX, int iY)
{
	m_bGroupAttack = true;

	m_iGroupAttackX = iX;
	m_iGroupAttackY = iY;
}


bool CvSelectionGroupAI::AI_isDeclareWar(CvPlot const& kPlot) const // advc: const; param no longer optional.
{
	FAssert(getHeadUnit() != NULL);

	if (isHuman())
		return false;
	// K-Mod
	if (AI_getMissionAIType() == MISSIONAI_EXPLORE)
		return false;
	// K-Mod end

	bool bLimitedWar = false;
	TeamTypes ePlotTeam = kPlot.getTeam();
	if (ePlotTeam != NO_TEAM)
	{
		WarPlanTypes eWarplan = GET_TEAM(getTeam()).AI_getWarPlan(
				GET_TEAM(ePlotTeam).getMasterTeam()); // advc.104j
		if (eWarplan == WARPLAN_LIMITED)
			bLimitedWar = true;
	}

	CvUnit* pHeadUnit = getHeadUnit();
	if (pHeadUnit == NULL)
		return false; // advc

	switch (pHeadUnit->AI_getUnitAIType())
	{
	case UNITAI_UNKNOWN:
	case UNITAI_ANIMAL:
	case UNITAI_SETTLE:
	case UNITAI_WORKER:
		return false;
	case UNITAI_ATTACK_CITY:
	case UNITAI_ATTACK_CITY_LEMMING:
		return true;
	case UNITAI_ATTACK:
	case UNITAI_COLLATERAL:
	case UNITAI_PILLAGE:
		return bLimitedWar;
	case UNITAI_PARADROP:
	case UNITAI_RESERVE:
	case UNITAI_COUNTER:
	case UNITAI_CITY_DEFENSE:
	case UNITAI_CITY_COUNTER:
	case UNITAI_CITY_SPECIAL:
	case UNITAI_EXPLORE:
	case UNITAI_MISSIONARY:
	case UNITAI_PROPHET:
	case UNITAI_ARTIST:
	case UNITAI_SCIENTIST:
	case UNITAI_GENERAL:
	case UNITAI_MERCHANT:
	case UNITAI_ENGINEER:
	case UNITAI_GREAT_SPY: // K-Mod
	case UNITAI_SPY:
	case UNITAI_ICBM:
	case UNITAI_WORKER_SEA:
		return false;
	case UNITAI_ATTACK_SEA:
	case UNITAI_RESERVE_SEA:
	case UNITAI_ESCORT_SEA:
		return bLimitedWar;
	case UNITAI_EXPLORE_SEA:
		return false;
	case UNITAI_ASSAULT_SEA:
		return hasCargo();
	case UNITAI_SETTLER_SEA:
	case UNITAI_MISSIONARY_SEA:
	case UNITAI_SPY_SEA:
	case UNITAI_CARRIER_SEA:
	case UNITAI_MISSILE_CARRIER_SEA:
	case UNITAI_PIRATE_SEA:
	case UNITAI_ATTACK_AIR:
	case UNITAI_DEFENSE_AIR:
	case UNITAI_CARRIER_AIR:
	case UNITAI_MISSILE_AIR:
		return false;
	default:
		FAssert(false);
		return false;
	}
}


/*	advc: Moved from CvSelectionGroup b/c this checks for the
	group owner's war plans. Param renamed from bIgnoreMinors
	b/c it also causes Barbarians to be ignored. */
bool CvSelectionGroupAI::AI_isHasPathToAreaEnemyCity(bool bMajorOnly, int iFlags, int iMaxPathTurns) const
{
	PROFILE_FUNC();

	//int iPass = 0; // advc: unused
	for (PlayerIter<ALIVE> it; it.hasNext(); ++it)
	{
		if (bMajorOnly && !it->isMajorCiv())
			continue;
		if (GET_TEAM(getTeam()).AI_mayAttack(it->getTeam()) &&
			isHasPathToAreaPlayerCity(it->getID(), iFlags, iMaxPathTurns))
		{
			return true;
		}
	}
	return false;
}


CvPlot* CvSelectionGroupAI::AI_getMissionAIPlot() /* advc: */ const
{
	return GC.getMap().plotSoren(m_iMissionAIX, m_iMissionAIY);
}


bool CvSelectionGroupAI::AI_isForceSeparate() /* advc: */ const
{
	return m_bForceSeparate;
}


void CvSelectionGroupAI::AI_setMissionAI(MissionAITypes eNewMissionAI,
	CvPlot const* pNewPlot, CvUnit const* pNewUnit) // advc: 2x const
{
	//PROFILE_FUNC();

	m_eMissionAIType = eNewMissionAI;

	if (pNewPlot != NULL)
	{
		m_iMissionAIX = pNewPlot->getX();
		m_iMissionAIY = pNewPlot->getY();
	}
	else
	{
		m_iMissionAIX = INVALID_PLOT_COORD;
		m_iMissionAIY = INVALID_PLOT_COORD;
	}

	if (pNewUnit != NULL)
		m_missionAIUnit = pNewUnit->getIDInfo();
	else m_missionAIUnit.reset();
}


CvUnitAI* CvSelectionGroupAI::AI_getMissionAIUnit() /* advc: */ const
{
	return ::AI_getUnit(m_missionAIUnit);
}


bool CvSelectionGroupAI::AI_isFull()
{
	if(getNumUnits() <= 0)
		return false;

	UnitAITypes eUnitAI = getHeadUnitAIType();
	// do two passes, the first pass, we ignore units with speical cargo
	int iSpecialCargoCount = 0;
	int iCargoCount = 0;

	// first pass, count but ignore special cargo units
	for (CLLNode<IDInfo> const* pUnitNode = headUnitNode(); pUnitNode != NULL;
		pUnitNode = nextUnitNode(pUnitNode))
	{
		CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
		if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
		{
			if (pLoopUnit->cargoSpace() > 0)
				iCargoCount++;

			if (pLoopUnit->specialCargo() != NO_SPECIALUNIT)
				iSpecialCargoCount++;
			else if (!pLoopUnit->isFull())
				return false;
		}
	}

	// if every unit in the group has special cargo, then check those, otherwise, consider ourselves full
	if (iSpecialCargoCount >= iCargoCount)
	{
		for (CLLNode<IDInfo> const* pUnitNode = headUnitNode(); pUnitNode != NULL;
			pUnitNode = nextUnitNode(pUnitNode))
		{
			CvUnit const* pLoopUnit = ::getUnit(pUnitNode->m_data);
			if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				if (!pLoopUnit->isFull())
					return false;
			}
		}
	}

	return true;
}


CvUnitAI* CvSelectionGroupAI::AI_ejectBestDefender(CvPlot* pDefendPlot)
{
	CvUnitAI* pBestUnit = NULL;
	int iBestUnitValue = 0;
	CLLNode<IDInfo>* pEntityNode = headUnitNode();
	while (pEntityNode != NULL)
	{
		CvUnitAI* pLoopUnit = ::AI_getUnit(pEntityNode->m_data); // advc.003u
		pEntityNode = nextUnitNode(pEntityNode);
		//if (!pLoopUnit->noDefensiveBonus())
		// commented out by K-Mod. The noDefBonus thing is already taken into account.
		{	/*  advc.159: Call AI_currEffectiveStr instead of currEffectiveStr
				And reduce the precision multiplier from 100 to 10. */
			int iValue = pLoopUnit->AI_currEffectiveStr(pDefendPlot) * 10;

			if (pDefendPlot->isCity(true, getTeam()))
			{
				iValue *= 100 + pLoopUnit->cityDefenseModifier();
				iValue /= 100;
			}

			iValue *= 100;
			//iValue /= (100 + pLoopUnit->cityAttackModifier() + pLoopUnit->getExtraCityAttackPercent());
			// advc.mnai: (Note that cityAttackModifier includes ExtraCityAttackPercent)
			iValue /= 100 + std::max(-50, 2 * pLoopUnit->cityAttackModifier());

			iValue /= 2 + (pLoopUnit->getLevel() *
					// advc.mnai:
					(pLoopUnit->AI_getUnitAIType() == UNITAI_ATTACK_CITY ? 2 : 1));
			if (iValue > iBestUnitValue)
			{
				iBestUnitValue = iValue;
				pBestUnit = pLoopUnit;
			}
		}
	}

	if (NULL != pBestUnit && getNumUnits() > 1)
		pBestUnit->joinGroup(NULL);

	return pBestUnit;
}

// <advc.003u> Based on CvSelectionGroup::getHeadUnit
CvUnitAI* CvSelectionGroupAI::AI_getHeadUnit() const
{
	CLLNode<IDInfo>* pNode = headUnitNode();
	return (pNode != NULL ? ::AI_getUnit(pNode->m_data) : NULL);
} // </advc.003u>


void CvSelectionGroupAI::read(FDataStreamBase* pStream)
{
	CvSelectionGroup::read(pStream);

	uint uiFlag=0;
	pStream->Read(&uiFlag);

	pStream->Read(&m_iMissionAIX);
	pStream->Read(&m_iMissionAIY);

	pStream->Read(&m_bForceSeparate);

	pStream->Read((int*)&m_eMissionAIType);

	pStream->Read((int*)&m_missionAIUnit.eOwner);
	m_missionAIUnit.validateOwner(); // advc.opt
	pStream->Read(&m_missionAIUnit.iID);

	pStream->Read(&m_bGroupAttack);
	pStream->Read(&m_iGroupAttackX);
	pStream->Read(&m_iGroupAttackY);
}


void CvSelectionGroupAI::write(FDataStreamBase* pStream)
{
	CvSelectionGroup::write(pStream);

	uint uiFlag=0;
	pStream->Write(uiFlag);
	REPRO_TEST_BEGIN_WRITE(CvString::format("SelGroupAI(%d,%d,%d)", getID(), getX(), getY()));
	pStream->Write(m_iMissionAIX);
	pStream->Write(m_iMissionAIY);

	pStream->Write(m_bForceSeparate);

	pStream->Write(m_eMissionAIType);

	pStream->Write(m_missionAIUnit.eOwner);
	pStream->Write(m_missionAIUnit.iID);

	pStream->Write(m_bGroupAttack);
	pStream->Write(m_iGroupAttackX);
	pStream->Write(m_iGroupAttackY);
	REPRO_TEST_END_WRITE();
}
