#include "stm32f4xx_adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"
#include "misc.h"

static __IO uint32_t TimingDelay;
int W=0;
int x=0;
int y=0;
int dzi=0;
int jed=0;
int f=0;

void gpio_init(void);
void adc_czujniki(void);
void adc1(void);
void wyswietlacz(void);
void InitializeTimer(void);
void Timer(void);
void EnableTimerInterrupt(void);
void Delay(__IO uint32_t nTime);

int main(void)
{

	SystemInit();
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	gpio_czujniki();
	adc_init();
	adc1();
	wyswietlacz();
	wyswietlacz_przelacznik();
	InitializeTimer();
	Timer();
	EnableTimerInterrupt();


	while(1)
    {


    	ADC_SoftwareStartConv(ADC1);
    	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    	x = ADC_GetConversionValue(ADC1);

    	if((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))==1)
    	{
    		f++;
    		if(f>2) f=0;

    		switch (f){
    		case 0:
    	    	ADC_Cmd(ADC1, DISABLE);
    	    	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);
    	   		ADC_Cmd(ADC1, ENABLE);
    	   		dzi=11;
    	   		jed=1;
    	   		Delay(3);
    	   	break;
    		case 1:
    		    	ADC_Cmd(ADC1, DISABLE);
    		    	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_84Cycles);
    		   		ADC_Cmd(ADC1, ENABLE);
    		   		dzi=11;
    		   		jed=2;
    		   		Delay(3);
    		break;
    		case 2:
    		    	ADC_Cmd(ADC1, DISABLE);
    		    	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_84Cycles);
    		   		ADC_Cmd(ADC1, ENABLE);
    		   		dzi=11;
    		   		jed=3;
    		   		Delay(3);
    		 break;
    		}
    	   }

    	switch (f){
	case 0:
		y=naswietlenie(x);

    break;
	case 1:
		y=temperatura(x);
	break;
	case 2:
		y=wilgotnosc(x);
	break;
    	}

    	dzi=y/10;
    	jed=y%10;

    	Delay(1);


 }
}

//CZUJNIKI//
int naswietlenie(int x)
{
	if(x>=4000)
		return 0;
	if(x<=700)
		return 99;
	else
		return (4000-x)/33;

}


int wilgotnosc(int x)
{


	if(x>=1900)
		return 90;
	if(x<=600)
		return 20;
	else
	{
		return 90-((1900-x)/20)-5;
	}

}
int n=0;

int temperatura(int x)
{


	if(x<1656)
		return n;
	if(x>=1656 && x<1797)
	{
		n=(x-1656)/15,66;
		return 0+n;
	}
	if(x>=1797 && x<1944)
	{
		n=(x-1797)/16,33;
		return 10+n;
	}
	if(x>=1944 && x<2020)
	{
		n=(x-1944)/19;
		return 20+n;
	}
	if(x>=2020 && x<2102)
	{
		n=(x-2020)/20,5;
		return 25+n;
	}
	if(x>=2102 && x<2272)
	{
		n=(x-2102)/18,88;
		return 30+n;
	}
	if(x>=2272 && x<2451)
	{
		n=(x-2272)/19,88;
		return 40+n;
	}
	if(x>=2451 && x<2637)
	{
		n=(x-2451)/20,66;
		return 50+n;
	}
	if(x>=2637 && x<2832)
	{
		n=(x-2637)/21,66;
		return 60+n;
	}
	if(x>=2832 && x<3035)
	{
		n=(x-2832)/22,55;
		return 70+n;
	}
	if(x>=3035 && x<3246)
	{
		n=(x-3025)/23,44;
		return 80+n;
	}
	if(x>=3246 && x<3466)
	{
		n=(x-3246)/24,44;
		return 90+n;
	}
	if(x>=3466)
		return 99;

}

