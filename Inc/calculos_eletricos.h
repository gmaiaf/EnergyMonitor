 /**
  *	@file calculos_eletricos.h
  * @author Andr� e Gustavo
  * @brief Implementa��o das fun��es que realizam os c�lculos dos par�metros el�tricos.
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
  * @param  g: ganho para ajuste do valor (n�o utilizado).
  * @param 	array: ponteiro para o vetor de dados.
  * @param	size: n�mero de elementos no vetor de dados.
  * @param	start: posi��o inicial do vetor de dados. Usado para que c�lculo ocorra sobre um n�mero inteiro de per�odos de 60 Hz.
  * @retval arrayrms: valor RMS do vetor.
  */
float32_t retornaRMS (float32_t g, float32_t *array, uint32_t size,  uint32_t start);
/**
  * @brief  Retorna a pot�ncia ativa resultante entre um vetor de tens�o e um vetor de corrente.
  * @param  gi: ganho para ajuste do valor de corrente (n�o utilizado).
  * @param  gv: ganho para ajuste do valor de tens�o (n�o utilizado).
  * @param 	array_tensao: ponteiro para o vetor de dados de tens�o.
  * @param 	array_corrente: ponteiro para o vetor de dados de corrente.
  * @param	size: n�mero de elementos no vetor de dados.
  * @param	start: posi��o inicial do vetor de dados. Usado para que c�lculo ocorra sobre um n�mero inteiro de per�odos de 60 Hz.
  * @retval potativa: pot�ncia ativa calculada.
  */
float32_t retornaPOTATIVA(float32_t gv, float32_t gi, float32_t *array_tensao, float32_t *array_corrente, uint32_t size, uint32_t start);
/**
  * @brief  Retorna a pot�ncia aparente resultante a partir dos valores RMS de tens�o e corrente j� calculados.
  * @param  gi: ganho para ajuste do valor de corrente (n�o utilizado).
  * @param  gv: ganho para ajuste do valor de tens�o (n�o utilizado).
  * @param 	vrms: tens�o RMS.
  * @param 	irms: corrente RMS.
  * @retval potaparente: pot�ncia aparente calculada.
  */
float32_t retornaPOTAPARENTE(float32_t gv, float32_t gi, float32_t vrms, float32_t irms);
/**
  * @brief  Retorna a pot�ncia reativa resultante a partir dos valores de pot�ncia ativa e aparente j� calculados.
  * @param 	potaparente: pot�ncia aparente.
  * @param 	potativa: pot�ncia ativa.
  * @retval potreativa: pot�ncia reativa calculada.
  */
float32_t retornaPOTREATIVA(float32_t potaparente,float32_t potativa);
/**
  * @brief  Retorna o fator de pot�ncia a partir dos valores de pot�ncia ativa e aparente j� calculados.
  * @param 	potaparente: pot�ncia aparente.
  * @param 	potativa: pot�ncia ativa.
  * @retval fp: fator de reativa calculada.
  */
float32_t retornaFP(float32_t potaparente, float32_t potativa);
/**
  * @brief  Retorna a pot�ncia das harm�nicas de um vetor com dados de corrente.
  * @param 	i_rms_harmonicos: ponteiro para vetor de sa�da com pot�ncia das harm�nicas.
  * @param 	array_corrente: ponteiro de entrada com valores dos dados de corrente.
  * @param  g: ganho para ajuste do valor (n�o utilizado).
  * @param	size: n�mero de elementos no vetor de dados.
  * @param	h: n�mero de elementos no vetor de harm�nicas.
  * @param	n: n�mero de ciclos de 60 Hz no vetor de entrada.
  * @param	start: posi��o inicial do vetor de dados. Usado para que c�lculo ocorra sobre um n�mero inteiro de per�odos de 60 Hz.
  * @retval None
  */
void retornaRMSHARMONICOS(float32_t *i_rms_harmonicos, float32_t *array_corrente, uint32_t size, uint32_t h, float32_t g, uint32_t n, uint32_t start);
/**
  * @brief  Tira a m�dia de n ciclos para c�lculo de harm�nicas
  * @param 	array_in: ponteiro com dados de entrada. Tamanho size.
  * @param 	array_out: ponteiro com dados de sa�da. Tamanho (size-start)/n.
  * @param	size: n�mero de elementos no vetor de dados.
  * @param	n: n�mero de ciclos de 60 Hz no vetor de entrada.
  * @param	start: posi��o inicial do vetor de dados. Usado para que c�lculo ocorra sobre um n�mero inteiro de per�odos de 60 Hz.
  * @retval None
  */
void retornaMEDIACICLOS(float32_t *array_in, float32_t *array_out, uint32_t size, uint32_t n, uint32_t start);
/**
  * @brief  Retorna o THD a partir de um vetor de harm�nicas.
  * @param 	array_in: ponteiro para vetor de harm�nicas.
  * @retval thd: Distor��o Harm�nica Total calculadas.
  */
float32_t retornaTHD(float32_t *array_in);


#endif /* CALCULOS_ELETRICOS_H_ */
