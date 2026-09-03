#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <stddef.h>

#ifndef LOGIN
#define LOGIN "login"
#endif

#define REAL_MIN (-2.0)
#define REAL_MAX  (1.0)
#define IMAG_MIN (-1.5)
#define IMAG_MAX  (1.5)

typedef struct {
    int width;
    int height;
    int max_iter;
    int num_threads;
    unsigned char *image;
} mandel_params_t;

int compute_pixel_iterations(int px, int py, int width, int height, int max_iter);
unsigned char iterations_to_intensity(int iterations, int max_iter);
int write_pgm_values(const char *filename, const unsigned char *image, int width, int height);
int parse_positive_int(const char *str, int *out);

int run_serial(mandel_params_t *p, double *elapsed_seconds);
int run_openmp(mandel_params_t *p, double *elapsed_seconds);
int run_pthreads1(mandel_params_t *p, double *elapsed_seconds);
int run_pthreads2(mandel_params_t *p, double *elapsed_seconds);

#endif
