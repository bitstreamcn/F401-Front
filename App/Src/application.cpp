/*
 * application.cpp
 *
 *  Created on: Apr 2, 2026
 *      Author: bitstream
 */
#include "main.h"
#include "application.h"
#include <cstdio>
#include <math.h>
#include "led.h"
#include "log.h"
#include "common.h"
#include "uartx.h"
#include "i2c.h"
#include "cppmain.h"
#include "custom_bus.h"

#define delay(ms) tx_thread_sleep(ms)

#ifdef __cplusplus
extern "C" {
#endif

#include "../Device/TOF/app_tof.h"

#ifdef __cplusplus
}
#endif

extern I2C_HandleTypeDef hi2c3;

Application app;

float roll = 0.0f;
float pitch = 0.0f;
float yaw = 0.0f;
float gyro_x = 0.0f;
float gyro_y = 0.0f;

#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0f)


typedef enum{
	LOOP_IDLE,
	LOOP_TOF,
	LOOP_PMW,
	LOOP_IIC,
	LOOP_END
}LOOP_STATE;


struct __attribute__((packed)){
	volatile int8_t IICReceive;
	volatile int8_t IICResponse_Addr;
	volatile int8_t IICResponse_Data;
	volatile int8_t IICRx_Addr;
	volatile int8_t IICRx_Data;
	volatile int8_t IICTx;
	volatile int8_t IICError;
	volatile int8_t IICDone;
	volatile uint8_t errcount;
	volatile int8_t step;
}iic_state;


Application::Application()
{

}

Application::~Application()
{

}

//根据倾斜度计算TOF测得的实际垂直高度
float compute_vertical_height(float tof_distance_mm, float pitch_deg, float roll_deg)
{
    float pitch_rad = DEG_TO_RAD(pitch_deg);
    float roll_rad  = DEG_TO_RAD(roll_deg);

    float vertical_height_mm = tof_distance_mm * cosf(pitch_rad) * cosf(roll_rad);
    return vertical_height_mm;
}


// 全局变量：保存起始偏航角（在初始化时记录）
static float start_yaw_rad = 0.0f;

// 初始化函数，在系统启动时调用一次，记录初始偏航角
void flow_init(float yaw_start_deg) {
    start_yaw_rad = DEG_TO_RAD(yaw_start_deg);
}

static float last_roll = 0.0f;
static float last_pitch = 0.0f;
static bool is_first_flow = true;
/**
 * @brief 将机体光流位移转换为起始坐标系下的地面位移
 * @param dx_sensor   机体X轴位移（m），右为正
 * @param dy_sensor   机体Y轴位移（m），前为正
 * @param roll_deg    当前横滚角（度），右倾为正
 * @param pitch_deg   当前俯仰角（度），抬头为正
 * @param yaw_deg     当前偏航角（度），与起始偏航角同定义（例如北偏东为正）
 * @param height 当前真实垂直高度
 * @param dx_ground   输出：起始坐标系下X轴位移增量（m）
 * @param dy_ground   输出：起始坐标系下Y轴位移增量（m）
 */
