#include "uart_helper.h"
#include "light_sequence.h"

char *buffer = 0;
light_sequence_t *new_sequence_buffered = 0;
int next_to_fill = 0;
int interrupts_enabled = 0;
int can_read = 1;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	can_read = 1;
}

void send_uart(void *message, int size) {
	if (interrupts_enabled) {
		while (HAL_UART_Transmit_IT(&huart6, (uint8_t*) message, size) != HAL_OK)
			;
	} else {
		while (HAL_UART_Transmit(&huart6, (uint8_t*) message, size, 100)
				!= HAL_OK)
			;
	}
}

void send_uart_str(void *message) {
	char *array = malloc(strlen(message) + 2);
	array[0] = 0;
	strcat(array, message);
	strcat(array, "\r\n");
	send_uart(array, strlen(array));
	free(array);
}

void send_uart_int(int message) {
	char array[50];
	itoa(message, array, 10);
	strcat(array, "\r\n");
	send_uart(array, strlen(array));
}

int is_new_command(char *message) {
	char *command = strstr(message, "new");
	return command == message;
}

int is_set_command(char *message) {
	char *command = strstr(message, "set");
	return command == message;
}
int is_set_interrupt_command(char *message) {
	char *command = strstr(message, "set interrupts");
	return command == message;
}

HAL_StatusTypeDef read_uart(char *data, size_t size) {
	HAL_StatusTypeDef result = HAL_BUSY;
	if (interrupts_enabled) {
		if (can_read) {
			can_read = 0;
			HAL_UART_Receive_IT(&huart6, (uint8_t*) &data, size);
		}
	} else {
		result = HAL_UART_Receive(&huart6, (uint8_t*) data, size,
				(uint32_t) 100);
	}
	return result;
}

uart_command_result_t process_uart_input() {

	uart_command_result_t uart_result;
	uart_result.result_type = UART_RESULT_NONE;

	if (buffer == 0) {
		buffer = malloc(1024 * sizeof(char));
		for (int i = 0; i < 1024; i++) {
			buffer[i] = 0;
		}
	}

	size_t size = 1;
	char data[size];
	HAL_StatusTypeDef result = read_uart(data, size);

	if (result == HAL_OK && can_read) {
		send_uart(data, 1);
		if (data[0] == 127) {
			buffer[next_to_fill] = 0;
			next_to_fill--;
		} else {
			buffer[next_to_fill++] = data[0];
		}
		if (data[0] == '\r') {
			strcat(buffer, "\n");

			send_uart("\r\n", 2);
//			send_uart(buffer, strlen(buffer));

			if (new_sequence_buffered != 0) {
				new_sequence_buffered->period = atoi(buffer);
				if (new_sequence_buffered->period != 0) {
					uart_result.result_type = UART_RESULT_NEW;
					uart_result.result_data = new_sequence_buffered;
					new_sequence_buffered = 0;
					send_uart_str("New sequence is being created");
				} else {
					send_uart_str("Please, send sequence period:");
				}
			}
			else if (is_new_command(buffer)) {
				send_uart_str("NEW command");
				int sequence_size = strlen(buffer) - 6;
				new_sequence_buffered = malloc(sizeof(light_sequence_t));
				new_sequence_buffered->states = malloc(
						sizeof(enum LED_STATE) * sequence_size);
				new_sequence_buffered->sequence_size = sequence_size;
				new_sequence_buffered->last_tick = 0;

				for (int i = 0; i < sequence_size; i++) {
					switch (buffer[4 + i]) {
					case 'r':
						new_sequence_buffered->states[i] = RED;
						break;
					case 'g':
						new_sequence_buffered->states[i] = GREEN;
						break;
					case 'y':
						new_sequence_buffered->states[i] = YELLOW;
						break;
					default:
						new_sequence_buffered->states[i] = BLACK;
					};
				}
				send_uart_str("Please, send sequence period:");

			} else if (is_set_interrupt_command(buffer)) {
				send_uart_str("SET_IT command");
				char *interrupt_on_str = strstr(buffer, " on");
				if (interrupt_on_str != 0) {
					interrupts_enabled = 0;
					HAL_NVIC_EnableIRQ(USART6_IRQn);
					send_uart_str("SET_IT ON");
				} else {
					interrupts_enabled = 0;
					HAL_UART_Abort_IT(&huart6);
					HAL_NVIC_DisableIRQ(USART6_IRQn);
					send_uart_str("SET_IT OFF");
				}
			} else if (is_set_command(buffer)) {
				send_uart_str("SET command");
				uart_result.result_type = UART_RESULT_SET;
				int *position = malloc(sizeof(int));
				*position = atoi(buffer + 4);
				uart_result.result_data = position;
			} else {
				send_uart_str("No such command");
			}

			next_to_fill = 0;
			for (int i = 0; i < 1024; i++) {
				buffer[i] = 0;
			}
		}
	}
	can_read = 1;
	return uart_result;
}
