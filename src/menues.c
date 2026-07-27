#include <gtk/gtk.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "menues.h"
#include "ui.h"

static login_callback saved_callback = NULL;
static image_device_callback saved_image_callback = NULL;

/*----------------------------------------------------------
 * CALLBACK REGISTRATION
 *----------------------------------------------------------*/
void register_callback(login_callback func)
{
    saved_callback = func;
}

void register_image_callback(image_device_callback func)
{
    saved_image_callback = func;
}

/*----------------------------------------------------------
 * USERNAME/PASSWORD VALIDATION
 *----------------------------------------------------------*/
bool is_valid_alphanumeric(const char *str)
{
    if (str == NULL || *str == '\0')
        return false;

    size_t len = strlen(str);

    if (len < 3 || len > 20)
        return false;

    while (*str)
    {
        if (!isalnum((unsigned char)*str))
            return false;

        str++;
    }

    return true;
}

/*----------------------------------------------------------
 * LOGIN DATA
 *----------------------------------------------------------*/
typedef struct
{
    GtkWidget *user;
    GtkWidget *pass;
} LoginData;

/*----------------------------------------------------------
 * IMAGE NEW DEVICE BUTTON
 *----------------------------------------------------------*/
static void imagehandler(GtkButton *button G_GNUC_UNUSED,
                         gpointer user_data)
{
    LoginData *data = user_data;

    const char *username =
        gtk_editable_get_text(GTK_EDITABLE(data->user));

    const char *password =
        gtk_editable_get_text(GTK_EDITABLE(data->pass));

    if (is_valid_alphanumeric(username) &&
        is_valid_alphanumeric(password))
    {
        if (saved_image_callback != NULL)
        {
            saved_image_callback(username, password);
        }
        else
        {
            g_print("Warning: No image_device_callback registered\n");
        }
    }
    else
    {
        g_print("Bad username/password format for device imaging.\n");
    }
}

/*----------------------------------------------------------
 * LOGIN BUTTON
 *----------------------------------------------------------*/
static void on_login_clicked(GtkButton *btn G_GNUC_UNUSED,
                             gpointer user_data)
{
    LoginData *data = (LoginData *)user_data;

    const char *username =
        gtk_editable_get_text(GTK_EDITABLE(data->user));

    const char *password =
        gtk_editable_get_text(GTK_EDITABLE(data->pass));

    if (is_valid_alphanumeric(username) &&
        is_valid_alphanumeric(password))
    {
        if (saved_callback != NULL)
        {
            saved_callback((password_pair){
                .username = username,
                .password = password
            });
        }
    }
    else
    {
        g_print("Bad username/password format.\n");
    }
}

/*----------------------------------------------------------
 * LOGIN PAGE
 *----------------------------------------------------------*/
static GtkWidget *create_login(void)
{
    GtkWidget *overlay = gtk_overlay_new();

    GtkWidget *bg =
        gtk_picture_new_for_filename("assets/login.png");

    gtk_overlay_set_child(GTK_OVERLAY(overlay), bg);

    GtkWidget *box =
        gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    GtkWidget *user = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(user), "Username");

    GtkWidget *pass = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(pass), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(pass), FALSE);

    GtkWidget *login =
        gtk_button_new_with_label("Login");

    GtkWidget *imageNewDevice =
        gtk_button_new_with_label("create New login");

    LoginData *data = g_malloc(sizeof(LoginData));

    data->user = user;
    data->pass = pass;

    g_signal_connect(login,
                     "clicked",
                     G_CALLBACK(on_login_clicked),
                     data);

    g_signal_connect(imageNewDevice,
                     "clicked",
                     G_CALLBACK(imagehandler),
                     data);

    gtk_box_append(GTK_BOX(box), user);
    gtk_box_append(GTK_BOX(box), pass);
    gtk_box_append(GTK_BOX(box), login);
    gtk_box_append(GTK_BOX(box), imageNewDevice);

    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), box);

    return overlay;
}

/*----------------------------------------------------------
 * MAIN MENU
 *----------------------------------------------------------*/
static GtkWidget *create_main(void)
{
    GtkWidget *box =
        gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *label =
        gtk_label_new("Main Menu");

    GtkWidget *settings =
        gtk_button_new_with_label("Settings");

    GtkWidget *logout =
        gtk_button_new_with_label("Logout");

    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), settings);
    gtk_box_append(GTK_BOX(box), logout);

    ui_route(settings, "settings");
    ui_route(logout, "login");

    return box;
}

/*----------------------------------------------------------
 * SETTINGS PAGE
 *----------------------------------------------------------*/
static GtkWidget *create_settings(void)
{
    GtkWidget *box =
        gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *label =
        gtk_label_new("Settings");

    GtkWidget *back =
        gtk_button_new_with_label("Back");

    gtk_box_append(GTK_BOX(box), label);
    gtk_box_append(GTK_BOX(box), back);

    ui_route(back, "main");

    return box;
}

/*----------------------------------------------------------
 * BUILD STACK
 *----------------------------------------------------------*/
GtkWidget *menus_create(GtkApplication *app G_GNUC_UNUSED)
{
    GtkWidget *stack = gtk_stack_new();

    gtk_stack_set_transition_type(
        GTK_STACK(stack),
        GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);

    gtk_stack_set_transition_duration(
        GTK_STACK(stack),
        250);

    ui_init(GTK_STACK(stack));

    GtkWidget *login = create_login();
    GtkWidget *main_menu = create_main();
    GtkWidget *settings = create_settings();

    gtk_stack_add_named(GTK_STACK(stack), login, "login");
    gtk_stack_add_named(GTK_STACK(stack), main_menu, "main");
    gtk_stack_add_named(GTK_STACK(stack), settings, "settings");

    ui_go("login");

    return stack;
}