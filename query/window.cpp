#include "window.hpp"
#include <assert.h>

#define SCREEN_WIDTH (1440)
#define SCREEN_HEIGHT (768)

enum
{
  COL_ARTIST = 0,
  COL_ALBUM,
  COL_TRACK,
  COL_TITLE,
  COL_DURATION,
  COL_GENRE,
  NUM_COLS
};

window::window(std::string query)
{
    gtk_ctx.query = query;
    gtk_ctx.thumb_size = THUMBNAIL_SIZE_320;
    gtk_ctx.thumb_quality = THUMBNAIL_QUALITY_HIGH;
}

void window::show()
{
    GtkApplication* app = gtk_application_new ("com.github.lonezor.mquery", G_APPLICATION_FLAGS_NONE);
    assert(app);

    g_signal_connect (app, "activate", G_CALLBACK (activate), (gpointer)&gtk_ctx);
    char* argv[] = {(char*)"no_arguments"};
    g_application_run (G_APPLICATION (app), 1, argv);
    g_object_unref (app);
}

void window::show_image_results(gtk_ctx_t* ctx)
{
clear_image_grid(ctx);
    int cols = (SCREEN_WIDTH) / (ctx->thumb_size + 20);
    int image_objects = 0;

    int x=0;
    int y=0;
    for (std::list<object>::iterator it=ctx->obj_list.begin(); it != ctx->obj_list.end(); ++it) {
                object_type_t object_type = (*it).get_object_type();
                if (object_type == OBJECT_TYPE_IMAGE) {
                    std::string thumbnail_path = "/home/output/";
                    thumbnail_path += (*it).get_thumbnail_path(ctx->thumb_size, ctx->thumb_quality);
                  //  printf("thumbnail_path %s\n", thumbnail_path.c_str());

                    GtkWidget* gtkImage = gtk_image_new_from_file (thumbnail_path.c_str());

                        GtkWidget* image_event_box = gtk_event_box_new();

                        gtk_container_add (GTK_CONTAINER (image_event_box), gtkImage);

                        g_signal_connect (G_OBJECT (image_event_box),
                        "button_press_event",
                        G_CALLBACK (thumbnail_image_clicked),
                        ctx);
                         
                         gtk_grid_attach (GTK_GRID (ctx->image_grid), image_event_box, x, y, 1, 1);

                         if (x >= cols - 1) {
                             x = 0;
                             y++;
                         } else {
                             x++;
                         }
                    image_objects++;
                }
          }

        ctx->image_grid_cols = cols;

        if (image_objects) {
            char label[128];
            snprintf(label, sizeof(label), "Image (%d %s)", image_objects, image_objects > 1 ? "matches" : "match");
            gtk_label_set_text((GtkLabel*)ctx->lbl_image_page, label);
        }

      gtk_widget_show_all(ctx->image_grid);
}

void window::show_audio_results(gtk_ctx_t* ctx)
{
    int audio_objects = 0;
    GtkTreeIter    iter;

    for (std::list<object>::iterator it=ctx->obj_list.begin(); it != ctx->obj_list.end(); ++it) {
                object_type_t object_type = (*it).get_object_type();
                if (object_type == OBJECT_TYPE_AUDIO) {
                     GtkTreeIter    iter;
                    gtk_list_store_append (ctx->audio_list_store, &iter);
                    gtk_list_store_set (ctx->audio_list_store, &iter,
                        COL_ARTIST, (*it).audio_metadata.artist.c_str(),
                        COL_ALBUM, (*it).audio_metadata.album.c_str(),
                         COL_TRACK, (*it).audio_metadata.track,
                         COL_TITLE, (*it).audio_metadata.title.c_str(),
                         COL_DURATION, (*it).audio_metadata.duration.c_str(),
                         COL_GENRE, (*it).audio_metadata.genre.c_str(),
                         -1);
                    audio_objects++;
                }
    }

    if (audio_objects) {
        char label[128];
        snprintf(label, sizeof(label), "Audio (%d %s)", audio_objects, audio_objects > 1 ? "matches" : "match");
        gtk_label_set_text((GtkLabel*)ctx->lbl_audio_page, label);
    }
}

void window::show_video_results(gtk_ctx_t* ctx)
{

}

void
window::search_buttom_clicked(GtkWidget *widget, gpointer data)
{
    gtk_ctx_t* ctx = (gtk_ctx_t*)data;

    printf("search button clicked\n");

    ctx->query = std::string(gtk_entry_get_text((GtkEntry*)ctx->query_entry));

    ctx->obj_list.clear();
    ctx->obj_map.clear();

    query_execute(ctx->query, ctx->obj_list, ctx->obj_map);

    show_image_results(ctx);
    show_audio_results(ctx);
    show_video_results(ctx);
}

gboolean
window::thumbnail_image_clicked (GtkWidget      *event_box,
                       GdkEventButton *event,
                       gpointer        data)
{
  g_print ("Event box clicked at coordinates %f,%f\n",
           event->x, event->y);
  return TRUE; // event processed
}

