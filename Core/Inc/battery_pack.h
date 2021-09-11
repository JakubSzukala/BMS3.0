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

#include "current_sensor.h"

/* Constants */
/* Errors levels*/
#define OVERVOLTAGE_ERR 120
#define UNDERVOLTAGE_ERR 82
#define HIGHTEMP_ERR 55
#define LOWTEMP_ERR 0
#define BAL_ERR 0.5

/* Warnings levels */
#define OVERVOLTAGE_WARN 117.5
#define UNDERVOLTAGE_WARN 83
#define HIGHTEMP_WARN 45
#define LOWTEMP_WARN 5
#define BAL_WARN 0.1

/* Errors / warnings IDs */
typedef enum{
	BqPack_OK = 0x0,
	BqPack_ErrOV = 0x01,
	BqPack_ErrUV = 0x02,
	BqPack_ErrHT = 0x03,
	BqPack_ErrLT = 0x04,
	BqPack_WarOV = 0x05,
	BqPack_WarUV = 0x06,
	BqPack_WarHT = 0x07,
	BqPack_WarLT = 0x08,
	BqPack_ErrBal = 0x09,
	BqPack_WarBal = 0x0A
}BqPack_Error_Status;

typedef struct BQ_PACK
{
	uint32_t voltage;
	uint16_t temperature1;
	uint16_t temperature2;
	uint16_t lowest_cell_volts;
	uint16_t highest_cell_volts;
	uint8_t op_mode;

	uint16_t warnings;
	uint16_t error;

	uint32_t current;

	uint32_t power;
	uint16_t avg_temperature;
	uint8_t charge_level;
}bq_pack;

extern bq_pack battery_pack;
extern uint8_t error_flag;
/* Functions declarations */
/* Structure updates */
void BqPack_StructInit(bq_pack *pack);
void BqPack_StructUpdate_CurrSensor(bq_pack *pack, CurrentData *control);
void BqPack_StructUpdate_MSP430(bq_pack *pack, char *control);
void BqPack_RecalculateData(bq_pack *pack);

/* Error checking */
uint8_t BqPack_CheckForErrors(bq_pack *pack);
void BqPack_CheckForWarnings(bq_pack *pack);
BqPack_Error_Status VoltageErrorCheck(uint32_t *voltage);
BqPack_Error_Status TemperatureErrorCheck(uint16_t *t1, uint16_t *t2);
BqPack_Error_Status VoltageDiffErrorCheck(uint16_t *voltage1, uint16_t *voltage2);
uint8_t VoltageWarningCheck(uint32_t *voltage);
uint8_t TemperatureWarningCheck(uint16_t *t1, uint16_t *t2);
uint8_t VoltageDiffWarningCheck(uint16_t *voltage1, uint16_t *voltage2);

void check_warning_error_conditions(bq_pack *pack);
void CAN_comm(bq_pack *pack); // when errors present change message type

#endif /* INC_BATTERY_PACK_H_ */
