// CvDeal.cpp

#include "CvGameCoreDLL.h"
#include "CvDeal.h"
#include "CoreAI.h"
#include "UWAIAgent.h" // advc.104
#include "CvCity.h"
#include "CvMap.h"
#include "CvGameTextMgr.h"
#include "CvInfo_Civics.h"
#include "CvInfo_Terrain.h" // just for a logBBAI call :(
#include "BBAILog.h" // BETTER_BTS_AI_MOD, AI logging, 10/02/09, jdog5000


CvDeal::CvDeal()
{
	reset();
}


CvDeal::~CvDeal()
{
	uninit();
}


void CvDeal::init(int iID, PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer)
{
	reset(iID, eFirstPlayer, eSecondPlayer); // Reset serialized data
	setInitialGameTurn(GC.getGame().getGameTurn());
}


void CvDeal::uninit()
{
	m_firstTrades.clear();
	m_secondTrades.clear();
}


// Initializes data members that are serialized.
void CvDeal::reset(int iID, PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer)
{
	uninit();

	m_iID = iID;
	m_iInitialGameTurn = 0;

	m_eFirstPlayer = eFirstPlayer;
	m_eSecondPlayer = eSecondPlayer;
}


void CvDeal::kill(bool bKillTeam, /* advc.130p: */ PlayerTypes eCancelPlayer)
{
	if (getLengthFirstTrades() > 0 || getLengthSecondTrades() > 0)
	{	// <advc.106j>
		bool bForce = false;
		for(CLLNode<TradeData> const* pNode = headTradesNode(); pNode != NULL;
			pNode = nextTradesNode(pNode))
		{
			if(isDual(pNode->m_data.m_eItemType) ||
				(pNode->m_data.m_eItemType == TRADE_RESOURCES &&
				!GET_PLAYER(getFirstPlayer()).canTradeNetworkWith(getSecondPlayer())))
			{
				bForce = true;
				break;
			}
		} // </advc.106j>
		if (GET_TEAM(getFirstPlayer()).isHasMet(TEAMID(getSecondPlayer())))
		{
			// advc: Auxiliary function to remove duplicate code
			announceCancel(getFirstPlayer(), getSecondPlayer(), /* advc.106j: */ bForce);
			announceCancel(getSecondPlayer(), getFirstPlayer(), /* advc.106j: */ bForce);
		}
	}
	// <advc.036>
	killSilent(bKillTeam, /* advc.130p: */ true, eCancelPlayer);
}
// advc: Cut from 'kill' above
void CvDeal::announceCancel(PlayerTypes eMsgTarget, PlayerTypes eOther, bool bForce) const
{
	CvWString szString;
	CvWStringBuffer szDealString;
	CvWString szCancelString = gDLL->getText("TXT_KEY_POPUP_DEAL_CANCEL");
	GAMETEXT.getDealString(szDealString, *this, eMsgTarget, /* advc.004w: */ true);
	szString.Format(L"%s: %s", szCancelString.GetCString(), szDealString.getCString());
	gDLL->UI().addMessage(eMsgTarget, /* advc.106j: */ bForce,
			-1, szString,
			bForce ? "AS2D_DEAL_CANCELLED" : NULL, // advc.106j
			// <advc.127b>
			MESSAGE_TYPE_INFO, NULL, NO_COLOR,
			GET_PLAYER(eOther).getCapitalX(eMsgTarget),
			GET_PLAYER(eOther).getCapitalY(eMsgTarget)); // </advc.127b>
}

void CvDeal::killSilent(bool bKillTeam, bool bUpdateAttitude, // </advc.036>
	PlayerTypes eCancelPlayer) // advc.130p
{
	CLLNode<TradeData> const* pNode;
	for (pNode = headFirstTradesNode(); pNode != NULL; pNode = nextFirstTradesNode(pNode))
	{
		endTrade(pNode->m_data, getFirstPlayer(), getSecondPlayer(), bKillTeam,
				bUpdateAttitude, // advc.036
				eCancelPlayer); // advc.130p
	}
	for (pNode = headSecondTradesNode(); pNode != NULL; pNode = nextSecondTradesNode(pNode))
	{
		endTrade(pNode->m_data, getSecondPlayer(), getFirstPlayer(), bKillTeam,
				bUpdateAttitude, // advc.036
				eCancelPlayer); // advc.130p
	}
	GC.getGame().deleteDeal(getID());
}


