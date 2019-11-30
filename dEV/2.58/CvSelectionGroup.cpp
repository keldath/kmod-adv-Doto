// selectionGroup.cpp

#include "CvGameCoreDLL.h"
#include "CvSelectionGroup.h"
#include "CvGamePlay.h"
#include "CvMap.h"
#include "BBAILog.h"
#include "FAStarNode.h"
#include "CvInfos.h"
#include "CvEventReporter.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyArgsList.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEngineIFaceBase.h" // advc.102
#include "CvDLLFAStarIFaceBase.h"

KmodPathFinder CvSelectionGroup::path_finder; // K-Mod


CvSelectionGroup::CvSelectionGroup()
{
	m = new Data(); // advc.003k
	reset(0, NO_PLAYER, true);
}


CvSelectionGroup::~CvSelectionGroup()
{
	uninit();
	SAFE_DELETE(m); // advc.003k
}


void CvSelectionGroup::init(int iID, PlayerTypes eOwner)
{
	//--------------------------------
	// Init saved data
	reset(iID, eOwner);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
	AI_init();
}


void CvSelectionGroup::uninit()
{
	m_units.clear();
	m->knownEnemies.clear(); // advc.004l
	m_missionQueue.clear();
}


// Initializes data members that are serialized.
void CvSelectionGroup::reset(int iID, PlayerTypes eOwner, bool bConstructorCall)
{
	uninit(); // Uninit class

	m_iID = iID;
	m_iMissionTimer = 0;

	m_bForceUpdate = false;

	m_eOwner = eOwner;

	m_eActivityType = ACTIVITY_AWAKE;
	m->eAutomateType = NO_AUTOMATE;
	m->bInitiallyVisible = true; // advc.102
	m_bIsBusyCache = false;

	if (!bConstructorCall)
		AI_reset();
}


void CvSelectionGroup::kill()
{
	FAssert(getOwner() != NO_PLAYER);
	FAssertMsg(getID() != FFreeList::INVALID_INDEX, "getID() is not expected to be equal with FFreeList::INVALID_INDEX");
	FAssertMsg(getNumUnits() == 0, "The number of units is expected to be 0");

	GET_PLAYER(getOwner()).removeGroupCycle(getID());
	GET_PLAYER(getOwner()).deleteSelectionGroup(getID());
}

bool CvSelectionGroup::sentryAlert(
		bool bUpdateKnownEnemies) // advc.004l
{
	CvUnit* pHeadUnit = NULL;
	int iMaxRange = 0;
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		int iRange = pLoopUnit->visibilityRange() + 1;

		if (iRange > iMaxRange)
		{
			iMaxRange = iRange;
			pHeadUnit = pLoopUnit;
		}
	}

	if(pHeadUnit == NULL)
		return false; // advc.003
	bool r = false; // advc.004l
	for (int iX = -iMaxRange; iX <= iMaxRange; ++iX)
	{
		for (int iY = -iMaxRange; iY <= iMaxRange; ++iY)
		{
			CvPlot* pPlot = ::plotXY(pHeadUnit->getX(), pHeadUnit->getY(), iX, iY);
			if(NULL == pPlot)
				continue; // advc.003
			if(pHeadUnit->plot()->canSeePlot(pPlot, pHeadUnit->getTeam(), iMaxRange - 1, NO_DIRECTION))
			{	// <advc.004l>
				CLLNode<IDInfo>* pNode = pPlot->headUnitNode();
				while(pNode != NULL) {
					CvUnit* pLoopUnit = ::getUnit(pNode->m_data);
					if(pPlot->isVisibleEnemyUnit(pHeadUnit, pLoopUnit)) {
						if(bUpdateKnownEnemies) {
							r = true;
							m->knownEnemies.insertAtEnd(pNode->m_data);
						}
						else {
							bool bKnown = false;
							// Linear search :(
							for(CLLNode<IDInfo>* pKnownNode = m->knownEnemies.head();
									pKnownNode != NULL;
									pKnownNode = m->knownEnemies.next(pKnownNode)) {
								if(pKnownNode->m_data.eOwner == pNode->m_data.eOwner &&
										pKnownNode->m_data.iID == pNode->m_data.iID) {
									bKnown = true;
									break;
								}
							}
							if(!bKnown)
								return true;
						}
					}
					pNode = pPlot->nextUnitNode(pNode);
				} // </advc.004l>
			}
		}
	}

	return r; // advc.004l
}

// Note: this function has had some editting and restructuring for K-Mod. There are some unmarked changes.
void CvSelectionGroup::doTurn()
{
	PROFILE_FUNC();

	FAssert(getOwner() != NO_PLAYER);
	// <advc.003>
	if(getNumUnits() <= 0) {
		doDelayedDeath();
		return;
	} // </advc.003>

	bool bCouldAllMove = canAllMove(); // K-Mod
	CvUnit* pHeadUnit = getHeadUnit(); // advc.003
	// K-Mod. Wake spies when they reach max fortify turns in foreign territory. I'm only checking the head unit.
	// Note: We only want to wake once. So this needs to be done before the fortify counter is increased.
	if (isHuman() && getActivityType() == ACTIVITY_SLEEP)
	{
		if (pHeadUnit->isSpy() && pHeadUnit->plot()->getTeam() != getTeam())
		{
			if (pHeadUnit->getFortifyTurns() == GC.getDefineINT("MAX_FORTIFY_TURNS")-1)
			{
				setActivityType(ACTIVITY_AWAKE); // time to wake up!
			}
		}
	}
	// K-Mod end

	// do unit's turns (checking for damage)
	bool bHurt = false;
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			pLoopUnit->doTurn();

			if (pLoopUnit->isHurt())
			{
				bHurt = true;
			}
		}
	}

	ActivityTypes eActivityType = getActivityType();

	// wake unit if skipped last turn
	//		or healing and automated or no longer hurt (automated healing is one turn at a time)
	//		or on sentry and there is danger
	// <advc.004l> Also wake the unit if healing outside of a city and danger
	bool bSentryAlert = // Just for performance:
			((eActivityType == ACTIVITY_HEAL || eActivityType == ACTIVITY_SENTRY) &&
			isHuman() && sentryAlert()); // </advc.004l>
	if (eActivityType == ACTIVITY_HOLD ||
		(eActivityType == ACTIVITY_HEAL && (AI_isControlled() || !bHurt
		|| (bSentryAlert && pHeadUnit->canSentryHeal(plot())) // advc.004l
		)) ||
		(eActivityType == ACTIVITY_SENTRY && bSentryAlert))
	{
		setActivityType(ACTIVITY_AWAKE);
	}

	if (AI_isControlled())
	{
		if (getActivityType() != ACTIVITY_MISSION || (!canFight() && GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 2)))
		{
			setForceUpdate(true);
			// K-Mod. (This stuff use to be part force update's job. Now it isn't.)
			clearMissionQueue();
			AI_cancelGroupAttack();
			// K-Mod end
		}
	}
	else
	{
		setForceUpdate(false); // K-Mod. (this should do nothing, unless we're coming out of autoplay or something like that.)

		if (getActivityType() == ACTIVITY_MISSION)
		{
			bool bNonSpy = false;
			for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				if (!pLoopUnit->isSpy())
				{
					bNonSpy = true;
					break;
				}
			}

			// K-Mod
			bool bBrave = headMissionQueueNode() && (headMissionQueueNode()->m_data.iFlags & MOVE_IGNORE_DANGER);
			// Originally I used "MOVE_IGNORE_DANGER" to actually skip the danger tests complete, but I've found that
			// sometimes produces unintuitive results in some situations. So now 'ignore danger' only ignores range==2.

			//if (bNonSpy && GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), 2) > 0)
			if (bNonSpy && GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), bBrave ? 1 : 2, true, false))
				clearMissionQueue();
			// K-Mod end
		}
	}

	if (isHuman())
	{
		if (GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS)
				&& GET_TEAM(getTeam()).hasMetHuman()) // K-Mod
		{
			int iBestWaitTurns = 0;

			CLLNode<IDInfo>* pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				int iWaitTurns = (GC.getDefineINT("MIN_TIMER_UNIT_DOUBLE_MOVES") - (GC.getGame().getTurnSlice() - pLoopUnit->getLastMoveTurn()));

				if (iWaitTurns > iBestWaitTurns)
				{
					iBestWaitTurns = iWaitTurns;
				}
			}

			setMissionTimer(std::max(iBestWaitTurns, getMissionTimer()));

			if (iBestWaitTurns > 0)
			{
				// Cycle selection if the current group is selected
				CvUnit* pSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
				if (pSelectedUnit && pSelectedUnit->getGroup() == this)
				{
					gDLL->getInterfaceIFace()->selectGroup(pSelectedUnit, false, false, false);
				}
			}
		}
	}
	// K-Mod
	if (!bCouldAllMove && isCycleGroup())
		GET_PLAYER(getOwner()).updateGroupCycle(this);
	// K-Mod end

	doDelayedDeath();
}

// <advc.004l>
void CvSelectionGroup::doTurnPost() {

	/*  In particular: If an enemy unit moves out of range and returns, it's
		no longer a known enemy. */
	m->knownEnemies.clear();
	ActivityTypes eActivity = getActivityType();
	if(eActivity == ACTIVITY_HEAL || eActivity == ACTIVITY_SENTRY)
		sentryAlert(true);
} // </advc.004l>


bool CvSelectionGroup::showMoves(/* advc.102: */ CvPlot const& kFromPlot) const
{
	if (GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS) ||
			GC.getGame().isSimultaneousTeamTurns())
		return false;

	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if(!kLoopPlayer.isAlive() || !kLoopPlayer.isHuman())
			continue; // advc.003
		CvUnit* pHeadUnit = getHeadUnit();
		if(pHeadUnit == NULL)
			continue;
		if (pHeadUnit->isEnemy(kLoopPlayer.getTeam()))
		{
			if (kLoopPlayer.isOption(PLAYEROPTION_SHOW_ENEMY_MOVES))
				return true;
			else continue;
		}
		if(!kLoopPlayer.isOption(PLAYEROPTION_SHOW_FRIENDLY_MOVES))
			continue;
		// <advc.102> Hide uninteresting friendly moves
		PlayerTypes eGroupOwner = m_eOwner;
		TeamTypes eObs = kLoopPlayer.getTeam();
		CvPlot const& kToPlot = *plot();
		PlayerTypes eFromOwner = kFromPlot.getOwner();
		PlayerTypes eToOwner = kToPlot.getOwner();
		bool bAwayFromHome = (eGroupOwner != eToOwner || eGroupOwner != eFromOwner);
		bool bInSpectatorsBorders = ((eFromOwner != NO_PLAYER &&
				eObs == TEAMID(eFromOwner)) || (eToOwner != NO_PLAYER &&
				eObs == TEAMID(eToOwner)));
		bool bShowWorkers = GC.getDefineINT("SHOW_FRIENDLY_WORKER_MOVES"),
			 bShowShips = GC.getDefineINT("SHOW_FRIENDLY_SEA_MOVES"),
			// Also refers to Executives; those have the same Unit AI.
			 bShowMissionaries = GC.getDefineINT("SHOW_FRIENDLY_MISSIONARY_MOVES");
		bool bEnteringOrLeaving = (plot()->isVisible(eObs, false) != kFromPlot.isVisible(eObs, false));
		bool bSeaPatrol = (getDomainType() == DOMAIN_SEA &&
				AI_getMissionAIType() == MISSIONAI_PATROL);
		// Just to avoid cycling through the units
		if(bInSpectatorsBorders && (bEnteringOrLeaving || !bSeaPatrol))
			return true;
		if(bShowWorkers && bShowShips && bShowMissionaries)
			return true;
		for(CLLNode<IDInfo>* pNode = headUnitNode(); pNode != NULL; pNode = nextUnitNode(pNode)) {
			CvUnit const* pLoopUnit = ::getUnit(pNode->m_data);
			if(pLoopUnit == NULL) {
				FAssert(false); // An invalid unit id while the stack is moving would be strange
				continue;
			}
			CvUnit const& u = *pLoopUnit;
			bool bSeaUnit = u.getDomainType() == DOMAIN_SEA;
			bool bCombatant = u.getUnitCombatType() != NO_UNITCOMBAT;
			if(!bSeaUnit && bCombatant && !bAwayFromHome && plot()->getNumUnits() == 1)
				break;
			bool bWorker = (u.AI_getUnitAIType() == UNITAI_WORKER ||
					u.AI_getUnitAIType() == UNITAI_WORKER_SEA);
			bool bNonTransportShip = (bSeaUnit && !u.isHuman() &&
					(u.cargoSpace() <= 1 || bSeaPatrol));
			bool bMissionary = (u.AI_getUnitAIType() == UNITAI_MISSIONARY);
			if(!bMissionary && bAwayFromHome && (!bSeaUnit ||
					!bNonTransportShip || bShowShips || bEnteringOrLeaving))
				return true;
			if((bWorker && bShowWorkers) || (bNonTransportShip && bShowShips &&
					bEnteringOrLeaving) || (bMissionary && bShowMissionaries))
				return true;
			if(!bWorker && !bNonTransportShip && !bMissionary)
				return true;
			// </advc.102>
		}
	}
	return false;
}

// <advc.102>
void CvSelectionGroup::setInitiallyVisible(bool b) {

	m->bInitiallyVisible = b;
} // </advc.102>


void CvSelectionGroup::updateTimers()
{
	FAssert(getOwner() != NO_PLAYER);

	if (getNumUnits() > 0)
	{
		bool bCombat = false;

		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit->isCombat())
			{
				if (pLoopUnit->isAirCombat())
				{
					pLoopUnit->updateAirCombat();
				}
				else
				{
					pLoopUnit->updateCombat();
				}

				bCombat = true;
				//break; // disabled by K-Mod.
				// (I've changed groupAttack to fix a problem, and now multiple units in a single group can be queued for combat.)
				// so this break can cause an infinite loop, as the currently fighting unit might not get updated.
			}
		}

		if (!bCombat)
		{
			updateMission();
		}
	}

	doDelayedDeath();
}

// Returns true if group was killed...
bool CvSelectionGroup::doDelayedDeath()
{
	FAssert(getOwner() != NO_PLAYER);

	if (isBusy())
	{
		return false;
	}

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		pLoopUnit->doDelayedDeath();
	}

	if (getNumUnits() == 0)
	{
		kill();
		return true;
	}

	return false;
}


void CvSelectionGroup::playActionSound()
{
	// Pitboss should not be playing sounds!
#ifndef PITBOSS

	CvUnit *pHeadUnit;
	int iScriptId = -1;

	pHeadUnit = getHeadUnit();
	if (pHeadUnit)
	{
		iScriptId = pHeadUnit->getArtInfo(0, GET_PLAYER(getOwner()).getCurrentEra())->getActionSoundScriptId();
	}

	if (iScriptId == -1 && pHeadUnit)
	{
		CvCivilizationInfo *pCivInfo;
		pCivInfo = &GC.getCivilizationInfo(pHeadUnit->getCivilizationType());
		if (pCivInfo)
		{
			iScriptId = pCivInfo->getActionSoundScriptId();
		}
	}

	if (iScriptId != -1 && pHeadUnit)
	{
		CvPlot* pPlot = GC.getMap().plot(pHeadUnit->getX(),pHeadUnit->getY());
		if (pPlot != NULL)
		{
			gDLL->Do3DSound(iScriptId, pPlot->getPoint());
		}
	}

#endif // n PITBOSS
}


void CvSelectionGroup::pushMission(MissionTypes eMission, int iData1, int iData2,
		int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI,
		CvPlot* pMissionAIPlot, CvUnit* pMissionAIUnit,
		bool bModified) { // advc.011b

	PROFILE_FUNC();

	FAssert(getOwner() != NO_PLAYER);

	if (!bAppend)
	{
		if (isBusy())
		{
			return;
		}

		clearMissionQueue();
	}

	if (bManual)
	{
		setAutomateType(NO_AUTOMATE);
	}

	MissionData mission;
	mission.eMissionType = eMission;
	mission.iData1 = iData1;
	mission.iData2 = iData2;
	mission.iFlags = iFlags;
	mission.iPushTurn = GC.getGame().getGameTurn();
	mission.bModified = bModified; //advc.011b

	if (canAllMove()) // K-Mod. Do not set the AI mission type if this is just a "follow" command!
		AI_setMissionAI(eMissionAI, pMissionAIPlot, pMissionAIUnit);

	insertAtEndMissionQueue(mission, !bAppend
			|| AI_isControlled()); // K-Mod (AI commands should execute immediately)

	if (bManual)
	{
		if (getOwner() == GC.getGame().getActivePlayer())
		{
			if (isBusy() && GC.getMissionInfo(eMission).isSound())
			{
				playActionSound();
			}

			gDLL->getInterfaceIFace()->setHasMovedUnit(true);
			/*  advc.001w: Prevent help text and mouse focus from lingering after
				a command button is clicked */
			GC.getGame().setUpdateTimer(CvGame::UPDATE_MOUSE_FOCUS, 2);
		}

		CvEventReporter::getInstance().selectionGroupPushMission(this, eMission);

		doDelayedDeath();
	}
}


void CvSelectionGroup::popMission()
{
	CLLNode<MissionData>* pTailNode;

	FAssert(getOwner() != NO_PLAYER);

	pTailNode = tailMissionQueueNode();

	if (pTailNode != NULL)
	{
		deleteMissionQueueNode(pTailNode);
	}
}


