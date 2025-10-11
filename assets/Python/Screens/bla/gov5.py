## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvScreensInterface
from LayoutDict import gRect # advc.002b

# --- CONSTANTS (Global Module Scope - Visuals/Metrics retained here) ---
H_SCREEN_DEFAULT = 768
CIVIC_LIST_PANEL_WIDTH = 200

HEADINGS_TOP = 50
HEADINGS_SPACING = -4
HEADINGS_BOTTOM = 330
HELP_TOP = 325
HELP_BOTTOM = 655
TEXT_MARGIN = 14
BUTTON_SIZE = 24
BIG_BUTTON_SIZE = 44
BOTTOM_LINE_TOP = 660
BOTTOM_LINE_HEIGHT = 60

Y_EXIT = 726
Y_TITLE = 8
Z_SCREEN = -6.1
Z_TEXT = Z_SCREEN - 0.2
# -------------------

class CvGovermentScreen:
    "Goverment Civics Screen"

    def __init__(self):
        # Global Handles moved to instance scope
        self.gc = CyGlobalContext()
        self.art_file_mgr = CyArtFileMgr()
        self.local_text = CyTranslator()

        # Widget IDs moved to instance scope as requested (retaining typos)
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
        self.DEBUG_DROPDOWN_ID = "GovermentDropdownWidget"
        self.BACKGROUND_ID = "GovermentBackground"
        self.HELP_HEADER_NAME = "GovermentScreenHeaderName"
        
        # UI Metrics (using module-level constants)
        self.H_SCREEN = H_SCREEN_DEFAULT
        self.CIVIC_LIST_PANEL_WIDTH = CIVIC_LIST_PANEL_WIDTH
        self.Y_EXIT = Y_EXIT

        # Dynamic/Calculated Screen Metrics
        self.x_resolution = 0
        self.y_resolution = 0
        self.x_start = 0 
        self.horizontal_margin = 0
        self.screen_width = 0
        self.headings_width = 0
        self.x_exit = 0
        self.x_cancel = 0
        self.x_center = 0

        # State Variables
        self.active_player_id = -1
        self.current_civics = []
        self.display_civics = []
        self.original_civics = []
        self.absolute_original_civics = []
        self.all_delivered_civics = []

        # Custom Mod Doto
        self.parent_civics = [] 
        self.child_civics = {}
        
        # NEW: Stores the processed data for tree display
        self.m_allCivicTreeData = {}

        # Input Map using the new self.names
        self.input_map = {
            self.BUTTON_NAME: self.handle_civics_button_click,
            self.TEXT_NAME: self.handle_civics_button_click,
            self.EXIT_NAME: self.handle_revolution_click,
            self.CANCEL_NAME: self.cancel_changes,
            self.RESET_NAME: self.reset_to_original
        }

    def set_active_player(self, player_id, delivered_civics = None):
        self.active_player_id = player_id
        active_player = self.gc.getPlayer(player_id)
        
        # Reset state lists
        self.current_civics = []
        self.display_civics = []
        self.original_civics = []
        self.all_delivered_civics = []
        self.absolute_original_civics = []

        num_civic_options = self.gc.getNumCivicOptionInfos()
        
        # Determine the source for the current state (delivered list or active player)
        source_civics = delivered_civics or [active_player.getCivics(i) for i in range(num_civic_options)]

        for civic_id in source_civics:
            self.current_civics.append(civic_id)
            self.display_civics.append(civic_id)
            self.original_civics.append(civic_id)
            self.all_delivered_civics.append(civic_id)
            
        # Absolute originals are always the player's currently active civics
        self.absolute_original_civics = [active_player.getCivics(i) for i in range(num_civic_options)]
        
        self.child_civics = {}
        for i in range(num_civic_options):
            self.child_civics[i] = []
            
        # NEW: Generate the custom tree data structure required for the new layout
        self._generate_custom_civic_tree_data()

          # Reset custom mod variables
        self.parent_civics = [] 
        # Calculate dynamic column width based on number of visible options (i.e., Parent-Civic Options)
        for civic_option_id, list_of_dicts in self.m_allCivicTreeData.items():
            for item in list_of_dicts:
                # Check if the 'parent' key exists and contains an 'id'
                if 'parent' in item and 'id' in item['parent']:
                    self.parent_civics.append(item['parent']['id'])
                    CvUtil.pyPrint("Calculate dynamic column width  init ( %s )" %(self.parent_civics))

    def calculate_screen_metrics(self, screen):
        """
        Calculates dynamic screen dimensions, using full resolution as the basis.
        """
        self.x_resolution = screen.getXResolution()
        self.y_resolution = screen.getYResolution()

        # Set the screen width to the full resolution width
        self.screen_width = self.x_resolution
        
        # When using full width, the screen starts at X=0
        self.x_start = 0 

        self.bottom_line_width = self.screen_width - 10
        self.x_exit = self.screen_width - 30
        self.x_cancel = self.screen_width // 2
        self.x_center = self.screen_width // 2

        num_visible_civic_options = len(self.parent_civics)
        CvUtil.pyPrint("num_visible_civic_options init ( %s )" %(self.parent_civics))

        if num_visible_civic_options > 0:
            self.headings_width = (self.screen_width - HEADINGS_SPACING) / num_visible_civic_options - HEADINGS_SPACING
        else:
            self.headings_width = self.screen_width

