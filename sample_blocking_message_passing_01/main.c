/*
 * main.c
 *
 *  Created on: Jan 20, 2019
 *      Author: batuhan düzgün
 */

#include <stdio.h>
#include "mpi.h"

#define MASTER 0
#define WORKER 1

int main(int argc, char *argv[])
{
	int taskCount, rank, sendDestination, receiveSource, errorCode=-1, count, tag=1;
	int master=-1, worker=-1;
	char receivedMessage, masterSentMessage='x', workerSentMessage='y';
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &taskCount);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(taskCount != 2)
	{
		MPI_Abort(MPI_COMM_WORLD, errorCode);
		return 0;
	}

	master = MASTER;
	worker = WORKER;

	if(rank == master)
	{
		sendDestination = worker;
		receiveSource = worker;
		MPI_Send(&masterSentMessage, 1, MPI_CHAR, sendDestination, tag, MPI_COMM_WORLD);
		MPI_Recv(&receivedMessage, 1, MPI_CHAR, receiveSource, tag, MPI_COMM_WORLD, &status);
	}
	else if(rank == worker)
	{
		sendDestination = master;
		receiveSource = master;
		MPI_Recv(&receivedMessage, 1, MPI_CHAR, receiveSource, tag, MPI_COMM_WORLD, &status);
		MPI_Send(&workerSentMessage, 1, MPI_CHAR, sendDestination, tag, MPI_COMM_WORLD);
	}

	MPI_Get_count(&status, MPI_CHAR, &count);

	printf("Task %d:Received Message: %c, Received %d char(s) from task %d with tag %d error code: %d \n",
			rank, receivedMessage, count, status.MPI_SOURCE, status.MPI_TAG, status.MPI_ERROR);

	MPI_Finalize();

	return 0;
}
