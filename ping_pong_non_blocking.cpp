#include <mpi.h>
#include <fstream>
#include <iostream>


int main(int argc, char *argv[]) {

    double startTime = MPI_Wtime();

    MPI_Init(&argc, &argv);

    // request object for non-blocking communication
    MPI_Request request_s0, request_r0, request_r1, request_s1;

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int maxIterations = 100;
    // int messageSize = 4073741824;

    // get cmd args 

    int messageSize = atoi(argv[1]);
    const char* csvfile = argv[2];


    printf("message size %lld \n", messageSize);
    printf("csvfile %s \n", csvfile);
    
    // char* message = new char[messageSize];
    char* message = (char*)malloc(messageSize);
    char* recbuf = (char*)malloc(messageSize);

    if (message == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        MPI_Abort(MPI_COMM_WORLD, 1); // Abort MPI execution
    }

    for (int iteration=0; iteration < maxIterations; iteration++) {
    
        if (world_rank == 0) {

            //MPI Request Array for non-blocking communication
            MPI_Request req[2];
            
            // Ping
            MPI_Isend(message, messageSize, MPI_UNSIGNED_CHAR, 1, 0, MPI_COMM_WORLD, &req[0]);

            MPI_Irecv(recbuf, messageSize, MPI_UNSIGNED_CHAR, 1, 1, MPI_COMM_WORLD,&req[1]);
            
            MPI_Waitall(2,req,MPI_STATUS_IGNORE);

            // printf("Process 0 Sent and Recieved Message %d \n", message);
            
        } 
        else if (world_rank == 1) {
            
            // Pong
            MPI_Request req[2];

            MPI_Irecv(recbuf, messageSize, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, &req[0]);

            MPI_Isend(message, messageSize, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD,&req[1]);

            MPI_Waitall(2,req,MPI_STATUS_IGNORE);

           
            // printf("Process 1 Sent and Recieved Message %d \n", message);
        }
    }   
    
    
    MPI_Finalize();
    
    free(message);
    free(recbuf);

    double endTime = MPI_Wtime();
    double totalTime = endTime - startTime;
    //Output results to csv file 

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

