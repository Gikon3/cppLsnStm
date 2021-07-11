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
#include "vector_data.h"
#include "chip.h"
#include "servo.h"
#include "stm_ctrl.h"
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
  .stack_size = 128 * 4,
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END startTskUsbTxCmd */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
