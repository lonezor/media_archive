#pragma once

#include <gtk/gtk.h>

typedef struct
{
    gboolean   stopTimer;
    GtkWidget* image_grid;
} gtk_ctx_t;

class window
{
public:
    window();

    void show(void);

    static void search_buttom_clicked(GtkWidget* widget, gpointer data);

    static gboolean thumbnail_image_clicked(GtkWidget* event_box, GdkEventButton *event, gpointer data);

    //static void refresh_image_grid()*


    static void activate(GtkApplication* app, gpointer user_data);
    static gboolean window_delete(GtkWidget* widget, GdkEvent* event, gpointer data);

private:
    static GtkWidget* create_search_page_container(gtk_ctx_t* ctx);
    static GtkWidget* create_tag_page_container(gtk_ctx_t* ctx);
    
    gtk_ctx_t gtk_ctx;

};