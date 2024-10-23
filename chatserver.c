#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define ROOT 0
#define MESSAGE_SIZE 100

int main(int argc, char *argv[]) {
    int rank, size;
    char message[MESSAGE_SIZE];
    MPI_Status status;

    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank and size
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Check if we have at least 4 processes
    if (size < 4) {
        if (rank == ROOT) {
            printf("This program requires at least 4 processes.\n");
        }
        MPI_Finalize();
        return -1;
    }

    if (rank == ROOT) {
        // Root process: broadcast a message to all other processes
        strcpy(message, "Hello from root!");
        printf("Process %d broadcasting message: %s\n", rank, message);
        MPI_Bcast(message, MESSAGE_SIZE, MPI_CHAR, ROOT, MPI_COMM_WORLD);

        // Receive responses from other processes
        for (int i = 1; i < size; i++) {
            MPI_Recv(message, MESSAGE_SIZE, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            printf("Root process received message: %s\n", message);
        }
    } 
    else {
        // Other processes: receive the broadcast from the root
        MPI_Bcast(message, MESSAGE_SIZE, MPI_CHAR, ROOT, MPI_COMM_WORLD);
        printf("Process %d received broadcast message: %s\n", rank, message);

        // Send a response back to the root
        sprintf(message, "Hello from process %d", rank);
        MPI_Send(message, MESSAGE_SIZE, MPI_CHAR, ROOT, 0, MPI_COMM_WORLD);

        // Optional: Communicate between non-root processes
        if (rank == 1) {
            // Process 1 sends a message to process 2
            strcpy(message, "Message from process 1 to process 2");
            MPI_Send(message, MESSAGE_SIZE, MPI_CHAR, 2, 0, MPI_COMM_WORLD);
        }
        if (rank == 2) {
            // Process 2 receives the message from process 1
            MPI_Recv(message, MESSAGE_SIZE, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status);
            printf("Process 2 received message: %s\n", message);
        }
    }

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}