#include "stm32f4xx_hal.h"

#define TILT_SENSOR_PIN GPIO_PIN_0
#define TILT_SENSOR_PORT GPIOA

#define WATER_SENSOR_PIN GPIO_PIN_1
#define WATER_SENSOR_PORT GPIOA

#define TILT_LED_PIN GPIO_PIN_12
#define TILT_LED_PORT GPIOD

#define HIGH_WATER_LED_PIN GPIO_PIN_13
#define HIGH_WATER_LED_PORT GPIOD

#define NO_WATER_LED_PIN GPIO_PIN_14
#define NO_WATER_LED_PORT GPIOD

void SystemClock_Config(void);
void GPIO_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  GPIO_Init();

  while (1)
  {
    // Eğik sensörün titreşip titreşmediğini kontrol et
    if (HAL_GPIO_ReadPin(TILT_SENSOR_PORT, TILT_SENSOR_PIN) == GPIO_PIN_SET)
    {
      HAL_GPIO_WritePin(TILT_LED_PORT, TILT_LED_PIN, GPIO_PIN_SET); // TILT_LED'i aç
    }
    else
    {
      HAL_GPIO_WritePin(TILT_LED_PORT, TILT_LED_PIN, GPIO_PIN_RESET); // TILT_LED'i kapat
    }

    // Su seviyesini kontrol et
    if (HAL_GPIO_ReadPin(WATER_SENSOR_PORT, WATER_SENSOR_PIN) == GPIO_PIN_RESET)
    {
      HAL_GPIO_WritePin(HIGH_WATER_LED_PORT, HIGH_WATER_LED_PIN, GPIO_PIN_SET); // HIGH_WATER_LED'i aç
      HAL_GPIO_WritePin(NO_WATER_LED_PORT, NO_WATER_LED_PIN, GPIO_PIN_RESET);   // NO_WATER_LED'i kapat
    }
    else if (HAL_GPIO_ReadPin(WATER_SENSOR_PORT, WATER_SENSOR_PIN) == GPIO_PIN_SET)
    {
      HAL_GPIO_WritePin(HIGH_WATER_LED_PORT, HIGH_WATER_LED_PIN, GPIO_PIN_RESET); // HIGH_WATER_LED'i kapat
      HAL_GPIO_WritePin(NO_WATER_LED_PORT, NO_WATER_LED_PIN, GPIO_PIN_SET);       // NO_WATER_LED'i aç
    }
    else
    {
      HAL_GPIO_WritePin(HIGH_WATER_LED_PORT, HIGH_WATER_LED_PIN, GPIO_PIN_RESET); // HIGH_WATER_LED'i kapat
      HAL_GPIO_WritePin(NO_WATER_LED_PORT, NO_WATER_LED_PIN, GPIO_PIN_RESET);     // NO_WATER_LED'i kapat
    }
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  // Eğik sensör pini yapılandırması
  GPIO_InitStruct.Pin = TILT_SENSOR_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(TILT_SENSOR_PORT, &GPIO_InitStruct);

  // Su sensörü pini yapılandırması
  GPIO_InitStruct.Pin = WATER_SENSOR_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(WATER_SENSOR_PORT, &GPIO_InitStruct);

  // TILT_LED pini yapılandırması
  GPIO_InitStruct.Pin = TILT_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TILT_LED_PORT, &GPIO_InitStruct);

  // HIGH_WATER_LED pini yapılandırması
  GPIO_InitStruct.Pin = HIGH_WATER_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(HIGH_WATER_LED_PORT, &GPIO_InitStruct);

  // NO_WATER_LED pini yapılandırması
  GPIO_InitStruct.Pin = NO_WATER_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(NO_WATER_LED_PORT, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  while (1)
  {
    // Hata oluştu, burada işleyebilirsiniz veya kendi hata işleme mekanizmanızı ekleyebilirsiniz
  }
}
