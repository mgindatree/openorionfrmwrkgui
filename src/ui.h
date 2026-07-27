#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

void ui_init(GtkStack *stack);
void ui_route(GtkWidget *button, const char *page);
void ui_go(const char *page);

#endif