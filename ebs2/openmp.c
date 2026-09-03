#include "mandelbrot.h"

#include <omp.h>

int run_openmp(mandel_params_t *p, double *elapsed_seconds) {
    int width = p->width;
    int height = p->height;
    int max_iter = p->max_iter;
    unsigned char *image = p->image;

    double start = omp_get_wtime();

    #pragma omp parallel for num_threads(p->num_threads) schedule(dynamic, 1)
    for (int py = 0; py < height; py++) {
        for (int px = 0; px < width; px++) {
            int iter = compute_pixel_iterations(px, py, width, height, max_iter);
            image[(size_t) py * (size_t) width + (size_t) px] =
                iterations_to_intensity(iter, max_iter);
        }
    }

    double end = omp_get_wtime();
    *elapsed_seconds = end - start;

    return 0;
}
