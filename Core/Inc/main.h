/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#if 0 // 8 bit mode
#define D5_Pin GPIO_PIN_8
#define D5_GPIO_Port GPIOD
#define D6_Pin GPIO_PIN_9
#define D6_GPIO_Port GPIOD
#define D7_Pin GPIO_PIN_10
#define D7_GPIO_Port GPIOD
#define RS_Pin GPIO_PIN_0
#define RS_GPIO_Port GPIOD
#define RW_Pin GPIO_PIN_1
#define RW_GPIO_Port GPIOD
#define E_Pin GPIO_PIN_2
#define E_GPIO_Port GPIOD
#define D0_Pin GPIO_PIN_3
#define D0_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_4
#define D1_GPIO_Port GPIOD
#define D2_Pin GPIO_PIN_5
#define D2_GPIO_Port GPIOD
#define D3_Pin GPIO_PIN_6
#define D3_GPIO_Port GPIOD
#define D4_Pin GPIO_PIN_7
#define D4_GPIO_Port GPIOD
#endif
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define D5_Pin GPIO_PIN_8
#define D5_GPIO_Port GPIOD
#define D6_Pin GPIO_PIN_9
#define D6_GPIO_Port GPIOD
#define D7_Pin GPIO_PIN_10
#define D7_GPIO_Port GPIOD
#define SRCLK_Pin GPIO_PIN_11
#define SRCLK_GPIO_Port GPIOD
#define RCLK_Pin GPIO_PIN_12
#define RCLK_GPIO_Port GPIOD
#define SER_Pin GPIO_PIN_13
#define SER_GPIO_Port GPIOD
#define RS_Pin GPIO_PIN_0
#define RS_GPIO_Port GPIOD
#define RW_Pin GPIO_PIN_1
#define RW_GPIO_Port GPIOD
#define E_Pin GPIO_PIN_2
#define E_GPIO_Port GPIOD
#define D0_Pin GPIO_PIN_3
#define D0_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_4
#define D1_GPIO_Port GPIOD
#define D2_Pin GPIO_PIN_5
#define D2_GPIO_Port GPIOD
#define D3_Pin GPIO_PIN_6
#define D3_GPIO_Port GPIOD
#define D4_Pin GPIO_PIN_7
#define D4_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */