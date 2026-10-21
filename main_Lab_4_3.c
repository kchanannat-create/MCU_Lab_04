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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c-lcd.h"
#include <stdio.h>
#include <string.h>
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
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t display_state = 0;
uint8_t force_update = 1;

// ตัวแปรสำหรับการเลื่อนข้อความ (Sculling Text)
uint32_t last_scroll_tick = 0;
int scroll_pos = 0;
int scroll_dir = 1;
char scroll_text[] = "<< Press button to show >>";

// ฟังก์ชันนี้จะถูกเรียกใน while(1)
void process_lcd_state(void)
{
  if (display_state == 0) // หน้าแรก
  {
    if (force_update)
    {
      lcd_clear();
      lcd_put_cur(0, 0);
      lcd_send_string("Member List");
      force_update = 0;
    }

    // อัปเดตข้อความเลื่อนบรรทัดที่ 2 ทุกๆ 300ms โดยไม่ใช้ HAL_Delay
    if (HAL_GetTick() - last_scroll_tick > 300)
    {
      last_scroll_tick = HAL_GetTick();

      // เรียกใช้ฟังก์ชันเลื่อนอักษรที่ประกาศไว้ใน i2c-lcd.h
      lcd_scrolling_text(1, scroll_text, scroll_pos, 16);

      scroll_pos += scroll_dir;

      // คำนวณขอบเขตการเลื่อน: (ความยาวข้อความ - ความกว้างจอ)
      int max_pos = strlen(scroll_text) - 16;
      if (scroll_pos >= max_pos) {
        scroll_pos = max_pos;
        scroll_dir = -1; // ชนขอบขวา ให้เลื่อนกลับซ้าย
      } else if (scroll_pos <= 0) {
        scroll_pos = 0;
        scroll_dir = 1;  // ชนขอบซ้าย ให้เลื่อนไปขวา
      }
    }
  }
  else if (display_state == 1 && force_update)
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("Y.Thathinon");
    lcd_put_cur(1, 0);
    lcd_send_string("116830462015-0");
    force_update = 0;
  }
  else if (display_state == 2 && force_update)
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("W.Theeramet");
    lcd_put_cur(1, 0);
    lcd_send_string("116830462011-9");
    force_update = 0;
  }
  else if (display_state == 3 && force_update)
  {
    lcd_clear();
    lcd_put_cur(0, 0);
    lcd_send_string("N.Tanapat");
    lcd_put_cur(1, 0);
    lcd_send_string("116830462012-7");
    force_update = 0;
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
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */
  lcd_init();               // เริ่มการทำงานของจอ LCD
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
    {
      /* USER CODE END WHILE */

      /* USER CODE BEGIN 3 */
      process_lcd_state(); // เรียกจัดการหน้าจอแสดงผล

      // --- ตรวจสอบการกดปุ่ม 1 (PA0: สลับรายชื่อสมาชิก) ---
      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
      {
        if (display_state == 0) {
            display_state = 1; // เริ่มจาก 0 ไป 1
        }
        else if (display_state == 1) {
            display_state = 2; // ไป 2
        }
        else if (display_state == 2) {
            display_state = 3; // ไป 3
        }
        else if (display_state == 3) {
            display_state = 1; // วนกลับมา 1
        }

        force_update = 1;
        HAL_Delay(200); // หน่วงเวลาป้องกันสัญญาณปุ่มเด้งเบิ้ล (Debounce)
      }

      // --- ตรวจสอบการกดปุ่ม 2 (PA1: กลับหน้าแรก) ---
      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET)
      {
        if (display_state != 0) {
            display_state = 0;
            scroll_pos = 0;
            scroll_dir = 1;
            force_update = 1;
        }
        HAL_Delay(200); // หน่วงเวลาป้องกันสัญญาณปุ่มเด้งเบิ้ล (Debounce)
      }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
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

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* ตั้งค่า PA0 (ปุ่ม 1 - เลื่อนขวา) และ PA1 (ปุ่ม 2 - เลื่อนซ้าย) เป็น Input แบบ Pull-Up */
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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

