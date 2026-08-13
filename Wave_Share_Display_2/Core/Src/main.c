/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "snow_tiger.h"
#include "image_data.h"
#include "string.h"
#include "stdio.h"
#include "GraphInit.h"
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

/* USER CODE BEGIN PV */
int _write(int fd, char *ptr, int len)
{
	HAL_StatusTypeDef hstatus;
	if(fd==1 || fd==2)
	{
		hstatus =  HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
		  return len;

		  if(hstatus == HAL_OK)
		  return len;

		  else
			  return -1;
	}
	return -1;
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* Make adc_val volatile so the main loop sees updates made by DMA/callbacks.
   Use a 32-bit storage because HAL_ADC_Start_DMA expects a uint32_t* buffer. */
#define Num_adc_channel 2

volatile uint32_t adc_val = 0;
char adc_text[16];
 volatile uint8_t count = 0;
uint8_t count_limit = 2;
volatile uint32_t adc_val2 = 0;
uint16_t adc_buffer_dma[Num_adc_channel];
uint16_t adc_buffer_dma2[Num_adc_channel];
uint8_t adc_data_ready = 0;


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        adc_buffer_dma2[0] = adc_buffer_dma[0];
        adc_buffer_dma2[1] = adc_buffer_dma[1];
        adc_data_ready = 1;
    }
}
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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	ILI9341_Init();

	//Graph_Init();

	ILI9341_SetRotation(SCREEN_HORIZONTAL_2);

	//ILI9341_FillScreen(WHITE);

//	ILI9341_DrawText("SUNFOX", FONT3, 60, 85, BLUE, WHITE);
//
//	HAL_Delay(500);
//
//	ILI9341_FillScreen(WHITE);

//#if IMAGE_1_ENABLE
//	ILI9341_DrawImage(image_1,SCREEN_HORIZONTAL_2);
//#endif

//#if image2_ENABLE
//	ILI9341_DrawImage(Background, SCREEN_HORIZONTAL_2);
//#endif

#if image3_ENABLE
	ILI9341_DrawImage(Background,SCREEN_HORIZONTAL_2);
#endif


/* Graph Testing */
//
//  ILI9341_DrawHLine(50,120, 220, GREEN);
//
  HAL_Delay(1250);

  ILI9341_FillScreen(BLACK);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);


  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

//	    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_val, 1);
//	    count++;
//
//	    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

	    //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,GPIO_PIN_SET);

if(count >= count_limit &&  adc_data_ready)
{
//	    Graph_Update(adc_val);
//	    Graph_Update2(adc_val2);
	Graph_Update(adc_buffer_dma2[0]);
	Graph_Update2(adc_buffer_dma2[1]);
	printf("%d , %d\r\n",adc_buffer_dma2[0],adc_buffer_dma2[1]);
	//printf("%d\r\n",adc_buffer_dma2[1]);
	    count = 0;
}
	   // Graph_Update2(adc_val);
	   // graph_push(adc_val);

     //	HAL_Delay(10);

//
//		snprintf(adc_text, sizeof(adc_text), "%lu", (unsigned long)adc_val);
//		ILI9341_DrawText(adc_text, FONT3, 100, 85, BLUE, WHITE);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
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

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
