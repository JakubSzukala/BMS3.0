/*
 * current_sensor.c
 *
 *  Created on: Sep 10, 2021
 *      Author: bursk
 */

#include "current_sensor.h"
#include "can.h"

void GetRawData(CurrentData *current_data, CanDataFrameInit *can_rx_frame){
	current_data->rx_header = can_rx_frame->rx_header;
	current_data->current = 0;
	uint32_t buffer;
	buffer = (uint32_t)(can_rx_frame->rx_data[3]) << 0;
	current_data->current += buffer;

	buffer = (uint32_t)(can_rx_frame->rx_data[2]) << 8;
	current_data->current += buffer;

	buffer = (uint32_t)(can_rx_frame->rx_data[1]) << 16;
	current_data->current += buffer;

	buffer = (uint32_t)(can_rx_frame->rx_data[0]) << 24;
	current_data->current += buffer;

}
