/*********************************************************************
*                SEGGER Microcontroller GmbH                         *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2018  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.48 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software  has been licensed to  Cypress Semiconductor Corporation,
whose registered  office is situated  at 198 Champion Ct. San Jose, CA 
95134 USA  solely for the  purposes of creating  libraries for Cypress
PSoC3 and  PSoC5 processor-based devices,  sublicensed and distributed
under  the  terms  and  conditions  of  the  Cypress  End User License
Agreement.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              Cypress Semiconductor Corp, 198 Champion Ct., San Jose, CA 95134, USA
Licensed SEGGER software: emWin
License number:           GUI-00319
License model:            Services and License Agreement, signed June 10th, 2009
Licensed platform:        Any Cypress platform (Initial targets are: PSoC3, PSoC5)
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2009-06-12 - 2022-07-27
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/
#include "mbed.h"
#include "GUI.h"
#include "GUIDRV_SPage.h"
#include "i2c_portapi.h"


/*********************************************************************
*
*       Layer configuration
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS 128
#define YSIZE_PHYS 64

//
// Color conversion
//
#define COLOR_CONVERSION GUICC_1

//
// Display driver, display data cache enabled
//
#define DISPLAY_DRIVER GUIDRV_SPAGE_1C1

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif
#ifndef   DISPLAY_DRIVER
  #error No display driver defined!
#endif

/*********************************************************************
*
*       SSD1306 Configuration Register Defines
*
**********************************************************************/

////////////////////////////////////////////////////////////////////////
// Fundamental Command Table Page 28
////////////////////////////////////////////////////////////////////////
#define OLED_SETCONTRAST                              0x81
// 0x81 + 0-0xFF Contrast ... reset = 0x7F

// A4/A5 commands to resume displaying data
// Resume to RAM content display
#define OLED_DISPLAYALLONRESUME                       0xA4

// Entire display ON Output ignores RAM content 
// I cant think of a reason you would do this
#define OLED_DISPLAYALLON                             0xA5
 
// 0xA6/A7 Normal 1=white 0=black Inverse 0=white  1=black
#define OLED_DISPLAYNORMAL                            0xA6
#define OLED_DISPLAYINVERT                            0xA7

// 0xAE/AF are a pair to turn screen off/on
#define OLED_DISPLAYOFF                               0xAE
#define OLED_DISPLAYON                                0xAF

////////////////////////////////////////////////////////////////////////
// Scrolling Commands 
////////////////////////////////////////////////////////////////////////

#define OLED_RIGHT_HORIZONTAL_SCROLL                  0x26
#define OLED_LEFT_HORIZONTAL_SCROLL                   0x27
#define OLED_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL     0x29
#define OLED_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL      0x2A

#define OLED_DEACTIVATE_SCROLL                        0x2E
#define OLED_ACTIVATE_SCROLL                          0x2F

#define OLED_SET_VERTICAL_SCROLL_AREA                 0xA3

////////////////////////////////////////////////////////////////////////
// Address Setting Command Table
////////////////////////////////////////////////////////////////////////


// 00-0F - set lower nibble of page address
// 10-1F - set upper niddle of page address

#define OLED_SETMEMORYMODE                            0x20
// 00 = horizontal, 01 = vertical 2= page >=3=illegal

// Only used for horizonal and vertical address modes
#define OLED_SETCOLUMNADDR                            0x21
#define OLED_SETPAGEADDR                              0x22

// Only used for page address modes
#define OLED_SETLOWCOLUMN                             0x00
#define OLED_SETHIGHCOLUMN                            0x10

////////////////////////////////////////////////////////////////////////
// Hardware Configuration
////////////////////////////////////////////////////////////////////////

// 40-7F - set address startline from 0-127 (6-bits)
#define OLED_SETSTARTLINE_ZERO                        0x40

// Y Direction
#define OLED_SEGREMAPNORMAL                           0xA0
#define OLED_SEGREMAPINV                              0xA1

#define OLED_SETMULTIPLEX                             0xA8

// X Direction
#define OLED_COMSCANINC                               0xC0
#define OLED_COMSCANDEC                               0xC8

// double byte with image wrap ...probably should be 0
#define OLED_SETDISPLAYOFFSET                         0xD3

// Double Byte Hardware com pins configuration
#define OLED_SETCOMPINS                               0xDA
// legal values 0x02, 0x12, 0x022, 0x032

#define OLED_SETDISPLAYCLOCKDIV                       0xD5
#define OLED_SETPRECHARGE                             0xD9