void CvDeal::addTrades(CLinkList<TradeData> const& kFirstList, CLinkList<TradeData> const& kSecondList, // advc: Take the lists as references
	bool bCheckAllowed)
{
	if (isVassalTrade(kFirstList) && isVassalTrade(kSecondList))
		return;
	// <advc.130p>
	TeamTypes eWarTradeTarget = NO_TEAM;
	TeamTypes ePeaceTradeTarget = NO_TEAM; // </advc.130p>
	bool bPeaceTreaty = false; // advc.ctr

	for (CLLNode<TradeData>* pNode = kFirstList.head(); pNode != NULL;
		pNode = kFirstList.next(pNode))
	{	// <advc.130p>
		if(pNode->m_data.m_eItemType == TRADE_WAR)
			eWarTradeTarget = (TeamTypes)pNode->m_data.m_iData;
		else if(pNode->m_data.m_eItemType == TRADE_PEACE)
			ePeaceTradeTarget = (TeamTypes)pNode->m_data.m_iData;
		// </advc.130p>  <advc.104m> (cf. CvPlayer::buildTradeTable)
		if (pNode->m_data.m_eItemType == TRADE_PEACE_TREATY)
		{
			pNode->m_data.m_bHidden = false;
			bPeaceTreaty = true;
		} // </advc.104m>
		if (bCheckAllowed &&
			!GET_PLAYER(getFirstPlayer()).canTradeItem(getSecondPlayer(), pNode->m_data))
		{
			return;
		}
	}

	for (CLLNode<TradeData>* pNode = kSecondList.head(); pNode != NULL;
		pNode = kSecondList.next(pNode))
	{	// <advc.130p>
		if(pNode->m_data.m_eItemType == TRADE_WAR)
			eWarTradeTarget = (TeamTypes)pNode->m_data.m_iData;
		else if(pNode->m_data.m_eItemType == TRADE_PEACE)
			ePeaceTradeTarget = (TeamTypes)pNode->m_data.m_iData;
		// </advc.130p>  <advc.104m>
		if (pNode->m_data.m_eItemType == TRADE_PEACE_TREATY)
		{
			pNode->m_data.m_bHidden = false;
			bPeaceTreaty = true;
		} // </advc.104m>
		if (bCheckAllowed &&
			!GET_PLAYER(getSecondPlayer()).canTradeItem(getFirstPlayer(), pNode->m_data))
		{
			return;
		}
	}

	TeamTypes eFirstTeam = GET_PLAYER(getFirstPlayer()).getTeam();
	TeamTypes eSecondTeam = GET_PLAYER(getSecondPlayer()).getTeam();
	bool bBumpUnits = false; // K-Mod
	/*  <advc.130p> MakePeace calls moved down. Want to count trade value (partially)
		for peace deals, and I don't think AI_dealValue will work correctly when
		no longer at war. */
	bool const bMakingPeace = ::atWar(eFirstTeam, eSecondTeam);
	bool bUpdateAttitude = false;
	/*  Calls to changePeacetimeTradeValue moved into a new function
		(also for advc.ctr) */
	if (GET_PLAYER(getSecondPlayer()).AI_processTradeValue(kFirstList, getFirstPlayer(),
		kSecondList.getLength() <= 0, bMakingPeace, ePeaceTradeTarget, eWarTradeTarget,
		bPeaceTreaty && !bMakingPeace)) // advc.ctr
	{
		bUpdateAttitude = true;
	}
	if (GET_PLAYER(getFirstPlayer()).AI_processTradeValue(kSecondList, getSecondPlayer(),
		kFirstList.getLength() <= 0, bMakingPeace, ePeaceTradeTarget, eWarTradeTarget,
		bPeaceTreaty && !bMakingPeace)) // advc.ctr
	{
		bUpdateAttitude = true;
	}
	if (bMakingPeace)
	{
		bUpdateAttitude = true; // </advc.130p>
		// free vassals of capitulating team before peace is signed
		/*  advc.130y: Deleted; let CvTeam::setVassal free the vassals
			after signing peace */
		/*if (isVassalTrade(pSecondList))
		{ ... }
		else if (isVassalTrade(pFirstList)) // K-Mod added 'else'
		{ ... }*/

		//GET_TEAM(eFirstTeam).makePeace(eSecondTeam, !isVassalTrade(pFirstList) && !isVassalTrade(pSecondList));
		/*  K-Mod. Bump units only after all trades are completed, because some deals
			(such as city gifts) may affect which units get bumped. (originally,
			units were bumped automatically while executing the peace deal trades)
			Note: the original code didn't bump units for vassal trades. This can
			erroneously allow the vassal's units to stay in the master's land. */
		// <advc.039>
		bool bSurrender = isVassalTrade(kFirstList) || isVassalTrade(kSecondList);
		bool bDone = false;
		if(!bSurrender && GC.getDefineBOOL(CvGlobals::ANNOUNCE_REPARATIONS))
		{
			int iFirstLength = kFirstList.getLength();
			int iSecondLength = kSecondList.getLength();
			// Call makePeace on the recipient of reparations
			if(iFirstLength == 1 && iSecondLength > 1)
			{
				GET_TEAM(eFirstTeam).makePeace(eSecondTeam, false, NO_TEAM,
						false, &kSecondList);
				bDone = true;
			}
			else if(iSecondLength == 1 && iFirstLength > 1)
			{
				GET_TEAM(eSecondTeam).makePeace(eFirstTeam, false, NO_TEAM,
						false, &kFirstList);
				bDone = true;
			}
		}
		if(!bDone) // </advc.039>
		{
		// <advc.034>
			GET_TEAM(eFirstTeam).makePeace(eSecondTeam, false, NO_TEAM,
					bSurrender); // advc.039
		} // </advc.034>
		bBumpUnits = true;
		// K-Mod end
	}
	if(bUpdateAttitude) // advc.opt: Don't update unnecessarily
	{
		// K-Mod
		GET_PLAYER(getFirstPlayer()).AI_updateAttitude(getSecondPlayer());
		GET_PLAYER(getSecondPlayer()).AI_updateAttitude(getFirstPlayer());
		// K-Mod end
	}

	bool bAlliance = false;

	// K-Mod. Vassal deals need to be implemented last, so that master/vassal power is set correctly.
	/*for (CLLNode<TradeData>* pNode = kFirstList.head(); pNode != NULL; pNode = kFirstList.next(pNode)) {
		if (isVassal(pNode->m_data.m_eItemType)) {
			pFirstList->moveToEnd(pNode);
			break;
		}
	}*/ // K-Mod end
	// advc (fixme): This whole loop is mostly duplicated below
	for (int iPass = 0; iPass < 2; iPass++) // advc: Replacing the K-Mod loop above
	{
		for (CLLNode<TradeData> const* pNode = kFirstList.head(); pNode != NULL;
			pNode = kFirstList.next(pNode))
		{	// <advc>
			if (isVassal(pNode->m_data.m_eItemType) == (iPass == 0))
				continue; // </advc>
			// <advc.104> Allow UWAI to record the value of the sponsorship
			if(pNode->m_data.m_eItemType == TRADE_WAR)
			{
				GET_PLAYER(getFirstPlayer()).uwai().getCache().
						reportSponsoredWar(kSecondList, getSecondPlayer(),
						(TeamTypes)pNode->m_data.m_iData);
			} // </advc.104>
			bool bSave = startTrade(pNode->m_data, getFirstPlayer(), getSecondPlayer(),
					bMakingPeace); // advc.ctr
			bBumpUnits = (bBumpUnits || pNode->m_data.m_eItemType == TRADE_PEACE); // K-Mod
			if (bSave)
				insertAtEndFirstTrades(pNode->m_data);
			if (pNode->m_data.m_eItemType == TRADE_PERMANENT_ALLIANCE)
				bAlliance = true;
		}
	}
	for (int iPass = 0; iPass < 2; iPass++) // advc: Replacing deleted K-Mod code; tagging advc.001 b/c I think that code contained a copy-paste error.
	{
		for (CLLNode<TradeData> const* pNode = kSecondList.head(); pNode!= NULL;
			pNode = kSecondList.next(pNode))
		{	// <advc>
			if (isVassal(pNode->m_data.m_eItemType) == (iPass == 0))
				continue; // </advc>
			// <advc.104> As above
			if(pNode->m_data.m_eItemType == TRADE_WAR)
			{
				GET_PLAYER(getSecondPlayer()).uwai().getCache().
						reportSponsoredWar(kFirstList, getFirstPlayer(),
						(TeamTypes)pNode->m_data.m_iData);
			} // </advc.104>
			bool bSave = startTrade(pNode->m_data, getSecondPlayer(), getFirstPlayer(),
					bMakingPeace); // advc.ctr
			bBumpUnits = (bBumpUnits || pNode->m_data.m_eItemType == TRADE_PEACE); // K-Mod

			if (bSave)
				insertAtEndSecondTrades(pNode->m_data);
			if (pNode->m_data.m_eItemType == TRADE_PERMANENT_ALLIANCE)
				bAlliance = true;
		}
	}

	if (bAlliance)
	{
		if (eFirstTeam < eSecondTeam)
			GET_TEAM(eFirstTeam).addTeam(eSecondTeam);
		else if (eSecondTeam < eFirstTeam)
			GET_TEAM(eSecondTeam).addTeam(eFirstTeam);
	}

	// K-Mod
	if (bBumpUnits)
		GC.getMap().verifyUnitValidPlot();
	// K-Mod end
}


