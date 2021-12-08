#ifndef INC_UART_HELPER_H_
#define INC_UART_HELPER_H_
#include "usart.h"

enum UART_RESULT {
	NULL_UART, UART_RESULT_NEW, UART_RESULT_SET, UART_RESULT_NONE,
};

typedef struct uart_command_result {
	enum UART_RESULT result_type;
	void *result_data;
} uart_command_result_t;

HAL_StatusTypeDef read_uart(char *data, size_t size);

void send_uart(void* message, int size);
void send_uart_str(void* message);
void send_uart_int(int message);

int is_new_command(char* message);
int is_set_command(char* message);
int is_set_interrupt_command(char* message);

uart_command_result_t process_uart_input();

#endif /* INC_UART_HELPER_H_ */
