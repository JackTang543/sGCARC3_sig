#include "sBSP_SPI.h"

//10dof f411

/**
 * SPI资源分配
 * 
 * SPI1 -> ST7789V
 * SCK  -> PA5
 * MOSI -> PA7
 * NSS  -> PA4
 * 
 * SPI2 -> 10DoF
 * SCK  -> PB10
 * MOSI -> PB15
 * MISO -> PB14
 * 
 * 
 * 
 * 
 */

//GPIOA
#define SPI1_SCK_PIN    GPIO_PIN_5
#define SPI1_MOSI_PIN   GPIO_PIN_7
#define SPI1_NSS_PIN    GPIO_PIN_4

//GPIOB
#define SPI2_SCK_PIN    GPIO_PIN_13
//GPIOC
#define SPI2_MOSI_PIN   GPIO_PIN_1
#define SPI2_MISO_PIN   GPIO_PIN_2



SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

//SPI_BAUDRATEPRESCALER_2 ~ SPI_BAUDRATEPRESCALER_256
int8_t sBSP_SPI1M_Init(uint32_t SPI_BAUDRATE){
    hspi1.Instance = SPI1;
    //SPI主模式
    hspi1.Init.Mode = SPI_MODE_MASTER;
    //单线
    hspi1.Init.Direction = SPI_DIRECTION_1LINE;
    //数据大小8Bit
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    //时钟极性,空闲低:CPOL(Clock Polarity):0
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    //时钟相位,第一个边沿采样:CPHA(Clock Phase):0
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    //NSS信号
    hspi1.Init.NSS = SPI_NSS_SOFT;
    //SPI频率
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATE;
    //高位先行
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    //TI(Texas Instruments)模式关闭
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    //CRC校验关闭
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    //CRC多项式7
    hspi1.Init.CRCPolynomial = 10;

    //模式0,CPOL = 0 CPHA = 0
    if(HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
    //__HAL_SPI_ENABLE(&hspi1);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1);
    return 0;
}

//SPI_BAUDRATEPRESCALER_2 ~ SPI_BAUDRATEPRESCALER_256
void sBSP_SPI2M_Init(uint32_t SPI_BAUDRATE){
    hspi2.Instance = SPI2;
    //SPI主模式
    hspi2.Init.Mode = SPI_MODE_MASTER;
    //双线全双工
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    //数据大小8Bit
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    //时钟极性,CPOL(Clock Polarity)
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    //时钟相位,CPHA(Clock Phase):0
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    //NSS信号
    hspi2.Init.NSS = SPI_NSS_SOFT;
    //SPI频率
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATE;
    //高位先行
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    //TI(Texas Instruments)模式关闭
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    //CRC校验关闭
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    //CRC多项式
    hspi2.Init.CRCPolynomial = 10;

    //模式0,CPOL = 0 CPHA = 0
    if(HAL_SPI_Init(&hspi2) != HAL_OK) {
        Error_Handler();
    }
    __HAL_SPI_ENABLE(&hspi2);
}


//重写HAL的MSP(MCU Support Package)Init
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi){
    GPIO_InitTypeDef gpio = {0};

    if(hspi->Instance == SPI1){
        __GPIOA_CLK_ENABLE();
        __SPI1_CLK_ENABLE();
        
        gpio.Mode = GPIO_MODE_AF_PP;
        gpio.Pin = SPI1_SCK_PIN | SPI1_MOSI_PIN;
        //gpio.Pull = GPIO_PULLUP;
        gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpio.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOA,&gpio);

        // gpio.Mode = GPIO_MODE_AF_PP;
        // gpio.Pin = SPI1_NSS_PIN;
        // gpio.Pull = GPIO_PULLUP;
        // HAL_GPIO_Init(GPIOA,&gpio);

        HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(SPI1_IRQn);
        
        sBSP_SPI1M_SetCS(1);
    }
    else if(hspi->Instance == SPI2){
        __GPIOB_CLK_ENABLE();
        __GPIOC_CLK_ENABLE();
        __SPI2_CLK_ENABLE();
        
        /**SPI2 GPIO Configuration
        PC1     ------> SPI2_MOSI
        PC2     ------> SPI2_MISO
        PB13     ------> SPI2_SCK
        */
        GPIO_InitTypeDef GPIO_InitStruct = {0};

        GPIO_InitStruct.Pin = AHRS_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_SPI2;
        HAL_GPIO_Init(AHRS_MOSI_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = AHRS_MISO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(AHRS_MISO_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = AHRS_SCK_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(AHRS_SCK_GPIO_Port, &GPIO_InitStruct);
    }
}

void sBSP_SPI1M_SetCS(uint8_t cs_en){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,(GPIO_PinState)cs_en);
}

void sBSP_SPI1M_SendBytes(uint8_t *pData,uint16_t Size){
    HAL_SPI_Transmit(&hspi1,pData,Size,1000);
}

void sBSP_SPI1M_ReciBytes(uint8_t *pData,uint16_t Size){
    HAL_SPI_Receive(&hspi1,pData,Size,1000);
}

void sBSP_SPI1M_SendByte(uint8_t byte){
    HAL_SPI_Transmit(&hspi1,&byte,1,100);
}

uint8_t sBSP_SPI1M_ReciByte(){
    uint8_t send_byte = 0;
    HAL_SPI_Receive (&hspi1,&send_byte,1,100);
    return send_byte;
}

void sBSP_SPI1M_SetEN(uint8_t en){
    if(en){
        __HAL_SPI_ENABLE(&hspi1);
    }else{
        __HAL_SPI_DISABLE(&hspi1);
    }
}


void sBSP_SPI2M_SendBytes(uint8_t *pData,uint16_t Size){
    HAL_SPI_Transmit(&hspi2,pData,Size,1000);
}

void sBSP_SPI2M_ReciBytes(uint8_t *pData,uint16_t Size){
    HAL_SPI_Receive(&hspi2,pData,Size,1000);
}

void sBSP_SPI2M_SendByte(uint8_t byte){
    HAL_SPI_Transmit(&hspi2,&byte,1,100);
}

uint8_t sBSP_SPI2M_ReciByte(){
    uint8_t send_byte = 0;
    HAL_SPI_Receive (&hspi2,&send_byte,1,100);
    return send_byte;
}


