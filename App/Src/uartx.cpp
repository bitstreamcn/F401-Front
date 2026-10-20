/*
 * uart.cpp
 *
 *  Created on: Apr 2, 2026
 *      Author: bitstream
 */
#include "main.h"
#include "usart.h"
#include "uartx.h"
#include <stdlib.h>
#include <cstring>

// 发送缓存 放在 D2 RAM（H7必须）
#define TX_BUFFER_SIZE    512
uint8_t UART1_Tx_Buffer[2][TX_BUFFER_SIZE] __attribute__((section(".RAM")));
uint8_t UART1_Tx_Buffer_Index = 0;
uint16_t UART1_Tx_Buffer_Data_Index = 0;

// 发送状态标志
volatile uint8_t UART1_Tx_Busy = 0;  // 1=发送中 0=空闲


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        // 发送完成，释放标志
        UART1_Tx_Busy = 0;
    }
}


// 功能：UART1 DMA 发送数据（非阻塞）
// 参数：数据指针、长度
// 返回：0成功 1忙
uint8_t UART1_DMA_Send(uint8_t *pData, uint16_t len)
{
    // 长度保护
    if(len > TX_BUFFER_SIZE)
    {
        return 1;
    }
    //缓冲区爆炸，发送数据太多，串口速度跟不上
    if ((UART1_Tx_Buffer_Data_Index + len + 5 + 4/*crc对齐*/) > TX_BUFFER_SIZE)
    {
    	UART1_Tx_Buffer_Data_Index = 0;
    	return 1;
    }
    //开始标志 AA55
    //UART1_Tx_Buffer[UART1_Tx_Buffer_Index][UART1_Tx_Buffer_Data_Index] = 0xaa;
    //UART1_Tx_Buffer[UART1_Tx_Buffer_Index][UART1_Tx_Buffer_Data_Index + 1] = 0x55;
    //UART1_Tx_Buffer[UART1_Tx_Buffer_Index][UART1_Tx_Buffer_Data_Index + 2] = len & 0xff;
    //UART1_Tx_Buffer[UART1_Tx_Buffer_Index][UART1_Tx_Buffer_Data_Index + 3] = (len >> 8) & 0xff;
    //UART1_Tx_Buffer_Data_Index += 4;

    uint8_t * pDataBuff = &(UART1_Tx_Buffer[UART1_Tx_Buffer_Index][UART1_Tx_Buffer_Data_Index]);

    // 复制数据到发送缓存
    memcpy(pDataBuff, pData, len);
    UART1_Tx_Buffer_Data_Index += len;

    //为了crc计算对齐
    //pDataBuff[len + 0] = 0;
    //pDataBuff[len + 1] = 0;
    //pDataBuff[len + 2] = 0;
    //pDataBuff[len + 3] = 0;

    //uint8_t crc = 0;
    //uint32_t crc_result = HAL_CRC_Calculate(&hcrc, (uint32_t*)pDataBuff, (len / 4) + 1);
    //crc = (crc_result & 0xff) + ((crc_result >> 8) & 0xff) + ((crc_result >> 16) & 0xff) + ((crc_result >> 24) & 0xff);

    //UART1_Tx_Buffer[UART1_Tx_Buffer_Index][UART1_Tx_Buffer_Data_Index] = crc;
    //crc校验码
    //UART1_Tx_Buffer_Data_Index++;

    if (UART1_Tx_Busy == 0)
    {
		// 标记发送中
		UART1_Tx_Busy = 1;

		// 启动 DMA 发送
		if(HAL_UART_Transmit_DMA(&huart1, UART1_Tx_Buffer[UART1_Tx_Buffer_Index], UART1_Tx_Buffer_Data_Index) != HAL_OK)
		{
			UART1_Tx_Busy = 0;
			return 1;
		}

		UART1_Tx_Buffer_Data_Index = 0;
		UART1_Tx_Buffer_Index = (UART1_Tx_Buffer_Index + 1) % 2;
    }

    return 0; // 启动成功
}


