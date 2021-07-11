/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GPIO10_Pin GPIO_PIN_2
#define GPIO10_GPIO_Port GPIOE
#define GPIO11_Pin GPIO_PIN_3
#define GPIO11_GPIO_Port GPIOE
#define GPIO12_Pin GPIO_PIN_4
#define GPIO12_GPIO_Port GPIOE
#define GPIO13_Pin GPIO_PIN_5
#define GPIO13_GPIO_Port GPIOE
#define GPIO14_Pin GPIO_PIN_6
#define GPIO14_GPIO_Port GPIOE
#define GPIO15_Pin GPIO_PIN_7
#define GPIO15_GPIO_Port GPIOE
#define GPIO0_Pin GPIO_PIN_8
#define GPIO0_GPIO_Port GPIOD
#define GPIO1_Pin GPIO_PIN_9
#define GPIO1_GPIO_Port GPIOD
#define GPIO2_Pin GPIO_PIN_10
#define GPIO2_GPIO_Port GPIOD
#define GPIO3_Pin GPIO_PIN_11
#define GPIO3_GPIO_Port GPIOD
#define GPIO4_Pin GPIO_PIN_12
#define GPIO4_GPIO_Port GPIOD
#define GPIO5_Pin GPIO_PIN_13
#define GPIO5_GPIO_Port GPIOD
#define GPIO6_Pin GPIO_PIN_14
#define GPIO6_GPIO_Port GPIOD
#define GPIO7_Pin GPIO_PIN_15
#define GPIO7_GPIO_Port GPIOD
#define CPP_IRQ_E_Pin GPIO_PIN_0
#define CPP_IRQ_E_GPIO_Port GPIOD
#define CPP_nRST_EXT_Pin GPIO_PIN_1
#define CPP_nRST_EXT_GPIO_Port GPIOD
#define CPP_nRST_POR_Pin GPIO_PIN_2
#define CPP_nRST_POR_GPIO_Port GPIOD
#define CPP_nRST_OUT_Pin GPIO_PIN_3
#define CPP_nRST_OUT_GPIO_Port GPIOD
#define GPIO8_Pin GPIO_PIN_0
#define GPIO8_GPIO_Port GPIOE
#define GPIO9_Pin GPIO_PIN_1
#define GPIO9_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
