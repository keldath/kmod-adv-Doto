## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

# globals
gc = CyGlobalContext()

class BarbCiv:
		
	def __init__(self):
		self.iRagingMultiplier	= 2	## Increased Chance for Raging Barbs
		self.iTechPercent		= 25	## 25% Progress Per Team with Tech
		self.iExtraBuildings		= 2	## Extra Buildings Per City, increased with Era
		self.iNumDefender		= 2	## Number of Free Defenders per City
		self.iNumWorker		= 1	## Number of Free Workers per City
		self.iPopulationBoost	= 0	## Number of Extra Population per City
		self.iCapitalBonus		= 1	## Number of Extra XXX in Capital
		self.iWorker		= gc.getInfoTypeForString("UNITCLASS_WORKER")

	def checkBarb(self):
		iBarbPlayer = gc.getBARBARIAN_PLAYER()
		pBarbPlayer = gc.getPlayer(iBarbPlayer)
		iPopulation = pBarbPlayer.getTotalPopulation()
		iCities = pBarbPlayer.getNumCities()
		iChance = iPopulation * iCities
		if CyGame().isOption(GameOptionTypes.GAMEOPTION_RAGING_BARBARIANS):
			iChance *= self.iRagingMultiplier
		if CyGame().getSorenRandNum(100, "Chosen Plot") < iChance:
			iNewPlayer = self.getNewID()
			if iNewPlayer == -1: return
			pNewPlayer = gc.getPlayer(iNewPlayer)
			if pNewPlayer.isEverAlive():
				self.spawnBarbCities(iNewPlayer, pBarbPlayer)
			else:
				iNewCiv = -1
				lCiv = []
				for iCiv in xrange(gc.getNumCivilizationInfos()):
					if CyGame().isCivEverActive(iCiv): continue
					if gc.getCivilizationInfo(iCiv).isPlayable():
						lCiv.append(iCiv)
				if len(lCiv) > 0:
					iNewCiv = lCiv[CyGame().getSorenRandNum(len(lCiv), "Chose Civ")]
				if iNewCiv == -1: return

				iNewLeader = -1
				lLeader = []
				for iLeader in xrange(gc.getNumLeaderHeadInfos()):
					if CyGame().isLeaderEverActive(iLeader): continue
					if iLeader == gc.getDefineINT("BARBARIAN_LEADER"): continue
					if not CyGame().isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV):
						if not gc.getCivilizationInfo(iNewCiv).isLeaders(iLeader): continue
					lLeader.append(iLeader)
				if len(lLeader) > 0:
					iNewLeader = lLeader[CyGame().getSorenRandNum(len(lLeader), "Chose Leader")]
				if iNewLeader == -1: return

				CyGame().addPlayer(iNewPlayer, iNewLeader, iNewCiv)
				self.spawnBarbCities(iNewPlayer, pBarbPlayer)

	def spawnBarbCities(self, iPlayer, pBarbPlayer):
		pPlayer = gc.getPlayer(iPlayer)
		iTeam = pPlayer.getTeam()
		pTeam = gc.getTeam(iTeam)
		(pCity, iter) = pBarbPlayer.firstCity(false)
		while(pCity):
			pPlayer.acquireCity(pCity, False, False)
			(pCity, iter) = pBarbPlayer.nextCity(iter, False)
		(pUnit, iter) = pBarbPlayer.firstUnit(false)
		while(pUnit):
			if not pUnit.isAnimal():
				iUnitType = gc.getCivilizationInfo(pPlayer.getCivilizationType()).getCivilizationUnits(pUnit.getUnitClassType())
				if iUnitType == -1:
					iUnitType = pUnit.getUnitType()
				pNewUnit = pPlayer.initUnit(iUnitType, pUnit.getX(), pUnit.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
				pNewUnit.convert(pUnit)
			(pUnit, iter) = pBarbPlayer.nextUnit(iter, False)
		CyInterface().addImmediateMessage(CyTranslator().getText("TXT_NEW_BARBIE", (pPlayer.getName(),)), "")

		## Grant Techs ##
		for iTech in xrange(gc.getNumTechInfos()):
			if pTeam.isHasTech(iTech): continue
			if pPlayer.canEverResearch(iTech):
				iCost = pTeam.getResearchCost(iTech)
				if gc.getTeam(gc.getBARBARIAN_TEAM()).isHasTech(iTech):
					pTeam.changeResearchProgress(iTech, iCost - pTeam.getResearchProgress(iTech), iPlayer)
					continue
				for iTeamX in xrange(gc.getMAX_CIV_TEAMS()):
					pTeamX = gc.getTeam(iTeamX)
					if pTeamX.isAlive() and pTeamX.isHasTech(iTech):
						iChange = min((self.iTechPercent * iCost / 100), (iCost - pTeam.getResearchProgress(iTech)))
						pTeam.changeResearchProgress(iTech, iChange, iPlayer)

		## Grant Buildings and Units ##
		(pCity, iter) = pPlayer.firstCity(False)
		while(pCity):
			iCapitalBonus = max(0, (pCity.isCapital() * self.iCapitalBonus))
			for i in xrange(pPlayer.getCurrentEra() + self.iExtraBuildings + iCapitalBonus):
				RandBuilding = []
				for iBuildingClass in xrange(gc.getNumBuildingClassInfos()):
					if isLimitedWonderClass(iBuildingClass): continue
					iBuilding = gc.getCivilizationInfo(pPlayer.getCivilizationType()).getCivilizationBuildings(iBuildingClass)
					if iBuilding == -1: continue
					if pCity.canConstruct(iBuilding, True, True, False):
						RandBuilding.append(iBuilding)
				if len(RandBuilding) > 0:
					pCity.pushOrder(OrderTypes.ORDER_CONSTRUCT, RandBuilding[CyGame().getSorenRandNum(len(RandBuilding), "Free Building")] , -1, False, False, False, True)
					pCity.popOrder(0, True, False)
				else:
					break

			iWorker = gc.getCivilizationInfo(pPlayer.getCivilizationType()).getCivilizationUnits(self.iWorker)
			if iWorker > -1:
				for i in xrange(self.iNumWorker + iCapitalBonus):
					pNewUnit = pPlayer.initUnit(iWorker, pCity.getX(), pCity.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			iDefender = pCity.getConscriptUnit()
			if iDefender > -1:
				for i in xrange(self.iNumDefender + iCapitalBonus):
					pNewUnit = pPlayer.initUnit(iDefender, pCity.getX(), pCity.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
			pCity.changePopulation(max(0, (self.iPopulationBoost + iCapitalBonus)))
			(pCity, iter) = pPlayer.nextCity(iter, False)

	def getNewID(self):
		lEverAlive = []
		for iPlayerX in xrange(gc.getMAX_CIV_PLAYERS()):
			pPlayerX = gc.getPlayer(iPlayerX)
			if pPlayerX.isAlive(): continue
			if pPlayerX.isEverAlive():
				if gc.getTeam(pPlayerX.getTeam()).isAlive(): continue
				lEverAlive.append(iPlayerX)
			else:
				return iPlayerX
		if len(lEverAlive) > 0:
			return lEverAlive[CyGame().getSorenRandNum(len(lEverAlive), "Chose ID")]
		return -1