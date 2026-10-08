#define _POSIX_C_SOURCE 200809L

#include "cpu.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int write_text(const char *path, const char *value) {
  FILE *file = fopen(path, "w");
  int result;

  if (file == NULL) {
    return -1;
  }
  result = fputs(value, file) == EOF ? -1 : 0;
  if (fclose(file) != 0) {
    result = -1;
  }
  return result;
}

static int make_zone(const char *root, const char *zone, const char *type,
                     const char *temperature) {
  char directory[512];
  char path[512];

  if (snprintf(directory, sizeof(directory), "%s/%s", root, zone) >=
          (int)sizeof(directory) ||
      mkdir(directory, 0700) != 0) {
    return -1;
  }
  if (snprintf(path, sizeof(path), "%s/type", directory) >= (int)sizeof(path) ||
      write_text(path, type) != 0 ||
      snprintf(path, sizeof(path), "%s/temp", directory) >= (int)sizeof(path) ||
      write_text(path, temperature) != 0) {
    return -1;
  }
  return 0;
}

static int test_thermal_sensor_preference(void) {
  char root[] = "/tmp/cpu-reader-thermal-XXXXXX";
  float temperature;
  int result = -1;

  if (mkdtemp(root) == NULL) {
    perror("mkdtemp thermal root");
    return -1;
  }
  /* Put a non-CPU sensor first lexically; selection must still prefer CPU. */
  if (make_zone(root, "thermal_zone0", "acpitz\n", "33000\n") != 0 ||
      make_zone(root, "thermal_zone1", "Package id 0\n", "55000\n") != 0) {
    perror("create thermal fixture");
    goto cleanup;
  }

  setenv("CPU_READER_THERMAL_PATH", root, 1);
  temperature = cpu_get_temperature();
  unsetenv("CPU_READER_THERMAL_PATH");
  if (temperature != 55.0f) {
    fprintf(stderr, "expected CPU/package sensor 55 C, got %.2f C\n",
            temperature);
    goto cleanup;
  }
  result = 0;

cleanup:
  {
    char path[512];
    const char *zones[] = {"thermal_zone0", "thermal_zone1"};
    size_t index;
    for (index = 0; index < sizeof(zones) / sizeof(zones[0]); index++) {
      snprintf(path, sizeof(path), "%s/%s/type", root, zones[index]);
      unlink(path);
      snprintf(path, sizeof(path), "%s/%s/temp", root, zones[index]);
      unlink(path);
      snprintf(path, sizeof(path), "%s/%s", root, zones[index]);
      rmdir(path);
    }
  }
  rmdir(root);
  return result;
}

static int test_unreadable_cpuinfo_file(void) {
  char path[] = "/tmp/cpu-reader-unreadable-XXXXXX";
  int fd = mkstemp(path);
  cpu_info_t *info;

  if (fd < 0) {
    perror("mkstemp unreadable cpuinfo");
    return -1;
  }
  close(fd);
  if (chmod(path, 0000) != 0) {
    unlink(path);
    perror("chmod unreadable cpuinfo");
    return -1;
  }

  setenv("CPU_READER_CPUINFO_PATH", path, 1);
  info = cpu_get_info();
  unsetenv("CPU_READER_CPUINFO_PATH");
  chmod(path, 0600);
  unlink(path);
  if (info != NULL || cpu_get_last_error_code() != CPU_ERROR_FILE_OPEN) {
    cpu_free_info(info);
    fprintf(stderr, "unreadable cpuinfo should report file-open failure\n");
    return -1;
  }
  return 0;
}

static int test_missing_thermal_sysfs_root(void) {
  float temperature;

  unsetenv("CPU_READER_CPU_TEMP_PATH");
  setenv("CPU_READER_THERMAL_PATH",
         "/tmp/cpu-reader-missing-thermal-root", 1);
  temperature = cpu_get_temperature();
  unsetenv("CPU_READER_THERMAL_PATH");
  if (temperature != -1.0f ||
      cpu_get_last_error_code() != CPU_ERROR_FILE_OPEN) {
    fprintf(stderr, "missing thermal sysfs root should be reported\n");
    return -1;
  }
  return 0;
}

