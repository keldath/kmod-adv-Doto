## Basic Limited Religions v1.3a by modifieda4 2011

from CvPythonExtensions import *
import CvUtil
import CvScreensInterface
import CvDebugTools
import CvWBPopups
import PyHelpers
import Popup as PyPopup
import CvCameraControls
import CvTopCivs
import sys
import CvWorldBuilderScreen
import CvAdvisorUtils
import CvTechChooser
import pickle
import BugCore
import BugUtil
import SdToolkit

gc = CyGlobalContext()
localText = CyTranslator()
PyPlayer = PyHelpers.PyPlayer
PyInfo = PyHelpers.PyInfo
PyCity = PyHelpers.PyCity
PyGame = PyHelpers.PyGame

class BasicLimitedReligionsEvent:

	def __init__(self, eventMgr):
		self.eventMgr = eventMgr
        	eventMgr.addEventHandler("techAcquired", self.onTechAcquired)
			

	def onTechAcquired(self, argsList):
		'Tech Acquired'
		iTechType, iTeam, iPlayer, bAnnounce = argsList
		# Note that iPlayer may be NULL (-1) and not a refer to a player object
		#CyInterface().addMessage(CyGame().getActivePlayer(),True,25,'%s was finished by Player %d' %(PyInfo.TechnologyInfo(iTechType).getDescription(), iPlayer),'AS2D_DISCOVERBONUS',1,'Art/Interface/Buttons/TerrainFeatures/Forest.dds',ColorTypes(8),0,0,False,False)
		#Check if religious tech
		bRTech = False
		if iTechType == gc.getInfoTypeForString("TECH_MONOTHEISM"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_JUDAISM")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True
		if iTechType == gc.getInfoTypeForString("TECH_THEOLOGY"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_CHRISTIANITY")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True
		if iTechType == gc.getInfoTypeForString("TECH_DIVINE_RIGHT"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_ISLAM")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True
		if iTechType == gc.getInfoTypeForString("TECH_MEDITATION"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_HINDUISM")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True
		if iTechType == gc.getInfoTypeForString("TECH_POLYTHEISM"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_BUDDHISM")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True
		if iTechType == gc.getInfoTypeForString("TECH_PHILOSOPHY"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_CONFUCIANISM")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True
		if iTechType == gc.getInfoTypeForString("TECH_DRAMA"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_TAOISM")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True	
		if iTechType == gc.getInfoTypeForString("TECH_HORSEBACK_RIDING"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_ODIN")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True	
		if iTechType == gc.getInfoTypeForString("TECH_LITERATURE"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_SHINTO")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True
		if iTechType == gc.getInfoTypeForString("TECH_WRITING"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_ZOROASTRIANISM")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True
		if iTechType == gc.getInfoTypeForString("TECH_CALENDAR"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_DRUIDISM")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True
		if iTechType == gc.getInfoTypeForString("TECH_PAPER"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_HELLENISM")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True
		if iTechType == gc.getInfoTypeForString("TECH_POTTERY"):
			iRelCheck = gc.getInfoTypeForString("RELIGION_AMON_RA")
			if not gc.getGame().isReligionSlotTaken(iRelCheck):
				bRTech = True	
	
		#Check if player already has Holy City
			
		bHolyCity = False
		lCities = PyPlayer(iPlayer).getCityList()
		for iCity in range(len(lCities)):
			pCity = gc.getPlayer(iPlayer).getCity(lCities[iCity].getID())
			for iReligionLoop in range( gc.getNumReligionInfos()):
				if pCity.isHolyCityByType(iReligionLoop):
					bHolyCity = True
		

		if bRTech == True:			
			if bHolyCity == False:
				if CyGame().isOption(GameOptionTypes.GAMEOPTION_PICK_RELIGION):
					if gc.getPlayer(iPlayer).isHuman():
						return 
						#old code, not needed: self.doPickReligionPopup(iPlayer,iRelCheck)						
					else:
						iNewReligion = self.AI_chooseReligion(iPlayer)
						if iNewReligion > -1:
							gc.getPlayer(iPlayer).foundReligion(iNewReligion,iRelCheck,True)
				else:
					for i in range(gc.getNumReligionInfos()):
						if gc.getReligionInfo(i).getTechPrereq() == iTechType: 
							if gc.getGame().isReligionFounded(i) == False:
								gc.getPlayer(iPlayer).foundReligion(i,i,True)				
		CvUtil.pyPrint('%s was finished by Team %d' 
			%(PyInfo.TechnologyInfo(iTechType).getDescription(), iTeam))

	def AI_chooseReligion(self, iPlayer):
		pPlayer = gc.getPlayer(iPlayer)
		eFavorite = gc.getLeaderHeadInfo(pPlayer.getLeaderType()).getFavoriteReligion()
		if eFavorite > -1 and not CyGame().isReligionFounded(eFavorite):
			return eFavorite

		aeReligions = list()
		for i in range(gc.getNumReligionInfos()):
			if not CyGame().isReligionFounded(i):
				aeReligions.append(i)

		if len(aeReligions) > 0:
			return aeReligions[CyGame().getSorenRandNum(len(aeReligions), "AI pick religion")]
		return -1

	def doPickReligionPopup(self, iPlayer, religionSlot):
		pPlayer = gc.getPlayer(iPlayer)

		popupInfo = CyPopupInfo()
		popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
		popupInfo.setData1(iPlayer)
		popupInfo.setData2(religionSlot)
		popupInfo.setOnClickedPythonCallback("foundReligion")

		popupInfo.setText(CyTranslator().getText(("TXT_KEY_FOUNDED_RELIGION"),()))

		for i in range(gc.getNumReligionInfos()):
			if not CyGame().isReligionFounded(i):
				popupInfo.addPythonButton(gc.getReligionInfo(i).getDescription(), gc.getReligionInfo(i).getButton())
		popupInfo.addPopup(0)

	
def doHolyCity():

	return True
		

def doHolyCityTech(argsList):
	eTeam = argsList[0]
	ePlayer = argsList[1]
	eTech = argsList[2]
	bFirst = argsList[3]
    
	iPlayer =  argsList[1]

	apCityList = PyPlayer(iPlayer).getCityList()
	for pCity in apCityList:
		iReligionLoop=0				
		for iReligionLoop in range(gc.getNumReligionInfos()):
			if pCity.isHolyCityByType(iReligionLoop):
				#BugUtil.alert("holy city found owned by player %s", ePlayer)
				return True
					
		if iReligionLoop == 0:
			return False
