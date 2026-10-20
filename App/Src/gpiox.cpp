/*
 * gpiox.cpp
 *
 *  Created on: Apr 15, 2026
 *      Author: bitstream
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"



extern void ( *Dio1IrqHandler )( void );

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
        case INT_Pin:
        	//GPIO_PIN_8
        	//前视雷达
        	break;

    }
}


#ifdef __cplusplus
}
#endif
