
#include <gtk/gtk.h>

#include "stackmanager.h"
#include "loader.h"


static void mod_button_clicked(
    GtkButton *button,
    gpointer user_data
)
{
    (void)button;

    const char *mod_name = user_data;

    if (mod_name != NULL)
        loader_show_mod(mod_name);
}


static void activate(
    GtkApplication *app,
    gpointer user_data
)
{
    (void)user_data;

    GtkWidget *window =
        gtk_application_window_new(app);

    gtk_window_set_title(
        GTK_WINDOW(window),
        "Hello"
    );

    gtk_window_fullscreen(
        GTK_WINDOW(window)
    );


    StackManager *stack =
        stack_manager_new();

    if (stack == NULL)
    {
        gtk_window_destroy(GTK_WINDOW(window));
        return;
    }


    /*
     * Background overlay.
     *
     * The background fills the entire page.
     * The main_box is placed on top of it.
     */

    GtkWidget *background_overlay =
        gtk_overlay_new();


    GtkWidget *background =
        gtk_picture_new_for_filename(
            "assets/startimg.png"
        );

    gtk_picture_set_content_fit(
        GTK_PICTURE(background),
        GTK_CONTENT_FIT_COVER
    );

    gtk_widget_set_hexpand(
        background,
        TRUE
    );

    gtk_widget_set_vexpand(
        background,
        TRUE
    );


    gtk_overlay_set_child(
        GTK_OVERLAY(background_overlay),
        background
    );


    /*
     * Main page contents.
     */

    GtkWidget *main_box =
        gtk_box_new(
            GTK_ORIENTATION_VERTICAL,
            10
        );

    gtk_widget_set_halign(
        main_box,
        GTK_ALIGN_CENTER
    );

    gtk_widget_set_valign(
        main_box,
        GTK_ALIGN_CENTER
    );


    GtkWidget *update_button =
        gtk_button_new_with_label(
            "check for updates"
        );

    gtk_box_append(
        GTK_BOX(main_box),
        update_button
    );


    /*
     * Put the controls over the background.
     */

    gtk_overlay_add_overlay(
        GTK_OVERLAY(background_overlay),
        main_box
    );


    /*
     * Add the complete overlay page to
     * the stack, not just main_box.
     */

    stack_manager_add(
        stack,
        "main",
        background_overlay
    );


    /*
     * Load all mods from the mods directory.
     *
     * This loads their pages but does not
     * automatically display them.
     */

    loader_load_mods(
        "mods",
        stack
    );


    /*
     * Create a button for every loaded mod.
     */

    size_t mod_count =
        loader_get_mod_count();


    for (size_t i = 0;
         i < mod_count;
         i++)
    {
        const char *mod_name =
            loader_get_mod_name(i);

        if (mod_name == NULL)
            continue;


        GtkWidget *button =
            gtk_button_new_with_label(
                mod_name
            );


        /*
         * Make a copy of the mod name.
         *
         * The copy remains alive until the
         * button's signal connection is destroyed.
         */

        char *name_copy =
            g_strdup(mod_name);


        g_signal_connect_data(
            button,
            "clicked",
            G_CALLBACK(mod_button_clicked),
            name_copy,
            (GClosureNotify)g_free,
            0
        );


        gtk_box_append(
            GTK_BOX(main_box),
            button
        );
    }


    /*
     * Start on the main page.
     */

    stack_manager_show(
        stack,
        "main"
    );


    /*
     * Put the stack into the window.
     */

    gtk_window_set_child(
        GTK_WINDOW(window),
        GTK_WIDGET(
            stack_manager_get_stack(stack)
        )
    );


    /*
     * Free the StackManager when the
     * window is destroyed.
     */

    g_object_set_data_full(
        G_OBJECT(window),
        "stack-manager",
        stack,
        (GDestroyNotify)stack_manager_free
    );


    gtk_window_present(
        GTK_WINDOW(window)
    );
}


int main(
    int argc,
    char **argv
)
{
    GtkApplication *app =
        gtk_application_new(
            "org.gtk.example",
            G_APPLICATION_DEFAULT_FLAGS
        );


    g_signal_connect(
        app,
        "activate",
        G_CALLBACK(activate),
        NULL
    );


    int status =
        g_application_run(
            G_APPLICATION(app),
            argc,
            argv
        );


    g_object_unref(app);


    return status;
}