void flow_displacement_to_ground(float dx_sensor, float dy_sensor,
                                 float roll_deg, float pitch_deg, float yaw_deg,
                                 float height_m,
                                 float *dx_ground, float *dy_ground)
{
    // 如果是第一帧，没法算差值，直接记录并返回
    if (is_first_flow) {
        last_roll = roll_deg;
        last_pitch = pitch_deg;
        is_first_flow = false;
        return;
    }

    // 1. 计算自上一次光流触发以来的角度变化量（转为弧度）
    float delta_pitch_rad = DEG_TO_RAD(pitch_deg - last_pitch);
    float delta_roll_rad  = DEG_TO_RAD(roll_deg - last_roll);

    // 更新历史角度
    last_pitch = pitch_deg;
    last_roll  = roll_deg;

    // 高度保护
    if (height_m < 0.1f) height_m = 0.1f;

    // 2. 计算由“机体倾斜”引起的虚假平移 (总虚假位移 = 角度变化 * 高度)
    // 注意正负号：抬头(+)通常会导致光流Y轴负方向移动，需要根据你的传感器实际方向调整正负号
    float dx_rot = delta_pitch_rad * height_m;
    float dy_rot = delta_roll_rad * height_m;

    // 3. 从光流原始读数中减去这段时间积累的虚假平移
    float dx_real = dx_sensor - dx_rot;
    float dy_real = dy_sensor - dy_rot;

    // 4. 投影到水平地面 (消除倾斜导致的透视形变)
    float cos_roll = cosf(DEG_TO_RAD(roll_deg));
    float cos_pitch = cosf(DEG_TO_RAD(pitch_deg));
    if (cos_roll < 0.5f) cos_roll = 0.5f;
    if (cos_pitch < 0.5f) cos_pitch = 0.5f;

    float dx_h = dx_real * cos_pitch;
    float dy_h = dy_real * cos_roll;

    // 5. 偏航旋转
    float delta_yaw = DEG_TO_RAD(yaw_deg) - start_yaw_rad;
    float cos_yaw = cosf(delta_yaw);
    float sin_yaw = sinf(delta_yaw);

    *dx_ground = dx_h * cos_yaw - dy_h * sin_yaw;
    *dy_ground = dx_h * sin_yaw + dy_h * cos_yaw;
}

// 传入你的 I2C 句柄，例如 &hi2c1
void I2C_Scan(I2C_HandleTypeDef *hi2c)
{
    uint8_t found_count = 0;

    // I2C 7位地址范围是 0x01 到 0x7F (0x00是通用呼叫地址，通常跳过)
    for (uint8_t addr = 1; addr < 128; addr++)
    {
        // 记录当前Tick，用于超时判断
        uint32_t tick_start = HAL_GetTick();

        // 作用1：HAL_I2C_IsDeviceReady 会发送地址并等待ACK
        // 作用2：STM32 HAL库要求传入8位地址，所以要把7位地址左移1位 (addr << 1)
        HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(addr << 1), 1, 10);

        if (status == HAL_OK)
        {
            // 收到应答，说明地址存在设备
            found_count++;
        }

        // 稍作延时，防止总线过于拥挤
        HAL_Delay(5);
    }
}

void Application::Init()
{

	HAL_StatusTypeDef status = HAL_I2C_EnableListen_IT(&hi2c1);
	if (status != HAL_OK)
	{
		Log::I("HAL_I2C_EnableListen_IT error:%x", status);
		iic_listen_error = true;
	}

	//BSP_I2C3_Init();
	//I2C_Scan(&hi2c3);

	//tof初始化
    Log::I("TOF init");
    do
    {
		if (!MX_TOF_Init())
		{
			Log::E("TOF init fail");
			Log::E("\n");
			led.Error();
			delay(10);
			continue;
		}
		break;
    }while(1);
    led.Clear();
    Log::I("TOF init done");


	Log::I("Reset reason: 0x%lX\n", RCC->CSR);
	HAL_Delay(100);
	__HAL_RCC_CLEAR_RESET_FLAGS();

	Log::I("application init done\r\n");

	mIsInit = true;
}

#define I2C_SCL_PORT  GPIOB
#define I2C_SCL_PIN   GPIO_PIN_6
#define I2C_SDA_PORT  GPIOB
#define I2C_SDA_PIN   GPIO_PIN_7

// 手动解锁 I2C 总线
void I2C_Unstick_Bus(I2C_HandleTypeDef *hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. 暂时关闭 I2C 外设
    __HAL_I2C_DISABLE(hi2c);

    // 2. 将 SCL 和 SDA 配置为开漏输出模式
    GPIO_InitStruct.Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // 开漏输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;        // 依赖外部上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C_SCL_PORT, &GPIO_InitStruct);

    // 3. 确保 SDA 和 SCL 初始为高电平
    HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_SET);

    // 4. 产生最多 9 个 SCL 时钟脉冲
    for (uint8_t i = 0; i < 9; i++)
    {
        // 拉低 SCL
        HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
        HAL_Delay(1); // 简单延时，给总线反应时间

        // 拉高 SCL
        HAL_GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
        HAL_Delay(1);
    }

    // 5. 产生一个 STOP 信号 (SCL 为高时，SDA 由低变高)
    HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_RESET); // SDA 拉低
    HAL_Delay(1);
    HAL_GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_PIN_SET);  // SDA 拉高 (产生 STOP)
    HAL_Delay(1);

    // 6. 将引脚交还给 I2C 外设 (恢复为复用功能)
    // 这一步实际上会在 MX_I2C1_Init() 中完成，但保险起见这里可以再初始化一次
    // HAL_I2C_DeInit(hi2c);
    // MX_I2C1_Init();

    // 7. 重新使能 I2C 外设
    __HAL_I2C_ENABLE(hi2c);
}