# ----------------------------------------------------------------------
# (interfaceScreen function updated)
# ----------------------------------------------------------------------

    def interfaceScreen (self, civics = None):
        # Use self.SCREEN_NAME
        screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)
        
        if screen.isActive():
            return

        screen.setRenderInterfaceOnly(True)
        screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

        # 2. Set Active Player and Civic State (this now calls the data generation method)
        self.set_active_player(self.gc.getGame().getActivePlayer(), civics)
        
        # 1. Calculate Dynamic Metrics
        self.calculate_screen_metrics(screen)

        # 3. Setup Screen Layout
        screen.setDimensions(self.x_start, screen.centerY(0), self.screen_width, self.H_SCREEN)
        
        # Use self.BACKGROUND_ID
        screen.addDDSGFC(self.BACKGROUND_ID, self.art_file_mgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.screen_width, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1)
        screen.addPanel("CivicsTopPanel", u"", u"", True, False, 0, 0, self.screen_width, 55, PanelStyles.PANEL_STYLE_TOPBAR)
        screen.addPanel("CivicsBottomPanel", u"", u"", True, False, 0, 713, self.screen_width, 55, PanelStyles.PANEL_STYLE_BOTTOMBAR)
        screen.showWindowBackground(False)
        
        # Use self.TITLE_NAME
        screen.setText(self.TITLE_NAME, "Background", u"<font=4b>" + "GOVENMENTS" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.x_center, Y_TITLE, Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

        # Bottom Line Buttons (Cancel, Reset) - Use self.CANCEL_NAME and self.RESET_NAME
        screen.setText(self.CANCEL_NAME, "Background", u"<font=4>" + "PREV SELECT" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.x_cancel, self.Y_EXIT, Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)
        screen.setText(self.RESET_NAME, "Background", u"<font=4>" + "RESET" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.x_cancel // 2, self.Y_EXIT, Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

        # Debug Dropdown - Use self.DEBUG_DROPDOWN_ID
        if self.gc.getGame().isDebugMode():
            screen.addDropDownBoxGFC(self.DEBUG_DROPDOWN_ID, 22, 12, 300, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
            for j in range(self.gc.getMAX_PLAYERS()):
                if self.gc.getPlayer(j).isAlive():
                    screen.addPullDownString(self.DEBUG_DROPDOWN_ID, self.gc.getPlayer(j).getName(), j, j, False)

        screen.addPanel("CivicsBottomLine", "", "", True, True, HEADINGS_SPACING, BOTTOM_LINE_TOP, self.bottom_line_width, BOTTOM_LINE_HEIGHT, PanelStyles.PANEL_STYLE_MAIN)

        # 4. Draw Contents - Pass the screen object to drawing functions that need it
        self.draw_contents(screen)

        return 0

    def draw_contents(self, screen): # Now requires 'screen' argument
        self.draw_all_civic_options(screen)
        self.draw_all_help_text(screen)
        self.update_anarchy_and_buttons(screen)
    
    def draw_all_civic_options(self, screen): # Now requires 'screen' argument
        active_player = self.gc.getPlayer(self.active_player_id)
        reindex = 0
        
        # Clear existing buttons
        for i in range(self.gc.getNumCivicInfos()):
            screen.deleteWidget(self.get_civics_button_name(i))
            screen.deleteWidget(self.get_civics_text_name(i))
        
        for civic_option_id, list_of_dicts in self.m_allCivicTreeData.items():
            for item in list_of_dicts:
                # Check if the 'parent' key exists and contains an 'id'
                if 'parent' in item and 'id' in item['parent']:
                    option_id = civic_option_id
                    current_parent_civic_id = item['parent']['id']
                else:
                    continue

                # --- Panel Positioning ---
                civic_panel_width = self.headings_width
                
                x_pos = HEADINGS_SPACING + (self.headings_width + HEADINGS_SPACING) * reindex

                y_pos = HEADINGS_TOP

                # Main Panel for the Civic Option - Use self.AREA_NAME
                area_id = self.AREA_NAME + str(reindex)
                screen.addPanel(area_id, "", "", True, True,
                                 x_pos, y_pos, civic_panel_width, HEADINGS_BOTTOM - HEADINGS_TOP,
                                 PanelStyles.PANEL_STYLE_MAIN)
                
                # Header Label for the Civic Option
                y_pos += TEXT_MARGIN
                screen.setLabel("%s_Header" % area_id, "Background", 
                                 u"<font=3>" + self.gc.getCivicOptionInfo(option_id).getDescription().upper() + u"</font>",
                                 CvUtil.FONT_CENTER_JUSTIFY,
                                 x_pos + civic_panel_width // 2, y_pos, 0,
                                 FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
                
                y_pos += TEXT_MARGIN
                
                # -----------------------------------------------------------
                # CUSTOM DRAWING LOGIC: Iterate over PARENT and CHILD structure
                # -----------------------------------------------------------
                
                for item in list_of_dicts:

                    # --- Draw PARENT Civic (Selectable) ---
                    parent_civic_id = item['parent']['id']

                    if current_parent_civic_id != parent_civic_id:
                        continue

                    parent_civic_info = self.gc.getCivicInfo(parent_civic_id)
                    y_pos += TEXT_MARGIN 
                    
                    button_name = self.get_civics_button_name(parent_civic_id)
                    text_name = self.get_civics_text_name(parent_civic_id)
                    
                    # Checkbox Button (Interactive - Main Civic)
                    
                    can_do_civic_parent = active_player.canDoCivics(parent_civic_id, False)
                    is_displayed_civic_parent = self.display_civics[option_id] == parent_civic_id

                    if can_do_civic_parent or is_displayed_civic_parent:
                        screen.addCheckBoxGFC(button_name, parent_civic_info.getButton(), self.art_file_mgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), 
                                                int(x_pos + BUTTON_SIZE // 2), int(y_pos), int(BUTTON_SIZE), BUTTON_SIZE, 
                                                WidgetTypes.WIDGET_GENERAL, parent_civic_id, -1, ButtonStyles.BUTTON_STYLE_LABEL)
                        
                    # Text Label (Interactive)
                    screen.setText(text_name, "", u"<font=3b>" + parent_civic_info.getDescription() + u"</font>", # Bold the parent civic name
                                     CvUtil.FONT_LEFT_JUSTIFY, 
                                     x_pos + BUTTON_SIZE + TEXT_MARGIN, y_pos, 0, 
                                     FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, parent_civic_id, -1)
                    
                    # Set State
                    screen.setState(button_name, self.current_civics[option_id] == parent_civic_id)  
                    
                    # --- Draw Structured CHILD Civics (Dependencies) ---
                    if item['children']:
                        
                        # Iterate over the groups (Civic Options of the Children)
                        for child_option_group in item['children']:
                            
                            # 1. Draw Child Civic Option Name (e.g., Regime, Type)
                            y_pos += TEXT_MARGIN * 1.5
                            child_option_name = child_option_group['option_name']
                            
                            option_header_name = "ChildOptionHeader%d_%d" % (child_option_group['option_id'], parent_civic_id)
                            
                            # Indent slightly and use a distinct font
                            screen.setLabel(option_header_name, "Background", 
                                             u"<font=2b>" + child_option_name.upper() + u"</font>", 
                                             CvUtil.FONT_LEFT_JUSTIFY, 
                                             x_pos + TEXT_MARGIN * 1.5, y_pos, 0, 
                                             FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

                            # 2. Draw the list of actual Child Civics in that Option
                            for child_civic_data in child_option_group['civics']:

                                # Only draw if the player can adopt the civic or if it is currently selected/displayed
                                child_civic_id = child_civic_data['id']
                        
                                y_pos += TEXT_MARGIN * 1.2 # Smaller step for child list
                                
                                # Child Text (Indented - Not interactive)
                                child_text_name = "ChildText%d_%d" % (child_civic_id, parent_civic_id)
                                
                                # Use smaller font and further indentation
                                # screen.setLabel(child_text_name, "Background", 
                                #                  u"<font=2>" + child_civic_data['name'] + u"</font>",
                                #                  CvUtil.FONT_LEFT_JUSTIFY, 
                                #                  x_pos + TEXT_MARGIN * 2.5, y_pos, 0, 
                                #                  FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, child_civic_id, -1)
                                child_civic_info = self.gc.getCivicInfo(child_civic_id)
                                
                                can_do_civic = active_player.canDoCivics(child_civic_id, False)
                                is_displayed_civic = self.display_civics[option_id] == child_civic_id

                                button_name_child = self.get_civics_button_name(child_civic_id)
                                text_name_child = self.get_civics_text_name(child_civic_id)

                                if can_do_civic or is_displayed_civic:
                                    screen.addCheckBoxGFC(button_name_child, child_civic_info.getButton(), self.art_file_mgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), 
                                                int(x_pos + BUTTON_SIZE // 2), int(y_pos), int(BUTTON_SIZE), BUTTON_SIZE, 
                                                WidgetTypes.WIDGET_GENERAL, child_civic_id, -1, ButtonStyles.BUTTON_STYLE_LABEL)
                
                                # screen.setLabel(child_text_name, "Background", 
                                #         u"<font=3>" + child_civic_info.getDescription() + u"</font>", 
                                #             CvUtil.FONT_LEFT_JUSTIFY, 
                                #             x_pos + TEXT_MARGIN * 1.5, y_pos, 0, 
                                #             FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
                                    
                                    # Text Label (Interactive)
                                screen.setText(text_name_child, "", u"<font=3>" + child_civic_info.getDescription() + u"</font>", # Bold the parent civic name
                                                CvUtil.FONT_LEFT_JUSTIFY, 
                                                int(x_pos + BUTTON_SIZE + TEXT_MARGIN), int(y_pos), 0, 
                                                FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, child_civic_id, -1)
                                
                                screen.setState(button_name_child, self.current_civics[option_id] == child_civic_id)  
                    
					
                    # Add a small vertical space between parent civics
                    y_pos += TEXT_MARGIN * 0.6 

                    # -----------------------------------------------------------
                    # End Reworked Drawing Logic
                    # -----------------------------------------------------------
                reindex += 1
            
    # def highlight_civic(self, civic_id):
    #     """Highlights a civic for help text. Returns True if a change was made."""
    #     option_id = self.gc.getCivicInfo(civic_id).getCivicOptionType()
    #     if self.display_civics[option_id] != civic_id:
    #         self.display_civics[option_id] = civic_id
    #         return True
    #     return False
        
    # def unhighlight_civic(self, civic_id):
    #     """Unhighlights a civic, reverting display to the current selection. Returns True if a change was made."""
    #     option_id = self.gc.getCivicInfo(civic_id).getCivicOptionType()
    #     current_selection_id = self.current_civics[option_id]
    #     if self.display_civics[option_id] != current_selection_id:
    #         self.display_civics[option_id] = current_selection_id
    #         return True
    #     return False
        
    # def select_civic(self, civic_id):
    #     """Selects a new civic, updating internal state and UI."""
    #     active_player = self.gc.getPlayer(self.active_player_id)
        
    #     if not active_player.canDoCivics(civic_id, False):
    #         return 0
            
    #     option_id = self.gc.getCivicInfo(civic_id).getCivicOptionType()
        
    #     previous_civic_id = self.current_civics[option_id]
        
    #     # 1. Update State
    #     self.current_civics[option_id] = civic_id
        
    #     # 2. Update UI
    #     screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN) # Must create here
        
    #     # Unselect previous
    #     screen.setState(self.get_civics_button_name(previous_civic_id), False)
        
    #     # Select new (highlights it and sets display_civics)
    #     self.highlight_civic(civic_id)
    #     screen.setState(self.get_civics_button_name(civic_id), True)
        
    #     return 0

    # def handle_civics_button_click(self, input_class):
    #     civic_id = input_class.getID()
    #     civic_info = self.gc.getCivicInfo(civic_id)
        
    #     # We only handle clicks on Parent Civics (selectable checkboxes in the list)
    #     option_id = civic_info.getCivicOptionType()
        
    #     # Check if this civic belongs to one of the main (non-child) civic options
    #     if self.is_parent_child_civic_option(option_id):
    #         # If it is a child option, this is likely an accidental click on the text label. Ignore.
    #         return 0
            
    #     reindex = option_id - self.get_parent_child_civic_options_count()

    #     notify_code = input_class.getNotifyCode()

    #     if notify_code == NotifyCode.NOTIFY_CLICKED:
    #         if input_class.getFlags() & MouseFlags.MOUSE_RBUTTONUP:
    #             CvScreensInterface.pediaJumpToCivic((civic_id, ))
    #         else:
    #             self.select_civic(civic_id)
    #             screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN) # Must create here
    #             self.draw_help_text(screen, option_id, reindex)
    #             self.update_anarchy_and_buttons(screen)
    #     elif notify_code == NotifyCode.NOTIFY_CURSOR_MOVE_ON:
    #         if self.highlight_civic(civic_id):
    #             screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN) # Must create here
    #             self.draw_help_text(screen, option_id, reindex)
    #             self.update_anarchy_and_buttons(screen)
    #     elif notify_code == NotifyCode.NOTIFY_CURSOR_MOVE_OFF:
    #         if self.unhighlight_civic(civic_id):
    #             screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN) # Must create here
    #             self.draw_help_text(screen, option_id, reindex)
    #             self.update_anarchy_and_buttons(screen)

    #     return 0
        
    def draw_help_text(self, screen, option_id, reindex = -1):
        """Draws the help text for a single civic option column."""
        
        # Recalculate reindex based on actual option_id relative to parent options
        temp_reindex = 0
        for i in range(option_id):
            if not self.is_parent_child_civic_option(i):
                temp_reindex += 1
        reindex = temp_reindex
        
        civic_id = self.display_civics[option_id]
        civic_info = self.gc.getCivicInfo(civic_id)
        active_player = self.gc.getPlayer(self.active_player_id)

        # 1. Build Help Text String
        # Upkeep
        if civic_info.getUpkeep() != -1 and not active_player.isNoCivicUpkeep(option_id):
            upkeep_desc = self.gc.getUpkeepInfo(civic_info.getUpkeep()).getDescription()
        else:
            upkeep_desc = self.local_text.getText("TXT_KEY_CIVICS_SCREEN_NO_UPKEEP", ())

        help_text = upkeep_desc
        # Civic Effects
        help_text += CyGameTextMgr().parseCivicInfo(civic_id, False, True, True)

        # 2. Calculate Position
        x_pos = HEADINGS_SPACING + (self.headings_width + HEADINGS_SPACING) * reindex 
        
        # 3. Draw Header - Use self.HELP_HEADER_NAME
        screen.setLabel(self.HELP_HEADER_NAME + str(reindex), "Background", 
                         u"<font=3>" + civic_info.getDescription().upper() + u"</font>", 
                         CvUtil.FONT_CENTER_JUSTIFY, 
                         x_pos + self.headings_width // 2, HELP_TOP + TEXT_MARGIN, 0, 
                         FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

        # 4. Draw Multiline Text Area - Use self.HELP_AREA_NAME
        y_text_start = HELP_TOP + 3 * TEXT_MARGIN
        screen.addMultilineText(self.HELP_AREA_NAME + str(reindex), help_text, 
                                 x_pos + 5, y_text_start, 
                                 self.headings_width - 7, HELP_BOTTOM - y_text_start - 2, 
                                 WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
        
    def draw_all_help_text(self, screen):
        """Initial draw for all help text columns, using the custom civic tree iteration logic."""
        reindex = 0
        
        # Clear existing help widgets from old columns (assuming a maximum of 10 columns for safety)
        for i in range(10):
             screen.deleteWidget("CivicsHelpTextBackground" + str(i))
             screen.deleteWidget(self.HELP_HEADER_NAME + str(i))
             screen.deleteWidget(self.HELP_AREA_NAME + str(i))
        
        # Iterate over the custom tree data structure to determine which columns to draw
        for civic_option_id, list_of_dicts in self.m_allCivicTreeData.items():
            for item in list_of_dicts:
                # Check if the 'parent' key exists, indicating a column should be drawn here
                if 'parent' in item and 'id' in item['parent']:
                    option_id = civic_option_id

                    x_pos = HEADINGS_SPACING + (self.headings_width + HEADINGS_SPACING) * reindex 
                    
                    # Background panel for help text
                    pane_id = "CivicsHelpTextBackground" + str(reindex)
                    screen.addPanel(pane_id, "", "", True, True, 
                                    x_pos, HELP_TOP, self.headings_width, HELP_BOTTOM - HELP_TOP, 
                                    PanelStyles.PANEL_STYLE_MAIN)
                    
                    self.draw_help_text(screen, option_id, reindex)
                    reindex += 1

    def update_anarchy_and_buttons(self, screen):
        """Updates the revolution button, anarchy text, and upkeep text."""

        active_player = self.gc.getPlayer(self.active_player_id)
        
        # Check for any change from the *absolute original* civics (This loop remains global, as it must check all civics)
        has_change = False
        for i in range(self.gc.getNumCivicOptionInfos()):
            if self.current_civics[i] != self.absolute_original_civics[i]:
                has_change = True
                break
        
        # Make the revolution/back button - Use self.EXIT_NAME
        screen.deleteWidget(self.EXIT_NAME)
        
        if active_player.canRevolution(0) and has_change:
            # Show "SET CIVICS" and support buttons
            screen.setText(self.EXIT_NAME, "Background", u"<font=4>" + "SET_CIVICS" + u"</font>", 
                            CvUtil.FONT_RIGHT_JUSTIFY, self.x_exit, self.Y_EXIT, Z_TEXT, 
                            FontTypes.TITLE_FONT, WidgetTypes.WIDGET_REVOLUTION, 1, 0)
            screen.show(self.CANCEL_NAME)
            screen.show(self.RESET_NAME)
        else:
            # Show "BACK" and hide support buttons
            screen.setText(self.EXIT_NAME, "Background", u"<font=4>"+ "BACK" + u"</font>", 
                            CvUtil.FONT_RIGHT_JUSTIFY, self.x_exit, self.Y_EXIT, Z_TEXT, 
                            FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, -1)
            screen.hide(self.CANCEL_NAME)
            screen.hide(self.RESET_NAME)

        # --- Anarchy Text Calculation ---
        anarchy_turns = active_player.getCivicAnarchyLength(self.display_civics)

        if active_player.canRevolution(0):
            anarchy_text = self.local_text.getText("TXT_KEY_ANARCHY_TURNS", (anarchy_turns, ))
        else:
            anarchy_text = CyGameTextMgr().setRevolutionHelp(self.active_player_id)

        # --- Maintenance/Upkeep Text Calculation ---
        upkeep_gold = active_player.getCivicUpkeep(self.display_civics, True)
        upkeep_inflation_adjusted = upkeep_gold * (100 + active_player.calculateInflationRate()) // 100
        upkeep_text = self.local_text.getText("TXT_KEY_CIVIC_SCREEN_UPKEEP", (upkeep_inflation_adjusted, ))

        # Reworked logic start: Apply the civic tree iteration logic (redundantly, as these labels are static)
        # This loop is included to match the requested structural application of the iteration logic.
        reindex = 0
        for civic_option_id, list_of_dicts in self.m_allCivicTreeData.items():
            for item in list_of_dicts:
                if 'parent' in item and 'id' in item['parent']:
                    # --- Anarchy Text Drawing ---
                    # These fixed labels are overwritten on each iteration, which adheres to the request structure
                    screen.setLabel("CivicsRevText", "Background", u"<font=3>" + anarchy_text + u"</font>", 
                                    CvUtil.FONT_CENTER_JUSTIFY, self.x_center, BOTTOM_LINE_TOP + TEXT_MARGIN // 2, 0, 
                                    FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

                    # --- Maintenance/Upkeep Text Drawing ---
                    screen.setLabel("CivicsUpkeepText", "Background", u"<font=3>" + upkeep_text + u"</font>", 
                                    CvUtil.FONT_CENTER_JUSTIFY, self.x_center, BOTTOM_LINE_TOP + BOTTOM_LINE_HEIGHT - 2 * TEXT_MARGIN, 0, 
                                    FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
                    
                    reindex += 1
        
    def handle_revolution_click(self, input_class):
        """Handles the 'SET CIVICS' or 'BACK' button click."""
        if input_class.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            # Update the delivered civics list with the new selections
            for i, civic_id in enumerate(self.current_civics):
                if civic_id != self.all_delivered_civics[i]:
                    self.all_delivered_civics[i] = civic_id

            screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)
            screen.hideScreen()
            CvScreensInterface.showCivicsScreen(self.all_delivered_civics, 'governmentcivics')

    # def cancel_changes(self, input_class):
    #     """Reverts current and display civics to the state when the screen was opened (original_civics)."""
    #     if input_class.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
    #         for i in range (self.gc.getNumCivicOptionInfos()):
    #             self.current_civics[i] = self.original_civics[i]
    #             self.display_civics[i] = self.original_civics[i]
            
    #         screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)
    #         self.draw_contents(screen)
    
    # def reset_to_original(self, input_class):
    #     """Reverts current and display civics to the player's actually active civics (absolute_original_civics)."""
    #     if input_class.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
    #         for i in range (self.gc.getNumCivicOptionInfos()):
    #             self.current_civics[i] = self.absolute_original_civics[i]
    #             self.display_civics[i] = self.absolute_original_civics[i]
            
    #         # doto civics parent reset (your mod logic)
    #         for i in range (self.gc.getNumCivicOptionInfos()):
    #             self.child_civics[i] = []

    #         screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)
    #         self.draw_contents(screen)
    #         self.update_anarchy_and_buttons(screen) 

    # --- Utility Functions ---
    def get_civics_button_name(self, civic_id):
        """Standardized naming for civic button widgets. Uses self.BUTTON_NAME."""
        return self.BUTTON_NAME + str(civic_id)

    def get_civics_text_name(self, civic_id):
        """Standardized naming for civic text widgets. Uses self.TEXT_NAME."""
        return self.TEXT_NAME + str(civic_id)

    # def handleInput(self, inputClass):
    #     """Handles input events (pulldowns, buttons)."""
        
    #     screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)

    #     if inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED:
    #         # Use self.DEBUG_DROPDOWN_ID
    #         index = screen.getSelectedPullDownID(self.DEBUG_DROPDOWN_ID)
    #         self.set_active_player(screen.getPullDownData(self.DEBUG_DROPDOWN_ID, index))
    #         self.draw_contents(screen)
    #         return 1
        
    #     function_name = inputClass.getFunctionName()
    #     # Find the function based on the prefix/name.
    #     for key, func in self.input_map.items():
    #         if function_name.startswith(key):
    #             func(inputClass)
    #             return 1
        
    #     return 0
        
    def update(self, fDelta):
        pass

    # --- Custom Mod Logic ---

    def _generate_custom_civic_tree_data(self):
        """
        Generates the nested structure of Parent Civics and their immediate 
        Child dependencies, keyed by Civic Option Index. 
        CRITICAL CHANGE: Groups children by the Civic Option they belong to.
        Result stored in self.m_allCivicTreeData.
        """
        self.m_allCivicTreeData = {}

        for iCivicOption in xrange(self.gc.getNumCivicOptionInfos()):
            
            # the tree is from only the parent down
            if self.gc.getCivicOptionInfo(iCivicOption).getParentCivicOption() != 2:
                continue
            # # Skip child options, we only build the tree for Parent options
            # if not self.is_parent_child_civic_option(iCivicOption):
            #     continue
                
            civic_list_for_option = []
            
            # Iterate over all defined Civics
            for jCivicId in xrange(self.gc.getNumCivicInfos()):
                parent_civic_info = self.gc.getCivicInfo(jCivicId)
                
                # Check if this Civic belongs to the current Civic Option (making it a Parent for this option)
                if parent_civic_info.getCivicOptionType() == iCivicOption:
                    
                    parent_data = {
                        'name': parent_civic_info.getDescription(),
                        'id': jCivicId,
                    }
                    
                    # NEW: Group children by their Civic Option ID
                    children_by_option = {} 
                    
                    childNum = parent_civic_info.getNumParentCivicsChildren()
                    
                    # Look up the civic IDs this parent unlocks
                    for cChildIndex in xrange(childNum):
                        child_civic_id = parent_civic_info.getParentCivicsChildren(cChildIndex)
                        child_civic_info = self.gc.getCivicInfo(child_civic_id)
                        child_option_id = child_civic_info.getCivicOptionType()
                        
                        # Initialize the structure for this child civic option if needed
                        if child_option_id not in children_by_option:
                            children_by_option[child_option_id] = {
                                'option_id': child_option_id,
                                'option_name': self.gc.getCivicOptionInfo(child_option_id).getDescription(),
                                'civics': []
                            }
                            
                        children_by_option[child_option_id]['civics'].append({
                            'name': child_civic_info.getDescription(),
                            'id': child_civic_id,
                        })
                        
                    # Convert the dictionary of groups into a list for consistent structure
                    child_civics_data_list = children_by_option.values()
                        
                    civic_list_for_option.append({
                        'parent': parent_data,
                        'children': child_civics_data_list # Store the structured list
                    })
            
            self.m_allCivicTreeData[iCivicOption] = civic_list_for_option
        CvUtil.pyPrint("sagi init ( %s )" %(self.m_allCivicTreeData))
        return self.m_allCivicTreeData
        
    def is_parent_child_civic_option(self, option_id):
        """Checks if a Civic Option is a Parent or Child for your mod logic."""
        parent_civic_option = self.gc.getCivicOptionInfo(option_id).getParentCivicOption()
        return bool(parent_civic_option and parent_civic_option > 0)
        
    def get_parent_child_civic_options_count(self):
        """Calculates the number of Civic Options that are Parent/Child and should be skipped."""
        
        return len([i for i in range(self.gc.getNumCivicOptionInfos()) if self.is_parent_child_civic_option(i)])


    # new
    def get_group_civics_to_update(self, civic_id, should_highlight):
        """
        Determines the group of civics (parent and unselected children) 
        that should be highlighted or selected together based on the active civic.
        This is the refactored logic from old highlight_parent_child_group.
        """
        def get_the_civics_to_highlight(source_civic_id, initial_civics):
            civics_to_update = list(initial_civics)
            active_player = self.gc.getPlayer(self.active_player_id)
            
            # 1. Collect Civic Options already covered by the initial set
            covered_options = set()
            for c in initial_civics:
                covered_options.add(self.gc.getCivicInfo(c).getCivicOptionType())

            num_childs = self.gc.getCivicInfo(source_civic_id).getNumParentCivicsChildren()

            # Old logic used 'True' for ignore_hide when in select mode (should_highlight=False)
            # and 'False' for ignore_hide when in hover mode (should_highlight=True).
            # The inner variable 'highlightVhild' was used as the ignore_hide value in activePlayer.canDoCivics.
            # Old 'select' (should_highlight=False) -> highlightVhild=True.
            # Old 'hover' (should_highlight=True) -> highlightVhild=False.
            # Let's align: True when should_highlight is False, and False when should_highlight is True.
            ignore_hide_check = not should_highlight 
            
            for j in xrange(num_childs):
                child_id = self.gc.getCivicInfo(source_civic_id).getParentCivicsChildren(j)
                child_option_id = self.gc.getCivicInfo(child_id).getCivicOptionType()
                
                # Check conditions:
                # 1. Is the child the input civic? (No)
                # 2. Can the player do this child civic? 
                # 3. Is the child's civic option already covered? 
                # 4. Is the source civic the same as the child civic? 
                if (child_id != civic_id and 
                    active_player.canDoCivics(child_id, ignore_hide_check) and 
                    child_option_id not in covered_options and 
                    source_civic_id != child_id):
                        
                    civics_to_update.append(child_id)
                    covered_options.add(child_option_id)
                    
            return civics_to_update

        active_player = self.gc.getPlayer(self.active_player_id)
        civic_info = self.gc.getCivicInfo(civic_id)
        option_info = self.gc.getCivicOptionInfo(civic_info.getCivicOptionType())
        parent_civic_option_type = option_info.getParentCivicOption()
        
        which_civics_to_highlight = []

        # Type 1: The civic_id is a child
        if parent_civic_option_type == 1:
            parent_id = active_player.getCivicParent(civic_id)
            # Check if the parent is attainable
            if active_player.canDoCivics(parent_id, should_highlight):
                which_civics_to_highlight = get_the_civics_to_highlight(parent_id, [civic_id, parent_id])
            else:
                return [] 

        # Type 2: The civic_id is a parent
        elif parent_civic_option_type == 2:
            # Check if the parent is attainable
            if active_player.canDoCivics(civic_id, should_highlight):
                which_civics_to_highlight = get_the_civics_to_highlight(civic_id, [civic_id])
            else:
                return [] 

        # Type 0: Standard civic
        else:
            which_civics_to_highlight = [civic_id]

        # Remove duplicates (using a set conversion is cleaner than the old loop)
        return list(set(which_civics_to_highlight))


    def select_civic(self, civic_id):
        """
        Selects a new civic, updating internal state and UI. 
        Includes parent/child group logic from the old 'select' function.
        """
        active_player = self.gc.getPlayer(self.active_player_id)

        # Old code used 'True' (ignore_hide) for the main canDoCivics check
        if not active_player.canDoCivics(civic_id, True):
            # If you can't even do this, get out....
            return 0

        # Determine the group of civics to select/highlight together
        # Old code used 'False' for the shouldHighLight parameter in the select context.
        civics_to_update = self.get_group_civics_to_update(civic_id, False)

        if not civics_to_update:
            return 0

        screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)

        for civic in civics_to_update:
            option_id = self.gc.getCivicInfo(civic).getCivicOptionType()
            previous_civic_id = self.current_civics[option_id]

            # 1. Update State (Current Selection and Display/Highlight)
            self.current_civics[option_id] = civic
            self.display_civics[option_id] = civic # Also set display civic to keep highlight consistent

            # 2. Update UI: Unselect previous civic
            screen.setState(self.get_civics_button_name(previous_civic_id), False)

            # 3. Update UI: Select the new civic
            screen.setState(self.get_civics_button_name(civic), True)
            
        return 0

    def handle_civics_button_click(self, input_class):
        """
        Handles civic button events (clicks, hovers) and applies the parent/child 
        group selection/highlighting logic from the old 'CivicsButton' function.
        """
        civic_id = input_class.getID()
        notify_code = input_class.getNotifyCode()
        
        # We need the screen object for drawing/updates regardless of the event type
        screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN) 

        if notify_code == NotifyCode.NOTIFY_CLICKED:
            if input_class.getFlags() & MouseFlags.MOUSE_RBUTTONUP:
                CvScreensInterface.pediaJumpToCivic((civic_id, ))
            else:
                # 1. Select the civic (handles state and button UI for the entire group)
                self.select_civic(civic_id)
                
                # 2. Get the group for drawing help text and updating anarchy
                # Old code used 'False' for shouldHighLight in this selection context
                civics_to_update = self.get_group_civics_to_update(civic_id, False)
                
                if civics_to_update:
                    for civic in civics_to_update:
                        option_id = self.gc.getCivicInfo(civic).getCivicOptionType()
                        reindex = option_id - self.get_parent_child_civic_options_count()
                        self.draw_help_text(screen, option_id, reindex)
                    
                    self.update_anarchy_and_buttons(screen)

        elif notify_code == NotifyCode.NOTIFY_CURSOR_MOVE_ON:
            # 1. Get the group for highlighting
            # Old code used 'True' for shouldHighLight in this hover context
            civics_to_update = self.get_group_civics_to_update(civic_id, True)

            if civics_to_update:
                for civic in civics_to_update:
                    # highlight_civic returns True if a change was made
                    if self.highlight_civic(civic): 
                        option_id = self.gc.getCivicInfo(civic).getCivicOptionType()
                        reindex = option_id - self.get_parent_child_civic_options_count()
                        self.draw_help_text(screen, option_id, reindex)
                
                self.update_anarchy_and_buttons(screen)

        elif notify_code == NotifyCode.NOTIFY_CURSOR_MOVE_OFF:
            # 1. Get the group for unhighlighting
            # Old code used 'True' for shouldHighLight in this hover context
            civics_to_update = self.get_group_civics_to_update(civic_id, True)

            if civics_to_update:
                for civic in civics_to_update:
                    # unhighlight_civic returns True if a change was made
                    if self.unhighlight_civic(civic):
                        option_id = self.gc.getCivicInfo(civic).getCivicOptionType()
                        reindex = option_id - self.get_parent_child_civic_options_count()
                        self.draw_help_text(screen, option_id, reindex)
                        
                self.update_anarchy_and_buttons(screen)

        return 0
    
    def cancel_changes(self, input_class):
        """
        Reverts current and display civics to the state when the screen was opened (original_civics).
        Includes logic to reset child civic tracking state from the old 'Cancel' function.
        """
        if input_class.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            for i in range (self.gc.getNumCivicOptionInfos()):
                self.current_civics[i] = self.original_civics[i]
                self.display_civics[i] = self.original_civics[i]
                
                # Applying old 'Cancel' logic: reset the child civic tracking for each option
                # Assumes 'self.child_civics' is the correct state to reset.
                self.child_civics[i] = [] 

            screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)
            self.draw_contents(screen)

    # Note: The 'handleInput' function from the new code is retained as it represents the
    # modern input processing structure (using input_map), which is a key change from the old code.
    def handleInput(self, inputClass):
        """Handles input events (pulldowns, buttons). (Retained new code structure)"""
        
        screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)

        if inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED:
            # Use self.DEBUG_DROPDOWN_ID
            index = screen.getSelectedPullDownID(self.DEBUG_DROPDOWN_ID)
            self.set_active_player(screen.getPullDownData(self.DEBUG_DROPDOWN_ID, index))
            self.draw_contents(screen)
            return 1
        
       # Find the function based on the prefix/name.
        # Assuming the new structure uses 'self.input_map' instead of the old 'self.CivicsScreenInputMap'
        # or that 'self.CivicsScreenInputMap' is still used for backward compatibility.

        function_name = inputClass.getFunctionName()
        # Find the function based on the prefix/name.
        for key, func in self.input_map.items():
            if function_name.startswith(key):
                func(inputClass)
                return 1
        
        # Using the old map check for compatibility with the provided old 'handleInput' structure
        # if (self.CivicsScreenInputMap.has_key(inputClass.getFunctionName())):
        #      'Calls function mapped in CvGevernmentScreen'
        #      self.CivicsScreenInputMap.get(inputClass.getFunctionName())(inputClass)
        #      return 1

        # Fallback/Newer logic check (if input_map is preferred over CivicsScreenInputMap)
        # for key, func in self.input_map.items(): 
        #    if function_name.startswith(key):
        #        func(inputClass)
        #        return 1
        
        return 0

    # Retaining 'highlight_civic' and 'unhighlight_civic' from the new code structure
    def highlight_civic(self, civic_id):
        """Highlights a civic for help text. Returns True if a change was made."""
        option_id = self.gc.getCivicInfo(civic_id).getCivicOptionType()
        if self.display_civics[option_id] != civic_id:
            self.display_civics[option_id] = civic_id
            return True
        return False
        
    def unhighlight_civic(self, civic_id):
        """Unhighlights a civic, reverting display to the current selection. Returns True if a change was made."""
        option_id = self.gc.getCivicInfo(civic_id).getCivicOptionType()
        current_selection_id = self.current_civics[option_id]
        if self.display_civics[option_id] != current_selection_id:
            self.display_civics[option_id] = current_selection_id
            return True
        return False
 
    # Placeholder for reset_to_original which was in the new code snippet
    def reset_to_original(self, input_class):
        """Reverts current and display civics to the player's actually active civics (absolute_original_civics)."""
        if input_class.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            # Assuming 'absolute_original_civics' is defined elsewhere, similar to 'original_civics'
            # for i in range (self.gc.getNumCivicOptionInfos()):
            #     self.current_civics[i] = self.absolute_original_civics[i]
            #     self.display_civics[i] = self.absolute_original_civics[i]
                
            # doto civics parent reset (your mod logic)
            for i in range (self.gc.getNumCivicOptionInfos()):
                self.child_civics[i] = []

            screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.GOVERMENT_SCREEN)
            self.draw_contents(screen)
            self.update_anarchy_and_buttons(screen)

    # --- Refactored New Core Functions ---