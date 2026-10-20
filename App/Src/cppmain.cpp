/*
 * cppmain.cpp
 *
 *  Created on: Apr 2, 2026
 *      Author: bitstream
 */
#include "main.h"
#include "cppmain.h"
#include "application.h"
#include "app_azure_rtos.h"
#include "stm32f4xx.h"
#include "log.h"
#include "led.h"


#define TX_STR(s) const_cast<CHAR*>(s)

#define CPP_APP_MEM_POOL_SIZE (1024 * 12)
__ALIGN_BEGIN static UCHAR cpp_byte_pool_buffer[CPP_APP_MEM_POOL_SIZE] __ALIGN_END;
static TX_BYTE_POOL cpp_app_byte_pool;


static TX_THREAD thread_tof;
TX_SEMAPHORE tof_semaphore;
static void tof_thread_entry(ULONG input);


static TX_THREAD thread_main;
static void main_thread_entry(ULONG input);



#ifdef __cplusplus
extern "C" {
#endif


void rtos_stack_error_handler(TX_THREAD *thread_ptr) {
    // 在这里可以打印出错线程的指针、名称等信息
    // 也可以在这里触发一个断点，或保存错误现场
    Log::E("Stack error in thread: %s\n", thread_ptr->tx_thread_name);
    // 进一步处理...
}

void cppmain(void)
{
	if (tx_byte_pool_create(&cpp_app_byte_pool, TX_STR("Cpp App memory pool"), cpp_byte_pool_buffer, CPP_APP_MEM_POOL_SIZE) != TX_SUCCESS)
	{
		Error_Handler();
	}

	UINT status = tx_thread_stack_error_notify(rtos_stack_error_handler);

#define THREAD_MAIN_STACK_SIZE (1024 * 4)

    UCHAR *pointer_main;
	if (tx_byte_allocate(&cpp_app_byte_pool, (VOID **)&pointer_main,
			THREAD_MAIN_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
	{
		Error_Handler();
	}

	if (tx_thread_create(&thread_main, TX_STR("main Thread"),
			main_thread_entry, 0,
			pointer_main, THREAD_MAIN_STACK_SIZE,
					 10, 10, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
	{
		Error_Handler();
	}

}


#ifdef __cplusplus
}
#endif


static void main_thread_entry(ULONG input)
{
	//等待2.4G模块初始化完成
	//HAL_Delay(1000);
	//tx_thread_sleep(1000);

	Log::Init();

	//硬件驱动和组件初始化
	app.Init();

#if 1
	//创建tof线程
	if (tx_semaphore_create(&tof_semaphore, TX_STR("tof_semaphore"), 0) != TX_SUCCESS)
	{
		Error_Handler();
	}

#define THREAD_TOF_STACK_SIZE (1024 * 2)

    UCHAR *pointer_tof;
	if (tx_byte_allocate(&cpp_app_byte_pool, (VOID **)&pointer_tof,
			THREAD_TOF_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
	{
		Error_Handler();
	}
	//默认创建的线程是优先级是10，tof读取速度特别慢，优先级一定要低于主线程
	if (tx_thread_create(&thread_tof, TX_STR("tof Thread"),
			tof_thread_entry, 0,
			pointer_tof, THREAD_TOF_STACK_SIZE,
					 20, 20, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
	{
		Error_Handler();
	}
#endif

	//主循环
	app.Loop();
}


static void tof_thread_entry(ULONG input)
{
	UINT status;
	while (1)
	{
		// 无限期等待信号量
		//status = tx_semaphore_get(&tof_semaphore, TX_WAIT_FOREVER);
		//if (status == TX_SUCCESS)
		{
			// 成功获取信号量，执行你的任务处理
			app.TofUpdate();
		}
		tx_thread_sleep(10);
	}
}

//解决HAL_GetTick()返回0问题




