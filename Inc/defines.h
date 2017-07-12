  /**
  *	@file defines.h
  * @author Gustavo
  * @brief Defini��es dos par�metros do projeto.
  *
  */

#ifndef DEFINES_H_
#define DEFINES_H_

/* Defines do ARM */
//#define ARM_MATH_CM4
//#define __FPU_PRESENT   1U // ja definido no driver da st

/* Parametros da memoria com historico de medicoes */
/**
  * @brief  Tamanho da mem�ria de hist�rico de medi��es.
  */
#define MEM_SIZE (uint32_t) 4

/* Parametros dos buffers para aquisicao e calculos */
/**
  * @brief  Tamanho do buffer de aquisi��o do ADC.
  */
#define BUFFER_SIZE (uint32_t) 2048
/**
  * @brief  Fator de dizima��o.
  */
#define DIZIMACAO (uint32_t) 4
/**
  * @brief  N�mero de taps do filtro FIR.
  */
#define FILTER_TAP_NUM (uint32_t) 29
/**
  * @brief  Tamanho do buffer dizimado.
  */
#define BUFFER_DIZ (uint32_t) (BUFFER_SIZE/DIZIMACAO)
/**
  * @brief  Fator de convers�o de milisegundo para hora.
  */
#define MS2H (float32_t) 1/3600000
/**
  * @brief  Frequ�ncia de amostragem do sistema.
  */
#define F_SAMP (uint32_t) 12000
/**
  * @brief  Frequ�ncia de amostragem ap�s dizima��o.
  */
#define F_SAMP_DIZ (uint32_t) (F_SAMP/DIZIMACAO)

/* Pontos por periodo de 60 Hz*/
/**
  * @brief  Pontos por per�odo.
  */
#define PPP (uint32_t) (F_SAMP_DIZ / 60)
/**
  * @brief  N�mero de pontos no buffer dizimado que n�o preenchem um per�odo de 60 Hz.
  */
#define N_START (uint32_t) (BUFFER_DIZ-(BUFFER_DIZ/PPP)*PPP)
/**
  * @brief  N�mero inteiro de per�odos de 60 Hz no buffer dizimado.
  */
#define N_PERIODOS (uint32_t) ((BUFFER_DIZ-N_START)/PPP)

/* Parametros da transformacao linear dos dados do ADC */
// transformar em tensao de 0 a 127V e corrente de acordo com condicionamento
// observar que a tensao tem 180 de fase em relacao a corrente (amplificador inversor)
/**
  * @brief  Fator de convers�o A da calibra��o de tens�o.
  */
#define TENSAO_A (float32_t) -0.0992
/**
  * @brief  Fator de convers�o B da calibra��o de tens�o.
  */
#define TENSAO_B (float32_t) 200.8922
/**
  * @brief  Fator de convers�o A da calibra��o de corrente.
  */
#define CORRENTE_A (float32_t) 3.3/4095*10
/**
  * @brief  Fator de convers�o B da calibra��o de corrente.
  */
#define CORRENTE_B (float32_t) -2048*CORRENTE_A

/* Parametros matematicos para calculos */
/**
  * @brief  Toler�ncia do evento de corrente, em %.
  */
#define RMS_TOLERANCIA (float32_t) 7
/**
  * @brief  Limite superior da toler�ncia do evento de corrente.
  */
#define RMS_UPPERBOUND (float32_t) (100+RMS_TOLERANCIA)/100
/**
  * @brief  Limite inferior da toler�ncia do evento de corrente.
  */
#define RMS_LOWERBOUND (float32_t) (100-RMS_TOLERANCIA)/100
/**
  * @brief  DEPRECATED Ganho de corrente para ajuste. 
  */
#define GV 1
/**
  * @brief  DEPRECATED Ganho de tens�o para ajuste. 
  */
#define GI 1
/**
  * @brief  N�mero de harm�nicas consideradas.
  */
#define MAX_HARMONICA 10

/* Parametros das estruturas de dados e da memoria de medicoes */
/**
  * @brief  N�mero m�ximo de equipamentos na base de dados.
  */
#define EQUIP_ARRAY_MAX 16

#endif /* DEFINES_H_ */