bool CvSelectionGroup::autoMission() // K-Mod changed this from void to bool.
{
	FAssert(getOwner() != NO_PLAYER);

	if (getNumUnits() > 0)
	{
		if (headMissionQueueNode() != NULL)
		{
			if (!isBusy())
			{
				bool bVisibleHuman = false;
				//if (isHuman())
				if (!AI_isControlled()) // K-Mod. (otherwise the automation will just reissue commands immediately after they are cleared, resulting in an infinite loop.)
				{
					for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
					{
						CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
						if (!pLoopUnit->alwaysInvisible())
						{
							bVisibleHuman = true;
							break;
						}
					}
				}

				//if (bVisibleHuman && GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), 1) > 0)
				// K-Mod. I want to allow players to queue actions when in danger without being overruled by this clause.
				if (bVisibleHuman && headMissionQueueNode()->m_data.iPushTurn != GC.getGame().getGameTurn() && !(headMissionQueueNode()->m_data.iFlags & MOVE_IGNORE_DANGER) &&
					GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 1, true, false))
				// K-Mod end
				{
					clearMissionQueue();
				}
				else
				{
					/* original bts code
					if (getActivityType() == ACTIVITY_MISSION)
						continueMission();
					else startMission();*/
					// K-Mod
					if (getActivityType() != ACTIVITY_MISSION)
						startMission();
					else if (readyForMission())
						continueMission();
					// K-Mod end
				}
			}
		}
	}

	return doDelayedDeath();
}


void CvSelectionGroup::updateMission()
{
	FAssert(getOwner() != NO_PLAYER);

	if (getMissionTimer() > 0)
	{
		changeMissionTimer(-1);

		if (getMissionTimer() == 0)
		{
			if (getActivityType() == ACTIVITY_MISSION)
			{
				continueMission();
			}
			else
			{
				if (getOwner() == GC.getGame().getActivePlayer())
				{
					if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == NULL)
					{
						GC.getGame().cycleSelectionGroups_delayed(1, true);
					}
				}
			}
		}
	}
}


CvPlot* CvSelectionGroup::lastMissionPlot()
{
	CLLNode<MissionData>* pMissionNode = tailMissionQueueNode();
	while (pMissionNode != NULL)
	{
		switch (pMissionNode->m_data.eMissionType)
		{
		case MISSION_MOVE_TO:
		case MISSION_ROUTE_TO:
			return GC.getMap().plot(pMissionNode->m_data.iData1, pMissionNode->m_data.iData2);
			break;

		case MISSION_MOVE_TO_UNIT: {
			CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)pMissionNode->m_data.iData1).getUnit(pMissionNode->m_data.iData2);
			if (pTargetUnit != NULL)
				return pTargetUnit->plot();
			break;
		}
		case MISSION_SKIP:
		case MISSION_SLEEP:
		case MISSION_FORTIFY:
		case MISSION_PLUNDER:
		case MISSION_AIRPATROL:
		case MISSION_SEAPATROL:
		case MISSION_HEAL:
		case MISSION_SENTRY_HEAL: // advc.004l
		case MISSION_SENTRY:
		case MISSION_AIRLIFT:
		case MISSION_NUKE:
		case MISSION_RECON:
		case MISSION_PARADROP:
		case MISSION_AIRBOMB:
		case MISSION_BOMBARD:
		case MISSION_RANGE_ATTACK:
		case MISSION_PILLAGE:
		case MISSION_SABOTAGE:
		case MISSION_DESTROY:
		case MISSION_STEAL_PLANS:
		case MISSION_FOUND:
		case MISSION_SPREAD:
		case MISSION_SPREAD_CORPORATION:
		case MISSION_JOIN:
		case MISSION_CONSTRUCT:
		case MISSION_DISCOVER:
		case MISSION_HURRY:
		case MISSION_TRADE:
		case MISSION_GREAT_WORK:
		case MISSION_INFILTRATE:
		case MISSION_GOLDEN_AGE:
		case MISSION_BUILD:
		case MISSION_LEAD:
		case MISSION_ESPIONAGE:
		case MISSION_DIE_ANIMATION:
			break;

		default:
			FAssert(false);
			break;
		}

		pMissionNode = prevMissionQueueNode(pMissionNode);
	}

	return plot();
}


bool CvSelectionGroup::canStartMission(int iMission, int iData1, int iData2, CvPlot* pPlot, bool bTestVisible, bool bUseCache)
{
	if (bUseCache)
	{
		if (m_bIsBusyCache)
			return false;
	}
	else
	{
		if (isBusy())
			return false;
	}

	return canDoMission(iMission, iData1, iData2, pPlot, bTestVisible, false); // K-Mod. (original code merged into CvSelectionGroup::canDoMission
}


