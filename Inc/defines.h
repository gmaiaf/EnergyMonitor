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
//#define __FPU_PRESENT   1U // ja definido no driver da st

/* Parametros da memoria com historico de medicoes */
#define MEM_SIZE (uint32_t) 4

/* Parametros dos buffers para aquisicao e calculos */
#define BUFFER_SIZE (uint32_t) 2048

#define DIZIMACAO (uint32_t) 8
#define FILTER_TAP_NUM (uint32_t) 45
#define BUFFER_DIZ (uint32_t) (BUFFER_SIZE/DIZIMACAO)

#define F_SAMP (uint32_t) 12000
#define F_SAMP_DIZ (uint32_t) (F_SAMP/DIZIMACAO)

/* Pontos por periodo de 60 Hz*/
#define PPP (uint32_t) (F_SAMP_DIZ / 60)
#define N_START (uint32_t) (BUFFER_DIZ-(BUFFER_DIZ/PPP)*PPP)
#define N_PERIODOS (uint32_t) ((BUFFER_DIZ-N_START)/PPP)

/* Parametros da transformacao linear dos dados do ADC */
// transformar em tensao de 0 a 127V e corrente de acordo com condicionamento
// observar que a tensao tem 180 de fase em relacao a corrente (amplificador inversor)
#define TENSAO_A (float32_t) -0.0992
#define TENSAO_B (float32_t) 200.8922
#define CORRENTE_A (float32_t) 3.3/4095*10
#define CORRENTE_B (float32_t) -2048*CORRENTE_A

/* Parametros matematicos para calculos */
#define RMS_TOLERANCIA (float32_t) 10
#define RMS_UPPERBOUND (float32_t) (100+RMS_TOLERANCIA)/100
#define RMS_LOWERBOUND (float32_t) (100-RMS_TOLERANCIA)/100
#define GV 1
#define GI 1
#define MAX_HARMONICA 10

/* Parametros das estruturas de dados e da memoria de medicoes */
#define EQUIP_ARRAY_MAX 16

#endif /* DEFINES_H_ */
