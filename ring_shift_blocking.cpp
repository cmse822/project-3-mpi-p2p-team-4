#include <stdio.h>
#include <mpi.h>

int main() {

    double startTime = MPI_Wtime();

    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int maxIterations = 100;
    int messageSize = 2;
    char* message = new char[messageSize];

    for (int iteration=0; iteration < maxIterations; iteration++) {
    
        if (world_rank == 0) {
            // Rank 0 sends first and then receives
            MPI_Send(&message, messageSize, MPI_UNSIGNED_CHAR, world_rank + 1, 0, MPI_COMM_WORLD);
            printf("Process 0 Sent Message %p To Process 1\n", message);

            MPI_Recv(&message, 1, MPI_UNSIGNED_CHAR, world_size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process 0 Received Message %d From Process %d\n", message, world_size-1);
        } 
        else {
            // All the other ranks receive first and then send
            MPI_Recv(&message, 1, MPI_UNSIGNED_CHAR, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process %d Received Message %d From Process %d\n", world_rank, message, world_rank - 1);

            MPI_Send(&message, 1, MPI_UNSIGNED_CHAR, world_rank + 1, 1, MPI_COMM_WORLD);
            printf("Process %d Sent Message %d To Process %d\n", world_rank, message, world_rank + 1);
        }
    }   
    
    MPI_Finalize();

    double endTime = MPI_Wtime();
    double totalTime = endTime - startTime;

    if (world_rank == 0) {
        printf("\nTotal Time: %d \n", totalTime);
    }

}