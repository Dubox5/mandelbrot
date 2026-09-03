#include "mandelbrot.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void free_all(unsigned char *a, unsigned char *b, unsigned char *c, unsigned char *d) {
    free(a);
    free(b);
    free(c);
    free(d);
}

static const char *build_filename(char *buffer, size_t size, const char *suffix) {
    /* LOGIN e definido pelo Makefile como uma string literal, ex: -DLOGIN=\"mla\" */
    snprintf(buffer, size, "mandelbrot_%s_%s.pgm", LOGIN, suffix);
    return buffer;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr,
                "Erro: numero de argumentos invalido.\n"
                "Uso: %s <largura> <altura> <max_iteracoes> <num_threads>\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    int width, height, max_iter, num_threads;

    if (parse_positive_int(argv[1], &width) != 0) {
        fprintf(stderr, "Erro: largura invalida ('%s'). Informe um inteiro positivo.\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (parse_positive_int(argv[2], &height) != 0) {
        fprintf(stderr, "Erro: altura invalida ('%s'). Informe um inteiro positivo.\n", argv[2]);
        return EXIT_FAILURE;
    }
    if (parse_positive_int(argv[3], &max_iter) != 0) {
        fprintf(stderr, "Erro: numero maximo de iteracoes invalido ('%s'). Informe um inteiro positivo.\n", argv[3]);
        return EXIT_FAILURE;
    }
    if (parse_positive_int(argv[4], &num_threads) != 0) {
        fprintf(stderr, "Erro: numero de threads invalido ('%s'). Informe um inteiro positivo.\n", argv[4]);
        return EXIT_FAILURE;
    }

    size_t total_pixels = (size_t) width * (size_t) height;

    unsigned char *img_serial = malloc(total_pixels);
    unsigned char *img_openmp = malloc(total_pixels);
    unsigned char *img_pth1 = malloc(total_pixels);
    unsigned char *img_pth2 = malloc(total_pixels);

    if (img_serial == NULL || img_openmp == NULL || img_pth1 == NULL || img_pth2 == NULL) {
        fprintf(stderr, "Erro: falha na alocacao de memoria para as imagens.\n");
        free_all(img_serial, img_openmp, img_pth1, img_pth2);
        return EXIT_FAILURE;
    }

    mandel_params_t p_serial = {width, height, max_iter, num_threads, img_serial};
    mandel_params_t p_openmp = {width, height, max_iter, num_threads, img_openmp};
    mandel_params_t p_pth1   = {width, height, max_iter, num_threads, img_pth1};
    mandel_params_t p_pth2   = {width, height, max_iter, num_threads, img_pth2};

    double t_serial = 0.0, t_openmp = 0.0, t_pth1 = 0.0, t_pth2 = 0.0;

    if (run_serial(&p_serial, &t_serial) != 0) {
        fprintf(stderr, "Erro: falha na execucao da implementacao serial.\n");
        free_all(img_serial, img_openmp, img_pth1, img_pth2);
        return EXIT_FAILURE;
    }

    if (run_openmp(&p_openmp, &t_openmp) != 0) {
        fprintf(stderr, "Erro: falha na execucao da implementacao OpenMP.\n");
        free_all(img_serial, img_openmp, img_pth1, img_pth2);
        return EXIT_FAILURE;
    }

    if (run_pthreads1(&p_pth1, &t_pth1) != 0) {
        fprintf(stderr, "Erro: falha na execucao da implementacao Pthreads 1.\n");
        free_all(img_serial, img_openmp, img_pth1, img_pth2);
        return EXIT_FAILURE;
    }

    if (run_pthreads2(&p_pth2, &t_pth2) != 0) {
        fprintf(stderr, "Erro: falha na execucao da implementacao Pthreads 2.\n");
        free_all(img_serial, img_openmp, img_pth1, img_pth2);
        return EXIT_FAILURE;
    }

    char filename[256];

    if (write_pgm_values(build_filename(filename, sizeof(filename), "serial"), img_serial, width, height) != 0) {
        fprintf(stderr, "Erro: falha ao criar o arquivo de saida da implementacao serial.\n");
        free_all(img_serial, img_openmp, img_pth1, img_pth2);
        return EXIT_FAILURE;
    }

    if (write_pgm_values(build_filename(filename, sizeof(filename), "openmp"), img_openmp, width, height) != 0) {
        fprintf(stderr, "Erro: falha ao criar o arquivo de saida da implementacao OpenMP.\n");
        free_all(img_serial, img_openmp, img_pth1, img_pth2);
        return EXIT_FAILURE;
    }

    if (write_pgm_values(build_filename(filename, sizeof(filename), "pthreads1"), img_pth1, width, height) != 0) {
        fprintf(stderr, "Erro: falha ao criar o arquivo de saida da implementacao Pthreads 1.\n");
        free_all(img_serial, img_openmp, img_pth1, img_pth2);
        return EXIT_FAILURE;
    }

    if (write_pgm_values(build_filename(filename, sizeof(filename), "pthreads2"), img_pth2, width, height) != 0) {
        fprintf(stderr, "Erro: falha ao criar o arquivo de saida da implementacao Pthreads 2.\n");
        free_all(img_serial, img_openmp, img_pth1, img_pth2);
        return EXIT_FAILURE;
    }

    FILE *times = fopen("times.txt", "w");
    if (times == NULL) {
        fprintf(stderr, "Erro: nao foi possivel criar o arquivo times.txt.\n");
        free_all(img_serial, img_openmp, img_pth1, img_pth2);
        return EXIT_FAILURE;
    }

    fprintf(times, "Serial: %.6f segundos\n", t_serial);
    fprintf(times, "OpenMP: %.6f segundos\n", t_openmp);
    fprintf(times, "Pthreads1: %.6f segundos\n", t_pth1);
    fprintf(times, "Pthreads2: %.6f segundos\n", t_pth2);

    if (fclose(times) != 0) {
        fprintf(stderr, "Erro: falha ao finalizar a escrita do arquivo times.txt.\n");
        free_all(img_serial, img_openmp, img_pth1, img_pth2);
        return EXIT_FAILURE;
    }

    free_all(img_serial, img_openmp, img_pth1, img_pth2);
    return EXIT_SUCCESS;
}
