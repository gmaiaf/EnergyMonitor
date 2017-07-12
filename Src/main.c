/*
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  /**
  *	@file main.c
  * @author André, Bruno, Gustado e Leonador
  * @brief Aplicação: Sistema de Monitoramento de Consumo de Energia.
  *
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
/* Include bibliotecas do projeto */
#include "defines.h"
#include "arm_math.h"
#include "dizimacao.h"
#include "adc_util.h"
#include "equipamentos.h"
#include "calculos_eletricos.h"
#include "usart_util.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
/* Variavel de estado da FSM */
/**
  * @brief  Estados da máquina de estados da aplicação.
  */
enum FSM {START, AQUISICAO, PROCESSAMENTO, RMS_CORRENTE, CALCULOS, DELTA, ID, ENVIAR}
estado = START;
/* Historico de medicoes e indexador*/
/**
  * @brief  Vetor de #Medicao . Contém histórico de medições.
  */
Medicao memoria[MEM_SIZE];
/**
  * @brief  Índice do histórico de medições.
  */
uint32_t memoria_index = 0;
/* Base de dados de equipamentos */
/**
  * @brief  Base de dados de #Equipamento .
  */
Equipamento BaseDados[EQUIP_ARRAY_MAX];
/* Buffers usados ao longo do processamento */
/**
  * @brief  Buffer circular de leitura de tensão. Interrupções ocorrem quando está na metade ou cheio.
  */
uint32_t buffer_tensao_DMA[2*BUFFER_SIZE];
/**
  * @brief  Buffer circular de leitura de corrente. Interrupções ocorrem quando está na metade ou cheio.
  */
uint32_t buffer_corrente_DMA[2*BUFFER_SIZE];
/**
  * @brief  Buffer auxiliar de leitura de tensão.
  */
uint32_t buffer_tensao_leitura[BUFFER_SIZE];
/**
  * @brief  Buffer auxiliar de leitura de corrente.
  */
uint32_t buffer_corrente_leitura[BUFFER_SIZE];
/**
  * @brief  Buffer auxiliar de leitura de tensão em ponto flutuante.
  */
float32_t buffer_corrente_float[BUFFER_SIZE];
/**
  * @brief  Buffer auxiliar de leitura de corrente em ponto flutuante.
  */
float32_t buffer_tensao_float[BUFFER_SIZE];
/**
  * @brief  Buffer auxiliar de leitura de tensão em ponto flutuante dizimado.
  */
float32_t buffer_tensao_diz[BUFFER_DIZ];
/**
  * @brief  Buffer auxiliar de leitura de corrente em ponto flutuante dizimado.
  */
float32_t buffer_corrente_diz[BUFFER_DIZ];
/* Flags de controle */
/**
  * @brief  Variável de testes.
  */
uint32_t count = 0;
/**
  * @brief  Variável de testes.
  */
uint32_t tic, toc;
/**
  * @brief  Flag de controle. Ativada quando ocorre interrupção de buffer cheio.
  */
uint8_t flag_buffercheio = 0;
/**
  * @brief  Flag de controle. Ativada quando ocorre interrupção de buffer na metade.
  */
uint8_t flag_buffermetade = 0;
/**
  * @brief  Flag de testes.
  */
uint8_t flag_aquisicao = 0;
/* Filtro FIR + dizimacao */
/**
  * @brief  Estrutura da biblioteca CMSIS para realização da dizimação.
  */
