// buttonPopup.cpp

#include "CvGameCoreDLL.h"
#include "CvDLLButtonPopup.h"
#include "CvPopupInfo.h"
#include "CoreAI.h"
#include "CvCityAI.h"
#include "CvUnit.h"
#include "CvSelectionGroup.h"
#include "CvMap.h"
#include "CvArea.h"
#include "RiseFall.h" // advc.706
#include "CvPopupReturn.h"
#include "CvInfo_All.h"
#include "CvGameTextMgr.h"
#include "CvMessageControl.h"


#define PASSWORD_DEFAULT (L"*****")

CvDLLButtonPopup* CvDLLButtonPopup::m_pInst = NULL;

// K-Mod. In the original code, arbitrary integers are used to express the options of the in-game main menu.
// I found it confusing, and I know that such use of 'magic numbers' is prone to mistakes - especially since I intended to add a new option into the middle...
// I've created this enum set to make sure there is no confusion as to what each item number is.
static enum MainMenuOptions
{
	MM_EXIT_TO_DESKTOP = 0,
	MM_EXIT_TO_MAIN_MENU,
	MM_RETIRE,
	MM_REGENERATE_MAP,
	MM_LOAD_GAME,
	MM_SAVE_GAME,
	MM_OPTIONS,
	MM_BUG_OPTIONS, // new in K-Mod
	MM_ENTER_WB,
	MM_GAME_DETAILS,
	MM_PLAYER_DETAILS,
	MM_CANCEL,
};
// K-Mod end

CvDLLButtonPopup& CvDLLButtonPopup::getInstance()
{
	if (m_pInst == NULL)
		m_pInst = new CvDLLButtonPopup;
	return *m_pInst;
}


void CvDLLButtonPopup::freeInstance()
{
	SAFE_DELETE(m_pInst);
}


CvDLLButtonPopup::CvDLLButtonPopup() : m_kUI(gDLL->UI()) {} // advc


CvDLLButtonPopup::~CvDLLButtonPopup() {}


void CvDLLButtonPopup::OnAltExecute(CvPopup& popup, const PopupReturn& popupReturn, CvPopupInfo &info)
{
	CvPopupInfo* pInfo = new CvPopupInfo;
	if (pInfo != NULL)
	{
		*pInfo = info;
		m_kUI.addPopup(pInfo);
		m_kUI.popupSetAsCancelled(&popup);
	}
}


void CvDLLButtonPopup::OnOkClicked(CvPopup* pPopup, PopupReturn *pPopupReturn, CvPopupInfo &info)
{
	CvGame& g = GC.getGame();
	FAssert(g.getActivePlayer() != NO_PLAYER); // K-Mod

	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_TEXT:
		break;

	case BUTTONPOPUP_CONFIRM_MENU:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			switch (info.getData1())
			{
			case 0:
				gDLL->SetDone(true);
				break;
			case 1:
				m_kUI.exitingToMainMenu();
				break;
			case 2:
				//g.doControl(CONTROL_RETIRE);
				g.retire(); // K-Mod
				break;
			case 3:
				g.regenerateMap();
				break;
			case 4:
				//g.doControl(CONTROL_WORLD_BUILDER);
				g.enterWorldBuilder(); // K-Mod
				break;
			}
		}
		break;

	case BUTTONPOPUP_MAIN_MENU:
		// K-Mod. The following section use to be a long chain of "else if" statements.
		// It was making me sad, so I rewrote it as a switch. (and also inserted a BUG options item.)
		switch (pPopupReturn->getButtonClicked())
		{
		case MM_EXIT_TO_DESKTOP:
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
				if (NULL != pInfo)
				{
					pInfo->setData1(0);
					m_kUI.addPopup(pInfo, g.getActivePlayer(), true);
				}
			}
			break;
		case MM_EXIT_TO_MAIN_MENU:
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
				if (NULL != pInfo)
				{
					pInfo->setData1(1);
					m_kUI.addPopup(pInfo, g.getActivePlayer(), true);
				}
			}
			break;
		case MM_RETIRE:
			/*{ CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			if (NULL != pInfo) {
				pInfo->setData1(2);
				m_kUI.addPopup(pInfo, g.getActivePlayer(), true);
			} }*/ // BtS
			g.doControl(CONTROL_RETIRE); // K-Mod
			break;
		case MM_REGENERATE_MAP:
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
				if (NULL != pInfo)
				{
					pInfo->setData1(3);
					m_kUI.addPopup(pInfo, g.getActivePlayer(), true);
				}
			}
			break;
		case MM_LOAD_GAME:
			g.doControl(CONTROL_LOAD_GAME);
			break;
		case MM_SAVE_GAME:
			g.doControl(CONTROL_SAVE_NORMAL);
			break;
		case MM_OPTIONS:
			GC.getPythonCaller()->showPythonScreen("OptionsScreen");
			break;
		case MM_BUG_OPTIONS:
			GC.getPythonCaller()->showPythonScreen("BugOptionsScreen");
			break;
		case MM_ENTER_WB:
			/*{ CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
				if (NULL != pInfo) {
					pInfo->setData1(4);
					m_kUI.addPopup(pInfo, g.getActivePlayer(), true);
			} }*/ // BtS
			g.doControl(CONTROL_WORLD_BUILDER); // K-Mod
			break;
		case MM_GAME_DETAILS:
			g.doControl(CONTROL_ADMIN_DETAILS);
			break;
		case MM_PLAYER_DETAILS:
			g.doControl(CONTROL_DETAILS);
			break;
		default: // cancel
			break;
		}
		// K-Mod end
		break;

	case BUTTONPOPUP_DECLAREWARMOVE:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			CvMessageControl::getInstance().sendChangeWar((TeamTypes)info.getData1(), true);
		}
		if (((pPopupReturn->getButtonClicked() == 0) || info.getOption2()) && info.getFlags() == 0)
		{	// <advc.035>
			CvUnit* pUnit = m_kUI.getHeadSelectedUnit();
			CvPlot* pAt = (pUnit == NULL ? NULL : pUnit->plot());
			// Don't move ahead if the tile we're on is going to flip
			if(pAt != NULL && (!pAt->isOwned() ||
					(GET_TEAM(pAt->getSecondOwner()).getMasterTeam() !=
					GET_TEAM((TeamTypes)info.getData1()).getMasterTeam() &&
					!GET_TEAM(GET_TEAM(pAt->getSecondOwner()).getMasterTeam()).
					isDefensivePact(GET_TEAM((TeamTypes)info.getData1()).
					getMasterTeam())))) // </advc.035>
			{
				//g.selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_MOVE_TO, info.getData2(), info.getData3(), info.getFlags(), false, info.getOption1());
				g.selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_MOVE_TO, info.getData2(), info.getData3(), info.getFlags() | MOVE_DECLARE_WAR, false, info.getOption1()); // K-Mod
				// (See comments in CvGame::selectionListGameNetMessage for an explanation for the MOVE_DECLARE_WAR flag. Basically, it's a kludge.)
			}
		}
		break;

	case BUTTONPOPUP_CONFIRMCOMMAND:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			int iAction = info.getData1();
			g.selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, GC.getActionInfo(iAction).getCommandType(), GC.getActionInfo(iAction).getCommandData(), -1, 0, info.getOption1());
		}
		break;

	case BUTTONPOPUP_LOADUNIT:
		if (pPopupReturn->getButtonClicked() != 0)
		{
			CvSelectionGroup* pSelectionGroup = m_kUI.getSelectionList();
			if (pSelectionGroup == NULL)
				break; // advc

			CvPlot* pPlot = pSelectionGroup->plot();
			int iCount = pPopupReturn->getButtonClicked();

			CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
			while (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if (pSelectionGroup->canDoCommand(COMMAND_LOAD_UNIT, pLoopUnit->getOwner(), pLoopUnit->getID()))
				{
					iCount--;
					if (iCount == 0)
					{
						g.selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, COMMAND_LOAD_UNIT, pLoopUnit->getOwner(), pLoopUnit->getID());
						break;
					}
				}
			}
		}
		break;

	case BUTTONPOPUP_LEADUNIT:
	{
		if (pPopupReturn->getButtonClicked() == 0)
			break; // advc

		CvSelectionGroup* pSelectionGroup = m_kUI.getSelectionList();
		if (NULL == pSelectionGroup)
			break; // advc

		CvPlot* pPlot = pSelectionGroup->plot();
		int iCount = pPopupReturn->getButtonClicked();

		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pPlot->nextUnitNode(pUnitNode);

			if (pLoopUnit->canPromote((PromotionTypes) info.getData1(), info.getData2()))
			{
				iCount--;
				if (iCount == 0)
				{
					g.selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_LEAD, pLoopUnit->getID());
					break;
				}
			}
		}
		break;
		}

	case BUTTONPOPUP_DOESPIONAGE:
		if (pPopupReturn->getButtonClicked() != NO_ESPIONAGEMISSION)
		{
			g.selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_ESPIONAGE, (EspionageMissionTypes) pPopupReturn->getButtonClicked());
		}
		break;

	case BUTTONPOPUP_DOESPIONAGE_TARGET:
		if (pPopupReturn->getButtonClicked() != -1)
		{
			g.selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_ESPIONAGE, (EspionageMissionTypes)info.getData1(), pPopupReturn->getButtonClicked());
		}
		break;

	case BUTTONPOPUP_CHOOSETECH:
		if (pPopupReturn->getButtonClicked() == GC.getNumTechInfos())
		{
			GC.getPythonCaller()->showPythonScreen("TechChooser");
			GET_PLAYER(g.getActivePlayer()).chooseTech(0, "", true);
		}
		break;

	case BUTTONPOPUP_RAZECITY:  // advc: Refactored this case a bit
	{
		CvCity* pCity = GET_PLAYER(g.getActivePlayer()).getCity(info.getData1());
		if (pCity == NULL)
		{
			FAssert(pCity != NULL);
			return;
		}
		switch(pPopupReturn->getButtonClicked())
		{
		case 0:
			pCity->chooseProduction();
			CvEventReporter::getInstance().cityAcquiredAndKept(g.getActivePlayer(), pCity);
			break;
		case 1:
			CvMessageControl::getInstance().sendDoTask(info.getData1(), TASK_RAZE,
					-1, -1, false, false, false, false);
			break;
		case 2:
			CvEventReporter::getInstance().cityAcquiredAndKept(g.getActivePlayer(), pCity);
			CvMessageControl::getInstance().sendDoTask(info.getData1(), //TASK_GIFT
					/*  advc.ctr: TASK_GIFT doesn't call CvCity::liberate if the
						bConquest=true liberation player differs from the normal
						liberation player */
					TASK_LIBERATE,
					info.getData2(), -1, false, false, false, false);
			break;
		case 3:
			m_kUI.selectCity(pCity, false);
			m_kUI.addPopup(new CvPopupInfo(BUTTONPOPUP_RAZECITY,
					info.getData1(), info.getData2(), info.getData3()),
					g.getActivePlayer(), false, true);
			break;
		default: FAssertMsg(false, "Clicked button not recognized");
		}
		break;
	}
	case BUTTONPOPUP_DISBANDCITY:
		if (pPopupReturn->getButtonClicked() == 1)
		{
			CvMessageControl::getInstance().sendDoTask(info.getData1(), TASK_DISBAND, -1, -1, false, false, false, false);
		}
		else if (pPopupReturn->getButtonClicked() == 0)
		{
			CvCity* pCity = GET_PLAYER(g.getActivePlayer()).getCity(info.getData1());
			if (NULL != pCity)
			{
				pCity->chooseProduction();
				CvEventReporter::getInstance().cityAcquiredAndKept(g.getActivePlayer(), pCity);
			}
		}
		break;

	case BUTTONPOPUP_CHOOSEPRODUCTION: {
		int iExamineCityID = 0;
		iExamineCityID = std::max(iExamineCityID, GC.getNumUnitInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumBuildingInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumProjectInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumProcessInfos());

		if (pPopupReturn->getButtonClicked() == iExamineCityID)
		{
			CvCity* pCity = GET_PLAYER(g.getActivePlayer()).getCity(info.getData1());
			if (pCity != NULL)
				m_kUI.selectCity(pCity, true);
		}
		break;
	}
	case BUTTONPOPUP_CHANGECIVIC:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			std::vector<CivicTypes> aeNewCivics(GC.getNumCivicOptionInfos());
			for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
			{
				aeNewCivics[iI] = GET_PLAYER(g.getActivePlayer()).getCivics((CivicOptionTypes)iI);
			}
			aeNewCivics[info.getData1()] = ((CivicTypes)(info.getData2()));

			CvMessageControl::getInstance().sendUpdateCivics(aeNewCivics);
		}
		else if (pPopupReturn->getButtonClicked() == 2)
			GC.getPythonCaller()->showPythonScreen("CivicsScreen");
		break;

	case BUTTONPOPUP_CHANGERELIGION:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			CvMessageControl::getInstance().sendConvert((ReligionTypes)(info.getData1()));
		}
		break;

	case BUTTONPOPUP_CHOOSEELECTION:
		{
			VoteSelectionData* pData = g.getVoteSelection(info.getData1());
			if (NULL != pData && pPopupReturn->getButtonClicked() < (int)pData->aVoteOptions.size())
			{
				CvMessageControl::getInstance().sendChooseElection((VoteTypes)(pPopupReturn->getButtonClicked()), info.getData1());
			}
		}
		break;

	case BUTTONPOPUP_DIPLOVOTE:
		CvMessageControl::getInstance().sendDiploVote(info.getData1(), (PlayerVoteTypes)pPopupReturn->getButtonClicked());
		break;

	case BUTTONPOPUP_ALARM:
		break;

	case BUTTONPOPUP_DEAL_CANCELED:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			gDLL->sendKillDeal(info.getData1(), info.getOption1());
		}
		break;

	case BUTTONPOPUP_PYTHON:
		GC.getPythonCaller()->onOKClicked(info, pPopupReturn->getButtonClicked());
		break;

	case BUTTONPOPUP_DETAILS:
		{
			CvInitCore& ic = GC.getInitCore();
			// Civ details
			PlayerTypes eID = ic.getActivePlayer();
			CvWString szLeaderName = ic.getLeaderName(eID);
			CvWString szCivDescription = ic.getCivDescription(eID);
			CvWString szCivShortDesc = ic.getCivShortDesc(eID);
			CvWString szCivAdjective = ic.getCivAdjective(eID);
			CvWString szCivPassword = PASSWORD_DEFAULT;
			CvString szEmail = ic.getEmail(eID);
			CvString szSmtpHost = ic.getSmtpHost(eID);

			if (pPopupReturn->getEditBoxString(0) && *(pPopupReturn->getEditBoxString(0)))
			{
				szLeaderName = pPopupReturn->getEditBoxString(0);
			}
			if (pPopupReturn->getEditBoxString(1) && *(pPopupReturn->getEditBoxString(1)))
			{
				szCivDescription = pPopupReturn->getEditBoxString(1);
			}
			if (pPopupReturn->getEditBoxString(2) && *(pPopupReturn->getEditBoxString(2)))
			{
				szCivShortDesc = pPopupReturn->getEditBoxString(2);
			}
			if (pPopupReturn->getEditBoxString(3) && *(pPopupReturn->getEditBoxString(3)))
			{
				szCivAdjective = pPopupReturn->getEditBoxString(3);
			}
			if (g.isHotSeat() || g.isPbem())
			{
				if (pPopupReturn->getEditBoxString(4) && *(pPopupReturn->getEditBoxString(4)))
				{
					szCivPassword = pPopupReturn->getEditBoxString(4);
				}
			}
			if (g.isPitboss() || g.isPbem())
			{
				if (pPopupReturn->getEditBoxString(5) && *(pPopupReturn->getEditBoxString(5)))
				{
					szEmail = CvString(pPopupReturn->getEditBoxString(5));
				}
			}
			if (g.isPbem())
			{
				if (pPopupReturn->getEditBoxString(6) && *(pPopupReturn->getEditBoxString(6)))
				{
					szSmtpHost = CvString(pPopupReturn->getEditBoxString(6));
				}
			}

			ic.setLeaderName(eID, szLeaderName);
			ic.setCivDescription(eID, szCivDescription);
			ic.setCivShortDesc(eID, szCivShortDesc);
			ic.setCivAdjective(eID, szCivAdjective);
			if (szCivPassword != PASSWORD_DEFAULT)
			{
				ic.setCivPassword(eID, szCivPassword);
			}
			ic.setEmail(eID, szEmail);
			ic.setSmtpHost(eID, szSmtpHost);
			gDLL->sendPlayerInfo(eID);

			if (g.isPbem() && pPopupReturn->getButtonClicked() == 0)
				gDLL->sendPbemTurn(NO_PLAYER);
		}
		break;

	case BUTTONPOPUP_ADMIN:
	{
		// Game details
		CvWString szGameName;
		CvWString szAdminPassword = GC.getInitCore().getAdminPassword();
		if (pPopupReturn->getEditBoxString(0) && *(pPopupReturn->getEditBoxString(0)))
			szGameName = pPopupReturn->getEditBoxString(0);
		if (pPopupReturn->getEditBoxString(1) &&
			CvWString(pPopupReturn->getEditBoxString(1)) != PASSWORD_DEFAULT)
		{
			if (*(pPopupReturn->getEditBoxString(1)))
			{
				szAdminPassword = CvWString(gDLL->md5String((char*)
						CvString(pPopupReturn->getEditBoxString(1)).GetCString()));
			}
			else szAdminPassword = L"";
		}
		if (!g.isGameMultiPlayer())
		{
			if (pPopupReturn->getCheckboxBitfield(2)
				//advc: Redundant
				/*&& pPopupReturn->getCheckboxBitfield(2) > 0*/)
			{
				gDLL->setChtLvl(1);
			}
			else gDLL->setChtLvl(0);
		}
		gDLL->sendGameInfo(szGameName, szAdminPassword);
		break;
	}

	case BUTTONPOPUP_ADMIN_PASSWORD:
	{
		CvWString szAdminPassword;
		if (pPopupReturn->getEditBoxString(0) &&
			CvWString(pPopupReturn->getEditBoxString(0)) != PASSWORD_DEFAULT)
		{
			szAdminPassword = pPopupReturn->getEditBoxString(0);
		}
		if (CvWString(gDLL->md5String((char*)CvString(szAdminPassword).GetCString())) ==
			GC.getInitCore().getAdminPassword())
		{
			switch ((ControlTypes)info.getData1())
			{
			case CONTROL_WORLD_BUILDER:
				m_kUI.setWorldBuilder(!gDLL->GetWorldBuilderMode());
				break;
			case CONTROL_ADMIN_DETAILS:
				m_kUI.showAdminDetails();
				break;
			}
		}
		else
		{
			CvPopupInfo* pInfo = new CvPopupInfo();
			if (NULL != pInfo)
			{
				pInfo->setText(gDLL->getText("TXT_KEY_BAD_PASSWORD_DESC"));
				m_kUI.addPopup(pInfo, NO_PLAYER, true);
			}
		}
		break;
	}

	case BUTTONPOPUP_EXTENDED_GAME:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			if (g.isNetworkMultiPlayer())
				CvMessageControl::getInstance().sendExtendedGame();
			else g.setGameState(GAMESTATE_EXTENDED);
		}
		else if (pPopupReturn->getButtonClicked() == 1)
		{
			// exit to main menu
			if (g.isNetworkMultiPlayer() && g.canDoControl(CONTROL_RETIRE) &&
				g.countHumanPlayersAlive() > 1)
			{
				//g.doControl(CONTROL_RETIRE);
				g.retire(); // K-Mod
			}
			else if (!m_kUI.isDebugMenuCreated())
				m_kUI.exitingToMainMenu();
			else gDLL->SetDone(true);
		}
		break;

	case BUTTONPOPUP_DIPLOMACY:
		if (pPopupReturn->getButtonClicked() != MAX_CIV_PLAYERS)
		{
			GET_PLAYER(g.getActivePlayer()).contact(
					(PlayerTypes)pPopupReturn->getButtonClicked());
		}
		break;

	case BUTTONPOPUP_ADDBUDDY:
		if (pPopupReturn->getButtonClicked() == 0)
			gDLL->AcceptBuddy(CvString(info.getText()).GetCString(), info.getData1());
		else gDLL->RejectBuddy(CvString(info.getText()).GetCString(), info.getData1());
		break;

	case BUTTONPOPUP_FORCED_DISCONNECT:
	case BUTTONPOPUP_PITBOSS_DISCONNECT:
	case BUTTONPOPUP_KICKED:
		m_kUI.exitingToMainMenu();
		break;

	case BUTTONPOPUP_VASSAL_DEMAND_TRIBUTE:
		if (pPopupReturn->getButtonClicked() < GC.getNumBonusInfos())
		{
			PlayerTypes eVassal = (PlayerTypes)info.getData1();
			if (GET_PLAYER(eVassal).isHuman())
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_VASSAL_GRANT_TRIBUTE,
						g.getActivePlayer(), pPopupReturn->getButtonClicked());
				if (NULL != pInfo)
					gDLL->sendPopup(eVassal, pInfo);
			}
			else
			{
				CLinkList<TradeData> ourList;
				CLinkList<TradeData> theirList;
				theirList.insertAtEnd(TradeData(
						TRADE_RESOURCES, pPopupReturn->getButtonClicked()));
				if (GET_PLAYER(eVassal).AI_considerOffer(g.getActivePlayer(), ourList, theirList))
				{
					gDLL->sendImplementDealMessage(eVassal, &ourList, &theirList);
					CvWString szBuffer = gDLL->getText("TXT_KEY_VASSAL_GRANT_TRIBUTE_ACCEPTED",
							GET_PLAYER(eVassal).getNameKey(),
							GET_PLAYER(g.getActivePlayer()).getNameKey(),
							GC.getInfo((BonusTypes)pPopupReturn->getButtonClicked()).getTextKeyWide());
					m_kUI.addMessage(g.getActivePlayer(), false, -1, szBuffer);
				}
				else CvMessageControl::getInstance().sendChangeWar(TEAMID(eVassal), true);
			}
		}
		break;

	case BUTTONPOPUP_VASSAL_GRANT_TRIBUTE:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			TradeData item;
			setTradeItem(&item, TRADE_RESOURCES, info.getData2());

			CLinkList<TradeData> ourList;
			CLinkList<TradeData> theirList;
			ourList.insertAtEnd(item);

			gDLL->sendImplementDealMessage((PlayerTypes)info.getData1(), &ourList, &theirList);

			CvWString szBuffer = gDLL->getText("TXT_KEY_VASSAL_GRANT_TRIBUTE_ACCEPTED",
					GET_PLAYER(g.getActivePlayer()).getNameKey(),
					GET_PLAYER((PlayerTypes)info.getData1()).getNameKey(),
					GC.getInfo((BonusTypes)info.getData2()).getTextKeyWide());
			m_kUI.addMessage((PlayerTypes)info.getData1(), false, -1, szBuffer);
		}
		else
		{
			CvMessageControl::getInstance().sendChangeWar(
					TEAMID((PlayerTypes)info.getData1()), true);
		}

		break;

	case BUTTONPOPUP_EVENT:
		if (pPopupReturn->getButtonClicked() == GC.getNumEventInfos())
		{
			CvPlayer& kActivePlayer = GET_PLAYER(g.getActivePlayer());
			EventTriggeredData* pTriggeredData = kActivePlayer.getEventTriggered(info.getData1());
			if (NULL != pTriggeredData)
			{
				CvCity* pCity = kActivePlayer.getCity(pTriggeredData->m_iCityId);
				if (pCity != NULL)
					m_kUI.selectCity(pCity, true);
			}

			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_EVENT, info.getData1());
			m_kUI.addPopup(pInfo, g.getActivePlayer(), false, true);
		}
		else if (-1 != pPopupReturn->getButtonClicked())
		{
			CvMessageControl::getInstance().sendEventTriggered(g.getActivePlayer(),
					(EventTypes)pPopupReturn->getButtonClicked(), info.getData1());
		}
		break;

	case BUTTONPOPUP_FREE_COLONY:
		if (pPopupReturn->getButtonClicked() > 0)
		{
			CvMessageControl::getInstance().sendEmpireSplit(
					g.getActivePlayer(), pPopupReturn->getButtonClicked());
		}
		else if (pPopupReturn->getButtonClicked() < 0)
		{
			CvMessageControl::getInstance().sendDoTask(-pPopupReturn->getButtonClicked(),
					TASK_LIBERATE, 0, -1, false, false, false, false);
		}
		break;

	case BUTTONPOPUP_LAUNCH:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			CvMessageControl::getInstance().sendLaunch(
					g.getActivePlayer(), (VictoryTypes)info.getData1());
		}
		break;

	case BUTTONPOPUP_FOUND_RELIGION:
		CvMessageControl::getInstance().sendFoundReligion(g.getActivePlayer(), (ReligionTypes)
				pPopupReturn->getButtonClicked(), (ReligionTypes)info.getData1());
		break;
	// <advc.706>
	case BUTTONPOPUP_RF_CHOOSECIV:
		g.getRiseFall().afterCivSelection(pPopupReturn->getButtonClicked());
		break;
	case BUTTONPOPUP_RF_DEFEAT:
		g.getRiseFall().handleDefeatPopup(pPopupReturn->getButtonClicked());
		break;
	case BUTTONPOPUP_RF_RETIRE:
		g.getRiseFall().handleRetirePopup(pPopupReturn->getButtonClicked());
		break; // </advc.706>

	default:
		FAssert(false);
	}
}

