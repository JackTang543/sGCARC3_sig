#include "sDRV_LPS22HBTR.h"


#include "sLib_Reg.h"

//240818~0820   第一版 支持SPI

#define USE_SPI_IF

//PB1 -> LPS22_CS
#define SPI_CS_CLK_EN    __GPIOB_CLK_ENABLE
#define SPI_CS_PORT      GPIOB
#define SPI_CS_PIN       GPIO_PIN_1

#ifdef USE_SPI_IF
    #include "sBSP_SPI.h"

#endif


float g_lps22_pa;
float g_lps22_temp;

//寄存器地址和掩码

//todo 气压获取的中断模式配置
#define ADDR_INTERRUPTCFG                         (0x0B)
//使能AUTORIFP功能 0:正常模式(默认) 1:AutoRifP使能
#define MSK_INTERRUPTCFG_AUTORIFP                 (0b10000000)
//复位AutoRifP功能 0:正常模式(默认) 1:复位AutoRifP功能
#define MSK_INTERRUPTCFG_RESETARP                 (0b01000000)
//使能Autozero功能 0:正常模式(默认) 1:使能Autozero
#define MSK_INTERRUPTCFG_AUTOZERO                 (0b00100000)
//复位Autozero功能 0:正常模式(默认) 1:复位Autozero
#define MSK_INTERRUPTCFG_RESETAZ                  (0b00010000)
//使能中断 0:禁用中断(默认) 1:中断使能
#define MSK_INTERRUPTCFG_DIFFEN                   (0b00001000)
//锁存中断请求到INT_SOURCE(0x25)寄存器 0:中断请求不锁存(默认) 1:中断请求被锁存
#define MSK_INTERRUPTCFG_LIR                      (0b00000100)
//使能气压低事件中断的产生 0:禁用(默认) 1:使能,当气压低于预设的阈值产生中断
#define MSK_INTERRUPTCFG_PLE                      (0b00000010)
//同上,不过是气压高事件
#define MSK_INTERRUPTCFG_PHE                      (0b00000001)

//todo THS_P_L 用户定义的气压时间中断阈值 低8位
#define ADDR_THSPL                                (0x0C)

//todo THS_P_H 用户定义的气压时间中断阈值 高8位
#define ADDR_THSPH                                (0x0D)

//todo WHO_AM_I 读出始终为0xB1
#define ADDR_WHOAMI                               (0x0F)

//todo CTRL_REG1
#define ADDR_CTRLREG1                             (0x10)
//输出数据速率 默认:0
#define MSK_CTRLREG1_ODR                          (0b01110000)
//使能气压数据LPF(连续模式启用时) 0:禁用(默认) 1:使能
//0: 带宽ODR/2 1:如果LPFP_CFG位=0,带宽ODR/9 如果=1,带宽ODR/20
#define MSK_CTRLREG1_ENLPFP                       (0b00001000)
//见上 默认0
#define MSK_CTRLREG1_LPFPCFG                      (0b00000100)
//阻塞数据更新 0:连续模式(默认) 1:输出寄存器只有MSB和LSB被读取之后才会更新
#define MSK_CTRLREG1_BDU                          (0b00000010)
//SPI模式设置 0:4线SPI(默认) 1:3线SPI  
#define MSK_CTRLREG1_SIM                          (0b00000001)

//todo CTRL_REG2
#define ADDR_CTRLREG2                             (0x11)
//复位内存内容 0:正常(默认) 1:复位
#define MSK_CTRLREG2_BOOT                         (0b10000000)
//FIFO使能
#define MSK_CTRLREG2_FIFOEN                       (0b01000000)
//是否在FIFO水位线的时候停止
#define MSK_CTRLREG2_STOPONFTH                    (0b00100000)
//多字节访问的时候寄存器地址是否自增 默认:1
#define MSK_CTRLREG2_IFADDINC                     (0b00010000)
//是否禁用I2C
#define MSK_CTRLREG2_I2CDIS                       (0b00001000)
//软件复位 写1软件复位
#define MSK_CTRLREG2_SWRESET                      (0b00000100)
//单次模式使能
#define MSK_CTRLREG2_ONESHOT                      (0b00000001)

