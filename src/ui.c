#include "ui.h"
#include <string.h>
/* global stack pointer */
static GtkStack *g_stack = NULL;

void ui_init(GtkStack *stack)
{
    g_stack = stack;
}

/* direct navigation */
void ui_go(const char *page)
{
    if (!g_stack) return;

    gtk_stack_set_visible_child_name(g_stack, page);
}

/* button auto-routing */
static void on_route(GtkButton *btn, gpointer user_data)
{
    const char *page = (const char *)user_data;
    ui_go(page);
}

void ui_route(GtkWidget *button, const char *page)
{
    g_signal_connect(button, "clicked",
                     G_CALLBACK(on_route),
                     (gpointer)page);
}