void CvDeal::doTurn()
{
	if (!isPeaceDeal()
	/*  <advc.130p> Open Borders and Defensive Pact have very small AI_dealVals.
		In (most?) other places, this doesn't matter b/c the AI never pays for
		these deals, but here it means that OB and DP have practically no impact
		on Grant and Trade values. I'm going to handle all isDual deals entirely
		in the CvPlayerAI::AI_get...Attitude functions and skip them here.
		In multiplayer, dual deals can be mixed with e.g. gold per turn, which is
		why I'm only skipping single-item deals. For mixed multiplayer deals,
		OB and DP will be counted as some value between 0 and 5, which is a bit
		messy, but not really a problem. */
		&& (getLengthSecondTrades() != getLengthFirstTrades() ||
		getLengthSecondTrades() > 1 ||
		!isDual(getFirstTrades()->head()->m_data.m_eItemType)) &&
		/*  The first ten turns of an annual deal are already counted when
			the deal is implemented */
		isCancelable()) // </advc.130p>
	{
		if (getLengthSecondTrades() > 0)
		{
			int iValue = (GET_PLAYER(getFirstPlayer()).AI_dealVal(getSecondPlayer(), *getSecondTrades()) /
					GC.getDefineINT(CvGlobals::PEACE_TREATY_LENGTH));
			if (getLengthFirstTrades() > 0)
				GET_PLAYER(getFirstPlayer()).AI_processPeacetimeTradeValue(getSecondPlayer(), iValue);
			else GET_PLAYER(getFirstPlayer()).AI_processPeacetimeGrantValue(getSecondPlayer(), iValue);
		}

		if (getLengthFirstTrades() > 0)
		{
			int iValue = (GET_PLAYER(getSecondPlayer()).AI_dealVal(getFirstPlayer(), *getFirstTrades()) /
					GC.getDefineINT(CvGlobals::PEACE_TREATY_LENGTH));
			if (getLengthSecondTrades() > 0)
				GET_PLAYER(getSecondPlayer()).AI_processPeacetimeTradeValue(getFirstPlayer(), iValue);
			else GET_PLAYER(getSecondPlayer()).AI_processPeacetimeGrantValue(getFirstPlayer(), iValue);
		}
		// K-Mod note: for balance reasons this function should probably be called at the boundry of some particular player's turn,
		// rather than at the turn boundry of the game itself. -- Unfortunately, the game currently doesn't work like this.
		// Also, note that we do not update attitudes of particular players here, but instead update all of them at the game turn boundry.
	}
}

// XXX probably should have some sort of message for the user or something...
void CvDeal::verify()
{
	// advc: Moved into auxiliary function to get rid of duplicate code
	if (!verify(getFirstPlayer(), getSecondPlayer()) ||
		!verify(getSecondPlayer(), getFirstPlayer()) ||
		(isCancelable(NO_PLAYER) && isPeaceDeal())) // advc.104m
	{
		kill();
	}
}

// advc: Cut from 'verify' above
bool CvDeal::verify(PlayerTypes eRecipient, PlayerTypes eGiver)
{
	CvPlayer& kGiver = GET_PLAYER(eGiver);
	for (CLLNode<TradeData> const* pNode = this->headReceivesNode(eRecipient); pNode != NULL;
		pNode = nextReceivesNode(pNode, eRecipient))
	{
		if (pNode->m_data.m_eItemType == TRADE_RESOURCES)
		{
			BonusTypes eBonus = (BonusTypes) pNode->m_data.m_iData;
			// XXX embargoes?
			if ((kGiver.getNumTradeableBonuses(eBonus) < 0) ||
				!kGiver.canTradeNetworkWith(eRecipient) ||
				GET_TEAM(kGiver.getTeam()).isBonusObsolete(eBonus) ||
				GET_TEAM(eRecipient).isBonusObsolete(eBonus))
			{
				return false;
			}
		}
	}
	return true;
}


bool CvDeal::isPeaceDeal() const  // advc: simplified
{
	for (CLLNode<TradeData> const* pNode = headTradesNode(); pNode != NULL;
		pNode = nextTradesNode(pNode))
	{
		if (pNode->m_data.m_eItemType == getPeaceItem())
			return true;
	}
	return false;
}

bool CvDeal::isVassalDeal() const
{
	return (isVassalTrade(m_firstTrades) || isVassalTrade(m_secondTrades));
}

bool CvDeal::isVassalTrade(CLinkList<TradeData> const& kList)
{
	for (CLLNode<TradeData> const* pNode = kList.head(); pNode != NULL; pNode = kList.next(pNode))
	{
		if (isVassal(pNode->m_data.m_eItemType))
			return true;
	}
	return false;
}

/*	advc.001: Rewrote this function. The BtS code was (even more) convoluted,
	inefficient and contained at least one bug.
	(The first player was passed to setVassalRevoltHelp as eVassal even if the
	first player was on the eMaster team; copy-paste error probably.) */
bool CvDeal::isUncancelableVassalDeal(PlayerTypes eByPlayer, CvWString* pszReason) const
{
	FAssertMsg(involves(eByPlayer), "Caller should have ensured this");
	// Vassal and master (technically) have the same master
	TeamTypes eMaster = GET_PLAYER(getFirstPlayer()).getMasterTeam();
	if (eMaster != GET_PLAYER(getSecondPlayer()).getMasterTeam())
		return false; // There is no vassal deal, so this can't be one.
	TeamTypes eVassal;
	if (GET_TEAM(getFirstPlayer()).isVassal(eMaster))
		eVassal = TEAMID(getFirstPlayer());
	else if(GET_TEAM(getSecondPlayer()).isVassal(eMaster))
		eVassal = TEAMID(getSecondPlayer());
	else return false; // sibling vassals
	if (TEAMID(getFirstPlayer()) == TEAMID(getSecondPlayer()))
		return false; // headGivesNode(TeamTypes) can't handle deals within a team
	for (CLLNode<TradeData> const* pNode = headGivesNode(eVassal); pNode != NULL;
		pNode = nextGivesNode(pNode, eVassal))
	{
		if (!isVassal(pNode->m_data.m_eItemType))
			continue;

		if (TEAMID(eByPlayer) == eMaster) // Master can never cancel
		{
			if (pszReason)
				*pszReason += gDLL->getText("TXT_KEY_MISC_DEAL_NO_CANCEL_EVER");
			return true;
		}
		FAssert(TEAMID(eByPlayer) == eVassal);
		// Voluntary vassal can always cancel
		if (pNode->m_data.m_eItemType != TRADE_SURRENDER)
			return false;
	
		if (!GET_TEAM(eVassal).canVassalRevolt(eMaster))
		{
			if (pszReason)
			{
				CvWStringBuffer szBuffer;
				GAMETEXT.setVassalRevoltHelp(szBuffer, eMaster, eVassal);
				*pszReason = szBuffer.getCString();
			}
			return true;
		}
		return false; // Assume at most one vassal item per deal
	}
	return false;
}


bool CvDeal::isVassalTributeDeal(const CLinkList<TradeData>* pList)
{
	for (CLLNode<TradeData> const* pNode = pList->head(); pNode != NULL;
		pNode = pList->next(pNode))
	{
		if (pNode->m_data.m_eItemType != TRADE_RESOURCES)
			return false;
	}
	return true;
}

/*  <advc> True if one side is the other side's vassal, and the vassal gives
	only resources, and the master gives nothing in return. */