void
window::size_combo_changed(GtkWidget *widget, gpointer data)
{
    gtk_ctx_t* ctx = (gtk_ctx_t*)data;


          gchar* text = gtk_combo_box_text_get_active_text((GtkComboBoxText*)ctx->cbo_thumb_size);
    if (strstr(text, "160")) {
        ctx->thumb_size = THUMBNAIL_SIZE_160;
            printf("%s 160\n", __func__);
    }

    else if (strstr(text, "320")) {
        ctx->thumb_size = THUMBNAIL_SIZE_320;
            printf("%s 320\n", __func__);
    }

    else if (strstr(text, "640")) {
        ctx->thumb_size = THUMBNAIL_SIZE_640;
            printf("%s 640\n", __func__);
    }

    else if (strstr(text, "1024")) {
        ctx->thumb_size = THUMBNAIL_SIZE_1024;
            printf("%s 1024\n", __func__);
    }
}

void
window::clear_image_grid(gtk_ctx_t* ctx)
{
    int i;
    for(i=0; i < ctx->image_grid_cols; i++) {
        gtk_grid_remove_column((GtkGrid*)ctx->image_grid, 0);
    }

    gtk_widget_show_all(ctx->image_grid);
}

void
window::clear_button_clicked(GtkWidget *widget, gpointer data)
{
    gtk_ctx_t* ctx = (gtk_ctx_t*)data;

    clear_image_grid(ctx);
    gtk_label_set_text((GtkLabel*)ctx->lbl_image_page, "Image");
    gtk_label_set_text((GtkLabel*)ctx->lbl_audio_page, "Audio");
    gtk_label_set_text((GtkLabel*)ctx->lbl_video_page, "Video");

    printf("%s\n", __func__);

}

void
window::quality_combo_changed(GtkWidget *widget, gpointer data)
{
    gtk_ctx_t* ctx = (gtk_ctx_t*)data;

    gchar* text = gtk_combo_box_text_get_active_text((GtkComboBoxText*)ctx->cbo_thumb_quality);
    if (strstr(text, "High")) {
        ctx->thumb_quality = THUMBNAIL_QUALITY_HIGH;
            printf("%s High\n", __func__);
    }

    else if (strstr(text, "Low")) {
        ctx->thumb_quality = THUMBNAIL_QUALITY_LOW;
        printf("%s Low\n", __func__);
    }


}

GtkWidget* window::create_search_settings_container(gtk_ctx_t* ctx)
{
    // Search Criteria Frame: Column #1 - Thumbnail size
    ctx->cbo_thumb_size = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text((GtkComboBoxText*)ctx->cbo_thumb_size, "160px");
    gtk_combo_box_text_append_text((GtkComboBoxText*)ctx->cbo_thumb_size, "320px");
    gtk_combo_box_text_append_text((GtkComboBoxText*)ctx->cbo_thumb_size, "640px");
    gtk_combo_box_text_append_text((GtkComboBoxText*)ctx->cbo_thumb_size, "1024px");
    gtk_combo_box_set_active((GtkComboBox*)ctx->cbo_thumb_size, 1);
    g_signal_connect (ctx->cbo_thumb_size, "changed", G_CALLBACK (size_combo_changed), ctx);

    // Search Criteria Frame: Column #2 - Thumbnail quality
    ctx->cbo_thumb_quality = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text((GtkComboBoxText*)ctx->cbo_thumb_quality, "High quality");
    gtk_combo_box_text_append_text((GtkComboBoxText*)ctx->cbo_thumb_quality, "Low quality");
    gtk_combo_box_set_active((GtkComboBox*)ctx->cbo_thumb_quality, 0);
    g_signal_connect (ctx->cbo_thumb_quality, "changed", G_CALLBACK (quality_combo_changed), ctx);

    // Search Criteria Frame: Column #3 - Search entry
    ctx->query_entry = gtk_entry_new();
    gtk_widget_set_size_request((GtkWidget*)ctx->query_entry, 600, 30);
    gtk_entry_set_text((GtkEntry*)ctx->query_entry, ctx->query.c_str());

    // Search Criteria Frame: Column #3 - Show button
    GtkWidget* search_button = gtk_button_new_with_label("Show");
    g_signal_connect (search_button, "clicked", G_CALLBACK (search_buttom_clicked), ctx);
    gtk_widget_set_size_request((GtkWidget*)search_button, 50, 30);

    // Search Criteria Frame: Column #3 - Clear button
    GtkWidget* clear_button = gtk_button_new_with_label("Clear");
    g_signal_connect (clear_button, "clicked", G_CALLBACK (clear_button_clicked), ctx);
    gtk_widget_set_size_request((GtkWidget*)clear_button, 50, 30);

    // Pack in horizontal box
    GtkWidget* hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start (GTK_BOX (hbox), ctx->cbo_thumb_size, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbox), ctx->cbo_thumb_quality, FALSE, FALSE, 0);  
    gtk_box_pack_start (GTK_BOX (hbox), ctx->query_entry, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbox), search_button, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hbox), clear_button, FALSE, FALSE, 0);

    // Pack in vertical frame box
    GtkWidget* vbox_criterias_frame = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start (GTK_BOX (vbox_criterias_frame), hbox, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (vbox_criterias_frame), 10);

     // Create 'search criterias' frame
    GtkWidget* criterias_frame = gtk_frame_new ("Search criterias");

    gtk_container_add (GTK_CONTAINER (criterias_frame), vbox_criterias_frame);

     // Pack all in in vertical box
    GtkWidget* vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start (GTK_BOX (vbox), criterias_frame, FALSE, FALSE, 0);

    gtk_container_set_border_width (GTK_CONTAINER (vbox), 10);

    return vbox;
}