void CvDLLButtonPopup::OnFocus(CvPopup* pPopup, CvPopupInfo &info)
{
	if (m_kUI.popupIsDying(pPopup))
		return;

	FAssert(GC.getGame().getActivePlayer() != NO_PLAYER); // K-Mod

	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_CHOOSETECH:
		if (info.getData1() == 0)
		{
			if (GET_PLAYER(GC.getGame().getActivePlayer()).getCurrentResearch() != NO_TECH ||
				GC.getGame().getGameState() == GAMESTATE_OVER)
			{
				m_kUI.popupSetAsCancelled(pPopup);
			}
			// K-Mod todo: Perhaps we should cancel this popup and create a new one on-the-fly so that we can be sure the tech-list is up-to-date.
		}
		// K-Mod
		else if (!GET_PLAYER(GC.getGame().getActivePlayer()).isChoosingFreeTech())
			m_kUI.popupSetAsCancelled(pPopup);
		// K-Mod end
		break;

	case BUTTONPOPUP_CHANGERELIGION:
		if (!GET_PLAYER(GC.getGame().getActivePlayer()).canChangeReligion() ||
			GC.getGame().getGameState() == GAMESTATE_OVER)
		{
			m_kUI.popupSetAsCancelled(pPopup);
		}
		break;

	case BUTTONPOPUP_CHOOSEPRODUCTION:
		if (GC.getGame().getGameState() == GAMESTATE_OVER)
			m_kUI.popupSetAsCancelled(pPopup);
		else
		{
			PlayerTypes ePlayer = GC.getGame().getActivePlayer();
			CvCity* pCity = GET_PLAYER(ePlayer).getCity(info.getData1());

			if (NULL == pCity || pCity->getOwner() != ePlayer || pCity->isProduction())
			{
				m_kUI.popupSetAsCancelled(pPopup);
				break;
			}
			// K-Mod - postpone the choose-production popup if the city is in disorder.
			else if (pCity->isDisorder() || pCity->isProductionAutomated())
			{
				m_kUI.popupSetAsCancelled(pPopup);
				pCity->setChooseProductionDirty(true);
				break;
			}
			// K-Mod end

			m_kUI.lookAtCityOffset(pCity->getID());
		}
		break;

	case BUTTONPOPUP_RAZECITY:
	case BUTTONPOPUP_DISBANDCITY:
	{
		PlayerTypes ePlayer = GC.getGame().getActivePlayer();
		CvCity* pCity = GET_PLAYER(ePlayer).getCity(info.getData1());
		if (NULL == pCity || pCity->getOwner() != ePlayer)
		{
			m_kUI.popupSetAsCancelled(pPopup);
			break;
		}
		m_kUI.lookAtCityOffset(pCity->getID());
		break;
	}

	case BUTTONPOPUP_CHANGECIVIC:
		if (!GET_PLAYER(GC.getGame().getActivePlayer()).canRevolution(NULL) ||
			GC.getGame().getGameState() == GAMESTATE_OVER)
		{
			m_kUI.popupSetAsCancelled(pPopup);
		}
		break;

	case BUTTONPOPUP_PYTHON:
	case BUTTONPOPUP_PYTHON_SCREEN:
		if (GC.getPythonCaller()->onFocus(info))
			m_kUI.popupSetAsCancelled(pPopup);
		break;
	// K-Mod. cancel unnecessary popups if the game is over. (It's tempting to do this for the 'default:' case, but that might break something.)
	case BUTTONPOPUP_CHOOSEELECTION:
	case BUTTONPOPUP_DIPLOVOTE:
		if (GC.getGame().getGameState() == GAMESTATE_OVER)
			m_kUI.popupSetAsCancelled(pPopup);
		break;
	// K-Mod end
	}
}

