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
  DIR *directory = opendir("/sys/class/thermal");
  struct dirent *entry;
  float fallback_temperature = -1.0f;

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
    char type_path[PATH_MAX];
    char type[128];
    float temperature;
    int is_cpu_sensor = 0;

    if (strncmp(entry->d_name, "thermal_zone", 12) != 0) {
      continue;
    }

    snprintf(path, sizeof(path), "/sys/class/thermal/%s/temp", entry->d_name);
    snprintf(type_path, sizeof(type_path), "/sys/class/thermal/%s/type",
             entry->d_name);
    if (read_first_line(type_path, type, sizeof(type)) == 0) {
      type[strcspn(type, "\\r\\n")] = '\\0';
      is_cpu_sensor = strstr(type, "cpu") != NULL ||
                      strstr(type, "CPU") != NULL ||
                      strstr(type, "pkg_temp") != NULL;
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

cpu_info_t *cpu_get_info(void) {
  FILE *file;
  cpu_info_t *info;
  const char *path = cpuinfo_path();
  char line[1024];
  long online_processors;
  int current_physical_id = -1;
  int current_core_id = -1;
  int have_processor = 0;
  int physical_cores = 0;
  int *physical_ids = NULL;
  int *core_ids = NULL;
  int physical_capacity;

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

  online_processors = sysconf(_SC_NPROCESSORS_ONLN);
  if (online_processors <= 0 || online_processors > INT_MAX) {
    fclose(file);
    free(info);
    cpu_set_last_error(CPU_ERROR_PARSE, "Numero de processadores invalido");
    return NULL;
  }
  /* This is the online logical CPU count, not a physical core count. */
  info->threads = (int)online_processors;
  physical_capacity = info->threads;
  physical_ids = calloc((size_t)physical_capacity, sizeof(*physical_ids));
  core_ids = calloc((size_t)physical_capacity, sizeof(*core_ids));
  if (physical_ids == NULL || core_ids == NULL) {
    free(physical_ids);
    free(core_ids);
    fclose(file);
    free(info);
    cpu_set_last_error(CPU_ERROR_MEMORY,
                       "Nao foi possivel alocar topologia da CPU");
    return NULL;
  }
  while (fgets(line, sizeof(line), file) != NULL) {
    char *separator = strchr(line, ':');

    if (strchr(line, '\n') == NULL && !feof(file)) {
      free(physical_ids);
      free(core_ids);
      fclose(file);
      free(info);
      cpu_set_last_error(CPU_ERROR_PARSE, "Linha truncada em %s", path);
      return NULL;
    }
    if (separator == NULL) {
      continue;
    }
    *separator = '\0';
    separator++;
    trim_key(line);

    if (strcmp(line, "processor") == 0) {
      long processor_id;
      if (cpu_parse_long(separator, &processor_id) != 0 ||
          processor_id < 0 || processor_id > INT_MAX) {
        free(physical_ids);
        free(core_ids);
        fclose(file);
        free(info);
        cpu_set_last_error(CPU_ERROR_PARSE,
                           "ID de processador invalido em %s", path);
        return NULL;
      }
      if (have_processor && current_physical_id >= 0 && current_core_id >= 0) {
        int index;
        for (index = 0; index < physical_cores; index++) {
          if (physical_ids[index] == current_physical_id &&
              core_ids[index] == current_core_id) {
            break;
          }
        }
        if (index == physical_cores && physical_cores < physical_capacity) {
          physical_ids[physical_cores] = current_physical_id;
          core_ids[physical_cores++] = current_core_id;
        }
      }
      info->logical_processors++;
      have_processor = 1;
      current_physical_id = -1;
      current_core_id = -1;
    } else if ((strcmp(line, "model name") == 0 ||
                strcmp(line, "Processor") == 0 ||
                strcmp(line, "Hardware") == 0) &&
               info->model[0] == '\0') {
      copy_value(info->model, sizeof(info->model), separator);
    } else if (strcmp(line, "cpu MHz") == 0 &&
               info->current_frequency_mhz == 0.0f) {
      if (cpu_parse_float(separator, &info->current_frequency_mhz) != 0 ||
          info->current_frequency_mhz <= 0.0f) {
        free(physical_ids);
        free(core_ids);
        fclose(file);
        free(info);
        cpu_set_last_error(CPU_ERROR_PARSE,
                           "Frequencia invalida em %s", path);
        return NULL;
      }
    } else if (strcmp(line, "physical id") == 0) {
      long value;
      if (cpu_parse_long(separator, &value) != 0 || value < 0 ||
          value > INT_MAX) {
        free(physical_ids);
        free(core_ids);
        fclose(file);
        free(info);
        cpu_set_last_error(CPU_ERROR_PARSE, "ID fisico invalido em %s", path);
        return NULL;
      }
      current_physical_id = (int)value;
    } else if (strcmp(line, "core id") == 0) {
      long value;
      if (cpu_parse_long(separator, &value) != 0 || value < 0 ||
          value > INT_MAX) {
        free(physical_ids);
        free(core_ids);
        fclose(file);
        free(info);
        cpu_set_last_error(CPU_ERROR_PARSE, "ID de nucleo invalido em %s", path);
        return NULL;
      }
      current_core_id = (int)value;
    } else if ((strcmp(line, "flags") == 0 ||
                strcmp(line, "Features") == 0) &&
               info->flags[0] == '\0') {
      copy_value(info->flags, sizeof(info->flags), separator);
    }
  }

  if (have_processor && current_physical_id >= 0 && current_core_id >= 0) {
    int index;
    for (index = 0; index < physical_cores; index++) {
      if (physical_ids[index] == current_physical_id &&
          core_ids[index] == current_core_id) {
        break;
      }
    }
    if (index == physical_cores && physical_cores < physical_capacity) {
      physical_ids[physical_cores] = current_physical_id;
      core_ids[physical_cores++] = current_core_id;
    }
  }
  fclose(file);
  free(physical_ids);
  free(core_ids);
  if (info->logical_processors <= 0 || info->threads <= 0) {
    free(info);
    cpu_set_last_error(CPU_ERROR_PARSE, "Topologia de CPU ausente em %s", path);
    return NULL;
  }
  if (info->model[0] == '\0') {
    snprintf(info->model, sizeof(info->model), "%s", "Unknown CPU");
  }
  info->physical_cores = physical_cores;
  info->cores = info->physical_cores;
  info->frequency_mhz = info->current_frequency_mhz;

  info->active_processes = cpu_read_active_processes(0);
  info->temperature_c = cpu_read_temperature(0);
  if (info->current_frequency_mhz <= 0.0f) {
    info->current_frequency_mhz = cpu_read_clock_speed(0);
  }
  if (info->current_frequency_mhz > 0.0f) {
    info->frequency_mhz = info->current_frequency_mhz;
  }
  if (info->current_frequency_mhz <= 0.0f) {
    /* Frequency telemetry is optional on some architectures and virtualized
     * systems. Keep the CPU snapshot useful and report unavailable as -1. */
    info->current_frequency_mhz = -1.0f;
    info->frequency_mhz = -1.0f;
  }

  cpu_clear_last_error();
  return info;
}
