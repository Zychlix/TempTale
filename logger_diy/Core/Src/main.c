/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "TempTale.h"
#include "TempTale_LCD.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

TT_COM com = TT_COM_0;
TMP112_t sensor={0};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

extern bmp_t  bmp;

TempTale_t device;
/* USER CODE END PM */



/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

LCD_HandleTypeDef hlcd;

/* USER CODE BEGIN PV */

void TT_enter_standby()
{
//    HAL_PWR_EnterSTANDBYMode();
//    __WFI();
//    HAL_PWR_EnableSleepOnExit();
   // HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);

   // HAL_PWREx_EnableUltraLowPower();
//    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
//    GPIO_InitTypeDef GPIO_InitStruct;
//    GPIO_InitStruct.Pin = START_BUTTON_Pin|STOP_BUTTON_Pin;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);



    MODIFY_REG(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk),0);
    /* Select the regulator state in Stop mode: Set PDDS and LPSDSR bit according to PWR_Regulator value */
    MODIFY_REG(PWR->CR, PWR_CR_PDDS, 0);
    MODIFY_REG(PWR->CSR, PWR_CSR_WUF, 0);
    MODIFY_REG(PWR->CSR, PWR_CSR_EWUP1, 0xffffffff);

    /* Set SLEEPDEEP bit of Cortex System Control Register */

    /* Select Stop mode entry --------------------------------------------------*/

    /* Request Wait For Interrupt */
    __WFI();


//    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_STOPENTRY_WFE);
//    HAL_SuspendTick();
//    __WFI();
//    NVIC_SystemReset();
    HAL_GPIO_TogglePin(LED_2_GPIO_Port,LED_2_Pin);
//    __WFI();
//SetWakeup
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t press_time = HAL_GetTick()-device.button_press_time;

    if(press_time>1000 && device.button_pressed)
    {
        HAL_GPIO_TogglePin(LED_1_GPIO_Port,LED_1_Pin);
        TT_enter_standby();

        if(GPIO_Pin == START_BUTTON_Pin)
        {
            TT_start_button_long_press_handler();
        }

        if(GPIO_Pin == STOP_BUTTON_Pin)
        {
            TT_stop_button_long_press_handler();
        }
    }
    else
    {
        if(GPIO_Pin == START_BUTTON_Pin)
        {
            TT_start_button_click_handler();
        }

        if(GPIO_Pin == STOP_BUTTON_Pin)
        {
            TT_stop_button_click_handler();
        }
    }

    HAL_GPIO_TogglePin(LED_1_GPIO_Port,LED_1_Pin);
    HAL_GPIO_TogglePin(LED_2_GPIO_Port,LED_2_Pin);
    device.button_pressed = 0;
}



int TT_start_button_click_handler()
{
    TT_toggle_mode(&device);
    return 0;
}

int TT_stop_button_click_handler()
{
    //HAL_GPIO_TogglePin(LED_1_GPIO_Port,LED_1_Pin);
    return 0;
}
int TT_start_button_long_press_handler()
{
    HAL_GPIO_TogglePin(LED_2_GPIO_Port,LED_2_Pin);
    TT_toggle_mode(&device);
    return 0;
}

int TT_stop_button_long_press_handler()
{
//    HAL_GPIO_TogglePin(LED_1_GPIO_Port,LED_1_Pin);
    device.enter_standby = 1;
//    TT_enter_standby();
    return 0;
}



int sensors_refresh()
{
    if(TMP_Read(&sensor)!=0)
    {
        // while (1);
    }

    bmp.uncomp.temp = get_ut ();
    bmp.data.temp = get_temp (&bmp);
    bmp.uncomp.press = get_up (bmp.oss);
    bmp.data.press = get_pressure (bmp);
    bmp.data.altitude = get_altitude (&bmp);
    return 0;
}

int button_press_handler()
{
    if(!HAL_GPIO_ReadPin(START_BUTTON_GPIO_Port,START_BUTTON_Pin) || !HAL_GPIO_ReadPin(STOP_BUTTON_GPIO_Port,STOP_BUTTON_Pin))
    {
        if(device.button_pressed)
        {

        } else
        {
            device.button_pressed=1;
            device.button_press_time = HAL_GetTick();
        }

    }
    return 0;
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LCD_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_LCD_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    TT_Display_t lcd;
    lcd.hlcd = &hlcd;
    sensor.i2c = &hi2c1;
    bmp_init(&bmp);

    device.lcd = &lcd;
    device.pressure_sensor = &bmp;
    device.temperature_sensor = &sensor;
    device.state = TT_Pressure;

    sensors_refresh();

    TempTale_init(&device);

    int k=0;

    while (1)
  {
      /* USER CODE END WHILE */

      /* USER CODE BEGIN 3 */

      //HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);



    while(1) {

        sensors_refresh();
        HAL_LCD_Clear(&hlcd);
        TempTale_mode_executor(&device);
        HAL_LCD_UpdateDisplayRequest(&hlcd);

        HAL_Delay(200);

        HAL_GPIO_TogglePin(LED_2_GPIO_Port,LED_2_Pin);

        button_press_handler();

        k++;
        if(k>20)
        {
            //HAL_PWR_EnterSTANDBYMode();
        }

        if(device.enter_standby)
        {
            TT_enter_standby();
            device.enter_standby = 0;
        }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_LCD;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.LCDClockSelection = RCC_RTCCLKSOURCE_LSE;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  * @brief LCD Initialization Function
  * @param None
  * @retval None
  */
static void MX_LCD_Init(void)
{

  /* USER CODE BEGIN LCD_Init 0 */

  /* USER CODE END LCD_Init 0 */

  /* USER CODE BEGIN LCD_Init 1 */

  /* USER CODE END LCD_Init 1 */
  hlcd.Instance = LCD;
  hlcd.Init.Prescaler = LCD_PRESCALER_1;
  hlcd.Init.Divider = LCD_DIVIDER_16;
  hlcd.Init.Duty = LCD_DUTY_1_4;
  hlcd.Init.Bias = LCD_BIAS_1_4;
  hlcd.Init.VoltageSource = LCD_VOLTAGESOURCE_INTERNAL;
  hlcd.Init.Contrast = LCD_CONTRASTLEVEL_3;
  hlcd.Init.DeadTime = LCD_DEADTIME_0;
  hlcd.Init.PulseOnDuration = LCD_PULSEONDURATION_7;
  hlcd.Init.MuxSegment = LCD_MUXSEGMENT_DISABLE;
  hlcd.Init.BlinkMode = LCD_BLINKMODE_OFF;
  hlcd.Init.BlinkFrequency = LCD_BLINKFREQUENCY_DIV8;
  if (HAL_LCD_Init(&hlcd) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the High Driver
  */
  __HAL_LCD_HIGHDRIVER_ENABLE(&hlcd);
  /* USER CODE BEGIN LCD_Init 2 */

  /* USER CODE END LCD_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_1_Pin */
  GPIO_InitStruct.Pin = LED_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : START_BUTTON_Pin STOP_BUTTON_Pin */
  GPIO_InitStruct.Pin = START_BUTTON_Pin|STOP_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


  /*Configure GPIO pin : LED_2_Pin */
  GPIO_InitStruct.Pin = LED_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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

#ifdef  USE_FULL_ASSERT
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