bool CvDeal::isVassalTributeDeal() const
{
	PlayerTypes eVassalPlayer = NO_PLAYER;
	PlayerTypes eMasterPlayer = NO_PLAYER;
	if(GET_TEAM(getFirstPlayer()).isVassal(TEAMID(getSecondPlayer())))
	{
		eVassalPlayer = getFirstPlayer();
		eMasterPlayer = getSecondPlayer();
	}
	else if(GET_TEAM(getSecondPlayer()).isVassal(TEAMID(getFirstPlayer())))
	{
		eVassalPlayer = getSecondPlayer();
		eMasterPlayer = getFirstPlayer();
	}
	return eVassalPlayer != NO_PLAYER && getGivesList(eMasterPlayer).getLength() <= 0 &&
			CvDeal::isVassalTributeDeal(&getGivesList(eVassalPlayer));
} // </advc>

// <advc.034>
bool CvDeal::isDisengage() const
{
	return m_firstTrades.getLength() == 1 && m_firstTrades.getLength() == 1 &&
			m_firstTrades.head()->m_data.m_eItemType == TRADE_DISENGAGE;
} // </advc.034>

// advc.130p: Replaced by code in recordTradeValue
/*bool CvDeal::isPeaceDealBetweenOthers(CLinkList<TradeData>* pFirstList, CLinkList<TradeData>* pSecondList) const {
//... }*/

void CvDeal::setID(int iID)
{
	m_iID = iID;
}


int CvDeal::getInitialGameTurn() const
{
	return m_iInitialGameTurn;
}


void CvDeal::setInitialGameTurn(int iNewValue)
{
	m_iInitialGameTurn = iNewValue;
}

// <advc.133>
int CvDeal::getAge() const
{
	return GC.getGame().getGameTurn() - getInitialGameTurn();
} // </advc.133>

// <advc> Might be important here that getFirstPlayer and getSecondPlayer are inlined
bool CvDeal::isBetween(PlayerTypes ePlayer, PlayerTypes eOtherPlayer) const
{
	return (ePlayer == getFirstPlayer() && eOtherPlayer == getSecondPlayer()) ||
		   (eOtherPlayer == getFirstPlayer() && ePlayer == getSecondPlayer());
}


bool CvDeal::isBetween(TeamTypes eTeam, TeamTypes eOtherTeam) const
{
	return (eTeam == TEAMID(getFirstPlayer()) && eOtherTeam == TEAMID(getSecondPlayer())) ||
		   (eOtherTeam == TEAMID(getFirstPlayer()) && eTeam == TEAMID(getSecondPlayer()));
}


bool CvDeal::isBetween(PlayerTypes ePlayer, TeamTypes eTeam) const
{
	return (ePlayer == getFirstPlayer() && eTeam == TEAMID(getSecondPlayer())) ||
		   (eTeam == TEAMID(getFirstPlayer()) && ePlayer == getSecondPlayer());
}


bool CvDeal::involves(PlayerTypes ePlayer) const
{
	return (getFirstPlayer() == ePlayer || getSecondPlayer() == ePlayer);
}


bool CvDeal::involves(TeamTypes eTeam) const
{
	return (TEAMID(getFirstPlayer()) == eTeam || TEAMID(getSecondPlayer()) == eTeam);
}


PlayerTypes CvDeal::getOtherPlayer(PlayerTypes ePlayer) const
{
	FAssert(ePlayer == getFirstPlayer() || ePlayer == getSecondPlayer());
	return (ePlayer == getFirstPlayer() ? getSecondPlayer() : getFirstPlayer());
}


CLinkList<TradeData> const& CvDeal::getGivesList(PlayerTypes ePlayer) const {

	FAssert(ePlayer == getFirstPlayer() || ePlayer == getSecondPlayer());
	return *(ePlayer == getFirstPlayer() ? getFirstTrades() : getSecondTrades());
}


CLinkList<TradeData> const& CvDeal::getGivesList(TeamTypes eTeam) const
{
	FAssert((TEAMID(getFirstPlayer()) == eTeam) != (TEAMID(getSecondPlayer()) == eTeam));
	return *(TEAMID(getFirstPlayer()) == eTeam ? getFirstTrades() : getSecondTrades());
}


CLinkList<TradeData> const& CvDeal::getReceivesList(PlayerTypes ePlayer) const
{
	FAssert(ePlayer == getFirstPlayer() || ePlayer == getSecondPlayer());
	return *(ePlayer == getFirstPlayer() ? getSecondTrades() : getFirstTrades());
}


CLinkList<TradeData> const& CvDeal::getReceivesList(TeamTypes eTeam) const
{
	FAssert((TEAMID(getFirstPlayer()) == eTeam) != (TEAMID(getSecondPlayer()) == eTeam));
	return *(TEAMID(getFirstPlayer()) == eTeam ? getSecondTrades() : getFirstTrades());
}


CLLNode<TradeData> const* CvDeal::headGivesNode(PlayerTypes ePlayer) const
{
	FAssert(ePlayer == getFirstPlayer() || ePlayer == getSecondPlayer());
	return (ePlayer == getFirstPlayer() ? headFirstTradesNode() : headSecondTradesNode());
}


CLLNode<TradeData> const* CvDeal::headReceivesNode(PlayerTypes ePlayer) const
{
	FAssert(ePlayer == getFirstPlayer() || ePlayer == getSecondPlayer());
	return (ePlayer == getFirstPlayer() ? headSecondTradesNode() : headFirstTradesNode());
}


CLLNode<TradeData> const* CvDeal::headGivesNode(TeamTypes eTeam) const
{
	FAssert((TEAMID(getFirstPlayer()) == eTeam) != (TEAMID(getSecondPlayer()) == eTeam));
	return (TEAMID(getFirstPlayer()) == eTeam ? headFirstTradesNode() : headSecondTradesNode());
}


CLLNode<TradeData> const* CvDeal::headReceivesNode(TeamTypes eTeam) const
{
	FAssert((TEAMID(getFirstPlayer()) == eTeam) != (TEAMID(getSecondPlayer()) == eTeam));
	return (TEAMID(getFirstPlayer()) == eTeam ? headSecondTradesNode() : headFirstTradesNode());
}

/*  No assertions in the next... functions - don't want to slow assert builds down,
	and normally the head function is called first.
	Due to the implementation of CLinkList, all the next functions return the same
	result. So one could actually remove all except nextTradesNode. But I guess it's
	better not to set the CLinkList implementation in stone. */
CLLNode<TradeData> const* CvDeal::nextGivesNode(CLLNode<TradeData> const* pNode,
	PlayerTypes ePlayer) const
{
	return (ePlayer == getFirstPlayer() ? nextFirstTradesNode(pNode) :
			nextSecondTradesNode(pNode));
}


CLLNode<TradeData> const* CvDeal::nextReceivesNode(CLLNode<TradeData> const* pNode,
	PlayerTypes ePlayer) const
{
	return (ePlayer == getFirstPlayer() ? nextSecondTradesNode(pNode) :
			nextFirstTradesNode(pNode));
}


CLLNode<TradeData> const* CvDeal::nextGivesNode(CLLNode<TradeData> const* pNode,
	TeamTypes eTeam) const
{
	return (TEAMID(getFirstPlayer()) == eTeam ? nextFirstTradesNode(pNode) :
			nextSecondTradesNode(pNode));
}


