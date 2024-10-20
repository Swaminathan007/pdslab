#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc,char **argv){
    int rank,size;
    MPI_Init(&argc,&argv);

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int num;

    if(rank == 0){
        num = 150;
        printf("Process 0 broadcasting...\n");
    }
    MPI_Bcast(&num,1,MPI_INT,0,MPI_COMM_WORLD);
    if(rank!=0){
        printf("Process %d has num = %d\n",rank,num);

    }
    MPI_Finalize();
}