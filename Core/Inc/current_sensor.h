/*
 * current_sensor.h
 *
 *  Created on: Sep 10, 2021
 *      Author: bursk
 */

#ifndef INC_CURRENT_SENSOR_H_
#define INC_CURRENT_SENSOR_H_

#include "stm32f1xx.h"
#include "can.h"


typedef struct{
	CAN_RxHeaderTypeDef rx_header;
	uint32_t current;
}CurrentData;

/* Global variable for storing current sensor data */
extern CurrentData current_data;

void GetRawData(CurrentData *current, CanDataFrameInit *can_frame);

#endif /* INC_CURRENT_SENSOR_H_ */
