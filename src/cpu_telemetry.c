#define _POSIX_C_SOURCE 200809L

#include "cpu_internal.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *cpuinfo_path(void) {
  const char *path = getenv("CPU_READER_CPUINFO_PATH");

  return path != NULL && path[0] != '\0' ? path : "/proc/cpuinfo";
}

static void trim_key(char *key) {
  size_t length = strlen(key);

  while (length > 0 && (key[length - 1] == ' ' || key[length - 1] == '\t')) {
    key[--length] = '\0';
  }
}

static const char *cpu_temp_path(void) {
  const char *path = getenv("CPU_READER_CPU_TEMP_PATH");

  if (path == NULL || path[0] == '\0') {
    return NULL;
  }

  return path;
}

static const char *thermal_root_path(void) {
  const char *path = getenv("CPU_READER_THERMAL_PATH");

  return path != NULL && path[0] != '\0' ? path : "/sys/class/thermal";
}

static int is_cpu_thermal_type(const char *type) {
  char normalized[128];
  size_t index;

  if (type == NULL) {
    return 0;
  }
  for (index = 0; index + 1 < sizeof(normalized) && type[index] != '\0'; index++) {
    normalized[index] = (char)tolower((unsigned char)type[index]);
  }
  normalized[index] = '\0';
  return strstr(normalized, "cpu") != NULL ||
         strstr(normalized, "pkg_temp") != NULL ||
         strstr(normalized, "package") != NULL;
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

static int parse_loadavg_line(char *line, float *load1, float *load5,
                              float *load15, int *running, int *total,
                              int *last_pid) {
  char *tokens[5];
  char *save = NULL;
  char *slash;
  long running_value;
  long total_value;
  long pid_value;
  int index;

  for (index = 0; index < 5; index++) {
    tokens[index] = strtok_r(index == 0 ? line : NULL, " \t\r\n", &save);
    if (tokens[index] == NULL) {
      return -1;
    }
  }
  if (strtok_r(NULL, " \t\r\n", &save) != NULL ||
      cpu_parse_float(tokens[0], load1) != 0 ||
      cpu_parse_float(tokens[1], load5) != 0 ||
      cpu_parse_float(tokens[2], load15) != 0) {
    return -1;
  }
  slash = strchr(tokens[3], '/');
  if (slash == NULL) {
    return -1;
  }
  *slash = '\0';
  if (cpu_parse_long(tokens[3], &running_value) != 0 ||
      cpu_parse_long(slash + 1, &total_value) != 0 ||
      cpu_parse_long(tokens[4], &pid_value) != 0 ||
      running_value < 0 || total_value <= 0 || running_value > total_value ||
      pid_value < 0 || *load1 < 0.0f || *load5 < 0.0f || *load15 < 0.0f ||
      running_value > INT_MAX || total_value > INT_MAX || pid_value > INT_MAX) {
    return -1;
  }
  *running = (int)running_value;
  *total = (int)total_value;
  *last_pid = (int)pid_value;
  return 0;
}

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
      float frequency;

      fclose(file);
      if (cpu_parse_float(separator, &frequency) == 0 && frequency > 0.0f) {
        return frequency;
      }
      return -1.0f;
    }
  }

  fclose(file);
  return -1.0f;
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
  if (strchr(buffer, '\n') == NULL && !feof(file)) {
    fclose(file);
    errno = EOVERFLOW;
    return -1;
  }

  fclose(file);
  return 0;
}

static float read_temperature_from_path(const char *path, int report_errors) {
  char buffer[128];
  long value;

  if (read_first_line(path, buffer, sizeof(buffer)) != 0) {
    if (report_errors) {
      if (errno == EOVERFLOW) {
        cpu_set_last_error(CPU_ERROR_PARSE,
                           "Linha de temperatura truncada em %s", path);
      } else {
        cpu_set_last_error(CPU_ERROR_FILE_OPEN,
                           "Nao foi possivel abrir %s: %s", path,
                           strerror(errno));
      }
    }
    return -1.0f;
  }

  if (cpu_parse_long(buffer, &value) != 0 || value < 0) {
    if (report_errors) {
      cpu_set_last_error(CPU_ERROR_PARSE,
                         "Nao foi possivel interpretar a temperatura em %s",
                         path);
    }
    return -1.0f;
  }

  /* Linux thermal sysfs values are integer millidegrees Celsius. */
  return (float)value / 1000.0f;
}

static float read_temperature_auto(int report_errors) {
  const char *root = thermal_root_path();
  DIR *directory = opendir(root);
  struct dirent *entry;
  float fallback_temperature = -1.0f;

  if (directory == NULL) {
    if (report_errors) {
      cpu_set_last_error(CPU_ERROR_FILE_OPEN,
                         "Nao foi possivel abrir %s: %s", root,
                         strerror(errno));
    }
    return -1.0f;
  }

  while ((entry = readdir(directory)) != NULL) {
    char path[PATH_MAX];
    char type_path[PATH_MAX];
    char type[128];
    float temperature;
    int is_cpu_sensor = 0;

    if (strncmp(entry->d_name, "thermal_zone", 12) != 0) {
      continue;
    }

    if (snprintf(path, sizeof(path), "%s/%s/temp", root, entry->d_name) >=
            (int)sizeof(path) ||
        snprintf(type_path, sizeof(type_path), "%s/%s/type", root,
                 entry->d_name) >= (int)sizeof(type_path)) {
      continue;
    }
    if (read_first_line(type_path, type, sizeof(type)) == 0) {
      type[strcspn(type, "\r\n")] = '\0';
      is_cpu_sensor = is_cpu_thermal_type(type);
    }

    temperature = read_temperature_from_path(path, 0);
    if (temperature < 0.0f) {
      continue;
    }
    if (is_cpu_sensor) {
      closedir(directory);
      return temperature;
    }
    if (fallback_temperature < 0.0f) {
      fallback_temperature = temperature;
    }
  }

  closedir(directory);
  if (fallback_temperature >= 0.0f) {
    return fallback_temperature;
  }
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
      if (errno == EOVERFLOW) {
        cpu_set_last_error(CPU_ERROR_PARSE,
                           "Linha de frequencia truncada em %s", path);
      } else {
        cpu_set_last_error(CPU_ERROR_FILE_OPEN,
                           "Nao foi possivel abrir %s: %s", path,
                           strerror(errno));
      }
    }
    return -1.0f;
  }

  if (cpu_parse_long(buffer, &value) != 0 || value <= 0) {
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
  char line[256];
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

  if (fgets(line, sizeof(line), file) == NULL ||
      strchr(line, '\n') == NULL ||
      parse_loadavg_line(line, &load1, &load5, &load15, &running, &total,
                         &last_pid) != 0) {
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

