#define _POSIX_C_SOURCE 200809L
#include "cpu.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#ifndef APP_VERSION
#define APP_VERSION "0.1.0"
#endif

typedef struct {
    GtkWidget *model, *usage_value, *usage_bar, *physical_cores;
    GtkWidget *logical_processors, *temperature, *frequency, *processes, *status;
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
    gtk_widget_set_margin_top(box, 14);
    gtk_widget_set_margin_bottom(box, 14);
    gtk_widget_set_margin_start(box, 14);
    gtk_widget_set_margin_end(box, 14);
    gtk_box_append(GTK_BOX(box), heading);
    gtk_box_append(GTK_BOX(box), value);
    gtk_frame_set_child(GTK_FRAME(frame), box);
    return frame;
}
static void update_dashboard(Dashboard *d) {
    cpu_info_t *info = cpu_get_info();
    float usage = cpu_get_usage();
    char usage_error[256] = "";
    float temperature;
    float frequency;
    char text[320];

    if (usage < 0.0f) {
        snprintf(usage_error, sizeof(usage_error), "%s", cpu_get_last_error());
    }
    temperature = cpu_get_temperature();
    frequency = cpu_get_clock_speed();

    if (info != NULL) {
        set_value(d->model, info->model[0] ? info->model : "Modelo indisponível");
        if (info->physical_cores > 0) snprintf(text, sizeof(text), "%d", info->physical_cores);
        else snprintf(text, sizeof(text), "Indisponível");
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

    if (usage >= 0.0f && usage <= 100.0f) {
        snprintf(text, sizeof(text), "%.1f%%", usage);
        set_value(d->usage_value, text);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(d->usage_bar), (double)usage / 100.0);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(d->usage_bar), text);
    } else {
        set_value(d->usage_value, "Indisponível");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(d->usage_bar), 0.0);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(d->usage_bar), "Sem dados");
    }
    if (temperature >= 0.0f) snprintf(text, sizeof(text), "%.1f °C", temperature);
    else snprintf(text, sizeof(text), "Indisponível");
    set_value(d->temperature, text);
    if (frequency >= 0.0f) snprintf(text, sizeof(text), "%.0f MHz", frequency);
    else snprintf(text, sizeof(text), "Indisponível");
    set_value(d->frequency, text);
    if (usage < 0.0f) {
        snprintf(text, sizeof(text), "Uso da CPU indisponível: %s", usage_error);
        set_value(d->status, text);
    } else {
        set_value(d->status, "Atualização automática a cada segundo · dados fornecidos pelo kernel Linux");
    }
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
    GtkWidget *subtitle = gtk_label_new("Monitor de hardware Linux");
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *usage_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *cores_grid = gtk_grid_new();
    GtkWidget *topology_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *status_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *separator;

    gtk_window_set_title(GTK_WINDOW(window), "CPU Reader");
    gtk_window_set_default_size(GTK_WINDOW(window), 760, 560);
    gtk_window_set_child(GTK_WINDOW(window), root);
    gtk_widget_add_css_class(title, "title-2");
    gtk_widget_add_css_class(subtitle, "dim-label");
    gtk_box_append(GTK_BOX(title_box), title);
    gtk_box_append(GTK_BOX(title_box), subtitle);
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header), title_box);
    gtk_box_append(GTK_BOX(root), header);

    gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_widget_set_margin_top(grid, 18);
    gtk_widget_set_margin_bottom(grid, 12);
    gtk_widget_set_margin_start(grid, 18);
    gtk_widget_set_margin_end(grid, 18);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);
    gtk_box_append(GTK_BOX(root), grid);

    d->model = gtk_label_new("Consultando...");
    gtk_label_set_xalign(GTK_LABEL(d->model), 0.0f);
    gtk_label_set_wrap(GTK_LABEL(d->model), TRUE);
    gtk_grid_attach(GTK_GRID(grid), card("PROCESSADOR", d->model), 0, 0, 2, 1);

    d->usage_value = value_label("—");
    d->usage_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(d->usage_bar), TRUE);
    gtk_widget_set_hexpand(d->usage_bar, TRUE);
    gtk_box_append(GTK_BOX(usage_box), d->usage_value);
    gtk_box_append(GTK_BOX(usage_box), d->usage_bar);
    gtk_grid_attach(GTK_GRID(grid), card("USO TOTAL DA CPU", usage_box), 0, 1, 2, 1);

    d->physical_cores = value_label("—");
    d->logical_processors = value_label("—");
    d->processes = value_label("—");
    gtk_grid_set_column_spacing(GTK_GRID(cores_grid), 22);
    gtk_grid_set_row_spacing(GTK_GRID(cores_grid), 8);
    gtk_grid_attach(GTK_GRID(cores_grid), gtk_label_new("Núcleos físicos"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(cores_grid), d->physical_cores, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(cores_grid), gtk_label_new("Processadores lógicos"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(cores_grid), d->logical_processors, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(cores_grid), gtk_label_new("Processos executáveis"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(cores_grid), d->processes, 1, 2, 1, 1);
    gtk_box_append(GTK_BOX(topology_box), cores_grid);
    gtk_grid_attach(GTK_GRID(grid), card("TOPOLOGIA", topology_box), 0, 2, 2, 1);

    d->temperature = value_label("—");
    d->frequency = value_label("—");
    gtk_grid_attach(GTK_GRID(grid), card("TEMPERATURA", d->temperature), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), card("FREQUÊNCIA ATUAL", d->frequency), 1, 3, 1, 1);

    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_append(GTK_BOX(root), separator);
    gtk_widget_set_margin_top(status_box, 8);
    gtk_widget_set_margin_bottom(status_box, 10);
    gtk_widget_set_margin_start(status_box, 18);
    gtk_widget_set_margin_end(status_box, 18);
    d->status = gtk_label_new("Inicializando...");
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
