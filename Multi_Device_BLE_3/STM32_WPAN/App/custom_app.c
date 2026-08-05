/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    App/custom_app.c
  * @author  MCD Application Team
  * @brief   Custom Example Application (Server)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "main.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "custom_app.h"
#include "custom_stm.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "app_variable.h"
#include "app_ble.h"
#include "stm32wbxx_hal_adc.h"
#include "main.h"
#include "User_Task.h"
extern ADC_HandleTypeDef hadc1;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  /* STM_BLE */
  uint8_t               Stm_notify_Notification_Status;
  /* USER CODE BEGIN CUSTOM_APP_Context_t */

  /* USER CODE END CUSTOM_APP_Context_t */

  uint16_t              ConnectionHandle;
} Custom_App_Context_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

static Custom_App_Context_t Custom_App_Context;

/**
 * END of Section BLE_APP_CONTEXT
 */

uint8_t UpdateCharData[512];
uint8_t NotifyCharData[512];
uint16_t Connection_Handle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* STM_BLE */
static void Custom_Stm_notify_Update_Char(void);

static void Custom_Stm_notify_Send_Notification(void);

/* USER CODE BEGIN PFP */
void myTask()
{ 
   // task_handle();
    if(Packet_Ready)
     {

     // Custom_STM_App_Update_Char( CUSTOM_STM_STM_NOTIFY,(uint8_t *)ADC_Buffer);
    	for(int i=0;i<CFG_BLE_NUM_LINK;i++)
    	{
    	    uint16_t handle;

    	    handle= APP_BLE_Get_Connection_Handle(i);

    	    if(handle != 0xFFFF)
    	    {
    	        Custom_STM_App_Update_Char_Ext(handle, CUSTOM_STM_STM_NOTIFY, (uint8_t*)ADC_Buffer);
    	    }
    	}
     Packet_Ready = 0;
    
     }
     UTIL_SEQ_SetTask(1<<CFG_TASK_MY_TASK, CFG_SCH_PRIO_0);
  }
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void Custom_STM_App_Notification(Custom_STM_App_Notification_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_1 */

  /* USER CODE END CUSTOM_STM_App_Notification_1 */
  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* USER CODE END CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* STM_BLE */
    case CUSTOM_STM_STM_NOTIFY_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_STM_NOTIFY_NOTIFY_ENABLED_EVT */

      /* USER CODE END CUSTOM_STM_STM_NOTIFY_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_STM_NOTIFY_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_STM_NOTIFY_NOTIFY_DISABLED_EVT */

      /* USER CODE END CUSTOM_STM_STM_NOTIFY_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_NOTIFICATION_COMPLETE_EVT:
      /* USER CODE BEGIN CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */

      /* USER CODE END CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_STM_App_Notification_default */

      /* USER CODE END CUSTOM_STM_App_Notification_default */
      break;
  }
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_2 */

  /* USER CODE END CUSTOM_STM_App_Notification_2 */
  return;
}

void Custom_APP_Notification(Custom_App_ConnHandle_Not_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_APP_Notification_1 */

  /* USER CODE END CUSTOM_APP_Notification_1 */

  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_APP_Notification_Custom_Evt_Opcode */

    /* USER CODE END P2PS_CUSTOM_Notification_Custom_Evt_Opcode */
    case CUSTOM_CONN_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_CONN_HANDLE_EVT */

      /* USER CODE END CUSTOM_CONN_HANDLE_EVT */
      break;

    case CUSTOM_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_DISCON_HANDLE_EVT */

      /* USER CODE END CUSTOM_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_APP_Notification_default */

      /* USER CODE END CUSTOM_APP_Notification_default */
      break;
  }

  /* USER CODE BEGIN CUSTOM_APP_Notification_2 */

  /* USER CODE END CUSTOM_APP_Notification_2 */

  return;
}

void Custom_APP_Init(void)
{
  /* USER CODE BEGIN CUSTOM_APP_Init */

  /* USER CODE END CUSTOM_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* STM_BLE */
__USED void Custom_Stm_notify_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Stm_notify_UC_1*/

  /* USER CODE END Stm_notify_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_STM_NOTIFY, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Stm_notify_UC_Last*/

  /* USER CODE END Stm_notify_UC_Last*/
  return;
}

__USED void Custom_Stm_notify_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Stm_notify_NS_1*/

  /* USER CODE END Stm_notify_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_STM_NOTIFY, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Stm_notify_NS_Last*/

  /* USER CODE END Stm_notify_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
