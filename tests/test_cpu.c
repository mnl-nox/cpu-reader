#define _POSIX_C_SOURCE 200809L

#include "cpu.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int write_file(const char *path, const char *content) {
  FILE *file = fopen(path, "w");

  if (file == NULL) {
    return -1;
  }

  if (fputs(content, file) == EOF) {
    fclose(file);
    return -1;
  }

  if (fclose(file) != 0) {
    return -1;
  }

  return 0;
}

static int make_temp_file(char *path_template, const char *content) {
  int fd = mkstemp(path_template);
  FILE *file;

  if (fd == -1) {
    return -1;
  }

  file = fdopen(fd, "w");
  if (file == NULL) {
    close(fd);
    unlink(path_template);
    return -1;
  }

  if (fputs(content, file) == EOF || fclose(file) != 0) {
    unlink(path_template);
    return -1;
  }

  return 0;
}

static int expect_last_error(cpu_error_t code, const char *message_fragment) {
  if (cpu_get_last_error_code() != code ||
      strstr(cpu_get_last_error(), message_fragment) == NULL) {
    fprintf(stderr, "unexpected error report: code=%d message=%s\n",
            cpu_get_last_error_code(), cpu_get_last_error());
    return -1;
  }

  return 0;
}

static int expect_info_failure_for_missing_file(void) {
  setenv("CPU_READER_CPUINFO_PATH", "/tmp/cpu-reader-missing-cpuinfo", 1);
  if (cpu_get_info() != NULL) {
    fprintf(stderr, "cpu_get_info should fail for a missing file\n");
    unsetenv("CPU_READER_CPUINFO_PATH");
    return -1;
  }

  if (expect_last_error(CPU_ERROR_FILE_OPEN, "Nao foi possivel abrir") != 0) {
    unsetenv("CPU_READER_CPUINFO_PATH");
    return -1;
  }

  unsetenv("CPU_READER_CPUINFO_PATH");
  return 0;
}

static int expect_info_failure_for_invalid_content(void) {
  char path[] = "/tmp/cpu-reader-cpuinfo-XXXXXX";

  if (make_temp_file(path, "model name : test only\ncpu MHz : 1234.0\n") != 0) {
    fprintf(stderr, "failed to create invalid cpuinfo fixture: %s\n",
            strerror(errno));
    return -1;
  }

  setenv("CPU_READER_CPUINFO_PATH", path, 1);
  if (cpu_get_info() != NULL) {
    fprintf(stderr, "cpu_get_info should fail for incomplete parsing\n");
    unsetenv("CPU_READER_CPUINFO_PATH");
    unlink(path);
    return -1;
  }

  if (expect_last_error(CPU_ERROR_PARSE, "Dados obrigatorios") != 0) {
    unsetenv("CPU_READER_CPUINFO_PATH");
    unlink(path);
    return -1;
  }

  unsetenv("CPU_READER_CPUINFO_PATH");
  unlink(path);
  return 0;
}

static int expect_usage_failure_for_missing_file(cpu_usage_context_t *context) {
  setenv("CPU_READER_PROC_STAT_PATH", "/tmp/cpu-reader-missing-stat", 1);
  if (cpu_get_usage_context(context) != -1.0f) {
    fprintf(stderr, "cpu_get_usage_context should fail for a missing file\n");
    unsetenv("CPU_READER_PROC_STAT_PATH");
    return -1;
  }

  if (expect_last_error(CPU_ERROR_FILE_OPEN, "Nao foi possivel abrir") != 0) {
    unsetenv("CPU_READER_PROC_STAT_PATH");
    return -1;
  }

  unsetenv("CPU_READER_PROC_STAT_PATH");
  return 0;
}

static int
expect_usage_failure_for_invalid_content(cpu_usage_context_t *context) {
  char path[] = "/tmp/cpu-reader-stat-XXXXXX";

  if (make_temp_file(path, "cpu invalid data\n") != 0) {
    fprintf(stderr, "failed to create invalid stat fixture: %s\n",
            strerror(errno));
    return -1;
  }

  setenv("CPU_READER_PROC_STAT_PATH", path, 1);
  if (cpu_get_usage_context(context) != -1.0f) {
    fprintf(stderr, "cpu_get_usage_context should fail for invalid parsing\n");
    unsetenv("CPU_READER_PROC_STAT_PATH");
    unlink(path);
    return -1;
  }

  if (expect_last_error(CPU_ERROR_PARSE, "Nao foi possivel interpretar") != 0) {
    unsetenv("CPU_READER_PROC_STAT_PATH");
    unlink(path);
    return -1;
  }

  unsetenv("CPU_READER_PROC_STAT_PATH");
  unlink(path);
  return 0;
}

