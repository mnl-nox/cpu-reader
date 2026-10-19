#define _POSIX_C_SOURCE 200809L
#include "cpu.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/statvfs.h>

#ifndef APP_VERSION
#define APP_VERSION "0.1.0"
#endif

#define MAX_CPUS 1024

typedef struct {
    unsigned long long total;
    unsigned long long idle;
    int valid;
} CpuSample;

typedef struct {
    GtkWidget *label;
    GtkWidget *bar;
    CpuSample previous;
} CoreRow;

typedef struct {
    GtkWidget *model, *usage_value, *usage_bar, *physical_cores;
    GtkWidget *logical_processors, *temperature, *frequency, *processes;
    GtkWidget *memory_label, *memory_bar, *swap_label, *swap_bar;
    GtkWidget *load_label, *disk_label, *network_label, *status;
    GtkWidget *cores_grid;
    CoreRow cores[MAX_CPUS];
    int core_count;
    uint64_t previous_rx, previous_tx;
    int network_sample_valid;
    guint refresh_source;
} Dashboard;

static void set_value(GtkWidget *label, const char *value) {
    gtk_label_set_text(GTK_LABEL(label), value);
}

static GtkWidget *value_label(const char *initial) {
    GtkWidget *label = gtk_label_new(initial);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_widget_add_css_class(label, "title-2");
    return label;
}

static GtkWidget *card(const char *title, GtkWidget *value) {
    GtkWidget *frame = gtk_frame_new(NULL);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *heading = gtk_label_new(title);
    gtk_label_set_xalign(GTK_LABEL(heading), 0.0f);
    gtk_widget_add_css_class(heading, "heading");
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 14);
    gtk_widget_set_margin_end(box, 14);
    gtk_box_append(GTK_BOX(box), heading);
    gtk_box_append(GTK_BOX(box), value);
    gtk_frame_set_child(GTK_FRAME(frame), box);
    return frame;
}

static int read_cpu_samples(CpuSample samples[MAX_CPUS], int *count,
                            float *aggregate) {
    FILE *file = fopen("/proc/stat", "r");
    char line[512];
    int found = 0;
    unsigned long long total, user, nice, system, idle, iowait;
    unsigned long long irq, softirq, steal, guest, guest_nice;
    if (!file) return -1;

    *aggregate = -1.0f;
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "cpu ", 4) == 0) {
            int n = sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal,
                &guest, &guest_nice);
            if (n >= 4) {
                total = user + nice + system + idle + iowait + irq + softirq + steal;
                (void)guest; (void)guest_nice;
                /* The first read is a baseline, not a meaningful percentage. */
                static unsigned long long prev_total, prev_idle;
                static int prev_valid;
                if (prev_valid && total > prev_total && idle + iowait >= prev_idle) {
                    unsigned long long dt = total - prev_total;
                    unsigned long long di = idle + iowait - prev_idle;
                    *aggregate = 100.0f * (float)(dt - (di > dt ? dt : di)) / (float)dt;
                }
                prev_total = total;
                prev_idle = idle + iowait;
                prev_valid = 1;
            }
            continue;
        }
        if (strncmp(line, "cpu", 3) == 0 && line[3] >= '0' && line[3] <= '9') {
            unsigned int id;
            int n = sscanf(line, "cpu%u %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                &id, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal,
                &guest, &guest_nice);
            if (n >= 5 && id < MAX_CPUS) {
                CpuSample *s = &samples[id];
                s->total = user + nice + system + idle + iowait + irq + softirq + steal;
                s->idle = idle + iowait;
                s->valid = 1;
                if ((int)id + 1 > found) found = (int)id + 1;
            }
        }
    }
    fclose(file);
    *count = found;
    return 0;
}

