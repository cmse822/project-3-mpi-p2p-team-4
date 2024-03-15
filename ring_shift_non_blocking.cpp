#include <fstream>
#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s messageSize\n", argv[0]);
        return 1;
    }

    int messageSize = atoi(argv[1]);

    printf("message size %lld \n", messageSize);
    
    if (messageSize <= 0) {
        fprintf(stderr, "messageSize must be a positive integer\n");
        return 1;
    }

    double startTime = MPI_Wtime();

    MPI_Init(&argc, &argv);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int maxIterations = 1;

    char* message = (char*)malloc(messageSize);
    char* recbuf = (char*)malloc(messageSize);

    const char* csvfile = argv[2];

    for (int iteration=0; iteration < maxIterations; iteration++) {
        if (world_rank == 0) {
            // Rank 0 sends first and then receives
            MPI_Request req[2];

            // Send message to the next rank (1)
            MPI_Isend(message, messageSize, MPI_UNSIGNED_CHAR, world_rank + 1, 0, MPI_COMM_WORLD, &req[0]);
            printf("Process 0 Sent Message %p To Process 1\n", message);

            // Receives message from the last rank, completing the ring
            MPI_Irecv(recbuf, messageSize, MPI_UNSIGNED_CHAR, world_size - 1, 0, MPI_COMM_WORLD, &req[1]);
            printf("Process 0 Received Message %d From Process %d\n", message, world_size-1);

            MPI_Waitall(2, req, MPI_STATUS_IGNORE);
        } else {
            // All the other ranks receive first and then send
            MPI_Request req[2];

            // Receieves message from the previous rank
            MPI_Irecv(recbuf, messageSize, MPI_UNSIGNED_CHAR, world_rank - 1, 0, MPI_COMM_WORLD, &req[0]);
            printf("Process %d Received Message %d From Process %d\n", world_rank, message, world_rank - 1);

            if (world_rank == world_size - 1) {
                // Final rank should send message to rank 0, completing the ring
                MPI_Isend(message, messageSize, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, &req[1]);
                printf("Process %d Sent Message %d To Process %d\n", world_rank, message, 0);
            } else {
                // All the other ranks send message to the next rank 
                MPI_Isend(message, messageSize, MPI_UNSIGNED_CHAR, world_rank + 1, 0, MPI_COMM_WORLD, &req[1]);
                printf("Process %d Sent Message %d To Process %d\n", world_rank, message, world_rank + 1);
            }

            MPI_Waitall(2, req, MPI_STATUS_IGNORE);
        }
    }   
    
    MPI_Finalize();

    double endTime = MPI_Wtime();
    double totalTime = endTime - startTime;

    if (world_rank == 0 ) {
        printf("\nTotal Time: %lf \n", totalTime);

        // const char* csvfile = "non_blocking.csv";

        std::ifstream infile(csvfile);
         //check if file is empty
        bool is_empty = infile.peek() == std::ifstream::traits_type::eof();
        infile.close();

        // Open the CSV file for writing (appending mode)
        std::ofstream file(csvfile, std::ios_base::app); // app = append mode

        // Write header if the file is empty
        if (is_empty) {
            file << "processors, message size, Time" << std::endl;
        }

        // Write results to the CSV file
        if (file.is_open()) {
            file << world_size << ',' << messageSize << ',' << totalTime << std::endl;
            file.close();
        } else {
            std::cerr << "Error: Unable to open file for writing." << std::endl;
        }
    }
}