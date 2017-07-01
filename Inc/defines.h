/*
 * defines.h
 *
 *  Created on: Jun 25, 2017
 *      Author: Gustavo
 */

#ifndef DEFINES_H_
#define DEFINES_H_

/* Defines do ARM */
//#define ARM_MATH_CM4
//#define __FPU_PRESENT   1 // ja definido no driver da st

/* Parametros da memoria com historico de medicoes */
#define MEM_SIZE 4

/* Parametros dos buffers para aquisicao e calculos */
#define BUFFER_SIZE 256
#define DIZIMACAO 8
#define BUFFER_DIZ BUFFER_SIZE/DIZIMACAO
#define F_SAMP 21875
#define F_SAMP_DIZ F_SAMP/DIZIMACAO
#define N_MAX 230

/* Parametros da transformacao linear dos dados do ADC */
// transformar em tensao de 0 a 127V e corrente de acordo com condicionamento
// observar que a tensao tem 180 de fase em relacao a corrente (amplificador inversor)
#define TENSAO_A 3.3/4095
#define TENSAO_B 0
#define CORRENTE_A 3.3/4095
#define CORRENTE_B 0

/* Parametros matematicos para calculos */
#define RMS_TOLERANCIA 5
#define RMS_UPPERBOUND (100+RMS_TOLERANCIA)/100
#define RMS_LOWERBOUND (100-RMS_TOLERANCIA)/100
#define GV 1
#define GI 1
#define MAX_HARMONICA 10

/* Parametros das estruturas de dados e da memoria de medicoes */
#define EQUIP_ARRAY_MAX 16

#endif /* DEFINES_H_ */