void CvSelectionGroup::startMission()
{
	//PROFILE_FUNC();

	FAssert(!isBusy());
	FAssert(getOwner() != NO_PLAYER);
	FAssert(headMissionQueueNode() != NULL);

	if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		if (!GET_PLAYER(getOwner()).isTurnActive())
		{
			if (getOwner() == GC.getGame().getActivePlayer())
			{
				if (IsSelected())
				{
					GC.getGame().cycleSelectionGroups_delayed(1, true);
				}
			}

			return;
		}
	}

	/* original bts code
	if (canAllMove())
		setActivityType(ACTIVITY_MISSION);
	else setActivityType(ACTIVITY_HOLD);*/
	// moved & changed by K-Mod.

	bool bDelete = false;
	bool bAction = false;
	bool bNuke = false;
	bool bNotify = false;

	if (!canStartMission(headMissionQueueNode()->m_data.eMissionType, headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2, plot()))
	{
		bDelete = true;
	}
	else
	{
		FAssertMsg(GET_PLAYER(getOwner()).isTurnActive() || GET_PLAYER(getOwner()).isHuman(), "It's expected that either the turn is active for this player or the player is human");

		// K-Mod. Moved from outside.
		if (readyForMission()) {
			setActivityType(ACTIVITY_MISSION);
			// <advc.029> (Not sure if this is the best place for this)
			if(getHeadUnit() != NULL && getDomainType() == DOMAIN_AIR) {
				MissionData data = headMissionQueueNode()->m_data;
				CvPlot* pDest = GC.getMap().plot(data.iData1, data.iData2);
				/*  Both air attack and rebase are MOVE_TO missions. Want to
					clear the recon-plot only for rebase. */
				if(data.eMissionType == MISSION_MOVE_TO && pDest != NULL &&
						pDest->isFriendlyCity(*getHeadUnit(), true))
					getHeadUnit()->setReconPlot(NULL);
			} // </advc.029>
		}
		else setActivityType(ACTIVITY_HOLD);
		// K-Mod end
		resetBoarded(); // advc.075

		// Whole group effects
		switch (headMissionQueueNode()->m_data.eMissionType)
		{
		case MISSION_MOVE_TO:
			// K-Mod. Prevent human players from accidentally attacking units that they can't see.
			if (isHuman() && !GC.getMap().plot(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2)->isVisible(getTeam(), false))
				headMissionQueueNode()->m_data.iFlags |= MOVE_NO_ATTACK;

			// also, we should allow an amphibious landing even if we are out of moves.
			if (!canAllMove())
			{
				if (groupAmphibMove(GC.getMap().plot(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2), headMissionQueueNode()->m_data.iFlags))
				{
					bDelete = true;
				}
			}
			// K-Mod end
		case MISSION_ROUTE_TO:
		case MISSION_MOVE_TO_UNIT:
			break;

		case MISSION_SKIP:
			setActivityType(ACTIVITY_HOLD);
			bDelete = true;
			break;

		case MISSION_SLEEP:
			setActivityType(ACTIVITY_SLEEP);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_FORTIFY:
			setActivityType(ACTIVITY_SLEEP);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_PLUNDER:
			setActivityType(ACTIVITY_PLUNDER);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_AIRPATROL:
			setActivityType(ACTIVITY_INTERCEPT);
			bDelete = true;
			break;

		case MISSION_SEAPATROL:
			setActivityType(ACTIVITY_PATROL);
			bDelete = true;
			break;

		case MISSION_HEAL:
		/*  advc.004l: No separate sentry heal activity. Instead, units in
			ACTIVITY_HEAL are going to apply sentry behavior whenever it makes sense.
			The sentry heal mission is only needed for the proper help text. */
		case MISSION_SENTRY_HEAL:
			setActivityType(ACTIVITY_HEAL);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_SENTRY:
			setActivityType(ACTIVITY_SENTRY);
			bNotify = true;
			bDelete = true;
			break;

		case MISSION_AIRLIFT:
		case MISSION_NUKE:
		case MISSION_RECON:
		case MISSION_PARADROP:
		case MISSION_AIRBOMB:
		case MISSION_BOMBARD:
		case MISSION_RANGE_ATTACK:
		case MISSION_SABOTAGE:
		case MISSION_DESTROY:
		case MISSION_STEAL_PLANS:
		case MISSION_FOUND:
		case MISSION_SPREAD:
		case MISSION_SPREAD_CORPORATION:
		case MISSION_JOIN:
		case MISSION_CONSTRUCT:
		case MISSION_DISCOVER:
		case MISSION_HURRY:
		case MISSION_TRADE:
		case MISSION_GREAT_WORK:
		case MISSION_INFILTRATE:
		case MISSION_GOLDEN_AGE:
			break;
		// K-Mod. Let fast units carry out the pillage action first.
		// (This is based on the idea from BBAI, which had a buggy implementation.)
		case MISSION_PILLAGE:
		{
			// Fast units pillage first
			std::vector<std::pair<int, int> > unit_list;
			CLLNode<IDInfo>* pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if (pLoopUnit->canMove() && pLoopUnit->canPillage(plot()))
				{
					int iPriority = 0;
					if (pLoopUnit->bombardRate() > 0)
						iPriority--;
					if (pLoopUnit->isMadeAttack())
						iPriority++;
					if (pLoopUnit->isHurt() && !pLoopUnit->hasMoved())
						iPriority--;

					iPriority = (3 + iPriority)*pLoopUnit->movesLeft() / 3;
					unit_list.push_back(std::make_pair(iPriority, pLoopUnit->getID()));
				}
			}
			std::sort(unit_list.begin(), unit_list.end(), std::greater<std::pair<int, int> >());

			CvPlayer& kOwner = GET_PLAYER(getOwner());
			for (size_t i = 0; i < unit_list.size(); i++)
			{
				CvUnit* pLoopUnit = kOwner.getUnit(unit_list[i].second);
				FAssert(pLoopUnit);

				if (pLoopUnit->pillage())
				{
					bAction = true;
					if (!isHuman() && canAllMove()) // AI groups might want to reconsider their action after pillaging.
						break;
				}
				/*if (pLoopUnit->isAttacking())
					break;*/ // Sea patrol intercept
				/*  <dlph.37> "If this selection group survives sea patrol battle
					then the game crashes because combat clears the mission queue [...].
					Since post-combat code clears the mission queue (and this sets
					activity to ACTIVITY_AWAKE) and also deals with unit selection
					for the active player, we can just skip rest of the function here." */
                if (!headMissionQueueNode() || pLoopUnit->isAttacking())
                    return; // </dlph.37>
			}
			break;
		}

		// K-Mod. If the worker is already in danger when the command is issued, use the MOVE_IGNORE_DANGER flag.
		case MISSION_BUILD:
			if (!AI_isControlled() && headMissionQueueNode()->m_data.iPushTurn == GC.getGame().getGameTurn() &&
				GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 2, true, false)) // cf. condition used in CvSelectionGroup::doTurn.
			{
				headMissionQueueNode()->m_data.iFlags |= MOVE_IGNORE_DANGER;
			}
			break;
		// K-Mod end
		case MISSION_LEAD:
		case MISSION_ESPIONAGE:
		case MISSION_DIE_ANIMATION:
			break;

		default:
			FAssert(false);
			break;
		}

		if (bNotify)
		{
			NotifyEntity(headMissionQueueNode()->m_data.eMissionType);
		}

		// Individual unit effects
		// K-Mod
		std::vector<CvUnit*> units_left_behind;
		bool bAbandonMoveless = false;
		switch (headMissionQueueNode()->m_data.eMissionType)
		{
		case MISSION_PARADROP:
			bAbandonMoveless = true;
		default:
			break;
		}
		// K-Mod end
		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (!pLoopUnit->canMove())
			{
				if (bAbandonMoveless)
					units_left_behind.push_back(pLoopUnit);
			}
			else
			{
				switch (headMissionQueueNode()->m_data.eMissionType)
				{
				// K-Mod
				case MISSION_SKIP:
					// If the unit has some particular purpose for its 'skip' mission, automatically unload it.
					// (eg. if a unit in a boat wants to do MISSIONAI_GUARD_CITY; we should unload it here.)
					switch (AI_getMissionAIType())
					{
					case NO_MISSIONAI:
					case MISSIONAI_LOAD_ASSAULT:
					case MISSIONAI_LOAD_SETTLER:
					case MISSIONAI_LOAD_SPECIAL:
						pUnitNode = 0; // don't auto-unload. Just do nothing.
						break;
					default:
						FAssert(AI_isControlled());
						pLoopUnit->unload(); // this checks canUnload internally
						break;
					}
					break;
				// K-Mod end
				case MISSION_MOVE_TO:
				case MISSION_ROUTE_TO:
				case MISSION_MOVE_TO_UNIT:
				case MISSION_SLEEP:
				case MISSION_FORTIFY:
				case MISSION_SEAPATROL:
				case MISSION_HEAL:
				case MISSION_SENTRY_HEAL: // advc.004l
				case MISSION_SENTRY:
				case MISSION_PILLAGE:
				case MISSION_BUILD:
					pUnitNode = 0; // K-Mod. Nothing to do, so we might as well abort the unit loop.
					break;
				// K-Mod. (this used to be a "do nothing" case.)
				case MISSION_AIRPATROL:
					if (!pLoopUnit->canAirDefend(plot())) // (We can't use 'canAirPatrol', because that checks 'isWaiting'.)
						units_left_behind.push_back(pLoopUnit);
					break;
				// K-Mod end

				case MISSION_AIRLIFT:
					if (pLoopUnit->airlift(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
					{
						bAction = true;
					}
					break;

				case MISSION_NUKE:
					if (pLoopUnit->nuke(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
					{
						bAction = true;

						if (GC.getMap().plot(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2)->isVisibleToWatchingHuman())
						{
							bNuke = true;
						}
					}
					break;

				case MISSION_RECON:
					if (pLoopUnit->recon(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
					{
						bAction = true;
					}
					break;

				case MISSION_PARADROP:
					if (pLoopUnit->paradrop(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
					{
						bAction = true;
					}
					// K-Mod
					else
						units_left_behind.push_back(pLoopUnit);
					// K-Mod end
					break;

				case MISSION_AIRBOMB:
					if (pLoopUnit->airBomb(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
					{
						bAction = true;
					}
					break;

				case MISSION_BOMBARD:
					if (pLoopUnit->bombard())
					{
						bAction = true;
					}
					break;

				case MISSION_RANGE_ATTACK:
					if (pLoopUnit->rangeStrike(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
					{
						bAction = true;
					}
					break;

				case MISSION_PLUNDER:
					if (pLoopUnit->plunder())
					{
						bAction = true;
					}
					break;

				case MISSION_SABOTAGE:
					if (pLoopUnit->sabotage())
					{
						bAction = true;
					}
					break;

				case MISSION_DESTROY:
					if (pLoopUnit->destroy())
					{
						bAction = true;
					}
					break;

				case MISSION_STEAL_PLANS:
					if (pLoopUnit->stealPlans())
					{
						bAction = true;
					}
					break;

				case MISSION_FOUND:
					if (pLoopUnit->found())
					{
						bAction = true;
					}
					break;

				case MISSION_SPREAD:
					if (pLoopUnit->spread((ReligionTypes)(headMissionQueueNode()->m_data.iData1)))
					{
						bAction = true;
					}
					break;

				case MISSION_SPREAD_CORPORATION:
					if (pLoopUnit->spreadCorporation((CorporationTypes)(headMissionQueueNode()->m_data.iData1)))
					{
						bAction = true;
					}
					break;

				case MISSION_JOIN:
					if (pLoopUnit->join((SpecialistTypes)(headMissionQueueNode()->m_data.iData1)))
					{
						bAction = true;
					}
					break;

				case MISSION_CONSTRUCT:
					if (pLoopUnit->construct((BuildingTypes)(headMissionQueueNode()->m_data.iData1)))
					{
						bAction = true;
					}
					break;

				case MISSION_DISCOVER:
					if (pLoopUnit->discover())
					{
						bAction = true;
					}
					break;

				case MISSION_HURRY:
					if (pLoopUnit->hurry())
					{
						bAction = true;
					}
					break;

				case MISSION_TRADE:
					if (pLoopUnit->trade())
					{
						bAction = true;
					}
					break;

				case MISSION_GREAT_WORK:
					if (pLoopUnit->greatWork())
					{
						bAction = true;
					}
					break;

				case MISSION_INFILTRATE:
					if (pLoopUnit->infiltrate())
					{
						bAction = true;
					}
					break;

				case MISSION_GOLDEN_AGE:
					//just play animation, not golden age - JW
					if (headMissionQueueNode()->m_data.iData1 != -1)
					{
						CvMissionDefinition kMission;
						kMission.setMissionTime(GC.getMissionInfo(MISSION_GOLDEN_AGE).getTime() * gDLL->getSecsPerTurn());
						kMission.setUnit(BATTLE_UNIT_ATTACKER, pLoopUnit);
						kMission.setUnit(BATTLE_UNIT_DEFENDER, NULL);
						kMission.setPlot(pLoopUnit->plot());
						kMission.setMissionType(MISSION_GOLDEN_AGE);
						gDLL->getEntityIFace()->AddMission(&kMission);
						pLoopUnit->NotifyEntity(MISSION_GOLDEN_AGE);
						bAction = true;
					}
					else
					{
						if (pLoopUnit->goldenAge())
						{
							bAction = true;
						}
					}
					break;

				case MISSION_LEAD:
					if (pLoopUnit->lead(headMissionQueueNode()->m_data.iData1))
					{
						bAction = true;
					}
					break;

				case MISSION_ESPIONAGE:
					if (pLoopUnit->espionage((EspionageMissionTypes)headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2))
					{
						bAction = true;
					}
					pUnitNode = NULL; // allow one unit at a time to do espionage
					break;

				case MISSION_DIE_ANIMATION:
					bAction = true;
					break;

				default:
					FAssert(false);
					break;
				}

				if (getNumUnits() == 0)
				{
					break;
				}

				if (headMissionQueueNode() == NULL)
				{
					break;
				}
			}
		}
		// K-Mod
		if (!units_left_behind.empty())
		{
			FAssert(isHuman()); // This isn't a problem. I just don't want the AI to choose missions which cause the group to separate.
			FAssert((int)units_left_behind.size() < getNumUnits()); // we should never leave _everyone_ behind!
			units_left_behind[0]->joinGroup(NULL, true);
			CvSelectionGroup* pNewGroup = units_left_behind[0]->getGroup();
			for (size_t i = 1; i < units_left_behind.size(); i++)
			{
				units_left_behind[i]->joinGroup(pNewGroup, true);
			}
		}
		// K-Mod end
	} // end if (can start mission)

	if (getNumUnits() > 0 && headMissionQueueNode() != NULL)
	{
		if (bAction)
		{
			if (isHuman())
			{
				if (plot()->isVisibleToWatchingHuman())
				{
					updateMissionTimer();
				}
			}
		}

		if (bNuke)
		{
			setMissionTimer(GC.getMissionInfo(MISSION_NUKE).getTime());
		}

		if (!isBusy())
		{
			if (bDelete)
			{
				deleteMissionQueueNode(headMissionQueueNode());
				// K-Mod
				if (headMissionQueueNode())
					activateHeadMission();
				// K-Mod end

				if (getOwner() == GC.getGame().getActivePlayer() && IsSelected())
				{
					GC.getGame().cycleSelectionGroups_delayed(GET_PLAYER(getOwner()).isOption(PLAYEROPTION_QUICK_MOVES) ? 1 : 2, true, readyToSelect(true));
				}
			}
			else if (getActivityType() == ACTIVITY_MISSION)
			{
				continueMission();
			}
			// K-Mod
			else if (getOwner() == GC.getGame().getActivePlayer() && IsSelected() && !canAnyMove())
			{
				GC.getGame().cycleSelectionGroups_delayed(GET_PLAYER(getOwner()).isOption(PLAYEROPTION_QUICK_MOVES) ? 1 : 2, true);
			}
			// K-Mod end
		}
	}
}

// K-Mod. CvSelectionGroup::continueMission used to be a recursive function.
// I've moved the bulk of the function into a new function, and turned continueMission into just a simple loop to remove the recursion.
void CvSelectionGroup::continueMission()
{
	int iSteps = 0;
	while (continueMission_bulk(iSteps))
	{
		iSteps++;
	}
}

// return true if we are ready to take another step
bool CvSelectionGroup::continueMission_bulk(int iSteps)  // advc.003: style changes
{
	FAssert(!isBusy());
	FAssert(getOwner() != NO_PLAYER);
	FAssert(getActivityType() == ACTIVITY_MISSION);

	CLLNode<MissionData>* pHeadMission = headMissionQueueNode();
	if (pHeadMission == NULL)
	{
		FAssert(pHeadMission != NULL);
		// just in case...
		setActivityType(ACTIVITY_AWAKE);
		return false;
	}
	MissionData missionData = pHeadMission->m_data;
	CvGame& g = GC.getGame();
	CvPlot* pFromPlot = plot(); // advc.102
	bool bDone = false;
	bool bAction = false;

	if (!(missionData.iFlags & MOVE_NO_ATTACK) && // K-Mod
		(missionData.iPushTurn == g.getGameTurn() ||
		missionData.iFlags & MOVE_THROUGH_ENEMY))
	{
		if (missionData.eMissionType == MISSION_MOVE_TO)
		{
			bool bFailedAlreadyFighting;
			if (groupAttack(missionData.iData1, missionData.iData2,
					missionData.iFlags, bFailedAlreadyFighting,
					/* advc.048: */ missionData.bModified))
				bDone = true;
		}
		// K-Mod. We need to do a similar check for MISSION_MOVE_TO_UNIT,
		// because with the MOVE_ATTACK_STACK flag, MOVE_TO_UNIT might actually want to attack something!
		// Note: the "else" is not just for efficiency. The code above may have actually killed "this" unit!
		else if (missionData.eMissionType == MISSION_MOVE_TO_UNIT &&
				(missionData.iFlags & MOVE_ATTACK_STACK))
		{	bool bDummy;
			CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)missionData.iData1).getUnit(missionData.iData2);
			if (pTargetUnit && groupAttack(pTargetUnit->getX(), pTargetUnit->getY(),
					missionData.iFlags, bDummy, /* advc.048: */ false))
				bDone = true;
		} // K-Mod end
	}
	// extra crash protection, should never happen (but a previous bug in groupAttack was causing a NULL here)
	// while that bug is fixed, no reason to not be a little more careful
	// K-Mod note: Actually, the mission queue is always cleared after combat. So this is required regardless of any bugs.
	pHeadMission = headMissionQueueNode();
	if (pHeadMission == NULL)
	{
		setActivityType(ACTIVITY_AWAKE);
		// K-Mod. Since I removed the cycle trigger from deactivateHeadMission, we need it here.
		if (getOwner() == g.getActivePlayer() && IsSelected())
			g.cycleSelectionGroups_delayed(1, true, canAnyMove());
		return false;
	}
	missionData = pHeadMission->m_data;

	// K-Mod. 'direct attack' should be used for attack commands only. (But in simultaneous turns mode, the defenders might have already left.)
	FAssert(bDone || !(missionData.iFlags & MOVE_DIRECT_ATTACK) || g.isMPOption(MPOPTION_SIMULTANEOUS_TURNS));

	if (!bDone && getNumUnits() > 0 && /* K-Mod: */ readyForMission()) //canAllMove()
	{
		switch (missionData.eMissionType)
		{
		case MISSION_MOVE_TO:
			if (getDomainType() == DOMAIN_AIR)
			{
				groupPathTo(missionData.iData1, missionData.iData2, missionData.iFlags);
				bDone = true;
			}
			else if (groupPathTo(missionData.iData1, missionData.iData2, missionData.iFlags))
			{
				bAction = true;
				/*  advc.003: Not sure if groupPathTo can pop the head mission;
					safer to update pHeadMission. */
				pHeadMission = headMissionQueueNode();
				if (getNumUnits() > 0 && !canAllMove() && pHeadMission != NULL)
				{
					missionData = pHeadMission->m_data;
					if (groupAmphibMove(GC.getMap().plot(
							missionData.iData1, missionData.iData2), missionData.iFlags))
					{
						bAction = false;
						bDone = true;
					}
				}
			}
			else bDone = true;
			break;

		case MISSION_ROUTE_TO:
			if (groupRoadTo(missionData.iData1, missionData.iData2, missionData.iFlags))
				bAction = true;
			else bDone = true;
			break;

		case MISSION_MOVE_TO_UNIT: {
			if (getHeadUnitAI() == UNITAI_CITY_DEFENSE && plot()->isCity() &&
					plot()->getTeam() == getTeam())
			{
				if (plot()->getBestDefender(getOwner())->getGroup() == this)
				{
					bAction = false;
					bDone = true;
					break;
				}
			}
			CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)missionData.iData1).getUnit(missionData.iData2);
			if (pTargetUnit == NULL)
			{
				bDone = true;
				break;
			}
			if (AI_getMissionAIType() != MISSIONAI_SHADOW && AI_getMissionAIType() != MISSIONAI_GROUP)
			{
				if (!plot()->isOwned() || plot()->getOwner() == getOwner())
				{
					CvPlot* pMissionPlot = pTargetUnit->getGroup()->AI_getMissionAIPlot();
					if (pMissionPlot != NULL && NO_TEAM != pMissionPlot->getTeam())
					{
						if (pMissionPlot->isOwned() && pTargetUnit->isPotentialEnemy(pMissionPlot->getTeam(), pMissionPlot))
						{
							bAction = false;
							bDone = true;
							break;
						}
					}
				}
			}
			if (groupPathTo(pTargetUnit->getX(), pTargetUnit->getY(),
					missionData.iFlags))
				bAction = true;
			else bDone = true;
			break;
		}

		case MISSION_SKIP:
		case MISSION_SLEEP:
		case MISSION_FORTIFY:
		case MISSION_PLUNDER:
		case MISSION_AIRPATROL:
		case MISSION_SEAPATROL:
		case MISSION_HEAL:
		case MISSION_SENTRY_HEAL: // advc.004l
		case MISSION_SENTRY:
			FAssert(false);
			break;

		case MISSION_AIRLIFT:
		case MISSION_NUKE:
		case MISSION_RECON:
		case MISSION_PARADROP:
		case MISSION_AIRBOMB:
		case MISSION_BOMBARD:
		case MISSION_RANGE_ATTACK:
		case MISSION_PILLAGE:
		case MISSION_SABOTAGE:
		case MISSION_DESTROY:
		case MISSION_STEAL_PLANS:
		case MISSION_FOUND:
		case MISSION_SPREAD:
		case MISSION_SPREAD_CORPORATION:
		case MISSION_JOIN:
		case MISSION_CONSTRUCT:
		case MISSION_DISCOVER:
		case MISSION_HURRY:
		case MISSION_TRADE:
		case MISSION_GREAT_WORK:
		case MISSION_INFILTRATE:
		case MISSION_GOLDEN_AGE:
		case MISSION_LEAD:
		case MISSION_ESPIONAGE:
		case MISSION_DIE_ANIMATION:
			break;

		case MISSION_BUILD:
			if(!groupBuild((BuildTypes)missionData.iData1,
					!missionData.bModified)) // advc.011b
				bDone = true;
			break;

		default:
			FAssert(false);
			break;
		}
	}

	pHeadMission = headMissionQueueNode();
	if(pHeadMission == NULL || getNumUnits() <= 0)
		return false;
	missionData = pHeadMission->m_data;

	if (!bDone)
	{
		switch (missionData.eMissionType)
		{
		case MISSION_MOVE_TO:
			missionData.iFlags |= MOVE_HAS_STEPPED; // K-Mod
			if (at(missionData.iData1, missionData.iData2))
			{
				bDone = true;
				handleBoarded(); // advc.075
			}
			break;

		case MISSION_ROUTE_TO:
			if (at(missionData.iData1, missionData.iData2))
			{
				if (getBestBuildRoute(plot()) == NO_ROUTE)
					bDone = true;
			}
			break;

		case MISSION_MOVE_TO_UNIT: {
			CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)missionData.iData1).getUnit(missionData.iData2);
			if (pTargetUnit == NULL || atPlot(pTargetUnit->plot()))
				bDone = true;
			break;
		}
		case MISSION_SKIP:
		case MISSION_SLEEP:
		case MISSION_FORTIFY:
		case MISSION_PLUNDER:
		case MISSION_AIRPATROL:
		case MISSION_SEAPATROL:
		case MISSION_HEAL:
		case MISSION_SENTRY_HEAL: // advc.004l
		case MISSION_SENTRY:
			FAssert(false);
			break;

		case MISSION_AIRLIFT:
		case MISSION_NUKE:
		case MISSION_RECON:
		case MISSION_PARADROP:
		case MISSION_AIRBOMB:
		case MISSION_BOMBARD:
		case MISSION_RANGE_ATTACK:
		case MISSION_PILLAGE:
		case MISSION_SABOTAGE:
		case MISSION_DESTROY:
		case MISSION_STEAL_PLANS:
		case MISSION_FOUND:
		case MISSION_SPREAD:
		case MISSION_SPREAD_CORPORATION:
		case MISSION_JOIN:
		case MISSION_CONSTRUCT:
		case MISSION_DISCOVER:
		case MISSION_HURRY:
		case MISSION_TRADE:
		case MISSION_GREAT_WORK:
		case MISSION_INFILTRATE:
		case MISSION_GOLDEN_AGE:
		case MISSION_LEAD:
		case MISSION_ESPIONAGE:
		case MISSION_DIE_ANIMATION:
			bDone = true;
			break;

		case MISSION_BUILD:
			// XXX what happens if two separate worker groups are both building the mine...
			/*if (plot()->getBuildType() != ((BuildTypes)(headMissionQueueNode()->m_data.iData1)))
					bDone = true; */
			break;

		default:
			FAssert(false);
			break;
		}
	}

	if (bAction &&
			//(bDone || !canAllMove())
			(bDone || !readyForMission())) // K-Mod (I don't think this actually matters)
	{	// <advc.102>
		bool bDestVisible = plot()->isVisibleToWatchingHuman();
		bool bStartVisible = pFromPlot->isVisibleToWatchingHuman();
		// Previously only DestVisible was checked
		if(bDestVisible || (bStartVisible && m->bInitiallyVisible)) {
			// Pass pFromPlot
			updateMissionTimer(iSteps, pFromPlot);
			if(g.getActivePlayer() != NO_PLAYER && getOwner() != g.getActivePlayer()) {
				bool bDestActiveVisible = !isInvisible(g.getActiveTeam());
				CvDLLInterfaceIFaceBase* pInterface = gDLL->getInterfaceIFace();
				if(gDLL->getEngineIFace()->isGlobeviewUp()) {
					if(bDestActiveVisible && g.getCurrentLayer() == GLOBE_LAYER_UNIT &&
							plot()->isActiveVisible(true))
						pInterface->setDirty(GlobeLayer_DIRTY_BIT, true);
				}
				else if(showMoves(*pFromPlot)) {
					// Show FromPlot when moving out of sight
					bool bStartActiveVisible = (bDestActiveVisible &&
							pFromPlot->isActiveVisible(false));
					bDestActiveVisible = (bDestActiveVisible &&
							plot()->isActiveVisible(false));
					if(bDestActiveVisible && bDestVisible)
						pInterface->lookAt(plot()->getPoint(), CAMERALOOKAT_NORMAL);
					else if(bStartActiveVisible && bStartVisible)
						pInterface->lookAt(pFromPlot->getPoint(), CAMERALOOKAT_NORMAL);
					// </advc.102>
				}
			}
		}
	}

	if (bDone)
	{	/* original bts code (roughly)
		if (!isBusy()) {
			if (getOwner() == g.getActivePlayer()) {
				if (IsSelected()) {
					if ((headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO) ||
						(headMissionQueueNode()->m_data.eMissionType == MISSION_ROUTE_TO) ||
						(headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_UNIT))
						g.cycleSelectionGroups_delayed(GET_PLAYER(getOwner()).isOption(PLAYEROPTION_QUICK_MOVES) ? 1 : 2, true, true);
				}
			}
			deleteMissionQueueNode(headMissionQueueNode());
		} */
		// K-Mod. If rapid-unit-cycling is enabled, I want to cycle as soon a possible. Otherwise, I want to mimic the original behaviour.
		// Note: I've removed cycleSelectionGroups_delayed(1, true, canAnyMove()) from inside CvSelectionGroup::deactivateHeadMission
		if (getOwner() == g.getActivePlayer() && IsSelected())
		{
			if ((missionData.eMissionType == MISSION_MOVE_TO ||
					missionData.eMissionType == MISSION_ROUTE_TO ||
					missionData.eMissionType == MISSION_MOVE_TO_UNIT) && !isBusy()) {
				g.cycleSelectionGroups_delayed(GET_PLAYER(getOwner()).
						isOption(PLAYEROPTION_QUICK_MOVES) ?
						2 : 3, true, canAnyMove()); // (? 1 : 2) + 1
			}
			else g.cycleSelectionGroups_delayed(1, true, canAnyMove());
		}

		if (!isBusy())
		{	// If this was a build mission, end the mission for all of our workers groups with the same job.
			// (this isn't strictly neccessary, because the workers will know to end their own mission anyway, but ending it now helps give a better unit cycling order.)
			if (missionData.eMissionType == MISSION_BUILD)
			{
				BuildTypes eBuildType = (BuildTypes)missionData.iData1; // (the head mission will be deleted soon)
				CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();
				while (pUnitNode)
				{
					CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = nextUnitNode(pUnitNode);
					if (pLoopUnit->isGroupHead() && pLoopUnit->getOwner() == getHeadOwner())
					{
						CvSelectionGroup* pLoopGroup = pLoopUnit->getGroup();
						if (pLoopGroup->getMissionType(0) == MISSION_BUILD &&
								pLoopGroup->getMissionData1(0) == eBuildType)
							pLoopGroup->deleteMissionQueueNode(pLoopGroup->headMissionQueueNode());
					}
				}
			}
			else deleteMissionQueueNode(pHeadMission);

			// start the next mission
			if (headMissionQueueNode())
				activateHeadMission();
		} // K-Mod end
	}
	else
	{	//if (canAllMove())
		if (readyForMission()) // K-Mod
		{
			//continueMission(iSteps + 1);
			return true;
		}
		else if (!isBusy() && getOwner() == g.getActivePlayer())
		{
			if (IsSelected())
				g.cycleSelectionGroups_delayed(1, true);
		}
	}
	return false;
}


bool CvSelectionGroup::canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache)
{
	PROFILE_FUNC();

	//cache isBusy
	if(bUseCache)
	{
		if(m_bIsBusyCache)
		{
			return false;
		}
	}
	else
	{
		if (isBusy())
		{
			return false;
		}
	}

	if(!canEverDoCommand(eCommand, iData1, iData2, bTestVisible, bUseCache))
		return false;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canDoCommand(eCommand, iData1, iData2, bTestVisible, false))
		{
			if(eCommand != COMMAND_LOAD) // advc.123c
				return true;
		} /*  <advc.123c> Normally, a group can do a command if any unit can do it,
			  but in the case of loading, it seems easier to make an exception than
			  to have the load command fail for some of the selected units. */
		else if(eCommand == COMMAND_LOAD)
			return false; // </advc.123c>
	}

	return eCommand == COMMAND_LOAD; // advc.123c: was //return false;
}