CLLNode<TradeData> const* CvDeal::nextReceivesNode(CLLNode<TradeData> const* pNode,
	TeamTypes eTeam) const
{
	return (TEAMID(getFirstPlayer()) == eTeam ? nextSecondTradesNode(pNode) :
			nextFirstTradesNode(pNode));
}


CLLNode<TradeData> const* CvDeal::headTradesNode() const
{
	return (headFirstTradesNode() == NULL ? headSecondTradesNode() : headFirstTradesNode());
}


CLLNode<TradeData> const* CvDeal::nextTradesNode(CLLNode<TradeData> const* pNode) const
{
	if (pNode == getFirstTrades()->tail())
		return headSecondTradesNode();
	return CLinkList<TradeData>::static_next(pNode);
}
// </advc>

void CvDeal::clearFirstTrades()
{
	m_firstTrades.clear();
}


void CvDeal::insertAtEndFirstTrades(TradeData trade)
{
	m_firstTrades.insertAtEnd(trade);
}


int CvDeal::getLengthFirstTrades() const
{
	return m_firstTrades.getLength();
}


void CvDeal::clearSecondTrades()
{
	m_secondTrades.clear();
}


void CvDeal::insertAtEndSecondTrades(TradeData trade)
{
	m_secondTrades.insertAtEnd(trade);
}


int CvDeal::getLengthSecondTrades() const
{
	return m_secondTrades.getLength();
}


void CvDeal::write(FDataStreamBase* pStream)
{
	PROFILE_FUNC(); // advc
	uint uiFlag=0;
	pStream->Write(uiFlag);
	REPRO_TEST_BEGIN_WRITE(CvString::format("Deal(%d,%d,%d)", getID(), getFirstPlayer(), getSecondPlayer()));
	pStream->Write(m_iID);
	pStream->Write(m_iInitialGameTurn);

	pStream->Write(m_eFirstPlayer);
	pStream->Write(m_eSecondPlayer);

	m_firstTrades.Write(pStream);
	m_secondTrades.Write(pStream);
	REPRO_TEST_END_WRITE();
}

void CvDeal::read(FDataStreamBase* pStream)
{
	uint uiFlag=0;
	pStream->Read(&uiFlag);

	pStream->Read(&m_iID);
	pStream->Read(&m_iInitialGameTurn);

	pStream->Read((int*)&m_eFirstPlayer);
	pStream->Read((int*)&m_eSecondPlayer);

	m_firstTrades.Read(pStream);
	m_secondTrades.Read(pStream);
}

