/*
 * application.h
 *
 *  Created on: Apr 2, 2026
 *      Author: bitstream
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

class Application
{
public:
	Application();
	~Application();

	void TofUpdate();

	void Init();
	void Loop();

	//检查传感器线程是否死掉
	bool CheckUpdateTimeout();

	float GetAlt();

	void IICResponse();
	void IICReceive();
	void IICRx();
	void IICTx();
	void IICError();
	void IICDone();

private:

	//是否接收寄存器地址
	volatile bool recv_addr = false;
	uint8_t reg_addr;     // 保存要读/写的寄存器地址
	typedef struct __attribute__((packed)){
		int16_t distance; //计算出的距离
		uint32_t tick;
		uint8_t validate;
	}st_tx_data;      // 接收要写入的数据（单字节示例，多字节可用数组）   ax, ay, az, gx, gy, gz, roll, pitch, yaw, alt, v
	st_tx_data tx_data;
	volatile bool is_data_ready = false;
	volatile bool iic_listen_error = false;
	volatile bool iic_error = false;

	volatile uint32_t iic_active_tick = 0; //iic活动时间，用来检测多久没收到数据了
	volatile bool     iic_active = false;

	//主机发送一个字节，然后再读取，判断读取的数据和写入的是否一致，确定iic通信正常
	uint8_t ping_data;

	//接收姿态数据
	int16_t ahr_data[6];
	volatile bool is_ahr_ready = false;

	//reset 命令5数据
	float reset_yaw = 0;

	void PushData();
	void IICDataUpdate();

	volatile bool mIsInit = false;

	float mTofAlt = 0;
	bool  mIsTofUpdate = false;


	uint32_t tof_update_tick = 0;



};


extern Application app;



#endif /* INC_APPLICATION_H_ */
