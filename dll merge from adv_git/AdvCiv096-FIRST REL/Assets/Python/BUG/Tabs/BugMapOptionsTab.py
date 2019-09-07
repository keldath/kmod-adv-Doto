## BugMapOptionsTab
##
## Tab for the BUG Map Options.
##
## Copyright (c) 2009 The BUG Mod.
##
## Author: EmperorFool

import BugOptionsTab

class BugMapOptionsTab(BugOptionsTab.BugOptionsTab):
	"BUG Nap Options Screen Tab"
	
	def __init__(self, screen):
		BugOptionsTab.BugOptionsTab.__init__(self, "Map", "Map")

	def create(self, screen):
		tab = self.createTab(screen)
		panel = self.createMainPanel(screen)
		column = self.addOneColumnLayout(screen, panel)
		
		left, center, right = self.addThreeColumnLayout(screen, column, "Top", True)
		# advc.004:
		self.addLabel(screen, left, "Layers", "Layers:")
		# <advc.004z>
		self.addCheckbox(screen, left, "MainInterface__ScoresInGlobeView")
		self.addCheckbox(screen, left, "MainInterface__ResourceIconOptions")
		self.addCheckbox(screen, left, "MainInterface__TribalVillageIcons")
		# </advc.004z>
		# advc.004m:
		self.addCheckbox(screen, left, "MainInterface__StartWithResourceIcons")
		# <advc.004h>
		self.addCheckbox(screen, left, "MainInterface__FoundingYields")
		self.addTextDropdown(screen, left, left, "MainInterface__FoundingBorder")
		# </advc.004h>
		self.addSpacer(screen, left, "Left1") # advc.004
		self.addLabel(screen, left, "StrategyOverlay", "Dot Map Overlay:")
		self.addCheckbox(screen, left, "StrategyOverlay__Enabled")
		# advc.004: Disabled; Ctrl+X should suffice.
		#self.addCheckbox(screen, left, "StrategyOverlay__ShowDotMap")
		self.addCheckbox(screen, left, "StrategyOverlay__DotMapDrawDots")
		leftL, leftR = self.addTwoColumnLayout(screen, left, "DotMapBrightness")
		#self.addTextEdit(screen, leftL, leftR, "StrategyOverlay__DotMapDotIcon")
		self.addSlider(screen, leftL, leftR, "StrategyOverlay__DotMapBrightness", False, False, False, "up", 0, 100)
		self.addSlider(screen, leftL, leftR, "StrategyOverlay__DotMapHighlightBrightness", False, False, False, "up", 0, 100)
		#self.addLabel(screen, center, "CityBar", "CityBar:")
		#self.addCheckbox(screen, center, "CityBar__StarvationTurns")
		# advc.011b, advc.099f:
		self.addLabel(screen, center, "TileHover", "Tile Hover:")
		# advc.011b:
		self.addCheckbox(screen, center, "MiscHover__PartialBuildsAlways")
		# advc.099f:
		self.addCheckbox(screen, center, "MiscHover__CultureInUnownedTiles")
		# advc.061:
		self.addCheckbox(screen, center, "MainInterface__ListUnitsPerOwner")
		#self.addCheckbox(screen, center, "MiscHover__LatLongCoords")
		#self.addCheckbox(screen, center, "MiscHover__PartialBuilds")
		# <advc.002f>
		self.addLabel(screen, center, "CityIcons", "City Icons:")
		self.addCheckbox(screen, center, "MainInterface__CityNetworkIcon")
		self.addCheckbox(screen, center, "MainInterface__AirportIcon")
		# </advc.002f>
		self.addLabel(screen, right, "Misc", "Misc:")
		self.addCheckbox(screen, right, "MainInterface__FieldOfView")
		self.addCheckbox(screen, right, "MainInterface__FieldOfView_Remember", True)
		self.addCheckbox(screen, right, "EventSigns__Enabled")
		
		#self.addCheckbox(screen, right, "Actions__IgnoreHarmlessBarbarians")
		
		# advc.009c: Commented out
		#screen.attachHSeparator(column, column + "Sep1")
		#left, right = self.addTwoColumnLayout(screen, column, "MapFinderEnabled", True)
		#self.addLabel(screen, left, "MapFinder", "MapFinder:")
		#self.addCheckbox(screen, right, "MapFinder__Enabled")
		
		#self.addTextEdit(screen, column, column, "MapFinder__Path")
		#self.addTextEdit(screen, column, column, "MapFinder__SavePath")
		
		#left, right = self.addTwoColumnLayout(screen, column, "MapFinder", True)
		#leftL, leftR = self.addTwoColumnLayout(screen, left, "MapFinderDelays")
		#self.addFloatDropdown(screen, leftL, leftR, "MapFinder__RegenerationDelay")
		#self.addFloatDropdown(screen, leftL, leftR, "MapFinder__SkipDelay")
		#self.addFloatDropdown(screen, leftL, leftR, "MapFinder__SaveDelay")
		
		#rightL, rightR = self.addTwoColumnLayout(screen, right, "MapFinderLimits")
		#self.addTextEdit(screen, rightL, rightR, "MapFinder__RuleFile")
		#self.addTextEdit(screen, rightL, rightR, "MapFinder__RegenerationLimit")
		#self.addTextEdit(screen, rightL, rightR, "MapFinder__SaveLimit")
