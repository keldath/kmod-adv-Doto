## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
## Custom Government/Parent-Child Civics Screen (Final Dynamic Layout - Python 2.4 - NO SCROLL)
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

class CvGovermentScreen:
    """
    Government Screen (doto mod)

    This screen displays a custom parent/child civic structure, featuring dynamic 
    positioning and Python 2.4 compatibility.
    """

    def __init__(self):
        """Initializes screen elements, layout constants, and state variables."""
        # --- Widget IDs ---
        self.SCREEN_NAME = "GovermentScreen"
        self.CANCEL_NAME = "GovermentCancel"
        self.RESET_NAME = "GovermentReset"
        self.EXIT_NAME = "GovermentExit"
        self.TITLE_NAME = "GovermentTitleHeader"
        self.BUTTON_NAME = "GovermentScreenButton"
        self.TEXT_NAME = "GovermentScreenText"
        self.AREA_NAME = "GovermentScreenArea"
        self.HELP_AREA_NAME = "GovermentScreenHelpArea"
        self.HELP_HEADER_NAME = "GovermentScreenHeaderName"
        self.BACKGROUND_ID = "GovermentBackground"
        self.DEBUG_DROPDOWN_ID = "GovermentDropdownWidget"

        # --- Base Layout Constants ---
        self.W_SCREEN = 1360
        self.H_SCREEN = 768
        self.Z_SCREEN = -6.1
        self.Z_TEXT = self.Z_SCREEN - 0.2 + 10
        self.Y_TITLE = 8
        self.TEXT_MARGIN = 14
        self.BUTTON_SIZE = 24
        self.CIVIC_ROW_HEIGHT = 28 # Estimated height for a single line civic name/button row

        # --- Custom Layout Constants (for Parent/Child Table) ---
        self.HEADINGS_WIDTH = 174
        self.HEADINGS_SPACING = -4
        self.HEADINGS_TOP = 50
        
        # RESTORED/REFINED MISSING CONSTANTS:
        self.GOV_CIVIC_HEADER_X = 480 
        self.PANEL_BOX_ADJUSTER = 100 
        self.TEXT_BOX_SIZE = 150 # Height of a single help text panel
        self.TEXT_BOX_SEPERATOR = 5 # Vertical spacing between help panels
        self.TEXT_BOX_WIDTH = self.HEADINGS_WIDTH + self.PANEL_BOX_ADJUSTER * 2 + self.PANEL_BOX_ADJUSTER/2 + 160 
        
        # --- Bottom Bar Layout ---
        self.BOTTOM_LINE_TOP = 675
        self.BOTTOM_LINE_HEIGHT = 40
        self.BOTTOM_LINE_WIDTH = 1370
        
        # Exit/Cancel/Reset Button positions
        self.X_EXIT = 1300
        self.Y_EXIT = 726
        self.X_CANCEL = 750
        self.Y_CANCEL = 726
        
        # --- Dynamic State Variables (Used to link layouts) ---
        self.P_CIVIC_END_Y = 0      
        self.A_CIVIC_END_Y = 0      
        self.HELP_PANEL_START_Y = 0 

        # --- Input Mapping and State Variables ---
        self.CivicsScreenInputMap = {
            self.BUTTON_NAME: self.handleCivicsButton,
            self.TEXT_NAME: self.handleCivicsButton,
            self.EXIT_NAME: self.handleExit,
            self.CANCEL_NAME: self.handleCancel,
            self.RESET_NAME: self.handleReset,
        }

        self.iActivePlayer = -1
        self.selected_civics = []      
        self.highlighted_civics = []   
        self.initial_civics = []       
        self.absolute_original_civics = [] 
        self.civics_to_return = []     
        self.parent_civics_layout = [] 
        self.child_civics_layout = {}  
        self.highlight_group = []      
        
    def getScreen(self):
        return CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)

    def setActivePlayer(self, iPlayer, civics=None):
        self.iActivePlayer = iPlayer
        activePlayer = gc.getPlayer(iPlayer)
        num_options = gc.getNumCivicOptionInfos()

        self.selected_civics = []
        self.highlighted_civics = []
        self.initial_civics = []
        self.absolute_original_civics = []
        self.civics_to_return = []
        self.child_civics_layout = {}
        self.highlight_group = []
        
        for i in xrange(num_options):
            if civics is not None:
                civic_id = civics[i]
            else:
                civic_id = activePlayer.getCivics(i)
            
            self.selected_civics.append(civic_id)
            self.highlighted_civics.append(civic_id)
            self.initial_civics.append(civic_id)
            self.civics_to_return.append(civic_id)
            self.absolute_original_civics.append(activePlayer.getCivics(i))
            self.child_civics_layout[i] = []
            
    def interfaceScreen(self, civics=None):
        screen = self.getScreen()
        if screen.isActive(): return
        
        screen.setRenderInterfaceOnly(True)
        screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
        
        screen.setDimensions(screen.centerX(-170), screen.centerY(0), self.W_SCREEN, self.H_SCREEN)
        bg_path = ArtFileMgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath()
        screen.addDDSGFC(self.BACKGROUND_ID, bg_path, 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1)
        
        screen.addPanel("TechTopPanel", u"", u"", True, False, 0, 0, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_TOPBAR)
        screen.addPanel("TechBottomPanel", u"", u"", True, False, 0, 713, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_BOTTOMBAR)
        screen.showWindowBackground(False)
        
        title_text = u"<font=4b><color=255,215,0,255>%s</color></font>" % localText.getText("TXT_KEY_GOVERNMENT_HEADER", ())
        screen.setText(self.TITLE_NAME, "Background", title_text, CvUtil.FONT_CENTER_JUSTIFY,
                       self.W_SCREEN / 2, self.Y_TITLE, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
        
        self.setActivePlayer(gc.getGame().getActivePlayer(), civics)

        if CyGame().isDebugMode():
            screen.addDropDownBoxGFC(self.DEBUG_DROPDOWN_ID, 22, 12, 300, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
            for j in xrange(gc.getMAX_PLAYERS()):
                if gc.getPlayer(j).isAlive():
                    screen.addPullDownString(self.DEBUG_DROPDOWN_ID, gc.getPlayer(j).getName(), j, j, False)

        screen.addPanel("CivicsBottomLine", "", "", True, True, self.HEADINGS_SPACING, self.BOTTOM_LINE_TOP,
                        self.BOTTOM_LINE_WIDTH, self.BOTTOM_LINE_HEIGHT, PanelStyles.PANEL_STYLE_MAIN)

        self.drawContents()
        return 0

    def drawContents(self):
        """Wipes and redraws all dynamic elements on the screen."""
        screen = self.getScreen()
        
        # 1. Draw Civics (Sets the dynamic Y end position: self.A_CIVIC_END_Y)
        self.drawAllButtons()
        
        # Set the Y start for the lower content area
        self.HELP_PANEL_START_Y = self.A_CIVIC_END_Y + self.TEXT_MARGIN 
        
        # 2. Draw Help Text (Content parent is the main Background panel)
        self.drawAllHelpText("Background")
        
        # 3. Draw Era Info (Content parent is the main Background panel)
        self.draw_era_convert_info("Background")

        # 4. Update Bottom Bar
        self.updateAnarchy()

    # =========================================================================
    # DYNAMIC LAYOUT HELPERS
    # =========================================================================

    def _get_civic_layout_data(self):
        """Preprocesses civic data to build layout maps and estimate max rows per column."""
        parent_civics_layout_temp = []
        child_civics_layout = {}
        max_heights = {} 
        
        for i in xrange(gc.getNumCivicOptionInfos()):
            child_civics_layout.setdefault(i, [])
            parent_civics_in_option = []
            
            for j in xrange(gc.getNumCivicInfos()):
                civic_info = gc.getCivicInfo(j)
                if i == civic_info.getCivicOptionType():
                    if civic_info.getNumParentCivicsChildren() > 0:
                        parent_civics_in_option.append(civic_info.getDescription()) 
                        
                        for m in xrange(gc.getNumCivicOptionInfos()):
                            civic_option_info = gc.getCivicOptionInfo(m)
                            if civic_option_info.getParentCivicOption() != 1: 
                                continue
                                
                            child_civics_layout.setdefault(m, [])
                            children_for_this_parent = []
                            for c in xrange(civic_info.getNumParentCivicsChildren()):
                                childCivic = gc.getCivicInfo(civic_info.getParentCivicsChildren(c))
                                if childCivic.getCivicOptionType() == m:
                                    children_for_this_parent.append(childCivic.getDescription())
                                    
                            if len(children_for_this_parent) > 0:
                                child_civics_layout[m].append(children_for_this_parent)

            if len(parent_civics_in_option) > 0:
                parent_civics_layout_temp.append([parent_civics_in_option])
            else:
                parent_civics_layout_temp.append([]) 

            if i == 0: 
                max_heights[i] = len(parent_civics_in_option)

        for option_id, list_of_civic_groups in child_civics_layout.items():
            max_count = 0
            for civic_group in list_of_civic_groups:
                if len(civic_group) > max_count:
                    max_count = len(civic_group)
            max_heights[option_id] = max_count
            
        self.parent_civics_layout = parent_civics_layout_temp
        self.child_civics_layout = child_civics_layout
        
        return max_heights

    # =========================================================================
    # DRAWING FUNCTIONS
    # =========================================================================

    def draw_era_convert_info(self, parent_id="Background"):
        """Draws the dynamic Era Revolution panel into a specified parent."""
        screen = self.getScreen()
        activePlayer = gc.getPlayer(self.iActivePlayer)
        curr_era = activePlayer.getCurrentEra()
        era_name = gc.getEraInfo(curr_era).getDescription()
        
        # --- 1. Data Collection (omitted for brevity) ---
        text_l = []
        for i in xrange(gc.getNumCivicOptionInfos()):
            if gc.getCivicOptionInfo(i).getParentCivicOption() > 0 or i == 0:
                civic_option = gc.getCivicOptionInfo(i).getDescription().upper()
                government_conversion_count = activePlayer.getGovermentConversionCounter(i)
                text_l.append([civic_option, government_conversion_count, i]) 

        # --- 2. Panel and Layout Setup (Relative to Background Panel) ---
        
        # FIX: Python 2.4 Conditional Assignment
        if self.HELP_PANEL_START_Y > 0:
            # If dynamic Y is set, use it directly (relative to Background)
            Y_START_ADJUSTED = self.HELP_PANEL_START_Y
        else:
            # Fallback position 
            Y_START_ADJUSTED = self.HEADINGS_TOP + 350


        FX = self.TEXT_BOX_WIDTH + self.TEXT_MARGIN 
        FX_PLUS = FX + 10
        MARGIN = 30
        H_PANEL = self.TEXT_BOX_SIZE * 3 - self.TEXT_BOX_SEPERATOR - 15
        W_PANEL = 320 
        szPaneID = "eraConvertsionDetails"
        
        # Add Panel: Note: parent_id is NOT an argument for addPanel
        screen.addPanel(szPaneID + "Background", "", "", True, True, 
                        FX, Y_START_ADJUSTED, W_PANEL, H_PANEL, 
                        PanelStyles.PANEL_STYLE_MAIN)
                        
        # --- 3. Drawing Content (Aligned to FX_PLUS and iterating FY) ---
        FY = Y_START_ADJUSTED + 5 

        # Title: Era Revolutions Per Civic
        title_text = u"<font=4b><color=205,180,55,255>%s</color></font>" % "Era Revolutions Per Civic"
        screen.setText(szPaneID + "label", parent_id, title_text, CvUtil.FONT_LEFT_JUSTIFY, FX_PLUS, FY, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
        FY += MARGIN

        # Current Era
        era_text = u"<font=3b>Current Era: <color=245,130,55,255>%s</color></font>" % era_name
        screen.setText(szPaneID + "era", parent_id, era_text, CvUtil.FONT_LEFT_JUSTIFY, FX_PLUS, FY , self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
        FY += MARGIN + 5
        
        # Separator Header
        screen.setText(szPaneID + "revo", parent_id,u"<font=2b>Civic Type: --- Available Revolutions:</font>", CvUtil.FONT_LEFT_JUSTIFY, FX_PLUS, FY, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
        screen.setText(szPaneID + "--", parent_id,u"<font=2>----------------------------------------------------------</font>", CvUtil.FONT_LEFT_JUSTIFY, FX_PLUS, FY+10, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
        
        FY += MARGIN + 15
        
        for civic_data in text_l:
            civic_option, count, _ = civic_data
            
            # FIX: Python 2.4 Conditional Assignment
            if count == 0:
                cnt_str = u"<color=196,30,58,255>%s</color>" % str(count)
            else:
                cnt_str = u"<color=34,139,34,255>%s</color>" % str(count)

            civic_text = u"<font=3b>%s: %s</font>" % (civic_option, cnt_str)
            screen.setText(szPaneID + str(civic_option), parent_id, civic_text, CvUtil.FONT_LEFT_JUSTIFY, FX_PLUS, FY, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
            
            FY += MARGIN

            if civic_option == 'GOVERNMENT':
                gov_cap_text = u"<font=3b><color=204,85,34,255>%s</color></font>" % "Cap only for inner Goverment Changes:"
                screen.setText(szPaneID + 'GovernmentMsg', parent_id, gov_cap_text, CvUtil.FONT_LEFT_JUSTIFY, FX_PLUS, FY, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
                FY += MARGIN + 5
        
        # Revolutions Per Era List
        FY += -14
        screen.setText(szPaneID + "------", parent_id,u"<font=2>----------------------------------------------------------</font>", CvUtil.FONT_LEFT_JUSTIFY, FX_PLUS, FY, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
        
        FY += MARGIN - 10 
        screen.setText(szPaneID + "generalera", parent_id,u"<font=3b>Revolution per Era:</font>", CvUtil.FONT_LEFT_JUSTIFY, FX_PLUS, FY, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
        FY += MARGIN
        
        era_names = []
        era_counts = []
        longest_name = 0
        
        for ii in xrange(gc.getNumEraInfos()):
            era_description = gc.getEraInfo(ii).getDescription()
            era_names.append(era_description + ": ")
            
            # Hardcoded Era Revolution Counts
            if ii == 0: era_counts.append(1)
            elif ii == 1: era_counts.append(2)
            elif ii in [2, 3]: era_counts.append(2)
            elif ii in [4, 5]: era_counts.append(3)
            else: era_counts.append(4)
                
            if len(era_description) > longest_name:
                longest_name = len(era_description)

        for ij in xrange(len(era_names)):
            era = era_names[ij]
            count = str(era_counts[ij])
            
            aligned_era = era.ljust(longest_name + 2)
            
            era_text = u"<font=2b>%s%s</font>" % (aligned_era, count)
            screen.setText(szPaneID + "era" + str(ij), parent_id, era_text, CvUtil.FONT_LEFT_JUSTIFY, FX_PLUS, FY, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
            FY += MARGIN - 10
                        
    def drawCivicOptionButtons(self, iCivicOption):
        activePlayer = gc.getPlayer(self.iActivePlayer)
        screen = self.getScreen()

        for j in xrange(gc.getNumCivicInfos()):
            civic_info = gc.getCivicInfo(j)
            if civic_info.getCivicOptionType() == iCivicOption:
                button_name = self.getCivicsButtonName(j)
                
                screen.setState(button_name, self.selected_civics[iCivicOption] == j)
                
                is_display_civic = self.highlighted_civics[iCivicOption] == j
                can_do_civic = activePlayer.canDoCivics(j, True) 

                if is_display_civic or can_do_civic:
                    screen.show(button_name)
                else:
                    screen.hide(button_name)

    def drawAllButtons(self): 
        """Draws all civic option buttons using dynamic panel heights."""
        screen = self.getScreen()
        fY = self.HEADINGS_TOP # Start Y for the top level panel
        header_height = 25 
        
        # 1. PRE-PROCESSING: Get layout data and max heights
        max_heights = self._get_civic_layout_data() 
        parent_civic_list = self.parent_civics_layout
        child_civic_map = self.child_civics_layout
        
        # Determine the maximum height needed for the parent (Government) panel
        parent_panel_height = max_heights.get(0, 5) * self.CIVIC_ROW_HEIGHT + self.TEXT_MARGIN 
        
        def doPivotLayoutCivics(civicIdx, civicName_l, civicoption, panelHeight, fY_panel, panelStyle):
            """Helper function to draw a single column of civics."""
            fX = self.HEADINGS_SPACING + (self.HEADINGS_WIDTH + self.HEADINGS_SPACING) * civicIdx
            
            szAreaID = self.AREA_NAME + "Option%dColumn%d" % (civicoption, civicIdx)
            # Note: addPanel here correctly omits the parent_id argument
            screen.addPanel(szAreaID, "", "", True, True, fX + 5, fY_panel, self.HEADINGS_WIDTH, panelHeight, panelStyle)
            
            current_fY = fY_panel + 5 
            button_spacing = self.CIVIC_ROW_HEIGHT 
            
            if civicoption == 0:
                button_spacing = self.CIVIC_ROW_HEIGHT - 3 
            
            for j in xrange(gc.getNumCivicInfos()):
                civic_info = gc.getCivicInfo(j)
                if civic_info.getDescription() in civicName_l:
                    
                    current_fY += button_spacing
                    
                    screen.addCheckBoxGFC(self.getCivicsButtonName(j), civic_info.getButton(), ArtFileMgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), 
                                          fX + self.BUTTON_SIZE / 2, current_fY, self.BUTTON_SIZE, self.BUTTON_SIZE,
                                          WidgetTypes.WIDGET_GENERAL, -1, -1, ButtonStyles.BUTTON_STYLE_LABEL)
                    
                    screen.setText(self.getCivicsTextName(j), "", u"<font=2>" + civic_info.getDescription() + u"</font>", 
                                   CvUtil.FONT_LEFT_JUSTIFY, fX + self.BUTTON_SIZE + self.TEXT_MARGIN, current_fY, 
                                   0, FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
            
            self.drawCivicOptionButtons(civicoption)

        # --- 2. DRAWING: Parent Civic Option (Government) ---
        parent_civic_option_id = 0 
        
        if len(parent_civic_list) > 0 and len(parent_civic_list[parent_civic_option_id]) > 0:
            header_text = u"<font=3><color=255,255,0,255>%s</color></font>" % gc.getCivicOptionInfo(parent_civic_option_id).getDescription()
            screen.setText("", "Background", header_text, CvUtil.FONT_LEFT_JUSTIFY, 
                           self.GOV_CIVIC_HEADER_X + self.PANEL_BOX_ADJUSTER - 10, 28, 0, FontTypes.SMALL_FONT, 
                           WidgetTypes.WIDGET_GENERAL, -1, -1)
            
            for i in xrange(len(parent_civic_list[parent_civic_option_id])):
                doPivotLayoutCivics(i, parent_civic_list[parent_civic_option_id][i], parent_civic_option_id, parent_panel_height, fY, PanelStyles.PANEL_STYLE_BLUE50)
        
        self.P_CIVIC_END_Y = fY + parent_panel_height + self.TEXT_MARGIN

        # --- 3. DRAWING: Child Civic Options ---
        current_child_y = self.P_CIVIC_END_Y 

        for j in sorted(child_civic_map.keys()):
            if gc.getCivicOptionInfo(j).getParentCivicOption() != 1:
                continue

            child_civic_list = child_civic_map.get(j, [])
            
            if len(child_civic_list) > 0: 
                child_panel_height = max_heights.get(j, 5) * self.CIVIC_ROW_HEIGHT + self.TEXT_MARGIN
                
                szAreaID = self.AREA_NAME + "ChildHeader%d" % j
                # Note: addPanel here correctly omits the parent_id argument
                screen.addPanel(szAreaID, "", "", True, False, 0, current_child_y, self.W_SCREEN, header_height, PanelStyles.PANEL_STYLE_MAIN_TAN)
                
                header_text = u"<font=3><color=255,255,0,255>%s</color></font>" % gc.getCivicOptionInfo(j).getDescription()
                screen.setText("", "Background", header_text, CvUtil.FONT_LEFT_JUSTIFY, 
                               self.GOV_CIVIC_HEADER_X + self.PANEL_BOX_ADJUSTER - 10, current_child_y + 3, 0, 
                               FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
                
                for i in xrange(len(child_civic_list)):
                    doPivotLayoutCivics(i, child_civic_list[i], j, child_panel_height, current_child_y + header_height, PanelStyles.PANEL_STYLE_CITY_COLUMNL)
                
                current_child_y += header_height + child_panel_height + self.TEXT_MARGIN

        self.A_CIVIC_END_Y = current_child_y


    def drawAllHelpText(self, parent_id="Background"):
        """Draws the help text boxes for all child civic options onto a specified parent."""
        
        counter_dependant = 0
        # Calculate Y position relative to the main Background panel
        const_fY = self.HELP_PANEL_START_Y
        
        HELP_PANEL_X = 0 
        
        for i in xrange(gc.getNumCivicOptionInfos()):
            if gc.getCivicOptionInfo(i).getParentCivicOption() > 0:
                szPaneID = "CivicsHelpTextBackground%d%d" % (counter_dependant, i)
                screen = self.getScreen()
                
                # FIX: addPanel should only take 11 arguments
                screen.addPanel(szPaneID, "", "", True, True, 
                                HELP_PANEL_X, const_fY, self.TEXT_BOX_WIDTH, self.TEXT_BOX_SIZE, 
                                PanelStyles.PANEL_STYLE_MAIN)
                
                # Pass the absolute Y position of the panel's top (const_fY)
                self.drawHelpText(i, const_fY, parent_id)
                
                counter_dependant += 1
                const_fY += self.TEXT_BOX_SIZE - self.TEXT_BOX_SEPERATOR 


    def drawHelpText(self, iCivicOption, panel_top_fY, parent_id="Background"):
        """Draws the help text for a single civic option."""
        
        iCivic = self.highlighted_civics[iCivicOption]
        screen = self.getScreen()
        
        activePlayer = gc.getPlayer(self.iActivePlayer)
        
        # Determine Upkeep Text
        if gc.getCivicInfo(iCivic).getUpkeep() != -1 and not activePlayer.isNoCivicUpkeep(iCivicOption):
            szUpkeepText = gc.getUpkeepInfo(gc.getCivicInfo(iCivic).getUpkeep()).getDescription()
        else:
            szUpkeepText = localText.getText("TXT_KEY_CIVICS_SCREEN_NO_UPKEEP", ())

        szHelpText = CyGameTextMgr().parseCivicInfo(iCivic, False, True, True)
        
        fX = 5
        
        # Calculate content Y relative to the panel_top_fY (which is relative to the Background panel)
        content_fY = panel_top_fY + self.TEXT_MARGIN # Start Y inside the panel
        
        civic_option_text = gc.getCivicOptionInfo(iCivicOption).getDescription().upper() + "--> "
        civic_text = gc.getCivicInfo(iCivic).getDescription().upper() + " --" + szUpkeepText
        
        # Draw Header 1 (Civic Option Name)
        header1_text = u"<font=3b><color=205,160,55,255>%s</color></font>" % civic_option_text
        screen.setLabel(self.HELP_HEADER_NAME + str(iCivicOption) + "1", parent_id, header1_text, 
                        CvUtil.FONT_LEFT_JUSTIFY, fX * 3, content_fY, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
                        
        # Draw Header 2 (Civic Name and Upkeep)
        header2_text = u"<font=3>" + civic_text + u"</font>"
        text_placement_X = fX + 250 
        screen.setLabel(self.HELP_HEADER_NAME + str(iCivicOption) + "2", parent_id, header2_text, 
                        CvUtil.FONT_LEFT_JUSTIFY, text_placement_X, content_fY, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
                        
        # Draw Multiline Help Text (Below headers)
        szHelpAreaID = self.HELP_AREA_NAME + str(iCivicOption)
        # FIX: Remove parent_id argument from addMultilineText (only 11 arguments in C++ signature)
        screen.addMultilineText(szHelpAreaID, szHelpText, fX + 10, content_fY + self.TEXT_MARGIN * 2, self.TEXT_BOX_WIDTH - 20, self.TEXT_BOX_SIZE - 70, 
                                WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)


    def updateAnarchy(self):
        """Updates the Anarchy and Upkeep display at the bottom of the screen."""
        screen = self.getScreen()
        activePlayer = gc.getPlayer(self.iActivePlayer)
        
        bChange = False
        for i in xrange(gc.getNumCivicOptionInfos()):
            if self.selected_civics[i] != self.initial_civics[i]:
                bChange = True
            
        screen.setText(self.CANCEL_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_GOVERNMENT_PREV_SELECT", ()) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_CANCEL + 10, self.Y_CANCEL + 5, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)
        screen.setText(self.RESET_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_GOVERNMENT_RESET", ()) + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.X_CANCEL / 2, self.Y_CANCEL, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)
        
        screen.deleteWidget(self.EXIT_NAME)
        
        # FIX: Python 2.4 Conditional Assignment for EXIT button logic
        if activePlayer.canRevolution(0) and bChange:
            screen.setText(self.EXIT_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_GOVERNMENT_SET_CIVICS", ()) + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_REVOLUTION, 1, 0)
            screen.show(self.CANCEL_NAME)
            screen.show(self.RESET_NAME)
        else:
            screen.setText(self.EXIT_NAME, "Background", u"<font=4>" + localText.getText("TXT_KEY_GOVERNMENT_BACK", ()) + u"</font>", CvUtil.FONT_RIGHT_JUSTIFY, self.X_EXIT, self.Y_EXIT, self.Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, -1)
            screen.hide(self.CANCEL_NAME)
            screen.hide(self.RESET_NAME)

        iTurns = activePlayer.getCivicAnarchyLength(self.highlighted_civics)
        
        # FIX: Python 2.4 Conditional Assignment for anarchy text
        if activePlayer.canRevolution(0):
            szText = localText.getText("TXT_KEY_ANARCHY_TURNS", (iTurns, ))
        else:
            szText = CyGameTextMgr().setRevolutionHelp(self.iActivePlayer)
            
        screen.setLabel("CivicsRevText", "Background", u"<font=3>" + szText + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, 700, self.BOTTOM_LINE_TOP + 5 + self.TEXT_MARGIN / 2, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

        upkeep_cost = activePlayer.getCivicUpkeep(self.highlighted_civics, True) * (100 + activePlayer.calculateInflationRate()) / 100
        szText = localText.getText("TXT_KEY_CIVIC_SCREEN_UPKEEP", (upkeep_cost, ))
        screen.setLabel("CivicsUpkeepText", "Background", u"<font=3>" + szText + u"</font>", CvUtil.FONT_LEFT_JUSTIFY, 100, self.BOTTOM_LINE_TOP - 6 + self.BOTTOM_LINE_HEIGHT - 2 * self.TEXT_MARGIN, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
        
    def handleExit(self, inputClass):
        if inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            self.highlight_group = []
            screen = self.getScreen()
            screen.hideScreen()
            for idx in xrange(len(self.initial_civics)): 
                self.civics_to_return[idx] = self.selected_civics[idx]
            CvScreensInterface.showCivicsScreen(self.civics_to_return, 'government')

    def handleCancel(self, inputClass):
        if inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            self.parent_civics_layout = []
            self.child_civics_layout = {}
            self.highlight_group = []
            for i in xrange(gc.getNumCivicOptionInfos()):
                self.selected_civics[i] = self.initial_civics[i]
                self.highlighted_civics[i] = self.initial_civics[i]
            self.drawContents()

    def handleReset(self, inputClass):
        if inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            self.parent_civics_layout = []
            self.child_civics_layout = {}
            self.highlight_group = []
            for i in xrange(gc.getNumCivicOptionInfos()):
                self.selected_civics[i] = self.absolute_original_civics[i]
                self.highlighted_civics[i] = self.absolute_original_civics[i]
            self.drawContents()

    def handleCivicsButton(self, inputClass):
        iCivicID = inputClass.getID()
        iCivicOption = gc.getCivicInfo(iCivicID).getCivicOptionType()
        
        if inputClass.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            if inputClass.getFlags() & MouseFlags.MOUSE_RBUTTONUP:
                CvScreensInterface.pediaJumpToCivic((iCivicID, ))
            elif gc.getPlayer(self.iActivePlayer).canDoCivics(iCivicID, True):
                self.selected_civics[iCivicOption] = iCivicID
                self.highlighted_civics[iCivicOption] = iCivicID
                self.drawContents()

        elif inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_ON:
            self.highlighted_civics[iCivicOption] = iCivicID
            self.drawContents()

        elif inputClass.getNotifyCode() == NotifyCode.NOTIFY_CURSOR_MOVE_OFF:
            self.highlighted_civics[iCivicOption] = self.selected_civics[iCivicOption]
            self.drawContents()

        return 0
        
    def getCivicsButtonName(self, iCivic):
        return self.BUTTON_NAME + str(iCivic)

    def getCivicsTextName(self, iCivic):
        return self.TEXT_NAME + str(iCivic)

    def handleInput(self, inputClass):
        if CyGame().isDebugMode() and inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED and inputClass.getFunctionName() == self.DEBUG_DROPDOWN_ID:
            screen = self.getScreen()
            iIndex = screen.getSelectedPullDownID(self.DEBUG_DROPDOWN_ID)
            self.setActivePlayer(screen.getPullDownData(self.DEBUG_DROPDOWN_ID, iIndex))
            self.drawContents()
            return 1

        if self.CivicsScreenInputMap.has_key(inputClass.getFunctionName()):
            self.CivicsScreenInputMap.get(inputClass.getFunctionName())(inputClass)
            return 1
            
        return 0

    def update(self, fDelta):
        return