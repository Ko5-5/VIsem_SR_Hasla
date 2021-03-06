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

#include <stdio.h>
#include <string.h>
#include "fonts.h"
#include "ssd1306.h"
#include "usbd_hid.h"
#include "w25qxx.h"
#include "keyboard_keys.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/*! \brief Struktura do obsługi urządzenia HID
 * 	Struktura używana przez urządzenie w trybie HID do wysyłania danych przez USB dla ramka danych
 */
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

//---- Status urządzenia ----
uint8_t deviceFLAG = 0; //!< Tryb urządzenia: 0 -> odczytywanie haseł; 1 -> zapis haseł z serial port
uint8_t deviceState = 0; //!< Status urządzenia: 0 - login ; 1 - normal ; 99 - block
uint8_t loginTries = 5; //!< Licznik prób logowania
uint8_t login[4] = {1, 2, 3, 4}; //!< Login do urządzenia
uint8_t loginBuff[4]; //!< Buffor obecnej próby logowania
uint8_t loginCounter = 0; //!< Licznik obecnej próby logowania
//----

//---- Macierz przycisków -----
GPIO_InitTypeDef GPIO_InitStructPrivate = {0}; //!< Struktura GPIO do obsługi matrycy przycisków na przerwaniach
volatile uint8_t keyPressed = 0; //!< Numer obecnie wciśniętego przycisku
volatile uint8_t keyFlag = 0; //!< Flaga oznaczająca wciśnięcie przycisku
volatile uint32_t previousMillis = 0; //!< Poprzedni czas użycia matrycy, debouncing
volatile uint32_t currentMillis = 0; //!< Obecny czas użycia matrycy, debouncing
//----

//---- Pamięć FLASH ----
uint8_t passwordWrite[64]; //!< Buffor zapisu hasła do pamięci flash
uint8_t passwordRead[64]; //!< Buffor odczytu hasła do pamięci flash
uint8_t cipherKey[8] = "hubertus"; //!< Klucz szyfrowania XOR
//----

//--- Enkoder obrotowy ----
uint32_t counter = 0; //!< Wartość odczytana z enkodera
int16_t count = 0; //!< Wartość względna odczytana z enkodera
int16_t old_count = 0; //!< Wartość poprzednia odczytana z enkodera
//----

//---- USB Device ----
extern USBD_HandleTypeDef hUsbDeviceFS; //!< Handler urządzenia usb dla trybu HID
keyboardHID keyboardhid = {0,0,0,0,0,0,0,0}; //!< Ramka danych dla trybu HID urządzenia USB
//----

//---- USB Vitural COM ----
uint8_t DataToSend[40]; //!< Tablica zawierajaca dane do wyslania
uint8_t MessageLength = 0; //!< Zawiera dlugosc wysylanej wiadomosci

uint8_t ReceivedData[64]; //!< Tablica przechowujaca odebrane dane
uint8_t ReceivedLength; //!< Długość otrzymanego hasła
uint8_t ReceivedIter = 0; //!< Iterator do odbierania wiadomości
uint8_t ReceivedPassNr; //!< Numer strony na którym należy zapisać hasło
uint8_t ReceivedDataFlag = 0; //!< Flaga informujaca o odebraniu danych
//----

//---- Wyświetlacz OLED ---
volatile uint32_t pageNumber = 0; //!< Numer strony haseł na ekran i do logiki
//----

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/*! \brief Funkcja szyfrująca hasło przy pomocy operacji binarnego XOR
 * 	Funkcja szyfrująca hasło przy pomocy operacji binarnego XOR, dostaje tablice char oraz klucz do szyfrowania
 */
void XORCipher(uint8_t* string, uint8_t* key)
{
	for(int i = 0; i < strlen(string); i++)
	{
		string[i] = string[i] ^ key[i % 8];
	}
}

/*! \brief Funkcja wyświetlająca ekran trybu wpisywania
 * 	Funkcja wyświetlająca ekran trybu wpisywania na wyświetlaczu OLED przy pomocy połączenia I2C
 */
