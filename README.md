# mbed-os-emwin-ssd1306
This repository contains the configuration files required to run emWin using the CY8CKIT-032 shield.  That shield has a Saloman SSD1306 Display driver that is attached using a I2C Bus

These files contain the GUI configuration files as well as the the SSD1306 interface files.  The interface is implemented using the mbedos HAL.

To use these files you should
* mbed add git@github.com:iotexpert/mbed-os-emwin-ssd1306.git

The pins are correct for the targets
* CY8CKIT062_WIFI_BT

To use this library you need to add the emWin library to mbed_app.json
```json
"target_overrides": {
        "*": {
            "target.components_add": ["EMWIN_OSNTS"]
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
All of the pins are defined in mbed_lib.json
```json
{

}
```
