/* USER CODE BEGIN Header */
/**
******************************************************************************
* @file           : main.c
* @brief          : Main program body
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
* All rights reserved.</center></h2>
*
* This software component is licensed by ST under BSD 3-Clause license,
* the "License"; You may not use this file except in compliance with the
* License. You may obtain a copy of the License at:
*                        opensource.org/licenses/BSD-3-Clause
*
******************************************************************************
*/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "usbd_hid.h"
#include "usb_device.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
struct keyboardHID_t {
	uint8_t id;
	uint8_t modifiers;
	uint8_t key1;
	uint8_t key2;
	uint8_t key3;
};

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define USB_KEY_RIGHT 			0x4f
#define USB_KEY_LEFT 			0x50
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

#include  <errno.h>
#include  <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO

int _write(int file, char *data, int len)
{
	if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
	{
		errno = EBADF;
		return -1;
	}

	// arbitrary timeout 1000
	HAL_StatusTypeDef status =
		HAL_UART_Transmit(&huart1, (uint8_t*)data, len, 1000);

	// return # of bytes written - as best we can tell
	return (status == HAL_OK ? len : 0);
}


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int32_t buff[128][2];
#define BUFF_SIZE (sizeof (buff) / sizeof (buff[0]))
int bi = 0;
void add_val (int a, int e) {
	buff[bi++][0] = a;
	buff[bi][1] = e;
	if (bi == sizeof (buff) / sizeof (buff[0]))
		bi = 0;
}

void print_vals () {
	int i;
	for (i=0; i< sizeof (buff) / sizeof (buff[0]); ++i) {
		printf ("%d %ld - %ld\n", i, buff[i][1], buff[i][0]);
	}
}

int32_t diff (int32_t a, int32_t b) {
	if ( (int32_t) a > (int32_t) b )
		return 0xffffffff + a - b;
	return a - b;
}

#define PREC		6
int inrange (int32_t a, int32_t ref) {
	if (a < ref + PREC &&
		a > ref - PREC)
		return true;
	return false;
}

int pulselen (size_t ind) {
	ind %= BUFF_SIZE;
	size_t next_ind = (ind + 1) % BUFF_SIZE;
	return diff (buff[next_ind][0], buff[ind][0]);
}

void buff_set (size_t ind, int val) {
	ind %= BUFF_SIZE;
	buff[ind][0] = val;
}