// Returns true if the trade should be saved...
bool CvDeal::startTrade(TradeData trade, PlayerTypes eFromPlayer, PlayerTypes eToPlayer,
	bool bPeace) // advc.ctr
{
	PROFILE_FUNC(); // advc
	bool bSave = false;

	switch (trade.m_eItemType)
	{
	case TRADE_TECHNOLOGIES:
	{
		// <advc.550e> Code moved into subroutine and modified there
		bool bSignificantTech = GET_PLAYER(eToPlayer).isSignificantDiscovery(
				(TechTypes)trade.m_iData); // </advc.550e>
		GET_TEAM(eToPlayer).setHasTech((TechTypes)trade.m_iData, true, eToPlayer, true, true);
		if(bSignificantTech) // advc.550e
			GET_TEAM(eToPlayer).setNoTradeTech((TechTypes)trade.m_iData, true);

		if (gTeamLogLevel >= 2)
		{
			logBBAI("    Player %d (%S) trades tech %S to player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), GC.getInfo((TechTypes)trade.m_iData).getDescription(), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		}

		for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++) // advc.003n: was MAX_PLAYERS
		{
			CvPlayerAI& kThirdParty = GET_PLAYER((PlayerTypes)iI); // advc.003
			if(!kThirdParty.isAlive() || /* advc.550e: */ !bSignificantTech)
				continue;
			// advc.130j (comment): Don't call AI_rememberEvent
			if (kThirdParty.getTeam() == TEAMID(eToPlayer))
				kThirdParty.AI_changeMemoryCount(eFromPlayer, MEMORY_TRADED_TECH_TO_US, 1);
			else // advc.550e: Commented out
			//if (bSignificantTech) // K-Mod
			{
				if (GET_TEAM(kThirdParty.getTeam()).isHasMet(TEAMID(eToPlayer))) 
					kThirdParty.AI_changeMemoryCount(eToPlayer, MEMORY_RECEIVED_TECH_FROM_ANY, 1);
			}
		}
		break;
	}

	case TRADE_RESOURCES:
	{
		GET_PLAYER(eFromPlayer).changeBonusExport((BonusTypes)trade.m_iData, 1);
		GET_PLAYER(eToPlayer).changeBonusImport((BonusTypes)trade.m_iData, 1);
		if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S) trades bonus type %S due to TRADE_RESOURCES with %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), GC.getInfo((BonusTypes)trade.m_iData).getDescription(), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		bSave = true;
		break;
	}
	case TRADE_CITIES:
	{
		CvCity* pCity = GET_PLAYER(eFromPlayer).getCity(trade.m_iData);
		if (pCity != NULL)
		{
			bool bLib = (pCity->getLiberationPlayer() == eToPlayer); // advc.ctr
			if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S) gives a city due to TRADE_CITIES with %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
			pCity->doTask(/* advc.ctr: */ bPeace ? TASK_CEDE :
					TASK_GIFT, eToPlayer);
			// <advc.ctr>
			if (!bPeace && !bLib)
			{
				FAssert(!GET_TEAM(eFromPlayer).isAtWar(TEAMID(eToPlayer)));
				GET_TEAM(eFromPlayer).signPeaceTreaty(TEAMID(eToPlayer));
			} // </advc.ctr>
		}
		break;
	}
	case TRADE_GOLD:
		GET_PLAYER(eFromPlayer).changeGold(-trade.m_iData);
		GET_PLAYER(eToPlayer).changeGold(trade.m_iData);
		GET_PLAYER(eFromPlayer).AI_changeGoldTradedTo(eToPlayer, trade.m_iData);
		if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S) trades gold %d due to TRADE_GOLD with player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), trade.m_iData, eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		CvEventReporter::getInstance().playerGoldTrade(eFromPlayer, eToPlayer, trade.m_iData);
		break;

	case TRADE_GOLD_PER_TURN:
		GET_PLAYER(eFromPlayer).changeGoldPerTurnByPlayer(eToPlayer, -(trade.m_iData));
		GET_PLAYER(eToPlayer).changeGoldPerTurnByPlayer(eFromPlayer, trade.m_iData);
		if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S) trades gold per turn %d due to TRADE_GOLD_PER_TURN with player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), trade.m_iData, eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		bSave = true;
		break;

	case TRADE_MAPS:
	{
		PROFILE("CvDeal::startTrade.MAPS"); // advc
		CvMap const& kMap = GC.getMap();
		for (int iI = 0; iI < kMap.numPlots(); iI++)
		{
			CvPlot& kPlot = kMap.getPlotByIndex(iI);
			if (kPlot.isRevealed(TEAMID(eFromPlayer)))
				kPlot.setRevealed(TEAMID(eToPlayer), true, false, TEAMID(eFromPlayer), false);
		}
		for (MemberIter it(TEAMID(eToPlayer)); it.hasNext(); ++it)
		{
			it->updatePlotGroups();
		}
		if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S) trades maps due to TRADE_MAPS with player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		break;
	}
	case TRADE_SURRENDER:
	case TRADE_VASSAL:
		if (trade.m_iData <= 0) // advc: Was ==. Important b/c I've changed the default value from 0 to -1.
		{
			startTeamTrade(trade.m_eItemType, TEAMID(eFromPlayer), GET_PLAYER(eToPlayer).getTeam(), false);
			GET_TEAM(eFromPlayer).setVassal(TEAMID(eToPlayer), true, TRADE_SURRENDER == trade.m_eItemType);
			if (gTeamLogLevel >= 2)
			{
				if (TRADE_SURRENDER == trade.m_eItemType) logBBAI("    Player %d (%S) trades themselves as vassal due to TRADE_SURRENDER with player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
				else logBBAI("    Player %d (%S) trades themselves as vassal due to TRADE_VASSAL with player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
			}
		}
		else bSave = true;

		break;

	case TRADE_PEACE:
		if (gTeamLogLevel >= 2) logBBAI("    Team %d (%S) makes peace with team %d due to TRADE_PEACE with %d (%S)", TEAMID(eFromPlayer), GET_PLAYER(eFromPlayer).getCivilizationDescription(0), trade.m_iData, eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		//GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).makePeace((TeamTypes)trade.m_iData);
		// K-Mod. (units will be bumped after the rest of the trade deals are completed.)
		// <advc.100b>
		GET_TEAM(eFromPlayer).makePeace((TeamTypes)trade.m_iData, false, TEAMID(eToPlayer));
		GET_TEAM(eFromPlayer).signPeaceTreaty((TeamTypes)trade.m_iData); // K-Mod. Use a standard peace treaty rather than a simple cease-fire.
		// </advc.100b>
		// K-Mod todo: this team should offer something fair to the peace-team if this teams endWarVal is higher.
		break;

	case TRADE_WAR:
		if (gTeamLogLevel >= 2) logBBAI("    Team %d (%S) declares war on team %d due to TRADE_WAR with %d (%S)", TEAMID(eFromPlayer), GET_PLAYER(eFromPlayer).getCivilizationDescription(0), trade.m_iData, eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		GET_TEAM(eFromPlayer).declareWar((TeamTypes)trade.m_iData, true, NO_WARPLAN,
				true, eToPlayer); // advc.100
		// advc.146:
		GET_TEAM(eFromPlayer).signPeaceTreaty(TEAMID(eToPlayer));
		for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
		{
			CvPlayerAI& kAttacked = GET_PLAYER((PlayerTypes)iI); // advc
			if(!kAttacked.isAlive() || kAttacked.getTeam() != (TeamTypes)trade.m_iData)
				continue;
			// advc.130j:
			kAttacked.AI_rememberEvent(eToPlayer, MEMORY_HIRED_WAR_ALLY);
			// <advc.104i> Similar to code in CvTeam::makeUnwillingToTalk
			if(kAttacked.AI_getMemoryCount(eFromPlayer, MEMORY_DECLARED_WAR_RECENT) < 2)
				kAttacked.AI_rememberEvent(eFromPlayer, MEMORY_DECLARED_WAR_RECENT);
			if(::atWar(TEAMID(eToPlayer), kAttacked.getTeam()) &&
					kAttacked. AI_getMemoryCount(eToPlayer, MEMORY_DECLARED_WAR_RECENT) < 2)
				kAttacked.AI_rememberEvent(eToPlayer, MEMORY_DECLARED_WAR_RECENT);
			// </advc.104i>
		}
		break;

	case TRADE_EMBARGO:
	{
		TeamTypes eTargetTeam = (TeamTypes)trade.m_iData;
		GET_PLAYER(eFromPlayer).stopTradingWithTeam(eTargetTeam);
		/*  <advc.130f> The instigator needs to stop too. (If an AI suggested
			the embargo to a human, that's not handled here but by CvPlayer::handleDiploEvent.) */
		if(!GET_TEAM(eFromPlayer).isCapitulated() || !GET_TEAM(eFromPlayer).isVassal(TEAMID(eToPlayer)))
			GET_PLAYER(eToPlayer).stopTradingWithTeam(eTargetTeam, false);
		// </advc.130f>
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayerAI& kTargetMember = GET_PLAYER((PlayerTypes)iI); // advc
			if (!kTargetMember.isAlive())
				continue;
			if (kTargetMember.getTeam() == eTargetTeam)
			{	// advc.130j:
				kTargetMember.AI_rememberEvent(eToPlayer, MEMORY_HIRED_TRADE_EMBARGO);
			}
		}
		if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S) signs embargo against team %d due to TRADE_EMBARGO with player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), (TeamTypes)trade.m_iData, eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		break;
	}
	case TRADE_CIVIC:
	{
		CivicTypes* paeNewCivics = new CivicTypes[GC.getNumCivicOptionInfos()];
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
			paeNewCivics[iI] = GET_PLAYER(eFromPlayer).getCivics((CivicOptionTypes)iI);

		paeNewCivics[GC.getInfo((CivicTypes)trade.m_iData).getCivicOptionType()] = (CivicTypes)trade.m_iData;

		GET_PLAYER(eFromPlayer).revolution(paeNewCivics, true);

		if (GET_PLAYER(eFromPlayer).AI_getCivicTimer() < GC.getDefineINT(CvGlobals::PEACE_TREATY_LENGTH))
			GET_PLAYER(eFromPlayer).AI_setCivicTimer(GC.getDefineINT(CvGlobals::PEACE_TREATY_LENGTH));

		if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S) switched civics due to TRADE_CIVICS with player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));

		SAFE_DELETE_ARRAY(paeNewCivics);
		break;
	}
	case TRADE_RELIGION:
		GET_PLAYER(eFromPlayer).convert((ReligionTypes)trade.m_iData,
				true); // advc.001v
		if (GET_PLAYER(eFromPlayer).AI_getReligionTimer() < GC.getDefineINT(CvGlobals::PEACE_TREATY_LENGTH))
			GET_PLAYER(eFromPlayer).AI_setReligionTimer(GC.getDefineINT(CvGlobals::PEACE_TREATY_LENGTH));
		if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S) switched religions due to TRADE_RELIGION with player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		break;

	case TRADE_OPEN_BORDERS:
		if (trade.m_iData <= 0) // advc: was ==
		{
			startTeamTrade(TRADE_OPEN_BORDERS, TEAMID(eFromPlayer), TEAMID(eToPlayer), true);
			GET_TEAM(eFromPlayer).setOpenBorders(TEAMID(eToPlayer), true);
			if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S_1) signs open borders due to TRADE_OPEN_BORDERS with player %d (%S_2)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		}
		else bSave = true;
		break;

	case TRADE_DEFENSIVE_PACT:
		if (trade.m_iData <= 0) // advc: was ==
		{
			startTeamTrade(TRADE_DEFENSIVE_PACT, TEAMID(eFromPlayer), TEAMID(eToPlayer), true);
			GET_TEAM(eFromPlayer).setDefensivePact(TEAMID(eToPlayer), true);
			if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S) signs defensive pact due to TRADE_DEFENSIVE_PACT with player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		}
		else bSave = true;
		break;

	case TRADE_PERMANENT_ALLIANCE:
		break;

	case TRADE_PEACE_TREATY:  // <advc.032>
		if (GET_TEAM(eFromPlayer).isForcePeace(TEAMID(eToPlayer)))
		{
			if (GET_PLAYER(eFromPlayer).resetPeaceTreaty(eToPlayer))
			{
				if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S) prolongs peace treaty with player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
				break;
			}
		} // </advc.032>
		GET_TEAM(eFromPlayer).setForcePeace(TEAMID(eToPlayer), true);
		if (gTeamLogLevel >= 2) logBBAI("    Player %d (%S) signs peace treaty due to TRADE_PEACE_TREATY with player %d (%S)", eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
		bSave = true;
		break;
	// <advc.034>
	case TRADE_DISENGAGE:
		if(trade.m_iData <= 0)
		{
			startTeamTrade(TRADE_DISENGAGE, TEAMID(eFromPlayer),
					TEAMID(eToPlayer), true);
			GET_TEAM(eFromPlayer).setDisengage(TEAMID(eToPlayer), true);
		}
		else bSave = true;
		break;
	// </advc.034>
	default:
		FAssert(false);
	}

	return bSave;
}


