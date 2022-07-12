#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include "stdlib.h"
#include"time.h"
#include <stddef.h>
#include "Source.h"

static inline 
void swap(int* v, int i, int j)
{
    int t = v[i];
    v[i] = v[j];
    v[j] = t;
}

void bubblesort(int* v, int n)
{
    int i, j;
    for (i = n - 2; i >= 0; i--)
        for (j = 0; j <= i; j++)
            if (v[j] > v[j + 1])
                swap(v, j, j + 1);
}

int* merge(int* v1, int n1, int* v2, int n2)
{
    int* result = (int*)malloc((n1 + n2) * sizeof(int));
    int i = 0;
    int j = 0;
    int k;
    for (k = 0; k < n1 + n2; k++) {
        if (i >= n1) {
            result[k] = v2[j];
            j++;
        }
        else if (j >= n2) {
            result[k] = v1[i];
            i++;
        }
        else if (v1[i] < v2[j]) { // i < n1 && j < n2
            result[k] = v1[i];
            i++;
        }
        else { // v2[j] <= v1[i]
            result[k] = v2[j];
            j++;
        }
    }
    return result;
}



int main(int argc, char* argv[])
{
    while (true)
    {
        printf("Enter array size: ");
        int size;
        scanf_s("%d", &size);
        int* Array = (int*)malloc(size * sizeof(int));
        time_t start, end;
        int rnd;

        //printf("Start Array: ");

        for (int i = 0; i < size; i++) {
            rnd = rand() % 100 + 1;
            Array[i] = rnd;
            //printf("%d ", rnd);
        }

        //start = clock();

        /*for (int i = 0; i < size; i++)
        {
            for (int j = size - 1; j > i; j--)
            {
                if (Array[j] <= Array[j - 1])
                {
                    int z = Array[j - 1];
                    Array[j - 1] = Array[j];
                    Array[j] = z;

                }
            }
        }
        
         printf("\nSorted Array: ");

        for (int i = 0; i < size; i++) {
            printf("%d ", Array[i]);
        }*/



        argc = 2;
       
        int n = size, c, s, * chunk, o, * other, step, p, id, i;
        double elapsed_time;
        long spend_time;
        MPI_Status status;

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &p);
        MPI_Comm_rank(MPI_COMM_WORLD, &id);

        MPI_Barrier(MPI_COMM_WORLD);
        start = clock();

        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

        c = n / p; if (n % p) c++;

        chunk = (int*)malloc(c * sizeof(int));
        MPI_Scatter(Array, c, MPI_INT, chunk, c, MPI_INT, 0, MPI_COMM_WORLD);
        free(Array);
        Array = NULL;

        s = (n >= c * (id + 1)) ? c : n - c * id;
        bubblesort(chunk, s);

        for (step = 1; step < p; step = 2 * step) {
            if (id % (2 * step) != 0) {
                MPI_Send(chunk, s, MPI_INT, id - step, 0, MPI_COMM_WORLD);
                break;
            }
            if (id + step < p) {
                o = (n >= c * (id + 2 * step)) ? c * step : n - c * (id + step);
                other = (int*)malloc(o * sizeof(int));
                MPI_Recv(other, o, MPI_INT, id + step, 0, MPI_COMM_WORLD, &status);
                Array = merge(chunk, s, other, o);

                free(chunk);
                free(other);
                chunk = Array;
                s = s + o;
            }
        }
        end = clock();

       /* printf("\nSorted Array: ");
        for (i = 0; i < s; i++)
            printf("%d ", chunk[i]);*/

        MPI_Finalize();       

        printf("\nTime used: %ld sec\n\n", (end - start) / CLOCKS_PER_SEC);

    }
}
