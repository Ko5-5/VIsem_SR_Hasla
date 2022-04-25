/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "fonts.h"
#include "ssd1306.h"
#include "usbd_hid.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct
{
	uint8_t MODIFIER;
	uint8_t RESERVED;
	uint8_t KEYCODE1;
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
} keyboardHID;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint32_t i=0;
uint32_t counter = 0;
int16_t count = 0;

extern USBD_HandleTypeDef hUsbDeviceFS;

keyboardHID keyboardhid = {0,0,0,0,0,0,0,0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void OLED_page_test_sc(){
	// OLED screen test
	SSD1306_Fill(SSD1306_COLOR_BLACK);
	SSD1306_GotoXY (4,6); // goto 10, 10
	SSD1306_Puts ("--MHS--", &Font_11x18, 1); // print Hello
	SSD1306_GotoXY (4,25); // goto 10, 10
	SSD1306_Puts ("PAGE", &Font_11x18, 1); // print Hello
	SSD1306_GotoXY (4,44); // goto 10, 10
	SSD1306_Puts ("NUMBER:", &Font_11x18, 1); // print Hello
	SSD1306_DrawLine(0, 1, 128, 1, SSD1306_COLOR_WHITE);
	SSD1306_DrawLine(0, 0, 0, 64, SSD1306_COLOR_WHITE);
	SSD1306_DrawLine(128, 0, 128, 64, SSD1306_COLOR_WHITE);
	SSD1306_DrawLine(0, 64, 128, 64, SSD1306_COLOR_WHITE);
	SSD1306_DrawRectangle(100, 2, 27, 62, SSD1306_COLOR_WHITE);
	SSD1306_GotoXY (110,25); // goto 10, 10
	SSD1306_Putc ((char)(i+1+'0'), &Font_11x18, 1); // print Hello
	SSD1306_UpdateScreen(); // update screen
	if(i==8)
		i=0;
	else
		i++;
	HAL_Delay(1000);
}

void USB_HID_test(){
	keyboardhid.MODIFIER = 0x02; // lewy Shift naciśniety
	keyboardhid.KEYCODE1 = 0x04; // litera a nacisnieta
	keyboardhid.KEYCODE2 = 0x05; // litera b nacisnieta
	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&keyboardhid, sizeof(keyboardhid));
	HAL_Delay(50);
	keyboardhid.MODIFIER = 0x00; // lewy Shift puszczony
	keyboardhid.KEYCODE1 = 0x00; // litera a puszczona
	keyboardhid.KEYCODE2 = 0x00; // litera b puszczona
	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&keyboardhid, sizeof(keyboardhid));
	HAL_Delay(50);
}
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
	MX_I2C1_Init();
	MX_SPI3_Init();
	MX_TIM2_Init();
	MX_USB_DEVICE_Init();
	/* USER CODE BEGIN 2 */
	SSD1306_Init();
	HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		OLED_page_test_sc();
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		USB_HID_test();
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
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 192;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
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
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	counter = __HAL_TIM_GET_COUNTER(htim);
	count = (int16_t)counter;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

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