bool process (int *ret_addr, int *ret_comm) {
	/* TODO: */
	size_t i;
	bool found = false;
	for (i = 0; i < BUFF_SIZE - 2; i++) {
		if (inrange (pulselen (i), 90) &&
			inrange (pulselen (i+1), 45)) {
			/* printf ("Found  start at index: %d\n", i); */
			found = true;
			break;
		}
	}
	if (! found)
		return false;
	char decoded[32] = {0};
	size_t di = 0;
	if (found) {
		HAL_Delay (60); 		/* wait for payload */
		HAL_TIM_IC_Stop (&htim2, TIM_CHANNEL_1);
		HAL_TIM_IC_Stop (&htim2, TIM_CHANNEL_2);

		buff_set (i, 0);
		buff_set (i+1, 0);
		i += 2;
		i %= BUFF_SIZE;
		for (; di < 32; ++i) {
			if (inrange (pulselen (i) , 5) &&
				inrange (pulselen (i+1), 5))
				decoded[di++] = 0;
			else if (inrange (pulselen (i) , 5) &&
					 inrange (pulselen (i+1), 17))
				decoded[di++] = 1;
			else {
				print_vals ();
				printf ("Decoding error at index %d : %d %d\n",
						i % BUFF_SIZE,
						pulselen (i), pulselen(i+1));
				HAL_TIM_IC_Start_IT (&htim2, TIM_CHANNEL_1);
				HAL_TIM_IC_Start_IT (&htim2, TIM_CHANNEL_2);
				/* while (1); */
				return false;
			}

			i++;
			if (i == BUFF_SIZE)
				i = 0;
		}
	}

	HAL_TIM_IC_Start_IT (&htim2, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT (&htim2, TIM_CHANNEL_2);

	uint8_t payload[4] = {0};
#ifdef PROC_DEB
	printf ("Decoded :");
	for (di=0; di < 32; ++di) {
		printf ("%d", decoded[di]);
	}
	puts ("");
#endif

	i = 0;
	size_t j = 0;
	for (j=0; j < 4; ++j) {
		for (i=0; i < 8 ; ++i) {
			payload[j] |= (decoded[j*8 + i] << i);
		}
	}

#ifdef PROC_DEB
	printf ("%02X %02X %02X %02X\n",
			payload[0],
			payload[1],
			payload[2],
			payload[3]);

	/* other checks */
#endif

	if ( (payload[0] ^ payload[1]) != 0xff ||
		 (payload[2] ^ payload[3]) != 0xff) {
		printf ("XOR error ");
		printf ("%02X %02X %02X %02X\n",
				payload[0],
				payload[1],
				payload[2],
				payload[3]);
		return false;
#ifdef PROC_DEB
		puts ("XOR error ");
#endif
	}


	uint8_t addr=payload[0], com = payload[2];

	printf ("Addr: %02X, com: %02X\n", addr, com);

	*ret_addr = addr;
	*ret_comm = com;
	return true;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)   {
	/* static int bleh; */
	/* add_val (bleh++); */
	int value = 0;
	if (htim->Channel == 1)
		value = HAL_TIM_ReadCapturedValue (&htim2, TIM_CHANNEL_1);
	else if (htim->Channel == 2)
		value = HAL_TIM_ReadCapturedValue (&htim2, TIM_CHANNEL_2);
	add_val (value, htim->Channel == 1);

}


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
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  /* unit tests */

  if (! (inrange (6,5) &&
		 inrange (16,16) &&
		 inrange (16, 17)) ) {
		  puts ("ERRRROR");
		  while (1);
	  }

	/* HAL_TIM_PWM_Start (&htim4, TIM_CHANNEL_1); */
	HAL_TIM_IC_Start_IT (&htim2, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT (&htim2, TIM_CHANNEL_2);

	puts ("\033[2J\033[H");
	while (1)
	{
		/* printf ("\033[2J\033[H Working! %d\n", rc++); */
		/* print_vals (); */
		HAL_Delay (2);

		int addr, com;
		if (process (&addr, &com)) {
			printf ("\t Command:  %d\n", com);
			struct keyboardHID_t keyboardHID = {0};
			keyboardHID.id = 1;

			do {
				if (com == 0x08) {
					keyboardHID.key1 = USB_KEY_LEFT;
				}
				else if (com == 0x5a) {
					keyboardHID.key1 = USB_KEY_RIGHT;
				}
				else
					break;
				USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&keyboardHID,
									sizeof(struct keyboardHID_t));

				HAL_Delay (30);
				keyboardHID.modifiers = 0;
				keyboardHID.key1 = 0;
				USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&keyboardHID,
									sizeof(struct keyboardHID_t));
			} while (0);

		}

		/* read b1 to get IR info */
		HAL_GPIO_ReadPin (GPIOB, GPIO_PIN_1);

#if 0
		uint8_t buff [ 16 ] = {0, 0, 4, 0, 0, 0, 0, 0};
		USBD_HID_SendReport (&hUsbDeviceFS, buff, 8);
#endif


#if 0
		struct keyboardHID_t keyboardHID;
		keyboardHID.id = 1;
		keyboardHID.modifiers = 0;
		keyboardHID.key1 = 0;
		keyboardHID.key2 = 0;
		keyboardHID.key3 = 0;


		keyboardHID.modifiers = 0;
		keyboardHID.key1 = 0;/////79;
		USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&keyboardHID, sizeof(struct keyboardHID_t));
		HAL_Delay(30);
		keyboardHID.modifiers = 0;
		keyboardHID.key1 = 0;
		USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&keyboardHID, sizeof(struct keyboardHID_t));
#endif

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 4799;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 64000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 35;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 499;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

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
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
	{
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
	   tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
