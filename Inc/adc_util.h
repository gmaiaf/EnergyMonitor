/*
 * adc_util.h
 *
 *  Created on: Jun 26, 2017
 *      Author: Gustavo
 */

#ifndef ADC_UTIL_H_
#define ADC_UTIL_H_

//#define ARM_MATH_CM4
//#define __FPU_PRESENT   1U

#include "main.h"
#include "adc.h"
#include "arm_math.h"

/**
  * @brief  Converte um buffer de entrada uint32_t em float32_t usando a transformacao linear y = a*x + b
  * @param  entrada: buffer de inteiros com valores entre 0 e 4095  (leitura do ADC)
  * @param 	saida: buffer de saida float
  * @param	n: numero de elementos no buffer de entrada e de saida
  * @param	a: termo linear da equacao y = ax + b
  * @param	b: termo constante da equacao y = ax + b
  * @retval None
  */
void ADCConvertBuffer(uint32_t * entrada, float32_t * saida, uint32_t n, float32_t a, float32_t b);

#endif /* ADC_UTIL_H_ */