#define OLED_SETVCOMDETECT                            0xDB
#define OLED_NOP                                      0xE3


#define OLED_CHARGEPUMP                               0x8D
#define OLED_CHARGEPUMP_ON                            0x14
#define OLED_CHARGEPUMP_OFF                           0x00


/*********************************************************************
*
*       _InitController
*
* Purpose:
*   Initializes the display controller
*/

static void _InitController(void) 
{
    const char initializeCmds[]={
        //////// Fundamental Commands
        OLED_DISPLAYOFF,          // 0xAE Screen Off
        OLED_SETCONTRAST,         // 0x81 Set contrast control
        0x7F,                     // 0-FF ... so half way

        OLED_DISPLAYNORMAL,       // 0xA6, //Set normal display 

        //////// Scrolling Commands
        OLED_DEACTIVATE_SCROLL,   // Deactive scroll

        //////// Addressing Commands
        OLED_SETMEMORYMODE,       // 0x20, //Set memory address mode
        0x02,                     // Page

        //////// Hardware Configuration Commands
        OLED_SEGREMAPINV,         // 0xA1, //Set segment re-map 
        OLED_SETMULTIPLEX,        // 0xA8 Set multiplex ratio
        0x3F,                     // Vertical Size - 1
        OLED_COMSCANDEC,          // 0xC0 Set COM output scan direction
        OLED_SETDISPLAYOFFSET,    // 0xD3 Set Display Offset
        0x00,                     //
        OLED_SETCOMPINS,          // 0xDA Set COM pins hardware configuration
        0x12,                     //
   
        //////// Timing and Driving Settings
        OLED_SETDISPLAYCLOCKDIV,  // 0xD5 Set display oscillator frequency 0-0xF /clock divide ratio 0-0xF
        0x80,                     // Default value
        OLED_SETPRECHARGE,        // 0xD9 Set pre-changed period
        0xF1,                     //
        OLED_SETVCOMDETECT,       // 0xDB, //Set VCOMH Deselected level
        0x20,                     // Default 

        //////// Charge pump regulator
        OLED_CHARGEPUMP,          // 0x8D Set charge pump
        0x14,                     // VCC generated by internal DC/DC circuit

        // Turn the screen back on...       
        OLED_DISPLAYALLONRESUME,  // 0xA4, //Set entire display on/off
        OLED_DISPLAYON,           // 0xAF  //Set display on
    };

    I2C_Init();
    I2C_WriteCmdStream(initializeCmds, sizeof(initializeCmds));
}


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*   
*/
void LCD_X_Config(void) 
{
    CONFIG_SPAGE Config = {0};
    GUI_DEVICE * pDevice;
    GUI_PORT_API PortAPI = {0};

    //
    // Set display driver and color conversion for 1st layer
    //
    pDevice = GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
    //
    // Display size configuration
    //
    LCD_SetSizeEx (0, XSIZE_PHYS,   YSIZE_PHYS);
    LCD_SetVSizeEx(0, VXSIZE_PHYS,  VYSIZE_PHYS);
    //
    // Driver configuration
    //
    Config.FirstSEG = 0;
    Config.FirstCOM = 0;
    GUIDRV_SPage_Config(pDevice, &Config);
    //
    // Configure hardware routines
    //
    PortAPI.pfWrite8_A0  = I2C_WriteCmd;
    PortAPI.pfWrite8_A1  = I2C_WriteData;
    PortAPI.pfWriteM8_A1 = I2C_WriteDataStream;
    
    /* SSD1306 is not readable through i2c. Cache is enabled 
    to use display without data read operations*/
    
    GUIDRV_SPage_SetBus8(pDevice, &PortAPI);
    //
    // Controller configuration
    //
    GUIDRV_SPage_Set1510(pDevice);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if 
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  GUI_USE_PARA(LayerIndex);
  GUI_USE_PARA(pData);
  switch (Cmd) 
    {
    case LCD_X_INITCONTROLLER:
        //
        // Called during the initialization process in order to set up the
        // display controller and put it into operation. If the display
        // controller is not initialized by any external routine this needs
        // to be adapted by the customer...
        //
        _InitController();
        r = 0;
        break;

    case LCD_X_ON: 
        I2C_WriteCmd(OLED_DISPLAYON);
        r = 0;
        break;
        
    case LCD_X_OFF: 
        I2C_WriteCmd(OLED_DISPLAYOFF);
        r = 0;
        break;

    default:
        r = -1;
        break;
  }
  return r;
}

/*************************** End of file ****************************/