static void format_bytes(uint64_t bytes, char *out, size_t size) {
    const char *units[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    double value = (double)bytes;
    int unit = 0;
    while (value >= 1024.0 && unit < 4) {
        value /= 1024.0;
        unit++;
    }
    snprintf(out, size, "%.1f %s", value, units[unit]);
}

static void update_memory(Dashboard *d) {
    FILE *file = fopen("/proc/meminfo", "r");
    char line[256], text[256];
    unsigned long long total = 0, available = 0, swap_total = 0, swap_free = 0;
    if (!file) {
        set_value(d->memory_label, "Indisponível");
        set_value(d->swap_label, "Indisponível");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(d->memory_bar), 0.0);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(d->swap_bar), 0.0);
        return;
    }
    while (fgets(line, sizeof(line), file)) {
        unsigned long long value;
        if (sscanf(line, "MemTotal: %llu kB", &value) == 1) total = value;
        else if (sscanf(line, "MemAvailable: %llu kB", &value) == 1) available = value;
        else if (sscanf(line, "SwapTotal: %llu kB", &value) == 1) swap_total = value;
        else if (sscanf(line, "SwapFree: %llu kB", &value) == 1) swap_free = value;
    }
    fclose(file);
    if (total > 0 && available <= total) {
        double used = 100.0 * (double)(total - available) / (double)total;
        snprintf(text, sizeof(text), "%.1f%% usado · ", used);
        char a[32], b[32];
        format_bytes((total - available) * 1024ULL, a, sizeof(a));
        format_bytes(total * 1024ULL, b, sizeof(b));
        snprintf(text + strlen(text), sizeof(text) - strlen(text), "%s / %s", a, b);
        set_value(d->memory_label, text);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(d->memory_bar), used / 100.0);
    } else {
        set_value(d->memory_label, "Indisponível");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(d->memory_bar), 0.0);
    }
    if (swap_total > 0 && swap_free <= swap_total) {
        double used = 100.0 * (double)(swap_total - swap_free) / (double)swap_total;
        char a[32], b[32];
        format_bytes((swap_total - swap_free) * 1024ULL, a, sizeof(a));
        format_bytes(swap_total * 1024ULL, b, sizeof(b));
        snprintf(text, sizeof(text), "%.1f%% usado · %s / %s", used, a, b);
        set_value(d->swap_label, text);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(d->swap_bar), used / 100.0);
    } else {
        set_value(d->swap_label, swap_total == 0 ? "Sem swap configurada" : "Indisponível");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(d->swap_bar), 0.0);
    }
}

static void update_load(Dashboard *d) {
    FILE *f = fopen("/proc/loadavg", "r");
    char text[256];
    double a, b, c;
    int running = 0, total = 0;
    if (!f) {
        set_value(d->load_label, "Indisponível");
        return;
    }
    if (fscanf(f, "%lf %lf %lf %d/%d", &a, &b, &c, &running, &total) == 5) {
        snprintf(text, sizeof(text), "1 min %.2f · 5 min %.2f · 15 min %.2f · executáveis %d/%d",
                 a, b, c, running, total);
        set_value(d->load_label, text);
    } else set_value(d->load_label, "Indisponível");
    fclose(f);
}

static void update_disk(Dashboard *d) {
    struct statvfs fs;
    char text[256], used_text[32], total_text[32];
    if (statvfs("/", &fs) != 0 || fs.f_blocks == 0) {
        set_value(d->disk_label, "Uso do disco raiz indisponível");
        return;
    }
    uint64_t total = (uint64_t)fs.f_blocks * fs.f_frsize;
    uint64_t free_bytes = (uint64_t)fs.f_bavail * fs.f_frsize;
    uint64_t used = total > free_bytes ? total - free_bytes : 0;
    format_bytes(used, used_text, sizeof(used_text));
    format_bytes(total, total_text, sizeof(total_text));
    snprintf(text, sizeof(text), "Raiz (/): %s / %s (%.1f%% usado)",
             used_text, total_text, total ? 100.0 * (double)used / (double)total : 0.0);
    set_value(d->disk_label, text);
}

