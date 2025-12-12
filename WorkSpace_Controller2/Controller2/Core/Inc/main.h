/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdbool.h>
#include "stdio.h"
#include <string.h>
/* SIMCom library */
#include "SIMCOM.h"
/* tof200c library */
#include "VL53L0X.h"
/* ultrasonic library */
#include "Ultrasonic.h"
/* load cell library */
#include "HX711.h"
/* voltage sensor */
#include "Voltage.h"
/* RTC */
#include "RTC.h"
#include "WATERSENSOR.h"
/* water sensor */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
HAL_StatusTypeDef LOG(const char* header,const char *cmd);

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
#define LOCK_Pin GPIO_PIN_6
#define LOCK_GPIO_Port GPIOA
#define VALVE_Pin GPIO_PIN_7
#define VALVE_GPIO_Port GPIOA
#define DAEGUN_LOAD_Pin GPIO_PIN_0
#define DAEGUN_LOAD_GPIO_Port GPIOB
#define DEVICE_Pin GPIO_PIN_1
#define DEVICE_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_12
#define LED_GPIO_Port GPIOB
#define Door_IN1_Pin GPIO_PIN_14
#define Door_IN1_GPIO_Port GPIOB
#define Door_IN2_Pin GPIO_PIN_15
#define Door_IN2_GPIO_Port GPIOB
#define EXTI_Wakeup_Pin GPIO_PIN_8
#define EXTI_Wakeup_GPIO_Port GPIOA
#define EXTI_Wakeup_EXTI_IRQn EXTI9_5_IRQn
#define U_Trig_Pin GPIO_PIN_3
#define U_Trig_GPIO_Port GPIOB
#define U_Echo_Pin GPIO_PIN_4
#define U_Echo_GPIO_Port GPIOB
#define LC_SCK_Pin GPIO_PIN_5
#define LC_SCK_GPIO_Port GPIOB
#define LC_Data1_Pin GPIO_PIN_8
#define LC_Data1_GPIO_Port GPIOB
#define LC_Data2_Pin GPIO_PIN_9
#define LC_Data2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
/* Buffer size define */
#define UART_RX_BUFFER_SIZE 500

/* Load cell IO define */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
