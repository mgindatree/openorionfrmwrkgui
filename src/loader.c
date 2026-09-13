#include "loader.h"

#include <dlfcn.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ============================================================
 * Mod function types
 * ============================================================ */

typedef void (*ModInitFunc)(
    ModAPI *api
);


typedef void (*ModCleanupFunc)(
    void
);


/* ============================================================
 * Loaded mod
 * ============================================================ */

typedef struct
{
    void *handle;

    ModInitFunc init;
    ModCleanupFunc cleanup;

    char *name;

} LoadedMod;


/* ============================================================
 * Loader state
 * ============================================================ */

static LoadedMod *loaded_mods = NULL;

static size_t loaded_mod_count = 0;

static StackManager *loader_stack = NULL;


/* ============================================================
 * addpage()
 * ============================================================ */

static void mod_addpage(
    ModAPI *api,
    const char *name,
    GtkWidget *widget
)
{
    if (api == NULL)
        return;

    if (api->stack == NULL)
        return;

    if (name == NULL)
        return;

    if (widget == NULL)
        return;


    stack_manager_add(
        api->stack,
        name,
        widget
    );
}


/* ============================================================
 * showpage()
 * ============================================================ */

static void mod_showpage(
    ModAPI *api,
    const char *name
)
{
    if (api == NULL)
        return;

    if (api->stack == NULL)
        return;

    if (name == NULL)
        return;


    stack_manager_show(
        api->stack,
        name
    );
}


/* ============================================================
 * Check for .so
 * ============================================================ */

static gboolean is_shared_library(
    const char *filename
)
{
    if (filename == NULL)
        return FALSE;


    size_t length =
        strlen(filename);


    if (length < 4)
        return FALSE;


    return strcmp(
        filename + length - 3,
        ".so"
    ) == 0;
}


/* ============================================================
 * Find loaded mod
 * ============================================================ */

static LoadedMod *find_mod(
    const char *name
)
{
    if (name == NULL)
        return NULL;


    for (size_t i = 0;
         i < loaded_mod_count;
         i++)
    {
        if (strcmp(
                loaded_mods[i].name,
                name
            ) == 0)
        {
            return &loaded_mods[i];
        }
    }


    return NULL;
}


/* ============================================================
 * Store loaded mod
 * ============================================================ */

static gboolean store_mod(
    void *handle,
    ModInitFunc init,
    ModCleanupFunc cleanup,
    const char *name
)
{
    LoadedMod *new_mods =
        realloc(
            loaded_mods,
            sizeof(LoadedMod) *
            (loaded_mod_count + 1)
        );


    if (new_mods == NULL)
        return FALSE;


    loaded_mods = new_mods;


    LoadedMod *mod =
        &loaded_mods[loaded_mod_count];


    mod->handle = handle;

    mod->init = init;

    mod->cleanup = cleanup;


    mod->name =
        strdup(name);


    if (mod->name == NULL)
        return FALSE;


    loaded_mod_count++;


    return TRUE;
}


/* ============================================================
 * Load one mod
 * ============================================================ */

static void load_mod(
    const char *path,
    const char *filename,
    StackManager *stack
)
{
    printf(
        "Loading mod: %s\n",
        filename
    );


    /*
     * Don't load the same mod twice.
     */

    if (find_mod(filename) != NULL)
    {
        printf(
            "Mod already loaded: %s\n",
            filename
        );

        return;
    }


    /*
     * Open shared library.
     */

    void *handle =
        dlopen(
            path,
            RTLD_NOW
        );


    if (handle == NULL)
    {
        fprintf(
            stderr,
            "Failed to load mod %s: %s\n",
            filename,
            dlerror()
        );

        return;
    }


    /*
     * Find mod_init().
     */

    dlerror();


    ModInitFunc init =
        (ModInitFunc)dlsym(
            handle,
            "mod_init"
        );


    const char *error =
        dlerror();


    if (error != NULL ||
        init == NULL)
    {
        fprintf(
            stderr,
            "Mod %s does not contain mod_init()\n",
            filename
        );

        dlclose(handle);

        return;
    }


    /*
     * Find optional mod_cleanup().
     */

    dlerror();


    ModCleanupFunc cleanup =
        (ModCleanupFunc)dlsym(
            handle,
            "mod_cleanup"
        );


    dlerror();


    /*
     * Create API for the mod.
     */

    ModAPI api;

    api.stack = stack;

    api.addpage =
        mod_addpage;

    api.showpage =
        mod_showpage;


    init(&api);


    if (!store_mod(
            handle,
            init,
            cleanup,
            filename))
    {
        fprintf(
            stderr,
            "Could not store mod: %s\n",
            filename
        );


        if (cleanup != NULL)
            cleanup();


        dlclose(handle);

        return;
    }


    printf(
        "Loaded mod: %s\n",
        filename
    );
}


/* ============================================================
 * Load all mods
 * ============================================================ */

