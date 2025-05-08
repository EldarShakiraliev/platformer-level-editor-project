                                        Shaliraliev Eldar Project #2
Simple guide for the program:
Use tile buttons on the left to place icons to change the level as you like.
Use Edit menu to undo, to resize, or clear.
Use undo to get rid of the last tile placement.
Use resize to change the amount of rows and columns.
Use clear to get rid of all the icons on the level.
Export your project from File or use File to close all windows.
Edit your own rll files or create a new one.

Description of the Project #2:
All basic features have been added to my Level Editor Program, as Grid editor, Tile selection panel, Basic editing tools, Menu bar, Export functionality and Documentation.
The Grid editor and Tile selection panel were added with help of the bonus lecture. 
Basic editing tools as: clear, undo and resize at first were buttons of Toolbox, but after I added them inside my Menu bar as an Edit button.
Menu bar consists of three parts: File, Edit, Help. Edit button is responsible for Basic editing tools. Help button is responsible for this Documentation and a simple guide for that program.
Export functionality was added as a part of File button, called Export Level. It converts the custom level, that was made by a user into a rll file that could be used later in Project #1 or Project #2 to be edited later.

Advanced features:
1) Splash and Welcome screen and Style sheets usage. Added the custom splash screen, that was made using the Project #1. The length of the splash screen is 3 seconds, then, the welcome screen appears. The welcome screen gives a choice to a user: Create new level, Edit level using the import functionality, or Cancel the working process to close the program. 
2) Additional level element tiles. I added 3 additional level tiles: cherry, diamond and shoe. All the tiles have their own logic in Project #1. All the new tiles have their new sounds. 
   Cherry: If the player picks up the cherry, his lives is increased by one. If he does have three lives (maximum), his lives will not increase. 
   Diamond: If the player picks up the diamond, his coin counter will increase by 5.
   Shoe: If the player picks up the shoe, his jump strength will increase by 3 tiles. By default, the player can jump 4 tiles in height, and with a shoe, the player jumps 7 tiles in height.
3) Import functionality & Error-handling. The user can import the rll file with any level, so that he could edit it. The error handling part was used to check if the rll file is corrupted, if it has wrong symbols, or if it is not the rll file.