 /**
  *	@file calculos_eletricos.c
  * @author André e Gustavo
  * @brief Implementação das funções que realizam os cálculos dos parâmetros elétricos.
  *
  */

//#define ARM_MATH_CM4
//#define __FPU_PRESENT   1U
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "arm_math.h"
#include "defines.h"
#define PI_VALUE (float32_t) 3.14159265358
#define SQRT2 (float32_t) 1.414213562373095

/**
  * @brief  DEPRECATED Cria um vetor com sinal senoidal para testes.
  * @param  array: vetor seno
  * @param 	f: frequencia
  * @param	fs: frequencia de amostragem
  * @param	n: numero de pontos por periodo
  * @param	phase: fase do seno
  * @retval None
  */
void *retornaSIN(float32_t *array, uint32_t f, float32_t fs, uint32_t n, float32_t phase){

	uint32_t size,j;
	float32_t phase_rad = phase*PI_VALUE/180;
	size = (uint32_t) roundf((fs/f)*n);

	for (j=0;j<size;j++) {
		array[j] = arm_sin_f32(2*PI_VALUE*f*j*(1/fs)+phase_rad);
	}

return array;
}

/**
  * @brief  Retorna o valor RMS de um vetor.
  * @param  g: ganho para ajuste do valor (não utilizado).
  * @param 	array: ponteiro para o vetor de dados.
  * @param	size: número de elementos no vetor de dados.
  * @param	start: posição inicial do vetor de dados. Usado para que cálculo ocorra sobre um número inteiro de períodos de 60 Hz.
  * @retval arrayrms: valor RMS do vetor.
  */
float32_t retornaRMS (float32_t g, float32_t *array, uint32_t size, uint32_t start){

	uint32_t i;
	float32_t arrayrms ;
	float32_t arraysum =0;

    for(i=start;i<size;i++){
        arraysum = arraysum + array[i]*array[i];
    }
    arraysum = arraysum / (size-start);
    arm_sqrt_f32(arraysum, &arrayrms);
    arrayrms = g * arrayrms;

    return arrayrms;
}

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
float32_t retornaPOTATIVA(float32_t gv, float32_t gi, float32_t *array_tensao, float32_t *array_corrente, uint32_t size, uint32_t start){

	float32_t potativa,potativasum =0;
    uint32_t i;

    for(i=start;i<size;i++){
        potativasum = potativasum + (*(array_corrente+i) * (*(array_tensao+i)));
    }
    potativa = gv*gi*potativasum/(size-start);

    return potativa;

}

/**
  * @brief  Retorna a potência aparente resultante a partir dos valores RMS de tensão e corrente já calculados.
  * @param  gi: ganho para ajuste do valor de corrente (não utilizado).
  * @param  gv: ganho para ajuste do valor de tensão (não utilizado).
  * @param 	vrms: tensão RMS.
  * @param 	irms: corrente RMS.
  * @retval potaparente: potência aparente calculada.
  */
float32_t retornaPOTAPARENTE(float32_t gv, float32_t gi, float32_t vrms, float32_t irms){

	float32_t potaparente;

    potaparente = irms*vrms;

    return potaparente;
}

/**
  * @brief  Retorna a potência reativa resultante a partir dos valores de potência ativa e aparente já calculados.
  * @param 	potaparente: potência aparente.
  * @param 	potativa: potência ativa.
  * @retval potreativa: potência reativa calculada.
  */
float32_t retornaPOTREATIVA(float32_t potaparente,float32_t potativa){
	float32_t potreativa;
	arm_sqrt_f32((potaparente*potaparente-potativa*potativa),&potreativa);
    return potreativa;
}

/**
  * @brief  Retorna o fator de potência a partir dos valores de potência ativa e aparente já calculados.
  * @param 	potaparente: potência aparente.
  * @param 	potativa: potência ativa.
  * @retval fp: fator de reativa calculada.
  */
float32_t retornaFP(float32_t potaparente, float32_t potativa){
	float32_t fp = potativa/potaparente;
    return fp;
}

/**
  * @brief  Tira a média de n ciclos para cálculo de harmônicas
  * @param 	array_in: ponteiro com dados de entrada. Tamanho size.
  * @param 	array_out: ponteiro com dados de saída. Tamanho (size-start)/n.
  * @param	size: número de elementos no vetor de dados.
  * @param	n: número de ciclos de 60 Hz no vetor de entrada.
  * @param	start: posição inicial do vetor de dados. Usado para que cálculo ocorra sobre um número inteiro de períodos de 60 Hz.
  * @retval None
  */
void retornaMEDIACICLOS(float32_t *array_in, float32_t *array_out, uint32_t size, uint32_t n, uint32_t start){

	float32_t aux[N_PERIODOS][PPP];
	//float32_t aux2[size/n];
	uint32_t i,j,k=0;

	for(i=0;i<n;i++){
		for(j=0;j<(size-start)/n;j++){
			aux[i][j] = array_in[k+start];
			k++;
		}
	}

	for(i=0;i<(size-start)/n;i++){
		array_out[i]=0;
	}

	for(j=0;j<(size-start)/n;j++){
		for(i=0;i<n;i++){
			array_out[j] = array_out[j] + aux[i][j] ;
		}
		array_out[j] = array_out[j]/n;
	}
}

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
void retornaRMSHARMONICOS(float32_t *i_rms_harmonicos, float32_t *array_corrente, uint32_t size, uint32_t h, float32_t g, uint32_t n, uint32_t start){

	uint32_t k,i;
	float32_t I_re[MAX_HARMONICA];
	float32_t I_im[MAX_HARMONICA];
	float32_t aux2[(BUFFER_DIZ-N_START)/N_PERIODOS];
	float32_t theta;
	float32_t harmonico;


   retornaMEDIACICLOS(array_corrente,aux2,size,n,start);

   size = (size-start)/n;

   for(i=0;i<size;i++){
   	   //printf("\naux2[%d]: %f\n", i, aux2[i]);
   }

	for (i=0;i<h;i++){

		I_re[i] = 0;
		I_im[i] = 0;

	}

	for (k=0;k<h;k++){

		for (i=0;i<size;i++){
			theta = (float32_t)2*PI_VALUE*k*i/size;
			I_re[k] =  I_re[k] + aux2[i]*arm_cos_f32(theta);
			I_im[k]  =  I_im[k] + aux2[i]*arm_sin_f32(theta);
		}
		harmonico = ((I_re[k]*I_re[k])+(I_im[k]*I_im[k]))*g*SQRT2/size;
		arm_sqrt_f32(harmonico, &harmonico);
		i_rms_harmonicos[k] = harmonico;
		//print("Harmonica %d: %d\n\r",k,harmonico);
	}

}

/**
  * @brief  Retorna o THD a partir de um vetor de harmônicas.
  * @param 	array_in: ponteiro para vetor de harmônicas.
  * @retval thd: Distorção Harmônica Total calculadas.
  */
float32_t retornaTHD(float32_t *array_in)
{
	uint32_t i = 0;
	float32_t soma = 0;
	float32_t thd;

	for(i=2; i<MAX_HARMONICA; i++)
	{
		soma = soma + array_in[i]*array_in[i];
	}
	arm_sqrt_f32(soma, &soma);
	thd = soma / array_in[1];

	return thd;
}


