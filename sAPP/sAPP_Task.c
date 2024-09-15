#include "sAPP_Task.h"




void sAPP_Task_LVGLHdlr(void* pvPara){
    for(;;){
        //屏幕渲染加锁
        if(xSemaphoreTake(g_screen_rending, portMAX_DELAY) == pdTRUE){
            lv_task_handler();
            //渲染完释放
            xSemaphoreGive(g_screen_rending);
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void sAPP_Task_10ms(void* pvPara){
    for(;;){
        sGBD_Handler();

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void sAPP_Task_50ms(void* pvPara){
    float i = 0;
    for(;;){
        //获取锁
        if(xSemaphoreTake(g_screen_rending, portMAX_DELAY) == pdTRUE){
            //调用UI处理函数
            update_screen();
            //释放
            xSemaphoreGive(g_screen_rending);
        }

        


        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void sAPP_Task_100ms(void* pvPara){
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    for(;;){
        //sHMI_Debug_Printf("100ms task\n");

        vTaskDelay(100 / portTICK_PERIOD_MS);
        //xTaskDelayUntil(&xLastWakeTime,100 / portTICK_PERIOD_MS);
    }
}


void sAPP_Task_500ms(void* pvPara){
    for(;;){
        //sHMI_Debug_Printf("500ms task\n");

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void sAPP_Task_1000ms(void* pvPara){
    for(;;){
        sAPP_Func_ScreenBLSet(100);


        vTaskDelay(2000 / portTICK_PERIOD_MS);

        sAPP_Func_ScreenBLSet(0);

        vTaskDelay(2000 / portTICK_PERIOD_MS);

    }
}


// void sAPP_Task_CLIReci(void* pvPara){
//     char* p_str_buf;
//     BaseType_t more_data_flag;

//     for(;;){
//         //如果串口收到数据了
//         if(xSemaphoreTake(g_sem_debug_uart_recied,portMAX_DELAY) == pdTRUE){
//             p_str_buf = (char*)g_UART1_RxBuf;
//             //确保字符串以0结尾
//             p_str_buf[UART1_RX_BUF_SIZE - 1] = '\0';

//             // 处理接收到的每一条命令
//             do {
//                 // 调用 FreeRTOS+CLI 的命令处理器
//                 more_data_flag = FreeRTOS_CLIProcessCommand(
//                     (const char*)p_str_buf,         // 命令字符串
//                     cOutputBuffer,           // 命令输出缓冲区
//                     sizeof(cOutputBuffer)    // 缓冲区大小
//                 );

//                 // 将命令处理的输出发送回UART
//                 sHMI_Debug_Printf("%s", cOutputBuffer);

//             } while (more_data_flag != pdFALSE);  // 如果还有更多输出，继续处理

//             // 清空接收缓冲区，准备下一次接收
//             memset(g_UART1_RxBuf, 0, UART1_RX_BUF_SIZE);

//         }
//     }
// }