// returns false if popup is not launched
bool CvDLLButtonPopup::launchButtonPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	// <advc.706>
	CvGame& g = GC.getGame();
	if(g.isOption(GAMEOPTION_RISE_FALL))
	{
		ButtonPopupTypes bpt = info.getButtonPopupType();
		if(bpt == BUTTONPOPUP_RF_CHOOSECIV)
			return g.getRiseFall().launchCivSelectionPopup(pPopup, info);
		if(bpt == BUTTONPOPUP_RF_DEFEAT)
			return g.getRiseFall().launchDefeatPopup(pPopup, info);
		if(CvPlot::isAllFog())
			return false;
		if(bpt == BUTTONPOPUP_RF_RETIRE)
			return g.getRiseFall().launchRetirePopup(pPopup, info);
		/*  The EXE launches these popups after human takeover; afterwards(?),
			the AI makes a choice, and the popup is killed once the player clicks
			on it. */
		if((bpt == BUTTONPOPUP_CHOOSEPRODUCTION || bpt == BUTTONPOPUP_CHOOSETECH) &&
			g.getRiseFall().isBlockPopups())
		{
			return false;
		}
	} // </advc.706>
	FAssert(GC.getGame().getActivePlayer() != NO_PLAYER); // K-Mod

	bool bLaunched = false;

	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_TEXT:
		bLaunched = launchTextPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CHOOSEPRODUCTION:
		bLaunched = launchProductionPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CHANGERELIGION:
		bLaunched = launchChangeReligionPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CHOOSEELECTION:
		bLaunched = launchChooseElectionPopup(pPopup, info);
		break;
	case BUTTONPOPUP_DIPLOVOTE:
		bLaunched = launchDiploVotePopup(pPopup, info);
		break;
	case BUTTONPOPUP_RAZECITY:
		bLaunched = launchRazeCityPopup(pPopup, info);
		break;
	case BUTTONPOPUP_DISBANDCITY:
		bLaunched = launchDisbandCityPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CHOOSETECH:
		bLaunched = launchChooseTechPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CHANGECIVIC:
		bLaunched = launchChangeCivicsPopup(pPopup, info);
		break;
	case BUTTONPOPUP_ALARM:
		bLaunched = launchAlarmPopup(pPopup, info);
		break;
	case BUTTONPOPUP_DECLAREWARMOVE:
		bLaunched = launchDeclareWarMovePopup(pPopup, info);
		break;
	case BUTTONPOPUP_CONFIRMCOMMAND:
		bLaunched = launchConfirmCommandPopup(pPopup, info);
		break;
	case BUTTONPOPUP_LOADUNIT:
		bLaunched = launchLoadUnitPopup(pPopup, info);
		break;
	case BUTTONPOPUP_LEADUNIT:
		bLaunched = launchLeadUnitPopup(pPopup, info);
		break;
	case BUTTONPOPUP_DOESPIONAGE:
		bLaunched = launchDoEspionagePopup(pPopup, info);
		break;
	case BUTTONPOPUP_DOESPIONAGE_TARGET:
		bLaunched = launchDoEspionageTargetPopup(pPopup, info);
		break;
	case BUTTONPOPUP_MAIN_MENU:
		bLaunched = launchMainMenuPopup(pPopup, info);
		break;
	case BUTTONPOPUP_CONFIRM_MENU:
		bLaunched = launchConfirmMenu(pPopup, info);
		break;
	case BUTTONPOPUP_PYTHON_SCREEN:
		// so the Popup object is deleted, since it's just a dummy
		bLaunched = false;
		GC.getPythonCaller()->launchPythonScreenPopup(info, pPopup);
		break;
	case BUTTONPOPUP_DEAL_CANCELED:
		bLaunched = launchCancelDeal(pPopup, info);
		break;
	case BUTTONPOPUP_PYTHON:
		bLaunched = launchPythonPopup(pPopup, info);
		break;
	case BUTTONPOPUP_DETAILS:
		bLaunched = launchDetailsPopup(pPopup, info);
		break;
	case BUTTONPOPUP_ADMIN:
		bLaunched = launchAdminPopup(pPopup, info);
		break;
	case BUTTONPOPUP_ADMIN_PASSWORD:
		bLaunched = launchAdminPasswordPopup(pPopup, info);
		break;
	case BUTTONPOPUP_EXTENDED_GAME:
		bLaunched = launchExtendedGamePopup(pPopup, info);
		break;
	case BUTTONPOPUP_DIPLOMACY:
		bLaunched = launchDiplomacyPopup(pPopup, info);
		break;
	case BUTTONPOPUP_ADDBUDDY:
		bLaunched = launchAddBuddyPopup(pPopup, info);
		break;
	case BUTTONPOPUP_FORCED_DISCONNECT:
		bLaunched = launchForcedDisconnectPopup(pPopup, info);
		break;
	case BUTTONPOPUP_PITBOSS_DISCONNECT:
		bLaunched = launchPitbossDisconnectPopup(pPopup, info);
		break;
	case BUTTONPOPUP_KICKED:
		bLaunched = launchKickedPopup(pPopup, info);
		break;
	case BUTTONPOPUP_VASSAL_DEMAND_TRIBUTE:
		bLaunched = launchVassalDemandTributePopup(pPopup, info);
		break;
	case BUTTONPOPUP_VASSAL_GRANT_TRIBUTE:
		bLaunched = launchVassalGrantTributePopup(pPopup, info);
		break;
	case BUTTONPOPUP_EVENT:
		bLaunched = launchEventPopup(pPopup, info);
		break;
	case BUTTONPOPUP_FREE_COLONY:
		bLaunched = launchFreeColonyPopup(pPopup, info);
		break;
	case BUTTONPOPUP_LAUNCH:
		bLaunched = launchLaunchPopup(pPopup, info);
		break;
	case BUTTONPOPUP_FOUND_RELIGION:
		bLaunched = launchFoundReligionPopup(pPopup, info);
		break;
	default:
		FAssert(false);
		break;
	}
	return (bLaunched);
}


bool CvDLLButtonPopup::launchTextPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetBodyString(pPopup, info.getText());
	m_kUI.popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return true;
}


