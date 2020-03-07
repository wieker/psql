#include <gtk/gtk.h>

#include "gtkcore.h"

int area_width, area_height;

gboolean
button_press_test_cb(GtkWidget *widget, GdkEventButton *event)
{
	printf("Pressed!\n");
	return 0;
}

int
tkui_gtkcore_init(struct ui_context* ctx, int* argc, char*** argv)
{
	gtk_init(argc, argv);
	
	//create widgets
	ctx->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	/*
	 * Layout Container
	 * 
	ctx->layout = gtk_layout_new(NULL, NULL);
	gtk_container_add (GTK_CONTAINER (ctx->window), ctx->layout);
	* */
                    
	ctx->buttons = gtk_hbutton_box_new();
	ctx->vertical_items = gtk_vbox_new(FALSE, 0);
	
	// add widgets to container hierarchy
	gtk_container_add (GTK_CONTAINER (ctx->window), ctx->vertical_items);

	//connect callbacks to events
	g_signal_connect(ctx->window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	/*g_signal_connect(drawing_area, "expose-event", G_CALLBACK (expose_event), NULL);
	g_signal_connect(drawing_area, "configure-event", G_CALLBACK (on_configure_event), NULL);*/

	
	return 1;
}

int
tkui_gtkcore_start(struct ui_context* ctx)
{
	//start UI
	gtk_box_pack_start((GtkBox*) ctx->vertical_items, ctx->buttons, FALSE, FALSE, 0);
	gtk_widget_show_all(ctx->window);
	gtk_main();
	
	return 1;
}

int
tkui_gtkcore_button(struct ui_context* ctx, int x, int y, button_cb_t cb, char* name)
{
	GtkWidget* button;
	button = gtk_button_new_with_label(name);
	
	/*
	 * Layout Container
	 * 
	gtk_layout_put(GTK_LAYOUT (ctx->layout), button, x, y);
	* */
	
	gtk_box_pack_start((GtkBox*) ctx->buttons, button, TRUE, TRUE, 0);
	g_signal_connect(button, "button-press-event", G_CALLBACK (cb), cb);
	return 1;
}

GtkWidget *
tkui_gtkcore_edlist(struct ui_context* ctx)
{
	GtkWidget* list;
	list = gtk_tree_view_new();
	GtkListStore *list_store;

	list_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_UINT);
	gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(list_store));
	
	gtk_box_pack_start((GtkBox*) ctx->vertical_items, list, TRUE, TRUE, 0);
	//g_signal_connect(list, "button-press-event", G_CALLBACK (cb), cb);
	return list;
}


int
tkui_gtkcore_column(struct ui_context* ctx, char* name)
{
	void* col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(GTK_TREE_VIEW_COLUMN (col), name);
	
	gtk_tree_view_insert_column(GTK_TREE_VIEW (ctx->last), GTK_TREE_VIEW_COLUMN (col), -1);
	
	return 1;
}
