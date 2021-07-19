/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rtc.h"
#include "queue.h"
#include "vector_data.h"
#include "usb_device.h"
#include "stm_ctrl.h"
#include "chip.h"
#include "servo.h"
#include <math.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for tskSpiService */
osThreadId_t tskSpiServiceHandle;
const osThreadAttr_t tskSpiService_attributes = {
  .name = "tskSpiService",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for tskServoService */
osThreadId_t tskServoServiceHandle;
const osThreadAttr_t tskServoService_attributes = {
  .name = "tskServoService",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for tskUsbTxData */
osThreadId_t tskUsbTxDataHandle;
const osThreadAttr_t tskUsbTxData_attributes = {
  .name = "tskUsbTxData",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for tskUsbRxCmd */
osThreadId_t tskUsbRxCmdHandle;
const osThreadAttr_t tskUsbRxCmd_attributes = {
  .name = "tskUsbRxCmd",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for tskUsbTxCmd */
osThreadId_t tskUsbTxCmdHandle;
const osThreadAttr_t tskUsbTxCmd_attributes = {
  .name = "tskUsbTxCmd",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for qData */
osMessageQueueId_t qDataHandle;
const osMessageQueueAttr_t qData_attributes = {
  .name = "qData"
};
/* Definitions for qCmdServo */
osMessageQueueId_t qCmdServoHandle;
const osMessageQueueAttr_t qCmdServo_attributes = {
  .name = "qCmdServo"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void startTskSpiService(void *argument);
void startTskServoService(void *argument);
void startTskUsbTxData(void *argument);
void startTskUsbRxCmd(void *argument);
void startTskUsbTxCmd(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of qData */
  qDataHandle = osMessageQueueNew (32, sizeof(Vector), &qData_attributes);

  /* creation of qCmdServo */
  qCmdServoHandle = osMessageQueueNew (4, sizeof(ServoControl), &qCmdServo_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of tskSpiService */
  tskSpiServiceHandle = osThreadNew(startTskSpiService, NULL, &tskSpiService_attributes);

  /* creation of tskServoService */
  tskServoServiceHandle = osThreadNew(startTskServoService, NULL, &tskServoService_attributes);

  /* creation of tskUsbTxData */
  tskUsbTxDataHandle = osThreadNew(startTskUsbTxData, NULL, &tskUsbTxData_attributes);

  /* creation of tskUsbRxCmd */
  tskUsbRxCmdHandle = osThreadNew(startTskUsbRxCmd, NULL, &tskUsbRxCmd_attributes);

  /* creation of tskUsbTxCmd */
  tskUsbTxCmdHandle = osThreadNew(startTskUsbTxCmd, NULL, &tskUsbTxCmd_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  servo_config();
  chip_config();
  for(;;) {
    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_startTskSpiService */
/**
* @brief Function implementing the tskSpiService thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startTskSpiService */
void startTskSpiService(void *argument)
{
  /* USER CODE BEGIN startTskSpiService */
  for(;;) {
    chip_msg_proc();
    osDelay(20 / portTICK_PERIOD_MS);
  }
  /* USER CODE END startTskSpiService */
}

/* USER CODE BEGIN Header_startTskServoService */
/**
* @brief Function implementing the tskServoService thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startTskServoService */
void startTskServoService(void *argument)
{
  /* USER CODE BEGIN startTskServoService */
  typedef enum RoatateDir_
  {
    dirNone,
    dirAnticlockwise,
    dirClockwise
  } RoatateDir;
  static uint32_t delay = 100 / portTICK_PERIOD_MS;
  RoatateDir rotateFl = dirNone;
  float angle = 0;
  float degreesPerSec = 0;
  for(;;) {
    uint32_t startTicks = xTaskGetTickCount() + delay;
    ServoControl ctrl;
    TickType_t tickWait = rotateFl != dirNone ? 0 : portMAX_DELAY;
    if (xQueueReceive(qCmdServoHandle, &ctrl, tickWait) == pdTRUE) {
      if (ctrl.cmd == servoAngle) {
        rotateFl = dirNone;
        servo_set_angle(ctrl.val);
      }
      else if (ctrl.cmd == servoRotate) {
        rotateFl = rotateFl == dirNone ? dirAnticlockwise : rotateFl;
        degreesPerSec = fabsf(ctrl.val);
        angle = servo_angle();
        continue;
      }
    }

    if (rotateFl != dirNone) {
      const int8_t rotateCoef = rotateFl == dirAnticlockwise ? 1 : -1;
      double degreesPerFrame = degreesPerSec / 1000.0 * delay;
      double mod360 = fmod(angle + rotateCoef * degreesPerFrame, 360);
      if (mod360 > 270.0 && rotateFl == dirAnticlockwise) {
        rotateFl = dirClockwise;
        angle = 270.0;
      }
      else if (mod360 < 0.0 && rotateFl == dirClockwise) {
        rotateFl = dirAnticlockwise;
        angle = 0.0;
      }
      else {
        angle = mod360;
      }
      servo_set_angle(angle);
      osDelayUntil(startTicks);
    }
  }
  /* USER CODE END startTskServoService */
}

/* USER CODE BEGIN Header_startTskUsbTxData */
/**
* @brief Function implementing the tskUsbTxData thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startTskUsbTxData */
void startTskUsbTxData(void *argument)
{
  /* USER CODE BEGIN startTskUsbTxData */
  for(;;) {
    Vector txData = make_vector_sz(16);
    Vector qTxData = make_vector();
    xQueueReceive(qDataHandle, &qTxData, portMAX_DELAY);
    vector_append_vct(&txData, &qTxData);
    vector_free(&qTxData);

    while (xQueueReceive(qDataHandle, &qTxData, 0) == pdTRUE) {
      vector_append_vct(&txData, &qTxData);
      vector_free(&qTxData);
    }
    usb_send_data(txData.data, txData.size);

    osDelay(1000 / portTICK_PERIOD_MS);
    vector_free(&txData);
  }
  /* USER CODE END startTskUsbTxData */
}

/* USER CODE BEGIN Header_startTskUsbRxCmd */
/**
* @brief Function implementing the tskUsbRxCmd thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startTskUsbRxCmd */
void startTskUsbRxCmd(void *argument)
{
  /* USER CODE BEGIN startTskUsbRxCmd */
  for(;;) {
    ctrl_cmd_proc();
    osDelay(100 / portTICK_PERIOD_MS);
  }
  /* USER CODE END startTskUsbRxCmd */
}

/* USER CODE BEGIN Header_startTskUsbTxCmd */
/**
* @brief Function implementing the tskUsbTxCmd thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startTskUsbTxCmd */
void startTskUsbTxCmd(void *argument)
{
  /* USER CODE BEGIN startTskUsbTxCmd */
  char cmdResponse[64];
  RTC_TimeTypeDef time;
  for(;;) {
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, NULL, RTC_FORMAT_BIN);
    size_t seconds = time.Hours * 3600 + time.Minutes * 60 + time.Seconds;
    int8_t reconfigEnable = chip_reconfig_enable() == chipReconfNo ? 0 : 1;
    int lenCmd = snprintf(cmdResponse, sizeof(cmdResponse),
                          "#time=%u\r\n#angle=%3.2f\r\n#reconfigenable=%1d\r\n",
                          seconds, servo_angle(), reconfigEnable);
    usb_send_cmd((uint8_t*)cmdResponse, lenCmd);
    osDelay(1000 / portTICK_PERIOD_MS);
  }
  /* USER CODE END startTskUsbTxCmd */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
