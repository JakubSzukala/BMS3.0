/*
 * battery_pack.c
 *
 *  Created on: Sep 7, 2021
 *      Author: bursk
 */
#include "battery_pack.h"

void BqPack_StructInit(bq_pack *pack)
{
	pack -> voltage = 0;
	pack -> temperature1 = 0;
	pack -> temperature2 = 0;
	pack -> lowest_cell_volts = 0;
	pack -> highest_cell_volts = 0;
	pack -> op_mode = 0;
}

void BqPack_StructUpdate(bq_pack *pack, char *control)
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

