#ifndef EQUIPAMENTOS_H_INCLUDED
#define EQUIPAMENTOS_H_INCLUDED

//#define ARM_MATH_CM4
//#define __FPU_PRESENT   1
#include "arm_math.h"
#include "defines.h"
#include "calculos_eletricos.h"
#include <math.h>

/*
 * \brief Estrutura com parametros eletricos
 */
typedef struct {
	float32_t pot_ap;
	float32_t pot_at;
	float32_t pot_re;
	float32_t harmonicos_RMS[MAX_HARMONICA];
	float32_t thd;
	float32_t pf;
	float32_t i_rms;
	float32_t v_rms;
} Parametros;

/*
 * \brief Estrutura armazenada na memoria com historico de medicoes e equipamentos
 */
typedef struct {
	uint32_t timestamp;
	uint16_t equipamentos[EQUIP_ARRAY_MAX];
	Parametros med;
} Medicao;

/*
 * \brief Estrutrua da base de dados de equipamentos
 */
typedef struct {
    uint16_t ID;
    char name[32];
    Parametros med;
} Equipamento;

/**
  * @brief  Cria estrutura com informacoes de um novo equipamento
  * @param  equip: ponteiro para estrutura Equipamento
  * @param 	nome: char* com nome do equipamento
  * @param	pot_at: potencia ativa
  * @param	pot_re: potencia reativa
  * @param	thd: taxa de distorção harmonica
  * @param	pf: fator de potencia
  * @param	harmonicos: vetor de harmonicas
  * @retval None
  */
void CadastroDeEquipamento(Equipamento *, uint16_t , char nome[40], float32_t , float32_t , float32_t , float32_t , float32_t *);
void DeltaParam(Parametros *, Parametros *, Parametros *, char* );
float ComparacaoDeEquipamentos(Equipamento *, Parametros *);
void InitMedicao(Medicao *);
void InitBaseDeDados(Equipamento *);
int IdentificarEquipamento(Equipamento *, Parametros *);

#endif // EQUIPAMENTOS_H_INCLUDED
