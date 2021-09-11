/*
 * fifo_buffer.c
 *
 *  Created on: 11 wrz 2021
 *      Author: bursk
 */

#include "fifo_buffer.h"
#include "can.h"

#include <assert.h>

queue_t Fifo_Queue;

void Queue_Init()
{
	Fifo_Queue.head = 0;
	Fifo_Queue.tail = 0;
	Fifo_Queue.size = 5;
	Fifo_Queue.data = malloc(sizeof(frame_to_send*) * 5);
}

/* Pass global variable FifoQueue as argument */
void* Queue_TxCanMessage() {
    if (Fifo_Queue.tail == Fifo_Queue.head) {
        return NULL;
    }
    frame_to_send *handle = Fifo_Queue.data[Fifo_Queue.tail];
    CanSendPdo(hcan, handle->tx_header, 8, &can_frame_template,
    		handle->tx_data[0],
			handle->tx_data[1],
			handle->tx_data[2],
			handle->tx_data[3],
			handle->tx_data[4],
			handle->tx_data[5],
			handle->tx_data[6],
			handle->tx_data[7]);

    CanClearTxDataFrame(&can_frame_template);
    Fifo_Queue.data[Fifo_Queue.tail] = NULL;
    Fifo_Queue.tail = (Fifo_Queue.tail + 1) % Fifo_Queue.size;
    return handle;
}

/* frame to send is just header (id) and data as array of uint8_t*/
int Queue_AddTxCanMessage(frame_to_send* handle) {
    if (((Fifo_Queue.head + 1) % Fifo_Queue.size) == Fifo_Queue.tail) {
        return -1;
    }
    Fifo_Queue.data[Fifo_Queue.head] = handle;
    Fifo_Queue.head = (Fifo_Queue.head + 1) % Fifo_Queue.size;
    return 0;
}