bool CvSelectionGroup::canEverDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible, bool bUseCache)
{
	if(eCommand == COMMAND_LOAD)
	{
		CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit *pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = plot()->nextUnitNode(pUnitNode);

			if (!pLoopUnit->isFull())
			{
				return true;
			}
		}

		//no cargo space on this plot
		return false;
	}
	else if(eCommand == COMMAND_UNLOAD)
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit *pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit->isCargo())
			{
				return true;
			}
		}

		//no loaded unit
		return false;
	}
	else if(eCommand == COMMAND_UPGRADE)
	{
		if(bUseCache)
		{
			//see if any of the different units can upgrade to this unit type
			for(int i=0;i<(int)m_aDifferentUnitCache.size();i++)
			{
				CvUnit *unit = m_aDifferentUnitCache[i];
				if(unit->canDoCommand(eCommand, iData1, iData2, bTestVisible, false))
					return true;
			}

			return false;
		}
	}

	return true;
}

void CvSelectionGroup::setupActionCache()
{
	//cache busy calculation
	m_bIsBusyCache = isBusy();

	//cache different unit types
	m_aDifferentUnitCache.erase(m_aDifferentUnitCache.begin(), m_aDifferentUnitCache.end());
	CLLNode<IDInfo> *pUnitNode = headUnitNode();
	while(pUnitNode != NULL)
	{
		CvUnit *unit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if(unit->isReadyForUpgrade())
		{
			UnitTypes unitType = unit->getUnitType();
			bool bFound = false;
			for(int i=0;i<(int)m_aDifferentUnitCache.size();i++)
			{
				if(unitType == m_aDifferentUnitCache[i]->getUnitType())
				{
					bFound = true;
					break;
				}
			}

			if(!bFound)
				m_aDifferentUnitCache.push_back(unit);
		}
	}
}

// Returns true if one of the units can support the interface mode...
bool CvSelectionGroup::canDoInterfaceMode(InterfaceModeTypes eInterfaceMode)
{
	PROFILE_FUNC();

	FAssertMsg(eInterfaceMode != NO_INTERFACEMODE, "InterfaceMode is not assigned a valid value");

	if (isBusy())
	{
		return false;
	}

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

		switch (eInterfaceMode)
		{
		case INTERFACEMODE_GO_TO:
			if ((getDomainType() != DOMAIN_AIR) && (getDomainType() != DOMAIN_IMMOBILE))
			{
				return true;
			}
			break;

		case INTERFACEMODE_GO_TO_TYPE:
			if ((getDomainType() != DOMAIN_AIR) && (getDomainType() != DOMAIN_IMMOBILE))
			{
				if (pLoopUnit->plot()->plotCount(PUF_isUnitType, pLoopUnit->getUnitType(), -1, pLoopUnit->getOwner()) > 1)
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_GO_TO_ALL:
			if ((getDomainType() != DOMAIN_AIR) && (getDomainType() != DOMAIN_IMMOBILE))
			{
				if (pLoopUnit->plot()->plotCount(NULL, -1, -1, pLoopUnit->getOwner()) > 1)
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_ROUTE_TO:
			if (pLoopUnit->AI_getUnitAIType() == UNITAI_WORKER)
			{
				if (pLoopUnit->canBuildRoute())
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_AIRLIFT:
			if (pLoopUnit->canAirlift(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_NUKE:
			if (pLoopUnit->canNuke(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_RECON:
			if (pLoopUnit->canRecon(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_PARADROP:
			if (pLoopUnit->canParadrop(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_AIRBOMB:
			if (pLoopUnit->canAirBomb(pLoopUnit->plot()))
			{
				return true;
			}
			break;

		case INTERFACEMODE_RANGE_ATTACK:
			if (pLoopUnit->canRangeStrike())
			{
				return true;
			}
			break;

		case INTERFACEMODE_AIRSTRIKE:
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				if (pLoopUnit->canAirAttack())
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_REBASE:
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				return true;
			}
			break;
		}

		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}

// Returns true if one of the units can execute the interface mode at the specified plot...
bool CvSelectionGroup::canDoInterfaceModeAt(InterfaceModeTypes eInterfaceMode, CvPlot* pPlot)
{
	FAssertMsg(eInterfaceMode != NO_INTERFACEMODE, "InterfaceMode is not assigned a valid value");

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

		switch (eInterfaceMode)
		{
		case INTERFACEMODE_AIRLIFT:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canAirliftAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY()))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_NUKE:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canNukeAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY()))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_RECON:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canReconAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY()))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_PARADROP:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canParadropAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY()))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_AIRBOMB:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canAirBombAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY()))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_RANGE_ATTACK:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canRangeStrikeAt(pLoopUnit->plot(), pPlot->getX(), pPlot->getY()))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_AIRSTRIKE:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canMoveInto(pPlot, true, false, false, false))
				{
					return true;
				}
			}
			break;

		case INTERFACEMODE_REBASE:
			if (pLoopUnit != NULL)
			{
				if (pLoopUnit->canMoveInto(pPlot))
				{
					return true;
				}
			}
			break;

		default:
			return true;
			break;
		}

		pUnitNode = nextUnitNode(pUnitNode);
	}

	return false;
}


bool CvSelectionGroup::isHuman() const
{
	if (getOwner() != NO_PLAYER)
	{
		return GET_PLAYER(getOwner()).isHuman();
	}

	return true;
}


bool CvSelectionGroup::isBusy()
{
	if (getNumUnits() == 0)
	{
		return false;
	}

	if (getMissionTimer() > 0)
	{
		return true;
	}

	CvPlot* pPlot = plot();

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->isCombat())
			{
				return true;
			}
		}
	}

	return false;
}


bool CvSelectionGroup::isCargoBusy()
{
	if (getNumUnits() == 0)
	{
		return false;
	}

	CvPlot* pPlot = plot();

	CLLNode<IDInfo>* pUnitNode1 = headUnitNode();
	while (pUnitNode1 != NULL)
	{
		CvUnit* pLoopUnit1 = ::getUnit(pUnitNode1->m_data);
		pUnitNode1 = nextUnitNode(pUnitNode1);

		if (pLoopUnit1 != NULL)
		{
			if (pLoopUnit1->getCargo() > 0)
			{
				CLLNode<IDInfo>* pUnitNode2 = pPlot->headUnitNode();
				while (pUnitNode2 != NULL)
				{
					CvUnit* pLoopUnit2 = ::getUnit(pUnitNode2->m_data);
					pUnitNode2 = pPlot->nextUnitNode(pUnitNode2);

					if (pLoopUnit2->getTransportUnit() == pLoopUnit1)
					{
						if (pLoopUnit2->getGroup()->isBusy())
						{
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}


int CvSelectionGroup::baseMoves() const
{
	int iBestValue = MAX_INT;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		int iValue = pLoopUnit->baseMoves();

		if (iValue < iBestValue)
		{
			iBestValue = iValue;
		}
	}

	return iBestValue;
}

// K-Mod
int CvSelectionGroup::maxMoves() const
{
	int iMoves = MAX_INT; // (was 0 - see comment below)

	for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		iMoves = std::min(iMoves, pLoopUnit->maxMoves());
		// note: in the original code, this used std::max -- I'm pretty sure that was just a mistake. I don't know why they'd want to use that.
	}
	return iMoves;
}

int CvSelectionGroup::movesLeft() const
{
	int iMoves = MAX_INT;

	for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL; pUnitNode = nextUnitNode(pUnitNode))
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		iMoves = std::min(iMoves, pLoopUnit->movesLeft());
	}
	return iMoves;
} // K-Mod end


bool CvSelectionGroup::isWaiting() const
{
	/* original bts code
	return (getActivityType() == ACTIVITY_HOLD ||
			getActivityType() == ACTIVITY_SLEEP ||
			getActivityType() == ACTIVITY_HEAL ||
			getActivityType() == ACTIVITY_SENTRY ||
			getActivityType() == ACTIVITY_PATROL ||
			getActivityType() == ACTIVITY_PLUNDER ||
			getActivityType() == ACTIVITY_INTERCEPT); */
	// K-Mod. (same functionality)
	return !(getActivityType() == ACTIVITY_AWAKE || getActivityType() == ACTIVITY_MISSION);
	// K-Mod end
}


bool CvSelectionGroup::isFull()
{
	if(getNumUnits() <= 0)
		return false;

	// do two passes, the first pass, we ignore units with special cargo
	int iSpecialCargoCount = 0;
	int iCargoCount = 0;

	// first pass, count but ignore special cargo units
	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->cargoSpace() > 0)
		{
			iCargoCount++;
		}

		if (pLoopUnit->specialCargo() != NO_SPECIALUNIT)
		{
			iSpecialCargoCount++;
		}
		else if (!pLoopUnit->isFull())
		{
			return false;
		}
	}

	// if every unit in the group has special cargo, then check those, otherwise, consider ourselves full
	if (iSpecialCargoCount >= iCargoCount)
	{
		pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (!(pLoopUnit->isFull()))
			{
				return false;
			}
		}
	}

	return true;
}


bool CvSelectionGroup::hasCargo()
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->hasCargo())
		{
			return true;
		}
	}

	return false;
}

int CvSelectionGroup::getCargo() const
{
	int iCargoCount = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		iCargoCount += pLoopUnit->getCargo();
	}

	return iCargoCount;
}

// K-Mod
int CvSelectionGroup::cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo, DomainTypes eDomainCargo) const
{
	int iSpace = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		iSpace += pLoopUnit->cargoSpaceAvailable(eSpecialCargo, eDomainCargo);
	}

	return iSpace;
} // K-Mod end


bool CvSelectionGroup::canAllMove()
{
	if(getNumUnits() <= 0)
		return false;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		FAssertMsg(pLoopUnit != NULL, "existing node, but NULL unit");

		if (pLoopUnit != NULL && !pLoopUnit->canMove())
		{
			return false;
		}
	}

	return true;
}


bool CvSelectionGroup::canAnyMove() const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canMove())
		{
			return true;
		}
	}

	return false;
}

// K-Mod. Originally, there there was a function called CvUnit::canCargoAllMove; which would only checked the cargo of that particular unit.
// I've removed that function and replaced it with this one, which checks the cargo of the entire group.
bool CvSelectionGroup::canCargoAllMove() const
{
	CvPlot* pPlot = plot();
	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->isCargo() && pLoopUnit->getTransportUnit()->getGroup() == this)
		{
			if (pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				if (!pLoopUnit->canMove())
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool CvSelectionGroup::hasMoved() const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->hasMoved())
		{
			return true;
		}
	}

	return false;
}


bool CvSelectionGroup::canEnterTerritory(TeamTypes eTeam, bool bIgnoreRightOfPassage) const
{
	if(getNumUnits() <= 0)
		return false;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!(pLoopUnit->canEnterTerritory(eTeam, bIgnoreRightOfPassage)))
		{
			return false;
		}
	}

	return true;
}

bool CvSelectionGroup::canEnterArea(TeamTypes eTeam, const CvArea* pArea, bool bIgnoreRightOfPassage) const
{
	if(getNumUnits() <= 0)
		return false;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!(pLoopUnit->canEnterArea(eTeam, pArea, bIgnoreRightOfPassage)))
		{
			return false;
		}
	}

	return true;
}


bool CvSelectionGroup::canMoveInto(CvPlot* pPlot, bool bAttack)
{
	if(getNumUnits() <= 0)
		return false;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canMoveInto(pPlot, bAttack))
		{
			return true;
		}
	}

	return false;
}


bool CvSelectionGroup::canMoveOrAttackInto(CvPlot* pPlot, bool bDeclareWar,
		bool bCheckMoves, bool bAssumeVisible) const // K-Mod
{
	if(getNumUnits() <= 0)
		return false;

	bool bVisible = bAssumeVisible || pPlot->isVisible(getHeadTeam(), false); // K-Mod
	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		//if (pLoopUnit->canMoveOrAttackInto(pPlot, bDeclareWar))
		if ((!bCheckMoves || pLoopUnit->canMove()) && // K-Mod
			(bVisible ? pLoopUnit->canMoveOrAttackInto(pPlot, bDeclareWar) : pLoopUnit->canMoveInto(pPlot, false, bDeclareWar, false, false)))
		{
			return true;
		}
	}

	return false;
}


bool CvSelectionGroup::canMoveThrough(CvPlot* pPlot, bool bDeclareWar, bool bAssumeVisible) const
{
	if(getNumUnits() <= 0)
		return false;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		//if (!pLoopUnit->canMoveThrough(pPlot))
		if (!pLoopUnit->canMoveInto(pPlot, false, bDeclareWar, true, bAssumeVisible)) // K-Mod
		{
			return false;
		}
	}

	return true;
}


bool CvSelectionGroup::canFight()
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canFight())
		{
			return true;
		}
	}

	return false;
}


bool CvSelectionGroup::canDefend()
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canDefend())
		{
			return true;
		}
	}

	return false;
}

bool CvSelectionGroup::canBombard(const CvPlot* pPlot)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->canBombard(pPlot))
		{
			return true;
		}
	}

	return false;
}

bool CvSelectionGroup::visibilityRange()
{
	int iMaxRange = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		int iRange = pLoopUnit->visibilityRange();
		if (iRange > iMaxRange)
		{
			iMaxRange = iRange;
		}
	}

	return iMaxRange;
}

/*  BETTER_BTS_AI_MOD, General AI, 03/30/10, jdog5000: START
	Approximate how many turns this group would take to reduce pCity's defense modifier to zero */
int CvSelectionGroup::getBombardTurns(CvCity* pCity) const
{
	PROFILE_FUNC();

	bool bHasBomber = (getOwner() != NO_PLAYER ? (GET_PLAYER(getOwner()).AI_calculateTotalBombard(DOMAIN_AIR) > 0) : false);
	bool bIgnoreBuildingDefense = bHasBomber;
	int iTotalBombardRate = (bHasBomber ? 16 : 0);
	int iUnitBombardRate = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->bombardRate() > 0)
		{
			iUnitBombardRate = pLoopUnit->bombardRate();

			if (pLoopUnit->ignoreBuildingDefense())
			{
				bIgnoreBuildingDefense = true;
			}
			else
			{
				iUnitBombardRate *= std::max(25, (100 - pCity->getBuildingBombardDefense()));
				iUnitBombardRate /= 100;
			}

			iTotalBombardRate += iUnitBombardRate;
		}
	}


	if (pCity->getTotalDefense(bIgnoreBuildingDefense) == 0)
	{
		return 0;
	}

	int iBombardTurns = pCity->getTotalDefense(bIgnoreBuildingDefense);

	if (iTotalBombardRate > 0)
	{
		iBombardTurns = (GC.getMAX_CITY_DEFENSE_DAMAGE() - pCity->getDefenseDamage());
		iBombardTurns *= pCity->getTotalDefense(false);
		iBombardTurns += (GC.getMAX_CITY_DEFENSE_DAMAGE() * iTotalBombardRate) - 1;
		iBombardTurns /= std::max(1, (GC.getMAX_CITY_DEFENSE_DAMAGE() * iTotalBombardRate));
	}

	//if (gUnitLogLevel > 2) logBBAI("      Bombard of %S will take %d turns at rate %d and current damage %d with bombard def %d", pCity->getName().GetCString(), iBombardTurns, iTotalBombardRate, pCity->getDefenseDamage(), (bIgnoreBuildingDefense ? 0 : pCity->getBuildingBombardDefense()));

	return iBombardTurns;
}


bool CvSelectionGroup::isHasPathToAreaPlayerCity(PlayerTypes ePlayer, int iFlags, int iMaxPathTurns) const
{
	PROFILE_FUNC();
	int iLoop;
	for(CvCity* pLoopCity = GET_PLAYER(ePlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(ePlayer).nextCity(&iLoop))
	{
		if (pLoopCity->area() == area())
		{
			int iPathTurns;
			if (generatePath(plot(), pLoopCity->plot(), iFlags, true, &iPathTurns, iMaxPathTurns))
			{
				if (iMaxPathTurns < 0 || iPathTurns <= iMaxPathTurns)
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvSelectionGroup::isHasPathToAreaEnemyCity(bool bIgnoreMinors, int iFlags, int iMaxPathTurns) const
{
	PROFILE_FUNC();

	//int iPass = 0; // advc.003: unused

	for(int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && isPotentialEnemy(getTeam(), TEAMID((PlayerTypes)iI)))
		{
			if (!bIgnoreMinors || (!GET_PLAYER((PlayerTypes)iI).isBarbarian() && !GET_PLAYER((PlayerTypes)iI).isMinorCiv()))
			{
				if (isHasPathToAreaPlayerCity((PlayerTypes)iI, iFlags, iMaxPathTurns))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvSelectionGroup::isStranded() const
{
	/*PROFILE_FUNC();
	if (!m_bIsStrandedCacheValid){
		m_bIsStrandedCache = calculateIsStranded();
		m_bIsStrandedCacheValid = true;
	}
	return m_bIsStrandedCache; */

	return AI_getMissionAIType() == MISSIONAI_STRANDED; // K-Mod
}


bool CvSelectionGroup::canMoveAllTerrain() const
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!(pLoopUnit->canMoveAllTerrain()))
		{
			return false;
		}
	}

	return true;
}
// BETTER_BTS_AI_MOD: END

void CvSelectionGroup::unloadAll()
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL)
		{
			pLoopUnit->unloadAll();
		}
	}
}


bool CvSelectionGroup::alwaysInvisible() const
{
	//PROFILE_FUNC(); // advc.003o
	if(getNumUnits() <= 0)
		return false;

	CLLNode<IDInfo>*pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!(pLoopUnit->alwaysInvisible()))
		{
			return false;
		}
	}

	return true;
}


