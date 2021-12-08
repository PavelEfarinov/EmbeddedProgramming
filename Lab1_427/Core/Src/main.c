/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led_helper.h"
#include "uart_helper.h"
#include "button_helper.h"
#include "light_sequence.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int increase_mode(int mode, size_t sequences_number) {
	if (mode >= sequences_number - 1) {
		return 0;
	}
	return mode + 1;
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
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */

	size_t sequences_number = 4;
	size_t place_to_create_new = 4;

	light_sequence_t **sequences = malloc(sizeof(light_sequence_t*) * 8);

	for (int i = 0; i < sequences_number; ++i) {
		sequences[i] = malloc(sizeof(light_sequence_t));
	}
	for (int i = 4; i < 8; ++i) {
		sequences[i] = 0;
	}
	sequences[0]->period = 500;
	sequences[0]->last_tick = 0;
	sequences[0]->sequence_size = 4;
	enum LED_STATE STATE_1[] = { GREEN, RED, GREEN, YELLOW };
	sequences[0]->states = STATE_1;

	sequences[1]->period = 200;
	sequences[1]->last_tick = 0;
	sequences[1]->sequence_size = 3;
	enum LED_STATE STATE_2[] = { GREEN, YELLOW, RED };
	sequences[1]->states = STATE_2;

	sequences[2]->period = 5000;
	sequences[2]->last_tick = 0;
	sequences[2]->sequence_size = 2;
	enum LED_STATE STATE_3[] = { GREEN, RED };
	sequences[2]->states = STATE_3;

	sequences[3]->period = 75;
	sequences[3]->last_tick = 0;
	sequences[3]->sequence_size = 6;
	enum LED_STATE STATE_4[] = { YELLOW, GREEN, YELLOW, RED, GREEN, RED };
	sequences[3]->states = STATE_4;

	int current_mode = 0, button_is_held = 0;
	uint32_t current_tick = HAL_GetTick(), start_tick = HAL_GetTick(),
			sequence_tick = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		current_tick = HAL_GetTick();
		if (current_tick - start_tick < 0) {
			start_tick = HAL_GetTick();
		}
		sequence_tick = (current_tick - start_tick
				+ sequences[current_mode]->last_tick);

		uart_command_result_t uart_command = process_uart_input();

		if (uart_command.result_type == UART_RESULT_SET) {
			int *new_mode = (int*) uart_command.result_data;
			if (*new_mode > 0 && *new_mode <= sequences_number) {
				sequences[current_mode]->last_tick = sequence_tick
						% (sequences[current_mode]->period
								* sequences[current_mode]->sequence_size);
				current_mode = *new_mode - 1;
				start_tick = current_tick;
			}

			free(uart_command.result_data);
		} else if (uart_command.result_type == UART_RESULT_NEW) {
			if (sequences[place_to_create_new] != 0) {
				free(sequences[place_to_create_new]->states);
				free(sequences[place_to_create_new]);
			}
			light_sequence_t *got_sequence =
					(light_sequence_t*) uart_command.result_data;

			sequences[place_to_create_new] = got_sequence;
			sequences[place_to_create_new]->last_tick = 0;

			send_uart_int(sequences[place_to_create_new]->period);
			send_uart_int(sequences[place_to_create_new]->sequence_size);

			if (place_to_create_new == 7) {
				place_to_create_new = 4;
			} else {
				place_to_create_new++;
			}
			if (sequences_number < 8) {
				sequences_number++;
			}
//			send_uart_int(sequences_number);
//			send_uart_int(place_to_create_new);
		}

		int step = (sequence_tick / sequences[current_mode]->period)
				% sequences[current_mode]->sequence_size;

		reset_leds();
		HAL_Delay(1);
		switch (sequences[current_mode]->states[step]) {
		case GREEN:
			turn_on_only_green();
			break;
		case RED:
			turn_on_only_red();
			break;
		case YELLOW:
			turn_on_only_yellow();
			break;
		case BLACK:
			reset_leds();
			break;
		}
		if (is_button_pressed() && !button_is_held) {
			sequences[current_mode]->last_tick = sequence_tick
					% (sequences[current_mode]->period
							* sequences[current_mode]->sequence_size);
			current_mode = increase_mode(current_mode, sequences_number);
			start_tick = current_tick;
			button_is_held = 1;
		} else if (!is_button_pressed() && button_is_held) {
			button_is_held = 0;
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
	while (1) {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
