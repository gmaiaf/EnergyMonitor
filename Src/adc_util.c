 /**
  *	@file adc_util.c
  * @author Gustavo
  * @brief Implementação das funções de auxílio no uso dos ADCs.
  *
  */

#include "adc_util.h"

/**
  * @brief  Converte um buffer de entrada uint32_t em float32_t usando a transformação linear y = a*x + b
  * @param  entrada: buffer de inteiros com valores entre 0 e 4095  (leitura do ADC)
  * @param 	saida: buffer de saída float
  * @param	a: termo linear da equação y = ax + b
  * @param	b: termo constante da equação y = ax + b
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
