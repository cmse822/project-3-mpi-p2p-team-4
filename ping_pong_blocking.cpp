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
            
            // Ping
            MPI_Send(&message, 1, MPI_UNSIGNED_CHAR, 1, 0, MPI_COMM_WORLD);
            printf("Process 0 Sent Message %d To Process 1\n", message);

            // 2nd Pong
            MPI_Recv(&message, 1, MPI_UNSIGNED_CHAR, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process 0 Received Message %d From Process 1\n", message);
        } 
        else if (world_rank == 1) {
            
            // Pong
            MPI_Recv(&message, 1, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process 1 Received Message %d From Process 0\n", message);

            // 2nd Ping
            MPI_Send(&message, 1, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD);

            printf("Process 1 Sent Message %d To Process 0\n", message);
        }
    }   
    
    MPI_Finalize();

    double endTime = MPI_Wtime();
    double totalTime = endTime - startTime;

    if (world_rank == 0) {
        printf("\nTotal Time: %lf \n", totalTime);
    }

}