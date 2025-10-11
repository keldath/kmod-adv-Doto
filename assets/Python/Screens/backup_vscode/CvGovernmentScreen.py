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

class CvGovermentScreen:
	"Goverment Screen"

	def __init__(self):
		self.SCREEN_NAME = "GovermentScreen"
		self.CANCEL_NAME = "GovermentCancel"
		self.RESET_NAME = "GovermentReset"
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
			self.BUTTON_NAME		: self.CivicsButton,
			self.TEXT_NAME			: self.CivicsButton,
			self.EXIT_NAME			: self.Revolution,
			self.CANCEL_NAME		: self.Cancel,
			self.RESET_NAME		: self.Reset,
		}

		self.iActivePlayer = -1

		self.m_paeCurrentCivics = []
		self.m_paeDisplayCivics = []
		self.m_paeOriginalCivics = []

		# doto civics start:
		self.m_allParentsCivics = []
		self.m_allChildCivics = {}
		#self.m_highLighterParent = 0
		self.GOV_CIVIC_HEADER = 480
		self.PANEL_BOX_ADJUSTER = 100
		self.TEXT_BOX_SIZE = 150
		self.TEXT_BOX_WIDTH = self.HEADINGS_WIDTH + self.PANEL_BOX_ADJUSTER * 2 + self.PANEL_BOX_ADJUSTER/2 + 160
		self.TEXT_BOX_SEPERATOR = 5
		self.TEXT_BOX_Y_START = 240

		self.allDeliveredCivics = []
		self.m_absoluteOriginalCivics = []
		# doto civics end:

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)

	def setActivePlayer(self, iPlayer, civics):

		self.iActivePlayer = iPlayer
		activePlayer = gc.getPlayer(iPlayer)

		self.m_paeCurrentCivics = []
		self.m_paeDisplayCivics = []
		self.m_paeOriginalCivics = []
		self.m_allChildCivics = {}
		self.allDeliveredCivics = []
		self.m_absoluteOriginalCivics = []
		self.m_aParentGroupHighlightCivics = []
	#doto 115 goverment screen
		for i in range (gc.getNumCivicOptionInfos()):
			if civics:		
				self.m_paeCurrentCivics.append(civics[i]);
				self.m_paeDisplayCivics.append(civics[i]);
				self.m_paeOriginalCivics.append(civics[i]);
				self.allDeliveredCivics.append(civics[i]);
			else:		
				self.m_paeCurrentCivics.append(activePlayer.getCivics(i));
				self.m_paeDisplayCivics.append(activePlayer.getCivics(i));
				self.m_paeOriginalCivics.append(activePlayer.getCivics(i));
				self.allDeliveredCivics.append(activePlayer.getCivics(i));
			self.m_allChildCivics[i] = [];
			self.m_aParentGroupHighlightCivics = [];
			self.m_absoluteOriginalCivics.append(activePlayer.getCivics(i));
	# doto civics end

	def interfaceScreen (self, civics=None):

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
		screen.setText(self.CANCEL_NAME, "Background", u"<font=4>" + "PREV_SELECT" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_CANCEL+10, self.Y_CANCEL+5, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

		screen.setText(self.RESET_NAME, "Background", u"<font=4>" + "RESET" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_CANCEL / 2, self.Y_CANCEL, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

		# Header...
		screen.setText(self.TITLE_NAME, "Background",u"<font=3b>" + u"<color=205,180,55,255>%s</color>" % "GOVERMENTS"  + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, self.GOV_CIVIC_HEADER, self.Y_TITLE-5, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)		
		
		# doto civic from the main
		self.setActivePlayer(gc.getGame().getActivePlayer(), civics)

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
		
		#doto era panel convert details
		self.draw_era_convert_info()

		# Draw the radio buttons
		self.drawAllButtons()
				
		# Draw Help Text
		self.drawAllHelpText()
		
		# Update Maintenance/anarchy/etc.
		self.updateAnarchy()

	def drawCivicOptionButtons(self, iCivicOption):

		activePlayer = gc.getPlayer(self.iActivePlayer)
		screen = self.getScreen()
		
		for j in xrange(gc.getNumCivicInfos()):

			if (gc.getCivicInfo(j).getCivicOptionType() == iCivicOption):										
				screen.setState(self.getCivicsButtonName(j), self.m_paeCurrentCivics[iCivicOption] == j)
				
				# CvUtil.pyPrint('draw the civics buttons')			
				if (self.m_paeDisplayCivics[iCivicOption] == j):
					#screen.setState(self.getCivicsButtonName(j), True)
					screen.show(self.getCivicsButtonName(j))
				elif (activePlayer.canDoCivics(j, True)):
					# doto 115 goverment screen / civic parent child
					# cvgovermentscreen sends a call from canDoCivics which checks if to draw the civic buttons.
					# i added the true , cvgovermentscreen_ignore_hide so the display of civics would display despite the 
					# getGovermentConversionCounter being 0. that is because if the counter is 0 it would show the civics as attainable,
					# which is true, but its confusing cause it feels like player just havnt learnt them yet.
					#screen.setState(self.getCivicsButtonName(j), False)
					screen.show(self.getCivicsButtonName(j))
				else:
					screen.hide(self.getCivicsButtonName(j))
								
	# Will draw the radio buttons (and revolution)
	def drawAllButtons(self):				
		# Doto start civic parent mod
		# this function was heavily modified for Doto keldath parent and dependant civic mod of goverments
		# it is based on index placment in the xml and uses sdk code that was written for this.
		def doPivotLayoutCivics(civicIdx, civicName_l, firstRow, panelLength, fY, line_seperator, civicoption, panel=PanelStyles.PANEL_STYLE_MAIN):
			# this will populate the column in a pivotal method and create the panels of it.
			fX = (self.HEADINGS_SPACING  + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * civicIdx ) #- 5
			
			szAreaID = self.AREA_NAME + str(civicoption)  + str(civicIdx) # keldath - needed section name to create a new area
			screen.addPanel(szAreaID, "", "", True, True, fX+5, fY, self.HEADINGS_WIDTH, panelLength, panel)
			
			for j in xrange(gc.getNumCivicInfos()):
				civicDesc = gc.getCivicInfo(j).getDescription()
				if civicDesc in civicName_l:
					fY += line_seperator
					# the first civic in a sub list civic needs to be closer to the start of the panerl
					# also for goverments itss posioning is different
					if civicName_l[0] == civicDesc and not firstRow:
						fY -= 15
					screen.addCheckBoxGFC(self.getCivicsButtonName(j), gc.getCivicInfo(j).getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), fX + self.BUTTON_SIZE/2, fY, self.BUTTON_SIZE, self.BUTTON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
					screen.setText(self.getCivicsTextName(j), "",u"<font=2>" + gc.getCivicInfo(j).getDescription() + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, fX + self.BUTTON_SIZE + self.TEXT_MARGIN, fY, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
			self.drawCivicOptionButtons(civicoption)


		screen = self.getScreen()
		constLength = self.HEADINGS_BOTTOM - self.HEADINGS_TOP

		# doto some starting position params
		fY = self.HEADINGS_TOP # top position start for the panel y axis
		line_seperator = 2 * self.TEXT_MARGIN  # space between top of the panel start
		govPnelLength = constLength - 240


		# create a list of civic options and its civics in an orderly fashion
		# the reason for the civic option loop is to ste the nested list in order.
		# the out put will be:
		# self.m_allParentsCivics = [['Despotism'], ['Hereditary Rule']....n]
		# self.m_allChildCivics = { 0: [], 1: [['RULE1', 'RULE2'], ['RULE3', 'RULE4'] ....n]}
		for i in xrange (gc.getNumCivicOptionInfos()):
			tempCivicOption_l = [] # all the civics of this civic option
			for j in xrange(gc.getNumCivicInfos()):
				if i == gc.getCivicInfo(j).getCivicOptionType() :
					childNum = gc.getCivicInfo(j).getNumParentCivicsChildren()
					if childNum > 0 :
						tempCivicOption_l.append([gc.getCivicInfo(j).getDescription()])
						# populate the child civics , each nested list is a column of the 
						# parent dependant civic (same index of the list of m_allParentsCivics)
						for m in xrange (gc.getNumCivicOptionInfos()):
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
			szAreaID = self.AREA_NAME + str(l) + str(l) + str(l)
			# doto civic parent ->hard coded first civic option name...too tired to write a loop just for this...
			screen.setText("", "Background",u"<font=3>" + u"<color=255,255,0,255>%s</color>" % gc.getCivicOptionInfo(0).getDescription() + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, self.GOV_CIVIC_HEADER + self.PANEL_BOX_ADJUSTER-10, 28, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			for i in xrange(len(civicList[l])):
				doPivotLayoutCivics(i, civicList[l][i], True, govPnelLength, fY, line_seperator - 15,  0, PanelStyles.PANEL_STYLE_BLUE50)
																									# CAN BE ALSO->	civicList[l]
			# self.drawCivicOptionButtons(l)

		# place the children civic options
		# hard coded position parameters for 2 children civic options
		fy_l = [45, 70]
		panelLength_l = [145, 160]
		panelPos = [85, 155]
		counter_pos = 0
		for j in self.m_allChildCivics.keys():
			if len(self.m_allChildCivics[j]) > 0:
				fY += fy_l[counter_pos]
				panelLength_l_val = constLength - panelLength_l[counter_pos]
				civicChild_l = self.m_allChildCivics[j]
				szAreaID = self.AREA_NAME + str(j) + str(j) + str(j)
				screen.addPanel(szAreaID, "", "", True, False, 0, panelPos[counter_pos], self.W_SCREEN, 25, PanelStyles.PANEL_STYLE_MAIN_TAN ) # 176
				screen.setText("", "Background",u"<font=3>" + u"<color=255,255,0,255>%s</color>" % gc.getCivicOptionInfo(j).getDescription()  + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, self.GOV_CIVIC_HEADER + self.PANEL_BOX_ADJUSTER-10, panelPos[counter_pos] + 3, 0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				for i in xrange(len(civicChild_l)):
					doPivotLayoutCivics(i, civicChild_l[i], False , panelLength_l_val, fY, line_seperator, j, PanelStyles.PANEL_STYLE_CITY_COLUMNL)
				# # the the Labor civic section
				counter_pos += 1

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
		# doto 115 goverment screen / civic parent child
		# cvgovermentscreen sends a call from canDoCivics which checks if to draw the civic buttons.
		# i added the true , cvgovermentscreen_ignore_hide so the display of civics would display despite the 
		# getGovermentConversionCounter being 0. that is because if the counter is 0 it would show the civics as attainable,
		# which is true, but its confusing cause it feels like player just havnt learnt them yet.
		#screen.setState(self.getCivicsButtonName(j), False)
		# also, allow selecting a civic child if its from another parent.
		if (not activePlayer.canDoCivics(iCivic, True)):
			# If you can't even do this, get out....
			return 0

		whichCivicstoHighlight = self.highlight_parent_child_group(iCivic, False, 'select')
		if (len(whichCivicstoHighlight) == 0):
			return

		self.m_aParentGroupHighlightCivics = [] # clear all marked highlighet so the new selection will be marked.
		for civic in whichCivicstoHighlight:
			iCivicOption = gc.getCivicInfo(civic).getCivicOptionType()

			# these civic types has a highlighted civics in them
			self.m_aParentGroupHighlightCivics.append(int(str(civic)))

			# Set the previous widget
			iCivicPrev = self.m_paeCurrentCivics[iCivicOption]

			# Switch the widgets
			self.m_paeCurrentCivics[iCivicOption] = civic
			
			# Unighlight the previous widget
			self.unHighlight(iCivicPrev)
			self.getScreen().setState(self.getCivicsButtonName(iCivicPrev), False)

			# highlight the new widget
			self.highlight(iCivic)		
			self.getScreen().setState(self.getCivicsButtonName(civic), True)
		
		return 0

	def CivicsButton(self, inputClass):
		
		# doto start
		input_ = gc.getCivicInfo(inputClass.getID()).getCivicOptionType()
		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			if (inputClass.getFlags() & MouseFlags.MOUSE_RBUTTONUP):
				CvScreensInterface.pediaJumpToCivic((inputClass.getID(), ))
			else:
				# Select button
				self.select(inputClass.getID())
				whichCivicstoHighlight = self.highlight_parent_child_group(inputClass.getID(), False, 'CivicsButton')
				if (len(whichCivicstoHighlight) == 0):
					return
				for civic in whichCivicstoHighlight:
					self.drawHelpText(gc.getCivicInfo(civic).getCivicOptionType(), gc.getCivicInfo(civic).getCivicOptionType())
					self.updateAnarchy()
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_ON) :
			# Highlight this button
			if gc.getCivicOptionInfo(input_).getParentCivicOption() == 2:
				whichCivicstoHighlight = self.highlight_parent_child_group(inputClass.getID(), False, 'CivicsButton')
				if (len(whichCivicstoHighlight) == 0):
					return
				for civic in whichCivicstoHighlight:
					if self.highlight(civic):
						self.drawHelpText(gc.getCivicInfo(civic).getCivicOptionType(), gc.getCivicInfo(civic).getCivicOptionType())
						self.updateAnarchy()
			else:		
				if self.highlight(inputClass.getID()):
					self.drawHelpText(input_, gc.getCivicInfo(inputClass.getID()).getCivicOptionType())
					self.updateAnarchy()
		elif (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_OFF) :
			if gc.getCivicOptionInfo(input_).getParentCivicOption() == 2:
				whichCivicstoHighlight = self.highlight_parent_child_group(inputClass.getID(), False, 'CivicsButton')
				if (len(whichCivicstoHighlight) == 0):
					return
				for civic in whichCivicstoHighlight:
					if self.unHighlight(civic):
						self.drawHelpText(gc.getCivicInfo(civic).getCivicOptionType(), gc.getCivicInfo(civic).getCivicOptionType())
						self.updateAnarchy()
			else:
				if self.unHighlight(inputClass.getID()):
					self.drawHelpText(input_, gc.getCivicInfo(inputClass.getID()).getCivicOptionType())
					self.updateAnarchy()

		return 0

		
	def drawHelpText(self, iCivicOption, true_index = -1):
		
		if true_index == -1:
			true_index = iCivicOption
		# doto 
		# heavily edited for doto, complete rewrite
		szPaneID = "CivicsHelpTextBackground" + str(true_index) + str(true_index) 
		# doto end 

		activePlayer = gc.getPlayer(self.iActivePlayer)
		iCivic = self.m_paeDisplayCivics[iCivicOption]
		screen = self.getScreen()
		szUpkeepText = u""

		# Upkeep string
		if ((gc.getCivicInfo(iCivic).getUpkeep() != -1) and not activePlayer.isNoCivicUpkeep(iCivicOption)):
			szUpkeepText = gc.getUpkeepInfo(gc.getCivicInfo(iCivic).getUpkeep()).getDescription()
		else:
			szUpkeepText = localText.getText("TXT_KEY_CIVICS_SCREEN_NO_UPKEEP", ())

		szHelpText = CyGameTextMgr().parseCivicInfo(iCivic, False, True, True)
		fX = 5
		fY = self.HELP_TOP + self.TEXT_MARGIN + -75
		if true_index > 0:
			fY = fY + (true_index * self.TEXT_BOX_SIZE - self.TEXT_BOX_SEPERATOR)
		civic_option_text = gc.getCivicOptionInfo(iCivicOption).getDescription().upper() + "--> "
		civic_text = gc.getCivicInfo(self.m_paeDisplayCivics[iCivicOption]).getDescription().upper()  + " --" +  szUpkeepText
		screen.setLabel(self.HELP_HEADER_NAME + str(true_index) + "1", "Background",  u"<font=3b>" + u"<color=205,160,55,255>%s</color>" % civic_option_text + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, fX * 3, fY, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.setLabel(self.HELP_HEADER_NAME + str(true_index) + "2", "Background",  u"<font=3>" + civic_text + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, fX + self.HEADINGS_WIDTH/2 + 20 + (len(civic_option_text) + len(civic_text) ) * 5, fY, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		szHelpAreaID = self.HELP_AREA_NAME + str(true_index)
		screen.addMultilineText(szHelpAreaID,  szHelpText , fX, fY, self.TEXT_BOX_WIDTH, self.TEXT_BOX_SIZE , WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
		
		
	# Will draw the help text
	def drawAllHelpText(self):
		# doto 
		# heavily edited for doto, complete rewrite

		counter_dependant = 0
		const_fY = 	self.TEXT_BOX_Y_START
		for i in range (gc.getNumCivicOptionInfos()):		

			if gc.getCivicOptionInfo(i).getParentCivicOption() > 0:
				# parent child civics
				szPaneID = "CivicsHelpTextBackground2" + str(counter_dependant) + str(i) # was i
				screen = self.getScreen()
				screen.addPanel(szPaneID, "", "", True, True, 0, const_fY, self.TEXT_BOX_WIDTH, self.TEXT_BOX_SIZE, PanelStyles.PANEL_STYLE_MAIN)
				self.drawHelpText(i, counter_dependant)
				counter_dependant += 1
				const_fY += self.TEXT_BOX_SIZE - self.TEXT_BOX_SEPERATOR	


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
			# doto make sure not to update anything if there was no change to the current active civics.
			# i added this mostly to avoid having the set_civic button appear after some messing with the new buttons.
			if (self.m_paeCurrentCivics[i] == self.m_absoluteOriginalCivics[i]):
				bChange = False

			i += 1		
		
		# Make the revolution button
		screen.deleteWidget(self.EXIT_NAME)
		if (activePlayer.canRevolution(0) and bChange):
			screen.setText(self.EXIT_NAME, "Background", u"<font=4>" + "SET_CIVICS" + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_REVOLUTION, 1, 0)
			screen.show(self.CANCEL_NAME)
			screen.show(self.RESET_NAME)
		else:
			screen.setText(self.EXIT_NAME, "Background", u"<font=4>" + "BACK" + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, -1)
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

		if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED) :
			# advc.001d: Clause added to prevent revolution when viewing another civ's civics through the Debug menu

			# doto 115 - removed handled in the main civic revolition button -> just passing the civics to the main screen
			# if self.iActivePlayer == gc.getGame().getActivePlayer() and activePlayer.canRevolution(0):
			# 	messageControl = CyMessageControl()
			# 	messageControl.sendUpdateCivics(self.m_paeDisplayCivics)
			self.m_aParentGroupHighlightCivics = []
			screen = self.getScreen()
			screen.hideScreen()

			# doto 115 back to the main civic screen start
			# if a civic was changed in this screen , update it to the sent civic from the main screen.
			for idx, civic in enumerate(self.m_paeOriginalCivics):
				if civic != self.m_paeDisplayCivics[idx]:
					self.allDeliveredCivics[idx] = self.m_paeDisplayCivics[idx]
			CvScreensInterface.showCivicsScreen(self.allDeliveredCivics, 'government')
			# doto 115 back to the main civic screen end

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
				self.m_aParentGroupHighlightCivics = [];
				# doto civics parent end

			self.drawContents()
	

	def Reset(self, inputClass):
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
				self.m_paeCurrentCivics[i] = self.m_absoluteOriginalCivics[i]
				self.m_paeDisplayCivics[i] = self.m_absoluteOriginalCivics[i]
				# doto civics parent start - im gonna assume my mod has 1 parent...
				#if gc.getCivicOptionInfo(i).getParentCivicOption() == 2:
				#	self.m_highLighterParent = activePlayer.getCivics(i);
				self.m_allChildCivics[i] = [];
				self.m_aParentGroupHighlightCivics = [];
				# doto civics parent end

			self.drawContents()

			# re add the exit button
			screen.setText(self.EXIT_NAME, "Background", u"<font=4>" + "BACK" + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, -1)
			


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
			'Calls function mapped in CvGevernmentScreen'
			# only get from the map if it has the key

			# get bound function from map and call it
			self.CivicsScreenInputMap.get(inputClass.getFunctionName())(inputClass)
			return 1
		return 0
		

	def update(self, fDelta):
		return

#doto 115 goverment screen
	def is_parent_child_goverment_civic(self, eCivicOption):

		is_parent_or_child = gc.getCivicOptionInfo(eCivicOption).getParentCivicOption()
		if is_parent_or_child:
			if is_parent_or_child > 0:
				return True

		return False
		
	def how_many_partent_child_civic_options(self):
		total = gc.getNumCivicOptionInfos()
		parent_or_child_cnt = 0
		for i in xrange(total):
			if self.is_parent_child_goverment_civic(i):
				parent_or_child_cnt += 1

		return parent_or_child_cnt
		

	def draw_era_convert_info(self):
		"""
			adds the era info box
		"""
		activePlayer = gc.getPlayer(self.iActivePlayer)
		curr_era = activePlayer.getCurrentEra()
		era_name = gc.getEraInfo(curr_era).getDescription()
		text_l = []
		for i in range (gc.getNumCivicOptionInfos()):
			if self.is_parent_child_goverment_civic(i):
				civic_option = gc.getCivicOptionInfo(i).getDescription().upper()
				government_conversion_count = activePlayer.getGovermentConversionCounter(i)
				text_l.append([civic_option, government_conversion_count])

		szPaneID = "eraConvertsionDetails"
		screen = self.getScreen()
		margin = 30
		fx = self.X_CANCEL + 20
		fx_plus =  fx + 10
		screen.addPanel(szPaneID + "Background", "", "", True, True, fx, self.HELP_TOP + self.TEXT_MARGIN + -95 , self.TEXT_BOX_WIDTH, self.TEXT_BOX_SIZE * 3 - self.TEXT_BOX_SEPERATOR - 15, PanelStyles.PANEL_STYLE_MAIN)
		fy =  self.HELP_TOP + self.TEXT_MARGIN -90
		screen.setText(szPaneID + "label", "Background",u"<font=4b>" + "" + u"<color=205,180,55,255>%s</color>" % "  Era Revolutions Per Civic" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY,fx_plus , fy, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		fy += margin
		screen.setText(szPaneID + "era", "Background",u"<font=3b>" + "  Current Era: " + u"<color=245,130,55,255>%s</color>" % era_name  + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, fx_plus, fy , self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		fy += margin + 5
		screen.setText(szPaneID + "revo", "Background",u"<font=2b>" + "  Civic Type: --- Available Revolutions:"  + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, fx_plus, fy, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setText(szPaneID + "--", "Background",u"<font=2>" + "----------------------------------------------------------"  + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, fx_plus, fy+10, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		fy += margin + 15
		for ii in xrange(len(text_l)):
			if text_l[ii][1] == 0:
				cnt = u"<color=196,30,58,255>%s</color>" % str(text_l[ii][1])
			else:
				cnt = u"<color=34,139,34,255>%s</color>" % str(text_l[ii][1])

			screen.setText(szPaneID + str(ii), "Background",u"<font=3b>" + "  " + str(text_l[ii][0]) + ": " + str(cnt)  + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, fx_plus, fy, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			
			fy += margin

			if (text_l[ii][0] == 'GOVERNMENT'): # lazy hard code - should add the id to text_l as a 3rd item
				screen.setText(szPaneID + str(ii) + 'Government', "Background",u"<font=3b>" + u"<color=204,85,34,255>%s</color>" % "  Cap only for inner Goverment Changes:" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, fx_plus, fy, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
				fy += margin + 5
		
		fy += -14
		screen.setText(szPaneID + "------", "Background",u"<font=2>" + "----------------------------------------------------------" + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, fx_plus, fy, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		fy += margin - 10	
		screen.setText(szPaneID + "generalera", "Background",u"<font=3b>" + "  Revolution per Era:"  + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, fx_plus, fy, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		fy += margin	

		# this is align with the setting in the DLL , i was lazy to make a parameter or something
		longest_name = 0
		era_name = []
		cnts = []
		for ii in xrange(gc.getNumEraInfos()):
			cnt = 0
			if (ii == 0):
				cnts.append(str(1))
			elif (ii == 1):
				cnts.append(str(2))
			elif (ii in [2,3]):
				cnts.append(str(2))
			elif (ii in [4,5]):
				cnts.append(str(3))
			else:
				cnts.append(str(4))

			if (len(gc.getEraInfo(ii).getDescription()) > 0):
				longest_name = len(gc.getEraInfo(ii).getDescription())
			era_name.append(str(gc.getEraInfo(ii).getDescription()) + ": ")

		for ij in xrange(len(era_name)):
			era = era_name[ij]
			cnt = cnts[ij]
			screen.setText(szPaneID + "era" + str(ij), "Background",u"<font=2b>" + " " + era.ljust(longest_name) + " " +  str(cnt) + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, fx_plus, fy, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
			fy += margin - 10
			


	def highlight_parent_child_group(self, iCivic, shouldHighLight, who):
		"""
			handles all the group government selection and highlights
		"""
		# if who != 'select':
		# 	return []
		iCivic = int(str(iCivic))
		# def get_the_civics_to_highlight(sourceCivic, initialCivics):
		# 	whichCivicstoHighlight_inner = initialCivics
		# 	numChilds = gc.getCivicInfo(sourceCivic).getNumParentCivicsChildren()

		# 	keepOneChildPerOption = []
		# 	# we got the civic we wanna highlight, if there are other children for those civicoptions,
		# 	# we dont want them to be highlighter as well/instead
		# 	for init_civic in initialCivics:
		# 		keepOneChildPerOption.append(int(str(gc.getCivicInfo(init_civic).getCivicOptionType())))

		# 	if (numChilds > 0):
		# 		for jj in xrange(numChilds):
		# 			child = int(str(gc.getCivicInfo(sourceCivic).getParentCivicsChildren(jj)))
		# 			for i in range (gc.getNumCivicOptionInfos()):	
		# 				if i not in keepOneChildPerOption:
		# 					if child not in whichCivicstoHighlight_inner:
		# 						if child in self.m_aParentGroupHighlightCivics: 
		# 							whichCivicstoHighlight_inner.append(child)
		# 							return whichCivicstoHighlight_inner

		# 		if initialCivics == whichCivicstoHighlight_inner:
		# 			for jj in xrange(numChilds):
		# 				child = int(str(gc.getCivicInfo(sourceCivic).getParentCivicsChildren(jj)))
		# 				for i in range (gc.getNumCivicOptionInfos()):	
		# 					if i not in keepOneChildPerOption:
		# 						if child not in whichCivicstoHighlight_inner:
		# 							whichCivicstoHighlight_inner.append(child)
		# 							return whichCivicstoHighlight_inner

			# CvUtil.pyPrint("numChilds child")
			# CvUtil.pyPrint(str(numChilds))
			# if (numChilds > 0):
			# 	for jj in xrange(numChilds):
			# 		child = int(str(gc.getCivicInfo(sourceCivic).getParentCivicsChildren(jj)))
			# 		if (child in self.m_aParentGroupHighlightCivics and child != iCivic):
			# 			if (child not in whichCivicstoHighlight_inner):
			# 				whichCivicstoHighlight_inner.append(child)			
			# 			return whichCivicstoHighlight_inner

				# highlightVhild = True
				# if shouldHighLight:
				# 	highlightVhild = False
				# for j in xrange(numChilds):
				# 	child = int(str(gc.getCivicInfo(sourceCivic).getParentCivicsChildren(j)))
				# 	CvUtil.pyPrint("sagi child")
				# 	CvUtil.pyPrint(str(child))		
				# 	civicOption = int(str(gc.getCivicInfo(child).getCivicOptionType()))
				# 	if iCivic != child and activePlayer.canDoCivics(child, highlightVhild):
				# 		if (civicOption not in keepOneChildPerOption):
				# 			#  and not self.m_paeDisplayCivics[civicOption] != child and sourceCivic != child
				# 			# find the first child and use it, if the selcted civic, 
				# 			# isnt selected / highlighted, do it automatically for the child
				# 			# also, if a child was manually highlighted before, no need to high light 
				# 			# another child of that civic option that is not the one which is highlighted.
				# 			whichCivicstoHighlight_inner.append(child)
				# 			keepOneChildPerOption.append(civicOption)

			# CvUtil.pyPrint("sagi numChilds")
			# CvUtil.pyPrint(str(whichCivicstoHighlight_inner))			
			# return whichCivicstoHighlight_inner
		CvUtil.pyPrint(who)
		activePlayer = gc.getPlayer(self.iActivePlayer)
		whichCivicstoHighlight = [iCivic]
		selectedCivicType = gc.getCivicInfo(iCivic).getCivicOptionType()
		keepOneChildPerOption = [selectedCivicType]
		eParent = -1
		if gc.getCivicOptionInfo(selectedCivicType).getParentCivicOption() == 1:
			eParent = int(str(activePlayer.getCivicParent(iCivic)))
			whichCivicstoHighlight.append(eParent)
		elif gc.getCivicOptionInfo(selectedCivicType).getParentCivicOption() == 2:
			eParent = iCivic

		children = []
		all_child_civicoption_type = []
		if eParent > -1:
			numChilds = gc.getCivicInfo(eParent).getNumParentCivicsChildren()
			CvUtil.pyPrint('num childs')
			CvUtil.pyPrint(str(numChilds))
			atleastOnechildselectedinanoption = False
			if numChilds > 0:
				CvUtil.pyPrint("parent")
				CvUtil.pyPrint(gc.getCivicInfo(eParent).getDescription())
				for jjj in xrange(numChilds): 
					child_c = int(str(gc.getCivicInfo(eParent).getParentCivicsChildren(jjj)))
					children.append(child_c)
					all_child_civicoption_type.append(gc.getCivicInfo(child_c).getCivicOptionType())

				if eParent == iCivic:
					for child in children:
						if iCivic not in children:
							civicOption = int(str(gc.getCivicInfo(child).getCivicOptionType()))			
							if (civicOption not in keepOneChildPerOption and activePlayer.canDoCivics(child, True)):
								# will verify the child is truly can be selected -> activePlayer.canDoCivics(child, True)
								# used True since parent changes will not affect child changes. and will not reduce child cap
								whichCivicstoHighlight.append(child)
								keepOneChildPerOption.append(civicOption)

				elif eParent in self.m_paeDisplayCivics:
					pass
				else:
					if activePlayer.canDoCivics(eParent, False):
						# parent must be chosebale including cap wise
						for child in children:
							civicOption = int(str(gc.getCivicInfo(child).getCivicOptionType()))	
							if (civicOption not in keepOneChildPerOption and activePlayer.canDoCivics(child, False)):
								# will verify the child is truly can be selected -> activePlayer.canDoCivics(child, False)
								# used False since child changes should cost child cap.
								CvUtil.pyPrint(gc.getCivicInfo(child).getDescription())
								whichCivicstoHighlight.append(child)
								keepOneChildPerOption.append(civicOption)

				# else:		
				# 	for jj in xrange(numChilds):
				# 		child = int(str(gc.getCivicInfo(eParent).getParentCivicsChildren(jj)))
				# 		civicOption = int(str(gc.getCivicInfo(child).getCivicOptionType()))
				# 		for y in self.m_aParentGroupHighlightCivics:
				# 			if y == child and child != iCivic:
				# 				atleastOnechildselectedinanoption = True

				# 	if not atleastOnechildselectedinanoption:
				# 		for jj in xrange(numChilds):
				# 			child = int(str(gc.getCivicInfo(eParent).getParentCivicsChildren(jj)))
				# 			if child != iCivic:
				# 				whichCivicstoHighlight.append(child)
				# 				# could add acheck if in theory more chilred of more civic options exists
				# 				break




		# activePlayer = gc.getPlayer(self.iActivePlayer)
		# if (gc.getCivicOptionInfo(gc.getCivicInfo(iCivic).getCivicOptionType()).getParentCivicOption() == 1):
		# 	eParent = int(str(activePlayer.getCivicParent(iCivic)))
		# 	if (activePlayer.canDoCivics(eParent, shouldHighLight)):
		# 		whichCivicstoHighlight = get_the_civics_to_highlight(eParent, [iCivic, eParent])
		# 	else:
		# 		return []
		# elif (gc.getCivicOptionInfo(gc.getCivicInfo(iCivic).getCivicOptionType()).getParentCivicOption() == 2):
		# 	if (activePlayer.canDoCivics(iCivic, shouldHighLight)):
		# 		whichCivicstoHighlight = get_the_civics_to_highlight(iCivic, [iCivic])
		# 	else:
		# 		return []
		# else:
		# 	whichCivicstoHighlight = [iCivic]

		no_dups = []
		for i in whichCivicstoHighlight:
			if i not in no_dups:
				no_dups.append(int(str(i))) # this int str is cause getParentCivicOption previously sent civictyps enum type and not an int

		if len(no_dups) < 3:
			# there has to be 3 options 1 parent, 2 children
			# todo : get the parent, get its children civics. + civic options
			# check if at least 1 civic appears in the no_dups selecttions per civic option child
			# in case a situation comes up where a parent was chosen and only 1 child or less happens, 
			# du to tech limits, make sure not to allow the choosing of that group at all.
			eParent
			no_civicoptions_dups = []
			for ii in all_child_civicoption_type:
				if ii not in no_civicoptions_dups:
					no_civicoptions_dups.append(int(str(ii)))

			which_civ_options_are_missing = []
			for which in no_dups:
				which_civic_opttion = int(str(gc.getCivicInfo(which).getCivicOptionType()))
				if which_civic_opttion not in all_child_civicoption_type:
					which_civ_options_are_missing.append(which_civic_opttion)

			if which_civ_options_are_missing:
				for cc in which_civ_options_are_missing:
					for child in children:
						if int(str(gc.getCivicInfo(child).getCivicOptionType())) == cc:
							no_dups.append(int(str(gc.getCivicInfo(child).getCivicOptionType())))
							break

		return no_dups