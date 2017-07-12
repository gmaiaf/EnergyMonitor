   /**
  *	@file dizimacao.h
  * @author Bruno e Gustavo
  * @brief Código para dizimação: Filtro FIR e downsampling usando biblioteca CMSIS.
  *
  */

#ifndef DIZIMACAO_H_INCLUDED
#define DIZIMACAO_H_INCLUDED

#include "defines.h"
#include "arm_math.h"

/*FIR filter designed with
http://t-filter.appspot.com

sampling frequency: 12000 Hz

* 0 Hz - 600 Hz
  gain = 1
  desired ripple = 0.1 dB
  actual ripple = 0.07168618441922267 dB

* 1500 Hz - 6000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -40.24445073899409 dB

*/

/**
  * @brief  Coeficientes do filtro FIR.
  */
static float pCoeffs[FILTER_TAP_NUM] = {
  0.007998549017202374,
  0.006388561949346508,
  0.004756427177175156,
  -0.0008349333679403463,
  -0.009842809472575242,
  -0.019723906468263483,
  -0.026242868239617655,
  -0.02448334034621427,
  -0.010509585087882752,
  0.017007061589710708,
  0.055725610289231155,
  0.09962338234788838,
  0.14028048488295913,
  0.16904057905428516,
  0.17942568144262785,
  0.16904057905428516,
  0.14028048488295913,
  0.09962338234788838,
  0.055725610289231155,
  0.017007061589710708,
  -0.010509585087882752,
  -0.02448334034621427,
  -0.026242868239617655,
  -0.019723906468263483,
  -0.009842809472575242,
  -0.0008349333679403463,
  0.004756427177175156,
  0.006388561949346508,
  0.007998549017202374
};

/**
  * @brief  Buffer auxiliar para estrutura da biblioteca CMSIS.
  */
float32_t pState[FILTER_TAP_NUM+BUFFER_SIZE-1];

/**
  * @brief  Estrutura da biblioteca CMSIS usada para dizimação.
  */
arm_fir_decimate_instance_f32 S = {DIZIMACAO, FILTER_TAP_NUM, pCoeffs, pState};

/**
  * @brief  Inicializa estrutura da biblioteca CMSIS usada para dizimação.
  * @param	S:  estrutura da biblioteca CMSIS usada para dizimação.
  * @retval	None
  */
void initializeFIR(arm_fir_decimate_instance_f32* S)
{
	uint32_t i = 0;
	for (i = 0; i<(FILTER_TAP_NUM+BUFFER_SIZE-1); i++) {pState[i] = 0;}

}

#endif
