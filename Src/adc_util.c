/*
 * adc_util.c
 *
 *  Created on: Jun 27, 2017
 *      Author: Gustavo
 */

#include "adc_util.h"

/**
  * @brief  Converte um buffer de entrada uint32_t em float32_t usando a transformacao linear y = a*x + b
  * @param  entrada: buffer de inteiros com valores entre 0 e 4095  (leitura do ADC)
  * @param 	saida: buffer de saida float
  * @param	a: termo linear da equacao y = ax + b
  * @param	b: termo constante da equacao y = ax + b
  * @retval None
  */
void ADCConvertBuffer(uint32_t * entrada, float32_t * saida, uint32_t n, float32_t a, float32_t b)
{
	uint32_t i = 0;

	for(i=0; i<n; i++)
	{
		saida[i] = a*( (float32_t) entrada[i]) + b;
	}

}
