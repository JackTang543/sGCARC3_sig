#include "sAPP_Func.h"



void lvgl_flush_cb(lv_display_t* disp,const lv_area_t* area,uint8_t* px_map);
static void lvgl_log_cb(lv_log_level_t level, const char* buf);




void sAPP_Func_DisplayInit(){
    /*初始化屏幕硬件*/
    sBSP_SPI1M_Init(SPI_BAUDRATEPRESCALER_2);   //45MBits/s
    sDRV_ST7789V_Init();

    /*初始化LVGL*/
    lv_init();
    //设置滴答定时器回调
    lv_tick_set_cb(HAL_GetTick);
    //注册日志回调
    lv_log_register_print_cb(lvgl_log_cb);
    //创建屏幕
    g_screen = lv_display_create(SDRV_ST7789V_LCDH, SDRV_ST7789V_LCDW);
    //屏幕旋转
    lv_display_set_rotation(g_screen,LV_DISPLAY_ROTATION_0);
    //注册屏幕刷新回调
    lv_display_set_flush_cb(g_screen,lvgl_flush_cb);
    //双缓冲区
    lv_display_set_buffers(g_screen,(void*)g_lvgl_buf1,(void*)g_lvgl_buf2,sizeof(g_lvgl_buf1),LV_DISPLAY_RENDER_MODE_PARTIAL);

}


void sAPP_Func_IMUInit(){
    /*SPI2 <-> IMU_SPI*/
    sBSP_SPI2M_Init(SPI_BAUDRATEPRESCALER_4);   //11.25MBits/s

    /*把IMU的三个CS都上拉*/
    __GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef gpio = {0};
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio.Pin   = GPIO_PIN_3 | GPIO_PIN_0 | GPIO_PIN_4;
    HAL_GPIO_Init(GPIOC,&gpio);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,1);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,1);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,1);
    
    /*初始化传感器*/
    sDRV_ICM_Init();    //陀螺仪+加速度计
    sDRV_LIS3_Init();   //磁强计
}

void sAPP_Func_CaliIMUStaticBias(){
    #define POINT_COUNT 10000
    float acc_x_accu = 0;
	float acc_y_accu = 0;
	float acc_z_accu = 0;
	float gyro_x_accu = 0;
	float gyro_y_accu = 0;
	float gyro_z_accu = 0;
	for(uint32_t i = 0; i < POINT_COUNT; i++){
        sDRV_ICM_GetData();
		acc_x_accu  += g_icm.acc_x;
		acc_y_accu  += g_icm.acc_y;
		acc_z_accu  += g_icm.acc_z;
		gyro_x_accu += g_icm.gyro_x;
		gyro_y_accu += g_icm.gyro_y;
		gyro_z_accu += g_icm.gyro_z;
		//HAL_Delay(1);
	}
	bias.acc_x  = acc_x_accu  / POINT_COUNT;
	bias.acc_y  = acc_y_accu  / POINT_COUNT;
	bias.acc_z  = acc_z_accu  / POINT_COUNT - 9.81398f;	//重力加速度
	bias.gyro_x = gyro_x_accu / POINT_COUNT;
	bias.gyro_y = gyro_y_accu / POINT_COUNT;
	bias.gyro_z = gyro_z_accu / POINT_COUNT;
}

lv_anim_t ips_bl_anim;
int32_t prev_bl,setting_bl;

static void ips_bl_ctrl_cb(void * user_data, int32_t brightness){
    //亮度扩大了100倍
    
    sBSP_TIM10_CH1SetDuty(sLib_GammaCorrect(brightness / 100));
    //sHMI_Debug_Printf("%d\n",brightness);
    //lv_label_set_text_fmt(label1,"%d",brightness);
}

void sAPP_Func_ScreenBLInit(){
    sBSP_TIM10_Init();
    sBSP_TIM10_SetPWMFreq(10000);   //10KHz
    sBSP_TIM10_CH1SetDuty(0);       //关闭背光
    sBSP_TIM10_CH1SetEN(1);
    /*初始化亮度变化的非线性动画*/
    lv_anim_init(&ips_bl_anim);
    lv_anim_set_var(&ips_bl_anim, NULL);                      // 传递给动画回调的用户数据
    lv_anim_set_exec_cb(&ips_bl_anim, ips_bl_ctrl_cb);        // 设置执行函数
    lv_anim_set_time(&ips_bl_anim, 1000);                     // 动画时间，单位为毫秒
    //lv_anim_set_values(&ips_bl_anim, 0, 10000);                 // 从0亮度变化到100亮度
    lv_anim_set_path_cb(&ips_bl_anim, lv_anim_path_ease_in_out); // 设置缓动方式
}




void sAPP_Func_ScreenBLSet(float percent){
    prev_bl = setting_bl;
    setting_bl = percent * 100;
    //开始非线性动画
    lv_anim_set_values(&ips_bl_anim, prev_bl, setting_bl);
    lv_anim_start(&ips_bl_anim);
}





__attribute__((section(".ramfunc"))) void lvgl_flush_cb(lv_display_t* disp,const lv_area_t* area,uint8_t* px_map){
    //uint16_t * buf16 = (uint16_t *)px_map; /*Let's say it's a 16 bit (RGB565) display*/

    //设置显示窗口
    //sDRV_ST7789V_SetWindow(area->x1 + 40, area->y1 + 53, area->x2 + 40, area->y2 + 53);
    sDRV_ST7789V_SetWindow(area->x1 + 40, area->y1 + 53 - 1, area->x2 + 40, area->y2 + 53 - 1);
    // 向ST7789V发送显示数据
    sDRV_ST7789V_SendColorArea(px_map, lv_area_get_width(area) * lv_area_get_height(area));
    // 通知LVGL刷新完成
    lv_display_flush_ready(disp);
}

static void lvgl_log_cb(lv_log_level_t level, const char* buf){
    sHMI_Debug_Printf("LEVEL:%d :%s",level, buf);
}

