#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define TAG 0

int main(int argc, char *argv[]) {
    int rank, size;
    char message[100];
    MPI_Status status;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Get number of processes and rank
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size < 2) {
        if(rank == 0) {
            printf("This program requires at least two processes.\n");
        }
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        // Process 0 sends a message to Process 1
        strcpy(message, "Hello from Process 0");
        MPI_Send(message, strlen(message)+1, MPI_CHAR, 1, TAG, MPI_COMM_WORLD);
        printf("Process 0 sent message: %s\n", message);

        // Receive a message back from Process 1
        MPI_Recv(message, 100, MPI_CHAR, 1, TAG, MPI_COMM_WORLD, &status);
        printf("Process 0 received message: %s\n", message);
    }
    else if (rank == 1) {
        // Receive message from Process 0
        MPI_Recv(message, 100, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
        printf("Process 1 received message: %s\n", message);

        // Send a response back to Process 0
        strcpy(message, "Hello back from Process 1");
        MPI_Send(message, strlen(message)+1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
        printf("Process 1 sent message: %s\n", message);
    }

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