void CvDeal::endTrade(TradeData trade, PlayerTypes eFromPlayer,
	PlayerTypes eToPlayer, bool bTeam, /* advc.036: */ bool bUpdateAttitude,
	PlayerTypes eCancelPlayer) // advc.130p  advc: refactored
{
	bool bTeamTradeEnded = false; // advc.133
	/*	<advc> Skip some steps when a trade ends through the defeat of one party -
		to avoid failed assertions */
	bool bAlive = (GET_PLAYER(eFromPlayer).isAlive() && GET_PLAYER(eToPlayer).isAlive());
	if (!bAlive)
		bUpdateAttitude = false; // </advc>
	switch(trade.m_eItemType)
	{
	case TRADE_RESOURCES:
		GET_PLAYER(eToPlayer).changeBonusImport(((BonusTypes)trade.m_iData), -1);
		GET_PLAYER(eFromPlayer).changeBonusExport(((BonusTypes)trade.m_iData), -1);
		break;

	case TRADE_GOLD_PER_TURN:
		GET_PLAYER(eFromPlayer).changeGoldPerTurnByPlayer(eToPlayer, trade.m_iData);
		GET_PLAYER(eToPlayer).changeGoldPerTurnByPlayer(eFromPlayer, -(trade.m_iData));
		break;

	// <advc.143>
	case TRADE_VASSAL:
	case TRADE_SURRENDER:
	{
		if (!bAlive)
			break;
		bool bSurrender = (trade.m_eItemType == TRADE_SURRENDER);
		// Canceled b/c of failure to protect vassal?
		bool bDeniedHelp = false;
		if(bSurrender)
			bDeniedHelp = (GET_TEAM(eFromPlayer).isLossesAllowRevolt(TEAMID(eToPlayer))
					// Doesn't count if losses obviously only from cultural borders
					&& GET_TEAM(eFromPlayer).AI_isAnyWarPlan());
		else
		{
			DenialTypes eReason = GET_TEAM(eFromPlayer).
					AI_surrenderTrade(TEAMID(eToPlayer));
			bDeniedHelp = (eReason == DENIAL_POWER_YOUR_ENEMIES);
		}
		GET_TEAM(eFromPlayer).setVassal(TEAMID(eToPlayer), false, bSurrender);
		if(bTeam)
		{
			if(bSurrender)
				endTeamTrade(TRADE_SURRENDER, TEAMID(eFromPlayer), TEAMID(eToPlayer));
			else endTeamTrade(TRADE_VASSAL, TEAMID(eFromPlayer), TEAMID(eToPlayer));
			bTeamTradeEnded = true; // advc.133
		}
		addEndTradeMemory(eFromPlayer, eToPlayer, TRADE_VASSAL);
		if(!bDeniedHelp) // Master remembers for 2x10 turns
			addEndTradeMemory(eFromPlayer, eToPlayer, TRADE_VASSAL);
		else /* Vassal remembers for 3x10 turns (and master for 1x10 turns,
				which could matter when a human becomes a vassal) */
		{
			for(int i = 0; i < 3; i++)
				addEndTradeMemory(eToPlayer, eFromPlayer, TRADE_VASSAL);
		}
		break;
	} // </advc.143>
	case TRADE_OPEN_BORDERS:
		GET_TEAM(eFromPlayer).setOpenBorders(TEAMID(eToPlayer), false);
		if(bTeam)
		{
			endTeamTrade(TRADE_OPEN_BORDERS, TEAMID(eFromPlayer), TEAMID(eToPlayer));
			bTeamTradeEnded = true; // advc.133
			// <advc.130p>
			/*  Don't check this after all -- if the other side loses all visible
				territory and then regains some, they may still not really be worth
				trading with. */
			/*if(eCancelPlayer == NULL || GET_TEAM(eCancelPlayer).AI_openBordersTrade(
					TEAMID(eCancelPlayer == eToPlayer ? eFromPlayer : eToPlayer)) !=
					DENIAL_NO_GAIN)*/
			if (bAlive)
				addEndTradeMemory(eFromPlayer, eToPlayer, TRADE_OPEN_BORDERS);
			/* (This class really shouldn't be adding cancellation
				memory as this is an AI thing; but BtS does it that way too.) */
			// </advc.130p>
		}
		break;

	case TRADE_DEFENSIVE_PACT:
		GET_TEAM(eFromPlayer).setDefensivePact(TEAMID(eToPlayer), false);
		if(bTeam)
		{
			endTeamTrade(TRADE_DEFENSIVE_PACT, TEAMID(eFromPlayer), TEAMID(eToPlayer));
			bTeamTradeEnded = true; // advc.133
			if (bAlive)  // advc.130p:
				addEndTradeMemory(eFromPlayer, eToPlayer, TRADE_DEFENSIVE_PACT);
		}
		break;

	case TRADE_PEACE_TREATY:
		GET_TEAM(eFromPlayer).setForcePeace(TEAMID(eToPlayer), false);
		break;
	// <advc.034>
	case TRADE_DISENGAGE:
		GET_TEAM(eFromPlayer).setDisengage(TEAMID(eToPlayer), false);
		if(bTeam)
			endTeamTrade(TRADE_DISENGAGE, TEAMID(eFromPlayer), TEAMID(eToPlayer));
		return; // No need to update attitude
	// </advc.034>
	default: FAssert(false);
	} // <advc.036>
	if (!bUpdateAttitude)
		return; // </advc.036>
	// <advc.133> (I think this is needed even w/o change 133 canceling more deals)
	if(!bTeamTradeEnded)
		GET_PLAYER(eFromPlayer).AI_updateAttitude(eToPlayer);
	else GET_TEAM(eFromPlayer).AI_updateAttitude(TEAMID(eToPlayer));
	// </advc.133>
}