void Application::Loop()
{
	LOOP_STATE state = LOOP_IDLE;

	while(1)
	{
		uint32_t tick = HAL_GetTick();
		(void)tick;

			switch(state)
			{
			case LOOP_IDLE:
				break;
/*
			case LOOP_TOF:
				TofUpdate();
				break;
*/
			case LOOP_IIC:
				IICDataUpdate();
				break;

			default:
				break;
			}

			//收到姿态数据
			if (is_ahr_ready)
			{
				is_ahr_ready = false;
				int16_t val = ahr_data[0] + ahr_data[1] + ahr_data[2] + ahr_data[3] + ahr_data[4];
				if (val == ahr_data[5])
				{
					roll = ahr_data[0];
					pitch = ahr_data[1];
					yaw = ahr_data[2];
					gyro_x = ahr_data[3];
					gyro_y = ahr_data[4];
					if (start_yaw_rad == 0)
					{
						flow_init(yaw);
					}
				}
			}

			//检查处理是否超过5毫秒，如果超过，会影响200hz控制频率和imu中断处理
			uint32_t remain = HAL_GetTick() - tick;
			if (remain >= 5)
			{
				//Log::W("process timeout(%d): %d ms", (int)state, remain);
			}
			state = (LOOP_STATE)(((int)state) + 1);
			if (state >= LOOP_END)
			{
				state = LOOP_TOF;
			}

			//检查iic开始通信到结束时间，如果超时，说明iic在操作中间没有返回
			if (iic_active && (tick - iic_active_tick) > 10)
			{
				iic_error = true;
			}

			//检查i2c是否存在错误，或者超过2秒没有收到请求
			if (iic_error)
			{
				iic_state.errcount += 1;
				iic_active = false;
				iic_active_tick = tick;
				iic_error = false;
				//Log::I("IICError");
				uint32_t error = HAL_I2C_GetError(&hi2c1);
				if (error == HAL_I2C_ERROR_BERR)
				{
					//Log::I("HAL_I2C_GetError error:%x", error);
				}
			    // 通过串口或 LED 指示 error 值
			    // 常见错误：
			    // HAL_I2C_ERROR_BERR  (总线错误)
			    // HAL_I2C_ERROR_ARLO  (仲裁丢失)
			    // HAL_I2C_ERROR_OVR   (过载/欠载)
			    // HAL_I2C_ERROR_AF    (应答失败)
			    HAL_I2C_DeInit(&hi2c1);
			    I2C_Unstick_Bus(&hi2c1);
			    HAL_I2C_Init(&hi2c1);

				HAL_StatusTypeDef status = HAL_I2C_EnableListen_IT(&hi2c1);
				if (status != HAL_OK)
				{
					//Log::I("IICError HAL_I2C_EnableListen_IT error:%x", status);
					iic_listen_error = true;
				}
			}

			if (iic_listen_error)
			{
				HAL_StatusTypeDef status = HAL_I2C_EnableListen_IT(&hi2c1);
				if (status != HAL_OK)
				{
					iic_listen_error = true;
				}
				else
				{
					iic_listen_error = false;
				}
			}

			//CALC_FUNC_FREQ("Application::Loop");

			//PERIODIC_LOGI(100, "alt:%f,tx:%f,ty:%f,x:%f,y:%f", mTofAlt, total_delta_x, total_delta_y, delta_x, delta_y);

			tx_thread_sleep(2);
	}
}





