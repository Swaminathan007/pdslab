#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>

int main(int argc,char **argv){
	int rank,size;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	printf("Hello world from process %d out of %d processes\n",rank,size);
	MPI_Finalize();
	return 0;
}
