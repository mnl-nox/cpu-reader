#define _POSIX_C_SOURCE 200809L

#include "cpu_internal.h"

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *cpuinfo_path(void) {
  const char *path = getenv("CPU_READER_CPUINFO_PATH");

  if (path == NULL || path[0] == '\0') {
    return "/proc/cpuinfo";
  }

  return path;
}

static const char *cpu_temp_path(void) {
  const char *path = getenv("CPU_READER_CPU_TEMP_PATH");

  if (path == NULL || path[0] == '\0') {
    return NULL;
  }

  return path;
}

static const char *cpu_freq_path(void) {
  const char *path = getenv("CPU_READER_CPU_FREQ_PATH");

  if (path == NULL || path[0] == '\0') {
    return "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";
  }

  return path;
}

static const char *proc_loadavg_path(void) {
  const char *path = getenv("CPU_READER_LOADAVG_PATH");

  if (path == NULL || path[0] == '\0') {
    return "/proc/loadavg";
  }

  return path;
}

static void trim_key(char *key);

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

static float parse_frequency_from_cpuinfo(void) {
  FILE *file = fopen(cpuinfo_path(), "r");
  char line[1024];

  if (file == NULL) {
    return -1.0f;
  }

  while (fgets(line, sizeof(line), file) != NULL) {
    char *separator = strchr(line, ':');

    if (separator == NULL) {
      continue;
    }

    *separator = '\0';
    separator++;
    trim_key(line);
    if (strcmp(line, "cpu MHz") == 0) {
      float frequency = strtof(separator, NULL);

      fclose(file);
      return frequency;
    }
  }

  fclose(file);
  return -1.0f;
}

static void copy_value(char *destination, size_t size, const char *value) {
  value += strspn(value, " \t");
  snprintf(destination, size, "%s", value);
  destination[strcspn(destination, "\n")] = '\0';
}

static void trim_key(char *key) {
  size_t length = strlen(key);

  while (length > 0 && (key[length - 1] == ' ' || key[length - 1] == '\t')) {
    key[--length] = '\0';
  }
}

static int read_first_line(const char *path, char *buffer, size_t size) {
  FILE *file = fopen(path, "r");

  if (file == NULL) {
    return -1;
  }

  if (fgets(buffer, (int)size, file) == NULL) {
    fclose(file);
    errno = EIO;
    return -1;
  }

  fclose(file);
  return 0;
}

static float read_temperature_from_path(const char *path, int report_errors) {
  char buffer[128];
  char *end = NULL;
  long value;

  if (read_first_line(path, buffer, sizeof(buffer)) != 0) {
    if (report_errors) {
      cpu_set_last_error(CPU_ERROR_FILE_OPEN, "Nao foi possivel abrir %s: %s",
                         path, strerror(errno));
    }
    return -1.0f;
  }

  errno = 0;
  value = strtol(buffer, &end, 10);
  if (errno != 0 || end == buffer) {
    if (report_errors) {
      cpu_set_last_error(CPU_ERROR_PARSE,
                         "Nao foi possivel interpretar a temperatura em %s",
                         path);
    }
    return -1.0f;
  }

  return (float)value / 1000.0f;
}

static float read_temperature_auto(int report_errors) {
  DIR *directory = opendir("/sys/class/thermal");
  struct dirent *entry;

  if (directory == NULL) {
    if (report_errors) {
      cpu_set_last_error(CPU_ERROR_FILE_OPEN,
                         "Nao foi possivel abrir /sys/class/thermal: %s",
                         strerror(errno));
    }
    return -1.0f;
  }

  while ((entry = readdir(directory)) != NULL) {
    char path[PATH_MAX];
    float temperature;

    if (strncmp(entry->d_name, "thermal_zone", 12) != 0) {
      continue;
    }

    snprintf(path, sizeof(path), "/sys/class/thermal/%s/temp", entry->d_name);
    temperature = read_temperature_from_path(path, 0);
    if (temperature >= 0.0f) {
      closedir(directory);
      return temperature;
    }
  }

  closedir(directory);
  if (report_errors) {
    cpu_set_last_error(CPU_ERROR_UNSUPPORTED,
                       "Nao foi possivel localizar um sensor de temperatura");
  }
  return -1.0f;
}

