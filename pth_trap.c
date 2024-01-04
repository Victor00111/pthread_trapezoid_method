/* File: pth_trap.c
 * Purpose: Calculate definite integral using trapezoidal
 * rule and PThreads.
 *
 * Input: a, b, n
 * Output: Estimate of integral from a to b of f(x)
 * using n trapezoids.
 *
 * Compile: gcc -g -o trap pth_trap.c -lpthread
 * Usage: ./trap
 *
 * Note: The function f(x) is hardwired.
 *
 * IPP2: Section 3.2.1 (pp. 101 and ff.) and 5.2 (p. 228)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

void *Trap(void *rank);
double f(double x);

int thread_count;
pthread_mutex_t mutex;
double h;
double a, b; /* Left and right endpoints */
int n;       /* Number of trapezoids */
double integral;

int main(int argc, char **argv) {
    thread_count = strtol(argv[1], NULL, 10);   
    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);
    
    printf("Enter a, b, n\n");
    scanf("%lf %lf %d", &a, &b, &n);
    h = (b - a) / n;

    double start, finish;
    GET_TIME(start);
    for (long i = 0; i < thread_count; i++) {
        pthread_create(&thread_handles[i], NULL, Trap, (void *)i);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(thread_handles[i], NULL);
    }
    GET_TIME(finish);

    printf("With n = %d trapezoids, our estimate\n", n);
    printf("of the integral from %f to %f = %.15f\n", a, b, integral);     
    printf("Time elasped: %f\n", finish - start);
    
    free(thread_handles);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *Trap(void *rank) {
    double local_sum;
    double x;
    long my_rank = (long)rank;

    if (my_rank == 0) {
        local_sum += (f(a)+f(b)) / 2.0;
    }
    
    int start = my_rank * n / thread_count;
    int end = ((my_rank + 1) * n / thread_count) - 1;

    for (int i = start; i <= end; i++) {
        x = a +  i * h;
        local_sum += f(x);
    }
    local_sum *= h;

    pthread_mutex_lock(&mutex);
    integral += local_sum;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

double f(double x) {
    double return_val;

    return_val = x * x;
    return return_val;
} /* f */
