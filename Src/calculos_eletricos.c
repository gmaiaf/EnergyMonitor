/*
 * calculos_eletricos.c
 *
 *  Created on: 20 de jun de 2017
 *      Author: andre
 */

//#define ARM_MATH_CM4
//#define __FPU_PRESENT   1U
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "arm_math.h"
#include "defines.h"
#define PI_VALUE 3.14159265358
#define SQRT2 1.414213562373095

/**
  * @brief  Cria um sinal seno
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
  * @brief
  * @param
  * @retval
  */
float32_t retornaRMS (float32_t g, float32_t *array, uint32_t size){

	uint32_t i;
	float32_t arrayrms ;
	float32_t arraysum =0;

    for(i=0;i<size;i++){
        arraysum = arraysum + array[i]*array[i];
    }
    arraysum = arraysum / size;
    arm_sqrt_f32(arraysum, &arrayrms);
    arrayrms = g * arrayrms;

    return arrayrms;
}

 /**
   * @brief
   * @param
   * @retval
   */
float32_t retornaPOTATIVA(float32_t gv, float32_t gi, float32_t *array_tensao, float32_t *array_corrente, uint32_t size){

	float32_t potativa,potativasum =0;
    uint32_t i;

    for(i=0;i<size;i++){
        potativasum = potativasum + (*(array_corrente+i) * (*(array_tensao+i)));
    }
    potativa = gv*gi*potativasum/size;

    return potativa;

}

/**
  * @brief
  * @param
  * @retval
  */
float32_t retornaPOTAPARENTE(float32_t gv, float32_t gi, float32_t vrms, float32_t irms, uint32_t size){

	float32_t potaparente;

    potaparente = irms*vrms;

    return potaparente;
}

/**
  * @brief
  * @param
  * @retval
  */
float32_t retornaPOTREATIVA(float32_t potaparente,float32_t potativa){
	float32_t potreativa;
	arm_sqrt_f32((potaparente*potaparente-potativa*potativa),&potreativa);
    return potreativa;
}

/**
  * @brief
  * @param
  * @retval
  */
float32_t retornaFP(float32_t potaparente, float32_t potativa){
	float32_t fp = potativa/potaparente;
    return fp;
}

/**
  * @brief
  * @param
  * @retval
  */
void retornaMEDIACICLOS(float32_t *array_in, float32_t *array_out, uint32_t size, uint32_t n){

	float32_t aux[n][size/n];
	//float32_t aux2[size/n];
	uint32_t i,j,k=0;

	for(i=0;i<n;i++){
		for(j=0;j<size/n;j++){
			aux[i][j] = array_in[k];
			k++;
		}
	}

	for(i=0;i<size/n;i++){
		array_out[i]=0;
	}

	for(j=0;j<size/n;j++){
		for(i=0;i<n;i++){
			array_out[j] = array_out[j] + aux[i][j] ;
		}
		array_out[j] = array_out[j]/n;
	}
}

/**
  * @brief
  * @param
  * @retval
  */
void retornaRMSHARMONICOS(float32_t *i_rms_harmonicos, float32_t *array_corrente, uint32_t size, uint32_t h, float32_t g, uint32_t n ){

	uint32_t k,i;
	float32_t I_re[h];
	float32_t I_im[h];
	float32_t aux2[size/n];
	float32_t theta;
	float32_t harmonico;

   retornaMEDIACICLOS(array_corrente,aux2,size,n);

   size = size/n;

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
	}

}





