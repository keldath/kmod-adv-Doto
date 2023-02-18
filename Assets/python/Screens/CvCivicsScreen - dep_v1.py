## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import string
import CvScreensInterface

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvCivicsScreen:
	"Civics Screen"

	def __init__(self):
		self.SCREEN_NAME = "CivicsScreen"
		self.CANCEL_NAME = "CivicsCancel"
		self.EXIT_NAME = "CivicsExit"
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

		self.CivicsScreenInputMap = {
			self.BUTTON_NAME		: self.CivicsButton,
			self.TEXT_NAME			: self.CivicsButton,
			self.EXIT_NAME			: self.Revolution,
			self.CANCEL_NAME		: self.Cancel,
			}

		self.iActivePlayer = -1

		self.m_paeCurrentCivics = []
		self.m_paeDisplayCivics = []
		self.m_paeOriginalCivics = []

		# doto civics
		self.m_highLighterParent = []

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.CIVICS_SCREEN)

	def setActivePlayer(self, iPlayer):

		self.iActivePlayer = iPlayer
		activePlayer = gc.getPlayer(iPlayer)

		self.m_paeCurrentCivics = []
		self.m_paeDisplayCivics = []
		self.m_paeOriginalCivics = []
		# doto civics
		self.m_highLighterParent = []
		for i in range (gc.getNumCivicOptionInfos()):
			self.m_paeCurrentCivics.append(activePlayer.getCivics(i));
			self.m_paeDisplayCivics.append(activePlayer.getCivics(i));
			self.m_paeOriginalCivics.append(activePlayer.getCivics(i));
			# doto civics
			self.m_highLighterParent.append(activePlayer.getCivics(i));

	def interfaceScreen (self):

		screen = self.getScreen()
		if screen.isActive():
			return
		screen.setRenderInterfaceOnly(True);
		screen.showScreen( PopupStates.POPUPSTATE_IMMEDIATE, False)
	
		# Set the background and exit button, and show the screen
		#KELDATH - POSITION THE SCREEN
		screen.setDimensions(screen.centerX(-170), screen.centerY(0), self.W_SCREEN, self.H_SCREEN)
		screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addPanel( "TechTopPanel", u"", u"", True, False, 0, 0, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_TOPBAR )
		screen.addPanel( "TechBottomPanel", u"", u"", True, False, 0, 713, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_BOTTOMBAR )
		screen.showWindowBackground(False)
		screen.setText(self.CANCEL_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_SCREEN_CANCEL", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_CANCEL, self.Y_CANCEL, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

		# Header...
		#screen.setText(self.TITLE_NAME, "Background", u"<font=4b>" + localText.getText("TXT_KEY_CIVICS_SCREEN_TITLE", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_SCREEN, self.Y_TITLE, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		# keldath civics dependancy
		screen.setText(self.TITLE_NAME, "Background", u"<font=4b>" + "Goverments Table" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_SCREEN, self.Y_TITLE, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
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

	# Draw the contents...
	def drawContents(self):
	
		# Draw the radio buttons
		self.drawAllButtons()
				
		# Draw Help Text
		# keldath remove for test
		self.drawAllHelpText()
		
		# Update Maintenance/anarchy/etc.
		self.updateAnarchy()

	def drawCivicOptionButtons(self, iCivicOption):

		activePlayer = gc.getPlayer(self.iActivePlayer)
		screen = self.getScreen()
		
		for j in range(gc.getNumCivicInfos()):

			if (gc.getCivicInfo(j).getCivicOptionType() == iCivicOption):										
				screen.setState(self.getCivicsButtonName(j), self.m_paeCurrentCivics[iCivicOption] == j)
							
				if (self.m_paeDisplayCivics[iCivicOption] == j):
					#screen.setState(self.getCivicsButtonName(j), True)
					screen.show(self.getCivicsButtonName(j))
				elif (activePlayer.canDoCivics(j)):
					#screen.setState(self.getCivicsButtonName(j), False)
					screen.show(self.getCivicsButtonName(j))
				else:
					screen.hide(self.getCivicsButtonName(j))
								
	# Will draw the radio buttons (and revolution)
	def drawAllButtons(self):				

		def govcivic(i, c, section, panelLength, fY, line_seperator, civicoption, panel=PanelStyles.PANEL_STYLE_MAIN):

			fX = (self.HEADINGS_SPACING  + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * i ) - 5
			
			szAreaID = self.AREA_NAME + section  + str(i) # keldath - needed section name to create a new area
			screen.addPanel(szAreaID, "", "", True, True, fX+5, fY, self.HEADINGS_WIDTH, panelLength, panel)
			
			civics = range(gc.getNumCivicInfos())
			for j in civics:
				civicDesc = gc.getCivicInfo(j).getDescription()
				if civicDesc in c:
					fY += line_seperator
					# the first civic in a sub list civic needs to be closer to the start of the panerl
					# also for goverments itss posioning is different
					if c[0] == civicDesc and section != "gov":
						fY -= 15
					screen.addCheckBoxGFC(self.getCivicsButtonName(j), gc.getCivicInfo(j).getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), fX + self.BUTTON_SIZE/2, fY, self.BUTTON_SIZE, self.BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
					screen.setText(self.getCivicsTextName(j), "", gc.getCivicInfo(j).getDescription(), CvUtil.FONT_LEFT_JUSTIFY, fX + self.BUTTON_SIZE + self.TEXT_MARGIN, fY, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			
			self.drawCivicOptionButtons(civicoption)

		screen = self.getScreen()
		constLength = self.HEADINGS_BOTTOM - self.HEADINGS_TOP
		numCivicOtopns = range(gc.getNumCivicOptionInfos())

		# set the goverments civic bar
		fY = self.HEADINGS_TOP # top position start for the panel y axis
		line_seperator = 2 * self.TEXT_MARGIN  # space between top of the panel start
		govPnelLength = constLength - 240
		govCivics =  [['Despotism'], ['Hereditary Rule'], ['Representation'], ['Provincial'], 
				['Police State'], ['Chiefdom'], ['Electorial'], ['Fundamentalism']]
		for i in xrange(len(govCivics)):
			govcivic(i, govCivics[i], "gov", govPnelLength, fY, line_seperator - 15,  numCivicOtopns[0], PanelStyles.PANEL_STYLE_BLUE50)

		# the order of the civics on the list must be according to the order of the xml.
		# cause of the civic loop. it affects positioning		
		# the the Legal civic section
		screen.addPanel( "govRuleBar", u"", u"", True, False, 0, 85, self.W_SCREEN, 25, PanelStyles.PANEL_STYLE_MAIN_TAN ) # 82
		screen.setText("rule", "", u"<font=4>" + "Govermant Rules" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 500, 88, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		fY += 45
		goveRulesPnelLength = constLength - 145
		goveRulesCivics = [['RULE1', 'RULE2'], ['RULE3', 'RULE4'], ['RULE5', 'RULE6'], ['RULE7', 'RULE8'], 
							['RULE9', 'RULE10'], ['RULE11', 'RULE12'], ['RULE13', 'RULE14'], ['RULE15', 'RULE16']]
		for i in xrange(len(goveRulesCivics)):
			govcivic(i, goveRulesCivics[i], "govRule", goveRulesPnelLength, fY, line_seperator, numCivicOtopns[1], PanelStyles.PANEL_STYLE_CITY_COLUMNL)

		# the the Labor civic section
		screen.addPanel( "SocietyBar", u"", u"", True, False, 0, 155, self.W_SCREEN, 25, PanelStyles.PANEL_STYLE_MAIN_TAN ) # 176
		screen.setText("soc", "",u"<font=4>" + "Society Ideals"  + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 500, 158, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		fY += 70
		legPnelLength = constLength - 160
		legCivics =  [['Anarchizem', 'Traditional'], ['Fanaticism', 'Aggrerian'], ['Intellectualism', 'Residential'], ['Individualism', 'Expontional'],
			 ['Creatism', 'Constructional'], ['Commercialism', 'Pastoral'], ['Imperialism', 'Exploitentional'], ['Pragmatism', 'Proggretional']]
		for i in xrange(len(legCivics)):
			govcivic(i, legCivics[i], "leg", legPnelLength, fY, line_seperator, numCivicOtopns[2], PanelStyles.PANEL_STYLE_CITY_COLUMNL)


		# fY += 55
		# line_seperator += 16 # need to handle first item sep
		# legPnelLength = constLength - 200
		# legCivics =  [['Elders Wisdom','Civil Law'], ['RULE1','RULE2'], ['Manorialism','RULE3'], ['Federal','RULE4'],
		# 	 ['Vassalage','RULE5'], ['Bureaucracy','RULE6'], ['Nationhood', 'RUL7'], ['Free Speech']]
		# for i in xrange(len(legCivics)):
		# 	govcivic(i, legCivics[i], "leg", legPnelLength, fY, line_seperator, PanelStyles.PANEL_STYLE_CITY_COLUMNL)

		# # the the Labor civic section
		# screen.addPanel( "LaborBar", u"", u"", True, False, 0, 170, self.W_SCREEN, 25, PanelStyles.PANEL_STYLE_MAIN_TAN ) # 176
		# screen.setText("labor", "", "Labor", CvUtil.FONT_LEFT_JUSTIFY, 500, 185, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		# fY += 94
		# labPnelLength = constLength - 230
		# LaborBar =  [['Tribalism'], ['Slavery'], ['Serfdom'], ['Caste System'], ['Emancipation'], ['Free Masons'], ['Proletariat'], ['UnionShip']]
		# for i in xrange(len(LaborBar)):
		# 	govcivic(i, LaborBar[i], "lab", labPnelLength, fY, line_seperator, PanelStyles.PANEL_STYLE_CITY_COLUMNL) # PanelStyles.PANEL_STYLE_BLUELARGE nice one

		# # the the Labor civic section
		# screen.addPanel( "EcoBar", u"", u"", True, False, 0, 230, self.W_SCREEN, 25, PanelStyles.PANEL_STYLE_MAIN_TAN ) # PANEL_STYLE_TECH 240
		# screen.setText("eco", "", "Economy", CvUtil.FONT_LEFT_JUSTIFY, 500, 248, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		# fY += 64
		# ecoPnelLength = constLength - 210
		# ecoBar =  [['Local Trade'], ['Mercantilism'], ['Free Market', 'Barter'], ['State Property'], ['Enviromental'], ['Free Masons'], ['Faith Driven'], ['Commodotive']] 
		# for i in xrange(len(ecoBar)):
		# 	govcivic(i, ecoBar[i], "eco", ecoPnelLength, fY, line_seperator, PanelStyles.PANEL_STYLE_CITY_COLUMNL)

		# # the the Labor civic section
		# screen.addPanel( "RelBar", u"", u"", True, False, 0, 320, self.W_SCREEN, 25, PanelStyles.PANEL_STYLE_MAIN_TAN ) # 325
		# screen.setText("rel", "", "Religion", CvUtil.FONT_LEFT_JUSTIFY, 500, 334, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		# fY += 84
		# relPnelLength = constLength - 200
		# relBar =  [['Paganism'], ['Orthodoxian'], ['Organized'], ['Theocracy'], ['Divine Charity'], ['Free Religion'], ['Atheism'], ['Agnosticism']] 
		# for i in xrange(len(relBar)):
		# 	govcivic(i, relBar[i], "rel", relPnelLength, fY, line_seperator, PanelStyles.PANEL_STYLE_CITY_COLUMNL) # PanelStyles.PANEL_STYLE_DAWNTOP

		# the the Labor civic section
		# screen.addPanel( "MilBar", u"", u"", True, False, 0, 420, self.W_SCREEN, 25, PanelStyles.PANEL_STYLE_MAIN_TAN )
		# screen.setText("mil", "", "Military", CvUtil.FONT_LEFT_JUSTIFY, 500, 430, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		# fY += 100
		# milPnelLength = constLength - 200
		# milBar =  [['Warring Tribes'], ['Bandits'], ['Merc'], ['Levee'], ['Volunteer'], ['Professional'], ['Guerrilla'], ['Obligatory']] 
		# for i in xrange(len(milBar)):
		# 	govcivic(i, milBar[i], "mil", milPnelLength, fY, line_seperator, PanelStyles.PANEL_STYLE_CITY_COLUMNL)


		# all the other normal civics
		ignoreCivics = ['Government', 'Rules', 'Ideals']
		#civicLi = range(gc.getNumCivicOptionInfos())
		# the the Labor civic section
		screen.addPanel( "otherCivics", u"", u"", True, False, 0, 235, self.W_SCREEN, 30, PanelStyles.PANEL_STYLE_MAIN_TAN ) # 176
		screen.setText("others", "",  u"<font=4>" +" Civics Table" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 500, 240, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		counter = 0 # keldath add - the index of the civics on the list is not from 0
		broad_width = 100
		const_fY = 	260
		for i in numCivicOtopns:
			
			if gc.getCivicOptionInfo(i).getDescription() not in ignoreCivics:

				fX = self.HEADINGS_SPACING  + ((self.HEADINGS_WIDTH + self.HEADINGS_SPACING + broad_width) * (counter)) # was * i
				szAreaID = self.AREA_NAME + str(i)
				fY = const_fY
				screen = self.getScreen()
				screen.addPanel(szAreaID, "", "", True, True, fX+5, fY, self.HEADINGS_WIDTH + broad_width, self.HEADINGS_BOTTOM - self.HEADINGS_TOP - 120, PanelStyles.PANEL_STYLE_MAIN)
				screen.setLabel("", "Background",  u"<font=3>" + gc.getCivicOptionInfo(i).getDescription().upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, fX + self.HEADINGS_WIDTH/2 + 50, fY + self.TEXT_MARGIN, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

				civic_counter = 0 # start from 1
				colum_num = 0
				fx_box_width = fX + self.BUTTON_SIZE/2
				fx_text_width = fX + self.BUTTON_SIZE + self.TEXT_MARGIN
				for j in range(gc.getNumCivicInfos()):
					if (gc.getCivicInfo(j).getCivicOptionType() == i):
						# split to a new column if 4 civics has been placed
						if civic_counter > 3 and colum_num < 1:
							fx_box_width += broad_width + 40
							fx_text_width += broad_width + 40
							fY = const_fY
							colum_num = 1

						fY += 2 * self.TEXT_MARGIN 
						screen.addCheckBoxGFC(self.getCivicsButtonName(j), gc.getCivicInfo(j).getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), fx_box_width, fY, self.BUTTON_SIZE, self.BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
						screen.setText(self.getCivicsTextName(j), "", gc.getCivicInfo(j).getDescription(), CvUtil.FONT_LEFT_JUSTIFY, fx_text_width, fY, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
						civic_counter += 1
						self.drawCivicOptionButtons(numCivicOtopns[i])

				counter += 1

		# for i in range(gc.getNumCivicOptionInfos()):
		
		# 	fX = self.HEADINGS_SPACING  + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * i
		# 	fY = self.HEADINGS_TOP
		# 	szAreaID = self.AREA_NAME + str(i)
		# 	screen = self.getScreen()
		# 	#KELDATH CIVIC POSITION
		# 	screen.addPanel(szAreaID, "", "", True, True, fX+5, fY, self.HEADINGS_WIDTH, self.HEADINGS_BOTTOM - self.HEADINGS_TOP, PanelStyles.PANEL_STYLE_MAIN)
		# 	screen.setLabel("", "Background",  u"<font=3>" + gc.getCivicOptionInfo(i).getDescription().upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, fX + self.HEADINGS_WIDTH/2, self.HEADINGS_TOP + self.TEXT_MARGIN, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		# 	fY += self.TEXT_MARGIN
			
		# 	for j in range(gc.getNumCivicInfos()):
		# 		if (gc.getCivicInfo(j).getCivicOptionType() == i):										
		# 			fY += 2 * self.TEXT_MARGIN
		# 			screen.addCheckBoxGFC(self.getCivicsButtonName(j), gc.getCivicInfo(j).getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), fX + self.BUTTON_SIZE/2, fY, self.BUTTON_SIZE, self.BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
		# 			screen.setText(self.getCivicsTextName(j), "", gc.getCivicInfo(j).getDescription(), CvUtil.FONT_LEFT_JUSTIFY, fX + self.BUTTON_SIZE + self.TEXT_MARGIN, fY, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# 	self.drawCivicOptionButtons(i)
					
							
	def highlight(self, iCivic):
		iCivicOption = gc.getCivicInfo(iCivic).getCivicOptionType()
		if self.m_paeDisplayCivics[iCivicOption] != iCivic:
			self.m_paeDisplayCivics[iCivicOption] = iCivic
			self.drawCivicOptionButtons(iCivicOption)
			return True
		return False
		
	def unHighlight(self, iCivic):		
		iCivicOption = gc.getCivicInfo(iCivic).getCivicOptionType()
		if self.m_paeDisplayCivics[iCivicOption] != self.m_paeCurrentCivics[iCivicOption]:
			self.m_paeDisplayCivics[iCivicOption] = self.m_paeCurrentCivics[iCivicOption]
			self.drawCivicOptionButtons(iCivicOption)
			return True
		return False
		
	def select(self, iCivic):
		activePlayer = gc.getPlayer(self.iActivePlayer)
		if (not activePlayer.canDoCivics(iCivic)):
			# If you can't even do this, get out....
			return 0

		# doto
		# gc.getCivicInfo(j).getDescription()
		# gc.getCivicInfo(iCivic).getNumParentCivicsChildren
		# iCivicOption = gc.getCivicInfo(iCivic).getCivicOptionType()
		# activePlayer.getCivics(i)

		iCivicOption = gc.getCivicInfo(iCivic).getCivicOptionType()
		
		# Set the previous widget
		iCivicPrev = self.m_paeCurrentCivics[iCivicOption]
		
		# Switch the widgets
		self.m_paeCurrentCivics[iCivicOption] = iCivic
		
		# Unighlight the previous widget
		self.unHighlight(iCivicPrev)
		self.getScreen().setState(self.getCivicsButtonName(iCivicPrev), False)

		# highlight the new widget
		self.highlight(iCivic)

		# doto update which parent is highlighted
		if gc.getCivicInfo(iCivic).getNumParentCivicsChildren() > 0 :
			m_highLighterParent[iCivicOption] = iCivic

		self.getScreen().setState(self.getCivicsButtonName(iCivic), True)
		
		return 0

	def CivicsButton(self, inputClass):
	
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			if (inputClass.getFlags() & MouseFlags.MOUSE_RBUTTONUP):
				CvScreensInterface.pediaJumpToCivic((inputClass.getID(), ))
			else:
				# Select button
				self.select(inputClass.getID())
				self.drawHelpText(gc.getCivicInfo(inputClass.getID()).getCivicOptionType())
				self.updateAnarchy()
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_ON) :
			# Highlight this button
			if self.highlight(inputClass.getID()):
				self.drawHelpText(gc.getCivicInfo(inputClass.getID()).getCivicOptionType())
				self.updateAnarchy()
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_OFF) :
			if self.unHighlight(inputClass.getID()):
				self.drawHelpText(gc.getCivicInfo(inputClass.getID()).getCivicOptionType())
				self.updateAnarchy()

		return 0

		
	def drawHelpText(self, iCivicOption):
		
		#return # keldath
		activePlayer = gc.getPlayer(self.iActivePlayer)
		iCivic = self.m_paeDisplayCivics[iCivicOption]

		szPaneID = "CivicsHelpTextBackground" + str(iCivicOption)
		screen = self.getScreen()

		szHelpText = u""

		# Upkeep string
		if ((gc.getCivicInfo(iCivic).getUpkeep() != -1) and not activePlayer.isNoCivicUpkeep(iCivicOption)):
			szHelpText = gc.getUpkeepInfo(gc.getCivicInfo(iCivic).getUpkeep()).getDescription()
		else:
			szHelpText = localText.getText("TXT_KEY_CIVICS_SCREEN_NO_UPKEEP", ())

		szHelpText += CyGameTextMgr().parseCivicInfo(iCivic, False, True, True)

		fX = self.HEADINGS_SPACING  + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * iCivicOption

		#screen.setLabel(self.HELP_HEADER_NAME + str(iCivicOption), "Background",  u"<font=3>" + gc.getCivicInfo(self.m_paeDisplayCivics[iCivicOption]).getDescription().upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, fX + self.HEADINGS_WIDTH/2, self.HELP_TOP + self.TEXT_MARGIN, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel(self.HELP_HEADER_NAME + str(iCivicOption), "Background",  u"<font=3>" + gc.getCivicInfo(self.m_paeDisplayCivics[iCivicOption]).getDescription().upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, fX + self.HEADINGS_WIDTH/2, self.HELP_TOP + self.TEXT_MARGIN + 90, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		fY = self.HELP_TOP - self.BIG_BUTTON_SIZE
		szHelpImageID = self.HELP_IMAGE_NAME + str(iCivicOption)
		#KELDATH - REMOVED THE MID BTN - DONT NEED IT, JUST TAKES UP SPACE		
		#screen.setImageButton(szHelpImageID, gc.getCivicInfo(iCivic).getButton(), fX + self.HEADINGS_WIDTH/2 - self.BIG_BUTTON_SIZE/2, fY, self.BIG_BUTTON_SIZE, self.BIG_BUTTON_SIZE, WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIVIC, iCivic, 1)

		fY = self.HELP_TOP + 3 * self.TEXT_MARGIN
		szHelpAreaID = self.HELP_AREA_NAME + str(iCivicOption)		
		#screen.addMultilineText(szHelpAreaID, szHelpText, fX+5, fY, self.HEADINGS_WIDTH-7, self.HELP_BOTTOM - fY-2, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)				
		screen.addMultilineText(szHelpAreaID, szHelpText, fX+5, fY+90, self.HEADINGS_WIDTH-7, self.HELP_BOTTOM - fY-2+150, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)				
		
		
	# Will draw the help text
	def drawAllHelpText(self):
		#return # keldath
		for i in range (gc.getNumCivicOptionInfos()):		

			fX = self.HEADINGS_SPACING  + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * i

			szPaneID = "CivicsHelpTextBackground" + str(i)
			screen = self.getScreen()
			#SCREEN CIVIC HELP POSITION
			#screen.addPanel(szPaneID, "", "", True, True, fX+5, self.HELP_TOP, self.HEADINGS_WIDTH, self.HELP_BOTTOM - self.HELP_TOP, PanelStyles.PANEL_STYLE_MAIN)
			screen.addPanel(szPaneID, "", "", True, True, fX+5, self.HELP_TOP+90, self.HEADINGS_WIDTH, self.HELP_BOTTOM - self.HELP_TOP-100, PanelStyles.PANEL_STYLE_MAIN)

			self.drawHelpText(i)


	# Will Update the maintenance/anarchy/etc
	def updateAnarchy(self):

		screen = self.getScreen()

		activePlayer = gc.getPlayer(self.iActivePlayer)

		bChange = False
		i = 0
		while (i  < gc.getNumCivicOptionInfos() and not bChange):
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
		#KELDATH CHANGES
		screen.setLabel("CivicsRevText", "Background", u"<font=3>" + szText + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 700, self.BOTTOM_LINE_TOP + 5 + self.TEXT_MARGIN//2, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# Maintenance		
		#szText = localText.getText("TXT_KEY_CIVIC_SCREEN_UPKEEP", (activePlayer.getCivicUpkeep(self.m_paeDisplayCivics, True), ))
		szText = localText.getText("TXT_KEY_CIVIC_SCREEN_UPKEEP", (activePlayer.getCivicUpkeep(self.m_paeDisplayCivics, True)*(100+activePlayer.calculateInflationRate())/100, )) # K-Mod
		#KELDATH CHANGE
		screen.setLabel("CivicsUpkeepText", "Background", u"<font=3>" + szText + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 100, self.BOTTOM_LINE_TOP - 6 + self.BOTTOM_LINE_HEIGHT - 2 * self.TEXT_MARGIN, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
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
			for i in range (gc.getNumCivicOptionInfos()):
				self.m_paeCurrentCivics[i] = self.m_paeOriginalCivics[i]
				self.m_paeDisplayCivics[i] = self.m_paeOriginalCivics[i]
			
			self.drawContents()
			
	def getCivicsButtonName(self, iCivic):
		szName = self.BUTTON_NAME + str(iCivic)
		return szName

	def getCivicsTextName(self, iCivic):
		szName = self.TEXT_NAME + str(iCivic)
		return szName

	# Will handle the input for this screen...
	def handleInput(self, inputClass):
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED):
			screen = self.getScreen()
			iIndex = screen.getSelectedPullDownID(self.DEBUG_DROPDOWN_ID)
			self.setActivePlayer(screen.getPullDownData(self.DEBUG_DROPDOWN_ID, iIndex))
			self.drawContents()
			return 1
		elif (self.CivicsScreenInputMap.has_key(inputClass.getFunctionName())):	
			'Calls function mapped in CvCivicsScreen'
			# only get from the map if it has the key		

			# get bound function from map and call it
			self.CivicsScreenInputMap.get(inputClass.getFunctionName())(inputClass)
			return 1
		return 0
		
	def update(self, fDelta):
		return




