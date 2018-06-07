/* C Example */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1E7
#define d 1E-7
#define d2 1E-14

int main (int argc, char* argv[])
{
    
    int rank, np, part;
    int star_end[2];
    
    MPI_Status status;
    MPI_Init (&argc, &argv);      /* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &np);        /* get number of processes */
   
    double tstart = MPI_Wtime();
    
    //int lenght = N / (np-1);
    //int mod = (int)N % (np-1);
    
    int inizio = 0;
    int fine = 0;
    
    if ( rank == 0) {
        int lenght = N / (np-1);
        int mod = (int)N % (np-1);
        double result = 0.0, result_slave = 0.0, pi = 0.0;
        
        for (int i = 1; i < np; i++) {
            MPI_Send(&inizio, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            int res = (i <= mod)?lenght+1:lenght;
            fine = inizio + res;
            MPI_Send(&fine, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            inizio = inizio + res;
        }

        for (int i = 1; i < np; i++) {
            MPI_Recv(&result_slave, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
            result += result_slave;
        }
        
        pi = 4 * d * result;
        printf("pi=%lf\n", pi);
        
        double tend = MPI_Wtime();
        printf("tempo di esecuzione = %lf\n", tend-tstart);
        
    } else {
        
        double x2=0.0, res=0.0;
        
        MPI_Recv(&inizio, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&fine, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        for (int j = inizio; j < fine; j++) {
            x2=d2*j*j;
            res+=1.0/(1.0+x2);
        }

        MPI_Send(&res, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}

