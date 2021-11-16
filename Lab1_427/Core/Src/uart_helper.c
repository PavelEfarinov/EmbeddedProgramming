#include "uart_helper.h"
#include "usart.h"

char *buffer = 0;
int next_to_fill = 0;

uart_command_result_t process_uart_input() {

	if (buffer == 0) {
		buffer = malloc(1024 * sizeof(char));
	}

	// only synchronous
	size_t size = 1;
	char data[size];
	HAL_StatusTypeDef result = HAL_UART_Receive(&huart6, (uint8_t*) data, size,
			(uint32_t) 1);

	if (result == HAL_OK) {
		buffer[next_to_fill++] = data[0];
		if (data[0] == '\r') {
			buffer[next_to_fill - 1] = '\n';
			buffer[next_to_fill] = '\r';
			buffer[next_to_fill + 1] = 0;
			HAL_UART_Transmit(&huart6, (uint8_t*) buffer, next_to_fill + 1,
					(uint32_t) 1);

			uart_command_result_t uart_result;

			char *command = strstr(buffer, "new");
			if (command == buffer) {
				uart_result.result_type = UART_RESULT_NEW;
			}

			command = strstr(buffer, "set");
			if (command == buffer) {
				uart_result.result_type = UART_RESULT_SET;
				int *position = malloc(sizeof(int));
				*position = atoi(buffer + 4);

				uart_result.result_data = position;
			}
			next_to_fill = 0;
			return uart_result;
		}
	}

	uart_command_result_t uart_result = { UART_RESULT_NONE, 0 };

	return uart_result;
}
