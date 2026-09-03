#include "mandelbrot.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int start_row;
    int end_row;
    int width;
    int height;
    int max_iter;
    unsigned char *image;
} thread_arg1_t;

static void *worker1(void *arg) {
    thread_arg1_t *targ = (thread_arg1_t *) arg;

    for (int py = targ->start_row; py < targ->end_row; py++) {
        for (int px = 0; px < targ->width; px++) {
            int iter = compute_pixel_iterations(px, py, targ->width, targ->height, targ->max_iter);
            targ->image[(size_t) py * (size_t) targ->width + (size_t) px] =
                iterations_to_intensity(iter, targ->max_iter);
        }
    }

    return NULL;
}

int run_pthreads1(mandel_params_t *p, double *elapsed_seconds) {
    int num_threads = p->num_threads;
    if (num_threads > p->height) {
        num_threads = p->height;
    }
    if (num_threads < 1) {
        num_threads = 1;
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * (size_t) num_threads);
    thread_arg1_t *args = malloc(sizeof(thread_arg1_t) * (size_t) num_threads);

    if (threads == NULL || args == NULL) {
        fprintf(stderr, "Falha na alocacao de memoria (pthreads1).\n");
        free(threads);
        free(args);
        return -1;
    }

    int rows_per_thread = p->height / num_threads;
    int remainder = p->height % num_threads;
    int current_row = 0;
    int created = 0;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int t = 0; t < num_threads; t++) {
        int rows_this = rows_per_thread + (t < remainder ? 1 : 0);

        args[t].start_row = current_row;
        args[t].end_row = current_row + rows_this;
        args[t].width = p->width;
        args[t].height = p->height;
        args[t].max_iter = p->max_iter;
        args[t].image = p->image;

        current_row += rows_this;

        int rc = pthread_create(&threads[t], NULL, worker1, &args[t]);
        if (rc != 0) {
            fprintf(stderr, "Falha ao criar thread %d (pthreads1).\n", t);
            for (int j = 0; j < created; j++) {
                pthread_join(threads[j], NULL);
            }
            free(threads);
            free(args);
            return -1;
        }
        created++;
    }

    for (int t = 0; t < created; t++) {
        pthread_join(threads[t], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    *elapsed_seconds = (double) (end.tv_sec - start.tv_sec) +
                       (double) (end.tv_nsec - start.tv_nsec) / 1e9;

    free(threads);
    free(args);
    return 0;
}
