/******************************************************************************
* File Name: ssd1306.cpp
*
* Version: 1.00
*/

#include "GUI.h"

#include "ssd1306.h"
#include "mbed.h"

/*********************************************************************
*
*       Defines: Configuration
*
**********************************************************************
  Needs to be adapted to custom hardware.
*/
/* I2C port to communicate with the OLED display controller */
I2C Display_I2C(MBED_CONF_SSD1306_OLED_SDA, MBED_CONF_SSD1306_OLED_SCL);

/* I2C slave address, Command and Data byte prefixes for the display controller */
#define OLED_CONTROL_BYTE_CMD_BYTE    (0x80)
#define OLED_CONTROL_BYTE_CMD_STREAM  (0x00)

#define OLED_CONTROL_BYTE_DATA_BYTE   (0xC0)
#define OLED_CONTROL_BYTE_DATA_STREAM (0x40)

/* I2C bus speed */

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
void ssd1306_Init(void) 
{
    Display_I2C.frequency(MBED_CONF_SSD1306_OLED_I2CFREQ);
}

/*******************************************************************************
* Function Name: void ssd1306_Write00(unsigned char c) 
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
void ssd1306_WriteCmd(unsigned char c) 
{
  //  printf("Command = %x\n",c);

    char buff[2];
    
    /* The first byte of the buffer tells the display that the following byte
        is a command */
    buff[0] = OLED_CONTROL_BYTE_CMD_BYTE;
    buff[1] = (char)c;
    
    /* Write the buffer to display controller */
    Display_I2C.write(MBED_CONF_SSD1306_OLED_I2CADDRESS, buff, sizeof(buff), false);
}

void ssd1306_WriteCmdStream(const char *data,int num)
{
    char *buffer = (char *)malloc(num+1);
    memcpy(buffer+1,data,num);
    buffer[0] = 0x00; // Continous stream of commands
    int rval = Display_I2C.write(MBED_CONF_SSD1306_OLED_I2CADDRESS, (const char *)buffer, num+1);
}

/*******************************************************************************
* Function Name: void ssd1306_Write01(unsigned char c) 
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
void ssd1306_WriteData(unsigned char c) 
{
//    printf("Data = %x\n",c);
    char buff[2];
    
    /* First byte of the buffer tells the display controller that the following byte 
        is a data byte */
    buff[0] = OLED_CONTROL_BYTE_DATA_BYTE;
    buff[1] = c;

    /* Write the buffer to display controller */
    Display_I2C.write(MBED_CONF_SSD1306_OLED_I2CADDRESS, buff, sizeof(buff), false);
}


/*******************************************************************************
* Function Name: void ssd1306_WriteM01(unsigned char * pData, int NumBytes) 
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
void ssd1306_WriteDataStream(unsigned char * pData, int numBytes) 
{   
//    printf("WriteDataStream %d\n",numBytes);
    int i;
    int rval;


    char *data = (char *)malloc(numBytes + 1);
    memcpy(data+1,pData,numBytes);
    data[0] = OLED_CONTROL_BYTE_DATA_STREAM;
    Display_I2C.write(MBED_CONF_SSD1306_OLED_I2CADDRESS,data,numBytes+1,false);
    free(data);
    
}

/*******************************************************************************
* Function Name: void ssd1306_ReadM01(unsigned char * pData, int numBytes)  
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
void ssd1306_ReadM01(unsigned char * pData, int numBytes) 
{
    /* SSD1306 is not readable through i2c. Using cache instead (LCDConf.c, GUIDRV_SPAGE_1C1)*/
}

/*************************** End of file ****************************/
