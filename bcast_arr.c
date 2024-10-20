#include<stdio.h>
#include<mpi.h>

int main(int argc , char **argv){
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    int arr[10];
    if(rank == 0){
        for(int i=0;i<10;i++){
            arr[i] = i+7;
        }
    }
    MPI_Bcast(&arr,10,MPI_INT,0,MPI_COMM_WORLD);
    if(rank!=0){
        printf("Array received from process 0:  {");
        for(int i=0;i<10;i++){
            printf("%d,",*(arr+i));
        }
        printf("}\n");
    }
    MPI_Finalize();
    return 0;
}