bool CvDLLButtonPopup::launchProductionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvWString szBuffer;
	CvString szArtFilename;
	CvWString szTemp;

	CvCityAI* pCity = GET_PLAYER(GC.getGame().getActivePlayer()).AI_getCity(info.getData1());
	if (pCity == NULL || pCity->isProductionAutomated())
		return false;

	if (GC.getPythonCaller()->isSkipProductionPopup(*pCity))
		return false;

	FAssert(pCity->getOwner() == GC.getGame().getActivePlayer());

	UnitTypes eTrainUnit = NO_UNIT;
	BuildingTypes eConstructBuilding = NO_BUILDING;
	ProjectTypes eCreateProject = NO_PROJECT;
	switch (info.getData2())
	{
	case (ORDER_TRAIN):
		eTrainUnit = (UnitTypes)info.getData3();
		break;
	case (ORDER_CONSTRUCT):
		eConstructBuilding = (BuildingTypes)info.getData3();
		break;
	case (ORDER_CREATE):
		eCreateProject = (ProjectTypes)info.getData3();
		break;
	default:
		break;
	}
	bool bFinish = info.getOption1();

	if (eTrainUnit != NO_UNIT)
	{
		if (bFinish)
		{
			szBuffer = gDLL->getText(GC.getInfo(eTrainUnit).isLimited() ?
					"TXT_KEY_POPUP_TRAINED_WORK_ON_NEXT_LIMITED" :
					"TXT_KEY_POPUP_TRAINED_WORK_ON_NEXT",
					GC.getInfo(eTrainUnit).getTextKeyWide(), pCity->getNameKey());
		}
		else
		{
			szBuffer = gDLL->getText(GC.getInfo(eTrainUnit).isLimited() ?
					"TXT_KEY_POPUP_CANNOT_TRAIN_WORK_NEXT_LIMITED" :
					"TXT_KEY_POPUP_CANNOT_TRAIN_WORK_NEXT",
					GC.getInfo(eTrainUnit).getTextKeyWide(), pCity->getNameKey());
		}
		szArtFilename = GET_PLAYER(pCity->getOwner()).getUnitButton(eTrainUnit);
	}
	else if (eConstructBuilding != NO_BUILDING)
	{
		if (bFinish)
		{
			szBuffer = gDLL->getText(GC.getInfo(eConstructBuilding).isLimited() ?
					"TXT_KEY_POPUP_CONSTRUCTED_WORK_ON_NEXT_LIMITED" :
					"TXT_KEY_POPUP_CONSTRUCTED_WORK_ON_NEXT",
					GC.getInfo(eConstructBuilding).getTextKeyWide(), pCity->getNameKey());
		}
		else
		{
			szBuffer = gDLL->getText(GC.getInfo(eConstructBuilding).isLimited() ?
					"TXT_KEY_POPUP_CANNOT_CONSTRUCT_WORK_NEXT_LIMITED" :
					"TXT_KEY_POPUP_CANNOT_CONSTRUCT_WORK_NEXT",
					GC.getInfo(eConstructBuilding).getTextKeyWide(), pCity->getNameKey());
		}
		szArtFilename = GC.getInfo(eConstructBuilding).getButton();
	}
	else if (eCreateProject != NO_PROJECT)
	{
		if (bFinish)
		{
			if (GC.getInfo(eCreateProject).isSpaceship())
			{
				szBuffer = gDLL->getText("TXT_KEY_POPUP_CREATED_WORK_ON_NEXT_SPACESHIP",
						GC.getInfo(eCreateProject).getTextKeyWide(), pCity->getNameKey());
			}
			else
			{
				szBuffer = gDLL->getText(GC.getInfo(eCreateProject).isLimited() ?
						// <advc.108e>
						(GC.getInfo(eCreateProject).nameNeedsArticle() ?
						"TXT_KEY_POPUP_CREATED_WORK_ON_NEXT_LIMITED_THE" :
						"TXT_KEY_POPUP_CREATED_WORK_ON_NEXT_LIMITED")
						// </advc.108e>
						: "TXT_KEY_POPUP_CREATED_WORK_ON_NEXT",
						GC.getInfo(eCreateProject).getTextKeyWide(), pCity->getNameKey());
			}
		}
		else
		{
			szBuffer = gDLL->getText(GC.getInfo(eCreateProject).isLimited() ?
					"TXT_KEY_POPUP_CANNOT_CREATE_WORK_NEXT_LIMITED" :
					"TXT_KEY_POPUP_CANNOT_CREATE_WORK_NEXT",
					GC.getInfo(eCreateProject).getTextKeyWide(), pCity->getNameKey());
		}
		szArtFilename = GC.getInfo(eCreateProject).getButton();
	}
	else
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_WHAT_TO_BUILD", pCity->getNameKey());
		szArtFilename = ARTFILEMGR.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_PRODUCTION")->getPath();
	}

	m_kUI.popupSetHeaderString(pPopup, szBuffer, DLL_FONT_LEFT_JUSTIFY);

	if (GC.getPythonCaller()->isShowExamineButton(*pCity))
	{
		int iExamineCityID = 0;
		iExamineCityID = std::max(iExamineCityID, GC.getNumUnitInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumBuildingInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumProjectInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumProcessInfos());
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_POPUP_EXAMINE_CITY").c_str(),
				ARTFILEMGR.getInterfaceArtPath("INTERFACE_BUTTONS_CITYSELECTION"),
				iExamineCityID, WIDGET_GENERAL, -1, -1, true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
	}
	// Were never read before being reassigned
	/*UnitTypes eProductionUnit = pCity->getProductionUnit();
	BuildingTypes eProductionBuilding = pCity->getProductionBuilding();*/
	ProjectTypes eProductionProject = pCity->getProductionProject();
	ProcessTypes eProductionProcess = pCity->getProductionProcess();

	int iNumBuilds = 0;

	UnitTypes eProductionUnit = GC.getPythonCaller()->recommendedUnit(*pCity);
	BuildingTypes eProductionBuilding = GC.getPythonCaller()->recommendedBuilding(*pCity);

	if (eProductionUnit == NO_UNIT)
	{
		eProductionUnit = pCity->AI_bestUnit(true,
				eProductionBuilding == NO_BUILDING ? NO_ADVISOR :
				GC.getInfo(eProductionBuilding).getAdvisorType());
	}

	if (eProductionBuilding == NO_BUILDING)
	{
		eProductionBuilding = pCity->AI_bestBuilding(0, 50, true,
				eProductionUnit == NO_UNIT ? NO_ADVISOR :
				GC.getInfo(eProductionUnit).getAdvisorType());
	}

	if (eProductionUnit != NO_UNIT)
	{
		int iTurns = pCity->getProductionTurnsLeft(eProductionUnit, 0);
		// advc.004x:
		iTurns = pCity->sanitizeProductionTurns(iTurns, ORDER_TRAIN, eProductionUnit);
		CvUnitInfo const& kInfo = GC.getInfo(eProductionUnit);
		szBuffer = gDLL->getText("TXT_KEY_POPUP_RECOMMENDED", kInfo.getTextKeyWide(), iTurns,
				GC.getInfo(kInfo.getAdvisorType()).getTextKeyWide());
		m_kUI.popupAddGenericButton(pPopup, szBuffer,
				GET_PLAYER(pCity->getOwner()).getUnitButton(eProductionUnit),
				kInfo.getUnitClassType(), WIDGET_TRAIN,
				kInfo.getUnitClassType(), pCity->getID(), true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
		iNumBuilds++;
	}

	if (eProductionBuilding != NO_BUILDING)
	{
		int iTurns = pCity->getProductionTurnsLeft(eProductionBuilding, 0);
		// advc.004x:
		iTurns = pCity->sanitizeProductionTurns(iTurns, ORDER_CONSTRUCT, eProductionBuilding);
		CvBuildingInfo const& kInfo = GC.getInfo(eProductionBuilding);
		szBuffer = gDLL->getText("TXT_KEY_POPUP_RECOMMENDED", kInfo.getTextKeyWide(), iTurns,
				GC.getInfo(kInfo.getAdvisorType()).getTextKeyWide());
		m_kUI.popupAddGenericButton(pPopup, szBuffer,
				kInfo.getButton(), kInfo.getBuildingClassType(), WIDGET_CONSTRUCT,
				kInfo.getBuildingClassType(), pCity->getID(), true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
		iNumBuilds++;
	}

	if (eProductionProject != NO_PROJECT)
	{
		int iTurns = pCity->getProductionTurnsLeft(eProductionProject, 0);
		// advc.004x:
		iTurns = pCity->sanitizeProductionTurns(iTurns, ORDER_CREATE, eProductionProject);
		CvProjectInfo const& kInfo = GC.getInfo(eProductionProject);
		szBuffer = gDLL->getText("TXT_KEY_POPUP_RECOMMENDED_NO_ADV",
				kInfo.getTextKeyWide(), iTurns);
		m_kUI.popupAddGenericButton(pPopup, szBuffer,
				kInfo.getButton(), eProductionProject, WIDGET_CREATE,
				eProductionProject, pCity->getID(), true, POPUP_LAYOUT_STRETCH,
				DLL_FONT_LEFT_JUSTIFY);
		iNumBuilds++;
	}

	if (eProductionProcess != NO_PROCESS)
	{
		CvProcessInfo const& kInfo = GC.getInfo(eProductionProcess);
		szBuffer = gDLL->getText("TXT_KEY_POPUP_RECOMMENDED_NO_ADV_OR_TURNS",
				kInfo.getTextKeyWide());
		m_kUI.popupAddGenericButton(pPopup, szBuffer,
				kInfo.getButton(), eProductionProcess, WIDGET_MAINTAIN,
				eProductionProcess, pCity->getID(), true, POPUP_LAYOUT_STRETCH,
				DLL_FONT_LEFT_JUSTIFY);
		iNumBuilds++;
	}
	CvCivilization const& kCiv = pCity->getCivilization(); // advc.003w
	for (int i = 0; i < kCiv.getNumUnits(); i++)
	{
		UnitTypes eLoopUnit = kCiv.unitAt(i);
		if (eLoopUnit == eProductionUnit || !pCity->canTrain(eLoopUnit))
			continue; // advc
		UnitClassTypes eUnitClass = kCiv.unitClassAt(i);
		int iTurns = pCity->getProductionTurnsLeft(eLoopUnit, 0);
		// advc.004x:
		iTurns = pCity->sanitizeProductionTurns(iTurns, ORDER_TRAIN, eLoopUnit);
		szBuffer.Format(L"%s (%d)", GC.getInfo(eLoopUnit).getDescription(), iTurns);
		m_kUI.popupAddGenericButton(pPopup, szBuffer,
				GET_PLAYER(pCity->getOwner()).getUnitButton(eLoopUnit), eUnitClass,
				WIDGET_TRAIN, eUnitClass, pCity->getID(), true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
		iNumBuilds++;

	}
	for (int i = 0; i < kCiv.getNumBuildings(); i++)
	{
		BuildingTypes eLoopBuilding = kCiv.buildingAt(i);
		if (eLoopBuilding == eProductionBuilding || !pCity->canConstruct(eLoopBuilding))
			continue; // advc
		BuildingClassTypes eBuildingClass = kCiv.buildingClassAt(i);
		int iTurns = pCity->getProductionTurnsLeft(eLoopBuilding, 0);
		// advc.004x:
		iTurns = pCity->sanitizeProductionTurns(iTurns, ORDER_CONSTRUCT, eLoopBuilding);
		szBuffer.Format(L"%s (%d)", GC.getInfo(eLoopBuilding).getDescription(), iTurns);
		m_kUI.popupAddGenericButton(pPopup, szBuffer,
				GC.getInfo(eLoopBuilding).getButton(), eBuildingClass,
				WIDGET_CONSTRUCT, eBuildingClass, pCity->getID(), true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
			iNumBuilds++;
	}
	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		ProjectTypes eLoopProject = (ProjectTypes)iI;
		if (eLoopProject == eProductionProject || !pCity->canCreate(eLoopProject))
			continue; // advc
		int iTurns = pCity->getProductionTurnsLeft((ProjectTypes)iI, 0);
		// advc.004x:
		iTurns = pCity->sanitizeProductionTurns(iTurns, ORDER_CREATE, eLoopProject);
		szBuffer.Format(L"%s (%d)", GC.getInfo(eLoopProject).getDescription(), iTurns);
		m_kUI.popupAddGenericButton(pPopup, szBuffer,
				GC.getInfo(eLoopProject).getButton(), eLoopProject, WIDGET_CREATE, eLoopProject,
				pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
		iNumBuilds++;
	}
	for (int iI = 0; iI < GC.getNumProcessInfos(); iI++)
	{
		ProcessTypes eLoopProcess = (ProcessTypes)iI;
		if (eLoopProcess == eProductionProcess || !pCity->canMaintain(eLoopProcess))
			continue; // advc
		m_kUI.popupAddGenericButton(pPopup,
				GC.getInfo(eLoopProcess).getDescription(),
				GC.getInfo(eLoopProcess).getButton(), eLoopProcess, WIDGET_MAINTAIN,
				eLoopProcess, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
		iNumBuilds++;
	}

	if (iNumBuilds <= 0)
	{	// city cannot build anything, so don't show popup after all
		return false;
	}

	m_kUI.popupSetPopupType(pPopup, POPUPEVENT_PRODUCTION, szArtFilename);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_MINIMIZED, 252);

	switch (info.getData2())
	{
	case ORDER_TRAIN:
		m_kUI.playGeneralSound(GC.getInfo((UnitTypes)info.getData3()).
				getArtInfo(0, GET_PLAYER(pCity->getOwner()).getCurrentEra(), NO_UNIT_ARTSTYLE)->getTrainSound());
		break;

	case ORDER_CONSTRUCT:
		m_kUI.playGeneralSound(GC.getInfo((BuildingTypes)info.getData3()).getConstructSound());
		break;

	case ORDER_CREATE:
		m_kUI.playGeneralSound(GC.getInfo((ProjectTypes)info.getData3()).getCreateSound());
		break;

	default: break;
	}

	return true;
}


bool CvDLLButtonPopup::launchChangeReligionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvWString szTemp;
	ReligionTypes eReligion = (ReligionTypes)info.getData1();

	if (NO_RELIGION == eReligion)
	{
		FAssert(false);
		return false;
	}

	CvPlayer& activePlayer = GET_PLAYER(GC.getGame().getActivePlayer());
	if (!activePlayer.canConvert(eReligion))
	{
		return false;
	}

	CvWString szBuffer;
	szBuffer = gDLL->getText("TXT_KEY_POPUP_RELIGION_SPREAD", GC.getInfo(eReligion).getTextKeyWide());
	if (activePlayer.getStateReligionHappiness() != 0)
	{
		if (activePlayer.getStateReligionHappiness() > 0)
		{
			szBuffer += gDLL->getText("TXT_KEY_POPUP_CONVERTING_EFFECTS",
				activePlayer.getStateReligionHappiness(), gDLL->getSymbolID(HAPPY_CHAR), GC.getInfo(eReligion).getChar());
		}
		else
		{
			szBuffer += gDLL->getText("TXT_KEY_POPUP_CONVERTING_EFFECTS",
				-activePlayer.getStateReligionHappiness(), gDLL->getSymbolID(UNHAPPY_CHAR), GC.getInfo(eReligion).getChar());
		}
	}
	szBuffer += gDLL->getText("TXT_KEY_POPUP_LIKE_TO_CONVERT");
	m_kUI.popupSetBodyString(pPopup, szBuffer);

	szBuffer = gDLL->getText("TXT_KEY_POPUP_CONVERT_RELIGION");
	int iAnarchyLength = activePlayer.getReligionAnarchyLength();
	if (iAnarchyLength > 0)
	{
		szBuffer += gDLL->getText("TXT_KEY_POPUP_TURNS_OF_ANARCHY", iAnarchyLength);
	}
	m_kUI.popupAddGenericButton(pPopup, szBuffer, NULL, 0);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO_CONVERSION").c_str());
	m_kUI.popupSetPopupType(pPopup, POPUPEVENT_RELIGION,
			ARTFILEMGR.getInterfaceArtPath("INTERFACE_POPUPBUTTON_RELIGION"));
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_MINIMIZED);
	return true;
}


bool CvDLLButtonPopup::launchChooseElectionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	VoteSelectionData* pVoteSelectionData = GC.getGame().getVoteSelection(info.getData1());
	if(NULL == pVoteSelectionData)
		return false;
	VoteSourceTypes eVoteSource = pVoteSelectionData->eVoteSource;
	m_kUI.popupSetBodyString(pPopup, GC.getInfo(eVoteSource).getPopupText());
	for(int iI = 0; iI < (int)pVoteSelectionData->aVoteOptions.size(); iI++)
	{
		m_kUI.popupAddGenericButton(pPopup,
				pVoteSelectionData->aVoteOptions[iI].szText, NULL, iI, WIDGET_GENERAL);
	}
	// advc.178:
	bool bEarlyElection = (GC.getGame().getSecretaryGeneralTimer(eVoteSource) > 0);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText(
			bEarlyElection ? "TXT_KEY_EARLY_ELECTION" : // advc.178
			"TXT_KEY_NONE").c_str(), NULL, GC.getNumVoteInfos(), WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return true;
}


bool CvDLLButtonPopup::launchDiploVotePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	VoteTriggeredData* pVoteTriggered = GC.getGame().getVoteTriggered(info.getData1());
	if (NULL == pVoteTriggered)
	{
		FAssert(false);
		return false;
	}

	VoteTypes eVote = pVoteTriggered->kVoteOption.eVote;
	VoteSourceTypes eVoteSource = pVoteTriggered->eVoteSource;

	TeamTypes eVassalOfTeam = NO_TEAM;
	bool bEligible = false;

	m_kUI.popupSetHeaderString(pPopup, GC.getInfo(eVoteSource).getDescription());
	m_kUI.popupSetBodyString(pPopup, pVoteTriggered->kVoteOption.szText);
	if (GC.getGame().isTeamVote(eVote))
	{
		for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GC.getGame().isTeamVoteEligible((TeamTypes)iI, eVoteSource))
				{
					if (GET_TEAM(GC.getGame().getActiveTeam()).isVassal((TeamTypes)iI))
					{
						eVassalOfTeam = (TeamTypes)iI;
						break;
					}
				}
			}
		}

		for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GC.getGame().isTeamVoteEligible((TeamTypes)iI, eVoteSource))
				{
					if (eVassalOfTeam == NO_TEAM || eVassalOfTeam == iI || iI == GC.getGame().getActiveTeam())
					{
						m_kUI.popupAddGenericButton(pPopup, GET_TEAM((TeamTypes)iI).getName().GetCString(),
								NULL, iI, WIDGET_GENERAL);
						bEligible = true;
					}
				}
			}
		}
	}
	else
	{
		m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES").c_str(),
				NULL, PLAYER_VOTE_YES, WIDGET_GENERAL);
		m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO").c_str(),
				NULL, PLAYER_VOTE_NO, WIDGET_GENERAL);

		if (GET_PLAYER(GC.getGame().getActivePlayer()).canDefyResolution(eVoteSource, pVoteTriggered->kVoteOption))
		{
			m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_VOTE_NEVER").c_str(),
					NULL, PLAYER_VOTE_NEVER, WIDGET_GENERAL);
		}
		bEligible = true;
	}

	if (eVassalOfTeam == NO_TEAM || !bEligible)
	{
		m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_ABSTAIN").c_str(),
				NULL, PLAYER_VOTE_ABSTAIN, WIDGET_GENERAL);
	}

	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return true;
}