void gpio_czujniki(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//inicjalizacja wejœcia ADC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void adc_init(void)
{
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	// niezale¿ny tryb pracy przetworników
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	// zegar g³ówny podzielony przez 2
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	// opcja istotna tylko dla tryby multi ADC
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	// czas przerwy pomiêdzy kolejnymi konwersjami
	ADC_CommonInitStructure.ADC_TwoSamplingDelay =
	ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

}

void adc1(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	//ustawienie rozdzielczoœci przetwornika na maksymaln¹ (12 bitów)
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	//wy³¹czenie trybu skanowania (odczytywaæ bêdziemy jedno wejœcie ADC
	//w trybie skanowania automatycznie wykonywana jest konwersja na wielu
	//wejœciach/kana³ach)
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	//w³¹czenie ci¹g³ego trybu pracy
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	//wy³¹czenie zewnêtrznego wyzwalania
	//konwersja mo¿e byæ wyzwalana timerem, stanem wejœcia itd. (szczegó³y w
	//dokumentacji)
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_ExternalTrigConvEdge =
	ADC_ExternalTrigConvEdge_None;
	//wartoœæ binarna wyniku bêdzie podawana z wyrównaniem do prawej
	//funkcja do odczytu stanu przetwornika ADC zwraca wartoœæ 16-bitow¹
	//dla przyk³adu, wartoœæ 0xFF wyrównana w prawo to 0x00FF, w lewo 0x0FF0
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	//liczba konwersji równa 1, bo 1 kana³ Strona 4 z 7

	ADC_InitStructure.ADC_NbrOfConversion = 1;
	// zapisz wype³nion¹ strukturê do rejestrów przetwornika numer 1
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);
		ADC_Cmd(ADC1, ENABLE);

	}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////


//WYSWIETLACZ//

void wyswietlacz(void){
	GPIO_InitTypeDef GPIO_InitStructure;
     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_15 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void wyswietlacz_przelacznik(void){
	GPIO_InitTypeDef GPIO_InitStructure;
     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void liczby(int l)
{
	switch (l){
	case 0:
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		GPIO_ResetBits(GPIOA,GPIO_Pin_10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
		break;
	case 1:
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
		GPIO_SetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
		GPIO_SetBits(GPIOA,GPIO_Pin_7);
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
		break;
	case 2:
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		GPIO_ResetBits(GPIOA,GPIO_Pin_10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		GPIO_SetBits(GPIOA,GPIO_Pin_15);
		GPIO_SetBits(GPIOA,GPIO_Pin_7);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		break;
	case 3:
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
		GPIO_SetBits(GPIOA,GPIO_Pin_7);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		break;
	case 4:
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
		GPIO_SetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		break;
	case 5:
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		GPIO_SetBits(GPIOA,GPIO_Pin_8);
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		break;
	case 6:
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		GPIO_SetBits(GPIOA,GPIO_Pin_8);
		GPIO_ResetBits(GPIOA,GPIO_Pin_10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		break;
	case 7:
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
		GPIO_SetBits(GPIOA,GPIO_Pin_7);
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
		break;
	case 8:
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		GPIO_ResetBits(GPIOA,GPIO_Pin_10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		break;
	case 9:
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		GPIO_SetBits(GPIOA,GPIO_Pin_10);
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		break;
	case 11://F
			GPIO_SetBits(GPIOA,GPIO_Pin_6);
			GPIO_SetBits(GPIOA,GPIO_Pin_8);
			GPIO_ResetBits(GPIOA,GPIO_Pin_10);
			GPIO_ResetBits(GPIOA,GPIO_Pin_4);
			GPIO_SetBits(GPIOA,GPIO_Pin_15);
			GPIO_ResetBits(GPIOA,GPIO_Pin_7);
			GPIO_ResetBits(GPIOA,GPIO_Pin_5);
			break;
	}
}

void Timer()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 84-1;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 1000-1;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &timerInitStructure);
    TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}

void InitializeTimer()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 84-1;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 1000-1;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
    TIM_Cmd(TIM2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void EnableTimerInterrupt()
{
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}

void TIM2_IRQHandler()
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {

    	switch (W)
    		 {
    		 case 0:
    		 GPIO_ResetBits(GPIOB,GPIO_Pin_0);
    		 GPIO_SetBits(GPIOB,GPIO_Pin_1);
    		 liczby(dzi);
    		 W++;
    		 break;

    		 case 1:
    		GPIO_ResetBits(GPIOB,GPIO_Pin_1);
    		GPIO_SetBits(GPIOB,GPIO_Pin_0);
    		liczby(jed);
    		W=0;
    		 break;
    		 }


        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

    }
}


void Delay(__IO uint32_t nTime)
{

	int i=0;
	for(i=0;i<215*nTime;i++)
	{
  TIM3->CNT=0;

  while(TIM3->CNT<900);
	}


}

