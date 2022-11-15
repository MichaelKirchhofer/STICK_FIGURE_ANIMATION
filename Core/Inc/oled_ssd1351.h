/**
  ******************************************************************************
  * @file           : oled_ssd1351.C
  * @brief          : OLED C Click header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 Michael Kirchhofer - FH Technikum Wien
  *
  * This file defines the various functions used to control and update the
  * OLED C Click.
  *
  ******************************************************************************
*/

#ifndef INC_OLED_SSD1351_H_
#define INC_OLED_SSD1351_H_

#include <oled_fonts.h>
#include <stdbool.h>
#include "stm32l4xx_hal.h"
#include "main.h"


/*Mandatory defines and declarations fpr the SPI interface*/
#define SSD1351_SPI_PORT hspi1
extern SPI_HandleTypeDef SSD1351_SPI_PORT;

/*Setting the different pins for using the SPI interface*/
#define SSD1351_RES_Pin       OLED_RESET_Pin
#define SSD1351_RES_GPIO_Port GPIOA
#define SSD1351_CS_Pin        SPI_CS_Pin
#define SSD1351_CS_GPIO_Port  GPIOB
#define SSD1351_DC_Pin        OLED_DATACOMMAND_Pin
#define SSD1351_DC_GPIO_Port  GPIOB
#define SSD1351_RW_Pin        OLED_RW_Pin
#define SSD1351_RW_GPIO_Port  GPIOA
#define SSD1351_E_Pin         OLED_ENABLE_Pin
#define SSD1351_E_GPIO_Port   GPIOA


/*Default dimensions of the OLED display, only 95x95 are usable though*/
#define SSD1351_WIDTH  128
#define SSD1351_HEIGHT 128

/*Definitions of common colors using ssd1351*/
#define	SSD1351_BLACK   0x0000
#define	SSD1351_BLUE    0x001F
#define	SSD1351_RED     0xF800
#define	SSD1351_GREEN   0x07E0
#define SSD1351_CYAN    0x07FF
#define SSD1351_MAGENTA 0xF81F
#define SSD1351_YELLOW  0xFFE0
#define SSD1351_WHITE   0xFFFF
#define SSD1351_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

/*Functions which simplify some complex functionality writing data in the OLED display buffer*/
void SSD1351_Init(void);
void SSD1351_Reinit(void);
void SSD1351_WriteCommand(uint8_t cmd);
void SSD1351_Unselect();
void SSD1351_InvertColors(bool invert);
void SSD1351_FillScreen(uint16_t color);
void SSD1351_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void SSD1351_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void SSD1351_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
void SSD1351_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);

#endif /* INC_OLED_SSD1351_H_ */