bool CvDLLButtonPopup::launchRazeCityPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayer& player = GET_PLAYER(GC.getGame().getActivePlayer());

	CvCity* pNewCity = player.getCity(info.getData1());
	if (NULL == pNewCity)
	{
		FAssert(false);
		return false;
	}
	if (GC.getDefineINT("PLAYER_ALWAYS_RAZES_CITIES") != 0)
	{
		player.raze(*pNewCity);
		return false;
	}

	int iCaptureGold = info.getData3();
	bool bRaze = player.canRaze(*pNewCity);
	PlayerTypes eLiberationPlayer = (PlayerTypes)info.getData2(); // advc: Was still named eHighestCulturePlayer as in Vanilla Civ 4
	// advc: Other bGift checks deleted; now implied by eLiberationPlayer.
	bool bGift = (eLiberationPlayer != NO_PLAYER);
	CvWString szBuffer;
	if (iCaptureGold > 0)
		szBuffer = gDLL->getText("TXT_KEY_POPUP_GOLD_CITY_CAPTURE", iCaptureGold, pNewCity->getNameKey());
	else szBuffer = gDLL->getText("TXT_KEY_POPUP_CITY_CAPTURE_KEEP", pNewCity->getNameKey());

	m_kUI.popupSetBodyString(pPopup, szBuffer);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_KEEP_CAPTURED_CITY").c_str(),
			NULL, 0, WIDGET_GENERAL);

	if (bRaze)
	{
		m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_RAZE_CAPTURED_CITY").c_str(),
				NULL, 1, WIDGET_GENERAL);
	}
	if (bGift)
	{	// <advc.ctr> 
		bool bReturnCity = pNewCity->isEverOwned(eLiberationPlayer);
		szBuffer = gDLL->getText(bReturnCity ? "TXT_KEY_POPUP_RETURN_ALLIED_CITY" :
				"TXT_KEY_POPUP_LIBERATE_ALLIED_CITY", // </advc.ctr>
				GET_PLAYER(eLiberationPlayer).getCivilizationDescriptionKey());
		m_kUI.popupAddGenericButton(pPopup, szBuffer, NULL, 2, WIDGET_GENERAL, 2, eLiberationPlayer);
	}
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_CITY_WARNING_ANSWER3").c_str(),
			NULL, 3, WIDGET_GENERAL, -1, -1);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	//m_kUI.playGeneralSound("AS2D_CITYCAPTURE"); // disabled by K-Mod (I've put this somewhere else.)

	return true;
}

bool CvDLLButtonPopup::launchDisbandCityPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayer& player = GET_PLAYER(GC.getGame().getActivePlayer());

	CvCity* pNewCity = player.getCity(info.getData1());
	if (NULL == pNewCity)
	{
		FAssert(false);
		return false;
	}

	CvWString szBuffer;
	szBuffer = gDLL->getText("TXT_KEY_POPUP_FLIPPED_CITY_KEEP", pNewCity->getNameKey());
	m_kUI.popupSetBodyString(pPopup, szBuffer);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_KEEP_FLIPPED_CITY").c_str(),
			NULL, 0, WIDGET_GENERAL);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_DISBAND_FLIPPED_CITY").c_str(),
			NULL, 1, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	m_kUI.playGeneralSound("AS2D_CULTUREFLIP");

	return true;
}

bool CvDLLButtonPopup::launchChooseTechPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayerAI& kActivePlayer = GET_PLAYER(GC.getGame().getActivePlayer());

	if (GC.getPythonCaller()->isSkipResearchPopup(kActivePlayer.getID()))
		return false;

	int iDiscover = info.getData1();
	CvWString szHeader = info.getText();
	if (szHeader.empty())
	{
		szHeader = (iDiscover > 0) ? gDLL->getText("TXT_KEY_POPUP_CHOOSE_TECH").c_str() :
				gDLL->getText("TXT_KEY_POPUP_RESEARCH_NEXT").c_str();
	}
	m_kUI.popupSetHeaderString(pPopup, szHeader, DLL_FONT_LEFT_JUSTIFY);
	if (iDiscover == 0)
	{
		if (GC.getPythonCaller()->isShowTechChooserButton(kActivePlayer.getID()))
		{
			// Allow user to Jump to the Tech Chooser
			m_kUI.popupAddGenericButton(pPopup,
					gDLL->getText("TXT_KEY_POPUP_SEE_BIG_PICTURE").c_str(),
					ARTFILEMGR.getInterfaceArtPath("INTERFACE_POPUPBUTTON_TECH"),
					GC.getNumTechInfos(), WIDGET_GENERAL, -1, MAX_INT, true,
					POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
			// Note: This button is NOT supposed to close the popup!!
		}
	}
	TechTypes eBestTech = GC.getPythonCaller()->recommendedTech(kActivePlayer.getID(), true);
	TechTypes eNextBestTech = NO_TECH;
	if (eBestTech == NO_TECH)
		eBestTech = kActivePlayer.AI_bestTech(1, (iDiscover > 0), true);
	if (eBestTech != NO_TECH)
	{
		eNextBestTech = GC.getPythonCaller()->recommendedTech(kActivePlayer.getID(),
				false, eBestTech);
		if (eNextBestTech == NO_TECH)
		{
			eNextBestTech = kActivePlayer.AI_bestTech(1, iDiscover > 0, true,
					eBestTech, (AdvisorTypes)GC.getInfo(eBestTech).getAdvisorType());
		}
	}
	int iNumTechs = 0;
	for (int iPass = 0; iPass < 2; iPass++)
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if((iI == eBestTech || iI == eNextBestTech) != (iPass == 0))
				continue; // advc
			TechTypes eTech = (TechTypes)iI;
			if(!kActivePlayer.canResearch(eTech))
				continue; // advc

			CvWString szBuffer;
			// <advc.004x>
			int iTurnsLeft = kActivePlayer.getResearchTurnsLeft(eTech, true);
			if(iDiscover > 0 || iTurnsLeft >= 0) // </advc.004x>
			{
				szBuffer.Format(L"%s (%d)", GC.getInfo(eTech).getDescription(),
						(iDiscover > 0 ? 0 : iTurnsLeft));
			} // advc.004x:
			else szBuffer.Format(L"%s", GC.getInfo(eTech).getDescription());

			if(iI == eBestTech || iI == eNextBestTech)
			{
				szBuffer += gDLL->getText("TXT_KEY_POPUP_RECOMMENDED_ONLY_ADV",
						GC.getInfo((AdvisorTypes)(GC.getInfo(eTech).
						getAdvisorType())).getTextKeyWide());
			}

			CvString szButton = GC.getInfo(eTech).getButton();
			CvGame const& g = GC.getGame(); // advc
			for (int iJ = 0; iJ < GC.getNumReligionInfos(); iJ++)
			{
				ReligionTypes eReligion = (ReligionTypes)iJ;
				if (GC.getInfo(eReligion).getTechPrereq() == iI)
				{
					if (!g.isReligionSlotTaken(eReligion))
					{
						szButton = g.isOption(GAMEOPTION_PICK_RELIGION) ?
								GC.getInfo(eReligion).getGenericTechButton() :
								GC.getInfo(eReligion).getTechButton();
						break;
					}
				}
			}
			m_kUI.popupAddGenericButton(pPopup, szBuffer,
					szButton, iI, WIDGET_RESEARCH, iI, iDiscover, true,
					POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
			iNumTechs++;
		}
	}
	if (iNumTechs == 0)
	{
		// player cannot research anything, so don't show this popup after all
		return false;
	}
	m_kUI.popupSetPopupType(pPopup, POPUPEVENT_TECHNOLOGY,
			ARTFILEMGR.getInterfaceArtPath("INTERFACE_POPUPBUTTON_TECH"));
	m_kUI.popupLaunch(pPopup, false, iDiscover > 0 ?
			POPUPSTATE_QUEUED : POPUPSTATE_MINIMIZED);
	return true;
}

bool CvDLLButtonPopup::launchChangeCivicsPopup(CvPopup* pPopup, CvPopupInfo &info)  // advc: some style changes
{
	CivicTypes* paeNewCivics = new CivicTypes[GC.getNumCivicOptionInfos()];
	if (paeNewCivics == NULL)
		return false;

	CivicOptionTypes eCivicOption = (CivicOptionTypes)info.getData1();
	CivicTypes eCivic = (CivicTypes)info.getData2();
	bool bValid = false;
	bool bStartButton = true; // advc.004o

	CvPlayer const& kActivePlayer = GET_PLAYER(GC.getGame().getActivePlayer());
	if (eCivic != NO_CIVIC)
	{
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			if (iI == eCivicOption)
			{
				paeNewCivics[iI] = eCivic;
			}
			else
			{
				paeNewCivics[iI] = kActivePlayer.getCivics((CivicOptionTypes)iI);
			}
		}

		if (kActivePlayer.canRevolution(paeNewCivics))
		{
			bValid = true;
			// <advc.004o>
			for(int i = 0; i < GC.getNumCivicInfos(); i++) {
				CivicTypes eLoopCivic = (CivicTypes)i;
				if(eLoopCivic != paeNewCivics[GC.getInfo(eLoopCivic).getCivicOptionType()] &&
						!kActivePlayer.isCivic(eLoopCivic) &&
						kActivePlayer.canDoCivics(eLoopCivic)) {
					bStartButton = false;
					break;
				}
			} // </advc.004o>
		}
	}
	else
	{
		bValid = true;
	}

	if (bValid)
	{
		CvWString szBuffer;
		if (eCivic != NO_CIVIC)
		{
			szBuffer = gDLL->getText("TXT_KEY_POPUP_NEW_CIVIC", GC.getInfo(eCivic).getTextKeyWide());
			if (!CvWString(GC.getInfo(eCivic).getStrategy()).empty())
			{
				CvWString szTemp;
				szTemp.Format(L" (%s)", GC.getInfo(eCivic).getStrategy());
				szBuffer += szTemp;
			}
			szBuffer += gDLL->getText("TXT_KEY_POPUP_START_REVOLUTION");
			m_kUI.popupSetBodyString(pPopup, szBuffer);
			// <advc.004o>
			if(bStartButton || GC.getInfo(GC.getGame().getGameSpeedType()).
				/*  "> 100" would leave the get-started button alone on Epic and
					Marathon. That's what I meant to do initially, but now
					I think the button shouldn't be there in any case, so,
					1000 is just an arbitrary high number. */
				getAnarchyPercent() > 1000) // </advc.004o>
			{
				szBuffer = gDLL->getText("TXT_KEY_POPUP_YES_START_REVOLUTION");
				int iAnarchyLength = GET_PLAYER(GC.getGame().getActivePlayer()).getCivicAnarchyLength(paeNewCivics);
				if (iAnarchyLength > 0)
				{
					szBuffer += gDLL->getText("TXT_KEY_POPUP_TURNS_OF_ANARCHY", iAnarchyLength);
				}
				m_kUI.popupAddGenericButton(pPopup, szBuffer, NULL, 0, WIDGET_GENERAL);
			} // advc.004o
		}
		else
		{
			/*  <advc.001> The EXE tries to show the first-revolution pop-up twice
				when playing Advanced Start in a later era; perhaps also with
				other settings. */
			if(GC.getGame().getGameTurn() != GC.getGame().getStartTurn())
			{
				SAFE_DELETE_ARRAY(paeNewCivics)
				return false;
			} // </advc.001>
			m_kUI.popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_FIRST_REVOLUTION"));
		}

		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_POPUP_OLD_WAYS_BEST").c_str(), NULL, 1, WIDGET_GENERAL);
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_POPUP_SEE_BIG_PICTURE").c_str(), NULL, 2, WIDGET_GENERAL);
		m_kUI.popupSetPopupType(pPopup,
				POPUPEVENT_CIVIC, ARTFILEMGR.getInterfaceArtPath("INTERFACE_POPUPBUTTON_CIVICS"));
		m_kUI.popupLaunch(pPopup, false, POPUPSTATE_MINIMIZED);
	}

	SAFE_DELETE_ARRAY(paeNewCivics); // kmodx: Memory leak

	return (bValid);
}


bool CvDLLButtonPopup::launchAlarmPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.playGeneralSound("AS2D_ALARM");

	m_kUI.popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_POPUP_ALARM_TITLE").c_str());
	m_kUI.popupSetBodyString(pPopup, info.getText());
	m_kUI.popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);

	return true;
}


