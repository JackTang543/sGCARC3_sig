#pragma once

/*STM32*/
#include "stm32f4xx_hal.h"
#include "arm_math.h"
/*FreeRTOS*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
/*LVGL*/
#include "lv_conf.h"
#include "lvgl.h"

#include "sLib.h"
#include "sGenBtnDrv2.h"

#include "string.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdarg.h"

/*应用层*/
#include "sAPP_Task.h"
#include "sAPP_Anim.h"
#include "sAPP_Func.h"
#include "sAPP_UI.h"
#include "sAPP_Cb.h"

#include "sHMI_Debug.h"
#include "sHMI_Btns.h"

#include "sDRV_ST7789V.h"

#include "sBSP_SYS.h"
#include "sBSP_SPI.h"
#include "sBSP_Timebase.h"
#include "sBSP_TIM.h"

#include "sDRV_ICM42688.h"
#include "sDRV_LIS3MDLTR.h"



extern SemaphoreHandle_t g_screen_rending;


#define LVGL_DRAW_BUF_SIZE (SDRV_ST7789V_LCDH * SDRV_ST7789V_LCDW / 10 * (LV_COLOR_DEPTH / 8))
extern uint8_t g_lvgl_buf1[LVGL_DRAW_BUF_SIZE / 4];
extern uint8_t g_lvgl_buf2[LVGL_DRAW_BUF_SIZE / 4];
extern lv_display_t* g_screen;


extern lv_obj_t* label;
extern lv_obj_t* label1;

extern sLIB_6AXIS_INPUT_t input;
extern sLIB_ATTITUDE_RESULT_t result;
extern sLIB_6AXIS_INPUT_t bias;




void update_screen();



void Error_Handler();
void assert_failed(uint8_t* file, uint32_t line);
void vApplicationMallocFailedHook();
void vApplicationIdleHook();
void vApplicationTickHook();
void vApplicationStackOverflowHook( TaskHandle_t xTask,char * pcTaskName );




#define KEY4_Pin GPIO_PIN_13
#define KEY4_GPIO_Port GPIOC
#define ICM_CS_Pin GPIO_PIN_0
#define ICM_CS_GPIO_Port GPIOC
#define AHRS_MOSI_Pin GPIO_PIN_1
#define AHRS_MOSI_GPIO_Port GPIOC
#define AHRS_MISO_Pin GPIO_PIN_2
#define AHRS_MISO_GPIO_Port GPIOC
#define LIS3_CS_Pin GPIO_PIN_3
#define LIS3_CS_GPIO_Port GPIOC
#define GMR_RA_Pin GPIO_PIN_0
#define GMR_RA_GPIO_Port GPIOA
#define GMR_RB_Pin GPIO_PIN_1
#define GMR_RB_GPIO_Port GPIOA
#define MR_PWM1_Pin GPIO_PIN_2
#define MR_PWM1_GPIO_Port GPIOA
#define MR_PWM2_Pin GPIO_PIN_3
#define MR_PWM2_GPIO_Port GPIOA
#define IPS_CS_Pin GPIO_PIN_4
#define IPS_CS_GPIO_Port GPIOA
#define IPS_SCK_Pin GPIO_PIN_5
#define IPS_SCK_GPIO_Port GPIOA
#define GMR_LA_Pin GPIO_PIN_6
#define GMR_LA_GPIO_Port GPIOA
#define IPS_MOSI_Pin GPIO_PIN_7
#define IPS_MOSI_GPIO_Port GPIOA
#define SPL06_CS_Pin GPIO_PIN_4
#define SPL06_CS_GPIO_Port GPIOC
#define ADC_VBAT_Pin GPIO_PIN_1
#define ADC_VBAT_GPIO_Port GPIOB
#define QSPI_CLK_Pin GPIO_PIN_2
#define QSPI_CLK_GPIO_Port GPIOB
#define KEY3_Pin GPIO_PIN_12
#define KEY3_GPIO_Port GPIOB
#define AHRS_SCK_Pin GPIO_PIN_13
#define AHRS_SCK_GPIO_Port GPIOB
#define LIGHT1_Pin GPIO_PIN_14
#define LIGHT1_GPIO_Port GPIOB
#define LIGHT2_Pin GPIO_PIN_15
#define LIGHT2_GPIO_Port GPIOB
#define I2C_SCL_Pin GPIO_PIN_6
#define I2C_SCL_GPIO_Port GPIOC
#define I2C_SDA_Pin GPIO_PIN_7
#define I2C_SDA_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_8
#define KEY1_GPIO_Port GPIOC
#define QSPI_IO0_Pin GPIO_PIN_9
#define QSPI_IO0_GPIO_Port GPIOC
#define MCO_ICM_32K_Pin GPIO_PIN_8
#define MCO_ICM_32K_GPIO_Port GPIOA
#define DEBUG_RX_Pin GPIO_PIN_9
#define DEBUG_RX_GPIO_Port GPIOA
#define DEBUG_RXA10_Pin GPIO_PIN_10
#define DEBUG_RXA10_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define ML_PWM1_Pin GPIO_PIN_15
#define ML_PWM1_GPIO_Port GPIOA
#define QSPI_IO1_Pin GPIO_PIN_10
#define QSPI_IO1_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_12
#define KEY2_GPIO_Port GPIOC
#define SPI3_CS_Pin GPIO_PIN_2
#define SPI3_CS_GPIO_Port GPIOD
#define IPS_DC_Pin GPIO_PIN_4
#define IPS_DC_GPIO_Port GPIOB
#define GMR_LB_Pin GPIO_PIN_5
#define GMR_LB_GPIO_Port GPIOB
#define QSPI_NCS_Pin GPIO_PIN_6
#define QSPI_NCS_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_7
#define BUZZER_GPIO_Port GPIOB
#define IPS_BLK_Pin GPIO_PIN_8
#define IPS_BLK_GPIO_Port GPIOB
#define ML_PWM2_Pin GPIO_PIN_9
#define ML_PWM2_GPIO_Port GPIOB













