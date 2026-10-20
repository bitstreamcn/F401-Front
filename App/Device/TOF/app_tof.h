/**
  ******************************************************************************
  * @file          : app_tof.h
  * @author        : IMG SW Application Team
  * @brief         : This file provides code for the configuration
  *                  of the STMicroelectronics.X-CUBE-TOF1.3.4.3 instances.
  ******************************************************************************
  *
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
//TOF 配置教程：
// https://www.st.com.cn/resource/en/user_manual/um2974-how-to-use-the-vl53l5cx-with-stmicroelectronics-xcubetof1-timeofflight-sensor-software-packages-for-stm32cubemx-stmicroelectronics.pdf
// https://atta.szlcsc.com/upload/public/pdf/source/20221107/F1D94AEC77977327D037247A3E95176C.pdf

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_TOF_H
#define __APP_TOF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Exported defines ----------------------------------------------------------*/

extern volatile uint8_t ToF_EventDetected;
extern float ToF_Distance;

/* Exported functions --------------------------------------------------------*/
bool MX_TOF_Init(void);
bool MX_TOF_Process(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_TOF_H */
