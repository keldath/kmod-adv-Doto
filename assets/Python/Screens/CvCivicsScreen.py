## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
import CvScreensInterface
from LayoutDict import gRect # advc.002b

# globals
gc = CyGlobalContext()
art_file_mgr = CyArtFileMgr()
local_text = CyTranslator()

# --- CONSTANTS ---
# Use ALL_CAPS for module-level constants
SCREEN_NAME = "RegularCivicScreen"
CANCEL_WIDGET_ID = "RegularCivicsCancel"
RESET_WIDGET_ID = "RegularReset"
EXIT_WIDGET_ID = "RegularCivicsExit"
TITLE_WIDGET_ID = "RegularCivicsTitleHeader"
BUTTON_WIDGET_PREFIX = "RegularCivicsScreenButton"
TEXT_WIDGET_PREFIX = "RegularCivicsScreenText"
AREA_WIDGET_PREFIX = "RegularCivicsScreenArea"
HELP_AREA_PREFIX = "RegularCivicsScreenHelpArea"
HELP_IMAGE_PREFIX = "RegularCivicsScreenCivicOptionImage"
DEBUG_DROPDOWN_ID = "RegularCivicsDropdownWidget"
BACKGROUND_ID = "RegularCivicsBackground"
HELP_HEADER_PREFIX = "RegularCivicsScreenHeaderName"

# Visual Constants (Vertical/Spacing)
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

class CvRegularCivicScreen:
    "Regular Civics Screen"

    def __init__(self):
        # Widget IDs (kept as instance vars for legacy access)
        self.SCREEN_NAME = SCREEN_NAME
        self.CANCEL_NAME = CANCEL_WIDGET_ID
        self.RESET_NAME = RESET_WIDGET_ID
        self.EXIT_NAME = EXIT_WIDGET_ID
        self.TITLE_NAME = TITLE_WIDGET_ID
        
        # UI Metrics
        self.H_SCREEN = H_SCREEN_DEFAULT
        self.CIVIC_LIST_PANEL_WIDTH = CIVIC_LIST_PANEL_WIDTH

        # Dynamic/Calculated Screen Metrics
        self.x_resolution = 0
        self.y_resolution = 0
        # New variable for custom screen start X (screen.centerX(-170))
        self.x_start = 0 
        self.horizontal_margin = 0
        self.screen_width = 0
        self.headings_width = 0
        self.x_exit = 0
        self.x_cancel = 0
        self.x_center = 0
        self.Y_EXIT = Y_EXIT

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

        self.input_map = {
            BUTTON_WIDGET_PREFIX: self.handle_civics_button_click,
            TEXT_WIDGET_PREFIX: self.handle_civics_button_click,
            EXIT_WIDGET_ID: self.handle_revolution_click,
            CANCEL_WIDGET_ID: self.cancel_changes,
            RESET_WIDGET_ID: self.reset_to_original
        }

    def set_active_player(self, player_id, delivered_civics = None):
        self.active_player_id = player_id
        active_player = gc.getPlayer(player_id)
        
        # Reset state lists
        self.current_civics = []
        self.display_civics = []
        self.original_civics = []
        self.all_delivered_civics = []
        self.absolute_original_civics = []

        num_civic_options = gc.getNumCivicOptionInfos()
        
        # Determine the source for the current state (delivered list or active player)
        source_civics = delivered_civics or [active_player.getCivics(i) for i in range(num_civic_options)]

        for civic_id in source_civics:
            self.current_civics.append(civic_id)
            self.display_civics.append(civic_id)
            self.original_civics.append(civic_id)
            self.all_delivered_civics.append(civic_id)
            
        # Absolute originals are always the player's currently active civics
        self.absolute_original_civics = [active_player.getCivics(i) for i in range(num_civic_options)]
        
        # Reset custom mod variables
        self.parent_civics = []
        self.child_civics = {}
        for i in range(num_civic_options):
            self.child_civics[i] = []

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

        # We must now redefine the content area width if we want it less than full screen.
        # However, to avoid complexity, we'll let the main panel take up the full screen, 
        # and keep the text/button centering relative to that full width.

        self.bottom_line_width = self.screen_width - 10
        self.x_exit = self.screen_width - 30
        self.x_cancel = self.screen_width // 2
        self.x_center = self.screen_width // 2

        # Calculate dynamic column width based on number of visible options
        num_visible_civic_options = gc.getNumCivicOptionInfos() - self.get_parent_child_civic_options_count()
        if num_visible_civic_options > 0:
            # Note: This will make headings very wide on large resolutions.
            self.headings_width = (self.screen_width - HEADINGS_SPACING) / num_visible_civic_options - HEADINGS_SPACING
        else:
            self.headings_width = self.screen_width

