/*
 * usart_util.h
 *
 *  Created on: May 10, 2017
 *      Author: Gustavo
 *      Board: Nucleo STM32F446RE
 *
 *  Biblioteca com fun��es �teis para utilizar com a USART2
 *
 *  Pinos:
 *  	TX_USART: PA2
 *  	RX_USART: PA3
 */

#ifndef USART_UTIL_H_
#define USART_UTIL_H_

// Fun��es de transmiss�o e recep��o
void USART2_Transmit_Char (char );
void USART2_Transmit_String (char *);
void USART2_Transmit_UInt (uint32_t);
void USART2_Transmit_Int (int);

char USART2_Receive_Command(int );
void USAR2_Receive_Interrupt_Enable(void );

// Fun��o printf light
void print( const char* , ...);


#endif /* USART_UTIL_H_ */
