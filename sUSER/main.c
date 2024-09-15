#include "main.h"

/**
 * LIS3通信失败
 */



//屏幕渲染中的互斥锁
SemaphoreHandle_t g_screen_rending;

uint8_t g_lvgl_buf1[LVGL_DRAW_BUF_SIZE / 4];
uint8_t g_lvgl_buf2[LVGL_DRAW_BUF_SIZE / 4];
lv_display_t* g_screen;


lv_obj_t* label;
lv_obj_t* label1;

sLIB_6AXIS_INPUT_t input = {0};
sLIB_ATTITUDE_RESULT_t result = {0};
sLIB_6AXIS_INPUT_t bias = {0};







int main(){
    /*初始化HAL和RCC*/
    sBSP_SYS_Init();

    sHMI_Debug_Init(115200);
    sHMI_Debug_Printf("Hello F446 sGenCARCtrller!\n");
    uint32_t sys_freq = HAL_RCC_GetSysClockFreq();
    sHMI_Debug_Printf("System clock freq: %d MHz\n", sys_freq / 1000000);


    //屏幕渲染资源互斥锁
    g_screen_rending = xSemaphoreCreateMutex();


    /*初始化按键*/
    sAPP_Btns_Init();
    /*初始化板载屏幕和LVGL*/
    sAPP_Func_DisplayInit();
    /*初始化屏幕背光控制*/
    sAPP_Func_ScreenBLInit();  //todo 未完成 背光非线性动画
    /*初始化UI的控件*/
    sAPP_UI_InitWidghts();
    /*初始化IMU*/
    sAPP_Func_IMUInit();




    /*校准IMU静态零偏*/
    HAL_Delay(1000);
    sAPP_Func_CaliIMUStaticBias();
    

    
    

    

    
    //1K
    xTaskCreate(sAPP_Task_LVGLHdlr          , "LVGLHdlr"        ,  (8192) / sizeof(StackType_t), NULL, 4, NULL);
    

    //10ms 100Hz任务
    xTaskCreate(sAPP_Task_10ms              , "10ms"        ,  128, NULL, 1, NULL);
    //50ms 20Hz任务
    xTaskCreate(sAPP_Task_50ms              , "50ms"        ,  512, NULL, 2, NULL);
    //100ms 10Hz任务
    xTaskCreate(sAPP_Task_100ms             , "100ms"       ,  128, NULL, 1, NULL);
    //500ms 2Hz任务
    xTaskCreate(sAPP_Task_500ms             , "500ms"       ,  128, NULL, 1, NULL);
    //1000ms 1Hz任务
    xTaskCreate(sAPP_Task_1000ms            , "1000ms"      ,  128, NULL, 1, NULL);

    sHMI_Debug_Printf("FreeRTOS启动任务调度\n");
    vTaskStartScheduler();


    while(1){
        update_screen();
        HAL_Delay(50);

    }
}

void update_screen(){
        sDRV_LIS3_GetData();
        sDRV_ICM_GetData();
        
        input.acc_x = g_icm.acc_x - bias.acc_x;
        input.acc_y = g_icm.acc_y - bias.acc_y;
        input.acc_z = g_icm.acc_z - bias.acc_z;
        input.gyro_x = g_icm.gyro_x - bias.gyro_x;
        input.gyro_y = g_icm.gyro_y - bias.gyro_y;
        input.gyro_z = g_icm.gyro_z - bias.gyro_z;
        float heading = atan2(g_lis3.mag_y, g_lis3.mag_x) * RAD2DEG;
        heading = 180 + heading;

        sLib_6AxisCompFilter(&input,&result);
        

        lv_label_set_text_fmt(label1,"Pitch: %6.2f\nRoll:%6.2f\nYaw:%6.2f\nMagYaw:%6.2f"\
            ,result.pitch,result.roll,result.yaw,heading);
}



void Error_Handler(){
    __disable_irq();
    while (1){
        sHMI_Debug_Printf("WARNING警告:错误! 禁用IRQ,死循环...\n");
        HAL_Delay(500);
    }
}

void assert_failed(uint8_t* file, uint32_t line){
    __disable_irq();
    while (1){
        sHMI_Debug_Printf("WARNING警告:断言! 文件名:%s,行:%u 禁用IRQ,死循环...\n",file,line);
        HAL_Delay(500);
    }
}

void vApplicationMallocFailedHook(){
    sHMI_Debug_Printf("WARNING警告:内存申请失败! 禁用IRQ\n");
}

void vApplicationIdleHook(){
    
}   

void vApplicationTickHook(){
}


void vApplicationStackOverflowHook(TaskHandle_t xTask,char* pcTaskName){
    sHMI_Debug_Printf("WARNING警告:触发栈金丝雀机制! 任务句柄:0x%X,任务名:%s\n",xTask,pcTaskName);
}




/**
 * LIS3 c编译器o3优化导致数据读取失败
 */