void loader_load_mods(
    const char *directory,
    StackManager *stack
)
{
    if (directory == NULL)
        return;


    if (stack == NULL)
        return;


    loader_stack = stack;


    DIR *dir =
        opendir(directory);


    if (dir == NULL)
    {
        fprintf(
            stderr,
            "Could not open mod directory: %s\n",
            directory
        );

        return;
    }


    struct dirent *entry;


    while ((entry = readdir(dir)) != NULL)
    {
        const char *filename =
            entry->d_name;


        /*
         * Ignore "." and "..".
         */

        if (strcmp(filename, ".") == 0 ||
            strcmp(filename, "..") == 0)
        {
            continue;
        }


        /*
         * Only load shared libraries.
         */

        if (!is_shared_library(filename))
            continue;


        /*
         * Build complete path.
         */

        size_t path_length =
            strlen(directory) +
            strlen(filename) +
            2;


        char *path =
            malloc(path_length);


        if (path == NULL)
        {
            fprintf(
                stderr,
                "Out of memory loading %s\n",
                filename
            );

            continue;
        }


        snprintf(
            path,
            path_length,
            "%s/%s",
            directory,
            filename
        );


        load_mod(
            path,
            filename,
            stack
        );


        free(path);
    }


    closedir(dir);
}


/* ============================================================
 * Show mod
 * ============================================================ */

gboolean loader_show_mod(
    const char *name
)
{
    if (name == NULL)
        return FALSE;


    if (loader_stack == NULL)
        return FALSE;


    /*
     * First look for the exact mod name.
     */

    LoadedMod *mod =
        find_mod(name);


    if (mod == NULL)
    {
        /*
         * Also allow selecting a mod
         * without the ".so" extension.
         */

        size_t name_length =
            strlen(name);


        char *filename =
            malloc(name_length + 4);


        if (filename == NULL)
            return FALSE;


        snprintf(
            filename,
            name_length + 4,
            "%s.so",
            name
        );


        mod =
            find_mod(filename);


        free(filename);


        if (mod == NULL)
            return FALSE;
    }


    /*
     * The mod itself can have a page
     * with the same name as the mod.
     */

    if (stack_manager_has(
            loader_stack,
            mod->name))
    {
        stack_manager_show(
            loader_stack,
            mod->name
        );

        return TRUE;
    }


    /*
     * If the mod was named:
     *
     *     calculator.so
     *
     * try:
     *
     *     calculator
     */

    const char *dot =
        strrchr(
            mod->name,
            '.'
        );


    if (dot != NULL)
    {
        size_t length =
            (size_t)(dot - mod->name);


        char *page_name =
            malloc(length + 1);


        if (page_name == NULL)
            return FALSE;


        memcpy(
            page_name,
            mod->name,
            length
        );


        page_name[length] =
            '\0';


        gboolean exists =
            stack_manager_has(
                loader_stack,
                page_name
            );


        if (exists)
        {
            stack_manager_show(
                loader_stack,
                page_name
            );
        }


        free(page_name);


        if (exists)
            return TRUE;
    }


    fprintf(
        stderr,
        "Mod loaded but no page found for: %s\n",
        name
    );


    return FALSE;
}


/* ============================================================
 * Has mod
 * ============================================================ */

gboolean loader_has_mod(
    const char *name
)
{
    if (name == NULL)
        return FALSE;


    if (find_mod(name) != NULL)
        return TRUE;


    size_t length =
        strlen(name);


    char *filename =
        malloc(length + 4);


    if (filename == NULL)
        return FALSE;


    snprintf(
        filename,
        length + 4,
        "%s.so",
        name
    );


    gboolean result =
        find_mod(filename) != NULL;


    free(filename);


    return result;
}


/* ============================================================
 * Get mod count
 * ============================================================ */

size_t loader_get_mod_count(void)
{
    return loaded_mod_count;
}


/* ============================================================
 * Get mod name
 * ============================================================ */

const char *loader_get_mod_name(
    size_t index
)
{
    if (index >= loaded_mod_count)
        return NULL;


    return loaded_mods[index].name;
}


/* ============================================================
 * Get StackManager
 * ============================================================ */

StackManager *loader_get_stack(void)
{
    return loader_stack;
}


/* ============================================================
 * Unload mods
 * ============================================================ */

void loader_unload_mods(void)
{
    /*
     * Unload in reverse order.
     */

    for (size_t i = loaded_mod_count;
         i > 0;
         i--)
    {
        LoadedMod *mod =
            &loaded_mods[i - 1];


        /*
         * Give mod a chance to clean up.
         */

        if (mod->cleanup != NULL)
        {
            mod->cleanup();
        }


        /*
         * Close shared library.
         */

        if (mod->handle != NULL)
        {
            dlclose(
                mod->handle
            );
        }


        free(
            mod->name
        );
    }


    free(
        loaded_mods
    );


    loaded_mods = NULL;

    loaded_mod_count = 0;

    loader_stack = NULL;
}