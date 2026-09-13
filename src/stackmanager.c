
#include "stackmanager.h"

#include <stdlib.h>
#include <string.h>


/* ============================================================
 * StackManager structure
 * ============================================================ */

struct StackManager
{
    GtkStack *stack;
};


/* ============================================================
 * Creation / destruction
 * ============================================================ */

StackManager *stack_manager_new(void)
{
    StackManager *manager = malloc(sizeof(StackManager));

    if (manager == NULL)
        return NULL;

    manager->stack = GTK_STACK(gtk_stack_new());

    /*
     * Default settings.
     *
     * Change these if you want different behavior.
     */
    gtk_stack_set_transition_type(
        manager->stack,
        GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT
    );

    gtk_stack_set_transition_duration(
        manager->stack,
        250
    );

    return manager;
}


void stack_manager_free(StackManager *manager)
{
    if (manager == NULL)
        return;

    /*
     * GtkStack is owned by the GTK widget hierarchy.
     * We only free our manager structure here.
     */
    free(manager);
}


/* ============================================================
 * Stack access
 * ============================================================ */

GtkStack *stack_manager_get_stack(StackManager *manager)
{
    if (manager == NULL)
        return NULL;

    return manager->stack;
}


/* ============================================================
 * Page management
 * ============================================================ */

void stack_manager_add(
    StackManager *manager,
    const char *name,
    GtkWidget *widget
)
{
    if (manager == NULL ||
        name == NULL ||
        widget == NULL)
    {
        return;
    }

    /*
     * If a page with this name already exists,
     * remove it first.
     *
     * This makes changing/replacing pages easier.
     */
    if (stack_manager_has(manager, name))
    {
        stack_manager_remove(manager, name);
    }

    gtk_stack_add_named(
        manager->stack,
        widget,
        name
    );
}


void stack_manager_remove(
    StackManager *manager,
    const char *name
)
{
    if (manager == NULL ||
        name == NULL)
    {
        return;
    }

    GtkWidget *widget =
        gtk_stack_get_child_by_name(
            manager->stack,
            name
        );

    if (widget == NULL)
        return;

    gtk_stack_remove(
        manager->stack,
        widget
    );
}


gboolean stack_manager_has(
    StackManager *manager,
    const char *name
)
{
    if (manager == NULL ||
        name == NULL)
    {
        return FALSE;
    }

    return gtk_stack_get_child_by_name(
        manager->stack,
        name
    ) != NULL;
}


/* ============================================================
 * Page navigation
 * ============================================================ */

void stack_manager_show(
    StackManager *manager,
    const char *name
)
{
    if (manager == NULL ||
        name == NULL)
    {
        return;
    }

    /*
     * Don't attempt to switch to a page that
     * doesn't exist.
     */
    if (!stack_manager_has(manager, name))
        return;

    gtk_stack_set_visible_child_name(
        manager->stack,
        name
    );
}


GtkWidget *stack_manager_get_current(
    StackManager *manager
)
{
    if (manager == NULL)
        return NULL;

    return gtk_stack_get_visible_child(
        manager->stack
    );
}


const char *stack_manager_get_current_name(
    StackManager *manager
)
{
    if (manager == NULL)
        return NULL;

    GtkWidget *current =
        gtk_stack_get_visible_child(
            manager->stack
        );

    if (current == NULL)
        return NULL;

    GtkStackPage *page =
        gtk_stack_get_page(
            manager->stack,
            current
        );

    if (page == NULL)
        return NULL;

    return gtk_stack_page_get_name(page);
}


/* ============================================================
 * Configuration
 * ============================================================ */

void stack_manager_set_transition(
    StackManager *manager,
    GtkStackTransitionType transition
)
{
    if (manager == NULL)
        return;

    gtk_stack_set_transition_type(
        manager->stack,
        transition
    );
}


void stack_manager_set_transition_duration(
    StackManager *manager,
    guint duration
)
{
    if (manager == NULL)
        return;

    gtk_stack_set_transition_duration(
        manager->stack,
        duration
    );
}
