/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
#include <string.h>

#include "usart.h"

#define LCD_ADDR (0x27 << 1)       // адрес дисплея, сдвинутый на 1 бит влево (HAL работает с I2C-адресами, сдвинутыми на 1 бит влево)

#define PIN_RS    (1 << 0)         // если на ножке 0, данные воспринимаются как команда, если 1 - как символы для вывода
#define PIN_EN    (1 << 2)         // бит, по изменению сост. которого считывается информация
#define BACKLIGHT (1 << 3)         // управление подсветкой

#define LCD_DELAY_MS 5             // пауза перед высвечиванием символа

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
#if 0
void I2C_SendData(I2C_HandleTypeDef *hi2cx, uint8_t devAddr, uint8_t *data, uint8_t len)
{
	HAL_StatusTypeDef res;
	// бесконечный цикл
	for(;;)
	{
		// проверяем, готово ли устройство по адресу lcd_addr для связи
		res = HAL_I2C_IsDeviceReady(&hi2c1, devAddr << 1, 1, HAL_MAX_DELAY);
		// если да, то выходим из бесконечного цикла
		if(res == HAL_OK) break;
	}
	HAL_I2C_Master_Transmit(hi2cx, LCD_ADDR, data, len, HAL_MAX_DELAY);
}

void I2C_Scan ()
{
	// создание переменной, содержащей статус
        HAL_StatusTypeDef res;
	// сообщение о начале процедуры
	char info[] = "Scanning I2C bus...\r\n";
	// отправка сообщения по UART
	HAL_UART_Transmit(&huart1, (uint8_t*)info, strlen(info), HAL_MAX_DELAY);
//	 &huart5 - адрес используемого UART
//	 (uint8_t*)info - указатель на значение для отправки
//	 strlen(info) - длина отправляемого сообщения
//	 HAL_MAX_DELAY - задержка

	// перебор всех возможных адресов
	for(uint16_t i = 0; i < 128; i++)
	{
        // проверяем, готово ли устройство по адресу i для связи
        res = HAL_I2C_IsDeviceReady(&hi2c1, i << 1, 1, HAL_MAX_DELAY);
	    // если да, то
        if(res == HAL_OK)
	    {
	    	char msg[64];
	    	// запись адреса i, на который откликнулись, в строку в виде
            // 16тиричного значения:
	    	snprintf(msg, sizeof(msg), "0x%02X\n", i);
	    	// отправка номера откликнувшегося адреса
	    	HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	    	// переход на новую строчку
	    	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
	    }
	    else HAL_UART_Transmit(&huart1, (uint8_t*)".", 1, HAL_MAX_DELAY);
	}
	HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
}

void I2C_Send(uint8_t data, uint8_t flags)
{
	HAL_StatusTypeDef res;
	// бесконечный цикл
	for(;;) {
		// проверяем, готово ли устройство по адресу lcd_addr для связи
		res = HAL_I2C_IsDeviceReady(&hi2c1, LCD_ADDR, 1, HAL_MAX_DELAY);
		// если да, то выходим из бесконечного цикла
		if(res == HAL_OK) break;
	}
	// операция �? с 1111 0000 приводит к обнулению бит с 0 по 3, остаются биты с 4 по 7
	uint8_t up = data & 0xF0;
	// то же самое, но data сдвигается на 4 бита влево
	uint8_t lo = (data << 4) & 0xF0;

	uint8_t data_arr[4];
	// 4-7 биты содержат информацию, биты 0-3 настраивают работу дисплея
	data_arr[0] = up|flags|BACKLIGHT|PIN_EN;
	// дублирование сигнала, на выводе Е в этот раз 0
	data_arr[1] = up|flags|BACKLIGHT;
	data_arr[2] = lo|flags|BACKLIGHT|PIN_EN;
	data_arr[3] = lo|flags|BACKLIGHT;

	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_arr, sizeof(data_arr), HAL_MAX_DELAY);
	HAL_Delay(LCD_DELAY_MS);
}

void LCD_SendString(char *str)
{
    // *char по сути является строкой
    // пока строчка не закончится
	while(*str)
	{
		// передача первого символа строки
		I2C_Send((uint8_t)(*str), 1);
		// сдвиг строки налево на 1 символ
		str++;
	}
}
#endif
/* USER CODE END 1 */
