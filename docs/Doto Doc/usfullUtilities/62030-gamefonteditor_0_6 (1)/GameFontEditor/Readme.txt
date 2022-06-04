Civilization IV Game Font Editor by Asaf - version 0.5
-------------------------------------------------------

Download link: http://forums.civfanatics.com/downloads.php?do=file&id=17276
Discussion thread: http://forums.civfanatics.com/showthread.php?t=429541
Asaf's CFC home page: http://forums.civfanatics.com/member.php?u=179974

This software is used under GNU GPL, version 2. See license-gpl.txt which comes with this program.
(license source: http://www.opensource.org/licenses/gpl-2.0.php)

This software uses the FreeImage open source image library. See http://freeimage.sourceforge.net for details.
FreeImage is used under the GNU GPL, version 2. See license-gpl.txt which comes with this program.
(license source: http://www.opensource.org/licenses/gpl-2.0.php)

Icons are taken from http://www.iconza.com/

Change log
============
V0.6 changes:
--------------
- Bug fix (reported by stolenrays): Scrollbar jumps to its initial position after most actions when image file is large (e.g. WOC)

V0.5 changes:
--------------
- Bug fix: Could not find rows if 'rows fixing' was disabled (i.e. max corrupted pixels = 0), even if no corrupted pixels were found.
- Bug fix: Image type is identified even if renamed and of unusual size - looks for '75' in the file name.

V0.4 changes:
--------------
- Feature: Added ability to add transparency to an opaque icon: 'Alpha!' button. Transparent color is determined by the image corners, and the sensitivity can be changed using a slider. (Useful, for example, when you've found an image online: Just do a 'copy image' from the browser - no need to even save it to file - then paste it into a cell, and add transparency.)
- Feature: Can run an external image editor to specific slots from within the game font editor (click on 'Edit'), and then update the slot after saving in the external editor (click on 'Update').
- Feature: Added config file which holds all the user's settings to be loaded for the next time. File is created and updated when exiting the program, and is saved in the Documents folder (or 'My Documents'). It's called Civ4GameFontEditor.cfg.
- Feature: Enabled changing the background color for the font image when not showing the grid.
- Bug fix: Changed UI colors to absolute colors
- Bug fix: Fixed a bug that caused a crash when using 'Save As'
- Bug fix: Fixed a bug where title did not change when using 'Save As'

V0.3 changes:
--------------
- Feature: Added undo/redo mechanism. There's no limit on number of actions.
- Feature: Added cut/copy/paste of icons in the image. (Can also import/export that way from other applications, but it usually doesn't copy the alpha channel)
- Feature: Added ability to add a 'Star Overlay' to an existing icon, to mark holy city and corp HQ.
- Feature: Added auto-backup for image. Happens once before saving it for the first time. Does not override previous backups.
- Feature: Can optionally enable editing the actual fonts.
- Feature: More settings to allow flexibilty of the auto-correction functionality.
- Bug fix: Allows more flexibility for rows height and font position.
- Bug fix: When clearing cell, cell image was not updated.
- UI: Added tool bar icons.
- UI: Added hidable settings panel.
- UI: Added hotkeys:
	- Open/Save image (Ctrl-O,Ctrl-S)
	- Cut/Copy/Paste (Ctrl-X,Ctrl-C,Ctrl-V)
	- Undo/Redo (Ctrl-Z,Ctrl-Y)
- UI: Added more tool tips
- UI: Added an exception display dialog, in case the program crashes.

V0.2 changes:
--------------
- Bug fix: Editor didn't work on Windows 64 bit.
- Bug fix: Corrupted huge WoC font files when loaded.
- Bug fix: When not showing grid, changes were not displayed.

V0.1 features:
--------------
- Opening and analyzing game font files.
- Exporting individual slot images from the font file to image files (TGA, PNG).
- Importing image files (TGA, PNG, DDS) into specific slots in the font files.
- Clearing specific slots.
- Editing the cyan marker - adding/removing and changing its position.
- Automatic fix of corrupted game font files. Handles bad alpha and corrupted colors on the grid.
- Saving your changes to a game font file (kinda obvious).
- Displaying the game font image with and without the grid.
