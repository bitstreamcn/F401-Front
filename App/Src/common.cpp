/*
 * common.cpp
 *
 *  Created on: Apr 15, 2026
 *      Author: bitstream
 */
#include "main.h"
#include "common.h"


#ifdef __cplusplus
extern "C" {
#endif


uint32_t micros(void) {
	static uint32_t last_cycle = 0;
	static uint64_t cycle_overflow = 0;
	uint32_t cycle = DWT->CYCCNT;

	// 检测溢出
	if (cycle < last_cycle) {
		cycle_overflow += (1ULL << 32);
	}
	last_cycle = cycle;

	uint64_t total_cycles = cycle_overflow + cycle;
	return total_cycles / (SystemCoreClock / 1000000ULL);
    //return DWT->CYCCNT / (SystemCoreClock / 1000000);
}
uint32_t millis()
{
    return HAL_GetTick();
}
// 可在任务或中断中使用的微秒延时函数
void delay_us(uint32_t us)
{
    // 必须先开启 DWT 调试单元（脱机运行也能用）
    if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0)
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 开启调试跟踪
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // 开启CYCCNT计数器
    }

    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = (HAL_RCC_GetHCLKFreq() / 1000000) * us;
    while ((DWT->CYCCNT - start) < cycles)
    {
    }
}


uint32_t TimerGetCurrentTime(void) {
    return HAL_GetTick();
}


uint32_t TimerGetElapsedTime( uint32_t eventInTime )
{
	uint32_t elapsedTime = 0;

    // Needed at boot, cannot compute with 0 or elapsed time will be equal to current time
    if( eventInTime == 0 )
    {
        return 0;
    }

    elapsedTime = TimerGetCurrentTime();

    if( elapsedTime < eventInTime )
    { // roll over of the counter
        return( elapsedTime + ( 0xFFFFFFFF - eventInTime ) );
    }
    else
    {
        return( elapsedTime - eventInTime );
    }
}

#ifdef __cplusplus
}
#endif
