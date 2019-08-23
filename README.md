# mbed-os-emwin-ssd1306
This repository contains the configuration files required to run emWin using the CY8CKIT-032 shield.  That shield has a SOLOMON SYSTECH SSD1306 Display driver that is attached using a I2C Bus

These files contain the GUI configuration files as well as the the SSD1306 interface files.  The interface is implemented using the mbedos HAL.

To use these files you should
* mbed add git@github.com:iotexpert/mbed-os-emwin-ssd1306.git
* mbed add git@github.com:cypresssemiconductorco/emwin.git

The pins are correct for the targets
* CY8CKIT062_WIFI_BT
* CY8CKIT_062_BLE

To use this library you need to add the emWin library to mbed_app.json
```json
"target_overrides": {
        "*": {
            "target.components_add": ["EMWIN_OSNTS"]
        }
```
An example program
```
#include "mbed.h"
#include "GUI.h"

int main()
{
    GUI_Init();
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);
    GUI_SetFont(GUI_FONT_8_ASCII);
    GUI_SetTextAlign(GUI_TA_CENTER);
    GUI_DispStringAt("Hello World", GUI_GetScreenSizeX()/2,GUI_GetScreenSizeY()/2);
}
```

If you need to use it for a different kit you can add pins using the target overides like this:
```json
"target_overrides": {
        "*": {
            "target.components_add": ["EMWIN_OSNTS"]
        },
        "CY8CKIT_062_WIFI_BT" : {
            "ST7789V_TFT.TFTRD":"P12_3"
        }
    }
```
All of the configuration and pins are defined in mbed_lib.json
```json
{
    "name" : "SSD1306_OLED",
    "config": {
        "SDA":"P6_1",
        "SCL":"P6_0",
        "I2CADDRESS":"0x78",
        "I2CFREQ":"400000"
    }
}
```
