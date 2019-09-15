#pragma once

#if 0

#include <gtk/gtk.h>
#include <string>
#include "query.hpp"

typedef struct
{
    gboolean   stopTimer;
    GtkWidget* image_grid;
    std::string query;
    std::list<object> obj_list;
    std::unordered_map<uint32_t, object> obj_map;
    thumbnail_size_t thumb_size;
    thumbnail_quality_t thumb_quality;
    GtkWidget* cbo_thumb_quality;
    GtkWidget* cbo_thumb_size;
    int image_grid_cols;

    GtkWidget* lbl_image_page;
    GtkWidget* lbl_audio_page;
    GtkWidget* lbl_video_page;
    GtkWidget* query_entry;
    GtkListStore* audio_list_store;
} gtk_ctx_t;

class window
{
public:
    window(std::string query);

    void show(void);

    static void search_buttom_clicked(GtkWidget* widget, gpointer data);

    static gboolean thumbnail_image_clicked(GtkWidget* event_box, GdkEventButton *event, gpointer data);

    //static void refresh_image_grid()*


    static void activate(GtkApplication* app, gpointer user_data);
    static void size_combo_changed(GtkWidget *widget, gpointer data);
    static void quality_combo_changed(GtkWidget *widget, gpointer data);
    static void clear_button_clicked(GtkWidget *widget, gpointer data);
    static gboolean window_delete(GtkWidget* widget, GdkEvent* event, gpointer data);


private:
    static GtkWidget* create_search_settings_container(gtk_ctx_t* ctx);
    static GtkWidget* create_image_page_container(gtk_ctx_t* ctx);
    static GtkWidget* create_audio_page_container(gtk_ctx_t* ctx);
    static GtkWidget* create_video_page_container(gtk_ctx_t* ctx);

    static void clear_image_grid(gtk_ctx_t* ctx);

    static void show_image_results(gtk_ctx_t* ctx);
    static void show_audio_results(gtk_ctx_t* ctx);
    static void show_video_results(gtk_ctx_t* ctx);
    
    gtk_ctx_t gtk_ctx;

};

#endif
