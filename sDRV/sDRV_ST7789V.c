#include "sDRV_ST7789V.h"

#include "sLib_Reg.h"

#define USE_SPI_IF
//#define USE_SPI_HARD_CS

//PA4 -> ST7789V CS 硬件NSS 需要上拉
#define SPI_CS_CLK_EN    __GPIOA_CLK_ENABLE
#define SPI_CS_PORT      GPIOA
#define SPI_CS_PIN       GPIO_PIN_4

//PB4 -> ST7789V DC
#define DC_CLK_EN        __GPIOB_CLK_ENABLE
#define DC_PORT          GPIOB
#define DC_PIN           GPIO_PIN_4

#ifdef USE_SPI_IF
    #include "sBSP_SPI.h"
#endif


//接口

static inline void portDelayMs(uint32_t ms){
    HAL_Delay(ms);
}

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

//初始化DC引脚
static void portDCInit(){
    DC_CLK_EN();
    GPIO_InitTypeDef gpio = {0};
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio.Pin   = DC_PIN;
    HAL_GPIO_Init(DC_PORT,&gpio);
}

static inline void portSetCS(bool lv){
    HAL_GPIO_WritePin(SPI_CS_PORT,SPI_CS_PIN,(GPIO_PinState)lv);
}

static inline void portSetDC(bool lv){
    HAL_GPIO_WritePin(DC_PORT,DC_PIN,(GPIO_PinState)lv);
}


//发送单字节命令
static void send_comm(uint8_t comm){
    #ifndef USE_SPI_HARD_CS
        portSetCS(0);
    #endif

    //设置DC为0(命令字)
    portSetDC(0);
    sBSP_SPI1M_SendByte(comm);

    #ifndef USE_SPI_HARD_CS
        portSetCS(1);
    #endif
}

//发送多字节数据
static void send_data(uint8_t* pData,uint8_t len){
    #ifndef USE_SPI_HARD_CS
        portSetCS(0);
    #endif

    //设置DC为1
    portSetDC(1);
    sBSP_SPI1M_SendBytes(pData,len);

    #ifndef USE_SPI_HARD_CS
        portSetCS(1);
    #endif
}

static uint8_t read_data(){
    #ifndef USE_SPI_HARD_CS
        portSetCS(0);
    #endif

    uint8_t tmp = 0;

    tmp = sBSP_SPI1M_ReciByte();

    #ifndef USE_SPI_HARD_CS
        portSetCS(1);
    #endif

    return tmp;
}

/*ST7789V命令表1*/

//todo 命令NOP
#define COMM_NOP                              (0x00)

//todo 软件复位
#define COMM_SWRESET                          (0x01)

//todo 读取显示ID 紧接着要读取4个字节
//1:空数据 2:LCD模块的制造商ID 3:LCD模组/驱动版本ID 4:LED模组/驱动ID
#define COMM_RDDID                            (0x04)

//todo 进入睡眠模式
#define COMM_SLPIN                            (0x10)

//todo 退出睡眠模式
#define COMM_SLPOUT                           (0x11)

//todo 部分显示模式启动
#define COMM_PTLON                            (0x12)

//todo 正常模式启动
#define COMM_NORON                            (0x13)

//todo 关闭反显
#define COMM_INVOFF                           (0x20)

//todo 开启反显
#define COMM_INVON                            (0x21)

//todo 伽马矫正设置 需要带一个参数
#define COMM_GAMSET                           (0x26)

//todo 显示关闭
#define COMM_DISPOFF                          (0x28)

//todo 显示开启
#define COMM_DISPON                           (0x29)

//todo 列地址设置 带四个参数
//1:XS MSB 2:XS LSB 3:XE MSB 4:XE LSB
#define COMM_CASET                            (0x2A)

//todo 行地址设置 带四个参数
//1:YS MSB 2:YS LSB 3:YE MSB 4:YE LSB
#define COMM_RASET                            (0x2B)

//todo 内存写 后可接无限个参数
//1: RGB565模式下 颜色MSB 2: 颜色LSB ...
#define COMM_RAMWR                            (0x2C)

//todo 内存读 后可接无限个参数
#define COMM_RAMRD                            (0x2E)

