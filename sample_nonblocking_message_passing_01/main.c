/*
 * main.c
 *
 *  Created on: Jan 20, 2019
 *      Author: batuhan düzgün
 */

#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
	int taskCount, myRank, previousRank, nextRank, lastTaskRank,
		nextRankOperationTag=1, previousRankOperationTag=2;
	long receivedDataFromPreviousRank, receivedDataFromNextRank;

	MPI_Request requestList[4];
	MPI_Status statusList[4];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &taskCount);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	lastTaskRank = (taskCount - 1);
	previousRank = myRank - 1;
	nextRank = myRank + 1;

	if(myRank == 0)
	{
		previousRank = lastTaskRank;
	}
	if(myRank == lastTaskRank)
	{
		nextRank = 0;
	}

	int k;
	long data = -1;
	for(k=0; k < 50; k++)
	{
		MPI_Irecv(&receivedDataFromPreviousRank, 1, MPI_LONG, previousRank, previousRankOperationTag, MPI_COMM_WORLD, &requestList[0]);
		MPI_Irecv(&receivedDataFromNextRank, 1, MPI_LONG, nextRank, nextRankOperationTag, MPI_COMM_WORLD, &requestList[1]);

		if(k > 0)
		{
			MPI_Isend(&data, 1, MPI_LONG, previousRank, nextRankOperationTag, MPI_COMM_WORLD, &requestList[2]);
			MPI_Isend(&data, 1, MPI_LONG, nextRank, previousRankOperationTag, MPI_COMM_WORLD, &requestList[3]);
		}
		else
		{
			long myRankAsLong = myRank;
			MPI_Isend(&myRankAsLong, 1, MPI_LONG, previousRank, nextRankOperationTag, MPI_COMM_WORLD, &requestList[2]);
			MPI_Isend(&myRankAsLong, 1, MPI_LONG, nextRank, previousRankOperationTag, MPI_COMM_WORLD, &requestList[3]);
		}

		MPI_Waitall(4, requestList, statusList);

		printf("[Step %d] Task Id: %d received from previous: %ld, received from next: %ld \n", k, myRank, receivedDataFromPreviousRank, receivedDataFromNextRank);

		data = receivedDataFromPreviousRank + receivedDataFromNextRank;
	}

	MPI_Finalize();

	return 0;
}
