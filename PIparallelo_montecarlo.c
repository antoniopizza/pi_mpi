#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#define SEED 35791246

int main(int argc, char** argv)
{
    int rank, np, i, count=0, res, count_slave=0;
    int niter = atoi(argv[1]);
    double x, y, z, pi;
    
    if (argc < 1) {
        printf("specificare il numero di iterazioni");
        return(0);
    }
    
    MPI_Status status;
    MPI_Init (&argc, &argv);      /* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &np);        /* get number of processes */
    
    double tstart = MPI_Wtime();
    
    //srand(SEED);
    int lenght = niter / (np-1);
    int mod = niter % (np-1);
    
    if (rank == 0) {
        for (int i = 1; i < np; i++) {
            res = (i<=mod)?lenght+1:lenght;
            printf("slave%d -- res%d\n", i, res);
            MPI_Send(&res, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        
        for (int i = 1; i < np; i++) {
            MPI_Recv(&count_slave, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            count+=count_slave;
        }
        
        pi=(double)count/niter*4;
        
        double tend = MPI_Wtime();
        printf("tempo di esecuzione = %lf\n", tend-tstart);
        
        printf("# of trials= %d , estimate of pi is %g \n",niter,pi);
    } else {
        
            srand(SEED);
            MPI_Recv(&res, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        
            for (int j = 0; j < res; j++) {
                x = (double)rand()/RAND_MAX;
                y = (double)rand()/RAND_MAX;
                z = x*x+y*y;
                if (z<=1) count++;
            }
        
            MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}
