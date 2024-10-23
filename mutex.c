#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int token;

    // If process is rank 0, it sends a token to process 1 to start the critical section.
    if (rank == 0) {
        printf("Process 0 initializing token.\n");
        token = 1;
        // Send token to the next process (process 1)
        MPI_Send(&token, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }

    // All processes except process 0 will wait to receive the token
    if (rank != 0) {
        // Receive the token from the previous process
        MPI_Recv(&token, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Entering critical section
    printf("Process %d Entering critical section\n", rank);
    sleep(1);  // Simulate work inside critical section
    printf("Process %d Exiting critical section\n", rank);

    // Pass the token to the next process, unless it is the last process
    if (rank < size - 1) {
        MPI_Send(&token, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }

    // Finalize MPI environment
    MPI_Finalize();
    return 0;
}
