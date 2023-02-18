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

		# doto civics start:
		self.m_allParentsCivics = []
		self.m_allChildCivics = {}
		self.m_highLighterParent = "" # []
		# doto civics end:

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.CIVICS_SCREEN)

	def setActivePlayer(self, iPlayer):

		self.iActivePlayer = iPlayer
		activePlayer = gc.getPlayer(iPlayer)

		self.m_paeCurrentCivics = []
		self.m_paeDisplayCivics = []
		self.m_paeOriginalCivics = []
		# doto civics
		self.m_highLighterParent = "" # []
		self.m_allChildCivics = {}
		for i in range (gc.getNumCivicOptionInfos()):
			self.m_paeCurrentCivics.append(activePlayer.getCivics(i));
			self.m_paeDisplayCivics.append(activePlayer.getCivics(i));
			self.m_paeOriginalCivics.append(activePlayer.getCivics(i));
			# doto civics start
			#self.m_highLighterParent.append(activePlayer.getCivics(i));
			self.m_allChildCivics[i] = [];
			# doto civics end

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
		screen.setText(self.CANCEL_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_SCREEN_CANCEL", ()).upper() + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_CANCEL+30, self.Y_CANCEL+10, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

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
		# Doto start
		# this function was heavily modified for Doto keldath parent and dependant civic mod of goverments
		# it is based on index placment in the xml and uses sdk code that was written for this.
		def doPivotLayoutCivics(civicIdx, civicName_l, firstRow, panelLength, fY, line_seperator, civicoption, panel=PanelStyles.PANEL_STYLE_MAIN):

			fX = (self.HEADINGS_SPACING  + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * civicIdx ) - 5
			
			szAreaID = self.AREA_NAME + str(civicoption)  + str(civicIdx) # keldath - needed section name to create a new area
			screen.addPanel(szAreaID, "", "", True, True, fX+5, fY, self.HEADINGS_WIDTH, panelLength, panel)
			
			civics = range(gc.getNumCivicInfos())
			for j in civics:
				civicDesc = gc.getCivicInfo(j).getDescription()
				if civicDesc in civicName_l:
					fY += line_seperator
					# the first civic in a sub list civic needs to be closer to the start of the panerl
					# also for goverments itss posioning is different
					if civicName_l[0] == civicDesc and not firstRow:
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


		# create a list of civic options and its civics in an orderly fashion
		# the reason for the civic option loop is to ste the nested list in order.
		# the out put will be:
		# self.m_allParentsCivics = [['Despotism'], ['Hereditary Rule']....n]
		# self.m_allChildCivics = { 0: [], 1: [['RULE1', 'RULE2'], ['RULE3', 'RULE4'] ....n]}
		for i in range (gc.getNumCivicOptionInfos()):
			tempCivicOption_l = [] # all the civics of this civic option
			for j in range(gc.getNumCivicInfos()):
				if i == gc.getCivicInfo(j).getCivicOptionType() :
					childNum = gc.getCivicInfo(j).getNumParentCivicsChildren()
					if childNum > 0 :
						tempCivicOption_l.append([gc.getCivicInfo(j).getDescription()])
						# populate the child civics , each nested list is a column of the 
						# parent dependant civic (same index of the list of m_allParentsCivics)
						for m in range (gc.getNumCivicOptionInfos()):
							if gc.getCivicOptionInfo(m).getParentCivicOption() != 1 : # child civic option
								continue
							# fill in all the child civics for a specific parent under the current civic
							tmpChildCivicsOfCivicOption = []
							for c in xrange(childNum) :
								# loop in ann the children
								childCivic = gc.getCivicInfo(gc.getCivicInfo(j).getParentCivicsChildren(c))
								if childCivic.getCivicOptionType() == m :
									tmpChildCivicsOfCivicOption.append(childCivic.getDescription())
							if len(tmpChildCivicsOfCivicOption) < 1:
								return # safty check
							self.m_allChildCivics[m].append(tmpChildCivicsOfCivicOption)

			self.m_allParentsCivics.append(tempCivicOption_l)

		# build the parent (goverment civic option latyout)
		civicList = self.m_allParentsCivics
		for l in xrange(len(civicList)):
			if len(civicList[l]) < 1 :
				continue
			for i in xrange(len(civicList[l])):
				doPivotLayoutCivics(i, civicList[l][i], True, govPnelLength, fY, line_seperator - 15,  l, PanelStyles.PANEL_STYLE_BLUE50)
			# self.drawCivicOptionButtons(l)

		# place the children civic options
		fy_l = [45, 70]
		panelLength_l = [145, 160]
		panelPos = [85, 155]
		counter_pos = 0
		for j in self.m_allChildCivics.keys():
			if len(self.m_allChildCivics[j]) > 0:
				fY += fy_l[counter_pos]
				panelLength_l_val = constLength - panelLength_l[counter_pos]
				civicChild_l = self.m_allChildCivics[j]
				screen.addPanel("", u"", u"", True, False, 0, panelPos[counter_pos], self.W_SCREEN, 25, PanelStyles.PANEL_STYLE_MAIN_TAN ) # 176
				screen.setText("" , "",u"<font=4>" + gc.getCivicOptionInfo(j).getDescription()  + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 500,panelPos[counter_pos] + 3, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				for i in xrange(len(civicChild_l)):
					doPivotLayoutCivics(i, civicChild_l[i], False , panelLength_l_val, fY, line_seperator, numCivicOtopns[j], PanelStyles.PANEL_STYLE_CITY_COLUMNL)
				# self.drawCivicOptionButtons(j)
				# # the the Labor civic section
				counter_pos += 1

		
		########################
		# build normal civics section
		screen.addPanel( "otherCivics", u"", u"", True, False, 0, 235, self.W_SCREEN, 30, PanelStyles.PANEL_STYLE_MAIN_TAN ) # 176
		screen.setText("others", "",  u"<font=4>" +" Civics Table" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 500, 240, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		counter = 0 # keldath add - the index of the civics on the list is not from 0
		broad_width = 100
		const_fY = 	260
		for i in numCivicOtopns:
			
			if gc.getCivicOptionInfo(i).getParentCivicOption() < 1:

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

				counter += 1
				self.drawCivicOptionButtons(i)

		# bts/advc original code for this function:
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
		
		# Doto end			
							
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

		# doto civics start
		# if a parent is no highlighted and one of its childs
		# is this current civic in the select - forbid it from being selected or highlighted.
		for i in range (gc.getNumCivicInfos()):
			numChilds = gc.getCivicInfo(i).getNumParentCivicsChildren()
			if numChilds > 0 :
				for j in xrange(numChilds):
					child = gc.getCivicInfo(i).getParentCivicsChildren(j)
					if iCivic == child:
						if self.m_highLighterParent != i:
							return 0

		# gc.getCivicInfo(j).getDescription()
		# gc.getCivicInfo(iCivic).getNumParentCivicsChildren
		# iCivicOption = gc.getCivicInfo(iCivic).getCivicOptionType()
		# activePlayer.getCivics(i)

		# doto civics end

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

		# doto civics start
		# update which parent is highlighted
		if gc.getCivicInfo(iCivic).getNumParentCivicsChildren() > 0 :
			self.m_highLighterParent = iCivic
		# doto civics end

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
		#doto civic change
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
		#doto KELDATH CHANGE
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




