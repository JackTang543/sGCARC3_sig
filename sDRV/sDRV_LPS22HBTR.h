#pragma once
#ifdef __cplusplus
extern "C"{
#endif

#include "stm32f4xx_hal.h"


extern float g_lps22_pa;
extern float g_lps22_temp;


//输出速率配置 应用位段:MSK_CTRLREG1_ODR
typedef enum{
    SDRV_LPS22_ODR_POWERDOWN     = 0b000,       //关机模式/单次模式使能
    SDRV_LPS22_ODR_1HZ           = 0b001,
    SDRV_LPS22_ODR_10HZ          = 0b010,
    SDRV_LPS22_ODR_25HZ          = 0b011,
    SDRV_LPS22_ODR_50HZ          = 0b100,
    SDRV_LPS22_ODR_75HZ          = 0b101,
}sDRV_LPS22_ODR_t;

//FIFO模式配置 应用位段:MSK_FIFOCTRL_FMODE
typedef enum{
    SDRV_LPS22_FIFOMODE_BYPASS         = 0b000,
    SDRV_LPS22_FIFOMODE_FIFO           = 0b001,
    SDRV_LPS22_FIFOMODE_STREAM         = 0b010,
    SDRV_LPS22_FIFOMODE_STREAM_FIFO    = 0b011,
    SDRV_LPS22_FIFOMODE_BYPASS_STREAM  = 0b100,
    SDRV_LPS22_FIFOMODE_DYNAMIC_STREAM = 0b110,
    SDRV_LPS22_FIFOMODE_BYPASS_FIFO    = 0b111,
}sDRV_LPS22_FIFOMODE_t;



int sDRV_LPS22_Init();

void sDRV_LPS22_GetData();
float sDRV_LPS22_GetTemp();
float sDRV_LPS22_GetPress();



#ifdef __cplusplus
}
#endif


