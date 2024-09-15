#pragma once
#ifdef __cplusplus
extern "C"{
#endif

#include "stm32f4xx_hal.h"

/*显示范围偏移(0~255)*/
#define SDRV_ST7789V_OFFSETW          (40u)
#define SDRV_ST7789V_OFFSETH          (52u)

/*LCD显示区域长宽*/
#define SDRV_ST7789V_LCDW             (135u)
#define SDRV_ST7789V_LCDH             (240u)

/**** Common 16bit color codes ***************/
#define	ST7789_BLACK   0x0000
#define	ST7789_BLUE    0x001F
#define	ST7789_RED     0xF800
#define	ST7789_GREEN   0x07E0
#define ST7789_CYAN    0x07FF
#define ST7789_MAGENTA 0xF81F
#define ST7789_YELLOW  0xFFE0
#define ST7789_WHITE   0xFFFF


//用于初始化命令序列
typedef struct{
    uint8_t  comm;     //命令字
    uint16_t wait_ms;  //需要延时的时间
    uint8_t  data_size;//这个命令后面有多少个附带数据
    uint8_t* pData;    //附带数据
}sDRV_ST7789V_CommSeq_t;



int sDRV_ST7789V_Init();
void sDRV_ST7789V_SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
void sDRV_ST7789V_SetPixel(uint16_t x, uint16_t y, uint16_t color);
void sDRV_ST7789V_SendColorArea(uint8_t *color_p, uint32_t size);



#ifdef __cplusplus
}
#endif


