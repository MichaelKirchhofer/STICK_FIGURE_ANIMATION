/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 Michael Kirchhofer - FH Technikum Wien
 *
 * This program utilises the Mikroe OLED C-, as well as the ESP32Wroom Click
 * to display a short animation on the OLED Module. The User can use a Axis-
 * Input on the partner MCU to invert the colour of the LED Display, as well as
 * the animation speed.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wifi_cmd.h"
#include "oled_c_cmd.h"
#include "oled_c_image.h"
#include "oled_ssd1351.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

/* Definitions for OledDisplayTask */
osThreadId_t OledDisplayTaskHandle;
uint32_t OledDisplayTaskBuffer[ 256 ];
osStaticThreadDef_t OledDisplayTaskControlBlock;
const osThreadAttr_t OledDisplayTask_attributes = {
  .name = "OledDisplayTask",
  .cb_mem = &OledDisplayTaskControlBlock,
  .cb_size = sizeof(OledDisplayTaskControlBlock),
  .stack_mem = &OledDisplayTaskBuffer[0],
  .stack_size = sizeof(OledDisplayTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for WifiConnectTask */
osThreadId_t WifiConnectTaskHandle;
uint32_t WifiConnectTaskBuffer[ 512 ];
osStaticThreadDef_t WifiConnectTaskControlBlock;
const osThreadAttr_t WifiConnectTask_attributes = {
  .name = "WifiConnectTask",
  .cb_mem = &WifiConnectTaskControlBlock,
  .cb_size = sizeof(WifiConnectTaskControlBlock),
  .stack_mem = &WifiConnectTaskBuffer[0],
  .stack_size = sizeof(WifiConnectTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MsgHandleTask */
osThreadId_t MsgHandleTaskHandle;
uint32_t MsgHandleTaskBuffer[ 512 ];
osStaticThreadDef_t MsgHandleTaskControlBlock;
const osThreadAttr_t MsgHandleTask_attributes = {
  .name = "MsgHandleTask",
  .cb_mem = &MsgHandleTaskControlBlock,
  .cb_size = sizeof(MsgHandleTaskControlBlock),
  .stack_mem = &MsgHandleTaskBuffer[0],
  .stack_size = sizeof(MsgHandleTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for m_oled_anim_spd */
osMutexId_t m_oled_anim_spdHandle;
osStaticMutexDef_t m_oled_anim_spdControlBlock;
const osMutexAttr_t m_oled_anim_spd_attributes = {
  .name = "m_oled_anim_spd",
  .cb_mem = &m_oled_anim_spdControlBlock,
  .cb_size = sizeof(m_oled_anim_spdControlBlock),
};
/* Definitions for m_oled_color_shft */
osMutexId_t m_oled_color_shftHandle;
osStaticMutexDef_t m_oled_color_shftControlBlock;
const osMutexAttr_t m_oled_color_shft_attributes = {
  .name = "m_oled_color_shft",
  .cb_mem = &m_oled_color_shftControlBlock,
  .cb_size = sizeof(m_oled_color_shftControlBlock),
};
/* Definitions for m_wifi_transmit */
osMutexId_t m_wifi_transmitHandle;
osStaticMutexDef_t m_wifi_transmitControlBlock;
const osMutexAttr_t m_wifi_transmit_attributes = {
  .name = "m_wifi_transmit",
  .cb_mem = &m_wifi_transmitControlBlock,
  .cb_size = sizeof(m_wifi_transmitControlBlock),
};
/* Definitions for m_wifi_received */
osMutexId_t m_wifi_receivedHandle;
osStaticMutexDef_t m_wifi_receivedControlBlock;
const osMutexAttr_t m_wifi_received_attributes = {
  .name = "m_wifi_received",
  .cb_mem = &m_wifi_receivedControlBlock,
  .cb_size = sizeof(m_wifi_receivedControlBlock),
};
/* USER CODE BEGIN PV */

static uint32_t g_oled_anim_spd = OLED_ANIM_SPD_DFLT;
static bool g_oled_color_shft = false;
static bool g_wifi_transmit = false;
static bool g_wifi_received = false;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI1_Init(void);
void StartOledDisplayTask(void *argument);
void StartWifiConnectTask(void *argument);
void StartMsgHandleTask(void *argument);

/* USER CODE BEGIN PFP */
uint16_t __io_putchar(uint16_t ch);
uint16_t _write(uint16_t file, char *buf, uint16_t len);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  SSD1351_Init();

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of m_oled_anim_spd */
  m_oled_anim_spdHandle = osMutexNew(&m_oled_anim_spd_attributes);

  /* creation of m_oled_color_shft */
  m_oled_color_shftHandle = osMutexNew(&m_oled_color_shft_attributes);

  /* creation of m_wifi_transmit */
  m_wifi_transmitHandle = osMutexNew(&m_wifi_transmit_attributes);

  /* creation of m_wifi_received */
  m_wifi_receivedHandle = osMutexNew(&m_wifi_received_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of OledDisplayTask */
  OledDisplayTaskHandle = osThreadNew(StartOledDisplayTask, NULL, &OledDisplayTask_attributes);

  /* creation of WifiConnectTask */
  WifiConnectTaskHandle = osThreadNew(StartWifiConnectTask, NULL, &WifiConnectTask_attributes);

  /* creation of MsgHandleTask */
  MsgHandleTaskHandle = osThreadNew(StartMsgHandleTask, NULL, &MsgHandleTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OLED_RW_GPIO_Port, OLED_RW_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, OLED_ENABLE_Pin|OLED_RESET_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OLED_DATACOMMAND_GPIO_Port, OLED_DATACOMMAND_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : OLED_RW_Pin OLED_ENABLE_Pin OLED_RESET_Pin */
  GPIO_InitStruct.Pin = OLED_RW_Pin|OLED_ENABLE_Pin|OLED_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI_CS_Pin OLED_DATACOMMAND_Pin */
  GPIO_InitStruct.Pin = SPI_CS_Pin|OLED_DATACOMMAND_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/**
 * @brief  Function sends a character via USART interface in interrupt mode
 * and configures printf to send via USART
 * @param  argument: uint16_t char
 * @retval uint16_t char
 */
uint16_t __io_putchar(uint16_t ch) {
	uint16_t ret;
	while ((ret = HAL_UART_GetState(&huart2)) != HAL_UART_STATE_READY)
		;
	if (ch == '\n') {
		static unsigned char buf2[2] = { '\r', '\n' };
		HAL_UART_Transmit_IT(&huart2, buf2, 2);
	} else {
		static unsigned char buf;
		buf = (char) ch;
		HAL_UART_Transmit_IT(&huart2, &buf, 1);
	}
	return ch;
}

/**
 * @brief  Function splits string into characters for __io_putchar function
 * @param  argument: uint16_t file, char *buf, uint16_t len
 * @retval uint16_t length;
 */
uint16_t _write(uint16_t file, char *buf, uint16_t len) {
	uint16_t count = len;
	while (count-- > 0) {
		__io_putchar(*buf++);
	}
	return len;
}

/**
 * @brief  Function implements the receiving of incoming wifi messages
 * @param  argument: USART Handle, Size of Message
 * @retval none
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {

	/* Receive event handler
	 * Mutexes and Semaphores are blocked by this callback
	 * Leave callback as soon as possible to enable mcu to  handle other tasks
	 */

	g_wifi_received = true;

}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartOledDisplayTask */
/**
 * @brief  Function implementing the OLED display handler, which handles
 * 		Wifi sent commands via the changing of the animation speed and
 * 		a possible inversion of the image colour if the corresponding
 * 		command has been received
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartOledDisplayTask */
void StartOledDisplayTask(void *argument)
{
  /* USER CODE BEGIN 5 */
int swt = 0;
uint32_t anim_spd = OLED_ANIM_SPD_DFLT;
bool color_shft = false;
SSD1351_FillScreen(0xFFFF);
printf("OLED initialisation complete !\n");
osDelay(10);
/* Infinite loop */
for (;;) {
//----------------------------------Accessing Mutexes--------------------------------------------------
	if (osMutexAcquire(m_oled_anim_spdHandle, 100) != osOK) {
		printf("Error in acquiring mutex m_oled_anim_spdHandle !\n");
	} else {
		// Set anim_spd to potentially changed animation speed
		anim_spd = g_oled_anim_spd;

		if (osMutexRelease(m_oled_anim_spdHandle) != osOK) {
			printf("Error in releasing mutex m_oled_anim_spdHandle !\n");
		}
	}

	if (osMutexAcquire(m_oled_color_shftHandle, 100) != osOK) {
		printf("Error in acquiring mutex m_oled_color_shftHandle !\n");
	} else {
		// Set if color_shift flag has been sent
		color_shft = g_oled_color_shft;

		if (osMutexRelease(m_oled_color_shftHandle) != osOK) {
			printf("Error in releasing mutex m_oled_color_shftHandle !\n");
		}
	}
//---------------------------------------------------------------------------------------------------

	//Invert display colors
	SSD1351_InvertColors(color_shft);

	//Cycle frames to create animation
	switch (swt) {
	case 0:
		SSD1351_DrawImage(OLED_X_POS, OLED_Y_POS, OLED_IMG_DIM,
		OLED_IMG_DIM, stick_bmp[swt]);
		swt++;
		break;
	case 1:
		SSD1351_DrawImage(OLED_X_POS, OLED_Y_POS, OLED_IMG_DIM,
		OLED_IMG_DIM, stick_bmp[swt]);
		swt++;
		break;
	case 2:
		SSD1351_DrawImage(OLED_X_POS, OLED_Y_POS, OLED_IMG_DIM,
		OLED_IMG_DIM, stick_bmp[swt]);
		swt++;
		break;
	case 3:
		SSD1351_DrawImage(OLED_X_POS, OLED_Y_POS, OLED_IMG_DIM,
		OLED_IMG_DIM, stick_bmp[swt]);
		swt++;
		break;
	case 4:
		SSD1351_DrawImage(OLED_X_POS, OLED_Y_POS, OLED_IMG_DIM,
		OLED_IMG_DIM, stick_bmp[swt]);
		swt++;
		break;
	case 5:
		SSD1351_DrawImage(OLED_X_POS, OLED_Y_POS, OLED_IMG_DIM,
		OLED_IMG_DIM, stick_bmp[swt]);
		swt = 0;
		break;
	case 99:
		if(anim_spd != OLED_ANIM_SPD_DFLT){
			swt = 0;
		}
		break;
	default:
		break;
	}

	osDelay(anim_spd);
}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartWifiConnectTask */
/**
 * @brief Function implementing the wifi communication with another
 * 		 MCU. After the init is complete, the Module starts to
 * 		 query for receive / transmit requests
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartWifiConnectTask */
void StartWifiConnectTask(void *argument)
{
  /* USER CODE BEGIN StartWifiConnectTask */

uint8_t wifi_send_buf[WIFI_TX_BUF_SIZE];
memset(wifi_send_buf, '\0', sizeof(wifi_send_buf));

bool wifi_transmit = false;

//Init Wifi AP
wifi_init_AP(&huart1);

printf("Wifi AP initialisation complete!\n");
/* Infinite loop */
for (;;) {
	// Set local transmit flag for operation / set global flag to false
	if (osMutexAcquire(m_wifi_transmitHandle, 100) != osOK) {
		printf("Error in acquiring mutex m_wifi_transmitHandle !\n");
	} else {

		wifi_transmit = g_wifi_transmit;
		g_wifi_transmit = false;

		if (osMutexRelease(m_wifi_transmitHandle) != osOK) {
			printf("Error in releasing mutex m_wifi_transmitHandle !\n");
		}
	}

	//Send only when flag has been set / Sending is only implemented in one direction, so this flag will always be false
	if (wifi_transmit) {
		sprintf((char*) wifi_send_buf, "%s0,%d\r\n", WIFI_SND_MSG,WIFI_RX_BUF_SIZE);
		HAL_UART_Transmit_DMA(&huart1, wifi_send_buf,sizeof((char*) wifi_send_buf));
		wifi_transmit = false;
		osDelay(WIFI_CMD_DELAY_LONG);
	} else {
		//printf("Ready to receive Data!\n");
		osDelay(WIFI_CMD_DELAY_LONG);
	}

}
  /* USER CODE END StartWifiConnectTask */
}

/* USER CODE BEGIN Header_StartMsgHandleTask */
/**
 * @brief Function that handles the received wifi input and sets the animation handler parameters
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartMsgHandleTask */
void StartMsgHandleTask(void *argument)
{
  /* USER CODE BEGIN StartMsgHandleTask */


//Wifi receive buffer
uint8_t wifi_msg_buf[WIFI_RX_BUF_SIZE];
memset(wifi_msg_buf,'X',sizeof(wifi_msg_buf));


//Input parameters that get handed down to the animation handler
bool color_shft;
int anim_spd = 0;

//Flags that are set if a corresponding input has been received
bool update_parameters = false;
bool wifi_received = false;

//Helper variables
char i_anim_spd[4];
char *anim_msg;
char *ip_pkg;
char *clt_connect_msg;
//Start receiving uart input over DMA / disable halftransmit interrupt
HAL_UARTEx_ReceiveToIdle_DMA(&huart1, wifi_msg_buf, WIFI_RX_BUF_SIZE);
__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);

/* Infinite loop */
for (;;) {
	if (osMutexAcquire(m_wifi_receivedHandle, 100) != osOK) {
		printf("Error in acquiring binary semaphore m_wifi_receivedHandle!\n");
	} else {
		wifi_received = g_wifi_received;
		g_wifi_received = false;

		if (osMutexRelease(m_wifi_receivedHandle) != osOK) {
			printf(
					"Error in releasing binary semaphore m_wifi_receivedHandle!\n");
		}

	}

	if (wifi_received) {
		wifi_received = false;

		//Search for parameters
		anim_msg = strchr((char*) wifi_msg_buf, '#');
		//Search for IP data package
		ip_pkg = strstr((char *)wifi_msg_buf,"+IPD");
		//Search for connection occured
		clt_connect_msg = strstr((char *)wifi_msg_buf,"CONNECT");

		if(clt_connect_msg!=NULL){
			printf("Client connected to TCP Port 333\n");
		}

		if (anim_msg != NULL) {
			printf("IP Package: %.35s \nLength: %d\n",ip_pkg,strlen(ip_pkg));
			printf("Animation Parameter: %.10s\n",anim_msg);
			if (anim_msg[1] == '-' && anim_msg[2] == 'i' && anim_msg[4] == '-' && anim_msg[5] == 's') {

				//Invert Flag
				switch (anim_msg[3]) {
				case '0':
					color_shft = false;
					break;
				case '1':
					color_shft = true;
					break;
				default:
					color_shft = false;
					break;
				}

				//Animation Speed parameter
				i_anim_spd[0] = anim_msg[6];
				i_anim_spd[1] = anim_msg[7];
				i_anim_spd[2] = anim_msg[8];
				i_anim_spd[3] = anim_msg[9];

				anim_spd = atoi(i_anim_spd);
				//If incorrect value has been send, remain at default value
				if (anim_spd <= 1000 && anim_spd > 5000) {
					anim_spd = OLED_ANIM_SPD_DFLT;
				}

				update_parameters = true;
			}
		}

		//ONLY change run parameters if input has been received
		if (update_parameters) {
			update_parameters = false;
			if (osMutexAcquire(m_oled_anim_spdHandle, 100) != osOK) {
				printf("Error in acquiring mutex m_oled_anim_spdHandle!\n");
			} else {

				g_oled_anim_spd = (uint32_t) anim_spd;

				if (osMutexRelease(m_oled_anim_spdHandle) != osOK) {
					printf("Error in releasing mutex m_oled_anim_spdHandle!\n");
				}
			}

			if (osMutexAcquire(m_oled_color_shftHandle, 100) != osOK) {
				printf("Error in acquiring mutex m_oled_color_shftHandle!\n");
			} else {

				g_oled_color_shft = color_shft;

				if (osMutexRelease(m_oled_color_shftHandle) != osOK) {
					printf(
							"Error in releasing mutex m_oled_color_shftHandle!\n");
				}
			}
		}

		memset(wifi_msg_buf,'X',sizeof(wifi_msg_buf));
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, wifi_msg_buf,WIFI_RX_BUF_SIZE);
		__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
	}
	osDelay(WIFI_RCV_DELAY);
}

  /* USER CODE END StartMsgHandleTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
/* User can add his own implementation to report the HAL error return state */

TickType_t ticks;

ticks = xTaskGetTickCount();
__disable_irq();
while (1) {
	printf("Runtime Error occurred after %d !\n Please press RST Button !\n",
			(int) ticks);

}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	    /* User can add his own implementation to report the file name and line number,
	       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