bool CvSelectionGroup::isInvisible(TeamTypes eTeam) const
{
	if(getNumUnits() <= 0)
		return false;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!pLoopUnit->isInvisible(eTeam, false))
		{
			return false;
		}
	}

	return true;
}


int CvSelectionGroup::countNumUnitAIType(UnitAITypes eUnitAI)
{
	FAssertMsg(headUnitNode() != NULL, "headUnitNode() is not expected to be equal with NULL");

	int iCount = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		// count all units if NO_UNITAI passed in
		if (NO_UNITAI == eUnitAI || pLoopUnit->AI_getUnitAIType() == eUnitAI)
		{
			iCount++;
		}
	}

	return iCount;
}


bool CvSelectionGroup::hasWorker()
{
	return ((countNumUnitAIType(UNITAI_WORKER) > 0) || (countNumUnitAIType(UNITAI_WORKER_SEA) > 0));
}


bool CvSelectionGroup::IsSelected()
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit->IsSelected())
		{
			return true;
		}
	}

	return false;
}


void CvSelectionGroup::NotifyEntity(MissionTypes eMission)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		::getUnit(pUnitNode->m_data)->NotifyEntity(eMission);
		pUnitNode = nextUnitNode(pUnitNode);
	}
}


void CvSelectionGroup::airCircle(bool bStart)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		::getUnit(pUnitNode->m_data)->airCircle(bStart);
		pUnitNode = nextUnitNode(pUnitNode);
	}
}


void CvSelectionGroup::setBlockading(bool bStart)
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		::getUnit(pUnitNode->m_data)->setBlockading(bStart);
		pUnitNode = nextUnitNode(pUnitNode);
	}
}


int CvSelectionGroup::getX() const
{
	CvUnit* pHeadUnit = getHeadUnit();
	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->getX();
	}
	else
	{
		return INVALID_PLOT_COORD;
	}
}


int CvSelectionGroup::getY() const
{
	CvUnit* pHeadUnit = getHeadUnit();
	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->getY();
	}
	else
	{
		return INVALID_PLOT_COORD;
	}
}


bool CvSelectionGroup::at(int iX, int iY) const
{
	return((getX() == iX) && (getY() == iY));
}


bool CvSelectionGroup::atPlot( const CvPlot* pPlot) const
{
	return (plot() == pPlot);
}


CvPlot* CvSelectionGroup::plot() const
{
	CvUnit* pHeadUnit = getHeadUnit();
	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->plot();
	}
	else
	{
		return NULL;
	}
}


int CvSelectionGroup::getArea() const
{
	CvUnit* pHeadUnit = getHeadUnit();
	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->getArea();
	}
	else
	{
		return NULL;
	}
}

CvArea* CvSelectionGroup::area() const
{
	CvUnit* pHeadUnit = getHeadUnit();
	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->area();
	}
	else
	{
		return NULL;
	}
}


DomainTypes CvSelectionGroup::getDomainType() const
{
	CvUnit* pHeadUnit = getHeadUnit();
	if (pHeadUnit != NULL)
	{
		return getHeadUnit()->getDomainType();
	}
	else
	{
		return NO_DOMAIN;
	}
}


RouteTypes CvSelectionGroup::getBestBuildRoute(CvPlot* pPlot, BuildTypes* peBestBuild) const
{
	PROFILE_FUNC();

	if (peBestBuild != NULL)
	{
		*peBestBuild = NO_BUILD;
	}

	int iBestValue = 0;
	RouteTypes eBestRoute = NO_ROUTE;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
		{
			RouteTypes  eRoute = ((RouteTypes)(GC.getBuildInfo((BuildTypes) iI).getRoute()));

			if (eRoute != NO_ROUTE)
			{
				if (pLoopUnit->canBuild(pPlot, ((BuildTypes)iI)))
				{
					int iValue = GC.getRouteInfo(eRoute).getValue();

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestRoute = eRoute;
						if (peBestBuild != NULL)
						{
							*peBestBuild = ((BuildTypes)iI);
						}
					}
				}
			}
		}
	}

	return eBestRoute;
}

// Returns true if attack was made...
bool CvSelectionGroup::groupAttack(int iX, int iY, int iFlags, bool& bFailedAlreadyFighting,
		bool bMaxSurvival) // advc.048
{
	PROFILE_FUNC();

	FAssert(!isBusy()); // K-Mod

	CvPlot* pDestPlot = GC.getMap().plot(iX, iY);

	// K-Mod. Rather than clearing the existing path data; use a temporary pathfinder.
	KmodPathFinder final_path;
	final_path.SetSettings(this, iFlags & ~MOVE_DECLARE_WAR);
	/* original bts code
	if (iFlags & MOVE_THROUGH_ENEMY) {
		if (generatePath(plot(), pDestPlot, iFlags))
			pDestPlot = getPathFirstPlot();
	} */
	// K-Mod
	if (iFlags & (MOVE_THROUGH_ENEMY | MOVE_ATTACK_STACK) && !(iFlags & MOVE_DIRECT_ATTACK))
	{
		if (final_path.GeneratePath(pDestPlot))
		{
			pDestPlot = final_path.GetPathFirstPlot();
		}
	} // K-Mod end
	FAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");

	if (getNumUnits() <= 0)
		return false; // advc.003

	if(getDomainType() != DOMAIN_AIR && stepDistance(getX(), getY(),
			pDestPlot->getX(), pDestPlot->getY()) != 1)
		return false; // advc.003

	bool bAttack = false;
	//if ((iFlags & MOVE_DIRECT_ATTACK) || (getDomainType() == DOMAIN_AIR) || (iFlags & MOVE_THROUGH_ENEMY) || (generatePath(plot(), pDestPlot, iFlags) && (getPathFirstPlot() == pDestPlot)))
	// K-Mod.
	if (iFlags & (MOVE_THROUGH_ENEMY | MOVE_ATTACK_STACK | MOVE_DIRECT_ATTACK) ||
			getDomainType() == DOMAIN_AIR || (final_path.GeneratePath(pDestPlot) &&
			final_path.GetPathFirstPlot() == pDestPlot)) // K-Mod end
	{
		int iAttackOdds;
		CvUnit* pBestAttackUnit = AI_getBestGroupAttacker(pDestPlot, true, iAttackOdds);
		if (pBestAttackUnit == NULL)
			return false; // advc.003
		// K-Mod, bugfix. This needs to happen before hadDefender, since hasDefender tests for war..
		// (note: this check is no longer going to be important at all once my new AI DOW code is complete.)
		/*if (groupDeclareWar(pDestPlot))
			return true;*/
		// K-Mod end

		// if there are no defenders, do not attack
		/* original
		CvUnit* pBestDefender = pDestPlot->getBestDefender(NO_PLAYER, getOwner(), pBestAttackUnit, true);
		if (NULL == pBestDefender)
			return false;*/
		// Lead From Behind by UncutDragon
		if (!pDestPlot->hasDefender(false, NO_PLAYER, getOwner(), pBestAttackUnit, true))
			return false;

		//bool bNoBlitz = (!pBestAttackUnit->isBlitz() || !pBestAttackUnit->isMadeAttack());
		/*  advc.164: This looks OK - exclude units that have made
			at least 1 attack. Just for clarity: */
		bool bBlitz = (pBestAttackUnit->isBlitz() && pBestAttackUnit->isMadeAttack());
		/*if (groupDeclareWar(pDestPlot))
			return true; */ // K-Mod, moved up.
		while (true)
		{	// <advc.048>
			// (Don't want to add another pure virtual function to the messed up CvSelectionGroup class)
			pBestAttackUnit = static_cast<CvSelectionGroupAI*>(this)-> // </advc.048>
					AI_getBestGroupAttacker(pDestPlot, false, iAttackOdds,
					false, /* advc.164: */ !bBlitz,
					!bMaxSurvival, bMaxSurvival); // advc.048
			if (pBestAttackUnit == NULL)
				break;

			// advc.048: AI_getBestGroupSacrifice moved into AI_getBestGroupAttacker

			bAttack = true;

			if (GC.getUSE_DO_COMBAT_CALLBACK()) { // K-Mod. block unused python callbacks
				CySelectionGroup* pyGroup = new CySelectionGroup(this);
				CyPlot* pyPlot = new CyPlot(pDestPlot);
				CyArgsList argsList;
				argsList.add(gDLL->getPythonIFace()->makePythonObject(pyGroup));
				argsList.add(gDLL->getPythonIFace()->makePythonObject(pyPlot));
				long lResult=0;
				gDLL->getPythonIFace()->callFunction(PYGameModule, "doCombat", argsList.makeFunctionArgs(), &lResult);
				delete pyGroup; delete pyPlot;
				if (lResult == 1)
					break;
			}

			bool bStack = (isHuman() && (getDomainType() == DOMAIN_AIR ||
					GET_PLAYER(getOwner()).isOption(PLAYEROPTION_STACK_ATTACK)));
			bFailedAlreadyFighting = false;
			if (getNumUnits() > 1)
			{	/* original bts code
				if (pBestAttackUnit->plot()->isFighting() || pDestPlot->isFighting())
					bFailedAlreadyFighting = true;
				else pBestAttackUnit->attack(pDestPlot, bStack);*/
				// K-Mod
				if (pBestAttackUnit->plot()->isFighting() || pDestPlot->isFighting())
					bFailedAlreadyFighting = true;
				//if (!pBestAttackUnit->isCombat())
				FAssert(!pBestAttackUnit->isCombat());
				// we need to issue the attack order to start the attack
				pBestAttackUnit->attack(pDestPlot, bStack);
				// K-Mod end
			}
			else
			{	// K-Mod note. We should do this even if the fight can't happen right away.
				pBestAttackUnit->attack(pDestPlot, false);
				break;
			}
			if (bFailedAlreadyFighting || !bStack)
			{
				if (!isHuman() && getNumUnits() > 1 &&
						// K-Mod: if this is AI stack, follow through with the attack to the end
						!(iFlags & MOVE_SINGLE_ATTACK))
				{	//AI_queueGroupAttack(iX, iY);
					AI_queueGroupAttack(pDestPlot->getX(), pDestPlot->getY()); // K-Mod
				}
				break;
			}
		}
	}
	return bAttack;
}


void CvSelectionGroup::groupMove(CvPlot* pPlot, bool bCombat, CvUnit* pCombatUnit, bool bEndMove)
{
	//PROFILE_FUNC();

	FAssert(!isBusy()); // K-Mod

	// K-Mod. Some variables to help us regroup appropriately if not everyone can move.
	CvSelectionGroup* pStaticGroup = 0;
	UnitAITypes eHeadAI = getHeadUnitAI();

	// Move the combat unit first, so that no-capture units don't get unneccarily left behind.
	if (pCombatUnit)
		pCombatUnit->move(pPlot, true);
	// K-Mod end

	/*  advc.001: Units that can't capture cities move in stage 1 (i.e. always last).
		This allows other units to capture an empty city, and then all units can
		advance as one group. Relevant when attacking with a Gunship grouped
		together with weaker units. (pCombatUnit is then NULL.)
		CvUnit::updateCombat may still unselect the no-capture unit through
		checkRemoveSelectionAfterAttack. This could be fixed, doesn't seem worth
		the trouble.
		K-Mod 1.45 has rewritten this function, which may fix the problem (and some others
		too), but then I'd also have to merge the K-Mod fix for the Gunship city capture
		bug ... too much work. */
	for(int iStage = 0; iStage < 2; iStage++) {
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			//if ((pLoopUnit->canMove() && ((bCombat && (!(pLoopUnit->isNoCapture()) || !(pPlot->isEnemyCity(*pLoopUnit)))) ? pLoopUnit->canMoveOrAttackInto(pPlot) : pLoopUnit->canMoveInto(pPlot))) || (pLoopUnit == pCombatUnit))
			// K-Mod
			if (pLoopUnit == pCombatUnit)
				continue; // this unit is moved before the loop.
			// <advc.001>
			if(pLoopUnit->isNoCapture() != (bool)iStage)
				continue; // </advc.001>
			if (pLoopUnit->canMove() &&
					/*  advc.001: This condition was removed in K-Mod 1.44, but is needed
						b/c canMoveOrAttackInto doesn't cover it (perhaps it should). */
					!(pLoopUnit->isNoCapture() && pPlot->isEnemyCity(*pLoopUnit)) &&
					(bCombat ? pLoopUnit->canMoveOrAttackInto(pPlot) : pLoopUnit->canMoveInto(pPlot)))
				pLoopUnit->move(pPlot, true);
			else
			{
				/* original bts code
				pLoopUnit->joinGroup(NULL, true);
				pLoopUnit->ExecuteMove(((float)(GC.getMissionInfo(MISSION_MOVE_TO).getTime() * gDLL->getMillisecsPerTurn())) / 1000.0f, false); */

				// K-Mod. all units left behind should stay in the same group. (unless it would mean a change of group AI)
				// (Note: it is important that units left behind are not in the original group.
				// The later code assumes that the original group has moved, and if it hasn't, there will be an infinite loop.)
				if (pStaticGroup && (isHuman() || pStaticGroup->getHeadUnitAI() == eHeadAI))
					pLoopUnit->joinGroup(pStaticGroup, true);
				else
				{
					pLoopUnit->joinGroup(0, true);
					pStaticGroup = pLoopUnit->getGroup();
				}
				//
			}
			// K-Mod. If the unit is no longer in the original group; then display it's movement animation now.
			// (this replaces the ExecuteMove line commented out in the above block, and it also handles the case of loading units onto boats.)
			if (pLoopUnit->getGroupID() != getID())
				pLoopUnit->ExecuteMove(((float)(GC.getMissionInfo(MISSION_MOVE_TO).getTime() * gDLL->getMillisecsPerTurn())) / 1000.0f, false);
			// K-Mod end
		}
	} // advc.001: end of iStage loop

	//execute move
	if(bEndMove || !canAllMove())
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while(pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			pLoopUnit->ExecuteMove(((float)(GC.getMissionInfo(MISSION_MOVE_TO).getTime() * gDLL->getMillisecsPerTurn())) / 1000.0f, false);
		}
	}
}

// Returns true if move was made...
bool CvSelectionGroup::groupPathTo(int iX, int iY, int iFlags)
{
	KmodPathFinder final_path; // K-Mod
	CvPlot* pOriginPlot = plot(); // K-Mod

	if (at(iX, iY))
	{
		return false; // XXX is this necessary?
	}

	FAssert(!isBusy());
	FAssert(getOwner() != NO_PLAYER);
	FAssert(headMissionQueueNode() != NULL);

	CvPlot* pDestPlot = GC.getMap().plot(iX, iY);
	FAssertMsg(pDestPlot != NULL, "DestPlot is not assigned a valid value");

	//FAssertMsg(canAllMove(), "canAllMove is expected to be true");
	FAssert(getDomainType() == DOMAIN_AIR ? canAnyMove() : canAllMove()); // K-Mod

	CvPlot* pPathPlot;

	if (getDomainType() == DOMAIN_AIR)
	{
		if (!canMoveInto(pDestPlot))
		{
			return false;
		}

		pPathPlot = pDestPlot;
	}
	else
	{
		/* original bts code
		if (!generatePath(plot(), pDestPlot, iFlags & ~MOVE_DECLARE_WAR))
			return false;
		pPathPlot = getPathFirstPlot(); */
		// K-Mod. I've added & ~MOVE_DECLARE_WAR so that if we need to declare war at this point, and haven't yet done so,
		// the move will fail here rather than splitting the group inside groupMove.
		// Also, I've change it to use a different pathfinder, to avoid clearing the path data - and to avoid OOS errors.
		final_path.SetSettings(this, iFlags & ~MOVE_DECLARE_WAR);
		if (!final_path.GeneratePath(pDestPlot))
		{
			return false;
		}

		pPathPlot = final_path.GetPathFirstPlot();
		// K-Mod end

		if (groupAmphibMove(pPathPlot, iFlags))
		{
			return false;
		}
	}

	/* original bts code
	bool bForce = false;
	MissionAITypes eMissionAI = AI_getMissionAIType();
	if (eMissionAI == MISSIONAI_BLOCKADE || eMissionAI == MISSIONAI_PILLAGE)
		bForce = true;
	if (groupDeclareWar(pPathPlot, bForce))
		return false;*/
	// Disabled by K-Mod. AI war decisions have no business being here.

	bool bEndMove = false;
	if(pPathPlot == pDestPlot)
		bEndMove = true;

	//groupMove(pPathPlot, iFlags & MOVE_THROUGH_ENEMY, NULL, bEndMove);
	groupMove(pPathPlot, false, NULL, bEndMove); // K-Mod

	FAssert(getNumUnits() == 0 || atPlot(pPathPlot)); // K-Mod

	// K-Mod.
	if (!AI_isControlled() && !bEndMove)
	{
		//If the step we just took will make us change our path to something longer, then cancel the move.
		// This prevents units from wasting all their moves by trying to walk around enemy units.
		FAssert(final_path.IsPathComplete());
		std::pair<int, int> old_moves = std::make_pair(final_path.GetPathTurns(), -final_path.GetFinalMoves());
		if (!final_path.GeneratePath(pDestPlot)
			|| std::make_pair(final_path.GetPathTurns(), -final_path.GetFinalMoves()) > old_moves)
		{
			clearMissionQueue();
		}
		// Also, if the step we just took causes us to backtrack - its probably because we've lost vision of a unit that was blocking the path.
		// Apply the MOVE_ASSUME_VISIBLE flag, so that we remember to go the long way around.
		else if (final_path.GetPathFirstPlot() == pOriginPlot)
		{
			headMissionQueueNode()->m_data.iFlags |= MOVE_ASSUME_VISIBLE;
		}
	}
	// K-Mod end

	return true;
}