static int test_topology_with_and_without_ids(void) {
  char path[] = "/tmp/cpu-reader-topology-XXXXXX";
  const char *with_ids =
      "processor : 0\nmodel name : Test CPU\nphysical id : 0\ncore id : 3\n"
      "\nprocessor : 1\nphysical id : 0\ncore id : 3\n";
  const char *without_ids = "processor : 0\nProcessor : ARMv7 Test CPU\n";
  cpu_info_t *info;
  int fd = mkstemp(path);

  if (fd < 0) {
    perror("mkstemp topology");
    return -1;
  }
  close(fd);

  if (write_text(path, with_ids) != 0) {
    unlink(path);
    return -1;
  }
  setenv("CPU_READER_CPUINFO_PATH", path, 1);
  info = cpu_get_info();
  if (info == NULL || info->logical_processors != 2 ||
      info->physical_cores != 1) {
    fprintf(stderr, "topology with repeated physical/core pair was miscounted\n");
    cpu_free_info(info);
    unsetenv("CPU_READER_CPUINFO_PATH");
    unlink(path);
    return -1;
  }
  cpu_free_info(info);

  if (write_text(path, "processor : 0\nphysical id : 0\n") != 0) {
    unsetenv("CPU_READER_CPUINFO_PATH");
    unlink(path);
    return -1;
  }
  info = cpu_get_info();
  if (info == NULL || info->logical_processors != 1 ||
      info->physical_cores != 0) {
    fprintf(stderr, "partial topology metadata must not invent a physical core\n");
    cpu_free_info(info);
    unsetenv("CPU_READER_CPUINFO_PATH");
    unlink(path);
    return -1;
  }
  cpu_free_info(info);

  if (write_text(path, without_ids) != 0) {
    unsetenv("CPU_READER_CPUINFO_PATH");
    unlink(path);
    return -1;
  }
  info = cpu_get_info();
  unsetenv("CPU_READER_CPUINFO_PATH");
  unlink(path);
  if (info == NULL || info->logical_processors != 1 ||
      info->physical_cores != 0 || strcmp(info->model, "ARMv7 Test CPU") != 0) {
    fprintf(stderr, "missing topology IDs should be unknown, not fabricated\n");
    cpu_free_info(info);
    return -1;
  }
  cpu_free_info(info);
  return 0;
}

static int test_topology_grows_beyond_online_count(void) {
  char path[] = "/tmp/cpu-reader-topology-large-XXXXXX";
  long online = sysconf(_SC_NPROCESSORS_ONLN);
  long count;
  long index;
  int fd;
  FILE *file;
  cpu_info_t *info;

  if (online <= 0 || online > 4096) {
    fprintf(stderr, "unexpected online processor count for fixture: %ld\n",
            online);
    return -1;
  }
  count = online + 2;
  fd = mkstemp(path);
  if (fd < 0) {
    perror("mkstemp large topology");
    return -1;
  }
  file = fdopen(fd, "w");
  if (file == NULL) {
    close(fd);
    unlink(path);
    return -1;
  }
  for (index = 0; index < count; index++) {
    if (fprintf(file, "processor : %ld\nphysical id : 0\ncore id : %ld\n\n",
                index, index) < 0) {
      fclose(file);
      unlink(path);
      return -1;
    }
  }
  if (fclose(file) != 0) {
    unlink(path);
    return -1;
  }

  setenv("CPU_READER_CPUINFO_PATH", path, 1);
  info = cpu_get_info();
  unsetenv("CPU_READER_CPUINFO_PATH");
  unlink(path);
  if (info == NULL || info->logical_processors != count ||
      info->physical_cores != count || info->threads != online) {
    fprintf(stderr, "topology storage did not grow for larger cpuinfo fixture\n");
    cpu_free_info(info);
    return -1;
  }
  cpu_free_info(info);
  return 0;
}

typedef struct {
  cpu_error_t code;
  char message[256];
  cpu_error_info_t snapshot;
} thread_result_t;

