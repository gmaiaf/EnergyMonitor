/*
 * defines.h
 *
 *  Created on: Jun 25, 2017
 *      Author: Gustavo
 */

#ifndef DEFINES_H_
#define DEFINES_H_

/* Defines do ARM */
#define ARM_MATH_CM4
#define __FPU_PRESENT   1

/* Parametros da memoria com historico de medicoes */
#define MEM_SIZE 8

/* Parametros dos buffers para aquisicao e calculos */
#define BUFFER_SIZE 256
#define DIZIMACAO 8
#define F_SAMP 21875
#define F_SAMP_DIZ F_SAMP/DIZIMACAO
#define N_MAX 230

/* Parametros da transformacao linear dos dados do ADC */
// transformar em tensao de 0 a 127V e corrente de acordo com condicionamento
#define TENSAO_A 3.3/4095
#define TENSAO_B 0
#define CORRENTE_A 3.3/4095
#define CORRENTE_B 0

/* Parametros matematicos para calculos */
#define RMS_TOLERANCIA 5
#define RMS_UPPERBOUND (100+RMS_TOLERANCIA)/100
#define RMS_LOWERBOUND (100-RMS_TOLERANCIA)/100

#endif /* DEFINES_H_ */