static void update_network(Dashboard *d) {
    FILE *f = fopen("/proc/net/dev", "r");
    char line[512], name[64], text[256];
    uint64_t rx = 0, tx = 0;
    int valid = 0;
    if (!f) {
        set_value(d->network_label, "Estatísticas de rede indisponíveis");
        return;
    }
    (void)fgets(line, sizeof(line), f);
    (void)fgets(line, sizeof(line), f);
    while (fgets(line, sizeof(line), f)) {
        unsigned long long rxb, rxp, rxe, rxd, rxf, rxfr, rxco, rxmu;
        unsigned long long txb, txp, txe, txd, txf, txco, txca, txmu;
        char *colon = strchr(line, ':');
        if (!colon) continue;
        *colon = '\0';
        if (sscanf(line, " %63s", name) != 1 || strcmp(name, "lo") == 0) continue;
        if (sscanf(colon + 1, " %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
            &rxb, &rxp, &rxe, &rxd, &rxf, &rxfr, &rxco, &rxmu,
            &txb, &txp, &txe, &txd, &txf, &txco, &txca, &txmu) == 16) {
            rx += rxb; tx += txb; valid = 1;
        }
    }
    fclose(f);
    if (!valid) {
        set_value(d->network_label, "Interfaces de rede indisponíveis");
        return;
    }
    if (d->network_sample_valid && rx >= d->previous_rx && tx >= d->previous_tx) {
        char r[32], t[32];
        format_bytes(rx - d->previous_rx, r, sizeof(r));
        format_bytes(tx - d->previous_tx, t, sizeof(t));
        snprintf(text, sizeof(text), "Recebido: %s/s · Enviado: %s/s", r, t);
    } else {
        snprintf(text, sizeof(text), "Coletando amostra de rede…");
    }
    d->previous_rx = rx;
    d->previous_tx = tx;
    d->network_sample_valid = 1;
    set_value(d->network_label, text);
}

static void update_cores(Dashboard *d) {
    CpuSample samples[MAX_CPUS] = {{0}};
    int count = 0, i;
    float aggregate = -1.0f;
    char text[128];
    if (read_cpu_samples(samples, &count, &aggregate) != 0) {
        set_value(d->usage_value, "Indisponível");
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(d->usage_bar), "Sem dados");
        return;
    }
    if (aggregate >= 0.0f && aggregate <= 100.0f) {
        snprintf(text, sizeof(text), "%.1f%%", aggregate);
        set_value(d->usage_value, text);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(d->usage_bar), aggregate / 100.0);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(d->usage_bar), text);
    } else {
        set_value(d->usage_value, "Aguardando amostra");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(d->usage_bar), 0.0);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(d->usage_bar), "Aguardando amostra");
    }
    for (i = 0; i < d->core_count; i++) {
        CoreRow *row = &d->cores[i];
        if (i >= count || !samples[i].valid) {
            set_value(row->label, "Offline / indisponível");
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(row->bar), 0.0);
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(row->bar), "—");
            continue;
        }
        if (row->previous.valid && samples[i].total > row->previous.total &&
            samples[i].idle >= row->previous.idle) {
            unsigned long long dt = samples[i].total - row->previous.total;
            unsigned long long di = samples[i].idle - row->previous.idle;
            float usage = 100.0f * (float)(dt - (di > dt ? dt : di)) / (float)dt;
            if (usage < 0.0f) usage = 0.0f;
            if (usage > 100.0f) usage = 100.0f;
            snprintf(text, sizeof(text), "CPU %d · %.1f%%", i, usage);
            set_value(row->label, text);
            snprintf(text, sizeof(text), "%.1f%%", usage);
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(row->bar), usage / 100.0);
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(row->bar), text);
        } else {
            snprintf(text, sizeof(text), "CPU %d · aguardando amostra", i);
            set_value(row->label, text);
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(row->bar), 0.0);
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(row->bar), "—");
        }
        row->previous = samples[i];
    }
}

static void update_dashboard(Dashboard *d) {
    cpu_info_t *info = cpu_get_info();
    float temperature = cpu_get_temperature();
    float frequency = cpu_get_clock_speed();
    char text[320];

    if (info) {
        set_value(d->model, info->model[0] ? info->model : "Modelo indisponível");
        snprintf(text, sizeof(text), "%s", info->physical_cores > 0 ? "" : "Indisponível");
        if (info->physical_cores > 0) snprintf(text, sizeof(text), "%d", info->physical_cores);
        set_value(d->physical_cores, text);
        if (info->logical_processors > 0) snprintf(text, sizeof(text), "%d", info->logical_processors);
        else snprintf(text, sizeof(text), "Indisponível");
        set_value(d->logical_processors, text);
        if (info->active_processes >= 0) snprintf(text, sizeof(text), "%d", info->active_processes);
        else snprintf(text, sizeof(text), "Indisponível");
        set_value(d->processes, text);
        cpu_free_info(info);
    } else {
        snprintf(text, sizeof(text), "Falha ao consultar CPU: %s", cpu_get_last_error());
        set_value(d->model, text);
    }
    if (temperature >= 0.0f) snprintf(text, sizeof(text), "%.1f °C", temperature);
    else snprintf(text, sizeof(text), "Indisponível (sensor/driver não exposto)");
    set_value(d->temperature, text);
    if (frequency > 0.0f) snprintf(text, sizeof(text), "%.0f MHz", frequency);
    else snprintf(text, sizeof(text), "Indisponível");
    set_value(d->frequency, text);

    update_cores(d);
    update_memory(d);
    update_load(d);
    update_disk(d);
    update_network(d);
    set_value(d->status, "Atualização a cada segundo · dados locais do kernel Linux; métricas dependem do hardware e das permissões.");
}

