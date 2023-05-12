/*******************************************************************************
 *
 * Description: Assignment4
 * Project name: ABL_MCU_V1.0.0
 *
 *
 * Last Changed By:  $Author: TrungNT $
 * Revision:         $Revision: 1.0 $
 * Last Changed:     $Date: 15/11/2022 $
 *
 ******************************************************************************/

/**Libraries******************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stm32f401re_rcc.h>
#include <stm32f401re_adc.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_tim.h>
#include <timer.h>
#include <lightsensor.h>
#include <kalman_filter.h>
#include <Ucglib.h>
#include <ucg.h>
#include <string.h>
/* Private defined*****************************************************************/
#define ADC_PORT				    		GPIOC
#define ADC_PIN             			    GPIO_Pin_5
#define Tim_Period      			  		8399
/* Private variables**************************************************************/
uint16_t AdcValue=0;
static uint16_t lightLevelAfterFilter = 0;
static uint16_t Kanman_light=0;
static char src3[20] = "";
static ucg_t ucg;
/* Private function prototypes****************************************************/
static void AppInitCommon(void);
static void LightSensor_AdcInit(void);
static void LedControl_TimerOCInit(void);
uint16_t LightSensor_AdcPollingRead(void);
uint16_t Kanman_Light(uint16_t lightLevel);
static void LedControl_TimerOCSetPwm(uint32_t Compare);
static void ABL_Process(void);
int main(void)
{
	AppInitCommon();

	while(1)
	{
		processTimerScheduler(); //Xử lý các sự kiện theo thời gian đã được cài đặt.
		ABL_Process(); // thay đổi độ sáng của led RGB theo cường độ ánh sáng với chu kỳ 100 ms.
	}
}

/**
 * @func   AppInitCommon
 * @brief  Application initializer
 * @param  None
 * @retval None
 */
static void AppInitCommon(void)
{
	SystemCoreClockUpdate(); 	//Cấp nguồn clock cho vi điều khiển là 84 Mhz.
	TimerInit();				//Khởi tạo timer sử dụng system tick.
	LightSensor_AdcInit();		//Khởi tạo ngoại vi ADC ở chế độ Polling.
	LedControl_TimerOCInit();   //Khởi tạo ngoại vi TIMER ở chế độ băm xung Output Compare.

	KalmanFilterInit(2, 2, 0.001); // Khởi tạo bộ lọc Kalman

	Ucglib4WireSWSPI_begin(&ucg, UCG_FONT_MODE_SOLID);
	ucg_SetFont(&ucg, ucg_font_ncenR10_hf);
	ucg_ClearScreen(&ucg);
	ucg_SetColor(&ucg, 0, 255, 255, 255);
	ucg_SetColor(&ucg, 1, 0, 0, 0);
	ucg_SetRotate180(&ucg);

}

/**
 * @func   LightSensor_AdcInit
 * @brief  Light Sensor ADC Polling initialize
 * @param  None
 * @retval None
 */
static void LightSensor_AdcInit(void)
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable peripheral clocks ***********************************************/
	RCC_APB2PeriphClockCmd(ADCx_CLK, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);

	/* ADC Deinit ***************************************************/
	ADC_DeInit();

	/* ADC Common Init ********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 Init **************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	//Polling Configure
	ADC_EOCOnEachRegularChannelCmd(ADC1, ENABLE);
	ADC_ContinuousModeCmd(ADC1, DISABLE);
	ADC_DiscModeChannelCountConfig(ADC1, 1);
	ADC_DiscModeCmd(ADC1, ENABLE);


	/* ADC1 regular channel15 configuration ************************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_15Cycles);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
}

/**
 * @func   LedControl_TimerOCInit
 * @brief  Control Led with Timer Output Capture
 * @param  None
 * @retval None
 */
static void LedControl_TimerOCInit(void)
{
	GPIO_InitTypeDef 			GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef 	TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef			TIM_OC_InitStruct;


	// GPIO Configure
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_TIM1);

	//TimeBase Configure

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;

	/************************************************************************
	 * 	FPwm = Fclk / (ARR+1) * (PSC + 1)																	*
	 ************************************************************************/
	TIM_TimeBaseInitStruct.TIM_Period = 8399; // ARR = 8399
	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);


	//TimeIC Configure
	TIM_OC_InitStruct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OC_InitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC_InitStruct.TIM_Pulse = 0;
	TIM_OC_InitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC4Init(TIM1, &TIM_OC_InitStruct);

	TIM_Cmd(TIM1, ENABLE);


	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/**
 * @func   LightSensor_AdcPollingRead
 * @brief  Read value from ADC
 * @param  None
 * @retval None
 */
uint16_t LightSensor_AdcPollingRead(void)
{
	uint16_t result = 0;

	//  Bắt đầu quá trình chuyển đổi dữ liệu ADC.
	ADC_SoftwareStartConv(ADCx_SENSOR);

	// Đợi quá trình chuyển đổi được hoàn tất để đọc dữ liệu.
	while (ADC_GetFlagStatus(ADCx_SENSOR, ADC_FLAG_EOC) == RESET);

	// Read value
	result = ADC_GetConversionValue(ADCx_SENSOR);

	return result;

}

/**
 * @func   Kanman_Light
 * @brief  Use Kanman filter for Light Sensor
 * @param  None
 * @retval None
 */
uint16_t Kanman_Light(uint16_t lightLevel)
{
	lightLevelAfterFilter = KalmanFilter_updateEstimate(lightLevel);

	return lightLevelAfterFilter;
}

/**
 * @func   LedControl_TimerOCSetPwm
 * @brief  Control Led with PWM
 * @param  None
 * @retval None
 */
static void LedControl_TimerOCSetPwm(uint32_t Compare)
{
	TIM_SetCompare4(TIM1,Compare);
}
/**
 * @func   ABL_Process
 * @brief  AutoBrightness use Light Sensor
 * @param  None
 * @retval None
 */
static void ABL_Process(void)
{
	uint32_t dwTimeCurrent;
	static uint32_t dwTimeTotal, dwTimeInit;

	dwTimeCurrent = GetMilSecTick();

	if(dwTimeCurrent >= dwTimeInit)
	{
		dwTimeTotal += dwTimeCurrent - dwTimeInit;
	}
	else
	{
		dwTimeTotal += 0xFFFFFFFFU - dwTimeCurrent + dwTimeInit;
	}

	if(dwTimeTotal >= 100)
	{
		dwTimeTotal = 0;
		AdcValue  = LightSensor_AdcPollingRead(); //Đọc giá trị ADC đo được từ cảm biến ánh sáng
		Kanman_light = Kanman_Light(AdcValue);


		LedControl_TimerOCSetPwm(Kanman_light);
		memset(src3, 0, sizeof(src3));
		sprintf(src3, " Light = %d lux  ", Kanman_light);
		ucg_DrawString(&ucg, 0, 72, 0, src3);
	}
	dwTimeInit = dwTimeCurrent;
}
