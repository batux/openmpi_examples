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
	int taskNumber, rank;

	float sendBuffer[SIZE][SIZE] = {
				{1.0, 2.0, 3.0, 4.0},
				{5.0, 6.0, 7.0, 8.0},
				{9.0, 10.0, 11.0, 12.0},
				{13.0, 14.0, 15.0, 16.0}
	};

	float receivedRowArray[SIZE];
	float receivedColumnArray[SIZE];

	MPI_Status stat[2];
	MPI_Datatype rowType;
	MPI_Datatype columnType;


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &taskNumber);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	MPI_Type_contiguous(SIZE, MPI_FLOAT, &rowType);
	MPI_Type_commit(&rowType);

	MPI_Type_vector(SIZE, 1, SIZE, MPI_FLOAT, &columnType);
	MPI_Type_commit(&columnType);

	if(rank == 0)
	{
		// Master task
		int taskId;
		for(taskId=0; taskId < SIZE; taskId++)
		{
			MPI_Send(&sendBuffer[taskId][0], 1, rowType, taskId, 1, MPI_COMM_WORLD);
			MPI_Send(&sendBuffer[0][taskId], 1, columnType, taskId, 2, MPI_COMM_WORLD);
		}
	}

	int receiveSize = SIZE;
	int source = 0; // source is master task id!
	MPI_Recv(receivedRowArray, receiveSize, MPI_FLOAT, source, 1, MPI_COMM_WORLD, &stat[0]);
	MPI_Recv(receivedColumnArray, receiveSize, MPI_FLOAT, source, 2, MPI_COMM_WORLD, &stat[1]);

	printf("[MPI_Type_contiguous Way] Received Row Array: ");
	int k;
	for(k=0; k < SIZE; k++)
	{
		printf(" %f ", receivedRowArray[k]);
	}
	printf("\n");

	printf("[MPI_Type_vector Way] Received Column Array: ");
	for(k=0; k < SIZE; k++)
	{
		printf(" %f ", receivedColumnArray[k]);
	}
	printf("\n");

	MPI_Type_free(&rowType);
	MPI_Type_free(&columnType);
	MPI_Finalize();

	return 0;
}
