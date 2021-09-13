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
uint8_t error_flag;
uint8_t vehicle_active_flag;

void BqPack_StructInit(bq_pack *pack)
{
	pack -> voltage = 100000;
	pack -> temperature[0] = 30;
	pack -> temperature[1] = 40;
	pack -> temperature[2] = 50;
	pack -> temperature[3] = 30;
	pack -> temperature[4] = 30;
	pack -> temperature[5] = 220;
	pack -> temperature[6] = 30;
	pack -> temperature[7] = 30;
	pack -> temperature[8] = 30;
	pack -> temperature[9] = 30;
	pack -> temperature[10] = 30;
	pack -> temperature[11] = 30;

	pack->cell_voltages[0] = 3200;
	pack->cell_voltages[1] = 4000;
	for(uint8_t i = 2; i < NUM_OF_CELLS ; i++)
	{
		pack->cell_voltages[i] = 3500;
	}

	pack->warnings = 0;

	pack->current = 2200;
	pack->power = 4000;
	pack->avg_temperature = 3500;
	pack->charge_level = 100;

	pack -> lowest_cell_volts = 3200;
	pack -> highest_cell_volts = 4000;
	pack -> op_mode = 0;

	/* Init global flags */
	error_flag = 0;
	charging_flag = 1;
	vehicle_active_flag = 0;
}

void BqPack_StructUpdate_CurrSensor(bq_pack *pack, CurrentData *control)
{
	pack->current = control->current;
}


void BqPack_RecalculateData(bq_pack *pack)
{
	pack->power = pack->current * pack->voltage;
	pack->charge_level = 0;
	pack->avg_temperature = (	pack->temperature[0] +
								pack->temperature[1] +
								pack->temperature[2] +
								pack->temperature[3] +
								pack->temperature[4] +
								pack->temperature[5] +
								pack->temperature[6] +
								pack->temperature[7] +
								pack->temperature[8] +
								pack->temperature[9] +
								pack->temperature[10]+
								pack->temperature[11])/12;
}


void BqPack_StructUpdate_MSP430(bq_pack *pack, char *control)
{
	char *ptr = control + 1;

	// voltages read
	if(*control == 'V')
	{
		for(uint8_t i = 0; i < NUM_OF_CELLS; i++)
		{
			char *data;
			data = strtok(ptr, ";");
			if(data != NULL)
			{
				uint32_t temp = (uint32_t)strtol(data, NULL, 0);
				if(temp)
				{
					pack->cell_voltages[i] = temp;
				}
			}
		}
	}

	if(*control == 'T')
	{
		for(uint8_t i = 0; i < NUM_OF_TEMPS; i++)
		{
			char *data;
			data = strtok(ptr, ";");
			if(data != NULL)
			{
				uint32_t temp = (uint32_t)strtol(data, NULL, 0);
				if(temp)
				{
					pack->temperature[i] = temp;
				}
			}
		}
	}
	if(*control == 'G')
	{
		char *data;
		data = strtok(ptr, ";");
		if(data != NULL)
		{
			uint32_t temp = (uint32_t)strtol(data, NULL, 0);
			if(temp)
			{
				pack->voltage = temp;
			}
		}

		data = strtok(ptr, ";");
		if(data != NULL)
		{
			uint32_t temp = (uint32_t)strtol(data, NULL, 0);
			if(temp)
			{
				pack->highest_cell_volts = temp;
			}
		}

		data = strtok(ptr, ";");
		if(data != NULL)
		{
			uint32_t temp = (uint32_t)strtol(data, NULL, 0);
			if(temp)
			{
				pack->lowest_cell_volts = temp;
			}
		}
	}
}

uint8_t BqPack_CheckForErrors(bq_pack *pack)
{
	uint8_t error;
	error = VoltageErrorCheck(&(pack->voltage));
	if(error!= BqPack_OK)
	{
		SetErrorFlag();
		CanSendPdo(hcan, 0x85, 8, &can_frame_template, error, (uint8_t)((pack->voltage)/1000), 0, 0, 0, 0, 0, 0);
		return error;
	}

	error = TemperatureErrorCheck(&battery_pack.temperature);
	if(error != BqPack_OK)
	{
		SetErrorFlag();
		uint8_t htemp = 0;
		for(uint8_t i = 0; i < 12 ; i++)
		{
			if(htemp < battery_pack.temperature[i]) htemp = battery_pack.temperature[i];
		}
		CanSendPdo(hcan, 0x85, 8, &can_frame_template, error, (uint8_t)htemp + 30, 0, 0, 0, 0, 0, 0);
		return error;
	}

	error = VoltageDiffErrorCheck(&pack->highest_cell_volts, &pack->lowest_cell_volts);
	if(error_flag != BqPack_OK)
	{
		SetErrorFlag();
		uint8_t volt_diff = abs(pack->highest_cell_volts - pack->lowest_cell_volts);
		CanSendPdo(hcan, 0x85, 8, &can_frame_template, error, volt_diff, 0, 0, 0, 0, 0, 0);
		return error;
	}
	return error;
}

BqPack_Error_Status VoltageErrorCheck(uint32_t *voltage)
{
	if(*voltage > OVERVOLTAGE_ERR) 		return BqPack_ErrOV;
	if(*voltage < UNDERVOLTAGE_ERR) 	return BqPack_ErrUV;

	return BqPack_OK;
}

BqPack_Error_Status TemperatureErrorCheck(uint8_t (*temperature)[12])
{
	for(uint8_t i = 0; i < 12; i++)
	{
		if(*temperature[i] > HIGHTEMP_ERR) return BqPack_ErrHT;
		if(*temperature[i] < LOWTEMP_ERR) return BqPack_ErrLT;
	}

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
	pack->warnings += TemperatureWarningCheck(&pack->temperature);
	pack->warnings += VoltageDiffErrorCheck(&pack->highest_cell_volts, &pack->lowest_cell_volts);

	if(pack->warnings != 0)
	{
		uint8_t htemp = 0;
		for(uint8_t i = 0; i < 12 ; i++)
		{
			if(htemp < battery_pack.temperature[i]) htemp = battery_pack.temperature[i];
		}
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

uint8_t TemperatureWarningCheck(uint8_t (*temperature)[12])
{
	for(uint8_t i = 0; i < 12; i++)
	{
		if(*temperature[i] > HIGHTEMP_WARN) return pow(2, 2);
		if(*temperature[i] < LOWTEMP_WARN) return pow(2, 3);
	}
	return 0;
}

uint8_t VoltageDiffWarningCheck(uint16_t *voltage1, uint16_t *voltage2)
{
	if(abs(*voltage1 - *voltage2) > BAL_WARN) 	return pow(2, 4);
	return 0;
}

void SetErrorFlag()
{
	// set a flag and immidiately turn off stycznik
	error_flag = 1;
	HAL_GPIO_WritePin(PWR_SWITCH_GPIO_Port, PWR_SWITCH_Pin, GPIO_PIN_RESET);
}
uint8_t GetErrorFlag()
{
	return error_flag;
}

void SetVehicleActiveFlag(uint8_t state)
{
	vehicle_active_flag = state;
}

uint8_t GetVehivleActivveFlag()
{
	return vehicle_active_flag;
}