// Returns true if move was made...
bool CvSelectionGroup::groupRoadTo(int iX, int iY, int iFlags)
{
	if (!AI_isControlled() || !at(iX, iY) || getLengthMissionQueue() == 1)
	{
		BuildTypes eBestBuild = NO_BUILD;
		//RouteTypes eBestRoute = // advc.003: unused
			getBestBuildRoute(plot(), &eBestBuild);
		if (eBestBuild != NO_BUILD)
		{
			groupBuild(eBestBuild);
			return true;
		}
	}
	return groupPathTo(iX, iY, iFlags
			/*  advc.049: In the debugger, I'm seeing this function get called via
				continueMission without any flags set, and these calls cause
				the AI to build roads through foreign territory. */
			| MOVE_ROUTE_TO);
}


// Returns true if build should continue...
bool CvSelectionGroup::groupBuild(BuildTypes eBuild, /* advc.011b: */ bool bFinish)
{
	FAssert(getOwner() != NO_PLAYER);
	FAssertMsg(eBuild < GC.getNumBuildInfos(), "Invalid Build");

	bool bContinue = false;
	CvPlot* pPlot = plot();
	/* original bts code
	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();
	if (eImprovement != NO_IMPROVEMENT) {
		if (AI_isControlled()) {
			if (GET_PLAYER(getOwner()).isOption(PLAYEROPTION_SAFE_AUTOMATION)) {
				if ((pPlot->getImprovementType() != NO_IMPROVEMENT) && (pPlot->getImprovementType() != GC.getRUINS_IMPROVEMENT())) {
					BonusTypes eBonus = (BonusTypes)pPlot->getNonObsoleteBonusType(GET_PLAYER(getOwner()).getTeam());
					if ((eBonus == NO_BONUS) || !GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eBonus)) {
						if (GC.getImprovementInfo(eImprovement).getImprovementPillage() != NO_IMPROVEMENT)
							return false;
	}}}}}*/
	/*  K-Mod. Leave old improvements should mean _all_ improvements,
		not 'unless it will connect a resource'.
		Note. The only time this bit of code might matter is if the automated unit has orders queued.
		Ideally, the AI should never issue orders which violate the leave old improvements rule. */
	if (isAutomated() && GET_PLAYER(getOwner()).isOption(PLAYEROPTION_SAFE_AUTOMATION) &&
		GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT && pPlot->getImprovementType() != NO_IMPROVEMENT &&
		pPlot->getImprovementType() != GC.getRUINS_IMPROVEMENT()
		// <advc.121> Forts on unworkable tiles are OK despite SAFE_AUTOMATION.
		&& (!GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBuild).
		getImprovement()).isActsAsCity() || pPlot->getWorkingCity() == NULL)
		) // </advc.121>
	{
		FAssertMsg(false, "AI has issued an order which violates PLAYEROPTION_SAFE_AUTOMATION");
		return false;
	}
	// K-Mod end
	bool bStopOtherWorkers = false; // advc.011c
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		FAssertMsg(pLoopUnit->atPlot(pPlot), "pLoopUnit is expected to be at pPlot");
		if(!pLoopUnit->canBuild(pPlot, eBuild))
			continue; // advc.003

		bContinue = true;
		if (pLoopUnit->build(eBuild))
		{
			bContinue = false;
			break;
		}
		// advc.011c:
		if(!bFinish && isHuman() && pPlot->getBuildTurnsLeft(eBuild, getOwner()) == 1) {
			// <advc.011b>
			CvWString szBuild = GC.getBuildInfo(eBuild).getDescription();
			// Get rid of the LINK tags b/c these result in an underscore
			for(int i = 0; i < 2; i++) {
				int posOpening = szBuild.find(L'<');
				if(posOpening == CvWString::npos)
					continue;
				int posClosing = szBuild.find(L'>');
				if(posClosing == CvWString::npos || posClosing < posOpening)
					continue;
				szBuild = (szBuild.substr(0, posOpening) +
						szBuild.substr(posClosing + 1, szBuild.length() - posClosing - 1));
			}
			CvWString szBuffer = gDLL->getText("TXT_KEY_BUILD_NOT_FINISHED", szBuild.c_str());
			gDLL->getInterfaceIFace()->addHumanMessage(getOwner(), false,
					GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_INFO,
					GC.getBuildInfo(eBuild).getButton()/*getHeadUnit()->getButton()*/,
					(ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"/*"COLOR_BUILDING_TEXT"*/),
					getX(), getY(), true, false);
			// </advc.011b>
			// <advc.011c>
			bContinue = false;
			bStopOtherWorkers = true;
			break;
		}
	}
	if(bStopOtherWorkers) {
		pUnitNode = pPlot->headUnitNode();
		while(pUnitNode != NULL) {
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);
			CvSelectionGroup* pSelectionGroup = pLoopUnit->getGroup();
			if(pSelectionGroup != NULL && pSelectionGroup != this &&
					pSelectionGroup->getOwner() == getOwner() &&
					pSelectionGroup->getActivityType() == ACTIVITY_MISSION &&
					pSelectionGroup->getLengthMissionQueue() > 0 &&
					pSelectionGroup->getMissionType(0) == GC.getBuildInfo(eBuild).getMissionType() &&
					pSelectionGroup->getMissionData1(0) == eBuild)
				pSelectionGroup->deleteMissionQueueNode(pSelectionGroup->headMissionQueueNode());
		}
	} // </advc.001c>

	return bContinue;
}


void CvSelectionGroup::setTransportUnit(CvUnit* pTransportUnit,
		// BETTER_BTS_AI_MOD, General AI, 04/18/10, jdog5000:
		CvSelectionGroup** pOtherGroup)
{
	// if we are loading
	if (pTransportUnit != NULL)
	{
		CvUnit* pHeadUnit = getHeadUnit();
		FAssertMsg(pHeadUnit != NULL, "non-zero group without head unit");

		int iCargoSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pHeadUnit->getSpecialUnitType(), pHeadUnit->getDomainType());

		// if no space at all, give up
		if (iCargoSpaceAvailable < 1)
		{
			return;
		}

		// if there is space, but not enough to fit whole group, then split us, and set on the new group
		if (iCargoSpaceAvailable < getNumUnits())
		{
			CvSelectionGroup* pSplitGroup = splitGroup(iCargoSpaceAvailable, NULL,
					pOtherGroup); // BETTER_BTS_AI_MOD, General AI, 04/18/10, jdog5000
			if (pSplitGroup != NULL)
			{
				pSplitGroup->setTransportUnit(pTransportUnit);
			}

			return;
		}

		FAssertMsg(iCargoSpaceAvailable >= getNumUnits(), "cargo size too small");

		// setTransportUnit removes the unit from the current group (at least currently), so we have to be careful in the loop here
		// so, loop until we do not load one
		bool bLoadedOne;
		do
		{
			bLoadedOne = false;

			// loop over all the units, finding one to load
			CLLNode<IDInfo>* pUnitNode = headUnitNode();
			while (pUnitNode != NULL && !bLoadedOne)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				// just in case implementation of setTransportUnit changes, check to make sure this unit is not already loaded
				if (pLoopUnit != NULL && pLoopUnit->getTransportUnit() != pTransportUnit)
				{
					// if there is room, load the unit and stop the loop (since setTransportUnit ungroups this unit currently)
					bool bSpaceAvailable = pTransportUnit->cargoSpaceAvailable(pLoopUnit->getSpecialUnitType(), pLoopUnit->getDomainType());
					if (bSpaceAvailable)
					{
						pLoopUnit->setTransportUnit(pTransportUnit);
						bLoadedOne = true;

					}
				}
			}
		}
		while (bLoadedOne);
	}
	// otherwise we are unloading
	else
	{
		// loop over all the units, unloading them
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit != NULL)
			{
				// unload unit
				pLoopUnit->setTransportUnit(NULL);
			}
		}
	}
}

bool CvSelectionGroup::isAmphibPlot(const CvPlot* pPlot) const
{
	bool bFriendlyCity = true; // K-Mod. I've renamed this from bFriendly, because it was confusing me.
	CvUnit* pUnit = getHeadUnit();
	if (NULL != pUnit)
	{
		bFriendlyCity = pPlot->isFriendlyCity(*pUnit, true);
	}

	//return ((getDomainType() == DOMAIN_SEA) && pPlot->isCoastalLand() && !bFriendlyCity && !canMoveAllTerrain());
	// BETTER_BTS_AI_MOD, General AI, 04/18/10, jdog5000: START
	if (getDomainType() == DOMAIN_SEA)
	{
		if (pPlot->isCity() && !bFriendlyCity && (pPlot->isCoastalLand() || pPlot->isWater() || canMoveAllTerrain()))
		{
			return true;
		}
		return (pPlot->isCoastalLand() && !bFriendlyCity && !canMoveAllTerrain());
	}
	return false;
	// BETTER_BTS_AI_MOD: END
}

// Returns true if attempted an amphib landing...
bool CvSelectionGroup::groupAmphibMove(CvPlot* pPlot, int iFlags)
{
	bool bLanding = false;

	FAssert(getOwner() != NO_PLAYER);

	/* original bts code
	if (groupDeclareWar(pPlot))
		return true;*/
	// K-Mod. I've disabled this groupDeclareWar for a bunch of reasons. Suffice to say, it shouldn't be here.

	if (isAmphibPlot(pPlot))
	{
		if (stepDistance(getX(), getY(), pPlot->getX(), pPlot->getY()) == 1)
		{
			CLLNode<IDInfo>* pUnitNode1 = headUnitNode();

			// K-Mod: I've rearranged some stuff in the following section to fix a bug.
			// originally, the cargo groups loop was done for each cargo-carrying unit - which is incorrect.
			std::vector<CvSelectionGroup*> aCargoGroups;
			while (pUnitNode1 != NULL)
			{
				CvUnit* pLoopUnit1 = ::getUnit(pUnitNode1->m_data);
				pUnitNode1 = nextUnitNode(pUnitNode1);

				if (pLoopUnit1->getCargo() > 0 && pLoopUnit1->domainCargo() == DOMAIN_LAND)
				{
					std::vector<CvUnit*> aCargoUnits;
					pLoopUnit1->getCargoUnits(aCargoUnits);
					for (size_t i = 0; i < aCargoUnits.size(); ++i)
					{
						CvSelectionGroup* pGroup = aCargoUnits[i]->getGroup();
						if (std::find(aCargoGroups.begin(), aCargoGroups.end(), pGroup) == aCargoGroups.end())
						{
							aCargoGroups.push_back(aCargoUnits[i]->getGroup());
						}
					}
				}
			}
			for (size_t i = 0; i < aCargoGroups.size(); ++i)
			{
				CvSelectionGroup* pGroup = aCargoGroups[i];
				if (pGroup->canAllMove())
				{
					FAssert(!pGroup->at(pPlot->getX(), pPlot->getY()));
					pGroup->pushMission(MISSION_MOVE_TO, pPlot->getX(), pPlot->getY(), (MOVE_IGNORE_DANGER | iFlags));
					bLanding = true;
				}
			}
			// K-Mod end
		}
	}

	return bLanding;
}


bool CvSelectionGroup::readyToSelect(bool bAny)
{
	return (readyToMove(bAny) && !isAutomated());
}


bool CvSelectionGroup::readyToMove(bool bAny)
{
	//return (((bAny) ? canAnyMove() : canAllMove()) && (headMissionQueueNode() == NULL) && (getActivityType() == ACTIVITY_AWAKE) && !isBusy() && !isCargoBusy());
	// K-Mod:
	return (bAny ? canAnyMove() : canAllMove()) &&
		(isForceUpdate() ||
		(headMissionQueueNode() == NULL && getActivityType() ==  ACTIVITY_AWAKE)) &&
		!isBusy() && !isCargoBusy();
}


bool CvSelectionGroup::readyToAuto()
{
	//return (canAllMove() && (headMissionQueueNode() != NULL));
	return readyForMission() || (isAutomated() && getActivityType() == ACTIVITY_AWAKE && canAllMove());
}

// K-Mod. In the original code, there was an implicit assumption that all missions required "canAllMove".
// I've removed that assumption by creating a pair of new functions:
// CvSelectionGroup::canDoMission, which returns true if the group is capable of executing a particular mission - including an optional movement points check.
// CvSelectionGroup::readyForMission, which basically just calls canDoMission for the current mission, as direct replacement for the canAllMove condition.
// (canStartMission now calls canDoMission.)
bool CvSelectionGroup::readyForMission()
{
	if (headMissionQueueNode() == NULL)
		return false;

	MissionData& kData = headMissionQueueNode()->m_data;

	// direct attack is a special case...    sorry about that.
	bool bCheckMoves = true;

	if (kData.eMissionType == MISSION_MOVE_TO && kData.iFlags & MOVE_DIRECT_ATTACK)
	{
		if (canAnyMove())
			bCheckMoves = false;
		else
			return false;
	}

	return (canDoMission(kData.eMissionType, kData.iData1, kData.iData2, plot(),
			false, bCheckMoves) || canAllMove());
	// note: if the whole group can move, but they can't do the mission, then the mission will be canceled inside CvSelectionGroup::continueMission.
}


