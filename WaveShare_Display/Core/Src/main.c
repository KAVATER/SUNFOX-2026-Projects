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
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "lvgl.h"
//#include "snow_tiger.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define Horizontal_Resolution 320
#define Vertical_Resolution 240

#define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static uint8_t buf1[Horizontal_Resolution * Vertical_Resolution / 10 * BYTES_PER_PIXEL];
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void my_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    uint32_t size = w * h * 2;   // RGB565 = 2 bytes per pixel

    ILI9341_Set_Address(area->x1, area->y1, area->x2, area->y2);
    ILI9341_WriteBuffer(px_map, size);

    lv_display_flush_ready(display);
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
   int16_t AD_RES = 0;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
 // MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
	ILI9341_Init(); //initializing drivers
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

//	// Set rotation
ILI9341_Set_Rotation(SCREEN_HORIZONTAL_2);
//
//	//  fill screen with white background
	ILI9341_Fill_Screen(BLACK);
	HAL_Delay(1000);
	ILI9341_Fill_Screen(WHITE);
	HAL_Delay(1000);

//
//	ILI9341_Draw_Text("SUNFOX", 75, 100, BLUE, 4, WHITE);
//	HAL_Delay(2000);
	
	// LVGL
	lv_init();

	/* Use HAL SysTick as LVGL time source */
	lv_tick_set_cb(HAL_GetTick);
	
	lv_display_t * display1 = lv_display_create(Horizontal_Resolution, Vertical_Resolution);
	lv_display_set_color_format(display1, LV_COLOR_FORMAT_RGB565_SWAPPED);
	lv_display_set_buffers(display1, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

	lv_display_set_flush_cb(display1, my_flush_cb);

	/* Create a screen object */
	lv_obj_t * scr = lv_screen_active();

	/* Create a label */
	lv_obj_t * label = lv_label_create(scr);

		  /* Set the text */
		  lv_label_set_text(label, "SUNFOX");

		  	/* Set the text color and font */
		  	lv_obj_set_style_text_color(label,
		  	                            lv_color_hex(0x0000FF),
		  	                            LV_PART_MAIN);

		  	lv_obj_set_style_text_font(label,
		  	                           &lv_font_montserrat_30,
		  	                           LV_PART_MAIN);

		  	/* Center the label */
		  	lv_obj_center(label);

//  HAL_ADC_Start(&hadc1);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  uint32_t time_till_next =lv_timer_handler();
	  HAL_Delay(time_till_next);
//
//	  HAL_ADC_PollForConversion(&hadc1, 1);
//	  AD_RES = HAL_ADC_GetValue(&hadc1);
//
//	  /* Set the text */
//	  lv_label_set_text(label, "SUNFOX");
//	 // lv_label_set_text_fmt(label,"%d", AD_RES);
//
//	  	/* Set the text color and font */
//	  	lv_obj_set_style_text_color(label,
//	  	                            lv_color_hex(0x0000FF),
//	  	                            LV_PART_MAIN);
//
//	  	lv_obj_set_style_text_font(label,
//	  	                           &lv_font_montserrat_30,
//	  	                           LV_PART_MAIN);
//
//	  	/* Center the label */
//	  	lv_obj_center(label);

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