//todo 部分区域设置 带四个参数
//1: PSL MSB 2: PSL LSB 3: PEL MSB 4: PEL LSB
#define COMM_PTLAR                            (0x30)

//todo 垂直滚动定义 略
#define COMM_VSCRDEF                          (0x33)

//todo 撕裂行功能关闭 (就类似于垂直同步)
#define COMM_TEOFF                            (0x34)

//todo 撕裂行功能开启 后接一个参数 略
#define COMM_TEON                             (0x35)

//todo 内存数据访问控制 后接一个参数
//D7: MY 页地址顺序 0: 从顶到底扫描(默认) 1: 从底到顶扫描
//D6: MX 列地址顺序 0: 从左到右扫描(默认) 1: 从右到左扫描
//D5: MV 页/列顺序  0: 正常默认(默认)     1: 反转页面和列地址顺序
//D4: ML 行地址顺序 0: 显示刷新从顶到底(默认) 1:显示刷新从底到顶
//D3: RGB RGB/BGR 顺序 0:RGB(默认) 1:BGR
//D2: MH 显示数据锁存顺序 0:显示刷新从左到右(默认) 1:显示刷新从右到左
#define COMM_MADCTL                           (0x36)

//todo RAM的垂直滚动起始地址 后接两个参数 略
#define COMM_VSCRSADD                         (0x37)

//todo 空闲模式关
#define COMM_IDMOFF                           (0x38)

//todo 空闲模式开 打开空闲模式:每个像素只有RGB的最高位合成颜色 一共8种颜色
#define COMM_IDMON                            (0x39)

//todo 接口像素格式 后接一个参数
//D6 D5 D4 :RGB接口颜色格式 101:65K 110:262K
//D2 D1 D0 :控制接口颜色格式 010:12bit/px 101:16bit 110:18bit 111:16M截断(模拟24bit色深)
#define COMM_COLMOD                           (0x3A)

//todo 继续写内存 后可接无限参数
#define COMM_WRMEMC                           (0x3C)

//todo 继续读内存 后可读无限参数
#define COMM_RDMEMC                           (0x3E)

//todo 设置撕裂扫描线 后接两个参数
#define COMM_TESCAN                           (0x44)

//todo 获取扫描线
#define COMM_GSCAN                            (0x45)

//todo 写显示亮度 后接一个参数(亮度0~255)
#define COMM_WRDISBV                          (0x51)

//todo 读显示亮度 略
#define COMM_RDDISBV                          (0x52)

//todo 写控制屏幕 后接一个参数
//D5: BCTRL 亮度控制块开关 0:关闭(亮度寄存器为00h,DBV[7:0]) 1:开(根据其他参数)
//D3: DD 显示调光(仅适用于手动亮度设置) 0:显示调光打开 1:显示调光关闭
//D2: BL 背光控制开关 0:关闭(完全关闭背光电路) 1:开
#define COMM_WRCTRLD                          (0x53)

//todo 读控制屏幕 略
#define COMM_RDCTRLD                          (0x54)

//todo 写内容适应性亮度控制和颜色增强 带一个参数
//D7: CECTRL 0:关闭颜色增强 1:开启颜色增强
//D4 D3: 00:低级增强 01:中级增强 11:高级增强
//D1 D0: 00:关闭 01:用户界面模式 10:静止图像模式 11:动态图像模式
#define COMM_WRCACE                           (0x55)

//todo 写内容适应性亮度控制和颜色增强 略
#define COMM_RDCABC                           (0x56)

//todo 写CABC最小亮度 后接一个参数:亮度(0~255)
#define COMM_WRCABCMB                         (0x5E)

//todo 读CABC最小亮度 略
#define COMM_RDCABCMB                         (0x5F)

//todo 读自动亮度控制自检结果 略
#define COMM_RDABCSDR                         (0x68)

//todo 读LCD模组厂商ID
#define COMM_RDID1                            (0xDA)

//todo 读LCD模组/驱动IC版本
#define COMM_RDID2                            (0xDB)

//todo 读LCD模组/驱动ID
#define COMM_RDID3                            (0xDC)

/*系统功能命令表2*/

