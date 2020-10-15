#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMTHRDS 4
#define VECLEN 100000
pthread_t callThd[NUMTHRDS];
double *array_a;
double *array_b;
double big_sum;
int veclen;

//Creates mutex for use throughout the program functions
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *dotprod(void *arg)
{
    double *x, *y;
    x = array_a;
    y = array_b;
    int i, start = 0, end = veclen, mysum;

    mysum = 0;
    for (i=start; i<end ; i++)
    {
        mysum += (x[i] * y[i]);
    }
    //Since big_sum is a global variable,
    //big_sum needs to be protected protected by the mutex,
    //so no two threads access it at
    //the same time, hence there is no data loss.
    pthread_mutex_lock(&mutex);
    big_sum += mysum;
    pthread_mutex_unlock(&mutex);

    return NULL;
}
int main (int argc, char *argv[])
{
    long i;
    double *a, *b;
    void *status;
    a = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));
    b = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));

    for (i=0; i<VECLEN*NUMTHRDS; i++)
    {
        a[i]=1;
        b[i]=a[i];
    }
    veclen = VECLEN;
    array_a = a;
    array_b = b;
    big_sum = 0;

    pthread_mutex_init(&mutex, NULL);

    /* create threads */
    for(i=0;i<NUMTHRDS;i++)
    {
        /* Each thread works on a different set of data.
           The offset is specified by 'i'. The size of
           the data for each thread is indicated by VECLEN.
        */
        pthread_create(&callThd[i], NULL, dotprod, &i);
    }
    /* Wait on the other threads */
    for(i=0;i<NUMTHRDS;i++)
    {
        //Calls pthread_join with the threads created by the last loop
        pthread_join(callThd[i], NULL);
    }
    printf ("Sum = %f \n", big_sum);
    free (a);
    free (b);
    pthread_exit(NULL);
}