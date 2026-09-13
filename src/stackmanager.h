
#ifndef STACKMANAGER_H
#define STACKMANAGER_H

#include <gtk/gtk.h>

/*
 * StackManager
 *
 * Generic manager for a GTK4 GtkStack.
 *
 * The application decides what each page contains.
 * The manager only handles creating, adding, removing,
 * and switching between pages.
 */

typedef struct StackManager StackManager;


/* ============================================================
 * Creation / destruction
 * ============================================================ */

/* Create a new StackManager */
StackManager *stack_manager_new(void);

/* Destroy the StackManager */
void stack_manager_free(StackManager *manager);


/* ============================================================
 * Stack access
 * ============================================================ */

/* Get the underlying GtkStack */
GtkStack *stack_manager_get_stack(StackManager *manager);


/* ============================================================
 * Page management
 * ============================================================ */

/*
 * Add a widget to the stack.
 *
 * name:
 *     Unique name used to identify the page.
 *
 * widget:
 *     Widget that will become the page.
 */
void stack_manager_add(
    StackManager *manager,
    const char *name,
    GtkWidget *widget
);


/* Remove a page by name */
void stack_manager_remove(
    StackManager *manager,
    const char *name
);


/* Check whether a page exists */
gboolean stack_manager_has(
    StackManager *manager,
    const char *name
);


/* ============================================================
 * Page navigation
 * ============================================================ */

/* Show a page by name */
void stack_manager_show(
    StackManager *manager,
    const char *name
);


/* Get the currently visible page */
GtkWidget *stack_manager_get_current(
    StackManager *manager
);


/* Get the name of the currently visible page */
const char *stack_manager_get_current_name(
    StackManager *manager
);


/* ============================================================
 * Configuration
 * ============================================================ */

/* Set the stack transition animation */
void stack_manager_set_transition(
    StackManager *manager,
    GtkStackTransitionType transition
);


/* Set transition animation duration in milliseconds */
void stack_manager_set_transition_duration(
    StackManager *manager,
    guint duration
);

#endif
