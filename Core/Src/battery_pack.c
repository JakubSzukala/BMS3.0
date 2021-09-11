/*
 * battery_pack.c
 *
 *  Created on: Sep 7, 2021
 *      Author: bursk
 */
#include "battery_pack.h"

CanDataFrameInit can_frame_template;

void BqPack_StructInit(bq_pack *pack)
{
	pack -> voltage = 0;
	pack -> temperature1 = 0;
	pack -> temperature2 = 0;
	pack -> lowest_cell_volts = 0;
	pack -> highest_cell_volts = 0;
	pack -> op_mode = 0;
}

void BqPack_StructUpdate_CurrSensor(bq_pack *pack, CurrentData *control)
{
	pack->current = control->current;
}


void BqPack_RecalculateData(bq_pack *pack)
{
	pack->power = pack->current * pack->voltage;
	pack->charge_level = 0;
	pack->avg_temperature = (pack->temperature1 + pack->temperature2)/2;
}


void BqPack_StructUpdate_MSP430(bq_pack *pack, char *control)
{
	char *data;

	data = strtok(control, ";");
	if(data != NULL)
	{
		uint32_t temp = (uint32_t)strtol(data, NULL, 0);
		if(temp)
		{
			pack -> voltage = temp;
		}
	}

	data = strtok(NULL, ";");
	if(data != NULL)
	{
		uint16_t temp = (uint16_t)strtol(data, NULL, 0);
		if(temp)
		{
			pack -> temperature1 = temp;
		}
	}

	data = strtok(NULL, ";");
	if(data != NULL)
	{
		uint16_t temp = (uint16_t)strtol(data, NULL, 0);
		if(temp)
		{
			pack -> temperature2 = temp;
		}
	}

	data = strtok(NULL, ";");
	if(data != NULL)
	{
		uint16_t temp = (uint16_t)strtol(data, NULL, 0);
		if(temp)
		{
			pack -> lowest_cell_volts = temp;
		}
	}

	data = strtok(NULL, ";");
	if(data != NULL)
	{
		uint16_t temp = (uint16_t)strtol(data, NULL, 0);
		if(temp)
		{
			pack -> highest_cell_volts = temp;
		}
	}

	data = strtok(NULL, ";");
	if(data != NULL)
	{
		uint16_t temp = (uint16_t)strtol(data, NULL, 0);
		if(temp)
		{
			pack -> op_mode = temp;
		}
	}
}

void BqPack_CheckForErrors(bq_pack *pack)
{
	BqPack_Error_Status status;
	status = VoltageErrorCheck(&(pack->voltage));
	if(status != BqPack_OK)
	{
		CanSendPdo(hcan,
				0x85,
				8,
				&can_frame_template,
				status, 0, 0, 0, 0, 0, 0, 0);
		CanClearTxDataFrame(&can_frame_template);
	}

	status = TemperatureErrorCheck(&(pack->temperature1), &(pack->temperature2));
	if(status != BqPack_OK)
	{
		CanSendPdo(hcan,
				0x85,
				8,
				&can_frame_template,
				status, 0, 0, 0, 0, 0, 0, 0);
		CanClearTxDataFrame(&can_frame_template);
	}

	status = VoltageDiffErrorCheck(&(pack->highest_cell_volts), &(pack->lowest_cell_volts));
	if(status != BqPack_OK)
	{
		CanSendPdo(hcan,
				0x85,
				8,
				&can_frame_template,
				status, 0, 0, 0, 0, 0, 0, 0);
		CanClearTxDataFrame(&can_frame_template);
	}
}

BqPack_Error_Status VoltageErrorCheck(uint32_t *voltage)
{
	if(*voltage > OVERVOLTAGE_ERR) 		return BqPack_ErrOV;
	if(*voltage < UNDERVOLTAGE_ERR) 	return BqPack_ErrUV;
	if(*voltage > OVERVOLTAGE_WARN)		return BqPack_WarOV;
	if(*voltage < UNDERVOLTAGE_WARN)	return BqPack_WarUV;

	return BqPack_OK;
}

BqPack_Error_Status TemperatureErrorCheck(uint16_t *t1, uint16_t *t2)
{
	if(*t1 > HIGHTEMP_ERR || *t2 > HIGHTEMP_ERR) 	return BqPack_ErrHT;
	if(*t1 < LOWTEMP_ERR || *t2 < LOWTEMP_ERR) 		return BqPack_ErrLT;
	if(*t1 > HIGHTEMP_WARN || *t2 > HIGHTEMP_WARN) 	return BqPack_WarHT;
	if(*t1 < LOWTEMP_WARN || *t2 < LOWTEMP_WARN) 	return BqPack_WarLT;

	return BqPack_OK;
}

BqPack_Error_Status VoltageDiffErrorCheck(uint16_t *voltage1, uint16_t *voltage2)
{
	if(abs(*voltage1 - *voltage2) > BAL_ERR) 	return BqPack_ErrBal;
	if(abs(*voltage1 - *voltage2) > BAL_WARN) 	return BqPack_WarBal;

	return BqPack_OK;
}
