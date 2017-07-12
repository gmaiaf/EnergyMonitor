 /**
  *	@file adc_util.h
  * @author Gustavo
  * @brief Implementação das funções de auxílio no uso dos ADCs.
  *
  */

#ifndef ADC_UTIL_H_
#define ADC_UTIL_H_

//#define ARM_MATH_CM4
//#define __FPU_PRESENT   1U

#include "main.h"
#include "adc.h"
#include "arm_math.h"

/**
  * @brief  Converte um buffer de entrada uint32_t em float32_t usando a transformação linear y = a*x + b
  * @param  entrada: buffer de inteiros com valores entre 0 e 4095  (leitura do ADC)
  * @param 	saida: buffer de saída float
  * @param	a: termo linear da equação y = ax + b
  * @param	b: termo constante da equação y = ax + b
  * @retval None
  */
void ADCConvertBuffer(uint32_t * entrada, float32_t * saida, uint32_t n, float32_t a, float32_t b);

#endif /* ADC_UTIL_H_ */
