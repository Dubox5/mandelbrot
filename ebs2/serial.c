#include "mandelbrot.h"

#include <time.h>

int run_serial(mandel_params_t *p, double *elapsed_seconds) {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int py = 0; py < p->height; py++) {
        for (int px = 0; px < p->width; px++) {
            int iter = compute_pixel_iterations(px, py, p->width, p->height, p->max_iter);
            p->image[(size_t) py * (size_t) p->width + (size_t) px] =
                iterations_to_intensity(iter, p->max_iter);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    *elapsed_seconds = (double) (end.tv_sec - start.tv_sec) +
                       (double) (end.tv_nsec - start.tv_nsec) / 1e9;

    return 0;
}
