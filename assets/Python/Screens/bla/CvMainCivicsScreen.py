## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import string
import CvScreensInterface
from LayoutDict import gRect # advc.002b
import CvCivicsScreen
# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvMainCivicsScreen:
	"Goverment Screen"

	def __init__(self):
		self.SCREEN_NAME = "GovermentScreen"
		self.CANCEL_NAME = "GovermentCancel"
		self.EXIT_NAME = "GovermentExit"
		self.TITLE_NAME = "GovermentTitleHeader"
		self.BUTTON_NAME = "GovermentScreenButton"
		self.TEXT_NAME = "GovermentScreenText"
		self.AREA_NAME = "GovermentScreenArea"
		self.HELP_AREA_NAME = "GovermentScreenHelpArea"
		self.HELP_IMAGE_NAME = "GovermentScreenCivicOptionImage"
		self.DEBUG_DROPDOWN_ID =  "GovermentDropdownWidget"
		self.BACKGROUND_ID = "GovermentBackground"
		self.HELP_HEADER_NAME = "GovermentScreenHeaderName"

		self.HEADINGS_WIDTH = 174 #199 #192 #176 #199 #171
		self.HEADINGS_TOP = 50 #70
		self.HEADINGS_SPACING = -4 #5 #0
		self.HEADINGS_BOTTOM = 330 #285 #305 #280
		self.HELP_TOP = 325 #350 #370 # 300
		self.HELP_BOTTOM = 680 #700 #610
		self.TEXT_MARGIN = 14 #15
		self.BUTTON_SIZE = 24
		self.BIG_BUTTON_SIZE = 44 #64
		self.BOTTOM_LINE_TOP = 675 #700 #630 #700
		self.BOTTOM_LINE_WIDTH = 1370 #6 * self.HEADINGS_WIDTH + 5 * self.HEADINGS_SPACING #990 #693 #1014 ##1024
		self.BOTTOM_LINE_HEIGHT = 40 #60 # 60

		self.X_EXIT = 1300 #994
		self.Y_EXIT = 726 #726 ##715

		self.X_CANCEL = 750 #552
		self.Y_CANCEL = 726 #726

		self.X_SCREEN = 200
		self.Y_SCREEN = 396
		self.W_SCREEN = 1360 #1124 #1024 #SCREEN WIDTH
		self.H_SCREEN = 768
		self.Z_SCREEN = -6.1
		self.Y_TITLE = 8	
		#KELDATH CHANGE	
		self.Z_TEXT = self.Z_SCREEN - 0.2 + 10

		self.CivicsScreenInputMap = {
			# self.BUTTON_NAME		: self.CivicsButton,
			# self.TEXT_NAME			: self.CivicsButton,
			self.EXIT_NAME			: self.Revolution,
			self.CANCEL_NAME		: self.Cancel,
			}

		self.iActivePlayer = -1

		self.m_paeCurrentCivics = []
		self.m_paeDisplayCivics = []
		self.m_paeOriginalCivics = []

		# doto civics start:
		self.m_allParentsCivics = []
		self.m_allChildCivics = {}

		self.m_allCurrentCivics = {}

		#self.m_highLighterParent = 0
		self.GOV_CIVIC_HEADER = 480
		self.PANEL_BOX_ADJUSTER = 100
		self.TEXT_BOX_SIZE = 150
		self.TEXT_BOX_WIDTH = self.HEADINGS_WIDTH + self.PANEL_BOX_ADJUSTER * 2 + self.PANEL_BOX_ADJUSTER/2 + 160
		self.TEXT_BOX_SEPERATOR = 5
		self.TEXT_BOX_Y_START = 240
		# doto civics end:

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)

	def setActivePlayer(self, iPlayer):

		self.iActivePlayer = iPlayer
		activePlayer = gc.getPlayer(iPlayer)

		self.m_paeCurrentCivics = []
		self.m_paeDisplayCivics = []
		self.m_paeOriginalCivics = []
		# doto civics
		# self.m_paePrevDisplayCivics = [] # future for comparison in the screen
		self.m_allChildCivics = {}
		for i in range (gc.getNumCivicOptionInfos()):
			self.m_paeCurrentCivics.append(activePlayer.getCivics(i));
			self.m_paeDisplayCivics.append(activePlayer.getCivics(i));
			self.m_paeOriginalCivics.append(activePlayer.getCivics(i));
			# self.m_paePrevDisplayCivics.append(activePlayer.getCivics(i));
			self.m_allChildCivics[i] = [];
			# doto civics end
		
	def interfaceScreen (self, civics=None):

		#doto 115 test
		CvUtil.pyPrint("sagis prop hell( %s )" %(civics))

		screen = self.getScreen()
		if screen.isActive():
			return
		screen.setRenderInterfaceOnly(True);
		screen.showScreen( PopupStates.POPUPSTATE_IMMEDIATE, False)
	
		# Set the background and exit button, and show the screen
		#Doto KELDATH - POSITION THE SCREEN - originally was for 7 civic columns
		screen.setDimensions(screen.centerX(-170), screen.centerY(0), self.W_SCREEN, self.H_SCREEN)
		screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addPanel( "TechTopPanel", u"", u"", True, False, 0, 0, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_TOPBAR )
		screen.addPanel( "TechBottomPanel", u"", u"", True, False, 0, 713, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_BOTTOMBAR )
		screen.showWindowBackground(False)
		screen.setText(self.CANCEL_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_SCREEN_CANCEL", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_CANCEL+10, self.Y_CANCEL+5, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

		# Header...
		screen.setText(self.TITLE_NAME, "Background",u"<font=3b>" + u"<color=205,180,55,255>%s</color>" % "Goverments Dependancy by Keldath"  + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, self.GOV_CIVIC_HEADER, self.Y_TITLE-5, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)		
		self.setActivePlayer(gc.getGame().getActivePlayer())						

		if (CyGame().isDebugMode()):
			self.szDropdownName = self.DEBUG_DROPDOWN_ID
			screen.addDropDownBoxGFC(self.szDropdownName, 22, 12, 300, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for j in range(gc.getMAX_PLAYERS()):
				if (gc.getPlayer(j).isAlive()):
					screen.addPullDownString(self.szDropdownName, gc.getPlayer(j).getName(), j, j, False )

		screen.addPanel("CivicsBottomLine", "", "", True, True, self.HEADINGS_SPACING, self.BOTTOM_LINE_TOP, self.BOTTOM_LINE_WIDTH, self.BOTTOM_LINE_HEIGHT, PanelStyles.PANEL_STYLE_MAIN)

		# Draw Contents
		self.drawContents()

		return 0


	def update(self, fDelta):
		return


	def drawContents(self):
		
		# Update Maintenance/anarchy/etc.
		self.updateAnarchy()
	

	# Will Update the maintenance/anarchy/etc
	def updateAnarchy(self):

		screen = self.getScreen()

		activePlayer = gc.getPlayer(self.iActivePlayer)

		bChange = False
		i = 0
		while (i  < gc.getNumCivicOptionInfos() and not bChange):
			#if self.is_parent_child_goverment_civic(i):
			# doto, doesnt matter if its gov or not anarchy is anarchy
			if (self.m_paeCurrentCivics[i] != self.m_paeOriginalCivics[i]):
				bChange = True
			i += 1		
		
		# Make the revolution button
		screen.deleteWidget(self.EXIT_NAME)
		if (activePlayer.canRevolution(0) and bChange):
			screen.setText(self.EXIT_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_CONCEPT_REVOLUTION", ( )).upper() + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_REVOLUTION, 1, 0)
			screen.show(self.CANCEL_NAME)
		else:
			screen.setText(self.EXIT_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ( )).upper() + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, -1)
			screen.hide(self.CANCEL_NAME)

		# Anarchy
		iTurns = activePlayer.getCivicAnarchyLength(self.m_paeDisplayCivics);

		if (activePlayer.canRevolution(0)):
			szText = localText.getText("TXT_KEY_ANARCHY_TURNS", (iTurns, ))
		else:
			szText = CyGameTextMgr().setRevolutionHelp(self.iActivePlayer)
		#DOTO - 7 screen column KELDATH CHANGES
		screen.setLabel("CivicsRevText", "Background", u"<font=3>" + szText + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 700, self.BOTTOM_LINE_TOP + 5 + self.TEXT_MARGIN/2, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Maintenance		
		#szText = localText.getText("TXT_KEY_CIVIC_SCREEN_UPKEEP", (activePlayer.getCivicUpkeep(self.m_paeDisplayCivics, True), ))
		szText = localText.getText("TXT_KEY_CIVIC_SCREEN_UPKEEP", (activePlayer.getCivicUpkeep(self.m_paeDisplayCivics, True)*(100+activePlayer.calculateInflationRate())/100, )) # K-Mod
		#doto 7 column KELDATH CHANGE
		screen.setLabel("CivicsUpkeepText", "Background", u"<font=3>" + szText + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 100, self.BOTTOM_LINE_TOP - 6 + self.BOTTOM_LINE_HEIGHT - 2 * self.TEXT_MARGIN, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
	

	# Revolution!!!
	def Revolution(self, inputClass):

		activePlayer = gc.getPlayer(self.iActivePlayer)

		allSelectedCivics = []
		for i in range (gc.getNumCivicOptionInfos()):
			if self.m_allCurrentCivics.has_key(i):
				allSelectedCivics.append(m_allCurrentCivics[i])
			else:	
				allSelectedCivics.append(activePlayer.getCivics(i));

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			# advc.001d: Clause added to prevent revolution when viewing another civ's civics through the Debug menu
			if self.iActivePlayer == gc.getGame().getActivePlayer() and activePlayer.canRevolution(0):
				messageControl = CyMessageControl()
				messageControl.sendUpdateCivics(allSelectedCivics)
			screen = self.getScreen()
			screen.hideScreen()

		
		civicScreen = CvCivicsScreen.CvCivicsScreen()
		def showCivicsScreen():
			if (-1 != CyGame().getActivePlayer()):
				civicScreen.interfaceScreen()
		showCivicsScreen()


	def Cancel(self, inputClass):
		screen = self.getScreen()
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			# doto civics parent start
			# gotta rest all params
			activePlayer = gc.getPlayer(gc.getGame().getActivePlayer())
			self.m_allParentsCivics = []
			#self.m_highLighterParent = 0
			self.m_allChildCivics = {}
			# doto civics parent end
			for i in range (gc.getNumCivicOptionInfos()):
				self.m_paeCurrentCivics[i] = self.m_paeOriginalCivics[i]
				self.m_paeDisplayCivics[i] = self.m_paeOriginalCivics[i]
				# doto civics parent start - im gonna assume my mod has 1 parent...
				#if gc.getCivicOptionInfo(i).getParentCivicOption() == 2:
				#	self.m_highLighterParent = activePlayer.getCivics(i);
				self.m_allChildCivics[i] = [];
				# doto civics parent end


	# Will handle the input for this screen...
	def handleInput(self, inputClass):

		CvUtil.pyPrint("sagi handleInput( %s )" %(inputClass.getFunctionName()))
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			screen = self.getScreen()
			iIndex = screen.getSelectedPullDownID(self.DEBUG_DROPDOWN_ID)
			self.setActivePlayer(screen.getPullDownData(self.DEBUG_DROPDOWN_ID, iIndex))
			self.drawContents()

			return 1
		elif (self.CivicsScreenInputMap.has_key(inputClass.getFunctionName())):
			'Calls function mapped in CvGevernmentScreen'
			# only get from the map if it has the key

			# get bound function from map and call it
			self.CivicsScreenInputMap.get(inputClass.getFunctionName())(inputClass)

			# CvUtil.pyPrint("sagi( %s )" %(civicScreen.m_paeChangedCivics))
			# for i in civicScreen.m_paeChangedCivics.keys():
			# 	m_allCurrentCivics[i] = civicScreen.m_paeChangedCivics[i]
			# CvUtil.pyPrint("sagiss( %s )" %(civicScreen.m_paeChangedCivics))
			return 1
		return 0

