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
    """
    The main Civics/Government selection screen.
    REVISED DYNAMIC: Ensures all elements scale proportionally to the current screen resolution 
    and column widths are calculated independently for the two rows.
    """

    def __init__(self):
        """Initializes screen widget IDs and base layout constants."""
        # Widget IDs (unchanged)
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
        self.DEBUG_DROPDOWN_ID = "CivicsDropdownWidget"
        self.BACKGROUND_ID = "CivicsBackground"
        self.HELP_HEADER_NAME = "CivicsScreenHeaderName"

        # --- Base Layout Constants (Proportional/Fixed) ---
        self.TOP_BAR_HEIGHT = 55
        self.BOTTOM_BAR_HEIGHT = 55
        self.HEADER_PANEL_HEIGHT = 80 # Fixed height for the Civic Option Title panel
        self.HEADINGS_SPACING = 10 # Spacing between columns (fixed small gap)
        self.TEXT_MARGIN = 14
        self.BUTTON_SIZE = 24
        self.Y_TITLE = 8 
        self.Z_SCREEN = -6.1
        self.Z_TEXT = self.Z_SCREEN - 0.2 + 10

        # Dynamic Metric Placeholders (Initialized in set_screen_resolution)
        self.W_SCREEN = 0
        self.H_SCREEN = 0
        self.CONTENT_WIDTH = 0
        self.BUTTON_AREA_WIDTH = 0
        self.X_CONTENT_START = 0
        self.X_BUTTON_AREA_START = 0
        self.ROW_HEIGHT = 0
        self.Y_ROW1_TOP = 0
        self.Y_ROW2_TOP = 0
        self.Y_ROW1_HELP_TOP = 0
        self.Y_ROW2_HELP_TOP = 0
        self.HELP_PANEL_HEIGHT = 0
        self.BOTTOM_LINE_TOP = 0
        self.X_EXIT = 0
        self.Y_EXIT = 0
        self.X_CANCEL = 0
        self.Y_CANCEL = 0
        self.x_center_70 = 0
        
        # Dynamic Width Metrics for Government Row (Top)
        self.GOV_HEADINGS_WIDTH = 0 
        self.GOV_HEADINGS_REMAINDER = 0
        
        # Dynamic Width Metrics for Regular Civics Row (Bottom)
        self.REG_HEADINGS_WIDTH = 0
        self.REG_HEADINGS_REMAINDER = 0

        # Player and Civic Data (unchanged)
        self.iActivePlayer = -1
        self.m_paeCurrentCivics = [] 
        self.m_paeDisplayCivics = []
        self.m_paeOriginalCivics = []
        self.allDeliveredCivics = []
        self.invokecivics = False
        
        # Input Map (unchanged)
        self.CivicsScreenInputMap = {
            self.EXIT_NAME: self.Revolution,
            self.CANCEL_NAME: self.Cancel,
            self.REGULAR_CIVICS_BTN_NAME: self.Civics,
            self.GOVERNMENT_BTN_NAME: self.Governments
        }
    
    def set_screen_resolution(self, screen):
        """
        Calculates dynamic screen dimensions and proportional layout metrics based on current resolution.
        """
        # --- Get current resolution ---
        self.W_SCREEN = screen.getXResolution()
        self.H_SCREEN = screen.getYResolution()
        
        # --- 70/30 Proportional Split ---
        self.CONTENT_WIDTH = int(self.W_SCREEN * 0.70)
        self.BUTTON_AREA_WIDTH = self.W_SCREEN - self.CONTENT_WIDTH 
        
        self.X_CONTENT_START = 0
        self.X_BUTTON_AREA_START = self.CONTENT_WIDTH
        
        # --- Vertical Layout ---
        self.BOTTOM_LINE_TOP = self.H_SCREEN - self.BOTTOM_BAR_HEIGHT
        
        content_height = self.BOTTOM_LINE_TOP - self.TOP_BAR_HEIGHT
        self.ROW_HEIGHT = int(content_height / 2)

        self.Y_ROW1_TOP = self.TOP_BAR_HEIGHT + 5
        self.Y_ROW2_TOP = self.Y_ROW1_TOP + self.ROW_HEIGHT
        
        # Help Text Panel Layout
        self.Y_ROW1_HELP_TOP = self.Y_ROW1_TOP + self.HEADER_PANEL_HEIGHT + 5
        self.Y_ROW2_HELP_TOP = self.Y_ROW2_TOP + self.HEADER_PANEL_HEIGHT + 5
        self.HELP_PANEL_HEIGHT = self.ROW_HEIGHT - self.HEADER_PANEL_HEIGHT - 10 
        
        # --- Button Positions (Positioned in the 30% area) ---
        self.x_center_70 = self.CONTENT_WIDTH / 2 

        self.X_EXIT = self.X_BUTTON_AREA_START + self.BUTTON_AREA_WIDTH - 20 
        self.Y_EXIT = self.BOTTOM_LINE_TOP + 5

        self.X_CANCEL = self.X_BUTTON_AREA_START + 20 
        self.Y_CANCEL = self.BOTTOM_LINE_TOP + 5

    def getScreen(self):
        """Returns the CyGInterfaceScreen instance."""
        return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.CIVICS_SCREEN)

    def setActivePlayer(self, iPlayer, civics):
        """Sets the active player and initializes the civic lists (FIXED for Python 2.4)."""
        self.iActivePlayer = iPlayer
        activePlayer = gc.getPlayer(iPlayer)
        self.m_paeCurrentCivics = []
        self.m_paeDisplayCivics = []
        self.m_paeOriginalCivics = []
        self.allDeliveredCivics = []

        num_civic_options = gc.getNumCivicOptionInfos()
        for i in range(num_civic_options):
            
            # Explicit if/else for Python 2.4 compatibility
            if civics:
                civic_id = civics[i] 
            else:
                civic_id = activePlayer.getCivics(i)
                
            self.m_paeCurrentCivics.append(civic_id)
            self.m_paeDisplayCivics.append(civic_id)
            self.allDeliveredCivics.append(civic_id)
            self.m_paeOriginalCivics.append(activePlayer.getCivics(i))

    def interfaceScreen(self, civics=None, source=None):
        """Draws the main interface screen, initializing with dynamic resolution."""
        screen = self.getScreen()
        if screen.isActive():
            return

        # 1. Calculate Dynamic Metrics
        self.set_screen_resolution(screen)
        
        screen.setRenderInterfaceOnly(True)
        screen.setDimensions(0, 0, self.W_SCREEN, self.H_SCREEN)
        screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

        # 2. Setup Background and Fixed Panels
        screen.addDDSGFC(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1)
        screen.addPanel("TechTopPanel", u"", u"", True, False, 0, 0, self.W_SCREEN, self.TOP_BAR_HEIGHT, PanelStyles.PANEL_STYLE_TOPBAR)
        screen.addPanel("TechBottomPanel", u"", u"", True, False, 0, self.BOTTOM_LINE_TOP, self.W_SCREEN, self.BOTTOM_BAR_HEIGHT, PanelStyles.PANEL_STYLE_BOTTOMBAR)
        screen.showWindowBackground(False)

        # 3. Set Active Player and Civic State
        self.setActivePlayer(gc.getGame().getActivePlayer(), civics)
        
        # 4. Add Fixed Widgets (Title, Debug, Cancel)
        screen.setText(self.TITLE_NAME, "Background", u"<font=5b>" + u"<color=205,180,55,255>%s</color>" % "ACTIVE CURRENT CIVICS" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY,  self.W_SCREEN / 2, self.Y_TITLE - 5, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
        screen.setText(self.CANCEL_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_SCREEN_CANCEL", ()).upper() + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, self.X_CANCEL, self.Y_CANCEL, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

        if CyGame().isDebugMode():
            self.szDropdownName = self.DEBUG_DROPDOWN_ID
            screen.addDropDownBoxGFC(self.szDropdownName, 22, 12, 300, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
            for j in range(gc.getMAX_PLAYERS()):
                if gc.getPlayer(j).isAlive():
                    screen.addPullDownString(self.szDropdownName, gc.getPlayer(j).getName(), j, j, False)

        # 5. Add Bottom Line Panel (Covers 70% content area)
        screen.addPanel("CivicsBottomLine", "", "", True, True, self.X_CONTENT_START, self.BOTTOM_LINE_TOP, self.W_SCREEN, self.BOTTOM_BAR_HEIGHT, PanelStyles.PANEL_STYLE_MAIN)

        # 6. Draw dynamic contents
        self.drawContents()

        return 0

    #-----------------------------------------------------------------------

    def update(self, fDelta):
        """Called every frame. Currently unused. Must be present."""
        pass 

    def drawContents(self):
        """
        Redraws all dynamic content on the screen.
        Calculates dynamic column widths separately for the Government and Regular Civic rows.
        """
        screen = self.getScreen()
        
        # --- Widget Deletion Code ---
        num_civic_options = gc.getNumCivicOptionInfos()
        for i in range(num_civic_options):
            for j in self.allDeliveredCivics:
                screen.deleteWidget(self.getCivicsButtonName(j))
                screen.deleteWidget(self.getCivicsTextName(j))
            screen.deleteWidget("CivicsHelpTextBackground" + str(i) + "True")
            screen.deleteWidget("CivicsHelpTextBackground" + str(i) + "False")
            screen.deleteWidget(self.AREA_NAME + str(i) + "True")
            screen.deleteWidget(self.AREA_NAME + str(i) + "False")
            screen.deleteWidget(self.HELP_AREA_NAME + str(i) + "True")
            screen.deleteWidget(self.HELP_AREA_NAME + str(i) + "False")
        
        # --- DYNAMIC COLUMN WIDTH CALCULATION (FIXED & INDEPENDENT) ---
        
        gov_cols = sum(1 for i in range(num_civic_options) if self.is_parent_child_goverment_civic(i))
        reg_cols = num_civic_options - gov_cols
        
        def calculate_width_metrics(col_count):
            if col_count == 0:
                return 0, 0
            
            # Usable Inner Width = Total Width (70%) - Left Margin - Right Margin
            # We use two margins to ensure the content is visibly inset slightly from the 70% panel edges
            usable_inner_width = self.CONTENT_WIDTH - (self.TEXT_MARGIN * 2) 

            # Total space taken by SPACING between columns
            total_spacing_width = self.HEADINGS_SPACING * (col_count - 1) * 0 # KELDATH ZEROED
            
            # Total width dedicated to the COLUMN PANELS
            total_panel_width_needed = usable_inner_width - total_spacing_width
            
            # Base width of each column (truncated by integer division)
            base_width = total_panel_width_needed / col_count
            
            # Calculate and store the REMAINING PIXELS (for distribution)
            remainder = total_panel_width_needed % col_count
            return base_width, remainder

        # Calculate metrics for Government Row
        self.GOV_HEADINGS_WIDTH, self.GOV_HEADINGS_REMAINDER = calculate_width_metrics(gov_cols)
        
        # Calculate metrics for Regular Civics Row
        self.REG_HEADINGS_WIDTH, self.REG_HEADINGS_REMAINDER = calculate_width_metrics(reg_cols)

        # --- Draw Components ---
        self.updateAnarchy()
        
        # Pass the specific metrics for each row
        self.drawAllButtons(True, gov_cols, self.GOV_HEADINGS_WIDTH, self.GOV_HEADINGS_REMAINDER)  # Government Row
        self.drawAllButtons(False, reg_cols, self.REG_HEADINGS_WIDTH, self.REG_HEADINGS_REMAINDER) # Regular Civics Row
        
        self.drawAllHelpText(True, gov_cols, self.GOV_HEADINGS_WIDTH, self.GOV_HEADINGS_REMAINDER) # Government Help
        self.drawAllHelpText(False, reg_cols, self.REG_HEADINGS_WIDTH, self.REG_HEADINGS_REMAINDER) # Regular Civics Help
        
        self.drawcCivicsScreenButtons()
    
    
    def _get_column_coordinates(self, reindex, is_government, base_width, remainder):
        """
        Calculates the X and Y coordinates for a civic column using dynamic widths.
        This function determines the start X based on the accumulated corrected widths of preceding columns.
        """
        
        # The true width of all *preceding* panels needs to be summed up to get the correct start X
        preceding_panel_width = 0
        for i in range(reindex):
            width_to_add = base_width
            if i < remainder:
                width_to_add += 1
            preceding_panel_width += width_to_add
        
        # Horizontal position: Initial Margin + Total Preceding Panel Width + Total Preceding Spacing
        total_preceding_spacing = self.HEADINGS_SPACING * reindex * 0 # KELDATH ZEROED
        fX = self.X_CONTENT_START + self.TEXT_MARGIN + preceding_panel_width + total_preceding_spacing
        
        # Vertical position
        if is_government:
            fY = self.Y_ROW1_TOP
        else:
            fY = self.Y_ROW2_TOP
            
        return fX, fY

    def drawAllButtons(self, goverment, col_count, base_width, remainder):
        """Draws the civic selection check boxes and applies pixel remainder correction."""
        screen = self.getScreen()
        reindex = 0
        
        if col_count == 0: return 

        for i in range(gc.getNumCivicOptionInfos()):
            is_parent_civic = self.is_parent_child_goverment_civic(i)
            draw_boxes = (goverment and is_parent_civic) or (not goverment and not is_parent_civic)

            if draw_boxes:
                # Calculate coordinates using the specific row metrics
                fX, fY = self._get_column_coordinates(reindex, goverment, base_width, remainder)

                # --- DYNAMIC WIDTH CORRECTION ---
                panel_width = base_width
                
                # Apply 1 pixel of the remainder to the first 'remainder' number of columns
                if reindex < remainder:
                    panel_width += 1 
                # --------------------------------

                # Civic Option Panel (Header)
                szAreaID = self.AREA_NAME + str(reindex) + str(goverment)
                screen.addPanel(szAreaID , "", "", True, True,
                                fX, fY, panel_width, self.HEADER_PANEL_HEIGHT,
                                PanelStyles.PANEL_STYLE_MAIN)
        
                # Civic Option Title 
                screen.setLabel("", "Background", 
                                u"<font=3>" + gc.getCivicOptionInfo(i).getDescription().upper() + u"</font>",
                                CvUtil.FONT_CENTER_JUSTIFY,
                                fX + panel_width / 2, fY + self.TEXT_MARGIN, 0,
                                FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
        
                fY_current = fY + self.HEADER_PANEL_HEIGHT / 2 # + self.TEXT_MARGIN 
                
                # Draw individual civics for this option
                for j in self.allDeliveredCivics:
                    if gc.getCivicInfo(j).getCivicOptionType() == i:
                        
                        # Checkbox Button (Left-aligned with panel)
                        screen.addCheckBoxGFC(self.getCivicsButtonName(j), gc.getCivicInfo(j).getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), 
                                            fX + self.TEXT_MARGIN, fY_current, self.BUTTON_SIZE, self.BUTTON_SIZE, 
                                            WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
        
                        # Civic Name Text (Right of the button)
                        screen.setText(self.getCivicsTextName(j), "", gc.getCivicInfo(j).getDescription(), 
                                        CvUtil.FONT_LEFT_JUSTIFY, fX + (self.BUTTON_SIZE / 2) + self.TEXT_MARGIN * 2, fY_current, 0, 
                                        FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
                        
                        fY_current += self.BUTTON_SIZE + self.TEXT_MARGIN / 2 
                        
                reindex += 1


    def drawAllHelpText(self, goverment, col_count, base_width, remainder):
        """Draws the help text panels for all civic options for one row, applying pixel correction."""
        screen = self.getScreen()
        re_index = 0
        
        if col_count == 0: return 

        for i in range(gc.getNumCivicOptionInfos()):
            is_parent_civic = self.is_parent_child_goverment_civic(i)
            draw_boxes = (goverment and is_parent_civic) or (not goverment and not is_parent_civic)

            if draw_boxes:
                # Calculate coordinates using the specific row metrics
                fX, fY_header = self._get_column_coordinates(re_index, goverment, base_width, remainder)

                # Determine Y position for the help panel (below the header panel)
                if goverment:
                    fY = self.Y_ROW1_HELP_TOP
                else:
                    fY = self.Y_ROW2_HELP_TOP

                # --- DYNAMIC WIDTH CORRECTION ---
                panel_width = base_width
                if re_index < remainder:
                    panel_width += 1 
                # --------------------------------

                szPaneID = "CivicsHelpTextBackground" + str(re_index) + str(goverment)
                
                # Add background panel for the help text
                screen.addPanel(szPaneID, "", "", True, True, fX, fY, 
                                panel_width, self.HELP_PANEL_HEIGHT, 
                                PanelStyles.PANEL_STYLE_MAIN)
        
                # Call drawHelpText with the corrected width
                self.drawHelpText(i, re_index, goverment, panel_width, base_width, remainder) 
                re_index += 1


    def drawHelpText(self, iCivicOption, true_index, goverment, corrected_width, base_width, remainder):
        """Draws the detailed help text for a single civic option inside its help panel."""
        
        activePlayer = gc.getPlayer(self.iActivePlayer)
        iCivic = self.allDeliveredCivics[iCivicOption]

        screen = self.getScreen()
        szHelpText = u""

        # Upkeep string and Civic Effects
        civic_info = gc.getCivicInfo(iCivic)
        if civic_info.getUpkeep() != -1 and not activePlayer.isNoCivicUpkeep(iCivicOption):
            szHelpText = gc.getUpkeepInfo(civic_info.getUpkeep()).getDescription()
        else:
            szHelpText = localText.getText("TXT_KEY_CIVICS_SCREEN_NO_UPKEEP", ())

        szHelpText += CyGameTextMgr().parseCivicInfo(iCivic, False, True, True)

        # Recalculate X position using the correct base width and remainder
        fX, fY_header = self._get_column_coordinates(true_index, goverment, base_width, remainder)

        if goverment:
            fY = self.Y_ROW1_HELP_TOP + 5
        else:
            fY = self.Y_ROW2_HELP_TOP + 5
            
        szHelpAreaID = self.HELP_AREA_NAME + str(true_index) + str(goverment)
        
        # Draw multiline text inside the panel (subtract padding)
        screen.addMultilineText(szHelpAreaID, szHelpText, fX + 5, fY, 
                                corrected_width - 7, self.HELP_PANEL_HEIGHT - 10, 
                                WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)

    #-----------------------------------------------------------------------
        
    def getCivicsButtonName(self, iCivic):
        """Generates a unique name for a civic's button widget."""
        return self.BUTTON_NAME + str(iCivic)

    def getCivicsTextName(self, iCivic):
        """Generates a unique name for a civic's text widget."""
        return self.TEXT_NAME + str(iCivic)
        
    #-----------------------------------------------------------------------

    def drawcCivicsScreenButtons(self):
        """Draws the large navigation buttons in the 30% area of the screen."""
        screen = self.getScreen()
        
        # Center the buttons within the 30% area
        button_width = self.BUTTON_AREA_WIDTH - 40 # 20px padding on left/right
        button_height = int(self.ROW_HEIGHT / 2) 
        
        x_center = self.X_BUTTON_AREA_START + (self.BUTTON_AREA_WIDTH / 2)
        
        # Government Button (Aligned with the top row's vertical space)
        y_gov_panel = self.Y_ROW1_TOP + (self.ROW_HEIGHT / 2) - (button_height / 2)
        y_gov_text = y_gov_panel + (button_height / 2) - self.TEXT_MARGIN

        screen.addPanel('government_panel' + "Background", "", "", True, True, 
                        x_center - (button_width / 2), y_gov_panel, 
                        button_width, button_height, PanelStyles.PANEL_STYLE_MAIN)
        
        screen.setText(self.GOVERNMENT_BTN_NAME, "Background", u"<font=4>" + "CHANGE GOVERNMENTS" + u"</font>", 
                        CvUtil.FONT_CENTER_JUSTIFY, x_center, y_gov_text, 
                        self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)
        
        # Civics Button (Aligned with the bottom row's vertical space)
        y_civ_panel = self.Y_ROW2_TOP + (self.ROW_HEIGHT / 2) - (button_height / 2)
        y_civ_text = y_civ_panel + (button_height / 2) - self.TEXT_MARGIN

        screen.addPanel('civics_panel' + "Background", "", "", True, True, 
                        x_center - (button_width / 2), y_civ_panel, 
                        button_width, button_height, PanelStyles.PANEL_STYLE_MAIN)
        
        screen.setText(self.REGULAR_CIVICS_BTN_NAME, "Background", u"<font=4>" + "CHANGE CIVICS" + u"</font>", 
                        CvUtil.FONT_CENTER_JUSTIFY, x_center, y_civ_text, 
                        self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)


    def updateAnarchy(self):
        """Updates the Revolution button state, anarchy turns, and civic upkeep text."""
        screen = self.getScreen()
        activePlayer = gc.getPlayer(self.iActivePlayer)
        bChange = False
        i = 0
        while (i < gc.getNumCivicOptionInfos() and not bChange):
            if (self.m_paeCurrentCivics[i] != self.m_paeOriginalCivics[i]):
                bChange = True
            i += 1 
        
        # Revolution/Exit Button (30% area on the bottom bar)
        screen.deleteWidget(self.EXIT_NAME)
        Y_EXIT_adjust = self.Y_EXIT + 5
        if activePlayer.canRevolution(0) and bChange:
            screen.setText(self.EXIT_NAME, "Background", 
                            u"<font=4>" + localText.getText("TXT_KEY_CONCEPT_REVOLUTION", ()).upper() + u"</font>", 
                            CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, Y_EXIT_adjust, self.Z_TEXT, 
                            FontTypes.TITLE_FONT, WidgetTypes.WIDGET_REVOLUTION, 1, 0)
            screen.show(self.CANCEL_NAME)
        else:
            screen.setText(self.EXIT_NAME, "Background", 
                            u"<font=4>" + localText.getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + u"</font>", 
                            CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, Y_EXIT_adjust, self.Z_TEXT, 
                            FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, -1)
            if bChange: 
                screen.show(self.CANCEL_NAME)
            else:
                screen.hide(self.CANCEL_NAME)


        # Anarchy Text (Centered in 70% bottom panel)
        iTurns = activePlayer.getCivicAnarchyLength(self.m_paeDisplayCivics)
        if activePlayer.canRevolution(0):
            szText = localText.getText("TXT_KEY_ANARCHY_TURNS", (iTurns,))
        else:
            szText = CyGameTextMgr().setRevolutionHelp(self.iActivePlayer)
            
        y_text = self.BOTTOM_LINE_TOP + (self.BOTTOM_BAR_HEIGHT / 2) - self.TEXT_MARGIN + 5
        screen.setLabel("CivicsRevText", "Background", u"<font=3>" + szText + u"</font>", 
                        CvUtil.FONT_CENTER_JUSTIFY, self.x_center_70, y_text, 0, 
                        FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

        # Upkeep/Maintenance Text (Left-justified in 70% bottom panel)
        base_upkeep = activePlayer.getCivicUpkeep(self.m_paeDisplayCivics, True)
        inflation_rate = activePlayer.calculateInflationRate()
        final_upkeep = base_upkeep * (100 + inflation_rate) / 100
        szText = localText.getText("TXT_KEY_CIVIC_SCREEN_UPKEEP", (final_upkeep,))
        
        screen.setLabel("CivicsUpkeepText", "Background", u"<font=3>" + szText + u"</font>", 
                        CvUtil.FONT_LEFT_JUSTIFY, self.X_CONTENT_START + self.TEXT_MARGIN, y_text, 0, 
                        FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)


    def Civics(self, inputClass):
        """Handles the click event for the 'CHANGE CIVICS' button."""
        if inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            self.getScreen().hideScreen()
            CvScreensInterface.showRegularCivicScreen(self.allDeliveredCivics)

    def Governments(self, inputClass):
        """Handles the click event for the 'CHANGE GOVERNMENTS' button."""
        if inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            self.getScreen().hideScreen()
            CvScreensInterface.showGovermentScreen(self.allDeliveredCivics)

    def Revolution(self, inputClass):
        """Handles the click event for the 'Revolution' or 'Exit' button."""
        if inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            activePlayer = gc.getPlayer(self.iActivePlayer)
            
            if self.iActivePlayer == gc.getGame().getActivePlayer() and activePlayer.canRevolution(0):
                CyMessageControl().sendUpdateCivics(self.m_paeDisplayCivics)
                
            self.getScreen().hideScreen()

    def Cancel(self, inputClass):
        """Handles the click event for the 'Cancel' button."""
        if inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            for i in range (gc.getNumCivicOptionInfos()):
                self.m_paeCurrentCivics[i] = self.m_paeOriginalCivics[i]
                self.m_paeDisplayCivics[i] = self.m_paeOriginalCivics[i]
                self.allDeliveredCivics[i] = self.m_paeOriginalCivics[i]
            
            # Recalculate and redraw everything
            self.set_screen_resolution(self.getScreen())
            self.drawContents()

    def handleInput(self, inputClass):
        """Manages input events from the screen widgets."""
        if (inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED and 
            hasattr(self, 'DEBUG_DROPDOWN_ID') and inputClass.getFunctionName() == self.DEBUG_DROPDOWN_ID):
            
            screen = self.getScreen()
            iIndex = screen.getSelectedPullDownID(self.DEBUG_DROPDOWN_ID)
            self.setActivePlayer(screen.getPullDownData(self.DEBUG_DROPDOWN_ID, iIndex), None)
            
            # Recalculate and redraw everything
            self.set_screen_resolution(screen)
            self.drawContents()
            return 1
            
        elif self.CivicsScreenInputMap.has_key(inputClass.getFunctionName()):
            self.CivicsScreenInputMap.get(inputClass.getFunctionName())(inputClass)
            return 1
            
        return 0

    def is_parent_child_goverment_civic(self, eCivicOption):
        """Checks if a civic option is designated as a 'government' type (is a child civic)."""
        is_parent_or_child = gc.getCivicOptionInfo(eCivicOption).getParentCivicOption()
        
        if is_parent_or_child is not None and is_parent_or_child > 0:
            return True

        return False