/*
 * iicx.cpp
 *
 *  Created on: May 10, 2026
 *      Author: bitstream
 */
#include "main.h"
#include "application.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
	if (hi2c->Instance == I2C1)
	{
		if (TransferDirection == I2C_DIRECTION_TRANSMIT) {
			// 主机要写数据（可能是：只写寄存器地址，或 写地址+数据）
			app.IICReceive();
		}
		else if (TransferDirection == I2C_DIRECTION_RECEIVE) {
			// 主机要读数据：此时 reg_addr 必须已经在之前保存好了
			app.IICResponse();
		}
	}
}

// 侦听完成回调函数
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (hi2c->Instance == I2C1)
	{
		app.IICDone();
	}
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        // 数据已存于 rx_buffer，长度为 rx_len
        // 处理接收数据...
    	app.IICRx();
    }
}

// 当主机执行读操作时，自动进入发送完成回调
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    // 此时主机正在读，从机刚发送完数据
    if (hi2c->Instance == I2C1)
    {
    	app.IICTx();
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    // 处理错误，如重新初始化或重启接收/发送
    if (hi2c->Instance == I2C1)
    {
    	app.IICError();
    }
}

#ifdef __cplusplus
}
#endif
