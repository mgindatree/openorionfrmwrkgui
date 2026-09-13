
#include <gtk/gtk.h>
#include "stackmanager.h"

static void activate(GtkApplication *app, gpointer user_data)
{
    (void)user_data;

    GtkWidget *window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), "Hello");
    gtk_window_fullscreen(GTK_WINDOW(window));

    StackManager *stack = stack_manager_new();

    GtkWidget *page = gtk_button_new_with_label("check for updates");

    gtk_widget_set_halign(page, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(page, GTK_ALIGN_CENTER);

    stack_manager_add(stack, "main", page);
    stack_manager_show(stack, "main");

    gtk_window_set_child(
        GTK_WINDOW(window),
        GTK_WIDGET(stack_manager_get_stack(stack))
    );

    g_object_set_data_full(
        G_OBJECT(window),
        "stack-manager",
        stack,
        (GDestroyNotify)stack_manager_free
    );

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new(
        "org.gtk.example",
        G_APPLICATION_DEFAULT_FLAGS
    );

    g_signal_connect(
        app,
        "activate",
        G_CALLBACK(activate),
        NULL
    );

    int status = g_application_run(
        G_APPLICATION(app),
        argc,
        argv
    );

    g_object_unref(app);

    return status;
}

