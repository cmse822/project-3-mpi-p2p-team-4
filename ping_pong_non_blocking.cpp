#include <mpi.h>
#include <fstream>
#include <iostream>


int main() {

    double startTime = MPI_Wtime();

    MPI_Init(NULL, NULL);

    // request object for non-blocking communication
    MPI_Request request_s0, request_r0, request_r1, request_s1;

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int maxIterations = 100;
    int messageSize = 2;
    char* message = new char[messageSize];

    for (int iteration=0; iteration < maxIterations; iteration++) {
    
        if (world_rank == 0) {

            //MPI Request Array for non-blocking communication
            MPI_Request req[2];
            
            // Ping
            MPI_Isend(&message, 1, MPI_UNSIGNED_CHAR, 1, 0, MPI_COMM_WORLD, &req[0]);

            MPI_Irecv(&message, 1, MPI_UNSIGNED_CHAR, 1, 1, MPI_COMM_WORLD,&req[1]);
            
            MPI_Waitall(2,req,MPI_STATUS_IGNORE);

            // printf("Process 0 Sent and Recieved Message %d \n", message);
            
        } 
        else if (world_rank == 1) {
            
            // Pong
            MPI_Request req[2];

            MPI_Irecv(&message, 1, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, &req[0]);

            MPI_Isend(&message, 1, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD,&req[1]);

            MPI_Waitall(2,req,MPI_STATUS_IGNORE);

           
            // printf("Process 1 Sent and Recieved Message %d \n", message);
        }
    }   
    
    MPI_Finalize();

    double endTime = MPI_Wtime();
    double totalTime = endTime - startTime;
    //Output results to csv file 

    if (world_rank == 0 ) {

        printf("\nTotal Time: %lf \n", totalTime);

        const char* csvfile = "non_blocking.csv";

        std::ifstream infile(csvfile);
         //check if file is empty
        bool is_empty = infile.peek() == std::ifstream::traits_type::eof();
        infile.close();

        // Open the CSV file for writing (appending mode)
        std::ofstream file(csvfile, std::ios_base::app); // app = append mode

        // Write header if the file is empty
        if (is_empty) {
            file << "message size, Iterations, Time" << std::endl;
        }

        // Write results to the CSV file
        if (file.is_open()) {
            file << messageSize << ',' << maxIterations << ',' << totalTime << std::endl;
            file.close();
        } else {
            std::cerr << "Error: Unable to open file for writing." << std::endl;
        }

    }

}