bool CvDLLButtonPopup::launchDeclareWarMovePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	TeamTypes eRivalTeam = (TeamTypes)info.getData1();
	int iX = info.getData2();
	int iY = info.getData3();
	FAssert(eRivalTeam != NO_TEAM);

	PlayerTypes eRivalPlayer = NO_PLAYER; // advc.130h

	CvWString szBuffer;
	CvTeam const& kActiveTeam = GET_TEAM(GC.getGame().getActiveTeam());
	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (pPlot != NULL && pPlot->getTeam() == eRivalTeam)
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_ENTER_LANDS_WAR",
				GET_PLAYER(pPlot->getOwner()).getCivilizationAdjective());
		eRivalPlayer = pPlot->getOwner(); // advc.130h
		if (kActiveTeam.isOpenBordersTrading())
		{
			szBuffer += gDLL->getText("TXT_KEY_POPUP_ENTER_WITH_OPEN_BORDERS");
		}
	}
	else
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_DOES_THIS_MEAN_WAR",
				GET_TEAM(eRivalTeam).getName().GetCString());
	}
	/*  <advc.130h> Overlaps with code in CvTeam::declareWar, CvTeam::getDefensivePower,
		MilitaryAnalyst and perhaps further places.
		Tbd.: Put (efficient) code for anticipating the effects of a DoW in a single place. */
	std::vector<PlayerTypes> aeTargets;
	//std::vector<PlayerTypes> aeAllies;
	for(int i = 0; i < MAX_CIV_PLAYERS; i++)
	{
		CvPlayer const& kPlayer = GET_PLAYER((PlayerTypes)i);
		if(!kPlayer.isAlive())
			continue;
		/*if(kPlayer.getMasterTeam() == kActiveTeam.getMasterTeam())
			aeAllies.push_back(kPlayer.getID());*/
		if(!kActiveTeam.isHasMet(kPlayer.getTeam()) ||
				kActiveTeam.getMasterTeam() == kPlayer.getMasterTeam())
			continue;
		if(GET_TEAM(kPlayer.getTeam()).getMasterTeam() == GET_TEAM(eRivalTeam).getMasterTeam())
			aeTargets.push_back(kPlayer.getID());
	}
	std::vector<PlayerTypes> aeDP;
	std::vector<PlayerTypes> aeDisapproving;
	bool bReceivedTribute = false; // advc.130o
	for(size_t i = 0; i < aeTargets.size(); i++)
	{
		for(int j = 0; j < MAX_CIV_PLAYERS; j++)
		{
			CvPlayerAI const& kPlayer = GET_PLAYER((PlayerTypes)j);
			if(!kPlayer.isAlive())
				continue;
			if(!kActiveTeam.isHasMet(kPlayer.getTeam()) ||
					kActiveTeam.getMasterTeam() == kPlayer.getMasterTeam())
				continue;
			if(GET_TEAM(GET_PLAYER(aeTargets[i]).getMasterTeam()).isDefensivePact(
					kPlayer.getTeam()))
				aeDP.push_back(kPlayer.getID());
			if(!kPlayer.isHuman() && kPlayer.AI_disapprovesOfDoW(kActiveTeam.getID(),
					TEAMID(aeTargets[i])))
				aeDisapproving.push_back(kPlayer.getID());
			// <advc.130o>
			if(!bReceivedTribute &&
					kActiveTeam.isHuman() && !GET_PLAYER(aeTargets[i]).isHuman() &&
					GET_TEAM(eRivalTeam).AI_getMemoryCount(kActiveTeam.getID(),
					MEMORY_MADE_DEMAND) > 0)
				bReceivedTribute = true; // </advc.130o>
		}
	}
	::removeDuplicates(aeDP);
	::removeDuplicates(aeDisapproving);
	bool bFirst = true;
	/*  If eRivalPlayer is set, it means that the DoW popup has only mentioned one
		team member; then the others need to be listed here. */
	uint uiMentioned = (eRivalPlayer != NO_PLAYER ? 1 : GET_TEAM(eRivalTeam).getNumMembers());
	if(aeTargets.size() > uiMentioned)
	{
		szBuffer += NEWLINE;
		bFirst = false;
		szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_DOW_EXTRA_TARGETS") + NEWLINE;
		bool bFirstInner = true;
		for(size_t i = 0; i < aeTargets.size(); i++)
		{
			if((eRivalPlayer != NO_PLAYER && aeTargets[i] == eRivalPlayer) ||
					(eRivalPlayer == NO_PLAYER && TEAMID(aeTargets[i]) == eRivalTeam))
				continue;
			if(bFirstInner)
				bFirstInner = false;
			else szBuffer += L", ";
			szBuffer += GET_PLAYER(aeTargets[i]).getName();
		}
	}
	// <advc.130o>
	if(bReceivedTribute)
	{
		szBuffer += NEWLINE;
		bFirst = false;
		szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_DOW_TRIBUTE") + NEWLINE;
	} // </advc.130o>
	if(!aeDP.empty())
	{
		if(bFirst)
		{
			szBuffer += NEWLINE;
			bFirst = false;
		}
		szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_DOW_DP") + NEWLINE;
		for(size_t i = 0; i < aeDP.size(); i++)
		{
			szBuffer += GET_PLAYER(aeDP[i]).getName();
			if(i < aeDP.size() - 1)
				szBuffer += L", ";
		}
	}
	if(!aeDisapproving.empty())
	{
		if(bFirst)
		{
			szBuffer += NEWLINE;
			bFirst = false;
		}
		szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_DOW_PENALTIES") + NEWLINE;
		for(size_t i = 0; i < aeDisapproving.size(); i++)
		{
			szBuffer += GET_PLAYER(aeDisapproving[i]).getName();
			if(i < aeDisapproving.size() - 1)
				szBuffer += L", ";
		}
	} // </advc.130h>
	m_kUI.popupSetBodyString(pPopup, szBuffer);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_DECLARE_WAR_YES").c_str(), NULL, 0);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_DECLARE_WAR_NO").c_str());
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return true;
}


bool CvDLLButtonPopup::launchConfirmCommandPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	int iAction = info.getData1();
	CvWString szBuffer;
	szBuffer = gDLL->getText("TXT_KEY_POPUP_ARE_YOU_SURE_ACTION", GC.getActionInfo(iAction).getTextKeyWide());
	m_kUI.popupSetBodyString(pPopup, szBuffer);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES").c_str(), NULL, 0);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO").c_str());
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}


bool CvDLLButtonPopup::launchLoadUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvSelectionGroup* pSelectionGroup = m_kUI.getSelectionList();
	if (pSelectionGroup == NULL)
		return false;

	m_kUI.popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_TRANSPORT"));

	CvPlot* pPlot = pSelectionGroup->plot();
	if (pPlot == NULL)
		return false;

	CvWStringBuffer szBuffer;
	CvUnit* pFirstUnit = NULL;
	int iCount = 1;
	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pSelectionGroup->canDoCommand(COMMAND_LOAD_UNIT, pLoopUnit->getOwner(), pLoopUnit->getID()))
		{
			if (!pFirstUnit)
			{
				pFirstUnit = pLoopUnit;
			}
			szBuffer.clear();
			GAMETEXT.setUnitHelp(szBuffer, pLoopUnit, true);
			szBuffer.append(L", ");
			szBuffer.append(gDLL->getText("TXT_KEY_UNIT_HELP_CARGO_SPACE", pLoopUnit->getCargo(), pLoopUnit->cargoSpace()));
			m_kUI.popupAddGenericButton(pPopup, CvWString(szBuffer.getCString()), NULL, iCount, WIDGET_GENERAL);
			iCount++;
		}
	}

	if (iCount <= 2)
	{
		if (pFirstUnit)
		{
			GC.getGame().selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, COMMAND_LOAD_UNIT, pFirstUnit->getOwner(), pFirstUnit->getID());
		}
		return false;
	}

	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, 0, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}


bool CvDLLButtonPopup::launchLeadUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvSelectionGroup* pSelectionGroup = m_kUI.getSelectionList();
	if (pSelectionGroup == NULL)
		return false;

	CvPlot* pPlot = pSelectionGroup->plot();
	if (pPlot == NULL)
		return false;

	m_kUI.popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_UNIT_TO_LEAD"));

	CvWStringBuffer szBuffer;
	CvUnit* pFirstUnit = NULL;
	int iCount = 1;
	CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pPlot->nextUnitNode(pUnitNode);

		if (pLoopUnit->canPromote((PromotionTypes) info.getData1(), info.getData2()))
		{
			if (!pFirstUnit)
			{
				pFirstUnit = pLoopUnit;
			}
			szBuffer.clear();
			GAMETEXT.setUnitHelp(szBuffer, pLoopUnit, true);
			m_kUI.popupAddGenericButton(pPopup,
					CvWString(szBuffer.getCString()), NULL, iCount, WIDGET_GENERAL);
			iCount++;
		}
	}

	if (iCount <= 2)
	{
		if (pFirstUnit)
		{
			GC.getGame().selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_LEAD, pFirstUnit->getID());
		}
		return false;
	}

	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, 0, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchDoEspionagePopup(CvPopup* pPopup, CvPopupInfo &info)  // advc: style changes
{
	CvUnit* pUnit = m_kUI.getHeadSelectedUnit();
	if (pUnit == NULL)
		return false;

	CvPlot* pPlot = pUnit->plot();
	if (pPlot == NULL)
		return false;

	m_kUI.popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_ESPIONAGE_MISSION"));

	CvWString szBuffer;
	for (int i = 0; i < GC.getNumEspionageMissionInfos(); i++)
	{
		EspionageMissionTypes eLoopMission = (EspionageMissionTypes)i;
		if (GC.getInfo(eLoopMission).isPassive() ||
				!GET_PLAYER(pUnit->getOwner()).canDoEspionageMission(
				eLoopMission, pPlot->getOwner(), pPlot, -1, pUnit))
			continue;

		if (GC.getInfo(eLoopMission).isTwoPhases())
		{
			szBuffer = GC.getInfo(eLoopMission).getDescription();
			m_kUI.popupAddGenericButton(pPopup, szBuffer,
					ARTFILEMGR.getInterfaceArtPath("ESPIONAGE_BUTTON"),
					eLoopMission, WIDGET_GENERAL);
		}
		else
		{
			int iCost = GET_PLAYER(pUnit->getOwner()).getEspionageMissionCost(
					eLoopMission, pPlot->getOwner(), pPlot, -1, pUnit);
			if (iCost > 0)
			{
				szBuffer = gDLL->getText("TXT_KET_ESPIONAGE_MISSION_COST",
						GC.getInfo(eLoopMission).getTextKeyWide(), iCost);
				m_kUI.popupAddGenericButton(pPopup, szBuffer,
						ARTFILEMGR.getInterfaceArtPath("ESPIONAGE_BUTTON"),
						eLoopMission, WIDGET_HELP_ESPIONAGE_COST, eLoopMission, -1);
			}
		}
	}

	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"),
			ARTFILEMGR.getInterfaceArtPath("INTERFACE_BUTTONS_CANCEL"),
			NO_ESPIONAGEMISSION, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchDoEspionageTargetPopup(CvPopup* pPopup, CvPopupInfo &info)  // advc: some style changes
{
	CvUnit* pUnit = m_kUI.getHeadSelectedUnit();
	if (pUnit == NULL)
		return false;
	CvPlot const& kPlot = pUnit->getPlot();
	PlayerTypes eTargetPlayer = kPlot.getOwner();
	if (eTargetPlayer == NO_PLAYER)
		return false;

	EspionageMissionTypes eMission = (EspionageMissionTypes)info.getData1();
	if (eMission == NO_ESPIONAGEMISSION)
		return false;

	m_kUI.popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_ESPIONAGE_CHOOSE_TARGET"));

	CvPlayer& kPlayer = GET_PLAYER(GC.getGame().getActivePlayer());
	CvEspionageMissionInfo& kMission = GC.getInfo(eMission);
	if (kMission.getDestroyBuildingCostFactor() > 0)
	{
		CvCity* pCity = kPlot.getPlotCity();
		if (pCity != NULL)
		{
			for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
			{
				if (kPlayer.canDoEspionageMission(
					eMission, eTargetPlayer, &kPlot, iBuilding, pUnit))
				{
					CvBuildingInfo& kBuilding = GC.getInfo((BuildingTypes)iBuilding);
					if (pCity->getNumRealBuilding((BuildingTypes)iBuilding) > 0)
					{
						int iCost = kPlayer.getEspionageMissionCost(
								eMission, eTargetPlayer, &kPlot, iBuilding, pUnit);
						CvWString szBuffer = gDLL->getText("TXT_KET_ESPIONAGE_MISSION_COST",
								kBuilding.getDescription(), iCost);
						m_kUI.popupAddGenericButton(pPopup, szBuffer, kBuilding.getButton(),
								iBuilding, WIDGET_HELP_ESPIONAGE_COST, eMission, iBuilding);
					}
				}
			}
		}
	}
	else if (kMission.getDestroyUnitCostFactor() > 0)
	{
		CLLNode<IDInfo>* pUnitNode = kPlot.headUnitNode();
		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = kPlot.nextUnitNode(pUnitNode);

			if (NULL != pLoopUnit)
			{
				if (kPlayer.canDoEspionageMission(
					eMission, eTargetPlayer, &kPlot, pLoopUnit->getUnitType(), pUnit))
				{
					if (pLoopUnit->getTeam() == GET_PLAYER(eTargetPlayer).getTeam())
					{
						CvUnitInfo& kUnit = pLoopUnit->getUnitInfo();
						int iCost = kPlayer.getEspionageMissionCost(eMission, eTargetPlayer,
								&kPlot, pLoopUnit->getUnitType(), pUnit);
						CvWString szBuffer = gDLL->getText("TXT_KET_ESPIONAGE_MISSION_COST",
								kUnit.getDescription(), iCost);
						m_kUI.popupAddGenericButton(pPopup, szBuffer,pLoopUnit->getButton(),
								pLoopUnit->getID(), WIDGET_HELP_ESPIONAGE_COST,
								eMission, pLoopUnit->getUnitType());
					}
				}
			}
		}
	}
	else if (kMission.getDestroyProjectCostFactor() > 0)
	{
		for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
		{
			if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, &kPlot, iProject, pUnit))
			{
				if (GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).getProjectCount(
					(ProjectTypes)iProject) > 0)
				{
					int iCost = kPlayer.getEspionageMissionCost(
							eMission, eTargetPlayer, &kPlot, iProject, pUnit);
					CvWString szBuffer = gDLL->getText("TXT_KET_ESPIONAGE_MISSION_COST",
							GC.getInfo((ProjectTypes)iProject).getDescription(), iCost);
					m_kUI.popupAddGenericButton(pPopup, szBuffer,
							GC.getInfo((ProjectTypes)iProject).getButton(), iProject,
							WIDGET_HELP_ESPIONAGE_COST, eMission, iProject);
				}
			}
		}
	}
	else if (kMission.getBuyTechCostFactor() > 0)
	{
		for (int iTech = 0; iTech < GC.getNumTechInfos(); ++iTech)
		{
			if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, &kPlot, iTech, pUnit))
			{
				int iCost = kPlayer.getEspionageMissionCost(
						eMission, eTargetPlayer, &kPlot, iTech, pUnit);
				CvTechInfo& kTech = GC.getInfo((TechTypes)iTech);
				CvWString szBuffer = gDLL->getText("TXT_KET_ESPIONAGE_MISSION_COST",
						kTech.getDescription(), iCost);
				m_kUI.popupAddGenericButton(pPopup, szBuffer, kTech.getButton(),
						iTech, WIDGET_HELP_ESPIONAGE_COST, eMission, iTech);
			}
		}
	}
	else if (kMission.getSwitchCivicCostFactor() > 0)
	{
		for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
		{
			if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, &kPlot, iCivic, pUnit))
			{
				int iCost = kPlayer.getEspionageMissionCost(
						eMission, eTargetPlayer, &kPlot, iCivic, pUnit);
				CvCivicInfo& kCivic = GC.getInfo((CivicTypes)iCivic);
				CvWString szBuffer = gDLL->getText("TXT_KET_ESPIONAGE_MISSION_COST",
						kCivic.getDescription(), iCost);
				m_kUI.popupAddGenericButton(pPopup, szBuffer, kCivic.getButton(),
						iCivic, WIDGET_HELP_ESPIONAGE_COST, eMission, iCivic);
			}
		}
	}
	else if (kMission.getSwitchReligionCostFactor() > 0)
	{
		for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); ++iReligion)
		{
			if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, &kPlot, iReligion, pUnit))
			{
				int iCost = kPlayer.getEspionageMissionCost(
						eMission, eTargetPlayer, &kPlot, iReligion, pUnit);
				CvReligionInfo& kReligion = GC.getInfo((ReligionTypes)iReligion);
				CvWString szBuffer = gDLL->getText("TXT_KET_ESPIONAGE_MISSION_COST",
						kReligion.getDescription(), iCost);
				m_kUI.popupAddGenericButton(pPopup, szBuffer, kReligion.getButton(), iReligion,
						WIDGET_HELP_ESPIONAGE_COST, eMission, iReligion);
			}
		}
	}

	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"),
			ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(),
			NO_ESPIONAGEMISSION, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

