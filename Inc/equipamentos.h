#ifndef EQUIPAMENTOS_H_INCLUDED
#define EQUIPAMENTOS_H_INCLUDED

//#define ARM_MATH_CM4
//#define __FPU_PRESENT   1
#include "arm_math.h"
#include "defines.h"
#include <math.h>

/*
 * \brief Estrutura com parametros eletricos
 */
typedef struct {
	float32_t pot_ap;
	float32_t pot_at;
	float32_t pot_re;
	float32_t harmonicos_RMS[10];
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
    char *name;
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
void CadastroDeEquipamento(Equipamento *equip, uint16_t ID, char nome[40], float32_t pot_at, float32_t pot_re, float32_t thd, float32_t pf, float32_t *harmonicos){
    equip->ID = ID;
    equip->name = nome;

    equip->med.pot_at = pot_at;
    equip->med.pot_re = pot_re;
    equip->med.thd = thd;
    equip->med.pf = pf;

    int i;
    for(i = 0;i<10;i++){
        equip->med.harmonicos_RMS[i] = harmonicos[i];
    }



    printf("\n---------- Equipamento Cadastrado -----------\n\n\n");
    printf("Identificacao..: %i", equip->ID);
    printf("\n");
    printf("Nome..: %s", equip->name);
    printf("\n");
    printf("Potencia Ativa..: %f", equip->med.pot_at);
    printf("\n");
    printf("Potencia Reativa..: %f", equip->med.pot_re);
    printf("\n");
    printf("Distorcao Harmonica..: %f", equip->med.thd);
    printf("\n");
    printf("Fator de Potencia..: %f", equip->med.pf);
    printf("\n");

    for(i = 0;i<10;i++){
        printf("Harmonico %i..: %f", i, equip->med.harmonicos_RMS[i]);
        printf("\n");
    }
}

float ComparacaoDeEquipamentos(Equipamento *equip1, Parametros *medida){
    float dif_rel[4];
    dif_rel[0] = (equip1->med.pot_at - medida->pot_at)/equip1->med.pot_at;
    dif_rel[1] = (equip1->med.pot_re - medida->pot_re)/equip1->med.pot_re;
//    dif_rel[2] = (equip1->med.thd - medida->thd)/equip1->med.thd;
//    dif_rel[3] = (equip1->med.pf - medida->pf)/equip1->med.pf;

    float harm;
    int i;
    dif_rel[2] = 0 ;
    for(i=0;i<10;i++){
        dif_rel[2] = dif_rel[2] + equip1->med.harmonicos_RMS[i] - medida->harmonicos_RMS[i];
        harm = harm + equip1->med.harmonicos_RMS[i];
    }
    dif_rel[2] = dif_rel[2]/harm;

    printf("\n---------- Diferencas -----------\n\n\n");
    printf("Potencia Ativa..: %f", dif_rel[0]);
    printf("\n");
    printf("Potencia Reativa..: %f", dif_rel[1]);
    printf("\n");
    printf("Harmonico..: %f", dif_rel[2]);
    printf("\n");

    float dif = pow(dif_rel[0],2) + pow(dif_rel[1],2) + pow(dif_rel[2],2) + pow(dif_rel[3],2);

    return dif;
}

void InitMedicao(Medicao *medicao)
{
	medicao->med.pot_ap = 0;
	medicao->med.pot_at = 0;
    medicao->med.pot_re = 0;
    medicao->med.thd = 0;
    medicao->med.pf = 0;
    medicao->med.i_rms = 0;
    medicao->med.v_rms = 0;
    for(int i=0; i<10; i++) {medicao->med.harmonicos_RMS[i] = 0;}
}

#endif // EQUIPAMENTOS_H_INCLUDED
