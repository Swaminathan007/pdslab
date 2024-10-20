#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc,char **argv){
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    printf("Hello world from process %d\n",rank);
    return 0;
}