/**
  ******************************************************************************
  * @file           : oled_fonts.h
  * @brief          : OLED display fonts
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 Michael Kirchhofer - FH Technikum Wien
  *
  * This file contains font configuration for the OLED C Click
  *
  *
  ******************************************************************************
  */

#ifndef INC_OLED_FONTS_H_
#define INC_OLED_FONTS_H_

#include <stdint.h>


/*Declaration of a structure to deal with the dimension of the different fonts*/
typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;

/*Define different structures for the font sizes*/
extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;

#endif

