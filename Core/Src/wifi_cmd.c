/**
  ******************************************************************************
  * @file           : wifi_cmd.c
  * @brief          : Wifi function implementation
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 Michael Kirchhofer  / Andreas Stigler - FH Technikum Wien
  *
  * This file contains the ESP32 Wroom init sequences
  *
  ******************************************************************************
  */

#include "wifi_cmd.h"

/**
 * @brief  Function initializes the mcu as access point
 * @param  argument: UART_HandleTypeDef *huart, pointer to huart interface
 * @retval void
 */
void wifi_init_AP(UART_HandleTypeDef *huart){

	//----------------------------------Initialise WIFI AP--------------------------------------------------
	//Reset
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_RST_CMD, strlen(WIFI_RST_CMD));
	osDelay(WIFI_CMD_DELAY_LONG);
	//Set Module Hostname
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_SET_HOSTNAME_AP, strlen(WIFI_SET_HOSTNAME_AP));
    osDelay(WIFI_CMD_DELAY_LONG);
    //Set AP-Base Station Mode
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_SET_MODE_AP, strlen(WIFI_SET_MODE_AP));
	osDelay(WIFI_CMD_DELAY_LONG);
	//Set Multiple Connections to true
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_SET_MULT_CON, strlen(WIFI_SET_MULT_CON));
	osDelay(WIFI_CMD_DELAY_LONG);
	//Create Server
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_CREATE_SVR, strlen(WIFI_CREATE_SVR));
	osDelay(WIFI_CMD_DELAY_LONG);
	//Configure Server Access Point
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_CONF_SAP, strlen(WIFI_CONF_SAP));
	osDelay(WIFI_CMD_DELAY_LONG);
	//Query IP from Server Access Point
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_GET_APIP, strlen(WIFI_GET_APIP));
	osDelay(WIFI_CMD_DELAY_LONG);

	//------------------------------------------------------------------------------------------------------

}
/**
 * @brief  Function initializes the mcu as client
 * @param  UART_HandleTypeDef *huart, pointer to huart interface
 * @retval void
 */
void wifi_init_CLT(UART_HandleTypeDef *huart){

	//----------------------------------Initialise WIFI Client Mode-----------------------------------------
	//Reset
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_RST_CMD, strlen(WIFI_RST_CMD));
	osDelay(WIFI_CMD_DELAY_LONG);
	//Set Module Hostname
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_SET_HOSTNAME_CLT, strlen(WIFI_SET_HOSTNAME_CLT));
    osDelay(WIFI_CMD_DELAY_LONG);
    //Set Client Mode
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_SET_MODE_CLT, strlen(WIFI_SET_MODE_CLT));
	osDelay(WIFI_CMD_DELAY_LONG);
	//Connect to AP
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_SET_CON_PARAM, strlen(WIFI_SET_CON_PARAM));
	osDelay(WIFI_CMD_DELAY_LONG*5);
	//Query IP from Module
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_GET_IP, strlen(WIFI_GET_IP));
	osDelay(WIFI_CMD_DELAY_LONG);
	//Start TCP Connection
	HAL_UART_Transmit_DMA(huart, (uint8_t *)WIFI_SET_TCP_CON, strlen(WIFI_SET_TCP_CON));
	osDelay(WIFI_CMD_DELAY_LONG);
	//------------------------------------------------------------------------------------------------------

}