//todo CTRL_REG3
#define ADDR_CTRLREG3                             (0x12)
//中断激活极性 0:激活为HI(默认) 1:激活为LO
#define MSK_CTRLREG3_INTHL                        (0b10000000)
//中断引脚模式 0:推挽(默认) 1:开漏
#define MSK_CTRLREG3_PPOD                         (0b01000000)
//FIFO满标志使能
#define MSK_CTRLREG3_FFSS5                        (0b00100000)
//FIFO水位线标志使能
#define MSK_CTRLREG3_FFTH                         (0b00010000)
//FIFO超限中断标志使能
#define MSK_CTRLREG3_FOVR                         (0b00001000)
//数据准备好信号使能
#define MSK_CTRLREG3_DRDY                         (0b00000100)
//数据信号在INT引脚控制 00:数据信号(按优先级排序:DRDY F_FTH F_OVR F_FFS5)
//01:气压高 10:气压低 11:气压低或高
#define MSK_CTRLREG3_INTS                         (0b00000011)

//todo FIFO_CTRL
#define ADDR_FIFOCTRL                             (0x14)
//FIFO模式选择
#define MSK_FIFOCTRL_FMODE                        (0b11100000)
//FIFO水位线选择
#define MSK_FIFOCTRL_WTM                          (0b00011111)

//todo REF_P_XL 参考气压次低位
#define ADDR_REFPXL                               (0x15)

//todo REF_P_L 参考气压低位
#define ADDR_REFPL                                (0x16)

//todo REF_P_H 参考气压高位
#define ADDR_REFPH                                (0x17)

//todo RPDS_L 参考气压低位
#define ADDR_RPDSL                                (0x18)

//todo RPDS_H 参考气压高位
#define ADDR_RPDSH                                (0x19)

//todo RES_CONF 低功耗模式配置
#define ADDR_RESCONF                              (0x1A)
//低电流模式,只能在非启动模式下修改 0:低噪声模式(默认) 1:低电流模式
#define MSK_RESCONF_LCEN                          (0b00000001)

//todo INT_SOURCE 中断源
#define ADDR_INTSOURCE                            (0x25)
//如果为1表示reboot阶段正在进行
#define MSK_INTSOURCE_BOOTSTATUS                  (0b10000000)
//中断使能
#define MSK_INTSOURCE_IA                          (0b00000100)
//差分气压低
#define MSK_INTSOURCE_PL                          (0b00000010)
//差分气压高
#define MSK_INTSOURCE_PH                          (0b00000001)

//todo FIFO_STATUS
#define ADDR_FIFOSTATUS                           (0x26)
//FIFO水位线状态 0:FIFO低于阈值 1:等于或高于阈值
#define MSK_FIFOSTATUS_FTHFIFO                    (0b10000000)
//FIFO超限状态 0:FIFO没满 1:FIFO满了并且最后一个样本被覆盖
#define MSK_FIFOSTATUS_OVR                        (0b01000000)
//FIFO保存数据的多少 0:FIFO为空 ~0b100000:FIFO已满并且有32个未读样本
#define MSK_FIFOSTATUS_FSS                        (0b00111111)

//todo STATUS
#define ADDR_STATUS                               (0x27)
//温度数据超限 0:无 1:一个新的温度数据已经覆写了上一个
#define MSK_STATUS_TOR                            (0b00100000)
//气压数据超限 同上
#define MSK_STATUS_POR                            (0b00010000)
//温度数据可用 0:新的温度数据还不可用 1:新的温度数据产生
#define MSK_STATUS_TDA                            (0b00000010)
//气压数据可用 同上
#define MSK_STATUS_PDA                            (0b00000001)

//todo PRESS_OUT_XL
#define ADDR_PRESSOUTXL                           (0x28)

//todo PRESS_OUT_L
#define ADDR_PRESSOUTL                            (0x29)

//todo PRESS_OUT_H
#define ADDR_PRESSOUTH                            (0x2A)

//todo TEMP_OUT_L
#define ADDR_TEMPOUTL                             (0x2B)

//todo TEMP_OUT_H
#define ADDR_TEMPOUTH                             (0x2C)

//todo LPFP_RES
//低通滤波器复位寄存器 如果 LPFP 处于活动状态，为了避免过渡阶段
//可以在生成压力测量值之前通过读取此寄存器来复位滤波器
#define ADDR_LPFPRES                              (0x2D)



