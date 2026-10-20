/*
 * led.cpp
 *
 *  Created on: Apr 2, 2026
 *      Author: bitstream
 */
#include "main.h"
#include "led.h"

Led led;

void Led::Error()
{
	if (error_on == 0)
	{
		//HAL_GPIO_WritePin(LedError_GPIO_Port, LedError_Pin, GPIO_PIN_SET);
		error_on = 1;
	}
}
void Led::Clear()
{
	if (error_on == 1)
	{
		//HAL_GPIO_WritePin(LedError_GPIO_Port, LedError_Pin, GPIO_PIN_RESET);
		error_on = 0;
	}
}
