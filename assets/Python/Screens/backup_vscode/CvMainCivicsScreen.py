## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import string
import CvScreensInterface
from LayoutDict import gRect # advc.002b

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvCivicsScreen:
	"Goverment Screen"

	def __init__(self):
		self.SCREEN_NAME = "CivicsScreen"
		self.CANCEL_NAME = "CivicsCancel"
		self.EXIT_NAME = "CivicsExit"
		self.REGULAR_CIVICS_BTN_NAME = "CivicsRegularScreen"
		self.GOVERNMENT_BTN_NAME = "CivicsGovernmentScreen"
		self.TITLE_NAME = "CivicsTitleHeader"
		self.BUTTON_NAME = "CivicsScreenButton"
		self.TEXT_NAME = "CivicsScreenText"
		self.AREA_NAME = "CivicsScreenArea"
		self.HELP_AREA_NAME = "CivicsScreenHelpArea"
		self.HELP_IMAGE_NAME = "CivicsScreenCivicOptionImage"
		self.DEBUG_DROPDOWN_ID =  "CivicsDropdownWidget"
		self.BACKGROUND_ID = "CivicsBackground"
		self.HELP_HEADER_NAME = "CivicsScreenHeaderName"

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

		self.CIVIC_LIST_PANEL_WIDTH = 180

		self.CivicsScreenInputMap = {
			# self.BUTTON_NAME		: self.CivicsButton,
			# self.TEXT_NAME			: self.CivicsButton,
			self.EXIT_NAME			: self.Revolution,
			self.CANCEL_NAME		: self.Cancel,
			self.REGULAR_CIVICS_BTN_NAME: self.Civics,
			self.GOVERNMENT_BTN_NAME: self.Governments
		}

		self.iActivePlayer = -1

		self.m_paeCurrentCivics = []
		self.m_paeDisplayCivics = []
		self.m_paeOriginalCivics = []

		self.GOV_CIVIC_HEADER = 480
		self.PANEL_BOX_ADJUSTER = 100
		self.TEXT_BOX_SIZE = 150
		self.TEXT_BOX_WIDTH = self.HEADINGS_WIDTH + self.PANEL_BOX_ADJUSTER * 2 + self.PANEL_BOX_ADJUSTER/2 + 160
		self.TEXT_BOX_SEPERATOR = 5
		self.TEXT_BOX_Y_START = 240

		self.allDeliveredCivics = []
		# doto civics end:

		self.invokecivics = False

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.CIVICS_SCREEN)

	def setActivePlayer(self, iPlayer, civics):

		self.iActivePlayer = iPlayer
		activePlayer = gc.getPlayer(iPlayer)

		self.m_paeCurrentCivics = []
		self.m_paeDisplayCivics = []
		self.m_paeOriginalCivics = []
		self.allDeliveredCivics = []

		# doto civics
		for i in range (gc.getNumCivicOptionInfos()):
			if civics:		
				self.m_paeCurrentCivics.append(civics[i]);
				self.m_paeDisplayCivics.append(civics[i]);
				self.allDeliveredCivics.append(civics[i]);
			else:
				self.m_paeCurrentCivics.append(activePlayer.getCivics(i));
				self.m_paeDisplayCivics.append(activePlayer.getCivics(i));
				self.allDeliveredCivics.append(activePlayer.getCivics(i));

			self.m_paeOriginalCivics.append(activePlayer.getCivics(i));

		# doto civics end
		
	def interfaceScreen (self, civics=None, source=None):

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
		screen.setText(self.TITLE_NAME, "Background",u"<font=4b>" + u"<color=205,180,55,255>%s</color>" % "MAIN CIVICS"  + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, self.GOV_CIVIC_HEADER, self.Y_TITLE-5, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)		
		
		# doto -> lets save the civic that was chosen in the civic or gov screens
		self.setActivePlayer(gc.getGame().getActivePlayer(), civics)
		# doto end


		if (CyGame().isDebugMode()):
			self.szDropdownName = self.DEBUG_DROPDOWN_ID
			screen.addDropDownBoxGFC(self.szDropdownName, 22, 12, 300, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
			for j in range(gc.getMAX_PLAYERS()):
				if (gc.getPlayer(j).isAlive()):
					screen.addPullDownString(self.szDropdownName, gc.getPlayer(j).getName(), j, j, False )

		screen.addPanel("CivicsBottomLine", "", "", True, True, self.HEADINGS_SPACING, self.BOTTOM_LINE_TOP + 5, self.BOTTOM_LINE_WIDTH, self.BOTTOM_LINE_HEIGHT, PanelStyles.PANEL_STYLE_MAIN)
	
		# Draw Contents
		self.drawContents()

		return 0


	def update(self, fDelta):
		return


	def drawContents(self):
		
		# Update Maintenance/anarchy/etc.
		self.updateAnarchy()
		self.drawAllButtons(True)
		self.drawAllButtons(False)
		self.drawAllHelpText(True)
		self.drawAllHelpText(False)
		self.drawcCivicsScreenButtons()
	
	# Will draw the radio buttons (and revolution)
	def drawAllButtons(self, goverment):				
		
		reindex = 0
		for i in range(gc.getNumCivicOptionInfos()):
#doto 115 goverment screen
			check_parent = self.is_parent_child_goverment_civic(i)
			if goverment:
				draw_boxes = check_parent
			else:
				draw_boxes = not check_parent

			if draw_boxes :
#doto 115 goverment screen
				#fX = self.HEADINGS_SPACING + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * i
				# <advc.002b>
				iDeltaWidths = self.HEADINGS_WIDTH - self.CIVIC_LIST_PANEL_WIDTH
				fX = iDeltaWidths / 2 + self.HEADINGS_SPACING + (self.CIVIC_LIST_PANEL_WIDTH + self.HEADINGS_SPACING + iDeltaWidths) * reindex #i # </advc.002b>
				
				if goverment:
					fY = self.HEADINGS_TOP
				else:
					fY = self.HEADINGS_TOP + 315
				
				szAreaID = self.AREA_NAME + str(reindex) + str(goverment)
				screen = self.getScreen()
				screen.addPanel(szAreaID , "", "", True, True,
						# advc.002b: CIVIC_LIST_PANEL_WIDTH instead of HEADINGS_WIDTH
						fX, fY, self.CIVIC_LIST_PANEL_WIDTH, 80,
						PanelStyles.PANEL_STYLE_MAIN)
	
				screen.setLabel("", "Background", 
						u"<font=3>" + gc.getCivicOptionInfo(i).getDescription().upper() + u"</font>",
						CvUtil.FONT_CENTER_JUSTIFY,
						# advc.002b: CIVIC_LIST_PANEL_WIDTH instead of HEADINGS_WIDTH
						fX + self.CIVIC_LIST_PANEL_WIDTH / 2, fY + self.TEXT_MARGIN, 0,
						FontTypes.GAME_FONT,
						WidgetTypes.WIDGET_GENERAL, -1, -1 )
	
				fY += self.TEXT_MARGIN
				
				for j in self.allDeliveredCivics:
					if (gc.getCivicInfo(j).getCivicOptionType() == i):										
						fY += 2 * self.TEXT_MARGIN
	
						screen.addCheckBoxGFC(self.getCivicsButtonName(j), gc.getCivicInfo(j).getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), fX + self.BUTTON_SIZE/2, fY, self.BUTTON_SIZE, self.BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
	
						screen.setText(self.getCivicsTextName(j), "", gc.getCivicInfo(j).getDescription(), CvUtil.FONT_LEFT_JUSTIFY, fX + self.BUTTON_SIZE + self.TEXT_MARGIN, fY, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
	
				# self.drawCivicOptionButtons(i)
				reindex += 1


	# Will draw the help text
	def drawAllHelpText(self, goverment):
		re_index = 0
		for i in range (gc.getNumCivicOptionInfos()):
#doto 115 goverment screen			
			check_parent = self.is_parent_child_goverment_civic(i)
			if goverment:
				draw_boxes = check_parent
			else:
				draw_boxes = not check_parent

			if draw_boxes :
#doto 115 goverment screen
				fX = self.HEADINGS_SPACING  + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * re_index # i

				if goverment:
					fY =  125
				else:
					fY =  self.HELP_TOP + 115

				szPaneID = "CivicsHelpTextBackground" + str(re_index) + str(goverment)
				screen = self.getScreen()
				screen.addPanel(szPaneID, "", "", True, True, fX, fY, self.HEADINGS_WIDTH, self.HELP_BOTTOM - (self.HELP_TOP + 110) , PanelStyles.PANEL_STYLE_MAIN)
	
				self.drawHelpText(i, re_index, goverment)
				re_index += 1


	def drawHelpText(self, iCivicOption, true_index = -1, goverment = None):
		
		if true_index == -1:
			true_index = iCivicOption
		activePlayer = gc.getPlayer(self.iActivePlayer)
		iCivic = self.allDeliveredCivics[iCivicOption]

		szPaneID = "CivicsHelpTextBackground" + str(true_index) + str(goverment)
		screen = self.getScreen()

		szHelpText = u""

		# Upkeep string
		if ((gc.getCivicInfo(iCivic).getUpkeep() != -1) and not activePlayer.isNoCivicUpkeep(iCivicOption)):
			szHelpText = gc.getUpkeepInfo(gc.getCivicInfo(iCivic).getUpkeep()).getDescription()
		else:
			szHelpText = localText.getText("TXT_KEY_CIVICS_SCREEN_NO_UPKEEP", ())

		szHelpText += CyGameTextMgr().parseCivicInfo(iCivic, False, True, True)

		fX = self.HEADINGS_SPACING  + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * true_index # iCivicOption

		# screen.setLabel(self.HELP_HEADER_NAME + str(true_index), "Background",  u"<font=3>" + gc.getCivicInfo(self.allDeliveredCivics[iCivicOption]).getDescription().upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, fX + self.HEADINGS_WIDTH/2, self.HELP_TOP + self.TEXT_MARGIN, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		if goverment:
			fY = 130
		else:
			fY = self.HELP_TOP + 3 * self.TEXT_MARGIN + 80
		szHelpAreaID = self.HELP_AREA_NAME + str(true_index) + str(goverment)	 	
		screen.addMultilineText(szHelpAreaID, szHelpText, fX+5, fY, self.HEADINGS_WIDTH-7, self.HELP_BOTTOM - fY, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)				
		


	def getCivicsButtonName(self, iCivic):
		szName = self.BUTTON_NAME + str(iCivic)
		return szName

	def getCivicsTextName(self, iCivic):
		szName = self.TEXT_NAME + str(iCivic)
		return szName
				
	def drawcCivicsScreenButtons(self):

		screen = self.getScreen()
		seperator = 50
		# screen.addPanel('civics_panel' + "Background", "", "", True, True, (self.BOTTOM_LINE_WIDTH / 2) - (self.TEXT_BOX_WIDTH / 2), self.HEADINGS_BOTTOM + self.HEADINGS_BOTTOM / 2  , self.TEXT_BOX_WIDTH / 2, self.TEXT_BOX_SIZE, PanelStyles.PANEL_STYLE_MAIN)
		# screen.addPanel('government_panel' + "Background", "", "", True, True, self.BOTTOM_LINE_WIDTH / 2 + seperator,  self.HEADINGS_BOTTOM + self.HEADINGS_BOTTOM / 2, self.TEXT_BOX_WIDTH / 2, self.TEXT_BOX_SIZE, PanelStyles.PANEL_STYLE_MAIN)
		
		# screen.setText(self.REGULAR_CIVICS_BTN_NAME, "Background", u"<font=4>" + "CIVICS" + u"</font>",  CvUtil.FONT_CENTER_JUSTIFY, (self.BOTTOM_LINE_WIDTH / 2) - (self.TEXT_BOX_WIDTH / 2) + ((self.TEXT_BOX_WIDTH / 2) / 2), self.HEADINGS_BOTTOM + self.HEADINGS_BOTTOM / 2 + ((self.TEXT_BOX_SIZE / 2) - (seperator /2)) , self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)
		# screen.setText(self.GOVERNMENT_BTN_NAME, "Background", u"<font=4>" + "GOVERNMENTS" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.BOTTOM_LINE_WIDTH / 2 + ((self.TEXT_BOX_WIDTH / 2) / 2) + seperator, self.HEADINGS_BOTTOM + self.HEADINGS_BOTTOM / 2 + ((self.TEXT_BOX_SIZE / 2) - (seperator /2)) , self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)
		
		screen.addPanel('civics_panel' + "Background", "", "", True, True, self.BOTTOM_LINE_WIDTH / 2 + ((self.TEXT_BOX_WIDTH / 2)/2) + ((self.TEXT_BOX_WIDTH / 2)/2), self.HEADINGS_BOTTOM + self.HEADINGS_BOTTOM / 2  , self.TEXT_BOX_WIDTH / 2 + (seperator/3), self.TEXT_BOX_SIZE, PanelStyles.PANEL_STYLE_MAIN)
		screen.addPanel('government_panel' + "Background", "", "", True, True, self.BOTTOM_LINE_WIDTH / 2 + ((self.TEXT_BOX_WIDTH / 2)/2) + ((self.TEXT_BOX_WIDTH / 2)/2),  self.HEADINGS_BOTTOM - (self.HEADINGS_BOTTOM /2), self.TEXT_BOX_WIDTH / 2 + (seperator/3), self.TEXT_BOX_SIZE, PanelStyles.PANEL_STYLE_MAIN)
		
		screen.setText(self.REGULAR_CIVICS_BTN_NAME, "Background", u"<font=4>" + "CHANGE CIVICS" + u"</font>",  CvUtil.FONT_CENTER_JUSTIFY,  self.BOTTOM_LINE_WIDTH / 2 + ((self.TEXT_BOX_WIDTH / 2)/2) + ((self.TEXT_BOX_WIDTH / 2)/2)  + (seperator * 3), self.HEADINGS_BOTTOM + (self.HEADINGS_BOTTOM /2) + ((self.TEXT_BOX_SIZE / 2) - (seperator /3)) , self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)
		screen.setText(self.GOVERNMENT_BTN_NAME, "Background", u"<font=4>" + "CHANGE GOVERNMENTS" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY,  self.BOTTOM_LINE_WIDTH / 2 + ((self.TEXT_BOX_WIDTH / 2)/2) + ((self.TEXT_BOX_WIDTH / 2)/2)  + (seperator * 3), self.HEADINGS_BOTTOM - (self.HEADINGS_BOTTOM /2) + ((self.TEXT_BOX_SIZE / 2) - (seperator /2)) , self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)
		

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
		screen.setLabel("CivicsUpkeepText", "Background", u"<font=3>" + szText + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 100, self.BOTTOM_LINE_TOP - 6 + self.BOTTOM_LINE_HEIGHT - 2 * self.TEXT_MARGIN + 5, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
	

	def Civics(self, inputClass):

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			screen = self.getScreen()
			screen.hideScreen()

			CvScreensInterface.showRegularCivicScreen(self.allDeliveredCivics)

	def Governments(self, inputClass):
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			screen = self.getScreen()
			screen.hideScreen()

			CvScreensInterface.showGovermentScreen(self.allDeliveredCivics)

	# Revolution!!!
	def Revolution(self, inputClass):

		activePlayer = gc.getPlayer(self.iActivePlayer)
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			# advc.001d: Clause added to prevent revolution when viewing another civ's civics through the Debug menu
			if self.iActivePlayer == gc.getGame().getActivePlayer() and activePlayer.canRevolution(0):
				messageControl = CyMessageControl()
				messageControl.sendUpdateCivics(self.m_paeDisplayCivics)
			screen = self.getScreen()
			screen.hideScreen()


	def Cancel(self, inputClass):
		screen = self.getScreen()
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			activePlayer = gc.getPlayer(gc.getGame().getActivePlayer())
			for i in range (gc.getNumCivicOptionInfos()):
				self.m_paeCurrentCivics[i] = self.m_paeOriginalCivics[i]
				self.m_paeDisplayCivics[i] = self.m_paeOriginalCivics[i]
				self.allDeliveredCivics[i] = self.m_paeOriginalCivics[i]

			self.drawContents()


	# Will handle the input for this screen...
	def handleInput(self, inputClass):

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
			return 1
		return 0

	#doto 115 goverment screen
	def is_parent_child_goverment_civic(self, eCivicOption):

		is_parent_or_child = gc.getCivicOptionInfo(eCivicOption).getParentCivicOption()
		if is_parent_or_child:
			if is_parent_or_child > 0:
				return True

		return False

