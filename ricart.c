#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

// Define message types
#define REQUEST 1
#define OK 2
#define RELEASE 3
#define ACK_RELEASE 4

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Status status;

    // Initialize MPI environment
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Ensure there are at least 2 processes (Process 0 and at least one responder)
    if (size < 2) {
        if (rank == 0) {
            printf("This program requires at least 2 MPI processes.\n");
        }
        MPI_Finalize();
        return 0;
    }

    // Initialize logical clock
    int clock = 0;

    if (rank == 0) {
        // Seed random number generator
        srand(time(NULL) + rank);

        // Number of times Process 0 will request the critical section
        int iterations = 1; // Set to 1 as per user request

        for (int iter = 0; iter < iterations; iter++) {
            // Simulate non-critical work
            int non_critical_work = rand() % 1000000; // up to 1 second
            printf("Process %d performing non-critical work for %d microseconds.\n", rank, non_critical_work);
            usleep(non_critical_work);

            // Request to enter critical section
            clock++;
            printf("Process %d requesting critical section\n", rank);

            // Send REQUEST to all other processes
            for (int i = 1; i < size; i++) {
                int msg[2] = {REQUEST, clock};
                MPI_Send(msg, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
                printf("Process %d sent REQUEST to Process %d.\n", rank, i);
            }

            // Receive OK from all other processes
            int ok_received = 0;
            while (ok_received < size - 1) {
                int msg[2];
                MPI_Recv(msg, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                int msg_type = msg[0];
                int msg_time = msg[1];
                int sender = status.MPI_SOURCE;

                // Update logical clock
                if (clock < msg_time) {
                    clock = msg_time;
                }
                clock++;

                if (msg_type == OK) {
                    printf("Process %d received OK from Process %d\n", rank, sender);
                    ok_received++;
                }
            }

            // Enter critical section
            printf("Process %d in critical section\n", rank);
            // Simulate critical section work for 3 seconds
            sleep(3);
            printf("Process %d releasing critical section\n", rank);

            // Send RELEASE to all other processes
            for (int i = 1; i < size; i++) {
                int msg[2] = {RELEASE, clock};
                MPI_Send(msg, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
                printf("Process %d sent RELEASE to Process %d.\n", rank, i);
            }

            // Receive ACK_RELEASE from all other processes
            int ack_received = 0;
            while (ack_received < size - 1) {
                int ack_msg[2];
                MPI_Recv(ack_msg, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                int ack_type = ack_msg[0];
                int ack_time = ack_msg[1];
                int ack_sender = status.MPI_SOURCE;

                // Update logical clock
                if (clock < ack_time) {
                    clock = ack_time;
                }
                clock++;

                if (ack_type == ACK_RELEASE) {
                    printf("Process %d received ACK_RELEASE from Process %d\n", rank, ack_sender);
                    ack_received++;
                }
            }

            printf("Process %d completed iteration %d.\n", rank, iter + 1);
        }
    }
    else {
        // Responder processes
        bool running = true;
        int release_count = 0;
        int total_releases = 1; // Should match the number of iterations in Process 0

        while (running) {
            int msg[2];
            MPI_Recv(msg, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            int msg_type = msg[0];
            int msg_time = msg[1];
            int sender = status.MPI_SOURCE;

            // Update logical clock
            if (clock < msg_time) {
                clock = msg_time;
            }
            clock++;

            if (msg_type == REQUEST) {
                printf("Process %d received REQUEST from Process %d\n", rank, sender);
                // Send OK to Process 0
                clock++;
                int reply[2] = {OK, clock};
                MPI_Send(reply, 2, MPI_INT, sender, 0, MPI_COMM_WORLD);
                printf("Process %d sent OK to Process %d\n", rank, sender);
            }
            else if (msg_type == RELEASE) {
                printf("Process %d received RELEASE from Process %d\n", rank, sender);
                release_count++;

                // Send ACK_RELEASE to Process 0
                clock++;
                int ack_msg[2] = {ACK_RELEASE, clock};
                MPI_Send(ack_msg, 2, MPI_INT, sender, 0, MPI_COMM_WORLD);
                printf("Process %d sent ACK_RELEASE to Process %d\n", rank, sender);

                // Terminate after receiving all RELEASE messages
                if (release_count >= total_releases) {
                    running = false;
                }
            }
        }
    }

    // Finalize MPI environment
    MPI_Finalize();
    return 0;
}
