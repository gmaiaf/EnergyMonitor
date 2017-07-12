  
 /**
  *	@file equipamentos.h
  * @author Bruno e Gustavo
  * @brief Header com as estruturas do projeto e fun��es que manipulam essas estruturas.
  *
  */

#ifndef EQUIPAMENTOS_H_INCLUDED
#define EQUIPAMENTOS_H_INCLUDED

//#define ARM_MATH_CM4
//#define __FPU_PRESENT   1
#include "arm_math.h"
#include "defines.h"
#include "calculos_eletricos.h"
#include <math.h>

/**
 * @brief Estrutura com parametros el�tricos
 *
 *	Esta estrutura � usada para armazenar os par�metros el�tricos de interesse no projeto.
 *	Uma entrada desse tipo � usada nas outras estruturas.
 */
typedef struct Parametros{
	float32_t pot_ap; /**< Potencia aparente em Watts. */
	float32_t pot_at; /**< Potencia ativa em Watts. */
	float32_t pot_re; /**< Potencia reativa em Watts. */
	float32_t harmonicos_RMS[MAX_HARMONICA]; /**< Vetor com a pot�ncia das harm�nicas. */
	float32_t thd; /**< Distor��o harm�nica total. */
	float32_t pf; /**< Fator de pot�ncia. */
	float32_t i_rms; /**< Corrente RMS em Amp�res. */
	float32_t v_rms; /**< Tens�o RMS em Volts. */
} Parametros;

/**
 * @brief Estrutura armazenada na mem�ria com hist�rico de medi��es e equipamentos
 *
 *	Essa estrutura usa uma entrada da estrutura Parametros para armazenar os dados el�tricos lidos em associa��o a um timestamp e um vetor de equipamentos el�tricos ligados na rede.
 */
typedef struct Medicao{
	uint32_t timestamp; /**< Marca��o de tempo (timestamp) associado a uma medi��o da rede. */
	uint16_t equipamentos[EQUIP_ARRAY_MAX];/**< Vetor que indica os equipamentos ligados na rede. O �ndice do vetor indica uma ID de equipamento e o conte�do indica quantos equipamentos deste tipo est�o ligados na rede no momento.*/
	Parametros med; /**< Entrada para estrutura Parametros que armazena par�metros el�tricos de interesse.*/
} Medicao;

/**
 * @brief Estrutura da base de dados de equipamentos
 *
 *	Essa estrutura � uma entrada da base de dados de equipamentos. Armazena informa��es referentes a cada equipamento que pode ser identificado.
 */
typedef struct {
    uint16_t ID;/**< ID para o equipamento */
    char name[32];/**< String com o nome do equipamento. */
    Parametros med;/**< Entrada para estrutura Parametros que armazena par�metros el�tricos de interesse.*/
} Equipamento;

/**
  * @brief  Cria estrutura com informa��es de um novo equipamento.
  * @param  equip: ponteiro para estrutura #Equipamento .
  * @param 	nome: char* com nome do equipamento.
  * @param	pot_at: potencia ativa.
  * @param	pot_re: potencia reativa.
  * @param	thd: taxa de distor��o harmonica.
  * @param	pf: fator de potencia.
  * @param	harmonicos: vetor de harmonicas.
  * @retval None
  */
void CadastroDeEquipamento(Equipamento *, uint16_t , char nome[40], float32_t , float32_t , float32_t , float32_t , float32_t *);
/**
  * @brief  Calcula delta entre duas estruturas #Parametros .
  * @param  medida_nova: ponteiro para uma estrutura #Parametros que cont�m os par�metros da medi��o mais recente.
  * @param 	medida_velha: ponteiro para uma estrutura #Parametros que cont�m os par�metros da medi��o anterior.
  * @param	delta: ponteiro para uma estrutura #Parametros que armazana o delta entre as duas entradas.
  * @param	new: ponteiro para um char que indicar� se um equipamento foi adicionado ou retirado da rede.
  * @retval None
  */
void DeltaParam(Parametros *, Parametros *, Parametros *, char* );
/**
  * @brief  DEPRECATED Calcula delta entre duas estruturas #Parametros .
  * @retval None
  */
float ComparacaoDeEquipamentos(Equipamento *, Parametros *);
/**
  * @brief  Inicializa uma estrutura #Medicao .
  * @param  medicao: ponteiro para uma estrutura #Medicao que ser� inicializada com zeros.
  * @retval None
  */
void InitMedicao(Medicao *);
/**
  * @brief  Inicializa uma vetor (base de dados) de estruturas #Equipamento .
  * @param  BaseDados: ponteiro para um vetor de #Equipamento que ser� inicializada com zeros.
  * @retval None
  */
void InitBaseDeDados(Equipamento *);

/**
  * @brief  Fun��o com implementa��o b�sica de um algoritmo de identifica��o de equipamento na rede.
  * @param  BaseDados: ponteiro para um vetor de #Equipamento sobre o qual � feita a busca da #Medicao.
  * @param	Medicao: ponteiro para #Parametros que cont�m a medi��o do delta na rede. Representa um equipamento acrescido ou retirado da rede.
  * @retval min: �ndice da base de dados que cont�m o equipamento identificado.
  */
int IdentificarEquipamento(Equipamento *, Parametros *);

#endif // EQUIPAMENTOS_H_INCLUDED
