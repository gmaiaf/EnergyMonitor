/*
 * equipamentos.c
 *
 *  Created on: Jul 3, 2017
 *      Author: Gustavo
 */

#include "equipamentos.h"
#include "string.h"
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
    strcpy(equip->name,nome);

    equip->med.pot_at = pot_at;
    equip->med.pot_re = pot_re;
    equip->med.thd = thd;
    equip->med.pf = pf;

    int i;
    for(i = 0;i<10;i++){
        equip->med.harmonicos_RMS[i] = harmonicos[i];
    }



    /*printf("\n---------- Equipamento Cadastrado -----------\n\n\n");
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
    }*/
}


void DeltaParam(Parametros *medida_nova, Parametros *medida_velha, Parametros *delta, char* new)
{
	/* new = 1 -> equipamento adicionado, new = 0 -> equipamento removido */

	/* Variavel auxiliar */
	float32_t aux;

	/* Delta de RMS de corrente */
	aux = (medida_nova->i_rms)*(medida_nova->i_rms)-(medida_velha->i_rms)*(medida_velha->i_rms);
	if (aux < 0) {
		aux = (-1)*aux;
		*new = 0;
	}
	else
	{
		*new = 1;
	}
	arm_sqrt_f32(aux,&aux);
	delta->i_rms = aux;
	/* Delta de RMS de tensao */
	aux = (medida_nova->v_rms)*(medida_nova->v_rms)-(medida_velha->v_rms)*(medida_velha->v_rms);
	if (aux < 0) {aux = (-1)*aux;}
	arm_sqrt_f32(aux,&aux);
	delta->v_rms = aux;

	/* Delta de potencia */
	delta->pot_ap = medida_nova->pot_ap - medida_velha->pot_ap;
	if (delta->pot_ap < 0) {delta->pot_ap = (-1)*delta->pot_ap;}
	delta->pot_at = medida_nova->pot_at - medida_velha->pot_at;
	if (delta->pot_at < 0) {delta->pot_at = (-1)*delta->pot_at;}
	delta->pot_re = medida_nova->pot_re - medida_velha->pot_re;
	if (delta->pot_re < 0) {delta->pot_re = (-1)*delta->pot_re;}

	/* Fator de potencia do delta de potencia */
	delta->pf = delta->pot_at/delta->pot_ap;

	/* Delta dos harmonicos */
	for(int i=0; i<MAX_HARMONICA; i++)
	{
		delta->harmonicos_RMS[i] = medida_nova->harmonicos_RMS[i] - medida_velha->harmonicos_RMS[i];
	}
	delta->thd = retornaTHD(delta->harmonicos_RMS);
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

    /*printf("\n---------- Diferencas -----------\n\n\n");
    printf("Potencia Ativa..: %f", dif_rel[0]);
    printf("\n");
    printf("Potencia Reativa..: %f", dif_rel[1]);
    printf("\n");
    printf("Harmonico..: %f", dif_rel[2]);
    printf("\n");*/

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
    medicao->timestamp = 0;
    for(int i=0; i<EQUIP_ARRAY_MAX; i++) {medicao->equipamentos[i] = 0;}
}

void InitBaseDeDados(Equipamento * BaseDados)
{
	int32_t i = 0;
	int32_t k = 0;
	for(i=0; i<EQUIP_ARRAY_MAX; i++)
	{
		BaseDados[i].ID = i;
		strcpy(BaseDados[i].name, "Sem equipamento");
		BaseDados[i].med.i_rms = 0;
		BaseDados[i].med.v_rms = 0;
		BaseDados[i].med.pot_ap = 0;
		BaseDados[i].med.pot_at = 0;
		BaseDados[i].med.pot_re = 0;
		BaseDados[i].med.thd = 0;
		BaseDados[i].med.pf = 0;
		for(k=0; k<MAX_HARMONICA; k++) {BaseDados[i].med.harmonicos_RMS[k] = 0;}

	}

	/* Equipamento 1 */
	strcpy(BaseDados[1].name, "Equipamento 1");
	BaseDados[1].med.i_rms = 3;

	/* Equipamento 2 */
	strcpy(BaseDados[2].name, "Equipamento 2");
	BaseDados[2].med.i_rms = 1;

}


int IdentificarEquipamento(Equipamento *BaseDados, Parametros *Medicao)
{
	float32_t min_dist, aux_dist, relative_dist;
	int min;
	int i;

	/* Inicializar minimo */
	relative_dist = (BaseDados[0].med.i_rms - Medicao->i_rms)/(BaseDados[0].med.i_rms);
	min_dist = relative_dist*relative_dist;
	relative_dist = (BaseDados[0].med.pf - Medicao->pf)/(BaseDados[0].med.pf);
	min_dist = min_dist + relative_dist*relative_dist;
	relative_dist = (BaseDados[0].med.thd - Medicao->thd)/(BaseDados[0].med.thd);
	min_dist = min_dist + relative_dist*relative_dist;
	min = 0;

	/* Loop por todos equipamentos para encontrar a menor distancia*/
	for(i=1; i<EQUIP_ARRAY_MAX; i++)
	{
		relative_dist = (BaseDados[i].med.i_rms - Medicao->i_rms)/(BaseDados[i].med.i_rms);
		aux_dist = relative_dist*relative_dist;
		relative_dist = (BaseDados[i].med.pf - Medicao->pf)/(BaseDados[i].med.pf);
		aux_dist = aux_dist + relative_dist*relative_dist;
		relative_dist = (BaseDados[i].med.thd - Medicao->thd)/(BaseDados[i].med.thd);
		aux_dist = aux_dist + relative_dist*relative_dist;

		if(aux_dist < min_dist) min = i;
	}

	/* Verificar se a menor distancia está dentro da faixa do equipamento */
	if ((Medicao->i_rms < (RMS_LOWERBOUND*BaseDados[min].med.i_rms)) || (Medicao->i_rms > (RMS_UPPERBOUND*BaseDados[min].med.i_rms))) {
		return -1;
	}
	else {
		return min;
	}
}



