  /**
  *	@file usart_util.c
  * @author Gustavo
  * @brief Biblioteca com funções úteis para utilizar com a USART2
  *
  */

#include "usart.h"
#include "usart_util.h"
#include "string.h"
#include <stdarg.h>

/**
 * @brief Transmite caractere ASCII pela USART 2
 * @param send: caracter a ser transferido pela USART 2
 * @retval None
 */
void USART2_Transmit_Char (char send)
{
	HAL_StatusTypeDef Status;
	/*
	 * Funcao HAL para transmitir bytes
	 * 	huart2: ponteiro para estrutura uart2 criada em usart.h
	 *	send: char a ser transmitido
	 */

	Status = HAL_UART_Transmit(&huart2, (uint8_t *) &send, (uint16_t)1, 100);

	switch(Status)
	{
		case HAL_OK:
			// no errors
			break;
		case HAL_ERROR:
			// print hal error
			break;
		case HAL_BUSY:
			// print hal busy
			break;
		case HAL_TIMEOUT:
			// print hal timeout
			break;
	}
}

/**
 * @brief Transmite string pela USART 2
 * @param send: string a ser transferida pela USART 2
 * @retval None
 */
void USART2_Transmit_String (char * send)
{
	HAL_StatusTypeDef Status;
	/*
	 * Funcao HAL para transmitir bytes
	 * 	huart2: ponteiro para estrutura uart2 criada em usart.h
	 *	send: ponteiro para primeiro byte a ser transmitido
	 *	strlen(send): numero de bytes a ser transmitido
	 */

	Status = HAL_UART_Transmit(&huart2, (uint8_t *) send, (uint16_t)strlen(send), 100);

	switch(Status)
	{
		case HAL_OK:
			// no errors
			break;
		case HAL_ERROR:
			// print hal error
			break;
		case HAL_BUSY:
			// print hal busy
			break;
		case HAL_TIMEOUT:
			// print hal timeout
			break;
	}
}

/**
 * @brief Transmite inteiro unsigned pela USART 2
 * @param send: inteiro a ser transferido pela USART 2
 * @retval None
 */
void USART2_Transmit_UInt (uint32_t send)
{
	HAL_StatusTypeDef Status;

	char value[10]; // array temporario
	int i = 0; //loop index

	do // obter digitos do menos significativo para o mais significativo
	{
		value[i] = (char)(send % 10) + '0'; //converter resto da divisao por 10
		send /= 10; // dividir por 10
		i++;
	} while(send); // parar quando divisao por 10 resultar em 0



	while(i) //enviar dados do mais significativo para o menos significativo
	{
		/*
		 * Funcao HAL para transmitir bytes
		 * 	huart2: ponteiro para estrutura uart2 criada em usart.h
		 *	send: ponteiro para primeiro byte a ser transmitido
		 *	strlen(send): numero de bytes a ser transmitido
		 */
		Status = HAL_UART_Transmit(&huart2, (uint8_t *) &value[--i], 1, 100);

		switch(Status)
		{
			case HAL_OK:
				// no errors
				break;
			case HAL_ERROR:
				// print hal error
				break;
			case HAL_BUSY:
				// print hal busy
				break;
			case HAL_TIMEOUT:
				// print hal timeout
				break;
		}
	}
}

/**
 * @brief Transmite inteiro signed pela USART 2
 * @param send: inteiro a ser transferido pela USART 2
 * @retval None
 */
void USART2_Transmit_Int (int send)
{
	uint32_t number;

	if (send >= 0)
	{
		number = (uint32_t) send;
	}
	else
	{
		USART2_Transmit_Char('-');
		number = (uint32_t) ((-1)*send);
	}

	USART2_Transmit_UInt(number);
}


/**
 * @brief Função semelhante à printf usando USART 2. Funciona apenas para %d e %c.
 * @param send: string formatada a ser transferida pela USART 2
 * @retval None
 */
/*
 * Funcao: void printf( const char* , ...)
 * Entrada: char *
 * Saida: void
 * Descricao: envia string formatada send usando UART2.
 *
 * DONE:
 *	string original
 *	integer d
 *	char c
 * TO DO:
 * 	long int ld
 * 	string %s
 * 	hexa %x
 * 	precisao (.)
 * 	0 padding
 *
*/
void print( const char* send, ...)
{
	char ch;
	va_list argp;

	va_start( argp, send);

	for ( ; *send; send++) {
		// avançar na string até encontrar %
		if (*send != '%') {
			USART2_Transmit_Char(*send);
			continue;
		}

		// % encontrado, tratar especificador

		// inicializar possiveis flags

		try_next:
		ch = *(++send);

		// tratar possiveis flags

		switch (ch) {
			case '%':
				USART2_Transmit_Char('%');
				continue;
			case '-':
				// tratar flag -
				break;
			case '.':
				// tratar precisão
				break;
			case 'l':
				// tratar long int
				break;
			case 'd':
				USART2_Transmit_Int(va_arg(argp, int));
				continue;
			case 'x':
				// tratar hexadecimal
				continue;
			case 's':
				// tratar string
				continue;
			case 'c':
				USART2_Transmit_Char(va_arg(argp, int));
				continue;
			/*case '\\':
				// tratar simbolos ascii (\n\r etc)
				switch (*send) {
					case 'a':
						USART2_Transmit_Char('BEL');
						break;
					case 'h':
						USART2_Transmit_Char('BS');
						break;
					case 'r':
						USART2_Transmit_Char('CR');
						break;
					case 'n':
						USART2_Transmit_Char('CR');
						USART2_Transmit_Char('LF');
						break;
					default:
						USART2_Transmit_Char(*send);
						break;
				}
				send++;
				break;*/
			default:
				continue;
		}
		goto try_next;
	}
	va_end( argp);
}

/**
 * @brief Recebe caracter de comando pela USART2.
 * @param timeout: Timeout em ms.
 * @retval None
 */
/*
 * Funcao: void USART2_Receive_Command (void)
 * Entrada: int timeout
 * Saida: char - comando
 * Descricao: Recebe comando por char
 */
char USART2_Receive_Command (int timeout)
{
	uint8_t command;
	HAL_StatusTypeDef Status;

	/*
	 * Funcao HAL para receber bytes
	 * 	huart2: ponteiro para estrutura uart2 criada em usart.h
	 *	receive: ponteiro para primeiro byte a ser recebido
	 *	strlen(send): numero de bytes a ser recebido
	 */

	Status = HAL_UART_Receive(&huart2, &command, (uint16_t) 1, (uint32_t) timeout);

	switch(Status)
	{
		case HAL_OK:
			// no errors
			break;
		case HAL_ERROR:
			// print hal error
			break;
		case HAL_BUSY:
			// print hal busy
			break;
		case HAL_TIMEOUT:
			print("Receive Timeout\n\r");
			break;
	}

	return (char)command;
}

/**
 * @brief Habilita interrupção.
 * @retval None
 */
/*
 * Funcao: void USAR2_Receive_Interrupt_Enable (void)
 * Entrada: void
 * Saida: void
 * Descricao: ativa interrupcao
 */
void USAR2_Receive_Interrupt_Enable(void )
{
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
}
