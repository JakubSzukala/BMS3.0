/*
 * battery_pack.c
 *
 *  Created on: Sep 7, 2021
 *      Author: bursk
 */
#include <math.h>

#include "battery_pack.h"
#include "fifo_buffer.h"


CanDataFrameInit can_frame_template;

/* Global variables */
queue_t Fifo_Queue;
uint8_t error_flag;

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

uint8_t BqPack_CheckForErrors(bq_pack *pack)
{
	error_flag = VoltageErrorCheck(&(pack->voltage));
	if(error_flag != BqPack_OK)
	{
		CanSendPdo(hcan, 0x85, 8, &can_frame_template, error_flag, (uint8_t)((pack->voltage)/1000), 0, 0, 0, 0, 0, 0);
		return error_flag;
	}

	error_flag = TemperatureErrorCheck(&pack->temperature1, &pack->temperature2);
	if(error_flag != BqPack_OK)
	{
		uint8_t htemp = (pack->temperature1 > pack->temperature2) ? pack->temperature1 : pack->temperature2;
		CanSendPdo(hcan, 0x85, 8, &can_frame_template, error_flag, (uint8_t)htemp + 30, 0, 0, 0, 0, 0, 0);
		return error_flag;
	}

	error_flag = VoltageDiffErrorCheck(&pack->highest_cell_volts, &pack->lowest_cell_volts);
	if(error_flag != BqPack_OK)
	{
		uint8_t volt_diff = abs(pack->highest_cell_volts - pack->lowest_cell_volts);
		CanSendPdo(hcan, 0x85, 8, &can_frame_template, error_flag, volt_diff, 0, 0, 0, 0, 0, 0);
		return error_flag;
	}
	return error_flag;
}

BqPack_Error_Status VoltageErrorCheck(uint32_t *voltage)
{
	if(*voltage > OVERVOLTAGE_ERR) 		return BqPack_ErrOV;
	if(*voltage < UNDERVOLTAGE_ERR) 	return BqPack_ErrUV;

	return BqPack_OK;
}

BqPack_Error_Status TemperatureErrorCheck(uint16_t *t1, uint16_t *t2)
{
	if(*t1 > HIGHTEMP_ERR || *t2 > HIGHTEMP_ERR) 	return BqPack_ErrHT;
	if(*t1 < LOWTEMP_ERR || *t2 < LOWTEMP_ERR) 		return BqPack_ErrLT;

	return BqPack_OK;
}

BqPack_Error_Status VoltageDiffErrorCheck(uint16_t *voltage1, uint16_t *voltage2)
{
	if(abs(*voltage1 - *voltage2) > BAL_ERR) 	return BqPack_ErrBal;

	return BqPack_OK;
}
/*************************************************************************************/
void BqPack_CheckForWarnings(bq_pack *pack)
{
	pack->warnings += VoltageWarningCheck(&pack->voltage);
	pack->warnings += TemperatureWarningCheck(&pack->temperature1, &pack->temperature2);
	pack->warnings += VoltageDiffErrorCheck(&pack->highest_cell_volts, &pack->lowest_cell_volts);

	if(pack->warnings != 0)
	{
		uint8_t htemp = (pack->temperature1 > pack->temperature2) ? pack->temperature1 : pack->temperature2;
		uint8_t volt_diff = abs(pack->highest_cell_volts - pack->lowest_cell_volts);
		CanSendPdo(hcan, 0x86, 8, &can_frame_template, pack->warnings, (uint8_t)(pack->voltage/1000), htemp, volt_diff, 0, 0, 0, 0);
	}

}

uint8_t VoltageWarningCheck(uint32_t *voltage)
{
	if(*voltage > OVERVOLTAGE_WARN)		return pow(2, 0);
	if(*voltage < UNDERVOLTAGE_WARN)	return pow(2, 1);
	return 0;
}

uint8_t TemperatureWarningCheck(uint16_t *t1, uint16_t *t2)
{
	if(*t1 > HIGHTEMP_WARN || *t2 > HIGHTEMP_WARN) 	return pow(2, 2);
	if(*t1 < LOWTEMP_WARN || *t2 < LOWTEMP_WARN) 	return pow(2, 3);
	return 0;
}

uint8_t VoltageDiffWarningCheck(uint16_t *voltage1, uint16_t *voltage2)
{
	if(abs(*voltage1 - *voltage2) > BAL_WARN) 	return pow(2, 4);
	return 0;
}