//todo RAM控制
//参数1: D4: RM RAM访问选择 0:RAM通过MCU接口访问 1:RAM通过RGB接口访问
//D1 D0: DM 显示操作选择 00:MCU接口 01:RGB接口 10:VSYNC接口
//参数2: D5 D4: EPF 65k 和 4k 的数据转换为帧数据 数据手册256页
//D3: ENDIAN 0:正常(MSB先) 1:小端(LSB先)
//D2: RIM 指定RGB接口总线宽度 0:18bit宽 1:6bit宽
//D1 D0: MDT 像素数据传输模式 Please refer to section 8.8 Data Color Coding
#define COMM_RAMCTRL                          (0xB0)

//todo RGB接口控制 略
#define COMM_RGBCTRL                          (0xB1)

//todo 前后廊设置 略
#define COMM_PORCTRL                          (0xB2)

//todo 帧速率配置(部分模式/空闲模式) 略
#define COMM_FRCTRL1                          (0xB3)

//todo 栅极控制 带一个参数
//D6 D5 D4: VGHS D2 D1 D0: VGLS
#define COMM_GCTRL                            (0xB7)
#define COMM_DGMEN                            (0xBA)
#define COMM_VCOMS                            (0xBB)
#define COMM_LCMCTRL                          (0xC0)
#define COMM_IDSET                            (0xC1)
#define COMM_VDVVRHEN                         (0xC2)
#define COMM_VRHS                             (0xC3)
#define COMM_VDVSET                           (0xC4)
#define COMM_VCMOFSET                         (0xC5)
#define COMM_FRCTR2                           (0xC6)
#define COMM_CABCCTRL                         (0xC7)
#define COMM_REGSEL1                          (0xC8)
#define COMM_REGSEL2                          (0xCA)
#define COMM_PWMFRSEL                         (0xCC)
#define COMM_PWCTRL1                          (0xD0)
#define COMM_VAPVANEN                         (0xD2)
#define COMM_CMD2EN                           (0xDF)
#define COMM_PVGAMCTRL                        (0xE0)
#define COMM_NVGAMCTRL                        (0xE1)
#define COMM_DGMLUTR                          (0xE2)
#define COMM_DGMLUTB                          (0xE3)
#define COMM_GATECTRL                         (0xE4)
#define COMM_PWCTRL2                          (0xE8)
#define COMM_EQCTRL                           (0xE9)
#define COMM_PROMCTRL                         (0xEC)
#define COMM_PROMEN                           (0xFA)
#define COMM_NVMSET                           (0xFC)
#define COMM_PROMACT                          (0xFE)
#define COMM_END                              (0xFF)

//CASET设置显示宽度
const uint8_t init_seq_caset[4] = {
    0, SDRV_ST7789V_OFFSETW,
    (SDRV_ST7789V_LCDW - 1) >> 8, (SDRV_ST7789V_LCDW - 1) & 0xFF,
};

//RASET设置显示高度
const uint8_t init_seq_raset[4] = {
    0, SDRV_ST7789V_OFFSETH,
    (SDRV_ST7789V_LCDH - 1) >> 8, (SDRV_ST7789V_LCDH - 1) & 0xFF,
};

//const uint8_t madctl[1] = {0b01100000};
const uint8_t madctl[1] = {0b10100000};

