#ifndef LOADER_H
#define LOADER_H

#include <gtk/gtk.h>

#include "stackmanager.h"


/* ============================================================
 * Mod API
 * ============================================================ */

typedef struct ModAPI ModAPI;


/*
 * Add a GTK page to the application's stack.
 */
typedef void (*ModAddPageFunc)(
    ModAPI *api,
    const char *name,
    GtkWidget *widget
);


/*
 * Show a page in the application's stack.
 */
typedef void (*ModShowPageFunc)(
    ModAPI *api,
    const char *name
);


/*
 * API passed to every mod.
 */
struct ModAPI
{
    StackManager *stack;

    ModAddPageFunc addpage;
    ModShowPageFunc showpage;
};


/* ============================================================
 * Loader
 * ============================================================ */


/*
 * Load all .so files from a directory.
 *
 * Example:
 *
 *     loader_load_mods("mods", stack);
 *
 * Mods are loaded but their pages are NOT automatically shown.
 */
void loader_load_mods(
    const char *directory,
    StackManager *stack
);


/*
 * Show a page belonging to a loaded mod.
 *
 * Example:
 *
 *     loader_show_mod("calculator");
 */
gboolean loader_show_mod(
    const char *name
);


/*
 * Check whether a mod exists.
 */
gboolean loader_has_mod(
    const char *name
);


/*
 * Get the number of loaded mods.
 */
size_t loader_get_mod_count(void);


/*
 * Get the name of a loaded mod.
 *
 * Index starts at 0.
 *
 * Returns NULL if the index is invalid.
 */
const char *loader_get_mod_name(
    size_t index
);


/*
 * Get the StackManager used by the loader.
 */
StackManager *loader_get_stack(void);


/*
 * Unload all mods.
 */
void loader_unload_mods(void);


#endif