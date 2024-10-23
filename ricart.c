#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#define REQUEST 1
#define OK 2
#define RELEASE 3
#define ACK_RELEASE 4

int main(int argc, char **argv) {
    int rank, size;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) {
            printf("This program requires at least 2 MPI processes.\n");
        }
        MPI_Finalize();
        return 0;
    }

    int clock = 0;

    if (rank == 0) {
        srand(time(NULL) + rank);
        clock++;
        printf("Process %d requesting critical section\n", rank);
        for (int i = 1; i < size; i++) {
            int msg[2] = {REQUEST, clock};
            MPI_Send(msg, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
            printf("Process %d sent REQUEST to Process %d.\n", rank, i);
        }
        int ok_received = 0;
        while (ok_received < size - 1) {
            int msg[2];
            MPI_Recv(msg, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            int msg_type = msg[0];
            int msg_time = msg[1];
            int sender = status.MPI_SOURCE;

            // Update the clock
            if (clock < msg_time) {
                clock = msg_time;
            }
            clock++;

            if (msg_type == OK) {
                printf("Process %d received OK from Process %d\n", rank, sender);
                ok_received++;
            }
        }

        // Critical section
        printf("Process %d in critical section\n", rank);
        sleep(3);  // Simulating critical section work
        printf("Process %d releasing critical section\n", rank);
        for (int i = 1; i < size; i++) {
            clock++;
            int msg[2] = {RELEASE, clock};
            MPI_Send(msg, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
            printf("Process %d sent RELEASE to Process %d.\n", rank, i);
        }
    } 
    else {
        bool running = true;
        int release_count = 0;
        int total_releases = 1; // Should match the number of iterations in Process 0

        while (running) {
            int msg[2];
            MPI_Recv(msg, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            int msg_type = msg[0];
            int msg_time = msg[1];
            int sender = status.MPI_SOURCE;

            // Update clock
            if (clock < msg_time) {
                clock = msg_time;
            }
            clock++;

            if (msg_type == REQUEST) {
                printf("Process %d received REQUEST from Process %d\n", rank, sender);
                clock++;
                int reply[2] = {OK, clock};
                MPI_Send(reply, 2, MPI_INT, sender, 0, MPI_COMM_WORLD);
                printf("Process %d sent OK to Process %d\n", rank, sender);
            } 
            else if (msg_type == RELEASE) {
                printf("Process %d received RELEASE from Process %d\n", rank, sender);
                release_count++;
            }
        }
    }

    MPI_Finalize();
    return 0;
}
