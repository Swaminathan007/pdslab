#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int hr, min;
    int average_hr, average_min;

    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Ensure that exactly 3 processes are running
    if (size != 3) {
        if (rank == 0) {
            fprintf(stderr, "Error: This program requires exactly 3 MPI processes.\n");
        }
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Assign hr and min based on the process rank
    if (rank == 0) {
        hr = 3;
        min = 0;
    } else if (rank == 1) {
        hr = 2;
        min = 45;
    } else if (rank == 2) {
        hr = 3;
        min = 15;
    }

    // Each process prepares its time data
    int time[2];
    time[0] = hr;
    time[1] = min;

    // Root process will gather all time data
    int all_times[3][2]; // Assuming 3 processes

    // Gather time data from all processes to root (Process 0)
    MPI_Gather(time, 2, MPI_INT, all_times, 2, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Compute total minutes from all processes
        int total_minutes = 0;
        for (int i = 0; i < size; i++) {
            int proc_hr = all_times[i][0];
            int proc_min = all_times[i][1];
            total_minutes += proc_hr * 60 + proc_min;
            printf("Process %d: %d hr %d min\n", i, proc_hr, proc_min);
        }

        // Compute average minutes
        int avg_minutes_total = total_minutes / size;

        // Convert back to hours and minutes
        average_hr = avg_minutes_total / 60;
        average_min = avg_minutes_total % 60;

        printf("Average Time: %d hr %d min\n", average_hr, average_min);
    }

    // Broadcast the average time from root to all processes
    int avg_time[2];
    if (rank == 0) {
        avg_time[0] = average_hr;
        avg_time[1] = average_min;
    }

    MPI_Bcast(avg_time, 2, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process stores the average time
    average_hr = avg_time[0];
    average_min = avg_time[1];

    // Each process prints the received average time
    printf("Process %d received Average Time: %d hr %d min\n", rank, average_hr, average_min);

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}
