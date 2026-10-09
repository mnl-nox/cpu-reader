#define _POSIX_C_SOURCE 200809L

#include "cpu.h"

#include <stdio.h>
#include <time.h>

static double elapsed_ns(struct timespec start, struct timespec end) {
  return (double)(end.tv_sec - start.tv_sec) * 1000000000.0 +
         (double)(end.tv_nsec - start.tv_nsec);
}

static int benchmark_info(unsigned long iterations) {
  struct timespec start, end;
  unsigned long index;
  unsigned long completed = 0;

  if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
    perror("clock_gettime");
    return -1;
  }
  for (index = 0; index < iterations; index++) {
    cpu_info_t *info = cpu_get_info();
    if (info == NULL) {
      fprintf(stderr, "cpu_get_info failed: %s\n", cpu_get_last_error());
      return -1;
    }
    cpu_free_info(info);
    completed++;
  }
  if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
    perror("clock_gettime");
    return -1;
  }
  printf("cpu_get_info: %lu iterations, %.0f ns/op\n", completed,
         elapsed_ns(start, end) / (double)completed);
  return 0;
}

static int benchmark_usage(unsigned long iterations) {
  cpu_usage_context_t context;
  struct timespec start, end;
  unsigned long index;
  unsigned long completed = 0;

  if (cpu_usage_context_init(&context) != 0 ||
      cpu_get_usage_context(&context) < 0.0f) {
    fprintf(stderr, "usage context init failed: %s\n", cpu_get_last_error());
    return -1;
  }
  if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
    perror("clock_gettime");
    return -1;
  }
  for (index = 0; index < iterations; index++) {
    if (cpu_get_usage_context(&context) < 0.0f) {
      fprintf(stderr, "cpu_get_usage_context failed: %s\n",
              cpu_get_last_error());
      return -1;
    }
    completed++;
  }
  if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
    perror("clock_gettime");
    return -1;
  }
  printf("cpu_get_usage_context: %lu iterations, %.0f ns/op\n", completed,
         elapsed_ns(start, end) / (double)completed);
  return 0;
}

int main(void) {
  const unsigned long iterations = 1000;

  printf("CPU Reader microbenchmark (informational; no pass/fail threshold)\n");
  if (benchmark_info(iterations) != 0 ||
      benchmark_usage(iterations * 10) != 0) {
    return 1;
  }
  return 0;
}