arm_fir_decimate_instance_f32 S;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Configuração de clock.
  */
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
/* Callbacks das interrupcoes do ADC-DMA */
/**
  * @brief  Callback da interrupção de transferência completa da DMA.
  * @param	hadc: Handler para ADC.
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *);
/**
  * @brief  Callback da interrupção de transferência na metade da DMA.
  * @param	hadc: Handler para ADC.
  */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * @brief  Main da aplicação.
  *
  *	Implementa a máquina de estados descrita no relatório 3.
  *
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* Variaveis locais */
	float32_t corrente_RMS = 0;
	float32_t corrente_RMS_anterior = 0;
	uint32_t i = 0;
	uint32_t timestamp = 0;
	uint32_t timestamp_anterior = 0;
	float32_t energia = 0;
	Parametros param_aux, param_delta;
	int32_t float_inteiro;
	int32_t float_fracao;
	char new_equip;
	int add_new_equip;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_USART2_UART_Init();
  MX_TIM8_Init();

  /* USER CODE BEGIN 2 */

  /* Inicializar memoria com zeros */
  for (i=0; i<MEM_SIZE; i++)
  {
	  InitMedicao(&(memoria[i]));
  }
  /* Inicializar Base de Dados */
  InitBaseDeDados(BaseDados);

  print("%d\n\r",BaseDados[1].ID);
  print("%d\n\r",BaseDados[2].ID);
  print("%d\n\r",BaseDados[3].ID);
  print("%d\n\r",BaseDados[4].ID);
  print("%d\n\r",BaseDados[5].ID);
  print("%d\n\r",BaseDados[6].ID);
  print("%d\n\r",BaseDados[7].ID);
  print("%d\n\r",BaseDados[8].ID);


  /* Inicializar filtro FIR */
  initializeFIR(&S);
  /* Inicializar Timer 8 */
  HAL_TIM_Base_Start(&htim8);
  /* Inicializar ADCs */
  HAL_ADC_Start(&hadc2);
  HAL_ADC_Start_DMA(&hadc1, buffer_tensao_DMA, 2*BUFFER_SIZE);
  HAL_ADC_Start_DMA(&hadc2, buffer_corrente_DMA, 2*BUFFER_SIZE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
	  switch(estado){
	  case START:
		  /*
		   * Estado: START
		   * Descricao: Estado inicial da FSM
		   * Proximo estado: AQUISICAO
		   * Transicao: sempre
		   */
		  estado = AQUISICAO;
		  break;

	  case AQUISICAO:
		  /*
		   * Estado: AQUISICAO
		   * Descricao: Espera pela aquisicao de dados pelo ADC
		   * Proximo estado: PROCESSAMENTO
		   * Transicao: transferencia do buffer do ADC iniciada
		   */

		  /* Manter estado em AQUISICAO*/
		  estado = AQUISICAO;

		  if (flag_buffercheio == 1){
			  /* Timestamp da medicao */
			  timestamp_anterior = timestamp;
			  timestamp = HAL_GetTick();

			  /* Transferir dados do buffer da DMA para buffer de leitura e esperar transferencia*/
			  HAL_DMA_Start(&hdma_memtomem_dma2_stream3, (uint32_t) (buffer_corrente_DMA+BUFFER_SIZE), (uint32_t) buffer_corrente_leitura,BUFFER_SIZE);
			  HAL_DMA_Start(&hdma_memtomem_dma2_stream1, (uint32_t) (buffer_tensao_DMA+BUFFER_SIZE), (uint32_t) buffer_tensao_leitura,BUFFER_SIZE);
			  /* Setar flag */
			  estado = PROCESSAMENTO;
		  }

		  if (flag_buffermetade == 1){
			  /* Timestamp da medicao */
			  timestamp_anterior = timestamp;
			  timestamp = HAL_GetTick();

			  /* Transferir dados do buffer da DMA para buffer de leitura e esperar transferencia*/
			  HAL_DMA_Start(&hdma_memtomem_dma2_stream3, (uint32_t) buffer_corrente_DMA, (uint32_t) buffer_corrente_leitura, BUFFER_SIZE);
			  HAL_DMA_Start(&hdma_memtomem_dma2_stream1, (uint32_t) buffer_tensao_DMA, (uint32_t) buffer_tensao_leitura, BUFFER_SIZE);
			  /* Setar flag */
			  estado = PROCESSAMENTO;
		  }
		  break;

	  case PROCESSAMENTO:
		  /*
		   * Estado: PROCESSAMENTO
		   * Descricao: Transforma leitura do ADC em valores do mundo real e dizima sinal
		   * Proximo estado: RMS_CORRENTE
		   * Transicao: Conversão completa
		   */

		  HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream3, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
		  /* Converter valor do ADC 0-4095 para float32_t */
		  ADCConvertBuffer(buffer_corrente_leitura, buffer_corrente_float, BUFFER_SIZE, CORRENTE_A, CORRENTE_B);
		  /* Filtro FIR  e Dizimacao da corrente*/
		  initializeFIR(&S);
		  arm_fir_decimate_f32(&S, buffer_corrente_float, buffer_corrente_diz, BUFFER_SIZE);

		  HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream1, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);

		  /* Converter valor do ADC 0-4095 para float32_t */
		  ADCConvertBuffer(buffer_tensao_leitura, buffer_tensao_float, BUFFER_SIZE, TENSAO_A, TENSAO_B);
		  /* Filtro FIR e Dizimacao da tensao */
		  initializeFIR(&S);
		  arm_fir_decimate_f32(&S, buffer_tensao_float, buffer_tensao_diz, BUFFER_SIZE);

		  estado = RMS_CORRENTE;

		  // enviar valor pela UART - DEBUG
		  for(i=0; i<BUFFER_SIZE; i++)
		  {
			  // print valor leitura adc
			  /*USART2_Transmit_UInt(buffer_tensao_leitura[i]);
			  USART2_Transmit_Char(',');
			  USART2_Transmit_UInt(buffer_corrente_leitura[i]);
			  USART2_Transmit_String("\n\r");*/

			  // print valor real antes de dizimacao (mA e V)
			  float_inteiro = (int32_t) (1000*buffer_corrente_float[i]);
			  USART2_Transmit_Int(float_inteiro);
			  //float_fracao = abs((int32_t)(buffer_corrente_float[i]*1000)%1000);
			  //USART2_Transmit_Char('.');
			  //USART2_Transmit_Int(float_fracao);
			  USART2_Transmit_Char(',');
			  float_inteiro = (int32_t) (buffer_tensao_float[i]);
			  USART2_Transmit_Int(float_inteiro);
			  float_fracao = abs((int32_t)(buffer_tensao_float[i]*1000)%1000);
			  USART2_Transmit_Char('.');
			  USART2_Transmit_UInt(float_fracao);
			  USART2_Transmit_String("\n\r");
		  }

		  for(i=0; i<BUFFER_DIZ; i++)
		  {

			  // print valor real depois da dizimacao (mA e V)
			  /*float_inteiro = (int32_t) (1000*buffer_corrente_diz[i]);
			  USART2_Transmit_Int(float_inteiro);
			  //float_fracao = abs((int32_t)(buffer_corrente_diz[i]*1000)%1000);
			  //USART2_Trans 	mit_Char('.');
			  //USART2_Transmit_Int(float_fracao);
			  USART2_Transmit_Char(',');
			  float_inteiro = (int32_t) (buffer_tensao_diz[i]);
			  USART2_Transmit_Int(float_inteiro);
			  float_fracao = abs((int32_t)(buffer_tensao_diz[i]*1000)%1000);
			  USART2_Transmit_Char('.');
			  USART2_Transmit_UInt(float_fracao);
			  USART2_Transmit_String("\n\r");*/
		  }
		  HAL_Delay(100);
		  break;

	  case RMS_CORRENTE:
		  /*
		   * Estado: RMS_CORRENTE
		   * Descricao: Medir valor RMS da corrente e verificar se houve alguma alteracao
		   * Proximo estado: AQUISICAO
		   * Transicao: Valor RMS da corrente e proximo a ultima medicao
		   * Proximo estado: CALCULOS
		   * Transicao: Valor RMS da corrente esta fora da faixa da ultima medicao
		   */

		  /* Calcular RMS da corrente sobre a aquisicao */
		  corrente_RMS = retornaRMS(GI, buffer_corrente_diz, BUFFER_DIZ, N_START);

		  // print valor RMS lido
		  //float_inteiro = (int32_t) (1000*corrente_RMS);
		  //print("RMS Lido: %d\n\r", float_inteiro);

		  /* Ler ultimo valor de RMS do historico*/
		  corrente_RMS_anterior = memoria[memoria_index%MEM_SIZE].med.i_rms;

		  energia = energia + memoria[memoria_index%MEM_SIZE].med.v_rms*corrente_RMS*memoria[memoria_index%MEM_SIZE].med.pf*((float32_t)(timestamp-timestamp_anterior))*MS2H;

		  //print valor RMS lido
		  //float_inteiro = (int32_t) (1000*corrente_RMS_anterior);
		  //print("RMS Anterior: %d\n\r", float_inteiro);

		  /* Verificar se novo valor de RMS esta dentro ou fora de uma faixa */
		  if ((corrente_RMS < (RMS_LOWERBOUND*corrente_RMS_anterior)) || (corrente_RMS > (RMS_UPPERBOUND*corrente_RMS_anterior))) {
			  estado = CALCULOS;
			  // print valor RMS lido
			  //float_inteiro = (int32_t) (1000*corrente_RMS);
			  //print("RMS Lido: %d\n\r", float_inteiro);
			  //print("Proximo estado: CALCULOS\n\r");
		  }
		  else {
			  estado = AQUISICAO;
			  //print("Proximo estado: AQUISICAO\n\r");
		  }
		  break;

	  case CALCULOS:
		  /*
		   * Estado: CALCULOS
		   * Descricao: Realiza calculos de potencia e harmonicas do intervalor
		   * Proximo estado: DELTA
		   * Transicao: Calculos completos
		   */
		  /* Calcular parametros eletricos obtidos*/

		  param_aux.i_rms = corrente_RMS;
		  float_inteiro = (int32_t) (1000*param_aux.i_rms);
		  //print("I RMS: %d\n\r", float_inteiro);
		  param_aux.v_rms = retornaRMS(GI, buffer_tensao_diz, BUFFER_DIZ, N_START);
		  float_inteiro = (int32_t) (param_aux.v_rms);
		  //print("V RMS: %d\n\r", float_inteiro);
		  param_aux.pot_at = retornaPOTATIVA(GV, GI, buffer_tensao_diz, buffer_corrente_diz, BUFFER_DIZ, N_START);
		  float_inteiro = (int32_t) (param_aux.pot_at);
		  //print("Potencia Ativa: %d\n\r", float_inteiro);
		  param_aux.pot_ap = retornaPOTAPARENTE(GV, GI, param_aux.v_rms, param_aux.i_rms);
		  float_inteiro = (int32_t) (param_aux.pot_ap);
		  //print("Potencia Aparente: %d\n\r", float_inteiro);
		  param_aux.pot_re = retornaPOTREATIVA(param_aux.pot_ap, param_aux.pot_at);
		  float_inteiro = (int32_t) (param_aux.pot_re);
		  //print("Potencia Reativa: %d\n\r", float_inteiro);
		  param_aux.pf = retornaFP(param_aux.pot_ap, param_aux.pot_at);
		  float_inteiro = (int32_t) (1000*param_aux.pf);
		  //print("Fator de Potencia: %d\n\r", float_inteiro);
		  param_aux.harmonicos_RMS[0]=1;
		  retornaRMSHARMONICOS(param_aux.harmonicos_RMS, buffer_corrente_diz, BUFFER_DIZ, MAX_HARMONICA, GI, N_PERIODOS, N_START);
		  for(i=0; i<MAX_HARMONICA; i++)
		  {
			  //print("Harmonica %d: %d\n\r",i,(int32_t)(1000*param_aux.harmonicos_RMS[i]));
		  }
		  param_aux.thd = retornaTHD(param_aux.harmonicos_RMS);
		  float_inteiro = (int32_t) (1000*param_aux.thd);
		  //print("THD: %d\n\r", float_inteiro);
		  float_inteiro = (int32_t) (1000000*energia);
		  //print("Energia Total: %d\n\r\r", float_inteiro);

		  estado = DELTA;
		  break;

	  case DELTA:
		  /*
		   * Estado: DELTA
		   * Descricao: Compara resultado dos calculos obtidos com os da ultima medicao
		   * Proximo estado: ID
		   * Transicao: Delta obtido
		   */
		  /* Obter delta entre medicao atual e medicao anterior */
		  DeltaParam(&param_aux, &(memoria[memoria_index%MEM_SIZE]), &param_delta, &new_equip);

		  /*float_inteiro = (int32_t) (1000*param_delta.i_rms);
		  print("I RMS: %d\n\r", float_inteiro);
		  float_inteiro = (int32_t) (param_delta.v_rms);
		  print("V RMS: %d\n\r", float_inteiro);
		  float_inteiro = (int32_t) (param_delta.pot_at);
		  print("Potencia Ativa: %d\n\r", float_inteiro);
		  float_inteiro = (int32_t) (param_delta.pot_ap);
		  print("Potencia Aparente: %d\n\r", float_inteiro);
		  float_inteiro = (int32_t) (param_delta.pot_re);
		  print("Potencia Reativa: %d\n\r", float_inteiro);
		  float_inteiro = (int32_t) (1000*param_delta.pf);
		  print("Fator de Potencia: %d\n\r", float_inteiro);
		  for(i=0; i<MAX_HARMONICA; i++)
		  {
			  print("Harmonica %d: %d\n\r",i,(int32_t)(1000*param_delta.harmonicos_RMS[i]));
		  }
		  float_inteiro = (int32_t) (1000*param_delta.thd);
		  print("THD: %d\n\r", float_inteiro);*/

		  estado = ID;
		  break;

	  case ID:
		  /*
		   * Estado: ID
		   * Descricao: Identificacao dos equipamentos na rede e associacao com a medicao
		   * Proximo estado: ENVIAR
		   * Transicao: Identificacao completa
		   */
		  /* Identificar o delta de medicoes com base de dados de equipamentos */
		  add_new_equip = IdentificarEquipamento(BaseDados, &param_delta);

		  /* Entrar como novo dado na memoria */
		  memoria_index = (memoria_index+1)%MEM_SIZE;
		  for(i=0; i<EQUIP_ARRAY_MAX; i++) {
			  memoria[memoria_index%MEM_SIZE].equipamentos[i] = memoria[(memoria_index-1)%MEM_SIZE].equipamentos[i];
		  }

		  /* Atualizar vetor de equipamentos com nova entrada */
		  if(add_new_equip != -1)
		  {
			  if (new_equip == 1)
			  {
				  memoria[memoria_index%MEM_SIZE].equipamentos[add_new_equip]++;
			  }
			  else
			  {
				  memoria[memoria_index%MEM_SIZE].equipamentos[add_new_equip]--;
			  }
		  }

		  /* Entrar como o restante de dados na memoria */
		  memoria[memoria_index%MEM_SIZE].med.i_rms = param_aux.i_rms;
		  memoria[memoria_index%MEM_SIZE].med.v_rms = param_aux.v_rms;
		  memoria[memoria_index%MEM_SIZE].med.pot_ap = param_aux.pot_ap;
		  memoria[memoria_index%MEM_SIZE].med.pot_at = param_aux.pot_at;
		  memoria[memoria_index%MEM_SIZE].med.pot_re = param_aux.pot_re;
		  memoria[memoria_index%MEM_SIZE].med.thd = param_aux.thd;
		  memoria[memoria_index%MEM_SIZE].med.pf = param_aux.pf;
		  for(i=0; i<MAX_HARMONICA; i++) {
			  memoria[memoria_index%MEM_SIZE].med.harmonicos_RMS[i] = param_aux.harmonicos_RMS[i];
		  }
		  memoria[memoria_index%MEM_SIZE].timestamp = timestamp;

		  // Adicionar struct de medicoes (antes do delta) a memoria de medicoes em {memoria_index%MEM_SIZE}
		  estado = ENVIAR;
		  break;

	  case ENVIAR:
		  /*
		   * Estado: ENVIAR
		   * Descricao: Enviar nova entrada na tabela pela UART (conectada a um PC ou ao ESP8266)
		   * Proximo estado: AQUISICAO
		   * Transicao: Envio completo
		   */
		  /* Enviar nova medicao pela UART*/
		  //print("Timestamp: %d\n\r", memoria[memoria_index%MEM_SIZE].timestamp);
		  float_inteiro = (int32_t) (1000*memoria[memoria_index%MEM_SIZE].med.i_rms);
		  //print("I RMS: %d\n\r", float_inteiro);
		  float_inteiro = (int32_t) (memoria[memoria_index%MEM_SIZE].med.v_rms);
		  //print("V RMS: %d\n\r", float_inteiro);
		  float_inteiro = (int32_t) (memoria[memoria_index%MEM_SIZE].med.pot_at);
		  //print("Potencia Ativa: %d\n\r", float_inteiro);
		  float_inteiro = (int32_t) (memoria[memoria_index%MEM_SIZE].med.pot_ap);
		  //print("Potencia Aparente: %d\n\r", float_inteiro);
		  float_inteiro = (int32_t) (memoria[memoria_index%MEM_SIZE].med.pot_re);
		  //print("Potencia Reativa: %d\n\r", float_inteiro);
		  float_inteiro = (int32_t) (1000*memoria[memoria_index%MEM_SIZE].med.pf);
		  //print("Fator de Potencia: %d\n\r", float_inteiro);
		  for(i=0; i<MAX_HARMONICA; i++)
		  {
			  //print("Harmonica %d: %d\n\r",i,(int32_t)(1000*memoria[memoria_index%MEM_SIZE].med.harmonicos_RMS[i]));
		  }
		  float_inteiro = (int32_t) (1000*memoria[memoria_index%MEM_SIZE].med.thd);
		  //print("THD: %d\n\r", float_inteiro);
		  for(i=0; i<EQUIP_ARRAY_MAX; i++) {
			  //print("Equipamento %d: %d\n\r", i, memoria[memoria_index%MEM_SIZE].equipamentos[i]);
		  }

		  // chamar funcao de envio usando {memoria_index%MEM_SIZE}
		  estado = AQUISICAO;
		  break;

	  default:
		  /*
		   * Estado: DEFAULT
		   * Proximo estado: START
		   * Transicao: sempre
		   */
		  estado = START;
		  break;
	  }
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
/*
 * Funcao: void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
 * @param hadc ponteiro para estrutura de controle de ADC
 * @retval void
 * @brief Callback de transferencia completa da DMA-ADC
 */
 /**
  * @brief  Callback da interrupção de transferência completa da DMA.
  * @param	hadc: Handler para ADC.
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	/* Verificar ADC1 (mestre) e ativar flag */
	if (hadc->Instance == ADC1) {
		flag_buffercheio = 1;
		flag_buffermetade = 0;
		/*count++;
		if (count==10) {tic = HAL_GetTick();}
		if (count==20) {
			toc = HAL_GetTick();
			print("Tempo: %d\n\r", toc-tic);
			count = 0;
		}*/

	}
}

/*
 * Funcao: void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
 * @param hadc ponteiro para estrutura de controle de ADC
 * @retval void
 * @brief Callback de transferencia pela meta da DMA-ADC
 */
 /**
  * @brief  Callback da interrupção de transferência na metade da DMA.
  * @param	hadc: Handler para ADC.
  */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	/* Verificar ADC1 (mestre) e ativar flag */
	if (hadc->Instance == ADC1) {
		flag_buffermetade = 1;
		flag_buffercheio = 0;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
