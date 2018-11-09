# ARENA - Arduboy 3d Shooter
Welcome to ARENA (Arduboy 3d Shooter)  
Version 1.0 ... (c) 2018 by deetee/zooxo  
This software is covered by the 3-clause BSD license.  

xxx![aos](https://user-images.githubusercontent.com/16148023/47264994-5adefb80-d521-11e8-996a-7acc3bd4717e.jpg)  

xxxSee a short video of AOS:
xxx* Version 1.0: https://youtu.be/dVvj3j-pYlQ

## PREAMBLE
Actually I am not a gamer - except puzzels and logic games. But my first contacts with Wolfenstein and Quake were very impressive because of the overwhelming plastic effects. Doing the math, when programming a 3D-game seemed to be a real challenge. Many years (and the fantastic Arduboy hardware) later I am giving this challenge a try.  
I asserted that using approximating approaches are often faster and memory friendly than using the exact vector math. And the eyes don't really care if the tiles of a wall are drawn exactly.  
So I tried to program a game which has very open setting possibilities. You can determine how many enemies and medipacks you want to play with. You can even draw your own arena maze with the build in map editor and save it permanently to the EEPROM memory. With the OFF feature ("deep sleep mode") you can pause your game anytime and go on weeks later without charging the battery. You can even toggle between a 3D an 2D view (even if the latter one is too small and was inteded for orientation purposes).

Enjoy!
deetee


## OVERVIEW

![all](https://user-images.githubusercontent.com/16148023/46715416-2edd9380-cc60-11e8-8aa4-b2d6a0e266b3.png)

Central part of AOS is the memory which consists of three different kinds of stacks:
* **Bytestack**:
This stack of 16 bytes is the central memory for inputs of whole numbers between 0 and 255 which can be interpreted as figure or ascii character. With [Ibyte], [Ichar] and [Inum] values can be entered respectively with [PpB] the value of the next program step will be interpreted as a number (and not as a command).
Values of the bytestack can further be used as parameters for settings, as characters for printing messages, as bytes to send to interfaces or as ascii characters to create float numbers.
* **Floatstack**:
Similar to most RPN-calculators the floatstack consists of four registers (X, Y, Z and T) which are able to hold float numbers (-1E38 to 1E38) with a precision of mostly 7 significant digits.
Once on the floatstack numbers can be processed (i.e. added), treated (i.e. mathematical function) or compared (conditions in programs).
To save the floatstack permanently to the EEPROM use [Fsave].
* **Program**:
Two stacks of 256 bytes (0...255) can hold programs (like shell scripts) or ascii characters (text). Programs can be executed with [Prun] and edited with [Pedit] respectively text files can be edited with [TXT]. To switch between active program and inactive twin use [Pswap]. Even a simple subprogram feature is possible (see [Psub]).
Note that there are some commands that are only useful in executable programs (like [PpB], [Pgoto] or [Psub]).


## COMMANDS

### **COMMANDS - Sections** (capital letters in commands):
``````
    B     bytestack operations
    F     floatstack operations
    C     operations with constants
    CALC  RPN-calculator
    D     disk operations (EEPROM)
    I     input operations to select bytes
    KBD   sends selected bytes as HID (keyboard) to USB
    OFF   deep sleep mode (wake up with A) - auto poweroff after 3 min
    P     programing related commands (ie goto)
    S     settings (ie contrast, screensaver time)
    TERM  control aos with PC-keyboard
    TORCH toggle torch on/off
    TXT   simple text editor
    WATCH simple digital clock
``````

### **COMMANDS - Overview** (commands and corresponding ascii characters):
  ```
      DEC  |  0       1       2       3       4       5       6       7
      -----|--+-------+-------+-------+-------+-------+-------+-------+------
      032  |    about ! B2F   " B>>F  # Bclr  $ Bprnt % Bshow & CALC  ' Cload
      040  |  ( Csave ) D     * Dfmt  + F!    , F*    - F+    . F-    / F/
      048  |  0 F2h   1 F2hms 2 F2p   3 F2r   4 F<    5 F<=   6 F<>   7 F=
      056  |  8 F>    9 F>=   : Facos ; Facsh < Fannu = Fasin > Fasnh ? Fatan
      064  |  @ Fatnh A Fbatt B Fce   C Fchs  D Fclx  E Fcos  F Fcosh G FEE
      072  |  H Fexp  I Fgaus J Fint  K Finv  L Fln   M FLR   N Fpow  O Fpush
      080  |  P Frcl  Q Frot  R Fsave S Fshow T Fsin  U Fsinh V Fsolv W Fsqrt
      088  |  X Fstat Y Fsto  Z Fsum  [ Fswap \ Ftan  ] Ftanh ^ Ibyte _ Ichar
      096  |  ` Inum  a KBD   b OFF   c Pedit d Pexp  e Pgoto f Pimp  g Pname
      104  |  h Pnop  i PpB   j Ppse  k Prun  l Pstop m Psub  n Pswap o Py/n
      112  |  p Sfn   q Slit  r Sss   s TERM  t TORCH u TXT   v WATCH w Wset
```

### **COMMANDS** - Detailed description:
* **[about]** Print about message  
Prints the name of this software, the version and the author.
* **[B2F]** Pull bytestack[0] and push/cast to floatstack[0]  
Pulls first bytestack register and pushes it (with casting to double) to the first floatstack register.
* **[B>>F]** Flush bytestack to calculator  
Flushes the inverted bytestack (not equal NULL) to become interpreted by the the calculator (ie digits, decimal point). Useful in programs to enter float numbers to the floatstack.
* **[Bclr]** Clear bytestack  