// K-Mod. I've changed the code so that my MainMenuOptions enum types are used for the 'generic button' numbers used in this function.
// eg. I've changed this:
//	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXIT_TO_DESKTOP").c_str(), NULL, 0, WIDGET_GENERAL, 0, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
// to this
//	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXIT_TO_DESKTOP").c_str(), NULL, MM_EXIT_TO_DESKTOP, WIDGET_GENERAL, MM_EXIT_TO_DESKTOP, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
//
bool CvDLLButtonPopup::launchMainMenuPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetStyle(pPopup, "Window_NoTitleBar_Style");

	// 288,72
	m_kUI.popupAddDDS(pPopup, "resource/temp/civ4_title_small.dds", 192, 48);

	m_kUI.popupAddSeparator(pPopup);

	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXIT_TO_DESKTOP").c_str(),
			NULL, MM_EXIT_TO_DESKTOP, WIDGET_GENERAL, MM_EXIT_TO_DESKTOP, 0, true,
			POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);

	if (!m_kUI.isDebugMenuCreated())
	{
		m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXIT_TO_MAIN_MENU").c_str(),
				NULL, MM_EXIT_TO_MAIN_MENU, WIDGET_GENERAL, MM_EXIT_TO_MAIN_MENU, 0, true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if (GC.getGame().canDoControl(CONTROL_RETIRE))
	{
		m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_RETIRE").c_str(),
				NULL, MM_RETIRE, WIDGET_GENERAL, MM_RETIRE, 0, true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if (GC.getGame().getElapsedGameTurns() == 0 &&
		!GC.getGame().isGameMultiPlayer() && !GC.getInitCore().getWBMapScript())
	{
		// Don't allow if there has already been diplomacy
		bool bShow = true;
		for (int i = 0; bShow && i < MAX_CIV_TEAMS; i++)
		{
			for (int j = i+1; bShow && j < MAX_CIV_TEAMS; j++)
			{
				if (GET_TEAM((TeamTypes)i).isHasMet((TeamTypes)j))
					bShow = false;
			}
		}
		/* <advc.004j> Commented out.
		   After ending the initial turn, some script data seems to
		   be set by the EXE. Not sure what data that is and whether
		   CvGame::regenerateMap can handle it; experimental for now.
		   Fwiw, I'm resetting all script data in regenerateMap. */
		/*if (bShow) {
			if (!GC.getGame().getScriptData().empty())
				bShow = false;
		}
		if (bShow) {
			for (int i = 0; i < GC.getMap().numPlots(); ++i) {
				CvPlot* pPlot = GC.getMap().plotByIndex(i);
				if (!pPlot->getScriptData().empty()) {
					bShow = false;
					break;
				}
			}
		}*/ // </advc.004j>
		if (bShow)
		{
			m_kUI.popupAddGenericButton(pPopup,
					gDLL->getText("TXT_KEY_POPUP_REGENERATE_MAP").c_str(),
					NULL, MM_REGENERATE_MAP, WIDGET_GENERAL, MM_REGENERATE_MAP, 0,
					true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
		}
	}

	if (GC.getGame().canDoControl(CONTROL_LOAD_GAME))
	{
		m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_LOAD_GAME").c_str(),
				NULL, MM_LOAD_GAME, WIDGET_GENERAL, MM_LOAD_GAME, 0, true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}
	if (GC.getGame().canDoControl(CONTROL_SAVE_NORMAL))
	{
		m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_SAVE_GAME").c_str(),
				NULL, MM_SAVE_GAME, WIDGET_GENERAL, MM_SAVE_GAME, 0, true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_OPTIONS").c_str(),
			NULL, MM_OPTIONS, WIDGET_GENERAL, MM_OPTIONS, 0, true,
			POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	// K-Mod: The BUG options menu!
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_BUG_OPT_TITLE").c_str(),
			NULL, MM_BUG_OPTIONS, WIDGET_GENERAL, MM_BUG_OPTIONS, 0, true,
			POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);

	if (GC.getGame().canDoControl(CONTROL_WORLD_BUILDER))
	{
		m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_ENTER_WB").c_str(),
				NULL, MM_ENTER_WB, WIDGET_GENERAL, MM_ENTER_WB, 0, true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if (GC.getGame().canDoControl(CONTROL_ADMIN_DETAILS))
	{
		m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_GAME_DETAILS").c_str(),
				NULL, MM_GAME_DETAILS, WIDGET_GENERAL, MM_GAME_DETAILS, 0, true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if (GC.getGame().canDoControl(CONTROL_DETAILS))
	{
		m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_DETAILS_TITLE").c_str(),
				NULL, MM_PLAYER_DETAILS, WIDGET_GENERAL, MM_PLAYER_DETAILS, 0, true,
				POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_CANCEL").c_str(),
			NULL, MM_CANCEL, WIDGET_GENERAL, MM_CANCEL, 0, true,
			POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchConfirmMenu(CvPopup *pPopup, CvPopupInfo &info)
{
	/*m_kUI.popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_ARE_YOU_SURE").c_str());
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES").c_str(), NULL, 0, WIDGET_GENERAL);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO").c_str(), NULL, 1, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);*/ // BtS

	// K-Mod. The confirmation box should actually tell the user what they are being asked to confirm!
	// Note: the text here should correspond to the functionality defined by CvDLLButtonPopup::OnOkClicked.
	// (It's unfortunate that there's no enum defined for the different options... but I don't feel like fixing that now.)
	m_kUI.popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_ARE_YOU_SURE").c_str());
	switch (info.getData1())
	{
	case 0:
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_POPUP_EXIT_TO_DESKTOP").c_str(), NULL, 0, WIDGET_GENERAL);
		break;
	case 1:
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_POPUP_EXIT_TO_MAIN_MENU").c_str(), NULL, 0, WIDGET_GENERAL);
		break;
	case 2:
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_POPUP_RETIRE").c_str(), NULL, 0, WIDGET_GENERAL);
		break;
	case 3:
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_POPUP_REGENERATE_MAP").c_str(), NULL, 0, WIDGET_GENERAL);
		break;
	case 4:
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_POPUP_ENTER_WB").c_str(), NULL, 0, WIDGET_GENERAL);
		break;
	default:
		FAssertMsg(false, "launchConfirmMenu called with unknown type");
		return false;
	}
	m_kUI.popupAddGenericButton(pPopup,
			gDLL->getText("TXT_KEY_POPUP_CANCEL").c_str(), NULL, 1, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	// K-Mod end

	return true;
}

bool CvDLLButtonPopup::launchCancelDeal(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetBodyString(pPopup,  gDLL->getText("TXT_KEY_POPUP_CANCEL_DEAL"));
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_CANCEL_DEAL_YES"),
			NULL, 0, WIDGET_GENERAL);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_CANCEL_DEAL_NO"),
			NULL, 1, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return true;
}

bool CvDLLButtonPopup::launchPythonPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetBodyString(pPopup, info.getText());
	for (int i = 0; i < info.getNumPythonButtons(); i++)
	{
		m_kUI.popupAddGenericButton(pPopup,
				info.getPythonButtonText(i), info.getPythonButtonArt(i).IsEmpty() ?
				NULL : info.getPythonButtonArt(i).GetCString(), i);
	}
	m_kUI.popupSetPopupType(pPopup, POPUPEVENT_WARNING,
			ARTFILEMGR.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_WARNING")->getPath());
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return true;
}

bool CvDLLButtonPopup::launchDetailsPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvInitCore const& ic = GC.getInitCore();
	if (!info.getOption1())
	{
		m_kUI.popupSetHeaderString(pPopup,
				gDLL->getText("TXT_KEY_POPUP_DETAILS_TITLE"));

		m_kUI.popupSetBodyString(pPopup,
				gDLL->getText("TXT_KEY_MENU_LEADER_NAME"));
		m_kUI.popupCreateEditBox(pPopup,
				GET_PLAYER(GC.getGame().getActivePlayer()).getName(), WIDGET_GENERAL,
				gDLL->getText("TXT_KEY_MENU_LEADER_NAME"), 0, POPUP_LAYOUT_STRETCH, 0,
				MAX_PLAYERINFO_CHAR_COUNT);
		m_kUI.popupAddSeparator(pPopup);
		m_kUI.popupSetBodyString(pPopup,
				gDLL->getText("TXT_KEY_MENU_CIV_DESC"));
		m_kUI.popupCreateEditBox(pPopup,
				GET_PLAYER(GC.getGame().getActivePlayer()).getCivilizationDescription(),
				WIDGET_GENERAL, gDLL->getText("TXT_KEY_MENU_CIV_DESC"), 1,
				POPUP_LAYOUT_STRETCH, 0, MAX_PLAYERINFO_CHAR_COUNT);
		m_kUI.popupAddSeparator(pPopup);
		m_kUI.popupSetBodyString(pPopup,
				gDLL->getText("TXT_KEY_MENU_CIV_SHORT_DESC"));
		m_kUI.popupCreateEditBox(pPopup,
				GET_PLAYER(GC.getGame().getActivePlayer()).getCivilizationShortDescription(),
				WIDGET_GENERAL, gDLL->getText("TXT_KEY_MENU_CIV_SHORT_DESC"), 2,
				POPUP_LAYOUT_STRETCH, 0, MAX_PLAYERINFO_CHAR_COUNT);
		m_kUI.popupAddSeparator(pPopup);
		m_kUI.popupSetBodyString(pPopup,
				gDLL->getText("TXT_KEY_MENU_CIV_ADJ"));
		m_kUI.popupCreateEditBox(pPopup,
				GET_PLAYER(GC.getGame().getActivePlayer()).getCivilizationAdjective(),
				WIDGET_GENERAL, gDLL->getText("TXT_KEY_MENU_CIV_ADJ"), 3, POPUP_LAYOUT_STRETCH, 0,
				MAX_PLAYERINFO_CHAR_COUNT);
		m_kUI.popupAddSeparator(pPopup);
	}
	else if (!ic.getCivPassword(ic.getActivePlayer()).empty())
	{
		// the purpose of the popup with the option set to true is to ask for the civ password if it's not set
		return false;
	}
	if (GC.getGame().isPbem() || GC.getGame().isHotSeat() || GC.getGame().isPitboss())
	{
		m_kUI.popupSetBodyString(pPopup,
				gDLL->getText("TXT_KEY_MAIN_MENU_PASSWORD"));
		m_kUI.popupCreateEditBox(pPopup,
				PASSWORD_DEFAULT, WIDGET_GENERAL,
				gDLL->getText("TXT_KEY_MAIN_MENU_PASSWORD"), 4,
				POPUP_LAYOUT_STRETCH, 0, MAX_PASSWORD_CHAR_COUNT);
		m_kUI.popupAddSeparator(pPopup);
	}
	if ((GC.getGame().isPitboss() || GC.getGame().isPbem()) && !info.getOption1())
	{
		m_kUI.popupSetBodyString(pPopup,
				gDLL->getText("TXT_KEY_POPUP_DETAILS_EMAIL"));
		m_kUI.popupCreateEditBox(pPopup,
				CvWString(ic.getEmail(ic.getActivePlayer())), WIDGET_GENERAL,
				gDLL->getText("TXT_KEY_POPUP_DETAILS_EMAIL"), 5,
				POPUP_LAYOUT_STRETCH, 0, MAX_PLAYEREMAIL_CHAR_COUNT);
		m_kUI.popupAddSeparator(pPopup);
	}
	if (GC.getGame().isPbem() && !info.getOption1())
	{
		m_kUI.popupSetBodyString(pPopup,
				gDLL->getText("TXT_KEY_POPUP_DETAILS_SMTP"));
		m_kUI.popupCreateEditBox(pPopup,
				CvWString(ic.getSmtpHost(ic.getActivePlayer())), WIDGET_GENERAL,
				gDLL->getText("TXT_KEY_POPUP_DETAILS_SMTP"), 6,
				POPUP_LAYOUT_STRETCH, 0, MAX_PLAYEREMAIL_CHAR_COUNT);
		m_kUI.popupAddSeparator(pPopup);

		if (GC.getGame().getPbemTurnSent())
		{
			m_kUI.popupAddGenericButton(pPopup,
					gDLL->getText("TXT_KEY_MISC_SEND"), NULL, 0, WIDGET_GENERAL);
		}
	}

	// Disable leader name edit box for internet games
	if (ic.getMultiplayer() && gDLL->isFMPMgrPublic())
		m_kUI.popupEnableEditBox(pPopup, 0, false);

	m_kUI.popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return true;
}

bool CvDLLButtonPopup::launchAdminPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetHeaderString(pPopup,
			gDLL->getText("TXT_KEY_POPUP_GAME_DETAILS"));
	m_kUI.popupSetBodyString(pPopup,
			gDLL->getText("TXT_KEY_MAIN_MENU_GAME_NAME"));
	m_kUI.popupCreateEditBox(pPopup,
			GC.getInitCore().getGameName(), WIDGET_GENERAL,
			gDLL->getText("TXT_KEY_MAIN_MENU_GAME_NAME"), 0,
			POPUP_LAYOUT_STRETCH, 0, MAX_GAMENAME_CHAR_COUNT);
	m_kUI.popupAddSeparator(pPopup);
	m_kUI.popupSetBodyString(pPopup,
			gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"));
	m_kUI.popupCreateEditBox(pPopup,
			PASSWORD_DEFAULT, WIDGET_GENERAL,
			gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"), 1,
			POPUP_LAYOUT_STRETCH, 0, MAX_PASSWORD_CHAR_COUNT);
	m_kUI.popupAddSeparator(pPopup);
	if (!GC.getGame().isGameMultiPlayer())
	{
		m_kUI.popupCreateCheckBoxes(pPopup, 1, 2);
		m_kUI.popupSetCheckBoxText(pPopup,
				0, gDLL->getText("TXT_KEY_POPUP_ADMIN_ALLOW_CHEATS"), 2);
		m_kUI.popupSetCheckBoxState(pPopup,
				0, gDLL->getChtLvl() > 0, 2);
	}
	m_kUI.popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return true;
}


bool CvDLLButtonPopup::launchAdminPasswordPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetBodyString(pPopup,
			gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"));
	m_kUI.popupCreateEditBox(pPopup,
			L"", WIDGET_GENERAL, gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"), 0,
			POPUP_LAYOUT_STRETCH, 0, MAX_PASSWORD_CHAR_COUNT);
	m_kUI.popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return true;
}


bool CvDLLButtonPopup::launchExtendedGamePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetHeaderString(pPopup,
			gDLL->getText("TXT_KEY_EXTENDED_GAME_TITLE"));

	if (GC.getGame().countHumanPlayersAlive() > 0)
	{
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_EXTENDED_GAME_YES"), NULL, 0, WIDGET_GENERAL);
	}
	if (!gDLL->UI().isDebugMenuCreated())
	{
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_EXTENDED_GAME_NO_MENU"), NULL, 1, WIDGET_GENERAL);
	}
	else
	{
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_EXTENDED_GAME_NO_DESKTOP"), NULL, 1, WIDGET_GENERAL);
	}

	m_kUI.popupLaunch(pPopup, false);
	return true;
}

bool CvDLLButtonPopup::launchDiplomacyPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_DIPLOMACY_TITLE"));

	int iCount = 0;
	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER(GC.getGame().getActivePlayer()).canContact((PlayerTypes)iI))
			{
				m_kUI.popupAddGenericButton(pPopup,
						GET_PLAYER((PlayerTypes)iI).getName(), NULL, iI, WIDGET_GENERAL);
				iCount++;
			}
		}
	}

	if (iCount == 0)
		return false;

	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"),
			NULL, MAX_CIV_PLAYERS, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return true;
}


