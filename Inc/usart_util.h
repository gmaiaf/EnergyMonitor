  /**
  *	@file usart_util.h
  * @author Gustavo
  * @brief Biblioteca com fun��es �teis para utilizar com a USART2
  *
  */
 
#ifndef USART_UTIL_H_
#define USART_UTIL_H_

// Fun��es de transmiss�o e recep��o
/**
 * @brief Transmite caractere ASCII pela USART 2
 * @param send: caracter a ser transferido pela USART 2
 * @retval None
 */
void USART2_Transmit_Char (char );
/**
 * @brief Transmite string pela USART 2
 * @param send: string a ser transferida pela USART 2
 * @retval None
 */
void USART2_Transmit_String (char *);
/**
 * @brief Transmite inteiro unsigned pela USART 2
 * @param send: inteiro a ser transferido pela USART 2
 * @retval None
 */
void USART2_Transmit_UInt (uint32_t);
/**
 * @brief Transmite inteiro signed pela USART 2
 * @param send: inteiro a ser transferido pela USART 2
 * @retval None
 */
void USART2_Transmit_Int (int);

/**
 * @brief Recebe caracter de comando pela USART2.
 * @param timeout: Timeout em ms.
 * @retval None
 */
char USART2_Receive_Command(int );
/**
 * @brief Habilita interrup��o.
 * @retval None
 */
void USAR2_Receive_Interrupt_Enable(void );
/**
 * @brief Fun��o semelhante � printf usando USART 2. Funciona apenas para %d e %c.
 * @param send: string formatada a ser transferida pela USART 2
 * @retval None
 */
// Fun��o printf light
void print( const char* , ...);


#endif /* USART_UTIL_H_ */