bool CvSelectionGroup::canDoMission(int iMission, int iData1, int iData2,
		CvPlot* pPlot, bool bTestVisible, bool bCheckMoves) const
{
	if(!pPlot)
		pPlot = plot();

	bool bValid = false;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		switch (iMission)
		{
		case MISSION_MOVE_TO:
			if (!bValid)
			{
				if (pPlot->at(iData1, iData2))
					return false;

				if (!bCheckMoves)
					return true;

				if (pLoopUnit->getDomainType() != DOMAIN_AIR)
					bValid = true; // air units don't have to move as a group
			}

			if (pLoopUnit->canMove() != bValid)
				return !bValid; // huh?
			break;

		case MISSION_ROUTE_TO:
			if (!bValid)
			{
				if (pPlot->at(iData1, iData2) && getBestBuildRoute(pPlot) == NO_ROUTE)
					return false;

				if (!bCheckMoves)
					return true;

				bValid = true;
			}
			if (!pLoopUnit->canMove())
				return false;
			break;

		case MISSION_MOVE_TO_UNIT:
		{
			FAssertMsg(iData1 != NO_PLAYER, "iData1 should be a valid Player");
			CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)iData1).getUnit(iData2);
			if (!bValid)
			{
				if (!pTargetUnit || pTargetUnit->atPlot(pPlot))
					return false;

				if (!bCheckMoves)
					return true;

				bValid = true;
			}
			if (!pLoopUnit->canMove())
				return false;
			break;
		}

		case MISSION_SKIP:
			if (pLoopUnit->canHold(pPlot))
				return true;
			break;

		case MISSION_SLEEP:
			if (pLoopUnit->canSleep(pPlot))
				return true;
			break;

		case MISSION_FORTIFY:
			if (pLoopUnit->canFortify(pPlot))
				return true;
			break;

		case MISSION_AIRPATROL:
			if (pLoopUnit->canAirPatrol(pPlot) && (!bCheckMoves || pLoopUnit->canMove()))
				return true; // note: this mission will automatically ungroup any unsuitable units.
			break;

		case MISSION_SEAPATROL: // <advc.004k>
			if(isHuman())
				return false;
			else { // </advc.004k>
				if (!bValid && pLoopUnit->canSeaPatrol(pPlot))
				{
					if (!bCheckMoves)
						return true;

					bValid = true;
				}
				if (!pLoopUnit->canMove())
					return false;
				break;
			}
		case MISSION_HEAL:
			if (pLoopUnit->canHeal(pPlot)
					// advc.004l: AI_control check only for performance
					&& (const_cast<CvSelectionGroup*>(this)->AI_isControlled() || !pLoopUnit->canSentryHeal(pPlot)))
				return true;
			break;
		// <advc.004l> Make the two heal missions mutually exclusive (for humans)
		case MISSION_SENTRY_HEAL:
			if(pLoopUnit->canHeal(pPlot) && pLoopUnit->canSentryHeal(pPlot))
				return true;
			break;
		// </advc.004l>
		case MISSION_SENTRY:
			if (pLoopUnit->canSentry(pPlot))
				return true;
			break;

		case MISSION_AIRLIFT:
			if (pLoopUnit->canAirliftAt(pPlot, iData1, iData2) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_NUKE:
			if (pLoopUnit->canNukeAt(pPlot, iData1, iData2) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_RECON:
			if (pLoopUnit->canReconAt(pPlot, iData1, iData2) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_PARADROP:
			if (pLoopUnit->canParadropAt(pPlot, iData1, iData2) && (!bCheckMoves || pLoopUnit->canMove()))
				return true; // note: this mission will automatically ungroup any unsuitable units.
			break;

		case MISSION_AIRBOMB:
			if (pLoopUnit->canAirBombAt(pPlot, iData1, iData2) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_BOMBARD:
			if (pLoopUnit->canBombard(pPlot) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_RANGE_ATTACK:
			if (pLoopUnit->canRangeStrikeAt(pPlot, iData1, iData2) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_PLUNDER:
			if (pLoopUnit->canPlunder(pPlot, bTestVisible) &&
					/*  advc.001: Replacing the clause below. The bug occurred when
						a player set a unit to Blockade (=plunder), spending all its
						movement points, and then clicking on the Blockade button
						again. The unit then stopped blockading. Not sure if this
						is the best way to fix it, but it least it works: Hides the
						Blockade button when a unit has no moves left. */
					pLoopUnit->canMove())
					//(!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_PILLAGE:
			if (pLoopUnit->canPillage(pPlot) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_SABOTAGE:
			if (pLoopUnit->canSabotage(pPlot, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_DESTROY:
			if (pLoopUnit->canDestroy(pPlot, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_STEAL_PLANS:
			if (pLoopUnit->canStealPlans(pPlot, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_FOUND:
			if (pLoopUnit->canFound(pPlot, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_SPREAD:
			if (pLoopUnit->canSpread(pPlot, (ReligionTypes)iData1, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_SPREAD_CORPORATION:
			if (pLoopUnit->canSpreadCorporation(pPlot, (CorporationTypes)iData1, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_JOIN:
			if (pLoopUnit->canJoin(pPlot, (SpecialistTypes)iData1) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_CONSTRUCT:
			if (pLoopUnit->canConstruct(pPlot, (BuildingTypes)iData1, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_DISCOVER:
			if (pLoopUnit->canDiscover(pPlot) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_HURRY:
			if (pLoopUnit->canHurry(pPlot, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_TRADE:
			if (pLoopUnit->canTrade(pPlot, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_GREAT_WORK:
			if (pLoopUnit->canGreatWork(pPlot) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_INFILTRATE:
			if (pLoopUnit->canInfiltrate(pPlot, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_GOLDEN_AGE:
			//this means to play the animation only
			if (iData1 != -1)
				return true;

			if (pLoopUnit->canGoldenAge(pPlot, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_BUILD:
			FAssertMsg(((BuildTypes)iData1) < GC.getNumBuildInfos(), "Invalid Build");
			if (pLoopUnit->canBuild(pPlot, (BuildTypes)iData1, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_LEAD:
			if (pLoopUnit->canLead(pPlot, iData1) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_ESPIONAGE:
			if (pLoopUnit->canEspionage(pPlot, bTestVisible) && (!bCheckMoves || pLoopUnit->canMove()))
				return true;
			break;

		case MISSION_DIE_ANIMATION:
			return false;
			break;

		case MISSION_BEGIN_COMBAT:
		case MISSION_END_COMBAT:
		case MISSION_AIRSTRIKE: // note: airstrike missions are actually done using MISSION_MOVE_TO. This mission is apparently only used for graphics.
		case MISSION_SURRENDER:
		case MISSION_IDLE:
		case MISSION_DIE:
		case MISSION_DAMAGE:
		case MISSION_MULTI_SELECT:
		case MISSION_MULTI_DESELECT:
			break;

		default:
			FAssert(false);
			break;
		}
	}

	return bValid;
}
// K-Mod end

int CvSelectionGroup::getID() const
{
	return m_iID;
}


void CvSelectionGroup::setID(int iID)
{
	m_iID = iID;
}


TeamTypes CvSelectionGroup::getTeam() const
{
	if (getOwner() != NO_PLAYER)
	{
		return GET_PLAYER(getOwner()).getTeam();
	}

	return NO_TEAM;
}


int CvSelectionGroup::getMissionTimer() const
{
	return m_iMissionTimer;
}


void CvSelectionGroup::setMissionTimer(int iNewValue)
{
	FAssert(getOwner() != NO_PLAYER);

	m_iMissionTimer = iNewValue;
	FAssert(getMissionTimer() >= 0);
}


void CvSelectionGroup::changeMissionTimer(int iChange)
{
	setMissionTimer(getMissionTimer() + iChange);
}


void CvSelectionGroup::updateMissionTimer(int iSteps, /* advc.102: */ CvPlot* pFromPlot)
{
	int iTime = 0;
	if (!isHuman() && (!showMoves( // <advc.102>
			pFromPlot == NULL ? *plot() : *pFromPlot) ||
			// Are these timers synchronized?
			(!GC.getGame().isNetworkMultiPlayer() &&
			gDLL->getEngineIFace()->isGlobeviewUp()))) // </advc.102>
		iTime = 0;
	else if (headMissionQueueNode() != NULL)
	{
		iTime = GC.getMissionInfo((MissionTypes)(headMissionQueueNode()->m_data.eMissionType)).getTime();

		if ((headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO) ||
				(headMissionQueueNode()->m_data.eMissionType == MISSION_ROUTE_TO) ||
				(headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_UNIT))
		{
			CvPlot* pTargetPlot = NULL;
			if (headMissionQueueNode()->m_data.eMissionType == MISSION_MOVE_TO_UNIT)
			{
				CvUnit* pTargetUnit = GET_PLAYER((PlayerTypes)headMissionQueueNode()->m_data.iData1).getUnit(headMissionQueueNode()->m_data.iData2);
				if (pTargetUnit != NULL)
				{
					pTargetPlot = pTargetUnit->plot();
				}
			}
			else
			{
				pTargetPlot = GC.getMap().plot(headMissionQueueNode()->m_data.iData1, headMissionQueueNode()->m_data.iData2);
			}

			if (atPlot(pTargetPlot))
			{
				iTime += iSteps;
			}
			else
			{
				iTime = std::min(iTime, 2);
			}
		}

		if (isHuman() && (isAutomated() || (GET_PLAYER(
				GC.getGame().isNetworkMultiPlayer() ? getOwner() :
				GC.getGame().getActivePlayer()).
				isOption(PLAYEROPTION_QUICK_MOVES))))
			iTime = std::min(iTime, 1);
	}

	setMissionTimer(iTime);
}

// K-Mod. this is what force update use to do - but I don't use it like this anymore.
/*void CvSelectionGroup::doForceUpdate() {

	if (isForceUpdate()) {
		setForceUpdate(false);
		clearMissionQueue();
		setActivityType(ACTIVITY_AWAKE);
		// if we are in the middle of attacking with a stack, cancel it
		AI_cancelGroupAttack();
	}
}*/
// K-Mod end

ActivityTypes CvSelectionGroup::getActivityType() const
{
	return m_eActivityType;
}


void CvSelectionGroup::setActivityType(ActivityTypes eNewValue)
{

	FAssert(getOwner() != NO_PLAYER);

	ActivityTypes eOldActivity = getActivityType();
	if(eOldActivity == eNewValue)
		return;

	CvPlot* pPlot = plot();

	if (eOldActivity == ACTIVITY_INTERCEPT)
	{
		airCircle(false);
	}

	setBlockading(false);

	bool bWasWaiting = isWaiting(); // K-Mod

	m_eActivityType = eNewValue;

	// K-Mod
	if (bWasWaiting && !isWaiting())
		GET_PLAYER(getOwner()).updateGroupCycle(this);
	// K-Mod end

	if (getActivityType() == ACTIVITY_INTERCEPT)
	{
		airCircle(true);
	}

	if (getActivityType() != ACTIVITY_MISSION)
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		if (getActivityType() != ACTIVITY_INTERCEPT)
		{
			//don't idle intercept animation
			while (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				pLoopUnit->NotifyEntity(MISSION_IDLE);
			}
		}

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			if (pPlot != NULL)
			{
				pPlot->setFlagDirty(true);
			}
		}
	}

	if (pPlot == gDLL->getInterfaceIFace()->getSelectionPlot())
	{
		gDLL->getInterfaceIFace()->setDirty(PlotListButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}
}


AutomateTypes CvSelectionGroup::getAutomateType() const
{
	return m->eAutomateType;
}


bool CvSelectionGroup::isAutomated() const
{
	return (getAutomateType() != NO_AUTOMATE);
}


void CvSelectionGroup::setAutomateType(AutomateTypes eNewValue)
{
	FAssert(getOwner() != NO_PLAYER);
	FAssert(isHuman() || eNewValue == NO_AUTOMATE); // The AI shouldn't use automation.

	if (getAutomateType() != eNewValue)
	{
		m->eAutomateType = eNewValue;

		clearMissionQueue();
		setActivityType(ACTIVITY_AWAKE);

		// if canceling automation, cancel on cargo as well
		if (eNewValue == NO_AUTOMATE)
		{
			CvPlot* pPlot = plot();
			if (pPlot != NULL)
			{
				CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
				while (pUnitNode != NULL)
				{
					CvUnit* pCargoUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pPlot->nextUnitNode(pUnitNode);

					CvUnit* pTransportUnit = pCargoUnit->getTransportUnit();
					if (pTransportUnit != NULL && pTransportUnit->getGroup() == this)
					{
						pCargoUnit->getGroup()->setAutomateType(NO_AUTOMATE);
						pCargoUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
					}
				}
			}
		}
	}
}

// Disabled by K-Mod. (This function is deprecated.)
/* FAStarNode* CvSelectionGroup::getPathLastNode() const
{
	//return path_finder.GetEndNode();
	//return gDLL->getFAStarIFace()->GetLastNode(&GC.getPathFinder());
} */


CvPlot* CvSelectionGroup::getPathFirstPlot() const
{
	return path_finder.GetPathFirstPlot();
}


CvPlot* CvSelectionGroup::getPathEndTurnPlot() const
{
	return path_finder.GetPathEndTurnPlot();
}


bool CvSelectionGroup::generatePath( const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags, bool bReuse, int* piPathTurns, int iMaxPath) const
{
	// K-Mod - if I can stop the UI from messing with this pathfinder, I might be able to reduce OOS bugs.
	// (note, the const-cast is just to get around the bad code from the original developers)
	FAssert(const_cast<CvSelectionGroup*>(this)->AI_isControlled());
	// K-Mod end

	PROFILE("CvSelectionGroup::generatePath()");

	if (pFromPlot == NULL || pToPlot == NULL)
		return false;

	/*if (!bReuse)
		path_finder.Reset();*/
	path_finder.SetSettings(this, iFlags, iMaxPath);
	bool bSuccess = path_finder.GeneratePath(pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY());
	/* test.
	if (bSuccess != gDLL->getFAStarIFace()->GeneratePath(&GC.getPathFinder(), pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(), false, iFlags, bReuse)) {
		pNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getPathFinder());
		if (bSuccess || iMaxPath < 0 || !pNode || pNode->m_iData2 <= iMaxPath) {
			//::MessageBoxA(NULL,"pathfind mismatch","CvGameCore",MB_OK);
			FAssert(false);
		}
	}*/

	if (piPathTurns != NULL)
	{
		*piPathTurns = MAX_INT;

		if (bSuccess)
		{
			*piPathTurns = path_finder.GetPathTurns();
		}
	}

	return bSuccess;
}

/* void CvSelectionGroup::resetPath() const
{
	//path_finder.Reset(); // note. the K-Mod finder doesn't need resetting in all the same places.
	gDLL->getFAStarIFace()->ForceReset(&GC.getPathFinder());
} */


void CvSelectionGroup::clearUnits()
{
	CLLNode<IDInfo>* pUnitNode;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pUnitNode = deleteUnitNode(pUnitNode);
	}
}


// Returns true if the unit is added...
bool CvSelectionGroup::addUnit(CvUnit* pUnit, bool bMinimalChange)
{
	//PROFILE_FUNC();

	if (!(pUnit->canJoinGroup(pUnit->plot(), this)))
	{
		return false;
	}

	bool bAdded = false;

	// K-Mod. Start the air circling animation if required. (CvSelectionGroup::joinGroup will interupt the air patrol mission anyway.)
	/* if (getActivityType() == ACTIVITY_INTERCEPT)
		pUnit->airCircle(true); */ // disabled for now, because it results in jerky animations.
	// K-Mod end

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		if ((pUnit->AI_groupFirstVal() > pLoopUnit->AI_groupFirstVal()) ||
			  ((pUnit->AI_groupFirstVal() == pLoopUnit->AI_groupFirstVal()) &&
				 (pUnit->AI_groupSecondVal() > pLoopUnit->AI_groupSecondVal())))
		{
			m_units.insertBefore(pUnit->getIDInfo(), pUnitNode);
			bAdded = true;
			break;
		}
		pUnitNode = nextUnitNode(pUnitNode);
	}

	if (!bAdded)
	{
		m_units.insertAtEnd(pUnit->getIDInfo());
	}

	if(!bMinimalChange)
	{
		if (getOwner() == NO_PLAYER)
		{
			if (getNumUnits() > 0)
			{
				pUnitNode = headUnitNode();
				while (pUnitNode != NULL)
				{//if (pUnitNode != headUnitNode())
					::getUnit(pUnitNode->m_data)->NotifyEntity(MISSION_MULTI_SELECT);
					pUnitNode = nextUnitNode(pUnitNode);
				}
			}
		}
	}

	return true;
}


void CvSelectionGroup::removeUnit(CvUnit* pUnit)
{
	CLLNode<IDInfo>* pUnitNode;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		if (::getUnit(pUnitNode->m_data) == pUnit)
		{
			// K-Mod. Cancel the air circling animation.
			if (getActivityType() == ACTIVITY_INTERCEPT)
				pUnit->airCircle(false);
			// K-Mod end
			deleteUnitNode(pUnitNode);
			break;
		}
		else
		{
			pUnitNode = nextUnitNode(pUnitNode);
		}
	}
}


CLLNode<IDInfo>* CvSelectionGroup::deleteUnitNode(CLLNode<IDInfo>* pNode)
{
	CLLNode<IDInfo>* pNextUnitNode;

	if (getOwner() != NO_PLAYER)
	{
		setAutomateType(NO_AUTOMATE);
		clearMissionQueue();

		switch (getActivityType())
		{
		case ACTIVITY_SLEEP:
		case ACTIVITY_INTERCEPT:
		case ACTIVITY_PATROL:
		case ACTIVITY_PLUNDER:
		case ACTIVITY_BOARDED: // advc.075
			break;
		default:
			setActivityType(ACTIVITY_AWAKE);
			break;
		}
	}

	pNextUnitNode = m_units.deleteNode(pNode);

	return pNextUnitNode;
}


CLLNode<IDInfo>* CvSelectionGroup::nextUnitNode(CLLNode<IDInfo>* pNode) const
{
	return m_units.next(pNode);
}


int CvSelectionGroup::getNumUnits() const
{
	return m_units.getLength();
}

void CvSelectionGroup::mergeIntoGroup(CvSelectionGroup* pSelectionGroup)
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	// merge groups, but make sure we do not change the head unit AI
	// this means that if a new unit is going to become the head, change its AI to match, if possible
	// AI_setUnitAIType removes the unit from the current group (at least currently), so we have to be careful in the loop here
	// so, loop until we have not changed unit AIs
	bool bChangedUnitAI;
	do
	{
		bChangedUnitAI = false;

		// loop over all the units, moving them to the new group,
		// stopping if we had to change a unit AI, because doing so removes that unit from our group, so we have to start over
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		while (pUnitNode != NULL && !bChangedUnitAI)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit != NULL)
			{
				UnitAITypes eUnitAI = pLoopUnit->AI_getUnitAIType();

				// if the unitAIs are different, and the loop unit has a higher val, then the group unitAI would change
				// change this UnitAI to the old group UnitAI if possible
				CvUnit* pNewHeadUnit = pSelectionGroup->getHeadUnit();
				UnitAITypes eNewHeadUnitAI = pSelectionGroup->getHeadUnitAI();
				if (pNewHeadUnit!= NULL && eUnitAI != eNewHeadUnitAI && pLoopUnit->AI_groupFirstVal() > pNewHeadUnit->AI_groupFirstVal())
				{
					// non-zero AI_unitValue means that this UnitAI is valid for this unit (that is the check used everywhere)
					if (kPlayer.AI_unitValue(pLoopUnit->getUnitType(), eNewHeadUnitAI, NULL) > 0)
					{
						// this will remove pLoopUnit from the current group
						pLoopUnit->AI_setUnitAIType(eNewHeadUnitAI);

						bChangedUnitAI = true;
					}
				}

				pLoopUnit->joinGroup(pSelectionGroup);
			}
		}
	}
	while (bChangedUnitAI);
}

/*  split this group into two new groups, one of iSplitSize,
	the other the remaining units.
	split up each unit AI type as evenly as possible. */
/*  K-Mod. I've rewritten most of this function. The new version is faster,
	gives a more even split, and does not create a dummy group.
	(unless I've made a mistake.) */
CvSelectionGroup* CvSelectionGroup::splitGroup(int iSplitSize, CvUnit* pNewHeadUnit, CvSelectionGroup** ppOtherGroup)
{
	FAssert(pNewHeadUnit == 0 || pNewHeadUnit->getGroup() == this);

	if (iSplitSize <= 0)
	{
		FAssertMsg(false, "non-positive splitGroup size");
		return NULL;
	}

	// are we already small enough?
	if (getNumUnits() <= iSplitSize)
	{
		return this;
	}

	CvUnit* pOldHeadUnit = getHeadUnit();

	// if pNewHeadUnit NULL, then we will use our current head to head the new split group of target size
	if (pNewHeadUnit == NULL)
		pNewHeadUnit = pOldHeadUnit;

	UnitAITypes eOldHeadAI = pOldHeadUnit->AI_getUnitAIType();
	UnitAITypes eNewHeadAI = pNewHeadUnit->AI_getUnitAIType();

	int iGroupSize = getNumUnits();

	int aiTotalAIs[NUM_UNITAI_TYPES] = {};
	int aiNewGroupAIs[NUM_UNITAI_TYPES] = {};
	FAssert(iGroupSize > 0);

	// populate 'aiTotalAIs' with the number of each AI type in the existing group.
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		aiTotalAIs[pLoopUnit->AI_getUnitAIType()]++;
	}

	// Next, from those numbers, work out how many of each unit type we need in new group.
	// round evenly, and carry the rounding-error onto the next unit type so that we don't have an off-by-one error at the end.
	int iCarry = 0;

	// There are a couple of special cases that we need to do first (so that iCarry can work correctly at the end)
	{
		// reserve a unit of the old AI type to lead the remainder group
		// and more importantly, reserve a unit of the new AI type to lead the new group!

		int x = std::max(1, (aiTotalAIs[eNewHeadAI] * iSplitSize + iGroupSize/2 + iCarry) / iGroupSize);
		if (eOldHeadAI == eNewHeadAI)
		{
			if (x > 1 && aiTotalAIs[eOldHeadAI] == x)
				x--;

			iCarry += aiTotalAIs[eNewHeadAI] * iSplitSize - x * iGroupSize;
			aiNewGroupAIs[eNewHeadAI] = x;
		}
		else
		{
			iCarry += aiTotalAIs[eNewHeadAI] * iSplitSize - x * iGroupSize;
			aiNewGroupAIs[eNewHeadAI] = x;

			x = (aiTotalAIs[eOldHeadAI] * iSplitSize + iGroupSize/2 + iCarry) / iGroupSize;

			if (x > 0 && aiTotalAIs[eOldHeadAI] == x)
				x--;

			iCarry += aiTotalAIs[eOldHeadAI] * iSplitSize - x * iGroupSize;
			aiNewGroupAIs[eOldHeadAI] = x;
		}
	}

	for (UnitAITypes i = (UnitAITypes)0; i < NUM_UNITAI_TYPES; i = (UnitAITypes)(i+1))
	{
		if (aiTotalAIs[i] == 0 || i == eNewHeadAI || i == eOldHeadAI)
			continue; // already done. (see above)

		int x = (aiTotalAIs[i] * iSplitSize + iGroupSize/2 + iCarry) / iGroupSize;

		// In rare situations x can be rounded up above the maximum,
		// because iCarry may oversized if one of the original head units is reserved.
		x = std::min(x, aiTotalAIs[i]);
		FAssert(x >= 0 && x <= aiTotalAIs[i]);

		iCarry += aiTotalAIs[i] * iSplitSize - x * iGroupSize;
		aiNewGroupAIs[i] = x;
		FAssert(iCarry >= -iGroupSize && iCarry <= iGroupSize);
	}
	FAssert(iCarry == 0);

	// make the new group for the new head
	pNewHeadUnit->joinGroup(0);
	CvSelectionGroup* pSplitGroup = pNewHeadUnit->getGroup();
	aiNewGroupAIs[eNewHeadAI]--;
	aiTotalAIs[eNewHeadAI]--;
	CvSelectionGroup* pRemainderGroup = this;

	// Populate the new group with the quantity of AI types specified in aiNewGroupAIs.
	// However, the units of each AI type should not simply be taken from the front of the list,
	// because we always want to have an even distribution of unit types. (not just unitAI types)
	pUnitNode = headUnitNode();
	while (pUnitNode)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		UnitAITypes eAI = pLoopUnit->AI_getUnitAIType();
		FAssert(eAI != NO_UNITAI);
		if (aiNewGroupAIs[eAI] > 0)
		{
			if (iGroupSize * aiNewGroupAIs[eAI] >= iSplitSize * aiTotalAIs[eAI])
			{
				pLoopUnit->joinGroup(pSplitGroup);
				aiNewGroupAIs[eAI]--;
				FAssert(aiNewGroupAIs[eAI] >= 0);
			}
		}
		aiTotalAIs[eAI]--; // (note. this isn't really important if aiNewGroupAIs[eAI] == 0; but we might as well keep it accurate anyway.)
		FAssert(aiTotalAIs[eAI] >= 0);
	}

	FAssert(pSplitGroup->getNumUnits() == iSplitSize);

	// K-Mod
	// if the remainder group doesn't have the same unitAI, then it should be split up, so that we don't get any strange groups forming.
	// Note: the force split can be overridden by the calling function if need be.
	/*  <advc.706> Uncommented this old K-Mod code b/c my splitGroup(1) calls
		failed the FAssert below. */
	if (GC.getGame().isOption(GAMEOPTION_RISE_FALL) && pRemainderGroup && pRemainderGroup->getHeadUnitAI() != eOldHeadAI)
		pRemainderGroup->AI_setForceSeparate(); // </advc.706>
	FAssert(!pRemainderGroup || pRemainderGroup->getHeadUnitAI() == eOldHeadAI || pRemainderGroup->AI_isForceSeparate()); // this should now be automatic, because of my other edits.
	// K-Mod end

	if (ppOtherGroup != NULL)
		*ppOtherGroup = pRemainderGroup;

	return pSplitGroup;
}

// K-Mod
// If the group has units of different plots, this function will create one new group for each of those plots
void CvSelectionGroup::regroupSeparatedUnits()
{
	const CvUnit* pHeadUnit = getHeadUnit();
	UnitAITypes eHeadUnitAI = getHeadUnitAI(); // the AI doesn't like to stay grouped when the group's AI type had changed.
	std::vector<CvSelectionGroup*> new_groups;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	FAssert(pHeadUnit || !pUnitNode);
	while (pUnitNode)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		if (pLoopUnit->plot() != pHeadUnit->plot())
		{
			bool bFoundGroup = false;
			for (size_t i = 0; !bFoundGroup && i < new_groups.size(); i++)
			{
				if (pLoopUnit->plot() == new_groups[i]->plot())
				{
					if (isHuman() || new_groups[i]->getHeadUnitAI() == eHeadUnitAI)
						pLoopUnit->joinGroup(new_groups[i], true);
					else
						pLoopUnit->joinGroup(0, true);
					bFoundGroup = true;
				}
			}
			if (!bFoundGroup)
			{
				pLoopUnit->joinGroup(0, true);
				new_groups.push_back(pLoopUnit->getGroup());
			}
		}
	}
}
// K-Mod end

// Returns the zero-based index of the unit within the group, or -1 if it is not in the group.
int CvSelectionGroup::getUnitIndex(CvUnit* pUnit, int maxIndex /* = -1 */) const
{
	int iIndex = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit == pUnit)
		{
			return iIndex;
		}

		iIndex++;

		//early out if not interested beyond maxIndex
		if((maxIndex >= 0) && (iIndex >= maxIndex))
			return -1;
	}

	return -1;
}

CLLNode<IDInfo>* CvSelectionGroup::headUnitNode() const
{
	return m_units.head();
}


CvUnit* CvSelectionGroup::getHeadUnit() const
{
	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	if (pUnitNode != NULL)
	{
		return ::getUnit(pUnitNode->m_data);
	}
	else
	{
		return NULL;
	}
}

CvUnit* CvSelectionGroup::getUnitAt(int index) const
{
	int numUnits = getNumUnits();
	if(index >= numUnits)
	{
		FAssertMsg(false, "[Jason] Selectiongroup unit index out of bounds.");
		return NULL;
	}
	else
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();
		for(int i=0;i<index;i++)
			pUnitNode = nextUnitNode(pUnitNode);

		CvUnit *pUnit = ::getUnit(pUnitNode->m_data);
		return pUnit;
	}
}


UnitAITypes CvSelectionGroup::getHeadUnitAI() const
{
	CvUnit* pHeadUnit = getHeadUnit();
	if (pHeadUnit != NULL)
	{
		return pHeadUnit->AI_getUnitAIType();
	}

	return NO_UNITAI;
}


PlayerTypes CvSelectionGroup::getHeadOwner() const
{
	CvUnit* pHeadUnit = getHeadUnit();
	if (pHeadUnit != NULL)
	{
		return pHeadUnit->getOwner();
	}

	return NO_PLAYER;
}


TeamTypes CvSelectionGroup::getHeadTeam() const
{
	CvUnit* pHeadUnit = getHeadUnit();
	if (pHeadUnit != NULL)
	{
		return pHeadUnit->getTeam();
	}

	return NO_TEAM;
}


void CvSelectionGroup::clearMissionQueue()
{
	FAssert(getOwner() != NO_PLAYER);

	deactivateHeadMission();

	m_missionQueue.clear();

	if (getOwner() == GC.getGame().getActivePlayer() && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}


int CvSelectionGroup::getLengthMissionQueue() const
{
	return m_missionQueue.getLength();
}


MissionData* CvSelectionGroup::getMissionFromQueue(int iIndex) const
{
	CLLNode<MissionData>* pMissionNode;

	pMissionNode = m_missionQueue.nodeNum(iIndex);

	if (pMissionNode != NULL)
	{
		return &(pMissionNode->m_data);
	}
	else
	{
		return NULL;
	}
}


void CvSelectionGroup::insertAtEndMissionQueue(MissionData mission, bool bStart)
{
	//PROFILE_FUNC();

	FAssert(getOwner() != NO_PLAYER);

	m_missionQueue.insertAtEnd(mission);

	if (getLengthMissionQueue() == 1 && bStart)
	{
		activateHeadMission();
	}

	if (getOwner() == GC.getGame().getActivePlayer() && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}


CLLNode<MissionData>* CvSelectionGroup::deleteMissionQueueNode(CLLNode<MissionData>* pNode)
{
	FAssertMsg(pNode != NULL, "Node is not assigned a valid value");
	FAssert(getOwner() != NO_PLAYER);

	if (pNode == headMissionQueueNode())
	{
		deactivateHeadMission();
	}

	CLLNode<MissionData>* pNextMissionNode = m_missionQueue.deleteNode(pNode);

	/* original bts code
	if (pNextMissionNode == headMissionQueueNode())
		activateHeadMission();*/
	// Disabled by K-Mod. It should be possible to delete the head mission without immediately starting the next one!

	if (getOwner() == GC.getGame().getActivePlayer() && IsSelected())
	{
		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}

	return pNextMissionNode;
}


CLLNode<MissionData>* CvSelectionGroup::nextMissionQueueNode(CLLNode<MissionData>* pNode) const
{
	return m_missionQueue.next(pNode);
}


CLLNode<MissionData>* CvSelectionGroup::prevMissionQueueNode(CLLNode<MissionData>* pNode) const
{
	return m_missionQueue.prev(pNode);
}


CLLNode<MissionData>* CvSelectionGroup::headMissionQueueNode() const
{
	return m_missionQueue.head();
}


CLLNode<MissionData>* CvSelectionGroup::tailMissionQueueNode() const
{
	return m_missionQueue.tail();
}


int CvSelectionGroup::getMissionType(int iNode) const
{
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if (iNode == iCount)
		{
			return pMissionNode->m_data.eMissionType;
		}

		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}

	return -1;
}


int CvSelectionGroup::getMissionData1(int iNode) const
{
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if (iNode == iCount)
		{
			return pMissionNode->m_data.iData1;
		}

		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}

	return -1;
}


int CvSelectionGroup::getMissionData2(int iNode) const
{
	int iCount = 0;
	CLLNode<MissionData>* pMissionNode = headMissionQueueNode();

	while (pMissionNode != NULL)
	{
		if (iNode == iCount)
		{
			return pMissionNode->m_data.iData2;
		}

		iCount++;

		pMissionNode = nextMissionQueueNode(pMissionNode);
	}

	return -1;
}

// <advc.075>
void CvSelectionGroup::handleBoarded() {

	if(!isHuman() || getDomainType() != DOMAIN_SEA ||
			GET_PLAYER(getOwner()).isOption(PLAYEROPTION_NO_UNIT_CYCLING))
		return;
	CLLNode<MissionData>* pMissionNode = headMissionQueueNode();
	if(pMissionNode == NULL) {
		FAssert(pMissionNode != NULL); // MOVE_TO mission should still be queued
		return;
	}
	if(nextMissionQueueNode(pMissionNode) != NULL)
		return;
	if(movesLeft() || !hasMoved())
		return;
	if(plot()->isWater() && !plot()->isAdjacentToLand())
		return;

	std::vector<CvSelectionGroup*> apLandCargoGroups;
	getLandCargoGroups(apLandCargoGroups);
	std::vector<CvSelectionGroup*> aAwake;
	for(size_t i = 0; i < apLandCargoGroups.size(); i++) {
		CvSelectionGroup& kGroup = *apLandCargoGroups[i];
		if(kGroup.getActivityType() == ACTIVITY_BOARDED && kGroup.canDisembark())
			aAwake.push_back(&kGroup);
	} // Putting all awoken units in one group should be more convenient
	if(!aAwake.empty())
		aAwake[0]->setActivityType(ACTIVITY_AWAKE);
	for(size_t i = 1; i < aAwake.size(); i++) {
		if(aAwake[i]->getDomainType() == aAwake[0]->getDomainType())
			aAwake[i]->mergeIntoGroup(aAwake[0]);
		// One of the doDelayedDeath calls will clean the empty groups up
	}
}


bool CvSelectionGroup::canDisembark() const {

	if(!plot()->isWater() && movesLeft())
		return true;
	for(int i = 0; i < NUM_DIRECTION_TYPES; i++) {
		CvPlot* pAdj = plotDirection(getX(), getY(), (DirectionTypes)i);
		if(pAdj != NULL && canMoveOrAttackInto(pAdj, false, true, false))
			return true;
	}
	return false;
}


void CvSelectionGroup::resetBoarded() {

	if(!isHuman() || getDomainType() != DOMAIN_SEA ||
			GET_PLAYER(getOwner()).isOption(PLAYEROPTION_NO_UNIT_CYCLING))
		return;

	std::vector<CvSelectionGroup*> apLandCargoGroups;
	getLandCargoGroups(apLandCargoGroups);
	for(size_t i = 0; i < apLandCargoGroups.size(); i++) {
		if(apLandCargoGroups[i]->getActivityType() == ACTIVITY_AWAKE)
			apLandCargoGroups[i]->setActivityType(ACTIVITY_BOARDED);
	}
}


void CvSelectionGroup::getLandCargoGroups(std::vector<CvSelectionGroup*>& r) {

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while(pUnitNode != NULL) {
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		if(pLoopUnit->domainCargo() == DOMAIN_LAND && pLoopUnit->hasCargo()) {
			std::vector<CvUnit*> apCargo;
			pLoopUnit->getCargoUnits(apCargo);
			for(size_t i = 0; i < apCargo.size(); i++) {
				CvSelectionGroup* gr = apCargo[i]->getGroup();
				if(gr == NULL) {
					FAssert(gr != NULL);
					continue;
				}
				r.push_back(gr);
			}
		}
	}
} // </advc.075>


void CvSelectionGroup::read(FDataStreamBase* pStream)
{
	// Init saved data
	reset();

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iID);
	pStream->Read(&m_iMissionTimer);

	pStream->Read(&m_bForceUpdate);

	pStream->Read((int*)&m_eOwner);
	pStream->Read((int*)&m_eActivityType);
	pStream->Read((int*)&m->eAutomateType);

	m_units.Read(pStream);
	// <advc.004l>
	if(uiFlag >= 2)
		m->knownEnemies.Read(pStream); // </advc.004l>
	// <advc.011b>
	if(uiFlag <= 0) {
		CLinkList<MissionDataLegacy> tmp;
		tmp.Read(pStream);
		for(CLLNode<MissionDataLegacy>* pNode = tmp.head(); pNode != NULL; pNode = tmp.next(pNode)) {
			MissionDataLegacy tmpMission = pNode->m_data;
			MissionData mission;
			mission.bModified = false;
			mission.eMissionType = tmpMission.eMissionType;
			mission.iData1 = tmpMission.iData1;
			mission.iData2 = tmpMission.iData2;
			mission.iFlags = tmpMission.iFlags;
			mission.iPushTurn = tmpMission.iPushTurn;
			m_missionQueue.insertAtEnd(mission);
		}
	}
	else // </advc.011b>
		m_missionQueue.Read(pStream);
}


void CvSelectionGroup::write(FDataStreamBase* pStream)
{
	uint uiFlag=0;
	uiFlag = 1; // advc.011b
	uiFlag = 2; // advc.004l
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iID);
	pStream->Write(m_iMissionTimer);

	pStream->Write(m_bForceUpdate);

	pStream->Write(m_eOwner);
	pStream->Write(m_eActivityType);
	pStream->Write(m->eAutomateType);

	m_units.Write(pStream);
	m->knownEnemies.Write(pStream); // advc.004l
	m_missionQueue.Write(pStream);
}

// Protected Functions...

void CvSelectionGroup::activateHeadMission()
{
	FAssert(getOwner() != NO_PLAYER);

	if (headMissionQueueNode() != NULL)
	{
		if (!isBusy())
		{
			startMission();
		}
	}
}


void CvSelectionGroup::deactivateHeadMission()
{
	FAssert(getOwner() != NO_PLAYER);

	if (headMissionQueueNode() != NULL)
	{
		if (getActivityType() == ACTIVITY_MISSION)
		{
			setActivityType(ACTIVITY_AWAKE);
		}

		setMissionTimer(0);

		/* if (getOwner() == GC.getGame().getActivePlayer()) {
			if (IsSelected())
				GC.getGame().cycleSelectionGroups_delayed(1, true, canAnyMove());
		} */
	}
}
