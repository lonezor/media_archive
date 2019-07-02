#include "window.hpp"
#include <assert.h>

#define SCREEN_WIDTH (1440)
#define SCREEN_HEIGHT (768)

window::window()
{

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

void
window::search_buttom_clicked(GtkWidget *widget, gpointer data)
{
    gtk_ctx_t* ctx = (gtk_ctx_t*)data;

    printf("search button clicked\n");

    GtkWidget* gtkImage_01 = gtk_image_new_from_file ("/home/output/fe0931f1e411d43d1782ded88784031e8157ae02_1024.jpg");
    GtkWidget* gtkImage_02 = gtk_image_new_from_file ("/home/output/e9dcbc3d8ee5823c90e865af01a4af93be1caad0_1024.jpg");
    GtkWidget* gtkImage_03 = gtk_image_new_from_file ("/home/output/d0cf3e641645c4b33cfc301e8a04e79846c176ae_1024.jpg");
    GtkWidget* gtkImage_04 = gtk_image_new_from_file ("/home/output/b86f1b57ee750587265d66f58b7da1ce70c36b34_1024.jpg");
    GtkWidget* gtkImage_05 = gtk_image_new_from_file ("/home/output/322f81add1e1667353a23d4bc1d3a8755f3c2f9e_1024.jpg");
    GtkWidget* gtkImage_06 = gtk_image_new_from_file ("/home/output/26244da29c59b6202fdcb96c34c1cd9081e0f6ad_1024.jpg");
    GtkWidget* gtkImage_07 = gtk_image_new_from_file ("/home/output/19a30589697d379ccae319273bf42cdc8e1df5b4_1024.jpg");
    GtkWidget* gtkImage_08 = gtk_image_new_from_file ("/home/output/16beba1d78186279670697ad958eb0efb9bd31f8_1024.jpg");

    GtkWidget* image_event_box = gtk_event_box_new();

    gtk_container_add (GTK_CONTAINER (image_event_box), gtkImage_01);

   g_signal_connect (G_OBJECT (image_event_box),
                    "button_press_event",
                    G_CALLBACK (thumbnail_image_clicked),
                    ctx);

    gtk_grid_attach (GTK_GRID (ctx->image_grid), image_event_box, 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (ctx->image_grid), gtkImage_02, 0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (ctx->image_grid), gtkImage_03, 0, 2, 1, 1);
    gtk_grid_attach (GTK_GRID (ctx->image_grid), gtkImage_04, 0, 3, 1, 1);
    gtk_grid_attach (GTK_GRID (ctx->image_grid), gtkImage_05, 1, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (ctx->image_grid), gtkImage_06, 1, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (ctx->image_grid), gtkImage_07, 1, 2, 1, 1);
    gtk_grid_attach (GTK_GRID (ctx->image_grid), gtkImage_08, 1, 3, 1, 1);

      gtk_widget_show_all(ctx->image_grid);
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

GtkWidget* window::create_search_page_container(gtk_ctx_t* ctx)
{
    // Search Criteria Frame: Column #1 - Thumbnail size
    GtkWidget* size_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text((GtkComboBoxText*)size_combo, "160px");
    gtk_combo_box_text_append_text((GtkComboBoxText*)size_combo, "320px");
    gtk_combo_box_text_append_text((GtkComboBoxText*)size_combo, "640px");
    gtk_combo_box_text_append_text((GtkComboBoxText*)size_combo, "1024px");
    gtk_combo_box_set_active((GtkComboBox*)size_combo, 1);

    // Padding
    GtkWidget* hpadding_01 = gtk_event_box_new();
    gtk_widget_set_size_request((GtkWidget*)hpadding_01, 10, 20);

    // Search Criteria Frame: Column #2 - Thumbnail quality
    GtkWidget* quality_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text((GtkComboBoxText*)quality_combo, "High quality");
    gtk_combo_box_text_append_text((GtkComboBoxText*)quality_combo, "Low quality");
    gtk_combo_box_set_active((GtkComboBox*)quality_combo, 0);

    // Padding
    GtkWidget* hpadding_02 = gtk_event_box_new();
    gtk_widget_set_size_request((GtkWidget*)hpadding_02, 10, 20);

    // Search Criteria Frame: Column #3 - Search entry
    GtkWidget* query_entry = gtk_entry_new();
    gtk_widget_set_size_request((GtkWidget*)query_entry, 600, 30);

    // Padding
    GtkWidget* hpadding_03 = gtk_event_box_new();
    gtk_widget_set_size_request((GtkWidget*)hpadding_03, 10, 20);

    // Search Criteria Frame: Column #3 - Search entry
    GtkWidget* search_button = gtk_button_new_with_label("Run");
    g_signal_connect (search_button, "clicked", G_CALLBACK (search_buttom_clicked), ctx);
    gtk_widget_set_size_request((GtkWidget*)search_button, 50, 30);


    ctx->image_grid = gtk_grid_new();

    gtk_grid_set_column_spacing((GtkGrid*)ctx->image_grid, 25);
    gtk_grid_set_row_spacing((GtkGrid*)ctx->image_grid, 25);



    GtkWidget* scrolled_window = gtk_scrolled_window_new (NULL, NULL);

     gtk_widget_set_size_request(scrolled_window, SCREEN_WIDTH, SCREEN_HEIGHT );

 
    gtk_container_add (GTK_CONTAINER (scrolled_window), ctx->image_grid);


GtkWidget* top_level_grid = gtk_grid_new();



GtkWidget* vbox = gtk_vbox_new (FALSE, 0);

 GtkWidget* event_box = gtk_event_box_new ();
 gtk_widget_set_size_request((GtkWidget*)event_box, SCREEN_WIDTH, SCREEN_HEIGHT);
 

 GtkWidget* hbox = gtk_hbox_new (FALSE, 0);

 gtk_box_pack_start (GTK_BOX (hbox), size_combo, FALSE, FALSE, 0);

gtk_box_pack_start (GTK_BOX (hbox), hpadding_01, FALSE, FALSE, 0);


 gtk_box_pack_start (GTK_BOX (hbox), quality_combo, FALSE, FALSE, 0);  

 gtk_box_pack_start (GTK_BOX (hbox), hpadding_02, FALSE, FALSE, 0);
 gtk_box_pack_start (GTK_BOX (hbox), query_entry, FALSE, FALSE, 0);
 gtk_box_pack_start (GTK_BOX (hbox), hpadding_03, FALSE, FALSE, 0);
 gtk_box_pack_start (GTK_BOX (hbox), search_button, FALSE, FALSE, 0);

 GtkWidget* criterias_frame = gtk_frame_new ("Search criterias");

 GtkWidget* frame_padding = gtk_event_box_new();
 gtk_widget_set_size_request((GtkWidget*)frame_padding, 100, 10);

 GtkWidget* results_frame = gtk_frame_new ("Results");

 GtkWidget* vbox_criterias_frame = gtk_vbox_new (FALSE, 0);
 gtk_box_pack_start (GTK_BOX (vbox_criterias_frame), hbox, FALSE, FALSE, 0);
 gtk_container_set_border_width (GTK_CONTAINER (vbox_criterias_frame), 10);

 GtkWidget* vbox_results_frame = gtk_vbox_new (FALSE, 0);
 gtk_box_pack_start (GTK_BOX (vbox_results_frame), scrolled_window, FALSE, FALSE, 0);
 gtk_container_set_border_width (GTK_CONTAINER (vbox_results_frame), 10);

 gtk_container_add (GTK_CONTAINER (criterias_frame), vbox_criterias_frame);
 gtk_container_add (GTK_CONTAINER (results_frame), vbox_results_frame);




 gtk_box_pack_start (GTK_BOX (vbox), criterias_frame, FALSE, FALSE, 0);
 gtk_box_pack_start (GTK_BOX (vbox), frame_padding, FALSE, FALSE, 0);

 gtk_box_pack_start (GTK_BOX (vbox), results_frame, FALSE, FALSE, 0);

 gtk_container_set_border_width (GTK_CONTAINER (vbox), 20);

    return vbox;

}

GtkWidget* window::create_tag_page_container(gtk_ctx_t* ctx)
{
    GtkWidget* vbox = gtk_vbox_new (FALSE, 0);
//http://zetcode.com/gui/gtk2/gtktreeview/
    return vbox;
}

void window::activate(GtkApplication* app, gpointer user_data) // static: callback function
{
    gtk_ctx_t* ctx = (gtk_ctx_t*)user_data;
    assert(ctx);

    GtkWidget* window = gtk_application_window_new (app);
    assert(window);

    
 
    GtkWidget* search_page = gtk_label_new("Query");
    GtkWidget* tag_page = gtk_label_new("Available tags");

    GtkWidget* notebook = gtk_notebook_new();
gtk_notebook_append_page ((GtkNotebook*)notebook,
                          create_search_page_container(ctx),
                          search_page);

gtk_notebook_append_page ((GtkNotebook*)notebook,
                          create_tag_page_container(ctx),
                          tag_page);


    gtk_container_add (GTK_CONTAINER (window), notebook);



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