// <advc.130p> Cut-and-pasted from CvDeal::endTrade
void CvDeal::addEndTradeMemory(PlayerTypes eFromPlayer, PlayerTypes eToPlayer,
		TradeableItems eItemType)
{
	for(int i = 0; i < MAX_CIV_PLAYERS; i++)
	{
		PlayerTypes eToTeamMember = (PlayerTypes)i;
		if(!GET_PLAYER(eToTeamMember).isAlive() ||
				TEAMID(eToTeamMember) != TEAMID(eToPlayer))
			continue;
		for(int j = 0; j < MAX_CIV_PLAYERS; j++)
		{
			PlayerTypes eFromTeamMember = (PlayerTypes)j;
			if(!GET_PLAYER(eFromTeamMember).isAlive() ||
					TEAMID(eFromTeamMember) != TEAMID(eFromPlayer))
				continue;
			MemoryTypes eMemory = MEMORY_CANCELLED_OPEN_BORDERS;
			if(eItemType == TRADE_DEFENSIVE_PACT)
				eMemory = MEMORY_CANCELLED_DEFENSIVE_PACT;
			else if(eItemType == TRADE_VASSAL)
				eMemory = MEMORY_CANCELLED_VASSAL_AGREEMENT;
			// advc.130j:
			GET_PLAYER(eToTeamMember).AI_setMemoryCount(eFromTeamMember, eMemory, 2);
		}
	}
} // </advc.130p>


void CvDeal::startTeamTrade(TradeableItems eItem, TeamTypes eFromTeam, TeamTypes eToTeam, bool bDual)
{
	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++) // advc.003n: was MAX_PLAYERS
	{
		CvPlayer& kFromMember = GET_PLAYER((PlayerTypes)iI); // advc
		if (!kFromMember.isAlive() || kFromMember.getTeam() != eFromTeam)
			continue;

		for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
		{
			CvPlayer& kToMember = GET_PLAYER((PlayerTypes)iJ); // advc
			if (!kToMember.isAlive() || kToMember.getTeam() != eToTeam)
				continue;

			TradeData item(eItem, 1);
			CLinkList<TradeData> ourList;
			ourList.insertAtEnd(item);
			CLinkList<TradeData> theirList;
			if (bDual)
				theirList.insertAtEnd(item);
			GC.getGame().implementDeal(kFromMember.getID(), kToMember.getID(),
					ourList, theirList);
		}
	}
}

void CvDeal::endTeamTrade(TradeableItems eItem, TeamTypes eFromTeam, TeamTypes eToTeam)  // advc: style changes
{
	CLLNode<TradeData> const* pNode = NULL;
	FOR_EACH_DEAL_VAR(pLoopDeal)
	{
		if (pLoopDeal == this)
			continue;

		bool bValid = true;
		if (TEAMID(pLoopDeal->getFirstPlayer()) == eFromTeam &&
			TEAMID(pLoopDeal->getSecondPlayer()) == eToTeam)
		{
			if (pLoopDeal->getFirstTrades() != NULL)
			{
				for (pNode = pLoopDeal->getFirstTrades()->head(); pNode != NULL; pNode = pLoopDeal->getFirstTrades()->next(pNode))
				{
					if (pNode->m_data.m_eItemType == eItem)
						bValid = false;
				}
			}
		}

		if (bValid && TEAMID(pLoopDeal->getFirstPlayer()) == eToTeam &&
				TEAMID(pLoopDeal->getSecondPlayer()) == eFromTeam)
		{
			if (pLoopDeal->getSecondTrades() != NULL)
			{
				for (pNode = pLoopDeal->getSecondTrades()->head(); pNode != NULL; pNode = pLoopDeal->getSecondTrades()->next(pNode))
				{
					if (pNode->m_data.m_eItemType == eItem)
						bValid = false;
				}
			}
		}

		if (!bValid)
			pLoopDeal->kill(false);
	}
}

bool CvDeal::isCancelable(PlayerTypes eByPlayer, CvWString* pszReason) const
{
	if (eByPlayer != NO_PLAYER)
	{
		// <advc.001> Not really a bug, but you'd really expect this function to check this.
		if (!involves(eByPlayer))
			return false; // </advc.001>
		if (isUncancelableVassalDeal(eByPlayer, pszReason))
			return false;
	}
	int iTurns = turnsToCancel(eByPlayer);
	if (pszReason != NULL && iTurns > 0)
		*pszReason = gDLL->getText("TXT_KEY_MISC_DEAL_NO_CANCEL", iTurns);
	return (iTurns <= 0);
}

/*  <advc.130f> Based on isCancelable; doesn't check turnsToCancel.
	See declaration in CvDeal.h. */
bool CvDeal::isEverCancelable(PlayerTypes eByPlayer) const 
{
	if (!involves(eByPlayer))
		return false;

	return !isUncancelableVassalDeal(eByPlayer);
} // </advc.130f>

int CvDeal::turnsToCancel(PlayerTypes eByPlayer) const
{	// <advc.034>
	int len = GC.getDefineINT(CvGlobals::PEACE_TREATY_LENGTH);
	if(isDisengage())
		len = std::min(GC.getDefineINT(CvGlobals::DISENGAGE_LENGTH), len);
	return (getInitialGameTurn() + len - // </advc.034>
			GC.getGame().getGameTurn());
}
// <advc>
bool CvDeal::isAllDual() const
{
	CLLNode<TradeData> const* pNode;
	for(pNode = headTradesNode(); pNode != NULL; pNode = nextTradesNode(pNode))
	{
		if(!CvDeal::isDual(pNode->m_data.m_eItemType))
			return false;
	}
	return true;
} // </advc>

// static
bool CvDeal::isAnnual(TradeableItems eItem)
{
	switch (eItem)
	{
	case TRADE_RESOURCES:
	case TRADE_GOLD_PER_TURN:
	case TRADE_VASSAL:
	case TRADE_SURRENDER:
	case TRADE_OPEN_BORDERS:
	case TRADE_DISENGAGE: // advc.034
	case TRADE_DEFENSIVE_PACT:
	case TRADE_PERMANENT_ALLIANCE:
		return true;
		break;
	}
	return false;
}

// static
bool CvDeal::isDual(TradeableItems eItem, bool bExcludePeace)
{
	switch (eItem)
	{
	case TRADE_OPEN_BORDERS:
	case TRADE_DISENGAGE: // advc.034
	case TRADE_DEFENSIVE_PACT:
	case TRADE_PERMANENT_ALLIANCE:
		return true;
	case TRADE_PEACE_TREATY:
		return (!bExcludePeace);
	}
	return false;
}

// static
bool CvDeal::hasData(TradeableItems eItem)
{
	return (eItem != TRADE_MAPS &&
		eItem != TRADE_VASSAL &&
		eItem != TRADE_SURRENDER &&
		eItem != TRADE_OPEN_BORDERS &&
		eItem != TRADE_DISENGAGE && // advc.034
		eItem != TRADE_DEFENSIVE_PACT &&
		eItem != TRADE_PERMANENT_ALLIANCE &&
		eItem != TRADE_PEACE_TREATY);
}

// static
bool CvDeal::isEndWar(TradeableItems eItem)
{
	if (eItem == getPeaceItem())
		return true;

	if (isVassal(eItem))
		return true;

	return false;
}
