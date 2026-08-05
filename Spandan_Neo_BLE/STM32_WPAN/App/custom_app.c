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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  /* SPANDAN_PATCH */
  uint8_t               Mycharnotify_Notification_Status;
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

uint8_t UpdateCharData[1024];
uint8_t NotifyCharData[1024];
uint16_t Connection_Handle;
/* USER CODE BEGIN PV */
//unsigned char UpdateCharData[100];
int  UCnt = 0;
extern int i;
extern _Bool Packet_Constructed;

extern int Health_Cnt1;

extern int ECG_1_ADC_BUFFER[];
extern int ECG_2_ADC_BUFFER[];

unsigned char VCP_STRING[256];

extern int Data_Sending_Enable;

//extern float battery_percentage; // SOC in percentage (0-100%)
//extern float battery_voltage;   // VCELL in volts
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* SPANDAN_PATCH */
static void Custom_Mycharnotify_Update_Char(void);
static void Custom_Mycharnotify_Send_Notification(void);

/* USER CODE BEGIN PFP */
void myTask(void)
{
  if(Packet_Constructed)
  {
		Custom_Mycharnotify_Update_Char();
		Custom_Mycharnotify_Send_Notification();
		Packet_Constructed = 0;
  }
	UTIL_SEQ_SetTask(1<<CFG_TASK_MY_TASK , CFG_SCH_PRIO_0 );
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

    /* SPANDAN_PATCH */
    case CUSTOM_STM_CHAR_WR_WRITE_EVT:
      /* USER CODE BEGIN CUSTOM_STM_CHAR_WR_WRITE_EVT */

      /* USER CODE END CUSTOM_STM_CHAR_WR_WRITE_EVT */
      break;

    case CUSTOM_STM_MYCHARNOTIFY_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_MYCHARNOTIFY_NOTIFY_ENABLED_EVT */

      /* USER CODE END CUSTOM_STM_MYCHARNOTIFY_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_MYCHARNOTIFY_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_MYCHARNOTIFY_NOTIFY_DISABLED_EVT */

      /* USER CODE END CUSTOM_STM_MYCHARNOTIFY_NOTIFY_DISABLED_EVT */
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
unsigned char STR_SPDNY[] = "splgY";
unsigned char Version_No[7]="000.03"; //  Change the version in both the lines
unsigned char DID[20] = "SPLG-DN01-2312110001";
unsigned char DUID[12]="12345678902";;
uint8_t Utsav_BLE_Status = 0;


extern int RX_Cnt;
extern int TX_Cnt;


extern uint8_t Data_Queue[Query_len][Query_Size];

_Bool Roll_Back = 0;

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* SPANDAN_PATCH */
__USED void Custom_Mycharnotify_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;
//  Utsav_BLE_Status = 0;
//  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Mycharnotify_UC_1*/
  updateflag = Data_Sending_Enable;
//  HAL_GPIO_TogglePin(Health_LED_GPIO_Port, Health_LED_Pin);
  if(Data_Sending_Enable == 2)
  {
	  memset(UpdateCharData,0x00,247);

	  memset(VCP_STRING,0x00,50);

	  	for(i=0;i<5;i++)
	  	{
	  		if(i==5-1)
	  			VCP_STRING[i] = '-';
	  		else
	  			VCP_STRING[i] = STR_SPDNY[i];
	  	}
	  	for(i=0;i<7;i++)
	  	{
	  		if(i==7-1)
	  			VCP_STRING[i+5] = '-';
	  		else
	  			VCP_STRING[i+5] = Version_No[i];
	  	}
	  	for(i=0;i<21;i++)
	  	{
	  		if(i==21-1)
	  			VCP_STRING[i+5+7] = '-';
	  		else
	  			VCP_STRING[i+5+7] = DID[i];
	  	}
	  	for(i=0;i<13;i++)
	  	{
	  			VCP_STRING[i+5+7+21] = DUID[i];
	  	}
	  	VCP_STRING[45] = 'Y';

	  	memcpy(UpdateCharData,VCP_STRING,46);

	  Data_Sending_Enable = 0;
  }
  /* USER CODE END Mycharnotify_UC_1*/

  if (updateflag != 0)
  {
		if((TX_Cnt < RX_Cnt) || (Roll_Back == 1))
		{
		  Utsav_BLE_Status = 0;

		  memcpy(UpdateCharData,Data_Queue[TX_Cnt],Data_Quere_Len);

		  Utsav_BLE_Status = Custom_STM_App_Update_Char(CUSTOM_STM_MYCHARNOTIFY, (uint8_t *)UpdateCharData);

		  if(Utsav_BLE_Status == 0)
			{
				TX_Cnt++;
				if(TX_Cnt == Query_len)
				{
					TX_Cnt = 0;
					Roll_Back = 0;
				}
			}
		}
  }

  /* USER CODE BEGIN Mycharnotify_UC_Last*/

  /* USER CODE END Mycharnotify_UC_Last*/
  return;
}

void Custom_Mycharnotify_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Mycharnotify_NS_1*/

  /* USER CODE END Mycharnotify_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_MYCHARNOTIFY, (uint8_t *)NotifyCharData);
  }

  /* USER CODE BEGIN Mycharnotify_NS_Last*/

  /* USER CODE END Mycharnotify_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
