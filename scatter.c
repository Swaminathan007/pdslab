#include<stdio.h>
#include<mpi.h>

int main(int argc,char **argv){
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int arr[size],num;
    if(rank == 0){
        for(int i=0;i<size;i++){
            arr[i] = i*8;
        }
    }
    MPI_Scatter(arr,1,MPI_INT,&num,1,MPI_INT,0,MPI_COMM_WORLD);
    
    printf("Process %d received %d\n",rank,num);
    MPI_Finalize();
    return 0;
} 