# ----------------------------------------------------------------------
# (interfaceScreen function updated)
# ----------------------------------------------------------------------

    def interfaceScreen (self, civics = None):
        # The screen object is created here directly.
        screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REGULAR_CIVICS_SCREEN)
        
        if screen.isActive():
            return

        screen.setRenderInterfaceOnly(True)
        screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

        # 1. Calculate Dynamic Metrics
        self.calculate_screen_metrics(screen)
        
        # 2. Set Active Player and Civic State
        self.set_active_player(gc.getGame().getActivePlayer(), civics)

        # 3. Setup Screen Layout
        # Use X=0 and full resolution width. The Y position is still centered.
        screen.setDimensions(self.x_start, screen.centerY(0), self.screen_width, self.H_SCREEN)
        
        # All internal panels now use the full screen width.
        screen.addDDSGFC(BACKGROUND_ID, art_file_mgr.getInterfaceArtInfo("MAINMENU_SLIDESHOW_LOAD").getPath(), 0, 0, self.screen_width, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1)
        screen.addPanel("CivicsTopPanel", u"", u"", True, False, 0, 0, self.screen_width, 55, PanelStyles.PANEL_STYLE_TOPBAR)
        screen.addPanel("CivicsBottomPanel", u"", u"", True, False, 0, 713, self.screen_width, 55, PanelStyles.PANEL_STYLE_BOTTOMBAR)
        screen.showWindowBackground(False)
        
        # Title
        screen.setText(TITLE_WIDGET_ID, "Background", u"<font=4b>" + "CIVICS" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.x_center, Y_TITLE, Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

        # Bottom Line Buttons (Cancel, Reset)
        screen.setText(CANCEL_WIDGET_ID, "Background", u"<font=4>" + "PREV SELECT" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.x_cancel, self.Y_EXIT, Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)
        screen.setText(RESET_WIDGET_ID, "Background", u"<font=4>" + "RESET" + u"</font>", CvUtil.FONT_CENTER_JUSTIFY, self.x_cancel // 2, self.Y_EXIT, Z_TEXT, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, 0)

        # Debug Dropdown
        if CyGame().isDebugMode():
            screen.addDropDownBoxGFC(DEBUG_DROPDOWN_ID, 22, 12, 300, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
            for j in range(gc.getMAX_PLAYERS()):
                if gc.getPlayer(j).isAlive():
                    screen.addPullDownString(DEBUG_DROPDOWN_ID, gc.getPlayer(j).getName(), j, j, False)

        screen.addPanel("CivicsBottomLine", "", "", True, True, HEADINGS_SPACING, BOTTOM_LINE_TOP, self.bottom_line_width, BOTTOM_LINE_HEIGHT, PanelStyles.PANEL_STYLE_MAIN)

        # 4. Draw Contents - Pass the screen object to drawing functions that need it
        self.draw_contents(screen)

        return 0

    def draw_contents(self, screen): # Now requires 'screen' argument
        self.draw_all_civic_options(screen)
        self.draw_all_help_text(screen)
        self.update_anarchy_and_buttons(screen)
    
    def draw_all_civic_options(self, screen): # Now requires 'screen' argument
        active_player = gc.getPlayer(self.active_player_id)
        reindex = 0
        
        # Clear existing buttons
        for i in range(gc.getNumCivicInfos()):
            screen.deleteWidget(self.get_civics_button_name(i))
            screen.deleteWidget(self.get_civics_text_name(i))

        for option_id in range(gc.getNumCivicOptionInfos()):
            if self.is_parent_child_civic_option(option_id):
                continue

            # --- ADJUSTED LOGIC START ---
            
            # The width of the civic panel will now be the dynamically calculated self.headings_width
            civic_panel_width = self.headings_width
            
            # Calculate horizontal position: 
            # X_position = HEADINGS_SPACING + (self.headings_width + HEADINGS_SPACING) * reindex
            x_pos = HEADINGS_SPACING + (self.headings_width + HEADINGS_SPACING) * reindex

            # --- ADJUSTED LOGIC END ---

            y_pos = HEADINGS_TOP

            # Main Panel for the Civic Option
            area_id = AREA_WIDGET_PREFIX + str(reindex)
            screen.addPanel(area_id, "", "", True, True,
                            x_pos, y_pos, civic_panel_width, HEADINGS_BOTTOM - HEADINGS_TOP,
                            PanelStyles.PANEL_STYLE_MAIN)
            
            # Header Label for the Civic Option
            y_pos += TEXT_MARGIN
            # FIX: Replaced f-string with Python 2.4 '%' formatting
            screen.setLabel("%s_Header" % area_id, "Background", 
                            u"<font=3>" + gc.getCivicOptionInfo(option_id).getDescription().upper() + u"</font>",
                            CvUtil.FONT_CENTER_JUSTIFY,
                            # Center the header text using the new civic_panel_width
                            x_pos + civic_panel_width // 2, y_pos, 0,
                            FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
            
            y_pos += TEXT_MARGIN
            
            # Draw individual Civics
            for civic_id in range(gc.getNumCivicInfos()):
                civic_info = gc.getCivicInfo(civic_id)
                if civic_info.getCivicOptionType() == option_id:
                    
                    can_do_civic = active_player.canDoCivics(civic_id, False)
                    is_displayed_civic = self.display_civics[option_id] == civic_id
                    
                    if can_do_civic or is_displayed_civic:
                        y_pos += 2 * TEXT_MARGIN
                        
                        button_name = self.get_civics_button_name(civic_id)
                        text_name = self.get_civics_text_name(civic_id)
                        
                        # Checkbox Button
                        screen.addCheckBoxGFC(button_name, civic_info.getButton(), art_file_mgr.getInterfaceArtInfo("BUTTON_HILITE_SQUARE").getPath(), 
                                              x_pos + BUTTON_SIZE // 2, y_pos, BUTTON_SIZE, BUTTON_SIZE, 
                                              WidgetTypes.WIDGET_GENERAL, civic_id, -1, ButtonStyles.BUTTON_STYLE_LABEL)
                        
                        # Text Label
                        screen.setText(text_name, "", civic_info.getDescription(), 
                                       CvUtil.FONT_LEFT_JUSTIFY, 
                                       x_pos + BUTTON_SIZE + TEXT_MARGIN, y_pos, 0, 
                                       FontTypes.SMALL_FONT, WidgetTypes.WIDGET_GENERAL, civic_id, -1)
                        
                        # Set State
                        screen.setState(button_name, self.current_civics[option_id] == civic_id)
                    
            reindex += 1
            
    def highlight_civic(self, civic_id):
        """Highlights a civic for help text. Returns True if a change was made."""
        option_id = gc.getCivicInfo(civic_id).getCivicOptionType()
        if self.display_civics[option_id] != civic_id:
            self.display_civics[option_id] = civic_id
            return True
        return False
        
    def unhighlight_civic(self, civic_id):
        """Unhighlights a civic, reverting display to the current selection. Returns True if a change was made."""
        option_id = gc.getCivicInfo(civic_id).getCivicOptionType()
        current_selection_id = self.current_civics[option_id]
        if self.display_civics[option_id] != current_selection_id:
            self.display_civics[option_id] = current_selection_id
            return True
        return False
        
    def select_civic(self, civic_id):
        """Selects a new civic, updating internal state and UI."""
        active_player = gc.getPlayer(self.active_player_id)
        
        if not active_player.canDoCivics(civic_id, False):
            return 0
            
        option_id = gc.getCivicInfo(civic_id).getCivicOptionType()
        
        previous_civic_id = self.current_civics[option_id]
        
        # 1. Update State
        self.current_civics[option_id] = civic_id
        
        # 2. Update UI
        screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REGULAR_CIVICS_SCREEN) # Must create here
        
        # Unselect previous
        screen.setState(self.get_civics_button_name(previous_civic_id), False)
        
        # Select new (highlights it and sets display_civics)
        self.highlight_civic(civic_id)
        screen.setState(self.get_civics_button_name(civic_id), True)
        
        return 0

    def handle_civics_button_click(self, input_class):
        civic_id = input_class.getID()
        option_id = gc.getCivicInfo(civic_id).getCivicOptionType()
        
        reindex = option_id - self.get_parent_child_civic_options_count()

        notify_code = input_class.getNotifyCode()

        if notify_code == NotifyCode.NOTIFY_CLICKED:
            if input_class.getFlags() & MouseFlags.MOUSE_RBUTTONUP:
                CvScreensInterface.pediaJumpToCivic((civic_id, ))
            else:
                self.select_civic(civic_id)
                screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REGULAR_CIVICS_SCREEN) # Must create here
                self.draw_help_text(screen, option_id, reindex)
                self.update_anarchy_and_buttons(screen)
        elif notify_code == NotifyCode.NOTIFY_CURSOR_MOVE_ON:
            if self.highlight_civic(civic_id):
                screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REGULAR_CIVICS_SCREEN) # Must create here
                self.draw_help_text(screen, option_id, reindex)
                self.update_anarchy_and_buttons(screen)
        elif notify_code == NotifyCode.NOTIFY_CURSOR_MOVE_OFF:
            if self.unhighlight_civic(civic_id):
                screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REGULAR_CIVICS_SCREEN) # Must create here
                self.draw_help_text(screen, option_id, reindex)
                self.update_anarchy_and_buttons(screen)

        return 0
        
    def draw_help_text(self, screen, option_id, reindex = -1):
        """Draws the help text for a single civic option column."""
        
        if reindex == -1:
            reindex = option_id - self.get_parent_child_civic_options_count()

        civic_id = self.display_civics[option_id]
        civic_info = gc.getCivicInfo(civic_id)
        active_player = gc.getPlayer(self.active_player_id)

        # 1. Build Help Text String
        # Upkeep
        if civic_info.getUpkeep() != -1 and not active_player.isNoCivicUpkeep(option_id):
            upkeep_desc = gc.getUpkeepInfo(civic_info.getUpkeep()).getDescription()
        else:
            upkeep_desc = local_text.getText("TXT_KEY_CIVICS_SCREEN_NO_UPKEEP", ())

        help_text = upkeep_desc
        # Civic Effects
        help_text += CyGameTextMgr().parseCivicInfo(civic_id, False, True, True)

        # 2. Calculate Position
        x_pos = HEADINGS_SPACING + (self.headings_width + HEADINGS_SPACING) * reindex 
        
        # 3. Draw Header
        screen.setLabel(HELP_HEADER_PREFIX + str(reindex), "Background", 
                        u"<font=3>" + civic_info.getDescription().upper() + u"</font>", 
                        CvUtil.FONT_CENTER_JUSTIFY, 
                        x_pos + self.headings_width // 2, HELP_TOP + TEXT_MARGIN, 0, 
                        FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

        # 4. Draw Multiline Text Area
        y_text_start = HELP_TOP + 3 * TEXT_MARGIN
        screen.addMultilineText(HELP_AREA_PREFIX + str(reindex), help_text, 
                                x_pos + 5, y_text_start, 
                                self.headings_width - 7, HELP_BOTTOM - y_text_start - 2, 
                                WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
        
    def draw_all_help_text(self, screen):
        """Initial draw for all help text columns."""
        reindex = 0
        for option_id in range (gc.getNumCivicOptionInfos()):
            if self.is_parent_child_civic_option(option_id):
                continue

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

        active_player = gc.getPlayer(self.active_player_id)
        
        # Check for any change from the *absolute original* civics
        # Python 2.4-compliant loop replacing any()
        has_change = False
        for i in range(gc.getNumCivicOptionInfos()):
            if self.current_civics[i] != self.absolute_original_civics[i]:
                has_change = True
                break
        
        # Make the revolution/back button
        screen.deleteWidget(EXIT_WIDGET_ID)
        
        if active_player.canRevolution(0) and has_change:
            # Show "SET CIVICS" and support buttons
            screen.setText(EXIT_WIDGET_ID, "Background", u"<font=4>" + "SET_CIVICS" + u"</font>", 
                           CvUtil.FONT_RIGHT_JUSTIFY, self.x_exit, self.Y_EXIT, Z_TEXT, 
                           FontTypes.TITLE_FONT, WidgetTypes.WIDGET_REVOLUTION, 1, 0)
            screen.show(CANCEL_WIDGET_ID)
            screen.show(RESET_WIDGET_ID)
        else:
            # Show "BACK" and hide support buttons
            screen.setText(EXIT_WIDGET_ID, "Background", u"<font=4>"+ "BACK" + u"</font>", 
                           CvUtil.FONT_RIGHT_JUSTIFY, self.x_exit, self.Y_EXIT, Z_TEXT, 
                           FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, 1, -1)
            screen.hide(CANCEL_WIDGET_ID)
            screen.hide(RESET_WIDGET_ID)

        # --- Anarchy Text ---
        anarchy_turns = active_player.getCivicAnarchyLength(self.display_civics)

        if active_player.canRevolution(0):
            anarchy_text = local_text.getText("TXT_KEY_ANARCHY_TURNS", (anarchy_turns, ))
        else:
            anarchy_text = CyGameTextMgr().setRevolutionHelp(self.active_player_id)

        screen.setLabel("CivicsRevText", "Background", u"<font=3>" + anarchy_text + u"</font>", 
                        CvUtil.FONT_CENTER_JUSTIFY, self.x_center, BOTTOM_LINE_TOP + TEXT_MARGIN // 2, 0, 
                        FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

        # --- Maintenance/Upkeep Text ---
        upkeep_gold = active_player.getCivicUpkeep(self.display_civics, True)
        upkeep_inflation_adjusted = upkeep_gold * (100 + active_player.calculateInflationRate()) // 100
        upkeep_text = local_text.getText("TXT_KEY_CIVIC_SCREEN_UPKEEP", (upkeep_inflation_adjusted, ))
        
        screen.setLabel("CivicsUpkeepText", "Background", u"<font=3>" + upkeep_text + u"</font>", 
                        CvUtil.FONT_CENTER_JUSTIFY, self.x_center, BOTTOM_LINE_TOP + BOTTOM_LINE_HEIGHT - 2 * TEXT_MARGIN, 0, 
                        FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
        

    def handle_revolution_click(self, input_class):
        """Handles the 'SET CIVICS' or 'BACK' button click."""
        if input_class.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            # Update the delivered civics list with the new selections
            for i, civic_id in enumerate(self.current_civics):
                if civic_id != self.all_delivered_civics[i]:
                    self.all_delivered_civics[i] = civic_id

            screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REGULAR_CIVICS_SCREEN) # Must create here
            screen.hideScreen()
            CvScreensInterface.showCivicsScreen(self.all_delivered_civics, 'regularcivics')

    def cancel_changes(self, input_class):
        """Reverts current and display civics to the state when the screen was opened (original_civics)."""
        if input_class.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            for i in range (gc.getNumCivicOptionInfos()):
                self.current_civics[i] = self.original_civics[i]
                self.display_civics[i] = self.original_civics[i]
            
            screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REGULAR_CIVICS_SCREEN) # Must create here
            self.draw_contents(screen)
    
    def reset_to_original(self, input_class):
        """Reverts current and display civics to the player's actually active civics (absolute_original_civics)."""
        if input_class.getNotifyCode() == NotifyCode.NOTIFY_CLICKED:
            for i in range (gc.getNumCivicOptionInfos()):
                self.current_civics[i] = self.absolute_original_civics[i]
                self.display_civics[i] = self.absolute_original_civics[i]
            
            # doto civics parent reset (your mod logic)
            for i in range (gc.getNumCivicOptionInfos()):
                self.child_civics[i] = []

            screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REGULAR_CIVICS_SCREEN) # Must create here
            self.draw_contents(screen)
            self.update_anarchy_and_buttons(screen) 

    # --- Utility Functions ---
    def get_civics_button_name(self, civic_id):
        """Standardized naming for civic button widgets."""
        return BUTTON_WIDGET_PREFIX + str(civic_id)

    def get_civics_text_name(self, civic_id):
        """Standardized naming for civic text widgets."""
        return TEXT_WIDGET_PREFIX + str(civic_id)

    def handleInput(self, inputClass):
        """Handles input events (pulldowns, buttons)."""
        
        # In Civ4 style, you often need the screen object to get values/change it
        screen = CyGInterfaceScreen(self.SCREEN_NAME, CvScreenEnums.REGULAR_CIVICS_SCREEN)

        if inputClass.getNotifyCode() == NotifyCode.NOTIFY_LISTBOX_ITEM_SELECTED:
            index = screen.getSelectedPullDownID(DEBUG_DROPDOWN_ID)
            self.set_active_player(screen.getPullDownData(DEBUG_DROPDOWN_ID, index))
            self.draw_contents(screen)
            return 1
        
        function_name = inputClass.getFunctionName()
        # Find the function based on the prefix/name.
        for key, func in self.input_map.items():
            if function_name.startswith(key):
                func(inputClass)
                return 1
        
        return 0
        
    def update(self, fDelta):
        pass

    # --- Custom Mod Logic ---
    def is_parent_child_civic_option(self, option_id):
        """Checks if a Civic Option is a Parent or Child for your mod logic."""
        parent_civic_option = gc.getCivicOptionInfo(option_id).getParentCivicOption()
        return bool(parent_civic_option and parent_civic_option > 0)
        
    def get_parent_child_civic_options_count(self):
        """Calculates the number of Civic Options that are Parent/Child and should be skipped."""
        
        # FIX for Python 2.4: Replace generator expression with list comprehension if sum() is used on it.
        # While the current code might work, explicit list creation is safer for older Python 2 versions.
        return len([i for i in range(gc.getNumCivicOptionInfos()) if self.is_parent_child_civic_option(i)])