/*
 * button_helper.c
 *
 *  Created on: Sep 21, 2021
 *      Author: efarinov
 */

#include "button_helper.h"
#include "gpio.h"

int is_button_pressed() {
	static int prev_state = 0;
	static int last_release = 0;
	int current_tick = HAL_GetTick();
	int current_state = HAL_GPIO_ReadPin(Button_GPIO_Port, Button_Pin) == GPIO_PIN_RESET ?
			1 : 0;

	if(prev_state && current_state)
	{
		if(current_tick - last_release > 50)
		{
			return 1;
		}
		return 0;
	}
	else
	{
		prev_state = current_state;
		last_release = current_tick;
		return 0;
	}
}
