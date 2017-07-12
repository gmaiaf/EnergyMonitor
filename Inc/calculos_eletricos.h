 /**
  *	@file calculos_eletricos.h
  * @author André e Gustavo
  * @brief Implementação das funções que realizam os cálculos dos parâmetros elétricos.
  *
  */

#ifndef CALCULOS_ELETRICOS_H_
#define CALCULOS_ELETRICOS_H_

/**
  * @brief  DEPRECATED Cria um vetor com sinal senoidal para testes.
  * @param  array: vetor seno
  * @param 	f: frequencia
  * @param	fs: frequencia de amostragem
  * @param	n: numero de pontos por periodo
  * @param	phase: fase do seno
  * @retval None
  */
void *retornaSIN(float32_t *array,uint32_t f,float32_t fs, uint32_t n, float32_t phase);
/**
  * @brief  Retorna o valor RMS de um vetor.
  * @param  g: ganho para ajuste do valor (não utilizado).
  * @param 	array: ponteiro para o vetor de dados.
  * @param	size: número de elementos no vetor de dados.
  * @param	start: posição inicial do vetor de dados. Usado para que cálculo ocorra sobre um número inteiro de períodos de 60 Hz.
  * @retval arrayrms: valor RMS do vetor.
  */
float32_t retornaRMS (float32_t g, float32_t *array, uint32_t size,  uint32_t start);
/**
  * @brief  Retorna a potência ativa resultante entre um vetor de tensão e um vetor de corrente.
  * @param  gi: ganho para ajuste do valor de corrente (não utilizado).
  * @param  gv: ganho para ajuste do valor de tensão (não utilizado).
  * @param 	array_tensao: ponteiro para o vetor de dados de tensão.
  * @param 	array_corrente: ponteiro para o vetor de dados de corrente.
  * @param	size: número de elementos no vetor de dados.
  * @param	start: posição inicial do vetor de dados. Usado para que cálculo ocorra sobre um número inteiro de períodos de 60 Hz.
  * @retval potativa: potência ativa calculada.
  */
float32_t retornaPOTATIVA(float32_t gv, float32_t gi, float32_t *array_tensao, float32_t *array_corrente, uint32_t size, uint32_t start);
/**
  * @brief  Retorna a potência aparente resultante a partir dos valores RMS de tensão e corrente já calculados.
  * @param  gi: ganho para ajuste do valor de corrente (não utilizado).
  * @param  gv: ganho para ajuste do valor de tensão (não utilizado).
  * @param 	vrms: tensão RMS.
  * @param 	irms: corrente RMS.
  * @retval potaparente: potência aparente calculada.
  */
float32_t retornaPOTAPARENTE(float32_t gv, float32_t gi, float32_t vrms, float32_t irms);
/**
  * @brief  Retorna a potência reativa resultante a partir dos valores de potência ativa e aparente já calculados.
  * @param 	potaparente: potência aparente.
  * @param 	potativa: potência ativa.
  * @retval potreativa: potência reativa calculada.
  */
float32_t retornaPOTREATIVA(float32_t potaparente,float32_t potativa);
/**
  * @brief  Retorna o fator de potência a partir dos valores de potência ativa e aparente já calculados.
  * @param 	potaparente: potência aparente.
  * @param 	potativa: potência ativa.
  * @retval fp: fator de reativa calculada.
  */
float32_t retornaFP(float32_t potaparente, float32_t potativa);
/**
  * @brief  Retorna a potência das harmônicas de um vetor com dados de corrente.
  * @param 	i_rms_harmonicos: ponteiro para vetor de saída com potência das harmônicas.
  * @param 	array_corrente: ponteiro de entrada com valores dos dados de corrente.
  * @param  g: ganho para ajuste do valor (não utilizado).
  * @param	size: número de elementos no vetor de dados.
  * @param	h: número de elementos no vetor de harmônicas.
  * @param	n: número de ciclos de 60 Hz no vetor de entrada.
  * @param	start: posição inicial do vetor de dados. Usado para que cálculo ocorra sobre um número inteiro de períodos de 60 Hz.
  * @retval None
  */
void retornaRMSHARMONICOS(float32_t *i_rms_harmonicos, float32_t *array_corrente, uint32_t size, uint32_t h, float32_t g, uint32_t n, uint32_t start);
/**
  * @brief  Tira a média de n ciclos para cálculo de harmônicas
  * @param 	array_in: ponteiro com dados de entrada. Tamanho size.
  * @param 	array_out: ponteiro com dados de saída. Tamanho (size-start)/n.
  * @param	size: número de elementos no vetor de dados.
  * @param	n: número de ciclos de 60 Hz no vetor de entrada.
  * @param	start: posição inicial do vetor de dados. Usado para que cálculo ocorra sobre um número inteiro de períodos de 60 Hz.
  * @retval None
  */
void retornaMEDIACICLOS(float32_t *array_in, float32_t *array_out, uint32_t size, uint32_t n, uint32_t start);
/**
  * @brief  Retorna o THD a partir de um vetor de harmônicas.
  * @param 	array_in: ponteiro para vetor de harmônicas.
  * @retval thd: Distorção Harmônica Total calculadas.
  */
float32_t retornaTHD(float32_t *array_in);


#endif /* CALCULOS_ELETRICOS_H_ */
