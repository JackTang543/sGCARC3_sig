#pragma once
#ifdef __cplusplus
extern "C"{
#endif


#include "stm32f4xx_hal.h"
#include "main.h"
#include "sLib.h"

void sBSP_TIM10_Init();
void sBSP_TIM10_SetPWMFreq(uint32_t freq);
void sBSP_TIM10_CH1SetEN(bool is_en);
void sBSP_TIM10_CH1SetDuty(float percent);







#ifdef __cplusplus
}
#endif
