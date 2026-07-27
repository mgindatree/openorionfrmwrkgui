#include <gtk/gtk.h>
#include <sodium.h>
#include "menues.h"
#include <libusb-1.0/libusb.h>
static char stored_hash[crypto_pwhash_STRBYTES];

static char stored_username[256];
void testpassword(password_pair passandusr)   //reguler log in
{
    if (strcmp(passandusr.username, stored_username) != 0)
    {
        g_print("Unknown username.\n");
        return;
    }

    if (crypto_pwhash_str_verify(
            stored_hash,
            passandusr.password,
            strlen(passandusr.password)) == 0)
    {
        g_print("Login successful!\n");
        
    }
    else
    {
        g_print("Incorrect password.\n");
    }
}
void test_image_device(const char *username, const char *password)   //create login
{
    if (stored_hash[0] != '\0' && stored_username[0] != '\0') {
        g_print("Device already registered. Cannot create new login.\n");
        return;
    }
    if (crypto_pwhash_str(
            stored_hash,
            password,
            strlen(password),
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0)
    {
        g_print("Failed to hash password.\n");
        return;
    }

    g_strlcpy(stored_username, username, sizeof(stored_username));

    g_print("New device registered.\n");
    g_print("Username: %s\n", stored_username);
    g_print("Password hash:\n%s\n", stored_hash);
}
static void activate(GtkApplication *app)
{
    GtkWidget *window = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(window), "chicken eyes");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    register_image_callback(test_image_device);
    register_callback(testpassword);
    GtkWidget *stack = menus_create(app);

    gtk_window_set_child(GTK_WINDOW(window), stack);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    if (sodium_init() == -1) {
        return 1;
    }
    GtkApplication *app =
        gtk_application_new("com.chickeneyes.ui", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    return g_application_run(G_APPLICATION(app), argc, argv);
}