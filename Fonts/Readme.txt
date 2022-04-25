Copy files in 
//C:\Users\nlv05901\Documents\Arduino\libraries\GxFont_GFX_TFT_eSPI-master\Fonts\Custom
//C:\Users\nlv05901\Documents\Arduino\libraries\Adafruit_GFX_Library\Fonts
C:\Users\nlv05901\Documents\Arduino\libraries\GxFont_GFX_TFT_eSPI-master\Fonts\GFXFF

C:\Users\nlv05901\Documents\Arduino\libraries\GxEPD-master\src\GxFont_GFX.h
// select the library/libraries to add, none to preserve code space
//#include "U8g2_for_Adafruit_GFX.h"
//#include <Adafruit_ftGFX.h>
#include <GxFont_GFX_TFT_eSPI.h>


Declare in User_Custom_Fonts.h
  #include <Fonts/Custom/Sea_Dog_2001_Italic12pt7b.h> // CF SD1p12
  #include <Fonts/Custom/Sea_Dog_2001_Italic16pt7b.h> // CF SD1p16
  #include <Fonts/Custom/Sea_Dog_2001_Italic20pt7b.h> // CF SD1p20
  #include <Fonts/Custom/SF_Distant_Galaxy12pt7b.h>   // CF SD2p12
  #include <Fonts/Custom/SF_Distant_Galaxy16pt7b.h>   // CF SD2p16
  #include <Fonts/Custom/SF_Distant_Galaxy20pt7b.h>   // CF SD2p20
  #include <Fonts/Custom/SPINC___12pt7b.h>            // CF SD3p12
  #include <Fonts/Custom/SPINC___16pt7b.h>            // CF SD3p16
  #include <Fonts/Custom/SPINC___20pt7b.h>            // CF SD3p20
...
#define SD1p12  &Sea_Dog_2001_Italic12pt7b
#define SD1p16  &Sea_Dog_2001_Italic16pt7b
#define SD1p20  &Sea_Dog_2001_Italic20pt7b
#define SD2p12  &SF_Distant_Galaxy12pt7b
#define SD2p16  &SF_Distant_Galaxy16pt7b
#define SD2p20  &SF_Distant_Galaxy20pt7b
#define SD3p12  &SPINC___12pt7b.h
#define SD3p16  &SPINC___16pt7b.h
#define SD3p20  &SPINC___20pt7b.h