GtkWidget* window::create_image_page_container(gtk_ctx_t* ctx)
{
    

    // Create image grid
    ctx->image_grid = gtk_grid_new();
    gtk_grid_set_column_spacing((GtkGrid*)ctx->image_grid, 25);
    gtk_grid_set_row_spacing((GtkGrid*)ctx->image_grid, 25);
    gtk_widget_set_halign (ctx->image_grid, GTK_ALIGN_CENTER);

    // Put grid inside scrolled window
    GtkWidget* scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request(scrolled_window, 10, SCREEN_HEIGHT );
    gtk_container_add (GTK_CONTAINER (scrolled_window), ctx->image_grid);

    // Padding
    GtkWidget* top_padding = gtk_event_box_new();
    gtk_widget_set_size_request((GtkWidget*)top_padding, 100, 10);

    GtkWidget* bottom_padding = gtk_event_box_new();
    gtk_widget_set_size_request((GtkWidget*)bottom_padding, 100, 10);

    // Pack in vertical box
    GtkWidget* vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start (GTK_BOX (vbox), top_padding, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), bottom_padding, FALSE, FALSE, 0);

    return vbox;
}





GtkWidget* window::create_audio_page_container(gtk_ctx_t* ctx)
{
    GtkCellRenderer* renderer;
    GtkTreeModel*     model;
    GtkWidget*        view;

    view = gtk_tree_view_new ();

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Artist",  
                                               renderer,
                                               "text", COL_ARTIST,
                                               NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Album",  
                                               renderer,
                                               "text", COL_ALBUM,
                                               NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Track",  
                                               renderer,
                                               "text", COL_TRACK,
                                               NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Title",  
                                               renderer,
                                               "text", COL_TITLE,
                                               NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Duration",  
                                               renderer,
                                               "text", COL_DURATION,
                                               NULL);
  
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Genre",  
                                               renderer,
                                               "text", COL_GENRE,
                                               NULL);

    ctx->audio_list_store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    model = GTK_TREE_MODEL (ctx->audio_list_store);
    gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

    g_object_unref (model);

    GtkWidget* vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start (GTK_BOX (vbox), view, FALSE, FALSE, 0);

    return vbox;
}

GtkWidget* window::create_video_page_container(gtk_ctx_t* ctx)
{
    GtkWidget* vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
//http://zetcode.com/gui/gtk2/gtktreeview/
    return vbox;
}

void window::activate(GtkApplication* app, gpointer user_data) // static: callback function
{
    gtk_ctx_t* ctx = (gtk_ctx_t*)user_data;
    assert(ctx);

    GtkWidget* window = gtk_application_window_new (app);
    assert(window);

    GtkWidget* vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget* search_settings_vbox = create_search_settings_container(ctx);
    
 
    ctx->lbl_image_page = gtk_label_new("Image");
    ctx->lbl_audio_page = gtk_label_new("Audio");
    ctx->lbl_video_page = gtk_label_new("Video");

    GtkWidget* notebook = gtk_notebook_new();
    gtk_notebook_append_page ((GtkNotebook*)notebook,
                          create_image_page_container(ctx),
                          ctx->lbl_image_page);

gtk_notebook_append_page ((GtkNotebook*)notebook,
                          create_audio_page_container(ctx),
                          ctx->lbl_audio_page);

    gtk_notebook_append_page ((GtkNotebook*)notebook,
                          create_video_page_container(ctx),
                          ctx->lbl_video_page);





    gtk_box_pack_start (GTK_BOX (vbox), search_settings_vbox, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), notebook, FALSE, FALSE, 0);

        gtk_container_add (GTK_CONTAINER (window), vbox);


    gtk_window_set_title (GTK_WINDOW (window), "mquery");
    gtk_window_set_default_size (GTK_WINDOW (window), SCREEN_WIDTH, SCREEN_HEIGHT);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_widget_show_all (window);

    g_signal_connect (window, "delete-event", G_CALLBACK (window_delete), ctx);
}

gboolean window::window_delete(GtkWidget* widget, GdkEvent* event, gpointer data)
{
    gtk_ctx_t* ctx = (gtk_ctx_t*)data;
    assert(ctx);

    return FALSE;
}