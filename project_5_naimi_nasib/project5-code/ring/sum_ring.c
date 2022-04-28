/****************************************************************
 *                                                              *
 * This file has been written as a sample solution to an        *
 * exercise in a course given at the CSCS-USI Summer School.    *
 * It is made freely available with the understanding that      *
 * every copy of this file must include this header and that    *
 * CSCS/USI take no responsibility for the use of the enclosed  *
 * teaching material.                                           *
 *                                                              *
 * Purpose: Parallel sum using a ping-pong                      *
 *                                                              *
 * Contents: C-Source                                           *
 *                                                              *
 ****************************************************************/


#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int my_rank, size;
    int snd_buf, rcv_buf;
    int right, left;
    int sum, i;

    MPI_Status  status;
    MPI_Request request;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &size);


    printf ("Nr Processes %i \n", size);
    // implement ring neighbours
    right = (my_rank == size - 1) ? 0 : my_rank + 1;
    left = (my_rank == 0) ? size - 1 : my_rank - 1;

    /* Implement ring addition code
     * do not use if (rank == 0) .. else ..
     * every rank sends initialy its rank number to a neighbor, and then sends what
     * it receives from that neighbor, this is done n times with n = number of processes
     * all ranks will obtain the sum.
     */
    sum = 0;
    snd_buf = my_rank;
    for(i = 0; i < size; i++){

        // MPI_Isend(&snd_buf,1,MPI_INT,right,0,MPI_COMM_WORLD, &request);
	if ((my_rank % 2) == 0) {
            MPI_Send(&snd_buf,1,MPI_INT,right,0,MPI_COMM_WORLD);
            MPI_Recv(&rcv_buf,1,MPI_INT,left,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	} else {
	    MPI_Recv(&rcv_buf,1,MPI_INT,left,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	    MPI_Send(&snd_buf,1,MPI_INT,right,0,MPI_COMM_WORLD);
	}
        // MPI_Wait(&request, &status);

        sum += rcv_buf;
        snd_buf = rcv_buf;

    }


    printf ("Process %i:\tSum = %i\n", my_rank, sum);

    MPI_Finalize();
    return 0;

//     // Initialize the MPI environment
//   MPI_Init(NULL, NULL);
//   // Find out rank, size
//   int world_rank;
//   MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
//   int world_size;
//   MPI_Comm_size(MPI_COMM_WORLD, &world_size);

//   int token;
//   // Receive from the lower process and send to the higher process. Take care
//   // of the special case when you are the first process to prevent deadlock.
//   if (world_rank != 0) {
//     MPI_Recv(&token, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD,
//              MPI_STATUS_IGNORE);
//     printf("Process %d received token %d from process %d\n", world_rank, token,
//            world_rank - 1);
//   } else {
//     // Set the token's value if you are process 0
//     token = -1;
//   }
//   MPI_Send(&token, 1, MPI_INT, (world_rank + 1) % world_size, 0,
//            MPI_COMM_WORLD);
//   // Now process 0 can receive from the last process. This makes sure that at
//   // least one MPI_Send is initialized before all MPI_Recvs (again, to prevent
//   // deadlock)
//   if (world_rank == 0) {
//     MPI_Recv(&token, 1, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD,
//              MPI_STATUS_IGNORE);
//     printf("Process %d received token %d from process %d\n", world_rank, token,
//            world_size - 1);
//   }
//   MPI_Finalize();


}
