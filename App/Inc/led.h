/*
 * led.h
 *
 *  Created on: Apr 2, 2026
 *      Author: bitstream
 */

#ifndef INC_LED_H_
#define INC_LED_H_

class Led
{
public:
	void Error();
	void Clear();
private:
	uint8_t error_on = 0;
};

extern Led led;

#endif /* INC_LED_H_ */
