/******************************************************************************
* File Name: i2c_portapi.cpp
*
* Version: 1.00
*
* Description: This file contains the I2C port API functions that will be
*               used by emWin to communicate with the OLED display
*
* Hardware Dependency: CY8CKIT-032 AFE Shield
*
*******************************************************************************
* Copyright (2019), Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* (“Software”), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries (“Cypress”) and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software (“EULA”).
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress 
* reserves the right to make changes to the Software without notice. Cypress 
* does not assume any liability arising out of the application or use of the 
* Software or any product or circuit described in the Software. Cypress does 
* not authorize its products for use in any products where a malfunction or 
* failure of the Cypress product may reasonably be expected to result in 
* significant property damage, injury or death (“High Risk Product”). By 
* including Cypress’s product in a High Risk Product, the manufacturer of such 
* system or application assumes all risk of such use and in doing so agrees to 
* indemnify Cypress against all liability.
*******************************************************************************/

#include "GUI.h"

#include "i2c_portapi.h"
#include "mbed.h"

/*********************************************************************
*
*       Defines: Configuration
*
**********************************************************************
  Needs to be adapted to custom hardware.
*/
/* I2C port to communicate with the OLED display controller */
I2C Display_I2C(MBED_CONF_APP_DISPLAY_SDA, MBED_CONF_APP_DISPLAY_SCL);
//I2C Display_I2C(P6_1, P6_0);

/* I2C slave address, Command and Data byte prefixes for the display controller */
#define OLED_I2C_ADDRESS              (0x78)
#define OLED_CONTROL_BYTE_CMD_BYTE    (0x80)
#define OLED_CONTROL_BYTE_CMD_STREAM  (0x00)

#define OLED_CONTROL_BYTE_DATA_BYTE   (0xC0)
#define OLED_CONTROL_BYTE_DATA_STREAM (0x40)

/* I2C bus speed */
#define I2C_SPEED                   (100000)

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*******************************************************************************
* Function Name: void I2C_Init(void)
********************************************************************************
*
* Summary: This function initializes I2C interface.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void I2C_Init(void) 
{
//    printf("I2C_Init\n");
    Display_I2C.frequency(I2C_SPEED);
//    printf("I2C_INIT Done\n");
}

/*******************************************************************************
* Function Name: void I2C_Write00(unsigned char c) 
********************************************************************************
*
* Summary: This function writes a command byte to the display controller with A0 = 0
*
* Parameters:
*  unsigned char c - command to be written to the display controller
*
* Return:
*  None
*
*******************************************************************************/
void I2C_WriteCmd(unsigned char c) 
{
  //  printf("Command = %x\n",c);

    char buff[2];
    
    /* The first byte of the buffer tells the display that the following byte
        is a command */
    buff[0] = OLED_CONTROL_BYTE_CMD_BYTE;
    buff[1] = (char)c;
    
    /* Write the buffer to display controller */
    Display_I2C.write(OLED_I2C_ADDRESS, buff, sizeof(buff), false);
}

void I2C_WriteCmdStream(const char *data,int num)
{
    char *buffer = (char *)malloc(num+1);
    memcpy(buffer+1,data,num);
    buffer[0] = 0x00; // Continous stream of commands
    int rval = Display_I2C.write(OLED_I2C_ADDRESS, (const char *)buffer, num+1);

}



/*******************************************************************************
* Function Name: void I2C_Write01(unsigned char c) 
********************************************************************************
*
* Summary: This function writes a data byte to the display controller with A0 = 1
*
* Parameters:
*  unsigned char c - data to be written to the display controller
*
* Return:
*  None
*
*******************************************************************************/
void I2C_WriteData(unsigned char c) 
{
//    printf("Data = %x\n",c);
    char buff[2];
    
    /* First byte of the buffer tells the display controller that the following byte 
        is a data byte */
    buff[0] = OLED_CONTROL_BYTE_DATA_BYTE;
    buff[1] = c;

    /* Write the buffer to display controller */
    Display_I2C.write(OLED_I2C_ADDRESS, buff, sizeof(buff), false);
}


/*******************************************************************************
* Function Name: void I2C_WriteM01(unsigned char * pData, int NumBytes) 
********************************************************************************
*
* Summary: This function writes multiple data bytes to the display controller with A0 = 1
*
* Parameters:
*  unsigned char *pData - Pointer to the buffer that has data
*  int numBytes - Number of bytes to be written to the display controller
*
* Return:
*  None
*
*******************************************************************************/
void I2C_WriteDataStream(unsigned char * pData, int numBytes) 
{   
//    printf("WriteDataStream %d\n",numBytes);
    int i;
    int rval;


    char *data = (char *)malloc(numBytes + 1);
    memcpy(data+1,pData,numBytes);
    data[0] = OLED_CONTROL_BYTE_DATA_STREAM;
    Display_I2C.write(OLED_I2C_ADDRESS,data,numBytes+1,false);
    free(data);
    
}

/*******************************************************************************
* Function Name: void I2C_ReadM01(unsigned char * pData, int numBytes)  
********************************************************************************
*
* Summary: This function reads multiple data bytes from the display controller with A0 = 1
*
* Parameters:
*  unsigned char *pData - Pointer to the destination buffer
*  int numBytes - Number of bytes to be read from the display controller
*
* Return:
*  None
*
*******************************************************************************/
void I2C_ReadM01(unsigned char * pData, int numBytes) 
{
    /* SSD1306 is not readable through i2c. Using cache instead (LCDConf.c, GUIDRV_SPAGE_1C1)*/
}

/*************************** End of file ****************************/
