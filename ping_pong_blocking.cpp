#include <mpi.h>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s messageSize\n", argv[0]);
        return 1;
    }

    int messageSize = atoi(argv[1]);
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

    int maxIterations = 100;
    char* message = new char[messageSize];

    const char* csvfile = argv[2];

    // printf("message size %d \n", messageSize);
    // printf("csvfile %s \n", csvfile);

    for (int iteration=0; iteration < maxIterations; iteration++) {
    
        if (world_rank == 0) {
            
            // Ping
            MPI_Send(message, messageSize, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            // printf("Process 0 Sent Message To Process 1\n");

            // 2nd Pong
            MPI_Recv(message, messageSize, MPI_CHAR, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("Process 0 Received Message From Process 1\n");
        } 
        else if (world_rank == 1) {
            
            // Pong
            MPI_Recv(message, messageSize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("Process 1 Received Message From Process 0\n");

            // 2nd Ping
            MPI_Send(message, messageSize, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
            // printf("Process 1 Sent Message To Process 0\n");
        }
    }   
    
    MPI_Finalize();

    delete[] message; // Free the allocated memory

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
    return 0;

}
