/**
  ******************************************************************************
  * @file           : oled_ssd1351.C
  * @brief          : OLED C Click functions
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 Michael Kirchhofer - FH Technikum Wien
  *
  * This file contains the various functions used to control and update the
  * OLED C Click.
  *
  ******************************************************************************
*/

#include <oled_ssd1351.h>

static void SSD1351_Reset();
static void SSD1351_Select();
static void SSD1351_WriteData(uint8_t* buff, size_t buff_size);
static void SSD1351_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void SSD1351_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);


/**
  * @brief  Re-Initialization of the OLED module
  * @param  none
  * @retval none
  */
void SSD1351_Reinit(void) {
    /*Unselects the OLED module*/
    SSD1351_Unselect();
    /*Initializes the OLED module*/
    SSD1351_Init();
}

/**
  * @brief  Initialization of the OLED module with default values
  * @param  none
  * @retval none
  */
void SSD1351_Init() {
    /*Selects the OLED / Pulls CS pin to logic 0*/
	SSD1351_Select();
	/*Resets the OLED module to default / Pulls reset pin to 0*/
    SSD1351_Reset();
    /*command list is based on https://github.com/adafruit/Adafruit-SSD1351-library*/
    SSD1351_WriteCommand(0xFD); /*COMMANDLOCK*/
    {
        uint8_t data[] = { 0x12 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xFD); /*COMMANDLOCK*/
    {
        uint8_t data[] = { 0xB1 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xAE); /*DISPLAYOFF*/
    SSD1351_WriteCommand(0xB3); /*CLOCKDIV*/
    SSD1351_WriteCommand(0xF1); /*7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)*/
    SSD1351_WriteCommand(0xCA); /*MUXRATIO*/
    {
        uint8_t data[] = { 0x7F }; /*127*/
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xA0); /*SETREMAP*/
    {
        uint8_t data[] = { 0x72 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0x15); /*SETCOLUMN*/
    {
        uint8_t data[] = { 0x20, 0x60 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0x75); /*SETROW*/
    {
        uint8_t data[] = { 0x20, 0x60 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xA1); /*STARTLINE*/
    {
        uint8_t data[] = { 0x80 }; /*96 if display height == 96*/
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xA2); /*DISPLAYOFFSET*/
    {
        uint8_t data[] = { 0x00 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xB5); /*SETGPIO*/
    {
        uint8_t data[] = { 0x00 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xAB); /*FUNCTIONSELECT*/
    {
        uint8_t data[] = { 0x01 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xB1); /*PRECHARGE*/
    {
        uint8_t data[] = { 0x32 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xBE); /*VCOMH*/
    {
        uint8_t data[] = { 0x05 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xA6); /*NORMALDISPLAY (don't invert)*/
    SSD1351_WriteCommand(0xC1); /*CONTRASTABC*/
    {
        uint8_t data[] = { 0xC8, 0x80, 0xC8 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xC7); /*CONTRASTMASTER*/
    {
        uint8_t data[] = { 0x0F };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xB4); /*SETVSL*/
    {
        uint8_t data[] = { 0xA0, 0xB5, 0x55 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xB6); /*PRECHARGE2*/
    {
        uint8_t data[] = { 0x01 };
        SSD1351_WriteData(data, sizeof(data));
    }
    SSD1351_WriteCommand(0xAF); /*DISPLAYON*/
    /*Unselects the OLED module*/
    SSD1351_Unselect();
}

/**
  * @brief  Function to unselect the OLED module
  * @param  none
  * @retval none
  */
void SSD1351_Unselect() {
    HAL_GPIO_WritePin(SSD1351_CS_GPIO_Port, SSD1351_CS_Pin, GPIO_PIN_SET);
}

/**
  * @brief  Initializes the IDs for several MsgQueues needed for this program.
  * 				The first the for sending data to the monitoring task and
  * 				the other 3 to send the times data back.
  * @param  x: Information of the x position of the pixel
  * @param  y: Information of the y position of the pixel
  * @param  color: Information which color the set pixel should have in 16 bit color format
  * @retval none
  */
void SSD1351_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    /*Checks the input for validity*/
	if((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT))
        return;
    /*Selects the OLED module*/
    SSD1351_Select();
    /*Set the size of the window of the OLD module in which data should be addressed*/
    SSD1351_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    SSD1351_WriteData(data, sizeof(data));
    /*Unselects the OLED module*/
    SSD1351_Unselect();
}

/**
  * @brief  Function to write entire strings to the display starting at a predefined x and y location
  * 				until the x and y limit of the display is reached.
  * @param  x: Information of the x position of the pixel
  * @param  y: Information of the y position of the pixel
  * @param  str: A pointer to the string which should be written to the buffer of the OLED module
  * @param  font: Information of font which should be used
  * @param  color: Information which color the set pixel should have in 16 bit color format
  * @param  bgcolor: Information which color the background should have in 16 bit color format
  * @retval none
  */
void SSD1351_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    /*Selects the OLED module*/
	SSD1351_Select();
    /*As long as the end of the string isn't reached, it is checked whether the actual character
     * reaches the size limit of x axis and kumbs down a row until it reaches the very last
     * possible character on display*/
    while(*str) {
        if(x + font.width >= SSD1351_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= SSD1351_HEIGHT) {
            	/*If it reaches the last possible character in x and y axis, it breaks the loop
            	 * and stops writing to the buffer of the OLED*/
                break;
            }
            if(*str == ' ') {
                /*kip spaces in the beginning of the new line*/
                str++;
                continue;
            }
        }
        /*Writes a character to the buffer of the OLED display*/
        SSD1351_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }
    /*Unselects the OLED module*/
    SSD1351_Unselect();
}

/**
  * @brief  Draws a rectangle to the buffer of the OLED module starting from x and y location for
  * 				the width and height as well as the color which was given with.
  * @param  x: Information of the x position of the pixel
  * @param  y: Information of the y position of the pixel
  * @param  w: Information about the width of the rectangle
  * @param  h: Information about the height of the rectangle
  * @param  color: Information which color the set pixel should have in 16 bit color format
  * @retval none
  */
void SSD1351_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    /*Checks the input for validity*/
    if((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT)) return;
    if((x + w - 1) >= SSD1351_WIDTH) w = SSD1351_WIDTH - x;
    if((y + h - 1) >= SSD1351_HEIGHT) h = SSD1351_HEIGHT - y;
    /*Selects the OLED module*/
    SSD1351_Select();
    /*Set the size of the window of the OLD module in which data should be addressed*/
    SSD1351_SetAddressWindow(x, y, x+w-1, y+h-1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    HAL_GPIO_WritePin(SSD1351_DC_GPIO_Port, SSD1351_DC_Pin, GPIO_PIN_SET);
    /*Fills the buffer of the OLED module with the information of the rectangle in the given size*/
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            HAL_SPI_Transmit(&SSD1351_SPI_PORT, data, sizeof(data), HAL_MAX_DELAY);
        }
    }
    /*Unselects the OLED module*/
    SSD1351_Unselect();
}

/**
  * @brief  Fills the entire buffer of the OLED module with a defined 16 bit color for the complete size
  * 				of 128x128
  * @param  color: Information which color the set pixel should have in 16 bit color format
  * @retval none
  */
void SSD1351_FillScreen(uint16_t color) {
    SSD1351_FillRectangle(0, 0, SSD1351_WIDTH, SSD1351_HEIGHT, color);
}

/**
  * @brief  Function to draw a image to the OLED display in the proportions which were sent with
  * 			starting at location x and y.
  * @param  x: x location where the image should start to be drawn
  * @param  y: y location where the image should start to be drawn
  * @param  w: Width of the image which should be drawn
  * @param  h: Height of the image which should be drawn
  * @param  data: Pointer to the data of the image which should be drawn to the OLED display
  * @retval none
  */
void SSD1351_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    /*Checks the input for validity*/
	if((x >= SSD1351_WIDTH) || (y >= SSD1351_HEIGHT)) return;
    if((x + w - 1) >= SSD1351_WIDTH) return;
    if((y + h - 1) >= SSD1351_HEIGHT) return;
    /*Selects the OLED module*/
    SSD1351_Select();
    /*Set the size of the window of the OLD module in which data should be addressed*/
    SSD1351_SetAddressWindow(x, y, x+w-1, y+h-1);
    /*Writes the data of the image to the buffer of the OLED module*/
    SSD1351_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
    /*Unselects the OLED module*/
    SSD1351_Unselect();
}

/**
  * @brief  Function that inverts the colors of the display either when set to 1
  * 				or not with 0
  * @param  invert: 1 inverts display
  * 				0 sets normal display
  * @retval none
  */
void SSD1351_InvertColors(bool invert) {
    /*Selects the OLED module*/
	SSD1351_Select();
	/*Either inverts or don't inverts the OLED display*/
    SSD1351_WriteCommand(invert ? 0xA7 /* INVERTDISPLAY */ : 0xA6 /* NORMALDISPLAY */);
    /*Unselects the OLED module*/
    SSD1351_Unselect();
}


/**
  * @brief  Function that unselects the OLED module
  * @param  none
  * @retval none
  */
static void SSD1351_Select() {
    HAL_GPIO_WritePin(SSD1351_CS_GPIO_Port, SSD1351_CS_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  Function to simplify writing characters to the buffer of the OLED module.
  * @param  x: x position where the character should start to be drawn
  * @param  y: y position where the character should start to be drawn
  * @param  ch: Character that should be drawn
  * @param  font: Used font which should be used
  * @param  color: Information about the color which should be used for the pixels of the character
  * @param  bgcolor: Information about the background color
  * @retval none
  */
static void SSD1351_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;
    /*Set the size of the window of the OLD module in which data should be addressed*/
    SSD1351_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);
    /*Iterates through the horizontal and vertical size of the character in dependency of the font size
     * and writes the information to the buffer of the OLED module*/
    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                SSD1351_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                SSD1351_WriteData(data, sizeof(data));
            }
        }
    }
}

/**
  * @brief  Function to reset the OLED module
  * @param  none
  * @retval none
  */
static void SSD1351_Reset() {
    HAL_GPIO_WritePin(SSD1351_RES_GPIO_Port, SSD1351_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(SSD1351_RES_GPIO_Port, SSD1351_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(SSD1351_RES_GPIO_Port, SSD1351_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(500);
}

/**
  * @brief  Function that writs commands to the OLED module
  * @param  cmd: Is the information of the command which should be handled by
  * 				the OLED module
  * @retval none
  */
void SSD1351_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(SSD1351_DC_GPIO_Port, SSD1351_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&SSD1351_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
}

/**
  * @brief  Function that writes data to the OLED module in chunks
  * @param  buff: data which should be written to the OLED module
  * @param  buff_size: size of the buffer which should be written to the OLED module
  * @retval none
  */
static void SSD1351_WriteData(uint8_t* buff, size_t buff_size) {
	HAL_GPIO_WritePin(SSD1351_DC_GPIO_Port, SSD1351_DC_Pin, GPIO_PIN_SET);
    /*Split data in small chunks because HAL can't send more then 64K at once*/
    while(buff_size > 0) {
        uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
        HAL_SPI_Transmit(&SSD1351_SPI_PORT, buff, chunk_size, HAL_MAX_DELAY);
        buff += chunk_size;
        buff_size -= chunk_size;
    }
}

/**
  * @brief  Function that sets the address window for the OLED module in which data should be
  * 			written starting from the starting point x0/y0 ending with x1/y1.
  * @param  x0: x location of the starting point
  * @param  y0: y location of the starting point
  * @param  x1: x location of the ending point
  * @param  y1: y location of the ending point
  * @retval none
  */
static void SSD1351_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    /*Column address set*/
    SSD1351_WriteCommand(0x15); /*SETCOLUMN*/
    {
        uint8_t data[] = { x0 & 0xFF, x1 & 0xFF };
        SSD1351_WriteData(data, sizeof(data));
    }
    /*Row address set*/
    SSD1351_WriteCommand(0x75); /*SETROW*/
    {
        uint8_t data[] = { y0 & 0xFF, y1 & 0xFF };
        SSD1351_WriteData(data, sizeof(data));
    }
    /*Write to RAM*/
    SSD1351_WriteCommand(0x5C); /*WRITERAM*/
}
