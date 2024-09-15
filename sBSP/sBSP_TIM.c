#include "sBSP_TIM.h"

/**
 * STM32F446RET6
 * 2024.09.15
 * 
 */


//TIM10 APB2时钟频率 180MHz
#define TIM10_CLK_FREQ (180000000)
//TIM10用于PWM控制IPS屏幕背光
TIM_HandleTypeDef htim10;
//PWM分辨率0.01% ARR=9999
const uint32_t TIM10_ARRVal = 9999;
const uint32_t TIM10_PSCVal = 0;


void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle);


void sBSP_TIM10_Init(){
    __TIM10_CLK_ENABLE();
    htim10.Instance               = TIM10;
    htim10.Init.Prescaler         = TIM10_PSCVal;
    htim10.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim10.Init.Period            = TIM10_ARRVal;
    htim10.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if(HAL_TIM_Base_Init(&htim10) != HAL_OK){
        Error_Handler();
    }
    if(HAL_TIM_PWM_Init(&htim10) != HAL_OK){
        Error_Handler();
    }
    TIM_OC_InitTypeDef oc = {0};
    oc.OCMode     = TIM_OCMODE_PWM1;
    oc.Pulse      = 0;
    oc.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim10, &oc, TIM_CHANNEL_1) != HAL_OK){
        Error_Handler();
    }

    HAL_TIM_MspPostInit(&htim10);
}

//freq 1~18KHz
void sBSP_TIM10_SetPWMFreq(uint32_t freq){
    if(freq > 18000){freq = 18000;}
    if(freq < 1)    {freq = 1;}
    __HAL_TIM_SET_PRESCALER(&htim10,((TIM10_CLK_FREQ / (freq * (TIM10_ARRVal + 1))) - 1));
}

void sBSP_TIM10_CH1SetEN(bool is_en){
    if(is_en){
        HAL_TIM_PWM_Start(&htim10,TIM_CHANNEL_1);
    }else{
        HAL_TIM_PWM_Stop(&htim10,TIM_CHANNEL_1);
    }
}

//duty范围:0~100%
void sBSP_TIM10_CH1SetDuty(float percent){
    //把0~100映射到0~9999
    __HAL_TIM_SET_COMPARE(&htim10,TIM_CHANNEL_1,(uint32_t)(sLib_Fmap(percent,0,100,0,9999)));
}



void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle){
    GPIO_InitTypeDef gpio = {0};


    if(timHandle->Instance == TIM10){
        __HAL_RCC_GPIOB_CLK_ENABLE();
        //IPS_BLK -> PB8 -> TIM10_CH1
        gpio.Pin = GPIO_PIN_8;
        gpio.Mode = GPIO_MODE_AF_PP;
        gpio.Pull = GPIO_NOPULL;
        gpio.Speed = GPIO_SPEED_FREQ_LOW;
        gpio.Alternate = GPIO_AF3_TIM10;
        HAL_GPIO_Init(GPIOB, &gpio);
    }
}


