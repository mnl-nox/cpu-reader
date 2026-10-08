#define _POSIX_C_SOURCE 200809L

#include "cpu_internal.h"

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

static int record_physical_core(int physical_id, int core_id,
                               int **physical_ids, int **core_ids,
                               int *count, int *capacity) {
  int index;

  for (index = 0; index < *count; index++) {
    if ((*physical_ids)[index] == physical_id &&
        (*core_ids)[index] == core_id) {
      return 0;
    }
  }

  if (*count >= *capacity) {
    int new_capacity;
    int *new_physical_ids;
    int *new_core_ids;

    if (*capacity <= 0 || *capacity > INT_MAX / 2) {
      return -1;
    }
    new_capacity = *capacity * 2;
    if ((size_t)new_capacity > ((size_t)-1) / sizeof(int)) {
      return -1;
    }
    new_physical_ids =
        realloc(*physical_ids, (size_t)new_capacity * sizeof(int));
    if (new_physical_ids == NULL) {
      return -1;
    }
    *physical_ids = new_physical_ids;
    new_core_ids = realloc(*core_ids, (size_t)new_capacity * sizeof(int));
    if (new_core_ids == NULL) {
      return -1;
    }
    *core_ids = new_core_ids;
    *capacity = new_capacity;
  }

  (*physical_ids)[*count] = physical_id;
  (*core_ids)[*count] = core_id;
  (*count)++;
  return 0;
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
      if (have_processor && current_physical_id >= 0 &&
          current_core_id >= 0 &&
          record_physical_core(current_physical_id, current_core_id,
                               &physical_ids, &core_ids, &physical_cores,
                               &physical_capacity) != 0) {
        free(physical_ids);
        free(core_ids);
        fclose(file);
        free(info);
        cpu_set_last_error(CPU_ERROR_MEMORY,
                           "Nao foi possivel expandir a topologia da CPU");
        return NULL;
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

  if (ferror(file)) {
    int read_error = errno != 0 ? errno : EIO;
    free(physical_ids);
    free(core_ids);
    fclose(file);
    free(info);
    cpu_set_last_error(CPU_ERROR_FILE_OPEN, "Erro ao ler %s: %s", path,
                       strerror(read_error));
    return NULL;
  }

  if (have_processor && current_physical_id >= 0 &&
      current_core_id >= 0 &&
      record_physical_core(current_physical_id, current_core_id, &physical_ids,
                           &core_ids, &physical_cores, &physical_capacity) !=
          0) {
    free(physical_ids);
    free(core_ids);
    fclose(file);
    free(info);
    cpu_set_last_error(CPU_ERROR_MEMORY,
                       "Nao foi possivel expandir a topologia da CPU");
    return NULL;
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
