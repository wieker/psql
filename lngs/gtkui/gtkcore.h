#ifndef GTKCORE_H
#define GTKCORE_H

#include <gtk/gtk.h>

struct ui_context {
	GtkWidget *window;
	GtkWidget *layout;
	GtkWidget *vertical_items;
	GtkWidget *buttons;
	void* dlhandle;
	GtkWidget *last;
};

typedef gboolean (* button_cb_t) (GtkWidget *widget, GdkEventButton *event);

int tkui_gtkcore_init(struct ui_context* ctx, int* argc, char*** argv);
int tkui_gtkcore_start(struct ui_context* ctx);
int tkui_gtkcore_button(struct ui_context* ctx, int x, int y, button_cb_t cb, char* name);
GtkWidget * tkui_gtkcore_edlist(struct ui_context* ctx);
int tkui_gtkcore_column(struct ui_context* ctx, char* name);

#endif
