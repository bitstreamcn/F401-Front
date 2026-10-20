/*
 * log.cpp
 *
 *  Created on: Apr 12, 2026
 *      Author: bitstream
 */
#include "main.h"
#include "uartx.h"
#include "log.h"
#include "led.h"
#include "ring_buffer.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "app_azure_rtos.h"

#define TX_STR(s) const_cast<CHAR*>(s)
// 最大日志长度
#define LOG_MAX_LEN 128

extern TX_QUEUE log_queue;

uint8_t log_buffer_data[1024 * 4];
RingBuffer log_buffer;

bool ispause = false;

void Log::I(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);       // 绑定可变参数

    // 【关键】调用 vwrite 版本，把可变参数转发过去
    write("[I] ", format, ap);

    va_end(ap);
}

void Log::W(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);       // 绑定可变参数

    // 【关键】调用 vwrite 版本，把可变参数转发过去
    write("[W] ", format, ap);

    va_end(ap);
}

void Log::E(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);       // 绑定可变参数

    // 【关键】调用 vwrite 版本，把可变参数转发过去
    write("[E] ", format, ap);

    va_end(ap);
}

TX_MUTEX log_mutex;

void Log::Init()
{
	if (tx_mutex_create(&log_mutex, TX_STR("Log Mutex"), TX_INHERIT) != TX_SUCCESS) {
	        // 创建失败处理
		Error_Handler();
	}
	rb_init(&log_buffer, log_buffer_data, sizeof(log_buffer_data));
}

void Log::Pause()
{
	ispause = true;
}

void Log::Resume()
{
	ispause = false;
}

void Log::write(const char * tag, const char *format, va_list ap)
{
	//下载log，暂停输出
	if (ispause)
	{
		return;
	}
    // 获取互斥锁，如果已被占用则无限期等待
    //UINT status = tx_mutex_get(&log_mutex, TX_WAIT_FOREVER);
	//if (status == TX_SUCCESS)
	{
	    char buffer[LOG_MAX_LEN + 1];

	    int taglen = strlen(tag);

	    char * p = buffer;
	    snprintf(buffer, LOG_MAX_LEN, "%s", tag);
	    p += taglen;

	    vsnprintf(p, LOG_MAX_LEN - taglen, format, ap);
	    uint32_t len = strlen(buffer);
	    if (buffer[len - 1] != '\n')
	    {
	    	buffer[len] = '\n';
	    	buffer[len + 1] = '\0';
	    	len += 1;
	    }

		if (0 != UART1_DMA_Send((uint8_t*)buffer, len))
		{
			//串口出问题，亮灯显示错误
			led.Error();
		}

		// 释放互斥锁，供其他线程使用
		//tx_mutex_put(&log_mutex);
	}
}

#ifdef __cplusplus
extern "C" {
#endif

void LogInit(void)
{
	Log::Init();
}

void LogI(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    Log::write("[I] ", format, ap);
    va_end(ap);
}
void LogW(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    //Log::W(format, ap); // 调用 C++ 类
    Log::write("[W] ", format, ap);
    va_end(ap);
}
void LogE(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    //Log::E(format, ap); // 调用 C++ 类
    Log::write("[E] ", format, ap);
    va_end(ap);
}

void LogPause()
{
	Log::Pause();
}
void LogResume()
{
	Log::Resume();
}

#ifdef __cplusplus
}
#endif

