/*
 * calculos_eletricos.h
 *
 *  Created on: 20 de jun de 2017
 *      Author: andre
 */

#ifndef CALCULOS_ELETRICOS_H_
#define CALCULOS_ELETRICOS_H_

void *retornaSIN(float32_t *array,uint32_t f,float32_t fs, uint32_t n, float32_t phase);
float32_t retornaRMS (float32_t g, float32_t *array, uint32_t size,  uint32_t start);
float32_t retornaPOTATIVA(float32_t gv, float32_t gi, float32_t *array_tensao, float32_t *array_corrente, uint32_t size, uint32_t start);
float32_t retornaPOTAPARENTE(float32_t gv, float32_t gi, float32_t vrms, float32_t irms);
float32_t retornaPOTREATIVA(float32_t potaparente,float32_t potativa);
float32_t retornaFP(float32_t potaparente, float32_t potativa);
void retornaRMSHARMONICOS(float32_t *i_rms_harmonicos, float32_t *array_corrente, uint32_t size, uint32_t h, float32_t g, uint32_t n, uint32_t start);
void retornaMEDIACICLOS(float32_t *array_in, float32_t *array_out, uint32_t size, uint32_t n, uint32_t start);
float32_t retornaTHD(float32_t *array_in);


#endif /* CALCULOS_ELETRICOS_H_ */
