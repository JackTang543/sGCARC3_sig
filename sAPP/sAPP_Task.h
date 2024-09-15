#ifndef __SAPP_TASK_H__
#define __SAPP_TASK_H__
#ifdef __cplusplus
extern "C"{
#endif

#include "main.h"




void sAPP_Task_LVGLHdlr(void* pvPara);

void sAPP_Task_10ms(void* pvPara);
void sAPP_Task_50ms(void* pvPara);
void sAPP_Task_100ms(void* pvPara);
void sAPP_Task_500ms(void* pvPara);
void sAPP_Task_1000ms(void* pvPara);

void sAPP_Task_CLIReci(void* pvPara);


#ifdef __cplusplus
}
#endif
#endif
