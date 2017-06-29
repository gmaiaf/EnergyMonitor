/**
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
#include "adc_util.h"
// .h do Bruno
// .h do Andre
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
/* Variavel de estado da FSM */
enum FSM {START, AQUISICAO, RMS_CORRENTE, CALCULOS, DELTA, ID, ENVIAR}
estado = START;
/* Tabela de medicoes e indexador*/
uint32_t * memoria[MEM_SIZE]; // mudar para .h do Bruno
uint32_t memoria_index = 0;
/* Buffers usados ao longo do processamento */
uint32_t buffer_tensao_DMA[2*BUFFER_SIZE];
uint32_t buffer_corrente_DMA[2*BUFFER_SIZE];
uint32_t buffer_tensao_leitura[BUFFER_SIZE];
uint32_t buffer_corrente_leitura[BUFFER_SIZE];
float32_t buffer_corrente_float[BUFFER_SIZE];
float32_t buffer_tensao_float[BUFFER_SIZE];
float32_t buffer_corrente_FIR[BUFFER_SIZE];
float32_t buffer_tensao_diz[BUFFER_SIZE/DIZIMACAO];
float32_t buffer_corrente_diz[BUFFER_SIZE/DIZIMACAO];
/* Flags de controle */
uint8_t flag_buffercheio = 0;
uint8_t flag_buffermetade = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
/* Callbacks das interrupcoes do ADC-DMA */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *);
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  /* Variaveis locais */
	float32_t corrente_RMS = 0;
	float32_t corrente_RMS_anterior = 0;
	int i = 0;
	// tres structs de medicao do .h do Bruno auxiliares
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
		   * Proximo estado: RMS_CORRENTE
		   * Transicao: transferencia do buffer do ADC na metade ou completa
		   */
		  if (flag_buffercheio == 1){
			  /* Timestamp da medicao */
			  // Obter timestamp da medicao

			  /* Transferir dados do buffer da DMA para buffer de leitura e esperar transferencia*/
			  HAL_DMA_Start(&hdma_memtomem_dma2_stream3, (uint32_t) (buffer_corrente_DMA+BUFFER_SIZE), (uint32_t) buffer_corrente_leitura,BUFFER_SIZE);
			  HAL_DMA_Start(&hdma_memtomem_dma2_stream1, (uint32_t) (buffer_tensao_DMA+BUFFER_SIZE), (uint32_t) buffer_tensao_leitura,BUFFER_SIZE);
			  HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream3, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);

			  /* Converter valor do ADC 0-4095 para float32_t */
			  ADCConvertBuffer(buffer_corrente_leitura, buffer_corrente_float, BUFFER_SIZE, CORRENTE_A, CORRENTE_B);

			  /* Filtro FIR */
			  // Aplicar filtro FIR ao {buffer_corrente_float}. Saida em {buffer_corrente_FIR}
			  /* Dizimacao da corrente */
			  // Aplicar funcao de dizimacao ao {buffer_corrente_FIR}. Saida em {buffer_corrente_diz}

			  HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream1, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
			  /* Converter valor do ADC 0-4095 para float32_t */
			  ADCConvertBuffer(buffer_tensao_leitura, buffer_tensao_float, BUFFER_SIZE, TENSAO_A, TENSAO_B);

			  /* Dizimacao da tensao */
			  // Aplicar funcao de dizimacao ao {buffer_tensao_float}. Saida em {buffer_tensao_diz}
			  estado = RMS_CORRENTE;

			  // enviar valor pela UART - DEBUG - UART ESTA LENTA
			  for(i=0; i<BUFFER_SIZE/2; i++)
			  {
				  /*print("%d%c%d%d%", (uint32_t)(buffer_corrente_float[i])%10, '.', ((uint32_t)(buffer_corrente_float[i]*10)%10), ((uint32_t)(buffer_corrente_float[i]*100)%10));
				  USART2_Transmit_Char(',');
				  print("%d%c%d%d%", (uint32_t)(buffer_tensao_float[i])%10, '.', ((uint32_t)(buffer_tensao_float[i]*10)%10), ((uint32_t)(buffer_tensao_float[i]*100)%10));
				  USART2_Transmit_String("\n\r");*/

				  /*USART2_Transmit_UInt(buffer_tensao_leitura[i]);
				  USART2_Transmit_Char(',');
				  USART2_Transmit_UInt(buffer_corrente_leitura[i]);
				  USART2_Transmit_String("\n\r");*/
			  }


		  }
		  if (flag_buffermetade == 1){
			  /* Timestamp da medicao */
			  // Obter timestamp da medicao

			  /* Transferir dados do buffer da DMA para buffer de leitura e esperar transferencia*/
			  HAL_DMA_Start(&hdma_memtomem_dma2_stream3, (uint32_t) buffer_corrente_DMA, (uint32_t) buffer_corrente_leitura, BUFFER_SIZE);
			  HAL_DMA_Start(&hdma_memtomem_dma2_stream1, (uint32_t) buffer_tensao_DMA, (uint32_t) buffer_tensao_leitura, BUFFER_SIZE);

			  HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream3, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);

			  /* Converter valor do ADC 0-4095 para float32_t */
			  ADCConvertBuffer(buffer_corrente_leitura, buffer_corrente_float, BUFFER_SIZE, CORRENTE_A, CORRENTE_B);
			  /* Filtro FIR */
			  // Aplicar filtro FIR ao {buffer_corrente_leitura}. Saida em {buffer_corrente_leitura_FIR}
			  /* Dizimacao da corrente */
			  // Aplicar funcao de dizimacao ao {buffer_corrente_leitura_FIR}. Saida em {buffer_corrente_diz}

			  HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream1, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
			  /* Converter valor do ADC 0-4095 para float32_t */
			  ADCConvertBuffer(buffer_tensao_leitura, buffer_tensao_float, BUFFER_SIZE, TENSAO_A, TENSAO_B);

			  /* Dizimacao da tensao */
			  // Aplicar funcao de dizimacao ao {buffer_tensao_leitura}. Saida em {buffer_tensao_diz}

			  estado = RMS_CORRENTE;

			  // enviar valor pela UART - DEBUG - UART ESTA LENTA
			  for(i=0; i<BUFFER_SIZE/2; i++)
			  {
				  /*print("%d%c%d%d%", (uint32_t)(buffer_corrente_float[i])%10, '.', ((uint32_t)(buffer_corrente_float[i]*10)%10), ((uint32_t)(buffer_corrente_float[i]*100)%10));
				  USART2_Transmit_Char(',');
				  print("%d%c%d%d%", (uint32_t)(buffer_tensao_float[i])%10, '.', ((uint32_t)(buffer_tensao_float[i]*10)%10), ((uint32_t)(buffer_tensao_float[i]*100)%10));
				  USART2_Transmit_String("\n\r");*/

				  /*USART2_Transmit_UInt(buffer_tensao_leitura[i]);
				  USART2_Transmit_Char(',');
				  USART2_Transmit_UInt(buffer_corrente_leitura[i]);
				  USART2_Transmit_String("\n\r");*/
			  }

		  }
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
		  /* Calcular RMS da corrente sobre a ultima medicao */
		  // Aplicar calculo de RMS sobre {buffer_corrente_leitura_FIR}. Saida em {corrente_RMS}
		  /* Ler ultimo valor de RMS */
		  // Usar {memoria_index%MEM_SIZE}. Saida em {corrente_RMS_anterior}*/
		  /* Verificar se novo valor de RMS esta dentro ou fora de uma faixa */
		  if ((corrente_RMS < RMS_LOWERBOUND*corrente_RMS_anterior) || (corrente_RMS > RMS_UPPERBOUND*corrente_RMS_anterior)) {
			  estado = CALCULOS;
		  }
		  else {
			  estado = AQUISICAO;
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
		  // Aplicar todas as funcoes de calculos eletricos do .h do Andre e salvar na struct do .h do Bruno
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
		  // Ler struct do .h do Bruno usando {memoria_index%MEM_SIZE}
		  // Aplicar funcao de delta nas duas structs
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
		  // Aplicar funcao de classificacao com struct delta
		  // Verificar qual equipamento foi adicionado ou removido e atualizar vetor auxiliar de equipamentos
		  /* Entrar como novo dado na memoria */
		  memoria_index = (memoria_index+1)%MEM_SIZE;
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
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	/* Verificar ADC1 (mestre) e ativar flag */
	if (hadc->Instance == ADC1) {
		flag_buffercheio = 1;
		flag_buffermetade = 0;
	}
}

/*
 * Funcao: void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
 * @param hadc ponteiro para estrutura de controle de ADC
 * @retval void
 * @brief Callback de transferencia pela meta da DMA-ADC
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
