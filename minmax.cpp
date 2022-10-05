#include "mpi.h"
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <math.h>
#define ARRAY_SIZE 1000000

int main (int argc,  char *argv[]) {

   int myid, numprocs;
   int namelen;
   int local_min = 0;
   int local_max = 0;
   int* numbers = new int[ARRAY_SIZE];
   char processor_name[MPI_MAX_PROCESSOR_NAME];

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);
   MPI_Get_processor_name(processor_name, &namelen);
 
   printf("Process %d on %s\n", myid, processor_name);
 

   //Random Number Generation
   srand((unsigned)(myid)); 

   for (int i=0; i<ARRAY_SIZE; i++)
      numbers[i] = ((int)rand());  //create 1000000 arrays filled with random numbers

   int s = (int)floor(ARRAY_SIZE/numprocs);
   int s0 = s + ARRAY_SIZE%numprocs;

   int startIndex = s0 + (myid-1)*s;
   int endIndex = startIndex + s;

   double startwtime;
   if (myid == 0) {
      startwtime = MPI_Wtime();
   }

   int i;
   int part_sum = 0;
   int local_minimum = numbers[1];
   int local_maximum = numbers[1];

   if (myid == 0) {
      // master worker - comput the master's numbers
      for (i=0; i<s0; i++) 
      {
         if (numbers[i] < local_minimum)
         {
            local_minimum = numbers[i];
         }
         if (numbers[i] > local_minimum)
         {
            local_maximum = numbers[i];
         }
      }
      printf("Process %d - startIndex 0 endIndex %d; local_minimum %i; local_maximum %i\n",
             myid, s0-1, local_minimum, local_maximum);
   } else {
      //slave's work
      for (i= startIndex; i<endIndex; i++) 
      {
         if (numbers[i] < local_minimum)
         {
            local_minimum = numbers[i];
         }
         if (numbers[i] > local_minimum)
         {
            local_maximum = numbers[i];
         }
      }
      printf ("Process %d - startIndex %d endIndex %d; local_min %i: local_max %i\n",
              myid, startIndex, endIndex-1, local_minimum, local_maximum);
   }
   int min = 0;
   int max = 0;

   MPI_Reduce(&local_minimum, &min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
   MPI_Reduce(&local_maximum, &max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

   if (myid == 0) {
      double runTime = MPI_Wtime() - startwtime;
      printf("Execution time (sec) = %f min = %i; max = %i \n",
             runTime, min, max);
   }

   delete[] numbers;

   MPI_Finalize();
}

