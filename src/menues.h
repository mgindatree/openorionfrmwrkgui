#ifndef MENUS_H
#define MENUS_H

#include <gtk/gtk.h>

typedef struct
{
    const char *username;
    const char *password;
} password_pair;

/* Normal login callback */
typedef void (*login_callback)(password_pair passandusr);

/* Dedicated callback for imaging a new special device */
typedef void (*image_device_callback)(const char *username, const char *password);

void register_callback(login_callback func);
void register_image_callback(image_device_callback func);

void trigger_library_event(void);
GtkWidget *menus_create(GtkApplication *app);

#endif