void OLED_write_sc(){
	static uint32_t lastRefresh = 0;
	if(HAL_GetTick() - lastRefresh > 1000){
		SSD1306_Fill(SSD1306_COLOR_BLACK);
		SSD1306_GotoXY (4,6);
		SSD1306_Puts ("-- WRITE --", &Font_11x18, 1);
		SSD1306_GotoXY (4,25);
		SSD1306_Puts ("-- MODE  --", &Font_11x18, 1);
		SSD1306_DrawLine(0, 1, 128, 1, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(0, 0, 0, 64, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(128, 0, 128, 64, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(0, 64, 128, 64, SSD1306_COLOR_WHITE);
		SSD1306_UpdateScreen();
	}
}

/*! \brief Funkcja wyświetlająca ekran wyboru strony
 * 	Funkcja wyświetlająca ekran wyboru strony na wyświetlaczu OLED przy pomocy połączenia I2C
 */
void OLED_page_sc(){
	static uint32_t lastRefresh = 0;
	if(HAL_GetTick() - lastRefresh > 1000){
		SSD1306_Fill(SSD1306_COLOR_BLACK);
		SSD1306_GotoXY (4,6);
		SSD1306_Puts ("--MHS--", &Font_11x18, 1);
		SSD1306_GotoXY (4,25);
		SSD1306_Puts ("PAGE", &Font_11x18, 1);
		SSD1306_GotoXY (4,44);
		SSD1306_Puts ("NUMBER:", &Font_11x18, 1);
		SSD1306_DrawLine(0, 1, 128, 1, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(0, 0, 0, 64, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(128, 0, 128, 64, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(0, 64, 128, 64, SSD1306_COLOR_WHITE);
		SSD1306_DrawRectangle(100, 2, 27, 62, SSD1306_COLOR_WHITE);
		SSD1306_GotoXY (110,25);
		SSD1306_Putc ((char)(pageNumber+'0'), &Font_11x18, 1);
		SSD1306_UpdateScreen();
	}
}

/*! \brief Funkcja wyświetlająca ekran logowania
 * 	Funkcja wyświetlająca ekran logowania na wyświetlaczu OLED przy pomocy połączenia I2C
 */
void OLED_login_sc(){
	static uint32_t lastRefresh = 0;
	if(HAL_GetTick() - lastRefresh > 1000){
		SSD1306_Fill(SSD1306_COLOR_BLACK);
		SSD1306_GotoXY (4,6);
		SSD1306_Puts ("-ENTER-", &Font_11x18, 1);
		SSD1306_GotoXY (4,25);
		SSD1306_Puts ("PASSWORD", &Font_11x18, 1);
		SSD1306_GotoXY (4,44);
		SSD1306_Puts ("TRIES:", &Font_11x18, 1);
		SSD1306_DrawLine(0, 1, 128, 1, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(0, 0, 0, 64, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(128, 0, 128, 64, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(0, 64, 128, 64, SSD1306_COLOR_WHITE);
		SSD1306_DrawRectangle(100, 2, 27, 62, SSD1306_COLOR_WHITE);
		SSD1306_GotoXY (110,25);
		SSD1306_Putc ((char)(loginTries+'0'), &Font_11x18, 1);
		SSD1306_UpdateScreen();
	}
}

/*! \brief Funkcja wyświetlająca ekran blokady
 * 	Funkcja wyświetlająca ekran blokady na wyświetlaczu OLED przy pomocy połączenia I2C
 */
void OLED_block_sc(){
	static uint32_t lastRefresh = 0;
	if(HAL_GetTick() - lastRefresh > 1000){
		SSD1306_Fill(SSD1306_COLOR_BLACK);
		SSD1306_GotoXY (4,6);
		SSD1306_Puts ("- DEVICE  -", &Font_11x18, 1);
		SSD1306_GotoXY (4,25);
		SSD1306_Puts ("- BLOCKED -", &Font_11x18, 1);
		SSD1306_DrawLine(0, 1, 128, 1, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(0, 0, 0, 64, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(128, 0, 128, 64, SSD1306_COLOR_WHITE);
		SSD1306_DrawLine(0, 64, 128, 64, SSD1306_COLOR_WHITE);
		SSD1306_UpdateScreen();
	}
}

/*! \brief Funkcja przesyłająca hasło do komputera po USB
 * 	Funkcja dostaje tablice znaków char, którą po literze wysyła do komputera poprzez połączenie USB
 */
void sendUSB(uint8_t *pass){
	while(*pass != '\0'){
		for(int i=0; i<KEYS_NUM; i++){
			if(*pass == keys[i].value){
				keyboardhid.KEYCODE1 = keys[i].hex_num;
				USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&keyboardhid, sizeof(keyboardhid));
				HAL_Delay(50);
				keyboardhid.KEYCODE1 = 0x00;
				USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&keyboardhid, sizeof(keyboardhid));
				HAL_Delay(50);
				pass++;
				break;
			}
			else if(*pass == keys[i].shiftValue){
				keyboardhid.MODIFIER = 0x02;
				keyboardhid.KEYCODE1 = keys[i].hex_num;
				USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&keyboardhid, sizeof(keyboardhid));
				HAL_Delay(50);
				keyboardhid.MODIFIER = 0x00;
				keyboardhid.KEYCODE1 = 0x00;
				USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&keyboardhid, sizeof(keyboardhid));
				HAL_Delay(50);
				pass++;
				break;
			}
		}
	}
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
	if(HAL_GPIO_ReadPin(ROT_BUT_GPIO_Port, ROT_BUT_Pin)){
		MX_USB_DEVICE_Init(0);
		deviceFLAG = 0;
	}else{
		MX_USB_DEVICE_Init(1);
		deviceFLAG = 1;
	}
	/* USER CODE BEGIN 2 */

	//---- Inicjalizacja wyświetlacza OLED ----
	SSD1306_Init();

	//---- Inicjalizacja obsługi enkodera ----
	HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL);

	//---- Inicjalizacja pamięci FLASH ----
	W25qxx_Init();
	//---- Wypełnianie pamięci domyślnymi hasłami ----
	//W25qxx_EraseChip();
	/*
	memcpy(passwordWrite, "123456\0", sizeof(passwordWrite));
	XORCipher(passwordWrite, cipherKey);
	W25qxx_WritePage(passwordWrite, 1, 0, 64);
	memcpy(passwordWrite, "654321\0", sizeof(passwordWrite));
	XORCipher(passwordWrite, cipherKey);
	W25qxx_WritePage(passwordWrite, 2, 0, 64);
	memcpy(passwordWrite, "HaloHalo\0", sizeof(passwordWrite));
	XORCipher(passwordWrite, cipherKey);
	W25qxx_WritePage(passwordWrite, 3, 0, 64);
	memcpy(passwordWrite, "Cartoon-Duck-14-Coffee-Glvs\0", sizeof(passwordWrite));
	XORCipher(passwordWrite, cipherKey);
	W25qxx_WritePage(passwordWrite, 4, 0, 64);
	memcpy(passwordWrite, "doubleclick\0", sizeof(passwordWrite));
	XORCipher(passwordWrite, cipherKey);
	W25qxx_WritePage(passwordWrite, 5, 0, 64);
	memcpy(passwordWrite, "supersecure\0", sizeof(passwordWrite));
	XORCipher(passwordWrite, cipherKey);
	W25qxx_WritePage(passwordWrite, 6, 0, 64);
	memcpy(passwordWrite, "Qwerty\0", sizeof(passwordWrite));
	XORCipher(passwordWrite, cipherKey);
	W25qxx_WritePage(passwordWrite, 7, 0, 64);
	memcpy(passwordWrite, "DEFAULT\0", sizeof(passwordWrite));
	XORCipher(passwordWrite, cipherKey);
	W25qxx_WritePage(passwordWrite, 8, 0, 64);
	memcpy(passwordWrite, "password\0", sizeof(passwordWrite));
	XORCipher(passwordWrite, cipherKey);
	W25qxx_WritePage(passwordWrite, 9, 0, 64);
	*/
	/*
	memcpy(passwordWrite, "0\0", sizeof(passwordWrite));
	XORCipher(passwordWrite, cipherKey);
	W25qxx_WritePage(passwordWrite, 10, 0, 64);
	*/
	//---- Inicjalizacja obsługi macierzy przycisków
	HAL_GPIO_WritePin(COL1_GPIO_Port, COL1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(COL2_GPIO_Port, COL2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(COL3_GPIO_Port, COL3_Pin, GPIO_PIN_SET);

	//---- LED oznaczający zakończenie setupu
	HAL_GPIO_WritePin(PWR_LED_GPIO_Port, PWR_LED_Pin, 1);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		if(deviceFLAG){  // Tryb urządzenia Virtual COM do zapisywania przesłanych haseł
			OLED_write_sc();
			uint8_t Text[] = "WRITE MODE -> len:nr:password\r\n";
			CDC_Transmit_FS(Text,strlen(Text)); /*when commented the port is recognized*/
			if(ReceivedDataFlag == 1){
				ReceivedDataFlag = 0;
				//MessageLength = sprintf(DataToSend, "Odebrano: %s\n\r", ReceivedData);
				//CDC_Transmit_FS(DataToSend, MessageLength);
				uint8_t text[64];
				ReceivedPassNr = (ReceivedData[0] - '0') * 10 + (ReceivedData[1] - '0');
				for(int i = 0; i<strlen(ReceivedData)-3; i++){
					text[i] = ReceivedData[i+3];
				}
				text[strlen(ReceivedData)-3] = '\0';
				memcpy(passwordWrite, text, sizeof(passwordWrite));
				XORCipher(passwordWrite, cipherKey);
				W25qxx_WritePage(passwordWrite, (uint32_t)ReceivedPassNr, 0, 64);
				memset(ReceivedData,0, 64);
				ReceivedPassNr = 0;
			}
			HAL_Delay(1000);
		}else{ // Tryb urządzenia HID do odczytywania haseł
			if(deviceState == 0){ // Urządzenie zablokowane -> wpisywanie hasła urządzenia
				OLED_login_sc();
				if(keyFlag){
					loginBuff[loginCounter] = keyPressed;
					loginCounter++;
					keyFlag = 0;
				}
				if(loginCounter == 4){
					if(login[0] == loginBuff[0] && login[1] == loginBuff[1] && login[2] == loginBuff[2] && login[3] == loginBuff[3]){
						deviceState = 1;
					}else{
						loginCounter = 0;
						loginTries--;
					}
				}
				if(loginTries == 0){
					deviceState = 99;
				}
			}
			else if(deviceState == 99){ // Urządzenie całkowicie zablokowane
				OLED_block_sc();
			}
			else{ // Urządzenie odblokowane -> odczyt haseł
				OLED_page_sc();
				if(keyFlag){
					W25qxx_ReadPage(passwordRead, pageNumber*9+keyPressed, 0, 64);
					XORCipher(passwordRead, cipherKey);
					sendUSB(passwordRead);
					keyFlag = 0;
				}
			}
		}
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
	if(count - old_count > 5)
	{
		if(pageNumber > 0)
			pageNumber--;
		old_count = count;
	}else if(count - old_count < -5)
	{
		if(pageNumber < 9)
			pageNumber++;
		old_count = count;
	}

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	currentMillis = HAL_GetTick();
	if(currentMillis - previousMillis > 300)
	{
		GPIO_InitStructPrivate.Pin = ROW1_Pin|ROW2_Pin|ROW3_Pin;
		GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
		GPIO_InitStructPrivate.Pull = GPIO_PULLDOWN;
		GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructPrivate);

		HAL_GPIO_WritePin(COL1_GPIO_Port, COL1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(COL2_GPIO_Port, COL2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(COL3_GPIO_Port, COL3_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin)){
			keyPressed = 1;
		}
		else if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin)){
			keyPressed = 4;
		}
		else if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin)){
			keyPressed = 7;

		}
		HAL_GPIO_WritePin(COL1_GPIO_Port, COL1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(COL2_GPIO_Port, COL2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(COL3_GPIO_Port, COL3_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin)){
			keyPressed = 2;
		}
		else if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin)){
			keyPressed = 5;
		}
		else if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin)){
			keyPressed = 8;
		}
		HAL_GPIO_WritePin(COL1_GPIO_Port, COL1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(COL2_GPIO_Port, COL2_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(COL3_GPIO_Port, COL3_Pin, GPIO_PIN_SET);
		if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin)){
			keyPressed = 3;
		}
		else if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin)){
			keyPressed = 6;
		}
		else if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin)){
			keyPressed = 9;
		}

		HAL_GPIO_WritePin(COL1_GPIO_Port,COL1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(COL2_GPIO_Port,COL2_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(COL3_GPIO_Port,COL3_Pin, GPIO_PIN_SET);

		GPIO_InitStructPrivate.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStructPrivate.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStructPrivate);


		keyFlag = 1;
		previousMillis = HAL_GetTick();
	}

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