static void *worker_read_missing_cpuinfo(void *argument) {
  thread_result_t *result = (thread_result_t *)argument;
  cpu_info_t *info = cpu_get_info_ex(&result->snapshot);

  cpu_free_info(info);
  result->code = result->snapshot.code;
  snprintf(result->message, sizeof(result->message), "%s", result->snapshot.message);
  return NULL;
}

static int test_error_snapshot_survives_later_success(void) {
  char missing_path[] = "/tmp/cpu-reader-missing-explicit-XXXXXX";
  char valid_path[] = "/tmp/cpu-reader-cpuinfo-valid-XXXXXX";
  const char *valid_fixture = "processor : 0\nProcessor : Test CPU\n";
  cpu_error_info_t error;
  cpu_info_t *info;
  int missing_fd = mkstemp(missing_path);
  int fd = mkstemp(valid_path);

  if (missing_fd < 0) {
    perror("mkstemp missing explicit error path");
    return -1;
  }
  close(missing_fd);
  unlink(missing_path);
  if (fd < 0) {
    perror("mkstemp explicit error fixture");
    return -1;
  }
  close(fd);
  if (write_text(valid_path, valid_fixture) != 0) {
    unlink(valid_path);
    return -1;
  }

  cpu_error_info_clear(&error);
  setenv("CPU_READER_CPUINFO_PATH", missing_path, 1);
  info = cpu_get_info_ex(&error);
  if (info != NULL || error.code != CPU_ERROR_FILE_OPEN ||
      strstr(error.message, "Nao foi possivel abrir") == NULL) {
    cpu_free_info(info);
    unsetenv("CPU_READER_CPUINFO_PATH");
    unlink(valid_path);
    fprintf(stderr, "explicit error snapshot was not populated\n");
    return -1;
  }

  setenv("CPU_READER_CPUINFO_PATH", valid_path, 1);
  info = cpu_get_info();
  unsetenv("CPU_READER_CPUINFO_PATH");
  unlink(valid_path);
  if (info == NULL || error.code != CPU_ERROR_FILE_OPEN ||
      strstr(error.message, "Nao foi possivel abrir") == NULL) {
    cpu_free_info(info);
    fprintf(stderr, "later successful operation modified caller-owned error\n");
    return -1;
  }
  cpu_free_info(info);
  cpu_error_info_clear(&error);
  if (error.code != CPU_ERROR_NONE || error.message[0] != '\0') {
    fprintf(stderr, "explicit error snapshot clear failed\n");
    return -1;
  }
  return 0;
}

static int test_error_state_is_thread_local(void) {
  pthread_t thread;
  thread_result_t result;
  memset(&result, 0, sizeof(result));
  setenv("CPU_READER_CPUINFO_PATH", "/tmp/cpu-reader-missing-thread-cpuinfo", 1);
  if (cpu_usage_context_init(NULL) != -1 ||
      cpu_get_last_error_code() != CPU_ERROR_INVALID_ARGUMENT) {
    unsetenv("CPU_READER_CPUINFO_PATH");
    return -1;
  }
  if (pthread_create(&thread, NULL, worker_read_missing_cpuinfo, &result) != 0) {
    unsetenv("CPU_READER_CPUINFO_PATH");
    return -1;
  }
  if (pthread_join(thread, NULL) != 0) {
    unsetenv("CPU_READER_CPUINFO_PATH");
    return -1;
  }
  unsetenv("CPU_READER_CPUINFO_PATH");

  if (result.code != CPU_ERROR_FILE_OPEN ||
      strstr(result.message, "Nao foi possivel abrir") == NULL ||
      cpu_get_last_error_code() != CPU_ERROR_INVALID_ARGUMENT) {
    fprintf(stderr, "error diagnostics leaked between threads\n");
    return -1;
  }
  return 0;
}

int main(void) {
  if (test_thermal_sensor_preference() != 0 ||
      test_unreadable_cpuinfo_file() != 0 ||
      test_missing_thermal_sysfs_root() != 0 ||
      test_topology_with_and_without_ids() != 0 ||
      test_topology_grows_beyond_online_count() != 0 ||
      test_error_snapshot_survives_later_success() != 0 ||
      test_error_state_is_thread_local() != 0) {
    return 1;
  }
  return 0;
}
