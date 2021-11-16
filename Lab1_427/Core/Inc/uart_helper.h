#ifndef INC_UART_HELPER_H_
#define INC_UART_HELPER_H_

enum UART_RESULT {
	UART_RESULT_NEW, UART_RESULT_SET, UART_RESULT_NONE, UART_RESULT_SET_IT,
};

typedef struct uart_command_result {
	enum UART_RESULT result_type;
	void *result_data;
} uart_command_result_t;

uart_command_result_t process_uart_input();

#endif /* INC_UART_HELPER_H_ */