void Application::TofUpdate()
{
	//运行时间过长，25ms，拖慢系统速度，应该用单独的线程读取
	if (MX_TOF_Process())
	{
		float alt = ToF_Distance;

		//Log::I("tof=%f", alt);
		//alt = compute_vertical_height(alt * 1000, imuex.pitch, imuex.roll) / 1000;
		//alt = compute_vertical_height(alt * 1000, pitch, roll) / 1000;
		//Log::I("tof=%f", alt);
		//超过4米不准
		if (isnan(alt) || alt > 4 || alt < 0)
		{
			mTofAlt = 0;
			return;
		}
		//校正气压计高度
		mTofAlt = alt;
		mIsTofUpdate = true;
		tof_update_tick = HAL_GetTick();
	}
}

bool Application::CheckUpdateTimeout()
{
	if (!mIsInit)
	{
		return false;
	}
	uint32_t now = HAL_GetTick();
	if (tof_update_tick > 0 && (now - tof_update_tick) > 1000)
	{
		return true;
	}
	return false;
}

float Application::GetAlt()
{
	return mTofAlt;
}


void Application::IICReceive()
{
	iic_active_tick = HAL_GetTick();
	iic_active = true;
	//Log::I("IICReceive");
	recv_addr = true;
	HAL_StatusTypeDef status = HAL_I2C_Slave_Seq_Receive_IT(&hi2c1, &reg_addr, 1, I2C_FIRST_FRAME);  // 先收1字节寄存器地址
	if (status != HAL_OK)
	{
		Log::I("IICReceive HAL_I2C_Slave_Seq_Receive_IT err:%x", status);
		iic_listen_error = true;
	}

	memset(&iic_state, 0, sizeof(iic_state) - 2);
	iic_state.step = 1;
	iic_state.IICReceive = iic_state.step;
}

void Application::IICRx()
{
	iic_state.step += 1;
	//Log::I("IICRx:%d - %d", recv_addr, reg_addr);
	if (recv_addr)
	{
		iic_state.IICRx_Addr = iic_state.step;
		recv_addr = false;
		if (reg_addr == 3)
		{
			HAL_StatusTypeDef status = HAL_I2C_Slave_Seq_Receive_IT(&hi2c1, &ping_data, 1, I2C_LAST_FRAME);
			if (status != HAL_OK)
			{
				Log::I("IICRx HAL_I2C_Slave_Seq_Receive_IT error1:%x", status);
				iic_listen_error = true;
			}
		}
		else if (reg_addr == 4)
		{
			//Log::I("IICRx 4, buffsize:%d", sizeof(ahr_data));
			HAL_StatusTypeDef status = HAL_I2C_Slave_Seq_Receive_IT(&hi2c1, (uint8_t *)ahr_data, sizeof(ahr_data), I2C_LAST_FRAME);
			if (status != HAL_OK)
			{
				Log::I("IICRx HAL_I2C_Slave_Seq_Receive_IT error2:%x", status);
				iic_listen_error = true;
			}
		}
		else if (reg_addr == 5)
		{
			HAL_StatusTypeDef status = HAL_I2C_Slave_Seq_Receive_IT(&hi2c1, (uint8_t *)&reset_yaw, sizeof(reset_yaw), I2C_LAST_FRAME);
			if (status != HAL_OK)
			{
				Log::I("IICRx HAL_I2C_Slave_Seq_Receive_IT error3:%x", status);
				iic_listen_error = true;
			}
		}
		else if (reg_addr != 1 && reg_addr != 2)
		{
			// 重新使能监听（该函数会自动处理）
			HAL_StatusTypeDef status = HAL_I2C_EnableListen_IT(&hi2c1);
			if (status != HAL_OK)
			{
				Log::I("IICRx HAL_I2C_EnableListen_IT error4:%x", status);
				iic_listen_error = true;
			}
		}
	}
	else
	{
		iic_state.IICRx_Data = iic_state.step;
		//接收完主机写的数据, 解析数据
		if (reg_addr == 4)
		{
			is_ahr_ready = true;
		}
		else if (reg_addr == 5)
		{
		}
	}
}

void Application::IICTx()
{
	iic_state.step += 1;
	iic_state.IICTx = iic_state.step;
	//Log::I("IICTx");
	// 读操作完成
	is_data_ready = false;
	/*
	HAL_StatusTypeDef status = HAL_I2C_EnableListen_IT(&hi2c1);
	if (status != HAL_OK)
	{
		//Log::I("IICTx HAL_I2C_EnableListen_IT error:%x", status);
		iic_listen_error = true;
	}
	*/
}

