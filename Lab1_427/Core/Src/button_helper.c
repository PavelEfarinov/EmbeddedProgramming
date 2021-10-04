/*
 * button_helper.c
 *
 *  Created on: Sep 21, 2021
 *      Author: efarinov
 */

#include "button_helper.h"
#include "gpio.h"

int is_button_pressed() {
	return HAL_GPIO_ReadPin(Button_GPIO_Port, Button_Pin) == GPIO_PIN_RESET ?
			1 : 0;
}
