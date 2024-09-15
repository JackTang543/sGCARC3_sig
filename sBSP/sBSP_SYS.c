#include "sBSP_SYS.h"

void sBSP_SYS_Init(){
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_Init();
    sBSP_RCC_Init();

    HAL_InitTick(0);
}