bool CvDLLButtonPopup::launchAddBuddyPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_SYSTEM_ADD_BUDDY", info.getText().GetCString()));
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES"), NULL, 0, WIDGET_GENERAL);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO"), NULL, 1, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false);
	return true;
}

bool CvDLLButtonPopup::launchForcedDisconnectPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_MAIN_MENU_FORCED_DISCONNECT_INGAME"));
	m_kUI.popupLaunch(pPopup, true);
	return true;
}

bool CvDLLButtonPopup::launchPitbossDisconnectPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_PITBOSS_DISCONNECT"));
	m_kUI.popupLaunch(pPopup, true);
	return true;
}

bool CvDLLButtonPopup::launchKickedPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_POPUP_KICKED"));
	m_kUI.popupLaunch(pPopup, true);
	return true;
}

bool CvDLLButtonPopup::launchVassalDemandTributePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	if (info.getData1() == NO_PLAYER)
		return false;

	CvGame const& g = GC.getGame();
	CvPlayer& kVassal = GET_PLAYER((PlayerTypes)info.getData1());
	if (!GET_TEAM(kVassal.getTeam()).isVassal(g.getActiveTeam()))
		return false;

	int iNumResources = 0;
	if (kVassal.canTradeNetworkWith(g.getActivePlayer()))
	{
		m_kUI.popupSetBodyString(pPopup,
				gDLL->getText("TXT_KEY_VASSAL_DEMAND_TRIBUTE", kVassal.getNameKey()));
		for (int iBonus = 0; iBonus < GC.getNumBonusInfos(); iBonus++)
		{
			//if (kVassal.getNumTradeableBonuses((BonusTypes)iBonus) > 0 && GET_PLAYER(GC.getGame().getActivePlayer()).getNumAvailableBonuses((BonusTypes)iBonus) == 0)
			// <advc.036> Replacing the above
			if(kVassal.canTradeItem(g.getActivePlayer(), TradeData(
				TRADE_RESOURCES, iBonus), true)) // </advc.036>
			{
				CvBonusInfo& info = GC.getInfo((BonusTypes)iBonus);
				m_kUI.popupAddGenericButton(pPopup,
						info.getDescription(), info.getButton(), iBonus,
						WIDGET_GENERAL, iBonus, -1, true,
						POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
				++iNumResources;
			}
		}
	}

	if (iNumResources > 0)
	{
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_NEVER_MIND"),
				ARTFILEMGR.getInterfaceArtPath("INTERFACE_BUTTONS_CANCEL"),
				GC.getNumBonusInfos(), WIDGET_GENERAL);
	}
	else
	{
		m_kUI.popupAddGenericButton(pPopup,
				gDLL->getText("TXT_KEY_VASSAL_TRIBUTE_NOT_POSSIBLE"),
				ARTFILEMGR.getInterfaceArtPath("INTERFACE_BUTTONS_CANCEL"),
				GC.getNumBonusInfos(), WIDGET_GENERAL);
	}

	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchVassalGrantTributePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	if (info.getData1() == NO_PLAYER)
		return false;

	CvPlayer& kMaster = GET_PLAYER((PlayerTypes)info.getData1());
	if (!GET_TEAM(GC.getGame().getActiveTeam()).isVassal(kMaster.getTeam()))
		return false;

	if (info.getData2() == NO_BONUS)
		return false;

	m_kUI.popupSetBodyString(pPopup,
			gDLL->getText("TXT_KEY_VASSAL_GRANT_TRIBUTE", kMaster.getCivilizationDescriptionKey(),
			kMaster.getNameKey(), GC.getInfo((BonusTypes)info.getData2()).getTextKeyWide()));
	m_kUI.popupAddGenericButton(pPopup,
			gDLL->getText("TXT_KEY_VASSAL_GRANT_TRIBUTE_YES"), NULL, 0, WIDGET_GENERAL);
	m_kUI.popupAddGenericButton(pPopup,
			gDLL->getText("TXT_KEY_VASSAL_GRANT_TRIBUTE_NO"), NULL, 1, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchEventPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayer& kActivePlayer = GET_PLAYER(GC.getGame().getActivePlayer());
	EventTriggeredData* pTriggeredData = kActivePlayer.getEventTriggered(info.getData1());
	if (pTriggeredData == NULL)
		return false;

	if (pTriggeredData->m_eTrigger == NO_EVENTTRIGGER)
		return false;

	CvEventTriggerInfo& kTrigger = GC.getInfo(pTriggeredData->m_eTrigger);

	gDLL->UI().popupSetBodyString(pPopup, pTriggeredData->m_szText);

	bool bEventAvailable = false;
	for (int i = 0; i < kTrigger.getNumEvents(); i++)
	{
		if (GET_PLAYER(GC.getGame().getActivePlayer()).canDoEvent((EventTypes)kTrigger.getEvent(i), *pTriggeredData))
		{
			m_kUI.popupAddGenericButton(pPopup,
					GC.getEventInfo((EventTypes)kTrigger.getEvent(i)).getDescription(),
					GC.getInfo((EventTypes)kTrigger.getEvent(i)).getButton(),
					kTrigger.getEvent(i), WIDGET_CHOOSE_EVENT, kTrigger.getEvent(i), info.getData1());
			bEventAvailable = true;
		}
		else
		{
			m_kUI.popupAddGenericButton(pPopup,
					GC.getInfo((EventTypes)kTrigger.getEvent(i)).getDescription(),
					ARTFILEMGR.getInterfaceArtPath("INTERFACE_EVENT_UNAVAILABLE_BULLET"),
					-1, WIDGET_CHOOSE_EVENT, kTrigger.getEvent(i), info.getData1(), false);
		}
	}

	if (!bEventAvailable)
		return false;

	if (kTrigger.isPickCity())
	{
		CvCity* pCity = kActivePlayer.getCity(pTriggeredData->m_iCityId);
		if (pCity != NULL)
		{
			m_kUI.popupAddGenericButton(pPopup,
					gDLL->getText("TXT_KEY_POPUP_EXAMINE_CITY").c_str(),
					ARTFILEMGR.getInterfaceArtPath("INTERFACE_BUTTONS_CITYSELECTION"),
					GC.getNumEventInfos(), WIDGET_GENERAL, -1, -1);
		}
		else FAssert(pCity != NULL);
	}

	if (kTrigger.isShowPlot())
	{
		CvPlot* pPlot = GC.getMap().plot(pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY);
		if (NULL != pPlot)
		{
			gDLL->getEngineIFace()->addColoredPlot(pPlot->getX(), pPlot->getY(),
					GC.getInfo(GC.getColorType("WARNING_TEXT")).getColor(),
					PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS);
			m_kUI.lookAt(pPlot->getPoint(), CAMERALOOKAT_NORMAL);
		}
	}

	m_kUI.popupLaunch(pPopup, !bEventAvailable, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchFreeColonyPopup(CvPopup* pPopup, CvPopupInfo &info)  // advc: style changes
{
	m_kUI.popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_FREE_COLONY"));

	CvPlayer const& kPlayer = GET_PLAYER(GC.getGame().getActivePlayer());	
	if (kPlayer.canSplitEmpire())
	{
		FOR_EACH_AREA(pLoopArea)
		{
			if (!kPlayer.canSplitArea(*pLoopArea))
				continue;

			CvWString szCityList;
			int iNumCities = 0;
			FOR_EACH_CITY(pLoopCity, kPlayer)
			{
				if (pLoopCity->isArea(*pLoopArea))
				{
					if (!szCityList.empty())
						szCityList += L", ";
					iNumCities++;
					szCityList += pLoopCity->getName();
				}
			}
			CvWString szBuffer = gDLL->getText("TXT_KEY_SPLIT_EMPIRE", szCityList.GetCString());
			m_kUI.popupAddGenericButton(pPopup,
					szBuffer, ARTFILEMGR.getInterfaceArtPath("INTERFACE_BUTTONS_CITYSELECTION"),
					pLoopArea->getID(), WIDGET_GENERAL);
		}
	}
	FOR_EACH_CITY(pLoopCity, kPlayer)
	{
		// UNOFFICIAL_PATCH, Bugfix, 08/04/09, jdog5000
		/*PlayerTypes ePlayer = pLoopCity->getLiberationPlayer(false);
		if (NO_PLAYER != ePlayer)
		{
			CvWString szCity = gDLL->getText("TXT_KEY_CITY_LIBERATE", pLoopCity->getNameKey(), GET_PLAYER(ePlayer).getNameKey());
			m_kUI.popupAddGenericButton(pPopup, szCity, ARTFILEMGR.getInterfaceArtPath("INTERFACE_BUTTONS_CITYSELECTION"), -pLoopCity->getID(), WIDGET_GENERAL);
		}*/ // BtS
		// Avoid potential variable name conflict
		PlayerTypes eLibPlayer = pLoopCity->getLiberationPlayer(false);
		if (eLibPlayer != NO_PLAYER)
		{
			// Don't offer liberation during war
			if (!GET_TEAM(kPlayer.getTeam()).isAtWar(TEAMID(eLibPlayer)))
			{
				CvWString szCity = gDLL->getText("TXT_KEY_CITY_LIBERATE", pLoopCity->getNameKey(), GET_PLAYER(eLibPlayer).getNameKey());
				m_kUI.popupAddGenericButton(pPopup,
						szCity, ARTFILEMGR.getInterfaceArtPath("INTERFACE_BUTTONS_CITYSELECTION"),
						-pLoopCity->getID(), WIDGET_GENERAL);
			}
		} // UNOFFICIAL_PATCH: END
	}

	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"),
			ARTFILEMGR.getInterfaceArtPath("INTERFACE_BUTTONS_CANCEL"), 0, WIDGET_GENERAL);
	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchLaunchPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	VictoryTypes eVictory = (VictoryTypes)info.getData1();
	if (NO_VICTORY == eVictory)
		return false;

	CvTeam const& kTeam = GET_TEAM(GC.getGame().getActiveTeam()); // advc
	// K-Mod. Cancel the popup if something has happened to prevent the launch.
	if (!kTeam.canLaunch(eVictory))
		return false;
	// K-Mod end

	if (kTeam.getVictoryCountdown(eVictory) > 0 || GC.getGame().getGameState() != GAMESTATE_ON)
		return false;

	CvWString szDate;
	GAMETEXT.setTimeStr(szDate, GC.getGame().getGameTurn() + kTeam.getVictoryDelay(eVictory), false);

	m_kUI.popupSetHeaderString(pPopup, GC.getInfo(eVictory).getCivilopedia());
	//m_kUI.popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_ESTIMATED_VICTORY_DATE", szDate.GetCString()));
	// K-Mod. Include number of turns, and success probability.
	m_kUI.popupSetBodyString(pPopup,
	gDLL->getText("TXT_KEY_SPACE_SHIP_SCREEN_TRAVEL_TIME_LABEL", kTeam.getVictoryDelay(eVictory)) + NEWLINE +
	gDLL->getText("TXT_KEY_ESTIMATED_VICTORY_DATE", szDate.GetCString()) + NEWLINE +
	gDLL->getText("TXT_KEY_SPACESHIP_CHANCE_OF_SUCCESS", kTeam.getLaunchSuccessRate(eVictory)));
	// K-Mod end

	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES"), NULL, 0, WIDGET_GENERAL);
	m_kUI.popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO"), NULL, 1, WIDGET_GENERAL);

	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchFoundReligionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	m_kUI.popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_FOUNDED_RELIGION"));

	bool bFound = false;
	for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); iReligion++)
	{
		CvReligionInfo& kReligion = GC.getInfo((ReligionTypes)iReligion);
		if (!GC.getGame().isReligionFounded((ReligionTypes)iReligion))
		{
			m_kUI.popupAddGenericButton(pPopup,
					kReligion.getDescription(), kReligion.getButton(), iReligion, WIDGET_GENERAL);
			bFound = true;
		}
	}

	if (!bFound)
		return false;

	m_kUI.popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}
