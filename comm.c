#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc,char **argv){
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int num;

    if(rank == 0){
        num = 100;
    }
    
    if(rank == 0){
        MPI_Send(&num,1,MPI_INT,1,0,MPI_COMM_WORLD);
        printf("Process 0 send 100 to process 1\n");
        MPI_Recv(&num,1,MPI_INT,size-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("Process 0 received %d from process %d\n",num,size-1);
    }
    else{
        MPI_Recv(&num,1,MPI_INT,rank-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("Process %d received %d from process %d\n",rank,num,rank-1);
        MPI_Send(&num,1,MPI_INT,(rank+1)%size,0,MPI_COMM_WORLD);
        printf("Process %d sent %d to process %d\n",rank,num,(rank+1)%size);
    }
    MPI_Finalize();
    return 0;
}