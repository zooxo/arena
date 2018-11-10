# ARENA - Arduboy 3d Shooter
Welcome to ARENA (Arduboy 3d Shooter)  
Version 1.0 ... (c) 2018 by deetee/zooxo  
This software is covered by the 3-clause BSD license.  

![ARENA](https://user-images.githubusercontent.com/16148023/48252998-865d5380-e406-11e8-815f-6acf1334c7d1.jpg)  

See a short video of AOS:  
 Version 1.0: https://youtu.be/KJUq3ZrXiq8

## PREAMBLE
Actually I am not a gamer - except puzzels and logic games. But my first contacts with Wolfenstein and Quake were very impressive because of the overwhelming plastic effects. Doing the math, when programming a 3D-game seemed to be a real challenge. Many years (and the fantastic Arduboy hardware) later I am giving this challenge a try.  
I asserted that using approximating approaches are often faster and memory friendly than using the exact vector math. And the eyes don't really care if the tiles of a wall are drawn exactly.  
So I tried to program a game which has very open setting possibilities. You can determine how many enemies and medipacks you want to play with. You can even draw your own arena maze with the build in map editor and save it permanently to the EEPROM memory. With the OFF feature ("deep sleep mode") you can pause your game anytime and go on weeks later without charging the battery. You can even toggle between a 3D an 2D view (even if the latter one is too small and was intended for orientation purposes).

Enjoy!  
deetee

## OVERVIEW

![arena_overview](https://user-images.githubusercontent.com/16148023/48253026-970dc980-e406-11e8-9c46-7cb1a5500ddf.jpg)

## SCREEN

![arena_screen](https://user-images.githubusercontent.com/16148023/48253033-9aa15080-e406-11e8-964e-16f50c795315.jpg)

## ENEMIES AND OBJECTS

![arena_enemies](https://user-images.githubusercontent.com/16148023/48254125-67ac8c00-e409-11e8-8e9d-fe9e67bcb907.jpg)

## MENU
``````
     NEW GAME   ... Start a new game (doesn't reset settings -> see RESET)
     SETTINGS   ... Reset all setting variables
       2D/3D    ... Toggle between 2d- and 3d-view
       MEDI     ... Set number of medipacks (1-5)
       AMMO     ... Set number of ammopacks (1-5)
       ENEMIES  ... Set number of enemies (1-9)
       SOUND    ... Sound on/off
       CONTRAST ... Set contrast/brightness of screen (0...255)
       STATUS   ... Show/hide status view (health, ammo, compass)
       SKY      ... Show/hide sky view (ceiling and floor line)
       MESSAGES ... Show/hide messages (comments)
       RESET    ... Reset settings to default values
     EDIT MAP   ... Map editor (edit current map)
       WHITE    ... Draw white pixel at cursor
       BLACK    ... Draw black pixel at cursor (clear pixel)
       CLEAR    ... Clear map (no walls at all)
       LOAD     ... Load to EEPROM saved map (slot 1-9)
       SAVE     ... Save current map to EEPROM (slot 1-9)
     BATT       ... Show supply voltage x 100 - approx. 310 (empty) to 430 (full)
     ABOUT      ... About info
     OFF        ... Deep sleep mode (wake up with button A)
``````     