void Application::IICError()
{
	iic_state.step += 1;
	iic_state.IICError = iic_state.step;

	iic_error = true;
	iic_active = false;
}

void Application::IICResponse()
{
	iic_state.step += 1;
	//Log::I("IICResponse reg_addr:%d", reg_addr);
	if (reg_addr == 1)
	{
		iic_state.IICResponse_Addr = iic_state.step;
		if (is_data_ready)
		{
			HAL_StatusTypeDef status = HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, (uint8_t *)&tx_data, sizeof(tx_data), I2C_LAST_FRAME);
			if (status != HAL_OK)
			{
				Log::I("IICResponse HAL_I2C_Slave_Seq_Transmit_IT error1:%x", status);
				iic_listen_error = true;
			}
		}
		else
		{
			static st_tx_data zero_data;
			static bool isinit = false;
			if (!isinit)
			{
				zero_data.distance = 0;
				zero_data.tick = 0;
				zero_data.validate = 0;
				isinit = true;
			}
			HAL_StatusTypeDef status = HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, (uint8_t *)&zero_data, sizeof(zero_data), I2C_LAST_FRAME);
			if (status != HAL_OK)
			{
				Log::I("IICResponse HAL_I2C_Slave_Seq_Transmit_IT error2:%x", status);
				iic_listen_error = true;
			}
		}
	}
	//返回whoami
	else if (reg_addr == 2)
	{
		iic_state.IICResponse_Data = iic_state.step;
		static uint8_t whoami = 0x43;
		HAL_StatusTypeDef status = HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, &whoami, 1, I2C_LAST_FRAME);
		if (status != HAL_OK)
		{
			Log::I("IICResponse HAL_I2C_Slave_Seq_Transmit_IT error3:%x", status);
			iic_listen_error = true;
		}
	}
	else if (reg_addr == 3)
	{
		iic_state.IICResponse_Data = iic_state.step;
		HAL_StatusTypeDef status = HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, &ping_data, 1, I2C_LAST_FRAME);
		if (status != HAL_OK)
		{
			Log::I("IICResponse HAL_I2C_Slave_Seq_Transmit_IT error4:%x", status);
			iic_listen_error = true;
		}
	}
	else
	{
		iic_state.IICResponse_Data = iic_state.step;
		HAL_StatusTypeDef status = HAL_I2C_EnableListen_IT(&hi2c1);
		if (status != HAL_OK)
		{
			Log::I("IICResponse HAL_I2C_EnableListen_IT error5:%x", status);
			iic_listen_error = true;
		}
	}
}


void Application::IICDone()
{
	iic_state.step += 1;
	iic_state.IICDone = iic_state.step;
	//Log::I("IICDone");
	// 完成一次通信，清除状态
	HAL_StatusTypeDef status = HAL_I2C_EnableListen_IT(&hi2c1);
	if (status != HAL_OK)
	{
		Log::I("IICDone HAL_I2C_EnableListen_IT error:%x", status);
		iic_listen_error = true;
	}
	iic_active = false;
}


void Application::PushData()
{
	if (is_data_ready)
	{
		return;
	}
	if (CheckUpdateTimeout())
	{
		mTofAlt = 0;
	}
	//Log::I("PushData");
	tx_data.distance = (int16_t)(mTofAlt * 100);
	tx_data.tick = HAL_GetTick();
	int16_t validate =
			tx_data.distance + tx_data.tick;
	tx_data.validate = validate & 0xff;
	is_data_ready = true;
}

void Application::IICDataUpdate()
{
	static uint32_t refresh_tick = 0;
	uint32_t now = HAL_GetTick();
	uint32_t remain = now - refresh_tick;
	uint32_t ticks = 1000 / 50; //50hz更新率
	if (remain > ticks)
	{
		refresh_tick = now;
		if (!is_data_ready)
		{
			PushData();
		}
		if (is_data_ready)
		{

		}
	}
}





#ifdef __cplusplus
extern "C" {
#endif

float GetAlt(void)
{
	return app.GetAlt();
}

#ifdef __cplusplus
}
#endif