static int expect_temperature_success(void) {
  char path[] = "/tmp/cpu-reader-temp-XXXXXX";
  float temperature;

  if (make_temp_file(path, "47000\n") != 0) {
    fprintf(stderr, "failed to create temperature fixture: %s\n",
            strerror(errno));
    return -1;
  }

  setenv("CPU_READER_CPU_TEMP_PATH", path, 1);
  temperature = cpu_get_temperature();
  unsetenv("CPU_READER_CPU_TEMP_PATH");
  unlink(path);
  if (temperature != 47.0f) {
    fprintf(stderr, "cpu_get_temperature returned unexpected fixture value\n");
    return -1;
  }

  return 0;
}

static int expect_temperature_failure(void) {
  char path[] = "/tmp/cpu-reader-temp-invalid-XXXXXX";

  if (make_temp_file(path, "invalid\n") != 0) {
    fprintf(stderr, "failed to create invalid temperature fixture: %s\n",
            strerror(errno));
    return -1;
  }

  setenv("CPU_READER_CPU_TEMP_PATH", path, 1);
  if (cpu_get_temperature() != -1.0f) {
    fprintf(stderr, "cpu_get_temperature should fail for invalid parsing\n");
    unsetenv("CPU_READER_CPU_TEMP_PATH");
    unlink(path);
    return -1;
  }

  unsetenv("CPU_READER_CPU_TEMP_PATH");
  unlink(path);
  return expect_last_error(CPU_ERROR_PARSE, "temperatura");
}

static int expect_clock_speed_success(void) {
  char path[] = "/tmp/cpu-reader-freq-XXXXXX";
  float clock_speed;

  if (make_temp_file(path, "3200000\n") != 0) {
    fprintf(stderr, "failed to create frequency fixture: %s\n",
            strerror(errno));
    return -1;
  }

  setenv("CPU_READER_CPU_FREQ_PATH", path, 1);
  clock_speed = cpu_get_clock_speed();
  unsetenv("CPU_READER_CPU_FREQ_PATH");
  unlink(path);
  if (clock_speed != 3200.0f) {
    fprintf(stderr, "cpu_get_clock_speed returned unexpected fixture value\n");
    return -1;
  }

  return 0;
}

static int expect_clock_speed_failure(void) {
  char path[] = "/tmp/cpu-reader-freq-invalid-XXXXXX";

  if (make_temp_file(path, "invalid\n") != 0) {
    fprintf(stderr, "failed to create invalid frequency fixture: %s\n",
            strerror(errno));
    return -1;
  }

  setenv("CPU_READER_CPU_FREQ_PATH", path, 1);
  if (cpu_get_clock_speed() != -1.0f) {
    fprintf(stderr, "cpu_get_clock_speed should fail for invalid parsing\n");
    unsetenv("CPU_READER_CPU_FREQ_PATH");
    unlink(path);
    return -1;
  }

  unsetenv("CPU_READER_CPU_FREQ_PATH");
  unlink(path);
  return expect_last_error(CPU_ERROR_PARSE, "frequencia");
}

static int expect_clock_speed_rejects_trailing_data(void) {
  char path[] = "/tmp/cpu-reader-freq-truncated-XXXXXX";

  if (make_temp_file(path, "3200000junk\n") != 0) {
    return -1;
  }
  setenv("CPU_READER_CPU_FREQ_PATH", path, 1);
  if (cpu_get_clock_speed() != -1.0f ||
      expect_last_error(CPU_ERROR_PARSE, "frequencia") != 0) {
    unsetenv("CPU_READER_CPU_FREQ_PATH");
    unlink(path);
    return -1;
  }
  unsetenv("CPU_READER_CPU_FREQ_PATH");
  unlink(path);
  return 0;
}

static int expect_active_processes_success(void) {
  char path[] = "/tmp/cpu-reader-loadavg-XXXXXX";
  int active_processes;

  if (make_temp_file(path, "0.10 0.20 0.30 7/321 12345\n") != 0) {
    fprintf(stderr, "failed to create loadavg fixture: %s\n", strerror(errno));
    return -1;
  }

  setenv("CPU_READER_LOADAVG_PATH", path, 1);
  active_processes = cpu_get_active_processes();
  unsetenv("CPU_READER_LOADAVG_PATH");
  unlink(path);
  if (active_processes != 7) {
    fprintf(stderr,
            "cpu_get_active_processes returned unexpected fixture value\n");
    return -1;
  }

  return 0;
}

static int expect_active_processes_failure(void) {
  char path[] = "/tmp/cpu-reader-loadavg-invalid-XXXXXX";

  if (make_temp_file(path, "invalid\n") != 0) {
    fprintf(stderr, "failed to create invalid loadavg fixture: %s\n",
            strerror(errno));
    return -1;
  }

  setenv("CPU_READER_LOADAVG_PATH", path, 1);
  if (cpu_get_active_processes() != -1) {
    fprintf(stderr,
            "cpu_get_active_processes should fail for invalid parsing\n");
    unsetenv("CPU_READER_LOADAVG_PATH");
    unlink(path);
    return -1;
  }

  unsetenv("CPU_READER_LOADAVG_PATH");
  unlink(path);
  return expect_last_error(CPU_ERROR_PARSE, "processos ativos");
}