//初始化命令序列
const sDRV_ST7789V_CommSeq_t init_seq[] = {
    {COMM_SWRESET  ,150, 0,            NULL},        //复位系统
    {COMM_SLPOUT   ,150, 0,            NULL},        //退出睡眠模式
    {COMM_CMD2EN   ,100, 0,            NULL},        //开启命令表2的执行权限
    {COMM_MADCTL   ,  0, 1,(uint8_t*)madctl},        //内存访问控制
    {COMM_COLMOD   ,  0, 1,(uint8_t*)"\x55"},        //65K 16bit颜色
    {COMM_INVON    ,  0, 0,            NULL},        //启动反向显示
    {COMM_CASET    ,  0, 4,(uint8_t*)init_seq_caset},//设置显示宽度
    {COMM_RASET    ,  0, 4,(uint8_t*)init_seq_raset},//设置显示高度
    {COMM_GCTRL    ,  0, 1,(uint8_t*)"\x35"},        //设置VGH=12.54V,VGL=-9.6V
    {COMM_VCOMS    ,  0, 1,(uint8_t*)"\x1f"},        //设置VCOM=1.475V
    {COMM_VDVVRHEN ,  0, 1,(uint8_t*)"\x01"},        // Enable VDV/VRH control
    {COMM_LCMCTRL  ,  0, 1,(uint8_t*)"\x2c"},// LCM control
    {COMM_VRHS     ,  0, 1,(uint8_t*)"\x12"},// VAP(GVDD) = 4.45+(vcom+vcom offset+vdv)
    {COMM_VDVSET   ,  0, 1,(uint8_t*)"\x20"},// VDV = 0V
    {COMM_PWCTRL1  ,  0, 2,(uint8_t*)"\xa4\xa1"},// AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V
    //{COMM_FRCTR2   ,  0, 1,(uint8_t*)"\x0f"},//  60 fps
    {COMM_FRCTR2   ,  0, 1,(uint8_t*)"\x00"},//  119 fps
    {COMM_GAMSET   ,  0, 1,(uint8_t*)"\x01"},// Gama 2.2
    // Gama curve
    {COMM_PVGAMCTRL,  0,14,(uint8_t*)"\xd0\x08\x11\x08\x0c\x15\x39\x33\x50\x36\x13\x14\x29\x2d"},
    {COMM_NVGAMCTRL,  0,14,(uint8_t*)"\xd0\x08\x10\x08\x06\x06\x39\x44\x51\x0b\x16\x14\x2f\x31"},
    
    {COMM_END      ,  0, 0,NULL}                   // End of commands
};

const sDRV_ST7789V_CommSeq_t init_seq2[] = {
    {COMM_RGBCTRL  ,  0, 3,(uint8_t *)"\x02\x08\x3c"},  // HSYNC = 0x3C, VSYNC = 0x80
    {COMM_RAMCTRL  ,  0, 2,(uint8_t*)"\x00\xc2"},       // RAMCTRL Select MCU interface
    {COMM_DISPON   ,100, 0,NULL},                      // Display on
    {COMM_SLPOUT   ,100, 0,NULL},                      // Sleep out
    
    //{COMM_RAMWR, 50, 0, NULL},                        // Begin GRAM write
    {COMM_END      ,  0, 0,NULL},                       // End of commands
};


//运行命令序列中的一条命令
static void run_comm_seq1(const sDRV_ST7789V_CommSeq_t* comm) {
    //发送命令
    send_comm(comm->comm);
    //发送命令的数据
    send_data(comm->pData, comm->data_size);
    //延时
    if(comm->wait_ms > 0){
        portDelayMs(comm->wait_ms);
    }
}

//运行命令序列
static void run_comm_seq(const sDRV_ST7789V_CommSeq_t* pCommSeq) {
    while (pCommSeq->comm != COMM_END) {
        run_comm_seq1(pCommSeq);
        pCommSeq++;
    }
}

void sDRV_ST7789V_SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd) {
    uint8_t caset[4];
    uint8_t raset[4];

    caset[0] = (uint8_t)(xStart >> 8);
    caset[1] = (uint8_t)(xStart & 0xFF);
    caset[2] = (uint8_t)(xEnd >> 8);
    caset[3] = (uint8_t)(xEnd & 0xFF);

    raset[0] = (uint8_t)(yStart >> 8);
    raset[1] = (uint8_t)(yStart & 0xFF);
    raset[2] = (uint8_t)(yEnd >> 8);
    raset[3] = (uint8_t)(yEnd & 0xFF);


    sDRV_ST7789V_CommSeq_t sequence[] = {
        {COMM_CASET, 0, 4, caset},
        {COMM_RASET, 0, 4, raset},
        {COMM_END  , 0, 0, NULL},
    };

    run_comm_seq(sequence);

    //* Start RAM WRITE command
    const sDRV_ST7789V_CommSeq_t ram_wr = {COMM_RAMWR, 0, 0, NULL};
    run_comm_seq1(&ram_wr);
}

uint8_t buf[200 + 1];