float cpu_read_temperature(int report_errors) {
  const char *path = cpu_temp_path();
  float temperature;

  temperature = path != NULL ? read_temperature_from_path(path, report_errors)
                             : read_temperature_auto(report_errors);
  if (temperature >= 0.0f && report_errors) {
    cpu_clear_last_error();
  }

  return temperature;
}

float cpu_read_clock_speed(int report_errors) {
  const char *path = cpu_freq_path();
  char buffer[128];
  char *end = NULL;
  long value;
  float fallback_frequency;

  if (read_first_line(path, buffer, sizeof(buffer)) != 0) {
    fallback_frequency = parse_frequency_from_cpuinfo();
    if (fallback_frequency > 0.0f) {
      if (report_errors) {
        cpu_clear_last_error();
      }
      return fallback_frequency;
    }

    if (report_errors) {
      cpu_set_last_error(CPU_ERROR_FILE_OPEN, "Nao foi possivel abrir %s: %s",
                         path, strerror(errno));
    }
    return -1.0f;
  }

  errno = 0;
  value = strtol(buffer, &end, 10);
  if (errno != 0 || end == buffer) {
    if (report_errors) {
      cpu_set_last_error(CPU_ERROR_PARSE,
                         "Nao foi possivel interpretar a frequencia em %s",
                         path);
    }
    return -1.0f;
  }

  if (report_errors) {
    cpu_clear_last_error();
  }

  return (float)value / 1000.0f;
}

int cpu_read_active_processes(int report_errors) {
  const char *path = proc_loadavg_path();
  FILE *file = fopen(path, "r");
  float load1;
  float load5;
  float load15;
  int running;
  int total;
  int last_pid;

  if (file == NULL) {
    if (report_errors) {
      cpu_set_last_error(CPU_ERROR_FILE_OPEN, "Nao foi possivel abrir %s: %s",
                         path, strerror(errno));
    }
    return -1;
  }

  if (fscanf(file, "%f %f %f %d/%d %d", &load1, &load5, &load15, &running,
             &total, &last_pid) != 6) {
    fclose(file);
    if (report_errors) {
      cpu_set_last_error(
          CPU_ERROR_PARSE,
          "Nao foi possivel interpretar os processos ativos em %s", path);
    }
    return -1;
  }

  fclose(file);
  (void)load1;
  (void)load5;
  (void)load15;
  (void)total;
  (void)last_pid;

  if (report_errors) {
    cpu_clear_last_error();
  }

  return running;
}

cpu_info_t *cpu_get_info(void) {
  FILE *file;
  cpu_info_t *info;
  const char *path = cpuinfo_path();
  char line[1024];

  file = fopen(path, "r");
  if (file == NULL) {
    cpu_set_last_error(CPU_ERROR_FILE_OPEN, "Nao foi possivel abrir %s: %s",
                       path, strerror(errno));
    return NULL;
  }

  info = calloc(1, sizeof(*info));
  if (info == NULL) {
    fclose(file);
    cpu_set_last_error(CPU_ERROR_MEMORY,
                       "Nao foi possivel alocar informacoes da CPU");
    return NULL;
  }

  info->threads = (int)sysconf(_SC_NPROCESSORS_ONLN);
  while (fgets(line, sizeof(line), file) != NULL) {
    char *separator = strchr(line, ':');

    if (separator == NULL) {
      continue;
    }
    *separator = '\0';
    separator++;
    trim_key(line);

    if (strcmp(line, "processor") == 0) {
      info->cores++;
    } else if (strcmp(line, "model name") == 0 && info->model[0] == '\0') {
      copy_value(info->model, sizeof(info->model), separator);
    } else if (strcmp(line, "cpu MHz") == 0 && info->frequency_mhz == 0.0f) {
      info->frequency_mhz = strtof(separator, NULL);
    } else if (strcmp(line, "flags") == 0 && info->flags[0] == '\0') {
      copy_value(info->flags, sizeof(info->flags), separator);
    }
  }

  fclose(file);
  if (info->cores <= 0 || info->threads <= 0) {
    free(info);
    cpu_set_last_error(CPU_ERROR_PARSE, "Dados obrigatorios ausentes em %s",
                       path);
    return NULL;
  }

  info->active_processes = cpu_read_active_processes(0);
  info->temperature_c = cpu_read_temperature(0);
  if (info->frequency_mhz <= 0.0f) {
    info->frequency_mhz = cpu_read_clock_speed(0);
  }

  cpu_clear_last_error();
  return info;
}
