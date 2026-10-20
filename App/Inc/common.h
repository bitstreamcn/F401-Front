/*
 * common.h
 *
 *  Created on: Apr 15, 2026
 *      Author: bitstream
 */

#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint32_t TimerGetCurrentTime(void);
uint32_t TimerGetElapsedTime( uint32_t eventInTime );
uint32_t micros(void); //微秒 1000 000 = 1 秒
uint32_t millis(); //毫秒
void delay_us(uint32_t us);



// 辅助宏：将两个符号拼接成一个新的符号
#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

// 用法：STABLE_FOR(value, limit, timeout_ms)
// 返回值：当 fabsf(value) < limit 连续达到 timeout_ms 毫秒时返回 true
// 内部使用 __LINE__ 生成独立的静态变量，因此可以在同一函数中多次调用而不互相干扰
#define STABLE_FOR(value, limit, timeout_ms)                                     \
({                                                                               \
    static uint32_t CONCAT(_start_tick_, __LINE__) = 0;                         \
    static bool CONCAT(_started_, __LINE__) = false;                            \
    bool _ret = false;                                                          \
    if (fabsf(value) < (limit)) {                                               \
        if (!CONCAT(_started_, __LINE__)) {                                     \
            CONCAT(_start_tick_, __LINE__) = HAL_GetTick();                     \
            CONCAT(_started_, __LINE__) = true;                                 \
        } else if ((HAL_GetTick() - CONCAT(_start_tick_, __LINE__)) >= (timeout_ms)) { \
            _ret = true;                                                        \
        }                                                                       \
    } else {                                                                    \
        CONCAT(_started_, __LINE__) = false;                                    \
    }                                                                           \
    _ret;                                                                       \
})

// 功能：统计函数调用频率，1秒刷新一次，带自定义标识名，自动Log输出
// 使用：CALC_FUNC_FREQ("你的标识名");
#define CALC_FUNC_FREQ(name)                                                     \
do {                                                                             \
    static uint32_t last_tick_##__LINE__ = 0;                                     \
    static uint32_t call_count_##__LINE__ = 0;                                   \
    const uint32_t now_tick = HAL_GetTick();                                     \
    const uint32_t diff_ms = now_tick - last_tick_##__LINE__;                    \
                                                                                 \
    call_count_##__LINE__++;                                                     \
    static float freq_hz = 0;                                                    \
	static uint32_t log_ms = 0;													 \
    if (diff_ms >= 1000) {                                                       \
        freq_hz = (float)call_count_##__LINE__ * 1000.0f / diff_ms;              \
                                                                                 \
        call_count_##__LINE__ = 0;                                               \
        last_tick_##__LINE__ = now_tick;                                         \
    }                                                                            \
	if ((now_tick - log_ms) >= 2000)                                             \
	{                                                                            \
		log_ms = now_tick;                                                       \
        Log::I("[Freq] %s : %.1f Hz", name, freq_hz);                            \
	}                                                                            \
} while(0)



#define PERIODIC_LOGI(ms, format, ...)                                                     \
do {                                                                             \
    static uint32_t last_tick_##__LINE__ = 0;                                     \
    const uint32_t now_tick = HAL_GetTick();                                     \
    const uint32_t diff_ms = now_tick - last_tick_##__LINE__;                    \
	if (diff_ms >= ms)                                             \
	{                                                                            \
		last_tick_##__LINE__ = now_tick;                                                       \
        LogI(format, ##__VA_ARGS__);                            	 \
	}                                                                            \
} while(0)


#define PERIODIC_LOGE(ms, format, ...)                                                     \
do {                                                                             \
    static uint32_t last_tick_##__LINE__ = 0;                                     \
    const uint32_t now_tick = HAL_GetTick();                                     \
    const uint32_t diff_ms = now_tick - last_tick_##__LINE__;                    \
	if (diff_ms >= ms)                                             \
	{                                                                            \
		last_tick_##__LINE__ = now_tick;                                                       \
        LogE(format, ##__VA_ARGS__);                            	 \
	}                                                                            \
} while(0)



#ifdef __cplusplus
}
#endif

#endif /* INC_COMMON_H_ */
