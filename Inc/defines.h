  /**
  *	@file defines.h
  * @author Gustavo
  * @brief Definições dos parâmetros do projeto.
  *
  */

#ifndef DEFINES_H_
#define DEFINES_H_

/* Defines do ARM */
//#define ARM_MATH_CM4
//#define __FPU_PRESENT   1U // ja definido no driver da st

/* Parametros da memoria com historico de medicoes */
/**
  * @brief  Tamanho da memória de histórico de medições.
  */
#define MEM_SIZE (uint32_t) 4

/* Parametros dos buffers para aquisicao e calculos */
/**
  * @brief  Tamanho do buffer de aquisição do ADC.
  */
#define BUFFER_SIZE (uint32_t) 2048
/**
  * @brief  Fator de dizimação.
  */
#define DIZIMACAO (uint32_t) 4
/**
  * @brief  Número de taps do filtro FIR.
  */
#define FILTER_TAP_NUM (uint32_t) 29
/**
  * @brief  Tamanho do buffer dizimado.
  */
#define BUFFER_DIZ (uint32_t) (BUFFER_SIZE/DIZIMACAO)
/**
  * @brief  Fator de conversão de milisegundo para hora.
  */
#define MS2H (float32_t) 1/3600000
/**
  * @brief  Frequência de amostragem do sistema.
  */
#define F_SAMP (uint32_t) 12000
/**
  * @brief  Frequência de amostragem após dizimação.
  */
#define F_SAMP_DIZ (uint32_t) (F_SAMP/DIZIMACAO)

/* Pontos por periodo de 60 Hz*/
/**
  * @brief  Pontos por período.
  */
#define PPP (uint32_t) (F_SAMP_DIZ / 60)
/**
  * @brief  Número de pontos no buffer dizimado que não preenchem um período de 60 Hz.
  */
#define N_START (uint32_t) (BUFFER_DIZ-(BUFFER_DIZ/PPP)*PPP)
/**
  * @brief  Número inteiro de períodos de 60 Hz no buffer dizimado.
  */
#define N_PERIODOS (uint32_t) ((BUFFER_DIZ-N_START)/PPP)

/* Parametros da transformacao linear dos dados do ADC */
// transformar em tensao de 0 a 127V e corrente de acordo com condicionamento
// observar que a tensao tem 180 de fase em relacao a corrente (amplificador inversor)
/**
  * @brief  Fator de conversão A da calibração de tensão.
  */
#define TENSAO_A (float32_t) -0.0992
/**
  * @brief  Fator de conversão B da calibração de tensão.
  */
#define TENSAO_B (float32_t) 200.8922
/**
  * @brief  Fator de conversão A da calibração de corrente.
  */
#define CORRENTE_A (float32_t) 3.3/4095*10
/**
  * @brief  Fator de conversão B da calibração de corrente.
  */
#define CORRENTE_B (float32_t) -2048*CORRENTE_A

/* Parametros matematicos para calculos */
/**
  * @brief  Tolerância do evento de corrente, em %.
  */
#define RMS_TOLERANCIA (float32_t) 7
/**
  * @brief  Limite superior da tolerância do evento de corrente.
  */
#define RMS_UPPERBOUND (float32_t) (100+RMS_TOLERANCIA)/100
/**
  * @brief  Limite inferior da tolerância do evento de corrente.
  */
#define RMS_LOWERBOUND (float32_t) (100-RMS_TOLERANCIA)/100
/**
  * @brief  DEPRECATED Ganho de corrente para ajuste. 
  */
#define GV 1
/**
  * @brief  DEPRECATED Ganho de tensão para ajuste. 
  */
#define GI 1
/**
  * @brief  Número de harmônicas consideradas.
  */
#define MAX_HARMONICA 10

/* Parametros das estruturas de dados e da memoria de medicoes */
/**
  * @brief  Número máximo de equipamentos na base de dados.
  */
#define EQUIP_ARRAY_MAX 16

#endif /* DEFINES_H_ */
