/**
  ******************************************************************************
  * @file           : oled_c_cmc.C
  * @brief          : OLED C commands
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 Michael Kirchhofer - FH Technikum Wien
  *
  * This file contains all possible command codes for the Oled C Click
  ******************************************************************************
*/

#define OLED_CMD_SETCOLUMN       0x15
#define OLED_CMD_SETROW          0x75
#define OLED_CMD_WRITERAM        0x5C
#define OLED_CMD_READRAM         0x5D
#define OLED_CMD_SETREMAP        0xA0
#define OLED_CMD_STARTLINE       0xA1
#define OLED_CMD_DISPLAYOFFSET   0xA2
#define OLED_CMD_DISPLAYALLOFF   0xA4
#define OLED_CMD_DISPLAYALLON    0xA5
#define OLED_CMD_NORMALDISPLAY   0xA6
#define OLED_CMD_INVERTDISPLAY   0xA7
#define OLED_CMD_FUNCTIONSELECT  0xAB
#define OLED_CMD_DISPLAYOFF      0xAE
#define OLED_CMD_DISPLAYON       0xAF
#define OLED_CMD_PRECHARGE       0xB1
#define OLED_CMD_DISPLAYENHANCE  0xB2
#define OLED_CMD_CLOCKDIV        0xB3
#define OLED_CMD_SETVSL          0xB4
#define OLED_CMD_SETGPIO         0xB5
#define OLED_CMD_PRECHARGE2      0xB6
#define OLED_CMD_SETGRAY         0xB8
#define OLED_CMD_USELUT          0xB9
#define OLED_CMD_PRECHARGELEVEL  0xBB
#define OLED_CMD_VCOMH           0xBE
#define OLED_CMD_CONTRASTABC     0xC1
#define OLED_CMD_CONTRASTMASTER  0xC7
#define OLED_CMD_MUXRATIO        0xCA
#define OLED_CMD_COMMANDLOCK     0xFD
#define OLED_CMD_HORIZSCROLL     0x96
#define OLED_CMD_STOPSCROLL      0x9E
#define OLED_CMD_STARTSCROLL     0x9F
#define OLED_DISP_WIDTH          0x60
#define OLED_DISP_HEIGHT         0x60
#define OLED_ROW_OFF             0x00
#define OLED_COL_OFF             0x10
#define OLED_DEFAULT_MUX_RATIO  95
#define OLED_DEFAULT_START_LINE 0x80
#define OLED_DEFAULT_OFFSET     0x20
#define OLED_DEFAULT_OLED_LOCK     0x12
#define OLED_DEFAULT_CMD_LOCK      0xB1
#define OLED_DEFAULT_DIVSET        0xF1
#define OLED_DEFAULT_PRECHARGE     0x32
#define OLED_DEFAULT_VCOMH         0x05
#define OLED_DEFAULT_MASTER_CONT   0xCF
#define OLED_DEFAULT_PRECHARGE_2   0x01