void st7789_FillArea(uint16_t color, uint16_t startX, uint16_t startY, uint16_t width, uint16_t height) {
    uint8_t tmp[2] = {0};
    tmp[0] = (color >> 8) & 0xFF;
    tmp[1] = (uint8_t)color;

    //* Set window based on (x,y)
    sDRV_ST7789V_SetWindow(startX, startY, startX + width - 1, startY + height - 1);

    //一次刷新一个屏幕
    portSetDC(1);
    portSetCS(0);
    for (int i = 0; i < width * height; i++) {
        HAL_SPI_Transmit(&hspi1, tmp, 2, 500);
    }
    portSetCS(1);

}


//! 有问题
void st7789_Clear(uint16_t color) {
    st7789_FillArea(color, 52, 40, SDRV_ST7789V_LCDW, SDRV_ST7789V_LCDH);
}

// void sDRV_ST7789V_SendColorArea(uint8_t *color_p, uint32_t size) {
//     // 设置DC为数据模式
//     portSetDC(1);
//     // 选中SPI从设备
//     portSetCS(0);

//     // 每次最多发送 65536 字节的数据
//     const uint32_t max_chunk_size = 1024;
//     uint32_t bytes_to_send = size * 1; // 计算总共需要发送的字节数
//     uint8_t *current_ptr = color_p; // 指向当前发送的数据块

//     while (bytes_to_send > 0) {
//         // 计算本次需要发送的字节数
//         uint32_t chunk_size = (bytes_to_send > max_chunk_size) ? max_chunk_size : bytes_to_send;

//         // 发送当前数据块
//         HAL_SPI_Transmit(&hspi1, current_ptr, chunk_size, HAL_MAX_DELAY);

//         // 更新指针和剩余字节数
//         current_ptr += chunk_size;
//         bytes_to_send -= chunk_size;
//     }

//     // 释放SPI从设备
//     portSetCS(1);
// }


void sDRV_ST7789V_SendColorArea(uint8_t *color_p, uint32_t size) {
    uint16_t * buf16 = (uint16_t *)color_p;
    // 设置DC为数据模式
    portSetDC(1);
    // 选中SPI从设备
    portSetCS(0);
    // 将颜色数据通过SPI发送到显示屏
    for(uint32_t i = 0; i < size; i++) {
        uint8_t tmp[2] = {0};
        tmp[0] = (*buf16 >> 8) & 0xFF;
        tmp[1] = (uint8_t)*buf16;
        HAL_SPI_Transmit(&hspi1, tmp, 2, 500);
        buf16++;
    }
    // 释放SPI从设备
    portSetCS(1);
}


void sDRV_ST7789V_SetPixel(uint16_t x, uint16_t y, uint16_t color){
    uint8_t tmp[2];

    // 将16位RGB565颜色值分成两个8位数据
    tmp[0] = (color >> 8) & 0xFF;
    tmp[1] = (uint8_t)color;

    // 设置窗口为一个像素点的大小
    sDRV_ST7789V_SetWindow(x + 40, y + 53,\
        x + 40, y + 53);

    // 开始写入RAM
    portSetDC(1);  // 设置DC为数据模式
    portSetCS(0);  // 选中芯片

    // 发送单个像素的数据
    HAL_SPI_Transmit(&hspi1, tmp, 2, 500);

    portSetCS(1);  // 释放芯片
}


int sDRV_ST7789V_Init(){
    #ifdef USE_SPI_IF
        #ifndef USE_SPI_HARD_CS
            portCSInit();
            portSetCS(1);
        #endif
        portDCInit();
        portSetDC(1);
    #endif
    

    run_comm_seq(init_seq);

    HAL_Delay(10);
    //st7789_Clear(ST7789_BLACK);

    run_comm_seq(init_seq2);

    // // //HAL_Delay(500);
    // st7789_Clear(ST7789_RED);
    // //HAL_Delay(500);
    // st7789_Clear(ST7789_GREEN);
    // //HAL_Delay(500);
    // st7789_Clear(ST7789_BLUE);
    // //HAL_Delay(500);
    // st7789_Clear(ST7789_WHITE);


    //ST7789V_ReadID();

    //sHMI_Debug_Printf("id0:%x id1:%x id2:%x id3:%x\n", id[0], id[1], id[2], id[3]);




    sHMI_Debug_Printf("[OK  ]ST7789V初始化成功\n");

    return 0;
}


