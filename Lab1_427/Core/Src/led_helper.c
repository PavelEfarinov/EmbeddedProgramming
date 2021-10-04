/*
 * led_helper.c
 *
 *  Created on: Sep 21, 2021
 *      Author: efarinov
 */
#include "led_helper.h"
#include "gpio.h"

void turn_on_green() {
	HAL_GPIO_WritePin(Green_Led_GPIO_Port, Green_Led_Pin, GPIO_PIN_SET);
}

void turn_off_green() {
	HAL_GPIO_WritePin(Green_Led_GPIO_Port, Green_Led_Pin, GPIO_PIN_RESET);
}

void turn_on_red() {
	HAL_GPIO_WritePin(Red_Led_GPIO_Port, Red_Led_Pin, GPIO_PIN_SET);
}

void turn_on_only_red() {
	turn_off_yellow();
	turn_off_green();
	turn_on_red();
}

void turn_on_only_green() {
	turn_off_red();
	turn_off_yellow();
	turn_on_green();
}

void turn_on_only_yellow() {
	turn_off_red();
	turn_on_yellow();
	turn_off_green();
}

void turn_off_red() {
	HAL_GPIO_WritePin(Red_Led_GPIO_Port, Red_Led_Pin, GPIO_PIN_RESET);
}

void turn_on_yellow() {
	HAL_GPIO_WritePin(Yellow_Led_GPIO_Port, Yellow_Led_Pin, GPIO_PIN_SET);
}

void turn_off_yellow() {
	HAL_GPIO_WritePin(Yellow_Led_GPIO_Port, Yellow_Led_Pin, GPIO_PIN_RESET);
}

