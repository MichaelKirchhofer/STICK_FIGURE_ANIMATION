/**
  ******************************************************************************
  * @file           : wifi_cmd.h
  * @brief          : Clickshield Wifi Commands / Function headers
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 Michael Kirchhofer / Andreas Stigler - FH Technikum Wien
  *
  * This file defines the ESP32 Wroom Wifi Commands, as well as the function
  * headers for the init sequences.
  *
  ******************************************************************************
  */

#ifndef INC_WIFI_CMD_H_
#define INC_WIFI_CMD_H_

#include "main.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#define WIFI_CMD_DELAY_SHORT 2000
#define WIFI_CMD_DELAY_LONG 5000
#define WIFI_RCV_DELAY 1000
#define WIFI_SEND_DELAY 1500
#define WIFI_RX_BUF_SIZE 256
#define WIFI_TX_BUF_SIZE 256


//----------------------------------General Commands--------------------------------------------------------------------
/*Send Data over Module 4 --> length of message has to be spliced on to message string
for example a message with a length of 5 equals to the following cmd:
strcpy((char*)wificmd,WIFI_SND_MSG+"5\r\n"); */

static const char WIFI_SND_MSG [] = "AT+CIPSEND=2";

/*Configure Module to Receive is not necessary, as the receive passthrough to the MCU
  is enabled per default with header "+IPD"

  Example Package sent by the Module:
  +IPD,0,100 -> Incoming Package on Connection 0 with length of 100 Byte

  To receive the Data, the following command has to be used:
  (the connection ID and the length have to be spliced onto the command)
  for example:
  strcpy((char *)wificmd,WIFI_RCV_MSG+"0,100\r\n");*/

static const char WIFI_RCV_MSG [] = "AT+CIPRECVDATA=";

//Reset Wifi Module
static const char WIFI_RST_CMD [] = "AT+RST\r\n";

//Configure AP Hostname
static const char WIFI_SET_HOSTNAME_AP [] = "AT+CWHOSTNAME=\"stick_AP\"\r\n";

//----------------------------------Client Mode Commands----------------------------------------------------------------
//Set Mode to client
static const char WIFI_SET_MODE_CLT [] = "AT+CWMODE=1\r\n";

//Set Connection Parameters
static const char WIFI_SET_CON_PARAM [] = "AT+CWJAP=\"STICK_AP\",\"1234STICK\"\r\n";

//Query IP after connection to AP
static const char WIFI_GET_IP [] = "AT+CIPSTA?\r\n";

//Initiate TCP Connection
static const char WIFI_SET_TCP_CON [] = "AT+CIPSTART=\"TCP\",\"192.168.4.1\",333";

//Configure Client Hostname
static const char WIFI_SET_HOSTNAME_CLT [] = "AT+CWHOSTNAME=\"stick_CLT\"\r\n";

//-------------------------------------Access Point Commands-------------------------------------------------------------

//Set Mode to Station and SoftAP (Mode = 3)
static const char WIFI_SET_MODE_AP [] = "AT+CWMODE=3\r\n";

//Allow multiple connections
static const char WIFI_SET_MULT_CON [] = "AT+CIPMUX=1\r\n";

//Create AP Server MODE: 1 (CREATE) --> Default Port 333, Default Server Type TCP
static const char WIFI_CREATE_SVR [] = "AT+CIPSERVER=1\r\n";

//Set AP parameter SSID: STICK_AP, PW: 1234STICK, CHANNEL: Channel 5, ENCRYPTION: WPA2_PSK
static const char WIFI_CONF_SAP [] = "AT+CWSAP=\"STICK_AP\",\"1234STICK\",5,3\r\n";

//Query IP Address from Access Point
static const char WIFI_GET_APIP [] = "AT+CIPAP?\r\n";

//Configure ECHO on Module (Echo is Off = E0)
static const char WIFI_SET_ECHO [] = "ATE0\r\n";

//Check if AP connection is ok
static const char WIFI_GET_AP [] = "AT+CWJAP?";


void wifi_init_AP(UART_HandleTypeDef *huart);
void wifi_init_CLT(UART_HandleTypeDef *huart);

#endif /* INC_WIFI_CMD_H_ */