static gboolean refresh_dashboard(gpointer data) {
    update_dashboard((Dashboard *)data);
    return G_SOURCE_CONTINUE;
}

static void on_shutdown(GApplication *app, gpointer data) {
    Dashboard *d = (Dashboard *)data;
    (void)app;
    if (d->refresh_source) {
        g_source_remove(d->refresh_source);
        d->refresh_source = 0;
    }
    cpu_cleanup();
}

static void on_activate(GtkApplication *app, gpointer data) {
    Dashboard *d = (Dashboard *)data;
    GtkWidget *window = gtk_application_window_new(app);
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_header_bar_new();
    GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *title = gtk_label_new("CPU Reader");
    GtkWidget *subtitle = gtk_label_new("Monitor de hardware e sistema Linux");
    GtkWidget *scroller = gtk_scrolled_window_new();
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *usage_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *cores_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *memory_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *swap_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *system_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *status_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    int i;

    gtk_window_set_title(GTK_WINDOW(window), "CPU Reader");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 760);
    gtk_window_set_child(GTK_WINDOW(window), root);
    gtk_widget_add_css_class(title, "title-2");
    gtk_widget_add_css_class(subtitle, "dim-label");
    gtk_box_append(GTK_BOX(title_box), title);
    gtk_box_append(GTK_BOX(title_box), subtitle);
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header), title_box);
    gtk_box_append(GTK_BOX(root), header);

    gtk_widget_set_vexpand(scroller, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), grid);
    gtk_box_append(GTK_BOX(root), scroller);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_widget_set_margin_top(grid, 16);
    gtk_widget_set_margin_bottom(grid, 16);
    gtk_widget_set_margin_start(grid, 16);
    gtk_widget_set_margin_end(grid, 16);

    d->model = gtk_label_new("Consultando…");
    gtk_label_set_xalign(GTK_LABEL(d->model), 0.0f);
    gtk_label_set_wrap(GTK_LABEL(d->model), TRUE);
    gtk_grid_attach(GTK_GRID(grid), card("PROCESSADOR", d->model), 0, 0, 2, 1);

    d->usage_value = value_label("Aguardando amostra");
    d->usage_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(d->usage_bar), TRUE);
    gtk_widget_set_hexpand(d->usage_bar, TRUE);
    gtk_box_append(GTK_BOX(usage_box), d->usage_value);
    gtk_box_append(GTK_BOX(usage_box), d->usage_bar);
    gtk_grid_attach(GTK_GRID(grid), card("USO TOTAL DA CPU", usage_box), 0, 1, 2, 1);

    d->physical_cores = value_label("—");
    d->logical_processors = value_label("—");
    d->processes = value_label("—");
    GtkWidget *topology = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(topology), 6);
    gtk_grid_set_column_spacing(GTK_GRID(topology), 18);
    gtk_grid_attach(GTK_GRID(topology), gtk_label_new("Núcleos físicos"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(topology), d->physical_cores, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(topology), gtk_label_new("Processadores lógicos"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(topology), d->logical_processors, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(topology), gtk_label_new("Processos executáveis"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(topology), d->processes, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), card("TOPOLOGIA", topology), 0, 2, 1, 1);

    d->temperature = value_label("—");
    d->frequency = value_label("—");
    GtkWidget *sensor_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_append(GTK_BOX(sensor_box), gtk_label_new("Temperatura"));
    gtk_box_append(GTK_BOX(sensor_box), d->temperature);
    gtk_box_append(GTK_BOX(sensor_box), gtk_label_new("Frequência atual"));
    gtk_box_append(GTK_BOX(sensor_box), d->frequency);
    gtk_grid_attach(GTK_GRID(grid), card("SENSORES DA CPU", sensor_box), 1, 2, 1, 1);

    d->cores_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(d->cores_grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(d->cores_grid), 10);
    gtk_widget_set_hexpand(d->cores_grid, TRUE);
    gtk_box_append(GTK_BOX(cores_box), d->cores_grid);
    gtk_grid_attach(GTK_GRID(grid), card("UTILIZAÇÃO POR NÚCLEO LÓGICO", cores_box), 0, 3, 2, 1);

    d->memory_label = gtk_label_new("Consultando…");
    d->memory_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(d->memory_bar), TRUE);
    gtk_box_append(GTK_BOX(memory_box), d->memory_label);
    gtk_box_append(GTK_BOX(memory_box), d->memory_bar);
    gtk_grid_attach(GTK_GRID(grid), card("MEMÓRIA RAM", memory_box), 0, 4, 1, 1);

    d->swap_label = gtk_label_new("Consultando…");
    d->swap_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(d->swap_bar), TRUE);
    gtk_box_append(GTK_BOX(swap_box), d->swap_label);
    gtk_box_append(GTK_BOX(swap_box), d->swap_bar);
    gtk_grid_attach(GTK_GRID(grid), card("SWAP", swap_box), 1, 4, 1, 1);

    d->load_label = gtk_label_new("Consultando…");
    d->disk_label = gtk_label_new("Consultando…");
    d->network_label = gtk_label_new("Consultando…");
    gtk_label_set_xalign(GTK_LABEL(d->load_label), 0.0f);
    gtk_label_set_xalign(GTK_LABEL(d->disk_label), 0.0f);
    gtk_label_set_xalign(GTK_LABEL(d->network_label), 0.0f);
    gtk_box_append(GTK_BOX(system_box), d->load_label);
    gtk_box_append(GTK_BOX(system_box), d->disk_label);
    gtk_box_append(GTK_BOX(system_box), d->network_label);
    gtk_grid_attach(GTK_GRID(grid), card("SISTEMA", system_box), 0, 5, 2, 1);

    d->core_count = 0;
    {
        CpuSample samples[MAX_CPUS] = {{0}};
        float aggregate;
        if (read_cpu_samples(samples, &d->core_count, &aggregate) != 0) d->core_count = 0;
        if (d->core_count > MAX_CPUS) d->core_count = MAX_CPUS;
    }
    for (i = 0; i < d->core_count; i++) {
        GtkWidget *label = gtk_label_new("Aguardando amostra");
        GtkWidget *bar = gtk_progress_bar_new();
        gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
        gtk_widget_set_hexpand(bar, TRUE);
        gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(bar), TRUE);
        d->cores[i].label = label;
        d->cores[i].bar = bar;
        gtk_grid_attach(GTK_GRID(d->cores_grid), label, 0, i, 1, 1);
        gtk_grid_attach(GTK_GRID(d->cores_grid), bar, 1, i, 1, 1);
    }
    if (d->core_count == 0) {
        gtk_grid_attach(GTK_GRID(d->cores_grid), gtk_label_new("Núcleos indisponíveis em /proc/stat"), 0, 0, 2, 1);
    }

    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_append(GTK_BOX(root), separator);
    gtk_widget_set_margin_top(status_box, 8);
    gtk_widget_set_margin_bottom(status_box, 10);
    gtk_widget_set_margin_start(status_box, 16);
    gtk_widget_set_margin_end(status_box, 16);
    d->status = gtk_label_new("Inicializando…");
    gtk_label_set_xalign(GTK_LABEL(d->status), 0.0f);
    gtk_label_set_wrap(GTK_LABEL(d->status), TRUE);
    gtk_widget_add_css_class(d->status, "dim-label");
    gtk_box_append(GTK_BOX(status_box), d->status);
    gtk_box_append(GTK_BOX(root), status_box);

    update_dashboard(d);
    d->refresh_source = g_timeout_add_seconds(1, refresh_dashboard, d);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    Dashboard dashboard = {0};
    int result;
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        printf("CPU Reader %s\n", APP_VERSION);
        return 0;
    }
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("Uso: cpu-reader [--version|--help]\n");
        return 0;
    }
    if (cpu_init() != 0) {
        fprintf(stderr, "CPU Reader: não foi possível inicializar a biblioteca.\n");
        return 1;
    }
    app = gtk_application_new("io.github.mnl_nox.CPUReader", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), &dashboard);
    g_signal_connect(app, "shutdown", G_CALLBACK(on_shutdown), &dashboard);
    result = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return result;
}
