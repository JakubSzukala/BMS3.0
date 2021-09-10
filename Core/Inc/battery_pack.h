/*
 * battery_pack.h
 *
 *  Created on: Sep 7, 2021
 *      Author: js
 */

#ifndef INC_BATTERY_PACK_H_
#define INC_BATTERY_PACK_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct BQ_PACK
{
	uint32_t voltage;
	uint16_t temperature1;
	uint16_t temperature2;
	uint16_t lowest_cell_volts;
	uint16_t highest_cell_volts;
	uint8_t op_mode;
	uint8_t error_status;


}bq_pack;

/* Functions declarations */
void BqPack_StructInit(bq_pack *pack);
void BqPack_StructUpdate(bq_pack *pack, char *control); // this will update from rx and
// also calculate power, battery charge level average temperature and difference between high and low vol
void check_warning_error_conditions(bq_pack *pack);
void CAN_comm(bq_pack *pack); // when errors present change message type

#endif /* INC_BATTERY_PACK_H_ */
