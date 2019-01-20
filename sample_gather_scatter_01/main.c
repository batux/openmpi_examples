/*
 * main.c
 *
 *  Created on: Jan 20, 2019
 *      Author: batuhan düzgün
 */

#include <stdio.h>
#include "mpi.h"

#define SIZE 4

int main(int argc, char *argv[])
{
	int taskCount, rank;

	float sendBuffer[SIZE][SIZE] = {
			{1.0, 2.0, 3.0, 4.0},
			{5.0, 6.0, 7.0, 8.0},
			{9.0, 10.0, 11.0, 12.0},
			{13.0, 14.0, 15.0, 16.0}
	};

	float receiveBuffer[SIZE];

	float gatherSendBuffer[SIZE];
	float gatherReceiveBuffer[SIZE][SIZE];

	float totalReducedValue;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &taskCount);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(taskCount == 4)
	{
		int sourceRank = 1;
		int scatterSendDataSize = SIZE;
		int scatterReceiveDataSize = SIZE;

		// Task-1 will scather data to other tasks!
		MPI_Scatter(&sendBuffer, scatterSendDataSize, MPI_FLOAT,
				&receiveBuffer, scatterReceiveDataSize, MPI_FLOAT, sourceRank, MPI_COMM_WORLD);

		// We will show the received array from Task-1
		printf("rank= %d  Results: %f %f %f %f\n",rank,receiveBuffer[0],
				receiveBuffer[1],receiveBuffer[2],receiveBuffer[3]);

		// Multiply received array and sum total value!
		int index;
		float totalReceivedValue = 0.0;
		int receiveBufferSize = SIZE;
		for(index = 0; index < receiveBufferSize; index++)
		{
			gatherSendBuffer[index] = receiveBuffer[index] * 2;
			totalReceivedValue += receiveBuffer[index];
		}

		printf("Task%d total value: %f\n", rank, totalReceivedValue);

		int gatherSendDataSize = SIZE;
		int gatherReceiveDataSize = SIZE;
		int gatherSourceRank = 1;
		// Task-1 will gather multiplied arrays from other tasks!
		MPI_Gather(&gatherSendBuffer, gatherSendDataSize, MPI_FLOAT,
				&gatherReceiveBuffer, gatherReceiveDataSize, MPI_FLOAT, gatherSourceRank, MPI_COMM_WORLD);

		int reduceSourceRank = 1;
		// We will reduce values and the result will be only sent to Task-1!
		MPI_Reduce(&totalReceivedValue, &totalReducedValue, 1, MPI_FLOAT, MPI_SUM, reduceSourceRank, MPI_COMM_WORLD);

		// We will reduce values and the result will be sent to all tasks in the communication world!
		MPI_Allreduce(&totalReceivedValue, &totalReducedValue, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);


		if(rank == 1)
		{
			printf("********* TASK-%d MPI_Gather and MPI_Reduce Results ************\n", rank);

			// Show gathered matrix on Task-1
			int i,j;
			for(i = 0; i < SIZE; i++)
			{
				for(j = 0; j < SIZE; j++)
				{
					printf(" %f ", gatherReceiveBuffer[i][j]);
				}
				printf("\n");
			}

			// Also reduced total value will be shown on Task-1
			printf("TASK-%d Reduced Value: %f \n", rank, totalReducedValue);
		}
		else
		{
			printf("TASK-%d Reduced Value: %f \n", rank, totalReducedValue);
		}
	}

	MPI_Finalize();

	return 0;
}
