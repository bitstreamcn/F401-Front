/*
 * cppmain.h
 *
 *  Created on: Apr 2, 2026
 *      Author: bitstream
 */

#ifndef INC_CPPMAIN_H_
#define INC_CPPMAIN_H_

#include "app_azure_rtos.h"

#ifdef __cplusplus
extern "C" {
#endif

void cppmain(void);

extern TX_SEMAPHORE _200hz_semaphore;
extern TX_SEMAPHORE _500hz_semaphore;
extern TX_SEMAPHORE tof_semaphore;

int IsArmed();

#ifdef __cplusplus
}
#endif


#endif /* INC_CPPMAIN_H_ */