static int expect_usage_rejects_truncated_line(void) {
  char path[] = "/tmp/cpu-reader-stat-truncated-XXXXXX";
  cpu_usage_context_t context;

  if (make_temp_file(path, "cpu 10 0 5 20\n") != 0 ||
      cpu_usage_context_init(&context) != 0) {
    return -1;
  }
  setenv("CPU_READER_PROC_STAT_PATH", path, 1);
  if (cpu_get_usage_context(&context) != -1.0f ||
      expect_last_error(CPU_ERROR_PARSE, "linha cpu") != 0) {
    unsetenv("CPU_READER_PROC_STAT_PATH");
    unlink(path);
    return -1;
  }
  unsetenv("CPU_READER_PROC_STAT_PATH");
  unlink(path);
  return 0;
}

int main(void) {
  cpu_info_t *info = cpu_get_info();
  cpu_usage_context_t first_context;
  cpu_usage_context_t second_context;
  cpu_usage_context_t fixture_context;
  float usage;
  char stat_fixture[] = "/tmp/cpu-reader-stat-valid-XXXXXX";

  if (info == NULL || info->logical_processors <= 0 ||
      info->threads <= 0 || info->model[0] == '\0' ||
      info->current_frequency_mhz <= 0.0f ||
      info->flags[0] == '\0') {
    fprintf(stderr, "cpu_get_info returned incomplete data\n");
    return 1;
  }

  if (cpu_usage_context_init(NULL) != -1 ||
      expect_last_error(CPU_ERROR_INVALID_ARGUMENT, "nao pode ser nulo") != 0) {
    fprintf(stderr, "cpu_usage_context_init did not report a null context\n");
    cpu_free_info(info);
    return 1;
  }

  if (cpu_usage_context_init(&first_context) != 0 ||
      cpu_usage_context_init(&second_context) != 0 ||
      cpu_get_usage_context(NULL) >= 0.0f ||
      cpu_get_usage_context(&first_context) != 0.0f ||
      cpu_get_usage_context(&second_context) != 0.0f ||
      cpu_usage_context_init(&fixture_context) != 0 || cpu_init() != 0 ||
      cpu_get_usage() != 0.0f) {
    fprintf(stderr, "cpu usage contexts failed\n");
    cpu_free_info(info);
    return 1;
  }

  if (expect_info_failure_for_missing_file() != 0 ||
      expect_info_failure_for_invalid_content() != 0 ||
      expect_usage_failure_for_missing_file(&fixture_context) != 0 ||
      expect_usage_failure_for_invalid_content(&fixture_context) != 0 ||
      expect_temperature_success() != 0 || expect_temperature_failure() != 0 ||
      expect_clock_speed_success() != 0 || expect_clock_speed_failure() != 0 ||
      expect_clock_speed_rejects_trailing_data() != 0 ||
      expect_active_processes_success() != 0 ||
      expect_active_processes_failure() != 0 ||
      expect_usage_rejects_truncated_line() != 0) {
    cpu_free_info(info);
    return 1;
  }

  if (make_temp_file(stat_fixture,
                     "cpu 10 0 5 20 0 0 0 0\ncpu0 10 0 5 20 0 0 0 0\n") != 0) {
    fprintf(stderr, "failed to create stat fixture: %s\n", strerror(errno));
    cpu_free_info(info);
    return 1;
  }

  setenv("CPU_READER_PROC_STAT_PATH", stat_fixture, 1);
  if (cpu_get_usage_context(&fixture_context) != 0.0f ||
      write_file(stat_fixture,
                 "cpu 20 0 10 20 0 0 0 0\ncpu0 20 0 10 20 0 0 0 0\n") != 0) {
    fprintf(stderr, "failed to prepare stat fixture sequence\n");
    unsetenv("CPU_READER_PROC_STAT_PATH");
    unlink(stat_fixture);
    cpu_free_info(info);
    return 1;
  }

  usage = cpu_get_usage_context(&fixture_context);
  unsetenv("CPU_READER_PROC_STAT_PATH");
  unlink(stat_fixture);
  if (usage != 100.0f) {
    fprintf(stderr,
            "cpu_get_usage_context returned unexpected fixture usage\n");
    cpu_free_info(info);
    return 1;
  }

  if (cpu_get_last_error_code() != CPU_ERROR_NONE ||
      cpu_get_last_error()[0] != '\0') {
    fprintf(stderr,
            "successful usage reading did not clear the error report\n");
    cpu_free_info(info);
    return 1;
  }

  usage = cpu_get_usage();
  if (usage < 0.0f || usage > 100.0f) {
    fprintf(stderr, "cpu_get_usage returned an invalid value\n");
    cpu_free_info(info);
    return 1;
  }

  if (info->active_processes < 0) {
    fprintf(stderr, "cpu_get_info returned invalid active_processes\n");
    cpu_free_info(info);
    return 1;
  }

  cpu_free_info(info);
  return 0;
}
