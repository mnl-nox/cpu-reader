#define _POSIX_C_SOURCE 200809L
#include "cpu.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define THREAD_COUNT 6
#define ITERATIONS 80

typedef struct {
  int failed;
  int thread_index;
} worker_result_t;

static void *worker(void *opaque) {
  worker_result_t *result = (worker_result_t *)opaque;
  cpu_usage_context_t context;
  int index;
  if (cpu_usage_context_init(&context) != 0) {
    result->failed = 1;
    return NULL;
  }
  for (index = 0; index < ITERATIONS; ++index) {
    float usage = cpu_get_usage_context(&context);
    if (usage < 0.0f || usage > 100.0f) {
      result->failed = 1;
      break;
    }
    if (index % 10 == 0) {
      cpu_error_info_t error;
      cpu_info_t *info = cpu_get_info_ex(&error);
      if (info == NULL || error.code != CPU_ERROR_NONE) {
        cpu_free_info(info);
        result->failed = 1;
        break;
      }
      cpu_free_info(info);
    }
  }
  if (result->thread_index == 0) {
    cpu_error_info_t error;
    if (cpu_get_usage_context_ex(NULL, &error) != -1.0f ||
        error.code != CPU_ERROR_INVALID_ARGUMENT ||
        cpu_get_last_error_code() != CPU_ERROR_INVALID_ARGUMENT) {
      result->failed = 1;
    }
  } else if (cpu_get_last_error_code() != CPU_ERROR_NONE) {
    result->failed = 1;
  }
  cpu_usage_context_cleanup(&context);
  return NULL;
}

int main(void) {
  pthread_t threads[THREAD_COUNT];
  worker_result_t results[THREAD_COUNT];
  int index;
  for (index = 0; index < THREAD_COUNT; ++index) {
    results[index].failed = 0;
    results[index].thread_index = index;
    if (pthread_create(&threads[index], NULL, worker, &results[index]) != 0) {
      fprintf(stderr, "pthread_create failed at worker %d\n", index);
      return 1;
    }
  }
  for (index = 0; index < THREAD_COUNT; ++index) {
    if (pthread_join(threads[index], NULL) != 0 || results[index].failed) {
      fprintf(stderr, "concurrency contract failed at worker %d\n", index);
      return 1;
    }
  }
  puts("concurrency contract tests passed");
  return 0;
}