//接口
//初始化CS引脚
static void portCSInit(){
    SPI_CS_CLK_EN();
    GPIO_InitTypeDef gpio = {0};
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio.Pin   = SPI_CS_PIN;
    HAL_GPIO_Init(SPI_CS_PORT,&gpio);
}

static inline void portSetCS(bool lv){
    HAL_GPIO_WritePin(SPI_CS_PORT,SPI_CS_PIN,(GPIO_PinState)lv);
}

static uint8_t read_reg(uint8_t addr){
    //读时序有两个字节,第一个字节最高位为1表示读,为0表示写时序,其他位为寄存器地址
    portSetCS(0);
    //首先发送第一个字节
    sBSP_SPI2M_SendByte(0x80 | addr);
    //然后读取第二个字节
    uint8_t tmp = sBSP_SPI2M_ReciByte();
    portSetCS(1);
    return tmp;
}
static void write_reg(uint8_t addr,uint8_t data){
    portSetCS(0);
    //首先发送第一个字节
    sBSP_SPI2M_SendByte(0x00 | addr);
    //然后发送第二个字节
    sBSP_SPI2M_SendByte(data);
    portSetCS(1);
}
//一次读取多个寄存器,用于加速读取原始数据速度
static void read_regs(uint8_t addr,uint8_t* pData,uint8_t len){
    portSetCS(0);
    //首先发送第一个字节
    sBSP_SPI2M_SendByte(0x80 | addr);
    //然后读取
    sBSP_SPI2M_ReciBytes(pData,len);
    portSetCS(1);
    
}
// 对寄存器进行修改,形参:寄存器地址,修改的部分,修改的数据
static void reg_modify(uint8_t reg_addr, uint8_t reg_msk, uint8_t data){
    //读改写
    //首先读出寄存器
    uint8_t tmpreg = read_reg(reg_addr);
	//sHMI_Debug_Printf("读出位置0x%X的内容:0x%X\n",reg_addr,tmpreg);
    //进行修改
    sLib_ModifyReg(&tmpreg, reg_msk, data);
	//sHMI_Debug_Printf("位掩码:0x%X,数据:0x%X,修改后的内容:0x%X\n\n",reg_msk,data,tmpreg);
    //写回
    write_reg(reg_addr, tmpreg);
}



int sDRV_LPS22_Init(){
    #ifdef USE_SPI_IF
        portCSInit();
        portSetCS(1);
    #endif
    //检查通信是否正常
    uint8_t who_am_i = read_reg(ADDR_WHOAMI);
    if(who_am_i != 0xB1){
        sHMI_Debug_Printf("[ERR ]LPS22HBTR初始化失败\n");
        return -1;
    }
    //复位器件
    reg_modify(ADDR_CTRLREG2,MSK_CTRLREG2_SWRESET,1);
    HAL_Delay(20);
    //如果SPI通信则关闭I2C功能
    #ifdef USE_SPI_IF
        reg_modify(ADDR_CTRLREG2,MSK_CTRLREG2_I2CDIS,1);
    #endif
    //设置输出速率
    reg_modify(ADDR_CTRLREG1,MSK_CTRLREG1_ODR,SDRV_LPS22_ODR_75HZ);
    //防止数据混合
    reg_modify(ADDR_CTRLREG1,MSK_CTRLREG1_BDU,1);


    sHMI_Debug_Printf("[OK  ]LPS22HBTR初始化成功\n");

    return 0;
}

void sDRV_LPS22_GetData(){
    uint8_t buf[5];

    //read_regs(ADDR_PRESSOUTXL,buf,5);

    buf[0] = read_reg(ADDR_PRESSOUTXL);
    buf[1] = read_reg(ADDR_PRESSOUTL);
    buf[2] = read_reg(ADDR_PRESSOUTH);
    buf[3] = read_reg(ADDR_TEMPOUTL);
    buf[4] = read_reg(ADDR_TEMPOUTH);


    //气压数据
    int32_t raw_press = (buf[2] << 16) | (buf[1] << 8) | buf[0];
    g_lps22_pa = (float)raw_press / 4096.0f;

    //温度数据
    int16_t raw_temp = (buf[4] << 8) | buf[3];
    g_lps22_temp = (float)raw_temp / 100.0f;

}

float sDRV_LPS22_GetTemp(){

    return g_lps22_temp;
}

float sDRV_LPS22_GetPress(){

    return g_lps22_pa;
}



